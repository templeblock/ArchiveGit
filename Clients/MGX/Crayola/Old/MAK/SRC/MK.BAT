    @echo off

    set APP=STUDIO
    set BUILD=
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
    shift
    if not %1.==.   goto INIT
:MAKE
    nmake /nologo %APP%=1 %2 %3 %4 %5
	set APP=
	set BUILD=
