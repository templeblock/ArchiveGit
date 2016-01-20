// ****************************************************************************
//
//  File Name:			BorderInterfaceImp.h
//
//  Project:			Renaissance Graphic Component
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RBorderInterfaceImp class
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
//  $Logfile:: /PM8/GraphicComp/Include/BorderInterfaceImp.h                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:18p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_BORDERINTERFACEIMP_H_
#define		_BORDERINTERFACEIMP_H_

#include "BorderInterface.h"

class RBorderView;
class RBorderDocument;

// ****************************************************************************
//
//  Class Name:	RBorderInterfaceImp
//
//  Description:	Implementation of the border interface
//
// ****************************************************************************
//
class RBorderInterfaceImp : public RBorderInterface
	{
	//	Construction
	public :
													RBorderInterfaceImp( const RBorderView* pView );
		static RInterface*					CreateInterface( const RComponentView* pView );

	// Operations
	public :
		void										SetBorderThickness( BOOLEAN fWide );

	// Members
	private :
		const RBorderView*					m_pBorderView;
	};

#endif	// _BORDERINTERFACEIMP_H_
