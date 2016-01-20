@echo off
set ZTMP=c:\
if '%1' == '' goto error
set DST=%2
if '%2' == '' set DST=a:

pkzip -ex -P -b%ZTMP% -%1 %dst%\files.zip files\*.*

goto end
:error
echo Please enter '%0 i [destination path]' for an incremental update or,
echo        enter '%0 f [destination path]' to freshen the backup
echo        enter '%0 a [destination path]' to backup everything
:end
set DST=
set ZTMP=