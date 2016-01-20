/*
// $Workfile: PROPCTL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/PROPCTL.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:12   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:54   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:41:02   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:54   JAY
// Initial revision.
// 
//    Rev 1.1   06 Dec 1995 14:48:34   JAY
// Now works correctly with CTL3D.
// 
//    Rev 1.1   13 Nov 1995 16:07:22   JAY
// Fixed text control color bug
// 
//    Rev 1.0   10 Nov 1995 09:41:02   FRED
// Initial revision.
// 
//    Rev 1.2   30 Oct 1995 12:12:22   FRED
//  
// 
//    Rev 1.1   26 Oct 1995 10:27:42   FRED
//  
*/ 

class CMlsPropertyControl;
class CMlsColorTextControl;

#ifndef __PROPCTL_H__
#define __PROPCTL_H__

#include "propstr.h"

class CMlsPropertyControl : public CWnd
{
public:
	CMlsPropertyControl();
	virtual ~CMlsPropertyControl();

public:
	static void SubclassPropertyControls(CWnd* pWnd);

public:
	virtual CMlsPropertyString* Properties(void)
		{ return &m_Properties; }

protected:
	CMlsPropertyString m_Properties;

private:
	virtual void PostNcDestroy();
	
protected:	
	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }
};


class CMlsColorTextControl : public CMlsPropertyControl
{
public:
	CMlsColorTextControl();
	virtual ~CMlsColorTextControl();

public:

	// Generated message map functions
	//{{AFX_MSG(CMlsColorTextControl)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnSetText(WPARAM, LPARAM);

protected:
	virtual BOOL OnChildNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);

protected:
	HBRUSH m_hBackgroundBrush;
	COLORREF m_clBackgroundBrush;
};

#endif
