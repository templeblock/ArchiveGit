VERSION 4.00
Begin VB.Form frmInstallApp 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Install The Adventures of Pinocchio"
   ClientHeight    =   4095
   ClientLeft      =   1050
   ClientTop       =   1335
   ClientWidth     =   5715
   Height          =   4455
   Icon            =   "FRMINSTA.frx":0000
   Left            =   990
   LinkTopic       =   "Form1"
   ScaleHeight     =   4095
   ScaleWidth      =   5715
   ShowInTaskbar   =   0   'False
   Top             =   1035
   Width           =   5835
   Begin VB.PictureBox Picture1 
      Height          =   2655
      Left            =   60
      ScaleHeight     =   2595
      ScaleWidth      =   3135
      TabIndex        =   8
      Top             =   60
      Width           =   3195
      Begin VB.PictureBox Picture2 
         AutoSize        =   -1  'True
         BorderStyle     =   0  'None
         Height          =   2550
         Left            =   30
         Picture         =   "FRMINSTA.frx":0442
         ScaleHeight     =   170
         ScaleMode       =   3  'Pixel
         ScaleWidth      =   205
         TabIndex        =   9
         Top             =   30
         Width           =   3075
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Installation Directory:"
      Height          =   675
      Left            =   60
      TabIndex        =   4
      Top             =   2760
      Width           =   5595
      Begin VB.CommandButton cmdChangeDir 
         Caption         =   "Change Directory..."
         Height          =   435
         Left            =   3720
         TabIndex        =   5
         Top             =   170
         Width           =   1815
      End
      Begin VB.Label lblDestDir 
         Caption         =   "Label1"
         Height          =   195
         Left            =   120
         TabIndex        =   6
         Top             =   285
         Width           =   3555
      End
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   435
      Left            =   2940
      TabIndex        =   3
      Top             =   3540
      Width           =   2715
   End
   Begin VB.CommandButton cmdInstallVFW 
      Caption         =   "Install Intel Indeo®..."
      Height          =   435
      Left            =   3600
      TabIndex        =   1
      Top             =   720
      Width           =   1815
   End
   Begin VB.CommandButton cmdInstallWin32s 
      Caption         =   "Install Win32s..."
      Height          =   435
      Left            =   3600
      TabIndex        =   0
      Top             =   1620
      Width           =   1815
   End
   Begin VB.CommandButton cmdInstallApp 
      Caption         =   "Install The Adventures of Pinocchio"
      Enabled         =   0   'False
      Height          =   435
      Left            =   60
      TabIndex        =   2
      Top             =   3540
      Width           =   2775
   End
   Begin VB.Label lblDDE 
      AutoSize        =   -1  'True
      BorderStyle     =   1  'Fixed Single
      Caption         =   "This label is used for DDE connection to the Program Manager"
      Height          =   255
      Left            =   600
      TabIndex        =   7
      Top             =   3420
      Visible         =   0   'False
      Width           =   4485
   End
End
Attribute VB_Name = "frmInstallApp"
Attribute VB_Creatable = False
Attribute VB_Exposed = False

Sub CheckInstalledStuff()
    Dim intTemp1 As Integer
    Dim intTemp2 As Integer
    
    intTemp1 = False
    intTemp2 = False
    
    If IsWin32 Then
        cmdInstallWin32s.Enabled = False
        intTemp1 = True
    Else
        If IsWin32sLoaded() Then
            cmdInstallWin32s.Enabled = False
            intTemp1 = True
        End If
    End If
    
    If IsWin32 Then
        If DoesFileExist(gstrWinSysDir & "IR32_32.DLL") Then
            cmdInstallVFW.Enabled = False
            intTemp2 = True
        End If
    Else
        If DoesFileExist(gstrWinSysDir & "IR32.DLL") Then
            cmdInstallVFW.Enabled = False
            intTemp2 = True
        End If
    End If
    
    If intTemp1 = True And intTemp2 = True Then
        cmdChangeDir.Enabled = True
        cmdInstallApp.Enabled = True
    End If
End Sub


Private Sub cmdCancel_Click()
    ExitSetup Me, gintRET_EXIT
End Sub


Private Sub cmdChangeDir_Click()
    ShowPathDialog gstrDIR_DEST

    If gfRetVal = gintRET_CONT Then
        lblDestDir.Caption = gstrDestDir
    End If
End Sub

Private Sub cmdInstallApp_Click()
    
    Unload Me
    
End Sub

Private Sub cmdInstallVFW_Click()
    Dim RetVal As Integer
    Dim strTemp As String
    Dim intTemp As Integer
    Dim ReadPath As String * 100
    
    Call CheckInstalledStuff
    
    If cmdInstallVFW.Enabled = True Then
        If IsWin32 Then
            RetVal = Shell(gstrSrcPath + "INDEO4\CODEC32\SETUPA.EXE", 1)
        Else
            If Shell(gstrSrcPath + "INDEO4\CODEC16\SETUPA.EXE", 1) Then
                intTemp = GetPrivateProfileString("Settings", "Startup", "", ReadPath, Len(ReadPath), "PROGMAN.INI")
                strTemp = Left$(ReadPath, intTemp)
                If strTemp = "" Then
                    strTemp = "StartUp"
                    intTemp = WritePrivateProfileString("Settings", "Startup", strTemp, "PROGMAN.INI")
                End If
                CreateOSProgramGroup frmInstallApp, strTemp, "STARTUP.GRP"
                CreateOSLink frmInstallApp, gstrSrcPath & "SETUP.EXE", "", "Adventures of Pinocchio Setup"
            End If
        End If
    End If
    
End Sub


Private Sub cmdInstallWin32s_Click()
    Dim RetVal As Integer
    Dim strTemp As String
    Dim intTemp As Integer
    Dim ReadPath As String * 100
    
    Call CheckInstalledStuff
    
    If cmdInstallWin32s.Enabled = True Then
        If Shell(gstrSrcPath + "WIN32S\SETUPA.EXE", 1) Then
            intTemp = GetPrivateProfileString("Settings", "Startup", "", ReadPath, Len(ReadPath), "PROGMAN.INI")
            strTemp = Left$(ReadPath, intTemp)
            If strTemp = "" Then
                strTemp = "StartUp"
                intTemp = WritePrivateProfileString("Settings", "Startup", strTemp, "PROGMAN.INI")
            End If
            CreateOSProgramGroup frmInstallApp, strTemp, "STARTUP.GRP"
            CreateOSLink frmInstallApp, gstrSrcPath & "SETUP.EXE", "", "Adventures of Pinocchio Setup"
        End If
    End If
    
End Sub




Private Sub Form_GotFocus()
    Call CheckInstalledStuff
End Sub

Private Sub Form_Load()

    'center it on the screen
    Me.Top = (Screen.Height - Me.Height) \ 2
    Me.Left = (Screen.Width - Me.Width) \ 2
    
    Picture2.Top = (Picture1.Height - Picture2.Height) \ 2
    Picture2.Left = (Picture1.Width - Picture2.Width) \ 2
    
    lblDestDir.Caption = gstrDestDir
    
    Call CheckInstalledStuff

End Sub


