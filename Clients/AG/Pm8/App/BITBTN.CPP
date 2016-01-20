/*
// $Workfile: BITBTN.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/BITBTN.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 3     2/22/99 11:56a Jfleischhauer
// in Initialize(), added ASSERT and warning message if bitmap is not
// found by m_Properties.AsResourceIDFromString()
// 
// 2     2/18/99 4:46p Jfleischhauer
// changed AsString() call to AsResourceIDFromString() to obviate resource
// bug with string-identifiers
// 
//    Rev 1.0   14 Aug 1997 15:18:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:12   Fred
// Initial revision.
// 
//    Rev 1.16   15 Jul 1997 10:38:58   dennisp
// Changed behavior of drawing shadows to match windows.
// 
//    Rev 1.15   22 Apr 1997 13:16:54   Fred
// Start of project and art browsers
// 
//    Rev 1.14   08 Aug 1996 17:30:24   Fred
// More 16-bit changes
// 
//    Rev 1.13   03 Aug 1996 16:56:40   Fred
//  
// 
//    Rev 1.12   03 Aug 1996 16:42:50   Fred
//  
// 
//    Rev 1.11   02 Aug 1996 15:40:52   Fred
//  
// 
//    Rev 1.10   02 Aug 1996 15:02:20   Fred
// 16-bit changes
// 
//    Rev 1.9   02 Aug 1996 13:10:50   Fred
// More 16-bit changes
// 
//    Rev 1.8   01 Aug 1996 14:18:36   FRED
// SuperWndProc
// 
//    Rev 1.7   01 Aug 1996 09:22:34   Fred
// More 16-bit changes
// 
//    Rev 1.6   31 Jul 1996 11:56:24   Fred
// 16-bit compatibility
// 
//    Rev 1.5   28 Jul 1996 13:33:34   Fred
// Disabling button pops it up
// 
//    Rev 1.4   28 Jul 1996 07:46:54   Fred
// More album control stuff
// 
//    Rev 1.3   26 Jul 1996 17:42:30   Fred
//  
// 
//    Rev 1.2   26 Jul 1996 13:27:06   Fred
// CMlsSimpleBitmapButton
// 
//    Rev 1.1   05 Jun 1996 08:49:26   Fred
// Draws focus rect on selected button
// 
//    Rev 1.0   03 Jun 1996 08:44:12   Fred
//  
*/ 

#include "stdafx.h"
#include "util.h"
#include "bitbtn.h"

#ifndef WIN32
#ifndef COLOR_BTNHILIGHT
#define	COLOR_BTNHILIGHT	COLOR_BTNHIGHLIGHT
#endif
#endif

#define BITBTN_COLOR_OUTER_SHADOW   RGB(0, 0, 0)

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMlsBitmapButton

WNDPROC CMlsBitmapButton::m_pfnSuperWndProc = NULL;

CMlsBitmapButton::CMlsBitmapButton()
{
	m_fHaveBitmap = FALSE;

	m_clFace = GetSysColor(COLOR_BTNFACE);
	m_clText = GetSysColor(COLOR_BTNTEXT);
	m_clHilight = GetSysColor(COLOR_BTNHILIGHT);
	m_clShadow = GetSysColor(COLOR_BTNSHADOW);
	m_pPalette = NULL;
}

CMlsBitmapButton::~CMlsBitmapButton()
{
}

BEGIN_MESSAGE_MAP(CMlsBitmapButton, CButton)
	//{{AFX_MSG_MAP(CMlsBitmapButton)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_PAINT()
	ON_MESSAGE(BM_SETSTATE, OnSetState)
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMlsBitmapButton::Initialize(UINT nID, CWnd* pParent)
{
	CString csText;
//	CString csBitmap;
	int nBitmapID;
	CString csStyle;
	CDC* pDC = NULL;
	CDC dcBitmap;
	CBitmap* pOldBitmap = NULL;
	CRect crBitmap;
	CDC dcMask;
	CBitmap* pOldMask = NULL;
	CRect crMask;

	TRY
	{
		// Subclass the dialog item.
		if (SubclassDlgItem(nID, pParent))
		{
			// Get the control's text and extract the properties.
			GetWindowText(csText);
			m_Properties.ExtractProperties(csText);
			SetWindowText(csText);

#if 0 //- the old way:
			m_Properties.AsString(0, csBitmap);
			if (!csBitmap.IsEmpty())
			{
				m_bmBitmap.LoadBitmap(csBitmap);
#endif
			//	Note: had to change the strategy for loading these bitmaps
			//	because LoadBitmap() fails under Win95/98 with VC6 when
			//	the resource file is bloated.  For more info, see PROPSTR.CPP.
			//												- jdf - 2/18/99
			if( ! m_Properties.AsResourceIDFromString(0, &nBitmapID) )
			{
				ASSERT( FALSE );
				// this is a bit unkosher, but I put this in here to help
				// QA trap missing resources in the release build
				AfxMessageBox( "There is a missing bitmap resource for one of the buttons in this window.\n\nThis will not hinder behavior of the program.\n\nPlease report this to Mindscape programmers." );
			}
			else
			{
				m_bmBitmap.LoadBitmap( MAKEINTRESOURCE(nBitmapID) );

				// Extract some information about the bitmap.
				BITMAP Bitmap;
				if (m_bmBitmap.GetObject(sizeof(Bitmap), &Bitmap) != 0)
				{
					crBitmap.SetRect(0, 0, Bitmap.bmWidth, Bitmap.bmHeight);
					m_nBitmapWidth = crBitmap.Width();
					m_nBitmapHeight = crBitmap.Height();

					// Get the DC for this control.
					pDC = GetDC();

					// Get the transparent color indicated by the pixel at (0,Height-1).
					dcBitmap.CreateCompatibleDC(pDC);
					pOldBitmap = dcBitmap.SelectObject(&m_bmBitmap);
					COLORREF clTransparent = dcBitmap.GetPixel(0, m_nBitmapHeight-1);

					// Create the mask bitmap with 1's (white) for pixels that match the
					// transparent color and 0's (black) for pixels that are not transparent.
					Util::CreateMask(dcMask, m_bmMask, crMask, dcBitmap, crBitmap, clTransparent, TRUE);
					
					if ((pOldMask = dcMask.SelectObject(&m_bmMask)) != NULL)
					{
						// Set all the transparent pixels in the bitmap to 0's (black).
						dcBitmap.SetTextColor(RGB(255,255,255));	// 0's in mask (opaque) go to 1's
						dcBitmap.SetBkColor(RGB(0,0,0));				// 1's in mask (transparent) go to 0's
						dcBitmap.BitBlt(0, 0, m_nBitmapWidth, m_nBitmapHeight, &dcMask, 0, 0, SRCAND);

						m_fHaveBitmap = TRUE;
					}
				}
			}

			// Unpack the button's style.
			m_Properties.AsString(1, csStyle, "", TRUE);
			UnpackAlignment(csStyle);
		}
	}
	END_TRY
	
	if (pDC != NULL)
	{
		ReleaseDC(pDC);
		pDC = NULL;
	}
	
	if (pOldBitmap != NULL)
	{
		dcBitmap.SelectObject(pOldBitmap);
		pOldBitmap = NULL;
	}
	
	if (pOldMask != NULL)
	{
		dcMask.SelectObject(pOldMask);
		pOldMask = NULL;
	}
}

void CMlsBitmapButton::UnpackAlignment(const CString& csStyle)
{
	// Unpack the major alignment type (L, R, T, or B).
	     if (csStyle.Find('L') != -1) m_Alignment = Left;
	else if (csStyle.Find('R') != -1) m_Alignment = Right;
	else if (csStyle.Find('T') != -1) m_Alignment = Top;
	else if (csStyle.Find('B') != -1) m_Alignment = Bottom;
	else m_Alignment = Left;

	// Unpack the alignment variation (0, 1, 2, or 3).
	     if (csStyle.Find('0') != -1) m_nVariation = 0;
	else if (csStyle.Find('1') != -1) m_nVariation = 1;
	else if (csStyle.Find('2') != -1) m_nVariation = 2;
	else if (csStyle.Find('3') != -1) m_nVariation = 3;
	else m_nVariation = 3;

	// Unpack the multiple line flag.
	     if (csStyle.Find('S') != -1) m_fMultipleLine = FALSE;
	else if (csStyle.Find('M') != -1) m_fMultipleLine = TRUE;
	else m_fMultipleLine = FALSE;

	// Unpack the word wrap flag.
	     if (csStyle.Find('N') != -1) m_fWordWrap = FALSE;
	else if (csStyle.Find('W') != -1) m_fWordWrap = TRUE;
	else m_fWordWrap = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CMlsBitmapButton message handlers

// Owner draw buttons can be double clicked. We turn this into a single click since
// we want to simulate the behavior of a regular button.

void CMlsBitmapButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	SendMessage(WM_LBUTTONDOWN, (WPARAM)nFlags, (LPARAM)(*((DWORD*)(&point))));
}

// OnPaint() is called when the button needs to be repainted.

void CMlsBitmapButton::OnPaint() 
{
	CPaintDC dc(this);
	DrawButton(&dc);
}

// DrawButton() is the main function for drawing a button.

void CMlsBitmapButton::DrawButton(CDC* pDC)
{
	// Upack the state of the button.
	DWORD dwState = SendMessage(BM_GETSTATE, 0, 0);
	DWORD dwCode = SendMessage(WM_GETDLGCODE, 0, 0);
	BOOL fSelected = (dwState & 4) != 0;
	BOOL fFocused = (dwState & 8) != 0;
	BOOL fDisabled = !IsWindowEnabled();
	BOOL fDefault = (dwCode & DLGC_DEFPUSHBUTTON) != 0;

	// Create a bitmap which will be used to draw the button image.
	// When the bitmap is complete, it will be drawn onto the button.
	CRect crButton;
	GetClientRect(crButton);

	CBitmap bmButton;
	bmButton.CreateCompatibleBitmap(pDC, crButton.Width(), crButton.Height());

	CDC dcButton;
	dcButton.CreateCompatibleDC(pDC);

	CPalette* pOldPalette = NULL;
	CPalette* pOldButtonPalette = NULL;

	if (m_pPalette != NULL)
	{
		pDC->SelectPalette(m_pPalette, FALSE);
		pDC->RealizePalette();
		pOldButtonPalette = dcButton.SelectPalette(m_pPalette, FALSE);
	}

	dcButton.SelectObject(&bmButton);

	// Initialize the button bitmap to the button face color.
	Util::FillRectangleWithColor(dcButton, crButton, m_clFace);

	// Compute the area available for content.
	CRect crContent(crButton);
	crContent.InflateRect(-MinimumMargin, -MinimumMargin);
	CRect crTextContent(crContent);

	// Compute the bitmap dimensions and its nominal position. Adjust crTextContent to the
	// area which is available next to the bitmap.
	CRect crBitmap;
	crBitmap.SetRectEmpty();

	if (!crContent.IsRectEmpty())
	{
		if (m_fHaveBitmap)
		{
			crBitmap.SetRect(0, 0, m_nBitmapWidth, m_nBitmapHeight);

			// Set first bitmap position base on major alignment type.
			switch (m_Alignment)
			{
				case Left:
				{
					crBitmap.OffsetRect(crContent.left, crContent.top+(crContent.Height()-crBitmap.Height())/2);
					crTextContent.left += crBitmap.Width()+Separator;
					break;
				}
				case Right:
				{
					crBitmap.OffsetRect(crContent.right-crBitmap.Width(), crContent.top+(crContent.Height()-crBitmap.Height())/2);
					crTextContent.right -= crBitmap.Width()+Separator;
					break;
				}
				case Top:
				{
					crBitmap.OffsetRect(crContent.left+(crContent.Width()-crBitmap.Width())/2, crContent.top);
					crTextContent.top += crBitmap.Height()+Separator;
					break;
				}
				case Bottom:
				{
					crBitmap.OffsetRect(crContent.left+(crContent.Width()-crBitmap.Width())/2, crContent.bottom-crBitmap.Height());
					crTextContent.bottom -= crBitmap.Height()+Separator;
					break;
				}
				default:
				{
					ASSERT(FALSE);
				}
			}
		}
		else
		{
			crBitmap.SetRectEmpty();
		}
	}

	// Compute the text dimensions and its nominal position.
	CRect crText;
	crText.SetRectEmpty();
	CString csText;

	if (!crTextContent.IsRectEmpty())
	{
		GetWindowText(csText);
		if (!csText.IsEmpty())
		{
			// Get the font which we are using for the text.
			CFont* pFont = GetFont();
			if (pFont != NULL)
			{
				// Select the font we are going to use for drawing the text.
				dcButton.SelectObject(pFont);

				// Compute the extent of the text. The text is formatted into the area next to
				// the button. If the text is multiple line, the full width is always used.
				crText.SetRect(0, 0, crTextContent.Width(), crTextContent.Height());
				int nTextHeight = Util::DrawText(&dcButton, csText, crText, DT_CALCRECT | (m_fMultipleLine ? (m_fWordWrap ? DT_WORDBREAK : 0) : DT_SINGLELINE));

				// Build the rectangle that has the dimensions of the text.
				crText.bottom = nTextHeight;

				// Clip to content dimensions.
				if (crText.Width() > crTextContent.Width())
				{
					crText.right = crTextContent.Width();
				}
				if (crText.Height() > crTextContent.Height())
				{
					crText.bottom = crTextContent.Height();
				}

				// Adjust text rectangle within content rectangle.
				int nXOffset = 0;
				int nYOffset = 0;

				switch (m_Alignment)
				{
					case Left:
					case Right:
					{
						switch (m_nVariation)
						{
							case 0: nXOffset = 0;                                        break;
							case 1: nXOffset = (crTextContent.Width()-crText.Width())/2; break;
							case 2: nXOffset = (crTextContent.Width()-crText.Width());   break;
							case 3: nXOffset = 0;                                        break;
						}
						if (m_Alignment == Right)
						{
							nXOffset = (crTextContent.Width()-crText.Width())-nXOffset;
						}
						nYOffset = (crTextContent.Height()-crText.Height())/2;
						break;
					}
					case Top:
					case Bottom:
					{
						switch (m_nVariation)
						{
							case 0: nYOffset = 0;                                          break;
							case 1: nYOffset = (crTextContent.Height()-crText.Height())/2; break;
							case 2: nYOffset = (crTextContent.Height()-crText.Height());   break;
							case 3: nYOffset = 0;                                          break;
						}
						if (m_Alignment == Bottom)
						{
							nYOffset = (crTextContent.Height()-crText.Height())-nYOffset;
						}
						nXOffset = (crTextContent.Width()-crText.Width())/2;
						break;
					}
					default:
					{
						ASSERT(FALSE);
					}
				}

				crText.OffsetRect(crTextContent.left+nXOffset, crTextContent.top+nYOffset);
			}
		}
	}

	// If the layout variation is 3, then center both the bitmap and text within the
	// content rectangle.
	if (!crContent.IsRectEmpty())
	{
		if (m_nVariation == 3)
		{
			// Compute the rectangle which encloses both the bitmap and text.
			CRect crBoth;
			crBoth.SetRectEmpty();
			if (!crBitmap.IsRectEmpty())
			{
				crBoth |= crBitmap;
			}
			if (!crText.IsRectEmpty())
			{
				crBoth |= crText;
			}

			// Move the bitmap and text back to the content rectangle.
			int nXOffset = 0;
			int nYOffset = 0;

			switch (m_Alignment)
			{
				case Left:
				case Right:
				{
					nXOffset = (crContent.Width()-crBoth.Width())/2;
					if (m_Alignment == Right)
					{
						nXOffset = -nXOffset;
					}
					break;
				}
				case Top:
				case Bottom:
				{
					nYOffset = (crContent.Height()-crBoth.Height())/2;
					if (m_Alignment == Bottom)
					{
						nYOffset = -nYOffset;
					}
					break;
				}
				default:
				{
					ASSERT(FALSE);
				}
			}

			// Adjust the bitmap and text rectangles.
			crBitmap.OffsetRect(nXOffset, nYOffset);
			crText.OffsetRect(nXOffset, nYOffset);
		}
	}

	// If the button is selected, offset the bitmap and text.
	if (fSelected)
	{
		crBitmap.OffsetRect(1, 1);
		crText.OffsetRect(1, 1);
	}

	// Draw the bitmap onto the button image at the computed position.
	if (!crBitmap.IsRectEmpty())
	{
		CDC dcBitmap;
		dcBitmap.CreateCompatibleDC(pDC);

		// Erase to 0's (black) the areas of the button where the opaque portions
		// of the bitmap will be drawn.
		dcButton.SetTextColor(RGB(0,0,0));
		dcButton.SetBkColor(RGB(255,255,255));
		dcBitmap.SelectObject(&m_bmMask);
		dcButton.BitBlt(crBitmap.left, crBitmap.top, crBitmap.Width(), crBitmap.Height(), &dcBitmap, 0, 0, SRCAND);

		// Draw the bitmap onto the button.
		dcBitmap.SelectObject(&m_bmBitmap);
		dcButton.BitBlt(crBitmap.left, crBitmap.top, crBitmap.Width(), crBitmap.Height(), &dcBitmap, 0, 0, SRCPAINT);
	}

	// Draw the text onto the button image at the computed position.
	if (!crText.IsRectEmpty())
	{
		// Draw the text.
		dcButton.SetTextColor(m_clText);
		dcButton.SetBkMode(TRANSPARENT);
		Util::DrawText(&dcButton, csText, crText, (m_fMultipleLine ? (DT_CENTER | (m_fWordWrap ? DT_WORDBREAK : 0)) : DT_SINGLELINE));
	}

	// If the button is disabled, indicate this by embossing the bitmap.
	if (fDisabled)
	{
		Util::Emboss(dcButton, crButton, m_clFace, m_clHilight, m_clShadow, m_clHilight);
	}

	// Draw the button in its current state.
	if (fSelected)
	{
		// Button is selected, draw a black border with a shadow border inside.
		CRect crDraw(crButton);
		Util::DrawBorderWithColor(dcButton, crDraw, m_clText, 1);
		crDraw.InflateRect(-1, -1);
		Util::DrawBorderWithColor(dcButton, crDraw, m_clShadow, 1);
	}
	else
	{
		CRect crDraw(crButton);

		// If the button is focused or the default button, draw a
		// black border around it. The rest of the image moves in
		// to make room.
		if (fFocused | fDefault)
		{
			Util::DrawBorderWithColor(dcButton, crDraw, m_clText, 1);
			crDraw.InflateRect(-1, -1);
		}

		// Draw the raised 3D border:
		//
		//    W-----------WB
		//    |           D|
		//    |           ||
		//    |           ||
		//    WD----------D|
		//    B------------B


		CRect crSide;

		// W horizontal
		crSide.SetRect(crDraw.left, crDraw.top, crDraw.right-1, crDraw.top+1);
		Util::FillRectangleWithColor(dcButton, crSide, m_clHilight);

		// W vertical
		crSide.SetRect(crDraw.left, crDraw.top+1, crDraw.left+1, crDraw.bottom-1);
		Util::FillRectangleWithColor(dcButton, crSide, m_clHilight);

		// B horizontal
		crSide.SetRect(crDraw.left, crDraw.bottom-1, crDraw.right, crDraw.bottom);
		Util::FillRectangleWithColor(dcButton, crSide, BITBTN_COLOR_OUTER_SHADOW);

		// B vertical
		crSide.SetRect(crDraw.right-1, crDraw.top, crDraw.right, crDraw.bottom-1);
		Util::FillRectangleWithColor(dcButton, crSide, BITBTN_COLOR_OUTER_SHADOW);

		// D horizontal
		crSide.SetRect(crDraw.left+1, crDraw.bottom-2, crDraw.right-1, crDraw.bottom-1);
		Util::FillRectangleWithColor(dcButton, crSide, m_clShadow);

		// D vertical
		crSide.SetRect(crDraw.right-2, crDraw.top+1, crDraw.right-1, crDraw.bottom-2);
		Util::FillRectangleWithColor(dcButton, crSide, m_clShadow);
	}

	// If the button is focused, draw the focus rectangle.
	if (fFocused)
	{
		CRect crDraw(crButton);
		crDraw.InflateRect(-4, -4);
		dcButton.DrawFocusRect(crDraw);
	}

	// Copy the bitmap onto the button.
	pDC->BitBlt(crButton.left, crButton.top, crButton.Width(), crButton.Height(), &dcButton, 0, 0, SRCCOPY);

	if (pOldPalette != NULL)
	{
		pDC->SelectPalette(pOldPalette, FALSE);
	}

	if (pOldButtonPalette != NULL)
	{
		pDC->SelectPalette(pOldButtonPalette, FALSE);
	}

	dcButton.DeleteDC();
	bmButton.DeleteObject();
}

// OnSetState() is called when a BM_SETSTATE message is received by the button.
// This message is sent to the button to raise it up or down, which happens to be
// the time when the normal button class does its internal redraw without sending
// WM_PAINT. To defeat this behavior and have make sure the button calls us to
// draw itself, so we bracket the Default() call with calls to SetRedraw() and
// then call Invalidate() to force a WM_PAINT.

LRESULT CMlsBitmapButton::OnSetState(WPARAM wParam, LPARAM lParam)
{
	// If we're changing states, do it and redraw.
	if ((BOOL)((GetState() & 4) != 0) != (BOOL)wParam)
	{
		SetRedraw(FALSE);
		Default();
		SetRedraw(TRUE);
		Invalidate();
	}

	return 0;
}

// When the button is enabled or disabled it will need to be redrawn.
// Normally, the button class draws the button itself without sending
// a WM_PAINT message. To defeat this behavior, we bracket the call to
// CButton:OnEnable() with calls to SetRedraw() and then invalidate the
// button to force a WM_PAINT. Note that setting the style to BS_OWNERDRAW
// does not work because the default button processing is not handled
// properly.

void CMlsBitmapButton::OnEnable(BOOL bEnable) 
{
	// WM_ENABLE is send AFTER the WS_DISABLED bit is set.
	if (bEnable == IsWindowEnabled())
	{
		SetRedraw(FALSE);
		INHERITED::OnEnable(bEnable);
		SetRedraw(TRUE);
		Invalidate();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMlsSimpleBitmapButton

CMlsSimpleBitmapButton::CMlsSimpleBitmapButton()
{
	m_hFont = NULL;
	m_fHaveBitmap = FALSE;
	m_fSelect = FALSE;
}

CMlsSimpleBitmapButton::~CMlsSimpleBitmapButton()
{
	DeleteFont();
}

void CMlsSimpleBitmapButton::DeleteFont(void)
{
	if (m_hFont != NULL)
	{
		DeleteObject(m_hFont);
		m_hFont = NULL;
	}
}

BEGIN_MESSAGE_MAP(CMlsSimpleBitmapButton, CWnd)
	//{{AFX_MSG_MAP(CMlsSimpleBitmapButton)
	ON_WM_CREATE()
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_MESSAGE(WM_GETTEXT, OnGetText)
	ON_MESSAGE(WM_GETTEXTLENGTH, OnGetTextLength)
	ON_MESSAGE(WM_GETFONT, OnGetFont)
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMlsSimpleBitmapButton::Initialize(UINT nID, CWnd* pParent)
{
	CString csText;
	TRY
	{
		CWnd* pControl = pParent->GetDlgItem(nID);
		if (pControl != NULL)
		{
			CRect crControl;
			pControl->GetWindowRect(crControl);
			pParent->ScreenToClient(crControl);
			pControl->GetWindowText(csText);
			Create(NULL, csText, pControl->GetStyle(), crControl, pParent, nID, NULL);
			SetWindowPos(pControl, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
			SetFont(pControl->GetFont());
			pControl->DestroyWindow();
		}
	}
	END_TRY
}

void CMlsSimpleBitmapButton::UnpackProperties(void)
{
	CString csText;
	CString csBitmap;
	CString csStyle;
	CDC* pDC = NULL;
	CDC dcBitmap;
	CBitmap* pOldBitmap = NULL;
	CRect crBitmap;
	CDC dcMask;
	CBitmap* pOldMask = NULL;
	CRect crMask;

	TRY
	{
		// Extract the properties.
		GetWindowText(csText);
		m_Properties.ExtractProperties(csText);
		SetWindowText(csText);

		// Get the bitmap name.
		m_Properties.AsString(0, csBitmap);
		if (!csBitmap.IsEmpty())
		{
			// Load the bitmap.
			m_bmBitmap.LoadBitmap(csBitmap);
	
			// Extract some information about the bitmap.
			BITMAP Bitmap;
			if (Util::GetBitmap(&m_bmBitmap, &Bitmap))
			{
				crBitmap.SetRect(0, 0, Bitmap.bmWidth, Bitmap.bmHeight);
				m_nBitmapWidth = crBitmap.Width();
				m_nBitmapHeight = crBitmap.Height();
	
				// Get the DC for this control.
				pDC = GetDC();
	
				// Get the transparent color is indicated by the pixel at (0,Height-1).
				dcBitmap.CreateCompatibleDC(pDC);
				dcBitmap.SelectObject(&m_bmBitmap);
				COLORREF clTransparent = dcBitmap.GetPixel(0, m_nBitmapHeight-1);
	
				// Create the mask bitmap with 1's (white) for pixels that match the
				// transparent color and 0's (black) for pixels that are not transparent.
				Util::CreateMask(dcMask, m_bmMask, crMask, dcBitmap, crBitmap, clTransparent, TRUE);
						
				if ((pOldMask = dcMask.SelectObject(&m_bmMask)) != NULL)
				{
					// Set all the transparent pixels in the bitmap to 0's (black).
					dcBitmap.SetTextColor(RGB(255,255,255));	// 0's in mask (opaque) go to 1's
					dcBitmap.SetBkColor(RGB(0,0,0));				// 1's in mask (transparent) go to 0's
					dcBitmap.BitBlt(0, 0, m_nBitmapWidth, m_nBitmapHeight, &dcMask, 0, 0, SRCAND);
		
					m_fHaveBitmap = TRUE;
				}
			}
		}
	
		// Unpack the button's style.
		m_Properties.AsString(1, csStyle, "", TRUE);
		UnpackAlignment(csStyle);
	}
	END_TRY
	
	// Clean up.
	if (pDC != NULL)
	{
		ReleaseDC(pDC);
		pDC = NULL;
	}
	if (pOldBitmap != NULL)
	{
		dcBitmap.SelectObject(pOldBitmap);
		pOldBitmap = NULL;
	}
	if (pOldMask != NULL)
	{
		dcMask.SelectObject(pOldMask);
		pOldMask = NULL;
	}
}

void CMlsSimpleBitmapButton::UnpackAlignment(const CString& csStyle)
{
	// Unpack the major alignment type (L, R, T, or B).
	     if (csStyle.Find('L') != -1) m_Alignment = Left;
	else if (csStyle.Find('R') != -1) m_Alignment = Right;
	else if (csStyle.Find('T') != -1) m_Alignment = Top;
	else if (csStyle.Find('B') != -1) m_Alignment = Bottom;
	else m_Alignment = Left;

	// Unpack the alignment variation (0, 1, 2, or 3).
	     if (csStyle.Find('0') != -1) m_nVariation = 0;
	else if (csStyle.Find('1') != -1) m_nVariation = 1;
	else if (csStyle.Find('2') != -1) m_nVariation = 2;
	else if (csStyle.Find('3') != -1) m_nVariation = 3;
	else m_nVariation = 3;

	// Unpack the multiple line flag.
	     if (csStyle.Find('S') != -1) m_fMultipleLine = FALSE;
	else if (csStyle.Find('M') != -1) m_fMultipleLine = TRUE;
	else m_fMultipleLine = FALSE;

	// Unpack the word wrap flag.
	     if (csStyle.Find('N') != -1) m_fWordWrap = FALSE;
	else if (csStyle.Find('W') != -1) m_fWordWrap = TRUE;
	else m_fWordWrap = FALSE;
}

void CMlsSimpleBitmapButton::SetSelect(BOOL fSelect /*=TRUE*/)
{
	if (m_fSelect != fSelect)
	{
		// Selection changed.
		m_fSelect = fSelect;
		Invalidate();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMlsSimpleBitmapButton message handlers

int CMlsSimpleBitmapButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (INHERITED::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	// Get the control's text and extract the properties.
	SetWindowText(lpCreateStruct->lpszName);
	UnpackProperties();

	return 0;
}

LRESULT CMlsSimpleBitmapButton::OnSetText(WPARAM wParam, LPARAM lParam)
{
	BOOL fResult = FALSE;
	TRY
	{
		if (strcmp(m_csText, (LPCSTR)lParam) != 0)
		{
			m_csText = (LPCSTR)lParam;
			Invalidate();
		}
		fResult = TRUE;
	}
	END_TRY
	return (LRESULT)fResult;
}

LRESULT CMlsSimpleBitmapButton::OnGetText(WPARAM wParam, LPARAM lParam)
{
	int nCount = (int)wParam;
	if (nCount > 0)
	{
		LPSTR pszBuffer = (LPSTR)lParam;

		nCount--;
		if (nCount > m_csText.GetLength())
		{
			nCount = m_csText.GetLength();
		}

		if (nCount > 0)
		{
			strncpy(pszBuffer, m_csText, nCount);
		}

		pszBuffer[nCount] = '\0';
		nCount++;
	}

	return nCount;
}

LRESULT CMlsSimpleBitmapButton::OnGetTextLength(WPARAM wParam, LPARAM lParam)
{
	return m_csText.GetLength();
}

LRESULT CMlsSimpleBitmapButton::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	HFONT hNewFont = NULL;
	if (wParam != 0)
	{
		LOGFONT LogFont;
		if (::GetObject((HFONT)wParam, sizeof(LogFont), &LogFont) != 0)
		{
			hNewFont = CreateFontIndirect(&LogFont);
		}
	}
		
	DeleteFont();
	m_hFont = hNewFont;
	
	if (LOWORD(lParam) != 0)
	{
		Invalidate();
	}
	
	return 0;
}

LRESULT CMlsSimpleBitmapButton::OnGetFont(WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)(UINT)m_hFont;
}

void CMlsSimpleBitmapButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Set the state to selected and capture the mouse until the button is released.
	SetSelect(TRUE);
	SetCapture();
}

void CMlsSimpleBitmapButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// Remember the select state before losing it.
	BOOL fSelect = m_fSelect;

	// The user finally let go, pop the button back up.
	SetSelect(FALSE);
	ReleaseCapture();

	if (fSelect)
	{
		// Let our owner know that the button was pressed.
		CWnd* pOwner = GetOwner();
		if (pOwner != NULL)
		{
			Util::SendWmCommand(pOwner, GetDlgCtrlID(), this, 0);
		}
	}
}

void CMlsSimpleBitmapButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// Default behavior turns double clicks into single clicks.
	SendMessage(WM_LBUTTONDOWN, (WPARAM)nFlags, MAKELPARAM(point.x, point.y));
}

void CMlsSimpleBitmapButton::OnMouseMove(UINT nFlags, CPoint point)
{
	// The user is moving the mouse around willy-nilly. If the mouse is
	// moved off the button, pop it up until it's moved back on.
	if (GetCapture() == this)
	{
		CRect crClient;
		GetClientRect(crClient);
		SetSelect(crClient.PtInRect(point));
	}
}

void CMlsSimpleBitmapButton::OnEnable(BOOL bEnable) 
{
	// WM_ENABLE is sent AFTER the WS_DISABLED bit is set.
	if (bEnable == IsWindowEnabled())
	{
		// If button is being disabled, pop it back out.
		if (!bEnable)
		{
			SetSelect(FALSE);
			ReleaseCapture();
		}

		Default();
		Invalidate();
	}
}

// OnPaint() is called when the button needs to be repainted.

void CMlsSimpleBitmapButton::OnPaint() 
{
	CPaintDC dc(this);
	DrawButton(&dc);
}

// DrawButton() is the main function for drawing a button.

void CMlsSimpleBitmapButton::DrawButton(CDC* pDC)
{
	// Upack the state of the button.
	BOOL fSelected = GetSelect();
	BOOL fFocused = FALSE;
	BOOL fDisabled = !IsWindowEnabled();
	BOOL fDefault = FALSE;

	// Create a bitmap which will be used to draw the button image.
	// When the bitmap is complete, it will be drawn onto the button.
	CRect crButton;
	GetClientRect(crButton);

	CBitmap bmButton;
	bmButton.CreateCompatibleBitmap(pDC, crButton.Width(), crButton.Height());

	CDC dcButton;
	dcButton.CreateCompatibleDC(pDC);

	dcButton.SelectObject(&bmButton);

	// Initialize the button bitmap to the button face color.
	Util::FillRectangleWithColor(dcButton, crButton, GetSysColor(COLOR_BTNFACE));

	// Compute the area available for content.
	CRect crContent(crButton);
	crContent.InflateRect(-MinimumMargin, -MinimumMargin);
	CRect crTextContent(crContent);

	// Compute the bitmap dimensions and its nominal position. Adjust crTextContent to the
	// area which is available next to the bitmap.
	CRect crBitmap;
	crBitmap.SetRectEmpty();

	if (!crContent.IsRectEmpty())
	{
		if (m_fHaveBitmap)
		{
			crBitmap.SetRect(0, 0, m_nBitmapWidth, m_nBitmapHeight);

			// Set first bitmap position base on major alignment type.
			switch (m_Alignment)
			{
				case Left:
				{
					crBitmap.OffsetRect(crContent.left, crContent.top+(crContent.Height()-crBitmap.Height())/2);
					crTextContent.left += crBitmap.Width()+Separator;
					break;
				}
				case Right:
				{
					crBitmap.OffsetRect(crContent.right-crBitmap.Width(), crContent.top+(crContent.Height()-crBitmap.Height())/2);
					crTextContent.right -= crBitmap.Width()+Separator;
					break;
				}
				case Top:
				{
					crBitmap.OffsetRect(crContent.left+(crContent.Width()-crBitmap.Width())/2, crContent.top);
					crTextContent.top += crBitmap.Height()+Separator;
					break;
				}
				case Bottom:
				{
					crBitmap.OffsetRect(crContent.left+(crContent.Width()-crBitmap.Width())/2, crContent.bottom-crBitmap.Height());
					crTextContent.bottom -= crBitmap.Height()+Separator;
					break;
				}
				default:
				{
					ASSERT(FALSE);
				}
			}
		}
		else
		{
			crBitmap.SetRectEmpty();
		}
	}

	// Compute the text dimensions and its nominal position.
	CRect crText;
	crText.SetRectEmpty();
	CString csText;

	if (!crTextContent.IsRectEmpty())
	{
		GetWindowText(csText);
		if (!csText.IsEmpty())
		{
			// Get the font which we are using for the text.
			CFont* pFont = GetFont();
			if (pFont != NULL)
			{
				// Select the font we are going to use for drawing the text.
				dcButton.SelectObject(pFont);

				// Compute the extent of the text. The text is formatted into the area next to
				// the button. If the text is multiple line, the full width is always used.
				crText.SetRect(0, 0, crTextContent.Width(), crTextContent.Height());
				int nTextHeight = Util::DrawText(&dcButton, csText, crText, DT_CALCRECT | (m_fMultipleLine ? (m_fWordWrap ? DT_WORDBREAK : 0) : DT_SINGLELINE));

				// Build the rectangle that has the dimensions of the text.
				crText.bottom = nTextHeight;

				// Clip to content dimensions.
				if (crText.Width() > crTextContent.Width())
				{
					crText.right = crTextContent.Width();
				}
				if (crText.Height() > crTextContent.Height())
				{
					crText.bottom = crTextContent.Height();
				}

				// Adjust text rectangle within content rectangle.
				int nXOffset = 0;
				int nYOffset = 0;

				switch (m_Alignment)
				{
					case Left:
					case Right:
					{
						switch (m_nVariation)
						{
							case 0: nXOffset = 0;                                        break;
							case 1: nXOffset = (crTextContent.Width()-crText.Width())/2; break;
							case 2: nXOffset = (crTextContent.Width()-crText.Width());   break;
							case 3: nXOffset = 0;                                        break;
						}
						if (m_Alignment == Right)
						{
							nXOffset = (crTextContent.Width()-crText.Width())-nXOffset;
						}
						nYOffset = (crTextContent.Height()-crText.Height())/2;
						break;
					}
					case Top:
					case Bottom:
					{
						switch (m_nVariation)
						{
							case 0: nYOffset = 0;                                          break;
							case 1: nYOffset = (crTextContent.Height()-crText.Height())/2; break;
							case 2: nYOffset = (crTextContent.Height()-crText.Height());   break;
							case 3: nYOffset = 0;                                          break;
						}
						if (m_Alignment == Bottom)
						{
							nYOffset = (crTextContent.Height()-crText.Height())-nYOffset;
						}
						nXOffset = (crTextContent.Width()-crText.Width())/2;
						break;
					}
					default:
					{
						ASSERT(FALSE);
					}
				}

				crText.OffsetRect(crTextContent.left+nXOffset, crTextContent.top+nYOffset);
			}
		}
	}

	// If the layout variation is 3, then center both the bitmap and text within the
	// content rectangle.
	if (!crContent.IsRectEmpty())
	{
		if (m_nVariation == 3)
		{
			// Compute the rectangle which encloses both the bitmap and text.
			CRect crBoth;
			crBoth.SetRectEmpty();
			if (!crBitmap.IsRectEmpty())
			{
				crBoth |= crBitmap;
			}
			if (!crText.IsRectEmpty())
			{
				crBoth |= crText;
			}

			// Move the bitmap and text back to the content rectangle.
			int nXOffset = 0;
			int nYOffset = 0;

			switch (m_Alignment)
			{
				case Left:
				case Right:
				{
					nXOffset = (crContent.Width()-crBoth.Width())/2;
					if (m_Alignment == Right)
					{
						nXOffset = -nXOffset;
					}
					break;
				}
				case Top:
				case Bottom:
				{
					nYOffset = (crContent.Height()-crBoth.Height())/2;
					if (m_Alignment == Bottom)
					{
						nYOffset = -nYOffset;
					}
					break;
				}
				default:
				{
					ASSERT(FALSE);
				}
			}

			// Adjust the bitmap and text rectangles.
			crBitmap.OffsetRect(nXOffset, nYOffset);
			crText.OffsetRect(nXOffset, nYOffset);
		}
	}

	// If the button is selected, offset the bitmap and text.
	if (fSelected)
	{
		crBitmap.OffsetRect(1, 1);
		crText.OffsetRect(1, 1);
	}

	// Draw the bitmap onto the button image at the computed position.
	if (!crBitmap.IsRectEmpty())
	{
		CDC dcBitmap;
		dcBitmap.CreateCompatibleDC(pDC);

		// Erase to 0's (black) the areas of the button where the opaque portions
		// of the bitmap will be drawn.
		dcButton.SetTextColor(RGB(0,0,0));
		dcButton.SetBkColor(RGB(255,255,255));
		dcBitmap.SelectObject(&m_bmMask);
		dcButton.BitBlt(crBitmap.left, crBitmap.top, crBitmap.Width(), crBitmap.Height(), &dcBitmap, 0, 0, SRCAND);

		// Draw the bitmap onto the button.
		dcBitmap.SelectObject(&m_bmBitmap);
		dcButton.BitBlt(crBitmap.left, crBitmap.top, crBitmap.Width(), crBitmap.Height(), &dcBitmap, 0, 0, SRCPAINT);
	}

	// Draw the text onto the button image at the computed position.
	if (!crText.IsRectEmpty())
	{
		// Draw the text.
		dcButton.SetTextColor(GetSysColor(COLOR_BTNTEXT));
		dcButton.SetBkMode(TRANSPARENT);
		Util::DrawText(&dcButton, csText, crText, (m_fMultipleLine ? (DT_CENTER | (m_fWordWrap ? DT_WORDBREAK : 0)) : DT_SINGLELINE));
	}

	// If the button is disabled, indicate this by embossing the bitmap.
	if (fDisabled)
	{
		Util::Emboss(dcButton, crButton, GetSysColor(COLOR_BTNFACE), GetSysColor(COLOR_BTNHILIGHT), GetSysColor(COLOR_BTNSHADOW), GetSysColor(COLOR_BTNHILIGHT));
	}

	// Draw the button in its current state.
	if (fSelected)
	{
		// Button is selected, draw a black border with a shadow border inside.
		CRect crDraw(crButton);
		Util::DrawBorderWithColor(dcButton, crDraw, GetSysColor(COLOR_BTNTEXT), 1);
		crDraw.InflateRect(-1, -1);
		Util::DrawBorderWithColor(dcButton, crDraw, GetSysColor(COLOR_BTNSHADOW), 1);
	}
	else
	{
		CRect crDraw(crButton);

		// If the button is focused or the default button, draw a
		// black border around it. The rest of the image moves in
		// to make room.
		if (fFocused | fDefault)
		{
			Util::DrawBorderWithColor(dcButton, crDraw, GetSysColor(COLOR_BTNTEXT), 1);
			crDraw.InflateRect(-1, -1);
		}

		// Draw the raised 3D border:
		//
		//    W-----------WB
		//    |           D|
		//    |           ||
		//    |           ||
		//    WD----------D|
		//    B------------B


		CRect crSide;

		// W horizontal
		crSide.SetRect(crDraw.left, crDraw.top, crDraw.right-1, crDraw.top+1);
		Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNHILIGHT));

		// W vertical
		crSide.SetRect(crDraw.left, crDraw.top+1, crDraw.left+1, crDraw.bottom-1);
		Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNHILIGHT));

		// B horizontal
		crSide.SetRect(crDraw.left, crDraw.bottom-1, crDraw.right, crDraw.bottom);
		Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNTEXT));

		// B vertical
		crSide.SetRect(crDraw.right-1, crDraw.top, crDraw.right, crDraw.bottom-1);
		Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNTEXT));

		// D horizontal
		crSide.SetRect(crDraw.left+1, crDraw.bottom-2, crDraw.right-1, crDraw.bottom-1);
		Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNSHADOW));

		// D vertical
		crSide.SetRect(crDraw.right-2, crDraw.top+1, crDraw.right-1, crDraw.bottom-2);
		Util::FillRectangleWithColor(dcButton, crSide, GetSysColor(COLOR_BTNSHADOW));
	}

	// If the button is focused, draw the focus rectangle.
	if (fFocused)
	{
		CRect crDraw(crButton);
		crDraw.InflateRect(-4, -4);
		dcButton.DrawFocusRect(crDraw);
	}

	// Copy the bitmap onto the button.
	pDC->BitBlt(crButton.left, crButton.top, crButton.Width(), crButton.Height(), &dcButton, 0, 0, SRCCOPY);

	dcButton.DeleteDC();
	bmButton.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
