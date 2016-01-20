@echo off
rem
rem I use this when building libraries with the NT librarian.
rem My command line is : ntlib lib-name @cmd-file
rem
if exist %1 goto exists
echo on
lib /out:%1 %2
@echo off
goto exit

:exists
echo on
lib %1 %2
echo off
:exit
