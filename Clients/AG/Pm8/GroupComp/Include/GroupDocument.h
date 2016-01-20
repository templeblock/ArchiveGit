// ****************************************************************************
//
//  File Name:			GroupDocument.h
//
//  Project:			Group Application
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RGroupDocument class
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
//  $Logfile:: /PM8/GroupComp/Include/GroupDocument.h                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_GROUPDOCUMENT_H_
#define		_GROUPDOCUMENT_H_

#include "ComponentDocument.h"

// ****************************************************************************
//
//  Class Name:	RGroupDocument
//
//  Description:	The Group document.
//
// ****************************************************************************
//
class RGroupDocument : public RComponentDocument
	{
	// Construction & Destruction
	public :
												RGroupDocument( RApplication* pApp,
																	 const RComponentAttributes& componentAttributes,
																	 const YComponentType& componentType,
																	 BOOLEAN fLoading );

	// Operations
	public :
		virtual RComponentView*			CreateView( const YComponentBoundingRect& boundingRect, RView* pParentView );
	
// Debugging support
#ifdef	TPSDEBUG
	public :
		virtual void						Validate( ) const;
#endif
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

#endif		// _GROUPDOCUMENT_H_
