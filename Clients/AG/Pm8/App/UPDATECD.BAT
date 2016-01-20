if not exist q:\install.exe goto :badmap
if not exist q:\autorun.inf goto :badmap
goto :goodmap
:badmap
echo Q: is not mapped correctly!
goto :end
:goodmap

if not "%1"=="pl" goto :notpl
set IDFILE=pmpl70i.id
set INIFILE=msregpl.ini
goto :doit1

:notpl
if not "%1"=="dx" goto :notdx
set IDFILE=pmdx70i.id
set INIFILE=msregdx.ini
goto :doit1

:notdx
if not "%1"=="ps" goto :notps
set IDFILE=pmps70.id
set INIFILE=msregps.ini
goto :doit1

:notps
if not "%1"=="bp" goto :notbp
set IDFILE=pmbp70.id
set INIFILE=msregbp.ini
goto :doit1

:notbp
if not "%1"=="bpn" goto :notbpn
set IDFILE=pmbp70.id
set INIFILE=msregbpn.ini
goto :doit1

:notbpn
if not "%1"=="cl" goto :notcl
set IDFILE=pmcl70.id
set INIFILE=msregcl.ini
goto :doit1

:notcl
if not "%1"=="clg" goto :notclg
set IDFILE=pmcl70.id
set INIFILE=msregclg.ini
goto :doit1

:notclg
if not "%1"=="net" goto :notnet
set IDFILE=pmps70.id
set INIFILE=msregnet.ini
goto :doit1

:notnet
if not "%1"=="ng" goto :notng
set IDFILE=pmng70.id
set INIFILE=msregng.ini
goto :doit1

:notng
if not "%1"=="hp" goto :nothp
set IDFILE=pmps70.id
set INIFILE=msreghp.ini
goto :doit1

:nothp
if not "%1"=="pr" goto :notpr
set IDFILE=pmpr70i.id
set INIFILE=msregpr.ini
goto :doit1

:notpr
if not "%1"=="os" goto :notos
set IDFILE=pmos70i.id
set INIFILE=msregos.ini
goto :doit1

:notpr
echo Specify version: dx, ps, bp, bpn, clg, net, ng, hp, pr or os
goto :end

:doit1

if "%1"=="pr" goto :copypr
if "%1"=="pl" goto :copypl
if "%1"=="dx" goto :copydx
goto :doit2

:copypl
rem copy j:\images\platinum.col\*.* q:\content
goto :doit2

:copypr
rem copy j:\images\premier.col\*.* q:\content
rem copy j:\images\deluxe.col\*.* q:\content
goto :doit2

:copydx
rem copy g:\images\deluxe.col\*.* q:\content
goto :doit2

:doit2

rem Copy INI File
call get %INIFILE%
copy %INIFILE% q:\msreg\msreg.ini

rem Copy ID File
call get %IDFILE%
del q:\pm??70*.id
copy %IDFILE% q:\

rem Copy PMW and Install Programs (and more)
copy winrel\pmw.exe q:\pmw32\pmw.exe
copy winrel\pmwres32.dll q:\pmw32\pmwres32.dll
copy winrel\hubres32.dll q:\pmw32\hubres32.dll
copy winrel\install.exe q:\install\inst32.exe
copy border\release\border.dll q:\pmw32\border.dll
copy ..\hints\hints.hnt q:\pmw32\hints.hnt
copy pmw.chm q:\pmw32\pmw.chm

rem Copy Maps
copy winrel\pmw.map pmw32.map
pkzip -a -ex -s5106525464 q:\diag\pmwmaps. pmw32.map
rem copy winrel\install.map inst32.map
rem pkzip -a -ex -s5106525464 q:\diag\pmwmaps. inst32.map
rem copy dbconvrt.exe q:\install\dbconvrt.exe
rem pkzip -a -ex -s5106525464 q:\diag\pmwmaps. dbconvrt.map
rem copy ..\tt\winrel\pmwtt.map pmwtt32.map
rem pkzip -a -ex -s5106525464 q:\diag\pmwmaps. pmwtt32.map

rem Update PFL Files
if "%1"=="net" goto :updnet
if "%1"=="pl" goto :updpl
call updpfl32.bat
goto upddone

:updnet
call updnet32.bat
goto upddone

:updpl
call updpflpl.bat
goto upddone

:upddone

if not "%1"=="ng" goto notngupd
rem Create National Geographic update PFL files.

call updng32.bat

:notngupd

:end

