// Jim - A flicker-free resizing dialog class
// Based on http://www.codeproject.com/dialog/RPResizeDlg.asp#xx933860xx
 
#pragma once
#include <vector>

// How windows size ...
#define WST_NONE		0x00	// No size changed
#define WST_LEFT		0x01	// size to left
#define WST_TOP			0x02	// size to top
#define WST_RIGHT		0x04	// size to right
#define WST_BOTTOM		0x08	// size to bottom

typedef
enum {				// possible Control reSize Type
	CST_NONE = 0,
	CST_RESIZE,		// NOMOVE + SIZE, add all delta-size of dlg to control
	CST_REPOS,		// MOVE(absolutely) + NOSIZE, move control's pos by delta-size
	CST_RELATIVE,	// MOVE(proportional)  + NOSIZE, keep control always at a relative pos
	CST_ZOOM,		// MOVE + SIZE (both are automatically proportional)
	CST_DELTA_ZOOM	// MOVE(proportional, set manually) + SIZE(proportional, set manuall)
} CST;

// Contained class to hold item state
class CItemCtrl
{
public:
	CItemCtrl();
	CItemCtrl(const CItemCtrl& src);

	bool xUpdate(int st, const CRect& curRect, CRect& ctrlRect, CRect* pnRect, CRect* poRect, CRect* pR0);
	bool yUpdate(int st, const CRect& curRect, CRect& ctrlRect, CRect* pnRect, CRect* poRect, CRect* pR0);
	HDWP OnSize(HDWP hdwp, int sizeType, CRect* pnCltRect, CRect* poCltRect, CRect* pR0, CWnd* pDlg);

	CItemCtrl& operator=(const CItemCtrl& src);

protected:
	void Assign(const CItemCtrl& src);

public:
	UINT m_nID;
	CRect m_wRect;
	CST m_stxLeft;		// when left resizing ...
	CST m_stxRight;		// when right resizing ...
	CST m_styTop;		// when top resizing ...
	CST m_styBottom;	// when bottom resizing ...
	bool m_bFlickerFree;
	bool m_bInvalidate;	// Invalidate ctrl's rect(eg. no-automatic update for static when resize+move)
	double m_xRatio;
	double m_cxRatio;
	double m_yRatio;
	double m_cyRatio;
};

class CResizeDlg : public CDialog
{
public:
	CResizeDlg(UINT nID,CWnd* pParentWnd = NULL);
	virtual ~CResizeDlg();

protected:
	DECLARE_DYNAMIC(CResizeDlg)
	DECLARE_MESSAGE_MAP()

public:
	void AddControl(UINT nID, CST xl, CST xr, CST yt, CST yb, bool bFlickerFree = true, bool bInvalidate = false, 
			double xRatio = -1.0, double cxRatio = -1.0,double yRatio = -1.0, double cyRatio = -1.0);
	void SetMinSize(int dx, int dy);
	void SetMaxSize(int dx, int dy);
	void ShowSizeGrip(bool bShow);
	int UpdateControlRect(UINT nID, CRect* pnr);

	virtual BOOL OnInitDialog();
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* pmmi);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);

private:
	std::vector<CItemCtrl> m_Items; // array of controlled items
	CRect m_ScreenRect;
	CRect m_ScreenRectOrig;
	bool m_bShowSizeGrip;
	int m_xMin;
	int m_yMin;
	int m_xMax;
	int m_yMax;
	UINT m_nDelaySide;
};
