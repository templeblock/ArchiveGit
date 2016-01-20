VERSION 4.00
Begin VB.Form frmLaunch 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Start The Jungle Book"
   ClientHeight    =   5895
   ClientLeft      =   2310
   ClientTop       =   1785
   ClientWidth     =   6435
   Height          =   6270
   Icon            =   "FRMLAUNC.frx":0000
   Left            =   2250
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5895
   ScaleWidth      =   6435
   ShowInTaskbar   =   0   'False
   Top             =   1470
   Width           =   6555
   Begin VB.PictureBox Picture1 
      Height          =   5235
      Left            =   60
      ScaleHeight     =   5175
      ScaleWidth      =   6255
      TabIndex        =   2
      Top             =   60
      Width           =   6315
      Begin VB.PictureBox Picture2 
         AutoSize        =   -1  'True
         Height          =   5160
         Left            =   0
         Picture         =   "FRMLAUNC.frx":030A
         ScaleHeight     =   340
         ScaleMode       =   3  'Pixel
         ScaleWidth      =   410
         TabIndex        =   3
         Top             =   0
         Width           =   6210
      End
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   435
      Left            =   3180
      TabIndex        =   1
      Top             =   5400
      Width           =   1815
   End
   Begin VB.CommandButton cmdLaunchApp 
      Caption         =   "Start The Jungle Book"
      Height          =   435
      Left            =   1260
      TabIndex        =   0
      Top             =   5400
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
    
    Picture2.Top = (Picture1.Height - Picture2.Height) \ 2
    Picture2.Left = (Picture1.Width - Picture2.Width) \ 2

End Sub


Private Sub Picture1_Click()

End Sub


