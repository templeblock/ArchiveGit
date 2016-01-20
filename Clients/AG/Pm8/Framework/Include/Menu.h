// ****************************************************************************
//
//  File Name:			Menu.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RMenu class
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
//  $Logfile:: /PM8/Framework/Include/Menu.h                                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_MENU_H_
#define		_MENU_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RView;

// ****************************************************************************
//
//  Class Name:	RMenu
//
//  Description:	This class represents a menu bar
//
// ****************************************************************************
//
class FrameworkLinkage RMenu : public RObject
	{
	// Construction
	public :
												RMenu( YResourceMenu menu );

	// Operations
	public :
		virtual RMenu*						GetSubMenu( YMenuIndex subMenuId ) const;
		void									AppendItem( YCommandID commandId, YResourceId stringId);
		void									AppendItem( YCommandID commandId, const RMBCString& string );
		void									AppendMenu( RMenu& menu );
		void									AppendSeperator( );
		void									InsertItemByIndex( YMenuIndex insertBeforeIndex, YCommandID commandId, YResourceId stringId );
		void									InsertItemByIndex( YMenuIndex insertBeforeIndex, YCommandID commandId, const RMBCString& string );
		void									InsertItemByCommand( YCommandID insertBeforeId, YCommandID commandId, YResourceId stringId );
		void									InsertSeperator( YMenuIndex insertBeforeIndex );
		void									RemoveItemByIndex( YMenuIndex menuIndex );
		void									RemoveItemByCommand( YCommandID menuId );
		void									SetItemTextByIndex( YMenuIndex menuIndex, const RMBCString& string );
		void									SetItemTextByIndex( YMenuIndex menuIndex, YResourceId stringId );
		void									SetItemTextByCommand( YCommandID menuId, YResourceId stringId );
		void									EnableItemByIndex( YMenuIndex menuIndex );
		void									DisableItemByIndex( YMenuIndex menuIndex );
		void									CheckItemByIndex( YMenuIndex menuIndex );
		void									UncheckItemByIndex( YMenuIndex menuIndex );
		YMenuIndex							GetItemCount( ) const;
		YMenuIndex							GetIndexFromCommand( YCommandID commandId ) const;
		YCommandID							GetCommandFromIndex( YMenuIndex index ) const;
		BOOLEAN								IsSeperator( YMenuIndex index ) const;
		BOOLEAN								IsEnabled( YMenuIndex index ) const;
												operator YResourceMenu( ) const;
		BOOLEAN								Update( );

	// Disable the assignment operator
	private :
		RMenu&								operator=( const RMenu& rhs );

	// Member data
	protected :
		YResourceMenu						m_Menu;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
//
//  Class Name:	RMenuBar
//
//  Description:	This class represents the toplevel menu bar
//
// ****************************************************************************
//
class RMenuBar : public RMenu
	{
	// Construction
	public :
												RMenuBar( YResourceMenu menu );

	// Operations
	public :
		virtual RMenu*						GetSubMenu( YMenuIndex subMenuId ) const;
	};

// ****************************************************************************
//
//  Class Name:	RPopupMenu
//
//  Description:	This class represents a popup menu
//
// ****************************************************************************
//
class FrameworkLinkage RPopupMenu : public RMenu
	{
	// Construction
	public :
												RPopupMenu( YResourceId resourceId );
												~RPopupMenu( );

	// Operations
	public :
		void									TrackPopupMenu( RView* pView, const RRealPoint& point );

	private:
		YResourceMenu						m_MenuResource;
	};

// ****************************************************************************
//
//  Class Name:		RMenuCommandUI
//
//  Description:		CommandUI class using an RMenu object
//
// ****************************************************************************
//
class FrameworkLinkage RMenuCommandUI : public RCommandUI
	{
	//	Construction
	public :
													RMenuCommandUI( RMenu* pMenu, YMenuIndex menuIndex );

	//	Implementation
	public :
		virtual void							SetString( const RMBCString& string );
		virtual void							Enable( );
		virtual void							Disable( );
		virtual void							SetCheck( ECommandUICheckState eState );
		virtual void							SetCurSel( const RMBCString& string );
		virtual BOOLEAN						GetDroppedState( );
		virtual YCommandID					GetCommandID( ) const;

	//	Member data
	private :
		RMenu*									m_pMenu;
		YMenuIndex								m_MenuIndex;
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _MENU_H_
