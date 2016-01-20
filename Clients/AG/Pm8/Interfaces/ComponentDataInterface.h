// ****************************************************************************
//
//  File Name:			ComponentDataInterface.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RComponentDataInterface class
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
//  $Logfile:: /PM8/Interfaces/ComponentDataInterface.h                       $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_COMPONENTDATAINTERFACE_H_
#define		_COMPONENTDATAINTERFACE_H_

#ifndef		_INTERFACE_H_
#include		"Interface.h"
#endif

class RStorage;
class RChunkStorage;
class RDrawingSurface;

// ****************************************************************************
//
//  Class Name:	RComponentDataHolder
//
//  Description:	A Pure virtual class for Holding the data of a data interface
//
// ****************************************************************************
//
class RComponentDataHolder : public RObject
	{
	//	Construction and Destruction
	public :
													RComponentDataHolder( ) { ; };
		virtual									~RComponentDataHolder( ) { ; };
	} ;

// ****************************************************************************
//
//  Class Name:	RComponentDataInterface
//
//  Description:	A pure virtual class interfacing with a text component
//
// ****************************************************************************
//
class RComponentDataInterface : public RInterface
	{
	//	Construction & Destruction
	public :
													RComponentDataInterface( const RComponentView* pView ) : RInterface( pView ) { ; }

	//	Operations
	public :
		virtual RComponentDataHolder*		GetData( ) = 0;
		virtual void							EditData( ) = 0;
		virtual void							SetData( RComponentDataHolder* ) = 0;
		virtual void							ClearData( ) = 0;
		virtual void							Read( RChunkStorage& ) = 0;
		virtual void							Write( RChunkStorage& ) = 0;
		virtual void							Render( RDrawingSurface& , const R2dTransform&, const RIntRect&, const RIntRect& ) = 0;

	//	Private Members
	private :
	} ;

#endif	//	_COMPONENTDATAINTERFACE_H_
