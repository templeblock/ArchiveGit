@echo off
if '%1' == '/out' goto REDIRECT

nmake build=NT nodebug=1

goto END

:REDIRECT

nmake build=NT nodebug=1 > mk.out

:END
