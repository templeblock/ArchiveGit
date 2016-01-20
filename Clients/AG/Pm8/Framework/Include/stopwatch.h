//****************************************************************************
//
// File Name:		StopWatch.cpp
//
// Project:			Renaissance application framework
//
// Author:			Mike Houle
//
// Description:	Source code for RStopWatch
//
// Portability:	Windows Specific
//
// Developed by:  Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/stopwatch.h                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//****************************************************************************

#ifndef	_STOPWATCH_H_
#define _STOPWATCH_H_

class RStopWatch
{
	public :
		RStopWatch( LPSZ pszText );
		~RStopWatch( );

		void TimeStamp( LPSZ pszText );

	public :
		uLONG	m_ulTickCount;
		CString m_szText;
} ;

inline RStopWatch::RStopWatch( LPSZ pszText )
{
	CString	string;
	m_szText=pszText;
	string.Format("\nStart to time: %s\n", pszText );
	OutputDebugString(string);
	m_ulTickCount	= RenaissanceGetTickCount( );
}
inline RStopWatch::~RStopWatch( )
{
	CString	string;
	string.Format("\nEnd of time: %s", m_szText );
	TimeStamp( (LPSZ)((LPCTSTR)string) );
}
inline void	RStopWatch::TimeStamp( LPSZ pszText )
{
	uLONG ulTickCount	= RenaissanceGetTickCount( );
	CString	string;
	string.Format("%s - %ld\n", pszText, ulTickCount - m_ulTickCount );
	OutputDebugString(string);
}

#endif	//	STOPWATCH_H_

