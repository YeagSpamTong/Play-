@echo off
mkdir build_debugger_vs2019
pushd build_debugger_vs2019
cmake ../.. -G "Visual Studio 16 2019" -A x64 -DUSE_QT=on -DBUILD_PSFPLAYER=on -DDEBUGGER_INCLUDED=on -DCMAKE_PREFIX_PATH="C:\Qt\5.15.2\msvc2019_64"
popd
