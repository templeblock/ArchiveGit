/*
// $Workfile: COMLINK.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/COMLINK.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     7/09/98 10:48a Mwilson
// changed include paths
// 
//    Rev 1.0   14 Aug 1997 15:41:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:34   Fred
// Initial revision.
// 
//    Rev 1.1   12 Jun 1997 16:30:44   Fred
// PMG 4.0 changes. Time passed in.
// 
//    Rev 1.0   08 Sep 1996 09:40:46   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:40   JAY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1995 10:27:42   JAY
// Initial revision.
// 
//    Rev 1.1   30 Oct 1995 13:07:30   FRED
// 
//    Rev 1.0   27 Oct 1995 16:34:44   JAY
// Initial revision.
*/ 

#ifndef __COMLINK_H__
#define __COMLINK_H__

#include "compplib.hpp"

/*
// Our wrapper for the GFComLink-derived class.
// This allows us to customize the GFModem class as necessary.
*/

#ifdef WIN32
class CComLink : public GFComWin32
#else
class CComLink : public GFComWin3
#endif
{
public:
	CComLink(int nPort);
	BOOL Valid(void)
		{ return handle >= 0; }
	int LastError(void)
		{ return lasterror; }
protected:
	virtual int SystemIdle(void);
};

#endif
