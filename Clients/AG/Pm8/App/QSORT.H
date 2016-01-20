/*
// $Workfile: QSORT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/QSORT.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:40   Fred
// Initial revision.
// 
//    Rev 1.1   10 Jun 1997 13:35:50   Fred
// new setup for multiple inhieritance
// 
//    Rev 1.0   19 Aug 1996 08:09:14   Fred
// Initial revision.
*/

class CQuickSortBase;

#ifndef __QSORT_H__
#define __QSORT_H__

class CQuickSortBase
{
public:

	// Construction.
	CQuickSortBase(void);
	virtual ~CQuickSortBase(void);

public:

	// Operations.

	// Sort the elements.
	void QuickSort(void);

	// Find data item or the place to insert it.
	BOOL QuickFind(void* pData, DWORD& dwIndex);

protected:

	// Recursive helper which does all the work.
	void DoQuickSort(DWORD dwPivot, DWORD dwElements);

	// return number of elements (must be static during sort)
	virtual DWORD Size(void) = 0;

	// Compare two elements.
	virtual int Compare(DWORD dwLeft, DWORD dwRight) = 0;

	// Compare data item to an element.
	virtual int Compare(void* pData, DWORD dwIndex) = 0;

	// Exchange two elements.
	virtual void Exchange(DWORD dwLeft, DWORD dwRight) = 0;
};

#endif
