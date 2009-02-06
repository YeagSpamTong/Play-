#include "Iop_Loadcore.h"
#include "Iop_Dynamic.h"
#include "IopBios.h"
#include "../Log.h"

using namespace Iop;
using namespace std;

#define LOG_NAME "iop_loadcore"

#define FUNCTION_REGISTERLIBRARYENTRIES "RegisterLibraryEntries"

CLoadcore::CLoadcore(CIopBios& bios, uint8* ram, CSifMan& sifMan) :
m_bios(bios),
m_ram(ram)
{
    sifMan.RegisterModule(MODULE_ID, this);
}

CLoadcore::~CLoadcore()
{

}

string CLoadcore::GetId() const
{
    return "loadcore";
}

string CLoadcore::GetFunctionName(unsigned int functionId) const
{
    switch(functionId)
    {
    case 6:
        return FUNCTION_REGISTERLIBRARYENTRIES;
        break;
    default:
	    return "unknown";
        break;
    }
}

void CLoadcore::Invoke(CMIPS& context, unsigned int functionId)
{
    switch(functionId)
    {
    case 6:
        context.m_State.nGPR[CMIPS::V0].nD0 = static_cast<int32>(RegisterLibraryEntries(
            reinterpret_cast<uint32*>(&m_ram[context.m_State.nGPR[CMIPS::A0].nV0])
            ));
        break;
    default:
        CLog::GetInstance().Print(LOG_NAME, "Unknown function (%d) called (PC: 0x%0.8X).\r\n", 
            functionId, context.m_State.nPC);
        break;
    }
}

bool CLoadcore::Invoke(uint32 method, uint32* args, uint32 argsSize, uint32* ret, uint32 retSize, uint8* ram)
{
	switch(method)
	{
	case 0x00:
		LoadModule(args, argsSize, ret, retSize);
		break;
    case 0x06:
        LoadModuleFromMemory(args, argsSize, ret, retSize);
        break;
	case 0xFF:
		//This is sometimes called after binding this server with a client
		Initialize(args, argsSize, ret, retSize);
		break;
	default:
        CLog::GetInstance().Print(LOG_NAME, "Invoking unknown function %d.\r\n", method);
		break;
	}
    return true;
}

uint32 CLoadcore::RegisterLibraryEntries(uint32* exportTable)
{
#ifdef _DEBUG
    CLog::GetInstance().Print(LOG_NAME, FUNCTION_REGISTERLIBRARYENTRIES "(...);\r\n");
#endif
    m_bios.RegisterDynamicModule(new CDynamic(exportTable));
    return 0;    
}

void CLoadcore::LoadModule(uint32* args, uint32 argsSize, uint32* ret, uint32 retSize)
{
	char sModuleName[253];

	assert(argsSize == 512);

	//Sometimes called with 4, sometimes 8
	assert(retSize >= 4);

	memset(sModuleName, 0, 253);
	strncpy(sModuleName, &(reinterpret_cast<const char*>(args))[8], 252);

	//Load the module???
	CLog::GetInstance().Print(LOG_NAME, "Request to load module '%s' received.\r\n", sModuleName);

    m_bios.LoadAndStartModule(sModuleName, NULL, 0);

	//This function returns something negative upon failure
	ret[0] = 0x00000000;
}

void CLoadcore::LoadModuleFromMemory(uint32* args, uint32 argsSize, uint32* ret, uint32 retSize)
{
    m_bios.LoadAndStartModule(args[0], NULL, 0);
    ret[0] = 0x00000000;
}

void CLoadcore::Initialize(uint32* args, uint32 argsSize, uint32* ret, uint32 retSize)
{
	assert(argsSize == 0);
	assert(retSize == 4);

	ret[0] = 0x2E2E2E2E;
}
