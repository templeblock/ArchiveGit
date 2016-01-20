/*=================================================================
Contains:	xxx put contents here xxx
=================================================================*/

#include "scspcrec.h"
#include "scfileio.h"
#include "sccallbk.h"
#include "scexcept.h"

int32 scSpecRun::terminate_ = LONG_MAX;

/* ==================================================================== */
scSpecRun::scSpecRun()
{
	terminate();
}

/* ==================================================================== */
scSpecRun::scSpecRun( const scSpecRun& sr )
{
	for ( int i = 0; i < sr.NumItems(); i++ )
		Append( sr[i] );
}

/* ==================================================================== */
scSpecRun::scSpecRun( TypeSpec firstspec )
{
	Append( scSpecRecord( firstspec, 0 ) );
	terminate();
}

/* ==================================================================== */
scSpecRun::~scSpecRun()
{
}

/* ==================================================================== */
TypeSpec scSpecRun::GetFirstSpec( void )
{
	return (*this)[0].spec();	
}

/* ==================================================================== */
TypeSpec scSpecRun::SpecAtOffset( int32 offset )
{
	return (*this)[ indexAtOffset( offset ) ].spec();	
}

/* ==================================================================== */
TypeSpec scSpecRun::GetLastSpec( void )
{
	return (*this)[NumItems()].spec();
}

/* ==================================================================== */
int32 scSpecRun::GetLastOffset( void )
{
	if ( NumItems() > 2 )
		return (*this)[ NumItems() - 2 ].offset();
	return 0;
}

/* ==================================================================== */
void scSpecRun::insertSpecRec( const scSpecRecord& sr, int32 index )
{
	Insert( index, sr );
}

/* ==================================================================== */
void scSpecRun::AppendSpec( TypeSpec ts, int32 offset )
{
	int index = NumItems() - 1;
	scAssert( index >= 0 );

	scSpecRecord rec( ts, offset );
	Insert( index, rec );
	consolidate( 0 );
}

/* ==================================================================== */
void scSpecRun::ApplySpec( TypeSpec spec, int32 start, int32 end )
{
	int startIndex = indexAtOffset( start );
	int endIndex   = indexAtOffset( end );

	if ( spec == (*this)[startIndex].spec() ) {
		if ( startIndex == endIndex )
			return;
		if ( startIndex + 1 == endIndex && (*this)[endIndex].offset() == end )
			return;
	}

	TypeSpec ts = SpecAtOffset( end );
	scSpecRecord endrec( ts, end );

	if ( (*this)[startIndex].offset() == start )
		(*this)[startIndex].spec_.exch( spec );
	else {
		scSpecRecord startrec( spec, start );
		insertSpecRec( startrec, ++startIndex );
	}
	++startIndex;
	while ( (*this)[startIndex].offset() <= end ) {
		if ( !(*this)[startIndex].isTerminator() )
			Remove( startIndex );
		else
			break;
	}


	if ( !endrec.isTerminator() )
		insertSpecRec( endrec, startIndex );

	consolidate();

	DebugRun( "ApplySpec - end" );
}

/* ==================================================================== */
void scSpecRun::Clear( int32 start, int32 end )
{
	if ( start == end )
		return;

	int tailInsert = 0;
	TypeSpec ts = SpecAtOffset( end );
	scSpecRecord endrec( ts, end );

	int32	startIndex	= indexAtOffset( start );
	if ( (*this)[startIndex].offset() != start )
		startIndex++;
	int32	endIndex	= indexAtOffset( end );
	if ( (*this)[endIndex].offset() != end )
		tailInsert = 1;

#if 1
	while ( (*this)[startIndex].offset() < end )
		Remove( startIndex ); 

	if ( startIndex ) {
		while ( !(*this)[startIndex].isTerminator() && (*this)[startIndex -1 ].spec() == (*this)[startIndex].spec() )
			Remove( startIndex ); 
	}

	if ( tailInsert )
		Insert( startIndex, endrec );

	BumpOffset( start, -(end - start) );

#else
	if ( startIndex != endIndex ) {
		while ( (*this)[startIndex].offset() < end )
			Remove( startIndex ); 

		if ( startIndex ) {
			while ( !(*this)[startIndex].isTerminator() && (*this)[startIndex -1 ].spec() == (*this)[startIndex].spec() )
				Remove( startIndex ); 
		}

		if ( tailInsert )
			Insert( startIndex, endrec );
	}

	BumpOffset( start, -(end - start) );
#endif

	DebugRun( "Clear: end" );
}

/* ==================================================================== */
int32 scSpecRun::removeOffsets( int32 start, int32 end )
{
	int32	startIndex	= indexAtOffset( start );
	if ( (*this)[startIndex].offset() != start )
		startIndex++;

	while ( (*this)[startIndex].offset() < end )
		Remove( startIndex ); 
	return startIndex;
}

/* ==================================================================== */
void scSpecRun::InsertRun( int32 offset, int32 len, const scSpecRun& sr )
{
	if ( len == 0 )
		return;

	DebugRun( "Insert: destination 1" );
	sr.DebugRun( "Insert: source 1a" );

	TypeSpec startspec = SpecAtOffset( offset );
	scSpecRecord endrec( startspec, offset + len );
	
	int index = indexAtOffset( offset );
	BumpOffset( offset, len );

	//PrintRun( "Insert: destination 2" );

	int i = 0;
	for (i = 0; i < sr.NumItems() && !sr[i].isTerminator(); i++ ) {
		TypeSpec ts = sr[i].spec();
		scSpecRecord rec( ts, sr[i].offset() + offset );
		Insert( index + i + 1, rec );
	}
	
	//PrintRun( "Insert: destination 3" );

	Insert( index + i + 1, endrec );

	//PrintRun( "Insert: destination 4" );

	while( !(*this)[index].isTerminator() ) {
		if ( (*this)[index].spec() == (*this)[index+1].spec() ) {
			Remove( index+1 );
		}
		else if ( (*this)[index].offset() >= (*this)[index+1].offset() ) {
			Remove( index );
		}
		else
			index++;
	}
	consolidate();

	//PrintRun( "Insert: destination 5" );
	DebugRun( "Insert: source 5" );
}

/* ==================================================================== */
void scSpecRun::bumpIndicies( int32 startIndex, int32 amount )
{
	for ( int index = startIndex; !(*this)[index].isTerminator(); index++ )
		(*this)[index].bumpOffset( amount );

		// if we were decrementing then check whether we now have two specs with same offset
	if ( amount < 0 )
		backwardCleanUpRun( startIndex );
}

/* ==================================================================== */
void scSpecRun::BumpOffset( int32 offset, int32 amount )
{
	long		startIndex;
	long		index;

	if ( amount == 0 )
		return;

	startIndex = indexAtOffset( offset );
		
	if ( amount > 0 ) {
		for ( index = startIndex; !(*this)[index].isTerminator(); index++ ) {
			if ( (*this)[index].offset() > offset )
				(*this)[index].bumpOffset( amount );
		}
	}
	else {
		if ( !startIndex && (*this)[startIndex].offset() != 0 )
			;
		else if ( (*this)[startIndex].offset() <= offset )
			startIndex++;
		for ( index = startIndex; !(*this)[index].isTerminator(); index++ )
			(*this)[index].bumpOffset( amount );
		
			// if we were decrementing then check whether we now have 
			// two specs with same offset
		backwardCleanUpRun( startIndex );
	}


	DebugRun( "BumpOffset: end" );
}


/* ==================================================================== */
void scSpecRun::removeIndicies( int32 startIndex, int32 endIndex )
{
	int recsToRemove = endIndex - startIndex;

	for ( int i = 0; i < recsToRemove; i++ )
		Remove( startIndex );
}

/* ==================================================================== */
int scSpecRun::Includes( TypeSpec spec )
{
	for ( int i = 0; i < NumItems(); i++ ) {
		if ( spec == (*this)[i].spec() )
			return 1;
	}
	return 0;
}

/* ==================================================================== */
scSpecRecord& scSpecRun::SpecRecAtOffset( int32 offset )
{
	return (*this)[ indexAtOffset( offset ) ];	
}

/* ==================================================================== */
const scSpecRecord& scSpecRun::SpecRecAtOffset( int32 offset ) const
{
	return (*this)[ indexAtOffset( offset ) ];
}

/* ==================================================================== */
void scSpecRun::Copy( scSpecRun& dst, int32 start, int32 end ) const
{
	dst.RemoveAll();
	//PrintRun( "scSpecRun::Copy" );

	int index1 = indexAtOffset( start );
	int index2 = indexAtOffset( end > start ? end - 1 : end  );

	for ( int i = index1; i <= index2; i++ ) {
		dst.Insert( i - index1, (*this)[ i ] );
		dst[i - index1].bumpOffset( -start );
	}
	dst.terminate();

	dst.DebugRun( "Copy: destination" );
}

/* ==================================================================== */
scSpecRun& scSpecRun::operator=( const scSpecRun& sr )
{
	RemoveAll();
	for ( int i = 0; i < sr.NumItems(); i++ )
		Append( sr[i] );
	return *this;
}

/* ==================================================================== */
int scSpecRun::operator==(const scSpecRun& sr ) const
{
	if ( NumItems() != sr.NumItems() )
		return 0;
	for ( int i = 0; i < NumItems(); i++ ) {
		if ( (*this)[i] != sr[i] )
			return 0;
	}
	return 1;
}

/* ==================================================================== */
int scSpecRun::operator!=(const scSpecRun& ) const
{
	return 0;
}

/* ==================================================================== */
int scSpecRun::isTerminated() const
{
	int index = NumItems() - 1;

	if ( index >= 0 )
		return (*this)[ index ].offset() == terminate_;
	return 0;
}

/* ==================================================================== */
void scSpecRun::terminate()
{
	TypeSpec ts(0);
	scSpecRecord sr( ts, terminate_ );
	Append( sr );
}

/* ==================================================================== */
int scSpecRun::indexAtOffset( int32 offset ) const 
{
	int index;

	for ( index = 0; index < NumItems() && offset >= (*this)[index+1].offset(); index++ ) {
		if ( (*this)[index].isTerminator() )
			break;
	}
	return index;
}

/* ==================================================================== */
void scSpecRun::backwardCleanUpRun( int startIndex )
{
	int 		index;

	if ( startIndex > 0 ) {
		if ( (*this)[startIndex].offset() <= (*this)[startIndex-1].offset() ) {
			(*this)[startIndex].offset() = (*this)[startIndex-1].offset();		//??
			Remove( startIndex - 1 );
		}
		else if ( (*this)[startIndex].spec() == (*this)[startIndex-1].spec() )
			Remove( startIndex );
	}

	for ( index = 0; NumItems() > 1 && index+1 < NumItems();  ) {
		if ( (*this)[index].offset() >= (*this)[index+1].offset()
				|| (*this)[index].spec() == (*this)[index+1].spec() ) {
			Remove( index + 1 );
		}
		else
			index++;
	}
}

#ifdef SCDEBUG

/* ==================================================================== */
void scSpecRun::DebugRun( const char* str ) const
{
	int foo;

	if ( (*this)[0].offset() != 0 || !isTerminated() )
		PrintRun( str );

	scAssert( (*this)[0].offset() == 0 );
	isTerminated();

#if 1
	static int debugrun;

	if ( debugrun )
		PrintRun( str );
	else {
		int printit = false;

		for ( int index = 0; index < NumItems() - 1; index++ ) {
			const scSpecRecord& s1 = (*this)[index];
			const scSpecRecord& s2 = (*this)[index+1];
			if ( (*this)[index].spec() == (*this)[index+1].spec() )
				printit = true;
			if ( (*this)[index].offset() >= (*this)[index+1].offset() )
				printit = true;
		}

		if ( printit )
			PrintRun( str );
	}
#endif

	for ( int i = 0; i < NumItems() - 1; i++ ) {
		const scSpecRecord& s1 = (*this)[i];
		const scSpecRecord& s2 = (*this)[i+1];
		scAssert( (*this)[i].spec() != (*this)[i+1].spec() );
		scAssert( (*this)[i].offset() < (*this)[i+1].offset() );
		foo = i;
	}
}

#endif

/* ==================================================================== */
void scSpecRun::PrintRun( const char* info ) const
{
	SCDebugTrace( 0, "RUN: \"%s\"\n", info );
	
	for ( int i = 0; i < NumItems(); i++ )
		SCDebugTrace( 0, "\t%d\t%11d\t0x%08x\n", i, (*this)[i].offset(), (*this)[i].spec().ptr() );
}

/* ==================================================================== */
void scSpecRun::consolidate( int outer )
{
	for ( int i = 0; i < NumItems() - 1;  ) {
		const scSpecRecord& s1 = (*this)[i];
		const scSpecRecord& s2 = (*this)[i+1];
		if ( (*this)[i].spec() == (*this)[i+1].spec() )
			Remove( i + 1 );
		else if ( (*this)[i].offset() >= (*this)[i+1].offset() ) {
			if ( outer )
				Remove( i + 1 );
			else
				Remove( i );
		}
		else
			i++;
	}
}

/* ==================================================================== */
void scSpecRun::Read( APPCtxPtr ctxPtr, IOFuncPtr readFunc )
{
	uchar			sbuf[8];
	const uchar*	pbuf;
	long			numspecs;
	
	ReadLong( numspecs, ctxPtr, readFunc, kIntelOrder );
	
	SetNumSlots( numspecs + 1 );

	for ( int i = 0; i < numspecs; i++ ) {
		ReadBytes( sbuf, ctxPtr, readFunc, 8 );

		pbuf = sbuf;
			
		ulong specid;
		pbuf = BufGet_long( pbuf, specid, kIntelOrder );

		ulong offset;
		pbuf = BufGet_long( pbuf, offset, kIntelOrder );

		TypeSpec spec( APPDiskIDToPointer( ctxPtr, specid, diskidTypespec ) );

		scSpecRecord rec( spec, offset );

		Insert( i, rec ); 
	}

	consolidate();

	DebugRun( "scSpecRun::Read" );
}

/* ==================================================================== */
void scSpecRun::Write( APPCtxPtr	ctxPtr,
					   IOFuncPtr	writeFunc )
{
	int 			i;	
	uchar			sbuf[8];
	uchar*			pbuf;

		// do not write out the terminator
	WriteLong( NumItems() - 1, ctxPtr, writeFunc, kIntelOrder );

	for ( i = 0; i < NumItems(); i++ ) {
		pbuf = sbuf;

		if ( !(*this)[i].isTerminator() ) {
			long	diskid = APPPointerToDiskID( ctxPtr,
												 (*this)[i].spec().ptr(),
												 diskidTypespec );
			pbuf = BufSet_long( pbuf, diskid, kIntelOrder );
			pbuf = BufSet_long( pbuf, (*this)[i].offset(), kIntelOrder );				
			
			long size = (*writeFunc)( ctxPtr, sbuf, 8 );
			raise_if( size != 8, scERRfile );
		}
	}

}

/* ==================================================================== */
void scSpecRecord::restorePointer()
{
}

/* ==================================================================== */
void scSpecRun::RestorePointers( void )
{
	for ( int i = 0; i < NumItems(); i++ )
		(*this)[i].restorePointer();
}

/* ==================================================================== */
long scSpecRun::ExternalSize( void ) const
{
	return sizeof( scSpecRecord ) * NumItems() + sizeof( int32 );
}	

/* ==================================================================== */
void scSpecRun::SetContentSize( int32 size )
{
	int index = NumItems() - 2;

	//PrintRun( "SetContentSize" );

	while ( index > 0 && size <= (*this)[index].offset() )
		Remove( index-- );

	//PrintRun( "SetContentSize" );

	DebugRun( "SetContentSize" );
}
