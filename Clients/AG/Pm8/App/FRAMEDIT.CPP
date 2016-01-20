/*
// $Workfile: FRAMEDIT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/FRAMEDIT.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:32   Fred
// Initial revision.
// 
//    Rev 1.17   16 Jul 1997 10:14:54   Jay
// Don't memset fcs; it has a constructor
// 
//    Rev 1.16   16 Apr 1997 12:12:22   Jay
// Fixes for text undo
// 
//    Rev 1.15   12 Mar 1997 14:54:42   Fred
// Changes for flipped warped and rotated text
// 
//    Rev 1.14   08 Nov 1996 09:42:20   Jay
// Removed some test code I had accidentally checked in.
// 
//    Rev 1.13   08 Nov 1996 08:57:42   Jay
// Removed 16-bit compress.obj
// 
//    Rev 1.12   06 Nov 1996 12:44:34   johno
// Passes in the extent that matches the clip rect in OnPaint's call to update.
// 
//    Rev 1.11   28 Oct 1996 10:55:40   johno
// Moved strings to resource file
// 
//    Rev 1.10   22 Oct 1996 15:23:42   johno
// Added CS_GLOBALCLASS to style (RegisterMyClass)
// 
//    Rev 1.9   10 Sep 1996 09:11:18   Jay
// Sets default point size of frame edit.
// 
//    Rev 1.8   02 Aug 1996 15:25:28   Jay
// Bug fixes
// 
//    Rev 1.7   22 Jul 1996 13:06:20   Jay
// New text select mechanism
// 
//    Rev 1.6   17 Jul 1996 15:43:04   Jay
//  
// 
//    Rev 1.5   28 Jun 1996 16:18:58   Jay
//  
// 
//    Rev 1.4   27 Jun 1996 13:26:08   Jay
//  
// 
//    Rev 1.3   26 Jun 1996 14:23:12   Jay
//  
// 
//    Rev 1.2   05 Jun 1996 08:21:46   Jay
//  
// 
//    Rev 1.1   24 May 1996 09:27:50   Jay
// Got rid of some warnings.
// 
//    Rev 1.0   14 Mar 1996 13:33:40   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:36   FRED
// Initial revision.
// 
//    Rev 1.10   24 Aug 1995 14:08:58   JAY
// Fixed to text highlight.
// 
//    Rev 1.9   14 Aug 1995 17:18:54   JAY
// Cut/paste/undo from warp text dialog.
// 
//    Rev 1.8   11 Jul 1995 09:40:56   JAY
// Slight refresh fix. More needed.
// 
//    Rev 1.7   05 Jun 1995 11:34:08   JAY
//  
// 
//    Rev 1.6   02 May 1995 17:09:22   JAY
// Fixed some smart fields bugs.
// 
//    Rev 1.5   02 May 1995 15:39:30   JAY
// More smart fields stuff.
// 
//    Rev 1.4   01 May 1995 15:27:54   JAY
// Restructured the macro stuff
// 
//    Rev 1.3   26 Apr 1995 14:38:58   JAY
// Eliminated the gray flash when scroll bars come on and go off.
// 
//    Rev 1.2   25 Apr 1995 15:24:36   JAY
//  
// 
//    Rev 1.1   25 Apr 1995 09:58:58   JAY
// Get and put warp text.
// 
//    Rev 1.0   25 Apr 1995 09:26:22   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"
#include "pmwview.h"
#include "pmgfont.h"

#include "framedit.h"
#include "framerec.h"
#include "textrec.h"
#include "frameobj.h"
#include "utils.h"
#include "textflow.h"
#include "tcommand.h"

extern BYTE far screen_gamma_curve[];
extern BOOL standard_check_interrupt(LPVOID pData);
extern CPalette* near pOurPal;
extern PCOORD scroll_scale_pcoord(PCOORD x, PCOORD n, PCOORD d);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Text copying code.

static CTextRecord* LockTextOf(CFrameObject* pObject)
{
	CTextRecord* pText = NULL;
	PMGDatabase* pDatabase = pObject->get_database();

	CFrameRecord* pFrame = pObject->LockFrameRecord();
	if (pFrame != NULL)
	{
		pText = pFrame->LockTextRecord();
		pFrame->release();
	}

	return pText;
}

static void CopyFrameToFrame(CFrameObject* pSource, CFrameObject* pDest, BOOL fNoStretch = FALSE)
{
	CTextRecord* pSourceText;

	if ((pSourceText = LockTextOf(pSource)) != NULL)
	{
		CTextRecord* pDestText;
		if ((pDestText = LockTextOf(pDest)) != NULL)
		{
		/* Copy the text, style, etc. */

			pDestText->assign(*pSourceText);

			if (fNoStretch)
			{
				pDestText->ClearStretch();
			}

			pDestText->release(TRUE);			// <- Modified!

			CTextFlow TextFlow(pDest, 0, -1);
			TextFlow.FlowText();
		}
		pSourceText->release();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CFrameEdit

static char cbFrameEditPreamble[] = "Frame Edit Storage\r\n\x1a";

char CFrameEdit::m_cbFileTemplate[] = "~FE";

CFrameEdit::CFrameEdit() :
	m_Txp(NULL)
{
	m_fValid = FALSE;
	m_pFrame = NULL;
	m_fCursorVisible = FALSE;
	m_fCursorOn = FALSE;
	m_TextCursorPBox.x0 = -1;

	m_fCreatedByClassName = FALSE;
	m_fHorizontalScrollShown = -1;
	m_fVerticalScrollShown = -1;
}

CFrameEdit::~CFrameEdit()
{
	m_Txp.Reset();

	if (m_pFrame != NULL)
	{
		m_pFrame->destroy();
		delete m_pFrame;
	}
}

BOOL CFrameEdit::Init(CMacroServer* pMacroServer, int nDefaultPointSize /*=-1*/)
{
/*
// Create the database we will be using.
*/

/* Create the database now. */

	if ((m_Mapping.device = new TmpFile(m_cbFileTemplate)) == NULL)
	{
		return FALSE;
	}

	ERRORCODE error;

	if ((error = m_Mapping.map_to_file(NULL, FALSE)) != ERRORCODE_None
			|| (error = m_Mapping.initialize(cbFrameEditPreamble,
													SIGNATURE,
													FILE_TYPE,
													0)) != ERRORCODE_None)
	{
		m_Mapping.FreeAll();

		return FALSE;			// No database. Sorry.
	}

/*
// We now have the database.
// Create the mandatory records.
*/

	if (m_Mapping.CreateMandatoryRecords() != ERRORCODE_None)
	{
		m_Mapping.flush_database();

		m_Mapping.FreeAll();
		return FALSE;
	}

	PMGDatabase* pDatabase = m_Mapping.database;

	if (nDefaultPointSize != -1)
	{
		((PMGFontServer*)m_Mapping.font_server)->m_nDefaultPointSize = nDefaultPointSize;
	}
	((PMGFontServer*)m_Mapping.font_server)->set_database(pDatabase);
	pDatabase->SetMacroServer(pMacroServer);

	pDatabase->set_refresh_notify(RefreshNotify, this);

/*
// We can now proceed.
*/

	CRect crBounds;
	GetEditRect(crBounds);

	m_rc.hwnd = GetSafeHwnd();

	m_rc.clip_rect = crBounds;
	m_rc.destination_rect = crBounds;

/*
// Build the redisplay context.
*/

	m_rc.scaled_source_x0 = m_rc.scaled_source_y0 = 0;

	m_rc.destination_x0 = crBounds.left;
	m_rc.destination_y0 = crBounds.top;

	m_rc.register_toggle(RedispToggleFunction, this);

	HDC hdcScreen = ::GetDC(0);

	if (hdcScreen != NULL)
	{
		m_rc.x_resolution = GetDeviceCaps(hdcScreen, LOGPIXELSX);
		m_rc.y_resolution = GetDeviceCaps(hdcScreen, LOGPIXELSY);

		m_rc.set_info(hdcScreen);

		::ReleaseDC(0, hdcScreen);

		m_rc.outline_gamma_curve = m_rc.bitmap_gamma_curve = screen_gamma_curve;

		m_rc.set_check_interrupt(standard_check_interrupt, (VOIDPTR)&m_rc);
		m_rc.terminate_interrupts = TRUE;

	/*
	// Create the frame object now.
	*/

		FRAMEOBJ_CREATE_STRUCT fcs;

		m_rc.screen_to_pbox(crBounds, &fcs.bound);
		m_rc.source_pbox = fcs.bound;
		fcs.flags = FRAME_FLAG_stretch_frame
						| FRAME_FLAG_ystretch_frame
						| FRAME_FLAG_use_white_space;
		// Create a style to use.

#if 0
		TextStyle style(pDatabase);
		style.set_default();
		style.set_size(18);
		style.set_base_size(18);
		style.update_font_metrics();
		fcs.pStyle = &style;
#else
		fcs.pStyle = NULL;			// We'll get the style of the frame.
#endif

		if ((m_pFrame = pDatabase->create_frame_object(&fcs)) != NULL)
		{
			m_pFrame->set_select_flags(0);
			m_pFrame->calc();

			m_Txp.Init(m_pFrame, 0, TxpToggleFunction, this);

			m_nEditState = STATE_INIT;

			m_fValid = TRUE;

			ChangeView(VIEW_LAST);
		}
	}

	return m_fValid;
}

void CFrameEdit::SetTextTo(CFrameObject* pObject)
{
	CopyFrameToFrame(m_pFrame, pObject);
}

void CFrameEdit::GetTextFrom(CFrameObject* pObject)
{
	CopyFrameToFrame(pObject, m_pFrame, TRUE);
	m_Txp.CalcAll();
	ChangeView(VIEW_LAST);		// Make everybody be current.
}

void CFrameEdit::GetEditRect(CRect& crEdit)
{
	GetClientRect(crEdit);
	crEdit.InflateRect(-2,-2);
}

BEGIN_MESSAGE_MAP(CFrameEdit, CWnd)
	//{{AFX_MSG_MAP(CFrameEdit)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_GETDLGCODE()
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFrameEdit message handlers

void CFrameEdit::ReframeOnTxp(VOID)
{
	if (!(m_Txp.Visible() & CTxp::TOGGLE_Cursor))
	{
	/* Make sure the cursor box is calculated if not on. */

		m_Txp.CalcCursor();
	}

/* Get the bounds for the txp cursor and make sure we're there. */

	PBOX pbox = m_Txp.CursorBox();

	if (memcmp(&m_TextCursorPBox, &pbox, sizeof(PBOX)) != 0)
	{
	/* Cursor has changed position! */
		m_TextCursorPBox = pbox;
		ReframeView(pbox);
	}
}

/*
// Position the view at a certain locaton.
*/

VOID CFrameEdit::PositionView(PCOORD new_x0, PCOORD new_y0, PCOORD new_x1, PCOORD new_y1, FLAGS flags, int nXRound /*=0*/, int nYRound /*=0*/)
{
	BOOL scrolled = FALSE;
	PBOX new_window;
	PBOX view_extent = m_pFrame->get_bound();

#if 0
	od("PV: %ld, %ld, %ld, %ld (%ld, %ld, %ld, %ld)\r\n",
 					new_x0, new_y0, new_x1, new_y1,
					rc.source_pbox);
#endif

/*
// Compute clipped centered coordinates.
*/

	PCOORD window_x_size, window_y_size;

	window_x_size = m_rc.source_pbox.x1-m_rc.source_pbox.x0;
	window_y_size = m_rc.source_pbox.y1-m_rc.source_pbox.y0;

	if (flags & PVIEW_FLAG_center)
	{
		PCOORD cursor_half_x_size, cursor_half_y_size;

		cursor_half_x_size = (new_x1-new_x0)/2;
		cursor_half_y_size = (new_y1-new_y0)/2;

		new_window.x0 = new_x0+cursor_half_x_size-(window_x_size/2);
		new_window.y0 = new_y0+cursor_half_y_size-(window_y_size/2);
	}
	else
	{
		new_window.x0 = new_x0;
		new_window.y0 = new_y0;
	}

	if (new_window.x0+window_x_size > view_extent.x1)
	{
		new_window.x0 = view_extent.x1-window_x_size;
	}

	if (new_window.x0 < view_extent.x0)
	{
		new_window.x0 = view_extent.x0;
	}

	if (new_window.y0+window_y_size > view_extent.y1)
	{
		new_window.y0 = view_extent.y1-window_y_size;
	}

	if (new_window.y0 < view_extent.y0)
	{
		new_window.y0 = view_extent.y0;
	}

	SHORT x_scale = PAGE_RESOLUTION/m_rc.x_resolution;
	SHORT y_scale = PAGE_RESOLUTION/m_rc.y_resolution;
//	BOOL can_scroll = current_view >= VIEW_100 && current_view <= VIEW_400;
//	BOOL can_scroll = TRUE;
	BOOL can_scroll = FALSE;

	if (can_scroll)
	{
	/* Keep the damn thing aligned on our "grid". */
#if 0
		if (current_view < VIEW_100)
		{
			x_scale /= m_rc.GetScaleDenominator();		// Numerator should be 1.
			y_scale /= m_rc.GetScaleDenominator();
		}
#endif
//		od("xscale: %d, yscale: %d\r\n", x_scale, y_scale);
//		od("Window p0: %ld, %ld -> ", new_window.x0, new_window.y0);
		if (nXRound != 0)
		{
			new_window.x0 += nXRound*(x_scale-1);
		}
		if (nYRound != 0)
		{
			new_window.y0 += nYRound*(y_scale-1);
		}
		new_window.x0 /= x_scale;
		new_window.x0 *= x_scale;
		new_window.y0 /= y_scale;
		new_window.y0 *= y_scale;
//		od("aligned p0: %ld, %ld\r\n", new_window.x0, new_window.y0);
	}

	new_window.x1 = new_window.x0 + window_x_size;
	new_window.y1 = new_window.y0 + window_y_size;

	if (memcmp(&new_window, &m_rc.source_pbox, sizeof(PBOX)) != 0)
	{
		if (can_scroll && (flags & PVIEW_FLAG_scroll))
		{
			PCOORD width, height, cur_width, cur_height;

			width = new_window.x1 - new_window.x0;
			height = new_window.y1 - new_window.y0;
			cur_width = m_rc.source_pbox.x1 - m_rc.source_pbox.x0;
			cur_height = m_rc.source_pbox.y1 - m_rc.source_pbox.y0;

			if (cur_width == width && cur_height == height)
			{
				PCOORD dx = m_rc.source_pbox.x0/x_scale - new_window.x0/x_scale;
				PCOORD dy = m_rc.source_pbox.y0/y_scale - new_window.y0/y_scale;

#if 0
				if (current_view > VIEW_100)
				{
					dx *= rc.GetScaleNumerator();		// Denominator should be 1.
					dy *= rc.GetScaleNumerator();		// Denominator should be 1.
				}
#endif

				CRect client;
				GetEditRect(client);

			/* Window is scrolling. */

				if (dx < (PCOORD)(client.right - client.left)
						&& dy < (PCOORD)(client.bottom - client.top))
				{
//					od("SW dx: %d, dy: %d\r\n", (SHORT)dx, (SHORT)dy);
					ScrollWindow((SHORT)dx, (SHORT)dy, &client, &client);
					scrolled = TRUE;
				}
			}
		}

		m_rc.source_pbox = new_window;

//		od("Spacing: %ld, %ld\r\n", spacing);
		m_rc.scaled_source_x0 = (m_rc.source_pbox.x0 /*+spacing.x*/) * m_rc.x_resolution;
		m_rc.scaled_source_y0 = (m_rc.source_pbox.y0 /*+spacing.y*/) * m_rc.y_resolution;

		if (!scrolled)
		{
			InvalidateRect(&m_rc.destination_rect /*, FALSE*/);
		}

	/*
	// Update the scroll bars.
	*/

		if (flags & PVIEW_FLAG_set_scrolls)
		{
			PCOORD view_width = m_rc.source_pbox.x1 - m_rc.source_pbox.x0;
			PCOORD view_height = m_rc.source_pbox.y1 - m_rc.source_pbox.y0;
			SHORT z;
			PPNT dims;

			dims.x = view_extent.x1 - view_extent.x0;
			dims.y = view_extent.y1 - view_extent.y0;

			PCOORD full_range;

			full_range = dims.x - view_width - 1;
			z = (SHORT)scroll_scale_pcoord(m_rc.source_pbox.x0 - view_extent.x0, 1000-1, full_range);
			SetScrollPos(SB_HORZ, z);

			full_range = dims.y - view_height - 1;
			z = (SHORT)scroll_scale_pcoord(m_rc.source_pbox.y0 - view_extent.y0, 1000-1, full_range);
			SetScrollPos(SB_VERT, z);
		}
	}
}

VOID CFrameEdit::UpdateScrolls(RECT *client, PBOX *pbox, BOOL horz, BOOL vert)
{
	int cwidth = client->right - client->left;
	int cheight = client->bottom - client->top;
	BOOL need_horz, need_vert;

	need_horz = horz && (pbox->x1-pbox->x0) > (PCOORD)cwidth;
	need_vert = vert && (pbox->y1-pbox->y0) > (PCOORD)cheight;

/* Handle the scroll bars. */

	if (m_fHorizontalScrollShown != need_horz
		 || m_fVerticalScrollShown != need_vert)
	{
		CRect crEdit;
		GetEditRect(crEdit);

		ValidateRect(crEdit);

		m_fHorizontalScrollShown = need_horz;
		SetScrollRange(SB_HORZ, 0, need_horz ? 1000 : 0, TRUE); 

		m_fVerticalScrollShown = need_vert;
		SetScrollRange(SB_VERT, 0, need_vert ? 1000 : 0, TRUE);
	}
}

/*
// Reframe the view to contain a certain locaton.
*/

VOID CFrameEdit::ReframeView(PBOX cursor)
{
	PBOX view_extent = m_pFrame->get_bound();

	if ((!((cursor.x0 <= m_rc.source_pbox.x0) && (cursor.x1 >= m_rc.source_pbox.x1)) &&
	      (((cursor.x0 < m_rc.source_pbox.x0) && (m_rc.source_pbox.x0 > view_extent.x0)) ||
	       ((cursor.x1 > m_rc.source_pbox.x1) && (m_rc.source_pbox.x1 < view_extent.x1)))) ||
	    (!((cursor.y0 <= m_rc.source_pbox.y0) && (cursor.y1 >= m_rc.source_pbox.y1)) &&
	      (((cursor.y0 < m_rc.source_pbox.y0) && (m_rc.source_pbox.y0 > view_extent.y0)) ||
	       ((cursor.y1 > m_rc.source_pbox.y1) && (m_rc.source_pbox.y1 < view_extent.y1)))))
	{
   	PositionView(
					cursor.x0,
 					cursor.y0,
 					cursor.x1,
					cursor.y1,
					PVIEW_FLAG_center | PVIEW_FLAG_set_scrolls | PVIEW_FLAG_scroll);
	}
}

/*
// Signal a new event.
*/

VOID CFrameEdit::SignalMouseEvent(FLAGS nFlags, CPoint point, EVENT event)
{
	m_EventInfo.old_xy = m_EventInfo.current_xy;
	m_EventInfo.current_xy = point;
	m_EventInfo.new_position = m_EventInfo.current_xy != m_EventInfo.old_xy;

	m_EventInfo.flags = nFlags;
	m_EventInfo.event = event;

	InvokeEventHandler();
}

void CFrameEdit::InsertCharacter(CHARACTER c)
{
	OnChar(c, 1, 0);
}

void CFrameEdit::ReflowMacros(void)
{
	m_pFrame->ReflowMacros();
}

void CFrameEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_EventInfo.new_position = FALSE;

	m_EventInfo.v_key = nChar;
	m_EventInfo.repeat_count = nRepCnt;
	*(UINT*)&m_EventInfo.key_state = nFlags;

	if (m_EventInfo.key_state.previous_state)
	{
		m_EventInfo.event = EVENT_KEY_DOWN;
	}
	else
	{
		if (nChar == VK_SHIFT)
		{
			m_EventInfo.flags |= MK_SHIFT;
		}

		if (nChar == VK_CONTROL)
		{
			m_EventInfo.flags |= MK_CONTROL;
		}

		m_EventInfo.event = EVENT_KEY_RISING;
	}

	InvokeEventHandler();
}

void CFrameEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	m_EventInfo.new_position = FALSE;
	m_EventInfo.v_key = nChar;

	if (nChar == VK_SHIFT)
	{
		m_EventInfo.flags &= ~MK_SHIFT;
	}
	if (nChar == VK_CONTROL)
	{
		m_EventInfo.flags &= ~MK_CONTROL;
	}

	m_EventInfo.event = EVENT_KEY_FALLING;

	InvokeEventHandler();
}

void CFrameEdit::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);
	m_Txp.Off(CTxp::TOGGLE_Cursor);
	m_fCursorVisible = FALSE;
}

void CFrameEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();

	SignalMouseEvent(nFlags, point, EVENT_BUTTON_RISING);
}

void CFrameEdit::OnLButtonUp(UINT nFlags, CPoint point)
{
	SignalMouseEvent(nFlags, point, EVENT_BUTTON_FALLING);

/* don't release the capture if right mouse still down... */

	if( !( nFlags & MK_RBUTTON ))
	{
		ReleaseCapture();
	}
}

void CFrameEdit::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	SignalMouseEvent(nFlags, point, EVENT_BUTTON_DBLCLK);
}

void CFrameEdit::OnMouseMove(UINT nFlags, CPoint point)
{
	SignalMouseEvent(nFlags, point, EVENT_MOUSE_MOVE);
}

void CFrameEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
//	od("Got char %d, %d, %u\r\n", nChar, nRepCnt, nFlags);

	m_EventInfo.new_position = FALSE;

	m_EventInfo.v_key = nChar;
	m_EventInfo.repeat_count = nRepCnt;
	*(UINT*)&m_EventInfo.key_state = nFlags;

	m_EventInfo.event = EVENT_KEY;

	InvokeEventHandler();
}

void CFrameEdit::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

	m_fCursorVisible = TRUE;
	m_fCursorOn = FALSE;
	m_Txp.On();
}

void CFrameEdit::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	if (m_fValid)
	{
		HDC hdcSave = m_rc.destination_hdc;
		m_rc.destination_hdc = dc.GetSafeHdc();

		if (GetClipBox(m_rc.destination_hdc, &m_rc.clip_rect) != NULLREGION)
		{
		/* Realize our palette. */
		
			HPALETTE hOurPal = (HPALETTE)pOurPal->GetSafeHandle();
			HPALETTE hOldPal = SelectPalette(m_rc.destination_hdc, hOurPal, FALSE);
			RealizePalette(m_rc.destination_hdc);

		/* Draw a nice frame around our image. */

#if 0
			HPEN hOldPen = (HPEN)SelectObject(m_rc.destination_hdc, GetStockObject(BLACK_PEN));
			HBRUSH hOldBrush = (HBRUSH)SelectObject(m_rc.destination_hdc, GetStockObject(WHITE_BRUSH));

			CRect crClient;
			GetClientRect(crClient);

			m_rc.toggle(FALSE, &crClient);
			Rectangle(m_rc.destination_hdc,
				crClient.left,
				crClient.top,
				crClient.right,
				crClient.bottom);
			m_rc.toggle(TRUE, &crClient);
			SelectObject(m_rc.destination_hdc, hOldBrush);
			SelectObject(m_rc.destination_hdc, hOldPen);
#endif
			UpdateStatePtr ustate;

         PBOX extent;

         m_rc.screen_to_pbox(&m_rc.clip_rect, &extent);

         if (IntersectBox(&extent, &m_rc.source_pbox, &extent))
         {
			   if ((ustate = m_pFrame->update(&m_rc, &extent, &m_rc.clip_rect, NULL, REFRESH_REDRAW)) != NULL)
			   {
			   /* See what happened. */
				   switch (ustate->type)
				   {
					   case UPDATE_TYPE_Interrupted:
					   {
					   /* This should not happen. */
						   delete ustate;
					   /* Fall through to... */
					   }
					   case UPDATE_TYPE_Terminated:
					   {
					   /* Try again later. */
						   CRect crInvalid;
						   crInvalid.IntersectRect(&m_rc.destination_rect, &m_rc.clip_rect);
						   InvalidateRect(crInvalid /*, FALSE*/);
						   break;
					   }
					   default:
					   {
						   break;
					   }
				   }
			   }
         }
		}
		m_rc.destination_hdc = hdcSave;

#if 0
		if (GetFocus() == this)
		{
			m_Txp.On();
		}
#else
		m_Txp.On((GetFocus() == this) ? CTxp::TOGGLE_All : CTxp::TOGGLE_Select);
#endif
	}
}

void CFrameEdit::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
		case FRAME_EDIT_CURSOR_TIMER:
		{
//			od("Toggle the cursor!\r\n");
			ToggleTxpCursor();
			break;
		}
		default:
		{
			CWnd::OnTimer(nIDEvent);
			break;
		}
	}
}

static BOOL near IsReturnKey(MSG msg)
{
	if (msg.lParam != 0)
	{
		LPMSG pMsg = (LPMSG)msg.lParam;

	/* Get the message to be handled. */

		switch (pMsg->message)
		{
			case WM_CHAR:
			case WM_KEYDOWN:
			{
				if (pMsg->wParam == VK_RETURN)
				{
				/* We want to get this message ourselves. */
					return TRUE;
				}
				break;
			}
			default:
			{
				break;
			}
		}
	}
	return FALSE;
}

UINT CFrameEdit::OnGetDlgCode()
{
	UINT uRet = DLGC_WANTARROWS | DLGC_WANTCHARS;

	if (IsReturnKey(*GetCurrentMessage()))
	{
		uRet |= DLGC_WANTMESSAGE;
	}
	return uRet;
}

/*
// Toggle a rectangle on the redisplay map.
// The destination_hdc field must be valid in the rc before calling this.
*/

VOID CFrameEdit::toggle_square(PBOX pbox)
{
	// NOTE: This does not take into account warping and rotation.
	RECT r;
	
	if (pbox.x0 > pbox.x1)
	{
		PCOORD x0 = pbox.x0;
		pbox.x0 = pbox.x1;
		pbox.x1 = x0;
	}

	/* Now do the real area. */

	CRect crEdit;

	GetEditRect(crEdit);

	m_rc.pbox_to_screen(&pbox, TRUE);
	if (m_rc.convert_pbox(&pbox, &r, crEdit))
	{
//		od("ts: %x\r\n", (WORD)m_rc.destination_hdc);
		PatBlt(m_rc.destination_hdc, r.left, r.top, r.right-r.left, r.bottom-r.top, DSTINVERT);
	}
}

/*
// A little routine to help this damn flipped thing.
*/

static VOID near
do_UnionBox(PBOX_PTR dest, PBOX src, BOOL first_time)
{
	if (src.x1 < src.x0)
	{
		PCOORD x = src.x0;
		src.x0 = src.x1;
		src.x1 = x;
	}

	if (first_time)
	{
		*dest = src;
	}
	else
	{
		UnionBox(dest, dest, &src);
	}
}

/*
// Toggle the select.
// The destination_hdc field must be valid in the rc before calling this.
*/

VOID CFrameEdit::toggle_select(CTxp* pTxp)
{
	if (pTxp->SomethingSelected())
	{
		pTxp->UpdateSelectBounds();
		int nBounds = pTxp->NumberOfSelectBounds();
		for (int nBound = 0; nBound < nBounds; nBound++)
		{
			PBOX Bound = pTxp->SelectBound(nBound);
			toggle_square(Bound);
		}
	}
}

/*
// The toggle function for the txp.
*/

void CFrameEdit::TxpToggleFunction(CTxp* pTxp, int nWhich, LPVOID pData)
{
	CFrameEdit* pThis = (CFrameEdit*)pData;
	CDC* pCdc = pThis->GetDC();
	HDC hdcSave = pThis->m_rc.destination_hdc;

/*
// Get an HDC to use.
*/

	pThis->m_rc.destination_hdc = pCdc->GetSafeHdc();

	if (nWhich & CTxp::TOGGLE_Cursor)
	{
		if (pTxp->Visible() & CTxp::TOGGLE_Cursor)
		{
		/* Turning on! */

			pThis->m_fCursorOn = TRUE;

			if (pThis->m_fCursorVisible)
			{
				pThis->toggle_square(pTxp->CursorBox());
			}

			pThis->SetTimer(FRAME_EDIT_CURSOR_TIMER, ::GetCaretBlinkTime(), NULL);
		}
		else
		{
		/* Turning off! */

			if (pThis->m_fCursorOn && pThis->m_fCursorVisible)
			{
				pThis->toggle_square(pTxp->CursorBox());
			}
			pThis->KillTimer(FRAME_EDIT_CURSOR_TIMER);
//			on_time = 0;

			pThis->m_fCursorOn = FALSE;
		}
	}

	if (nWhich & CTxp::TOGGLE_Select)
	{
		pThis->toggle_select(pTxp);
	}

	pThis->ReleaseDC(pCdc);
	pThis->m_rc.destination_hdc = hdcSave;
}

/*
// Our toggle for when redisplaying is happening.
*/

BOOL CFrameEdit::RedispToggleFunction(RedisplayContext* rc, BOOL turn_on, LPRECT clip, VOIDPTR pData)
{
	CFrameEdit* pThis = (CFrameEdit*)pData;

	int visible = pThis->m_Txp.Visible();

	if (pThis->m_fCursorVisible && (visible & CTxp::TOGGLE_Cursor))
	{
		pThis->toggle_square(pThis->m_Txp.CursorBox());
	}

	if (visible & CTxp::TOGGLE_Select)
	{
		pThis->toggle_select(&pThis->m_Txp);
	}

	return TRUE;
}

/*
// Our notification of the need to be refreshed.
*/

void CFrameEdit::RefreshNotify(LPVOID pData, PBOX_PTR pExtent, REFRESH_TYPE nType, PMGPageObjectPtr pObject, LPRECT pExtraPixels)
{
	CFrameEdit* pThis = (CFrameEdit*)pData;

	BOOL fVisible = pThis->IsWindowVisible();

#if 1
	od("Got refresh extent: (%ld, %ld, %ld, %ld) [%d] {vis:%d}\r\n",
		*pExtent,
		nType,
		fVisible);
#endif

	if (fVisible)
	{
		PBOX Extent = *pExtent;
		CRect crRefresh;

		pThis->m_rc.pbox_to_screen(&Extent, TRUE);
		if (pThis->m_rc.convert_pbox(&Extent, &crRefresh, NULL))
		{
			if (crRefresh.IntersectRect(&crRefresh, &pThis->m_rc.destination_rect))
			{
				od("RI: %d, %d, %d, %d\r\n", *(LPCRECT)crRefresh);
				pThis->InvalidateRect(&crRefresh /*, FALSE*/);
			}
		}
	}
}

/*
// Toggle the txp cursor if there is one.
*/

void CFrameEdit::ToggleTxpCursor(void)
{
	if (m_Txp.Valid())
	{
		if (m_Txp.Visible() & CTxp::TOGGLE_Cursor)
		{
			m_Txp.Off(CTxp::TOGGLE_Cursor);
			m_fCursorVisible = !m_fCursorVisible;
			m_Txp.On(CTxp::TOGGLE_Cursor);
		}
	}
}

void CFrameEdit::TxpCursorOn(void)
{
	TRACE("TxpCursorOn...\n");
	if (m_Txp.Transition())
	{
		m_Txp.Off(CTxp::TOGGLE_Cursor);
		m_fCursorVisible = TRUE;			// Make the cursor show up next time.
	}
}

/*
// Handle a keystroke.
*/

VOID CFrameEdit::Keystroke(EVENT_INFO *pInfo)
{
	BOOL shifted = SHIFT_KEY(pInfo->flags);
	BOOL modified = FALSE;
	FLAGS flags = m_Txp.Flags();
	PBOX bound = m_pFrame->get_bound();

	switch (pInfo->event)
	{
		case EVENT_KEY_RISING:
		case EVENT_KEY_DOWN:
		{
			switch (pInfo->v_key)
			{
				case VK_UP:
				{
					m_Txp.Up(shifted);
					break;
				}
				case VK_DOWN:
				{
					m_Txp.Down(shifted);
					break;
				}
				case VK_LEFT:
				{
					if (CTRL_KEY(pInfo->flags))
					{
						m_Txp.LeftWord(shifted);
					}
					else
					{
						m_Txp.Left(shifted);
					}
					break;
				}
				case VK_RIGHT:
				{
					if (CTRL_KEY(pInfo->flags))
					{
						m_Txp.RightWord(shifted);
					}
					else
					{
						m_Txp.Right(shifted);
					}
					break;
				}
				case VK_HOME:
				{
					if (CTRL_KEY(pInfo->flags))
					{
						m_Txp.Bot(shifted);
					}
					else
					{
						m_Txp.Bol(shifted);
					}
					break;
				}
				case VK_END:
				{
					if (CTRL_KEY(pInfo->flags))
					{
						m_Txp.Eot(shifted);
					}
					else
					{
						m_Txp.Eol(shifted);
					}
					break;
				}
				case VK_PRIOR:
				{
					m_Txp.Bot(shifted);
					break;
				}
				case VK_NEXT:
				{
					m_Txp.Eot(shifted);
					break;
				}
				case VK_BACK:
				{
					UpdateUndoBackspace(&m_Txp, pInfo->repeat_count);

					m_Txp.Backspace(pInfo->repeat_count);

					modified = TRUE;
					break;
				}
				case VK_DELETE:
				{
					UpdateUndoDelete(&m_Txp, pInfo->repeat_count);

					m_Txp.DeleteText(pInfo->repeat_count);

					modified = TRUE;
					break;
				}
				default:
				{
#if 0
					if (CTRL_KEY(pInfo->flags))
					{
					/* See if this is a control key we trap. */
						switch (pInfo->v_key)
						{
							case 'B':
							{
								OnTextBold();
								break;
							}
							case 'I':
							{
								OnTextItalic();
								break;
							}
							case 'U':
							{
								OnTextUnderline();
								break;
							}
							default:
							{
								break;
							}
						}
					}
#endif
//					od("Key %x pressed [%x]\r\n", pInfo->v_key, pInfo->flags);
					break;
				}
			}
			break;
		}
		case EVENT_KEY:
		{
//			od("Got key %d (%d) [%x]\r\n", pInfo->v_key, pInfo->repeat_count, pInfo->flags);

			CHARACTER c;
			BOOL do_insert = TRUE;

			c = pInfo->v_key;

			if (c < ' ' && CTRL_KEY(pInfo->flags))
			{
			/* Don't insert any control keys. */
				do_insert = FALSE;
			}
			else
			{
				switch (c)
				{
					case VK_BACK:
					{
						do_insert = FALSE;
						break;
					}
					case VK_TAB:
					{
						c = ' ';
						break;
					}
					case VK_RETURN:
					{
						c = '\n';
						break;
					}
					default:
					{
						break;
					}
				}
			}

		/* Insert the character. */

			if (do_insert)
			{
				UpdateUndoTyping(&m_Txp, c, pInfo->repeat_count);

				if (m_Txp.InsertText(&c, pInfo->repeat_count, TRUE) == ERRORCODE_Full)
				{
					AfxMessageBox(IDS_TEXT_BOX_FULL, MB_OK | MB_ICONEXCLAMATION);
				}
//				od("Inserted %x\r\n", c);
				modified = TRUE;
			}

			break;
		}
		default:
		{
			break;
		}
	}

	if (modified)
	{
		m_pFrame->calc();
		PBOX new_bound = m_pFrame->get_bound();
		if (memcmp(&new_bound, &bound, sizeof(PBOX)) != 0)
		{
			m_rc.pbox_to_screen(&new_bound, TRUE);

			CRect crClient;
			GetEditRect(crClient);

			UpdateScrolls(crClient, &new_bound, TRUE, TRUE);
		}
	}

/* See if the txp moved. */

	TxpCursorOn();

/* See if we did something to modify us. */

	ReframeOnTxp();
}

/*
// Normal state.
*/

BOOL CFrameEdit::StateNormal(EVENT_INFO *pInfo)
{
	EVENT_INFO new_info;

	if ((pInfo->flags & MK_LBUTTON)
			&& pInfo->event != EVENT_BUTTON_RISING
			&& pInfo->event != EVENT_BUTTON_DBLCLK)
	{
	/* Button is down from some other click. Ignore this. */
		m_nEditState = STATE_RELEASE;
		return TRUE;
	}

	if (pInfo->event == EVENT_KEY_FALLING)
	{
	/* Ignore these. */
		return -1;
	}

	switch (pInfo->event)
	{
		case EVENT_EDIT_DELETE:
		{
		/* Simulate a DEL key pressed (for modularity's sake) */
			new_info.event = EVENT_KEY_DOWN;
			new_info.v_key = VK_DELETE;
			new_info.repeat_count = 1;

			pInfo = &new_info;

		/* Fall through to... */
		}
		case EVENT_KEY_RISING:
		case EVENT_KEY_DOWN:
		case EVENT_KEY_FALLING:
		case EVENT_KEY:
		{
		/* Handle keys elsewhere. */
			Keystroke(pInfo);
			return TRUE;				// Handled
		}
		default:
		{
			break;
		}
	}

	PPNT p;
	m_rc.screen_to_ppnt(pInfo->current_xy, &p);

	PMGPageObjectPtr this_object = m_Txp.Object();

/*
// See if we're over our frame.
*/

	if (pInfo->flags & MK_LBUTTON)
	{
		if (pInfo->event == EVENT_BUTTON_DBLCLK)
		{
		/* Double click! Let everybody know! */

			m_Txp.SelectAll();
		}
		else
		{
			SetFocus();
			m_Txp.MoveToPoint(p,
									SHIFT_KEY(pInfo->flags)
											? CTxp::END_Unknown
											: CTxp::END_NoSelection,
									TRUE);
			m_nEditState = TEXT_EDIT_STATE_DRAGGING;
			TxpCursorOn();
		}
	}

	ReframeOnTxp();
	return TRUE;			/* Handled */
}

/*
// Initialization state.
*/

BOOL CFrameEdit::StateInit(EVENT_INFO *pInfo)
{
	m_nEditState = STATE_NORMAL;

	return StateNormal(pInfo);				/* Do it now, too. */
}

#define TEXT_SCROLL_DELAY	2

/*
// The "dragging" state of text entry.
*/

BOOL CFrameEdit::StateDragging(EVENT_INFO *pInfo)
{
	if (!(pInfo->flags & MK_LBUTTON))
	{
	/* Done dragging! */
		m_nEditState = STATE_NORMAL;
	}
	else
	{
		PPNT p;
		PBOX view_extent = m_pFrame->get_bound();

		static ULONG last_scroll = 0L;
		ULONG now = GetTickCount();

		m_rc.screen_to_ppnt(pInfo->current_xy, &p);

		if (p.x > m_rc.source_pbox.x1 && m_rc.source_pbox.x1 < view_extent.x1)
		{
			if (now > last_scroll+TEXT_SCROLL_DELAY)
			{
				CHARACTER_INDEX last = m_Txp.LastCharacterInLine();

				last_scroll = now;

				if (m_Txp.Index() < last)
				{
					PBOX pbox;
					PCOORD dx;

					m_Txp.Right(TRUE);
					m_Txp.CalcCursor();

					dx = m_Txp.CursorBox().x1 - m_rc.source_pbox.x1;

					pbox.y0 = m_rc.source_pbox.y0;
					pbox.y1 = m_rc.source_pbox.y1;
					pbox.x0 = m_rc.source_pbox.x0 + dx;
					pbox.x1 = m_rc.source_pbox.x1 + dx;
					ReframeView(pbox);
				}
			}
		}
		else if (p.x < m_rc.source_pbox.x0 && m_rc.source_pbox.x0 > view_extent.x0)
		{
			if (now > last_scroll+TEXT_SCROLL_DELAY)
			{
				CHARACTER_INDEX first = m_Txp.FirstCharacterInLine();

				last_scroll = now;

				if (m_Txp.Index() > first)
				{
					PBOX pbox;
					PCOORD dx;

					m_Txp.Left(TRUE);
					m_Txp.CalcCursor();

					dx = m_Txp.CursorBox().x0 - m_rc.source_pbox.x0;

					pbox.y0 = m_rc.source_pbox.y0;
					pbox.y1 = m_rc.source_pbox.y1;
					pbox.x0 = m_rc.source_pbox.x0 + dx;
					pbox.x1 = m_rc.source_pbox.x1 + dx;
					ReframeView(pbox);
				}
			}
		}
		else if (pInfo->new_position)
		{
			m_Txp.MoveToPoint(p, CTxp::END_Current, TRUE);
			TxpCursorOn();
		}
	}
	return TRUE;
}

/*
// Release state.
// Wait for the user to release the mouse button.
*/

BOOL CFrameEdit::StateRelease(EVENT_INFO *pInfo)
{
	if (!(pInfo->flags & MK_LBUTTON))
	{
	/* Done waiting! */
		m_nEditState = STATE_NORMAL;
	}
	return TRUE;
}

/*
// The states for the text mode.
*/

CFrameEdit::STATE_HANDLER CFrameEdit::States[] =
{
	CFrameEdit::StateInit,
	CFrameEdit::StateNormal,
	CFrameEdit::StateRelease,

	CFrameEdit::StateDragging
};

#define TEXT_EDIT_STATE_COUNT		(sizeof(States)/sizeof(States[0]))

/*
// The text mode handler.
*/

void CFrameEdit::InvokeEventHandler(void)
{
	SHORT nState = m_nEditState;

	if (nState > STATE_BASE_END)
	{
		if (nState >= TEXT_EDIT_STATE_BASE)
		{
		/* One of our special states! */
			nState -= (TEXT_EDIT_STATE_BASE-STATE_BASE_END);
		}
		else
		{
		/* Not a valid state for here. */
			nState = TEXT_EDIT_STATE_COUNT;
		}
	}

/* See if we need to handle the state. */

	if (nState < TEXT_EDIT_STATE_COUNT)
	{
	/* Valid state, process. */
		(this->*States[nState])(&m_EventInfo);
	}

/* Make sure we're on. */

	if (GetFocus() == this)
	{
		TRACE("Handler: m_Txp.On...\n");
		m_Txp.On();
	}
}

BOOL CFrameEdit::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest == HTERROR)
	{
		return CWnd::OnSetCursor(pWnd, nHitTest, message);
	}
	else
	{
		if (nHitTest == HTCLIENT)
		{
			SetCursor(::LoadCursor(NULL, IDC_IBEAM));
			return TRUE;
		}
		else
		{
		/* Arrow or whatever. */
			return CWnd::OnSetCursor(pWnd, nHitTest, message);
		}
	}
}

BOOL CFrameEdit::OnEraseBkgnd(CDC* pDC)
{
	HDC hdcSave = m_rc.destination_hdc;
	m_rc.destination_hdc = pDC->GetSafeHdc();

	int nSave = ::SaveDC(m_rc.destination_hdc);

	SelectObject(m_rc.destination_hdc, GetStockObject(BLACK_PEN));
	SelectObject(m_rc.destination_hdc, GetStockObject(WHITE_BRUSH));

	CRgn rgnFrame;
	CRgn rgnView;
	CRgn rgnErase;

	TRY
	{
		CRect crEdit;
		GetEditRect(crEdit);

		PBOX Bound = m_pFrame->get_bound();

		RECT rFrame;
		m_rc.pbox_to_screen(&Bound, TRUE);
		if (m_rc.convert_pbox(&Bound, &rFrame, &crEdit))
		{
			rgnFrame.CreateRectRgn(rFrame.left,
										  rFrame.top,
										  rFrame.right,
										  rFrame.bottom);

			CRect crClient;
			GetClientRect(crClient);

			rgnView.CreateRectRgn(crClient.left,
										 crClient.top,
										 crClient.right,
										 crClient.bottom);

			// Create a minimal region to be the destination of the combine.
			rgnErase.CreateRectRgn(0, 0, 0, 0);

			int nResult = rgnErase.CombineRgn(&rgnView, &rgnFrame, RGN_DIFF);
			if (nResult != ERROR && nResult != NULLREGION)
			{
				SelectClipRgn(m_rc.destination_hdc, (HRGN)rgnErase.GetSafeHandle());

				m_rc.toggle(FALSE, &crClient);
				Rectangle(m_rc.destination_hdc,
					crClient.left,
					crClient.top,
					crClient.right,
					crClient.bottom);
				m_rc.toggle(TRUE, &crClient);
			}
		}
	}
	END_TRY

	::RestoreDC(m_rc.destination_hdc, nSave);
	return TRUE;

//	return CWnd::OnEraseBkgnd(pDC);
}
void CFrameEdit::OnSize(UINT nType, int cx, int cy)
{
//	od(">>> OnSize...\r\n");
	CWnd::OnSize(nType, cx, cy);

	if (m_pFrame != NULL)
	{
		ChangeView(VIEW_LAST);
	}
}

/*
// Generate the scaled view parameters.
*/

void CFrameEdit::ViewScale(PCOORD numerator, PCOORD denominator, PPNT *dimensions, PBOX *pbox)
{                               
//	od("In values: %ld, %ld -> ", numerator, denominator);

	while (numerator > 1024 || denominator > 1024)
	{
		numerator >>= 1;
		denominator >>= 1;
	}

	if (numerator == 0)
	{
		numerator++;
	}
	if (denominator == 0)
	{
		denominator++;
	}

	PCOORD big_d = 10*denominator;

	if (numerator > big_d)
	{
		numerator = big_d;
	}

//	od("Scale factors: %ld / %ld\r\n", numerator, denominator);

/* Set the scale factors. */

	m_rc.SetScaling((USHORT)numerator, (USHORT)denominator);

/* Compute the bounds of the document at the new scale. */

	pbox->x0 = 0;
	pbox->y0 = 0;
	pbox->x1 = dimensions->x;
	pbox->y1 = dimensions->y;

	m_rc.pbox_to_screen(pbox, TRUE);

//	od("Scaled doc dims: %ld, %ld, %ld, %ld\r\n", *pbox);
	pbox->x1 -= pbox->x0;
	pbox->y1 -= pbox->y0;

	pbox->x0 = pbox->y0 = 0;
}

void CFrameEdit::ChangeView(int new_view, BOOL force, PBOX_PTR area)
{
/* See if there's even anything to do. */

#if 0
	if (!force && new_view == current_view)
	{
	/* Same view; skip. */
		return;
	}

	if (new_view == VIEW_LAST)
	{
		new_view = current_view;
	}

	if (new_view == VIEW_NONE)
	{
		return;
	}
#endif

/* Get our frame bounds so we can know how big we are. */
	
	PBOX view_extent = m_pFrame->get_bound();

	PPNT dimensions;
	PBOX pbox;
	CRect client;

	dimensions.x = view_extent.x1 - view_extent.x0;
	dimensions.y = view_extent.y1 - view_extent.y0;

/* See what our client is. */

	GetEditRect(client);
//	od("***>>> Client rect: %d, %d, %d, %d <<<***\r\n", client);

/* Space a 1/4" around (theoretically) */

	m_rc.destination_x0 = 0;
	m_rc.destination_y0 = 0;

//	InflateRect(&client, -m_rc.destination_x0, -m_rc.destination_y0);

	if (client.right <= client.left)
	{
		client.right = client.left + 1;
	}
	if (client.bottom <= client.top)
	{
		client.bottom = client.top + 1;
	}

	PBOX old_source = m_rc.source_pbox;

/* Handle the appropriate view. */

	FLAGS pos_flags = 0;				// Additional position flags.

	ViewScale(1, 1, &dimensions, &pbox);
	UpdateScrolls(&client, &pbox, TRUE, TRUE);

/* Compute the source pbox from our destination client area rect. */

	m_rc.scaled_source_x0 =
	m_rc.scaled_source_y0 = 0;

	GetEditRect(client);			/* Get the now current client. */

/*
// Center the view in the client area.
*/

	pbox.x0 = 0;
	pbox.y0 = 0;
	pbox.x1 = dimensions.x;
	pbox.y1 = dimensions.y;
	m_rc.pbox_to_screen(&pbox, TRUE);

#if 1
	if (pbox.y1 - pbox.y0 < client.bottom - client.top)
	{
		m_rc.destination_y0 = (client.top
									+ client.bottom
									- (int)(pbox.y1 - pbox.y0))/2;
	}
#endif

	m_rc.destination_rect = client;
	m_rc.destination_x0 = client.left;
	m_rc.destination_y0 = client.top;

//	od("old source pbox is %ld, %ld, %ld, %ld -> ", rc.source_pbox);
	m_rc.screen_to_pbox(&client, &pbox);

//	spacing.x = -pbox.x0;
//	spacing.y = -pbox.y0;

//	pbox.x0 += view_portion.x0;
//	pbox.y0 += view_portion.y0;
//	pbox.x1 += view_portion.x0;
//	pbox.y1 += view_portion.y0;

	m_rc.source_pbox = pbox;

//	od("View set pbox to (%ld, %ld, %ld, %ld)\r\n", m_rc.source_pbox);

/* Compute view extent. */

//	view_extent.x0 = view_portion.x0-spacing.x;
//	view_extent.y0 = view_portion.y0-spacing.y;
//	view_extent.x1 = view_portion.x1+spacing.x;
//	view_extent.y1 = view_portion.y1+spacing.y;

	if (old_source.x0 == old_source.x1)
	{
		SetScrollPos(SB_HORZ, 0);
		SetScrollPos(SB_VERT, 0);
	}
#if 1
	else
	{
	/* Not first time! */
		PositionView(old_source.x0, old_source.y0,
						 old_source.x1, old_source.y1,
						 PVIEW_FLAG_set_scrolls | pos_flags);
	}
#endif

/* Make sure we're correct. */

	m_rc.scaled_source_x0 = (m_rc.source_pbox.x0 /*+spacing.x*/) * m_rc.x_resolution;
	m_rc.scaled_source_y0 = (m_rc.source_pbox.y0 /*+spacing.y*/) * m_rc.y_resolution;

	m_rc.set_redisplay_pixel();

#if 0
	od("fnl src: %ld, %ld, %ld, %ld; sclsrc:%ld, %ld; dst:%d, %d\r\n"
			"scale factor: %d/%d\r\n",
 					m_rc.source_pbox,
					m_rc.scaled_source_x0,
					m_rc.scaled_source_y0,
					m_rc.destination_x0,
					m_rc.destination_y0,
					m_rc.GetScaleNumerator(),
					m_rc.GetScaleDenominator());
#endif

/* Invalidate our whole view. */

	InvalidateRect(&m_rc.destination_rect /*, FALSE*/);
}

//
// Support for create by class name.
//

WNDPROC CFrameEdit::m_pfnSuperWndProc = NULL;

LONG CALLBACK AFX_EXPORT CFrameEdit::FirstMsgOnlyWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	/*
	// Construct the object and then attach the hWnd to it.
	*/
	
	CFrameEdit* pEdit = new CFrameEdit;
	pEdit->Attach(hWnd) ;
	
	pEdit->m_fCreatedByClassName = TRUE ;
	
	/*
	// Subclass the window by changing the window procedure to AfxWndProc
	// and pass the current message to it.
	*/
	
	::SetWindowLong(hWnd, GWL_WNDPROC, (DWORD)AfxWndProc);

#ifdef STRICT
	return ::CallWindowProc((WNDPROC)AfxWndProc, hWnd, msg, wParam, lParam);
#else
	return ::CallWindowProc((FARPROC)AfxWndProc, hWnd, msg, wParam, lParam);
#endif
}

BOOL CFrameEdit::RegisterMyClass()
{
	WNDCLASS wcls;
	
	m_pfnSuperWndProc = ::DefWindowProc;
	
	static const char szClassName[] = "MlsFrameEdit";
	
	if (::GetClassInfo(AfxGetInstanceHandle(), szClassName, &wcls))
	{
		return (wcls.lpfnWndProc == CFrameEdit::FirstMsgOnlyWndProc);
	}
	
	// JRO
  //wcls.style = CS_DBLCLKS;
  wcls.style = CS_GLOBALCLASS | CS_DBLCLKS;
	wcls.cbClsExtra = 0;
	wcls.cbWndExtra = 0;
	wcls.lpfnWndProc = CFrameEdit::FirstMsgOnlyWndProc;
	wcls.hIcon = NULL;
	wcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcls.hInstance = AfxGetInstanceHandle();
	wcls.hbrBackground = (HBRUSH)(COLOR_BACKGROUND+1);
	wcls.lpszMenuName = NULL;
	wcls.lpszClassName = szClassName;
	
	return (RegisterClass(&wcls) != 0);
}

void CFrameEdit::PostNcDestroy()
{
	if (m_fCreatedByClassName)
	{
		delete this;
	}
}

/*
// Handle a scroll event.
*/

void CFrameEdit::OnScroll(int nBar, UINT nSBCode, UINT nPos)
{
	BOOL bHorz = (nBar == SB_HORZ);
	int zOrig, z;   // z = x or y depending on 'nBar'
	int zMin, zMax;
	int nRound = 0;

	zOrig = z = GetScrollPos(nBar);
	GetScrollRange(nBar, &zMin, &zMax);

	ASSERT(zMin == 0);

	if (zMax <= 0)
	{
		TRACE0("Warning: no scroll range - ignoring scroll message\n");
		ASSERT(z == 0);     // must be at top
		return;
	}

	switch (nSBCode)
	{
		case SB_TOP:
		{
			z = 0;
			break;
		}

		case SB_BOTTOM:
		{
			z = zMax;
			break;
		}
		
		case SB_LINEUP:
		{
			z -= 10;
			nRound = -1;
			break;
		}

		case SB_LINEDOWN:
		{
			z += 10;
			nRound = 1;
			break;
		}

		case SB_PAGEUP:
		{
			z -= 100;
			nRound = -1;
			break;
		}

		case SB_PAGEDOWN:
		{
			z += 100;
			nRound = 1;
			break;
		}

#if 1
		case SB_THUMBTRACK:
		{
			z = nPos;
			break;
		}
#endif

		default:        // ignore other notifications
		{
			return;
		}
	}

	if (z < 0)
	{
		z = 0;
	}
	else if (z > zMax)
	{
		z = zMax;
	}

	if (z != zOrig)
	{
		PPNT dimensions;

		PCOORD new_x = m_rc.source_pbox.x0;
		PCOORD view_width = m_rc.source_pbox.x1 - new_x;
		PCOORD new_y = m_rc.source_pbox.y0;
		PCOORD view_height = m_rc.source_pbox.y1 - new_y;

//		od("view extent: %ld, %ld, %ld, %ld; spacing: %u, %d\r\n",
// 						view_extent, spacing);

		PBOX view_extent = m_pFrame->get_bound();

		dimensions.x = view_extent.x1 - view_extent.x0;
		dimensions.y = view_extent.y1 - view_extent.y0;

		if (bHorz)
		{
			PCOORD full_range = dimensions.x - view_width - 1;
			new_x = scroll_scale_pcoord(z, full_range, 1000-1) + view_extent.x0;
		}
		else
		{
			PCOORD full_range = dimensions.y - view_height - 1;
			new_y = scroll_scale_pcoord(z, full_range, 1000-1) + view_extent.y0;
		}

//		od("Scroll %d; (%ld, %ld)\r\n", z, new_x, new_y);

//		SetScrollPos(nBar, z);

		PositionView(new_x, new_y, 
						new_x + view_width, new_y + view_height,
						PVIEW_FLAG_scroll | PVIEW_FLAG_set_scrolls,
						bHorz ? nRound : 0,
						bHorz ? 0 : nRound);
	}
}

void CFrameEdit::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	OnScroll(SB_HORZ, nSBCode, nPos);
}

void CFrameEdit::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	OnScroll(SB_VERT, nSBCode, nPos);
}

BOOL CFrameEdit::UpdateUndoTyping(CTxp* pTxp, CHARACTER c, int nCount)
{
#if 0
   /* See if there is an existing typing command that we can extend. */

	CCmdAddText* pCommand = (CCmdAddText*)m_CommandList.UndoCommand();

	if (pCommand != NULL
		 && pCommand->ID() == IDCmd_TypeText
		 && pCommand->UpdateTyping(pTxp, c, nCount))
	{
		return TRUE;
	}

/* Doesn't exist yet. Create one. */

	pCommand = new CCmdAddText;

/* Save state before the typing begins. */

	if (!pCommand->Before(pTxp, NULL))
	{
		delete pCommand;
		return FALSE;
	}

/* Add this typing into the undo command. */

	pCommand->UpdateTyping(pTxp, c, nCount);

/* Add the undo/redo command. */

	m_CommandList.AddCommand(pCommand);
#endif

	return TRUE;
}

BOOL CFrameEdit::UpdateUndoDelete(CTxp* pTxp, int nCount)
{
#if 0
/* Fixup the txp to have a selection. */

	if (!pTxp->SomethingSelected())
	{
		pTxp->SelectText(pTxp->Index(), pTxp->Index()+nCount);
		nCount = 1;
		if (!pTxp->SomethingSelected())
		{
		/* Nothing to do. */
			return FALSE;
		}
	}

/* See if there is an existing delete command that we can extend. */

	CCmdDeleteText* pCommand = (CCmdDeleteText*)m_CommandList.UndoCommand();

	if (pCommand != NULL
		 && pCommand->ID() == IDCmd_DeleteText
		 && pCommand->UpdateDelete(pTxp, nCount))
	{
		return TRUE;
	}

/* Doesn't exist yet. Create one. */

	pCommand = new CCmdDeleteText;
	pCommand->SetStuff(pTxp, NULL);

/* Add this backspace into the undo command. */

	pCommand->UpdateDelete(pTxp, nCount);

/* Add the undo/redo command. */

	m_CommandList.AddCommand(pCommand);
#endif

	return TRUE;
}

BOOL CFrameEdit::UpdateUndoBackspace(CTxp* pTxp, int nCount)
{
#if 0
/* Fixup the txp to have a selection. */

	if (!pTxp->SomethingSelected())
	{
		pTxp->SelectText(pTxp->Index()-nCount, pTxp->Index());
		nCount = 1;
		if (!pTxp->SomethingSelected())
		{
		/* Nothing to do. */
			return FALSE;
		}
	}

/* See if there is an existing backspace command that we can extend. */

	CCmdBackspace* pCommand = (CCmdBackspace*)m_CommandList.UndoCommand();

	if (pCommand != NULL
		 && pCommand->ID() == IDCmd_Backspace
		 && pCommand->UpdateBackspace(pTxp, nCount))
	{
		return TRUE;
	}

/* Doesn't exist yet. Create one. */

	pCommand = new CCmdBackspace;
	pCommand->SetStuff(pTxp, NULL);

/* Add this backspace into the undo command. */

	pCommand->UpdateBackspace(pTxp, nCount);

/* Add the undo/redo command. */

	m_CommandList.AddCommand(pCommand);
#endif
	return TRUE;
}

