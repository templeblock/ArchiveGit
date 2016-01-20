/*
// $Workfile: DAY.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/DAY.CPP $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.1   30 Jul 1997 13:57:50   Fred
// Fixed crash with illegal dates
// 
//    Rev 1.0   25 Apr 1997 09:24:24   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:55:46   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "remind.h"
#include "day.h"

#include <ctype.h>

// Try to figure out what month the given string refers to.
// Return -1 on no match.
static int GetMonth(const CString& csMonth)
{
	static UINT MonthNames[] =
	{
		IDS_JANUARY,
		IDS_FEBRUARY,
		IDS_MARCH,
		IDS_APRIL,
		IDS_MAY,
		IDS_JUNE,
		IDS_JULY,
		IDS_AUGUST,
		IDS_SEPTEMBER,
		IDS_OCTOBER,
		IDS_NOVEMBER,
		IDS_DECEMBER
	};
	
	int nMonth = -1;
	
	if (csMonth.GetLength() >= 3)
	{
		// Have at least three characters of month name.
		for (int nMonthName = 0; nMonthName < sizeof(MonthNames)/sizeof(MonthNames[0]); nMonthName++)
		{
			// Check for a month name match.
			CString csMonthName;
			csMonthName.LoadString(MonthNames[nMonthName]);
			if (csMonthName.GetLength() >= csMonth.GetLength())
			{
				if (csMonth.CompareNoCase(csMonthName.Left(csMonth.GetLength())) == 0)
				{
					// Got the month.
					nMonth = nMonthName;
				}
			}
		}
	}
	
	return nMonth;
}

static void RemoveCharacters(CString& csString, const CString& csCharacters)
{
	TRY
	{
		if (!csString.IsEmpty())
		{
			// Strip off any run of the given characters.
			CString csRemove = csString.SpanIncluding(csCharacters);
			if (!csRemove.IsEmpty())
			{
				csString = csString.Mid(csRemove.GetLength());
			}
		}
	}
	END_TRY
}

static BOOL GetNextElement(CString& csElement, CString& csString, const CString& csSeparators)
{
	BOOL fResult = FALSE;
	
	TRY
	{
		csElement.Empty();
		
		// Make sure the string is not empty, otherwise, we're done.
		if (!csString.IsEmpty())
		{
			// Strip off any separator characters.
			RemoveCharacters(csString, csSeparators);
			
			// Get the element (run of digits or non-separator characters.)
			if (isdigit(csString[0]))
			{
				csElement = csString.SpanIncluding("0123456789");
			}
			else
			{
				csElement = csString.SpanExcluding(csSeparators);
			}
			if (!csElement.IsEmpty())
			{
				// Remove element from original string.
				csString = csString.Mid(csElement.GetLength());
				
				// Sweet smell of success!
				fResult = TRUE;
			}
		}
	}
	END_TRY
	
	return fResult;
}

// Construct for current time.
CDay::CDay(void)
{
	m_fIsValid = FALSE;

	CTimeStamp Time;
	Time.SetTime();

	CommonConstruct(Time);
}
	
// Construct for given time.
CDay::CDay(const CTimeStamp& t)
{
	m_fIsValid = FALSE;

	CommonConstruct(t);
}
	
// Construct for given time.
CDay::CDay(const FILETIME& t)
{
	m_fIsValid = FALSE;

	CTimeStamp Time;
	Time.SetTime(t);

	CommonConstruct(Time);
}
	
// Construct for given time.
CDay::CDay(const SYSTEMTIME& t)
{
	m_fIsValid = FALSE;

	CTimeStamp Time;
	Time.SetTime(t);

	CommonConstruct(Time);
}

// Construct for given time.
CDay::CDay(const time_t& t)
{
	m_fIsValid = FALSE;

	CTimeStamp Time;
	Time.SetTime(t);

	CommonConstruct(Time);
}
	
// Construct for given time.
CDay::CDay(const tm& t)
{
	m_fIsValid = FALSE;

	CTimeStamp Time;
	Time.SetTime(t);

	CommonConstruct(Time);
}
	
// Construct for given string.
CDay::CDay(LPCSTR pszDay)
{
	m_fIsValid = FALSE;
	
	int nMonth = -1;
	int nDay = -1;
	
	TRY
	{
		// Save the string used to create the day.
		m_csDay = pszDay;
		
		// Load the separators which can be used to separate elements of the date.
		CString csSeparators;
		csSeparators.LoadString(IDS_DATE_SEPARATORS);
				
		// Collect all the elements in the day string.
		CString csDay = pszDay;
		CWordArray cwaElements;
		CString csElement;
		while (GetNextElement(csElement, csDay, csSeparators))
		{
			BOOL fKeep = FALSE;
				
			// Only include elements which are numbers between 1 and 31.
			// If we see a month name go by, remember it.
			if (!csElement.IsEmpty())
			{
				if (isdigit(csElement[0]))
				{
					int nNumber = atoi(csElement);
					if ((nNumber >= 1) && (nNumber <= 31))
					{
						cwaElements.Add((WORD)nNumber);
					}
				}
				else
				{
					// Only keep the first month name.
					if (nMonth == -1)
					{
						nMonth = GetMonth(csElement);
					}
				}
			}
		}
		
		// Make sure the number of elements makes sense.
		int nElements = cwaElements.GetSize();
		
		if (nMonth != -1)
		{
			// We saw a real month name, so all we have to do is
			// find the day number. For now, just use the first number.
			if (nElements >= 1)
			{
				nDay = (int)cwaElements.GetAt(0);
			}
		}
		else
		{
			// We did not see an explicit month name. Thus we could have a date
			// composed of only numbers. We need at least two of them.
			if (nElements >= 2)
			{
				int nM = -1;
				int nD = -1;
				int nY = -1;
				int nIndex = 0;
				int i;
				
				// Get the preferred date format.
				CString csDateFormat = DateFormat;
				
				// Get MDY ordering
				for (i = 0; i < csDateFormat.GetLength(); i++)
				{
					if (csDateFormat[i] == 'M')
					{
						if (nM == -1)
						{
							nM = nIndex++;
						}
					}
					else if (csDateFormat[i] == 'D')
					{
						if (nD == -1)
						{
							nD = nIndex++;
						}
					}
					else if (csDateFormat[i] == 'Y')
					{
						if (nY == -1)
						{
							nY = nIndex++;
						}
					}
				}

				// Assign default values.				
				if (nM == -1)
				{
					nM = nIndex++;
				}
				if (nD == -1)
				{
					nM = nIndex++;
				}
				if (nY == -1)
				{
					nY = nIndex++;
				}
				
				// If we have only two elements, then skip looking for the year.
				if (nElements == 2)
				{
					nY = -1;
				}
				
				// Search for the month, day, and year.
				for (i = 0; i <= 2; i++)
				{
					for (int nElement = 0; nElement < cwaElements.GetSize(); nElement++)
					{
						int nValue = cwaElements.GetAt(nElement);
						if (i == nD)
						{
							// Found day.
							nDay = nValue;
							cwaElements.RemoveAt(nElement);
							break;
						}
						if (i == nM)
						{
							if ((nValue >= 1) && (nValue <= 12))
							{
								// Found month.
								nMonth = nValue-1;
								cwaElements.RemoveAt(nElement);
								break;
							}
						}
						if (i == nY)
						{
							// Found year, delete it.
							cwaElements.RemoveAt(nElement);
							break;
						}
					}
				}
			}
		}
	}
	END_TRY
		
	// At this point we should have a month and day. If we do, celebrate.
	if ((nDay != -1) && (nMonth != -1))
	{
		// Create a time structure that represents the day, month, and year of the
		// next time this event will happen.

		SYSTEMTIME t;
		GetLocalTime(&t);
		t.wHour = 0;
		t.wMinute = 0;
		t.wSecond = 0;
		t.wMilliseconds = 0;

		// Remember the current day,
		CTimeStamp CurrentTime;
		CurrentTime.SetTime(t);

		t.wMonth = nMonth+1;
		t.wDayOfWeek = 0;
		t.wDay = nDay;

		CTimeStamp Time;
		Time.SetTime(t);

		// If the event day is before the current day then move to the next year.
		if (Time < CurrentTime)
		{
			t.wYear++;
			Time.SetTime(t);
		}

		CommonConstruct(Time);
	}
}

void CDay::CommonConstruct(const CTimeStamp& Time)
{
	// Normalize the time to the start of the day (hour, minute, second = 0).
	CTimeStamp NormalizedTime;
	NormalizedTime = Time;
	if (NormalizedTime.IsValid())
	{
		SYSTEMTIME SystemTime;
		if (NormalizedTime.GetTime(SystemTime))
		{
			SystemTime.wMilliseconds = 0;
			SystemTime.wSecond = 0;
			SystemTime.wMinute = 0;
			SystemTime.wHour = 0;
			NormalizedTime.SetTime(SystemTime);
		}
	}

	m_Time = NormalizedTime;

	m_fIsValid = m_Time.IsValid();
}

// Return the difference in days.
int CDay::operator-(const CDay& Day) const
{
	int nResult = 0;
	
	if (IsValid() && Day.IsValid())
	{
		nResult = m_Time.Days()-Day.m_Time.Days();
	}
	
	return nResult;
}

// Check if two dates are equal.
BOOL CDay::operator==(const CDay& Day) const
{
	BOOL fResult = FALSE;
	
	if (IsValid() && Day.IsValid())
	{
		fResult = m_Time == Day.m_Time;
	}
	
	return fResult;
}

// Check if one date is before another.
BOOL CDay::operator<(const CDay& Day) const
{
	BOOL fResult = FALSE;
	
	if (IsValid() && Day.IsValid())
	{
		fResult = m_Time < Day.m_Time;
	}
	
	return fResult;
}
