/*
// $Workfile: EVENT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/EVENT.H $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:48   Fred
// Initial revision.
// 
//    Rev 1.1   25 Feb 1997 09:53:36   Fred
// Support for sender events
// 
//    Rev 1.0   20 Feb 1997 13:56:16   Fred
// Initial revision.
*/

class CEvent;
class CEventCheckedListBoxItem;
class CEventCheckedListBox;
class CReminderListBoxItem;
class CReminderListBox;

#ifndef __EVENT_H__
#define __EVENT_H__

#include "inifile.h"
#include "checklb.h"
#include "sortarry.h"
#include "day.h"


#define	CURRENT_EVENTS_SECTION	"CurrentEvents"
#define	DISABLED_EVENTS_SECTION	"DisabledEvents"


/////////////////////////////////////////////////////////////////////////////
// CEventIniFile

class CEventIniFile : public CIniFile
{
public:
	CEventIniFile();
	virtual ~CEventIniFile();
	
public:
//	BOOL GetKey(LPCSTR pszSection, CString& csKey);
};


/////////////////////////////////////////////////////////////////////////////
// CEvent

class CEvent
{
public:
	CEvent();
	CEvent(LPCSTR pszIniString);
	CEvent(LPCSTR pszAddressBookUserName, BOOL m_fIsSenderDatabase, DWORD dwEventTypeId, DWORD dwNameId, int nDays, CDay* pDay = NULL);
	CEvent(const CEvent& Event);
	void CommonConstruct(void);
	virtual ~CEvent();

public:
	BOOL GetEventString(CString& csString) const;
	BOOL GetIniString(CString& csString) const;
	
	const CString& GetAddressBookUserName(void) const
		{ return m_csAddressBookUserName; }
	BOOL GetIsSenderDatabase(void) const
		{ return m_fIsSenderDatabase; }
	BOOL GetIsStandardFormatAddressBook(void) const
		{ return m_fIsStandardFormatAddressBook; }
	DWORD GetEventTypeId(void) const
		{ return m_dwEventTypeId; }
	const CString& GetEventType(void) const
		{ return m_csEventType; }
	DWORD GetNameId(void) const
		{ return m_dwNameId; }
	const CString& GetName(void) const
		{ return m_csName; }
	int GetDays(void) const
		{ return m_nDays; }
	const CString& GetDay(void) const
		{ return m_csDay; }
	const CTimeStamp GetTime(void) const
		{ return m_Time; }

	BOOL IsValid(void) const
		{ return m_fIsValid; }
	void Invalidate(void);
	
	CEvent& operator=(const CEvent& Event);
		
	int ComparePartial(const CEvent& Event);
		
protected:
	CString m_csAddressBookUserName;
	BOOL m_fIsSenderDatabase;
	BOOL m_fIsStandardFormatAddressBook;
	DWORD m_dwEventTypeId;
	CString m_csEventType;
	DWORD m_dwNameId;
	CString m_csName;
	int m_nDays;
	CString m_csDay;
	CTimeStamp m_Time;
	
	BOOL m_fIsValid;
};


//////////////////////////////////////////////////////////////////////////
// CEventArray

class CEventArray : public CPtrArray
{
	INHERIT(CEventArray, CPtrArray)

public:

	// construction and destruction
	CEventArray();
	virtual ~CEventArray();
	
public:

	// Get the number of enabled events.
	int GetEnabledCount(void);

	// Overridden delete methods.
	void DeleteAll(void);
	void DeleteAt(int nIndex, int nCount = 1);
	
	// Add a new event with sorting and duplicate checking.
	virtual void Add(CEvent* pEvent);
	virtual void Delete(CEvent* pEvent);
	virtual int Find(CEvent* pFindEvent, int* pnInsertPosition = NULL);

	CEvent* GetAt(int nIndex) const
		{ return (CEvent*)(INHERITED::GetAt(nIndex)); }
	CEvent*& ElementAt(int nIndex)
		{ return (CEvent*&)(INHERITED::ElementAt(nIndex)); }
	CEvent* operator[](int nIndex) const
		{ return (CEvent*)(INHERITED::GetAt(nIndex)); }
	CEvent*& operator[](int nIndex)
		{ return (CEvent*&)(INHERITED::ElementAt(nIndex)); }
		
	BOOL m_fNoDuplicates;
};

#endif
