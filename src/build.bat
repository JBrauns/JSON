@echo off

set COMMON_COMPILER_FLAGS=-nologo -EHsc -Zi -DJSON_DEV_MODE
set COMMON_LINKER_FLAGS=user32.lib

IF NOT EXIST ..\build mkdir ..\build

pushd ..\build
cl %COMMON_COMPILER_FLAGS% ..\src\json_test.cpp /link  %COMMON_LINKER_FLAGS%
popd

IF NOT EXIST ..\data mkdir ..\data

pushd ..\data
del test_aa.*
python ..\src\test_gen.py
cl %COMMON_COMPILER_FLAGS% test_aa.cpp -I..\src /link  %COMMON_LINKER_FLAGS%
test_aa.exe
popd
