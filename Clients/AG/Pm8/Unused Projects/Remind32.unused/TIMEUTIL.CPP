/*
// $Workfile: TIMEUTIL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:27p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/TIMEUTIL.CPP $
// 
// 1     3/03/99 6:27p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
*/

#include "stdafx.h"
#include "timeutil.h"

CTimeStamp::CTimeStamp()
{
	SetInvalidTime();
}

CTimeStamp::~CTimeStamp()
{
}

BOOL CTimeStamp::SetTime(void)
{
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);

	return SetTime(SystemTime);
}

BOOL CTimeStamp::SetTime(const CTimeStamp& t)
{
	*this = t;
	return IsValid();
}

BOOL CTimeStamp::SetTime(const FILETIME& t)
{
	SetInvalidTime();

	m_Time.dwLowDateTime = t.dwLowDateTime;
	m_Time.dwHighDateTime = t.dwHighDateTime;
	
	return IsValid();
}

BOOL CTimeStamp::SetTime(const SYSTEMTIME& t)
{
	SetInvalidTime();

	FILETIME FileTime;
	if (SystemTimeToFileTime(&t, &FileTime))
	{
		SetTime(FileTime);
	}

	return IsValid();
}

BOOL CTimeStamp::SetTime(const time_t& t)
{
	SetInvalidTime();

	tm* ptm = localtime(&t);
	if (ptm != NULL)
	{
		SYSTEMTIME SystemTime;
		SystemTime.wYear = ptm->tm_year+1900;
		SystemTime.wMonth = ptm->tm_mon+1;
		SystemTime.wDayOfWeek = ptm->tm_wday;
		SystemTime.wDay = ptm->tm_mday;
		SystemTime.wHour = ptm->tm_hour;
		SystemTime.wMinute = ptm->tm_min;
		SystemTime.wSecond = ptm->tm_sec;
		SystemTime.wMilliseconds = 0;
		SetTime(SystemTime);
	}

	return IsValid();
}

BOOL CTimeStamp::SetTime(const tm& t)
{
	SetInvalidTime();

	SYSTEMTIME SystemTime;
	SystemTime.wYear = t.tm_year+1900;
	SystemTime.wMonth = t.tm_mon+1;
	SystemTime.wDayOfWeek = t.tm_wday;
	SystemTime.wDay = t.tm_mday;
	SystemTime.wHour = t.tm_hour;
	SystemTime.wMinute = t.tm_min;
	SystemTime.wSecond = t.tm_sec;
	SystemTime.wMilliseconds = 0;
	SetTime(SystemTime);

	return IsValid();
}

BOOL CTimeStamp::SetTime(LPCSTR pszTime)
{
	DWORD dwHighDateTime = (DWORD)-1;
	DWORD dwLowDateTime = (DWORD)-1;

	CString csTime;
	CString csHighDateTime;
	CString csLowDateTime;

	TRY
	{
		csTime = pszTime;
		if (csTime.GetLength() == 1+8+8+1)
		{
			if ((csTime[0] == '<') && (csTime[1+8+8] == '>'))
			{
				csHighDateTime = csTime.Mid(1, 8);
				csLowDateTime = csTime.Mid(9, 8);

				sscanf(csHighDateTime, "%X", &dwHighDateTime);
				sscanf(csLowDateTime, "%X", &dwLowDateTime);
			}
		}
	}
	END_TRY

	m_Time.dwHighDateTime = dwHighDateTime;
	m_Time.dwLowDateTime = dwLowDateTime;

	return IsValid();
}

BOOL CTimeStamp::GetTime(FILETIME& t) const
{
	BOOL fResult = FALSE;

	if (IsValid())
	{
		t = m_Time;
		fResult = TRUE;
	}

	return fResult;
}

BOOL CTimeStamp::GetTime(SYSTEMTIME& t) const
{
	BOOL fResult = FALSE;

	if (IsValid())
	{
		if (FileTimeToSystemTime(&m_Time, &t))
		{
			fResult = TRUE;
		}
	}

	return fResult;
}

BOOL CTimeStamp::GetTime(time_t& t) const
{
	BOOL fResult = FALSE;

	SYSTEMTIME SystemTime;
	if (GetTime(SystemTime))
	{
		tm Time;
		Time.tm_hour = SystemTime.wHour;
		Time.tm_isdst = -1;
		Time.tm_mday = SystemTime.wDay;
		Time.tm_min = SystemTime.wMinute;
		Time.tm_mon = SystemTime.wMonth-1;
		Time.tm_sec =  SystemTime.wSecond;
		Time.tm_wday = 0;
		Time.tm_yday = 0;
		Time.tm_year = SystemTime.wYear-1900;

		t = mktime(&Time);
		if (t != (time_t)-1)
		{
			fResult = TRUE;
		}
	}

	return fResult;
}

BOOL CTimeStamp::GetTime(tm& t) const
{
	BOOL fResult = FALSE;

	SYSTEMTIME SystemTime;
	if (GetTime(SystemTime))
	{
		t.tm_hour = SystemTime.wHour;
		t.tm_isdst = -1;
		t.tm_mday = SystemTime.wDay;
		t.tm_min = SystemTime.wMinute;
		t.tm_mon = SystemTime.wMonth-1;
		t.tm_sec =  SystemTime.wSecond;
		t.tm_wday = 0;
		t.tm_yday = 0;
		t.tm_year = SystemTime.wYear-1900;

		// Calling mktime() will normalize values in tm structure.
		if (mktime(&t) != (time_t)-1)
		{
			fResult = TRUE;
		}
	}

	return fResult;
}

BOOL CTimeStamp::GetTime(CString& csTime) const
{
	csTime.Format("<%08X%08X>", m_Time.dwHighDateTime, m_Time.dwLowDateTime);
	return TRUE;
}

DWORD CTimeStamp::Days(void) const
{
	return (DWORD)((((double)m_Time.dwHighDateTime)*4294967295.0+((double)m_Time.dwLowDateTime))/864000000000.0);
}

DWORD CTimeStamp::Hours(void) const
{
	return (DWORD)((((double)m_Time.dwHighDateTime)*4294967295.0+((double)m_Time.dwLowDateTime))/36000000000.0);
}

DWORD CTimeStamp::Minutes(void) const
{
	return (DWORD)((((double)m_Time.dwHighDateTime)*4294967295.0+((double)m_Time.dwLowDateTime))/600000000.0);
}



void CTimeStamp::SetInvalidTime(void)
{
	m_Time.dwLowDateTime = (DWORD)-1;
	m_Time.dwHighDateTime = (DWORD)-1;
}

BOOL CTimeStamp::IsValidTime(const FILETIME& t) const
{
	return (t.dwLowDateTime != (DWORD)-1) || (t.dwHighDateTime != (DWORD)-1);
}

BOOL CTimeStamp::operator==(const CTimeStamp& t) const
{
	return CompareFileTime(&m_Time, &t.m_Time) == 0;
}

BOOL CTimeStamp::operator<(const CTimeStamp& t) const
{
	return CompareFileTime(&m_Time, &t.m_Time) < 0;
}

BOOL CTimeStamp::operator<=(const CTimeStamp& t) const
{
	return (*this < t) || (*this == t);
}

BOOL CTimeStamp::operator>(const CTimeStamp& t) const
{
	return !(*this <= t);
}

BOOL CTimeStamp::operator>=(const CTimeStamp& t) const
{
	return !(*this < t);
}
