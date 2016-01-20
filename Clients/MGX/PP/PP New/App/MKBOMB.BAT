@echo off

if '%0' == 'mkbomb/out' goto REDIRECT

nmake /NOLOGO TRIAL_VERSION=1 NODEBUG=1 
goto END

:REDIRECT
nmake /I /NOLOGO TRIAL_VERSION=1 NODEBUG=1 >mk.out

:END
