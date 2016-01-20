/*
// $Workfile: MSENGINE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/MSENGINE.H $
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
//    Rev 1.0   14 Aug 1997 15:36:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:26   Fred
// Initial revision.
// 
//    Rev 1.2   11 Jun 1997 11:10:44   Jay
// Changes
// 
//    Rev 1.1   10 Jun 1997 15:44:24   Fred
// Cleanup
// 
//    Rev 1.0   20 May 1997 10:40:42   johno
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
// $Log: /PM8/App/LICDLL/MSENGINE.H $
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
//    Rev 1.0   08 Sep 1996 09:40:52   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:56   JAY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1995 10:27:46   JAY
// Initial revision.
// 
//    Rev 1.2   31 Oct 1995 13:23:18   JAY
// Now overrides Open in file engine.
// 
//    Rev 1.1   31 Oct 1995 09:08:40   JAY
// Added GetNextName() override.
// 
//    Rev 1.0   30 Oct 1995 16:55:48   JAY
// Initial revision.
*/

#ifndef __MSENGINE_H__
#define __MSENGINE_H__

#include "gcpp300\h\compplib.hpp"
#include "gcpp300\h\filexfer.hpp"

class CFileEngine : public GFDefaultFileEngine
{
public:
	CFileEngine();
	void SetReceiveName(LPCSTR pName);
protected:
	BOOL m_fGotName;
	CString m_csName;
	virtual int Open(char* pFileName, char* pMode);
	virtual int Initialize(char* pInputName = NULL, int nRecurseFlag = 0);
	virtual int GetNextName(void);

	LPCSTR GetNameToUse(LPCSTR pName);
};

#endif
