#if !defined(AFX_CONNMGRCTL_H__94FB2C53_512E_11D2_8BEF_00A0C9B12C3D__INCLUDED_)
#define AFX_CONNMGRCTL_H__94FB2C53_512E_11D2_8BEF_00A0C9B12C3D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// ConnMgrCtl.h : Declaration of the CConnMgrCtrl ActiveX Control class.
#include "CConnMgr.h"

/////////////////////////////////////////////////////////////////////////////
// CConnMgrCtrl : See ConnMgrCtl.cpp for implementation.

class CConnMgrCtrl : public COleControl
{
	DECLARE_DYNCREATE(CConnMgrCtrl)

// Constructor
public:
	CConnMgrCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConnMgrCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual DWORD GetControlFlags();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CConnMgrCtrl();

	DECLARE_OLECREATE_EX(CConnMgrCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CConnMgrCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CConnMgrCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CConnMgrCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CConnMgrCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CConnMgrCtrl)
	afx_msg long Connect();
	afx_msg BOOL IsConnected();
	afx_msg long EditPrefs();
	afx_msg BOOL HangUp();
	afx_msg BOOL EnableIsConnectedCheck(BOOL bEnable);
	afx_msg BOOL EnableHangUpPrompt(BOOL bEnable);
	afx_msg void SetURL(LPCTSTR szURL);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

// Event maps
	//{{AFX_EVENT(CConnMgrCtrl)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CConnMgrCtrl)
	dispidConnect = 1L,
	dispidIsConnected = 2L,
	dispidEditPrefs = 3L,
	dispidHangUp = 4L,
	dispidEnableIsConnectedCheck = 5L,
	dispidEnableHangUpPrompt = 6L,
	dispidSetURL = 7L,
	//}}AFX_DISP_ID
	};

   CConnectionManager   m_connMgr;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONNMGRCTL_H__94FB2C53_512E_11D2_8BEF_00A0C9B12C3D__INCLUDED)
