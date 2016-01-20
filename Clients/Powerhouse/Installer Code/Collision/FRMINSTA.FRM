VERSION 4.00
Begin VB.Form frmInstallApp 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Install Collision"
   ClientHeight    =   5235
   ClientLeft      =   2550
   ClientTop       =   1830
   ClientWidth     =   5955
   Height          =   5610
   Icon            =   "FRMINSTA.frx":0000
   Left            =   2490
   LinkTopic       =   "Form1"
   ScaleHeight     =   5235
   ScaleWidth      =   5955
   ShowInTaskbar   =   0   'False
   Top             =   1515
   Width           =   6075
   Begin VB.Frame Frame1 
      Caption         =   "Installation Directory:"
      Height          =   675
      Left            =   60
      TabIndex        =   4
      Top             =   3720
      Width           =   5835
      Begin VB.CommandButton cmdChangeDir 
         Caption         =   "Change Directory..."
         Height          =   435
         Left            =   3960
         TabIndex        =   5
         Top             =   165
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
   Begin VB.PictureBox Picture1 
      Height          =   3630
      Left            =   60
      Picture         =   "FRMINSTA.frx":0442
      ScaleHeight     =   238
      ScaleMode       =   3  'Pixel
      ScaleWidth      =   241
      TabIndex        =   3
      Top             =   60
      Width           =   3675
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   435
      Left            =   3060
      TabIndex        =   2
      Top             =   4620
      Width           =   1815
   End
   Begin VB.CommandButton cmdInstallVFW 
      Caption         =   "Install Indeo 4.1..."
      Height          =   435
      Left            =   3960
      TabIndex        =   0
      Top             =   1620
      Width           =   1815
   End
   Begin VB.CommandButton cmdInstallApp 
      Caption         =   "Install Collision..."
      Enabled         =   0   'False
      Height          =   435
      Left            =   1080
      TabIndex        =   1
      Top             =   4620
      Width           =   1815
   End
   Begin VB.Label lblDDE 
      AutoSize        =   -1  'True
      BorderStyle     =   1  'Fixed Single
      Caption         =   "This label is used for DDE connection to the Program Manager"
      Height          =   255
      Left            =   720
      TabIndex        =   7
      Top             =   4380
      Visible         =   0   'False
      Width           =   4485
   End
End
Attribute VB_Name = "frmInstallApp"
Attribute VB_Creatable = False
Attribute VB_Exposed = False

Sub CheckInstalledStuff()
    If IsWin32 Then
        If DoesFileExist(gstrWinSysDir & "IR32_32.DLL") Then
            cmdInstallVFW.Enabled = False
            cmdChangeDir.Enabled = True
            cmdInstallApp.Enabled = True
        Else
            cmdInstallVFW.Enabled = True
            cmdChangeDir.Enabled = False
            cmdInstallApp.Enabled = False
        End If
    Else
        If DoesFileExist(gstrWinSysDir & "IR32.DLL") Then
            cmdInstallVFW.Enabled = False
            cmdChangeDir.Enabled = True
            cmdInstallApp.Enabled = True
        Else
            cmdInstallVFW.Enabled = True
            cmdChangeDir.Enabled = False
            cmdInstallApp.Enabled = False
        End If
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
    
    If cmdInstallVFW.Enabled = True Then
        If IsWin32 Then
            RetVal = Shell(gstrSrcPath + "INDEO4\CODEC32\SETUPA.EXE", 1)
        Else
            If Shell(gstrSrcPath + "INDEO4\CODEC16\SETUPA.EXE", 1) Then
                intTemp = GetPrivateProfileString("Settings", "Startup", "", ReadPath, Len(ReadPath), "PROGMAN.INI")
                strTemp = Left$(ReadPath, intTemp)
                If strTemp = "" Then
                    strTemp = "Startup"
                    intTemp = WritePrivateProfileString("Options", "Startup", strTemp, "PROGMAN.INI")
                End If
                CreateOSProgramGroup frmInstallApp, strTemp, "STARTUP.GRP"
                CreateOSLink frmInstallApp, gstrSrcPath & "SETUP.EXE", "", "Collision Setup"
            End If
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
    
    lblDestDir.Caption = gstrDestDir
    
    Call CheckInstalledStuff

End Sub


