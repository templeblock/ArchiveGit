/*
// $Workfile: MSSERIAL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/MSSERIAL.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     7/09/98 10:48a Mwilson
// changed include paths
// 
//    Rev 1.0   14 Aug 1997 15:41:58   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:46   Fred
// Initial revision.
// 
//    Rev 1.1   12 Jun 1997 16:30:46   Fred
// PMG 4.0 changes. Time passed in.
// 
//    Rev 1.0   08 Sep 1996 09:40:54   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:54   JAY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1995 10:27:42   JAY
// Initial revision.
// 
//    Rev 1.0   30 Oct 1995 12:12:30   FRED
//  
// 
//    Rev 1.1   27 Oct 1995 16:34:16   JAY
// Changes.
// 
//    Rev 1.0   27 Oct 1995 13:52:08   JAY
// Initial revision.
*/ 

#ifndef __MSSERIAL_H__
#define __MSSERIAL_H__

#include "compplib.hpp"

class CSerial : public GFSerial
{
public:
	CSerial(GFComLink* pComLink);
	void SetBaud(long lSpeed);
};

#endif
