/***************************************************************************

	File:		SCSTCACH.CPP


	$Header: /Projects/Toolbox/ct/SCSTCACH.CPP 4     6/17/97 4:16p Wmanis $
	
	Contains:	Code for the style cache sub-system within the
				Stonehand Composition Toolbox.

	Written by: Manis


	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.


********************************************************************/

#include "scstcach.h"
#include "sccallbk.h"
#include "sccharex.h"
#include "screfdat.h"
#include "scparagr.h"
#include "scctype.h"
#include <math.h>

scCachedStyle*	scCachedStyle::fCachedStyles;
int				scCachedStyle::fEntries;

int				scCachedStyle::fLast;
long			scCachedStyle::fCacheTime;
scFlowDir		scCachedStyle::fFlowDir( eRomanFlow );

scCachedStyle		scCachedStyle::cachedParaStyle_;
const scContUnit*   scCachedStyle::cachedPara_;

/* ==================================================================== */

void scCachedStyle::BuildCache( int entries )
{
	fCachedStyles	= new scCachedStyle [entries];
	fEntries		= entries;
	fLast 			= -1;
	fLast 			= -1;
	cachedPara_		= 0;
}

/* ==================================================================== */

void scCachedStyle::DeleteCache( void )
{
	delete [] fCachedStyles, fCachedStyles = 0;
}

/* ==================================================================== */

int scCachedStyle::GetOldestIndex( void )
{
	int		i,
			oldest = -1;
	long	oldestTime = LONG_MAX;

	for ( i = 0; i < fEntries; i++ ) {
		if ( oldestTime > fCachedStyles[i].fTimeStamp ) {
			oldestTime = fCachedStyles[i].fTimeStamp;
			oldest = i;
		}
	}
	scAssert( oldest >= 0 );

	return oldest;	
}

/* ==================================================================== */

scCachedStyle& scCachedStyle::GetCachedStyle( TypeSpec& ts )	
{
	if ( fLast >= 0 ) {
		if ( fCachedStyles[fLast].GetSpec().ptr() == ts.ptr() )
			return fCachedStyles[fLast];
	}

	return FindCachedStyle( ts );
}
									  

/* ==================================================================== */

scCachedStyle&	scCachedStyle::FindCachedStyle( TypeSpec& ts )
{
	int i;

	scAssert( ts.ptr() );

	for ( i = 0; i < fEntries; i++ ) {
		if ( fCachedStyles[i].GetSpec().ptr() == ts.ptr() )	{
			fLast = i;
//			SCDebugTrace( 0, "scCachedStyle::FindCachedStyle: found %d\n", fLast );						
			return fCachedStyles[i];
		}
	}

	int oldest = GetOldestIndex();

	fCachedStyles[oldest].Init( ts );	
	fLast = oldest;
	
//	SCDebugTrace( 0, "scCachedStyle::FindCachedStyle: new cache %d\n", oldest );
	
	return fCachedStyles[oldest];			// now the newest
}

/* ==================================================================== */

void scCachedStyle::StyleInvalidateCache( TypeSpec& ts )
{
	int i;

	TypeSpec nullSpec;

	for ( i = 0; i < fEntries; i++ ) {
		if ( ts.ptr() == fCachedStyles[i].GetSpec().ptr() )
			fCachedStyles[i].Init( nullSpec );
		else
			fCachedStyles[i].Init( nullSpec );			
	}
	cachedParaStyle_.Init( nullSpec );
}

/* ==================================================================== */

void scCachedStyle::SetParaStyle( const scContUnit* cu, TypeSpec& ts )
{
	if ( ts.ptr() == cachedParaStyle_.fSpec.ptr() )
		return;
	cachedPara_ = cu;
	cachedParaStyle_.Init( ts );
}

/* ==================================================================== */

MicroPoint scCachedStyle::GetParaSpace( scContUnit* cu1,
										  scContUnit* cu2 )
{
	SetParaStyle( cu1, cu1->GetDefaultSpec() );
	MicroPoint below = cachedParaStyle_.GetSpaceBelow();

	SetParaStyle( cu2, cu2->GetDefaultSpec() );
	MicroPoint above = cachedParaStyle_.GetSpaceAbove();

	return below + above;
}

/* ==================================================================== */
	
MicroPoint scCachedStyle::GetMaxParaSpace( scContUnit* cu1,
											 scContUnit* cu2 )
{
	SetParaStyle( cu1, cu1->GetDefaultSpec() );
	MicroPoint below = cachedParaStyle_.GetMaxSpaceBelow();

	SetParaStyle( cu2, cu2->GetDefaultSpec() );
	MicroPoint above = cachedParaStyle_.GetMaxSpaceAbove();

	return below + above;
}

/* ==================================================================== */
	
void scCachedStyle::SetFlowdir( const scFlowDir& fd )
{
	if ( scCachedStyle::fFlowDir != fd ) {
		int i;

		for ( i = 0; i < fEntries; i++ ) {
			if ( fCachedStyles[i].GetSpec().ptr() )
				fCachedStyles[i].InitFlowdir( fd );
		}
	}		
}

/* ==================================================================== */

inline void scCachedStyle::InitWidths( )
{
	register i;
	for ( i = 0; i < 256; i++ )
		fWidths[i] = kInvalMP;
}

/* ==================================================================== */

void scCachedStyle::ComputeExtentsnCursor( void )
{
	REAL			obliqOff;
	
	if ( GetDeviceValues() ) {
		scXRect			rect;
		MicroPoint		a,b,c,d;

		FIgetDEVFontExtents( fSpec, a, b, c, d, rect );

		if ( fFlowDir.IsHorizontal() ) {
			scAssert( rect.Valid( eFirstQuad ) );
			rect.FirstToFourth( GetPtSize() );
			scAssert( rect.Valid( eFourthQuad ) );
			
			fInkExtents.y1	= rect.y1;
			fInkExtents.y2	= rect.y2;
			fInkExtents.x1	= rect.x1;
			fInkExtents.x2	= rect.x2;
		}
		else {
			fInkExtents.y1	= 0;
			fInkExtents.y2	= rect.y1 - rect.y2;
			fInkExtents.x1	= -(rect.x2/2) - rect.x1;
			fInkExtents.x2	= rect.x2/2;
		}
	}
	else {
		scRLURect		rect;
		RLU				a,b,c,d;

		FIgetRLUFontExtents( fSpec, a, b, c, d, rect );

		if ( fFlowDir.IsHorizontal() ) {
			scAssert( rect.Valid( eFirstQuad ) );
			rect.FirstToFourth( scBaseRLUsystem );
		}
		scAssert( rect.Valid( eFourthQuad ) );

		fInkExtents.y1	= scRoundMP( fPtConv * rect.rluTop );
		fInkExtents.y2	= scRoundMP( fPtConv * rect.rluBottom );
		fInkExtents.x1	= scRoundMP( fSetConv * rect.rluLeft );
		fInkExtents.x2	= scRoundMP( fSetConv * rect.rluRight );		

	}

	if (  GetHorzOblique()  )
		obliqOff = (REAL)tan( AngleToRadians( GetHorzOblique() ) );

	if ( GetHorzOblique() < 0 )
		 fInkExtents.x1	+= scRoundMP( GetPtSize() * obliqOff );

	if ( GetHorzOblique() < 0 )	
		fInkExtents.x2 += scRoundMP( GetPtSize() * obliqOff );

	scAssert( fInkExtents.Valid( eFourthQuad ) );
	
	if ( fFlowDir.IsHorizontal() )
		fInkExtents.Translate( 0, -GetBaseline() );
	else 
		fInkExtents.Translate( GetBaseline(), 0 );	
	
	if ( fFlowDir.IsHorizontal() ) {
		scLEADRefData	ld;

		ld.Set( GetPtSize(), fFlowDir );
		fLogicalExtents.Set( 0, -ld.GetAboveLead(),
							 GetSetSize(), ld.GetBelowLead() );													
	}
	else
		fLogicalExtents.Set( -GetSetSize()/2, 0, GetSetSize()/2, GetPtSize() );

	
	if ( fFlowDir.IsHorizontal() ) {
		fCursorY1		= -scRoundMP( fPtConv * RLU_BASEfmTop );
		fCursorY2		= scRoundMP( fPtConv * RLU_BASEfmBottom );
	}
	else {
		fCursorX1		= -GetSetSize() / 2;
		fCursorX2		= GetSetSize() / 2;
	}
}

/* ==================================================================== */
	
void scCachedStyle::Init( TypeSpec& ts )
{
	if ( ts.ptr() ) {
		TSGetStyle( ts, *this );
		
		SetSpec( ts );
		fTimeStamp = ++scCachedStyle::fCacheTime;

		fPtConv		= (REAL)GetGlyphHeight() / scBaseRLUsystem;
		fSetConv	= (REAL)GetGlyphWidth()  / scBaseRLUsystem;
		 
		InitWidths();
		ComputeExtentsnCursor();
	}
	else {
		TypeSpec nullSpec;
		SetSpec( nullSpec );
		fPtConv		= 0;
		fSetConv	= 0;
		InitWidths();
		fTimeStamp = 0;
	}
		
}

/* ==================================================================== */

void scCachedStyle::InitFlowdir( const scFlowDir& fd )
{
	fFlowDir = fd;
	InitWidths();
	ComputeExtentsnCursor();
}

/* ==================================================================== */

GlyphSize scCachedStyle::GetEscapement( UCS2 ch )
{
	if ( GetDeviceValues() ) {

		if ( ch >= 256 )
			return FIgetDEVEscapement( fSpec, ch );
		
		if ( fWidths[ch] == kInvalMP ) {
			if ( GetSmallCaps() && CTIsLowerCase( ch ) )
				fWidths[ch] = scRoundMP( FIgetDEVEscapement( fSpec, ::CTToUpper( ch ) ) * kSmallCapCorrection );				
			else
				fWidths[ch] = FIgetDEVEscapement( fSpec, GetCorrectedGlyph( ch ) );
		}
	}
	else {
		REAL		conversion;	

		if ( fFlowDir.IsHorizontal() )
			conversion = fSetConv;
		else
			conversion = fPtConv;

		if ( ch >= 256 ) {
			RLU rluWidth = FIgetRLUEscapement( fSpec, ch );
			if ( rluWidth == scBaseRLUsystem ) {
				if ( fFlowDir.IsHorizontal() ) 
					return GetSetSize();
				else
					return GetPtSize();
			}	
			return scRoundGS( conversion * rluWidth ) + GetOptLSP();
		}

		if ( fWidths[ch] == kInvalMP ) {
				// it has not been previously computed
			switch ( ch ) {
				default:
					{
						RLU	rluWidth;
						if ( GetSmallCaps() && CTIsLowerCase( ch ) )
							rluWidth = (RLU)scRoundGS( FIgetRLUEscapement( fSpec, ::CTToUpper( ch ) ) * kSmallCapCorrection );
						else
							rluWidth = FIgetRLUEscapement( fSpec, GetCorrectedGlyph( ch ) );
						
						if ( rluWidth == scBaseRLUsystem ) {
							fWidths[ch] = scRoundMP( conversion * rluWidth );
							if ( fFlowDir.IsHorizontal() )		
								fWidths[ch] = GetSetSize() + GetOptLSP();
							else
								fWidths[ch] = GetPtSize() + GetOptLSP();
						}
						else
							fWidths[ch] = scRoundMP( conversion * rluWidth );
					}
				break;

				case 0:
				case scVertTab:
				case scHardReturn:
					if ( fFlowDir.IsHorizontal() )
						fWidths[ch] = GetSetSize();
					else
						fWidths[ch] = GetPtSize();
					break;

				case scNoBreakSpace:
					fWidths[ch]	= GetOptWord();
					break;

					// these really need to be further up stream
				case scNoBreakHyph:
				case scDiscHyphen:
					fWidths[ch]	= 0;
					break;
				case scFigureSpace:
					fWidths[ch]	= scRoundMP( conversion * FIgetRLUEscapement( fSpec, '0' ) );
					break;
				case scThinSpace:
					fWidths[ch]	= scRoundMP( conversion * scBaseRLUsystem / 6 );
					break;
				case scEnSpace:
					fWidths[ch]	= scRoundMP( conversion * scBaseRLUsystem / 2 );
					break;
				case scEmSpace:
					fWidths[ch]	= scRoundMP( conversion * scBaseRLUsystem );
					break;
			}
		}
		if ( fWidths[ch] == 0 )
			return 0;
	}
	return fWidths[ch] + GetOptLSP();
}
	
/* ==================================================================== */	

GlyphSize scCachedStyle::GetKernValue( UCS2 ch1, UCS2 ch2 )
{
	RLU				kern;
	
	if ( GetDeviceValues() )
		return FIgetDEVKern( fSpec, ch1, ch2 );
	else {
		kern = FIgetRLUKern( fSpec, ch1, ch2 );

		if ( kern != 0 ) {
			if ( fFlowDir.IsHorizontal() )
				return scRoundGS( fSetConv * kern );
			return scRoundGS( fPtConv * kern );
		}
	}
	return 0;
}

/* ==================================================================== */	
