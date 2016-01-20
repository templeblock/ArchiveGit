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
// $Workfile: CARRAY.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/
/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/CARRAY.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:35:56   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:12   Fred
// Initial revision.
// 
//    Rev 1.0   20 May 1997 10:40:26   johno
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
// $Log: /PM8/App/LICDLL/CARRAY.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   08 Sep 1996 09:40:44   Fred
// Initial revision.
// 
//    Rev 1.0   02 Feb 1996 08:59:04   JAY
// Initial revision.
// 
//    Rev 1.0   02 Dec 1994 15:29:08   JAY
// Initial revision.
*/

#ifndef __CARRAY_H__
#define __CARRAY_H__

typedef void FAR* LPARRAYELEMENT;
typedef const void FAR* LPCARRAYELEMENT;

class CArray : public CObject
{
	DECLARE_DYNAMIC(CArray)
public:

// Construction
	CArray(int nElementSize = 0);

// Attributes
	int GetSize() const;
	int GetUpperBound() const;
	void SetSize(int nNewSize, int nGrowBy = -1);
	void SetElementSize(int nElementSize);
	int GetElementSize(void) const;

// Operations
	// Clean up
	void FreeExtra();
	void RemoveAll();

	// Accessing elements
	LPCARRAYELEMENT GetAt(int nIndex) const;
	void SetAt(int nIndex, LPCARRAYELEMENT pNewElement);
	LPARRAYELEMENT ElementAt(int nIndex);

	// Potentially growing the array
	void SetAtGrow(int nIndex, LPCARRAYELEMENT pNewElement);
	int Add(LPCARRAYELEMENT pNewElement);

	// overloaded operator helpers
	LPCARRAYELEMENT operator[](int nIndex) const;
	LPARRAYELEMENT operator[](int nIndex);

	// Operations that move elements around
	void InsertAt(int nIndex, LPCARRAYELEMENT pNewElement, int nCount = 1);
	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CArray* pNewArray);

// Implementation
protected:
	BYTE* m_pData;			// the actual array of data
	int m_nElementSize;	// size of an element in bytes
	int m_nSize;			// # of elements (upperBound - 1)
	int m_nMaxSize;		// max allocated
	int m_nGrowBy;			// grow amount

public:
	virtual ~CArray();

#ifdef _DEBUG
	void Dump(CDumpContext&) const;
	void AssertValid() const;
#endif
};

inline void CArray::SetElementSize(int nElementSize)
	{ m_nElementSize = nElementSize;}
inline int CArray::GetElementSize() const
	{ return m_nElementSize; }
inline int CArray::GetSize() const
	{ return m_nSize; }
inline int CArray::GetUpperBound() const
	{ return m_nSize-1; }
inline void CArray::RemoveAll()
	{ SetSize(0); }
inline LPCARRAYELEMENT CArray::GetAt(int nIndex) const
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return (LPCARRAYELEMENT)(m_pData + nIndex*m_nElementSize); }
inline LPARRAYELEMENT CArray::ElementAt(int nIndex)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return (LPARRAYELEMENT)(m_pData + nIndex*m_nElementSize); }
inline int CArray::Add(LPCARRAYELEMENT pNewElement)
	{ int nIndex = m_nSize;
		SetAtGrow(nIndex, pNewElement);
		return nIndex; }
inline LPCARRAYELEMENT CArray::operator[](int nIndex) const
	{ return GetAt(nIndex); }
inline LPARRAYELEMENT CArray::operator[](int nIndex)
	{ return ElementAt(nIndex); }


#endif					// #ifndef __CARRAY_H__
