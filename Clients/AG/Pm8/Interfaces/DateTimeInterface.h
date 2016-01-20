// ****************************************************************************
//
//  File Name:			DateTimeInterface.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RDateTimeInterface class
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
//  $Logfile:: /PM8/Interfaces/DateTimeInterface.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_DATETIMEINTERFACE_H_
#define		_DATETIMEINTERFACE_H_

#ifndef	_INTERFACE_H_
#include "Interface.h"
#endif

const YInterfaceId	kDateTimeInterfaceId				= 5;

//	Forward references for DateTimeInterface
class RDateTime;

// ****************************************************************************
//
//  Class Name:	RDateTimeInterface
//
//  Description:	The base class for letting calendars Get/Set the date
//
// ****************************************************************************
//
class RDateTimeInterface : public RInterface
	{
	//	Construction and Destruction
	public :
		RDateTimeInterface( const RComponentView* pView ) : RInterface( pView ) { ; }

	//	Operations
	public :
		virtual const RDateTime&				GetDateTime( ) const = 0;
		virtual void								SetDateTime( const RDateTime&, BOOLEAN ) = 0;
		virtual YResourceId						GetDateFormatId( ) const = 0;
	};

#endif	//	_DATETIMEINTERFACE_H_
