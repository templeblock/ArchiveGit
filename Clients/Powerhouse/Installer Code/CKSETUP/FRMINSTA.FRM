VERSION 4.00
Begin VB.Form frmInstallApp 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Install Chess Kids"
   ClientHeight    =   5445
   ClientLeft      =   2940
   ClientTop       =   1800
   ClientWidth     =   5040
   Height          =   5820
   Icon            =   "FRMINSTA.frx":0000
   Left            =   2880
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5445
   ScaleWidth      =   5040
   ShowInTaskbar   =   0   'False
   Top             =   1485
   Width           =   5160
   Begin VB.Frame Frame1 
      Caption         =   "Installation Directory:"
      Height          =   1215
      Left            =   60
      TabIndex        =   3
      Top             =   3540
      Width           =   4875
      Begin VB.CommandButton cmdChangeDir 
         Caption         =   "Change &Directory..."
         Height          =   435
         Left            =   1530
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
         Width           =   4635
      End
   End
   Begin VB.PictureBox Picture1 
      Height          =   3405
      Left            =   0
      Picture         =   "FRMINSTA.frx":0442
      ScaleHeight     =   223
      ScaleMode       =   3  'Pixel
      ScaleWidth      =   332
      TabIndex        =   2
      Top             =   0
      Width           =   5040
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "&Cancel"
      Height          =   435
      Left            =   2580
      TabIndex        =   1
      Top             =   4920
      Width           =   1815
   End
   Begin VB.CommandButton cmdInstallApp 
      Caption         =   "&Install Chess Kids..."
      Height          =   435
      Left            =   600
      TabIndex        =   0
      Top             =   4920
      Width           =   1815
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
    
    lblDestDir.Caption = gstrDestDir
    
    'Call CheckInstalledStuff

End Sub


