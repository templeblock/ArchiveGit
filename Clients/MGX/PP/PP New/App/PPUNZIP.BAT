@echo off
if '%1' == '' goto error
set SRC=%1
pkunzip -d -n %SRC%\*.zip
goto end
:error
echo Please enter 'ppunzip [source path]'
:end
set SRC=
