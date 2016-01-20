/****************************************************************************

	File:	   SCCOLUM3.C


	$Header: /Projects/Toolbox/ct/Sccolum3.cpp 3	 5/30/97 8:45a Wmanis $

	Contains:	Contains the code to allocate lines for containers and
				other miscellaneous code.

	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

***************************************************************************/

#include "scpubobj.h"
#include "sccolumn.h"
#include "scstcach.h"
#include "scglobda.h"
#include "sccallbk.h"
#include "scmem.h"
#include "scparagr.h"
#include "scregion.h"
#include "sctextli.h"
#include "screfdat.h"


/* ==================================================================== */
// translate the lines

void scColumn::TranslateLines( const scMuPoint& trans )
{
	scTextline* txl;

	for ( txl = fFirstline; txl; txl = txl->GetNext() ) {
//		scAssert( txl->fOrigin.x + trans.x >= 0 );
		txl->Translate( trans );
	}
}


/* ==================================================================== */
/* reposition or realign the lines in this column which is probably a
 * flex column
 */

void scColumn::RepositionLines( )
{
	scTextline	*txl;
	MicroPoint	measure;
		
	if ( GetFlowdir().IsHorizontal() )
		measure = Width( );
	else	
		measure = Depth();

	for ( txl = fFirstline; txl; txl = txl->GetNext() )
		txl->Reposition( measure );
}

/* ==================================================================== */
// set the column as the active container in the reformatting cache

void scCOLRefData::SetActive( scColumn* col )
{
	fCol = col;
	if ( col )
		fPData.SetFlowDir( col->GetFlowdir() );
	else
		fPData.SetFlowDir( scFlowDir( ) );		
}

/* ==================================================================== */
// free lines marks as invalid and collect their damaged area

void scCOLRefData::FreeInvalidLines( void )
{
	scTextline* txl;
	scTextline* nextTxl;

	for ( txl = fCol->GetFirstline(); txl; txl = nextTxl ) {
		nextTxl = txl->GetNext();
		if ( txl->Marked( scINVALID ) )
			txl->Delete( fLineDamage );
		else if ( txl->Marked( scREPAINT ) ) {
			scXRect damage;
			txl->QueryExtents( damage, 1 );
			fLineDamage.Union( damage );
			txl->Unmark( scREPAINT );
		}
	}
}

/* ==================================================================== */
// save the linelist for damage determination in formatting

void scCOLRefData::SaveLineList( )
{
	scTextline* txl;

	for ( txl = fCol->GetFirstline(); txl; txl = txl->GetNext() ) {
		if ( txl->Marked( scREPAINT ) ) {
			scXRect xrect;
			txl->QueryExtents( xrect, 1 );
			fLineDamage.Union( xrect );
			txl->Marked( scREPAINT );
		}	
	}
		
	fSavedLineState.SaveLineList( fCol );
}

/* ==================================================================== */
// initialize the the data structures to perform linebreaking in a column,
// this is primarily used for irregular run-arounds

Bool scCOLRefData::COLInit( scColumn* col, scContUnit* p )
{
	fCol = col;
	fLineDamage.Invalidate();

	FreeInvalidLines( );

	SCDebugTrace( 2, scString( "\tCOLStartReformat: col 0x%08x %d\n" ), fCol, fCol->GetCount( ) );
	scVertex*		startV;
	scContUnit* 	prevPara;
	PrevParaData	prevParaData;


	prevParaData.lastLineH	= NULL;
	prevParaData.lastSpec.clear();

//	SCDebugTrace( 2, scString( "COLStartReformat %d" ), col->fColumnCount );

	if ( !fCol->DamOpen() )
		return false;

#if SCDEBUG > 1
	if ( fCol->GetPrev() )
		scAssert( !fCol->GetPrev()->Marked( scLAYACTIVE ) );
	scAssert( fCol && ! fCol->Marked( scLAYACTIVE ) );
#endif

	fCol->Mark( scLAYACTIVE );
	
	ggcS.theActiveColH	= fCol;
	
	SetActive( fCol );

	scCachedStyle::SetFlowdir( fCol->GetFlowdir() );

		// set these to defaults
	fSavedPrevEnd.Set( LONG_MIN, FIRST_LINE_POSITION );

		// now check to see if we are starting reformatting in the 
		// middle of the column, if we are we should set the prevbaseline up
	prevPara = p->GetPrev();
	if ( prevPara ) {
		scTextline* txl = prevPara->GetLastline( );
		if ( txl && txl->GetColumn() == fCol ) {
			scColumn*			tCol = fCol;
			scLEADRefData	lead;
			MicroPoint		baseline = fSavedPrevEnd.y;
			p->LocateFirstLine( *this, p->SpecAtStart(), tCol, baseline, lead, prevParaData );
			scAssert( tCol == fCol );
		}
	}

		/* If this fails, no problem. COLLineListChanges */
		/* will simply repaint ALL lines.				 */
	SaveLineList( );

	SetRegion( 0 );
	
	switch ( fCol->GetShapeType() ) {
		default:
			break;
		case eFlexShape:
		case eHorzFlex:
			if ( fCol->GetFlowdir().IsVertical() ) {
				scTextline* txl = fCol->GetFirstline();
				if ( txl ) {
					MicroPoint	position		= txl->GetOrigin().x;
					TypeSpec	ts				= txl->SpecAtStart( );
					MicroPoint	firstlinepos	= CSfirstLinePosition( fCol->GetAPPName(), ts );
						
					scMuPoint trans( mpInfinity - position - firstlinepos, 0 );
					fCol->TranslateLines( trans );
					fCol->SetWidth( mpInfinity );
				}
			}
			break;

		case eVertShape:
			scCachedStyle::GetCurrentCache().SetRunAroundBorder( CSrunaroundBorder( fCol->GetAPPName(), scCachedStyle::GetCurrentCache().GetSpec() ) ); 		
			startV			= (scVertex *)MEMLockHnd( fCol->GetVertList() );
			
			try {
				fRgnH = NewHRgn( scSliverSize() );
				PolyHRgn( fRgnH, startV );
			} 
			catch ( ... ) {
				DisposeHRgn( fRgnH ), fRgnH = 0;
				MEMUnlockHnd( fCol->GetVertList() );
				throw;
			} 
			
			MEMUnlockHnd( fCol->GetVertList() );

			InsetHRgn( fRgnH, scCachedStyle::GetCurrentCache().GetRunAroundBorder(), scCachedStyle::GetCurrentCache().GetRunAroundBorder(), true );
							
			fRgn = (HRgn*)MEMLockHnd( fRgnH );

			break;

		case eRgnShape:
			scCachedStyle::GetCurrentCache().SetRunAroundBorder( CSrunaroundBorder( fCol->GetAPPName(), scCachedStyle::GetCurrentCache().GetSpec() ) );

			try {
				fRgnH = NewHRgn( RGNSliverSize( fCol->GetRgn() ) );

				CopyHRgn( fRgnH, fCol->GetRgn() );
				InsetHRgn( fRgnH, scCachedStyle::GetCurrentCache().GetRunAroundBorder(), scCachedStyle::GetCurrentCache().GetRunAroundBorder(), true );
			} 
			catch ( ... ) {
				DisposeHRgn( fRgnH ), fRgnH = 0;
				throw;
			} 

			fRgn = (HRgn *)MEMLockHnd( fRgnH );
			break;
	}
	
	
	fPData.fInitialLine.fBaseline	= FIRST_LINE_POSITION;
	fPData.fComposedLine.fBaseline	= FIRST_LINE_POSITION;

	return true;
}

/* ==================================================================== */
// close out the data structures when finished line breaking in a column
		
void scCOLRefData::COLFini( Bool finished )
{	
	if ( fCol ) {
		scXRect extents;

		scAssert( fCol->Marked( scLAYACTIVE ) && fCol == GetActive() );
		fCol->Unmark( scLAYACTIVE );

			//	SCDebugTrace( 2, scString( "COLEndReformat %d" ), col->fColumnCount );

		ggcS.theActiveColH	= NULL;

		switch ( fCol->GetShapeType() ) {
			default:
				fCol->VertJustify( );
				break;
				
			case eVertShape:
			case eRgnShape:
				fCol->VertJustify( );
				if ( fRgnH ) {
					MEMUnlockHnd( fRgnH );
					DisposeHRgn( fRgnH ), fRgnH = NULL;
				}
				break;
				
			case eHorzFlex: 		
			case eFlexShape:
			case eVertFlex:
				fCol->QueryMargins( extents );	
				if ( fCol->GetFlowdir().IsHorizontal() ) {
					if ( fCol->GetShapeType() & eVertFlex )
						fCol->SetDepth( extents.y2 );
					if ( fCol->GetShapeType() & eHorzFlex ) {
						fCol->SetWidth( extents.x2 );
						fCol->RepositionLines();
					}
				}
				else {		
					if ( fCol->GetShapeType() & eHorzFlex ) {
						scMuPoint trans( extents.Width() - fCol->Width(), 0 );
						fCol->TranslateLines( trans );
						
						fCol->SetWidth( extents.Width() );	
					}
					if ( fCol->GetShapeType() & eVertFlex ) {
						fCol->SetDepth( extents.y2 );
						fCol->RepositionLines();
					}
				}
				fCol->VertJustify();
				break;
		}

		fSavedLineState.LineListChanges( fCol, fLineDamage, fRedispList );
	
		if ( finished )
			fCol->Unmark( scINVALID | scREALIGN );

		SCDebugTrace( 2, scString( "\tCOLEndReformat: col 0x%08x %d\n" ), fCol, fCol->GetCount( ) );
	}
}

/* ==================================================================== */
// add all lines that need to be repainted to the repaint rect

scXRect& scColumn::RepaintExtent( scXRect& repaintExtents )
{
	scXRect 	lineExtents;
	scTextline* txl;

	repaintExtents.Invalidate();
	for ( txl = GetFirstline(); txl; txl = txl->GetNext() ) {
		if ( txl->Marked( scREPAINT ) ) {
			txl->QueryExtents( lineExtents, 1 );
			if ( lineExtents.Width() == 0 )
				lineExtents.x2	= lineExtents.x1 + 1;
			repaintExtents.Union( lineExtents );
			txl->Marked( scREPAINT );
		}
	}
	Marked( scREPAINT );

	return repaintExtents;
}

/* ==================================================================== */
// set the column's vertical justification attribute

void scColumn::SetVJ( eVertJust attr )
{
	if ( GetVertJust() != attr )
		Mark( scREALIGN );
	SetVertJust( attr );
}

/* ==================================================================== */
// this does the actual space allocation within the column

Bool scColumn::GetStrip2( scLINERefData&	lineData,
						  int				breakType,
						  scCOLRefData& 	colRefData )
{
	scXRect 		tryRect;
	MicroPoint		tryX,
					tryY,
					colWidth	= Width( ),
					colDepth	= Depth(),
					firstLinePosition;
	Bool			firstLine	= false;
	int 			shapeType;

		// the specs have been properly inited so the block 
		// indent values should be correct
	scAssert( this ==  colRefData.GetActive() );

		// we are in an overflow condidtion
	if ( lineData.fOrg.y == LONG_MIN )
		return false;
	if ( breakType == eColumnBreak )
		return false;

	lineData.fColShapeType	= GetShapeType();
	shapeType				= GetShapeType();

		// We ran into a memory error in COLStartReformat; just use rectangle shape.
	if (fRgnH && colRefData.fRgnH == NULL ) 
		lineData.fColShapeType = eNoShape;

	if ( lineData.IsVertical() ) {
		colWidth	= Depth();
		colDepth	= Width();
		switch ( GetShapeType() ) {
			case eVertFlex:
				shapeType = eHorzFlex;
				break;
			case eHorzFlex:
				shapeType = eVertFlex;
				break;
		}
	}
	
	colRefData.fPrevEnd 	= colRefData.fSavedPrevEnd;

	switch ( shapeType ) {
		default:
		case eNoShape:
			lineData.fOrg.x 	=  scCachedStyle::GetParaStyle().GetLeftBlockIndent();
			lineData.fMeasure	= colWidth - scCachedStyle::GetParaStyle().GetLeftBlockIndent();
			if ( lineData.fOrg.y == FIRST_LINE_POSITION )
				lineData.fOrg.y = CSfirstLinePosition( GetAPPName(), scCachedStyle::GetCurrentCache().GetSpec() );
			else
				lineData.fOrg.y += lineData.fInitialLead.GetLead();
				
			return lineData.fOrg.y <= colDepth - CSlastLinePosition( GetAPPName(), scCachedStyle::GetCurrentCache().GetSpec() );

		case eVertShape:
		case eRgnShape:
			tryRect.Set( 0, 0, MAX( scCachedStyle::GetParaStyle().GetMinMeasure(), colRefData.fRgn->fVertInterval ), lineData.fLogicalExtents.Depth() );

			if ( lineData.fOrg.y == FIRST_LINE_POSITION ) {
				firstLine			= true;
				firstLinePosition	= CSfirstLinePosition( GetAPPName(), scCachedStyle::GetCurrentCache().GetSpec()  );
				lineData.fOrg.y 	= colRefData.fRgn->FirstLinePos( firstLinePosition, lineData.fInitialLead.GetLead() );
				tryX				= colRefData.fPrevEnd.x;
				tryY				= lineData.fOrg.y - firstLinePosition;
				tryRect.y2			= firstLinePosition + CSlastLinePosition( GetAPPName(), scCachedStyle::GetCurrentCache().GetSpec() );
				colRefData.SetFirstlinePos( lineData.fOrg.y );
				colRefData.SetFirstSpec( scCachedStyle::GetCurrentCache().GetSpec() );
			}
			else {
				if ( lineData.fOrg.y == colRefData.GetFirstlinePos() ) {
					firstLine			= true;
					firstLinePosition	= CSfirstLinePosition( GetAPPName(), colRefData.GetFirstSpec() );	
					lineData.fOrg.y 	= colRefData.fRgn->FirstLinePos( firstLinePosition, lineData.fInitialLead.GetLead() );
					tryX				= colRefData.fPrevEnd.x;
					tryY				= lineData.fOrg.y - firstLinePosition;
					tryRect.y2			= firstLinePosition + CSlastLinePosition( GetAPPName(), scCachedStyle::GetCurrentCache().GetSpec() );
				}
				else
					tryY = lineData.fOrg.y + lineData.fLogicalExtents.y1;
				
				if ( lineData.IsHorizontal() ) {
					if ( colRefData.fPrevEnd.y == lineData.fOrg.y )
						tryX = colRefData.fPrevEnd.x;
					else
						tryX = LONG_MIN;
				}
				else {
					if ( ( colDepth - colRefData.fPrevEnd.x ) == lineData.fOrg.y )
						tryX = colRefData.fPrevEnd.y;
					else
						tryX = LONG_MIN;
				}
			}

			colRefData.fRgn->SectRect( tryRect, tryY, tryX, lineData.fInitialLead.GetLead() );
			
			if ( lineData.fOrg.y == FIRST_LINE_POSITION || lineData.fOrg.y == colRefData.GetFirstlinePos() ) {
					// this is here to fix the smi bug 1538 - given that we are using approximations 
					// alot in regions this may be an insufficient fix for other issues that smi 
					// may raise, but since we are using approximations i have no way of reliably 
					// pridicting these issues
				scXRect rgnXRect( colRefData.fRgn->fOrigBounds );
				scXRect tryXRect( tryRect );
			
				if ( !rgnXRect.Contains( tryXRect ) )
					tryRect.x2 = tryRect.x1;
			}

			if ( tryRect.Width() == 0 )
				return false;
			else {
				if ( firstLine == true )
					lineData.fOrg.y = tryRect.y1 + firstLinePosition;
				else
					lineData.fOrg.y = tryRect.y1 - lineData.fLogicalExtents.y1;

				if ( lineData.fOrg.y > RGNMaxDepth( colRefData.fRgnH ) )
					return false;

#if defined(LEFTBLOCKINDENT)
				if ( tryRect.x < 0 )
					lineData.fOrg.x = tryRect.x +  gfmS.GetLeftBlockIndent();
				else
					lineData.fOrg.x = MAX( tryRect.x,  gfmS.GetLeftBlockIndent() );
#else
				lineData.fOrg.x 		= tryRect.x1 +	scCachedStyle::GetParaStyle().GetLeftBlockIndent();
#endif
					
				if ( lineData.fOrg.x != tryRect.x1 )
					lineData.fMeasure = tryRect.Width() + ( tryRect.x1 - lineData.fOrg.x );
				else
					lineData.fMeasure = tryRect.Width();

				return true;
			}
			break;	  /*NOTREACHED*/

		case eVertFlex:
			lineData.fOrg.x 		=  scCachedStyle::GetParaStyle().GetLeftBlockIndent();
			if ( lineData.fOrg.y == FIRST_LINE_POSITION )
				lineData.fOrg.y = CSfirstLinePosition( GetAPPName(), scCachedStyle::GetCurrentCache().GetSpec() );
			else
				lineData.fOrg.y += lineData.fInitialLead.GetLead();
				
			lineData.fMeasure = colWidth - scCachedStyle::GetParaStyle().GetLeftBlockIndent();
			return lineData.fMeasure > 0;

		case eHorzFlex:
			lineData.fOrg.x 		=  scCachedStyle::GetParaStyle().GetLeftBlockIndent();
			if ( lineData.fOrg.y == FIRST_LINE_POSITION )
				lineData.fOrg.y = CSfirstLinePosition( GetAPPName(), scCachedStyle::GetCurrentCache().GetSpec() );
			else
				lineData.fOrg.y += lineData.fInitialLead.GetLead();
			lineData.fMeasure		= HorzFlexMeasure;

			return lineData.fOrg.y <= colDepth - CSlastLinePosition( GetAPPName(), scCachedStyle::GetCurrentCache().GetSpec() );

		case eFlexShape:
			lineData.fOrg.x 		=  scCachedStyle::GetParaStyle().GetLeftBlockIndent();
			if ( lineData.fOrg.y == FIRST_LINE_POSITION )
				lineData.fOrg.y = CSfirstLinePosition( GetAPPName(), scCachedStyle::GetCurrentCache().GetSpec() );
			else
				lineData.fOrg.y += lineData.fInitialLead.GetLead();
			lineData.fMeasure		= HorzFlexMeasure;

			return true;
	}
	/*NOTREACHED*/
	return false;
}

/* ==================================================================== */
// allocate geometry using args

Bool scColumn::GetStrip( scLINERefData& lineData,
						 int			breakType,
						 scCOLRefData&	colRefData )
{
	Bool		doit;

		// since the get strip logic uses regions and can only really
		// deal in one dimension we need to convert the coordinate
		// system of the used variables as we go in and out of the
		// get strip code - refer to the discussion of coordinate
		// systems in the Toolbox Concept doc
	if ( lineData.IsVertical() )
		lineData.fLogicalExtents.FourthToThird( 0 );

	lineData.fOrg.y 	= lineData.fBaseline;

	doit = GetStrip2( lineData, breakType, colRefData );

	lineData.fBaseline	= lineData.fOrg.y;

	if ( lineData.IsVertical() ) {
		lineData.fOrg.ThirdToFourth( Width() );
		lineData.fLogicalExtents.ThirdToFourth( 0 );
	}

	return doit;
}


/* ==================================================================== */
// allocate geeomtry using cached values

Bool scCOLRefData::AllocGeometry( void )
{
	Bool		doit;

		// since the get strip logic uses regions and can only really
		// deal in one dimension we need to convert the coordinate
		// system of the used variables as we go in and out of the
		// get strip code - refer to the discussion of coordinate
		// systems in the Toolbox Concept doc
	if ( fPData.fComposedLine.IsVertical() ) {
		fPData.fComposedLine.fLogicalExtents.FourthToThird( 0 );
	}

	fPData.fComposedLine.fOrg.y = fPData.fComposedLine.fBaseline;

	doit = fCol->GetStrip2( fPData.fComposedLine, fPData.fBreakType, *this );

	fPData.fComposedLine.fBaseline	= fPData.fComposedLine.fOrg.y;

	if ( fPData.fComposedLine.IsVertical() ) {
		fPData.fComposedLine.fOrg.ThirdToFourth( fCol->Width() );
		fPData.fComposedLine.fLogicalExtents.ThirdToFourth( 0 );
	}

	return doit;
}


/* ==================================================================== */
