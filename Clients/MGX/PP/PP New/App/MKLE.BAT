@echo off
if '%1' == '/out' goto REDIRECT

nmake /NOLOGO le_version=1 nodebug=1 

goto END

:REDIRECT

nmake /I /NOLOGO le_version=1 nodebug=1 > mk.out

:END
