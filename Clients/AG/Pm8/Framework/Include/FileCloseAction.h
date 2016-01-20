// ****************************************************************************
//
//  File Name:			FileCloseAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				E. VanHelene
//
//  Description:		Declaration of the RFileCloseAction class
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
//  $Logfile:: /PM8/Framework/Include/FileCloseAction.h                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_FILECLOSEACTION_H_
#define		_FILECLOSEACTION_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RFileCloseAction
//
//  Description:	Action to open a document
//
// ****************************************************************************
//
class RFileCloseAction : public RAction
	{
	// Construction, destruction & Intialization
	public :
												RFileCloseAction( RScript& script );
												RFileCloseAction( const RMBCString& rInitialFilename );
		virtual								~RFileCloseAction( );

	//	Identifier
	public :
		static	YActionId				m_ActionId;

	// Operations
	public :
		virtual BOOLEAN					Do( );

	// Scripting Operations
	public :
		virtual BOOLEAN					WriteScript( RScript& script ) const;
		const RMBCString&					GetInitialFileName( );

	// Member data
	private :
		RMBCString							m_rInitialFilename;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		// _FILECLOSEACTION_H_
