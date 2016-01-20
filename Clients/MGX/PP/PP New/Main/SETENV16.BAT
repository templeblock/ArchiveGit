@ECHO OFF
if '%1' == '' goto BadArgs
if '%2' == '' goto BadArgs

path %path%;%1\bin

set include=..\inc;%2\lib;%2\frame;%2\browser;%2\ctllib;%2\pp\inc;%1\include;%1\mfc\include
set lib=%2\lib;%2\frame\lib;%2\browser\lib;%2\ctllib\lib;%1\lib;%1\mfc\lib
goto End

:BadArgs
echo usage: %0 [compiler path] [build path]
:End

