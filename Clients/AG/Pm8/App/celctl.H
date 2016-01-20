/*
// $Workfile: celctl.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/celctl.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 2     5/28/98 4:10p Jayn
// More cool table stuff (cell formatting, etc.)
// 
// 1     5/26/98 1:33p Jayn
*/ 

#ifndef __CELCTL_H__
#define __CELCTL_H__

#include "tbldef.h"				// For CTableBorderSegment, etc.

/////////////////////////////////////////////////////////////////////////////
// CCellOutlineControl window

class CCellOutlineControl : public CWnd
{
public:
// Construction
public:
	CCellOutlineControl();

// Implementation
public:
	virtual ~CCellOutlineControl();

	void Invalidate(void)
		{ InvalidateRect(NULL); }

	// Initialize the control.
	void Init(CTableCellFormatting* pFormatting);

	void SetFillColor(COLOR FillColor);
	void SetLineColor(COLOR LineColor);
	void SetLineWidth(int nWidth);
	int GetLineWidth(void);

	int GetSelectedSegment(void) const
		{ return m_nSelectedSegment; }

	CTableBorderSegment& GetSegment(int nSegment)
		{ return m_pFormatting->m_Segment[nSegment]; }


protected:
	// Generated message map functions
	//{{AFX_MSG(CCellOutlineControl)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
// stuff for supporting creation by class name
public:
	// Member function to register a window class
	static BOOL RegisterMyClass();

protected:	
	static WNDPROC m_pfnSuperWndProc ;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }

protected:	
	// Implementation
	// To keep me from going insane.
	enum
	{
		SEGMENT_None				= CTableCellFormatting::SEGMENT_None,
		SEGMENT_Left				= CTableCellFormatting::SEGMENT_Left,
		SEGMENT_Top					= CTableCellFormatting::SEGMENT_Top,
		SEGMENT_Right				= CTableCellFormatting::SEGMENT_Right,
		SEGMENT_Bottom				= CTableCellFormatting::SEGMENT_Bottom,
		SEGMENT_InnerVertical	= CTableCellFormatting::SEGMENT_InnerVertical,
		SEGMENT_InnerHorizontal = CTableCellFormatting::SEGMENT_InnerHorizontal,
		NUM_SEGMENTS				= CTableCellFormatting::NUM_SEGMENTS
	};

	CTableCellFormatting* m_pFormatting;

	CRect m_crClient;
	CRect m_crSegment[NUM_SEGMENTS];

	int m_nSelectedSegment;			// The enum above.

	void DrawSegment(CDC* pDC, RedisplayContext* rc, int nSegment, CPoint p0, CPoint p1, CPoint p0Extend, CPoint p1Extend);
	void ToggleSelectedSegment(CDC* pDC);

	enum
	{
		MARKER_PointsLeft,
		MARKER_PointsRight,
		MARKER_PointsDown,
		MARKER_PointsUp,
		NUMBER_OF_MARKERS
	};

	class CMarkerBitmap : public CBitmap
	{
	public:
		CSize m_czBitmap;
		CPoint m_cpOrigin;
	};
	CMarkerBitmap m_cbmMarker[NUMBER_OF_MARKERS];
	BOOL CreateMarkerBitmaps(void);
	BOOL CreateMarkerBitmap(int nMarker, UINT idBitmap, CSize czBitmap, CPoint cpOrigin);

private:		
	static LONG CALLBACK AFX_EXPORT FirstMsgOnlyWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam);

	BOOL m_fCreatedByClassName;
	virtual void PostNcDestroy();
};

/////////////////////////////////////////////////////////////////////////////

#endif
