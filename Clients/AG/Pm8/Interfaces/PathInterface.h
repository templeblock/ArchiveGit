// ****************************************************************************
//
//  File Name:			PathInterface.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RPathInterface class
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
//  $Logfile:: /PM8/Interfaces/PathInterface.h                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_PATHINTERFACE_H_
#define		_PATHINTERFACE_H_

#ifndef		_COMPONENTDATAINTERFACE_H_
#include		"ComponentDataInterface.h"
#endif

const	YInterfaceId	kPathInterfaceId				= 130;

// ****************************************************************************
//
//  Class Name:	RPathHolder
//
//  Description:	A Pure virtual class for Holding the data of a data interface
//
// ****************************************************************************
//
class RPathHolder : public RComponentDataHolder
{
//	Construction and Destruction
public :
									RPathHolder( ) { ; };
	virtual						~RPathHolder( ) { ; };
} ;

// ****************************************************************************
//
//  Class Name:	IPathInterface
//
//  Description:	A pure virtual class interfacing with a path component
//
// ****************************************************************************
//
class IPathInterface : public RComponentDataInterface
{
//	Construction & Destruction
public :
									IPathInterface( const RComponentView* pView ) : RComponentDataInterface( pView ) { ; }

public:

	virtual RPath*				GetPath( ) = 0;
	virtual void				SetPath( const RPath& rPath, BOOLEAN fResize = TRUE ) = 0;
};



#endif	//	_PATHINTERFACE_H_
