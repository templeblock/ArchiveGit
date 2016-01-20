/****************************************************************************

	File:		SCARRAY.H


	$Header: /btoolbox/lib/SCARRAY.H 3	   3/31/96 3:48p Will $

	Contains:	Templates for Vector

	Written by: Manis

	Copyright (c) 1989-95 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	
****************************************************************************/


#ifndef _H_SCARRAY
#define _H_SCARRAY

#ifdef _DEBUG
	#define ifdebug( x )	x
#else
	#define ifdebug( x )
#endif

/* ==================================================================== */

template <class T>
class scAutoDelete {
public:
		scAutoDelete( T* ptr = 0 ): ptr_( ptr ) {}
		~scAutoDelete() 	{ freePtr(); }

  T*	deref() const		{ return ptr_; }		
  T*	operator->() const	{ return ptr_; }
  T&	operator*() const	{ return *ptr_; }
		
  void	operator=( T* p )	{ if ( ptr_ ) freePtr(); ptr_ = p; }
  int	operator==( const scAutoDelete<T>& p )
							{ return ptr_ == p.deref(); }

private:
  void	freePtr()			{ delete ptr_, ptr_ = 0; }
  T*	ptr_;
};

/* ==================================================================== */

template <class T>
class scArray {
public:
	int 		NumItems( void ) const;
	
	T&			operator[]( int n );
	const T&	operator[]( int n ) const;

	T*			ptr( void );
	const T*	ptr( void ) const;

//	void exch( const scArray<T>& );
protected:

				scArray() :
					numItems_(0),
					items_(0){}

				scArray( int num, T* mem ) :
						numItems_( num ),
						items_( mem ) {}


	
	int 		numItems_;
	T*			items_;

#ifndef __MWERKS__	
					// the following are not declared because of the
					// deep vs shallow copy problem
				scArray( const scArray& );		// not declared
	scArray&	operator=( const scArray& );	// not declared
#endif
};


template <class T>
#ifndef __MWERKS__
inline
#endif
int  scArray<T>::NumItems() const
	{
		return numItems_;
	}

template <class T>
#ifndef __MWERKS__
inline
#endif
T&	scArray<T>::operator[]( int n )
	{
		return items_[ n ];
	}

template <class T>
#ifndef __MWERKS__
inline
#endif
const T&  scArray<T>::operator[]( int n ) const
	{
		return items_[ n ];
	}


template <class T>
#ifndef __MWERKS__
inline
#endif
T*	scArray<T>::ptr()
	{
		return items_;
	}


template <class T>
#ifndef __MWERKS__
inline
#endif
const T*  scArray<T>::ptr() const
	{
		return items_;
	}


#if 0
template <class T>
#ifndef __MWERKS__
inline
#endif
void scArray<T>::exch( const scArray<T>& arr )
{
	scArray<T>	array = (scArray<T>&)arr;
	
	int t = array.numItems_;
	array.numItems_ = numItems_;
	numItems_ = array.numItems_;
	
	T* tmp = array.items_;
	array.items_ = items_;
	items_ = tmp;
}
#endif

/* ==================================================================== */

template <class T>
class scStaticArray : public scArray<T> {
public:
				scStaticArray( int num, T* mem = 0 );
				~scStaticArray();
};

/* ==================================================================== */

template <class T>
class scStaticArrayD : public scArray<T> {
public:
				scStaticArrayD( int num, T* mem = 0 );
				~scStaticArrayD();				
};

/* ==================================================================== */

template <class T>
class scSizeableArray : public scArray<T> {
public:
				scSizeableArray();
				~scSizeableArray();
						
	void		Remove( int );
	void		RemoveAll( void );
	int 		Append( const T& );

	T&			Grow();
	
	void		Insert( int, const T& );

	void		Set( int, const T& );	

	void		SetNumSlots( int numSlots );

	void		SetRetainMem( int tf )
					{
						retainMem_ = tf ? 1 : 0;
					}
	int 		GetRetainMem( void ) const
					{
						return retainMem_;
					}

//	void		exch( const scSizeableArray<T>& );
	
private:
	void		MoreSlots( void )
					{
						GrowSlots( blockSize_ );
					}
	void		ShrinkSlots( void )
					{
						SetNumSlots( numItems_ );
					}
	
	void		SizeSlots( unsigned );
	void		GrowSlots( int );	
	void		ClearMem( unsigned );

	unsigned	elemSlots_; 		// num of elements potentially in allocated space
	unsigned	blockSize_ : 16;		// for growing and shrinking we grow in greater
									// than one element unit - this is that unit
									// typically 4
	unsigned	retainMem_ : 1; 	// do not shrink memory if this is set
};


/* ==================================================================== */

template <class T>
class scSizeableArrayD : public scArray<T> {
public:
				scSizeableArrayD();
				~scSizeableArrayD();
						
	void		Remove( int );
	void		RemoveAll( void );

	T&			Grow();
	
	int 		Append( const T& );
	void		Insert( int, const T& );

	void		Set( int, const T& );	

	void		SetNumSlots( int numSlots );

	void		SetRetainMem( int tf )
					{
						retainMem_ = tf ? 1 : 0;
					}
	int 		GetRetainMem( void ) const
					{
						return retainMem_;
					}

//	void		exch( const scSizeableArrayD<T>& );
	
private:
	void		constructItem( int );
	void		deleteItem( int );

	void		MoreSlots( void )
					{
						GrowSlots( blockSize_ );
					}
	void		ShrinkSlots( void )
					{
						SetNumSlots( numItems_ );
					}
	
	void		SizeSlots( unsigned );
	void		GrowSlots( int );	
	void		ClearMem( unsigned );

	unsigned	elemSlots_; 		// num of elements potentially in allocated space
	unsigned	blockSize_ : 16;	// for growing and shrinking we grow in greater
									// than one element unit - this is that unit
									// typically 4
	unsigned	retainMem_ : 1; 	// do not shrink memory if this is set
};


/* ==================================================================== */

// CT is a comparison class - it must have a method
// Compare( const T&, const T& )

template <class T, class CT>
class scBinarySortedArray : public scSizeableArray<T> {
public:
	int 	Find( const T&, int* insertIndex = 0 ) const;
	int 	Find1( const T&, const CT&, int* insertIndex = 0 ) const;
	int 	SortInsert( const T& );
};

/* ==================================================================== */

template <class T, class CT>
class scBinarySortedArrayD : public scSizeableArrayD<T> {
public:
	int 	Find( const T&, int* insertIndex = 0 ) const;
	int 	Find1( const T&, const CT&, int* insertIndex = 0 ) const;
	int 	SortInsert( const T& );
};

/* ==================================================================== */

#ifdef DEFINE_TEMPLATES
#include "scarray.cpp"
#endif

#endif

