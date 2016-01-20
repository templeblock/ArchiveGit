/*
// $Workfile: CustBar.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CustBar.cpp $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 6     6/11/98 9:41a Dennis
// Added HINSTANCE to bitmap loading. Now uses Util::LoadResourceBitmap
// 
// 5     4/03/98 2:54p Fredf
// Traps WM_DISPLAYCHANGE, WM_WININICHANGE, and WM_SETTINGCHANGED to
// default weird behavior of toolbar control which sizes the buttons very
// small and changes the bitmap colors.
// 
// 4     3/30/98 4:32p Fredf
// Fixed resource leak.
// 
// 3     3/17/98 2:42p Fredf
// New workspace toolbars.
// 
// 2     3/16/98 6:46p Fredf
// 
// 1     3/16/98 5:23p Fredf
*/

#include "stdafx.h"
#include "CustBar.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCustomToolBar

CCustomToolBar::CCustomToolBar()
{
	m_pPalette = NULL;
	m_pBackground = NULL;
	m_clBackground = GetSysColor(COLOR_BTNFACE);
	m_pBitmap = NULL;
	m_pBitmapMask = NULL;
	m_nBitmapIDCount = 0;
	m_pBitmapIDArray = NULL;
	m_nLastButtonUnderCursor = -1;
	m_uTimer = 0;
}

CCustomToolBar::~CCustomToolBar()
{
	// Free any allocated memory.
	FreeBitmaps();
}

BOOL CCustomToolBar::SetPalette(CPalette* pPalette)
{
	// Set the palette for the toolbar.
	
	// NOTE: The palette object is owned by the caller and must
	// remain valid throughout the life of the toolbar.

	m_pPalette = pPalette;

	// Redraw the window. It's probably not a good idea to change the
	// palette without resetting the background and button bitmaps since
	// any existing bitmaps were created and adjusted for the previous
	// palette, not the new one.
	//
	// IDEA: The toolbar could remember the resource names for the
	// background and button bitmaps and auto-reload them with the
	// new palette.

	Invalidate();

	return TRUE;
}

BOOL CCustomToolBar::SetBackgroundBitmap(LPCTSTR pszResourceName, HINSTANCE hInstance)
{
	BOOL fResult = FALSE;

	// Free any background bitmap that was allocated.
	FreeBackground();

	// Load the new background bitmap.
	TRY
	{
		m_pBackground = new CBitmap;
		if (Util::LoadResourceBitmap(*m_pBackground, pszResourceName, m_pPalette, hInstance))
		{
			// Redraw the window.
			Invalidate();

			fResult = TRUE;
		}
	}
	END_TRY

	if (!fResult)
	{
		// Something failed, make sure everything is freed.
		FreeBackground();
	}

	return fResult;
}

BOOL CCustomToolBar::SetBackgroundColor(COLORREF clColor)
{
	// Free any background bitmap that was allocated.
	if (m_pBackground != NULL)
	{
		m_pBackground->DeleteObject();
		delete m_pBackground;
		m_pBackground = NULL;
	}

	// Save the new color.
	m_clBackground = clColor;

	// Redraw the window.
	Invalidate();

	return TRUE;
}

BOOL CCustomToolBar::SetButtonBitmaps(LPCTSTR pszResourceName, COLORREF clTransparent, SIZE sizeButton, const UINT* lpIDArray, int nIDCount, HINSTANCE hInstance)
{
	BOOL fResult = FALSE;

	// Free any existing bitmaps.
	FreeBitmaps();

	// Sanity Check.
	ASSERT(lpIDArray != NULL);
	ASSERT(nIDCount > 0);

	TRY
	{
		if ((lpIDArray != NULL)
		 && (nIDCount > 0))
		{
			// Set the button and image sizes for the toolbar.
			// We fake the image size at the minimum value (1,1).
			// For CCustomToolBar, the button and image sizes
			// are always the same.
			ASSERT(sizeButton.cx > 7);
			ASSERT(sizeButton.cy > 6);
			CToolBar::SetSizes(sizeButton, CSize(1, 1));

			// Attempt to load the button bitmaps.
			m_pBitmap = new CBitmap;
         if (Util::LoadResourceBitmap(*m_pBitmap, pszResourceName, m_pPalette, hInstance))
			{
				// Make a copy of the button bitmap ID array.
				m_nBitmapIDCount = nIDCount;
				m_pBitmapIDArray = new UINT[m_nBitmapIDCount];
				memcpy(m_pBitmapIDArray, lpIDArray, m_nBitmapIDCount*sizeof(*lpIDArray));

				// If there's a transparent color, then we need to create a mask bitmap.
				if (clTransparent == (COLORREF)-1)
				{
					// No transparency, so no mask needed.
					m_pBitmapMask = NULL;
					fResult = TRUE;
				}
				else
				{
					// Create a monochrome mask for the button bitmap.

					// Get the bitmap dimensions.
					BITMAP Bitmap;
					if (m_pBitmap->GetBitmap(&Bitmap))
					{
						// Create a DC to hold the bitmap.
						CDC dcBitmap;
						if (dcBitmap.CreateCompatibleDC(NULL))
						{
							// Select our palette into the DC.
							CPalette* pOldPalette;
							SelectPalette(dcBitmap, pOldPalette, TRUE);

							// Select the bitmap into the DC. This will be
							// the "source" DC (color).
							CBitmap* pOldBitmap = dcBitmap.SelectObject(m_pBitmap);
							if (pOldBitmap != NULL)
							{
								// Create the monochrome bitmap for the mask.
								m_pBitmapMask = new CBitmap;
								m_pBitmapMask->CreateBitmap(Bitmap.bmWidth, Bitmap.bmHeight, 1, 1, NULL);

								// Create a DC to hold the mask bitmap.
								CDC dcBitmapMask;
								if (dcBitmapMask.CreateCompatibleDC(&dcBitmap))
								{
									// Select the mask bitmap into the DC. This will be
									// the "destination" DC (monochrome).
									CBitmap* pOldBitmapMask = dcBitmapMask.SelectObject(m_pBitmapMask);
									if (pOldBitmapMask != NULL)
									{
										// Remember the text and background colors set
										// in the source DC. We will be changing them
										// and we'll want to restore them later.
										COLORREF clOldTextColor = dcBitmap.GetTextColor();
										COLORREF clOldBkColor = dcBitmap.GetBkColor();

										// Set the background color of the source DC
										// to the color we want to used for the masked
										// areas. When the color bitmap is copied to
										// the monochrome bitmap, the pixels that match
										// the specified color will be set to 1 in the
										// mask and the other pixels will be set to 0.
										dcBitmap.SetBkColor(PaletteColor(clTransparent));

										// Copy the data. The monochrome mask will be created.
										dcBitmapMask.BitBlt(
											0,
											0,
											Bitmap.bmWidth,
											Bitmap.bmHeight,
											&dcBitmap,
											0,
											0,
											SRCCOPY);

										// Now use the monochrome mask to set all the
										// transparent pixels in the original bitmap
										// to 0's (black). The makes easy to OR the
										// color bitmap onto the destination and
										// achieve a transparent effect.
										dcBitmap.SetTextColor(RGB(255,255,255)); // 0's in mask (opaque) go to 1's
										dcBitmap.SetBkColor(RGB(0,0,0));         // 1's in mask (transparent) go to 0's

										dcBitmap.BitBlt(
											0,
											0,
											Bitmap.bmWidth,
											Bitmap.bmHeight,
											&dcBitmapMask,
											0,
											0,
											SRCAND);

										// Restore the colors we changed.
										dcBitmap.SetBkColor(clOldBkColor);
										dcBitmap.SetTextColor(clOldTextColor);

										// Select the old bitmap back in.
										dcBitmapMask.SelectObject(pOldBitmapMask);
										pOldBitmapMask = NULL;

										// Everything worked!
										fResult = TRUE;
									}

									dcBitmapMask.DeleteDC();
								}

								dcBitmap.SelectObject(pOldBitmap);
								pOldBitmap = NULL;

								// Restore the previous palette.
								DeselectPalette(dcBitmap, pOldPalette, TRUE);
							}

							dcBitmap.DeleteDC();
						}
					}
				}
			}
		}
	}
	END_TRY

	if (!fResult)
	{
		// Something did not work. Make sure any allocated memory is freed.
		FreeBitmaps();
	}

	// Redraw ourselves.
	Invalidate();

	return fResult;
}

void CCustomToolBar::FreeBackground(void)
{
	// Free any background bitmap that was allocated.
	if (m_pBackground != NULL)
	{
		m_pBackground->DeleteObject();
		delete m_pBackground;
		m_pBackground = NULL;
	}
}

void CCustomToolBar::FreeBitmaps(void)
{
	// Free any bitmap that was allocated.
	FreeBackground();

	// Free any bitmap that was allocated.
	if (m_pBitmap != NULL)
	{
		m_pBitmap->DeleteObject();
		delete m_pBitmap;
		m_pBitmap = NULL;
	}

	// Free any mask bitmap that was allocated.
	if (m_pBitmapMask != NULL)
	{
		m_pBitmapMask->DeleteObject();
		delete m_pBitmapMask;
		m_pBitmapMask = NULL;
	}

	// Free any ID array that was allocated.
	m_nBitmapIDCount = 0;
	if (m_pBitmapIDArray != NULL)
	{
		delete [] m_pBitmapIDArray;
		m_pBitmapIDArray = NULL;
	}
}

void CCustomToolBar::SelectPalette(CDC& dc, CPalette*& pOldPalette, BOOL fForceBackground)
{
	// Helper method for selecting the palette into a DC.
   pOldPalette = NULL;
   if (m_pPalette != NULL)
   {
		pOldPalette = dc.SelectPalette(m_pPalette, fForceBackground);

		if (!fForceBackground)
		{
			dc.RealizePalette();
		}
   }
}

void CCustomToolBar::DeselectPalette(CDC& dc, CPalette*& pOldPalette, BOOL fForceBackground)
{
	// Helper method for deselecting the palette out of a DC.
   if (pOldPalette != NULL)
   {
      dc.SelectPalette(pOldPalette, fForceBackground);

		if (!fForceBackground)
		{
			dc.RealizePalette();
		}

      pOldPalette = NULL;
   }
}

void CCustomToolBar::DrawBorder(CDC& dc, CRect& crBorder)
{
	// Override this function to draw custom borders.

	// Be default, we call CToolBar::DrawBorders(). The rectangle, crBorder,
	// must be adjusted on return to specify the area inside the borders.
	// This area is then typically filled with the background pattern. If
	// you provide new functionality, you must also adjust crBorder or all
	// your custom drawing could be overwritten.

	// This function assumes that the caller has done all necessary
	// palette selection into the specified DC.

	CToolBar::DrawBorders(&dc, crBorder);
}

void CCustomToolBar::DrawBackground(CDC& dc, const CRect& crBackground, const CSize& czOffset)
{
	// Override this function to draw a custom background.

	// This function assumes that the caller has done all necessary
	// palette selection into the specified DC.

	// Check if we have a solid color background. If so, then just fill
	// the rectangle with the color.

	// IDEA: It would be more flexible to replace the "solid color" background
	// with a brush background. The brush would be owned by the toolbar.

	if (m_pBackground == NULL)
	{
		// Solid color. Fill the background rectangle.
		dc.FillSolidRect(crBackground, PaletteColor(m_clBackground));
	}
	else
	{
		// Bitmap background. We need to tile the bitmap onto the rectangle.
		// The offset parameter, czOffset, gives the offset from the origin
		// of the tiled background pattern to the origin of the specified DC.

		// Get the dimensions of the background bitmap.
		BITMAP Bitmap;
		if (m_pBackground->GetBitmap(&Bitmap))
		{
			// Create a memory DC to hold the background bitmap.
			CDC dcBitmap;
			if (dcBitmap.CreateCompatibleDC(&dc))
			{
				// Select the background bitmap into the DC.
				CBitmap* pOldBitmap = dcBitmap.SelectObject(m_pBackground);
				if (pOldBitmap != NULL)
				{
					// Compute the origin of the first background tile to
					// use for the fill.
					int nX0 = crBackground.left-((crBackground.left+czOffset.cx) % Bitmap.bmWidth);
					if (nX0 > crBackground.left)
					{
						nX0 -= Bitmap.bmWidth;
					}
					ASSERT(nX0 <= crBackground.left);

					int nY0 = crBackground.top-((crBackground.top+czOffset.cy) % Bitmap.bmHeight);
					if (nY0 > crBackground.top)
					{
						nY0 -= Bitmap.bmHeight;
					}
					ASSERT(nY0 <= crBackground.top);

					// Draw all the tiles required to cover the background. This could
					// be changed to draw to an offscreen bitmap, but I don't see any
					// advantage to this.
					for (int nY = nY0; nY < crBackground.bottom; nY += Bitmap.bmHeight)
					{
						for (int nX = nX0; nX < crBackground.right; nX += Bitmap.bmWidth)
						{
							// Figure out the BitBlt parameters for this tile.
							// This function does not assume that the DC has
							// a clipping region set up for the background
							// rectangle. The following code makes sure that
							// no drawing occurs outside of crBackground.

							int nDestinationX = nX;
							int nDestinationY = nY;
							int nXSize = Bitmap.bmWidth;
							int nYSize = Bitmap.bmHeight;
							int nSourceX = 0;
							int nSourceY = 0;

							// Clip left.
							if (nDestinationX < crBackground.left)
							{
								int nDelta = crBackground.left-nDestinationX;
								nXSize -= nDelta;
								nDestinationX += nDelta;
								nSourceX += nDelta;
							}

							// Clip top.
							if (nDestinationY < crBackground.top)
							{
								int nDelta = crBackground.top-nDestinationY;
								nYSize -= nDelta;
								nDestinationY += nDelta;
								nSourceY += nDelta;
							}

							// Clip right.
							if (nDestinationX+nXSize > crBackground.right)
							{
								nXSize -= nDestinationX+nXSize-crBackground.right;
							}

							// Clip bottom.
							if (nDestinationY+nYSize > crBackground.bottom)
							{
								nYSize -= nDestinationY+nYSize-crBackground.bottom;
							}

							if ((nXSize > 0) && (nYSize > 0))
							{
								// Draw the tile (or whatever's left of it after clipping.)
								dc.BitBlt(
									nDestinationX,
									nDestinationY,
									nXSize,
									nYSize,
									&dcBitmap,
									nSourceX,
									nSourceY,
									SRCCOPY);
							}
						}
					}

					dcBitmap.SelectObject(pOldBitmap);
					pOldBitmap = NULL;
				}
			}
		}
	}
}

void CCustomToolBar::DrawButton(CDC& dc, const CRect& crButton, int nButtonIndex, UINT uButtonID, WORD wButtonStyle, WORD wButtonState)
{
	// Override this function to draw custom buttons.

	// This function assumes that the caller has done all necessary
	// palette selection into the specified DC.

	// Make sure we have a bitmap to draw from.
	if (BitmapInitialized())
	{
		// Figure out which bitmap to use. In general, the button
		// index is not the same as the bitmap index.
		int nBitmapIndex = -1;
		for (int nIndex = 0; nIndex < m_nBitmapIDCount; nIndex++)
		{
			if (m_pBitmapIDArray[nIndex] == uButtonID)
			{
				nBitmapIndex = nIndex;
				break;
			}
		}

		// Draw the bitmap that corresponds to the button ID.
		if (nBitmapIndex != -1)
		{
			// Create a memory DC to hold the bitmap.
			CDC dcBitmap;
			if (dcBitmap.CreateCompatibleDC(&dc))
			{
				// Select the bitmap into the memory DC.
				CBitmap* pOldBitmap = dcBitmap.SelectObject(m_pBitmap);
				if (pOldBitmap != NULL)
				{
					// Figure out which state bitmap to use:
					//
					//		0:		Up			/ Cursor not over button
					//		1:		Up			/ Cursor over button
					//		2:		Down
					//		3:		Checked	/ Cursor not over button
					//		4:		Checked	/ Cursor over button
					//		5:		Disabled	/ Cursor not over button
					//		6:		Disabled	/ Cursor over button

					// Set some state variables.
					BOOL fButtonPressed ((wButtonState & TBSTATE_PRESSED) != 0);
					BOOL fButtonChecked ((wButtonState & TBSTATE_CHECKED) != 0);
					BOOL fButtonEnabled = GetToolBarCtrl().IsButtonEnabled((int)uButtonID);
					BOOL fButtonUnderCursor = IsActive() && (nButtonIndex == m_nLastButtonUnderCursor);

					int nX = nBitmapIndex;
					int nY = 0;
					if (!fButtonEnabled)
					{
						// Button is disabled.
						nY = fButtonUnderCursor ? 6 : 5;
					}
					else if (fButtonChecked)
					{
						// Button is checked.
						nY = fButtonUnderCursor ? 4 : 3;
					}
					else if (fButtonPressed)
					{
						// Button is pressed.
						nY = 2;
					}
					else
					{
						nY = fButtonUnderCursor ? 1 : 0;
					}

					// Draw the bitmap.
					if (m_pBitmapMask == NULL)
					{
						// There are no transparent areas, just copy the button bitmap.
						dc.BitBlt(
							crButton.left,
							crButton.top,
							crButton.Width(),
							crButton.Height(),
							&dcBitmap,
							nX*m_sizeButton.cx,
							nY*m_sizeButton.cy,
							SRCCOPY);
					}
					else
					{
						// There are transparent areas. We need to use the mask to
						// draw transparently.

						// Create a memory DC to hold the monochrome mask bitmap.
						CDC dcBitmapMask;
						if (dcBitmapMask.CreateCompatibleDC(&dcBitmap))
						{
							// Select the monochrom mask bitmap into the DC.
							CBitmap* pOldBitmapMask = dcBitmapMask.SelectObject(m_pBitmapMask);
							if (pOldBitmapMask != NULL)
							{
								// Erase to 0's (black) the areas of the desintation
								// where the opaque portions of the bitmap will be drawn.
								// This allows us to OR the button bitmap into the
								// destination.
								COLORREF clOldTextColor = dc.SetTextColor(RGB(0,0,0));    // 0's in mask (opaque) go to 0's
								COLORREF clOldBkColor = dc.SetBkColor(RGB(255,255,255));  // 1's in mask (transparent) go to 1's

								dc.BitBlt(
									crButton.left,
									crButton.top,
									crButton.Width(),
									crButton.Height(),
									&dcBitmapMask,
									nX*m_sizeButton.cx,
									nY*m_sizeButton.cy,
									SRCAND);

								// Restore old colors.
								dc.SetTextColor(clOldTextColor);
								dc.SetBkColor(clOldBkColor);

								// Restore the previously selected bitmap.
								dcBitmapMask.SelectObject(pOldBitmapMask);
								pOldBitmapMask = NULL;

								// OR the button bitmap into the destination.
								// The button bitmap has been preprocessed
								// in SetButtonBitmaps so that the transparent
								// areas are set to 0's. The destination bitmap
								// has already been cleared to 0's for the
								// opaque portions of the bitmap. Thus we can
								// OR the button bitmap data into the destination
								// bitmap data for a transparent effect.

								dc.BitBlt(
									crButton.left,
									crButton.top,
									crButton.Width(),
									crButton.Height(),
									&dcBitmap,
									nX*m_sizeButton.cx,
									nY*m_sizeButton.cy,
									SRCPAINT);
							}

							dcBitmapMask.DeleteDC();
						}
					}
						
					dcBitmap.SelectObject(pOldBitmap);
					pOldBitmap = NULL;
				}

				dcBitmap.DeleteDC();
			}
		}
	}
}

void CCustomToolBar::DrawSeparator(CDC& dc, const CRect& crButton, int nButtonIndex, UINT uButtonID, WORD wButtonStyle, WORD wButtonState)
{
	// Override this function to draw custom separators.

	// Normally, you don't want to draw separators if they are
	// at a wrap point.
	if ((wButtonState & TBSTATE_WRAP) == 0)
	{
		// Draw the separator.
		
		// This function assumes that the caller has done all necessary
		// palette selection into the specified DC.
	}
}

BEGIN_MESSAGE_MAP(CCustomToolBar, CToolBar)
	//{{AFX_MSG_MAP(CCustomToolBar)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_NCPAINT()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
	ON_MESSAGE(WM_WININICHANGE, OnDisplayChange)
	ON_MESSAGE(WM_SETTINGCHANGE, OnDisplayChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCustomToolBar message handlers

BOOL CCustomToolBar::OnEraseBkgnd(CDC* pDC) 
{
	// We want to draw everything to an offscreen bitmap to
	// avoid flicker, so we draw the background in OnPaint().
	return FALSE;
}

void CCustomToolBar::OnNcPaint() 
{
	// Get window DC that we will be drawing into.
	CWindowDC dc(this);

	// Create a DC for the offscreen bitmap. By using an offscreen bitmap
	// we can ensure that there will be no flickering.
	CDC dcOffScreen;
	if (dcOffScreen.CreateCompatibleDC(&dc))
	{
		// Compute the rectangle we will be updating.
		CRect crUpdate;
		GetWindowRect(crUpdate);
		ScreenToClient(crUpdate);

		// The DC we have is for the entire window including the non-client
		// area, so we have to make it zero-based for that area.
		crUpdate.OffsetRect(-crUpdate.left, -crUpdate.top);

		// Create the offscreen bitmap in a format compatible
		// with the destination DC.
		CBitmap OffScreenBitmap;
		if (OffScreenBitmap.CreateCompatibleBitmap(&dc, crUpdate.Width(), crUpdate.Height()))
		{
			// Select our palette into the offscreen DC.
			CPalette* pOldOffScreenPalette;
			SelectPalette(dcOffScreen, pOldOffScreenPalette, TRUE);

			// Select the bitmap into the DC.
			CBitmap* pOldBitmap = dcOffScreen.SelectObject(&OffScreenBitmap);
			if (pOldBitmap != NULL)
			{
				// Draw borders in non-client area.
				CRect crBorder(crUpdate);
				DrawBorder(dcOffScreen, crBorder);

				// Don't allow background to draw over borders.
				dcOffScreen.IntersectClipRect(crBorder);

				// Draw the background.
				CRect crOffset;
				GetWindowRect(crOffset);
				ScreenToClient(crOffset);
				DrawBackground(dcOffScreen, crBorder, CSize(crOffset.left, crOffset.top));

				// We now have the bitmap for the non-client area. We want to
				// copy this onto the window DC without overwriting the client
				// area. To do this we compute the client area and exclude it
				// from the clipping rectangle.
				CRect crClient;
				GetClientRect(crClient);
				crClient.OffsetRect(-crOffset.left, -crOffset.top);
				dc.ExcludeClipRect(crClient);

				// Select our palette into the DC.
				CPalette* pOldPalette;
				SelectPalette(dc, pOldPalette, FALSE);

				// Copy the offscreen bitmap to the destination DC.
				dc.BitBlt(
					crUpdate.left,
					crUpdate.top,
					crUpdate.Width(),
					crUpdate.Height(),
					&dcOffScreen,
					crUpdate.left,
					crUpdate.top,
					SRCCOPY);

				// Restore the previous palette.
				DeselectPalette(dc, pOldPalette, FALSE);

				dcOffScreen.SelectObject(pOldBitmap);
				pOldBitmap = NULL;
			}

			// Cleanup the offscreen bitmap.
			OffScreenBitmap.DeleteObject();
			DeselectPalette(dcOffScreen, pOldOffScreenPalette, TRUE);
		}

		dcOffScreen.DeleteDC();
	}
}

void CCustomToolBar::OnPaint() 
{
	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////
	// Copied from CToolBar::OnPaint()

	if (m_bDelayedButtonLayout)
	{
		m_bDelayedButtonLayout = FALSE;
		BOOL bHorz = (m_dwStyle & CBRS_ORIENT_HORZ) != 0;
		if ((m_dwStyle & CBRS_FLOATING) && (m_dwStyle & CBRS_SIZE_DYNAMIC))
			CalcDynamicLayout(0, LM_HORZ | LM_MRUWIDTH | LM_COMMIT);
		else if (bHorz)
 			CalcDynamicLayout(0, LM_HORZ | LM_HORZDOCK | LM_COMMIT);
		else
			CalcDynamicLayout(0, LM_VERTDOCK | LM_COMMIT);
	}

	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////

	// Get the rectangle we're going to be drawing.
	CRect crUpdate;
	if (GetUpdateRect(crUpdate))
	{
		// Get the actual device context we will be painting onto.
		CPaintDC dc(this);

		// Select out palette into the DC.
		CPalette* pOldPalette;
		SelectPalette(dc, pOldPalette, FALSE);

		// We construct an offscreen bitmap with the contents
		// to copy onto the final DC. This should avoid all flicker.

		// Create a DC for the offscreen bitmap.
		CDC dcOffScreen;
		if (dcOffScreen.CreateCompatibleDC(&dc))
		{
			// Create the offscreen bitmap in a format compatible
			// with the destination DC.
			CBitmap OffScreenBitmap;
			if (OffScreenBitmap.CreateCompatibleBitmap(&dc, crUpdate.Width(), crUpdate.Height()))
			{
				// Select our palette into the offscreen DC.
				CPalette* pOldOffScreenPalette;
				SelectPalette(dcOffScreen, pOldOffScreenPalette, TRUE);

				// Select the bitmap into the DC.
				CBitmap* pOldBitmap = dcOffScreen.SelectObject(&OffScreenBitmap);
				if (pOldBitmap != NULL)
				{
					// Offset the origin of the offscreen DC to match
					// the coordinate system of the final destination DC.
					dcOffScreen.OffsetWindowOrg(crUpdate.left, crUpdate.top);

					// Draw the background (DrawBackground() is virtual.)
					DrawBackground(dcOffScreen, crUpdate, CSize(0,0));

					int nButtons = GetToolBarCtrl().GetButtonCount();

					for (int nButton = 0; nButton < nButtons; nButton++)
					{
						// Get the bounding rectangle of the button.
						CRect crButton;
						GetItemRect(nButton, crButton);

						// Check if the button is in the update region. If not, then
						// there's nothing to draw.
						CRect crIntersect;
						if (crIntersect.IntersectRect(crButton, crUpdate))
						{
							// Get the button information.
							UINT uButtonID;
							UINT uButtonStyleState;
							int nButtonBitmap;
							GetButtonInfo(nButton, uButtonID, uButtonStyleState, nButtonBitmap);
							WORD wButtonStyle = LOWORD(uButtonStyleState);
							WORD wButtonState = HIWORD(uButtonStyleState);

							// If the button is hidden, then there's nothing to draw.
							if ((wButtonState & TBSTATE_HIDDEN) == 0)
							{
								if (wButtonStyle == TBSTYLE_SEP)
								{
									// Draw the separator (DrawSeparator() is virtual.)
									DrawSeparator(dcOffScreen, crButton, nButton, uButtonID, wButtonStyle, wButtonState);
								}
								else
								{
									// Draw the button (DrawButton() is virtual.)
									DrawButton(dcOffScreen, crButton, nButton, uButtonID, wButtonStyle, wButtonState);
								}
							}
						}
					}

					// Copy the offscreen bitmap to the destination DC.
					dc.BitBlt(
						crUpdate.left,
						crUpdate.top,
						crUpdate.Width(),
						crUpdate.Height(),
						&dcOffScreen,
						crUpdate.left,
						crUpdate.top,
						SRCCOPY);

					dcOffScreen.SelectObject(pOldBitmap);
					pOldBitmap = NULL;
				}

				// Clean up the offscreen bitmap and DC.
				OffScreenBitmap.DeleteObject();
				DeselectPalette(dcOffScreen, pOldOffScreenPalette, TRUE);
			}

			dcOffScreen.DeleteDC();
		}

		DeselectPalette(dc, pOldPalette, FALSE);
	}
}

void CCustomToolBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	// Check if the mouse is over one of our buttons. If so, then
	// we may need to redraw.

	// If we have the capture, then, by convention, we assume the
	// mouse is still over the button that has the capture.
	if ((CWnd::GetCapture() != this)
	 || (m_nLastButtonUnderCursor == -1))
	{
		// Remember the button that was underneath the cursor.
		int nButtonUnderCursor = -1;

		// Search for the button which is under the cursor. During
		// the search we can possible redraw two buttons: the button
		// that *was* under the cursor, and the button that is *now*
		// under the cursor. Only non-hidden, non-seprator buttons
		// are readrawn.
		int nButtons = GetToolBarCtrl().GetButtonCount();
		for (int nButton = 0; nButton < nButtons; nButton++)
		{
			// Get the bounding rectangle of the button.
			CRect crButton;
			GetItemRect(nButton, crButton);

			// Get the button information.
			UINT uButtonID;
			UINT uButtonStyleState;
			int nButtonBitmap;
			GetButtonInfo(nButton, uButtonID, uButtonStyleState, nButtonBitmap);
			WORD wButtonStyle = LOWORD(uButtonStyleState);
			WORD wButtonState = HIWORD(uButtonStyleState);

			// If the button is hidden or a separator, ignore it.
			if (((wButtonState & TBSTATE_HIDDEN) == 0)
			 && (wButtonStyle != TBSTYLE_SEP))
			{
				// Set some state variables.
				BOOL fButtonPressed ((wButtonState & TBSTATE_PRESSED) != 0);
				BOOL fButtonChecked ((wButtonState & TBSTATE_CHECKED) != 0);
				BOOL fButtonEnabled = GetToolBarCtrl().IsButtonEnabled((int)uButtonID);

				POINT ptCursor;
				::GetCursorPos(&ptCursor);
				ScreenToClient(&ptCursor);
				BOOL fButtonUnderCursor = IsActive() && crButton.PtInRect(ptCursor);

				if (fButtonUnderCursor)
				{
					// The cursor is over the current button.
					// Redraw it if necessary.
					nButtonUnderCursor = nButton;
					if (nButton != m_nLastButtonUnderCursor)
					{
						InvalidateRect(crButton, TRUE);
					}
				}
				else
				{
					// The cursor is not over the current button.
					// If this is the same button that the cursor
					// use to be over then redraw it.
					if (nButton == m_nLastButtonUnderCursor)
					{
						InvalidateRect(crButton, TRUE);
					}
				}
			}
		}

		// Remember the button underneath the cursor.
		m_nLastButtonUnderCursor = nButtonUnderCursor;
	}

	// If the user moves the mouse too fast, then it can leave the
	// toolbar without us noticing and the last button will not
	// be drawn. We use a timer to check if the mouse has left the
	// toolbar and the last button needs to be redrawn.
	KillTimer(m_uTimer);
	m_uTimer = SetTimer(1, 100, NULL);

	CToolBar::OnMouseMove(nFlags, point);
}

// A timer is used to handle the case when the mouse leaves the
// toolbar so quickly that we do not get a WM_MOUSEMOVE and the
// last button the mouse was over is not properly redrawn.
void CCustomToolBar::OnTimer(UINT nIDEvent) 
{
	// Make sure it's our timer.
	if (nIDEvent == m_uTimer)
	{
		// Check if a button needs to be redrawn. If we still have
		// the capture, then, by convention, behave as if the mouse
		// is still over the captured button.
		if ((CWnd::GetCapture() != this)
		 && (m_nLastButtonUnderCursor >= 0))
		{
			// Redraw the button if the mouse has moved outside of our window.
			// If the mouse is still in our window, we should handle this
			// on a WM_MOUSEMOVE message.
			CRect crWindow;
			GetWindowRect(crWindow);

			POINT ptCursor;
			::GetCursorPos(&ptCursor);
			if (!crWindow.PtInRect(ptCursor))
			{
				// Redraw the button.
				CRect crButton;
				GetItemRect(m_nLastButtonUnderCursor, crButton);
				InvalidateRect(crButton, TRUE);

				// Clear the associated state variables.
				m_nLastButtonUnderCursor = -1;

			}
		}

		// If we no longer have an active button, then we no longer
		// need a timer.
		if (m_nLastButtonUnderCursor == -1)
		{
			KillTimer(m_uTimer);
			m_uTimer = 0;
		}
	}

	CToolBar::OnTimer(nIDEvent);
}

// The toolbar is being destroyed. Free any timer resource that was allocated.
void CCustomToolBar::OnDestroy() 
{
	KillTimer(m_uTimer);
	m_uTimer = 0;

	CToolBar::OnDestroy();
}

LRESULT CCustomToolBar::OnDisplayChange(WPARAM, LPARAM)
{
	return 0L;
}
