/*
// $Workfile: DAY.H $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/DAY.H $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:46   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:56:14   Fred
// Initial revision.
*/

class CDay;

#ifndef __DATE_H__
#define __DATE_H__

#include "timeutil.h"

class CDay
{
public:

	// Construct for current time.
	CDay(void);
	
	// Construct for given time.
	CDay(const CTimeStamp& t);
	
	// Construct for given time.
	CDay(const FILETIME& t);
	
	// Construct for given time.
	CDay(const SYSTEMTIME& Time);
	
	// Construct for given time.
	CDay(const time_t& t);
	
	// Construct for given time.
	CDay(const tm& t);
	
	// Construct for given string.
	CDay(LPCSTR pszDay);
	
	// Return if day is valid.
	BOOL IsValid(void) const
		{ return m_fIsValid; }
		
	const CTimeStamp& GetTime(void) const
		{ return m_Time; }
		
	const CString& GetDay(void) const
		{ return m_csDay; }
		
protected:
	void CommonConstruct(const CTimeStamp& Time);
	
public:

	int operator-(const CDay& Day) const;
	BOOL operator==(const CDay& Day) const;
	BOOL operator<(const CDay& Day) const;
	
protected:
	BOOL m_fIsValid;
	CTimeStamp m_Time;
	CString m_csDay;
};
  
#endif
