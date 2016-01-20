/***********************************************************************

	File:		SCTEXTLI.C


	$Header: /Projects/Toolbox/ct/SCTEXTLI.CPP 2	 5/30/97 8:45a Wmanis $

	Contains:	Implementation of scTextlin class.

	Written by: Manis


	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

*****************************************************************************/

#include "sctextli.h"
#include "sccallbk.h"
#include "sccolumn.h"
#include "scmem.h"
#include "scstcach.h"
#include "scparagr.h"
#include "scset.h"
#include "screfdat.h"
#include "scbreak.h"
#include "scselect.h"


#define LETTERSPACE( ch ) ( (ch)->character!=wordSpace \
									&& ((ch)+1)->character!=wordSpace )

/* ==================================================================== */

scTextline& scTextline::operator=( const scTextline& tl )
{
	scTBObj::operator=( tl );
	
	fPara			= tl.fPara;
	fStartOffset	= tl.fStartOffset;
	fEndOffset		= tl.fEndOffset;
	fColumn 		= tl.fColumn;
	fOrigin 		= tl.fOrigin;
	fInkExtents 	= tl.fInkExtents;
	fVJOffset		= tl.fVJOffset;
	fLength 		= tl.fLength;
	fCursorY1		= tl.fCursorY1;
	fCursorY2		= tl.fCursorY2;
	fMaxLeadSpec	= tl.fMaxLeadSpec;
	fMaxLead		= tl.fMaxLead;
	fLineCount		= tl.fLineCount;
	fLspAdjustment	= tl.fLspAdjustment;

	return *this;
}

/* ==================================================================== */

void scTextline::MarkForDeletion( void )
{
	scXRect xrect = QueryExtents( xrect, 1 );
	
	if ( fPara && fPara->GetFirstline() == this )
		fPara->SetFirstline( 0 );

		// de-normalize extents
	if ( fColumn->GetFlowdir().IsVertical() )
		xrect.Translate( fVJOffset, 0 );	
	else								
		xrect.Translate( 0, -fVJOffset );
	
	fInkExtents 	= xrect;
	fPara			= 0; 
	fLineCount		= -1;
	fStartOffset	= -1;
	fEndOffset		= -1;
	Mark( scINVALID );
}	

/* ==================================================================== */

#if SCDEBUG > 1

void scTextline::scAssertValid( Bool recurse ) const
{ 
	scTBObj::scAssertValid(); 
	scAssert( !Marked( scINVALID ) );
	scAssert( fMaxLeadSpec != 0 );
	 
	if ( recurse ) { 
		fColumn->scAssertValid( false );
		fPara->scAssertValid( false );
	}
	else {
		scAssert( fColumn != 0 );
		scAssert( fPara != 0 );
	}
	scAssert( fStartOffset >= 0 && fEndOffset <= fPara->GetContentSize() );
}

#endif

/* ==================================================================== */

scTextline* scTextline::GetPrevLogical( void ) const
{
	if ( fPrev )
		return GetPrev();

	scColumn* col = fColumn;

	do {
		col = col->GetPrev();
	} while ( col && !col->GetFirstline() );
	
	return col ? col->GetLastline( ) : 0;
}

/* ==================================================================== */	

scTextline* scTextline::GetNextLogical( void ) const
{
	if ( fNext )
		return GetNext();

	scColumn* col = fColumn;

	do {
		col = col->GetNext();
	} while ( col && !col->GetFirstline() ) ;
	
	return col ? col->GetFirstline() : 0;
}

/* ==================================================================== */	
/* instantiate a new line and initialize to the passed in parameters */

scTextline::scTextline( scContUnit* p,
						scColumn*	col,
						scTextline* prev ) :
							fPara( p ),
							fStartOffset( 0 ),
							fEndOffset( 0 ),
							fColumn( col ),
							fOrigin( 0, 0 ),
							fVJOffset( 0 ),
							fLength( 0 ),
							fCursorY1( 0 ),
							fCursorY2( 0 ),
							fMaxLeadSpec( 0 ),
							fMaxLead( 0 ),
							fLineCount( 0 ),
							fLspAdjustment( 0 )
{
	SetPrev( prev );

	if ( prev )
		prev->SetNext( this );
	else if ( col ) // this has no antecedent so lets set it to the column
		col->SetFirstline( this );
}		

/* ==================================================================== */

scTextline::~scTextline()
{
	scTBObj::scAssertValid();

//	if ( fPara ) {
//		fPara->scTBObj::scAssertValid();
//		scAssert( fPara->GetFirstLine() != this ); 
//	}					  
//	char buf[256];
//	SCDebugTrace( scString( "scTextline::~scTextline 0x%08x (para 0x%08x) %s\n" ), this, fPara, fInkExtents.DebugStr( buf, scTwipsFactor*20 ) );
}


/* ==================================================================== */
/* delete this line from the data structure */

void scTextline::Delete( scXRect& lineDamage )
{
		// patch the line pointers
	if ( GetPrev() )
		GetPrev()->SetNext( GetNext() );
	else
		GetColumn()->SetFirstline( GetNext() );

	if ( GetNext() )
		GetNext()->SetPrev( GetPrev() );

		// patch the paragraph pointer
	if ( fPara && this == fPara->GetFirstline() ) {
		scTextline* nexttxl = GetNext();
		
		if ( !nexttxl )
			nexttxl = GetNextLogical();

		if ( nexttxl && nexttxl->GetPara() == fPara )
			fPara->SetFirstline( nexttxl );
		else
			fPara->SetFirstline( 0 );
	}
		// compute the damage area
	scXRect xrect( fInkExtents );
	if ( xrect.Valid() ) {
		QueryExtents( xrect, 1 );
		lineDamage.Union( xrect );
	}

	delete this;
}

/* ==================================================================== */

Bool scTextline::TestOffsets( scTextline*				orgTxl,
							  const scStreamChangeInfo& sc )
{
	if ( GetPara() == sc.GetPara() ) {
		if ( GetStartOffset() > sc.GetOffset() )
			return GetStartOffset() - sc.GetLength() == orgTxl->GetStartOffset() &&
				   GetEndOffset() - sc.GetLength()	 == orgTxl->GetEndOffset();
	}	
	return GetStartOffset() == orgTxl->GetStartOffset() && 
		   GetEndOffset()	== orgTxl->GetEndOffset();
}

/* ==================================================================== */

Bool scTextline::Compare( scTextline*				orgTxl,
						  const scStreamChangeInfo& streamChange )
{
#if 1
	if ( fPara != orgTxl->GetPara() )
		return false;
		 
	if ( fOrigin != orgTxl->fOrigin )
		return false;
	

	if ( !TestOffsets( orgTxl, streamChange ) )
		return false;

	if ( fVJOffset != orgTxl->fVJOffset )
		return false;

	if ( fMaxLeadSpec != orgTxl->fMaxLeadSpec )
		return false;

	if ( fLength != orgTxl->fLength )
		return false;

	if ( fLineCount != orgTxl->fLineCount )
		return false;

	if ( fInkExtents != orgTxl->fInkExtents )
		return false;

	if ( Marked( scLASTLINE ) != orgTxl->Marked( scLASTLINE ) )
		return false;
				 
	if ( fCursorY1 != orgTxl->fCursorY1 )
		 return false;

	if ( fCursorY2 != orgTxl->fCursorY2 )
		return false;

	if ( fColumn != orgTxl->fColumn )
		return false;

	if ( fLspAdjustment != orgTxl->fLspAdjustment )
		return false;

	return true;
	


#else
	Bool	notChanged;

	if ( GetPara()					== orgTxl->GetPara()			&&
		 fOrigin					== orgTxl->fOrigin				&&
		 TestOffsets( orgTxl, p, offset, len )						&&		 
		 fVJOffset					== orgTxl->fVJOffset			&&
		 fMaxLeadSpec				== orgTxl->fMaxLeadSpec 		&&
		 fLength					== orgTxl->fLength				&&
		 fLineCount 				== orgTxl->fLineCount			&&
		 fInkExtents				== orgTxl->fInkExtents			&&
		 Marked( scLASTLINE )		== orgTxl->Marked( scLASTLINE ) &&
		 fCursorY1					== orgTxl->fCursorY1			&&
		 fCursorY2					== orgTxl->fCursorY2			&&
		 fColumn					== orgTxl->fColumn				&&
		 fLspAdjustment 			== orgTxl->fLspAdjustment )
				notChanged = true;
	else
		notChanged = false;

	return notChanged;
#endif	
}

/* ==================================================================== */

scTextline *LNInsertNew( scContUnit*	para,
						 scColumn*		col,
						 scTextline*	next )
{
	scTextline* prev;
	scTextline* txl = scTextline::Allocate( );	

	txl->Insert( next );

	txl->SetPara( para );
	txl->SetColumn( col );
	prev			= txl->GetPrev();

	if ( !prev )/* this has no antecedent so lets set it to the column */
		col->SetFirstline( txl );
	
	return txl;
}

/* ==================================================================== */
// update the lines instance variables, determining if there have
// been any changes in the line

void scTextline::Set( short 			lineCount,
					  eBreakType		breakType,
					  scLINERefData&	lineData )
{
	Bool	changed;
	scXRect exrect = lineData.fInkExtents;
	
	scAssert( lineData.fInkExtents.Valid() );
	
	TypeSpec ts = lineData.GetMaxLeadSpec();
	scCachedStyle& cs = scCachedStyle::GetCachedStyle( ts );

	if ( fOrigin == lineData.fOrg
			&& fLength					== lineData.fComputedLen
			&& GetCharCount()			== lineData.GetCharCount()
			&& fLineCount				== lineCount
			&& fLspAdjustment			== lineData.fLetterSpace
			&& fInkExtents				== lineData.fInkExtents
			&& Marked( scLASTLINE ) 	&& ( breakType == eEndStreamBreak ) 
			&& fMaxLead 				== lineData.fEndLead.GetLead()
			&& fMaxLeadSpec 			== lineData.GetMaxLeadSpec() ) {
				changed = false;
	}
	else
		changed = true;


	if ( !changed ) {
		if ( fColumn->GetFlowdir().IsVertical() ) {
			changed = ( fCursorY1 == lineData.fOrg.x + cs.GetCursorX1() )		&& 
						( fCursorY2 == lineData.fOrg.x + cs.GetCursorX2() );
		}
		else {
			changed = ( fCursorY1 == lineData.fOrg.y + cs.GetCursorY1() )		&& 
						( fCursorY2 == lineData.fOrg.y + cs.GetCursorY2() );
		}
	}

	SetOffsets( lineData.GetStartCharOffset(), lineData.GetEndCharOffset() );
	if ( changed ) {
		fOrigin 			= lineData.fOrg;
		fVJOffset			= 0;
		fLength 			= lineData.fComputedLen;
		fLineCount			= lineCount;
		if ( breakType == eEndStreamBreak )
			Mark( scLASTLINE );
		else
			Unmark( scLASTLINE );
		fLspAdjustment		= lineData.fLetterSpace;
		fInkExtents 		= lineData.fInkExtents;
		fMaxLead			= lineData.fEndLead.GetLead();
		fMaxLeadSpec		= lineData.GetMaxLeadSpec();		// fMaxLeadSpec
		
		
		if ( fColumn->GetFlowdir().IsVertical() ) {
			fCursorY1			= lineData.fOrg.x + cs.GetCursorX1();
			fCursorY2			= lineData.fOrg.x + cs.GetCursorX2();
			lineData.fInkExtents.x1 = fCursorY1;
			lineData.fInkExtents.x2 = fCursorY2;
			if ( !lineData.GetStartCharOffset() && scCachedStyle::GetParaStyle().GetNumbered() )
				lineData.fInkExtents.y1 -= scCachedStyle::GetParaStyle().GetBulletIndent();
		}
		else {
			fCursorY1			= lineData.fOrg.y + cs.GetCursorY1();
			fCursorY2			= lineData.fOrg.y + cs.GetCursorY2();
			lineData.fInkExtents.y1 = fCursorY1;
			lineData.fInkExtents.y2 = fCursorY2;
			if ( !lineData.GetStartCharOffset() && scCachedStyle::GetParaStyle().GetNumbered() )
				lineData.fInkExtents.x1 -= scCachedStyle::GetParaStyle().GetBulletIndent();

		}
		fInkExtents.Union( lineData.fInkExtents );
	}

	if ( fColumn->GetFlowdir().IsVertical() && lineData.fColShapeType & eVertFlex )
		SetFlexLineAdjustment( lineData.fRagSetting );		
	else if ( lineData.fColShapeType & eHorzFlex )
		SetFlexLineAdjustment( lineData.fRagSetting );
	else
		SetFlexLineAdjustment( eNoRag );

	lineData.fLastLineLen = lineData.fComputedLen;

	if ( changed )
		fColumn->Mark( scREPAINT );

	scAssertValid( false );
}


/* ==================================================================== */
/* reposition the line, this line is part of a flex column and it may
 * need repositioning depending on the line rag
 * returns the next line - thus optimizing loop in column a little
 */
 
void scTextline::Reposition( MicroPoint measure )
{
	MicroPoint	displacement;
	CharRecordP chP;
	scFlowDir	flowDir;
	scMuPoint	transPt( fOrigin );

	flowDir = fColumn->GetFlowdir( );
	
	if ( flowDir.IsHorizontal() ) {
		transPt.x = -transPt.x;
		transPt.y = 0;
	}
	else {
		transPt.x = 0;
		transPt.y = -transPt.y;
	}

	switch ( GetFlexLineAdjustment() &	eRagFlag ) {
		case eRagLeft:
			fOrigin.Translate( transPt );
			fInkExtents.Translate( transPt );
				
			displacement	= measure - fLength;
			if ( flowDir.IsHorizontal() ) {
				fOrigin.Translate( displacement, 0 );
				fInkExtents.Translate( displacement, 0 );
			}
			else {
				fOrigin.Translate( 0, displacement );
				fInkExtents.Translate( 0, displacement );
			}
			break;
			
		case eRagCentered:
			fOrigin.Translate( transPt );
			fInkExtents.Translate( transPt );
		
			displacement	= ( measure - fLength ) / 2;
			if ( flowDir.IsHorizontal() ) {
				fOrigin.Translate( displacement, 0 );
				fInkExtents.Translate( displacement, 0 );
			}
			else {
				fOrigin.Translate( 0, displacement );
				fInkExtents.Translate( 0, displacement );
			}
			break;

		case eRagJustified:
			if ( !Marked( scLASTLINE ) || GetFlexLineAdjustment() & eLastLineJust ) {
				chP = (CharRecordP)GetPara()->GetCharArray().Lock();
				if ( BRKJustify( chP, GetStartOffset(), GetEndOffset(), measure ) )
					Mark( scREPAINT );

				if ( flowDir.IsHorizontal() )
					fInkExtents.x2	= fInkExtents.x1 + measure + fInkExtents.Depth() / 2;
				else 
					fInkExtents.y2	= fInkExtents.y1 + measure + fInkExtents.Width() / 2;
				fLength 	= measure;
				GetPara()->GetCharArray().Unlock();
			}
			break;
		
		case eRagRight:
			break;
	}
}

/* ==================================================================== */

Bool scTextline::IsHyphenated( ) const
{
	CharRecordP charP;
	Bool		hyphenated;

	charP = (CharRecordP)fPara->GetCharArray().Lock();
	if ( fEndOffset )
		charP +=  fEndOffset - 1;
	hyphenated = charP->flags.IsHyphPresent() || ( charP->character == scBreakingHyphen );
	fPara->GetCharArray().Unlock();

	return hyphenated;
}

/* ==================================================================== */
/* return the extents of the line */

scXRect& scTextline::QueryExtents( scXRect& extents, int dcextended ) const
{
	extents = fInkExtents;

	if ( fColumn->GetFlowdir().IsVertical() )
		extents.Translate( -fVJOffset, 0 ); 
	else								
		extents.Translate( 0, fVJOffset );	

	if ( dcextended ) {
		scXRect dcextents;
		if ( fStartOffset == 0 && fEndOffset > 0 ) {
			scCachedStyle::SetParaStyle( fPara, fPara->GetDefaultSpec() );
			if ( scCachedStyle::GetParaStyle().GetDropCap() ) {
					// set up specs for drop cap stuff
				TypeSpec ts = SpecAtStart();
				scCachedStyle::GetCachedStyle( ts );
				GetDropCapExtent( dcextents );
				extents.Union( dcextents );
			}
		}
	}
	return extents;
}

/* ==================================================================== */
/* return the spec on the line with the maximum leading */

MicroPoint scTextline::MaxLead( TypeSpec& ts )
{
	ts = fMaxLeadSpec;
	return fMaxLead;
}

/* ==================================================================== */

void scTextline::Translate( const scMuPoint& translate )
{
	fOrigin.Translate( translate );
	fInkExtents.Translate( translate );
	
	scFlowDir	flowDir;
	flowDir = fColumn->GetFlowdir( );
	
	if ( flowDir.IsHorizontal() ) {
		fCursorY1	+= translate.y;
		fCursorY2	+= translate.y;
	}
	else {
		fCursorY1	+= translate.x;
		fCursorY2	+= translate.x;
	}	
}

/* ==================================================================== */
// return the flow direction of the parent column

const scFlowDir&	scTextline::GetFlowdir( void ) const
{
	scAssert( fColumn != 0 ); 
	
	return fColumn->GetFlowdir(); 
}

/* ==================================================================== */
/* hilite the line by calling a rectangle drawing routine (call back to 
 * application), in here we have to determine what has to be hilited 
 * (xored) on the line
 */
 

Bool scTextline::SortMarkers( const TextMarker*&	startTM,
							  MicroPoint&			startLoc, 
							  int32&				startOffset,
							  const TextMarker*&	endTM,
							  MicroPoint&			endLoc, 
							  int32&				endOffset,
							  Bool					isVertical )
{
	scMuPoint	startpt,
				endpt;
	

	if ( startLoc == LONG_MIN || startLoc == LONG_MAX || endLoc == LONG_MIN || endLoc == LONG_MAX ) {
		if ( startLoc == LONG_MIN ) {
			Locate( fStartOffset, startpt, eCursBackward );
			startOffset = fStartOffset;
		}
		else if ( startLoc == LONG_MAX ) {
			Locate( fEndOffset, startpt, eCursForward );
			startOffset = fEndOffset;			
		}
		else if ( startTM ) {
			long offset = startTM->fOffset;  
			Locate( offset, startpt, eCursBackward );
			startOffset = offset;
		}
		else if ( !isVertical ) {
			startpt.x = startLoc;
			startOffset = fStartOffset;
		}
		else {
			startpt.y = startLoc;
			startOffset = fStartOffset;
		}
			
		if ( endLoc == LONG_MIN ) {
			Locate( fStartOffset, endpt, eCursForward );
			endOffset = fStartOffset;
		}
		else if ( endLoc == LONG_MAX ) {
			Locate( fEndOffset, endpt, eCursForward );
			endOffset = fEndOffset;
		}
		else if ( endTM ) { 
			long offset = endTM->fOffset;
			Locate( offset, endpt, eCursForward );
			endOffset = offset;
		}
		else if ( !isVertical ) {
			endpt.x = endLoc;
			endOffset = fEndOffset;
		}
		else {
			endpt.y = endLoc;
			endOffset = fEndOffset;
		}
	}
	else if ( startTM && endTM ) {
		startOffset = startTM->fOffset;
		endOffset	= endTM->fOffset;

		if ( startTM->fOffset > endTM->fOffset ) {		
			const TextMarker*	pTM = endTM;
			endTM	= startTM;
			startTM = pTM;
			
			MicroPoint tmp	= endLoc;
			endLoc		= startLoc;
			startLoc	= tmp;
		}
		long offset = startTM->fOffset;
		Locate( offset, startpt, eCursBackward );
		if ( !isVertical )
			startpt.x = startLoc;
		else
			startpt.y = startLoc;
		
		offset = endTM->fOffset;
		Locate( offset, endpt, eCursForward );
		if ( !isVertical )
			endpt.x = endLoc;
		else
			endpt.y = endLoc;
	}
	if ( isVertical ) {
		startLoc	= startpt.y;
		endLoc		= endpt.y;
	}
	else {
		startLoc	= startpt.x;
		endLoc		= endpt.x;
	}
	
	if ( startLoc > endLoc ) {
		MicroPoint tmp	= endLoc;
		endLoc		= startLoc;
		startLoc	= tmp;
	}

	if ( startOffset > endOffset ) {
		int32 tmp	= endOffset;
		endOffset	= startOffset;
		startOffset = tmp;
	}

	return startLoc == endLoc;
}

/* ==================================================================== */

#ifdef SHOW_HILITE
#include <stdio.h>
#endif
/* ==================================================================== */

inline int TopMergeSelections( MicroPoint linediff, MicroPoint inkHeight )
{
	return linediff < inkHeight * 3;
}

/* ==================================================================== */

inline int BottomMergeSelections( MicroPoint linediff, MicroPoint inkHeight )
{
	return linediff < inkHeight * 3;
}

/* ==================================================================== */

MicroPoint scTextline::TopLead( void ) const
{
	const scTextline* prev = this;
	do {
		prev = (scTextline*)prev->fPrev;
	} while ( prev && fOrigin.y == prev->fOrigin.y );
	
	if ( prev )
		return (MicroPoint)( (fOrigin.y - prev->fOrigin.y) * 0.80); 	
	else
		return fCursorY1;			
}

/* ==================================================================== */

MicroPoint scTextline::BottomLead( void ) const
{
	const scTextline* next = (scTextline*)this;
	do {
		next = (scTextline*)next->fNext;
	} while ( next && fOrigin.y == next->fOrigin.y );

	if ( next )
		return (MicroPoint)( (next->fOrigin.y - fOrigin.y) * 0.20);
	else
		return fCursorY2;
}

/* ==================================================================== */

MicroPoint scTextline::TopHilite( void ) const
{
	const scTextline* prev = this;
	do {
		prev = (scTextline*)prev->fPrev;
	} while ( prev && fOrigin.y == prev->fOrigin.y );
	
	if ( prev && TopMergeSelections( fOrigin.y - prev->fOrigin.y, fCursorY2 - fCursorY1 ) )
		return (MicroPoint)( (prev->fOrigin.y - fOrigin.y) * 0.80) + fOrigin.y; 	
	else
		return fCursorY1;			
}

/* ==================================================================== */

MicroPoint scTextline::BottomHilite( void ) const
{
	const scTextline* next = (scTextline*)this;
	do {
		next = (scTextline*)next->fNext;
	} while ( next && fOrigin.y == next->fOrigin.y );

	if ( next && BottomMergeSelections( next->fOrigin.y - fOrigin.y, fCursorY2 - fCursorY1 ) )
		return (MicroPoint)( (next->fOrigin.y - fOrigin.y) * 0.20) + fOrigin.y;
	else
		return fCursorY2;		
}

/* ==================================================================== */

void scTextline::GetDropCapExtent( scXRect& xrect ) const
{
	DropCapInfo 	dcInfo;
	TypeSpec		ps = fPara->GetDefaultSpec();
	TypeSpec		ts = SpecAtStart();

	scCachedStyle::SetParaStyle( fPara, ps );

	if ( ::DCCompute( dcInfo, ps, ts, 0L, 0L, CharAtStart() ) ) {
		xrect.Set( dcInfo.dcMinX, dcInfo.dcMinY, dcInfo.dcMaxX, dcInfo.dcMaxY );
		xrect.Translate( fOrigin.x - dcInfo.dcLineOrgChange, fOrigin.y + fVJOffset );
	}
}

/* ==================================================================== */

Bool scTextline::DropCapHilite( APPDrwCtx		mat,
								HiliteFuncPtr	func ) const
{
	DropCapInfo 	dcInfo;
	TypeSpec		ps = fPara->GetDefaultSpec();	
	TypeSpec		ts = SpecAtStart();

	scCachedStyle::SetParaStyle( fPara, ps );

	if ( ::DCCompute( dcInfo, ps, ts, 0L, 0L, CharAtStart() ) ) {	
		scXRect xrect( dcInfo.dcMinX, dcInfo.dcMinY, dcInfo.dcMaxX, dcInfo.dcPosition.dcVOffset );
		//scXRect	xrect( dcInfo.dcMinX, dcInfo.dcMinY, dcInfo.dcMaxX, dcInfo.dcMaxY );
		
		xrect.Translate( fOrigin.x - dcInfo.dcLineOrgChange, fOrigin.y + fVJOffset );
		
		(*func)( xrect, mat, false );
		return true;
	}
	return false;
}

/* ==================================================================== */

void scTextline::Hilite( const TextMarker*	startTM,
						 MicroPoint 		startLoc,
						 const TextMarker*	endTM,				
						 MicroPoint 		endLoc,
						 APPDrwCtx			mat,
						 HiliteFuncPtr		func,
						 const scSelection& selection )
{
	scXRect 		xrect;
	int32			startOffset,
					endOffset;

	if ( startLoc==endLoc && (startLoc==LONG_MIN || startLoc==LONG_MAX) )
		return;

#if SCDEBUG > 1
	DbgPrintInfo( 2 );
#endif

	scCachedStyle::SetParaStyle( fPara, fPara->GetDefaultSpec() );

	if ( SortMarkers( startTM, startLoc, startOffset, 
					  endTM, endLoc, endOffset,
					  fColumn->GetFlowdir().IsVertical() ) ) {
		if ( !selection.IsSliverCursor() ) {
			if ( startTM && *startTM == selection.fMark && fPara->GetContentSize() )
				return;
			else if ( endTM && *endTM == selection.fPoint && fPara->GetContentSize()  )
				return;
			else
				endLoc = startLoc;// + one_point;
		}
	}
	else if ( scCachedStyle::GetParaStyle().GetDropCap() ) {
		if ( startOffset == 0 && endOffset > 0 )
			DropCapHilite( mat, func );
		startLoc = MAX( fOrigin.x, startLoc );
	}

	if ( fColumn->GetFlowdir().IsVertical() ) {
		xrect.Set( fCursorY1, startLoc, fCursorY2, endLoc );
		xrect.Translate( -fVJOffset, 0 );
	}
	else {
		MicroPoint top		= TopHilite( );
		MicroPoint bottom	= BottomHilite();
		xrect.Set( startLoc, top, endLoc, bottom );
		xrect.Translate( 0, fVJOffset );
	}


		// call back for rectangle drawing
	(*func)( xrect, mat, selection.IsSliverCursor() );
}

/* ==================================================================== */
/* inserts all the specs resident on this line into a spec list */

void scTextline::GetTSList( scTypeSpecList& tsList )
{
	fPara->OffsetGetTSList( fStartOffset, fEndOffset, tsList );
}

/* ==================================================================== */
/* return true if a spec exists on a line */

Bool scTextline::ContainTS( TypeSpec ts )
{
	scSpecRun&		run = fPara->GetSpecRun();

	int i = run.indexAtOffset( fStartOffset );

	do {
		if ( run[i].spec().ptr() == ts.ptr() )
			return true;			
	} while ( run[++i].offset() < fEndOffset );

	return false;
}

/* ==================================================================== */
// the spec at the start of the line 

TypeSpec scTextline::SpecAtStart( ) const
{
	return fPara->SpecAtOffset( fStartOffset );
}

/* ==================================================================== */

void scTextline::InvertExtents( HiliteFuncPtr	func,
								APPDrwCtx		mat ) const
{
	scXRect exRect = QueryExtents( exRect, 1 );
	
	(*func)( exRect, mat, false );
}

/* ==================================================================== */
/* the following exist to maintain parallel constructs in the file io stuff */

void LNAbort( scTBObj* txl )
{
	delete txl;
}

/* ==================================================================== */

void LNPtrRestore( scTBObj* obj, scSet* enumTable )
{
}

/* ==================================================================== */

Bool LNFromFile( APPCtxPtr	ctxPtr,
				   IOFuncPtr	readFunc,
				   scSet*		enumTable,
				   scTBObj& 	baseObj, 
				   scTBObj*&	obj )
{
	scTextline* txl;

	txl = SCNEW scTextline;
	enumTable->Set( baseObj.GetEnumCount(), txl );
 //   SCPIO_CompleteTextLine( ctxPtr, readFunc, txl, obj );

	obj = txl;
	return true;
}

/* ==================================================================== */

Bool scTextline::IsLastLinePara ( ) const
{
	Bool			tf;

	if ( Marked( scINVALID | scREPAINT ) )
		return true;

	scAssertValid( false );

	tf = Marked( scLASTLINE );
	if ( tf ) {
		if ( !fPara->Marked( scREBREAK ) )
			scAssert( GetEndOffset() == PARAChSize( fPara ) );
	}
	return tf;
}

/* ==================================================================== */

MicroPoint scTextline::GetBaseline() const
{
	return fColumn->GetFlowdir().IsVertical() ? fOrigin.x : fOrigin.y;
}

/* ==================================================================== */
// this determines the leading plus paragraph space between this - the
// last line of a paragraph - and the next paragraph

MicroPoint scTextline::ParaLead( scLEADRefData& 	lead,
								 const scFlowDir&	fd )
{
	TypeSpec		prevLeadSpec,
					curLeadSpec;
	MicroPoint		spaceAbove,
					spaceBelow;
	MicroPoint		paraspace;
	scLEADRefData	topPara;
	scLEADRefData	belowPara;

	scContUnit* 	nextPara;
	
	nextPara = GetPara()->GetNext();
	if ( !nextPara )
		return 0;
	
	MaxLead( prevLeadSpec );
	curLeadSpec = nextPara->SpecAtStart();

	if ( fd.IsHorizontal() ) {
		spaceBelow =	scCachedStyle::GetCachedStyle( prevLeadSpec ).GetPtSize();
		spaceAbove =	scCachedStyle::GetCachedStyle( curLeadSpec ).GetPtSize();
	}
	else {
		spaceBelow =	scCachedStyle::GetCachedStyle( prevLeadSpec ).GetSetSize();
		spaceAbove =	scCachedStyle::GetCachedStyle( curLeadSpec ).GetSetSize();
	}
	
	topPara.Set( spaceBelow, fd );	
	belowPara.Set( spaceAbove, fd );	

	paraspace		= scCachedStyle::GetParaSpace( GetPara(), nextPara );
	
	lead.SetBelowLead( topPara.GetBelowLead() );
	lead.SetAboveLead( belowPara.GetAboveLead() );
	lead.SetExternalSpace( paraspace );
	
	return lead.GetLead();
}

/* ==================================================================== */

void scTextline::GetLineInfo( scLineInfo& lineInfo ) const
{
	lineInfo.fLineID		= this;
	lineInfo.fOrg			= fOrigin;
	lineInfo.fInkExtents	= fInkExtents;
	lineInfo.fLogicalExtents.Set( 0, 0, 0, fLength );
	lineInfo.fAscenderHite	= 0;
	lineInfo.fXHite 		= 0;
	lineInfo.fCapHite		= 0;
	lineInfo.fDescenderHite = 0;
}

/* ==================================================================== */

#if SCDEBUG > 1

void scTextline::DbgPrintInfo( int debugLevel ) const
{
	scChar buf[256];
	scXRect inkextents;

	QueryExtents( inkextents );
	
	inkextents.DebugStr( buf, 1 );
	
	SCDebugTrace( debugLevel, scString( "SCTEXTLINE 0x%08x ( <- 0x%08x -> 0x%08x ) fPara 0x%08x ( %d - %d )\n" ), 
				  this, fPrev, fNext, fPara, fStartOffset, fEndOffset );
	
	SCDebugTrace( debugLevel, scString( "        ink extents %s invalid %d layactive %d %d\n" ),
				  buf, fLayBits.fInvalid,
				  fLayBits.fLayRecomposeActive,
				  fPara->Marked( scREBREAK ) );

	SCDebugTrace( debugLevel, scString( "        VJOffset %d cursorY (%d %d) ORG (%d %d)\n"),
							  fVJOffset, fCursorY1, fCursorY2, fOrigin.x, fOrigin.y );
	SCDebugTrace( debugLevel, scString( "        fMaxLead %d ( %d %d ) %s\n" ),
							  fMaxLead, TopLead(), BottomLead(),
							  ( fMaxLead - TopLead() - BottomLead() ) ? "NO" : "YES" ); 	
}

#endif

/* ==================================================================== */

scTextline* scTextline::Allocate( scContUnit*	p,
								  scColumn* 	col,
								  scTextline*	prev)
{
	return SCNEW scTextline( p, col, prev );
}


/* ==================================================================== */

