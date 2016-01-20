@echo off
if "%1"=="" goto noname
if exist "..\%1" goto already
pkzip -rp -ex -x@rellist.txt ..\%1 *.*
goto done
:noname
echo.
echo You must specify a file name that does not exist
echo in the parent directory.
echo.
goto done
:already
echo.
echo File already exists in parent directory
echo.
dir ..\%1
goto done
:done
