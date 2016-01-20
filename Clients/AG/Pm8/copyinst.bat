@echo off

if "%2"=="Rel" goto :IsReleaseCopy
if "%2"=="Debug" goto :IsDebugCopy
if not "%2"=="" goto :notMatch
REM Debug copy is assumed, so fall through
:IsDebugCopy
set COPYTYPE=WinDebug
goto :CheckVersion
:IsReleaseCopy
set COPYTYPE=winRel

:CheckVersion
if not "%1"=="AGPRST" goto :notAGPRST
@echo Product is American Greetings Print Standard
set PRODUCTCFG=AGPRST.DAT
set PRODUCTHUB=AGPRHUB.DLL
set PRODHUBDIR=Resource\AG\Print\%COPYTYPE%
set INIFILE=msregprs.ini
goto :copyit

:notAGPRST
if not "%1"=="AGPRPR" goto :notAGPRPR
@echo Product is American Greetings Print Premium
set PRODUCTCFG=AGPRINT.DAT
set PRODUCTHUB=AGPRHUB.DLL
set PRODHUBDIR=Resource\AG\Print\%COPYTYPE%
set INIFILE=msregprp.ini
goto :copyit

:notAGPRPR
if not "%1"=="AGCR" goto :notAGCRST
@echo Product is American Greetings Craft
set PRODUCTCFG=AGCRAFT.DAT
set PRODUCTHUB=AGCRHUB.DLL
set PRODHUBDIR=Resource\AG\Craft\%COPYTYPE%
set INIFILE=msregcrs.ini
goto :copyit

:notAGCRST
if not "%1"=="AGCRDX" goto :notAGCRDX
@echo Product is American Greetings Craft Deluxe
set PRODUCTCFG=AGCRAFTD.DAT
set PRODUCTHUB=AGCRDXHUB.DLL
set PRODHUBDIR=Resource\AG\Craft\%COPYTYPE%
set INIFILE=msregcrd.ini
goto :copyit

:notAGCRDX
if not "%1"=="AGCPARTY" goto :notAGCPARTY
@echo Product is American Greetings CreataParty
set PRODUCTCFG=AGCPARTY.DAT
set PRODUCTHUB=AGCPHUB.DLL
set PRODHUBDIR=Resource\AG\CreateAParty\%COPYTYPE%
set INIFILE=msregcps.ini
goto :copyit

:notAGCPARTY
if not "%1"=="AGSP" goto :notAGSPIRIT
@echo Product is American Greetings Spirit
set PRODUCTCFG=AGSPIRIT.DAT
set PRODUCTHUB=AGSPHUB.DLL
set PRODHUBDIR=Resource\AG\Spirit\%COPYTYPE%
set INIFILE=msregsps.ini
goto :copyit

:notAGSPIRIT
if not "%1"=="PMPS" goto :notPMPS
@echo Product is PrintMaster Publishing Suite
set PRODUCTCFG=PMPS.DAT
set PRODUCTHUB=hubres32.DLL
set PRODHUBDIR=%COPYTYPE%\PMHub
set INIFILE=msregps.ini
goto :copyit

:notPMPS
if not "%1"=="PMPLAT" goto :notPMPLAT
@echo Product is PrintMaster Platiunum
set PRODUCTCFG=PMPLAT.DAT
set PRODUCTHUB=hubres32.DLL
set PRODHUBDIR=%COPYTYPE%\PMHub
set INIFILE=msregpl.ini
goto :copyit

:notPMPLAT
if not "%1"=="PRNTSHOP" goto :notMatch
@echo Product is Print Shop
set PRODUCTCFG=PRNTSHOP.DAT
set PRODUCTHUB=hubres32.DLL
set PRODHUBDIR=%COPYTYPE%\PMHub
set INIFILE=msregpri.ini
goto :copyit

:notMatch
@echo Specify Operation:
@echo AGPRST   (American Greetings Print Standard)
@echo AGPRPR   (American Greetings Print Premium)
@echo AGCR     (American Greetings Craft)
@echo AGCRDX   (American Greetings Craft Deluxe)
@echo AGCPARTY (American Greetings CreataParty)
@echo AGSP     (American Greetings Spiritual)
@echo PMPS     (PrintMaster Publishing Suite)
@echo PMPLAT   (PrintMaster Platinum)
@echo PRNTSHOP (Print Shop)
@echo.
@echo Specify Debug or Release Build to set (Debug is default if none given)
@echo Rel     (Place files for a release build)
@echo Debug   (Place files for a debug build)
goto :end

:copyit

@echo Copying Installation Data
@echo Copying instdat\%PRODUCTCFG% to %COPYTYPE%\pminst.dat
copy instdat\%PRODUCTCFG% %COPYTYPE%\pminst.dat
@echo Copying %PRODHUBDIR%\%PRODUCTHUB% to %COPYTYPE%\hubres32.dll
copy %PRODHUBDIR%\%PRODUCTHUB% %COPYTYPE%\hubres32.dll
@echo Copying App\%INIFILE% to %COPYTYPE%\msreg.ini
copy App\%INIFILE% %COPYTYPE%\msreg.ini
@echo Done

:end
set PRODUCTCFG=
set INIFILE=
set PRODUCTHUB=
set PRODHUBDIR=
@echo on
