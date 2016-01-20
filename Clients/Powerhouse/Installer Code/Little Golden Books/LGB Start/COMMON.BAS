Attribute VB_Name = "basCommon"
Global gstrTitleExe As String

'This string needs to be the same as the "Title=" line of the SETUP.LST and it is case sensitive
Global Const gstrAppName$ = "Right's Animal Farm"
Global Const gstrIniFile$ = "LGBOOK.INI"                'Program *.INI File

#If Win16 Then
'
'API/DLL Declarations for 16 bit SetupToolkit
'
Declare Function DiskSpaceFree Lib "STKIT416.DLL" () As Long
Declare Function SetTime Lib "STKIT416.DLL" (ByVal strFileGetTime As String, ByVal strFileSetTime As String) As Integer
Declare Function AllocUnit Lib "STKIT416.DLL" () As Long
Declare Function GetWinPlatform Lib "STKIT416.DLL" () As Long
Declare Function DLLSelfRegister Lib "STKIT416.DLL" (ByVal lpDllName As String) As Integer
Declare Sub lmemcpy Lib "STKIT416.DLL" (strDest As Any, ByVal strSrc As Any, ByVal intBytes As Integer)

Declare Function GetPrivateProfileString Lib "Kernel" (ByVal lpApplicationName As String, ByVal lpKeyName As Any, ByVal lpDefault As String, ByVal lpReturnedString As String, ByVal nSize As Integer, ByVal lpFileName As String) As Integer
Declare Function WritePrivateProfileString Lib "Kernel" (ByVal lpApplicationName As Any, ByVal lpKeyName As Any, ByVal lpString As Any, ByVal lplFileName As String) As Integer
Declare Function GetWindowsDirectory Lib "Kernel" (ByVal lpBuffer As String, ByVal nSize As Integer) As Integer
Declare Function GetSystemDirectory Lib "Kernel" (ByVal lpBuffer As String, ByVal nSize As Integer) As Integer
Declare Function GetDriveType16 Lib "Kernel" Alias "GetDriveType" (ByVal intDriveNum As Integer) As Integer
Declare Function GetTempFileName16 Lib "Kernel" Alias "GetTempFileName" (ByVal cDriveLetter As Integer, ByVal lpPrefixString As String, ByVal wUnique As Integer, ByVal lpTempFileName As String) As Integer

Declare Function VerInstallFile Lib "VER.DLL" (ByVal Flags%, ByVal SrcName$, ByVal DestName$, ByVal SrcDir$, ByVal DestDir$, ByVal CurrDir As Any, ByVal TmpName$, lpTmpFileLen&) As Long
Declare Function GetFileVersionInfoSize Lib "VER.DLL" (ByVal strFileName As String, lVerHandle As Long) As Long
Declare Function GetFileVersionInfo Lib "VER.DLL" (ByVal strFileName As String, ByVal lVerHandle As Long, ByVal lcbSize As Long, lpvData As Byte) As Integer
Declare Function VerQueryValue Lib "VER.DLL" (lpvVerData As Byte, ByVal lpszSubBlock As String, lplpBuf As Long, lpcb As Long) As Integer

Declare Function GetModuleUsage Lib "Kernel" (ByVal hModule As Integer) As Integer

Declare Function IsWin32sLoaded Lib "ISWIN32.DLL" () As Integer
Declare Function IsCDROMLoaded Lib "ISWIN32.DLL" (ByVal strFileName As String) As Integer
Declare Function FindApp Lib "ISWIN32.DLL" (ByVal strFileName As String) As Integer

Declare Function FindWindow Lib "User" (ByVal lpClassName As String, ByVal lpWindowName As String) As Integer

#End If

#If Win32 Then
'
'API/DLL Declarations for 32 bit SetupToolkit
'
Declare Function DiskSpaceFree Lib "STKIT432.DLL" Alias "DISKSPACEFREE" () As Long
Declare Function SetTime Lib "STKIT432.DLL" (ByVal strFileGetTime As String, ByVal strFileSetTime As String) As Integer
Declare Function AllocUnit Lib "STKIT432.DLL" () As Long
Declare Function GetWinPlatform Lib "STKIT432.DLL" () As Long
Declare Function fNTWithShell Lib "STKIT432.DLL" () As Boolean
Declare Function FSyncShell Lib "STKIT432.DLL" Alias "SyncShell" (ByVal strCmdLine As String, ByVal intCmdShow As Long) As Long
Declare Function DLLSelfRegister Lib "STKIT432.DLL" (ByVal lpDllName As String) As Integer
Declare Sub lmemcpy Lib "STKIT432.DLL" (strDest As Any, ByVal strSrc As Any, ByVal lBytes As Long)
Declare Function OSfCreateShellGroup Lib "STKIT432.DLL" Alias "fCreateShellFolder" (ByVal lpstrDirName As String) As Long
Declare Function OSfCreateShellLink Lib "STKIT432.DLL" Alias "fCreateShellLink" (ByVal lpstrFolderName As String, ByVal lpstrLinkName As String, ByVal lpstrLinkPath As String, ByVal lpstrLinkArguments As String) As Long
Declare Function OSfRemoveShellLink Lib "STKIT432.DLL" Alias "fRemoveShellLink" (ByVal lpstrFolderName As String, ByVal lpstrLinkName As String) As Long
Private Declare Function OSGetLongPathName Lib "STKIT432.DLL" Alias "GetLongPathName" (ByVal lpszLongPath As String, ByVal lpszShortPath As String, ByVal cchBuffer As Long) As Long

Declare Function GetPrivateProfileString Lib "Kernel32" Alias "GetPrivateProfileStringA" (ByVal lpApplicationName As String, ByVal lpKeyName As Any, ByVal lpDefault As String, ByVal lpReturnedString As String, ByVal lSize As Long, ByVal lpFileName As String) As Long
Declare Function WritePrivateProfileString Lib "Kernel32" Alias "WritePrivateProfileStringA" (ByVal lpApplicationName As Any, ByVal lpKeyName As Any, ByVal lpString As Any, ByVal lplFileName As String) As Long
Declare Function GetWindowsDirectory Lib "Kernel32" Alias "GetWindowsDirectoryA" (ByVal lpBuffer As String, ByVal nSize As Long) As Long
Declare Function GetSystemDirectory Lib "Kernel32" Alias "GetSystemDirectoryA" (ByVal lpBuffer As String, ByVal nSize As Long) As Long
Declare Function GetDriveType32 Lib "Kernel32" Alias "GetDriveTypeA" (ByVal strWhichDrive As String) As Long
Declare Function GetTempFileName32 Lib "Kernel32" Alias "GetTempFileNameA" (ByVal strWhichDrive As String, ByVal lpPrefixString As String, ByVal wUnique As Integer, ByVal lpTempFileName As String) As Long

Declare Function VerInstallFile Lib "VERSION.DLL" Alias "VerInstallFileA" (ByVal Flags&, ByVal SrcName$, ByVal DestName$, ByVal SrcDir$, ByVal DestDir$, ByVal CurrDir As Any, ByVal TmpName$, lpTmpFileLen&) As Long
Declare Function GetFileVersionInfoSize Lib "VERSION.DLL" Alias "GetFileVersionInfoSizeA" (ByVal strFileName As String, lVerHandle As Long) As Long
Declare Function GetFileVersionInfo Lib "VERSION.DLL" Alias "GetFileVersionInfoA" (ByVal strFileName As String, ByVal lVerHandle As Long, ByVal lcbSize As Long, lpvData As Byte) As Long
Declare Function VerQueryValue Lib "VERSION.DLL" Alias "VerQueryValueA" (lpvVerData As Byte, ByVal lpszSubBlock As String, lplpBuf As Long, lpcb As Long) As Long
Private Declare Function OSGetShortPathName Lib "Kernel32" Alias "GetShortPathNameA" (ByVal lpszLongPath As String, ByVal lpszShortPath As String, ByVal cchBuffer As Long) As Long
#End If


Function IsInstallFileTheSame(strInstallFile As String, strFoundFile As String) As Integer
    Dim szTemp As Integer
    
    IsInstallFileTheSame = False
    
    If FileDateTime(strInstallFile) = FileDateTime(strFoundFile) Then
        IsInstallFileTheSame = True
    End If
End Function


Function DoesFileExist(PathFile$) As Integer
    On Error GoTo FileExistErrHandler
    Dim FileNumber As Integer
    FileNumber = FreeFile
    Open PathFile$ For Input As FileNumber%
    Close FileNumber%
    DoesFileExist = True
    Exit Function

FileExistErrHandler:
    If Err = 53 Then
        DoesFileExist = False
    End If
    Exit Function
End Function


Sub Main()
    Dim intTemp As Integer
    Dim Msg As String
    Dim Style As Integer
    Dim Title As String
    Dim response As Integer
    Dim ReadPath As String * 100
    
    intTemp = GetPrivateProfileString("Programs", gstrAppName, "", ReadPath, Len(ReadPath), gstrIniFile)
    gstrTitleExe = Left$(ReadPath, intTemp)
    
    '
    'Check to see if the app is already running, if so then bail.
    '
    If App.PrevInstance Or FindApp(gstrTitleExe) Then
        Msg = gstrAppName & " is already running."  ' Define message.
        Style = vbOKOnly & vbExclamation            ' Define buttons.
        Title = "Error"                             ' Define title.
        
        response = MsgBox(Msg, Style, Title)        ' Display message.
        End
    Else
        If IsCDROMLoaded(gstrTitleExe) Then
            intTemp = Shell(gstrTitleExe, 1)
            End
        Else
            Msg = "Please insert " & gstrAppName & " CD-ROM in drive " & Left$(gstrTitleExe, 2) & " and try again."  ' Define message.
            Style = vbOKOnly & vbExclamation    ' Define buttons.
            Title = "Error"                     ' Define title.
            
            response = MsgBox(Msg, Style, Title) ' Display message.
        End If
    End If

End Sub

Public Function StripFilePath(Path$) As String
    Dim I%, Position%, LastPosition%
    
    I% = 0
    
    Do
        LastPosition% = Position%
        Position% = InStr(I% + 1, Path$, "\")
        I% = Position% + 1
        
    Loop Until Position% = 0

    StripFilePath = Left$(UCase$(Path$), LastPosition% - 1)
    
End Function





'-----------------------------------------------------------
' FUNCTION: IsWin32
'
' Returns true if this program is running under Win32 (i.e.
'   any 32-bit operating system)
'-----------------------------------------------------------
'
Function IsWin32() As Boolean
    IsWin32 = (IsWindows95() Or IsWindowsNT())
End Function

'-----------------------------------------------------------
' FUNCTION: IsWindows95
'
' Returns true if this program is running under Windows 95
'   or successor
'-----------------------------------------------------------
'
Function IsWindows95() As Boolean
    Const dwMask95 = &H2&
    If GetWinPlatform() And dwMask95 Then
        IsWindows95 = True
    Else
        IsWindows95 = False
    End If
End Function

'-----------------------------------------------------------
' FUNCTION: IsWindowsNT
'
' Returns true if this program is running under Windows NT
'-----------------------------------------------------------
'
Function IsWindowsNT() As Boolean
    Const dwMaskNT = &H1&
    If GetWinPlatform() And dwMaskNT Then
        IsWindowsNT = True
    Else
        IsWindowsNT = False
    End If
End Function


'-----------------------------------------------------------
' FUNCTION: MsgFunc
'
' Forces mouse pointer to default and calls VB's MsgBox
' function
'
' IN: [strMsg] - message to display
'     [intFlags] - MsgBox function type flags
'     [strCaption] - caption to use for message box
'     [fLogAsError] - If present and True (MSGBOX_ERR), the 32-bit
'                       version logs this message and the user's
'                       response in the logfile as an error.
'                       Otherwise it is presented to the user
'                       only.  (It is easier to use the MsgError()
'                       function.)
' Returns: Result of MsgBox function
'-----------------------------------------------------------
'
Function MsgFunc(ByVal strMsg As String, ByVal intFlags As Integer, ByVal strCaption As String) As Integer
    Dim intOldPointer As Integer
  
    intOldPointer = Screen.MousePointer

    Screen.MousePointer = gintMOUSE_DEFAULT
    MsgFunc = MsgBox(strMsg, intFlags, strCaption)
    Screen.MousePointer = intOldPointer
End Function





