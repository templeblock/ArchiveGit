@echo off
if '%1' == '' goto BadArgs
if '%2' == '' goto BadArgs

set Include=%1\h;..\inc;%2\lib;%2\frame;%2\browser;%2\pp\inc
set Lib=%1\lib;%2\lib;%2\frame\lib;%2\browser\lib
goto End

:BadArgs
echo usage: %0 [compiler path] [build path]
:End
