/*
// $Workfile: timestmp.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/timestmp.cpp $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 5     6/08/98 4:14p Johno
// Added a DEBUG_ERRORBOX
// 
// 4     1/21/98 9:50a Fredf
// Year 2000 compliance.
// 
// 3     1/20/98 3:44p Fredf
// Got rid of GetNowTime(), use GetLocalTime(SYSTEMTIME) instead.
// 
// 2     1/20/98 8:55a Fredf
// Moved CTimeStamp from UTIL to TIMESTMP.
*/ 

#include "stdafx.h"
#include "util.h"
#include "timestmp.h"

/*
// Year 2000 compliant time stamp class.
// No virtual members or constructors! Size must remain
// 8 bytes (FILETIME)since these are part of structures
// written to the disk starting with PMG 5.0.
*/

BOOL CTimeStamp::SetFileCreate(HANDLE hFile)
{
	if (!::GetFileTime(hFile, &m_Time, NULL, NULL))
	{
		Invalidate();
	}
	return IsValid();
}

BOOL CTimeStamp::SetFileAccess(HANDLE hFile)
{
	if (!GetFileTime(hFile, NULL, &m_Time, NULL))
	{
		Invalidate();
	}
	return IsValid();
}

BOOL CTimeStamp::SetFileModify(HANDLE hFile)
{
	if (!GetFileTime(hFile, NULL, NULL, &m_Time))
	{
		Invalidate();
	}
	return IsValid();
}

BOOL CTimeStamp::SetFileCreate(LPCSTR pszFileName)
{
	Invalidate();

	FILETIME Time;
	if (Util::GetFileTime(pszFileName, &Time, NULL, NULL))
	{
		return Set(Time);
	}
   else
      DEBUG_ERRORBOX("CTimeStamp::SetFileCreate Failed");
   

	return IsValid();
}

BOOL CTimeStamp::SetFileAccess(LPCSTR pszFileName)
{
	Invalidate();

	FILETIME Time;
	if (Util::GetFileTime(pszFileName, NULL, &Time, NULL))
	{
		return Set(Time);
	}

	return IsValid();
}

BOOL CTimeStamp::SetFileModify(LPCSTR pszFileName)
{
	Invalidate();

	FILETIME Time;
	if (Util::GetFileTime(pszFileName, NULL, NULL, &Time))
	{
		return Set(Time);
	}

	return IsValid();
}

BOOL CTimeStamp::Set(const FILETIME& t)
{
	Invalidate();

	m_Time.dwLowDateTime = t.dwLowDateTime;
	m_Time.dwHighDateTime = t.dwHighDateTime;
	
	return IsValid();
}

BOOL CTimeStamp::Set(const SYSTEMTIME& t)
{
	Invalidate();

	FILETIME FileTime;
	if (SystemTimeToFileTime(&t, &FileTime))
	{
		Set(FileTime);
	}

	return IsValid();
}

BOOL CTimeStamp::Set(const time_t& t)
{
	Invalidate();

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
		Set(SystemTime);
	}

	return IsValid();
}

BOOL CTimeStamp::Set(const tm& t)
{
	Invalidate();

	SYSTEMTIME SystemTime;
	SystemTime.wYear = t.tm_year+1900;
	SystemTime.wMonth = t.tm_mon+1;
	SystemTime.wDayOfWeek = t.tm_wday;
	SystemTime.wDay = t.tm_mday;
	SystemTime.wHour = t.tm_hour;
	SystemTime.wMinute = t.tm_min;
	SystemTime.wSecond = t.tm_sec;
	SystemTime.wMilliseconds = 0;
	Set(SystemTime);

	return IsValid();
}

BOOL CTimeStamp::Set(void)
{
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	return Set(SystemTime);
}

CArchive& operator <<(CArchive& ar, CTimeStamp time)
{
	ar << (DWORD)time.m_Time.dwHighDateTime;
	return ar << (DWORD)time.m_Time.dwLowDateTime;
}

CArchive& operator >>(CArchive& ar, CTimeStamp& rtime)
{
	ar >> (DWORD&)rtime.m_Time.dwHighDateTime;
	return ar >> (DWORD&)rtime.m_Time.dwLowDateTime;
}

CArchive& operator<<(CArchive& ar, CTimeStampSpan timeSpan)
{
	LARGE_INTEGER Span;
	Span.QuadPart = timeSpan.m_Span;
	ar << (DWORD)Span.HighPart;
	return ar << (DWORD)Span.LowPart;
}

CArchive& operator>>(CArchive& ar, CTimeStampSpan& rtimeSpan)
{
	LARGE_INTEGER& Span = (LARGE_INTEGER&)rtimeSpan;
	ar >> (DWORD&)Span.HighPart;
	return ar >> (DWORD&)Span.LowPart;
}
