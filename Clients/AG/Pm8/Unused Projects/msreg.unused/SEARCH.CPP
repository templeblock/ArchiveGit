/*
// $workfile:   register.h  $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/SEARCH.CPP $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.2   01 Oct 1996 10:39:06   FRED
//  
// 
//    Rev 1.1   05 Dec 1995 13:02:38   JAY
// Now derived from CBaseDialog
// 
//    Rev 1.0   22 Nov 1995 08:52:06   FRED
// Initial revision.
// 
//    Rev 1.14   16 Nov 1995 16:26:52   JAY
// 
//    Rev 1.13   16 Nov 1995 16:12:46   JAY
// One more cancel-miss case.
// 
//    Rev 1.12   16 Nov 1995 10:31:20   FRED
//  
// 
//    Rev 1.11   01 Nov 1995 11:54:48   FRED
// Added Register by Phone
// 
//    Rev 1.10   31 Oct 1995 16:46:42   JAY
// Even better Cancel handling.
// 
//    Rev 1.9   31 Oct 1995 15:54:58   JAY
// Improvements to Cancel handling.
// 
//    Rev 1.8   31 Oct 1995 15:09:14   FRED
// Uses CWnd::CenterWindow()
// 
//    Rev 1.7   31 Oct 1995 14:47:22   FRED
// Check for already cancelled
// 
//    Rev 1.6   31 Oct 1995 14:39:16   FRED
// Only searches up to 2400 baud
// 
//    Rev 1.5   31 Oct 1995 12:01:48   FRED
//  
// 
//    Rev 1.4   30 Oct 1995 16:16:24   JAY
// Increased the timeouts to make 16-bit version and Zoom modem work together.
// 
//    Rev 1.3   30 Oct 1995 12:16:26   FRED
// 
//    Rev 1.2   30 Oct 1995 12:11:48   FRED
//  
// 
//    Rev 1.1   27 Oct 1995 13:51:28   JAY
// Added GreenLeaf libarary
*/ 

#include "stdafx.h"
#include "msregx.h"
#include "search.h"
#include "msmodem.h"
#include "util.h"
#include "propctl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define SEARCH_TIMER		1566			/* It's a long story... */

/////////////////////////////////////////////////////////////////////////////
// CSearchingDialog dialog

CSearchingDialog::CSearchingDialog(CWnd* pParent /*=NULL*/)
			: CBaseDialog(IDD, pParent)
{
	//{{AFX_DATA_INIT(CSearchingDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nPort = 0;
	m_lSpeed = 0;

	m_hTimer = 0;
}

BOOL CSearchingDialog::DestroyWindow() 
{
	StopTimer();

	return CBaseDialog::DestroyWindow();
}

void CSearchingDialog::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchingDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSearchingDialog, CBaseDialog)
	//{{AFX_MSG_MAP(CSearchingDialog)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchingDialog message handlers

BOOL CSearchingDialog::OnInitDialog() 
{
	CBaseDialog::OnInitDialog();

	CMlsPropertyControl::SubclassPropertyControls(this);

	CenterWindow();
	
	SetState(FindModem);

	return TRUE;
}

void CSearchingDialog::OnTimer(UINT nIDEvent) 
{
	switch (nIDEvent)
	{
		case SEARCH_TIMER:
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

void CSearchingDialog::OnCancel() 
{
	if (m_State != Cancelled)
	{
		m_nPort = -1;
		m_lSpeed = 0;

		CString csText;
		TRY
		{
			csText.LoadString(IDS_CancelOperation);
		}
		END_TRY

		ShowStatus(csText);
		SetState(Cancelled);

		if (m_pModem != NULL)
		{
			m_pModem->Abort();
		}
	}
}

void CSearchingDialog::StopTimer(void)
{
	if (m_hTimer != 0)
	{
		KillTimer(m_hTimer);
		m_hTimer = 0;
	}
}

BOOL CSearchingDialog::StartTimer(int nInterval)
{
	StopTimer();

	if (nInterval == -1)
	{
		return TRUE;
	}

	if (SetTimer(SEARCH_TIMER, nInterval, NULL))
	{
		m_hTimer = SEARCH_TIMER;
		return TRUE;
	}
	else
	{
	// What do we do if we fail?
		return FALSE;
	}
}

void CSearchingDialog::SetState(SearchState State)
{
	static int nStateInterval[] =
	{
		-1,				// Closing - Closing the dialog
		10,				// FindModem - Delay to start of modem search
		2000,				// FindSpeed - Delay to show good news from search
		3000,				// Succeeded - Delay to show good news from test
		3000,				// Failed - Delay to show bad news from search/test
		500				// Cancelling - Delay to show bad news from search/test
	};
	m_State = State;
	StartTimer(nStateInterval[State]);
}

/*
// Process the next state of our state machine.
*/

void CSearchingDialog::ProcessState(void)
{
	switch (m_State)
	{
		case FindModem:
		{
			ProcessFindModem();
			break;
		}
		case FindSpeed:
		{
			ProcessFindSpeed();
			break;
		}
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
		default:
		{
		/* Illegal state */
			ASSERT(FALSE);
			break;
		}
	}
}

void CSearchingDialog::EndDialog(int nResult)
{
	// Close down...
	SetState(Closing);
	CBaseDialog::EndDialog(nResult);
}

/*
// Handle the "find modem" state.
*/

void CSearchingDialog::ProcessFindModem(void)
{
	CString csFormat;
	CString csText;

	StopTimer();				// No more timers while we're in here.

	TRY
	{
	/*
	// Look through the ports.
	*/

		for (m_nPort = 1; m_nPort < LastPort; m_nPort++)
		{
			CModem Modem;

			m_pModem = &Modem;			// Allow user to cancel
			Modem.WaitForOk(1000, "OK");
			int nResult = Modem.Init(m_nPort, 1200);	// Be able to talk to all modems we support.
			if (m_State == Cancelled || m_nPort == -1)
			{
				nResult = GCPP_USER_ABORT;
			}
			if (nResult == GCPP_OK)
			{
				csFormat.LoadString(IDS_TryingComPort);
				csText.Format(csFormat, m_nPort);
				ShowStatus(csText);
				Modem.WaitForOk(1000, "OK");

				for (int nTry = 0; nTry < MaxOkAttempts; nTry++)
				{
					TRACE("Try write #%d...\n", nTry);
					Modem.GetPort()->RXFlush();
					nResult = Modem.Write("E1V1Q0", 1);
					if (m_State == Cancelled || m_nPort == -1)
					{
						nResult = GCPP_USER_ABORT;
					}
					if (nResult == GCPP_OK)
					{
						m_pModem = NULL;

					/* State that we are searching for the port. */

						csFormat.LoadString(IDS_FoundModemOnComPort);
						csText.Format(csFormat, m_nPort);
						ShowStatus(csText);

						SetState(FindSpeed);
						return;
					}

					TRACE("nResult == %d (%d)\n", nResult, m_nPort);
					if (nResult == GCPP_USER_ABORT)
					{
						nResult = GCPP_USER_ABORT;
						break;
					}
				}
			}
			m_pModem = NULL;

			if (nResult == GCPP_USER_ABORT || m_State == Cancelled)
			{
				return;
			}

		// Go to next port.
		}
	}
	END_TRY

	csText.LoadString(IDS_CantFindModem);
	ShowStatus(csText);
	SetState(Failed);
}

/*
// Handle the "find modem speed" state.
*/

void CSearchingDialog::ProcessFindSpeed(void)
{
	CString csFormat;
	CString csText;

	StopTimer();				// No more timers while we're in here.

	static long lSpeeds[] =
	{
		1200L,
		2400L,
//		4800L,
//		9600L,
		0L
	};

	TRY
	{
	/*
	// Look through the ports.
	*/

		CModem Modem;
		m_pModem = &Modem;			// Allow user to cancel
		Modem.WaitForOk(1000, "OK");

		int nResult = Modem.Init(m_nPort, 1200L);	// Be able to talk to all modems we support.
		m_lSpeed = 0;		// No speed found yet.

		if (m_State == Cancelled || m_nPort == -1)
		{
			nResult = GCPP_USER_ABORT;
		}

		if (nResult == GCPP_OK)
		{
			Modem.WaitForOk(1000, "OK");

			long lSpeed;
			for (int nSpeedIndex = 0; (lSpeed = lSpeeds[nSpeedIndex]) != 0L; nSpeedIndex++)
			{
				Modem.GetPort()->SetBaud(lSpeed);

				csFormat.LoadString(IDS_TestingComPort);
				csText.Format(csFormat, m_nPort, lSpeed);
				ShowStatus(csText);

				for (int nTry = 0; nTry < MaxOkAttempts; nTry++)
				{
					TRACE("Speed %ld: Try write #%d...\n", lSpeed, nTry);
					Modem.GetPort()->RXFlush();
					nResult = Modem.Write("E1V1Q0", 1);
					if (m_State == Cancelled || m_nPort == -1)
					{
						nResult = GCPP_USER_ABORT;
					}

					if (nResult == GCPP_OK)
					{
						m_lSpeed = lSpeed;			// This one tested OK!
						break;
					}

					if (nResult == GCPP_USER_ABORT)
					{
						break;
					}
				}

				if (m_lSpeed != lSpeed)
				{
				/* The last one failed. Break out now. */
					break;
				}
				// Go to next speed.
			}
		}

		m_pModem = NULL;

	//	TRACE("Done with result %d (%d)\n", nResult, m_State);
		if (nResult == GCPP_USER_ABORT || m_State == Cancelled)
		{
		// User abort. Just return.
			return;
		}
		else if (m_lSpeed == 0L)
		{
			csText.LoadString(IDS_CantFindModemSpeed);
			ShowStatus(csText);
			SetState(Failed);
		}
		else
		{
			csFormat.LoadString(IDS_TestedComPort);
			csText.Format(csFormat, m_nPort, m_lSpeed);
			ShowStatus(csText);
			SetState(Succeeded);
		}
	}
	END_TRY
}

/*
// Show some status in the dialog.
*/

void CSearchingDialog::ShowStatus(LPCSTR pText)
{
	CWnd* pWnd = GetDlgItem(IDC_SEARCH_STATUS);
	if (pWnd != NULL)
	{
		pWnd->SetWindowText(pText);
	}
}
