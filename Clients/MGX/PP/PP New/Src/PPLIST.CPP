/*=======================================================================*\

	CLIST.C - generic pointer list 

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/
#include "pp.h"

//=======================================================================
CPPList::CPPList(int nBlockSize)
//=======================================================================
{
	lpList = NULL;
	TRY
	{
		lpList = new CPtrList;
	}
	CATCH_ALL(e){}
	END_CATCH_ALL
}
	
//=======================================================================
CPPList::~CPPList()
//=======================================================================
{
	if (lpList)
		delete lpList;
}

//=======================================================================
int CPPList::GetCount() const
//=======================================================================
{
	return (lpList ? lpList->GetCount() : 0); 
}

//=======================================================================
BOOL CPPList::IsValid()
//=======================================================================
{
	return(lpList != NULL);
}

//=======================================================================
BOOL CPPList::IsEmpty() const
//=======================================================================
{
	return (lpList ? lpList->IsEmpty() : TRUE); 
}

//=======================================================================
void* CPPList::GetHeadPtr() const
//=======================================================================
{
	return (lpList ? lpList->GetHead() : NULL); 
}

//=======================================================================
void* CPPList::GetTailPtr() const
//=======================================================================
{
	return (lpList ? lpList->GetTail() : NULL); 
}

//=======================================================================
void* CPPList::RemoveHeadPtr()
//=======================================================================
{
	return (lpList ? lpList->RemoveHead() : NULL); 
}

//=======================================================================
void* CPPList::RemoveTailPtr()
//=======================================================================
{
	return (lpList ? lpList->RemoveTail() : NULL); 
}

//=======================================================================
POSITION CPPList::AddHead(void* newElement)
//=======================================================================
{
	TRY
	{
		return (lpList ? lpList->AddHead(newElement) : NULL); 
	}
	CATCH_ALL(e){}
	END_CATCH_ALL
	return(NULL);
}

//=======================================================================
POSITION CPPList::AddTail(void* newElement)
//=======================================================================
{
	TRY
	{
		return (lpList ? lpList->AddTail(newElement) : NULL); 
	}
	CATCH_ALL(e){}
	END_CATCH_ALL
	return(NULL);
}

//=======================================================================
BOOL CPPList::AddHead(CPPList* pNewList)
//=======================================================================
{
	TRY
	{
		if (lpList)
		{
			lpList->AddHead(pNewList->lpList);
			return(TRUE);
		}
	}
	CATCH_ALL(e){}
	END_CATCH_ALL
	return(FALSE);
}

//=======================================================================
BOOL CPPList::AddTail(CPPList* pNewList)
//=======================================================================
{
	TRY
	{
		if (lpList)
		{
			lpList->AddTail(pNewList->lpList);
			return(TRUE);
		}
	}					   
	CATCH_ALL(e){}
	END_CATCH_ALL
	return(FALSE);
}

//=======================================================================
void CPPList::RemoveAll()
//=======================================================================
{
	if(lpList)
		lpList->RemoveAll();
}

//=======================================================================
POSITION CPPList::GetHeadPosition() const
//=======================================================================
{
	return (lpList ? lpList->GetHeadPosition() : NULL); 
}

//=======================================================================
POSITION CPPList::GetTailPosition() const
//=======================================================================
{
	return (lpList ? lpList->GetTailPosition() : NULL); 
}

//=======================================================================
void* CPPList::GetNextPtr(POSITION& rPosition) // return *Position+
//=======================================================================
{
	return ((lpList && rPosition) ? lpList->GetNext(rPosition) : NULL); 
}

//=======================================================================
void* CPPList::GetPrevPtr(POSITION& rPosition) // return *Position-
//=======================================================================
{
	return ((lpList && rPosition) ? lpList->GetPrev(rPosition) : NULL); 
}

//=======================================================================
void* CPPList::GetAtPtr(POSITION position) const
//=======================================================================
{
	return ((lpList && position) ? lpList->GetAt(position) : NULL); 
}

//=======================================================================
void CPPList::SetAt(POSITION pos, void* newElement)
//=======================================================================
{
	if(lpList && pos)
		lpList->SetAt(pos, newElement);
}

//=======================================================================
void CPPList::RemoveAt(POSITION position)
//=======================================================================
{
	if(lpList && position)
		lpList->RemoveAt(position);
}

//=======================================================================
POSITION CPPList::InsertBefore(POSITION position, void* newElement)
//=======================================================================
{
	TRY
	{
		return ((lpList && position) ? lpList->InsertBefore(position,newElement) : NULL); 
	}
	CATCH_ALL(e){}
	END_CATCH_ALL
	return(NULL);
}

//=======================================================================
POSITION CPPList::InsertAfter(POSITION position, void* newElement)
//=======================================================================
{
	TRY
	{
		return ((lpList && position) ? lpList->InsertAfter(position,newElement) : NULL); 
	}
	CATCH_ALL(e){}
	END_CATCH_ALL
	return(NULL);
}

//=======================================================================
POSITION CPPList::Find(void* searchValue, POSITION startAfter) const
//=======================================================================
{
	return ((lpList && startAfter) ? lpList->Find(searchValue, startAfter) : NULL); 
}

//=======================================================================
POSITION CPPList::FindIndex(int nIndex) const
//=======================================================================
{
	return (lpList ? lpList->FindIndex(nIndex) : NULL); 
}

//=======================================================================
// 	Calls lpProc for each item in order.
//	If any of the calls returns false then execution stops and returns 
//		FALSE.
//=======================================================================
BOOL CPPList::ForEach(LPLISTPROC lpProc)
//=======================================================================
{
	POSITION pos;
	
	if (IsEmpty())
		return(TRUE);
	
	if (!(pos = GetHeadPosition()))
		return(TRUE);
		
	while (pos)
		if (!lpProc(GetNextPtr(pos)))
			return(FALSE);
	return(TRUE);
}

//=======================================================================
// 	Calls lpProc for each item in order.
//	If any of the calls returns false then execution stops and returns 
//		FALSE.
//=======================================================================
BOOL CPPList::ForEach1(LPLISTPROC1 lpProc, LONG lData)
//=======================================================================
{
	POSITION pos;
	
	if (IsEmpty())
		return(TRUE);
	
	if (!(pos = GetHeadPosition()))
		return(TRUE);
		
	while (pos)
		if (!lpProc(GetNextPtr(pos), lData))
			return(FALSE);
	return(TRUE);
}
