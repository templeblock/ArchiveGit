/*
// $Workfile: QSORT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/QSORT.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:14   Fred
// Initial revision.
// 
//    Rev 1.1   10 Jun 1997 13:35:50   Fred
// new setup for multiple inhieritance
// 
//    Rev 1.0   19 Aug 1996 08:09:18   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "qsort.h"

// The Quick Sort algorithm was first described by C.A.R.Hoare in the Computer
// Journal, No. 5 (1962), pp.10..15, and in addition is frequently described
// in computing literature, notably in D. Knuth's Sorting and Searching.
// 
// The median-of-three technique described by Singleton (Communications
// of the A.C.M., No 12 (1969) pp 185..187) is used, where the median
// operation is also the special case sort for 3 elements.  This slightly
// improves the average speed, especially when comparisons are slower
// than exchanges, but more importantly it prevents worst-case behavior
// on partly sorted files.  If a simplistic quick-sort is run on a file
// which is only slightly disordered (a common need in some applications)
// then it is as slow as a bubble-sort. The median technique prevents this.
//
// Another serious problem with the plain algorithm is that worst-case
// behavior causes very deep recursion (almost one level per table
// element!), so again it is best to use the median technique.
// 
// Recursion is used both for simplicity and because there is no practical
// gain from conversion to loops: the extra housekeeping needed for loops
// needs registers for speed, but the 8086 family has not enough registers.
// Juggling registers takes just as much time as calling subroutines.

CQuickSortBase::CQuickSortBase(void)
{
}

CQuickSortBase::~CQuickSortBase(void)
{
}

void CQuickSortBase::DoQuickSort(DWORD dwPivot, DWORD dwElements)
{
	DWORD dwLeft, dwRight, dwPivotEnd, dwPivotTemp, dwLeftTemp, dwPivotElements;

tail_recursion:

	if (dwElements <= 2)
	{
		if (dwElements == 2)
		{
			dwRight = dwPivot+1;
			if (Compare(dwPivot, dwRight) > 0)
			{
				Exchange(dwPivot, dwRight);
			}
		}

		return;
	}

	dwRight = dwPivot+(dwElements-1);
	dwLeft = dwPivot+(dwElements >> 1);

	// Sort the pivot, left, and right elements for "median of 3".

	if (Compare(dwLeft, dwRight) > 0)
	{
		Exchange(dwLeft, dwRight);
	}

	if (Compare(dwLeft, dwPivot) > 0)
	{
		Exchange(dwLeft, dwPivot);
	}
	else
	{
		if (Compare(dwPivot, dwRight) > 0)
		{
			Exchange(dwPivot, dwRight);
		}
	}

	if (dwElements == 3)
	{
		Exchange(dwPivot, dwLeft);
		return;
	}

	// Now for the classic Hoare algorithm.

	dwLeft = dwPivotEnd = dwPivot+1;

	do
	{
		int nCompare;

		while ((nCompare = Compare(dwLeft, dwPivot)) <= 0)
		{
			if (nCompare == 0)
			{
				Exchange(dwLeft, dwPivotEnd);
				dwPivotEnd++;
			}

			if (dwLeft < dwRight)
			{
				dwLeft++;
			}

			else
			{
				goto quick_break;
			}
		}

		while (dwLeft < dwRight)
		{
			if ((nCompare = Compare(dwPivot, dwRight)) < 0)
			{
				dwRight--;
			}

			else
			{
				Exchange(dwLeft, dwRight);

				if (nCompare != 0)
				{
					dwLeft++;
					dwRight--;
				}

				break;
			}
		}
	} while (dwLeft < dwRight);

quick_break:

	if (Compare(dwLeft, dwPivot) <= 0)
	{
		dwLeft++;
	}

	dwLeftTemp = dwLeft-1;
	dwPivotTemp = dwPivot;

	while ((dwPivotTemp < dwPivotEnd) && (dwLeftTemp >= dwPivotEnd))
	{
		Exchange(dwPivotTemp, dwLeftTemp);
		dwPivotTemp++;
		dwLeftTemp--;
	}

	dwPivotElements = dwLeft-dwPivotEnd;
	dwElements = dwPivot+dwElements-dwLeft;

	// Sort smaller partition first to reduce stack usage.

	if (dwElements < dwPivotElements)
	{
		DoQuickSort(dwLeft, dwElements);
		dwElements = dwPivotElements;
	}
	else
	{
		DoQuickSort(dwPivot, dwPivotElements);
		dwPivot = dwLeft;
	}

	goto tail_recursion;
}

void CQuickSortBase::QuickSort(void)
{
	DoQuickSort(0, Size());
}

BOOL CQuickSortBase::QuickFind(void* pData, DWORD& dwIndex)
{
	// Remember if it was an exact match.
	BOOL fDuplicate = FALSE;

	// Perform a binary search for the element.
	DWORD dwMin = 0;
	DWORD dwMax = Size();

	// Remember first known index which is greater than new data (end of array).
	dwIndex = dwMax;

	while (dwMin < dwMax)
	{
		// Split the search space.
		DWORD dwTry = (dwMin+dwMax)/2;

		// Compare the element we are searching for to the current element.
		int nResult = Compare(pData, dwTry);

		// Check for exact match.
		if (nResult == 0)
		{
			// Elements are equal. Insert here.
			dwIndex = dwTry;
			fDuplicate = TRUE;
			break;
		}

		// Check if new data is greater than current data.
		if (nResult > 0)
		{
			// We know everything from dwTry back is lesser.
			dwMin = dwTry+1;
		}

		// Check if new data is less than current data.
		if (nResult < 0)
		{
			// We know everything from dwTry forward is greater.
			dwMax = dwTry;

			// Record index of element we were less than.
			dwIndex = dwMax;
		}
	}

	return fDuplicate;
}
