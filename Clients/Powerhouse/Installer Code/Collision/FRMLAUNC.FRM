VERSION 4.00
Begin VB.Form frmLaunch 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Start Collision"
   ClientHeight    =   6630
   ClientLeft      =   8220
   ClientTop       =   3120
   ClientWidth     =   5895
   Height          =   7005
   Icon            =   "FRMLAUNC.frx":0000
   Left            =   8160
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   6630
   ScaleWidth      =   5895
   ShowInTaskbar   =   0   'False
   Top             =   2805
   Width           =   6015
   Begin VB.PictureBox Picture1 
      Height          =   5835
      Left            =   60
      Picture         =   "FRMLAUNC.frx":030A
      ScaleHeight     =   385
      ScaleMode       =   3  'Pixel
      ScaleWidth      =   382
      TabIndex        =   2
      Top             =   60
      Width           =   5790
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   435
      Left            =   3060
      TabIndex        =   1
      Top             =   6060
      Width           =   1815
   End
   Begin VB.CommandButton cmdLaunchApp 
      Caption         =   "Start Collision..."
      Height          =   435
      Left            =   1080
      TabIndex        =   0
      Top             =   6060
      Width           =   1815
   End
End
Attribute VB_Name = "frmLaunch"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
Private Sub Command1_Click()
End Sub


Private Sub cmdCancel_Click()
    ExitSetup Me, gintRET_FINISHEDSUCCESSNOMESSAGE
End Sub


Private Sub cmdInstallVFW_Click()

    RetVal = Shell(App.Path + "\VFW\SETUP.EXE", 1)
    
End Sub


Private Sub cmdInstallWin32s_Click()

    RetVal = Shell(App.Path + "\WIN32S\SETUP.EXE", 1)
    
End Sub


Private Sub cmdLaunchApp_Click()
    
    Dim strTemp As String
    Dim intTemp As Integer
    Dim ReadPath As String * 100
    
    intTemp = GetPrivateProfileString("Options", "Path", "", ReadPath, Len(ReadPath), gstrIniFile)
    strTemp = Left$(ReadPath, intTemp)
    
    intTemp = Shell(strTemp + gstrAppExe, 1)
    ExitSetup Me, gintRET_FINISHEDSUCCESSNOMESSAGE
    
End Sub


Private Sub Form_Load()

    'center it on the screen
    Me.Top = (Screen.Height - Me.Height) \ 2
    Me.Left = (Screen.Width - Me.Width) \ 2

End Sub


