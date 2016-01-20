/*
// $Workfile: MSSERIAL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/MSSERIAL.CPP $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.0   22 Nov 1995 08:52:10   FRED
// Initial revision.
// 
//    Rev 1.0   30 Oct 1995 12:12:22   FRED
//  
// 
//    Rev 1.2   27 Oct 1995 16:34:18   JAY
// Changes.
// 
//    Rev 1.1   27 Oct 1995 16:13:20   FRED
// More communications stuff.
// 
//    Rev 1.0   27 Oct 1995 13:52:08   JAY
// Initial revision.
*/ 

#include "stdafx.h"
#include "msserial.h"

/*
// Constructor for CSerial.
*/

CSerial::CSerial(GFComLink* pComLink)
		: GFSerial(pComLink)
{
}

void CSerial::SetBaud(long lSpeed)
{
	GFDataFormat* pDataFormat = GetDataFormat();
	if (pDataFormat != NULL)
	{
		pDataFormat->BaudRate(lSpeed);
	}
}
