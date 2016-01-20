# Microsoft Developer Studio Project File - Name="CIndex" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CIndex - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cindexnt.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cindexnt.mak" CFG="CIndex - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CIndex - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CIndex - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/PMW", UYAAAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CIndex - Win32 Release"

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
# ADD CPP /nologo /Zp1 /MT /W3 /GX /Zi /Ot /Oi /Oy /Ob1 /Gf /Gy /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D i386=1 /D "_X86_" /D "MSC" /D "NT" /YX /FD /c
# SUBTRACT CPP /Ox /Oa /Ow
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "CIndex - Win32 Debug"

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
# ADD CPP /nologo /Zp1 /MTd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D i386=1 /D "_X86_" /D "MSC" /D "NT" /FR /YX /FD /c
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

# Name "CIndex - Win32 Release"
# Name "CIndex - Win32 Debug"
# Begin Source File

SOURCE=.\BUFFERS.C
# End Source File
# Begin Source File

SOURCE=.\BYTEFLIP.C
# End Source File
# Begin Source File

SOURCE=.\CCHECK.C
# End Source File
# Begin Source File

SOURCE=.\CCONVERT.C
# End Source File
# Begin Source File

SOURCE=.\CINDEXMU.C
# End Source File
# Begin Source File

SOURCE=.\CINDEXS.C
# End Source File
# Begin Source File

SOURCE=.\CNDXNT.C
# End Source File
# Begin Source File

SOURCE=.\CURKEY.C
# End Source File
# Begin Source File

SOURCE=.\DCHECK.C
# End Source File
# Begin Source File

SOURCE=.\DELETE.C
# End Source File
# Begin Source File

SOURCE=.\DFILE.C
# End Source File
# Begin Source File

SOURCE=.\DFUNC.C
# End Source File
# Begin Source File

SOURCE=.\DISKIO.C
# End Source File
# Begin Source File

SOURCE=.\DREAD.C
# End Source File
# Begin Source File

SOURCE=.\DWRITE.C
# End Source File
# Begin Source File

SOURCE=.\ENTPROC.C
# End Source File
# Begin Source File

SOURCE=.\READ.C
# End Source File
# Begin Source File

SOURCE=.\RELKEY.C
# End Source File
# Begin Source File

SOURCE=.\WRITE.C
# End Source File
# End Target
# End Project
