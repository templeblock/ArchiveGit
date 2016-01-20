@echo off

set folder=%1
if '%folder%'=='' goto :Error

set file=%2
if '%file%'=='' goto :Error

set proj=$/disk2/websitesimg/american/html/cp/install
set dest="V:\websitesimg\american\html\cp\install"
cd %dest%

set ssdir=\\walt\websource\vss\royal
set sse="C:\Program Files\Microsoft Visual Studio\VSS\win32\ss.exe"
rem The working folder has no effect on the command line and vice versa
rem echo Setting the working folder for project %proj% to %dest%...
rem %sse% workfold %proj% %dest% -I-

echo Checking out "%file%"...
V:
%sse% checkout %proj%/%file% -I- -C- -GCD
c:

echo Copying "%folder%\%file%" to %dest%...
copy %folder%\%file% %dest%

echo Checking in "%file%"...
V:
%sse% checkin %proj%/%file% -I- -C- -GCD
c:

goto :end

:Error
echo syntax: Checkin folder file

:end
