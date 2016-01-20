@echo off
if '%1' == '' goto error
set SRC=%1
pkunzip -d -n %SRC%\animate.zp
goto end
:error
echo Please enter '%0 [source path]'
:end
set SRC=
