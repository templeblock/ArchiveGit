/*
// $Workfile: MSENGINE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/MSENGINE.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     7/09/98 10:48a Mwilson
// changed include paths
// 
//    Rev 1.0   14 Aug 1997 15:41:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:44   Fred
// Initial revision.
// 
//    Rev 1.1   12 Jun 1997 16:30:44   Fred
// PMG 4.0 changes. Time passed in.
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

#include "compplib.hpp"
#include "filexfer.hpp"

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
