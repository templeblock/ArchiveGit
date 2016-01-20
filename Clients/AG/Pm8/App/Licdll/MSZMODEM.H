/*
// $Workfile: MSZMODEM.H $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/MSZMODEM.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 3     8/13/98 2:18p Mwilson
// changed paths for include files
// 
// 2     7/14/98 5:49p Psasse
// include files follow same path as in SourceSafe
// Deletion of ambiguous Corrupt file Message Box
// 
//    Rev 1.0   14 Aug 1997 15:36:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:30   Fred
// Initial revision.
// 
//    Rev 1.2   11 Jun 1997 11:10:46   Jay
// Changes
// 
//    Rev 1.1   10 Jun 1997 15:44:26   Fred
// Cleanup
// 
//    Rev 1.0   20 May 1997 10:40:48   johno
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
// $Log: /PM8/App/LICDLL/MSZMODEM.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 3     8/13/98 2:18p Mwilson
// changed paths for include files
// 
// 2     7/14/98 5:49p Psasse
// include files follow same path as in SourceSafe
// Deletion of ambiguous Corrupt file Message Box
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

#include "gcpp300\h\compplib.hpp"
#include "gcpp300\h\filexfer.hpp"

class CZmodem : public GFZmodem
{
public:
	CZmodem(GFSerial* pCom, GFMonitor* pMonitor = NULL);

	virtual int ReplaceFileEngine(GFFileIO* pNewFileEngine);
};

#endif
