// ****************************************************************************
//
//  File Name:			GroupApplication.h
//
//  Project:			Group Application
//
//  Author:				Renaissance Component AppWizard
//
//  Description:		Declaration of the RGroupApplication class
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
//  $Logfile:: /PM8/GroupComp/Include/GroupApplication.h                      $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_GROUPAPPLICATION_H_
#define		_GROUPAPPLICATION_H_

#include "ComponentApplication.h"

class RComponentDocument;

// ****************************************************************************
//
//  Class Name:	RGroupApplication
//
//  Description:	The Group Application.
//
// ****************************************************************************
//
class RGroupApplication : public RComponentApplication
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

#endif		// _GROUPAPPLICATION_H_
