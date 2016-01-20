/*
// $Workfile: OD.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:08p $
//
// Copyright (C) 1990-1992 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/OD.CPP $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:36:52   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:29:48   FRED
// Initial revision.
// 
//    Rev 2.2   27 Aug 1995 18:30:44   FRED
// Changes to make install program not reference OLE
// 
//    Rev 2.1   08 Feb 1995 13:37:56   JAY
// Reverted. New series.
// 
//    Rev 1.2   04 Aug 1994 07:34:38   JAY
// Large model and 32-bit changes
// 
//    Rev 1.1   18 Jul 1994 10:08:26   JAY
// Now uses global to enable messages instead of an app variable.
# 
#    Rev 1.0   16 Dec 1993 16:28:00   JAY
#  
*/ 


#include "stdafx.h"

#include	<stdarg.h>

BOOL near fWantDebugMessages = FALSE;

void od(LPSTR lpFormat, ...)
{
	if (fWantDebugMessages)
	{
		BYTE buffer[256];
		va_list ap;

		va_start(ap, lpFormat);

		wvsprintf((LPSTR)buffer, lpFormat, (LPSTR)ap);

		OutputDebugString((LPSTR)buffer);

		va_end(ap);
	}
}
