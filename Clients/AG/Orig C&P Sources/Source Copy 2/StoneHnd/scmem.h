/*=================================================================
 
	File:		MEM.H

	$Header: /Projects/Toolbox/ct/SCMEM.H 2 	5/30/97 8:45a Wmanis $

	Contains:	Memory bottle neck fucntions. These functions should be replaced
				by the client - integrating stonehand memory management and
				the clients memory management.

	Written by: Sealy

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

	@doc
	
=================================================================*/

#ifndef _H_MEM
#define _H_MEM

#ifdef SCMACINTOSH
	#pragma once
#endif

#include "scexcept.h"

#ifndef useSMARTHEAP
	typedef long MEM_POOL;
#endif

// @struct scPoolInfo | A structure for allocating mem pools.
struct scPoolInfo {
	size_t		fBlockSize; 	// @field Sizeof block for pool.
	MEM_POOL	fPool;			// @field See Smart Heap, for use by MEMInit.
};


// @CALLBACK This initializes memory for use by Composition Toolbox.
// Called from <f SCENG_Init>.
//
void		MEMInit(
					scPoolInfo pools[]	);		// @parm <t scPoolInfo> array is null terminated.

// @CALLBACK Called when Toolbox is closed with <f SCENG_Fini>.
void		MEMFini( void );


#if !defined( MEM_DEBUG )

// @CALLBACK Allocate fixed block, return ptr.
void*		MEMAllocPtr(
				ulong sz ); // @parm Size in bytes.

// @CALLBACK Allocate moveable block, return handle that can be dereferenced with **.
scMemHandle MEMAllocHnd(
				ulong sz ); // @parm Size in bytes.

//void* 	MEMAllocObj( ulong sz );	// allocate using object factory, return ptr

// @CALLBACK Duplicate a pointer.
void*		MEMDupPtr(
				 const void *ptr ); 	// @parm Pointer to dup.

// @CALLBACK Duplicate a handle.
scMemHandle MEMDupHnd(
				scMemHandle hnd );		// @parm Handle to dup.

//void* 	MEMDupObj( void * );

#endif	


// @CALLBACK Free a pointer.
void		MEMFreePtr(
				void *ptr );			// @parm Pointer to free.

// @CALLBACK Free a handle.
void		MEMFreeHnd(
				scMemHandle hnd );		// @parm Handle to free.
//void		MEMFreeObj( void * );


// @CALLBACK Resize a pointer.
void*		MEMResizePtr(
				void**	ptr,		// @parm Pointer to resize.
				ulong		sz );	// @parm New size in bytes.


// @CALLBACK Resize a Handle.
scMemHandle MEMResizeHnd(
				scMemHandle hnd,	// @parm Handle to resize.
				ulong		sz );	// @parm New size in bytes.

// @CALLBACK Get size in bytes of pointer.
ulong		MEMGetSizePtr(
				const void* ptr );		// @parm Pointer to size.

// @CALLBACK Get size in bytes of handle.
ulong		MEMGetSizeHnd(
				scMemHandle hnd ); // @parm Handle to size.

//ulong 	MEMGetSizeObj( void * );

// @CALLBACK Lock a handle, returns ptr to handle contents.
void*		MEMLockHnd(
				scMemHandle 	hnd,			// @parm Handle to lock.
				int 			counted = 1 );	// @parm If non-zero count the locks.


// @CALLBACK Unlock a handle.
void		MEMUnlockHnd(
				scMemHandle 	hnd,			// @parm Handle to unlock.
				int counted = 0 );				// @parm If non-zero count the locks.


#if SCDEBUG < 2
	inline void MEMValidate( void * ){}
#else
	void	MEMValidate( void *ptr );
#endif


#ifndef SCmemset		// we are in a 16 bit world

void scFar * scFar scCDecl SCmemset( void scFar *,
									 int,
									 long );

void scFar * scFar scCDecl SCmemmove( void scFar *,
									  const void scFar *,
									  long );

void scFar * scFar scCDecl SCmemcpy(	void scFar *,
										const void scFar *,
										long );
int scFar scCDecl SCmemcmp( const void scFar *,
							const void scFar *,
							long );

#endif	

#if defined( MEM_DEBUG )

void		memDumpMetrics( void );

void*		MEMAllocPtrDebug( ulong sz, const char *filename, int line );
scMemHandle MEMAllocHndDebug( ulong sz, const char *filename, int line );

void*		MEMDupPtrDebug( const void *, const char *filename, int line );
scMemHandle MEMDupHndDebug( scMemHandle, const char *filename, int line );

void*		MEMResizePtrDebug( void **, ulong sz, const char* file, int line );
scMemHandle MEMResizeHndDebug( scMemHandle, ulong sz, const char* file, int line );

#define MEMAllocPtr( sz )		MEMAllocPtrDebug( (sz), __FILE__, __LINE__ )
#define MEMAllocHnd( sz )		MEMAllocHndDebug( (sz), __FILE__, __LINE__ )

#define MEMResizeHnd( h, sz )	MEMResizeHndDebug( (h), (sz), __FILE__, __LINE__ )
#define MEMResizePtr( p, sz )	MEMResizePtrDebug( (p), (sz), __FILE__, __LINE__ )

#define MEMDupPtr( p )			MEMDupPtrDebug( (p), __FILE__, __LINE__ )
#define MEMDupHnd( p )			MEMDupHndDebug( (p), __FILE__, __LINE__ )


//void* 	MEMAllocObjDebug( ulong sz, const char *filename, int line );
//void* 	MEMDupObjDebug( void *, const char *filename, int line );
//#define	MEMAllocObj( sz )	MEMAllocObjDebug( (sz), __FILE__, __LINE__ )
//#define MEMDupObj( p )		MEMDupObjDebug( (p), __FILE__, __LINE__ )

#endif	/* SCDEBUG */

/* ======================================================================== */
/* ======================================================================== */

class scAutoUnlock {
public:
			scAutoUnlock( scMemHandle hnd );
		   ~scAutoUnlock( void );

	void	*operator *() { return scMemDeref( fHandle ); }
	
private:
	scMemHandle fHandle;
};

/* ======================================================================== */

// The scStackMem is a convenient way to allocate some temporary
// memopry without having to worry about freeing it.
// since it is rather unsafe to create a stack object that 
// allocates memory we will create the next best thing,
// the object will be created storing the desired size,
// and then the fucntion can get the memory by calling Init.
// The memory will be freed by the contstructor or may be freed
// by the user using Free. Resize can resize the memory if needed

/* ==================================================================== */
/* ==================================================================== */

#endif /* _H_MEM */
