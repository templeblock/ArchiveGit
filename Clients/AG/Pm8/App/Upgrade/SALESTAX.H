/*
// $Workfile: SALESTAX.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/SALESTAX.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:58   Fred
// Initial revision.
// 
//    Rev 1.1   14 Oct 1996 13:34:54   Fred
// International Version
// 
//    Rev 1.1   11 Oct 1996 15:29:20   FRED
// International Version
// 
//    Rev 1.0   01 Mar 1996 17:35:34   FRED
// Initial revision.
*/

class CSalesTaxComboBox;

#ifndef __SALESTAX_H__
#define __SALESTAX_H__

#include "reginfo.h"

/////////////////////////////////////////////////////////////////////////////
// CSalesTaxList

class CSalesTaxList : public CNameList
{
public:
	CSalesTaxList();
	virtual ~CSalesTaxList();
	void Init(void);
public:
	int GetCount(void)
		{ return GetSize() / 2; }
	LPCSTR GetName(int nIndex)
		{ return ElementAt(nIndex*2+0); }
	long GetRate(int nIndex)
		{ return (long)(atof(ElementAt(nIndex*2+1))*100L); }
};
  
/////////////////////////////////////////////////////////////////////////////
// CSalesTaxComboBox window

class CSalesTaxComboBox : public CComboBox
{
public:
	CSalesTaxComboBox();
	virtual ~CSalesTaxComboBox();
	
protected:
	// Generated message map functions
	//{{AFX_MSG(CSalesTaxComboBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
public:
	void Init(int nSelection = -1);
	long CurrentTax(long lTotal);
	void SetCurrentTaxIndex(int nIndex);
	static long StateTax(LPCSTR pszState, long lTotal);
	static int StateTaxIndex(LPCSTR pszState);
};

extern CSalesTaxList SalesTaxList;

#endif
