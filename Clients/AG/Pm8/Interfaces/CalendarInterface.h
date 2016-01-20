// ****************************************************************************
//
//  File Name:			CalendarInterface.h
//
//  Project:			Renaissance Application 
//
//  Author:				Michael Houle
//
//  Description:		Declaration of the RCalendarInterface class
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
//  $Logfile:: /PM8/Interfaces/CalendarInterface.h                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:22p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_CALENDARINTERFACE_H_
#define		_CALENDARINTERFACE_H_

#include "Interface.h"

const	YInterfaceId kCalendarInterfaceId = 100;

const uLONG	kCellFlagCircleHeader	= 0x80000000L;

class RGrid;
class RCellContents;
enum  EGridItem;

// ****************************************************************************
//
//  Class Name:	RCalendarInterface
//
//  Description:	Calendar interface
//
// ****************************************************************************
//
class RCalendarInterface : public RInterface
	{
	//	Construction
	public :
													RCalendarInterface( const RComponentView* pView ) : RInterface( pView ) { ; }

	// Operations
	public :
		//	Let the calendar set its attributes.
		virtual void							SetGridAttributes( ) = 0;

		//	To be able to handle the edit cell dialogs
		virtual RGrid*							GetGrid( ) = 0;
		virtual EGridItem						GetSelection( int& nCell ) = 0;
		virtual RCellContents*				GetCellContents( int nCell ) = 0;
		virtual void							SetCellContents( int nCell, RCellContents* pNewContents ) = 0;
	};

#endif	// _CALENDARINTERFACE_H_
