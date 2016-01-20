/*
// $Workfile: DBTABS.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/DBTABS.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 4     12/23/98 1:50p Psasse
// Code clean up
// 
// 3     7/11/98 2:48p Jayn
// GetDC didn't have a ReleaseDC.
// 
// 2     5/20/98 4:33p Cboggio
// Remove A-Z tab limit for localization
// Put "Other" and "Empty" into resource file
// 
//    Rev 1.0   14 Aug 1997 15:20:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:04   Fred
// Initial revision.
// 
//    Rev 1.7   31 Jul 1996 11:56:38   Fred
// 16-bit compatibility
// 
//    Rev 1.6   25 Jul 1996 11:45:16   Fred
// Fixed some refresh problems
// 
//    Rev 1.5   19 Jul 1996 17:52:50   Fred
//  
// 
//    Rev 1.4   06 Jul 1996 09:25:18   Fred
// Picture editor and other improvements
// 
//    Rev 1.3   01 Jul 1996 07:30:32   Fred
// Receiver list
// 
//    Rev 1.2   18 Jun 1996 15:20:40   Fred
// Field value list management and editing
// 
//    Rev 1.1   18 Jun 1996 08:24:56   Fred
// More adress book changes
// 
//    Rev 1.0   10 Jun 1996 08:22:32   Fred
// Initial revision.
*/ 

#include "stdafx.h"
#include "dbtabs.h"
#include <limits.h>

#ifdef LOCALIZE
#include "clocale.h"
#define MIN_ALPHAUPPER	(CurrentLocale.MinAlphaUpper())
#define MAX_ALPHAUPPER	(CurrentLocale.MaxAlphaUpper())
#else // if localize not defined then just use hard-coded A-Z
#define MIN_ALPHAUPPER	'A'
#define MAX_ALPHAUPPER	'Z'
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// CTabDescriptionBase
//
// The base class for an object used to describe a tab. This information
// is kept separate from the tab control object itself.

CTabDescriptionBase::CTabDescriptionBase(LPCSTR pszName, DWORD dwNumber)
{
	TRY
	{
		m_csName = pszName;
		m_dwNumber = dwNumber;
	}
	END_TRY
}

CTabDescriptionBase::~CTabDescriptionBase()
{
}


//////////////////////////////////////////////////////////////////////////
// CTabDescriptionString
//
// The object used to describe a tab associated with a string field.

CTabDescriptionString::CTabDescriptionString(LPCSTR pszName, DWORD dwNumber, LPCSTR pszValue) :
	CTabDescriptionBase(pszName, dwNumber)
{
	TRY
	{
		m_csValue = pszValue;
	}
	END_TRY
}

CTabDescriptionString::~CTabDescriptionString()
{
}

//////////////////////////////////////////////////////////////////////////
// CTabDescriptionArray
//
// An array of CTabDescriptionBase (or derived) objects.

CTabDescriptionArray::CTabDescriptionArray()
{
}

CTabDescriptionArray::~CTabDescriptionArray()
{
	RemoveAll();
}
	
void CTabDescriptionArray::RemoveAll()
{
	int i;
	
	for (i = 0; i < GetSize(); i++)
	{
		CTabDescriptionBase* pItem = GetAt(i);
		
		ASSERT(pItem != NULL);
		
		delete pItem;
		SetAt(i, NULL);
	}
	
	CPtrArray::RemoveAll();
}

void CTabDescriptionArray::RemoveAt(int nIndex, int nCount /*=1*/, BOOL fDelete /*=TRUE*/)
{
	if (fDelete)
	{
		for (int i = 0; i < nCount; i++)
		{
			if (((nIndex+i) >= 0) && ((nIndex+i) < GetSize()))
			{
				CTabDescriptionBase* pItem = GetAt(nIndex+i);

				ASSERT(pItem != NULL);

				delete pItem;
				SetAt(nIndex+i, NULL);
			}
		}
	}
	
	CPtrArray::RemoveAt(nIndex, nCount);
}

//////////////////////////////////////////////////////////////////////////
// CDatabaseTabsGeneratorBase
//
// The base class from which all tab generators are derived.

CDatabaseTabsGeneratorBase::CDatabaseTabsGeneratorBase(CFlatFileDatabaseCursor* pCursor, CAddressBookTabControl* pTabs)
{
	m_pDataCursor = pCursor;
	m_pSearchCursor = NULL;
	m_pTabs = pTabs;
}

CDatabaseTabsGeneratorBase::~CDatabaseTabsGeneratorBase()
{
	FreeSearchCursor();
}

// Create a "search" cursor which references the same database and
// index as the current data cursor.
void CDatabaseTabsGeneratorBase::CreateSearchCursor(void)
{
	// Delete any current search cursor.
	FreeSearchCursor();
	ASSERT(m_pSearchCursor == NULL);

	ASSERT(m_pDataCursor != NULL);
	if (m_pDataCursor != NULL)
	{
		// Attempt to create the search cursor.
		TRY
		{
			// Allocate the search cursor object.
			m_pSearchCursor = new CFlatFileDatabaseCursor;

			// Initialize the search cursor from the current database cursor.
			m_pSearchCursor->SetCursor(*m_pDataCursor);
		}
		CATCH_ALL(e)
		{
			FreeSearchCursor();
		}
		END_CATCH_ALL
	}

	ASSERT(m_pSearchCursor != NULL);
}

// Free any allocated "search" cursor.
void CDatabaseTabsGeneratorBase::FreeSearchCursor(void)
{
	delete m_pSearchCursor;
	m_pSearchCursor = NULL;
}

// rebuild all tab descriptions from database
void CDatabaseTabsGeneratorBase::UpdateTabs(void)
{
	// This is a highly abstracted function which calls a lot of
	// virtual functions to do the work.

	// get rid of any current descriptions
	m_Descriptions.RemoveAll();

	// make sure we have a cursor we can use for searching
	CreateSearchCursor();
	if (m_pSearchCursor != NULL)
	{
		// Do any necessary initialization.
		if (UpdateTabsStart())
		{
			// Set the current key to empty.
			SetKeyToEmpty();

			// Search for keys until we have exceeded the maximum key that can be
			// represented directly on a tab.
			while (!KeyGreaterThanMaximum())
			{
				// Create a database search key for the current tab key.
				MakeSearchKey();

				// Find the nearest database record which can be represented by the tab.
				FindNearest();

				// Retrieve the key value from the record that was found.
				if (!GetValue())
				{
					break;
				}

				// If the tab key was greater than the record which was found,
				// then basically no record was found and we must be done.
				if (SearchGreaterThanValue())
				{
					break;
				}

				// If the value of the record matches the current tab key,
				// then add the tab and advance the tab key.
				if (SearchEqualToValue())
				{
					Add(KeyName());
					IncrementKey();
				}
				else
				{
					// The record that was found belongs on a greater tab.
					// Skip to the tab represented by the record and add it.
					SetKeyToValue();

					// If the record is less than the minimum key, then
					// use the "less" name.
					if (ValueLessThanMinimum())
					{
						Add(LessName());
						SetKeyToMinimum();
					}

					else
					{
						// If the record is greater than the maximum key, then
						// use the "Greater" name.
						if (ValueGreaterThanMaximum())
						{
							Add(GreaterName());
							break;
						}
					}
				}
			}

			UpdateTabsFinish();
		}
	}

	// Show the new tabs.
	SetTabNames();
}

// perform any actions necessary at the start of UpdateTabs()
BOOL CDatabaseTabsGeneratorBase::UpdateTabsStart(void)
{
	BOOL fResult = FALSE;

	m_pSearchField = NULL;

	TRY
	{
		// Initialize member variables.
		m_dwSequenceNumber = (DWORD)-1;
		m_csKeyName.Empty();
		m_csLessName.Empty();
		m_csGreaterName.Empty();

		ASSERT(m_pSearchCursor != NULL);
		if (m_pSearchCursor != NULL)
		{
			// Use the first field associated with the index as the search field.
			DWORD dwFieldNumber = m_pSearchCursor->GetIndexFieldNumber(m_pSearchCursor->GetIndex(), 0);
			ASSERT(dwFieldNumber != (DWORD)-1);
			if (dwFieldNumber != -1)
			{
				m_pSearchField = m_pSearchCursor->GetField(dwFieldNumber);
				ASSERT(m_pSearchField != NULL);
				if (m_pSearchField != NULL)
				{
					fResult = TRUE;
				}
			}
		}
	}
	END_TRY

	return fResult;
}

// add the desription of the current tab key to the end of the description array
void CDatabaseTabsGeneratorBase::Add(LPCSTR pszName)
{
	TRY
	{
		// Make the description.
		CTabDescriptionBase*	pDescription = Description(pszName);
		ASSERT(pDescription != NULL);
		if (pDescription != NULL)
		{
			m_Descriptions.Add(pDescription);
		}
	}
	END_TRY
}

// perform any actions necessary at the end of UpdateTabs()
void CDatabaseTabsGeneratorBase::UpdateTabsFinish(void)
{
}

// select the tab which represents the record at the current cursor position
void CDatabaseTabsGeneratorBase::UpdateCurrentTab(void)
{
	if (m_pTabs != NULL)
	{
		int nTabIndex = CurrentRecordTabIndex();
//		ASSERT(nTabIndex != -1);
		if (nTabIndex != -1)
		{
			m_pTabs->SetCurSel(nTabIndex);
		}
	}
}

// move the data cursor to the first record represented by a given tab
void CDatabaseTabsGeneratorBase::UpdateCurrentRecord(int nTabIndex)
{
	TRY
	{
		// Validate a bunch of stuff.
		if (m_pDataCursor != NULL)
		{
			ASSERT(m_pTabs != NULL);
			if (m_pTabs != NULL)
			{
				ASSERT((nTabIndex >= 0) && (nTabIndex < m_pTabs->GetItemCount()));
				if ((nTabIndex >= 0) && (nTabIndex < m_pTabs->GetItemCount()))
				{
					// Use the first field associated with the index as the search field.
					DWORD dwFieldNumber = m_pDataCursor->GetIndexFieldNumber(m_pDataCursor->GetIndex(), 0);
					ASSERT(dwFieldNumber != (DWORD)-1);
					if (dwFieldNumber != -1)
					{
						CFlatFileDatabaseField* pField = m_pDataCursor->GetField(dwFieldNumber);
						ASSERT(pField != NULL);
						if (pField != NULL)
						{
							// Set up the cursor for searching.
							m_pDataCursor->New();
							SetSearchField(pField, nTabIndex);
							m_pDataCursor->Find();
						}
					}
				}
			}
		}
	}
	END_TRY
}

// move the data cursor to the first record represented by a given tab
int CDatabaseTabsGeneratorBase::CurrentRecordTabIndex(void)
{
	int nTabIndex = -1;

	TRY
	{
		// Validate some things.
		if (m_pDataCursor != NULL)
		{
			// Get the field to use for sorting.
			DWORD dwFieldNumber = m_pDataCursor->GetIndexFieldNumber(m_pDataCursor->GetIndex(), 0);
			ASSERT(dwFieldNumber != (DWORD)-1);
			if (dwFieldNumber != -1)
			{
				CFlatFileDatabaseField* pField = m_pDataCursor->GetField(dwFieldNumber);
				ASSERT(pField != NULL);
				if (pField != NULL)
				{
					// Get the field value.
					GetFieldValue(pField);

					// Search through the tabs to find the one which best represents the value.
					int nCount = m_Descriptions.GetSize();
					for (nTabIndex = 0; nTabIndex < nCount; nTabIndex++)
					{
						// Check the current tab description.
						if (CompareFieldValue(nTabIndex))
						{
							break;
						}
					}
					nTabIndex--;
				}
			}
		}
	}
	END_TRY

	return nTabIndex;
}

// set the text in the tab control tabs to the name strings in the descriptions
void CDatabaseTabsGeneratorBase::SetTabNames(void)
{
	if (m_pTabs != NULL)
	{
		// Setting the tab names cases a redraw, so check if they're equal first.
		if (!TabNamesEqual())
		{
			// Copy the tabs names from the descriptions to the tab control.
			m_pTabs->DeleteAllTabs();
			
#ifdef WIN32
			TC_ITEM Item;
			Item.mask = TCIF_TEXT;
//			Item.lpReserved1 = 0;
//			Item.lpReserved2 = 0;
			Item.pszText = NULL;
			Item.cchTextMax = 0;
			Item.iImage = -1;
			Item.lParam = 0;

			CFont* pFont = m_pTabs->GetFont();
			if (pFont != NULL)
			{
				CDC* pDC = m_pTabs->GetDC();
				if (pDC != NULL)
				{
					if (pDC->SelectObject(pFont) != NULL)
					{
						CSize czExtent(0, 0);

						int nCount = m_Descriptions.GetSize();
						for (int i = 0; i < nCount; i++)
						{
							CTabDescriptionBase* pDescription = m_Descriptions.GetAt(i);
							ASSERT(pDescription != NULL);
							if (pDescription != NULL)
							{
								Item.pszText = (LPSTR)(pDescription->GetName());	// casting away const!

								CSize czText = pDC->GetTextExtent(Item.pszText);
								if (czText.cx > czExtent.cx)	czExtent.cx = czText.cx;
								if (czText.cy > czExtent.cy)	czExtent.cy = czText.cy;

								m_pTabs->InsertItem(i, &Item);
							}
						}

						czExtent.cx += 4+4;
						czExtent.cy += 4+4;

						m_pTabs->SetItemSize(czExtent);
					}
					m_pTabs->ReleaseDC(pDC);
				}
			}
#else
			int nCount = m_Descriptions.GetSize();
			for (int i = 0; i < nCount; i++)
			{
				CTabDescriptionBase* pDescription = m_Descriptions.GetAt(i);
				ASSERT(pDescription != NULL);
				if (pDescription != NULL)
				{
					m_pTabs->AddTab(pDescription->GetName());
				}
			}
#endif
		}
	}
}

// check if the tab names have already been set (used for redraw optimization)
BOOL CDatabaseTabsGeneratorBase::TabNamesEqual(void)
{
	BOOL fResult = FALSE;

	if (m_pTabs != NULL)
	{
		// Start by checking the tab counts.
		int nCount = m_Descriptions.GetSize();
		if (m_pTabs->GetItemCount() == nCount)
		{
			// The counts match, check the actual items.
			for (int i = 0; i < nCount; i++)
			{
				// Get the tab name.
				CString csTabName;
				m_pTabs->GetTabName(i, csTabName);

				// Get the current tab description.
				CTabDescriptionBase* pDescription = m_Descriptions.GetAt(i);
				ASSERT(pDescription != NULL);
				if (pDescription == NULL)
				{
					break;
				}

				// Check if strings are the same.
				if (strcmp(csTabName, pDescription->GetName()) != 0)
				{
					// Tab strings are different.
					break;
				}
			}

			// If we compared all of the items, and they all matched, return TRUE.
			if (i == nCount)
			{
				fResult = TRUE;
			}
		}
	}

	return fResult;
};

//////////////////////////////////////////////////////////////////////////
// CDatabaseTabsGeneratorAlpha
//
// The tab generator class for the Alpha1, Alpha2, ... format types.

CDatabaseTabsGeneratorAlpha::CDatabaseTabsGeneratorAlpha(CFlatFileDatabaseCursor* pCursor, CAddressBookTabControl* pTabs, UINT nLength) :
	CDatabaseTabsGeneratorBase(pCursor, pTabs)
{
	m_nLength = nLength;
}

CDatabaseTabsGeneratorAlpha::~CDatabaseTabsGeneratorAlpha()
{
}

// perform any actions necessary at the start of UpdateTabs()
BOOL CDatabaseTabsGeneratorAlpha::UpdateTabsStart(void)
{
	BOOL fResult = CDatabaseTabsGeneratorBase::UpdateTabsStart();

	if (fResult)
	{
		m_csKey.Empty();
		m_csSearch.Empty();
		m_csValue.Empty();
	}

	return fResult;
}

// set the current tab key to empty to search for empty fields
void CDatabaseTabsGeneratorAlpha::SetKeyToEmpty(void)
{
	TRY
	{
		m_csKey.Empty();
	}
	END_TRY
}

// check if the current tab key is greater than its maximum value
BOOL CDatabaseTabsGeneratorAlpha::KeyGreaterThanMaximum(void)
{
	return (m_csKey.GetLength() > 0) && ((unsigned) m_csKey[0] > (unsigned) MAX_ALPHAUPPER);
}

// create a database search key from the current tab key
void CDatabaseTabsGeneratorAlpha::MakeSearchKey(void)
{
	TRY
	{
		// Strip off trailing A's (Go A's!)
		int nNewLength = m_csKey.GetLength();
		while ((nNewLength > 1) && (m_csKey[nNewLength-1] == MIN_ALPHAUPPER))
		{
			nNewLength--;
		}

		m_csSearch = m_csKey.Left(nNewLength);
	}
	END_TRY
}

// search for the record which is nearest (greater than or equal to) to the current search key
void CDatabaseTabsGeneratorAlpha::FindNearest(void)
{
	ASSERT(m_pSearchCursor != NULL);
	ASSERT(m_pSearchField != NULL);

	m_pSearchCursor->New();
	m_pSearchField->SetAsString(m_csSearch);
	m_pSearchCursor->Find();
	m_dwSequenceNumber = m_pSearchCursor->GetPosition();
}

// retrieve and normalize for comparison the field value
BOOL CDatabaseTabsGeneratorAlpha::GetValue(void)
{
	BOOL fResult = FALSE;

	ASSERT(m_pSearchCursor != NULL);
	ASSERT(m_pSearchField != NULL);

	if (!m_pSearchCursor->AtEOF())
	{
		TRY
		{
			// Get field value.
			m_csValue = m_pSearchField->GetAsString();

			// Trim to same number of characters as search key (unless search key is empty.)
			int nLength = m_csSearch.GetLength();
			if (nLength > 0)
			{
				m_csValue = m_csValue.Left(m_nLength);
			}

			// Uppercase the result for comparison purposes.
			m_csValue.MakeUpper();

			fResult = TRUE;
		}
		END_TRY
	}

	return fResult;
}

// check if the value searched for is greater than the value found
BOOL CDatabaseTabsGeneratorAlpha::SearchGreaterThanValue(void)
{
	return m_csSearch > m_csValue;
}

// check if the value searched for is "equal" to what was found
BOOL CDatabaseTabsGeneratorAlpha::SearchEqualToValue(void)
{
	return (m_csValue >= m_csSearch) && (m_csValue <= m_csKey);
}

// build a name string for the current tab key
LPCSTR CDatabaseTabsGeneratorAlpha::KeyName(void)
{
	TRY
	{
		m_csKeyName = m_csKey;
		if (m_csKeyName.IsEmpty())
		{
//			m_csKeyName = "Empty";
			m_csKeyName = " ";
		}
	}
	END_TRY

	return m_csKeyName;
}

// build a tab name string that means "less than minimum"
LPCSTR CDatabaseTabsGeneratorAlpha::LessName(void)
{
	TRY
	{
		if (m_csLessName.IsEmpty())
		{
//			m_csLessName = "Other";
			m_csLessName.LoadString(IDS_DBTABS_OTHER);
		}
	}
	END_TRY

	return m_csLessName;
}

// build a tab name string that means "greater than maximum"
LPCSTR CDatabaseTabsGeneratorAlpha::GreaterName(void)
{
	TRY
	{
		if (m_csGreaterName.IsEmpty())
		{
//			m_csGreaterName = "Other";
			m_csGreaterName.LoadString(IDS_DBTABS_OTHER);
		}
	}
	END_TRY

	return m_csGreaterName;
}

// create a drescription object to describe the current tab
CTabDescriptionBase* CDatabaseTabsGeneratorAlpha::Description(LPCSTR pszName)
{
	CTabDescriptionBase* pDescription = NULL;

	TRY
	{
		pDescription = new CTabDescriptionString(pszName, m_dwSequenceNumber, m_csValue);
	}
	END_TRY

	return pDescription;
}

// set the key to its minimum non empty search value
void CDatabaseTabsGeneratorAlpha::SetKeyToSearchMinimum(void)
{
	TRY
	{
		m_csKey = "!";
	}
	END_TRY
}

// increment the current tab key to its next value
void CDatabaseTabsGeneratorAlpha::IncrementKey(void)
{
	// Make sure we have room to increment.
	if (!KeyGreaterThanMaximum())
	{
		if (m_csKey.IsEmpty())
		{
			// Empty key goes to minimum search key.
			SetKeyToSearchMinimum();
		}
		else if ((unsigned) m_csKey[0] < (unsigned) MIN_ALPHAUPPER)
		{
			// Non alpha key goes to minimum alpha key.
			SetKeyToMinimum();
		}
		else
		{
			// Advance key (A-Z counter with m_nLength digits).
			int i = m_nLength-1;

			// Advance last "digit".
			m_csKey.SetAt(i, m_csKey.GetAt(i)+1);

			// Handle any "carry".
			while ((i > 0) && (m_csKey[i] == MAX_ALPHAUPPER))
			{
				m_csKey.SetAt(i, MIN_ALPHAUPPER);
				i--;
				m_csKey.SetAt(i, m_csKey.GetAt(i)+1);
			}
		}
	}
}

// set the tab key to the value from the field
void CDatabaseTabsGeneratorAlpha::SetKeyToValue(void)
{
	TRY
	{
		// Allocate a buffer for building the key.
		m_csKey = CString(MIN_ALPHAUPPER, m_nLength);

		// Build the key.
		int nCount = m_csValue.GetLength();
		if ((UINT)nCount > m_nLength)
		{
			nCount = (int)m_nLength;
		}
		for (int i = 0; i < nCount; i++)
		{
			m_csKey.SetAt(i, m_csValue.GetAt(i));

			if ((unsigned) m_csKey[i] < (unsigned) MIN_ALPHAUPPER)
			{
				m_csKey.SetAt(i, MIN_ALPHAUPPER);
			}
			if ((unsigned) m_csKey[i] > (unsigned) MAX_ALPHAUPPER)
			{
				m_csKey.SetAt(i, MAX_ALPHAUPPER);
			}
		}
	}
	END_TRY
}

// set the tab key to it minimum non-empty value
void CDatabaseTabsGeneratorAlpha::SetKeyToMinimum(void)
{
	TRY
	{
		m_csKey = CString(MIN_ALPHAUPPER, m_nLength);
	}
	END_TRY
}

// check if the field value is less than the minimum key
BOOL CDatabaseTabsGeneratorAlpha::ValueLessThanMinimum(void)
{
	return (m_csValue.GetLength() > 0) && ((unsigned) m_csValue[0] < (unsigned) MIN_ALPHAUPPER);
}

// check if the field value is greater than the maximum key
BOOL CDatabaseTabsGeneratorAlpha::ValueGreaterThanMaximum(void)
{
	return (m_csValue.GetLength() > 0) && ((unsigned) m_csValue[0] > (unsigned) MAX_ALPHAUPPER);
}

// perform any actions necessary at the end of UpdateTabs()
void CDatabaseTabsGeneratorAlpha::UpdateTabsFinish(void)
{
	CDatabaseTabsGeneratorBase::UpdateTabsFinish();
}

// set the search field for UpdateCurrentRecord()
void CDatabaseTabsGeneratorAlpha::SetSearchField(CFlatFileDatabaseField* pField, int nTabIndex)
{
	ASSERT(pField != NULL);
	if (pField != NULL)
	{
		ASSERT((nTabIndex >= 0) && (nTabIndex < m_Descriptions.GetSize()));
		if ((nTabIndex >= 0) && (nTabIndex < m_Descriptions.GetSize()))
		{
			CTabDescriptionString* pDescription = (CTabDescriptionString*)(m_Descriptions[nTabIndex]);
			ASSERT(pDescription != NULL);
			if (pDescription != NULL)
			{
				pField->SetAsString(pDescription->GetValue());
			}
		}
	}
}

// get the current value of a field to use during a call to CurrentRecordTabIndex()
void CDatabaseTabsGeneratorAlpha::GetFieldValue(CFlatFileDatabaseField* pField)
{
	TRY
	{
		ASSERT(pField != NULL);
		if (pField != NULL)
		{
			m_csFieldValue = pField->GetAsString();
			m_csFieldValue.MakeUpper();
		}
	}
	END_TRY
}

// compare the current field value with the value of the given tab description
BOOL CDatabaseTabsGeneratorAlpha::CompareFieldValue(int nDescription)
{
	BOOL fResult = FALSE;

	ASSERT((nDescription >= 0) && (nDescription < m_Descriptions.GetSize()));
	if ((nDescription >= 0) && (nDescription < m_Descriptions.GetSize()))
	{
		CTabDescriptionString* pDescription = (CTabDescriptionString*)(m_Descriptions[nDescription]);
		ASSERT(pDescription != NULL);
		if (pDescription != NULL)
		{
			fResult  = strcmp(m_csFieldValue, pDescription->GetValue()) < 0;
		}
	}

	return fResult;
}

//////////////////////////////////////////////////////////////////////////
// CDatabaseTabsGeneratorAlphaList
//
// The tab generator class for the AlphaList format types.

CDatabaseTabsGeneratorAlphaList::CDatabaseTabsGeneratorAlphaList(CFlatFileDatabaseCursor* pCursor, CAddressBookTabControl* pTabs, CSortedPtrArray* pList) :
	CDatabaseTabsGeneratorBase(pCursor, pTabs)
{
	m_pList = pList;
}

CDatabaseTabsGeneratorAlphaList::~CDatabaseTabsGeneratorAlphaList()
{
}

// perform any actions necessary at the start of UpdateTabs()
BOOL CDatabaseTabsGeneratorAlphaList::UpdateTabsStart(void)
{
	BOOL fResult = CDatabaseTabsGeneratorBase::UpdateTabsStart();

	if (fResult)
	{
		m_nKey = -1;
		m_csSearch.Empty();
		m_csValue.Empty();
	}

	return fResult;
}

// set the current tab key to empty to search for empty fields
void CDatabaseTabsGeneratorAlphaList::SetKeyToEmpty(void)
{
	m_nKey = -1;
}

// check if the current tab key is greater than its maximum value
BOOL CDatabaseTabsGeneratorAlphaList::KeyGreaterThanMaximum(void)
{
	return m_nKey >= m_pList->GetSize();
}

// create a database search key from the current tab key
void CDatabaseTabsGeneratorAlphaList::MakeSearchKey(void)
{
	TRY
	{
		if (m_nKey < 0)
		{
			m_csSearch.Empty();
		}
		else
		{
			m_csSearch = m_pList->ElementAt(m_nKey)->GetAsString();
			m_csSearch.MakeUpper();
		}
	}
	END_TRY
}

// search for the record which is nearest (greater than or equal to) to the current search key
void CDatabaseTabsGeneratorAlphaList::FindNearest(void)
{
	ASSERT(m_pSearchCursor != NULL);
	ASSERT(m_pSearchField != NULL);

	m_pSearchCursor->New();
	m_pSearchField->SetAsString(m_csSearch);
	m_pSearchCursor->Find();
	m_dwSequenceNumber = m_pSearchCursor->GetPosition();
}

// retrieve and normalize for comparison the field value
BOOL CDatabaseTabsGeneratorAlphaList::GetValue(void)
{
	BOOL fResult = FALSE;

	ASSERT(m_pSearchCursor != NULL);
	ASSERT(m_pSearchField != NULL);

	if (!m_pSearchCursor->AtEOF())
	{
		TRY
		{
			// Get field value.
			m_csValue = m_pSearchField->GetAsString();

			// Uppercase the result for comparison purposes.
			m_csValue.MakeUpper();

			fResult = TRUE;
		}
		END_TRY
	}

	return fResult;
}

// check if the value searched for is greater than the value found
BOOL CDatabaseTabsGeneratorAlphaList::SearchGreaterThanValue(void)
{
	return m_csSearch > m_csValue;
}

// check if the value searched for is "equal" to what was found
BOOL CDatabaseTabsGeneratorAlphaList::SearchEqualToValue(void)
{
	return m_csValue == m_csSearch;
}

// build a name string for the current tab key
LPCSTR CDatabaseTabsGeneratorAlphaList::KeyName(void)
{
	TRY
	{
		if (m_nKey < 0)
		{
//			m_csKeyName = "Empty";
			m_csKeyName.LoadString(IDS_DBTABS_EMPTY);
		}
		else
		{
			m_csKeyName = m_pList->ElementAt(m_nKey)->GetAsString();
		}
	}
	END_TRY

	return m_csKeyName;
}

// build a tab name string that means "less than minimum"
LPCSTR CDatabaseTabsGeneratorAlphaList::LessName(void)
{
	TRY
	{
		if (m_csLessName.IsEmpty())
		{
//			m_csLessName = "Other";
			m_csLessName.LoadString(IDS_DBTABS_OTHER);
		}
	}
	END_TRY

	return m_csLessName;
}

// build a tab name string that means "greater than maximum"
LPCSTR CDatabaseTabsGeneratorAlphaList::GreaterName(void)
{
	TRY
	{
		if (m_csGreaterName.IsEmpty())
		{
//			m_csGreaterName = "Other";
			m_csGreaterName.LoadString(IDS_DBTABS_OTHER);
		}
	}
	END_TRY

	return m_csGreaterName;
}

// create a drescription object to describe the current tab
CTabDescriptionBase* CDatabaseTabsGeneratorAlphaList::Description(LPCSTR pszName)
{
	CTabDescriptionBase* pDescription = NULL;

	TRY
	{
		pDescription = new CTabDescriptionString(pszName, m_dwSequenceNumber, m_csValue);
	}
	END_TRY

	return pDescription;
}

// get the index of the list string which is less than or equal to the given string
int CDatabaseTabsGeneratorAlphaList::GetStringIndex(LPCSTR pszString)
{
	int nIndex;

	for (nIndex = 0; nIndex < m_pList->GetSize(); nIndex++)
	{
		if (stricmp(pszString, m_pList->ElementAt(nIndex)->GetAsString()) < 0)
		{
			break;
		}
	}

	return nIndex-1;
}

// increment the current tab key to its next value
void CDatabaseTabsGeneratorAlphaList::IncrementKey(void)
{
	// Make sure we have room to increment.
	if (!KeyGreaterThanMaximum())
	{
		m_nKey++;
	}
}

// set the tab key to the value from the field
void CDatabaseTabsGeneratorAlphaList::SetKeyToValue(void)
{
	m_nKey = GetStringIndex(m_csValue);
	if (m_nKey >= 0)
	{
		if (stricmp(m_csValue, m_pList->ElementAt(m_nKey)->GetAsString()) > 0)
		{
			m_nKey++;
		}
	}
}

// set the tab key to it minimum non-empty value
void CDatabaseTabsGeneratorAlphaList::SetKeyToMinimum(void)
{
	m_nKey = 0;
}

// check if the field value is less than the minimum key
BOOL CDatabaseTabsGeneratorAlphaList::ValueLessThanMinimum(void)
{
	return GetStringIndex(m_csValue) < 0;
}

// check if the field value is greater than the maximum key
BOOL CDatabaseTabsGeneratorAlphaList::ValueGreaterThanMaximum(void)
{
	return GetStringIndex(m_csValue) >= m_pList->GetSize();
}

// perform any actions necessary at the end of UpdateTabs()
void CDatabaseTabsGeneratorAlphaList::UpdateTabsFinish(void)
{
	CDatabaseTabsGeneratorBase::UpdateTabsFinish();
}

// set the search field for UpdateCurrentRecord()
void CDatabaseTabsGeneratorAlphaList::SetSearchField(CFlatFileDatabaseField* pField, int nTabIndex)
{
	ASSERT(pField != NULL);
	if (pField != NULL)
	{
		ASSERT((nTabIndex >= 0) && (nTabIndex < m_Descriptions.GetSize()));
		if ((nTabIndex >= 0) && (nTabIndex < m_Descriptions.GetSize()))
		{
			CTabDescriptionString* pDescription = (CTabDescriptionString*)(m_Descriptions[nTabIndex]);
			ASSERT(pDescription != NULL);
			if (pDescription != NULL)
			{
				pField->SetAsString(pDescription->GetValue());
			}
		}
	}
}

// get the current value of a field to use during a call to CurrentRecordTabIndex()
void CDatabaseTabsGeneratorAlphaList::GetFieldValue(CFlatFileDatabaseField* pField)
{
	TRY
	{
		ASSERT(pField != NULL);
		if (pField != NULL)
		{
			m_csFieldValue = pField->GetAsString();
			m_csFieldValue.MakeUpper();
		}
	}
	END_TRY
}

// compare the current field value with the value of the given tab description
BOOL CDatabaseTabsGeneratorAlphaList::CompareFieldValue(int nDescription)
{
	BOOL fResult = FALSE;

	ASSERT((nDescription >= 0) && (nDescription < m_Descriptions.GetSize()));
	if ((nDescription >= 0) && (nDescription < m_Descriptions.GetSize()))
	{
		CTabDescriptionString* pDescription = (CTabDescriptionString*)(m_Descriptions[nDescription]);
		ASSERT(pDescription != NULL);
		if (pDescription != NULL)
		{
			fResult  = strcmp(m_csFieldValue, pDescription->GetValue()) < 0;
		}
	}

	return fResult;
}

//////////////////////////////////////////////////////////////////////////
// CDatabaseTabsGeneratorIndexList
//
// The tab generator class for the IndexList format types.

CDatabaseTabsGeneratorIndexList::CDatabaseTabsGeneratorIndexList(CFlatFileDatabaseCursor* pCursor, CAddressBookTabControl* pTabs, CFlatFileValueListManager* pListManager, CFlatFileIndexDescription* pIndex) :
	CDatabaseTabsGeneratorAlphaList(pCursor, pTabs, NULL)
{
	m_pListManager = pListManager;
	m_pIndex = pIndex;
	ASSERT(m_pListManager != NULL);
	ASSERT(m_pIndex != NULL);
}

CDatabaseTabsGeneratorIndexList::~CDatabaseTabsGeneratorIndexList()
{
}

// perform any actions necessary at the start of UpdateTabs()
BOOL CDatabaseTabsGeneratorIndexList::UpdateTabsStart(void)
{
	BOOL fResult = CDatabaseTabsGeneratorAlphaList::UpdateTabsStart();

	// Ask the list manager for the list for the index.
	if (fResult)
	{
		fResult = FALSE;

		// Forget any existing list... don't delete since
		// it's owned by the list manager.
		m_pList = NULL;

		if ((m_pListManager != NULL) && (m_pIndex != NULL))
		{
			// Get the new list.
			m_pList = m_pListManager->GetListForIndex(m_pIndex);

			// Make sure we got it.
			if (m_pList != NULL)
			{
				fResult = TRUE;
			}
		}
	}

	ASSERT(m_pList != NULL);
	return fResult;
}

//////////////////////////////////////////////////////////////////////////
// CDatabaseTabsGeneratorZip
//
// The tab generator class for the Zip1, Zip2, ... format types.

CDatabaseTabsGeneratorZip::CDatabaseTabsGeneratorZip(CFlatFileDatabaseCursor* pCursor, CAddressBookTabControl* pTabs, UINT nLength) :
	CDatabaseTabsGeneratorBase(pCursor, pTabs)
{
	m_nLength = nLength;
}

CDatabaseTabsGeneratorZip::~CDatabaseTabsGeneratorZip()
{
}

// perform any actions necessary at the start of UpdateTabs()
BOOL CDatabaseTabsGeneratorZip::UpdateTabsStart(void)
{
	BOOL fResult = CDatabaseTabsGeneratorBase::UpdateTabsStart();

	if (fResult)
	{
		m_csKey.Empty();
		m_csSearch.Empty();
		m_csValue.Empty();
	}

	return fResult;
}

// set the current tab key to empty to search for empty fields
void CDatabaseTabsGeneratorZip::SetKeyToEmpty(void)
{
	TRY
	{
		m_csKey.Empty();
	}
	END_TRY
}

// check if the current tab key is greater than its maximum value
BOOL CDatabaseTabsGeneratorZip::KeyGreaterThanMaximum(void)
{
	return (m_csKey.GetLength() > 0) && (m_csKey[0] > '9');
}

// create a database search key from the current tab key
void CDatabaseTabsGeneratorZip::MakeSearchKey(void)
{
	TRY
	{
		// Strip off trailing zeros.
		int nNewLength = m_csKey.GetLength();
		while ((nNewLength > 1) && (m_csKey[nNewLength-1] == '0'))
		{
			nNewLength--;
		}

		m_csSearch = m_csKey.Left(nNewLength);
	}
	END_TRY
}

// search for the record which is nearest (greater than or equal to) to the current search key
void CDatabaseTabsGeneratorZip::FindNearest(void)
{
	ASSERT(m_pSearchCursor != NULL);
	ASSERT(m_pSearchField != NULL);

	m_pSearchCursor->New();
	m_pSearchField->SetAsString(m_csSearch);
	m_pSearchCursor->Find();
	m_dwSequenceNumber = m_pSearchCursor->GetPosition();
}

// retrieve and normalize for comparison the field value
BOOL CDatabaseTabsGeneratorZip::GetValue(void)
{
	BOOL fResult = FALSE;

	ASSERT(m_pSearchCursor != NULL);
	ASSERT(m_pSearchField != NULL);

	if (!m_pSearchCursor->AtEOF())
	{
		TRY
		{
			// Get field value.
			m_csValue = m_pSearchField->GetAsString();

			// Trim to same number of characters as search key (unless search key is empty.)
			int nLength = m_csSearch.GetLength();
			if (nLength > 0)
			{
				m_csValue = m_csValue.Left(m_nLength);
			}

			// Uppercase the result for comparison purposes.
			m_csValue.MakeUpper();

			fResult = TRUE;
		}
		END_TRY
	}

	return fResult;
}

// check if the value searched for is greater than the value found
BOOL CDatabaseTabsGeneratorZip::SearchGreaterThanValue(void)
{
	return m_csSearch > m_csValue;
}

// check if the value searched for is "equal" to what was found
BOOL CDatabaseTabsGeneratorZip::SearchEqualToValue(void)
{
	return (m_csValue >= m_csSearch) && (m_csValue <= m_csKey);
}

// build a name string for the current tab key
LPCSTR CDatabaseTabsGeneratorZip::KeyName(void)
{
	TRY
	{
		if (m_csKey.IsEmpty())
		{
//			m_csKeyName = "Empty";
			m_csKeyName.LoadString(IDS_DBTABS_EMPTY);
		}
		else
		{
			m_csKeyName = m_csKey;
			if (m_csKeyName.GetLength() < 5)
			{
				m_csKeyName += CString('x', 5-m_csKeyName.GetLength());
			}
		}
	}
	END_TRY

	return m_csKeyName;
}

// build a tab name string that means "less than minimum"
LPCSTR CDatabaseTabsGeneratorZip::LessName(void)
{
	TRY
	{
		if (m_csLessName.IsEmpty())
		{
//			m_csLessName = "Other";
			m_csLessName.LoadString(IDS_DBTABS_OTHER);
		}
	}
	END_TRY

	return m_csLessName;
}

// build a tab name string that means "greater than maximum"
LPCSTR CDatabaseTabsGeneratorZip::GreaterName(void)
{
	TRY
	{
		if (m_csGreaterName.IsEmpty())
		{
//			m_csGreaterName = "Other";
			m_csGreaterName.LoadString(IDS_DBTABS_OTHER);
		}
	}
	END_TRY

	return m_csGreaterName;
}

// create a drescription object to describe the current tab
CTabDescriptionBase* CDatabaseTabsGeneratorZip::Description(LPCSTR pszName)
{
	CTabDescriptionBase* pDescription = NULL;

	TRY
	{
		pDescription = new CTabDescriptionString(pszName, m_dwSequenceNumber, m_csValue);
	}
	END_TRY

	return pDescription;
}

// increment the current tab key to its next value
void CDatabaseTabsGeneratorZip::IncrementKey(void)
{
	// Make sure we have room to increment.
	if (!KeyGreaterThanMaximum())
	{
		if (m_csKey.IsEmpty())
		{
			// Empty key goes to minimum search key.
			SetKeyToMinimum();
		}
		else
		{
			// Advance key (0-9 counter with m_nLength digits).
			int i = m_nLength-1;

			// Advance last "digit".
			m_csKey.SetAt(i, m_csKey.GetAt(i)+1);

			// Handle any "carry".
			while ((i > 0) && (m_csKey[i] == '9'))
			{
				m_csKey.SetAt(i, '0');
				i--;
				m_csKey.SetAt(i, m_csKey.GetAt(i)+1);
			}
		}
	}
}

// set the tab key to the value from the field
void CDatabaseTabsGeneratorZip::SetKeyToValue(void)
{
	TRY
	{
		// Allocate a buffer for building the key.
		m_csKey = CString('0', m_nLength);

		// Build the key.
		int nCount = m_csValue.GetLength();
		if ((UINT)nCount > m_nLength)
		{
			nCount = (int)m_nLength;
		}
		for (int i = 0; i < nCount; i++)
		{
			m_csKey.SetAt(i, m_csValue.GetAt(i));
			if (m_csKey[i] < '0')
			{
				m_csKey.SetAt(i, '0');
			}
			if (m_csKey[i] > '9')
			{
				m_csKey.SetAt(i, '9');
			}
		}
	}
	END_TRY
}

// set the tab key to it minimum non-empty value
void CDatabaseTabsGeneratorZip::SetKeyToMinimum(void)
{
	TRY
	{
		m_csKey = CString('0', m_nLength);
	}
	END_TRY
}

// check if the field value is less than the minimum key
BOOL CDatabaseTabsGeneratorZip::ValueLessThanMinimum(void)
{
	return (m_csValue.GetLength() > 0) && (m_csValue[0] < '0');
}

// check if the field value is greater than the maximum key
BOOL CDatabaseTabsGeneratorZip::ValueGreaterThanMaximum(void)
{
	return (m_csValue.GetLength() > 0) && (m_csValue[0] > '9');
}

// perform any actions necessary at the end of UpdateTabs()
void CDatabaseTabsGeneratorZip::UpdateTabsFinish(void)
{
	CDatabaseTabsGeneratorBase::UpdateTabsFinish();
}

// set the search field for UpdateCurrentRecord()
void CDatabaseTabsGeneratorZip::SetSearchField(CFlatFileDatabaseField* pField, int nTabIndex)
{
	ASSERT(pField != NULL);
	if (pField != NULL)
	{
		ASSERT((nTabIndex >= 0) && (nTabIndex < m_Descriptions.GetSize()));
		if ((nTabIndex >= 0) && (nTabIndex < m_Descriptions.GetSize()))
		{
			CTabDescriptionString* pDescription = (CTabDescriptionString*)(m_Descriptions[nTabIndex]);
			ASSERT(pDescription != NULL);
			if (pDescription != NULL)
			{
				pField->SetAsString(pDescription->GetValue());
			}
		}
	}
}

// get the current value of a field to use during a call to CurrentRecordTabIndex()
void CDatabaseTabsGeneratorZip::GetFieldValue(CFlatFileDatabaseField* pField)
{
	TRY
	{
		ASSERT(pField != NULL);
		if (pField != NULL)
		{
			m_csFieldValue = pField->GetAsString();
			m_csFieldValue.MakeUpper();
		}
	}
	END_TRY
}

// compare the current field value with the value of the given tab description
BOOL CDatabaseTabsGeneratorZip::CompareFieldValue(int nDescription)
{
	BOOL fResult = FALSE;

	ASSERT((nDescription >= 0) && (nDescription < m_Descriptions.GetSize()));
	if ((nDescription >= 0) && (nDescription < m_Descriptions.GetSize()))
	{
		CTabDescriptionString* pDescription = (CTabDescriptionString*)(m_Descriptions[nDescription]);
		ASSERT(pDescription != NULL);
		if (pDescription != NULL)
		{
			fResult  = strcmp(m_csFieldValue, pDescription->GetValue()) < 0;
		}
	}

	return fResult;
}

//////////////////////////////////////////////////////////////////////////
// CDatabaseTabs
//
// A class that wraps a given generator class to hide the face that
// changing field types changes the underlying generator object.

CDatabaseTabs::CDatabaseTabs(CFlatFileDatabaseCursor* pCursor, CAddressBookTabControl* pTabs)
{
	ASSERT(m_pDataCursor != NULL);
	ASSERT(m_pTabs != NULL);

	m_pDataCursor = pCursor;
	m_pTabs = pTabs;
	m_pGenerator = NULL;
}

CDatabaseTabs::~CDatabaseTabs()
{
	FreeGenerator();
}

void CDatabaseTabs::SetAlphaFormat(UINT nLength)
{
	ASSERT(m_pDataCursor != NULL);
	ASSERT(m_pTabs != NULL);

	// Free any existing generator.
	FreeGenerator();

	// Allocate a new CDatabaseTabsGeneratorAlpha object.
	TRY
	{
		m_pGenerator = new CDatabaseTabsGeneratorAlpha(m_pDataCursor, m_pTabs, nLength);
	}
	END_TRY
}

void CDatabaseTabs::SetAlphaListFormat(CSortedPtrArray* pList)
{
	ASSERT(m_pDataCursor != NULL);
	ASSERT(m_pTabs != NULL);

	// Free any existing generator.
	FreeGenerator();

	// Allocate a new CDatabaseTabsGeneratorAlphaList object.
	TRY
	{
		m_pGenerator = new CDatabaseTabsGeneratorAlphaList(m_pDataCursor, m_pTabs, pList);
	}
	END_TRY
}

void CDatabaseTabs::SetIndexListFormat(CFlatFileValueListManager* pListManager, CFlatFileIndexDescription* pIndex)
{
	ASSERT(m_pDataCursor != NULL);
	ASSERT(m_pTabs != NULL);

	// Free any existing generator.
	FreeGenerator();

	// Allocate a new CDatabaseTabsGeneratorIndexList object.
	TRY
	{
		m_pGenerator = new CDatabaseTabsGeneratorIndexList(m_pDataCursor, m_pTabs, pListManager, pIndex);
	}
	END_TRY
}

void CDatabaseTabs::SetZipFormat(UINT nLength)
{
	ASSERT(m_pDataCursor != NULL);
	ASSERT(m_pTabs != NULL);

	// Free any existing generator.
	FreeGenerator();

	// Allocate a new CDatabaseTabsGeneratorZip object.
	TRY
	{
		m_pGenerator = new CDatabaseTabsGeneratorZip(m_pDataCursor, m_pTabs, nLength);
	}
	END_TRY
}

// rebuild all tab descriptions from database
void CDatabaseTabs::UpdateTabs(void)
{
	ASSERT(m_pGenerator != NULL);
	if (m_pGenerator != NULL)
	{
		m_pGenerator->UpdateTabs();
	}
}

// select the tab which represents the record at the current cursor position
void CDatabaseTabs::UpdateCurrentTab(void)
{
	ASSERT(m_pGenerator != NULL);
	if (m_pGenerator != NULL)
	{
		m_pGenerator->UpdateCurrentTab();
	}
}

// move the cursor to the first record represented by a given tab
void CDatabaseTabs::UpdateCurrentRecord(int nTabIndex)
{
	ASSERT(m_pGenerator != NULL);
	if (m_pGenerator != NULL)
	{
		m_pGenerator->UpdateCurrentRecord(nTabIndex);
	}
}

// return the index of the tab that represents the record at the current cursor position
int CDatabaseTabs::CurrentRecordTabIndex(void)
{
	int nTabIndex = -1;

	ASSERT(m_pGenerator != NULL);
	if (m_pGenerator != NULL)
	{
		nTabIndex = m_pGenerator->CurrentRecordTabIndex();
	}

	return nTabIndex;
}

// Return a pointer to the current tab descriptions.
CTabDescriptionArray* CDatabaseTabs::GetTabDescriptions(void)
{
	CTabDescriptionArray* pTabDescriptions = NULL;

	ASSERT(m_pGenerator != NULL);
	if (m_pGenerator != NULL)
	{
		pTabDescriptions = m_pGenerator->GetTabDescriptions();
	}

	return pTabDescriptions;
}

// free any currently allocated generator
void CDatabaseTabs::FreeGenerator(void)
{
	delete m_pGenerator;
	m_pGenerator = NULL;
}
