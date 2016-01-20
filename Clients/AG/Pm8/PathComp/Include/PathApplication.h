// ****************************************************************************
//
//  File Name:			PathApplication.h
//
//  Project:			Graphic Application
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RPathApplication class
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
//  $Logfile:: /PM8/PathComp/Include/PathApplication.h                     $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:23p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PATHAPPLICATION_H_
#define		_PATHAPPLICATION_H_

#include "ComponentApplication.h"

class RComponentDocument;

// ****************************************************************************
//
//  Class Name:	RPathApplication
//
//  Description:	The Path Application.
//
// ****************************************************************************
//
class RPathApplication : public RComponentApplication
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

#endif		// _PATHAPPLICATION_H_
