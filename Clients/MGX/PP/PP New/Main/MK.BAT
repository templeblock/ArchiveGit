@echo off

if '%0' == 'mk/out' goto REDIRECT

nmake /NOLOGO NODEBUG=1
goto END

:REDIRECT
nmake /I /NOLOGO NODEBUG=1 >mk.out

:END

REM signal end of build
be beep

