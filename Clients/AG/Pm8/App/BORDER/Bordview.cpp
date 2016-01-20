// Bordview.cpp : implementation file
//

#include "stdafx.h"
#include "border.h"
#include "Bordview.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define HANDLE_KNOBS

// Some display constants
const BORDER_W = 20;
const BORDER_H = 20;

const VBAR_W = 9;
const VBAR_H = 14;
const HBAR_W = 14;
const HBAR_H = 9;

/////////////////////////////////////////////////////////////////////////////
// CGridHandle

CGridHandle::CGridHandle(CDC* pMemDC, CBitmap* pBitmap, int nGridIndex) :
	HANDLE_W(2)
{
	// Members
	m_nGridIndex = nGridIndex;
	m_WallLimit = (nGridIndex == 1) ? LOWER_WALL : UPPER_WALL;

	m_pMemDC = pMemDC;
	m_pBitmap = pBitmap;

	// Create the pen
	m_pPen = new CPen(PS_DOT, 1, RGB(255, 255, 255));

}

CGridHandle::~CGridHandle()
{
	delete m_pPen;
}

void CGridHandle::VerifyHandleLimits()
{
	// Check for limits
	int nLevel = m_nLevel;
	if (m_WallLimit == LOWER_WALL)
	{
		if (nLevel > *m_pUpperLimit - 1)
		{
			nLevel = *m_pUpperLimit - 1;
		}
		if (nLevel < *m_pLowerLimit + 1)
		{
			nLevel = *m_pLowerLimit + 1;
		}
	}
	else
	{
		if (nLevel < *m_pLowerLimit + 1)
		{
			nLevel = *m_pLowerLimit + 1;
		}
		if (nLevel > *m_pUpperLimit - 1)
		{
			nLevel = *m_pUpperLimit - 1;
		}
	}
	m_nLevel = nLevel;
}

void CGridHandle::ComputeHandleEdge(int& nValue, int nThresh)
{
	int nFromUpper, nFromLower;

	nFromUpper = *m_pUpperLimit - nValue;
	nFromLower = nValue - *m_pLowerLimit;
	if (nFromUpper < nThresh)
	{
		nValue -= (nThresh - nFromUpper);
	}
	if (nFromLower < nThresh)
	{
		nValue += (nThresh - nFromLower);
	}

}

/////////////////////////////////////////////////////////////////////////////
// CHGridHandle
// Handles horizontal handles

void CHGridHandle::MoveHandle(int nHMove, int nVMove, BOOL bUseSym)
{
	// Move the level up/down
	m_nLevel += nVMove;
	VerifyHandleLimits();
	if (m_pSymmetryHandle && bUseSym)
	{
		m_pSymmetryHandle->MoveHandle(0, -nVMove, FALSE);
	}
}

BOOL CHGridHandle::AdjustSymmetry(RECTAliasT& rctLimits, int& nLevelOffset)
{
	if (m_pSymmetryHandle)
	{
		LONG nSymLevel;
		BOOL bSwitchHandles;
		if (m_WallLimit == LOWER_WALL)
		{
			nSymLevel = *rctLimits.bottom - (m_nLevel - *rctLimits.top);
			bSwitchHandles = (nSymLevel < m_nLevel);
		}
		else
		{
			nSymLevel = *rctLimits.top + (*rctLimits.bottom - m_nLevel);
			bSwitchHandles = (nSymLevel > m_nLevel);
		}

		// May need to call for other handle in case of cross-over
		if (bSwitchHandles)
		{
			m_pSymmetryHandle->AdjustSymmetry(rctLimits, nLevelOffset);
			nLevelOffset = m_nLevel - m_nLastLevel;
			return (FALSE);
		}
		else
		{
			m_pSymmetryHandle->SetLevel(nSymLevel, FALSE);
		}
	}

	return (TRUE);
}

void CHGridHandle::DrawHandleLine(CDC* pDC, BOOL bDragging)
{
	// Draw connecting line
	BOOL bDrawLine = !bDragging;
	CPen* pOldPen = pDC->SelectObject(m_pPen);
	if (pOldPen != NULL)
	{
		pDC->SetROP2(R2_XORPEN);

		// Drawing adjustments
		int nDrawLevel = m_nLevel;
		int nDrawLastLevel = m_nLastLevel;
		if (m_nGridIndex == 1)
		{
			nDrawLevel--;
			nDrawLastLevel--;
		}

		// If level has changed, erase the last
		if (m_nLevel != m_nLastLevel)
		{
			pDC->MoveTo(m_nEnd1, nDrawLastLevel);
			pDC->LineTo(m_nEnd2, nDrawLastLevel);
			m_nLastLevel = m_nLevel;
			bDrawLine = TRUE;
		}

		if (bDrawLine)
		{
			pDC->MoveTo(m_nEnd1, nDrawLevel);
			pDC->LineTo(m_nEnd2, nDrawLevel);
		}
		pDC->SelectObject(pOldPen);
	}
}

void CHGridHandle::DrawHandleKnobs(CDC* pDC, int nSide, int nLevelOffset)
{
	// Draw 2 end points
	CBitmap* pOldBitmap = m_pMemDC->SelectObject(m_pBitmap);
	if (pOldBitmap != NULL)
	{
		// Compute nTopY value. If handle is coming close to a limit,
		// halt movement
		int nTopY = m_nLevel;
		CGridHandle::ComputeHandleEdge(nTopY, HBAR_H/2);
		nTopY -= (nLevelOffset + HBAR_H/2); 

		// Draw 1 knob
		if (nSide == 0)
		{
			pDC->BitBlt(
				0, nTopY,
				HBAR_W, HBAR_H,
				m_pMemDC, 
				0, 0,
				SRCCOPY);
		}
		else
		{
			pDC->BitBlt(
				0, nTopY,
				HBAR_W, HBAR_H,
				m_pMemDC, 
				0, 0,
				SRCCOPY);
		}
		m_pMemDC->SelectObject(pOldBitmap);
	}
}

BOOL CHGridHandle::PointInHandle(CPoint& point, int& nKnob)
{
	// Build a rectangle, and check if the point
	// is in there
	CRect rctHandleBounds(m_nEnd1, m_nLevel - HANDLE_W, m_nEnd2, m_nLevel + HANDLE_W);
	CRect rctKnob1Bounds(m_nEnd1 - HBAR_W, m_nLevel - HANDLE_W, m_nEnd1, m_nLevel + HANDLE_W);
	CRect rctKnob2Bounds(m_nEnd2, m_nLevel - HANDLE_W, m_nEnd2 + HBAR_W, m_nLevel + HANDLE_W);

	// Check for main handle
	if (rctHandleBounds.PtInRect(point))
	{
		return (TRUE);
	}

#ifdef HANDLE_KNOBS
	// Check for knobs
	if (rctKnob1Bounds.PtInRect(point))
	{
		nKnob = 0;
		return (TRUE);
	}
	if (rctKnob2Bounds.PtInRect(point))
	{
		nKnob = 1;
		return (TRUE);
	}
#endif

	return (FALSE);

}

/////////////////////////////////////////////////////////////////////////////
// CVGridHandle
// Handles horizontal handles

void CVGridHandle::MoveHandle(int nHMove, int nVMove, BOOL bUseSym)
{
	// Move the level left/right
	m_nLevel += nHMove;
	VerifyHandleLimits();
	if (m_pSymmetryHandle && bUseSym)
	{
		m_pSymmetryHandle->MoveHandle(-nHMove, 0, FALSE);
	}
}

BOOL CVGridHandle::AdjustSymmetry(RECTAliasT& rctLimits, int& nLevelOffset)
{
	if (m_pSymmetryHandle)
	{
		LONG nSymLevel;
		BOOL bSwitchHandles;
		if (m_WallLimit == LOWER_WALL)
		{
			nSymLevel = *rctLimits.right - (m_nLevel - *rctLimits.left);
			bSwitchHandles = (nSymLevel < m_nLevel);
		}
		else
		{
			nSymLevel = *rctLimits.left + (*rctLimits.right - m_nLevel);
			bSwitchHandles = (nSymLevel > m_nLevel);
		}

		// May need to call for other handle in case of cross-over
		if (bSwitchHandles)
		{
			m_pSymmetryHandle->AdjustSymmetry(rctLimits, nLevelOffset);
			nLevelOffset = m_nLevel - m_nLastLevel;
			return (FALSE);
		}
		else
		{
			m_pSymmetryHandle->SetLevel(nSymLevel, FALSE);
		}
	}

	return (TRUE);
}

void CVGridHandle::DrawHandleLine(CDC* pDC, BOOL bDragging)
{
	// Draw connecting line
	BOOL bDrawLine = !bDragging;
	CPen* pOldPen = pDC->SelectObject(m_pPen);
	if (pOldPen != NULL)
	{
		pDC->SetROP2(R2_XORPEN);

		// Drawing adjustments
		int nDrawLevel = m_nLevel, nDrawLastLevel = m_nLastLevel;
		if (m_nGridIndex == 1)
		{
			nDrawLevel--;
			nDrawLastLevel--;
		}

		// If level has changed, erase the last
		if (m_nLevel != m_nLastLevel)
		{
			pDC->MoveTo(nDrawLastLevel, m_nEnd1);
			pDC->LineTo(nDrawLastLevel, m_nEnd2);
			m_nLastLevel = m_nLevel;
			bDrawLine = TRUE;
		}

		if (bDrawLine)
		{
			pDC->MoveTo(nDrawLevel, m_nEnd1);
			pDC->LineTo(nDrawLevel, m_nEnd2);
		}
		pDC->SelectObject(pOldPen);
	}
}

void CVGridHandle::DrawHandleKnobs(CDC* pDC, int nSide, int nLevelOffset)
{
	// Draw 2 end points
	CBitmap* pOldBitmap = m_pMemDC->SelectObject(m_pBitmap);
	if (pOldBitmap != NULL)
	{
		// Compute nLeftX value. If handle is coming close to a limit,
		// halt movement
		int nLeftX = m_nLevel;
		CGridHandle::ComputeHandleEdge(nLeftX, VBAR_W/2);
		nLeftX -= (nLevelOffset + VBAR_W/2);

		if (nSide)
		{
			pDC->BitBlt(
				nLeftX, 0, 
				VBAR_W, VBAR_H,
				m_pMemDC, 
				0, 0,
				SRCCOPY);
		}
		else
		{
			pDC->BitBlt(
				nLeftX, 0, 
				VBAR_W, VBAR_H,
				m_pMemDC, 
				0, 0,
				SRCCOPY);
		}
		m_pMemDC->SelectObject(pOldBitmap);
	}
}

BOOL CVGridHandle::PointInHandle(CPoint& point, int& nKnob)
{
	// Build a rectangle, and check if the point
	// is in there
	CRect rctHandleBounds(m_nLevel - HANDLE_W, m_nEnd1, m_nLevel + HANDLE_W, m_nEnd2);
	CRect rctKnob1Bounds(m_nLevel - HANDLE_W, m_nEnd1 - VBAR_H, m_nLevel + HANDLE_W, m_nEnd1);
	CRect rctKnob2Bounds(m_nLevel - HANDLE_W, m_nEnd2, m_nLevel + HANDLE_W, m_nEnd2 + VBAR_H);

	// Check for main handle
	if (rctHandleBounds.PtInRect(point))
	{
		return (TRUE);
	}

#ifdef HANDLE_KNOBS
	// Check for knobs
	if (rctKnob1Bounds.PtInRect(point))
	{
		nKnob = 0;
		return (TRUE);
	}
	if (rctKnob2Bounds.PtInRect(point))
	{
		nKnob = 1;
		return (TRUE);
	}
#endif

	return (FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CDragDropState

CDragDropState::CDragDropState()
{
	m_fShowFeedback = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CBordview

IMPLEMENT_DYNCREATE(CBordview, CView)

UINT CBordview::m_cfDataExchangeName	= ::RegisterClipboardFormat("PMG 4.0 Art Browser");
UINT CBordview::m_cfDataExchangeID		= ::RegisterClipboardFormat("PMG 4.0 Border Editor");
CBordview::CBordview()
{
	// Create interface bitmaps and mem DCs
	m_pDisplayMemDC = new CDC;
	m_pStripMemDC = new CDC;
	m_pVBar = new CBitmap;
	m_pHBar = new CBitmap;

	// Create the handles
	m_apGridHandles[0] = new CHGridHandle(m_pDisplayMemDC, m_pHBar, 1);
	m_apGridHandles[1] = new CHGridHandle(m_pDisplayMemDC, m_pHBar, 2);
	m_apGridHandles[2] = new CVGridHandle(m_pDisplayMemDC, m_pVBar, 1);
	m_apGridHandles[3] = new CVGridHandle(m_pDisplayMemDC, m_pVBar, 2);

	// Clear selected handles
	m_nSelectedHGridHandle = -1;
	m_nSelectedVGridHandle = -1;

	// Clear movement
	m_LastPoint = CPoint(0, 0);
	m_nLastCursor = 0;
	m_bDraggingHandle = FALSE;
	m_bEndDraggingHandle = FALSE;
	m_bAllowHandleMove = TRUE;

	// the BOOLs
	m_bDrawGridBackground = TRUE;
	m_bDragSymmetry = TRUE;
	m_bSelectionsOnly = FALSE;
	m_bShowHandles = TRUE;
	m_bDrawMinimum = FALSE;
	m_bEmptyDraw = FALSE;

	// OLE
	m_pDropTarget = NULL;
}

CBordview::~CBordview()
{
	// Delete bitmaps and DCs
	delete m_pDisplayMemDC;
	delete m_pStripMemDC;
	delete m_pHBar;
	delete m_pVBar;

	// Delete the handles
	int i;
	for (i = 0; i < 4; i++)
	{
		delete m_apGridHandles[i];
	}

	// OLE
	delete m_pDropTarget;
}

void CBordview::CreateDropTarget()
{
   m_pDropTarget = new COleDropTarget();
   m_pDropTarget->Register(this);
}

BEGIN_MESSAGE_MAP(CBordview, CView)
	//{{AFX_MSG_MAP(CBordview)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_DRAG_SYMMETRY, OnDragSymmetry)
	ON_UPDATE_COMMAND_UI(ID_DRAG_SYMMETRY, OnUpdateDragSymmetry)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_SHOW_HANDLES, OnShowHandles)
	ON_UPDATE_COMMAND_UI(ID_SHOW_HANDLES, OnUpdateShowHandles)
	ON_WM_MOVE()
	ON_WM_KEYUP()
	ON_MESSAGE(WM_DOREALIZE, OnDoRealize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBordview drawing

void CBordview::OnDraw(CDC* pDC)
{
	CBorddoc* pDoc = (CBorddoc*)GetDocument();
	ASSERT_VALID(pDoc);
		
	// Draw the window
	if (!m_bEmptyDraw)
	{
		if (!m_bDrawMinimum)
		{
			DrawClient(pDC, pDoc);
		}
		else
		{
			DrawMinimum(pDC, pDoc);
		}
	}

	// Set up for real draw
	if (m_bEmptyDraw)
	{
		m_bEmptyDraw = FALSE;
		Invalidate(FALSE);
	}
}

void CBordview::DrawClient(CDC* pDC, CBorddoc* pDoc)
{
	int i;    

	// Background drawing
	if (!m_bDraggingHandle)
	{
		if (!m_bSelectionsOnly)
		{
			// Fill the background to grey
			FillBackground(pDC, pDoc);
			
			// Draw the grid
			CRect rctInvalid;
			GetUpdateRect(rctInvalid);
			if (!pDoc->DrawBEGrid(pDC, m_pDisplayMemDC, this, m_bDrawGridBackground))
			{
				// Interrupt ocurred, redraw
				InvalidateRect(rctInvalid, FALSE);
			}
		}
		
		// Draw selection boxes
		pDoc->DrawBESelections(pDC, !m_bSelectionsOnly);
		
	}

	// Draw the handles lines
	if (!m_bSelectionsOnly)
	{
		if (m_bShowHandles)
		{
			for (i = 0; i < 4; i++)
			{
				m_apGridHandles[i]->DrawHandleLine(pDC, m_bDraggingHandle);
			}
		}

#ifdef HANDLE_KNOBS
		// Create offscreen bitmaps for smooth animation
		CBitmap HBitmap, VBitmap;
		CBitmap* pOldMap = NULL;
		CRect rctFill;
		int nDestX, nDestY;
		int nLevelOffset;
		int nHHeight = m_rctMinRedraw.Height();
		int nVWidth = m_rctMinRedraw.Width();
		HBitmap.CreateCompatibleBitmap(pDC, HBAR_W, nHHeight);
		VBitmap.CreateCompatibleBitmap(pDC, nVWidth, VBAR_H);

		// Set the bitmap
		pOldMap = m_pStripMemDC->SelectObject(&HBitmap);
		if (pOldMap != NULL)
		{
			rctFill.SetRect(-1, -1, HBAR_W, nHHeight);

			// Do left & right
			for (i = 0, nDestX = m_rctMinRedraw.TopLeft().x - HBAR_W; i < 2; i++)
			{
				m_pStripMemDC->FillSolidRect(&rctFill, BACKGROUND_RGB);

				// Draw knobs on one side into the mem bitmap
				nLevelOffset = m_rctMinRedraw.TopLeft().y;
				m_apGridHandles[0]->DrawHandleKnobs(m_pStripMemDC, i, nLevelOffset);
				m_apGridHandles[1]->DrawHandleKnobs(m_pStripMemDC, i, nLevelOffset);

				// Blit knobs
				pDC->BitBlt(
					nDestX, nLevelOffset, 
					HBAR_W, nHHeight,
					m_pStripMemDC, 
					0, 0,
					SRCCOPY);
				nDestX = m_rctMinRedraw.BottomRight().x+1;
			}

			// Set the bitmap
			m_pStripMemDC->SelectObject(&VBitmap);
			rctFill.SetRect(-1, -1, nVWidth, VBAR_H);

			// Do top & bottom
			for (i = 0, nDestY = m_rctMinRedraw.TopLeft().y - VBAR_H; i < 2; i++)
			{
				m_pStripMemDC->FillSolidRect(&rctFill, BACKGROUND_RGB);

				// Draw knobs on one side into the mem bitmap
				nLevelOffset = m_rctMinRedraw.TopLeft().x;
				m_apGridHandles[2]->DrawHandleKnobs(m_pStripMemDC, i, nLevelOffset);
				m_apGridHandles[3]->DrawHandleKnobs(m_pStripMemDC, i, nLevelOffset);

				// Blit knobs
				pDC->BitBlt(
					nLevelOffset, nDestY, 
					nVWidth, VBAR_H,
					m_pStripMemDC, 
					0, 0,
					SRCCOPY);
				nDestY = m_rctMinRedraw.BottomRight().y+1;
			}	
			m_pStripMemDC->SelectObject(pOldMap);
		}
#endif
	}

	// Reset BOOLs
	m_bSelectionsOnly = FALSE;
	m_bDrawGridBackground = TRUE;
	if (m_bEndDraggingHandle)
	{
		// Unselect handles
		UnselectHandles();
		m_bEndDraggingHandle = FALSE;
	}
}

void CBordview::DrawMinimum(CDC* pDC, CBorddoc* pDoc)
{
	// Fill the background to grey
	FillBackground(pDC, pDoc);

	// Draw the "grid"
	pDoc->DrawMinBEGrid(pDC);
}

void CBordview::FillBackground(CDC* pDC, CBorddoc* pDoc)
{
	RECT rctClient;
	GetClientRect(&rctClient);

	// Get outer grid coordinates
	LONG aXValues[4], aYValues[4];
	pDoc->GetBEGrid(aXValues, aYValues);

	// Need 4 rectangles
	CRect rctFill;

	rctFill.SetRect(rctClient.left, rctClient.top, rctClient.right, aYValues[0]);
	pDC->FillSolidRect(&rctFill, BACKGROUND_RGB);
	rctFill.SetRect(aXValues[3], aYValues[0], rctClient.right, aYValues[3]);
	pDC->FillSolidRect(&rctFill, BACKGROUND_RGB);
	rctFill.SetRect(rctClient.left, aYValues[3], rctClient.right, rctClient.bottom);
	pDC->FillSolidRect(&rctFill, BACKGROUND_RGB);
	rctFill.SetRect(rctClient.left, aYValues[0], aXValues[0], aYValues[3]);
	pDC->FillSolidRect(&rctFill, BACKGROUND_RGB);
}

/////////////////////////////////////////////////////////////////////////////
// CBordview diagnostics

#ifdef _DEBUG
void CBordview::AssertValid() const
{
	CView::AssertValid();
}

void CBordview::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBordview operations
void CBordview::ResizeDoc()
{
	CBorddoc* pDoc = (CBorddoc*)GetDocument();
	ASSERT_VALID(pDoc);

	RECT rctClient;
	GetClientRect(&rctClient);
	pDoc->Resize(rctClient, CSize(BORDER_W, BORDER_H));
}

DROPEFFECT CBordview::ComputeDropEffect(COleDataObject* pDataObject, DWORD dwKeyState)
{
	DROPEFFECT de = DROPEFFECT_NONE;

	if (IsDataAvailable(pDataObject))
	{
		de = DROPEFFECT_MOVE;
	}

	return (de);
}

BOOL CBordview::IsDataAvailable(COleDataObject* pDataObject)
{
	return (
		pDataObject->IsDataAvailable(m_cfDataExchangeName) ||
		pDataObject->IsDataAvailable(m_cfDataExchangeID));
}

BOOL CBordview::UpdateDragFeedback(DROPEFFECT de, CPoint* point)
{
	CBorddoc* pDoc = (CBorddoc*)GetDocument();
	ASSERT_VALID(pDoc);
	BOOL r = FALSE;

	// Highlight/un-highlight selections
	r = pDoc->SelectBEDragRegion(point);
	if (point == NULL)
	{
		m_bSelectionsOnly = FALSE;
	}

	return (r);
}

/////////////////////////////////////////////////////////////////////////////
// CBordview message handlers
int CBordview::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Set up paste timer
	m_bEnablePaste = FALSE;

	// Set up OLE
	CreateDropTarget();
	
	return 0;
}

void CBordview::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// Create mem DC for bitmaps, and load bitmaps
	if (m_pDisplayMemDC->GetSafeHdc() == NULL) {
		CClientDC dc(this);
		OnPrepareDC(&dc);

		// Load the arrow bitmaps
		m_pVBar->LoadBitmap(IDB_VERTICAL);
		m_pHBar->LoadBitmap(IDB_HORIZONTAL);

		// Create the DCs
		m_pDisplayMemDC->CreateCompatibleDC(&dc);
		m_pStripMemDC->CreateCompatibleDC(m_pDisplayMemDC);
	}

	// Retrieve the current cursor clip
	::GetClipCursor(&m_rctOldCursorClip);

}

void CBordview::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
#if 0
	CBorddoc* pDoc = (CBorddoc*)GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->ClearBEDrawing();	// trigger BG clear
#endif
	
	m_bEmptyDraw = TRUE;
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

BOOL CBordview::DestroyWindow() 
{	
	return CView::DestroyWindow();
}

void CBordview::OnMove(int x, int y) 
{
	CView::OnMove(x, y);
	Invalidate(FALSE);
}

void CBordview::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// Resize the document
	ResizeDoc();
}

BOOL CBordview::OnEraseBkgnd(CDC* pDC) 
{	
	// Don't call the base class to avoid that annoying flicker
	//return CView::OnEraseBkgnd(pDC);
	return TRUE;
}

void CBordview::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{

	// Create the min redraw rectangle
	if (lHint & ComputeMinRect)
	{
		CBorddoc* pDoc = (CBorddoc*)pHint;
		LONG aXValues[4], aYValues[4];

		// Retrieve the grid arrays
		pDoc->GetBEGrid(aXValues, aYValues);

		// Set the rectangle
		m_rctMinRedraw.SetRect(
			aXValues[0],
			aYValues[0],
			aXValues[3],
			aYValues[3]);

	}

	// See if we need to update the handles
	if (lHint & AdjustGridHandles)
	{
		CBorddoc* pDoc = (CBorddoc*)pHint;
		LONG aXValues[4], aYValues[4];

		// Retrieve the grid arrays
		pDoc->GetBEGrid(aXValues, aYValues);

		// Set the handles appropriately
		int i;
		
		// Horizontal
		for (i = 0; i < 2; i++)
		{
			m_apGridHandles[i]->SetPoints(aXValues[0], aXValues[3]);
			m_apGridHandles[i]->SetLevel(aYValues[i+1]);
		}
		// Vertical
		for (i = 2; i < 4; i++)
		{
			m_apGridHandles[i]->SetPoints(aYValues[0], aYValues[3]);
			m_apGridHandles[i]->SetLevel(aXValues[i-1]);
		}

		// Redraw
		Invalidate(TRUE);
	}

	// May need to set grid handle limit pointers
	if (lHint & SetHandleLimits)
	{
		CBorddoc* pDoc = (CBorddoc*)pHint;
		RECTAliasT rctLimits;

		// Retrieve the grid arrays
		pDoc->GetBELimits(rctLimits);

		// Set Limits
		m_apGridHandles[0]->SetLimitsObj(rctLimits.top, m_apGridHandles[1], FALSE);
		m_apGridHandles[1]->SetLimitsObj(rctLimits.bottom, m_apGridHandles[0], TRUE);
		m_apGridHandles[2]->SetLimitsObj(rctLimits.left, m_apGridHandles[3], FALSE);
		m_apGridHandles[3]->SetLimitsObj(rctLimits.right, m_apGridHandles[2], TRUE);

		// Set symmetry
		m_apGridHandles[0]->SetSymmetry(m_apGridHandles[1]);
		m_apGridHandles[1]->SetSymmetry(m_apGridHandles[0]);
		m_apGridHandles[2]->SetSymmetry(m_apGridHandles[3]);
		m_apGridHandles[3]->SetSymmetry(m_apGridHandles[2]);
	}

	// Invalidate the modified rectangles
	if (lHint & DrawChangedRects)
	{
		CCntArray<CRect, CRect&>* pRectArray = (CCntArray<CRect, CRect&>*)pHint;
		int nSize = pRectArray->GetNValidItems();
		int i;

		//m_bDrawGridBackground = GetUpdateRect(NULL);
		for (i = 0; i < nSize; i++)
		{
			InvalidateRect(pRectArray->ElementAt(i), FALSE);
		}
	}

	// Set to only draw the selection boxes
	if (lHint & SelectionsOnly)
	{
		m_bSelectionsOnly = !(GetUpdateRect(NULL) || m_bDrawMinimum);
	}

	// Set to not only draw the selection boxes
	if (lHint & NotSelectionsOnly)
	{
		m_bSelectionsOnly = FALSE;
	}

	// Basic redraw
	if (lHint & Revert)
	{
		ResizeDoc();
	}
	
	// Basic redraw
	if (lHint & Redraw)
	{
		Invalidate(FALSE);
	}
}


void CBordview::OnMouseMove(UINT nFlags, CPoint point) 
{
	// If a handle is selected, move it.
	// Otherwise, check if cursor is over a handle.
	if (m_nSelectedHGridHandle != -1 || m_nSelectedVGridHandle != -1)
	{
		// Determine offset
		CPoint moved = point - m_LastPoint;

		// Move selected handles
		if (m_bAllowHandleMove)
		{
			if (m_nSelectedHGridHandle != -1)
			{
				m_apGridHandles[m_nSelectedHGridHandle]->MoveHandle(moved.x, moved.y, m_bDragSymmetry);
			}
			if (m_nSelectedVGridHandle != -1)
			{
				m_apGridHandles[m_nSelectedVGridHandle]->MoveHandle(moved.x, moved.y, m_bDragSymmetry);
			}
		}

		// Update cursor clip
		SetCursorClip();

		// Invalidate
		Invalidate(FALSE);

	}
	else
	{
		// Check for point in handles
		// If point is over both handles of a type,
		// last one is taken
		int i;
		int nHPick = -1, nVPick = -1;
		m_nHKnob = -1;
		for (i = 0; i < 2; i++)
		{
			if (m_apGridHandles[i]->PointInHandle(point, m_nHKnob))
			{
				nHPick = i;	// found horizontal
			}
		}
		m_nVKnob = -1;
		for (i = 2; i < 4; i++)
		{
			if (m_apGridHandles[i]->PointInHandle(point, m_nVKnob))
			{
				nVPick = i;	// found vertical
			}
		}

		// Change cursor
		static UINT nCursorIDs[] = {
			IDC_NWSE, IDC_WE, IDC_NS
		};

		int nNewCursor = 0;
		if (nHPick != -1 && nVPick != -1)
		{
			nNewCursor = 1;
		}
		else if (nHPick != -1 || nVPick != -1)
		{
			nNewCursor = (nHPick == -1) ? 2 : 3;
		}

		// Only change cursor if we need to
		if (nNewCursor != m_nLastCursor)
		{
			// Keep input captured on sensitive zones
			if (nNewCursor != 0) SetCapture();
			else ReleaseCapture();

			if (nNewCursor)
			{
				::SetCursor(theBorderApp.LoadCursor(nCursorIDs[nNewCursor-1]));
			}
			else
			{
				::SetCursor(theBorderApp.LoadStandardCursor(IDC_ARROW));
			}
			m_nLastCursor = nNewCursor;
		}

		// Record possible selections
		m_nPreSelectedHGridHandle = nHPick;
		m_nPreSelectedVGridHandle = nVPick;
	}

	// Keep track
	m_LastPoint = point;
	m_bAllowHandleMove = TRUE;

	CView::OnMouseMove(nFlags, point);
}

void CBordview::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Select handles
	if (nFlags & MK_LBUTTON)
	{
		m_nSelectedHGridHandle = m_nPreSelectedHGridHandle;
		m_nSelectedVGridHandle = m_nPreSelectedVGridHandle;
		m_bDraggingHandle = (
			m_nSelectedHGridHandle != -1 ||
			m_nSelectedVGridHandle != -1);
		SetCursorClip();
		m_nPreSelectedHGridHandle = -1;
		m_nPreSelectedVGridHandle = -1;

		// Save current handle positions
		int i;
		for (i = 0; i < 4; i++)
		{
			m_apGridHandles[i]->StoreLevel();
		}

		// If handles have been selected, readjust symmetry
		if (m_bDraggingHandle && m_bDragSymmetry)
		{
			CBorddoc* pDoc = (CBorddoc*)GetDocument();
			ASSERT_VALID(pDoc);			
			RECTAliasT rctLimits;
			pDoc->GetBELimits(rctLimits);
			
			// Adjust selected handles
			int m_nLevelOffset;
			POINT point;
			::GetCursorPos(&point);
			if (m_nSelectedHGridHandle != -1)
			{
				if (!m_apGridHandles[m_nSelectedHGridHandle]->AdjustSymmetry(rctLimits, m_nLevelOffset))
				{
					m_bAllowHandleMove = FALSE;
					::SetCursorPos(point.x, point.y + m_nLevelOffset);
				}
			}
			if (m_nSelectedVGridHandle != -1)
			{
				if (!m_apGridHandles[m_nSelectedVGridHandle]->AdjustSymmetry(rctLimits, m_nLevelOffset))
				{
					m_bAllowHandleMove = FALSE;
					::SetCursorPos(point.x + m_nLevelOffset, point.y);
				}
			}

			// Invalidate
			Invalidate(FALSE);
		}
	}

	// If no handles selected, select an editing region
	if (m_nSelectedHGridHandle == -1 && m_nSelectedVGridHandle == -1)
	{
		SelectEditRegions(nFlags, point);
	}

	CView::OnLButtonDown(nFlags, point);
}

void CBordview::SetCursorClip()
{
	CBorddoc* pDoc = (CBorddoc*)GetDocument();
	ASSERT_VALID(pDoc);
	
	CRect rctCursorClip;
	CRect rctHClip, rctVClip;
	RECTAliasT rctLimits;
	BOOL bBuildCursorClip = FALSE, bSetCursorClip = FALSE;
	LONG* pUpperLimit;
	LONG* pLowerLimit;
	pDoc->GetBELimits(rctLimits);
	
	// Set initial cursor clip
	rctLimits.GetRECTAlias(rctHClip);
	rctLimits.GetRECTAlias(rctVClip);

	if (m_nSelectedHGridHandle != -1)
	{
		bBuildCursorClip = bSetCursorClip = TRUE;

		// See if clip rect is bounded by limits
		m_apGridHandles[m_nSelectedHGridHandle]->GetLimits(pUpperLimit, pLowerLimit);
		rctHClip.SetRect(
			*rctLimits.left,
			*pLowerLimit + 1,
			*rctLimits.right,
			*pUpperLimit - 1);

		// Build cursor clip rect for horizontal knob
		if (m_nHKnob != -1)
		{
			bBuildCursorClip = FALSE;
			if (m_nHKnob == 0)
			{
				rctCursorClip.SetRect(
					*rctLimits.left - HBAR_W,
					*pLowerLimit + 1,
					*rctLimits.left,
					*pUpperLimit - 1);
			}
			else
			{
				rctCursorClip.SetRect(
					*rctLimits.right,
					*pLowerLimit + 1,
					*rctLimits.right + HBAR_W,
					*pUpperLimit - 1);
			}
		}
	}
	if (m_nSelectedVGridHandle != -1)
	{
		bBuildCursorClip = bSetCursorClip = TRUE;

		// See if clip rect is bounded by limits
		m_apGridHandles[m_nSelectedVGridHandle]->GetLimits(pUpperLimit, pLowerLimit);
		rctVClip.SetRect(
			*pLowerLimit + 1,
			*rctLimits.top,
			*pUpperLimit - 1,
			*rctLimits.bottom);

		// Build cursor clip rect for horizontal knob
		if (m_nVKnob != -1)
		{
			bBuildCursorClip = FALSE;
			if (m_nVKnob == 0)
			{
				rctCursorClip.SetRect(
					*pLowerLimit + 1,
					*rctLimits.top - VBAR_H,
					*pUpperLimit - 1,
					*rctLimits.top);
			}
			else
			{
				rctCursorClip.SetRect(
					*pLowerLimit + 1,
					*rctLimits.bottom,
					*pUpperLimit - 1,
					*rctLimits.bottom + VBAR_H);
			}
		}
	}

	// See if we need to set the cursor clip rect
	if (bSetCursorClip)
	{
		if (bBuildCursorClip)
		{
			rctCursorClip.IntersectRect(rctHClip, rctVClip);
		}

		// Set clipping
		ClientToScreen(rctCursorClip);
		::ClipCursor(&rctCursorClip);
	}
}

void CBordview::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (SelectEditRegions(nFlags, point))
	{
		// Bring up graphic selection
		m_bSelectionsOnly = FALSE;
		Invalidate(FALSE);
		PostMessage(WM_COMMAND, ID_CHOOSE_GRAPHIC);
	}
			
}

BOOL CBordview::SelectEditRegions(UINT nFlags, CPoint point) 
{
	CBorddoc* pDoc = (CBorddoc*)GetDocument();
	ASSERT_VALID(pDoc);
	BOOL r;

	// Have the document search for edit regions
	BOOL bControl = (nFlags & MK_CONTROL) != 0;
	BOOL bShift = (nFlags & MK_SHIFT) != 0;
	r = pDoc->SelectBEEditRegion(bShift || bControl, bControl, point);

	// If right button is down, bring up a pop-up menu
	if (nFlags & MK_RBUTTON)
	{

	}

	return (r);
}

void CBordview::OnLButtonUp(UINT nFlags, CPoint point) 
{	
	// Transfer handle movement to border object	
	if (m_bDraggingHandle)
	{
		CBorddoc* pDoc = (CBorddoc*)GetDocument();
		ASSERT_VALID(pDoc);

		int nXIndex = -1, nYIndex = -1;
		LONG nXLevel, nYLevel;

		// symmetry values
		int nSymXIndex = -1, nSymYIndex = -1;
		LONG nSymXLevel, nSymYLevel;
		CGridHandle* pSymmetryHandle;

		if (m_nSelectedHGridHandle != -1)
		{
			m_apGridHandles[m_nSelectedHGridHandle]->GetLevelInfo(nYIndex, nYLevel);
			if (m_bDragSymmetry &&
				(pSymmetryHandle = m_apGridHandles[m_nSelectedHGridHandle]->GetSymmetry()) != NULL)
			{
				pSymmetryHandle->GetLevelInfo(nSymYIndex, nSymYLevel);
			}
		}
		if (m_nSelectedVGridHandle != -1)
		{
			m_apGridHandles[m_nSelectedVGridHandle]->GetLevelInfo(nXIndex, nXLevel);
			if (m_bDragSymmetry &&
				(pSymmetryHandle = m_apGridHandles[m_nSelectedVGridHandle]->GetSymmetry()) != NULL)
			{
				pSymmetryHandle->GetLevelInfo(nSymXIndex, nSymXLevel);
			}
		}

		// Change the coordinates
		pDoc->SetGridCoords(nXIndex, nYIndex, nXLevel, nYLevel, nSymXIndex, nSymYIndex, nSymXLevel, nSymYLevel);

		// Give back cursor clip
		::ClipCursor(&m_rctOldCursorClip);

		// Redraw
		Invalidate(FALSE);
	}

	// Unselect handles
	UnselectHandles();

	CView::OnLButtonUp(nFlags, point);
}

void CBordview::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// ESC cancels function
	if (nChar == 27)
	{
		// If dragging, go back
		if (m_bDraggingHandle)
		{			
			// Give back cursor clip
			::ClipCursor(&m_rctOldCursorClip);

			// Restore current handle positions
			int i;
			for (i = 0; i < 4; i++)
			{
				m_apGridHandles[i]->RestoreLevel();
			}

			// Redraw
			Invalidate(FALSE);

			// Store a pending unselect handles
			m_bEndDraggingHandle = TRUE;
		}
	}

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CBordview::UnselectHandles()
{
	m_bDraggingHandle = FALSE;
	m_nPreSelectedHGridHandle = m_nSelectedHGridHandle;
	m_nPreSelectedVGridHandle = m_nSelectedVGridHandle;
	m_nSelectedHGridHandle = -1;
	m_nSelectedVGridHandle = -1;
}

void CBordview::OnDragSymmetry() 
{
	m_bDragSymmetry = !m_bDragSymmetry;	
}

void CBordview::OnUpdateDragSymmetry(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bDragSymmetry);	
}

void CBordview::OnShowHandles() 
{
	m_bShowHandles = !m_bShowHandles;
	
	// Redraw
	Invalidate(FALSE);
	
}

void CBordview::OnUpdateShowHandles(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowHandles);	
	
}

// OLE
DROPEFFECT CBordview::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	pDataObject->BeginEnumFormats();
	FORMATETC fmt;
	while (pDataObject->GetNextFormat(&fmt))
	{
		TRACE1("format: %d\n", fmt.cfFormat);
	}
	TRACE("OnDragEnter...\n");

	// Init OLE
	m_DragDropState.m_fShowFeedback = IsDataAvailable(pDataObject);
	CBorddoc* pDoc = (CBorddoc*)GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->InitBEDragDrop();

	return OnDragOver(pDataObject, dwKeyState, point);
}

DROPEFFECT CBordview::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	DROPEFFECT de = ComputeDropEffect(pDataObject, dwKeyState);

	if (m_DragDropState.m_fShowFeedback)
	{
		CPoint pt(point);
		if (!UpdateDragFeedback(de, &pt))
		{
			de = DROPEFFECT_NONE;
		}
	}

	return de;
}

void CBordview::OnDragLeave() 
{
	TRACE("OnDragLeave...\n");
	if (m_DragDropState.m_fShowFeedback)
	{
		UpdateDragFeedback();
	}

	CView::OnDragLeave();
}


BOOL CBordview::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	CBorddoc* pDoc = (CBorddoc*)GetDocument();
	ASSERT_VALID(pDoc);

	TRACE("OnDrop\n");
	if (dropEffect != DROPEFFECT_NONE)
	{
		pDoc->PasteGraphic(pDataObject);
	}

	if (m_DragDropState.m_fShowFeedback)
	{
		CPoint pt(point);
		pDoc->PermSelectBEDragRegion();
	}

	return CView::OnDrop(pDataObject, dropEffect, point);
}

void CBordview::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	COleDataObject Data;
	Data.AttachClipboard();
	m_bEnablePaste = IsDataAvailable(&Data);
	Data.Detach();

	pCmdUI->Enable(m_bEnablePaste);
}

LRESULT CBordview::OnDoRealize(WPARAM wParam, LPARAM)
{
	ASSERT(wParam != NULL);
	CBorddoc* pDoc = (CBorddoc*)GetDocument();
	ASSERT_VALID(pDoc);

	CMainFrame* pFrame = (CMainFrame*)theBorderApp.m_pMainWnd;
	ASSERT(pFrame->IsKindOf(RUNTIME_CLASS( CMainFrame )));
	CClientDC frameDC(pFrame);
	
	// Update the palette
	HPALETTE hPalette = (pDoc->GetCallbackInterface())->GetPalette();
	
	// All views but one should be a background palette.
	// wParam contains a handle to the active view, so the SelectPalette
	// bForceBackground flag is FALSE only if wParam == m_hWnd (this view)
	CPalette* pOldPalette = frameDC.SelectPalette(CPalette::FromHandle(hPalette), ((HWND)wParam) != m_hWnd);

	if (pOldPalette != NULL)
	{
		UINT nColorsChanged = frameDC.RealizePalette();
		if (nColorsChanged > 0)
			pDoc->UpdateAllViews(NULL, Redraw);
		frameDC.SelectPalette(pOldPalette, TRUE);
	}
	else
	{
		TRACE0("\tSelectPalette failed in CModelView::OnPaletteChanged\n");
	}

	return 0L;
}
