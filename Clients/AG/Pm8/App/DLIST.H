//
// DLIST.H
//
//		Copyright (C) 1993 MicroLogic Software, Inc.
//
// Interface for doubly-linked list class for Printmaster Gold.
//
// $Header: /PM8/App/DLIST.H 1     3/03/99 6:04p Gbeddow $
// $Log: /PM8/App/DLIST.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:36   Fred
// Initial revision.
// 
//    Rev 1.4   28 Jan 1997 16:56:46   Fred
// Start of component drawing
// 
//    Rev 1.3   24 Jan 1997 13:26:00   Fred
// Added last pointer and other improvements
// 
//    Rev 1.2   15 Jan 1997 16:43:04   dennisp
// Added GetCount()
// 
//    Rev 1.1   06 Jan 1997 15:45:44   Jay
// Some optimizations
// 
//    Rev 1.0   14 Mar 1996 13:47:22   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:18   FRED
// Initial revision.
// 
//    Rev 2.1   19 Jan 1996 09:21:34   JAY
// More fixes for based() code.
// 
//    Rev 2.0   07 Feb 1995 15:48:28   JAY
// Initial revision.
// 
//    Rev 1.3   04 Aug 1994 07:32:36   JAY
// Large model and 32-bit changes
// 
//    Rev 1.2   23 Feb 1994 14:16:34   MACDUFF
// DlistItem constructor initializes prev/next pointers to zero.
// 
//    Rev 1.1   22 Feb 1994 15:55:02   MACDUFF
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
//    Rev 1.0   16 Dec 1993 16:26:18   JAY
//  
// 
//    Rev 1.2   26 Feb 1993 14:30:00   MACDUFF
// Full implementation of font caching
// 
//    Rev 1.1   25 Feb 1993 11:26:10   MACDUFF
// Defined a Valid() method, #ifdef'd out.
// 
//    Rev 1.0   23 Feb 1993 15:33:12   MACDUFF
// Doubly-linked list class
//

#ifndef __DLIST_H__
#define __DLIST_H__

/////////////////////////////////////////////////////////////////
// tDoubleLinkedListItem

typedef class tDoubleLinkedListItem far *pDoubleLinkedListItem;

class tDoubleLinkedListItem
{
private:
	void SetNext(pDoubleLinkedListItem p);
	void SetPrevious(pDoubleLinkedListItem p);

protected:
	tDoubleLinkedListItem()
		{ next = NULL; previous = NULL; } 

	virtual ~tDoubleLinkedListItem();

	pDoubleLinkedListItem next;
	pDoubleLinkedListItem previous;

	friend class tDoubleLinkedList;

public:
	pDoubleLinkedListItem GetNext() const;
	pDoubleLinkedListItem GetPrevious() const;
};

/////////////////////////////////////////////////////////////////
// tDoubleLinkedList

typedef int (*tTestFunc)(const void far *item);
typedef int (*tTestFunc2)(const void far *item, const void far *data);

class tDoubleLinkedList
{
public:
	tDoubleLinkedList(void)
		{ first = NULL; last = NULL; count = NULL;}
	virtual ~tDoubleLinkedList();

public:
	BOOL IsEmpty(void)
		{ return (count == 0); }
	pDoubleLinkedListItem FirstThat(tTestFunc) const;
	pDoubleLinkedListItem FirstThat(tTestFunc2, VOIDPTR data) const;
	virtual void Unlink(pDoubleLinkedListItem);
	virtual void InsertAtFront(pDoubleLinkedListItem);
	virtual void InsertAtEnd(pDoubleLinkedListItem);
	void Remove(pDoubleLinkedListItem aItem);
	void RemoveFirstItem(void)
		{ Remove(first); }
	void RemoveLastItem(void)
		{ Remove(last); }
	void MoveToFront(pDoubleLinkedListItem aItem)
		{ if (aItem != first) { Unlink(aItem); InsertAtFront(aItem); } }
	void MoveToEnd(pDoubleLinkedListItem aItem)
		{ if (aItem != last) { Unlink(aItem); InsertAtEnd(aItem); } }
	int GetCount() const
		{ return count; }
	pDoubleLinkedListItem GetFirst(void) const
		{ return first; }
	pDoubleLinkedListItem GetLast(void) const
		{ return last; }
#ifdef DEBUG_DLIST
	int Valid(void) const;
#endif

protected:
	pDoubleLinkedListItem first;
	pDoubleLinkedListItem last;
	int count;
};

// Inline functions.

inline void tDoubleLinkedListItem::SetNext(pDoubleLinkedListItem p)
	{ next = p; }
inline void tDoubleLinkedListItem::SetPrevious(pDoubleLinkedListItem p)
	{ previous = p; }
inline pDoubleLinkedListItem tDoubleLinkedListItem::GetNext() const
	{ return next; }
inline pDoubleLinkedListItem tDoubleLinkedListItem::GetPrevious() const
	{ return previous; }
#endif									// #ifndef __DLIST_H__
