/*
// $Workfile: PROGRESS.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PROGRESS.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:34   Fred
// Initial revision.
// 
//    Rev 1.1   05 Aug 1996 18:55:32   Fred
//  
// 
//    Rev 1.0   09 Jul 1996 16:33:18   Fred
// Initial revision.
*/

class CMlsProgressBar;

#ifndef __PROGRESS_H__
#define __PROGRESS_H__

/////////////////////////////////////////////////////////////////////////////
// MLS Progress Bar
//

// CMlsProgressBar
class CMlsProgressBar : public CWnd
{
	INHERIT(CMlsProgressBar, CWnd)
	
public:
// Construction
	CMlsProgressBar();

// Implementation
public:
	virtual ~CMlsProgressBar();

protected:
	// Generated message map functions
	//{{AFX_MSG(CMlsProgressBar)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void Initialize(UINT nID, CWnd* pParent);

protected:
	void DrawBorder(CDC& dc, CRect& crRect);

protected:
	virtual void DrawProgressBar(CDC* pDC);

protected:
	COLORREF m_clBackground;
	COLORREF m_clBackgroundText;
	COLORREF m_clForeground;
	COLORREF m_clForegroundText;

protected:
	double m_dPosition;
	double m_dMinimum;
	double m_dMaximum;
	
	CString m_csFormat;

public:
	void SetRange(double dMinimum, double dMaximum);
	void SetPosition(double dPosition);
};

#endif
