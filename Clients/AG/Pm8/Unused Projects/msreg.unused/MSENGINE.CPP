/*
// $Workfile: MSENGINE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/MSENGINE.CPP $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.0   22 Nov 1995 08:52:10   FRED
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
