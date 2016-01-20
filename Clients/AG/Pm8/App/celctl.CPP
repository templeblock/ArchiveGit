/*
// $Workfile: celctl.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/celctl.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 4     5/29/98 6:38p Jayn
// Insert column, etc.
// 
// 3     5/28/98 4:10p Jayn
// More cool table stuff (cell formatting, etc.)
// 
// 2     5/26/98 2:38p Jayn
// New table code.
// 
// 1     5/26/98 1:33p Jayn
*/ 

#include "stdafx.h"
#include "pmw.h"
#include "pmwdoc.h"

#include "celctl.h"
#include "tblobj.h"
#include "util.h"
#include "utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCellOutlineControl

WNDPROC CCellOutlineControl::m_pfnSuperWndProc = NULL;

LONG CALLBACK AFX_EXPORT CCellOutlineControl::FirstMsgOnlyWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	/*
	// Construct the object and then attach the hWnd to it.
	*/

	CCellOutlineControl* pCellOutlineControl = new CCellOutlineControl;
	pCellOutlineControl->Attach(hWnd);

	pCellOutlineControl->m_fCreatedByClassName = TRUE;
	
	/*
	// Subclass the window by changing the window procedure to AfxWndProc
	// and pass the current message to it.
	*/
	
	::SetWindowLong(hWnd, GWL_WNDPROC, (DWORD)AfxWndProc);

	return ::CallWindowProc((WNDPROC)AfxWndProc, hWnd, msg, wParam, lParam);
}

BOOL CCellOutlineControl::RegisterMyClass()
{
	WNDCLASS wcls;
	
	m_pfnSuperWndProc = ::DefWindowProc;
	
	static const char szClassName[] = "TableCellOutlineControl";
	
	if (::GetClassInfo(AfxGetInstanceHandle(), szClassName, &wcls))
	{
		return (wcls.lpfnWndProc == CCellOutlineControl::FirstMsgOnlyWndProc);
	}
	
	wcls.style = CS_GLOBALCLASS | CS_DBLCLKS;
	wcls.cbClsExtra = 0;
	wcls.cbWndExtra = 0;
	wcls.lpfnWndProc = CCellOutlineControl::FirstMsgOnlyWndProc;
	wcls.hIcon = NULL;
	wcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcls.hInstance = AfxGetInstanceHandle();
//	wcls.hbrBackground = (HBRUSH)(COLOR_BACKGROUND+1);
	wcls.hbrBackground = (HBRUSH)::GetStockObject(NULL_BRUSH);
	wcls.lpszMenuName = NULL;
	wcls.lpszClassName = szClassName;
	
	return (RegisterClass(&wcls) != 0);
}

void CCellOutlineControl::PostNcDestroy()
{
	if (m_fCreatedByClassName)
	{
		delete this;
	}
}

CCellOutlineControl::CCellOutlineControl()
{
	m_fCreatedByClassName = FALSE;

	m_pFormatting = NULL;

//	m_nSelectedSegment = SEGMENT_None;
	m_nSelectedSegment = SEGMENT_Left;
}

CCellOutlineControl::~CCellOutlineControl()
{
}

void CCellOutlineControl::Init(CTableCellFormatting* pFormatting)
{
	m_pFormatting = pFormatting;

	// Set our rectangles for drawing.

	// First, set the client rect.
	// We want this to be square.
	CRect crClient;
	GetClientRect(crClient);
	ASSERT(crClient.left == 0);
	ASSERT(crClient.top == 0);
	int nSize = (crClient.right < crClient.bottom) ? crClient.right : crClient.bottom;
	m_crClient.SetRect(0, 0, nSize, nSize);

	// Compute the rectangles for hit tests.
	CRect crHit = m_crClient;
	crHit.InflateRect(-4, -4);
	ASSERT(crHit.Width() == crHit.Height());
	int n = crHit.Width()/6;

	crHit.InflateRect(-n, -n);

	int nHitWidth = 8;
	m_crSegment[SEGMENT_Left].left = crHit.left - nHitWidth/2;
	m_crSegment[SEGMENT_Left].right = m_crSegment[SEGMENT_Left].left+nHitWidth;
	m_crSegment[SEGMENT_Left].top = crHit.top;
	m_crSegment[SEGMENT_Left].bottom = crHit.bottom;

	m_crSegment[SEGMENT_Right].left = crHit.right - nHitWidth/2;
	m_crSegment[SEGMENT_Right].right = m_crSegment[SEGMENT_Right].left+nHitWidth;
	m_crSegment[SEGMENT_Right].top = crHit.top;
	m_crSegment[SEGMENT_Right].bottom = crHit.bottom;

	m_crSegment[SEGMENT_Top].top = crHit.top - nHitWidth/2;
	m_crSegment[SEGMENT_Top].bottom = m_crSegment[SEGMENT_Top].top+nHitWidth;
	m_crSegment[SEGMENT_Top].left = crHit.left;
	m_crSegment[SEGMENT_Top].right = crHit.right;

	m_crSegment[SEGMENT_Bottom].top = crHit.bottom - nHitWidth/2;
	m_crSegment[SEGMENT_Bottom].bottom = m_crSegment[SEGMENT_Bottom].top+nHitWidth;
	m_crSegment[SEGMENT_Bottom].left = crHit.left;
	m_crSegment[SEGMENT_Bottom].right = crHit.right;

	if (m_pFormatting->m_nType & CTableCellFormatting::MultipleDown)
	{
		m_crSegment[SEGMENT_InnerHorizontal].top = (crHit.top+crHit.bottom)/2 - nHitWidth/2;
		m_crSegment[SEGMENT_InnerHorizontal].bottom = m_crSegment[SEGMENT_InnerHorizontal].top+nHitWidth;
		m_crSegment[SEGMENT_InnerHorizontal].left = crHit.left;
		m_crSegment[SEGMENT_InnerHorizontal].right = crHit.right;
	}
	else
	{
		m_crSegment[SEGMENT_InnerHorizontal].SetRectEmpty();
	}

	if (m_pFormatting->m_nType & CTableCellFormatting::MultipleAcross)
	{
		m_crSegment[SEGMENT_InnerVertical].left = (crHit.left+crHit.right)/2 - nHitWidth/2;
		m_crSegment[SEGMENT_InnerVertical].right = m_crSegment[SEGMENT_InnerVertical].left+nHitWidth;
		m_crSegment[SEGMENT_InnerVertical].top = crHit.top;
		m_crSegment[SEGMENT_InnerVertical].bottom = crHit.bottom;
	}
	else
	{
		m_crSegment[SEGMENT_InnerVertical].SetRectEmpty();
	}

	CreateMarkerBitmaps();
}

void CCellOutlineControl::SetFillColor(COLOR FillColor)
{
	m_pFormatting->m_FillColor = FillColor;
	Invalidate();
}

void CCellOutlineControl::SetLineColor(COLOR LineColor)
{
	CTableBorderSegment& Segment = m_pFormatting->m_Segment[m_nSelectedSegment];
	Segment.m_Color = LineColor;
	Invalidate();
}

void CCellOutlineControl::SetLineWidth(int nWidth)
{
	CTableBorderSegment& Segment = m_pFormatting->m_Segment[m_nSelectedSegment];
	Segment.m_bWidth = (BYTE)nWidth;
	Invalidate();
}

int CCellOutlineControl::GetLineWidth(void)
{
	return (int)(short)m_pFormatting->m_Segment[m_nSelectedSegment].m_bWidth;
}

BEGIN_MESSAGE_MAP(CCellOutlineControl, CWnd)
	//{{AFX_MSG_MAP(CCellOutlineControl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCellOutlineControl message handlers

void CCellOutlineControl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// Fill the background to white.
	dc.PatBlt(m_crClient.left, m_crClient.top,
				 m_crClient.Width(), m_crClient.Height(),
				 WHITENESS);

	// Set up a redisplay context to use for generating widths, etc.
	// Most of what we need is set default by the rc constructor.
	RedisplayContext rc;
	rc.set_info(dc.GetSafeHdc());
	rc.source_pbox.x0 = 0;
	rc.source_pbox.y0 = 0;
	rc.scaled_source_x0 = 0;
	rc.scaled_source_y0 = 0;
	rc.destination_x0 = 0;
	rc.destination_y0 = 0;
	rc.x_resolution = dc.GetDeviceCaps(LOGPIXELSX);
	rc.y_resolution = dc.GetDeviceCaps(LOGPIXELSY);

	CRect crDraw = m_crClient;
	crDraw.InflateRect(-4, -4);
	ASSERT(crDraw.Width() == crDraw.Height());
	int n = crDraw.Width()/6;

	// Draw the fill color.
	COLOR FillColor = m_pFormatting->m_FillColor;
	if (FillColor != TRANSPARENT_COLOR
		 && FillColor != UNDEFINED_COLOR)
	{
		CRect crFill = crDraw;
		crFill.InflateRect(-n, -n);
		CBrush cbFill;
		if (cbFill.CreateSolidBrush(COLORREF_FROM_COLOR(FillColor)))
		{
			dc.FillRect(crFill, &cbFill);
		}
	}

	// Draw the corners.
	CPen cpLine;
	if (cpLine.CreatePen(PS_SOLID, 1, RGB(192, 192, 192)))
	{
		CPen* pOldPen = dc.SelectObject(&cpLine);
		if (pOldPen != NULL)
		{
			// Upper left.
			dc.MoveTo(crDraw.left+n, crDraw.top);
			dc.LineTo(crDraw.left+n, crDraw.top+n);
			dc.LineTo(crDraw.left, crDraw.top+n);

			// Upper right.
			dc.MoveTo(crDraw.right-n, crDraw.top);
			dc.LineTo(crDraw.right-n, crDraw.top+n);
			dc.LineTo(crDraw.right, crDraw.top+n);

			// Bottom left.
			dc.MoveTo(crDraw.left+n, crDraw.bottom);
			dc.LineTo(crDraw.left+n, crDraw.bottom-n);
			dc.LineTo(crDraw.left, crDraw.bottom-n);

			// Bottom right.
			dc.MoveTo(crDraw.right-n, crDraw.bottom);
			dc.LineTo(crDraw.right-n, crDraw.bottom-n);
			dc.LineTo(crDraw.right, crDraw.bottom-n);

			if (m_pFormatting->m_nType & CTableCellFormatting::MultipleDown)
			{
				// Middle horizontal
				int y = (crDraw.top+crDraw.bottom)/2;
				dc.MoveTo(crDraw.left+n, y);
				dc.LineTo(crDraw.left, y);
				dc.MoveTo(crDraw.right-n, y);
				dc.LineTo(crDraw.right, y);
			}

			if (m_pFormatting->m_nType & CTableCellFormatting::MultipleAcross)
			{
				// Middle vertical
				int x = (crDraw.left+crDraw.right)/2;
				dc.MoveTo(x, crDraw.top+n);
				dc.LineTo(x, crDraw.top);
				dc.MoveTo(x, crDraw.bottom-n);
				dc.LineTo(x, crDraw.bottom);
			}

			dc.SelectObject(pOldPen);
		}
	}

	//
	// Draw each of the four segments.
	//

	int nLeftWidth = m_pFormatting->m_Segment[SEGMENT_Left].GetWidth(&rc);
	int nTopWidth = m_pFormatting->m_Segment[SEGMENT_Top].GetWidth(&rc);
	int nRightWidth = m_pFormatting->m_Segment[SEGMENT_Right].GetWidth(&rc);
	int nBottomWidth = m_pFormatting->m_Segment[SEGMENT_Bottom].GetWidth(&rc);
	nLeftWidth /= 2;						// Odd rounds smaller
	nTopWidth /= 2;						// Odd rounds smaller
	nRightWidth -= nRightWidth/2;		// Odd rounds larger
	nBottomWidth -= nBottomWidth/2;	// Odd rounds larger

	// Draw the top segment.
	DrawSegment(&dc,
					&rc,
					SEGMENT_Top,
					CPoint(crDraw.left+n, crDraw.top+n),
					CPoint(crDraw.right-n, crDraw.top+n),
					CPoint(nLeftWidth, 0),
					CPoint(nRightWidth, 0));

	// Draw the bottom segment.
	DrawSegment(&dc,
					&rc,
					SEGMENT_Bottom,
					CPoint(crDraw.left+n, crDraw.bottom-n),
					CPoint(crDraw.right-n, crDraw.bottom-n),
					CPoint(nLeftWidth, 0),
					CPoint(nRightWidth, 0));

	if (m_pFormatting->m_nType & CTableCellFormatting::MultipleDown)
	{
		// Draw the inner horizontal segment.
		int nMiddle = (crDraw.top + crDraw.bottom)/2;
		DrawSegment(&dc,
						&rc,
						SEGMENT_InnerHorizontal,
						CPoint(crDraw.left+n, nMiddle),
						CPoint(crDraw.right-n, nMiddle),
						CPoint(nLeftWidth, 0),
						CPoint(nRightWidth, 0));

	}

	// Draw the left segment.
	DrawSegment(&dc,
					&rc,
					SEGMENT_Left,
					CPoint(crDraw.left+n, crDraw.top+n),
					CPoint(crDraw.left+n, crDraw.bottom-n),
					CPoint(0, nTopWidth),
					CPoint(0, nBottomWidth));

	// Draw the right segment.
	DrawSegment(&dc,
					&rc,
					SEGMENT_Right,
					CPoint(crDraw.right-n, crDraw.top+n),
					CPoint(crDraw.right-n, crDraw.bottom-n),
					CPoint(0, nTopWidth),
					CPoint(0, nBottomWidth));

	if (m_pFormatting->m_nType & CTableCellFormatting::MultipleAcross)
	{
		// Draw the inner vertical segment.
		int nMiddle = (crDraw.left + crDraw.right)/2;
		DrawSegment(&dc,
						&rc,
						SEGMENT_InnerVertical,
						CPoint(nMiddle, crDraw.top+n),
						CPoint(nMiddle, crDraw.bottom-n),
						CPoint(0, nTopWidth),
						CPoint(0, nBottomWidth));
	}
	// Toggle the mark for the selected segment.
	ToggleSelectedSegment(&dc);
}

void CCellOutlineControl::OnLButtonDown(UINT nFlags, CPoint point)
{
	for (int nSegment = 0; nSegment < NUM_SEGMENTS; nSegment++)
	{
		if (m_crSegment[nSegment].PtInRect(point))
		{
			if (m_nSelectedSegment != nSegment)
			{
				CDC* pDC = GetDC();
				ToggleSelectedSegment(pDC);
				m_nSelectedSegment = nSegment;
				ToggleSelectedSegment(pDC);
				ReleaseDC(pDC);
				Util::SendWmCommand(GetParent(), GetDlgCtrlID(), this, BN_CLICKED);
				break;
			}
		}
	}
}

void CCellOutlineControl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}

void CCellOutlineControl::DrawSegment(CDC* pDC, RedisplayContext* rc, int nSegment, CPoint p0, CPoint p1, CPoint p0Extend, CPoint p1Extend)
{
	CPen cpEdge;
	CBrush cbEdge;
	int nWidth;

	if (m_pFormatting->m_Segment[nSegment].SetupToDraw(rc, cpEdge, cbEdge, nWidth))
	{
		if (((HBRUSH)cbEdge) != NULL)
		{
			CRect crFill;
			p0.x -= p0Extend.x;
			p0.y -= p0Extend.y;
			p1.x += p1Extend.x;
			p1.y += p1Extend.y;
			if (p0.x == p1.x)
			{
				crFill.left = p0.x - nWidth/2;
				crFill.right = crFill.left + nWidth;
				crFill.top = p0.y;
				crFill.bottom = p1.y;
			}
			else
			{
				ASSERT(p0.y == p1.y);
				crFill.top = p0.y - nWidth/2;
				crFill.bottom = crFill.top + nWidth;
				crFill.left = p0.x;
				crFill.right = p1.x;
			}
			pDC->FillRect(crFill, &cbEdge);
		}
		else if (((HPEN)cpEdge) != NULL)
		{
			CPen* pOldPen = pDC->SelectObject(&cpEdge);
			if (pOldPen != NULL)
			{
				pDC->MoveTo(p0.x, p0.y);
				pDC->LineTo(p1.x, p1.y);
				pDC->SelectObject(pOldPen);
			}
		}
	}
}

//
// Toggle the marker for the selected segment.
//

void CCellOutlineControl::ToggleSelectedSegment(CDC* pDC)
{
	ASSERT(m_crClient.Width() == m_crClient.Height());
	CRect crDraw = m_crClient;
	crDraw.InflateRect(-4, -4);
	int nGap = crDraw.Width()/6;
	int nStandoff = 6;

	crDraw.InflateRect(-nGap, -nGap);

	int nMarker[2];
	CPoint cpMarker[2];

	switch (m_nSelectedSegment) 
	{
		case SEGMENT_None:
		{
			// Nothing is selected.
			return;
		}
		case SEGMENT_Left:
		{
			nMarker[0] = MARKER_PointsDown;
			cpMarker[0].x = crDraw.left;
			cpMarker[0].y = crDraw.top - nStandoff;

			nMarker[1] = MARKER_PointsUp;
			cpMarker[1].x = crDraw.left;
			cpMarker[1].y = crDraw.bottom + nStandoff;

			break;
		}
		case SEGMENT_Top:
		{
			nMarker[0] = MARKER_PointsRight;
			cpMarker[0].x = crDraw.left - nStandoff;
			cpMarker[0].y = crDraw.top;

			nMarker[1] = MARKER_PointsLeft;
			cpMarker[1].x = crDraw.right + nStandoff;
			cpMarker[1].y = crDraw.top;

			break;
		}
		case SEGMENT_Right:
		{
			nMarker[0] = MARKER_PointsDown;
			cpMarker[0].x = crDraw.right;
			cpMarker[0].y = crDraw.top - nStandoff;

			nMarker[1] = MARKER_PointsUp;
			cpMarker[1].x = crDraw.right;
			cpMarker[1].y = crDraw.bottom + nStandoff;

			break;
		}
		case SEGMENT_Bottom:
		{
			nMarker[0] = MARKER_PointsRight;
			cpMarker[0].x = crDraw.left - nStandoff;
			cpMarker[0].y = crDraw.bottom;

			nMarker[1] = MARKER_PointsLeft;
			cpMarker[1].x = crDraw.right + nStandoff;
			cpMarker[1].y = crDraw.bottom;

			break;
		}
		case SEGMENT_InnerHorizontal:
		{
			nMarker[0] = MARKER_PointsRight;
			cpMarker[0].x = crDraw.left - nStandoff;
			cpMarker[0].y = (crDraw.top + crDraw.bottom)/2;

			nMarker[1] = MARKER_PointsLeft;
			cpMarker[1].x = crDraw.right + nStandoff;
			cpMarker[1].y = cpMarker[0].y;

			break;
		}
		case SEGMENT_InnerVertical:
		{
			nMarker[0] = MARKER_PointsDown;
			cpMarker[0].x = (crDraw.left + crDraw.right)/2;
			cpMarker[0].y = crDraw.top - nStandoff;

			nMarker[1] = MARKER_PointsUp;
			cpMarker[1].x = cpMarker[0].x;
			cpMarker[1].y = crDraw.bottom + nStandoff;

			break;
		}
		default:
		{
			// Invalid "selected segment".
			ASSERT(FALSE);
			return;
		}
	}

	COLORREF OldTextColor = pDC->SetTextColor(RGB(0, 0, 0));
	COLORREF OldBkColor = pDC->SetBkColor(RGB(255, 255, 255));

	CDC dcSource;
	if (dcSource.CreateCompatibleDC(NULL))
	{
		for (int i = 0; i < 2; i++)
		{
			CMarkerBitmap* pMarker = m_cbmMarker+nMarker[i];
			if (pMarker->GetSafeHandle() != NULL)
			{
				CBitmap* pOldBitmap = dcSource.SelectObject(pMarker);
				if (pOldBitmap != NULL)
				{
					// Do the inverting BitBlt.
					pDC->BitBlt(cpMarker[i].x-pMarker->m_cpOrigin.x, cpMarker[i].y-pMarker->m_cpOrigin.y,
									pMarker->m_czBitmap.cx, pMarker->m_czBitmap.cy,
									&dcSource,
									0, 0, SRCINVERT);
					// Re-select the old bitmap.
					dcSource.SelectObject(pOldBitmap);
				}
			}
		}
	}

	pDC->SetBkColor(OldBkColor);
	pDC->SetTextColor(OldTextColor);
}

BOOL CCellOutlineControl::CreateMarkerBitmaps(void)
{
	int nArrowSize = 7;

	// Left pointer.
	if (!CreateMarkerBitmap(MARKER_PointsLeft,
									IDB_LEFTMARK,
									CSize(nArrowSize, 2*nArrowSize-1),
									CPoint(0, nArrowSize-1)))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	// Right pointer.
	if (!CreateMarkerBitmap(MARKER_PointsRight,
									IDB_RIGHTMARK,
									CSize(nArrowSize, 2*nArrowSize-1),
									CPoint(nArrowSize, nArrowSize-1)))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	// Down pointer.
	if (!CreateMarkerBitmap(MARKER_PointsDown,
									IDB_DOWNMARK,
									CSize(2*nArrowSize-1, nArrowSize),
									CPoint(nArrowSize-1, nArrowSize)))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	// Up pointer.
	if (!CreateMarkerBitmap(MARKER_PointsUp,
									IDB_UPMARK,
									CSize(2*nArrowSize-1, nArrowSize),
									CPoint(nArrowSize-1, 0)))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	return TRUE;
}

BOOL CCellOutlineControl::CreateMarkerBitmap(int nMarker, UINT idBitmap, CSize czBitmap, CPoint cpOrigin)
{
	BOOL fSuccess = FALSE;
	CMarkerBitmap& cbmMarker = m_cbmMarker[nMarker];
	ASSERT(cbmMarker.GetSafeHandle() == NULL);

	cbmMarker.m_czBitmap = czBitmap;
	cbmMarker.m_cpOrigin = cpOrigin;

	return cbmMarker.LoadBitmap(idBitmap);
}

