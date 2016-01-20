#if !defined(AFX_BUILDPROGRESS_H__104456E2_9278_11D1_9EE4_006008947D80__INCLUDED_)
#define AFX_BUILDPROGRESS_H__104456E2_9278_11D1_9EE4_006008947D80__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BuildProgress.h : header file
//

#include "profiledocfiles.h"
#include "RawData.h"
#include "thermobject.PC.h"
#include "resource.h"

#define WM_PROFILEFINISH	(WM_USER + 103)
// progress flags
#define BUILD_STATUS_INIT		0x0000
#define BUILD_STATUS_ERROR		0x0001
#define BUILD_STATUS_FINISHED	0x0002
#define BUILD_STATUS_UPDATE		0x0003

/////////////////////////////////////////////////////////////////////////////
// CBuildProgress dialog

class CBuildProgress : public CDialog
{
// Construction
public:
	CBuildProgress(long input, long data, ProcessingStates type, const char* name, CWnd* pParent = NULL);   // standard constructor
	virtual ~CBuildProgress();

// Dialog Data
	//{{AFX_DATA(CBuildProgress)
	enum { IDD = IDD_BUILD_DIALOG };
	CButton	m_btnCancel;
	CProgressCtrl	m_ctlProgress;
	CString	m_strMessage;
	//}}AFX_DATA


	ProfileDocFiles	*m_pProfile;
	RawData*	m_rawdata;
	McoHandle*		m_data;
	ProcessingStates m_type;
	HANDLE m_hThread;
	CWinThread* m_pThread;
	BOOL m_bTerminate;
	ThermObject * m_therm;

	CString m_title;	
	long	m_range;
	long	m_scale;
	short	m_state;
	UINT	m_nTimer;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBuildProgress)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	afx_msg LONG OnFinish( UINT, LONG );

protected:

	// Generated message map functions
	//{{AFX_MSG(CBuildProgress)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// This is the worker thread function
	friend UINT AFX_CDECL BuildProfile( LPVOID lParam);
	friend UINT AFX_CDECL BuildGamutSurface( LPVOID lParam);
	friend UINT AFX_CDECL BuildGamut( LPVOID lParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUILDPROGRESS_H__104456E2_9278_11D1_9EE4_006008947D80__INCLUDED_)
