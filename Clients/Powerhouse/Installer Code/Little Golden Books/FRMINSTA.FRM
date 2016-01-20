VERSION 4.00
Begin VB.Form frmInstallApp 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Install AppName"
   ClientHeight    =   5700
   ClientLeft      =   1050
   ClientTop       =   1320
   ClientWidth     =   4875
   Height          =   6060
   Icon            =   "FRMINSTA.frx":0000
   Left            =   990
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5700
   ScaleWidth      =   4875
   ShowInTaskbar   =   0   'False
   Top             =   1020
   Width           =   4995
   Begin VB.Frame Frame1 
      Caption         =   "Installation Directory:"
      Height          =   1215
      Left            =   60
      TabIndex        =   3
      Top             =   3780
      Width           =   4755
      Begin VB.CommandButton cmdChangeDir 
         Caption         =   "Change &Directory..."
         Height          =   435
         Left            =   1440
         TabIndex        =   4
         Top             =   660
         Width           =   1815
      End
      Begin VB.Label lblDestDir 
         BorderStyle     =   1  'Fixed Single
         Caption         =   "Label1"
         Height          =   270
         Left            =   120
         TabIndex        =   5
         Top             =   285
         Width           =   4515
      End
   End
   Begin VB.PictureBox Picture1 
      Height          =   3660
      Left            =   0
      ScaleHeight     =   240
      ScaleMode       =   3  'Pixel
      ScaleWidth      =   320
      TabIndex        =   2
      Top             =   0
      Width           =   4860
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "&Cancel"
      Height          =   495
      Left            =   2520
      TabIndex        =   1
      Top             =   5100
      Width           =   2175
   End
   Begin VB.CommandButton cmdInstallApp 
      Caption         =   "&Install AppName..."
      Height          =   495
      Left            =   120
      TabIndex        =   0
      Top             =   5100
      Width           =   2175
   End
End
Attribute VB_Name = "frmInstallApp"
Attribute VB_Creatable = False
Attribute VB_Exposed = False


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




Private Sub Form_Load()

    'center it on the screen
    Me.Top = (Screen.Height - Me.Height) \ 2
    Me.Left = (Screen.Width - Me.Width) \ 2
    
    Me.Caption = "Install " & gstrAppName
    cmdInstallApp.Caption = "Install..."
    
    lblDestDir.Caption = gstrDestDir
    
    Picture1.Picture = LoadPicture(gstrSrcPath & "TITLE.BMP")
    
End Sub


Private Sub Form_QueryUnload(Cancel As Integer, UnloadMode As Integer)
    If UnloadMode <> 1 Then
        ExitSetup Me, gintRET_EXIT
        Cancel = 1
    End If
End Sub


