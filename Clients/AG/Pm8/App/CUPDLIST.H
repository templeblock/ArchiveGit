/*
// $Workfile: CUPDLIST.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright (C) 1994 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CUPDLIST.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:26:48   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:44   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:39:22   JAY
// Reverted. New series.
// 
//    Rev 1.0   03 Nov 1994 12:56:22   FRED
//  
// 
//    Rev 1.4   27 Oct 1994 14:51:04   FRED
// Added CUpdateListItem.Bounds(CSize).
// 
// 
//    Rev 1.3   11 Oct 1994 12:35:02   JAY
// Added SetEmpty().
// 
//    Rev 1.2   11 Oct 1994 10:29:46   JAY
//  
// 
//    Rev 1.1   10 Oct 1994 17:43:12   JAY
//  
// 
//    Rev 1.0   10 Oct 1994 15:44:02   FRED
// Initial revision.
*/

class CUpdateListItem;
class CUpdateList;

#ifndef _CUPDLIST_H_
#define _CUPDLIST_H_


// CUpdateListItem
//
//		This class represents a single item in an update list.

class CUpdateListItem
{

public:

	CUpdateListItem();
	CUpdateListItem(CUpdateListItem& Item);
	virtual ~CUpdateListItem();
	
	const CRect& Bounds(void) const
		{ return m_crBounds; };
	void Bounds(const CRect& crBounds)
		{ m_crBounds = crBounds; }
	void Bounds(const CSize& crSize)
		{ m_crBounds.left = 0; m_crBounds.top = 0; m_crBounds.right = crSize.cx; m_crBounds.bottom = crSize.cy; }
		
	BOOL IsEmpty(void) const;
	void SetEmpty(void);
	BOOL Overlaps(const CUpdateListItem& Item) const;
	BOOL Contains(const CUpdateListItem& Item) const;
	void Union(const CUpdateListItem& Item);
	BOOL Region(CRgn& Rgn) const;
	void Offset(const CPoint& cpOffset);
		
protected:
	
	CRect m_crBounds;
};


// CUpdateList
//
//		This class represents a collection of CUpdateListItem objects.

class CUpdateList : public CPtrArray
{

public:

	CUpdateList();
	virtual ~CUpdateList();

	CUpdateListItem* GetAt(int nIndex) const
		{ return (CUpdateListItem*)CPtrArray::GetAt(nIndex); }
	CUpdateListItem*& ElementAt(int nIndex)
		{ return (CUpdateListItem*&)CPtrArray::ElementAt(nIndex); }
	void RemoveAll(void)
		{ DeleteItem(); CPtrArray::RemoveAll(); }
	void RemoveAt(int nIndex, int nCount = 1)
		{ DeleteItem(nIndex, nCount); CPtrArray::RemoveAt(nIndex, nCount); }
	CUpdateListItem* operator[](int nIndex) const
		{ return GetAt(nIndex); }
	CUpdateListItem*& operator[](int nIndex)
		{ return ElementAt(nIndex); }
	CUpdateList& operator=(const CUpdateList& List);

	void Offset(const CPoint& cpOffset);
	BOOL IsEmpty(void) const
		{ return GetSize() == 0; }

	void DeleteItem(int nIndex = -1, int nCount = 1);		
	void Union(const CUpdateList& List);
	void Union(const CUpdateListItem& Item);
	BOOL Overlaps(const CUpdateListItem& Item) const;
	BOOL Contains(const CUpdateListItem& Item) const;
	void Bounds(CRect& crBounds) const;
	BOOL Region(CRgn& Rgn) const;
};

#endif		// #ifndef _CUPDLIST_H_
