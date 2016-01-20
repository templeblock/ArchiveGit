/*
// $Workfile: KEYWORD.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/KEYWORD.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:22:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:58   Fred
// Initial revision.
// 
//    Rev 1.4   06 Sep 1996 18:54:08   Fred
// 3000 changed to 3F00
// 
//    Rev 1.3   06 Sep 1996 00:12:06   Fred
// Extreme kludge to prevent DWORD array overflow
// 
//    Rev 1.2   30 Aug 1996 10:45:20   Jay
// Keyword/item stuff
// 
//    Rev 1.1   15 Aug 1996 17:41:46   Fred
// 16-bit fixes
// 
//    Rev 1.0   15 Aug 1996 16:00:20   Jay
// Initial revision.
*/

#include "stdafx.h"
#include "keyword.h"
#include "fileitem.h"
#ifndef WIN32
#include <ctype.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// CSortedKeywordArray

CSortedKeywordArray::CSortedKeywordArray(BOOL fNoDuplicates /*=TRUE*/)
	: CSortedDWordArray(fNoDuplicates)
{
	m_fUseCustomCompare = TRUE;
	m_pManager = NULL;
	m_pszSearchKeyword = NULL;
}

int CSortedKeywordArray::CustomCompare(DWORD dwValue, DWORD dwTry)
{
	int nResult = -1;
	ASSERT(m_pManager != NULL);
	if (m_pManager != NULL)
	{
		CKeywordRecord* pTry = (CKeywordRecord*)m_pManager->GetRecord((DATAID)dwTry, DATATYPE_Keyword, TRUE);
		if (pTry != NULL)
		{
			if (dwValue == 0)
			{
				ASSERT(m_pszSearchKeyword != NULL);
				nResult = pTry->Name().Compare(m_pszSearchKeyword);
			}
			else
			{
				CKeywordRecord* pValue = (CKeywordRecord*)m_pManager->GetRecord((DATAID)dwValue, DATATYPE_Keyword, TRUE);
				if (pValue != NULL)
				{
					nResult = pTry->Name().Compare(pValue->Name());
//					nResult = (dwTry > dwValue) ? 1 : ((dwTry < dwValue) ? -1 : 0);
					pValue->ReleaseReadOnly();
				}
			}
			pTry->ReleaseReadOnly();
		}
	}
	return nResult;
}

/*
// Search for a value by name.
*/

int CSortedKeywordArray::Find(LPCSTR pszKeyword)
{
	int nIndex = -1;
	ASSERT(pszKeyword != NULL);
	if (pszKeyword != NULL)
	{
		m_pszSearchKeyword = pszKeyword;
		nIndex = CSortedDWordArray::Find(0);
		m_pszSearchKeyword = NULL;
	}
	return nIndex;
}

ERRORCODE CSortedKeywordArray::ReadData(StorageDevice* pDevice)
{
	ERRORCODE error;
	WORD wCount;

	if ((error = pDevice->read(&wCount, sizeof(wCount))) == ERRORCODE_None)
	{
#ifndef WIN32
		// MLSTODO: Create huge Dword array class (useful for address book receiver list).
		// Clip count in case there are too many keywords.
		if (wCount > 0x3F00)
		{
			wCount = 0x3F00;
		}
#endif
		if (wCount != 0)
		{
			SetSize(wCount);			// Optimization
			RemoveAt(0, wCount);

			for (WORD w = 0; w < wCount; w++)
			{
				DWORD dwKeyword;

				if ((error = pDevice->read(&dwKeyword, sizeof(dwKeyword))) != ERRORCODE_None)
				{
					break;
				}
				CDWordArray::Add(dwKeyword);
			}
		}
	}
	return error;
}

ERRORCODE CSortedKeywordArray::WriteData(StorageDevice* pDevice)
{
	ERRORCODE error;
	WORD wCount = GetSize();

	if ((error = pDevice->write(&wCount, sizeof(wCount))) == ERRORCODE_None)
	{
		for (WORD w = 0; w < wCount; w++)
		{
			DWORD dwKeyword = GetAt(w);

			if ((error = pDevice->write(&dwKeyword, sizeof(dwKeyword))) != ERRORCODE_None)
			{
				break;
			}
		}
	}
	return error;
}

ST_MAN_SIZE CSortedKeywordArray::SizeofData(StorageDevice* pDevice)
{
	return sizeof(WORD) + GetSize()*sizeof(DWORD);
}

/////////////////////////////////////////////////////////////////////////////
// Keyword utilities.

void FixupKeyword(CString& csWord)
{
	// Convert to lowercase.
	csWord.MakeLower();

	// Remove any non alphabetic or numeric characters.
	char szBuffer[256];
	char* pChar = szBuffer;

	int nLength = csWord.GetLength();
	if (nLength >= sizeof(szBuffer))
	{
		nLength = sizeof(szBuffer)-1;
	}

	for (int nChar = 0; nChar < nLength; nChar++)
	{
		if (isalnum(csWord[nChar]))
		{
			*(pChar++) = csWord[nChar];
		}
	}

	*pChar = '\0';
	csWord = szBuffer;
}

// Helper function which massages a keyword string typed in by the user
// into an array of keywords ready to match against the search database.
void GetKeywords(LPCSTR pszKeywords, CMlsStringArray& Keywords)
{
	// Break the words at spaces.
	Keywords.SetNames(pszKeywords, ' ');

	// Run through each word and:
	//
	//		2) Lowercase the word.
	//		3) Remove all non alphabetic or numeric characters.
	//		4) Remove word if it's empty.

	for (int nWord = Keywords.GetSize()-1; nWord >= 0; nWord--)
	{
		TRY
		{
			// Get the string.
			CString& csWord = Keywords.ElementAt(nWord);

			FixupKeyword(csWord);

			// Remove the word if it's empty.
			if (csWord.IsEmpty())
			{
				Keywords.RemoveAt(nWord);
			}
		}
		CATCH_ALL(e)
		{
			Keywords.RemoveAt(nWord);
		}
		END_CATCH_ALL
	}
}

/////////////////////////////////////////////////////////////////////////////
// CKeywordRecord

/*
// The creator for a keyword record.
*/

CStandardDataRecord* CKeywordRecord::Create()
{
	return new CKeywordRecord;
}

/*
// The initialization routine for the keyword record.
*/

BOOL CKeywordRecord::Init(DATAID id, DATATYPE type, CStandardDataManager* pManager, LPVOID pData, ST_DEV_POSITION* in_where)
{
	if (CStandardDataRecord::Init(id, type, pManager, NULL, in_where))
	{
		if (pData != NULL)
		{
			// Creation data.
			m_csName = (LPCSTR)pData;
		}
	}
	return TRUE;
}

/*
// The constructor for a keyword record.
*/

CKeywordRecord::CKeywordRecord()
{
	memset(&m_KRecord, 0, sizeof(m_KRecord));
}

/*
// The destructor.
*/

CKeywordRecord::~CKeywordRecord()
{
}

/*
// Add a item to this keyword.
*/

void CKeywordRecord::AddItem(DATAID lItem)
{
	// The item should not be added twice. Search first.
	if (m_Items.find(lItem) == -1)
	{
		m_Items.add(lItem);
		Modified();
	}
}

void CKeywordRecord::RemoveItem(DATAID lItem)
{
	m_Items.remove(lItem);
	Modified();
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE CKeywordRecord::ReadData(StorageDevicePtr pDevice)
{
	ERRORCODE error;

	if ((error = CStandardDataRecord::ReadData(pDevice)) == ERRORCODE_None
		 && (error = pDevice->read_record(&m_KRecord, sizeof(m_KRecord))) == ERRORCODE_None
		 && (error = pDevice->read_cstring(m_csName)) == ERRORCODE_None
		 && (error = m_Items.read(pDevice)) == ERRORCODE_None)
	{
		// Success!
	}
	return error;
}

/*
// WriteData()
//
// Write the record.
// Only the record is written. The actual data should already have been
// written.
*/

ERRORCODE CKeywordRecord::WriteData(StorageDevicePtr pDevice)
{
	ERRORCODE error;

	if ((error = CStandardDataRecord::WriteData(pDevice)) == ERRORCODE_None
		 && (error = pDevice->write_record(&m_KRecord, sizeof(m_KRecord))) == ERRORCODE_None
		 && (error = pDevice->write_cstring(m_csName)) == ERRORCODE_None
		 && (error = m_Items.write(pDevice)) == ERRORCODE_None)
	{
		// Success!
	}
	return error;
}

/*
// SizeofData()
//
// Return the size of the record.
// This is the complete size of the written part and the file image.
*/

ST_MAN_SIZE CKeywordRecord::SizeofData(StorageDevicePtr pDevice)
{
	return CStandardDataRecord::SizeofData(pDevice)
				+ pDevice->size_record(sizeof(m_KRecord))
				+ pDevice->size_cstring(m_csName)
				+ m_Items.size(pDevice);
}

/*
// Assign method.
*/

void CKeywordRecord::Assign(const CDataRecord& Record)
{
/* Assign the base record first. */
	CStandardDataRecord::Assign(Record);
	CKeywordRecord& KRecord = (CKeywordRecord&)Record;

	m_KRecord = KRecord.m_KRecord;
	m_csName = KRecord.m_csName;
	m_Items.assign(KRecord.m_Items);
}

/////////////////////////////////////////////////////////////////////////////
// CKeywordArrayRecord

/*
// The creator for a keyword array record.
*/

CStandardDataRecord* CKeywordArrayRecord::Create()
{
	return new CKeywordArrayRecord;
}

/*
// The initialization routine for the keyword array record.
*/

BOOL CKeywordArrayRecord::Init(DATAID id, DATATYPE type, CStandardDataManager* pManager, LPVOID pData, ST_DEV_POSITION* in_where)
{
	m_Keywords.m_pManager = pManager;
	if (CStandardDataRecord::Init(id, type, pManager, NULL, in_where))
	{
		if (pData != NULL)
		{
			// Creation data.
		}
	}
	return TRUE;
}

/*
// The constructor for a keyword array record.
*/

CKeywordArrayRecord::CKeywordArrayRecord()
{
	memset(&m_KRecord, 0, sizeof(m_KRecord));
}

/*
// The destructor.
*/

CKeywordArrayRecord::~CKeywordArrayRecord()
{
}

/*
// Add an item to keywords.
*/

#if 0
void CKeywordArrayRecord::AddItemToKeywords(DATAID lItem, LPCSTR pszKeywords)
{
	CMlsStringArray csaKeywords;

	// Get the keyword(s) from the passed string.
	GetKeywords(pszKeywords, csaKeywords);

	// Add in the keyword(s).
	int nKeywords = csaKeywords.GetSize();
	for (int nKeyword = 0; nKeyword < nKeywords; nKeyword++)
	{
		// Get the keyword record for the next keyword string.
		CKeywordRecord* pKeyword = AddKeyword(csaKeywords.GetAt(nKeyword));
		if (pKeyword != NULL)
		{
			// Add the item to this keyword.
			pKeyword->AddItem(lItem);
			pKeyword->Release();
		}
	}
}
#endif

/*
// Add an item to keywords.
// This does the back (double) attach between item and keyword.
*/

void CKeywordArrayRecord::AddItemToKeywords(CFileItemData* pItemData, LPCSTR pszKeywords)
{
	CMlsStringArray csaKeywords;

	// Get the keyword(s) from the passed string.
	GetKeywords(pszKeywords, csaKeywords);

	// Add in the keyword(s).
	int nKeywords = csaKeywords.GetSize();
	for (int nKeyword = 0; nKeyword < nKeywords; nKeyword++)
	{
		// Get the keyword record for the next keyword string.
		CKeywordRecord* pKeyword = AddKeyword(csaKeywords.GetAt(nKeyword));
		if (pKeyword != NULL)
		{
			// Add the item to this keyword.
			pKeyword->AddItem(pItemData->Id());
			// Add this keyword to the item.
			pItemData->AddKeyword(pKeyword->Id());
			pKeyword->Release();
		}
	}
}

/*
// Add a keyword.
// The keyword is returned and must be Released.
*/

CKeywordRecord* CKeywordArrayRecord::AddKeyword(LPCSTR pszKeyword)
{
	// Try to find an existing keyword.
	CKeywordRecord* pKeyword = GetKeyword(pszKeyword);

	// If found, then return it. Otherwise...
	if (pKeyword == NULL)
	{
		// We need to create a new keyword.
		pKeyword = (CKeywordRecord*)(Manager()->NewRecord(DATATYPE_Keyword, (LPVOID)pszKeyword));

		// If we successfully created the new keyword, add it to the array.
		ASSERT(pKeyword != NULL);
		if (pKeyword != NULL)
		{
			m_Keywords.Add(pKeyword->Id());
			Modified();
		}
	}
	return pKeyword;
}

/*
// Get a keyword.
// The keyword must be Released.
*/

CKeywordRecord* CKeywordArrayRecord::GetKeyword(LPCSTR pszKeyword)
{
	CKeywordRecord* pKeyword = NULL;

	// Lookup the keyword by name.
	int nIndex = m_Keywords.Find(pszKeyword);

	// If we found it, then lock the record, and return it.
	if (nIndex != -1)
	{
		pKeyword = (CKeywordRecord*)(Manager()->GetRecord(m_Keywords.GetAt(nIndex), DATATYPE_Keyword));
	}

	return pKeyword;
}

/*
// Delete a keyword.
*/

void CKeywordArrayRecord::DeleteKeyword(LPCSTR pszKeyword)
{
	int nIndex;
	if ((nIndex = m_Keywords.Find(pszKeyword)) != -1)
	{
		// Delete the record itself.
		Manager()->DeleteRecord(m_Keywords.GetAt(nIndex));

		// Remove it from the array.
		m_Keywords.RemoveAt(nIndex);
		Modified();
	}
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE CKeywordArrayRecord::ReadData(StorageDevicePtr pDevice)
{
	ERRORCODE error;

	if ((error = CStandardDataRecord::ReadData(pDevice)) == ERRORCODE_None
		 && (error = pDevice->read_record(&m_KRecord, sizeof(m_KRecord))) == ERRORCODE_None
		 && (error = m_Keywords.ReadData(pDevice)) == ERRORCODE_None)
	{
		// Success!
	}
	return error;
}

/*
// WriteData()
//
// Write the record.
// Only the record is written. The actual data should already have been
// written.
*/

ERRORCODE CKeywordArrayRecord::WriteData(StorageDevicePtr pDevice)
{
	ERRORCODE error;

	if ((error = CStandardDataRecord::WriteData(pDevice)) == ERRORCODE_None
		 && (error = pDevice->write_record(&m_KRecord, sizeof(m_KRecord))) == ERRORCODE_None
		 && (error = m_Keywords.WriteData(pDevice)) == ERRORCODE_None)
	{
		// Success!
	}
	return error;
}

/*
// SizeofData()
//
// Return the size of the record.
// This is the complete size of the written part and the file image.
*/

ST_MAN_SIZE CKeywordArrayRecord::SizeofData(StorageDevicePtr pDevice)
{
	return CStandardDataRecord::SizeofData(pDevice)
				+ pDevice->size_record(sizeof(m_KRecord))
				+ m_Keywords.SizeofData(pDevice);
}

/*
// Assign method.
*/

void CKeywordArrayRecord::Assign(const CDataRecord& Record)
{
/* Assign the base record first. */
	CStandardDataRecord::Assign(Record);
	CKeywordArrayRecord& KRecord = (CKeywordArrayRecord&)Record;

	m_KRecord = KRecord.m_KRecord;
//	m_Keywords = KRecord.m_Keywords;
	ASSERT(FALSE);
}
