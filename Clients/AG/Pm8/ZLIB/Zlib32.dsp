# Microsoft Developer Studio Project File - Name="ZLIB Library" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=ZLIB Library - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Zlib32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Zlib32.mak" CFG="ZLIB Library - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ZLIB Library - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ZLIB Library - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PMW", UYAAAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ZLIB Library - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp2 /MT /W3 /GX /Zi /Ot /Oi /Oy /Ob1 /Gf /Gy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# SUBTRACT CPP /Ox /Oa /Ow
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "ZLIB Library - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp2 /MTd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "ZLIB Library - Win32 Release"
# Name "ZLIB Library - Win32 Debug"
# Begin Source File

SOURCE=.\ADLER32.C
# End Source File
# Begin Source File

SOURCE=.\COMPRESS.C
# End Source File
# Begin Source File

SOURCE=.\CRC32.C
# End Source File
# Begin Source File

SOURCE=.\DEFLATE.C
# End Source File
# Begin Source File

SOURCE=.\GZIO.C
# End Source File
# Begin Source File

SOURCE=.\INFBLOCK.C
# End Source File
# Begin Source File

SOURCE=.\INFCODES.C
# End Source File
# Begin Source File

SOURCE=.\INFFAST.C
# End Source File
# Begin Source File

SOURCE=.\INFLATE.C
# End Source File
# Begin Source File

SOURCE=.\INFTREES.C
# End Source File
# Begin Source File

SOURCE=.\INFUTIL.C
# End Source File
# Begin Source File

SOURCE=.\MINIGZIP.C
# End Source File
# Begin Source File

SOURCE=.\TREES.C
# End Source File
# Begin Source File

SOURCE=.\UNCOMPR.C
# End Source File
# Begin Source File

SOURCE=.\ZUTIL.C
# End Source File
# End Target
# End Project
