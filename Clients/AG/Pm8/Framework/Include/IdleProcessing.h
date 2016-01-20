// ****************************************************************************
//
//  File Name:			IdleProcessing.h
//
//  Project:			Renaissance Application Framework
//
//  Author:				E. VanHelene
//
//  Description:		Declaration of the RApplication class
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
//  $Logfile:: /PM8/Framework/Include/IdleProcessing.h                        $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_IDLEPROCESSING_H_
#define		_IDLEPROCESSING_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//	we will display memory exceptions periodically not every time we see one
//	remember the last time one was seen and how long to wait in between
//	reporting them ( ten seconds )
const uLONG kMemoryTimeoutMillSecs = 10 * 1000;

class RIdleInfo
	{
	// Constructor
	public:
												RIdleInfo( );

	// Member data
	public :
		BOOLEAN								m_fMemoryLowWarningNeeded;
		YTickCount							m_yLastMemExceptionTime;
								
		BOOLEAN								m_fResourceLowWarningNeeded;
		YTickCount							m_yLastResourceExceptionTime;
	};

// ****************************************************************************
// 					Inlines
// ****************************************************************************

inline RIdleInfo::RIdleInfo( )
	: m_fMemoryLowWarningNeeded( FALSE ),
	  m_yLastMemExceptionTime( 0 ),
	  m_fResourceLowWarningNeeded( FALSE ),
	  m_yLastResourceExceptionTime( 0 )
	{
	NULL;
	}

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif		//	_IDLEPROCESSING_H_