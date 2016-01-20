
#ifndef MCO_MEM
#define MCO_MEM
////////////////////////////////////////////////////////////////////////////////
//
//	(c) Copyright 1993 Monaco Systems Inc.
//	mcomem.h
//
//	Defines Mco Memory methods
//
//	Create Date:	8/8/93
//
//	Revision History:
//
////////////////////////////////////////////////////////////////////////////////

#include "mcotypes.h"

long GetFreeMem(void);

McoHandle McoNewHandle(int32);

void McoDeleteHandle(McoHandle);

void* McoLockHandle(McoHandle);

void* McoLockHandleHi(McoHandle h);

void McoUnlockHandle(McoHandle);

void McoCheckSafe(McoHandle h,unsigned char *s);

void *McoMalloc(int32 size);

void McoFree(void *thePtr);

extern int32 	McoTotMem;
extern int32	McoNumH;
extern int32 	McoNumP;
extern int32 	McoLockNumH;
extern int32	totalSize;
extern int32	contigSize;
extern int32	mem_count;

void McoTestMem(void);

// Get the count of current number of handles allocated
// using McoNewHandle
int32 McoNumHandles(void);

//Get the current amount of allocated memory
int32 McoTotalMem(void);

// The new memory methods that keep track of safe and unsafe use of the memory
// A special debuging flag allows each use of memory to be checked

typedef struct {
	Handle 		h;
	Boolean		Safe;
	int32		LockCount;
} McoSafeMemStruct;

class McoSafeMem {

private:
protected:
	Handle 		h;
	Boolean		Safe;
	int32		LockCount;
	
public:

	McoSafeMem(void);
	~McoSafeMem(void);
	Boolean AllocSpace(int32); 
	void* Lock(void);
	void  Unlock(void);
	void  Dispose(void);
	Boolean IsSafe(void);
	// The following displays a message that memory access is unsafe 
	void    CheckSafe(int32);
	void    CopyFromStruct(McoSafeMemStruct *sms);
	void    CopyToStruct(McoSafeMemStruct *sms);
	};	

void CopyMem(char **out, void *in, int32 num);
void CopyMem2(char **out, void *in, int32 num);

#endif