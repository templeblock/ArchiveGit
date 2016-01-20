#include <windows.h>
#include "proto.h"
#include "puzzctrl.h"
#include "scene.h"
#include "sound.h"
#include "puzzleid.h"

/***********************************************************************/
LOCAL BOOL ComputeDIBControlSize( HWND hControl, LPINT lpDX, LPINT lpDY )
/***********************************************************************/
{
	int iWidth, iHeight;

	// Make sure that the DIB is uncompressed
	PPUZZLE pPuzzle = GetPuzzle(hControl);
	if (pPuzzle && pPuzzle->GetSize(&iWidth, &iHeight))
	{
		DWORD dwStyle = GetWindowLong(hControl, GWL_STYLE);
		if (dwStyle & WS_BORDER)
		{
			iWidth += 2;
			iHeight += 2;
		}
		if ( lpDX )
			*lpDX = iWidth;
		if ( lpDY )
			*lpDY = iHeight;
	}
	else
	{
		if ( lpDX )
			*lpDX = 1;
		if ( lpDY )
			*lpDY = 1;
	}
	return( YES );
}

/***********************************************************************/
BOOL Puzzle_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
	PPUZZLE pPuzzle = new CPuzzle(hWindow/*GetApp()->m_hPal*/);
	SetPuzzle(hWindow, pPuzzle);
	return(TRUE);
}

/***********************************************************************/
LOCAL void Puzzle_OnDestroy(HWND hWindow)
/***********************************************************************/
{
	PPUZZLE pPuzzle = GetPuzzle(hWindow);
	if (pPuzzle)
	{
		if ( pPuzzle->GetTracking() )
		{
			ReleaseCapture();
			pPuzzle->SetTracking(NO);
		}
		delete pPuzzle;
		SetPuzzle(hWindow, NULL);
	}
}

/***********************************************************************/
LOCAL void Puzzle_OnSize(HWND hWindow, UINT state, int cx, int cy)
/***********************************************************************/
{
	PPUZZLE pPuzzle = GetPuzzle(hWindow);
	if (pPuzzle)
		pPuzzle->Resize();
	FORWARD_WM_SIZE(hWindow, state, cx, cy, DefWindowProc);
}

/***********************************************************************/
LOCAL UINT Puzzle_OnGetDlgCode(HWND hWindow, LPMSG lpmsg)
/***********************************************************************/
{
	return( DLGC_WANTALLKEYS );
}

/***********************************************************************/
LOCAL BOOL Puzzle_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
	return(TRUE);
}

/***********************************************************************/
LOCAL UINT Puzzle_OnNCHitTest(HWND hWindow, int x, int y)
/***********************************************************************/
{
	UINT uReturn = FORWARD_WM_NCHITTEST(hWindow, x, y, DefWindowProc);
	if ( SHIFT && (uReturn == HTCLIENT) )
		uReturn = HTCAPTION;
	return( uReturn );
}

/***********************************************************************/
LOCAL void Puzzle_OnMove(HWND hWindow, int x, int y)
/***********************************************************************/
{
	InvalidateRect( hWindow, NULL, FALSE );
}

/***********************************************************************/
LOCAL void Puzzle_OnPaint(HWND hWindow)
/***********************************************************************/
{
	PAINTSTRUCT ps;
	HDC hDC;

	if (hDC = BeginPaint( hWindow, &ps ) )
	{
		PPUZZLE pPuzzle = GetPuzzle(hWindow);
		if (pPuzzle)
			pPuzzle->OnDraw(hDC, &ps.rcPaint, !pPuzzle->IsSolved());
		EndPaint( hWindow, &ps );
	}
}

/***********************************************************************/
LOCAL void Puzzle_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	PPUZZLE pPuzzle = GetPuzzle(hWindow);
	if (!pPuzzle)
		return;
	pPuzzle->OnLButtonDown(hWindow, fDoubleClick, x, y, keyFlags);
}

/***********************************************************************/
LOCAL void Puzzle_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	PPUZZLE pPuzzle = GetPuzzle(hWindow);
	if (!pPuzzle)
		return;
	pPuzzle->OnLButtonUp(hWindow, x, y, keyFlags);
}

/***********************************************************************/
LOCAL void Puzzle_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
}

/***********************************************************************/
LOCAL void Puzzle_OnRButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	RECT ClientRect, ScreenRect;
	GetWindowRect( hWindow, &ScreenRect );
	GetClientRect( hWindow, &ClientRect );
	MapWindowPoints( hWindow, GetParent(hWindow), (LPPOINT)&ClientRect, 2 );
	Print( "From UL Screen (%d,%d),\nFrom UL Parent (%d,%d),\nx = %d y = %d,\nSize (%d,%d)",
			ScreenRect.left, ScreenRect.top,
			ClientRect.left, ClientRect.top,
			x, y,
			ClientRect.right - ClientRect.left,
			ClientRect.bottom - ClientRect.top );
}

/***********************************************************************/
LOCAL void Puzzle_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	PPUZZLE pPuzzle = GetPuzzle(hWindow);
	if (!pPuzzle)
		return;
	pPuzzle->OnMouseMove(hWindow, x, y, keyFlags);
}

/***********************************************************************/
LOCAL void Puzzle_OnEnable(HWND hWindow, BOOL fEnable)
/***********************************************************************/
{
	FORWARD_WM_ENABLE(hWindow, fEnable, DefWindowProc);
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow(hWindow);
}

/***********************************************************************/
LOCAL void Puzzle_OnPaletteChanged(HWND hWindow, HWND hwndPaletteChange)
/***********************************************************************/
{
	InvalidateRect(hWindow, NULL, FALSE);
}

/***********************************************************************/
LOCAL void Puzzle_OnKey(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
}

/***********************************************************************/
LONG WINPROC EXPORT PuzzleControl(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch ( message )
	{
		HANDLE_MSG(hWindow, WM_CREATE, Puzzle_OnCreate);
		HANDLE_MSG(hWindow, WM_DESTROY, Puzzle_OnDestroy);
		HANDLE_MSG(hWindow, WM_SIZE, Puzzle_OnSize);
		HANDLE_MSG(hWindow, WM_GETDLGCODE, Puzzle_OnGetDlgCode);
		HANDLE_MSG(hWindow, WM_ERASEBKGND, Puzzle_OnEraseBkgnd);
		HANDLE_MSG(hWindow, WM_NCHITTEST, Puzzle_OnNCHitTest);
		HANDLE_MSG(hWindow, WM_MOVE, Puzzle_OnMove);
		HANDLE_MSG(hWindow, WM_PAINT, Puzzle_OnPaint);
  		HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Puzzle_OnLButtonDown);
 		HANDLE_MSG(hWindow, WM_LBUTTONUP, Puzzle_OnLButtonUp);
   		HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Puzzle_OnLButtonDblClk);
  		HANDLE_MSG(hWindow, WM_RBUTTONUP, Puzzle_OnRButtonUp);
		HANDLE_MSG(hWindow, WM_MOUSEMOVE, Puzzle_OnMouseMove);
		HANDLE_MSG(hWindow, WM_ENABLE, Puzzle_OnEnable);
		HANDLE_MSG(hWindow, WM_PALETTECHANGED, Puzzle_OnPaletteChanged);
		HANDLE_MSG(hWindow, WM_KEYDOWN, Puzzle_OnKey);

		default:
		return DefWindowProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
PPUZZLE GetPuzzle(HWND hWindow)
/***********************************************************************/
{
	return((PPUZZLE)GetWindowLong(hWindow, GWL_PUZZLE));
}

/***********************************************************************/
void SetPuzzle(HWND hWindow, PPUZZLE pPuzzle)
/***********************************************************************/
{
	SetWindowLong(hWindow, GWL_PUZZLE, (long)pPuzzle);
}

/***********************************************************************/
CPuzzle::CPuzzle(HWND hWindow, HPALETTE hPal)
/***********************************************************************/
{
	STRING szString; 
	HWND hItem;

	m_hWnd = hWindow;
	m_pDib = NULL;
	m_iRows = 4;
	m_iCols = 4;
	m_iSelect = -1;
	m_szDibFile[0] = '\0';
	m_bTrack = FALSE;
	m_bInRect = FALSE;
	m_hPal = hPal;
	m_idResource = GetDlgCtrlID(m_hWnd) + 1;
	m_bHintMode = FALSE;

	GetWindowText(hWindow, szString, sizeof(szString));
	m_lpSwitches = ExtractSwitches(szString);

	#ifdef UNUSED
	GetModuleFileName(GetWindowInstance(hWindow), szFileName, sizeof(szFileName));
	StripFile(szFileName);

 	if (GetStringParm(szString, 0, ',', szFile))
 	{
 		lstrcpy(m_szDibFile, szFileName);
 		lstrcat(m_szDibFile, szFile);
 	}
	if (lstrlen(m_szDibFile))
	#endif

	HWND hParent = GetParent(m_hWnd);
	hItem = GetDlgItem(hParent, m_idResource);
	if (hItem)
	{
		GetWindowText(hItem, m_szDibFile, sizeof(m_szDibFile));
	    m_pDib = CDib::LoadDibFromFile( m_szDibFile, m_hPal, FALSE, TRUE );
	}
	else
	    m_pDib = CDib::LoadDibFromResource( GetWindowInstance(m_hWnd), MAKEINTRESOURCE(m_idResource), m_hPal, FALSE, TRUE );
	if (m_pDib)
		Setup();
}

/***********************************************************************/
CPuzzle::~CPuzzle()
/***********************************************************************/
{
	if (m_pDib)
		delete m_pDib;
	if (m_lpSwitches)
		FreeUp(m_lpSwitches);
}

/***********************************************************************/
BOOL CPuzzle::Init(int iRows, int iCols)
/***********************************************************************/
{
	if (iRows > MAX_ROWS)
		iRows = MAX_ROWS;
	if (iRows <= 1)
		iRows = 2;
	if (iCols > MAX_COLS)
		iCols = MAX_COLS;
	if (iCols <= 1)
		iCols = 2;

	if (iRows != m_iRows || iCols != m_iCols)
	{
		LPSCENE lpScene = CScene::GetScene(GetParent(m_hWnd));
		if (lpScene)
		{
			LPOFFSCREEN lpOffScreen = lpScene->GetOffScreen();
			if (lpOffScreen)
				lpOffScreen->CopyBits();
		}
		m_iRows = iRows;
		m_iCols = iCols;
		Setup();
	}
	return(TRUE);
}

/***********************************************************************/
int CPuzzle::NextPuzzle()
/***********************************************************************/
{
	HWND hParent;
	FNAME szFileName;

	hParent = GetParent(m_hWnd);
	PlaySound(IDW_PUZZLENEXT);
	++m_idResource;
	if (m_pDib)
		delete m_pDib;
	if (GetDlgItem(hParent, GetDlgCtrlID(m_hWnd) + 1))
	{
		if (!GetDlgItem(hParent, m_idResource))
			m_idResource = GetDlgCtrlID(m_hWnd) + 1;
		GetWindowText(GetDlgItem(hParent, m_idResource), szFileName, sizeof(szFileName));
    	m_pDib = CDib::LoadDibFromFile( szFileName, m_hPal, FALSE, TRUE );
	}
	else
	{
    	m_pDib = CDib::LoadDibFromResource( GetWindowInstance(m_hWnd), MAKEINTRESOURCE(m_idResource), m_hPal, FALSE, TRUE );
		if (!m_pDib)
		{
			m_idResource = GetDlgCtrlID(m_hWnd) + 1;
    		m_pDib = CDib::LoadDibFromResource( GetWindowInstance(m_hWnd), MAKEINTRESOURCE(m_idResource), m_hPal, FALSE, TRUE );
		}
	}
	if (m_pDib)
	{
	 	Setup();
		return(m_idResource);
	}
	else
		return(0);
}

/***********************************************************************/
void CPuzzle::Setup()
/***********************************************************************/
{
	int i, iPieces, iPiecesInPlace;

	// get the total number of pieces
	iPieces = m_iCols * m_iRows;

	// clear the map
	for (i = 0; i < iPieces; ++i)
		m_iMap[i] = -1;

	// create the random location map
	// upper-left is 0, lower-right is (iPieces-1)  
	iPiecesInPlace = 0;
	for (i = 0; i < iPieces; ++i)
	{
		// gotta fill every entry somehow
		while (TRUE)
		{
			// get random number to use
			int random = GetRandomNumber(iPieces);

			// see if we already used this random number
			for (int j = 0; j < i; ++j)
			{
				if (m_iMap[j] == random)
					break;
			}
			// if it hasn't been used then use it
			if (j == i)
			{
				// make the user move at least have the pieces
				if ( (i != random) || (iPiecesInPlace < (iPieces/2)) )
				{
					m_iMap[i] = random;
					if (i == random)
						++iPiecesInPlace;
					break;
				}
				//else
				//{
				//	OutputDebugString("Piece Location Rejected\n");
				//}
			}
		}
	}
	// window resizing is necessary, because we want all the tiles
	// to be exactly the same size, so we may not use some on
	// the right and bottom sides
	Resize();
	InvalidateRect(m_hWnd, NULL, TRUE);
}

/***********************************************************************/
void CPuzzle::OnDraw(HDC hDC, LPRECT lpRect, BOOL fGrid)
/***********************************************************************/
{
	int iPieces;
	RECT rOut, rClient, rPaint, rWindow;
	HDC hDCOut;
	POINT ptDst;
	PDIB pDibOut = NULL;

	GetClientRect(m_hWnd, &rClient);
	GetWindowRect(m_hWnd, &rWindow);
	if (!IntersectRect(&rPaint, &rClient, lpRect))
		return;
	if (!m_pDib)
		return;

	LPSCENE lpScene = CScene::GetScene(GetParent(m_hWnd));
	if (!lpScene)
		return;
	LPOFFSCREEN lpOffScreen = lpScene->GetOffScreen();
	if (lpOffScreen)
	{
		hDCOut = lpOffScreen->GetDC();
		rOut = rPaint;
		MapWindowPoints( m_hWnd, GetParent(m_hWnd), (LPPOINT)&rOut, 2 );
		MapWindowPoints( m_hWnd, GetParent(m_hWnd), (LPPOINT)&rClient, 2 );
		MapWindowPoints( NULL, GetParent(m_hWnd), (LPPOINT)&rWindow, 2 );
		pDibOut = lpOffScreen->GetWritableDIB();
	}
	else
		hDCOut = hDC;

	iPieces = m_iCols * m_iRows;
	for (int i = 0; i < iPieces; ++i)
		DrawPiece(i, hDCOut, pDibOut, (LPPOINT)&rClient.left, &rPaint, fGrid);

	if (m_iSelect >= 0)
	{
		if (m_bHintMode)
			DrawHint(m_iSelect, hDCOut, (LPPOINT)&rClient.left, &rPaint);
		DrawPiece(m_iSelect, hDCOut, pDibOut, (LPPOINT)&rClient.left, &rPaint, FALSE, &m_ptSelect);
	}

	// when we are all done we draw from the WinG DC to the output DC
	if (lpOffScreen)
	{
		ptDst.x = rPaint.left;
		ptDst.y = rPaint.top;
		lpOffScreen->DrawRect( hDC, &rOut, &ptDst );
	}
}

/***********************************************************************/
BOOL CPuzzle::GetSize(LPINT lpWidth, LPINT lpHeight)
/***********************************************************************/
{
	if (!m_pDib)
		return(FALSE);
	int iCol = m_pDib->GetWidth() / m_iCols;
	int iRow = m_pDib->GetHeight() / m_iRows;
	*lpWidth = iCol * m_iCols;
	*lpHeight = iRow * m_iRows;
	return(TRUE);
}

/***********************************************************************/
void CPuzzle::OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	if ( SHIFT )
		return;
	if ( m_bTrack )
		return;
	if (IsSolved())
		return;

	SetCapture( hWindow ); m_bTrack = TRUE;
	if ( GetFocus() != hWindow )
		SetFocus( hWindow );
	m_bInRect = YES; 

	if (!m_pDib)
		return;

	int iColWidth = m_pDib->GetWidth() / m_iCols;
	int iRowHeight = m_pDib->GetHeight() / m_iRows;
	int iCol = x / iColWidth;
	int iRow = y / iRowHeight;
	m_iSelect = (iRow * m_iCols) + iCol;

	m_ptSelect.x = iCol * iColWidth;
	m_ptSelect.y = iRow * iRowHeight;
	m_ptLast.x = x;
	m_ptLast.y = y;

	PlaySound(IDW_PUZZLEDOWN);
	if (m_bHintMode)
	{
		HDC hDC = GetDC(hWindow);
		DrawHint(m_iSelect, hDC, NULL, NULL);
		ReleaseDC(hWindow, hDC);
	}
}

/***********************************************************************/
void CPuzzle::OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	if ( !m_bTrack )
		return;
	ReleaseCapture(); m_bTrack = FALSE;
	
	if (!m_pDib)
		return;

	if (m_bInRect && m_iSelect >= 0)
	{
		int iColWidth = m_pDib->GetWidth() / m_iCols;
		int iRowHeight = m_pDib->GetHeight() / m_iRows;
		int iCol = x / iColWidth;
		int iRow = y / iRowHeight;
		int iNewSpot = (iRow * m_iCols) + iCol;
		int temp = m_iMap[m_iSelect];
		m_iMap[m_iSelect] = m_iMap[iNewSpot];
		m_iMap[iNewSpot] = temp;

	}
	m_iSelect = -1;
	if (IsSolved())
		PlaySound(IDW_PUZZLESOLVED);
	else
		PlaySound(IDW_PUZZLEUP);
	InvalidateRect(hWindow, NULL, FALSE);
}

/***********************************************************************/
void CPuzzle::OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	RECT ClientRect, rUpdate, rNew;
	POINT pt;

	if ( !m_bTrack )
		return;
	GetClientRect( hWindow, &ClientRect );
	pt.x = x;
	pt.y = y;
	if ( m_bInRect == !PtInRect( &ClientRect, pt ) )
	{
		m_bInRect = !m_bInRect;
	}
	if (!m_bInRect)
		return;
	if (!m_pDib)
		return;
	int dx = x - m_ptLast.x;
	int dy = y - m_ptLast.y;
	if (!dx && !dy)
		return;

	m_ptLast.x = x;
	m_ptLast.y = y;

	int iColWidth = m_pDib->GetWidth() / m_iCols;
	int iRowHeight = m_pDib->GetHeight() / m_iRows;

	SetRect(&rUpdate, m_ptSelect.x, m_ptSelect.y,
				m_ptSelect.x+iColWidth,
				m_ptSelect.y+iRowHeight);

	m_ptSelect.x += dx;
	m_ptSelect.y += dy;

	SetRect(&rNew, m_ptSelect.x, m_ptSelect.y,
				m_ptSelect.x+iColWidth,
				m_ptSelect.y+iRowHeight);

	UnionRect(&rUpdate, &rNew, &rUpdate);

	// undraw the selected piece
	HDC hDC = GetDC(hWindow);
	OnDraw(hDC, &rUpdate, TRUE);
	ReleaseDC(hWindow, hDC);
}

/***********************************************************************/
void CPuzzle::DrawPiece(int i,
						HDC hDC,
						PDIB pDibOut,
						LPPOINT ptDib,
						LPRECT lpPaint,
						BOOL fDrawGrid,
						LPPOINT ptPiece)
/***********************************************************************/
{
	int sx, sy, iSrcCol, iSrcRow, dx, dy, iDstCol, iDstRow, iPiece;
	int iColWidth, iRowHeight;
	RECT rDst, rSrc, rSect, rFrame, rClient;

	iColWidth = m_pDib->GetWidth() / m_iCols;
	iRowHeight = m_pDib->GetHeight() / m_iRows;

	iPiece = m_iMap[i];
	iSrcCol = iPiece % m_iCols;
	iSrcRow = iPiece / m_iCols;
	sx = iSrcCol * iColWidth;
	sy = iSrcRow * iRowHeight;
	iDstCol = i % m_iCols;
	iDstRow = i / m_iCols;
	if (ptPiece)
	{
		dx = ptPiece->x;
		dy = ptPiece->y;
	}
	else
	{
		dx = iDstCol * iColWidth;
		dy = iDstRow * iRowHeight;
	}

	SetRect(&rDst, dx, dy, dx+iColWidth, dy+iRowHeight);
	SetRect(&rSrc, sx, sy, sx+iColWidth, sy+iRowHeight);

	if (!IntersectRect(&rSect, &rDst, lpPaint))
		return;

	rFrame = rDst;
	rFrame.left -= 1;
	rFrame.top -= 1;
	GetClientRect(m_hWnd, &rClient);
	if (abs(rFrame.right-rClient.right) < 5)
		rFrame.right = rClient.right+1;
	if (abs(rFrame.bottom-rClient.bottom) < 5)
		rFrame.bottom = rClient.bottom+1;
   	OffsetRect(&rFrame, ptDib->x, ptDib->y);

	if (m_iSelect == i && (ptPiece == NULL))
	{
		OffsetRect(&rSect, ptDib->x, ptDib->y);
		FillRect(hDC, &rSect, (HBRUSH)GetStockObject(GRAY_BRUSH));
	}
	else
	{
		ClipToRect(&rSect, &rDst, &rSrc);
		if (pDibOut)
		{
			OffsetRect(&rDst, ptDib->x, ptDib->y);
			m_pDib->DibBlt(pDibOut,
   				rDst.left, rDst.top, rDst.right-rDst.left, rDst.bottom-rDst.top,
   				rSrc.left, rSrc.top, rSrc.right-rSrc.left, rSrc.bottom-rSrc.top,
				NO);
		}
		else
		{
			m_pDib->DCBlt(hDC,
   				rDst.left, rDst.top, rDst.right-rDst.left, rDst.bottom-rDst.top,
   				rSrc.left, rSrc.top, rSrc.right-rSrc.left, rSrc.bottom-rSrc.top);
		}
	}
	if (fDrawGrid)
		FrameRect(hDC, &rFrame, (HBRUSH)GetStockObject(BLACK_BRUSH));
}

/***********************************************************************/
void CPuzzle::DrawHint(int i,
						HDC hDC,
						LPPOINT ptDib,
						LPRECT lpPaint)
/***********************************************************************/
{
	int sx, sy, iSrcCol, iSrcRow;
	int iColWidth, iRowHeight, iPiece;
	RECT  rSrc, rSect, rClient;
	POINT ptRect[5];
	HPEN hPen, hOldPen;

	iColWidth = m_pDib->GetWidth() / m_iCols;
	iRowHeight = m_pDib->GetHeight() / m_iRows;

	iPiece = m_iMap[i];
	iSrcCol = iPiece % m_iCols;
	iSrcRow = iPiece / m_iCols;
	sx = iSrcCol * iColWidth;
	sy = iSrcRow * iRowHeight;
	SetRect(&rSrc, sx, sy, sx+iColWidth, sy+iRowHeight);

	if (lpPaint)
	{
		if (!IntersectRect(&rSect, &rSrc, lpPaint))
			return;
	}
	
	if (ptDib)
		OffsetRect(&rSrc, ptDib->x, ptDib->y);
	rSrc.left -= 1;
	rSrc.top -= 1;
	GetClientRect(m_hWnd, &rClient);
	if (abs(rSrc.right-rClient.right) < 5)
		rSrc.right = rClient.right+1;
	if (abs(rSrc.bottom-rClient.bottom) < 5)
		rSrc.bottom = rClient.bottom+1;
	InflateRect(&rSrc, -2, -2);

	ptRect[0].x = rSrc.left;
	ptRect[0].y = rSrc.top;
	ptRect[1].x = rSrc.right;
	ptRect[1].y = rSrc.top;
	ptRect[2].x = rSrc.right;
	ptRect[2].y = rSrc.bottom;
	ptRect[3].x = rSrc.left;
	ptRect[3].y = rSrc.bottom;
	ptRect[4].x = rSrc.left;
	ptRect[4].y = rSrc.top;

	hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
	hOldPen = (HPEN)SelectObject(hDC, hPen);
	Polyline(hDC, ptRect, 5);
	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);
}

/***********************************************************************/
void CPuzzle::ClipToRect(LPRECT lpSect, LPRECT lpDst, LPRECT lpSrc)
/***********************************************************************/
{
	int iExtra;

	iExtra = lpSect->left - lpDst->left;
	lpDst->left += iExtra;
	lpSrc->left += iExtra;
	iExtra = lpSect->top - lpDst->top;
	lpDst->top += iExtra;
	lpSrc->top += iExtra;
	iExtra = lpDst->right - lpSect->right;
	lpDst->right -= iExtra;
	lpSrc->right -= iExtra;
	iExtra = lpDst->bottom - lpSect->bottom;
	lpDst->bottom -= iExtra;
	lpSrc->bottom -= iExtra;
}

/***********************************************************************/
BOOL CPuzzle::IsSolved()
/***********************************************************************/
{
	int iPieces = m_iCols * m_iRows;
	for (int i = 0; i < iPieces; ++i)
	{
		if (m_iMap[i] != i)
			break;
	}
	return (i == iPieces);
}

/***********************************************************************/
void CPuzzle::Resize()
/***********************************************************************/
{
	int dx, dy;
	if ( ComputeDIBControlSize( m_hWnd, &dx, &dy ) )
	{
		ResizeControl( m_hWnd, dx, dy );
		if (m_lpSwitches && (GetSwitchValue('c', m_lpSwitches) != 0))
			CenterWindow(m_hWnd);
	}
}

/***********************************************************************/
void CPuzzle::PlaySound(UINT idResource)
/***********************************************************************/
{
	FNAME szFileName;

	HWND hParent = GetParent(m_hWnd);
	HWND hItem = GetDlgItem(hParent, idResource);
	LPSCENE lpScene = CScene::GetScene(hParent);
	if (hItem)
	{
		GetWindowText(hItem, szFileName, sizeof(szFileName));
		if (lpScene->GetSound())
			lpScene->GetSound()->StartFile( szFileName, NO/*bLoop*/, -1/*iChannel*/ );
	}
	else
	{
		if (lpScene->GetSound())
			lpScene->GetSound()->StartResourceID( IDW_PUZZLENEXT, NO/*bLoop*/, -1/*iChannel*/, GetWindowInstance(m_hWnd) );
	}
}