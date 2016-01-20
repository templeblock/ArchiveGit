@echo off
set file1="..\..\release\Crusher.dll"
set file2="..\..\..\3rd party\ssce\runtime\lex\ssce5332.dll"
set file3="..\..\..\3rd party\ssce\runtime\lex\ssceam.tlx"
set file4="..\..\..\3rd party\ssce\runtime\lex\ssceam2.clx"
set file5="..\..\..\3rd party\ssce\runtime\lex\correct.tlx"
set filex=Crusher.inf
set outfile=Crusher.cab
if exist %outfile% del %outfile%

if exist %file1% goto file1OK
echo File '%file1%' does not exist
goto end
:file1OK

if exist %file2% goto file2OK
echo File '%file2%' does not exist
goto end
:file2OK

if exist %file3% goto file3OK
echo File '%file3%' does not exist
goto end
:file3OK

if exist %file4% goto file4OK
echo File '%file4%' does not exist
goto end
:file4OK

if exist %file5% goto file5OK
echo File '%file5%' does not exist
goto end
:file5OK

rem call CabArc -s 6144 n %outfile% %file1% %file2% %file3% %file4% %file5% %filex%
call CabArc -s 6144 n %outfile% %file1% %filex%
call "C:\My Projects\AgApplications\CodeSigning\Sign.bat" %outfile% "Spell Check and Photo eCards" "http://www.americangreetings.com"

:end
