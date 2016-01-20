/****************************************************************************

	File:		MEM.C

	$Header: /Projects/Toolbox/ct/SCNSHMEM.CPP 2	 5/30/97 8:45a Wmanis $

	Contains:	Memory management routines based on our own heap managers

	Written by: Sealy

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	
*****************************************************************************/

#include "scmem.h"

#include <string.h>
#include <malloc.h>

class  MacHandle {
public:
			MacHandle( scMemHandle	ptr ) :
						block_( (char*)ptr + sizeof( MacHandle ) ),
						magic_( 0xfafafafa ),
						count_( 0 ),
						size_( _msize( ptr ) - sizeof( MacHandle ) )
						{
						}
	void*	Lock( void ) 
				{ 
					Validate(); 
					scAssert( count_ >= 0 ); 
					count_++; 
					return (void*)block_; 
				}
	void	Unlock( void )		
				{ 
					Validate(); 
					scAssert( count_ > 0 ); 
					--count_; 
				}
	void	Validate()
				{ 
					unsigned int size = _msize( this );
					scAssert( size_ + sizeof( MacHandle ) == size ); 
					scAssert( block_ == (char*)this + sizeof( MacHandle ) );
				}
	inline int Count() const 
				{
					return count_;
				}
private:
	const void* block_;
	ulong		magic_;
	int 		count_;
	int 		size_;
};


#ifdef MEM_DEBUG
	#include <stdlib.h>  // for rand

	long gMemUsage;
	#define dbgTrackAmount( n ) 	gMemUsage += (n)
#else
	#define dbgTrackAmount( n ) 
#endif

/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */

void MEMInit( scPoolInfo [] )
{
}

void MEMFini()
{
}

/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */

#ifndef MEM_DEBUG

/* ======================================================================== */

void *MEMAllocPtr( ulong sz )
{
	void		*ptr;

	ptr = malloc( sz );
	raise_if( !ptr, scERRmem );
	
	return ptr;
}

/* ======================================================================== */

scMemHandle MEMAllocHnd( ulong sz )
{
	scMemHandle hnd = 0;

	hnd = (scMemHandle)malloc( sizeof( MacHandle) + sz );
	raise_if( !hnd, scERRmem );

	MacHandle macHandle( hnd );

	*(MacHandle*)hnd = macHandle;
	
	return hnd;
}

/* ======================================================================== */

void *MEMDupPtr( void *obj )
{
	void		*ptr;
	ulong		sz = MEMGetSizePtr( obj );

	ptr = MEMAllocPtr( sz );
	raise_if( !ptr, scERRmem );

	SCmemcpy( ptr, obj, sz );
	return ptr;
}

/* ======================================================================== */

scMemHandle MEMDupHnd( scMemHandle obj )
{
	scMemHandle hnd;


	ulong	sz = MEMGetSizePtr( obj );

	hnd = MEMAllocHnd( sz );

	try {
		void*	srcP = MEMLockHnd( obj );
		void*	dstP = MEMLockHnd( hnd );
		SCmemcpy( dstP, srcP, sz ); 
	}
	catch( status e ) {
		MEMUnlockHnd( hnd );
		MEMUnlockHnd( obj );
		throw( e );
	}
	catch ( ... ) {
		MEMUnlockHnd( hnd );
		MEMUnlockHnd( obj );
		throw;
	}

	MEMUnlockHnd( hnd );
	MEMUnlockHnd( obj );
	
	return hnd;
}

/* ======================================================================== */

void *MEMDupObj( void *obj )
{
	void		*ptr;
	ulong		sz = MEMGetSizePtr( obj );

	ptr = MEMAllocPtr( sz );
	raise_if( !ptr, scERRmem );

	SCmemcpy( ptr, obj, sz );
	return ptr;
}

/* ======================================================================== */

void* MEMResizePtr( void**	obj, ulong reqSize )
{
	void		*ptr;

	if ( !*obj )
		ptr = malloc( reqSize );
	else
		ptr = realloc( *obj, reqSize );

	raise_if( !ptr, scERRmem );

	return *obj = ptr;
}

/* ======================================================================== */

scMemHandle MEMResizeHnd( scMemHandle obj, ulong reqSize )
{
	if ( !obj )
		obj = MEMAllocHnd( reqSize );
	else {
		scAssert( ((MacHandle*)obj)->Count() == 0 ); // don't resize a locked handle
		obj = (scMemHandle)realloc( obj, reqSize + sizeof( MacHandle ) );
	}
		
	MacHandle macHandle( obj );

	*(MacHandle*)obj = macHandle;
	
	return obj;
}

/* ======================================================================== */

#endif /* !SCDEBUG */

/* ======================================================================== */

void MEMFreePtr( void* obj )
{
	
	if ( obj == 0 )
		return;

	dbgTrackAmount( -(int)_msize( obj ) );
	
	free( obj );
}

/* ======================================================================== */

void MEMFreeHnd( scMemHandle hnd )
{

	if ( hnd == 0 )
		return;

	MacHandle* mh = (MacHandle*)hnd;
	scAssert( !mh->Count() );

	dbgTrackAmount( -(int)_msize( hnd ) );
	
	free( hnd );	
}

/* ======================================================================== */

ulong MEMGetSizePtr( const void *obj )
{
	if ( obj == 0 )
		return 0;
			
	return _msize( (void*)obj );
}

/* ======================================================================== */

ulong MEMGetSizeHnd( scMemHandle obj )
{
	if ( obj == 0 )
		return 0;
	
	return _msize( (void*)obj ) - sizeof( MacHandle );
}

/* ======================================================================== */

void *MEMLockHnd( scMemHandle hnd, int counted )
{
	MacHandle* mh = (MacHandle*)hnd;
	return mh->Lock();
}

/* ======================================================================== */

void MEMUnlockHnd( scMemHandle hnd, int counted )
{
	MacHandle* mh = (MacHandle*)hnd;
	mh->Unlock();
}

/* ======================================================================== */

#ifdef MEM_DEBUG

/* ======================================================================== */

void	MEMValidate( void *ptr )
{
}

/* ======================================================================== */

void memDumpMetrics()
{
}

/* ======================================================================== */

inline void memRecordTrackInfo( void *ptr, const char *filename, int line )
{
}

/* ======================================================================== */

inline void memRecordTrackInfo( scMemHandle ptr, const char *filename, int line )
{
}

/* ======================================================================== */

int gRandomFailure; 				// randomly fail memory allocations

static Boolean RandomFailure()
{
	if ( !gRandomFailure )
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
	void*	ptr;
	
	raise_if( RandomFailure(), scERRmem );

	scAssert( sz > 0 );
	ptr = malloc( sz );
	raise_if( !ptr, scERRmem );
	
	memRecordTrackInfo(ptr, filename, line);

	dbgTrackAmount( sz );
	
	return ptr;
}

/* ======================================================================== */

scMemHandle MEMAllocHndDebug( ulong sz, const char *filename, int line )
{
	scMemHandle hnd;
 
	raise_if( RandomFailure(), scERRmem );

	hnd = (scMemHandle)malloc( sizeof(MacHandle) + sz );
	raise_if( !hnd, scERRmem );

	MacHandle macHandle( hnd );

	*(MacHandle*)hnd = macHandle;
	((MacHandle*)hnd)->Validate();

	memRecordTrackInfo( hnd, filename, line);

	dbgTrackAmount( sz + sizeof(MacHandle) );
	
	return hnd;
}

/* ======================================================================== */

void* MEMResizePtrDebug( void** 		obj,
						 ulong			reqSize,
						 const char*	file,
						 int			line )
{
	void		*ptr;

	dbgTrackAmount( reqSize - (int)_msize( *obj ) );
	
	ptr = realloc( *obj, reqSize );
	raise_if( !ptr, scERRmem );

	return *obj = ptr;
}

/* ======================================================================== */

scMemHandle MEMResizeHndDebug( scMemHandle	obj,
							   ulong		reqSize,
							   const char*	file,
							   int			line )
{  
	int size1 = 0;

	if ( obj ) 
		size1 = _msize( obj );

	if ( !obj )
		obj = MEMAllocHndDebug( reqSize, file, line );
	else {
		scAssert( ((MacHandle*)obj)->Count() == 0 ); // don't resize a locked handle
		obj = (scMemHandle)realloc( obj, reqSize + sizeof( MacHandle ) );
	}

	MacHandle macHandle( obj );

	*(MacHandle*)obj = macHandle;

	int size2 = _msize( obj ) - sizeof( MacHandle );

	dbgTrackAmount( reqSize - size1 );	
	return obj;
}

/* ======================================================================== */

void *MEMDupPtrDebug( void *obj, const char *filename, int line )
{
	void		*ptr;

	if ( !RandomFailure() ) {
		ulong		sz = _msize( obj );

		ptr = MEMAllocPtrDebug( sz, filename, line );
		raise_if( !ptr, scERRmem );

		SCmemcpy( ptr, obj, sz );	
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
	scMemHandle hnd;

	if ( !RandomFailure() ) {
		ulong	sz = _msize( obj ) - sizeof( MacHandle );

		hnd = MEMAllocHndDebug( sz, filename, line );

		try {
			void*	srcP = MEMLockHnd( obj );
			void*	dstP = MEMLockHnd( hnd );
			SCmemcpy( dstP, srcP, sz ); 
		}
		catch (...) {
			MEMUnlockHnd( hnd );
			MEMUnlockHnd( obj );
			throw;
		}

		MEMUnlockHnd( hnd );
		MEMUnlockHnd( obj );
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
	MEMLockHnd( fHandle );
}

scAutoUnlock::~scAutoUnlock()
{
	MEMUnlockHnd( fHandle );	
}

/* ==================================================================== */

#ifndef SCmemset		// we are in a 16 bit world

void scFar*  scFar scCDecl SCmemset( void scFar*	ptr,
									 int			val,
									 long			len )
{
	return _fmemset( ptr, val, (size_t)len );
}

/* ==================================================================== */

void scFar*  scFar scCDecl SCmemmove( void scFar*		dst,
									  const void scFar* src,
									  long				len )
{
	return _fmemmove( dst, src, (size_t)len );
}

/* ==================================================================== */

void scFar*  scFar scCDecl SCmemcpy( void scFar*		dst,
									 const void scFar*	src,
									 long				len )
{
	return _fmemcpy( dst, src, (size_t)len );
}

/* ==================================================================== */

int scFar scCDecl SCmemcmp( const void scFar*	p1,
							const void scFar*	p2,
							long				len )
{
	return _fmemcmp( p1, p2, (size_t)len );
}

#endif

/* ==================================================================== */
