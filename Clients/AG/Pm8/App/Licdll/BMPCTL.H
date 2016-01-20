/*
// $Workfile: BMPCTL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/BMPCTL.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:35:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:12   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:40:20   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/BMPCTL.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:44   Fred
// Initial revision.
// 
//    Rev 1.1   02 Feb 1996 13:44:02   FRED
// Added palette support to bitmap control
// 
//    Rev 1.1   02 Feb 1996 09:51:50   FRED
// Palette Support
// 
//    Rev 1.0   22 Nov 1995 08:51:50   FRED
// Initial revision.
// 
//    Rev 1.1   26 Oct 1995 10:27:42   FRED
//  
// 
//    Rev 1.0   24 Oct 1995 16:55:22   JAY
// Initial revision.
*/ 

class CBitmapControl;

#ifndef __BMPCTL_H__
#define __BMPCTL_H__

class CDDB;

/////////////////////////////////////////////////////////////////////////////
// CBitmapControl window

class CBitmapControl : public CWnd
{
// flags and constants
public:
	// Control specific styles
	enum
	{
		SBS_NOSIZECONTROL	=	0x0001,
		SBS_SIZEBITMAP		=	0x0002
	};

// Construction
public:
	CBitmapControl();

// Implementation
public:
	virtual ~CBitmapControl();

protected:
	// Generated message map functions
	//{{AFX_MSG(CBitmapControl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	CWnd *m_pcwAssociate;			// Associate Window
	DWORD m_dwStyle;					// Copy of GetWindowLong(GWL_STYLE)
	CDDB* m_pDDB;

// stuff for supporting creation by class name
public:
	// Member function to register a window class
	static BOOL RegisterMyClass();

protected:	
	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }
	
	BOOL SetBitmapResource(void);
	BOOL LoadBitmapResource(void);
	void FreeBitmapResource(void);
	
	int m_nBitmapWidth;
	int m_nBitmapHeight;
  
private:		
	static LONG CALLBACK AFX_EXPORT FirstMsgOnlyWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam);

	BOOL m_fCreatedByClassName;
	virtual void PostNcDestroy(void);
};

/////////////////////////////////////////////////////////////////////////////

#endif
