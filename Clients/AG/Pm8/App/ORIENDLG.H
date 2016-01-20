//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/ORIENDLG.H 1     3/03/99 6:08p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/ORIENDLG.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 6     5/31/98 5:18p Hforman
// added flags to erase full area and show 3d border
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef _ORIENDLG_H_
#define _ORIENDLG_H_

#include "pdlgclr.h"

/////////////////////////////////////////////////////////////////////////////
// CProjectMakePreview

class CProjectMakePreview : public CButton
{
// Construction
public:
	CProjectMakePreview(BOOL fEraseArea = TRUE, BOOL fDrawBorder = TRUE);
	virtual ~CProjectMakePreview();
	void FreeCBitmap();
	virtual void UpdateCBitmap(const char *cnName);
	
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProjectMakePreview)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	
protected:
	CString			m_csCBitmapName;
	LPBITMAPINFO	m_pCBitmap;

	BOOL				m_fEraseArea;		// erase areas not covered by actual preview?
	BOOL				m_fDrawBorder;		// draw 3D border?

	static WNDPROC m_pfnSuperWndProc;
	virtual WNDPROC* GetSuperWndProcAddr() { return &m_pfnSuperWndProc; }

};

/////////////////////////////////////////////////////////////////////////////
// COrientationDialog dialog

class COrientationDialog : public CPmwDialogColor
{
// Construction
private:

	VOID UpdateControls(void);

public:
// standard constructor
	COrientationDialog(NEWINFO_REF info, UINT id = 0, WORD wSoundID = 0, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(COrientationDialog)
	enum { IDD = IDD_ORIENT};
	int		m_subtype;
	int		m_orientation;
	int		m_nPanels;
	BOOL		m_fDoubleSided;
	//}}AFX_DATA

	int m_nType;

// Implementation
protected:
	CProjectMakePreview  m_Preview;

	virtual void 
   DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	
   void 
   ShowControl(UINT nID, BOOL fShow);

   void
   DoRadioClick (void);

	// Generated message map functions
	//{{AFX_MSG(COrientationDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnFinish();
	afx_msg void OnBack();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	VOID get_exit_info(NEWINFO_PTR info);
};

#endif
