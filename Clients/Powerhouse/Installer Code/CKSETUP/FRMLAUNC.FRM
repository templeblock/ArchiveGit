VERSION 4.00
Begin VB.Form frmLaunch 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Play Chess Kids"
   ClientHeight    =   4155
   ClientLeft      =   2805
   ClientTop       =   1620
   ClientWidth     =   5040
   Height          =   4530
   Icon            =   "FRMLAUNC.frx":0000
   Left            =   2745
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4155
   ScaleWidth      =   5040
   ShowInTaskbar   =   0   'False
   Top             =   1305
   Width           =   5160
   Begin VB.Timer LongTimer1 
      Enabled         =   0   'False
      Interval        =   2500
      Left            =   60
      Top             =   2940
   End
   Begin VB.PictureBox Picture1 
      Height          =   3405
      Index           =   3
      Left            =   0
      Picture         =   "FRMLAUNC.frx":0442
      ScaleHeight     =   3345
      ScaleWidth      =   4980
      TabIndex        =   6
      Top             =   0
      Visible         =   0   'False
      Width           =   5040
   End
   Begin VB.PictureBox Picture1 
      Height          =   3405
      Index           =   2
      Left            =   0
      Picture         =   "FRMLAUNC.frx":129B8
      ScaleHeight     =   3345
      ScaleWidth      =   4980
      TabIndex        =   5
      Top             =   0
      Visible         =   0   'False
      Width           =   5040
   End
   Begin VB.PictureBox Picture1 
      Height          =   3405
      Index           =   1
      Left            =   0
      Picture         =   "FRMLAUNC.frx":24F2E
      ScaleHeight     =   3345
      ScaleWidth      =   4980
      TabIndex        =   4
      Top             =   0
      Visible         =   0   'False
      Width           =   5040
   End
   Begin VB.CommandButton Command1 
      Caption         =   "C&redits..."
      Height          =   435
      Left            =   1740
      TabIndex        =   1
      Top             =   3600
      Width           =   1515
   End
   Begin VB.PictureBox Picture1 
      Height          =   3405
      Index           =   0
      Left            =   0
      Picture         =   "FRMLAUNC.frx":374A4
      ScaleHeight     =   223
      ScaleMode       =   3  'Pixel
      ScaleWidth      =   332
      TabIndex        =   3
      Top             =   0
      Width           =   5040
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "&Cancel"
      Height          =   435
      Left            =   3390
      TabIndex        =   2
      Top             =   3600
      Width           =   1515
   End
   Begin VB.CommandButton cmdLaunchApp 
      Caption         =   "&Play Chess Kids..."
      Height          =   435
      Left            =   90
      TabIndex        =   0
      Top             =   3600
      Width           =   1515
   End
End
Attribute VB_Name = "frmLaunch"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
Dim CreditMode As Integer
Private Sub Command1_Click()
    Command1.Enabled = False
    CreditMode = 1
    Picture1(CreditMode).Top = Picture1(CreditMode).Top - Picture1(CreditMode).Height
    Picture1(CreditMode).Visible = True
    Do Until Picture1(CreditMode).Top = 0
        Picture1(CreditMode).Top = Picture1(CreditMode).Top + 1
    Loop
    LongTimer1.Enabled = True
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
    Dim Msg As String
    Dim Style As Integer
    Dim Title As String
    Dim response As Integer
    
    intTemp = GetPrivateProfileString("Program", "Path", "", ReadPath, Len(ReadPath), gstrIniFile)
    strTemp = Left$(ReadPath, intTemp)
    
    If IsWin32 Then
        If IsCDROMLoaded(strTemp + "CHESSKID.EXE") Then
            ChDir strTemp
            intTemp = Shell(strTemp + "CHESSKID.EXE", 1)
            ExitSetup Me, gintRET_FINISHEDSUCCESSNOMESSAGE
        Else
            Msg = "Please insert the Chess Kids CD-ROM in drive " + Left$(strTemp, 2) + " and try again."  ' Define message.
            Style = vbOKOnly + vbExclamation    ' Define buttons.
            Title = "Error"             ' Define title.
            
            response = MsgBox(Msg, Style, Title) ' Display message.
        End If
    Else
        If IsWin32sLoaded() Then
            If IsCDROMLoaded(strTemp + "CHESSKID.EXE") Then
                ChDir strTemp
                intTemp = Shell(strTemp + "CHESSKID.EXE", 1)
                ExitSetup Me, gintRET_FINISHEDSUCCESSNOMESSAGE
            Else
                Msg = "Please insert the Chess Kids CD-ROM in drive " + Left$(strTemp, 2) + " and try again."  ' Define message.
                Style = vbOKOnly + vbExclamation    ' Define buttons.
                Title = "Error"             ' Define title.
                
                response = MsgBox(Msg, Style, Title) ' Display message.
            End If
        Else
            ExitSetup Me, gintRET_FINISHEDINSTALLWIN32S
        End If
    End If
    
End Sub


Private Sub Form_Load()

    'center it on the screen
    Me.Top = (Screen.Height - Me.Height) \ 2
    Me.Left = (Screen.Width - Me.Width) \ 2
    
    CreditMode = 0

End Sub



Private Sub LongTimer1_Timer()
    CreditMode = CreditMode + 1
    If CreditMode = 4 Then
        CreditMode = 0
        Picture1(1).Visible = False
        Picture1(2).Visible = False
        Picture1(3).Visible = False
        Command1.Enabled = True
        LongTimer1.Enabled = False
        Exit Sub
    End If
    Picture1(CreditMode).Top = Picture1(CreditMode).Top - Picture1(CreditMode).Height
    Picture1(CreditMode).Visible = True
    Do Until Picture1(CreditMode).Top = 0
        Picture1(CreditMode).Top = Picture1(CreditMode).Top + 1
    Loop
End Sub


