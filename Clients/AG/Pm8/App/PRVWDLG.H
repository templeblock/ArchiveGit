/*
// $Workfile: PRVWDLG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/PRVWDLG.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:40   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:30:30   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:08   FRED
// Initial revision.
// 
//    Rev 2.1   26 Jun 1995 16:58:38   JAY
//  
*/

#ifndef __PRVWDLG_H__
#define __PRVWDLG_H__

/////////////////////////////////////////////////////////////////////////////
// CProgressDialog dialog

class CProgressDialog : public CDialog
{
// Construction
public:
	CProgressDialog(UINT Id, CWnd* pParent);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CProgressDialog)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
	static BOOL m_fUserAbort;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CProgressDialog)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	//}}AFX_MSG
//	afx_msg void OnDestroy();
//	afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	static BOOL abort_check(VOIDPTR data);
};

/////////////////////////////////////////////////////////////////////////////
// CBuildingPreview dialog

class CBuildingPreview : public CProgressDialog
{
// Construction
public:
	CBuildingPreview(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBuildingPreview)
	enum { IDD = IDD_BUILDING_PREVIEW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBuildingPreview)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CUpdatingClipboard dialog

class CUpdatingClipboard : public CProgressDialog
{
// Construction
public:
	CUpdatingClipboard(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CUpdatingClipboard)
	enum { IDD = IDD_UPDATING_CLIPBOARD };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CUpdatingClipboard)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif			// __PRVWDLG_H__
