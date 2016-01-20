/*
// $Workfile: MSZMODEM.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/MSZMODEM.CPP $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.0   22 Nov 1995 08:52:10   FRED
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
