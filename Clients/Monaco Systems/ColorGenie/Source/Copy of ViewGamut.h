#pragma once

// ViewGamut.h : header file
//
#include "Qd3dGamutView.h"
#include "ProfilerDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CViewGamut dialog

class CViewGamut : public CDialog
{
// Construction
public:
//	CViewGamut(CProfileDoc* pDoc, int iDataType, CWnd* pParent = NULL);   // standard constructor
	CViewGamut(long data, int iDataType, const char* description, CWnd* pParent = NULL);   // standard constructor
	~CViewGamut();
	BOOL OpenModelFile(void);
	void SaveModelFile(void);
	BOOL BrowseForPathName(char *inPathName, BOOLEAN fOpen);
	void DoFlagCommand(unsigned long flag);
	void InitFlagMenu(void);

// Dialog Data
	//{{AFX_DATA(CViewGamut)
	enum { IDD = IDD_VIEWGAMUT };
	int m_iDataType;
	CProfileDoc* m_pDoc;
	CViewGamut** m_ppMyself;
	TQ3ViewerObject m_pViewer;
	CQd3dGamutView m_Qd3dGamut;
	//}}AFX_DATA

	CString	m_description;
	McoHandle m_data;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewGamut)
	public:
	BOOL DoModeless(CWnd* pParent, CViewGamut** ppHandleToDialog);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CViewGamut)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnActive();
	afx_msg void OnBadge();
	afx_msg void OnCamerabutton();
	afx_msg void OnClear();
	afx_msg void OnControlstrip();
	afx_msg void OnCopy();
	afx_msg void OnCut();
	afx_msg void OnDollybutton();
	afx_msg void OnDragAndDrop();
	afx_msg void OnDrawframe();
	afx_msg void OnExit();
	afx_msg void OnOpen();
	afx_msg void OnOrbitbutton();
	afx_msg void OnPaste();
	afx_msg void OnResetbutton();
	afx_msg void OnSaveas();
	afx_msg void OnTextmode();
	afx_msg void OnTruckbutton();
	afx_msg void OnUndo();
	afx_msg void OnZoombutton();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSysColorChange();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
