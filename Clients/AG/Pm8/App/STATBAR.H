/*
// $Workfile: STATBAR.H $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/STATBAR.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:02   Fred
// Initial revision.
// 
//    Rev 1.7   18 Mar 1997 13:23:22   Jay
// Preview stuff
// 
//    Rev 1.6   11 Mar 1997 16:14:04   Jay
// Extensions to the workspace
// 
//    Rev 1.5   06 Feb 1997 09:12:20   Jay
// WM_COMMANDs on mouse click.
// 
//    Rev 1.4   17 Jan 1997 09:17:40   Jay
// New status panes.
// 
//    Rev 1.3   16 Jan 1997 13:02:56   Jay
// New status stuff. More cleanup.
// 
//    Rev 1.2   31 Jul 1996 11:57:42   Fred
// 16-bit compatibility
// 
//    Rev 1.1   23 Jul 1996 16:53:38   Fred
//  
// 
//    Rev 1.0   23 Jul 1996 08:16:50   Fred
// Initial revision.
*/

class CPmwStatusBar;

#ifndef __STATBAR_H__
#define __STATBAR_H__

//////////////////////////////////////////////////////////////////////////
// CPmwStatusBar

class CPmwStatusBar : public CStatusBar
{
	INHERIT(CPmwStatusBar, CStatusBar)

public:
	CPmwStatusBar();
	virtual ~CPmwStatusBar();

	BOOL Create(CWnd* pParentWnd);

	void SetEditPanes(void);
	void SetPreviewPanes(void);

#ifdef WIN32
public:
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
#endif

	// Overridable in WIN32. Our own function in WIN16.
	virtual void DrawItem(LPDRAWITEMSTRUCT);

protected:
	// Generated message map functions
	//{{AFX_MSG(CPmwStatusBar)
	afx_msg LRESULT OnSizeParent(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
#ifndef WIN32
	virtual void DoPaint(CDC* pDC);
#endif
	BOOL LoadIndicator(int nIndex, UINT uID);
	struct CIndicator
	{
      CIndicator();
		BOOL m_fValid;
		CBitmap m_Bitmap;
		CSize m_Size;
	} m_Indicators[3];
};

#endif

