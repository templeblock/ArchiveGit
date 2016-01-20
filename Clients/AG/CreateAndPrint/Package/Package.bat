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
choice "Do you want to make the Netscape JAR file?" YN
if errorlevel 2 goto :SkipMakeJar
echo.
cd Netscape
call MakeJar.bat
cd ..

:SkipMakeJar

echo.
choice "Do you want to make the Netscape Popup JAR file?" YN
if errorlevel 2 goto :SkipMakePopupJar
echo.
cd Netscape
call MakePopupJar.bat
cd ..

:SkipMakePopupJar

echo.
choice "Do you want to check in the IE CAB file?" YN
if errorlevel 2 goto :SkipCheckinCab
echo.
call Checkin.bat IE AxCtp2.cab

:SkipCheckinCab

echo.
choice "Do you want to check in the Netscape JAR file?" YN
if errorlevel 2 goto :SkipCheckinJar
echo.
call Checkin.bat Netscape NpCtp.jar

:SkipCheckinJar

echo.
choice "Do you want to check in the Netscape Popup JAR file?" YN
if errorlevel 2 goto :SkipCheckinPopupJar
echo.
call Checkin.bat Netscape NpPopup.jar

:SkipCheckinPopupJar

echo.
echo.
pause
:end
