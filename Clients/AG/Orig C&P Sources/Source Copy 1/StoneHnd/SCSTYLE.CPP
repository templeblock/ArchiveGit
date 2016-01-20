/****************************************************************************

    File:       SCSTYLE.CPP

    $Header: /Projects/Toolbox/ct/SCSTYLE.CPP 2     5/30/97 8:45a Wmanis $
    
    Written by: Manis

    Copyright (c) 1989-1994 Stonehand Inc., of Cambridge, MA.  
    All rights reserved.

    This notice is intended as a precaution against inadvertent publication
    and does not constitute an admission or acknowledgment that publication
    has occurred or constitute a waiver of confidentiality.

    Composition Toolbox software is the proprietary
    and confidential property of Stonehand Inc.

****************************************************************************/

#include "scstyle.h"
#include "scctype.h"

int basefontsize_ = 10;

/* ==================================================================== */	

void scStyle::SmallCapCorrection()
{	
	if ( 0 && fSmallCaps ) {
		glyphHeight_	= scRoundGS( kSmallCapCorrection * fPtSize );
		glyphWidth_		= scRoundGS( kSmallCapCorrection * fSetSize );	
	}	
	else {
		glyphHeight_	= fPtSize;
		glyphWidth_		= fSetSize;	
	}		
}

/* ==================================================================== */	

UCS2 scStyle::SmallCapsCorrection( UCS2 ch ) const
{
	return ::CTToUpper( ch );
}

/* ==================================================================== */	

void scStyle::SetBulletEmphasis( eEmphasisType etype, Bool allBits )
{
	if ( ! allBits ) {
		switch ( etype ) {
			default:
				break;
			case eNormal:
				bulletEmphasis_ = 0;
				break;
			case eItalic:
			case eBold:
			case eUnderline:
			case eStrikeThru:
				bulletEmphasis_ |= (ulong)etype;
				break;
		}
	} 
	else
		bulletEmphasis_ = etype;
}

/* ==================================================================== */	

void scStyle::SetEmphasis( eEmphasisType etype, Bool allBits )
{
	if ( ! allBits ) {
		switch ( etype ) {
			default:
				break;
			case eNormal:
				fEmphasis = 0;
				break;
			case eItalic:
			case eBold:
			case eUnderline:
			case eStrikeThru:
				fEmphasis |= (ulong)etype;
				break;
		}
	} 
	else
		fEmphasis = etype;
}

/* ==================================================================== */	

void scStyle::SetDropEmphasis( eEmphasisType etype, Bool allBits )
{
	if ( ! allBits ) {
		switch ( etype ) {
			default:
				break;
			case eNormal:
				dropEmphasis_ = 0;
				break;
			case eItalic:
			case eBold:
			case eUnderline:
			case eStrikeThru:
				dropEmphasis_ |= (ulong)etype;
				break;
		}
	} 
	else
		dropEmphasis_ = etype;
}

/* ==================================================================== */	

int scStyle::operator==( const scStyle& st ) const
{
	if ( fFont != st.fFont )
		return 0;

	if ( fEmphasis != st.fEmphasis )
		return 0;

	if ( fSmallCaps != st.fSmallCaps )
		return 0;

	if ( fAllCaps != st.fAllCaps )
		return 0;
	
	if ( fColor != st.fColor )
		return 0;

	if ( fPtSize != st.fPtSize )
		return 0;
	if ( fSetSize != st.fPtSize )
		return 0;

	if ( fDeviceValues != st.fDeviceValues )
		return 0;
											// use device values and not design
											// values
	
	if ( fHOblique != st.fHOblique )
		return 0;
	if ( fVOblique != st.fVOblique )
		return 0;
	if ( fRotation != st.fRotation )
		return 0;

	if ( fHorzBaseline != st.fHorzBaseline )
		return 0;
	if ( fVertBaseline != st.fVertBaseline )
		return 0;
	
	if ( fBaseline != st.fBaseline )
		return 0;

	
			////////////////////////////////////////////////
			////////// MULTIPLE CHARACTER PROPERTIES ///////
			////////////////////////////////////////////////
	
	if ( fKern != st.fKern )
		return 0;

	if ( fMinLSP != st.fMinLSP )
		return 0;
	if ( fOptLSP != st.fOptLSP )
		return 0;
	if ( fMaxLSP != st.fMaxLSP )
		return 0;
	

	if ( fMinWord != st.fMinWord )
		return 0;
	if ( fOptWord != st.fOptWord )
		return 0;
	if ( fMaxWord != st.fMaxWord )
		return 0;

	
	//////////////////////////////////////////////////////////////////////		
	////////////////////// LINE LEVEL PROPERTIES /////////////////////////
	//////////////////////////////////////////////////////////////////////	

	if ( fMinMeasure != st.fMinMeasure )
		return 0;
	
	if ( fLeftHang != st.fLeftHang )
		return 0;
	if ( fRightHang != st.fRightHang )
		return 0;
	
	if ( fLeftBlockIndent != st.fLeftBlockIndent )
		return 0;
	if ( fRightBlockIndent != st.fRightBlockIndent )
		return 0;

	if ( fDecimalChar != st.fDecimalChar )
		return 0;
	
			////////////////////////////////////////////////
			////////// MULTIPLE LINE PROPERTIES ////////////
			////////////////////////////////////////////////
	
	if ( fLead != st.fLead )
		return 0;
	if ( fMaxLead != st.fMaxLead )
		return 0;
	if ( fHyphenate != st.fHyphenate )
		return 0;
	if ( fCaseHyphs != st.fCaseHyphs )
		return 0;
	if ( fAcronymHyphs != st.fAcronymHyphs )
		return 0;
	if ( fMaxConsHyphs != st.fMaxConsHyphs )
		return 0;
	if ( fHyphChar != st.fHyphChar )
		return 0;
	if ( fHyphLanguage != st.fHyphLanguage )
		return 0;
	if ( fPreHyph != st.fPreHyph )
		return 0;
	if ( fPostHyph != st.fPostHyph )
		return 0;
	if ( fMaxWordHyph != st.fMaxWordHyph )
		return 0;
	
	//////////////////////////////////////////////////////////////////////		
	////////////////// PARAGRAPH LEVEL PROPERTIES ////////////////////////
	//////////////////////////////////////////////////////////////////////
	
	if ( fBreakLang != st.fBreakLang )
		return 0;
	if ( fAutoWrap != st.fAutoWrap )
		return 0;

	if ( fRag != st.fRag )
		return 0;

	if ( fRagZone != st.fRagZone )
		return 0;
	if ( fDiffRagZone != st.fDiffRagZone )
		return 0;
	
	if ( fIndentAmount != st.fIndentAmount )
		return 0;
	if ( fLinesToIndent != st.fLinesToIndent )
		return 0;

	if ( bulletChar_ != st.bulletChar_ )
		return 0;
	if ( bulletFontFamily_ != st.bulletFontFamily_ )
		return 0;
	if ( bulletEmphasis_ != st.bulletEmphasis_ )
		return 0;
	if ( bulletIndent_ != st.bulletIndent_ )
		return 0;

	if ( bulletRender_ != st.bulletRender_ )
		return 0;
	if ( bulletColor_ != st.bulletColor_ )
		return 0;
	
	if ( dropLines_	!= st.dropLines_ )
		return 0;				
	if ( dropSize_ != st.dropSize_ )
		return 0;
	if ( dropSetWidth_ != st.dropSetWidth_ )
		return 0;
	if ( dropFontFamily_ != st.dropFontFamily_ )
		return 0;
	if ( dropEmphasis_ != st.dropEmphasis_ )
		return 0;
	if ( dropRender_ != st.dropRender_ )
		return 0;
	if ( dropColor_	!= st.dropColor_ )
		return 0;
	if ( dropHOffset_ != st.dropHOffset_ )
		return 0;
	if ( dropVOffset_ != st.dropVOffset_ )
		return 0;
	if ( dropHBorder_ != st.dropHBorder_ )
		return 0;
	if ( dropVBorder_ != st.dropVBorder_ )
		return 0;

	if ( fDropCap != st.fDropCap )
		return 0;
	if ( fMaxParaLead != st.fMaxParaLead )
		return 0;
	
	if ( fSpaceAbove != st.fSpaceAbove )
		return 0;
	if ( fMaxSpaceAbove != st.fMaxSpaceAbove )
		return 0;	
	if ( fSpaceBelow != st.fSpaceBelow )
		return 0;
	if ( fMaxSpaceBelow != st.fMaxSpaceBelow )
		return 0;	

	if ( fLinesBefore != st.fLinesBefore )
		return 0;
	if ( fLinesAfter != st.fLinesAfter )
		return 0;
	if ( fNoBreak != st.fNoBreak )
		return 0;
	if ( fKeepWithNext != st.fKeepWithNext )
		return 0;

	for ( int i = 0; i < kMaxTabs; i++ ) {
		if ( tabs_[i] != st.tabs_[i] )
			return 0;
	}
	return 1;
}
