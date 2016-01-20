/****************************************************************************

	File:		SCSTREAM.C

	$Header: /Projects/Toolbox/ct/SCSTREAM.CPP 2	 5/30/97 8:45a Wmanis $

	Contains:	scStream impelemention.

	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

****************************************************************************/

#include "scstream.h"
#include "scpubobj.h"			// need this for the call backs
#include "scapptex.h"
#include "sccolumn.h"
#include "scglobda.h"
#include "scmem.h"
#include "scparagr.h"
#include "scselect.h"
#include "scexcept.h"
#include "screfdat.h"
#include "sccallbk.h"
#include "sctextli.h"
#include "scset.h"


#ifdef SCMACINTOSH
	#ifdef THINK_CPLUS
		#include <pascal.h>
	#else
		#include <Strings.h>
	#endif
#endif

/* ==================================================================== */
/* ==================================================================== */
/* ==================================================================== */


scTicks scReformatTimeSlice 		= LONG_MAX;
scTicks scInteractiveTimeSlice		= LONG_MAX;



#if SCDEBUG > 1
	inline void STRVerifyCount( scContUnit* para )
	{
		long count = 0;
		
		for ( ; para; para = para->GetNext(), count++ ) {
			scAssert( count == para->GetCount() );
		}
		
	}
#endif

/* ==================================================================== */
/* build up a list of columns that have been marked to be repainted */

static void BuildRepaintList( scColumn* col )
{
//	SCDebugTrace( 2, scString( "BuildRepaintList - in\n" ) );
	for ( col = (scColumn*)col->FirstInChain(); col; col = col->GetNext() ) {
		scAssert( !col->Marked( scLAYACTIVE ) );
//		scAssert( !col->Marked( scREPAINT ) );
	}
}

/* ==================================================================== */
/* check the position of the last line of paraH and the firstline of 
 * nextParaH, if they are not positioned correctly (e.g. nextParaH first 
 * line is not where it is supposed to be) mark nextParaH to be 
 * rebroken so that it will be properly positioned
 */
 
static Bool CheckPositionAndMark( scContUnit*		para,		/* the para we just reformatted */
								  scContUnit*		nextPara,
								  const scFlowDir&	fd )	/* the next para we may have to rebreak */
{
	scTextline* lastTxl;	/* last line of paraH */
	scTextline* nextTxl;	/* first line of nextParaH */
	MicroPoint	nextBaseln; /* 'ideal' baseline of nextTxlH */

	if ( nextPara ) {
		if ( !nextPara->Marked( scREBREAK ) ) {
			lastTxl = para->GetLastline();
			
			if ( lastTxl ) {
				nextTxl = LNNext( lastTxl );
				if ( nextTxl ) {
						// determine what the baseline should be
					scLEADRefData	ld;
					MicroPoint	paraLead = lastTxl->ParaLead( ld, fd );

					if ( !fd.IsVertical() )
						nextBaseln = lastTxl->GetBaseline() + paraLead;
					else 
						nextBaseln = lastTxl->GetBaseline() - paraLead; 				

						// compare it with reality and mark it if it is not correct
					if ( nextBaseln != nextTxl->GetBaseline() || nextTxl->GetPara() != nextPara )
						nextPara->Mark( scREBREAK );
				}
				else if ( !nextTxl )		/* no line - so mark rebreak */
					nextPara->Mark( scREBREAK );
			}
		}
		else if ( !para->GetLastline() ) {
				// if there is no last line we should unmark this para
			nextPara->Unmark( scREBREAK );
			return false;
		}
	}
	return true;
}

/* ==================================================================== */
/* Delete last paragraph of previous column to keep it with first		*/
/* paragraph of this column.											*/

static Bool STRResetKeepWNext( scContUnit*& 	para,
							   scContUnit*& 	prevPara,
							   scContUnit*& 	nextPara,
							   scColumn*&		col,
							   scCOLRefData&	colRefData )
{
	scColumn*	prevCol;
	scColumn*	newStartCol;
	scTextline* prevLine;
	scTextline* firstKeepLine;
	scTextline* curLine = para->GetFirstline();

	if ( curLine && prevPara && prevPara->Marked( scKEEPNEXT ) ) {

		firstKeepLine	= prevPara->GetFirstline();

		if ( firstKeepLine 
				&& (prevCol=firstKeepLine->GetColumn())!=curLine->GetColumn()) {
			para->Mark(scREBREAK );
			prevPara->Mark( scREBREAK );

				/* colH not necessarily COLNext( prevColH )!! */
			newStartCol = prevCol->GetNext();	

			prevLine	= firstKeepLine->GetPrev();
			while ( prevCol != col ) {
				prevCol->DeleteExcessLines( NULL, prevLine, false, colRefData );
					/* should be null for subsequent columns */ 			
				prevLine	= NULL; 
				prevCol 	= prevCol->GetNext();
			};
			colRefData.COLFini( true );
			col->DeleteExcessLines( NULL, NULL, false, colRefData );

			nextPara	= para;
			para		= prevPara;
			prevPara	= para->GetPrev( );

			col 	= newStartCol;
			colRefData.COLInit( col, para );

			return true;
		}
	}

	return false;
}

/* ==================================================================== */

eRefEvent STRIncReformat( scContUnit*	p,
						  scTicks		ticksAllowed,
						  scRedispList* redispList )
{
	eRefEvent refEvent = STRReformat( 0, p, ticksAllowed, redispList );
	
//	if ( refEvent != eNoReformat ) 
//		 SLCUpdateSelection( PARAFirstInChain( p ) );
	
	return refEvent;
}

/* ==================================================================== */
/* high level reformatting - walk down the stream looking at who has
 * to be reformatted 
 */

eRefEvent STRReformat( scColumn*		theColH,	// column to attempt to start reformatting in
					   scContUnit*		para,		// paragraph to start reformatting with
					   scTicks			ticksAllowed,
					   scRedispList*	redisp )
{
	try {
		scColumn*		col 		= NULL; /* the col we will format into */
		scContUnit* 	nextPara;
		scContUnit* 	prevPara;
		scTextline* 	lastTxlH;
		scCOLRefData	colRefData( redisp );
		PrevParaData	prevParaData;
		int 			keepWNextControl	= 0;
		Bool			overflow;	
		scTicks 		startTick			= SCTickCount();
		int 			parasProcessed		= 0;
		int 			timeout 			= 0;
		TypeSpec		nullSpec;
		
	#if SCDEBUG > 1
		static int		reformatEvent;
	#endif

		colRefData.fSavedLineState.LineListInit();

		prevParaData.Init();

		if ( !theColH ) {
			if ( !para )				// Can't reformat into nothing
				return eNoReformat;
			theColH = scColumn::FindFlowset( para->GetStream() );
			if ( !theColH ) 			// Can't reformat into nothing
				return eNoReformat;
		}

		
		if ( !theColH->GetRecomposition() ) {
			theColH->Mark( scINVALID );
			return eNoReformat;
		}

		if ( !para )
			return eNoReformat; 		// can't reformat with nothing

		
	#if SCDEBUG > 1 
		STRVerifyCount( (scContUnit*)para->FirstInChain() );
	#endif

		try {
			colRefData.COLInit( theColH, para );

		
			prevPara = para->GetPrev( );
			nextPara = para->GetNext( );

			for ( overflow = false; !overflow && para && !timeout;	) {
					// don't check time out until we have processed at
					// least on paragragh
				scTicks currentTick = SCTickCount();
			
				timeout = ( currentTick - startTick > ticksAllowed );
				if ( timeout )
					SCDebugTrace( 2, scString( "currentTick %u startTick %u ticksAllowed %u\n" ), 
								  currentTick, startTick, ticksAllowed );
			
				if ( para->Marked( scRETABULATE ) )
					para->Retabulate( nullSpec );
				
				if ( para->Marked( scREBREAK ) ) {
					if ( para->GetFirstline() ) {
							// we know about where to start
						if ( col == NULL ) {
							col = para->GetFirstCol();
							if ( prevPara == NULL )
								col = (scColumn*)col->FirstInChain();
						}
						scAssert( col != 0 );
					}
					else if ( col == NULL ) {
							// the paragraph does not have a column
							// associated with it, we can associate the 
							// the column passed in with 
						col = theColH;
					}

					if ( col != colRefData.GetActive() ) {
						if ( colRefData.GetActive() != NULL )
							colRefData.COLFini( true );
						if ( colRefData.COLInit( col, para ) )
							continue;
					}

	#if SCDEBUG > 1
					eRefEvent revnt = para->Reformat( colRefData, prevParaData, keepWNextControl, reformatEvent );
	#else
					eRefEvent revnt = para->Reformat( colRefData, prevParaData, keepWNextControl );
	#endif
					col = colRefData.GetActive();
				
					keepWNextControl = 0;

					switch ( revnt ) {
						case eNormalReformat:
							scAssert( !para->Marked( scREBREAK ) );
							SCDebugTrace( 2, scString( "eNormalReformat %d %u\n" ),
										  para->GetCount(), para->Marked( scREBREAK ) );
							break;

						
						case eNoReformat:
							SCDebugTrace( 2, scString( "eNoReformat\n" ) );
							overflow = true;
							break;

						case eOverflowGeometry: 		// more text than columns
						case eOverflowContent:			// more columns than text
							SCDebugTrace( 2, scString( "eOverflowxxxxx\n" ) );
							colRefData.COLFini( false );
							gStreamChangeInfo.Set( 0, 0, 0, 0 );
							gbrS.Init();
							return revnt;
					}

				}
				else
					prevParaData.Init();
				 
				if ( !CheckPositionAndMark( para, nextPara, colRefData.GetActive()->GetFlowdir() ) )
					overflow = true;

				if ( STRResetKeepWNext( para, prevPara, nextPara, col, colRefData ) ) {
					keepWNextControl = 1;
					prevParaData.Init();
					continue;
				}

				para->scAssertValid();

					// We need to preserve final value of para
				if ( nextPara == NULL ) 
					break;

				prevPara	= para;
				para		= nextPara;
				nextPara	= para->GetNext( );


				parasProcessed++;

					// check to see if we have any more time
				ticksAllowed += APPEventAvail( scReformatProc );
			}


			colRefData.COLFini( true );

				// This is why we need to preserve final value of para in above loop
				// Delete lines that used to be associated with stream.
			if ( !para->GetNext() && !para->Marked( scRETABULATE | scREBREAK ) ) {
				lastTxlH = para->GetLastline();
				if ( lastTxlH != NULL ) {
					col = lastTxlH->GetColumn();
					if ( col != NULL ) {
						for ( col = col->GetNext(); col; col = col->GetNext() ) {
							scXRect lineDamage;
							col->FreeLines( true, lineDamage );
							col->Unmark( scINVALID );
							if ( redisp )
								redisp->AddColumn( col, lineDamage );
						}
					}
				}
			}
		}
		catch( ... ) {
			if ( colRefData.GetActive() )
				colRefData.COLFini( true );
			gbrS.Init();
			throw;
		} 

			// add all columns marked for repaint to repaint structure
		BuildRepaintList( theColH );

	#if 1
		static int doit = 0;
		
		if ( doit ) {
			startTick = SCTickCount() - startTick;
			SCDebugTrace( 2, scString( "Ticks %lu" ), startTick );
		}
	#endif

			// clear information about changes to the stream for redisplay 
			// purposes
		gStreamChangeInfo.Set( 0, 0, 0, 0 );
		gbrS.Init();
	}
	catch( ... ) {
		throw( scERRmemReformat );
	}
	return eNormalReformat;
}

/* ==================================================================== */
/* search thru all the columns and find who has this and reset the
 * stream
 */
 
void scStream::ResetStream( scStream* newStr )
{
	scColumn* col;

	for ( col = scColumn::GetBaseContextList( ); col; col = col->GetContext() ) {
		if ( col->GetStream() == this )
			col->SetFlowsetStream( newStr );
		
	}
}

/* ==================================================================== */
/* enumerate a stream, prepare to write to file */

void scStream::DeepEnumerate( long& objEnumerate )
{
	scContUnit* para = First();

	for ( ; para; para = para->GetNext( ) )
		para->Enumerate( objEnumerate );
}

/* ==================================================================== */
/* return the first paragraph in a stream */

scContUnit* scStream::First( ) const
{
	const scContUnit* prevPara	= NULL;
	const scContUnit* p 		= this;

	for ( ; (prevPara = p->GetPrev( )) != NULL; p = prevPara )
		;
	return (scContUnit*)p;
}

/* ==================================================================== */

scContUnit* scStream::Last( void ) const
{
	const scContUnit* nextPara = NULL;
	const scContUnit* p 		= this;

	for ( ; (nextPara = p->GetNext( )) != NULL; p = nextPara )
		;
	return (scContUnit*)p;
}

/* ==================================================================== */
/* determine if stream contains the following paragraph */

Bool scStream::Contains( scContUnit* para )
{
	scContUnit* testPara;

	for ( testPara = this; testPara; testPara = testPara->GetNext( ) ) {
		if ( testPara == para )
			return true;
	}
	return false;
}

/* ==================================================================== */
/* copy a stream */

void scStream::STRCopy( scStream*& dst ) const
{
	scContUnit* srcPara = First();
	scContUnit* dstPara;

	try {
		dst 	= 0;
		dstPara = 0;
		for ( ; srcPara; srcPara = srcPara->GetNext( ) ) {
			dstPara = srcPara->Copy( dstPara );
			if ( dst == NULL )
				dst = dstPara->GetStream();
		}
	} catch( ... ) {
		if ( dst )
			dst->STRFree(), dst = 0;
		throw;
	} 
}

/* ==================================================================== */
/* copy a stream to application text */

void scStream::CopyAPPText( stTextImportExport& appText )

{
	scContUnit* para;
	
	appText.reset();

	for ( para = First(); para; para = para->GetNext( ) )
		para->WriteAPPText( appText );
}

/* ==================================================================== */
/* mark all the members of a stream with the passed in mark */

void scStream::STRMark( const scLogBits& mark )
{
	scContUnit* para = First();

	for ( ; para; para = para->GetNext() )
		para->Mark( mark );
}


/* ==================================================================== */
/* in a stream find the nth paragraph */

scContUnit *scStream::NthPara( long count )
{
	scContUnit * para = First();

	if ( count < 0 )
		return NULL;
		
	for ( ; para && count--; para = para->GetNext( ) )
		;
	return para;
}


/* ==================================================================== */

void scStream::STRRestorePointers( scSet* enumTable )
{
	scContUnit* p = First();

	for ( ; p; p = p->GetNext() )
		p->RestorePointers( enumTable );

}

/* ==================================================================== */

void scStream::STRZeroEnumeration( void )
{
	scContUnit* p = First();

	for ( ; p; p = p->GetNext() )
		p->ZeroEnum( );
}

/* ==================================================================== */
/* write a stream to file */

void scStream::STRToFile( APPCtxPtr ctxPtr,
						  IOFuncPtr 	writeFunc )
{
	scContUnit	*p;
	
	for ( p = First(); p; p = p->GetNext( ) )
		p->StartWrite( ctxPtr, writeFunc );
}

/* ==================================================================== */
/* write str to scrap */

void scStream::STRWriteToFile( APPCtxPtr	ctxPtr,
							   IOFuncPtr	writeFunc )
{	
	long objEnumerate = 0;
	DeepEnumerate( objEnumerate );
	STRToFile( ctxPtr, writeFunc );
}

/* ==================================================================== */
/* get stream from file */

scStream* scStream::STRFromFile( scSet* 		eTable,
								 APPCtxPtr		ctxP,
								 IOFuncPtr		readFunc )
{
	scStream* volatile	stream	= 0;
	scTBObj*			obj 	= NULL;

	try {
		while ( ( obj = scTBObj::StartRead( eTable, ctxP, readFunc ) ) != 0 ) {
			if ( stream == NULL )
				stream = (scStream *)obj;

				// even though the pointers have not been restored we
				// may assume that a 0 value in a pointer equals a null
				// pointer and non 0 value equals a valid pointer
			
			if ( !obj->Next() )
				break;
		}
	} catch( ... ) {
		eTable->DeleteAll();
		throw;
	} 

	return stream;
}

/* ==================================================================== */
/* write to  a textfile */

void scStream::STRWriteTextFile( APPCtxPtr	ctxPtr,
								 IOFuncPtr	writeFunc,
								 Bool		addDcr )/* add double carriage return */
{
	scContUnit* para;

	for ( para = First(); para; para = para->GetNext( ) ) {
		if ( para->GetNext( ) )
			para->WriteStream( true, ctxPtr, writeFunc );
		else
			para->WriteStream( addDcr, ctxPtr, writeFunc );
	}
}

/* ==================================================================== */
/* read from a text file and build into a paragraph */

scStream* scStream::ReadTextFile( TypeSpec	spec,
								  APPCtxPtr ctxPtr,
								  IOFuncPtr readFunc,
								  long		count )
{
	scContUnit* volatile para	= 0;
	scStream*	volatile stream = 0;

	try {
		para = scContUnit::Allocate( spec, NULL, count++ );

		stream = (scStream*)para;
		
		while ( para->ReadStream( ctxPtr, readFunc ) > 0 ) {
			para = scContUnit::Allocate( spec, para, count++ );
		}

	} catch( ... ) {
		if ( stream )
			stream->STRFree( ), stream = 0;
		throw;
	} 
	return stream;
}

/* ==================================================================== */
/* read from a text file and build into a stream */

scStream* scStream::ReadAPPText( stTextImportExport& appText )
{
	scStream*	stream		= 0;
	scContUnit* p			= 0;
	long		count		= 0L;

	try {
		TypeSpec pspec;
		while ( appText.NextPara( pspec ) >= 0 ) {
			p = scContUnit::Allocate( pspec, p, count++ );
			if ( !stream )
				stream = (scStream*)p;
			p->ReadAPPText( appText ) ;
		}
	} catch( ... ) {
		stream->STRFree(), stream = 0;
		throw;
	} 
	return stream;
}

/* ==================================================================== */
/* this stream seems to have no column associated with it, more than likely
 * it is some text that has not been formatted into a column,
 */

Bool scStream::FindColumn( scColumn*& foundCol )
{
	scColumn*	col;

	for ( col = scColumn::GetBaseContextList( ); col; col = col->GetContext() ) {
		if ( col->GetStream() == this ) {
			foundCol = col;
			return true;
		}
	}
	return false;
}

/* ==================================================================== */
/* free all paragraphs in a stream */

void scStream::STRFree()
{
	scContUnit* para = First();
	scContUnit* nextPara;

	for ( ; para; para = nextPara ) {
		nextPara = para->GetNext();
		para->Free( );
	}
}

/* ==================================================================== */
/* split a stream into two streams at streamID2 */

scStream* scStream::Split( scContUnit* cu )
{
	scContUnit* prevPara;
	scContUnit* para;

	/* insure that they are in the same stream */
	raise_if( this != (scContUnit *)cu->FirstInChain( ), scERRlogical );

	/* make the split */
	prevPara = cu->GetPrev();
	prevPara->SetNext( NULL );
	cu->SetPrev( NULL );

	/* walk down the second half and delete any existing lines */
	for ( para = cu; para; para = para->GetNext( ) )
		para->Deformat( );
	
	/* renumber the streams */
	Renumber();
	cu->Renumber();
	return (scStream*)cu;
}

/* ==================================================================== */
/* build a list of typespecs that are contained in the stream
 */

void scStream::STRGetTSList( scTypeSpecList& tsList )
{
	scContUnit* para = First();

	for ( ; para; para = para->GetNext( ) )
		para->GetTSList( tsList );
}

/* ==================================================================== */
/* build a list of default paragraph typespecs that are contained in the stream
 */

void scStream::GetParaTSList( scTypeSpecList& tsList )
{
	scContUnit* para = First();

	for ( ; para; para = para->GetNext( ) )
	{
		TypeSpec ts = para->GetDefaultSpec();
		tsList.Insert( ts );
	}
}

/* ==================================================================== */
/* get style information about the stream, return in CharSpecListHandle */

void scStream::GetCharSpecList( scSpecLocList&	csList )
{
	csList.SetStream( this );
	scContUnit* para = First();

	for ( ; para; para = para->GetNext( ) )
		para->OffsetGetCharSpecList( 0, LONG_MAX, csList );
}

/* ==================================================================== */
/* count the characters in the stream */

void scStream::ChCount( long& count )
{
	scContUnit* para = First();

	count = 0;
	for ( ; para; para = para->GetNext( ) )
		count += PARAChSize( para );
}

/* ==================================================================== */
/* the following are used to read and write characters to memory, typically
 * for clip board support 
 */
/* ==================================================================== */

struct MemFileStruct {	
						MemFileStruct( SystemMemoryObject* sysmem ) :
							fMagic( 0xdababada ),
							fStartMem( 0 ),
							fDstPtr( 0 ),
							fSrcPtr( 0 ),
							fTotalSize( 0 ),
							fBytesWritten( 0 ),
							fTheSysMemObj( sysmem ){}

	ulong				fMagic;
	scChar* 			fStartMem;
	scChar* 			fDstPtr;
	scChar* 			fSrcPtr;
	long				fTotalSize,
						fBytesWritten;
	SystemMemoryObject* fTheSysMemObj;
};

static long MemRead( APPCtxPtr	ctxPtr, 
					   void*		dstPtrV,
					   long 	size )
{
	MemFileStruct*	mfs = (MemFileStruct*)ctxPtr;
	
	long	startSize	= size;
	scChar* dstPtr		= (scChar*)dstPtrV;

	for ( ; size && *mfs->fSrcPtr ; size-- )
		*dstPtr++ = *mfs->fSrcPtr++;
	
	return ( startSize - size );
}

/* ==================================================================== */


static long MemWrite( APPCtxPtr ctxPtr,
						void*		ptr,
						long		size )
{
	MemFileStruct*	mfs = (MemFileStruct*)ctxPtr;	


	if ( ( mfs->fBytesWritten + size ) > mfs->fTotalSize ) {
		
		mfs->fTheSysMemObj->UnlockHandle();
		
		status stat = mfs->fTheSysMemObj->SetHandleSize( mfs->fBytesWritten + size );
		if ( stat != scSuccess )
			return 0;
			
		mfs->fStartMem = (scChar*)mfs->fTheSysMemObj->LockHandle( );
		
		mfs->fTotalSize = mfs->fBytesWritten + size;
		mfs->fDstPtr	= mfs->fStartMem + mfs->fBytesWritten;
	}

	SCmemmove( mfs->fDstPtr, ptr, size );
	
	mfs->fDstPtr += size;
	
	mfs->fBytesWritten += size;
	
	scAssert( (long)(mfs->fDstPtr - mfs->fStartMem) <= mfs->fTotalSize );
	return size;
}


/* ==================================================================== */

void scStream::STRWriteMemText( Bool				addDcr,
								SystemMemoryObject& sysConPBlock )
{
	scContUnit* 	para;
	MemFileStruct	mfs( &sysConPBlock );
	
	for ( mfs.fTotalSize = 0, para = First(); para; para = para->GetNext( ) )
		mfs.fTotalSize += ( PARAChSize( para ) + 1 );	// for carriage returns
	mfs.fTotalSize++;									// NULL terminator

	sysConPBlock.SetHandleSize( mfs.fTotalSize );
	
	mfs.fBytesWritten	= 0;
	mfs.fDstPtr 		= mfs.fStartMem = (scChar*)sysConPBlock.LockHandle( );

	STRWriteTextFile( (APPCtxPtr)&mfs, MemWrite, addDcr );

	MemWrite( (APPCtxPtr)&mfs, "", 1 ); 	/* NULL terminate */

	sysConPBlock.UnlockHandle( );
}

/* ==================================================================== */

scStream* scStream::ReadMemText( TypeSpec		spec,
								 const scChar*	ptr )
{
	MemFileStruct	mfs( 0 );
	
	mfs.fBytesWritten	= 0;
	mfs.fSrcPtr 		= mfs.fStartMem = (scChar*)ptr;

	scStream* stream = scStream::ReadTextFile( spec, (APPCtxPtr)&mfs, MemRead, 0 );
	return stream;
}

/* ==================================================================== */
/* return the last paragraph with a line in a stream */

scContUnit* scStream::LastVisiblePara(	)
{
	scContUnit* para = First();
	scContUnit* lastParaWithLine = NULL;

	for ( ; para->GetFirstline(); para = para->GetNext( ) )
		lastParaWithLine = para;

	return lastParaWithLine;
}


/* ==================================================================== */

void scStream::Iter( SubstituteFunc func,
					 scRedispList*	damage )
{
	scContUnit* para = First();
	long		endOffset;
	
	for ( ; para; para = para->GetNext( ) ) {
		endOffset = PARAChSize( para );
		para->Iter( func, 0, endOffset );
	}
	
	STRReformat( NULL, this, scReformatTimeSlice, damage );
}

/* ==================================================================== */

scTextline* scStream::LastVisibleLine( )
{
	scContUnit* para = LastVisiblePara();
	return para ? para->GetLastVisibleLine() : 0;
}

/* ==================================================================== */

Bool scStream::Compare( const scStream* str2 ) const
{
	scContUnit* p1;
	scContUnit* p2;
				
	p1	= First();
	p2	= str2->First();
	
	for ( ; p1 && p2; p1 = p1->GetNext( ), p2 = p2->GetNext( ) ) {
		if ( *p1 != *p2 )
			return false;
	}
	
	return ( !p1 && !p2 ) ? true : false;
}

/* ==================================================================== */
// remove all layout information from the stream

void scStream::STRDeformat()
{
	scContUnit* p = First();
	
	for ( ; p; p = p->GetNext( ) )
		p->Deformat( );
}

/* ==================================================================== */

#if SCDEBUG > 1

/* ==================================================================== */

long scStream::STRDebugSize( void )
{
	scContUnit* p;
	long		psize = 0;

	for ( p = First(); p; p = p->GetNext( ) )
		psize += ( sizeof( scContUnit ) 	+ 
				 ( p->GetContentSize() * sizeof( CharRecord ) ) + 
				 ( p->GetSpecRun().NumItems() * sizeof( scSpecRecord ) ) );
	
	return psize;
}

/* ==================================================================== */

void scStream::STRDbgPrintInfo( void )
{
	scContUnit* p;

	for ( p = First(); p; p = p->GetNext( ) )
		p->DbgPrintInfo();
}

/* ==================================================================== */
	
#endif

/* ==================================================================== */

void scStream::GetParaSpecList( scSpecLocList& csList )
{
	for ( scContUnit* p = First(); p; p = p->GetNext() ) {
		scSpecLocation specLoc( p->GetCount(), -1, p->GetDefaultSpec() );
		csList.Append( specLoc );
	}
}

/* ==================================================================== */

void scStream::SetParaSpecList( const scSpecLocList&	csList,
								scRedispList*	redispList )
{
	scColumn*		firstCol	= 0;
	scContUnit* 	firstPara	= 0;	
	
	for ( int i = 0; i < csList.NumItems(); i++ ) {
		if ( csList[i].spec().ptr() ) {
			scContUnit* p	= NthPara( csList[i].offset().fParaOffset );
			if ( !firstPara )
				firstPara = p;
			if ( !firstCol && p->GetFirstline() )
				firstCol = p->GetFirstline()->GetColumn();

			TypeSpec ts = csList[i].spec();
			p->SetDefaultSpec( ts );
		}
	}

	STRReformat( firstCol, firstPara, scReformatTimeSlice, redispList );

	if ( firstCol ) {
		scSelection* select = firstCol->FlowsetGetSelection();
		if ( select )
			select->UpdateSelection();
	}
}

/* ==================================================================== */

void scStream::SetCharSpecList( const scSpecLocList&	csList,
								scRedispList*	redispList )
{
	scColumn*		firstCol	= 0;
	scContUnit* 	firstPara	= 0;	
	
	for ( int i = 0; i < csList.NumItems(); i++ ) {
		if ( csList[i].spec().ptr() ) {
			scContUnit* p	= NthPara( csList[i].offset().fParaOffset );
			
			if ( !firstPara )
				firstPara = p;
			if ( !firstCol && p->GetFirstline() )
				firstCol = p->GetFirstline()->GetColumn();
			p->SetStyle( csList[i].offset().fCharOffset, 
						 csList[i+1].offset().fCharOffset, 
						 csList[i].spec(), false, true );
		}
	}

	STRReformat( firstCol, firstPara, scReformatTimeSlice, redispList );

	if ( firstCol ) {
		scSelection* select = firstCol->FlowsetGetSelection();
		if ( select )
			select->UpdateSelection();
	}
}

/* ==================================================================== */

void scStream::RemoveEmptyTrailingParas( scColumn* flowset )
{
	scContUnit* p = Last();
	scContUnit* prevP;
	scXRect 	xrect;

	while ( p && !p->GetContentSize() ) {
		prevP = p->GetPrev();
		if ( p->GetFirstline() )
			p->GetFirstline()->Delete( xrect );
		
		p->Unlink( );
		p->Free( flowset->FlowsetGetSelection() );
		p = prevP;
	}
	if ( !p )
		flowset->SetFlowsetStream( 0 );
}

/* ==================================================================== */
