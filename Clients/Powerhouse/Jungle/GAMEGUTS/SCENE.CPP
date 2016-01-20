#include <windows.h>
#include "proto.h"
#include "scene.h"
#include "sprite.h"

// superclass window proc
WNDPROC CScene::m_lpOldWndProc = NULL;
// temporary scene pointer for attach on WM_NCCREATE
LPSCENE CScene::m_lpScene = NULL;
// the scene's midi theme 
HMCI CScene::m_hMidiTheme = NULL;

//************************************************************************
CScene::CScene()
//************************************************************************
{
	m_nSceneNo = -1;
	m_lpOffScreen = NULL;
	m_bSceneNoErase = NO;
	m_bNoErase = YES;
	m_hWnd = NULL;
	m_pAnimator = NULL;
	m_idAnimatorTimer = 0;
	m_pSound = NULL;
	m_bDestructing = FALSE;
}

//************************************************************************
CScene::~CScene()
//************************************************************************
{
	m_bDestructing = TRUE;
	if (m_lpOffScreen)
		delete m_lpOffScreen;
	if (m_pAnimator)
		delete m_pAnimator;
	if (m_pSound)
		delete m_pSound;
	if (m_hWnd)
		OnClose(m_hWnd);
}

//************************************************************************
void CScene::Attach(HWND hWnd)
//************************************************************************
{
	// link class to window
	m_hWnd = hWnd;
	// link window to class
	SetWindowLong(hWnd, DWL_USER, (long)this);
}

//************************************************************************
void CScene::Detach()
//************************************************************************
{
	// unlink window to class
	SetWindowLong(m_hWnd, DWL_USER, (long)NULL);
	// unlink class to window
	m_hWnd = NULL;
}

//************************************************************************
LPSCENE CScene::GetScene(HWND hWindow)
//************************************************************************
{
	return((LPSCENE)GetWindowLong(hWindow, DWL_USER));
}

//************************************************************************
HPALETTE CScene::GetPalette(void)
//************************************************************************
{
	if (m_lpOffScreen)
		return(m_lpOffScreen->GetPalette());
	else
	#ifdef USE_WINTOON
	if (GetToon())
		return(GetToon()->GetPalette());
	else
	#endif
		return(GetApp()->m_hPal);
}

//************************************************************************
BOOL CScene::Create(int iModeless, HINSTANCE hInstance, HWND hWndParent, int iScene)
//************************************************************************
{					 
	// this is setup temporarily so that we can attach the class to the
	// window in the WM_NCCREATE, don't ever rely on this being set
	// instead you should always be able to call CScene::GetScene(hWnd)
	m_nSceneNo = iScene;
	m_lpScene = this;
	HWND hWnd = DialogStart( iModeless, hInstance, hWndParent, iScene, SceneDialogProc );
	return(hWnd != NULL);
}

/************************************************************************/
BOOL CScene::OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	// Force the caption flag off for win95
	DWORD dwStyle = GetWindowLong( hWnd, GWL_STYLE );
	SetWindowLong( hWnd, GWL_STYLE, dwStyle & (~WS_CAPTION) );

	// Get the command string
	STRING szString;
	GetWindowText( hWnd, szString, sizeof(STRING)-1 );
	FNAME szMidi;
	GetStringParm( szString, 1/*nIndex*/, ',', szMidi );
	LPLONG lpSwitches = ExtractSwitches( szString );

	// Create the sound class with or w/o wavemix
	m_pSound = new CSound((BOOL)GetSwitchValue( 'w', lpSwitches));
	if (m_pSound)
		m_pSound->Open( NULL/*"powersnd.dll"*/ );

	if ( lpSwitches )
		FreeUp( (LPTR)lpSwitches );

	// Fix the coordinates from the resource file
	RepositionDialogControls( hWnd );

	// Center the scene within the app's client area
	CenterWindow( hWnd );

	// This should not be necessary, since it's done in the OnClose()
	if ( m_hMidiTheme )
	{ // close any looping midi theme
		MCIClose( m_hMidiTheme );
		m_hMidiTheme = NULL;
	}

	if ( *szMidi )
	{ // The scene has a midi theme, so start it
		LPLONG lpMidiSwitches = ExtractSwitches( szMidi );
		if ( m_hMidiTheme = MCIOpen( GetApp()->m_hDeviceMID, szMidi, NULL/*lpAlias*/ ) )
		{
			if ( lpMidiSwitches )
				MCILoop( m_hMidiTheme, (BOOL)GetSwitchValue( 'l', lpMidiSwitches ) );
			if ( !MCIPlay( m_hMidiTheme, GetApp()->GetMainWnd() ) )
			{
				MCIClose( m_hMidiTheme );
				m_hMidiTheme = NULL;
			} // if it's not looping, it will close itself when it ends
		}
		if ( lpMidiSwitches )
			FreeUp( (LPTR)lpMidiSwitches );
	}

	// if we have an animation object then setup a timer for it
	m_idAnimatorTimer = SetTimer(hWnd, ANIMATOR_TIMER_ID, 1, NULL);

	return(TRUE);
}

/************************************************************************/
void CScene::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
}

//************************************************************************
void CScene::OnClose(HWND hWnd)
//************************************************************************
{
	if ( m_hMidiTheme )
	{ // close any looping midi theme
		MCIClose( m_hMidiTheme );
		m_hMidiTheme = NULL;
	}
	GetApp()->m_bAppNoErase = m_bSceneNoErase = m_bNoErase; // don't want the app background to paint in between
	GetApp()->ResetJoyMap();
	DialogEnd( hWnd, TRUE );
}

//************************************************************************
void CScene::OnDestroy(HWND hWnd)
//************************************************************************
{
	if (m_idAnimatorTimer)
		KillTimer(hWnd, m_idAnimatorTimer);
}

/***********************************************************************/
void CScene::OnSize(HWND hWnd, UINT state, int cx, int cy)
/***********************************************************************/
{
	STRING szString;
	FNAME szDib;
	PTOON pToon;

	// if there is a toon associated with this scene, there
	// is no need to create an offscreen, since a toon already
	// has all the offscreen functionality.
	if ( !(pToon = GetToon()) )
	{
		// Instantiate the COffScreen object for this scene
		// if it has not already been done
		GetWindowText( hWnd, szString, sizeof(STRING)-1 );
		GetStringParm( szString, 0/*nIndex*/, ',', szDib );
		if ( !m_lpOffScreen )
		{
			// /p switch sets up offscreen to use identity palette
			// created from the read-only dib instead of the
			// common application palette
			LPLONG lpSwitches = ExtractSwitches( szString );
			if (GetSwitchValue('p', lpSwitches))
				m_lpOffScreen = new COffScreen();
			else
				m_lpOffScreen = new COffScreen(GetApp()->m_hPal);
			FreeUp(lpSwitches);
		}

		// if we don't have enough memory to instantiate
		// the offscreen, then we are done
		if ( !m_lpOffScreen )
				return;

		// load the read-only background dib if it has not been done already
		if ( !m_lpOffScreen->GetReadOnlyDIB() )
		{
			if ( *szDib )
				m_lpOffScreen->Load( szDib );
			else
				m_lpOffScreen->Load( GetWindowInstance(hWnd), m_nSceneNo );
			// copy the colors to the stage dib to be used with WinG
			m_lpOffScreen->CopyColors();
		}

		// if we were unsuccessful, then clean up and bail out
		if ( !m_lpOffScreen->GetReadOnlyDIB() )
		{
			delete m_lpOffScreen;
			m_lpOffScreen = NULL;
			return;
		}

		// Resize the stage dib and create the WinG bitmap
		m_lpOffScreen->Resize( cx, cy );
		// Copy bits from the read-only dib to the WinG bitmap
		m_lpOffScreen->CopyBits();
	}

	// setup animator object to be used with sprite engine
	// if it has not been setup already. 
	if (!m_pAnimator)
	{
		if (pToon)
			m_pAnimator = new CAnimator(pToon);
		else
			m_pAnimator = new CAnimator(hWnd, m_lpOffScreen);
	}
}

/***********************************************************************/
BOOL CScene::OnEraseBkgnd(HWND hDlg, HDC hDC)
/***********************************************************************/
{
	if ( m_bSceneNoErase )
		{
		m_bSceneNoErase = NO;
		return( TRUE ); // handle ERASEBKGND and do nothing; PAINT covers everything
		}
	return( FALSE );
}

//************************************************************************
void CScene::OnPaint( HWND hWnd )
//************************************************************************
{
	HDC hDC;
	PAINTSTRUCT ps;
	if ( !(hDC = BeginPaint( hWnd, &ps )) )
		return;
	Paint( hDC, &ps.rcPaint );
	EndPaint( hWnd, &ps );
}

//************************************************************************
void CScene::Paint( HDC hDC, LPRECT lpRect, LPPOINT lpDstPt )
//************************************************************************
{
	if ( m_lpOffScreen )
		m_lpOffScreen->DrawRect( hDC, lpRect, lpDstPt );
}

/***********************************************************************/
BOOL CScene::GetDIBs( LPPDIB lppCleanDIB, LPPDIB lppDirtyDIB, HDC FAR * lpDC )
/***********************************************************************/
{
	LPOFFSCREEN lpOffScreen = GetOffScreen();
	if ( !lpOffScreen )
	{
		if ( lppCleanDIB )
			*lppCleanDIB = NULL;
		if ( lppDirtyDIB )
			*lppDirtyDIB = NULL;
		if ( lpDC )
			*lpDC = NULL;
		return( NO );
	}
	else
	{
		if ( lppCleanDIB )
			*lppCleanDIB = lpOffScreen->GetReadOnlyDIB();
		if ( lppDirtyDIB )
			*lppDirtyDIB = lpOffScreen->GetWritableDIB();
		if ( lpDC )
			*lpDC = lpOffScreen->GetDC();
		return( YES );
	}
}

/***********************************************************************/
void CScene::OnRButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
#ifdef _DEBUG
	if ( CONTROL )
		PrintWindowCoordinates( hWindow );
#endif
}

/***********************************************************************/
BOOL CScene::OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{			  
	return (FALSE);
}

/***********************************************************************/
LONG CScene::WindowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch (msg)
	{
		HANDLE_MSG(hWnd, WM_NCCREATE, OnNCCreate);	
		HANDLE_MSG(hWnd, WM_NCDESTROY, OnNCDestroy);

		default:
			return((*m_lpOldWndProc)(hWnd, msg, wParam, lParam));
		break;
	}
}

/***********************************************************************/
BOOL CScene::OnNCCreate(HWND hWnd, CREATESTRUCT FAR* lpCreateStruct)
/***********************************************************************/
{
	Attach(hWnd);
	return FORWARD_WM_NCCREATE(hWnd, lpCreateStruct, m_lpOldWndProc);
}

/***********************************************************************/
void CScene::OnNCDestroy(HWND hWnd)
/***********************************************************************/
{
	Detach();
	FORWARD_WM_NCDESTROY(hWnd, m_lpOldWndProc);
	// make sure this dialog gets removed from dialog list
	// this is needed for DLL's where the dialog list
	// can possibly stick around.  All is cool if dialogs always
	// get destroyed through DialogEnd(), however that is not
	// always the case. (i.e. - when you close the app)
	DialogRemove(hWnd);
	if (!m_bDestructing)
		delete this;
}

//************************************************************************
void CScene::OnTimer(HWND hWnd, UINT id)
//************************************************************************
{
	if (id == m_idAnimatorTimer)
	{
		if (m_pAnimator)
			m_pAnimator->TimeSlice();
	}
}

//************************************************************************
void CScene::OnAppActivate(BOOL fActivate)
//************************************************************************
{
	// activate sound mixer
	if (m_pSound)
		m_pSound->Activate(fActivate);
	if (fActivate)
	{
		if (!m_idAnimatorTimer)
			m_idAnimatorTimer = SetTimer(m_hWnd, ANIMATOR_TIMER_ID, 1, NULL);
	}
	else
	{
		if (m_idAnimatorTimer)
		{
			KillTimer(m_hWnd, m_idAnimatorTimer);
			m_idAnimatorTimer = 0;
		}
	}
}

/***********************************************************************/
BOOL CScene::DialogProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch (msg)
	{
		HANDLE_DLGMSG(hDlg, WM_INITDIALOG, OnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, OnCommand);
		HANDLE_DLGMSG(hDlg, WM_CLOSE, OnClose);
		HANDLE_DLGMSG(hDlg, WM_SIZE, OnSize);
		HANDLE_DLGMSG(hDlg, WM_ERASEBKGND, OnEraseBkgnd);
		HANDLE_DLGMSG(hDlg, WM_PAINT, OnPaint);
		#ifdef _DEBUG
		HANDLE_DLGMSG(hDlg, WM_RBUTTONUP, OnRButtonUp);
		#endif
		HANDLE_DLGMSG(hDlg, WM_DESTROY, OnDestroy);
		HANDLE_DLGMSG(hDlg, WM_TIMER, OnTimer);
		HANDLE_DLGMSG(hDlg, WM_KEYDOWN, OnKey);

		default:
			return(OnMessage(hDlg, msg, wParam, lParam));
		break;
	}
	return(FALSE);
}

/***********************************************************************/
LONG WINPROC EXPORT SceneWindowProc( HWND hWnd, UINT msg, WPARAM wParam,
							  LPARAM lParam )
/***********************************************************************/
{
	LPSCENE lpScene;

	if (msg == WM_NCCREATE) // first message a window gets
	{
		lpScene = CScene::m_lpScene;
		CScene::m_lpScene = NULL;
	}
	else
		lpScene = CScene::GetScene(hWnd);

	if (lpScene)
		return(lpScene->WindowProc(hWnd, msg, wParam, lParam));
	else
		return((*CScene::m_lpOldWndProc)(hWnd, msg, wParam, lParam));
}

/***********************************************************************/
BOOL WINPROC EXPORT SceneDialogProc( HWND hDlg, UINT msg, WPARAM wParam,
							  LPARAM lParam )
/***********************************************************************/
{
	LPSCENE lpScene = CScene::GetScene(hDlg);
	if (lpScene)
		return(lpScene->DialogProc(hDlg, msg, wParam, lParam));
	else
		return(FALSE);
}
