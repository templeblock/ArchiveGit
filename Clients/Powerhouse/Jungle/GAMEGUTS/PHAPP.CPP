#include <windows.h>
#include <mmsystem.h>
#include <direct.h>
#include "proto.h"
#include "sound.h"
#include "sprite.h"
#include "wing.h" 
#include "commonid.h"	 
#include "control.h"
#include "scene.h"

// keep track of current app
static LPPHAPP pApp = NULL;

#define WS_PHCAPTION (WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)

#define JOY_TIMER		1001
#define CAPTION_TIMER 	1002
#define CAPTION_DELAY	1000
#define HINT_TIMER		1003
#define HINT_DELAY		1000

long WINPROC EXPORT ApplicationWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
long WINPROC EXPORT TitleBarWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
long WINPROC EXPORT BubbleWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

// DLL must provide its own version of GetApp()
#ifndef _WINDLL
//***********************************************************************
LPPHAPP GetApp()
//***********************************************************************
{
	return (pApp);		
}
#endif

//***********************************************************************
CPHApp::CPHApp()
//***********************************************************************
{
	// in non-running state until WinMain
	m_hInstance = NULL;
	m_hPrevInstance = NULL;
	m_lpCmdLine = NULL;
	m_nCmdShow = SW_SHOWNORMAL;
	m_hMainWnd = NULL;
	m_hPal = NULL;
	m_hDeviceMID = NULL;
	m_hDeviceAVI = NULL;
	m_hDeviceWAV = NULL;
	m_bAppNoErase = NO;
	m_hAccel = NULL;
	m_lpScene = NULL;
	m_hTitleBarWnd = NULL;
	m_fAppClosing = FALSE;
	ResetKeyMap();
	ResetJoyMap();
	m_idCaptionTimer = 0;
	m_idHintTimer = 0;
	m_hHintWnd = NULL;
	m_hBubbleWnd = NULL;
	m_WinVer = WV_WIN31;

	// setup for calls to GetApp()
	pApp = this;
 }

//***********************************************************************
CPHApp::~CPHApp()
//***********************************************************************
{
}

//***********************************************************************
void CPHApp::WinInit(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					 LPTSTR lpCmdLine, int nCmdShow)
//***********************************************************************
{
	m_hInstance = hInstance;
	m_hPrevInstance = hPrevInstance;
	m_lpCmdLine = lpCmdLine;
	m_nCmdShow = nCmdShow;
}

//***********************************************************************
BOOL CPHApp::InitApplication()
//***********************************************************************
{
	return TRUE;
}

//***********************************************************************
BOOL CPHApp::InitInstance()
//***********************************************************************
{
	#ifdef WIN32
		OSVERSIONINFO info;

		info.dwOSVersionInfoSize = sizeof(info);
		GetVersionEx(&info);
		if (info.dwPlatformId == VER_PLATFORM_WIN32s)
			m_WinVer = WV_WIN32S;
		else
		if (info.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
			m_WinVer = WV_WIN95;
		else
		//if (info.dwPlatformId == VER_PLATFORM_WIN32_NT)
			m_WinVer = WV_WINNT;
	#else
		if (GetWinFlags() & WF_WINNT)
			m_WinVer = WV_WINNT;
		else
		{
			DWORD dwVersion = GetVersion();
			WORD wWinVer = LOWORD(dwVersion);
			BYTE bMajor = LOBYTE(wWinVer);
			BYTE bMinor = HIBYTE(wWinVer);
			if (bMajor > 3 || (bMajor == 3 && bMinor >= 95))
				m_WinVer = WV_WIN95;
			else
				m_WinVer = WV_WIN31;
		}
	#endif
	
	// Load class name resource
	LoadString( GetInstance(), IDS_CLASS, m_szClass, sizeof(m_szClass) );

	// if we are already running activate running instance and exit
	if (CheckForPreviousInstance())
		return(FALSE);

	// Change to the exe's directory
	FNAME szPath;
	szPath[0] = '\0';
	GetModuleFileName( GetApp()->GetInstance(), szPath, sizeof( szPath ) );
	StripFile( szPath );
	int i; if ( i = lstrlen(szPath) )
		szPath[i-1] = '\0';
	_chdir( szPath );
	char c = *szPath;
	if ( c >= 'A' && c <= 'Z' )
		i = _chdrive( *szPath - 'A' + 1 );
	else
	if ( c >= 'a' && c <= 'z' )
		i = _chdrive( *szPath - 'a' + 1 );
	_getcwd( szPath, sizeof(szPath) );

	// register all our window classes
	if (! RegisterAllClasses() )
		return(FALSE);

	// create our WinG palette
	if ( m_hPal = LoadPalette( "indeo32.pal" ) )
		MakeIdentityPalette( m_hPal );
	else
		m_hPal = WinGCreateHalftonePalette();

	// Clear the system palette with black, so we know there are no colors to map to
	// JMM - Is this necessary?
	ClearSystemPalette();

	// create the main application window
	if (!CreateMainWindow())
		return(FALSE);
					
	if (!CreateAdditionalWindows())
		return(FALSE);

	if (!SetupDevices())
		return(FALSE);

	// Bring up the first scene dialog
	GotoScene( NULL/*hWndPreviousScene*/, 1 );

	return TRUE;
}

//***********************************************************************
int CPHApp::ExitInstance()
//***********************************************************************
{
	if (m_hPal)
	{
		DeleteObject(m_hPal);
		m_hPal = NULL;
	}
	return m_msgCur.wParam; // Returns the value from PostQuitMessage
}

//***********************************************************************
BOOL CPHApp::CheckForPreviousInstance()
//***********************************************************************
{
	HWND hWnd;

	// Only allow a single instance of the app
	if ( hWnd = FindWindow( m_szClass, NULL ) )
	{
		if ( IsIconic( hWnd ) )
			ShowWindow( hWnd, SW_RESTORE );
		else
			BringWindowToTop( hWnd );
		return( TRUE );
	}
	return(FALSE);
}

//***********************************************************************
BOOL CPHApp::RegisterAllClasses()
//***********************************************************************
{
	WNDCLASS WndClass;

	// Register the app class without a menu.
//	WndClass.hCursor		= LoadCursor( NULL, IDC_ARROW );
	WndClass.hCursor		= LoadCursor( GetInstance(), MAKEINTRESOURCE(IDC_TITLE_CURSOR));
	if (!WndClass.hCursor)
	WndClass.hCursor		= LoadCursor( GetInstance(), MAKEINTRESOURCE(IDC_HAND_CLOSED_CURSOR) );
	WndClass.hIcon			= LoadIcon( GetInstance(), MAKEINTRESOURCE(-1) );
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= m_szClass;
	HBITMAP hBitmap = LoadBitmap(GetInstance(), MAKEINTRESOURCE(IDB_TITLE_BACKGROUND));
	HBRUSH hBrush = NULL;
	if (hBitmap)
		hBrush = CreatePatternBrush(hBitmap);
	if (hBrush)
		WndClass.hbrBackground	= hBrush;
	else
		WndClass.hbrBackground	= (HBRUSH)GetStockObject( BLACK_BRUSH ); // COLOR_APPWORKSPACE+1;
	WndClass.hInstance		= GetInstance();
	WndClass.style			= CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc	= ApplicationWndProc;
	WndClass.cbClsExtra		= 0;
	WndClass.cbWndExtra		= 0;

	if (!RegisterClass( &WndClass ) )
		return FALSE;

	// subclass the standard dialog class
	if ( GetClassInfo( NULL, WC_DIALOG, &WndClass ) )
	{
		CScene::m_lpOldWndProc = WndClass.lpfnWndProc;
		WndClass.lpfnWndProc = (WNDPROC)SceneWindowProc;
		WndClass.lpszClassName	= (LPSTR)"scene";
		WndClass.hInstance = m_hInstance;
		if (!RegisterClass( &WndClass ))
			return( FALSE );
	}

	WndClass.hCursor		= LoadCursor( GetInstance(), MAKEINTRESOURCE(IDC_HAND_CLOSED_CURSOR) );
	WndClass.hIcon			= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= (LPSTR)"video";
	WndClass.hbrBackground	= (HBRUSH)GetStockObject( WHITE_BRUSH );
	WndClass.hInstance		= GetInstance();
	WndClass.style			= CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNWINDOW;
	WndClass.lpfnWndProc	= VideoControl;
	WndClass.cbClsExtra		= 0;
	WndClass.cbWndExtra		= 16;

	if (!RegisterClass( &WndClass ) )
		return FALSE;

	WndClass.hCursor		= LoadCursor( GetInstance(), MAKEINTRESOURCE(IDC_HAND_POINT_CURSOR) );
	WndClass.hIcon			= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= (LPSTR)"dib";
	WndClass.hbrBackground	= (HBRUSH)GetStockObject( WHITE_BRUSH );
	WndClass.hInstance		= GetInstance();
	WndClass.style			= CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc	= BitmapControl;
	WndClass.cbClsExtra		= 0;
	WndClass.cbWndExtra		= BITMAP_EXTRA;

	if (!RegisterClass( &WndClass ) )
		return FALSE;

	WndClass.hCursor		= LoadCursor( GetInstance(), MAKEINTRESOURCE(IDC_HAND_POINT_CURSOR) );
	WndClass.hIcon			= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= (LPSTR)"dib2";
	WndClass.hbrBackground	= (HBRUSH)GetStockObject( WHITE_BRUSH );
	WndClass.hInstance		= GetInstance();
	WndClass.style			= CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc	= BmultiControl;
	WndClass.cbClsExtra		= 0;
	WndClass.cbWndExtra		= BITMAP_EXTRA;

	if (!RegisterClass( &WndClass ) )
		return FALSE;

	WndClass.hCursor		= LoadCursor( GetInstance(), MAKEINTRESOURCE(IDC_HAND_POINT_CURSOR) );
	WndClass.hIcon			= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= (LPSTR)"hot";
	WndClass.hbrBackground	= (HBRUSH)GetStockObject( WHITE_BRUSH );
	WndClass.hInstance		= GetInstance();
	WndClass.style			= 0;
	WndClass.lpfnWndProc	= HotspotControl;
	WndClass.cbClsExtra		= 0;
	WndClass.cbWndExtra		= 16;

	if (!RegisterClass( &WndClass ) )
		return FALSE;

	// Horz. Slider control class
	WndClass.hCursor		= LoadCursor( GetInstance(), MAKEINTRESOURCE(IDC_HAND_POINT_CURSOR) );
	WndClass.hIcon			= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= (LPSTR)"hslider";
	WndClass.hbrBackground	= (HBRUSH)GetStockObject( WHITE_BRUSH );
	WndClass.hInstance		= GetInstance();
	WndClass.style			= 0;
	WndClass.lpfnWndProc	= HSliderControl;
	WndClass.cbClsExtra		= 0;
	WndClass.cbWndExtra		= 16;

	if (!RegisterClass( &WndClass ) )
		return FALSE;

	// Gauge control class
	WndClass.hCursor		= LoadCursor( NULL, IDC_ARROW );
	WndClass.hIcon			= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= (LPSTR)"gauge";
	WndClass.hbrBackground	= (HBRUSH)GetStockObject( WHITE_BRUSH );
	WndClass.hInstance		= GetInstance();
	WndClass.style			= 0;
	WndClass.lpfnWndProc	= GaugeControl;
	WndClass.cbClsExtra		= 0;
	WndClass.cbWndExtra		= 16;

	if (!RegisterClass( &WndClass ) )
		return FALSE;

	// Radar control class
	WndClass.hCursor		= LoadCursor( GetInstance(), MAKEINTRESOURCE(IDC_HAND_POINT_CURSOR) );
	WndClass.hIcon			= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= (LPSTR)"radar";
	WndClass.hbrBackground	= (HBRUSH)GetStockObject( BLACK_BRUSH );
	WndClass.hInstance		= GetInstance();
	WndClass.style			= 0;
	WndClass.lpfnWndProc	= RadarControl;
	WndClass.cbClsExtra		= 0;
	WndClass.cbWndExtra		= 16;

	if (!RegisterClass( &WndClass ) )
		return FALSE;

	// Meter control class
	WndClass.hCursor		= LoadCursor( NULL, IDC_ARROW );
	WndClass.hIcon			= NULL;
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= (LPSTR)"meter";
	WndClass.hbrBackground	= (HBRUSH)GetStockObject( WHITE_BRUSH );
	WndClass.hInstance		= GetInstance();
	WndClass.style			= 0;
	WndClass.lpfnWndProc	= MeterControl;
	WndClass.cbClsExtra		= 0;
	WndClass.cbWndExtra		= 16;

	if (!RegisterClass( &WndClass ) )
		return FALSE;

	// Register the titlebar class.
	WndClass.hCursor		= LoadCursor( NULL, IDC_ARROW );
	WndClass.hIcon			= LoadIcon( GetInstance(), MAKEINTRESOURCE(-1) );
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= "titlebar";
	WndClass.hbrBackground	= (HBRUSH)COLOR_APPWORKSPACE+1;
	WndClass.hInstance		= GetInstance();
	WndClass.style			= CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc	= TitleBarWndProc;
	WndClass.cbClsExtra		= 0;
	WndClass.cbWndExtra		= 0;

	if (!RegisterClass( &WndClass ) )
		return FALSE;

    WndClass.style         = CS_SAVEBITS;
    WndClass.lpfnWndProc   = BubbleWndProc;
    WndClass.cbClsExtra    = 0;
    WndClass.cbWndExtra    = 0;
    WndClass.hInstance     = GetInstance();
    WndClass.hIcon         = NULL;
    WndClass.hCursor       = LoadCursor( NULL, MAKEINTRESOURCE( IDC_ARROW ));
	if (GetWinVersion() == WV_WIN95)
		WndClass.hbrBackground	= (HBRUSH)(COLOR_INFOBK+1);
	else	
		WndClass.hbrBackground	= (HBRUSH)GetStockObject( WHITE_BRUSH );
    WndClass.lpszMenuName  = NULL;
    WndClass.lpszClassName =( LPTSTR )"bubble";

	if (!RegisterClass( &WndClass ) )
		return FALSE;

	if (!RegisterTitleClasses())
		return FALSE;

	return TRUE;
}

//***********************************************************************
BOOL CPHApp::CreateMainWindow()
//***********************************************************************
{
	int yPos;
	int iHeight, iWidth;
	STRING szTitle;
	DWORD dwStyle;

	// Size the application window based on the screen size
	iWidth = GetSystemMetrics( SM_CXSCREEN );
	iHeight = GetSystemMetrics( SM_CYSCREEN );
	yPos = 0;

//	// When you want to have a menu bar, but hide it off screen, add this in
//	iMenuHeight = GetSystemMetrics( SM_CYMENU ) + 1;
//	yPos = -iMenuHeight;
//	iHeight += iMenuHeight;
//	// If hiding the menu bar....
//	if ( cmdShow == SW_MAXIMIZE )
//		cmdShow = SW_NORMAL;

	// Load app name resource
	LoadString( GetInstance(), IDS_TITLE, szTitle, sizeof(szTitle) );

	// no caption or frame in 640x480 mode
	if (iWidth > 640 && iHeight > 480)
		dwStyle =	WS_PHCAPTION;
	else
	{
		// 640 x 480 we have a maximized app with no caption
		dwStyle = 0;
		m_nCmdShow = SW_MAXIMIZE;
	}

	// Create the application window
	if ( !(m_hMainWnd = CreateWindow(
		m_szClass,			// lpClassName
		szTitle,			// lpWindowName
		WS_POPUP |			// with WS_OVERLAPPED you can't make window bigger than screen
		WS_MAXIMIZE |
		WS_THICKFRAME |
		WS_CLIPCHILDREN |
		dwStyle,			// dwStyle
		0, yPos,			// position of window
		iWidth, iHeight,	// size of window
		NULL,				// hWindowParent (null for tiled)
		NULL,				// hMenu (null = class menu)
		GetInstance(),			// hInstance
		NULL				// lpParam
		)) )
			return FALSE;

	// Make window visible according to the way the app is activated
	ShowWindow( m_hMainWnd, m_nCmdShow );
	if ( m_nCmdShow != SW_HIDE )
		UpdateWindow( m_hMainWnd );

	return(TRUE);
}

//***********************************************************************
BOOL CPHApp::SetupDevices()
//***********************************************************************
{
	// Open the global MCI devices
	m_hDeviceMID = MCIOpenDevice( (LPSTR)MCI_DEVTYPE_SEQUENCER );
	m_hDeviceWAV = MCIOpenDevice( (LPSTR)MCI_DEVTYPE_WAVEFORM_AUDIO );
	m_hDeviceAVI = MCIOpenDevice( "avivideo" );
	return(TRUE);
}

//***********************************************************************
void CPHApp::CloseMCIDevices()
//***********************************************************************
{
	if (m_hDeviceMID)
	{
		MCIClose(m_hDeviceMID );
		m_hDeviceMID = NULL;
	}

	if (m_hDeviceWAV)
	{
		MCIClose( m_hDeviceWAV );
		m_hDeviceWAV = NULL;
	}

	if (m_hDeviceAVI)
	{
		MCIClose( m_hDeviceAVI );
		m_hDeviceAVI = NULL;
	}

	MCICloseAll();
}

//***********************************************************************
BOOL CPHApp::GotoScene( HWND hWndPreviousScene, int iScene )
//***********************************************************************
{
	if ( !iScene )
	{
		PostMessage( GetMainWnd(), WM_CLOSE, 0, 0L );
		return( TRUE );
	}

	if (m_hBubbleWnd)
	{
		DestroyWindow(m_hBubbleWnd);
		m_hBubbleWnd = NULL;
	}
	if (m_idHintTimer)
	{
		KillTimer(GetMainWnd(), HINT_TIMER);
		m_idHintTimer = 0;
	}

	ResetKeyMap();  
	ResetJoyMap();  
	
	// You can't do a delete of the scene because the derived class's
	// destructor will get called before the base class(CScene) destructor.
	// Since the base class destructor closes the window, then
	// any messages generated by the close will not be able to
	// be sent to virtual functions in the derived class. (i.e.-WM_DESTROY)
	//if(m_lpScene)
	//	delete m_lpScene;
	if ( hWndPreviousScene )
		SendMessage( hWndPreviousScene, WM_CLOSE, 0, 0L );

	m_lpScene = new CScene();
	if (!m_lpScene)
		return(FALSE);
	if (!m_lpScene->Create(YES, GetInstance(), GetMainWnd(), iScene))
		return(FALSE);
	return( TRUE );
}

//***********************************************************************
void CPHApp::DestroyTitleBar()
//***********************************************************************
{
	if (m_hTitleBarWnd)
	{
		DestroyWindow(m_hTitleBarWnd);
		m_hTitleBarWnd = NULL;
	}
}

//***********************************************************************
BOOL CPHApp::PumpMessage()
//***********************************************************************
{
	if (!GetMessage(&m_msgCur, NULL, NULL, NULL))
	{
		return FALSE;
	}

	// handle hidden title bar
	if (TranslateTitleBar(&m_msgCur))
		return(TRUE);

	// handle any entries in our keymap
	if (TranslateKeyMap(&m_msgCur))
		return(TRUE);

	// handle any accelerators
	if (m_hAccel && TranslateAccelerator(m_hMainWnd, m_hAccel, &m_msgCur))
		return(TRUE);

	// If some child window took the message, go get another one
	if ( !DialogMsg( &m_msgCur ) )
	{
		TranslateMessage( &m_msgCur );
		DispatchMessage( &m_msgCur );
	}
	return TRUE;
}

// Main running routine until application exits
//***********************************************************************
int CPHApp::Run()
//***********************************************************************
{
	if (m_hMainWnd == NULL)
	{
		// has no main window!
		PostQuitMessage(0);
	}

	// Acquire and dispatch messages until a WM_QUIT message is received.
	for (; ;)
	{
		LONG lIdleCount = 0;
		// check to see if we can do idle work
		while (!PeekMessage(&m_msgCur, NULL, NULL, NULL, PM_NOREMOVE) &&
			OnIdle(lIdleCount++))
		{
			// more work to do
		}

		// either we have a message, or OnIdle returned false

		if (!PumpMessage())
			break;
	}

	return ExitInstance();
}

// CWinApp idle processing
//***********************************************************************
BOOL CPHApp::OnIdle(LONG lCount)
//***********************************************************************
{
	return(FALSE);
}

/***********************************************************************/
BOOL CPHApp::OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
	return(TRUE);
}

/***********************************************************************/
void CPHApp::OnActivate(HWND hWnd, UINT state, HWND hwndActDeact, BOOL fMinimized)
/***********************************************************************/
{
}

//************************************************************************
#ifdef WIN32
void CPHApp::OnActivateApp(HWND hWnd, BOOL fActivate, ULONG htaskActDeact)
#else
void CPHApp::OnActivateApp(HWND hWnd, BOOL fActivate, HTASK htaskActDeact)
#endif
//************************************************************************
{
	// Let scene do any stuff scene needs to do on app activation
	if (m_lpScene)
		m_lpScene->OnAppActivate(fActivate);
	if ( fActivate )
	{ // The app is being activated
		// Set the global MCIAVI settings the way we want them
		VideoInit( NO/*bZoomBy2*/, NO/*bFullScreen*/ );
		joySetThreshold( JOYSTICKID1, 65535/3 ); // put on a grid of 3 x 3
		joySetCapture( GetMainWnd(), JOYSTICKID1, 100/*milliseconds*/, TRUE/*bChanged*/ );

		// see if we have a caption
		if ((GetWindowLong(hWnd, GWL_STYLE) & WS_CAPTION) == 0)
		{ // no caption
			// use stack variables because of a bug I
			// remembered in PP
			BOOL fMinimized = IsIconic(hWnd);
			BOOL fMaximized = IsZoomed(hWnd);
			// if not minimized and not maximized,
			// force the app to be maximized.  We do this here
			// because when done in OnDisplayChange()
			// it caused our app to become activated.
			if (!fMinimized && !fMaximized)
				ShowWindow(hWnd, SW_SHOWMAXIMIZED);
		}

	}
	else
	{ // The app is being deactivated
		// Restore the global MCIAVI settings
		VideoDone();
		joyReleaseCapture( JOYSTICKID1 );
	}
}

//************************************************************************
void CPHApp::OnClose(HWND hWnd)
//************************************************************************
{
	LPSOUND pSound = new CSound();
	if (pSound)
	{
		pSound->StopAndFree();
		pSound->StartResourceID( IDC_CLOSE, NO/*bLoop*/, -1/*iChannel*/, GetInstance(), TRUE );
		delete pSound;
	}

	DestroyWindow(hWnd);
}

//************************************************************************
void CPHApp::OnDestroy(HWND hWnd)
//************************************************************************
{
	// the MCI devices need to be close before the main app window
	// goes away or it will cause an error!!!
	m_fAppClosing = TRUE;
	if (m_idCaptionTimer)
		KillTimer(hWnd, CAPTION_TIMER);
	if (m_idHintTimer)
		KillTimer(hWnd, HINT_TIMER);
	DestroyTitleBar();
	PostQuitMessage( 0 );
}

//************************************************************************
void CPHApp::OnNCDestroy(HWND hWnd)
//************************************************************************
{
	CloseMCIDevices();
}

/***********************************************************************/
void CPHApp::OnSize(HWND hWnd, UINT state, int cx, int cy)
/***********************************************************************/
{
	if (state != SIZE_MINIMIZED)
	{
		// Make sure the children get re-centered
		HWND hChild = GetWindow( hWnd, GW_CHILD );
		while (hChild)
		{
			if ( IsWindowVisible(hChild) )
				CenterWindow( hChild );
			hChild = GetWindow( hChild, GW_HWNDNEXT );
		}
	}
}

/***********************************************************************/
BOOL CPHApp::OnEraseBackground(HWND hWnd, HDC hDC)
/***********************************************************************/
{
	if ( m_bAppNoErase )
		{
		m_bAppNoErase = NO;
		return( TRUE ); // handle ERASEBKGND and do nothing; PAINT covers everything
		}
	else
		return FORWARD_WM_ERASEBKGND(hWnd, hDC, DefWindowProc);
}

/***********************************************************************/
int CPHApp::RealizeOurPalette(HWND hWnd, BOOL bForceBackground)
/***********************************************************************/
{
	HDC hDC;
	int iEntriesChanged;
	HPALETTE hPalOld, hPal;
		
	hPal = NULL;
	if (m_lpScene)
	{
		LPOFFSCREEN lpOffScreen = m_lpScene->GetOffScreen();
		if (lpOffScreen)
			hPal = lpOffScreen->GetPalette();
	}
	if (!hPal)
		hPal = m_hPal;

	if ( !hPal )
		return TRUE;
	if ( !( hDC = GetDC(hWnd)) )
		return TRUE;

	hPalOld = SelectPalette( hDC, hPal, bForceBackground/*bProtectPhysicalPal*/ );
	iEntriesChanged = RealizePalette( hDC );

	// JMM - Don't know why it is necessary to do this for the old palette
	// if you don't select it you will get an error in debug Windows
	SelectPalette( hDC, hPalOld, TRUE/*bProtectPhysicalPal*/ );
	//RealizePalette( hDC );

	ReleaseDC( hWnd, hDC );

	if ( iEntriesChanged )
	{ // Make sure the children get redrawn
		HWND hChild = GetWindow( hWnd, GW_CHILD );
		while (hChild)
		{
			if ( IsWindowVisible(hChild) )
				InvalidateRect( hChild, NULL, FALSE );
			hChild = GetWindow( hChild, GW_HWNDNEXT );
		}
	}
	return ( iEntriesChanged );
}

/***********************************************************************/
void CPHApp::OnPaletteChanged(HWND hWnd, HWND hWndCausedBy)
/***********************************************************************/
{ // Some window caused (or is causing) the palette to change
	if ( hWndCausedBy != hWnd )
		RealizeOurPalette( hWnd, TRUE );
}

/***********************************************************************/
BOOL CPHApp::OnQueryNewPalette(HWND hWnd)
/***********************************************************************/
{ // Receiving focus; chance to select and realize our palette
	return( RealizeOurPalette( hWnd, FALSE ) != 0 );
}

/***********************************************************************/
void CPHApp::OnRButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
#ifdef _DEBUG
	if ( CONTROL )
		PrintWindowCoordinates( hWindow );
#endif
}

static HWND hWndJoyMessage[8];
static UINT msgJoyMessage[8];
static WPARAM wParamJoyMessage[8];
static LPARAM lParamJoyMessage[8];
static ITEMID idJoyDownTimer;
static UINT iJoyDownButton;
static HWND hJoyDownTimerWnd;
static BOOL bJoyDown;
//************************************************************************
void CPHApp::ResetJoyMap( void )
//************************************************************************
{
	if ( idJoyDownTimer )
		KillTimer( hJoyDownTimerWnd, idJoyDownTimer );
	idJoyDownTimer = NULL;

	for ( int i = 0; i < 8; i++ )
		hWndJoyMessage[i] = NULL;
	bJoyDown = NO;
}

/***********************************************************************/
void CPHApp::SetJoyMessageMap( UINT iButton, HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
if ( iButton == VK_LEFT	 ) iButton = 4; else
if ( iButton == VK_RIGHT ) iButton = 5; else
if ( iButton == VK_UP	 ) iButton = 6; else
if ( iButton == VK_DOWN	 ) iButton = 7; else
if ( iButton >= 1 && iButton <= 4 ) iButton--; else
	return;

msgJoyMessage[iButton]	  = msg;
hWndJoyMessage[iButton]	  = hWindow;
wParamJoyMessage[iButton] = wParam;
lParamJoyMessage[iButton] = lParam;
}

/***********************************************************************/
BOOL CPHApp::OnJoyButtonUp(HWND hWindow, UINT iButtonCode, UINT x, UINT y)
/***********************************************************************/
{
	if ( !bJoyDown )
		return( NO );
	if ( idJoyDownTimer )
		KillTimer( hJoyDownTimerWnd, idJoyDownTimer );
	bJoyDown = NO;
	idJoyDownTimer = NULL;
	return( YES );
}

/***********************************************************************/
BOOL CPHApp::OnJoyButtonDown(HWND hWindow, UINT iButtonCode, UINT x, UINT y)
/***********************************************************************/
{
	if ( bJoyDown && (iJoyDownButton == iButtonCode) )
		return( NO );

	bJoyDown = YES;
	iJoyDownButton = iButtonCode;
	if ( (iJoyDownButton & 1) && hWndJoyMessage[0] ) // button 1
	{
		SendMessage( hWndJoyMessage[0], msgJoyMessage[0], wParamJoyMessage[0], lParamJoyMessage[0] );
		if ( !idJoyDownTimer )
		{
			hJoyDownTimerWnd = hWindow;
			idJoyDownTimer = JOY_TIMER;
			SetTimer( hJoyDownTimerWnd, idJoyDownTimer, 100/*ms*/, NULL );
		}
	}

	if ( (iJoyDownButton & 2) && hWndJoyMessage[1] ) // button 2
		SendMessage( hWndJoyMessage[1], msgJoyMessage[1], wParamJoyMessage[1], lParamJoyMessage[1] );
	
	if ( (iJoyDownButton & 4) && hWndJoyMessage[2] ) // button 3
		SendMessage( hWndJoyMessage[2], msgJoyMessage[2], wParamJoyMessage[2], lParamJoyMessage[2] );
	
	if ( (iJoyDownButton & 8) && hWndJoyMessage[3] ) // button 4
		SendMessage( hWndJoyMessage[3], msgJoyMessage[3], wParamJoyMessage[3], lParamJoyMessage[3] );
	
	return( YES );
}

/***********************************************************************/
void CPHApp::OnTimer( HWND hWnd, UINT idTimer )
/***********************************************************************/
{ 
	if (idTimer == CAPTION_TIMER)
	{
		STRING szTitle;
		DWORD dwStyle;
		RECT rWindow;

		KillTimer(m_hMainWnd, CAPTION_TIMER);
		m_idCaptionTimer = 0;

		GetClientRect(m_hMainWnd, &rWindow);
		ClientToScreen(m_hMainWnd, (LPPOINT)&rWindow.left);
		ClientToScreen(m_hMainWnd, (LPPOINT)&rWindow.right);

		GetWindowText(m_hMainWnd, szTitle, sizeof(szTitle));
		if (IsZoomed(m_hMainWnd))
			dwStyle = WS_MINIMIZEBOX;
		else
			dwStyle = WS_MAXIMIZEBOX;
		m_hTitleBarWnd = CreateWindowEx(
			0,
			"titlebar",			// lpClassName
			szTitle,			// lpWindowName
			WS_POPUP |
			WS_VISIBLE | 
			WS_CAPTION |
			WS_SYSMENU |
			dwStyle,	// dwStyle
			rWindow.left, rWindow.top,	// position of window
			RectWidth(&rWindow), GetSystemMetrics(SM_CYCAPTION),	// size of window
			m_hMainWnd,			// hWindowParent (null for tiled)
			NULL,				// hMenu (null = class menu)
			GetInstance(),		// hInstance
			NULL				// lpParam
			);
	}
	else
	if (idTimer == HINT_TIMER)
	{
		KillTimer(hWnd, HINT_TIMER);
		CreateBubble();		
	}
	if (idTimer == idJoyDownTimer)
	{
		if ( (iJoyDownButton & 1) && hWndJoyMessage[0] ) // button 1
			SendMessage( hWndJoyMessage[0], msgJoyMessage[0], wParamJoyMessage[0], lParamJoyMessage[0] );
	}
}

/***********************************************************************/
BOOL CPHApp::OnJoyMove(HWND hWindow, UINT iButtonCode, UINT x, UINT y)
/***********************************************************************/
{
#define ONE_THIRD  (65535/3)
#define TWO_THIRDS (65535-ONE_THIRD)

if ( x < ONE_THIRD	&& hWndJoyMessage[4] ) // left
	SendMessage( hWndJoyMessage[4], msgJoyMessage[4], wParamJoyMessage[4], lParamJoyMessage[4] );
else
if ( x > TWO_THIRDS && hWndJoyMessage[5] ) // right
	SendMessage( hWndJoyMessage[5], msgJoyMessage[5], wParamJoyMessage[5], lParamJoyMessage[5] );

if ( y < ONE_THIRD	&& hWndJoyMessage[6] ) // up
	SendMessage( hWndJoyMessage[6], msgJoyMessage[6], wParamJoyMessage[6], lParamJoyMessage[6] );
else
if ( y > TWO_THIRDS && hWndJoyMessage[7] ) // down
	SendMessage( hWndJoyMessage[7], msgJoyMessage[7], wParamJoyMessage[7], lParamJoyMessage[7] );

return( YES );
}

/***********************************************************************/
BOOL CPHApp::OnJoyZMove(HWND hWindow, UINT iButtonCode, UINT x, UINT y)
/***********************************************************************/
{
return( YES );
}

//************************************************************************
void CPHApp::ResetKeyMap( void )
//************************************************************************
{
	m_iKeyMapEntries = 0;
}

//************************************************************************
BOOL CPHApp::SetKeyMapEntry(HWND hWnd, UINT vk, UINT msg, WPARAM wParam, LPARAM lParam, BOOL fOnDown)
//************************************************************************
{
	if (m_iKeyMapEntries < MAX_KEYS)
	{
		m_KeyMap[m_iKeyMapEntries].hWnd = hWnd;
		m_KeyMap[m_iKeyMapEntries].vk = vk;
		m_KeyMap[m_iKeyMapEntries].msg = msg;
		m_KeyMap[m_iKeyMapEntries].wParam = wParam;
		m_KeyMap[m_iKeyMapEntries].lParam = lParam;
		m_KeyMap[m_iKeyMapEntries].fOnDown = fOnDown;
		++m_iKeyMapEntries;
		return(TRUE);
	}
	else
		return(FALSE);
}

//************************************************************************
BOOL CPHApp::RemoveKeyMapEntry(HWND hWnd, UINT vk, BOOL fOnDown)
//************************************************************************
{
	int i;

	for (i = 0; i < m_iKeyMapEntries; ++i)
	{
		// make sure the window in this entry exists
		if ((m_KeyMap[i].hWnd == hWnd) && (m_KeyMap[i].vk == vk))
		{
			if (!m_KeyMap[i].msg || (m_KeyMap[i].fOnDown == fOnDown))
			{
				m_KeyMap[i].hWnd = NULL;
				return(TRUE);
			}
		}
	}
	return(FALSE);
}

//************************************************************************
BOOL CPHApp::TranslateKeyMap(LPMSG pMsg)
//************************************************************************
{
	// We only handle WM_KEYDOWN and WM_KEYUP messages
	BOOL fDown = pMsg->message == WM_KEYDOWN;
	if ( !fDown && !(pMsg->message == WM_KEYUP))
		return(FALSE);

	// get the key we are looking for
	UINT vk = (UINT)pMsg->wParam;
	for ( int i = 0; i < m_iKeyMapEntries; ++i)
	{
		// make sure the window in this entry exists and it matches
		if (m_KeyMap[i].hWnd && IsWindow(m_KeyMap[i].hWnd) && (m_KeyMap[i].vk == vk))
		{
			// does somebody wants this keystroke translated to another message?
			if (m_KeyMap[i].msg)
			{
				// if translating to another message, see if they wanted
				// the message on an up or down
				if (m_KeyMap[i].fOnDown == fDown)
				{
					SendMessage(m_KeyMap[i].hWnd, m_KeyMap[i].msg, m_KeyMap[i].wParam, pMsg->lParam/*m_KeyMap[i].lParam*/);
					return(TRUE);
				}
			}
			else
			{
				// somebody just wants the keystroke so send it on to the
				// window that would like it
				SendMessage(m_KeyMap[i].hWnd, pMsg->message, pMsg->wParam, pMsg->lParam);
				return(TRUE);
			}
		}
	}

	return(FALSE);
}

//************************************************************************
BOOL CPHApp::TranslateTitleBar(LPMSG pMsg)
//************************************************************************
{
	if (GetWindowLong(m_hMainWnd, GWL_STYLE) & WS_CAPTION)
		return(FALSE);

	// We handle WM_KEYDOWN and WM_KEYUP messages
	if (pMsg->message == WM_MOUSEMOVE)
	{
		POINT pt;
		RECT rWindow;

		pt.x = LOWORD(pMsg->lParam);
		pt.y = HIWORD(pMsg->lParam);
		ClientToScreen(pMsg->hwnd, &pt);

		if (m_hTitleBarWnd)
		{
			GetWindowRect(m_hTitleBarWnd, &rWindow);
			if (pt.y > rWindow.bottom)
			{
				DestroyTitleBar();
			}
		}
		else
		{
			if (m_idCaptionTimer)
			{
				KillTimer(m_hMainWnd, CAPTION_TIMER);
				m_idCaptionTimer = 0;
			}

			GetClientRect(m_hMainWnd, &rWindow);
			ClientToScreen(m_hMainWnd, (LPPOINT)&rWindow.left);
			ClientToScreen(m_hMainWnd, (LPPOINT)&rWindow.right);

			if (pt.x >= rWindow.left &&
				pt.x < rWindow.right &&
				pt.y <= (rWindow.top+1))
			{
				m_idCaptionTimer = SetTimer( m_hMainWnd, CAPTION_TIMER, CAPTION_DELAY, NULL );
			}
		}
	}
	return(FALSE);
}

//************************************************************************
void CPHApp::OnDisplayChange(HWND hWnd, BYTE cBitsPerPixel, int cxScreen, int cyScreen)
//************************************************************************
{
	// 640x480: Remove caption
	// others: Add Caption
	ModifyCaption(hWnd, (cxScreen > 640) && (cyScreen > 480));
}

//************************************************************************
void CPHApp::ModifyCaption(HWND hWnd, BOOL fWantCaption)
//************************************************************************
{
	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
	BOOL fHaveCaption = (dwStyle & WS_CAPTION) == WS_CAPTION;
	if (fHaveCaption == fWantCaption)
		return;

	if (fHaveCaption)
	{
		// strip off our caption bits
		dwStyle &= ~WS_PHCAPTION;
	}
	else
	{
		// add caption bits
		dwStyle |= WS_PHCAPTION;
	}
	SetWindowLong(hWnd, GWL_STYLE, dwStyle);
	// SWP_FRAMECHANGED is the key here that causes
	// WM_NCCALCSIZE to happen
	SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
		SWP_FRAMECHANGED|SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
}

//************************************************************************
BOOL CPHApp::OnSetCursor(HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg)
//************************************************************************
{
	STRING szClass;

	if (m_idHintTimer)
	{
		KillTimer(hWnd, HINT_TIMER);
		m_idHintTimer;
	}
	if (m_hHintWnd != hWndCursor || !m_hBubbleWnd)
	{
		if (m_hBubbleWnd)
		{
			DestroyWindow(m_hBubbleWnd);
			m_hBubbleWnd = NULL;
		}
		m_hHintWnd = NULL;
		GetClassName(hWndCursor, szClass, sizeof(szClass));

		if (!lstrcmp(szClass, "meter") && m_lpScene)
		{
			if (IsChild(m_lpScene->GetWindow(), hWndCursor))
			{
				DWORD dwStyle = GetWindowLong(hWndCursor, GWL_STYLE);
				if (dwStyle & (WS_HINTCAPTION|WS_HINTRESOURCE))
				{
					m_hHintWnd = hWndCursor;
  					m_idHintTimer = SetTimer( m_hMainWnd, HINT_TIMER, HINT_DELAY, NULL );
				}
			}
		}
	}
	return FORWARD_WM_SETCURSOR(hWnd, hWndCursor, codeHitTest, msg, DefWindowProc);
}

//************************************************************************
long CPHApp::WindowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************
{
	switch ( msg )
	{
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_ACTIVATE, OnActivate);
		HANDLE_MSG(hWnd, WM_ACTIVATEAPP, OnActivateApp);
		HANDLE_MSG(hWnd, WM_DISPLAYCHANGE, OnDisplayChange);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_CLOSE, OnClose);
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_NCDESTROY, OnNCDestroy);
		HANDLE_MSG(hWnd, WM_SIZE, OnSize);
		HANDLE_MSG(hWnd, WM_ERASEBKGND, OnEraseBackground);
		HANDLE_MSG(hWnd, WM_PALETTECHANGED, OnPaletteChanged);
		HANDLE_MSG(hWnd, WM_QUERYNEWPALETTE, OnQueryNewPalette);
		#ifdef _DEBUG
		HANDLE_MSG(hWnd, WM_RBUTTONUP, OnRButtonUp);
		#endif
		HANDLE_MSG(hWnd, MM_MCINOTIFY, MCIHandleNotify);
		HANDLE_MSG(hWnd, MM_JOY1BUTTONDOWN, OnJoyButtonDown);
		HANDLE_MSG(hWnd, MM_JOY1BUTTONUP, OnJoyButtonUp);
		HANDLE_MSG(hWnd, MM_JOY1MOVE, OnJoyMove);
		HANDLE_MSG(hWnd, MM_JOY1ZMOVE, OnJoyZMove);      
		HANDLE_MSG(hWnd, WM_TIMER, OnTimer);
		HANDLE_MSG(hWnd, WM_SETCURSOR, OnSetCursor);

		default:
		#ifdef VOICE_RECO
			// Voice reco stuff
			long ProcessICSSReplyMsg(HWND hWnd, UINT wFuncId, LONG lData);
			extern UINT ICSSMessageId;
			if (msg == ICSSMessageId)
			/******************************************************************************/
			/* This function call is used to process the asynchronous return messages from*/
			/* asynchronous ICSS calls. Note that the ICSSMessageId was registered during */
			/* application initialization in InitAllInstances.							  */
			/******************************************************************************/
				return (ProcessICSSReplyMsg (hWnd, wParam, lParam));
			else
		#endif
				return (DefWindowProc (hWnd, msg, wParam, lParam));
	}
}

//************************************************************************
long WINPROC EXPORT ApplicationWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************
{
	CPHApp *pApp = GetApp();
	if (pApp)
		return pApp->WindowProc(hWnd, msg, wParam, lParam);
	else
		return DefWindowProc( hWnd, msg, wParam, lParam );
}

//************************************************************************
void Title_OnSysCommand(HWND hWindow, UINT cmd, int x, int y)
//************************************************************************
{
	if (cmd == SC_SIZE ||
		cmd == SC_MOVE ||
		cmd == SC_MINIMIZE ||
		cmd == SC_MAXIMIZE ||
		cmd == SC_CLOSE ||
		cmd == SC_RESTORE)
	{
		GetApp()->DestroyTitleBar();
		//if (cmd == SC_MINIMIZE)
		//	GetApp()->ModifyCaption(GetApp()->GetMainWnd(), TRUE);
		FORWARD_WM_SYSCOMMAND(GetApp()->GetMainWnd(), cmd, x, y, SendMessage);
	}
	else
		FORWARD_WM_SYSCOMMAND(hWindow, cmd, x, y, DefWindowProc);
}

//************************************************************************
void Title_OnInitMenuPopup(HWND hWindow, HMENU hMenu, int item, BOOL fSystemMenu)
//************************************************************************
{
	if (fSystemMenu)
	{
		BOOL fMaximized = IsZoomed(GetApp()->GetMainWnd());
		BOOL fMinimized = IsIconic(GetApp()->GetMainWnd());
		EnableMenuItem(hMenu, SC_SIZE, MF_BYCOMMAND|MF_GRAYED);		
		EnableMenuItem(hMenu, SC_MOVE, MF_BYCOMMAND|MF_GRAYED);
		if (fMinimized)		
			EnableMenuItem(hMenu, SC_MINIMIZE, MF_BYCOMMAND|MF_GRAYED);		
		else
			EnableMenuItem(hMenu, SC_MINIMIZE, MF_BYCOMMAND|MF_ENABLED);		
		if (fMaximized)		
			EnableMenuItem(hMenu, SC_MAXIMIZE, MF_BYCOMMAND|MF_GRAYED);		
		else
			EnableMenuItem(hMenu, SC_MAXIMIZE, MF_BYCOMMAND|MF_ENABLED);		
		EnableMenuItem(hMenu, SC_CLOSE, MF_BYCOMMAND|MF_ENABLED);		
//		if (fMinimized || fMaximized)
//			EnableMenuItem(hMenu, SC_RESTORE, MF_BYCOMMAND|MF_ENABLED);		
//		else
			EnableMenuItem(hMenu, SC_RESTORE, MF_BYCOMMAND|MF_GRAYED);		
	}
	else
	{
		FORWARD_WM_INITMENUPOPUP(hWindow, hMenu, item, fSystemMenu, DefWindowProc);
	}
}

//************************************************************************
UINT Title_OnNCHitTest(HWND hWindow, int x, int y)
//************************************************************************
{
	UINT code = FORWARD_WM_NCHITTEST(hWindow, x, y, DefWindowProc);
	if (code == HTCAPTION)
		code = HTNOWHERE;
	return(code);
}

//************************************************************************
long WINPROC EXPORT TitleBarWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************
{
	switch (msg)
	{
		HANDLE_MSG(hWnd, WM_SYSCOMMAND, Title_OnSysCommand);
		HANDLE_MSG(hWnd, WM_INITMENUPOPUP, Title_OnInitMenuPopup);
		HANDLE_MSG(hWnd, WM_NCHITTEST, Title_OnNCHitTest);
		default:
			return DefWindowProc( hWnd, msg, wParam, lParam );
	}

}

//***********************************************************************
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
//***********************************************************************
{
	int nReturnCode = -1;
	CPHApp *pApp = GetApp();
	pApp->WinInit(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	// App global initializations (rare)
	if (hPrevInstance == NULL && !pApp->InitApplication())
		goto InitFailure;

	// Perform specific initializations
	if (!pApp->InitInstance())
	{
		if (pApp->GetMainWnd() != NULL)
		{
			DestroyWindow(pApp->GetMainWnd());
		}
		nReturnCode = pApp->ExitInstance();
		goto InitFailure;
	}
	nReturnCode = pApp->Run();

InitFailure:
	return nReturnCode;
}

//////////////////////////////////////////////////////////////////////////////

#define CX_BUFFER               8
#define CY_BUFFER               4
#define SPACE_BUFFER            2

LRESULT CALLBACK BubbleWndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
//////////////////////////////////////////////////////////////////////////////
//  This is the bubble windowproc. It's sole purpose in life is to display  //
//  the title text in an attractive manner.                                 //
//////////////////////////////////////////////////////////////////////////////
{
    LRESULT     lResult = 0L;
    HDC         hDC;
    PAINTSTRUCT ps;
    RECT        rect;
    STRING      szTemp;
    HGDIOBJ     hPrevBrush;    
	COLORREF	oldColor;

    switch( msg )
    {
        case WM_PAINT:
            hDC = BeginPaint( hWnd, (LPPAINTSTRUCT)&ps );
            GetClientRect( hWnd, (LPRECT)&rect );    
            GetWindowText( hWnd, (LPTSTR)szTemp, sizeof(szTemp) );

            hPrevBrush = SelectObject( hDC, GetStockObject( NULL_BRUSH ));
            SelectObject( hDC, GetStockObject(ANSI_VAR_FONT));
            Rectangle( hDC, 0, 0, rect.right, rect.bottom );

            SetBkMode( hDC, TRANSPARENT );

			if (GetApp()->GetWinVersion() == WV_WIN95)
				oldColor = SetTextColor( hDC, GetSysColor(COLOR_INFOTEXT) );
            TextOut( hDC, CX_BUFFER / 2, CY_BUFFER / 2, 
                (LPTSTR)szTemp, GetWindowTextLength( hWnd ));
			if (GetApp()->GetWinVersion() == WV_WIN95)
				SetTextColor( hDC, oldColor );

            SelectObject( hDC, hPrevBrush );
            EndPaint( hWnd, (LPPAINTSTRUCT)&ps );
            break;

        default:
            lResult = DefWindowProc( hWnd, msg, wParam, lParam );
            break;
    }
    return( lResult );
}

//************************************************************************
void CPHApp::CreateBubble()
//************************************************************************
{
    RECT  rect;
    HDC   hDC;
    SIZE  size;
    int   x, y, iCtrlExt, iScreenExtent, id;
	STRING szHintTitle;
	DWORD dwStyle;

    if( m_hBubbleWnd )
    {    
        DestroyWindow( m_hBubbleWnd );
        m_hBubbleWnd = NULL;
    }

    if( !( IsWindow( m_hHintWnd )))
	{
		m_hHintWnd = NULL;
        return;
	}

    // Calculate the size of the window based on the title
	dwStyle = GetWindowLong(m_hHintWnd, GWL_STYLE);
	if (dwStyle & WS_HINTCAPTION)
		GetWindowText(m_hHintWnd, szHintTitle, sizeof(szHintTitle));
	else
	{
		id = GetDlgCtrlID(m_hHintWnd);
		szHintTitle[0] = '\0';
		LoadString( GetInstance(), id, szHintTitle, sizeof(szHintTitle) );
	}

	// no hint to display
	if (!lstrlen(szHintTitle))
		return;

    hDC = GetDC( NULL );
    SelectObject( hDC, GetStockObject( ANSI_VAR_FONT ));
    GetTextExtentPoint( hDC, ( const char * )szHintTitle, 
        lstrlen(szHintTitle), ( LPSIZE )&size );
    ReleaseDC( NULL, hDC );

    size.cx += CX_BUFFER;
    size.cy += CY_BUFFER;

    // Calculate the X/Y position of the window
    GetWindowRect( m_hHintWnd, &rect );

	if (TRUE)
    {	 // hint above
        iCtrlExt = rect.right - rect.left;

        x = max( CX_BUFFER, rect.left -(( size.cx - iCtrlExt ) / 2 ));

        if(( x + size.cx ) >( iScreenExtent = GetSystemMetrics( SM_CXSCREEN )) )
            // Won't fit on screen, move it to the left.
            x = ( iScreenExtent - ( size.cx + CX_BUFFER ));

        y = rect.bottom + SPACE_BUFFER;

        if(( y + size.cy ) >( iScreenExtent= GetSystemMetrics( SM_CYSCREEN )))
        {
            y = rect.top - ( SPACE_BUFFER + size.cy );
        }
    }
    else                // it may be a custom toolbar
    {
        // locate hint to the left or right of tool
        x = rect.right + CX_BUFFER / 2;

        if(( x + size.cx ) >( iScreenExtent = GetSystemMetrics( SM_CXSCREEN )))
            // Won't fit on screen, move it to the left.
            x = rect.left - ( size.cx + CX_BUFFER / 2 );

        iCtrlExt = rect.bottom - rect.top;
        y = rect.top + (( iCtrlExt - size.cy ) / 2 );   // bubble centered vertically next to button
    }

    m_hBubbleWnd = ::CreateWindowEx( 0, ( LPTSTR )"bubble", 
        ( const char * )szHintTitle, WS_POPUP | WS_CHILD, 
        x, y, size.cx, size.cy, GetMainWnd(), NULL, 
        GetInstance(), NULL );

    if( m_hBubbleWnd )
        ShowWindow( m_hBubbleWnd, SW_SHOWNOACTIVATE );
}

