/*
// $Workfile: GLOBATOM.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/GLOBATOM.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:33:42   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:28:38   FRED
// Initial revision.
// 
//    Rev 1.1   26 Jan 1996 15:09:30   JAY
//  
// 
//    Rev 1.0   26 Jan 1996 08:40:26   FRED
// Initial revision.
*/ 

#include "stdafx.h"
#include "globatom.h"

/////////////////////////////////////////////////////////////////////////////
// CGlobalAtom Class

CGlobalAtom::CGlobalAtom(LPCSTR pszName)
{
	if (pszName != NULL)
	{
		TRY
		{
			m_csName = pszName;
		}
		END_TRY
	}

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
		m_Atom = GlobalAddAtom(m_csName);
	}

	return m_Atom;
}

BOOL CGlobalAtom::Exists(void)
{
	return GlobalFindAtom(m_csName) != NULL;
}

void CGlobalAtom::Destroy(void)
{
	if (m_Atom != NULL)
	{
		GlobalDeleteAtom(m_Atom);
		m_Atom = NULL;
	}
}
