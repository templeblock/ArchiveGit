/*
// $Header: /PM8/App/DYNARRAY.H 1     3/03/99 6:05p Gbeddow $
//
// $Workfile: DYNARRAY.H $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/DYNARRAY.H $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 2     10/22/98 12:38p Mwilson
// added include
// 
//    Rev 1.0   14 Aug 1997 15:20:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:40   Fred
// Initial revision.
// 
//    Rev 1.4   01 May 1997 12:34:40   Jay
// Changes for huge 16-bit arrays.
// 
//    Rev 1.3   15 Apr 1997 14:29:16   dennisp
// Add RemoveAt()
// 
//    Rev 1.2   20 Jan 1997 09:39:32   Fred
// GetElement is now fast, use GetElementSafe to be safe
// 
//    Rev 1.1   12 Jul 1996 16:46:12   Jay
// Added Compare().
// 
//    Rev 1.0   14 Mar 1996 13:46:38   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:44   FRED
// Initial revision.
// 
//    Rev 1.4   13 May 1995 11:14:42   JAY
//  
// 
//    Rev 1.3   09 May 1995 08:04:22   JAY
// Added InsertElement.
// 
//    Rev 1.2   05 May 1995 16:49:32   JAY
// Exciting new additions to OutlinePaths!
// 
//    Rev 1.1   27 Apr 1995 13:20:16   FRED
// 
//    Rev 1.0   25 Apr 1995 10:33:52   FRED
// Initial revision.
*/

class CDynamicArray;

#ifndef __DYNARRAY_H__
#define __DYNARRAY_H__

#define DYNARRAY_IO
#include "storeman.h"

/////////////////////////////////////////////////////////////////////////////
// CDynamicArray

class CDynamicArray
{
public:
	CDynamicArray(DWORD dwElementSize, DWORD dwIncrement, DWORD dwMaximum);
	virtual ~CDynamicArray(void);
protected:
	void Reset(BOOL fConstructing = FALSE);
	void Allocate(DWORD dwCount);
	void SetElementCount(DWORD dwCount);
public:
	BOOL AddElement(LPVOID pElement, DWORD dwCount = 1);
	BOOL InsertElement(DWORD dwIndex, LPVOID pElement, DWORD dwCount = 1);
   BOOL RemoveAt(DWORD dwIndex, DWORD dwCount = 1, BOOL bPack = FALSE);
	LPVOID GetElementSafe(DWORD dwIndex, DWORD dwCount = 1);
	LPVOID GetElement(DWORD dwIndex)
#ifdef WIN32
		{ return m_pArray+(dwIndex*m_dwElementSize); }
#else
		{ return ((BYTE huge*)m_pArray)+(dwIndex*m_dwElementSize); }
#endif
	DWORD GetElementCount(void);
	DWORD GetElementSize(void);
	void Pack(void);
	void Free(void);
	BOOL Assign(CDynamicArray* pArray);
	BOOL Copy(const CDynamicArray* pArray);
	BOOL Compare(const CDynamicArray& Array);
#ifdef DYNARRAY_IO
	virtual ERRORCODE ReadData(StorageDevice* pDevice);
	virtual ERRORCODE WriteData(StorageDevice* pDevice);
	virtual ST_MAN_SIZE SizeofData(StorageDevice* pDevice);
#endif
protected:
	HGLOBAL m_hArray;
	LPBYTE m_pArray;
	DWORD m_dwElementSize;
	DWORD m_dwCount;
	DWORD m_dwAllocated;
	DWORD m_dwIncrement;
	DWORD m_dwMaximum;
};

#endif
