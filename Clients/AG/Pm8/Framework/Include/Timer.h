// ****************************************************************************
//
//  File Name:			Timer.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				S. Athanas
//
//  Description:		Declaration of the RTimer class
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
//  $Logfile:: /PM8/Framework/Include/Timer.h                                 $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef	_TIMER_H_
#define	_TIMER_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

// ****************************************************************************
//
//  Class Name:		RTimer
//
//  Description:		Base class for timers
//
// ****************************************************************************
//
class FrameworkLinkage RTimer
	{
	// Construction & destruction
	public :
												RTimer( uLONG timeOut );
		virtual								~RTimer( );

	// Timer callback
	private :
		virtual void						OnTimer( ) = 0;

#ifdef	_WINDOWS
	// Implementation
	private :
		typedef UINT YTimerId;

		struct RTimerInfo
			{
			YTimerId							m_TimerId;
			RTimer*							m_pTimer;
			};

		typedef RList<RTimerInfo> YTimerCollection;
		typedef YTimerCollection::YIterator YTimerIterator;

		static VOID CALLBACK				TimerProc( HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime );
		static YTimerCollection			m_TimerCollection;
#endif	// _WINDOWS
	};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	// _TIMER_H_