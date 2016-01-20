// ****************************************************************************
//
//  File Name:			ReplaceImageAction.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				John Fleischhauer
//
//  Description:		Declaration of the RReplaceImageAction class
//
//  Portability:		Platform independent
//
//  Developed by:		Broderbund Software, Inc.
//
//  Copyright (C) 1998 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#ifndef		_REPLACEIMAGEACTION_H_
#define		_REPLACEIMAGEACTION_H_

#include	"UndoableAction.h"
#include "View.h"
#include "CompositeSelection.h"

class RComponentDocument;
class RClippingPath;

// ****************************************************************************
//
//  Class Name:	RReplaceImageAction
//
//  Description:	Action which replaces the image in a component with another image
//
// ****************************************************************************
//
class FrameworkLinkage RReplaceImageAction : public RUndoableAction
{
	// Construction, destruction & Intialization
	public :
										RReplaceImageAction( RCompositeSelection* pCurrentSelection,
																	RComponentDocument* pOldComponent,
																	RComponentDocument* pNewComponent,
																	YResourceId undoRedoPairId );

		virtual						~RReplaceImageAction();

	// Operations
		virtual BOOLEAN			Do();
		virtual void				Undo();

	// Scripting Operations
		virtual BOOLEAN			WriteScript( RScript& script ) const;

	//	Identifier
		static YActionId			m_ActionId;

	// Member data
	protected :
		RCompositeSelection*		m_pCurrentSelection;
		RComponentDocument*		m_pComponent;

		RBitmapImage*				m_pReplacementImage;
		RClippingPath*				m_pReplacementClippingPath;
		YComponentBoundingRect	m_rReplacementBoundingRect;

		RBitmapImage*				m_pOldImage;
		RClippingPath*				m_pOldClippingPath;
		YComponentBoundingRect	m_rOldBoundingRect;

		RPath*						m_pFramePath;
		BOOLEAN						m_bMustCalculateBoundingRect;

		BOOLEAN						Rotated0or180(	RRealSize& rOldImageSize,
															RRealSize& rNewImageSize,
															RRealRect& rOldCropArea,
															RRealRect& rNewCropArea );

		BOOLEAN						Rotated90or270(	RRealSize& rOldImageSize,
																RRealSize& rNewImageSize,
																RRealRect& rOldCropArea,
																RRealRect& rNewCropArea );

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void				Validate() const;
#endif
	};

#endif		// _REPLACEIMAGEACTION_H_
