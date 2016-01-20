/*=================================================================
 
	File:		scparag3.c

	$Header: /Projects/Toolbox/ct/SCPARAG3.CPP 3	 5/30/97 8:45a Wmanis $

	Contains:	reformatting code for content units.

	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.


=================================================================*/


#include "scparagr.h"
#include "sccolumn.h"
#include "scbreak.h"
#include "scstcach.h"
#include "scglobda.h"
#include "scctype.h"
#include "scmem.h"
#include "scspcrec.h"
#include "scstream.h"
#include "sctextli.h"
#include "screfdat.h"
#include "scparagr.h"
#include "sccolumn.h"
#include "sccallbk.h"

#ifdef scUseRubi
#include "scrubi.h"
#endif

/* ==================================================================== */

scPARARefData::scPARARefData() :
	fPrevPara( 0 ),
	fPrevSpec( 0 ),
	fOrigin( 0, 0 ),
	fPrevline( 0 ), 							 
	fTextline( 0 ),   
	fPara( 0 ), 										
	fBreakType( eUndefinedBreak ),	
	fColumnCount( 0 ),
	fLinesBefore( 0 ),
	fLinesAfter( 0 ),
	fLineNumber( 0 ),
	fLinesHyphed( 0 ),
	fCharRecs( 0 ),
	fStartSpecRec( 0 ),
	fCurSpecRec( 0 ),
	fSpecCount( 0 ),
#ifdef scUseRubi
	fAnnotations( 0 ),
#endif
	column_( 0 )
{
}

/* ==================================================================== */

void scLINERefData::xxInit( )
{
	fOrg.Set( 0, 0 );
	
	fCharRecs		= 0;
	fStartCharOffset= 0;
	fCharCount		= 0;
	
	fSpecRec		= 0;
	fStartSpecRunOffset = 0;
	fSpecRunCount	= 0;
				
	// fMaxLeadSpec; DO NOT SET THIS IN HERE
	
#ifdef _RUBI_SUPPORT
	fAnnotations	= 0;
#endif
	
	fInkExtents.Invalidate();
	fLogicalExtents.Invalidate();
	
	fLastLineLen	= 0;
	fMeasure		= 0;
	fComputedLen	= 0;
	fRagSetting 	= eRagCentered;
	fLetterSpace	= 0;
	fStartAngle 	= 0;
	fEndAngle		= 0;
	fColShapeType	= eNoShape;
	fBaselineJump	= 0;
}

/* ==================================================================== */

void scLINERefData::Init( const scFlowDir& fd )
{
	xxInit(); 
	fInitialLead.Init( fd );  
	fEndLead.Init( fd );
	fFlowDir		= fd;
}

/* ==================================================================== */

scLEADRefData::scLEADRefData( const scLEADRefData& ld )
{
	fFlow		= ld.fFlow;
	fAboveLead	= ld.fAboveLead;
	fBelowLead	= ld.fBelowLead;
}

/* ==================================================================== */

void scLEADRefData::Set( MicroPoint lead )
{
	ComputeAboveBelow( lead, fFlow );
}

/* ==================================================================== */

void scLEADRefData::Set( MicroPoint lead, const scFlowDir& fd )
{
	ComputeAboveBelow( lead, fd );
}

/* ==================================================================== */

MicroPoint scLEADRefData::Compute( MicroPoint ptsize, MicroPoint lead, eFntBaseline baseline )
{
	scFlowDir fd;
	switch ( baseline ) {
			// horizontal flow
		case kTopBaseline:
		case kMiddleBaseline:
		case kBottomBaseline:
		case kRomanBaseline:
			fd.SetFlow( eRomanFlow );
			break;
		
			// vertical flow
		case kLeftBaseline:
		case kCenterBaseline:
		case kRightBaseline:
			fd.SetFlow( eVertJapanFlow );
			break;		
	}
	ComputeAboveBelow( lead, fd );
	return GetLead();
}

/* ==================================================================== */

void scLEADRefData::Set( MicroPoint aboveLead, MicroPoint belowLead, const scFlowDir& fd )
{
	fAboveLead	= aboveLead;
	fBelowLead	= belowLead;
	fFlow		= fd;
}

/* ==================================================================== */

void scLEADRefData::ComputeAboveBelow( MicroPoint lead, const scFlowDir& fd )
{
	if ( fd.IsHorizontal() ) {
		static REAL realAbove = (REAL)RLU_BASEfmTop / scBaseRLUsystem;
		static REAL realBelow = (REAL)RLU_BASEfmBottom / scBaseRLUsystem;
		static MicroPoint lastlead;
		static MicroPoint abovelead;
		static MicroPoint belowlead;

		if ( lastlead != lead ) {
			abovelead	= scRoundMP( realAbove * lead );
			belowlead	= scRoundMP( realBelow * lead );
			lastlead	= lead;
		}
		fAboveLead = abovelead;
		fBelowLead = belowlead;
	}
	else {
		fAboveLead	= lead / 2;
		fBelowLead	= lead / 2; 
	}
}

/* ==================================================================== */

scSpecRecord* scPARARefData::GetSpecRecord( long offset )
{
	scSpecRecord* specrec;

	for ( specrec = fStartSpecRec; offset > (specrec+1)->offset(); specrec++ )
		;
	return specrec; 
}

/* ==================================================================== */
	
void scPARARefData::SaveData( /* const scCOLRefData& cData */ )
{
//	fSavedBreakType 	= fBreakType;
//	fSavedLineData		= fLineData;
//	fSavedscCOLRefData	= cData;
//	fSavedCurSpec		= fCurSpec;
}
	
/* ==================================================================== */

void scPARARefData::RestoreData( /* scCOLRefData& cData */ )
{
//	TypeSpec maxLeadSpec	= fLineData.fMaxLeadSpec;
	
//	fBreakType				= fSavedBreakType;
//	fCurSpec				= fSavedCurSpec;
//	fLineData				= fSavedLineData;	
//	cData					= fSavedscCOLRefData;
	
//	fLineData.fMaxLeadSpec	= maxLeadSpec;
	
//	FMSetMetrics( fCurSpec );
}

/* ==================================================================== */

Bool scPARARefData::AdjustLead( void ) const
{
	return fInitialLine.fInitialLead.GetAboveLead() < fComposedLine.fEndLead.GetAboveLead();
}

/* ==================================================================== */
// delete excess lines in the paragraph we are currently formatting

void scCOLRefData::PARADeleteExcessLines( void )
{ 
	scContUnit* para	= fPData.fPara;
	scTextline* txl 	= fPData.fTextline;
	scTextline* nextTxl;
	scColumn*	nextCol;

	if ( txl )
		txl = txl->GetNext();
	else
		txl = para->GetFirstline();

		// delete excess lines in paragraph
	for ( ; txl; txl = nextTxl ) {

		nextTxl = txl->GetNext();

		if ( nextTxl == NULL ) {
				// check next column
			if ( ( nextCol = txl->GetColumn()->GetNext() ) != NULL )
				nextTxl = para->NextColumn( nextCol );
		}

		if ( txl->GetPara() == para )
			txl->Delete( fLineDamage );
		else
			break;
	}

		// delete any excess lines in column if no further paragraphs
	if ( para && !para->GetNext() && txl ) {
		for ( ; txl; txl = nextTxl ) {
			nextTxl = LNNext( txl );
			txl->Delete( fLineDamage );
		}
	}

}

/* ==================================================================== */
/* figure out where to put the first line of a paragraph */

scTextline* scContUnit::LocateFirstLine( scCOLRefData&	cData,
										 TypeSpec		curSpec,
										 scColumn*& 	col,
										 MicroPoint&	baseline,
										 scLEADRefData& lead,
										 PrevParaData&	prevParaData )
{
	scTextline* lastTxl;

	if ( GetPrev() ) {	 /* there are prior paragraphs in the stream */
		lastTxl = GetPrev()->GetLastline();

		if ( lastTxl ) {
			lastTxl->ParaLead( lead, col->GetFlowdir() );

			if ( lastTxl->GetColumn() != cData.GetActive() ) {
				cData.COLFini( false );
				col 			= lastTxl->GetColumn();
				cData.COLInit( col, this );
			}
			baseline		= lastTxl->GetBaseline();
			
			if ( col->GetFlowdir().IsHorizontal() )
				cData.fPrevEnd.Set( col->Width(), lastTxl->GetOrigin().y );
			else
				cData.fPrevEnd.Set( lastTxl->GetOrigin().x, col->Depth() );
			cData.fSavedPrevEnd = cData.fPrevEnd;

				// we have to fool the baseline into thinking that it is
				// in a vertically oriented column
			if ( col->GetFlowdir().IsVertical() )
				baseline = col->Width() - baseline;
		}
		else {
				// overflow stuff, in COLGetStrip
			TypeSpec ts = SpecAtStart( );
			lead.Set( scCachedStyle::GetCachedStyle( ts ).GetComputedLead(), col->GetFlowdir() );
			baseline	= LONG_MIN;
		}
	}
	else {
			// this is the first paragraph in the stream
		TypeSpec ts = SpecAtStart(	);
		lead.Set( scCachedStyle::GetCachedStyle( ts ).GetComputedLead(), col->GetFlowdir() );
		lastTxl 	= NULL;
		baseline	= FIRST_LINE_POSITION;
	}
	return lastTxl;
}


/* ==================================================================== */

Bool scCOLRefData::AllocLine( Bool leadRetry )
{
	if ( leadRetry ) {
		scAssert( fPData.fTextline != 0 );
		return true;
	}

	if ( !fPData.fPrevline ) {
		fPData.fTextline = fCol->GetFirstline();
		if ( fPData.fTextline && fPData.fTextline->GetPara()->GetCount() < fPData.fPara->GetCount() ) {
			fCol->FreeLines( true, fLineDamage );
			fPData.fTextline = NULL;
		}
	}
	else
		fPData.fTextline = fPData.fPrevline->GetNext();

	if ( !fPData.fTextline ) {
		fPData.fTextline = scTextline::Allocate( fPData.fPara,
												 fCol,
												 fPData.fPrevline );
	}
	else if ( fPData.fTextline->GetPara() != fPData.fPara ) {
		fPData.fPrevline = fPData.fTextline;
		fPData.fTextline = LNInsertNew( fPData.fPara, fCol, fPData.fTextline );
	}
	else
		fPData.fTextline->InitForReuse( fPData.fPara );

	return fPData.fTextline != NULL;
}

/* ==================================================================== */	
// Set up line data according to current spec

void scPARARefData::SetLineData( Bool leadRetry )
{
	if ( leadRetry ) {
			// we want to reset the initial leading and the max lead spec
			// but everything else should be the same
			// because we are going to retry setting the line
		fInitialLine.fEndLead.SetBelowLead( fInitialLine.fInitialLead.GetBelowLead() );

		MicroPoint maxAboveLead = MAX( fInitialLine.fEndLead.GetAboveLead(),
									   fComposedLine.fEndLead.GetAboveLead() );
		fInitialLine.fEndLead.SetAboveLead( maxAboveLead );
		fInitialLine.SetMaxLeadSpec( fComposedLine.GetMaxLeadSpec() );
		fComposedLine			= fInitialLine; 
	}	
	
	scAssert( fInitialLine.fCharRecs != 0 );

		// we have accepted the composed line at this point
		// so we can copy it over
	fInitialLine						= fComposedLine;
										  
	fInitialLine.fStartCharOffset		= fComposedLine.GetEndCharOffset();  
	fInitialLine.fCharCount 			= 0;
//	fCharCount
	
		// get to the spec at the beginning of the line  
	fInitialLine.fSpecRec = GetSpecRecord( fInitialLine.fStartCharOffset );
	TypeSpec ts = fInitialLine.fSpecRec->spec();
	fInitialLine.SetInitialSpec( ts );
	scCachedStyle::GetCachedStyle( ts );

	if ( !leadRetry )
		fInitialLine.SetMaxLeadSpec( ts );
		
	fInitialLine.fStartSpecRunOffset	= 0;
	fInitialLine.fSpecRunCount			= 0;

#ifdef scUseRubi
	fInitialLine.fAnnotations			= 0;
#endif
	
	fInitialLine.fInkExtents			=  scCachedStyle::GetCurrentCache().GetInkExtents();
	fInitialLine.fInkExtents.Translate( 0, scCachedStyle::GetCurrentCache().GetBaseline() );	
	fInitialLine.fLogicalExtents		=  scCachedStyle::GetCurrentCache().GetLogicalExtents();

// fLastLineLen
// fMeasure
// fComputedLen
// fRagSetting


		// set primary lead - we are setting here for COLGetStrip
	fInitialLine.fInitialLead			= fComposedLine.fEndLead;
// fEndLead
// fLetterSpace
	
// fColShapeType
	
	fInitialLine.fBaselineJump			=  scCachedStyle::GetCurrentCache().GetBaseline();

// fFlowDir 	
	
		// now the the initial values are set copy them over to the line
		// that we will compose, if the composition fails for some
		// reason or other we will have our initial values and can
		// reset things
	fComposedLine						=  fInitialLine;
}

/* ==================================================================== */
 
void scPARARefData::PARAInit( scContUnit*p,
							  const scFlowDir& fd )
{
	scAssert( fPara == 0 );
	
	fPara			= p;
	fCharRecs		= (CharRecordP)fPara->GetCharArray().Lock( );

	fStartSpecRec	= fPara->GetSpecRun().ptr( );

	TypeSpec ts = fStartSpecRec->spec();
	fPara->InitParaSpec( ts );

	fComposedLine.Init( fd );

		// !!!!!NOTE: this should only be set here	or in the line breaker
	fComposedLine.SetMaxLeadSpec( fStartSpecRec->spec() );	  
															

	scCachedStyle::GetCachedStyle( ts );

	fBreakLang		= scCachedStyle::GetCurrentCache().GetBreakLang();
}

/* ==================================================================== */

void scPARARefData::PARAFini(  )
{	
	fPara->GetCharArray().Unlock( );
	
	fPara			= 0;
	fCharRecs		= 0;
	fStartSpecRec	= 0;
	
}

/* ==================================================================== */
// Break and set the current line of the paragraph, 
//			RETURNS 	false if no need to retry the linebreaker
//						true if we need to retry due to a leading change
//

Bool scPARARefData::ComposeLine( DCState& dcState )
{	
	scSpecRecord*	curSpecRec = fCurSpecRec;

		// set primary lead - for ???
	fComposedLine.fInitialLead.SetAboveLead( 0 );
	fComposedLine.fInitialLead.SetBelowLead( 0 );

	scAssert( fComposedLine.fCharRecs != 0 );		
	scAssert( fComposedLine.GetMaxLeadSpec() != 0 );

#ifdef scJIS4051
	if ( fBreakLang == eCompRoman ) {
#endif
		fBreakType = BRKRomanLineBreak( fComposedLine.fCharRecs,
										  fComposedLine.fStartCharOffset,
										  fComposedLine.fCharCount,
										  fComposedLine,
										  fLineNumber,
										  fLinesHyphed,
										  &fCurSpecRec,
										  fComposedLine.fInkExtents,
										  fComposedLine.fLetterSpace ); 			
		scAssert( fComposedLine.fInkExtents.Valid() );
#ifdef scJIS4051
	}
	else if ( fBreakLang == eCompJapanese ) {
		fBreakType = BRKJapanLineBreak( fComposedLine.fCharRecs,
										  fComposedLine.fStartCharOffset,
										  fComposedLine.fCharCount,
										  fComposedLine,
#ifdef scUseRubi
										  fAnnotations,
#endif
										  fLineNumber,
										  fLinesHyphed,
										  &fCurSpecRec,
										  fComposedLine.fInkExtents,
										  fComposedLine.fLetterSpace,
										  dcState );
		scAssert( fComposedLine.fInkExtents.Valid() );			
	}
#endif

	if ( AdjustLead() ) {
		fBreakType = eUndefinedBreak;
		fCurSpecRec = curSpecRec;
		if ( fInitialLine.fBaseline == FIRST_LINE_POSITION ) {
			TypeSpec ts = fInitialLine.GetInitialSpec();
			MicroPoint firstline = CSfirstLinePosition( GetColumn()->GetAPPName(), ts );
			fInitialLine.fBaseline = fComposedLine.fBaseline - firstline;
		}
		return true;
	}
		
	fTextline->Set( fLineNumber, fBreakType, fComposedLine );
	
	if ( !fPara->GetFirstline() ) {
		scAssert( fTextline->GetEndOffset() <= fPara->GetContentSize() );
		scAssert( fTextline->GetStartOffset() == 0 );
		fPara->SetFirstline( fTextline );
	}

	scLEADRefData	aboveLeadData( fComposedLine.fInitialLead.GetFlow() );
	scLEADRefData	belowLeadData( fComposedLine.fInitialLead.GetFlow() );

	if ( fComposedLine.GetMaxLeadSpec() != fCurSpecRec->spec() ) {
		TypeSpec ts = fComposedLine.GetMaxLeadSpec();
		scCachedStyle::GetCachedStyle( ts );
		belowLeadData.ComputeAboveBelow( scCachedStyle::GetCurrentCache().GetComputedLead(), scCachedStyle::GetCurrentCache().GetFlowdir() );
		TypeSpec ts1 = fCurSpecRec->spec();
		scCachedStyle::GetCachedStyle( ts1 );
		aboveLeadData.ComputeAboveBelow( scCachedStyle::GetCurrentCache().GetComputedLead(), scCachedStyle::GetCurrentCache().GetFlowdir() );	
	}
	else {
		TypeSpec ts = fCurSpecRec->spec(); 
		scCachedStyle::GetCachedStyle( ts );
		belowLeadData.ComputeAboveBelow( scCachedStyle::GetCurrentCache().GetComputedLead(), scCachedStyle::GetCurrentCache().GetFlowdir() );

		int endoffset = fTextline->GetEndOffset() + 1;
		if ( endoffset >= (fCurSpecRec+1)->offset() ) {
			fCurSpecRec++;
			ts = fCurSpecRec->spec();
			scCachedStyle::GetCachedStyle( ts );			
		}
		aboveLeadData.ComputeAboveBelow( scCachedStyle::GetCurrentCache().GetComputedLead(), scCachedStyle::GetCurrentCache().GetFlowdir() );
	}

	fComposedLine.fEndLead.SetBelowLead( belowLeadData.GetBelowLead() );				 
	fComposedLine.fEndLead.SetAboveLead( aboveLeadData.GetAboveLead() );
	fComposedLine.fEndLead.SetExternalSpace( 0 );

	
	fLineNumber++;
	fLinesBefore++;
	if ( fColumnCount )
		fLinesAfter++;
	fPrevline = fTextline;

	return false;
}

/* ==================================================================== */
/* This column is full. If there is a next one, get it ready for		*/
/* reformatting and return true; else, return false;					*/

Bool scCOLRefData::FindNextCol( DCState& dcState )
{
	Bool colRefStat = false;
	
	if ( fCol->GetNext() ) {
		COLFini( true );
		fCol		= fCol->GetNext();
		colRefStat	= COLInit( fCol, fPData.fPara );
		
			// if the character count is zero we are transitioning 
			// columns before we set any text so reset the dcState column
		if ( fPData.fComposedLine.fCharCount == 0 ) 
			dcState.SetColumn( fCol  );
	}
	
	if ( colRefStat == true ) {
		fPData.fPrevline					= NULL;
		fPData.fTextline					= NULL;
		fPData.fInitialLine.fLastLineLen	= LONG_MIN;
		fPData.fBreakType					= eCharBreak;
		fPData.fColumnCount++;
		fPData.fLinesAfter					= 0;
		return true;
	}
	
	fPData.fTextline = fPData.fPrevline;
	return false;
}

/* ==================================================================== */
/* Set up initial paragraph reformat data.								*/

void scCOLRefData::PARAInit( scContUnit*	p,
							 int			breakControl,
							 int			keepWNextControl,
							 PrevParaData&	prevParaData )
{
	fPData.PARAInit( p, fCol->GetFlowdir() );
	
	fPData.fPara->SetFirstline( 0 );

	fPData.fCurSpecRec			= fPData.fStartSpecRec;

	fPData.fTextline			= NULL;
	fPData.fBreakType			= eParaBreak;
	fPData.fLineNumber			= 0;
	fPData.fLinesHyphed 		= 0;

	
	fPData.fInitialLine.Init( fCol->GetFlowdir() );
	fPData.fInitialLine.SetCharacters( fPData.fCharRecs );	
	
	fPData.fComposedLine.Init( fCol->GetFlowdir() );	
	fPData.fComposedLine.SetCharacters( fPData.fCharRecs ); 	
	
	scCachedStyle::GetParaStyle().GetParaBreak( fPData.fBreakParams );
	
	if ( fPData.fBreakParams.KeepWithNext() )
		 fPData.fPara->Mark( scKEEPNEXT );
	else
		 fPData.fPara->Unmark( scKEEPNEXT );

	if ( keepWNextControl ) {
		fPData.fPrevline			= NULL;
		fPData.fLinesBefore 		= 0;
		fPData.fLinesAfter			= 0;
		fPData.fColumnCount 		= 0;
	}
	else if ( breakControl ) {
		fPData.fPrevline				= NULL;
	}
	else {
		fPData.fPrevline = fPData.fPara->LocateFirstLine( *this,
														  fPData.fCurSpecRec->spec(),
														  fCol, 
														  fPData.fComposedLine.fBaseline, 
														  fPData.fComposedLine.fEndLead,
														  prevParaData );

		fPData.fLinesBefore 	= 0;
		fPData.fLinesAfter		= 0;
		fPData.fColumnCount 	= 0;
	}
	
		// this is where in a layout world I would make the distinction between
		// a logical unit and a paragraph
	fPData.fComposedLine.fLastLineLen	= LONG_MIN;
}

/* ==================================================================== */
/* Check for orphan or no break condition violation. If found, delete	*/
/* excess lines of last column and return true so para reformat can 	*/
/* try again. If pData.lastTxlH is NULL, there are no lines in the		*/
/* column, and we did not fail due to an orphan condition -- then,		*/
/* delete all lines in column.											*/

Bool scCOLRefData::ResetOrphan( Bool testGetStrip )
{
	scContUnit*p = fPData.GetPara();
	scAssert( p != 0 );
		
	if ( fPData.fBreakParams.NoBreak() || fPData.fLinesBefore < fPData.fBreakParams.LinesBefore() ) {
		
		if ( p->GetPrev() && fCol->GetNext() ) {

			fPData.fPrevline	= p->GetPrev()->GetLastline();
			fCol->DeleteExcessLines( p, fPData.fPrevline, testGetStrip, *this );
			fPData.PARAFini( );
			COLFini( true );

			fCol = fCol->GetNext();
			return COLInit( fCol, p );
		}
	}

	return false;
}

/* ==================================================================== */
/* Check here for widows. If the columns had the same measure we could	*/
/* just grab sufficient lines, but since columns may be of different	*/
/* measure, we will just grab one line at a time and check the fit. 	*/
/* Each time we iterate, we will grab another line. This is slower but	*/
/* more acurate. An optimization may be to check the measures and, if	*/
/* they are close, grab more than one line on an iteration. 			*/

/* If a widow is found, reset parameters and return true so 			*/
/* para reformat can try again. 										*/

Bool scContUnit::ResetWidow( scCOLRefData&	cData,
							 Bool			testGetStrip )
{
	if ( cData.fPData.fColumnCount && cData.fPData.fLinesAfter < cData.fPData.fBreakParams.LinesAfter() ) {
			/* each time we try to remove just one line */
			/* back up to previous column */
		cData.COLFini( false );
		cData.fCol	= cData.fCol->GetPrev();
		cData.COLInit( cData.fCol, this );

			/* remove one excess line */
		cData.fPData.fPrevline = cData.fCol->GetLastline();
		if ( cData.fPData.fPrevline ) {
			cData.fPData.fPrevline = LNPrev( cData.fPData.fPrevline );
			cData.fCol->DeleteExcessLines( this, cData.fPData.fPrevline, testGetStrip, cData );

				/* reset state */
			if ( cData.fPData.fPrevline ) {
				cData.fPData.fLinesBefore					= cData.fPData.fPrevline->GetLinecount();
				cData.fPData.fLineNumber					= (short)(cData.fPData.fLinesBefore + 1);
				cData.fPData.fInitialLine.fStartCharOffset	= cData.fPData.fPrevline->GetEndOffset();
				
				cData.fPData.fCurSpecRec				= cData.fPData.GetSpecRecord( cData.fPData.fPrevline->GetEndOffset() );
				TypeSpec ts = cData.fPData.fCurSpecRec->spec();
				scCachedStyle::GetCachedStyle( ts );
				cData.fPData.fBreakType 			= eColumnBreak;

				return true;
			}
		}
	}

	return false;
}

/* ==================================================================== */
// perform the line breaking on the paragraph

eRefEvent scContUnit::Reformat( scCOLRefData&	cData,
								PrevParaData&	prevParaData,
								int 			keepWNextControl
#if SCDEBUG > 1
								, int&			reformatEvent 
#endif
								)
{
	int 		breakControl = 0;
	eRefEvent	refEvent;
	do {
		
		refEvent = Reformat2( cData,
							  prevParaData,
							  keepWNextControl,
							  breakControl
#if SCDEBUG > 1
							  , reformatEvent
#endif
							   );
		
	} while ( refEvent == eRebreak );
	return refEvent;
}

/* ==================================================================== */

eRefEvent scContUnit::Reformat2( scCOLRefData&	cData,
								 PrevParaData&	prevParaData,
								 int			keepWNextControl,
								 int&			breakControl			
#if SCDEBUG > 1
								, int&			reformatEvent 
#endif
								)
{

	DCState dcState;
	Bool	tryAgain;
	Bool	leadRetry		= false;	// a retry based upon a leading increase on the line
	Bool	overFlow		= false;
	Bool	testGetStrip	= true;


	SCDebugTrace( 1, scString( "scContUnit::Reformat IN 0x%08x %d\n" ), this, GetCount() );

	cData.PARAInit( this, breakControl, keepWNextControl, prevParaData );
 
	dcState.SetColumn( cData.GetActive()  );
	scFlowDir fd( cData.fCol->GetFlowdir() );
	
	if ( cData.fPData.fPrevline ) {
		if ( fd.IsHorizontal() )
			cData.fSavedPrevEnd.x = LONG_MAX;
		else {
			cData.fSavedPrevEnd.y = LONG_MAX;
		}
	}

	for ( tryAgain = true; tryAgain; ) {
		for ( overFlow = false; cData.fPData.fBreakType != eEndStreamBreak || overFlow; ) {
			
			if ( !overFlow )
				cData.fPData.SetLineData( leadRetry );		// set up initial line conditions
			
			if ( !overFlow && cData.AllocGeometry() ) { 	// allocatate the geometry from
															// the parent column, if we overflow
															// the column, we go to the next column

				try {
					cData.AllocLine( leadRetry );			// allocate the memory, reuse scheme is used
				} 
				catch( ... ) {
					cData.fPData.PARAFini( );
					throw;
				} 

				cData.fPData.SetColumn( cData.fCol );
				leadRetry = cData.fPData.ComposeLine( dcState );  // compose the line
				
				if ( !leadRetry ) {
					// we are accepting the line as is 
					MicroPoint x, y;
					if ( fd.IsHorizontal() ) {
						x = cData.fPData.fComposedLine.fOrg.x + cData.fPData.fComposedLine.fMeasure;
						y = cData.fPData.fComposedLine.fOrg.y;
					}
					else {
						x = cData.fPData.fComposedLine.fOrg.x;
						y = cData.fPData.fComposedLine.fOrg.y + cData.fPData.fComposedLine.fMeasure;
					}
					cData.fSavedPrevEnd.Set( x, y );
				}
				else
					; // we are going to reposition the line and rebreak it
			}
			else {
					// No more room in column, let's try the next column
				overFlow	= false;

					// if we try and relead at the bottom of a container and we 
					// overflow we need to set "leadRetry" to false since at this
					// point it is not a retry any more but a whole new container
				leadRetry	= false;

				if ( !cData.fPData.fPrevline )
					testGetStrip = false;

				if ( !breakControl && cData.ResetOrphan( testGetStrip ) ) {
					breakControl++;
					return eRebreak;
				}
				
				cData.fCol->DeleteExcessLines( this, cData.fPData.fPrevline, testGetStrip, cData );
				testGetStrip = true;

					// find the next column 
				if ( cData.FindNextCol( dcState ) ) {
						// the finding of the next column may reset the para spec
					scCachedStyle::SetParaStyle( this, defspec_ ); 
					continue;
				}
				else {
					overFlow = true;
					break;
				}
			}


		}

		tryAgain = ResetWidow( cData, testGetStrip );

	}

	if ( GetFirstline() )
		cData.PARADeleteExcessLines( );

#if SCDEBUG > 1
	SetReformatEvent( reformatEvent++ );
#endif

	Unmark( scREBREAK );

	if ( !overFlow )
		scAssert( GetLastline() != 0 );

	cData.fPData.PARAFini( );

	prevParaData.lastLineH	= cData.fPData.fTextline;
	prevParaData.lastSpec	= cData.fPData.fCurSpecRec->spec();

	SCDebugTrace( 1, scString( "scContUnit::Reformat OUT 0x%08x %d\n" ), this, GetCount() );

	return !overFlow ? eNormalReformat : eOverflowGeometry;
}

/* ==================================================================== */
