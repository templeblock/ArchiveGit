@echo off

rem
rem
rem
rem	Windows NT cmd.exe batch file for building various versions of
rem	the wlm crayola app
rem
rem		nt		build windows nt version
rem		mac		build macintosh version
rem
rem		ship	build ship version
rem		debug	build debug version
rem

setlocal

set mac=0
set debug=1
set TYPE=STUDIO

if "%wings%" == "" set wings=..\..

:nextarg

:chkship
if not "%1" == "ship" goto chkdb
shift
set debug=0
goto nextarg

:chkdb
if not "%1" == "cv" goto chkmac
set debug=1
shift
goto nextarg

:chkmac
if not "%1" == "mac" goto chkwin
shift
set mac=1
goto nextarg

:chkwin
if not "%1" == "win" goto chknt
shift
set mac=0
goto nextarg

:chknt
if not "%1" == "nt" goto domk
shift
set mac=0
goto nextarg

:domk
if "%mac%" == "" set mac=1
if "%debug%" == "" set debug=1

if "%mac%" == "0" goto nmake

set include=%wings%\include
set lib=%wings%\lib
set path=%wings%\bin;%path%
:nmake
@echo on
nmake /f make_nt.mak /nologo APP=%TYPE% DEBUG="%debug%" MAC="%mac%" %1 %2 %3
endlocal
