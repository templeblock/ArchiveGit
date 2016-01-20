/************************************************************************

	File:		scmemarr.h

	$Header: /Projects/Toolbox/ct/SCMEMARR.H 2	   5/30/97 8:45a Wmanis $

	Contains:	The variable size array class.

	Written by: Manis

	Copyright (c) 1989-1994 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.


**************************************************************************/

#ifndef _H_CMEMARR
#define _H_CMEMARR
			
#include "sctypes.h"
#include "scobject.h"
#include "scexcept.h"
   

typedef char scFar *ElementPtr;

typedef void (*CVoidFunc0)( ElementPtr );
typedef void (*CVoidFunc1)( ElementPtr, long );
typedef void (*CVoidFunc2)( ElementPtr, long, long );


typedef Bool (*CBoolFunc0)( ElementPtr );
typedef Bool (*CBoolFunc1)( ElementPtr, long );
typedef Bool (*CBoolFunc2)( ElementPtr, long, long );

typedef Bool (*CBoolConstFunc0)( const ElementPtr );
typedef Bool (*CBoolConstFunc1)( const ElementPtr, long );
typedef Bool (*CBoolConstFunc2)( const ElementPtr, long, long );

extern "C" {
	typedef int (scCDecl* CPtrArrayCmpFunc)(const void *, const void *);
}

/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */
/* ======================================================================== */

class scAbstractArray : public scObject {
	scDECLARE_RTTI; 	
public:
					scAbstractArray( int elemsize, unsigned clearmem = 0 ) :
						fElemSize( elemsize ),
						fNumItems( 0 ),
						fElemSlots( 0 ),
						fBlockSize( 4 ),
						fClearMem( clearmem ),
						fRetainMem( 0 ),
						fPad( 0 ),
						fItems( 0 ) {}
							
					

					scAbstractArray( const scAbstractArray& aa ) :
								fElemSize( aa.fElemSize ),
								fNumItems( aa.fNumItems ),
								fElemSlots( 0 ),
								fBlockSize( aa.fBlockSize ),
								fClearMem( aa.fClearMem ),
								fRetainMem( aa.fRetainMem ),
								fPad( 0 ),
								fItems( 0 ) {}

					
					~scAbstractArray()
					{
						fElemSize	= 0, 
						fNumItems	= 0,
						fElemSlots	= 0,
						fBlockSize	= 4,
						fItems		= 0;
					}
							
	
	scAbstractArray&	operator=( const scAbstractArray& );

	virtual int 	IsEqual( const scObject& ) const;

		// use this call extrememly wisely for purposes of copying
	void			SetMemory( void *mem )
					{
						fItems = mem;
					}

	long			GetNumItems( void ) const
					{
						return fNumItems;
					}

	virtual void	RemoveAll( void )
					{
						fNumItems=0, ShrinkSlots();
					}

	virtual void	Paste( long, const scAbstractArray& );
	virtual void	Copy( scAbstractArray&, long, long ) const;
	virtual void	Cut( scAbstractArray&, long, long );	
	
	void			RemoveDataAt( long, long num = 1 );
	void			AppendData( const ElementPtr, long num = 1 );
	void			AlterDataAt( long, ElementPtr, long num = 1 );
	void			InsertAt( long, ElementPtr, long num = 1 );
	ElementPtr		GetDataAt( long, ElementPtr, long num = 1 ) const;
	
					// stack operators
	void			Push( const ElementPtr p, long elements = 1 )
					{
						AppendData( p, elements );
					}
	ElementPtr		Pop( ElementPtr, long elements = 1 );
	
					// get the top element(s)
	ElementPtr		GetTop( ElementPtr, long elements = 1 );
	
		
	void			DoForEach( CVoidFunc0 );
	void			DoForEach( CVoidFunc1, long );
	void			DoForEach( CVoidFunc2, long, long  );


		// these will return the index of the first
		// successful match of the data
		// NOTE: if their is NO match it RETURNS -1
	long			FirstSuccess( CBoolFunc0 );
	long			FirstSuccess( CBoolFunc1, long );
	long			FirstSuccess( CBoolFunc2, long, long  );
	
	long			FirstSuccess( CBoolConstFunc0 ) const;
	long			FirstSuccess( CBoolConstFunc1, long ) const;
	long			FirstSuccess( CBoolConstFunc2, long, long  ) const;
	
		// these will return the index of the nth
		// successful match of the data
		// NOTE: if their is NO match it RETURNS -1
	long			NthSuccess( CBoolFunc0, long nth );
	long			NthSuccess( CBoolFunc1, long nth, long );
	long			NthSuccess( CBoolFunc2, long nth, long, long  );
	
	long			NthSuccess( CBoolConstFunc0, long nth ) const;
	long			NthSuccess( CBoolConstFunc1, long nth, long ) const;
	long			NthSuccess( CBoolConstFunc2, long nth, long, long ) const;

	void			QuickSort( CPtrArrayCmpFunc );	

	void			SetRetainMem( unsigned tf )
					{
						fRetainMem = tf ? 1 : 0;
					}
	unsigned		GetRetainMem( void ) const
					{
						return fRetainMem;
					}
	
	virtual void	SetNumSlots( long numSlots )
					{
						SizeSlots( ( ( numSlots / fBlockSize ) + 1 ) * fBlockSize );
					}
	
protected:
	virtual void	MoreSlots( void )
					{
						GrowSlots( fBlockSize );
					}
	virtual void	SizeSlots( long ) = 0;
	virtual void	GrowSlots( long ) = 0;	
	virtual void	ShrinkSlots( void )
					{
						SetNumSlots( fNumItems );
					}
	void			ClearMem( long oldsize );

	virtual ElementPtr	Lock( void ) const = 0;
	virtual void		Unlock( void ) const = 0;

	long		fNumItems;			// num of elements in the array
	long		fElemSlots; 		// num of elements potentially in allocated space
	unsigned	fElemSize	: 16;	// element size
	unsigned	fBlockSize	: 8;	// for growing and shrinking we grow in greater
									// than one element unit - this is that unit
									// typically 4
	unsigned	fClearMem	: 1;	// if this is set this will zero out mem
									// that is allocated
	unsigned	fRetainMem	: 1;	// do not shrink memory if this is set
	unsigned	fPad		: 7;	
	void*		fItems; 			// the data
};


/* ==================================================================== */
/* ==================================================================== */


class scMemArray : public scAbstractArray {
	scDECLARE_RTTI; 	
public:
					scMemArray() : scAbstractArray( sizeof( void* ) ){} 				
					scMemArray( size_t elemSize, unsigned clearmem = 0 );
					scMemArray( const scMemArray& );
					~scMemArray();

	scMemArray& 	operator=( const scMemArray& );
	
	ElementPtr		GetMem( void ) const
					{
						return (ElementPtr)fItems;
					}
	ElementPtr		GetMem( long n ) const
					{
						return (ElementPtr)((char*)fItems + (n*fElemSize));
					}
	
	virtual int 	IsEqual( const scObject& ) const;

protected:
	void			GrowSlots( long );
	void			SizeSlots( long );
	
	ElementPtr		Lock( void ) const;
	void			Unlock( void ) const	{}
};


/* ==================================================================== */
/* ==================================================================== */

class scHandleArrayLock;

class scHandleArray : public scAbstractArray {
	scDECLARE_RTTI; 	
	friend scHandleArrayLock;
public:
					scHandleArray() : scAbstractArray( sizeof( void* ) ){}
					scHandleArray( size_t elemSize, unsigned clearmem = 0 );
					scHandleArray( const scHandleArray&, unsigned clearmem = 0 );
					
					~scHandleArray();					

	scHandleArray&	operator=( const scHandleArray& );

		// NOTE: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// these rely upon the fact the memory manager uses a mac type
		// handle, smart heap does this so if you use smart heap or the
		// stonehand memory manager you are safe
		// NOTE: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!	
	ElementPtr		GetMem( void ) const
					{
						return (ElementPtr)*((char**)fItems);
					}
	ElementPtr		GetMem( long n ) const
					{
						return (ElementPtr)(*((char**)fItems) + (n*fElemSize));
					}	

	virtual int 	IsEqual( const scObject& ) const;

protected:
	void			GrowSlots( long );
	void			SizeSlots( long );
	
	ElementPtr		Lock( void ) const;
	void			Unlock( void ) const;
};


class scHandleArrayLock {
public:
			scHandleArrayLock( scHandleArray* array ) :
					array_( *array )
					{
						array_.Lock();
					}
			scHandleArrayLock( scHandleArray& array ) :
					array_( array )
					{
						array_.Lock();
					}
		   ~scHandleArrayLock( void)
					{
					   array_.Unlock();
					}
			   

	void	*operator *() { return array_.GetMem(); }
	
private:
	scHandleArray&	array_;
};


#endif /* _H_CMEMARR */
