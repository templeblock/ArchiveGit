/****************************************************************************

	File:		sccallbk.h


	$Header: /Projects/Toolbox/ct/SCCALLBK.H 2     5/30/97 8:45a Wmanis $

	Contains:	The call backs to the client from the composition toolbox.

	Written by: Manis

	Copyright (c) 1989-1994 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
@doc

***************************************************************************/

#ifndef _H_SCSPECSY
#define _H_SCSPECSY

#ifdef SCMACINTOSH
#pragma once
#endif

#include "sctypes.h"

/*======================================================================*/
// The following are call backs that the application must support
// in order for the above selection messages to work properly.
//


// @CALLBACK Provides the Toolbox with the drawing
// context of the column, used for highlighting or drawing. 
//
status scIMPL_IMPORT	APPDrawContext(
							APPColumn		appCol,		// @parm <t APPColumn>
							const scColumn* col,		// @parm <c scColumn>
							APPDrwCtx&		drwctx );	// @parm <t APPDrwCtx>


// CALL BACK - this informs the composition toolbox whether it should recompose
// this column or not, the client may prevent recomposition of columns that are not
// visible, though keep in mind that if a subsequent column is visble we
// will have to recompose this column at some point in time.
Bool scIMPL_IMPORT		APPRecomposeColumn( APPColumn );

/* ==================================================================== */
// @CALLBACK Maps a pointer of a client object to an ID on disk. Typically
// a TypeSpec.

enum stDiskidClass {
	diskidUnknown,
	diskidColumn,
	diskidTypespec,
	diskidOther
};
			
long scIMPL_IMPORT		APPPointerToDiskID(
							APPCtxPtr,
							void*  clientObj,		// @parm Pointer to client object.
							stDiskidClass );		// class of object


// @CALLBACK  Maps a disk ID to a pointer. Typically a TypeSpec.

void* scIMPL_IMPORT		APPDiskIDToPointer(
							APPCtxPtr,
							long	diskID,		// @parm A value returned by <f APPPointerToDiskID>
												// that we want a valid pointer to now.
							stDiskidClass );	// class of object			   


/* ==================================================================== */
//
// called periodically by the Toolbox during actions
// that will take some time. If the call for an event returns 0,
// the action will be aborted and control will revert to application.
// The client can give the Toolbox and hint as to how much more time
// it can process for. The client can return a negative number as
// an indicator to get out fast.
//

	// this describes the current process type that the toolbox
	// is performing.
typedef enum scProcTypes {
	scDrawProc,		// toolbox is drawing
	scReformatProc	// toolbox is reformatting
} scProcType;


scTicks	scIMPL_IMPORT	APPEventAvail( scProcType );

/* ==================================================================== */

// @enum eSpecChange | When a TypeSpec is changed externally to the
// Toolbox, the Toolbox needs to be informed that a change has occurred
// so that reformatting may occur. In an effort to minimize the work
// the function <f SpecTaskCalculate> can calculate the minimum amount
// of work that needs to be done. (e.g. changing the color of a spec
// should only require repainting and not reformattion,
// SpecTaskCalculate(scColor) would return eSCRepaint ) With the return
// value of SpecTaskCalculate one can inform the Toolbox about the changed
// spec <f SCENG_ChangedTS>( ts, <t eSpecTask>, <c scRedispList> ) and
// get information about the minimal area to update.

typedef enum eSpecChanges {
	scLanguage,
	scFont,
	scColor,
	scRenderAttribute,
	scCharTransform,

	scPointSize,
	scSetSize,
	scHoblique,
	scVoblique,
	scRotation,

	scKern,
	scMarginKern,
	scTrack,
	scMinLsp,
	scOptLsp,
	scMaxLsp,

	scMinWsp,
	scOptWsp,
	scMaxWsp,

	scLead,
	scBaseline,
	scAboveLead,
	scBelowLead,

	scIndLines,
	scIndAmount,
	scIndDepth,
	scIndLeftBL,
	scIndRightBL,		scIndentExtra1, scIndentExtra2,
	scNoHyphLastWord,
	scColNoBreak,
	scKeepNext,
	scLinesBefore,
	scLinesAfter,		scWidowOrphanExtra1,scWidowOrphanExtra2,

	scRag,
	scForceJust,
	scRagPattern,
	scRagZone,
	scKernMargins,
	scHLeft,
	scHRight,
	scHLeftAmount,
	scHRightAmount, 	scRagExtra1, scRagExtra2,scHPuncExtra1,scHPuncExtra2,

	scHyphenation,
	scHyphChar,
	scHyphLines,
	scHyphExcep,
	scHyphMinSize,
	scPreHyphs,
	scPostHyphs,
	scHyphPropensity,
	scHyphCaps,
	scHyphAcros,		scHyphExtra1, scHyphExtra2,

	scDCShow,
	scDCChar,
	scDCptSize,
	scDCsetSize,
	scDChOffset,
	scDCvOffset,
	scDChBorder,
	scDCvBorder,
	scDCfont,
	scDCcolor,

	scMaxFillChars,
	scFillPos,
	scFillChar,
	scFillAlign,

	scMaxTabs,
	scTabPos,
	scTabAlign,
	scTabChar,
	scTabFillAlign,

	scMinMeasure,
	scRunAroundBorder,
	scFirstLine,

	scMaxValType
} eSpecChange;


// @CALLBACK Used to determine minimal work on a spec change.
// @rdesc <t eSpecTask>
eSpecTask			SpecTaskCalculate(
						 eSpecChange specChange );	// @parm <t eSpecChange>

/* ==================================================================== */
/* ==================================================================== */
/* ================== SPEC SUB-YSTEM CALL BACKS ====================== */
/* ==================================================================== */
/* ==================================================================== */

class scStyle;

// @CALLBACK  Gets the scStyle structure.

status	scIMPL_IMPORT	TSGetStyle( TypeSpec& ts,		// @parm <t TypeSpec>
									scStyle& style );	// @parm <c scStyle>

// @CALLBACK This call back is used to determine positioning of tabs.
// @ex Default value for tab positioning might be. |
//	tabInfo.xPos = ( xPos / defaultTabWidth + 1 ) * defaultTabWidth );
// 
status scIMPL_IMPORT	TSTabInfo(
							TypeSpec&	paraspec,	// paragraph spec
							TypeSpec&	ts,			// @parm <t TypeSpec>
							scTabInfo&	tabInfo,	// @parm <t scTabInfo>
							MicroPoint	xPos,		// @parm X position in column.
							MicroPoint	yPos,		// @parm Depth in column.
							long		lineNum );	// @parm Line num in para.

					// default wordspace
status scIMPL_IMPORT	TSfillCharInfo( TypeSpec&,
										UCS2&,
										eFCAlignment&,
										MicroPoint,
										MicroPoint,
										long ); 

					// default return false
Bool scIMPL_IMPORT	TSdropCap( TypeSpec&,		// para spec
							   TypeSpec&,		// character spec
							   DCPosition&,		// position struct
							   UCS2 );			// dropcap character
//line Bool			TSdropCap( TypeSpec, DCPosition& ) { return false; }


/* ==================================================================== */
/* ==================================================================== */ 
// COLUMN SPECIFICATIONS - 'CS'

	// By sending in the two specs the spec management system may generate
	// a value intelligently, either a hard coded value or parametrically
	// derived value using the pointsize of the type

// @CALLBACK  Position of first line in a column, default should
// be point size, this is not for use with dropcaps. Client
// may return any reasonable value and may use none, one or
// both of the parameters.
MicroPoint scIMPL_IMPORT	CSfirstLinePosition(
								APPColumn appcol,		// @parm <t APPColumn>
								TypeSpec  ts );			// @parm <t TypeSpec>

// @CALLBACK  Position of last line in a column,
// default should be zero since this will allow
// multiple columns with different pointsizes
// to bottom align.
MicroPoint scIMPL_IMPORT	CSlastLinePosition(
						APPColumn appcol,		// @parm <t APPColumn>
						TypeSpec  ts );			// @parm <t TypeSpec>

// @CALLBACK  Border to inset text from shape applied to
// column -- default is 0, the spec is the first
// encountered in the column.
inline MicroPoint	CSrunaroundBorder(
						APPColumn appcol,		// @parm <t APPColumn>
						TypeSpec  ts )			// @parm <t TypeSpec>
							{ return 0; }

/* ==================================================================== */
/* ================ FONT METRIC CALL BACKS ============================ */
/* ==================================================================== */

        /////////// DESIGN METRICS /////////////////////////////
		// DESIGN COORDINATES ARE THE RELATIVE UNIT SYSTEM DEFINED
		// IN scBaseRLUSystem

// @CALLBACK  Return the escapement of the glyph in design coordinates.
//
RLU scIMPL_IMPORT	FIgetRLUEscapement(
						const scFontRender& fr,		// @parm <t scFontRender>
						UCS2  ch );					// @parm Glyph.

RLU scIMPL_IMPORT	FIgetRLUEscapement( const scFontRender&,
										UCS2,
										RLU /*suggestedWidth*/ );

// @CALLBACK  Return the kerning value of the glyphs in design coordinates.
//
RLU scIMPL_IMPORT	FIgetRLUKern(
						const scFontRender& fr,		// @parm <t scFontRender>
						UCS2	ch1,				// @parm Glyph one.
						UCS2	ch2 );				// @parm Glyph two.

// @CALLBACK  Return the glyph ink box in design coordinates
//
scRLURect& scIMPL_IMPORT	FIgetRLUExtents(
								const scFontRender& fr,		// @parm <t scFontRender>
								UCS2				ch,		// @parm Glyph one.
								scRLURect&		inkBox );	// @parm <c scRLURect>

// @CALLBACK  Return the various font metrics in design coordinates
void scIMPL_IMPORT	FIgetRLUFontExtents(
						const scFontRender&		fontrender,	// @parm <t scFontRender>
						RLU&					capHite,	// @parm Cap height.
						RLU&					xHite,		// @parm Lower case x height.
						RLU&					ascenderHite,	// @parm Ascender height.
						RLU&					descenderDepth,	// @parm Descender height.
						scRLURect&				maxInkExt );	// @parm <c scRLURect> union of
																// ink extents of all glyphs in
																// font.


        /////////// DEVICE METRICS /////////////////////////////

// @CALLBACK  Return the escapement of the glyph in device coordinates
// ( transformed into toolbox coordinates ).
GlyphSize scIMPL_IMPORT		FIgetDEVEscapement(
								const scFontRender& fr,		// @parm <t scFontRender>
								UCS2  ch );					// @parm Glyph.


GlyphSize scIMPL_IMPORT		FIgetDEVEscapement( const scFontRender&,
												UCS2,
												GlyphSize /*suggestedWidth*/ );

// @CALLBACK  Return the kerning value of the glyphs in device coordinates
// ( transformed into toolbox coordinates ).

GlyphSize scIMPL_IMPORT		FIgetDEVKern(
						const scFontRender& fr,		// @parm <t scFontRender>
						UCS2	ch1,				// @parm Glyph one.
						UCS2	ch2 );				// @parm Glyph two.

// @CALLBACK  Return the glyph ink box in device coordinates
// ( transformed into toolbox coordinates ).
							//
scXRect& scIMPL_IMPORT		FIgetDEVExtents(
								const scFontRender& fr,		// @parm <t scFontRender>
								UCS2				ch,		// @parm Glyph one.
								scXRect&			inkBox );// @parm <c scXRect>

// @CALLBACK  Return the various font metrics in device coordinates
// ( transformed into toolbox coordinates ).

void scIMPL_IMPORT	FIgetDEVFontExtents(
						const scFontRender&		fontrender,	// @parm <t scFontRender>
						MicroPoint&				capHite,	// @parm Cap height.
						MicroPoint&				xHite,		// @parm Lower case x height.
						MicroPoint&				ascenderHite,	// @parm Ascender height.
						MicroPoint&				descenderDepth,	// @parm Descender height.
						scXRect&				maxInkExt );	// @parm <c scXRect> union of
																// ink extents of all glyphs in
																// font.
												

/* ==================================================================== */
/* =================   HYPHENATION SUB-SYSTEM ========================= */
/* ==================================================================== */

// @CALLBACK  Initializes the Hyphenation subs-sytem to the indicated langauge.
// Returns true if language properly inited.
//
Bool scIMPL_IMPORT	HYFLanguageInit( 
						APPLanguage lang );  		// @parm <t APPLanguage>

// @CALLBACK  Chars are in word, NULL terminated, return hyph values in hyfs, max
// len of either is 64. if word is hyphenated return true.
//
Bool scIMPL_IMPORT	HYFWord( 
						const UCS2*	theWord, 		// @parm The word.
						short* 		hyphArray );   	// @parm The hyphenation array.

/* ==================================================================== */
/* ==================================================================== */
/* =================== CHAR DRAWING CALLBACKS ========================= */
/* ==================================================================== */
/* ==================================================================== */


// @CALLBACK  Called before the start of drawing a line.
//
void scIMPL_IMPORT  APPDrawStartLine(
						APPDrwCtx		drwctx,		// @parm <t APPDrawCtx>
						MicroPoint		x,			// @parm X origin of line.
						MicroPoint		y,			// @parm Y origin of line.
						const scXRect&	inkext );	// @parm Max ink extents of line.

// @CALLBACK  Called n times ( for each style or full buffer ) between a APPDrawStartLine
// and an APPDrawEndLine.
// @xref <f SCCOL_Update>
void scIMPL_IMPORT	APPDrawString(
						APPDrwCtx			dc,	// @parm Pass thru context.
						const scGlyphArray* ga,	// @parm <t scGlyphArray> array.
						short				num,// @parm Number of glyphs in array.
						MicroPoint			x,	// @parm X origin of string.
						MicroPoint			y,	// @parm Y origin of string.
						const scGlyphInfo&	gi );// @parm <t scGlyphInfo>

// @CALLBACK  Called at the end  of drawing a line.
void scIMPL_IMPORT  APPDrawEndLine(
						APPDrwCtx dc );		// @parm <t APPDrwCtx> drawing context.


// @CALLBACK Used to draw hiliting rectangles.
void scIMPL_IMPORT	APPDrawRect(
						const scXRect& xorRect,	// @parm <c scXRect> to xor.
						APPDrwCtx	   dc,   	// @parm <t APPDrwCtx> drawing context.
						Bool		   sliverCursor );

void scIMPL_IMPORT	APPDrawRule( const scMuPoint&,
								 const scMuPoint&,
								 const scGlyphInfo&,
								 APPDrwCtx );


/* ==================================================================== */
/* ==================================================================== */
/* ==================================================================== */

class clField {
public:
    static  clField&	createField( scStream*, uint8 );

    virtual uint8		id() const = 0;
    virtual void		release() = 0;
    virtual void		content( stUnivString&, APPColumn, TypeSpec& ) = 0;
};


#endif /* _H_SCCALLBK */



