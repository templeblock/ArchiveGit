/*
// $Workfile: GLOBATOM.H $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/GLOBATOM.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:38:00   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:22   FRED
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
		{ m_csName = pszName; }
	const CString& Name(void) const
		{ return m_csName; }

public:
	ATOM Create(void);
	ATOM Atom(void) const
		{ return m_Atom; }
	BOOL Exists(void);
	void Destroy(void);

protected:
	CString m_csName;
	ATOM m_Atom;
};

#endif
