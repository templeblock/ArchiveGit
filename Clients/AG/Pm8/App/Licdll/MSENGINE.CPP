/*
// $Workfile: MSENGINE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/MSENGINE.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:22   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:19:52   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:41:42   johno
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
// $Log: /PM8/App/LICDLL/MSENGINE.CPP $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:30   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:06   JAY
// Initial revision.
// 
//    Rev 1.0   29 Nov 1995 10:27:38   JAY
// Initial revision.
// 
//    Rev 1.2   31 Oct 1995 13:23:30   JAY
// Now overrides Open in file engine.
// 
//    Rev 1.1   31 Oct 1995 09:08:50   JAY
// Added GetNextName() override.
// 
//    Rev 1.0   30 Oct 1995 16:55:50   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "msengine.h"

CFileEngine::CFileEngine()
{
}

void CFileEngine::SetReceiveName(LPCSTR pName)
{
	m_csName = pName;
	m_fGotName = FALSE;
}

LPCSTR CFileEngine::GetNameToUse(LPCSTR pName)
{
	if (pName != NULL && m_csName.GetLength() != 0)
	{
	/* Substitute the receive name. */
		pName = m_csName;
	}
	return pName;
}

int CFileEngine::Initialize(char* pInputName /*=NULL*/, int nRecurseFlag /*=0*/)
{
	return GFDefaultFileEngine::Initialize((char*)GetNameToUse(pInputName), nRecurseFlag);
}

int CFileEngine::GetNextName(void)
{
	if (m_fGotName)
	{
	// No more names to get.
		return FALSE;
	}
	else
	{
		strcpy(input_name, m_csName);
		m_fGotName = TRUE;
		return TRUE;
	}
}

int CFileEngine::Open(char* pFileName, char* pMode)
{
	return GFDefaultFileEngine::Open((char*)GetNameToUse(pFileName), pMode);
}
