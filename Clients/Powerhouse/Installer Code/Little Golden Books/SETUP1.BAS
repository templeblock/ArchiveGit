Attribute VB_Name = "basSetup1"
Option Explicit
Option Compare Text

'
'Global Constants
'

'Return values for setup toolkit functions
Global Const gintRET_CONT% = 1
Global Const gintRET_CANCEL% = 2
Global Const gintRET_EXIT% = 3
Global Const gintRET_ABORT% = 4
Global Const gintRET_FATAL% = 5
Global Const gintRET_FINISHEDSUCCESS% = 6 'Used only as parameter to ExitSetup at end of successful install
Global Const gintRET_FINISHEDINSTALLVFW% = 7 'Used only as parameter to ExitSetup at end of successful install and go ahead and install VFW.
Global Const gintRET_FINISHEDREADMEVFW% = 8 'Used only as parameter to ExitSetup at end of successful install and tell the user how to install VFW under Win 95.
Global Const gintRET_FINISHEDSUCCESSNOMESSAGE% = 9 'Used to exit setup gracefully without a messagebox

'Error levels for GetAppRemovalCmdLine()
Global Const APPREMERR_NONE = 0 'no error
Global Const APPREMERR_FATAL = 1 'fatal error
Global Const APPREMERR_NONFATAL = 2 'non-fatal error, user chose to abort
Global Const APPREMERR_USERCANCEL = 3 'user chose to cancel (no error)

'Flag for Path Dialog specifying Source or Dest directory needed
Global Const gstrDIR_SRC$ = "S"
Global Const gstrDIR_DEST$ = "D"

'Beginning of lines in [Files] and [Bootstrap] section of SETUP.LST
Global Const gstrINI_FILE$ = "File"
Global Const gstrINI_REMOTE$ = "Remote"

'
'Type Definitions
'
Type FILEINFO                                               'Setup information file line format
    intDiskNum As Integer                                   'disk number
    fSplit As Integer                                       'split flag
    strSrcName As String                                    'name of source file
    strDestName As String                                   'name of destination file
    strDestDir As String                                    'destination directory
    strRegister As String                                   'registration info
    fShared As Boolean                                      'whether the file is shared or private
    fSystem As Boolean                                      'whether the file is a system file (i.e. should be installed but never removed)
    varDate As Variant                                      'file date
    lFileSize As Long                                       'file size
    sVerInfo As VERINFO                                     'file version number
End Type

Type DISKINFO                                               'Disk drive information
    lAvail As Long                                          'Bytes available on drive
    lReq As Long                                            'Bytes required for setup
    lMinAlloc As Long                                       'minimum allocation unit
End Type

Type DESTINFO                                               'save dest dir for certain files
    strAppDir As String
#If Win16 Then
    strBtrieve As String
#End If
#If Win32 Then
    strAUTMGR32 As String
    strRACMGR32 As String
#End If
End Type

Type REGINFO                                                'save registration info for files
    strFileName As String
    strRegister As String
    
    'The following are used only for remote server registration
    strNetworkAddress As String
    strNetworkProtocol As String
    intAuthentication As Integer
End Type

'
'Global Variables
'
Global gstrSETMSG As String
Global gfRetVal As Integer                                  'return value for form based functions
Global gstrAppName As String                                'name of app being installed
Global gstrTitle As String                                  '"setup" name of app being installed
Global gstrTitleExe As String                               '"setup" name of .EXE being installed
Global gstrDestDir As String                                'dest dir for application files
Global gstrAppExe As String                                 'name of app .EXE being installed
Global gstrSrcPath As String                                'path of source files
Global gstrSetupInfoFile As String                          'pathname of SETUP.LST file
Global gstrWinDir As String                                 'windows directory
Global gstrWinSysDir As String                              'windows\system directory
Global gsDiskSpace() As DISKINFO                            'disk space for target drives
Global gstrDrivesUsed As String                             'dest drives used by setup
Global glTotalCopied As Long                                'total bytes copied so far
Global gintCurrentDisk As Integer                           'current disk number being installed
Global gsDest As DESTINFO                                   'dest dirs for certain files
#If Win32 And LOGGING Then
Global gstrAppRemovalLog As String                          'name of the app removal logfile
Global gstrAppRemovalEXE As String                          'name of the app removal executable
Global gfAppRemovalFilesMoved As Boolean                    'whether or not the app removal files have been moved to the application directory
#End If
Global gfForceUseDefDest As Boolean                         'If set to true, then the user will not be prompted for the destination directory
Global fMainGroupWasCreated As Boolean                      'Whether or not a main folder/group has been created

Global gintVFWInstalled As Integer                          'Set to true if VFW is installed
Global gintWin32sInstalled As Integer                       'Set to true if Win32s is installed
Global gstrAppPath As String

'
'Form/Module Constants
'

'Possible ProgMan actions
Const mintDDE_ITEMADD% = 1                                  'AddProgManItem flag
Const mintDDE_GRPADD% = 2                                   'AddProgManGroup flag

'Special file names
#If Win16 Then
Const mstrFILE_BTRIEVE$ = "BTRIEVE.TRN"
Const mstrAUTPRX16$ = "AUTPRX16.DLL"
Const mstrAUTPRX$ = "AUTPRX.DLL"
Global Const mstrFILE_RPCREG$ = "RPCREG.DAT"
#End If
#If Win32 And LOGGING Then
Const mstrFILE_APPREMOVALLOGBASE$ = "ST4UNST"               'Base name of the app removal logfile
Const mstrFILE_APPREMOVALLOGEXT$ = ".LOG"                   'Default extension for the app removal logfile
Const mstrFILE_AUTMGR32 = "AUTMGR32.EXE"
Const mstrFILE_RACMGR32 = "RACMGR32.EXE"
Const mstrFILE_CTL3D32$ = "CTL3D32.DLL"
#End If

'Name of temporary file used for concatenation of split files
Const mstrCONCATFILE$ = "VB4STTMP.CCT"

'setup information file registration macros
Const mstrDLLSELFREGISTER$ = "$(DLLSELFREGISTER)"
Const mstrEXESELFREGISTER$ = "$(EXESELFREGISTER)"
Const mstrREMOTEREGISTER$ = "$(REMOTE)"

'
'Form/Module Variables
'
Private msRegInfo() As REGINFO                                  'files to be registered
Private mlTotalToCopy As Long                                   'total bytes to copy
Private mintConcatFile As Integer                               'handle of dest file for concatenation
Private mlSpaceForConcat As Long                                'extra space required for concatenation
Private mstrConcatDrive As String                               'drive to use for concatenation
Private mstrVerTmpName As String                                'temp file name for VerInstallFile API
Public mstrLastCreatedShellGroup As String                      'last folder created via call to CreateShellGroup

' Hkey cache (used for logging purposes)
Private Type HKEY_CACHE
    hkey As Long
    strHkey As String
End Type

Private hkeyCache() As HKEY_CACHE

#If Win32 Then
' Registry manipulation API's (32-bit)
Global Const HKEY_CLASSES_ROOT = &H80000000
Global Const HKEY_CURRENT_USER = &H80000001
Global Const HKEY_LOCAL_MACHINE = &H80000002
Global Const HKEY_USERS = &H80000003
Const ERROR_SUCCESS = 0&
Const ERROR_NO_MORE_ITEMS = 259&

Const REG_SZ = 1
Const REG_BINARY = 3
Const REG_DWORD = 4


Declare Function OSRegCloseKey Lib "advapi32" Alias "RegCloseKey" (ByVal hkey As Long) As Long
Declare Function OSRegCreateKey Lib "advapi32" Alias "RegCreateKeyA" (ByVal hkey As Long, ByVal lpszSubKey As String, phkResult As Long) As Long
Declare Function OSRegDeleteKey Lib "advapi32" Alias "RegDeleteKeyA" (ByVal hkey As Long, ByVal lpszSubKey As String) As Long
Declare Function OSRegEnumKey Lib "advapi32" Alias "RegEnumKeyA" (ByVal hkey As Long, ByVal iSubKey As Long, ByVal lpszName As String, ByVal cchName As Long) As Long
Declare Function OSRegOpenKey Lib "advapi32" Alias "RegOpenKeyA" (ByVal hkey As Long, ByVal lpszSubKey As String, phkResult As Long) As Long
Declare Function OSRegQueryValueEx Lib "advapi32" Alias "RegQueryValueExA" (ByVal hkey As Long, ByVal lpszValueName As String, ByVal dwReserved As Long, lpdwType As Long, lpbData As Any, cbData As Long) As Long
Declare Function OSRegSetValueEx Lib "advapi32" Alias "RegSetValueExA" (ByVal hkey As Long, ByVal lpszValueName As String, ByVal dwReserved As Long, ByVal fdwType As Long, lpbData As Any, ByVal cbData As Long) As Long
#Else
' Registry manipulation API's (16-bit)
Global Const HKEY_CLASSES_ROOT = 1
Const ERROR_SUCCESS = 0&

Const REG_SZ = 1

Declare Function OSRegCloseKey Lib "shell.dll" Alias "RegCloseKey" (ByVal hkey As Long) As Long
Declare Function OSRegCreateKey Lib "shell.dll" Alias "RegCreateKey" (ByVal hkey As Long, ByVal lpszSubKey As String, phkResult As Long) As Long
Declare Function OSRegDeleteKey Lib "shell.dll" Alias "RegDeleteKey" (ByVal hkey As Long, ByVal lpszSubKey As String) As Long
Declare Function OSRegEnumKey Lib "shell.dll" Alias "RegEnumKey" (ByVal hkey As Long, ByVal iSubKey As Long, ByVal lpstrBuffer As String, ByVal cbBuffer As Long) As Long
Declare Function OSRegOpenKey Lib "shell.dll" Alias "RegOpenKey" (ByVal hkey As Long, ByVal lpszSubKey As String, phkResult As Long) As Long
Declare Function OSRegSetValue Lib "shell.dll" Alias "RegSetValue" (ByVal hkey As Long, ByVal lpszSubKey As String, ByVal fdwType As Long, ByVal lpszValue As Any, ByVal cb As Long) As Long
Declare Function OSRegQueryValue Lib "shell.dll" Alias "RegQueryValue" (ByVal hkey As Long, ByVal lpszSubKey As Any, ByVal lpszValue As String, cb As Long) As Long
#End If

#If Win32 Then
Declare Function GetCurrentProcessId Lib "Kernel32" () As Long
#End If

'-----------------------------------------------------------
' SUB: AddPerAppPath
'
' Adds an application's full pathname and per-app path to the
'   system registry (this is currently only meaningful to
'   Windows 95).
'
' IN: [strAppExe] - app EXE name, not including path
'     [strAppDir] - full path of EXE, not including filename
'     [strAppPath] - per-app path for this application
'       (semicolon-separated list of directory path names)
'       If this is the empty string (""), no per-app path
'       is registered, but the full pathname of the
'       exe IS still registered.
'
' OUT:
'   Example registry entries:
'     HKEY_LOCAL_MACHINE\[strPathsBaseKeyName]\MyApp.Exe
'       [Default]=C:\Program Files\MyApp\MyApp.Exe
'       [Path]=C:\Program Files\MyApp;C:\Program Files\MyApp\System
'
'-----------------------------------------------------------
'
#If Win32 And LOGGING Then
Sub AddPerAppPath(ByVal strAppExe As String, ByVal strAppDir As String, ByVal strPerAppPath As String)
    If Not TreatAsWin95() Then
        Exit Sub
    End If
    
    Dim strPathsBaseKeyName As String
    Const strAppPaths$ = "App Paths"
    Const strAppPathKeyName = "Path"
    Dim fOK As Boolean
    Dim hkey As Long
    
    AddDirSep strAppDir
    
    ' Create the new key, whose name is based on the app's name
   If Not RegCreateKey(HKEY_LOCAL_MACHINE, RegPathWinCurrentVersion(), strAppPaths & gstrSEP_DIR & strAppExe, hkey) Then
        GoTo Err
    End If
    
    fOK = True
    
    ' Default value indicates full EXE pathname
    fOK = fOK And RegSetStringValue(hkey, "", strAppDir & strAppExe)
    
    ' [Path] value indicates the per-app path
    If strPerAppPath <> "" Then
        fOK = fOK And RegSetStringValue(hkey, strAppPathKeyName, strPerAppPath)
    End If
    
    If Not fOK Then
        GoTo Err
    End If
    
    RegCloseKey hkey
    
    Exit Sub
    
Err:
    MsgError ResolveResString(resERR_REG), vbExclamation Or vbOKOnly, gstrTitle
End Sub
#End If

'-----------------------------------------------------------
' FUNCTION: AddQuotesToFN
'
' Given a pathname (directory and/or filename), returns
'   that pathname surrounded by double quotes if the
'   path contains spaces or commas.  This is required for
'   setting up an icon correctly, since otherwise such paths
'   would be interpreted as a pathname plus arguments.
'-----------------------------------------------------------
'
Function AddQuotesToFN(ByVal strFileName) As String
    #If Win32 Then
        If InStr(strFileName, " ") Or InStr(strFileName, ",") Then
            AddQuotesToFN = """" & strFileName & """"
        Else
            AddQuotesToFN = strFileName
        End If
    #Else
        'Quotes around filenames are not in general supported by 16-bit
        AddQuotesToFN = strFileName
    #End If
End Function

#If Win16 Then
'-----------------------------------------------------------
' SUB: AddShareIfNeeded
'
' Adds file sharing capability which is required for VB4
'-----------------------------------------------------------
'
Sub AddShareIfNeeded()
    Const strSECTIONSTART$ = "["
    Const strCOMMENT$ = ";"
    Const strASSIGN$ = "="
    Const strFILE_VSHARE$ = "VSHARE.386"
    Const strINI_DEVICE$ = "DEVICE="
    Const strFILE_SYSINI$ = "SYSTEM.INI"
    Const strFILE_SYSTEM$ = "SYSTEM."
    Const strINI_386Enh$ = "[386ENH]"

    Dim intCount As Integer
    Dim intPos As Integer
    Dim intSysIniFile As Integer
    Dim intTmpFile As Integer
    Dim strTmp As String
    Dim strTmpFileName As String

    On Error GoTo ASINError

    '
    'If not running under Win 3.x, i.e.; if running under NT 3.5 WOW, etc.
    '
    If IsWindowsNT() Or IsWindows95() Then
        Exit Sub
    End If
    
    '
    '
    'Un-Cache System.Ini File
    '
    intCount = WritePrivateProfileString(0&, 0&, 0&, strFILE_SYSINI)

    '
    'Open system.ini file and read until we get to the [386Enh] section
    '
    intSysIniFile = FreeFile
    Open gstrWinDir & strFILE_SYSINI For Input Access Read As intSysIniFile

    intCount = 0

    Do
        Line Input #intSysIniFile, strTmp
        If EOF(intSysIniFile) Then
            GoTo ASINError
        End If

        intCount = intCount + 1
    Loop While Left$(strTmp, Len(strINI_386Enh)) <> strINI_386Enh

    '
    'Check each 'device=' line to see if it contains vshare.386 (but not as a comment)
    'If we run out of 'device=' lines before finding it, then we know we need to add it
    '
    Do While Not EOF(intSysIniFile)
        Line Input #intSysIniFile, strTmp
        If Left$(strTmp, Len(strINI_DEVICE)) = strINI_DEVICE Then
            intPos = InStr(strTmp, strASSIGN)
            If intPos = 0 Then
                GoTo ASINError
            End If
            intPos = InStr(intPos, strTmp, strCOMMENT)
            If intPos > 0 Then
                strTmp = Left$(strTmp, intPos - 1)
            End If
            If InStr(strTmp, strFILE_VSHARE) > 0 Then
                '
                'File already includes vshare.386, bail out
                '
                Close intSysIniFile
                Exit Sub
            End If
        Else
            If Left$(strTmp, 1) = strSECTIONSTART Then
                Exit Do
            End If
        End If

        intCount = intCount + 1
    Loop

    '
    'Rewind to beginning of system.ini file and open a temporary file
    '
    Seek intSysIniFile, 1
    strTmpFileName = Space$(gintMAX_SIZE)
    If GetTempFileName(0, gstrNULL, 0, strTmpFileName) = 0 Then
        GoTo ASINError
    End If

    intTmpFile = FreeFile
    Open strTmpFileName For Output As intTmpFile

    '
    'Read all lines before the location we'll be adding vshare and write them out to a temporary file
    '
    Do
        Line Input #intSysIniFile, strTmp
        Print #intTmpFile, strTmp
        intCount = intCount - 1
    Loop While intCount > 0

    '
    'Add device=vshare.386 line
    '
    Print #intTmpFile, strINI_DEVICE & strFILE_VSHARE
    
    '
    'Write out remainder of file
    '
    Do While Not EOF(intSysIniFile)
        Line Input #intSysIniFile, strTmp
        Print #intTmpFile, strTmp
    Loop

    Close intSysIniFile
    Close intTmpFile

    intCount = 0

    On Error Resume Next

    '
    'Rename existing system.ini to system.00x
    '
    Do
        Err = 0
        Name gstrWinDir & strFILE_SYSINI As gstrWinDir & strFILE_SYSTEM & Format$(intCount, "000")
        intCount = intCount + 1
        If Err > 0 And Err <> 58 Then
            GoTo ASINError
        End If
    Loop While Err = 58     'File already exists

    On Error GoTo ASINError

    '
    'Rename or copy new file to system.ini
    '
    If Left$(strTmpFileName, 1) = Left$(gstrWinDir, 1) Then
        Name strTmpFileName As gstrWinDir & strFILE_SYSINI
    Else
        FileCopy strTmpFileName, gstrWinDir & strFILE_SYSINI
        Kill strTmpFileName
    End If

    Err = 0

    Exit Sub

ASINError:
    If intSysIniFile > 0 Then
        Close intSysIniFile
        If intTmpFile > 0 Then
            Close intTmpFile
        End If
    End If

    MsgError ResolveResString(resERR_VSHARE), MB_ICONEXCLAMATION Or MB_OK, gstrTitle

    Exit Sub
End Sub
#End If

'-----------------------------------------------------------
' SUB: CalcDiskSpace
'
' Calculates disk space required for installing the files
' listed in the specified section of the setup information
' file (SETUP.LST)
'-----------------------------------------------------------
'
Sub CalcDiskSpace(ByVal strSECTION As String)
    Static fSplitFile As Integer
    Static lDestFileSpace As Long

    Dim intIdx As Integer
    Dim intDrvIdx As Integer
    Dim sFile As FILEINFO
    Dim strDrive As String
    Dim lThisFileSpace As Long

    intIdx = 1

    On Error GoTo CalcDSError

    '
    'For each file in the specified section, read info from the setup info file
    '
    Do While ReadSetupFileLine(strSECTION, intIdx, sFile) = True
        '
        'if the file isn't split or if this is the first section of a split file
        '
        If sFile.strDestDir <> gstrNULL Then
            fSplitFile = sFile.fSplit

            '
            'Get the dest drive used for this file.  If this is the first file using
            'the drive for a destination, add the drive to the drives used 'table',
            'allocate an array element for the holding the drive info, and get
            'available disk space and minimum allocation unit
            '
            strDrive = Left$(sFile.strDestDir, 1)
        
            intDrvIdx = InStr(gstrDrivesUsed, strDrive)
            If intDrvIdx = 0 Then
                gstrDrivesUsed = gstrDrivesUsed & strDrive
                intDrvIdx = Len(gstrDrivesUsed)

                ReDim Preserve gsDiskSpace(intDrvIdx)
                gsDiskSpace(intDrvIdx).lAvail = GetDiskSpaceFree(strDrive)

                gsDiskSpace(intDrvIdx).lMinAlloc = GetDrivesAllocUnit(strDrive)
            End If

            '
            'Calculate size of the dest final (file size + minimum allocation for drive)
            '
            lThisFileSpace = CalcFinalSize(sFile.lFileSize, strDrive)
            mlTotalToCopy = mlTotalToCopy + lThisFileSpace

            '
            'If the file already exists, then if we copy it at all, we'll be
            'replacing it.  So, we get the size of the existing dest file so
            'that we can subtract it from the amount needed later.
            '
            If FileExists(sFile.strDestDir & sFile.strDestName) Then
                lDestFileSpace = FileLen(sFile.strDestDir & sFile.strDestName)
            Else
                lDestFileSpace = 0
            End If
        End If

        '
        'If file not split, or if the last section of a split file
        '
        If sFile.fSplit = False Then
            '
            'If this is the last section of a split file, then if it's the *largest*
            'split file, set the extra space needed for concatenation to this size
            '
            If fSplitFile = True And lThisFileSpace > mlSpaceForConcat Then
                mlSpaceForConcat = lThisFileSpace
            End If

            '
            'Subtract size of existing dest file, if applicable and then accumulate
            'space required
            '
            lThisFileSpace = lThisFileSpace - lDestFileSpace
            If lThisFileSpace < 0 Then
                lThisFileSpace = 0
            End If

            gsDiskSpace(intDrvIdx).lReq = gsDiskSpace(intDrvIdx).lReq + lThisFileSpace
        End If

        intIdx = intIdx + 1
    Loop

    Exit Sub

CalcDSError:
    MsgError Error$ & LS$ & ResolveResString(resCALCSPACE), MB_ICONSTOP, gstrSETMSG
    ExitSetup frmMessage, gintRET_FATAL
End Sub

'-----------------------------------------------------------
' SUB: CalcFinalSize
'
' Computes the space required for a file of the size
' specified on the given dest path.  This includes the
' file size plus a padding to ensure that the final size
' is a multiple of the minimum allocation unit for the
' dest drive
'-----------------------------------------------------------
'
Function CalcFinalSize(lBaseFileSize As Long, strDestPath As String) As Long
    Dim lMinAlloc As Long
    Dim intPadSize As Long

    lMinAlloc = gsDiskSpace(InStr(gstrDrivesUsed, Left$(strDestPath, 1))).lMinAlloc
    intPadSize = lMinAlloc - (lBaseFileSize Mod lMinAlloc)
    If intPadSize = lMinAlloc Then
        intPadSize = 0
    End If

    CalcFinalSize = lBaseFileSize + intPadSize
End Function

'-----------------------------------------------------------
' SUB: CenterForm
'
' Centers the passed form just above center on the screen
'-----------------------------------------------------------
'
Sub CenterForm(frm As Form)
    SetMousePtr gintMOUSE_HOURGLASS

    frm.Top = (Screen.Height * 0.85) \ 2 - frm.Height \ 2
    frm.Left = Screen.Width \ 2 - frm.Width \ 2

    SetMousePtr gintMOUSE_DEFAULT
End Sub

'-----------------------------------------------------------
' FUNCTION: CheckDiskSpace
'
' Reads from the space required array generated by calling
' the 'CalcDiskSpace' function and determines whether there
' is sufficient free space on all of the drives used for
' installation
'
' Returns: True if there is enough space, False otherwise
'-----------------------------------------------------------
'
Function CheckDiskSpace() As Integer
    Static fDontAskOnSpaceErr As Integer

    Dim intIdx As Integer
    Dim intTmpDrvIdx As Integer
    Dim lDiskSpaceLeft As Long
    Dim lMostSpaceLeft As Long
                                             
    '
    'Default to True (enough space on all drives)
    '
    CheckDiskSpace = True

    '
    'For each drive that is the destination for one or more files, compare
    'the space available to the space required.
    '
    For intIdx = 1 To Len(gstrDrivesUsed)
        lDiskSpaceLeft = gsDiskSpace(intIdx).lAvail - gsDiskSpace(intIdx).lReq
        If lDiskSpaceLeft < 0 Then
            GoSub CheckDSAskSpace
        Else
            '
            'If no "TMP" drive was found, or if the "TMP" drive wasn't ready,
            'save the index of the drive and the amount of space on the drive
            'which will have the most free space.  If no "TMP" drive was
            'found in InitDiskInfo(), then this drive will be used as a
            'temporary drive for concatenating split files
            '
            If mstrConcatDrive = gstrNULL Then
                If lDiskSpaceLeft > lMostSpaceLeft Then
                    lMostSpaceLeft = lDiskSpaceLeft
                    intTmpDrvIdx = intIdx
                End If
            Else
                '
                '"TMP" drive was specified, so we'll use that
                '
                If Left$(mstrConcatDrive, 1) = Mid$(gstrDrivesUsed, intIdx, 1) Then
                    intTmpDrvIdx = intIdx
                End If
            End If
        End If
    Next

    '
    'If at least one drive was specified as a destination (if there was at least
    'one CalcDiskSpace call in Form_Load of SETUP1.FRM), then subtract the extra
    'space needed for concatenation from either:
    '   The "TMP" drive if available  - OR -
    '   The drive with the most space remaining
    '
    If intTmpDrvIdx > 0 Then
        gsDiskSpace(intTmpDrvIdx).lReq = gsDiskSpace(intTmpDrvIdx).lReq + mlSpaceForConcat
        If gsDiskSpace(intTmpDrvIdx).lAvail < gsDiskSpace(intTmpDrvIdx).lReq Then
            GoSub CheckDSAskSpace
        End If

        '
        'If a "TMP" drive was found, we use it regardless, otherwise we use the drive
        'with the most free space
        '
        If mstrConcatDrive = gstrNULL Then
            mstrConcatDrive = Mid$(gstrDrivesUsed, intTmpDrvIdx, 1) & gstrCOLON & gstrSEP_DIR
            AddDirSep mstrConcatDrive
        End If
    End If

    Exit Function

CheckDSAskSpace:
    '
    'if the user hasn't been prompted before in the event of not enough free space,
    'then display table of drive space and allow them to (basically) abort, retry,
    'or ignore.
    '
    If fDontAskOnSpaceErr = False Then
        frmDskSpace.Show 1
        If gfRetVal <> gintRET_CONT Then
            CheckDiskSpace = False
            Exit Function
        Else
            fDontAskOnSpaceErr = True
        End If
    End If

    Return
End Function

'-----------------------------------------------------------
' FUNCTION: CheckDrive
'
' Check to see if the specified drive is ready to be read
' from.  In the case of a drive that holds removable media,
' this would mean that formatted media was in the drive and
' that the drive door was closed.
'
' IN: [strDrive] - drive to check
'     [strCaption] - caption if the drive isn't ready
'
' Returns: True if the drive is ready, False otherwise
'-----------------------------------------------------------
'
Function CheckDrive(ByVal strDrive As String, ByVal strCaption As String) As Integer
    Dim strDir As String
    Dim strMsg As String
    Dim fIsUNC As Boolean

    On Error Resume Next

    SetMousePtr gintMOUSE_HOURGLASS

    Do
        Err = 0
        fIsUNC = False
        '
        'Attempt to read the current directory of the specified drive.  If
        'an error occurs, we assume that the drive is not ready
        '
        If IsUNCName(strDrive) Then
            fIsUNC = True
            strDir = Dir$(GetUNCShareName(strDrive))
        Else
            strDir = Dir$(Left$(strDrive, 2))
        End If

        If Err > 0 Then
            If fIsUNC Then
                strMsg = Error$ & LS$ & ResolveResString(resCANTREADUNC, "|1", strDrive) & LS$ & ResolveResString(resCHECKUNC)
            Else
                strMsg = Error$ & LS$ & ResolveResString(resDRVREAD) & strDrive & LS$ & ResolveResString(resDRVCHK)
            End If
            If MsgError(strMsg, MB_ICONEXCLAMATION Or MB_RETRYCANCEL, strCaption) = IDCANCEL Then
                CheckDrive = False
                Err = 0
            End If
        Else
            CheckDrive = True
        End If
    Loop While Err

    SetMousePtr gintMOUSE_DEFAULT
End Function

'-----------------------------------------------------------
' FUNCTION: CheckOverwritePrivateFile
'
' Checks if a private file that we are about to install
' already exists in the destination directory.  If it
' does, there will be problems if the user ever tries to
' remove either application, so warn the user and suggest
' selecting a different destination directory.
'
' IN: [strFN] - Full path of the private file that is
'               about to be installed.
'
'-----------------------------------------------------------
'
Sub CheckOverwritePrivateFile(ByVal strFN As String)
    Static fIgnoreOverwrite As Boolean
    
    If fIgnoreOverwrite Then
        'If the users once chooses to ignore this warning,
        'we will not bring it up again.
        Exit Sub
    End If
    
    If FileExists(strFN) Then
        Do
            Select Case MsgError(ResolveResString(resOVERWRITEPRIVATE) & LS$ & ResolveResString(resCANCELSETUP), vbYesNo Or vbDefaultButton1 Or vbExclamation, gstrTitle)
            Case vbYes
                'The user chose to cancel.  (This is best.)
                MsgError ResolveResString(resCHOOSENEWDEST), vbOKOnly, gstrTitle
                ExitSetup frmCopy, gintRET_FATAL
            Case Else
                'One more level of warning to let them know that we highly
                '  recommend cancelling setup at this point
                Select Case MsgError(ResolveResString(resOVERWRITEPRIVATE2) & LS$ & ResolveResString(resVERIFYCONTINUE), vbYesNo Or vbDefaultButton2 Or vbExclamation, gstrTitle)
                Case vbNo
                    'User chose "no, don't continue"
                    'Repeat the first-level warning
                Case Else
                    'They decided to continue anyway
                    fIgnoreOverwrite = True
                    Exit Do
                End Select
            End Select
        Loop
    End If
End Sub

'-----------------------------------------------------------
' FUNCTION: ConcatSplitFile
'
' Reads and appends the source file passed in onto the
' previously opened destination file specified by
' mintConcatFile.  mintConcatFile should be opened
' by calling OpenConcatFile() before calling this function.
'
' IN: [strSrcName] - Source file to append to destination
'
' Returns: True if copy was successful, IDIGNORE if user
'          elects to ignore a reported copy error
'-----------------------------------------------------------
'
Function ConcatSplitFile(ByVal strSrcName As String) As Integer
    Const lMAXCOPYBUF& = 64512
    Const lMINCOPYBUFSIZE& = 4096
    Const intOPEN% = 1
    Const intGET% = 2
    Const intPUT% = 3
    Const intMEMFAIL% = 4

    Dim intSrcFile As Integer
    Dim intStatus As Integer
    Dim lBytesLeftToWrite As Long
    Dim lBytesThisTime As Long
    Dim byteFileBuf() As Byte 'This must be byte rather than String, so no Unicode conversion takes place
    Dim strMsg As String

    On Error GoTo CSFError
    
    '
    'Ensure that the specified source file is available
    '
    If DetectFile(strSrcName) = IDIGNORE Then
        ConcatSplitFile = IDIGNORE
        Exit Function
    End If

    lBytesLeftToWrite = FileLen(strSrcName)

    '
    'For error reporting, flag that we're attempting to open the file now
    '
    intStatus = intOPEN

    '
    'Open the source file for reading now
    '
    intSrcFile = FreeFile
    Open strSrcName For Binary Access Read As intSrcFile

    '
    'Initially, we'll try to copy lMAXCOPYBUF bytes at a time.  If our attempt
    'to allocate a copy buffer (Space$(...)) fails, the error handling logic
    'will cause the buffer size to be halved and another allocation attempt to
    'be made.
    '
    lBytesThisTime = lMAXCOPYBUF
    ReDim byteFileBuf(1 To lBytesThisTime) As Byte

    While lBytesLeftToWrite <> 0
        '
        'while source file hasn't been read, if the number of bytes left is bigger than
        'the buffer size, reduce the buffer size
        '
        If lBytesThisTime > lBytesLeftToWrite Then
            lBytesThisTime = lBytesLeftToWrite
            ReDim byteFileBuf(1 To lBytesThisTime) As Byte
        End If
        
        '
        'Set operation status and Get from the source file and Put to the dest file
        '
        intStatus = intGET
        Get intSrcFile, , byteFileBuf

        intStatus = intPUT
        Put mintConcatFile, , byteFileBuf

        lBytesLeftToWrite = lBytesLeftToWrite - lBytesThisTime
    Wend

    ConcatSplitFile = True
    GoTo CSFCleanup

CSFError:
    If Err = 14 Then    'Out of String Space
        lBytesThisTime = lBytesThisTime \ 2
        If lBytesThisTime >= lMINCOPYBUFSIZE Then
            Resume
        Else
            intStatus = intMEMFAIL
        End If
    End If

    strMsg = LF$ & strSrcName

    Select Case intStatus
    Case intOPEN
        strMsg = ResolveResString(resCANTOPEN) & strMsg
    Case intGET
        strMsg = ResolveResString(resCANTREAD) & strMsg
    Case intPUT
        strMsg = ResolveResString(resCANTWRITE) & strMsg & LS$ & ResolveResString(resCHKSPACE)
    Case intMEMFAIL
        strMsg = ResolveResString(resOUTOFMEMORY) & strMsg
    End Select

    Select Case MsgError(Error$ & LS$ & strMsg, MB_ABORTRETRYIGNORE Or MB_ICONEXCLAMATION Or MB_DEFBUTTON2, gstrSETMSG)
    Case IDABORT
        ExitSetup frmCopy, gintRET_ABORT
    Case IDIGNORE
        ConcatSplitFile = IDIGNORE
    Case IDRETRY
        Resume
    End Select

CSFCleanup:
    Close intSrcFile
    Err = 0
    Exit Function
End Function

'-----------------------------------------------------------
' FUNCTION: CopyFile
'
' Uses the Windows VerInstallFile API to copy a file from
' the specified source location/name to the destination
' location/name.  Split files should be combined via the
' '...Concat...' file routines before calling this
' function.
' If the file is successfully updated and the file is a
' shared file (fShared = True), then the
' files reference count is updated (32-bits only)
'
' IN: [strSrcDir] - directory where source file is located
'     [strDestDir] - destination directory for file
'     [strSrcName] - name of source file
'     [strDestName] - name of destination file
'
' PRECONDITION: NewAction() must have already been called
'               for this file copy (of type either
'               gstrKEY_SHAREDFILE or gstrKEY_PRIVATE --
'               see CopySection for an example of how
'               this works).  See NewAction() and related
'               functions in LOGGING.BAS for comments on
'               using the logging function.
'               Either CommitAction() or AbortAction() will
'               allows be called by this procedure, and
'               should not be done by the caller.
'
' Returns: True if copy was successful, False otherwise
'
' POSTCONDITION: The current action will be either committed or
'                aborted.
'-----------------------------------------------------------
'
Function CopyFile(ByVal strSrcDir As String, ByVal strDestDir As String, ByVal strSrcName As String, ByVal strDestName As String, ByVal fShared As Boolean) As Boolean
    Const intUNKNOWN% = 0
    Const intCOPIED% = 1
    Const intNOCOPY% = 2
    Const intFILEUPTODATE% = 3

    '
    'VerInstallFile() Flags
    '
    Const VIFF_FORCEINSTALL% = &H1
    Const VIF_TEMPFILE& = &H1
    Const VIF_SRCOLD& = &H4
    Const VIF_DIFFLANG& = &H8
    Const VIF_DIFFCODEPG& = &H10
    Const VIF_DIFFTYPE& = &H20
    Const VIF_WRITEPROT& = &H40
    Const VIF_FILEINUSE& = &H80
    Const VIF_OUTOFSPACE& = &H100
    Const VIF_ACCESSVIOLATION& = &H200
    Const VIF_SHARINGVIOLATION = &H400
    Const VIF_CANNOTCREATE = &H800
    Const VIF_CANNOTDELETE = &H1000
    Const VIF_CANNOTRENAME = &H2000
    Const VIF_OUTOFMEMORY = &H8000&
    Const VIF_CANNOTREADSRC = &H10000
    Const VIF_CANNOTREADDST = &H20000
    Const VIF_BUFFTOOSMALL = &H40000

    Static fIgnoreWarn As Integer             'user warned about ignoring error?

    Dim strMsg As String
    Dim lRC As Long
    Dim lpTmpNameLen As Long
    Dim intFlags As Integer
    Dim intRESULT As Integer
    Dim fFileAlreadyExisted

    On Error Resume Next

    CopyFile = False

    '
    'Ensure that the source file is available for copying
    '
    If DetectFile(strSrcDir & strSrcName) = IDIGNORE Then
        #If Win32 And LOGGING Then
            AbortAction
        #End If
        Exit Function
    End If
    
    '
    'Make the destination directory, prompt the user to retry if there is an error
    '
    If Not MakePath(strDestDir) Then
        #If Win32 And LOGGING Then
            AbortAction ' Abort file copy
        #End If
        Exit Function
    End If

    '
    'Make sure we have the LFN (long filename) of the destination directory
    '
    #If Win32 Then
    strDestDir = GetLongPathName(strDestDir)
    #End If
    
    '
    'Setup for VerInstallFile call
    '
    lpTmpNameLen = gintMAX_SIZE
    mstrVerTmpName = String$(lpTmpNameLen, 0)
    intFlags = 0
    fFileAlreadyExisted = FileExists(strDestDir & strDestName)

    intRESULT = intUNKNOWN

    Do While intRESULT = intUNKNOWN
        'VerInstallFile under Windows 95 does not handle
        '  long filenames, so we must give it the short versions
        '  (32-bit only).
        Dim strShortSrcName As String
        Dim strShortDestName As String
        Dim strShortSrcDir As String
        Dim strShortDestDir As String
        
        strShortSrcName = strSrcName
        strShortSrcDir = strSrcDir
        strShortDestName = strDestName
        strShortDestDir = strDestDir
        #If Win32 Then
            If Not FileExists(strDestDir & strDestName) Then
                'If the destination file does not already
                '  exist, we create a dummy with the correct
                '  (long) filename so that we can get its
                '  short filename for VerInstallFile.
                Open strDestDir & strDestName For Output Access Write As #1
                Close #1
            End If
        
            On Error GoTo UnexpectedErr
            If Not IsWindowsNT() Then
                'This conversion is not necessary under Windows NT
                strShortSrcDir = GetShortPathName(strSrcDir)
                strShortSrcName = GetFileName(GetShortPathName(strSrcDir & strSrcName))
                strShortDestDir = GetShortPathName(strDestDir)
                strShortDestName = GetFileName(GetShortPathName(strDestDir & strDestName))
            End If
            On Error Resume Next
        #Else
            'We cannot support installing long filenames under 16-bit platforms
        #End If
        
        lRC = VerInstallFile(intFlags, strShortSrcName, strShortDestName, strShortSrcDir, strShortDestDir, 0&, mstrVerTmpName, lpTmpNameLen)
        If Err <> 0 Then
            '
            'If the version or file expansion DLLs couldn't be found, then abort setup
            '
            ExitSetup frmCopy, gintRET_FATAL
        End If

        If lRC = 0 Then
            '
            'File was successfully installed, increment reference count if needed
            '
            
            'One more kludge for long filenames: VerInstallFile may have renamed
            'the file to its short version if it went through with the copy.
            'Therefore we simply rename it back to what it should be.
            Name strDestDir & strShortDestName As strDestDir & strDestName
            intRESULT = intCOPIED
        ElseIf lRC And VIF_SRCOLD Then
            '
            'Source file was older, so not copied, the existing version of the file
            'will be used.  Increment reference count if needed
            '
            intRESULT = intFILEUPTODATE
        ElseIf lRC And (VIF_DIFFLANG Or VIF_DIFFCODEPG Or VIF_DIFFTYPE) Then
            '
            'We retry and force installation for these cases.  You can modify the code
            'here to prompt the user about what to do.
            '
            intFlags = VIFF_FORCEINSTALL
        ElseIf lRC And VIF_WRITEPROT Then
            strMsg = ResolveResString(resWRITEPROT)
            GoSub CFMsg
        ElseIf lRC And VIF_FILEINUSE Then
            strMsg = ResolveResString(resINUSE)
            GoSub CFMsg
        ElseIf lRC And VIF_OUTOFSPACE Then
            strMsg = ResolveResString(resOUTOFSPACE) & Left$(strDestDir, 2)
            GoSub CFMsg
        ElseIf lRC And VIF_ACCESSVIOLATION Then
            strMsg = ResolveResString(resACCESSVIOLATION)
            GoSub CFMsg
        ElseIf lRC And VIF_SHARINGVIOLATION Then
            strMsg = ResolveResString(resSHARINGVIOLATION)
            GoSub CFMsg
        ElseIf lRC And VIF_OUTOFMEMORY Then
            strMsg = ResolveResString(resOUTOFMEMORY)
            GoSub CFMsg
        Else
            '
            'For these cases, we generically report the error and do not install the file
            '
            If lRC And VIF_CANNOTCREATE Then
                strMsg = ResolveResString(resCANNOTCREATE)
            ElseIf lRC And VIF_CANNOTDELETE Then
                strMsg = ResolveResString(resCANNOTDELETE)
            ElseIf lRC And VIF_CANNOTRENAME Then
                strMsg = ResolveResString(resCANNOTRENAME)
            ElseIf lRC And VIF_CANNOTREADSRC Then
                strMsg = ResolveResString(resCANNOTREADSRC)
            ElseIf lRC And VIF_CANNOTREADDST Then
                strMsg = ResolveResString(resCANNOTREADDST)
            ElseIf lRC And VIF_BUFFTOOSMALL Then
                strMsg = ResolveResString(resBUFFTOOSMALL)
            End If

            strMsg = strMsg & ResolveResString(resNOINSTALL)
            MsgError strMsg, MB_OK Or MB_ICONEXCLAMATION, gstrTitle
            intRESULT = intNOCOPY
        End If
    Loop

    '
    'If there was a temp file left over from VerInstallFile, remove it
    '
    If lRC And VIF_TEMPFILE Then
        Kill mstrVerTmpName
    End If

    'Abort or commit the current Action, and do reference counting
    #If Win32 And LOGGING Then
        Select Case intRESULT
        Case intNOCOPY
            AbortAction
        Case intCOPIED
            DecideIncrementRefCount strDestDir & strDestName, fShared, fFileAlreadyExisted
            AddActionNote ResolveResString(resLOG_FILECOPIED)
            CommitAction
            CopyFile = True
        Case intFILEUPTODATE
            DecideIncrementRefCount strDestDir & strDestName, fShared, fFileAlreadyExisted
            AddActionNote ResolveResString(resLOG_FILEUPTODATE)
            CommitAction
            CopyFile = True
        Case Else
            AbortAction ' Defensive - this shouldn't be reached
        End Select
    #Else
        Select Case intRESULT
        Case intCOPIED, intFILEUPTODATE
            CopyFile = True
        End Select
    #End If
    '
    'if we successfully copied the file, compare the name of the dest file with
    'one of the names (possibly) requiring special action.
    '
    If intRESULT = intCOPIED Then
        Select Case strDestName
        #If Win16 Then
        Case mstrFILE_BTRIEVE
            '
            'Used for updating WIN.INI file in 'DoBtrieve' subroutine
            '
            gsDest.strBtrieve = strDestDir & mstrFILE_BTRIEVE
        #End If
        End Select
    End If

    Exit Function

UnexpectedErr:
    MsgError Error$ & LS$ & ResolveResString(resUNEXPECTED), vbOKOnly Or vbExclamation, gstrTitle
    ExitSetup frmCopy, gintRET_FATAL
    
CFMsg: '(Subroutine)
    strMsg = strDestDir & strDestName & LS$ & strMsg
    Select Case MsgError(strMsg, MB_ABORTRETRYIGNORE Or MB_ICONEXCLAMATION Or MB_DEFBUTTON2, gstrTitle)
    Case IDABORT
        ExitSetup frmCopy, gintRET_ABORT
    Case IDIGNORE
        If fIgnoreWarn = True Then
            intRESULT = intNOCOPY
        Else
            fIgnoreWarn = True
            strMsg = strMsg & LS$ & ResolveResString(resWARNIGNORE)
            If MsgError(strMsg, MB_YESNO Or MB_ICONQUESTION Or MB_DEFBUTTON2, gstrTitle) = IDYES Then
                intRESULT = intNOCOPY
            Else
                'Will retry
            End If
        End If
    End Select

    Return
End Function

'-----------------------------------------------------------
' SUB: CopySection
'
' Attempts to copy the files that need to be copied from
' the named section of the setup info file (SETUP.LST)
'
' IN: [strSection] - name of section to copy files from
'
'-----------------------------------------------------------
'
Sub CopySection(ByVal strSECTION As String)
    Dim intIdx As Integer
    Dim fSplit As Integer
    Dim fSrcVer As Integer
    Dim sFile As FILEINFO
    Dim strLastFile As String
    Dim intRC As Integer
    Dim lThisFileSize As Long
    Dim strSrcDir As String
    Dim strDestDir As String
    Dim strSrcName As String
    Dim strDestName As String
    Dim strRegister As String
    Dim sSrcVerInfo As VERINFO
    Dim sDestVerInfo As VERINFO
    Dim fFileWasUpToDate As Boolean

    On Error Resume Next

    intIdx = 1

    '
    'For each file in the specified section, read info from the setup info file
    '
    Do While ReadSetupFileLine(strSECTION, intIdx, sFile) = True
        fFileWasUpToDate = False
        
        '
        'If last result was IGNORE, and if this is an extent of a split file,
        'then no need to process this chunk of the file either
        '
        If intRC = IDIGNORE And sFile.strDestName = strDestName Then
            GoTo CSContinue
        End If

        intRC = 0

        '
        'If a new disk is called for, or if for some reason we can't find the
        'source path (user removed the install floppy, for instance) then
        'prompt for the next disk.  The PromptForNextDisk function won't
        'actually prompt the user unless it determines that the source drive
        'contains removeable media or is a network connection
        '
        If sFile.intDiskNum <> gintCurrentDisk Or DirExists(gstrSrcPath) = False Then
            PromptForNextDisk sFile.intDiskNum, sFile.strSrcName
        End If

        strSrcName = sFile.strSrcName
        strSrcDir = gstrSrcPath

        '
        'if the file isn't split, or if this is the first section of a split file
        '
        If sFile.strDestDir <> gstrNULL Then
            fSplit = sFile.fSplit

            strDestDir = sFile.strDestDir
            strDestName = sFile.strDestName
            
            'We need to go ahead and create the destination directory, or else
            'GetLongPathName() may fail
            If Not MakePath(strDestDir) Then
                intRC = IDIGNORE
            End If
            
            If intRC <> IDIGNORE Then
                #If Win32 Then
                    Err = 0
                    strDestDir = GetLongPathName(strDestDir)
                #End If

                frmCopy.lblDestFile.Caption = strDestDir & sFile.strDestName
                frmCopy.lblDestFile.Refresh

                #If Win32 And LOGGING Then
                    If sFile.fShared Then
                        NewAction gstrKEY_SHAREDFILE, """" & strDestDir & strDestName & """"
                    ElseIf sFile.fSystem Then
                        NewAction gstrKEY_SYSTEMFILE, """" & strDestDir & strDestName & """"
                    Else
                        NewAction gstrKEY_PRIVATEFILE, """" & strDestDir & strDestName & """"
                        CheckOverwritePrivateFile strDestDir & strDestName
                    End If
                #End If
            End If
            
            '
            'If the file info just read from SETUP.LST is the application .EXE
            '(i.e.; it's the value of the AppExe Key in the [Setup] section,
            'then save it's full pathname for later use
            '
            If strDestName = gstrAppExe Then
                '
                'Used for creating a program manager icon in Form_Load of SETUP1.FRM
                'and for registering the per-app path
                '
                gsDest.strAppDir = strDestDir
            End If

            'Special case for CTL3D32.DLL (32-bits only)
            '-- we never install these files unders Windows 95, only under Windows NT
            #If Win32 Then
            If strDestName = mstrFILE_CTL3D32 Then
                If Not IsWindowsNT() Then
                    'We're not running under NT - do not install this file.
                    intRC = IDIGNORE
                    #If Win32 And LOGGING Then
                        LogNote ResolveResString(resCOMMON_CTL3D32NOTCOPIED, "|1", strDestName)
                        AbortAction
                    #End If
                End If
            End If
            #End If
            
            'Special case for RPCREG.DAT (16-bits only).  The file on the disk
            'is only a dummy.  We will build this file instead from information
            'found on the end-user machine.
            #If Win16 Then
                If strDestName = mstrFILE_RPCREG Then
                    InstallRpcRegFile
                    intRC = IDIGNORE
                End If
            #End If
            
            strRegister = sFile.strRegister

            lThisFileSize = CalcFinalSize(sFile.lFileSize, sFile.strDestDir)

            '
            'The stuff below trys to save some time by pre-checking whether a file
            'should be installed before a split file is concatenated or before
            'VerInstallFile does its think which involves a full file read (for
            'a compress file) at the minimum.  Basically, if both files have
            'version numbers, they are compared.  If one file has a version number
            'and the other doesn't, the one with the version number is deemed
            '"Newer".  If neither file has a version number, we compare date.
            '
            'Always attempt to get the source file version number.  If the setup
            'info file did not contain a version number (sSrcVerInfo.nMSHi =
            'gintNOVERINFO), we attempt to read the version number from the source
            'file.  Reading the version number from a split file will always fail.
            'That's why it's a good idea to include the version number for a file
            '(especially split ones) in the setup info file (SETUP.LST)
            '
            fSrcVer = True
            sSrcVerInfo = sFile.sVerInfo
            If sSrcVerInfo.nMSHi = gintNOVERINFO Then
                fSrcVer = GetFileVerStruct(strSrcDir & strSrcName, sSrcVerInfo)
            End If

            '
            'If there is an existing destination file with version information, then
            'compare its version number to the source file version number.
            '
            If intRC <> IDIGNORE Then
                If GetFileVerStruct(strDestDir & strDestName, sDestVerInfo, sFile.strRegister = mstrREMOTEREGISTER) = True Then
                    If fSrcVer = True Then
                        If IsNewerVer(sSrcVerInfo, sDestVerInfo) = False Then
                            '
                            'Existing file is newer than the one we want to install;
                            'the existing file will be used instead
                            '
                            intRC = IDIGNORE
                            fFileWasUpToDate = True
                            #If Win32 And LOGGING Then
                                DecideIncrementRefCount strDestDir & strDestName, sFile.fShared, True
                                AddActionNote ResolveResString(resLOG_FILEUPTODATE)
                                CommitAction
                            #End If
                        End If
                    End If
                Else
                    '
                    'If the destination file has no version info, then we'll copy the
                    'source file if it *does* have a version.  If neither file has a
                    'version number, then we compare date.
                    '
                    If fSrcVer = False Then
                        If sFile.varDate <= CVDate(FileDateTime(strDestDir & strDestName)) Then
                            If Err = 0 Then
                                '
                                'Although neither the source nor the existing file contain version
                                'information, the existing file has a newer date so we'll use it.
                                '
                                intRC = IDIGNORE
                                fFileWasUpToDate = True
                                #If Win32 And LOGGING Then
                                    DecideIncrementRefCount strDestDir & strDestName, sFile.fShared, True
                                    AddActionNote ResolveResString(resLOG_FILEUPTODATE)
                                    CommitAction
                                #End If
                            Else
                                Err = 0
                            End If
                        End If
                    End If
                End If
            End If
            
            '
            'If we've decided to try the copy, and if this is the first extent of a split file
            'then open the temporary file used for concatentation
            '
            If intRC <> IDIGNORE And fSplit = True Then
                mintConcatFile = OpenConcatFile()
                If mintConcatFile = -1 Then
                    'The open failed, and the user chose to ignore the error
                    mintConcatFile = 0
                    intRC = IDIGNORE
                    #If Win32 And LOGGING Then
                        AbortAction
                    #End If
                End If
            End If
        End If

        '
        'If this is an extent of a split file, and we're going to try the copy, then
        'append this source file extent to the end of the concatentation file
        '
        If fSplit = True Then
            If intRC <> IDIGNORE Then
                intRC = ConcatSplitFile(strSrcDir & strSrcName)
                If intRC = IDIGNORE Then
                    #If Win32 And LOGGING Then
                    AbortAction
                    #End If
                End If
            End If

            If intRC = IDIGNORE And mintConcatFile > 0 Then
                Close mintConcatFile
                mintConcatFile = 0
            End If

            fSplit = sFile.fSplit
        End If

        '
        'If the file wasn't split, or if this is the last extent of a split file
        '
        If fSplit = False Then
            If mintConcatFile > 0 Then
                '
                'If this was the last extent of a split file, close the concatenated
                'file.  At this point, the concatentated file is a true representation
                'of the desired source file, so we point to it instead of the split file
                'extent on the installation media
                '
                Close mintConcatFile
                strSrcDir = mstrConcatDrive
                strSrcName = mstrCONCATFILE
            End If

            '
            'After all of this, if we're still ready to copy, then give it a whirl!
            '
            If intRC <> IDIGNORE Then
                ' CopyFile will increment the reference count for us, and will either
                ' commit or abort the current Action.
                intRC = IIf(CopyFile(strSrcDir, strDestDir, strSrcName, strDestName, sFile.fShared), 0, IDIGNORE)
            End If

            '
            'Save the paths of certain files for later use, if they were
            'successfully installed or were already on the system
            '
            If (intRC = 0 Or fFileWasUpToDate) Then
                #If Win32 Then
                Select Case strDestName
                Case mstrFILE_AUTMGR32
                    '
                    'Used for creating an icon if installed
                    '
                    gsDest.strAUTMGR32 = strDestDir & mstrFILE_AUTMGR32
                Case mstrFILE_RACMGR32
                    '
                    'Used for creating an icon if installed
                    '
                    gsDest.strRACMGR32 = strDestDir & mstrFILE_RACMGR32
                End Select
                #End If
            
                '
                'If we successfully copied the file, and if registration information was
                'specified in the setup info file, save the registration info into an
                'array so that we can register all files requiring it in one fell swoop
                'after all the files have been copied.
                '
                If strRegister <> gstrNULL Then
                    Err = 0
                    ReDim Preserve msRegInfo(UBound(msRegInfo) + 1)
    
                    If Err > 0 Then
                        ReDim msRegInfo(0)
                    End If
    
                    msRegInfo(UBound(msRegInfo)).strFileName = strDestDir & strDestName
    
                    Select Case strRegister
                    Case mstrDLLSELFREGISTER, mstrEXESELFREGISTER
                        'Nothing in particular to do
                    Case mstrREMOTEREGISTER
                        'We need to look for and parse the corresponding "RemoteX=..." line
                        If Not ReadSetupRemoteLine(strSECTION, intIdx, msRegInfo(UBound(msRegInfo))) = True Then
                            MsgError ResolveResString(resREMOTELINENOTFOUND, "|1", strDestName, "|2", gstrINI_REMOTE & Format$(intIdx)), vbExclamation Or vbOKOnly, gstrTitle
                            ExitSetup frmSetup1, gintRET_FATAL
                        End If
                    Case Else
                        '
                        'If the registration info specified the name of a file with
                        'registration info (which we assume if a registration macro
                        'was not specified), then we also assume that, if no path
                        'information is available, this reginfo file is in the same
                        'directory as the file it registers
                        '
                        If InStr(strRegister, gstrSEP_DIR) = 0 Then
                            strRegister = strDestDir & strRegister
                        End If
                    End Select
    
                    msRegInfo(UBound(msRegInfo)).strRegister = strRegister
                End If
            
            End If

            '
            'If we created a temporary concatenation file, nuke it
            '
            If mintConcatFile > 0 Then
                Kill mstrConcatDrive & mstrCONCATFILE
                mintConcatFile = 0
            End If
        End If

        strLastFile = sFile.strDestName

CSContinue:
        '
        'If the file wasn't split, or if this was the last extent of a split file, then
        'update the copy status bar.  We need to do the update regardless of whether a
        'file was actually copied or not.
        '
        If sFile.fSplit = False Then
            glTotalCopied = glTotalCopied + lThisFileSize
            UpdateStatus frmCopy.picStatus, glTotalCopied / mlTotalToCopy
        End If

        '
        'Give a chance for the 'Cancel' button command to be processed if it was pressed
        '
        DoEvents
        intIdx = intIdx + 1
    Loop

    Err = 0
End Sub

'-----------------------------------------------------------
' SUB: CreateOSProgramGroup
'
' Calls CreateProgManGroup under Windows NT or
' CreateShellGroup under Windows 95
'-----------------------------------------------------------
'
Sub CreateOSProgramGroup(frm As Form, ByVal strFolderName As String, ByVal strGroupPath As String)
#If Win32 And LOGGING Then
    If TreatAsWin95() Then
        CreateShellGroup strFolderName
    Else
#End If
        CreateProgManGroup frm, strFolderName, strGroupPath
#If Win32 And LOGGING Then
    End If
#End If
End Sub

'-----------------------------------------------------------
' SUB: CreateOSLink
'
' Calls CreateProgManItem under Windows NT or
' CreateFolderLink under Windows 95.
'
' If fLog is missing, the default is True.
'-----------------------------------------------------------
'
Sub CreateOSLink(frm As Form, ByVal strLinkPath As String, ByVal strLinkArguments As String, ByVal strLinkName As String, Optional ByVal fLog)
    If IsMissing(fLog) Then
        fLog = True
    End If
    
#If Win32 And LOGGING Then
    If TreatAsWin95() Then
        CreateShellLink strLinkPath, strLinkArguments, strLinkName, fLog
    Else
#End If
        #If Win32 Then
            strLinkPath = GetShortPathName(strLinkPath)
        #End If
        CreateProgManItem frm, strLinkPath & " " & strLinkArguments, strLinkName, fLog
#If Win32 And LOGGING Then
    End If
#End If
End Sub

'-----------------------------------------------------------
' SUB: CreateProgManGroup
'
' Creates a new group in the Windows program manager if
' the specified groupname doesn't already exist
'
' IN: [frm] - form containing a label named 'lblDDE'
'     [strGroupName] - text name of the group
'     [strGroupPath] - file system name of the group file,
'                      ex: 'c:\windows\myapp.grp'.  Under
'                      Win32, this parameter is passed, but
'                      it is ignored.
'     [fLog] - Whether or not to write to the logfile (default
'                is true if missing)
'-----------------------------------------------------------
'
Sub CreateProgManGroup(frm As Form, ByVal strGroupName As String, ByVal strGroupPath As String, Optional ByVal fLog)
    '
    'Call generic progman DDE function with flag to add a group
    '
    If IsMissing(fLog) Then
        fLog = True
    End If
    
    'Save the group name for use in logging on the next call to CallProgManItem
    mstrLastCreatedShellGroup = strGroupName
    
    'Perform the DDE to create the group
    PerformDDE frm, strGroupName, strGroupPath, gstrNULL, mintDDE_GRPADD, fLog
End Sub

'-----------------------------------------------------------
' SUB: CreateProgManItem
'
' Creates (or replaces) a program manager icon in the active
' program manager group
'
' IN: [frm] - form containing a label named 'lblDDE'
'     [strCmdLine] - command line for the item/icon,
'                    Ex: 'c:\myapp\myapp.exe'
'                    Note:  If this path contains spaces
'                      or commas, it should be enclosed
'                      with quotes so that it is properly
'                      interpreted by Windows (see AddQuotesToFN)
'     [strIconTitle] - text caption for the icon
'     [fLog] - Whether or not to write to the logfile (default
'                is true if missing)
'
' PRECONDITION: CreateProgManGroup has already been called.  The
'               new icon will be created in the group last created.
'-----------------------------------------------------------
'
Sub CreateProgManItem(frm As Form, ByVal strCmdLine As String, ByVal strIconTitle As String, Optional ByVal fLog)
    '
    'Call generic progman DDE function with flag to add an item
    '
    If IsMissing(fLog) Then
        fLog = True
    End If
    PerformDDE frm, mstrLastCreatedShellGroup, strCmdLine, strIconTitle, mintDDE_ITEMADD, fLog
End Sub

'-----------------------------------------------------------
' SUB: CreateShellGroup
'
' Creates a new program group off of Start>Programs in the
' Windows 95 shell if the specified folder doesn't already exist.
'
' IN: [strFolderName] - text name of the folder.
'                      This parameter may not contain
'                      backslashes.
'                      ex: "My Application" - this creates
'                        the folder Start>Programs>My Application
'     [fLog] - Whether or not to write to the logfile (default
'                is true if missing)
'-----------------------------------------------------------
'
#If Win32 And LOGGING Then
Sub CreateShellGroup(ByVal strFolderName As String, Optional ByVal fLog)
    If IsMissing(fLog) Then
        fLog = True
    End If

    ReplaceDoubleQuotes strFolderName
    
    'Save this folder name for use with the next call
    'to CreateShellLink()
    mstrLastCreatedShellGroup = strFolderName
    
    If strFolderName = "" Then
        Exit Sub
    End If

    If fLog Then
        NewAction gstrKEY_SHELLFOLDER, """" & strFolderName & """"
    End If

Retry:
    
    Dim fSuccess As Boolean
    fSuccess = OSfCreateShellGroup(strFolderName)
    If fSuccess Then
        If fLog Then
            CommitAction
        End If
    Else
        Select Case (MsgError(ResolveResString(resCANTCREATEPROGRAMGROUP, "|1", strFolderName), vbRetryCancel Or vbExclamation, gstrTitle))
        Case vbCancel
            ExitSetup frmSetup1, gintRET_EXIT
            GoTo Retry
        End Select
        
        GoTo Retry
    End If
End Sub
#End If

'-----------------------------------------------------------
' SUB: CreateShellLink
'
' Creates (or replaces) a link in either Start>Programs or
' any of its immediate subfolders in the Windows 95 shell.
'
' IN: [strLinkPath] - full path to the target of the link
'                     Ex: 'c:\Program Files\My Application\MyApp.exe"
'     [strLinkArguments] - command-line arguments for the link
'                     Ex: '-f -c "c:\Program Files\My Application\MyApp.dat" -q'
'     [strLinkName] - text caption for the link
'     [fLog] - Whether or not to write to the logfile (default
'                is true if missing)
'
' OUT:
'   The link will be created in the most recent folder created
'   by a call to CreateShellGroup.  If this function has
'   never been called, then the link will be created directly
'   in the Start>Programs menu, and not in any subfolder.

'-----------------------------------------------------------
'
#If Win32 And LOGGING Then
Sub CreateShellLink(ByVal strLinkPath As String, ByVal strLinkArguments As String, ByVal strLinkName As String, Optional ByVal fLog)
    If IsMissing(fLog) Then
        fLog = True
    End If
    
    If fLog Then
        NewAction gstrKEY_SHELLLINK, """" & mstrLastCreatedShellGroup & """" & ", " & """" & strLinkName & """"
    End If
    
    ReplaceDoubleQuotes strLinkName

Retry:

    Dim fSuccess As Boolean
    fSuccess = OSfCreateShellLink(mstrLastCreatedShellGroup & "", strLinkName, strLinkPath, strLinkArguments & "") 'the path should never be enclosed in double quotes
    If fSuccess Then
        If fLog Then
            CommitAction
        End If
    Else
        Select Case (MsgError(ResolveResString(resCANTCREATEPROGRAMICON, "|1", strLinkName), vbAbortRetryIgnore Or vbExclamation, gstrTitle))
        Case vbAbort
            ExitSetup frmSetup1, gintRET_ABORT
            GoTo Retry
        Case vbRetry
            GoTo Retry
        Case vbIgnore
            If fLog Then
                AbortAction
            End If
        End Select
    End If
End Sub
#End If

'-----------------------------------------------------------
' FUNCTION: DecideIncrementRefCount
'
' Increments the reference count of a file under 32-bits
' if the file is a shared file.
'
' IN: [strFullPath] - full pathname of the file to reference
'                     count.  Example:
'                     'C:\MYAPP\MYAPP.DAT'
'     [fShared] - whether the file is shared or private
'     [fFileAlreadyExisted] - whether or not the file already
'                             existed on the hard drive
'                             before our setup program
'-----------------------------------------------------------
'
#If Win32 And LOGGING Then
Sub DecideIncrementRefCount(ByVal strFullPath As String, ByVal fShared As Boolean, ByVal fFileAlreadyExisted As Boolean)
    'Reference counting takes place under both Windows 95 and Windows NT
    If fShared Then
        IncrementRefCount strFullPath, fFileAlreadyExisted
    End If
End Sub
#End If
            
'-----------------------------------------------------------
' FUNCTION: DetectFile
'
' Detects whether the specified file exists.  If it can't
' be found, the user is given the opportunity to abort,
' retry, or ignore finding the file.  This call is used,
' for example, to ensure that a floppy with the specified
' file name is in the drive before continuing.
'
' IN: [strFileName] - name of file to detect, usually
'                     should include full path, Example:
'                     'A:\MYAPP.DAT'
'
' Returns: TRUE if the file was detected, IDIGNORE if
'          the user chose ignore when the file couldn't
'          be found, or calls ExitSetup upon 'Abort'
'-----------------------------------------------------------
'
Function DetectFile(ByVal strFileName As String) As Integer
    Dim strMsg As String

    DetectFile = True
                      
    Do While FileExists(strFileName) = False
        strMsg = ResolveResString(resCANTOPEN) & LS$ & strFileName
        Select Case MsgError(strMsg, MB_ABORTRETRYIGNORE Or MB_ICONEXCLAMATION Or MB_DEFBUTTON2, gstrSETMSG)
        Case IDABORT
            ExitSetup frmCopy, gintRET_ABORT
        Case IDIGNORE
            DetectFile = IDIGNORE
            Exit Do
        End Select
    Loop
End Function

'-----------------------------------------------------------
' FUNCTION: DirExists
'
' Determines whether the specified directory name exists.
' This function is used (for example) to determine whether
' an installation floppy is in the drive by passing in
' something like 'A:\'.
'
' IN: [strDirName] - name of directory to check for
'
' Returns: True if the directory exists, False otherwise
'-----------------------------------------------------------
'
Function DirExists(ByVal strDirName As String) As Integer
    Const strWILDCARD$ = "*.*"
    Const ATTR_DIRECTORY% = 16

    Dim strDummy As String

    On Error Resume Next

    AddDirSep strDirName
    strDummy = Dir$(strDirName & strWILDCARD, ATTR_DIRECTORY)
#If Win16 Then
    DirExists = IIf(Err, False, True)
#Else
    DirExists = IIf(strDummy = gstrNULL, False, True)
#End If

    Err = 0
End Function

'-----------------------------------------------------------
' SUB: DoBtrieve
'
' Handles special processing when Btrieve driver is flagged
' for installation (Btrieve=1 in [Setup] section of
' SETUP.LST)
'-----------------------------------------------------------
'
#If Win16 Then
Sub DoBtrieve()
    Const strINI_OPTIONS$ = "OPTIONS"
    Const strFILE_WININI$ = "WIN.INI"
    Const strBTROPTS$ = "/m:64 /p:4096 /b:16 /f:20 /l:40 /n:12 /t:"

    Dim strTmp As String
    Dim intRC As Integer

    If gsDest.strBtrieve = gstrNULL Then
        gsDest.strBtrieve = gstrDestDir & mstrFILE_BTRIEVE
    End If

    strTmp = Space$(gintMAX_SIZE)
    If GetPrivateProfileString(gstrINI_BTRIEVE, strINI_OPTIONS, "1", strTmp, gintMAX_SIZE, strFILE_WININI) <= 1 Then
        intRC = WritePrivateProfileString(gstrINI_BTRIEVE, strINI_OPTIONS, strBTROPTS & gsDest.strBtrieve, strFILE_WININI)
    End If
End Sub
#End If

'-----------------------------------------------------------
' SUB: EtchedLine
'
' Draws an 'etched' line upon the specified form starting
' at the X,Y location passed in and of the specified length.
' Coordinates are in the current ScaleMode of the passed
' in form.
'
' IN: [frmEtch] - form to draw the line upon
'     [intX1] - starting horizontal of line
'     [intY1] - starting vertical of line
'     [intLength] - length of the line
'-----------------------------------------------------------
'
Sub EtchedLine(frmEtch As Form, ByVal intX1 As Integer, ByVal intY1 As Integer, ByVal intLength As Integer)
    Const lWHITE& = vb3DHighlight
    Const lGRAY& = vb3DShadow

    frmEtch.Line (intX1, intY1)-(intX1 + intLength, intY1), lGRAY
    frmEtch.Line (frmEtch.CurrentX + 5, intY1 + 20)-(intX1 - 5, intY1 + 20), lWHITE
End Sub

'-----------------------------------------------------------
' SUB: ExeSelfRegister
'
' Synchronously runs the file passed in (which should be
' an executable file that supports the /REGSERVER switch,
' for instance, a VB4 generated OLE server .EXE).
'
' IN: [strFileName] - .EXE file to register
'-----------------------------------------------------------
'
Sub ExeSelfRegister(ByVal strFileName As String)
    Const strREGSWITCH$ = " /REGSERVER"

    Dim fShell As Integer

    '
    'Synchronously shell out and run the .EXE with the self registration switch
    '
    fShell = FSyncShell(AddQuotesToFN(strFileName) & strREGSWITCH, 7)
    frmSetup1.Refresh
End Sub

'-----------------------------------------------------------
' SUB: ExitSetup
'
' Handles shutdown of the setup app.  Depending upon the
' value of the intExitCode parm, may prompt the user and
' exit the sub if the user chooses to cancel the exit
' process.
'
' IN: [frm] - active form to unload upon exit
'     [intExitCode] - code specifying exit action
'-----------------------------------------------------------
'
Sub ExitSetup(frm As Form, intExitCode As Integer)
    Dim strMsg As String
    Dim intTemp As Integer

    On Error Resume Next

    Select Case intExitCode
    Case gintRET_EXIT
        '
        'If user chose an Exit or Cancel button
        '
        If MsgWarning(ResolveResString(resASKEXIT), MB_ICONQUESTION Or MB_YESNO Or MB_DEFBUTTON2, gstrTitle) = IDNO Then
            Exit Sub
        End If
    Case gintRET_ABORT
        '
        'If user chose to abort before a pending action
        '
        strMsg = ResolveResString(resINCOMPLETE) & LS$ & ResolveResString(resQUITNOW) & LS$
        strMsg = strMsg & ResolveResString(resQUITSETUP)
        If MsgWarning(strMsg, MB_ICONQUESTION Or MB_YESNO Or MB_DEFBUTTON2, gstrSETMSG) = IDNO Then
            Exit Sub
        End If
    End Select

    #If Win32 And LOGGING Then
    'Abort any pending actions
    While fWithinAction()
        AbortAction
    Wend
    #End If
    
    Close

    '
    'Clean up any temporary files from VerInstallFile or split file concatenation
    '
    Kill mstrVerTmpName
    If mintConcatFile > 0 Then
        Close mintConcatFile
        Kill mstrConcatDrive & mstrCONCATFILE
    End If

    If frm.hwnd <> frmSetup1.hwnd Then
        Unload frm
    End If
    
    frmSetup1.SetFocus

    '
    'Give appropriate ending message depending upon exit code
    '
    Select Case intExitCode
        Case gintRET_EXIT, gintRET_ABORT
            strMsg = ResolveResString(resINTERRUPTED, "|1", gstrAppName) & LS$ & ResolveResString(resCANRUN, "|1", gstrAppName)
            MsgWarning strMsg, MB_OK Or MB_ICONSTOP, gstrTitle
        Case gintRET_FATAL
            MsgError ResolveResString(resERROR, "|1", gstrAppName), MB_OK Or MB_ICONSTOP, gstrTitle
        Case gintRET_FINISHEDSUCCESS
            'MsgFunc ResolveResString(resSUCCESS, "|1", gstrAppName), MB_OK, gstrTitle
            MsgError gstrAppName & " has been setup successfully.", MB_OK Or MB_ICONINFORMATION, gstrTitle
        Case gintRET_FINISHEDINSTALLVFW
            MsgError gstrAppName & " cannot be run for the first time until you install Video for Windows. The Video for Windows setup program will now be started.", MB_OK Or MB_ICONEXCLAMATION, gstrTitle
            intTemp = Shell(gstrSrcPath & "VFW11E\SETUPA.EXE", 1)
        Case gintRET_FINISHEDREADMEVFW
            If MsgError(gstrAppName & " has been setup successfully. But before you run " & gstrAppName & " for the first time you must install Video for Windows. Because it is included on the Windows 95 disk we have included a readme file that will guide you through the installation process.  Would you like to view this readme file now?", MB_YESNO Or MB_ICONQUESTION, gstrTitle) = IDYES Then
                intTemp = ShellExecute(frmSetup1.hwnd, vbNullString, gstrSrcPath & "VFWREAD.WRI", vbNullString, gstrSrcPath, 1)
            Else
                'Exit Gracefully
            End If
        Case gintRET_FINISHEDSUCCESSNOMESSAGE
            'Exit with no messages
        Case Else
            strMsg = ResolveResString(resINTERRUPTED, "|1", gstrAppName) & LS$ & ResolveResString(resCANRUN, "|1", gstrAppName)
            MsgWarning strMsg, MB_OK Or MB_ICONSTOP, gstrTitle
    End Select

    #If Win32 And LOGGING Then
        'Stop logging
        DisableLogging
    #End If
    
    #If Win32 And LOGGING Then
        '32-bit only: Clean up an aborted installation
        If (intExitCode = gintRET_FINISHEDSUCCESS) Or (intExitCode = gintRET_FINISHEDSUCCESSNOMESSAGE) Then
            'Setup finished successfully - Temporary files should
            'have already been cleaned up.  Nothing else to do.
        Else
            'Setup has been aborted for one reason or another
            If (gstrAppRemovalEXE <> "") Then
                Dim nErrorLevel As Integer
                Select Case intExitCode
                Case gintRET_FATAL
                    nErrorLevel = APPREMERR_FATAL
                Case gintRET_EXIT
                    nErrorLevel = APPREMERR_USERCANCEL
                Case gintRET_ABORT
                    nErrorLevel = APPREMERR_NONFATAL
                Case Else
                    nErrorLevel = APPREMERR_FATAL
                End Select
            
                MsgFunc ResolveResString(resLOG_ABOUTTOREMOVEAPP), vbInformation Or vbOKOnly, gstrTitle
                
                Err = 0
                Shell GetAppRemovalCmdLine(gstrAppRemovalEXE, gstrAppRemovalLog, nErrorLevel, True), vbNormalFocus
                If Err Then
                    MsgError Error$ & LS$ & ResolveResString(resLOG_CANTRUNAPPREMOVER), MB_ICONEXCLAMATION Or MB_OK, gstrTitle
                End If

                'Since the app removal program will attempt to delete this program and all of our runtime
                'files, we should exit as soon as possible (otherwise the app remover will not be
                'able to remove these files)
            End If
            
            'Note: We do not delete the logfile if an error occurs.
            'The application removal EXE will do that if needed.
            
        End If
    #End If
    
    Unload frmSetup1

    'End the program
    End
End Sub

'-----------------------------------------------------------
' FUNCTION: ProcessCommandLine
'
' Processes the command-line arguments
'
' OUT: Fills in the passed-in byref parameters as appropriate
'-----------------------------------------------------------
'
#If Win32 And LOGGING Then
Sub ProcessCommandLine(ByVal strCommand As String, ByRef strSrcPath As String, ByRef strAppRemovalLog As String, ByRef strAppRemovalEXE As String)
#Else
Sub ProcessCommandLine(ByVal strCommand As String, ByRef strSrcPath As String)
#End If
    Dim fErr As Boolean
    
    strSrcPath = ""
    #If Win32 Then
        strAppRemovalLog = ""
    #End If
    
    strCommand = Trim$(strCommand)
    
    ' We expect to find the source directory,
    ' name/path of the logfile, and name/path
    ' of the app removal executable, separated only by
    ' spaces
    strSrcPath = strExtractFilenameArg(strCommand, fErr)
    If fErr Then GoTo BadCommandLine
    
    #If Win32 Then
        strAppRemovalLog = strExtractFilenameArg(strCommand, fErr)
        If fErr Then GoTo BadCommandLine
        
        strAppRemovalEXE = strExtractFilenameArg(strCommand, fErr)
        If fErr Then GoTo BadCommandLine
        
        
        ' Both the app removal logfile and executable must exist
        If Not FileExists(strAppRemovalLog) Then
            GoTo BadAppRemovalLog
        End If
        
        If Not FileExists(strAppRemovalEXE) Then
            GoTo BadAppRemovalEXE
        End If
    #End If
    
    ' Last check:  There should be nothing else on the command line
    strCommand = Trim$(strCommand)
    If strCommand <> "" Then
        GoTo BadCommandLine
    End If
    
    Exit Sub
    
#If Win32 Then
BadAppRemovalLog:
    MsgError ResolveResString(resCANTFINDAPPREMOVALLOG, "|1", strAppRemovalLog), MB_ICONEXCLAMATION Or MB_OK, gstrTitle
    ExitSetup frmSetup1, gintRET_FATAL
    
BadAppRemovalEXE:
    MsgError ResolveResString(resCANTFINDAPPREMOVALEXE, "|1", strAppRemovalEXE), MB_ICONEXCLAMATION Or MB_OK, gstrTitle
    ExitSetup frmSetup1, gintRET_FATAL
#End If
    
BadCommandLine:
    MsgError ResolveResString(resBADCOMMANDLINE), MB_ICONEXCLAMATION Or MB_OK, gstrTitle
    ExitSetup frmSetup1, gintRET_FATAL
End Sub

'-----------------------------------------------------------
' FUNCTION: GetDrivesAllocUnit
'
' Gets the minimum file size allocation unit for the
' specified drive
'
' IN: [strDrive] - Drive to get allocation unit for
'
' Returns: minimum allocation unit of drive, or -1 if
'          this value couldn't be determined
'-----------------------------------------------------------
'
Function GetDrivesAllocUnit(ByVal strDrive As String) As Long
    Dim strCurDrive As String
    Dim lAllocUnit As Long

    On Error Resume Next

    '
    'Save current drive
    '
    strCurDrive = Left$(CurDir$, 2)

    '
    'append a colon to the end of the drivespec if none supplied
    '
    If InStr(strDrive, gstrCOLON) = 0 Or Len(strDrive) > 2 Then
        strDrive = Left$(strDrive, 1) & gstrCOLON
    End If

    '
    'Change to the drive to determine the allocation unit for.  The AllocUnit()
    'API returns this value for the current drive only
    '
    ChDrive strDrive

    '
    'If there was an error accessing the specified drive, flag error return.
    'It is also possible for the AllocUnit() API to return -1 on other failure
    '
    If Err <> 0 Or (strDrive <> Left$(CurDir$, 2)) Then
        lAllocUnit = -1
    Else
        lAllocUnit = AllocUnit()
        If Err <> 0 Then
            lAllocUnit = -1
        End If
    End If

    If lAllocUnit = -1 Then
        MsgError Error$ & LS$ & ResolveResString(resALLOCUNIT) & strDrive, MB_ICONEXCLAMATION, gstrTitle
    End If

    GetDrivesAllocUnit = lAllocUnit

    '
    'Restore to original drive
    '
    ChDrive strCurDrive

    Err = 0
End Function

'-----------------------------------------------------------
' FUNCTION: GetFileName
'
' Return the filename portion of a path
'
'-----------------------------------------------------------
'
Function GetFileName(ByVal strPath As String) As String
    Dim strFileName As String
    Dim iSep As Integer
    
    strFileName = strPath
    Do
        iSep = InStr(strFileName, gstrSEP_DIR)
        If iSep = 0 Then iSep = InStr(strFileName, gstrCOLON)
        If iSep = 0 Then
            GetFileName = strFileName
            Exit Function
        Else
            strFileName = Right(strFileName, Len(strFileName) - iSep)
        End If
    Loop
End Function

'-----------------------------------------------------------
' FUNCTION: GetFileSize
'
' Determine the size (in bytes) of the specified file
'
' IN: [strFileName] - name of file to get size of
'
' Returns: size of file in bytes, or -1 if an error occurs
'-----------------------------------------------------------
'
Function GetFileSize(strFileName As String) As Long
    On Error Resume Next

    GetFileSize = FileLen(strFileName)

    If Err > 0 Then
        GetFileSize = -1
        Err = 0
    End If
End Function

#If Win32 And LOGGING Then
'-----------------------------------------------------------
' FUNCTION: GetAppRemovalCmdLine
'
' Returns the correct command-line arguments (including
' path to the executable for use in calling the
' application removal executable)
'
' IN: [strAppRemovalEXE] - Full path/filename of the app removal EXE
'     [strAppRemovalLog] - Full path/filename of the app removal logfile
'     [nErrorLevel] - Error level:
'                        APPREMERR_NONE - no error
'                        APPREMERR_FATAL - fatal error
'                        APPREMERR_NONFATAL - non-fatal error, user chose to abort
'                        APPREMERR_USERCANCEL - user chose to cancel (no error)
'     [fWaitForParent] - True if the application removal utility should wait
'                        for the parent (this process) to finish before starting
'                        to remove files.  Otherwise it may not be able to remove
'                        this process' executable file, depending upon timing.
'                        Defaults to False if not specified.
'-----------------------------------------------------------
'
Function GetAppRemovalCmdLine(ByVal strAppRemovalEXE As String, ByVal strAppRemovalLog, ByVal nErrorLevel As Integer, Optional fWaitForParent)
    Dim strEXE As String
    Dim strLog As String
    Dim strErrLevel As String
    Dim strForce As String
    Dim strWait As String

    If IsMissing(fWaitForParent) Then
        fWaitForParent = False
    End If
    
    strEXE = AddQuotesToFN(strAppRemovalEXE)
    strLog = "-n " & """" & GetLongPathName(strAppRemovalLog) & """"
    strErrLevel = IIf(nErrorLevel <> APPREMERR_NONE, "-e " & Format(nErrorLevel), "")
    If nErrorLevel <> APPREMERR_NONE Then
        strForce = " -f"
    End If
    If fWaitForParent Then
        Dim curProcessId As Currency
        Dim Wrap As Currency
        Dim lProcessId As Long
        Dim cProcessId As Currency
        
        Wrap = 2 * (CCur(&H7FFFFFFF) + 1)

        'Always print as an unsigned long
        lProcessId = GetCurrentProcessId()
        cProcessId = lProcessId
        If cProcessId < 0 Then cProcessId = cProcessId + Wrap

        strWait = " -w " & str(cProcessId)
    End If
    
    GetAppRemovalCmdLine = strEXE & " " & strLog & " " & strErrLevel & strForce & strWait
End Function
#End If

#If Win32 And LOGGING Then
'-----------------------------------------------------------
' FUNCTION: IncrementRefCount
'
' Increments the reference count on a file in the registry
' so that it may properly be removed if the user chooses
' to remove this application.
'
' IN: [strFullPath] - FULL path/filename of the file
'     [fFileAlreadyExisted] - indicates whether the given
'                             file already existed on the
'                             hard drive
'-----------------------------------------------------------
'
Sub IncrementRefCount(ByVal strFullPath As String, ByVal fFileAlreadyExisted As Boolean)
    Dim strSharedDLLsKey As String
    strSharedDLLsKey = RegPathWinCurrentVersion() & "\SharedDLLs"
    
    'We must always use the LFN for the filename, so that we can uniquely
    'and accurately identify the file in the registry.
    strFullPath = GetLongPathName(strFullPath)
    
    'Get the current reference count for this file
    Dim fSuccess As Boolean
    Dim hkey As Long
    fSuccess = RegCreateKey(HKEY_LOCAL_MACHINE, strSharedDLLsKey, "", hkey)
    If fSuccess Then
        Dim lCurRefCount As Long
        If Not RegQueryRefCount(hkey, strFullPath, lCurRefCount) Then
            'No current reference count for this file
            If fFileAlreadyExisted Then
                'If there was no reference count, but the file was found
                'on the hard drive, it means one of two things:
                '  1) This file is shipped with the operating system
                '  2) This file was installed by an older setup program
                '     that does not do reference counting
                'In either case, the correct conservative thing to do
                'is assume that the file is needed by some application,
                'which means it should have a reference count of at
                'least 1.  This way, our application removal program
                'will not delete this file.
                lCurRefCount = 1
            Else
                lCurRefCount = 0
            End If
        End If
        
        'Increment the count in the registry
        fSuccess = RegSetNumericValue(hkey, strFullPath, lCurRefCount + 1, False)
        If Not fSuccess Then
            GoTo DoErr
        End If
        RegCloseKey hkey
    Else
        GoTo DoErr
    End If
    
    Exit Sub
    
DoErr:
    'An error message should have already been shown to the user
    Exit Sub
End Sub
#End If

'-----------------------------------------------------------
' FUNCTION: InitDiskInfo
'
' Called before calculating disk space to initialize
' values used/determined when calculating disk space
' required.
'-----------------------------------------------------------
'
Sub InitDiskInfo()
    Const strTmp$ = "TMP"
    Const strTemp$ = "TEMP"

    '
    'Initialize "table" of drives used and disk space array
    '
    gstrDrivesUsed = gstrNULL
    Erase gsDiskSpace

    mlTotalToCopy = 0

    '
    'Get drive/directory for temporary files
    '
    mstrConcatDrive = UCase$(Environ$(strTmp))
    If mstrConcatDrive = gstrNULL Then
        mstrConcatDrive = UCase$(Environ$(strTemp))
    End If
    AddDirSep mstrConcatDrive

    If mstrConcatDrive <> gstrNULL Then
        If CheckDrive(mstrConcatDrive, ResolveResString(resTEMPDRIVE)) = False Then
            mstrConcatDrive = gstrNULL
        Else
            '
            'If we found a temp drive and the drive is "ready", add it to the
            'table of drives used
            '
            gstrDrivesUsed = Left$(mstrConcatDrive, 1)
            ReDim Preserve gsDiskSpace(1)
            gsDiskSpace(1).lAvail = GetDiskSpaceFree(mstrConcatDrive)
            gsDiskSpace(1).lMinAlloc = GetDrivesAllocUnit(mstrConcatDrive)
        End If
    End If
End Sub

'-----------------------------------------------------------
' FUNCTION: IsDisplayNameUnique
'
' Determines whether a given display name for registering
'   the application removal executable is unique or not.  This
'   display name is the title which is presented to the
'   user in Windows 95's control panel Add/Remove Programs
'   applet.
'
' IN: [hkeyAppRemoval] - open key to the path in the registry
'                       containing application removal entries
'     [strDisplayName] - the display name to test for uniqueness
'
' Returns: True if the given display name is already in use,
'          False if otherwise
'-----------------------------------------------------------
'
#If Win32 And LOGGING Then
Function IsDisplayNameUnique(ByVal hkeyAppRemoval As Long, ByVal strDisplayName As String) As Boolean
    Dim lIdx As Long
    Dim strSubkey As String
    Dim strDisplayNameExisting As String
    Const strKEY_DISPLAYNAME$ = "DisplayName"
    
    IsDisplayNameUnique = True
    
    lIdx = 0
    Do
        Select Case RegEnumKey(hkeyAppRemoval, lIdx, strSubkey)
        Case ERROR_NO_MORE_ITEMS
            'No more keys - must be unique
            Exit Do
        Case ERROR_SUCCESS
            'We have a key to some application removal program.  Compare its
            '  display name with ours
            Dim hkeyExisting As Long
            
            If RegOpenKey(hkeyAppRemoval, strSubkey, hkeyExisting) Then
                If RegQueryStringValue(hkeyExisting, strKEY_DISPLAYNAME, strDisplayNameExisting) Then
                    If strDisplayNameExisting = strDisplayName Then
                        'There is a match to an existing display name
                        IsDisplayNameUnique = False
                        RegCloseKey hkeyExisting
                        Exit Do
                    End If
                End If
                RegCloseKey hkeyExisting
            End If
        Case Else
            'Error, we must assume it's unique.  An error will probably
            '  occur later when trying to add to the registry
            Exit Do
        End Select
        lIdx = lIdx + 1
    Loop
End Function
#End If

'-----------------------------------------------------------
' FUNCTION: IsNewerVer
'
' Compares two file version structures and determines
' whether the source file version is newer (greater) than
' the destination file version.  This is used to determine
' whether a file needs to be installed or not
'
' IN: [sSrcVer] - source file version information
'     [sDestVer] - dest file version information
'
' Returns: True if source file is newer than dest file,
'          False if otherwise
'-----------------------------------------------------------
'
Function IsNewerVer(sSrcVer As VERINFO, sDestVer As VERINFO) As Integer
    IsNewerVer = False

    If sSrcVer.nMSHi > sDestVer.nMSHi Then GoTo INVNewer
    If sSrcVer.nMSHi < sDestVer.nMSHi Then GoTo INVOlder
    
    If sSrcVer.nMSLo > sDestVer.nMSLo Then GoTo INVNewer
    If sSrcVer.nMSLo < sDestVer.nMSLo Then GoTo INVOlder
    
    If sSrcVer.nLSHi > sDestVer.nLSHi Then GoTo INVNewer
    If sSrcVer.nLSHi < sDestVer.nLSHi Then GoTo INVOlder
    
    If sSrcVer.nLSLo > sDestVer.nLSLo Then GoTo INVNewer

    GoTo INVOlder

INVNewer:
    IsNewerVer = True
INVOlder:
End Function

'-----------------------------------------------------------
' FUNCTION: IsValidDestDir
'
' Determines whether or not the destination directory
' specifed in the "DefaultDir" key of the [Setup] section
' in SETUP.LST or a destination dir entered by the user
' is not a subdirectory of the source directory.
'
' Notes: [gstrSrcPath] - points to the source directory
'        [gstrDestDir] - points to the dest directory
'
' Returns: True if dest dir is a valid location, False
'          otherwise
'-----------------------------------------------------------
'
Function IsValidDestDir() As Integer
    Dim strMsg As String
    Dim intSrc As Integer
    Dim intDest As Integer

    '
    'Both of these paths are *always* in the format 'X:\' or 'X:\DIRNAME\'.
    '

    intSrc = InStr(4, gstrSrcPath, gstrSEP_DIR)
    If intSrc = 0 Then
        intSrc = Len(gstrSrcPath)
    End If

    intDest = InStr(4, gstrDestDir, gstrSEP_DIR)
    If intDest = 0 Then
        intDest = Len(gstrDestDir)
    End If

    If Left$(gstrDestDir, intDest) = Left$(gstrSrcPath, intSrc) > 0 Then
        IsValidDestDir = False
        strMsg = ResolveResString(resDIRSPECIFIED) & LF$ & gstrDestDir & LF$ & ResolveResString(resSAMEASSRC)
        MsgFunc strMsg, MB_OK Or MB_ICONEXCLAMATION, gstrTitle
    Else
        IsValidDestDir = True
    End If
End Function

'-----------------------------------------------------------
' FUNCTION: MakePath
'
' Creates the specified directory path
'
' IN: [strDirName] - name of the dir path to make
'     [fAllowIgnore] - whether or not to allow the user to
'                      ignore any encountered errors.  If
'                      false, the function only returns
'                      if successful.  If missing, this
'                      defaults to True.
'
' Returns: True if successful, False if error and the user
'          chose to ignore.  (The function does not return
'          if the user selects ABORT/CANCEL on an error.)
'-----------------------------------------------------------
'
Public Function MakePath(ByVal strDir As String, Optional ByVal fAllowIgnore) As Boolean
    If IsMissing(fAllowIgnore) Then
        fAllowIgnore = True
    End If
    
    Do
        If MakePathAux(strDir) Then
            MakePath = True
            Exit Function
        Else
            Dim strMsg As String
            Dim iRet As Integer
            
            strMsg = ResolveResString(resMAKEDIR) & LF$ & strDir
            iRet = MsgError(strMsg, IIf(fAllowIgnore, MB_ABORTRETRYIGNORE, MB_RETRYCANCEL) Or MB_ICONEXCLAMATION Or MB_DEFBUTTON2, gstrSETMSG)
            Select Case iRet
            Case IDABORT, IDCANCEL
                ExitSetup frmCopy, gintRET_ABORT
            Case IDIGNORE
                MakePath = False
                Exit Function
            Case IDRETRY
            End Select
        End If
    Loop
End Function

#If Win32 And LOGGING Then
'----------------------------------------------------------
' SUB: MoveAppRemovalFiles
'
' Moves the app removal logfile to the application directory,
' and registers the app removal executable with the operating
' system.
'----------------------------------------------------------
Sub MoveAppRemovalFiles()
    Dim strNewAppRemovalLogName As String
    
    'Find a unique name for the app removal logfile in the
    'application directory
    
    '...First try the default extension
    strNewAppRemovalLogName = gstrDestDir & mstrFILE_APPREMOVALLOGBASE & mstrFILE_APPREMOVALLOGEXT
    If FileExists(strNewAppRemovalLogName) Then
        '...Next try incrementing integral extensions
        Dim iExt As Integer
        Do
            If iExt > 999 Then
                GoTo CopyErr
            End If
            
            strNewAppRemovalLogName = gstrDestDir & mstrFILE_APPREMOVALLOGBASE & gstrSEP_EXT & Format(iExt, "000")
            If Not FileExists(strNewAppRemovalLogName) Then
                Exit Do 'Unique name was found
            Else
                iExt = iExt + 1
            End If
        Loop
    End If
    
    
    
    On Error GoTo CopyErr
    FileCopy gstrAppRemovalLog, strNewAppRemovalLogName
    
    'Now we need to start logging in the new logfile, so that the
    'creation of the application removal icon under NT gets logged.
    EnableLogging strNewAppRemovalLogName
    
    On Error GoTo 0
    If Not RegisterAppRemovalEXE(gstrAppRemovalEXE, strNewAppRemovalLogName) Then
        If TreatAsWin95() Then
            MsgError ResolveResString(resCANTREGISTERAPPREMOVER), MB_ICONEXCLAMATION Or MB_OK, gstrTitle
        Else
            MsgError ResolveResString(resCANTCREATEAPPREMOVALICON), MB_ICONEXCLAMATION Or MB_OK, gstrTitle
        End If
        ExitSetup frmSetup1, gintRET_FATAL
    End If
    
    'Now we can delete the original logfile, since we no longer have a reference
    'to it, and start using the new logfile
    On Error Resume Next
    Kill gstrAppRemovalLog
    
    'This temporary app removal logfile should no longer be used
    gstrAppRemovalLog = strNewAppRemovalLogName
    gfAppRemovalFilesMoved = True
    
    Exit Sub
    
CleanUpOnErr:
    On Error Resume Next
    Kill strNewAppRemovalLogName
    On Error GoTo 0
    MsgError ResolveResString(resCANTCOPYLOG, "|1", gstrAppRemovalLog), vbExclamation Or vbOKOnly, gstrTitle
    ExitSetup Screen.ActiveForm, gintRET_FATAL
    
CopyErr:
    Resume CleanUpOnErr
End Sub
#End If

'-----------------------------------------------------------
' FUNCTION: OpenConcatFile
'
' Opens a file to be the destination for concatenation of
' two or more source files that (typically) have been
' split across disks.
'
' Returns: The handle of the file to use for concatentation
'          if the open was successful, or -1 if the open
'          failed and the user chose to ignore the error.
'-----------------------------------------------------------
'
Function OpenConcatFile() As Integer
    Dim intFileNum As Integer
    Dim strMsg As String

    On Error Resume Next

    Do
        Kill mstrConcatDrive & mstrCONCATFILE
        Err = 0

        intFileNum = FreeFile
        Open mstrConcatDrive & mstrCONCATFILE For Binary Access Write As intFileNum

        If Err > 0 Then
            strMsg = ResolveResString(resNOCREATE) & LS$ & mstrConcatDrive & mstrCONCATFILE
            strMsg = strMsg & LS$ & ResolveResString(resNOTPROTECT)
            Select Case MsgError(strMsg, MB_ABORTRETRYIGNORE Or MB_ICONEXCLAMATION Or MB_DEFBUTTON2, gstrSETMSG)
            Case IDABORT
                ExitSetup frmCopy, gintRET_ABORT
            Case IDIGNORE
                OpenConcatFile = -1
                Exit Function
            End Select
        End If
    Loop While Err > 0

    OpenConcatFile = intFileNum
End Function

'-----------------------------------------------------------
' SUB: ParseDate
'
' Same as CDate with a string argument, except that it
' ignores the current localization settings.  This is
' important because SETUP.LST always uses the same
' format for dates.
'
' IN: [strDate] - string representing the date in
'                 the format mm/dd/yy or mm/dd/yyyy
' OUT: The date which strDate represents
'-----------------------------------------------------------
'
Function ParseDate(ByVal strDate As String) As Date
    Const strSEP$ = "/"
    Dim iMonth As Integer
    Dim iDay As Integer
    Dim iYear As Integer
    Dim iPos As Integer
    
    iPos = InStr(strDate, strSEP)
    If iPos = 0 Then GoTo Err
    iMonth = Val(Left$(strDate, iPos - 1))
    strDate = Mid$(strDate, iPos + 1)
    
    iPos = InStr(strDate, strSEP)
    If iPos = 0 Then GoTo Err
    iDay = Val(Left$(strDate, iPos - 1))
    strDate = Mid$(strDate, iPos + 1)
    
    iYear = Val(strDate)
    If iYear < 100 Then iYear = iYear + 1900
    
    ParseDate = DateSerial(iYear, iMonth, iDay)
    
    Exit Function
    
Err:
    Error 13 'Type mismatch error, same as intrinsic CDate triggers on error
End Function

'-----------------------------------------------------------
' SUB: PerformDDE
'
' Performs a Program Manager DDE operation as specified
' by the intDDE flag and the passed in parameters.
' Possible operations are:
'
'   mintDDE_ITEMADD:  Add an icon to the active group
'   mintDDE_GRPADD:   Create a program manager group
'
' IN: [frm] - form containing a label named 'lblDDE'
'     [strGroup] - name of group to create
'     [strTitle] - title of icon or group
'     [strCmd] - command line for icon/item to add
'     [intDDE] - ProgMan DDE action to perform
'-----------------------------------------------------------
'
Sub PerformDDE(frm As Form, ByVal strGroup As String, ByVal strCmd As String, ByVal strTitle As String, ByVal intDDE As Integer, ByVal fLog As Boolean)
    Const strCOMMA$ = ","
    Const strRESTORE$ = ", 1)]"
    Const strENDCMD$ = ")]"
    Const strSHOWGRP$ = "[ShowGroup("
    Const strADDGRP$ = "[CreateGroup("
    Const strREPLITEM$ = "[ReplaceItem("
    Const strADDITEM$ = "[AddItem("

    Dim intIdx As Integer        'loop variable

    SetMousePtr gintMOUSE_HOURGLASS

    '
    'Initialize for DDE Conversation with Windows Program Manager in
    'manual mode (.LinkMode = 2) where destination control is not auto-
    'matically updated.  Set DDE timeout for 10 seconds.  The loop around
    'DoEvents() is to allow time for the DDE Execute to be processsed.
    '

    Dim intRetry As Integer
    For intRetry = 1 To 20
        On Error Resume Next
        frm.lblDDE.LinkTopic = "PROGMAN|PROGMAN"
        If Err = 0 Then
            Exit For
        End If
        DoEvents
    Next intRetry
        
    frm.lblDDE.LinkMode = 2
    For intIdx = 1 To 10
      DoEvents
    Next
    frm.lblDDE.LinkTimeout = 100

    On Error Resume Next

    If Err = 0 Then
        Select Case intDDE
        Case mintDDE_ITEMADD
            'The item will be created in the most-recently created group
            #If Win32 And LOGGING Then
                'Write the action to the logfile
                If fLog Then
                    NewAction gstrKEY_PROGMANITEM, """" & strGroup & """" & ", " & """" & strTitle & """"
                End If
            #End If
            frm.lblDDE.LinkExecute strREPLITEM & strTitle & strENDCMD
            Err = 0
            frm.lblDDE.LinkExecute strADDITEM & strCmd & strCOMMA & strTitle & String$(3, strCOMMA) & strENDCMD
        Case mintDDE_GRPADD
            #If Win16 Then
                frm.lblDDE.LinkExecute strADDGRP & strGroup & strCOMMA & strCmd & strENDCMD
            #Else
                ' Win32
                #If LOGGING Then
                    'Write the action to the logfile
                    If fLog Then
                        NewAction gstrKEY_PROGMANGROUP, """" & strGroup & """"
                    End If
                #End If
                frm.lblDDE.LinkExecute strADDGRP & strGroup & strENDCMD
            #End If
            frm.lblDDE.LinkExecute strSHOWGRP & strGroup & strRESTORE
        End Select
    End If

    
    '
    'Disconnect DDE Link
    '

    frm.lblDDE.LinkMode = 0
    frm.lblDDE.LinkTopic = ""


    SetMousePtr gintMOUSE_DEFAULT

    #If Win32 And LOGGING Then
        If fLog Then
            CommitAction
        End If
    #End If
    
    Err = 0
End Sub

'-----------------------------------------------------------
' SUB: PromptForNextDisk
'
' If the source media is removable or a network connection,
' prompts the user to insert the specified disk number
' containing the filename which is used to determine that
' the correct disk is inserted.
'
' IN: [intDiskNum] - disk number to insert
'     [strDetectFile] - file to search for to ensure that
'                       the correct disk was inserted
'
' Notes: [gstrSrcPath] - used to identify the source drive
'-----------------------------------------------------------
'
Sub PromptForNextDisk(ByVal intDiskNum As Integer, ByVal strDetectFile As String)
    Static intDrvType As Integer

    Dim intRC As Integer
    Dim strMsg As String
    Dim strDrive As String

    On Error Resume Next

    '
    'Get source drive and, if we haven't yet determined it, get the
    'source drive type
    '
    
    strDrive = Left$(gstrSrcPath, 2)
    If intDrvType = 0 Then
        If IsUNCName(strDrive) Then
            intDrvType = intDRIVE_REMOTE
            strDrive = gstrSrcPath
        Else
            intDrvType = GetDriveType(Asc(strDrive) - 65)
        End If
    End If

    If intDrvType <> intDRIVE_FIXED Then
        While FileExists(gstrSrcPath & strDetectFile) = False
            Select Case intDrvType
            Case 0, intDRIVE_REMOVABLE
                strMsg = ResolveResString(resINSERT) & LF$ & ResolveResString(resDISK) & Format$(intDiskNum)
                strMsg = strMsg & ResolveResString(resINTO) & strDrive
            Case intDRIVE_REMOTE
                strMsg = ResolveResString(resCHKCONNECT) & strDrive
            End Select

            Beep
            intRC = MsgFunc(strMsg, MB_OKCANCEL Or MB_ICONEXCLAMATION, gstrSETMSG)
            If intRC = IDCANCEL Then
                ExitSetup frmCopy, gintRET_EXIT
            End If
        Wend
    End If

    gintCurrentDisk = intDiskNum
End Sub

'-----------------------------------------------------------
' FUNCTION: ReadIniFile
'
' Reads a value from the specified section/key of the
' specified .INI file
'
' IN: [strIniFile] - name of .INI file to read
'     [strSection] - section where key is found
'     [strKey] - name of key to get the value of
'
' Returns: non-zero terminated value of .INI file key
'-----------------------------------------------------------
'
Function ReadIniFile(ByVal strIniFile As String, ByVal strSECTION As String, ByVal strKey As String) As String
    Dim strBuffer As String
    Dim intPos As Integer

    '
    'If successful read of .INI file, strip any trailing zero returned by the Windows API GetPrivateProfileString
    '
    strBuffer = Space$(gintMAX_SIZE)
    
    If GetPrivateProfileString(strSECTION, strKey, gstrNULL, strBuffer, gintMAX_SIZE, strIniFile) > 0 Then
        ReadIniFile = RTrim$(StripTerminator(strBuffer))
    Else
        ReadIniFile = gstrNULL
    End If
End Function

'-----------------------------------------------------------
' SUB: ReadSetupFileLine
'
' Reads the requested 'FileX=' key from the specified
' section of the setup information file (SETUP.LST).
'
' IN: [strSection] - name of section to read from SETUP.LST,
'                    Ex: "Files"
'     [intFileNum] - file number index to read
'
' OUT: [sFile] - FILEINFO Type variable that, after parsing,
'                holds the information for the file
'                described.
'
' Returns: True if the requested info was successfully read,
'          False otherwise
'
' Notes: Lines in the setup information file have the
'        following format:
'
'        #,[SPLIT],SrcName,DestName,DestDir,Register,
'        Date,Size,Version
'
'        [#] - disk number where this file is located
'        [SPLIT] - optional, determines whether this is
'                  an extent of a split file.  The last
'                  extent does not specify this key
'        [SrcName] - filename on the installation media
'        [DestName] - file name to use when copied
'
'        (For split files, the following info is required only
'        for the *first* extent)
'
'        [DestDir] - dirname or macro specifying destdir
'        [Register] - reginfo file name or macro specifying
'                     file registration action
'        [Date] - date of the source file
'        [Size] - size of the source file
'        [Version] - optional, version number string
'-----------------------------------------------------------
'
Function ReadSetupFileLine(ByVal strSECTION As String, ByVal intFileNum As Integer, sFile As FILEINFO) As Integer
    Static strSplitName As String
    Const CompareBinary = 0

    Dim strLine As String
    Dim strMsg As String
    Dim intOffset As Integer
    Dim intAnchor As Integer
    Dim fDone As Integer
    Dim fErr As Boolean

    ReadSetupFileLine = False

    sFile.fSystem = False
    sFile.fShared = False
    
    '
    'Read the requested line, if unable to read it (strLine = gstrNULL) then exit
    '
    strLine = ReadIniFile(gstrSetupInfoFile, strSECTION, gstrINI_FILE & Format$(intFileNum))
    If strLine = gstrNULL Then
        Exit Function
    End If

    '
    'Get the disk number
    '
    intOffset = InStr(1, strLine, gstrCOMMA, CompareBinary)
    sFile.intDiskNum = Val(Left$(strLine, intOffset - 1))
    If sFile.intDiskNum < 1 Then
        GoTo RSFLError
    End If

    '
    'Is this a split file extent (other than the last extent of a split file)
    '
    intAnchor = intOffset + 1
    
    intOffset = InStr(intAnchor, strLine, gstrCOMMA, CompareBinary)
    If intOffset > 0 Then
        sFile.fSplit = IIf(Mid$(strLine, intAnchor, intOffset - intAnchor) = gstrNULL, False, True)
    Else
        GoTo RSFLError
    End If

    '
    'source file name, ensure it's not a UNC name
    '
    intAnchor = intOffset + 1
    sFile.strSrcName = strExtractFilenameItem(strLine, intAnchor, fErr)
    If fErr Then GoTo RSFLError
    If IsUNCName(sFile.strSrcName) = True Then GoTo RSFLError
    intAnchor = intAnchor + 1 'Skip past the comma
    
    '
    'dest file name, ensure it's not a UNC name
    '
    sFile.strDestName = strExtractFilenameItem(strLine, intAnchor, fErr)
    If fErr Then GoTo RSFLError
    If IsUNCName(sFile.strDestName) = True Then GoTo RSFLError
    If Mid$(strLine, intAnchor, 1) = gstrCOMMA Then
        If IsUNCName(sFile.strDestName) = True Then
            GoTo RSFLError
        End If
        intAnchor = intAnchor + 1 'Skip past the comma
    Else
        '
        'If no list separator after the dest file name, then this should be a
        'split file extent
        '
        If strSplitName = gstrNULL Then
            GoTo RSFLError
        Else
            sFile.strDestDir = gstrNULL
            fDone = True
        End If
    End If

    '
    'Ensure that SPLIT files in SETUP.LST are ended properly by checking that all dest
    'file names after the first SPLIT line are identical, up to and including the
    'dest file name of the very next occurring *non* SPLIT line.
    '
    If sFile.fSplit = True Then
        If strSplitName = gstrNULL Then
            strSplitName = sFile.strDestName
        Else
            If strSplitName <> sFile.strDestName Then
                GoTo RSFLError
            End If
        End If
    Else
        If strSplitName <> gstrNULL And strSplitName <> sFile.strDestName Then
            GoTo RSFLError
        Else
            strSplitName = gstrNULL
        End If
    End If

    If fDone = True Then
        GoTo RSFLDone
    End If

    '
    'parse and resolve destination directory
    '
    intOffset = InStr(intAnchor, strLine, gstrCOMMA, CompareBinary)
    If intOffset > 0 Then
        Dim strInitialDestDir As String
        strInitialDestDir = Mid$(strLine, intAnchor, intOffset - intAnchor)
        If InStr(strInitialDestDir, gstrWINSYSDESTSYSFILE) Then
            sFile.fSystem = True
        End If
        sFile.strDestDir = ResolveDestDir(strInitialDestDir)
        If sFile.strDestDir <> "?" Then
            sFile.strDestDir = ResolveDir(sFile.strDestDir, False, False)
            If sFile.strDestDir = gstrNULL Or IsUNCName(sFile.strDestDir) Then
                GoTo RSFLError
            End If
        End If
    Else
        GoTo RSFLError
    End If

    '
    'file registration information
    '
    intAnchor = intOffset + 1
    intOffset = InStr(intAnchor, strLine, gstrCOMMA, CompareBinary)
    If intOffset > 0 Then
        sFile.strRegister = Mid$(strLine, intAnchor, intOffset - intAnchor)
    Else
        GoTo RSFLError
    End If

    '
    'Extract file share type
    '
    intAnchor = intOffset + 1
    intOffset = InStr(intAnchor, strLine, gstrCOMMA, CompareBinary)
    sFile.fShared = False
    If intOffset > 0 Then
        Dim strShareType As String
        strShareType = Mid$(strLine, intAnchor, intOffset - intAnchor)
        Select Case strShareType
        Case mstrPRIVATEFILE
            sFile.fShared = False
        Case mstrSHAREDFILE
            If sFile.fSystem Then
                'A file cannot be both system and shared
                GoTo RSFLError
            End If
            
            sFile.fShared = True
        Case Else
            GoTo RSFLError
        End Select
    End If
    
    '
    'Extract file date and convert to a date variant
    '
    intAnchor = intOffset + 1
    intOffset = InStr(intAnchor, strLine, gstrCOMMA, CompareBinary)
    If intOffset > 0 Then
        If IsDate(Mid$(strLine, intAnchor, intOffset - intAnchor)) = True Then
            sFile.varDate = ParseDate(Mid$(strLine, intAnchor, intOffset - intAnchor))
        Else
            GoTo RSFLError
        End If
    End If

    '
    'Get file size, this may be the last field on the line, so need special check
    '
    intAnchor = intOffset + 1
    intOffset = InStr(intAnchor, strLine, gstrCOMMA, CompareBinary)
    If intOffset > 0 Then
        sFile.lFileSize = Val(Mid$(strLine, intAnchor, intOffset - intAnchor))
    Else
        sFile.lFileSize = Val(Mid$(strLine, intAnchor))
    End If
    If sFile.lFileSize < 0 Then
        GoTo RSFLError
    End If

    '
    'If there was a comma after the file size, the rest of the line is assumed to be
    'the version number, otherwise flag that there is no version info
    '
    If intOffset > 0 Then
        PackVerInfo Mid$(strLine, intOffset + 1), sFile.sVerInfo
    Else
        sFile.sVerInfo.nMSHi = gintNOVERINFO
    End If

RSFLDone:
    ReadSetupFileLine = True
    Exit Function

RSFLError:
    strMsg = gstrSetupInfoFile & LS$ & ResolveResString(resINVLINE) & LS$
    strMsg = strMsg & ResolveResString(resSECTNAME) & strSECTION & LF$ & strLine
    MsgError strMsg, MB_ICONSTOP, gstrTitle
    ExitSetup frmSetup1, gintRET_FATAL
End Function

'-----------------------------------------------------------
' SUB: ReadSetupRemoteLine
'
' Reads the requested 'RemoteX=' key from the specified
' section of the setup information file (SETUP.LST).
'
' IN: [strSection] - name of section to read from SETUP.LST,
'                    Ex: "Files"
'     [intFileNum] - remote number index to read
'
' OUT: [rInfo] - REGINFO Type variable that, after parsing,
'                holds the information for the line
'                described.
'
' Returns: True if the requested info was successfully read,
'          False otherwise
'
' Notes: Remote server lines in the setup information file
'        have the following format:
'
'        address,protocol,authentication-level
'
'        [address] - network address of the server, if known
'        [protocol] - network protocol name, if known
'        [authentication level] - authentication level (or 0 for default)
'-----------------------------------------------------------
'
Function ReadSetupRemoteLine(ByVal strSECTION As String, ByVal intFileNum As Integer, rInfo As REGINFO) As Integer
    Dim strLine As String
    Dim strMsg As String
    Dim intAnchor As Integer
    Dim fErr As Boolean

    ReadSetupRemoteLine = False

    '
    'Read the requested line, if unable to read it (strLine = gstrNULL) then exit
    '
    strLine = ReadIniFile(gstrSetupInfoFile, strSECTION, gstrINI_REMOTE & Format$(intFileNum))
    If strLine = gstrNULL Then
        Exit Function
    End If

    '
    'Get the network address
    '
    intAnchor = 1
    fErr = False
    If Mid$(strLine, intAnchor, 1) = gstrCOMMA Then
        rInfo.strNetworkAddress = ""
    Else
        rInfo.strNetworkAddress = strExtractFilenameItem(strLine, intAnchor, fErr)
    End If
    If fErr Then GoTo RSRLError
    intAnchor = intAnchor + 1 'Skip past the comma

    '
    'Get the network protocol
    '
    If Mid$(strLine, intAnchor, 1) = gstrCOMMA Then
        rInfo.strNetworkProtocol = ""
    Else
        rInfo.strNetworkProtocol = strExtractFilenameItem(strLine, intAnchor, fErr)
    End If
    If fErr Then GoTo RSRLError
    intAnchor = intAnchor + 1 'Skip past the comma

    '
    'Get the authentication level (must be a single digit
    '  in the range 0..6)
    '
    Const intMaxAuthentication = 6
    Dim strAuthentication As String
    
    strAuthentication = Mid$(strLine, intAnchor)
    If Len(strAuthentication) <> 1 Then GoTo RSRLError
    If (Asc(strAuthentication) < Asc("0")) Or (Asc(strAuthentication) > Asc("9")) Then GoTo RSRLError
    rInfo.intAuthentication = Val(strAuthentication)
    If rInfo.intAuthentication > intMaxAuthentication Then GoTo RSRLError

    ReadSetupRemoteLine = True
    Exit Function

RSRLError:
    strMsg = gstrSetupInfoFile & LS$ & ResolveResString(resINVLINE) & LS$
    strMsg = strMsg & ResolveResString(resSECTNAME) & strSECTION & LF$ & strLine
    MsgError strMsg, MB_ICONSTOP, gstrTitle
    ExitSetup frmSetup1, gintRET_FATAL
End Function

'-----------------------------------------------------------
' FUNCTION: RegCloseKey
'
' Closes an open registry key.
'
' Returns: True on success, else False.
'-----------------------------------------------------------
'
Function RegCloseKey(ByVal hkey As Long) As Boolean
    Dim lResult As Long
    
    On Error GoTo 0
    lResult = OSRegCloseKey(hkey)
    RegCloseKey = (lResult = ERROR_SUCCESS)
End Function

'-----------------------------------------------------------
' FUNCTION: RegCreateKey
'
' Opens (creates if already exists) a key in the system registry.
'
' IN: [hkey] - The HKEY parent.
'     [lpszSubKeyPermanent] - The first part of the subkey of
'         'hkey' that will be created or opened.  The application
'         removal utility (32-bit only) will never delete any part
'         of this subkey.  May NOT be an empty string ("").
'     [lpszSubKeyRemovable] - The subkey of hkey\lpszSubKeyPermanent
'         that will be created or opened.  If the application is
'         removed (32-bit only), then this entire subtree will be
'         deleted, if it is empty at the time of application removal.
'         If this parameter is an empty string (""), then the entry
'         will not be logged.
'
' OUT: [phkResult] - The HKEY of the newly-created or -opened key.
'
' Returns: True if the key was created/opened OK, False otherwise
'   Upon success, phkResult is set to the handle of the key.
'
'-----------------------------------------------------------
Function RegCreateKey(ByVal hkey As Long, ByVal lpszSubKeyPermanent As String, ByVal lpszSubKeyRemovable As String, phkResult As Long) As Boolean
    Dim lResult As Long
    #If Win32 Then
    Dim strHkey As String
    Dim fLog As Boolean
    #End If
    Dim strSubKeyFull As String

    On Error GoTo 0

    If lpszSubKeyPermanent = "" Then
        RegCreateKey = False 'Error: lpszSubKeyPermanent must not = ""
        Exit Function
    End If
    
    If Left$(lpszSubKeyRemovable, 1) = "\" Then
        lpszSubKeyRemovable = Mid$(lpszSubKeyRemovable, 2)
    End If

    #If Win32 Then
        If lpszSubKeyRemovable = "" Then
            fLog = False
        Else
            fLog = True
        End If
    #End If
    
    If lpszSubKeyRemovable <> "" Then
        strSubKeyFull = lpszSubKeyPermanent & "\" & lpszSubKeyRemovable
    Else
        strSubKeyFull = lpszSubKeyPermanent
    End If
    #If Win32 Then
        strHkey = strGetHKEYString(hkey)
    #End If

    #If Win32 And LOGGING Then
        If fLog Then
            NewAction _
              gstrKEY_REGKEY, _
              """" & strHkey & "\" & lpszSubKeyPermanent & """" _
                & ", " & """" & lpszSubKeyRemovable & """"
        End If
    #End If

    lResult = OSRegCreateKey(hkey, strSubKeyFull, phkResult)
    If lResult = ERROR_SUCCESS Then
        RegCreateKey = True
        #If Win32 And LOGGING Then
            If fLog Then
                CommitAction
            End If
            AddHkeyToCache phkResult, strHkey & "\" & strSubKeyFull
        #End If
    Else
        RegCreateKey = False
        MsgError ResolveResString(resERR_REG), vbOKOnly Or vbExclamation, gstrTitle
        #If Win32 And LOGGING Then
            If fLog Then
                AbortAction
            End If
        #End If
    End If
End Function

'-----------------------------------------------------------
' FUNCTION: RegDeleteKey
'
' Deletes an existing key in the system registry.
'
' Returns: True on success, False otherwise
'-----------------------------------------------------------
'
Function RegDeleteKey(ByVal hkey As Long, ByVal lpszSubKey As String) As Boolean
    Dim lResult As Long
    
    On Error GoTo 0
    lResult = OSRegDeleteKey(hkey, lpszSubKey)
    RegDeleteKey = (lResult = ERROR_SUCCESS)
End Function

'-----------------------------------------------------------
' SUB: RegEdit
'
' Calls REGEDIT to add the information in the specifed file
' to the system registry.  If your .REG file requires path
' information based upon the destination directory given by
' the user, then you will need to write and call a .REG fixup
' routine before performing the registration below.
'
' WARNING: Use of this functionality under Win32 is not recommended,
' WARNING: because the application removal utility does not support
' WARNING: undoing changes that occur as a result of calling
' WARNING: REGEDIT on an arbitrary .REG file.
' WARNING: Instead, it is recommended that you use the RegCreateKey(),
' WARNING: RegOpenKey(), RegSetStringValue(), etc. functions in
' WARNING: this module instead.  These make entries to the
' WARNING: application removal logfile, thus enabling application
' WARNING: removal to undo such changes.
'
' IN: [strRegFile] - name of file containing reg. info
'-----------------------------------------------------------
'
Sub RegEdit(ByVal strRegFile As String)
    Const strREGEDIT$ = "REGEDIT /S "

    Dim fShellOK As Integer

    On Error Resume Next

    If FileExists(strRegFile) = True Then
        #If Win32 Then
            'Because regedit is a 16-bit application, it does not accept
            'double quotes around the filename.  Thus, if strRegFile
            'contains spaces, the only way to get this to work is to pass
            'regedit the short pathname version of the filename.
            strRegFile = GetShortPathName(strRegFile)
        #End If
        
        fShellOK = FSyncShell(strREGEDIT & strRegFile, 7)
        frmSetup1.Refresh
    Else
        MsgError ResolveResString(resCANTFINDREGFILE, "|1", strRegFile), vbExclamation Or vbOKOnly, gstrTitle
        ExitSetup frmSetup1, gintRET_FATAL
    End If

    Err = 0
End Sub

' FUNCTION: RegEnumKey
'
' Enumerates through the subkeys of an open registry
' key (returns the "i"th subkey of hkey, if it exists)
'
' Returns:
'   ERROR_SUCCESS on success.  strSubkeyName is set to the name of the subkey.
'   ERROR_NO_MORE_ITEMS if there are no more subkeys (32-bit only)
'   anything else - error
'
Function RegEnumKey(ByVal hkey As Long, ByVal i As Long, strKeyName As String) As Long
    Dim strResult As String
    
    strResult = String(300, " ")
    RegEnumKey = OSRegEnumKey(hkey, i, strResult, Len(strResult))
    strKeyName = StripTerminator(strResult)
End Function

'-----------------------------------------------------------
' SUB: RegisterFiles
'
' Loop through the list (array) of files to register that
' was created in the CopySection function and register
' each file therein as required
'
' Notes: msRegInfo() array created by CopySection function
'-----------------------------------------------------------
'
Sub RegisterFiles()
    Const strEXT_EXE$ = "EXE"

    Dim intIdx As Integer
    Dim intLastIdx As Integer
    Dim strFileName As String

    On Error Resume Next

    '
    'Get number of items to register, if none then we can get out of here
    '
    intLastIdx = UBound(msRegInfo)
    If Err > 0 Then
        GoTo RFCleanup
    End If

    For intIdx = 0 To intLastIdx
        strFileName = msRegInfo(intIdx).strFileName

        Select Case msRegInfo(intIdx).strRegister
        Case mstrDLLSELFREGISTER
            Dim intDllSelfRegRet As Integer
            Dim intErrRes As Integer
            Const FAIL_OLE = 2
            Const FAIL_LOAD = 3
            Const FAIL_ENTRY = 4
            Const FAIL_REG = 5
            
            #If Win32 And LOGGING Then
                NewAction gstrKEY_DLLSELFREGISTER, """" & strFileName & """"
            #End If
            
RetryDllSelfReg:
            Err = 0
            intErrRes = 0
            intDllSelfRegRet = DLLSelfRegister(strFileName)
            If Err Then
                intErrRes = resCOMMON_CANTREGUNEXPECTED
            Else
                Select Case intDllSelfRegRet
                Case 0
                    'Good - everything's okay
                Case FAIL_OLE
                    intErrRes = resCOMMON_CANTREGOLE
                Case FAIL_LOAD
                    intErrRes = resCOMMON_CANTREGLOAD
                Case FAIL_ENTRY
                    intErrRes = resCOMMON_CANTREGENTRY
                Case FAIL_REG
                    intErrRes = resCOMMON_CANTREGREG
                Case Else
                    intErrRes = resCOMMON_CANTREGUNEXPECTED
                End Select
            End If
            
            If intErrRes Then
                'There was some kind of error
                
                #If Win32 And LOGGING Then
                    'Log the more technical version of the error message -
                    'this would be too confusing to show to the end user
                    LogError ResolveResString(intErrRes, "|1", strFileName)
                #End If
                
                'Now show a general error message to the user
AskWhatToDo:
                Dim strMsg As String
                
                strMsg = ResolveResString(resCOMMON_CANTREG, "|1", strFileName)
                #If 0 Then 'See vb4:11057
                #If Win16 Then
                    If GetFileName(strFileName) = mstrAUTPRX Or GetFileName(strFileName) = mstrAUTPRX16 Then
                        strMsg = strMsg & LS$ & ResolveResString(resCOMMON_CANTREGAUTPRXRPC1) & "  " & ResolveResString(resCOMMON_CANTREGAUTPRXRPC2)
                    End If
                #End If
                #End If
                
                Select Case MsgError(strMsg, vbExclamation Or vbAbortRetryIgnore, gstrTitle)
                Case vbAbort:
                    ExitSetup frmSetup1, gintRET_ABORT
                    GoTo AskWhatToDo
                Case vbRetry:
                    GoTo RetryDllSelfReg
                Case vbIgnore:
                    #If Win32 And LOGGING Then
                        AbortAction
                    #End If
                End Select
            Else
                #If Win32 And LOGGING Then
                    CommitAction
                #End If
            End If
        Case mstrEXESELFREGISTER
            '
            'Only self register EXE files
            '
            If Extension(strFileName) = strEXT_EXE Then
                #If Win32 And LOGGING Then
                    NewAction gstrKEY_EXESELFREGISTER, """" & strFileName & """"
                #End If
                Err = 0
                ExeSelfRegister strFileName
                #If Win32 And LOGGING Then
                    If Err Then
                        AbortAction
                    Else
                        CommitAction
                    End If
                #End If
            End If
        Case mstrREMOTEREGISTER
            #If Win32 And LOGGING Then
                NewAction gstrKEY_REMOTEREGISTER, """" & strFileName & """"
            #End If
            Err = 0
            RemoteRegister strFileName, msRegInfo(intIdx)
            #If Win32 And LOGGING Then
                If Err Then
                    AbortAction
                Else
                    CommitAction
                End If
            #End If
        Case Else
            RegEdit msRegInfo(intIdx).strRegister
        End Select
    Next

    Erase msRegInfo

RFCleanup:
    Err = 0
End Sub

#If Win32 And LOGGING Then
'----------------------------------------------------------
' SUB: RegisterAppRemovalEXE
'
' Registers the application removal program (Windows 95 only)
' or else places an icon for it in the application directory.
'
' Returns True on success, False otherwise.
'----------------------------------------------------------
Function RegisterAppRemovalEXE(ByVal strAppRemovalEXE As String, ByVal strAppRemovalLog As String) As Boolean
    On Error GoTo Err
    
    Const strREGSTR_VAL_AppRemoval_DISPLAYNAME = "DisplayName"
    Const strREGSTR_VAL_AppRemoval_COMMANDLINE = "UninstallString"
    
    Dim strREGSTR_PATH_UNINSTALL As String
    strREGSTR_PATH_UNINSTALL = RegPathWinCurrentVersion() & "\Uninstall"
    
    'The command-line for the application removal executable is simply the path
    'for the installation logfile
    Dim strAppRemovalCmdLine As String
    strAppRemovalCmdLine = GetAppRemovalCmdLine(strAppRemovalEXE, strAppRemovalLog, APPREMERR_NONE)
        
    Dim iAppend As Integer
        
    If TreatAsWin95() Then
        'Create registry entries to tell Windows where the app removal executable is,
        '  how it should be displayed to the user, and what the command-line arguments are
        
        Dim fOK As Boolean
        Dim hkeyAppRemoval As Long
        Dim hkeyOurs As Long
        Dim i As Integer
        
        'Go ahead and create a key to the main Uninstall branch
        If Not RegCreateKey(HKEY_LOCAL_MACHINE, strREGSTR_PATH_UNINSTALL, "", hkeyAppRemoval) Then
            GoTo Err
        End If
        
        'We need a unique key.  This key is never shown to the end user.  We will use a key of
        'the form 'ST4UNST #xxx'
        Dim strAppRemovalKey As String
        Dim strAppRemovalKeyBase As String
        Dim hkeyTest As Long
        strAppRemovalKeyBase = mstrFILE_APPREMOVALLOGBASE$ & " #"
        iAppend = 1
        
        Do
            strAppRemovalKey = strAppRemovalKeyBase & Format(iAppend)
            If RegOpenKey(hkeyAppRemoval, strAppRemovalKey, hkeyTest) Then
                'This key already exists.  But we need a unique key.
                RegCloseKey hkeyTest
            Else
                'We've found a key that doesn't already exist.  Use it.
                Exit Do
            End If
            
            iAppend = iAppend + 1
        Loop
        
    
        'We also need a unique displayname.  This name is
        'the only means the user has to identify the application
        'to remove
        Dim strDisplayName As String
        strDisplayName = gstrAppName 'First try... Application name
        If Not IsDisplayNameUnique(hkeyAppRemoval, strDisplayName) Then
            'Second try... Add path
            strDisplayName = strDisplayName & " (" & gstrDestDir & ")"
            If Not IsDisplayNameUnique(hkeyAppRemoval, strDisplayName) Then
                'Subsequent tries... Append a unique integer
                Dim strDisplayNameBase As String
                
                strDisplayNameBase = strDisplayName
                iAppend = 3
                Do
                    strDisplayName = strDisplayNameBase & " #" & Format(iAppend)
                    If IsDisplayNameUnique(hkeyAppRemoval, strDisplayName) Then
                        Exit Do
                    Else
                        iAppend = iAppend + 1
                    End If
                Loop
            End If
        End If
        
        'Go ahead and fill in entries for the app removal executable
        If Not RegCreateKey(hkeyAppRemoval, strAppRemovalKey, "", hkeyOurs) Then
            GoTo Err
        End If
        If Not RegSetStringValue(hkeyOurs, strREGSTR_VAL_AppRemoval_DISPLAYNAME, strDisplayName, False) Then
            GoTo Err
        End If
        If Not RegSetStringValue(hkeyOurs, strREGSTR_VAL_AppRemoval_COMMANDLINE, strAppRemovalCmdLine, False) Then
            GoTo Err
        End If
    Else
        ' Under NT, we simply place an icon to the app removal EXE in the program manager
        If fMainGroupWasCreated Then
            CreateProgManItem frmSetup1, strAppRemovalCmdLine, ResolveResString(resAPPREMOVALICONNAME, "|1", gstrAppName)
        Else
            'If you get this message, it means that you incorrectly customized Form_Load().
            'Under 32-bits and NT 3.51, a Program Manager group must always be created.
            MsgError ResolveResString(resNOFOLDERFORICON, "|1", strAppRemovalEXE), MB_OK Or MB_ICONEXCLAMATION, gstrTitle
            ExitSetup frmSetup1, gintRET_FATAL
        End If
    End If
    
    RegCloseKey hkeyAppRemoval
    RegCloseKey hkeyOurs
    
    RegisterAppRemovalEXE = True
    Exit Function
    
Err:
    If hkeyOurs Then
        RegCloseKey hkeyOurs
        RegDeleteKey hkeyAppRemoval, strAppRemovalKey
    End If
    If hkeyAppRemoval Then
        RegCloseKey hkeyAppRemoval
    End If
    
    RegisterAppRemovalEXE = False
    Exit Function
End Function
#End If

'-----------------------------------------------------------
' FUNCTION: RegOpenKey
'
' Opens an existing key in the system registry.
'
' Returns: True if the key was opened OK, False otherwise
'   Upon success, phkResult is set to the handle of the key.
'-----------------------------------------------------------
'
Function RegOpenKey(ByVal hkey As Long, ByVal lpszSubKey As String, phkResult As Long) As Boolean
    Dim lResult As Long
    #If Win32 Then
    Dim strHkey As String
    #End If

    On Error GoTo 0

    #If Win32 Then
        strHkey = strGetHKEYString(hkey)
    #End If

    lResult = OSRegOpenKey(hkey, lpszSubKey, phkResult)
    If lResult = ERROR_SUCCESS Then
        RegOpenKey = True
        #If Win32 And LOGGING Then
        AddHkeyToCache phkResult, strHkey & "\" & lpszSubKey
        #End If
    Else
        RegOpenKey = False
    End If
End Function
 
#If Win32 And LOGGING Then
'----------------------------------------------------------
' FUNCTION: RegPathWinCurrentVersion
'
' Returns the name of the registry key
' "\HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion"
'----------------------------------------------------------
Function RegPathWinCurrentVersion() As String
    RegPathWinCurrentVersion = "SOFTWARE\Microsoft\Windows\CurrentVersion"
End Function
#End If

'----------------------------------------------------------
' FUNCTION: RegQueryIntValue
'
' Retrieves the integer data for a named
' (strValueName = name) or unnamed (strValueName = "")
' value within a registry key.  If the named value
' exists, but its data is not a REG_DWORD, this function
' fails.
'
' NOTE: There is no 16-bit version of this function.
'
' Returns: True on success, else False.
'   On success, lData is set to the numeric data value
'
'----------------------------------------------------------
#If Win32 Then
Function RegQueryNumericValue(ByVal hkey As Long, ByVal strValueName As String, lData As Long) As Boolean
    Dim lResult As Long
    Dim lValueType As Long
    Dim lBuf As Long
    Dim lDataBufSize As Long
    
    RegQueryNumericValue = False
    
    On Error GoTo 0
    
    ' Get length/data type
    lDataBufSize = 4
        
    lResult = OSRegQueryValueEx(hkey, strValueName, 0&, lValueType, lBuf, lDataBufSize)
    If lResult = ERROR_SUCCESS Then
        If lValueType = REG_DWORD Then
            lData = lBuf
            RegQueryNumericValue = True
        End If
    End If
End Function
#End If

' FUNCTION: RegQueryStringValue
'
' Retrieves the string data for a named
' (strValueName = name) or unnamed (strValueName = "")
' value within a registry key.  If the named value
' exists, but its data is not a string, this function
' fails.
'
' NOTE: For 16-bits, strValueName MUST be "" (but the
' NOTE: parameter is left in for source code compatability)
'
' Returns: True on success, else False.
'   On success, strData is set to the string data value
'
Function RegQueryStringValue(ByVal hkey As Long, ByVal strValueName As String, strData As String) As Boolean
#If Win32 Then
    Dim lResult As Long
    Dim lValueType As Long
    Dim strBuf As String
    Dim lDataBufSize As Long
    
    RegQueryStringValue = False
    On Error GoTo 0
    ' Get length/data type
    lResult = OSRegQueryValueEx(hkey, strValueName, 0&, lValueType, ByVal 0&, lDataBufSize)
    If lResult = ERROR_SUCCESS Then
        If lValueType = REG_SZ Then
            strBuf = String(lDataBufSize, " ")
            lResult = OSRegQueryValueEx(hkey, strValueName, 0&, 0&, ByVal strBuf, lDataBufSize)
            If lResult = ERROR_SUCCESS Then
                RegQueryStringValue = True
                strData = StripTerminator(strBuf)
            End If
        End If
    End If
#Else
'16-bit
    Dim lResult As Long
    Dim lValueType As Long
    Dim strBuf As String
    Dim lDataBufSize As Long

    RegQueryStringValue = False
    
    If strValueName <> "" Then
        'Under 16-bits, strValueName MUST be ""
        Exit Function
    End If
    
    On Error GoTo 0
    lDataBufSize = 500
    strBuf = String(lDataBufSize, " ")
    lResult = OSRegQueryValue(hkey, "", strBuf, lDataBufSize)
    If lResult = ERROR_SUCCESS Then
        RegQueryStringValue = True
        strData = StripTerminator(strBuf)
    End If
#End If
End Function

'----------------------------------------------------------
' FUNCTION: RegQueryRefCount
'
' Retrieves the data inteded as a reference count for a
' particular value within a registry key.  Although
' REG_DWORD is the preferred way of storing reference
' counts, it is possible that some installation programs
' may incorrect use a string or binary value instead.
' This routine accepts the data whether it is a string,
' a binary value or a DWORD (Long).
'
' NOTE: There is no 16-bit version of this function.
'
' Returns: True on success, else False.
'   On success, lrefcount is set to the numeric data value
'
'----------------------------------------------------------
#If Win32 Then
Function RegQueryRefCount(ByVal hkey As Long, ByVal strValueName As String, lRefCount As Long) As Boolean
    Dim lResult As Long
    Dim lValueType As Long
    Dim lBuf As Long
    Dim lDataBufSize As Long

    RegQueryRefCount = False

    On Error GoTo 0

    ' Get length/data type
    lDataBufSize = 4

    lResult = OSRegQueryValueEx(hkey, strValueName, 0&, lValueType, lBuf, lDataBufSize)
    If lResult = ERROR_SUCCESS Then
        Select Case lValueType
        Case REG_DWORD
            lRefCount = lBuf
            RegQueryRefCount = True
        Case REG_BINARY
            If lDataBufSize = 4 Then
                lRefCount = lBuf
                RegQueryRefCount = True
            End If
        Case REG_SZ
            Dim strRefCount As String
            
            If RegQueryStringValue(hkey, strValueName, strRefCount) Then
                lRefCount = Val(strRefCount)
                RegQueryRefCount = True
            End If
        End Select
    End If
End Function
#End If

' FUNCTION: RegSetNumericValue
'
' Associates a named (strValueName = name) or unnamed (strValueName = "")
'   value with a registry key.
'
' If fLog is missing or is True, then this action is logged in the logfile,
' and the value will be deleted by the application removal utility if the
' user choose to remove the installed application.
'
' NOTE: There is no 16-bit version of this function.
'
' Returns: True on success, else False.
'
#If Win32 Then
Function RegSetNumericValue(ByVal hkey As Long, ByVal strValueName As String, ByVal lData As Long, Optional ByVal fLog) As Boolean
    Dim lResult As Long
    Dim strHkey As String

    On Error GoTo 0
    
    If IsMissing(fLog) Then fLog = True

    strHkey = strGetHKEYString(hkey)
    
    If fLog Then
        NewAction _
          gstrKEY_REGVALUE, _
          """" & strHkey & """" _
            & ", " & """" & strValueName & """"
    End If

    lResult = OSRegSetValueEx(hkey, strValueName, 0&, REG_DWORD, lData, 4)
    If lResult = ERROR_SUCCESS Then
        RegSetNumericValue = True
        If fLog Then
            CommitAction
        End If
    Else
        RegSetNumericValue = False
        MsgError ResolveResString(resERR_REG), vbOKOnly Or vbExclamation, gstrTitle
        If fLog Then
            AbortAction
        End If
    End If
End Function
#End If

' FUNCTION: RegSetStringValue
'
' Associates a named (strValueName = name) or unnamed (strValueName = "")
'   value with a registry key.
'
' If fLog is missing or is True, then this action is logged in the
' logfile, and the value will be deleted by the application removal
' utility if the user choose to remove the installed application.
'
' NOTE: For 16-bits, strValueName MUST be "" (but the
' NOTE: parameter is left in for source code compatability)
'
' NOTE: Under 16-bits, fLog is ignored.
'
' Returns: True on success, else False.
'
Function RegSetStringValue(ByVal hkey As Long, ByVal strValueName As String, ByVal strData As String, Optional ByVal fLog) As Boolean
    Dim lResult As Long
    #If Win32 Then
    Dim strHkey As String
    #End If
    
    On Error GoTo 0
    
    If IsMissing(fLog) Then fLog = True

    If hkey = 0 Then
        Exit Function
    End If
    
    #If Win32 Then
        strHkey = strGetHKEYString(hkey)
    
        If fLog Then
            NewAction _
              gstrKEY_REGVALUE, _
              """" & strHkey & """" _
                & ", " & """" & strValueName & """"
        End If
    #Else 'Win16
        If strValueName <> "" Then
            'Under 16-bits, strValueName MUST be ""
            RegSetStringValue = False
            Exit Function
        End If
    #End If

    #If Win32 Then
        lResult = OSRegSetValueEx(hkey, strValueName, 0&, REG_SZ, ByVal strData, Len(strData) + 1)
    #Else
        lResult = OSRegSetValue(hkey, "", REG_SZ, strData, Len(strData) + 1)
    #End If
    
    If lResult = ERROR_SUCCESS Then
        RegSetStringValue = True
        #If Win32 Then
            If fLog Then
                CommitAction
            End If
        #End If
    Else
        RegSetStringValue = False
        MsgError ResolveResString(resERR_REG), vbOKOnly Or vbExclamation, gstrTitle
        #If Win32 Then
            If fLog Then
                AbortAction
            End If
        #End If
    End If
End Function

'-----------------------------------------------------------
' SUB: RemoteRegister
'
' Synchronously run the client registration utility on the
' given remote server registration file in order to set it
' up properly in the registry.
'
' IN: [strFileName] - .EXE file to register
'-----------------------------------------------------------
'
Sub RemoteRegister(ByVal strFileName As String, rInfo As REGINFO)
    #If Win32 Then
        Const strClientRegistrationUtility$ = "CLIREG32.EXE"
    #Else
        Const strClientRegistrationUtility$ = "CLIREG16.EXE"
    #End If
    Const strAddressSwitch = " /s "
    Const strProtocolSwitch = " /p "
    Const strNoLogoSwitch = " /nologo "
    Const strAuthenticationSwitch = " /a "
    Const strTypelibSwitch = " /t "
    Const strEXT_REMOTE$ = "VBR"
    Const strEXT_REMOTETLB$ = "TLB"

    Dim strAddress As String
    Dim strProtocol As String
    Dim intAuthentication As Integer
    Dim strCmdLine As String
    Dim fShell As Integer
    Dim strMatchingTLB As String

    'Find the name of the matching typelib file.  This should have already
    'been installed to the same directory as the .VBR file.
    strMatchingTLB = strFileName
    If Right$(strMatchingTLB, Len(strEXT_REMOTE)) = strEXT_REMOTE Then
        strMatchingTLB = Left$(strMatchingTLB, Len(strMatchingTLB) - Len(strEXT_REMOTE))
    End If
    strMatchingTLB = strMatchingTLB & strEXT_REMOTETLB

    strAddress = rInfo.strNetworkAddress
    strProtocol = rInfo.strNetworkProtocol
    intAuthentication = rInfo.intAuthentication
    frmRemoteServerDetails.GetServerDetails strFileName, strAddress, strProtocol
    frmMessage.Refresh
    strCmdLine = _
      strClientRegistrationUtility _
      & strAddressSwitch & """" & strAddress & """" _
      & strProtocolSwitch & strProtocol _
      & strAuthenticationSwitch & Format$(intAuthentication) & " " _
      & strNoLogoSwitch _
      & strTypelibSwitch & """" & strMatchingTLB & """" & " " _
      & """" & strFileName & """"
      
    '
    'Synchronously shell out and run the utility with the correct switches
    '
    fShell = FSyncShell(strCmdLine, vbNormal)
    If Not fShell Then
        MsgError ResolveResString(resCANTRUNCLIREG, "|1", strClientRegistrationUtility), vbOKOnly Or vbExclamation, gintRET_FATAL
        ExitSetup frmSetup1, gintRET_FATAL
    End If
End Sub

'-----------------------------------------------------------
' SUB: RemoveShellLink
'
' Removes a link in either Start>Programs or any of its
' immediate subfolders in the Windows 95 shell.
'
' IN: [strFolderName] - text name of the immediate folder
'                       in which the link to be removed
'                       currently exists, or else the
'                       empty string ("") to indicate that
'                       the link can be found directly in
'                       the Start>Programs menu.
'     [strLinkName] - text caption for the link
'
' This action is never logged in the app removal logfile.
'
' PRECONDITION: strFolderName has already been created and is
'               an immediate subfolder of Start>Programs, if it
'               is not equal to ""
'-----------------------------------------------------------
'
#If Win32 And LOGGING Then
Sub RemoveShellLink(ByVal strFolderName As String, ByVal strLinkName As String)
    Dim fSuccess As Boolean
    
    ReplaceDoubleQuotes strFolderName
    ReplaceDoubleQuotes strLinkName
    
    fSuccess = OSfRemoveShellLink(strFolderName, strLinkName)
End Sub
#End If

'-----------------------------------------------------------
' FUNCTION: ResolveDestDir
'
' Given a destination directory string, equate any macro
' portions of the string to their runtime determined
' actual locations and return a string reflecting the
' actual path.
'
' IN: [strDestDir] - string containing directory macro info
'                    and/or actual dir path info
'
' Return: A string containing the resolved dir name
'-----------------------------------------------------------
'
Function ResolveDestDir(ByVal strDestDir As String) As String
    Const strMACROSTART$ = "$("
    Const strMACROEND$ = ")"

    Dim intPos As Integer
    Dim strResolved As String
    #If Win32 Then
    Dim hkey As Long
    Dim strPathsKey As String
    strPathsKey = RegPathWinCurrentVersion()
    #End If

    'We take the first part of destdir, and if its $( then we need to get the portion
    'of destdir up to and including the last paren.  We then test against this for
    'macro expansion.  If no ) is found after finding $(, then must assume that it's
    'just a normal file name and do no processing.  Only enter the case statement
    'if intPos > 0.

    If Left$(strDestDir, 2) = strMACROSTART Then
        intPos = InStr(strDestDir, strMACROEND)

        Select Case Left$(strDestDir, intPos)
        Case gstrAPPDEST
            If gstrDestDir <> gstrNULL Then
                strResolved = gstrDestDir
            Else
                strResolved = "?"
            End If
        Case gstrWINDEST
            strResolved = gstrWinDir
        Case gstrWINSYSDEST, gstrWINSYSDESTSYSFILE
            strResolved = gstrWinSysDir
        Case gstrPROGRAMFILES
            #If Win32 And LOGGING Then
                If TreatAsWin95() Then
                    Const strProgramFilesKey = "ProgramFilesDir"

                    If RegOpenKey(HKEY_LOCAL_MACHINE, strPathsKey, hkey) Then
                        RegQueryStringValue hkey, strProgramFilesKey, strResolved
                        RegCloseKey hkey
                    End If
                End If
            #End If

            If strResolved = "" Then
                'If not otherwise set, let strResolved be the root of the first fixed disk
                strResolved = strRootDrive()
            End If
        Case gstrCOMMONFILES
            'First determine the correct path of Program Files\Common Files, if under Win95
            strResolved = strGetCommonFilesPath()
            If strResolved = "" Then
                'If not otherwise set, let strResolved be the Windows directory
                strResolved = gstrWinDir
            End If
        Case gstrCOMMONFILESSYS
            'First determine the correct path of Program Files\Common Files, if under Win95
            Dim strCommonFiles As String
            
            strCommonFiles = strGetCommonFilesPath()
            If strCommonFiles <> "" Then
                'Okay, now just add \System, and we're done
                strResolved = strCommonFiles & "System\"
            Else
                'If Common Files isn't in the registry, then map the
                'entire macro to the Windows\{system,system32} directory
                strResolved = gstrWinSysDir
            End If
        Case gstrDAODEST
            strResolved = strGetDAOPath()
        Case Else
            intPos = 0
        End Select
    End If
    
    If intPos <> 0 Then
        AddDirSep strResolved
    End If

    If intPos = 0 Then
        '
        'if no drive spec, and doesn't begin with any root path indicator ("\"),
        'then we assume that this destination is relative to the app dest dir
        '
        If Mid$(strDestDir, 2, 1) <> gstrCOLON Then
            If Left$(strDestDir, 1) <> gstrSEP_DIR Then
                strResolved = gstrDestDir
            End If
        End If
    Else
        If Mid$(strDestDir, intPos + 1, 1) = gstrSEP_DIR Then
            intPos = intPos + 1
        End If
    End If

    ResolveDestDir = strResolved & Mid$(strDestDir, intPos + 1)
End Function

'-----------------------------------------------------------
' FUNCTION: ResolveDir
'
' Given a pathname, resolve it to its smallest form.  If
' the pathname is invalid, then optionally warn the user.
'
' IN: [strPathName] - pathname to resolve
'     [fMustExist] - enforce that the path actually exists
'     [fWarn] - If True, warn user upon invalid path
'
' Return: A string containing the resolved dir name
'-----------------------------------------------------------
'
Function ResolveDir(ByVal strPathName As String, fMustExist As Integer, fWarn As Integer) As String
    Const OF_PARSE% = &H100
    Const HFILE_ERROR% = -1

    Dim sOFS As OFSTRUCT
    Dim strMsg As String
    Dim fInValid As Integer
    Dim strResolvedPath As String

    On Error Resume Next

    '
    'If the pathname is a UNC name (16-bit only), or if it's in actuality a file name, then it's invalid
    '
    #If Win16 Then
    If IsUNCName(strPathName) = True Then
        fInValid = True
        GoTo RDContinue
    End If
    #End If
    If FileExists(strPathName) = True Then
        fInValid = True
        GoTo RDContinue
    End If

    strResolvedPath = strPathName

    If InStr(3, strResolvedPath, gstrSEP_DIR) > 0 Then
        '
        'temporarily remove any trailing dir sep of OpenFile will always fail
        '
        If Right$(strResolvedPath, 1) = gstrSEP_DIR Then
            strResolvedPath = Left$(strResolvedPath, Len(strResolvedPath) - 1)
        End If

        '
        'The Windows API OpenFile actually does all of the work of resolving the
        'file name, i.e.; paths like "C:\.\TEMP\..\TEMP\.\.." are resolved to "C:\"
        'and so on
        '
        If OpenFile(strResolvedPath, sOFS, OF_PARSE) = HFILE_ERROR Then
            ChDir strResolvedPath
            AddDirSep strResolvedPath
            If Err > 0 Then
                Err = 0
                ChDir strResolvedPath
                If Err > 0 Then
                    fInValid = True
                End If
            End If
        Else
            '
            'Remove any terminator and ensure that the drive specified is valid
            'and available
            '
            strResolvedPath = StripTerminator(sOFS.szPathName)
            
            If CheckDrive(strResolvedPath, gstrTitle) = False Then
                fInValid = True
            Else
                AddDirSep strResolvedPath
                If fMustExist = True Then
                    Err = 0
                    
                    Dim strDummy As String
                    strDummy = Dir$(strResolvedPath & "*.*")
                    
                    If Err > 0 Then
                        strMsg = ResolveResString(resNOTEXIST) & LS$
                        fInValid = True
                    End If
                End If
            End If
        End If
    Else
        fInValid = True
    End If

RDContinue:
    If fInValid = True Then
        If fWarn = True Then
            strMsg = strMsg & ResolveResString(resDIRSPECIFIED) & LS$ & strPathName & LS$
            strMsg = strMsg & ResolveResString(resDIRINVALID)
            MsgError strMsg, MB_OK Or MB_ICONEXCLAMATION, ResolveResString(resDIRINVNAME)
        End If

        ResolveDir = gstrNULL
    Else
        ResolveDir = strResolvedPath
    End If

    Err = 0
End Function

'-----------------------------------------------------------
' SUB: RestoreProgMan
'
' Restores Windows Program Manager
'-----------------------------------------------------------
'
Sub RestoreProgMan()
    Const strPMTITLE$ = "Program Manager"

    On Error Resume Next

    'Try the localized name first
    AppActivate ResolveResString(resPROGRAMMANAGER)
    
    If Err Then
        'If that doesn't work, try the English name
        AppActivate strPMTITLE
    End If

    Err = 0
End Sub

'-----------------------------------------------------------
' FUNCTION: SetFileDateTime
'
' Set the Destination File's date and time to the Source file's date and time
'
' IN: [strFileGetTime] - file to get time/date info from
'     [strFileSetTime] - file to set time/date info for
'
' Returns: True if set date/time successful, False otherwise
'-----------------------------------------------------------
'
Function SetFileDateTime(strFileGetTime As String, strFileSetTime As String) As Integer
    SetFileDateTime = IIf(SetTime(strFileGetTime, strFileSetTime) = -1, False, True)
End Function

'-----------------------------------------------------------
' SUB: ShowPathDialog
'
' Display form to allow user to get either a source or
' destination path
'
' IN: [strPathRequest] - determines whether to ask for the
'                        source or destination pathname.
'                        gstrDIR_SRC for source path
'                        gstrDIR_DEST for destination path
'-----------------------------------------------------------
'
Sub ShowPathDialog(ByVal strPathRequest As String)
    frmSetup1.Tag = strPathRequest

    '
    'frmPath.Form_Load() reads frmSetup1.Tag to determine whether
    'this is a request for the source or destination path
    '
    frmPath.Show 1

    If strPathRequest = gstrDIR_SRC Then
        gstrSrcPath = frmSetup1.Tag
    Else
        If gfRetVal = gintRET_CONT Then
            gstrDestDir = frmSetup1.Tag
        End If
    End If
End Sub

'-----------------------------------------------------------
' FUNCTION: strExtractFilenameArg
'
' Extracts a quoted or unquoted filename from a string
'   containing command-line arguments
'
' IN: [str] - string containing a filename.  This filename
'             begins at the first character, and continues
'             to the end of the string or to the first space
'             or switch character, or, if the string begins
'             with a double quote, continues until the next
'             double quote
' OUT: Returns the filename, without quotes
'      str is set to be the remainder of the string after
'      the filename and quote (if any)
'
'-----------------------------------------------------------
'
Function strExtractFilenameArg(str As String, fErr As Boolean)
    Dim strFileName As String
    
    str = Trim$(str)
    
    Dim iEndFilenamePos As Integer
    If Left$(str, 1) = """" Then
        ' Filenames is surrounded by quotes
        iEndFilenamePos = InStr(2, str, """") ' Find matching quote
        If iEndFilenamePos > 0 Then
            strFileName = Mid$(str, 2, iEndFilenamePos - 2)
            str = Right$(str, Len(str) - iEndFilenamePos)
        Else
            fErr = True
            Exit Function
        End If
    Else
        ' Filename continues until next switch or space or quote
        Dim iSpacePos As Integer
        Dim iSwitch1 As Integer
        Dim iSwitch2 As Integer
        Dim iQuote As Integer
        
        iSpacePos = InStr(str, " ")
        iSwitch1 = InStr(str, gstrSwitchPrefix1)
        iSwitch2 = InStr(str, gstrSwitchPrefix2)
        iQuote = InStr(str, """")
        
        If iSpacePos = 0 Then iSpacePos = Len(str) + 1
        If iSwitch1 = 0 Then iSwitch1 = Len(str) + 1
        If iSwitch2 = 0 Then iSwitch2 = Len(str) + 1
        If iQuote = 0 Then iQuote = Len(str) + 1
        
        iEndFilenamePos = iSpacePos
        If iSwitch1 < iEndFilenamePos Then iEndFilenamePos = iSwitch1
        If iSwitch2 < iEndFilenamePos Then iEndFilenamePos = iSwitch2
        If iQuote < iEndFilenamePos Then iEndFilenamePos = iQuote
        
        strFileName = Left$(str, iEndFilenamePos - 1)
        If iEndFilenamePos > Len(str) Then
            str = ""
        Else
            str = Right(str, Len(str) - iEndFilenamePos + 1)
        End If
    End If
    
    strFileName = Trim$(strFileName)
    If strFileName = "" Then
        fErr = True
        Exit Function
    End If
    
    fErr = False
    strExtractFilenameArg = strFileName
    str = Trim$(str)
End Function

'-----------------------------------------------------------
' FUNCTION: strStripQuotes
'
' Removes double quotes from the beginning and ending of a
' string, if they are present
'-----------------------------------------------------------
'
Function strStripQuotes(ByVal str As String) As String
    If Left$(str, 1) = """" Then
        str = Mid$(str, 2)
    End If
    If Right$(str, 1) = """" Then
        str = Mid$(str, 1, Len(str) - 1)
    End If
    
    strStripQuotes = str
End Function

'-----------------------------------------------------------
' SUB: TreatAsWin95
'
' Returns True iff either we're running under Windows 95
' or we are treating this version of NT as if it were
' Windows 95 for registry and application loggin and
' removal purposes.  (Note:  for this version, the function
' is true only when IsWindows95() is true.)
'-----------------------------------------------------------
'
#If Win32 And LOGGING Then
Function TreatAsWin95() As Boolean
    If IsWindows95() Then
        TreatAsWin95 = True
    ElseIf fNTWithShell() Then
        TreatAsWin95 = True
    Else
        TreatAsWin95 = False
    End If
End Function
#End If

'-----------------------------------------------------------
' SUB: UpdateStatus
'
' "Fill" (by percentage) inside the PictureBox and also
' display the percentage filled
'
' IN: [pic] - PictureBox used to bound "fill" region
'     [sngPercent] - Percentage of the shape to fill
'     [fBorderCase] - Indicates whether the percentage
'        specified is a "border case", i.e. exactly 0%
'        or exactly 100%.  Unless fBorderCase is True,
'        the values 0% and 100% will be assumed to be
'        "close" to these values, and 1% and 99% will
'        be used instead.
'
' Notes: Set AutoRedraw property of the PictureBox to True
'        so that the status bar and percentage can be auto-
'        matically repainted if necessary
'-----------------------------------------------------------
'
Sub UpdateStatus(pic As PictureBox, ByVal sngPercent As Single, Optional ByVal fBorderCase)
    Dim strPercent As String
    Dim intX As Integer
    Dim intY As Integer
    Dim intWidth As Integer
    Dim intHeight As Integer

    If IsMissing(fBorderCase) Then fBorderCase = False
    
    'For this to work well, we need a white background and any color foreground (blue)
    Const colBackground = &HFFFFFF ' white
    Const colForeground = &H800000 ' dark blue

    pic.ForeColor = colForeground
    pic.BackColor = colBackground
    
    '
    'Format percentage and get attributes of text
    '
    Dim intPercent
    intPercent = Int(100 * sngPercent + 0.5)
    
    'Never allow the percentage to be 0 or 100 unless it is exactly that value.  This
    'prevents, for instance, the status bar from reaching 100% until we are entirely done.
    If intPercent = 0 Then
        If Not fBorderCase Then
            intPercent = 1
        End If
    ElseIf intPercent = 100 Then
        If Not fBorderCase Then
            intPercent = 99
        End If
    End If
    
    strPercent = Format$(intPercent) & "%"
    intWidth = pic.TextWidth(strPercent)
    intHeight = pic.TextHeight(strPercent)

    '
    'Now set intX and intY to the starting location for printing the percentage
    '
    intX = pic.Width / 2 - intWidth / 2
    intY = pic.Height / 2 - intHeight / 2

    '
    'Need to draw a filled box with the pics background color to wipe out previous
    'percentage display (if any)
    '
    pic.DrawMode = 13 ' Copy Pen
    pic.Line (intX, intY)-Step(intWidth, intHeight), pic.BackColor, BF

    '
    'Back to the center print position and print the text
    '
    pic.CurrentX = intX
    pic.CurrentY = intY
    pic.Print strPercent

    '
    'Now fill in the box with the ribbon color to the desired percentage
    'If percentage is 0, fill the whole box with the background color to clear it
    'Use the "Not XOR" pen so that we change the color of the text to white
    'wherever we touch it, and change the color of the background to blue
    'wherever we touch it.
    '
    pic.DrawMode = 10 ' Not XOR Pen
    If sngPercent > 0 Then
        pic.Line (0, 0)-(pic.Width * sngPercent, pic.Height), pic.ForeColor, BF
    Else
        pic.Line (0, 0)-(pic.Width, pic.Height), pic.BackColor, BF
    End If

    pic.Refresh
End Sub

'-----------------------------------------------------------
' FUNCTION: WriteAccess
'
' Determines whether there is write access to the specified
' directory.
'
' IN: [strDirName] - directory to check for write access
'
' Returns: True if write access, False otherwise
'-----------------------------------------------------------
'
Function WriteAccess(ByVal strDirName As String) As Integer
    Dim intFileNum As Integer

    On Error Resume Next

    AddDirSep strDirName

    intFileNum = FreeFile
    Open strDirName & mstrCONCATFILE For Output As intFileNum

    WriteAccess = IIf(Err, False, True)
    
    Close intFileNum

    Kill strDirName & mstrCONCATFILE

    Err = 0
End Function

'Adds or replaces an HKEY to the list of HKEYs in cache.
'Note that it is not necessary to remove keys from
'this list.
Private Sub AddHkeyToCache(ByVal hkey As Long, ByVal strHkey As String)
    Dim intIdx As Integer
    
    intIdx = intGetHKEYIndex(hkey)
    If intIdx < 0 Then
        'The key does not already exist.  Add it to the end.
        On Error Resume Next
        ReDim Preserve hkeyCache(0 To UBound(hkeyCache) + 1)
        If Err Then
            'If there was an error, it means the cache was empty.
            On Error GoTo 0
            ReDim hkeyCache(0 To 0)
        End If
        On Error GoTo 0

        intIdx = UBound(hkeyCache)
    Else
        'The key already exists.  It will be replaced.
    End If

    hkeyCache(intIdx).hkey = hkey
    hkeyCache(intIdx).strHkey = strHkey
End Sub

'Given a predefined HKEY, return the text string representing that
'key, or else return "".
Private Function strGetPredefinedHKEYString(ByVal hkey As Long) As String
    Select Case hkey
    Case HKEY_CLASSES_ROOT
        strGetPredefinedHKEYString = "HKEY_CLASSES_ROOT"
    #If Win32 Then
    Case HKEY_CURRENT_USER
        strGetPredefinedHKEYString = "HKEY_CURRENT_USER"
    Case HKEY_LOCAL_MACHINE
        strGetPredefinedHKEYString = "HKEY_LOCAL_MACHINE"
    Case HKEY_USERS
        strGetPredefinedHKEYString = "HKEY_USERS"
    #End If
    End Select
End Function

'Given an HKEY, return the text string representing that
'key.
Private Function strGetHKEYString(ByVal hkey As Long) As String
    Dim strKey As String

    'Is the hkey predefined?
    strKey = strGetPredefinedHKEYString(hkey)
    If strKey <> "" Then
        strGetHKEYString = strKey
        Exit Function
    End If
    
    'It is not predefined.  Look in the cache.
    Dim intIdx As Integer
    intIdx = intGetHKEYIndex(hkey)
    If intIdx >= 0 Then
        strGetHKEYString = hkeyCache(intIdx).strHkey
    Else
        strGetHKEYString = ""
    End If
End Function

'Searches the cache for the index of the given HKEY.
'Returns the index if found, else returns -1.
Private Function intGetHKEYIndex(ByVal hkey As Long) As Integer
    Dim intUBound As Integer
    
    On Error Resume Next
    intUBound = UBound(hkeyCache)
    If Err Then
        'If there was an error accessing the ubound of the array,
        'then the cache is empty
        GoTo NotFound
    End If
    On Error GoTo 0

    Dim intIdx As Integer
    For intIdx = 0 To intUBound
        If hkeyCache(intIdx).hkey = hkey Then
            intGetHKEYIndex = intIdx
            Exit Function
        End If
    Next intIdx
    
NotFound:
    intGetHKEYIndex = -1
End Function

'Returns the location of the Program Files\Common Files path, if
'it is present in the registry.  Otherwise, returns "".
Private Function strGetCommonFilesPath() As String
    Dim hkey As Long
    Dim strPath As String
    
    #If Win32 Then
        If TreatAsWin95() Then
            Const strCommonFilesKey = "CommonFilesDir"

            If RegOpenKey(HKEY_LOCAL_MACHINE, RegPathWinCurrentVersion(), hkey) Then
                RegQueryStringValue hkey, strCommonFilesKey, strPath
                RegCloseKey hkey
            End If
        End If
    #End If

    If strPath <> "" Then
        AddDirSep strPath
    End If
    
    strGetCommonFilesPath = strPath
End Function

'Returns the directory where DAO is or should be installed.  If the
'key does not exist in the registry, it is created.  For instance, under
'NT 3.51 this location is normally 'C:\WINDOWS\MSAPPS\DAO'
Private Function strGetDAOPath() As String
    Const strMSAPPS$ = "MSAPPS\"
    Const strDAO3032$ = "DAO3032.DLL"
    
    #If Win16 Then
        'For 16-bits, DAO is always in windows\MSAPPS\DAO
        strGetDAOPath = gstrWinDir & strMSAPPS & "DAO"
        Exit Function
    #Else
        'For Win32, first look in the registry
        Const strKey = "SOFTWARE\Microsoft\Shared Tools\DAO"
        Const strValueName = "Path"
        Dim hkey As Long
        Dim strPath As String

        If RegOpenKey(HKEY_LOCAL_MACHINE, strKey, hkey) Then
            RegQueryStringValue hkey, strValueName, strPath
            RegCloseKey hkey
        End If

        If strPath <> "" Then
            strPath = GetPathName(strPath)
            AddDirSep strPath
            strGetDAOPath = strPath
            Exit Function
        End If
        
        'It's not yet in the registry, so we need to decide
        'where the directory should be, and then need to place
        'that location in the registry.
    
        If TreatAsWin95() Then
            'For Win95, use "Common Files\Microsoft Shared\DAO"
            strPath = strGetCommonFilesPath() & ResolveResString(resMICROSOFTSHARED) & "DAO\"
        Else
            'Otherwise use Windows\MSAPPS\DAO
            strPath = gstrWinDir & strMSAPPS & "DAO\"
        End If
        
        'Place this information in the registry (note that we point to DAO3032.DLL
        'itself, not just to the directory)
        If RegCreateKey(HKEY_LOCAL_MACHINE, strKey, "", hkey) Then
            RegSetStringValue hkey, strValueName, strPath & strDAO3032, False
            RegCloseKey hkey
        End If

        strGetDAOPath = strPath
    #End If
End Function

' Replace all double quotes with single quotes
Public Sub ReplaceDoubleQuotes(str As String)
    Dim i As Integer
    
    For i = 1 To Len(str)
        If Mid$(str, i, 1) = """" Then
            Mid$(str, i, 1) = "'"
        End If
    Next i
End Sub

'Get the path portion of a filename
Function GetPathName(ByVal strFileName As String) As String
    Dim intPos As Integer
    Dim strPathOnly As String
    Dim dirTmp As DirListBox
    Dim i As Integer

    On Error Resume Next

    Err = 0
    
    intPos = Len(strFileName)

    '
    'Change all '/' chars to '\'
    '

    For i = 1 To Len(strFileName)
        If Mid$(strFileName, i, 1) = gstrSEP_DIRALT Then
            Mid$(strFileName, i, 1) = gstrSEP_DIR
        End If
    Next i

    If InStr(strFileName, gstrSEP_DIR) = intPos Then
        If intPos > 1 Then
            intPos = intPos - 1
        End If
    Else
        Do While intPos > 0
            If Mid$(strFileName, intPos, 1) <> gstrSEP_DIR Then
                intPos = intPos - 1
            Else
                Exit Do
            End If
        Loop
    End If

    If intPos > 0 Then
        strPathOnly = Left$(strFileName, intPos)
        If Right$(strPathOnly, 1) = gstrCOLON Then
            strPathOnly = strPathOnly & gstrSEP_DIR
        End If
    Else
        strPathOnly = CurDir$
    End If

    If Right$(strPathOnly, 1) = gstrSEP_DIR Then
        strPathOnly = Left$(strPathOnly, Len(strPathOnly) - 1)
    End If

    GetPathName = UCase16(strPathOnly)
    
    Err = 0
End Function

'Returns the path to the root of the first fixed disk
Function strRootDrive() As String
    Dim intDriveNum As Integer
    
    For intDriveNum = 0 To Asc("Z") - Asc("A") - 1
        If GetDriveType(intDriveNum) = intDRIVE_FIXED Then
            strRootDrive = Chr$(Asc("A") + intDriveNum) & gstrCOLON & gstrSEP_DIR
            Exit Function
        End If
    Next intDriveNum
    
    strRootDrive = "C:\"
End Function

'Returns "" if the path is not complete, or is a UNC pathname
Function strGetDriveFromPath(ByVal strPath As String) As String
    If Len(strPath) < 2 Then
        Exit Function
    End If
    
    If Mid$(strPath, 2, 1) <> gstrCOLON Then
        Exit Function
    End If
    
    strGetDriveFromPath = Mid$(strPath, 1, 1) & gstrCOLON & gstrSEP_DIR
End Function

#If Win16 Then
'Searches for a file in the current directory, in Windows, in
'Windows\System, or on the path.  Returns the full path if
'found, else returns the empty string ("").
Function strFindFile16(ByVal strFileName As String) As String
    Dim openBuf As OFSTRUCT
    
    If OpenFile(strFileName, openBuf, OF_EXIST Or OF_SEARCH) <> HFILE_ERROR Then
        strFindFile16 = StripTerminator(openBuf.szPathName)
    End If
End Function
#End If

#If Win16 Then
Sub InstallRpcRegFile()
    Const RPCREG_KEY = "RPC_REG_DATA_FILE"
    Const RPCREG_WIN_INI_SECTION = "Rpc Runtime Configuration"
    Const WIN_INI_FILE_NAME = "win.ini"
    Const PROTOCOL_INI_FILE_NAME = "protocol.ini"
    Const LANA_SECTION = "network.setup"
    Const LANA_PREFIX = "lana"
    Const RPC_REG_NB_PATH = "\Hkey_Local_Machine\Software\Microsoft\Rpc\NetBios\"
    Const RPC_DAT_NB_PREFIX = "\Root\Software\Microsoft\Rpc\NetBios\"
    Const RPC_REG_NB_IPX = "ncacn_nb_ipx0"
    Const RPC_REG_NB_NB = "ncacn_nb_nb0"
    Const RPC_REG_NB_TCP = "ncacn_nb_tcp0"
    Const RPC_REG_NB_DECNET = "ncacn_nb_dnet0"
    Const RPC_REG_NB_XNS = "ncacn_nb_xns0"

    Dim bSuccess As Integer
    Dim strBuf As String
    Dim nBufSize As Integer
    Dim i As Integer
    Dim bFileOpen As Integer
    Dim nFile As Integer
    Dim bShouldCreate As Integer
    Dim strError As String
    
RetryAll:
    On Error GoTo irrfErr
    strError = ""

    strBuf = Space$(gintMAX_SIZE)

    'If rpcreg.dat file already exists, do nothing and return

    If FileExists(GetWindowsDir() & mstrFILE_RPCREG) Then Exit Sub
    If FileExists(GetWindowsSysDir() & mstrFILE_RPCREG) Then Exit Sub
    If strFindFile16(mstrFILE_RPCREG) <> "" Then Exit Sub
    If FileExists(strRootDrive() & mstrFILE_RPCREG) Then Exit Sub
    If FileExists(strGetDriveFromPath(GetWindowsDir()) & mstrFILE_RPCREG) Then Exit Sub
    If FileExists(strGetDriveFromPath(GetWindowsSysDir()) & mstrFILE_RPCREG) Then Exit Sub

    'If RPC_REG_DATA_FILE is set in win.ini, do nothing and return

    If ReadIniFile(WIN_INI_FILE_NAME, RPCREG_WIN_INI_SECTION, RPCREG_KEY) <> "" Then Exit Sub

    'If RPC_REG_DATA_FILE is in the DOS environment, do nothing and return

    If Environ(RPCREG_KEY) <> "" Then Exit Sub

    bShouldCreate = True

RetryCreate:
    On Error GoTo irrfErr
    strError = ""

    'Search for PROTOCOL.INI, first in Windows and Windows\System, and then in the path
    Dim strProtocolINI As String
    strProtocolINI = GetWindowsDir() & PROTOCOL_INI_FILE_NAME
    If Not FileExists(strProtocolINI) Then
        strProtocolINI = GetWindowsSysDir() & PROTOCOL_INI_FILE_NAME
        If Not FileExists(strProtocolINI) Then
            strProtocolINI = strFindFile16(PROTOCOL_INI_FILE_NAME)
            If Not FileExists(strProtocolINI) Then
                strProtocolINI = PROTOCOL_INI_FILE_NAME
            End If
        End If
    End If
            
    'Get any existing LANA indexes for netbios protocols from protocol.ini and write them to rpcreg.dat
    nFile = FreeFile
    Open strRootDrive() & mstrFILE_RPCREG For Output As nFile 'create rpcreg.dat file in root dir
    bFileOpen = True
    i = 0
    strBuf = ReadIniFile(strProtocolINI, LANA_SECTION, LANA_PREFIX & Format(i))
    While strBuf <> ""
        If InStr(strBuf, "ipx") > 0 Then
            Print #nFile, RPC_DAT_NB_PREFIX & RPC_REG_NB_IPX & "=" & Format(i)
        ElseIf InStr(strBuf, "tcp") > 0 Then
            Print #nFile, RPC_DAT_NB_PREFIX & RPC_REG_NB_TCP & "=" & Format(i)
        ElseIf InStr(strBuf, "netbeui") > 0 Then
            Print #nFile, RPC_DAT_NB_PREFIX & RPC_REG_NB_NB & "=" & Format(i)
        ElseIf InStr(strBuf, "dnet") > 0 Then
            Print #nFile, RPC_DAT_NB_PREFIX & RPC_REG_NB_DECNET & "=" & Format(i)
        ElseIf InStr(strBuf, "decnet") > 0 Then
            Print #nFile, RPC_DAT_NB_PREFIX & RPC_REG_NB_DECNET & "=" & Format(i)
        End If
        
        i = i + 1
        strBuf = ReadIniFile(strProtocolINI, LANA_SECTION, LANA_PREFIX & Format(i))
    Wend
    GoTo irrfExit

irrfErr:
    strError = Error$
    Resume irrfRecover

irrfRecover:
    On Error Resume Next
    If bFileOpen Then Close nFile
    bFileOpen = False
    On Error GoTo 0
    
    Select Case MsgError(ResolveResString(resUNEXPECTEDRPCREGDAT, "|1", mstrFILE_RPCREG) & LS$ & strError, vbExclamation Or vbAbortRetryIgnore, gstrTitle)
    Case vbAbort
        ExitSetup frmSetup1, gintRET_ABORT
        GoTo irrfRecover
    Case vbRetry
        If bShouldCreate Then
            GoTo RetryCreate
        Else
            GoTo RetryAll
        End If
    Case vbIgnore
        Exit Sub
    End Select
    
irrfExit:
    If bFileOpen Then Close nFile
End Sub
#End If

