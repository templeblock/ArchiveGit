@echo off
if '%1' == '/out' goto REDIRECT

nmake build=NT proj=framedll builddll=1 objdir=dllobj

goto END

:REDIRECT
nmake build=NT proj=framedll builddll=1 objdir=dllobj > mk.out

:END
