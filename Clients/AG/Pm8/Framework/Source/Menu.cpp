// ****************************************************************************
//
//  File Name:			Menu.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RMenu class
//
//  Portability:		Platform independent
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
//  $Logfile:: /PM8/Framework/Source/Menu.cpp                                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "Menu.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ApplicationGlobals.h"
#include "ResourceManager.h"
#include "View.h"
#include "StandaloneApplication.h"

const YCommandID kMFCSystemCommandId = 0xF000;

// ****************************************************************************
//
//  Function Name:	RMenu::RMenu( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMenu::RMenu( YResourceMenu menu ) : m_Menu( menu )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMenu::GetSubMenu( )
//
//  Description:		Retrieves the specified submenu.
//
//							Note: the subMenuId is a 0 based index
//
//  Returns:			Pointer to a submenu object. It should be deleted by the
//							caller.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMenu* RMenu::GetSubMenu( YMenuIndex subMenuIndex ) const
	{
#ifdef	_WINDOWS

	// Get the submenu handle
	HMENU hSubMenu = ::GetSubMenu( m_Menu, subMenuIndex );

	// Create and return a new menu object
	if( hSubMenu )
		return new RMenu( hSubMenu );

	return NULL;

#endif
	}

// ****************************************************************************
//
//  Function Name:	RMenu::AppendItem( )
//
//  Description:		Appends a new menu item to the end of this menu.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenu::AppendItem( YCommandID commandId, YResourceId stringID )
	{
	// Load the string
	RResourceManager& resourceManager = ::GetResourceManager( );
	RMBCString string = resourceManager.GetResourceString( stringID );

	AppendItem( commandId, string );
	}

// ****************************************************************************
//
//  Function Name:	RMenu::AppendItem( )
//
//  Description:		Appends a new menu item to the end of this menu.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenu::AppendItem( YCommandID commandId, const RMBCString& string )
	{
#ifdef	_WINDOWS
	::AppendMenu( m_Menu, MF_STRING, commandId, string );
#endif
	}

// ****************************************************************************
//
//  Function Name:	RMenu::AppendMenu( )
//
//  Description:		Appends a menu to the end of this menu.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenu::AppendMenu( RMenu& menu )
	{
#ifdef _WINDOWS
	::AppendMenu( m_Menu, MF_POPUP, (UINT)menu.m_Menu, NULL );
#endif
	}

// ****************************************************************************
//
//  Function Name:	RMenu::AppendSeperator( )
//
//  Description:		Appends a new menu item to the end of this menu.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenu::AppendSeperator( )
	{
#ifdef	_WINDOWS
	// Append the menu
	::AppendMenu( m_Menu, MF_SEPARATOR, 0, NULL );
#endif
	}

// ****************************************************************************
//
//  Function Name:	RMenu::InsertItemByIndex( )
//
//  Description:		Inserts a new menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenu::InsertItemByIndex( YMenuIndex insertBeforeIndex, YCommandID commandId, YResourceId stringId )
	{
	// Load the string
	RResourceManager& resourceManager = ::GetResourceManager( );
	RMBCString string = resourceManager.GetResourceString( stringId );

	// Insert the item
	InsertItemByIndex( insertBeforeIndex, commandId, string );
	}

// ****************************************************************************
//
//  Function Name:	RMenu::InsertItemByIndex( )
//
//  Description:		Inserts a new menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenu::InsertItemByIndex( YMenuIndex insertBeforeIndex, YCommandID commandId, const RMBCString& string )
	{
#ifdef _WINDOWS

	MENUITEMINFO menuItemInfo;
	menuItemInfo.cbSize = sizeof( menuItemInfo );
	menuItemInfo.fMask = MIIM_ID | MIIM_TYPE;
	menuItemInfo.fType = MFT_STRING;
	menuItemInfo.wID = commandId;
	menuItemInfo.dwTypeData = const_cast<LPSZ>( (LPCSZ)string );
	menuItemInfo.cch = string.GetStringLength( );

	::InsertMenuItem( m_Menu, insertBeforeIndex, TRUE, &menuItemInfo );

#endif
	}

// ****************************************************************************
//
//  Function Name:	RMenu::InsertItemByCommand( )
//
//  Description:		Inserts a new menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenu::InsertItemByCommand( YCommandID insertBeforeCommand, YCommandID commandId, YResourceId stringId )
	{
	// Load the string
	RResourceManager& resourceManager = ::GetResourceManager( );
	RMBCString string = resourceManager.GetResourceString( stringId );

#ifdef _WINDOWS

	MENUITEMINFO menuItemInfo;
	menuItemInfo.cbSize = sizeof( menuItemInfo );
	menuItemInfo.fMask = MIIM_ID | MIIM_TYPE;
	menuItemInfo.fType = MFT_STRING;
	menuItemInfo.wID = commandId;
	menuItemInfo.dwTypeData = const_cast<LPSZ>( (LPCSZ)string );
	menuItemInfo.cch = string.GetStringLength( );

	::InsertMenuItem( m_Menu, insertBeforeCommand, FALSE, &menuItemInfo );

#endif
	}

// ****************************************************************************
//
//  Function Name:	RMenu::InsertSeperator( )
//
//  Description:		Inserts a seperator
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenu::InsertSeperator( YMenuIndex insertBeforeIndex )
	{
#ifdef _WINDOWS

	MENUITEMINFO menuItemInfo;
	menuItemInfo.cbSize = sizeof( menuItemInfo );
	menuItemInfo.fMask = MIIM_TYPE;
	menuItemInfo.fType = MFT_SEPARATOR;

	::InsertMenuItem( m_Menu, insertBeforeIndex, TRUE, &menuItemInfo );

#endif
	}

// ****************************************************************************
//
//  Function Name:	RMenu::RemoveItemByIndex( )
//
//  Description:		Removes a menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenu::RemoveItemByIndex( YMenuIndex menuIndex )
	{
#ifdef _WINDOWS
	::RemoveMenu( m_Menu, menuIndex, MF_BYPOSITION );
#endif
	}

// ****************************************************************************
//
//  Function Name:	RMenu::RemoveItemByCommand( )
//
//  Description:		Removes a menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenu::RemoveItemByCommand( YCommandID menuId )
	{
#ifdef _WINDOWS
	::RemoveMenu( m_Menu, menuId, MF_BYCOMMAND );
#endif
	}

// ****************************************************************************
//
//  Function Name:	RMenu::SetItemTextByIndex( )
//
//  Description:		Modifies a menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenu::SetItemTextByIndex( YMenuIndex menuIndex, const RMBCString& string )
	{
#ifdef _WINDOWS

	MENUITEMINFO menuItemInfo;
	menuItemInfo.cbSize = sizeof( menuItemInfo );
	menuItemInfo.fMask = MIIM_TYPE;
	menuItemInfo.fType = MFT_STRING;
	menuItemInfo.dwTypeData = const_cast<LPSZ>( (LPCSZ)string );
	menuItemInfo.cch = string.GetStringLength( );

	::SetMenuItemInfo( m_Menu, menuIndex, TRUE, &menuItemInfo );

#endif
	}

// ****************************************************************************
//
//  Function Name:	RMenu::SetItemTextByIndex( )
//
//  Description:		Modifies a menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenu::SetItemTextByIndex( YMenuIndex menuIndex, YResourceId stringId )
	{
	// Load the string
	RResourceManager& resourceManager = ::GetResourceManager( );
	RMBCString string = resourceManager.GetResourceString( stringId );

	// Set the text
	SetItemTextByIndex( menuIndex, string );
	}

// ****************************************************************************
//
//  Function Name:	RMenu::SetItemTextByCommand( )
//
//  Description:		Modifies a menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenu::SetItemTextByCommand( YCommandID menuId, YResourceId stringId )
	{
	// Load the string
	RResourceManager& resourceManager = ::GetResourceManager( );
	RMBCString string = resourceManager.GetResourceString( stringId );

#ifdef _WINDOWS

	MENUITEMINFO menuItemInfo;
	menuItemInfo.cbSize = sizeof( menuItemInfo );
	menuItemInfo.fMask = MIIM_TYPE;
	menuItemInfo.fType = MFT_STRING;
	menuItemInfo.dwTypeData = const_cast<LPSZ>( (LPCSZ)string );
	menuItemInfo.cch = string.GetStringLength( );

	::SetMenuItemInfo( m_Menu, menuId, FALSE, &menuItemInfo );

#endif
	}

// ****************************************************************************
//
//  Function Name:	RMenu::EnableItemByIndex( )
//
//  Description:		Modifies a menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenu::EnableItemByIndex( YMenuIndex menuIndex )
	{
#ifdef _WINDOWS

	::EnableMenuItem( m_Menu, menuIndex, MF_BYPOSITION | MF_ENABLED );

#endif
	}

// ****************************************************************************
//
//  Function Name:	RMenu::DisableItemByIndex( )
//
//  Description:		Modifies a menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenu::DisableItemByIndex( YMenuIndex menuIndex )
	{
#ifdef _WINDOWS

	::EnableMenuItem( m_Menu, menuIndex, MF_BYPOSITION | MF_GRAYED | MF_DISABLED );

#endif
	}

// ****************************************************************************
//
//  Function Name:	RMenu::CheckItemByIndex( )
//
//  Description:		Modifies a menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenu::CheckItemByIndex( YMenuIndex menuIndex )
	{
#ifdef _WINDOWS

	MENUITEMINFO menuItemInfo;
	menuItemInfo.cbSize = sizeof( menuItemInfo );
	menuItemInfo.fMask = MIIM_CHECKMARKS | MIIM_STATE;
	menuItemInfo.fState = MFS_CHECKED;
	menuItemInfo.hbmpChecked = NULL;
	menuItemInfo.hbmpUnchecked = NULL;

	::SetMenuItemInfo( m_Menu, menuIndex, TRUE, &menuItemInfo );

#endif
	}

// ****************************************************************************
//
//  Function Name:	RMenu::UncheckItemByIndex( )
//
//  Description:		Modifies a menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenu::UncheckItemByIndex( YMenuIndex menuIndex )
	{
#ifdef _WINDOWS

	MENUITEMINFO menuItemInfo;
	menuItemInfo.cbSize = sizeof( menuItemInfo );
	menuItemInfo.fMask = MIIM_CHECKMARKS | MIIM_STATE;
	menuItemInfo.fState = MFS_UNCHECKED;
	menuItemInfo.hbmpChecked = NULL;
	menuItemInfo.hbmpUnchecked = NULL;

	::SetMenuItemInfo( m_Menu, menuIndex, TRUE, &menuItemInfo );

#endif
	}

// ****************************************************************************
//
//  Function Name:	RMenu::GetItemCount( )
//
//  Description:		Gets the number of items in this menu
//
//  Returns:			See description
//
//  Exceptions:		None
//
// ****************************************************************************
//
YMenuIndex RMenu::GetItemCount( ) const
	{
#ifdef _WINDOWS

	return ::GetMenuItemCount( m_Menu );

#endif
	}

// ****************************************************************************
//
//  Function Name:	RMenu::GetIndexFromCommand( )
//
//  Description:		Gets the index of the item in this menu with the specified
//							command id
//
//  Returns:			See description
//
//  Exceptions:		None
//
// ****************************************************************************
//
YMenuIndex RMenu::GetIndexFromCommand( YCommandID commandId ) const
	{
	// Get the number of items in this menu
	YMenuIndex numItems = GetItemCount( );

	// Iterate the items, looking for one with the specified command id
	for( YMenuIndex i = 0; i < numItems; ++i )
		if( GetCommandFromIndex( i ) == commandId )
			return i;

	TpsAssertAlways( "Command id not found in menu." );

	return -1;
	}

// ****************************************************************************
//
//  Function Name:	RMenu::GetCommandFromIndex( )
//
//  Description:		Gets the command of the item in this menu with the specified
//							index
//
//  Returns:			See description
//
//  Exceptions:		None
//
// ****************************************************************************
//
YCommandID RMenu::GetCommandFromIndex( YMenuIndex index ) const
	{
#ifdef _WINDOWS

	return static_cast<YCommandID>( ::GetMenuItemID( m_Menu, index ) );

#endif
	}

// ****************************************************************************
//
//  Function Name:	RMenu::GetCommandFromIndex( )
//
//  Description:		Determines if the menu item identified by the specified
//							index is a seperator.
//
//  Returns:			TRUE if the item is a seperator
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RMenu::IsSeperator( YMenuIndex index ) const
	{
#ifdef _WINDOWS
	
	// If the command id is 0, its a seperator
	if( GetCommandFromIndex( index ) == 0 )
		return TRUE;

	return FALSE;

#endif
	}

// ****************************************************************************
//
//  Function Name:	RMenu::Update( )
//
//  Description:		Determines if the menu item identified by the specified
//							index is enabled.
//
//  Returns:			TRUE the item is enabled
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RMenu::IsEnabled( YMenuIndex index ) const
	{
#ifdef _WINDOWS

	MENUITEMINFO menuItemInfo;
	menuItemInfo.cbSize = sizeof( menuItemInfo );
	menuItemInfo.fMask = MIIM_STATE;

	::GetMenuItemInfo( m_Menu, index, TRUE, &menuItemInfo );

	if( !( menuItemInfo.fState & MFS_DISABLED ) )
		return TRUE;

	return FALSE;

#endif
	}

// ****************************************************************************
//
//  Function Name:	RMenu::Update( )
//
//  Description:		Updates the state of this menu
//
//  Returns:			TRUE if at least one item in this menu was enabled
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RMenu::Update( )
	{
	// Use the active view as the default comand target
	RCommandTarget* pCommandTarget = ::GetActiveView( );

	// If there is no active view, route to the application
	if( !pCommandTarget )
		pCommandTarget = &::GetApplication( );

	// Iterate the items, calling command UI handlers for each one
	BOOLEAN fSomethingEnabled = FALSE;
	for( YMenuIndex i = 0; i < GetItemCount( ); ++i )
		{
		// Check to see if it is a submenu
		RMenu* pSubMenu = GetSubMenu( i );
		if( pSubMenu )
			{
			// Update the submenu. If there are no enabled items in the submenu, disable the entire menu
			if( pSubMenu->Update( ) )
				EnableItemByIndex( i );
			else
				DisableItemByIndex( i );

			// Delete the submenu
			delete pSubMenu;
			}

		// Now check to see if it is a seperator. If so skip it
		else if( IsSeperator( i ) )
			continue;

		// Its not a submenu, and its not a seperator. Create a CommandUI object
		// and send it to the active view
		else
			{
			// Create a menu command UI object
			RMenuCommandUI commandUI( this, i );

			// Route through our command map
			if( !pCommandTarget->GetRCommandMap( )->OnCommandUI( commandUI, pCommandTarget ) )
				{
				BOOLEAN fHandled = FALSE;
				BOOLEAN fAutoEnable = TRUE;

#ifdef _WINDOWS
				// If we couldnt handle it, let MFC try. Build a CCmdUI object
				CCmdUI cmdUI;
				cmdUI.m_nID = commandUI.GetCommandID( );
				cmdUI.m_nIndex = i;
				cmdUI.m_nIndexMax = GetItemCount( ); 
				cmdUI.m_pMenu = CMenu::FromHandle( m_Menu );
				fAutoEnable = static_cast<BOOLEAN>( cmdUI.m_nID < kMFCSystemCommandId );
				cmdUI.DoUpdate( ::AfxGetMainWnd( ), fAutoEnable );
				fHandled = IsEnabled( i );
#endif

				// Enable the command if we can find a command handler
				if( !fHandled && fAutoEnable )
					{
					if( pCommandTarget->GetRCommandMap( )->OnCommand( commandUI.GetCommandID( ), pCommandTarget, FALSE ) )
						commandUI.Enable( );
					else
						commandUI.Disable( );
					}
				}
			}

		// Remember if something was enabled, so we can return it
		if( IsEnabled( i ) )
			fSomethingEnabled = TRUE;
		}

	return fSomethingEnabled;
	}
									  
// ****************************************************************************
//
//  Function Name:	RMenu::operator YMenuPlatformReference( )
//
//  Description:		Conversion operator
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMenu::operator YResourceMenu( ) const
	{
	return m_Menu;
	}

// ****************************************************************************
//
//  Function Name:	RPopupMenu::RPopupMenu( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPopupMenu::RPopupMenu( YResourceId menuResource ) : RMenu( NULL )
	{
	// Load the menu resource
	m_MenuResource = ::GetResourceManager( ).GetResourceMenu( menuResource );

#ifdef _WINDOWS
	// Get the first sub menu and use it as the pop up
	m_Menu = ::GetSubMenu( m_MenuResource, 0 );
#endif

	TpsAssert( m_Menu, "No popup menu." );
	}

// ****************************************************************************
//
//  Function Name:	RPopupMenu::RPopupMenu( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RPopupMenu::~RPopupMenu( )
	{
	::DestroyMenu( m_MenuResource );
	}

// ****************************************************************************
//
//  Function Name:	RPopupMenu::TrackPopupMenu( )
//
//  Description:		Tracks a popup menu
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RPopupMenu::TrackPopupMenu( RView* pView, const RRealPoint& point )
	{
	// Update the menu
	Update( );

	// Get the view transform
	R2dTransform transform;
	pView->GetViewTransform( transform, TRUE );

	// Transform the point
	RIntPoint systemPoint = point * transform;

#ifdef	 _WINDOWS
	
	// Get the HWND of the toplevel window
	HWND hwnd = pView->GetCWnd( ).GetSafeHwnd( );

	// Convert the client point to a screen point
	CPoint screenPoint( systemPoint.m_x, systemPoint.m_y );
	::ClientToScreen( hwnd, &screenPoint );

	// Begin tracking the popup menu
	::TrackPopupMenu( m_Menu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, screenPoint.x, screenPoint.y, 0, hwnd, NULL );

#endif	// _WINDOWS
	}

// ****************************************************************************
//
//  Function Name:	RMenuBar::RMenuBar( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMenuBar::RMenuBar( YResourceMenu menu ) : RMenu( menu )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMenuBar::GetSubMenu( )
//
//  Description:		Retrieves the specified submenu.
//
//							Note: the subMenuId is a 0 based index
//
//  Returns:			Pointer to a submenu object. It should be deleted by the
//							caller.
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMenu* RMenuBar::GetSubMenu( YMenuIndex subMenuIndex ) const
	{
#ifdef	_WINDOWS

	// If the active frame is maximized, the MDI system menu is at index 0, and we
	// need to increment the submenu index
	CWnd* pActiveFrame = static_cast<CFrameWnd*>( AfxGetMainWnd( ) )->GetActiveFrame( );
	if( pActiveFrame && pActiveFrame->IsZoomed( ) )
		subMenuIndex++;

#endif

	// Call the base method to get the submenu
	return RMenu::GetSubMenu( subMenuIndex );
	}

// ****************************************************************************
//
//  Function Name:	RMenuCommandUI::RMenuCommandUI( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMenuCommandUI::RMenuCommandUI( RMenu* pMenu, YMenuIndex menuIndex )
	: m_pMenu( pMenu ),
	  m_MenuIndex( menuIndex )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RMenuCommandUI::GetCommandID( )
//
//  Description:		Return the Command ID for this CommandUI
//
//  Returns:			m_CommandID
//
//  Exceptions:		None
//
// ****************************************************************************
//
YCommandID RMenuCommandUI::GetCommandID( ) const
	{
	return m_pMenu->GetCommandFromIndex( m_MenuIndex );
	}

// ****************************************************************************
//
//  Function Name:	RMenuCommandUI::SetString( )
//
//  Description:		Set the menu to have the given string
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenuCommandUI::SetString( const RMBCString& string )
	{
	m_pMenu->SetItemTextByIndex( m_MenuIndex, string );
	}

// ****************************************************************************
//
//  Function Name:	RMenuCommandUI::Enable( )
//
//  Description:		Enable the menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenuCommandUI::Enable( )
	{
	m_pMenu->EnableItemByIndex( m_MenuIndex );
	}

// ****************************************************************************
//
//  Function Name:	RMenuCommandUI::Disable( )
//
//  Description:		Disable the menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenuCommandUI::Disable( )
	{
	m_pMenu->DisableItemByIndex( m_MenuIndex );
	}

// ****************************************************************************
//
//  Function Name:	RMenuCommandUI::SetCheck( )
//
//  Description:		Set a check for the menu item
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenuCommandUI::SetCheck( ECommandUICheckState eState )
	{
	if( eState == kCheckMark )
		m_pMenu->CheckItemByIndex( m_MenuIndex );
	else
		m_pMenu->UncheckItemByIndex( m_MenuIndex );
	}

// ****************************************************************************
//
//  Function Name:	RMenuCommandUI::SetCurSel( )
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
void RMenuCommandUI::SetCurSel( const RMBCString& )
	{
	TpsAssertAlways( "CommandUI object is not a control" );
	}

// ****************************************************************************
//
//  Function Name:	RMenuCommandUI::GetDroppedState( )
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
BOOLEAN RMenuCommandUI::GetDroppedState( )
	{
	TpsAssertAlways( "CommandUI object is not a control" );

	return FALSE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RMenu::Validate( )
//
//  Description:		Validate the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMenu::Validate( ) const
	{
	RObject::Validate( );
	}

#endif		//	TPSDEBUG
