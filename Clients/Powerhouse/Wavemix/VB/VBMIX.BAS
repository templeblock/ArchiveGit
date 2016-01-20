'-----------------------------------------------------------------------
' VBMIX.BAS - Visual Basic Wavemix Sample Code
'-----------------------------------------------------------------------

'About box constants
Global Const APP_NAME = "VBMix"
Global Const COPYRIGHT_YEAR = "1994"
Global Const VER_MAJOR = "1"
Global Const VER_MINOR = "0"

'taken from CONSTANT.TXT
Global Const PREPROCESS = -1
Global Const WA_INACTIVE = 0
Global Const WA_ACTIVE = 1
Global Const WA_CLICKACTIVE = 2
Global Const WM_ACTIVATE = &H6
Global Const WM_ACTIVATEAPP = &H1C
Global Const MB_ICONSTOP = 16          ' Critical message
Global Const KEY_LBUTTON = &H1
Global Const KEY_RBUTTON = &H2

'C-style BOOL values
Global Const WMIX_TRUE = 1
Global Const WMIX_FALSE = 0

'define the min and max wave numbers
Global Const MIN_NUM_WAVES = 1
Global Const MAX_NUM_WAVES = 7

'/ *************************** BEGIN WaveMix Stuff ************* /
' globals for each wave
Global glpMix1 As Long
Global glpMix2 As Long
Global glpMix3 As Long
Global glpMix4 As Long
Global glpMix5 As Long
Global glpMix6 As Long
Global glpMix7 As Long

' global to a MixSession
Global ghMixSession As Integer

'structures (wavemix user defined types)
Type MIXPLAYPARAMS
     wSize As Integer
     hMixSession As Integer
     iChannel As Integer
     lpMixWave As Long
     hWndNotify As Integer
     dwFlags As Long
     wLoops As Integer  '/* = &HFFFF means loop forever */
End Type

Type WAVEMIXINFO
     wSize As Integer
     bVersionMajor As String * 1
     bVersionMinor As String * 1
     szDate As String * 12 '/* Mmm dd yyyy */
     dwFormats As Long  '/* see waveOutGetDevCaps (wavemix requires synchronous device) */
End Type

Type MIXCONFIG
     wSize As Integer
     dwFlags As Long
     wChannels As Integer  '/* 1 = MONO, 2 = STEREO */
     wSamplingRate As Integer  '/* 11,22,44  (11=11025, 22=22050, 44=44100 Hz) */
End Type

'wavmix16 function declarations
Declare Function WaveMixInit Lib "wavmix16.dll" () As Integer
Declare Function WaveMixConfigureInit Lib "wavmix16.dll" (lpConfig As MIXCONFIG) As Integer
Declare Function WaveMixActivate Lib "wavmix16.dll" (ByVal hMixSession As Integer, ByVal fActivate As Integer) As Integer
Declare Function WaveMixOpenWave Lib "wavmix16.dll" (ByVal hMixSession As Integer, ByVal szWaveFilename As String, ByVal hInst As Integer, ByVal dwFlags As Long) As Long
Declare Function WaveMixOpenChannel Lib "wavmix16.dll" (ByVal hMixSession As Integer, ByVal iChannel As Integer, ByVal dwFlags As Long) As Integer
Declare Function WaveMixPlay Lib "wavmix16.dll" (lpMixPlayParams As MIXPLAYPARAMS) As Integer
Declare Function WaveMixFlushChannel Lib "wavmix16.dll" (ByVal hMixSession As Integer, ByVal iChannel As Integer, ByVal dwFlags As Long) As Integer
Declare Function WaveMixCloseChannel Lib "wavmix16.dll" (ByVal hMixSession As Integer, ByVal iChannel As Integer, ByVal dwFlags As Long) As Integer
Declare Function WaveMixFreeWave Lib "wavmix16.dll" (ByVal hMixSession As Integer, ByVal lpMixWave As Long) As Integer
Declare Function WaveMixCloseSession Lib "wavmix16.dll" (ByVal hMixSession As Integer) As Integer
Declare Function WaveMixPump Lib "wavmix16.dll" ()
Declare Function WaveMixGetInfo Lib "wavmix16.dll" (lpWaveMixInfo As WAVEMIXINFO) As Integer

'/* Wavemix constants */
Global Const WMIX_CONFIG_CHANNELS = &H1
Global Const WMIX_CONFIG_SAMPLINGRATE = &H2
Global Const WMIX_FILE = &H1
Global Const WMIX_RESOURCE = &H2
Global Const WMIX_MEMORY = &H4
Global Const WMIX_OPENSINGLE = 0   '/* open the single channel specified by iChannel */
Global Const WMIX_OPENALL = 1      '/* opens all the channels, iChannel ignored */
Global Const WMIX_OPENCOUNT = 2    '/* open iChannel Channels (eg. if iChannel = 4 will create channels 0-3) */
Global Const WMIX_ALL = &H1        '/* stops sound on all the channels, iChannel ignored */
Global Const WMIX_NOREMIX = &H2    '/* prevents the currently submited blocks from being remixed to exclude new channel */

'/* Wavemix flag values for play params */
Global Const WMIX_QUEUEWAVE = &H0
Global Const WMIX_CLEARQUEUE = &H1
Global Const WMIX_USELRUCHANNEL = &H2
Global Const WMIX_HIPRIORITY = &H4
Global Const WMIX_WAIT = &H8

'/ *************************** END WaveMix Stuff ************* /

