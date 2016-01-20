// ****************************************************************************
//
//  File Name:			CommandUI.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle, S. Athanas
//
//  Description:		Definition of the RCCommandUI classes
//
//  Portability:		Platform independant
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/CommandUI.cpp                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "CommandUI.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Menu.h"

#ifdef	_WINDOWS

// ****************************************************************************
//
//  Function Name:	RMFCCommandUI::RMFCCommandUI( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMFCCommandUI::RMFCCommandUI( CCmdUI* pCmdUI )
	: m_pCmdUI( pCmdUI )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMFCCommandUI::GetCommandID( )
//
//  Description:		Return the Command ID for this CommandUI
//
//  Returns:			m_CommandID
//
//  Exceptions:		None
//
// ****************************************************************************
//
YCommandID RMFCCommandUI::GetCommandID( ) const
	{
	return m_pCmdUI->m_nID;
	}

// ****************************************************************************
//
//  Function Name:	RMFCCommandUI::SetString( )
//
//  Description:		Set the menu to have the given string
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMFCCommandUI::SetString( const RMBCString& string )
	{
	m_pCmdUI->SetText( string );
	}

// ****************************************************************************
//
//  Function Name:	RMFCCommandUI::Enable( )
//
//  Description:		Enable the menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMFCCommandUI::Enable( )
	{
	m_pCmdUI->Enable( TRUE );
	}

// ****************************************************************************
//
//  Function Name:	RMFCCommandUI::Disable( )
//
//  Description:		Disable the menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMFCCommandUI::Disable( )
	{
	m_pCmdUI->Enable( FALSE );
	}

// ****************************************************************************
//
//  Function Name:	RMFCCommandUI::SetCheck( )
//
//  Description:		Set a check for the menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMFCCommandUI::SetCheck( ECommandUICheckState eState )
	{
	m_pCmdUI->SetCheck( eState );
	}

// ****************************************************************************
//
//  Function Name:	RMFCCommandUI::SetCurSel( )
//
//  Description:		This function is for when the update must be for a 
//							combobox.  If the RMFCCommandUI object is a combobox or has
//							a child combobox, this checks the combobox to see if the
//							string passed in has been loaded.  If it has, this sets 
//							the combobox selection to that string; otherwise, it 
//							clears the combobox selection.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMFCCommandUI::SetCurSel( const RMBCString& string )
	{
	TpsAssert( m_pCmdUI->m_pMenu == NULL, "CommandUI object is not a control" );
	TpsAssert( m_pCmdUI->m_pOther, "m_pCmdUI->m_pOther is NULL" );

	HWND hWnd = m_pCmdUI->m_pOther->GetSafeHwnd();
	CWnd* pWnd = NULL;

	while ( hWnd )
		{
		pWnd = pWnd->FromHandle( hWnd ); 
		if ( pWnd->IsKindOf( RUNTIME_CLASS( CComboBox ) ) )
			{
				CComboBox* pComboBox = ( CComboBox* )pWnd;
				pComboBox->SetCurSel( pComboBox->FindStringExact( -1, string ) );
				break;
			}
		else
			hWnd = ::GetWindow( hWnd, GW_CHILD );
		}
	}

// ****************************************************************************
//
//  Function Name:	RMFCCommandUI::GetDroppedState( )
//
//  Description:		This function is for when the update must be for a 
//							combobox.  If the RMFCCommandUI object is a combobox or has
//							a child combobox, this checks the combobox to see if its
//							list box is dropped.
//
//  Returns:			TRUE:		List box is dropped
//							FALSE:	List box is not dropped
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RMFCCommandUI::GetDroppedState( )
	{
	TpsAssert( m_pCmdUI->m_pMenu == NULL, "CommandUI object is not a control" );
	TpsAssert( m_pCmdUI->m_pOther, "m_pCmdUI->m_pOther is NULL" );

	HWND hWnd = m_pCmdUI->m_pOther->GetSafeHwnd();
	CWnd* pWnd = NULL;

	while ( hWnd )
		{
		pWnd = pWnd->FromHandle( hWnd ); 
		if ( pWnd->IsKindOf( RUNTIME_CLASS( CComboBox ) ) )
			{
				CComboBox* pComboBox = ( CComboBox* )pWnd;
				return BOOLEAN( pComboBox->GetDroppedState() );
			}
		else
			hWnd = ::GetWindow( hWnd, GW_CHILD );
		}

	TpsAssertAlways( "RMFCCommandUI object is not a combobox" );
	return FALSE;
	}

#endif	// _WINDOWS
