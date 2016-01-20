/*
// $Header: /PM8/App/WARPPSH.H 1     3/03/99 6:13p Gbeddow $
//
// $Log: /PM8/App/WARPPSH.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:27:06   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:32   Fred
// Initial revision.
// 
//    Rev 1.1   28 Mar 1997 17:12:10   Jay
// Ripped out albums, sound manager, side bar, etc.
// 
//    Rev 1.0   14 Mar 1996 13:31:30   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:58   FRED
// Initial revision.
// 
//    Rev 1.9   20 Dec 1995 09:56:56   JAY
// 32-bit fixes
// 
//    Rev 1.8   22 Sep 1995 15:44:20   JAY
// Fix in WIN32 for misdirected tool bar updates.
// 
//    Rev 1.7   04 Sep 1995 12:32:48   FRED
//  
// 
//    Rev 1.6   02 Sep 1995 15:06:12   FRED
// More changes to make help work
// 
//    Rev 1.5   09 Jun 1995 07:43:42   FRED
// Sound Manager changes
// 
//    Rev 1.4   12 May 1995 14:55:20   FRED
// More warp effects preview stuff
// 
//    Rev 1.3   12 May 1995 10:35:16   FRED
// Warp text preview
// 
//    Rev 1.2   05 May 1995 17:10:02   FRED
//  
// 
//    Rev 1.1   02 May 1995 15:39:32   JAY
// More smart fields stuff.
// 
//    Rev 1.0   27 Apr 1995 13:30:08   FRED
// Initial revision.
*/

class CWarpTextSheet;
class CWarpTextSheetPreviewButton;

#ifndef __WARPPSH_H__
#define __WARPPSH_H__

#include "warptext.h"
#include "warpshap.h"
#include "warpfx.h"
#include "pmwprop.h"

class WarpTextObject;

/////////////////////////////////////////////////////////////////////////////
// CWarpTextSheetPreviewButton

class CWarpTextSheetPreviewButton : public CButton
{
// Construction
public:
	CWarpTextSheetPreviewButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWarpTextSheetPreviewButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWarpTextSheetPreviewButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWarpTextSheetPreviewButton)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CWarpTextSheet

class CWarpTextSheet : public CPmwPropertySheet
{
	DECLARE_DYNAMIC(CWarpTextSheet)

// Construction
public:
	CWarpTextSheet(WarpTextObject* pObject, CPmwView* pView, UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWarpTextSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWarpTextSheet();

	CWarpTextTextPage* GetTextPage(void)
		{ return &m_TextPage; }

	// Generated message map functions
protected:
	//{{AFX_MSG(CWarpTextSheet)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

#ifdef WIN32
	virtual BOOL OnInitDialog();
#else
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
#endif

	afx_msg void OnOK();

// Property pages
protected:
	void AddPages(void);
	CWarpTextTextPage m_TextPage;
	CWarpTextShapePage m_ShapePage;
	CWarpTextEffectsPage m_EffectsPage;
	
// Preview button
protected:
	CWarpTextSheetPreviewButton m_PreviewButton;
	
protected:
	void DrawPreview(LPDRAWITEMSTRUCT lpDrawItemStruct);
	CRect m_crPreview;
	CButton* m_pPreview;
	WarpTextObject* m_pPreviewObject;
	WarpTextObject* m_pObject;
	RedisplayContext m_RedisplayContext;
};

/////////////////////////////////////////////////////////////////////////////

#endif
