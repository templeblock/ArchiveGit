# Microsoft Developer Studio Project File - Name="Framework" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Framework - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Framework.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Framework.mak" CFG="Framework - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Framework - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Framework - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PrintShop/Development", NQZAAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Framework - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir "."
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\winRel\"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "."
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W4 /GR /GX /Zi /O1 /I "..\ConfigurationDetection" /I "..\ConfigurationDetection\include" /I "..\Atm\Windows" /I "..\Accusoft" /I "..\TpsLibs\Include" /I ".\Include" /I "..\Resource" /I "..\Interfaces" /I "..\Inso\CorrectSpell\Include" /I "..\Inso\IntelliFinder\Include" /I "..\Resource\PS" /I "..\Png" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN" /D "_AFXDLL" /D "_MBCS" /D "FRAMEWORK" /D "_WINDLL" /Yu"FrameworkIncludes.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\winRel/Pmw.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ..\ConfigurationDetection\release\CfgDtect.lib version.lib mpr.lib /nologo /subsystem:windows /dll /pdb:none /debug /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libc"

!ELSEIF  "$(CFG)" == "Framework - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir "."
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\winDebug\"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "."
# ADD BASE CPP /nologo /MD /W4 /GR /GX /O2 /I "Smartheap\Include" /I "Atm\Windows" /I "TpsLibs\Include" /I "Framework\Include" /I "Accusoft" /I "Resource\Include" /I "." /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "FRAMEWORK" /Yu"FrameworkIncludes.h" /c
# SUBTRACT BASE CPP /Fr
# ADD CPP /nologo /MDd /W4 /Gm /Gi /GR /GX /Zi /Od /I "..\ConfigurationDetection\include" /I "..\Atm\Windows" /I "..\Accusoft" /I "..\TpsLibs\Include" /I ".\Include" /I "..\Resource" /I "..\Interfaces" /I "..\Inso\CorrectSpell\Include" /I "..\Inso\IntelliFinder\Include" /I "..\Resource\PS" /I "..\Png" /D "_DEBUG" /D "TPSDEBUG" /D "MEM_DEBUG" /D "WIN32" /D "_WINDOWS" /D "WIN" /D "_AFXDLL" /D "_MBCS" /D "FRAMEWORK" /D "_WINDLL" /Fr /Yu"FrameworkIncludes.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\winDebug/Pmw.bsc"
LINK32=link.exe
# ADD BASE LINK32 msvcrt.lib SmartHeap\Lib\shdw32m.lib accusoft\accuisr5.lib /nologo /base:"0x11200000" /subsystem:windows /dll /machine:I386 /nodefaultlib:"libcmt" /nodefaultlib:"libc"
# ADD LINK32 ..\ConfigurationDetection\debug\CfgDtect.lib version.lib mpr.lib /nologo /subsystem:windows /dll /incremental:yes /debug /machine:I386 /nodefaultlib:"libcd" /nodefaultlib:"libcmt" /nodefaultlib:"msvcrt" /out:"..\winDebug\FrameworkDebug.dll"
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "Framework - Win32 Release"
# Name "Framework - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Source\2dtransform.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Action.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ActivateComponentAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Alert.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AlignDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AlignSelectionAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Application.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ApplicationGlobals.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ATMFont.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\AutoDrawingSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\BackgroundColorSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Bezier.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\BreakGroupAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\BufferStream.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Caret.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CellContents.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ChainedUndoableAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ChangeBackgroundColorAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ChangeTintAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ChangeURLAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Character.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CharacterInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ChunkyBackstore.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ChunkyStorage.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ClipboardDataTransfer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ClipPolygon.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CollectionManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Color.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ColorDialogInterfaceImp.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ColorPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ColorSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CommandUI.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CommonStandaloneDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CommonView.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ComponentApplication.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ComponentAttributes.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ComponentCollection.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ComponentDataRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ComponentDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ComponentEdgeOutlineSelectionAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ComponentGlowSelectionAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ComponentManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ComponentShadowSelectionAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ComponentSpecialEdgeAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ComponentView.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ComponentVignetteSelectionAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CompositeSelection.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Configuration.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ConfigurationBase.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ControlContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CopySelectionAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\CorrectSpell.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Cursor.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\DataObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\DataTransfer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\DateTime.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\DcDrawingSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\DeactivateComponentAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\DefaultDrawingSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\DeleteSelectionAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\DeviceDepBitmapImage.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\DialCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\DialogUtilities.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\DibOutline.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Document.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\DragDropAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\DragDropDataTransfer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\DrawingSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\DuplicateSelectionAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\EdgeOutlineSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\EditComponentAttribAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\EditComponentAttribDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\EditFileFailure.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\EditMemFailure.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ExceptionHandling.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\FileCloseAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\FileFailDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\FileFormat.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\FileLocator.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\FileOpenAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\FilePrintAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\FileSaveAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\FileType.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\FixFlaw.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\FlipSelectionAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\FolderDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Font.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\FontLists.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Frame.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\FrameColorSelectionAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\FramesDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\FrameSelectionAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Resource\Framework.rc
# ADD BASE RSC /l 0x409 /i "Resource"
# ADD RSC /l 0x409 /i "Resource" /i "..\Resource"
# End Source File
# Begin Source File

SOURCE=.\Source\FrameworkIncludes.cpp
# ADD CPP /W3 /Yc"FrameworkIncludes.h"
# End Source File
# Begin Source File

SOURCE=.\Source\GdiObjects.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GifAnimator.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GifReader.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GlobalBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GradientFill.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GradientFillBackdropDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Grid.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\GroupSelectionAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\HyperlinkDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\HyperlinkNode.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ImageColor.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\IntelliFinder.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\LzCompression.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\LzwBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Mainframe.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\MdiChildFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\MemoryFailDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Menu.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\MergeData.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\MfcDataTransfer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\MouseTracker.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\MoveSelectionAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\MultiUndoableAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\MultiUndoManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\NameAddress.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\NewComponentAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\NewHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Object.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\OrderSelectionActions.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PaneView.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PasteAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Path.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PersistantObject.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PixelGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PolyPolygon.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PostScriptDrawingSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PreviewCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Printer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PrinterCompensation.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PrinterDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PrinterDrawingSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PrintManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PrintQueue.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\PropertyAttribImp.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\QuadraticSpline.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\RasterFont.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\RDocID.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ReadRegistryKey.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\RenderCache.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ReplaceComponentAction.cpp
# End Source File
# Begin Source File

SOURCE=.\SOURCE\ReplaceImageAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ResizeSelectionAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ResourceManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\RFileFinder.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\RotateDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\RotateSelectionAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\scaledialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ScaleSelectionToParentAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Script.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ScrollBar.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Selection.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\SelectionRectTracker.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\SelectionTracker.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\SetLockStateActions.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\SingleGraphic.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\SingleSelection.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\SliderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\SoftGlowSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\SoftShadowSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\SoftVignetteSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\SolidColor.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\SpellCheckDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\SpellChecker.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\SplashScreen.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\StandaloneApplication.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\StandaloneView.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Storage.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Stream.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\StreamScript.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\SymbolGridCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\TestPageResultsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ThesaurusDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ThreadSupport.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Timer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\TintSettings.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\TrackingFeedbackRenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\TransparentUndoableAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\TrueTypeFont.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\UIContextData.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\UndoableAction.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\UndoManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\UnitConversion.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\URL.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\URLList.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\Utilities.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\VectorRect.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\VersionInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ViewCommandHandlers.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\ViewDrawingSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WarpPath.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WHPTriple.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinAngleCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinBitmapImage.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinColorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WindowsFileStream.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WindowsMBCString.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WindowsMemMapFileStream.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WindowsMemoryStream.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WindowsMfcFileStream.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WindowsPictDrawingSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WindowsSaveAsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WindowsStandaloneDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WindowView.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinGridCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinImageLibrary.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinMemoryMappedBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinOffscreenDrawingSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinPostScriptDrawingSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinPrinter.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinPrinterDrawingSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinPrinterStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinPrintManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinPrintStatusDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\Source\WinVectorImage.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\Include\2dTransform.h
# End Source File
# Begin Source File

SOURCE=.\Include\Action.h
# End Source File
# Begin Source File

SOURCE=.\Include\ActivateComponentAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\Alert.h
# End Source File
# Begin Source File

SOURCE=.\Include\Algorithms.h
# End Source File
# Begin Source File

SOURCE=.\Include\AlignDialog.h
# End Source File
# Begin Source File

SOURCE=.\Include\AlignSelectionAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\Application.h
# End Source File
# Begin Source File

SOURCE=.\Include\ApplicationGlobals.h
# End Source File
# Begin Source File

SOURCE=.\Include\Archive.h
# End Source File
# Begin Source File

SOURCE=.\Include\ArchiveSupport.h
# End Source File
# Begin Source File

SOURCE=.\Include\Array.H
# End Source File
# Begin Source File

SOURCE=.\Include\ATMFont.h
# End Source File
# Begin Source File

SOURCE=.\Include\AutoDrawingSurface.h
# End Source File
# Begin Source File

SOURCE=.\Include\Bezier.h
# End Source File
# Begin Source File

SOURCE=.\Include\BitmapImage.h
# End Source File
# Begin Source File

SOURCE=.\Include\BreakGroupAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\Buffer.h
# End Source File
# Begin Source File

SOURCE=.\Include\BufferStream.h
# End Source File
# Begin Source File

SOURCE=.\Include\Cache.h
# End Source File
# Begin Source File

SOURCE=.\Include\Caret.h
# End Source File
# Begin Source File

SOURCE=.\Include\ChainedUndoableAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\ChangeBackgroundColorAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\ChangeTintAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\ChangeURLAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\Character.h
# End Source File
# Begin Source File

SOURCE=.\Include\CharacterInfo.h
# End Source File
# Begin Source File

SOURCE=.\Include\ChunkyBackstore.h
# End Source File
# Begin Source File

SOURCE=.\Include\ChunkyStorage.h
# End Source File
# Begin Source File

SOURCE=.\Include\ClipboardDataTransfer.h
# End Source File
# Begin Source File

SOURCE=.\Include\ClipPolygon.h
# End Source File
# Begin Source File

SOURCE=.\Include\CollectionManager.h
# End Source File
# Begin Source File

SOURCE=.\Include\Color.h
# End Source File
# Begin Source File

SOURCE=.\Include\Color.inl
# End Source File
# Begin Source File

SOURCE=.\Include\ColorPalette.h
# End Source File
# Begin Source File

SOURCE=.\Include\ColorSettings.h
# End Source File
# Begin Source File

SOURCE=.\Include\CommandMap.h
# End Source File
# Begin Source File

SOURCE=.\Include\CommandTarget.h
# End Source File
# Begin Source File

SOURCE=.\Include\CommandUI.h
# End Source File
# Begin Source File

SOURCE=.\Include\ComponentApplication.h
# End Source File
# Begin Source File

SOURCE=.\Include\ComponentAttributes.h
# End Source File
# Begin Source File

SOURCE=.\Include\ComponentCollection.h
# End Source File
# Begin Source File

SOURCE=.\Include\ComponentDataRenderer.h
# End Source File
# Begin Source File

SOURCE=.\Include\ComponentDocument.h
# End Source File
# Begin Source File

SOURCE=.\Include\ComponentGlowSelectionAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\ComponentManager.h
# End Source File
# Begin Source File

SOURCE=.\Include\ComponentShadowSelectionAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\ComponentView.h
# End Source File
# Begin Source File

SOURCE=.\Include\ComponentVignetteSelectionAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\CompositeSelection.h
# End Source File
# Begin Source File

SOURCE=.\Include\Configuration.h
# End Source File
# Begin Source File

SOURCE=.\Include\ConfigurationBase.h
# End Source File
# Begin Source File

SOURCE=.\Include\ControlContainer.h
# End Source File
# Begin Source File

SOURCE=.\Include\CopySelectionAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\CorrectSpell.h
# End Source File
# Begin Source File

SOURCE=.\Include\Cursor.h
# End Source File
# Begin Source File

SOURCE=.\Include\DataObject.h
# End Source File
# Begin Source File

SOURCE=.\Include\DataTransfer.h
# End Source File
# Begin Source File

SOURCE=.\Include\DateTime.h
# End Source File
# Begin Source File

SOURCE=.\Include\DateTime.inl
# End Source File
# Begin Source File

SOURCE=.\Include\DcDrawingSurface.h
# End Source File
# Begin Source File

SOURCE=.\Include\DcDrawingSurface.inl
# End Source File
# Begin Source File

SOURCE=.\Include\DeactivateComponentAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\DefaultDrawingSurface.h
# End Source File
# Begin Source File

SOURCE=.\Include\DeleteSelectionAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\DeviceDepBitmapImage.h
# End Source File
# Begin Source File

SOURCE=.\Include\DialCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Include\DialogUtilities.h
# End Source File
# Begin Source File

SOURCE=.\Include\DibOutline.h
# End Source File
# Begin Source File

SOURCE=.\Include\Document.h
# End Source File
# Begin Source File

SOURCE=.\Include\DragDropAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\DragDropDataTransfer.h
# End Source File
# Begin Source File

SOURCE=.\Include\DrawingSurface.h
# End Source File
# Begin Source File

SOURCE=.\Include\DrawingSurface.inl
# End Source File
# Begin Source File

SOURCE=.\Include\DuplicateSelectionAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\EditComponentAttribAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\EditComponentAttribDialog.h
# End Source File
# Begin Source File

SOURCE=.\Include\EditFileFailure.h
# End Source File
# Begin Source File

SOURCE=.\Include\EditMemFailure.h
# End Source File
# Begin Source File

SOURCE=.\Include\ExceptionHandling.h
# End Source File
# Begin Source File

SOURCE=.\Include\FileCloseAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\FileFailDlg.h
# End Source File
# Begin Source File

SOURCE=.\Include\FileFormat.h
# End Source File
# Begin Source File

SOURCE=.\Include\FileLocator.h
# End Source File
# Begin Source File

SOURCE=.\Include\FileOpenAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\FilePrintAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\FileSaveAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\FileStream.h
# End Source File
# Begin Source File

SOURCE=.\Include\FileType.h
# End Source File
# Begin Source File

SOURCE=.\Include\FlipSelectionAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\FloatingPoint.h
# End Source File
# Begin Source File

SOURCE=.\Include\FolderDialog.h
# End Source File
# Begin Source File

SOURCE=.\Include\Font.h
# End Source File
# Begin Source File

SOURCE=.\Include\FontLists.h
# End Source File
# Begin Source File

SOURCE=.\Include\Frame.h
# End Source File
# Begin Source File

SOURCE=.\Include\FrameColorSelectionAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\FramesDialog.h
# End Source File
# Begin Source File

SOURCE=.\Include\FrameSelectionAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\FrameworkBaseTypes.h
# End Source File
# Begin Source File

SOURCE=.\Include\FrameworkComplexTypes.h
# End Source File
# Begin Source File

SOURCE=.\Include\FrameworkIncludes.h
# End Source File
# Begin Source File

SOURCE=.\Include\FrameworkLinkage.h
# End Source File
# Begin Source File

SOURCE=.\Include\GDIObjects.h
# End Source File
# Begin Source File

SOURCE=.\Include\GifAnimator.h
# End Source File
# Begin Source File

SOURCE=.\Include\GIFReader.h
# End Source File
# Begin Source File

SOURCE=.\Include\GlobalBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Include\GpDrawingSurface.h
# End Source File
# Begin Source File

SOURCE=.\Include\GradientFill.h
# End Source File
# Begin Source File

SOURCE=.\Include\GradientFillBackdropDialog.h
# End Source File
# Begin Source File

SOURCE=.\Include\Graphic.h
# End Source File
# Begin Source File

SOURCE=.\Include\Grid.h
# End Source File
# Begin Source File

SOURCE=.\Include\Grid.inl
# End Source File
# Begin Source File

SOURCE=.\Include\GroupSelectionAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\HyperlinkDialog.h
# End Source File
# Begin Source File

SOURCE=.\Include\HyperlinkNode.h
# End Source File
# Begin Source File

SOURCE=.\Include\IdleProcessing.h
# End Source File
# Begin Source File

SOURCE=.\Include\Image.h
# End Source File
# Begin Source File

SOURCE=.\Include\ImageLibrary.h
# End Source File
# Begin Source File

SOURCE=.\Include\IntelliFinder.h
# End Source File
# Begin Source File

SOURCE=.\Include\LzCompression.h
# End Source File
# Begin Source File

SOURCE=.\Include\LzwBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Include\Mainframe.h
# End Source File
# Begin Source File

SOURCE=.\Include\MBCString.h
# End Source File
# Begin Source File

SOURCE=.\Include\MdiChildFrame.h
# End Source File
# Begin Source File

SOURCE=.\Include\MemoryFailDlg.h
# End Source File
# Begin Source File

SOURCE=.\Include\MemoryMappedBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Include\MemoryMappedStream.h
# End Source File
# Begin Source File

SOURCE=.\Include\MemoryStream.h
# End Source File
# Begin Source File

SOURCE=.\Include\Menu.h
# End Source File
# Begin Source File

SOURCE=.\Include\MergeData.h
# End Source File
# Begin Source File

SOURCE=.\Include\MessageDispatch.h
# End Source File
# Begin Source File

SOURCE=.\Include\MfcDataTransfer.h
# End Source File
# Begin Source File

SOURCE=.\Include\MfcFileStream.h
# End Source File
# Begin Source File

SOURCE=.\Include\MouseTracker.h
# End Source File
# Begin Source File

SOURCE=.\Include\MoveSelectionAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\MultiUndoableAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\MultiUndoManager.h
# End Source File
# Begin Source File

SOURCE=.\Include\NameAddress.h
# End Source File
# Begin Source File

SOURCE=.\Include\NewComponentAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\NewHandler.h
# End Source File
# Begin Source File

SOURCE=.\Include\Object.h
# End Source File
# Begin Source File

SOURCE=.\Include\OffscreenDrawingSurface.h
# End Source File
# Begin Source File

SOURCE=.\Include\OrderSelectionActions.h
# End Source File
# Begin Source File

SOURCE=.\Include\PaneView.h
# End Source File
# Begin Source File

SOURCE=.\Include\PasteAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\Path.h
# End Source File
# Begin Source File

SOURCE=.\Include\PersistantObject.h
# End Source File
# Begin Source File

SOURCE=.\Include\PictureDrawingSurface.h
# End Source File
# Begin Source File

SOURCE=.\Include\PixelGrid.h
# End Source File
# Begin Source File

SOURCE=.\Include\Point.h
# End Source File
# Begin Source File

SOURCE=.\Include\PolyPolygon.h
# End Source File
# Begin Source File

SOURCE=.\Include\PostScriptDrawingSurface.h
# End Source File
# Begin Source File

SOURCE=.\Include\PreviewCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Include\Printer.h
# End Source File
# Begin Source File

SOURCE=.\Include\PrinterCompensation.h
# End Source File
# Begin Source File

SOURCE=.\Include\PrinterDatabase.h
# End Source File
# Begin Source File

SOURCE=.\Include\PrinterDrawingSurface.h
# End Source File
# Begin Source File

SOURCE=.\Include\PrinterStatus.h
# End Source File
# Begin Source File

SOURCE=.\Include\PrintManager.h
# End Source File
# Begin Source File

SOURCE=.\Include\PrintQueue.h
# End Source File
# Begin Source File

SOURCE=.\Include\PrintStatusDialog.h
# End Source File
# Begin Source File

SOURCE=.\Include\PropertyAttribImp.h
# End Source File
# Begin Source File

SOURCE=.\Include\QuadraticSpline.h
# End Source File
# Begin Source File

SOURCE=.\Include\RasterFont.h
# End Source File
# Begin Source File

SOURCE=.\Include\RDocID.h
# End Source File
# Begin Source File

SOURCE=.\Include\ReadRegistryKey.h
# End Source File
# Begin Source File

SOURCE=.\Include\Rect.h
# End Source File
# Begin Source File

SOURCE=.\Include\RenderCache.h
# End Source File
# Begin Source File

SOURCE=.\Include\ReplaceComponentAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\ResizeSelectionAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\ResourceManager.h
# End Source File
# Begin Source File

SOURCE=.\Include\RFileFinder.h
# End Source File
# Begin Source File

SOURCE=.\Include\RotateDialog.h
# End Source File
# Begin Source File

SOURCE=.\Include\RotateSelectionAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\SaveAsDialog.h
# End Source File
# Begin Source File

SOURCE=.\Include\ScaleDialog.h
# End Source File
# Begin Source File

SOURCE=.\Include\ScratchBitmapImage.h
# End Source File
# Begin Source File

SOURCE=.\Include\Script.h
# End Source File
# Begin Source File

SOURCE=.\Include\ScrollBar.h
# End Source File
# Begin Source File

SOURCE=.\Include\Selection.h
# End Source File
# Begin Source File

SOURCE=.\Include\SelectionRectTracker.h
# End Source File
# Begin Source File

SOURCE=.\Include\SelectionTracker.h
# End Source File
# Begin Source File

SOURCE=.\Include\SetLockStateActions.h
# End Source File
# Begin Source File

SOURCE=.\Include\Settings.h
# End Source File
# Begin Source File

SOURCE=.\Include\SingleGraphic.h
# End Source File
# Begin Source File

SOURCE=.\Include\SingleSelection.h
# End Source File
# Begin Source File

SOURCE=.\Include\Size.h
# End Source File
# Begin Source File

SOURCE=.\Include\SliderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Include\SoftGlowSettings.h
# End Source File
# Begin Source File

SOURCE=.\Include\SoftShadowSettings.h
# End Source File
# Begin Source File

SOURCE=.\Include\SoftVignetteSettings.h
# End Source File
# Begin Source File

SOURCE=.\Include\SolidColor.h
# End Source File
# Begin Source File

SOURCE=.\Include\SpellCheckDialog.h
# End Source File
# Begin Source File

SOURCE=.\Include\SpellChecker.h
# End Source File
# Begin Source File

SOURCE=.\Include\SplashScreen.h
# End Source File
# Begin Source File

SOURCE=.\Include\StandaloneApplication.h
# End Source File
# Begin Source File

SOURCE=.\Include\StandaloneDocument.h
# End Source File
# Begin Source File

SOURCE=.\Include\StandaloneView.h
# End Source File
# Begin Source File

SOURCE=.\Include\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\Include\StorageSupport.h
# End Source File
# Begin Source File

SOURCE=.\Include\Stream.h
# End Source File
# Begin Source File

SOURCE=.\Include\StreamScript.h
# End Source File
# Begin Source File

SOURCE=.\Include\SymbolGridCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Include\TestPageResultsDlg.h
# End Source File
# Begin Source File

SOURCE=.\Include\ThesaurusDialog.h
# End Source File
# Begin Source File

SOURCE=.\Include\ThreadSupport.h
# End Source File
# Begin Source File

SOURCE=.\Include\TileAttributes.h
# End Source File
# Begin Source File

SOURCE=.\Include\Timer.h
# End Source File
# Begin Source File

SOURCE=.\Include\TintSettings.h
# End Source File
# Begin Source File

SOURCE=.\Include\TrackingFeedbackRenderer.h
# End Source File
# Begin Source File

SOURCE=.\Include\TransparentUndoableAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\TrueTypeFont.h
# End Source File
# Begin Source File

SOURCE=.\Include\twainWrapper.h
# End Source File
# Begin Source File

SOURCE=.\Include\UIContextData.h
# End Source File
# Begin Source File

SOURCE=.\Include\UndoableAction.h
# End Source File
# Begin Source File

SOURCE=.\Include\UndoManager.h
# End Source File
# Begin Source File

SOURCE=.\Include\UnitConversion.h
# End Source File
# Begin Source File

SOURCE=.\Include\Utilities.h
# End Source File
# Begin Source File

SOURCE=.\Include\VectorImage.h
# End Source File
# Begin Source File

SOURCE=.\Include\VectorRect.h
# End Source File
# Begin Source File

SOURCE=.\Include\VersionInfo.h
# End Source File
# Begin Source File

SOURCE=.\Include\View.h
# End Source File
# Begin Source File

SOURCE=.\Include\ViewDrawingSurface.h
# End Source File
# Begin Source File

SOURCE=.\Include\WarpPath.h
# End Source File
# Begin Source File

SOURCE=.\Include\WHPTriple.h
# End Source File
# Begin Source File

SOURCE=.\Include\WinAngleCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Include\WinColorDlg.h
# End Source File
# Begin Source File

SOURCE=.\Include\WindowView.h
# End Source File
# Begin Source File

SOURCE=.\Include\WinGridCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Include\WinGridCtrl.inl
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=..\Atm\Windows\Atm.lib
# End Source File
# Begin Source File

SOURCE=..\AccuSoft\Gear32sd.lib
# End Source File
# Begin Source File

SOURCE=..\Inso\CorrectSpell\Bins\Icdllw32.lib
# End Source File
# Begin Source File

SOURCE=..\Inso\CorrectSpell\Bins\Pddllw32.lib
# End Source File
# Begin Source File

SOURCE=..\Inso\IntelliFinder\Bins\thes.lib
# End Source File
# End Target
# End Project
