@echo off
rem The following variables should be double quoted 
set file=%1
set name=%2
set url=%3
if  '%file%' == '' goto usage
if  '%name%' == '' goto usage
if  '%url%'  == '' goto usage
if not '%4%' == '' goto usage
if exist %file% goto fileOK
echo .
echo File %file% does not exist
echo .
goto end

:fileOK
call "C:\My Projects\AgApplications\CodeSigning\SignCode.exe" -n %name% -i %url% -spc "C:\My Projects\AgApplications\CodeSigning\MyCredentials.spc" -v "C:\My Projects\AgApplications\CodeSigning\MyPrivateKey.pvk" -t http://timestamp.verisign.com/scripts/timstamp.dll %file%
call "C:\My Projects\AgApplications\CodeSigning\ChkTrust.exe" -q %file%
goto end

:usage
echo This batch file takes 3 parameters, all of which must be double-quoted
echo .
echo Usage: SIGN "File" "Name" "Url"
echo .
echo       "File" is the name of the file that will be signed
echo       "Name" is the friendly name of the application and is 
echo              presented to the user in the security warning
echo       "Url"  is the url which is visited if the user clicks
echo              on the friendly name in the security warning

:end
pause
