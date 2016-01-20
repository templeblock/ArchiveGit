/*
// $Workfile: MSMODEM.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/MSMODEM.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:19:52   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:41:42   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/MSMODEM.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:30   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:00   JAY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1995 10:27:34   JAY
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

	m_pComLink = new CComLink(nPort-1 + COM1);
	
	if (!(m_pComLink->Valid()))
	{
		int nError = m_pComLink->LastError();
		delete m_pComLink;
		m_pComLink = NULL;
		return nError;
	}

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
