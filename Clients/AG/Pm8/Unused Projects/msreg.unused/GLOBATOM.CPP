/*
// $Workfile: GLOBATOM.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/GLOBATOM.CPP $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.0   31 Jan 1996 13:25:28   FRED
// Initial revision.
// 
//    Rev 1.1   26 Jan 1996 15:09:30   JAY
//  
// 
//    Rev 1.0   26 Jan 1996 08:40:26   FRED
// Initial revision.
*/ 

#include <windows.h>
#include "globatom.h"

/////////////////////////////////////////////////////////////////////////////
// CGlobalAtom Class

CGlobalAtom::CGlobalAtom(LPCSTR pszName)
{
	m_pszName = pszName;
	m_Atom = NULL;
}

CGlobalAtom::~CGlobalAtom()
{
	Destroy();
}

ATOM CGlobalAtom::Create(void)
{
	if (m_Atom == NULL)
	{
		m_Atom = GlobalAddAtom(m_pszName);
	}

	return m_Atom;
}

BOOL CGlobalAtom::Exists(void)
{
	return GlobalFindAtom(m_pszName) != NULL;
}

void CGlobalAtom::Destroy(void)
{
	if (m_Atom != NULL)
	{
		GlobalDeleteAtom(m_Atom);
		m_Atom = NULL;
	}
}
