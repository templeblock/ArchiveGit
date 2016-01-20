/*
// $Workfile: MSZMODEM.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/MSZMODEM.H $
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
//    Rev 1.0   31 Oct 1995 07:58:20   JAY
// Initial revision.
// 
//    Rev 1.0   30 Oct 1995 16:55:48   JAY
// Initial revision.
*/

#ifndef __MSZMODEM_H__
#define __MSZMODEM_H__

#include ".\gcpp300\h\compplib.hpp"
#include ".\gcpp300\h\filexfer.hpp"

class CZmodem : public GFZmodem
{
public:
	CZmodem(GFSerial* pCom, GFMonitor* pMonitor = NULL);

	virtual int ReplaceFileEngine(GFFileIO* pNewFileEngine);
};

#endif
