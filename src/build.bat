@echo off

set COMMON_COMPILER_FLAGS=-nologo -EHsc -Zi -DJSON_DEV_MODE
set COMMON_LINKER_FLAGS=user32.lib

IF NOT EXIST ..\build mkdir ..\build

pushd ..\build
cl %COMMON_COMPILER_FLAGS% ..\src\json_test.cpp /link  %COMMON_LINKER_FLAGS%
popd

IF NOT EXIST ..\data mkdir ..\data

pushd ..\data
..\build\json_test.exe

REM del test_aa.*
REM python ..\src\test_gen.py
REM cl %COMMON_COMPILER_FLAGS% test_aa.cpp -I..\src /link  %COMMON_LINKER_FLAGS%
REM test_aa.exe
popd
