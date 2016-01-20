// ifaced.h : Header for the derived class 'CBEInterfaceD'
//

#ifndef __IFACED_H__
#define __IFACED_H__

#include "borddef.h"

/////////////////////////////////////////////////////////////////////////////
// CBEInterfaceD - interface definition for border DLL methods
//                This object is created by the border DLL.

class CBEInterfaceD : public CBEInterface
{
// Interface
public:
	// Release this interface. The interface is not valid after the call.
	virtual int Release(void);

	// Create a new instance of a border object.
	// The object will have default settings unless creation data is passed in.
	virtual int NewObject(CBEObject*& pObject,
								 void* pDatabase,
								 int nLength = 0,
								 void* pData = NULL);

	// Free a DLL-created draw state.
	virtual int FreeDrawState(void* pDrawState);
};

#endif
