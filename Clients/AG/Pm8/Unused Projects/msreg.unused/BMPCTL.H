/*
// $Workfile: BMPCTL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/BMPCTL.H $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.2   29 Mar 1996 14:02:08   JAY
// Extensions for file-based BMPs.
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
	static LPCSTR m_pszWorkingDirectory;

// stuff for supporting creation by class name
public:
	// Member function to register a window class
	static BOOL RegisterMyClass();

	// Member function to set working directory for bitmap files.
	static void SetWorkingDirectory(LPCSTR pszWorkingDirectory)
		{ m_pszWorkingDirectory = pszWorkingDirectory; }
	// Member function to get working directory for bitmap files.
	static LPCSTR GetWorkingDirectory(void)
		{ return m_pszWorkingDirectory; }

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
