// ****************************************************************************
//
//  File Name:			CommandUI.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle, S. Athanas
//
//  Description:		Declaration of the RCommandUI classes
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
//  $Logfile:: /PM8/Framework/Include/CommandUI.h                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_COMMANDUI_H_
#define		_COMMANDUI_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RMBCString;
class RMenu;

// ****************************************************************************
//
//  Class Name:		RCommandUI
//
//  Description:		Class for letting Views, Docs, or Apps set menu items.
//
// ****************************************************************************
//
class FrameworkLinkage RCommandUI
	{
	//	Typedefs and Enums
	public :
		typedef enum tagECommandUICheckState { kNoCheck = 0, kCheckMark, kUnknownState } ECommandUICheckState;

	//	Implementation
	public :
		virtual void							SetString( const RMBCString& string ) = 0;
		virtual void							Enable( ) = 0;
		virtual void							Disable( ) = 0;
		virtual void							SetCheck( ECommandUICheckState eState ) = 0;
		virtual void							SetCurSel( const RMBCString& string ) = 0;
		virtual BOOLEAN						GetDroppedState( ) = 0;
		virtual YCommandID					GetCommandID( ) const = 0;
	};

#ifdef	_WINDOWS

// ****************************************************************************
//
//  Class Name:		RMFCCommandUI
//
//  Description:		CommandUI class using an MFC CCmdUI object
//
// ****************************************************************************
//
class FrameworkLinkage RMFCCommandUI : public RCommandUI
	{
	//	Construction
	public :
													RMFCCommandUI( CCmdUI* pCmdUI );

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
		CCmdUI*									m_pCmdUI;
	};

#endif	// _WINDOWS

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		//	_COMMANDUI_H_
