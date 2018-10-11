@echo off

set COMMON_COMPILER_FLAGS=-nologo -EHsc -Zi
set COMMON_LINKER_FLAGS=user32.lib

IF NOT EXIST ..\build mkdir ..\build

pushd ..\build
cl %COMMON_COMPILER_FLAGS% ..\src\json_test.cpp /link  %COMMON_LINKER_FLAGS%
popd

IF NOT EXIST ..\data mkdir ..\data

pushd ..\data
..\build\json_test.exe
popd
