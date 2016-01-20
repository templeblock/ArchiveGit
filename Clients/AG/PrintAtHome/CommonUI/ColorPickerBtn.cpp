#include "stdafx.h"
#include "ColorPickerBtn.h"

extern int g_ciArrowSizeX = 4;
extern int g_ciArrowSizeY = 2;

//////////////////////////////////////////////////////////////////////
CColorPickerBtn::CColorPickerBtn()
{
	bSetInvisible = false;
	m_clrToolBar = RGB(229, 228, 232);
	m_clrBorders = RGB(103, 103, 103);
}

//////////////////////////////////////////////////////////////////////
LRESULT CColorPickerBtn::OnDrawItem (UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) 
{
	LPDRAWITEMSTRUCT lpItem = (LPDRAWITEMSTRUCT) lParam;
	CDC dc (lpItem ->hDC);

	//
	// Get data about the request
	//

	UINT uState = lpItem ->itemState;
	CRect rcDraw = lpItem ->rcItem;

	//
	// give it toolbar bkgd color
	//
	::FillRect(dc.m_hDC, &rcDraw, ::CreateSolidBrush(m_clrToolBar));

	m_fPopupActive = false;
	{
		//
		// Draw the outer edge
		//

		UINT uFrameState = DFCS_FLAT | DFCS_ADJUSTRECT; //DFCS_BUTTONPUSH | DFCS_ADJUSTRECT;
		if ((uState & ODS_SELECTED) != 0 || m_fPopupActive)
			uFrameState |= DFCS_PUSHED;
		if ((uState & ODS_DISABLED) != 0)
			uFrameState |= DFCS_INACTIVE;

		//
		// Adjust the position if we are selected (gives a 3d look)
		//
		
		if ((uState & ODS_SELECTED) != 0 || m_fPopupActive)
			rcDraw .OffsetRect (1, 1);
	}

	//
	// Draw the arrow
	//

	{
		CRect rcArrow;
		rcArrow .left   = (rcDraw. right - 2) - g_ciArrowSizeX - ::GetSystemMetrics (SM_CXEDGE) / 2;
		rcArrow .top    = (rcDraw.bottom + rcDraw.top)/2 - g_ciArrowSizeY / 2;
		rcArrow .right  = rcArrow.left + g_ciArrowSizeX;
		rcArrow .bottom = (rcDraw .bottom + rcDraw .top) / 2 + g_ciArrowSizeY / 2;

		DrawArrow (dc, rcArrow, 0, 
			(uState & ODS_DISABLED) ? ::GetSysColor (COLOR_GRAYTEXT) : RGB (0,0,0));

		rcDraw.right = rcArrow.left - ::GetSystemMetrics (SM_CXEDGE) / 2;
	}

	//
	// Draw color
	//

	if ((uState & ODS_DISABLED) == 0)
	{
		m_clrCurrent = (bSetInvisible ? m_clrToolBar : m_clrCurrent);
		dc .SetBkColor ((m_clrCurrent == CLR_DEFAULT) ? m_clrDefault : m_clrCurrent);
		dc .ExtTextOut (0, 0, ETO_OPAQUE, &rcDraw, NULL, 0, NULL);
		dc .FrameRect (&rcDraw, ::CreateSolidBrush(m_clrBorders));
		//dc .FrameRect (&rcDraw, (HBRUSH)::GetStockObject (BLACK_BRUSH));
	}

	//
	// Draw a frame around the arrow
	//
	{
		CRect frmRect = lpItem ->rcItem;
		if ((uState & ODS_SELECTED) != 0 || m_fPopupActive)
			frmRect.OffsetRect (1, 1);

		dc.FrameRect(&frmRect, ::CreateSolidBrush(m_clrBorders));
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////
LRESULT CColorPickerBtn::OnClicked (WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled) 
{
	//
	// Mark button as active and invalidate button to force a redraw
	//
	m_fPopupActive = TRUE;
	InvalidateRect (NULL);

	//
	// Get the parent window
	//

	HWND hWndParent = GetParent ();

	//
	// Send the drop down notification to the parent
	//

	SendNotification (CPN_DROPDOWN, m_clrCurrent, TRUE); 

	//
	// Save the current color for future reference
	//

    COLORREF clrOldColor = m_clrCurrent;

	//
	// Display the popup
	//

	BOOL fOked = Picker (true);

	//
	// Cancel the popup
	//

	m_fPopupActive = FALSE;

	//
	// If the popup was canceled without a selection
	//

	if (!fOked)
	{

		//
		// If we are tracking, restore the old selection
		//

		if (m_fTrackSelection)
		{
			if (clrOldColor != m_clrCurrent)
			{
				m_clrCurrent = clrOldColor;
				SendNotification (CPN_SELCHANGE, m_clrCurrent, TRUE); 
			}
		}
		SendNotification (CPN_CLOSEUP, m_clrCurrent, TRUE); 
		SendNotification (CPN_SELENDCANCEL, m_clrCurrent, TRUE); 
	}
	else
	{
		bSetInvisible = false;
		if (clrOldColor != m_clrCurrent)
		{
			SendNotification (CPN_SELCHANGE, m_clrCurrent, TRUE); 
		}
		SendNotification (CPN_CLOSEUP, m_clrCurrent, TRUE); 
		SendNotification (CPN_SELENDOK, m_clrCurrent, TRUE); 
	}

	//
	// Invalidate button to force repaint
	//

	InvalidateRect (NULL);
	return TRUE;
}
