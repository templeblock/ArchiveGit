/*
// $Workfile: EVENT.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/EVENT.CPP $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:26   Fred
// Initial revision.
// 
//    Rev 1.1   25 Feb 1997 09:53:38   Fred
// Support for sender events
// 
//    Rev 1.0   20 Feb 1997 13:55:46   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "remind.h"
#include "event.h"

/////////////////////////////////////////////////////////////////////////////
// CEventIniFile

CEventIniFile::CEventIniFile()
{
}

CEventIniFile::~CEventIniFile()
{
}

#if 0
// Get a key which is not in use in the specified section.
// WARNING: This does not work with shared .INI files.
// Fortunately, event information files are "nidded."
// If this data needs to be shared, then a non-INI file format
// could be used.
BOOL CEventIniFile::GetKey(LPCSTR pszSection, CString& csKey)
{
	BOOL fResult = FALSE;
	
	CString csTest;
	LPSTR pszKeys = NULL;
	
	TRY
	{
		csKey.Empty();
		
		// Attempt to get the next key to use. If it's missing,
		// we'll have to resynchronize.
		
		BOOL fResynchronize = FALSE;
		
		DWORD dwKey = GetDword(pszSection, "NextKey", (DWORD)-1);
		if (dwKey == (DWORD)-1)
		{
			// Must resynchronize.
			fResynchronize = TRUE;
		}
		else
		{
			// The key existed, confirm that it's not already in use.
			csTest.Format("%lu", dwKey);
			csTest = GetString(pszSection, csTest);
			if (!csTest.IsEmpty())
			{
				// Key exist, must resynchronize count.
				fResynchronize = TRUE;
			}
		}
		
		// Check if we have to resynchronize.
		if (fResynchronize)
		{
			// Forget the invalid key.
			dwKey = (DWORD)-1;
			
			// Read in all the keys and find an entry in the name space.
			// Note the algorithm used is really braindead, but will be
			// fast enough for a small number of keys and won't be fooled
			// if some large key values get in.
			
			pszKeys = GetSectionKeys(pszSection);
			if (pszKeys != NULL)
			{
				DWORD dwTrial = 0;
			
				while (dwTrial != (DWORD)-1)
				{
					// Search for the trial key in the current keys.
					// If it's not found, then we can use it.
					LPCSTR pszKey = pszKeys;
					while (*pszKey != '\0')
					{
						// Check if key's match.
						if (dwTrial == (DWORD)atol(pszKey))
						{
							// Trial key already in use, try another.
							break;
						}
					
						// Move to next key.
						pszKey += strlen(pszKey)+1;
					}
					
					// Check if we found the key
					if (*pszKey == '\0')
					{
						// Trial key is not in use.
						break;
					}
					
					// Try the next possible key.
					dwTrial++;
				}
				
				// Use the trial key (or -1 if none was found.)
				dwKey = dwTrial;
				
				// Save the next value as the best guess for the next key.
				if (dwKey != (DWORD)-1)
				{
					WriteDword(pszSection, "NextKey", dwKey+1);
				}
			}
		}
		
		// Create the key.
		if (dwKey != (DWORD)-1)
		{
			csKey.Format("%lu", dwKey);
			fResult = TRUE;
		}
	}
	END_TRY
	
	// Release any section keys we allocated.
	delete [] pszKeys;
	pszKeys = NULL;
	
	return fResult;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CEvent

CEvent::CEvent()
{
	m_fIsValid = FALSE;
	TRY
	{
		m_dwEventTypeId = (DWORD)-1;
		m_fIsSenderDatabase = FALSE;
		m_fIsStandardFormatAddressBook = FALSE;
		m_dwNameId = (DWORD)-1;
		m_nDays = -1;
		m_Time.SetInvalidTime();
	}
	END_TRY
}

CEvent::CEvent(LPCSTR pszIniString)
{
	m_fIsValid = FALSE;
	
	CMlsStringArray csaEvent;
	TRY
	{
		csaEvent.SetNames(pszIniString);
		if (csaEvent.GetSize() == 6)
		{
			m_csAddressBookUserName = csaEvent.ElementAt(0);
			m_fIsSenderDatabase = (BOOL)atoi(csaEvent.ElementAt(1));
			m_fIsStandardFormatAddressBook = FALSE;
			m_dwEventTypeId = atol(csaEvent.ElementAt(2));
			m_dwNameId = atol(csaEvent.ElementAt(3));
			m_nDays = atoi(csaEvent.ElementAt(4));

			if ((csaEvent.ElementAt(5))[0] != '<')
			{
				// Old-style times (time_t)
				time_t t = (time_t)atol(csaEvent.ElementAt(5));
				m_Time.SetTime(t);
			}
			else
			{
				m_Time.SetTime(csaEvent.ElementAt(5));
			}

			CommonConstruct();
		}
	}
	END_TRY
}

CEvent::CEvent(LPCSTR pszAddressBookUserName, BOOL fIsSenderDatabase, DWORD dwEventTypeId, DWORD dwNameId, int nDays, CDay* pDay /*=NULL*/)
{
	m_fIsValid = FALSE;
	
	TRY
	{
		m_csAddressBookUserName = pszAddressBookUserName;
		m_fIsSenderDatabase = fIsSenderDatabase;
		m_fIsStandardFormatAddressBook = FALSE;
		m_dwEventTypeId = dwEventTypeId;
		m_dwNameId = dwNameId;
		m_nDays = nDays;
		
		if (pDay != NULL)
		{
			m_Time = pDay->GetTime();
			m_csDay = pDay->GetDay();
		}
		else
		{
			m_Time.SetInvalidTime();
		}
		
		CommonConstruct();
	}
	END_TRY
}

CEvent::CEvent(const CEvent& Event)
{
	m_fIsValid = FALSE;
	TRY
	{
		m_csAddressBookUserName = Event.m_csAddressBookUserName;
		m_fIsSenderDatabase = Event.m_fIsSenderDatabase;
		m_fIsStandardFormatAddressBook = Event.m_fIsStandardFormatAddressBook;
		m_dwEventTypeId = Event.m_dwEventTypeId;
		m_csEventType = Event.m_csEventType;
		m_dwNameId = Event.m_dwNameId;
		m_csName = Event.m_csName;
		m_nDays = Event.m_nDays;
		m_csDay = Event.m_csDay;
		m_Time = Event.m_Time;
		m_fIsValid = TRUE;
	}
	END_TRY
}

void CEvent::CommonConstruct(void)
{
	TRY
	{
		if (AddressBook->OpenAddressBook(m_csAddressBookUserName, m_fIsSenderDatabase) == ERRORCODE_None)
		{
			m_fIsStandardFormatAddressBook = AddressBook->GetIsStandardFormat();
			
			if (m_dwEventTypeId == (DWORD)-1)
			{
				m_csEventType.LoadString(IDS_ALL_EVENT_TYPE_NAME);
			}
			else
			{
				CFlatFileDatabaseCursor* pIdCursor = AddressBook->GetIdCursor();
				if (pIdCursor != NULL)
				{
					CFlatFileDatabaseField* pField = pIdCursor->GetField(m_dwEventTypeId);
					if (pField != NULL)
					{
						m_csEventType = pField->GetName();
					}
				}
			}
				
			if (AddressBook->GetNameFromId(m_dwNameId, m_csName))
			{
				m_fIsValid = TRUE;
			}
		}
	}
	END_TRY
}
	

CEvent::~CEvent()
{
}

BOOL CEvent::GetEventString(CString& csString) const
{
	// Special formatting for event. People localizing this
	// code take special note of the following code. It was set
	// up so that hopefully you'll be able to pick a format string
	// that will work for the target country. The id of the resource
	// string used for formating is IDS_UPCOMING_EVENT_FORMAT_STANDARD
	// for standard address books and IDS_UPCOMING_EVENT_FORMAT_IMPORTED
	// for imported address books.
	//
	// Special format sequences:
	//
	//		%% - percent sign
	//		%A - address book name
	//		%E - event name (case is same as from address book)
	//		%e - event name (all lowercase)
	//		%N - user name (case is same as from address book)
	//		%T - days away (0 --> IDS_TODAY, 1 --> IDS_TOMORROW, other --> IDS_DAYS_AWAY formatted with number)
	//		%t - days away (always a number)
	//		%D - event date (from address book)
	
	BOOL fResult = FALSE;
	
	CString csFormat;
	CString csScratch1;
	CString csScratch2;
	
	TRY
	{
		csString.Empty();
		
		csFormat.LoadString(GetIsStandardFormatAddressBook() ? IDS_UPCOMING_EVENT_FORMAT_STANDARD : IDS_UPCOMING_EVENT_FORMAT_IMPORTED);
		if (!csFormat.IsEmpty())
		{
			// March through format and generate the result.
			int nIndex = 0;
			while (nIndex < csFormat.GetLength())
			{
				// Check special character.
				if (csFormat[nIndex] == '%')
				{
					// Check the next character.
					if (++nIndex < csFormat.GetLength())
					{
						switch (csFormat[nIndex])
						{
							case '%':
							{
								// %% - percent sign
								csString += '%';
								break;
							}
							case 'A':
							{
								// %A - address book name
								csString += GetAddressBookUserName();
								break;
							}
							case 'E':
							{
								// %E - event name (case is same as from address book)
								csString += GetEventType();
								break;
							}
							case 'e':
							{
								// %e - event name (all lowercase)
								csScratch1 = GetEventType();
								csScratch1.MakeLower();
								csString += csScratch1;
								break;
							}
							case 'N':
							{
								// %N - user name (case is same as from address book)
								csString += GetName();
								break;
							}
							case 'T':
							{
								// %T - days away (0 --> IDS_TODAY, 1 --> IDS_TOMORROW, other --> IDS_DAYS_AWAY formatted with number)
								int nDays = GetDays();
								if (nDays == 0)
								{
									csScratch1.LoadString(IDS_TODAY);
								}
								else if (nDays == 1)
								{
									csScratch1.LoadString(IDS_TOMORROW);
								}
								else
								{
									csScratch2.LoadString(IDS_DAYS_AWAY);
									csScratch1.Format(csScratch2, nDays);
								}
								csString += csScratch1;
								break;
							}
							case 't':
							{
								// %t - days away (always a number)
								csScratch1.Format("%d", GetDays());
								csString += csScratch1;
								break;
							}
							case 'D':
							{
								// %D - event date (from address book)
								csString += GetDay();
								break;
							}
							default:
							{
								csString += csFormat[nIndex];
								break;
							}
						}
						
						// Move on to the next character.
						nIndex++;
					}
				}
				else
				{
					// Nothing special, just copy it.
					csString += csFormat[nIndex];
					
					// Move on to next character
					nIndex++;
				}
			}
			
			fResult = TRUE;
		}
	}
	END_TRY
				
	return fResult;
}

BOOL CEvent::GetIniString(CString& csString) const
{
	BOOL fResult = FALSE;

	CString csTime;
	m_Time.GetTime(csTime);
	
	TRY
	{
		csString.Format(
			"%s|%d|%ld|%ld|%d|%s", 
			(LPCSTR)m_csAddressBookUserName,
			(int)m_fIsSenderDatabase,
			m_dwEventTypeId,
			m_dwNameId,
			m_nDays,
			csTime);
						
		fResult = TRUE;
	}
	END_TRY
	
	return fResult;
}

void CEvent::Invalidate(void)
{
	m_fIsValid = FALSE;
	TRY
	{
		m_csAddressBookUserName.Empty();
		m_fIsSenderDatabase = FALSE;
		m_fIsStandardFormatAddressBook = FALSE;
		m_dwEventTypeId = (DWORD)-1;
		m_csEventType.Empty();
		m_dwNameId = (DWORD)-1;
		m_csName.Empty();
		m_nDays = -1;
		m_csDay.Empty();
		m_Time.SetInvalidTime();
	}
	END_TRY
}

CEvent& CEvent::operator=(const CEvent& Event)
{
	m_fIsValid = FALSE;
	TRY
	{
		m_csAddressBookUserName = Event.m_csAddressBookUserName;
		m_fIsSenderDatabase = Event.m_fIsSenderDatabase;
		m_fIsStandardFormatAddressBook = Event.m_fIsStandardFormatAddressBook;
		m_dwEventTypeId = Event.m_dwEventTypeId;
		m_csEventType = Event.m_csEventType;
		m_dwNameId = Event.m_dwNameId;
		m_csName = Event.m_csName;
		m_nDays = Event.m_nDays;
		m_Time = Event.m_Time;
		m_csDay = Event.m_csDay;
		m_fIsValid = TRUE;
	}
	END_TRY
	
	return *this;
}

int CEvent::ComparePartial(const CEvent& Event)
{
	int nResult = 0;
	
	// Compare two events. Only the address book, event type and
	// name are used. All string compares are case insensitive.
	// The number of days and time are ignored.
	
	if (IsValid() && Event.IsValid())
	{
		// First, sort by address book name.
		nResult = strcmpi(GetAddressBookUserName(), Event.GetAddressBookUserName());
		if (nResult == 0)
		{
			// Next, sort by address book type. This is manily to make sure sender and nonsender
			// databases with the same name will not hide each other.
			nResult = m_fIsSenderDatabase-Event.m_fIsSenderDatabase;
			if (nResult == 0)
			{
				// Next, sort by event type.
				nResult = stricmp(GetEventType(), Event.GetEventType());
				if (nResult == 0)
				{
					// Last, sort by name.
					nResult = strcmpi(GetName(), Event.GetName());
				}
			}
		}
	}
	
	return nResult;
}


/////////////////////////////////////////////////////////
// CEventArray

CEventArray::CEventArray()
{
	m_fNoDuplicates = TRUE;
}

CEventArray::~CEventArray()
{
	DeleteAll();
}
	
// Get the number of enabled events.
int CEventArray::GetEnabledCount(void)
{
	int nCount = 0;
	
	for (int i = 0; i < GetSize(); i++)
	{
		CEvent* pEvent = ElementAt(i);
		if (!GETAPP()->IsEventDisabled(pEvent))
		{
			nCount++;
		}
	}
	
	return nCount;
}

void CEventArray::DeleteAll(void)
{
	DeleteAt(0, GetSize());
}

void CEventArray::DeleteAt(int nIndex, int nCount /*=1*/)
{
	for (int i = 0; i < nCount; i++)
	{
		if (((nIndex+i) >= 0) && ((nIndex+i) < GetSize()))
		{
			delete GetAt(nIndex+i);
			SetAt(nIndex+i, NULL);
		}
	}

	INHERITED::RemoveAt(nIndex, nCount);
}

// Add a new event with sorting and duplicate checking.
void CEventArray::Add(CEvent* pEvent)
{
	BOOL fInserted = FALSE;
	
	if ((pEvent != NULL) && (pEvent->IsValid()))
	{
		int nInsertIndex;
		int nIndex = Find(pEvent, &nInsertIndex);
		if ((nIndex == -1) || (!m_fNoDuplicates))
		{
			TRY
			{	
				InsertAt(nInsertIndex, pEvent);
				fInserted = TRUE;
			}
			END_TRY
		}
	}
	
	// If the event was not inserted for any reason, then we delete it.
	if (!fInserted)
	{
		delete pEvent;
		pEvent = NULL;
	}
}

// Delete an event.
void CEventArray::Delete(CEvent* pEvent)
{
	int nIndex = Find(pEvent);
	if (nIndex != -1)
	{
		DeleteAt(nIndex);
	}
}

// Search for an event.
int CEventArray::Find(CEvent* pFindEvent, int* pnInsertPosition /*=NULL*/)
{
	int nIndex = -1;

	// Perform a binary search for the event. If the event is not found,
	// then -1 is returned.
	int nMin = 0;
	int nMax = GetSize();

	// Remember first known index of the event which is greater than the search event (end of array).
	int nPosition = nMax;

	while (nMin < nMax)
	{
		// Split the search space (elements are 4 bytes, so no overflow).
		int nTry = (nMin+nMax)/2;

		// Get the event from the array.
		CEvent* pEvent = GetAt(nTry);

		// Compare the events.
		// Note that the number of days and disable time are ignored as well as
		// any case differences between the event type and name.
		int nResult = pEvent->ComparePartial(*pFindEvent);

		// Check for exact match.
		if (nResult == 0)
		{
			// Events are equal.
			nPosition = nTry;
			nIndex = nTry;
			break;
		}

		// Check if current event is greater than search event.
		if (nResult > 0)
		{
			// We know everything from nTry forward is greater.
			nMax = nTry;

			// Record index of event we were less than.
			nPosition = nMax;
		}

		// Check if current event is less than search event.
		if (nResult < 0)
		{
			// We know everything from nTry back is lesser.
			nMin = nTry+1;
		}
	}

	// Save the position where we would insert.
	if (pnInsertPosition != NULL)
	{
		*pnInsertPosition = nPosition;
	}

	return nIndex;
}
