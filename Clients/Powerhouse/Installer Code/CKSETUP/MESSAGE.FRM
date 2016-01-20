VERSION 4.00
Begin VB.Form frmMessage 
   BorderStyle     =   3  'Fixed Dialog
   ClientHeight    =   900
   ClientLeft      =   1065
   ClientTop       =   1995
   ClientWidth     =   5340
   ClipControls    =   0   'False
   ControlBox      =   0   'False
   BeginProperty Font 
      name            =   "MS Sans Serif"
      charset         =   0
      weight          =   700
      size            =   8.25
      underline       =   0   'False
      italic          =   0   'False
      strikethrough   =   0   'False
   EndProperty
   Height          =   1305
   Icon            =   "MESSAGE.frx":0000
   Left            =   1005
   LinkTopic       =   "Form2"
   LockControls    =   -1  'True
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   900
   ScaleWidth      =   5340
   Top             =   1650
   Width           =   5460
   Begin VB.Image imgMsg 
      Height          =   480
      Left            =   270
      Picture         =   "MESSAGE.frx":0442
      Top             =   210
      Width           =   480
   End
   Begin VB.Label lblMsg 
      AutoSize        =   -1  'True
      Caption         =   "*"
      Height          =   195
      Left            =   945
      TabIndex        =   0
      Top             =   360
      Width           =   4110
      WordWrap        =   -1  'True
   End
End
Attribute VB_Name = "frmMessage"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
Option Explicit
Option Compare Text

