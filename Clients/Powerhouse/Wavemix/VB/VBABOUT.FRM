VERSION 2.00
Begin Form frmAbout 
   BackColor       =   &H00C0C0C0&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "About VBMix"
   ClientHeight    =   1995
   ClientLeft      =   1275
   ClientTop       =   3330
   ClientWidth     =   4770
   ControlBox      =   0   'False
   Height          =   2400
   Left            =   1215
   LinkTopic       =   "Form2"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   1995
   ScaleWidth      =   4770
   Top             =   2985
   Width           =   4890
   Begin CommandButton btnOK 
      Caption         =   "&Ok"
      Height          =   285
      Left            =   1920
      TabIndex        =   9
      Top             =   1620
      Width           =   1005
   End
   Begin Label lblWaveMixVersion 
      Alignment       =   2  'Center
      AutoSize        =   -1  'True
      BackColor       =   &H00C0C0C0&
      Caption         =   "WAVEMIX.DLL Version - X.XX"
      Height          =   195
      Left            =   1035
      TabIndex        =   10
      Top             =   1200
      Width           =   2580
   End
   Begin Line Line2 
      X1              =   120
      X2              =   4680
      Y1              =   1485
      Y2              =   1485
   End
   Begin Line Line1 
      X1              =   120
      X2              =   4680
      Y1              =   1080
      Y2              =   1080
   End
   Begin Label lblAboutVerMinor 
      AutoSize        =   -1  'True
      BackColor       =   &H00C0C0C0&
      Caption         =   "X"
      Height          =   195
      Left            =   2655
      TabIndex        =   8
      Top             =   360
      Width           =   165
   End
   Begin Label lblAbout4 
      AutoSize        =   -1  'True
      BackColor       =   &H00C0C0C0&
      Caption         =   "."
      Height          =   195
      Left            =   2565
      TabIndex        =   7
      Top             =   360
      Width           =   195
   End
   Begin Label lblAboutVerMajor 
      AutoSize        =   -1  'True
      BackColor       =   &H00C0C0C0&
      Caption         =   "X"
      Height          =   195
      Left            =   2430
      TabIndex        =   6
      Top             =   360
      Width           =   135
   End
   Begin Label lblAbout6 
      AutoSize        =   -1  'True
      BackColor       =   &H00C0C0C0&
      Caption         =   ", Microsoft Corporation."
      Height          =   195
      Left            =   2160
      TabIndex        =   5
      Top             =   840
      Width           =   2010
   End
   Begin Label lblAboutCopyrightYear 
      AutoSize        =   -1  'True
      BackColor       =   &H00C0C0C0&
      Caption         =   "XXXX"
      Height          =   195
      Left            =   1680
      TabIndex        =   4
      Top             =   840
      Width           =   495
   End
   Begin Label lblAbout5 
      AutoSize        =   -1  'True
      BackColor       =   &H00C0C0C0&
      Caption         =   "Copyright (c) "
      Height          =   195
      Left            =   480
      TabIndex        =   3
      Top             =   840
      Width           =   1155
   End
   Begin Label lblAbout3 
      AutoSize        =   -1  'True
      BackColor       =   &H00C0C0C0&
      Caption         =   "Version"
      Height          =   195
      Left            =   1710
      TabIndex        =   2
      Top             =   360
      Width           =   645
   End
   Begin Label lblAbout2 
      Alignment       =   2  'Center
      BackColor       =   &H00C0C0C0&
      Caption         =   "by Chris Grinstead"
      Height          =   255
      Left            =   360
      TabIndex        =   1
      Top             =   600
      Width           =   3855
   End
   Begin Label lblAbout1 
      Alignment       =   2  'Center
      AutoSize        =   -1  'True
      BackColor       =   &H00C0C0C0&
      Caption         =   "VBMix - Visual Basic WaveMix Test Application"
      Height          =   195
      Left            =   360
      TabIndex        =   0
      Top             =   120
      Width           =   4035
   End
End
'-----------------------------------------------------------------------
' VBABOUT.FRM - Visual Basic Wavemix Sample Code
'-----------------------------------------------------------------------

Sub btnOK_Click ()
Unload frmAbout
End Sub

Sub Form_Load ()
Dim lpWaveMixInfo  As WAVEMIXINFO

lpWaveMixInfo.wSize = Len(lpWaveMixInfo)
If (WaveMixGetInfo(lpWaveMixInfo)) Then
    MsgBox "WaveMixGetInfo failed", MB_ICONSTOP, APP_NAME
    End
Else
     lblWaveMixVersion.Caption = "WAVMIX16.DLL Version - " + Hex$(Asc(lpWaveMixInfo.bVersionMajor)) + "." + Hex$(Asc(lpWaveMixInfo.bVersionMinor))
End If

lblAboutVerMajor.Caption = VER_MAJOR
lblAboutVerMinor.Caption = VER_MINOR
lblAboutCopyrightYear.Caption = COPYRIGHT_YEAR
End Sub

