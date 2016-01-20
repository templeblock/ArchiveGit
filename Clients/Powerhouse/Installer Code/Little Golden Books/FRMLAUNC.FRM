VERSION 4.00
Begin VB.Form frmLaunch 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Start AppName"
   ClientHeight    =   4500
   ClientLeft      =   1065
   ClientTop       =   1320
   ClientWidth     =   4845
   Height          =   4860
   Icon            =   "FRMLAUNC.frx":0000
   Left            =   1005
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4500
   ScaleWidth      =   4845
   ShowInTaskbar   =   0   'False
   Top             =   1020
   Width           =   4965
   Begin VB.PictureBox Picture1 
      Height          =   3660
      Index           =   0
      Left            =   0
      Picture         =   "FRMLAUNC.frx":030A
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
      Top             =   3900
      Width           =   2175
   End
   Begin VB.CommandButton cmdLaunchApp 
      Caption         =   "&Start AppName..."
      Height          =   495
      Left            =   120
      TabIndex        =   0
      Top             =   3900
      Width           =   2175
   End
End
Attribute VB_Name = "frmLaunch"
Attribute VB_Creatable = False
Attribute VB_Exposed = False

Private Sub cmdCancel_Click()
    ExitSetup Me, gintRET_FINISHEDSUCCESSNOMESSAGE
End Sub

Private Sub cmdLaunchApp_Click()
    
    Dim strTemp As String
    Dim intTemp As Integer
    Dim ReadPath As String * 100
    Dim Msg As String
    Dim Style As Integer
    Dim Title As String
    Dim response As Integer
    
    intTemp = GetPrivateProfileString("Programs", gstrAppName, "", ReadPath, Len(ReadPath), gstrIniFile)
    strTemp = Left$(ReadPath, intTemp)
    
    If IsCDROMLoaded(strTemp) Then
        intTemp = Shell(strTemp, 1)
        ExitSetup Me, gintRET_FINISHEDSUCCESSNOMESSAGE
    Else
        Msg = "Please insert the " & gstrAppName & " CD-ROM in drive " + Left$(strTemp, 2) + " and try again."  ' Define message.
        Style = vbOKOnly + vbExclamation    ' Define buttons.
        Title = "Error"             ' Define title.
        
        response = MsgBox(Msg, Style, Title) ' Display message.
    End If
    
End Sub


Private Sub Form_Load()

    'center it on the screen
    Me.Top = (Screen.Height - Me.Height) \ 2
    Me.Left = (Screen.Width - Me.Width) \ 2
    
    Me.Caption = "Start " & gstrAppName
    cmdLaunchApp.Caption = "Start " & gstrAppName & "..."
    
End Sub




