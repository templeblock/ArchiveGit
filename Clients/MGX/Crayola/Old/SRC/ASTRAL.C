//®PL1¯®FD1¯®BT0¯®TP0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/************************************************************************/
/* Astral.c    Astral picture publishing application			*/
/************************************************************************/
#define MAIN

#include <windows.h>
#include <shellapi.h>
#include <time.h>
#include "types.h"
#include "id.h"
#include "data.h"
#include "routines.h"
#include "ppole.h"
#include "mmsystem.h"
#include "animate.h"

// Static prototypes
static int AstralInit( HINSTANCE hInstance );
static void SetFileMenus( void );
static HCURSOR hDialogCursor;
//static void PlayCDOpening();
static LPAOBJECT PlaySimpleOpening(HWND hWindow);

#define EXE_NAME_MAX_SIZE 128
#define COLOR_PATCH_CLASS 1
#define RIBBON_ICON_CLASS 2

// instance for add-on room DLL's
extern	HINSTANCE	hCurrentDLL;

/************************************************************************/
/* The global handles							*/
/************************************************************************/
HINSTANCE	hInstAstral;
HWND	hWndAstral;
HWND	hClientAstral;
HWND 	hActiveWindow;
HACCEL	hAccelTable;
BOOL	fAppActive;

/************************************************************************/
/* Other global data							*/
/************************************************************************/
char szAppName[MAX_STR_LEN];
char szUntitled[MAX_STR_LEN];

//#ifdef CDBUILD
//BOOL	bFunkyOpening;
//#endif

/***********************************************************************/
int WINPROC WinMain( HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpszCmdLine, int cmdShow )
/***********************************************************************/
{
MSG msg;
WORD wHeight, wWidth;
HWND hWindow, hWnd, hDlg;
FNAME szFileName;
int i, iMenuHeight, x, y;
HDC hDC;
HBITMAP hBitmap;
WORD Pattern[16];
LPSTR lp;
BOOL bPrint;
OLESTATUS os;
LPAOBJECT Asession;
time_t __cdecl time(time_t *);
void __cdecl srand(unsigned int);
int yPos;
bool bLButton = FALSE;
int iColorBits;

// Seed the random number generator
srand((unsigned)time(NULL));

// Allocate the Linebuffer buffers (used for scanning as well)
if ( !(Control.lpSegment = GetBuffers16Kx4( (LPPTR)LineBuffer, YES )) )
	{
	if ( !(Control.lpSegment = GetBuffers16Kx4( (LPPTR)LineBuffer, NO )) )
		{
		Message( IDS_EMEMALLOC );
		return( FALSE );
		}
	}

// Allocate the SBLTLinebuffer buffers (used for scanning as well)
if (!(SBLTLineBuffer[0]  = (LPSTR)Alloc(65536L)))
	{
	Message(IDS_EMEMALLOC);
	return(FALSE);
	}
SBLTLineBuffer[1] = SBLTLineBuffer[0] + 16384;
SBLTLineBuffer[2] = SBLTLineBuffer[1] + 16384;
SBLTLineBuffer[3] = SBLTLineBuffer[2] + 16384;

/* Set the global instance handle */
hInstAstral = hInstance;

/* Get the path of the file being executed (needed by preferences) */
#ifdef _MAC
lstrcpy(Control.ProgHome, "");
#else
GetModuleFileName( hInstance, Control.ProgHome, sizeof(Control.ProgHome)-1);
stripfile( Control.ProgHome );
FixPath( Control.ProgHome );

// this will be different for CD-ROM
lstrcpy (Control.WorkPath, Control.ProgHome);

#endif // _MAC

/************************************************************************/
/* CD-ROM setup                                                         */
/************************************************************************/
#ifdef CDBUILD
if (GetDefaultInt ("CdInit",0) == 0)
{
    char szSource[256];
    char szDestin[256];
    char szWindows[256];
    HWND hProgMgr = NULL;
    int  i;

	// if they click on the locked program, bypass this stuff
	if (GetLockType() != 1)
	{
    	// alert the user
    	MessageBox (HWND_DESKTOP,
        	GetString (IDS_CD_MSGALERT,szSource),
        	GetString (IDS_CD_MSGTITLE,szDestin),MB_OK);

    	// setup windows directory
    	GetWindowsDirectory (szWindows,sizeof szWindows);
    	OemToAnsi (szWindows,szWindows);

    	// setup new ini file
    	PathCat (szSource,Control.ProgHome,"CRAYOLA.INI");
    	PathCat (szDestin,szWindows,"CRAYOLA.INI");
    	CopyFile (szSource,szDestin);
    	ResetIniPath();

    	// setup wavemix ini file
    	PathCat (szSource,Control.ProgHome,"WAVEMIX.INI");
    	PathCat (szDestin,szWindows,"WAVEMIX.INI");
    	CopyFile (szSource,szDestin);

    	// setup work directory
    	PathCat (Control.WorkPath,szWindows,"CRAYOLA");
    	PutDefStr (Control.WorkPath,Control.WorkPath);
    	_mkdir (Control.WorkPath);
    	FixPath (Control.WorkPath);

    	// setup launch utility
    	PathCat (szSource,Control.ProgHome,"LAUNCH.EXE");
    	PathCat (szDestin,Control.WorkPath,"LAUNCH.EXE"); // szDestin used below!
    	CopyFile (szSource,szDestin);

    	// add icons to frogman
    	create_dde_window (hInstance);
    	if (init_progmgr_session (&hProgMgr))
    	{
        	char szApp[256];
        	char szCmdLine[256];
        	char szGroup[256];
        	char szIconText[256];
        	int nIcon;

        	// setup group
        	GetString (IDS_CD_APPGROUP,szGroup);
        	add_progmgr_group (hProgMgr,szGroup);
        	shw_progmgr_group (hProgMgr,szGroup,1);

        	// setup primary application icon
        	GetString (IDS_CD_APPICON,szIconText);
    	#ifdef ADVENT
        	PathCat (szApp,Control.ProgHome,"ADVENTUR.EXE");
        	nIcon = 0;
    	#else
        	PathCat (szApp,Control.ProgHome,"STUDIO.EXE");
        	nIcon = 1;
    	#endif
        	lstrcpy (szCmdLine,szDestin);
        	lstrcat (szCmdLine," ");
        	lstrcat (szCmdLine,szApp);
        	add_progmgr_item (hProgMgr,szIconText,szCmdLine,szDestin,nIcon);

        	// setup secondary (locked) application icon
        	GetString (IDS_CD_APPOFFER,szIconText);
    	#ifdef ADVENT
        	PathCat (szApp,Control.ProgHome,"STUDIO.EXE");
        	nIcon = 1;
    	#else
        	PathCat (szApp,Control.ProgHome,"ADVENTUR.EXE");
        	nIcon = 0;
    	#endif
        	lstrcpy (szCmdLine,szDestin);
        	lstrcat (szCmdLine," ");
        	lstrcat (szCmdLine,szApp);
        	add_progmgr_item (hProgMgr,szIconText,szCmdLine,szDestin,nIcon);

        	// setup demo icon
        	GetString (IDS_CD_DEMOTEXT,szIconText);
        	PathUp (szApp,Control.ProgHome);
        	PathCat (szApp,szApp,"DEMO\\ARTDEMO.EXE intro01");
        	nIcon = 2;
        	lstrcpy (szCmdLine,szDestin);
        	lstrcat (szCmdLine," /D "); // the "DOS" switch!
        	lstrcat (szCmdLine,szApp);
        	add_progmgr_item (hProgMgr,szIconText,szCmdLine,szDestin,nIcon);

        	// setup video clip icons
        	PathUp (szSource,Control.ProgHome);
        	PathCat (szSource,szSource,"VIDEOS\\CRAYOLA%i.AVI");
        	for (i = 0; i < 5; i++)
        	{
            	wsprintf (szApp,szSource,i + 1);
            	GetString ((int)(IDS_CD_AVIICON1 + i),szIconText);
            	nIcon = 3 + i; // icons are contiguous starting at 3
            	lstrcpy (szCmdLine,szDestin);
            	lstrcat (szCmdLine," ");
            	lstrcat (szCmdLine,szApp);
            	add_progmgr_item (hProgMgr,szIconText,szCmdLine,szDestin,nIcon);
        	}

//        	// setup rules for safety icon
//        	GetString (IDS_CD_SAFETY,szIconText);
//        	PathUp (szApp,Control.ProgHome);
//        	PathCat (szApp,szApp,"SAFETY\\8RULES.EXE");
//        	nIcon = 4;
//        	lstrcpy (szCmdLine,szDestin);
//        	lstrcat (szCmdLine," ");
//        	lstrcat (szCmdLine,szApp);
//        	add_progmgr_item (hProgMgr,szIconText,szCmdLine,szDestin,nIcon);

        	// end frogman session
        	term_progmgr_session (hProgMgr,FALSE);
    	}
    	destroy_dde_window (hInstance);
    	PutDefaultInt ("CdInit",1);
	}
}
else
{
    GetDefStr (Control.WorkPath,Control.WorkPath);
    FixPath (Control.WorkPath);
}
#endif
/************************************************************************/
/* end of CD-ROM setup                                                  */
/************************************************************************/

Control.Retail = AstralStr( IDS_RETAIL, (LPSTR FAR *)&lp ); // Is this retail?

/************************************************************************/
/* Initialization for the 1st instance					*/
/************************************************************************/

// Only allow a single instance of the app
if ( hWnd = FindWindow( (LPCSTR)ID_APPCLASS, NULL ) )
	{
	if ( IsIconic( hWnd ) )
			ShowWindow( hWnd, SW_NORMAL );
	else	BringWindowToTop( hWnd );
	SendMessage( hWnd, WM_COMMAND, IDM_ALTOPEN, (long)lpszCmdLine );
	return( FALSE );
	}

/************************************************************************/
/* Initialization for all instances						*/
/************************************************************************/

if ( !AstralInit( hInstance ) )
	return( FALSE );

// Initialize the app as an OLE server
if ( !PPOLEInstanceInit( hInstance, lpszCmdLine, (LPWORD)&cmdShow ) )
	{
	PPOLEExit();
	return( FALSE );
	}

/* Register our own external clipboard data format */
Control.cfImage = RegisterClipboardFormat("MGX_IMAGE");

/* Load strings from the string table */
LoadString(hInstAstral, IDS_APPNAME, szAppName, MAX_STR_LEN);

/* Get the handles to the keyboard accelerators */
hAccelTable = LoadAccelerators( hInstAstral, MAKEINTRESOURCE( ID_ACCEL ) );

/* Size the application window based on the screen size */
wWidth = GetSystemMetrics( SM_CXSCREEN );
wHeight = GetSystemMetrics( SM_CYSCREEN );
iMenuHeight = GetSystemMetrics( SM_CYMENU );
if ( cmdShow == SW_MAXIMIZE )
	cmdShow = SW_NORMAL;

#ifdef _MAC	
yPos = iMenuHeight - 1;
wHeight += 3;	// cover the bottom of the screen
#else
yPos = -iMenuHeight - 1;
wHeight += iMenuHeight + 1;
#endif	

/* Create the main application window */
if ( !(hWindow = CreateWindow(
	ID_APPCLASS,	/** lpClassName **/
	NULL,			/** lpWindowName **/
	WS_POPUP | // with WS_OVERLAPPED you can't make window bigger than screen
	WS_MINIMIZEBOX |
	WS_CLIPCHILDREN, 				/** dwStyle **/
	0, yPos,						/** position of window **/
	wWidth, wHeight,				/** size of window **/
	NULL,							/** hWindowParent (null for tiled) **/
	NULL,							/** hMenu (null = classmenu) **/
	hInstAstral,					/** hInstance **/
	NULL							/** lpParam **/
	)) )
		{
		PPOLEExit();
		return( FALSE );
		}

/* Set the global application window handle */
hWndAstral = hWindow;

// Setup all the Windows pens, brushes, and fonts
Window.hShadowBrush = CreateSolidBrush( GetSysColor(COLOR_BTNSHADOW) );
Window.hButtonBrush = CreateSolidBrush( GetSysColor(COLOR_BTNFACE) );
for ( i=0; i<16; i+=2 )
	Pattern[i] = 0xAAAA;
for ( i=1; i<16; i+=2 )
	Pattern[i] = 0x5555;
hBitmap = CreateBitmap( 16, 16, 1, 1, (LPTR)Pattern );
Window.hHilightBrush = CreatePatternBrush( hBitmap );
DeleteObject( hBitmap );
Window.hShadowPen = CreatePen( PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
Window.hButtonPen = CreatePen( PS_SOLID, 1, GetSysColor(COLOR_BTNFACE));
Window.iLabelFont = GetDefaultInt( "LabelSet", ANSI_VAR_FONT );

// Allocate the brush settings buffer
lpBrushSettings = (LPBRUSHSETTINGS)AllocX((long) (sizeof(BRUSHSETTINGS) *
								 TOTAL_BRUSHES), GMEM_ZEROINIT);
if ( !lpBrushSettings )
	{
	Message( IDS_EMEMALLOC );
	PPOLEExit();
	return(0);
	}

if (!SetupBrushBuffers(YES))
	{
	Message( IDS_EMEMALLOC );
	PPOLEExit();
	return(0);
	}

ResetAllMaps();

/* Initialize all of the palettes and map structures */
hDC = GetDC( hWndAstral );
lpBltScreen = InitSuperBlt( hDC, NULL, (LPTR)SBLTLineBuffer[0], YES, YES );
ReleaseDC( hWndAstral, hDC );
if (!lpBltScreen)
	{
	Message( IDS_EMEMALLOC );
	PPOLEExit();
	return(0);
	}

/* Put the application's name up in lights! */
SetWindowText( hWindow, szAppName );

/* Display the application window */
ShowWindow( hWindow, cmdShow );
if ( cmdShow != SW_HIDE )
	UpdateWindow( hWindow );

// Check to see if this is a locked CD app
if ( GetLockType() == 1 )
	if ( !CheckRegistration(YES) )
		return( FALSE );

// Used to set the height of combo boxes to be the same as edit boxes
AstralDlg( YES, hInstAstral, hWndAstral, IDD_COMBO, (DLGPROC)DlgComboProc );

lstrcpy( szFileName, "Crayola" );
#ifndef _MAC
if ( !GetProfileInt( szFileName, "Warning", 0 ) )
	{
	AstralDlg( NO, hInstAstral, hWndAstral, IDD_WARNING, (DLGPROC)DlgWarningProc );
	WriteProfileString( szFileName, "Warning", "1" );
	}
#endif // _MAC	

/* Load the global data structures with memory related info */
LoadAllPreferences(YES);
SetSuperBltGamma(lpBltScreen, BltSettings.RGamma, BltSettings.GGamma,
	 BltSettings.BGamma);

/* Load the global data structures with any default values */
LoadAllPreferences(NO);

// Open the sound DLL - preferences must be loaded before this
SoundOpen (GetString(IDS_SOUNDDLL,NULL));

Window.fHasZoom = GetDefaultInt( "upZoom", NO );
hZoomWindow = NULL;

// See if this is a 30 day demo
Control.Crippled = NO;
if ( Bomb( hInstance ) )
	{
	PPOLEExit();
	return( FALSE );
	}

if (Control.UseWacom)
	Control.UseWacom = WacStart();

// Play the opening theme
if ( lpszCmdLine && *lpszCmdLine )
	Control.bNoTheme = YES;

Asession = NULL;

//#ifdef CDBUILD
//	hDC = GetDC(hWindow);
//	iColorBits = GetDeviceCaps(hDC, BITSPIXEL);
//	ReleaseDC(hWindow, hDC);
//	if (iColorBits > 8)	// greater than 265 colors screws up.
//	{
//		Asession = PlaySimpleOpening(hWindow);
//		bFunkyOpening = FALSE;
//		bLButton = LBUTTON;
//	}
//	else
//	{
//		PlayCDOpening();
//		bFunkyOpening = TRUE;
//	}
//#else
	Asession = PlaySimpleOpening(hWindow);
	bLButton = LBUTTON;
//#endif // #ifdef CDBUILD

// Bring up the paint screen; initially invisible
AstralDlg( -1/*Modeless on bottom*/, hInstAstral, hWindow, IDD_MAIN,
	(DLGPROC)DlgMainProc );
if ( hDlg = AstralDlgGet( IDD_MAIN ) )
	hDialogCursor = (HCURSOR)SET_CLASS_CURSOR( hDlg,
		LoadCursor( hInstance, MAKEINTRESOURCE(ID_HAND01) ) );

if ( Asession )
	{
	if ( !bLButton )		// This keeps the screen intact when the animations are deleted
		ACopyBackgroundScreen( Asession, hWindow, NULL );
	ADeleteAllObjects( Asession );
	ADeleteAnimator( Asession );
	}

// If the caller passed in a file name, try to load it or print it
HandleCommandLine( hWindow, lpszCmdLine, &bPrint );


#ifdef USEOLE
// Unblock the OLE server
if ( pOLE )
     do	{ os = OleUnblockServer( pOLE->lhSvr, &i ); } while( i );
#endif // #ifdef USEOLE	 

SetFocus( hWindow );

if ( bPrint ) // The message loop takes care of the exit
	SendMessage( hWindow, WM_COMMAND, IDM_EXIT, 0L );

/* Enter the one and only message loop... */
while ( TRUE )	{
 	if (!AstralGetMessage(&msg))
 		break;

	AstralProcessMessage(&msg);
	}

/* The application reveived a WM_QUIT Message */
return( msg.wParam );
}


/***********************************************************************/
static BOOL AstralInit( HINSTANCE hInstance )
/***********************************************************************/
{
WNDCLASS WndClass;

/* Setup and define the window class for the application */
WndClass.hIcon		= LoadIcon( hInstance, MAKEINTRESOURCE(IDC_ABOUTAPP) );
WndClass.lpszMenuName	= (LPSTR)"CDP";
WndClass.lpszClassName	= (LPSTR)ID_APPCLASS;
WndClass.hCursor	= LoadCursor( hInstance, MAKEINTRESOURCE(ID_HAND01) );
WndClass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH); //COLOR_APPWORKSPACE+1;
WndClass.hInstance	= hInstance;
WndClass.style		= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
WndClass.lpfnWndProc	= (WNDPROC)AstralWndProc;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 0;

if (!RegisterClass( &WndClass ))
	return( FALSE );

/* Setup and define a window class for the child image windows */
WndClass.hIcon		= NULL;
WndClass.lpszMenuName	= NULL;
WndClass.lpszClassName	= (LPSTR)"ppimage";
WndClass.hCursor	= NULL;
WndClass.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
WndClass.hInstance	= hInstance;
WndClass.style		= CS_DBLCLKS; // | CS_BYTEALIGNCLIENT;
WndClass.lpfnWndProc	= (WNDPROC)WndImageProc;
WndClass.cbClsExtra	= 0;
WndClass.cbWndExtra	= 24;

if (!RegisterClass( &WndClass ))
	return( FALSE );

RegisterAllClasses( hInstance );
return( TRUE );
}


/***********************************************************************/
BOOL AstralGetMessage( LPMSG lpMsg )
{
static long lLastTickCount;
long lTickCount;
BOOL bMessage;

while ( TRUE )
	{
    if (!fAppActive || 
        (!ImgDisplayMarquees(lpImage) && 
         !AnimateIsOn() && 
         !IntermissionGetIsOn() ))
		return( GetMessage( lpMsg, NULL, NULL, NULL ) );

    bMessage = PeekMessage( lpMsg, NULL, NULL, NULL, PM_REMOVE);
    IntermissionMsg(lpMsg, bMessage);
	if ( !bMessage )
	{
        if (AnimateIsOn())
            AnimateBatchPlay();
        else
        {
    		lTickCount = GetTickCount();
    		if ((lTickCount-lLastTickCount) > 20)
    			{
    			lLastTickCount = lTickCount;
    			Marquee(NO);
    			}
        }
		continue;
	}

	if ( lpMsg->message == WM_QUIT )
		return(FALSE);

	return(TRUE);
	}
}


/***********************************************************************/
void AstralProcessMessage( LPMSG lpMsg )
{
/* If not a mouse-related message, hide the bubble help */
if (lpMsg->message != WM_SETCURSOR &&
    lpMsg->message != WM_NCHITTEST &&
    lpMsg->message != WM_MOUSEMOVE &&
    lpMsg->message != WM_TIMER &&
    lpMsg->message != 0x0118 &&			// Humm... where is this documented?
    lpMsg->message != WM_MENUSELECT)
    HintLine (0);

/* If an accelerator was executed, go get another command */
if (TranslateMDISysAccel (hClientAstral, lpMsg))
	return;

if ( TranslateAccelerator( hWndAstral, hAccelTable, lpMsg ) )
	return;

// Check to see if the puzzle room is active and wants this message.	
if (PuzzleRoomMsg(lpMsg))
	return;	

/* If some child window took the message, go get another one */
if ( AstralDlgMsg( lpMsg ) )
	return;

/* Process the message with AstralWndProc() */
TranslateMessage( lpMsg );
DispatchMessage( lpMsg );
}


/***********************************************************************/
long WINPROC EXPORT AstralWndProc( HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam)
{
HMENU 	hMenu;
LPSTR 	lp;
int 	i, lo, hi, id, ret, x, y, dx, dy;
BOOL 	fHasZoom;
long 	lReturn;
FNAME 	szFileName;
UINT	uMenuID;
UINT	uMenuFlags;
static POINT ClosePoint;
HWND	hDlg;

switch ( message )
    {
    case WM_CREATE:
	DragAcceptFiles( hWindow, TRUE );

	Window.hNullCursor = LoadCursor( hInstAstral, MAKEINTRESOURCE(ID_NULL));
	break;

	case WM_SIZE:
	if ( !hClientAstral )
		break;
	if ( !AstralStr( IDS_POS_CLIENT, (LPSTR FAR *)&lp ) )
		break;
	AsciiToInt4( lp, &x, &y, &dx, &dy );
	MoveWindow( hClientAstral, x, y, dx, dy, TRUE );
	break;

    case WM_DROPFILES:
//	DragQueryPoint( (HANDLE)wParam, &pt );
	i = DragQueryFile( (HDROP)wParam, 0xFFFF, NULL, NULL );
	while ( --i >= 0 )
		{
		DragQueryFile( (HDROP)wParam, i, szFileName,
			sizeof(szFileName));
		AstralImageLoad( NULL, szFileName, MAYBE, YES );
		}
	DragFinish( (HDROP)wParam );
	break;

    case WM_INITMENU:
	SetFileMenus();
	break;

    case WM_NCLBUTTONDBLCLK:
	if ( wParam != HTMENU ||
	     LOWORD(lParam) != ClosePoint.x || HIWORD(lParam) != ClosePoint.y )
		return( DefFrameProc( hWindow, hClientAstral, message, wParam,
			lParam) );
	{
	RECT Cookie;

	GetWindowRect( hWindow, (LPRECT)&Cookie);

	Cookie.top  += GetSystemMetrics(SM_CYCAPTION);
	Cookie.left += GetSystemMetrics(SM_CXBORDER);

	Cookie.right  = Cookie.left+GetSystemMetrics(SM_CYMENU);
	Cookie.bottom = Cookie.top+GetSystemMetrics(SM_CYMENU);

	/* Make sure we are in the system cookie area */

	if ((ClosePoint.x >= Cookie.left &&
	     ClosePoint.x <= Cookie.right) &&
	    (ClosePoint.y >= Cookie.top  &&
	     ClosePoint.y <= Cookie.bottom)) {
		/* Time to close the child window */
		return( SendMessage( hWindow, WM_COMMAND, IDM_CLOSE, lParam ) );
		}
	return( DefFrameProc( hWindow, hClientAstral, message, wParam, lParam) );
    }
	break;

    case WM_SYSCOMMAND:
	if ((wParam & 0x000F) == HTMENU) {
		ClosePoint.x = LOWORD(lParam);
		ClosePoint.y = HIWORD(lParam);
	}
	return( DefFrameProc( hWindow, hClientAstral, message, wParam, lParam));
	break;

   case WM_ACTIVATEAPP:
	if ( !wParam && fAppActive )
		{ // If deactivating...
		if ( hDlg = AstralDlgGet( IDD_MAIN ) )
			SET_CLASS_CURSOR( hDlg, hDialogCursor);

		DeactivateTool();
		RestoreSystemPalette( hWindow );
		fAppActive = FALSE;
		lReturn = DefFrameProc( hWindow,hClientAstral,message,wParam,lParam );
//		ShowWindow( hWindow, SW_MINIMIZE );
		}
	if ( wParam && !fAppActive )
		{ // If activating...
		if ( hDlg = AstralDlgGet( IDD_MAIN ) )
			hDialogCursor = (HCURSOR)SET_CLASS_CURSOR( hDlg,
				LoadCursor( hInstAstral, MAKEINTRESOURCE(ID_HAND01) ) );

		lReturn = DefFrameProc( hWindow,hClientAstral,message,wParam,lParam );
		SaveSystemPalette( hWindow );
		fAppActive = TRUE;
		if (lpImage)
			ActivateDocument(lpImage->hWnd);
		}
	return( lReturn );

   case WM_QUERYNEWPALETTE:
	return(HandleQueryNewPalette(hWindow, NO, NO));

   case WM_PALETTECHANGED:
	if (IsOurWindow((HWND)wParam))
		SendMessageToChildren(hWindow, message, wParam, lParam);
	break;

    case WM_MOUSEACTIVATE: // Get when there's a mouse click on a child window
	return( DefFrameProc( hWindow, hClientAstral, message, wParam, lParam));

    case WM_SETCURSOR:
	if ( SetupCursor( wParam, lParam, NULL ) )
		return( TRUE );
	return( DefFrameProc(hWindow, hClientAstral, message, wParam, lParam) );

    case WM_MENUSELECT:
/*
	WIN16:	ID          = wParam
			menu flags  = LOWORD(lParam)
			menu handle = HIWORD(lParam)
			
	WIN32:	ID			= LOWORD(wParam)
			menu flags  = HIWORD(wParam)
			menu handle = lParam			
*/
#ifdef WIN32
	uMenuID    = LOWORD(wParam);
	uMenuFlags = HIWORD(wParam);
	hMenu      = (HMENU)lParam;
#else
	uMenuID    = wParam;
	uMenuFlags = LOWORD(lParam);
	hMenu      = (HMENU)HIWORD(lParam);
#endif // #ifdef WIN32
		
//	lo = LOWORD(lParam);
//	hi = HIWORD(lParam);
	if (hMenu == 0)
		break;
	if (uMenuFlags == -1)
		break;
	if (uMenuFlags & MF_SYSMENU)
		break;
//	if (uMenuFlags & MF_POPUP)
//		{
//		if ( !(hMenu = (HMENU)wParam) )
//			break;
//		while (GetSubMenu(hMenu, 0))
//			hMenu = GetSubMenu(hMenu, 0);
//		if ( hMenu )
//			break;
//		id = GetMenuItemID(hMenu, 0);
//		if (id <= 0)
//			break;
//		wParam = id - 1;
//		}
	HintLine( uMenuID );
	SoundStartResource( "menus", NO/*bLoop*/, NULL/*hInstance*/ );
	break;

    case WM_COMMAND:
	if ( !(wParam & 0x8000) )  // our commands don't have high bit set
		if ( lReturn = DoCommand( hWindow, wParam, lParam ) )
			return( lReturn );
	return( DefFrameProc( hWindow, hClientAstral, message, wParam, lParam));

    case WM_RENDERALLFORMATS: // If closing our app...
	OpenClipboard( hWindow );

#ifdef NOTCRAYOLA
	if ( !AstralAffirm( IDS_OKTORENDERCLIPBOARD ) )
		{
		EmptyClipboard();
		CloseClipboard();
		break;
		}
#endif
	wParam = 0; // fall through and do all formats

    case WM_RENDERFORMAT: // Deferred action from the IDM_COPY command
	ret = RenderToClipboard( hWindow, wParam );
   	if ( message == WM_RENDERALLFORMATS )  // If closing our app...
		CloseClipboard();
	return( ret );
	break;

    case WM_ENDSESSION:
	if (!wParam)
		break;
	// fall through...

    case WM_DESTROY:	// Sent after being removed from the display
			// and before destroying children

	/* Free up any DOS memory */
	GetBuffers16Kx4( NULL, YES );
	FreeUp(SBLTLineBuffer[0]);

    // Delete bubble help bubble
    Status ((HWND)-1,NULL);

	// Delete the background bitmaps
	LoadRoomBitmap( NULL );

	/* Delete any objects that might be hanging around */
	DeleteObject( Window.hShadowBrush );
	DeleteObject( Window.hButtonBrush );
	DeleteObject( Window.hHilightBrush );
	DeleteObject( Window.hShadowPen );
	DeleteObject( Window.hButtonPen );

	DestroyMgxBrush(Retouch.hBrush);
	if ( lpBrushSettings )
		FreeUp( (LPTR) lpBrushSettings );
	SetupBrushBuffers(NO);

	if (lpBltScreen)
		EndSuperBlt(lpBltScreen);
	LoadAllExtNamePaths( YES ); // FreeUp the ext name paths
//	// Delete the clipboard file if NO PP's (class name) are running
//	if ( !FindWindow( (LPTR)ID_APPCLASS, NULL ) )
//		{
//		GetExtNamePath( szFileName, IDN_CLIPBOARD );
//		WildcardDelete( szFileName, "WINCLIP.*" );
//		GetExtNamePath( szFileName, IDN_MASK );
//		WildcardDelete( szFileName, "WINCLIP.*" );
//		}
	Help( HELP_QUIT, 0L );
	if (message == WM_DESTROY)
		PostQuitMessage( 0 );
	hClientAstral = NULL;
	break;

    case WM_CLOSE:
    case WM_QUERYENDSESSION: /* end Windows session */

	// Hey!  You can't just quit while animations are running.
	if (AnimateIsOn())
		return FALSE;

	DeactivateTool();
	/* Check to see if the image needs to be saved */
	if (! ConfirmClose (YES,NO) )
		return (FALSE);

	if ( hDlg = AstralDlgGet( IDD_MAIN ) )
		SET_CLASS_CURSOR( hDlg,  hDialogCursor);

	// if IN an add-on room DLL dialog, close it and free it
	if (hCurrentDLL != NULL && idCurrentRoom < RM_FIRST)
	{
		HWND hDllDlg;
		hDllDlg = AstralDlgGet (idCurrentRoom);
		if (hDllDlg)
			DestroyWindow (hDllDlg);

		FreeLibrary (hCurrentDLL);
		hCurrentDLL = NULL;
	}

	// Close the sound DLL
	SoundClose();

	fHasZoom = Window.fHasZoom; 	// Closing images will set this to False
	CloseImage( YES, NULL ); // Close all images
	DeactivateTool();
	/* Save any default values */
	SaveMiscPreferences();
	PPOLEExit();
	if (message == WM_CLOSE)
		DestroyWindow( hWindow );
	return( TRUE );

    default:
#ifdef _MAC
	// DefFrameProc() doesn't seems to work on the mac --- WHY ????
	// The Main window (this one) doesn't recieve WM_PAINT, WM_ERASEBKGND messages.	
	return DefWindowProc(hWindow, message, wParam, lParam);
#else	
	return( DefFrameProc( hWindow, hClientAstral, message, wParam, lParam));
#endif	
    }

return( TRUE );
}


/***********************************************************************/
BOOL SetupCursor( WPARAM wParam, LPARAM lParam, WORD idWnd )
/***********************************************************************/
{
HWND hWnd;
WORD HitCode;
POINT CursorPos;
BOOL bHasHelp;
long id, wClassExtra;

hWnd = (HWND)wParam;
if ( !lpImage || !IsDoc(hWnd) )
	{ // if its any old (non-image) window, use whatever cursor it wants...
	DisplayBrush( 0, 0, 0, OFF );
	wClassExtra = GET_CLASS_CBCLSEXTRA( hWnd );
	if ( wClassExtra == COLOR_PATCH_CLASS )
		return( FALSE ); // hint line handle in the control
	if ( GetWindowTask(hWndAstral) != GetWindowTask(hWnd) )
		return( FALSE ); // If it's not our app, get out
	if ( hWnd == hWndAstral )
		id = IDS_READY; // If it's the app itself, use "Ready" hint
	else
	if ( !StyleOn( hWnd, WS_CHILD ) || !(id = GET_WINDOW_ID( hWnd )) )  // If no id for the window
		if ( !(id = idWnd) ) // and no id passed in,
        {
        	HintLine( 0 );
			return( FALSE );	      // use the default cursor
        }
        // see if special ribbon icon class - a bit of a hack, but so what!!!
	if ( wClassExtra == RIBBON_ICON_CLASS ) // Do the hint line
        bHasHelp = HintLineSpecial( (int)id, IDC_RIBBONSTART );
	else
    {
        int i,rows,cols;
        POINT pt;
        RECT rect;

        //***************************************************************//**
        // The following code was added to differentiate color hints     //**
        // See also LAYOUT.C ColorCommand() [mpd]                        //**
        //***************************************************************//**
        switch (idWnd)                                                   //**
        {                                                                //**
		case IDC_COLORS_CRAYON:                                          //**
		case IDC_COLORS_MARKER:                                          //**
			GetWindowRect( hWnd, &rect );                                //**
			GetCursorPos( &pt );                                         //**
			pt.x -= rect.left;                                           //**
			pt.y -= rect.top;                                            //**
			OffsetRect( &rect, -rect.left, -rect.top );                  //**
			#ifdef STUDIO                                                //**
				rows = 2; cols = 12;	// 2 x 12 color layout           //**
			#else                                                        //**
				rows = 1; cols = 16;	// 1 x 16 color layout           //**
			#endif                                                       //**
            if (pt.x >= rect.right/cols*cols) // compensate for round-off err
                pt.x = rect.right/cols*cols - 1;
            if (pt.y >= rect.bottom/rows*rows)
                pt.y = rect.bottom/rows*rows - 1;
			i  = (pt.y / (rect.bottom/rows));                            //**
			i *= cols;                                                   //**
			i += (pt.x / (rect.right/cols));                             //**
			#ifndef STUDIO                                               //**
			// remap the index                                           //**
				if ( i == 10 )	i = 11;		else                         //**
				if ( i == 11 )	i = 23;		else                         //**
				if ( i == 12 )	i = 10;		else                         //**
				if ( i == 13 )	i = 22;		else                         //**
				if ( i == 14 )	i = 13;		else                         //**
				if ( i == 15 )	i = 12;                                  //**
			#endif                                                       //**
            id = IDC_COLOR_FIRST + i;                                    //**
			break;                                                       //**
                                                                         //**
		case IDC_COLORS_PAINT:                                           //**
		case IDC_COLORS_TEXT:                                            //**
		case IDC_COLORS_LINES:                                           //**
		case IDC_COLORS_FILL:                                            //**
		case IDC_COLORS_SHAPES:                                          //**
			GetWindowRect( hWnd, &rect );                                //**
			GetCursorPos( &pt );                                         //**
			pt.x -= rect.left;                                           //**
			pt.y -= rect.top;                                            //**
			OffsetRect( &rect, -rect.left, -rect.top );                  //**
			#ifdef STUDIO                                                //**
				rows = 3; cols = 8;		// 3 x 8 color layout            //**
			#else                                                        //**
				rows = 2; cols = 8;		// 2 x 8 color layout            //**
			#endif                                                       //**
            if (pt.x >= rect.right/cols*cols) // compensate for round-off err
                pt.x = rect.right/cols*cols - 1;
            if (pt.y >= rect.bottom/rows*rows)
                pt.y = rect.bottom/rows*rows - 1;
			i  = (pt.y / (rect.bottom/rows));                            //**
			i *=cols;                                                    //**
			i += (pt.x / (rect.right/cols));                             //**
			// remap the index                                           //**
			if ( i == 1 )	i = 12;		else                             //**
			if ( i == 2 )	i = 13;		else                             //**
			if ( i == 3 )	i = 1;		else                             //**
			if ( i == 4 )	i = 22;		else                             //**
			if ( i == 5 )	i = 10;		else                             //**
			if ( i == 6 )	i = 23;		else                             //**
			if ( i == 7 )	i = 11;		else                             //**
			if ( i >= 8 && i <= 15 ) i -= 6; else // 2nd row, primaries  //**
			if ( i >= 16 && i <= 23 ) i -= 2; // 3rd row, secondaries    //**
            id = IDC_COLOR_FIRST + i;                                    //**
			break;                                                       //**
        }                                                                //**
        //***************************************************************//**
        //***************************************************************//**

        bHasHelp = HintLine( (int)id );
    }
	return( FALSE ); // use the default cursor
	}

// It's an image window
HitCode = LOWORD( lParam );
if ( HitCode != HTCLIENT )
	{
	DisplayBrush( 0, 0, 0, OFF );
	HintLine( 0 );
	return( FALSE ); // use the default cursor
	}

if ( !fAppActive || lpImage->hWnd != hWnd )
	{ // if the app is not active, or the window's inactive use the arrow...
	DisplayBrush( 0, 0, 0, OFF );
	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
	return( TRUE ); // means we have taken care of cursor
	}

// Its the active image...
GetCursorPos( &CursorPos );
ScreenToClient( hWnd, &CursorPos );
if ( !ONIMAGE( CursorPos.x, CursorPos.y ) )
	{ // if not on the image display area, use the arrow...
	DisplayBrush( 0, 0, 0, OFF );
//	SetCursor( LoadCursor( NULL, IDC_ARROW ) );
	SetCursor (LoadCursor (NULL, MAKEINTRESOURCE(ID_HAND01)) );
	HintLine( 0 );
	return( TRUE ); // means we have taken care of cursor
	}

if ( !Tool.bActive )
	HintLineSpecial( Tool.id, IDC_IMAGESTART );

if ( Window.hCursor != Window.hNullCursor )
	DisplayBrush( 0, 0, 0, OFF );

return( FALSE ); // use the default cursor
}


/***********************************************************************/
static void SetFileMenus()
/***********************************************************************/
{
int state;
HMENU hMenu;

if ( !(hMenu = GetMenu( hWndAstral )) )
	return;

CheckMenuItem( hMenu, IDC_SOUND, (SoundIsOn()? MF_CHECKED : MF_UNCHECKED) );
CheckMenuItem( hMenu, IDM_HELP, (Control.Hints? MF_CHECKED : MF_UNCHECKED) );

// Only enabled in the paint screen (Current room is closed)
state = ( !AstralDlgGet(idCurrentRoom) ? MF_ENABLED : MF_GRAYED );
EnableMenuItem( hMenu, IDM_NEW, state );
EnableMenuItem( hMenu, IDM_OPEN, state );
EnableMenuItem( hMenu, IDM_SAVEWALLPAPER, state );

if ( state == MF_ENABLED )
	state = ( lpImage ? MF_ENABLED : MF_GRAYED );

// Disable some menu items if there is no image open
EnableMenuItem( hMenu, IDM_REVERT, state );
EnableMenuItem( hMenu, IDM_SAVE, state );
EnableMenuItem( hMenu, IDM_SAVEAS, state );
if ( !lpImage || !ImgChanged(lpImage) )
	EnableMenuItem( hMenu, IDM_REVERT, MF_GRAYED );
}


/***********************************************************************/
void CloseImage( BOOL fCloseAll, LPIMAGE lpCloseImage)
{
int i;
HWND hWnd, hOldWnd;
LPIMAGE lpTheImage;

if ( !lpImage )
	return;


// Destroy the image window
if (fCloseAll)
    { // Close all image windows
    while (NumDocs() > 0)
		{
		hWnd = GetDoc(0);
		SendMessage( hClientAstral, WM_MDIDESTROY, (WPARAM)hWnd, 0L );
		}
    }
else
if ( lpCloseImage )
	{ // Close all image windows related to a specific image pointer
	for (i = 0; i < NumDocs(); ++i)
		{
		hWnd = GetDoc(i);
		if ( !SetCurView(hWnd, &hOldWnd) )
			continue;
		lpTheImage = lpImage;
		SetCurView( hOldWnd, NULL );
		if ( lpTheImage == lpCloseImage )
	 		{
			SendMessage( hClientAstral, WM_MDIDESTROY, (WPARAM)hWnd, 0L );
			i = -1; // Start the loop over again
			}
		}
	}
else
	{ // Close only the active image window
	SendMessage( hClientAstral, WM_MDIDESTROY, (WPARAM)lpImage->hWnd, 0L );
	}
}


/***********************************************************************/
BOOL ConfirmClose(BOOL fCloseAll, BOOL fCloseView)
{
int	ret, i, j;
LPIMAGE lpTheImage;
HWND hWnd, hSaveWnd;

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
	if (SetCurView(hWnd, NULL) && ImgChanged(lpImage) )
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
			    ret = AstralConfirm( IDS_AREYOUSURE, (LPSTR)lpImage->CurFile );
			    if ( ret == IDCANCEL )
				{
				SetCurView(hSaveWnd, NULL);
				return( NO );
				}
			    if ( ret == IDYES )
				{
				if (!HandleSaveCommand(hWndAstral, IDM_SAVE,NO))
					{
					SetCurView(hSaveWnd, NULL);
					return( NO );
					}
				}
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
	if (fCloseView)
		if (AnotherView(lpImage))
			return(YES);
    /* Check to see if the image needs to be saved */
    if ( ImgChanged(lpImage) )
	{
	ret = AstralConfirm( IDS_AREYOUSURE, (LPSTR)lpImage->CurFile );
	if ( ret == IDCANCEL )
	    return( NO );
	if ( ret == IDYES )
	    {
	    if (!HandleSaveCommand(hWndAstral, IDM_SAVE, NO))
		return(NO);
	    }
	}
    }
return( YES );
}

/************************************************************************/
BOOL AnotherView(LPIMAGE lpTheImage)
/************************************************************************/
{
int i;
HWND hTheWnd, hWnd;
LPIMAGE lpTryImage;

hTheWnd = lpTheImage->hWnd;
for (i = 0; i < NumDocs(); ++i)
	{
	hWnd = GetDoc(i);
	if (!(lpTryImage = (LPIMAGE)GetWindowLong(hWnd, GWL_IMAGEPTR)))
		continue;
    if (lpTryImage == lpTheImage && hWnd != hTheWnd)
		return(TRUE);
	}
return(FALSE);
}

/************************************************************************/
BOOL ConfirmDiscardAll()
/************************************************************************/
{
int retc;

if ( !lpImage )
	return( TRUE );
if ( (retc = AstralConfirm( IDS_OKTOCLOSEALL )) == IDCANCEL )
	return( FALSE );
if ( retc == IDNO )
	return( TRUE );
if ( !ConfirmClose(YES,NO) )
	return( FALSE );
CloseImage( YES, NULL );
return( TRUE );
}


/***********************************************************************/
LPSTR GetUntitledName()
/***********************************************************************/
{
char num[10];

LoadString( hInstAstral, IDS_UNTITLED, szUntitled, MAX_STR_LEN );
itoa( ++Control.UntitledNo, num, 10 );
lstrcat( (LPSTR)szUntitled, (LPSTR)num );
return( (LPSTR)szUntitled );
}


/***********************************************************************/
BOOL HandleQueryNewPalette(HWND	hWnd, BOOL	fSpecialPal, BOOL	fColorPal)
{
HPALETTE	hPal;
HDC		hDC;

if (!lpBltScreen) // should never happen - not initialized yet
	return(FALSE);

if (fSpecialPal)
	// window needs a specific display palette which
	// may be different than palette for active image
	{
	if (fColorPal)
		hPal = lpBltScreen->hColorPal;
	else
		hPal = lpBltScreen->hGrayPal;
	}
else if (lpImage)
	// use active image palette if there is an active image
	hPal = lpImage->hPal;
else if (lpBltScreen->hColorPal)
	// always use color display palette if no active image
	hPal = lpBltScreen->hColorPal;
else
	hPal = NULL;
if (!hPal)
	return(FALSE);
lpBltScreen->hPal = hPal;
hDC = GetDC(hWnd);
SelectPalette(hDC, hPal, FALSE);
RealizePalette(hDC);
ReleaseDC(hWnd, hDC);
return(TRUE);
}


/***********************************************************************/
BOOL IsOurWindow(HWND	hWnd)
{
#ifdef WIN32
	return TRUE;
#else
	return(GetCurrentTask() == GetWindowTask(hWnd));
#endif
}


//************************************************************************
BOOL WackWindows( HINSTANCE hInst )
//************************************************************************
{
FARPROC  lpProc, lpProc1;

if ( !(lpProc = MakeProcInstance( (FARPROC)CallBkProc, hInst )) )
	return( FALSE );
if ( !(lpProc1 = MakeProcInstance( (FARPROC)CallBkProc1, hInst )) )
	return( FALSE );

EnumWindows( (WNDENUMPROC)lpProc, (LPARAM)lpProc1 );
FreeProcInstance( lpProc );
FreeProcInstance( lpProc1 );
return( TRUE );
}


//************************************************************************
BOOL CALLBACK CallBkProc( HWND hWnd, LPARAM lParam )
//************************************************************************
{
char  aClassName[128];
RECT  rect;

GetClassName( hWnd, aClassName, sizeof( aClassName ) );

if ( !lstrcmpi( aClassName, "MDICLIENT" ) )
	{
	GetClientRect( hWnd, &rect );
	InvalidateRect( hWnd, &rect, TRUE );
	}
else	EnumChildWindows( hWnd, (WNDENUMPROC)lParam, 0 );
return( TRUE );
}


//************************************************************************
BOOL CALLBACK CallBkProc1( HWND hWnd, LPARAM lParam )
//************************************************************************
{
char  aClassName[128];
RECT  rect;

GetClassName( hWnd, aClassName, sizeof( aClassName ) );

if ( !lstrcmpi( aClassName, "MDICLIENT" ) )
	{
	GetClientRect( hWnd, &rect );
	InvalidateRect( hWnd, &rect, TRUE );
	}
return( TRUE );
}

//************************************************************************
HMENU AstralGetSubMenu(HMENU hMenu, int nPos)
//************************************************************************
{
if ( !hMenu )
	return( NULL );
if (lpImage && IsZoomed(lpImage->hWnd))
	++nPos;
return(GetSubMenu(hMenu, nPos));
}


#ifdef WIN32
POINT MAKEPOINT(long l)	// POINT is 32bit values, POINTS is 16bit values	
{
	POINT  pt32;
	POINTS pt16 = MAKEPOINTS(l);
	pt32.x = pt16.x;
	pt32.y = pt16.y;
	return pt32;
}
#endif




LPAOBJECT PlaySimpleOpening(HWND hWindow)
{
FNAME szFileName;
int x, y, wWidth, wHeight;
LPAOBJECT Asession = NULL;

if ( !LBUTTON && !Control.bNoTheme && (Asession = ACreateNewAnimator() ))
	{
	SoundStartFile( "opening", FALSE, NULL );

	wWidth = GetSystemMetrics( SM_CXSCREEN );
	wHeight = GetSystemMetrics( SM_CYSCREEN );
	// The MGX logo is 475 x 63
	x = (wWidth - 475) / 2;
	y = (wHeight - 286) / 2; // 286 = height of all parts + 2*28 spaces
	wsprintf( szFileName, "SETSPEED 38\rJUMP %d,%d\rMOVE %d,%d\r",
		0-475, y, x, y );
	if ( !LBUTTON )
		AnimateDibID( hWindow, NULL/*&rRect*/, hInstAstral, IDC_MGX_LOGO,
			(LPTR)Asession, szFileName );
	// The FUTURE logo is 477 x 28
	y += (63 + 28);
	wsprintf( szFileName, "SETSPEED 38\rJUMP %d,%d\rMOVE %d,%d\r",
		wWidth, y, x, y );
	if ( !LBUTTON )
		AnimateDibID( hWindow, NULL/*&rRect*/, hInstAstral, IDC_FUTURE_LOGO,
			(LPTR)Asession, szFileName );
	// The PRESENTS logo is 237 x 75
	x += (475-237);
	y += (28 + 28);
	wsprintf( szFileName, "SETSPEED 38\rJUMP %d,%d\rMOVE %d,%d\r",
		0-237, y, x, y );
	if ( !LBUTTON )
		AnimateDibID( hWindow, NULL/*&rRect*/, hInstAstral, IDC_PRESENTS_LOGO,
			(LPTR)Asession, szFileName );
	}
	return Asession;
}


//#ifdef CDBUILD
//
//void PlayCDOpening()
//{
//    char szOpeningEXE[256];
//    HDC  hDC;
//    int  iColorBits;
//    MSG  msg;
//	DWORD dwLastTick ;
//	DWORD dwTickCount;
//	
//	if (Control.bNoTheme)
//		return;
//
//	hDC = GetDC(NULL);
//	iColorBits = GetDeviceCaps(hDC, BITSPIXEL);
//	ReleaseDC(NULL, hDC);
//
//    PathUp (szOpeningEXE, Control.ProgHome);
//#ifdef STUDIO
//	if (iColorBits < 8)
//        PathCat (szOpeningEXE, szOpeningEXE, "opening\\as4.exe");
//    else
//        PathCat (szOpeningEXE, szOpeningEXE, "opening\\as.exe");
//#else
//	if (iColorBits < 8)
//        PathCat (szOpeningEXE, szOpeningEXE, "opening\\aaa4.exe");
//    else
//        PathCat (szOpeningEXE, szOpeningEXE, "opening\\aaa.exe");
//#endif
//
//    {
//    char szCurrDir[MAX_PATH];
//    char szExecDir[MAX_PATH];
//    UINT wExec;
//
//    GetCurrentDir (szCurrDir,sizeof szCurrDir);
//    PathCat (szExecDir,Control.ProgHome,"opening");
//    SetCurrentDir (szExecDir);
//    wExec = WinExec (szOpeningEXE,SW_SHOW);
//    SetCurrentDir (szCurrDir);
//	if (wExec < 32)
//		return;
//    }
//
//// Wait for the app to load
//	dwLastTick = GetTickCount();
//	dwTickCount = dwLastTick;
//    while (dwTickCount < dwLastTick+30000 &&
//    	!FindWindow(NULL, "Projector")) 
//    {
//		if (PeekMessage (&msg,NULL,NULL,NULL,PM_REMOVE))
//		{
//        	TranslateMessage (&msg);
//        	DispatchMessage (&msg);
//		}
//		dwTickCount = GetTickCount();
//    }
//
//// Wait for the app to finish	
//	dwLastTick = GetTickCount();
//	dwTickCount = dwLastTick;
//    while (dwTickCount < dwLastTick+60000 &&
//    	   FindWindow(NULL, "Projector")) 
//    {
//		if (PeekMessage (&msg,NULL,NULL,NULL,PM_REMOVE))
//		{
//        	TranslateMessage (&msg);
//        	DispatchMessage (&msg);
//		}
//		dwTickCount = GetTickCount();
//    }
//}
//
//#endif // #ifdef CDBUILD
