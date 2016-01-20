@echo off
set ZTMP=c:\
if '%1' == '' goto error
set DST=%2
if '%2' == '' set DST=a:

pkzip -P -b%ZTMP% -%1 %dst%\bin.zip -r makefile mk.bat vcs.cfg
pkzip -P -b%ZTMP% -%1 %dst%\bin.zip link* *.def *.lib *.bat pcdlib.dll
pkzip -P -b%ZTMP% -%1 %dst%\src.zip src\*.c src\*.asm
pkzip -P -b%ZTMP% -%1 %dst%\lib.zip lib\*.c lib\*.asm
pkzip -P -b%ZTMP% -%1 %dst%\inc.zip inc\*.h inc\*.rh
pkzip -P -b%ZTMP% -%1 %dst%\res.zip res\pp.rc res\*.rx res\*.rh res\*.h res\*.ico res\*.bmp res\*.cur

goto end
:error
echo Please enter 'ppzip i [destination path]' for an incremental update or,
echo        enter 'ppzip f [destination path]' to freshen the backup
echo        enter 'ppzip a [destination path]' to backup everything
:end
set DST=
set ZTMP=
