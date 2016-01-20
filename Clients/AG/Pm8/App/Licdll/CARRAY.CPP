/*
// Generic (simple, non-object) array.
// Code is modified version of MFC's CByteArray.
//
// This class can only hold elements which can be in normal arrays
// (i.e. no constructors, destructors, vtables, etc.). In other words,
// this good for storing simple data structures.
//
// In some ways, this class is dangerous (if used improperly). Please only
// use this properly.
//
// $Workfile: CARRAY.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/CARRAY.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:35:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:19:38   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:41:30   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/CARRAY.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:22   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:58:20   JAY
// Initial revision.
// 
//    Rev 1.0   02 Dec 1994 15:29:06   JAY
// Initial revision.
*/

/////////////////////////////////////////////////////////////////////////////
//
// Implementation of a generic Array
//
/////////////////////////////////////////////////////////////////////////////
// NOTE: we allocate an array of 'm_nMaxSize' elements, but only
//  the current size 'm_nSize' contains properly constructed
//  objects.

#include "stdafx.h"
#include "carray.h"

#include <limits.h>
#define SIZE_T_MAX  UINT_MAX            /* max size for a size_t */

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

IMPLEMENT_DYNAMIC(CArray, CObject)

/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////

CArray::CArray(int nElementSize /*=0*/)
{
	m_nElementSize = nElementSize;
	m_pData = NULL;
	m_nSize = m_nMaxSize = m_nGrowBy = 0;
}

CArray::~CArray()
{
	ASSERT_VALID(this);

	delete [] (BYTE*)m_pData;
}

void CArray::SetSize(int nNewSize, int nGrowBy /* = -1 */)
{
	ASSERT_VALID(this);
	ASSERT(nNewSize >= 0);

	if (nGrowBy != -1)
		m_nGrowBy = nGrowBy;  // set new size

	if (nNewSize == 0)
	{
		// shrink to nothing
		delete [] (BYTE*)m_pData;
		m_pData = NULL;
		m_nSize = m_nMaxSize = 0;
	}
	else if (m_pData == NULL)
	{
		// create one with exact size
#ifdef SIZE_T_MAX
		ASSERT((long)nNewSize * m_nElementSize <= SIZE_T_MAX);  // no overflow
#endif
		m_pData = (BYTE*) new BYTE[nNewSize * m_nElementSize];

		memset(m_pData, 0, nNewSize * m_nElementSize);  // zero fill

		m_nSize = m_nMaxSize = nNewSize;
	}
	else if (nNewSize <= m_nMaxSize)
	{
		// it fits
		if (nNewSize > m_nSize)
		{
			// initialize the new elements

			memset(m_pData + m_nSize*m_nElementSize, 0, (nNewSize-m_nSize)*m_nElementSize);
		}

		m_nSize = nNewSize;
	}
	else
	{
		// Otherwise grow array
		int nNewMax;
		if (nNewSize < m_nMaxSize + m_nGrowBy)
			nNewMax = m_nMaxSize + m_nGrowBy;  // granularity
		else
			nNewMax = nNewSize;  // no slush

#ifdef SIZE_T_MAX
		ASSERT((long)nNewMax * m_nElementSize <= SIZE_T_MAX);  // no overflow
#endif
		BYTE* pNewData = (BYTE*) new BYTE[nNewMax * m_nElementSize];

		// copy new data from old
		memcpy(pNewData, m_pData, m_nSize * m_nElementSize);

		// construct remaining elements
		ASSERT(nNewSize > m_nSize);

		memset(pNewData + m_nSize*m_nElementSize, 0, (nNewSize-m_nSize)*m_nElementSize);

		// get rid of old stuff (note: no destructors called)
		delete [] (BYTE*)m_pData;
		m_pData = pNewData;
		m_nSize = nNewSize;
		m_nMaxSize = nNewMax;
	}
}

void CArray::FreeExtra()
{
	ASSERT_VALID(this);

	if (m_nSize != m_nMaxSize)
	{
		// shrink to desired size
#ifdef SIZE_T_MAX
		ASSERT((long)m_nSize * m_nElementSize <= SIZE_T_MAX);  // no overflow
#endif
		BYTE* pNewData = NULL;
		if (m_nSize != 0)
		{
			pNewData = (BYTE*) new BYTE[m_nSize * m_nElementSize];
			// copy new data from old
			memcpy(pNewData, m_pData, m_nSize * m_nElementSize);
		}

		// get rid of old stuff (note: no destructors called)
		delete [] (BYTE*)m_pData;
		m_pData = pNewData;
		m_nMaxSize = m_nSize;
	}
}

/////////////////////////////////////////////////////////////////////////////


void CArray::SetAt(int nIndex, LPCARRAYELEMENT pNewElement)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0 && nIndex < m_nSize);
	memcpy(m_pData + nIndex*m_nElementSize, pNewElement, m_nElementSize);
}

void CArray::SetAtGrow(int nIndex, LPCARRAYELEMENT pNewElement)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0);

	if (nIndex >= m_nSize)
		SetSize(nIndex+1);

	SetAt(nIndex, pNewElement);
}

void CArray::InsertAt(int nIndex, LPCARRAYELEMENT pNewElement, int nCount /*=1*/)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0);    // will expand to meet need
	ASSERT(nCount > 0);     // zero or negative size not allowed

	if (nIndex >= m_nSize)
	{
		// adding after the end of the array
		SetSize(nIndex + nCount);  // grow so nIndex is valid
	}
	else
	{
		// inserting in the middle of the array
		int nOldSize = m_nSize;
		SetSize(m_nSize + nCount);  // grow it to new size
		// shift old data up to fill gap
		memmove(m_pData + (nIndex+nCount)*m_nElementSize,
				  m_pData + nIndex*m_nElementSize,
			(nOldSize-nIndex) * m_nElementSize);

		// re-init slots we copied from

		memset(m_pData + nIndex*m_nElementSize, 0, nCount * m_nElementSize);

	}

	// insert new value in the gap
	ASSERT(nIndex + nCount <= m_nSize);
	LPBYTE pData = m_pData + nIndex*m_nElementSize;
	while (nCount--)
	{
		memcpy(pData, pNewElement, m_nElementSize);
		pData += m_nElementSize;
	}
}

void CArray::RemoveAt(int nIndex, int nCount /* = 1 */)
{
	ASSERT_VALID(this);
	ASSERT(nIndex >= 0);
	ASSERT(nCount >= 0);
	ASSERT(nIndex + nCount <= m_nSize);

	// just remove a range
	int nMoveCount = m_nSize - (nIndex + nCount);

	if (nMoveCount)
	{
		memcpy(m_pData + nIndex*m_nElementSize,
				 m_pData + (nIndex + nCount)*m_nElementSize,
				 nMoveCount * m_nElementSize);
	}
	m_nSize -= nCount;
}

void CArray::InsertAt(int nStartIndex, CArray* pNewArray)
{
	ASSERT_VALID(this);
	ASSERT(pNewArray != NULL);
	ASSERT(pNewArray->IsKindOf(RUNTIME_CLASS(CArray)));
	ASSERT_VALID(pNewArray);
	ASSERT(nStartIndex >= 0);
	ASSERT(pNewArray->m_nElementSize == m_nElementSize);

	if (pNewArray->GetSize() > 0)
	{
		InsertAt(nStartIndex, pNewArray->GetAt(0), pNewArray->GetSize());
		for (int i = 0; i < pNewArray->GetSize(); i++)
			SetAt(nStartIndex + i, pNewArray->GetAt(i));
	}
}

/////////////////////////////////////////////////////////////////////////////
// Diagnostics

#ifdef _DEBUG
void CArray::Dump(CDumpContext& dc) const
{
	ASSERT_VALID(this);

#define MAKESTRING(x) #x
	AFX_DUMP1(dc, "a " MAKESTRING(CArray) " with ", m_nSize);
	AFX_DUMP0(dc, " elements");
#undef MAKESTRING

#if 0
	if (dc.GetDepth() > 0)
	{
		AFX_DUMP0(dc, "\n");
		for (int i = 0; i < m_nSize; i++)
		{
			AFX_DUMP1(dc, "\n\t[", i);
			AFX_DUMP1(dc, "] = ", m_pData[i]);
		}
	}
#endif
}

void CArray::AssertValid() const
{
	CObject::AssertValid();
	ASSERT(m_nElementSize != 0);
	if (m_pData == NULL)
	{
		ASSERT(m_nSize == 0);
		ASSERT(m_nMaxSize == 0);
	}
	else
	{
		ASSERT(m_nSize >= 0);
		ASSERT(m_nMaxSize >= 0);
		ASSERT(m_nSize <= m_nMaxSize);
		ASSERT(AfxIsValidAddress(m_pData, m_nMaxSize * m_nElementSize));
	}
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
