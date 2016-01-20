VERSION 4.00
Begin VB.Form frmSetup1 
   AutoRedraw      =   -1  'True
   BackColor       =   &H00400000&
   BorderStyle     =   0  'None
   Caption         =   "VB4 Setup Toolkit"
   ClientHeight    =   1770
   ClientLeft      =   225
   ClientTop       =   1590
   ClientWidth     =   7950
   ClipControls    =   0   'False
   DrawStyle       =   5  'Transparent
   FillStyle       =   0  'Solid
   BeginProperty Font 
      name            =   "Times New Roman"
      charset         =   1
      weight          =   700
      size            =   24
      underline       =   0   'False
      italic          =   -1  'True
      strikethrough   =   0   'False
   EndProperty
   ForeColor       =   &H00000000&
   Height          =   2145
   Icon            =   "SETUP1.frx":0000
   Left            =   165
   LinkMode        =   1  'Source
   LockControls    =   -1  'True
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   118
   ScaleMode       =   3  'Pixel
   ScaleWidth      =   530
   ShowInTaskbar   =   0   'False
   Top             =   1275
   Width           =   8070
   WindowState     =   2  'Maximized
   Begin VB.Label lblModify 
      AutoSize        =   -1  'True
      BorderStyle     =   1  'Fixed Single
      Caption         =   $"SETUP1.frx":0442
      BeginProperty Font 
         name            =   "MS Sans Serif"
         charset         =   1
         weight          =   400
         size            =   8.25
         underline       =   0   'False
         italic          =   0   'False
         strikethrough   =   0   'False
      EndProperty
      Height          =   450
      Left            =   15
      TabIndex        =   1
      Top             =   15
      Visible         =   0   'False
      Width           =   7860
      WordWrap        =   -1  'True
   End
   Begin VB.Label lblDDE 
      AutoSize        =   -1  'True
      BorderStyle     =   1  'Fixed Single
      Caption         =   "This label is used for DDE connection to the Program Manager"
      BeginProperty Font 
         name            =   "MS Sans Serif"
         charset         =   1
         weight          =   400
         size            =   8.25
         underline       =   0   'False
         italic          =   0   'False
         strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   15
      TabIndex        =   0
      Top             =   1515
      Visible         =   0   'False
      Width           =   4485
   End
End
Attribute VB_Name = "frmSetup1"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
Option Explicit
Option Compare Text

'
'Can't put this is a resource because it indicated resource load failure, must localize separately
'
Const mstrRESOURCELOADFAIL$ = "An error occurred while initializing string resources used by Setup."
'-----------------------------------------------------------
' SUB: ShowLaunchForm
'
' Displays the begin setup form
'-----------------------------------------------------------
'
Private Sub ShowLaunchForm()
    frmLaunch.Show 1
End Sub


'-----------------------------------------------------------
' SUB: ShowInstallForm
'
' Displays the begin setup form
'-----------------------------------------------------------
'
Private Sub ShowInstallForm()
    frmInstallApp.Show 1
End Sub


'-----------------------------------------------------------
' SUB: DrawBackGround
'
' Draws the 'blue wash' screen and prints the 'shadowed'
' app setup title
'-----------------------------------------------------------
'
Private Sub DrawBackGround()
    Const intBLUESTART% = 255
    Const intBLUEEND% = 0
    Const intBANDHEIGHT% = 2
    Const intSHADOWSTART% = 8
    Const intSHADOWCOLOR% = 0
    Const intTEXTSTART% = 4
    Const intTEXTCOLOR% = 15

    Dim sngBlueCur As Single
    Dim sngBlueStep As Single
    Dim intFormHeight As Integer
    Dim intFormWidth As Integer
    Dim intY As Integer

    '
    'Get system values for height and width
    '
    intFormHeight = ScaleHeight
    intFormWidth = ScaleWidth

    '
    'Calculate step size and blue start value
    '
    sngBlueStep = intBANDHEIGHT * (intBLUEEND - intBLUESTART) / intFormHeight
    sngBlueCur = intBLUESTART

    '
    'Paint blue screen
    '
    For intY = 0 To intFormHeight Step intBANDHEIGHT
        Line (-1, intY - 1)-(intFormWidth, intY + intBANDHEIGHT), RGB(0, 0, sngBlueCur), BF
        sngBlueCur = sngBlueCur + sngBlueStep
    Next intY

    '
    'Print 'shadowed' appname
    '
    CurrentX = intSHADOWSTART
    CurrentY = intSHADOWSTART
    ForeColor = QBColor(intSHADOWCOLOR)
    Print Caption
    CurrentX = intTEXTSTART
    CurrentY = intTEXTSTART
    ForeColor = QBColor(intTEXTCOLOR)
    Print Caption
End Sub
'-----------------------------------------------------------
' SUB: GetInstalledComponents
'
' Checks to see if VFW and Win32s need to be installed.
'-----------------------------------------------------------
'
Private Sub GetInstalledComponents()
    If IsWin32 Then
    
    Else
        gintWin32sInstalled = False
    End If
End Sub

Private Sub Form_Load()
    Dim hkey As Long
    '
    ' Debug Message for Debugging the program the old way
    '
    'Dim Msg As String
    'Dim Style As Integer
    'Dim Title As String
    'Dim response As Integer
    
    'Msg = "The string you are calling is:  " + strTemp   ' Define message.
    'Style = vbOKOnly + vbExclamation    ' Define buttons.
    'Title = "Debug Message"             ' Define title.
    
    'response = MsgBox(Msg, Style, Title)' Display message.
    
    '
    'Check to see if the app is already running, if so then bail.
    '
    If FindWindow("jungle", vbNullString) Then
        ExitSetup Me, gintRET_FINISHEDSUCCESSNOMESSAGE
    End If
    
    Dim strTemp As String
    Dim intTemp As Integer
    Dim ReadPath As String * 100
    
    Const strINI_FILES$ = "Files"           'default section to install
    Const strEXT_GRP$ = "GRP"               'extension for progman group
    
    #If Win16 Then
    Dim fBtrieve As Integer                                 'Uses Btrieve?
    #End If
    Dim strGrpPath As String                                'Name for group file
    Dim strGroupName As String                              'Name of Program Group
    Dim sFile As FILEINFO                                   'first Files= line info

    On Error GoTo MainError
    
    '
    'Initialize linespacing variables for message box calls
    '
    LF$ = Chr$(10)
    LS$ = LF$ & LF$

    '
    'Initialize string resources used by global vars and forms/controls
    '
    GetStrings
    
    '
    'Get Windows and Windows\System directories
    '
    gstrWinDir = GetWindowsDir()
    gstrWinSysDir = GetWindowsSysDir()

    '
    'If the Windows System directory is a subdirectory of the
    'Windows directory, the proper place for installation of
    'files specified in the setup.lst as $(WinSysDest) is always
    'the Windows \System directory.  If the Windows \System
    'directory is *not* a subdirectory of the Windows directory,
    'then the user is running a shared version of Windows.  In
    'this case, if the user does not have write access to the
    'shared system directory, we change the system files
    'destination to the windows directory
    '
    If InStr(gstrWinSysDir, gstrWinDir) = 0 Then
        If WriteAccess(gstrWinSysDir) = False Then
            gstrWinSysDir = gstrWinDir
        End If
    End If
    
    '
    'The command-line arguments must be processed as early
    'as possible, because without them it is impossible to
    'call the app removal program to clean up after an aborted
    'setup.
    '
    #If Win32 And LOGGING Then
        ProcessCommandLine Command$, gstrSrcPath, gstrAppRemovalLog, gstrAppRemovalEXE
    #Else
        ProcessCommandLine Command$, gstrSrcPath
    #End If
    
    AddDirSep gstrSrcPath
    
    '
    'The Setup Bootstrapper (SETUP.EXE) copies SETUP1/132.EXE and SETUP.LST to
    'the end user's windows directory.  Information required for setup such
    'as setup flags and fileinfo is read from the copy of SETUP.LST found in
    'that directory.
    '
    gstrSetupInfoFile = gstrWinDir & gstrFILE_SETUP
    
    gstrAppName = ReadIniFile(gstrSetupInfoFile, gstrINI_SETUP, gstrINI_APPNAME)
    If gstrAppName = gstrNULL Then
        MsgError ResolveResString(resNOSETUPLST), MB_OK Or MB_ICONSTOP, gstrSETMSG
        gstrTitle = ResolveResString(resSETUP, "|1", gstrAppName)
        ExitSetup Me, gintRET_FATAL
    End If
    
    gstrTitle = ResolveResString(resSETUP, "|1", gstrAppName)
    
    '
    'Get name of application's executable file.  This name will be added to the
    'program manager.  Only the EXE Name should be entered under this key in
    'the setup information file (SETUP.LST).
    '
    gstrAppExe = ReadIniFile(gstrSetupInfoFile, gstrINI_SETUP, gstrINI_APPEXE)
    
    '
    'Try to find the program EXE. If we can't find it run the setup procedure,
    'and if not then run the launch procedure.
    '
    intTemp = GetPrivateProfileString("Options", "Path", "", ReadPath, Len(ReadPath), gstrIniFile)
    strTemp = Left$(ReadPath, intTemp)
    Dim strTempPath As String
    Dim intDoInstall As Integer
    
    If IsWin32 Then
        strTempPath = "SETUP\WIN32\JUNGLE.EX_"
    Else
        strTempPath = "SETUP\WIN16\JUNGLE.EX_"
    End If
    intDoInstall = True
    If DoesFileExist(strTemp & "JUNGLE.EXE") Then
        If IsInstallFileTheSame(gstrSrcPath & strTempPath, strTemp & "JUNGLE.EXE") Then
            intDoInstall = False
        End If
    End If
        
    If Not intDoInstall Then
        '
        'Display the Lauch form becase we have already been installed.
        '
        ShowLaunchForm
    Else
        'Nuke the Setup Icon if it is in the StartUp Group
        If Not IsWin32 Then
            intTemp = GetPrivateProfileString("Settings", "Startup", "", ReadPath, Len(ReadPath), "PROGMAN.INI")
            strTemp = Left$(ReadPath, intTemp)
            If strTemp = "" Then
                strTemp = "StartUp"
            End If
            CreateOSProgramGroup frmSetup1, strTemp, "STARTUP.GRP"
            DeleteOSLink frmSetup1, strTemp, "Jungle Book Setup"
        End If
    
        '
        'Display the background "blue-wash" setup screen as soon as we get the title
        '
        ShowMainForm
    
        '
        'If this key has a non-zero value in SETUP.LST, then set the variable to
        'TRUE - otherwise set it to false
        '
        #If Win16 Then
            fBtrieve = IIf(Val(ReadIniFile(gstrSetupInfoFile, gstrINI_SETUP, gstrINI_BTRIEVE)), True, False)
        #End If
    
        '
        'Display the welcome dialog
        '
        ShowWelcomeForm
    
        'If this flag is set, then the default destination directory is used
        'without question, and the user is never given a chance to change it.
        'This is intended for installing an .EXE/.DLL as a component rather
        'than as an application in an application directory.  In this case,
        'having an application directory does not really make sense.
        If ReadIniFile(gstrSetupInfoFile, gstrINI_SETUP, gstrINI_FORCEUSEDEFDEST) = "1" Then
            gfForceUseDefDest = True
        End If
        
        '
        'Read default destination directory.  If the name specified conflicts
        'with the name of a file, then prompt for a new default directory
        '
        gstrDestDir = ResolveDestDir(ReadIniFile(gstrSetupInfoFile, gstrINI_SETUP, gstrINI_APPDIR))
        While FileExists(gstrDestDir) = True Or gstrDestDir = gstrNULL
            If MsgError(ResolveResString(resBADDEFDIR), MB_OKCANCEL Or MB_ICONQUESTION, gstrSETMSG) = IDCANCEL Then
                ExitSetup Me, gintRET_FATAL
            End If
            ShowPathDialog gstrDIR_DEST
        Wend
    
        '
        'Ensure a trailing backslash on the destination directory
        '
        AddDirSep gstrDestDir
    
        Do
            '
            'Display install button and default directory.  The user
            'can change the destination directory from here.
            '
            'ShowBeginForm
    
            '
            'Display the installation form
            '
            ShowInstallForm
            
            '
            'Initialize "table" of drives used and disk space array
            '
            InitDiskInfo
    
            SetMousePtr gintMOUSE_HOURGLASS
            ShowStaticMessageDialog ResolveResString(resDISKSPACE)
    
            '
            'For every section in SETUP.LST that will be installed, call CalcDiskSpace
            'with the name of the section
            '
            CalcDiskSpace "CommonFiles"
            
            If IsWin32 Then
                CalcDiskSpace "App32Files"
            Else
                CalcDiskSpace "App16Files"
            End If
            
            'If you created an options dialog, you need to check results here to
            'determine whether disk space needs to be calculated (if the option(s)
            'will be installed)
            '
            'If chkInstallSamples.Value = TRUE then
            '    CalcDiskSpace "Samples"
            'End If
            '
    
            HideStaticMessageDialog
            SetMousePtr gintMOUSE_DEFAULT
    
        '
        'After all CalcDiskSpace calls are complete, call CheckDiskSpace to check
        'the results and display warning form (if necessary).  If the user wants
        'to try another destination directory (or cleanup and retry) then
        'CheckDiskSpace will return False
        '
        Loop While CheckDiskSpace() = False
    
        '
        'Show copy form and set copy gauge percentage to zero
        '
        SetMousePtr gintMOUSE_HOURGLASS
        ShowCopyDialog
        UpdateStatus frmCopy.picStatus, 0, True
    
        '
        'Always start with Disk #1
        '
        gintCurrentDisk = 1
    
        '
        'Starts logging to the setup logfile (will be used for application removal)
        '
        #If Win32 And LOGGING Then
            EnableLogging gstrAppRemovalLog
            
            'Should go ahead and force the application directory to be created,
            'since the application removal logfile will later be copied there.
            MakePath gstrDestDir, False 'User may not ignore errors here
        #End If
        
        '
        'For every section in SETUP.LST that needs to be installed, call CopySection
        'with the name of the section
        '
        
        CopySection "CommonFiles"
        
        If IsWin32 Then
            CopySection "App32Files"
        Else
            CopySection "App16Files"
        End If
    
        UpdateStatus frmCopy.picStatus, 1, True
        
        HideCopyDialog
    
        '
        'Now, do all the 'invisible' update things that are required
        '
        SetMousePtr gintMOUSE_DEFAULT
        ShowStaticMessageDialog ResolveResString(resUPDATING)
    
        '
        'Add Btrieve info to win.ini if necessary
        '
        #If Win16 Then
            If fBtrieve = True Then
                DoBtrieve
            End If
        #End If
        
        '
        'Add file sharing (via VSHARE.386) if required
        '
        #If Win16 Then
            AddShareIfNeeded
        #End If
    
        '
        'Register all the files that have been saved in the registration array.  The
        'CopySection API adds a registration entry (when required) if a file is copied.
        '
        RegisterFiles
    
        '
        '(32-bit Enterprise edition only)
        'Create a separate program group and icons for the Remote Automation
        'Connection Manager and the Automation Manager, if either has been
        'installed.
        'This program group is entirely separate from the one created for the
        'application program (if any), because it will be shared by all
        'VB applications which install them.
        '
        'NOTE: This is NOT the place to install additional icons.  This is
        'NOTE: handled after the Remote Automation icons have been created.
        #If Win32 Then
            ShowStaticMessageDialog ResolveResString(resPROGMAN)
            If gsDest.strAUTMGR32 <> "" Or gsDest.strRACMGR32 <> "" Then
                'At least one of these programs was installed.  Go ahead
                'and create the program group.
                Const strREMAUTGROUPPATH = "RemAutVB.GRP"
                CreateOSProgramGroup frmSetup1, ResolveResString(resREMAUTGROUPNAME), strREMAUTGROUPPATH
    
                'Now create the icons for AUTMGR32.EXE and RACMGR32.EXE
                If gsDest.strRACMGR32 <> "" Then
                    CreateOSLink frmSetup1, gsDest.strRACMGR32, "", ResolveResString(resRACMGR32ICON), False
                End If
                If gsDest.strAUTMGR32 <> "" Then
                    CreateOSLink frmSetup1, gsDest.strAUTMGR32, "", ResolveResString(resAUTMGR32ICON), False
                End If
            End If
            mstrLastCreatedShellGroup = "" 'Do not use this same group for other icons
        #End If
    
    
        '
        'Create program groups and icons (or links, i.e. shortcuts)
        '
        
        ShowStaticMessageDialog ResolveResString(resPROGMAN)
        
        Const fDefCreateGroupUnderWin95 = False
        'If fDefCreateGroupUnderWin95 is set to False (this is the default), then no
        'program group will be created under Win95 unless it is absolutely necessary.
        '
        'By default under Windows 95, no group should be created, and the
        'single program icon should be placed directly under the
        'Start>Programs menu (unless there are other, user-defined icons to create
        
        Dim fAdditionalIcons As Boolean
        'There are two default reasons why we will install more icons than
        'simply the program executable:
        '  1) If we are installing user-defined icons (by customizing this procedure)
        '  2) If we are creating an program removal icon (whenever we're running under NT)
        '
        'If you have modified this procedure to install more icons, make sure you set
        'this variable to True, so that a program group will be created for the icons.
        fAdditionalIcons = False
        #If Win32 Then
            fAdditionalIcons = fAdditionalIcons Or (Not TreatAsWin95())
        #Else
            fAdditionalIcons = fAdditionalIcons
        #End If
        
        'The following variable determines whether or not we create a program
        'group for icons.  It is controlled by fNoGroupUnderWin95,
        'fAdditionalIcons, and FTreatAsWin95().
        Dim fCreateGroup As Boolean
    #If Win32 Then
        If TreatAsWin95() Then
            'Win95 only:
            'We create a program group only if we have additional icons besides
            'the application executable (if any), or if fDefCreateGroupUnderWin95
            'has been set to True to override this default behavior.
    #Else
        If False Then
    #End If
            fCreateGroup = fDefCreateGroupUnderWin95 Or fAdditionalIcons
        Else
            #If Win32 Then
                'Win32 NT only:
                'We must always create a Program Manager group
                'because we always create an icon for the application removal program.
                fCreateGroup = True
            #Else
                'Win16 only:
                'If we are deploying this project as an application (as opposed to simply
                '  a shared component), then we create a group and icon(s), or
                '  if any additional icons need to be created.
                fCreateGroup = (gstrAppExe <> "") Or fAdditionalIcons
            #End If
        End If
        
        If fCreateGroup Then
            strGroupName = "IBM Multimedia"
            'strGroupName = gstrAppName
            If gstrAppExe = "" Then
                strGrpPath = Left$(gstrAppName, InStr(gstrAppName, gstrSEP_EXT)) & strEXT_GRP
            Else
                strGrpPath = Left$(gstrAppExe, InStr(gstrAppExe, gstrSEP_EXT)) & strEXT_GRP
            End If
            
            '
            'Go ahead and create the main program group
            '
            CreateOSProgramGroup frmSetup1, strGroupName, strGrpPath
            fMainGroupWasCreated = True
        End If
        
        If gstrAppExe <> "" Then
            '
            'Create the main application icon or link
            '
            CreateOSLink frmSetup1, gsDest.strAppDir & gstrAppExe, "", gstrAppName
            CreateOSLink frmSetup1, gsDest.strAppDir & "JBREAD.WRI", "", "Jungle Book Read Me"
            CreateOSLink frmSetup1, gsDest.strAppDir & "ICTHRESH.EXE", "", "Threshold Utility"
            CreateOSLink frmSetup1, gstrSrcPath & "PINOC_TR.AVI", "", "Pinocchio Preview"
        End If
            
        '
        'If you need to create more icons, insert code here, and make certain that you
        'have set the variable fAdditionalIcons to True above
        '
        'If Not fAdditionalIcons Then
        '  MsgError "Internal Setup Customization Error: fAdditionalIcons = False", vbOKOnly Or vbExclamation, gstrTitle
        '  ExitSetup Me, gintRET_FATAL
        'End If
        'CreateOSLink frmSetup1, gsDest.strAppDir & "My Exe 1.exe", "My Exe 1 command-line arguments", "My Exe 1"
        'CreateOSLink frmSetup1, gsDest.strAppDir & "My Exe 2.exe", "My Exe 2 command-line arguments", "My Exe 2"
        '
    
        '
        'Write all of the Program INI settings
        '
        intTemp = WritePrivateProfileString("Options", "Path", gsDest.strAppDir, gstrIniFile)
        intTemp = WritePrivateProfileString("Options", "Content", gstrSrcPath, gstrIniFile)
    
        '
        'Set all of the Registration Database Entry Crap
        '
        '#If Win32 Then
        '    If RegCreateKey(HKEY_LOCAL_MACHINE, "\Software\IBM\ICSS", "", hkey) Then
        '        RegSetStringValue hkey, "InstallDir", gsDest.strAppDir, False
        '        RegCloseKey hkey
        '    End If
        '#End If
        
        '
        'Register the per-app path for 32-bit applications
        '
        #If Win32 And LOGGING Then
            If gstrAppExe <> "" Then
                Dim strPerAppPath As String
                strPerAppPath = ReadIniFile(gstrSetupInfoFile, gstrINI_SETUP, gstrINI_APPPATH)
                AddPerAppPath gstrAppExe, gsDest.strAppDir, strPerAppPath
            End If
        #End If
    End If
    

ExitSetup:
    HideStaticMessageDialog
    RestoreProgMan
    #If Win32 And LOGGING Then
        If fWithinAction() Then
            'By now, all logging actions should have been either aborted or committed.
            MsgError ResolveResString(resSTILLWITHINACTION), vbExclamation Or vbOKOnly, gstrTitle
            ExitSetup Me, gintRET_FATAL
        End If
        MoveAppRemovalFiles
    #End If
    ExitSetup Me, gintRET_FINISHEDSUCCESS

MainError:
    Select Case MsgError(Error$ & LS$ & ResolveResString(resUNEXPECTED), MB_RETRYCANCEL Or MB_ICONEXCLAMATION, gstrTitle)
    Case IDRETRY
        Resume
    Case IDCANCEL
        ExitSetup Me, gintRET_ABORT
        Resume
    End Select
End Sub

'-----------------------------------------------------------
' SUB: HideCopyDialog
'
' Unloads the copy files status form
'-----------------------------------------------------------
'
Private Sub HideCopyDialog()
    Unload frmCopy
End Sub

'-----------------------------------------------------------
' SUB: HideStaticMessageDialog
'
' Unloads the setup messages form
'-----------------------------------------------------------
'
Private Sub HideStaticMessageDialog()
    Unload frmMessage
End Sub

'-----------------------------------------------------------
' SUB: ShowBeginForm
'
' Displays the begin setup form
'-----------------------------------------------------------
'
Private Sub ShowBeginForm()
    frmBegin.Show 1
End Sub

'-----------------------------------------------------------
' SUB: ShowCopyDialog
'
' Displays the copy files status form
'-----------------------------------------------------------
'
Private Sub ShowCopyDialog()
    CenterForm frmCopy
    frmCopy.Show
    frmCopy.Refresh
    frmCopy.cmdExit.SetFocus
End Sub

'-----------------------------------------------------------
' SUB: ShowMainForm
'
' Displays the main setup 'blue wash' form
'-----------------------------------------------------------
'
Private Sub ShowMainForm()
    Me.Caption = gstrTitle
    Me.Show
    DrawBackGround
    Me.Refresh
End Sub

'-----------------------------------------------------------
' SUB: ShowStaticMessageDialog
'
' Displays a setup message in a 'box' of the appropriate
' size for the message
'
' IN: [strMessage] - message to display
'-----------------------------------------------------------
'
Private Sub ShowStaticMessageDialog(ByVal strMessage As String)
    Dim frm As Form

    Set frm = frmMessage
    frm.lblMsg.Caption = strMessage

    '
    'Default height is twice the height of the setup icon.
    'If the height of the message text is greater, then
    'increase the form height to the label height plus
    'half an icon height
    '
    frm.ScaleHeight = frm.imgMsg.Height * 2
    If frm.lblMsg.Height > frm.ScaleHeight Then
        frm.ScaleHeight = frm.lblMsg.Height + frm.imgMsg.Height * 0.5
    End If

    '
    'Vertically center the icon and label within the form
    '
    frm.imgMsg.Top = frm.ScaleHeight / 2 - frm.imgMsg.Height / 2
    frm.lblMsg.Top = frm.ScaleHeight / 2 - frm.lblMsg.Height / 2

    CenterForm frm

    frm.Show
    frm.Refresh
End Sub

'-----------------------------------------------------------
' SUB: ShowWelcomeForm
'
' Displays the welcome to setup form
'-----------------------------------------------------------
'
Private Sub ShowWelcomeForm()
    frmWelcome.Show 1
End Sub

'-----------------------------------------------------------
' SUB: GetStrings
'
' Loads string resources into global vars and forms/controls
'-----------------------------------------------------------
'
Private Sub GetStrings()
    On Error GoTo GSErr
    
    gstrSETMSG = ResolveResString(resSETMSG)
    
    Exit Sub
    
GSErr:
    MsgError mstrRESOURCELOADFAIL, MB_ICONSTOP Or MB_OK, gstrNULL
    ExitSetup Me, gintRET_FATAL
End Sub

