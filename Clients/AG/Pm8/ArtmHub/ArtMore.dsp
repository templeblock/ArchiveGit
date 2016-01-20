# Microsoft Developer Studio Project File - Name="artmore" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=artmore - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "artmore.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "artmore.mak" CFG="artmore - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "artmore - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "artmore - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/artmore", UYAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "artmore - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\asRel"
# PROP Intermediate_Dir ".\asRel"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /Zp2 /MD /W3 /GX /Ot /Oi /Oy /Ob1 /Gf /Gy /D "NDEBUG" /D "_AFXDLL" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "NEW_THUMBNAILS" /D "NEW_COMPRESSED_CONTENT" /D "ARTSTORE" /Yu"STDAFX.H" /FD /c
# SUBTRACT CPP /Ox /Oa /Ow /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 app/al201/v4x/alfw.lib lib/implod32.lib winmm.lib mpr.lib smacker\smkw32ms.lib app\pegasus\picnm.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libcmt" /out:".\asRel/pmw.exe"
# SUBTRACT LINK32 /debug /nodefaultlib

!ELSEIF  "$(CFG)" == "artmore - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\asDebug"
# PROP Intermediate_Dir ".\asDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /Zp2 /MDd /W3 /GX /ZI /Od /D "_DEBUG" /D "_AFXDLL" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "NEW_THUMBNAILS" /D "NEW_COMPRESSED_CONTENT" /D "ARTSTORE" /Yu"STDAFX.H" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 app/al201/v4x/alfwd.lib lib/implod32.lib winmm.lib mpr.lib smacker\smkw32ms.lib app\pegasus\picnm.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libcmtd" /out:".\asDebug/pmw.exe"
# SUBTRACT LINK32 /profile /map /nodefaultlib

!ENDIF 

# Begin Target

# Name "artmore - Win32 Release"
# Name "artmore - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Aboutdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\AcctInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ActAcDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Addcalpg.cpp
# End Source File
# Begin Source File

SOURCE=.\addprnvw.cpp
# End Source File
# Begin Source File

SOURCE=.\addrbook.cpp
# End Source File
# Begin Source File

SOURCE=.\ADDRDATA.CPP
# End Source File
# Begin Source File

SOURCE=.\addredit.cpp
# End Source File
# Begin Source File

SOURCE=.\addrimp.cpp
# End Source File
# Begin Source File

SOURCE=.\AddrInfoPage.cpp
# End Source File
# Begin Source File

SOURCE=.\addrlist.cpp
# End Source File
# Begin Source File

SOURCE=.\AdLocReq.cpp
# End Source File
# Begin Source File

SOURCE=.\adoninst.cpp
# End Source File
# Begin Source File

SOURCE=.\aprogres.CPP
# End Source File
# Begin Source File

SOURCE=.\Array.cpp
# End Source File
# Begin Source File

SOURCE=.\Arrayrec.cpp
# End Source File
# Begin Source File

SOURCE=.\ArtCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\ArtExistsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\artstore.cpp
# End Source File
# Begin Source File

SOURCE=.\Autoexec.cpp
# End Source File
# Begin Source File

SOURCE=.\automate.cpp
# End Source File
# Begin Source File

SOURCE=.\Backobj.cpp
# End Source File
# Begin Source File

SOURCE=.\Banndoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Bannview.cpp
# End Source File
# Begin Source File

SOURCE=.\Barcombo.cpp
# End Source File
# Begin Source File

SOURCE=.\Barmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\BasReqIn.cpp
# End Source File
# Begin Source File

SOURCE=.\bfinit.cpp
# End Source File
# Begin Source File

SOURCE=.\BillingInfoPage.cpp
# End Source File
# Begin Source File

SOURCE=.\BITBTN.CPP
# End Source File
# Begin Source File

SOURCE=.\bkgobj.CPP
# End Source File
# Begin Source File

SOURCE=.\blowfish.cpp
# End Source File
# Begin Source File

SOURCE=.\Bmp.cpp
# End Source File
# Begin Source File

SOURCE=.\bmparray.cpp
# End Source File
# Begin Source File

SOURCE=.\Bmpfile.cpp
# End Source File
# Begin Source File

SOURCE=.\BmpTiler.cpp
# End Source File
# Begin Source File

SOURCE=.\BmStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\Borders.cpp
# End Source File
# Begin Source File

SOURCE=.\BrdrDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BrdrObj.cpp
# End Source File
# Begin Source File

SOURCE=.\Breakup.cpp
# End Source File
# Begin Source File

SOURCE=.\BRODOC.CPP
# End Source File
# Begin Source File

SOURCE=.\broprint.CPP
# End Source File
# Begin Source File

SOURCE=.\BROVIEW.CPP
# End Source File
# Begin Source File

SOURCE=.\BrowDrop.cpp
# End Source File
# Begin Source File

SOURCE=.\BROWSER.CPP
# End Source File
# Begin Source File

SOURCE=.\Browsupp.cpp
# End Source File
# Begin Source File

SOURCE=.\BtnNRect.cpp
# End Source File
# Begin Source File

SOURCE=.\BTNWND.CPP
# End Source File
# Begin Source File

SOURCE=.\cache.cpp
# End Source File
# Begin Source File

SOURCE=.\Calbuild.cpp
# End Source File
# Begin Source File

SOURCE=.\Calcalc.cpp
# End Source File
# Begin Source File

SOURCE=.\CalDateD.cpp
# End Source File
# Begin Source File

SOURCE=.\Caldoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Calendar.cpp
# End Source File
# Begin Source File

SOURCE=.\Calobj.cpp
# End Source File
# Begin Source File

SOURCE=.\CalPicD.cpp
# End Source File
# Begin Source File

SOURCE=.\CalProgD.cpp
# End Source File
# Begin Source File

SOURCE=.\calrec.cpp
# End Source File
# Begin Source File

SOURCE=.\Calstyle.cpp
# End Source File
# Begin Source File

SOURCE=.\CalTypeD.cpp
# End Source File
# Begin Source File

SOURCE=.\Calview.cpp
# End Source File
# Begin Source File

SOURCE=.\Carddoc.cpp
# End Source File
# Begin Source File

SOURCE=.\CardSrvr.cpp
# End Source File
# Begin Source File

SOURCE=.\Cardview.cpp
# End Source File
# Begin Source File

SOURCE=.\CARRAY.CPP
# End Source File
# Begin Source File

SOURCE=.\Catalog.cpp
# End Source File
# Begin Source File

SOURCE=.\CatOrgnz.cpp
# End Source File
# Begin Source File

SOURCE=.\Ccommand.cpp
# End Source File
# Begin Source File

SOURCE=.\Cdcache.cpp
# End Source File
# Begin Source File

SOURCE=.\cdeflate.CPP
# End Source File
# Begin Source File

SOURCE=.\cdemodlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Cdibdc.cpp
# End Source File
# Begin Source File

SOURCE=.\CDReq.cpp
# End Source File
# Begin Source File

SOURCE=.\celctl.CPP
# End Source File
# Begin Source File

SOURCE=.\celobj.cpp
# End Source File
# Begin Source File

SOURCE=.\celoutd.cpp
# End Source File
# Begin Source File

SOURCE=.\Cfixed.cpp
# End Source File
# Begin Source File

SOURCE=.\Cgm.cpp
# End Source File
# Begin Source File

SOURCE=.\CheckArtExists.cpp
# End Source File
# Begin Source File

SOURCE=.\Cimplode.cpp
# End Source File
# Begin Source File

SOURCE=.\CINDEX.CPP
# End Source File
# Begin Source File

SOURCE=.\Clip.cpp
# End Source File
# Begin Source File

SOURCE=.\Cmfaddon.cpp
# End Source File
# Begin Source File

SOURCE=.\CnfrmRm.cpp
# End Source File
# Begin Source File

SOURCE=.\Cntritem.cpp
# End Source File
# Begin Source File

SOURCE=.\COLLBLD.CPP
# End Source File
# Begin Source File

SOURCE=.\COLLCTG.CPP
# End Source File
# Begin Source File

SOURCE=.\COLLFILE.CPP
# End Source File
# Begin Source File

SOURCE=.\COLLIMP.CPP
# End Source File
# Begin Source File

SOURCE=.\COLLMGR.CPP
# End Source File
# Begin Source File

SOURCE=.\CollReq.cpp
# End Source File
# Begin Source File

SOURCE=.\CollSel.cpp
# End Source File
# Begin Source File

SOURCE=.\Color.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorCbo.cpp
# End Source File
# Begin Source File

SOURCE=.\Colorsel.cpp
# End Source File
# Begin Source File

SOURCE=.\Command.cpp
# End Source File
# Begin Source File

SOURCE=.\Commands.cpp
# End Source File
# Begin Source File

SOURCE=.\Comp32.cpp
# End Source File
# Begin Source File

SOURCE=.\Compfn.cpp
# End Source File
# Begin Source File

SOURCE=.\ConnMgr.CPP
# End Source File
# Begin Source File

SOURCE=.\Connset.cpp
# End Source File
# Begin Source File

SOURCE=.\ContMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Convert.cpp
# End Source File
# Begin Source File

SOURCE=.\Cprogbar.cpp
# End Source File
# Begin Source File

SOURCE=.\CraftDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Craster.cpp
# End Source File
# Begin Source File

SOURCE=.\credit.cpp
# End Source File
# Begin Source File

SOURCE=.\CrPGSAcc.cpp
# End Source File
# Begin Source File

SOURCE=.\CTWAIN.CPP
# End Source File
# Begin Source File

SOURCE=.\ctxp.cpp
# End Source File
# Begin Source File

SOURCE=.\CustBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Cutpaste.cpp
# End Source File
# Begin Source File

SOURCE=.\Cwmf.cpp
# End Source File
# Begin Source File

SOURCE=.\Cwmfctl.cpp
# End Source File
# Begin Source File

SOURCE=.\Database.cpp
# End Source File
# Begin Source File

SOURCE=.\Dateobj.cpp
# End Source File
# Begin Source File

SOURCE=.\Dbmap.cpp
# End Source File
# Begin Source File

SOURCE=.\dbtabs.cpp
# End Source File
# Begin Source File

SOURCE=.\Ddb.cpp
# End Source File
# Begin Source File

SOURCE=.\Delname.cpp
# End Source File
# Begin Source File

SOURCE=.\Demodlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Desktop.cpp
# End Source File
# Begin Source File

SOURCE=.\dib2file.cpp
# End Source File
# Begin Source File

SOURCE=.\Dibbrush.cpp
# End Source File
# Begin Source File

SOURCE=.\Disptext.cpp
# End Source File
# Begin Source File

SOURCE=.\Dither.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlgmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\DLGURL.CPP
# End Source File
# Begin Source File

SOURCE=.\Dlist.cpp
# End Source File
# Begin Source File

SOURCE=.\Doccmd.cpp
# End Source File
# Begin Source File

SOURCE=.\docdib.CPP
# End Source File
# Begin Source File

SOURCE=.\DocDump.cpp
# End Source File
# Begin Source File

SOURCE=.\Docedit.cpp
# End Source File
# Begin Source File

SOURCE=.\Docpict.cpp
# End Source File
# Begin Source File

SOURCE=.\Docrec.cpp
# End Source File
# Begin Source File

SOURCE=.\Drawobj.cpp
# End Source File
# Begin Source File

SOURCE=.\Drivemgr.cpp
# End Source File
# Begin Source File

SOURCE=.\DsnCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\Dynarray.cpp
# End Source File
# Begin Source File

SOURCE=.\effects.cpp
# End Source File
# Begin Source File

SOURCE=.\Efilerec.cpp
# End Source File
# Begin Source File

SOURCE=.\Elliobj.cpp
# End Source File
# Begin Source File

SOURCE=.\enumproc.cpp
# End Source File
# Begin Source File

SOURCE=.\Envdoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Envprint.cpp
# End Source File
# Begin Source File

SOURCE=.\Envprvw.cpp
# End Source File
# Begin Source File

SOURCE=.\Envtype.cpp
# End Source File
# Begin Source File

SOURCE=.\Envview.cpp
# End Source File
# Begin Source File

SOURCE=.\Errdiff.cpp
# End Source File
# Begin Source File

SOURCE=.\Error.cpp
# End Source File
# Begin Source File

SOURCE=.\exgraf.cpp
# End Source File
# Begin Source File

SOURCE=.\Fcache.cpp
# End Source File
# Begin Source File

SOURCE=.\Fdirrec.cpp
# End Source File
# Begin Source File

SOURCE=.\Ffldfile.cpp
# End Source File
# Begin Source File

SOURCE=.\Fielddlg.cpp
# End Source File
# Begin Source File

SOURCE=.\filcmprs.cpp
# End Source File
# Begin Source File

SOURCE=.\File.cpp
# End Source File
# Begin Source File

SOURCE=.\Filedb.cpp
# End Source File
# Begin Source File

SOURCE=.\filemgr.CPP
# End Source File
# Begin Source File

SOURCE=.\fill.cpp
# End Source File
# Begin Source File

SOURCE=.\findrep.cpp
# End Source File
# Begin Source File

SOURCE=.\FindText.cpp
# End Source File
# Begin Source File

SOURCE=.\flatdb.cpp
# End Source File
# Begin Source File

SOURCE=.\Flexmath.cpp
# End Source File
# Begin Source File

SOURCE=.\Flistrec.cpp
# End Source File
# Begin Source File

SOURCE=.\Fontrec.cpp
# End Source File
# Begin Source File

SOURCE=.\Framedit.cpp
# End Source File
# Begin Source File

SOURCE=.\framedlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Frameobj.cpp
# End Source File
# Begin Source File

SOURCE=.\Framerec.cpp
# End Source File
# Begin Source File

SOURCE=.\FREELIST.CPP
# End Source File
# Begin Source File

SOURCE=.\Gif.cpp
# End Source File
# Begin Source File

SOURCE=.\Glicense.cpp
# End Source File
# Begin Source File

SOURCE=.\Globatom.cpp
# End Source File
# Begin Source File

SOURCE=.\gohtml.cpp
# End Source File
# Begin Source File

SOURCE=.\GradDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Grafobj.cpp
# End Source File
# Begin Source File

SOURCE=.\Grafrec.cpp
# End Source File
# Begin Source File

SOURCE=.\GrLocReq.cpp
# End Source File
# Begin Source File

SOURCE=.\GrphReq.cpp
# End Source File
# Begin Source File

SOURCE=.\Grpobj.cpp
# End Source File
# Begin Source File

SOURCE=.\GUID.CPP
# End Source File
# Begin Source File

SOURCE=.\Halo.cpp
# End Source File
# Begin Source File

SOURCE=.\HelpHint.cpp
# End Source File
# Begin Source File

SOURCE=.\hhctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\htmlconv.cpp
# End Source File
# Begin Source File

SOURCE=.\Htmldoc.cpp
# End Source File
# Begin Source File

SOURCE=.\hubmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\hyperrec.cpp
# End Source File
# Begin Source File

SOURCE=.\hyplkdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Image.cpp
# End Source File
# Begin Source File

SOURCE=.\Imagebuf.cpp
# End Source File
# Begin Source File

SOURCE=.\Imagedrv.cpp
# End Source File
# Begin Source File

SOURCE=.\imagemap.cpp
# End Source File
# Begin Source File

SOURCE=.\Imagesrc.cpp
# End Source File
# Begin Source File

SOURCE=.\inetconn.CPP
# End Source File
# Begin Source File

SOURCE=.\Inifile.cpp
# End Source File
# Begin Source File

SOURCE=.\Instdat.cpp
# End Source File
# Begin Source File

SOURCE=.\Ipframe.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg.CPP
# End Source File
# Begin Source File

SOURCE=.\JPEGIMPL.CPP
# End Source File
# Begin Source File

SOURCE=.\KEY.CPP
# End Source File
# Begin Source File

SOURCE=.\Labeldlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Labels.cpp
# End Source File
# Begin Source File

SOURCE=.\Layrec.cpp
# End Source File
# Begin Source File

SOURCE=.\Lbldoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Lblprint.cpp
# End Source File
# Begin Source File

SOURCE=.\Lblprvw.cpp
# End Source File
# Begin Source File

SOURCE=.\Lblview.cpp
# End Source File
# Begin Source File

SOURCE=.\licdll.cpp
# End Source File
# Begin Source File

SOURCE=.\License.cpp
# End Source File
# Begin Source File

SOURCE=.\LINECROS.CPP
# End Source File
# Begin Source File

SOURCE=.\Lineobj.cpp
# End Source File
# Begin Source File

SOURCE=.\Linewid.cpp
# End Source File
# Begin Source File

SOURCE=.\List.cpp
# End Source File
# Begin Source File

SOURCE=.\LoginReq.cpp
# End Source File
# Begin Source File

SOURCE=.\Lzw.cpp
# End Source File
# Begin Source File

SOURCE=.\Macro.cpp
# End Source File
# Begin Source File

SOURCE=.\Macrodlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Macrorec.cpp
# End Source File
# Begin Source File

SOURCE=.\Macrostd.cpp
# End Source File
# Begin Source File

SOURCE=.\Mainfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Marquee.cpp
# End Source File
# Begin Source File

SOURCE=.\Maskedbm.cpp
# End Source File
# Begin Source File

SOURCE=.\Mciwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Memdev.cpp
# End Source File
# Begin Source File

SOURCE=.\Modecrop.cpp
# End Source File
# Begin Source File

SOURCE=.\modecrt.cpp
# End Source File
# Begin Source File

SOURCE=.\Modedraw.cpp
# End Source File
# Begin Source File

SOURCE=.\MODELINK.CPP
# End Source File
# Begin Source File

SOURCE=.\Modesize.cpp
# End Source File
# Begin Source File

SOURCE=.\Modetext.cpp
# End Source File
# Begin Source File

SOURCE=.\Modezoom.cpp
# End Source File
# Begin Source File

SOURCE=.\MonthEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\Namerec.cpp
# End Source File
# Begin Source File

SOURCE=.\Netcount.cpp
# End Source File
# Begin Source File

SOURCE=.\Netid.cpp
# End Source File
# Begin Source File

SOURCE=.\Netusers.cpp
# End Source File
# Begin Source File

SOURCE=.\Network.cpp
# End Source File
# Begin Source File

SOURCE=.\Newdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\newfont.CPP
# End Source File
# Begin Source File

SOURCE=.\newsdoc.CPP
# End Source File
# Begin Source File

SOURCE=.\newsview.CPP
# End Source File
# Begin Source File

SOURCE=.\nfx.CPP
# End Source File
# Begin Source File

SOURCE=.\Nheap.cpp
# End Source File
# Begin Source File

SOURCE=.\Nofillin.cpp
# End Source File
# Begin Source File

SOURCE=.\NumberEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\NwsPrint.cpp
# End Source File
# Begin Source File

SOURCE=.\Objdb.cpp
# End Source File
# Begin Source File

SOURCE=.\Objimg.cpp
# End Source File
# Begin Source File

SOURCE=.\Objlist.cpp
# End Source File
# Begin Source File

SOURCE=.\Od.cpp
# End Source File
# Begin Source File

SOURCE=.\Olddoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Oleobj.cpp
# End Source File
# Begin Source File

SOURCE=.\OnAdDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\OpenDlgs.cpp
# End Source File
# Begin Source File

SOURCE=.\ORIENDLG.CPP
# End Source File
# Begin Source File

SOURCE=.\Outline.cpp
# End Source File
# Begin Source File

SOURCE=.\Outpath.cpp
# End Source File
# Begin Source File

SOURCE=.\Ownercb.cpp
# End Source File
# Begin Source File

SOURCE=.\Ownerlb.cpp
# End Source File
# Begin Source File

SOURCE=.\pagedlgs.cpp
# End Source File
# Begin Source File

SOURCE=.\Pageprop.cpp
# End Source File
# Begin Source File

SOURCE=.\Pagerec.cpp
# End Source File
# Begin Source File

SOURCE=.\Paint.cpp
# End Source File
# Begin Source File

SOURCE=.\PANEL.CPP
# End Source File
# Begin Source File

SOURCE=.\Paper.cpp
# End Source File
# Begin Source File

SOURCE=.\PARADLG.CPP
# End Source File
# Begin Source File

SOURCE=.\Pararec.cpp
# End Source File
# Begin Source File

SOURCE=.\Parse.cpp
# End Source File
# Begin Source File

SOURCE=.\Pathmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\Pathrec.cpp
# End Source File
# Begin Source File

SOURCE=.\Patobj.cpp
# End Source File
# Begin Source File

SOURCE=.\Pcd.cpp
# End Source File
# Begin Source File

SOURCE=.\pcrdprnt.cpp
# End Source File
# Begin Source File

SOURCE=.\Pcx.cpp
# End Source File
# Begin Source File

SOURCE=.\PDLGCLR.CPP
# End Source File
# Begin Source File

SOURCE=.\PDPropSt.cpp
# End Source File
# Begin Source File

SOURCE=.\PEGASUS.CPP
# End Source File
# Begin Source File

SOURCE=.\PGSSess.cpp
# End Source File
# Begin Source File

SOURCE=.\Pictattr.cpp
# End Source File
# Begin Source File

SOURCE=.\Pictprvw.cpp
# End Source File
# Begin Source File

SOURCE=.\pmaodlg.CPP
# End Source File
# Begin Source File

SOURCE=.\pmbmbut.cpp
# End Source File
# Begin Source File

SOURCE=.\Pmcfile.cpp
# End Source File
# Begin Source File

SOURCE=.\Pmdirdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Pmgdb.cpp
# End Source File
# Begin Source File

SOURCE=.\Pmgfont.cpp
# End Source File
# Begin Source File

SOURCE=.\Pmgobj.cpp
# End Source File
# Begin Source File

SOURCE=.\pmhelp.cpp
# End Source File
# Begin Source File

SOURCE=.\Pmw.cpp
# End Source File
# Begin Source File

SOURCE=.\Pmwcfg.cpp
# End Source File
# Begin Source File

SOURCE=.\PMWCOLL.CPP
# End Source File
# Begin Source File

SOURCE=.\Pmwcolor.cpp
# End Source File
# Begin Source File

SOURCE=.\PMWDISP.ODL
# End Source File
# Begin Source File

SOURCE=.\Pmwdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Pmwdoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Pmwmdi.cpp
# End Source File
# Begin Source File

SOURCE=.\Pmwprint.cpp
# End Source File
# Begin Source File

SOURCE=.\Pmwprop.cpp
# End Source File
# Begin Source File

SOURCE=.\PMWRFL.CPP
# End Source File
# Begin Source File

SOURCE=.\Pmwtempl.cpp
# End Source File
# Begin Source File

SOURCE=.\PMWTOOL.CPP
# End Source File
# Begin Source File

SOURCE=.\Pmwview.cpp
# End Source File
# Begin Source File

SOURCE=.\PMWWIZ.CPP
# End Source File
# Begin Source File

SOURCE=.\PNGIMPL.CPP
# End Source File
# Begin Source File

SOURCE=.\Polypoly.cpp
# End Source File
# Begin Source File

SOURCE=.\POPUP.CPP
# End Source File
# Begin Source File

SOURCE=.\Posdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Position.cpp
# End Source File
# Begin Source File

SOURCE=.\poslist.cpp
# End Source File
# Begin Source File

SOURCE=.\postcard.cpp
# End Source File
# Begin Source File

SOURCE=.\PREFMISC.CPP
# End Source File
# Begin Source File

SOURCE=.\prefonl.cpp
# End Source File
# Begin Source File

SOURCE=.\Prefpsh.cpp
# End Source File
# Begin Source File

SOURCE=.\Prefsnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Prefwrk.cpp
# End Source File
# Begin Source File

SOURCE=.\Prevrec.cpp
# End Source File
# Begin Source File

SOURCE=.\Printadv.cpp
# End Source File
# Begin Source File

SOURCE=.\Printart.cpp
# End Source File
# Begin Source File

SOURCE=.\Printer.cpp
# End Source File
# Begin Source File

SOURCE=.\Printprj.cpp
# End Source File
# Begin Source File

SOURCE=.\PRINTWIZ.CPP
# End Source File
# Begin Source File

SOURCE=.\prnaddlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PrntCalD.cpp
# End Source File
# Begin Source File

SOURCE=.\Prntrnge.cpp
# End Source File
# Begin Source File

SOURCE=.\Prntsize.cpp
# End Source File
# Begin Source File

SOURCE=.\procloc.cpp
# End Source File
# Begin Source File

SOURCE=.\profile.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgBar.cpp
# End Source File
# Begin Source File

SOURCE=.\progress.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgStat.cpp
# End Source File
# Begin Source File

SOURCE=.\Project.cpp
# End Source File
# Begin Source File

SOURCE=.\projgrph.CPP
# End Source File
# Begin Source File

SOURCE=.\Projobj.cpp
# End Source File
# Begin Source File

SOURCE=.\Projtext.cpp
# End Source File
# Begin Source File

SOURCE=.\PromtDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Proplist.cpp
# End Source File
# Begin Source File

SOURCE=.\PROPSTR.CPP
# End Source File
# Begin Source File

SOURCE=.\Prvwdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PSD.CPP
# End Source File
# Begin Source File

SOURCE=.\QSORT.CPP
# End Source File
# Begin Source File

SOURCE=.\Recarray.cpp
# End Source File
# Begin Source File

SOURCE=.\Record.cpp
# End Source File
# Begin Source File

SOURCE=.\Rectobj.cpp
# End Source File
# Begin Source File

SOURCE=.\Redisp.cpp
# End Source File
# Begin Source File

SOURCE=.\Refresh.cpp
# End Source File
# Begin Source File

SOURCE=.\Register.cpp
# End Source File
# Begin Source File

SOURCE=.\Rm.cpp
# End Source File
# Begin Source File

SOURCE=.\RMAGCom.cpp
# End Source File
# Begin Source File

SOURCE=.\RmCraft.cpp
# End Source File
# Begin Source File

SOURCE=.\RmPrint.cpp
# End Source File
# Begin Source File

SOURCE=.\RmSpirit.cpp
# End Source File
# Begin Source File

SOURCE=.\Rotdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Rotutils.cpp
# End Source File
# Begin Source File

SOURCE=.\RTFPARSE.CPP
# End Source File
# Begin Source File

SOURCE=.\Ruler.cpp
# End Source File
# Begin Source File

SOURCE=.\Rwsem.cpp
# End Source File
# Begin Source File

SOURCE=.\save2pic.cpp
# End Source File
# Begin Source File

SOURCE=.\SAVEPROJ.CPP
# End Source File
# Begin Source File

SOURCE=.\scrollga.cpp
# End Source File
# Begin Source File

SOURCE=.\Shapedlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Shapesel.cpp
# End Source File
# Begin Source File

SOURCE=.\Shapfile.cpp
# End Source File
# Begin Source File

SOURCE=.\Sharenot.cpp
# End Source File
# Begin Source File

SOURCE=.\Signdoc.cpp
# End Source File
# Begin Source File

SOURCE=.\Signview.cpp
# End Source File
# Begin Source File

SOURCE=.\Simplecb.cpp
# End Source File
# Begin Source File

SOURCE=.\Simplelb.cpp
# End Source File
# Begin Source File

SOURCE=.\Smacker.cpp
# End Source File
# Begin Source File

SOURCE=.\sortarry.cpp
# End Source File
# Begin Source File

SOURCE=.\SOUNDFX.CPP
# End Source File
# Begin Source File

SOURCE=.\SpaceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SPELLCHK.CPP
# End Source File
# Begin Source File

SOURCE=.\SpellDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SPELLENG.CPP
# End Source File
# Begin Source File

SOURCE=.\Spinbutt.cpp
# End Source File
# Begin Source File

SOURCE=.\splitter.cpp
# End Source File
# Begin Source File

SOURCE=.\SplshTxt.cpp
# End Source File
# Begin Source File

SOURCE=.\Spltarry.cpp
# End Source File
# Begin Source File

SOURCE=.\Srchart.cpp
# End Source File
# Begin Source File

SOURCE=.\srvacdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\statbar.cpp
# End Source File
# Begin Source File

SOURCE=.\STATICE.CPP
# End Source File
# Begin Source File

SOURCE=.\StaticSpinCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\stkdsktp.cpp
# End Source File
# Begin Source File

SOURCE=.\stkrdoc.cpp
# End Source File
# Begin Source File

SOURCE=.\StkrPrnD.cpp
# End Source File
# Begin Source File

SOURCE=.\stkrview.cpp
# End Source File
# Begin Source File

SOURCE=.\Storeman.cpp
# End Source File
# Begin Source File

SOURCE=.\strclass.cpp
# End Source File
# Begin Source File

SOURCE=.\STRMCACH.CPP
# End Source File
# Begin Source File

SOURCE=.\Style.cpp
# End Source File
# Begin Source File

SOURCE=.\Stylehlp.cpp
# End Source File
# Begin Source File

SOURCE=.\Styleit.cpp
# End Source File
# Begin Source File

SOURCE=.\stylerec.CPP
# End Source File
# Begin Source File

SOURCE=.\SvBorder.cpp
# End Source File
# Begin Source File

SOURCE=.\svpicdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Tableman.cpp
# End Source File
# Begin Source File

SOURCE=.\Tabsdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\tblcmd.cpp
# End Source File
# Begin Source File

SOURCE=.\tblctl.CPP
# End Source File
# Begin Source File

SOURCE=.\tbledit.cpp
# End Source File
# Begin Source File

SOURCE=.\TBLFMT.CPP
# End Source File
# Begin Source File

SOURCE=.\TblObj.cpp
# End Source File
# Begin Source File

SOURCE=.\TblPrpD.cpp
# End Source File
# Begin Source File

SOURCE=.\TCOMBO.CPP
# End Source File
# Begin Source File

SOURCE=.\Tcommand.cpp
# End Source File
# Begin Source File

SOURCE=.\Testprnt.cpp
# End Source File
# Begin Source File

SOURCE=.\Text.cpp
# End Source File
# Begin Source File

SOURCE=.\Textarry.cpp
# End Source File
# Begin Source File

SOURCE=.\Textflow.cpp
# End Source File
# Begin Source File

SOURCE=.\Textline.cpp
# End Source File
# Begin Source File

SOURCE=.\textrec.CPP
# End Source File
# Begin Source File

SOURCE=.\Tfengine.cpp
# End Source File
# Begin Source File

SOURCE=.\Tiff.cpp
# End Source File
# Begin Source File

SOURCE=.\Tiffdrv.cpp
# End Source File
# Begin Source File

SOURCE=.\Tilesbtn.cpp
# End Source File
# Begin Source File

SOURCE=.\timestmp.cpp
# End Source File
# Begin Source File

SOURCE=.\TIMPORT.CPP
# End Source File
# Begin Source File

SOURCE=.\tinycash.cpp
# End Source File
# Begin Source File

SOURCE=.\Ttdll.cpp
# End Source File
# Begin Source File

SOURCE=.\TutList.cpp
# End Source File
# Begin Source File

SOURCE=.\TutorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TXPDEST.CPP
# End Source File
# Begin Source File

SOURCE=.\TypeCmbo.cpp
# End Source File
# Begin Source File

SOURCE=.\Typeface.cpp
# End Source File
# Begin Source File

SOURCE=.\Upgdll.cpp
# End Source File
# Begin Source File

SOURCE=.\Upgradlg.cpp
# End Source File
# Begin Source File

SOURCE=.\urlproc.cpp
# End Source File
# Begin Source File

SOURCE=.\USERSET.CPP
# End Source File
# Begin Source File

SOURCE=.\UsrCtgry.cpp
# End Source File
# Begin Source File

SOURCE=.\Util.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils.cpp
# End Source File
# Begin Source File

SOURCE=.\Uuencode.cpp
# End Source File
# Begin Source File

SOURCE=.\Vfl.cpp
# End Source File
# Begin Source File

SOURCE=.\Viewcmd.cpp
# End Source File
# Begin Source File

SOURCE=.\Warpfile.cpp
# End Source File
# Begin Source File

SOURCE=.\Warpfld.cpp
# End Source File
# Begin Source File

SOURCE=.\Warpobj.cpp
# End Source File
# Begin Source File

SOURCE=.\Warpsel.cpp
# End Source File
# Begin Source File

SOURCE=.\WebDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\webdoc.CPP
# End Source File
# Begin Source File

SOURCE=.\WebPubQ.cpp
# End Source File
# Begin Source File

SOURCE=.\webview.CPP
# End Source File
# Begin Source File

SOURCE=.\WHCalend.cpp
# End Source File
# Begin Source File

SOURCE=.\WHGreet.cpp
# End Source File
# Begin Source File

SOURCE=.\WHLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\WHlpCmn.cpp
# End Source File
# Begin Source File

SOURCE=.\WHlpProj.cpp
# End Source File
# Begin Source File

SOURCE=.\WHlpView.cpp
# End Source File
# Begin Source File

SOURCE=.\WHlpWiz.cpp
# End Source File
# Begin Source File

SOURCE=.\WHLtrhd.cpp
# End Source File
# Begin Source File

SOURCE=.\WHNwsltr.cpp
# End Source File
# Begin Source File

SOURCE=.\WHPoster.cpp
# End Source File
# Begin Source File

SOURCE=.\WHWebPg.cpp
# End Source File
# Begin Source File

SOURCE=.\Widthsel.cpp
# End Source File
# Begin Source File

SOURCE=.\Wmf.cpp
# End Source File
# Begin Source File

SOURCE=.\WndBig.cpp
# End Source File
# Begin Source File

SOURCE=.\WndHub.cpp
# End Source File
# Begin Source File

SOURCE=.\WPG.CPP
# End Source File
# Begin Source File

SOURCE=.\XERROR.CPP
# End Source File
# Begin Source File

SOURCE=.\Yeardll.cpp
# End Source File
# Begin Source File

SOURCE=.\YywbDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\ABOUTDLG.H
# End Source File
# Begin Source File

SOURCE=.\ACTION.H
# End Source File
# Begin Source File

SOURCE=.\Addcalpg.h
# End Source File
# Begin Source File

SOURCE=.\addprnvw.h
# End Source File
# Begin Source File

SOURCE=.\ADDRBOOK.H
# End Source File
# Begin Source File

SOURCE=.\ADDRDATA.H
# End Source File
# Begin Source File

SOURCE=.\ADDREDIT.H
# End Source File
# Begin Source File

SOURCE=.\ADDRIMP.H
# End Source File
# Begin Source File

SOURCE=.\AddrInfoPage.h
# End Source File
# Begin Source File

SOURCE=.\ADDRLIST.H
# End Source File
# Begin Source File

SOURCE=.\AdLocReq.h
# End Source File
# Begin Source File

SOURCE=.\APROGRES.H
# End Source File
# Begin Source File

SOURCE=.\ARRAY.H
# End Source File
# Begin Source File

SOURCE=.\ARRAYREC.H
# End Source File
# Begin Source File

SOURCE=.\ArtCheck.h
# End Source File
# Begin Source File

SOURCE=.\ArtExistsDlg.h
# End Source File
# Begin Source File

SOURCE=.\artstore.h
# End Source File
# Begin Source File

SOURCE=.\Autoexec.h
# End Source File
# Begin Source File

SOURCE=.\automate.h
# End Source File
# Begin Source File

SOURCE=.\BACKOBJ.H
# End Source File
# Begin Source File

SOURCE=.\BANNDOC.H
# End Source File
# Begin Source File

SOURCE=.\BANNVIEW.H
# End Source File
# Begin Source File

SOURCE=.\BARCOMBO.H
# End Source File
# Begin Source File

SOURCE=.\BARMENU.H
# End Source File
# Begin Source File

SOURCE=.\BillingInfoPage.h
# End Source File
# Begin Source File

SOURCE=.\BindStat.h
# End Source File
# Begin Source File

SOURCE=.\BITBTN.H
# End Source File
# Begin Source File

SOURCE=.\blowfish.h
# End Source File
# Begin Source File

SOURCE=.\BMP.H
# End Source File
# Begin Source File

SOURCE=.\Bmparray.h
# End Source File
# Begin Source File

SOURCE=.\BmpTiler.h
# End Source File
# Begin Source File

SOURCE=.\BmStatic.h
# End Source File
# Begin Source File

SOURCE=.\Border\borddef.h
# End Source File
# Begin Source File

SOURCE=.\Borders.h
# End Source File
# Begin Source File

SOURCE=.\BrdrDlg.h
# End Source File
# Begin Source File

SOURCE=.\BrdrObj.h
# End Source File
# Begin Source File

SOURCE=.\BRODOC.H
# End Source File
# Begin Source File

SOURCE=.\BROPRINT.H
# End Source File
# Begin Source File

SOURCE=.\BROVIEW.H
# End Source File
# Begin Source File

SOURCE=.\BrowDrop.h
# End Source File
# Begin Source File

SOURCE=.\BROWSER.H
# End Source File
# Begin Source File

SOURCE=.\BROWSUPP.H
# End Source File
# Begin Source File

SOURCE=.\BtnNRect.h
# End Source File
# Begin Source File

SOURCE=.\BTNWND.H
# End Source File
# Begin Source File

SOURCE=.\CACHE.H
# End Source File
# Begin Source File

SOURCE=.\CalDateD.h
# End Source File
# Begin Source File

SOURCE=.\CALDOC.H
# End Source File
# Begin Source File

SOURCE=.\CALENDAR.H
# End Source File
# Begin Source File

SOURCE=.\CALINFO.H
# End Source File
# Begin Source File

SOURCE=.\CALOBJ.H
# End Source File
# Begin Source File

SOURCE=.\CalPicD.h
# End Source File
# Begin Source File

SOURCE=.\CalProgD.h
# End Source File
# Begin Source File

SOURCE=.\calrec.h
# End Source File
# Begin Source File

SOURCE=.\CALSTYLE.H
# End Source File
# Begin Source File

SOURCE=.\CalTypeD.h
# End Source File
# Begin Source File

SOURCE=.\CALVIEW.H
# End Source File
# Begin Source File

SOURCE=.\CARDDOC.H
# End Source File
# Begin Source File

SOURCE=.\CARDSRVR.H
# End Source File
# Begin Source File

SOURCE=.\CARDVIEW.H
# End Source File
# Begin Source File

SOURCE=.\CARRAY.H
# End Source File
# Begin Source File

SOURCE=.\CATALOG.H
# End Source File
# Begin Source File

SOURCE=.\CatOrgnz.h
# End Source File
# Begin Source File

SOURCE=.\CCOMMAND.H
# End Source File
# Begin Source File

SOURCE=.\CDCACHE.H
# End Source File
# Begin Source File

SOURCE=.\CDEFLATE.H
# End Source File
# Begin Source File

SOURCE=.\CDEMODLG.H
# End Source File
# Begin Source File

SOURCE=.\CDIBDC.H
# End Source File
# Begin Source File

SOURCE=.\CDReq.h
# End Source File
# Begin Source File

SOURCE=.\celobj.h
# End Source File
# Begin Source File

SOURCE=.\celOutD.h
# End Source File
# Begin Source File

SOURCE=.\CFIXED.H
# End Source File
# Begin Source File

SOURCE=.\Cgm.h
# End Source File
# Begin Source File

SOURCE=.\CHARRAY.H
# End Source File
# Begin Source File

SOURCE=.\chdirdlg.h
# End Source File
# Begin Source File

SOURCE=.\CheckArtExists.h
# End Source File
# Begin Source File

SOURCE=.\Cimplode.h
# End Source File
# Begin Source File

SOURCE=.\CINDEX.H
# End Source File
# Begin Source File

SOURCE=.\CLIP.H
# End Source File
# Begin Source File

SOURCE=.\Cmfaddon.h
# End Source File
# Begin Source File

SOURCE=.\CNDX.H
# End Source File
# Begin Source File

SOURCE=.\CNDXNT.H
# End Source File
# Begin Source File

SOURCE=.\CNDXVC.H
# End Source File
# Begin Source File

SOURCE=.\CnfrmRm.h
# End Source File
# Begin Source File

SOURCE=.\CNTRITEM.H
# End Source File
# Begin Source File

SOURCE=.\COLLBLD.H
# End Source File
# Begin Source File

SOURCE=.\COLLCTG.H
# End Source File
# Begin Source File

SOURCE=.\COLLFILE.H
# End Source File
# Begin Source File

SOURCE=.\COLLIMP.H
# End Source File
# Begin Source File

SOURCE=.\COLLMGR.H
# End Source File
# Begin Source File

SOURCE=.\CollReq.h
# End Source File
# Begin Source File

SOURCE=.\COLOR.H
# End Source File
# Begin Source File

SOURCE=.\COLORDEF.H
# End Source File
# Begin Source File

SOURCE=.\COLORSEL.H
# End Source File
# Begin Source File

SOURCE=.\COMMAND.H
# End Source File
# Begin Source File

SOURCE=.\COMMANDS.H
# End Source File
# Begin Source File

SOURCE=.\COMPFN.H
# End Source File
# Begin Source File

SOURCE=.\Compress.h
# End Source File
# Begin Source File

SOURCE=.\ContMgr.h
# End Source File
# Begin Source File

SOURCE=.\CONVERT.H
# End Source File
# Begin Source File

SOURCE=.\CPROGBAR.H
# End Source File
# Begin Source File

SOURCE=.\CraftDlg.h
# End Source File
# Begin Source File

SOURCE=.\Craster.h
# End Source File
# Begin Source File

SOURCE=.\credit.h
# End Source File
# Begin Source File

SOURCE=.\CrPGSAcc.h
# End Source File
# Begin Source File

SOURCE=.\CTWAIN.H
# End Source File
# Begin Source File

SOURCE=.\CTXP.H
# End Source File
# Begin Source File

SOURCE=.\CustBar.h
# End Source File
# Begin Source File

SOURCE=.\Cwmf.h
# End Source File
# Begin Source File

SOURCE=.\Cwmfctl.h
# End Source File
# Begin Source File

SOURCE=.\DATABASE.H
# End Source File
# Begin Source File

SOURCE=.\DATEOBJ.H
# End Source File
# Begin Source File

SOURCE=.\DB386.H
# End Source File
# Begin Source File

SOURCE=.\DBMAP.H
# End Source File
# Begin Source File

SOURCE=.\DBSTORE.H
# End Source File
# Begin Source File

SOURCE=.\DBTABS.H
# End Source File
# Begin Source File

SOURCE=.\DDB.H
# End Source File
# Begin Source File

SOURCE=.\DELNAME.H
# End Source File
# Begin Source File

SOURCE=.\DEMODLG.H
# End Source File
# Begin Source File

SOURCE=.\DESKTOP.H
# End Source File
# Begin Source File

SOURCE=.\dib2file.h
# End Source File
# Begin Source File

SOURCE=.\DIBBRUSH.H
# End Source File
# Begin Source File

SOURCE=.\Digitalv.h
# End Source File
# Begin Source File

SOURCE=.\DITHER.H
# End Source File
# Begin Source File

SOURCE=.\DLGMGR.H
# End Source File
# Begin Source File

SOURCE=.\DLGURL.H
# End Source File
# Begin Source File

SOURCE=.\DLIST.H
# End Source File
# Begin Source File

SOURCE=.\DocDump.h
# End Source File
# Begin Source File

SOURCE=.\DOCREC.H
# End Source File
# Begin Source File

SOURCE=.\DRAWDLG.H
# End Source File
# Begin Source File

SOURCE=.\DRAWOBJ.H
# End Source File
# Begin Source File

SOURCE=.\DRIVEMGR.H
# End Source File
# Begin Source File

SOURCE=.\DYNARRAY.H
# End Source File
# Begin Source File

SOURCE=.\EFFECTS.H
# End Source File
# Begin Source File

SOURCE=.\Efilerec.h
# End Source File
# Begin Source File

SOURCE=.\ELLIOBJ.H
# End Source File
# Begin Source File

SOURCE=.\ENVDOC.H
# End Source File
# Begin Source File

SOURCE=.\ENVPRINT.H
# End Source File
# Begin Source File

SOURCE=.\ENVPRVW.H
# End Source File
# Begin Source File

SOURCE=.\ENVTYPE.H
# End Source File
# Begin Source File

SOURCE=.\ENVVIEW.H
# End Source File
# Begin Source File

SOURCE=.\ERROR.H
# End Source File
# Begin Source File

SOURCE=.\EVENT.H
# End Source File
# Begin Source File

SOURCE=.\exgraf.h
# End Source File
# Begin Source File

SOURCE=.\FCACHE.H
# End Source File
# Begin Source File

SOURCE=.\FDIRREC.H
# End Source File
# Begin Source File

SOURCE=.\FFLDFILE.H
# End Source File
# Begin Source File

SOURCE=.\FIELDDLG.H
# End Source File
# Begin Source File

SOURCE=.\filcmprs.h
# End Source File
# Begin Source File

SOURCE=.\FILE.H
# End Source File
# Begin Source File

SOURCE=.\FILEDB.H
# End Source File
# Begin Source File

SOURCE=.\fill.h
# End Source File
# Begin Source File

SOURCE=.\FINDREP.H
# End Source File
# Begin Source File

SOURCE=.\FINDTEXT.H
# End Source File
# Begin Source File

SOURCE=.\FLATDB.H
# End Source File
# Begin Source File

SOURCE=.\Flexmath.h
# End Source File
# Begin Source File

SOURCE=.\Flistrec.h
# End Source File
# Begin Source File

SOURCE=.\FONTREC.H
# End Source File
# Begin Source File

SOURCE=.\FONTSRV.H
# End Source File
# Begin Source File

SOURCE=.\FRAMEDIT.H
# End Source File
# Begin Source File

SOURCE=.\FRAMEDLG.H
# End Source File
# Begin Source File

SOURCE=.\FRAMEOBJ.H
# End Source File
# Begin Source File

SOURCE=.\FRAMEREC.H
# End Source File
# Begin Source File

SOURCE=.\FREELIST.H
# End Source File
# Begin Source File

SOURCE=.\GCACHE.H
# End Source File
# Begin Source File

SOURCE=.\GERONIMO.H
# End Source File
# Begin Source File

SOURCE=.\GIF.H
# End Source File
# Begin Source File

SOURCE=.\GLICENSE.H
# End Source File
# Begin Source File

SOURCE=.\Globatom.h
# End Source File
# Begin Source File

SOURCE=.\GradDlg.h
# End Source File
# Begin Source File

SOURCE=.\GRAFOBJ.H
# End Source File
# Begin Source File

SOURCE=.\GRAFREC.H
# End Source File
# Begin Source File

SOURCE=.\GrLocReq.h
# End Source File
# Begin Source File

SOURCE=.\GrphReq.h
# End Source File
# Begin Source File

SOURCE=.\GRPOBJ.H
# End Source File
# Begin Source File

SOURCE=.\HALO.H
# End Source File
# Begin Source File

SOURCE=.\HEAP.H
# End Source File
# Begin Source File

SOURCE=.\HelpHint.h
# End Source File
# Begin Source File

SOURCE=.\hhctrl.h
# End Source File
# Begin Source File

SOURCE=.\hint.h
# End Source File
# Begin Source File

SOURCE=.\htmlconv.h
# End Source File
# Begin Source File

SOURCE=.\Htmldoc.h
# End Source File
# Begin Source File

SOURCE=.\htmlhelp.h
# End Source File
# Begin Source File

SOURCE=.\HUBMGR.H
# End Source File
# Begin Source File

SOURCE=.\hyperrec.h
# End Source File
# Begin Source File

SOURCE=.\hyplkdlg.h
# End Source File
# Begin Source File

SOURCE=.\IDRVMGR.H
# End Source File
# Begin Source File

SOURCE=.\Image.h
# End Source File
# Begin Source File

SOURCE=.\Imagebuf.h
# End Source File
# Begin Source File

SOURCE=.\IMAGEDEF.H
# End Source File
# Begin Source File

SOURCE=.\IMAGEDRV.H
# End Source File
# Begin Source File

SOURCE=.\imagemap.h
# End Source File
# Begin Source File

SOURCE=.\Imagesrc.h
# End Source File
# Begin Source File

SOURCE=.\Implod16.h
# End Source File
# Begin Source File

SOURCE=.\Implod32.h
# End Source File
# Begin Source File

SOURCE=.\INIFILE.H
# End Source File
# Begin Source File

SOURCE=.\INSTDAT.H
# End Source File
# Begin Source File

SOURCE=.\IPATHMGR.H
# End Source File
# Begin Source File

SOURCE=.\IPFRAME.H
# End Source File
# Begin Source File

SOURCE=.\IUSERSET.H
# End Source File
# Begin Source File

SOURCE=.\JPEG.H
# End Source File
# Begin Source File

SOURCE=.\Jpegimpl.h
# End Source File
# Begin Source File

SOURCE=.\JTRACE.H
# End Source File
# Begin Source File

SOURCE=.\KEY.H
# End Source File
# Begin Source File

SOURCE=.\LABELDLG.H
# End Source File
# Begin Source File

SOURCE=.\LABELS.H
# End Source File
# Begin Source File

SOURCE=.\LAYREC.H
# End Source File
# Begin Source File

SOURCE=.\LBLDOC.H
# End Source File
# Begin Source File

SOURCE=.\LBLPRINT.H
# End Source File
# Begin Source File

SOURCE=.\LBLPRVW.H
# End Source File
# Begin Source File

SOURCE=.\LBLVIEW.H
# End Source File
# Begin Source File

SOURCE=.\LICDLL.H
# End Source File
# Begin Source File

SOURCE=.\LICENSE.H
# End Source File
# Begin Source File

SOURCE=.\Linecros.h
# End Source File
# Begin Source File

SOURCE=.\LINEOBJ.H
# End Source File
# Begin Source File

SOURCE=.\LINEWID.H
# End Source File
# Begin Source File

SOURCE=.\LIST.H
# End Source File
# Begin Source File

SOURCE=.\LoginReq.h
# End Source File
# Begin Source File

SOURCE=.\Lzw.h
# End Source File
# Begin Source File

SOURCE=.\MACRODLG.H
# End Source File
# Begin Source File

SOURCE=.\MACROREC.H
# End Source File
# Begin Source File

SOURCE=.\MACROSRV.H
# End Source File
# Begin Source File

SOURCE=.\MACROSTD.H
# End Source File
# Begin Source File

SOURCE=.\MAINFRM.H
# End Source File
# Begin Source File

SOURCE=.\MARQUEE.H
# End Source File
# Begin Source File

SOURCE=.\Maskedbm.h
# End Source File
# Begin Source File

SOURCE=.\MCIWND.H
# End Source File
# Begin Source File

SOURCE=.\MEMDEV.H
# End Source File
# Begin Source File

SOURCE=.\MENUTYPE.H
# End Source File
# Begin Source File

SOURCE=.\MISC.H
# End Source File
# Begin Source File

SOURCE=.\MonthEdit.h
# End Source File
# Begin Source File

SOURCE=.\MSREG.H
# End Source File
# Begin Source File

SOURCE=.\MULTINET.H
# End Source File
# Begin Source File

SOURCE=.\NAMEREC.H
# End Source File
# Begin Source File

SOURCE=.\Netcount.h
# End Source File
# Begin Source File

SOURCE=.\NETID.H
# End Source File
# Begin Source File

SOURCE=.\NETUSERS.H
# End Source File
# Begin Source File

SOURCE=.\NEWDLG.H
# End Source File
# Begin Source File

SOURCE=.\NEWFONT.H
# End Source File
# Begin Source File

SOURCE=.\NEWSDOC.H
# End Source File
# Begin Source File

SOURCE=.\NEWSVIEW.H
# End Source File
# Begin Source File

SOURCE=.\NFX.H
# End Source File
# Begin Source File

SOURCE=.\Nheap.h
# End Source File
# Begin Source File

SOURCE=.\NOFILLIN.H
# End Source File
# Begin Source File

SOURCE=.\NumberEdit.h
# End Source File
# Begin Source File

SOURCE=.\NWSPRINT.H
# End Source File
# Begin Source File

SOURCE=.\OBJDB.H
# End Source File
# Begin Source File

SOURCE=.\OBJIMG.H
# End Source File
# Begin Source File

SOURCE=.\OBJLIST.H
# End Source File
# Begin Source File

SOURCE=.\OLDDOC.H
# End Source File
# Begin Source File

SOURCE=.\OLDGRAF.H
# End Source File
# Begin Source File

SOURCE=.\OLDTEXT.H
# End Source File
# Begin Source File

SOURCE=.\OLEOBJ.H
# End Source File
# Begin Source File

SOURCE=.\OnAdDlg.h
# End Source File
# Begin Source File

SOURCE=.\OpenDlgs.h
# End Source File
# Begin Source File

SOURCE=.\ORIENDLG.H
# End Source File
# Begin Source File

SOURCE=.\Outline.h
# End Source File
# Begin Source File

SOURCE=.\OUTPATH.H
# End Source File
# Begin Source File

SOURCE=.\OWNERCB.H
# End Source File
# Begin Source File

SOURCE=.\OWNERLB.H
# End Source File
# Begin Source File

SOURCE=.\PAGEDEFS.H
# End Source File
# Begin Source File

SOURCE=.\PAGEDLGS.H
# End Source File
# Begin Source File

SOURCE=.\PAGEREC.H
# End Source File
# Begin Source File

SOURCE=.\Paint.h
# End Source File
# Begin Source File

SOURCE=.\PANEL.H
# End Source File
# Begin Source File

SOURCE=.\PAPER.H
# End Source File
# Begin Source File

SOURCE=.\PARADLG.H
# End Source File
# Begin Source File

SOURCE=.\PARAREC.H
# End Source File
# Begin Source File

SOURCE=.\PARSE.H
# End Source File
# Begin Source File

SOURCE=.\Pathit.h
# End Source File
# Begin Source File

SOURCE=.\PATHMGR.H
# End Source File
# Begin Source File

SOURCE=.\Pathrec.h
# End Source File
# Begin Source File

SOURCE=.\PATOBJ.H
# End Source File
# Begin Source File

SOURCE=.\PCD.H
# End Source File
# Begin Source File

SOURCE=.\PCDLIB.H
# End Source File
# Begin Source File

SOURCE=.\PCX.H
# End Source File
# Begin Source File

SOURCE=.\PDLGCLR.H
# End Source File
# Begin Source File

SOURCE=.\PDPropSt.h
# End Source File
# Begin Source File

SOURCE=.\Pegasus.h
# End Source File
# Begin Source File

SOURCE=.\PGSSess.h
# End Source File
# Begin Source File

SOURCE=.\PICTATTR.H
# End Source File
# Begin Source File

SOURCE=.\PICTPRVW.H
# End Source File
# Begin Source File

SOURCE=.\pmaodlg.H
# End Source File
# Begin Source File

SOURCE=.\PMBMBUT.H
# End Source File
# Begin Source File

SOURCE=.\PMCFILE.H
# End Source File
# Begin Source File

SOURCE=.\Pmdirdlg.h
# End Source File
# Begin Source File

SOURCE=.\PMGDB.H
# End Source File
# Begin Source File

SOURCE=.\PMGFONT.H
# End Source File
# Begin Source File

SOURCE=.\PMGOBJ.H
# End Source File
# Begin Source File

SOURCE=.\pmhelp.h
# End Source File
# Begin Source File

SOURCE=.\Pmobject.h
# End Source File
# Begin Source File

SOURCE=.\PMW.H
# End Source File
# Begin Source File

SOURCE=.\PMWCFG.H
# End Source File
# Begin Source File

SOURCE=.\PMWCOLL.H
# End Source File
# Begin Source File

SOURCE=.\PMWCOLOR.H
# End Source File
# Begin Source File

SOURCE=.\PMWDLG.H
# End Source File
# Begin Source File

SOURCE=.\PMWDOC.H
# End Source File
# Begin Source File

SOURCE=.\pmwinet.h
# End Source File
# Begin Source File

SOURCE=.\PMWINI.H
# End Source File
# Begin Source File

SOURCE=.\PMWMDI.H
# End Source File
# Begin Source File

SOURCE=.\PMWPRINT.H
# End Source File
# Begin Source File

SOURCE=.\PMWPROP.H
# End Source File
# Begin Source File

SOURCE=.\PMWRFL.H
# End Source File
# Begin Source File

SOURCE=.\PMWTEMPL.H
# End Source File
# Begin Source File

SOURCE=.\PMWTOOL.H
# End Source File
# Begin Source File

SOURCE=.\PMWVIEW.H
# End Source File
# Begin Source File

SOURCE=.\PMWWIZ.H
# End Source File
# Begin Source File

SOURCE=.\Polypoly.h
# End Source File
# Begin Source File

SOURCE=.\POPUP.H
# End Source File
# Begin Source File

SOURCE=.\POSDLG.H
# End Source File
# Begin Source File

SOURCE=.\POSITION.H
# End Source File
# Begin Source File

SOURCE=.\POSLIST.H
# End Source File
# Begin Source File

SOURCE=.\PREFMISC.H
# End Source File
# Begin Source File

SOURCE=.\PREFPSH.H
# End Source File
# Begin Source File

SOURCE=.\PREFSND.H
# End Source File
# Begin Source File

SOURCE=.\PREFWRK.H
# End Source File
# Begin Source File

SOURCE=.\PREVREC.H
# End Source File
# Begin Source File

SOURCE=.\PRINTADV.H
# End Source File
# Begin Source File

SOURCE=.\PRINTART.H
# End Source File
# Begin Source File

SOURCE=.\PRINTER.H
# End Source File
# Begin Source File

SOURCE=.\PRINTPRJ.H
# End Source File
# Begin Source File

SOURCE=.\PRINTWIZ.H
# End Source File
# Begin Source File

SOURCE=.\prnaddlg.h
# End Source File
# Begin Source File

SOURCE=.\PrntCalD.h
# End Source File
# Begin Source File

SOURCE=.\PRNTRNGE.H
# End Source File
# Begin Source File

SOURCE=.\PRNTSIZE.H
# End Source File
# Begin Source File

SOURCE=.\procloc.h
# End Source File
# Begin Source File

SOURCE=.\PROFILE.H
# End Source File
# Begin Source File

SOURCE=.\PROGRESS.H
# End Source File
# Begin Source File

SOURCE=.\PROJECT.H
# End Source File
# Begin Source File

SOURCE=.\PROJGRPH.H
# End Source File
# Begin Source File

SOURCE=.\PROJOBJ.H
# End Source File
# Begin Source File

SOURCE=.\PROJTEXT.H
# End Source File
# Begin Source File

SOURCE=.\PromtDlg.h
# End Source File
# Begin Source File

SOURCE=.\PROPLIST.H
# End Source File
# Begin Source File

SOURCE=.\PROPSTR.H
# End Source File
# Begin Source File

SOURCE=.\PRVWDLG.H
# End Source File
# Begin Source File

SOURCE=.\PSD.H
# End Source File
# Begin Source File

SOURCE=.\QSORT.H
# End Source File
# Begin Source File

SOURCE=.\Recarray.h
# End Source File
# Begin Source File

SOURCE=.\RECORD.H
# End Source File
# Begin Source File

SOURCE=.\Rectobj.h
# End Source File
# Begin Source File

SOURCE=.\REDISP.H
# End Source File
# Begin Source File

SOURCE=.\REFRESH.H
# End Source File
# Begin Source File

SOURCE=.\REGISTER.H
# End Source File
# Begin Source File

SOURCE=.\ROTDLG.H
# End Source File
# Begin Source File

SOURCE=.\ROTUTILS.H
# End Source File
# Begin Source File

SOURCE=.\RTFPARSE.H
# End Source File
# Begin Source File

SOURCE=.\RULER.H
# End Source File
# Begin Source File

SOURCE=.\RWSEM.H
# End Source File
# Begin Source File

SOURCE=.\SAVEPROJ.H
# End Source File
# Begin Source File

SOURCE=.\SCROLLGA.H
# End Source File
# Begin Source File

SOURCE=.\SHAPEDLG.H
# End Source File
# Begin Source File

SOURCE=.\SHAPESEL.H
# End Source File
# Begin Source File

SOURCE=.\SHAPFILE.H
# End Source File
# Begin Source File

SOURCE=.\SHARENOT.H
# End Source File
# Begin Source File

SOURCE=.\SIGNDOC.H
# End Source File
# Begin Source File

SOURCE=.\SIGNVIEW.H
# End Source File
# Begin Source File

SOURCE=.\SIMPLECB.H
# End Source File
# Begin Source File

SOURCE=.\SIMPLELB.H
# End Source File
# Begin Source File

SOURCE=.\SMACKER.H
# End Source File
# Begin Source File

SOURCE=.\SORTARRY.H
# End Source File
# Begin Source File

SOURCE=.\SOUNDFX.H
# End Source File
# Begin Source File

SOURCE=.\SPACEDLG.H
# End Source File
# Begin Source File

SOURCE=.\SPELLCHK.H
# End Source File
# Begin Source File

SOURCE=.\SPELLDLG.H
# End Source File
# Begin Source File

SOURCE=.\SPELLENG.H
# End Source File
# Begin Source File

SOURCE=.\SPINBUTT.H
# End Source File
# Begin Source File

SOURCE=.\SPLITTER.H
# End Source File
# Begin Source File

SOURCE=.\SplshTxt.h
# End Source File
# Begin Source File

SOURCE=.\SPLTARRY.H
# End Source File
# Begin Source File

SOURCE=.\srvacdlg.h
# End Source File
# Begin Source File

SOURCE=.\SSCE.H
# End Source File
# Begin Source File

SOURCE=.\STATBAR.H
# End Source File
# Begin Source File

SOURCE=.\STATICE.H
# End Source File
# Begin Source File

SOURCE=.\StaticSpinCtrl.h
# End Source File
# Begin Source File

SOURCE=.\STDAFX.H
# End Source File
# Begin Source File

SOURCE=.\stkdsktp.h
# End Source File
# Begin Source File

SOURCE=.\stkrdoc.h
# End Source File
# Begin Source File

SOURCE=.\StkrPrnD.h
# End Source File
# Begin Source File

SOURCE=.\stkrview.h
# End Source File
# Begin Source File

SOURCE=.\STOREMAN.H
# End Source File
# Begin Source File

SOURCE=.\STRCLASS.H
# End Source File
# Begin Source File

SOURCE=.\STRMCACH.H
# End Source File
# Begin Source File

SOURCE=.\STYLE.H
# End Source File
# Begin Source File

SOURCE=.\Stylehlp.h
# End Source File
# Begin Source File

SOURCE=.\STYLEIT.H
# End Source File
# Begin Source File

SOURCE=.\STYLEREC.H
# End Source File
# Begin Source File

SOURCE=.\SvBorder.h
# End Source File
# Begin Source File

SOURCE=.\svpicdlg.h
# End Source File
# Begin Source File

SOURCE=.\SYSTEM.H
# End Source File
# Begin Source File

SOURCE=.\TABLEMAN.H
# End Source File
# Begin Source File

SOURCE=.\TABSDLG.H
# End Source File
# Begin Source File

SOURCE=.\TBLFMT.H
# End Source File
# Begin Source File

SOURCE=.\TblObj.h
# End Source File
# Begin Source File

SOURCE=.\TblPrpD.h
# End Source File
# Begin Source File

SOURCE=.\TCOMBO.H
# End Source File
# Begin Source File

SOURCE=.\TCOMMAND.H
# End Source File
# Begin Source File

SOURCE=.\TESTPRNT.H
# End Source File
# Begin Source File

SOURCE=.\TEXT.H
# End Source File
# Begin Source File

SOURCE=.\TEXTARRY.H
# End Source File
# Begin Source File

SOURCE=.\TEXTFLOW.H
# End Source File
# Begin Source File

SOURCE=.\Textline.h
# End Source File
# Begin Source File

SOURCE=.\TEXTREC.H
# End Source File
# Begin Source File

SOURCE=.\TFENGINE.H
# End Source File
# Begin Source File

SOURCE=.\TIFF.H
# End Source File
# Begin Source File

SOURCE=.\TIFFDRV.H
# End Source File
# Begin Source File

SOURCE=.\TILESBTN.H
# End Source File
# Begin Source File

SOURCE=.\timestmp.h
# End Source File
# Begin Source File

SOURCE=.\TIMPORT.H
# End Source File
# Begin Source File

SOURCE=.\tinycash.h
# End Source File
# Begin Source File

SOURCE=.\TOOLBARX.H
# End Source File
# Begin Source File

SOURCE=.\TRUETYPE.H
# End Source File
# Begin Source File

SOURCE=.\TTDLL.H
# End Source File
# Begin Source File

SOURCE=.\TutList.h
# End Source File
# Begin Source File

SOURCE=.\TutorDlg.h
# End Source File
# Begin Source File

SOURCE=.\TWAIN.H
# End Source File
# Begin Source File

SOURCE=.\TXPDEST.H
# End Source File
# Begin Source File

SOURCE=.\TypeCmbo.h
# End Source File
# Begin Source File

SOURCE=.\TYPEFACE.H
# End Source File
# Begin Source File

SOURCE=.\UPGDLL.H
# End Source File
# Begin Source File

SOURCE=.\UPGRADLG.H
# End Source File
# Begin Source File

SOURCE=.\USERSET.H
# End Source File
# Begin Source File

SOURCE=.\UsrCtgry.h
# End Source File
# Begin Source File

SOURCE=.\Util.h
# End Source File
# Begin Source File

SOURCE=.\UTILS.H
# End Source File
# Begin Source File

SOURCE=.\VFL.H
# End Source File
# Begin Source File

SOURCE=.\WARPFILE.H
# End Source File
# Begin Source File

SOURCE=.\WARPFLD.H
# End Source File
# Begin Source File

SOURCE=.\WARPOBJ.H
# End Source File
# Begin Source File

SOURCE=.\WARPSEL.H
# End Source File
# Begin Source File

SOURCE=.\webdef.h
# End Source File
# Begin Source File

SOURCE=.\WebDlg.h
# End Source File
# Begin Source File

SOURCE=.\webdoc.H
# End Source File
# Begin Source File

SOURCE=.\WebPubQ.h
# End Source File
# Begin Source File

SOURCE=.\webview.H
# End Source File
# Begin Source File

SOURCE=.\WHCalend.h
# End Source File
# Begin Source File

SOURCE=.\WHGreet.h
# End Source File
# Begin Source File

SOURCE=.\WHLabel.h
# End Source File
# Begin Source File

SOURCE=.\WHlpCmn.h
# End Source File
# Begin Source File

SOURCE=.\WHlpProj.h
# End Source File
# Begin Source File

SOURCE=.\WHlpView.h
# End Source File
# Begin Source File

SOURCE=.\WHlpWiz.h
# End Source File
# Begin Source File

SOURCE=.\WHLtrhd.h
# End Source File
# Begin Source File

SOURCE=.\WHNwsltr.h
# End Source File
# Begin Source File

SOURCE=.\WHPoster.h
# End Source File
# Begin Source File

SOURCE=.\WHWebPg.h
# End Source File
# Begin Source File

SOURCE=.\WIDTHSEL.H
# End Source File
# Begin Source File

SOURCE=.\WINNET.H
# End Source File
# Begin Source File

SOURCE=.\WMF.H
# End Source File
# Begin Source File

SOURCE=.\WNDBIG.H
# End Source File
# Begin Source File

SOURCE=.\WNDHUB.H
# End Source File
# Begin Source File

SOURCE=.\WPG.H
# End Source File
# Begin Source File

SOURCE=.\XERROR.H
# End Source File
# Begin Source File

SOURCE=.\Yeardll.h
# End Source File
# Begin Source File

SOURCE=.\YywbDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\Docs\pmtodo.doc
# End Source File
# End Target
# End Project
