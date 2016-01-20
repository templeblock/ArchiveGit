/*
// $Workfile: MSMODEM.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/MSMODEM.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:41:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:44   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:40:54   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:54   JAY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1995 10:27:40   JAY
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
	CComLink* m_pComLink;
	CSerial* m_pSerialPort;

	BOOL m_fAborted;

	// Our overrides.
	virtual int AbortModemFunction(void);
	virtual void ModemPrinter(char c);
};

#endif
