/*=================================================================
Contains:	Memory bottle neck functions. These functions should be replaced
			by the client - integrating stonehand memory management and
			the clients memory management.
=================================================================*/

#pragma once

#include "scexcept.h"

typedef long MEM_POOL;

inline void* scMemDeref( scMemHandle h )
{ 
	return *( (void**)(h) ); 
}

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

class scAutoUnlock
{
public:
	scAutoUnlock( scMemHandle hnd );
	~scAutoUnlock( void );

	void	*operator *() { return scMemDeref( fHandle ); }
	
private:
	scMemHandle fHandle;
};
