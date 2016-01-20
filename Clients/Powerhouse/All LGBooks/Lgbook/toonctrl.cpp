#include <windows.h>
#include "lgbook.h"
#include "control.h" 
#include "mmsystem.h"
    
#define WM_TOONDRAW	(WM_USER + 1)

class CMyToon : public CToon
{
public:
	CMyToon(HWND hWindow, int idCursor, int idNullCursor, HINSTANCE hInstance, HPALETTE hPalette = NULL)
		: CToon(hWindow, idCursor, idNullCursor, hInstance, hPalette) {}
	BOOL OnNotify(CToon::NotifyCode code);
protected:
	HWND GetParentDlg() { return ::GetParent(::GetParent(m_hWnd)); }
};  

BOOL CMyToon::OnNotify(CToon::NotifyCode code)
{
	HWND hWindow = GetWindow();
	FORWARD_WM_COMMAND(GetParentDlg(), GET_WINDOW_ID(hWindow), hWindow, code, SendMessage);
	return(TRUE);
}
            
/***********************************************************************/
void SetToon(HWND hWindow, PTOON pToon)
/***********************************************************************/
{  
	SetWindowLong(hWindow, GWL_TOONPTR, (long)pToon);
}
                        
/***********************************************************************/
PTOON GetToon(HWND hWindow)
/***********************************************************************/
{  
	return((PTOON)GetWindowLong(hWindow, GWL_TOONPTR));
}
                        
/***********************************************************************/
BOOL Toon_Init(HWND hWindow)
/***********************************************************************/
{  
	PTOON pToon;
	int	nLanguageOffset = 0;

	pToon = GetToon(hWindow);
	if (!pToon)
		return(FALSE);

	LPLGBAPP pApp = (LPLGBAPP)GetApp();
	if (pApp->GetLanguage() == CLGBApp::Spanish)
		nLanguageOffset = IDD_SPANISHOFFSET;
    SetToon(hWindow, pToon); 
    if (!pToon->Create(WM_TOONDRAW, nLanguageOffset))
    {
    	delete pToon;
    	SetToon(hWindow, NULL);
    	return(FALSE);
    }

	return(TRUE);
}

/***********************************************************************/
LOCAL BOOL Toon_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{                          
	PTOON pToon;

	// create a toon info object
	pToon = (PTOON)new CMyToon(hWindow, ID_POINTER, ID_NULLCURSOR, GetWindowInstance(hWindow), GetApp()->m_hPal);
	if (!pToon) 
	{
		Print("Out of memory");
		return(FALSE);
	}

	SetToon(hWindow, pToon);
    if (!pToon->CreateToon())
    {
    	delete pToon;
    	SetToon(hWindow, NULL);
    	return(FALSE);
    }
        
	return(TRUE);
}

/***********************************************************************/
LOCAL void Toon_OnDestroy(HWND hWindow)
/***********************************************************************/
{           
	PTOON pToon = GetToon(hWindow);
	if (pToon)
	{
		delete pToon;   
		SetToon(hWindow, NULL);
	}
}

/***********************************************************************/
BOOL Toon_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
	return(TRUE);
}

/***********************************************************************/
void Toon_OnPaint(HWND hWindow)
/***********************************************************************/
{
	PTOON pToon = GetToon(hWindow);
	if (pToon)
		pToon->OnPaint();
	else
		FORWARD_WM_PAINT(hWindow, ToonDefWindowProc);
}

/***********************************************************************/
LOCAL void Toon_OnSize(HWND hWindow, UINT state, int cx, int cy)
/***********************************************************************/
{                  
	//static BOOL fResizing = FALSE;

   	//PTOON pToon = GetToon(hWindow);
	//if (pToon && !fResizing)
	//{
		// the window must be the same size as the toon
	//	if ((cx != pToon->GetWidth()) || (cy != pToon->GetHeight()))
	//	{          
	//		fResizing = TRUE;
	//		SetWindowPos(hWindow, NULL, 0, 0, pToon->GetWidth(), pToon->GetHeight(),
	//					SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE); 
	//		fResizing = FALSE;
	//	}
	//}

	FORWARD_WM_SIZE(hWindow, state, cx, cy, DefWindowProc);
}

/***********************************************************************/
LOCAL BOOL Toon_OnWindowPosChanging( HWND hWindow, LPWINDOWPOS lpWindowPos )
/***********************************************************************/
{
	int cx, cy;

	if ( lpWindowPos->flags & SWP_NOSIZE )
		return( YES );

	//PTOON pToon = GetToon(hWindow);
	//if (!pToon)
	//	return(YES);

	cx = 640/*(pToon->GetWidth()*/;
	cy = 480/*pToon->GetHeight()*/;	
	if ( lpWindowPos->cx == cx && lpWindowPos->cy == cy )
		return( YES );

	lpWindowPos->cx = cx;
	lpWindowPos->cy = cy;
	return( NO );
}

/***********************************************************************/
LOCAL UINT Toon_OnGetDlgCode(HWND hWindow, LPMSG lpmsg)
/***********************************************************************/
{
	return( DLGC_WANTALLKEYS );
}

/***********************************************************************/
LOCAL void Toon_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{        
	PTOON pToon = GetToon(hWindow);
	if (pToon)
		pToon->OnLButtonDown(fDoubleClick, x, y, keyFlags);
}

/***********************************************************************/
LOCAL void Toon_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	PTOON pToon = GetToon(hWindow);
	if (pToon)
		pToon->OnLButtonUp(x, y, keyFlags);
}

/***********************************************************************/
LOCAL void Toon_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
}

/***********************************************************************/
LOCAL void Toon_OnRButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{        
	PTOON pToon = GetToon(hWindow);
	if (pToon)
		pToon->OnRButtonDown(fDoubleClick, x, y, keyFlags);
}

/***********************************************************************/
LOCAL void Toon_OnRButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	PTOON pToon = GetToon(hWindow);
	if (pToon)
		pToon->OnRButtonUp(x, y, keyFlags);
}

/***********************************************************************/
LOCAL void Toon_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	PTOON pToon = GetToon(hWindow);
	if (pToon)
		pToon->OnMouseMove(x, y, keyFlags);
}

/***********************************************************************/
LOCAL void Toon_OnPaletteChanged(HWND hWindow, HWND hwndPaletteChange)
/***********************************************************************/
{
	InvalidateRect(hWindow, NULL, FALSE);
}

//***************************************************************************
UINT Toon_OnMCINotify( HWND hWindow, UINT codeNotify, HMCI hDevice )
//***************************************************************************
{
	PTOON pToon = GetToon(hWindow);
	if (pToon)
		return(pToon->OnMCINotify(codeNotify, hDevice));
	else
		return(FALSE);
}

//***************************************************************************
LOCAL BOOL Toon_OnSetCursor(HWND hWindow, HWND hWndCursor, UINT codeHitTest, UINT msg)
//***************************************************************************
{
	PTOON pToon = GetToon(hWindow);
	if (pToon)
	{
		if (pToon->OnSetCursor(hWndCursor, codeHitTest, msg))
			return(TRUE);
	}
	return FORWARD_WM_SETCURSOR(hWindow, hWndCursor, codeHitTest, msg, DefWindowProc);
}

//***************************************************************************
BOOL Toon_OnToonDraw(HWND hWindow, HTOON hToon, LONG lFrame)
//***************************************************************************
{
	PTOON pToon = GetToon(hWindow);
	if (pToon)
		return(pToon->OnToonDraw(hToon, lFrame));
	else
		return(FALSE);
	return(TRUE);
}

//***************************************************************************
LOCAL void Toon_OnTimer(HWND hWindow, UINT id)
//***************************************************************************
{
	PTOON pToon = GetToon(hWindow);
	if (pToon)
		pToon->OnTimer(id);
	else // who's timer is this?w
		KillTimer(hWindow, id);
}

/***********************************************************************/
LONG WINPROC EXPORT ToonControl(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{                
	switch ( message )
	{
		HANDLE_MSG(hWindow, WM_CREATE, Toon_OnCreate);
		HANDLE_MSG(hWindow, WM_DESTROY, Toon_OnDestroy);
		HANDLE_MSG(hWindow, WM_ERASEBKGND, Toon_OnEraseBkgnd);
		HANDLE_MSG(hWindow, WM_PAINT, Toon_OnPaint);
		HANDLE_MSG(hWindow, WM_SIZE, Toon_OnSize); 
		HANDLE_MSG(hWindow, WM_WINDOWPOSCHANGING, Toon_OnWindowPosChanging); 
		HANDLE_MSG(hWindow, WM_GETDLGCODE, Toon_OnGetDlgCode);
  		HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Toon_OnLButtonDown);
 		HANDLE_MSG(hWindow, WM_LBUTTONUP, Toon_OnLButtonUp);
  		HANDLE_MSG(hWindow, WM_RBUTTONDOWN, Toon_OnRButtonDown);
 		HANDLE_MSG(hWindow, WM_RBUTTONUP, Toon_OnRButtonUp);
   		HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Toon_OnLButtonDblClk);
		HANDLE_MSG(hWindow, WM_MOUSEMOVE, Toon_OnMouseMove);
		HANDLE_MSG(hWindow, MM_MCINOTIFY, Toon_OnMCINotify);
		HANDLE_MSG(hWindow, WM_SETCURSOR, Toon_OnSetCursor);
		HANDLE_MSG(hWindow, WM_TOONDRAW, Toon_OnToonDraw);
		HANDLE_MSG(hWindow, WM_TIMER, Toon_OnTimer);

		default:
		return ToonDefWindowProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
LOCAL BOOL ToonHolder_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{                          
	STRING szDialogCaption, szHolderCaption;

	// parent window has the background dib
	GetWindowText(hWindow, szHolderCaption, sizeof(szHolderCaption));
	GetWindowText(GetParent(hWindow), szDialogCaption, sizeof(szDialogCaption));
	SetWindowText(hWindow, szDialogCaption);

	HWND hToonWnd = FindClassDescendent(GetApp()->GetMainWnd(), "toon");
	if (hToonWnd)
	{
  		// set to be child of toon holder window until destroy
		SetParent(hToonWnd, hWindow);
		SetWindowText(hToonWnd, szHolderCaption);
  		// show the toon window and make sure we get no redraw
  		SetWindowPos(hToonWnd, NULL, 0, 0, 0, 0,
  			SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOREDRAW|
  			SWP_NOZORDER);
		Toon_Init(hToonWnd);
	}

	return(TRUE);
}

/***********************************************************************/
LOCAL void ToonHolder_OnDestroy(HWND hWindow)
/***********************************************************************/
{           
	// get the toon window
	HWND hToonWnd = GetWindow(hWindow, GW_CHILD);
	if (hToonWnd)
	{
		// get the main app window
		HWND hMainWnd = GetApp()->GetMainWnd();
		if (hMainWnd)
		{
			PTOON pToon = GetToon(hToonWnd);
			if (pToon)
				pToon->FreeUp();
			if (!GetApp()->IsAppClosing())
			{
				// hide the toon window and make sure we get no redraw
				SetWindowPos(hToonWnd, NULL, 0, 0, 0, 0,
					SWP_HIDEWINDOW|SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE|SWP_NOREDRAW|
					SWP_NOZORDER);
				// set to be child of main window until next toon holder comes along
				SetParent(hToonWnd, hMainWnd);
			}
		}
	}
}

/***********************************************************************/
BOOL ToonHolder_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
	return(TRUE);
}

/***********************************************************************/
void ToonHolder_OnPaint(HWND hWindow)
/***********************************************************************/
{
	PAINTSTRUCT ps;

	if (BeginPaint(hWindow, &ps))
		EndPaint(hWindow, &ps);
}

/***********************************************************************/
LOCAL BOOL ToonHolder_OnWindowPosChanging( HWND hWindow, LPWINDOWPOS lpWindowPos )
/***********************************************************************/
{
	int cx, cy;

	if ( lpWindowPos->flags & SWP_NOSIZE )
		return( YES );

	//HWND hToonWnd = GetWindow(hWindow, GW_CHILD);
	//if (!hToonWnd)
	//	return(YES);
	//PTOON pToon = GetToon(hToonWnd);
	//if (!pToon)
	//	return(YES);

	cx = 640;/*pToon->GetWidth()*/
	cy = 480;/*pToon->GetHeight()*/	
	if ( lpWindowPos->cx == cx && lpWindowPos->cy == cy )
		return( YES );

	lpWindowPos->cx = cx;
	lpWindowPos->cy = cy;
	return( NO );
}

/***********************************************************************/
LONG WINPROC EXPORT ToonHolderControl(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{                
	switch ( message )
	{
		HANDLE_MSG(hWindow, WM_CREATE, ToonHolder_OnCreate);
		HANDLE_MSG(hWindow, WM_DESTROY, ToonHolder_OnDestroy);
		HANDLE_MSG(hWindow, WM_ERASEBKGND, ToonHolder_OnEraseBkgnd);
		HANDLE_MSG(hWindow, WM_PAINT, ToonHolder_OnPaint);
		HANDLE_MSG(hWindow, WM_WINDOWPOSCHANGING, ToonHolder_OnWindowPosChanging); 

		default:
		return DefWindowProc( hWindow, message, wParam, lParam );
	}
}

