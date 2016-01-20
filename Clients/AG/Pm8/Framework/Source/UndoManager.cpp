// ****************************************************************************
//
//  File Name:			UndoManager.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				M. Houle
//
//  Description:		Definition of the RUndoManager class
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
//  $Logfile:: /PM8/Framework/Source/UndoManager.cpp                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "UndoManager.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include	"UndoableAction.h"
#include "ResourceManager.h"
#include "ApplicationGlobals.h"
#include "FrameworkResourceIds.h"
#include "Configuration.h"

// Command Map
RCommandMap<RUndoManager, RCommandTarget> RUndoManager::m_CommandMap;

RCommandMap<RUndoManager, RCommandTarget>::RCommandMap( )
	{
	UpdateCommand( COMMAND_MENU_EDIT_UNDO, RUndoManager::OnUpdateUndo );
	UpdateCommand( COMMAND_MENU_EDIT_REDO, RUndoManager::OnUpdateRedo );
	}

// ****************************************************************************
//
//  Function Name:	RUndoManager::RUndoManager( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUndoManager::RUndoManager( )
	: m_MaximumUndos( kDefaultUndoCount ),
	  m_fTrackingSaveCounter( TRUE ),
	  m_fUndoListChangedSinceDocSaved( FALSE ),
	  m_UndoList( )
	{
	m_LastDocSaveUndo = 0;
	m_CurrentUndo = m_UndoList.End();
	///xxx	TODO: Retrieve the Maximum Undos from
	//					the preferences or something

	//	Check if the undo list has been increased by a PowerUser...	
	uLONG uValue;
	RPowerUserConfiguration rConfig;
	if (rConfig.GetLongValue(RPowerUserConfiguration::kUndoCount, uValue)) m_MaximumUndos = Max(YCounter(uValue), m_MaximumUndos);
	}


// ****************************************************************************
//
//  Function Name:	RUndoManager::~RUndoManager( )
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUndoManager::~RUndoManager( )
	{
	FreeAllActions( );

#if 1
	TpsAssert( m_UndoList.Count() == 0, "There are still actions in the undo list." );
#else
	//	REVIEW - ESV 7/31/96 - this was not necessary in the past. Mike Houle 
	//	should look to see why this is necessary now when he gets back from vacation
	
	//	we still have some actions in this list so get rid of them
	RUndoListIterator	iterator;
	while ( (iterator = m_UndoList.Start()) != m_UndoList.End() )
		{
		RUndoableAction*	pAction = GetAndValidateAction( iterator, TRUE );
		m_UndoList.RemoveAt( iterator );
		delete pAction;
		}
#endif
	}

// ****************************************************************************
//
//	Function Name:		RUndoManager::GetAndValidateAction
//
//	Description:		Code reuse to get better UndoAction warning messages.
//
//	Returns:				The action pointed to by the iterator
//
//	Exceptions:			None
//
// ****************************************************************************
//
inline	RUndoableAction*	RUndoManager::GetAndValidateAction( const RUndoListIterator& iterator, BOOLEAN fActionDone ) const
	{
	TpsAssertIsObject( RUndoableAction, *iterator );
	RUndoableAction*		pAction = *iterator;

	fActionDone;	//	To stop the warning of Unreference parameter in a non-debug build

#ifdef	TPSDEBUG
	//	Validate the EActionState of the action
	if ( fActionDone )
		{
		TpsAssert( pAction->GetState() != RUndoableAction::kActionNotDone, "Action state should be Do or Redo, but is NotDone." );
		TpsAssert( pAction->GetState() != RUndoableAction::kActionUndo, "Action state should be Do or Redo, but is Undo." );
		}
	else
		{
		TpsAssert( pAction->GetState() != RUndoableAction::kActionNotDone, "Action state should be Undo, but is NotDone." );
		TpsAssert( pAction->GetState() != RUndoableAction::kActionDo, "Action state should be Undo, but is Do." );
		TpsAssert( pAction->GetState() != RUndoableAction::kActionRedo, "Action state should be Undo, but is Redo." );
		}
#endif	//	TPSDEBUG

	return pAction;
	}


// ****************************************************************************
//
//  Function Name:	RUndoManager::AddAction( )
//
//  Description:		Add the given action to the undo managers list
//
//  Returns:			Nothing
//
//  Exceptions:		Memory Exception
//
// ****************************************************************************
//
void RUndoManager::AddAction( RUndoableAction* pAction )
	{
	TpsAssert( pAction->GetState() == RUndoableAction::kActionDo, "Action state is not DO, was it setup correctly" );

	//	First, delete all pending redo's
	m_fUndoListChangedSinceDocSaved = FreeUndoneActions();

	//	Next check if we must free up an action
	while ( m_MaximumUndos == CountUndoableActions( ) )
		{
		RUndoListIterator	iterator				= m_UndoList.Start();
		
		RUndoableAction*	pActionToDelete	= GetAndValidateAction( iterator, TRUE );
		m_UndoList.RemoveAt( iterator );
		delete pActionToDelete;

		if ( m_fTrackingSaveCounter )			//	if we're checking whether our saved doc has been made dirty
			{
			//	decrement the index of the last undo before the save because
			//	we've removed an action from the beginning of the undo list
			--m_LastDocSaveUndo;			 

			if ( 0 > m_LastDocSaveUndo )
				{
				//	if we're removing our doc save marker then
				//	remember that we can't make the saved doc on disk match the memory doc
				m_fTrackingSaveCounter = FALSE;
				m_fUndoListChangedSinceDocSaved = kUndoListTruncatedSoDocIsDirty;
				}
			}										
		}

	//	Add this action to the end
	try
		{
		pAction->m_pUndoManager = this;
		m_UndoList.InsertAtEnd( pAction );
		m_CurrentUndo = m_UndoList.End();
		--m_CurrentUndo;
		}
	catch( YException /* exception */ )
		{
		///xxx Alert user action will not be undoable
		throw;	//	just pass exception up
		}
	}

// ****************************************************************************
//
//  Function Name:	RUndoManager::UndoAction( )
//
//  Description:		Undo the current item pointed to by the CurrentUndo
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RUndoManager::UndoAction( )
	{
	TpsAssert(CountUndoableActions( ) > 0, "UndoAction called with no undo's in the list" );

	while (m_CurrentUndo != m_UndoList.End())
		{
		RUndoableAction*	pAction = GetAndValidateAction( m_CurrentUndo, TRUE );
		pAction->Undo( );
		//	decrementing the current undo will cause the	list to wrap 
		// from Start to End causing no more undo's to be generated.
		--m_CurrentUndo;
		if ( !pAction->IsTransparent( ) )
			break;	//	We executed 1 undo, so bail
		}
	}

// ****************************************************************************
//
//  Function Name:	RUndoManager::RedoAction( )
//
//  Description:		Undo the current item pointed to by the CurrentUndo
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RUndoManager::RedoAction( )
	{
	TpsAssert(CountUndoableActions( ) > 0, "RedoAction called with no undo's in the list" );

	RUndoList::YIterator	iterator	= m_UndoList.End();
	--iterator;
	while (m_CurrentUndo != iterator)
		{
		//	decrementing the current undo will cause the	list to wrap 
		// from Start to End causing no more undo's to be generated.
		++m_CurrentUndo;
		RUndoableAction*	pAction = GetAndValidateAction( m_CurrentUndo, FALSE );
		pAction->Redo( );
		if ( !pAction->IsTransparent() )
			break;	//	We Redid 1 action, so bail
		}
	}

// ****************************************************************************
//
//  Function Name:	RUndoManager::DocSaved( )
//
//  Description:		remember the current undo state for our doc so we may
//							tell it when it has changed
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RUndoManager::DocSaved( )
	{
	m_fTrackingSaveCounter = TRUE;
	m_LastDocSaveUndo = GetCurrentUndoIndex();
	m_fUndoListChangedSinceDocSaved = FALSE;
	}

// ****************************************************************************
//
//  Function Name:	RUndoManager::ForceDocDirty( )
//
//  Description:		Force the doc to be dirty.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RUndoManager::ForceDocDirty( )
	{
	//	Tell the undo manager that an undoable action has occurred.
	m_fUndoListChangedSinceDocSaved = TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RUndoManager::DocDirty( )
//
//  Description:		return whether the doc is dirty or not
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RUndoManager::DocDirty( )
	{
	//	the doc is dirty if we have invalidated the m_LastDocSaveUndo by removing the action it refers to from the undo list
	//	or the  user has added or removed an undoable action to/from the undo list since the last save
	YCounter ycount = GetCurrentUndoIndex();

	if ( m_fTrackingSaveCounter )
		return static_cast<BOOLEAN>( m_fUndoListChangedSinceDocSaved || (ycount != m_LastDocSaveUndo) );
	else
		return static_cast<BOOLEAN>( m_fUndoListChangedSinceDocSaved );
	}

// ****************************************************************************
//
//  Function Name:	RUndoManager::FreeAllActions( )
//
//  Description:		Remove all actions from the list and deletes them
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RUndoManager::FreeAllActions( )
	{
	FreeUndoneActions( );

	//	Only free actions if there are any actions left.
	if (m_UndoList.Count() > 0)
		{
		RUndoListIterator	iterator;
		while ( (iterator = m_UndoList.Start()) != m_UndoList.End() )
			{
			RUndoableAction*	pAction = GetAndValidateAction( iterator, TRUE );
			m_UndoList.RemoveAt( iterator );
			delete pAction;
			}
		}
	
	if ( m_fTrackingSaveCounter && (0 != m_LastDocSaveUndo) )
		{
		//	we removed all actions so the doc is definitely dirty
		m_fTrackingSaveCounter = FALSE;
		m_fUndoListChangedSinceDocSaved = kUndoListTruncatedSoDocIsDirty;
		}

	// Reset the state
	m_LastDocSaveUndo = 0;
	m_CurrentUndo = m_UndoList.End();
	}

// ****************************************************************************
//
//  Function Name:	RUndoManager::FreeUndoneActions( )
//
//  Description:		Remove all actions from the undone section of the list
//							and deletes them
//
//  Returns:			whether freeing actions made the doc dirty
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RUndoManager::FreeUndoneActions( )
	{
	//	return the current state of the doc dirty flag if we don't change it below
	BOOLEAN fDocMadeDirty = m_fUndoListChangedSinceDocSaved;

	if (CountUndoableActions( ) > 0)
		{
		RUndoListIterator	iterator = m_UndoList.End();
		
		while ( --iterator != m_CurrentUndo )
			{
			RUndoableAction*	pAction = GetAndValidateAction( iterator, FALSE );
			m_UndoList.RemoveAt( iterator );
			delete pAction;

			iterator = m_UndoList.End();
			}

		//	if we're checking whether our saved doc has been made dirty
		//	and the last undoable action before the last save is no longer on the
		// list then the doc is dirty
		if ( m_fTrackingSaveCounter && (GetCurrentUndoIndex() < m_LastDocSaveUndo) )
			{
			m_fTrackingSaveCounter = FALSE;
			fDocMadeDirty = kUndoListTruncatedSoDocIsDirty;
			}
		}
	
	return fDocMadeDirty;
	}

// ****************************************************************************
//
//  Function Name:	RUndoManager::GetCurrentUndoIndex( ) const
//
//  Description:		Returns the undo list index of the m_CurrentUndo iterator
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YCounter RUndoManager::GetCurrentUndoIndex( ) const
	{
	//	when there are no actions on the undo list the index must be 0
	//	otherwise count the number of actions from the start of the list to the current
	//	undo and add one because the start list entry is not counted by the Count fcn
	YCounter yListCount = m_UndoList.Count();
	if ( (0 != yListCount) && (m_UndoList.End() != m_CurrentUndo) )
		return Count( m_UndoList.Start(), m_CurrentUndo ) + 1;
//		return Count( m_CurrentUndo, m_UndoList.End() ) + 1;
	else 
		return 0;
	}

// ****************************************************************************
//
//  Function Name:	RUndoManager::GetUndoStringId( )
//
//  Description:		Returns the ID of the String to use for the Undo Menu item
//							returns a default value if no items are undoable
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
uWORD RUndoManager::GetUndoStringId( ) const
	{
	RUndoListIterator	iterator;
	for (iterator = m_CurrentUndo; iterator != m_UndoList.End(); --iterator )
		{
		RUndoableAction* pAction = GetAndValidateAction( iterator, TRUE );
		if ( !pAction->IsTransparent() )
			return pAction->GetUndoStringId();
		}
	return STRING_CANT_UNDO;
	}

// ****************************************************************************
//
//  Function Name:	RUndoManager::GetRedoStringId( )
//
//  Description:		Returns the ID of the String to use for the Redo Menu item
//							returns a default value if no items are undoable
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
uWORD RUndoManager::GetRedoStringId( ) const
	{
	RUndoListIterator	iterator		= m_CurrentUndo;
	RUndoListIterator	iteratorEnd	= m_UndoList.End();
	--iteratorEnd;
	while (iterator != iteratorEnd)
		{
		RUndoableAction* pAction = GetAndValidateAction( ++iterator, FALSE );
		if ( !pAction->IsTransparent( ) )
			return pAction->GetRedoStringId();
		}
	return STRING_CANT_REDO;
	}


// ****************************************************************************
//
//  Function Name:	RUndoManager::IsNotTransparent( RUndoableAction* pAction )
//
//  Description:		Return TRUE if the given action is not transparent
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN	IsNotTransparent( RUndoableAction* pAction)
	{
	return static_cast<BOOLEAN>( !pAction->IsTransparent() );
	}

// ****************************************************************************
//
//  Function Name:	RUndoManager::CountUndoableActions( )
//
//  Description:		Return the number of actions that are not transparent
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YCounter RUndoManager::CountUndoableActions( ) const
	{
	return CountIf( m_UndoList.Start(), m_UndoList.End(), IsNotTransparent );
	}

// ****************************************************************************
//
//  Function Name:	RUndoManager::GetRedoStringId( )
//
//  Description:		Returns the ID of the String to use for the Redo Menu item
//							returns a default value if no items are undoable
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RUndoManager::SetMaxUndoableActions( YCounter newMax )
	{
	TpsAssert( newMax >= 1, "Invalid Maximum UndoableAction value" );

	//	If the new max is less than the current list size,
	//	I must adjust the current undo list.
	if (newMax <= CountUndoableActions( ))
		{
		//	First purge the Undone actions.
		FreeUndoneActions( );
		
		//	if we are still too high, start to free done actions
		while (newMax <= CountUndoableActions( ))
			{
			RUndoListIterator	iterator = m_UndoList.Start();
			RUndoableAction*	pAction = GetAndValidateAction( iterator, TRUE );
			m_UndoList.RemoveAt( iterator );
			delete pAction;
			}
		}

	//	Set the maximum to the given value
	m_MaximumUndos = newMax;
	}

// ****************************************************************************
//
//  Function Name:	GetUndoStringAndId( RMBCString& menuString )
//
//  Description:		return the stringid and menu string for the current undoable
//							action.
//
//  Returns:			^
//
//  Exceptions:		None
//
// ****************************************************************************
//
YResourceId RUndoManager::GetUndoStringAndId( RMBCString& menuString ) const
	{
	YResourceId	stringId;
	stringId = GetUndoStringId();
	TpsAssert( stringId != 0, "Undo StringId is not defined. Please go define it." );
	menuString = GetResourceManager().GetResourceString(stringId);
	if (menuString.IsEmpty())
		{
		stringId = STRING_CANT_UNDO;
		menuString = GetResourceManager().GetResourceString(stringId);
		}
	
	return stringId;
	}

// ****************************************************************************
//
//  Function Name:	RUndoManager::GetNextActionId( )
//
//  Description:		Get the id of the action that will be undone next
//
//  Returns:			The id
//
//  Exceptions:		None
//
// ****************************************************************************
//
const YActionId& RUndoManager::GetNextActionId( ) const
	{
	static const YActionId emptyId;

	if( m_CurrentUndo == m_UndoList.End( ) )
		return emptyId;

	RUndoableAction*	pAction = GetAndValidateAction( m_CurrentUndo, TRUE );
	return pAction->GetActionId( );
	}

// ****************************************************************************
//
//  Function Name:	RUndoManager::GetNextActionToBeUndone( )
//
//  Description:		Get action that will be undone next
//
//  Returns:			The id
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUndoableAction* RUndoManager::GetNextActionToBeUndone( ) const
	{
	RUndoListIterator	iterator;
	for (iterator = m_CurrentUndo; iterator != m_UndoList.End(); --iterator )
		{
		RUndoableAction* pAction = GetAndValidateAction( iterator, TRUE );
		if ( !pAction->IsTransparent() )
			return pAction;
		}
	return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RUndoManager::GetNextActionToBeRedone( )
//
//  Description:		Get action that will be redone next
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUndoableAction* RUndoManager::GetNextActionToBeRedone( ) const
	{
	RUndoListIterator	iterator		= m_CurrentUndo;
	RUndoListIterator	iteratorEnd	= m_UndoList.End();
	--iteratorEnd;
	while (iterator != iteratorEnd)
		{
		RUndoableAction* pAction = GetAndValidateAction( ++iterator, FALSE );
		if ( !pAction->IsTransparent( ) )
			return pAction;
		}
	return NULL;
	}

// ****************************************************************************
//
//  Function Name:	RUndoManager::OnUpdateUndo( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RUndoManager::OnUpdateUndo( RCommandUI& commandUI ) const
	{
	RMBCString menuString;

	YResourceId stringId = GetUndoStringAndId( menuString );
	TpsAssert( !menuString.IsEmpty(), "Can not find undo string." );

	commandUI.SetString( menuString );

	if (stringId == STRING_CANT_UNDO)
		commandUI.Disable( );
	else
		commandUI.Enable( );
	}

// ****************************************************************************
//
//  Function Name:	RUndoManager::OnUpdateRedo( )
//
//  Description:		Command updater
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RUndoManager::OnUpdateRedo( RCommandUI& commandUI ) const
	{
	YResourceId	stringId = GetRedoStringId();
	TpsAssert( stringId != 0, "Redo StringId is not defined. Please go define it." );
	RMBCString menuString = GetResourceManager().GetResourceString(stringId);
	if (menuString.IsEmpty())
		{
		stringId = STRING_CANT_REDO;
		menuString = GetResourceManager().GetResourceString(stringId);
		}
	TpsAssert( !menuString.IsEmpty(), "Can not find redo string." );
	commandUI.SetString( menuString );

	if (stringId == STRING_CANT_REDO)
		commandUI.Disable( );
	else
		commandUI.Enable( );
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RUndoManager::Validate( )
//
//  Description:		Verify that the UndoManager is in a coherent state
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RUndoManager::Validate( ) const
	{
	RObject::Validate( );
	TpsAssert( m_MaximumUndos >= 1, "Maximum Undos has been set to an invalid value" );
	}

#endif
