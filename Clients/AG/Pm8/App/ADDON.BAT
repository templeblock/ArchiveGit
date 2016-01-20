@echo off
echo PrintMaster addon control
echo .
if %1x == onx goto ON
if %1x == ONx goto ON
if %1x == offx goto OFF
if %1x == OFFx goto OFF
goto USAGE
:ON
echo National Geographic 
echo Y|copy aongia.ini addon.ini
echo Y|copy aongia.ini .\windebug\addon.ini
echo Y|copy aongia.ini .\winrel\addon.ini
goto BYE
:OFF
del addon.ini
del .\windebug\addon.ini
del .\winrel\addon.ini 
goto BYE
:USAGE
echo USAGE: %0 [on, a or b] or %0 off
:BYE
echo DOS lives!
