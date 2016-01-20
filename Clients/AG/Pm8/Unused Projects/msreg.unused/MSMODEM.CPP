/*
// $Workfile: MSMODEM.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/MSMODEM.CPP $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.1   11 Dec 1995 16:30:58   JAY
// Win32s stuff
// 
//    Rev 1.0   22 Nov 1995 08:52:04   FRED
// Initial revision.
// 
//    Rev 1.2   30 Oct 1995 15:59:28   JAY
// Added our modem info so we can modify the init string.
// 
//    Rev 1.1   30 Oct 1995 13:06:54   FRED
// 
//    Rev 1.0   30 Oct 1995 12:12:20   FRED
//  
// 
//    Rev 1.2   27 Oct 1995 16:34:04   JAY
// Changes.
// 
//    Rev 1.1   27 Oct 1995 16:13:14   FRED
// More communications stuff.
// 
//    Rev 1.0   27 Oct 1995 13:51:50   JAY
// Initial revision.
*/ 

#include "stdafx.h"
#include "msmodem.h"
#include "comlink.h"

/*
// Our modem info.
// This is basically a copy of the default one from the Greenleaf libarry;
// we modify the init string to not set a specific result code level (used
// to be 'X1' - this disabled 'no dialtone', 'busy', etc). This is in line
// with what the Turbo Power library sent, so I feel fairly safe.
*/

GFModemInfo CModem::m_ModemInfo =
{
	"Hayes compatible",					// description of modem
	"AT",										// normal command prefix
	"\r",										// normal command terminator
	"V1 S0=0",								// modem initialization string
	"DT",										// tone dial code
	"DP",										// pulse dial code
	"~~~+++~~~ATH0",						// hangup code
	"Z",										// reset code
	"S%d=%d",								// code to set a register
	"S%d?",									// code to get a register
	"S0=1",									// auto answer on
	"S0=0",									// auto answer off
	"A",										// answer code
	"E1",										// echo on
	"E0",										// echo off
	"O",										// force modem to go online
	"H0",										// force modem to go on-hook
	"H1",										// force modem to go off-hook
	"V0",										// put modem in numeric response mode
	"V1",										// put modem in verbal response mode
	{"M0", "M1", "M2"},					// speaker settings
	{"X0", "X1", "X2", "X3", "X4", "X5", "X6", "X7"}	// result code settings
};

/*
// Constructor for CModem.
*/

CModem::CModem()
{
	m_pComLink = NULL;
	m_pSerialPort = NULL;

	/*
	// We haven't aborted yet.
	*/

	m_fAborted = FALSE;
}

CModem::~CModem()
{
	delete m_pSerialPort;
	delete m_pComLink;
}

/*
// Do the Init.
// This will fail if the modem is not there.
// m_nPort is 1 for COM1, 2 for COM2, etc.
*/

int CModem::Init(int nPort, long lSpeed)
{
	/*
	// Create the com link.
	*/

#ifdef WIN32
	if( GetWindowsVersion() == GCPP_SYS_WIN32S )
	{
		m_pComLink = CreateWin32Port(nPort-1 + COM1);
		if (m_pComLink == NULL)
		{
			return GCPP_ENOMEM_ERROR;
		}

	/* Validate the Win32s object. */

#if 1
		if (((GFComWin32s*)m_pComLink)->CommHandle == 0)
		{
			delete m_pComLink;
			m_pComLink = NULL;
			return GCPP_ERROR;
		}
#endif
	}
	else
	{
		CComLink* pComLink = new CComLink(nPort-1 + COM1);
		if (!(pComLink->Valid()))
		{
			int nError = pComLink->LastError();
			delete pComLink;
			m_pComLink = NULL;
			return nError;
		}
		m_pComLink = pComLink;
	}
#else
	m_pComLink = new CComLink(nPort-1 + COM1);
	if (!(m_pComLink->Valid()))
	{
		int nError = m_pComLink->LastError();
		delete m_pComLink;
		m_pComLink = NULL;
		return nError;
	}
#endif

	/*
	// Create the serial port.
	*/

	m_pSerialPort = new CSerial(m_pComLink);
	m_pSerialPort->SetBaud(lSpeed);

	/*
	// Install this port.
	*/

	return Install(m_pSerialPort, &m_ModemInfo);
}

void CModem::Abort(BOOL fAbort /*=TRUE*/)
{
	m_fAborted = fAbort;
}

int CModem::AbortModemFunction(void)
{
	if (m_fAborted)
	{
		return GCPP_USER_ABORT;
	}
	return GFModem::AbortModemFunction();
}

void CModem::ModemPrinter(char c)
{
	TRACE("%c", c);
}
