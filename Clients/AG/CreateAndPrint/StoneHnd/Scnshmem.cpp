/****************************************************************************
Contains:	Memory management routines based on our own heap managers
*****************************************************************************/

#include "scmem.h"
#include <malloc.h>

class  MemHandle
{
public:
			MemHandle( scMemHandle	ptr ) :
						block_( (char*)ptr + sizeof( MemHandle ) ),
						magic_( 0xfafafafa ),
						count_( 0 ),
						size_( _msize( ptr ) - sizeof( MemHandle ) )
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
					scAssert( size_ + sizeof( MemHandle ) == size ); 
					scAssert( block_ == (char*)this + sizeof( MemHandle ) );
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

/* ======================================================================== */
void MEMInit( scPoolInfo [] )
{
}

void MEMFini()
{
}

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

	hnd = (scMemHandle)malloc( sizeof( MemHandle) + sz );
	raise_if( !hnd, scERRmem );

	MemHandle memHandle( hnd );

	*(MemHandle*)hnd = memHandle;
	
	return hnd;
}

/* ======================================================================== */
void *MEMDupPtr( void *obj )
{
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
	scMemHandle hnd;


	ulong	sz = MEMGetSizePtr( obj );

	hnd = MEMAllocHnd( sz );

	try {
		void*	srcP = MEMLockHnd( obj );
		void*	dstP = MEMLockHnd( hnd );
		memcpy( dstP, srcP, sz ); 
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

	memcpy( ptr, obj, sz );
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
		scAssert( ((MemHandle*)obj)->Count() == 0 ); // don't resize a locked handle
		obj = (scMemHandle)realloc( obj, reqSize + sizeof( MemHandle ) );
	}
		
	MemHandle memHandle( obj );

	*(MemHandle*)obj = memHandle;
	
	return obj;
}

/* ======================================================================== */
void MEMFreePtr( void* obj )
{
	
	if ( obj == 0 )
		return;

	free( obj );
}

/* ======================================================================== */
void MEMFreeHnd( scMemHandle hnd )
{

	if ( hnd == 0 )
		return;

	MemHandle* mh = (MemHandle*)hnd;
	scAssert( !mh->Count() );

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
	
	return _msize( (void*)obj ) - sizeof( MemHandle );
}

/* ======================================================================== */
void *MEMLockHnd( scMemHandle hnd, int counted )
{
	MemHandle* mh = (MemHandle*)hnd;
	return mh->Lock();
}

/* ======================================================================== */
void MEMUnlockHnd( scMemHandle hnd, int counted )
{
	MemHandle* mh = (MemHandle*)hnd;
	mh->Unlock();
}

/* ======================================================================== */
scAutoUnlock::scAutoUnlock( scMemHandle hnd )
	: fHandle(hnd)
{
	MEMLockHnd( fHandle );
}

/* ======================================================================== */
scAutoUnlock::~scAutoUnlock()
{
	MEMUnlockHnd( fHandle );	
}
