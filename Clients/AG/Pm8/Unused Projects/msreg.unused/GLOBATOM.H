/*
// $Workfile: GLOBATOM.H $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/GLOBATOM.H $
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
//    Rev 1.0   26 Jan 1996 08:40:24   FRED
// Initial revision.
*/ 

class CGlobalAtom;

#ifndef __GLOBATOM_H__
#define __GLOBATOM_H__

/////////////////////////////////////////////////////////////////////////////
// CGlobalAtom Class

class CGlobalAtom
{
// Construction
public:
	CGlobalAtom(LPCSTR pszName = NULL);
	virtual ~CGlobalAtom();

	void Name(LPCSTR pszName)
		{ m_pszName = pszName; }
	LPCSTR Name(void) const
		{ return m_pszName; }

public:
	ATOM Create(void);
	ATOM Atom(void) const
		{ return m_Atom; }
	BOOL Exists(void);
	void Destroy(void);

protected:
	LPCSTR m_pszName;
	ATOM m_Atom;
};

#endif
