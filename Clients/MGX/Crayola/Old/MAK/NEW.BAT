@echo off

if '%1' == '' goto BadArgs
set VERSION=%2
if '%2' == '' set VERSION=-r

set SRC=N:\DEV\CRAYOLA\PAINT
cd %1
set SUB=src
cd %SUB% | get %VERSION% -u %SRC%\%SUB%\PVCS\*.??V | cd ..
set SUB=res
cd %SUB% | get %VERSION% -u %SRC%\%SUB%\PVCS\*.??V | cd ..

set SUB=
set SRC=
goto End

:BadArgs
echo usage: %0 [destination directory]
:End
