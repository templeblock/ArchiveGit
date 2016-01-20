#ifndef __pplist_h__
#define __pplist_h__
/*=======================================================================*\

	PPLIST.H - generic pointer list 

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include "root.h"

typedef BOOL (FAR *LPLISTPROC)(LPVOID lpItem);
typedef BOOL (FAR *LPLISTPROC1)(LPVOID lpItem, LONG lData);

class CPPList : public CPPRoot
{
public:
// Construction
	CPPList(int nBlockSize=10);
	~CPPList();
	BOOL IsValid();
	// count of elements
	int GetCount() const;
	BOOL IsEmpty() const;
	// peek at head or tail
	void* GetHeadPtr() const;
	void* GetTailPtr() const;
	// get head or tail (and remove it) - don't call on empty list!
	void* RemoveHeadPtr();
	void* RemoveTailPtr();
	// add before head or after tail
	POSITION AddHead(void* newElement);
	POSITION AddTail(void* newElement);
	// add another list of elements before head or after tail
	BOOL AddHead(CPPList* pNewList);
	BOOL AddTail(CPPList* pNewList);
	// remove all elements
	void RemoveAll();
	// iteration
	POSITION GetHeadPosition() const;
	POSITION GetTailPosition() const;
	void* GetNextPtr(POSITION& rPosition); // return *Position++
	void* GetPrevPtr(POSITION& rPosition); // return *Position--

	// getting/modifying an element at a given position
	void* GetAtPtr(POSITION position) const;
	void SetAt(POSITION pos, void* newElement);
	void RemoveAt(POSITION position);

	// inserting before or after a given position
	POSITION InsertBefore(POSITION position, void* newElement);
	POSITION InsertAfter(POSITION position, void* newElement);

	// helper functions (note: O(n) speed)
	POSITION Find(void* searchValue, POSITION startAfter = NULL) const;
						// defaults to starting at the HEAD
						// return NULL if not found
	POSITION FindIndex(int nIndex) const;
						// get the 'nIndex'th element (may return NULL)
						
	BOOL ForEach(LPLISTPROC lpProc);
	BOOL ForEach1(LPLISTPROC1 lpProc, LONG lData);
	
private:
	CPtrList FAR *lpList;
};

class CPPClassList : public CPPList
{
public:
// Construction
	CPPClassList(int nBlockSize=10) : CPPList() {}
	~CPPClassList() {}
	inline CPPRoot FAR* GetHeadClass() const 
			{return((LPCPPRoot)GetHeadPtr());}
	inline LPCPPRoot GetTailClass() const 
			{return((LPCPPRoot)GetTailPtr());}
	inline LPCPPRoot RemoveHeadClass() 
			{return((LPCPPRoot)RemoveHeadPtr());}
	inline LPCPPRoot RemoveTailClass() 
			{return((LPCPPRoot)RemoveTailPtr());}
	inline POSITION AddHead(LPCPPRoot newElement) 
			{return(CPPList::AddHead((void *)newElement));}
	inline POSITION AddTail(LPCPPRoot newElement) 
			{return(CPPList::AddTail((void *)newElement));}
	inline BOOL AddHead(CPPClassList* pNewList) 
			{return(CPPList::AddHead((CPPList *)pNewList));}
	inline BOOL AddTail(CPPClassList* pNewList) 
			{return(CPPList::AddTail((CPPList *)pNewList));}
	inline LPCPPRoot GetNextClass(POSITION& rPosition)
			{return((LPCPPRoot)GetNextPtr(rPosition));}
	inline LPCPPRoot GetPrevClass(POSITION& rPosition) 
			{return((LPCPPRoot)GetPrevPtr(rPosition));}
	inline LPCPPRoot GetAtClass(POSITION position) const 
			{return((LPCPPRoot)GetAtPtr(position));}
	inline void SetAt(POSITION pos, LPCPPRoot newElement) 
			{CPPList::SetAt(pos, (void *)newElement);}
	inline POSITION InsertBefore(POSITION position, void* newElement) 
			{return(InsertBefore(position, (void *)newElement));}
	inline POSITION InsertAfter(POSITION position, void* newElement) 
			 {return(InsertAfter(position, (void *)newElement));}
	inline POSITION Find(LPCPPRoot searchValue, POSITION startAfter = NULL) const 
			{return(CPPList::Find((void *)searchValue, startAfter));}
};

#endif /* __pplist_h__ */

