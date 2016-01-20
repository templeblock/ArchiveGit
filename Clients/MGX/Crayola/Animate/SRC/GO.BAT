@echo off

if '%1' == 'cv' goto :codeview
nmake animate.mak
goto :end

:codeview
nmake animate.mak cv=1

:end
