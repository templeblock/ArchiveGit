// ****************************************************************************
//
//  File Name:			tpsmem.h
//
//  Project:			TPS Libraries
//
//  Author:				Bruce Rosenblum
//
//  Description:		Macro wrappers for TPS memory APIs that call Smartheap
//
//  Portability:		Platform independent, 32 bit systems only
//
//  Developed by:		Turning Point Software
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Turning Point Software         
//
//  Copyright (C) 1996 Turning Point Software, Inc. All Rights Reserved.
//
//  $Logfile:: /PM8/TPSLibs/Include/TPSMEM.H                                  $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:35p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef  _tpsmem_h
#define  _tpsmem_h

#include	<string.h>

#include "tpsdefs.h"

#ifdef	_WINDOWS

#if defined(__cplusplus)
	extern "C" {
#endif /* __cplusplus */

//	Use only to get access to Native Windows handles, not SmartHeap handles
//		Ask permission of your project lead to use this macro!
#define	tmemLockNative( hm )						(GlobalLock( hm ))
#define	tmemDeRefNative( hm )					(GlobalUnlock( hm ),GlobalLock( hm ))
#define	tmemUnlockNative( hm )					(GlobalUnlock( hm ))

//	Use for accessing handle returned from the Resource Manager
#define	tmemLockNativeResource( hm )			(LockResource( hm ))
//	used for native Windows resources because tmemDeRefNative does not seem
//		to work on resource handles under Win32 - it returns NULL
//		Note that Win32 SDK ref states UnlockResource is not needed in Win32
#define	tmemDeRefNativeResource( hm )			(LockResource( hm ))
#define	tmemUnlockNativeResource( hm )		( TRUE )		//	doesn't do much, but at least it is TRUE
#define	tmemPurgeNativeResource( hm )			( TRUE )		//	see above

#ifdef	USE_SMARTHEAP

typedef	MEM_HANDLE	HTMEM;			//	TPS handle
typedef	HGLOBAL		HNATIVE;			//	Native memory handle

#define	TMEM_AFZEROINIT	MEM_ZEROINIT

//	If this is a debug build, set the SmartHeap debug flags
#ifdef	TPSDEBUG
#define	MEM_DEBUG	1
#endif

//	Include the smart heap header information
#include "smrtheap.h"

//	Note: any file that includes and uses this header must link to an
//		application that defines Get/SetGlobalSmartheapHandlePool.
extern MEM_POOL	GetGlobalSmartHeapHandlePool( void );
extern MEM_POOL	SetGlobalSmartHeapHandlePool( MEM_POOL mpPool );

//	MemPoolInit returns MEM_POOL or NULL on failure - tmemStartup returns BOOLEAN
#define	tmemStartup( wmFlags )					(NULL != ( SetGlobalSmartHeapHandlePool (MemPoolInit( MEM_POOL_DEFAULT ))) )

//	MemPoolFree returns MEM_BOOL true if successful
#define	tmemShutdown( )							(MemPoolFree( GetGlobalSmartHeapHandlePool() ))

//	MemAlloc returns MEM_HANDLE, or NULL on failure
#define	tmemAlloc( ulSize, wmFlags )			(MemAlloc( (GetGlobalSmartHeapHandlePool()), (wmFlags | MEM_MOVEABLE), (ulSize) ))

//	MemFree returns MEM_BOOL true if successful
#define	tmemFree( hm )								(MemFree( hm ))

//	MemReAlloc returns MEM_HANDLE hm, or NULL on failure
//	Note that we don't support passing flags to MemReAlloc because braindead
//	SmartHeap will only support MEM_ZEROINIT if the original call to MemAlloc
//	also had this flag set - since we can't assure it was set before, you have
//	to handle it yourself to make sure that an extended block is set to 0 data
#define	tmemReSize( hm, ulSize )	(MemReAlloc( (hm), (ulSize), (MEM_MOVEABLE) ))

//	MemLock returns void *
#define	tmemLock( hm )								(MemLock( hm ))

//	MemUnlock returns unsigned int with new lock count
#define	tmemUnlock( hm )							(MemUnlock( hm ))

//	MEM_REFERENCE returns void *
#define	tmemDeRef( hm )							(MEM_REFERENCE( hm ))

//	MemHandle returns MEM_HANDLE
#define	tmemGetHandle( lpv )						(MemHandle( lpv ))

//	MemLockCount returns unsigned int
#define	tmemGetLockCount( hm )					(MemLockCount( hm ))

//	MemSize returns unsigned long
#define	tmemGetSize( hm )							(MemSize( hm ))

//	MemAllocPtr returns void *
#define	tmemAllocNativePtr( ulSize )			(MemAllocPtr( (GetGlobalSmartHeapHandlePool()), ulSize, 0 ))

//	MemFreePtr returns MEM_BOOL true if successful
#define	tmemFreeNativePtr( lpv )				(MemFreePtr( lpv ))

#else	//	USE_SMARTHEAP

typedef	HGLOBAL		HTMEM;			//	TPS handle
typedef	HGLOBAL		HNATIVE;			//	Native memory handle

#define	TMEM_AFZEROINIT	0

//	Nothing for now, using system memory calls
#define	tmemStartup( wmFlags )					( TRUE )

//	Nothing started, nothing needed to shutdown
#define	tmemShutdown( )							( TRUE )

//	Return the native handle
#define	tmemAlloc( ulSize, wmFlags )			( GlobalAlloc( GMEM_MOVEABLE, ulSize ) )

//	Free the native handle
#define	tmemFree( hm )								( GlobalFree( (HTMEM)hm ) )

//	Resize the data as necessary
#define	tmemReSize( hm, ulSize )				( GlobalReAlloc( (HTMEM)hm, ulSize, GMEM_MOVEABLE ) )

//	Lock returns void *
#define	tmemLock( hm )								( GlobalLock( (HTMEM)hm ) )

//	Unlock returns nothing valid
#define	tmemUnlock( hm )							( GlobalUnlock( (HTMEM)hm ) )

//	returns void*
#define	tmemDeRef( hm )							( GlobalUnlock((HTMEM)hm),GlobalLock((HTMEM)hm) )

//	MemHandle returns MEM_HANDLE
#define	tmemGetHandle( lpv )						( GlobalHandle(lpv) )

//	Not A Valid call
#define	tmemGetLockCount( hm )					( 1 )

//	MemSize returns unsigned long
#define	tmemGetSize( hm )							( GlobalSize( (HTMEM)hm ) )

//	MemAllocPtr returns void *
#define	tmemAllocNativePtr( ulSize )			( malloc( ulSize ) )

//	MemFreePtr returns MEM_BOOL true if successful
#define	tmemFreeNativePtr( lpv )				( free( lpv ) )

#endif	//	USE_SMARTHEAP

#define	tmemZeroBlock( lpv, ulSize )			( memset( lpv, 0, ulSize ) )

#define	tmemCopyBlock( lpvSrc, lpvDst, ulSize)	( memcpy( lpvDst, lpvSrc, ulSize ) )

#if defined(__cplusplus)
	}
#endif /* __cplusplus */

#else		//	_WINDOWS

typedef	Handle	HTMEM;
typedef	Handle		HNATIVE;			//	Native memory handle

#define	TMEM_AFZEROINIT	0x01

#if defined(__cplusplus)
	extern "C" {
#endif /* __cplusplus */

//	MemPoolInit returns MEM_POOL or NULL on failure - tmemStartup returns BOOLEAN
#define	tmemStartup( wmFlags )					(TRUE)

//	MemPoolFree returns MEM_BOOL true if successful
#define	tmemShutdown( )							(NULL)

//	MemAlloc returns MEM_HANDLE, or NULL on failure
#define	tmemAlloc( ulSize, wmFlags )			(( wmFlags & TMEM_AFZEROINIT)? NewHandleClear( ulSize ) : NewHandle( ulSize ) )

//	MemFree returns MEM_BOOL true if successful
#define	tmemFree( hm )								(DisposeHandle( hm ))

//	MemReAlloc returns MEM_HANDLE hm, or NULL on failure
//	Note that we don't support passing flags to MemReAlloc because braindead
//	SmartHeap will only support MEM_ZEROINIT if the original call to MemAlloc
//	also had this flag set - since we can't assure it was set before, you have
//	to handle it yourself to make sure that an extended block is set to 0 data
#define	tmemReSize( hm, ulSize )				( (SetHandleSize( (hm), (ulSize) )), ((MemError() == noErr)? hm : NULL) )

//	MemLock returns void *
#define	tmemLock( hm )								(HLock( hm ), (*hm))

//	MemUnlock returns unsigned int with new lock count
#define	tmemUnlock( hm )							(HUnlock( hm ))

//	MEM_REFERENCE returns void *
#define	tmemDeRef( hm )							((void *)(*hm))

//	Use only to get access to Native Macintosh handles, not SmartHeap handles
#define	tmemLockNative( hm )						(HLock( hm ))
#define	tmemDeRefNative( hm )					( *(hm) )
#define	tmemUnlockNative( hm )					(HUnlock( hm ))

#define	tmemLockNativeResource( hm )			tmemLockNative( hm )
#define	tmemDeRefNativeResource( hm )			tmemDeRefNative( hm )
#define	tmemUnlockNativeResource( hm )		tmemUnlockNative( hm )
#define	tmemPurgeNativeResource( hm )			( HPurge( hm ) )

//	MemHandle returns MEM_HANDLE
//	Warning to yea who calls this function with a pointer not retrieved from a handle
#define	tmemGetHandle( lpv )						(RecoverHandle( lpv ))

//	MemLockCount returns unsigned int
#define	tmemGetLockCount( hm )					(1)

//	MemSize returns unsigned long
#define	tmemGetSize( hm )							(GetHandleSize( hm ))

//	MemAllocPtr returns void *
#define	tmemAllocNativePtr( ulSize )			(NewPtr( ulSize ))

//	MemFreePtr returns MEM_BOOL true if successful
#define	tmemFreeNativePtr( lpv )				(DisposePtr( (Ptr)lpv ))

#define	tmemZeroBlock( lpv, ulSize )			(memset( lpv, 0, ulSize ))

#define	tmemCopyBlock( lpvSrc, lpvDst, ulSize)	(memcpy( lpvDst, lpvSrc, ulSize ))

#if defined(__cplusplus)
	}
#endif /* __cplusplus */

#endif	//	_WINDOWS
// ****************************************************************************

// #define	TEST_TPS_MEMORY_HEADERS
#ifdef	TEST_TPS_MEMORY_HEADERS

#include "tpsdefs.h"
#include "tpsmem.h"

MEM_POOL	m_pool = NULL;

int main(void);
int main(void)
{

//	test code for this header resides in this file since there is no
//		corresponding .C or .CPP file. Move this function into a .C or .CPP
//		file and call it to test this header. If this code functions correctly,
//		then no asserts should trigger in the test routine.

#define	kTestMemBlocks		(50)
#define	kTestMemStartSize	(2L)				//	start with 2 byte block
#define	kTestMemBlockInc	(128L)

#define	kThisBlockSize			(kTestMemStartSize + (i * kTestMemBlockInc))

	BOOLEAN	fMemStartup;
	int		i, j, iUnlock;
	uLONG		ulSize;
	HTMEM		pHtMemBlocks[kTestMemBlocks];
	HTMEM		htMem;
	PSBYTE	psb;
	
	for(i = 0; i < kTestMemBlocks; i++)		//	clear the test array
		pHtMemBlocks[i] = NULL;

	fMemStartup = tmemStartup(0);
	TpsAssert(fMemStartup, "Memory Manage did not startup");

	for(i = 0; i < kTestMemBlocks; i++)
	{
		pHtMemBlocks[i] = tmemAlloc(kThisBlockSize, 0);
		TpsAssertL(pHtMemBlocks[i] != NULL, "Out of memory on alloc", &i, 1);
	}

	for(i = 0; i < kTestMemBlocks; i++)
	{
		psb = (PSBYTE)tmemLock(pHtMemBlocks[i]);
		TpsAssert(psb != NULL, "Bad memory lock");
		
		iUnlock = tmemGetLockCount(pHtMemBlocks[i]);
		TpsAssertL(iUnlock == 1, "Bad lock count after lock", &iUnlock, 1);
		
		for(j = 0; j < kThisBlockSize; j++)
			psb[j] = i % 256;
		
		tmemUnlock(pHtMemBlocks[i]);
	}	

	for(i = 0; i < kTestMemBlocks; i++)
	{
		htMem = tmemReSize(pHtMemBlocks[i],
						kThisBlockSize + kTestMemBlockInc);
		TpsAssertL(htMem != NULL, "Out of memory on re-alloc", &htMem, 1);
		TpsAssert(htMem == pHtMemBlocks[i], "Handle changed on resize");
		pHtMemBlocks[i] = htMem;
		
		ulSize = tmemGetSize(pHtMemBlocks[i]);
		TpsAssertL(ulSize != (uLONG)(kThisBlockSize + kTestMemBlockInc),
						"Incorrect block size", &ulSize, 1);
	}

	for(i = 0; i < kTestMemBlocks; i++)
	{
		psb = (PSBYTE)tmemDeRef(pHtMemBlocks[i]);
		TpsAssert(psb != NULL, "Bad memory deref");

		for(j = 0; j < kThisBlockSize; j++)
			TpsAssert(psb[j] == i % 256, "Bad block data")
			
		htMem = tmemGetHandle(psb);
		TpsAssert(htMem == pHtMemBlocks[i], "Handle from pointer is bad");
	}	

	for(i = 0; i < kTestMemBlocks; i++)
	{
		tmemFree(pHtMemBlocks[i]);
	}

	tmemShutdown();	
	return 1;
}

#endif	/* TEST_TPS_MEMORY_HEADERS */

#endif	/* _tpsmem_h */
