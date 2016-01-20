
////////////////////////////////////////////////////////////////////////////////
//
//	(c) Copyright 1993 Monaco Systems Inc.
//	mcomem.c
//
//	Defines Mco Memory methods
//
//	Create Date:	8/8/93
//
//	Revision History:
//
////////////////////////////////////////////////////////////////////////////////

#include "mcotypes.h"
#include "mcomem.h"
#include "errordial.h"

//#define DBG_MODE

#ifdef DBG_MODE
#include "mem_dbg.h"
#endif 	

int32 McoTotMem = 0;
int32 McoNumH = 0;

#ifndef PHOTO_MEM

int32 	McoLockNumH = 0;
int32 	McoNumP = 0;
int32	totalSize = 0;
int32	contigSize = 0;
int32	mem_count = 0;



void McoTestMem(void)
{
	PurgeSpace(&totalSize, &contigSize);
}

long GetFreeMem(void)
{
return FreeMem();
}

McoHandle McoNewHandle(int32 size)
 {
 	McoHandle h;
 	int32 cSize;
 	#ifdef MemBug
 	int32	temp;
 	#endif

 	h = NewHandle(size);
 	
 	#ifdef MemBug
 	temp = GetHandleSize(h);
 	#endif
 	
 	if(h==0L) {
 		// Try compacting the heap
 		cSize = CompactMem(size);
 		if(cSize>=size)
 			h = NewHandle(size);
 	}

	

#ifdef DBG_MODE
	debug_newhand(size,h);
#endif 	

 	return h;
 }
 
void* McoLockHandle(McoHandle h)
 {
#ifdef DBG_MODE
	debug_checkhand(h);
#endif 	

 	HLock(h);
 	return *h;
 }

void McoUnlockHandle(McoHandle h)
 {
#ifdef DBG_MODE
	debug_checkhand(h);
#endif 	
 	HUnlock(h);
 }

void McoDeleteHandle(McoHandle h)
 {

#ifdef DBG_MODE
	debug_freehand(h);
#endif 	

 	DisposeHandle(h);


 }
 
 void* McoLockHandleHi(McoHandle h)
{
#ifdef DBG_MODE
	if (h == 0L)  McoErrorAlert(MCO_INVALID_PTR);
#endif 	
	HLockHi(h);
	return *h;
}

void *McoMalloc(int32 size)
{
	void * ptr;
	
#ifdef DBG_MODE
	ptr = NewPtr(size);
	debug_newptr(size,ptr);

	return ptr;	
#else
	ptr = NewPtr(size);
#endif
}

void McoFree(void *thePtr)
{
#ifdef DBG_MODE
	debug_freeptr(thePtr);
#endif

	DisposPtr((Ptr)thePtr);	

}
 
 int32 McoNumHandles()
  {
 	return McoNumH; 
  }
  
 int32 McoTotalMem()
  {
  return McoTotMem;
  }
  
void McoCheckSafe(McoHandle h, unsigned char *s)
 {
 #ifdef MemBug
 unsigned char c;
 
 c = HGetState((Handle)h);
 if ((c & 128) ^ 128) McoMessAlert(MCO_SAFE_MEM,s);
 #endif
 }
  
  
McoSafeMem::McoSafeMem(void)
{
h = 0L;
}

McoSafeMem::~McoSafeMem(void)
{
}

/* This was modified by James to support Peter's debugging (sept 1)*/

Boolean McoSafeMem::AllocSpace(int32 size)
{
 	int32 cSize;
 	
 	h = 0L;
	Safe = FALSE;
	LockCount = 0;
 	
 	h = NewHandle(size);
 	
 	if(h==0L) {
 		// Try compacting the heap
 		cSize = CompactMem(size);
 		if(cSize>=size)
 			h = NewHandle(size);
 	}
 	
 #ifdef TEST_MODE 	
 	if(h){ 	
 		int32 i;
	 	McoNumH++;
	 	McoTotMem += size;
		for(i = 0; i < TOTALTESTNUM; i++){
			if(lockaddress[i] == 0){
				lockaddress[i] = (long)h;
				break;
			}
		}
 	}
#endif	
 	
 if (h==0L) return FALSE;
 return TRUE;
 }

/* end of mod */

 
 void* McoSafeMem::Lock(void)
 {
 if (Safe == FALSE) 
 	{
 	HLock(h);
 	Safe = TRUE;
 	LockCount ++;
 	return *h;
 	}
 return *h;
 }
 
 void McoSafeMem::Unlock(void)
 {
 if (Safe)
 	{
 	HUnlock(h);
 	Safe = FALSE;
 	}
#ifdef MemBug
 else 
 	{
 	McoMessAlert(MCO_SAFE_MEM,"\pUnlocking");
 	}
#endif
 }
 
 /* This was modified by James to support Peter's debugging (sept 1)*/
 
 void McoSafeMem::Dispose(void)
 {
 
#ifdef TEST_MODE
	int32 size, i;
	long	addr;
	
	addr = (long)h;
	
	size = GetHandleSize(h);
#endif
 
 DisposeHandle(h);
 h = 0L;
#ifdef TEST_MODE
 McoNumH--;
 McoTotMem -= size;
for(i = 0; i < TOTALTESTNUM; i++){
	if(lockaddress[i] == addr){
		lockaddress[i] = 1;
		break;
	}
}

#endif	
 
 }
 
 /* End of mod */
 
 
 Boolean McoSafeMem::IsSafe(void)
 {
 return Safe;
 }
 
 void McoSafeMem::CheckSafe(int32 where)
 {
 Str255 s;
 
 #ifdef MemBug
 if (Safe == FALSE) 
 	{
 	NumToString(where,s);
 	McoMessAlert(MCO_SAFE_MEM,s);
 	}
 #endif
 }
 
 
 void McoSafeMem::CopyFromStruct(McoSafeMemStruct *sms)
 {
 h = sms->h;
 Safe = sms->Safe;
 LockCount = sms->LockCount;
 }
 
  void McoSafeMem::CopyToStruct(McoSafeMemStruct *sms)
 {
 sms->h = h;
 sms->Safe = Safe;
 sms->LockCount = LockCount;
 }
 
 
 // The following copies one piece of memory to another
 
 void CopyMem(char **out, void *in, int32 num)
 {
 int i;
 
 for (i=0; i<num; i++)
 	{
 	*(*out+i) = *((char*)(in)+i);
 	}
 (*out) += num;
 }
 
  // The following copies one piece of memory to another
 
 void CopyMem2(char **out, void *in, int32 num)
 {
 int i;
 
 for (i=0; i<num; i++)
 	{
 	*((char*)(in)+i) = *(*out+i);
 	}
(*out) += num;
 }
 
 
// Use the photoshop memory handles 
 
#else

#include "PIUtilities.h"

McoHandle McoNewHandle(int32 size)
 {
 	McoHandle h;
 	int32 cSize;
 	
 	h = PINewHandle(size);
 	
 	
 	McoNumH++;
 	
 	return h;
 
 }
void* McoLockHandle(McoHandle h)
 {
 	PILockHandle(h,FALSE);
 	
 	return *h;
 }

void McoUnlockHandle(McoHandle h)
 {
 #ifdef MemBug
 unsigned char s;
  s = HGetState((Handle)h);
 if ((s & 128) ^ 128) McoMessAlert(MCO_SAFE_MEM,"\pAt an unlock");
 #endif
 	PIUnlockHandle(h);
 }
 
void McoDeleteHandle(McoHandle h)
 {
 	PIDisposeHandle(h);
 	
 	McoNumH--;
 }
 
 int32 McoNumHandles()
  {
 	return McoNumH; 
  }
  
void McoCheckSafe(McoHandle h, unsigned char *s)
 {
 #ifdef MemBug
 unsigned char c;
 
 c = HGetState((Handle)h);
 if ((c & 128) ^ 128) McoMessAlert(MCO_SAFE_MEM,s);
 #endif
 }
  
  
McoSafeMem::McoSafeMem(void)
{
}

McoSafeMem::~McoSafeMem(void)
{
}

Boolean McoSafeMem::AllocSpace(int32 size)
{
 	int32 cSize;
 	
 	h = 0L;
	Safe = FALSE;
	LockCount = 0;
 	
 	h = PINewHandle(size);
 	
 	
 if (h==0L) return FALSE;
 return TRUE;
 }
 
 void* McoSafeMem::Lock(void)
 {
 if (Safe == FALSE) 
 	{
 	PILockHandle(h,FALSE);
 	Safe = TRUE;
 	LockCount ++;
 	return *h;
 	}
 return *h;
 }
 
 void McoSafeMem::Unlock(void)
 {
 if (Safe)
 	{
 	PIUnlockHandle(h);
 	Safe = FALSE;
 	}
#ifdef MemBug
 else 
 	{
 	McoMessAlert(MCO_SAFE_MEM,"\pUnlocking");
 	}
#endif
 }
 
 void McoSafeMem::Dispose(void)
 {
 PIDisposeHandle(h);
 }
 
 Boolean McoSafeMem::IsSafe(void)
 {
 return Safe;
 }
 
 void McoSafeMem::CheckSafe(int32 where)
 {
 Str255 s;
 
 #ifdef MemBug
 if (Safe == FALSE) 
 	{
 	NumToString(where,s);
 	McoMessAlert(MCO_SAFE_MEM,s);
 	}
 #endif
 }
 
 
 void McoSafeMem::CopyFromStruct(McoSafeMemStruct *sms)
 {
 h = sms->h;
 Safe = sms->Safe;
 LockCount = sms->LockCount;
 }
 
  void McoSafeMem::CopyToStruct(McoSafeMemStruct *sms)
 {
 sms->h = h;
 sms->Safe = Safe;
 sms->LockCount = LockCount;
 }
 
 
 // The following copies one piece of memory to another
 
 void CopyMem(char **out, void *in, int32 num)
 {
 int i;
 
 for (i=0; i<num; i++)
 	{
 	*(*out+i) = *((char*)(in)+i);
 	}
 (*out) += num;
 }
 
  // The following copies one piece of memory to another
 
 void CopyMem2(char **out, void *in, int32 num)
 {
 int i;
 
 for (i=0; i<num; i++)
 	{
 	*((char*)(in)+i) = *(*out+i);
 	}
(*out) += num;
 }
 
#endif
 
 	