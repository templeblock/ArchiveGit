/*
// $Workfile: MSENGINE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/MSENGINE.H $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.1   28 Apr 1997 15:48:52   Fred
// Changed include to not reference V drive
// 
//    Rev 1.0   22 Nov 1995 08:51:58   FRED
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

#include ".\gcpp300\h\compplib.hpp"
#include ".\gcpp300\h\filexfer.hpp"

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
