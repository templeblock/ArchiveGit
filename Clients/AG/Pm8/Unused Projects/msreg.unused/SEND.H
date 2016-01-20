/*
// $Workfile: SEND.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/SEND.H $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.1   05 Dec 1995 13:02:34   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.0   22 Nov 1995 08:51:56   FRED
// Initial revision.
// 
//    Rev 1.4   30 Oct 1995 14:05:06   JAY
// Progress!
// 
//    Rev 1.3   27 Oct 1995 17:01:22   JAY
// 
//    Rev 1.2   27 Oct 1995 16:13:14   FRED
// More communications stuff.
// 
//    Rev 1.1   26 Oct 1995 10:27:52   FRED
//  
*/ 

// send.h : header file
//

class CSendingDialog;

#ifndef __SEND_H__
#define __SEND_H__

#include "basedlg.h"

class CModem;
class CSendingMonitor;

/////////////////////////////////////////////////////////////////////////////
// CSendingDialog dialog

class CSendingDialog : public CBaseDialog
{
// Construction
public:
	CSendingDialog(int nPort, long lSpeed, LPCSTR pFileToSend, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSendingDialog();

// Dialog Data
	//{{AFX_DATA(CSendingDialog)
	enum { IDD = IDD_SENDING };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


	// States for our state machine.
	enum SearchState
	{
		Closing,
		Succeeded,
		Cancelled,
		Failed,
		Initializing,
		Dialing,
		Sending,
		Receiving
	};

	SearchState GetState(void) const
		{ return m_State; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSendingDialog)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	int m_nPort;
	long m_lSpeed;

	UINT m_hTimer;
	CModem* m_pModem;
	CSendingMonitor* m_pMonitor;

	CString m_csFileToSend;

	SearchState m_State;
	BOOL m_fDialed;

	void DeleteAll(void);

	void SetState(SearchState State);

	void OnFailed(void);

	void StopTimer(void);
	BOOL StartTimer(int nInterval);

	void ProcessState(void);
	void ProcessInitializing(void);
	void ProcessDialing(void);
	void ProcessSending(void);
	void ProcessReceiving(void);

	void ShowStatus(LPCSTR pText);

	void EndDialog(int nResult);

	void Hangup(void);

	// Generated message map functions
	//{{AFX_MSG(CSendingDialog)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
