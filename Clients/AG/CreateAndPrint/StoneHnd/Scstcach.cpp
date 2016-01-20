/***************************************************************************
Contains:	Code for the style cache sub-system within the
			Stonehand Composition Toolbox.
********************************************************************/

#include "scstcach.h"
#include "sccallbk.h"
#include "sccharex.h"
#include "screfdat.h"
#include "scparagr.h"
#include "scctype.h"
#include <math.h>

scCachedStyle*	scCachedStyle::fCachedStyles;
int 			scCachedStyle::fEntries;

int 			scCachedStyle::fLast;
long			scCachedStyle::fCacheTime;
scFlowDir		scCachedStyle::fFlowDir( eRomanFlow );

scCachedStyle		scCachedStyle::cachedParaStyle_;
const scContUnit*	scCachedStyle::cachedPara_;

/* ==================================================================== */
void scCachedStyle::BuildCache( int entries )
{
	fCachedStyles	= new scCachedStyle [entries];
	fEntries		= entries;
	fLast			= -1;
	fLast			= -1;
	cachedPara_ 	= 0;
}

/* ==================================================================== */
void scCachedStyle::DeleteCache( void )
{
	delete [] fCachedStyles, fCachedStyles = 0;
}

/* ==================================================================== */
int scCachedStyle::GetOldestIndex( void )
{
	int 	i,
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
		if ( fCachedStyles[i].GetSpec().ptr() == ts.ptr() ) {
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
	register int i;
	for ( i = 0; i < 256; i++ )
		fWidths[i] = kInvalMP;
}

/* ==================================================================== */
void scCachedStyle::ComputeExtentsnCursor( void )
{
	scXRect rect;
	FIgetDEVFontExtents( fSpec, rect );

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


	scAssert( fInkExtents.Valid( eFourthQuad ) );
	
	if ( fFlowDir.IsHorizontal() )
		fInkExtents.Translate( 0, -GetBaseline() );
	else 
		fInkExtents.Translate( GetBaseline(), 0 );	
	
	if ( fFlowDir.IsHorizontal() ) {
		scLEADRefData	ld;

		ld.Set( GetPtSize(), fFlowDir );
		fLogicalExtents.Set( 0, -ld.GetAboveLead(), GetSetSize(), ld.GetBelowLead() ); 												
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

		fPtConv 	= (double)GetGlyphHeight() / scBaseRLUsystem;
		fSetConv	= (double)GetGlyphWidth()  / scBaseRLUsystem;
		 
		InitWidths();
		ComputeExtentsnCursor();
	}
	else {
		TypeSpec nullSpec;
		SetSpec( nullSpec );
		fPtConv 	= 0;
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
	if ( ch >= 256 )
		return FIgetDEVEscapement( fSpec, ch );
	
	if ( fWidths[ch] == kInvalMP )
	{
		fWidths[ch] = FIgetDEVEscapement( fSpec, ch );
	}

	return fWidths[ch] + GetOptLSP();
}
	
/* ==================================================================== */	
GlyphSize scCachedStyle::GetKernValue( UCS2 ch1, UCS2 ch2 )
{
	return FIgetDEVKern( fSpec, ch1, ch2 );
}
