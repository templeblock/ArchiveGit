/****************************************************************************

	File:		SCOBJECT.C


	$Header: /Projects/Toolbox/ct/SCTBOBJ.CPP 2     5/30/97 8:45a Wmanis $

	Contains:	basic toolbox object definition

	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	
***************************************************************************/

#include "sctbobj.h"
#include "scmem.h"
#include "scfileio.h"
#include "scset.h"


static char*	scNullObject = "scNullObject";		// used for writing out null objects

/* ==================================================================== */

#if SCDEBUG > 1

void scTBObj::scAssertValid( Bool ) const
{
}

#endif

/* ==================================================================== */

int CharacterBufLen( const UCS2* ch )
{
	int len = 0;
	for ( ; *ch++; len++ )
		;
	return len;
}

/* ==================================================================== */

void scTBObj::Link( scTBObj* obj2 )
{
	scTBObj*	nextObj;
	scTBObj*	lastObj;

	nextObj		= Next();

	lastObj		= obj2->LastInChain( );

	SetNext( obj2 );
	obj2->SetPrev( this );

	if ( lastObj )
		lastObj->SetNext( nextObj );

	if ( nextObj )
		nextObj->SetPrev( lastObj );
}

/* ==================================================================== */

void scTBObj::Unlink( )
{
	scTBObj*	prev;
	scTBObj*	next;

	prev = Prev();
	next = Next();

	if ( prev )
		prev->SetNext( next );
	if ( next )
		next->SetPrev( prev );

	SetPrev( 0 );
	SetNext( 0 );
}

/* ==================================================================== */
// insert the this into the chain before the arg

void scTBObj::Insert( scTBObj* next )
{
	scTBObj* prev;
	
	scAssert( this && next );

	scAssert( Prev() == 0 && Next() == 0 );

	if ( ( prev = next->Prev() ) != 0 ) {
		SetPrev( prev );
		prev->SetNext( this );
	}
	next->SetPrev( this );
	SetNext( next );
}

/* ==================================================================== */

scTBObj* scTBObj::FirstInChain( void ) const
{
	scTBObj*	prev;
	scTBObj*	obj = (scTBObj*)this;

	scAssert( this != 0 );
	for ( prev = Prev(); prev; prev = obj->Prev() )
		obj = prev;
	
	return obj;
}

/* ==================================================================== */

scTBObj* scTBObj::LastInChain( void ) const
{
	scTBObj* next;
	scTBObj* obj = (scTBObj*)this;

	if ( this ) {
		for ( next = Next(); next; next = obj->Next() )
			obj = next;
	}
	return obj;
}
	
/* ==================================================================== */

Bool scTBObj::LinkSetContains( scTBObj* obj ) const
{
	const scTBObj*	myChainObj = FirstInChain();
	
	for ( ; myChainObj;  myChainObj = myChainObj->fNext ) {
		if ( myChainObj == obj )
			return true;
	}
	return false;
}

/* ==================================================================== */

#define kHeaderNameSize		32
#define FILE_SIZE_OBJECT	12

scTBObj* scTBObj::StartRead( scSet*		enumTable,
							 APPCtxPtr	ctxPtr,
							 IOFuncPtr	readFunc )
{
	scTBObj*	obj = 0;
	char		buf[kHeaderNameSize];

	ReadBytes( (uchar*)buf, ctxPtr, readFunc, kHeaderNameSize );
	
	raise_if( strlen( buf ) == 0, scERRfile );

	if ( strcmp( buf, scNullObject ) ) {
		obj = (scTBObj*)scClass::MakeInstance( buf );

		raise_if( obj == 0, scERRfile );

		long uval;
		ReadLong( uval, ctxPtr, readFunc, kIntelOrder );

//		SCDebugTrace( 0, scString( "%s %d\n" ), buf, uval );		
		
		enumTable->Set( uval, obj );
		
		obj->fEnumCount = uval;
		
		obj->Read( enumTable, ctxPtr, readFunc );
		
		return obj;
	}
//	SCDebugTrace( 0, scString( "%s\n" ), buf );	
	return 0;
}

/* ==================================================================== */

void scTBObj::Read( scSet*		enumTable,
					APPCtxPtr	ctxPtr,
				    IOFuncPtr	readFunc )
{
	uchar			abuf[FILE_SIZE_OBJECT];
	const uchar*	pbuf	= abuf;

	ReadBytes( abuf, ctxPtr, readFunc, FILE_SIZE_OBJECT );
	
	ulong	val;
	pbuf = BufGet_long( pbuf, val, kIntelOrder );
	fBits = val;

	pbuf = BufGet_long( pbuf, val, kIntelOrder );
	fNext = (scTBObj*)val;

	pbuf = BufGet_long( pbuf, val, kIntelOrder );		
	fPrev = (scTBObj*)val;

	Unmark( scPTRRESTORED );
	scAssert ((size_t)(pbuf-abuf) == FILE_SIZE_OBJECT );
}

/* ==================================================================== */

void scTBObj::WriteHeaderString( const char*	name,
								 APPCtxPtr		ctxPtr,	
								 IOFuncPtr		writeFunc )
{
	char buf[kHeaderNameSize];

	memset( buf, '=', kHeaderNameSize );
	strcpy( buf, name );
	WriteBytes( (const uchar*)buf, ctxPtr, writeFunc, kHeaderNameSize );
//	SCDebugTrace( 0, scString( "%s" ), buf );
}

/* ==================================================================== */
// NULL TERMINATES THE OBJECT LIST ON DISK

void scTBObj::WriteNullObject( APPCtxPtr	ctxPtr,
							   IOFuncPtr	writeFunc )
{
	scTBObj::WriteHeaderString( scNullObject, ctxPtr, writeFunc );
//	SCDebugTrace( 0, scString( "\n" ) );
}

/* ==================================================================== */
// write the header string of an object
// the basic file format is as:
//		* header string with object name	32 bytes
//		* enum count						 4 bytes
//		* individual records with values in a predetermined byte order
//

void scTBObj::StartWrite( APPCtxPtr		ctxPtr,
						  IOFuncPtr		writeFunc )
{
	scTBObj::WriteHeaderString( GetClassname(), ctxPtr, writeFunc );
//	SCDebugTrace( 0, scString( "    %d\n" ), fEnumCount );

	WriteLong( fEnumCount, ctxPtr, writeFunc, kIntelOrder );

		// write the rest of the object out
	Write( ctxPtr, writeFunc );
}

/* ==================================================================== */

void scTBObj::Write( APPCtxPtr	ctxPtr,
					 IOFuncPtr	writeFunc )
{
	uchar	abuf[FILE_SIZE_OBJECT];
	uchar*	pbuf	= abuf;

	pbuf = BufSet_long( pbuf, GetBits( ), kIntelOrder );
	pbuf = BufSet_long( pbuf, fNext ? fNext->GetEnumCount() : 0, kIntelOrder );
	pbuf = BufSet_long( pbuf, fPrev ? fPrev->GetEnumCount() : 0, kIntelOrder );		

	scAssert ((size_t)(pbuf-abuf) == FILE_SIZE_OBJECT );

	WriteBytes( abuf, ctxPtr, writeFunc, FILE_SIZE_OBJECT );
}

/* ==================================================================== */
	
void scTBObj::RestorePointers( scSet* enumTable )
{
	if ( !Marked( scPTRRESTORED ) ) {
		fNext = (scTBObj*)enumTable->Get( (long)fNext );
		fPrev = (scTBObj*)enumTable->Get( (long)fPrev );		
		Mark( scPTRRESTORED );
	}
}

/* ==================================================================== */	
