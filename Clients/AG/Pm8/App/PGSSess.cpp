// PGSSess.cpp  

#include "stdafx.h"

#ifndef ARTSTORE

#include "pmw.h"
#include "pmwinet.h"
#include "PGSSess.h"
#include "progstat.h"
#include "pmwcfg.h"


/////////////////////////////////////////////////////////////////////////////
// Globals

/////////////////////////////////////////////////////////////////////////////
// CPGSSession object

// PGSSession wants to use its own derivative of the CInternetSession class
// just so it can implement an OnStatusCallback() override.

CPGSSession::CPGSSession(LPCTSTR pszAppName, int nMethod)
	: CInternetSession(pszAppName, 1, nMethod)
{
	m_pStatusDialog = NULL;
	m_fAbort = FALSE;
}

void CPGSSession::OnStatusCallback(DWORD dwContext, DWORD dwInternetStatus,
	LPVOID /* lpvStatusInfomration */, DWORD /* dwStatusInformationLen */)
{
	if (m_pStatusDialog != NULL)
	{

		UINT uID = 0;
		switch (dwInternetStatus)
		{
			case INTERNET_STATUS_CONNECTING_TO_SERVER:
			{
				// "Connecting to server..."
				uID = IDS_InternetConnecting;
				break;
			}
			case INTERNET_STATUS_SENDING_REQUEST:
			{
				// "Sending request..."
				uID = IDS_InternetSending;
				break;
			}
			case INTERNET_STATUS_RECEIVING_RESPONSE:
			{
				// "Receiving data..."
				uID = 0; //IDS_InternetReceiving;
				break;
			}
			case INTERNET_STATUS_CLOSING_CONNECTION:
			{
				// "Closing connection..."
				uID = IDS_InternetDisconnecting;
				break;
			}
//			case INTERNET_STATUS_RESOLVING_NAME:
//			case INTERNET_STATUS_NAME_RESOLVED:
//			case INTERNET_STATUS_CONNECTED_TO_SERVER:
//			case INTERNET_STATUS_REQUEST_SENT:
//			case INTERNET_STATUS_RESPONSE_RECEIVED:
//			case INTERNET_STATUS_CTL_RESPONSE_RECEIVED:
//			case INTERNET_STATUS_PREFETCH:
//			case INTERNET_STATUS_CONNECTION_CLOSED:
//			case INTERNET_STATUS_HANDLE_CREATED:
//			case INTERNET_STATUS_HANDLE_CLOSING:
//			case INTERNET_STATUS_REQUEST_COMPLETE:
//			case INTERNET_STATUS_REDIRECT:
			default:
			{
				break;
			}
		}

		if (uID != 0)
		{
			CString csStatus;
			LoadConfigurationString(uID, csStatus);
			m_pStatusDialog->SetStatusText(csStatus);
		}
	}
	TRACE("OSC - context: %ld, status: %ld\n", dwContext, dwInternetStatus);
	//if (!bProgressMode)
	//	return;

	//if (dwInternetStatus == INTERNET_STATUS_CONNECTED_TO_SERVER)
		//AfxMessageBox(_T("Connection made!"));
		//cerr << _T("Connection made!") << endl;
}

#endif
