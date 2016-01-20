// ****************************************************************************
//
//  File Name:			FileOpenAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				E. VanHelene
//
//  Description:		Declaration of the RFileOpenAction class
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
//  $Logfile:: /PM8/Framework/Include/FileOpenAction.h                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_FILEOPENACTION_H_
#define		_FILEOPENACTION_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RFileOpenAction
//
//  Description:	Action to open a document
//
// ****************************************************************************
//
class RFileOpenAction : public RAction
	{
	// Construction, destruction & Intialization
	public :
												RFileOpenAction( RScript& script );
												RFileOpenAction( const RMBCString& rFilename );
		virtual								~RFileOpenAction( );

	//	Identifier
	public :
		static	YActionId				m_ActionId;

	// Operations
	public :
		virtual BOOLEAN					Do( );

	// Scripting Operations
	public :
		virtual BOOLEAN					WriteScript( RScript& script ) const;
		const RMBCString&					GetFileName( );

	// Member data
	private :
		RMBCString							m_rFilename;

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

#endif		// _FILEOPENACTION_H_
