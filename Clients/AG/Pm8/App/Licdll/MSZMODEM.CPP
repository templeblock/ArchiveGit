/*
// $Workfile: MSZMODEM.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/MSZMODEM.CPP $
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
// $Log: /PM8/App/LICDLL/MSZMODEM.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:32   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:06   JAY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1995 10:27:38   JAY
// Initial revision.
// 
//    Rev 1.1   31 Oct 1995 08:05:54   JAY
// 
//    Rev 1.0   31 Oct 1995 07:58:20   JAY
// Initial revision.
// 
//    Rev 1.0   30 Oct 1995 16:55:48   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "mszmodem.h"

CZmodem::CZmodem(GFSerial* pCom, GFMonitor* pMonitor /*=NULL*/)
	: GFZmodem(pCom, pMonitor)
{
}

int CZmodem::ReplaceFileEngine(GFFileIO* pNewFileEngine)
{
	int nResult = GFZmodem::ReplaceFileEngine(pNewFileEngine);
	if (nResult == 1)			// using user-defined engine.
	{
		use_default_file_engine = FALSE;
	}
	return nResult;
}
