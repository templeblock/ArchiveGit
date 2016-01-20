VERSION 4.00
Begin VB.Form frmRemoteServerDetails 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "#"
   ClientHeight    =   4545
   ClientLeft      =   3195
   ClientTop       =   2400
   ClientWidth     =   7800
   ControlBox      =   0   'False
   Height          =   4950
   Icon            =   "SERVERDT.frx":0000
   Left            =   3135
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   4545
   ScaleWidth      =   7800
   Top             =   2055
   Width           =   7920
   Begin VB.CommandButton cmdCancel 
      Caption         =   "#"
      Height          =   375
      Left            =   5580
      TabIndex        =   5
      Top             =   3930
      Width           =   1935
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "#"
      Default         =   -1  'True
      Enabled         =   0   'False
      Height          =   375
      Left            =   3540
      TabIndex        =   4
      Top             =   3930
      Width           =   1935
   End
   Begin VB.ComboBox cboNetworkProtocol 
      Height          =   300
      Left            =   2400
      Style           =   2  'Dropdown List
      TabIndex        =   3
      Top             =   3165
      Width           =   5100
   End
   Begin VB.TextBox txtNetworkAddress 
      Height          =   300
      Left            =   2400
      TabIndex        =   1
      Top             =   2535
      Width           =   5100
   End
   Begin VB.Frame Frame1 
      Height          =   555
      Left            =   225
      TabIndex        =   7
      Top             =   1395
      Width           =   7290
      Begin VB.Label lblServerName 
         Alignment       =   2  'Center
         AutoSize        =   -1  'True
         Caption         =   "#"
         BeginProperty Font 
            name            =   "MS Sans Serif"
            charset         =   0
            weight          =   700
            size            =   8.25
            underline       =   0   'False
            italic          =   0   'False
            strikethrough   =   0   'False
         EndProperty
         Height          =   195
         Left            =   135
         TabIndex        =   8
         Top             =   240
         Width           =   7020
         WordWrap        =   -1  'True
      End
   End
   Begin VB.Label lblNetworkProtocol 
      AutoSize        =   -1  'True
      Caption         =   "#"
      Height          =   195
      Left            =   210
      TabIndex        =   2
      Top             =   3165
      Width           =   2100
      WordWrap        =   -1  'True
   End
   Begin VB.Label lblNetworkAddress 
      AutoSize        =   -1  'True
      Caption         =   "#"
      Height          =   195
      Left            =   225
      TabIndex        =   0
      Top             =   2535
      Width           =   2100
      WordWrap        =   -1  'True
   End
   Begin VB.Label lblRemoteServerDetails 
      AutoSize        =   -1  'True
      Caption         =   "#"
      BeginProperty Font 
         name            =   "MS Sans Serif"
         charset         =   0
         weight          =   400
         size            =   9.75
         underline       =   0   'False
         italic          =   0   'False
         strikethrough   =   0   'False
      EndProperty
      Height          =   240
      Left            =   360
      TabIndex        =   6
      Top             =   360
      Width           =   7020
      WordWrap        =   -1  'True
   End
End
Attribute VB_Name = "frmRemoteServerDetails"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
Option Explicit
Option Compare Text

Private fNetworkAddressSpecified As Boolean
Private fNetworkProtocolSpecified As Boolean

#If Win32 Then
Private Declare Function RpcNetworkIsProtseqValid Lib "rpcrt4.dll" Alias "RpcNetworkIsProtseqValidA" (ByVal strProtseq As String) As Long
#End If

#If Win32 Then
'Determines whether a given protocol sequence is supported and available on this machine
Function fIsProtocolSeqSupported(ByVal strProto As String, ByVal strProtoFriendlyName) As Boolean
    Const RPC_S_OK = 0&
    Const RPC_S_PROTSEQ_NOT_SUPPORTED = 1703&
    Const RPC_S_INVALID_RPC_PROTSEQ = 1704&

    Dim rcps As Long
    Static fUnexpectedErr As Boolean

    On Error Resume Next

    fIsProtocolSeqSupported = False
    rcps = RpcNetworkIsProtseqValid(strProto)

    Select Case rcps
        Case RPC_S_OK
            fIsProtocolSeqSupported = True
        Case RPC_S_PROTSEQ_NOT_SUPPORTED
            LogNote ResolveResString(resNOTEPROTOSEQNOTSUPPORTED, "|1", strProto, "|2", strProtoFriendlyName)
        Case RPC_S_INVALID_RPC_PROTSEQ
            LogWarning ResolveResString(resNOTEPROTOSEQINVALID, "|1", strProto, "|2", strProtoFriendlyName)
        Case Else
            If Not fUnexpectedErr Then
                MsgWarning ResolveResString(resPROTOSEQUNEXPECTEDERR), vbOKOnly Or vbInformation, gstrTitle
                fUnexpectedErr = True
            End If
    End Select
End Function
#End If

Private Sub cboNetworkProtocol_Click()
    cmdOK.Enabled = fValid()
End Sub

Private Sub cmdCancel_Click()
    ExitSetup frmRemoteServerDetails, gintRET_EXIT
End Sub

Private Sub cmdOK_Click()
    Hide
End Sub

Private Sub Form_Load()
    Dim fMoveControlsUp As Boolean 'Whether or not to move controls up to fill in an empty space
    Dim yTopCutoff As Integer 'We will move all controls lower down than this y value
    
    Caption = ResolveResString(resREMOTESERVERDETAILSTITLE)
    lblRemoteServerDetails.Caption = ResolveResString(resREMOTESERVERDETAILSLBL)
    lblNetworkAddress.Caption = ResolveResString(resNETWORKADDRESS)
    lblNetworkProtocol.Caption = ResolveResString(resNETWORKPROTOCOL)
    cmdOK.Caption = ResolveResString(resOK)
    cmdCancel.Caption = ResolveResString(resCANCEL)

    FillInProtocols
    
    'Now we selectively turn on/off the available controls depending on how
    '  much information we need from the user.
    If fNetworkAddressSpecified Then
        'The network address has already been filled in, so we can hide this
        '  control and move all the other controls up
        txtNetworkAddress.Visible = False
        lblNetworkAddress.Visible = False
        fMoveControlsUp = True
        yTopCutoff = txtNetworkAddress.Top
    ElseIf fNetworkProtocolSpecified Then
        'The network protocol has already been filled in, so we can hide this
        '  control and move all the other controls up
        cboNetworkProtocol.Visible = False
        lblNetworkProtocol.Visible = False
        fMoveControlsUp = True
        yTopCutoff = cboNetworkProtocol.Top
    End If
    
    If fMoveControlsUp Then
        'Find out how much to move the controls up
        Dim yDiff As Integer
        yDiff = cboNetworkProtocol.Top - txtNetworkAddress.Top
        
        Dim c As Control
        For Each c In Controls
            If c.Top > yTopCutoff Then
                c.Top = c.Top - yDiff
            End If
        Next c
        
        'Finally, shrink the form
        Height = Height - yDiff
    End If
    
    'Center the form
    Top = (Screen.Height - Height) \ 2
    Left = (Screen.Width - Width) \ 2
End Sub

'-----------------------------------------------------------
' SUB: GetServerDetails
'
' Requests any missing information about a remote server from
' the user.
'
' Input:
'   [strRegFile] - the name of the remote registration file
'   [strNetworkAddress] - the network address, if known
'   [strNetworkProtocol] - the network protocol, if known
'
' Ouput:
'   [strNetworkAddress] - the network address either passed
'                         in or obtained from the user
'   [strNetworkProtocol] - the network protocol either passed
'                          in or obtained from the user
'-----------------------------------------------------------
'
Public Sub GetServerDetails( _
    ByVal strRegFile As String, _
    strNetworkAddress As String, _
    strNetworkProtocol As String _
)
    Dim i As Integer
    
    'See if anything is missing
    fNetworkAddressSpecified = (strNetworkAddress <> "")
    fNetworkProtocolSpecified = (strNetworkProtocol <> "")
    
    If fNetworkAddressSpecified And fNetworkProtocolSpecified Then
        'Both the network address and protocol sequence have already
        'been specified in SETUP.LST.  There is no need to ask the
        'user for more information.
        
        'However, we do need to check that the protocol sequence specified
        'in SETUP.LST is actually installed and available on this machine
        '(32-bit only).
        CheckSpecifiedProtocolSequence strNetworkProtocol, strGetServerName(strRegFile)
        
        Exit Sub
    End If
    
    lblServerName.Caption = strGetServerName(strRegFile)
    
    'Show the form and extract necessary information from the user
    Show 1
        
    If fNetworkProtocolSpecified Then
        'The network protocol sequence had already been specified
        'in SETUP.LST.  We need to check that the protocol sequence specified
        'in SETUP.LST is actually installed and available on this machine
        '(32-bit only).
        CheckSpecifiedProtocolSequence strNetworkProtocol, strGetServerName(strRegFile)
    End If
    
    If Not fNetworkAddressSpecified Then
        strNetworkAddress = txtNetworkAddress
    End If
    If Not fNetworkProtocolSpecified Then
        strNetworkProtocol = gProtocol(cboNetworkProtocol.ListIndex + 1).strName
    End If
    
    Unload Me
End Sub

'-----------------------------------------------------------
' SUB: FillInProtocols
'
' Fills in the protocol combo with the available protocols from
'   setup.lst
'-----------------------------------------------------------
Private Sub FillInProtocols()
    Dim i As Integer
    Dim fSuccessReading As Boolean
    
    cboNetworkProtocol.Clear
    fSuccessReading = ReadProtocols(gstrSetupInfoFile, gstrINI_SETUP)
    If Not fSuccessReading Or gcProtocols <= 0 Then
        MsgError ResolveResString(resNOPROTOCOLSINSETUPLST), vbExclamation Or vbOKOnly, gstrTitle
        ExitSetup frmRemoteServerDetails, gintRET_FATAL
    End If
    
    For i = 1 To gcProtocols
        #If Win32 Then
        If fIsProtocolSeqSupported(gProtocol(i).strName, gProtocol(i).strFriendlyName) Then
        #End If
            cboNetworkProtocol.AddItem gProtocol(i).strFriendlyName
        #If Win32 Then
        End If
        #End If
    Next i

    If cboNetworkProtocol.ListCount Then
        'We were successful in finding at least one protocol available on this machine
        Exit Sub
    End If
    
    'None of the protocols specified in SETUP.LST are available on this machine.  We need
    'to let the user know what's wrong, including which protocol(s) were expected.
    MsgError ResolveResString(resNOPROTOCOLSSUPPORTED1), vbExclamation Or vbOKOnly, gstrTitle
    Dim strMsg As String
    strMsg = ResolveResString(resNOPROTOCOLSSUPPORTED2) & LF$

    For i = 1 To gcProtocols
        strMsg = strMsg & LF$ & Chr$(9) & gProtocol(i).strFriendlyName
    Next i
    
    MsgError strMsg, vbExclamation Or vbOKOnly, gstrTitle
    ExitSetup frmRemoteServerDetails, gintRET_FATAL
End Sub

'-----------------------------------------------------------
' SUB: strGetServerName
'
' Given a remote server registration file, retrieves the
'   friendly name of the server
'-----------------------------------------------------------
Private Function strGetServerName(ByVal strRegFilename As String) As String
    Const strKey = "AppDescription="
    Dim strLine As String
    Dim iFile As Integer
    
    On Error GoTo DoErr
    
    'This will have to do if we can't find the friendly name
    strGetServerName = GetFileName(strRegFilename)
    
    iFile = FreeFile
    Open strRegFilename For Input Access Read Lock Read Write As #iFile
    While Not EOF(iFile)
        Line Input #iFile, strLine
        If Left$(strLine, Len(strKey)) = strKey Then
            'We've found the line with the friendly server name
            Dim strName As String
            strName = Mid$(strLine, Len(strKey) + 1)
            If strName <> "" Then
                strGetServerName = strName
            End If
            Close iFile
            Exit Function
        End If
    Wend
    Close iFile
    
    Exit Function
DoErr:
    strGetServerName = ""
End Function

Private Sub txtNetworkAddress_Change()
    cmdOK.Enabled = fValid()
End Sub

'Returns True iff the inputs are valid
Private Function fValid() As Boolean
    fValid = True
    
    If Not fNetworkProtocolSpecified And (cboNetworkProtocol.ListIndex < 0) Then
        fValid = False
    End If
    
    If Not fNetworkAddressSpecified And (txtNetworkAddress = "") Then
        fValid = False
    End If
End Function

Private Sub CheckSpecifiedProtocolSequence(ByVal strNetworkProtocol As String, ByVal strFriendlyServerName As String)
    #If Win32 Then
        'Attempt to find the friendly name of this protocol from the list in SETUP.LST
        Dim fSuccessReading As Boolean
        Dim strFriendlyName As String
        Dim i As Integer
        
        strFriendlyName = strNetworkProtocol 'This will have to do if we can't find anything better
        
        fSuccessReading = ReadProtocols(gstrSetupInfoFile, gstrINI_SETUP)
        If fSuccessReading And gcProtocols > 0 Then
            For i = 1 To gcProtocols
                If gProtocol(i).strName = strNetworkProtocol Then
                    strFriendlyName = gProtocol(i).strFriendlyName
                    Exit For
                End If
            Next i
        End If
        
        'Now check to see if this protocol is available
        If fIsProtocolSeqSupported(strNetworkProtocol, strFriendlyName) Then
            'OK
            Exit Sub
        Else
            'Nope, not supported.  Give an informational message about what to do, then continue with setup.
Retry:
            If MsgError( _
                ResolveResString(resSELECTEDPROTONOTSUPPORTED, "|1", strFriendlyServerName, "|2", strFriendlyName), _
                vbInformation Or vbOKCancel, _
                gstrTitle) _
              = vbCancel Then
                'The user chose cancel.  Give them a chance to exit.
                ExitSetup frmRemoteServerDetails, gintRET_EXIT
                GoTo Retry
            Else
                'The user chose OK.  Continue with setup.
                Exit Sub
            End If
        End If
    #Else
        Exit Sub
    #End If
End Sub

