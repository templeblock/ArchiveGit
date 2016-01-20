rem
rem This batch file builds Windows NT DLLs.  Microsoft Visual C++ has a
rem problem properly exporting all the library classes and functions
rem into the dll.
rem

rem
rem The following section of code has been remarked out since MS Visual C++
rem v2.0's librarian does not function properly.  Anytime the librarian is
rem run it deletes the old library and starts from scratch rather than
rem replacing the new object modules into the old library.  This make it
rem necessary to rebuild the complete library anytime a source file is changed.
rem (A little time consuming)
rem
rem **** START ****
rem if %3 == NOWIN32S goto tag1
rem echo CW32S.OBJ          > _NTDLL.CMD
rem goto buildlib

rem :tag1
rem echo VIDALL.OBJ         >  _NTDLL.CMD
rem echo ASCIITRN.OBJ       >> _NTDLL.CMD
rem echo BP.OBJ             >> _NTDLL.CMD
rem echo CHECKSUM.OBJ       >> _NTDLL.CMD
rem echo CHK16CRC.OBJ       >> _NTDLL.CMD
rem echo CHK32CRC.OBJ       >> _NTDLL.CMD
rem echo CHKCCITT.OBJ       >> _NTDLL.CMD
rem echo COMLINK.OBJ        >> _NTDLL.CMD
rem echo DATAFM_R.OBJ       >> _NTDLL.CMD
rem echo DATAFORM.OBJ       >> _NTDLL.CMD
rem echo ERRNAME.OBJ        >> _NTDLL.CMD
rem echo XFERHELP.OBJ       >> _NTDLL.CMD
rem echo GETBIOS.OBJ        >> _NTDLL.CMD
rem echo XMODEM.OBJ         >> _NTDLL.CMD
rem echo KERMITR.OBJ        >> _NTDLL.CMD
rem echo KERMITS.OBJ        >> _NTDLL.CMD
rem echo LINEST_R.OBJ       >> _NTDLL.CMD
rem echo LINESTAT.OBJ       >> _NTDLL.CMD
rem echo MEMFUN.OBJ         >> _NTDLL.CMD
rem echo MODEM.OBJ          >> _NTDLL.CMD
rem echo PVTRAP.OBJ         >> _NTDLL.CMD
rem echo SCRNDRVR.OBJ       >> _NTDLL.CMD
rem echo SERIAL.OBJ         >> _NTDLL.CMD
rem echo TERMINAL.OBJ       >> _NTDLL.CMD
rem echo TRANSFER.OBJ       >> _NTDLL.CMD
rem echo TRANSTXT.OBJ       >> _NTDLL.CMD
rem echo USESTIME.OBJ       >> _NTDLL.CMD
rem echo VT100.OBJ          >> _NTDLL.CMD
rem echo VT52.OBJ           >> _NTDLL.CMD
rem echo KERMIT.OBJ         >> _NTDLL.CMD
rem echo ENGINE.OBJ         >> _NTDLL.CMD
rem echo XMODEM1K.OBJ       >> _NTDLL.CMD
rem echo XMODEMKG.OBJ       >> _NTDLL.CMD
rem echo XMODEMR.OBJ        >> _NTDLL.CMD
rem echo XMODEMS.OBJ        >> _NTDLL.CMD
rem echo YMODEM.OBJ         >> _NTDLL.CMD
rem echo YMODEMG.OBJ        >> _NTDLL.CMD
rem echo ZMODEM.OBJ         >> _NTDLL.CMD
rem echo ZMODEMC.OBJ        >> _NTDLL.CMD
rem echo ZMODEMR.OBJ        >> _NTDLL.CMD
rem echo ZMODEMS.OBJ        >> _NTDLL.CMD
rem echo NASI_X.OBJ         >> _NTDLL.CMD
rem echo ANSITERM.OBJ       >> _NTDLL.CMD
rem echo IOCONTRL.OBJ       >> _NTDLL.CMD
rem echo CW32_R.OBJ         >> _NTDLL.CMD
rem echo MABRTW32.OBJ       >> _NTDLL.CMD
rem echo MESS_W32.OBJ       >> _NTDLL.CMD
rem echo TABRTW32.OBJ       >> _NTDLL.CMD
rem echo TIMERW32.OBJ       >> _NTDLL.CMD
rem echo TWDSPW32.OBJ       >> _NTDLL.CMD
rem echo FILE_W32.OBJ       >> _NTDLL.CMD
rem echo CW32.OBJ           >> _NTDLL.CMD
rem echo TW_W32.OBJ         >> _NTDLL.CMD
rem echo VIDC32.OBJ         >> _NTDLL.CMD
rem echo VIDDC32.OBJ        >> _NTDLL.CMD
rem echo WINCOMAL.OBJ       >> _NTDLL.CMD
rem echo WINVER32.OBJ       >> _NTDLL.CMD

rem :buildlib
rem lib /OUT:_%1.LIB @_NTDLL.CMD
rem **** STOP ****

if %3 == NOWIN32S goto tag2
echo CW32S.OBJ          > _NTDLL.CMD
goto builddll

:tag2
echo VIDALL.OBJ         >  _NTDLL.CMD
echo ASCIITRN.OBJ       >> _NTDLL.CMD
echo BP.OBJ             >> _NTDLL.CMD
echo CHECKSUM.OBJ       >> _NTDLL.CMD
echo CHK16CRC.OBJ       >> _NTDLL.CMD
echo CHK32CRC.OBJ       >> _NTDLL.CMD
echo CHKCCITT.OBJ       >> _NTDLL.CMD
echo COMLINK.OBJ        >> _NTDLL.CMD
echo DATAFM_R.OBJ       >> _NTDLL.CMD
echo DATAFORM.OBJ       >> _NTDLL.CMD
echo ERRNAME.OBJ        >> _NTDLL.CMD
echo XFERHELP.OBJ       >> _NTDLL.CMD
echo GETBIOS.OBJ        >> _NTDLL.CMD
echo XMODEM.OBJ         >> _NTDLL.CMD
echo KERMITR.OBJ        >> _NTDLL.CMD
echo KERMITS.OBJ        >> _NTDLL.CMD
echo LINEST_R.OBJ       >> _NTDLL.CMD
echo LINESTAT.OBJ       >> _NTDLL.CMD
echo MEMFUN.OBJ         >> _NTDLL.CMD
echo MODEM.OBJ          >> _NTDLL.CMD
echo PVTRAP.OBJ         >> _NTDLL.CMD
echo SCRNDRVR.OBJ       >> _NTDLL.CMD
echo SERIAL.OBJ         >> _NTDLL.CMD
echo TERMINAL.OBJ       >> _NTDLL.CMD
echo TRANSFER.OBJ       >> _NTDLL.CMD
echo TRANSTXT.OBJ       >> _NTDLL.CMD
echo USESTIME.OBJ       >> _NTDLL.CMD
echo VT100.OBJ          >> _NTDLL.CMD
echo VT52.OBJ           >> _NTDLL.CMD
echo KERMIT.OBJ         >> _NTDLL.CMD
echo ENGINE.OBJ         >> _NTDLL.CMD
echo XMODEM1K.OBJ       >> _NTDLL.CMD
echo XMODEMKG.OBJ       >> _NTDLL.CMD
echo XMODEMR.OBJ        >> _NTDLL.CMD
echo XMODEMS.OBJ        >> _NTDLL.CMD
echo YMODEM.OBJ         >> _NTDLL.CMD
echo YMODEMG.OBJ        >> _NTDLL.CMD
echo ZMODEM.OBJ         >> _NTDLL.CMD
echo ZMODEMC.OBJ        >> _NTDLL.CMD
echo ZMODEMR.OBJ        >> _NTDLL.CMD
echo ZMODEMS.OBJ        >> _NTDLL.CMD
echo NASI_X.OBJ         >> _NTDLL.CMD
echo ANSITERM.OBJ       >> _NTDLL.CMD
echo IOCONTRL.OBJ       >> _NTDLL.CMD
echo CW32_R.OBJ         >> _NTDLL.CMD
echo MABRTW32.OBJ       >> _NTDLL.CMD
echo MESS_W32.OBJ       >> _NTDLL.CMD
echo TABRTW32.OBJ       >> _NTDLL.CMD
echo TIMERW32.OBJ       >> _NTDLL.CMD
echo TWDSPW32.OBJ       >> _NTDLL.CMD
echo FILE_W32.OBJ       >> _NTDLL.CMD
echo CW32.OBJ           >> _NTDLL.CMD
echo TW_W32.OBJ         >> _NTDLL.CMD
echo VIDC32.OBJ         >> _NTDLL.CMD
echo VIDDC32.OBJ        >> _NTDLL.CMD
echo WINCOMAL.OBJ       >> _NTDLL.CMD
echo WINVER32.OBJ       >> _NTDLL.CMD

:builddll
echo /NOD /DLL /SUBSYSTEM:WINDOWS /IMPLIB:CP%1.LIB              >> _NTDLL.CMD
if %2 == NODEBUG goto nodebug
echo /DEBUG                                                     >> _NTDLL.CMD
:nodebug
echo /DEF:..\misc\CP%1.DEF /OUT:CP%1.DLL                        >> _NTDLL.CMD
echo libc.lib oldnames.lib kernel32.lib user32.lib gdi32.lib    >> _NTDLL.CMD
echo winspool.lib comdlg32.lib advapi32.lib shell32.lib         >> _NTDLL.CMD
if %3 == NOWIN32S goto tag3
if %2 == NODEBUG goto tag4
echo cp30fwd.lib w32sut32.lib                                   >> _NTDLL.CMD
goto tag3
:tag4
echo cp30fw.lib w32sut32.lib                                    >> _NTDLL.CMD
:tag3

link @_NTDLL.CMD

del _NTDLL.CMD

