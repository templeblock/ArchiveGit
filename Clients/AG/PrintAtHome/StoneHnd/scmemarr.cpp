/************************************************************************
Contains:	Variable sized array code.
**************************************************************************/

#include "scmemarr.h"
#include "scmem.h"
#include <stdlib.h>

/* ==================================================================== */
void scAbstractArray::ClearMem( long oldsize )
{
		// either we do need to clear memory or we have shrunk it
	if ( !fClearMem || oldsize >= fElemSlots )
		return;

	ElementPtr	elements = Lock();

	memset( elements + ( oldsize * fElemSize ),
			0,
			( fElemSlots - oldsize ) * fElemSize );
	
	Unlock();
}

/* ==================================================================== */
void scAbstractArray::Paste( long index, const scAbstractArray& arr )
{
	ElementPtr ptr = arr.Lock();

	try {
		InsertAt( index, ptr, arr.GetNumItems() );
	}

	catch ( ... ) {
		arr.Unlock();
		throw;
	}

	arr.Unlock();			
}

/* ==================================================================== */	
void scAbstractArray::Copy( scAbstractArray& arr, long start, long end ) const
{
	ElementPtr ptr = Lock();

	try {
		arr.InsertAt( 0, ptr, end - start );
	}
	
	catch ( ... ) {
		Unlock();
		throw;
	} 

	Unlock();
}

/* ==================================================================== */	
void scAbstractArray::Cut( scAbstractArray& arr, long start, long end )
{
	Copy( arr, start, end );
	RemoveDataAt( start, end - start );
}

/* ==================================================================== */	
void scAbstractArray::AppendData( const ElementPtr	elemptr,
								  long				elements )
{
	SetNumSlots( fNumItems + elements );

	ElementPtr	ptr = Lock();
	memmove( ptr + ( (long)fNumItems * fElemSize ),
			 elemptr,
			 (fElemSize * elements) );
	Unlock();
	fNumItems += elements;
}

/* ==================================================================== */
void scAbstractArray::RemoveDataAt( long index,
									long elements )
{
	scAssert( index + elements <= fNumItems );
	
	ElementPtr	ptr = Lock();
	
	memmove( ptr + ( index * fElemSize ),
			 ptr + ( ( index + elements ) * fElemSize ),
			 ( fNumItems - index - elements ) * fElemSize  );

	Unlock();

	fNumItems -= elements;

	ShrinkSlots();
}

/* ==================================================================== */
void scAbstractArray::AlterDataAt( long 		index,
								   ElementPtr	elemptr,
								   long 		elements )
{
	ElementPtr	ptr = Lock();
	memmove( ptr + ( (long)index * fElemSize ),
			 elemptr,
			 fElemSize * elements );
	Unlock();
}

/* ==================================================================== */
void scAbstractArray::InsertAt( long		index,
								ElementPtr	elemptr,
								long		elements )
{
	SetNumSlots( fNumItems + elements );

	ElementPtr	ptr = Lock();

	memmove( ptr + ( ( index + elements ) * fElemSize ),
			 ptr + ( index * fElemSize ),
			 ( fNumItems - index ) * fElemSize	);
	
	memmove(	ptr + ( index * fElemSize ),
			  elemptr,
			  elements * fElemSize );

	Unlock();

	fNumItems += elements;
}

/* ==================================================================== */
ElementPtr scAbstractArray::GetDataAt( long 		index,
									   ElementPtr	elemptr,
									   long 		elements ) const
{
	scAssert( index + elements <= fNumItems );
	
	ElementPtr	ptr = Lock();

	memmove( elemptr,
			 ptr + (index * fElemSize),
			 elements * fElemSize );

	Unlock();

	return elemptr;
}

/* ==================================================================== */
ElementPtr scAbstractArray::Pop( ElementPtr elemptr,
								 long		elements )
{
	long index = fNumItems - 1;
	
	elemptr = GetDataAt( index, elemptr, elements );
	RemoveDataAt( index, elements );
	return elemptr;
}

/* ==================================================================== */
ElementPtr scAbstractArray::GetTop( ElementPtr	elemptr,
									long		elements )
{
	long index = fNumItems - 1;
	
	elemptr = GetDataAt( index, elemptr, elements );
	return elemptr;
}

/* ==================================================================== */	
void scAbstractArray::DoForEach( CVoidFunc0 func )
{
	ElementPtr	elemPtr = Lock();
	int 	i;

	for ( i = 0; i < fNumItems; i++, elemPtr += fElemSize )
		(*func)( elemPtr );

	Unlock();
}

/* ==================================================================== */
void scAbstractArray::DoForEach( CVoidFunc1 func, long param )
{
	int 		i;
	ElementPtr	elemPtr = Lock();

	for ( i = 0; i < fNumItems; i++, elemPtr += fElemSize )
		(*func)( elemPtr, param );

	Unlock();
}

/* ==================================================================== */
void scAbstractArray::DoForEach( CVoidFunc2 func, long param1, long param2	)
{
	int 		i;
	ElementPtr	elemPtr = Lock();

	for ( i = 0; i < fNumItems; i++, elemPtr += fElemSize )
		(*func)( elemPtr, param1, param2 );

	Unlock();
}

/* ==================================================================== */
long scAbstractArray::FirstSuccess( CBoolFunc0 func )
{
	int 		i;
	ElementPtr	elemPtr = Lock();

	for ( i = 0; i < fNumItems; i++, elemPtr += fElemSize )
		if ( (*func)( elemPtr ) )
			break;

	Unlock();

	return i < fNumItems ? i : -1;

}

/* ==================================================================== */
long scAbstractArray::FirstSuccess( CBoolFunc1	func,
									long		param )
{
	int 		i;
	ElementPtr	elemPtr = Lock();


	for ( i = 0; i < fNumItems; i++, elemPtr += fElemSize )
		if ( (*func)( elemPtr, param ) )
			break;

	Unlock();

	return i < fNumItems ? i : -1;
}

/* ==================================================================== */
long scAbstractArray::FirstSuccess( CBoolFunc2	func,
									long		param1,
									long		param2 )
{
	int 		i;
	ElementPtr	elemPtr = Lock();


	for ( i = 0; i < fNumItems; i++, elemPtr += fElemSize )
		if( (*func)( elemPtr, param1, param2 ) )
			break;

	Unlock();

	return i < fNumItems ? i : -1;
}

/* ==================================================================== */
long scAbstractArray::FirstSuccess( CBoolConstFunc0 func ) const
{
	int 		i;
	ElementPtr	elemPtr = Lock();


	for ( i = 0; i < fNumItems; i++, elemPtr += fElemSize )
		if ( (*func)( elemPtr ) )
			break;

	Unlock();

	return i < fNumItems ? i : -1;

}

/* ==================================================================== */
long scAbstractArray::FirstSuccess( CBoolConstFunc1 func,
									long			param ) const
{
	int 		i;
	ElementPtr	elemPtr = Lock();

	for ( i = 0; i < fNumItems; i++, elemPtr += fElemSize )
		if ( (*func)( elemPtr, param ) )
			break;

	Unlock();

	return i < fNumItems ? i : -1;
}

/* ==================================================================== */
long scAbstractArray::FirstSuccess( CBoolConstFunc2 func,
									long			param1,
									long			param2	) const
{
	int 		i;
	ElementPtr	elemPtr = Lock();

	for ( i = 0; i < fNumItems; i++, elemPtr += fElemSize )
		if( (*func)( elemPtr, param1, param2 ) )
			break;

	Unlock();

	return i < fNumItems ? i : -1;
}

/* ==================================================================== */
long scAbstractArray::NthSuccess( CBoolFunc0	func,
								  long			nth )
{
	long		i;
	ElementPtr	elemPtr = Lock();

	for ( i = 0; i < fNumItems; i++, elemPtr += fElemSize ) {
		if ( (*func)( elemPtr ) ) {
			if ( --nth == 0 )
				break;
		}
	}
	
	Unlock();

	return i < fNumItems ? i : -1;
}

/* ==================================================================== */
long scAbstractArray::NthSuccess( CBoolFunc1	func,
								  long			nth,
								  long			param )
{
	long		i;
	ElementPtr	elemPtr = Lock();

	for ( i = 0; i < fNumItems; i++, elemPtr += fElemSize ) {
		if ( (*func)( elemPtr, param ) ) {
			if ( --nth == 0 )
				break;
		}
	}

	Unlock();

	return i < fNumItems ? i : -1;
}

/* ==================================================================== */
long scAbstractArray::NthSuccess( CBoolFunc2	func,
								  long			nth,
								  long			param1,
								  long			param2	)
{
	long		i;
	ElementPtr	elemPtr = Lock();

	for ( i = 0; i < fNumItems; i++, elemPtr += fElemSize ) {
		if( (*func)( elemPtr, param1, param2 ) ) {
			if ( --nth == 0 )
				break;
		}
	}
	
	Unlock();

	return i < fNumItems ? i : -1;
}

/* ==================================================================== */
long scAbstractArray::NthSuccess( CBoolConstFunc0	func,
								  long				nth ) const
{
	int 		i;
	ElementPtr	elemPtr = Lock();

	for ( i = 0; i < fNumItems; i++, elemPtr += fElemSize ) {
		if ( (*func)( elemPtr ) ) {
			if ( --nth == 0 )
				break;
		}
	}
	
	Unlock();

	return i < fNumItems ? i : -1;
}

/* ==================================================================== */
long scAbstractArray::NthSuccess( CBoolConstFunc1	func,
								  long				nth,
								  long				param ) const
{
	ElementPtr	elemPtr = Lock();
	int 	i;

	for ( i = 0; i < fNumItems; i++, elemPtr += fElemSize ) {
		if ( (*func)( elemPtr, param ) ) {
			if ( --nth == 0 )
				break;
		}
	}

	Unlock();

	return i < fNumItems ? i : -1;
}

/* ==================================================================== */
long scAbstractArray::NthSuccess( CBoolConstFunc2	func,
								  long				nth,
								  long				param1,
								  long				param2	) const
{
	int 		i;
	ElementPtr	elemPtr = Lock();

	for ( i = 0; i < fNumItems; i++, elemPtr += fElemSize ) {
		if( (*func)( elemPtr, param1, param2 ) ) {
			if ( --nth == 0 )
				break;
		}
	}
	
	Unlock();

	return i < fNumItems ? i : -1;
}

/* ==================================================================== */
void scAbstractArray::QuickSort( CPtrArrayCmpFunc compfunc )
{
	raise_if( fItems == NULL, scERRmem );
	
	ElementPtr	elemPtr = Lock();

	::qsort( elemPtr, fNumItems, fElemSize, compfunc );
	
	Unlock();
}

/*======================================================================*/

scAbstractArray&	scAbstractArray::operator=( const scAbstractArray& absarray )
{	
	fNumItems	= absarray.fNumItems;	
	fElemSlots	= absarray.fElemSlots;
	fElemSize	= absarray.fElemSize;
	fBlockSize	= absarray.fBlockSize;
	
	return *this;
}

/* ==================================================================== */
// this is a very dumb and dangerous method if you are using the abstract
// array class to hold structures, structure alignment may cause there to
// be "dead" space in the structure which may be filled with 
// garbage --- rendering this method useless, so overide if needed
int scAbstractArray::IsEqual( const scObject& obj ) const
{
		// by the time we hit here we assume we have the same number of
		// items
	const scAbstractArray&	absarray	= ((const scAbstractArray&)obj);
	ElementPtr				elemPtr 	= Lock();
	ElementPtr				elemPtr2	= absarray.Lock();
	int 					isEqual;
	
	isEqual = ( fElemSize == absarray.fElemSize );
	if ( isEqual )
		isEqual = !memcmp( elemPtr, elemPtr2, fNumItems * fElemSize );
	
	Unlock();
	absarray.Unlock();
	
	return isEqual;
}

/* ==================================================================== */
scMemArray::scMemArray( size_t		elemSize,
						unsigned	clearmem ) :
							scAbstractArray( elemSize, clearmem )
{
	fItems		= MEMAllocPtr( fElemSize * fBlockSize );
	fElemSlots	= fBlockSize;
	ClearMem( 0 );
}


/* ==================================================================== */
scMemArray::scMemArray( const scMemArray& ma ) :
	scAbstractArray( ma )
{
	fItems		= MEMAllocPtr( fElemSize * ma.fElemSlots );
	fElemSlots	= ma.fElemSlots;
	memcpy( fItems, ma.fItems, fElemSize * ma.fElemSlots );
}

/* ==================================================================== */
scMemArray::~scMemArray()
{
	if ( fItems )
		MEMFreePtr( fItems ), fItems = 0;
}

/* ==================================================================== */
int scMemArray::IsEqual( const scObject& obj ) const
{
	const scHandleArray&	harray	= (const scHandleArray&)obj;
	if ( fNumItems != harray.GetNumItems() )
		return 0;
	return scAbstractArray::IsEqual( obj );
}

/* ==================================================================== */
void scMemArray::GrowSlots( long newItems )
{
	long oldSize = fElemSlots;
	raise_if( fItems == NULL, scERRmem );
	MEMResizePtr( &fItems, fElemSize * ( fElemSlots + newItems ) );
	fElemSlots += newItems;
	ClearMem( oldSize );
}

/* ==================================================================== */
void scMemArray::SizeSlots( long numItems )
{
		// do not shrink if we are retaining memory or if no resizing is
		// necessary
		//
	if ( ( numItems < fElemSlots && fRetainMem ) || fElemSlots == numItems )
		return;

	long oldSize = fElemSlots;	
	raise_if( fItems == NULL, scERRmem );	
	MEMResizePtr( &fItems, fElemSize * numItems );
	fElemSlots = numItems;
	ClearMem( oldSize );	
}

/*======================================================================*/

ElementPtr scMemArray::Lock() const
{ 
	raise_if( fItems == NULL, scERRmem );
	return (ElementPtr)fItems; 
}

/* ==================================================================== */
scMemArray& scMemArray::operator=( const scMemArray& cpa )
{
	if ( fItems )
		MEMResizePtr( &fItems, cpa.fElemSize * cpa.fElemSlots );
	else
		fItems = MEMAllocPtr( cpa.fElemSize * cpa.fElemSlots );
		
	scAbstractArray::operator=( cpa );	
	memcpy( fItems, cpa.fItems, fElemSize * fNumItems );
	
	return *this;
}

/* ==================================================================== */
scHandleArray::scHandleArray( size_t	elemSize,
							  unsigned	clearmem ) :
								scAbstractArray( elemSize, clearmem )
{
	fItems		= (scMemHandle)MEMAllocHnd( fElemSize * fBlockSize );
	fElemSlots	= fBlockSize;
	ClearMem( 0 );
}

/* ==================================================================== */
scHandleArray::~scHandleArray()
{
	if ( fItems )
		MEMFreeHnd( (scMemHandle)fItems ), fItems = 0;
}

/* ==================================================================== */
int scHandleArray::IsEqual( const scObject& obj ) const
{
	const scHandleArray&	harray	= (const scHandleArray&)obj;
	if ( fNumItems != harray.fNumItems )
		return 0;
	return scAbstractArray::IsEqual( obj );
}

/* ==================================================================== */
void scHandleArray::GrowSlots( long newItems )
{
	long oldSize = fElemSlots;
	raise_if( fItems == NULL, scERRmem );
	fItems = MEMResizeHnd( (scMemHandle)fItems, fElemSize * ( fElemSlots + newItems ) );
	fElemSlots += newItems;
	ClearMem( oldSize );
}

/* ==================================================================== */
void scHandleArray::SizeSlots( long numItems )
{
		// do not shrink if we are retaining memory or if no resizing is
		// necessary
		//
	if ( ( numItems < fElemSlots && fRetainMem ) || fElemSlots == numItems )
		return;

	long oldSize = fElemSlots;
	raise_if( fItems == NULL, scERRmem );
	fItems		= MEMResizeHnd( (scMemHandle)fItems, fElemSize * numItems );
	fElemSlots	= numItems;
	ClearMem( oldSize );
}

/* ==================================================================== */
ElementPtr scHandleArray::Lock( void ) const
{
	raise_if( fItems == NULL, scERRmem );
	return (ElementPtr)MEMLockHnd( (scMemHandle)fItems );
}

/* ==================================================================== */
void scHandleArray::Unlock( void ) const
{
	raise_if( fItems == NULL, scERRmem );
	MEMUnlockHnd( (scMemHandle)fItems );
}

/* ==================================================================== */
scHandleArray&	scHandleArray::operator=( const scHandleArray& cpa )
{
	if ( fItems )
		MEMResizeHnd( &fItems, cpa.fElemSize * cpa.fElemSlots );
	else
		fItems = MEMAllocHnd( cpa.fElemSize * cpa.fElemSlots );
		
	scAbstractArray::operator=( cpa );
	
	scMemHandle 	h1_ = (scMemHandle)fItems;
	scMemHandle 	h2_ = (scMemHandle)cpa.fItems;

	scAutoUnlock	h1( h1_ );
	scAutoUnlock	h2( h2_ );
		
	memcpy( *h1, *h2, fElemSize * fNumItems );
	
	return *this;
}
