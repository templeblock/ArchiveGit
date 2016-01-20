// ****************************************************************************
//
//  File Name:			BorderInterface.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RBorderInterface class
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
//  $Logfile:: /PM8/Interfaces/BorderInterface.h                              $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_BORDERINTERFACE_H_
#define		_BORDERINTERFACE_H_

#include "Interface.h"

const	YInterfaceId kBorderInterfaceId = 150;

// ****************************************************************************
//
//  Class Name:	RBorderInterface
//
//  Description:	Border interface
//
// ****************************************************************************
//
class RBorderInterface : public RInterface
	{
	//	Construction
	public :
													RBorderInterface( const RComponentView* pView ) : RInterface( pView ) { ; }

	// Operations
	public :
		virtual void							SetBorderThickness( BOOLEAN fWide ) = 0;
	};

#endif	// _BORDERINTERFACE_H_
