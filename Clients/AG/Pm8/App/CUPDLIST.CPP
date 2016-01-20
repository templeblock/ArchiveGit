/*
// $Workfile: CUPDLIST.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright (C) 1994 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CUPDLIST.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:36:12   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:27:50   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:36:02   JAY
// Reverted. New series.
// 
//    Rev 1.0   03 Nov 1994 12:53:46   FRED
//  
// 
//    Rev 1.3   24 Oct 1994 09:18:44   FRED
// Fixed memory leak.
// 
// 
//    Rev 1.2   11 Oct 1994 10:29:46   JAY
//  
// 
//    Rev 1.1   10 Oct 1994 17:43:14   JAY
//  
// 
//    Rev 1.0   10 Oct 1994 15:44:04   FRED
// Initial revision.
*/

#include "stdafx.h"

#include "cupdlist.h"


// CUpdateListItem
//
//		This class represents a single item in an update list.

CUpdateListItem::CUpdateListItem()
{
	SetEmpty();
}

CUpdateListItem::CUpdateListItem(CUpdateListItem& Item)
{
	*this = Item;
}

CUpdateListItem::~CUpdateListItem()
{
}

BOOL CUpdateListItem::IsEmpty(void) const
{
	return Bounds().IsRectEmpty();
}

void CUpdateListItem::SetEmpty(void)
{
	Bounds(CRect(0, 0, 0, 0));
}

BOOL CUpdateListItem::Overlaps(const CUpdateListItem& Item) const
{
	return
		(Bounds().left < Item.Bounds().right)
	&& (Bounds().right > Item.Bounds().left)
	&&	(Bounds().top < Item.Bounds().bottom)
	&& (Bounds().bottom > Item.Bounds().top);
}

BOOL CUpdateListItem::Contains(const CUpdateListItem& Item) const
{
	return
		(Bounds().left <= Item.Bounds().left)
	&& (Bounds().right >= Item.Bounds().right)
	&&	(Bounds().top <= Item.Bounds().top)
	&& (Bounds().bottom >= Item.Bounds().bottom);
}

void CUpdateListItem::Union(const CUpdateListItem& Item)
{
	Bounds(Bounds() | Item.Bounds());
}

BOOL CUpdateListItem::Region(CRgn& Rgn) const
{
	return Rgn.CreateRectRgnIndirect(Bounds());
}

void CUpdateListItem::Offset(const CPoint& cpOffset)
{
	m_crBounds += cpOffset;
}

// CUpdateList
//
//		This class represents a collection of CUpdateListItem objects.

CUpdateList::CUpdateList()
{
}

CUpdateList::~CUpdateList()
{
	DeleteItem();
}

CUpdateList& CUpdateList::operator=(const CUpdateList& List)
{
	// Construct a copy of the current update list.
	
	CUpdateListItem* pItem;
	
	TRY
	{
		int nMaximumIndex = List.GetUpperBound();
		
		for (int i = 0; i <= nMaximumIndex; i++)
		{
			pItem = new CUpdateListItem(*List[i]);
			Add(pItem);
			pItem = NULL;
		}
	}
	CATCH_ALL(e)
	{
		if (pItem != NULL)
		{
			delete pItem;
			pItem = NULL;
		}
		
		DeleteItem();
		
		THROW_LAST();
	}
	END_CATCH_ALL
	
	return *this;
}

void CUpdateList::Offset(const CPoint& cpOffset)
{
	int nSize = GetSize();

	for (int i = 0; i < nSize; i++)
	{
		GetAt(i)->Offset(cpOffset);
	}
}

void CUpdateList::DeleteItem(int nIndex /*=-1*/, int nCount /*=1*/)
{
	// Delete one or more of the items. An index of -1 means all
	// items will be deleted.
	
	if (nIndex == -1)
	{
		nIndex = 0;
		nCount = GetSize();
	}
	
	for (int i = 0; i < nCount; i++)
	{
		delete GetAt(nIndex+i);
		SetAt(nIndex+i, NULL);
	}
}

void CUpdateList::Union(const CUpdateList& List)
{
	// Union each item, one at a time.
	
	int nMaximumIndex = List.GetUpperBound();
	
	for (int i = 0; i <= nMaximumIndex; i++)
	{
		Union(*List[i]);
	}
}

void CUpdateList::Union(const CUpdateListItem& Item)
{
	if (!Item.IsEmpty())
	{
		int nMaximumIndex = GetUpperBound();
		
		for (int i = 0; i <= nMaximumIndex; i++)
		{
			if (GetAt(i)->Contains(Item))
			{
				return;
			}
			
			if (Item.Contains(*GetAt(i)))
			{
				RemoveAt(i);
				nMaximumIndex--;
				i--;
			}
		}

		CUpdateListItem* pItem = new CUpdateListItem;
		*pItem = Item;

		if (pItem != NULL)
		{
			Add(pItem);
		}
	}
}

BOOL CUpdateList::Overlaps(const CUpdateListItem& Item) const
{
	// Check if the given item overlaps any item in this update list.
	
	int nMaximumIndex = GetUpperBound();
	
	for (int i = 0; i <= nMaximumIndex; i++)
	{
		if (GetAt(i)->Overlaps(Item))
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL CUpdateList::Contains(const CUpdateListItem& Item) const
{
	// Check if the given item is fully contained by any item in this update list.
	
	int nMaximumIndex = GetUpperBound();
	
	for (int i = 0; i <= nMaximumIndex; i++)
	{
		if (GetAt(i)->Contains(Item))
		{
			return TRUE;
		}
	}
	
	return FALSE;
}

void CUpdateList::Bounds(CRect& crBounds) const
{
	// Compute the bounding rectangle for all the items this list.
	// Note: crBounds is not initialized, the bounding rectangles
	// are ORed with the current value.
	
	int nMaximumIndex = GetUpperBound();
	
	for (int i = 0; i <= nMaximumIndex; i++)
	{
		crBounds |= GetAt(i)->Bounds();
	}
}

BOOL CUpdateList::Region(CRgn& Rgn) const
{
	BOOL fSuccess = FALSE;
	
	CRgn rgnItem;
	
	TRY
	{
		// Loop through all of our items and add them to the 
						
		int nMaximumIndex = GetUpperBound();
		
		if (nMaximumIndex < 0)
		{
			return FALSE;
		}
		
		if (!GetAt(0)->Region(Rgn))
		{
			return FALSE;
		}
						
		for (int i = 1; i <= nMaximumIndex; i++)
		{
			// Get the region for the current item.
			
			if (!GetAt(i)->Region(rgnItem))
			{
				break;
			}
			
			// Combine this region with the others.
			
			if (Rgn.CombineRgn(&Rgn, &rgnItem, RGN_OR) == ERROR)
			{
				break;
			}
			
			rgnItem.DeleteObject();
		}
		
		fSuccess = (i == nMaximumIndex+1);
	}
	CATCH_ALL(e)
	{
	}
	END_CATCH_ALL
	
	if (!fSuccess)
	{
		Rgn.DeleteObject();
	}
		
	return fSuccess;
}
