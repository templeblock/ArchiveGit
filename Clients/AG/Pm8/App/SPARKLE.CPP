/*
// $Workfile: SPARKLE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/SPARKLE.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:34:56   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:31:08   FRED
// Initial revision.
// 
//    Rev 2.2   07 Aug 1995 17:13:54   FRED
// Publishing Suite Install
// 
//    Rev 2.1   08 Feb 1995 13:35:06   JAY
// Reverted. New series.
// 
//    Rev 1.0   15 Jun 1994 19:01:34   FRED
// Initial revision.
*/ 

#pragma warning(disable:4100)
#pragma warning(disable:4505)
#pragma warning(disable:4704)

#ifndef NOSTRICT
#define STRICT
#endif

#include "stdafx.h"
#include "sparkle.h"

#ifndef ASSERT
#ifdef _DEBUG
#define ASSERT(p) if (!(p)) { MessageBox(NULL, "Assertion failed in " __FILE__, "Assertion Failed", MB_OK|MB_ICONEXCLAMATION); }
#else
#define ASSERT(p)
#endif
#endif

CSparkle::CSparkle()
{
	Release(TRUE);
}

CSparkle::~CSparkle()
{
	if (IsValid())
	{
		if (m_nCurrentCell != 0)
		{
			DrawCell(0);
		}
	}
	
	Release();
}

BOOL CSparkle::Init(
	HWND hWindow,					// window in which to draw sparkle
	HINSTANCE hInstance,			// instance containing resource
	LPCSTR lpResource,			// cell bitmap resource identifier
	COLORREF crMask,				// masking color
	HBITMAP hBackground,			// handle of background bitmap
	HPALETTE hPalette,			// palette to use
	int nCellWidth,				// cell width in pixels
	int nX,							// X position of sparkle
	int nY)							// Y position of sparkle
{
	#define	LOGICAL_WHITE	RGB(255,255,255)
	#define	LOGICAL_BLACK	RGB(0,0,0)

	/*
	// Release any currently used resources.
	*/
	
	Release();
		
	HDC hDC = NULL;
	
	HDC hCellDC = NULL;
	HBITMAP hCellBitmap = NULL;
	HBITMAP hOldCellBitmap = NULL;
	
	HDC hMaskDC = NULL;
	HBITMAP hMaskBitmap = NULL;
	HBITMAP hOldMaskBitmap = NULL;
	
	HDC hBackgroundDC = NULL;
	HBITMAP hOldBackgroundBitmap = NULL;
	
	HDC hSparkleDC = NULL;
	HBITMAP hSparkleBitmap = NULL;
	HBITMAP hOldSparkleBitmap = NULL;

	m_hWindow = hWindow;
	m_hPalette = hPalette;
	m_nX = nX;
	m_nY = nY;
		
	/*
	// Get the DC of the window to sparkle in.
	*/

	ASSERT(IsWindow(m_hWindow));
		
	if ((hDC = GetDC(m_hWindow)) == NULL)
	{
		goto cleanup;
	}
	
	int nOldMapMode;
	
	nOldMapMode = SetMapMode(hDC, MM_TEXT);
	
	/*
	// Load the bitmap resource.
	*/
	
	if ((hCellBitmap = LoadBitmap(hInstance, lpResource)) == NULL)
	{
		goto cleanup;
	}
	
	/*
	// Extract the size of the bitmap. The cells are arranged horizontally and the
	// total width must be an even multiple of the variable nCellWidth.
	*/
	
	BITMAP Bitmap;
	
	if (GetObject(hCellBitmap, sizeof(Bitmap), &Bitmap) == 0)
	{
		goto cleanup;
	}
	
	int nBitmapWidth;
	int nBitmapHeight;
	
	nBitmapWidth = Bitmap.bmWidth;
	nBitmapHeight = Bitmap.bmHeight;

	if ((nBitmapWidth == 0)
	 || ((nBitmapWidth % nCellWidth) != 0))
	{
		goto cleanup;
	}
	
	m_nNumberOfCells = nBitmapWidth/nCellWidth;
	m_nCellWidth = nCellWidth;
	m_nCellHeight = nBitmapHeight;
	
	/*
	// Create the mask for the cell data. The caller has specified a masking color.
	// Any pixels in the cell data with this color are made transparent.
	// The mask is created by BLTing the color cell data into a monochrome bitmap
	// with the background color in the source context set to the "mask" color.
	// Then, using the mask, the mask portions of the color cell data are set
	// to black so that they cell data can be OR'ed into the background data
	// after the mask has been applied.
	*/
	
	if (((hMaskBitmap = CreateBitmap(nBitmapWidth, nBitmapHeight, 1, 1, NULL)) == NULL)
	 || ((hCellDC = CreateCompatibleDC(hDC)) == NULL)
	 || (SetMapMode(hCellDC, MM_TEXT), FALSE)
	 || ((hOldCellBitmap = (HBITMAP)SelectObject(hCellDC, hCellBitmap)) == NULL)
	 || (SetBkColor(hCellDC, crMask) == 0x8000000L)
	 || ((hMaskDC = CreateCompatibleDC(hDC)) == NULL)
	 || (SetMapMode(hMaskDC, MM_TEXT), FALSE)
	 || ((hOldMaskBitmap = (HBITMAP)SelectObject(hMaskDC, hMaskBitmap)) == NULL)
	 || (BitBlt(hMaskDC, 0, 0, nBitmapWidth, nBitmapHeight, hCellDC, 0, 0, SRCCOPY) == 0)
	 || (SetTextColor(hCellDC, LOGICAL_WHITE) == 0x8000000L)
	 || (SetBkColor(hCellDC, LOGICAL_BLACK) == 0x8000000L)
	 || (BitBlt(hCellDC, 0, 0, nBitmapWidth, nBitmapHeight, hMaskDC, 0, 0, SRCAND) == 0))
	{
		goto cleanup;
	}
	
	/*
	// Construct a bitmap containing a copy of the background data for each cell.
	// Each cell is assumed to be centered on the sparkle origin.
	*/
	
	if (((hBackgroundDC = CreateCompatibleDC(hDC)) == NULL)
	 || (SetMapMode(hBackgroundDC, MM_TEXT), FALSE)
	 || ((hOldBackgroundBitmap = (HBITMAP)SelectObject(hBackgroundDC, hBackground)) == NULL)
	 || ((hSparkleBitmap = CreateCompatibleBitmap(hDC, m_nCellWidth+nBitmapWidth, nBitmapHeight)) == NULL)
	 || ((hSparkleDC = CreateCompatibleDC(hDC)) == NULL)
	 || (SetMapMode(hSparkleDC, MM_TEXT), FALSE)
	 || ((hOldSparkleBitmap = (HBITMAP)SelectObject(hSparkleDC, hSparkleBitmap)) == NULL)
	 || (BitBlt(hSparkleDC, 0, 0, m_nCellWidth, m_nCellHeight, hBackgroundDC, nX-(m_nCellWidth/2), nY-(m_nCellHeight/2), SRCCOPY) == 0))
	{
		goto cleanup;
	}
	
	int i;
	
	for (i = 1; i <= m_nNumberOfCells; i++)
	{
		if (BitBlt(hSparkleDC, m_nCellWidth*i, 0, m_nCellWidth, m_nCellHeight, hSparkleDC, 0, 0, SRCCOPY) == 0)
		{
			goto cleanup;
		}
	}
	
	/*
	// Form the final data for each cell by masking the cell data into the background data.
	*/
	
	if ((SetTextColor(hSparkleDC, LOGICAL_BLACK) == 0x8000000L)
	 || (SetBkColor(hSparkleDC, LOGICAL_WHITE) == 0x8000000L)
	 || (BitBlt(hSparkleDC, m_nCellWidth, 0, nBitmapWidth, nBitmapHeight, hMaskDC, 0, 0, SRCAND) == 0)
	 || (BitBlt(hSparkleDC, m_nCellWidth, 0, nBitmapWidth, nBitmapHeight, hCellDC, 0, 0, SRCPAINT) == 0))
	{
		goto cleanup;
	}
	
	/*
	// Everything worked, save the pointer to the bitmap.
	*/
	
	SelectObject(hSparkleDC, hOldSparkleBitmap);
	hOldSparkleBitmap = NULL;
	
	DeleteDC(hSparkleDC);
	hSparkleDC = NULL;
	
	m_hBitmap = hSparkleBitmap;
	hSparkleBitmap = NULL;
	
cleanup:

	/*
	// Free up any temporary resources still in use.
	*/

	if (hDC != NULL)
	{
		SetMapMode(hDC, nOldMapMode);
		ReleaseDC(m_hWindow, hDC);
		hDC = NULL;
	}

	if (hCellDC != NULL)
	{
		if (hOldCellBitmap != NULL)
		{
			SelectObject(hCellDC, hOldCellBitmap);
			hOldCellBitmap = NULL;
		}
		
		DeleteDC(hCellDC);
		hCellDC = NULL;
	}
	
	if (hCellBitmap != NULL)
	{
		DeleteObject(hCellBitmap);
		hCellBitmap = NULL;
	}
			
	if (hMaskDC != NULL)
	{
		if (hOldMaskBitmap != NULL)
		{
			SelectObject(hMaskDC, hOldMaskBitmap);
			hOldMaskBitmap = NULL;
		}
		
		DeleteDC(hMaskDC);
		hMaskDC = NULL;
	}
	
	if (hMaskBitmap != NULL)
	{
		DeleteObject(hMaskBitmap);
		hMaskBitmap = NULL;
	}
			
	if (hBackgroundDC != NULL)
	{
		if (hOldBackgroundBitmap != NULL)
		{
			SelectObject(hBackgroundDC, hOldBackgroundBitmap);
			hOldBackgroundBitmap = NULL;
		}
		
		DeleteDC(hBackgroundDC);
		hBackgroundDC = NULL;
	}
	
	if (hSparkleDC != NULL)
	{
		if (hOldSparkleBitmap != NULL)
		{
			SelectObject(hSparkleDC, hOldSparkleBitmap);
			hOldSparkleBitmap = NULL;
		}
		
		DeleteDC(hSparkleDC);
		hSparkleDC = NULL;
	}
	
	if (hSparkleBitmap != NULL)
	{
		DeleteObject(hSparkleBitmap);
		hSparkleBitmap = NULL;
	}
	
	if (m_hBitmap == NULL)
	{
		Release();
	}
			
	return IsValid();
	m_hBitmap != NULL;
}

BOOL CSparkle::DrawCell(void)
{
	/*
	// Compute the "next" cell to display.
	*/
	
	int nCell;
	
	nCell = m_nCurrentCell+1;
	
	if (nCell > m_nNumberOfCells)
	{
		nCell = 1;
	}
	
	return DrawCell(nCell);
}

BOOL CSparkle::DrawCell(int nCell)
{
	/*
	// Draw a particular cell.
	*/
	
	BOOL fResult = FALSE;
	
	if (IsValid())
	{
		if (nCell < 0)
		{
			nCell = 0;
		}
		
		if (nCell > m_nNumberOfCells)
		{
			nCell = m_nNumberOfCells;
		}
		
		m_nCurrentCell = nCell;
		
		HDC hDC;
		
		if (IsWindow(m_hWindow))
		{
			if ((hDC = GetDC(m_hWindow)) != NULL)
			{
				int nOldMapMode;
				
				nOldMapMode = SetMapMode(hDC, MM_TEXT);
				
				HDC hSparkleDC;
				
				if ((hSparkleDC = CreateCompatibleDC(hDC)) != NULL)
				{
					SetMapMode(hSparkleDC, MM_TEXT);
					
					HBITMAP hOldSparkleBitmap;
					
					if ((hOldSparkleBitmap = (HBITMAP)SelectObject(hSparkleDC, m_hBitmap)) != NULL)
					{
						HPALETTE hOldPalette;
						
						if ((hOldPalette = SelectPalette(hDC, m_hPalette, FALSE)) != NULL)
						{
							RealizePalette(hDC);
							
							if (BitBlt(hDC, m_nX-(m_nCellWidth/2), m_nY-(m_nCellHeight/2), m_nCellWidth, m_nCellHeight, hSparkleDC, nCell*m_nCellWidth, 0, SRCCOPY) != 0)
							{
								fResult = TRUE;
							}
							
							SelectPalette(hDC, hOldPalette, FALSE);
						}
						
						SelectObject(hSparkleDC, hOldSparkleBitmap);
						hOldSparkleBitmap = NULL;
					}
					
					DeleteDC(hSparkleDC);
					hSparkleDC = NULL;
				}
				
				SetMapMode(hDC, nOldMapMode);
				ReleaseDC(m_hWindow, hDC);
				hDC = NULL;
			}
		}
	}
	
	return fResult;
}

BOOL CSparkle::AtLastCell(void)
{
	return IsValid() && (m_nCurrentCell >= m_nNumberOfCells);
}
		
BOOL CSparkle::IsValid(void)
{
	return m_hBitmap != NULL;
}
	
BOOL CSparkle::Release(BOOL fConstructing)
{
	if (fConstructing)
	{
		m_hBitmap = NULL;
	}
	
	if (m_hBitmap != NULL)
	{
		DeleteObject(m_hBitmap);
		m_hBitmap = NULL;
	}
	
	m_hWindow = NULL;
	m_hPalette = NULL;
	m_nX = 0;
	m_nY = 0;	
	m_nNumberOfCells = 0;
	m_nCellWidth = 0;
	m_nCellHeight = 0;
	m_nCurrentCell = 0;
	
	return TRUE;
}

CSparklers::CSparklers()
{
	Release(TRUE);
}

CSparklers::~CSparklers()
{
	Release();
}

BOOL CSparklers::Init(
	HINSTANCE hInstance,					// instance containing resource
	HWND hWindow,							// window in which to draw sparkle
	HBITMAP hBackground,					// handle of background bitmap
	HPALETTE hPalette,					// palette to use
	UINT wSparkleCount,					// number of sparkles in sparkle array
	LPSPARKLEINFO pSparkleInfo)		// array of sparkle information structure
{
	/*
	// Release any currently used resources.
	*/
	
	Release();
	
	/*
	// Save the information passed to us. The background bitmap
	// (hBackground) and the sparkle information array must remain
	// valid through the lifetime of the CSparklers object.
	*/
	
	m_hInstance = hInstance;
	m_hWindow = hWindow;
	m_hBackground = hBackground;
	m_hPalette = hPalette;
	m_wSparkleCount = wSparkleCount;
	m_pSparkleInfo = pSparkleInfo;
	
	/*
	// Reset the sparkler back to the beginning of the
	// sparkle sequence.
	*/
	
	return Reset();
}

BOOL CSparklers::Reset(void)
{
	if (m_pSparkle != NULL)
	{
		delete m_pSparkle;
		m_pSparkle = NULL;
	}
	
	m_wSparkleIndex = 0;
	
	m_nCellDelay = 0;
	m_nSparkleDelay = 0;

	return IsValid();
}

BOOL CSparklers::Tick(void)
{
	BOOL fResult = FALSE;
	
	if (IsValid())
	{
		fResult = TRUE;
		
		if (m_nSparkleDelay > 0)
		{
			m_nSparkleDelay--;
		}
		
		else
		{
			if (m_nCellDelay > 0)
			{
				m_nCellDelay--;
			}
			
			else
			{
				if ((m_pSparkle != NULL)
				 && (m_pSparkle->AtLastCell()))
				{
					delete m_pSparkle;
					m_pSparkle = NULL;
					
					m_nSparkleDelay = GetDelay(m_pSparkleInfo[m_wSparkleIndex++].nSparkleDelay)-1;
				}
				
				if (m_nSparkleDelay <= 0)
				{
					if (m_pSparkle == NULL)
					{
						if (m_wSparkleIndex >= m_wSparkleCount)
						{
							m_wSparkleIndex = 0;
						}
						
						if ((m_pSparkle = new CSparkle) != NULL)
						{
							if (!m_pSparkle->Init(
										m_hWindow,												// window in which to draw sparkle
										m_hInstance,											// instance containing resource
										m_pSparkleInfo[m_wSparkleIndex].lpResource,	// cell bitmap resource identifier
										m_pSparkleInfo[m_wSparkleIndex].crMask,		// masking color
										m_hBackground,											// handle of background bitmap
										m_hPalette,												// palette to use
										m_pSparkleInfo[m_wSparkleIndex].nCellWidth,	// cell width in pixels
										m_pSparkleInfo[m_wSparkleIndex].nX,				// X position of sparkle
										m_pSparkleInfo[m_wSparkleIndex].nY))			// Y position of sparkle
							{
								delete m_pSparkle;
								m_pSparkle = NULL;
							}
						}
					}
					
					fResult = FALSE;
					
					if (m_pSparkle != NULL)
					{
						if (m_pSparkle->DrawCell())
						{
							m_nCellDelay = GetDelay(m_pSparkleInfo[m_wSparkleIndex].nCellDelay);
							fResult = TRUE;
						}
					}
				}
			}
		}
	}
	
	return fResult;
}

BOOL CSparklers::IsValid(void)
{
	return m_pSparkleInfo != NULL;
}

int CSparklers::GetDelay(int nDelay)
{
	if (nDelay >= 0)
	{
		return nDelay;
	}
	
	return (int)(GetTickCount() % (DWORD)(-nDelay));
}

BOOL CSparklers::Release(BOOL fConstructing)
{
	if (fConstructing)
	{
		m_pSparkle = NULL;
	}
	
	if (m_pSparkle != NULL)
	{
		delete m_pSparkle;
		m_pSparkle = NULL;
	}

	m_hInstance = NULL;
	m_hWindow = NULL;
	m_hBackground = NULL;
	m_hPalette = NULL;
	m_wSparkleCount = 0;
	m_pSparkleInfo = NULL;
	m_wSparkleIndex = 0;
	m_nCellDelay = 0;
	m_nSparkleDelay = 0;
	
	return TRUE;
}
