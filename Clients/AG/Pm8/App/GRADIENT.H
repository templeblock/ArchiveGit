/*
// $Workfile: GRADIENT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/GRADIENT.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:38:02   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:24   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:39:30   JAY
// Reverted. New series.
// 
//    Rev 1.1   21 Jun 1994 14:41:16   FRED
// Now derived from CFrameWnd instead of CWnd.
// 
// 
//    Rev 1.0   15 Jun 1994 18:59:18   FRED
// Initial revision.
*/ 

#ifndef _GRADIENT_H_
#define _GRADIENT_H_

/////////////////////////////////////////////////////////////////////////////
// CGradientWindow window

class CGradientWindow : public CFrameWnd
{
// Construction
public:
	CGradientWindow();

// Attributes
public:

// Operations
public:
	virtual void SetColor(COLORREF crStartColor = RGB(0,0,255), COLORREF crEndColor = RGB(0,0,0));
	virtual BOOL CGradientWindow::Create(
		DWORD dwExtStyle,
		LPCSTR lpszClassName,
		LPCSTR lpszWindowName,
		DWORD dwStyle,
		const RECT& rcPosition,
		CWnd* pParentWnd,
		UINT nID,
		CCreateContext* pContext);

// Implementation
public:
	virtual ~CGradientWindow();

protected:
	// Generated message map functions
	//{{AFX_MSG(CGradientWindow)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
private:
	static BYTE ScaleColor(BYTE bStart, BYTE bEnd, int nMinimum, int nCurrent, int nMaximum);
	BYTE m_bStartRed;
	BYTE m_bStartGreen;
	BYTE m_bStartBlue;
	BYTE m_bEndRed;
	BYTE m_bEndGreen;
	BYTE m_bEndBlue;
};

/////////////////////////////////////////////////////////////////////////////

#endif		// ifndef _GRADIENT_H_
