/*=================================================================
Contains:	Impelmentation of rubi storarge.
=================================================================*/

#include "scrange.h"
#include "sccallbk.h"
#include "scrubi.h"
#include "scannota.h"
#include "sctbobj.h"

#define kRubiMagic		0x1a1a1a1a

	// for writing out rubi data - str + start + end + spec 
#define kRubiDataSize	( 16 + 4 + 4 + 4 ) 

	// for writing out rubi data - magic + str + start + end + spec 
#define kRubiDataSize2	( 4 + 32 + 4 + 4 + 4 ) 

/* ==================================================================== */
void scAnnotation::Set( UCS2 *ch, int paraoffset, int start, int end  )
{
	memcpy( fCharStr, ch, 34 );
	fAnnotate		= fCharStr[0] != 0;
	fParaOffset 	= paraoffset;
	fStartOffset	= start;
	fEndOffset		= end;
}

/* ==================================================================== */
scRubiArray::scRubiArray( )
	: scMemArray( sizeof( scRubiData ) )
{
}

/* ==================================================================== */
scRubiArray::~scRubiArray()
{
}
			
/* ==================================================================== */
scRubiArray& scRubiArray::operator=( const scRubiArray& ra )
{
	scMemArray::operator=( ra );
	return *this;
}

/* ==================================================================== */
long scRubiArray::ExternalSize( void )
{
	return kRubiDataSize2 * GetNumItems();
}

/* ==================================================================== */
// is there an annotation at this location
static BOOL is_rubi_at( const ElementPtr ptr, long start, long end )
{
	scRubiData& rd = (scRubiData&)*ptr;
	scRange r1( rd.fStartOffset, rd.fEndOffset );
	scRange r2( start, end );

	return r1.Exclusive_Sect( r2 );
}

/* ==================================================================== */
BOOL scRubiArray::IsRubiData( long start, long end )
{
	return FirstSuccess( is_rubi_at, start, end  ) >= 0;
}

/* ==================================================================== */
// if the offset is at a border we will not return true
BOOL scRubiArray::IsRubiData( long offset )
{
	long		index;
	int 		nth;
	scRubiData	rd;
	
	for ( nth = 1; ( index = NthSuccess( is_rubi_at, nth, offset, offset ) ) >= 0; nth++ ) {
		GetDataAt( (int)index, (ElementPtr)&rd );
		if ( offset > rd.fStartOffset && offset < rd.fEndOffset )
			return true;
	}
	return false;
}

/* ==================================================================== */
// as we edit text change the annotations
static void bump_rubi_data( ElementPtr ptr, long start, long amount )
{
	scRubiData& rd = (scRubiData&)*ptr;
	
	if ( start <= rd.fStartOffset )
		rd.TransOffsets( amount );
}
	
/* ==================================================================== */
void scRubiArray::BumpRubiData( long start, long amount )
{
	DoForEach( bump_rubi_data, (long)start, amount );
}

/* ==================================================================== */
// apply the style to the rubidata found within the bounds
void scRubiArray::ApplyStyle( long start, long end, TypeSpec ts )
{
	scRubiData	rd;
	long		index;
	int 		nth;
	
	for ( nth = 1; ( index = NthSuccess( is_rubi_at, nth, start, end ) ) >= 0; nth++ ) {
		GetDataAt( (int)index, (ElementPtr)&rd );
		if ( rd.fStartOffset >= start && rd.fStartOffset < end ) {
			rd.fRubiSpec = ts;
			AlterDataAt( (int)index, (ElementPtr)&rd );
		}
	}
}

/* ==================================================================== */
extern "C" {
	static int _cdecl rubi_sort( const void *p1, const void *p2 )
	{
		scRubiData& rd1 = *(scRubiData *)p1;
		scRubiData& rd2 = *(scRubiData *)p2;

		return (int)(rd1.fStartOffset - rd2.fStartOffset) ;
	}
}

/* ==================================================================== */
// add rubi data and sort the data
BOOL scRubiArray::AddRubiData( scRubiData& rd )
{
	if ( IsRubiData( rd.fStartOffset, rd.fEndOffset ) )
		return false;
	
	AppendData( (ElementPtr)&rd );
	QuickSort( rubi_sort );
	return true;
}

/* ==================================================================== */
BOOL scRubiArray::GetRubiAt( scRubiData& rd, long offset )
{
	long		index;
	int 		nth;
	
	for ( nth = 1; ( index = NthSuccess( is_rubi_at, nth, offset, offset ) ) >= 0; nth++ ) {
		GetDataAt( (int)index, (ElementPtr)&rd );
		if ( offset > rd.fStartOffset && offset < rd.fEndOffset )
			return true;
	}
	return false;
}

/* ==================================================================== */
BOOL scRubiArray::GetNthRubi( int& index, scRubiData& rubiData, int nth, long start, long end )
{
	index = (int)NthSuccess( is_rubi_at, nth, start, end );

	if ( index < 0 )
		return false;
	
	GetDataAt( index, (ElementPtr)&rubiData );

	return true;
}

/* ==================================================================== */
// place the rubiarray into the existing rubi array at the
// indicated offset with it covering the number of chars
// indicated
void scRubiArray::Paste( const scRubiArray& ra, long offset, int size )
{
	scRubiData	rd;
	int 		i;
	
	BumpRubiData( offset, size );
	
	for ( i = 0; i < ra.GetNumItems(); i++ ) {
		ra.GetDataAt( i, (ElementPtr)&rd );
		rd.TransOffsets( offset );
		AddRubiData( rd );
	}
}

/* ==================================================================== */
// delete rubi data at the offset
void scRubiArray::DeleteRubiData( long offset )
{
	int 		index;
	scRubiData	rd;

	index = FirstSuccess( is_rubi_at, offset, offset );

	if ( index >= 0 )
		RemoveDataAt( index );
}

/* ==================================================================== */
// delete rubi data between the indicated offsets
void scRubiArray::DeleteRubiData( long start, long end )
{
	int index;
	
	while ( ( index = FirstSuccess( is_rubi_at, start, end ) ) >= 0 ) {
		RemoveDataAt( index );		
	}

	BumpRubiData( end, start - end );
}

/* ==================================================================== */
	static void readrubidata( ElementPtr ptr, long ctxPtr, long readFunc )
	{
		((scRubiData *)ptr)->Read( (APPCtxPtr)ctxPtr, (IOFuncPtr)readFunc );
	}

void scRubiArray::Read( APPCtxPtr	ctxPtr,
						IOFuncPtr		readFunc,
						int 			numToRead )
{
	GrowSlots( numToRead );
	fNumItems	= numToRead;
	
	DoForEach( readrubidata, (long)ctxPtr, (long)readFunc );
}

/* ==================================================================== */
static void writerubidata( ElementPtr ptr, long ctxPtr, long writeFunc )
{
	((scRubiData *)ptr)->Write( (APPCtxPtr)ctxPtr, (IOFuncPtr)writeFunc );
}

/* ==================================================================== */
void scRubiArray::Write( APPCtxPtr	ctxPtr,
						 IOFuncPtr		writeFunc )
{
	DoForEach( writerubidata, (long)ctxPtr, (long)writeFunc );
}

/* ==================================================================== */
static void ptrrestorerubidata( ElementPtr ptr )
{
	((scRubiData *)ptr)->PtrRestore();
}

/* ==================================================================== */
void scRubiArray::PtrRestore( )
{
	DoForEach( ptrrestorerubidata );
}

/* ==================================================================== */
scRubiData::scRubiData( const UCS2 *ch, long start, long end, TypeSpec ts )
{
	int len = MIN( CharacterBufLen( ch ), 16 );
	
	memcpy( fCh, ch, len * sizeof( UCS2 ) );
	fCh[len] = 0;
	fStartOffset		= start;
	fEndOffset			= end;
	fRubiSpec			= ts;	
	fOrg.Set( 0, 0 );
	fExtents.Set( 0, 0, 0, 0 );
}

/* ==================================================================== */
void scRubiData::Read( APPCtxPtr	ctxPtr,
					   IOFuncPtr	readFunc )
{
	char buf[kRubiDataSize2];
	
	int readin = (*readFunc)( ctxPtr, buf, 4 );
	
		// this nonsense is to fix a bug int the original i/o and to
		// try and maintain fill compatability
	if ( *(long*)buf == kRubiMagic ) {
		int readin = (*readFunc)( ctxPtr, buf + 4, sizeof( buf ) - 4 );
	
		::memcpy( fCh, buf + 4, 16 * sizeof( UCS2 ) );
		fCh[16] 		= 0;	
		fStartOffset	= *(long *)( buf + 36 );
		fEndOffset		= *(long *)( buf + 40 );
//		fRubiSpec		= (TypeSpec)*(long *)( buf + 44 );
//		long diskid = APPPointerToDiskID( ctxPtr,
//												 (*this)[i].spec().ptr(),
//												 diskidTypespec );

	}
	else {
		int readin = (*readFunc)( ctxPtr, buf + 4, kRubiDataSize - 4 );
	
		::memcpy( fCh, buf, 8 * sizeof( UCS2 ) );
		fCh[8]			= 0;	
		fStartOffset	= *(long *)( buf + 16 );
		fEndOffset		= *(long *)( buf + 20 );
//		fRubiSpec		= (TypeSpec)*(long *)( buf + 24 );
	}
}

/* ==================================================================== */
void scRubiData::PtrRestore( void )
{
//	fRubiSpec = (TypeSpec)APPDiskIDToPointer( (ulong)fRubiSpec );
}

/* ==================================================================== */
void scRubiData::Write( APPCtxPtr	ctxPtr,
						IOFuncPtr		writeFunc )
{
	char buf[kRubiDataSize2];
	
	*(long *)buf			= kRubiMagic;
	
	memcpy( buf + 4, fCh, 16 * sizeof( UCS2 ) );
	
	*(long *)( buf + 36 )	= fStartOffset;
	*(long *)( buf + 40 )	= fEndOffset;
//	*(long *)( buf + 44 )	= (ulong)APPPointerToDiskID( fRubiSpec );

	int written = (*writeFunc)( ctxPtr, buf, sizeof( buf ) );	
}

/* ==================================================================== */
void scRubiData::TransOffsets( long offset )
{
	fStartOffset	+= offset;
	fEndOffset		+= offset;
}
