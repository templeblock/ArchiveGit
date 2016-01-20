@echo off

SET AGIMAGEDIR=\\NVTODEV5\PROJECTS5B\PMG\AG\IMAGES
set SourceDir=winrel

if not "%1"=="PRST" goto :notPRST
@echo Building Print Standard Image
MAP DEL Q:
MAP R Q:=%AGIMAGEDIR%\PRINT10.CD 
if errorlevel 1 goto :end
CALL CHECKMAP.BAT
if errorlevel 1 goto :end
set IDFILE=agprst10.ID
set INIFILE=msregprs.ini
set PRODUCTRESDLL=printres.dll
set PRODUCTHUB=agprhub.dll
set APPFILENAME=agpr.exe
goto :doit1

:notPRST
if not "%1"=="PRPR" goto :notPRPR
@echo Building Print Premium Image
MAP DEL Q:
MAP R Q:=%AGIMAGEDIR%\PRINT10.CD 
if errorlevel 1 goto :end
CALL CHECKMAP.BAT
if errorlevel 1 goto :end
set IDFILE=agprpr10.id
set INIFILE=msregprp.ini
set PRODUCTRESDLL=printres.dll
set PRODUCTHUB=agprhub.dll
set APPFILENAME=agpr.exe
goto :doit1

:notPRPR
if not "%1"=="CR" goto :notCR
@echo Building Craft Standard Image
MAP DEL Q:
MAP R Q:=%AGIMAGEDIR%\CRAFT10.CD 
if errorlevel 1 goto :end
CALL CHECKMAP.BAT
if errorlevel 1 goto :end
set IDFILE=agcrst10.id
set INIFILE=msregcrs.ini
set PRODUCTRESDLL=craftres.dll
set PRODUCTHUB=agcrhub.dll
set APPFILENAME=agcr.exe
goto :doit1

:notCR
if not "%1"=="SP" goto :notSP
@echo Building Spiritual Image
MAP DEL Q:
MAP R Q:=%AGIMAGEDIR%\SPIRIT10.CD 
if errorlevel 1 goto :end
CALL CHECKMAP.BAT
if errorlevel 1 goto :end
set IDFILE=agspst10.id
set INIFILE=msregsps.ini
set PRODUCTRESDLL=spiritre.dll
set PRODUCTHUB=agsphub.dll
set APPFILENAME=agsp.exe
goto :doit1

:notSP
if not "%1"=="AM" goto :notAM
@echo Building Print Art/More Edition
MAP DEL Q:
MAP R Q:=%AGIMAGEDIR%\ARTSTORE.CD 
if errorlevel 1 goto :end
CALL CHECKMAP.BAT
if errorlevel 1 goto :end
set IDFILE=agstore1.id
set INIFILE=msregam.ini
set PRODUCTRESDLL=printres.dll
set PRODUCTHUB=artmhub1.dll
set APPFILENAME=agpram.exe
set SourceDir=asrel
goto :doit1

:notAM
@echo Specify Operation:
@echo PRST (Print Center Standard)
@echo PRPR (Print Center Premium)
@echo CR   (Craft Standard)
@echo SP   (Spirtual Standard)
@echo AM   (Print Art/More Edition)
goto :end

:doit1

@echo Copying INI File
CALL GET.BAT %INIFILE%
copy %INIFILE% q:\msreg\msreg.ini

@echo Copying ID File
CALL GET.BAT %IDFILE%
del q:\ag*.id
copy %IDFILE% q:\

pause

REM @echo Copying Help Wizard File
REM CALL GET.BAT whwizard.ini
REM copy whwizard.ini q:\withhelp
REM copy whwizard.ini q:\pmw\withhelp

@echo Copying Application, Install, and DLL files
copy %SourceDir%\pmw.exe q:\pmw32\%APPFILENAME%
copy winrel\pmwres32.dll q:\pmw32\pmwres32.dll
copy winrel\%PRODUCTRESDLL% q:\pmw32\%PRODUCTRESDLL%
copy %SourceDir%\%PRODUCTHUB% q:\pmw32\hubres32.dll
copy winrel\install.exe q:\install\inst32.exe
copy uninst\winrel\uninst32.exe q:\pmw32\uninst32.exe
pause

@echo Copying Program maps
copy winrel\pmw.map pmw32.map
pkzip -a -ex -s5106525464 q:\diag\pmwmaps. pmw32.map
copy winrel\install.map inst32.map
pkzip -a -ex -s5106525464 q:\diag\pmwmaps. inst32.map
copy dbconvrt.exe q:\install\dbconvrt.exe
pkzip -a -ex -s5106525464 q:\diag\pmwmaps. dbconvrt.map
copy ..\tt\winrel\pmwtt.map pmwtt32.map
pkzip -a -ex -s5106525464 q:\diag\pmwmaps. pmwtt32.map

@echo Updating PFL Files
call updagpfl.bat
goto upddone

:upddone

:end
set IDFILE=
set INIFILE=
SET AGIMAGEDIR=
set PRODUCTRESDLL=
set PRODUCTHUB=
set APPFILENAME=
set SourceExe=
set SourceDir=
@echo on
