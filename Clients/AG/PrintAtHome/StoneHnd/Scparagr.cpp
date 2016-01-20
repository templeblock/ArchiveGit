/************************************************************************
Contains:	content unit code.
***************************************************************************/

#include "scparagr.h"
#include "sccolumn.h"
#include "scbreak.h"
#include "scstcach.h"
#include "scglobda.h"
#include "scctype.h"
#include "scmem.h"
#include "scset.h"
#include "scspcrec.h"
#include "scstream.h"
#include "sctextli.h"
#include "scfileio.h"
#include "scpubobj.h"
#include "sccallbk.h"
#include "scapptex.h"

#ifdef _RUBI_SUPPORT
	#include "scrubi.h"
#endif _RUBI_SUPPORT

/* ==================================================================== */
void scContUnit::Read( scSet*		enumTable,
					   APPCtxPtr	ctxPtr,
					   IOFuncPtr	readFunc )
{
	scTBObj::Read( enumTable, ctxPtr, readFunc );

	ReadLong( fParaCount, ctxPtr, readFunc, kIntelOrder );
	
	fCharArray.Read( ctxPtr, readFunc );
	
	fSpecRun.Read( ctxPtr, readFunc );

	long diskid;
	ReadLong( diskid, ctxPtr, readFunc, kIntelOrder );
	
	TypeSpec spec( APPDiskIDToPointer( ctxPtr, diskid, diskidTypespec ) );
	defspec_ = spec;

#ifdef _RUBI_SUPPORT
	long rubipresent;
	ReadLong( rubipresent, ctxPtr, readFunc, kIntelOrder );
	if ( rubipresent ) {
		AllocRubiArray();
		fRubiArray->Read( ctxPtr, readFunc, rubipresent );
	}
#endif _RUBI_SUPPORT
		
	Mark( scRETABULATE );
	Mark( scREBREAK );
}

/* ==================================================================== */
void scContUnit::Write( APPCtxPtr	ctxPtr,
						IOFuncPtr	writeFunc )
{
	scTBObj::Write( ctxPtr, writeFunc );

	WriteLong( fParaCount, ctxPtr, writeFunc, kIntelOrder );
	
		// the characters 
	fCharArray.Write( ctxPtr, writeFunc );

		// the spec runs
	fSpecRun.Write( ctxPtr, writeFunc );

	WriteLong( APPPointerToDiskID( ctxPtr, defspec_.ptr(), diskidTypespec ),
			   ctxPtr,
			   writeFunc,
			   kIntelOrder );

#ifdef _RUBI_SUPPORT
	WriteLong( (ulong)fRubiArray ? 1 : 0, ctxPtr, writeFunc, kIntelOrder ); 	
	if ( fRubiArray )
		fRubiArray->Write( ctxPtr, writeFunc );
#endif _RUBI_SUPPORT
}

/* ==================================================================== */	
void scContUnit::RestorePointers( scSet* enumTable )
{
	if ( !Marked( scPTRRESTORED ) ) {
		scTBObj::RestorePointers( enumTable );
		fSpecRun.RestorePointers( );
	}
}

/* ==================================================================== */	
void scContUnit::CopySpecRun( const scSpecRun& spr )
{
	fSpecRun = spr;
}

/* ==================================================================== */
#ifdef _RUBI_SUPPORT
	
void scContUnit::AllocRubiArray( void )
{
	fRubiArray = new scRubiArray; 
}

/* ==================================================================== */
void scContUnit::AllocRubiArray( const scRubiArray& ra )
{
	fRubiArray = new scRubiArray;
	*fRubiArray = ra;
}

/* ==================================================================== */
void scContUnit::DeleteRubiArray( void )
{
	delete fRubiArray;
	fRubiArray = NULL;
}

/* ==================================================================== */	
BOOL scContUnit::IsRubiPresent( size_t start, size_t end )
{
	if ( !fRubiArray )
		return false;
		
	return fRubiArray->IsRubiData( start, end );
}

/* ==================================================================== */
#endif _RUBI_SUPPORT

scColumn* scContUnit::GetFirstCol( void ) const
{
	return fFirstline ? fFirstline->GetColumn() : 0; 
}

/* ==================================================================== */
// mark the paragraph
void scContUnit::Mark( const scLogBits& bits )
{
	scTBObj::Mark( bits );

	SCDebugTrace( 1, scString( "scContUnit::Mark 0x%08x %d\n" ), this, GetCount() );

	if ( bits.fRetabulate || bits.fRebreak || bits.fReposition ) {
		if ( fFirstline ) {
			scColumn* col = fFirstline->GetColumn();
			col->Mark( scINVALID );
		}	
	}
}

/* ==================================================================== */
// do this paragraph and this column intersect
BOOL scContUnit::ColSect( const scColumn* col ) const
{
	const scTextline *l;

		// if we use this maybe this method should be moved over to column
	for ( l = col->GetFirstline(); l; l = l->GetNext() ) {
		if ( l->GetPara() == this )
			return true;
	}
	return false;
}

/* ==================================================================== */
// does this para contain this spec
BOOL scContUnit::ContainTS( TypeSpec ts )
{
	if ( ts == 0 )		// 0 being the short cut for all specs
		return true;
	else if ( ts == GetDefaultSpec() )
		return true;

	return fSpecRun.Includes( ts );
}

/* ==================================================================== */
scContUnit* scContUnit::CopyText( long offset1, long offset2 ) const
{
	TypeSpec defspec = defspec_;

	scContUnit* dstPara = scContUnit::Allocate( defspec );

	if ( offset2 < offset1 ) {
		long tmp;
		tmp 	= offset1;
		offset1 = offset2;
		offset2 = tmp;
	}

	offset2 = MIN( offset2, GetContentSize() );
	offset1 = MAX( 0, offset1 );

	long maxSpecRecs	= fSpecRun.NumItems() + dstPara->fSpecRun.NumItems();
	dstPara->fSpecRun.SetNumSlots( maxSpecRecs );
	
	long maxChars		= fCharArray.GetNumItems() + dstPara->GetCharArray().GetNumItems();
	dstPara->GetCharArray().SetNumSlots( maxChars );

		// copy specrun
	fSpecRun.Copy( dstPara->fSpecRun, offset1, offset2 );

		// copy the text
	fCharArray.Copy( dstPara->GetCharArray(), offset1, offset2 );

#ifdef _RUBI_SUPPORT
		// copy the rubi annotations
	if ( fRubiArray ) {
		dstPara->AllocRubiArray( *GetRubiArray() );
		
		dstPara->GetRubiArray()->DeleteRubiData( offset2, GetContentSize() );


		dstPara->GetRubiArray()->DeleteRubiData( 0, offset1 );

		if ( !dstPara->GetRubiArray()->GetNumItems() )
			dstPara->DeleteRubiArray();
	}	
#endif _RUBI_SUPPORT

	dstPara->GetCharArray().RepairText( dstPara->fSpecRun, 0L, dstPara->GetContentSize() );

//	dstPara->fSpecRun.SetContentSize( dstPara->GetContentSize() );

	return dstPara;
}

/* ==================================================================== */	
#if SCDEBUG > 1
void scContUnit::DbgPrintInfo( int debugLevel ) const
{
	SCDebugTrace( debugLevel, scString( "\nSCPARAGRAPH	- reformat event %d\n" ), fReformatEvent );
	SCDebugTrace( debugLevel, scString( "firstline 0x%08x count %d\n" ), fFirstline, fParaCount );
	SCDebugTrace( debugLevel, scString( "retabulate %u rebreak %u\n" ), fLogBits.fRetabulate, fLogBits.fRebreak );
	SCDebugTrace( debugLevel, scString( "reposition %u logactive %u\n" ), fLogBits.fReposition, fLogBits.fLogActive );
	SCDebugTrace( debugLevel, scString( "keepnext %u\n" ), fLogBits.fKeepNext );
}

/* ==================================================================== */
void scContUnit::scAssertValid( BOOL recurse ) const
{
	scTBObj::scAssertValid();

	scAssert( !Marked( scRETABULATE ) ); 
	scAssert( !Marked( scREBREAK ) ); 

	if ( fFirstline ) {
		fFirstline->scAssertValid( false );
		scAssert( fFirstline->GetStartOffset() == 0 );
		scAssert( fFirstline->GetEndOffset() <= fCharArray.GetContentSize() );
	}
}

/* ==================================================================== */
void scContUnit::DebugParaSpecs()
{
	SCDebugTrace( 0, scString( "\npara spec - 0x%08x %d\n" ), defspec_, GetContentSize() );
	fSpecRun.PrintRun( "para specs" );
}

#endif

/* ==================================================================== */
scContUnit::scContUnit() :
	fFirstline( 0 ),
	fParaCount( 0 )
{
}

/* ==================================================================== */
scContUnit::scContUnit( TypeSpec&	spec,
						scContUnit* prevPara,
						long		count ) :
								fFirstline( 0 ),
								fParaCount( 0 ),
								fSpecRun( spec ),
								defspec_( spec )
#ifdef _RUBI_SUPPORT
								, fRubiArray( 0 )
#endif _RUBI_SUPPORT
{
#if SCDEBUG > 1 
	fReformatEvent = 0;
#endif

	try {
		SetCount( count );

		Mark( scRETABULATE );
		
		if ( prevPara )
			prevPara->SetNext( this );
		SetPrev( prevPara );
	}
	catch( ... ) {
		delete this;
		throw;
	} 
}

/* ==================================================================== */
scContUnit::~scContUnit()
{
//	SCDebugTrace( 0, scString( "scContUnit::~scContUnit: 0x%08x\n" ), this );
}


/* ==================================================================== */
/* find the next line in a following column containing
 * lines of this paragraph
 */
scTextline* scContUnit::NextColumn( scColumn*& col )
{
	scTextline* txl = NULL;

		// check next column
	for ( ; col; col = col->GetNext() ) {
		if ( col )
			txl = col->GetFirstline( );

		if ( txl ) {
			if ( txl->GetPara( ) != this )
				txl = NULL;
			break;
		}
	}
	return txl;
}

/* ==================================================================== */
scTextline* scContUnit::FindLine( long offset ) const
{
	scTextline* txl  = fFirstline;

	for ( ; txl && txl->GetPara() == this; txl = txl->GetNextLogical() ) {
		if ( txl->OffsetOnLine( offset ) )
			return txl;
	}
	return NULL;
}

/* ==================================================================== */
/* append para2H to the stream containing para1H */
void scContUnit::Append( scContUnit* p2 )
{
	scContUnit* p1 = (scContUnit*)LastInChain();
	p1->SetNext( p2 );
	p2->SetPrev( p1 );
	Renumber();
}

/* ==================================================================== */
/* insert para2 into the stream following para1 */
void scContUnit::PostInsert( scContUnit *p2 )
{
	scContUnit *nextP;

	if ( p2 ) {
		nextP = GetNext( );
		SetNext( p2 );
		p2->SetPrev( this );

		scContUnit* last = (scContUnit*)p2->LastInChain();

		last->SetNext( nextP );
		if ( nextP )
			nextP->SetPrev( last );
		Renumber( );
	}
}

/* ==================================================================== */
scContUnit* scContUnit::PasteParas( const scContUnit*	srcPara,
									long&				offset )
{
	scContUnit* finalPara;
	scContUnit* newPara;
	scContUnit* firstPara = this;
	long		tmpOffset;

		// split the para
	finalPara = Split( offset );

	tmpOffset = GetContentSize();
	PasteText( srcPara, tmpOffset );

	srcPara = srcPara->GetNext( );

	for ( ; srcPara->GetNext( ); srcPara = srcPara->GetNext( ) ) {
		newPara = srcPara->CopyText( 0L, PARAChSize( (scContUnit*)srcPara ) );
		firstPara->PostInsert( newPara );
		firstPara = newPara;
	}
	tmpOffset = 0;
	finalPara->PasteText( srcPara, tmpOffset );

	offset = srcPara->GetContentSize();
	Renumber();
	
	return finalPara;
}

/* ==================================================================== */
/* paste one para into another para */
void scContUnit::PasteText( const scContUnit*	srcPara,
							long&				offset )
{
	try {
		if ( offset == 0 ) {
			TypeSpec ts = srcPara->GetDefaultSpec();
			if ( ts.ptr() )
				SetDefaultSpec( ts );
		}
			
			// paste the specs in
		fSpecRun.InsertRun( offset, srcPara->GetContentSize(), srcPara->GetSpecRun() );

			// paste the text in
		fCharArray.Paste( ((scContUnit*)srcPara)->GetCharArray(), offset );
		Mark( scREBREAK );

#ifdef _RUBI_SUPPORT
			// paste the rubis in
		if ( fRubiArray || srcPara->GetRubiArray() ) {
	
			if ( fRubiArray && !srcPara->GetRubiArray() )
				fRubiArray->BumpRubiData( offset, srcPara->GetContentSize() );
			else if ( !fRubiArray && srcPara->GetRubiArray() ) {
				AllocRubiArray( *srcPara->GetRubiArray() );
				fRubiArray->BumpRubiData( 0, offset );
			}
			else
				fRubiArray->Paste( *srcPara->GetRubiArray(), offset, srcPara->GetContentSize() );
		}
#endif _RUBI_SUPPORT

		scTextline* txl = FindLine( offset );
		if ( txl )
			txl->Mark( scREPAINT ); /* force repaint */

		long startOffset = offset;
		offset += srcPara->GetContentSize();

		fSpecRun.SetContentSize( GetContentSize() );
		
		fCharArray.RepairText( fSpecRun, startOffset, offset );
	} 
	catch( ... ) {
		SCDebugBreak(); // remove stuff from the paragraph
		throw;
	} 
}

/* ==================================================================== */
/* split the paragraph into to two paragraphs at the split */
scContUnit* scContUnit::Split( long& offset )
{
	scContUnit* p2;

	p2 = CopyText( offset, GetContentSize() );

	ClearText( offset, GetContentSize() );
	if ( offset == 0 )
		SetFirstline( NULL );
	
	PostInsert( p2 );

	Mark( scREBREAK );
	p2->Mark( scREBREAK );
	offset = 0;
	return p2;
}

/* ==================================================================== */
// merge this paraH with the previous, "this" is deleted in
// this method, the new content unit is returned 
scContUnit* scContUnit::Merge( long& offset )
{
	scStream*	stream	= (scStream*)FirstInChain();
	scContUnit* prev = GetPrev();

	if ( prev ) {

		scColumn* col = scColumn::FindFlowset( stream );

		offset = prev->GetContentSize();

		Unlink( );
		prev->Renumber();

		long tmp = offset;

		prev->PasteText( this, tmp );
		
		Free( col ? col->FlowsetGetSelection() : 0 );
		
		return prev;
	}
	return this;
}

/* ==================================================================== */
/* this clears the text from a paragraph and returns true if the entire
 * text of paragraph has been deleted
 */
BOOL scContUnit::ClearText( long		offset1,
							long		offset2 )
{
	scTextline* txl;
	BOOL		entireParaDeleted = false;

	offset1 = MAX( MIN( offset1, GetContentSize() ), 0 );
	offset2 = MIN( MAX( offset2, 0 ), GetContentSize() );

	if ( offset1 == 0 && offset2 == GetContentSize() )
		entireParaDeleted = true;

	if ( entireParaDeleted ) {
		GetCharArray().RemoveBetweenOffsets( offset1, offset2 );
		fSpecRun.SetContentSize( 0 );
		
#ifdef _RUBI_SUPPORT
		DeleteRubiArray();
#endif _RUBI_SUPPORT
	}
	else {
		GetCharArray().RemoveBetweenOffsets( offset1, offset2 );
		fSpecRun.Clear( offset1, offset2 );
		GetCharArray().RepairText( fSpecRun, offset1, offset1 );
		
#ifdef _RUBI_SUPPORT
		if ( GetRubiArray() ) {
			GetRubiArray()->DeleteRubiData( offset1, offset2 );
			if ( !GetRubiArray()->GetNumItems() )
				DeleteRubiArray();
		}
#endif _RUBI_SUPPORT
	}

	Mark( scREBREAK );

		/* break link to first line if we remove that text, the refernece
		 * to this text will be patched in the reformatting process
		 */
	txl 			= GetFirstline();

	if ( txl ) {
		scColumn*	col = txl->GetColumn();
		if ( txl && col->GetRecomposition() ) {
			scTextline* nextTxl;
		
			for ( ; txl && txl->GetPara( ) == this; txl = nextTxl ) {
				nextTxl = txl->GetNextLogical();
				if ( offset2 >= txl->GetEndOffset( ) ) {
						// the delete takes care of patching the para
					txl->MarkForDeletion( );
				}
				else {
					long startOffset = MIN( txl->GetStartOffset( ) - offset2, GetContentSize() );
					long endOffset	 = MIN( txl->GetEndOffset( ) - offset2, GetContentSize() );
				
					txl->SetOffsets( startOffset, endOffset );
				}
			}
		}
	}
	
	return entireParaDeleted;
}

/* ==================================================================== */
/* force a repaint of the logical selection - map the logical world
 * into the layout world
 */
void scContUnit::ForceRepaint( long offset1,
							   long offset2 )
{
	scTextline* txl;
	scTextline* ntxl;
	scColumn*	col;

	txl = GetFirstline();

	if ( txl ) {
		col = txl->GetColumn();

			/* search the lines of the paragraph until we find a line
			 * containing the 'offset'
			 */
		for ( ; txl; txl = ntxl ) {

			if ( offset2 < txl->GetStartOffset( ) )
				break;
			else if ( offset1 > txl->GetEndOffset( ) )
				;
			else if ( offset1 < txl->GetEndOffset( ) )
				txl->Mark( scREPAINT );

			ntxl = LNNext( txl );
			if ( ntxl == NULL ) {
					/* hit the bottom of a column, check next column */
				col = col->GetNext();
				ntxl = NextColumn( col );
			}
			if ( txl->IsLastLinePara( ) )
				break;
		}
	}
}

/* ==================================================================== */
/* find the location of 'offset' in this paragraph */
BOOL scContUnit::FindLocation( long&		offset, 	/* offset location to find */
							   BOOL&		endOfLineP,/* true if cursor stays at end of
														* this line instead of moving to
														* next on hyphenated word
														*/
							   scTextline*& txl,	/* line that offset is on */
							   MicroPoint&	hLoc,		/* location on line from org*/
							   eContentMovement cursDirect )
{
	scTextline* ntxl;
	BOOL		endOfLine	= false;

		// find the first line of the paragraph
	txl = fFirstline;

		// search the lines of the paragraph until we find a line
		// containing the 'offset'
	for ( ; txl; txl = ntxl ) {

		if ( offset >= txl->GetStartOffset ()) {
			if ( offset < txl->GetEndOffset ()) {
				break;					/* we found it */
			}
				/* Stop here if we are at the end of a hyphenated
				 * line and endOfLineP is true
				 */
			else if ( endOfLineP && offset == txl->GetEndOffset() && txl->IsHyphenated()) {
				endOfLine = true;
				break;					/* we found it */
			}
		}

		ntxl = txl->GetNextLogical();				/* get next line */
		if ( !ntxl || ntxl->GetPara() != this )
			break;
	}

	endOfLineP	= endOfLine;

	if ( txl ) {
		scMuPoint	charOrg;
		
			// find location on line
		charOrg = txl->Locate( offset, charOrg, cursDirect );
		

		if ( txl->GetColumn()->GetFlowdir().IsVertical() )
			hLoc = charOrg.y;
		else
			hLoc = charOrg.x;
		return true;
	}
	
	hLoc	= LONG_MIN;
	return false;
}

/* ==================================================================== */
static void ArrowSupport( scKeyRecord&	keyRec,
						  long& 		tmMove )
{
	switch ( keyRec.keycode() ) {
		case scUpArrow:
			tmMove					= PREV_LINE;
			keyRec.replacedchar()	= scDownArrow;
			break;
		case scDownArrow:
			tmMove					= NEXT_LINE;
			keyRec.replacedchar()	= scUpArrow;
			break;
		case scLeftArrow:
			tmMove					= -1;
			keyRec.replacedchar()	= scRightArrow;
			break;
		case scRightArrow:
			tmMove					= 1;
			keyRec.replacedchar()	= scLeftArrow;
			break;
	}
}

/* ==================================================================== */
scContUnit* scContUnit::KeySplit( long& 		offset,
								  scKeyRecord&	keyRec,
								  long& 		tmMove,
								  short&		rebreak )
{
	scContUnit* p = Split( offset );

	offset		= 0;
	rebreak = true;
	if ( keyRec.restoreselect() ) { /* replacing forward deletion */
		tmMove = -1;
		keyRec.replacedchar() = scForwardDelete;
	}
	else {
		tmMove = 0;
		keyRec.replacedchar() = scBackSpace;
	}
	return p;
}

/* ==================================================================== */
scContUnit* scContUnit::KeyInsert( long&			offset,
								   scKeyRecord& 	keyRec,
								   long&			tmMove,
								   short&			rebreak,
								   BOOL 			textCleared,
								   TypeSpec 		clearedSpec,
								   BOOL&			iAmRemoved )
{
	scContUnit* startPara		= this;
	long		computedOffset	= 0;


		/* insert the character into the text */
	switch ( keyRec.keycode() ) {
		case scUpArrow:
		case scDownArrow:
		case scLeftArrow:
		case scRightArrow:
			ArrowSupport( keyRec, tmMove );
			break;
			
		case scParaSplit:
			startPara = KeySplit( offset, keyRec, tmMove, rebreak );
			break;

		case scBackSpace:
		case scForwardDelete:

			if ( keyRec.keycode() == scBackSpace && offset == 0 ) {
				startPara = Merge( offset );
				iAmRemoved = true;

				if ( startPara ) {
					startPara->Mark( scREBREAK );
					rebreak = true;
					keyRec.replacedchar() = scParaSplit;
				}
				else
					keyRec.noop() = true;

				tmMove = 0;
				
				return startPara;
			}
			else if ( keyRec.keycode() == scForwardDelete && offset == GetContentSize() ) {
				if ( GetNext() == NULL )
					keyRec.noop() = true;
				else {
					startPara = GetNext()->Merge( offset );
					startPara->Mark( scREBREAK );
					rebreak = true;
					keyRec.replacedchar() = scParaSplit;
					keyRec.restoreselect() = true;
						/* flag to reset cursor behind new ch */
				}

				tmMove = 0;
				break;
			}
			computedOffset = -1;
			/* FALL THROUGH */
		default:
			startPara->CharInsert( computedOffset,
								   offset, 
								   keyRec,
								   tmMove,
								   rebreak,
								   textCleared,
								   clearedSpec );
			break;
	}
	return startPara;
}
								   
/* ==================================================================== */
void scContUnit::Insert( const CharRecord&	ch,
						 TypeSpec&			spec,
						 long				offset )
{
	CharRecordP chRec = (CharRecordP)&ch;
	fCharArray.Insert( chRec, offset, 1 );
	fSpecRun.BumpOffset( offset, fCharArray.GetNumItems() );

	if ( spec.ptr() )
		fSpecRun.ApplySpec( spec, offset, offset + 1 );

	Mark( scREBREAK );
}

/* ==================================================================== */
void scContUnit::CharInsert( long		computedOffset,
							 long&		offset,
							 scKeyRecord&	keyRec,
							 long&		tmMove,
							 short& 	rebreak,
							 BOOL		textCleared,
							 TypeSpec	clearedSpec )
{
#if SCDEBUG > 1
	{
		static int doit;
		if ( doit )
			fSpecRun.PrintRun( "scContUnit::CharInsert" );
	}
#endif

#ifdef _RUBI_SUPPORT
	if ( fRubiArray ) {
		if ( fRubiArray->IsRubiData( offset + computedOffset ) ) {
			scRubiData rd;
			fRubiArray->GetRubiAt( rd, offset + computedOffset );
			fCharArray.Transform( rd.fStartOffset, rd.fEndOffset, eRemoveJapTran, 0 );

			fRubiArray->DeleteRubiData( offset );
			if ( !fRubiArray->GetNumItems() )
				DeleteRubiArray();
		}
	}
#endif _RUBI_SUPPORT

	if ( computedOffset >= 0 )
		fCharArray.SetNumSlots( fCharArray.GetNumItems() + 1 );
	
	fCharArray.CharInsert( tmMove, 
						   fSpecRun, 
#ifdef _RUBI_SUPPORT
						   fRubiArray,
#endif _RUBI_SUPPORT
						   offset, 
						   keyRec, 
						   textCleared,
						   clearedSpec );

	fSpecRun.SetContentSize( GetContentSize() );

#if SCDEBUG > 1
	{
		static int doit;
		if ( doit )
			fSpecRun.PrintRun( "void scContUnit::CharInsert 2" );
	}
#endif
			
	scTextline* txl = FindLine( offset );
	if ( txl )
		txl->Mark( scREPAINT ); /* force repaint */

	Mark( scREBREAK );

	rebreak = true;
}

/* ==================================================================== */
void scContUnit::SetDefaultSpec( TypeSpec& ts )
{
	if ( ts.ptr() != defspec_.ptr() ) {
		defspec_ = ts;
		Mark( scREBREAK );
	}
	ForceRepaint( 0, LONG_MAX );
}

/* ==================================================================== */
/* set the text style between offset1 and offset2, return the column or
 * previous column containing the effected text
 */
void scContUnit::SetStyle( long 	offset1,
						   long 	offset2,
						   TypeSpec style,
						   BOOL 	retabulate,
						   BOOL 	forceRepaint )
{
	long	tmp;
	long	compOffset1,
			compOffset2;

	if ( offset1 >= offset2 ) {
		tmp 	= offset1;
		offset1 = offset2;
		offset2 = tmp;
	}

	compOffset1 = MAX( offset1, 0 );
	compOffset2 = MIN( offset2, GetContentSize() );
	
		// apply the spec only if it is an emopty para		
	if ( compOffset1 == 0 && compOffset2 == 0 && GetContentSize() )
		return;

	if ( compOffset1 == 0 || compOffset1 != compOffset2 ) {
		if ( compOffset2 == GetContentSize() )
			fSpecRun.ApplySpec( style, compOffset1, LONG_MAX );
		else
			fSpecRun.ApplySpec( style, compOffset1, compOffset2 );

#ifdef _RUBI_SUPPORT
		if ( fRubiArray )
			fRubiArray->ApplyStyle( compOffset1, compOffset2, style );
#endif _RUBI_SUPPORT
			
		fSpecRun.SetContentSize( GetContentSize() );
		if ( forceRepaint )
			ForceRepaint( compOffset1, compOffset2 );
		
		if ( retabulate )
			fCharArray.Retabulate( fSpecRun, compOffset1, compOffset2, style, GetContentSize() );
		else
			Mark( scRETABULATE );
		
		Mark( scREBREAK );
	}
}

/* ==================================================================== */
/* fill in some specific information about a character */
void scContUnit::ChInfo( long			offset,
						 UCS2&			ch, 		/* character at offset */
						 ulong& 		flags,
						 MicroPoint&	escapement, /* escapement at offset */
						 MicroPoint&	wordspace,	/* ws escapement at offset */
						 TypeSpec&		ts, 		/* typespec at offset */
						 eUnitType& 	unitType )	/* relative or absolute */
{
	if ( offset == 0 ) {
		ch			= scParaStart;	
		flags		= 0;		
		escapement	= 0;
		ts			= fSpecRun.SpecAtOffset( offset );
		unitType	= eAbsUnit;
	}
	else if ( offset == GetContentSize() + 1 ) {
		ch			= scParaEnd;
		flags		= 0;
		escapement	= 0;
		ts			= fSpecRun.SpecAtOffset( GetContentSize() );
		unitType	= eAbsUnit;
	}
	else if ( offset > GetContentSize() ) {
		ch			= 0;
		flags		= 0;
		escapement	= 0;
		ts.clear();
		unitType	= eAbsUnit;
	}
	else
		fCharArray.CharInfo( fSpecRun, offset, ch, flags, escapement, ts, unitType );

	fCharArray.WordSpaceInfo( offset, wordspace );
}

/* ==================================================================== */
/* transform the text style between offset1 and offset2, return the column or
 * previous column containing the effected text
 */
void scContUnit::TextTrans( long		offset1,
							long		offset2,
							eChTranType trans,
							int 		numChars )
{
	long	tmp;

	if ( offset1 >= offset2 ) {
		if ( offset1 == offset2 )
			return;
		tmp = offset1;
		offset1 = offset2;
		offset2 = tmp;
	}

	if ( offset1 == LONG_MIN )
		offset1 = 0;
	if ( offset2 == LONG_MAX )
		offset2 = GetContentSize();

	fCharArray.Transform( offset1, offset2, trans, numChars );

	TypeSpec nullSpec;
	fCharArray.Retabulate( fSpecRun, offset1, offset2, nullSpec, GetContentSize() );
	
	Mark( scREBREAK );
}

/* ==================================================================== */
void scContUnit::InitParaSpec( TypeSpec& ts )
{
	if ( !defspec_.ptr() )
		defspec_ = ts;
	scCachedStyle::SetParaStyle( this, defspec_ ); 
}

/* ==================================================================== */
void scContUnit::LockMem( CharRecordP&		chRec,
						  scSpecRecord*&	specRec )
{	
	chRec	= (CharRecordP)GetCharArray().Lock( );
	specRec = fSpecRun.ptr();

	TypeSpec ts = specRec->spec();
	InitParaSpec( ts );
	scCachedStyle::GetCachedStyle( ts );
}

/* ==================================================================== */
void scContUnit::UnlockMem(  )
{
	GetCharArray().Unlock( );
}

/* ==================================================================== */
/* free a scrap handle */
void scContUnit::FreeScrap( long& bytesFreed )
{
	scContUnit* p = this;
	scContUnit* nextPara;

	bytesFreed = 0;
	for ( ; p; p = nextPara ) {
		nextPara = p->GetNext( );
		bytesFreed += p->ExternalSize();
		p->Free();
	}
}

/* ==================================================================== */
/* this is the simple case of freeing the paragraph,
 * NO disentangling of pointers
 */
void scContUnit::Free( scSelection* select )
{
	if ( select ) {
			// if para is on selection list remove it from selection list
		select->CheckFreePara( this );
	}

#ifdef _RUBI_SUPPORT
		// free rubi if present
	DeleteRubiArray();
#endif _RUBI_SUPPORT

	delete this;
}

/* ==================================================================== */
// duplicate a paragraph, using old paragraph as the model and linking it
// to 'prevParaH'
scContUnit* scContUnit::Copy( scContUnit* prevPara ) const
{
	TypeSpec nullSpec;

	scContUnit* dstPara = scContUnit::Allocate( nullSpec, prevPara, GetCount() );

	fCharArray.Copy( dstPara->GetCharArray(), 0, fCharArray.GetContentSize() );
	dstPara->CopySpecRun( fSpecRun );
	TypeSpec ts = defspec_;
	dstPara->SetDefaultSpec( ts );

#ifdef _RUBI_SUPPORT
	if ( fRubiArray ) {
		dstPara->AllocRubiArray( *fRubiArray );
	}
#endif _RUBI_SUPPORT

	return dstPara;
}

/* ==================================================================== */
long scContUnit::ReadStream( APPCtxPtr	ctxPtr,
							 IOFuncPtr	readFunc )
{
	long	ret = fCharArray.ReadText( fSpecRun, ctxPtr, readFunc );
	Mark( scRETABULATE );

	return ret;
}

/* ==================================================================== */
void scContUnit::WriteStream( BOOL		addDcr,
							  APPCtxPtr ctxPtr,
							  IOFuncPtr writeFunc )
{
	fCharArray.WriteText( fSpecRun, addDcr, ctxPtr, writeFunc );
}

/* ==================================================================== */
void scContUnit::ReadAPPText( stTextImportExport& appText )
{
	fCharArray.ReadAPPText( fSpecRun, appText );
	Mark( scRETABULATE );
}

/* ==================================================================== */
void scContUnit::WriteAPPText( stTextImportExport& appText )
{
	appText.StartPara( defspec_ );
	fCharArray.WriteAPPText( fSpecRun, appText );
}

/* ==================================================================== */
/* scRETABULATE a paragraph, if ts is NULL whole para will be reformatted
 * otherwise only the ts section will be reformatted
 */
void scContUnit::Retabulate( TypeSpec ts )
{
	if ( GetContentSize() > 0 ) {

		fCharArray.Retabulate( fSpecRun, 0L, GetContentSize(), ts, GetContentSize() );
		Mark( scREBREAK );
	}
	Unmark( scRETABULATE );
}

/* ==================================================================== */
/* find the last line of a paragraph */
scTextline* scContUnit::GetLastline() const
{
	scTextline* txl;
	scTextline* nexttxl;
	
	scAssertValid();
	for ( txl = fFirstline; txl; txl = nexttxl ) {
		if ( txl->IsLastLinePara() )
			break;
	
		nexttxl = txl->GetNextLogical();

		if ( !nexttxl || nexttxl->GetPara( ) != this )
			break;
	}
	
	if ( txl )
		scAssert( txl->GetPara( ) == this );
	return txl;
}

/* ==================================================================== */
/* find the last visible line of a paragraph */
scTextline* scContUnit::GetLastVisibleLine( ) const
{
	scTextline* prevTxl;
	scTextline* txl;

	scAssertValid();

		// start with first line of paragraph
	prevTxl = fFirstline;

	for ( txl = prevTxl; txl; txl = txl->GetNextLogical() ) {
		if ( txl->GetPara( ) != this )
			break;
		prevTxl = txl;	
	}	
	return prevTxl;
}

/* ==================================================================== */
/* find the last visible paragraph in a stream */
scContUnit* scContUnit::GetLastVisiblePara( ) const
{
	const scContUnit*	lastp;
	const scContUnit*	p = this;	

	for ( lastp = p; p; p = p->GetNext() ) {
		if ( !p->GetFirstline() )
			break;
		lastp = p;
	}

	return (scContUnit*)lastp;
}

/* ==================================================================== */
/* return the previous visible paragraph */
scContUnit* scContUnit::GetPrevVisiblePara( void ) const
{
	const scContUnit* p = this;
	
	for ( ; p; p = p->GetPrev() ) {
		if ( p->GetFirstline() )
			break;
	}
	return (scContUnit*)p;
}

/* ==================================================================== */
/* renumber the paragraphs of a stream */
void scContUnit::Renumber( ) 
{
	long		count;
	scContUnit* p = this;

		// back up
	for ( ; p && p->GetPrev(); p = p->GetPrev() )
		;

		// renumber
	for ( count = 0; p; p = p->GetNext() )
		p->SetCount( count++ );
}

/* ==================================================================== */
/* report back size of paragraph on disk */
long scContUnit::ExternalSize( ) const
{
	long exSize;

	exSize = (long)sizeof(scContUnit);

	exSize += fCharArray.ExternalSize();
	exSize += fSpecRun.ExternalSize( );

#ifdef _RUBI_SUPPORT
	if ( GetRubiArray() )
		exSize += GetRubiArray()->ExternalSize();
#endif _RUBI_SUPPORT
		
	return exSize;
}

/* ==================================================================== */
// unlink a paragraph from a stream
void scContUnit::Unlink( )
{
	scContUnit* lastPara;
	scContUnit* nextPara;

		// mark all the lines of the paragraph as being invalid
	Deformat( );

	lastPara = GetPrev();
	nextPara = GetNext();
	
	if ( lastPara )
		lastPara->SetNext( nextPara );
	else {
			// this is the first paragraph in the stream and we must let
			// the columns know that the head of the stream has changed
			//
		
		GetStream()->ResetStream( (scStream*)nextPara );

	}
	if ( nextPara )
		nextPara->SetPrev( lastPara );

	SetPrev( NULL );
	SetNext( NULL );
}

/* ==================================================================== */
/* return spec at begginning of paragraph */
TypeSpec scContUnit::SpecAtEnd( )
{
	int i = 0;
	for (i = 0; !fSpecRun[i].isTerminator(); i++ )
		;
	return fSpecRun[i].spec();
}

/* ==================================================================== */
/* return spec at end of paragraph */
TypeSpec scContUnit::SpecAtStart( )
{
	return fSpecRun[0].spec();
}

/* ==================================================================== */
/* return spec at offset of paragraph */

TypeSpec scContUnit::SpecAtOffset( long offset )
{
	return fSpecRun.SpecAtOffset( MAX( 0, offset - 1)  );
}

/* ==================================================================== */
/* insert specs in this selection of the paragraph */
void scContUnit::OffsetGetTSList( long				startOffset,
								  long				endOffset,
								  scTypeSpecList&	tsList )
{
	if ( endOffset == startOffset )
		return;

	if ( endOffset == LONG_MAX )
		endOffset = GetContentSize();
	if ( startOffset == LONG_MIN )
		startOffset = 0;

	int i = fSpecRun.indexAtOffset( startOffset );
	do {
		TypeSpec ts = fSpecRun[i].spec();
		tsList.Insert( ts );
	} while ( fSpecRun[++i].offset() < endOffset );
}

/* ==================================================================== */
/* insert specs in this the paragraph */
void scContUnit::GetTSList( scTypeSpecList& tsList )
{
	OffsetGetTSList( LONG_MIN, LONG_MAX, tsList );
}

/* ==================================================================== */
void scContUnit::OffsetGetCharSpecList( long			startOffset,
										long			endOffset,
										scSpecLocList&	csList )
{
	if ( endOffset == startOffset )
		return;

	if ( endOffset == LONG_MAX )
		endOffset = GetContentSize();
	if ( startOffset == LONG_MIN )
		startOffset = 0;

	int i = fSpecRun.indexAtOffset( startOffset );
	scSpecLocation chsploc( GetCount(), startOffset );
	chsploc.spec() = fSpecRun[i].spec();	
	csList.Append( chsploc );

	while ( fSpecRun[++i].offset() < endOffset ) {
		scSpecLocation chsploc( GetCount(), fSpecRun[i].offset() );
		chsploc.spec() = fSpecRun[i].spec();	
		csList.Append( chsploc );
	}

	csList.TermParagraph( GetCount(), endOffset );
}

/* ==================================================================== */
void scContUnit::SelectWord( long	offset,
							 long&	startWord,
							 long&	endWord )
{
	fCharArray.SelectWord( offset, startWord, endWord );
}

/* ==================================================================== */
void scContUnit::Deformat( )
{
	scTextline* txl;
	scTextline* next;
				
	for ( txl = GetFirstline(); txl && txl->GetPara() == this; txl = next ) {
		next = txl->GetNextLogical();
		if ( txl && txl->GetColumn() )
			txl->GetColumn()->Mark( scINVALID );
		txl->MarkForDeletion( );
	}
	SetFirstline( 0 );
	Mark( scREBREAK );
}

/* ==================================================================== */
/* return the earlier of these two paragraphs in a stream
 * NULL would indicate that they are not in the same stream
 */
scContUnit* scContUnit::Earlier( const scContUnit* p2 ) const
{
	const scContUnit* prevPara = this;

	for ( ; prevPara != NULL; prevPara = prevPara->GetPrev( ) )
		if ( prevPara == p2 )
			return (scContUnit*)p2;

	return (scContUnit*)this;
}

/* ==================================================================== */
scContUnit* scContUnit::Allocate( TypeSpec& 	spec,
								  scContUnit*	cu,
								  long			ct )
{
	return new scContUnit( spec, cu, ct );
}

/* ==================================================================== */
stPara::stPara( ) :
	paraspec_( 0 )
{
	reset();
}

/* ==================================================================== */
stPara::stPara( TypeSpec& pspec ) :
	paraspec_( pspec )
{
	if ( pspec.ptr() )
		specs_.AppendSpec( pspec, 0 );
	reset();
}

/* ==================================================================== */
stPara::~stPara()
{
}

/* ==================================================================== */
void stPara::append( TypeSpec& ts )
{
	if ( ts != specs_.SpecAtOffset( ch_.size() ) )
		 specs_.AppendSpec( ts, ch_.size() );
}

/* ==================================================================== */
void stPara::append( const uchar* ch, int len )
{
	for ( int i = 0; i < len; i++ )
		ch_.push_back( (UCS2)ch[i] );
}

/* ==================================================================== */
void stPara::append( USTR& ustr )
{
	for ( unsigned i = 0; i < ustr.len; i++ )
		ch_.push_back( (UCS2)ustr.ptr[i] );
}

/* ==================================================================== */
void stPara::append( UCS2 ch )
{
	ch_.push_back( ch );
}

/* ==================================================================== */
stPara& stPara::operator=( const stPara&  p )
{
	ch_.clear();
	for ( int i = 0; i < (int)p.ch_.size(); i++ )
		ch_.push_back( p.ch_[i] );

	choffset_ = p.choffset_;

	specs_.RemoveAll();
	for (int i = 0; i < p.specs_.NumItems(); i++ )
		specs_.Append( p.specs_[i] );

	paraspec_ = p.paraspec_;
	return *this;
}

/* ==================================================================== */
int stPara::get( UCS2& ch, TypeSpec& spec )
{
	if ( choffset_ < (int32)ch_.size() ) {
		spec = specs_.SpecAtOffset( choffset_ );
		ch = ch_[choffset_++];
		return choffset_;
	}
	return 0;
}

/* ==================================================================== */	
int stPara::validate() const
{
	scAssert( paraspec_.ptr() );
	specs_.DebugRun( "stPara::validate" );
	return 1;
}

/* ==================================================================== */
void stPara::setparaspec( TypeSpec& ts )
{
	paraspec_ = ts;
	scAssert( ch_.size() == 0 );
	specs_.AppendSpec( ts, 0 );
}

/* ==================================================================== */
int stPara::complete()
{
	if ( specs_.NumItems() == 1 )
		specs_.AppendSpec( paraspec_, 0 );
	
	return validate();
}
