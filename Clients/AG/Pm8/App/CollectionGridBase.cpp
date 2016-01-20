//****************************************************************************
//
// File Name:  CollectionGridBase.cpp
//
// Project:    Renaissance
//
// Author:     Lance Wilson
//
// Description: Implementation of class for keyboard supported collection 
//              grid controls
//
// Portability: Windows Specific
//
// Developed by:   Broderbund Software
//	                500 Redwood Blvd.
//                 Novato, CA 94948
//	                (415) 382-4400
//
//  Copyright (C) 1997 Broderbund Software. All Rights Reserved.
//
//  $Logfile:: /PM8/App/CollectionGridBase.cpp                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:04p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef	WIN
	#error	This file must be compiled only on Windows
#endif	//	WIN

#include "RenaissanceIncludes.h"
ASSERTNAME

#include "CollectionGridBase.h"

const UINT kTimerID     = 1 ;
const UINT kTimerElapse = 1000 ;

//*****************************************************************************
//
// Function Name: RBackdropGridCntrl::RBackdropGridCntrl() 
//
// Description:   Default constructor
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
RCollectionGridBase::RCollectionGridBase( UINT uiStyles ) :
	RGridCtrlBase( uiStyles ) 
{
	m_pResultArray = NULL ;
	m_strBuffer    = _T("") ;
	m_uiTimer      = 0 ;
}

//*****************************************************************************
//
// Function Name: RBackdropGridCntrl::RBackdropGridCntrl() 
//
// Description:   Destructor
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
RCollectionGridBase::~RCollectionGridBase()
{
}

BEGIN_MESSAGE_MAP(RCollectionGridBase, RGridCtrlBase)
	//{{AFX_MSG_MAP(RCollectionGridBase)
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_MESSAGE( LB_GETTEXT, OnLbGetText )
	ON_MESSAGE( LB_GETTEXTLEN, OnLbGetTextLen )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//*****************************************************************************
//
// Function Name:  RBackdropGridCtrl::SetData
//
// Description:    Sets the collection result array to be searched
//                 on when processing keyboard input.
//
// Returns:		    None
//
// Exceptions:	    None
//
//*****************************************************************************
void RCollectionGridBase::SetData( RSearchResultArray* pResultArray )
{
	m_pResultArray = pResultArray ;
}

//*****************************************************************************
//
// Function Name: CharToItem
//
// Description:   Virtual override for receiving character messages from
//                the grid control.
//
// Returns:		   None
//
// Exceptions:	   None
//
//*****************************************************************************
void RCollectionGridBase::CharToItem( UINT nKey, UINT nIndex )
{
	if (m_pResultArray == NULL)
		return;

	if (m_pResultArray->Count() <= 1)
		return;

	if (m_uiTimer)
	{
		// Kill the timer if there is one.
		KillTimer( m_uiTimer ) ;
	}

	// Update the buffer
	m_strBuffer  += toupper( (int) nKey ) ;

	int i ;
	int nLen   = m_strBuffer.GetLength() ;
	int nStart = (int) nIndex < m_pResultArray->Count() - 1 ? nIndex + 1 : 0 ;
	
	if (nLen > 1)
	{
		// Always start on the current
		// index, when there are multiple
		// characters to search on.
		nStart = nIndex ;
	}

	CString strItem( (*m_pResultArray)[nStart].namePtr ) ;
	strItem.MakeUpper() ;

	int compRetVal = SearchResult::CompareString( ::GetUserDefaultLCID(),
		NORM_IGNORECASE + SORT_STRINGSORT, (LPCTSTR) m_strBuffer,	nLen,
		(LPCTSTR) strItem, nLen );

	if (compRetVal == 1)
	{
		nStart = 0 ;
	}

	// Determine which new item to select
	for (i = nStart; i < m_pResultArray->Count(); i++)
	{
		strItem = (*m_pResultArray)[i].namePtr ;
//		strItem.MakeUpper() ;

		int compRetVal = SearchResult::CompareString( ::GetUserDefaultLCID(),
			NORM_IGNORECASE + SORT_STRINGSORT, (LPCTSTR) strItem,	-1,
			(LPCTSTR) m_strBuffer, -1 );

		if (compRetVal > 1)
		{
			goto _found_item ;
		}
	}

	// Went through the whole list without finding
	// a match, so set the selection to the last one.
	i -= 1 ;

_found_item:

	// Note, we only change the selection if the length of the search
	// string is one, or the buffer is an exact match of the found string
	// up to the number of characters in the buffer. (This behaviour is
	// what was observed in the windows tree control).
	compRetVal = 0 ;

	if (nLen > 1)
	{
		compRetVal = SearchResult::CompareString( ::GetUserDefaultLCID(),
			NORM_IGNORECASE + SORT_STRINGSORT, (LPCTSTR) strItem,	nLen,
			(LPCTSTR) m_strBuffer, nLen );
	}

	if (nLen == 1 || compRetVal == 2)
	{
		// Set the new selection
		SetCurSel( i ) ;
			
		GetParent()->PostMessage(WM_COMMAND, MAKEWPARAM( GetDlgCtrlID(), LBN_SELCHANGE ), 
				(LPARAM) GetSafeHwnd()) ;
	}

	// Start a new timer
	m_uiTimer = SetTimer( kTimerID, kTimerElapse, NULL ) ;
}

//****************************************************************************
//
// Function Name: OnDestroy
//
// Description:   WM_DESTROY message handler
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RCollectionGridBase::OnDestroy( ) 
{
	if (m_uiTimer)
	{
		KillTimer( m_uiTimer ) ;
		m_uiTimer = 0 ;
	}

	RGridCtrlBase::OnDestroy() ;
}

//****************************************************************************
//
// Function Name: OnTimer
//
// Description:   WM_TIMER message handler
//
// Returns:       None
//
// Exceptions:	   None
//
//****************************************************************************
void RCollectionGridBase::OnTimer( UINT nIDEvent ) 
{
	if (kTimerID == nIDEvent)
	{
		if (m_uiTimer)
		{
			KillTimer( m_uiTimer ) ;
			m_uiTimer = 0 ;
		}

		m_strBuffer = _T("") ;	// Clear the buffer
		return ;
	}
	
	RGridCtrlBase::OnTimer( nIDEvent ) ;
}


//****************************************************************************
//
// Function Name: OnLbGetText
//
// Description:   LB_GETTEXT message handler
//
// Returns:       Length of the string - copies string to buffer
//
// Exceptions:	   None
//
//****************************************************************************
LRESULT RCollectionGridBase::OnLbGetText( UINT wParam, LPTSTR lParam )
{
	// Get the text of the item at the specified index
	lstrcpy( lParam, (CString)(*m_pResultArray)[wParam].namePtr );
	
	// Return the length of the CString - The LB_GETTEXT message returns the length, so we should too
	return lstrlen( lParam );	
}


//****************************************************************************
//
// Function Name: OnLbGetTextLen
//
// Description:   LB_GETTEXTLEN message handler
//
// Returns:       Length of the string
//
// Exceptions:	   None
//
//****************************************************************************
LRESULT RCollectionGridBase::OnLbGetTextLen( UINT wParam, LONG /* lParam */ )
{
	return strlen((LPCTSTR)((*m_pResultArray)[wParam].namePtr));



	// Since the LB_GETTEXT message returns the length of the string
	// I'm just calling the OnLbGetText function and using the return
	// Why you might ask??? Well, any other way was not giving me consistent results
	// LPTSTR myStrItem;
	// return OnLbGetText(wParam, myStrItem);	
}
