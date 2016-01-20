@echo off
if "%1"=="" goto noname
if "%1"=="+" goto noname
if exist "..\%1" goto already
pkzip -rp -e0 -x@ziplist.txt ..\N%1 *.*
pkzip -a -rp -e0 ..\N%1 ack\ack*.obj
pkzip -a -rp -e0 ..\N%1 medit\measm.obj
if "%2"=="+" pkzip -a -rp -e0 ..\N%1 fd.exe
pkzip -m -ex ..\%1 ..\N%1.ZIP
goto done
:noname
echo.
echo You must specify a file name that does not exist
echo in the parent directory.  Specifying a "+" as the
echo second parameter will include the executable.
echo.
goto done
:already
echo.
echo File already exists in parent directory
echo.
dir ..\%1
goto done
:done
