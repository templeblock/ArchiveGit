@echo off
set ZTMP=c:\
if '%1' == '' goto error
set DST=%2
if '%2' == '' set DST=a:

pkzip -ex -P -b%ZTMP% -%1 %dst%\mak.zip *.bat *.mak src\makefile src\*.bat src\*.def
pkzip -ex -P -b%ZTMP% -%1 %dst%\src.zip src\*.c src\*.asm src\*.rc src\*.r?
pkzip -ex -P -b%ZTMP% -%1 %dst%\inc.zip src\*.h src\*.p
pkzip -ex -P -b%ZTMP% -%1 %dst%\res.zip res\*.ico res\*.bmp res\*.cur res\*.wav
pkzip -ex -P -b%ZTMP% -%1 %dst%\ref.zip ref\*.*
pkzip -ex -P -b%ZTMP% -%1 %dst%\bin.zip bin\*.exe

goto end
:error
echo Please enter '%0 i [destination path]' for an incremental update or,
echo        enter '%0 f [destination path]' to freshen the backup
echo        enter '%0 a [destination path]' to backup everything
:end
set DST=
set ZTMP=