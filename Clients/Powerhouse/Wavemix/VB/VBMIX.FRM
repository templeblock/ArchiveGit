VERSION 2.00
Begin Form main 
   BackColor       =   &H00000000&
   BorderStyle     =   3  'Fixed Double
   Caption         =   "VBMix"
   ClientHeight    =   2970
   ClientLeft      =   5220
   ClientTop       =   7020
   ClientWidth     =   6195
   Height          =   3660
   Icon            =   VBMIX.FRX:0000
   Left            =   5160
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   ScaleHeight     =   2970
   ScaleWidth      =   6195
   Top             =   6390
   Width           =   6315
   Begin MsgBlaster MsgBlaster1 
      Prop8           =   "Click on ""..."" for the About Box ---->"
   End
   Begin Timer Timer1 
      Interval        =   500
      Left            =   5760
      Top             =   45
   End
   Begin Label lblInstructions 
      Alignment       =   2  'Center
      AutoSize        =   -1  'True
      BackColor       =   &H00000000&
      BackStyle       =   0  'Transparent
      Caption         =   "PRESS 1, 2, 3, 4 or click a mouse button"
      FontBold        =   -1  'True
      FontItalic      =   0   'False
      FontName        =   "Arial"
      FontSize        =   9.75
      FontStrikethru  =   0   'False
      FontUnderline   =   0   'False
      ForeColor       =   &H00FFFFFF&
      Height          =   240
      Left            =   135
      TabIndex        =   0
      Top             =   135
      Width           =   3795
   End
   Begin Menu mnuManual 
      Caption         =   "&Manual!"
   End
   Begin Menu mnuAuto 
      Caption         =   "&Auto!"
   End
   Begin Menu mnuAbout 
      Caption         =   "&About!"
   End
End
'-----------------------------------------------------------------------
' VBMIX.FRM - Visual Basic Wavemix Sample Code
'-----------------------------------------------------------------------
Option Explicit

Sub CloseWaveStuff ()
Dim rc As Integer

rc = WaveMixActivate(ghMixSession, WMIX_FALSE)

rc = WaveMixCloseChannel(ghMixSession, 0, WMIX_ALL)

If (glpMix1) Then
    rc = WaveMixFreeWave(ghMixSession, glpMix1)
End If
If (glpMix2) Then
    rc = WaveMixFreeWave(ghMixSession, glpMix2)
End If
If (glpMix3) Then
    rc = WaveMixFreeWave(ghMixSession, glpMix3)
End If
If (glpMix4) Then
    rc = WaveMixFreeWave(ghMixSession, glpMix4)
End If
If (glpMix5) Then
    rc = WaveMixFreeWave(ghMixSession, glpMix5)
End If
If (glpMix6) Then
    rc = WaveMixFreeWave(ghMixSession, glpMix6)
End If
If (glpMix7) Then
    rc = WaveMixFreeWave(ghMixSession, glpMix7)
End If

rc = WaveMixCloseSession(ghMixSession)

End

End Sub

Sub Form_Activate ()
Dim rc As Integer

rc = WaveMixActivate(ghMixSession, WMIX_TRUE)

End Sub

Sub Form_DblClick ()
Dim rc As Integer
Form_MouseDown 1, 0, 0, 0

End Sub

Sub Form_KeyPress (KeyAscii As Integer)
Dim rc As Integer

Select Case KeyAscii
   Case Asc("1"):
       rc = PlayWave(1, WMIX_CLEARQUEUE Or WMIX_HIPRIORITY)
   Case Asc("2"):
       rc = PlayWave(2, WMIX_CLEARQUEUE Or WMIX_HIPRIORITY)
   Case Asc("3"):
       rc = PlayWave(3, WMIX_CLEARQUEUE Or WMIX_HIPRIORITY)
   Case Asc("4"):
       rc = PlayWave(4, WMIX_CLEARQUEUE Or WMIX_HIPRIORITY)
End Select

End Sub

Sub Form_Load ()
Dim Config As MIXCONFIG
Dim fErr As Integer

'Use msgblast.vbx to allow us to process
'the WM_ACTIVATE message.
MsgBlaster1.hWndTarget = Main.hWnd
MsgBlaster1.MsgList(0) = WM_ACTIVATE
MsgBlaster1.MsgPassage(0) = PREPROCESS

'timer off to begin with, Manual mode is
'default at startup
Timer1.Enabled = False

'get ready to call WaveMixConfigInit ()
Config.wSize = Len(Config)
Config.dwFlags = WMIX_CONFIG_CHANNELS
Config.wChannels = 2

'establish a WaveMix session
ghMixSession = WaveMixConfigureInit(Config)
If (ghMixSession) <= 0 Then
    MsgBox "WaveMixConfigureInit Failed", MB_ICONSTOP, APP_NAME
    End
End If

'create globals to opened wave files
glpMix1 = WaveMixOpenWave(ghMixSession, "1.wav", 0&, 0)
glpMix2 = WaveMixOpenWave(ghMixSession, "2.wav", 0&, 0)
glpMix3 = WaveMixOpenWave(ghMixSession, "3.wav", 0&, 0)
glpMix4 = WaveMixOpenWave(ghMixSession, "4.wav", 0&, 0)
glpMix5 = WaveMixOpenWave(ghMixSession, "5.wav", 0&, 0)
glpMix6 = WaveMixOpenWave(ghMixSession, "6.wav", 0&, 0)
glpMix7 = WaveMixOpenWave(ghMixSession, "7.wav", 0&, 0)

'open output channels
fErr = WaveMixOpenChannel(ghMixSession, 8, WMIX_OPENCOUNT)

'bail if not successful
If (fErr <> 0) Or ((glpMix1 = 0) And (glpMix2 = 0) And (glpMix3 = 0) And (glpMix4 = 0) And (glpMix5 = 0) And (glpMix6 = 0) And (glpMix7 = 0)) Then
    CloseWaveStuff
End If

'center the app on screen
mnuManual.Visible = False
Me.Top = screen.Height / 2 - (Me.Height / 2)
Me.Left = screen.Width / 2 - (Me.Width / 2)
Me.Show 1

End Sub

Sub Form_MouseDown (Button As Integer, Shift As Integer, X As Single, Y As Single)
Dim rc As Integer

If Button = KEY_LBUTTON Then
rc = WaveMixFlushChannel(ghMixSession, 6, WMIX_NOREMIX)
rc = PlayWave(5, WMIX_CLEARQUEUE Or WMIX_HIPRIORITY)
End If

If Button = KEY_RBUTTON Then
rc = PlayWave(6, WMIX_CLEARQUEUE Or WMIX_HIPRIORITY Or WMIX_WAIT)
rc = PlayWave(7, WMIX_CLEARQUEUE Or WMIX_HIPRIORITY Or WMIX_WAIT)
rc = PlayWave(1, WMIX_CLEARQUEUE Or WMIX_HIPRIORITY Or WMIX_WAIT)
rc = PlayWave(3, WMIX_CLEARQUEUE Or WMIX_HIPRIORITY)
End If
End Sub

Sub Form_Unload (Cancel As Integer)

    CloseWaveStuff

End Sub

Sub lblInstructions_DblClick ()
Dim rc As Integer
Form_MouseDown 1, 0, 0, 0

End Sub

Sub lblInstructions_MouseDown (Button As Integer, Shift As Integer, X As Single, Y As Single)
Dim rc As Integer
Form_MouseDown Button, Shift, X, Y

End Sub

Sub mnuAbout_Click ()

'Load the About box as modal, centered on screen
Load frmAbout
frmAbout.Top = screen.Height / 2 - (frmAbout.Height / 2)
frmAbout.Left = screen.Width / 2 - (frmAbout.Width / 2)
frmAbout.Show 1

End Sub

Sub mnuAuto_Click ()
'change menu item to Manual
mnuAuto.Visible = False
mnuManual.Visible = True

'start the timer which plays waves at random
'until user clicks on Manual menu item
Timer1.Enabled = True

End Sub

Sub mnuManual_Click ()
Dim rc As Integer
'change the menu item to Auto
mnuAuto.Visible = True
mnuManual.Visible = False

' discontinue the random wave playing
Timer1.Enabled = False

'flush all channels to stop the waves currently playing
rc = WaveMixFlushChannel(ghMixSession, 0, WMIX_NOREMIX Or WMIX_ALL)
End Sub

Sub MsgBlaster1_Message (MsgVal As Integer, wParam As Integer, lParam As Long, ReturnVal As Long)
Dim rc As Integer
'Handle the WM_ACTIVATE message.
'Call WaveMixActivate (FALSE) to give up the wave device
'if we're being de-activated.
'otherwise, call WavemixActivate (TRUE)
Select Case MsgVal
    Case WM_ACTIVATE
        If wParam = WA_INACTIVE Then
            rc = WaveMixActivate(ghMixSession, WMIX_FALSE)
        Else
            rc = WaveMixActivate(ghMixSession, WMIX_TRUE)
        End If
End Select

End Sub

Function PlayWave (glpWave As Integer, dwFlags As Long) As Integer

Dim lpMixPlayParams As MIXPLAYPARAMS
Dim rc As Integer

Select Case glpWave

    Case 1:
    lpMixPlayParams.lpMixWave = glpMix1
    lpMixPlayParams.iChannel = 1
    lpMixPlayParams.wLoops = 0
    Case 2:
    lpMixPlayParams.lpMixWave = glpMix2
    lpMixPlayParams.iChannel = 2
    lpMixPlayParams.wLoops = 0
    Case 3:
    lpMixPlayParams.lpMixWave = glpMix3
    lpMixPlayParams.iChannel = 3
    lpMixPlayParams.wLoops = 0
    Case 4:
    lpMixPlayParams.lpMixWave = glpMix4
    lpMixPlayParams.iChannel = 4
    lpMixPlayParams.wLoops = 0
    Case 5:
    lpMixPlayParams.lpMixWave = glpMix5
    lpMixPlayParams.iChannel = 5
    lpMixPlayParams.wLoops = 0
    Case 6:
    lpMixPlayParams.lpMixWave = glpMix6
    lpMixPlayParams.iChannel = 6
    lpMixPlayParams.wLoops = 0
    Case 7:
    lpMixPlayParams.lpMixWave = glpMix7
    lpMixPlayParams.iChannel = 7
    lpMixPlayParams.wLoops = 0

End Select

lpMixPlayParams.wSize = Len(lpMixPlayParams)
lpMixPlayParams.hMixSession = ghMixSession
lpMixPlayParams.hWndNotify = 0&
lpMixPlayParams.dwFlags = dwFlags

rc = WaveMixPlay(lpMixPlayParams)
If rc > 0 Then
MsgBox "Could not play wave", MB_ICONSTOP, APP_NAME
End If

End Function

Sub Timer1_Timer ()
Dim iWaveNum As Integer
Dim rc As Integer

'generate a random wave to play
iWaveNum = Int((MAX_NUM_WAVES - MIN_NUM_WAVES + 1) * Rnd + MIN_NUM_WAVES)

'Play it
rc = PlayWave(iWaveNum, WMIX_CLEARQUEUE Or WMIX_HIPRIORITY)
rc = DoEvents()

End Sub

