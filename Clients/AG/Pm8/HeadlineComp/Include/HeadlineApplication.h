// ****************************************************************************
//
//  File Name:			HeadlineApplication.h
//
//  Project:			Headline Application
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RHeadlineApplication class
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
//  $Logfile:: /PM8/HeadlineComp/Include/HeadlineApplication.h                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_HEADLINEAPPLICATION_H_
#define		_HEADLINEAPPLICATION_H_

#include "ComponentApplication.h"

class RComponentDocument;

// ****************************************************************************
//
//  Class Name:	RHeadlineApplication
//
//  Description:	The Headline Application.
//
// ****************************************************************************
//
class RHeadlineApplication : public RComponentApplication
	{
	// Component overrides
	public :
		virtual void						GetSupportedComponentTypes( RComponentTypeCollection& componentTypeCollection ) const;
		virtual RComponentDocument*	CreateNewDocument( const RComponentAttributes& componentAttributes, const YComponentType& componentType, BOOLEAN fLoading );

#ifdef	TPSDEBUG
	// Debugging support
	public :
		virtual void						Validate( ) const;
#endif
	};

#endif		// _HEADLINEAPPLICATION_H_
