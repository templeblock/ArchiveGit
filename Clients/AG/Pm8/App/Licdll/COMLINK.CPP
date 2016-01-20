/*
// $Workfile: COMLINK.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/COMLINK.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:35:58   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:19:40   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:41:32   johno
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
// $Log: /PM8/App/LICDLL/COMLINK.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:22   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:57:58   JAY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1995 10:27:34   JAY
// Initial revision.
// 
//    Rev 1.2   30 Oct 1995 14:05:16   JAY
// Handles WM_PAINT differently (correctly) now.
// 
//    Rev 1.1   27 Oct 1995 17:01:22   JAY
// 
//    Rev 1.0   27 Oct 1995 16:34:42   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "comlink.h"

CComLink::CComLink(int nPort)
#ifdef WIN32
	: GFComWin32(nPort)
#else
	: GFComWin3(nPort)
#endif
{
}

int CComLink::SystemIdle(void)
{
	MSG Msg;
	while (::PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE))
	{
		TRACE("PumpMessage (%x)\n", Msg.message);
		if (!AfxGetApp()->PumpMessage())
		{
			PostQuitMessage(0);
			return GCPP_USER_ABORT;
		}

		if (Msg.message == WM_PAINT)
		{
			break;
		}
	}

#ifdef WIN32
	return GFComWin32::SystemIdle();
#else
	return GFComWin3::SystemIdle();
#endif
}
