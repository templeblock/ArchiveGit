/****************************************************************************
Contains:	The 'methods' for the column objects.
****************************************************************************/

#include "sccolumn.h"
#include "scpubobj.h"
#include "scapptex.h"
#include "sccallbk.h"
#include "scstcach.h"
#include "scglobda.h"
#include "scmem.h"
#include "scparagr.h"
#include "scpolygo.h"
#include "scregion.h"
#include "scselect.h"
#include "scstream.h"
#include "scset.h"
#include "sctextli.h"
#include "screfdat.h"
#include "scfileio.h"

#include <float.h>

/* ==================================================================== */

scColumn* scColumn::fTheContextList = 0;

/* ===================================================================== */
scColumn::~scColumn()
{
	delete fSelection;
	fSelection = NULL;
}

/* ===================================================================== */
#define FILE_SIZE_COLUMN	28

void scColumn::Read( scSet* 	enumTable,
					 APPCtxPtr	ctxPtr,
					 IOFuncPtr	readFunc )
{
	uchar			abuf[FILE_SIZE_COLUMN];
	const uchar*	pbuf	= abuf;
	
	scTBObj::Read( enumTable, ctxPtr, readFunc );
	Mark( scINVALID );

		// read in the rest of the columns data
	ReadBytes( abuf, ctxPtr, readFunc, FILE_SIZE_COLUMN );
	
		// pointer to stream
	ulong uval;
	pbuf = BufGet_long( pbuf, uval, kIntelOrder );
	fStream = (scStream*)uval;

		// pointer to first line
	pbuf = BufGet_long( pbuf, uval, kIntelOrder );
	scAssert( uval == 0 );
	

		// flow direction
	ushort	uflow;
	pbuf = BufGet_short( pbuf, uflow, kIntelOrder );
	fFlowDir.SetLineDir( (eTextDirections)uflow );

	pbuf = BufGet_short( pbuf, uflow, kIntelOrder );
	fFlowDir.SetCharDir( (eTextDirections)uflow );

		// width & depth
	pbuf = BufGet_long( pbuf, uval, kIntelOrder );
	fSize.SetWidth( uval );
	
	pbuf = BufGet_long( pbuf, uval, kIntelOrder );
	fSize.SetDepth( uval );

		// application name
	pbuf = BufGet_long( pbuf, uval, kIntelOrder );
	fAppName = (APPColumn)APPDiskIDToPointer( ctxPtr, (long)uval, diskidColumn );

		// count
	pbuf = BufGet_long( pbuf, uval, kIntelOrder  );
	fColumnCount = uval;

	scAssert ((size_t)(pbuf-abuf) == FILE_SIZE_COLUMN );

		// shape type
	long val;
	ReadLong( val, ctxPtr, readFunc, kIntelOrder );

	if ( val ) {
		HRgnHandle rgnH = RGNfromFile( ctxPtr, readFunc, fShapePieces );

		SetShapeType( eRgnShape );

		fRgnH = rgnH;

		scAutoUnlock h( fRgnH );
		HRgn*	rgn = (HRgn *)*h;

		fShapePieces = (ushort)rgn->fNumSlivers;
	}
	else
		fShapePieces = 0;
			
	if ( !GetPrev() )
		scStream::STRFromFile( enumTable, ctxPtr, readFunc );		
}

/* ==================================================================== */
/* ACTUAL WRITE, this performs the write out of the column data structure,
 * paragraphs are written out with the first column in a set of linked columns
 * other than the column itself the only thing we will be writting out will
 * be the outline vertices
 */

void scColumn::Write( APPCtxPtr ctxPtr,
					  IOFuncPtr writeFunc )
{
	scTBObj::Write( ctxPtr, writeFunc );

	uchar	abuf[FILE_SIZE_COLUMN];
	uchar*	pbuf	= abuf;

		// pointer to stream
	pbuf = BufSet_long( pbuf, fStream ? fStream->GetEnumCount() : 0, kIntelOrder );

		// pointer to first line
	pbuf = BufSet_long( pbuf, 0, kIntelOrder ); 	
	

		// flow direction
	pbuf = BufSet_short( pbuf, (ushort)fFlowDir.GetLineDir(), kIntelOrder );
	pbuf = BufSet_short( pbuf, (ushort)fFlowDir.GetCharDir(), kIntelOrder );

		// width & depth
	pbuf = BufSet_long( pbuf, fSize.Width(), kIntelOrder );
	pbuf = BufSet_long( pbuf, fSize.Depth(), kIntelOrder );

		// application name
	pbuf = BufSet_long( pbuf,
						APPPointerToDiskID( ctxPtr, fAppName, diskidColumn ),
						kIntelOrder );

		// count
	pbuf = BufSet_long( pbuf, GetCount(), kIntelOrder  );

	scAssert ((size_t)(pbuf-abuf) == FILE_SIZE_COLUMN );

	WriteBytes( abuf, ctxPtr, writeFunc, FILE_SIZE_COLUMN );

	WriteLong( (ulong)fShapePieces, ctxPtr, writeFunc, kIntelOrder );

	switch ( GetShapeType() ) {

		default:
			break;
			
		case eVertShape:
			POLYtoFile( ctxPtr, writeFunc, fVertH, fShapePieces );
			break;

		case eRgnShape:
			RGNtoFile( ctxPtr, writeFunc, fRgnH, fShapePieces );
			break;
	}

	if ( !GetPrev() )
		fStream->STRToFile( ctxPtr, writeFunc );
}

/* ==================================================================== */
void scColumn::RestorePointers( scSet* enumTable )
{
	if ( !Marked( scPTRRESTORED ) ) {
		scTBObj::RestorePointers( enumTable );

		AddToCTXList();

		fStream = (scStream*)enumTable->Get( (long)fStream );
		if ( !GetPrev() )
			fStream->STRRestorePointers( enumTable );
	}
}

/* ==================================================================== */	
void scColumn::SetRecomposition( BOOL tf )
{
	scColumn* col = (scColumn*)FirstInChain();

	if ( tf )
		col->Mark( scLAYcomposeACTIVE );
	else
		col->Unmark( scLAYcomposeACTIVE );			
}

/* ==================================================================== */		
BOOL scColumn::GetRecomposition( void ) const
{
	scColumn* col = (scColumn*)FirstInChain();

	return col->Marked( scLAYcomposeACTIVE );
}

/* ==================================================================== */
// get the selection object associated with the flowset, if there is
// none it will create one
scSelection* scColumn::FlowsetGetSelection( void )
{
	scColumn*		col 	= (scColumn*)FirstInChain();

	if ( !col->GetSelection() )
		col->SetSelection( new scSelection( col ) );

	return col->GetSelection();
}

/* ==================================================================== */
// set the selection object for the flowset none should exist,
// since if it does error recovery might be a bit tricky
void scColumn::FlowsetSetSelection( scSelection* sel )
{
	scColumn*	col = (scColumn*)FirstInChain();

	col->SetSelection( sel );
}

/* ==================================================================== */
// this removes the selection from the flowset
// NOTE: it does not delete it
void scColumn::FlowsetRemoveSelection( void )
{
	scColumn*	col = (scColumn*)FirstInChain();

	col->SetSelection( 0 );
}

/* ==================================================================== */
void scColumn::FlowsetInvalidateSelection( void )
{
	scColumn*		col = (scColumn*)FirstInChain();
	scSelection*	sel = col->GetSelection();

	if ( sel )
		sel->Invalidate();
}

/* ==================================================================== */		
void scColumn::RecomposeFlowset( long ticks, scRedispList* redispList )
{
	scColumn* col = (scColumn*)FirstInChain();
	
	SetRecomposition( true );
	
	for ( ; col; col = col->GetNext() ) {
		if ( col->Marked( scINVALID ) )
			col->LimitDamage( redispList, ticks );
		else if ( col->Marked( scREALIGN ) ) {
			scRedisplayStoredLine rdl( GetLinecount( ) );
			rdl.SaveLineList( this );

			col->VertJustify();

			scXRect lineDamage;
			rdl.LineListChanges( this, lineDamage, redispList );
			col->Unmark( scREALIGN );
		}
	}
	scSelection* select = FlowsetGetSelection();
	select->UpdateSelection( );
}

/* ==================================================================== */
/* ==================================================================== */		
#if SCDEBUG > 1

void scColumn::scAssertValid( BOOL recurse ) const
{
	scTBObj::scAssertValid( recurse ); 
	if ( !recurse ) {
		if ( fFirstline ) 
			fFirstline->scAssertValid( false );

		if ( fStream )	
			fStream->scAssertValid( false );
	}
}

#endif

/* ===================================================================== */
/* set the max selection extent based upon the column flow direction */
static void COLSetSelMax( scColumn* 		col,
						  TextMarker*		tm,
						  const scMuPoint&	muPt )
{
	if ( col->GetFlowdir().IsVertical() )
		tm->fSelMaxX	= muPt.y;
	else
		tm->fSelMaxX	= muPt.x;
}

/* ------------------------------------------------------------ */
void scColumn::CheckOverflow(bool* pbOverflow)
{
	if (!GetStream())
	{
		*pbOverflow = false;
		return;
	}

	scContUnit* firstPara;
	if (Marked(scINVALID))
		firstPara = MarkParas();
	else
		firstPara = GetStream();

	scAssert(firstPara != 0);

	scRedispList redisplist;
	*pbOverflow = (eOverflowGeometry == STRReformat(this, firstPara, scReformatTimeSlice, &redisplist));
}

/* ==================================================================== */
/* hilite or dehilite the characters in this column */
void scColumn::Hilite( const TextMarker&		tmMark,
					   const TextMarker&		tmPoint,
					   HiliteFuncPtr			func,
					   const scSelection&		selection )
{
	scTextline* txl;
	scTextline* lastTxl;
	scTextline* txl1		= tmMark.fTxl;
	scTextline* txl2		= tmPoint.fTxl;
	MicroPoint	startLoc	= tmMark.fHLoc,
				stopLoc 	= tmPoint.fHLoc;
	APPDrwCtx	appMat;

	APPDrawContext( GetAPPName(), this, appMat );
	
	lastTxl = txl2->GetNext();
	
	for ( txl = txl1; txl && txl != lastTxl; txl = txl->GetNext() ) {
		if ( txl == txl1 )
			txl->Hilite( &tmMark, startLoc, NULL, LONG_MAX, appMat, func, selection );
		else if ( txl == txl2 )
			txl->Hilite( NULL, LONG_MIN, &tmPoint, stopLoc, appMat, func, selection );
		else
			txl->Hilite( NULL, LONG_MIN, NULL, LONG_MAX, appMat, func, selection );
	}
}

/* ==================================================================== */
/* select text in a col at the given hit point */
BOOL scColumn::Select( const scMuPoint& hitPt,
					   TextMarker*		textMarker,
					   double*			bestDist )
{
	scXRect 	exRect;
	scTextline* txl;
	long		count;
	scMuPoint	charOrg;
	MicroPoint	fudgeHFactor,
				fudgeVFactor;
	double		dist;
	BOOL		belowText	= false;
	BOOL		vertical	= false;
	int 		lineNum;

	vertical = GetFlowdir().IsVertical();

		/* make first hit infinitely far away */
	*bestDist = DBL_MAX;

	textMarker->fCol		= this;
	textMarker->fColCount	= GetCount();
	textMarker->fPara		= NULL;
	textMarker->fTxl		= NULL;

	fudgeHFactor = fudgeVFactor = 0;

	while ( GetFirstline() && !textMarker->fPara ) {
		for ( lineNum = 0, txl = GetFirstline(); txl; txl = LNNext( txl ), lineNum++ ) {

			txl->QueryExtents( exRect );
				// grow hit by fudge factor to account for sloppy hits, 
				// how well will this worked on zoomed text?, this value
				// is in world coordinates, NOT the screen coordinates
			exRect.Inset( fudgeHFactor, fudgeVFactor );

			if ( exRect.PinRect( hitPt ) ) { 
#if SCDEBUG > 1
				SCDebugTrace( 2, scString( "COLSelect: line #%d (%d,%d) (%d %d %d %d)\n" ),
							  lineNum, 
							  muPoints( hitPt.x ), muPoints( hitPt.y ),
							  muPoints( exRect.x1 ), muPoints( exRect.y1 ),
							  muPoints( exRect.x2 ), muPoints( exRect.y2 ) );
#endif
				BOOL endOfLine;
	
					// we have a hit within the extents of the line, now see 
					// exactly where on the line we may have selected
				dist = txl->Select( charOrg, count, hitPt, eCursNoMovement, endOfLine );

				if ( dist < *bestDist ) {
						// we have a hit that is better than any previous hit
					*bestDist = dist;

					if ( vertical ) 
						textMarker->fHLoc		= charOrg.y;
					else
						textMarker->fHLoc		= charOrg.x;
						
					textMarker->fOffset 	= count;

//					if ( LNOrigin( txl ) + LNLength(txl) <= fHLoc && LNIsHyphenated( txl ) )
//						textMarker->fEndOfLine	= true;
//					else
						textMarker->fEndOfLine	= endOfLine;
					
					textMarker->fPara		= txl->GetPara();
					textMarker->fTxl		= txl;
					textMarker->fParaCount	= textMarker->fPara->GetCount();
					textMarker->fLineCount	= txl->GetLinecount();
				}
			}
				/* if no selection and the y position is 
				 * lower than the top of the last line, then 
				 * select the last char on the last line
				 */
			
				/* assumes lines move from right to left */ 		
			if ( vertical )
				belowText = !LNNext( txl )	&& hitPt.x < exRect.x2 && *bestDist == DBL_MAX;
			else
				belowText = !LNNext( txl )	&& hitPt.y > exRect.y2 && *bestDist == DBL_MAX;
				
			if ( belowText ) {
				*bestDist = txl->Select( charOrg, count, hitPt, eCursForward, textMarker->fEndOfLine );

				textMarker->fOffset 	= txl->GetEndOffset();
				
				scMuPoint charOrg;
				charOrg 				= txl->Locate( textMarker->fOffset, charOrg, eCursForward );
				if ( vertical ) 			
					textMarker->fHLoc	= charOrg.y;
				else
					textMarker->fHLoc	= charOrg.x;

				textMarker->fPara		= txl->GetPara();
				textMarker->fTxl		= txl;
				textMarker->fParaCount	= textMarker->fPara->GetCount();
				textMarker->fLineCount	= txl->GetLinecount();
				break;
			}	
		}

		if ( vertical ) {
			fudgeHFactor -= scPOINTS(1);
			fudgeVFactor -= scPOINTS(8);
		}
		else {
			fudgeHFactor -= scPOINTS(144);
			fudgeVFactor -= scPOINTS(1);
		}
	}

	return( textMarker->fPara != NULL );
}

/* ==================================================================== */
/* return a number that is the square of the dx plus the square of the
 * dy between the 'pt' and a significant point
 */

void scColumn::ClickEvaluate( const scMuPoint&	pt,
							  double& 			dist )
{
	TextMarker	tm;
	double		nearDist;
	
	dist = DBL_MAX; 	/* defined in scmath.h */
	
	if ( GetStream() ) {
		tm.fCol = this;
		raise_if ( !Select( pt, &tm, &nearDist ), scERRlogical );
		dist = nearDist;	
	}
}

/* ==================================================================== */
/* select something special indicated by the SelectType */

void scColumn::SelectSpecial( const scMuPoint&	pt,
							  eSelectModifier	selectMod,
							  scSelection*& 	select )
{
	select = FlowsetGetSelection();
	
	scSelection 	newSelection( *select );
	double			dist;
	
	if ( !GetStream() )
		return;


	newSelection.fMark.fCol 	= this;
	COLSetSelMax( this, &newSelection.fMark, pt );

	if ( selectMod == eAllSelect )
		newSelection.AllSelect( );
	else {

#ifdef TESTEXTENTS
	
		{
			HRect			maxExRect,			/* column extents */
							maxMargRect;		/* column margins */
			
			/* if the point is to far out of the maxExRect 
			 * things will get very slow 
			 */

			maxExRect = col->fExtents;
			SetHRect( &maxMargRect, 0, 0, col->fWidth, col->fDepth );
		
			UnionHRect( &maxExRect, &maxMargRect, &maxExRect );


			if ( !MuPtInHRect( pt, &maxExRect ) ) {
				/* the point is in GM's front yard */				
				return scERRbounds; 
			}
		}
#endif /* TESTEXTENTS */


		raise_if( !Select( pt, &newSelection.fMark, &dist ), scERRbounds );

		newSelection.fPoint   = newSelection.fMark;

		switch ( selectMod ) {
			case eWordSelect:
				newSelection.WordSelect();
				break;
			case eLineSelect:
				newSelection.LineSelect();
				break;
			case eParaSelect:
				newSelection.ParaSelect();
				break;
			case eColumnSelect:
				newSelection.ColumnSelect();
		}
	}

	*select = newSelection;
}


/* ==================================================================== */
/* start selection in the original column
 */

void scColumn::StartClick( const scMuPoint& pt,
						   HiliteFuncPtr	func,
						   APPDrwCtx,
						   scSelection*&	select )
{
	double		dist;
	scSelection selection;

	if ( !GetStream() )
		return;

	selection.fMark.fCol	= this;
	
	COLSetSelMax( this, &selection.fMark, pt );

	raise_if ( !Select( pt, &selection.fMark, &dist ), scERRlogical );

	selection.fPoint	= selection.fMark;

	selection.LineHilite( func );

	select = FlowsetGetSelection();

	*select = selection;
}

/* ==================================================================== */

void scColumn::ContinueClick( const scMuPoint&	pt,
							  HiliteFuncPtr 	func,
							  scSelection*		select )
{
	double		dist;
	scSelection oldSelection( *select );

	raise_if( !select->fMark.fCol, scERRstructure );

	if ( !GetStream() )
		return;

	select->fPoint.fCol = this;

	if ( !GetFirstline() )
		return;

		// columns not in same stream, application program should catch this
	raise_if ( select->fMark.fCol->GetStream( ) != select->fPoint.fCol->GetStream(), scERRstructure ); 

	COLSetSelMax( this, &select->fPoint, pt );

	if ( Select( pt, &select->fPoint, &dist ) ) {
		select->InteractiveHilite( oldSelection, func );
	}
	else
		raise( scERRlogical );
}

/* ==================================================================== */

void scColumn::StartShiftClick( scStreamLocation&	mark,
								const scMuPoint&	pt,
								HiliteFuncPtr		func,
								APPDrwCtx,
								scSelection*&		select )
{
	double	dist;

	if ( !GetStream() )
		return;

	select = FlowsetGetSelection();
	select->Restore( &mark, 0, 0, false );

	select->fPoint.fCol = this;

	if ( Select( pt, &select->fPoint, &dist ) )
		select->LineHilite( func );
	else
		raise( scERRlogical );
}

/* ==================================================================== */

void scColumn::InitialSelection( TypeSpec&		ts,
								 scSelection*&	select )
{
	scMuPoint	mPt;
	TextMarker	tm;
	double		dist;
	scContUnit* firstPara;
	BOOL		iAdded	= false;
	
	select = NULL;

	raise_if( GetPrev(), scERRlogical );

	if ( !GetStream() ) {
		firstPara = scContUnit::Allocate( ts, NULL, 0L );

			// initialize spec cache
		scCachedStyle::SetParaStyle( firstPara, ts );
		scCachedStyle::GetCachedStyle( ts );	

		SetFlowsetStream( (scStream*)firstPara );

		Mark( scINVALID );
		LimitDamage( 0, scReformatTimeSlice );
		iAdded = true;
	}

	mPt.Set( 0, 0 );

	if ( !Select( mPt, &tm, &dist ) ) {
		if ( iAdded )
			FreeStream( );
		raise( scERRstructure );
	}
	
	select = FlowsetGetSelection();
	select->SetMark( tm );
	select->SetPoint( tm ); 	
}

/* ==================================================================== */

void scColumn::LineInfo( scLineInfoList*	lineInfoList,
						 long&				nLines,
						 BOOL&				moreText ) const
{
	scTextline* txl;

	nLines		= GetLinecount();
	moreText	= MoreText( );
	
	if ( lineInfoList && nLines ) {
		scLineInfo	lineInfo;
		
		lineInfoList->RemoveAll();
		
		for ( txl = GetFirstline(); txl; txl = txl->GetNext() ) {
			txl->GetLineInfo( lineInfo );
			lineInfoList->AppendData( (ElementPtr)&lineInfo );
		}
	}
}

/* ==================================================================== */
/* paste a region into the indicated column, rebreak and return
 * damaged areas
 */

void scColumn::PasteRgn( const HRgnHandle	srcRgnH, 
						 scRedispList*		redispList )
{
	HRgnHandle	dstRgnH;
	HRgn*		rgn;

	raise_if( srcRgnH == NULL, scERRstructure );

	dstRgnH = NewHRgn( RGNSliverSize( srcRgnH ) );

	if ( fRgnH ) {
		SectHRgn( fRgnH, srcRgnH, dstRgnH );
		DisposeHRgn( fRgnH );
	}
	else {
		CopyHRgn( dstRgnH, srcRgnH );
		SetShapeType( eRgnShape );
	}
	
	fRgnH = dstRgnH;
	scAutoUnlock h( fRgnH );
	rgn = (HRgn *)*h;

	fShapePieces = (ushort)rgn->fNumSlivers;



	Mark( scINVALID );
	LimitDamage( redispList, scReformatTimeSlice );
}

/* ==================================================================== */

void scColumn::CopyRgn( HRgnHandle& dstRgn )
{
	dstRgn = NewHRgn( RGNSliverSize( fRgnH ) );
	CopyHRgn( dstRgn, fRgnH );
}

/* ==================================================================== */
/* paste a polygon into the indicated column, rebreak and return
 * damaged areas
 */

void scColumn::ReplacePoly( scVertHandle	srcVertH, 
							scRedispList*	redispList )
{
	ushort			shapePieces;
	scVertex*		srcV;
	scVertex*		dstV;

	scAutoUnlock	h( srcVertH );
	srcV = (scVertex*)*h;

	shapePieces = POLYCountVerts( srcV );

	fVertH = MEMResizeHnd( fVertH, shapePieces * sizeof( scVertex ) );

	scAutoUnlock h1( fVertH );
	dstV = (scVertex*)*h1;
	memmove( dstV, srcV, (size_t)(shapePieces * sizeof( scVertex )) );
	fShapePieces = shapePieces;
	
		/* check if poly type is set */
	SetShapeType( eVertShape );
	
	Mark( scINVALID );
	LimitDamage( redispList, scReformatTimeSlice );
}

/* ==================================================================== */

#if defined( scColumnShape )

/* add a polygon into the indicated column, rebreak and return
 * damaged areas
 */

void scColumn::PastePoly( scVertHandle	srcVertH,
						  scRedispList* redispList )
{
	ushort			shapePieces;
	scVertex		*srcV,
					*dstV;

	raise_if( GetShapeType() == eRgnShape, scERRstructure );

	scAutoUnlock h( srcVertH );
	srcV = (scVertex*)*h;

	shapePieces = POLYCountVerts( srcV );

	SetShapeType( eVertShape );

	fVertH = MEMResizeHnd( fVertH, shapePieces * sizeof(scVertex) );


	scAutoUnlock h2( fVertH );
	dstV = (scVertex*)*h2;

	if ( fShapePieces ) {
		dstV += ( fShapePieces - 1 );
		scAssert( dstV->fPointType == eFinalPoint );
		dstV->fPointType = eStopPoint;
		dstV++;
	}

	memmove( dstV, srcV, (size_t)(shapePieces * sizeof( scVertex )) );
	fShapePieces = (ushort)(fShapePieces + shapePieces);

	Mark( scINVALID );
	LimitDamage( redispList, scReformatTimeSlice );
}

/* ==================================================================== */

void scColumn::ClearShape( scRedispList* redispList )
{
	switch ( GetShapeType() ) {
		case eVertShape:
		case eRgnShape:
			SetShapeType( eNoShape );
			Mark( scINVALID );
			LimitDamage( redispList, scReformatTimeSlice );
			break;
		case eVertFlex:
		case eHorzFlex:
		case eFlexShape:
		case eNoShape:
			break;
	}
}

/* ==================================================================== */

void scColumn::CopyPoly( scVertHandle* dstVertHP )
{
	scVertHandle	scrapPolyH; 			

	if ( fVertH && GetShapeType() == eVertShape ) {
		scrapPolyH = MEMAllocHnd( fShapePieces * sizeof(scVertex) );
		
		scAutoUnlock	h1( scrapPolyH );
		scAutoUnlock	h2( fVertH );
		memmove( (scVertex*)*h1, (scVertex*)*h2, (size_t)(fShapePieces * sizeof( scVertex ) ) );
	}
	else
		scrapPolyH = NULL;

	*dstVertHP = scrapPolyH;
}

/* ==================================================================== */

#endif

/* ==================================================================== */
/* this is primarily called when a column has changed, it forces a rebreak
 * of the paragraphs in the column, 'StrRerfomat' should take care of damage
 * to subsequent paragraphs in subsequent columns, this also forces the
 * the rebreaking of any paragraphs that have no first line, thus if
 * a column is deleted it will force the correct rebreaking
 */

void scColumn::LimitDamage( scRedispList* redisplist, long ticks )
{
	scContUnit* firstPara;
	scColumn*	nextcol;

	/* look thru the stream until we find an intersection of a paragraph
	 * and a column, once we have an intersection we mark all the remaining
	 * paragraphs to be rebroken, one problem is that if the column has been
	 * made so small no lines are in it, then no paras are marked, the code
	 * following the walk down the list takes care of that case
	 */

	if ( !GetRecomposition() ) {
		Mark( scINVALID );
		return;
	}

	if ( !GetStream() )
		return;

	if ( Marked( scINVALID ) )
		firstPara	= MarkParas( );
	else
		firstPara	= GetStream();

	/* before we get into the stream make sure all paras that need to
	 * be marked as REBREAK are marked as such
	 */
	for ( nextcol = this; nextcol; nextcol = nextcol->GetNext() ) {
		if ( nextcol->Marked( scINVALID ) )
			nextcol->MarkParas();
	}

	scAssert( firstPara != 0 );

	STRReformat( this, firstPara, ticks, redisplist );
}

/* ==================================================================== */
/* renumber all the columns */

void scColumn::Renumber( )
{
	scColumn* col = (scColumn*)FirstInChain();
	long		count;

		/* renumber */
	for ( count = 0; col; col = col->GetNext() )
		col->SetCount( count++ );
}

/* ==================================================================== */
/* creates a new unlinked empty column of specified width and depth */

scColumn::scColumn( APPColumn	appName,
					MicroPoint	width,
					MicroPoint	depth,
					scStream*	p,
					eCommonFlow flow ) :
						fShapePieces( 0 ),
						fRgnH( 0 ),
						fNextContext( 0 ),
						fAppName( appName ),
						fColumnCount( 0 ),
						fSize( width, depth ),
						fFlowDir( flow ),
						fStream( p ),
						fSelection( 0 ),
						fFirstline( 0 )
{
	SetShapeType( eNoShape );
	
	fInkExtents.Set( 0, 0, 0, 0 );

		/* add to context list */
	AddToCTXList( );

	if ( appName == 0 )
		fAppName = (APPColumn)this;
}

/* ==================================================================== */

scColumn* scColumn::FindFlowset( const scStream* str )
{
	scColumn *col;

	col 		= fTheContextList;

	for ( ; col; col = col->GetContext() ) {
		if ( col->GetStream() == str )
			return col->GetFlowset();
	}
	return 0;
}

/* ==================================================================== */

void scColumn::DeleteFromCTXList( )
{
	scColumn *col;

	col 		= fTheContextList;

	if ( this == col )
		fTheContextList = GetContext();
	else {
		for ( ; col && col->GetContext() != this; col = col->GetContext() )
			;
		if ( col )
			col->SetContext( GetContext() );
	}
}

/* ==================================================================== */

void scColumn::VerifyCTXList( void ) const
{
	register scColumn* col;

	for ( col = fTheContextList; col; col = col->GetContext() ) {
		if ( this == col )
			return;
	}

	raise( scERRidentification );
}

/* ==================================================================== */

void scColumn::FiniCTXList( void )
{
	scColumn*	col; 
	scColumn*	nextCol;

	for ( col = fTheContextList; col; col = nextCol ) {

		SCDebugTrace( 1, scString( "FiniCTXList: 0x%08x\n" ), col );
		
		nextCol = col->GetContext();

		col->FreeStream( );

			// must do this since all layout are tracked
		col->scTBObj::Unlink();
		col->Free();
	}
}

/* ==================================================================== */
	
void scColumn::SetVertFlex( BOOL			tf,
							scRedispList*	redispList )
{
	if ( tf )
		SetShapeType( eVertFlex );
	else
		fLayBits.fLayType = (eColShapeType)( fLayBits.fLayType & ~eVertFlex );				

	Mark( scINVALID );
	LimitDamage( redispList, scReformatTimeSlice );
}

/* ==================================================================== */

void scColumn::SetHorzFlex( BOOL			tf,
							scRedispList*	redispList )
{
	if ( tf )
		SetShapeType( eHorzFlex );
	else
		fLayBits.fLayType = (eColShapeType)( fLayBits.fLayType & ~eHorzFlex );		

	Mark( scINVALID );
	LimitDamage( redispList, scReformatTimeSlice );
}

/* ==================================================================== */

void scColumn::SetShapeType( eColShapeType type )
{
	switch ( type ) {
		case eVertShape:
		case eRgnShape:
			if ( (eColShapeType)fLayBits.fLayType != type )
				FreeShape();
			fLayBits.fLayType = type;
			break;
		case eVertFlex:
		case eHorzFlex:
		case eFlexShape:
		case eNoShape:
			if ( (eColShapeType)fLayBits.fLayType & eIrregShape ) {
				/* we are trying to turn an irregularly shaped container
				 * into a flexible container - we will have to free the shape
				 */
				FreeShape();
			}
			if ( type == eNoShape )
				fLayBits.fLayType = type;
			else
				fLayBits.fLayType = (eColShapeType)(( fLayBits.fLayType & eFlexShape ) | type );
			break;
	}
}

/* ==================================================================== */
/* free the lines with the column, this is tricky because we may want
 * to disentangle pointers at the same time
 */

void scColumn::FreeLines( BOOL		reportDamage,
						  scXRect&	lineDamage )
{		
	scTextline* txl;
	scTextline* nextTxl;
	scContUnit* para;
	scXRect 	extents;
	scContUnit* streamPresent = fStream;

	for ( txl = fFirstline; txl; txl = nextTxl ) {
#if SCDEBUG > 1
		txl->scTBObj::scAssertValid();
#endif
		nextTxl = LNNext( txl );
		if ( reportDamage ) {
			txl->QueryExtents( extents, 1 );
			if ( extents.Width() == 0 )
				extents.x2 = extents.x1 + 1;
			lineDamage.Union( extents );
		}
		if ( streamPresent != 0 ) {
			para = txl->GetPara( );
			if ( para && para->GetFirstline() == txl )
				para->SetFirstline( 0 );
		}

		delete txl;
	}

	SetFirstline( NULL );
}


/* ==================================================================== */
/* free the vertices of this column */

void scColumn::FreeShape( )
{
	switch ( GetShapeType() ) {
		case eVertShape:
			if ( fVertH != NULL )
				MEMFreeHnd( fVertH );
			fShapePieces = 0;
			fVertH = NULL;
			break;
		case eRgnShape:
			if ( fRgnH != NULL )
				DisposeHRgn( fRgnH );
			fShapePieces = 0;
			fRgnH = NULL;
			break;
		default:
			break;
	}
}

/* ==================================================================== */
/* free the column, no disentanglement of pointers, save its own internal
 * structures
 */

void scColumn::Free()
{
	scXRect lineDamage;

	FreeLines( false, lineDamage ); 	// deletes lines
	SetShapeType( eNoShape );

		// free it up from the context list
	DeleteFromCTXList( );

	delete this;
}

/* ==================================================================== */
/* free the column that is part of the scrap */

void scColumn::FreeScrap( )
{
	scAssert( !GetNext() );
		
	FreeStream( );	  /* deletes stream */

	DeleteFromCTXList( );
	Free();
}

/* ==================================================================== */
/* clear the stream from the set of linked columns,
 * that this column belongs to
 */

void scColumn::FlowsetClearStream( scRedispList* redispList )
{
	scColumn*	firstCol = GetFlowset();
	scXRect 	lineDamage;

		// invalidate selection
	FlowsetInvalidateSelection();

		// free all the lines associated with the column(s)
	scColumn* col;
	for ( col = firstCol; col; col = col->GetNext() ) {
		if ( col->GetFirstline() )
			col->FreeLines( true, lineDamage );  /* deletes lines */
	}
	
		// delete the stream from all the column(s)
	firstCol->FreeStream( );
}

/* ==================================================================== */
/* cut the stream from the set of linked columns,
 * that this column belongs to
 */

void scColumn::FlowsetCutStream( scStream*		stream, 
								 scRedispList*	redispList )
{
	scColumn*	firstCol = GetFlowset();
	scXRect 	lineDamage;

	FlowsetInvalidateSelection();

	stream->STRDeformat();

	scColumn* col;
	for ( col = firstCol; col; col = col->GetNext() )
		col->FreeLines( true, lineDamage );  /* deletes lines */

	SetFlowsetStream( 0 );
}

/* ==================================================================== */

void scColumn::FlowsetPasteStream( scStream*		stream, 
								   scRedispList*	redispList )
{
	scColumn*	firstCol = GetFlowset();


	stream->STRMark( scREBREAK );

	if ( GetStream() )
		GetStream()->Append( stream );
	else
		SetFlowsetStream( stream );

	firstCol->Mark( scINVALID );
	firstCol->LimitDamage( redispList, scReformatTimeSlice );
}

/* ==================================================================== */
/* free the column, not any text associated with it and unlink it from
 * its column chain
 */

void scColumn::Delete( scRedispList* redispList )
{
	scColumn* firstCol; 
	scColumn* nextCol;


	firstCol		= (scColumn*)FirstInChain();
	nextCol 		= GetNext();
	
	if ( this == firstCol ) {
				// trying to free a column in a chain without
				// unllinking it
		raise_if( nextCol && GetStream(), scERRstructure );
			
		if ( !nextCol && GetStream() ) {
				// we are the only column left so
				// we need to delete the text stream
			FreeStream( );

			TypeSpec nullSpec;

				// clear the cache to help eliminate refs to specs
			scCachedStyle::StyleInvalidateCache( nullSpec );
		}
	}
	scTBObj::Unlink( );
	DeleteFromCTXList( );


	if ( firstCol != this ) {
		firstCol->Renumber( );
		firstCol->Mark( scINVALID );
		firstCol->LimitDamage( redispList, scReformatTimeSlice );
	}
	else if ( nextCol ) {
		firstCol->Renumber( );		
		firstCol->Mark( scINVALID );
		firstCol->LimitDamage( redispList, scReformatTimeSlice );
	}

	Free();
}

/* ==================================================================== */
// because of reformatting nothing lands in here we will still return
// true
 

BOOL scColumn::HasText( ) const
{
	scContUnit* p;
	
	for ( p = GetStream(); p; p = p->GetNext( ) ) {
		if ( p->GetContentSize() > 0 )
			return true;
	}
	return false;
}

/* ==================================================================== */
/* does the text flow out the bottom of this container */

BOOL scColumn::MoreText( ) const
{
	scTextline* txl;
	scContUnit* para;
	scColumn*	neighborCol;

	txl = GetLastline();

	if ( txl ) {
		para = txl->GetPara();
		if ( para->GetNext() )
			return true;
		else if ( para->GetContentSize() > txl->GetEndOffset() )
			return true;
			
	}
	else if ( GetStream() ) {
		neighborCol =  NextWithLines(); 	// text in subsequent columns 
		if ( neighborCol )
			return true;

		neighborCol = PrevWithLines();
		
		if ( neighborCol ) { 
			txl = neighborCol->GetLastline();
		
				// this gets a little tricky, we are assuming that
				// the text cannot be reformatted into this or
				// some other column and therefore it hangs off the
				// end
			para = txl->GetPara();
			if ( para->GetNext() )
				return true;	// another paragraph beyond last formatted line
			else if ( para->GetContentSize() > txl->GetEndOffset() )
				return true;	// more characters byyond last formatted line
			return false;		// no more text
		}
		return true;			// no text formatted and we have a stream
	}
	return false;
}

/* ==================================================================== */
// determines line num in column of selection, assumes a sliver cursor	

short COLLineNum( scSelection* select )
{
	scColumn*	col;
	scTextline* txl;
	scTextline* countTxl;
	short		lineCount;

	if ( select ) {
		col = select->fMark.fCol;
		txl = select->fMark.fTxl;
		if ( col && txl ) {
			countTxl = col->GetFirstline();
			for ( lineCount = 1; 
					countTxl != NULL; 
						lineCount++, countTxl = LNNext( countTxl ) ) {
				if ( countTxl == txl )
					return lineCount;
			}
		}
	}
	return -1;
}

/* ==================================================================== */
/* determine the size of the damagerect for ImmediateRedisp depending
 * on the lines set
 */

void scColumn::LineExtents( scImmediateRedisp& immediateRedisp )
{
	scTextline* txl;
	short		count;
	scXRect 	colRect;
	scXRect 	rect;

	colRect.Invalidate();

	txl = fFirstline;
	for ( count = 1; txl && count < immediateRedisp.fStartLine; count++ )
		txl = txl->GetNext();

	if ( txl ) {
		do {
			txl->QueryExtents( rect );
			colRect.Union( rect );
			txl = txl->GetNext();
			count++;
		} while ( txl && count <= immediateRedisp.fStopLine );
	}

	if ( colRect.Valid() ) {
		if ( fFlowDir.IsHorizontal() ) {
			colRect.x1	= MIN( colRect.x1, 0 );
			colRect.x2	= MAX( colRect.x2, Width() );
		}
		else {
			colRect.y1	= MIN( colRect.y1, 0 );
			colRect.y2	= MAX( colRect.y2, Depth() );
		}
	}

	immediateRedisp.fImmediateRect = colRect;
}

/* ==================================================================== */
/* draw the line of text in the selection */

void scColumn::UpdateLine( scImmediateRedisp&	immediateRedisp, 
						   APPDrwCtx			mat )
{
	scTextline* paintTxl;
	short		count;
	scMuPoint	tx( 0, 0 );

	paintTxl = fFirstline;
	for ( count = 1; paintTxl != NULL && count < immediateRedisp.fStartLine; count++ )
		paintTxl = paintTxl->GetNext();

	if ( paintTxl != NULL ) {
		do {
			paintTxl->Draw( mat, GetFlowdir(), tx );
			paintTxl = paintTxl->GetNext();
			count++;
		} while ( paintTxl != NULL && count <= immediateRedisp.fStopLine );
	}
}

/* ==================================================================== */
/* draw the portions of the column that intersect the 'damagedRectangle' */

void scColumn::Draw( const scXRect& 	dRect, 
					 APPDrwCtx			dc,
					 const scMuPoint*	translation )
{
	scTextline* txl;
	scXRect 	exRect;
	scMuPoint	tx( 0, 0 );

	if ( translation )
		tx += *translation;

	for ( txl = GetFirstline(); txl != NULL; txl = txl->GetNext() ) {
		txl->QueryExtents( exRect, 1 );
		if ( exRect.Intersect( dRect )	) { 		
			txl->Draw( dc, fFlowDir, tx );
			txl->Unmark( scREPAINT );
		}
	}
}

/* ==================================================================== */
/* read from a text file */

void scColumn::ReadTextFile( TypeSpec		spec,
							 APPCtxPtr		ctxPtr, 				
							 IOFuncPtr		readFunc,
							 scRedispList*	redispList )
{
	scColumn*	startCol;

	scCachedStyle::SetFlowdir( GetFlowdir() );
	scCachedStyle::GetCachedStyle( spec );

	startCol = (scColumn*)FirstInChain();

	if ( GetStream() )
		GetStream()->RemoveEmptyTrailingParas( GetFlowset() );

	if ( startCol->GetStream() == NULL )
		SetFlowsetStream( scStream::ReadTextFile( spec, ctxPtr, readFunc, 0 ) );
	else
		startCol->GetStream()->Append( scStream::ReadTextFile( spec, ctxPtr, readFunc, 0 ) );

	startCol->Mark( scINVALID );
	startCol->LimitDamage( redispList, scReformatTimeSlice );	   /* reBreak */
}

/* ==================================================================== */
/* paste APPText into a text container */

void scColumn::PasteAPPText( stTextImportExport&		appText, 
							 scRedispList*	redispList )
{
	scColumn*	firstCol;
	TypeSpec	nullSpec;

	if ( GetStream() )
		GetStream()->RemoveEmptyTrailingParas( GetFlowset() );

	firstCol = GetFlowset();

	if ( !fStream )
		firstCol->SetFlowsetStream( scStream::ReadAPPText( appText ) );
	else
		fStream->Append( scStream::ReadAPPText( appText ) );

	firstCol->Mark( scINVALID );
	firstCol->LimitDamage( redispList, scReformatTimeSlice );		 /* reBreak */
}

/* ==================================================================== */
/* upon completion of reading data in from disk we search down the column list
 * finding the first columns in a chain and retabulate, rebreak and repaint
 */

void scColumn::Update( scRedispList *redispList )
{
	scColumn*	flowset;
	scColumn*	col;

	for ( col = GetBaseContextList(); col; col = col->GetContext() ) {
		if ( col->Marked( scINVALID ) && col->GetRecomposition() ) {
			flowset = (scColumn*)col->FirstInChain();
			
			scCachedStyle::SetFlowdir( flowset->GetFlowdir() );
					
			flowset->LimitDamage( redispList, scReformatTimeSlice );

			scColumn* p = flowset;
			for ( ; p; p = p->GetNext() )
				p->Unmark( scINVALID );
		}
	}
}

/* ==================================================================== */
/* this is still a little dirty - needs to be cleaned up a bit */
/* reformat all columns containing ts */

void scColumn::ChangedTS( TypeSpec		theTS,
						  eSpecTask 	task,
						  scRedispList* redispList )
{
	scColumn*	aColH;
	scContUnit* p;
	scTextline* txl;

	scCachedStyle::StyleInvalidateCache( theTS );
	
	for ( aColH = GetBaseContextList( ); aColH; aColH = aColH->GetContext() ) {
		if ( aColH->GetCount() == 0 ) {

			scCachedStyle::SetFlowdir( aColH->GetFlowdir() );

			p = aColH->GetStream(); 			
			for ( ; p; p = p->GetNext( ) ) {
				if ( p->ContainTS( theTS ) ) {
					if ( !aColH->GetRecomposition() ) {
						if ( ( txl = p->GetFirstline() ) != NULL ) {
							scColumn * colH;

							if ( ( colH = txl->GetColumn() ) != NULL )
								colH->Mark( scINVALID );
						}
						else
							aColH->Mark( scINVALID );

						if ( task & eSCRetabulate )
							p->Mark( scRETABULATE );
						if ( task & eSCRebreak )
							p->Mark( scREBREAK );
						if ( task & eSCRepaint )
							p->ForceRepaint( 0L, LONG_MAX );
					}
					else {
						if ( task & eSCRetabulate )
							p->Retabulate( theTS );
						if ( task & eSCRebreak )
							p->Mark( scREBREAK );
						if ( task & eSCRepaint )
							p->ForceRepaint( 0L, LONG_MAX );
					}

				}
			}
			if ( aColH->GetRecomposition() && aColH->GetStream() )
				STRReformat( aColH, aColH->GetStream(), scReformatTimeSlice, redispList );
			else {
				scSelection* select = aColH->FlowsetGetSelection();
				select->UpdateSelection( );
			}
		}
	}
}

/* ==================================================================== */
/* search a column building a list of typespecs that are contained
 * in the column
 */

void scColumn::GetTSList( scTypeSpecList& tsList ) const

{
	scTextline* txl;

	for ( txl = GetFirstline(); txl; txl = txl->GetNext() )
		txl->GetTSList( tsList );
}

/* ==================================================================== */
/* determine the prev column with a line in it */

scColumn* scColumn::PrevWithLines() const
{
	scColumn* col;
	
	for ( col = GetPrev(); col ; col = col->GetPrev() ) {
		if ( col->GetFirstline( ) )
			return col;
	}
	return 0;
}

/* ==================================================================== */
/* determine the next column with a line in it */

scColumn* scColumn::NextWithLines( ) const
{
	scColumn* col;

	for ( col = GetNext(); col; col = col->GetNext( ) ) {
		if ( col->GetFirstline( ) )
			return col;
	}
	return 0;
}

/* ==================================================================== */
/* return the last line in this column */

scTextline* scColumn::GetLastline( ) const
{
	scTextline* txl;
	scTextline* validLine = 0;

	for ( txl = fFirstline; txl; txl = txl->GetNext() ) {
		if ( !txl->Marked( scINVALID ) ) {
			validLine = txl;
		//	validLine->AssertValid();
		}
	}
	return validLine;
}

/* ==================================================================== */
/* mark all the paras contained within this container to be rebroken */

scContUnit* scColumn::MarkParas( )
{
	scContUnit* firstPara;
	scContUnit* lastPara;
	scContUnit* para;
	scColumn*	contentCol;

	firstPara = FirstPara();

	if ( firstPara ) {
			// in this case the container has some lines
		lastPara = LastPara();
		for ( para = firstPara; para; para = para->GetNext() ) {
			para->Mark( scREBREAK );
			if ( para == lastPara )
				break;
		}
	}
	else {
		/* in this case the container has no lines,
		 * we must try to find a neighbor that has
		 * some lines, first we look backwards and then
		 * we look forwards, we mark what we find and
		 * see if they will reformat into the container
		 */
		if ( !GetPrev() )
			firstPara = GetStream();
		else {
			contentCol = PrevWithLines();
			if ( contentCol )
				firstPara = contentCol->LastPara();
			else {
				contentCol = NextWithLines( );
				if ( contentCol )
					firstPara = contentCol->FirstPara();
			}
			if ( !firstPara ) {
				/* this would be executed if no containers had lines
				 * attached to them
				 */
				firstPara = GetStream();
			}
		}
		if ( firstPara )
			firstPara->Mark( scREBREAK );
	}
	return firstPara;
}

/* ==================================================================== */
/* return the paragraph of the last line of text in this column */

scContUnit* scColumn::LastPara( ) const
{
	scTextline* txl  = GetLastline( );

	for ( ; txl && txl->Marked( scINVALID ); txl = txl->GetPrev() )
		;
	
	return txl ? txl->GetPara() : NULL;
}

/* ==================================================================== */
// return the first valid paragraph of this column

scContUnit* scColumn::FirstPara( ) const
{
		// if no previous column the first guy in the stream is
		// the first paragraph
	if ( !GetPrev() )
		return fStream ? fStream->First() : 0;

	
	scColumn*	prev	= GetPrev();
	scTextline* txl 	= 0;
	scContUnit* p		= 0;

		// get last valid line in prev para, presumably
		// the container has been reformatted
	do {
		txl  = prev->GetLastline();
		if ( !txl )
			prev = prev->GetPrev();
	} while ( prev && !txl );

		// get the paragraph of the last line, check to
		// see if the end of the line represents the end
		// of the paragraph, if it does go to the next para
	if ( txl ) {
		p = txl->GetPara();
		
		if ( txl->GetEndOffset() == p->GetContentSize() )
			p = p->GetNext();
	}
	else 
		p = fStream ? fStream->First() : 0;
	
	return p;
}

/* ==================================================================== */
/* Delete excess lines in the column						*/

void scColumn::DeleteExcessLines( scContUnit*	para,
								  scTextline*	lastTxl,
								  BOOL			testGetStrip,
								  scCOLRefData& colReformatData )
{
	scTextline* txl;
	scTextline* nextTxl;
	BOOL		deleteLines 	= false;

	if ( lastTxl ) {
		if ( ( txl = LNNext( lastTxl ) ) != NULL )
			deleteLines = true;
	}
	else if ( ( txl = GetFirstline() ) != NULL ) {

		if (  para == NULL || para->GetCount() <= txl->GetPara()->GetCount() ) {
			if ( ! testGetStrip )
				deleteLines = true;
			else {
				scLINERefData	lineData;
				
				scCachedStyle::SetFlowdir( GetFlowdir() );
				TypeSpec ts = txl->SpecAtStart();
				scCachedStyle::GetCachedStyle( ts );

				lineData.fOrg				= txl->GetOrigin();
				lineData.fMeasure			= txl->GetMeasure();
				lineData.fLogicalExtents	= scCachedStyle::GetCurrentCache().GetLogicalExtents( );
				lineData.fInitialLead.Set( scCachedStyle::GetCurrentCache().GetComputedLead(), scCachedStyle::GetCurrentCache().GetFlowdir() );

				if ( !GetStrip( lineData, eStartColBreak, colReformatData ) )
						/* the first line will not fit, delete them */
					deleteLines = true; 
			}
		}
	}

	if ( deleteLines ) {
		Mark( scREPAINT );		/* if we delete we need to repaint */
		for ( ; txl; txl = nextTxl ) {
			nextTxl = txl->GetNext();
			txl->Delete( colReformatData.fLineDamage );
		}
	}
}

/* ==================================================================== */

void scColumn::SetFlowsetStream( scStream* cu )
{
	scColumn* col;
	
	for ( col = (scColumn*)FirstInChain(); col; col = col->GetNext() )
		col->SetStream( cu );
}

/* ==================================================================== */
/* free the stream from the column chain */

void scColumn::FreeStream()
{
	if ( fStream ) {
		fStream->STRFree( );
		SetFlowsetStream( 0 );
	}
}

/* ==================================================================== */
/* force the rebreaking of this column */

void scColumn::Rebreak( scRedispList* redispList )
{
		// save the recomposition state
	BOOL	saveRecomposeFlag	= GetRecomposition();

	SetRecomposition( true );

	Rebreak2( redispList );

		// restore the saved value
	SetRecomposition( saveRecomposeFlag );

	Unmark( scINVALID );
}

/* ==================================================================== */
/* rebreak of this column */

void scColumn::Rebreak2( scRedispList* redispList )
{
	Mark( scINVALID );
	
	LimitDamage( redispList, scReformatTimeSlice );
}

/* ==================================================================== */
/* give the column a new width & depth, rebreak and return damaged areas */
/* the column measure and/or depth has changed respond accordingly
 * OBVIOUS OPTIMIZATIONS
 *	  if depth increases just add stuff
 */

void scColumn::Resize( MicroPoint		width,
					   MicroPoint		depth, 
					   scRedispList*	redispList )
{
	switch ( GetShapeType() ) {
		case eRgnShape:
		case eVertShape:
			SetSize( width, depth );
			return;
		case eNoShape:
			SetSize( width, depth );
			break;
		case eVertFlex:
			SetWidth( width );
			break;
		case eHorzFlex:
			SetDepth( depth );		
			break;
		case eFlexShape:
			SetSize( width, depth );
			break;
	}
	Mark( scINVALID );
	LimitDamage( redispList, scReformatTimeSlice );
}

/* ==================================================================== */

void scColumn::Resize( const scSize&	newSize, 
					   scRedispList*	redispList )
{
	switch ( GetShapeType() ) {
		case eRgnShape:
		case eVertShape:
			SetSize( newSize );
			return;
		case eNoShape:
			SetSize( newSize );
			break;
		case eVertFlex:
			SetWidth( newSize.Depth() );
			break;
		case eHorzFlex:
			SetDepth( newSize.Width() );		
			break;
		case eFlexShape:
			SetSize( newSize );
			break;
	}
	Mark( scINVALID );
	LimitDamage( redispList, scReformatTimeSlice );
}

/* ==================================================================== */
/* ENUMERATE THE COLUMN AND ITS STRUCTURES */


void scColumn::Enumerate( long& objEnumerate )
{
	scTBObj::Enumerate( objEnumerate );
		
		// if the column has no previous members, that is it is 
		// the first column of a set of linked columns, enumerate 
		// the paragraphs and their text
		//
	if ( !Prev() && fStream )
		fStream->DeepEnumerate( objEnumerate );
}


/* ==================================================================== */

/* return the size of this column for storage purposes, the text stream
 * is always stored with the first column, subsequent columns store
 * just the container itself. (this may present problems for paging of text
 * in multipage documents)
 */

void scColumn::ExternalSize( long& exSize )
{
	scContUnit* para;

	exSize = sizeof(scColumn);

	if ( !GetPrev() ) {
		for ( para = GetStream(); para; para = para->GetNext( ) )
			exSize += para->ExternalSize();
	}
	switch ( GetShapeType() ) {

		case eVertShape:
#ifdef ColumnPolygon
			exSize += POLYExternalSize( fVertH, fShapePieces );
#endif /* ColumnPolygon */
			break;

		case eRgnShape:
			exSize += RGNExternalSize( fRgnH, fShapePieces );
			break;
	}
	exSize += sizeof( scTBObj );		/* NULL OBJECT */
}

/* ==================================================================== */

void scColumn::ZeroEnumeration(  )
{
	ZeroEnum();
	
	if ( !GetPrev() )
		GetStream()->STRZeroEnumeration();		
}

/* ==================================================================== */
/* determine extents of the column in its local coordinates */


void scColumn::ComputeInkExtents( )
{
	scXRect 	lineExtents;
	scTextline* txl;
	
	/* clear rect */
	fInkExtents.Set( 0, 0, 0, 0 );
	/* add each line to the current extents */
	for ( txl = fFirstline; txl; txl = LNNext( txl ) ) {
		txl->QueryExtents( lineExtents, 1 );
		if ( lineExtents.Width() <= 0 )
			lineExtents.x2 = lineExtents.x1 + 1;
		fInkExtents.Union( lineExtents );
	}
}

/* ==================================================================== */
/* determine extents of the column in its local coordinates */

	static void COLQueryMarginsVertical( const scColumn*	col, 
										 scXRect&			margins,
										 int				shapeType )
	{	
		scTextline	*txl;
		scMuPoint	translate;
		TypeSpec	spec;
		scXRect 	xrect2;
		
		switch ( shapeType ) {
			case eHorzFlex:
				txl = col->GetFirstline();
				
				if ( txl ) {				
					margins.Set( txl->GetOrigin().x,
								 txl->GetOrigin().y, 
								 txl->GetOrigin().x + CSfirstLinePosition( col->GetAPPName(), txl->SpecAtStart( ) ),
								 col->Depth() );
	
					for ( txl = col->GetFirstline(); txl; txl = LNNext( txl ) ) {
						xrect2.Set( txl->GetOrigin().x,
									 txl->GetOrigin().y, 
									 txl->GetOrigin().x + CSfirstLinePosition( col->GetAPPName(), txl->SpecAtStart( ) ),
									 col->Depth() );
						margins.Union( xrect2 );
					}
					
					txl = col->GetLastline( );
					txl->MaxLead( spec );
					xrect2.Set( txl->GetOrigin().x - CSlastLinePosition( col->GetAPPName(), spec ),
								txl->GetOrigin().y, 
								txl->GetOrigin().x,
								col->Depth() );
					
					margins.Union( xrect2 );				
				}
				break;
								
			case eFlexShape:	
				txl = col->GetFirstline();
				if ( txl ) {
					margins.Set( txl->GetOrigin().x,
								 txl->GetOrigin().y, 
								 txl->GetOrigin().x + CSfirstLinePosition( col->GetAPPName(), txl->SpecAtStart( ) ),
								 txl->GetMeasure() );
	
					for ( txl = col->GetFirstline(); txl; txl = LNNext( txl ) ) {
						xrect2.Set( txl->GetOrigin().x,
									 txl->GetOrigin().y, 
									 txl->GetOrigin().x + CSfirstLinePosition( col->GetAPPName(), txl->SpecAtStart( ) ),
									 txl->GetMeasure() );
						margins.Union( xrect2 );
					}
					
					txl = col->GetLastline( );
					txl->MaxLead( spec );
					xrect2.Set( txl->GetOrigin().x - CSlastLinePosition( col->GetAPPName(), spec ),
								txl->GetOrigin().y, 
								txl->GetOrigin().x,
								txl->GetMeasure() );
					
					margins.Union( xrect2 );				
				}
				break;
				
			case eVertFlex:
				margins.Set( 0, 0, col->Width(), 0 );
	
				for ( txl = col->GetFirstline(); txl; txl = txl->GetNext() )
					margins.y2 = MAX( txl->GetOrigin().y + txl->GetLength(), margins.y2 );
				break;
		}
	}
	
void scColumn::QueryMargins( scXRect& margins ) const
{
	scTextline	*txl;
	scTextline	*nextTxl;
	TypeSpec	spec;

	if ( GetFlowdir().IsVertical() ) {
		switch ( GetShapeType() ) {
			case eHorzFlex:
			case eVertFlex:
			case eFlexShape:
				COLQueryMarginsVertical( this, margins, GetShapeType() );
				return;
			default:
				break;
		}
	}

	switch ( GetShapeType() ) {
		case eFlexShape:
		case eVertFlex:

			if ( GetShapeType() == eFlexShape )
				margins.Set( 0, 0, 0, 0 );
			else
				margins.Set( 0, 0, Width(), 0 );

					/* add each line to the current extents */
			for ( txl = GetFirstline( ); txl; txl = nextTxl ) {

				if ( GetShapeType() == eFlexShape )
					margins.x2 = MAX( txl->GetOrigin().y + txl->GetLength(), margins.x2 );

				nextTxl = LNNext( txl );
				if ( !nextTxl ) {		/* last line */

					margins.y2 = MAX( txl->GetOrigin().y, margins.y2 );

						/* this makes vertical flex columns the size 
						 * of the text baseline plus whatever amount 
						 * of text the application wants to add to the bottom
						 */
					MicroPoint maxlead = txl->MaxLead( spec );
					if (spec.ptr()) {
						margins.y2 += CSlastLinePosition( GetAPPName(), spec );
					}

					margins.y2 += txl->GetVJOffset();
				}
			}
			break;

		case eHorzFlex:

			margins.Set( 0, 0, 0, Depth() );

			for ( txl = GetFirstline( ); txl; txl = txl->GetNext( ) )
				margins.x2 = MAX( txl->GetOrigin().x + txl->GetLength(), margins.x2 );
			break;

		case eVertShape:
		case eRgnShape:
		case eNoShape:
			margins.Set( 0, 0, Width(), Depth() );
			break;
	}
}

/* ==================================================================== */
/* determine maximum possible depth of the column in its local coordinates */

void scColumn::QuerySize( scSize& size ) const
{
	switch ( GetShapeType() ) {

#ifdef ColumnPolygon
		case eVertShape:
			size.SetDept( POLYMaxDepth( fVertH ) );
			break;
#endif /* ColumnPolygon */

		case eRgnShape:
			{
				scXRect xrect;

				RGNGetExtents( fRgnH, xrect );
					// this is open to some discussion
					// over what is correct
				size.SetWidth( xrect.x2 );
				size.SetDepth( xrect.y2 );
			}
			break;

		case eVertFlex:
		case eFlexShape:
			size.SetWidth( Width() );
			size.SetDepth( LONG_MAX );
			break;

		default:
		case eHorzFlex:
		case eNoShape:
			size	= GetSize();
			break;
	}
}

/* ==================================================================== */
/* Determine maximum depth of text from top (or from right in vertical) */

void scColumn::QueryTextDepth( MicroPoint& depth ) const
{
	switch ( GetShapeType()  ) {
		case eVertShape:
			depth = POLYMaxDepth( fVertH );
			break;

		case eRgnShape:
			depth = RGNMaxDepth( fRgnH );
			break;

		case eVertFlex:
			if ( GetFlowdir().IsVertical() ) {
				depth = TextDepth();
				break;
			}
		case eFlexShape:
			depth = LONG_MAX;
			break;

		case eHorzFlex:
			if ( GetFlowdir().IsVertical() ) {
				depth = LONG_MAX;
				break;
			}
		default:
		case eNoShape:
			depth = TextDepth();
			break;
	}
}

/* ==================================================================== */

MicroPoint scColumn::TextDepth( ) const
{
	return GetFlowdir().IsHorizontal() ? Depth() : Width();
}

/* ==================================================================== */

static BOOL COLLinkSetContains( scColumn * col1H, 
								scColumn * col2H )
{
	scColumn *	prevColH;

	/* backup */
	for ( ; col1H && (prevColH = col1H->GetPrev()) != NULL; 
				col1H = prevColH )
		;
	
	/* renumber */
	for ( ; col1H; col1H = col1H->GetNext() ) {
		if ( col1H == col2H )
			return true;
	}
	return false;
}

/* ==================================================================== */

void scColumn::Link( scColumn*		col2,
					 BOOL			reformat,
					 scRedispList*	redispList )
{
	scSelection*	select2 = 0;	

		// make sure the existing links make sense
	raise_if ( col2->GetPrev(), scERRstructure );

	raise_if( COLLinkSetContains( this, col2 ), scERRstructure );

		/* mark the paras in each to be rebroken */
	MarkParas( );		/* maybe we should only mark the last one */
	col2->MarkParas( );

	col2->FlowsetSetFlowdir( GetFlowdir() );

	if ( FlowsetGetSelection() && !col2->GetSelection() )
		;		// we are cool
	else if ( !FlowsetGetSelection() && col2->GetSelection() ) {
				// transfer selection
		select2 = col2->FlowsetGetSelection();
		col2->FlowsetRemoveSelection();
		FlowsetSetSelection( select2 );
	}
	else {
		select2 = col2->FlowsetGetSelection();
		col2->FlowsetRemoveSelection();
		delete select2;
		select2 = NULL;
	}
	
		// do the actual link
	scTBObj::Link( col2 );

		/* patch the stream(s)
		 * if either column has a stream we can deal with it easily,
		 * if both have it, append stream2 to stream1
		 */
	if ( GetStream() && !col2->GetStream() ) {
			/* col1 has a stream */ 	
		SetFlowsetStream( GetStream() );
	}
	else if ( col2->GetStream() && !GetStream() ) {
			/* col2 has a stream */
		SetFlowsetStream( col2->GetStream() );
	}
	else if ( GetStream() && col2->GetStream() ) {
			// both contain streams
		GetStream()->Append( col2->GetStream() );
		SetFlowsetStream( GetStream() );
	}
	else
		/* no column has a stream */;

		// renumber the streams
	Renumber();

		// patch selection
	
	if ( reformat ) {
		Mark( scINVALID );
		LimitDamage( redispList, scReformatTimeSlice );
	}
}

/* ==================================================================== */

void scColumn::Unlink( scRedispList* redispList )
{
	scColumn*	firstCol;
	scXRect 	lineDamage;

		// mark the paras in the container beings unlinked to be rebroken,
		// since they are losing their home, they definately need to
		// be rebroken
		//
	firstCol = GetPrev();
	if ( firstCol == NULL )
		firstCol = GetNext();

	if ( firstCol ) {
		MarkParas();
		FreeLines( true, lineDamage );	/* deletes lines */
		 
		if ( redispList )
			redispList->AddColumn( this, lineDamage );

		scTBObj::Unlink( );
		SetFlowsetStream( 0 );

		firstCol->Renumber( );
		firstCol->Mark( scINVALID );
		firstCol->LimitDamage( redispList, scReformatTimeSlice );
	}
}

/* ==================================================================== */

void scColumn::BreakChain( scColumn* col2 )
{
	raise_if( GetNext() != col2, scERRstructure );

	if ( GetStream() )
		GetStream()->STRDeformat(); 	// remove any layout information

		// break the link
	SetNext( 0 );
	col2->SetPrev( 0 );
	
	col2->SetFlowsetStream( 0 );	// set the stream in col 2 to nothing
}

/* ==================================================================== */

void scColumn::InvertExtents( HiliteFuncPtr func, 
							  APPDrwCtx 	mat )
{
	scTextline* txl;
	
	for ( txl = GetFirstline( ); txl; txl = txl->GetNext() )
		txl->InvertExtents( func, mat );
}

/* ==================================================================== */
/* set the flow direction of the container */

void scColumn::FlowsetSetFlowdir( const scFlowDir& flowDir )
{
	scColumn*	col = GetFlowset();
	
	for ( ; col != 0; col = col->GetNext() ) {
		if ( col->GetFlowdir() != flowDir ) {
			col->SetFlowdir( flowDir );
			col->Mark( scINVALID );
			scStream* str = col->GetStream();
			if ( str )
				col->GetStream()->STRMark( scRETABULATE | scREBREAK );
		}
	}
	scCachedStyle::SetFlowdir( flowDir );
	GetFlowset()->LimitDamage( 0, scReformatTimeSlice );
}

/* ==================================================================== */

void scFlowDir::SetFlow( eCommonFlow cf )
{
	if ( cf == eNoFlow ) {
		linedir_ = eInvalidFlow;
		glyphdir_ = eInvalidFlow;
	}
	else if ( cf == eRomanFlow ) {		
		linedir_ = eTopToBottom;
		glyphdir_ = eLeftToRight;
	} 
	else if ( cf == eVertJapanFlow ) {
		linedir_ = eRightToLeft;
		glyphdir_ = eTopToBottom;
	}
	else if ( cf == eBidiFlow ) {
		linedir_ = eTopToBottom;
		glyphdir_ = eRightToLeft;
	}
}

/* ==================================================================== */

eCommonFlow scFlowDir::GetFlow() const
{
	if ( linedir_ == eTopToBottom && glyphdir_ == eLeftToRight )
		return eRomanFlow;
	else if ( linedir_ == eRightToLeft && glyphdir_ == eTopToBottom )
		return eVertJapanFlow;
	else if ( linedir_ == eTopToBottom && glyphdir_ == eRightToLeft )
		return eBidiFlow;
	return eNoFlow;
}

/* ==================================================================== */

#if SCDEBUG > 1

void scColumn::DbgPrintInfo( int debugLevel  ) const
{
	SCDebugTrace( debugLevel, scString( "\nSCCOLUMN 0x%08x - firstline 0x%08x\n" ), this, fFirstline );

	scTextline* txl;

	for ( txl = fFirstline; txl; txl = txl->GetNext() )
		txl->DbgPrintInfo( debugLevel );
}

#endif
