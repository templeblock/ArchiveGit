@echo off

echo.
choice "Do you want to make the IE CAB file?" YN
if errorlevel 2 goto :SkipMakeCab
echo.
cd IE
call MakeCab.bat
cd ..

:SkipMakeCab

echo.
choice "Do you want to check in the IE CAB file?" YN
if errorlevel 2 goto :SkipCheckinCab
echo.
call Checkin.bat IE Crusher.cab

:SkipCheckinCab

echo.
echo.
pause
:end
