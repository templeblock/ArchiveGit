@echo off

set SRC=N:\DEV\CRAYOLA\PAINT
echo Checking src...
vlog -r -q -bl%2 %SRC%\src\pvcs\*.*
echo Checking res...
vlog -r -q -bl%2 %SRC%\res\pvcs\*.*

set SRC=
goto End

:BadArgs
echo usage: %0 [username]
:End
