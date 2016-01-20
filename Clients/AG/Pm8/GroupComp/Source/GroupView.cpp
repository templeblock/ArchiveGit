// ****************************************************************************
//
//  File Name:			GroupView.cpp
//
//  Project:			Group Component
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RGroupView class
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
//  $Logfile:: /PM8/GroupComp/Source/GroupView.cpp                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"GroupIncludes.h"

ASSERTNAME

#include	"GroupView.h"
#include	"GroupDocument.h"

#include "RenaissanceResource.h"

// ****************************************************************************
//
//  Function Name:	RGraphicView::RGraphicView( )
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RGroupView::RGroupView( const YComponentBoundingRect& boundingRect, RComponentDocument* pDocument, RView* pParentView )
	: RComponentView( boundingRect, pDocument, pParentView )
	{
	;
	}

// ****************************************************************************
//
//  Function Name:	RGroupView::OnXEditComponent( )
//
//  Description:		Called to edit this component
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGroupView::OnXEditComponent( EActivationMethod, const RRealPoint& )
	{
	;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RGroupView::Validate( )
//
//  Description:		Validate the object.
//							
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RGroupView::Validate( ) const
	{
	RComponentView::Validate( );
	TpsAssertIsObject( RGroupView, this );
	}

#endif	//	TPSDEBUG

