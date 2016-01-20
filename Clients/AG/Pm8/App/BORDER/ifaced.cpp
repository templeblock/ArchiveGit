// ifaced.cpp : Implements the derived class 'CBEInterfaceD'
//

#include "stdafx.h"
#include "ifaced.h"
#include "beobjd.h"
#include "border.h"

/////////////////////////////////////////////////////////////////////////////
// CBEInterfaceD
// Interface
int CBEInterfaceD::Release()
{
	delete this;
	return (0);
}

int CBEInterfaceD::NewObject(CBEObject*& pObject,
								 void* pDatabase,
								 int nLength,
								 void* pData)
{
	// Create a new Border Editor Object
	pObject = new CBEObjectD(pDatabase, nLength, pData);

	return (0);
}

int CBEInterfaceD::FreeDrawState(void* pDrawState)
{
	// Free the memory allocated by BEObject code
	if (pDrawState != NULL)
	{
		SRedrawStateT* pRedrawState = (SRedrawStateT*)pDrawState;
		theBorderApp.GetCallbacks()->FreeDrawState(pRedrawState->pDrawState);
		delete pRedrawState;
	}
	return (0);
}




