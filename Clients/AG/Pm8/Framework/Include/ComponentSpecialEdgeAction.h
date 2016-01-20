// ****************************************************************************
//
//  File Name:			ComponentSpecialEdgeAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				Michael Kelson
//
//  Description:		Declaration of the RComponentSpecialEdgeAction class
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software
//
// ****************************************************************************

#ifndef		_SPECIALEDGEACTION_H_
#define		_SPECIALEDGEACTION_H_

#include		"UndoableAction.h"
#include		"CompositeSelection.h"

class RSpecialEdgeSettings
{
public:

	RBitmapImage*	pAlphaMask;
	DWORD			dwMaskID;
	int				kSpecialEdgeType;
	DWORD			kUnused;

	RSpecialEdgeSettings()
		{ pAlphaMask = NULL; dwMaskID = 0; kSpecialEdgeType = 0; kUnused = 0; }

	RSpecialEdgeSettings( RBitmapImage * p, DWORD id, int type, DWORD u=0 )
		{ pAlphaMask = p; dwMaskID = id; kSpecialEdgeType = type; kUnused = u; }
};

typedef	RList<RSpecialEdgeSettings>		YEdgeStateList;
typedef	YEdgeStateList::YIterator		YEdgeStateListIterator;

// ****************************************************************************
//
//  Class Name:		RComponentSpecialEdgeAction
//
//  Description:	Action to add a special edge to a component
//
// ****************************************************************************
//
class FrameworkLinkage RComponentSpecialEdgeAction: public RUndoableAction
{
	// Construction & Destruction
	public :
										RComponentSpecialEdgeAction( RCompositeSelection* pCurrentSelection, const RSpecialEdgeSettings& fEdgeSettings );
										RComponentSpecialEdgeAction( RCompositeSelection* pCurrentSelection, RScript& script );

	// Operations
	public :
		virtual BOOLEAN					Do( );
		virtual void					Undo( );
		virtual void					Redo( );

	// Scripting Operations
	public :
		virtual BOOLEAN					WriteScript( RScript& script ) const;

	// Member data

	//	Identifier
	public :
		static	YActionId				m_ActionId;

	private :
		RCompositeSelection*			m_pCurrentSelection;
		RCompositeSelection				m_OldSelection;

		RSpecialEdgeSettings			m_fNewEdgeSettings;
		YEdgeStateList					m_yOldEdgeStates;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void					Validate( ) const;
#endif
};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#endif		// _SPECIALEDGEACTION_H_
