// ****************************************************************************
//
//  File Name:			Timer.cpp
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Implementation of the RTimer class
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
//  $Logfile:: /PM8/Framework/Source/Timer.cpp                                $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include	"Timer.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

RTimer::YTimerCollection RTimer::m_TimerCollection;

// ****************************************************************************
//
//  Function Name:	RTimer::RTimer
//
//  Description:		Constructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RTimer::RTimer( uLONG timeOut )
	{
	// Create a new timer and add it to the timer collection
	RTimerInfo timerInfo;
	timerInfo.m_pTimer = this;
	timerInfo.m_TimerId = ::SetTimer( NULL, 0, timeOut, TimerProc );
	m_TimerCollection.InsertAtEnd( timerInfo );
	}

// ****************************************************************************
//
//  Function Name:	RTimer::~RTimer
//
//  Description:		Destructor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RTimer::~RTimer( )
	{
	// Find the timer and kill it
	YTimerIterator iterator = m_TimerCollection.Start( );
	for( ; iterator != m_TimerCollection.End( ); ++iterator )
		{
		if( ( *iterator ).m_pTimer == this )
			{
			// Save the timer id
			YTimerId timerId = ( *iterator ).m_TimerId;

			// Remove the timer from the list
			m_TimerCollection.RemoveAt( iterator );

			// Kill the timer
			::KillTimer( NULL, timerId );

			return;
			}
		}

	TpsAssertAlways( "Timer id not found." );
	}

// ****************************************************************************
//
//  Function Name:	RTimer::TimerProc
//
//  Description:		Call back for Windows timers
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
VOID CALLBACK RTimer::TimerProc( HWND, UINT, UINT idEvent, DWORD )
	{
	// Find the timer and call its OnTimer function
	YTimerIterator iterator = m_TimerCollection.Start( );
	for( ; iterator != m_TimerCollection.End( ); ++iterator )
		{
		if( ( *iterator ).m_TimerId == idEvent )
			{
			( *iterator ).m_pTimer->OnTimer( );
			return;
			}
		}

	TpsAssertAlways( "Timer id not found." );
	}
