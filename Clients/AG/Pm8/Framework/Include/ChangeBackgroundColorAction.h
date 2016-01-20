// ****************************************************************************
//
//  File Name:			ChangeBackgroundColorAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				Eric VanHelene
//
//  Description:		Declaration of the RChangeBackgroundColorAction class
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
//  $Logfile:: /PM8/Framework/Include/ChangeBackgroundColorAction.h           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_CHANGEBACKGROUNDCOLORACTION_H_
#define		_CHANGEBACKGROUNDCOLORACTION_H_

#ifndef		_UNDOABLEACTION_H_
#include		"UndoableAction.h"
#endif

#ifndef		_COMPOSITESELECTION_H_
#include		"CompositeSelection.h"
#endif

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RPaneView;

class RColorList: public RList<RColor> {};

// ****************************************************************************
//
//  Class Name:		RChangeBackgroundColorAction
//
//  Description:		Changes the background color of a pane
//
// ****************************************************************************
class FrameworkLinkage RChangeBackgroundColorAction : public RUndoableAction
{
	// Construction & destruction
	public:
													RChangeBackgroundColorAction(const RColor& rColor );
													RChangeBackgroundColorAction( RScript& script );
		virtual									~RChangeBackgroundColorAction( );

	//	Identifier
	public :
		static YActionId						m_ActionId;

	// Operations
	public:
		virtual BOOLEAN						Do();
		virtual void							Undo();
		virtual void							Redo( );

	// Scripting Operations
	public :
		virtual BOOLEAN						WriteScript( RScript& script ) const;

	// Implementation
	protected :
		void										SetColor( RPaneView* pView, const RColor& color );
		void										SaveOldColor( const RColor& color );

	//	Member Data
	protected:
		RColorList								m_OldColors;					//	list of offsets used to align components to their aligned positions
		RColor									m_Color;							//	new color for selection

#ifdef TPSDEBUG
	//	Debugging code
	public:
		virtual void Validate() const;
#endif
};

// ****************************************************************************
//
//  Class Name:		RChangeSelectionBackgroundColorAction
//
//  Description:		Changes the color of a selection of components
//
// ****************************************************************************
class FrameworkLinkage RChangeSelectionBackgroundColorAction : public RChangeBackgroundColorAction
	{
	// Construction & destruction
	public:
													RChangeSelectionBackgroundColorAction( RCompositeSelection* pCurrentSelection, const RColor& rColor );
													RChangeSelectionBackgroundColorAction( RCompositeSelection* pCurrentSelection, RScript& script );

	// Operations
	public:
		virtual BOOLEAN						Do( );
		virtual void							Undo( );
		virtual void							Redo( );

	// Scripting Operations
	public :
		virtual BOOLEAN						WriteScript( RScript& script ) const;

	//	Private Member Data
	private:
		RCompositeSelection*					m_pCurrentSelection;
		RCompositeSelection					m_OldSelection;
	};

// ****************************************************************************
//
//  Class Name:		RChangePaneViewBackgroundColorAction
//
//  Description:		Changes the color of a paneview
//
// ****************************************************************************
class FrameworkLinkage RChangePaneViewBackgroundColorAction : public RChangeBackgroundColorAction
	{
	// Construction & destruction
	public:
													RChangePaneViewBackgroundColorAction( RPaneView* pView, const RColor& rColor );
													RChangePaneViewBackgroundColorAction( RPaneView* pView, RScript& script );

	// Operations
	public:
		virtual BOOLEAN						Do( );
		virtual void							Undo( );
		virtual void							Redo( );

	//	Private Member Data
	private:
		RPaneView*								m_pView;
	};


#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_CHANGEBACKGROUNDCOLORACTION_H_
