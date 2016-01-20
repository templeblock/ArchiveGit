@echo off

set outfile="C:\My Projects\AgApplications\CreateAndPrint\Setup\CreateAndPrintPluginSetup2\Product\SingleImage\DiskImages\DISK1\CreateAndPrintPluginSetup2.exe"

if exist %outfile% goto fileOK
echo Executable file '%outfile%' does not exist
goto end

:fileOK
call "C:\My Projects\AgApplications\CodeSigning\Sign.bat" %outfile% "The Create_&_Print_plugin" "http://www.americangreetings.com/cnp"

:end
pause