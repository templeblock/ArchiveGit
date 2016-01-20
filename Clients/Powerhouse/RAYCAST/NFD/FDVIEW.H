// fdview.h : interface of the CFdView class
//
/////////////////////////////////////////////////////////////////////////////

#define MIN_HEART_RATE	9
#define MAX_HEART_RATE	4

typedef struct DibHeader
{
	BITMAPINFOHEADER	Header;
	RGBQUAD				aColors[256];
} DibHeader;

class CFdView : public CView
{
protected: // create from serialization only
	CFdView();
	DECLARE_DYNCREATE(CFdView)

// Attributes
public:
	CFdDoc* GetDocument();

// Operations
public:

// Implementation
public:
	virtual ~CFdView();
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated routines added to class
	int	ResizeWindow(int divisor);
	void ClearDesktop(void);
	int AppAckInitialize(void);
	void AppAckWrapUp(void);
	void PaintScreen(int Left, int Top, int Width, int Height, int SrcLeft, int SrcTop, int SrcWidth, int SrcHeight);
	HBITMAP BitmapCreate(DibHeader *DibBufferInfo, HDC *hdcBuffer, void far **pBuffer);
	void DibBufferInfoCreate(DibHeader far *DibBufferInfo, int dx, int dy);

// Generated message map functions
protected:
	//{{AFX_MSG(CFdView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg long OnUserIdle(WPARAM wParam, LPARAM lParam);
	afx_msg long OnUserAction(WPARAM wParam, LPARAM lParam);
	afx_msg long OnUserSound(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnUpdateViewForward(CCmdUI* pCmdUI);
	afx_msg void OnViewForward();
	afx_msg void OnUpdateOptionsMusic(CCmdUI* pCmdUI);
	afx_msg void OnOptionsMusic();
	afx_msg void OnUpdateOptionsSound(CCmdUI* pCmdUI);
	afx_msg void OnOptionsSound();
	afx_msg void OnUpdateOptionsAutomatic(CCmdUI* pCmdUI);
	afx_msg void OnOptionsAutomatic();
	afx_msg void OnUpdateOptionsStatus(CCmdUI* pCmdUI);
	afx_msg void OnOptionsStatus();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnUpdateOptionsCrosshairs(CCmdUI* pCmdUI);
	afx_msg void OnOptionsCrosshairs();
	afx_msg void OnFileQuit();
	afx_msg long OnMmMomOpen(WPARAM hmOut, LPARAM lParam);
	afx_msg long OnMmMomDone(WPARAM hmOut, LPARAM mhOut);
	afx_msg long OnMmMomClose(WPARAM hmOut, LPARAM lParam);
	afx_msg void OnOptionsVolume();
	afx_msg void OnOptionsMap();
	afx_msg void OnUpdateOptionsMap(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewLowRes(CCmdUI* pCmdUI);
	afx_msg void OnViewLowRes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
static UINT near WM_USER_IDLE = ::RegisterWindowMessage("WM_USER_IDLE");
static UINT near WM_USER_ACTION = ::RegisterWindowMessage("WM_USER_ACTION");
static UINT near WM_USER_SOUND = ::RegisterWindowMessage("WM_USER_SOUND");

#ifndef _DEBUG	// debug version in fdview.cpp
inline CFdDoc* CFdView::GetDocument()
   { return (CFdDoc*) m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

