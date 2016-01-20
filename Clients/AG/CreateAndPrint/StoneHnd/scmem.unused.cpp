/****************************************************************************

	File:		MEM.C

	$Header: /Projects/Toolbox/ct/SCMEM.CPP 2     5/30/97 8:45a Wmanis $

	Contains:	Memory management routines based on our own heap managers

	Written by:	Sealy

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	
*****************************************************************************/

#include "scmem.h"

#if !useSMARTHEAP

#include <malloc.h>

struct MacHandle {
	const void*	fBlock;
	int			fCount;

			MacHandle( scMemHandle	ptr ) :
						fBlock( (char*)ptr + sizeof( MacHandle ) ),
						fCount( 0 ){}
			
	void*	Lock( void ) { scAssert( fCount >= 0 ); fCount++; return (void*)fBlock; }
	void	Unlock( void ) { scAssert( fCount > 0 ); --fCount; }
};

#endif


#include "scexcept.h"
#include <string.h>

#if SCDEBUG > 1
	#include <stdlib.h>  // for rand
#endif

/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */

#if useSMARTHEAP > 0

static MEM_POOL		hndPool;

static int			numPools;
static scPoolInfo*	pools;

/* ======================================================================== */

inline MEM_POOL GetHandlePool( void )
{
	return pools[ numPools - 1 ].fPool;
}


/* ======================================================================== */

inline MEM_POOL GetPool( size_t size )
{
	register i;
	
	for ( i = 0; i < numPools; i++ ) {
		if ( pools[i].fBlockSize && pools[i].fBlockSize == size )
			return pools[i].fPool;
		else
			return pools[i].fPool;
	}
	return 0;
}


/* ======================================================================== */

inline MEM_POOL PoolOfPtr( void* ptr )
{
	MEM_POOL_INFO	info;

	if ( MemPoolInfo( 0, ptr, &info ) )
		return info.pool;
	else
		return 0;
}

/* ======================================================================== */

inline int CountPools( scPoolInfo infoPools[] )
{
	register i;

	for ( i = 0; infoPools[i++].fBlockSize; )
		;

	return i;
}

/* ======================================================================== */

void MEMInit( scPoolInfo infoPools[] )
{
	register i;

	pools = infoPools;
	numPools = CountPools( pools );
	
	for ( i = 0;  i < numPools; i++ ) {
		if ( pools[i].fBlockSize ) {
			pools[i].fPool = MemPoolInitFS( pools[i].fBlockSize,
											1024,
											MEM_POOL_DEFAULT );
			raise_if( pools[i].fPool == 0, scERRmem );
		}
		else {
			pools[i].fPool = MemPoolInit( MEM_POOL_DEFAULT );
			raise_if( pools[i].fPool == 0, scERRmem );			
		}
	}
}

/* ======================================================================== */

#if SCDEBUG > 1

void dbgMemFormatPoolInfo( MEM_POOL_INFO* info )
{
	scChar buf[256];
	
	SCDebugTrace( 0, scString( "MEM_POOL_INFO\n" ) );

	scStrcpy( buf, scString( "" ) );
	if ( info->type & MEM_FS_BLOCK )
		scStrcat( buf, scString( "MEM_FS_BLOCK " ) );
	if ( info->type & MEM_VAR_MOVEABLE_BLOCK )
		scStrcat( buf, scString( "MEM_VAR_MOVEABLE_BLOCK " ) );
	if ( info->type & MEM_VAR_FIXED_BLOCK )
		scStrcat( buf, scString( "MEM_VAR_FIXED_BLOCK " ) );
	SCDebugTrace( 0, scString( "MEM_BLOCK_TYPE %s\n" ), buf );
	
	SCDebugTrace( 0, scString( "pagesize %d\n" ), info->pageSize );
	SCDebugTrace( 0, scString( "floor %lu\n" ), info->floor );
	SCDebugTrace( 0, scString( "ceiling %lu\n" ), info->ceiling );
	SCDebugTrace( 0, scString( "flags 0x%08x\n" ), info->flags );	
	
	
}
#endif

/* ======================================================================== */

void MEMFini()
{
	register 		i;

#if SCDEBUG > 1
	MEM_POOL_INFO	info;
	SCDebugTrace( 0, scString( "\n\nMemFini: BEGIN\n" ) );
#endif
	
	for ( i = 0; i < numPools; i++ ) {
		
		SCDebugTrace( 0, scString( "Free MemPool - start %d\n" ), i );
#if SCDEBUG > 1
		MemPoolCheck( pools[i].fPool );
		MemPoolInfo( pools[i].fPool, 0, &info );
		dbgMemFormatPoolInfo( &info );
#endif
		
		scAssert( MemPoolFree( pools[i].fPool ) ), pools[i].fPool = 0;

		SCDebugTrace( 0, scString( "Free MemPool - end %d\n\n\n" ), i );
	}
	
#if SCDEBUG > 1
	SCDebugTrace( 0, scString( "MemFini: DONE\n" ) );
#endif
}

/* ======================================================================== */
#else
/* ======================================================================== */

void MEMInit( scPoolInfo [] )
{
}

/* ======================================================================== */

void MEMFini()
{
}

#endif

/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */
#ifdef SCMACINTOSH

int 				gStartUpCompleted;

// NOTE: To understand this you should be aware of the Macintosh memory
// management as well as the handling of memory in the CApplication class.
// Read the TCL description of the CApplication class and how it handles
// the rainy day fund.
// The stack object CLoanApp tells the application that we can fail this
// memory request. We will assume that all other requests cannot fail.
// That means we must have sufficient memory to service the request.


	// this should really be a CStackObject - unfortunatley the chicken/egg
	// problem arises because the init of tcExceptContext calls these routines
	// and CStackObject relies upon tcExceptContext already existing.
	// The reason we would like it to be a stack object is that if we 
	// throw and exception this would reset the memory requests properly.
	// To reset the the memory request flags in the application I will
	// set them when we ignore the exception at the top of the event loop.
	
class CLoanApp {
public:
	CLoanApp();
	~CLoanApp();
private:
};


CLoanApp::CLoanApp()
{	
}


CLoanApp::~CLoanApp()
{
}

#else

int 				gStartUpCompleted = true;

#define CLoanApp
#define loanApp

#endif


/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */

#if SCDEBUG < 2

/* ======================================================================== */

void *MEMAllocPtr( ulong sz )
{
	CLoanApp	loanApp;	
	void		*ptr;

#if useSMARTHEAP	
	ptr = MemAllocPtr( GetPool( sz ), sz, 0 );
#else
	ptr = malloc( sz );
#endif
	
	raise_if( !ptr, scERRmem );
 	return ptr;
}

/* ======================================================================== */

scMemHandle MEMAllocHnd( ulong sz )
{
	CLoanApp	loanApp;		
	scMemHandle	hnd = 0;

#if useSMARTHEAP
	hnd = MemAlloc( GetHandlePool(), MEM_MOVEABLE | MEM_RESIZEABLE, sz );
#else	
	hnd = (scMemHandle)malloc( sizeof( MacHandle) + sz );

	MacHandle macHandle( hnd );

	*(MacHandle*)hnd = macHandle;
#endif
	
	raise_if( !hnd, scERRmem );
 	return hnd;
}

/* ======================================================================== */

//void *MEMAllocObj( ulong size )
//{
//	CLoanApp	loanApp;		
//	void		*ptr;
//
//	ptr = GetMemManager().AllocObj( (size_t)size );
//	raise_if( !ptr, scERRmem );
// 	return ptr;
//}

/* ======================================================================== */

void *MEMDupPtr( void *obj )
{
	CLoanApp	loanApp;		
	void		*ptr;
	ulong		sz = MEMGetSizePtr( obj );

	ptr = MEMAllocPtr( sz );
	raise_if( !ptr, scERRmem );
	memcpy( ptr, obj, sz );
 	return ptr;
}

/* ======================================================================== */

scMemHandle MEMDupHnd( scMemHandle obj )
{
	CLoanApp	loanApp;		
	scMemHandle hnd;

#if useSMARTHEAP
	ulong	sz = MemSize( obj );

	hnd = MEMAllocHnd( sz );

	try {
		void*	srcP = MemLock( obj );
		void*	dstP = MemLock( hnd );
		memcpy( dstP, srcP, sz );	
	}

	catch( ... ) {
		MemUnlock( hnd );
		MemUnlock( obj );
	} 

	MemUnlock( hnd );
	MemUnlock( obj );

#else
	
	ulong	sz = MEMGetSizePtr( obj );

	hnd = MEMAllocHnd( sz );

	try {
		void*	srcP = MEMLockHnd( obj );
		void*	dstP = MEMLockHnd( hnd );
		memcpy( dstP, srcP, sz );	
	}

	catch( ... ) {
		MEMUnlockHnd( hnd );
		MEMUnlockHnd( obj );
	} 

	MEMUnlockHnd( hnd );
	MEMUnlockHnd( obj );
	
#endif
	
 	return hnd;
}

/* ======================================================================== */

void *MEMDupObj( void *obj )
{
	CLoanApp	loanApp;		
	void		*ptr;
	ulong		sz = MEMGetSizePtr( obj );

	ptr = MEMAllocPtr( sz );
	raise_if( !ptr, scERRmem );
	memcpy( ptr, obj, sz );
 	return ptr;
}

/* ======================================================================== */

void* MEMResizePtr( void**	obj, ulong reqSize )
{
	CLoanApp	loanApp;		
	void		*ptr;

#if useSMARTHEAP
	if ( !*obj )
		ptr = MEMAllocPtr( reqSize );
	else
		ptr = MemReAllocPtr( *obj, reqSize, MEM_RESIZEABLE );
#else
	if ( !*obj )
		ptr = malloc( reqSize );
	else
		ptr = realloc( *obj, reqSize );
#endif
	raise_if( !ptr, scERRmem );
	return *obj = ptr;
}

/* ======================================================================== */

scMemHandle MEMResizeHnd( scMemHandle obj, ulong reqSize )
{
	CLoanApp	loanApp;

#if useSMARTHEAP
	if ( !obj )
		obj = MEMAllocHnd( reqSize );
	else
		obj = MemReAlloc( obj, reqSize, MEM_RESIZEABLE );
#else
	if ( !obj )
		obj = MEMAllocHnd( reqSize );
	else
		obj = (scMemHandle)realloc( obj, reqSize + sizeof( MacHandle ) );
		
	MacHandle macHandle( obj );

	*(MacHandle*)obj = macHandle;
#endif
	
	return obj;
}

/* ======================================================================== */

#endif /* !SCDEBUG */

/* ======================================================================== */

void MEMFreePtr( void *obj )
{
	if ( obj == 0 )
		return;
	
#if useSMARTHEAP
	MemFreePtr( obj );
#else
	free( obj );
#endif
}

/* ======================================================================== */

void MEMFreeHnd( scMemHandle obj )
{
	if ( obj == 0 )
		return;
	
#if useSMARTHEAP
	raise_if( MemLockCount( obj ), scERRmem );	
	MemFree( obj );
#else
	free( obj );	
#endif
}

/* ======================================================================== */

ulong MEMGetSizePtr( const void *obj )
{
	if ( obj == 0 )
		return 0;
			
#if useSMARTHEAP
	return MemSizePtr( (void*)obj );
#else
	return _msize( (void*)obj );
#endif
}

/* ======================================================================== */

ulong MEMGetSizeHnd( scMemHandle obj )
{
	if ( obj == 0 )
		return 0;
	
#if useSMARTHEAP
	return MemSize( obj );
#else
	return _msize( (void*)obj ) - sizeof( MacHandle );
#endif
}

/* ======================================================================== */

void *MEMLockHnd( scMemHandle hnd, int counted )
{
#if useSMARTHEAP
	return MemLock( hnd );
#else
	MacHandle* mh = (MacHandle*)hnd;
	return mh->Lock();
#endif	

}

/* ======================================================================== */

void MEMUnlockHnd( scMemHandle hnd, int counted )
{
#if useSMARTHEAP
	MemUnlock( hnd );
#else
	MacHandle* mh = (MacHandle*)hnd;
	mh->Unlock();
#endif
}

/* ======================================================================== */

#if SCDEBUG > 1

/* ======================================================================== */

void	MEMValidate( void *ptr )
{
#if useSMARTHEAP
	MEM_POOL pool = PoolOfPtr( ptr );

	if ( pool ) {
		scAssert( MemPoolCheck( pool ) );
	}
#else
#endif
}

/* ======================================================================== */

void memDumpMetrics()
{
#if useSMARTHEAP
#elif useMACHACK
#endif
}

/* ======================================================================== */

inline void memRecordTrackInfo( void *ptr, const char *filename, int line )
{
#ifdef MEM_TRACK_ALLOC
	#if useSMARTHEAP
	#else
	#endif
#endif
}

/* ======================================================================== */

inline void memRecordTrackInfo( scMemHandle ptr, const char *filename, int line )
{
#ifdef MEM_TRACK_ALLOC
	#if useSMARTHEAP
	#else
	#endif
#endif
}

/* ======================================================================== */

int gRandomFailure;					// randomly fail memory allocations

static Boolean RandomFailure()
{
	if ( !gRandomFailure || !gStartUpCompleted )
		return false;
	
	if ( ( rand() % gRandomFailure ) )
		return false;
	else {
		SCDebugTrace( 0, scString( "RANDOM FAILURE %d\n" ), gRandomFailure );
		return true;
	}
}

/* ======================================================================== */

void* MEMAllocPtrDebug( ulong sz, const char *filename, int line )
{
	CLoanApp	loanApp;		
	void		*ptr;
	
	raise_if( RandomFailure(), scERRmem );

#if useSMARTHEAP
	ptr = _dbgMemAllocPtr( GetPool( sz ), sz, 0, filename, line );
#else
	ptr = malloc( sz );
#endif
	
	raise_if( !ptr, scERRmem );
	memRecordTrackInfo(ptr, filename, line);

	return ptr;
}

/* ======================================================================== */

scMemHandle MEMAllocHndDebug( ulong sz, const char *filename, int line )
{
	CLoanApp	loanApp;		
	scMemHandle	hnd;
 
	raise_if( RandomFailure(), scERRmem );

#if useSMARTHEAP
	hnd = _dbgMemAlloc( GetHandlePool(), MEM_MOVEABLE | MEM_RESIZEABLE, sz, filename, line );
#else
	
	hnd = (scMemHandle)malloc( sizeof( MacHandle) + sz );

	MacHandle macHandle( hnd );

	*(MacHandle*)hnd = macHandle;

#endif

	raise_if( !hnd, scERRmem );
	memRecordTrackInfo( hnd, filename, line);

	return hnd;
}

/* ======================================================================== */

void* MEMResizePtrDebug( void**			obj,
						 ulong			reqSize,
						 const char*	file,
						 int			line )
{
	CLoanApp	loanApp;		
	void		*ptr;

#if useSMARTHEAP
	if ( !*obj )
		ptr = MEMAllocPtrDebug( reqSize, file, line );
	else
		ptr = _dbgMemReAllocPtr( *obj, reqSize, MEM_RESIZEABLE, file, line );
#else
	ptr = realloc( *obj, reqSize );
#endif

	raise_if( !ptr, scERRmem );
	return *obj = ptr;
}

/* ======================================================================== */

scMemHandle MEMResizeHndDebug( scMemHandle	obj,
							   ulong		reqSize,
							   const char*	file,
							   int	   		line )
{
	CLoanApp	loanApp;

#if useSMARTHEAP
	if ( !obj )
		obj = MEMAllocHndDebug( reqSize, file, line );
	else
		obj = _dbgMemReAlloc( obj, reqSize, MEM_RESIZEABLE, file, line );
#else
	obj = (scMemHandle)realloc( obj, reqSize + sizeof( MacHandle ) );
#endif
	
	return obj;
}

/* ======================================================================== */

void *MEMDupPtrDebug( void *obj, const char *filename, int line )
{
	CLoanApp	loanApp;		
	void		*ptr;

	if ( !RandomFailure() ) {
#if useSMARTHEAP
		ulong		sz = MemSizePtr( obj );

		ptr = MEMAllocPtrDebug( sz, filename, line );
		raise_if( !ptr, scERRmem );
		memcpy( ptr, obj, sz );	
#else
		ulong		sz = _msize( obj );

		ptr = MEMAllocPtrDebug( sz, filename, line );
		raise_if( !ptr, scERRmem );
		memcpy( ptr, obj, sz );	
#endif
	}
	else
		ptr = NULL;
	raise_if( !ptr, scERRmem );
	memRecordTrackInfo(ptr, filename, line);
	return ptr;
}

/* ======================================================================== */

scMemHandle MEMDupHndDebug( scMemHandle obj, const char *filename, int line )
{
	CLoanApp	loanApp;		
	scMemHandle	hnd;

	if ( !RandomFailure() ) {
		
#if useSMARTHEAP
		
		ulong	sz = MemSize( obj );

		hnd = MEMAllocHndDebug( sz, filename, line );

		try {
			void*	srcP = MemLock( obj );
			void*	dstP = MemLock( hnd );
			memcpy( dstP, srcP, sz );	
		}

		catch ( ... ) {
			MemUnlock( hnd );
			MemUnlock( obj );
			throw;
		} 

		MemUnlock( hnd );
		MemUnlock( obj );
		
#else
		
		ulong	sz = _msize( obj ) - sizeof( MacHandle );

		hnd = MEMAllocHndDebug( sz, filename, line );

		try {
			void*	srcP = MEMLockHnd( obj );
			void*	dstP = MEMLockHnd( hnd );
			memcpy( dstP, srcP, sz );	
		}

		catch ( ... ) {
			MEMUnlockHnd( hnd );
			MEMUnlockHnd( obj );
		} 

		MEMUnlockHnd( hnd );
		MEMUnlockHnd( obj );
		
#endif
	}
	else
		hnd = NULL;
	raise_if( !hnd, scERRmem );
	memRecordTrackInfo( hnd, filename, line );
	return hnd;
}

/* ======================================================================== */

#endif /* SCDEBUG */

/* ======================================================================== */

scAutoUnlock::scAutoUnlock( scMemHandle hnd )
	: fHandle(hnd)
{
#if useSMARTHEAP	
	MemLock( fHandle );
#else
	MEMLockHnd( fHandle );
#endif
}

scAutoUnlock::~scAutoUnlock()
{
#if useSMARTHEAP
	MemUnlock( fHandle );
#else
	MEMUnlockHnd( fHandle );	
#endif
}
