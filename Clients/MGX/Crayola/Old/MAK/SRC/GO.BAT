    @echo off

    rem Free some environment space for us DOS shell users:
    set RESERVE1=
    set RESERVE2=
    set RESERVE3=
    set RESERVE4=

    set APP=STUDIO
    set BUILD=
    set CDROM=
    if %1.==.       goto MAKE
:INIT
    if %1==!        goto MAKE
    if %1==ADVENT   set APP=ADVENT
    if %1==advent   set APP=ADVENT
    if %1==CV       set BUILD=CV
    if %1==cv       set BUILD=CV
    if %1==CVL      set BUILD=CVL
    if %1==cvl      set BUILD=CVL
    if %1==D        if exist ..\obj\depend.inc del ..\obj\depend.inc >nul
    if %1==d        if exist ..\obj\depend.inc del ..\obj\depend.inc >nul
    if %1==DNL      set BUILD=DNL
    if %1==dnl      set BUILD=DNL
    if %1==SHIP     set BUILD=SHIP
    if %1==ship     set BUILD=SHIP
    if %1==CD       set CDROM=CDROM
    if %1==cd       set CDROM=CDROM
    shift
    if not %1.==.   goto INIT
:MAKE
    nmake /nologo /k %APP%=1 %2 %3 %4 %5
	set APP=
	set BUILD=
