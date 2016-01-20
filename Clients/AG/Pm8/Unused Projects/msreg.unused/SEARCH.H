/*
// $Workfile: SEARCH.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/SEARCH.H $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.1   05 Dec 1995 13:02:32   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.0   22 Nov 1995 08:51:54   FRED
// Initial revision.
// 
//    Rev 1.2   27 Oct 1995 16:13:16   FRED
// More communications stuff.
// 
//    Rev 1.1   27 Oct 1995 13:51:28   JAY
// Added GreenLeaf libarary
*/ 

#ifndef __SEARCH_H__
#define __SEARCH_H__

#include "basedlg.h"

// search.h : header file
//

class CModem;

/////////////////////////////////////////////////////////////////////////////
// CSearchingDialog dialog

class CSearchingDialog : public CBaseDialog
{
// Construction
public:
	CSearchingDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSearchingDialog)
	enum { IDD = IDD_SEARCHING };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	int m_nPort;
	long m_lSpeed;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSearchingDialog)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	UINT m_hTimer;
	CModem* m_pModem;

	// Some constants.
	enum
	{
		MaxOkAttempts = 2,
		LastPort = 9
	};

	// States for our state machine.
	enum SearchState
	{
		Closing,
		FindModem,
		FindSpeed,
		Succeeded,
		Failed,
		Cancelled
	};

	SearchState m_State;

	void SetState(SearchState State);

	void StopTimer(void);
	BOOL StartTimer(int nInterval);

	void ProcessState(void);
	void ProcessFindModem(void);
	void ProcessFindSpeed(void);

	void ShowStatus(LPCSTR pText);

	void EndDialog(int nResult);

	// Generated message map functions
	//{{AFX_MSG(CSearchingDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
