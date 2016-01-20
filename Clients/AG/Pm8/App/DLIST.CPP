/*
// DLIST.CPP
//
//		Copyright (C) 1993 MicroLogic Software, Inc.
//
// Implementation of doubly-linked list class for Printmaster Gold.
//
// $Header: /PM8/App/DLIST.CPP 1     3/03/99 6:04p Gbeddow $
// $Log: /PM8/App/DLIST.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:10   Fred
// Initial revision.
// 
//    Rev 1.2   24 Jan 1997 13:26:08   Fred
// Added last pointer and other improvements
// 
//    Rev 1.1   06 Jan 1997 15:45:42   Jay
// Some optimizations
// 
//    Rev 1.0   14 Mar 1996 13:46:30   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:38   FRED
// Initial revision.
// 
//    Rev 2.2   19 Jan 1996 09:21:36   JAY
// More fixes for based() code.
// 
//    Rev 2.1   20 Dec 1995 09:53:18   JAY
// 
//    Rev 2.0   07 Feb 1995 15:47:40   JAY
// Initial revision.
// 
//    Rev 1.6   30 Nov 1994 16:33:56   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.5   04 Aug 1994 07:32:54   JAY
// Large model and 32-bit changes
// 
//    Rev 1.4   23 Feb 1994 14:17:34   MACDUFF
// Fixed two ASM errors, where memory was compared to immediate
// without specifying operand size (should be word).
// 
//    Rev 1.3   22 Feb 1994 16:01:36   MACDUFF
// Eliminated warnings caused by previous change.
// 
//    Rev 1.2   22 Feb 1994 15:57:08   MACDUFF
// Sped up font cache as follows:
// 1) Put all items of a cache in the same segment.
// 2) Used based pointers for the 'next' and 'previous' members of
// the DoubleLinkedListItem.
// 3) Use assembly code to eliminate unneeded segment register loads.
// 4) Do not hash on character codes.  This enables us to eliminate
// virtual methods in the DoubleLinkedListItem.  It turns out that
// under Windows the overhead of the VMT is greater than the savings
// gained from hashing.
// 
//    Rev 1.1   30 Dec 1993 16:10:26   JAY
//  
// 
//    Rev 1.0   16 Dec 1993 16:25:30   JAY
//  
// 
//    Rev 1.3   26 Feb 1993 14:30:00   MACDUFF
// Full implementation of font caching
// 
//    Rev 1.2   25 Feb 1993 11:25:38   MACDUFF
// Added debugging code, #ifdef'd out.
// Fixed some errors.
// 
//    Rev 1.1   24 Feb 1993 14:13:04   MACDUFF
// Fixed errors in Unlink() and InsertAtFront().
// 
//    Rev 1.0   23 Feb 1993 15:33:14   MACDUFF
// Doubly-linked list class
*/

#include "stdafx.h"
#include "dlist.h"


/////////////////////////////////////////////////////////////////
// tDoubleLinkedListItem

tDoubleLinkedListItem::~tDoubleLinkedListItem()
{
}

/////////////////////////////////////////////////////////////////
// tDoubleLinkedList

tDoubleLinkedList::~tDoubleLinkedList()
{
	pDoubleLinkedListItem item;

	item = first;
	while (item != NULL)
	{
		tDoubleLinkedListItem *item2;
		item2 = item->GetNext();
		delete item;
		item = item2;
	}
}

pDoubleLinkedListItem tDoubleLinkedList::FirstThat(tTestFunc TestFunc) const
{
	pDoubleLinkedListItem item;

	item = first;
	while (item != NULL && !TestFunc((VOIDPTR)item))
	{
		item = item->GetNext();
	}
	return item;
}

pDoubleLinkedListItem tDoubleLinkedList::FirstThat(tTestFunc2 TestFunc, VOIDPTR data) const
{
	pDoubleLinkedListItem item;

	item = first;
	while (item != NULL && !TestFunc((VOIDPTR)item, data))
	{
		item = item->GetNext();
	}
	return item;
}

void tDoubleLinkedList::Unlink(pDoubleLinkedListItem aItem)
{
	pDoubleLinkedListItem prev, next;

	prev = aItem->GetPrevious();
	next = aItem->GetNext();

/* Handle the previous item. */

	if (prev == NULL)
	{
	/* First item! */
		ASSERT(first == aItem);
		first = next;
	}
	else
	{
		prev->SetNext(next);
	}

/* Handle the next item if we have one. */

	if (next == NULL)
	{
	/* Last item! */
		ASSERT(last == aItem);
		last = prev;
	}
	else
	{
		next->SetPrevious(prev);
	}

	--count;
}

void tDoubleLinkedList::InsertAtFront(pDoubleLinkedListItem aItem)
{
	aItem->previous = NULL;
	if (first == NULL)
	{
		ASSERT(last == NULL);
		last = aItem;
		aItem->next = NULL;
	}
	else
	{
		ASSERT(last != NULL);
		aItem->SetNext(first);
		first->SetPrevious(aItem);
	}
	first = aItem;
	++count;
}

void tDoubleLinkedList::InsertAtEnd(pDoubleLinkedListItem aItem)
{
	aItem->next = NULL;
	if (last == NULL)
	{
		ASSERT(first == NULL);
		first = aItem;
		aItem->previous = NULL;
	}
	else
	{
		ASSERT(first != NULL);
		aItem->SetPrevious(last);
		last->SetNext(aItem);
	}
	last = aItem;
	++count;
}

void tDoubleLinkedList::Remove(pDoubleLinkedListItem aItem)
{
	Unlink(aItem);
	delete aItem;
}

#ifdef DEBUG_DLIST
int tDoubleLinkedList::Valid(void) const
{
	if (count < 0)
	{
		return FALSE;
	}
	if (first == NULL)
	{
		return (count == 0);
	}
	else
	{
		pDoubleLinkedListItem item;
		int n;

		if (first->GetPrevious() != NULL)
		{
			return FALSE;
		}

		item = first;
		n = 0;

		while (item != NULL)
		{
			if (++n > count)
			{
				return FALSE;
			}

			item = item->GetNext();
			if (item != 0)
			{
				if (item->GetPrevious() == NULL)
				{
					return FALSE;
				}
				if (item->GetPrevious()->GetNext() != item)
				{
					return FALSE;
				}
			}
		}
		if (n != count)
		{
			return FALSE;
		}
	}
	return TRUE;
}
#endif
