/*
// $Workfile: SEND.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/SEND.CPP $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.5   28 Apr 1997 15:48:28   Fred
// Changed include to not reference V drive
// 
//    Rev 1.4   01 Oct 1996 10:39:06   FRED
//  
// 
//    Rev 1.3   26 Jun 1996 07:04:06   FRED
// International Registration
// 
//    Rev 1.2   06 Dec 1995 09:52:24   JAY
// Added keys for "Mindscape" related text.
// 
//    Rev 1.1   05 Dec 1995 13:02:38   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.0   22 Nov 1995 08:52:08   FRED
// Initial revision.
// 
//    Rev 1.18   16 Nov 1995 10:31:20   FRED
//  
// 
//    Rev 1.17   31 Oct 1995 16:46:52   JAY
// Even better Cancel handling.
// 
//    Rev 1.16   31 Oct 1995 15:54:46   JAY
// Improvements to Cancel handling.
// 
//    Rev 1.15   31 Oct 1995 15:09:14   FRED
// Uses CWnd::CenterWindow()
// 
//    Rev 1.14   31 Oct 1995 14:47:22   FRED
// Check for already cancelled
// 
//    Rev 1.13   31 Oct 1995 13:23:30   JAY
// Sets overwrite option.
// 
//    Rev 1.12   31 Oct 1995 12:01:50   FRED
//  
// 
//    Rev 1.11   31 Oct 1995 08:38:46   JAY
// Changed some debugging outputs.
// 
//    Rev 1.10   31 Oct 1995 07:58:42   JAY
// Now uses out ZModem class.
// 
//    Rev 1.9   30 Oct 1995 17:07:16   JAY
// Now allocates the file engine since the library deletes it.
// 
//    Rev 1.8   30 Oct 1995 16:55:28   JAY
// Now uses CFileEngine.
// 
//    Rev 1.7   30 Oct 1995 16:15:32   JAY
// Now detects "NO DIALTONE" and "BUSY" as well as "NO CARRIER".
// 
//    Rev 1.6   30 Oct 1995 14:59:54   JAY
// More fixes.
// 
//    Rev 1.5   30 Oct 1995 14:05:14   JAY
// Progress!
// 
//    Rev 1.4   30 Oct 1995 12:11:50   FRED
//  
// 
//    Rev 1.3   27 Oct 1995 17:01:22   JAY
// 
//    Rev 1.2   27 Oct 1995 16:13:16   FRED
// More communications stuff.
// 
//    Rev 1.1   26 Oct 1995 10:27:54   FRED
//  
*/ 

// send.cpp : implementation file
//

#include "stdafx.h"
#include "msregx.h"
#include "send.h"
#include "msmodem.h"
#include "msregini.h"
#include ".\gcpp300\h\filexfer.hpp"
#include "util.h"
#include "propctl.h"
#include "cprogbar.h"
#include "msengine.h"
#include "mszmodem.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define SEND_TIMER		1566

/////////////////////////////////////////////////////////////////////////////
// CSendingMonitor class

class CSendingMonitor : public GFMonitor
{
public:
	CSendingMonitor();
	virtual ~CSendingMonitor();

	void SetModem(CModem* pModem)
		{ m_pModem = pModem; }

	void SetDialog(CSendingDialog* pDialog);
	void SetState(int nState);

	void Abort(void);

protected:
	CSendingDialog* m_pDialog;
	CProgressBar* m_pProgress;
	CModem* m_pModem;
	BOOL m_fAbort;
	int m_nState;

	virtual int TransferAbort(void);
	virtual int UserMonitor(int nCode);
	virtual void TransferMessage(char* pMessage);
};

CSendingMonitor::CSendingMonitor()
{
	m_fAbort = FALSE;
	m_pModem = NULL;
	m_pDialog = NULL;
	m_pProgress = NULL;
	m_nState = -1;
}

CSendingMonitor::~CSendingMonitor()
{
}

int CSendingMonitor::TransferAbort(void)
{
	return m_fAbort ? GCPP_USER_ABORT : GCPP_OK;
}

int CSendingMonitor::UserMonitor(int nCode)
{
#if 0
	TRACE("Mon: %d (%d) (%ld, %ld),(%ld, %ld), (%d, %d)\n",
			nCode,
			errors,
			packno, packex,
			nbytes, file_length,
			block_number, block_size
			);
#endif

	if (nCode == GCPP_OK)
	{
		if (m_pProgress != NULL)
		{
			int nPercent = -1;

			if (packex != 0)
			{
				nPercent = (int)((50L*packno)/packex);
			}
			else if (file_length != 0)
			{
				nPercent = (int)((50L*nbytes)/file_length);
			}
			if (nPercent != -1)
			{
				if (m_nState == CSendingDialog::Receiving)
				{
					nPercent += 50;
				}
			}
		}
	}
	return GFMonitor::UserMonitor(nCode);
}

void CSendingMonitor::TransferMessage(char* pMessage)
{
	TRACE("Xfer: %s\n", pMessage);
}

void CSendingMonitor::Abort(void)
{
	m_fAbort = TRUE;
	m_pModem->Abort();
}

void CSendingMonitor::SetDialog(CSendingDialog* pDialog)
{
	m_pDialog = pDialog;
	m_pProgress = (CProgressBar*)m_pDialog->GetDlgItem(IDC_PROGRESS_BAR);
	m_pProgress->ShowWindow(SW_HIDE);
}

void CSendingMonitor::SetState(int nState)
{
	m_nState = nState;

	if (m_pProgress != NULL)
	{
		switch (m_nState)
		{
			case CSendingDialog::Sending:
			{
				m_pProgress->SetPos(0);
				m_pProgress->ShowWindow(SW_SHOW);
				break;
			}
			case CSendingDialog::Receiving:
			{
				m_pProgress->SetPos(50);
				break;
			}
			case CSendingDialog::Succeeded:
			{
				m_pProgress->SetPos(100);
				break;
			}
			default:
			{
			// Nothing special.
				break;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSendingDialog dialog

CSendingDialog::CSendingDialog(int nPort, long lSpeed, LPCSTR pFileToSend, CWnd* pParent /*=NULL*/)
	: CBaseDialog(CSendingDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSendingDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nPort = nPort;
	m_lSpeed = lSpeed;

	m_csFileToSend = pFileToSend;
	m_hTimer = 0;
	m_pModem = NULL;
	m_pMonitor = NULL;

	m_fDialed = FALSE;
}

CSendingDialog::~CSendingDialog()
{
	DeleteAll();
}

void CSendingDialog::DeleteAll(void)
{
	delete m_pMonitor;
	m_pMonitor = NULL;
	delete m_pModem;
	m_pModem = NULL;
}

BOOL CSendingDialog::DestroyWindow() 
{
	DeleteAll();

	StopTimer();

	return CBaseDialog::DestroyWindow();
}

void CSendingDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendingDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSendingDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CSendingDialog)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendingDialog message handlers

BOOL CSendingDialog::OnInitDialog() 
{
	CBaseDialog::OnInitDialog();
	
	CMlsPropertyControl::SubclassPropertyControls(this);
	
	CenterWindow();

	CProgressBar* pProgress = (CProgressBar*)GetDlgItem(IDC_PROGRESS_BAR);
	pProgress->SetMinMaxPos(0, 100, 0);

	m_pModem = new CModem;

	// Set up the monitor.
	m_pMonitor = new CSendingMonitor;
	m_pMonitor->SetModem(m_pModem);
	m_pMonitor->SetDialog(this);

	CString csText;
	TRY
	{
		csText.LoadString(IDS_Working);
	}
	END_TRY

	ShowStatus(csText);
	SetState(Initializing);

	return TRUE;
}

void CSendingDialog::OnCancel() 
{
	if (m_State != Cancelled)
	{
		CString csText;
		TRY
		{
			csText.LoadString(IDS_CancelOperation);
		}
		END_TRY

		ShowStatus(csText);
		SetState(Cancelled);

		if (m_pMonitor != NULL)
		{
			m_pMonitor->Abort();
		}
		else if (m_pModem != NULL)
		{
			m_pModem->Abort();
		}
	}
}

void CSendingDialog::OnFailed()
{
	CString csText;
	TRY
	{
		csText.LoadString(IDS_UnableToCompleteOperation);
	}
	END_TRY

	ShowStatus(csText);
	SetState(Failed);
}

void CSendingDialog::OnTimer(UINT nIDEvent) 
{
	switch (nIDEvent)
	{
		case SEND_TIMER:
		{
			ProcessState();
			break;
		}
		default:
		{
			CBaseDialog::OnTimer(nIDEvent);
			break;
		}
	}
}

void CSendingDialog::StopTimer(void)
{
	if (m_hTimer != 0)
	{
		KillTimer(m_hTimer);
		m_hTimer = 0;
	}
}

BOOL CSendingDialog::StartTimer(int nInterval)
{
	StopTimer();

	if (nInterval == -1)
	{
		return TRUE;
	}

	if (SetTimer(SEND_TIMER, nInterval, NULL))
	{
		m_hTimer = SEND_TIMER;
		return TRUE;
	}
	else
	{
	// What do we do if we fail?
		return FALSE;
	}
}

void CSendingDialog::SetState(SearchState State)
{
	static int nStateInterval[] =
	{
		-1,			// Closing - Closing the dialog
		2000,			// Succeeded - Let the user see the happy message
		2000,			// Cancelled - Let the user know we're cancelling
		2000,			// Failed - Show the unhappy news for a while
		10,			// Initializing - Delay to let the dialog get shown
		0,				// Dialing
		0,				// Sending
		0,				// Receiving
	};
	m_State = State;
	if (m_pMonitor != NULL)
	{
	// Keep the monitor up-to-date.
		m_pMonitor->SetState(State);
	}
	StartTimer(nStateInterval[State]);
}

/*
// Process the next state of our state machine.
*/

void CSendingDialog::ProcessState(void)
{
	switch (m_State)
	{
		case Succeeded:
		{
			EndDialog(IDOK);
			break;
		}
		case Failed:
		{
			EndDialog(IDABORT);
			break;
		}
		case Cancelled:
		{
			EndDialog(IDCANCEL);
			break;
		}
		case Initializing:
		{
			ProcessInitializing();
			break;
		}
		case Dialing:
		{
			ProcessDialing();
			break;
		}
		case Sending:
		{
			ProcessSending();
			break;
		}
		case Receiving:
		{
			ProcessReceiving();
			break;
		}
		default:
		{
		/* Illegal state */
			ASSERT(FALSE);
			break;
		}
	}
}

void CSendingDialog::ProcessInitializing(void)
{
	StopTimer();						/* No more timers while we're here. */
	m_pModem->WaitForOk(1000);

	int nResult = m_pModem->Init(m_nPort, m_lSpeed);
	if (m_State == Cancelled)
	{
		nResult = GCPP_USER_ABORT;
	}

	switch (nResult)
	{
		case GCPP_OK:
		{
			SetState(Dialing);
			break;
		}
		case GCPP_USER_ABORT:
		{
			break;
		}
		default:
		{
			OnFailed();
			break;
		}
	}
}

void CSendingDialog::ProcessDialing(void)
{
	StopTimer();						/* No more timers while we're here */

	ASSERT(m_pModem != NULL);

// Build the phone number.
	CString csNumber;
	csNumber = GetApp()->GetModemNumber(TRUE);

	CString csStatus;
	csStatus.LoadString(IDS_Contacting);
	ShowStatus(csStatus);

	int nResult = m_pModem->Dial((LPSTR)(LPCSTR)csNumber);
	if (m_State == Cancelled)
	{
		nResult = GCPP_USER_ABORT;
	}
	switch (nResult)
	{
		case GCPP_OK:
		{
			m_fDialed = TRUE;

			long msecs = 50000L;
			CString csLine;
			const int nSize = 80;

			while (msecs > 0)
			{
				msecs = m_pModem->GetModemInput(csLine.GetBuffer(nSize), nSize, msecs);
				csLine.ReleaseBuffer();
				if (msecs > 0)
				{
					TRACE("GetModemInput: '%s'\n", (LPCSTR)csLine);
					if (csLine.Find("CONNECT") != -1)
					{
						TRACE("Found CONNECT... Breaking.\n");
						break;
					}
					else if (csLine.Find("NO CARRIER") != -1
								|| csLine.Find("BUSY") != -1
								|| csLine.Find("NO DIALTONE") != -1)
					{
						TRACE("Found error string... Breaking.\n");
						msecs = GCPP_CONNECT_FAILURE;		// Sounds good to me...
						break;
					}
				}
			}
			// See what happened...
			if (m_State == Cancelled)
			{
				msecs = GCPP_USER_ABORT;
			}

			if (msecs != GCPP_USER_ABORT)
			{
				if (msecs <= 0)
				{
					OnFailed();
				}
				else
				{
				// Made a successful connection.
				// Now go on to the file transfer.
					SetState(Sending);
				}
			}
			break;
		}
		case GCPP_USER_ABORT:
		{
			break;
		}
		default:
		{
			OnFailed();
			break;
		}
	}
}

void CSendingDialog::ProcessSending(void)
{
	StopTimer();						/* No more timers while we're here */

	CString csStatus;
	csStatus.LoadString(IDS_Sending);
	ShowStatus(csStatus);

	CZmodem Transfer(m_pModem->GetPort(), m_pMonitor);
	Transfer.SetOverwriteOption(ZMODEM_OVERWRITE_ALWAYS);

	int nResult = Transfer.SendFile((LPSTR)(LPCSTR)m_csFileToSend);
	if (m_State == Cancelled)
	{
		nResult = GCPP_USER_ABORT;
	}

	switch (nResult)
	{
		case GCPP_OK:
		{
			SetState(Receiving);
			break;
		}
		case GCPP_USER_ABORT:
		{
			break;
		}
		default:
		{
			OnFailed();
			break;
		}
	}
}

void CSendingDialog::ProcessReceiving(void)
{
	StopTimer();						/* No more timers while we're here */

	CString csStatus;
	csStatus.LoadString(IDS_Receiving);
	ShowStatus(csStatus);

	// Construct the transfer protocol.
	CZmodem Transfer(m_pModem->GetPort(), m_pMonitor);
	
	// Make our own file engine so we can specify the receive name.
	CFileEngine FileEngine;
	Transfer.ReplaceFileEngine(&FileEngine);
	FileEngine.SetReceiveName(m_csFileToSend);

	// Receive the file...
	int nResult = Transfer.ReceiveFile();
	if (m_State == Cancelled)
	{
		nResult = GCPP_USER_ABORT;
	}

	switch (nResult)
	{
		case GCPP_OK:
		{
			CString csText;
			csText.LoadString(IDS_OperationComplete);
			ShowStatus(csText);
			SetState(Succeeded);
			break;
		}
		case GCPP_USER_ABORT:
		{
			break;
		}
		default:
		{
			OnFailed();
			break;
		}
	}
}

void CSendingDialog::Hangup(void)
{
	if (m_fDialed)
	{
		m_fDialed = FALSE;

		StopTimer();				// No timers while we hang up.
		CString csText;
		csText.LoadString(IDS_HangingUp);
		ShowStatus(csText);
		m_pModem->Abort(FALSE);
		m_pModem->HangUp();
	}
}

void CSendingDialog::EndDialog(int nResult)
{
	// Hang up if need be.
	Hangup();
	// Close down...
	SetState(Closing);
	CBaseDialog::EndDialog(nResult);
}

/*
// Show some status in the dialog.
*/

void CSendingDialog::ShowStatus(LPCSTR pText)
{
	CWnd* pWnd = GetDlgItem(IDC_SEND_STATUS);
	if (pWnd != NULL)
	{
		pWnd->SetWindowText(pText);
	}
}
