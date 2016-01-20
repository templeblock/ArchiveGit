/***************************************************************************

	File:		SCSELECT.C


	$Header: /Projects/Toolbox/ct/Scselect.cpp 2	 5/30/97 8:45a Wmanis $

	Contains:	handles the selection object/id

	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

***************************************************************************/

#include "scselect.h"
#include "scpubobj.h"
#include "sccolumn.h"
#include "scglobda.h"
#include "scparagr.h"
#include "scstream.h"
#include "sctextli.h"
#include "scexcept.h"
#include "sccallbk.h"
#include "scstcach.h"

#include <limits.h>

typedef enum ePointAndMark {
	thePOINT = 1,
	theMARK
};

#ifndef HUGE_VAL
#define HUGE_VAL	HUGE
#endif

/* ==================================================================== */


#ifdef GermanHyphenation
static Bool 	SLCNextLineChar( scTextline * );
#endif


#define SLC_LOCATE				(LONG_MAX-1)

/* ==================================================================== */
/* get a list of type specs used in the selection */

void scSelection::GetTSList( scTypeSpecList& tsList )
{
	long		offset1,
				offset2;
	scContUnit* para;
	scContUnit* para1;
	scContUnit* para2;
	scContUnit* lastPara;

	scSelection aRng( *this );
	aRng.Sort( );		/* sort the selection */

	para1	= aRng.fMark.fPara;
	offset1 = aRng.fMark.fOffset;
	para2	= aRng.fPoint.fPara;
	offset2 = aRng.fPoint.fOffset;

	if ( para1 == para2 ) {
			// only one paragraph

		if (offset1 == offset2)
		{
			TypeSpec ts = para1->SpecAtOffset( offset1 );
			tsList.Insert( ts );
		}
		else
			para1->OffsetGetTSList( offset1, offset2, tsList );
	}
	else {
		lastPara = para2->GetNext( );
		for ( para = para1; para && para != lastPara; para = para->GetNext() ) {
			if ( para == para1 )
				para->OffsetGetTSList( offset1, LONG_MAX, tsList );
			else if ( para == para2 )
				para->OffsetGetTSList( 0, offset2, tsList );
			else
				para->OffsetGetTSList( 0, LONG_MAX, tsList );
		}
	}
}

/* ==================================================================== */

void scSelection::GetParaSpecList( scSpecLocList& csList )
{
	long		offset1,
				offset2;
	scContUnit* p;
	scContUnit* p1;
	scContUnit* p2;
	scContUnit* lastPara;

	scSelection aRng( *this );
	aRng.Sort( );

	p1		= aRng.fMark.fPara;
	offset1 = aRng.fMark.fOffset;
	p2		= aRng.fPoint.fPara;
	offset2 = aRng.fPoint.fOffset;

	lastPara = p2->GetNext( );
	for ( p = p1; p && p != lastPara; p = p->GetNext() ) {
		scSpecLocation specLoc( p->GetCount(), -1, p->GetDefaultSpec() );
		csList.Append( specLoc );
	}
}

/* ==================================================================== */

void scSelection::GetParaSpecList( scTypeSpecList& tsList )
{
	long		offset1,
				offset2;
	scContUnit* p;
	scContUnit* p1;
	scContUnit* p2;
	scContUnit* lastPara;

	scSelection aRng( *this );
	aRng.Sort( );

	p1		= aRng.fMark.fPara;
	offset1 = aRng.fMark.fOffset;
	p2		= aRng.fPoint.fPara;
	offset2 = aRng.fPoint.fOffset;

	lastPara = p2->GetNext( );
	for ( p = p1; p && p != lastPara; p = p->GetNext() ) {
		TypeSpec ts = p->GetDefaultSpec();
		tsList.Insert( ts );
	}
}

/* ==================================================================== */

void scSelection::GetCharSpecList( scSpecLocList& csList )
{
	long		offset1,
				offset2;
	scContUnit* p;
	scContUnit* p1;
	scContUnit* p2;
	scContUnit* lastPara;

	scSelection aRng( *this );
	aRng.Sort( );

	p1		= aRng.fMark.fPara;
	offset1 = aRng.fMark.fOffset;
	p2		= aRng.fPoint.fPara;
	offset2 = aRng.fPoint.fOffset;

	if ( p1 == p2 )
		p1->OffsetGetCharSpecList( offset1, offset2, csList );
	else {
		lastPara = p2->GetNext( );
		for ( p = p1; p && p != lastPara; p = p->GetNext() ) {
			if ( p == p1 )
				p->OffsetGetCharSpecList( offset1, LONG_MAX, csList );
			else if ( p == p2 )
				p->OffsetGetCharSpecList( 0, offset2, csList );
			else
				p->OffsetGetCharSpecList( 0, LONG_MAX, csList );
		}
	}
}

/* ==================================================================== */

void scSelection::MarkValidatedSelection( const scLayBits& mark )
{
	scSelection 	validatedSelection;

	if ( ValidateSelection( validatedSelection ) )
		validatedSelection.MarkLayoutBits( mark );
}

/* ==================================================================== */

void scSelection::ValidateHilite( HiliteFuncPtr func ) const
{
	scSelection validatedSelection;

#if SCDEBUG > 1
	DbgPrintInfo( 3 );

	static int noHilite;

	if ( noHilite )
		return;
#endif

	if ( ValidateSelection( validatedSelection ) )
		validatedSelection.LineHilite( func );
}

/* ==================================================================== */
/* invalidate the selection if the selection lies in the indicated stream */

void scSelection::Invalidate()
{
	fMark.Invalidate();
	fPoint.Invalidate();
}

/* ==================================================================== */
/* insure that we have a valid selection */

Bool scSelection::ValidateSelection( scSelection& validSelect ) const
{	
	scColumn*	col1;
	scColumn*	col2;
	scContUnit* lastPara;
	scTextline* lastTxl;
	scFlowDir	flowDir;

	col1 = fMark.fCol;
	col2 = fPoint.fCol;
	if ( col1 && col2 )
		validSelect = *this;
	else if ( col1 ) {
			// the mark lies outside the visible region

			// establish the last visible location that is visible
		lastPara			= fMark.fPara->GetLastVisiblePara();
		lastTxl 			= lastPara->GetLastVisibleLine();

		col2				= lastTxl->GetColumn();
		flowDir 			= col2->GetFlowdir();

			// mark should be ok
		validSelect.fMark			= fMark;

			// set up last visible point
		validSelect.fPoint.fPara		= lastPara;
		validSelect.fPoint.fParaCount	= lastPara->GetCount();

		validSelect.fPoint.fTxl 		= lastTxl;
		validSelect.fPoint.fLineCount	= lastTxl->GetLinecount();
		validSelect.fPoint.fOffset		= lastTxl->GetEndOffset( );

		validSelect.fPoint.fCol 		= col2;
		validSelect.fPoint.fColCount	= col2->GetCount( );
		
		
		if ( lastTxl->IsHyphenated() )
			validSelect.fPoint.fEndOfLine	= true;
		else
			validSelect.fPoint.fEndOfLine	= false;

#if 1
		validSelect.fPoint.fHLoc = LONG_MAX;
#else
		scXRect 	xRect;
		lastTxl->QueryExtents( xRect );

		if ( flowDir.IsVertical() ) 
			validSelect.fPoint.fHLoc	= xRect.y2;
		else
			validSelect.fPoint.fHLoc	= xRect.x2;
#endif
	}
	else {
		validSelect.fMark.Zero( theMARK, true );
		validSelect.fPoint.Zero( thePOINT, true );
		return false;
	}
	
	validSelect.scAssertValid();

	return true;
}

/* ==================================================================== */
/* zero out the layout portions of the text marker, the logical selection
 * remains the same but something has changed the layout structure and we
 * need to zero out that portion of the selection
 */

void TextMarker::Zero( int	type,
					   Bool zeroLogical )
{
	fCol		= 0;
	fTxl		= 0;
	fEndOfLine	= false;

	switch ( type ) {
		default:
		case thePOINT:
			fSelMaxX	= fHLoc 		= LONG_MAX;
			fColCount	= fLineCount	= LONG_MAX;
			break;
		case theMARK:
			fSelMaxX		= fHLoc 	= LONG_MIN;
			fColCount	= fLineCount	= LONG_MIN;
			break;
	}

	if ( zeroLogical ) {
		fPara		= 0;
		fParaCount	= 0;
		fOffset 	= 0;
	}
}

/* ==================================================================== */

void TextMarker::UpdateInfo( int setMax )
{
	if ( fPara ) {
		fPara->FindLocation( fOffset, fEndOfLine, fTxl, fHLoc, eCursNoMovement );

		if ( fTxl ) {
			fCol		= fTxl->GetColumn();
			fColCount	= fCol->GetCount();
			fLineCount	= fTxl->GetLinecount();
			if ( setMax )
				fSelMaxX = fHLoc;
		}
	}
	else
		Invalidate();
	
}

/* ==================================================================== */
// move the selection based upon the value of tmMove

Bool TextMarker::Update( eContentMovement	movement,
						 scColumn*			flowset )
{
	scColumn*	col;
	scTextline* txl;
	scMuPoint	mPt;
	Bool		setPosition 	= true;
	Bool		locate			= true;
	Bool		selectionMoved	= true;
	Bool		vertical		= false;
	scFlowDir	flowDir;

	if ( fTxl == 0 ) {
		fParaCount	= fPara->GetCount();

		fEndOfLine	= false;

			// this indicates previous line, lets set it to the beginning
			// of the paragraph and see what happens, if it finds a line
			// it will select it - otherwise it will simply fail and
			// zero the text marker
		if ( fOffset < 0 )
			fOffset = 0;

			// PARAFindLocation will set fEndOfLine to false if it 
			// finds it is no longer relevant (if we are in the middle 
			// of a line, for example).
		if ( !fPara->FindLocation( fOffset, fEndOfLine, fTxl, fHLoc, movement ) ) {
			if ( fPara && PARAChSize( fPara ) < fOffset )
				fOffset = PARAChSize( fPara );
			Zero( thePOINT, false );
			return false;
		}
		else {
			fCol = col	= fTxl->GetColumn();
			fColCount	= col->GetCount( );
			fLineCount	= fTxl->GetLinecount();
			fSelMaxX		= fHLoc;
			return selectionMoved;			
		}
	}

	if ( fOffset == NEXT_LINE || fOffset == PREV_LINE ) {
		if ( fOffset == NEXT_LINE )
			txl = fTxl->GetNextLogical();
		else
			txl = fTxl->GetPrevLogical();

		if ( txl ) {
			fCol = txl->GetColumn();
#if 0
			if ( fCol->GetFlowdir().IsVertical() ) {
				if ( txl->GetOrigin().y + txl->GetMeasure() <= fSelMaxX && txl->IsHyphenated() )
					fEndOfLine = true;
				else
					fEndOfLine = false;
			}
			else {
				if ( txl->GetOrigin().x + txl->GetMeasure() <= fSelMaxX && txl->IsHyphenated() )			
					fEndOfLine = true;
				else
					fEndOfLine = false;
			}
#endif
			fTxl = txl;
		}
		else
			selectionMoved = false;

		fPara	= fTxl->GetPara( );
		setPosition = false;

		flowDir = fTxl->GetFlowdir();
		vertical = flowDir.IsVertical();
		
		mPt 	= fTxl->GetOrigin();
		if ( vertical ) {
			mPt.y	= fSelMaxX;
			mPt.Translate( -fTxl->GetVJOffset(), 0 );
		}
		else {
			mPt.x	= fSelMaxX; 	
			mPt.Translate( 0, fTxl->GetVJOffset() );
		}

		scMuPoint	charOrg;
		fTxl->Select( charOrg, fOffset, mPt, movement, fEndOfLine );
		if ( vertical ) 
			fHLoc = charOrg.y;
		else
			fHLoc = charOrg.x;

		locate = false;
	}
	else if ( fOffset < 0 ) {
		/* previous paragraph */
		if ( fPara->GetPrev() ) {
			fPara	= fPara->GetPrev( );
			fOffset = PARAChSize( fPara );
		}
		else {
			fOffset 	= 0;
			selectionMoved	= false;
		}
		fEndOfLine	= false;
	}
	else if ( fOffset > PARAChSize( fPara ) ) {
		/* next paragraph */
		if ( fPara->GetNext( ) ) {
			fPara	= fPara->GetNext( );
			fOffset = 0;
		}
		else {
			fOffset 	= PARAChSize( fPara );
			selectionMoved	= false;
		}
		fEndOfLine	= false;
	}

	fParaCount	= fPara->GetCount();	

		// PARAFindLocation will set fEndOfLine to false if it
		// finds it is no longer relevant (if we are in the middle 
		// of a line, for example). 	
	if ( locate )
		fPara->FindLocation( fOffset, fEndOfLine, fTxl, fHLoc, movement );

	if ( fTxl ) {
		fCol = col	= fTxl->GetColumn();
		fColCount	= col->GetCount();
		fLineCount	= fTxl->GetLinecount();
		if ( setPosition )
			fSelMaxX = fHLoc;
	}
	else
		Zero( thePOINT, false );

	return selectionMoved;
}

/* ==================================================================== */
/* something has happened to change the selection, let us correct it */

void scSelection::UpdateSelection()
{
	if ( fMark.fPara )
		fMark.Update( eCursNoMovement, fFlowset );
	if ( fPoint.fPara )
		fPoint.Update( eCursNoMovement, fFlowset ); 	
}

/* ==================================================================== */

void scSelection::CheckFreePara( scContUnit* p )
{
}

/* ==================================================================== */

Bool scSelection::ChangeSelection( long tmMove,
								   Bool arrowKey )
{
	scTextline* 		prevLine;
	eContentMovement	cursDirect = tmMove > 0 ? eCursForward : eCursBackward;

		// the para and the offset are the only reliable values,
		// derive the others
	//scAssert( ggcS.scRecomposeActive != false );

	/* We use if-else instead of switch() because K&R requires */
	/* the switch() expression to be an ``int'', not a ``long''. */
	/* And oh yes, Microsoft C 6.0 generates buggy code for such a switch() */
	/* even though it claims to be ANSI-conformant! */

	if ( !arrowKey ) {
		if ( tmMove != SLC_LOCATE ) {
			fMark.fOffset	+= tmMove;
			fPoint.fOffset	+= tmMove;
		}
	}
	else {
		switch ( tmMove ) {
			case NEXT_LINE:
			case PREV_LINE:
				fMark.fOffset	= tmMove;
				fPoint.fOffset	= tmMove;
				break;
			case -1:
				if ( fMark.fTxl 									&& 
					 fMark.fOffset == fMark.fTxl->GetStartOffset( ) && 
					 ( ( prevLine = LNPrev( fMark.fTxl ) ) != 0 )	&& 
					 prevLine->IsHyphenated() ) {
					fMark.fEndOfLine	= true;
					fPoint.fEndOfLine	= true;
				}
				else {
					fMark.fEndOfLine	= false;
					fPoint.fEndOfLine	= false;
					fMark.fOffset--;
					fPoint.fOffset--;
				}
				break;
			case 1:
				if ( fMark.fEndOfLine ) {
					fMark.fEndOfLine	= false;
					fPoint.fEndOfLine	= false;
				}
				else {
					fMark.fOffset++;
					fPoint.fOffset++;
					if ( fMark.fTxl 									&& 
						 fMark.fOffset == fMark.fTxl->GetEndOffset()	&& 
						 fMark.fTxl->IsHyphenated() ) {
						fMark.fEndOfLine	= true;
						fPoint.fEndOfLine	= true;
					}
				}
				break;
		}
	}
//	SCASSERT ( select->fMark.fPara == select->fPoint.fPara );	
	fMark.Update( cursDirect, GetFlowset() );
	return fPoint.Update( cursDirect, GetFlowset() );
}

/* ==================================================================== */
/* Returns true if a German hyphenation spelling change also affects	*/
/* the following text line. 											*/

#ifdef GermanHyphenation

static Bool SLCNextLineChar( scTextline* txl )
{
	Bool		result;

	result	= txl->fNextLineChar;

	return result;
}

#endif

/* ==================================================================== */

#if SCDEBUG > 1

/* ==================================================================== */

void TextMarker::DbgPrintInfo( int debugLevel ) const
{
	SCDebugTrace( debugLevel, scString( "fCol 0x%08x fPara 0x%08x fTxl 0x%08x\n" ),
				  fCol, fPara, fTxl );
	SCDebugTrace( debugLevel, scString( "fColCount %d fParaCount %d fLineCount %d fOffset %d\n" ),
				  fColCount, fParaCount, fLineCount, fOffset );
}

/* ==================================================================== */

void TextMarker::scAssertValid()
{
	if ( fCol )
		fCol->scAssertValid();
	if ( fPara )
		fPara->scAssertValid();
	if ( fTxl )
		fTxl->scAssertValid();
}

/* ==================================================================== */

void scSelection::scAssertValid()
{
	fMark.scAssertValid();
	fPoint.scAssertValid();
}

/* ==================================================================== */

void scSelection::DbgPrintInfo( int debugLevel ) const
{
	SCDebugTrace( debugLevel, scString( "scSelection: \n" ) );
	fMark.DbgPrintInfo( debugLevel );
	fPoint.DbgPrintInfo( debugLevel );
}

/* ==================================================================== */

#endif

/* ==================================================================== */

int TextMarker::operator>( const TextMarker& tm ) const
{
	if ( fParaCount > tm.fParaCount )
		return true;
	else if ( fParaCount == tm.fParaCount )
		return fOffset > tm.fOffset;
	return false;
}

/* ==================================================================== */

int TextMarker::operator<( const TextMarker& tm ) const
{
	if ( fParaCount < tm.fParaCount )
		return true;
	else if ( fParaCount == tm.fParaCount )
		return fOffset < tm.fOffset;
	return false;
}

/* ==================================================================== */

int TextMarker::operator>=( const TextMarker& tm ) const
{
	if ( fParaCount > tm.fParaCount )
		return true;
	else if ( fParaCount == tm.fParaCount )
		return fOffset >= tm.fOffset;
	return false;
}

/* ==================================================================== */

int TextMarker::operator<=( const TextMarker& tm ) const
{
	if ( fParaCount < tm.fParaCount )
		return true;
	else if ( fParaCount == tm.fParaCount )
		return fOffset <= tm.fOffset;
	return false;
}

/* ==================================================================== */

int TextMarker::operator==( const TextMarker& tm ) const
{
	return fParaCount == tm.fParaCount && fOffset == tm.fOffset;
}
	
/* ==================================================================== */

int TextMarker::operator!=( const TextMarker& tm ) const
{
	return fParaCount != tm.fParaCount || fOffset != tm.fOffset;
}
	
/* ==================================================================== */

// sort the selection so that the mark appears before the point

void scSelection::Sort()
{
	if ( fMark <= fPoint )
		return;

	TextMarker temp( fMark );
	
	fMark	= fPoint;
	fPoint	= temp;
}

/* ==================================================================== */
/* take the current selection and convert it into a word selection */

void scSelection::WordSelect( void )
{
	fMark.fPara->SelectWord( fMark.fOffset, fMark.fOffset, fPoint.fOffset );
	ChangeSelection( SLC_LOCATE, false );
}

/*====================================================================*/
/* take the current selection and convert it into a line selection */

void scSelection::LineSelect( void )
{
	scTextline* txl;

	ChangeSelection( SLC_LOCATE, false );
	txl = fMark.fTxl;

	if ( txl ) {
		fMark.fOffset	= txl->GetStartOffset( );
		fPoint.fOffset	= txl->GetEndOffset( );
		if (
#ifdef GermanHyphenation
			txl->IsHyphenated() && ! SLCNextLineChar( txlH )
#else
			txl->IsHyphenated()
#endif
		)
			fPoint.fEndOfLine = true;
		else
			fPoint.fEndOfLine = false;

		ChangeSelection( SLC_LOCATE, false );
	}
}

/* ==================================================================== */
/* take the current selection and convert it into a para selection */

void scSelection::ParaSelect( void )
{
	scContUnit* para;

	ChangeSelection( SLC_LOCATE, false );
	para = fMark.fPara;

	if ( para ) {
		fMark.fOffset		= 0;
		fPoint.fOffset		= PARAChSize( para );
		fMark.fEndOfLine	= false;
		fPoint.fEndOfLine	= false;

		ChangeSelection( SLC_LOCATE, false );
	}
}

/* ==================================================================== */
/* take the current selection and convert it into a column selection */

void scSelection::ColumnSelect( void )
{
	ChangeSelection( SLC_LOCATE, false );

	if ( fMark.fCol ) {
		scTextline* firstline	= fMark.fCol->GetFirstline();
		scTextline* lastline	= fMark.fCol->GetLastline();	
	
		fMark.fPara 			= firstline->GetPara();
		fMark.fOffset			= firstline->GetStartOffset();
		fMark.fEndOfLine		= false;

		fPoint.fPara				= lastline->GetPara();
		fPoint.fOffset			= lastline->GetEndOffset();
		fPoint.fEndOfLine		= true;

		ChangeSelection( SLC_LOCATE, false );
	}
}

/* ==================================================================== */

/* take the current selection and convert it into a selection of the 
 * entire stream
 */

void scSelection::AllSelect( void )
{
	scStream	*stream;
	scContUnit	*lastPara;

	if ( fMark.fCol ) {
		stream			= fMark.fCol->GetStream();
		lastPara		= stream->Last( );
		if ( stream && lastPara ) {
			fMark.fPara 		= stream->First();
			fMark.fOffset		= 0;
			fPoint.fPara		= lastPara;
			fPoint.fOffset		= PARAChSize( lastPara );
			fMark.fEndOfLine	= false;
			fPoint.fEndOfLine	= false;

			ChangeSelection( SLC_LOCATE, false );
		}
	}
}

/* ==================================================================== */
/* hilite the selection range */

void scSelection::LineHilite( HiliteFuncPtr func )
{
	scColumn*	col1;
	scColumn*	col2;
	scTextline* txl1;
	scTextline* txl2;

	scColumn*	col;
	scTextline* txl;

	APPDrwCtx	drawCtx;
 
	scAssertValid();

	scSelection cSrtRng( *this );
	cSrtRng.Sort( );

	col1 = cSrtRng.fMark.fCol;
	col2 = cSrtRng.fPoint.fCol;
	txl1 = cSrtRng.fMark.fTxl;
	txl2 = cSrtRng.fPoint.fTxl;

#if SCDEBUG > 1
	cSrtRng.DbgPrintInfo( 3 );
#endif

	if ( col1 && col2 && txl1 && txl2 ) {

		if ( txl1 == txl2 ) {
			APPDrawContext( col1->GetAPPName(), col1, drawCtx );													
			txl1->Hilite( &cSrtRng.fMark, cSrtRng.fMark.fHLoc, &cSrtRng.fPoint, cSrtRng.fPoint.fHLoc, drawCtx, func, cSrtRng );
		}
		else if ( col1 == col2 ) {
			col1->Hilite( cSrtRng.fMark, cSrtRng.fPoint, func, cSrtRng );
		}
		else {
			for ( col = col1; col && col != col2->GetNext(); col = col->GetNext() ) {
				if ( col == col1 ) {
					APPDrawContext( col1->GetAPPName(), col1, drawCtx );										
					txl1->Hilite( &cSrtRng.fMark, cSrtRng.fMark.fHLoc, NULL, LONG_MAX, drawCtx, func, cSrtRng );
					for ( txl = LNNext(txl1); txl; txl = LNNext( txl ) )
						txl->Hilite( NULL, LONG_MIN, NULL, LONG_MAX, drawCtx, func, cSrtRng );
				}
				else if ( col == col2 ) {
					APPDrawContext( col2->GetAPPName(), col2, drawCtx );					
					for ( txl = col2->GetFirstline(); txl && txl != txl2; txl = LNNext( txl ) )
						txl->Hilite( NULL, LONG_MIN, NULL, LONG_MAX, drawCtx, func, cSrtRng );
					txl2->Hilite( NULL, LONG_MIN, &cSrtRng.fPoint, cSrtRng.fPoint.fHLoc, drawCtx, func, cSrtRng );
				}
				else {
					APPDrawContext( col->GetAPPName(), col, drawCtx );
					for ( txl = col->GetFirstline(); txl; txl = LNNext( txl ) )
						txl->Hilite( NULL, LONG_MIN, NULL, LONG_MAX, drawCtx, func, cSrtRng );
				}
			}
		}
	}
}

/* ==================================================================== */
/* mark all the lines of a selection range with the passed in bit */

void scSelection::MarkLayoutBits( const scLayBits& mark )
{
	scColumn*	c1;
	scColumn*	c2;
	scColumn*	c;
	scTextline* t1;
	scTextline* t2;
	scTextline* t;
	
	scSelection cSrtRng( *this );		/* current sorted range */
	cSrtRng.Sort( );

	
	c1 = cSrtRng.fMark.fCol;
	c2 = cSrtRng.fPoint.fCol;
	t1 = cSrtRng.fMark.fTxl;
	t2 = cSrtRng.fPoint.fTxl;
	
	if ( t1 == t2 )
		t1->Mark( mark );
	else if ( c1 == c2 ) {
		for ( ; t1 && t1 != LNNext(t2); t1 = LNNext(t1) )
			t1->Mark( mark );
	}
	else {
		for ( c = c1; c && c != c2->GetNext(); c = c->GetNext() ) {
			if ( c == c1 ) {
				t1->Mark( mark );
				for ( t = LNNext( t1 ); t; t = LNNext( t ) )
					t->Mark( mark );
			}
			else if ( c == c2 ) {
				for ( t = c2->GetFirstline(); t && t != LNNext(t2); t = LNNext( t ) )
					t->Mark( mark );
			}
			else {
				for ( t = c->GetFirstline(); t; t = LNNext( t ) )
					t->Mark( mark );
			}
		}
	}
}

/* ==================================================================== */
/* called from a continue click or after a appmouse down,
 * hilite the new area
 */

void scSelection::InteractiveHilite( scSelection&	prevRange,
									 HiliteFuncPtr	func )
{
	scSelection 	cSrtRng( *this );		/* current sorted range 	*/
	scSelection 	pSrtRng( prevRange );	/* previous sorted range	*/
	APPDrwCtx		drawCtx;

	if ( pSrtRng == cSrtRng )
		return;

	pSrtRng.Sort( );
	cSrtRng.Sort( );

	if ( pSrtRng.fMark.fTxl == pSrtRng.fPoint.fTxl ) {
			// previous selection was one line
			// turn off old hiliting
		APPDrawContext( pSrtRng.fPoint.fCol->GetAPPName(), pSrtRng.fPoint.fCol, drawCtx );
		pSrtRng.fPoint.fTxl->Hilite( &pSrtRng.fPoint, pSrtRng.fPoint.fHLoc, 
									 &pSrtRng.fMark, pSrtRng.fMark.fHLoc, 
									 drawCtx, func, pSrtRng );
				
			/* turn on new hiliting  */ 			
		LineHilite( func );
	}
	else if ( pSrtRng.fMark.fTxl==cSrtRng.fPoint.fTxl 
						|| pSrtRng.fPoint.fTxl==cSrtRng.fMark.fTxl ) {
		/* prev selection was behind mark and new selection is past mark */

			/* turn off old hiliting */ 					
		prevRange.LineHilite( func );
		
			/* turn on new hiliting  */ 	
		LineHilite( func );
	}
	else if ( prevRange.fPoint.fTxl == pSrtRng.fPoint.fTxl ) {
			/* Prev selection more than one line, and we haven't flipped it.
			 * Thus, the point succeeds the mark in both the previous and 
			 * current selections.	
			 */

			/* dehilite the line we were on */
		APPDrawContext( pSrtRng.fPoint.fCol->GetAPPName(), pSrtRng.fPoint.fCol, drawCtx );		
		pSrtRng.fPoint.fTxl->Hilite( NULL, LONG_MIN, &pSrtRng.fPoint, pSrtRng.fPoint.fHLoc, drawCtx, func, pSrtRng );

			/* we want to hilite from the old point to the new point,
			 * including the line containing the old point
			 */
		cSrtRng.fMark		= prevRange.fPoint;
		cSrtRng.fMark.fHLoc = LONG_MIN;
		cSrtRng.fPoint		= fPoint;

		switch ( cSrtRng.RangeMovement( ) ) {		
			case eBeforeSelect: /* Backing up, dehiliting bottom line(s)	*/
				cSrtRng.fPoint.fHLoc	= LONG_MIN;
				cSrtRng.LineHilite( func );
				
				APPDrawContext( fPoint.fCol->GetAPPName( ), fPoint.fCol, drawCtx );
				fPoint.fTxl->Hilite( NULL, LONG_MIN, 
									 &fPoint, fPoint.fHLoc, 
									 drawCtx, func, cSrtRng );
				break;
			case eEqualSelect:	/* Neither adding nor subtracting lines.	*/
			case eAfterSelect:	/* Adding line(s) to bottom of selection	*/
				cSrtRng.LineHilite( func );
				break;
		}
	}
	else if ( prevRange.fPoint.fTxl == pSrtRng.fMark.fTxl ) {
			/* Previous selection more than one line, and we haven't 
			 * flipped it. But point precedes the mark in both the 
			 * previous and current selections.
			 */

			/* dehilite the line we were on */
		APPDrawContext( pSrtRng.fMark.fCol->GetAPPName( ), pSrtRng.fMark.fCol, drawCtx );		
		pSrtRng.fMark.fTxl->Hilite( &pSrtRng.fMark, pSrtRng.fMark.fHLoc, 
									NULL, LONG_MAX, 
									drawCtx, func, pSrtRng );

			/* Start where old range left off.	*/					
		cSrtRng.fMark		= prevRange.fPoint;
		
			/* Extend to end of this first line.*/		
		cSrtRng.fMark.fHLoc = LONG_MAX;

			/* Point is unchanged.				*/		
		cSrtRng.fPoint		= fPoint;

		switch ( cSrtRng.RangeMovement( ) ) {		
			case eBeforeSelect: 
					/* Extending line(s) from beginning of selection	*/
			case eEqualSelect:	
					/* Neither adding nor subtracting lines.			*/
				cSrtRng.LineHilite( func ); 
				break;
			case eAfterSelect:
					/* Subtracting line(s) from beginning of selection	*/
				cSrtRng.fPoint.fHLoc	= LONG_MAX;
				cSrtRng.LineHilite( func );
				APPDrawContext( fPoint.fCol->GetAPPName( ), fPoint.fCol, drawCtx );
				fPoint.fTxl->Hilite( NULL, LONG_MAX, 
									 &fPoint, fPoint.fHLoc,
									 drawCtx,
									 func, cSrtRng );
				break;
		}
	}
}

/* ==================================================================== */
/* Determine how this interactive selection is moving.	*/
/* This is only called by SLCInteractiveHilite. 		*/
/* THIS ASSUMES MARK AND POINT ARE ON DIFFERENT LINES.	*/

eSelectMovement scSelection::RangeMovement( ) const
{
	if ( fMark.fParaCount > fPoint.fParaCount )
		return eBeforeSelect;
	else if ( fMark.fParaCount == fPoint.fParaCount ) {
		if ( fMark.fOffset > fPoint.fOffset )
			return eBeforeSelect;
		else if ( fMark.fOffset == fPoint.fOffset )
			return eEqualSelect;
	}

	return eAfterSelect;
}

/* ==================================================================== */

static eRefEvent SLCRecomposeLogical( scColumn* 	firstCol,
									  scContUnit*	firstPara,
									  scRedispList* redispList,
									  scSelection*	aRng,
									  short 		rebreak )
{
	scTextline* txl;
	scContUnit* prevPara;

	/* the first column may be NULL if the selection were outside the
	 * visible world, the SLCChangeSelection should may have put us back into
	 * the visible world so let's check, otherwise no reformatting will
	 * take place, there may be conditions in this type of situtation
	 * that may create anomalies, I don't know what they are as of yet
	 */

	if ( firstCol == 0 ) {
		/* there is a likelyhood here that we will
		 * have to do nothing, since we may be out
		 * of visible range, but then again this may
		 * be forcing us back in so this may be tricky
		 * 
		 * if the current paragraph doesn't have a line
		 * lets check the previous paragraph to see if it
		 * has a line - if it does we can start with that
		 * column
		 */
		 
		firstCol = aRng->fPoint.fCol;
		if ( firstCol == 0 ) {	
			if ( aRng->fPoint.fPara ) {
				txl = aRng->fPoint.fPara->GetFirstline();
				if ( txl == 0 ) {
					if ( ( prevPara = aRng->fPoint.fPara->GetPrev() ) != 0)
						txl = prevPara->GetFirstline();
					if ( !txl )
						return eNoReformat;
				}
				firstCol = txl->GetColumn();				
			}
			else
				return eNoReformat; /* !! */
		}
	}

	if ( rebreak && firstPara )
		return STRReformat( firstCol, firstPara, scInteractiveTimeSlice, redispList );
	return eNoReformat;
}

/* ==================================================================== */

static int GetOffsetChange( short			keyCount,
							scKeyRecord*	keyRecords )
{
	int offset = 0;

	for ( ; keyCount--; )
		offset += CMcontent( keyRecords[keyCount].keycode() );
	return offset;
}

/* ==================================================================== */

void scSelection::KeyArray( short			keyCount,
							scKeyRecord*	keyRecords,
							scRedispList*	redispList )
{
	scSelection 		aRng( *this );
	scColumn*			firstColPara;
	scColumn*			firstColCursor;
	scContUnit* 		para			= 0;
	scContUnit* 		firstPara;			/* para to start reformating at */
	scStreamLocation	dummyPoint;
	long				tmMove;
	register			count;
	short				rebreak 		= false;
	Bool				textCleared 	= false;
	Bool				selectionMoved;
	Bool				savedEndOfLine;
	scImmediateRedisp	immediateRedisp;
	TypeSpec			clearedSpec;

	aRng.Sort( );

	if ( aRng.fMark.fPara->GetFirstline( ) )
		firstColPara = aRng.fMark.fPara->GetFirstCol();
	else 
		firstColPara = aRng.fMark.fCol;
	
	firstColCursor = aRng.fMark.fCol;
	
		// store what line we started on
	if ( firstColCursor ) {
		immediateRedisp.fStartLine	= COLLineNum( &aRng );
		immediateRedisp.fStopLine	= immediateRedisp.fStartLine;
		immediateRedisp.fImmediateRect.Set( 0, 0, 0, 0 );
		firstColCursor->LineExtents( immediateRedisp );
		gStreamChangeInfo.Set( firstColCursor, aRng.fMark.fPara, aRng.fPoint.fOffset, GetOffsetChange( keyCount, keyRecords ) );
	}
	else {
		immediateRedisp.fStartLine = -1;
		gStreamChangeInfo.Set( 0, 0, 0, 0 );
	}		

		//
		// if the selection is multiple chars clear it, it will be the 
		// app's responsibility to clear this ahead of time for undoing 
		//
	if ( aRng.fMark.fPara != aRng.fPoint.fPara || aRng.fMark.fOffset != aRng.fPoint.fOffset ) {
		if ( CMcontent( keyRecords[0].keycode() ) ) {

			#if SCDEBUG > 1
				{
					static int doit = 0;
					if ( doit ) {
						aRng.fMark.fPara->GetSpecRun().PrintRun( "scSelection::KeyArray" );
						SCDebugTrace( 0, "offsets %d %d\n", 
									  aRng.fMark.fOffset, aRng.fPoint.fOffset ); 
					}
				}
			#endif

			clearedSpec = aRng.fMark.fPara->SpecAtOffset( aRng.fMark.fOffset + 1 );
			aRng.ClearText( redispList, true );
			textCleared = true;

			#if SCDEBUG > 1
				{
					static int doit;
					if ( doit )
						aRng.fMark.fPara->GetSpecRun().PrintRun( "scSelection::KeyArray 2" );
				}
			#endif

		}
		else {
			switch ( keyRecords[0].keycode() ) {
				case scLeftArrow:
				case scUpArrow:
					break;
				case scRightArrow:
				case scDownArrow:
					aRng.fMark = aRng.fPoint;
					break;
			}
		}
	}

	firstPara		= 0;

		// Remember this in case we add characters 
		// right at the hyphen point.				
	savedEndOfLine	= aRng.fMark.fEndOfLine;

		// walk down the array clearing text
	for ( count = 0; keyCount--; count++ ) {

		if ( keyRecords[count].noop() )
			continue;

		if ( ! CMcontent( keyRecords[count].keycode() ) ) {

			if (rebreak ) {
				SLCRecomposeLogical( firstColPara, firstPara, redispList, &aRng, rebreak );
				firstPara = 0;
			}

			if ( keyRecords[count].restoreselect() ) {
				Restore( &keyRecords[count].mark(), &keyRecords[count].mark(), 0, false );
				aRng = *this;
				aRng.Sort( );
				continue;
			}
			else {
				if ( rebreak ) {
						// Restore end of line setting after adding 
						// characters in case we are still at the end 
						// of a hyphenated line. If we aren't, 
						// SLCChangeSelection will reset fEndOfLine to false 
						// when it calls SLCSetTextMarker, which calls 
						// ParaFindLocation.				
						//
					aRng.fMark.fEndOfLine = aRng.fPoint.fEndOfLine = savedEndOfLine;
					aRng.ChangeSelection( SLC_LOCATE, false );
					savedEndOfLine	= aRng.fMark.fEndOfLine;
				}
				aRng.Decompose( keyRecords[count].mark(), dummyPoint );
				keyRecords[count].restoreselect() = true;
			}
			rebreak = false;
		}			/* at end of hyphenated line				*/

		if ( para == firstPara && aRng.fMark.fOffset == 0 && keyRecords[count].keycode() == scBackSpace ) {
				// we are going to merge with previous para
			firstPara = 0;
		}
			// if key is backspace at start of stream, this will 
			// set noOp field of key record 
			//
		TextMarker savedPosition = aRng.fMark;

		para = aRng.fMark.fPara;
		Bool iAmRemoved = false;
		
		aRng.fMark.fPara = para->KeyInsert( aRng.fMark.fOffset, 
											 keyRecords[count],
											 tmMove,
											 rebreak,
											 textCleared,
											 clearedSpec,
											 iAmRemoved );
		if ( iAmRemoved )
			para = 0;

		firstPara = aRng.fMark.fPara->Earlier( firstPara ? firstPara : para );
		aRng.fPoint = aRng.fMark;


		if ( !keyRecords[count].restoreselect() ) {
			if ( CMcontent( keyRecords[count].keycode() ) ) {
				selectionMoved	= aRng.ChangeSelection( tmMove, false );
			}
			else {
				selectionMoved	= aRng.ChangeSelection( tmMove, true );
				if ( !selectionMoved )
					keyRecords[count].noop() = true;
			}
		}
		else {
			aRng.fPoint = aRng.fMark = savedPosition;
		}
	}

	aRng.fPoint = aRng.fMark;

	eRefEvent refevent = SLCRecomposeLogical( firstColPara, firstPara, redispList, &aRng, rebreak );
//	if ( refevent == eNoReformat )
//		throw( scNoReformat );

//	if ( rebreak && firstPara )
//		firstPara->scAssertValid();

	if ( rebreak ) {
			//
			// Restore end of line setting after adding characters in case
			// we are still at the end of a hyphenated line. If we aren't,
			// SLCChangeSelection will reset fEndOfLine to false when it calls
			// SLCSetTextMarker, which calls ParaFindLocation.
			//
		aRng.fMark.fEndOfLine = aRng.fPoint.fEndOfLine = savedEndOfLine;
		aRng.ChangeSelection( SLC_LOCATE, false );
	}

		// set up data for immediate redisplay
	immediateRedisp.fStopLine = COLLineNum( &aRng );	
	if ( firstColCursor != aRng.fMark.fCol ) 
		immediateRedisp.fStartLine = immediateRedisp.fStopLine;

	if ( immediateRedisp.fStopLine < immediateRedisp.fStartLine ) {
		short tmp					= immediateRedisp.fStartLine;
		immediateRedisp.fStartLine	= immediateRedisp.fStopLine;
		immediateRedisp.fStopLine	= tmp;
	}

	if ( aRng.fMark.fCol && rebreak ) {
		aRng.fMark.fCol->LineExtents( immediateRedisp );
		if ( redispList )
			redispList->SetImmediateRect( aRng.fMark.fCol, immediateRedisp );
	}
	
	if ( firstColCursor && firstColCursor->MoreText( ) ) {
				//
				// this is to insure that if the addition of a character 
				// overflowed the box, but caused no damage we would 
				// report back that
				// there was more text in the column 
				//	
			//stat = COLColRectAdd( firstColCursor, "SLCKeyArray2" );
	}
	else if ( !firstColCursor && aRng.fMark.fCol ) {			
				//
				// this insures that if we transition from being in no 
				// container to being in a container and there is no 
				// damage we will report accurately more text
				//
			//stat = COLColRectAdd( aRng.fMark.fCol, "SLCKeyArray2" );
	}
	
	gStreamChangeInfo.Set( 0, 0, 0, 0 );
	*this = aRng;
}

/* ==================================================================== */

void scSelection::InsertField( const clField&	field,
							   TypeSpec&		spec,
							   scRedispList*	redisp )
{
	CharRecord ch;
	ch.character	= scField;
	ch.flags.SetField( field.id() );

	scContUnit* para = fMark.fPara;
	para->Insert( ch, spec, fMark.fOffset );

	scColumn*	col;
	if ( para->GetFirstline() )
		col = para->GetFirstCol();
	else 
		col = fMark.fCol;

	STRReformat( col, para, scReformatTimeSlice, redisp );
	MoveSelect( eNextChar );
}

/* ==================================================================== */

void scSelection::SetStyle( TypeSpec		style,
							scRedispList*	redispList )
{
	scColumn*	col1;
	long		offset1,
				offset2;
	scContUnit* para;
	scContUnit* para1;
	scContUnit* para2;
	scContUnit* lastPara;

	scSelection aRng( *this );
	aRng.Sort( );

	if ( aRng.fMark.fPara->GetFirstline() )
		col1 = aRng.fMark.fPara->GetFirstCol();
	else 
		col1 = aRng.fMark.fCol;

	para1	= aRng.fMark.fPara;
	offset1 = aRng.fMark.fOffset;
	para2	= aRng.fPoint.fPara;
	offset2 = aRng.fPoint.fOffset;

		// force repaint of range
	aRng.MarkValidatedSelection( scREPAINT );

	if ( para1 && para2 ) {
		if ( para1 == para2 )
			para1->SetStyle( offset1, offset2, style, true, false );
		else {
			lastPara = para2->GetNext( );
			for (para = para1; para && para != lastPara; para = para->GetNext( ) ) {
				if (para == para1 )
					para->SetStyle( offset1, LONG_MAX, style, true, false );
				else if ( para == para2 )
					para->SetStyle( 0, offset2, style, true, false );
				else
					para->SetStyle( 0, LONG_MAX, style, true, false );
			}
		}
	}


	if ( para1 )
		STRReformat( col1, para1, scReformatTimeSlice, redispList );

	ChangeSelection( SLC_LOCATE, false );
}

/* ==================================================================== */
/* apply the character transformation to the selection and return the 
 * damage
 */
 
void scSelection::TextTrans( eChTranType	trans,
							 int			numChars,
							 scRedispList*	redispList )
{
	scSelection aRng( *this );
	scColumn		*col1H;
	long		offset1,
				offset2;
	scContUnit	*para,
				*para1H,
				*para2H,
				*lastPara;

		// set my world up
	aRng.Sort( );

	if ( aRng.fMark.fPara->GetFirstline() )
		col1H = aRng.fMark.fPara->GetFirstCol();
	else 
		col1H = aRng.fMark.fCol;

	para1H		= aRng.fMark.fPara;
	offset1 	= aRng.fMark.fOffset;
	para2H		= aRng.fPoint.fPara;
	offset2 	= aRng.fPoint.fOffset;
	
	/* walk thru the paras transforming them */
	if ( para1H && para2H ) {
		if ( para1H == para2H )
			para1H->TextTrans( offset1, offset2, trans, numChars );
		else {
			lastPara = para2H->GetNext( );
			
			for ( para = para1H; 
						para && para != lastPara; 
						para = para->GetNext() ) {
							
				if ( para == para1H )
					para->TextTrans( offset1, LONG_MAX, trans, numChars );
				else if ( para == para2H )
					para->TextTrans( LONG_MIN, offset2, trans, numChars );
				else
					para->TextTrans( LONG_MIN, LONG_MAX, trans, numChars );
			}
		}
	}

	/* force repaint of range */
	aRng.MarkValidatedSelection( scREPAINT );

	/* reformat */
	if ( para1H )
		STRReformat( col1H, para1H, scReformatTimeSlice, redispList );

	/* correct selection */
	ChangeSelection( SLC_LOCATE, false );
}

/* ==================================================================== */
#ifdef _RUBI_SUPPORT

Bool scSelection::GetAnnotation( int			nth, 
								 scAnnotation&	annotation )
{
	scSelection aRng( *this );
	scColumn		*col1H;
	long		offset1,
				offset2;
	scContUnit	*p1,
				*p2;

	aRng.Sort( );
	col1H		= aRng.fMark.fCol;

	p1			= aRng.fMark.fPara;
	offset1 	= aRng.fMark.fOffset;
	p2			= aRng.fPoint.fPara;
	offset2 	= aRng.fPoint.fOffset;

	scAssert( p1 == p2 );

	return p1->GetAnnotation( nth, offset1, offset2, annotation );
}						 

/* ==================================================================== */

void scSelection::ApplyAnnotation( const scAnnotation&	annotation, 
								   scRedispList*		redispList )
{
	scSelection aRng( *this );
	scColumn*	col1H;
	long		offset1,
				offset2;
	scContUnit* para1H;
	scContUnit* para2H;

	aRng.Sort( );
	col1H		= aRng.fMark.fCol;

	para1H		= aRng.fMark.fPara;
	offset1 	= aRng.fMark.fOffset;
	para2H		= aRng.fPoint.fPara;
	offset2 	= aRng.fPoint.fOffset;

	scAssert( para1H == para2H );

	para1H->ApplyAnnotation( offset1, offset2, annotation );
	
	STRReformat( col1H, para1H, scReformatTimeSlice, redispList );
	ChangeSelection( SLC_LOCATE, false );	
}						  

#endif

/* ==================================================================== */
/* correct the selection, something probably has changed and we need to
 * correct the state contained in the selection
 */

void scSelection::CorrectSelection( scContUnit* para1,
									long		,
									scContUnit* para2,
									long		offset2 )
{
	/* we will end up blasting this paragraph so lets take
	 * care of the selection range
	 */
	if ( offset2 < PARAChSize( para2 ) ) {
		fMark.fPara 		= para1;
		fMark.fOffset		= 0;
		fMark.fEndOfLine	= false;
	}

	fPoint = fMark;
}

/* ==================================================================== */
/* cut the text contained within the selection */

/* cut the text contained within the selection */

void scSelection::CutText( scScrapPtr&		scrap,
						   scRedispList*	redispList )
{
	CopyText( scrap );
	ClearText( redispList, true );
}

/* ==================================================================== */
/* clear any text contained within the selection */

void scSelection::ClearText( scRedispList*	redispList,
							 Bool			repair )
{
	scColumn*	firstCol;
	scContUnit* firstPara;
	scContUnit* para;
	scContUnit* para1;
	scContUnit* para2;
	scContUnit* nextPara;
	scContUnit* lastPara;
	long		offset1,
				offset2;
	Bool		merge;

	scSelection aRng( *this );
	aRng.Sort( );

	if ( aRng.fMark.fPara->GetFirstline() )
		firstCol = aRng.fMark.fPara->GetFirstCol();
	else 
		firstCol = aRng.fMark.fCol;
	
	if ( aRng.fMark.fPara == aRng.fPoint.fPara ) {
		firstPara = aRng.fMark.fPara;
		 aRng.fMark.fPara->ClearText( aRng.fMark.fOffset, aRng.fPoint.fOffset );
	}
	else {
		para1	= aRng.fMark.fPara;
		offset1 = aRng.fMark.fOffset;
		para2	= aRng.fPoint.fPara;
		offset2 = aRng.fPoint.fOffset;

		/* if there will be text left over, then we will 
		 * merge the paragraphs 
		 */
		merge = ( offset1 && offset2 < PARAChSize( para2 ) );

		lastPara = para2->GetNext( );
		for ( para = para1; para && para != lastPara; para = nextPara ) {
			nextPara = para->GetNext( );
			if ( para == para1 ) {
				if ( offset1 == 0 ) {
					aRng.CorrectSelection( para1, offset1, para2, offset2 );
					merge = true;
				}
				para->ClearText( offset1, LONG_MAX );
			}
			else if ( para == para2 ) {
				if ( para->ClearText( LONG_MIN, offset2 ) ) {
					para->Unlink( );
					para->Free( );
					merge = false;
				}
			}
			else {
				para->ClearText( LONG_MIN, LONG_MAX );
				para->Unlink( );
				para->Free( );
			}
		}
		
		firstPara = para1;
		para1->Renumber();

		/* after this the paragraphs in the selection may be invalid */

		if ( merge ) {
			offset1 = 0;
			para2 = para2->Merge( offset1 );
		}
	}

	aRng.fPoint = aRng.fMark;
	*this = aRng;

	if ( repair ) {
		SLCRecomposeLogical( firstCol, firstPara, redispList, &aRng, true );
		ChangeSelection( SLC_LOCATE, false );
	}
}

/* ==================================================================== */
/* copy the text contained with the selection */

void scSelection::CopyText( scScrapPtr& scrap )
{
	scContUnit	*para,
				*para1,
				*para2,
				*lastPara;
	long		offset1,
				offset2;

	try {
		scSelection aRng( *this );
		aRng.Sort( );

		if ( aRng.fMark.fPara &&  aRng.fPoint.fPara ) {
			if ( aRng.fMark.fPara == aRng.fPoint.fPara )
				scrap = aRng.fMark.fPara->CopyText( aRng.fMark.fOffset, aRng.fPoint.fOffset );
			else {
				para1	= aRng.fMark.fPara;
				offset1 = aRng.fMark.fOffset;
				para2	= aRng.fPoint.fPara;
				offset2 = aRng.fPoint.fOffset;

				lastPara = para2->GetNext( );
				for ( para = para1; para && para != lastPara; para = para->GetNext() ) {
					if ( para == para1 )
						scrap = para->CopyText( offset1, LONG_MAX );
					else if ( para == para2 )
						((scContUnit*)scrap)->Append( para->CopyText( LONG_MIN, offset2 ) );
					else
						((scContUnit*)scrap)->Append( para->CopyText( LONG_MIN, LONG_MAX ) );
				}
			}
		}
	}
	catch( ... ) {
		((scStream*)scrap)->STRFree(), scrap = 0;
		throw;
	} 
}

/* ==================================================================== */
/* paste text into the selection applying the indicated style */

void scSelection::PasteText( const scStream*	scrap,
							 TypeSpec			style,
							 scRedispList*		redisplist )
{
	scColumn*	firstCol	= fMark.fCol;
	scContUnit* para;
	scStream*	scrapCopy;

	if ( fMark.fPara->GetFirstline() )
		firstCol = fMark.fPara->GetFirstCol();
	else 
		firstCol = fMark.fCol;

	if ( fMark != fPoint ) 
		ClearText( redisplist, false );
	scAssert( fMark == fPoint );

	/* the pasted text may have no spec associated with it, we need to
	 * associate a spec with it and to retabluate if necessary
	 */

	scrap->STRCopy( scrapCopy );

	if ( style.ptr() ) {
		for ( para = scrapCopy; para; para = para->GetNext( ) )
			para->SetStyle( 0, LONG_MAX, style, true, false );
	}

	TypeSpec nullSpec;

	for ( para = scrapCopy; para; para = para->GetNext( ) )
		para->Retabulate( nullSpec );

	if ( scrapCopy->GetNext() )
		fPoint.fPara = fPoint.fPara->PasteParas( scrapCopy, fPoint.fOffset );
	else
		fMark.fPara->PasteText( scrapCopy, fPoint.fOffset );

	((scStream*)scrapCopy)->STRFree();

	SLCRecomposeLogical( firstCol, fMark.fPara, redisplist, this, true );
	ChangeSelection( SLC_LOCATE, false );
}

/* ==================================================================== */
/* create part of a selection by using the information from a paralocation
 * probably being called from SLCRecompose
 */

static void SetLocationInfo( TextMarker&		tm, 
							 scStreamLocation*	pl,
							 scStream*			stream,
							 Bool				geoChange )
{
	scTextline* txl;
	MicroPoint	fHLoc;

	if ( stream )
		pl->fStream = stream;

	tm.fPara		= pl->fStream->NthPara( pl->fParaNum );
	if ( tm.fPara ) {
		tm.fParaCount	= pl->fParaNum;
		tm.fOffset		= pl->fParaOffset;
		tm.fEndOfLine	= pl->fEndOfLine;
	}
	else {	
		tm.fPara = pl->fStream->Last( );
		tm.fParaCount	= tm.fPara->GetCount();
		tm.fOffset		= 0;
		tm.fEndOfLine	= false;
	}


	if ( tm.fPara->FindLocation( tm.fOffset, tm.fEndOfLine, txl, fHLoc, eCursNoMovement ) ) {
		tm.fCol 		= txl->GetColumn();
		tm.fTxl 		= txl;
		tm.fColCount	= tm.fCol->GetCount( );
		tm.fLineCount	= txl->GetLinecount();
		tm.fHLoc		= fHLoc;
		if ( geoChange )
			tm.fSelMaxX 	= tm.fHLoc;
		else 
			tm.fSelMaxX 	= pl->fSelMaxX;
	}
	else {
		tm.fCol 		= 0;
		tm.fTxl 		= 0;
		tm.fColCount	= -1;
		tm.fLineCount	= -1;
		tm.fHLoc		= LONG_MIN;
		tm.fSelMaxX 	= LONG_MIN;
//		tm.fFlowDir.Invalidate();
	}
}

/* ==================================================================== */
// get information about this location within the text

static void GetLocationInfo( TextMarker& tm, scStreamLocation& pl )
{
	scTextline* txl;
	MicroPoint	fHLoc;
	scColumn*		col;

	pl.fStream		= tm.fPara->GetStream();
	pl.fParaNum 	= tm.fPara->GetCount();
	pl.fParaOffset	= tm.fOffset;
	pl.fEndOfLine	= tm.fEndOfLine;

	pl.fParaSpec	= tm.fPara->GetDefaultSpec();
	tm.fPara->ChInfo( tm.fOffset, pl.fTheCh, pl.fFlags,
					  pl.fTheChWidth, pl.fWordSpace, 
					  pl.fChSpec, pl.fUnitType );

	if ( tm.fPara->FindLocation( tm.fOffset, tm.fEndOfLine, txl, fHLoc, eCursNoMovement ) )
		pl.fAPPColumn = txl->GetColumn()->GetAPPName( );
	else {
		if ( pl.fStream->FindColumn( col ) )
			pl.fAPPColumn = ((scColumn*)col->LastInChain())->GetAPPName();
		else
			pl.fAPPColumn = 0;
	}
	pl.fPosOnLine		= ( txl ? tm.fHLoc : LONG_MIN );
	pl.fSelMaxX 		= ( txl ? tm.fSelMaxX : LONG_MIN );

	if ( pl.fChSpec.ptr() ) {
		pl.fFont			= scCachedStyle::GetCachedStyle( pl.fChSpec ).GetFont();
		pl.fPointSize		= scCachedStyle::GetCachedStyle( pl.fChSpec ).GetPtSize();
	}
	else {
		SCmemset( &pl.fFont, 0, sizeof( APPFont ) );
		pl.fPointSize		= 0;
	}

	if ( txl == 0 ) {
		pl.fBaseline	= LONG_MIN;
	}
	else {
		pl.fBaseline	= txl->GetBaseline();
		pl.fBaseline	+= txl->GetVJOffset();
	}
	
	pl.fMeasure 	= ( txl ? txl->GetLength() : LONG_MIN );
	pl.fLetterSpace = ( txl ? txl->GetLSP() : LONG_MIN );
}

/* ==================================================================== */
/* decompose the selection into two seperate structures that
 * contain information about the selection 
 */
 
void scSelection::Decompose( scStreamLocation&	mark,
							 scStreamLocation&	point )
{
	scSelection aRng( *this );
	aRng.Sort( );		/* sort the selection */

	GetLocationInfo( aRng.fMark, mark );
	GetLocationInfo( aRng.fPoint, point );
}

/* ==================================================================== */

void scSelection::Decompose2( scStreamLocation& mark,
							  scStreamLocation& point )
{
	scSelection aRng( *this );

	GetLocationInfo( aRng.fMark, mark );
	GetLocationInfo( aRng.fPoint, point );
}

/* ==================================================================== */

void scSelection::GetContUnits( scContUnit*& mark,
								scContUnit*& point ) const
{
	scSelection aRng( *this );
	aRng.Sort( );		/* sort the selection */

	mark	= aRng.fMark.fPara;
	point	= aRng.fPoint.fPara;	
}

/* ==================================================================== */	
/* create a selection point out of two ParaLocations */

void scSelection::Restore( const scStreamLocation*	mark,
						   const scStreamLocation*	point,
						   const scStream*			stream,
						   Bool 					geoChange )
{
	if ( mark ) {
		scStreamLocation tmark( *mark );
		SetLocationInfo( fMark, &tmark, (scStream*)stream, geoChange );
	}
	else
		fMark.Zero( theMARK, false );

	if ( point ) {
		scStreamLocation tpoint( *point );
		SetLocationInfo( fPoint, &tpoint, (scStream*)stream, geoChange );
	}
	else
		fPoint.Zero( thePOINT, false );
}

/* ==================================================================== */
// return stream associate with selection

scStream* scSelection::GetStream( ) const
{
	return fMark.fPara->GetStream();	
}

/* ==================================================================== */

TypeSpec scSelection::GetSpecAtStart( void ) const
{	
	scContUnit* firstContUnit;

	firstContUnit = fMark.fPara->Earlier( fPoint.fPara );
	if ( firstContUnit == fMark.fPara )
		return fMark.fPara->SpecAtOffset( fMark.fOffset );
	
	return fPoint.fPara->SpecAtOffset( fPoint.fOffset );		
}

/* ==================================================================== */

void	scSelection::Iter( SubstituteFunc	func,
						   scRedispList*	redispList )
{	
	scSelection 	validatedSelection;
	scContUnit	*para,
				*startParaH,
				*endParaH;
	long		startLocation,
				endLocation;
	
	raise_if( !ValidateSelection( validatedSelection ), scERRlogical );

	Sort( );
	startParaH		= fMark.fPara;
	endParaH		= fPoint.fPara;
	startLocation	= fMark.fOffset;
	
	if ( startParaH == endParaH ) {
		endLocation 	= fPoint.fOffset;	
		startParaH->Iter( func, startLocation, endLocation );		
		fPoint.fOffset = endLocation;
	}
	else {
		endLocation = PARAChSize( startParaH );
		startParaH->Iter( func, startLocation, endLocation );		
		
		for ( para = startParaH->GetNext(); para != endParaH; para = para->GetNext() ) {
			endLocation = PARAChSize( para );
			para->Iter( func, 0, endLocation ); 	
		}
		
		endLocation 	= fPoint.fOffset;	
		endParaH->Iter( func, 0, endLocation ); 	
		fPoint.fOffset = endLocation;		
	}
	
	STRReformat( NULL, startParaH, scReformatTimeSlice, redispList );

	ValidateSelection( validatedSelection );
	UpdateSelection( ); 
}

/* ==================================================================== */

int32 scSelection::ContentSize() const
{
	if ( fMark.fPara == fPoint.fPara )
		return ABS( fMark.fOffset - fPoint.fOffset );
	else {
		// not implemented
		return LONG_MAX;
	}
}

/* ==================================================================== */

void scSelection::CopyAPPText( stTextImportExport& apptext )
{
	scScrapPtr scrap;
	
	CopyText( scrap );
	if ( scrap ) {
		((scStream*)scrap)->CopyAPPText( apptext );

		long bytesFreed;
		((scContUnit*)scrap)->FreeScrap( bytesFreed );
	}
}

/* ==================================================================== */

void scSelection::PasteAPPText( stTextImportExport& apptext,
								scRedispList*		redisplist )
{
	TypeSpec ts;
	scStream* scrap = scStream::ReadAPPText( apptext );
	if ( scrap ) {
		PasteText( scrap, ts, redisplist );

		long bytesFreed;
		((scContUnit*)scrap)->FreeScrap( bytesFreed );
	}
}

/* ==================================================================== */

