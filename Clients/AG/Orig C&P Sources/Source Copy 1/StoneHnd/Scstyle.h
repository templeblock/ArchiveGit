/*****************************************************************************

	File:		SCSTYLE.H

	$Header: /Projects/Toolbox/ct/SCSTYLE.H 2     5/30/97 8:45a Wmanis $
		
	
	Contains:	This is the style (aka typographic state record )
				that is used to pass typographic state to the toolbox.
				The default constructor fills in default values for most
				values ( not font ). This is used internally by the toolbox
				to cache state changes and probably should be used on the
				outside to do the same.

	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

@doc

To add new fields.

  1. add field
  2. add accessors
  3. fix constructor
  4. fix default constructor
  5. fix operator==
  6. add field to specapi
  7. add stuff to type validator (isPtr,
  8. add code to computedelta

****************************************************************************/


#ifndef _H_SCSTYLE
#define _H_SCSTYLE

#include "sctypes.h"

// @class The scStyle class is a monlithic structure that contains
// typographic state. It is used to pass typographic state to the toolbox.
// The default constructor fills in default values for most
// values ( not font ). This is used internally by the toolbox
// to cache state changes and probably should be used on the
// outside to do the same.
// @xref <f TSGetStyle>

// Maximum number of tabs in a scope unit
// If kMaxTabs is increased remember to add new deltas in spec manager
#define kMaxTabs			10

#define kSmallCapCorrection	0.70


extern int basefontsize_;

class scStyle {
public:
				scStyle( APPFont font = "Times New Roman", 
						 GlyphSize ptsize = (GlyphSize)(one_point * basefontsize_) ) :
							fStructSize( sizeof( scStyle ) ),
							fUnique( 0 ),
							fFont( font ),
							fEmphasis( eNormal ),
							fSmallCaps( 0 ),
							fAllCaps( 0 ),
							fRender( 0 ),
							fSmallCapRender( 0 ),
							fColor( 0 ),
							fBreakLang( eCompRoman ),
							fRag( eRagRight ),
							fAutoWrap( true ),
							fRagZone( 0 ),
							fDiffRagZone( 0 ),
							fKern( false ),
							fPtSize( ptsize ),
							fSetSize( ptsize  ),
							glyphHeight_( 0 ),
							glyphWidth_( 0 ),
							fDeviceValues( true ),
							fHOblique( 0 ),
							fVOblique( 0 ),							
							fRotation( 0  ),
							fMinMeasure( one_pica * 2 ),
							fLeftHang( 0 ),
							fRightHang( 0 ),
							fLeftBlockIndent( 0 ),
							fRightBlockIndent( 0 ),
							fIndentAmount( 0 ),
							fLinesToIndent( 0 ),						
							bulletChar_( 0x0095 ),
							bulletFontFamily_( font ),
							bulletEmphasis_( 0 ),
							bulletIndent_( scPICAS(2) ),
							bulletRender_( 0 ),
							bulletColor_( 0 ),
							dropLines_( 0 ),
							dropSize_( 0 ),
							dropSetWidth_( 0 ),
							dropFontFamily_( font ),
							dropEmphasis_( 0 ),
							dropRender_( 0 ),					
							dropColor_( 0 ),
							dropHOffset_( 0 ),
							dropVOffset_( 0 ),
							dropHBorder_( 0 ),
							dropVBorder_( 0 ),
							fDropCap( false ),
							fLead( one_point * 2 ),
							fSpaceAbove( one_point * 6 ),
							fSpaceBelow( 0  ),
							fMaxLead( ( fPtSize + fLead ) * 2 ),
							fMaxSpaceAbove( fMaxLead ),
							fMaxSpaceBelow( 0 ),
							fHorzBaseline( kRomanBaseline ),
							fVertBaseline( kCenterBaseline ),	
							fBaseline( 0 ),
							fMinLSP( 0 ),
							fOptLSP( 0 ),
							fMaxLSP( 0 ),
							fMinWord( (GlyphSize)(ptsize / 4) ),
							fOptWord( (GlyphSize)(ptsize / 2) ),
							fMaxWord( ptsize ),							
							fHyphenate( false ),
							fCaseHyphs( false ),
							fAcronymHyphs( false ),
							fMaxConsHyphs( 2 ),
							fHyphChar( '-' ),
							fHyphLanguage( 1 ),
							fPreHyph( 2 ),
							fPostHyph( 2 ),
							fMaxWordHyph( 4 ),
							fDecimalChar( '.' ),
							fLinesBefore( 0 ),
							fLinesAfter( 0 ),
							fNoBreak( false ),
							fKeepWithNext( false ),
							fNumbered( 0 ),
							fAbsoluteLead( 0 )
								{
									// set default tab in tab array position 0
									SetTab( scTabInfo( one_inch/2, eTBLeftAlign ), 0 );
								}
									
				scStyle( const scStyle& st ) :
							fStructSize( sizeof( scStyle ) ),
							fUnique( st.fUnique ),
							fFont( st.fFont ),
							fEmphasis( st.fEmphasis ),
							fSmallCaps( st.fSmallCaps ),
							fAllCaps( st.fAllCaps ),
							fRender( st.fRender ),
							fSmallCapRender( st.fSmallCapRender ),
							fColor( st.fColor ),
							fBreakLang( st.fBreakLang ),
							fRag( st.fRag ),
							fAutoWrap( st.fAutoWrap ),
							fRagZone( st.fRagZone ),
							fDiffRagZone( st.fDiffRagZone ),
							fKern( st.fKern ),
							fPtSize( st.fPtSize ),
							fSetSize( st.fSetSize ),
							glyphHeight_( st.glyphHeight_ ),
							glyphWidth_( st.glyphWidth_ ),
							fDeviceValues( st.fDeviceValues ),	
							fHOblique( st.fHOblique ),
							fVOblique( st.fVOblique ),							
							fRotation( st.fRotation ),
							fMinMeasure( st.fMinMeasure ),
							fLeftHang( st.fLeftHang ),
							fRightHang( st.fRightHang ),
							fLeftBlockIndent( st.fLeftBlockIndent ),
							fRightBlockIndent( st.fRightBlockIndent ),
							fIndentAmount( st.fIndentAmount ),
							fLinesToIndent( st.fLinesToIndent ),						
							bulletChar_( st.bulletChar_ ),
							bulletFontFamily_( st.bulletFontFamily_ ),
							bulletEmphasis_( st.bulletEmphasis_ ),
							bulletIndent_( st.bulletIndent_ ),
							bulletRender_( st.bulletRender_ ),
							bulletColor_( st.bulletColor_ ),							
							dropLines_( st.dropLines_ ),
							dropSize_( st.dropSize_ ),
							dropSetWidth_( st.dropSetWidth_ ),
							dropFontFamily_( st.dropFontFamily_ ),
							dropEmphasis_( st.dropEmphasis_ ),
							dropRender_( st.dropRender_ ),
							dropColor_( st.dropColor_ ),
							dropHOffset_( st.dropHOffset_ ),
							dropVOffset_( st.dropVOffset_ ),
							dropHBorder_( st.dropHBorder_ ),
							dropVBorder_( st.dropVBorder_ ),
							fDropCap( st.fDropCap ),
							fLead( st.fLead ),
							fSpaceAbove( st.fSpaceAbove ),
							fSpaceBelow( st.fSpaceBelow ),
							fMaxLead( st.fMaxLead ),
							fMaxSpaceAbove( st.fMaxSpaceAbove ),
							fMaxSpaceBelow( st.fMaxSpaceBelow ),
							fHorzBaseline( st.fHorzBaseline ),
							fVertBaseline( st.fVertBaseline ),
							fBaseline( st.fBaseline ),
							fMinLSP( st.fMinLSP ),
							fOptLSP( st.fOptLSP ),
							fMaxLSP( st.fMaxLSP ),
							fMinWord( st.fMinWord ),
							fOptWord( st.fOptWord ),
							fMaxWord( st.fMaxWord ),
							fHyphenate( st.fHyphenate ),
							fCaseHyphs( st.fCaseHyphs ),
							fAcronymHyphs( st.fAcronymHyphs ),
							fMaxConsHyphs( st.fMaxConsHyphs ),
							fHyphChar( st.fHyphChar ),
							fHyphLanguage( st.fHyphLanguage ),
							fPreHyph( st.fPreHyph ),
							fPostHyph( st.fPostHyph ),
							fMaxWordHyph( st.fMaxWordHyph ),
							fDecimalChar( st.fDecimalChar ),
							fLinesBefore( st.fLinesBefore ),
							fLinesAfter( st.fLinesAfter ),
							fNoBreak( st.fNoBreak ),
							fKeepWithNext( st.fKeepWithNext ),
							fNumbered ( st.fNumbered ),
							fAbsoluteLead( st.fAbsoluteLead )
							{
								for ( int i = 0; i < kMaxTabs; i++ )
									tabs_[i] = st.tabs_[i];
							}
						
	uint32		GetUnique() const
					{
						return fUnique;
					}
	void		SetUnique( uint32 val )
					{
						fUnique = val;
					}

	APPFont		GetFont( void ) const
					{
						return fFont;
					}
	void		SetFont( APPFont font )
					{
						fFont = font;
					}

	eEmphasisType GetEmphasis( void ) const
					{
						return (eEmphasisType)fEmphasis;
					}
	void		SetEmphasis( eEmphasisType etype, Bool allBits = false );	

	Bool		GetSmallCaps( void ) const
					{
						return fSmallCaps;
					}
	void		SetSmallCaps( Bool tf )
					{
						fSmallCaps = tf;
					}

	Bool		GetAllCaps( void ) const
					{
						return fAllCaps;
					}
	void		SetAllCaps( Bool tf )
					{
						fAllCaps = tf;
					}
	
	APPRender	GetRender( void ) const
					{
						return fRender;
					}
	void		SetRender( APPRender ar )
					{
						fRender = ar;
					}
		
	APPRender	GetSmallCapRender( void ) const
					{
						return fSmallCapRender;
					}
	void		SetSmallCapRender( APPRender ar )
					{
						fSmallCapRender = ar;
					}

	APPColor	GetColor( void ) const
					{
						return fColor;
					}
	void		SetColor( APPColor col )
					{
						fColor = col;
					}
	
	eTSCompLang	GetBreakLang( void ) const
					{
						return fBreakLang;
					}
	void		SetBreakLang( eTSCompLang bl )
					{
						fBreakLang = bl;
					}

	eTSJust		GetRag( void ) const
					{
						return fRag;
					}
	void		SetRag( eTSJust rag )			
					{ 
						fRag = rag;
					}

	Bool		GetAutoWrap( void ) const
					{
						return fAutoWrap;
					}
	void		SetAutoWrap( Bool autowrap )
					{
						fAutoWrap = autowrap;
					}

	MicroPoint	GetRagZone( void ) const
					{
						return fRagZone;
					}
	void		SetRagZone( MicroPoint ragzone )
					{
						fRagZone = ragzone;
					}

	MicroPoint	GetDiffRagZone( void ) const
					{
						return fDiffRagZone;
					}
	void		SetDiffRagZone( MicroPoint drz )
					{
						fDiffRagZone = drz;
					}

	Bool		GetKern( void ) const
					{
						return fKern;
					}
	void		SetKern( Bool kern )
					{
						fKern = kern;
					}

	GlyphSize	GetPtSize( void ) const
					{
						return fPtSize;
					}
	void		SetPtSize( GlyphSize ptsize )
					{
						fPtSize = fSetSize = ptsize;
					}

	GlyphSize	GetGlyphHeight( void ) const
					{
						return glyphHeight_;
					}
	GlyphSize	GetGlyphWidth( void ) const
					{
						return glyphWidth_;
					}

	GlyphSize	GetSetSize( void ) const
					{
						return fSetSize;
					}
	void		SetSetSize( GlyphSize setsize )
					{
						fSetSize = setsize;
					}

	Bool		GetDeviceValues( void ) const
					{
						return fDeviceValues;
					}
	void		SetDeviceValues( Bool tf )
					{
						fDeviceValues = tf;
					}
	
	scAngle		GetHorzOblique( void ) const
					{
						return fHOblique;
					}
	void		SetHorzOblique( scAngle oblique )
					{
						fHOblique = oblique;
					}

	scAngle		GetVertOblique( void ) const
					{
						return fVOblique;
					}
	void		SetVertOblique( scAngle oblique )
					{
						fVOblique = oblique;
					}
	
	scAngle		GetRotation( void ) const
					{
						return fRotation;
					}
	void		SetRotation( scAngle rotation )
					{
						fRotation = rotation;
					}

	MicroPoint	GetMinMeasure( void ) const
					{
						return fMinMeasure;
					}
	void		SetMinMeasure( MicroPoint mm )
					{
						fMinMeasure = mm;
					}

	TenThousandth GetLeftHang( void ) const
					{
						return fLeftHang;
					}
	void		SetLeftHang( TenThousandth mm )
					{
						fLeftHang = mm;
					}

	TenThousandth GetRightHang( void ) const
					{
						return fRightHang;
					}
	void		SetRightHang( TenThousandth mm )
					{
						fRightHang = mm;
					}
	
	MicroPoint	GetLeftBlockIndent( void ) const	
					{ 
						return fLeftBlockIndent; 
					}
	void		SetLeftBlockIndent( MicroPoint lbi )
					{ 
						fLeftBlockIndent = lbi; 
					}

	MicroPoint	GetRightBlockIndent( void ) const
					{ 
						return fRightBlockIndent; 
					}
	void		SetRightBlockIndent( MicroPoint rbi )
					{ 
						fRightBlockIndent = rbi; 
					}

	MicroPoint	GetIndentAmount( void ) const
					{
						return fIndentAmount;
					}
	void		SetIndentAmount( MicroPoint iamt )
					{
						fIndentAmount = iamt;
					}

	short		GetLinesToIndent( void ) const
					{
						return fLinesToIndent;
					}
	void		SetLinesToIndent( short lti )
					{
						fLinesToIndent = lti;
					}

	void		SetNumbered( Bool tf )
					{
						fNumbered = ( tf ? 1 : 0 );
					}
	Bool		GetNumbered( ) const
					{
						return fNumbered;
					}

	void		SetBulletChar( UCS2 ch )
					{
						bulletChar_ = ch;
					}
	UCS2		GetBulletChar() const
					{
						return bulletChar_;
					}

	void		SetBulletFontFamily( APPFont font ) 
					{
						bulletFontFamily_ = font;
					}
	APPFont		GetBulletFontFamily() const
					{
						return bulletFontFamily_;
					}

	void			SetBulletEmphasis( eEmphasisType etype, Bool allBits = false );	
	eEmphasisType	GetBulletEmphasis( ) const
					{
						return (eEmphasisType)bulletEmphasis_;
					}
	void			SetBulletIndent( MicroPoint indent )
					{
						bulletIndent_ = indent; 
					}
	MicroPoint		GetBulletIndent() const
					{
						return bulletIndent_;
					}

	void			SetBulletRender( APPRender r )
			{
				bulletRender_ = r;
			}
	APPRender		GetBulletRender( ) const
			{
				return bulletRender_;		
			}
	void		SetBulletColor( APPColor c )
			{
				bulletColor_ = c;
			}
	APPColor		GetBulletColor( ) const
			{
				return bulletColor_;		
			}
	
	Bool		GetDropCap( void ) const
					{
						return fDropCap;
					}
	void		SetDropCap( Bool dropcap )
					{
						fDropCap = dropcap;
					}

	short		GetDropLines() const
					{
						return dropLines_;
					}
	void		SetDropLines( short lines )
					{
						dropLines_ = lines;
					}

	MicroPoint	GetDropSize() const
					{
						return dropSize_;
					}
	void		SetDropSize( MicroPoint size )
					{
						dropSize_ = size;
					}

	MicroPoint	GetDropWidth() const
					{
						return dropSetWidth_;
					}
	void		SetDropWidth( MicroPoint width )
					{
						dropSetWidth_ = width;
					}
	
	APPFont		GetDropFontFamily() const
					{
						return dropFontFamily_;
					}
	void		SetDropFontFamily( APPFont font )
					{
						dropFontFamily_ = font;
					}


	eEmphasisType GetDropEmphasis( void ) const
					{
						return (eEmphasisType)dropEmphasis_;
					}
	void		SetDropEmphasis( eEmphasisType etype, Bool allBits = false );	

	
	APPColor	GetDropColor() const
					{
						return dropColor_;
					}
	void		SetDropColor( APPColor dc )
					{
						dropColor_ = dc;
					}
	APPRender	GetDropRender() const
					{
						return dropRender_;
					}
	void		SetDropRender( APPRender dr )
					{
						dropRender_ = dr;
					}
	
	void		SetDropHOffset( MicroPoint val )
					{
						dropHOffset_ = val;
					}
	MicroPoint	GetDropHOffset() const
					{
						return dropHOffset_;
					}
	
	void		SetDropVOffset( MicroPoint val )
					{
						dropVOffset_ = val;
					}
	MicroPoint	GetDropVOffset() const
					{
						return dropVOffset_;
					}
	
	void		SetDropHBorder( MicroPoint val )
					{
						dropHBorder_ = val;
					}
	MicroPoint	GetDropHBorder() const
					{
						return dropHBorder_;
					}
	
	void		SetDropVBorder( MicroPoint val )
					{
						dropVBorder_ = val;
					}
	MicroPoint	GetDropVBorder() const
					{
						return dropVBorder_;
					}
	
	MicroPoint	GetComputedLead( void ) const
					{
						if ( fAbsoluteLead )
							return fLead;			// absolute
						else
							return fPtSize + fLead;	// relative (default)
					}
	MicroPoint	GetLead( void ) const
					{
						return fLead;
					}
	void		SetLead( MicroPoint lead )
					{
						fLead = lead;
					}

	Bool	GetAbsoluteLead( void ) const
					{
						return fAbsoluteLead;
					}
	void	SetAbsoluteLead( Bool lead )
					{
						fAbsoluteLead = lead;
					}

	MicroPoint	GetSpaceAbove( void ) const
					{
						return fSpaceAbove;
					}
	void		SetSpaceAbove( MicroPoint abovelead )
					{
						fSpaceAbove = abovelead;
					}

	MicroPoint	GetSpaceBelow( void ) const
					{
						return fSpaceBelow;
					}
	void		SetSpaceBelow( MicroPoint belowlead )
					{
						fSpaceBelow = belowlead;
					}


	MicroPoint	GetMaxLead( void ) const
					{
						return fMaxLead;
					}
	void		SetMaxLead( MicroPoint mp )
					{
						fMaxLead = mp;
					}
	MicroPoint	GetComputedMaxLead( void ) const
					{
						return GetComputedLead() + fMaxLead;
					}

	MicroPoint	GetMaxSpaceAbove( void ) const
					{
						return fMaxSpaceAbove;
					}
	void		SetMaxSpaceAbove( MicroPoint mp )
					{
						fMaxSpaceAbove = mp;
					}

	MicroPoint	GetMaxSpaceBelow( void ) const
					{
						return fMaxSpaceBelow;
					}
	void		SetMaxSpaceBelow( MicroPoint mp )
					{
						fMaxSpaceBelow = mp;
					}
	
    eFntBaseline GetHorzBaseline( void ) const
					{
						return fHorzBaseline;
					}
	void		SetHorzBaseline( eFntBaseline b )
					{
						fHorzBaseline = b;
					}
	
	eFntBaseline GetVertBaseline( void ) const
					{
						return fVertBaseline;
					}
	void		SetVertBaseline( eFntBaseline b )
					{
						fVertBaseline = b;
					}
	
	MicroPoint	GetBaseline( void ) const
					{
						return fBaseline;
					}
	void		SetBaseline( GlyphSize baseline )
					{
						fBaseline = baseline;
					}

	GlyphSize	GetMinLSP( void ) const
					{
						return fMinLSP;
					}
	void		SetMinLSP( GlyphSize minlsp )
					{
						fMinLSP = minlsp; ClampLSPVals();
					}

	GlyphSize	GetOptLSP( void ) const
					{
						return fOptLSP;
					}
	void		SetOptLSP( GlyphSize optlsp )
					{
						fOptLSP = optlsp; ClampLSPVals();
					}

	GlyphSize	GetMaxLSP( void ) const
					{
						return fMaxLSP;
					}
	void		SetMaxLSP( GlyphSize maxlsp )
					{
						fMaxLSP = maxlsp; ClampLSPVals();
					}

	GlyphSize	GetMinWord( void ) const
					{
						return fMinWord;
					}
	void		SetMinWord( GlyphSize minword )
					{
						fMinWord = minword; ClampWordVals();
					}

	GlyphSize	GetOptWord( void ) const
					{
						return fOptWord;
					}
	void		SetOptWord( GlyphSize optword )
					{
						fOptWord = optword; ClampWordVals();
					}

	GlyphSize	GetMaxWord( void ) const
					{
						return fMaxWord;
					}
	void		SetMaxWord( GlyphSize maxword )
					{
						fMaxWord = maxword; ClampWordVals();
					}

	Bool 		GetHyphenate( void ) const
					{
						return fHyphenate;
					}
	void		SetHyphenate( Bool hyphenate )
					{
						fHyphenate = hyphenate;
					}

	Bool		GetCaseHyphs( void ) const
					{
						return fCaseHyphs;
					}
	void		SetCaseHyphs( Bool casehyphs )
					{
						fCaseHyphs = casehyphs;
					}

	Bool		GetAcronymHyphs( void ) const
					{
						return fAcronymHyphs;
					}
	void		SetAcronymHyphs( Bool acrhyphs )
					{
						fAcronymHyphs = acrhyphs;
					}

	short		GetMaxConsHyphs( void ) const
					{
						return fMaxConsHyphs;
					}
	void		SetMaxConsHyphs( short mxh )
					{
						fMaxConsHyphs = mxh;
					}

	UCS2		GetCorrectedGlyph( UCS2 ) const;
	
	UCS2		GetHyphChar( void ) const
					{
						return fHyphChar;
					}
	void		SetHyphChar( UCS2 hyphchar )
					{
						fHyphChar = hyphchar;
					}

	APPLanguage	GetHyphLanguage( void ) const
					{
						return fHyphLanguage;
					}
	void		SetHyphLanguage( APPLanguage hl )
					{
						fHyphLanguage = hl;
					}

	short		GetPreHyph( void ) const
					{
						return fPreHyph;
					}
	void		SetPreHyph( short prehyph )
					{
						fPreHyph = prehyph;
					}

	short		GetPostHyph( void ) const
					{
						return fPostHyph;
					}
	void		SetPostHyph( short posthyph )
					{
						fPostHyph = posthyph;
					}

	short		GetMaxWordHyph( void ) const
					{
						return fMaxWordHyph;
					}
	void		SetMaxWordHyph( short maxword )
					{
						fMaxWordHyph = maxword;
					}

	UCS2		GetDecimalChar( void ) const
					{
						return fDecimalChar;
					}
	void		SetDecimalChar( UCS2 ch )
					{
						fDecimalChar = ch;
					}

	short		GetLinesBefore( void ) const
					{
						return fLinesBefore;
					}
	void		SetLinesBefore( short lb )
					{
						fLinesBefore = lb;
					}

	short		GetLinesAfter( void ) const
					{
						return fLinesAfter;
					}
	void		SetLinesAfter( short lb )
					{
						fLinesAfter = lb;
					}
	
	Bool		GetNoBreak( void ) const
					{
						return fNoBreak;
					}
	void		SetNoBreak( Bool tf )
					{
						fNoBreak = tf;
					}

	Bool		GetKeepWithNext( void ) const
					{
						return fKeepWithNext;
					}
	void		SetKeepWithNext( Bool tf )
					{
						fKeepWithNext = tf;
					}

	void		SetTab( const scTabInfo& ti, int index )
					{
						scDebugAssert( index < kMaxTabs );
						tabs_[index] = ti;
					}

	const scTabInfo&	GetTab( int index ) const
					{
						scDebugAssert( index < kMaxTabs );
						return tabs_[index];
					}
	int			CountTabs() const	// count number of defined tabs
					{
						for ( int i = 0; i < kMaxTabs; i++ ) {
							if ( tabs_[i].tabAlign == eTBNoAlign )
								break;
						}
						return i;
					}
	
	int			operator==( const scStyle& ) const;

	void		SmallCapCorrection( void );

private:

	UCS2		SmallCapsCorrection( UCS2 ch ) const;
	
	int				fStructSize;			// size of structure for versioning

	uint32			fUnique;
	
	//////////////////////////////////////////////////////////////////////		
	////////////////// CHARACTER LEVEL PROPERTIES ////////////////////////
	//////////////////////////////////////////////////////////////////////
	
	APPFont			fFont;					// font from font catalog

	ulong			fEmphasis;				// font emphasis

	Bool			fSmallCaps;				// font small caps
	Bool			fAllCaps;				// all caps	

	APPRender		fRender;				// any additional font rendering attributes
	APPRender		fSmallCapRender;
	APPColor		fColor;					// color

	GlyphSize		fPtSize;				// vertical size
	GlyphSize		fSetSize;				// horizontal size

	GlyphSize		glyphHeight_;			// vertical size
	GlyphSize		glyphWidth_;			// horizontal size

	Bool			fDeviceValues;			// if this is set to true we will
											// use device values and not design
											// values
	
	scAngle			fHOblique;				// horizontal shear
	scAngle			fVOblique;				// vertical shear
	scAngle			fRotation;				// rotation about glyph center

	eFntBaseline	fHorzBaseline;			// horz. baseline type
	eFntBaseline	fVertBaseline;			// vertical baseline type
	
	MicroPoint		fBaseline;				// movement from baseline

	
			////////////////////////////////////////////////
			////////// MULTIPLE CHARACTER PROPERTIES ///////
			////////////////////////////////////////////////
	
	Bool			fKern;					// kerning on or off

	GlyphSize		fMinLSP;				// minimum letter spacing
	GlyphSize		fOptLSP;				// optimum letter spacing including tracking
	GlyphSize		fMaxLSP;				// maximum letter spacing
	
	void			ClampLSPVals( void )
					{
						fMinLSP = MIN( fMinLSP, fOptLSP );
						fMaxLSP = MAX( fMaxLSP, fOptLSP );
					}

	GlyphSize		fMinWord;				// minimum word space
	GlyphSize		fOptWord;				// optimum word space
	GlyphSize		fMaxWord;				// maximum letter space

	void			ClampWordVals( void )
					{
						fMinWord = MIN( fMinWord, fOptWord );
						fMaxWord = MAX( fMaxWord, fOptWord );
					}
	
	
	//////////////////////////////////////////////////////////////////////		
	////////////////////// LINE LEVEL PROPERTIES /////////////////////////
	//////////////////////////////////////////////////////////////////////	

	MicroPoint		fMinMeasure;			// minimum measure of the line
	
	TenThousandth	fLeftHang;				// percent to hang punc characters on left
	TenThousandth	fRightHang;				// percent to hang punc characters on right
	
	MicroPoint		fLeftBlockIndent;		// block indent on left
	MicroPoint		fRightBlockIndent;		// block indent on right

	UCS2			fDecimalChar;			// decimal char to look for in tabs
	
			////////////////////////////////////////////////
			////////// MULTIPLE LINE PROPERTIES ////////////
			////////////////////////////////////////////////
	
	MicroPoint		fLead;				// line leading
	MicroPoint		fMaxLead;			// max line leading

	Bool 			fHyphenate;			// hyphenation
	Bool			fCaseHyphs;			// yphenate upper case
	Bool			fAcronymHyphs;		// hyphenate acronyms
	short			fMaxConsHyphs;		// max number of consecutive hyphens

	UCS2			fHyphChar;			// hyphenation character
	APPLanguage		fHyphLanguage;		// hyphenation language
	short			fPreHyph;			// glyphs before hyphen
	short			fPostHyph;			// glyphs after hyphen
	short			fMaxWordHyph;		// size of word to hyphenate

	Bool			fAbsoluteLead;		// flag for absolute leading

	//////////////////////////////////////////////////////////////////////		
	////////////////// PARAGRAPH LEVEL PROPERTIES ////////////////////////
	//////////////////////////////////////////////////////////////////////
	
	eTSCompLang		fBreakLang;			// break language
	Bool			fAutoWrap;			// automatic wrap

	eTSJust			fRag;				// rag type
	MicroPoint		fRagZone;			// rag zone at the end of line
	MicroPoint		fDiffRagZone;		// rag difference (every line end must
										// differ this much from previous line end)

	MicroPoint		fIndentAmount;		// paragraph line indent amount
	short			fLinesToIndent;		// how many paragraph lines to indent


	UCS2			bulletChar_;
	APPFont			bulletFontFamily_;
	ulong			bulletEmphasis_;
	MicroPoint		bulletIndent_;

	APPRender		bulletRender_;		// any additional font rendering attributes
	APPColor		bulletColor_;
	
						// the drop cap val are here as place holders,
						// it is still up to the client to
						// determine their interpretation in TSDropCap()
	short			dropLines_;			// number of lines to drop			
    MicroPoint		dropSize_;			// drop glyph size
    MicroPoint		dropSetWidth_;		// drop glyph width
    APPFont			dropFontFamily_;	// drop glyph font
    ulong			dropEmphasis_;		// drop glyph font emphasis
	APPRender		dropRender_;			// any additional font rendering attributes
	APPColor		dropColor_;			// drop glyph color
	MicroPoint		dropHOffset_;		// horz offset from line origin
	MicroPoint		dropVOffset_;		// vert offset from line origin	
	MicroPoint		dropHBorder_;		// horz border around drop glyph
	MicroPoint		dropVBorder_;		// vert border around drop glyph	

	
	unsigned		fDropCap : 1;			// if true dropcaps are enabled
	unsigned		fMaxParaLead : 1;		// if true use maximum of above or below
	unsigned		fNumbered : 1;			// if true numbering (bullets) is on
	unsigned 		fNoBreak : 1;			// do not break this paragraph across a column
	unsigned 		fKeepWithNext : 1;		// keep this with the next paragraph

	MicroPoint		fSpaceAbove;			// space above paragraph
	MicroPoint		fMaxSpaceAbove;			// max space above paragraph
	
	MicroPoint		fSpaceBelow;			// space below paragraph
	MicroPoint		fMaxSpaceBelow;			// max space above paragraph
	
	short			fLinesBefore;		// min lines before a column break
	short			fLinesAfter; 		// min lines after a column break

	scTabInfo		tabs_[kMaxTabs];	// tab array, default tab in position 0
};


inline UCS2 scStyle::GetCorrectedGlyph( UCS2 ch ) const
{
	if ( fSmallCaps || fAllCaps )
		return SmallCapsCorrection( ch );

	return ch;
}
#endif
