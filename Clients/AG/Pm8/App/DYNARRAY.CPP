/*
// $Header: /PM8/App/DYNARRAY.CPP 1     3/03/99 6:05p Gbeddow $
//
// $Workfile: DYNARRAY.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/DYNARRAY.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 2     4/27/98 3:38p Hforman
// fixed reversed src/dest in memmove() in InsertElement()
// 
//    Rev 1.0   14 Aug 1997 15:20:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:16   Fred
// Initial revision.
// 
//    Rev 1.5   15 Apr 1997 14:29:08   dennisp
// Add RemoveAt()
// 
//    Rev 1.4   20 Jan 1997 09:39:32   Fred
// GetElement is now fast, use GetElementSafe to be safe
// 
//    Rev 1.3   05 Nov 1996 14:06:46   Jay
// Got rid of warnings, etc.
// 
//    Rev 1.2   31 Jul 1996 11:58:54   Fred
// 16-bit compatibility
// 
//    Rev 1.1   12 Jul 1996 16:46:12   Jay
// Added Compare().
// 
//    Rev 1.0   14 Mar 1996 13:48:22   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:28   FRED
// Initial revision.
// 
//    Rev 1.6   13 Sep 1995 11:33:40   JAY
// New 32-bit stuff
// 
//    Rev 1.5   13 May 1995 11:14:26   JAY
//  
// 
//    Rev 1.4   09 May 1995 08:04:28   JAY
// Added InsertElement.
// 
//    Rev 1.3   05 May 1995 16:49:28   JAY
// Exciting new additions to OutlinePaths!
// 
//    Rev 1.2   28 Apr 1995 13:49:20   FRED
//  
// 
//    Rev 1.1   27 Apr 1995 13:21:36   FRED
//  
// 
//    Rev 1.0   25 Apr 1995 10:33:52   FRED
// Initial revision.
*/

#include "stdafx.h"

#include "dynarray.h"
#include "util.h"

/////////////////////////////////////////////////////////////////////////////
// CDynamicArray

CDynamicArray::CDynamicArray(DWORD dwElementSize, DWORD dwIncrement, DWORD dwMaximum)
{
	Reset(TRUE);
	
	m_dwElementSize = dwElementSize;
	m_dwIncrement = dwIncrement;
	m_dwMaximum = dwMaximum;
}

CDynamicArray::~CDynamicArray(void)
{
	Reset();
}

void CDynamicArray::Reset(BOOL fConstructing /*=FALSE*/)
{
	if (!fConstructing)
	{
		if (m_hArray != NULL)
		{
			if (m_pArray != NULL)
			{
				GlobalUnlock(m_hArray);
				m_pArray = NULL;
			}
				
			GlobalFree(m_hArray);
			m_hArray = NULL;
		}
	}
	
	m_hArray = NULL;
	m_pArray = NULL;
	m_dwCount = 0;
	m_dwAllocated = 0;
}

void CDynamicArray::Allocate(DWORD dwCount)
{
	if (dwCount > m_dwMaximum)
	{
		dwCount = m_dwMaximum;
	}
	
	if (m_dwAllocated != dwCount)
	{
		DWORD dwNewSize = m_dwElementSize*dwCount;
		
		if (dwNewSize == 0)
		{
			// Zero count, simply free the array.
			
			Reset();
		}
		else
		{
			if (m_hArray == NULL)
			{
				// Allocate a new array.
				
				m_hArray = GlobalAlloc(GMEM_MOVEABLE, dwNewSize);
				
				m_dwCount = 0;
				m_dwAllocated = dwCount;
			}
			else
			{
				// Resize the current array.
				
				if (m_pArray != NULL)
				{
					GlobalUnlock(m_hArray);
					m_pArray = NULL;
				}
				
				HGLOBAL hNewArray = GlobalReAlloc(m_hArray, dwNewSize, GMEM_MOVEABLE);
				
				if (hNewArray != NULL)
				{
					m_hArray = hNewArray;
					
					if (dwCount > m_dwAllocated)
					{
						m_dwAllocated = dwCount;
					}
				}
			}
			
			// Lock the array.
			
			if (m_hArray != NULL)
			{
				m_pArray = (LPBYTE)GlobalLock(m_hArray);
			}
		}
	}
}

void CDynamicArray::SetElementCount(DWORD dwCount)
{
	m_dwCount = dwCount;
	ASSERT(dwCount <= m_dwAllocated);
}

BOOL CDynamicArray::AddElement(LPVOID pNewElement, DWORD dwCount /*=1*/)
{
	if ((m_dwCount+dwCount) > m_dwAllocated)
	{
		Allocate(m_dwCount+dwCount+m_dwIncrement);
	}

	LPBYTE pElement = (LPBYTE)GetElementSafe(m_dwCount, dwCount);

	if (pElement != NULL)
	{
#ifdef WIN32
		memcpy(pElement, pNewElement, m_dwElementSize*dwCount);
#else
		hmemcpy(pElement, pNewElement, m_dwElementSize*dwCount);
#endif
		m_dwCount += dwCount;
		return TRUE;
	}

	return FALSE;
}

BOOL CDynamicArray::InsertElement(DWORD dwIndex, LPVOID pNewElement, DWORD dwCount /*=1*/)
{
	if ((m_dwCount+dwCount) > m_dwAllocated)
	{
		Allocate(m_dwCount+dwCount+m_dwIncrement);
	}

	LPBYTE pElement = (LPBYTE)GetElementSafe(dwIndex, dwCount);

	if (pElement != NULL)
	{
		DWORD dwSize = m_dwElementSize*dwCount;
		if (dwIndex != m_dwCount)
		{
#ifdef WIN32
			memmove((char*)(pElement+dwSize),	// dest
					  (char*)pElement,				// src
					  (m_dwCount - dwIndex)*m_dwElementSize);
#else
			Util::HMemMove((char __huge*)pElement,
								(char __huge*)(pElement+dwSize),
								(m_dwCount - dwIndex)*m_dwElementSize);
#endif
		}
#ifdef WIN32
		memcpy(pElement, pNewElement, dwSize);
#else
		hmemcpy(pElement, pNewElement, dwSize);
#endif
		m_dwCount += dwCount;
		return TRUE;
	}

	return FALSE;
}

BOOL CDynamicArray::RemoveAt(DWORD dwIndex, DWORD dwCount /* = 1 */, BOOL bPack /* FALSE */)
{
	// just remove a range
	DWORD dwMoveCount = GetElementCount() - (dwIndex + dwCount);
	LPBYTE pElement = (LPBYTE)GetElementSafe(dwIndex);
	LPBYTE pNewElement = (LPBYTE)GetElementSafe(dwIndex+dwCount);

   ASSERT(pElement);
   if(pElement == NULL)
      return FALSE;

	if (dwMoveCount)
	{
#ifdef WIN32
		memcpy(pElement, pNewElement, dwMoveCount * GetElementSize());
#else
		hmemcpy(pElement, pNewElement, dwMoveCount * GetElementSize());
#endif
	}
   // Shrink memory
   SetElementCount(m_dwCount - dwCount);
   if(bPack)
      Allocate(m_dwCount - dwCount);
   return TRUE;
}

LPVOID CDynamicArray::GetElementSafe(DWORD dwIndex, DWORD dwCount /*=1*/)
{
	if (m_hArray != NULL)
	{
		if (m_pArray != NULL)
		{
			if ((dwIndex >= 0) && (dwIndex+dwCount <= m_dwAllocated))
			{
				return GetElement(dwIndex);
			}
		}
	}
	
	return NULL;
}

DWORD CDynamicArray::GetElementCount(void)
{
	return m_dwCount;
}

DWORD CDynamicArray::GetElementSize(void)
{
	return m_dwElementSize;
}

void CDynamicArray::Pack(void)
{
	Allocate(m_dwCount);
}

void CDynamicArray::Free(void)
{
	Allocate(0);
}

BOOL CDynamicArray::Assign(CDynamicArray* pArray)
{
	BOOL fResult = FALSE;
	
	if (pArray != NULL)
	{
		if ((m_dwElementSize == pArray->m_dwElementSize)
		 && (m_dwIncrement == pArray->m_dwIncrement)
		 && (m_dwMaximum == pArray->m_dwMaximum))
		{
			Free();
			
			m_hArray = pArray->m_hArray;
			m_pArray = pArray->m_pArray;
			m_dwCount = pArray->m_dwCount;
			m_dwAllocated = pArray->m_dwAllocated;
			
			pArray->m_hArray = NULL;
			pArray->m_pArray = NULL;
			pArray->m_dwCount = 0;
			pArray->m_dwAllocated = 0;
			
			fResult = TRUE;
		}
		else
		{
			// Array mismatch. Assign has failed. Bad invocation of Assign!
			ASSERT(FALSE);
		}
	}
	
	return fResult;
}

BOOL CDynamicArray::Copy(const CDynamicArray* pArray)
{
	BOOL fResult = FALSE;

	Free();

	m_dwElementSize = pArray->m_dwElementSize;
	m_dwIncrement = pArray->m_dwIncrement;
	m_dwMaximum = pArray->m_dwMaximum;

	/*
	// We access the variables directly instead of the access functions
	// because the access functions don't like 'const' arrays.
	*/

	return AddElement(pArray->m_pArray, pArray->m_dwCount);
}

BOOL CDynamicArray::Compare(const CDynamicArray& Array)
{
#ifndef WIN32
	if (m_dwElementSize*m_dwCount >= 65535L)
	{
		ASSERT(FALSE);
		// They don't compare because we can't compare.
		return FALSE;
	}
#endif

	return m_dwElementSize == Array.m_dwElementSize
//			&& m_dwIncrement == Array.m_dwIncrement		// Who cares?
			&& m_dwMaximum == Array.m_dwMaximum
			&& m_dwCount == Array.m_dwCount
			&& (m_dwCount == 0
					|| (m_pArray != NULL
						 && Array.m_pArray != NULL
						 && memcmp(m_pArray, Array.m_pArray, (size_t)(m_dwElementSize*m_dwCount)) == 0));
}

#ifdef DYNARRAY_IO

struct CDynarrayIO
{
	DWORD m_dwElementSize;
	DWORD m_dwCount;
	DWORD m_dwIncrement;
	DWORD m_dwMaximum;
};

/*
// Read our data from a storage device.
*/

ERRORCODE CDynamicArray::ReadData(StorageDevice* pDevice)
{
	Free();

	ERRORCODE error;
	CDynarrayIO IORecord;

	if ((error = pDevice->read_record(&IORecord, sizeof(IORecord))) == ERRORCODE_None)
	{
		m_dwElementSize = IORecord.m_dwElementSize;
		m_dwIncrement = IORecord.m_dwIncrement;
		m_dwMaximum = IORecord.m_dwMaximum;

		Allocate(IORecord.m_dwCount);

		m_dwCount = IORecord.m_dwCount;

		if (m_pArray != NULL)
		{
			error = pDevice->huge_read(m_pArray, m_dwCount*m_dwElementSize);
		}
	}
	return error;
}

/*
// Write our data to a storage device.
*/

ERRORCODE CDynamicArray::WriteData(StorageDevice* pDevice)
{
	ERRORCODE error;
	CDynarrayIO IORecord;

	IORecord.m_dwElementSize = m_dwElementSize;
	IORecord.m_dwCount = m_dwCount;
	IORecord.m_dwIncrement = m_dwIncrement;
	IORecord.m_dwMaximum = m_dwMaximum;

	if ((error = pDevice->write_record(&IORecord, sizeof(IORecord))) == ERRORCODE_None)
	{
	/* Write the data. */
		error = pDevice->huge_write(m_pArray, m_dwCount*m_dwElementSize);
	}
	return error;
}

/*
// Return the size of our data if written to a storage device.
*/

ST_MAN_SIZE CDynamicArray::SizeofData(StorageDevice* pDevice)
{
	return pDevice->size_record(sizeof(CDynarrayIO)) + m_dwCount*m_dwElementSize;
}

#endif
