//****************************************************************************
//
// File Name:  WinGridCtrl.inl
//
// Project:    Renaissance
//
// Author:     Lance Wilson
//
// Description: Definition class for a multi-column listbox.
//
// Portability: Windows Specific
//
// Developed by: Broderbund Software
//	              500 Redwood Blvd.
//               Novato, CA 94948
//	              (415) 382-4400
//
//  Copyright (C) 1996 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: -------------------------------------------------------------- $
//   $Author:: -------------------------------------------------------------- $
//     $Date:: -------------------------------------------------------------- $
// $Revision:: -------------------------------------------------------------- $
//
//****************************************************************************


//*****************************************************************************
//
// Function Name:  RGridCtrl<T>::RGridCtrl
//
// Description:    Constructor (Default).
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
template <class T> 
RGridCtrl<T>::RGridCtrl( UINT uiStyles ) :
	RGridCtrlBase( uiStyles ) 
{
}

//*****************************************************************************
//
// Function Name:  RGridCtrl<T>::RGridCtrl
//
// Description:    Destructor.
//
// Returns:		   Nothing
//
// Exceptions:	   None
//
//*****************************************************************************
template <class T> 
RGridCtrl<T>::~RGridCtrl()
{
}

//*****************************************************************************
//
// Function Name:  RGridCtrl<T>::AddItem(const T& item )
//
// Description:    Call this member function to add am item to a 
//                 grid control. If the grid control was not created 
//                 with the LBS_SORT style, the string is added to 
//                 the end of the list. Otherwise, the string is 
//                 inserted into the list, and the list is sorted. 
//
//                 If the list box was created with the LBS_SORT 
//                 style but contains no data, the framework sorts 
//                 the list by one or more calls to the CompareItem 
//                 member function.  
//
//                 Use InsertItem to insert a string into a specific 
//                 location within the grid contorl.
//
// Returns:		   The zero-based index to the item in the grid
//                 control. The return value is LB_ERR if an error 
//                 occurs; the return value is LB_ERRSPACE if 
//                 insufficient space is available to store the new 
//                 item.
//
// Exceptions:	   None
//
//*****************************************************************************
template <class T> 
int RGridCtrl<T>::AddItem(const T& item )
{
	//
	// Determine where to insert the item at. If the LBS_SORT
	// flag is not set, add the item to the end of the list,
	// otherwise, find where it belongs, and insert it there.
	//
	int cxItemPos = GetCount() ;

	if (GetStyle() & LBS_SORT)
	{
		BOOL fFound ;

		cxItemPos = FindSortedItem( -1, item, fFound ) ;
		
		while (fFound)
		{
			FindSortedItem( cxItemPos++, item, fFound ) ;
		}
	}

	//
	// See if there is preallocated space to use.  If so,
	// assign the element to the correct position; otherwise
	// insert it into the array at the position cxItemPos
	//
	if (cxItemPos > m_itemArray.GetUpperBound() && 
		cxItemPos < m_itemArray.GetSize())
	{

		// Use the preallocated space.
		m_itemArray[cxItemPos] = item ;
	}
	else
	{
		try 
		{
			// Try adding the item to the array.
			m_itemArray.InsertAt( cxItemPos, const_cast<T&>(item) ) ;
		}
		catch (CMemoryException e) 
		{
			return LB_ERRSPACE ;
		}
	}

	OnItemAdded( cxItemPos ) ;

	return cxItemPos ;
}

//*****************************************************************************
//
// Function Name: RGridCtrl<T>::DeleteItem( UINT nIndex )
//
// Description:   Deletes an item from a grid control.
//
// Returns:       A count of the items remaining in the list. The 
//                return value is LB_ERR if nIndex specifies an index 
//                greater than the number of items in the list.
//
// Exceptions:	   None
//
//*****************************************************************************
template <class T> 
int RGridCtrl<T>::DeleteItem( UINT nIndex )
{
	int cxItemCount = GetCount() ;
	
	if ((int) nIndex >= cxItemCount)
		return LB_ERR ;

	// Remove the item from the list
	m_itemArray.RemoveAt( nIndex ) ;

	// Update the current selection, if necessary
	int nCurSel = GetCurSel() ;

	if (nCurSel > (int) nIndex)
	{
		SetCurSel( min( nCurSel - 1, cxItemCount - 1 ) ) ;
	}

	// Update the top index if necessary.
	if ((int) nIndex < GetTopIndex())
	{
		SetTopIndex( GetTopIndex() - 1 ) ;
	}
	else if ((int) nIndex <= LastVisibleItem())
	{
		CRect rect, rectItem ;

		GetGridRect( rect ) ;
		GetItemRect( nIndex, rectItem ) ;

		rect.top = rectItem.top ;
		InvalidateRect( rect ) ;
	}

	UpdateScrollBars() ;

	return (cxItemCount - 1) ;
}

//*****************************************************************************
//
// Function Name:  RGridCtrl<T>::GetCount()
//
// Description:    Retrieves the number of items in a list box. The
//                 returned count is one greater than the index 
//                 value of the last item (the index is zero-based).
//
// Returns:	       The number of items in the grid control, or 
//                 LB_ERR if an error occurs.
//
// Exceptions:	   None
//
//*****************************************************************************
template <class T> 
int RGridCtrl<T>::GetCount() const
{
	// Note, the size of the array may vary
	// from the upper bound.  So, return the
	// upper bound of the array plus one.

	return (m_itemArray.GetUpperBound() + 1) ;
}

//*****************************************************************************
//
// Function Name:  RGridCtrl<T>::GetItemData( int nIndex, T& item )
//
// Description:    Retrieves item data from the grid control
//
// Returns:		   TRUE if successful; otherwise FALSE.
//
// Exceptions:	   None
//
//*****************************************************************************
template <class T> 
BOOL RGridCtrl<T>::GetItemData( int nIndex, T& item ) const
{
	if (nIndex >= GetCount())
	{
		return FALSE ;
	}

	item = m_itemArray[nIndex] ;

	return TRUE ;
}

//*****************************************************************************
//
// Function Name:  RGridCtrl<T>::FindItem( int nStartAfter, const T& item )
//
// Description:    Finds the first item in a grid control that contains 
//                 the specified data without changing the control's 
//                 selection. Use the SelectItem member function to both 
//                 find and select an item.
//
// Returns:		   The zero-based index of the matching item, or LB_ERR if the search was unsuccessful.
//
// Exceptions:	   None
//
//*****************************************************************************
template <class T> 
int RGridCtrl<T>::FindItem( int nStartAfter, const T& item )
{
	int nIndex ;

	if (GetStyle() & LBS_SORT)
	{
		BOOL fFound ;
		nIndex = FindSortedItem( nStartAfter, item, fFound ) ;

		if (!fFound)
		{
			return LB_ERR ;
		}

		return nIndex ;
	}

	int nItemCount = GetCount() ;

	for (nIndex = nStartAfter + 1; nIndex < nItemCount; nIndex++)
	{
		T itemData ;
		GetItemData( nIndex, itemData ) ;

		if (item == itemData)
		{
			return nIndex ;
		}
	}

	return LB_ERR ;
}

//*****************************************************************************
//
// Function Name:  RGridCtrl<T>::FindSortedItem( int nStartAfter, const T& item, BOOL& fFound )
//
// Description:    Helper function to finds the first item in a sorted 
//                 grid control that is the closest match to the specified 
//                 data.  If an exact match is found, the fFound arguement 
//                 is set to TRUE ;
//
// Returns:		   The zero-based index of the closest matching item.
//
// Exceptions:	   None
//
//*****************************************************************************
template <class T> 
int RGridCtrl<T>::FindSortedItem( int nStartAfter, const T& item, BOOL& fFound )
{
	int nLastIndex  = GetCount() - 1 ;
	int nStartIndex = nStartAfter + 1 ;
	int nCurIndex ;

	T itemTemp ;

	fFound = FALSE ;

	while (nStartIndex <= nLastIndex) 
	{

		// Determine the next current index.
		nCurIndex = (nStartIndex + nLastIndex) / 2 ;

		GetItemData (nCurIndex, itemTemp) ;
//		nResult = CompareItems( item, itemTemp ) ;

		if (item > itemTemp)
//		if (nResult > 0)
		{
			nStartIndex = nCurIndex + 1 ;
		}
		else
		{
			//
			// By also updating the last index when a match is 
			// found, we will be guaranteed that the item returned
			// is the first matching entry in the list.
			//

			nLastIndex = nCurIndex - 1 ;
		
			if (item == itemTemp)
//			if (nResult == 0)
			{
				fFound = TRUE ;
			}
		}
	}

	return nStartIndex ;
}

//*****************************************************************************
//
// Function Name:  RGridCtrl<T>::InitStorage( int nItems )
//
// Description:    Allocates memory for storing grid control items. Call 
//                 this function before adding a large number of items to 
//                 a RGridCtrl.
//
// Returns:	       If successful, the maximum number of items that the 
//                 list box can store before a memory reallocation is 
//                 needed, otherwise LB_ERRSPACE, meaning not enough 
//                 memory is available.
//
// Exceptions:	   None
//
//*****************************************************************************
template <class T> 
int	RGridCtrl<T>::InitStorage( int nItems )
{
	ASSERT (m_itemArray.GetSize() == 0) ;

	try
	{
		m_itemArray.SetSize( nItems ) ;
		return nItems ;
	}
	catch (CMemoryException e)
	{
		return LB_ERRSPACE ;
	}
}

//*****************************************************************************
//
// Function Name:  RGridCtrl<T>::InsertItem( int nIndex, const T& item)
//
// Description:    Inserts an item into the grid control. Unlike the 
//                 AddItem member function, InsertItem does not cause 
//                 a list with the LBS_SORT style to be sorted.
//
// Returns:	       The zero-based index of the position at which the item
//                 was inserted. The return value is LB_ERR if an error 
//                 occurs; the return value is LB_ERRSPACE if insufficient 
//                 space is available to store the new item.
//
// Exceptions:	   None
//
//*****************************************************************************
template <class T> 
int RGridCtrl<T>::InsertItem( int nIndex, const T& item)
{
	if (nIndex >= GetCount())
	{
		return LB_ERR ;
	}

	try 
	{
		if (nIndex == -1)
		{
			nIndex = GetCount() ;
		}

		// Try adding the item to the array.
		m_itemArray.InsertAt( nIndex, const_cast<T&>(item) ) ;

		// REVIEW: if sorting is to be added, then
		// this will need to be modified.
	}
	catch (CMemoryException e) 
	{
		return LB_ERRSPACE ;
	}

	OnItemAdded( nIndex ) ;

	return nIndex ;
}

//*****************************************************************************
//
// Function Name:  RGridCtrl<T>::ResetContent( )
//
// Description:    Removes all items from a grid control.
//
// Returns:	       None
//
// Exceptions:	   None
//
//*****************************************************************************
template <class T> 
void RGridCtrl<T>::ResetContent( )
{
	m_itemArray.RemoveAll() ;
}

//*****************************************************************************
//
// Function Name:  RGridCtrl<T>::SelectItem( int nStartAfter, T& item )
//
// Description:    Searches for an item that matches the specified item, 
//                 and if a matching item is found, it selects the item. 
//
//                 The control is scrolled, if necessary, to bring the 
//                 selected item into view. 
//
//                 This member function cannot be used with a grid control
//                 that has the LBS_MULTIPLESEL style.  
//
// Returns:	       The index of the selected item if the search was 
//                 successful. If the search was unsuccessful, the return 
//                 value is LB_ERR and the current selection is not changed.
//
// Exceptions:	   None
//
//*****************************************************************************
template <class T> 
int RGridCtrl<T>::SelectItem( int nStartAfter, T& item )
{
	int nIndex = FindItem( nStartAfter, item ) ;

	if (LB_ERR != nIndex)
	{
		SetCurSel( nIndex ) ;
	}

	return nIndex ;
}

//*****************************************************************************
//
// Function Name:  RGridCtrl<T>::SetItemData( int nIndex, T& item)
//
// Description:    Replaces the item at the specified index, with the
//                 item arguement.
//
// Returns:	       LB_ERR if an error occurs.
//
// Exceptions:	   None
//
//*****************************************************************************
template <class T> 
int RGridCtrl<T>::SetItemData( int nIndex, T& item )
{
	if (nIndex > m_itemArray.GetUpperBound())
	{
		return LB_ERR ;
	}

	m_itemArray[nIndex] = item ;

	return 0 ;
}

//*****************************************************************************
//
// Function Name:  RGridCtrl<T>::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
//
// Description:    Called by the framework when a visual aspect of the 
//                 control changes. The itemAction and itemState members 
//                 of the DRAWITEMSTRUCT structure define the drawing 
//                 action that is to be performed. 
//
// Returns:	       None
//
// Exceptions:	    None
//
//*****************************************************************************
template <class T>
void RItemDrawnGridCtrl<T>::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
	T item ;
	GetItemData( lpDrawItemStruct->itemID, item ) ;

	item.DrawItem( lpDrawItemStruct ) ;
}

//*****************************************************************************
//
// Function Name:  RItemDrawnGridCtrl<T>::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
//
// Description:    Determines the metrics of the items the control is to
//                 display. 
//
// Returns:	       None
//
// Exceptions:	    None
//
//*****************************************************************************
template <class T>
void RItemDrawnGridCtrl<T>::MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
	T item ;
	GetItemData( lpMeasureItemStruct->itemID, item ) ;

	CSize size ;
	item.MeasureItem( size ) ;

	lpMeasureItemStruct->itemWidth  = size.cx ;
	lpMeasureItemStruct->itemHeight = size.cy ;
}
