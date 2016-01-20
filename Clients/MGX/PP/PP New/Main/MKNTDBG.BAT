@echo off
if '%1' == '/out' goto REDIRECT

nmake build=NT

goto END

:REDIRECT

nmake build=NT > mk.out

:END
