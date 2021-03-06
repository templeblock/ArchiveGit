/*
// $Workfile: MSSERIAL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
//
// Copyright � 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/MSSERIAL.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:19:54   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:41:44   johno
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
// $Log: /PM8/App/LICDLL/MSSERIAL.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:32   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:04   JAY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1995 10:27:34   JAY
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
