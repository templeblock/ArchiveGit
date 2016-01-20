# Microsoft Developer Studio Project File - Name="PMW" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PMW - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Pmw.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Pmw.mak" CFG="PMW - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PMW - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PMW - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PMW", UYAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PMW - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "winRel"
# PROP Intermediate_Dir "App\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /Zp2 /MD /W3 /GR /GX /Zi /Ot /Oi /Oy /Ob1 /Gf /Gy /I ".\Accusoft" /I ".\TpsLibs\Include" /I ".\Framework\Include" /I ".\Interfaces" /I ".\Resource" /I ".\PSLaunchDLL" /I ".\Png" /I ".\ZLib" /D "NDEBUG" /D "_AFXDLL" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "NEW_THUMBNAILS" /D "NEW_COMPRESSED_CONTENT" /D "PRECOMPILE_FRAMEWORK" /D "_NEWPORTABLE" /Fr /Yu"STDAFX.H" /FD /c
# SUBTRACT CPP /Ox /Oa /Ow
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib mpr.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libcmtd" /nodefaultlib:"libcmt"
# SUBTRACT LINK32 /map /nodefaultlib

!ELSEIF  "$(CFG)" == "PMW - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "winDebug"
# PROP Intermediate_Dir "App\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /Zp2 /MDd /W3 /GR /GX /Zi /Od /I ".\Accusoft" /I ".\TpsLibs\Include" /I ".\Framework\Include" /I ".\Interfaces" /I ".\Resource" /I ".\PSLaunchDLL" /I ".\Png" /I ".\ZLib" /D "_DEBUG" /D "_AFXDLL" /D "_WINDOWS" /D "WIN32" /D "_MBCS" /D "NEW_THUMBNAILS" /D "NEW_COMPRESSED_CONTENT" /D "PRECOMPILE_FRAMEWORK" /D "TPSDEBUG" /Fr /Yu"STDAFX.H" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 winmm.lib mpr.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc" /nodefaultlib:"libcmtd"
# SUBTRACT LINK32 /profile /map /nodefaultlib

!ENDIF 

# Begin Target

# Name "PMW - Win32 Release"
# Name "PMW - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\App\Aboutdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\ActAcDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Addcalpg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\addprnvw.cpp
# End Source File
# Begin Source File

SOURCE=.\App\addrbook.cpp
# End Source File
# Begin Source File

SOURCE=.\App\ADDRDATA.CPP
# End Source File
# Begin Source File

SOURCE=.\App\addredit.cpp
# End Source File
# Begin Source File

SOURCE=.\App\addrimp.cpp
# End Source File
# Begin Source File

SOURCE=.\App\AddrInfoPage.cpp
# End Source File
# Begin Source File

SOURCE=.\App\addrlist.cpp
# End Source File
# Begin Source File

SOURCE=.\App\adoninst.cpp
# End Source File
# Begin Source File

SOURCE=.\App\AnimationPreviewDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\aprogres.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Array.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Arrayrec.cpp
# End Source File
# Begin Source File

SOURCE=.\App\ArtCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\App\ArtExistsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\artstore.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Autoexec.cpp
# End Source File
# Begin Source File

SOURCE=.\App\automate.cpp
# End Source File
# Begin Source File

SOURCE=.\App\BackdropSearchCollection.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Backobj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Banndoc.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Bannview.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Barcombo.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Barmenu.cpp
# End Source File
# Begin Source File

SOURCE=.\App\BaseBld.cpp
# End Source File
# Begin Source File

SOURCE=.\App\BaseCustomPage.cpp
# End Source File
# Begin Source File

SOURCE=.\App\BaseDataSource.cpp
# End Source File
# Begin Source File

SOURCE=.\App\bfinit.cpp
# End Source File
# Begin Source File

SOURCE=.\App\BillingInfoPage.cpp
# End Source File
# Begin Source File

SOURCE=.\App\BITBTN.CPP
# End Source File
# Begin Source File

SOURCE=.\App\BitmapLoad.cpp
# End Source File
# Begin Source File

SOURCE=.\App\bkgobj.CPP
# End Source File
# Begin Source File

SOURCE=.\App\blowfish.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Bmp.cpp
# End Source File
# Begin Source File

SOURCE=.\App\bmparray.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Bmpfile.cpp
# End Source File
# Begin Source File

SOURCE=.\App\BmpTiler.cpp
# End Source File
# Begin Source File

SOURCE=.\App\BmStatic.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Borders.cpp
# End Source File
# Begin Source File

SOURCE=.\App\BrdrDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\BrdrObj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Breakup.cpp
# End Source File
# Begin Source File

SOURCE=.\App\BRODOC.CPP
# End Source File
# Begin Source File

SOURCE=.\App\broprint.CPP
# End Source File
# Begin Source File

SOURCE=.\App\BROVIEW.CPP
# End Source File
# Begin Source File

SOURCE=.\App\BrowDrop.cpp
# End Source File
# Begin Source File

SOURCE=.\App\BROWSER.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Browsupp.cpp
# End Source File
# Begin Source File

SOURCE=.\App\BtnNRect.cpp
# End Source File
# Begin Source File

SOURCE=.\App\BTNWND.CPP
# End Source File
# Begin Source File

SOURCE=.\App\BulletDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\App\cache.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Calbuild.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Calcalc.cpp
# End Source File
# Begin Source File

SOURCE=.\App\CalDateD.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Caldoc.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Calendar.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Calobj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\CalPicD.cpp
# End Source File
# Begin Source File

SOURCE=.\App\CalProgD.cpp
# End Source File
# Begin Source File

SOURCE=.\App\calrec.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Calstyle.cpp
# End Source File
# Begin Source File

SOURCE=.\App\CalTypeD.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Calview.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Carddoc.cpp
# End Source File
# Begin Source File

SOURCE=.\App\CardSrvr.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Cardview.cpp
# End Source File
# Begin Source File

SOURCE=.\App\CARRAY.CPP
# End Source File
# Begin Source File

SOURCE=.\App\CastLUTs.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Catalog.cpp
# End Source File
# Begin Source File

SOURCE=.\App\CATDATA.CPP
# End Source File
# Begin Source File

SOURCE=.\App\CatOrgnz.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Ccommand.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Cdcache.cpp
# End Source File
# Begin Source File

SOURCE=.\App\cdeflate.CPP
# End Source File
# Begin Source File

SOURCE=.\App\cdemodlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\CDib.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Cdibdc.cpp
# End Source File
# Begin Source File

SOURCE=.\App\celctl.CPP
# End Source File
# Begin Source File

SOURCE=.\App\celobj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\celoutd.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Cfixed.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Cgm.cpp
# End Source File
# Begin Source File

SOURCE=.\App\CheckArtExists.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Cimplode.cpp
# End Source File
# Begin Source File

SOURCE=.\App\CINDEX.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Clip.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Cmfaddon.cpp
# End Source File
# Begin Source File

SOURCE=.\App\CnfrmRm.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Cntritem.cpp
# End Source File
# Begin Source File

SOURCE=.\App\COLLBLD.CPP
# End Source File
# Begin Source File

SOURCE=.\App\COLLCTG.CPP
# End Source File
# Begin Source File

SOURCE=.\App\colldrw.CPP
# End Source File
# Begin Source File

SOURCE=.\App\COLLFILE.CPP
# End Source File
# Begin Source File

SOURCE=.\App\COLLIMP.CPP
# End Source File
# Begin Source File

SOURCE=.\App\COLLMGR.CPP
# End Source File
# Begin Source File

SOURCE=.\App\CollSel.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Color.cpp
# End Source File
# Begin Source File

SOURCE=.\App\ColorCbo.cpp
# End Source File
# Begin Source File

SOURCE=.\App\ColorDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\App\ColorPhotoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Colorsel.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Command.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Commands.cpp
# End Source File
# Begin Source File

SOURCE=.\App\CommonGroupedObjectDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Comp32.cpp
# End Source File
# Begin Source File

SOURCE=.\App\COMPDB.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Compfn.cpp
# End Source File
# Begin Source File

SOURCE=.\App\CompObj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\CompRec.cpp
# End Source File
# Begin Source File

SOURCE=.\App\connmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\App\ConnMgrW.cpp
# End Source File
# Begin Source File

SOURCE=.\App\ContMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\App\CONTSRV.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Convert.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Cprogbar.cpp
# End Source File
# Begin Source File

SOURCE=.\App\CraftDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Craster.cpp
# End Source File
# Begin Source File

SOURCE=.\App\credit.cpp
# End Source File
# Begin Source File

SOURCE=.\App\CrPGSAcc.cpp
# End Source File
# Begin Source File

SOURCE=.\App\CTWAIN.CPP
# End Source File
# Begin Source File

SOURCE=.\App\ctxp.cpp
# End Source File
# Begin Source File

SOURCE=.\App\CustBar.cpp
# End Source File
# Begin Source File

SOURCE=.\App\CustomGraphicSearch.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Cutpaste.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Cwmf.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Cwmfctl.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Database.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Dateobj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\dbbld.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Dbmap.cpp
# End Source File
# Begin Source File

SOURCE=.\App\dbtabs.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Ddb.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Delname.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Demodlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Desktop.cpp
# End Source File
# Begin Source File

SOURCE=.\App\dib2file.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Dibbrush.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Disptext.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Dither.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Dlgmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\App\DLGURL.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Dlist.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Doccmd.cpp
# End Source File
# Begin Source File

SOURCE=.\App\docdib.CPP
# End Source File
# Begin Source File

SOURCE=.\App\DocDump.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Docedit.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Docpict.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Docrec.cpp
# End Source File
# Begin Source File

SOURCE=.\App\DownDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\downmgr.CPP
# End Source File
# Begin Source File

SOURCE=.\App\DragDataSource.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Drawobj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Drivemgr.cpp
# End Source File
# Begin Source File

SOURCE=.\App\DsnCheck.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Dynarray.cpp
# End Source File
# Begin Source File

SOURCE=.\app\EasyPrintsdoc.cpp
# End Source File
# Begin Source File

SOURCE=.\app\EasyPrintsView.cpp
# End Source File
# Begin Source File

SOURCE=.\App\EditCropControl.cpp
# End Source File
# Begin Source File

SOURCE=.\App\effects.cpp
# End Source File
# Begin Source File

SOURCE=.\App\EffectsCtrls.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Efilerec.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Elliobj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\enumproc.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Envdoc.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Envprint.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Envprvw.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Envtype.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Envview.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Errdiff.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Error.cpp
# End Source File
# Begin Source File

SOURCE=.\App\exgraf.cpp
# End Source File
# Begin Source File

SOURCE=.\App\ExportGraphicDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Fcache.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Fdirrec.cpp
# End Source File
# Begin Source File

SOURCE=.\App\FeedbackDropSource.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Ffldfile.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Fielddlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\filcmprs.cpp
# End Source File
# Begin Source File

SOURCE=.\App\File.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Filedb.cpp
# End Source File
# Begin Source File

SOURCE=.\App\filemgr.CPP
# End Source File
# Begin Source File

SOURCE=.\App\fill.cpp
# End Source File
# Begin Source File

SOURCE=.\App\findrep.cpp
# End Source File
# Begin Source File

SOURCE=.\App\FindText.cpp
# End Source File
# Begin Source File

SOURCE=.\App\flatdb.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Flexmath.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Flistrec.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Flyout.cpp
# End Source File
# Begin Source File

SOURCE=.\App\FlyoutToolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\App\FontComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\App\FontPrvw.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Fontrec.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Framedit.cpp
# End Source File
# Begin Source File

SOURCE=.\App\framedlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Frameobj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Framerec.cpp
# End Source File
# Begin Source File

SOURCE=.\App\FREELIST.CPP
# End Source File
# Begin Source File

SOURCE=.\App\GenericGroupedObjectDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Gif.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Glicense.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Globatom.cpp
# End Source File
# Begin Source File

SOURCE=.\App\gohtml.cpp
# End Source File
# Begin Source File

SOURCE=.\App\GradDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Grafobj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Grafrec.cpp
# End Source File
# Begin Source File

SOURCE=.\App\GraphicCategorySearch.cpp
# End Source File
# Begin Source File

SOURCE=.\App\GraphicSearch.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Grpobj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\GUID.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Halo.cpp
# End Source File
# Begin Source File

SOURCE=.\App\HeadlineDepthPage.cpp
# End Source File
# Begin Source File

SOURCE=.\App\HeadlineDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\HeadlineFacePage.cpp
# End Source File
# Begin Source File

SOURCE=.\App\HeadlineFontDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\HeadlineOutlinePage.cpp
# End Source File
# Begin Source File

SOURCE=.\App\HeadlinePositionPage.cpp
# End Source File
# Begin Source File

SOURCE=.\App\HeadlineProportionPage.cpp
# End Source File
# Begin Source File

SOURCE=.\App\HeadlineShadowPage.cpp
# End Source File
# Begin Source File

SOURCE=.\App\HeadlineShapePage.cpp
# End Source File
# Begin Source File

SOURCE=.\App\HelpHint.cpp
# End Source File
# Begin Source File

SOURCE=.\App\HexColorGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\App\hhctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\App\htmlconv.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Htmldoc.cpp
# End Source File
# Begin Source File

SOURCE=.\App\hubmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\App\HybridBaseDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\hyperrec.cpp
# End Source File
# Begin Source File

SOURCE=.\App\hyplkdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Image.cpp
# End Source File
# Begin Source File

SOURCE=.\App\ImageBtn.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Imagebuf.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Imagedrv.cpp
# End Source File
# Begin Source File

SOURCE=.\App\ImageGridCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\App\imagemap.cpp
# End Source File
# Begin Source File

SOURCE=.\App\ImagePreviewCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Imagesrc.cpp
# End Source File
# Begin Source File

SOURCE=.\App\ImageUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\App\ImgCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\App\InCompdb.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Inifile.cpp
# End Source File
# Begin Source File

SOURCE=.\App\InitCapDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\App\InsertSymbolDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Instdat.cpp
# End Source File
# Begin Source File

SOURCE=.\App\INTLDLL.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Ipframe.cpp
# End Source File
# Begin Source File

SOURCE=.\App\jpeg.CPP
# End Source File
# Begin Source File

SOURCE=.\App\JPEGIMPL.CPP
# End Source File
# Begin Source File

SOURCE=.\App\KEY.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Labeldlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Labels.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Layrec.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Lbldoc.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Lblprint.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Lblprvw.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Lblview.cpp
# End Source File
# Begin Source File

SOURCE=.\App\licdll.cpp
# End Source File
# Begin Source File

SOURCE=.\App\License.cpp
# End Source File
# Begin Source File

SOURCE=.\App\LINECROS.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Lineobj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Linewid.cpp
# End Source File
# Begin Source File

SOURCE=.\App\List.cpp
# End Source File
# Begin Source File

SOURCE=.\App\LogoMakerDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Lzw.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Macro.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Macrodlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Macrorec.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Macrostd.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Mainfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Marquee.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Maskedbm.cpp
# End Source File
# Begin Source File

SOURCE=.\App\matchset.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Mciwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Memdev.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Modecrop.cpp
# End Source File
# Begin Source File

SOURCE=.\App\modecrt.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Modedraw.cpp
# End Source File
# Begin Source File

SOURCE=.\App\MODELINK.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Modesize.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Modetext.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Modezoom.cpp
# End Source File
# Begin Source File

SOURCE=.\App\MonthEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Namerec.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Netcount.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Netid.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Netusers.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Network.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Newdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\newfont.CPP
# End Source File
# Begin Source File

SOURCE=.\App\newsdoc.CPP
# End Source File
# Begin Source File

SOURCE=.\App\newsview.CPP
# End Source File
# Begin Source File

SOURCE=.\App\nfx.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Nheap.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Nofillin.cpp
# End Source File
# Begin Source File

SOURCE=.\App\NumberDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\App\NumberEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\App\NwsPrint.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Objdb.cpp
# End Source File
# Begin Source File

SOURCE=.\App\ObjectSize.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Objimg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Objlist.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Od.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Olddoc.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Oleobj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\OnAdDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\OpenDlgs.cpp
# End Source File
# Begin Source File

SOURCE=.\App\ORIENDLG.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Outline.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Outpath.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Ownercb.cpp
# End Source File
# Begin Source File

SOURCE=.\App\OwnerDrawMenu.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Ownerlb.cpp
# End Source File
# Begin Source File

SOURCE=.\App\pagedlgs.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Pageprop.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Pagerec.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Paint.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PaintCap.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PANEL.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Paper.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PARADLG.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Pararec.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Parse.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PathDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Pathmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Pathrec.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PathView.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Patobj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Pcd.cpp
# End Source File
# Begin Source File

SOURCE=.\App\pcrdprnt.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Pcx.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PDLGCLR.CPP
# End Source File
# Begin Source File

SOURCE=.\App\PDPropSt.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PEGASUS.CPP
# End Source File
# Begin Source File

SOURCE=.\App\PGSOrWiz.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PGSSess.cpp
# End Source File
# Begin Source File

SOURCE=.\app\phoprjprint.cpp
# End Source File
# Begin Source File

SOURCE=.\app\phoprjprvw.cpp
# End Source File
# Begin Source File

SOURCE=.\app\photoprj.cpp
# End Source File
# Begin Source File

SOURCE=.\app\PhPrjDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Pictattr.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Pictprvw.cpp
# End Source File
# Begin Source File

SOURCE=.\App\pmaodlg.CPP
# End Source File
# Begin Source File

SOURCE=.\App\pmbmbut.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Pmcfile.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Pmdirdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Pmgdb.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Pmgfont.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Pmgobj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\pmhelp.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Pmw.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Pmwcfg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PMWCOLL.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Pmwcolor.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PMWDISP.ODL
# End Source File
# Begin Source File

SOURCE=.\App\Pmwdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Pmwdoc.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Pmwmdi.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Pmwprint.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Pmwprop.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PMWRFL.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Pmwtempl.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PMWTOOL.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Pmwview.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PMWWIZ.CPP
# End Source File
# Begin Source File

SOURCE=.\App\PNGIMPL.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Polypoly.cpp
# End Source File
# Begin Source File

SOURCE=.\App\POPUP.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Posdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Position.cpp
# End Source File
# Begin Source File

SOURCE=.\App\poslist.cpp
# End Source File
# Begin Source File

SOURCE=.\App\postcard.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PreferencesDatePage.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PREFMISC.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Prefpsh.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Prefsnd.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Prefwrk.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Prevrec.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Printadv.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Printart.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Printer.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Printprj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PRINTWIZ.CPP
# End Source File
# Begin Source File

SOURCE=.\App\prnaddlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PrntCalD.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Prntrnge.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Prntsize.cpp
# End Source File
# Begin Source File

SOURCE=.\App\procloc.cpp
# End Source File
# Begin Source File

SOURCE=.\App\profile.cpp
# End Source File
# Begin Source File

SOURCE=.\App\ProgBar.cpp
# End Source File
# Begin Source File

SOURCE=.\App\progress.cpp
# End Source File
# Begin Source File

SOURCE=.\App\ProgStat.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Project.cpp
# End Source File
# Begin Source File

SOURCE=.\App\projgrph.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Projobj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Projtext.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PromtDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Proplist.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PROPSTR.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Prvwdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\PSD.CPP
# End Source File
# Begin Source File

SOURCE=.\App\QSORT.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Recarray.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Record.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Rectobj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Redisp.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Refresh.cpp
# End Source File
# Begin Source File

SOURCE=.\App\RegCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Register.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Rm.cpp
# End Source File
# Begin Source File

SOURCE=.\App\RMAGCom.cpp
# End Source File
# Begin Source File

SOURCE=.\App\RmCraft.cpp
# End Source File
# Begin Source File

SOURCE=.\App\RmPrint.cpp
# End Source File
# Begin Source File

SOURCE=.\App\RmSpirit.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Rotdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Rotutils.cpp
# End Source File
# Begin Source File

SOURCE=.\App\RTFPARSE.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Ruler.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Rwsem.cpp
# End Source File
# Begin Source File

SOURCE=.\App\save2pic.cpp
# End Source File
# Begin Source File

SOURCE=.\App\SAVEPROJ.CPP
# End Source File
# Begin Source File

SOURCE=.\App\ScrollableControl.cpp
# End Source File
# Begin Source File

SOURCE=.\App\scrollga.cpp
# End Source File
# Begin Source File

SOURCE=.\App\SealDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\App\SearchResult.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Shapedlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Shapesel.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Shapfile.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Sharenot.cpp
# End Source File
# Begin Source File

SOURCE=.\App\SignatureDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Signdoc.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Signview.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Simplecb.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Simplelb.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Smacker.cpp

!IF  "$(CFG)" == "PMW - Win32 Release"

!ELSEIF  "$(CFG)" == "PMW - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\App\sortarry.cpp
# End Source File
# Begin Source File

SOURCE=.\App\SOUNDFX.CPP
# End Source File
# Begin Source File

SOURCE=.\App\SpaceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\SPELLCHK.CPP
# End Source File
# Begin Source File

SOURCE=.\App\SpellDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\SPELLENG.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Spinbutt.cpp
# End Source File
# Begin Source File

SOURCE=.\App\splitter.cpp
# End Source File
# Begin Source File

SOURCE=.\App\SplshTxt.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Spltarry.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Srchart.cpp
# End Source File
# Begin Source File

SOURCE=.\App\srvacdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\SSHeaderArray.cpp
# End Source File
# Begin Source File

SOURCE=.\App\statbar.cpp
# End Source File
# Begin Source File

SOURCE=.\App\STATICE.CPP
# End Source File
# Begin Source File

SOURCE=.\App\StaticSpinCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\App\stkdsktp.cpp
# End Source File
# Begin Source File

SOURCE=.\App\stkrdoc.cpp
# End Source File
# Begin Source File

SOURCE=.\App\StkrPrnD.cpp
# End Source File
# Begin Source File

SOURCE=.\App\stkrview.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Storeman.cpp
# End Source File
# Begin Source File

SOURCE=.\App\strclass.cpp
# End Source File
# Begin Source File

SOURCE=.\App\STRMCACH.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Style.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Stylehlp.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Styleit.cpp
# End Source File
# Begin Source File

SOURCE=.\App\stylerec.CPP
# End Source File
# Begin Source File

SOURCE=.\App\SvBorder.cpp
# End Source File
# Begin Source File

SOURCE=.\App\svpicdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\TabbedEditAdjustCol.cpp
# End Source File
# Begin Source File

SOURCE=.\App\TabbedEditArtEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\App\TabbedEditBrightFocus.cpp
# End Source File
# Begin Source File

SOURCE=.\App\TabbedEditCrop.cpp
# End Source File
# Begin Source File

SOURCE=.\App\TabbedEditFixFlaw.cpp
# End Source File
# Begin Source File

SOURCE=.\App\TabbedEditImage.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Tableman.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Tabsdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\tblcmd.cpp
# End Source File
# Begin Source File

SOURCE=.\App\tblctl.CPP
# End Source File
# Begin Source File

SOURCE=.\App\tbledit.cpp
# End Source File
# Begin Source File

SOURCE=.\App\TBLFMT.CPP
# End Source File
# Begin Source File

SOURCE=.\App\TblObj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\TblPrpD.cpp
# End Source File
# Begin Source File

SOURCE=.\App\TCOMBO.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Tcommand.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Testprnt.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Text.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Textarry.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Textflow.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Textline.cpp
# End Source File
# Begin Source File

SOURCE=.\App\textrec.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Tfengine.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Tiff.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Tiffdrv.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Tilesbtn.cpp
# End Source File
# Begin Source File

SOURCE=.\App\TimePieceDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\timestmp.cpp
# End Source File
# Begin Source File

SOURCE=.\App\TIMPORT.CPP
# End Source File
# Begin Source File

SOURCE=.\App\tinycash.cpp
# End Source File
# Begin Source File

SOURCE=.\App\TransBtn.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Ttdll.cpp
# End Source File
# Begin Source File

SOURCE=.\App\TutList.cpp
# End Source File
# Begin Source File

SOURCE=.\App\TutorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\TXPDEST.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Typeface.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Upgdll.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Upgradlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\urlproc.cpp
# End Source File
# Begin Source File

SOURCE=.\App\USERSET.CPP
# End Source File
# Begin Source File

SOURCE=.\App\UsrCtgry.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Util.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Utils.cpp
# End Source File
# Begin Source File

SOURCE=.\App\UUENCODE.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Vfl.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Viewcmd.cpp
# End Source File
# Begin Source File

SOURCE=.\App\ViewComp.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Warpfile.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Warpfld.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Warpobj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Warpsel.cpp
# End Source File
# Begin Source File

SOURCE=.\App\WebDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\webdoc.CPP
# End Source File
# Begin Source File

SOURCE=.\App\WebPubQ.cpp
# End Source File
# Begin Source File

SOURCE=.\App\webview.CPP
# End Source File
# Begin Source File

SOURCE=.\App\WHCalend.cpp
# End Source File
# Begin Source File

SOURCE=.\App\WHGreet.cpp
# End Source File
# Begin Source File

SOURCE=.\App\WHLabel.cpp
# End Source File
# Begin Source File

SOURCE=.\App\WHlpCmn.cpp
# End Source File
# Begin Source File

SOURCE=.\App\WHlpProj.cpp
# End Source File
# Begin Source File

SOURCE=.\App\WHlpView.cpp
# End Source File
# Begin Source File

SOURCE=.\App\WHlpWiz.cpp
# End Source File
# Begin Source File

SOURCE=.\App\WHLtrhd.cpp
# End Source File
# Begin Source File

SOURCE=.\App\WHNwsltr.cpp
# End Source File
# Begin Source File

SOURCE=.\App\WHPoster.cpp
# End Source File
# Begin Source File

SOURCE=.\App\WHWebPg.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Widthsel.cpp
# End Source File
# Begin Source File

SOURCE=.\App\WindowsImageList.cpp
# End Source File
# Begin Source File

SOURCE=.\App\WinTextureBrowser.cpp
# End Source File
# Begin Source File

SOURCE=.\App\Wmf.cpp
# End Source File
# Begin Source File

SOURCE=.\App\WndBig.cpp
# End Source File
# Begin Source File

SOURCE=.\App\WndHub.cpp
# End Source File
# Begin Source File

SOURCE=.\App\WPG.CPP
# End Source File
# Begin Source File

SOURCE=.\App\XERROR.CPP
# End Source File
# Begin Source File

SOURCE=.\App\Yeardll.cpp
# End Source File
# Begin Source File

SOURCE=.\App\YywbDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\App\ABOUTDLG.H
# End Source File
# Begin Source File

SOURCE=.\App\ACTION.H
# End Source File
# Begin Source File

SOURCE=.\App\Addcalpg.h
# End Source File
# Begin Source File

SOURCE=.\App\addprnvw.h
# End Source File
# Begin Source File

SOURCE=.\App\ADDRBOOK.H
# End Source File
# Begin Source File

SOURCE=.\App\ADDRDATA.H
# End Source File
# Begin Source File

SOURCE=.\App\ADDREDIT.H
# End Source File
# Begin Source File

SOURCE=.\App\ADDRIMP.H
# End Source File
# Begin Source File

SOURCE=.\App\AddrInfoPage.h
# End Source File
# Begin Source File

SOURCE=.\App\ADDRLIST.H
# End Source File
# Begin Source File

SOURCE=.\App\AnimationPreviewDlg.h
# End Source File
# Begin Source File

SOURCE=.\App\APROGRES.H
# End Source File
# Begin Source File

SOURCE=.\App\ARRAY.H
# End Source File
# Begin Source File

SOURCE=.\App\ARRAYREC.H
# End Source File
# Begin Source File

SOURCE=.\App\ArtCheck.h
# End Source File
# Begin Source File

SOURCE=.\App\ArtExistsDlg.h
# End Source File
# Begin Source File

SOURCE=.\App\artstore.h
# End Source File
# Begin Source File

SOURCE=.\App\Autoexec.h
# End Source File
# Begin Source File

SOURCE=.\App\automate.h
# End Source File
# Begin Source File

SOURCE=.\App\BackdropSearchCollection.h
# End Source File
# Begin Source File

SOURCE=.\App\BACKOBJ.H
# End Source File
# Begin Source File

SOURCE=.\App\BANNDOC.H
# End Source File
# Begin Source File

SOURCE=.\App\BANNVIEW.H
# End Source File
# Begin Source File

SOURCE=.\App\BARCOMBO.H
# End Source File
# Begin Source File

SOURCE=.\App\BARMENU.H
# End Source File
# Begin Source File

SOURCE=.\App\Basebld.h
# End Source File
# Begin Source File

SOURCE=.\App\BaseCustomPage.h
# End Source File
# Begin Source File

SOURCE=.\App\BaseDataSource.h
# End Source File
# Begin Source File

SOURCE=.\App\BillingInfoPage.h
# End Source File
# Begin Source File

SOURCE=.\App\BindStat.h
# End Source File
# Begin Source File

SOURCE=.\App\BITBTN.H
# End Source File
# Begin Source File

SOURCE=.\App\BitmapLoad.h
# End Source File
# Begin Source File

SOURCE=.\App\blowfish.h
# End Source File
# Begin Source File

SOURCE=.\App\BMP.H
# End Source File
# Begin Source File

SOURCE=.\App\Bmparray.h
# End Source File
# Begin Source File

SOURCE=.\App\BmpTiler.h
# End Source File
# Begin Source File

SOURCE=.\App\BmStatic.h
# End Source File
# Begin Source File

SOURCE=.\App\Border\borddef.h
# End Source File
# Begin Source File

SOURCE=.\App\Borders.h
# End Source File
# Begin Source File

SOURCE=.\App\BrdrDlg.h
# End Source File
# Begin Source File

SOURCE=.\App\BrdrObj.h
# End Source File
# Begin Source File

SOURCE=.\App\BRODOC.H
# End Source File
# Begin Source File

SOURCE=.\App\BROPRINT.H
# End Source File
# Begin Source File

SOURCE=.\App\BROVIEW.H
# End Source File
# Begin Source File

SOURCE=.\App\BrowDrop.h
# End Source File
# Begin Source File

SOURCE=.\App\BROWSER.H
# End Source File
# Begin Source File

SOURCE=.\App\BROWSUPP.H
# End Source File
# Begin Source File

SOURCE=.\App\BtnNRect.h
# End Source File
# Begin Source File

SOURCE=.\App\BTNWND.H
# End Source File
# Begin Source File

SOURCE=.\App\BulletDialog.h
# End Source File
# Begin Source File

SOURCE=.\App\CACHE.H
# End Source File
# Begin Source File

SOURCE=.\App\CalDateD.h
# End Source File
# Begin Source File

SOURCE=.\App\CALDOC.H
# End Source File
# Begin Source File

SOURCE=.\App\CALENDAR.H
# End Source File
# Begin Source File

SOURCE=.\App\CALINFO.H
# End Source File
# Begin Source File

SOURCE=.\App\CALOBJ.H
# End Source File
# Begin Source File

SOURCE=.\App\CalPicD.h
# End Source File
# Begin Source File

SOURCE=.\App\CalProgD.h
# End Source File
# Begin Source File

SOURCE=.\App\calrec.h
# End Source File
# Begin Source File

SOURCE=.\App\CALSTYLE.H
# End Source File
# Begin Source File

SOURCE=.\App\CalTypeD.h
# End Source File
# Begin Source File

SOURCE=.\App\CALVIEW.H
# End Source File
# Begin Source File

SOURCE=.\App\CARDDOC.H
# End Source File
# Begin Source File

SOURCE=.\App\CARDSRVR.H
# End Source File
# Begin Source File

SOURCE=.\App\CARDVIEW.H
# End Source File
# Begin Source File

SOURCE=.\App\CARRAY.H
# End Source File
# Begin Source File

SOURCE=.\App\CastLUTs.h
# End Source File
# Begin Source File

SOURCE=.\App\CATALOG.H
# End Source File
# Begin Source File

SOURCE=.\App\CATDATA.H
# End Source File
# Begin Source File

SOURCE=.\App\CatOrgnz.h
# End Source File
# Begin Source File

SOURCE=.\App\CCOMMAND.H
# End Source File
# Begin Source File

SOURCE=.\App\CDCACHE.H
# End Source File
# Begin Source File

SOURCE=.\App\CDEFLATE.H
# End Source File
# Begin Source File

SOURCE=.\App\CDEMODLG.H
# End Source File
# Begin Source File

SOURCE=.\App\CDib.h
# End Source File
# Begin Source File

SOURCE=.\App\CDIBDC.H
# End Source File
# Begin Source File

SOURCE=.\App\CDReq.h
# End Source File
# Begin Source File

SOURCE=.\App\celobj.h
# End Source File
# Begin Source File

SOURCE=.\App\celOutD.h
# End Source File
# Begin Source File

SOURCE=.\App\CFIXED.H
# End Source File
# Begin Source File

SOURCE=.\App\Cgm.h
# End Source File
# Begin Source File

SOURCE=.\App\CHARRAY.H
# End Source File
# Begin Source File

SOURCE=.\App\chdirdlg.h
# End Source File
# Begin Source File

SOURCE=.\App\CheckArtExists.h
# End Source File
# Begin Source File

SOURCE=.\App\Cimplode.h
# End Source File
# Begin Source File

SOURCE=.\App\CINDEX.H
# End Source File
# Begin Source File

SOURCE=.\App\CLIP.H
# End Source File
# Begin Source File

SOURCE=.\App\Cmfaddon.h
# End Source File
# Begin Source File

SOURCE=.\App\CNDX.H
# End Source File
# Begin Source File

SOURCE=.\App\CNDXNT.H
# End Source File
# Begin Source File

SOURCE=.\App\CNDXVC.H
# End Source File
# Begin Source File

SOURCE=.\App\CnfrmRm.h
# End Source File
# Begin Source File

SOURCE=.\App\CNTRITEM.H
# End Source File
# Begin Source File

SOURCE=.\App\COLLBLD.H
# End Source File
# Begin Source File

SOURCE=.\App\COLLCTG.H
# End Source File
# Begin Source File

SOURCE=.\App\colldrw.H
# End Source File
# Begin Source File

SOURCE=.\App\CollectionSearcher.h
# End Source File
# Begin Source File

SOURCE=.\App\COLLFILE.H
# End Source File
# Begin Source File

SOURCE=.\App\COLLIMP.H
# End Source File
# Begin Source File

SOURCE=.\App\COLLMGR.H
# End Source File
# Begin Source File

SOURCE=.\App\CollReq.h
# End Source File
# Begin Source File

SOURCE=.\App\COLLSRCH.H
# End Source File
# Begin Source File

SOURCE=.\App\COLOR.H
# End Source File
# Begin Source File

SOURCE=.\App\COLORDEF.H
# End Source File
# Begin Source File

SOURCE=.\App\ColorDialog.h
# End Source File
# Begin Source File

SOURCE=.\App\COLORSEL.H
# End Source File
# Begin Source File

SOURCE=.\App\COMMAND.H
# End Source File
# Begin Source File

SOURCE=.\App\COMMANDS.H
# End Source File
# Begin Source File

SOURCE=.\App\CommonGroupedObjectDialog.h
# End Source File
# Begin Source File

SOURCE=.\App\COMPDB.H
# End Source File
# Begin Source File

SOURCE=.\App\COMPFN.H
# End Source File
# Begin Source File

SOURCE=.\App\Compress.h
# End Source File
# Begin Source File

SOURCE=.\App\connmgr.h
# End Source File
# Begin Source File

SOURCE=.\App\ConnMgrW.h
# End Source File
# Begin Source File

SOURCE=.\App\ContMgr.h
# End Source File
# Begin Source File

SOURCE=.\App\CONTSRV.H
# End Source File
# Begin Source File

SOURCE=.\App\CONVERT.H
# End Source File
# Begin Source File

SOURCE=.\App\CPROGBAR.H
# End Source File
# Begin Source File

SOURCE=.\App\CraftDlg.h
# End Source File
# Begin Source File

SOURCE=.\App\Craster.h
# End Source File
# Begin Source File

SOURCE=.\App\credit.h
# End Source File
# Begin Source File

SOURCE=.\App\CrPGSAcc.h
# End Source File
# Begin Source File

SOURCE=.\App\CTWAIN.H
# End Source File
# Begin Source File

SOURCE=.\App\CTXP.H
# End Source File
# Begin Source File

SOURCE=.\App\CustBar.h
# End Source File
# Begin Source File

SOURCE=.\App\CustCtrlDef.h
# End Source File
# Begin Source File

SOURCE=.\App\CustomGraphicSearch.h
# End Source File
# Begin Source File

SOURCE=.\App\Cwmf.h
# End Source File
# Begin Source File

SOURCE=.\App\Cwmfctl.h
# End Source File
# Begin Source File

SOURCE=.\App\DATABASE.H
# End Source File
# Begin Source File

SOURCE=.\App\DATEOBJ.H
# End Source File
# Begin Source File

SOURCE=.\App\DB386.H
# End Source File
# Begin Source File

SOURCE=.\App\DBBld.h
# End Source File
# Begin Source File

SOURCE=.\App\DBMAP.H
# End Source File
# Begin Source File

SOURCE=.\App\DBSTORE.H
# End Source File
# Begin Source File

SOURCE=.\App\DBTABS.H
# End Source File
# Begin Source File

SOURCE=.\App\DDB.H
# End Source File
# Begin Source File

SOURCE=.\App\DELNAME.H
# End Source File
# Begin Source File

SOURCE=.\App\DEMODLG.H
# End Source File
# Begin Source File

SOURCE=.\App\DESKTOP.H
# End Source File
# Begin Source File

SOURCE=.\App\dib2file.h
# End Source File
# Begin Source File

SOURCE=.\App\DIBBRUSH.H
# End Source File
# Begin Source File

SOURCE=.\App\Digitalv.h
# End Source File
# Begin Source File

SOURCE=.\App\DITHER.H
# End Source File
# Begin Source File

SOURCE=.\App\DLGMGR.H
# End Source File
# Begin Source File

SOURCE=.\App\DLGURL.H
# End Source File
# Begin Source File

SOURCE=.\App\DLIST.H
# End Source File
# Begin Source File

SOURCE=.\App\DocDump.h
# End Source File
# Begin Source File

SOURCE=.\App\DOCREC.H
# End Source File
# Begin Source File

SOURCE=.\App\DownDlg.h
# End Source File
# Begin Source File

SOURCE=.\App\downmgr.H
# End Source File
# Begin Source File

SOURCE=.\App\DragDataSource.h
# End Source File
# Begin Source File

SOURCE=.\App\DRAWDLG.H
# End Source File
# Begin Source File

SOURCE=.\App\DRAWOBJ.H
# End Source File
# Begin Source File

SOURCE=.\App\DRIVEMGR.H
# End Source File
# Begin Source File

SOURCE=.\App\DYNARRAY.H
# End Source File
# Begin Source File

SOURCE=.\app\easyprintsdoc.h
# End Source File
# Begin Source File

SOURCE=.\app\easyprintsview.h
# End Source File
# Begin Source File

SOURCE=.\App\EditCropControl.h
# End Source File
# Begin Source File

SOURCE=.\App\EFFECTS.H
# End Source File
# Begin Source File

SOURCE=.\App\EffectsCtrls.h
# End Source File
# Begin Source File

SOURCE=.\App\Efilerec.h
# End Source File
# Begin Source File

SOURCE=.\App\ELLIOBJ.H
# End Source File
# Begin Source File

SOURCE=.\App\ENVDOC.H
# End Source File
# Begin Source File

SOURCE=.\App\ENVPRINT.H
# End Source File
# Begin Source File

SOURCE=.\App\ENVPRVW.H
# End Source File
# Begin Source File

SOURCE=.\App\ENVTYPE.H
# End Source File
# Begin Source File

SOURCE=.\App\ENVVIEW.H
# End Source File
# Begin Source File

SOURCE=.\App\ERROR.H
# End Source File
# Begin Source File

SOURCE=.\App\EVENT.H
# End Source File
# Begin Source File

SOURCE=.\App\exgraf.h
# End Source File
# Begin Source File

SOURCE=.\App\ExportGraphicDlg.h
# End Source File
# Begin Source File

SOURCE=.\App\FCACHE.H
# End Source File
# Begin Source File

SOURCE=.\App\FDIRREC.H
# End Source File
# Begin Source File

SOURCE=.\App\FeedbackDropSource.h
# End Source File
# Begin Source File

SOURCE=.\App\FFLDFILE.H
# End Source File
# Begin Source File

SOURCE=.\App\FIELDDLG.H
# End Source File
# Begin Source File

SOURCE=.\App\filcmprs.h
# End Source File
# Begin Source File

SOURCE=.\App\FILE.H
# End Source File
# Begin Source File

SOURCE=.\App\FILEDB.H
# End Source File
# Begin Source File

SOURCE=.\App\fill.h
# End Source File
# Begin Source File

SOURCE=.\App\FINDREP.H
# End Source File
# Begin Source File

SOURCE=.\App\FINDTEXT.H
# End Source File
# Begin Source File

SOURCE=.\App\FLATDB.H
# End Source File
# Begin Source File

SOURCE=.\App\Flexmath.h
# End Source File
# Begin Source File

SOURCE=.\App\Flistrec.h
# End Source File
# Begin Source File

SOURCE=.\App\Flyout.h
# End Source File
# Begin Source File

SOURCE=.\App\FlyoutToolbar.h
# End Source File
# Begin Source File

SOURCE=.\App\FontComboBox.h
# End Source File
# Begin Source File

SOURCE=.\App\fontprvw.h
# End Source File
# Begin Source File

SOURCE=.\App\FONTREC.H
# End Source File
# Begin Source File

SOURCE=.\App\FONTSRV.H
# End Source File
# Begin Source File

SOURCE=.\App\FRAMEDIT.H
# End Source File
# Begin Source File

SOURCE=.\App\FRAMEDLG.H
# End Source File
# Begin Source File

SOURCE=.\App\FRAMEOBJ.H
# End Source File
# Begin Source File

SOURCE=.\App\FRAMEREC.H
# End Source File
# Begin Source File

SOURCE=.\Resource\FrameworkResourceIDs.h
# End Source File
# Begin Source File

SOURCE=.\App\FREELIST.H
# End Source File
# Begin Source File

SOURCE=.\App\GCACHE.H
# End Source File
# Begin Source File

SOURCE=.\App\GenericGroupedObjectDialog.h
# End Source File
# Begin Source File

SOURCE=.\App\GERONIMO.H
# End Source File
# Begin Source File

SOURCE=.\App\GIF.H
# End Source File
# Begin Source File

SOURCE=.\App\GLICENSE.H
# End Source File
# Begin Source File

SOURCE=.\App\Globatom.h
# End Source File
# Begin Source File

SOURCE=.\App\GradDlg.h
# End Source File
# Begin Source File

SOURCE=.\App\GRAFOBJ.H
# End Source File
# Begin Source File

SOURCE=.\App\GRAFREC.H
# End Source File
# Begin Source File

SOURCE=.\App\GraphicCategorySearch.h
# End Source File
# Begin Source File

SOURCE=.\App\GraphicSearch.h
# End Source File
# Begin Source File

SOURCE=.\App\GrLocReq.h
# End Source File
# Begin Source File

SOURCE=.\App\GrphReq.h
# End Source File
# Begin Source File

SOURCE=.\App\GRPOBJ.H
# End Source File
# Begin Source File

SOURCE=.\App\HALO.H
# End Source File
# Begin Source File

SOURCE=.\App\HeadlineDepthPage.h
# End Source File
# Begin Source File

SOURCE=.\App\HeadlineDlg.h
# End Source File
# Begin Source File

SOURCE=.\App\HeadlineFacePage.h
# End Source File
# Begin Source File

SOURCE=.\App\HeadlineFontDlg.h
# End Source File
# Begin Source File

SOURCE=.\App\HeadlineOutlinePage.h
# End Source File
# Begin Source File

SOURCE=.\App\HeadlinePositionPage.h
# End Source File
# Begin Source File

SOURCE=.\App\HeadlineProportionPage.h
# End Source File
# Begin Source File

SOURCE=.\App\HeadlineShadowPage.h
# End Source File
# Begin Source File

SOURCE=.\App\HeadlineShapePage.h
# End Source File
# Begin Source File

SOURCE=.\App\HEAP.H
# End Source File
# Begin Source File

SOURCE=.\App\HelpHint.h
# End Source File
# Begin Source File

SOURCE=.\App\HexColorGrid.h
# End Source File
# Begin Source File

SOURCE=.\App\hhctrl.h
# End Source File
# Begin Source File

SOURCE=.\App\hint.h
# End Source File
# Begin Source File

SOURCE=.\App\htmlconv.h
# End Source File
# Begin Source File

SOURCE=.\App\Htmldoc.h
# End Source File
# Begin Source File

SOURCE=.\App\htmlhelp.h
# End Source File
# Begin Source File

SOURCE=.\App\HUBMGR.H
# End Source File
# Begin Source File

SOURCE=.\App\HybridBaseDlg.h
# End Source File
# Begin Source File

SOURCE=.\App\hyperrec.h
# End Source File
# Begin Source File

SOURCE=.\App\hyplkdlg.h
# End Source File
# Begin Source File

SOURCE=.\App\ICOLLDRW.H
# End Source File
# Begin Source File

SOURCE=.\App\icollmgr.h
# End Source File
# Begin Source File

SOURCE=.\App\ICONNMGR.H
# End Source File
# Begin Source File

SOURCE=.\App\ICONTMGR.H
# End Source File
# Begin Source File

SOURCE=.\App\icontsrv.h
# End Source File
# Begin Source File

SOURCE=.\App\IDRVMGR.H
# End Source File
# Begin Source File

SOURCE=.\App\Image.h
# End Source File
# Begin Source File

SOURCE=.\App\ImageBtn.h
# End Source File
# Begin Source File

SOURCE=.\App\Imagebuf.h
# End Source File
# Begin Source File

SOURCE=.\App\IMAGEDEF.H
# End Source File
# Begin Source File

SOURCE=.\App\IMAGEDRV.H
# End Source File
# Begin Source File

SOURCE=.\App\ImageGridCtrl.h
# End Source File
# Begin Source File

SOURCE=.\App\imagemap.h
# End Source File
# Begin Source File

SOURCE=.\App\ImagePreviewCtrl.h
# End Source File
# Begin Source File

SOURCE=.\App\Imagesrc.h
# End Source File
# Begin Source File

SOURCE=.\App\ImageUtils.h
# End Source File
# Begin Source File

SOURCE=.\App\ImgCtrl.H
# End Source File
# Begin Source File

SOURCE=.\App\Implod16.h
# End Source File
# Begin Source File

SOURCE=.\App\Implod32.h
# End Source File
# Begin Source File

SOURCE=.\App\InCompdb.h
# End Source File
# Begin Source File

SOURCE=.\App\INIFILE.H
# End Source File
# Begin Source File

SOURCE=.\App\InitCapDialog.h
# End Source File
# Begin Source File

SOURCE=.\App\InsertSymbolDialog.h
# End Source File
# Begin Source File

SOURCE=.\App\INSTDAT.H
# End Source File
# Begin Source File

SOURCE=.\App\INTLDLL.H
# End Source File
# Begin Source File

SOURCE=.\App\IPATHMGR.H
# End Source File
# Begin Source File

SOURCE=.\App\IPFRAME.H
# End Source File
# Begin Source File

SOURCE=.\App\IUSERSET.H
# End Source File
# Begin Source File

SOURCE=.\App\JPEG.H
# End Source File
# Begin Source File

SOURCE=.\App\Jpegimpl.h
# End Source File
# Begin Source File

SOURCE=.\App\JTRACE.H
# End Source File
# Begin Source File

SOURCE=.\App\KEY.H
# End Source File
# Begin Source File

SOURCE=.\App\LABELDLG.H
# End Source File
# Begin Source File

SOURCE=.\App\LABELS.H
# End Source File
# Begin Source File

SOURCE=.\App\LAYREC.H
# End Source File
# Begin Source File

SOURCE=.\App\LBLDOC.H
# End Source File
# Begin Source File

SOURCE=.\App\LBLPRINT.H
# End Source File
# Begin Source File

SOURCE=.\App\LBLPRVW.H
# End Source File
# Begin Source File

SOURCE=.\App\LBLVIEW.H
# End Source File
# Begin Source File

SOURCE=.\App\LICDLL.H
# End Source File
# Begin Source File

SOURCE=.\App\LICENSE.H
# End Source File
# Begin Source File

SOURCE=.\App\Linecros.h
# End Source File
# Begin Source File

SOURCE=.\App\LINEOBJ.H
# End Source File
# Begin Source File

SOURCE=.\App\LINEWID.H
# End Source File
# Begin Source File

SOURCE=.\App\LIST.H
# End Source File
# Begin Source File

SOURCE=.\App\LoginReq.h
# End Source File
# Begin Source File

SOURCE=.\App\LogoMakerDialog.h
# End Source File
# Begin Source File

SOURCE=.\App\Lzw.h
# End Source File
# Begin Source File

SOURCE=.\App\MACRODLG.H
# End Source File
# Begin Source File

SOURCE=.\App\MACROREC.H
# End Source File
# Begin Source File

SOURCE=.\App\MACROSRV.H
# End Source File
# Begin Source File

SOURCE=.\App\MACROSTD.H
# End Source File
# Begin Source File

SOURCE=.\App\MAINFRM.H
# End Source File
# Begin Source File

SOURCE=.\App\MARQUEE.H
# End Source File
# Begin Source File

SOURCE=.\App\Maskedbm.h
# End Source File
# Begin Source File

SOURCE=.\App\matchset.h
# End Source File
# Begin Source File

SOURCE=.\App\MCIWND.H
# End Source File
# Begin Source File

SOURCE=.\App\MEMDEV.H
# End Source File
# Begin Source File

SOURCE=.\App\MENUTYPE.H
# End Source File
# Begin Source File

SOURCE=.\App\MISC.H
# End Source File
# Begin Source File

SOURCE=.\App\MonthEdit.h
# End Source File
# Begin Source File

SOURCE=.\App\MSREG.H
# End Source File
# Begin Source File

SOURCE=.\App\MULTINET.H
# End Source File
# Begin Source File

SOURCE=.\App\NAMEREC.H
# End Source File
# Begin Source File

SOURCE=.\App\Netcount.h
# End Source File
# Begin Source File

SOURCE=.\App\NETID.H
# End Source File
# Begin Source File

SOURCE=.\App\NETUSERS.H
# End Source File
# Begin Source File

SOURCE=.\App\NEWDLG.H
# End Source File
# Begin Source File

SOURCE=.\App\NEWFONT.H
# End Source File
# Begin Source File

SOURCE=.\App\NEWSDOC.H
# End Source File
# Begin Source File

SOURCE=.\App\NEWSVIEW.H
# End Source File
# Begin Source File

SOURCE=.\App\NFX.H
# End Source File
# Begin Source File

SOURCE=.\App\Nheap.h
# End Source File
# Begin Source File

SOURCE=.\App\NOFILLIN.H
# End Source File
# Begin Source File

SOURCE=.\App\NumberDialog.h
# End Source File
# Begin Source File

SOURCE=.\App\NumberEdit.h
# End Source File
# Begin Source File

SOURCE=.\App\NWSPRINT.H
# End Source File
# Begin Source File

SOURCE=.\App\OBJDB.H
# End Source File
# Begin Source File

SOURCE=.\App\ObjectSize.h
# End Source File
# Begin Source File

SOURCE=.\App\OBJIMG.H
# End Source File
# Begin Source File

SOURCE=.\App\OBJLIST.H
# End Source File
# Begin Source File

SOURCE=.\App\OLDDOC.H
# End Source File
# Begin Source File

SOURCE=.\App\OLDGRAF.H
# End Source File
# Begin Source File

SOURCE=.\App\OLDTEXT.H
# End Source File
# Begin Source File

SOURCE=.\App\OLEOBJ.H
# End Source File
# Begin Source File

SOURCE=.\App\OnAdDlg.h
# End Source File
# Begin Source File

SOURCE=.\App\OpenDlgs.h
# End Source File
# Begin Source File

SOURCE=.\App\ORIENDLG.H
# End Source File
# Begin Source File

SOURCE=.\App\Outline.h
# End Source File
# Begin Source File

SOURCE=.\App\OUTPATH.H
# End Source File
# Begin Source File

SOURCE=.\App\OWNERCB.H
# End Source File
# Begin Source File

SOURCE=.\App\OwnerDrawMenu.h
# End Source File
# Begin Source File

SOURCE=.\App\OWNERLB.H
# End Source File
# Begin Source File

SOURCE=.\App\PAGEDEFS.H
# End Source File
# Begin Source File

SOURCE=.\App\PAGEDLGS.H
# End Source File
# Begin Source File

SOURCE=.\App\PAGEREC.H
# End Source File
# Begin Source File

SOURCE=.\App\Paint.h
# End Source File
# Begin Source File

SOURCE=.\App\PaintCap.h
# End Source File
# Begin Source File

SOURCE=.\App\PANEL.H
# End Source File
# Begin Source File

SOURCE=.\App\PAPER.H
# End Source File
# Begin Source File

SOURCE=.\App\PARADLG.H
# End Source File
# Begin Source File

SOURCE=.\App\PARAREC.H
# End Source File
# Begin Source File

SOURCE=.\App\PARSE.H
# End Source File
# Begin Source File

SOURCE=.\App\PathDocument.h
# End Source File
# Begin Source File

SOURCE=.\App\Pathit.h
# End Source File
# Begin Source File

SOURCE=.\App\PATHMGR.H
# End Source File
# Begin Source File

SOURCE=.\App\Pathrec.h
# End Source File
# Begin Source File

SOURCE=.\App\PathView.h
# End Source File
# Begin Source File

SOURCE=.\App\PATOBJ.H
# End Source File
# Begin Source File

SOURCE=.\App\PCD.H
# End Source File
# Begin Source File

SOURCE=.\App\PCDLIB.H
# End Source File
# Begin Source File

SOURCE=.\App\PCX.H
# End Source File
# Begin Source File

SOURCE=.\App\PDLGCLR.H
# End Source File
# Begin Source File

SOURCE=.\App\PDPropSt.h
# End Source File
# Begin Source File

SOURCE=.\App\Pegasus.h
# End Source File
# Begin Source File

SOURCE=.\App\PGSOrWiz.h
# End Source File
# Begin Source File

SOURCE=.\App\PGSSess.h
# End Source File
# Begin Source File

SOURCE=.\app\phoprjprint.h
# End Source File
# Begin Source File

SOURCE=.\app\phoprjprvw.h
# End Source File
# Begin Source File

SOURCE=.\app\photoprj.h
# End Source File
# Begin Source File

SOURCE=.\app\PhPrjDlg.h
# End Source File
# Begin Source File

SOURCE=.\App\PICTATTR.H
# End Source File
# Begin Source File

SOURCE=.\App\PICTPRVW.H
# End Source File
# Begin Source File

SOURCE=.\App\pmaodlg.H
# End Source File
# Begin Source File

SOURCE=.\App\PMBMBUT.H
# End Source File
# Begin Source File

SOURCE=.\App\PMCFILE.H
# End Source File
# Begin Source File

SOURCE=.\App\Pmdirdlg.h
# End Source File
# Begin Source File

SOURCE=.\App\PMGDB.H
# End Source File
# Begin Source File

SOURCE=.\App\PMGFONT.H
# End Source File
# Begin Source File

SOURCE=.\App\PMGOBJ.H
# End Source File
# Begin Source File

SOURCE=.\App\pmhelp.h
# End Source File
# Begin Source File

SOURCE=.\App\Pmobject.h
# End Source File
# Begin Source File

SOURCE=.\App\PMW.H
# End Source File
# Begin Source File

SOURCE=.\App\PMWCFG.H
# End Source File
# Begin Source File

SOURCE=.\App\PMWCOLL.H
# End Source File
# Begin Source File

SOURCE=.\App\PMWCOLOR.H
# End Source File
# Begin Source File

SOURCE=.\App\PMWDLG.H
# End Source File
# Begin Source File

SOURCE=.\App\PMWDOC.H
# End Source File
# Begin Source File

SOURCE=.\App\PMWINI.H
# End Source File
# Begin Source File

SOURCE=.\App\PMWMDI.H
# End Source File
# Begin Source File

SOURCE=.\App\PMWPRINT.H
# End Source File
# Begin Source File

SOURCE=.\App\PMWPROP.H
# End Source File
# Begin Source File

SOURCE=.\App\PMWRFL.H
# End Source File
# Begin Source File

SOURCE=.\App\PMWTEMPL.H
# End Source File
# Begin Source File

SOURCE=.\App\PMWTOOL.H
# End Source File
# Begin Source File

SOURCE=.\App\PMWVIEW.H
# End Source File
# Begin Source File

SOURCE=.\App\PMWWIZ.H
# End Source File
# Begin Source File

SOURCE=.\App\Polypoly.h
# End Source File
# Begin Source File

SOURCE=.\App\POPUP.H
# End Source File
# Begin Source File

SOURCE=.\App\POSDLG.H
# End Source File
# Begin Source File

SOURCE=.\App\POSITION.H
# End Source File
# Begin Source File

SOURCE=.\App\POSLIST.H
# End Source File
# Begin Source File

SOURCE=.\App\PreferencesDatePage.h
# End Source File
# Begin Source File

SOURCE=.\App\PREFMISC.H
# End Source File
# Begin Source File

SOURCE=.\App\PREFPSH.H
# End Source File
# Begin Source File

SOURCE=.\App\PREFSND.H
# End Source File
# Begin Source File

SOURCE=.\App\PREFWRK.H
# End Source File
# Begin Source File

SOURCE=.\App\PREVREC.H
# End Source File
# Begin Source File

SOURCE=.\App\PRINTADV.H
# End Source File
# Begin Source File

SOURCE=.\App\PRINTART.H
# End Source File
# Begin Source File

SOURCE=.\App\PRINTER.H
# End Source File
# Begin Source File

SOURCE=.\App\PRINTPRJ.H
# End Source File
# Begin Source File

SOURCE=.\App\PRINTWIZ.H
# End Source File
# Begin Source File

SOURCE=.\App\prnaddlg.h
# End Source File
# Begin Source File

SOURCE=.\App\PrntCalD.h
# End Source File
# Begin Source File

SOURCE=.\App\PRNTRNGE.H
# End Source File
# Begin Source File

SOURCE=.\App\PRNTSIZE.H
# End Source File
# Begin Source File

SOURCE=.\App\procloc.h
# End Source File
# Begin Source File

SOURCE=.\App\PROFILE.H
# End Source File
# Begin Source File

SOURCE=.\App\PROGRESS.H
# End Source File
# Begin Source File

SOURCE=.\App\PROJECT.H
# End Source File
# Begin Source File

SOURCE=.\App\PROJGRPH.H
# End Source File
# Begin Source File

SOURCE=.\App\PROJOBJ.H
# End Source File
# Begin Source File

SOURCE=.\App\PROJTEXT.H
# End Source File
# Begin Source File

SOURCE=.\App\PromtDlg.h
# End Source File
# Begin Source File

SOURCE=.\App\PROPLIST.H
# End Source File
# Begin Source File

SOURCE=.\App\PROPSTR.H
# End Source File
# Begin Source File

SOURCE=.\App\PRVWDLG.H
# End Source File
# Begin Source File

SOURCE=.\App\PSD.H
# End Source File
# Begin Source File

SOURCE=.\App\QSORT.H
# End Source File
# Begin Source File

SOURCE=.\App\Recarray.h
# End Source File
# Begin Source File

SOURCE=.\App\RECORD.H
# End Source File
# Begin Source File

SOURCE=.\App\Rectobj.h
# End Source File
# Begin Source File

SOURCE=.\App\REDISP.H
# End Source File
# Begin Source File

SOURCE=.\App\REFRESH.H
# End Source File
# Begin Source File

SOURCE=.\App\RegCtrl.h
# End Source File
# Begin Source File

SOURCE=.\App\REGISTER.H
# End Source File
# Begin Source File

SOURCE=.\App\ROTDLG.H
# End Source File
# Begin Source File

SOURCE=.\App\ROTUTILS.H
# End Source File
# Begin Source File

SOURCE=.\App\RTFPARSE.H
# End Source File
# Begin Source File

SOURCE=.\App\RULER.H
# End Source File
# Begin Source File

SOURCE=.\App\RWSEM.H
# End Source File
# Begin Source File

SOURCE=.\App\SAVEPROJ.H
# End Source File
# Begin Source File

SOURCE=.\App\ScrollableControl.h
# End Source File
# Begin Source File

SOURCE=.\App\SCROLLGA.H
# End Source File
# Begin Source File

SOURCE=.\App\SealDialog.h
# End Source File
# Begin Source File

SOURCE=.\App\SearchResult.h
# End Source File
# Begin Source File

SOURCE=.\App\SHAPEDLG.H
# End Source File
# Begin Source File

SOURCE=.\App\SHAPESEL.H
# End Source File
# Begin Source File

SOURCE=.\App\SHAPFILE.H
# End Source File
# Begin Source File

SOURCE=.\App\SHARENOT.H
# End Source File
# Begin Source File

SOURCE=.\App\SignatureDialog.h
# End Source File
# Begin Source File

SOURCE=.\App\SIGNDOC.H
# End Source File
# Begin Source File

SOURCE=.\App\SIGNVIEW.H
# End Source File
# Begin Source File

SOURCE=.\App\SIMPLECB.H
# End Source File
# Begin Source File

SOURCE=.\App\SIMPLELB.H
# End Source File
# Begin Source File

SOURCE=.\App\SMACKER.H
# End Source File
# Begin Source File

SOURCE=.\App\SORTARRY.H
# End Source File
# Begin Source File

SOURCE=.\App\SOUNDFX.H
# End Source File
# Begin Source File

SOURCE=.\App\SPACEDLG.H
# End Source File
# Begin Source File

SOURCE=.\App\SPELLCHK.H
# End Source File
# Begin Source File

SOURCE=.\App\SPELLDLG.H
# End Source File
# Begin Source File

SOURCE=.\App\SPELLENG.H
# End Source File
# Begin Source File

SOURCE=.\App\SPINBUTT.H
# End Source File
# Begin Source File

SOURCE=.\App\SPLITTER.H
# End Source File
# Begin Source File

SOURCE=.\App\SplshTxt.h
# End Source File
# Begin Source File

SOURCE=.\App\SPLTARRY.H
# End Source File
# Begin Source File

SOURCE=.\App\srvacdlg.h
# End Source File
# Begin Source File

SOURCE=.\App\SSCE.H
# End Source File
# Begin Source File

SOURCE=.\App\SSHeaderArray.h
# End Source File
# Begin Source File

SOURCE=.\App\STATBAR.H
# End Source File
# Begin Source File

SOURCE=.\App\STATICE.H
# End Source File
# Begin Source File

SOURCE=.\App\StaticSpinCtrl.h
# End Source File
# Begin Source File

SOURCE=.\App\STDAFX.H
# End Source File
# Begin Source File

SOURCE=.\App\stkdsktp.h
# End Source File
# Begin Source File

SOURCE=.\App\stkrdoc.h
# End Source File
# Begin Source File

SOURCE=.\App\StkrPrnD.h
# End Source File
# Begin Source File

SOURCE=.\App\stkrview.h
# End Source File
# Begin Source File

SOURCE=.\App\STOREMAN.H
# End Source File
# Begin Source File

SOURCE=.\App\STRCLASS.H
# End Source File
# Begin Source File

SOURCE=.\App\STRMCACH.H
# End Source File
# Begin Source File

SOURCE=.\App\STYLE.H
# End Source File
# Begin Source File

SOURCE=.\App\Stylehlp.h
# End Source File
# Begin Source File

SOURCE=.\App\STYLEIT.H
# End Source File
# Begin Source File

SOURCE=.\App\STYLEREC.H
# End Source File
# Begin Source File

SOURCE=.\App\SvBorder.h
# End Source File
# Begin Source File

SOURCE=.\App\svpicdlg.h
# End Source File
# Begin Source File

SOURCE=.\App\SYSTEM.H
# End Source File
# Begin Source File

SOURCE=.\App\TabbedEditAdjustCol.h
# End Source File
# Begin Source File

SOURCE=.\App\TabbedEditArtEffect.h
# End Source File
# Begin Source File

SOURCE=.\App\TabbedEditBrightFocus.h
# End Source File
# Begin Source File

SOURCE=.\App\TabbedEditCrop.h
# End Source File
# Begin Source File

SOURCE=.\App\TabbedEditFixFlaw.h
# End Source File
# Begin Source File

SOURCE=.\App\TabbedEditImage.h
# End Source File
# Begin Source File

SOURCE=.\App\TABLEMAN.H
# End Source File
# Begin Source File

SOURCE=.\App\TABSDLG.H
# End Source File
# Begin Source File

SOURCE=.\App\TBLFMT.H
# End Source File
# Begin Source File

SOURCE=.\App\TblObj.h
# End Source File
# Begin Source File

SOURCE=.\App\TblPrpD.h
# End Source File
# Begin Source File

SOURCE=.\App\TCOMBO.H
# End Source File
# Begin Source File

SOURCE=.\App\TCOMMAND.H
# End Source File
# Begin Source File

SOURCE=.\App\TESTPRNT.H
# End Source File
# Begin Source File

SOURCE=.\App\TEXT.H
# End Source File
# Begin Source File

SOURCE=.\App\TEXTARRY.H
# End Source File
# Begin Source File

SOURCE=.\App\TEXTFLOW.H
# End Source File
# Begin Source File

SOURCE=.\App\Textline.h
# End Source File
# Begin Source File

SOURCE=.\App\TEXTREC.H
# End Source File
# Begin Source File

SOURCE=.\App\TFENGINE.H
# End Source File
# Begin Source File

SOURCE=.\App\TIFF.H
# End Source File
# Begin Source File

SOURCE=.\App\TIFFDRV.H
# End Source File
# Begin Source File

SOURCE=.\App\TILESBTN.H
# End Source File
# Begin Source File

SOURCE=.\App\TimePieceDlg.h
# End Source File
# Begin Source File

SOURCE=.\App\timestmp.h
# End Source File
# Begin Source File

SOURCE=.\App\TIMPORT.H
# End Source File
# Begin Source File

SOURCE=.\App\tinycash.h
# End Source File
# Begin Source File

SOURCE=.\App\TOOLBARX.H
# End Source File
# Begin Source File

SOURCE=.\App\TransBtn.h
# End Source File
# Begin Source File

SOURCE=.\App\TRUETYPE.H
# End Source File
# Begin Source File

SOURCE=.\App\TTDLL.H
# End Source File
# Begin Source File

SOURCE=.\App\TutList.h
# End Source File
# Begin Source File

SOURCE=.\App\TutorDlg.h
# End Source File
# Begin Source File

SOURCE=.\App\TWAIN.H
# End Source File
# Begin Source File

SOURCE=.\App\TXPDEST.H
# End Source File
# Begin Source File

SOURCE=.\App\TYPEFACE.H
# End Source File
# Begin Source File

SOURCE=.\App\UPGDLL.H
# End Source File
# Begin Source File

SOURCE=.\App\UPGRADLG.H
# End Source File
# Begin Source File

SOURCE=.\App\USERSET.H
# End Source File
# Begin Source File

SOURCE=.\App\UsrCtgry.h
# End Source File
# Begin Source File

SOURCE=.\App\Util.h
# End Source File
# Begin Source File

SOURCE=.\App\UTILS.H
# End Source File
# Begin Source File

SOURCE=.\App\UUENCODE.H
# End Source File
# Begin Source File

SOURCE=.\App\VFL.H
# End Source File
# Begin Source File

SOURCE=.\App\WARPFILE.H
# End Source File
# Begin Source File

SOURCE=.\App\WARPFLD.H
# End Source File
# Begin Source File

SOURCE=.\App\WARPOBJ.H
# End Source File
# Begin Source File

SOURCE=.\App\WARPSEL.H
# End Source File
# Begin Source File

SOURCE=.\App\webdef.h
# End Source File
# Begin Source File

SOURCE=.\App\WebDlg.h
# End Source File
# Begin Source File

SOURCE=.\App\webdoc.H
# End Source File
# Begin Source File

SOURCE=.\App\WebPubQ.h
# End Source File
# Begin Source File

SOURCE=.\App\webview.H
# End Source File
# Begin Source File

SOURCE=.\App\WHCalend.h
# End Source File
# Begin Source File

SOURCE=.\App\WHGreet.h
# End Source File
# Begin Source File

SOURCE=.\App\WHLabel.h
# End Source File
# Begin Source File

SOURCE=.\App\WHlpCmn.h
# End Source File
# Begin Source File

SOURCE=.\App\WHlpProj.h
# End Source File
# Begin Source File

SOURCE=.\App\WHlpView.h
# End Source File
# Begin Source File

SOURCE=.\App\WHlpWiz.h
# End Source File
# Begin Source File

SOURCE=.\App\WHLtrhd.h
# End Source File
# Begin Source File

SOURCE=.\App\WHNwsltr.h
# End Source File
# Begin Source File

SOURCE=.\App\WHPoster.h
# End Source File
# Begin Source File

SOURCE=.\App\WHWebPg.h
# End Source File
# Begin Source File

SOURCE=.\App\WIDTHSEL.H
# End Source File
# Begin Source File

SOURCE=.\App\WindowsImageList.h
# End Source File
# Begin Source File

SOURCE=.\App\WINNET.H
# End Source File
# Begin Source File

SOURCE=.\App\WinTextureBrowser.h
# End Source File
# Begin Source File

SOURCE=.\App\WMF.H
# End Source File
# Begin Source File

SOURCE=.\App\WNDBIG.H
# End Source File
# Begin Source File

SOURCE=.\App\WNDHUB.H
# End Source File
# Begin Source File

SOURCE=.\App\WPG.H
# End Source File
# Begin Source File

SOURCE=.\App\XERROR.H
# End Source File
# Begin Source File

SOURCE=.\App\Yeardll.h
# End Source File
# Begin Source File

SOURCE=.\App\YywbDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Resource\Shared\photoprojects.ico
# End Source File
# End Group
# Begin Source File

SOURCE=.\App\Pegasus\Picnm.lib
# End Source File
# Begin Source File

SOURCE=.\Lib\Implod32.lib
# End Source File
# Begin Source File

SOURCE=.\Smacker\Smkw32ms.lib
# End Source File
# Begin Source File

SOURCE=.\App\Al201\V4x\Alfwd.lib
# End Source File
# End Target
# End Project
