/*
// $Workfile: MSSERIAL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/MSSERIAL.H $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.1   28 Apr 1997 15:48:54   Fred
// Changed include to not reference V drive
// 
//    Rev 1.0   22 Nov 1995 08:51:58   FRED
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

#include ".\gcpp300\h\compplib.hpp"

class CSerial : public GFSerial
{
public:
	CSerial(GFComLink* pComLink);
	void SetBaud(long lSpeed);
};

#endif
