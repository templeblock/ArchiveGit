#include "windows.h"
#include "lgbook.h"
#include "control.h"
#include "sound.h"
#include "mmsystem.h"
#include "wing.h"

#define SHIFT ( GetAsyncKeyState( VK_SHIFT )<0 )
#define CANCEL ( GetAsyncKeyState( VK_ESCAPE )<0 )

static BOOL bTrack, bInRect;

/***********************************************************************/
LOCAL void Story_OnDraw(HWND hWindow, HDC hDC, LPRECT lpRect, BOOL fHighlight)
/***********************************************************************/
{
	BOOL bHasFocus, bSelected, bDown;
	DWORD dwStyle;
	RECT rSrcArea, rDstArea;
	POINT ptDst;
	PDIB pdibSrc, pdibDst, pDib;
	LPOFFSCREEN lpOffScreen;
	PTOON pToon;
	LPSCENE lpScene;
	PSTORY pStory;
	RGBQUAD rgbQuad[256];
	BITMAPINFOHEADER bmi;
	LPTR lp;
	HDC hWinGDC;
	HPALETTE hWinGPal = NULL, hDisplayPal = NULL, hOldPal;

	if (IsRectEmpty(lpRect))
		return;
	pStory = GetStory(hWindow);
	if (!pStory)
		return;
	if (!pStory->m_pDib)
		return;
	lpScene = CScene::GetScene(GetParent(hWindow));
	if (!lpScene)
		return;

	rSrcArea = *lpRect;
	rDstArea = *lpRect;
	bSelected = GetWindowWord(hWindow, GWW_STATE );
	dwStyle = GetWindowLong( hWindow, GWL_STYLE );
	bHasFocus = ( GetFocus() == hWindow );
	bDown = ( bSelected || (bTrack && bInRect && bHasFocus) );

	lpOffScreen = lpScene->GetOffScreen();
	if (lpOffScreen)
	{
		MapWindowPoints( hWindow, GetParent(hWindow), (LPPOINT)&rDstArea, 2 );
 		if ( !(pdibSrc = lpOffScreen->GetReadOnlyDIB()) )
			return;
		if ( !(pdibDst = lpOffScreen->GetWritableDIB()) )
			return;
		hWinGDC = lpOffScreen->GetDC();
		hWinGPal = hDisplayPal = GetApp()->m_hPal;
	}
	else
	{
		// fix
		HWND hToon = FindClassDescendent(GetParent(hWindow), "toon");
		if (!hToon)
			return;
		pToon = GetToon(hToon);
		if (!pToon)
			return;
		MapWindowPoints( hWindow, pToon->GetWindow(), (LPPOINT)&rDstArea, 2 );
		pdibSrc = pToon->GetStageDib();
		lp = ToonGetDIBPointer(pToon->GetToonHandle(), &bmi);
		ToonGetColors(pToon->GetToonHandle(), 0, 256, rgbQuad);
		bmi.biClrUsed = 256;
		pdibDst = new CDib(&bmi, rgbQuad, lp);
		if (!pdibDst)
			return;
		hWinGDC = ToonDC(pToon->GetToonHandle());
		// to make sure we don't change the system palette
		hDisplayPal = CopySystemPalette();
		// to match WinG dib
		hWinGPal = CreateCustomPalette(rgbQuad, 256);
	}

	if (!pStory->m_fMappedToPalette && ((dwStyle & BS_MASK) == 0))
	{
		pStory->m_pDib->MapToPalette(hWinGPal);
		pStory->m_fMappedToPalette = TRUE;
	}


	// Copy source dib so we can twiddle its bits
	pDib = new CDib();
	if (!pDib)
	{
		if (!lpOffScreen && hDisplayPal)
			DeleteObject(hDisplayPal);
		if (!lpOffScreen && hWinGPal)
			DeleteObject(hWinGPal);
		return;
	}
	if (!pDib->Create(pStory->m_pDib->GetBitCount(),
						lpRect->right-lpRect->left,
						lpRect->bottom-lpRect->top))
	{
		delete pDib;
		if (!lpOffScreen && hDisplayPal)
			DeleteObject(hDisplayPal);
		if (!lpOffScreen && hWinGPal)
			DeleteObject(hWinGPal);
		return;
	}
	pDib->CopyColorTable(pStory->m_pDib);
	// draw in source bitmap
	pStory->m_pDib->DibBlt( pDib,
					0, 0,
				 	rSrcArea.right - rSrcArea.left, rSrcArea.bottom - rSrcArea.top,
				 	rSrcArea.left, rSrcArea.top,
				 	rSrcArea.right - rSrcArea.left, rSrcArea.bottom - rSrcArea.top,
				 	NO /*bTransparent*/ );
	
	if ( pdibSrc->GetCompression() == BI_RLE8 ||
		 pdibDst->GetCompression() == BI_RLE8)
	{ // compressed DIBs must use GDI copying (lose transparency ability)
		pdibSrc->DCBlt( hWinGDC,
						rDstArea.left, rDstArea.top,
					 	rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
						rDstArea.left, rDstArea.top,
					 	rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top );
	}
	else
	{
		pdibSrc->DibBlt( pdibDst,
						rDstArea.left, rDstArea.top,
					 	rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
					 	rDstArea.left, rDstArea.top,
					 	rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
					 	NO /*bTransparent*/ );
	}

	if ( pDib->GetCompression() == BI_RLE8 ||
		 pdibDst->GetCompression() == BI_RLE8 )
	{ // compressed DIBs must use GDI copying (lose transparency ability)
		pDib->DCBlt( hWinGDC,
					rDstArea.left, rDstArea.top,
					rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
					rSrcArea.left, rSrcArea.top,
					rSrcArea.right - rSrcArea.left, rSrcArea.bottom - rSrcArea.top );
	}
	else
	{
		RGBTRIPLE rgb;
		LPRGBTRIPLE lpRGB = NULL;
		if (dwStyle & BS_MASK)
		{
			STRING szColor;
			GetWindowText(hWindow, szColor, sizeof(szColor));
			AsciiRGB( szColor, &rgb );
			if (fHighlight)
			{
				// this relies on the fact that AsciiRGB replaces commas
				// with NULL terminators
				LPTSTR lp = szColor + lstrlen(szColor) + 1; // passed red
				lp += lstrlen(lp) + 1; // passed green
				lp += lstrlen(lp) + 1; // passed blue to higlight color
				AsciiRGB(lp, &rgb);
				rgb.rgbtRed = rgb.rgbtGreen = 0; rgb.rgbtBlue = 255;
			}
			else
				rgb.rgbtRed = rgb.rgbtGreen = rgb.rgbtBlue = 0;
			lpRGB = &rgb;
		}
		else
		{
			BYTE bTrans = *pStory->m_pDib->GetXY(0, 0);
			LPTR lpColor = pStory->m_pDib->GetXY(1, 0);
			LPTR lpHighlight = pStory->m_pDib->GetXY(2, 0); 
			BYTE bColor = *lpColor;
			BYTE bHighlight = *lpHighlight;
			// if highlight color is the transparent color then
			// we are hiding and showing highlighted area
			// if not, then we are changing the color of highlighted area
			if (bHighlight == bTrans)
			{
				// we need to strip off highlight info if we are not
				// highlighted
				if (!fHighlight && (bColor != bTrans))
				{
					HPTR hp = pDib->GetPtr();
					DWORD dwSize = pDib->GetSizeImage();
					while (dwSize)
					{
						if (*hp != bColor)
							*hp = bTrans;
						++hp;
						--dwSize;
					}
				}
			}
			else
			{
				// we need to change the color if we are highlighted
				if (fHighlight && (bColor != bHighlight) && (bColor != bTrans))
				{
					HPTR hp = pDib->GetPtr();
					DWORD dwSize = pDib->GetSizeImage();
					while (dwSize)
					{
						if (*hp == bColor)
							*hp = bHighlight;
						++hp;
						--dwSize;
					}
				}
			}
		}
		pDib->DibBlt( pdibDst,
					rDstArea.left, rDstArea.top,
					rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
					0, 0,
					rSrcArea.right - rSrcArea.left, rSrcArea.bottom - rSrcArea.top,
					YES/*bTransparent*/, lpRGB, NULL, hWinGPal );
	}
	delete pDib;

	ptDst.x = rSrcArea.left;
	ptDst.y = rSrcArea.top;
	if (lpOffScreen)
	{
		lpOffScreen->DrawRect( hDC, &rDstArea, &ptDst );
	}
	else
	{
		//if (hDisplayPal)
		//{
		//	hOldPal = SelectPalette(hDC, hDisplayPal, FALSE);
		//	RealizePalette(hDC);
		//}
		//pdibDst->DCBlt( hDC,
		//	ptDst.x, ptDst.y,
		//	rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
		//	rDstArea.left, rDstArea.top,
		//	rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top );
		WinGStretchBlt( hDC,
			ptDst.x, ptDst.y,
			rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
			hWinGDC,
			rDstArea.left, rDstArea.top,
			rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top );
		//if (hDisplayPal)
	    //{
		//	SelectPalette(hDC, hOldPal, TRUE);
	   	//	DeleteObject(hDisplayPal);
		//}
		if (hWinGPal)
			DeleteObject(hWinGPal);
	}
}

#ifdef UNUSED
/***********************************************************************/
LOCAL void Story_OnDraw(HWND hWindow, HDC hDC, LPRECT lpRect, BOOL fHighlight)
/***********************************************************************/
{
	BOOL bHasFocus, bSelected, bDown;
	DWORD dwStyle;
	RECT rSrcArea, rDstArea;
	PTOON pToon = NULL;
	PDIB pdibSrc, pdibDst;
	LPOFFSCREEN lpOffScreen = NULL;

	if (IsRectEmpty(lpRect))
		return;
	PSTORY pStory = GetStory(hWindow);
	if (!pStory)
		return;
	if (!pStory->m_pDib)
		return;
	// Draw the Bitmap
	rSrcArea = *lpRect;
	rDstArea = *lpRect;

	LPSCENE lpScene = CScene::GetScene(GetParent(hWindow));
	if (lpScene)
		lpOffScreen = lpScene->GetOffScreen();
	if (lpOffScreen)
	{
	 	if ( !(pdibSrc = lpOffScreen->GetReadOnlyDIB()) )
			return;
		MapWindowPoints( hWindow, GetParent(hWindow), (LPPOINT)&rDstArea, 2 );
	}
	else
	{
		// see if there is a toon control
		HWND hToon = FindClassDescendent(GetParent(hWindow), "toon");
		if (!hToon)
			return;
		pToon = GetToon(hToon);
		if (!pToon)
			return;
		pdibSrc = pToon->GetStageDib();
		MapWindowPoints( hWindow, GetParent(hToon), (LPPOINT)&rDstArea, 2 );
 	}

	// create a dib to draw into
	pdibDst = new CDib();
	if (!pdibDst)
		return;
	if (!pdibDst->Create(pdibSrc->GetBitCount(),
						lpRect->right-lpRect->left,
						lpRect->bottom-lpRect->top))
	{
		delete pdibDst;
		return;
	}
	pdibDst->CopyColorTable(pdibSrc);
	
	// copy our color table into the dest dib
	dwStyle = GetWindowLong( hWindow, GWL_STYLE );
	bHasFocus = ( GetFocus() == hWindow );
	bDown = ( bSelected || (bTrack && bInRect && bHasFocus) );

	// draw in background
	pdibSrc->DibBlt( pdibDst,
					0, 0,
				 	rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
				 	rDstArea.left, rDstArea.top,
				 	rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
				 	NO /*bTransparent*/ );

	// now draw in foreground
	RGBTRIPLE rgb;
	LPRGBTRIPLE lpRGB = NULL;
	if (dwStyle & BS_MASK)
	{
		STRING szColor;
		GetWindowText(hWindow, szColor, sizeof(szColor));
		AsciiRGB( szColor, &rgb );
		if (fHighlight)
		{
			// this relies on the fact that AsciiRGB replaces commas
			// with NULL terminators
			LPTSTR lp = szColor + lstrlen(szColor) + 1; // passed red
			lp += lstrlen(lp) + 1; // passed green
			lp += lstrlen(lp) + 1; // passed blue to higlight color
			AsciiRGB(lp, &rgb);
			rgb.rgbtRed = rgb.rgbtGreen = 0; rgb.rgbtBlue = 255;
		}
		else
			rgb.rgbtRed = rgb.rgbtGreen = rgb.rgbtBlue = 0;
		lpRGB = &rgb;
	}
	pStory->m_pDib->DibBlt( pdibDst,
				0, 0,
				rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
				rSrcArea.left, rSrcArea.top,
				rSrcArea.right - rSrcArea.left, rSrcArea.bottom - rSrcArea.top,
				YES /*bTransparent*/, lpRGB, App.m_lpLut );

	pdibDst->DCBlt( hDC,
				rSrcArea.left, rSrcArea.top,
				rSrcArea.right - rSrcArea.left, rSrcArea.bottom - rSrcArea.top,
			 	0, 0,
				rSrcArea.right - rSrcArea.left, rSrcArea.bottom - rSrcArea.top);

	delete pdibDst;
}
#endif

#ifdef UNUSED
/***********************************************************************/
LOCAL void Story_OnDraw(HWND hWindow, HDC hDC, LPRECT lpRect, BOOL fHighlight)
/***********************************************************************/
{
	BOOL bHasFocus, bSelected, bDown;
	DWORD dwStyle;
	RECT rSrcArea, rDstArea;
	PTOON pToon = NULL;
	PDIB pdibSrc, pdibDst;
	LPOFFSCREEN lpOffScreen = NULL;

	if (IsRectEmpty(lpRect))
		return;
	PSTORY pStory = GetStory(hWindow);
	if (!pStory)
		return;
	if (!pStory->m_pDib)
		return;
	// Draw the Bitmap
	rSrcArea = *lpRect;
	rDstArea = *lpRect;

	LPSCENE lpScene = CScene::GetScene(GetParent(hWindow));
	if (lpScene)
		lpOffScreen = lpScene->GetOffScreen();
	if (lpOffScreen)
	{
	 	if ( !(pdibSrc = lpOffScreen->GetReadOnlyDIB()) )
			return;
		MapWindowPoints( hWindow, GetParent(hWindow), (LPPOINT)&rDstArea, 2 );
	}
	else
	{
		// see if there is a toon control
		HWND hToon = FindClassDescendent(GetParent(hWindow), "toon");
		if (!hToon)
			return;
		pToon = GetToon(hToon);
		if (!pToon)
			return;
		pdibSrc = pToon->GetStageDib();
		MapWindowPoints( hWindow, GetParent(hToon), (LPPOINT)&rDstArea, 2 );
 	}

	// create a dib to draw into
	pdibDst = new CDib();
	if (!pdibDst)
		return;
	if (!pdibDst->Create(pdibSrc->GetBitCount(),
						lpRect->right-lpRect->left,
						lpRect->bottom-lpRect->top))
	{
		delete pdibDst;
		return;
	}
	pdibDst->CopyColorTable(pdibSrc);
	
	// copy our color table into the dest dib
	dwStyle = GetWindowLong( hWindow, GWL_STYLE );
	bHasFocus = ( GetFocus() == hWindow );
	bDown = ( bSelected || (bTrack && bInRect && bHasFocus) );

	// draw in background
	pdibSrc->DibBlt( pdibDst,
					0, 0,
				 	rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
				 	rDstArea.left, rDstArea.top,
				 	rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
				 	NO /*bTransparent*/ );

	// now draw in foreground
	RGBTRIPLE rgb;
	LPRGBTRIPLE lpRGB = NULL;
	if (dwStyle & BS_MASK)
	{
		STRING szColor;
		GetWindowText(hWindow, szColor, sizeof(szColor));
		AsciiRGB( szColor, &rgb );
		if (fHighlight)
		{
			// this relies on the fact that AsciiRGB replaces commas
			// with NULL terminators
			LPTSTR lp = szColor + lstrlen(szColor) + 1; // passed red
			lp += lstrlen(lp) + 1; // passed green
			lp += lstrlen(lp) + 1; // passed blue to higlight color
			AsciiRGB(lp, &rgb);
			rgb.rgbtRed = rgb.rgbtGreen = 0; rgb.rgbtBlue = 255;
		}
		else
			rgb.rgbtRed = rgb.rgbtGreen = rgb.rgbtBlue = 0;
		lpRGB = &rgb;
	}
	pStory->m_pDib->DibBlt( pdibDst,
				0, 0,
				rDstArea.right - rDstArea.left, rDstArea.bottom - rDstArea.top,
				rSrcArea.left, rSrcArea.top,
				rSrcArea.right - rSrcArea.left, rSrcArea.bottom - rSrcArea.top,
				YES /*bTransparent*/, lpRGB, App.m_lpLut );

	pdibDst->DCBlt( hDC,
				rSrcArea.left, rSrcArea.top,
				rSrcArea.right - rSrcArea.left, rSrcArea.bottom - rSrcArea.top,
			 	0, 0,
				rSrcArea.right - rSrcArea.left, rSrcArea.bottom - rSrcArea.top);

	delete pdibDst;
}
#endif

/***********************************************************************/
LOCAL BOOL ComputeDIBControlSize( HWND hControl, LPINT lpDX, LPINT lpDY )
/***********************************************************************/
{
	// Make sure that the DIB is uncompressed
	PSTORY pStory = GetStory(hControl);
	if (pStory && pStory->m_pDib)
	{
		if ( lpDX )
			*lpDX = (int)pStory->m_pDib->GetWidth();
		if ( lpDY )
			*lpDY = (int)abs(pStory->m_pDib->GetHeight());
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
BOOL Story_OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct)
/***********************************************************************/
{
	PSTORY pStory = new CStory(hWindow);
	if (pStory)
	{
		DWORD dwStyle = GetWindowLong(hWindow, GWL_STYLE);
		if (lstrlen(pStory->m_szDibFile))
		    pStory->m_pDib = CDib::LoadDibFromFile( pStory->m_szDibFile, NULL, (dwStyle & BS_MASK) != 0 );
		//if (lstrlen(pStory->m_szStoryFile))
 		//	pStory->m_hMCIfile = MCIOpen( App.m_hDeviceWAV, pStory->m_szStoryFile, NULL/*lpAlias*/ );
	}
	SetStory(hWindow, pStory);
	return(TRUE);
}

/***********************************************************************/
LOCAL void Story_OnDestroy(HWND hWindow)
/***********************************************************************/
{
	if ( bTrack )
	{
		ReleaseCapture();
		bTrack = NO;
	}
	PSTORY pStory = GetStory(hWindow);
	if (pStory)
	{
		delete pStory;
		SetStory(hWindow, NULL);
	}
}

/***********************************************************************/
LOCAL void Story_OnSize(HWND hWindow, UINT state, int cx, int cy)
/***********************************************************************/
{
	int dx, dy;

	if ( ComputeDIBControlSize( hWindow, &dx, &dy ) )
		ResizeControl( hWindow, dx, dy );
	FORWARD_WM_SIZE(hWindow, state, cx, cy, DefWindowProc);
}

/***********************************************************************/
LOCAL UINT Story_OnGetDlgCode(HWND hWindow, LPMSG lpmsg)
/***********************************************************************/
{
	return( DLGC_WANTALLKEYS );
}

/***********************************************************************/
LOCAL BOOL Story_OnEraseBkgnd(HWND hWindow, HDC hDC)
/***********************************************************************/
{
	return(TRUE);
}

/***********************************************************************/
LOCAL UINT Story_OnNCHitTest(HWND hWindow, int x, int y)
/***********************************************************************/
{
	UINT uReturn = FORWARD_WM_NCHITTEST(hWindow, x, y, DefWindowProc);
	if ( SHIFT && (uReturn == HTCLIENT) )
		uReturn = HTCAPTION;
	return( uReturn );
}

/***********************************************************************/
LOCAL void Story_OnMove(HWND hWindow, int x, int y)
/***********************************************************************/
{
	InvalidateRect( hWindow, NULL, FALSE );
}

/***********************************************************************/
LOCAL void Story_OnPaint(HWND hWindow)
/***********************************************************************/
{
	PAINTSTRUCT ps;
	HDC hDC;

	if (hDC = BeginPaint( hWindow, &ps ) )
	{
		Story_OnDraw(hWindow, hDC, &ps.rcPaint, FALSE);
		EndPaint( hWindow, &ps );
	}
}

/***********************************************************************/
LOCAL void Story_OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	if ( SHIFT )
		return;
	if ( bTrack )
		return;
	PSTORY pStory = GetStory(hWindow);
	if (!pStory)
		return;
	if (pStory->m_fPlaying)
		return;

	if (pStory->m_fPlaying)
		pStory->StopPlaying();
	SetCapture( hWindow ); bTrack = TRUE;
	if ( GetFocus() != hWindow )
		SetFocus( hWindow );
	bInRect = YES;
	DWORD dwStyle = GetWindowLong(hWindow, GWL_STYLE);
	int i;
	if ((i = pStory->FindHotSpot(x, y)) >= 0)
	{
		HDC hDC = GetDC(hWindow);
		pStory->m_iHighlight = i + 1;
		Story_OnDraw(hWindow, hDC, &pStory->m_lpWordData[i].rArea, TRUE);
		if (lstrlen(pStory->m_szStoryFile))
 			pStory->m_hMCIfile = MCIOpen( App.m_hDeviceWAV, pStory->m_szStoryFile, NULL/*lpAlias*/ );
		if ( pStory->m_hMCIfile )
		{
			FORWARD_WM_COMMAND(GetParent(hWindow), GET_WINDOW_ID(hWindow), hWindow, 0, SendMessage);
			MCIPlay( pStory->m_hMCIfile, NULL, pStory->m_lpWordData[i].dwFrom, pStory->m_lpWordData[i].dwTo );
			MCIClose( pStory->m_hMCIfile );
			pStory->m_hMCIfile = NULL;
		}
		ReleaseDC(hWindow, hDC);
	}
}

/***********************************************************************/
LOCAL void Story_OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	if ( !bTrack )
		return;
	ReleaseCapture(); bTrack = FALSE;
	DWORD dwStyle = GetWindowLong(hWindow, GWL_STYLE);
	PSTORY pStory = GetStory(hWindow);
	if (pStory && pStory->m_iHighlight)
	{
		HDC hDC = GetDC(hWindow);
 		Story_OnDraw(hWindow, hDC, &pStory->m_lpWordData[pStory->m_iHighlight-1].rArea, FALSE);
		ReleaseDC(hWindow, hDC);
		pStory->m_iHighlight = 0;
		FORWARD_WM_COMMAND(GetParent(hWindow), GET_WINDOW_ID(hWindow), hWindow, 3, SendMessage);
	}	  
//	if ( bInRect )
//	{
//		FORWARD_WM_COMMAND(GetParent(hWindow), GET_WINDOW_ID(hWindow), hWindow, 0, SendMessage);
//	}
}

/***********************************************************************/
LOCAL void Story_OnLButtonDblClk(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	FORWARD_WM_COMMAND(GetParent(hWindow), GET_WINDOW_ID(hWindow), hWindow, 1, SendMessage);
}

/***********************************************************************/
LOCAL void Story_OnRButtonUp(HWND hWindow, int x, int y, UINT keyFlags)
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
LOCAL void Story_OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags)
/***********************************************************************/
{
	RECT ClientRect;
	POINT pt;

	if ( !bTrack )
		return;
	GetClientRect( hWindow, &ClientRect );
	pt.x = x;
	pt.y = y;
	if ( bInRect == !PtInRect( &ClientRect, pt ) )
	{
		bInRect = !bInRect;
	}
}

/***********************************************************************/
LOCAL void Story_OnEnable(HWND hWindow, BOOL fEnable)
/***********************************************************************/
{
	FORWARD_WM_ENABLE(hWindow, fEnable, DefWindowProc);
	InvalidateRect( hWindow, NULL, FALSE );
	UpdateWindow(hWindow);
}

/***********************************************************************/
LOCAL void Story_OnPaletteChanged(HWND hWindow, HWND hwndPaletteChange)
/***********************************************************************/
{
	InvalidateRect(hWindow, NULL, FALSE);
}

//************************************************************************
LOCAL void Story_OnTimer(HWND hWindow, UINT id)
//************************************************************************
{
	PSTORY pStory = GetStory(hWindow);
	if (!pStory)
		return;

	DWORD dwPos = MCIGetPosition(pStory->m_hMCIfile);
	for (int i = 0; i < pStory->m_nRects; ++i)
	{
		if (dwPos >= pStory->m_lpWordData[i].dwFrom && dwPos <= pStory->m_lpWordData[i].dwTo)
		{
			if ((pStory->m_iHighlight-1) != i)
			{
				HDC hDC = GetDC(hWindow);
				if (pStory->m_iHighlight)
					Story_OnDraw(hWindow, hDC, &pStory->m_lpWordData[pStory->m_iHighlight-1].rArea, FALSE);
				Story_OnDraw(hWindow, hDC, &pStory->m_lpWordData[i].rArea, TRUE);
				pStory->m_iHighlight = i+1;
				ReleaseDC(hWindow, hDC);
			}
			break;
		}
	}
}

//***************************************************************************
UINT Story_OnMCINotify( HWND hWindow, UINT codeNotify, HMCI hDevice )
//***************************************************************************
{
	PSTORY pStory = GetStory(hWindow);
	switch( codeNotify )
	{
		case MCI_NOTIFY_SUCCESSFUL:
		{
			if (pStory && pStory->m_fPlaying)
			{
				pStory->Reset();
				FORWARD_WM_COMMAND(GetParent(hWindow), GET_WINDOW_ID(hWindow), hWindow, 2, SendMessage);
			}
			return FALSE;
		}

		case MCI_NOTIFY_ABORTED:	// wParam == 4
		{ // aborted msg sent during playback, if we get a
			// seek, step, stop, or full screen playback escape command.
			// continue playing after a seek or step, unless
			// escaping from a full screen playback
			return( FALSE );
		}

		case MCI_NOTIFY_SUPERSEDED:	// wParam == 2
		case MCI_NOTIFY_FAILURE:	// wParam == 8
		{ // superseded msg sent during playback if we get a new play command
			return( FALSE );
		}
	}

return( FALSE );
}

/***********************************************************************/
LOCAL void Story_OnKey(HWND hWindow, UINT vk, BOOL fDown, int cRepeat, UINT flags)
/***********************************************************************/
{
	if (vk == VK_ESCAPE)
	{
		PSTORY pStory = GetStory(hWindow);
		if (pStory && pStory->m_fPlaying)
			MCIStop(pStory->m_hMCIfile);
	}
}

/***********************************************************************/
LOCAL BOOL Story_OnSetCursor(HWND hWindow, HWND hWndCursor, UINT codeHitTest, UINT msg)
/***********************************************************************/
{
	PSTORY pStory = GetStory(hWindow);
	if (pStory)
	{	
		POINT CursorPos;

		GetCursorPos( &CursorPos );
		ScreenToClient( hWindow, &CursorPos );
		if (pStory->FindHotSpot(CursorPos.x, CursorPos.y) >= 0)
		{
			SetCursor(pStory->m_hHotSpotCursor);
			return(TRUE);
		}
	}
	return FORWARD_WM_SETCURSOR(hWindow, hWndCursor, codeHitTest, msg, DefWindowProc);
}

/***********************************************************************/
LONG WINPROC EXPORT StoryControl(HWND hWindow, UINT message,
							WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch ( message )
	{
		HANDLE_MSG(hWindow, WM_CREATE, Story_OnCreate);
		HANDLE_MSG(hWindow, WM_DESTROY, Story_OnDestroy);
		HANDLE_MSG(hWindow, WM_SIZE, Story_OnSize);
		HANDLE_MSG(hWindow, WM_GETDLGCODE, Story_OnGetDlgCode);
		HANDLE_MSG(hWindow, WM_ERASEBKGND, Story_OnEraseBkgnd);
		HANDLE_MSG(hWindow, WM_NCHITTEST, Story_OnNCHitTest);
		HANDLE_MSG(hWindow, WM_MOVE, Story_OnMove);
		HANDLE_MSG(hWindow, WM_PAINT, Story_OnPaint);
  		HANDLE_MSG(hWindow, WM_LBUTTONDOWN, Story_OnLButtonDown);
 		HANDLE_MSG(hWindow, WM_LBUTTONUP, Story_OnLButtonUp);
   		HANDLE_MSG(hWindow, WM_LBUTTONDBLCLK, Story_OnLButtonDblClk);
  		HANDLE_MSG(hWindow, WM_RBUTTONUP, Story_OnRButtonUp);
		HANDLE_MSG(hWindow, WM_MOUSEMOVE, Story_OnMouseMove);
		HANDLE_MSG(hWindow, WM_ENABLE, Story_OnEnable);
		HANDLE_MSG(hWindow, WM_PALETTECHANGED, Story_OnPaletteChanged);
		HANDLE_MSG(hWindow, WM_TIMER, Story_OnTimer);
		HANDLE_MSG(hWindow, MM_MCINOTIFY, Story_OnMCINotify);
		HANDLE_MSG(hWindow, WM_KEYDOWN, Story_OnKey);
		HANDLE_MSG(hWindow, WM_SETCURSOR, Story_OnSetCursor);

		default:
		return DefWindowProc( hWindow, message, wParam, lParam );
	}
}

/***********************************************************************/
PSTORY GetStory(HWND hWindow)
/***********************************************************************/
{
	return((PSTORY)GetWindowLong(hWindow, GWL_STORY));
}

/***********************************************************************/
void SetStory(HWND hWindow, PSTORY pStory)
/***********************************************************************/
{
	SetWindowLong(hWindow, GWL_STORY, (long)pStory);
}

/***********************************************************************/
CStory::CStory(HWND hWindow)
/***********************************************************************/
{
	STRING szString, szFile; 
	FNAME szFileName;
	HRSRC hResource;
	HGLOBAL hData;
	LPSHORT lpShortData;
	DWORD dwSize;
	int id;

	m_hWnd = hWindow;
	m_nRects = 0;
	m_hMCIfile = NULL;
	m_iHighlight = 0;
	m_pDib = NULL;
	m_lpWordData = NULL;
	m_fPlaying = FALSE;
	m_szStoryFile[0] = '\0';
	m_szDibFile[0] = '\0';
	m_hHotSpotCursor = LoadCursor(GetWindowInstance(hWindow), MAKEINTRESOURCE(ID_POINTER));
	m_fCursorEnabled = TRUE;
	m_fMappedToPalette = FALSE;

	GetModuleFileName(GetWindowInstance(hWindow), szFileName, sizeof(szFileName));
	StripFile(szFileName);

	GetWindowText(hWindow, szString, sizeof(szString));
	for (int i = 0; i < 2; ++i)
	{
		if (GetStringParm(szString, i, ',', szFile))
		{
			if (lstrcmpi(Extension(szFile), ".wav") == 0)
			{
				lstrcpy(m_szStoryFile, szFileName);
				lstrcat(m_szStoryFile, szFile);
			}
			else
			{
				lstrcpy(m_szDibFile, szFileName);
				lstrcat(m_szDibFile, szFile);
			}
		}
		else
			break;
	}

	id = GetDlgCtrlID(hWindow)-IDC_STORY;	
	if (id)
		wsprintf(szString, "story%d", id+1);
	else
		lstrcpy(szString, "story");
		
	id = App.GetSceneNo(GetParent(hWindow));
	if ( !(hResource = FindResource( App.GetInstance(), MAKEINTRESOURCE(id), szString )) )
		return;
	if ( !(dwSize = SizeofResource( App.GetInstance(), hResource )) )
		return;
	if ( !(hData = LoadResource( App.GetInstance(), hResource )) )
		return;
	if ( !(lpShortData = (LPSHORT)LockResource( hData )) )
		{
		FreeResource( hData );
		return;
		}
	m_nRects = (int)(dwSize / (sizeof(WORD) * 6));
	if (m_nRects)
	{
		m_lpWordData = (LPWORD_DATA)AllocX( (m_nRects * sizeof(WORD_DATA)), GMEM_ZEROINIT|GMEM_SHARE );
		if (!m_lpWordData)
			return;
		for (int i = 0; i < m_nRects; ++i)
		{
			m_lpWordData[i].dwFrom = (int)*lpShortData++;
			m_lpWordData[i].dwTo = (int)*lpShortData++;
			m_lpWordData[i].rArea.left = (int)*lpShortData++;
			m_lpWordData[i].rArea.top = (int)*lpShortData++;
			m_lpWordData[i].rArea.right = (int)*lpShortData++;
			m_lpWordData[i].rArea.bottom = (int)*lpShortData++;
		}
	}
	UnlockResource( hData );
	FreeResource( hData );
}

/***********************************************************************/
CStory::~CStory()
/***********************************************************************/
{
	if (m_hMCIfile)
		MCIClose(m_hMCIfile);
	if (m_pDib)
		delete m_pDib;
	if (m_lpWordData)
		FreeUp(m_lpWordData);
	if (m_hHotSpotCursor)
		DestroyCursor(m_hHotSpotCursor);
}

/***********************************************************************/
BOOL CStory::Play()
/***********************************************************************/
{
	// returns TRUE if it started playing
	if (lstrlen(m_szStoryFile))
		m_hMCIfile = MCIOpen( App.m_hDeviceWAV, m_szStoryFile, NULL/*lpAlias*/ );
	if (m_hMCIfile)
	{
		if (m_nRects)  
		{
			HDC hDC = GetDC(m_hWnd);
			m_iHighlight = 0 + 1;
			Story_OnDraw(m_hWnd, hDC, &m_lpWordData[0].rArea, TRUE);
			ReleaseDC(m_hWnd, hDC);
		}
		if (MCIPlay( m_hMCIfile, m_hWnd, 0))
		{
			EnableCursor(FALSE);
			m_fPlaying = TRUE;
			SetTimer( m_hWnd, 0xDEAD, 5, NULL );
			return(TRUE);
		}
		else
		{
			MCIClose( m_hMCIfile );
			m_hMCIfile = NULL;
		}
	}
	return(FALSE);
}

/***********************************************************************/
void CStory::StopPlaying()
/***********************************************************************/
{
	if (m_hMCIfile)
	{
		if (m_fPlaying)
		{
			MCIStop(m_hMCIfile, TRUE);
			Reset();
		}
	}
}

/***********************************************************************/
void CStory::Reset()
/***********************************************************************/
{
	if (m_hMCIfile)
	{
		MCIClose( m_hMCIfile );
		m_hMCIfile = NULL;
	}
	if (m_fPlaying)
	{
		EnableCursor(TRUE);
		m_fPlaying = FALSE;
		KillTimer( m_hWnd, 0xDEAD );
		if (m_iHighlight)
		{
			HDC hDC = GetDC(m_hWnd);
			Story_OnDraw(m_hWnd, hDC, &m_lpWordData[m_iHighlight-1].rArea, FALSE);
			m_iHighlight = 0;
			ReleaseDC(m_hWnd, hDC);
		}
	}
}

int CStory::FindHotSpot(int x, int y)
{
	POINT pt;

	pt.x = x; pt.y = y;
	for (int i = 0; i < m_nRects; ++i)
	{
		if (PtInRect(&m_lpWordData[i].rArea, pt))
			return (i);
	}
	return(-1);
}

void CStory::EnableCursor(BOOL fEnable)
{
	// make sure we are changing enable state
	if (FALSE && fEnable != m_fCursorEnabled)
	{
		m_fCursorEnabled = fEnable;
		if (fEnable)
			ReleaseCapture();
		else
			SetCapture(m_hWnd);
		ShowCursor(fEnable);
	}
}

