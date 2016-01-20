/*
// $Workfile: OWNERLB.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright (C) 1994 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/OWNERLB.CPP $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:42:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:33:24   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:40:34   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:16   JAY
// Initial revision.
// 
//    Rev 1.4   21 Jan 1995 10:54:50   FRED
// Changes to make free items listed at
// the end of the phone order list box.
// 
// 
//    Rev 1.3   22 Dec 1994 15:38:00   FRED
// 
//    Rev 1.2   22 Dec 1994 15:16:52   FRED
//  
// 
//    Rev 1.1   13 Dec 1994 18:29:20   FRED
//  
// 
//    Rev 1.0   11 Dec 1994 11:01:58   FRED
// Initial revision.
*/

// ownerlb.cpp : implementation file
//

#include "stdafx.h"
//#include "catalog.h"
#include "ownerlb.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COwnerDrawListBoxItem object
//
// One of these objects is attached to each item in the owner draw list box.

COwnerDrawListBoxItem::COwnerDrawListBoxItem()
{
}

COwnerDrawListBoxItem::~COwnerDrawListBoxItem()
{
}

void COwnerDrawListBoxItem::DrawItem(COwnerDrawListBox* pList, LPDRAWITEMSTRUCT pDrawItemStruct)
{
}

void COwnerDrawListBoxItem::MeasureItem(COwnerDrawListBox* pList, LPMEASUREITEMSTRUCT pMeasureItemStruct)
{
}

int COwnerDrawListBoxItem::CompareItem(COwnerDrawListBox* pList, LPCOMPAREITEMSTRUCT pCompareItemStruct)
{
	return 0;
}

void COwnerDrawListBoxItem::DeleteItem(COwnerDrawListBox* pList, LPDELETEITEMSTRUCT pDeleteItemStruct)
{
}

/////////////////////////////////////////////////////////////////////////////
// COwnerDrawListBox

COwnerDrawListBox::COwnerDrawListBox()
{
}

COwnerDrawListBox::~COwnerDrawListBox()
{
}

BEGIN_MESSAGE_MAP(COwnerDrawListBox, CListBox)
	//{{AFX_MSG_MAP(COwnerDrawListBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int COwnerDrawListBox::Index(const CPoint& cpPoint)
{
	CRect crClient;
	GetClientRect(crClient);
	
	int nCount = GetCount()-1;
	int nIndex = GetTopIndex();
	
	while (nIndex < nCount)
	{
		CRect crItem;
		
		if (GetItemRect(nIndex, crItem) != LB_ERR)
		{
			if (crItem.top >= crClient.bottom)
			{
				break;
			}
			
			if (cpPoint.y < crItem.top)
			{
				nIndex--;
				
				if (nIndex < 0)
				{
					nIndex = 0;
				}
				
				break;
			}
			
			if (cpPoint.y < crItem.bottom)
			{
				break;
			}
		}
		
		nIndex++;
	}
	
	return nIndex;
	
#if 0
	// This code does a binary search of the item rectangles.
	// It assumes that the rectangles of successive items
	// have increasing Y coordinates.
	
	int nMin = 0;
	int nMax = GetCount()-1;
	int nIndex = -1;
	
	while (nMin <= nMax)
	{
		int nCompare = -1;
		
		nIndex = (nMin+nMax)/2;
		
		CRect crItem;
		
		if (GetItemRect(nIndex, crItem) == LB_ERR)
		{
			nMax = nIndex-1;
		}
		
		else
		{
			if (cpPoint.y < crItem.top)
			{
				nMax = nIndex-1;
			}
			else if (cpPoint.y >= crItem.bottom)
			{
				nMin = nIndex+1;
			}
			else
			{
				break;
			}
		}
	}
	
	if (nIndex > nMax)
	{
		nIndex = nMax;
	}
	
	return nIndex;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// COwnerDrawListBox message handlers

void COwnerDrawListBox::DrawItem(LPDRAWITEMSTRUCT pDrawItemStruct)
{
	if ((COwnerDrawListBoxItem*)pDrawItemStruct->itemData != NULL)
	{
		((COwnerDrawListBoxItem*)pDrawItemStruct->itemData)->DrawItem(this, pDrawItemStruct);
	}
}

void COwnerDrawListBox::MeasureItem(LPMEASUREITEMSTRUCT pMeasureItemStruct)
{
	pMeasureItemStruct->itemWidth = 0;
	pMeasureItemStruct->itemHeight = 0;
	
	// Get a pointer to the item to query.
	
	COwnerDrawListBoxItem* pItem;
	
	if ((GetStyle() & LBS_OWNERDRAWVARIABLE) != 0)
	{
		pItem = (COwnerDrawListBoxItem*)(pMeasureItemStruct->itemData);
	}
	
	else
	{
		pItem = (COwnerDrawListBoxItem*)GetItemDataPtr(0);
	}
	
	// Ask the item for its dimensions.
		
	if (pItem != (COwnerDrawListBoxItem*)(-1))
	{
		pItem->MeasureItem(this, pMeasureItemStruct);
	}
}

int COwnerDrawListBox::CompareItem(LPCOMPAREITEMSTRUCT pCompareItemStruct)
{
	return ((COwnerDrawListBoxItem*)pCompareItemStruct->itemData1)->CompareItem(this, pCompareItemStruct);
}

void COwnerDrawListBox::DeleteItem(LPDELETEITEMSTRUCT pDeleteItemStruct)
{
	if (pDeleteItemStruct->itemData != 0)
	{
		((COwnerDrawListBoxItem*)pDeleteItemStruct->itemData)->DeleteItem(this, pDeleteItemStruct);
	}
}
