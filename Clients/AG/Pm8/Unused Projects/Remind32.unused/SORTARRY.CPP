/*
// $Workfile: SORTARRY.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:27p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/SORTARRY.CPP $
// 
// 1     3/03/99 6:27p Gbeddow
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:38   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:56:04   Fred
// Initial revision.
// 
//    Rev 1.6   01 Sep 1996 16:45:04   Jay
// Added FindName()
// 
//    Rev 1.5   15 Aug 1996 16:00:52   Jay
//  
// 
//    Rev 1.4   14 Aug 1996 11:43:08   Fred
// Added CMlsStringArray::GetNames()
// 
//    Rev 1.3   24 Jul 1996 13:12:56   Fred
// Moved CMlsStringArray to SORTARRY
// 
//    Rev 1.2   01 Jul 1996 07:32:04   Fred
// Receiver list
// 
//    Rev 1.1   18 Jun 1996 15:21:56   Fred
// Some fixes
// 
//    Rev 1.0   18 Jun 1996 08:28:20   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "util.h"
#include "sortarry.h"

//////////////////////////////////////////////////////////////////////////
// CSortedPtrArrayElement

CSortedPtrArrayElement::CSortedPtrArrayElement()
{
}

CSortedPtrArrayElement::~CSortedPtrArrayElement()
{
}

//////////////////////////////////////////////////////////////////////////
// CSortedPtrArrayElementString

CSortedPtrArrayElementString::CSortedPtrArrayElementString(LPCSTR pszValue)
{
	TRY
	{
		m_csValue = pszValue;
	}
	END_TRY
}

CSortedPtrArrayElementString::~CSortedPtrArrayElementString()
{
}

// Comparison function (to same kind).
int CSortedPtrArrayElementString::Compare(CSortedPtrArrayElement* pElement)
{
	CSortedPtrArrayElementString* pOther = (CSortedPtrArrayElementString*)pElement;
	return stricmp(GetAsString(), pOther->GetAsString());
}

// Get/Set as string.
LPCSTR CSortedPtrArrayElementString::GetAsString(void)
{
	return m_csValue;
}

void CSortedPtrArrayElementString::SetAsString(LPCSTR pszValue)
{
	TRY
	{
		m_csValue = pszValue;
	}
	END_TRY
}

// Get/Set as 32-bit signed number.
long CSortedPtrArrayElementString::GetAsSigned32(void)
{
	return strtol(m_csValue, NULL, 10);
}

void CSortedPtrArrayElementString::SetAsSigned32(long lValue)
{
	char szBuffer[128];
	SetAsString(ltoa(lValue, szBuffer, 10));
}

// Get/Set as 32-bit unsigned number.
DWORD CSortedPtrArrayElementString::GetAsUnsigned32(void)
{
	return strtoul(m_csValue, NULL, 10);
}

void CSortedPtrArrayElementString::SetAsUnsigned32(DWORD dwValue)
{
	char szBuffer[128];
	SetAsString(ultoa(dwValue, szBuffer, 10));
}

//////////////////////////////////////////////////////////////////////////
// CSortedPtrArrayElementSigned32

CSortedPtrArrayElementSigned32::CSortedPtrArrayElementSigned32(long lValue)
{
	m_lValue = lValue;
}

CSortedPtrArrayElementSigned32::~CSortedPtrArrayElementSigned32()
{
}

// Comparison function (to same kind).
int CSortedPtrArrayElementSigned32::Compare(CSortedPtrArrayElement* pElement)
{
	CSortedPtrArrayElementSigned32* pOther = (CSortedPtrArrayElementSigned32*)pElement;
	if (m_lValue < pOther->GetAsSigned32())
	{
		return -1;
	}
	if (m_lValue > pOther->GetAsSigned32())
	{
		return 1;
	}
	return 0;
}

// Get/Set as string.
LPCSTR CSortedPtrArrayElementSigned32::GetAsString(void)
{
	// Convert to string.
	TRY
	{
		ltoa(m_lValue, m_csValue.GetBuffer(128), 10);
		m_csValue.ReleaseBuffer();
	}
	END_TRY

	// Return pointer to string.
	return m_csValue;
}

void CSortedPtrArrayElementSigned32::SetAsString(LPCSTR pszValue)
{
	m_lValue = strtol(pszValue, NULL, 10);
}

// Get/Set as 32-bit signed number.
long CSortedPtrArrayElementSigned32::GetAsSigned32(void)
{
	return m_lValue;
}

void CSortedPtrArrayElementSigned32::SetAsSigned32(long lValue)
{
	m_lValue = lValue;
}

// Get/Set as 32-bit unsigned number.
DWORD CSortedPtrArrayElementSigned32::GetAsUnsigned32(void)
{
	return (DWORD)GetAsSigned32();
}

void CSortedPtrArrayElementSigned32::SetAsUnsigned32(DWORD dwValue)
{
	SetAsSigned32((long)dwValue);
}

//////////////////////////////////////////////////////////////////////////
// CSortedPtrArrayElementUnsigned32

CSortedPtrArrayElementUnsigned32::CSortedPtrArrayElementUnsigned32(DWORD dwValue)
{
	m_dwValue = dwValue;
}

CSortedPtrArrayElementUnsigned32::~CSortedPtrArrayElementUnsigned32()
{
}

// Comparison function (to same kind).
int CSortedPtrArrayElementUnsigned32::Compare(CSortedPtrArrayElement* pElement)
{
	CSortedPtrArrayElementUnsigned32* pOther = (CSortedPtrArrayElementUnsigned32*)pElement;
	if (m_dwValue < pOther->GetAsUnsigned32())
	{
		return -1;
	}
	if (m_dwValue > pOther->GetAsUnsigned32())
	{
		return 1;
	}
	return 0;
}

// Get/Set as string.
LPCSTR CSortedPtrArrayElementUnsigned32::GetAsString(void)
{
	// Convert to string.
	TRY
	{
		ultoa(m_dwValue, m_csValue.GetBuffer(128), 10);
		m_csValue.ReleaseBuffer();
	}
	END_TRY

	// Return pointer to string.
	return m_csValue;
}

void CSortedPtrArrayElementUnsigned32::SetAsString(LPCSTR pszValue)
{
	m_dwValue = strtoul(pszValue, NULL, 10);
}

// Get/Set as 32-bit signed number.
long CSortedPtrArrayElementUnsigned32::GetAsSigned32(void)
{
	return (long)GetAsUnsigned32();
}

void CSortedPtrArrayElementUnsigned32::SetAsSigned32(long lValue)
{
	SetAsUnsigned32((DWORD)lValue);
}

// Get/Set as 32-bit unsigned number.
DWORD CSortedPtrArrayElementUnsigned32::GetAsUnsigned32(void)
{
	return m_dwValue;
}

void CSortedPtrArrayElementUnsigned32::SetAsUnsigned32(DWORD dwValue)
{
	m_dwValue = dwValue;
}

//////////////////////////////////////////////////////////////////////////
// CSortedPtrArray

CSortedPtrArray::CSortedPtrArray(BOOL fNoDuplicates /*=TRUE*/)
{
	m_fNoDuplicates = fNoDuplicates;
}

CSortedPtrArray::~CSortedPtrArray()
{
	// Delete all of the element objects.
	DeleteAll();
}

void CSortedPtrArray::DeleteAll(void)
{
	for (int nElement = 0; nElement < GetSize(); nElement++)
	{
		CSortedPtrArrayElement* pElement = (CSortedPtrArrayElement*)GetAt(nElement);
		if (pElement != NULL)
		{
			// Delete the element object.
			delete pElement;
			SetAt(nElement, NULL);
		}
	}

	// Delete the CPtrArray elements.
	RemoveAll();
}

int CSortedPtrArray::Add(CSortedPtrArrayElement* pNewElement)
{
	// Remember if it was an exact match.
	BOOL fDuplicate = FALSE;

	// Perform a binary search for the position to insert the new element.
	int nMin = 0;
	int nMax = GetSize();

	// Remember first known index which is greater than new data (end of array).
	int nPosition = nMax;

	while (nMin < nMax)
	{
		// Split the search space (elements are 4 bytes, so no overflow).
		int nTry = (nMin+nMax)/2;

		// Get the element pointer from the array.
		CSortedPtrArrayElement* pElement = (CSortedPtrArrayElement*)ElementAt(nTry);

		// Compare the elements.
		int nResult = pElement->Compare(pNewElement);

		// Check for exact match.
		if (nResult == 0)
		{
			// Elements are equal. Insert here.
			nPosition = nTry;
			fDuplicate = TRUE;
			break;
		}

		// Check if current data is greater than new data.
		if (nResult > 0)
		{
			// We know everything from nTry forward is greater.
			nMax = nTry;

			// Record index of element we were less than.
			nPosition = nMax;
		}

		// Check if current data is less than new data.
		if (nResult < 0)
		{
			// We know everything from nTry back is lesser.
			nMin = nTry+1;
		}
	}

	// We have the position, insert the new element.
	if (!fDuplicate || !m_fNoDuplicates)
	{
		InsertAt(nPosition, pNewElement);
	}
	else
	{
		// We ASSUME that the object should be deleted.
		delete pNewElement;
	}

	return nPosition;
};

CSortedDWordArray::CSortedDWordArray(BOOL fNoDuplicates /*=TRUE*/)
{
	m_fNoDuplicates = fNoDuplicates;
	m_fUseCustomCompare = FALSE;
}

CSortedDWordArray::~CSortedDWordArray()
{
}

// Add a new value with sorting and duplicate checking.
int CSortedDWordArray::Add(DWORD dwValue)
{
	int nInsertIndex;
	int nIndex = Find(dwValue, &nInsertIndex);
	if ((nIndex == -1) || (!m_fNoDuplicates))
	{
		InsertAt(nInsertIndex, dwValue);
	}
	return nIndex;
}

// Delete a value.
int CSortedDWordArray::Delete(DWORD dwValue)
{
	int nIndex = Find(dwValue);
	if (nIndex != -1)
	{
		RemoveAt(nIndex);
	}
	return nIndex;
}

// Search for a value.
int CSortedDWordArray::Find(DWORD dwValue, int* pnInsertPosition /*=NULL*/)
{
	int nIndex = -1;

	// Perform a binary search for the value. If the value is not found,
	// then the index of the next value is returned.
	int nMin = 0;
	int nMax = GetSize();

	// Remember first known index which is greater than the search value (end of array).
	int nPosition = nMax;

	while (nMin < nMax)
	{
		// Split the search space (elements are 4 bytes, so no overflow).
		int nTry = (nMin+nMax)/2;

		// Get the value from the array.
		DWORD dwTry = GetAt(nTry);

		// Compare the elements.
		int nResult;
		if (m_fUseCustomCompare)
		{
			nResult = CustomCompare(dwValue, dwTry);
		}
		else
		{
			// This could be a virtual function, but it would be slower...
			nResult = (dwTry > dwValue) ? 1 : ((dwTry < dwValue) ? -1 : 0);
		}

		// Check for exact match.
		if (nResult == 0)
		{
			// Elements are equal.
			nPosition = nTry;
			nIndex = nTry;
			break;
		}

		// Check if current value is greater than search value.
		if (nResult > 0)
		{
			// We know everything from nTry forward is greater.
			nMax = nTry;

			// Record index of value we were less than.
			nPosition = nMax;
		}

		// Check if current value is less than search value.
		if (nResult < 0)
		{
			// We know everything from nTry back is lesser.
			nMin = nTry+1;
		}
	}

	// Save the position where we will insert.
	if (pnInsertPosition != NULL)
	{
		*pnInsertPosition = nPosition;
	}

	return nIndex;
}

// Check if a value exists.
int CSortedDWordArray::Exists(DWORD dwValue)
{
	return Find(dwValue) != -1;
}

int CSortedDWordArray::CustomCompare(DWORD dwValue, DWORD dwTry)
{
	// Derive your own class and override if you set m_fUseCustomCompare!
	ASSERT(FALSE);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMlsStringArray

CMlsStringArray::CMlsStringArray(LPCSTR pszNames /*=NULL*/, char cSeparator /*='|'*/)
{
	SetNames(pszNames, cSeparator);
}

CMlsStringArray::CMlsStringArray(UINT uNames, char cSeparator /*='|'*/)
{
	SetNames(uNames, cSeparator);
}

CMlsStringArray::~CMlsStringArray()
{
}

void CMlsStringArray::SetNames(LPCSTR pszNames, char cSeparator /*='|'*/)
{
	CString csNames;
	CString csName;
	TRY
	{
		RemoveAll();
		if (pszNames != NULL)
		{
			csNames = pszNames;
			BOOL fDone = FALSE;
			while (!fDone)
			{
				fDone = GetNextName(csName, csNames, cSeparator);
				Add(csName);
			}
		}
	}
	END_TRY
}

void CMlsStringArray::SetNames(UINT uNames, char cSeparator /*='|'*/)
{
	CString csNames;
	TRY
	{
		RemoveAll();
		csNames.LoadString(uNames);
		SetNames(csNames, cSeparator);
	}
	END_TRY
}

void CMlsStringArray::GetNames(CString& csNames, char cSeparator /*='|'*/)
{
	TRY
	{
		csNames.Empty();
		for (int i = 0; i < GetSize(); i++)
		{
			if (i > 0)
			{
				csNames += cSeparator;
			}
			csNames += GetAt(i);
		}
	}
	CATCH_ALL(e)
	{
		csNames.Empty();
	}
	END_CATCH_ALL
}

int CMlsStringArray::FindName(LPCSTR pName)
{
	int nNames = GetSize();
	for (int nName = 0; nName < nNames; nName++)
	{
		if (GetAt(nName) == pName)
		{
			return nName;
		}
	}
	return -1;
}

BOOL CMlsStringArray::GetNextName(CString& csName, CString& csNames, char cSeparator)
{
	// Returns TRUE when the name returned is the last one.
	BOOL fResult = TRUE;
	TRY
	{
		int nIndex = csNames.Find(cSeparator);
		if (nIndex == -1)
		{
			csName = csNames;
			csNames.Empty();
			fResult = TRUE;
		}
		else
		{
			csName = csNames.Left(nIndex);
			csNames = csNames.Mid(nIndex+1);
			fResult = FALSE;
		}
		Util::Trim(csName);
	}
	CATCH_ALL(e)
	{
		TRY
		{
			csName.Empty();
		}
		END_TRY
		fResult = TRUE;
	}
	END_CATCH_ALL

	return fResult;
}
