/*
// $Workfile: SALESTAX.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/SALESTAX.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:32   Fred
// Initial revision.
// 
//    Rev 1.2   09 Jul 1997 09:55:28   Fred
// Moved strings to resource file
// 
//    Rev 1.1   14 Oct 1996 13:34:44   Fred
// International Version
// 
//    Rev 1.2   11 Oct 1996 15:29:24   FRED
// International Version
// 
//    Rev 1.1   07 Mar 1996 16:44:44   JAY
// Removed Illinois tax.
// 
//    Rev 1.0   01 Mar 1996 17:35:34   FRED
// Initial revision.
*/

#include "stdafx.h"
#include "upgrade.h"
#include "util.h"
#include "salestax.h"

// The sales tax information. Initialized by InitInstance().
CSalesTaxList SalesTaxList;

/////////////////////////////////////////////////////////////////////////////
// CSalesTaxList

CSalesTaxList::CSalesTaxList()
{
}

CSalesTaxList::~CSalesTaxList()
{
}

void CSalesTaxList::Init(void)
{
	CString csTaxInformation;
	GetApp()->GetTaxInformation(csTaxInformation);
	SetNames(csTaxInformation);
	ASSERT((GetSize() % 2) == 0);
}

/////////////////////////////////////////////////////////////////////////////
// CSalesTaxComboBox window

CSalesTaxComboBox::CSalesTaxComboBox()
{
}

CSalesTaxComboBox::~CSalesTaxComboBox()
{
}
	
BEGIN_MESSAGE_MAP(CSalesTaxComboBox, CComboBox)
	//{{AFX_MSG_MAP(CSalesTaxComboBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSalesTaxComboBox message handlers

/////////////////////////////////////////////////////////////////////////////
// CSalesTaxComboBox helper methods

void CSalesTaxComboBox::Init(int nSelection /*=-1*/)
{
	ResetContent();
	
	// Add in the tax table entries.
	for (int i = 0; i < SalesTaxList.GetCount(); i++)
	{
		LPCSTR pszName = SalesTaxList.GetName(i);
		if (pszName != NULL)
		{
			int nIndex = AddString(pszName);
			if (nIndex >= 0)
			{
				SetItemData(nIndex, (DWORD)i);
			}
		}
	}
	
	SetCurrentTaxIndex(nSelection);
}

// Return current sales tax.
long CSalesTaxComboBox::CurrentTax(long lTotal)
{
	int nIndex = GetCurSel();
	if (nIndex >= 0)
	{
		int i = (int)GetItemData(nIndex);
		return ((lTotal*SalesTaxList.GetRate(i))+5000)/10000;
	}
	
	return 0;
}

// Set the current tax list index.
void CSalesTaxComboBox::SetCurrentTaxIndex(int nIndex)
{
	if (nIndex == -1)
	{
		CString csNone;
		Util::SafeLoadString(IDS_NO_TAX, csNone);
		nIndex = FindStringExact(-1, csNone);
	}
	if (nIndex >= 0)
	{
		SetCurSel(nIndex);
	}
}

// Report the sales tax to collect for a particular state.
long CSalesTaxComboBox::StateTax(LPCSTR pszState, long lTotal)
{
	for (int i = 0; i < SalesTaxList.GetCount(); i++)
	{
		LPCSTR pszName = SalesTaxList.GetName(i);
		if (pszName != NULL)
		{
			if (lstrcmpi(pszState, pszName) == 0)
			{
				return ((lTotal*SalesTaxList.GetRate(i))+5000)/10000;
			}
		}
	}
	
	return 0;
}

int CSalesTaxComboBox::StateTaxIndex(LPCSTR pszState)
{
	for (int i = 0; i < SalesTaxList.GetCount(); i++)
	{
		LPCSTR pszName = SalesTaxList.GetName(i);
		if (pszName != NULL)
		{
			if (lstrcmpi(pszState, pszName) == 0)
			{
				return i;
			}
		}
	}
	
	return -1;
}
