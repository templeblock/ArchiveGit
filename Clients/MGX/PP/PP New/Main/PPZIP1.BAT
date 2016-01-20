@echo off
pkzip -P %1\build.zip *.bat makefile
pkzip -P %1\app.zip pp\*.bat pp\makefile
pkzip -P %1\inc.zip pp\inc\*.h
pkzip -P %1\res1.zip pp\res\*.rc pp\res\makefile pp\res\*.c pp\res\*.def pp\res\*.h
pkzip -P %1\res2.zip pp\res\*.ico pp\res\*.cur pp\res\*.bmp
pkzip -P %1\browser.zip browser\*.c browser\*.cpp browser\*.h browser\makefile
pkzip -P %1\lib.zip lib\*.*
pkzip -P %1\ctllib.zip ctllib\*.c ctllib\*.cpp ctllib\*.h ctllib\*.rh ctllib\makefile
pkzip -P %1\frame.zip frame\*.c frame\*.asm frame\*.def frame\*.cpp frame\*.h frame\makefile
pkzip -P %1\fx.zip fx\*.txt fx\src\*.c fx\src\*.asm fx\src\*.cpp fx\src\makefile fx\src\*.def fx\inc\*.h 
pkzip -P %1\fxres.zip fx\res\*.rc fx\res\*.ico fx\res\*.cur fx\res\*.bmp
