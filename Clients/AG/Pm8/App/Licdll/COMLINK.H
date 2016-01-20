/*
// $Workfile: COMLINK.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/COMLINK.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 3     8/13/98 2:18p Mwilson
// changed paths for include files
// 
// 2     7/14/98 5:49p Psasse
// include files follow same path as in SourceSafe
// Deletion of ambiguous Corrupt file Message Box
// 
//    Rev 1.0   14 Aug 1997 15:36:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:14   Fred
// Initial revision.
// 
//    Rev 1.2   11 Jun 1997 11:10:44   Jay
// Changes
// 
//    Rev 1.1   10 Jun 1997 15:44:12   Fred
// Cleanup
// 
//    Rev 1.0   20 May 1997 10:40:28   johno
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
// $Log: /PM8/App/LICDLL/COMLINK.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 3     8/13/98 2:18p Mwilson
// changed paths for include files
// 
// 2     7/14/98 5:49p Psasse
// include files follow same path as in SourceSafe
// Deletion of ambiguous Corrupt file Message Box
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

#include "gcpp300\h\compplib.hpp"

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
