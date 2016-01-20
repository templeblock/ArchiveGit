/*=================================================================
 
	File:		scselec2.c

	$Header: /Projects/Toolbox/ct/Scselec2.cpp 4	 6/18/97 10:20a Wmanis $

	Contains:	selection code

	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

=================================================================*/


#include "scselect.h"
#include "scglobda.h"
#include "scparagr.h"
#include "scstream.h"
#include "sctextli.h"
#include "scctype.h"
#include "sccolumn.h"
#include "scmem.h"


#define AssertInRange( val, low, high ) 	scAssert( val >= low && val <= high );

static void SelectLocateOnLine( TextMarker *tm, eContentMovement movement )
{
	scMuPoint	mPt;
	Bool		vert		= false;
	scFlowDir	flowDir;
	
	tm->fPara	= tm->fTxl->GetPara();

	flowDir = tm->fTxl->GetFlowdir( );
	vert = flowDir.IsVertical();

	if ( vert ) {
		mPt.x	= tm->fTxl->GetOrigin().x;
		mPt.y	= tm->fSelMaxX;
	}
	else {
		mPt.x	= tm->fSelMaxX;
		mPt.y	= tm->fTxl->GetOrigin().y;
	}

	if ( vert )
		mPt.x -= tm->fTxl->GetVJOffset();
	else
		mPt.y += tm->fTxl->GetVJOffset();

	scXRect xrect;
	tm->fTxl->QueryExtents( xrect, 1 );

	xrect.Clamp( mPt );

	scMuPoint	charOrg;
	tm->fTxl->Select( charOrg, tm->fOffset, mPt, movement, tm->fEndOfLine );
	if ( vert ) 
		tm->fHLoc = charOrg.y;
	else
		tm->fHLoc = charOrg.x;
}

/*====================================================================*/

void scSelection::SetParaSelection( scContUnit* p,
									long		start,
									long		end )
{
	scStreamLocation	mark;
	scStreamLocation	point;
	scStream*			stream;

	stream			= p->GetStream();
		
	mark.fParaNum	= p->GetCount();
	AssertInRange( start, 0, p->GetContentSize() );
	mark.fParaOffset	= start;

	point = mark;
	AssertInRange( end, start, p->GetContentSize() );
	point.fParaOffset = end;

	Restore( &mark, &point, stream, true ); 
}						
								
/*====================================================================*/

int TextMarker::SelectPrevCharInPara()
{
	if ( !fOffset )
		return 0;
	fOffset -= 1;
	
	return 1;
}

/* ==================================================================== */

int TextMarker::SelectNextCharInPara()
{
	if ( fOffset == fPara->GetContentSize() )
		return 0;
	
	fOffset = MIN( fPara->GetContentSize(), fOffset + 1 );
	return 1;
}

/* ==================================================================== */

int TextMarker::SelectPrevChar()
{
	if ( !SelectPrevCharInPara() ) {
		if ( fPara->GetPrev() ) {
			fPara = fPara->GetPrev();
			fOffset = fPara->GetContentSize();
		}
		else
			return 0;
	}
	return 1;
}

/*====================================================================*/

int TextMarker::SelectNextChar()
{
	if ( !SelectNextCharInPara() ) {
		if ( fPara->GetNext() ) {
			fOffset = 0;
			fPara = fPara->GetNext();
		}
		else
			return 0;
	}
	return 1;
}

/*====================================================================*/

int TextMarker::SelectPrevLine()
{
	if ( !fTxl )
		return 0;
	
	if ( fTxl->GetPrevLogical() )
		fTxl = fTxl->GetPrevLogical();
	else
		return 0;
	
	SelectLocateOnLine( this, eCursBackward );
	return 1;
}

/*====================================================================*/

int TextMarker::SelectNextLine()
{
	if ( !fTxl )
		return 0;
	
	if ( fTxl->GetNextLogical() )
		fTxl = fTxl->GetNextLogical();
	else
		return 0;
		
	SelectLocateOnLine( this, eCursForward );
	return 1;
}

/* ==================================================================== */

int TextMarker::SelectStartLine(  )
{
	scDebugAssert( fTxl != 0 );
	if ( fTxl ) {
		fCol		= fTxl->GetColumn();
		fPara		= fTxl->GetPara();
		
		fOffset 	= fTxl->GetStartOffset( );
		fEndOfLine	= false;
	}

	return fPara != 0 && fTxl != 0;
}

/*====================================================================*/

int TextMarker::SelectEndLine(	)
{
	scDebugAssert( fTxl != 0 );
	if ( fTxl ) {
		fCol		= fTxl->GetColumn();
		fPara		= fTxl->GetPara();
		
#if 1
		if ( CTIsSpace( fTxl->CharAtEnd( ) ) ) {
			fOffset 	= fTxl->GetEndOffset( ) - 1;
			fEndOfLine	= false;
		}
		else {
			fOffset 	= fTxl->GetEndOffset( );
			fEndOfLine	= true;
		}
#else
		fOffset 	= fTxl->GetEndOffset( );
		fEndOfLine	= true;
#endif
	}
	return fPara != 0 && fTxl != 0;
}

/*====================================================================*/

Bool TextMarker::SelectPrevWord(  )
{
	CharRecordP startChRec;
	UCS2		ch;
	long		endOffset;
	
	if ( !fOffset )
		return false;

	startChRec = (CharRecordP)fPara->GetCharArray().Lock();

	endOffset	= fOffset;
	
	do {
		ch =  startChRec[--endOffset].character ;
	} while ( !CTIsSelectable( ch ) && endOffset > 0 );
		
	fOffset = endOffset;
	
	fPara->GetCharArray().Unlock();
	return CTIsSelectable( ch );
}

/*====================================================================*/

Bool TextMarker::SelectNextWord(  )
{
	CharRecordP startChRec;
	UCS2		ch;
	long		endOffset,
				limitOffset;
	
	limitOffset = PARAChSize( fPara );
		
	if ( fOffset >= limitOffset )
		return false;
	
	startChRec = (CharRecordP)fPara->GetCharArray().Lock();
	
	endOffset	= fOffset;
	
	do {
		ch =  startChRec[endOffset++].character ;
	} while ( !CTIsSelectable( ch ) && endOffset <= limitOffset );
		
	fOffset = endOffset;

	fPara->GetCharArray().Unlock();
	return CTIsSelectable( ch );
}

/*====================================================================*/

Bool TextMarker::SelectPrevSpellWord( void )
{
	CharRecordP startChRec;
	UCS2		ch;
	long		endOffset;
	
	if ( !fOffset )
		return false;
	
	startChRec = (CharRecordP)fPara->GetCharArray().Lock();

	endOffset	= fOffset;
	
	do {
		ch =  startChRec[--endOffset].character ;
	} while ( CTIsSpace( ch ) && endOffset > 0 );
			
	fOffset = endOffset;
	
	fPara->GetCharArray().Unlock();

	return !CTIsSpace( ch );	
}

/*====================================================================*/

int TextMarker::SelectNextSpellWord(  )
{
	CharRecordP startChRec;
	UCS2		ch;
	long		endOffset,
				limitOffset;

	limitOffset = PARAChSize( fPara );
		
	if ( fOffset >= limitOffset )
		return 0;
		
	startChRec = (CharRecordP)fPara->GetCharArray().Lock();

	endOffset	= fOffset;
	
	do {
		ch =  startChRec[endOffset++].character ;
	} while ( CTIsSpace( ch ) && endOffset <= limitOffset );
	
	fOffset = endOffset;

	fPara->GetCharArray().Unlock();
		
	return !CTIsSpace( ch ) && ch != scEndStream;	
}

/*====================================================================*/

int TextMarker::SelectStartWord(  )
{
	CharRecordP startChRec;

	startChRec = (CharRecordP)fPara->GetCharArray().Lock();

	fOffset = TXTStartSelectableWord( startChRec, fOffset );

	fPara->GetCharArray().Unlock();

	return fPara != 0;
}

/*====================================================================*/

int TextMarker::SelectEndWord(	)
{
	scAssert( fOffset <= fPara->GetContentSize() );
	
	scHandleArrayLock	h( fPara->GetCharArray() );
	CharRecordP chRec = (CharRecordP)*h;
	
	fOffset = TXTEndSelectableWord( chRec, fOffset );

	return fPara != 0;
}

/*====================================================================*/

int TextMarker::SelectStartSpellWord( int eleminateLeadingSpaces )
{
	CharRecordP startChRec;

	startChRec = (CharRecordP)fPara->GetCharArray().Lock();

	fOffset = TXTStartWord( startChRec, fOffset, eleminateLeadingSpaces );

	fPara->GetCharArray().Unlock();

	return fPara != 0;
}

/*====================================================================*/

int TextMarker::SelectEndSpellWord( )
{
	CharRecordP startChRec;

	startChRec = (CharRecordP)fPara->GetCharArray().Lock();

	fOffset = TXTEndWord( startChRec, fOffset );

	fPara->GetCharArray().Unlock();

	return fPara != 0;
}

/*====================================================================*/

int TextMarker::SelectStartPara()
{
	if ( fOffset == 0 )
		return 0;
	
	fOffset = 0;
	return 1;
}

/*====================================================================*/

int TextMarker::SelectEndPara()
{
	if ( fOffset == PARAChSize( fPara ) )
		return 0;

	fOffset = PARAChSize( fPara );
	return 1;
}

/*====================================================================*/

int TextMarker::SelectPrevPara()
{
	if ( !fPara->GetPrev() )
		return 0;

	fPara = fPara->GetPrev();
	SelectStartPara();
	return 1;
}

/* ==================================================================== */
 
int TextMarker::SelectNextPara()
{
	if ( !fPara->GetNext() )
		return 0;

	fPara = fPara->GetNext();
	SelectEndPara();
	return 1;
}

/* ==================================================================== */


int TextMarker::SelectStartColumn(	)
{	
	scTextline* txl;
	
	scAssert( fCol != 0 );	
	
	txl = fCol->GetFirstline();
	
	fTxl		= txl;
	fPara		= txl->GetPara();
	fParaCount	= fPara->GetCount();
	return SelectStartLine();
}
							
/*====================================================================*/

int TextMarker::SelectEndColumn(  )
{	
	scTextline* txl;
	
	scAssert( fCol != 0 );	
	
	txl = fCol->GetLastline( );
	
	/* tm->fCol should be correct */
	fPara		= txl->GetPara();
	fTxl		= txl;
	/* tm->colCount should be correct */
	fParaCount	= fPara->GetCount();	
	fLineCount	= txl->GetLinecount();
	
	return SelectEndLine( );
}
							
/*====================================================================*/

int TextMarker::SelectStartStream()
{
	scStream* stream = fPara->GetStream();

	fPara = stream->First();
	fOffset = 0;
	return 1;
}

/*====================================================================*/

int TextMarker::SelectEndStream()
{
	scStream* stream = fPara->GetStream();

	fPara = stream->Last();
	fOffset = fPara->GetContentSize();
	return 1;
}


/*====================================================================*/
/* CONTENT BASED SELECTIONS */
/*====================================================================*/

int scSelection::PrevWord( Scope scope )
{
	TextMarker& point	= fPoint;

	while ( !point.SelectPrevWord() ) {
		if ( scope == inContUnit )
			return 0;
		point.fPara = point.fPara->GetPrev();
		if ( !point.fPara )
			return 0;
		point.fOffset = PARAChSize( point.fPara );
	}
	point.SelectStartWord( );

	SetMark( point );
	SetPoint( point );

	return point.fPara != 0;
}

/*====================================================================*/

int scSelection::NextWord( Scope scope )
{
	TextMarker point = fPoint;
	
	if ( CTIsSelectable( PARACharAtOffset( point.fPara, point.fOffset ) ) )
		point.SelectEndWord( );
		
	while ( !point.SelectNextWord( ) ) {
		if ( scope == inContUnit )
			return 0;
		point.fPara = point.fPara->GetNext();
		if ( !point.fPara )
			return 0;
		point.fOffset = 0;	
	}
	point.SelectStartWord();
		
	SetMark( point );
	SetPoint( point );

	return point.fPara != 0;
}

/*====================================================================*/

int scSelection::PrevSpellWord( Scope scope )
{
	scSelection sortedSelect( *this );
	
	sortedSelect.Sort();
	TextMarker& mark	= sortedSelect.fMark;
	TextMarker& point	= sortedSelect.fPoint;
	
	if ( CTIsAlpha( PARACharAtOffset( mark.fPara, mark.fOffset ) ) )
		mark.SelectStartSpellWord( );

	while ( !mark.SelectPrevSpellWord( ) ) {
		if ( scope == inContUnit )
			return 0;
		mark.fPara = mark.fPara->GetPrev();
		if ( !mark.fPara )
			return 0;
		mark.fOffset = PARAChSize( mark.fPara );
	}
	
	point = mark;
	
	mark.SelectStartSpellWord( );
	point.SelectEndSpellWord( );
		
	SetMark( mark );
	SetPoint( point );

	return mark.fPara != 0;
}

/*====================================================================*/

int scSelection::NextSpellWord( Scope scope )
{
	scSelection sortedSelect( *this );
	
	sortedSelect.Sort();
	TextMarker& mark	= sortedSelect.fMark;
	TextMarker& point	= sortedSelect.fPoint;
	
	UCS2 ch = PARACharAtOffset( point.fPara, point.fOffset );
	if ( !CTIsSpace( ch ) )
		point.SelectEndSpellWord(  );

	if ( !IsSliverCursor() ) {
		while ( !point.SelectNextSpellWord(  ) ) {
			if ( scope == inContUnit )
				return 0;
			point.fPara = point.fPara->GetNext();
			if ( !point.fPara )
				return 0;
			point.fOffset = 0;	
		}

		mark = point;

		if ( !mark.SelectStartSpellWord( ) || !point.SelectEndSpellWord( ) )
			return 0;
	}
		
	SetMark( mark );
	SetPoint( point );

	return mark.fPara != 0 && !IsSliverCursor();
}

/*====================================================================*/

int scSelection::StartWord( )
{
	scSelection sortedSelect( *this );
	
	sortedSelect.Sort();
	TextMarker& mark	= sortedSelect.fMark;
	TextMarker& point	= sortedSelect.fPoint;

	if ( !mark.SelectStartWord( ) )
		return 0;
	point = mark;

	SetMark( mark );
	SetPoint( point );
	return mark.fPara != 0;
}							

/*====================================================================*/
			
int scSelection::EndWord(  )
{
	scSelection sortedSelect( *this );
	
	sortedSelect.Sort();
	TextMarker& mark	= sortedSelect.fMark;
	TextMarker& point	= sortedSelect.fPoint;
	
	if ( !point.SelectEndWord( ) )
		return 0;
	mark = point;

	SetMark( mark );
	SetPoint( point );

	return mark.fPara != 0;
}

/*====================================================================*/
/* LAYOUT BASED SELECTIONS - these need error checking 
 * to see if layout exists
 */
/*====================================================================*/

static int SameBaseline( const scTextline* t1, const scTextline* t2 )
{
	return t1->GetBaseline() == t2->GetBaseline();
}

//#define SameBaseline( t1, t2 ) (t1)->GetOrigin().y == (t2)->GetOrigin().y

static scTextline* SearchLeft( scTextline* rightLineSegment,
							   MicroPoint selmax )
{  
	scFlowDir fd = rightLineSegment->GetFlowdir();
	scMuPoint mPt;
	if ( fd.IsHorizontal() )
		mPt.Set( selmax, rightLineSegment->GetOrigin().y );
	else
		mPt.Set( rightLineSegment->GetOrigin().x, selmax );

	scTextline* prevTxl = 0;
	scXRect xrect;

	MicroPoint inflation = 0;
	for( int i = 0; i < 20; i++ ) {
		for( prevTxl = rightLineSegment;
			 prevTxl && SameBaseline( prevTxl, rightLineSegment );
			 prevTxl = prevTxl->GetPrev() ) {
				prevTxl->QueryExtents( xrect, 0 );
				xrect.Inset( inflation, 0 );
				if ( xrect.PinRect( mPt ) )
					return prevTxl;
		}
		inflation -= scPOINTS( 4 );
	}
	return rightLineSegment;
}

/*====================================================================*/

static scTextline* SearchRight( scTextline* leftLineSegment,
								MicroPoint	selmax )
{
	scTextline* nextTxl = 0;
	scTextline* rightLineSegment = 0;

	for( nextTxl = leftLineSegment;
		 nextTxl && SameBaseline( nextTxl, leftLineSegment );
		 nextTxl = nextTxl->GetNext() ) {
			 rightLineSegment = nextTxl;
	}
	return SearchLeft( rightLineSegment, selmax );
}

/*====================================================================*/
		
int scSelection::PrevLine(	)
{
	scSelection sortedSelect( *this );
	
	sortedSelect.Sort();
	TextMarker& mark	= sortedSelect.fMark;
	TextMarker& point	= sortedSelect.fPoint;
	scTextline* prevTxl;

	if ( mark.fTxl ) {
		prevTxl = mark.fTxl;
		do {
			prevTxl = prevTxl->GetPrevLogical();	
		} while ( prevTxl && SameBaseline( mark.fTxl, prevTxl ) );

		if ( !prevTxl )
			return 0;

		prevTxl = SearchLeft( prevTxl, mark.fSelMaxX );
			
		if ( !prevTxl )
			return 0;
		
		mark.fTxl = prevTxl;
		
		SelectLocateOnLine( &mark, eCursBackward );
		point = mark;		
	}
	else if ( point.fTxl ) {
		prevTxl = point.fTxl;
		do {
			prevTxl = prevTxl->GetPrevLogical();	
		} while ( prevTxl && SameBaseline( point.fTxl, prevTxl ) );


		if ( !prevTxl )
			return 0;

		prevTxl = SearchLeft( prevTxl, point.fSelMaxX );

		if ( !prevTxl )
			return 0;
		point.fTxl = prevTxl;
		
		SelectLocateOnLine( &point, eCursBackward );			
		mark = point;		
	}

	SetMark( mark );
	SetPoint( point );
	return mark.fTxl != 0;
}

/*====================================================================*/

int scSelection::NextLine(	)
{
	scSelection sortedSelect( *this );
	
	sortedSelect.Sort();
	TextMarker& mark	= sortedSelect.fMark;
	TextMarker& point	= sortedSelect.fPoint;

	scTextline* nextTxl;	

	if ( point.fTxl ) {
		nextTxl = point.fTxl;	
		do {
			nextTxl = nextTxl->GetNextLogical();	
		} while ( nextTxl && SameBaseline( point.fTxl, nextTxl ) );

		if ( !nextTxl )
			return 0;

		nextTxl = SearchRight( nextTxl, point.fSelMaxX );

		if ( !nextTxl )
			return 0;
		
		point.fTxl = nextTxl;
			
		SelectLocateOnLine( &point, eCursForward );
		mark = point;		
	}
	else if ( mark.fTxl ) {
		nextTxl = mark.fTxl;	
		do {
			nextTxl = nextTxl->GetNextLogical();	
		} while ( nextTxl && SameBaseline( mark.fTxl, nextTxl ) );

		if ( !nextTxl )
			return 0;

		nextTxl = SearchRight( nextTxl, mark.fSelMaxX );

		if ( !nextTxl )
			return 0;
		
		point.fTxl = nextTxl;
			
		SelectLocateOnLine( &mark, eCursForward );
		point = mark;		
	}

	SetMark( mark );
	SetPoint( point );
	return mark.fTxl != 0;
}

/*====================================================================*/

int scSelection::PrevEntireLine(  )
{
	scSelection sortedSelect( *this );
	
	sortedSelect.Sort();
	TextMarker& mark	= sortedSelect.fMark;
	TextMarker& point	= sortedSelect.fPoint;
	
	scTextline* prevTxl;	

	if ( mark.fTxl ) {
		prevTxl = mark.fTxl->GetPrevLogical();	
		if ( !prevTxl )
			return 0;
		
		mark.fTxl = prevTxl;
		
		point = mark;		
		mark.SelectStartLine( );
		point.SelectEndLine( );
	}
		
	SetMark( mark );
	SetPoint( point );
	return mark.fTxl != 0;
}

/*====================================================================*/

int scSelection::NextEntireLine( )
{
	scSelection sortedSelect( *this );
	
	sortedSelect.Sort();
	TextMarker& mark	= sortedSelect.fMark;
	TextMarker& point	= sortedSelect.fPoint;

	scTextline* nextTxl;	

	if ( point.fTxl ) {
		nextTxl = point.fTxl->GetNextLogical(); 
		if ( !nextTxl )
			return 0;
	
		point.fTxl = nextTxl;

		mark = point;
		mark.SelectStartLine();
		point.SelectEndLine();
	}
	else if ( mark.fTxl ) {
		nextTxl = mark.fTxl->GetNextLogical();	
		if ( !nextTxl )
			return 0;
	
		mark.fTxl = nextTxl;

		point = mark;
		mark.SelectStartLine(); 	
		point.SelectEndLine();
	}
		
	SetMark( mark );
	SetPoint( point );
	
	return mark.fTxl != 0;
}

/*====================================================================*/

int scSelection::StartLine(  )
{
	scSelection sortedSelect( *this );
	
	sortedSelect.Sort();
	TextMarker& mark	= sortedSelect.fMark;
	TextMarker& point	= sortedSelect.fPoint;
	
	if ( mark.fTxl ) {
		mark.SelectStartLine();
		point = mark;
		SetMark( mark );
		SetPoint( point );
	}

	return mark.fTxl != 0;
}

/*====================================================================*/

int scSelection::EndLine( )
{
	scSelection sortedSelect( *this );
	
	sortedSelect.Sort();
	TextMarker& mark	= sortedSelect.fMark;
	TextMarker& point	= sortedSelect.fPoint;
	
	if ( point.fTxl ) {
		point.SelectEndLine( );
		mark = point;
	}
	else if ( mark.fTxl ) {
		mark.SelectEndLine();
		point = mark;	
	}
	
	SetMark( mark );
	SetPoint( point );

	return mark.fTxl != 0;
}

/*====================================================================*/

int scSelection::PrevColumn()
{
	scSelection sortedSelect( *this );
	
	sortedSelect.Sort();
	TextMarker& mark	= sortedSelect.fMark;
	TextMarker& point	= sortedSelect.fPoint;
	scColumn*	prevCol;
	
	if ( !mark.fCol ) {
		scContUnit* lastPara	= mark.fPara->GetPrevVisiblePara();
		scTextline* lastTxl 	= lastPara->GetLastVisibleLine();
		if ( lastPara && lastTxl )
			prevCol = lastTxl->GetColumn();
		else
			return 0;
	}
	else 
		prevCol = mark.fCol->GetPrev();
	
		/* check to see if we have a next column */
	if ( !prevCol )
		return 0;
		
	if ( !prevCol->GetFirstline() )
		return 0;
	
	mark.fCol	= prevCol;
	mark.fTxl	= prevCol->GetFirstline();
	mark.fPara	= mark.fTxl->GetPara();

	point = mark;
	mark.SelectStartColumn();
	point.SelectEndColumn();

	SetMark( mark );
	SetPoint( point );

	return mark.fCol != 0;
}

/*====================================================================*/

int scSelection::NextColumn()
{
	scSelection sortedSelect( *this );
	
	sortedSelect.Sort();
	TextMarker& mark	= sortedSelect.fMark;
	TextMarker& point	= sortedSelect.fPoint;
	scColumn*	nextCol;
	
	if ( !point.fCol ) {
		scContUnit* lastPara	= point.fPara->GetPrevVisiblePara();
		scTextline* lastTxl 	= lastPara->GetLastVisibleLine();
		if ( lastPara && lastTxl ) {
			point.fCol	= lastTxl->GetColumn();
			point.fPara = lastPara;
			point.fTxl		= lastTxl;
		}
		else
			return 0;
	}

	nextCol = point.fCol->GetNext();
	
		/* check to see if we have a next column */
	if ( !nextCol )
		return 0;
		
	if ( !nextCol->GetFirstline() )
		return 0;
	
	point.fCol = nextCol;
	point.fTxl = nextCol->GetFirstline();
	point.fPara = point.fTxl->GetPara();

	mark = point;
	mark.SelectStartColumn();		
	point.SelectEndColumn();

	
	SetMark( mark );
	SetPoint( point );

	return mark.fCol != 0;
}

/*====================================================================*/

int scSelection::EndColumn(  )
{
	scSelection sortedSelect( *this );
	
	sortedSelect.Sort();
	TextMarker& mark	= sortedSelect.fMark;
	TextMarker& point	= sortedSelect.fPoint;
	
	if ( !point.fCol ) {
		scContUnit* lastPara	= point.fPara->GetPrevVisiblePara();
		scTextline* lastTxl 	= 0;

		if ( lastPara )
			lastTxl = lastPara->GetLastVisibleLine();
		
		if ( lastPara && lastTxl ) {
			point.fCol	= lastTxl->GetColumn();
			point.fPara = lastPara;
			point.fTxl		= lastTxl;
		}
		else
			return 0;
	}
		
	point.SelectEndColumn( );
	mark = point;
	
	SetMark( mark );
	SetPoint( point );

	return mark.fCol != 0;
}

/*====================================================================*/

int scSelection::StartColumn( )
{
	scSelection sortedSelect( *this );
	
	sortedSelect.Sort();
	TextMarker& mark	= sortedSelect.fMark;
	TextMarker& point	= sortedSelect.fPoint;

	if ( !mark.fCol ) {
		scContUnit* lastPara	= mark.fPara->GetPrevVisiblePara();
		scTextline* txl 		= lastPara->GetFirstline();
		if ( lastPara && txl ) {
			mark.fCol	= txl->GetColumn();
			mark.fPara	= lastPara;
			mark.fTxl	= txl;
		}
		else
			return 0;
	}
	
	mark.SelectStartColumn();
	
	point = mark;
	
	SetMark( mark );
	SetPoint( point );

	return mark.fCol != 0;
}

/*====================================================================*/

int scSelection::BeginPara(  )
{
	scSelection sortedSelect( *this );
	
	sortedSelect.Sort();
	TextMarker& mark	= sortedSelect.fMark;
	TextMarker& point	= sortedSelect.fPoint;
	
	mark.fOffset = 0;
	point = mark;

	SetMark( mark );
	SetPoint( point );

	return mark.fPara != 0;
}

/*====================================================================*/

int scSelection::EndPara( )
{
	scSelection sortedSelect( *this );
	
	sortedSelect.Sort();
	TextMarker& mark	= sortedSelect.fMark;
	TextMarker& point	= sortedSelect.fPoint;
	
	point.fOffset = PARAChSize( point.fPara );
	mark = point;

	SetMark( mark );
	SetPoint( point );

	return mark.fPara != 0;
}

/*====================================================================*/

int scSelection::Para( eSelectMove	moveSelect )
{	
	scSelection sortedSelection( *this );
	scContUnit* para = NULL;
	
	sortedSelection.Sort( );			
	
	switch ( moveSelect ) {
		case ePrevPara:
			para = sortedSelection.fMark.fPara->GetPrev();		
			break;
		case eNextPara:
			para = sortedSelection.fMark.fPara->GetNext();		
			break;		
		case eFirstPara:
			para = (scContUnit*)sortedSelection.fMark.fPara->FirstInChain();
			break;
		case eLastPara:
			para = (scContUnit*)sortedSelection.fMark.fPara->LastInChain();
			break;
	}
	
	if ( para )
		SetParaSelection( para, 0, PARAChSize( para ) );

	return para != 0;
}
			
/*====================================================================*/
// returns true if selection moved

static int SLCCharacterMove( scSelection&	select, 
							 eSelectMove	moveSelect )
{
	int moved = 0;
	scSelection sortedSelect( select );
	
	sortedSelect.Sort();
	TextMarker& mark	= sortedSelect.fMark;
	TextMarker& point	= sortedSelect.fPoint;
	
	switch ( moveSelect ) {
		case ePrevChar:
			if ( !mark.fOffset && mark.fPara->GetPrev() ) {
				scContUnit* para = sortedSelect.fMark.fPara->GetPrev();
				select.SetParaSelection( para, PARAChSize( para ), PARAChSize( para ) );				
				break;
			}
		case ePrevCharInPara:
			point.fOffset = MAX( 0, point.fOffset - 1 );
			mark = point;
			moved = sortedSelect.fMark != mark;
			select.SetMark( mark );
			select.SetPoint( point );
			break;
			
		case eNextChar:
			if ( point.fOffset == PARAChSize( point.fPara ) && point.fPara->GetNext() ) {
				scContUnit* para = sortedSelect.fMark.fPara->GetNext();
				select.SetParaSelection( para, 0, 0 );
				break;
			}
		case eNextCharInPara:
			point.fOffset = MIN( point.fOffset + 1, PARAChSize( point.fPara ) );
			mark = point;
			moved = sortedSelect.fPoint != point;
			select.SetMark( mark );
			select.SetPoint( point );
			break;
		default:
			break;
	}

	return moved;
}

/*====================================================================*/

void scSelection::Extend( eSelectMove moveSelect )
{
	int setmax = 1;
	
	switch ( moveSelect ) { 	
		case ePrevChar:
			fPoint.SelectPrevChar();
			break;
		case eNextChar:
			fPoint.SelectNextChar();
			break;
			
		case ePrevWord:
		{
			TextMarker mark = fMark;
			PrevWord();
			fMark = mark;
			break;
		}
			
		case eNextWord:
		{
			TextMarker mark = fMark;
			NextWord( );
			fMark = mark;
			break;
		}

		case ePrevSpellWord:
			fPoint.SelectPrevSpellWord( );		
			break;
		case eNextSpellWord:	
			fPoint.SelectNextSpellWord( );
			break;
		
		case eStartWord:
			fPoint.SelectStartWord( );
			break;
			
		case eEndWord:
			fPoint.SelectEndWord( );
			break;
		
		case ePrevEntireLine:
			fPoint.SelectPrevLine();
			fPoint.SelectStartLine();
			break;		
			
		case eNextEntireLine:
			fPoint.SelectNextLine();
			fPoint.SelectEndLine();
			break;

		case ePrevLine:
			fPoint.SelectPrevLine();
			setmax = 0;
			break;
			
		case eNextLine: 
			fPoint.SelectNextLine();
			setmax = 0;
			break;
			
		case eStartLine:
			fPoint.SelectStartLine();			
			break;
			
		case eEndLine:
			fPoint.SelectEndLine();
			break;
			
		case eBeginPara:
			fPoint.SelectStartPara();
			break;

		case eEndPara:
			fPoint.SelectEndPara(); 	
			break;
			
		case eBeginColumn:
			fPoint.SelectStartColumn();
			break;

		case eEndColumn:
			fPoint.SelectEndColumn();
			break;

		case ePrevCharInPara:
			fPoint.SelectPrevCharInPara();
			break;

		case eNextCharInPara:
			fPoint.SelectNextCharInPara();
			break;

		case ePrevPara:
			fPoint.SelectPrevPara();
			break;

		case eNextPara: 
			fPoint.SelectNextPara();
			break;

		case eFirstPara:
			fPoint.SelectStartStream();
			break;

		case eLastPara: 
			fPoint.SelectEndStream();
			break;

		case eStartStream:
			fPoint.SelectStartStream();
			break;

		case eEndStream:
			fPoint.SelectEndStream();
			break;

		case ePrevEntireColumn:
		case eNextEntireColumn:
			
		default:
			SCDebugBreak();
			break;
	}
	fMark.UpdateInfo( setmax );
	fPoint.UpdateInfo( setmax );	
}

/*====================================================================*/

void scSelection::MoveSelect( eSelectMove moveSelect )
{
	int setmax = 1;
	
	switch ( moveSelect ) { 	
		case ePrevChar:
		case eNextChar: 		
		case ePrevCharInPara:
		case eNextCharInPara:
			SLCCharacterMove( *this, moveSelect );
			break;

		case ePrevWord:
			PrevWord( );		
			break;
		case eNextWord: 
			NextWord( );
			break;

		case ePrevSpellWord:
			PrevSpellWord( );		
			break;
		case eNextSpellWord:	
			NextSpellWord( );
			break;
		
		case eStartWord:
			StartWord( );
			break;
			
		case eEndWord:
			EndWord( );
			break;
		
		case ePrevEntireLine:
			PrevEntireLine();
			break;		
			
		case eNextEntireLine:
			NextEntireLine();
			break;

		case ePrevLine:
			PrevLine();
			setmax = 0;
			break;
			
		case eNextLine: 
			NextLine();
			setmax = 0;
			break;
			
		case eStartLine:
			StartLine();
			break;
			
		case eEndLine:
			EndLine();
			break;
			
		case ePrevPara:
		case eNextPara: 	
		case eFirstPara:		
		case eLastPara: 	
			Para( moveSelect );
			break;
			
		case eBeginPara:
			BeginPara();
			break;
		case eEndPara:
			EndPara();		
			break;
			
		case ePrevEntireColumn:
			PrevColumn();
			break;
		case eNextEntireColumn:
			NextColumn();
			break;

		case eBeginColumn:
			StartColumn( );
			break;
		case eEndColumn:
			EndColumn();
			break;
			
		case eStartStream:
			fMark.SelectStartStream();
			fPoint.SelectStartStream();
			break;

		case eEndStream:
			fMark.SelectEndStream();
			fPoint.SelectEndStream();
			break;

		default:
			SCDebugBreak();
			break;
	}
	fMark.UpdateInfo( setmax );
	fPoint.UpdateInfo( setmax );	
}

/*====================================================================*/

void scSelection::NthPara( scStream* stream, 
						   long 	 nthPara )
{
	scContUnit* p = stream->NthPara( nthPara );
	
	if ( p )	
		SetParaSelection( p, 0, p->GetContentSize() );
}

/*====================================================================*/
