#pragma once

// ViewGamut1.h : header file
//
#include "Qd3dControl.h"
#include "ProfilerDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CViewGamut1 dialog

class CViewGamut1 : public CDialog
{
// Construction
public:
	CViewGamut1(CProfileDoc* pDoc, CWnd* pParent = NULL);   // standard constructor
	~CViewGamut1();

// Dialog Data
	//{{AFX_DATA(CViewGamut1)
	enum { IDD = IDD_VIEWGAMUT1 };
	CProfileDoc* m_pDoc;
	CViewGamut1** m_ppMyself;
	CQd3dControl* m_pControl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewGamut1)
	public:
	BOOL DoModeless(CWnd* pParent, CViewGamut1** ppHandleToDialog);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewGamut1)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
