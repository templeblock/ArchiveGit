#if !defined(AFX_BORDVIEW_H__E12ED162_85E2_11D1_8680_006008661BA9__INCLUDED_)
#define AFX_BORDVIEW_H__E12ED162_85E2_11D1_8680_006008661BA9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// Bordview.h : header file
//

const COLORREF BACKGROUND_RGB = RGB(128, 128, 128);

/////////////////////////////////////////////////////////////////////////////
// CGridHandle

class CGridHandle
{
protected:
	// Constants
	const int HANDLE_W;

	// Line endpoitns
	LONG m_nEnd1, m_nEnd2;
	LONG m_nLevel;		// x or y line handle falls on

	// Limits (upper and lower refer to numerical values of
	// m_nLevel, where positive x is right, positive y is
	// down).
	enum {
		LOWER_WALL,
		UPPER_WALL
	} m_WallLimit;
	LONG* m_pUpperLimit;
	LONG* m_pLowerLimit;

	// Symmetry
	CGridHandle* m_pSymmetryHandle;

	// Grid index handle is tied to
	int m_nGridIndex;

	// Drawing
	CDC* m_pMemDC;
	CBitmap *m_pBitmap;
	CPen* m_pPen;

	// Movement
	LONG m_nOldLevel;
	LONG m_nLastLevel;

// Construction/destruction
public:
	CGridHandle(CDC* pMemDC, CBitmap* pBitmap, int nGridIndex);
	~CGridHandle();
	
// Attributes
public:
	void SetPoints(LONG nEnd1, LONG nEnd2)
	{
		m_nEnd1 = nEnd1;
		m_nEnd2 = nEnd2;
	}
	void SetLevel(LONG nLevel, BOOL bSetLast = TRUE)
	{
		m_nLevel = nLevel;
		if (bSetLast)
		{
			m_nLastLevel = nLevel;
		}
	}
	void StoreLevel()
	{
		m_nOldLevel = m_nLevel;
	}
	void RestoreLevel()
	{
		m_nLevel = m_nOldLevel;
	}
	void SetLimitsObj(LONG* pLimit, CGridHandle* pAdjHandle, BOOL bUpperFirst)
	{
		if (bUpperFirst)
			SetLimits(pLimit, &pAdjHandle->m_nLevel);
		else
			SetLimits(&pAdjHandle->m_nLevel, pLimit);
	}
	void SetLimits(LONG* pUpper, LONG* pLower)
	{
		m_pUpperLimit = pUpper;
		m_pLowerLimit = pLower;
	}
	void GetLimits(LONG*& pUpper, LONG*& pLower)
	{
		pUpper = m_pUpperLimit;
		pLower = m_pLowerLimit;
	}
	void SetSymmetry(CGridHandle* pSymmetryHandle)
	{
		m_pSymmetryHandle = pSymmetryHandle;
	}
	CGridHandle* GetSymmetry()
	{
		return (m_pSymmetryHandle);
	}
	void GetLevelInfo(int& nGridIndex, LONG& nLevel)
	{
		nGridIndex = m_nGridIndex;
		nLevel = m_nLevel;
	}

// Implememtation
public:
	virtual void MoveHandle(int nHMove, int nVMove, BOOL bUseSym) = 0;
	virtual BOOL AdjustSymmetry(RECTAliasT& rctLimits, int& nLevelOffset) = 0;
	virtual void DrawHandleLine(CDC* pDC, BOOL bDragging) = 0;
	virtual void DrawHandleKnobs(CDC* pDC, int nSide, int nLevelOffset) = 0;
	virtual BOOL PointInHandle(CPoint& point, int& nKnob) = 0;
	void VerifyHandleLimits();
	void ComputeHandleEdge(int& nValue, int nThresh);
};

/////////////////////////////////////////////////////////////////////////////
// CHGridHandle
// Handles horizontal handles

class CHGridHandle : public CGridHandle
{
// Construction/destruction
public:
	CHGridHandle(CDC* pMemDC, CBitmap* pBitmap, int nGridIndex) :
	  CGridHandle(pMemDC, pBitmap, nGridIndex)
	{
	}

// Implememtation
public:
	virtual void MoveHandle(int nHMove, int nVMove, BOOL bUseSym);
	virtual BOOL AdjustSymmetry(RECTAliasT& rctLimits, int& nLevelOffset);
	virtual void DrawHandleLine(CDC* pDC, BOOL bDragging);
	virtual void DrawHandleKnobs(CDC* pDC, int nSide, int nLevelOffset);
	virtual BOOL PointInHandle(CPoint& point, int& nKnob);
	
};

/////////////////////////////////////////////////////////////////////////////
// CVGridHandle
// Handles horizontal handles

class CVGridHandle : public CGridHandle
{
// Construction/destruction
public:
	CVGridHandle(CDC* pMemDC, CBitmap* pBitmap, int nGridIndex) :
	  CGridHandle(pMemDC, pBitmap, nGridIndex)
	{
	}

// Implememtation
public:
	virtual void MoveHandle(int nHMove, int nVMove, BOOL bUseSym);
	virtual BOOL AdjustSymmetry(RECTAliasT& rctLimits, int& nLevelOffset);
	virtual void DrawHandleLine(CDC* pDC, BOOL bDragging);
	virtual void DrawHandleKnobs(CDC* pDC, int nSide, int nLevelOffset);
	virtual BOOL PointInHandle(CPoint& point, int& nKnob);
	
};

/////////////////////////////////////////////////////////////////////////////
// CDragDropState
// Handles drag drop state info

class CDragDropState
{
public:
	CDragDropState();

	BOOL m_fShowFeedback;
/*
	PPNT m_DragSize;
	PPNT m_DragOffset;
	PPNT m_DragPoint;

	BOOL m_fShowFeedback;
	DROPEFFECT m_prevDropEffect;
	CRect m_crDrawnRect;
*/
};

/////////////////////////////////////////////////////////////////////////////
// CBordview view

class CBordview : public CView
{
	// offscreen DCs & bitmaps
	CDC* m_pDisplayMemDC;	
	CDC* m_pStripMemDC;	
	CBitmap* m_pVBar;
	CBitmap* m_pHBar;
	
	// Miscellaneous rectangles
	CRect m_rctMinRedraw;
	CRect m_rctOldCursorClip;

	// Display
	BOOL m_bDrawGridBackground;
	BOOL m_bSelectionsOnly;
	BOOL m_bShowHandles;
	BOOL m_bDrawMinimum;
	BOOL m_bEmptyDraw;

	// Array of Grid Handles
	// 0,1 are horizontal, 2,3 are vertical
	CGridHandle* m_apGridHandles[4];
	int m_nSelectedHGridHandle;		// indeces of selected grid handles (max 1 of each H and V)
	int m_nSelectedVGridHandle;		// -1 for none
	int m_nPreSelectedHGridHandle;	// indeces of grid handles cursor is over
	int m_nPreSelectedVGridHandle;	// -1 for none

	// Movement
	int m_nLastCursor;				// index of last cursor	
	CPoint m_LastPoint;				// for mouse movement
	BOOL m_bDraggingHandle;			// TRUE if 1 or more grid handles are being dragged
	BOOL m_bDraggingKnob;			// TRUE if the "knob" portion of the handle is moving
	BOOL m_bDragSymmetry;			// TRUE if symmetric handles are moved together
	BOOL m_bEndDraggingHandle;		// TRUE to turn off dragging handle after draw
	BOOL m_bAllowHandleMove;		// TRUE if cursor pulling is allowed
	int m_nHKnob, m_nVKnob;			// Indicates which knob side is being dragged
	
	// Paste timer
	BOOL m_bEnablePaste;

	// OLE
	COleDropTarget*	m_pDropTarget;
	CDragDropState m_DragDropState;
public:
	static UINT m_cfDataExchangeName;
	static UINT m_cfDataExchangeID;

protected:
	CBordview();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBordview)

// Attributes
public:
	enum lHints {	// hints used by OnUpdate
		AdjustGridHandles = 1,
		SetHandleLimits = 2,
		ComputeMinRect = 4,
		DrawChangedRects = 8,
		SelectionsOnly = 16,
		NotSelectionsOnly = 32,
		Redraw = 64,
		Revert = 128
	};

// Operations
public:
	void CreateDropTarget();
	void ResizeDoc();
	void DrawClient(CDC* pDC, CBorddoc* pDoc);
	void DrawMinimum(CDC* pDC, CBorddoc* pDoc);
	void FillBackground(CDC* pDC, CBorddoc* pDoc);
	void UnselectHandles();
	DROPEFFECT ComputeDropEffect(COleDataObject* pDataObject, DWORD dwKeyState);
	BOOL UpdateDragFeedback(DROPEFFECT de = DROPEFFECT_NONE, CPoint* point = NULL);
	BOOL IsDataAvailable(COleDataObject* pDataObject);
	BOOL SelectEditRegions(UINT nFlags, CPoint point);
	void SetCursorClip();
	void DrawMinimum(BOOL bState)
	{
		m_bDrawMinimum = bState;
		m_bSelectionsOnly = FALSE;	// regardless
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBordview)
	public:
	virtual void OnInitialUpdate();
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual BOOL DestroyWindow();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CBordview();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CBordview)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDragSymmetry();
	afx_msg void OnUpdateDragSymmetry(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnShowHandles();
	afx_msg void OnUpdateShowHandles(CCmdUI* pCmdUI);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnDoRealize(WPARAM wParam, LPARAM lParam);  // user message
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BORDVIEW_H__E12ED162_85E2_11D1_8680_006008661BA9__INCLUDED_)
