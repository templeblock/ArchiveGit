/*
// $Workfile: STATICE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/STATICE.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 2     6/09/98 10:31a Mwilson
// changed defaults to transparent bkgnd
// 
//    Rev 1.0   14 Aug 1997 15:26:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:04   Fred
// Initial revision.
// 
//    Rev 1.2   29 May 1997 11:50:42   dennisp
// Added support for SetWindowText()
// 
//    Rev 1.1   25 Apr 1997 11:43:22   Jay
// Added GetSuperWndProc
// 
//    Rev 1.0   22 Apr 1997 12:23:18   Fred
// Initial revision.
*/

class CStaticExtended;

#ifndef __STATICE_H__
#define __STATICE_H__


/////////////////////////////////////////////////////////////////////////////
// CStaticExtended window

class CStaticExtended : public CStatic
{
// Construction
public:
	CStaticExtended();

// Attributes
public:

// Operations
public:
   void SetFormat(UINT nFormat)
      {  m_nFormat = nFormat; }
   void SetBkColor(COLORREF rgbColor)
      {  m_rgbBkColor = rgbColor; }
   void SetTextColor(COLORREF rgbColor)
      {  m_rgbTextColor = rgbColor; }
   void SetPalette(HPALETTE hPalette)
      {  m_hPalette = hPalette; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticExtended)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStaticExtended();

protected:
   UINT           m_nFormat;
   COLORREF       m_rgbBkColor;
   COLORREF       m_rgbTextColor;
   HPALETTE       m_hPalette;
   CString        m_strWindowText;
   BOOL           m_bInitialized;

	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }

	// Generated message map functions
protected:
	//{{AFX_MSG(CStaticExtended)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif

