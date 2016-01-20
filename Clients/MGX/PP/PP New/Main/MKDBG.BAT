@echo off
if '%0' == 'mkdbg/out' goto REDIRECT

nmake

goto END

:REDIRECT

nmake > mk.out

:END
