// ****************************************************************************
//
//  File Name:			UndoManager.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RUndoManager class
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
//  $Logfile:: /PM8/Framework/Include/UndoManager.h                           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_UNDOMANAGER_H_
#define		_UNDOMANAGER_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//	Forward References
class RUndoableAction;

const	YCounter				kDefaultUndoCount = 10;

// ****************************************************************************
//
//  Class Name:		RUndoManager
//
//  Description:		The undo manager for all undoable actions in a document.
//
// ****************************************************************************
//
class	FrameworkLinkage RUndoManager : public RCommandTarget
	{
	// Construction, Destruction & Initialization
	public :
													RUndoManager( );
		virtual									~RUndoManager( );
	
	// Operations
	public :
		void										AddAction( RUndoableAction* pAction );
		void										UndoAction( );
		void										RedoAction( );
		void										FreeAllActions( );
		BOOLEAN									FreeUndoneActions( );
		void										DocSaved( );
		void										ForceDocDirty( );
		BOOLEAN									DocDirty( );

		YCounter									CountUndoableActions( ) const;

		uWORD										GetUndoStringId( ) const;
		uWORD										GetRedoStringId( ) const;
		YResourceId								GetUndoStringAndId( RMBCString& menuString ) const;
									
		void										SetMaxUndoableActions( YCounter maxUndos );

		const YActionId&						GetNextActionId( ) const;
	
		// Member data
		protected :
			typedef	RList<RUndoableAction*>	RUndoList;
			typedef	RUndoList::YIterator		RUndoListIterator;

			BOOLEAN								m_fUndoListChangedSinceDocSaved;		//	TRUE: the associated doc is dirty because the 
																									//		items that were on the undo list when the doc was last saved have changed
			BOOLEAN								m_fTrackingSaveCounter;					//	TRUE: m_LastDocSaveUndo is in use
			YCounter								m_LastDocSaveUndo;						//	last undone action when our associated document was last saved
			YCounter								m_MaximumUndos;
			RUndoList							m_UndoList;
			RUndoList::YIterator				m_CurrentUndo;

		// Implementation
	private :
		RUndoableAction*						GetAndValidateAction( const RUndoList::YIterator& iterator, BOOLEAN fActionDone ) const;
		YCounter									GetCurrentUndoIndex( ) const;
		RUndoableAction*						GetNextActionToBeUndone( ) const;
		RUndoableAction*						GetNextActionToBeRedone( ) const;

	// Command handlers
	private :
		void										OnUpdateUndo( RCommandUI& commandUI ) const;
		void										OnUpdateRedo( RCommandUI& commandUI ) const;

	// Command map
	public :
		virtual RCommandMapBase*		GetRCommandMap( ) const;

	private :
		static RCommandMap<RUndoManager, RCommandTarget> m_CommandMap;
		friend class RCommandMap<RUndoManager, RCommandTarget>;

	friend class RChainedUndoableAction;

#ifdef	TPSDEBUG	
		// Debugging Support
		public :
			virtual void						Validate( ) const;
#endif	//	TPSDEBUG
	};


//	this value returned from addaction means that the calling doc is dirty
//	because the undo list was truncated
const BOOLEAN kUndoListTruncatedSoDocIsDirty = TRUE;

// ****************************************************************************
// 					Inlines
// ****************************************************************************

// ****************************************************************************
//
//  Function Name:	RUndoManager::GetRCommandMap( )
//
//  Description:		Gets a pointer to this classes command map.
//
//  Returns:			See above
//
//  Exceptions:		None
//
// ****************************************************************************
//
inline RCommandMapBase* RUndoManager::GetRCommandMap( ) const
	{
	return &m_CommandMap;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_UNDOMANAGER_H_
