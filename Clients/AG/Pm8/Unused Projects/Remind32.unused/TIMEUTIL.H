/*
// $Workfile: TIMEUTIL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:27p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/TIMEUTIL.H $
// 
// 1     3/03/99 6:27p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
*/

class CTimeStamp;

#ifndef __TIMEUTIL_H__
#define __TIMEUTIL_H__

class CTimeStamp
{
public:
	// Construction.
	CTimeStamp();
	virtual ~CTimeStamp();

public:
	BOOL SetTime(void);
	BOOL SetTime(const CTimeStamp& t);
	BOOL SetTime(const FILETIME& t);
	BOOL SetTime(const SYSTEMTIME& t);
	BOOL SetTime(const time_t& t);
	BOOL SetTime(const tm& t);
	BOOL SetTime(LPCSTR pszTime);

public:
	BOOL GetTime(FILETIME& t) const;
	BOOL GetTime(SYSTEMTIME& t) const;
	BOOL GetTime(time_t& t) const;
	BOOL GetTime(tm& t) const;
	BOOL GetTime(CString& csTime) const;

public:
	DWORD Days(void) const;
	DWORD Hours(void) const;
	DWORD Minutes(void) const;

public:
	void SetInvalidTime(void);
	BOOL IsValidTime(const FILETIME& t) const;
	BOOL IsValid(void) const
		{ return IsValidTime(m_Time); }

public:
	BOOL operator==(const CTimeStamp& t) const;
	BOOL operator<(const CTimeStamp& t) const;
	BOOL operator<=(const CTimeStamp& t) const;
	BOOL operator>(const CTimeStamp& t) const;
	BOOL operator>=(const CTimeStamp& t) const;
	
protected:
	FILETIME m_Time;
};

#endif					// #ifndef __INIFILE_H__
