/****************************************************************************

	File:	   SCARRAY.CPP


	$Header: /btoolbox/lib/SCARRAY.CPP 3	 3/31/96 3:48p Will $

	Contains:		Templates for Vector

	Written by: 	Manis

	Copyright (c) 1989-95 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	
****************************************************************************/


#ifdef DEFINE_TEMPLATES

#if defined( __MWERKS__ )
#include <stdlib.h>
#else
#include <malloc.h>
#endif

#include <string.h>
#include <assert.h>

inline
void *operator new(size_t, void *p)
{
	return p;
}


#ifndef scAssert
	#define stAssert	assert
#else
	#define stAssert	scAssert
#endif

#define stMAX( a, b )		((a)>(b)?(a):(b))
#define stMIN( a, b )		((a)<(b)?(a):(b))


template <class T>
scStaticArray<T>::scStaticArray( int num, T* mem ) :
	scArray<T>( num, mem ? mem : new T[num] )
{
}

template <class T>
scStaticArray<T>::~scStaticArray()
{
	delete [] items_;
}

#define kInitBlockSize	4

template <class T>
scSizeableArray<T>::scSizeableArray() :
	elemSlots_( 4 ),
	blockSize_( kInitBlockSize ),
	scArray<T>( 0, (T*)malloc( kInitBlockSize * sizeof( T ) ) )
{
	ClearMem( 0 );
}

template <class T>
scSizeableArray<T>::~scSizeableArray()
{
	if ( items_ )
		free( items_ );
}

template <class T>
void scSizeableArray<T>::Remove( int index )
{
	stAssert( index < numItems_ );
	
	memmove( items_ +index, items_ +index + 1,
			 ( numItems_ - index - 1 ) * sizeof( T )  );

	numItems_ -= 1;

	ShrinkSlots();
}

template <class T>
void scSizeableArray<T>::RemoveAll()
{
	numItems_ = 0;
	ShrinkSlots();
}

template <class T>
void scSizeableArray<T>::SetNumSlots( int numSlots )
{
	int numBlocks = stMAX( 1, numSlots / blockSize_ + ( numSlots % blockSize_ ? 1 : 0 ) );
	SizeSlots( numBlocks * blockSize_ );
}
	
template <class T>
int scSizeableArray<T>::Append( const T& elem )
{
	SetNumSlots( numItems_ + 1 );
	items_[numItems_] = elem;
	return numItems_++;
}

template <class T>
T& scSizeableArray<T>::Grow()
{
	SetNumSlots( numItems_ + 1 );
	return items_[numItems_++];
}


template <class T>
void scSizeableArray<T>::Insert( int index, const T& elem )
{
	SetNumSlots( numItems_ + 1 );

	if ( numItems_ - index > 0 ) 
		memmove( items_ + index + 1, items_ + index,
				 ( numItems_ - index ) * sizeof(T) );

	items_[index] = elem;	
	numItems_++;
}

template <class T>
void scSizeableArray<T>::Set( int index, const T& elem )
{
	if ( index >= numItems_ ) {
		SetNumSlots( index + 1 );
		numItems_ = index + 1;
	}
	
	items_[index] = elem;	
}


template <class T>
void scSizeableArray<T>::SizeSlots( unsigned numItems )
{
		// do not shrink if we are retaining memory or if no resizing is
		// necessary
		//
	if ( elemSlots_ == numItems || ( numItems < elemSlots_ && retainMem_ ) )
		return;

	stAssert( numItems >= blockSize_ );

	long oldSize = elemSlots_;

	if ( items_ == 0 )
		throw( -1 ); 

	items_ = (T*)realloc( items_, sizeof(T) * numItems );
	elemSlots_ = numItems;
	ClearMem( oldSize );
}


template <class T>
void scSizeableArray<T>::GrowSlots( int newItems )
{
	int oldSize = elemSlots_;

	if ( items_ == 0 )
		throw( -1 );

	items_ = (T*)realloc( items_, sizeof(T) * ( elemSlots_ + newItems ) );
	elemSlots_ += newItems;
	ClearMem( oldSize );
}


template <class T>		
void scSizeableArray<T>::ClearMem( unsigned oldsize )
{
		// either we do need to clear memory or we have shrunk it
	if ( oldsize >= elemSlots_ )
		return;

	for ( unsigned index = oldsize; index < elemSlots_; index++ )
		memset( items_ + index, 0, sizeof(T) );
}

#if 0
template <class T>
void scSizeableArray<T>::exch( const scSizeableArray<T>& arr )
{
	scArray<T>::operator=( arr );
	
	scSizeableArray<T>& array = (scSizeableArray<T>&)arr;

	unsigned tmp = elemSlots_;
	elemSlots_ = array.elemSlots_;
	array.elemSlots_ = tmp;

	tmp = blockSize_;
	blockSize_ = array.blockSize_;
	array.blockSize_ = blockSize_;

	tmp = retainMem_;
	retainMem_ = array.retainMem_;
	array.retainMem_ = retainMem_;		
}
#endif
	
template<class T, class CT>
int scBinarySortedArray<T,CT>::Find( const T& val, int* insertIndexP ) const
{
	int 	low 	 = 0;
	int 	high	= numItems_ - 1;
	int insertIndex = 0;

	while ( low <= high ) {
		int index = (low + high) / 2;	
		int found = CT::Compare( val, items_[index] );
		if ( found == 0 )
			return index;
		else if ( found < 0 )
			high = index - 1, insertIndex = index;
		else
			low = index + 1, insertIndex = index + 1;
	}

	insertIndexP ? *insertIndexP = insertIndex : 0;
	return -1;
}

template<class T, class CT>
int scBinarySortedArray<T,CT>::Find1( const T& val, const CT& ct, int* insertIndexP ) const
{
	int 	low 	 = 0;
	int 	high	= numItems_ - 1;
	int insertIndex = 0;

	while ( low <= high ) {
		int index = (low + high) / 2;	
		int found = ct.Compare1( val, items_[index] );
		if ( found == 0 )
			return index;
		else if ( found < 0 )
			high = index - 1, insertIndex = index;
		else
			low = index + 1, insertIndex = index + 1;
	}

	insertIndexP ? *insertIndexP = insertIndex : 0;
	return -1;
}

template<class T, class CT>
int scBinarySortedArray<T,CT>::SortInsert( const T& item )
{
	int 	index;
	if ( Find( item, &index ) < 0 )
		Insert( index, item );
	return index;	
} 

////

template <class T>
scStaticArrayD<T>::scStaticArrayD( int num, T* mem ) :
	scArray<T>( num, mem ? mem : new T[num] )
{
}

template <class T>
scStaticArrayD<T>::~scStaticArrayD()
{
	for ( int i = 0; i < numItems_; i++) {
#ifdef __WATCOMC__
		items_[i].~T();
#elif defined(_MAC)
		T& tp = items_[i];
		tp.~T();
#else
		items_[i].T::~T();
#endif
	}
	delete [] items_;
}

#define kInitBlockSize	4

template <class T>
scSizeableArrayD<T>::scSizeableArrayD() :
	elemSlots_( 4 ),
	blockSize_( kInitBlockSize ),
	scArray<T>( 0, (T*)malloc( kInitBlockSize * sizeof( T ) ) )
{
	ClearMem( 0 );
}

template <class T>
scSizeableArrayD<T>::~scSizeableArrayD()
{
	for ( unsigned i = 0; i < elemSlots_; i++)
		deleteItem( i );

	if ( items_ )
		free( items_ );
}

template <class T>
void scSizeableArrayD<T>::constructItem( int index )
{
	(void)new( items_ + index ) T;
}

template <class T>
void scSizeableArrayD<T>::deleteItem( int index )
{
#ifdef __WATCOMC__
	items_[index].~T();
#elif defined(_MAC)
	T& tp = items_[index];
	tp.~T();
#else
	items_[index].T::~T();
#endif
}

template <class T>
void scSizeableArrayD<T>::Remove( int index )
{
	stAssert( index < numItems_ );
	deleteItem( index );

	if ( numItems_ - index - 1 )
		memmove( items_ +index, items_ +index + 1,
					( numItems_ - index - 1 ) * sizeof( T )  );

	numItems_ -= 1;

	ShrinkSlots();
}

template <class T>
void scSizeableArrayD<T>::RemoveAll()
{
	for ( int i = 0; i < numItems_; i++)
		deleteItem( i );

	numItems_ = 0;
	ShrinkSlots();
}

template <class T>
void scSizeableArrayD<T>::SetNumSlots( int numSlots )
{
	int numBlocks = stMAX( 1, numSlots / blockSize_ + ( numSlots % blockSize_ ? 1 : 0 ) );
	SizeSlots( numBlocks * blockSize_ );
}
	
template <class T>
int scSizeableArrayD<T>::Append( const T& elem )
{
	SetNumSlots( numItems_ + 1 );
	items_[numItems_] = elem;
	return numItems_++;
}


template <class T>
T& scSizeableArrayD<T>::Grow()
{
	SetNumSlots( numItems_ + 1 );
	return items_[numItems_++];
}


template <class T>
void scSizeableArrayD<T>::Insert( int index, const T& elem )
{
	SetNumSlots( numItems_ + 1 );

	if ( numItems_ - index > 0 ) 
		memmove( items_ + index + 1, items_ + index,
				 ( numItems_ - index ) * sizeof(T) );

	constructItem( index );
	items_[index] = elem;	
	numItems_++;
}

template <class T>
void scSizeableArrayD<T>::Set( int index, const T& elem )
{
	if ( index >= numItems_ ) {
		SetNumSlots( index + 1 );
		numItems_ = index + 1;
	}
	
	items_[index] = elem;	
}


template <class T>
void scSizeableArrayD<T>::SizeSlots( unsigned numItems )
{
		// do not shrink if we are retaining memory or if no resizing is
		// necessary
		//
	if ( elemSlots_ == numItems || ( numItems < elemSlots_ && retainMem_ ) )
		return;

	stAssert( numItems >= blockSize_ );

	long oldSize = elemSlots_;

	if ( items_ == 0 )
		throw( -1 ); 

	items_ = (T*)realloc( items_, sizeof(T) * numItems );
	elemSlots_ = numItems;
	ClearMem( oldSize );
}


template <class T>
void scSizeableArrayD<T>::GrowSlots( int newItems )
{
	int oldSize = elemSlots_;

	if ( items_ == 0 )
		throw( -1 );

	items_ = (T*)realloc( items_, sizeof(T) * ( elemSlots_ + newItems ) );
	elemSlots_ += newItems;
	ClearMem( oldSize );
}


template <class T>		
void scSizeableArrayD<T>::ClearMem( unsigned oldsize )
{
		// either we do need to clear memory or we have shrunk it
	if ( oldsize >= elemSlots_ )
		return;

	for ( unsigned index = oldsize; index < elemSlots_; index++ )
		constructItem( index ); 	
}

#if 0
template <class T>
void scSizeableArrayD<T>::exch( const scSizeableArrayD<T>& arr )
{
	scArray<T>::operator=( arr );
	
	scSizeableArrayD<T>& array = (scSizeableArrayD<T>&)arr;

	unsigned tmp = elemSlots_;
	elemSlots_ = array.elemSlots_;
	array.elemSlots_ = tmp;

	tmp = blockSize_;
	blockSize_ = array.blockSize_;
	array.blockSize_ = blockSize_;

	tmp = retainMem_;
	retainMem_ = array.retainMem_;
	array.retainMem_ = retainMem_;		
}
#endif

template<class T, class CT>
int scBinarySortedArrayD<T,CT>::Find( const T& val, int* insertIndexP ) const
{
	int 	low 	 = 0;
	int 	high	= numItems_ - 1;
	int insertIndex = 0;

	while ( low <= high ) {
		int index = (low + high) / 2;	
		int found = CT::Compare( val, items_[index] );
		if ( found == 0 )
			return index;
		else if ( found < 0 )
			high = index - 1, insertIndex = index;
		else
			low = index + 1, insertIndex = index + 1;
	}

	insertIndexP ? *insertIndexP = insertIndex : 0;
	return -1;
}

template<class T, class CT>
int scBinarySortedArrayD<T,CT>::Find1( const T& val, const CT& ct, int* insertIndexP ) const
{
	int 	low 	 = 0;
	int 	high	= numItems_ - 1;
	int insertIndex = 0;

	while ( low <= high ) {
		int index = (low + high) / 2;	
		int found = ct.Compare1( val, items_[index] );
		if ( found == 0 )
			return index;
		else if ( found < 0 )
			high = index - 1, insertIndex = index;
		else
			low = index + 1, insertIndex = index + 1;
	}

	insertIndexP ? *insertIndexP = insertIndex : 0;
	return -1;
}

template<class T, class CT>
int scBinarySortedArrayD<T,CT>::SortInsert( const T& item )
{
	int 	index;
	if ( Find( item, &index ) < 0 )
		Insert( index, item );
	return index;	
} 


#endif /* scDEFINE_TEMPLATES */
