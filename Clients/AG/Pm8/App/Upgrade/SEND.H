/*
// $Workfile: SEND.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/SEND.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:00   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:41:06   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:42   JAY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1995 10:27:46   JAY
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
