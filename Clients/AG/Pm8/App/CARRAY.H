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
// $Date: 3/03/99 6:03p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/CARRAY.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 2     12/04/97 2:39p Hforman
// CArray -> CVarArray
// 
//    Rev 1.0   14 Aug 1997 15:18:46   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:54   Fred
// Initial revision.
// 
//    Rev 1.1   07 Apr 1997 14:14:52   dennisp
// Added GetData()
// 
//    Rev 1.0   04 Jun 1996 17:38:54   Jay
//  
*/

#ifndef __CARRAY_H__
#define __CARRAY_H__

typedef void FAR* LPARRAYELEMENT;
typedef const void FAR* LPCARRAYELEMENT;

class CVarArray : public CObject
{
	DECLARE_DYNAMIC(CVarArray)
public:

// Construction
	CVarArray(int nElementSize = 0);

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

	LPARRAYELEMENT GetData();

	// Potentially growing the array
	void SetAtGrow(int nIndex, LPCARRAYELEMENT pNewElement);
	int Add(LPCARRAYELEMENT pNewElement);

	// overloaded operator helpers
	LPCARRAYELEMENT operator[](int nIndex) const;
	LPARRAYELEMENT operator[](int nIndex);

	// Operations that move elements around
	void InsertAt(int nIndex, LPCARRAYELEMENT pNewElement, int nCount = 1);
	void RemoveAt(int nIndex, int nCount = 1);
	void InsertAt(int nStartIndex, CVarArray* pNewArray);

// Implementation
protected:
	BYTE* m_pData;			// the actual array of data
	int m_nElementSize;	// size of an element in bytes
	int m_nSize;			// # of elements (upperBound - 1)
	int m_nMaxSize;		// max allocated
	int m_nGrowBy;			// grow amount

public:
	virtual ~CVarArray();

#ifdef _DEBUG
	void Dump(CDumpContext&) const;
	void AssertValid() const;
#endif
};

inline void CVarArray::SetElementSize(int nElementSize)
	{ m_nElementSize = nElementSize;}
inline int CVarArray::GetElementSize() const
	{ return m_nElementSize; }
inline int CVarArray::GetSize() const
	{ return m_nSize; }
inline int CVarArray::GetUpperBound() const
	{ return m_nSize-1; }
inline void CVarArray::RemoveAll()
	{ SetSize(0); }
inline LPCARRAYELEMENT CVarArray::GetAt(int nIndex) const
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return (LPCARRAYELEMENT)(m_pData + nIndex*m_nElementSize); }
inline LPARRAYELEMENT CVarArray::ElementAt(int nIndex)
	{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
		return (LPARRAYELEMENT)(m_pData + nIndex*m_nElementSize); }
inline LPARRAYELEMENT CVarArray::GetData()
	{ return ElementAt(0); }
inline int CVarArray::Add(LPCARRAYELEMENT pNewElement)
	{ int nIndex = m_nSize;
		SetAtGrow(nIndex, pNewElement);
		return nIndex; }
inline LPCARRAYELEMENT CVarArray::operator[](int nIndex) const
	{ return GetAt(nIndex); }
inline LPARRAYELEMENT CVarArray::operator[](int nIndex)
	{ return ElementAt(nIndex); }


#endif					// #ifndef __CARRAY_H__
