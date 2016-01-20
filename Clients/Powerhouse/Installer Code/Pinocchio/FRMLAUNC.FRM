VERSION 4.00
Begin VB.Form frmLaunch 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Start The Adventures of Pinocchio"
   ClientHeight    =   5820
   ClientLeft      =   1050
   ClientTop       =   1320
   ClientWidth     =   6435
   Height          =   6180
   Icon            =   "FRMLAUNC.frx":0000
   Left            =   990
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5820
   ScaleWidth      =   6435
   ShowInTaskbar   =   0   'False
   Top             =   1020
   Width           =   6555
   Begin VB.PictureBox Picture1 
      Height          =   5115
      Left            =   60
      ScaleHeight     =   5055
      ScaleWidth      =   6255
      TabIndex        =   2
      Top             =   60
      Width           =   6315
      Begin VB.PictureBox Picture2 
         AutoSize        =   -1  'True
         Height          =   5010
         Left            =   30
         Picture         =   "FRMLAUNC.frx":030A
         ScaleHeight     =   330
         ScaleMode       =   3  'Pixel
         ScaleWidth      =   410
         TabIndex        =   3
         Top             =   30
         Width           =   6210
      End
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   435
      Left            =   3360
      TabIndex        =   1
      Top             =   5280
      Width           =   3015
   End
   Begin VB.CommandButton cmdLaunchApp 
      Caption         =   "Start The Adventures of Pinocchio"
      Height          =   435
      Left            =   60
      TabIndex        =   0
      Top             =   5280
      Width           =   3075
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
    
    Picture2.Top = (Picture1.Height - Picture2.Height) \ 2
    Picture2.Left = (Picture1.Width - Picture2.Width) \ 2

End Sub


