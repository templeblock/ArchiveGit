// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®PL1¯®FD1¯®BT0¯®TP0¯*/
/************************************************************************/
/* Astral.c    Astral picture publishing application			*/
/************************************************************************/
#define MAIN

#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"
#include "scan.h"

#define EXE_NAME_MAX_SIZE 128
#define COLOR_PATCH_CLASS 1

/************************************************************************/
/* The global handles							*/
/************************************************************************/
HANDLE	hInstAstral;
HWND	hWndAstral;
HWND	hClientAstral;
HWND 	hActiveWindow;
HANDLE	hAccelTable, hPWAccelTable;
static BOOL fAppActive;
static BOOL fHelp;
static WORD idLastWindow;

/************************************************************************/
/* Other global data							*/
/************************************************************************/
char szAppName[MAX_STR_LEN];
char szUntitled[MAX_STR_LEN];
LPTR lpScannerDriver, lpGrabberDriver;
static FARPROC lpPrevMsgHook;

/***********************************************************************/
int PASCAL WinMain( hInstance, hPrev, lpszCmdLine, cmdShow )
/***********************************************************************/
HANDLE hInstance, hPrev;
LPSTR lpszCmdLine;
int cmdShow;
{
MSG msg;
WORD wHeight, wWidth;
HWND hWindow;
char temp[MAX_FNAME_LEN], *pDir;
int i, disable;
DWORD selseg;
HDC hDC;
HBITMAP hBitmap;
WORD Pattern[16];

/************************************************************************/
/* Initialization for the 1st instance					*/
/************************************************************************/

if (!hPrev)
    if (!AstralInit( hInstance ))
	return( 0 );

/************************************************************************/
/* Initialization for all instances						*/
/************************************************************************/

/* Load strings from the string table */
LoadString(hInstance, IDS_APPNAME, szAppName, MAX_STR_LEN);
Control.ColorEnabled = YES;
//i = 0;
//while ( szAppName[i] )
//	{
//	if ( !strcmp( "Plus", &szAppName[i] ) )
//		Control.ColorEnabled = YES;
//	i++;
//	}
Control.Junior = NO;
i = 0;
while ( szAppName[i] )
	{
	if ( !strcmp( "Junior", &szAppName[i] ) )
		{
		Control.ColorEnabled = NO;
		Control.Junior = YES;
		}
	i++;
	}

// Allocate the Linebuffer buffers
if ( !(selseg = GlobalDosAlloc(65536L)) )
	{
	Print("Couldn't allocate 64K of DOS memory; exiting...");
	return(0);
	}
Control.lpSelector = (LPTR)(selseg << 16);
Control.lpSegment  = (LPTR)(selseg & 0xffff0000L);

// Allocate the Linebuffer buffers
//if ( !(Control.lpSelector = Alloc(65536L)) )
//	{
//	Print("Couldn't allocate 64K of DOS memory; exiting...");
//	return(0);
//	}

// Allocate the brush buffer
RetouchBrushes = (BRUSHSETTINGS FAR *)
		Alloc ((long) (sizeof (BRUSHSETTINGS) * TOTAL_BRUSHES));
if( !RetouchBrushes )
	{
	Print("Couldn't allocate brush buffers; exiting...");
	return(0);
	}

LineBuffer[0] = Control.lpSelector;
LineBuffer[1] = Control.lpSelector + (16384L * 1);
LineBuffer[2] = Control.lpSelector + (16384L * 2);
LineBuffer[3] = Control.lpSelector + (16384L * 3);

/* Get the handles to the keyboard accelerators */
hAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( ID_ACCEL ) );
hPWAccelTable = LoadAccelerators( hInstance, MAKEINTRESOURCE( ID_PWACCEL ) );

/* Size the application window based on the screen size */
wWidth = GetSystemMetrics( SM_CXSCREEN );
wHeight = GetSystemMetrics( SM_CYSCREEN );

if ( Control.Is30 = ( (GetVersion() & 0xFF) >= 3 ) )
	{
	Control.IsPmode = ( GetWinFlags() & WF_PMODE ? YES : NO );
	Control.IsEnchanted = ( GetWinFlags() & WF_ENHANCED ? YES : NO );
	}
else	Control.IsPmode = NO;

/* Set the global instance handle */
hInstAstral = hInstance;

/* Create the main application window */
hWindow = CreateWindow(
	ID_APPL,		/** lpClassName **/
	NULL,			/** lpWindowName **/
	WS_OVERLAPPEDWINDOW |
	WS_MAXIMIZE |
	WS_CLIPCHILDREN, 	/** dwStyle **/
	1, 1, 			/** position of window **/
	wWidth-2, wHeight-2,	/** size of window **/
	NULL,			/** hWindowParent (null for tiled) **/
	NULL,			/** hMenu (null = classmenu) **/
	hInstance,		/** hInstance **/
	NULL			/** lpParam **/
	);

/* Set the global application window handle */
hWndAstral = hWindow;

/* Get the name of the file being executed */
GetModuleFileName( hInstAstral, Control.ProgHome, sizeof(Control.ProgHome)-1);
Control.Crippled = IsDemo( Control.ProgHome );

/* Get the drive and directory path as a default for file locations */
pDir = Control.ProgHome;
pDir += strlen( pDir );
while( *pDir != '\\' && pDir > Control.ProgHome )
	*pDir-- = '\0';

/* Load the global data structures with any default values */
LoadAllPreferences( Control.ProgHome );
Window.hBlackBrush = CreateSolidBrush( GetSysColor(COLOR_WINDOWFRAME) );
Window.hWhiteBrush = CreateSolidBrush( GetSysColor(COLOR_WINDOW) );
Window.hShadowBrush = CreateSolidBrush( GetSysColor(COLOR_BTNSHADOW) );
Window.hButtonBrush = CreateSolidBrush( GetSysColor(COLOR_BTNFACE) );
for ( i=0; i<16; i+=2 )
	Pattern[i] = 0xAAAA;
for ( i=1; i<16; i+=2 )
	Pattern[i] = 0x5555;
hBitmap = CreateBitmap( 16, 16, 1, 1, (LPTR)Pattern );
Window.hHilightBrush = CreatePatternBrush( hBitmap );
DeleteObject( hBitmap );
//Window.hHilightBrush = GetStockObject( LTGRAY_BRUSH );
//Window.hHilightBrush = CreateSolidBrush( RGB(192,192,192) );
Window.hBlackPen = CreatePen( PS_SOLID, 1, GetSysColor(COLOR_WINDOWFRAME));
Window.hWhitePen = CreatePen( PS_SOLID, 1, GetSysColor(COLOR_WINDOW));
Window.hShadowPen = CreatePen( PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
Window.hButtonPen = CreatePen( PS_SOLID, 1, GetSysColor(COLOR_BTNFACE));
Window.hDottedPen = CreatePen( PS_DOT, 1, RGB(0,0,0));
Window.hHelv10Font = CreateFont(/* Height */ 10,
				/* Width */ 10,
				/* Escapement */ 0,
				/* Orientation */ 0,
				/* Weight */ FW_LIGHT,
				/* Italic */ FALSE,
				/* UnderLine */ FALSE,
				/* StrikeOut */ FALSE,
				/* CharSet */ ANSI_CHARSET,
				/* Output Precis */ OUT_DEFAULT_PRECIS,
				/* Clip Precis */ CLIP_DEFAULT_PRECIS,
				/* Quality */ DEFAULT_QUALITY,
				/* PitchAndFamily */ DEFAULT_PITCH|FF_SWISS,
				/* Face */ "Helv" );

Window.hLabelFont = CreateFont(/* Height */ 8,
				/* Width */ 0,
				/* Escapement */ 0,
				/* Orientation */ 0,
				/* Weight */ FW_LIGHT,
				/* Italic */ FALSE,
				/* UnderLine */ FALSE,
				/* StrikeOut */ FALSE,
				/* CharSet */ ANSI_CHARSET,
				/* Output Precis */ OUT_DEFAULT_PRECIS,
				/* Clip Precis */ CLIP_DEFAULT_PRECIS,
				/* Quality */ DEFAULT_QUALITY,
				/* PitchAndFamily */ DEFAULT_PITCH|FF_SWISS,
				/* Face */ "Helv" );

/* Initialize all of the palettes and map structures */
hDC = GetDC( hWndAstral );
InitSuperBlt( hDC, &BltScreen, NULL );
ReleaseDC( hWndAstral, hDC );
ResetAllMaps();

/* Check the grabber driver */
if ( DevName((LPSTR)temp) ) /* devname returned TRUE,nograb.dgr loaded */
	EnableMenuItem( GetMenu( hWndAstral ), IDM_GRAB, MF_GRAYED );
else	{  /* put up the grabber's name in the file menu */
	ModifyMenu( GetMenu(hWndAstral), IDM_GRAB, MF_BYCOMMAND | MF_STRING,
		IDM_GRAB, temp );
	}

/* Disable the scanning option if the driver cannot be found */
disable = NO;
if ( ScanOpen() )
	ScanClose();
else	disable = YES;

/* Put the application's name up in lights! */
SetWindowText( hWindow, szAppName );

/* Display the application window */
if ( cmdShow == SW_NORMAL )
	cmdShow = SW_MAXIMIZE;
ShowWindow( hWindow, cmdShow );
UpdateWindow( hWindow );

/* Bring up the modeless windows */
AstralDlg( YES, hInstAstral, hWindow, IDD_STATUS, DlgStatusProc);
UpdateStatusBar( YES, YES, YES, YES );
MessageStatus( IDS_WELCOME, (LPTR)szAppName );
if ( !Control.IsPmode )
	DisableSavebits( AstralDlgGet(IDD_STATUS) );

if ( !Control.Junior )
	{
	AstralDlg( YES, hInstAstral, hWndAstral, IDD_TOOLS, DlgToolsProc );
//	AstralDlg( YES, hInstAstral, hWndAstral, IDD_PALETTE, DlgPaletteProc );
	}

SendMessage( hWndAstral, WM_SIZE, 0, 0L );

AstralDlg( NO, hInstAstral, hWndAstral, IDD_HERALD, DlgHeraldProc );
if ( (i = GetProfileInt( ID_APPL, "Warning", -1 )) <= 0 )
	{
	AstralDlg( NO, hInstAstral, hWndAstral, IDD_WARNING, DlgWarningProc );
	if ( !i )
		WriteProfileString( ID_APPL, "Warning", "1" );
	}

/* If the program is crippled, try to load a demo message file */
/* If there isn't one, put up the crippled message */
if ( Control.Crippled )
	if ( !InstallPlayer( "DEMO.SHO" ) )
		Message( Control.Crippled );

/* If the caller passed in a file name, try to load it */
if ( lpszCmdLine )
	{
	if ( *lpszCmdLine )
		{
		lstrcpy( temp, lpszCmdLine );
		AstralImageLoad( temp );
		}
	}

SetFocus( hWindow );

/* Enter the one and only message loop... */
while ( TRUE )
	{
	if (!AstralGetMessage(&msg))
		break;
	AstralProcessMessage(&msg);
	}

/* The application reveived a WM_QUIT Message */
return( msg.wParam );
}


/***********************************************************************/
BOOL AstralInit( hInstance )
/***********************************************************************/
HANDLE		hInstance;
{
WNDCLASS	WndClass;

/* Setup and define the window class for the application */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(ID_LOGO));
WndClass.lpszMenuName	= (LPSTR)"PP";
WndClass.lpszClassName	= (LPSTR)ID_APPL;
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= COLOR_APPWORKSPACE+1;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
WndClass.lpfnWndProc	= AstralWndProc;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 0;

if (!RegisterClass( &WndClass ))
	return( FALSE );

#ifndef PPVIDEO
/* Setup and define the window class for selection tracking on the scanframe */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(ID_LOGO));
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"frame";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= COLOR_APPWORKSPACE+1;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
WndClass.lpfnWndProc	= FrameControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 0;

if (!RegisterClass( &WndClass ))
	return( FALSE );
#endif

/* Setup and define the window class for a joystick */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(ID_LOGO));
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"joystick";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= COLOR_WINDOW+1;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
WndClass.lpfnWndProc	= StickControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 4;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for a joystick */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(ID_LOGO));
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"map";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= COLOR_WINDOW+1;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
WndClass.lpfnWndProc	= MapControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 12;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for a static path */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(ID_LOGO));
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"path";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
WndClass.lpfnWndProc	= PathControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= sizeof(short) + sizeof(BOOL) + sizeof(short);

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for a joystick */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(ID_LOGO));
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"lever";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
WndClass.lpfnWndProc	= LeverControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 4;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for 3 dimensional icons */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(ID_LOGO));
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"icon";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
WndClass.lpfnWndProc	= IconControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 8;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for text blocks */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(ID_LOGO));
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"block";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
WndClass.lpfnWndProc	= TextBlockControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 8;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for a hue control */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(ID_LOGO));
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"hue";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
WndClass.lpfnWndProc	= HueControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 4;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for a histo control */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(ID_LOGO));
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"histo";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= COLOR_WINDOW+1;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
WndClass.lpfnWndProc	= HistoControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 8;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for a histo control */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(ID_LOGO));
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"quarters";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
WndClass.lpfnWndProc	= QuartersControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 4;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for a color picker control */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(ID_LOGO));
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"picker";
WndClass.hCursor	= LoadCursor( hInstance, MAKEINTRESOURCE(ID_SELECTOR));
WndClass.hbrBackground	= COLOR_WINDOW+1;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
WndClass.lpfnWndProc	= PickerControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 4;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define an window class for an image window */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(ID_LOGO));
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"mydialog";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= COLOR_BTNFACE+1;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
WndClass.lpfnWndProc	= DefDlgProc;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= DLGWINDOWEXTRA+20;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define an window class for an image window */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(ID_LOGO));
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"color";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
WndClass.lpfnWndProc	= ColorControl;
WndClass.cbClsExtra	= COLOR_PATCH_CLASS;
WndClass.cbWndExtra	= 4;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define an window class for an image window */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(ID_LOGO));
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"color2";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
WndClass.lpfnWndProc	= Color2Control;
WndClass.cbClsExtra	= COLOR_PATCH_CLASS;
WndClass.cbWndExtra	= 8;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define an window class for an image window */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(ID_LOGO));
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"scroll";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW; /* NO CS_DBLCLKS */
WndClass.lpfnWndProc	= ScrollControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 4;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for static text using dialog bg */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(ID_LOGO));
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"stext";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW;
WndClass.lpfnWndProc	= StaticTextControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 4;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for static text using dialog bg */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(ID_LOGO));
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"mybutton";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= NULL;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW;
WndClass.lpfnWndProc	= ButtonControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 8;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define the window class for static text using dialog bg */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(ID_LOGO));
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"ruler";
WndClass.hCursor	= LoadCursor( NULL, IDC_ARROW );
WndClass.hbrBackground	= COLOR_WINDOW+1;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW;
WndClass.lpfnWndProc	= RulerControl;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 58;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define a window class for the child image windows */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(ID_LOGO));
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"ppimage";
WndClass.hCursor	= NULL;
WndClass.hbrBackground	= COLOR_BTNFACE+1;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS |
			  CS_BYTEALIGNCLIENT;
WndClass.lpfnWndProc	= WndImageProc;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 20;

if (!RegisterClass( &WndClass ))
	return( FALSE );

register_mgxbtn_class(hInstance, FALSE);

return( TRUE );
}


/***********************************************************************/
BOOL AstralGetMessage( lpMsg )
/***********************************************************************/
LPMSG lpMsg;
{
while (TRUE)
	{
	if (mask_active())
		{
		if ( !(GetTickCount() & 3) )
			Marquee(NO);
		if ( !PeekMessage( lpMsg, NULL,NULL,NULL,PM_REMOVE))
			continue;
		if ( lpMsg->message == WM_QUIT )
			return(FALSE);
		return(TRUE);
		}
	else	
		{
		return(GetMessage( lpMsg, NULL, NULL, NULL ) );
		}
	}
}

/***********************************************************************/
void AstralProcessMessage( lpMsg )
/***********************************************************************/
LPMSG lpMsg;
{
HWND hSaveWnd;

/* If an accelerator was executed, go get another command */
if (TranslateMDISysAccel (hClientAstral, lpMsg))
	return;
hSaveWnd = lpMsg->hwnd;
lpMsg->hwnd = hWndAstral;
TranslateAccelerator( hWndAstral, hAccelTable, lpMsg );
lpMsg->hwnd = hSaveWnd;

/* If some child window took the message, go get another one */
if ( AstralDlgMsg( lpMsg ) )
	return;

/* Process the message with AstralWndProc() */
TranslateMessage( lpMsg );
DispatchMessage( lpMsg );
}

/***********************************************************************/
LONG FAR PASCAL AstralWndProc( hWindow, message, wParam, lParam )
/***********************************************************************/
HWND hWindow;
unsigned message;
WORD wParam;
long lParam;
{
char szStr[MAX_STR_LEN];
char temp[MAX_STR_LEN];
HWND hWnd;
CLIENTCREATESTRUCT ccs;
HDC hDC;
RECT rect;
int y, dy;
DWORD dwBaseUnits;
MSG msg;

switch ( message )
    {
    case WM_CREATE:
	lpPrevMsgHook = SetWindowsHook( WH_MSGFILTER,
		MakeProcInstance( (FARPROC)MessageHookProc, hInstAstral ) );
	Window.hNullCursor = LoadCursor( hInstAstral, MAKEINTRESOURCE(ID_NULL));
	/* Find window menu where children will be listed */
	ccs.hWindowMenu = GetSubMenu (GetMenu(hWindow),IDM_WINDOWS);
	ccs.idFirstChild = IDM_WINDOWCHILD;

	/* Create the MDI client filling the client area */
	hClientAstral = CreateWindow ("mdiclient",
					NULL,
					WS_CHILD | WS_CLIPCHILDREN |
					WS_CLIPSIBLINGS,
					0,
					0,
					0,
					0,
					hWindow,
					0xCAC,
					hInstAstral,
					(LPSTR)&ccs);
	ShowWindow (hClientAstral,SW_SHOW);
	break;

    case WM_DESTROY:
	Help( HELP_QUIT, 0L );
	PostQuitMessage( 0 );
	break;

    case WM_RBUTTONDOWN:
	SetFullClientRect( hWindow );
	break;

    case WM_INITMENU:
	SetFileMenus();
	SetUndoMenus();
	SetPasteMenus();
	SetMaskMenus();
	SetWindowMenus();
//	SetShowHideMenus( IDM_PALETTESHOW, IDD_PALETTE );
//	SetShowHideMenus( IDM_SHIELDSHOW, IDD_SHIELDFLOAT );
//	SetShowHideMenus( IDM_TOOLSHOW, IDD_TOOLS );
//	SetShowHideMenus( IDM_STATUSSHOW, IDD_STATUS );
//	SetShowHideMenus( IDM_PASTEOPTIONS, IDD_PASTEOPTIONS );
	break;

   case WM_ACTIVATEAPP:
	if (wParam)
		{
		fAppActive = TRUE;
		if (lpImage)
			ActivateDocument(lpImage->hWnd);
		}
	else	{
		if ( fHelp )
			break;
		fAppActive = FALSE;
		}
	return(DefFrameProc(hWindow,hClientAstral,message,wParam,lParam));

   case WM_PALETTECHANGED:
	HandlePaletteChange(wParam);
	break;

    case WM_MOUSEACTIVATE: // Get when there's a mouse click on a child window
	return( DefFrameProc( hWindow, hClientAstral, message, wParam, lParam));
	break;

    case WM_SETCURSOR:
	idLastWindow = GetWindowWord( wParam, GWW_ID );
	if (SetupCursor((HWND)wParam, LOWORD(lParam), HIWORD(lParam)))
		return(TRUE);
	return( DefFrameProc(hWindow, hClientAstral, message, wParam, lParam) );

    case WM_MENUSELECT:
	idLastWindow = wParam;
	MessageStatus( wParam, (LPTR)"", (LPTR)"" );
	break;

    case WM_SYSCOMMAND:
	return( DefFrameProc( hWindow, hClientAstral, message, wParam, lParam));

    case WM_SIZE:
	if ( wParam == SIZEICONIC )
	     return( DefFrameProc( hWindow, hClientAstral, message,
		wParam, lParam));
	if ( hWnd = AstralDlgGet( IDD_STATUS ) )
		{
		SendMessage( hWnd, WM_INITDIALOG, 0, 0L );
		GetClientRect( hWnd, &rect );
		dy = RectHeight( &rect );
		}
	else	dy = 0;
	if ( hWnd = AstralDlgGet( Window.idOptionBox ) )
		{
		SendMessage( hWnd, WM_INITDIALOG, 0, 0L );
		GetClientRect( hWnd, &rect );
		y = RectHeight( &rect );
		}
	else	y = 0;

	GetClientRect( hWindow, &rect );
	rect.bottom -= (dy + y);
	MoveWindow(hClientAstral, 0, y, RectWidth(&rect), RectHeight(&rect),
		TRUE);
 	break;

    case WM_COMMAND:
	if ( HELP ) { Help( HELP_CONTEXT, (long)wParam ); }
	AstralCommand( hWindow, wParam );
	return( DefFrameProc( hWindow, hClientAstral, message, wParam, lParam));
	break;

    case WM_PAINT:
	AstralPaint( hWindow );
	break;

    case WM_RENDERALLFORMATS:
	OpenClipboard(hWindow);
	EmptyClipboard();

    case WM_RENDERFORMAT:
#ifdef UNUSED
	if(!(lCount = bmpinit(lpClipFrame)))
	   break;
	// Round up to the next 16 byte boundary
	lCount = ((lCount + 15) / 16) * 16;
	/* Allocate the memory from the global heap - NOT limited to 64K */
	if ( !(hMemory = GlobalAlloc(GMEM_MOVEABLE, lCount )) )
	   {
	    Print("Memory is not enough");
	    break;
	   }

	/* Get the memory pointer from the handle */
	if ( !(lpMemory = (LPTR)GlobalWire( hMemory )) )
		{
		 GlobalFree( hMemory );
		 Print("Memory is not enough");
		 break;
		}
	/* alloc memory for the palette */
	if ( !(hPalMem = GlobalAlloc(GMEM_MOVEABLE, 1028L )) )
	   {
	    Print("Memory is not enough");
	    break;
	   }

	/* Get the memory pointer from the handle */
	if ( !(lpPalMem = (LPTR)GlobalWire( hPalMem )) )
		{
		 GlobalFree( hPalMem );
		 Print("Memory is not enough");
		 break;
		}
	if(bmp2clp(lpClipFrame, lpMemory, lpPalMem) < 0)
	  {
	   GlobalUnWire( hPalMem );
	   GlobalFree( hPalMem );
	   GlobalUnWire( hMemory );
	   GlobalFree( hMemory );
	   Print("BMP conversion error");
	   break;
	  }
	SetClipboardData(CF_DIB, hMemory);  // write the DIB
	SetClipboardData(CF_PALETTE, hPalMem);  // write the DIB's palette
   	if( message == WM_RENDERALLFORMATS )  // closing our app
		CloseClipboard();
#endif
	break;

    case WM_CLOSE:
    case WM_QUERYENDSESSION: /* end Windows session */
	if ( lpPrevMsgHook )
		SetWindowsHook( WH_MSGFILTER, lpPrevMsgHook );
	if ( Scans.lpScanFrame )
		frame_close( Scans.lpScanFrame );
	/* Check to see if the image needs to be saved */
	if ( !ConfirmClose(YES) )
		return( FALSE );
	CloseImage(YES);
	DeactivateTool();
	/* Free up any DOS memory */
	if ( Control.lpSelector )
//		FreeUp( Control.lpSelector );
		GlobalDosFree( HIWORD(Control.lpSelector) );
	if ( RetouchBrushes )
		FreeUp( (LPTR) RetouchBrushes );
	/* Install any drivers that were performed by the user */
	if ( lpGrabberDriver )
		{
		lstrcpy( temp, Control.ProgHome );
		lstrcat( temp, "PPGRAB.DLL" );
		lstrcpy( szStr, lpGrabberDriver );
		Message( IDS_COPYDRV, (LPTR)szStr, (LPTR)temp );
		CopyFile( lpGrabberDriver, temp );
		FreeUp( lpGrabberDriver );
		}
	if ( lpScannerDriver )
		{
		lstrcpy( temp, Control.ProgHome );
		lstrcat( temp, "PPSCAN.DLL" );
		lstrcpy( szStr, lpScannerDriver );
		Message( IDS_COPYDRV, (LPTR)szStr, (LPTR)temp );
		CopyFile( lpScannerDriver, temp );
		FreeUp( lpScannerDriver );
		}
	RemovePlayer();
	RemoveRecorder();
	EMSclose();
	/* Save any default values */
	/* Delete any objects that might be hanging around */
	DeleteObject( Window.hBlackBrush );
	DeleteObject( Window.hWhiteBrush );
	DeleteObject( Window.hShadowBrush );
	DeleteObject( Window.hButtonBrush );
	DeleteObject( Window.hHilightBrush );
	DeleteObject( Window.hBlackPen );
	DeleteObject( Window.hWhitePen );
	DeleteObject( Window.hShadowPen );
	DeleteObject( Window.hDottedPen );
	DeleteObject( Window.hHelv10Font);
	DeleteObject( Window.hLabelFont);
	if ( BltScreen.hGrayPal )
		DeleteObject( BltScreen.hGrayPal );
	if ( BltScreen.hColorPal &&
	     ( BltScreen.hGrayPal != BltScreen.hColorPal ) )
		DeleteObject( BltScreen.hColorPal );
	DestroyWindow( hClientAstral );
	DestroyWindow( hWindow );
	return( TRUE );

    case WM_TIMER:
	Marquee(NO);
	break;

    default:
	return( DefFrameProc( hWindow, hClientAstral, message, wParam, lParam ) );
	break;
    }

return( TRUE );
}


/***********************************************************************/
BOOL SetupCursor( hWnd, HitCode, msg )
/***********************************************************************/
HWND hWnd;
WORD HitCode;
unsigned msg;
{
POINT CursorPos;

DisplayBrush( 0, 0, 0, OFF );
if ( !fAppActive )
	{ // if the app is not active, use the arrow...
	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
	return( TRUE ); // means we have taken care of cursor
	}

if ( fHelp )
	{ // if in "help" mode, use the help cursor...
	SetCursor( LoadCursor( hInstAstral, MAKEINTRESOURCE(ID_HELP) ) );
	return( TRUE ); // means we have taken care of cursor
	}

if ( !lpImage || HitCode != HTCLIENT || !IsDoc(hWnd) )
	{ // if its any old (non-image) window, use whatever cursor it wants...
	if ( HitCode == HTCLIENT )
		{
		if ( GetClassWord( hWnd, GCW_CBCLSEXTRA ) != COLOR_PATCH_CLASS )
			MessageStatus( GetWindowWord(hWnd,GWW_ID),
				(LPTR)"", (LPTR)"" );
		}
	return( FALSE );
	}

if ( lpImage->hWnd != hWnd )
	{ // if its an inactive image, use an arrow...
	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
	return( TRUE ); // means we have taken care of cursor
	}

// Its the active image...
if ( !Window.ToolActive )
	MessageStatus( Control.Function );

if ( Window.hCursor != Window.hNullCursor )
	{
	GetCursorPos( &CursorPos );
	ScreenToClient( hWnd, &CursorPos );
	if ( !ONIMAGE( CursorPos.x, CursorPos.y ) )
		{ // if not on the image display area, use the arrow...
		SetCursor( LoadCursor( NULL, IDC_ARROW ) );
		return( TRUE ); // means we have taken care of cursor
		}
	}

// Use whatever cursor is loaded
return( FALSE );
}


/***********************************************************************/
BOOL SetCurView( hWnd, lphOldWnd )
/***********************************************************************/
HWND hWnd;
LPHWND lphOldWnd;
{
static HWND hSaveWnd;
LPIMAGE lpOldImage;
LPDISPLAY lpOldDisplay;

if (lphOldWnd)
    *lphOldWnd = hSaveWnd;
lpOldImage = lpImage;
if (hWnd)
    {
    if ( lpImage = (LPIMAGE)GetWindowLong( hWnd, 12 ) )
	{
	lpOldDisplay = lpImage->lpDisplay;
	lpImage->lpDisplay = (LPDISPLAY)GetWindowLong(hWnd, 8);
	if (lpImage->lpDisplay)
	    {
	    lpImage->hWnd = hWnd;
	    frame_set(lpImage->EditFrame);
	    hSaveWnd = hWnd;
	    return(TRUE);
	    }
	else
	    {
	    lpImage->lpDisplay = lpOldDisplay;
	    lpImage = lpOldImage;
	    }
	}
    else
	{
  	lpImage = lpOldImage;
	}
    }
return(FALSE);
}

/***********************************************************************/
HWND ActivateDocument( hWnd )
/***********************************************************************/
HWND hWnd;
{
HWND hDlg, hOldActiveWindow;
RECT repairRect;

if ( hActiveWindow && Window.hDC )
	{
	if (!hWnd) /* a deactivate */
	    {
	    DeactivateTool();
    	    if (lpImage && lpImage->EditFrame && lpImage->EditFrame->WriteMask)
		{
		File2DispRect(&lpImage->EditFrame->WriteMask->rMask, &repairRect);
		InflateRect(&repairRect, 1, 1);
		PaintImage(Window.hDC, &repairRect, NULL);
		}
	    }
	ReleaseDC( hActiveWindow, Window.hDC );
	}
if ( !(hOldActiveWindow = hActiveWindow) )
	hOldActiveWindow = hWnd;
hActiveWindow = hWnd;
if (hActiveWindow)
    {
    Window.hDC = GetDC( hActiveWindow );
    if (SetCurView(hActiveWindow, NULL))
	{
	if (lpImage->hPal)
	    {
	    SelectPalette( Window.hDC, lpImage->hPal, FALSE );
	    RealizePalette( Window.hDC );
	    }
	BltScreen.hPal = lpImage->hPal;
	Marquee(YES);
	}
    UpdateStatusBar( YES, YES, YES, NO );
    }
else
    {
    Window.hDC = NULL;
    lpImage = NULL;
    }
return( hOldActiveWindow );
}

/***********************************************************************/
VOID HandlePaletteChange( hWnd )
/***********************************************************************/
HWND hWnd;
{
int i;
HWND hWindow;

if ( !IsDoc(hWnd) ) /* did our window cause the change */
	return;

InvalidateRect(hWnd, NULL, TRUE);
for (i = 0; i < NumDocs(); ++i)
	{
	hWindow = GetDoc(i);
	if (hWindow && hWindow != hWnd)
	    InvalidateRect(hWindow, NULL, TRUE);
	}

if ( hWindow = AstralDlgGet(IDD_TOOLS) )
	AstralControlPaint( hWindow, IDC_ACTIVECOLOR );

if ( hWindow = AstralDlgGet(IDD_SHIELDFLOAT) )
	{
	AstralControlPaint( hWindow, IDC_MASKCOLOR1 );
	AstralControlPaint( hWindow, IDC_MASKCOLOR2 );
	AstralControlPaint( hWindow, IDC_MASKCOLOR3 );
	AstralControlPaint( hWindow, IDC_MASKCOLOR4 );
	AstralControlPaint( hWindow, IDC_MASKCOLOR5 );
	AstralControlPaint( hWindow, IDC_MASKCOLOR6 );
	AstralControlPaint( hWindow, IDC_MASKCOLOR7 );
	AstralControlPaint( hWindow, IDC_MASKCOLOR8 );
	}

if ( hWindow = AstralDlgGet(IDD_PALETTE) )
	InvalidateRect(hWindow, NULL, TRUE);
}


/***********************************************************************/
VOID AstralCommand( hWindow, wParam )
/***********************************************************************/
HWND hWindow;
WORD wParam;
{
FNAME szFileName;
LPFRAME lpFrame;
LPMASK lpMask;
RECT rMask;
HWND hWnd;
RGBS rgb;
BOOL IsOutside;
STRING szClassName;
POINT Points[4];
int i, ret;

if (wParam != IDM_BACKUP &&
    wParam != IDM_DELETEITEM &&
    wParam != IDM_ESCAPE &&
    wParam != IDM_PASTEOPTIONS)
	DeactivateTool();

switch (wParam)
    {
    case IDM_EXIT:
	SendMessage( hWndAstral, WM_CLOSE, 0, 0L);
	break;

    case IDM_CLOSE:
	CloseImage( NO );
	break;

    case IDM_ABOUT:
	/* Bring up the modal 'About' dialog box */
	AstralDlg( NO, hInstAstral, hWindow, IDD_ABOUT, DlgAboutProc);
	break;

    case IDM_NEW:
	/* Bring up the modal 'New' box */
	if ( AstralDlg( NO, hInstAstral, hWindow, IDD_NEW, DlgNewProc))
		{
		AstralCursor( IDC_WAIT );
		new( Edit.Depth );
		AstralCursor( NULL );
		}
	break;

    case IDM_OPEN:
	/* Bring up the modal 'File Open' box */
	if ( DoOpenDlg( hWindow, IDD_OPEN, 0, szFileName, NO ) )
		AstralImageLoad( szFileName );
	break;

    case IDM_SAVE:
	if ( !lpImage )
		break;
	/* Save the image under the current name */
	if ( !lpImage->fUntitled) /* If the picture has a name, save into it */
		{
		if (AstralImageSave( lpImage->CurFile ))
			CacheApply();
		break;
		}
	/* else fall through to SaveAs... */

    case IDM_SAVEAS:
	if ( !lpImage )
		break;
	/* Bring up the modal 'SaveAs' box */
	if ( DoOpenDlg( hWindow, IDD_SAVEAS, 0, szFileName, YES ))
		if (AstralImageSave( szFileName ))
			CacheApply();
	break;

    case IDM_REVERT:
	if ( !lpImage )
		break;
	if ( lpImage->fUntitled ) // If the picture doesn't have a name, get out
		break;
	/* Check to see if the image needs to be saved */
	lstrcpy( szFileName, lpImage->CurFile );
	if ( !ConfirmClose(NO) )
		break;
	CloseImage(NO);
	AstralImageLoad( szFileName );
	break;

    case IDM_RECALLIMAGE0:
    case IDM_RECALLIMAGE1:
    case IDM_RECALLIMAGE2:
    case IDM_RECALLIMAGE3:
    case IDM_RECALLIMAGE4:
	GetMenuString( GetMenu(hWndAstral), wParam,
		szFileName, sizeof(szFileName), MF_BYCOMMAND );
	AstralImageLoad( szFileName );
	break;

    case IDM_GRAB:
	grabber(Control.FGPort, hInstAstral, hWndAstral);
	break;

    case IDM_GRABSETUP:
	AstralDlg( NO, hInstAstral, hWindow, IDD_GRABSETUP, DlgGrabSetupProc );
	break;

    case IDM_SCAN:
	/* Check to see if the image needs to be saved */
	if ( !ConfirmClose(YES) )
		break;
	CloseImage(YES);
	Scanner();
	break;

    case IDM_SCANSETUP:
	AstralDlg( NO, hInstAstral, hWindow, IDD_SCANSETUP, DlgScanSetupProc);
	break;

//    case IDM_HANDSCAN:
//	HandScan();
//	break;

    case IDM_PRINT:
	if ( !lpImage )
		break;
	/* Bring up the modal 'Print' box */
	AstralDlg( NO, hInstAstral, hWindow, IDD_PRINT, DlgPrintProc );
	break;

    case IDM_PRINTSETUP:
	/* Bring up the setup dialog box for the active printer */
	AstralDlg( NO, hInstAstral, hWindow, IDD_PRINTSETUP, DlgPrintSetupProc);
	break;

  case IDM_CALIBRATE:
	ret = AstralDlg( NO, hInstAstral, hWindow, IDD_SCANORPRINT,
		DlgScanPrintProc);
	if ( ret == IDCANCEL )
		break;
	if ( ret == IDC_CALIBSCAN )
		{
		AstralDlg( YES, hInstAstral, hWindow, IDD_CALIBRATESCAN,
			DlgCalibrateScanProc);
		}
	else
	if ( ret == IDC_CALIBPRINT )
		{
		AstralDlg( YES, hInstAstral, hWindow, IDD_CALIBRATEPRINT,
			DlgCalibratePrintProc);
		}
	break;

#ifdef UNUSED
  case IDM_CONTROL:
	/* Bring up the Control Panel application */
	if ( hWnd = FindWindow( NULL, "Control Panel" ) )
		{
		if ( IsIconic( hWnd ) )
			ShowWindow( hWnd, SW_NORMAL );
		else	BringWindowToTop( hWnd );
		}
	else	WinExec( "CONTROL.EXE", SW_NORMAL );
	break;

    case IDM_DOS:
	/* Bring up the DOS command module application */
	WinExec( "COMMAND.COM", SW_NORMAL );
	break;

    case IDM_TUTORIAL:
	if ( RemovePlayer() || RemoveRecorder() )
		break;
	/* Check to see if the image needs to be saved */
	if ( !ConfirmClose(YES) )
		break;
	/* Size the application window based on the screen size */
	x = GetSystemMetrics( SM_CXSCREEN );
	y = GetSystemMetrics( SM_CYSCREEN );
	if ( x < 640 || y < 480 )
		{
		Print("Can only run the tutorial with a Windows display driver of 640x480 or larger (yours is %dx%d);  Setup your Windows with the standard VGA driver", x, y );
		break;
		}
	/* Bring up the modal 'File Open' box */
	if ( !DoOpenDlg( hWindow, IDD_TUTORIAL, 0, szFileName,NO))
		break;
	if ( Control.Is30 )
		ControlPanelInfo( CP_SETBORDER, 3, 0L );
	else	ControlPanelInfo( CP_SETBORDER, 5, 0L );
	MoveWindow( hWindow, 1, 1, 640-2, 480-2, TRUE );
	CloseImage(YES);
	if ( ALT )
		{
		Print("Installing the Tutorial Recorder");
		if ( !InstallRecorder( szFileName ) )
			Message( IDS_ETUTORIAL );
		}
	else	{
		if ( !InstallPlayer( szFileName ) )
			Message( IDS_ETUTORIAL );
		}
	break;
#endif

    case IDM_BLEND:
	if ( !lpImage )
		break;
	if ( !CacheInit( NO, YES ) ) // Prepare the undo frame
		break;
	if ( DoBlend () )
		{
		lpImage->dirty = IDS_UNDOBLEND;
		lpImage->fChanged = YES;
		}
	break;

    case IDM_SIZE:
	if ( !lpImage )
		break;
	if ( AstralDlg( NO, hInstAstral, hWindow, IDD_SIZE, DlgProcessProc) )
		{
		lpImage->dirty = IDS_UNDOSIZE;
		lpImage->fChanged = YES;
		}
	break;

    case IDM_ROTATE90:
    case IDM_ROTATE180:
    case IDM_ROTATE270:
	if ( !lpImage )
		break;
	AstralCursor( IDC_WAIT );
	if (RotateImage( 90 * (1 + wParam - IDM_ROTATE90), NO ))
		{
		lpImage->dirty = IDS_UNDOROTATE90 + (wParam - IDM_ROTATE90);
 		lpImage->fChanged = YES;
		}
	AstralCursor( NULL );
	break;

    case IDM_ROTATEANY:
	if ( !lpImage )
		break;
	if ( AstralDlg( NO, hInstAstral, hWindow, IDD_ROTATE, DlgRotateProc) )
		{
		lpImage->dirty = IDS_UNDOROTATEANY;
 		lpImage->fChanged = YES;
		}
	break;

    case IDM_MIRRORX:
    case IDM_MIRRORY:
    case IDM_MIRRORXY:
	if ( !lpImage )
		break;
	if ( !CacheInit( NO, YES ) ) // Prepare the undo frame
		break;
	if ( DoMirror(wParam == IDM_MIRRORX || wParam == IDM_MIRRORXY,
		      wParam == IDM_MIRRORY || wParam == IDM_MIRRORXY) )
		lpImage->dirty = IDS_UNDOMIRRORX + (wParam - IDM_MIRRORX);
	break;

    case IDM_SMOOTH:
	if ( !lpImage )
		break;
	if ( AstralDlg( NO, hInstAstral, hWindow, IDD_SMOOTHFILT, DlgSmoothProc ) )
		{
		mask_rect( &rMask );
		AstralUnionRect(&lpImage->UndoRect, &lpImage->UndoRect, &rMask);
		UpdateImage(&rMask, YES);
		lpImage->dirty = IDS_UNDOSMOOTHFILTER;
		}
	break;

    case IDM_SHARPEN:
	if ( !lpImage )
		break;
	if ( AstralDlg( NO, hInstAstral, hWindow, IDD_SHARPENFILT, DlgSharpProc ) )
		{
		mask_rect( &rMask );
		AstralUnionRect(&lpImage->UndoRect, &lpImage->UndoRect, &rMask);
		UpdateImage(&rMask, YES);
		lpImage->dirty = IDS_UNDOSHARPENFILTER;
		}
	break;

    case IDM_EDGEDETECT:
	if ( !lpImage )
		break;
	if ( AstralDlg( NO, hInstAstral, hWindow, IDD_EDGEDETECTFILT, DlgEdgeProc ) )
		{
		mask_rect( &rMask );
		AstralUnionRect(&lpImage->UndoRect, &lpImage->UndoRect, &rMask);
		UpdateImage(&rMask, YES);
		lpImage->dirty = IDS_UNDOEDGEFILTER;
		}
	break;

    case IDM_SPECIALFX:
	if ( !lpImage )
		break;
	if ( AstralDlg( NO, hInstAstral, hWindow, IDD_SPECIALFXFILT, DlgSpecialProc ) )
		{
		mask_rect( &rMask );
		AstralUnionRect(&lpImage->UndoRect, &lpImage->UndoRect, &rMask);
		UpdateImage(&rMask, YES);
		lpImage->dirty = IDS_UNDOSPECIALFILTER;
		}
	break;

    case IDM_INVERT:
	if ( !lpImage )
		break;
	if ( !CacheInit( NO, YES ) ) // Prepare the undo frame
		break;
	InvertFill();
	lpImage->dirty = IDS_UNDOINVERT;
	lpImage->fChanged = YES;
	break;

    case IDM_UNDO:
	if ( !lpImage )
		break;
	// Use the dirty flag to update the menu "Redo Vignette"
	CacheUndo();
	break;

    case IDM_APPLY:
	if ( !lpImage )
		break;
	if ( !lpImage->dirty )
		break;
	CacheApply();
	break;

    case IDM_CUT:
    case IDM_COPY:
	if ( !lpImage )
		break;
	if ( !(lpFrame = frame_set(NULL)) )
		break;
	if ( !(lpMask = lpFrame->WriteMask) )
		break;
	lstrcpy( szFileName, Control.PouchPath );
	lstrcat( szFileName, "WINCLIP.BMP" );
	if ( !AstralFrameSave( szFileName, lpFrame, &lpMask->rMask,
	     lpImage->DataType, IDC_SAVEBMP, NO))
		break;
	lstrcpy( szFileName, Control.PouchPath );
	lstrcat( szFileName, "WINCLIP.MSK" );
	if ( !WriteMask( szFileName, lpMask, TRUE ) )
		{
		lstrcpy( szFileName, Control.PouchPath );
		lstrcat( szFileName, "WINCLIP.BMP" );
		unlink( szFileName );
		break;
		}
	OpenClipboard(hWindow);
	EmptyClipboard();
	SetClipboardData(CF_DIB, NULL);
	CloseClipboard();
	if ( wParam != IDM_CUT )
		break;

    // else fall through to IDM_DELETE

    case IDM_DELETE:
	if ( !lpImage )
		break;
	if ( !CacheInit( NO, YES ) ) // Prepare the undo frame
		break;
	rgb.red = rgb.green = rgb.blue = 255;
	if ( wParam == IDM_CUT )
		{
		IsOutside = Mask.IsOutside;
		Mask.IsOutside = NO;
		}
	TintFill( &rgb );
	if ( wParam == IDM_CUT )
		Mask.IsOutside = IsOutside;
	lpImage->dirty = IDS_UNDODELETE;
	lpImage->fChanged = YES;
	break;

    case IDM_COPYTOFILE:
	if ( !lpImage )
		break;
	if ( !mask_active() )
		{
		Message( IDS_ENOMASK );
		break;
		}
	AstralDlg( NO, hInstAstral, hWindow, IDD_COPYTOFILE, DlgCopyToFileProc);
	break;

    case IDM_PASTE:
	if ( !lpImage )
		break;
	if ( !(hWnd = GetClipboardOwner()) )
		break;
	GetClassName(hWnd, szClassName, sizeof(szClassName));
	if ( lstrcmp(szClassName, ID_APPL) )
		break;
	lstrcpy( Names.ClipImageFile, Control.PouchPath );
	lstrcat( Names.ClipImageFile, "WINCLIP.BMP" );
	lstrcpy( Names.ClipMaskFile, Control.PouchPath );
	lstrcat( Names.ClipMaskFile, "WINCLIP.MSK" );
	InitPaste();
	UpdateStatusBar( YES, NO, YES, NO );
	break;

    case IDM_PASTEFROMFILE:
	if ( !lpImage )
		break;
	if ( !AstralDlg( NO, hInstAstral, hWndAstral, IDD_PASTEFROMFILE,
	     DlgPasteFromFileProc) )
		break;
	InitPaste();
	UpdateStatusBar( YES, NO, YES, NO );
	break;

    case IDM_PASTEOPTIONS:
	if ( hWnd = AstralDlgGet( IDD_PASTEOPTIONS ) )
		{
		if ( IsWindowVisible( hWnd ) )
			ShowWindow( hWnd, SW_HIDE );
		else
		if ( IsIconic( hWnd ) )
			ShowWindow( hWnd, SW_MINIMIZE );
		else	ShowWindow( hWnd, SW_NORMAL );
		}
	else AstralDlg( YES, hInstAstral, hWindow, IDD_PASTEOPTIONS, DlgPasteProc);
	break;

    case IDM_MAP:
	AstralDlg( NO, hInstAstral, hWindow, IDD_MAP, DlgMapProc );
	break;

    case IDM_CONTBRIT:
	AstralDlg( NO, hInstAstral, hWindow, IDD_CONTBRIT, DlgContBritProc );
	break;

    case IDM_POSTERIZE:
	AstralDlg( NO, hInstAstral, hWindow, IDD_POSTERIZE, DlgPosterizeProc );
	break;

    case IDM_HUE:
	AstralDlg( NO, hInstAstral, hWindow, IDD_HUE, DlgHueProc );
	break;

    case IDM_QUARTERTONE:
	if (!lpImage)
		break;
	AstralDlg( NO, hInstAstral, hWindow, lpImage->Depth == 1 ?
		IDD_GQUARTERTONE : IDD_QUARTERTONE, DlgQuarterProc );
	break;

    case IDM_MASKLOAD:
	if ( !lpImage )
		break;
	AstralDlg( NO, hInstAstral, hWindow, IDD_MASKLOAD, DlgMaskLoadProc );
	Marquee(YES);
	UpdateStatusBar( YES, NO, YES, NO );
	break;

    case IDM_MASKSAVE:
	if ( !lpImage )
		break;
	if ( !mask_rect( &rMask ) )
		{
		Message( IDS_ENOMASK );
		break;
		}
	AstralDlg( NO, hInstAstral, hWindow, IDD_MASKSAVE, DlgMaskSaveProc );
	break;

    case IDM_MASKREMOVE:
	if ( !lpImage )
		break;
	RemoveMask();
	Marquee(YES);
	UpdateStatusBar( YES, NO, YES, NO );
	break;

    case IDM_MASKALL:
	if ( !lpImage )
		break;
	RemoveMask();
	Points[0].x = 0;			// left
	Points[0].y = 0;			// top
	Points[1].x = lpImage->npix - 1;	// right
	Points[1].y = 0;			// top
	Points[2].x = lpImage->npix - 1;	// right
	Points[2].y = lpImage->nlin - 1;	// bottom
	Points[3].x = 0;			// left
	Points[3].y = lpImage->nlin - 1;	// bottom
	i = 4;
	SetNewMask( Points, &i/*nPoints*/, 1/*nShapes*/, NO/*fCircle*/ );
	Marquee(YES);
	UpdateStatusBar( YES, NO, YES, NO );
	break;

    case IDM_CROP:
	if ( !lpImage )
		break;
	if ( ! (lpFrame = frame_set(NULL)))
		break;
	if ( ! (lpMask = lpFrame->WriteMask) )
		{
		Message( IDS_ENOMASK );
		break;
		}
	if ( DoCrop( &lpMask->rMask ) )
		{
		lpImage->dirty = IDS_UNDOCROP;
		lpImage->fChanged = YES;
		}
	break;

    case IDM_NEWWINDOW:
	if (lpImage)
	    NewImageWindow( lpImage, NULL, lpImage->EditFrame, 0, 0, TRUE, IMG_DOCUMENT, NULL );
	break;

    case IDM_RULERS:
	if (!lpImage)
		break;
	if (lpImage->lpDisplay->HasRulers)
		DestroyRulers();
	else
		CreateRulers();
	break;

    case IDM_SCRATCHPAD:
	AstralCursor( IDC_WAIT );
	CreateScratchPad();
	AstralCursor( NULL );
	break;

    case IDM_TOOLSHOW:
	if ( hWnd = AstralDlgGet( IDD_TOOLS ) )
		{
		if ( IsWindowVisible( hWnd ) )
			ShowWindow( hWnd, SW_HIDE );
		else
		if ( IsIconic( hWnd ) )
			ShowWindow( hWnd, SW_MINIMIZE );
		else	ShowWindow( hWnd, SW_NORMAL );
		}
	else AstralDlg( YES, hInstAstral, hWindow, IDD_TOOLS, DlgToolsProc);
	break;

    case IDM_STATUSSHOW:
	if ( hWnd = AstralDlgGet( IDD_STATUS ) )
		{
		if ( IsWindowVisible( hWnd ) )
			ShowWindow( hWnd, SW_HIDE );
		else
		if ( IsIconic( hWnd ) )
			ShowWindow( hWnd, SW_MINIMIZE );
		else	ShowWindow( hWnd, SW_NORMAL );
		}
	else AstralDlg( YES, hInstAstral, hWindow, IDD_STATUS, DlgStatusProc);
	UpdateStatusBar( YES, YES, YES, YES );
	break;

//    case IDM_PALETTESHOW:
//	if ( hWnd = AstralDlgGet( IDD_PALETTE ) )
//		{
//		if ( IsWindowVisible( hWnd ) )
//			ShowWindow( hWnd, SW_HIDE );
//		else
//		if ( IsIconic( hWnd ) )
//			ShowWindow( hWnd, SW_MINIMIZE );
//		else	ShowWindow( hWnd, SW_NORMAL );
//		}
//	else AstralDlg( YES, hInstAstral, hWindow, IDD_PALETTE, DlgPaletteProc);
//	break;
//
//    case IDM_SHIELDSHOW:
//	if ( hWnd = AstralDlgGet( IDD_SHIELDFLOAT ) )
//		{
//		if ( IsWindowVisible( hWnd ) )
//			ShowWindow( hWnd, SW_HIDE );
//		else
//		if ( IsIconic( hWnd ) )
//			ShowWindow( hWnd, SW_MINIMIZE );
//		else	ShowWindow( hWnd, SW_NORMAL );
//		}
//	else AstralDlg( YES, hInstAstral, hWindow, IDD_SHIELDFLOAT,
//		DlgShieldFloatProc);
//	UpdateStatusBar( NO, NO, NO, YES );
//	break;

    case IDM_ARRANGEICONS:
	SendMessage( hClientAstral, WM_MDIICONARRANGE, 0, 0L );
	break;

    case IDM_CASCADE:
	SendMessage( hClientAstral, WM_MDICASCADE, 0, 0L );
	break;

    case IDM_TILE:
	SendMessage( hClientAstral, WM_MDITILE, 0, 0L );
	break;

    case IDM_CLOSEALL:
	if (ConfirmClose(YES))
		CloseImage(YES);
	break;

    case IDM_BRUSHPRESSUP:
	if ( Retouch.BrushShape == IDC_BRUSHCUSTOM)
		break;
	if ( Retouch.Pressure == IDC_PRESSLIGHT )
		Retouch.Pressure = IDC_PRESSMEDIUM;
	else
	if ( Retouch.Pressure == IDC_PRESSMEDIUM )
		Retouch.Pressure = IDC_PRESSHEAVY;
	else	MessageBeep(0);
	SetupMask();
	if ( Window.idOptionBox )
		CheckRadioButton( AstralDlgGet(Window.idOptionBox),
			IDC_PRESSLIGHT, IDC_PRESSHEAVY, Retouch.Pressure );
	break;

    case IDM_BRUSHPRESSDOWN:
	if ( Retouch.BrushShape == IDC_BRUSHCUSTOM)
		break;
	if ( Retouch.Pressure == IDC_PRESSMEDIUM )
		Retouch.Pressure = IDC_PRESSLIGHT;
	else
	if ( Retouch.Pressure == IDC_PRESSHEAVY )
		Retouch.Pressure = IDC_PRESSMEDIUM;
	else	MessageBeep(0);
	SetupMask();
	if ( Window.idOptionBox )
		CheckRadioButton( AstralDlgGet(Window.idOptionBox),
			IDC_PRESSLIGHT, IDC_PRESSHEAVY, Retouch.Pressure );
	break;

    case IDM_BRUSHUP:
	if ( Retouch.BrushShape == IDC_BRUSHCUSTOM)
		break;
	if (lpImage)
	    DisplayBrush(0, 0, 0, OFF);
	if ( Retouch.BrushSize < MAX_BRUSH_SIZE )
		Retouch.BrushSize++;
	else	MessageBeep(0);
	SetupMask();
	if (lpImage && Window.hCursor == Window.hNullCursor)
		DisplayBrush(lpImage->hWnd, 32767, 32767, ON);
	if ( Window.idOptionBox )
		SetDlgItemInt( AstralDlgGet(Window.idOptionBox),
			IDC_BRUSHSIZE, Retouch.BrushSize, NO );
	break;

    case IDM_BRUSHDOWN:
	if ( Retouch.BrushShape == IDC_BRUSHCUSTOM)
		break;
	if (lpImage)
		DisplayBrush(0, 0, 0, OFF);
	if ( Retouch.BrushSize > 1 )
		Retouch.BrushSize--;
	else	MessageBeep(0);
	SetupMask();
	if (lpImage && Window.hCursor == Window.hNullCursor)
		DisplayBrush(lpImage->hWnd, 32767, 32767, ON);
	if ( Window.idOptionBox )
		SetDlgItemInt( AstralDlgGet(Window.idOptionBox),
			IDC_BRUSHSIZE, Retouch.BrushSize, NO );
	break;

    case IDM_BRUSHSHAPE:
	if (lpImage)
	    DisplayBrush(0, 0, 0, OFF);
	if ( ++Retouch.BrushShape >= IDC_BRUSHCUSTOM )
		Retouch.BrushShape = IDC_BRUSHCIRCLE;
	SetupMask();
	if (lpImage && Window.hCursor == Window.hNullCursor)
		DisplayBrush(lpImage->hWnd, 32767, 32767, ON);
	if ( Window.idOptionBox )
		CheckComboItem( AstralDlgGet(Window.idOptionBox),\
			IDC_BRUSHSHAPE, IDC_BRUSHCIRCLE, IDC_BRUSHCUSTOM,
			Retouch.BrushShape );
	break;

    case IDM_BRUSHSTYLE:
	if ( Retouch.BrushShape == IDC_BRUSHCUSTOM)
		break;
	if (lpImage)
	    DisplayBrush(0, 0, 0, OFF);
	if ( Retouch.BrushStyle == IDC_BRUSHSOLID )
		Retouch.BrushStyle = IDC_BRUSHSCATTER;
	else	Retouch.BrushStyle = IDC_BRUSHSOLID;
	SetupMask();
	if (lpImage && Window.hCursor == Window.hNullCursor)
		DisplayBrush(lpImage->hWnd, 32767, 32767, ON);
	break;

    case IDM_BACKUP:
	if (lpImage)
	    CommandProc(lpImage->hWnd, (long)wParam);
	break;
  
    case IDM_DELETEITEM:
	if (lpImage)
	    CommandProc(lpImage->hWnd, (long)wParam);
	break;

    case IDM_ESCAPE:
	if ( Window.ModalProc)
		if (lpImage && lpImage->hWnd)
    		    ModalDestroyProc( lpImage->hWnd, 1L);
	if ( Window.ToolActive && Window.ToolProc )
		if ( lpImage && lpImage->hWnd )
		    DestroyProc( lpImage->hWnd, 1L );
	break;
	
    case IDM_PREF:
	AstralDlg( NO, hInstAstral, hWindow, IDD_PREF, DlgPrefProc );
	break;

    case IDM_HELP_INDEX:
	/* Bring up the help system at the index */
	Help( HELP_INDEX, 0L );
	break;

    case IDM_HELP_TOOLBOX:
	/* Bring up the help system at toolbox */
	Help( HELP_KEY, (DWORD)(LPSTR)"toolbox" );
	break;

    case IDM_HELP_KEYBOARD:
	/* Bring up the help system at keyboard */
	Help( HELP_KEY, (DWORD)(LPSTR)"keyboard" );
	break;

    case IDM_HELP_COMMANDS:
	/* Bring up the help system at commands */
	Help( HELP_KEY, (DWORD)(LPSTR)"commands" );
	break;

    case IDM_HELP_PROCEDURES:
	/* Initiate context sensitive help */
	fHelp = YES;
	DisplayBrush( 0, 0, 0, OFF );
	SetCursor( LoadCursor( hInstAstral, MAKEINTRESOURCE(ID_HELP) ) );
	break;

    case IDM_HELP_INTRO:
	/* Bring up the help system with help on help */
	Help( HELP_HELPONHELP, 0L );
	break;

    case IDM_FUNC2:
	SendMessage( AstralDlgGet(IDD_TOOLS), WM_COMMAND, IDC_MASK, 2L);
	break;

    case IDM_FUNC3:
	SendMessage( AstralDlgGet(IDD_TOOLS), WM_COMMAND, IDC_RETOUCH, 2L);
	break;

    case IDM_FUNC4:
	SendMessage( AstralDlgGet(IDD_TOOLS), WM_COMMAND, IDC_FILTER, 2L);
	break;

    case IDM_FUNC5:
	SendMessage( AstralDlgGet(IDD_TOOLS), WM_COMMAND, IDC_FILL, 2L);
	break;

    case IDM_FUNC6:
	SendMessage( AstralDlgGet(IDD_TOOLS), WM_COMMAND, IDC_DRAW, 2L);
	break;

    case IDM_FUNC7:
	SendMessage( AstralDlgGet(IDD_TOOLS), WM_COMMAND, IDC_CUSTOMVIEW, 2L);
	break;

    case IDM_FUNC8:
	SendMessage( AstralDlgGet(IDD_TOOLS), WM_COMMAND, IDC_ERASER, 2L);
	break;

    case IDM_FUNC9:
	SendMessage( AstralDlgGet(IDD_TOOLS), WM_COMMAND, IDC_TEXT, 2L);
	break;

    case IDM_FUNC10:
	SendMessage( AstralDlgGet(IDD_TOOLS), WM_COMMAND, IDC_PROBE, 2L);
	break;

    default:
	return;
    }
} /* end AstralCommand */


/***********************************************************************/
VOID AstralCursor( lpCursor )
/***********************************************************************/
/* Displays lpCursor - if 0 is passed, the previous cursor is displayed */
/***********************************************************************/
LPSTR	lpCursor;
{
static HCURSOR hCursor = 0;

// Display the cursor identified by the id
/* If the caller didn't pass a cursor handle, use the previous one */
if ( !lpCursor )
	{
	if ( hCursor != 0 )
		SetCursor( hCursor );
	}
else	hCursor = SetCursor( LoadCursor( NULL, lpCursor ) );
}


/***********************************************************************/
BOOL AstralClockCursor( part, whole )
/***********************************************************************/
int part, whole;
{
int idCursor, clock;
static int last_clock = -1;

if (CANCEL)
	return(TRUE);
clock = FMUL( 12, FGET( part, whole ) );
if ( clock == last_clock )
	return(FALSE);
last_clock = clock;

switch( clock )
    {
    case 0:  idCursor = ID_CLOCK12; break;
    case 1:  idCursor = ID_CLOCK01; break;
    case 2:  idCursor = ID_CLOCK02; break;
    case 3:  idCursor = ID_CLOCK03; break;
    case 4:  idCursor = ID_CLOCK04; break;
    case 5:  idCursor = ID_CLOCK05; break;
    case 6:  idCursor = ID_CLOCK06; break;
    case 7:  idCursor = ID_CLOCK07; break;
    case 8:  idCursor = ID_CLOCK08; break;
    case 9:  idCursor = ID_CLOCK09; break;
    case 10: idCursor = ID_CLOCK10; break;
    case 11: idCursor = ID_CLOCK11; break;
    case 12: idCursor = ID_CLOCK12; break;
    default: idCursor = ID_CLOCK12; break;
    }

// Set the new cursor and show it
SetCursor( LoadCursor( hInstAstral, MAKEINTRESOURCE(idCursor) ) );
return(FALSE);
}

/***********************************************************************/
VOID AstralBeachCursor()
/***********************************************************************/
{
static long LastTime;
static short times;
int idCursor;
long CurrentTime;

CurrentTime = GetCurrentTime();
if ((CurrentTime - LastTime) > 150L)
	{
	LastTime = CurrentTime;
	idCursor = ID_BALL1 + (times++ % 4);
	SetCursor( LoadCursor( hInstAstral, MAKEINTRESOURCE(idCursor) ) );
	}
}

/***********************************************************************/
void SetFileMenus()
/***********************************************************************/
{
int state, i, id, next;
HANDLE hMenu;
static int items[] = { IDM_REVERT, IDM_PRINT, IDM_SAVE, IDM_SAVEAS, IDM_CLOSE, IDM_UNDO, IDM_APPLY, IDM_DELETE, IDM_MAP, IDM_CONTBRIT, IDM_POSTERIZE, IDM_HUE, IDM_QUARTERTONE, IDM_MASKALL, IDM_SIZE, IDM_ROTATE90, IDM_ROTATE180, IDM_ROTATE270, IDM_ROTATEANY, IDM_MIRRORX, IDM_MIRRORY, IDM_MIRRORXY, IDM_INVERT, IDM_SMOOTH, IDM_SHARPEN, IDM_EDGEDETECT, IDM_SPECIALFX, IDM_MASKLOAD, IDM_NEWWINDOW, IDM_ARRANGEICONS, IDM_TILE, IDM_CASCADE, IDM_CLOSEALL };

hMenu = GetMenu( hWndAstral );

if ( lpImage )
	state = MF_ENABLED;
else	state = MF_GRAYED;

for ( i=0; i<sizeof(items)/sizeof(int); i++ )
	EnableMenuItem( hMenu, items[i], state );
if ( !lpImage || lpImage->fUntitled )
	EnableMenuItem( hMenu, IDM_REVERT, MF_GRAYED );

if ( lpImage && lpImage->Depth == 1 )
	EnableMenuItem( hMenu, IDM_HUE, MF_GRAYED );

// Add the list of images to the menu (up to MAX_RECALL at most)
next = Control.RecallNext;
if ( next < 0 || next >= Control.RecallCount )
	next = 0;
for ( i=0; i<Control.RecallCount; i++ )
	{
	id = IDM_RECALLIMAGE0 + i;
	if ( EnableMenuItem( hMenu, id, MF_ENABLED ) >= 0 )
		ModifyMenu( hMenu, id, MF_BYCOMMAND | MF_STRING,
			id, Control.RecallImage[next] );
	else	InsertMenu( hMenu, id-1, MF_BYCOMMAND | MF_STRING,
			id, Control.RecallImage[next] );
	if ( ++next >= Control.RecallCount )
		next = 0;
	}
}


/***********************************************************************/
void SetMaskMenus()
/***********************************************************************/
{
int state, i;
HANDLE hMenu;
static int items[] = { IDM_CUT, IDM_COPY, IDM_COPYTOFILE, IDM_MASKSAVE, IDM_MASKREMOVE, IDM_CROP, IDM_BLEND };

hMenu = GetMenu( hWndAstral );

if ( mask_active() )
	state = MF_ENABLED;
else	state = MF_GRAYED;

for ( i=0; i<sizeof(items)/sizeof(int); i++ )
	EnableMenuItem( hMenu, items[i], state );
}


/***********************************************************************/
void SetUndoMenus()
/***********************************************************************/
{
int state;
HANDLE hMenu;
LPTR lpString;
STRING szString, szFormat;

hMenu = GetMenu( hWndAstral );

AstralStr( (Control.UseApply ? IDS_MANUALAPPLY : IDS_AUTOAPPLY), &lpString );
if ( lpString )
	ModifyMenu( hMenu, IDM_APPLY, MF_BYCOMMAND | MF_STRING, IDM_APPLY,
		lpString);

szFormat[0] = '\0';
if ( AstralStr( IDS_UNDO, &lpString ) )
	lstrcpy( szFormat, lpString );

if ( !lpImage )
	{
	wsprintf(szString, szFormat, (LPTR)"");
	state = MF_GRAYED;
	}
else
if ( !lpImage->dirty )
	{
	wsprintf(szString, szFormat, (LPTR)"");
	state = MF_GRAYED;
	}
else	{
	if ( !lpImage->UndoNotRedo )
	    if ( AstralStr( IDS_REDO, &lpString ) )
 		lstrcpy( szFormat, lpString );

	if ( AstralStr( (Control.UseApply ? IDS_EDITS : lpImage->dirty),
	     &lpString ) )
		{
		wsprintf(szString, szFormat, lpString);
		}
	else
		wsprintf(szString, szFormat, (LPTR)"");

	state = MF_ENABLED;
	}

ModifyMenu( hMenu, IDM_UNDO, MF_BYCOMMAND | MF_STRING, IDM_UNDO,(LPTR)szString);
EnableMenuItem( hMenu, IDM_UNDO, state );
EnableMenuItem( hMenu, IDM_APPLY, ((Control.UseApply && state == MF_ENABLED) ? MF_ENABLED : MF_GRAYED) );
}


/***********************************************************************/
void SetWindowMenus()
/***********************************************************************/
{
int state;
HANDLE hMenu;
LPTR lpString;
STRING szString, szFormat;

hMenu = GetMenu( hWndAstral );
EnableMenuItem( hMenu, IDM_RULERS, lpImage ? MF_ENABLED : MF_GRAYED);
if (lpImage)
	CheckMenuItem(hMenu, IDM_RULERS, lpImage->lpDisplay->HasRulers ? MF_CHECKED : MF_UNCHECKED);
else
	CheckMenuItem(hMenu, IDM_RULERS, MF_UNCHECKED);
//SetShowHideMenus( IDM_PALETTESHOW, IDD_PALETTE );
//SetShowHideMenus( IDM_SHIELDSHOW, IDD_SHIELDFLOAT );
SetShowHideMenus( IDM_TOOLSHOW, IDD_TOOLS );
SetShowHideMenus( IDM_STATUSSHOW, IDD_STATUS );
SetShowHideMenus( IDM_PASTEOPTIONS, IDD_PASTEOPTIONS );
}


/***********************************************************************/
void SetShowHideMenus( idMenu, idDialogBox )
/***********************************************************************/
WORD idMenu, idDialogBox;
{
HANDLE hMenu;
LPTR lpString;
BYTE szString[MAX_STR_LEN];
BYTE szString2[MAX_STR_LEN];
WORD idString;
HWND hWnd;

hMenu = GetMenu( hWndAstral );

if ( hWnd = AstralDlgGet( idDialogBox ) )
	{
	if ( IsWindowVisible( hWnd ) )
		idString = IDS_HIDE;
	else	idString = IDS_SHOW;
	}
else	idString = IDS_SHOW;

szString[0] = '\0';
if ( AstralStr( idString, &lpString ) )
	lstrcpy( szString, lpString );
GetMenuString( hMenu, idMenu, szString2, sizeof(szString2), MF_BYCOMMAND );
lpString = szString2;
while ( *lpString != ' ' )
	lpString++;
lstrcat( szString, lpString );
ModifyMenu( hMenu, idMenu, MF_BYCOMMAND | MF_STRING, idMenu, (LPTR)szString );
}


/***********************************************************************/
void SetPasteMenus()
/***********************************************************************/
{
int state;
HANDLE hMenu;

hMenu = GetMenu( hWndAstral );

if ( !lpImage )
	state = MF_GRAYED;
else	state = MF_ENABLED;
EnableMenuItem( hMenu, IDM_PASTEFROMFILE, state );

if ( state == MF_ENABLED && OpenClipboard(hWndAstral) )
	{
	if ( !IsClipboardFormatAvailable(CF_DIB) &&
	     !IsClipboardFormatAvailable(CF_BITMAP))
		state = MF_GRAYED;
	CloseClipboard();
	}

EnableMenuItem( hMenu, IDM_PASTE, state );
}


/***********************************************************************/
BOOL NewImageWindow( lpOldImage, szName, lpFrame, FileType, fSingleBit, fNewView, ImageType, ImageName )
/***********************************************************************/
LPIMAGE lpOldImage;
LPTR szName;
LPFRAME lpFrame;
int FileType;
BOOL fSingleBit;
BOOL fNewView;
int ImageType;
LPTR ImageName;
{
int cx, cy;
MDICREATESTRUCT mcs;
DWORD dXY;

if ( !lpFrame )
	return( FALSE );

lpImage = lpOldImage;
if ( !lpImage || fNewView ) // If this a really a new image...
	{ // Allocate and clear a new image structure
	if (!fNewView)
	    {
	    if ( !(lpImage = (LPIMAGE)Alloc( (long)sizeof(IMAGE) )) )
		return( FALSE );
	    clr( (LPTR)lpImage, sizeof(IMAGE) );
	    ResetAllMaps();
	    }
	if ( !(lpImage->lpDisplay = (LPDISPLAY)Alloc( (long)sizeof(DISPLAY) )) )
		return( FALSE );
	clr ((LPTR)lpImage->lpDisplay, sizeof(DISPLAY));
	}

frame_set( lpImage->EditFrame = lpFrame );
if (!fNewView)
    {
    if ( !szName )
	{
	lpImage->fUntitled = TRUE;
	lstrcpy( lpImage->CurFile, GetUntitledName() );
	}
    else	
	lstrcpy( lpImage->CurFile, szName );

    if ( lpFrame->Depth == 1 )
	    lpImage->hPal = BltScreen.hGrayPal;
    else    lpImage->hPal = BltScreen.hColorPal;

    if (lpImage->hPal)
	lpImage->nPaletteEntries = GetPaletteEntries( lpImage->hPal, 0, 256,
				lpImage->Palette );
    if ( fSingleBit )
	lpImage->DataType = IDC_SAVELA;
    else	
 	lpImage->DataType = ( lpFrame->Depth == 1 ? IDC_SAVECT
						  : IDC_SAVE24BITCOLOR );
    lpImage->fChanged  = NO;
    lpImage->Depth     = lpFrame->Depth;
    lpImage->xres      = lpFrame->Resolution;
    lpImage->yres      = lpFrame->Resolution;
    lpImage->npix      = lpFrame->Xsize;
    lpImage->nlin      = lpFrame->Ysize;
    lpImage->FileType  = FileType;
    SetRect( &lpImage->UndoRect, 30000, 30000, -30000, -30000 );
    lpImage->ImageType = ImageType;
    if (ImageName)
	lstrcpy(lpImage->ImageName, ImageName);
    }

lpImage->lpDisplay->FileRate = 0;
SetRect( &lpImage->lpDisplay->FileRect, 0,0,lpFrame->Xsize-1, lpFrame->Ysize-1);
SetRect( &lpImage->lpDisplay->UpdateRect, 32767, 32767, -32767, -32767 );
lpImage->lpDisplay->HasRulers = NO;
Edit.Crop = lpImage->lpDisplay->FileRect;
if ( lpOldImage && !fNewView)
	{ // Resize and redisplay the window
	SaveLastView();
	dXY = GetWinXY(lpImage->hWnd);
	cx = (lpImage->lpDisplay->FileRect.left + 
	      lpImage->lpDisplay->FileRect.right)/2;
	cy = (lpImage->lpDisplay->FileRect.top + 
	      lpImage->lpDisplay->FileRect.bottom)/2;
	FullViewWindow(lpImage->hWnd, LOWORD(dXY), HIWORD(dXY), cx, cy, YES);
	}
else	{
	mcs.szTitle = (LPSTR)"";
	mcs.szClass = (LPSTR)"ppimage";
	mcs.hOwner = hInstAstral;
	mcs.x = 0;
	mcs.y = 0;
	mcs.cx = 100;
	mcs.cy = 100;

	/* Set the style DWORD of the window to default */
	mcs.style =
		WS_VISIBLE |
		WS_CHILD |
		WS_CLIPSIBLINGS |
		WS_CLIPCHILDREN |
		WS_BORDER |
		WS_THICKFRAME |
		WS_CAPTION |
		WS_SYSMENU |
		WS_MINIMIZEBOX |
		WS_MAXIMIZEBOX |
		WS_VSCROLL |
		WS_HSCROLL;

	/* tell the MDI Client to create the child */
	SendMessage (hClientAstral, WM_MDICREATE, 0,
		(LONG)(LPMDICREATESTRUCT)&mcs);
	}

return( TRUE );
}


/***********************************************************************/
void CloseImage(fCloseAll)
/***********************************************************************/
BOOL fCloseAll;
{
int i;
HWND hWnd, hTheWnd, hOldWnd;
LPIMAGE lpTheImage;
BOOL fMultipleViews;

if ( !lpImage )
	return;

// Destroy the image window
if (fCloseAll)
    {
    while (NumDocs() > 0)
	{
	hWnd = GetDoc(0);
	if (SetCurView(hWnd, NULL))
	    {
	    lpTheImage = lpImage;
	    for (i = 0; i < NumDocs();)
		{
		hWnd = GetDoc(i);
		if (SetCurView(hWnd, NULL) && lpImage == lpTheImage)
		    {
		    SendMessage(hClientAstral, WM_MDIDESTROY, (WORD) hWnd, 0L);
		    i = 0;
		    }
		else
 		    ++i;
		}
	    CacheFree(lpTheImage);
	    FreeUp((LPTR)lpTheImage);
	    }
	}
    }
else
    {
    lpTheImage = lpImage;
    hTheWnd = lpImage->hWnd;
    fMultipleViews = FALSE;
    for (i = 0; i < NumDocs(); ++i)
	{
	hWnd = GetDoc(i);
	if (SetCurView(hWnd, &hOldWnd) && lpImage == lpTheImage && hWnd != hTheWnd)
	    {
	    fMultipleViews = TRUE;
	    SetCurView(hOldWnd, NULL);
	    break;
	    }
	SetCurView(hOldWnd, NULL);
	}
    SendMessage(hClientAstral, WM_MDIDESTROY, (WORD) lpImage->hWnd, 0L);
    if (!fMultipleViews)
	{
	CacheFree(lpTheImage);
	FreeUp((LPTR)lpTheImage);
	}
    }
UpdateStatusBar( YES, YES, YES, NO );
}

/***********************************************************************/
BOOL ConfirmClose(fCloseAll)
/***********************************************************************/
BOOL fCloseAll;
{
int	ret, i, j;
LPIMAGE lpTheImage;
HWND hWnd, hSaveWnd, hTheWnd, hOldWnd;

if (!lpImage)
    return(YES);

if (fCloseAll)
    {
    hSaveWnd = lpImage->hWnd;
    /* this outside loop steps through each document window */
    for (i = 0; i < NumDocs(); ++i)
	{
	hWnd = GetDoc(i);
	/* if the image has changed, check to see if we should do confirm */
	/* we only do confirms for the first occurence of the image in the */
	/* document list. */
	if (SetCurView(hWnd, NULL) && lpImage->fChanged)
	    {
	    lpTheImage = lpImage;
	    /* find the first occurrence of this lpImage */
	    for (j = 0; j < NumDocs(); ++j)
		{
		hWnd = GetDoc(j);
		if (SetCurView(hWnd, NULL))
		    {
		    if (lpImage == lpTheImage)
			{
			if (i == j) /* only confirm on first one in list */
			    {	    /* ignore others */
	    		    /* Check to see if the image needs to be saved */
			    ret = AstralConfirm( IDS_AREYOUSURE, (LPTR)lpImage->CurFile );
			    if ( ret == IDCANCEL )
				{
				SetCurView(hSaveWnd, NULL);
				return( NO );
				}
			    if ( ret == IDYES )
		    		AstralCommand( hWndAstral, IDM_SAVE );
	  		    }
			break; // go onto next document in doc list
			}
		    }
	 	}
	    }
	}
    SetCurView(hSaveWnd, NULL);
    }
else
    {
    /* check to see if another view exists */
    hTheWnd = lpImage->hWnd;
    lpTheImage = lpImage;
    for (i = 0; i < NumDocs(); ++i)
	{
	hWnd = GetDoc(i);
	if (SetCurView(hWnd, &hOldWnd))
	    if (lpImage == lpTheImage && hWnd != hTheWnd)
		{
		SetCurView(hOldWnd, NULL);
		return(YES);
		}
	SetCurView(hOldWnd, NULL);
	}
    /* Check to see if the image needs to be saved */
    if ( lpImage->fChanged )
	{
	ret = AstralConfirm( IDS_AREYOUSURE, (LPTR)lpImage->CurFile );
	if ( ret == IDCANCEL )
	    return( NO );
	if ( ret == IDYES )
	    AstralCommand( hWndAstral, IDM_SAVE );
	}
    }
return( YES );
}

/***********************************************************************/
LPSTR GetUntitledName()
/***********************************************************************/
{
char num[10];

LoadString(hInstAstral, IDS_UNTITLED, szUntitled, MAX_STR_LEN);
itoa(++Control.UntitledNo, num, 10);
lstrcat((LPTR)szUntitled, (LPTR)num);
return((LPSTR)szUntitled);
}

/***********************************************************************/
BOOL OKToOpen(lpFile, lppName)
/***********************************************************************/
LPTR lpFile;
LPPTR lppName;
{
HWND hWnd, hOldWnd;
int ret, i;

for (i = 0; i < NumDocs(); ++i)
    {
    hWnd = GetDoc(i);
    if (SetCurView(hWnd, &hOldWnd))
	{
	if (StringsEqual(lpFile, (LPTR)lpImage->CurFile))
	    {
	    ret = AstralOKCancel( IDS_OKTOOPEN, (LPTR)lpImage->CurFile );
	    if ( ret == IDCANCEL )
		{
		SetCurView(hOldWnd, NULL);
		return( NO );
		}
	    if ( ret == IDOK )
		{
		SetCurView(hOldWnd, NULL);
		*lppName = NULL;
		return(YES);
		}
	    }
	} 
    SetCurView(hOldWnd, NULL);
    }
*lppName = lpFile;
return(YES);
}


/***********************************************************************/
BOOL FAR PASCAL MessageHookProc( nCode, Dummy, lpMsg )
/***********************************************************************/
/*  Process the F1 key during a message box or a popup, and it is not a HELP
    popup. Send the WM_COMMAND/HELP_TOPIC command to the the main window. */
int nCode;
WORD Dummy;
LPMSG lpMsg;
{
BOOL bResult, bMenu, bShift;

if ( nCode < 0 )
	return( DefHookProc( nCode, Dummy, (DWORD)lpMsg, &lpPrevMsgHook ) );

if ( !fHelp && lpMsg->message == WM_KEYDOWN && lpMsg->wParam == VK_F12 )
	{
	fHelp = YES;
	DisplayBrush( 0, 0, 0, OFF );
	SetCursor( LoadCursor( hInstAstral, MAKEINTRESOURCE(ID_HELP) ) );
	return( TRUE );
	}

if ( ( nCode == MSGF_DIALOGBOX || nCode == MSGF_MESSAGEBOX )
   && lpMsg->message == WM_SETCURSOR )
	{
	MessageBeep(0);
	lpMsg->wParam = hWndAstral;
	}

if ( fHelp )
	{
	if ( lpMsg->message == WM_LBUTTONDBLCLK || 
	   ( lpMsg->message == WM_KEYUP && lpMsg->wParam == VK_ESCAPE ) )
		{
		fHelp = NO;
		SetCursor( LoadCursor( NULL, IDC_ARROW ) );
		return( TRUE );
		}
	bShift = SHIFT;
	if ( lpMsg->message == WM_COMMAND && !bShift )
		return( TRUE ); // ignore it
	if ( lpMsg->message == WM_LBUTTONDOWN && !bShift )
		return( TRUE ); // ignore it
	if ( lpMsg->message == WM_LBUTTONUP || 
	   ( lpMsg->message == WM_KEYUP && lpMsg->wParam == VK_RETURN ) )
		{
//		fHelp = NO;
//		Print("Help on %d", idLastWindow );
		if ( idLastWindow > 0 )
//			Help( HELP_CONTEXT, (long)idLastWindow );
			Help( HELP_INDEX, 0L );
//		SetCursor( LoadCursor( NULL, IDC_ARROW ) );
//		return( TRUE );
		}
	}

return( DefHookProc( nCode, Dummy, (DWORD)lpMsg, &lpPrevMsgHook ) );

if (bResult)
	{
	fHelp = YES;
	if ( bMenu = (nCode == MSGF_MENU) )
		{
		PostMessage( lpMsg->hwnd, WM_KEYDOWN, VK_ESCAPE, 0L );
//		PostMessage( hMainWindow, WM_MENUSELECT, HELP_MENU,
//			(DWORD)MenuHelp );
		}
//	Print("Help on %d", GetWindowWord(hLastWindow,GWW_ID) );
//	fHelp = NO;
//	Help( HELP_CONTEXT, 0L );
//	PostMessage( hWndAstral, WM_COMMAND, bMenu ? HELP_MENU : HELP_TOPIC,0L);
	}
else	{
	bResult = (BOOL)DefHookProc( nCode, Dummy, (DWORD)lpMsg,
		&lpPrevMsgHook );
	}

return (bResult);
}


#ifdef UNUSED
/***********************************************************************/
BOOL FAR PASCAL MessageHookProc( nCode, Dummy, lpMsg )
/***********************************************************************/
/*  Process the F1 key during a message box or a popup, and it is not a HELP
    popup. Send the WM_COMMAND/HELP_TOPIC command to the the main window. */
int nCode;
WORD Dummy;
LPMSG lpMsg;
{
BOOL bResult, bMenu;

bResult = nCode >= 0 && lpMsg->message == WM_KEYDOWN && lpMsg->wParam == VK_F12;

if (bResult)
	{
	fHelp = YES;
	if ( bMenu = (nCode == MSGF_MENU) )
		{
		PostMessage( lpMsg->hwnd, WM_KEYDOWN, VK_ESCAPE, 0L );
//		PostMessage( hMainWindow, WM_MENUSELECT, HELP_MENU,
//			(DWORD)MenuHelp );
		}
	Print("Help on %d", GetWindowWord(hLastWindow,GWW_ID) );
	fHelp = NO;
//	Help( HELP_CONTEXT, 0L );
//	PostMessage( hWndAstral, WM_COMMAND, bMenu ? HELP_MENU : HELP_TOPIC,0L);
	}
else	{
//	LockData(0);
	bResult = (BOOL)DefHookProc( nCode, Dummy, (DWORD)lpMsg,
		&lpPrevMsgHook );
//	UnlockData(0);
	}

return (bResult);
}
#endif
