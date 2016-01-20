// ****************************************************************************
//
//  File Name:			GroupView.h
//
//  Project:			Group Application
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
//  $Logfile:: /PM8/GroupComp/Include/GroupView.h                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_GROUPVIEW_H_
#define		_GROUPVIEW_H_

#include "ComponentView.h"

// ****************************************************************************
//
//  Class Name:	RGroupView
//
//  Description:	The Group view class. This class handles various xEvents,
//						and dispatches various xActions.
//
// ****************************************************************************
//
class RGroupView : public RComponentView
	{
	// Construction & Destruction
	public :
												RGroupView( const YComponentBoundingRect& boundingRect,
															   RComponentDocument* pDocument,
																RView* pParentView );

	// Operations
	protected :
		virtual void						OnXEditComponent( EActivationMethod activationMethod, const RRealPoint& mousePoint );

// Debugging stuff
#ifdef	TPSDEBUG
	public :
		virtual void						Validate( ) const;
#endif
	} ;

#endif	//	_GROUPVIEW_H_
