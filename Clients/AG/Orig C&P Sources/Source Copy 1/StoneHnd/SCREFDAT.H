/****************************************************************************

	File:		screfdat.h


	$Header: /Projects/Toolbox/ct/SCREFDAT.H 3     5/30/97 8:45a Wmanis $

	Contains:	Contans structs and classes for performing
				reformattng of text.

	Written by: Manis

	Copyright (c) 1989-1994 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.


****************************************************************************/


#ifndef _H_SCREFDAT
#define _H_SCREFDAT


#include "sctbobj.h"
#include "scspcrec.h"

class scRubiArray;
class PrevParaData;
class scContUnit;
class scColumn;
class scTextline;
class HRgn;
class scRedispList;


typedef enum eBreakTypes {
	eUndefinedBreak = -1,
	eNoBreak,
    eEndStreamBreak,
    eColumnBreak,
    eHyphBreak,
    eCharBreak,
	eQuadBreak,
    eSpaceBreak,
    eHardHyphBreak,
    eParaBreak,              /* start of a paragraph */
    eStartColBreak
} eBreakType;

/* ==================================================================== */

struct DropCapInfo {
    DCPosition  dcPosition;			// dc positioning info from spec
    MicroPoint	dcMinX,				// left char extent from the dc origin
				dcMinY,				// top char extent from the dc origin
				dcMaxX,				// right char extent from the dc origin
				dcMaxY,				// bottom char extent from dc origin
				dcLineOrgChange,	// the recomputed line origin
				dcHChange,			// horz offset from recomputed orgin
				dcVMax;				// until a baseline exceeds this values 
									// readjust the line origins
									
			DropCapInfo() :
    			dcMinX( 0 ),
				dcMinY( 0 ),
				dcMaxX( 0 ),
				dcMaxY( 0 ),
				dcLineOrgChange( 0 ),
				dcHChange( 0 ),
				dcVMax( 0 ){}
} ;


Bool			DCCompute( DropCapInfo&,
						   TypeSpec&,			// para spec
						   TypeSpec&,			// character spec
						   MicroPoint,
						   MicroPoint,
						   UCS2 );


/* ==================================================================== */

class scLEADRefData {
public:
						scLEADRefData() : 
								fFlow( eRomanFlow ),
								fAboveLead( 0 ),
								fBelowLead( 0 ),
								fExternalLead( 0 ){}
					
						scLEADRefData( const scLEADRefData& );
						scLEADRefData( const scFlowDir& fd )					{ Init( fd ); }
						scLEADRefData( MicroPoint lead, const scFlowDir& fd )	{ ComputeAboveBelow( lead, fd ); }

    void				Init( const scFlowDir& fd )
							{
								fFlow = fd;
								fAboveLead = 0;
								fBelowLead = 0;
								fExternalLead = 0;
							}
				   	
	void				Set( MicroPoint lead );
	void				Set( MicroPoint lead, const scFlowDir& );
	void				Set( MicroPoint aboveLead, MicroPoint belowLead )		{ SetAboveLead( aboveLead ); SetBelowLead( belowLead ); }
	void				Set( MicroPoint aboveLead, MicroPoint belowLead, const scFlowDir& );

	MicroPoint			Compute( MicroPoint ptsize, MicroPoint lead, eFntBaseline baseline );
	void				ComputeAboveBelow( MicroPoint lead, const scFlowDir& );
	
	void				SetFlow( const scFlowDir& fd )	{ fFlow = fd; }
	const scFlowDir&	GetFlow( void ) const		{ return fFlow; }
	
	MicroPoint			GetLead( void ) const
							{
								return fAboveLead + fBelowLead + fExternalLead;
							}
	
	void				SetAboveLead( MicroPoint lead ) { fAboveLead = lead; }
	MicroPoint			GetAboveLead( void ) const		{ return fAboveLead; }
	
	void				SetBelowLead( MicroPoint lead ) { fBelowLead = lead; }
	MicroPoint			GetBelowLead( void ) const		{ return fBelowLead; }

	void				SetExternalSpace( MicroPoint space )
							{
								fExternalLead = space;
							}
	MicroPoint			GetExternalSpace( ) const
							{
								return fExternalLead;
							}
private:
	scFlowDir			fFlow;
							// the above and below are a function of the
							// ptsize and the baseline and external lead
	MicroPoint			fAboveLead;
	MicroPoint			fBelowLead;

	MicroPoint			fExternalLead;
};


/* ==================================================================== */


class DCState {
public:
	Bool				fDCSet;
	DropCapInfo			fDCInfo;
	scColumn*			fColumn;
	MicroPoint			fDCLastBaseline;
	
			DCState() : 
				fDCSet( false ), 
				fColumn( 0 ), 
				fDCLastBaseline( 0 ){} 

	void	SetColumn( scColumn *col ) { fColumn = col; }
	scColumn*	GetColumn( void ) const { return fColumn; }
};

/* ==================================================================== */
// redisplay information

class scRedisplayStoredLine {
public:
					scRedisplayStoredLine( int lines );
					~scRedisplayStoredLine();

					scRedisplayStoredLine() :
						fStoredData( 0 ),
						fStoredLines( 0 ),
						fUsingStoredData( false ),
						fData( 0 ),
						fNumItems( 0 ){}


	void			LineListInit( int lines = 200 );
	void 			LineListFini( void );

	void			SaveLineList( scColumn* );
	void			LineListChanges( scColumn*, const scXRect&, scRedispList* );

private:					
	scTextline*		fStoredData;		/* the buffer */
	ushort			fStoredLines;		/* # of lines the buffer can store */
	Bool			fUsingStoredData;   /* are we using the buffer or 
										* a larger temp buf 
										*/
	scXRect			fOrgExtents;
	scTextline*		fData;
	ushort			fNumItems;
};

/* ==================================================================== */

class scLINERefData {
public:
					scLINERefData()	:
						fOrg( 0, 0 ),
						fBaseline( 0 ),
						fCharRecs( 0 ),
						fStartCharOffset( 0 ),
						fCharCount( 0 ),
						fSpecRec( 0 ),
						fStartSpecRunOffset( 0 ),
						fSpecRunCount( 0 ),
						initialSpec_( 0 ),
						fMaxLeadSpec( 0 ),
#ifdef _RUBI_SUPPORT
						fAnnotations( 0 ),
#endif
						fLastLineLen( 0 ),
						fMeasure( 0 ),
						fComputedLen( 0 ),
						fRagSetting( eNoRag ),
						fStartAngle( 0 ),
						fEndAngle( 0 ),
						fColShapeType( eNoShape ),
						fBaselineJump( 0 ){}

					// zero out all the line values		
	void			Init( const scFlowDir& );
	void			xxInit( void );	
	
	Bool			IsHorizontal( void ) const			{ return fFlowDir.IsHorizontal(); }
	Bool			IsVertical( void ) const			{ return fFlowDir.IsVertical(); }

	void			SetFlowDir( const scFlowDir& fd )	{ fFlowDir = fd; }
	const scFlowDir& GetFlowDir( void ) const			{ return fFlowDir; }
	
	void			SetOrg( const scMuPoint& p )		{ fOrg = p; }
	scMuPoint&		GetOrg( void )						{ return fOrg; }
	
	void			SetBaseline( MicroPoint bl )		{ fBaseline = bl; }
	MicroPoint		GetBaseline( void ) const			{ return fBaseline; }
	
	void			SetCharacters( CharRecordP chrec )	{ fCharRecs = chrec; }
	CharRecordP		GetCharacters( void ) const			{ return fCharRecs; }
	
	void			SetStartCharOffset( long n )		{ fStartCharOffset = n; }
	long			GetStartCharOffset( void ) const	{ return fStartCharOffset; }
	
	void			SetCharCount( long n )				{ fCharCount = n; }
	long			GetCharCount( void ) const			{ return fCharCount; }

	long			GetEndCharOffset( void ) const		{ return fStartCharOffset + fCharCount; }
	
	void			SetSpecRec( scSpecRecord* sr )		{ fSpecRec = sr; }
	scSpecRecord*	GetSpecRec( void )					{ return fSpecRec; }
	
	void			SetStartSpecRunOffset( long n )		{ fStartSpecRunOffset = n; }
	long			GetStartSpecRunOffset( void )		{ return fStartSpecRunOffset; }
	
	void			SetSpecRunCount( long n )			{ fSpecRunCount = n; }
	long			GetSpecRunCount( void )				{ return fSpecRunCount; }
	
	void			SetMaxLeadSpec( TypeSpec p )		{ fMaxLeadSpec = p; }
	TypeSpec		GetMaxLeadSpec( void )				{ return fMaxLeadSpec; }
	
	void			SetInitialSpec( TypeSpec& p )		{ initialSpec_ = p; }
	TypeSpec		GetInitialSpec( void )				{ return initialSpec_; }

#ifdef _RUBI_SUPPORT
	void			SetAnnotations( scRubiArray* ra )	{ fAnnotations = ra; }
	scRubiArray*	GetAnnotations( void )				{ return fAnnotations; }
#endif

	void			SetInkExtents( const scXRect& r )	{ fInkExtents = r; }
	scXRect&		GetInkExtents( void )				{ return fInkExtents; }
	
	void			SetLogicalExtents( const scXRect& r )	{ fLogicalExtents = r; }
	scXRect&		GetLogicalExtents( void )				{ return fLogicalExtents; }
	
	void			SetLastLineLen( MicroPoint len )	{ fLastLineLen = len; }
	MicroPoint		GetLastLineLen( void )				{ return fLastLineLen; }
	
	void			SetMeasure( MicroPoint m )			{ fMeasure = m; }
	MicroPoint		GetMeasure( void )					{ return fMeasure; }
	
	void			SetComputedLen( MicroPoint len )	{ fComputedLen = len; }
	MicroPoint		GetComputedLen( void )				{ return fComputedLen; }
	
	void			SetRagSetting( eTSJust rag )		{ fRagSetting = rag; }
	eTSJust			GetRagSetting( void )				{ return fRagSetting; }
	
	void			SetInitialLead( const scLEADRefData& lrd )	{ fInitialLead = lrd; }
	scLEADRefData&	GetInitialLead( void )						{ return fInitialLead; }
	
	void			SetEndLead( const scLEADRefData p )	{ fEndLead = p; }
	scLEADRefData&	GetEndLead( void )					{ return fEndLead; }
	
	void			SetStartAngle( scAngle p )			{ fStartAngle = p; }
	scAngle			GetStartAngle( void )				{ return fStartAngle; }
	
	void			SetEndAngle( scAngle p )			{ fEndAngle = p; }
	scAngle			GetEndAngle( void )					{ return fEndAngle; }
	
	void			SetColShapeType( eColShapeType p )	{ fColShapeType = p; }
	eColShapeType	GetColShapeType( void )				{ return fColShapeType; }
	
	void			SetBaselineJump( MicroPoint p )		{ fBaselineJump = p; }
	MicroPoint		GetBaselineJump( void )				{ return fBaselineJump; }
	

	
					// origin of the line, this is orginally set in
					// COLGetStrip and may be further modified in
					// the LineBreaker
	scMuPoint		fOrg;
					// this is used in COLGetStrip
	MicroPoint		fBaseline;

					//
					// the backing store set in the para initializer
					//
	CharRecordP		fCharRecs;
					// set in the para reformatter
	long			fStartCharOffset;
					// set in the linebreaker
	long			fCharCount;

	   
	scSpecRecord*	fSpecRec;
					// set in the para reformatter
	long			fStartSpecRunOffset;
					// set in the linebreaker
	long			fSpecRunCount;

#ifdef _RUBI_SUPPORT
					// the character annotations ( rubi or hyper-text )
	scRubiArray*	fAnnotations;
#endif
	

	// somewhere along in here we would add the unpositioned glyphs
	// and the positioned glyphs hooks

	
					// set in the LineBreaker
	scXRect			fInkExtents;			// extents on ink

					// set before calling COLGetStrip and may be
					// modified in the LineBreaker ( with a spec change )
	scXRect			fLogicalExtents;		// extents of em square

					// length of last line, used for aesthetic rag
	MicroPoint		fLastLineLen;

					// measure of the line as reported by COLGetStrip,
					// may be modified in the LineBreaker
	MicroPoint		fMeasure;

					// actual length after line breaking
	MicroPoint		fComputedLen;

					// rag setting of the current line, used for positioning
					// in flex columns
	eTSJust			fRagSetting;	

					// the init lead state of the line
	scLEADRefData	fInitialLead;
	
					// the lead state after line breaking	
	scLEADRefData	fEndLead;

					// the letterspace of the line - set in the line breaker
	GlyphSize		fLetterSpace;

					// the start and end angle of the glyphs ( pseudo-obliquing )
	scAngle			fStartAngle;
	scAngle			fEndAngle;
	
	eColShapeType	fColShapeType;
	MicroPoint		fBaselineJump;

	scFlowDir		fFlowDir;

private:
					// the spec with the max lead on the line
					// !!!!!NOTE: this should only be set in PARAInit
					// or in the line breaker
	TypeSpec		initialSpec_;
	TypeSpec		fMaxLeadSpec;
};


/* ==================================================================== */
					
class scPARARefData {
public:
					scPARARefData();

	void			PARAInit( scContUnit*, 
							  const scFlowDir& );
	void			PARAFini( void );

	void			SaveData( void );
	void			RestoreData( void );

	Bool			ComposeLine( DCState& );
	
	Bool			AdjustLead( void ) const;

	void			SetColumn( scColumn* col )
					{
						column_ = col;
					}
	scColumn*		GetColumn() const
					{
						return column_;
					}
					
					// the line data before we call the line breaker
	scLINERefData	fInitialLine;
	
					// the information after we call the line breaker
	scLINERefData	fComposedLine;	


		// this resets the InitialLineData between calls to the linebreaker
	void			SetLineData( Bool );
	

	void			SetPrevPara( scContUnit* p )			{ fPrevPara = p; }
	scContUnit*		GetPrevPara( void ) 				{ return fPrevPara; }	

	void			SetPrevSpec( TypeSpec ps )			{ fPrevSpec = ps; }
	TypeSpec		GetPrevSpec( void )					{ return fPrevSpec; }	

	void			SetOrigin( const scMuPoint& org )	{ fOrigin = org; }
	scMuPoint&		GetOrigin( void )					{ return fOrigin; }
	
	void			SetPrevLead( const scLEADRefData& cl )	{ fPrevLead = cl; }
	scLEADRefData&	GetPrevLead( void )					{ return fPrevLead; }
	
	void			SetPara( scContUnit* p )				{ fPara = p; }
	scContUnit*		GetPara( void )						{ return fPara; }
	
	void			SetBreakParams( const scParaColBreak&	pb )
						{ 
							fBreakParams = pb; 
						}
	scParaColBreak&	GetBreakParams( void )
						{ 
							return fBreakParams; 
						}
	
	void			SetLinesHyphed( short lh )			{ fLinesHyphed = lh; }
	short			GetLinesHyphed( void )				{ return fLinesHyphed; }	

	void			SetStartChar( CharRecordP ch  )		{ fCharRecs = ch; }
	CharRecordP		GetStartChar( void )				{ return fCharRecs; }
	
	void			SetStartSpecRecord( scSpecRecord* spr )	{ fStartSpecRec = spr; }
	scSpecRecord*	GetStartSpecRecord( void )				{ return fStartSpecRec; }
                     
	void			SetSpecCount( long count )			{ fSpecCount = count; }
	long			GetSpecCount( void )				{ return fSpecCount; }

	scSpecRecord*	GetSpecRecord( void )				{ return fStartSpecRec + fSpecCount; }
	scSpecRecord*	GetSpecRecord( long offset );
	
#ifdef _RUBI_SUPPORT
	void			SetAnnotations( scRubiArray* annot ){ fAnnotations = annot; }
	scRubiArray*	GetAnnotations( void )				{ return fAnnotations; }	
#endif

	void			SetFlowDir( const scFlowDir& fd )	{ fInitialLine.SetFlowDir( fd ); }

//private:
					// the previous para's data
	scContUnit*		fPrevPara;
	TypeSpec		fPrevSpec;
	scMuPoint		fOrigin;
	scLEADRefData	fPrevLead;
 
  	scTextline*		fPrevline;                              
 	scTextline*		fTextline;   

				// the current paragraphs data
	scContUnit*		fPara;                                         
	
                // the flow direction we are currently working with
    scFlowDir		fFlowDir;

				// this is set only at the begginning of the paragraph
	eTSCompLang		fBreakLang;
	
				// this is state that is maintained regarding widow/orphan and
				// lines hyphenated
	scParaColBreak	fBreakParams;
	eBreakType		fBreakType;	
	                             
	    		// the number of columns this paragraph has snaked thru                       
	short			fColumnCount;
	            // lines before the column break        
	short			fLinesBefore;
				// lines after column break
	short			fLinesAfter;
	
				// total number of lines formatted in para	
	short			fLineNumber;
	
				// current number of lines hyphenated in a row
	short			fLinesHyphed;

	
					// backing store and glyph array stuff
	CharRecordP		fCharRecs;
	
	scSpecRecord*	fStartSpecRec;
	scSpecRecord*	fCurSpecRec;
	long			fSpecCount;
	
#ifdef _RUBI_SUPPORT
	scRubiArray*	fAnnotations;
#endif
	
	// somewhere along in here we would add the unpositioned glyphs
	// and the positioned glyphs hooks


private:
	scColumn*		column_;
	
};


/* ==================================================================== */	


class scCOLRefData {
public:
					scCOLRefData( scRedispList* redisp ) :
							fCol( 0 ),
							fFirstSpec( 0 ),
							fFirstlinePos( 0 ),
							fRgn( 0 ),
							fRedispList( redisp ) {}

	Bool			COLInit( scColumn*, scContUnit* );
	void			COLFini( Bool );

					// free lines marks as invalid and collect their damaged area
	void			FreeInvalidLines( void );

		// this stores the existing lines and collects and repainting 
		// of the lines necessary
	void			SaveLineList( void );

	void			PARAInit( scContUnit*, int, int, PrevParaData& );

	Bool			AllocGeometry( void );
	Bool			AllocLine( Bool leadRetry );

					// delete excess lines in the paragraph we are currently formatting
	void			PARADeleteExcessLines( void );

	Bool			FindNextCol( DCState& );

					// move from one column to the next resetting
					// the appropriate values
    void			Transition( void );

	Bool			ResetOrphan( Bool testGetStrip );

	void			SetActive( scColumn* col );
	scColumn*		GetActive( void ) const			{ return fCol; }
	
	void			SetFirstSpec( TypeSpec spec )	 	{ fFirstSpec = spec; }
	TypeSpec		GetFirstSpec( void ) const			{ return fFirstSpec; }

	void			SetFirstlinePos( MicroPoint mp ) 	{ fFirstlinePos = mp; }
	MicroPoint		GetFirstlinePos( void ) const		{ return fFirstlinePos; }
		
	void			SetRegion( HRgn* rgn )				{ fRgn = rgn; }
	HRgn*			GetRegion( void ) const				{ return fRgn; }

	scPARARefData	fPData;				// the current para ref data
	
	scColumn*		fCol;				// the column currently being reformatted
	scXRect			fLineDamage;		// lines marked as damaged
	TypeSpec		fFirstSpec;			// the first spec active in the column
	MicroPoint		fFirstlinePos;		// the position of the first line in the col
	scMuPoint		fPrevEnd;			// ending position of previous line
	scMuPoint		fSavedPrevEnd;		// end pos - saved
	HRgn*			fRgn;				// the active region
	HRgnHandle		fRgnH;

	scRedisplayStoredLine	fSavedLineState;
	scRedispList*			fRedispList;
};


/* ==================================================================== */


#endif /* _H_SCREFDAT */
