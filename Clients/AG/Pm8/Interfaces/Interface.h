// ****************************************************************************
//
//  File Name:			Interface.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RInterface class
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
//  $Logfile:: /PM8/Interfaces/Interface.h                                    $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_INTERFACE_H_
#define		_INTERFACE_H_

const	YInterfaceId	kInvalidInterfaceId	= 0;
const	YInterfaceId	kUnknownInterfaceId	= 100;

class RInterface;
class RComponentView;

#ifdef	_WINDOWS
	#define	FUNCTIONCALL	
#else
	#define	FUNCTIONCALL	
#endif	//	_WINDOWS

typedef RInterface* ( FUNCTIONCALL InterfaceCreator)( const RComponentView* );
typedef	InterfaceCreator*	InterfaceCreatorProc;

struct InterfaceToCreateFunction
	{
	YInterfaceId			interfaceId;
	InterfaceCreatorProc	creator;
	} ;

class IUnknownInterface
{
public:

	virtual uLONG				AddRef( void ) = 0; 
	virtual uLONG				Release( void ) = 0; 
	virtual BOOLEAN			QueryInterface( YInterfaceId, void ** ppvObject ) = 0;
};

typedef IUnknownInterface RIUnknown;  // For backwards compatiblity

// ****************************************************************************
//
//  Class Name:	RInterface
//
//  Description:	A basic class for defining an interface for component
//						and application functionality.  It will allow for future expansion
//						as well as help with our current implementation.
//
// ****************************************************************************
//
class RInterface
	{
	//	Construction & Destruction
	public :
													RInterface( const RComponentView* )		{ ; };
		virtual									~RInterface( )									{ ; };
	} ;


#endif	//	_INTERFACE_H_
