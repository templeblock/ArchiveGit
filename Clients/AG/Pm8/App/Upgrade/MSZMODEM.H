/*
// $Workfile: MSZMODEM.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/MSZMODEM.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     7/09/98 10:48a Mwilson
// changed include paths
// 
//    Rev 1.0   14 Aug 1997 15:42:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:48   Fred
// Initial revision.
// 
//    Rev 1.1   12 Jun 1997 16:30:46   Fred
// PMG 4.0 changes. Time passed in.
// 
//    Rev 1.0   08 Sep 1996 09:40:56   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:56   JAY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1995 10:27:48   JAY
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

#include "compplib.hpp"
#include "filexfer.hpp"

class CZmodem : public GFZmodem
{
public:
	CZmodem(GFSerial* pCom, GFMonitor* pMonitor = NULL);

	virtual int ReplaceFileEngine(GFFileIO* pNewFileEngine);
};

#endif
