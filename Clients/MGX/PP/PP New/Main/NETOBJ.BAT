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
co %SRC%\INC\RCS\[i,g,h]*.h
co %SRC%\INC\RCS\[a,b,c]*.h
co %SRC%\INC\RCS\[d,e,f]*.h
co %SRC%\INC\RCS\[j,k,l]*.h
co %SRC%\INC\RCS\[m,n,o]*.h
co %SRC%\INC\RCS\[p,q,r]*.h
co %SRC%\INC\RCS\[s,t,u]*.h
co %SRC%\INC\RCS\[v,w,x]*.h
co %SRC%\INC\RCS\[y,z  ]*.h
cd ..

cd SRC
co %SRC%\SRC\RCS\*.cpp
co %SRC%\SRC\RCS\*.asm
co %SRC%\SRC\RCS\makefile
co %SRC%\SRC\RCS\[a,b,c]*.c
co %SRC%\SRC\RCS\[d,e,f]*.c
co %SRC%\SRC\RCS\[g,h,i]*.c
co %SRC%\SRC\RCS\[j,k,l]*.c
co %SRC%\SRC\RCS\[m,n,o]*.c
co %SRC%\SRC\RCS\[p,q,r]*.c
co %SRC%\SRC\RCS\[s,t,u]*.c
co %SRC%\SRC\RCS\[v,w,x]*.c
co %SRC%\SRC\RCS\[y,z  ]*.c
cd ..

cd RES
co %SRC%\RES\RCS\[a,b,c]*.ico
co %SRC%\RES\RCS\[d,e,f]*.ico
co %SRC%\RES\RCS\[g,h,i]*.ico
co %SRC%\RES\RCS\[j,k,l]*.ico
co %SRC%\RES\RCS\[m,n,o]*.ico
co %SRC%\RES\RCS\[p,q,r]*.ico
co %SRC%\RES\RCS\[s,t,u]*.ico
co %SRC%\RES\RCS\[v,w,x]*.ico
co %SRC%\RES\RCS\[y,z  ]*.ico

co %SRC%\RES\RCS\[a,b,c]*.cur
co %SRC%\RES\RCS\[d,e,f]*.cur
co %SRC%\RES\RCS\[g,h,i]*.cur
co %SRC%\RES\RCS\[j,k,l]*.cur
co %SRC%\RES\RCS\[m,n,o]*.cur
co %SRC%\RES\RCS\[p,q,r]*.cur
co %SRC%\RES\RCS\[s,t,u]*.cur
co %SRC%\RES\RCS\[v,w,x]*.cur
co %SRC%\RES\RCS\[y,z  ]*.cur
co %SRC%\RES\RCS\*.bmp
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
