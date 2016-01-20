/*
// $Workfile: MSMODEM.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/MSMODEM.H $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.1   11 Dec 1995 16:30:58   JAY
// Win32s stuff
// 
//    Rev 1.0   22 Nov 1995 08:51:56   FRED
// Initial revision.
// 
//    Rev 1.2   30 Oct 1995 15:59:44   JAY
// Added our modem info so we can modify the init string.
// 
//    Rev 1.1   30 Oct 1995 13:07:22   FRED
// 
//    Rev 1.0   30 Oct 1995 12:12:28   FRED
//  
// 
//    Rev 1.1   27 Oct 1995 16:13:20   FRED
// More communications stuff.
// 
//    Rev 1.0   27 Oct 1995 13:51:52   JAY
// Initial revision.
*/ 

#ifndef __MSMODEM_H__
#define __MSMODEM_H__

#include "msserial.h"
#include "comlink.h"

/*
// Our wrapper for GFModem.
// This allows us to customize the GFModem class as necessary.
*/

class CModem : public GFModem
{
public:
	CModem(void);
	virtual ~CModem(void);

	int Init(int nPort, long lSpeed);

	void Abort(BOOL fAbort = TRUE);
	CSerial* GetPort(void) const
		{ return m_pSerialPort; }

	static GFModemInfo m_ModemInfo;
protected:
	// Our links.
#ifdef WIN32
	GFComLink* m_pComLink;
#else
	CComLink* m_pComLink;
#endif
	CSerial* m_pSerialPort;

	BOOL m_fAborted;

	// Our overrides.
	virtual int AbortModemFunction(void);
	virtual void ModemPrinter(char c);
};

#endif
