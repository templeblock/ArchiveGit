// ****************************************************************************
//
//  File Name:			ComponentEdgeOutlineSelectionAction.cpp
//
//  Project:			Renaissance Application framework
//
//  Author:				Wynn
//
//  Description:		Definition of the RComponentEdgeOutlineSelectionAction class
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1998 Broderbund Software Inc. All Rights Reserved.
//
//	 To do:
//
//		//wb need to check if edge is outlineable if((*iterator)->GetComponentAttributes().IsEdgeOutlineable())
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"ComponentEdgeOutlineSelectionAction.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Script.h"
#include	"CompositeSelection.h"
#include "FrameworkResourceIds.h"
#include "ComponentView.h"
#include "ComponentDocument.h"

YActionId	RComponentEdgeOutlineSelectionAction::m_ActionId("RComponentEdgeOutlineSelectionAction");

// ****************************************************************************
//
//  Function Name:	RComponentEdgeOutlineSelectionAction::RComponentEdgeOutlineSelectionAction()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentEdgeOutlineSelectionAction::RComponentEdgeOutlineSelectionAction(RCompositeSelection* pCurrentSelection, const REdgeOutlineSettings& edgeOutlineSettings)
	: m_pCurrentSelection(pCurrentSelection),
	  m_OldSelection(*pCurrentSelection),
	  m_newEdgeOutlineSettings(edgeOutlineSettings),
	  RUndoableAction(m_ActionId, STRING_UNDO_ADD_EDGEOUTLINE_ON_OFF, STRING_REDO_ADD_EDGEOUTLINE_ON_OFF)
{
	// Iterate the selection, recording the EdgeOutline state of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start();
	REdgeOutlineSettings EdgeOutlineSettings;
	for(; iterator != m_pCurrentSelection->End(); ++iterator)
	{
		(*iterator)->GetEdgeOutlineSettings(EdgeOutlineSettings);
		m_oldEdgeOutlineStates.InsertAtEnd(EdgeOutlineSettings);
	}
}

// ****************************************************************************
//
//  Function Name:	RComponentEdgeOutlineSelectionAction::RComponentEdgeOutlineSelectionAction()
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RComponentEdgeOutlineSelectionAction::RComponentEdgeOutlineSelectionAction(RCompositeSelection* pCurrentSelection, RScript& script)
	: m_pCurrentSelection(pCurrentSelection),
	  m_OldSelection(*pCurrentSelection),
	  RUndoableAction(m_ActionId, script)
{
	//	Retrieve the selection and frame type
	pCurrentSelection->Read(script);

	//	Read the EdgeOutline settings
#if 0 // GCB 3/20/98 - if scriptability is needed >> will need to be written for REdgeOutlineSettings
	script >> m_newEdgeOutlineSettings;
#else
	UnimplementedCode();
#endif

	//	Reset internal values
	m_pCurrentSelection	= pCurrentSelection;
	m_OldSelection			= *pCurrentSelection;
}

// ****************************************************************************
//
//  Function Name:	RComponentEdgeOutlineSelectionAction::Do()
//
//  Description:		Does the action
//
//  Returns:			TRUE	: The action succeeded; script it
//							FALSE	: The action failed, do not script it
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentEdgeOutlineSelectionAction::Do()
{
	//	Setup the state properly
	RUndoableAction::Do();

	// Iterate the selection, changing the EdgeOutline settings of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start();
		for(; iterator != m_pCurrentSelection->End(); ++iterator)
		{
			(*iterator)->SetEdgeOutlineSettings(m_newEdgeOutlineSettings);
		}
	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews(kLayoutChanged, 0);

	// Invalidate the selection
	m_pCurrentSelection->Invalidate();

	return TRUE;
}

// ****************************************************************************
//
//  Function Name:	RComponentEdgeOutlineSelectionAction::Undo()
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentEdgeOutlineSelectionAction::Undo()
{
	//	Setup the state properly
	RUndoableAction::Undo();

	// Invalidate the current selection
	m_pCurrentSelection->Invalidate();

	// Restore the selection to the previous state
	*m_pCurrentSelection = m_OldSelection;

	// Iterate the selection, restoring the EdgeOutline state of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start();
	YEdgeOutlineStateListIterator oldEdgeOutlineStateIterator = m_oldEdgeOutlineStates.Start();

	for(; iterator != m_pCurrentSelection->End(); ++iterator, ++oldEdgeOutlineStateIterator)
	{
		TpsAssert(oldEdgeOutlineStateIterator != m_oldEdgeOutlineStates.End(), "end of old frames list encountered before end of selected components");
		(*iterator)->SetEdgeOutlineSettings((*oldEdgeOutlineStateIterator));
	}

	//	Tell the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews(kLayoutChanged, 0);

	// Invalidate the selection
	m_pCurrentSelection->Invalidate();
}

// ****************************************************************************
//
//  Function Name:	RComponentEdgeOutlineSelectionAction::Redo()
//
//  Description:		Undoes the action
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentEdgeOutlineSelectionAction::Redo()
{
	// Invalidate the old selection
	m_pCurrentSelection->Invalidate();

	// Restore the selection to the current state
	*m_pCurrentSelection = m_OldSelection;

	// Iterate the selection, changing the EdgeOutline state of each selected object
	RCompositeSelection::YIterator iterator = m_pCurrentSelection->Start();
	for(; iterator != m_pCurrentSelection->End(); ++iterator)
	{
		(*iterator)->SetEdgeOutlineSettings(m_newEdgeOutlineSettings);
	}
	//	Update the view that its layout has changed
	m_pCurrentSelection->GetView()->XUpdateAllViews(kLayoutChanged, 0);
	// Invalidate the selection
	m_pCurrentSelection->Invalidate();
	
	m_asLastAction = kActionRedo;
}

// ****************************************************************************
//
//  Function Name:	RComponentEdgeOutlineSelectionAction::WriteScript()
//
//  Description:		Writes the action to a script
//
//  Returns:			TRUE	: The action succeeded; script it
//							FALSE	: The action failed, do not script it
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RComponentEdgeOutlineSelectionAction::WriteScript(RScript& script) const
{
	if (RUndoableAction::WriteScript(script))
	{
		m_pCurrentSelection->Write(script);

#if 0 // GCB 3/20/98 - if scriptability is needed << will need to be written for REdgeOutlineSettings
		script << m_newEdgeOutlineSettings;
#else
		UnimplementedCode();
#endif
		
		return TRUE;
	}
	return FALSE;
}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RComponentEdgeOutlineSelectionAction::Validate()
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RComponentEdgeOutlineSelectionAction::Validate() const
{
	RAction::Validate();
	TpsAssertIsObject(RComponentEdgeOutlineSelectionAction, this);
	TpsAssertValid(m_pCurrentSelection);
}

#endif	//	TPSDEBUG
