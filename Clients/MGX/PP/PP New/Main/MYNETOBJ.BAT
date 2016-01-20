rem @echo off


if '%1' == 'app' goto APP
if '%1' == 'pp' goto PP
if '%1' == 'ctllib' goto CTLLIB
if '%1' == 'imgutil' goto IMGUTIL
if '%1' == 'apputil' goto APPUTIL
if '%1' == 'bitmapio' goto BITMAPIO
if '%1' == 'superblt' goto SUPERBLT
if '%1' == 'lib' goto LIB
if '%1' == 'fx' goto FX
if '%1' == 'browser' goto BROWSER

:APP
set SRC=L:\DEV\IMG\PP60
co     %SRC%\RCS\*

if NOT '%1' == '' goto DONE

:PP
set SRC=L:\DEV\IMG\PP60\PP
cd PP
co     %SRC%\APP\RCS\*
cd INC
co %SRC%\INC\RCS\*.h
cd ..

cd SRC
co %SRC%\SRC\RCS\*.cpp
co %SRC%\SRC\RCS\*.asm
co %SRC%\SRC\RCS\makefile
cd ..

cd RES
rem co %SRC%\RES\RCS\*.ico
rem co %SRC%\RES\RCS\*.cur
rem co %SRC%\RES\RCS\*.bmp
co %SRC%\RES\RCS\*.h
co %SRC%\RES\RCS\*.def
co %SRC%\RES\RCS\*.c
co %SRC%\RES\RCS\*.rc
co %SRC%\RES\RCS\makefile
cd ..
cd ..

if NOT '%1' == '' goto DONE

:BROWSER
set SRC=L:\DEV\IMG\PP60\BROWSER
cd BROWSER
co %SRC%\RCS\*
cd ..

if NOT '%1' == '' goto DONE

:CTLLIB
set SRC=L:\DEV\IMG\PP60\CTLLIB
cd CTLLIB
co %SRC%\RCS\*
co %SRC%\RCS\*.
cd ..

if NOT '%1' == '' goto DONE

:LIB
set SRC=L:\DEV\IMG\PP60\LIB
cd LIB
co %SRC%\RCS\*
cd ..

if NOT '%1' == '' goto DONE

:IMGUTIL
set SRC=L:\DEV\IMG\PP60\IMGUTIL
cd IMGUTIL
co %SRC%\RCS\*
cd ..

if NOT '%1' == '' goto DONE

:APPUTIL
set SRC=L:\DEV\IMG\PP60\APPUTIL
cd APPUTIL
co %SRC%\RCS\*
cd ..

if NOT '%1' == '' goto DONE

:SUPERBLT
set SRC=L:\DEV\IMG\PP60\SUPERBLT
cd SUPERBLT
co %SRC%\RCS\*
cd ..

if NOT '%1' == '' goto DONE

:BITMAPIO
set SRC=L:\DEV\IMG\PP60\BITMAPIO
cd BITMAPIO
co %SRC%\RCS\*
cd ..

goto DONE

:FX
set SRC=L:\DEV\IMG\PP60\FX
cd FX
co     %SRC%\RCS\*
cd SRC
co %SRC%\SRC\RCS\*
cd ..
cd INC
co %SRC%\INC\RCS\*
cd ..
cd RES
co %SRC%\RES\RCS\*
cd ..
cd ..
:DONE

