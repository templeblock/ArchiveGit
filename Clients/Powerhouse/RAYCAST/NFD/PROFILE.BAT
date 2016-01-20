@echo off
if exist c:\csips.dat		move c:\csips.dat	.
if exist c:\segentry.dat	move c:\segentry.dat	.
mapsym fd.map
showhits -Ifd.sym;d:\msvc\nodebug >fd.prf
echo.
echo.
echo	Output data is in FD.PRF
echo.
