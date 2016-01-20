/****************************************************************************

	File:		SCOBJECT.H

	$Header: /Projects/Toolbox/ct/Sctbobj.h 2	  5/30/97 8:45a Wmanis $

	Contains:	basic toolbox object

	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	
***************************************************************************/

#ifndef _H_SCOTBBJECT
#define _H_SCOTBBJECT

#ifdef SCMACINTOSH
#pragma once 
#endif

#include "sccharex.h"
#include "scobject.h"
#include "scchflag.h"	 

/* ==================================================================== */
/* ==================================================================== */


/*
The following are for storing processing (Volatile) and attribute (Attbribute)
bits for Composition Toolbox objects. We have two types of bits - a set
for logical/content objects and another for layout objects. In one instance
we have the same bit for both objects - the PointerRestored bit. You will see
a case in the code where we send a structure to a type that does not seem
correct, this is only to attain the greatest compaction of data.
*/

	//
	// LOGICAL OBJECT BITS
	//
	// the following are for processing
	// and thus are very VOLATILE in nature
	//
#define scRETABULATE		scConstructorLogBits( 1 )
#define scREBREAK			scConstructorLogBits( 0, 1 )
#define scREPOSITION		scConstructorLogBits( 0, 0, 1 )
#define scREADCONT			scConstructorLogBits( 0, 0, 0, 1 )
#define scLOGACTIVE 		scConstructorLogBits( 0, 0, 0, 0, 1 )

	// the following are ATTRIBUTE bits
#define scPTRRESTORED		scConstructorLogBits( 0, 0, 0, 0, 0, 1 )
#define scKEEPNEXT			scConstructorLogBits( 0, 0, 0, 0, 0, 0, 1 )


struct scLogBits {
	unsigned	fRetabulate 	: 1;
	unsigned	fRebreak		: 1;
	unsigned	fReposition 	: 1;
	unsigned	fReadContent	: 1;
	unsigned	fLogActive		: 1;
	
	unsigned	fPad			: 11;
	
	unsigned	fPtrRestored	: 1;	
	unsigned	fKeepNext		: 1;

	unsigned	fPad2			: 14;

	scLogBits	operator|=( const scLogBits& );
	scLogBits	operator&=( const scLogBits& );
	scLogBits	operator~( void );
	int 		operator&( const scLogBits& ) const;	
};

struct scConstructorLogBits : public scLogBits {
	scConstructorLogBits( unsigned a = 0,
						  unsigned b = 0,
						  unsigned c = 0,
						  unsigned d = 0,
						  unsigned e = 0,
						  unsigned f = 0,
						  unsigned g = 0 )
					{						
						fRetabulate 	= a;
						fRebreak		= b;
						fReposition 	= c;
						fReadContent	= d;
						fLogActive		= e;
						fPtrRestored	= f;
						fKeepNext		= g;
						fPad			= 0;
						fPad2			= 0;
					}

};

inline scLogBits scLogBits::operator|=( const scLogBits& bits )
{
	fRetabulate 	|= bits.fRetabulate;
	fRebreak		|= bits.fRebreak;
	fReposition 	|= bits.fReposition;
	fReadContent	|= bits.fReadContent;
	fLogActive		|= bits.fLogActive;
	fPtrRestored	|= bits.fPtrRestored;
	fKeepNext		|= bits.fKeepNext;
	return *this;
}

inline scLogBits scLogBits::operator&=( const scLogBits& bits )
{
	fRetabulate 	&= bits.fRetabulate;
	fRebreak		&= bits.fRebreak;
	fReposition 	&= bits.fReposition;
	fReadContent	&= bits.fReadContent;
	fLogActive		&= bits.fLogActive;
	fPtrRestored	&= bits.fPtrRestored;
	fKeepNext		&= bits.fKeepNext;
	return *this;
}

inline scLogBits scLogBits::operator~( void )
{
	fRetabulate 	= ~fRetabulate;
	fRebreak		= ~fRebreak;
	fReposition 	= ~fReposition;
	fReadContent	= ~fReadContent;
	fLogActive		= ~fLogActive;
	fPtrRestored	= ~fPtrRestored;
	fKeepNext		= ~fKeepNext;
	return *this;
}

inline int scLogBits::operator&( const scLogBits& bits ) const
{
	return ( fRetabulate & bits.fRetabulate 	||
			 fRebreak	 & bits.fRebreak		||
			 fReposition & bits.fReposition 	||
			 fReadContent& bits.fReadContent	||			 
			 fLogActive  & bits.fLogActive		||
			 fPtrRestored& bits.fPtrRestored	||
			 fKeepNext	 & bits.fKeepNext );
}

inline scLogBits operator|( scLogBits a, scLogBits b )
{
	a |= b;
	return a;
}

/* ==================================================================== */
/* ==================================================================== */
									   
	//
	// LAYOUT OBJECT BITS
	//
	// the following are for processing
	// and thus are very VOLATILE in nature
	//
#define scINVALID				scConstructorLayBits( 1 )
#define scREALIGN				scConstructorLayBits( 0, 1 )
#define scREPAINT				scConstructorLayBits( 0, 0, 1 )
#define scLAYACTIVE 			scConstructorLayBits( 0, 0, 0, 1 )
#define scLAYcomposeACTIVE		scConstructorLayBits( 0, 0, 0, 0, 1 )

//#define scPTRRESTORED 		0x0100		// the pointers have been restored
										// on this during readin


#define scLASTLINE				scConstructorLayBits( 0, 0, 0, 0, 0, 1 )
#define scSHAPETYPE( c )		scConstructorLayBits( 0, 0, 0, 0, 0, (c) )

#define scVJTYPE( c )			scConstructorLayBits( 0, 0, 0, 0, 0, 0, (c) )
#define scLINEADJUSTMENT( c )	scConstructorLayBits( 0, 0, 0, 0, 0, 0, (c) )




struct scLayBits {						
	unsigned	fInvalid				: 1;
	unsigned	fRealign				: 1;
	unsigned	fRepaint				: 1;
	unsigned	fLayActive				: 1;
	unsigned	fLayRecomposeActive 	: 1;	

	unsigned	fPad			: 11;

	
	unsigned	fPtrRestored	: 1;
	unsigned	fLayType		: 6;		// lastline(1) or shapetype(6)
	unsigned	fLayAdjustment	: 6;		// vj for columns(3) or tsjust for flex(6)

	unsigned	fPad2			: 3;


	scLayBits	operator|=( const scLayBits& );
	scLayBits	operator&=( const scLayBits& );
	scLayBits	operator~( void );
	int 		operator&( const scLayBits& ) const;	
	
};

struct scConstructorLayBits : public scLayBits {
	scConstructorLayBits( unsigned a = 0,
						  unsigned b = 0,
						  unsigned c = 0,
						  unsigned d = 0,
						  unsigned e = 0,
						  unsigned f = 0,
						  unsigned g = 0,
						  unsigned h = 0 )
			{
				fInvalid				= a;
				fRealign				= b;
				fRepaint				= c;
				fLayActive				= d;
				fPtrRestored			= e;
				fLayType				= f;
				fLayAdjustment			= g;
				fLayRecomposeActive 	= h;
				fPad					= 0;
				fPad2					= 0;
			}
};



inline scLayBits scLayBits::operator|=( const scLayBits& bits )
{
	fInvalid			|= bits.fInvalid;
	fRealign			|= bits.fRealign;
	fRepaint			|= bits.fRepaint;
	fLayActive			|= bits.fLayActive;
	fPtrRestored		|= bits.fPtrRestored;
	fLayType			|= bits.fLayType;
	fLayAdjustment		|= bits.fLayAdjustment;
	fLayRecomposeActive |= bits.fLayRecomposeActive;	
	return *this;
}
									   
inline scLayBits scLayBits::operator&=( const scLayBits& bits )
{
	fInvalid			&= bits.fInvalid;
	fRealign			&= bits.fRealign;
	fRepaint			&= bits.fRepaint;
	fLayActive			&= bits.fLayActive;
	fPtrRestored		&= bits.fPtrRestored;
	fLayType			&= bits.fLayType;
	fLayAdjustment		&= bits.fLayAdjustment;
	fLayRecomposeActive &= bits.fLayRecomposeActive;		
	return *this;
}

inline scLayBits scLayBits::operator~( void )
{
	fInvalid			= ~fInvalid;
	fRealign			= ~fRealign;
	fRepaint			= ~fRepaint;
	fLayActive			= ~fLayActive;
	fPtrRestored		= ~fPtrRestored;
	fLayType			= ~fLayType;
	fLayAdjustment		= ~fLayAdjustment;
	fLayRecomposeActive = ~fLayRecomposeActive; 	
	return *this;
}


inline int scLayBits::operator&( const scLayBits& bits ) const
{
	return ( fInvalid				& bits.fInvalid 		||
			 fRealign				& bits.fRealign 		||
			 fRepaint				& bits.fRepaint 		||
			 fLayActive 			& bits.fLayActive		||
			 fPtrRestored			& bits.fPtrRestored 	||
			 fLayType				& bits.fLayType 		||
			 fLayAdjustment 		& bits.fLayAdjustment	||
			 fLayRecomposeActive	& bits.fLayRecomposeActive
			 );
}


inline scLayBits operator|( scLayBits a, scLayBits b )
{
	a |= b;
	return a;
}

/* ==================================================================== */
/* ==================================================================== */
/* THIS IS THE BASIC CHARACTER IN THE TEXT ARRAY */

class CharRecord {
public:
	union {
		UCS2		character;
		scCharFlags flags;
		ulong		charflags;
	};
	GlyphSize		escapement;

					CharRecord();
					CharRecord( UCS2, GlyphSize );
	void			Set( UCS2, GlyphSize );
	void			Set( UCS2, scCharFlags, GlyphSize );

	void			ClearFlags();
	
	CharRecord& 	operator=( const CharRecord& );
	int 			operator==( const CharRecord& ) const;
	int 			operator!=( const CharRecord& ) const;
};


typedef CharRecord scHuge*	CharRecordP;

typedef scMemHandle CharHandle;

inline void CharRecord::ClearFlags()
{
	if ( character == scField )
		flags.ClearMinFlags();
	else
		flags.ClearAllFlags();
}

inline void CharRecord::Set( UCS2 ch, GlyphSize width )
{
	character = ch;
	flags.ClearAllFlags();
	escapement = width;
}
	
inline void CharRecord::Set( UCS2 ch, scCharFlags theFlags, GlyphSize width )
{
	flags = theFlags;
	character = ch;
	escapement = width;
}
	
inline CharRecord::CharRecord()
{
	Set( 0, 0 );
}

inline CharRecord::CharRecord( UCS2 ch, GlyphSize width )
{
	Set( ch, width );
}
	
inline CharRecord& CharRecord::operator=( const CharRecord& chrec )
	{
		charflags	= chrec.charflags;
		escapement	= chrec.escapement;
		return *this;
	}

inline int CharRecord::operator==( const CharRecord& chrec ) const
	{
		return charflags == chrec.charflags && escapement == chrec.escapement;
	}

inline int CharRecord::operator!=( const CharRecord& chrec ) const
	{
		return !( *this == chrec );
	}

int CharacterBufLen( const UCS2 *ch );


/* ==================================================================== */

struct CharFileRecord {
	UCS2	character;
	ushort	flags;
};

/* ==================================================================== */
/* ==================================================================== */
/* ==================================================================== */
/* THE BASE LEVEL OBJECT */

class scContUnit;
class scColumn;
class scTextline;
class scSet;

class scTBObj : public scObject {
	scDECLARE_RTTI; 	
public: 
					scTBObj() :
						fEnumCount( 0 ),
						fPrev( 0 ),
						fNext( 0 ),
						fBits( 0 ) {}

					~scTBObj(){}

	void			SetPrev( scTBObj* prev )			{ fPrev = prev; }
	void			SetNext( scTBObj*  next )			{ fNext = next; }

	void			Insert( scTBObj* next );
	void			Link( scTBObj* );
	void			Unlink( void );
	Bool			LinkSetContains( scTBObj* ) const;

	scTBObj*		Prev( void ) const					{ return fPrev; }
	scTBObj*		Next( void ) const					{ return fNext; }
	
	scTBObj*		FirstInChain( void ) const;
	scTBObj*		LastInChain( void ) const;	

	virtual void	ZeroEnum( void )					{ fEnumCount = 0; } 
	virtual long	GetEnumCount( void ) const			{ return fEnumCount; }
	virtual void	Enumerate( long& enumCount )
						{ fEnumCount = ++enumCount; }


						// FILE I/O

						// read the header string of an object, 
						// create an object, and return it
	static scTBObj* StartRead( scSet*, APPCtxPtr, IOFuncPtr );

						// complete the read
	virtual void	Read( scSet*, APPCtxPtr, IOFuncPtr );

						// restore the pointers after completing a read 
	virtual void	RestorePointers( scSet* );

						// write out object header string
	static void 	WriteHeaderString( const char*, APPCtxPtr, IOFuncPtr );
	
						// write the header string of an object
	void			StartWrite( APPCtxPtr, IOFuncPtr );

						// complete the write
	virtual void	Write( APPCtxPtr, IOFuncPtr );

						// write out a null terminating object
	static void 	WriteNullObject( APPCtxPtr, IOFuncPtr );


	
	void			SetBits( ulong bits )	
						{ 
							scAssert( sizeof( scLayBits ) == sizeof( ulong ) ); 
							fBits = bits;
						}
	ulong			GetBits( void ) const		
						{ 
							return fBits; 
						}

	virtual void	Mark( const scLayBits& bits )	
						{ 
							fLayBits |= bits; 
						}
	void			Unmark( const scLayBits& bits ) 	
						{ 
							fLayBits &= ~(scLayBits&)bits; 
						}
	int 			Marked( const scLayBits bits ) const	
						{ 
							return fLayBits & bits; 
						}

	void			Mark( const scLogBits& bits )		
						{
							fLogBits |= bits; 
						}
	void			Unmark( const scLogBits& bits ) 	
						{ 
							fLogBits &= ~(scLogBits&)bits; 
						}
	int 			Marked( const scLogBits bits ) const
						{ 
							return fLogBits & bits;
						}	
	
#if SCDEBUG > 1
	virtual void	scAssertValid( Bool recurse = true ) const;
#else
	virtual void	scAssertValid( Bool = true ) const{}	
#endif

	scTBObj&		operator=( const scTBObj& tbobj )
						 {
							fEnumCount = tbobj.fEnumCount;
							fBits	   = tbobj.fBits;
							
								// DANGER WILL ROBINSON!!!
							fNext	   = tbobj.fNext;
							fPrev	   = tbobj.fPrev;
							return *this;
						 }
protected:
	long			fEnumCount;   /* file enumeration */

	union {
		ulong		fBits;
		scLayBits	fLayBits;
		scLogBits	fLogBits;
	};
	scTBObj*	fNext;		  /* handle to next object */
	scTBObj*	fPrev;		  /* handle to previous object */
};

#endif
