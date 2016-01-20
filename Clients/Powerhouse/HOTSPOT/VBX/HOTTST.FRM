VERSION 2.00
Begin Form Form1 
   BackColor       =   &H00FF0000&
   Caption         =   "Form1"
   ClientHeight    =   4380
   ClientLeft      =   570
   ClientTop       =   1440
   ClientWidth     =   7170
   Height          =   4785
   Left            =   510
   LinkTopic       =   "Form1"
   ScaleHeight     =   4380
   ScaleWidth      =   7170
   Top             =   1095
   Width           =   7290
   Begin CheckBox ShowHotCheck 
      BackColor       =   &H00FF0000&
      Caption         =   "Visible"
      ForeColor       =   &H00FFFFFF&
      Height          =   375
      Index           =   1
      Left            =   3600
      TabIndex        =   12
      Top             =   3360
      Width           =   1215
   End
   Begin CheckBox ShowHotCheck 
      BackColor       =   &H00FF0000&
      Caption         =   "Invisible"
      ForeColor       =   &H00FFFFFF&
      Height          =   375
      Index           =   0
      Left            =   2040
      TabIndex        =   11
      Top             =   3360
      Width           =   1455
   End
   Begin CommandButton Command4 
      Caption         =   "Close"
      Height          =   495
      Left            =   5280
      TabIndex        =   8
      Top             =   3120
      Width           =   1455
   End
   Begin TextBox SeekText 
      Height          =   285
      Left            =   5160
      TabIndex        =   6
      Text            =   "0"
      Top             =   3960
      Width           =   1815
   End
   Begin OptionButton AviOption 
      BackColor       =   &H00FF0000&
      Caption         =   "Animation"
      ForeColor       =   &H00FFFFFF&
      Height          =   255
      Index           =   1
      Left            =   5280
      TabIndex        =   5
      Top             =   480
      Width           =   1455
   End
   Begin OptionButton AviOption 
      BackColor       =   &H00FF0000&
      Caption         =   "Surfer"
      ForeColor       =   &H00FFFFFF&
      Height          =   255
      Index           =   0
      Left            =   5280
      TabIndex        =   4
      Top             =   840
      Width           =   1455
   End
   Begin CommandButton Command3 
      Caption         =   "Reverse"
      Height          =   495
      Left            =   5280
      TabIndex        =   1
      Top             =   2520
      Width           =   1455
   End
   Begin CommandButton Command2 
      Caption         =   "Pause"
      Height          =   495
      Left            =   5280
      TabIndex        =   3
      Top             =   1920
      Width           =   1455
   End
   Begin CommandButton Command1 
      Caption         =   "Play"
      Height          =   495
      Left            =   5280
      TabIndex        =   2
      Top             =   1320
      Width           =   1455
   End
   Begin AVIHOT AVIHOT1 
      AVIFile         =   ""
      Height          =   735
      HOTFile         =   ""
      Left            =   240
      Top             =   600
      Width           =   855
   End
   Begin Frame Frame1 
      BackColor       =   &H00FF0000&
      Caption         =   "Hotspots"
      ForeColor       =   &H00FFFFFF&
      Height          =   735
      Left            =   1680
      TabIndex        =   9
      Top             =   3120
      Width           =   3375
   End
   Begin Frame Frame2 
      BackColor       =   &H00FF0000&
      Caption         =   "File"
      ForeColor       =   &H00FFFFFF&
      Height          =   975
      Left            =   5040
      TabIndex        =   10
      Top             =   240
      Width           =   1935
   End
   Begin Label Label2 
      BackStyle       =   0  'Transparent
      Caption         =   "Seek"
      ForeColor       =   &H00FFFFFF&
      Height          =   255
      Left            =   5280
      TabIndex        =   7
      Top             =   3720
      Width           =   1455
   End
   Begin Label Label1 
      BackColor       =   &H00FF0000&
      Caption         =   "Label1"
      FontBold        =   -1  'True
      FontItalic      =   0   'False
      FontName        =   "Times New Roman"
      FontSize        =   18
      FontStrikethru  =   0   'False
      FontUnderline   =   0   'False
      ForeColor       =   &H00FFFFFF&
      Height          =   375
      Left            =   240
      TabIndex        =   0
      Top             =   120
      Width           =   4215
   End
End
Option Explicit

Dim visflag As Integer

Sub AVIHOT1_AVIDone ()
    'DoEvents
    'AviHot1.AVIFile = "c:\hotspot\winser\animat.avi"
    'AviHot1.HOTFile = "c:\hotspot\winser\animat.hot"
    'DoEvents
    'End
    AviOption(0).Value = False
    AviOption(1).Value = False
End Sub

Sub AVIHOT1_HotspotClick (hotspotid As String)
    Label1.Caption = hotspotid
End Sub

Sub AviOption_Click (index As Integer)
    If index = 0 Then
    avihot1.AVIFile = "c:\winvideo\sample.avi"
    avihot1.HOTFile = "c:\winvideo\sample.hot"
    Label1.Caption = "Sample " + Mid$(Str$(avihot1.Length), 2)
    End If
    If index = 1 Then
    avihot1.AVIFile = "c:\hotspot\winser\animat.avi"
    avihot1.HOTFile = "c:\hotspot\winser\animat.hot"
    Label1.Caption = "Animation " + Mid$(Str$(avihot1.Length), 2)
    End If
    SeekText.Text = Mid$(Str$(avihot1.Frame), 2)
    avihot1.ShowHot = visflag
End Sub

Sub Command1_Click ()
    avihot1.Play = 1
    SeekText.Text = Mid$(Str$(avihot1.Frame), 2)
End Sub

Sub Command2_Click ()
    avihot1.Play = 0
    SeekText.Text = Mid$(Str$(avihot1.Frame), 2)
End Sub

Sub Command3_Click ()
    avihot1.Play = 2
    SeekText.Text = Mid$(Str$(avihot1.Frame), 2)
End Sub

Sub Command4_Click ()
    AviOption(0).Value = False
    AviOption(1).Value = False
    avihot1.Play = 3
    SeekText.Text = Mid$(Str$(avihot1.Frame), 2)
End Sub

Sub Form_DblClick ()
    avihot1.Play = 1
    Label1.Caption = "now playing"
End Sub

Sub Form_Load ()
    Label1.Caption = ""
    ShowHotCheck(0).Value = 1: visflag = 0
End Sub

Sub SeekText_KeyPress (KeyAscii As Integer)
    If (KeyAscii = 13) Then
        avihot1.Frame = Val(SeekText.Text)
    End If

End Sub

Sub ShowHotCheck_Click (index As Integer)
Dim x As Integer

    visflag = index
    x = 1 - index
    ShowHotCheck(x).Value = 0
    ShowHotCheck(visflag).Value = 1
    avihot1.ShowHot = visflag
End Sub

