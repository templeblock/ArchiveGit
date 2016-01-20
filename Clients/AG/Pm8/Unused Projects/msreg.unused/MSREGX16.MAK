# Microsoft Visual C++ generated build script - Do not modify

PROJ = MSREGX16
DEBUG = 0
PROGTYPE = 1
CALLER = test16.exe
ARGS = 
DLLS = 
D_RCDEFINES = /d_DEBUG 
R_RCDEFINES = /dNDEBUG 
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = D:\MSREG\
USEMFC = 1
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = /YcSTDAFX.H
CUSEPCHFLAG = 
CPPUSEPCHFLAG = /YuSTDAFX.H
FIRSTC =             
FIRSTCPP = STDAFX.CPP  
RC = rc
CFLAGS_D_WDLL = /nologo /G2 /W3 /Zi /ALw /Od /D "_DEBUG" /D "_USRDLL" /GD /Fd"MSREG16.PDB" 
CFLAGS_R_WDLL = /nologo /G3 /Gy /W3 /Gf /ALw /O1 /D "NDEBUG" /D "_USRDLL" /GD 
LFLAGS_D_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /SEG:256 /ALIGN:512 /ONERROR:NOEXE /CO /MAP /MAP:FULL
LFLAGS_R_WDLL = /NOLOGO /NOD /NOE /PACKC:61440 /SEG:256 /ALIGN:16 /ONERROR:NOEXE /MAP /MAP:FULL
LIBS_D_WDLL = lafxdwd .\gcpp300\msvc1x\_300d.lib oldnames libw ldllcew commdlg.lib mmsystem.lib olecli.lib olesvr.lib shell.lib 
LIBS_R_WDLL = lafxdw .\gcpp300\msvc1x\_300.lib oldnames libw ldllcew commdlg.lib mmsystem.lib olecli.lib olesvr.lib shell.lib 
RCFLAGS = /nologo 
RESFLAGS = /nologo 
RUNFLAGS = 
DEFFILE = MSREGX16.DEF
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WDLL)
LFLAGS = $(LFLAGS_D_WDLL)
LIBS = $(LIBS_D_WDLL)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WDLL)
LFLAGS = $(LFLAGS_R_WDLL)
LIBS = $(LIBS_R_WDLL)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = STDAFX.SBR \
		BYMAIL.SBR \
		CALLFAIL.SBR \
		COMLINK.SBR \
		CONGRATS.SBR \
		CWMFCTL.SBR \
		DIALOPTS.SBR \
		INIFILE.SBR \
		LATER.SBR \
		MSPACKET.SBR \
		NOMODEM.SBR \
		PHONENUM.SBR \
		PROPCTL.SBR \
		PROPSTR.SBR \
		REGINFO.SBR \
		REGISTER.SBR \
		REQUIRED.SBR \
		SEARCH.SBR \
		SEND.SBR \
		BMPCTL.SBR \
		UTIL.SBR \
		MSMODEM.SBR \
		MSSERIAL.SBR \
		CPROGBAR.SBR \
		MSENGINE.SBR \
		MSZMODEM.SBR \
		BYPHONE.SBR \
		MSREGX.SBR \
		BADCODE.SBR \
		REGBONUS.SBR \
		KEY.SBR \
		INSTALL.SBR \
		BASEDLG.SBR \
		CANTREG.SBR \
		CWMF.SBR \
		MLSPRINT.SBR \
		DDB.SBR \
		WHERERU.SBR \
		SYSINFO.SBR \
		REGFORM.SBR \
		INTLDLL.SBR


STDAFX_DEP = d:\msreg\stdafx.h


BYMAIL_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\msregini.h \
	d:\msreg\propctl.h \
	d:\msreg\propstr.h \
	d:\msreg\util.h \
	d:\msreg\regform.h \
	d:\msreg\cwmf.h \
	d:\msreg\mlsprint.h \
	d:\msreg\basedlg.h \
	d:\msreg\required.h \
	d:\msreg\phonenum.h \
	d:\msreg\bymail.h \
	d:\msreg\reginfo.h


CALLFAIL_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\callfail.h \
	d:\msreg\basedlg.h \
	d:\msreg\util.h \
	d:\msreg\propctl.h \
	d:\msreg\propstr.h


COMLINK_DEP = d:\msreg\stdafx.h \
	d:\msreg\comlink.h \
	d:\msreg\gcpp300\h\compplib.hpp \
	d:\msreg\gcpp300\h\cppcomp.h \
	d:\msreg\gcpp300\h\comdef.hpp \
	d:\msreg\gcpp300\h\asciidef.h \
	d:\msreg\gcpp300\h\_defs386.h \
	d:\msreg\gcpp300\h\compat20.h \
	d:\msreg\gcpp300\h\_comlink.hpp \
	d:\msreg\gcpp300\h\_dosisr.hpp \
	d:\msreg\gcpp300\h\_8250.hpp \
	d:\msreg\gcpp300\h\_star.hpp \
	d:\msreg\gcpp300\h\nasi.hpp


CONGRATS_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\congrats.h \
	d:\msreg\basedlg.h \
	d:\msreg\util.h \
	d:\msreg\propctl.h \
	d:\msreg\propstr.h


CWMFCTL_DEP = d:\msreg\stdafx.h \
	d:\msreg\cwmfctl.h \
	d:\msreg\cwmf.h \
	d:\msreg\propstr.h


DIALOPTS_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\msregini.h \
	d:\msreg\dialopts.h \
	d:\msreg\basedlg.h \
	d:\msreg\util.h \
	d:\msreg\propctl.h \
	d:\msreg\propstr.h


INIFILE_DEP = d:\msreg\stdafx.h \
	d:\msreg\inifile.h


LATER_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\later.h \
	d:\msreg\basedlg.h \
	d:\msreg\util.h \
	d:\msreg\propctl.h \
	d:\msreg\propstr.h


MSPACKET_DEP = d:\msreg\stdafx.h \
	d:\msreg\mspacket.h


NOMODEM_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\nomodem.h \
	d:\msreg\basedlg.h \
	d:\msreg\propctl.h \
	d:\msreg\propstr.h


PHONENUM_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\phonenum.h \
	d:\msreg\basedlg.h \
	d:\msreg\propctl.h \
	d:\msreg\propstr.h


PROPCTL_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\propctl.h \
	d:\msreg\propstr.h


PROPSTR_DEP = d:\msreg\stdafx.h \
	d:\msreg\util.h \
	d:\msreg\propstr.h


REGINFO_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\dialopts.h \
	d:\msreg\basedlg.h \
	d:\msreg\phonenum.h \
	d:\msreg\required.h \
	d:\msreg\msregini.h \
	d:\msreg\util.h \
	d:\msreg\reginfo.h \
	d:\msreg\propctl.h \
	d:\msreg\propstr.h


REGISTER_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\register.h \
	d:\msreg\basedlg.h \
	d:\msreg\reginfo.h \
	d:\msreg\sysinfo.h \
	d:\msreg\byphone.h \
	d:\msreg\bymail.h \
	d:\msreg\later.h \
	d:\msreg\send.h \
	d:\msreg\nomodem.h \
	d:\msreg\callfail.h \
	d:\msreg\cantreg.h \
	d:\msreg\mspacket.h \
	d:\msreg\util.h \
	d:\msreg\msregini.h \
	d:\msreg\propctl.h \
	d:\msreg\propstr.h \
	d:\msreg\congrats.h


REQUIRED_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\required.h \
	d:\msreg\basedlg.h \
	d:\msreg\propctl.h \
	d:\msreg\propstr.h


SEARCH_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\msmodem.h \
	d:\msreg\msserial.h \
	d:\msreg\gcpp300\h\compplib.hpp \
	d:\msreg\gcpp300\h\cppcomp.h \
	d:\msreg\gcpp300\h\comdef.hpp \
	d:\msreg\gcpp300\h\asciidef.h \
	d:\msreg\gcpp300\h\_defs386.h \
	d:\msreg\gcpp300\h\compat20.h \
	d:\msreg\gcpp300\h\_comlink.hpp \
	d:\msreg\gcpp300\h\_dosisr.hpp \
	d:\msreg\gcpp300\h\_8250.hpp \
	d:\msreg\gcpp300\h\_star.hpp \
	d:\msreg\gcpp300\h\nasi.hpp \
	d:\msreg\comlink.h \
	d:\msreg\util.h \
	d:\msreg\propctl.h \
	d:\msreg\propstr.h


SEND_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\send.h \
	d:\msreg\basedlg.h \
	d:\msreg\msmodem.h \
	d:\msreg\msserial.h \
	d:\msreg\gcpp300\h\compplib.hpp \
	d:\msreg\gcpp300\h\cppcomp.h \
	d:\msreg\gcpp300\h\comdef.hpp \
	d:\msreg\gcpp300\h\asciidef.h \
	d:\msreg\gcpp300\h\_defs386.h \
	d:\msreg\gcpp300\h\compat20.h \
	d:\msreg\gcpp300\h\_comlink.hpp \
	d:\msreg\gcpp300\h\_dosisr.hpp \
	d:\msreg\gcpp300\h\_8250.hpp \
	d:\msreg\gcpp300\h\_star.hpp \
	d:\msreg\gcpp300\h\nasi.hpp \
	d:\msreg\comlink.h \
	d:\msreg\msregini.h \
	d:\msreg\gcpp300\h\filexfer.hpp \
	d:\msreg\gcpp300\h\_engine.hpp \
	d:\msreg\util.h \
	d:\msreg\propctl.h \
	d:\msreg\propstr.h \
	d:\msreg\cprogbar.h \
	d:\msreg\msengine.h \
	d:\msreg\mszmodem.h


BMPCTL_DEP = d:\msreg\stdafx.h \
	d:\msreg\bmpctl.h \
	d:\msreg\ddb.h \
	d:\msreg\util.h \
	d:\msvc\mfc\src\auxdata.h


UTIL_DEP = d:\msreg\stdafx.h \
	d:\msreg\util.h


MSMODEM_DEP = d:\msreg\stdafx.h \
	d:\msreg\msmodem.h \
	d:\msreg\msserial.h \
	d:\msreg\gcpp300\h\compplib.hpp \
	d:\msreg\gcpp300\h\cppcomp.h \
	d:\msreg\gcpp300\h\comdef.hpp \
	d:\msreg\gcpp300\h\asciidef.h \
	d:\msreg\gcpp300\h\_defs386.h \
	d:\msreg\gcpp300\h\compat20.h \
	d:\msreg\gcpp300\h\_comlink.hpp \
	d:\msreg\gcpp300\h\_dosisr.hpp \
	d:\msreg\gcpp300\h\_8250.hpp \
	d:\msreg\gcpp300\h\_star.hpp \
	d:\msreg\gcpp300\h\nasi.hpp \
	d:\msreg\comlink.h


MSSERIAL_DEP = d:\msreg\stdafx.h \
	d:\msreg\msserial.h \
	d:\msreg\gcpp300\h\compplib.hpp \
	d:\msreg\gcpp300\h\cppcomp.h \
	d:\msreg\gcpp300\h\comdef.hpp \
	d:\msreg\gcpp300\h\asciidef.h \
	d:\msreg\gcpp300\h\_defs386.h \
	d:\msreg\gcpp300\h\compat20.h \
	d:\msreg\gcpp300\h\_comlink.hpp \
	d:\msreg\gcpp300\h\_dosisr.hpp \
	d:\msreg\gcpp300\h\_8250.hpp \
	d:\msreg\gcpp300\h\_star.hpp \
	d:\msreg\gcpp300\h\nasi.hpp


CPROGBAR_DEP = d:\msreg\stdafx.h \
	d:\msreg\cprogbar.h


MSENGINE_DEP = d:\msreg\stdafx.h \
	d:\msreg\msengine.h \
	d:\msreg\gcpp300\h\compplib.hpp \
	d:\msreg\gcpp300\h\cppcomp.h \
	d:\msreg\gcpp300\h\comdef.hpp \
	d:\msreg\gcpp300\h\asciidef.h \
	d:\msreg\gcpp300\h\_defs386.h \
	d:\msreg\gcpp300\h\compat20.h \
	d:\msreg\gcpp300\h\_comlink.hpp \
	d:\msreg\gcpp300\h\_dosisr.hpp \
	d:\msreg\gcpp300\h\_8250.hpp \
	d:\msreg\gcpp300\h\_star.hpp \
	d:\msreg\gcpp300\h\nasi.hpp \
	d:\msreg\gcpp300\h\filexfer.hpp


MSZMODEM_DEP = d:\msreg\stdafx.h \
	d:\msreg\mszmodem.h \
	d:\msreg\gcpp300\h\compplib.hpp \
	d:\msreg\gcpp300\h\cppcomp.h \
	d:\msreg\gcpp300\h\comdef.hpp \
	d:\msreg\gcpp300\h\asciidef.h \
	d:\msreg\gcpp300\h\_defs386.h \
	d:\msreg\gcpp300\h\compat20.h \
	d:\msreg\gcpp300\h\_comlink.hpp \
	d:\msreg\gcpp300\h\_dosisr.hpp \
	d:\msreg\gcpp300\h\_8250.hpp \
	d:\msreg\gcpp300\h\_star.hpp \
	d:\msreg\gcpp300\h\nasi.hpp \
	d:\msreg\gcpp300\h\filexfer.hpp


BYPHONE_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\msregini.h \
	d:\msreg\propctl.h \
	d:\msreg\propstr.h \
	d:\msreg\util.h \
	d:\msreg\byphone.h \
	d:\msreg\basedlg.h


MSREGX_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\bmpctl.h \
	d:\msreg\cwmfctl.h \
	d:\msreg\cwmf.h \
	d:\msreg\cprogbar.h \
	d:\msreg\whereru.h \
	d:\msreg\reginfo.h \
	d:\msreg\basedlg.h \
	d:\msreg\register.h \
	d:\msreg\congrats.h \
	d:\msreg\badcode.h \
	d:\msreg\msregini.h \
	d:\msreg\util.h \
	d:\msreg\install.h \
	d:\msreg\regbonus.h \
	d:\msreg\key.h \
	d:\msreg\already.h


MSREGX_RCDEP = d:\msreg\res\invite.bmp \
	d:\msreg\res\huh.bmp \
	d:\msreg\res\wavebye.bmp \
	d:\msreg\res\thanks.bmp \
	d:\msreg\res\delivery.bmp \
	d:\msreg\res\sherlock.bmp \
	d:\msreg\res\dogwrite.bmp \
	d:\msreg\res\womanclp.bmp \
	d:\msreg\res\phone.bmp \
	d:\msreg\res\strike.bmp \
	d:\msreg\res\sysinfo.bmp \
	d:\msreg\res\msregdll.rc2


BADCODE_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\msregini.h \
	d:\msreg\propctl.h \
	d:\msreg\propstr.h \
	d:\msreg\badcode.h \
	d:\msreg\basedlg.h


REGBONUS_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\propctl.h \
	d:\msreg\propstr.h \
	d:\msreg\regbonus.h \
	d:\msreg\basedlg.h


KEY_DEP = d:\msreg\stdafx.h \
	d:\msreg\key.h


INSTALL_DEP = d:\msreg\stdafx.h \
	d:\msreg\install.h \
	d:\msreg\util.h


BASEDLG_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\basedlg.h \
	d:\msvc\mfc\src\auxdata.h


CANTREG_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\msregini.h \
	d:\msreg\propctl.h \
	d:\msreg\propstr.h \
	d:\msreg\cantreg.h \
	d:\msreg\basedlg.h


CWMF_DEP = d:\msreg\stdafx.h \
	d:\msreg\cwmf.h


MLSPRINT_DEP = d:\msreg\stdafx.h \
	d:\msreg\mlsprint.h \
	d:\msreg\basedlg.h


DDB_DEP = d:\msreg\stdafx.h \
	d:\msreg\ddb.h


WHERERU_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\msregini.h \
	d:\msreg\util.h \
	d:\msreg\propctl.h \
	d:\msreg\propstr.h \
	d:\msreg\whereru.h \
	d:\msreg\reginfo.h \
	d:\msreg\basedlg.h


SYSINFO_DEP = d:\msreg\stdafx.h \
	d:\msreg\msregx.h \
	d:\msreg\intl\intldll.h \
	d:\msreg\inifile.h \
	d:\msreg\msreg.h \
	d:\msreg\sysinfo.h \
	d:\msreg\basedlg.h \
	d:\msreg\propctl.h \
	d:\msreg\propstr.h


REGFORM_DEP = d:\msreg\stdafx.h \
	d:\msreg\mlsprint.h \
	d:\msreg\basedlg.h \
	d:\msreg\regform.h \
	d:\msreg\cwmf.h


INTLDLL_DEP = d:\msreg\intl\stdafx.h \
	d:\msreg\intl\intldll.h


all:	$(PROJ).DLL

STDAFX.OBJ:	STDAFX.CPP $(STDAFX_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c STDAFX.CPP

BYMAIL.OBJ:	BYMAIL.CPP $(BYMAIL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c BYMAIL.CPP

CALLFAIL.OBJ:	CALLFAIL.CPP $(CALLFAIL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CALLFAIL.CPP

COMLINK.OBJ:	COMLINK.CPP $(COMLINK_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c COMLINK.CPP

CONGRATS.OBJ:	CONGRATS.CPP $(CONGRATS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CONGRATS.CPP

CWMFCTL.OBJ:	CWMFCTL.CPP $(CWMFCTL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CWMFCTL.CPP

DIALOPTS.OBJ:	DIALOPTS.CPP $(DIALOPTS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DIALOPTS.CPP

INIFILE.OBJ:	INIFILE.CPP $(INIFILE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c INIFILE.CPP

LATER.OBJ:	LATER.CPP $(LATER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c LATER.CPP

MSPACKET.OBJ:	MSPACKET.CPP $(MSPACKET_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MSPACKET.CPP

NOMODEM.OBJ:	NOMODEM.CPP $(NOMODEM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c NOMODEM.CPP

PHONENUM.OBJ:	PHONENUM.CPP $(PHONENUM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PHONENUM.CPP

PROPCTL.OBJ:	PROPCTL.CPP $(PROPCTL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PROPCTL.CPP

PROPSTR.OBJ:	PROPSTR.CPP $(PROPSTR_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c PROPSTR.CPP

REGINFO.OBJ:	REGINFO.CPP $(REGINFO_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c REGINFO.CPP

REGISTER.OBJ:	REGISTER.CPP $(REGISTER_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c REGISTER.CPP

REQUIRED.OBJ:	REQUIRED.CPP $(REQUIRED_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c REQUIRED.CPP

SEARCH.OBJ:	SEARCH.CPP $(SEARCH_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SEARCH.CPP

SEND.OBJ:	SEND.CPP $(SEND_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SEND.CPP

BMPCTL.OBJ:	BMPCTL.CPP $(BMPCTL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c BMPCTL.CPP

UTIL.OBJ:	UTIL.CPP $(UTIL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c UTIL.CPP

MSMODEM.OBJ:	MSMODEM.CPP $(MSMODEM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MSMODEM.CPP

MSSERIAL.OBJ:	MSSERIAL.CPP $(MSSERIAL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MSSERIAL.CPP

CPROGBAR.OBJ:	CPROGBAR.CPP $(CPROGBAR_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CPROGBAR.CPP

MSENGINE.OBJ:	MSENGINE.CPP $(MSENGINE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MSENGINE.CPP

MSZMODEM.OBJ:	MSZMODEM.CPP $(MSZMODEM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MSZMODEM.CPP

BYPHONE.OBJ:	BYPHONE.CPP $(BYPHONE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c BYPHONE.CPP

MSREGX.OBJ:	MSREGX.CPP $(MSREGX_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MSREGX.CPP

MSREGX.RES:	MSREGX.RC $(MSREGX_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r MSREGX.RC

BADCODE.OBJ:	BADCODE.CPP $(BADCODE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c BADCODE.CPP

REGBONUS.OBJ:	REGBONUS.CPP $(REGBONUS_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c REGBONUS.CPP

KEY.OBJ:	KEY.CPP $(KEY_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c KEY.CPP

INSTALL.OBJ:	INSTALL.CPP $(INSTALL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c INSTALL.CPP

BASEDLG.OBJ:	BASEDLG.CPP $(BASEDLG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c BASEDLG.CPP

CANTREG.OBJ:	CANTREG.CPP $(CANTREG_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CANTREG.CPP

CWMF.OBJ:	CWMF.CPP $(CWMF_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c CWMF.CPP

MLSPRINT.OBJ:	MLSPRINT.CPP $(MLSPRINT_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c MLSPRINT.CPP

DDB.OBJ:	DDB.CPP $(DDB_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c DDB.CPP

WHERERU.OBJ:	WHERERU.CPP $(WHERERU_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c WHERERU.CPP

SYSINFO.OBJ:	SYSINFO.CPP $(SYSINFO_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SYSINFO.CPP

REGFORM.OBJ:	REGFORM.CPP $(REGFORM_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c REGFORM.CPP

INTLDLL.OBJ:	INTL\INTLDLL.CPP $(INTLDLL_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c INTL\INTLDLL.CPP


$(PROJ).DLL::	MSREGX.RES

$(PROJ).DLL::	STDAFX.OBJ BYMAIL.OBJ CALLFAIL.OBJ COMLINK.OBJ CONGRATS.OBJ CWMFCTL.OBJ \
	DIALOPTS.OBJ INIFILE.OBJ LATER.OBJ MSPACKET.OBJ NOMODEM.OBJ PHONENUM.OBJ PROPCTL.OBJ \
	PROPSTR.OBJ REGINFO.OBJ REGISTER.OBJ REQUIRED.OBJ SEARCH.OBJ SEND.OBJ BMPCTL.OBJ \
	UTIL.OBJ MSMODEM.OBJ MSSERIAL.OBJ CPROGBAR.OBJ MSENGINE.OBJ MSZMODEM.OBJ BYPHONE.OBJ \
	MSREGX.OBJ BADCODE.OBJ REGBONUS.OBJ KEY.OBJ INSTALL.OBJ BASEDLG.OBJ CANTREG.OBJ CWMF.OBJ \
	MLSPRINT.OBJ DDB.OBJ WHERERU.OBJ SYSINFO.OBJ REGFORM.OBJ INTLDLL.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
STDAFX.OBJ +
BYMAIL.OBJ +
CALLFAIL.OBJ +
COMLINK.OBJ +
CONGRATS.OBJ +
CWMFCTL.OBJ +
DIALOPTS.OBJ +
INIFILE.OBJ +
LATER.OBJ +
MSPACKET.OBJ +
NOMODEM.OBJ +
PHONENUM.OBJ +
PROPCTL.OBJ +
PROPSTR.OBJ +
REGINFO.OBJ +
REGISTER.OBJ +
REQUIRED.OBJ +
SEARCH.OBJ +
SEND.OBJ +
BMPCTL.OBJ +
UTIL.OBJ +
MSMODEM.OBJ +
MSSERIAL.OBJ +
CPROGBAR.OBJ +
MSENGINE.OBJ +
MSZMODEM.OBJ +
BYPHONE.OBJ +
MSREGX.OBJ +
BADCODE.OBJ +
REGBONUS.OBJ +
KEY.OBJ +
INSTALL.OBJ +
BASEDLG.OBJ +
CANTREG.OBJ +
CWMF.OBJ +
MLSPRINT.OBJ +
DDB.OBJ +
WHERERU.OBJ +
SYSINFO.OBJ +
REGFORM.OBJ +
INTLDLL.OBJ +
$(OBJS_EXT)
$(PROJ).DLL
$(MAPFILE)
d:\msvc\lib\+
d:\msvc\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) MSREGX.RES $@
	@copy $(PROJ).CRF MSVC.BND
	implib /nowep $(PROJ).LIB $(PROJ).DLL

$(PROJ).DLL::	MSREGX.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) MSREGX.RES $@

run: $(PROJ).DLL
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
