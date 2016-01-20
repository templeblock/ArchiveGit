// ****************************************************************************
//
//  File Name:			DuplicateSelectionAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RDuplicateSelectionAction class
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
//  $Logfile:: /PM8/Framework/Include/DuplicateSelectionAction.h              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_DUPLICATESELCTIONACTION_H_
#define		_DUPLICATESELCTIONACTION_H_

#include "PasteAction.h"

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:	RDuplicateSelectionAction
//
//  Description:	Duplicates the current selection
//
// ****************************************************************************
//
class FrameworkLinkage RDuplicateSelectionAction : public RPasteActionBase
	{
	// Construction, destruction & Intialization
	public :
												RDuplicateSelectionAction( RCompositeSelection* pCurrentSelection );
												RDuplicateSelectionAction( RCompositeSelection* pCurrentSelection, RScript& script );

	// Initializer
	protected :
		void									Initialize( );

	//	Identifier
	public :
		static YActionId					m_ActionId;
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_DUPLICATESELCTIONACTION_H_
