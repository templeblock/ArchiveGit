//memory wrapper function
#include "mcomem.h"

#if defined(_WIN32)
//#include "afxwin.h"
#include "stdlib.h"

//create new handle
McoHandle McoNewHandle(int32 size)
 {
	//return (McoHandle)GlobalAlloc(GMEM_MOVEABLE, size);
	return (McoHandle)(new char [size]);
 }
 
//lock handle
void* McoLockHandle(McoHandle h)
 {
 	//return GlobalLock((HGLOBAL)h);
	return (void*)h;
 }

//unlock handle
void McoUnlockHandle(McoHandle h)
 {
 	//GlobalUnlock((HGLOBAL)h);
	return;
 }

//delete handle
void McoDeleteHandle(McoHandle h)
 {
 	//GlobalFree((HGLOBAL)h); 	
	delete []((void*)h);
 }

//lock handle in high mem(for Mac)
void* McoLockHandleHi(McoHandle h)
{
	//return GlobalLock((HGLOBAL)h);
	return (void*)h;
}

//malloc
void *McoMalloc(int32 size)
{
	//return GlobalAlloc(GMEM_FIXED, size);
//	return (void*)malloc(size);
	return (void*)(new char [size]);
}

//free
void McoFree(void *thePtr)
{
	//GlobalFree((HGLOBAL)thePtr);
	delete []((void*)thePtr);
//	free(thePtr);
}

#else

McoHandle McoNewHandle(int32 size)
 {
 	McoHandle h;
 	int32 cSize;
 	
 	h = NewHandle(size);
 	
 	if(h==NULL) {
 		// Try compacting the heap
 		cSize = CompactMem(size);
 		if(cSize>=size)
 			h = NewHandle(size);
 	}

 	return h;
 }
 
void* McoLockHandle(McoHandle h)
 {
 	HLock(h);
 	
 	return *h;
 }

void McoUnlockHandle(McoHandle h)
 {
 	HUnlock(h);
 }

void McoDeleteHandle(McoHandle h)
 {
 	DisposeHandle(h);	
 }
 
 void* McoLockHandleHi(McoHandle h)
{
	HLockHi(h);
	return *h;
}

void *McoMalloc(int32 size)
{	
	return NewPtr(size);
}

void McoFree(void *thePtr)
{

	DisposPtr((Ptr)thePtr);	
}

#endif 
 	