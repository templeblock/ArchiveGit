// ****************************************************************************
//
//  File Name:			MultiUndoManager.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RMultiUndoManager class
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
//  $Logfile:: /PM8/Framework/Include/MultiUndoManager.h                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_MULTIUNDOMANAGER_H_
#define		_MULTIUNDOMANAGER_H_

#ifndef	_UNDOMANAGER_H_
#include	"UndoManager.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:		RUndoManager
//
//  Description:		The undo manager for all undoable actions in a document.
//
// ****************************************************************************
//
class	FrameworkLinkage RMultiUndoManager : public RUndoManager
	{
	// Construction, Destruction & Initialization
	public :
													RMultiUndoManager( RUndoManager* pPreviousManager );
		virtual									~RMultiUndoManager( );
	
	// Operations
	public :
		void										UndoAllAction( );
		void										RedoAllAction( );
		RUndoManager*							GetPreviousUndoManager( ) const;

	//	Members
	private :
		RUndoManager*							m_pPreviousUndoManager;

#ifdef	TPSDEBUG	
		// Debugging Support
		public :
			virtual void						Validate( ) const;
#endif	//	TPSDEBUG
	};


// ****************************************************************************
// 					Inlines
// ****************************************************************************


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_MULTIUNDOMANAGER_H_
