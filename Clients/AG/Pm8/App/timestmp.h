/*
// $Workfile: timestmp.h $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/timestmp.h $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 6     5/14/98 12:17p Mwilson
// changed function name GetFileTime to GetTime.
// 
// 5     5/14/98 11:32a Mwilson
// Added accessor function to get the FILETIME variable
// 
// 4     1/20/98 3:44p Fredf
// Got rid of GetNowTime(), use GetLocalTime(SYSTEMTIME) instead.
// 
// 3     1/20/98 9:26a Fredf
// Removed old typedefs.
// 
// 2     1/20/98 8:55a Fredf
// Moved CTimeStamp from UTIL to TIMESTMP.
*/ 

#ifndef _TIMESTMP_H_
#define _TIMESTMP_H_

/*
// Year 2000 compliant time stamp class.
// No virtual members or constructors! Size must remain
// 8 bytes (FILETIME)since these are part of structures
// written to the disk starting with PMG 5.0.
*/

class CTimeStampSpan
{
public:
	static __int64 SecondsSpan(__int64 nSeconds)
		{ return nSeconds*10000000; }
	static __int64 MinutesSpan(__int64 nMinutes)
		{ return SecondsSpan(nMinutes*60); }
	static __int64 HoursSpan(__int64 nHours)
		{ return MinutesSpan(nHours*60); }
	static __int64 DaysSpan(__int64 nDays)
		{ return HoursSpan(nDays*24); }
protected:
	__int64 m_Span;
public:
	CTimeStampSpan(void)
		{ m_Span = 0; }
	CTimeStampSpan(CTimeStampSpan& Span)
		{ m_Span = __int64(Span); }
	CTimeStampSpan(__int64 nTicks)
		{ m_Span = nTicks; }
	CTimeStampSpan(__int64 nDays, __int64 nHours, __int64 nMinutes, __int64 nSeconds)
		{ SetSpan(nDays, nHours, nMinutes, nSeconds); }
public:
	void SetSpan(__int64 nDays = 0, __int64 nHours = 0, __int64 nMinutes = 0, __int64 nSeconds = 0)
		{ m_Span = DaysSpan(nDays)+HoursSpan(nHours)+MinutesSpan(nMinutes)+SecondsSpan(nSeconds); }
	void AddSpan(__int64 nDays = 0, __int64 nHours = 0, __int64 nMinutes = 0, __int64 nSeconds = 0)
		{ m_Span += DaysSpan(nDays)+HoursSpan(nHours)+MinutesSpan(nMinutes)+SecondsSpan(nSeconds); }
	operator __int64() const
		{ return m_Span; }
	__int64 GetSecondSpan(void) const
		{ return m_Span/((__int64)10000000); }
	__int64 GetMinuteSpan(void) const
		{ return m_Span/((__int64)10000000*60); }
	__int64 GetHourSpan(void) const
		{ return m_Span/((__int64)10000000*60*60); }
	__int64 GetDaySpan(void) const
		{ return m_Span/((__int64)10000000*60*60*24); }
	BOOL operator==(const CTimeStampSpan& d) const
		{ return m_Span == d.m_Span; }
	BOOL operator!=(const CTimeStampSpan& d) const
		{ return !(operator==(d)); }
	BOOL operator>(const CTimeStampSpan& d) const
		{ return m_Span > d.m_Span; }
	BOOL operator>=(const CTimeStampSpan& d) const
		{ return (operator>(d))||(operator==(d)); }
	BOOL operator<(const CTimeStampSpan& d) const
		{ return !(operator>=(d)); }
	BOOL operator<=(const CTimeStampSpan& d) const
		{ return !(operator>(d)); }
	CTimeStampSpan& operator+=(const CTimeStampSpan& d)
		{ m_Span += d.m_Span; return *this; }
	CTimeStampSpan& operator-=(const CTimeStampSpan& d)
		{ m_Span -= d.m_Span; return *this; }
	CTimeStampSpan operator+(const CTimeStampSpan& d) const
		{ return CTimeStampSpan(m_Span+d.m_Span); }
	CTimeStampSpan operator-(const CTimeStampSpan& d) const
		{ return CTimeStampSpan(m_Span-d.m_Span); }

	// serialization
	friend CArchive& operator<<(CArchive& ar, CTimeStampSpan timeSpan);
	friend CArchive& operator>>(CArchive& ar, CTimeStampSpan& rtimeSpan);
};

class CTimeStamp
{
protected:
	FILETIME m_Time;
protected:
	operator __int64() const
		{ return (((__int64)(m_Time.dwHighDateTime))<<32)+((__int64)(m_Time.dwLowDateTime)); }
	void Set(__int64 nTicks)
		{ m_Time.dwHighDateTime = (DWORD)((nTicks >> 32)&0x00000000ffffffff);
		  m_Time.dwLowDateTime = (DWORD)((nTicks >> 0)&0x00000000ffffffff); }
public:
	FILETIME GetTime() {return m_Time;}
	BOOL IsValid(void) const
		{ return (m_Time.dwLowDateTime != 0) || (m_Time.dwHighDateTime != 0); }
	void Invalidate(void)
		{ m_Time.dwLowDateTime = 0; m_Time.dwHighDateTime = 0; }
	BOOL SetFileCreate(HANDLE hFile);
	BOOL SetFileAccess(HANDLE hFile);
	BOOL SetFileModify(HANDLE hFile);
	BOOL SetFileCreate(LPCSTR pszFileName);
	BOOL SetFileAccess(LPCSTR pszFileName);
	BOOL SetFileModify(LPCSTR pszFileName);
	BOOL Set(const FILETIME& t);
	BOOL Set(const SYSTEMTIME& t);
	BOOL Set(const time_t& t);
	BOOL Set(const tm& t);
	BOOL Set(void);
	BOOL operator==(const CTimeStamp& t) const
		{ return memcmp(&t, this, sizeof(*this)) == 0; }
	BOOL operator!=(const CTimeStamp& t) const
		{ return !(operator==(t)); }
	BOOL operator>(const CTimeStamp& t) const
		{ return (m_Time.dwHighDateTime > t.m_Time.dwHighDateTime)
				|| ((m_Time.dwHighDateTime == t.m_Time.dwHighDateTime)
				 && (m_Time.dwLowDateTime > t.m_Time.dwLowDateTime)); }
	BOOL operator>=(const CTimeStamp& t) const
		{ return (operator>(t))||(operator==(t)); }
	BOOL operator<(const CTimeStamp& t) const
		{ return !(operator>=(t)); }
	BOOL operator<=(const CTimeStamp& t) const
		{ return !(operator>(t)); }
	CTimeStamp& operator+=(const CTimeStampSpan& d)
		{ Set(__int64(*this)+__int64(d)); return *this; }
	CTimeStamp& operator-=(const CTimeStampSpan& d)
		{ Set(__int64(*this)-__int64(d)); return *this; }
	CTimeStampSpan operator-(const CTimeStamp& t)
	{ return CTimeStampSpan(__int64(t)-__int64(*this)); }

	// serialization
	friend CArchive& operator<<(CArchive& ar, CTimeStamp time);
	friend CArchive& operator>>(CArchive& ar, CTimeStamp& rtime);
};

#endif
