#pragma once

#include "sctypes.h"

// The following are call backs that the application must support
// in order for the above selection messages to work properly.

// @CALLBACK Provides the Toolbox with the drawing context of the column, used for highlighting or drawing. 
status APPDrawContext(
					APPColumn		pTextCol, 	// @parm <t APPColumn>
					const scColumn* col,		// @parm <c scColumn>
					APPDrwCtx&		pTextCtx );	// @parm <t APPDrwCtx>

enum stDiskidClass {
	diskidUnknown,
	diskidColumn,
	diskidTypespec,
	diskidOther
};
			
// @CALLBACK Maps a pointer of a client object to an ID on disk. Typically a TypeSpec.
long APPPointerToDiskID(
					APPCtxPtr,
					void*  clientObj,	// @parm Pointer to client object.
					stDiskidClass );	// class of object

// @CALLBACK  Maps a disk ID to a pointer. Typically a TypeSpec.
APPSpec* APPDiskIDToPointer(
					APPCtxPtr,
					long	diskID, 	// @parm A value returned by <f APPPointerToDiskID>
										// that we want a valid pointer to now.
					stDiskidClass );	// class of object			   

// this describes the current process type that the toolbox is performing.
enum scProcType {
	scDrawProc, 	// toolbox is drawing
	scReformatProc	// toolbox is reformatting
};

// called periodically by the Toolbox during actions
// that will take some time. If the call for an event returns 0,
// the action will be aborted and control will revert to application.
// The client can give the Toolbox and hint as to how much more time
// it can process for. The client can return a negative number as
// an indicator to get out fast.
scTicks APPEventAvail(
					scProcType proctype );

/* ================== SPEC SUB-YSTEM CALL BACKS ====================== */

class scStyle;

// @CALLBACK  Gets the scStyle structure.
status TSGetStyle(
					TypeSpec& ts,		// @parm <t TypeSpec>
					scStyle& style );	// @parm <c scStyle>

// @CALLBACK This call back is used to determine positioning of tabs.
// @ex Default value for tab positioning might be. |
//	tabInfo.xPos = ( xPos / defaultTabWidth + 1 ) * defaultTabWidth );
status TSTabInfo(
					TypeSpec&	paraspec,	// paragraph spec
					TypeSpec&	ts, 		// @parm <t TypeSpec>
					scTabInfo&	tabInfo,	// @parm <t scTabInfo>
					MicroPoint	xPos,		// @parm X position in column.
					MicroPoint	yPos,		// @parm Depth in column.
					long		lineNum );	// @parm Line num in para.

// default wordspace
status TSfillCharInfo(
					TypeSpec& ts,
					UCS2& ch,
					eFCAlignment&,
					MicroPoint,
					MicroPoint,
					long ); 

// COLUMN SPECIFICATIONS - 'CS'
// By sending in the two specs the spec management system may generate
// a value intelligently, either a hard coded value or parametrically
// derived value using the pointsize of the type

// @CALLBACK  Position of first line in a column, default should be point size.
// Client may return any reasonable value and may use none, one or
// both of the parameters.
MicroPoint CSfirstLinePosition(
					APPColumn pTextCol,		// @parm <t APPColumn>
					TypeSpec  ts ); 		// @parm <t TypeSpec>

// @CALLBACK  Position of last line in a column,
// default should be zero since this will allow
// multiple columns with different pointsizes
// to bottom align.
MicroPoint CSlastLinePosition(
					APPColumn pTextCol,		// @parm <t APPColumn>
					TypeSpec  ts ); 		// @parm <t TypeSpec>

// @CALLBACK  Border to inset text from shape applied to
// column -- default is 0, the spec is the first
// encountered in the column.
MicroPoint CSrunaroundBorder(
					APPColumn pTextCol,		// @parm <t APPColumn>
					TypeSpec  ts );			// @parm <t TypeSpec>

/* ================ FONT METRIC CALL BACKS ============================ */

// @CALLBACK  Return the escapement of the glyph in device coordinates
// ( transformed into toolbox coordinates ).
GlyphSize FIgetDEVEscapement(
					const TypeSpec& fr, 	// @parm <t TypeSpec>
					UCS2 ch ); 				// @parm Glyph.

// @CALLBACK  Return the kerning value of the glyphs in device coordinates
// ( transformed into toolbox coordinates ).
GlyphSize FIgetDEVKern(
					const TypeSpec& fr,		// @parm <t TypeSpec>
					UCS2 ch1,				// @parm Glyph one.
					UCS2 ch2 );				// @parm Glyph two.

// @CALLBACK  Return the various font metrics in device coordinates
// ( transformed into toolbox coordinates ).
void FIgetDEVFontExtents(
					const TypeSpec& fontrender, // @parm <t TypeSpec>
					scXRect& maxInkExt );		// @parm <c scXRect> union of
												// ink extents of all glyphs in font.
												
/* =================== CHAR DRAWING CALLBACKS ========================= */

// @CALLBACK  Called before the start of drawing a line.
void APPDrawStartLine(
					APPDrwCtx		pTextCtx, 	// @parm <t APPDrawCtx>
					MicroPoint		x,			// @parm X origin of line.
					MicroPoint		y,			// @parm Y origin of line.
					const scXRect&	inkext );	// @parm Max ink extents of line.

// @CALLBACK  Called n times ( for each style or full buffer ) between a APPDrawStartLine
// and an APPDrawEndLine.
// @xref <f SCCOL_Update>
void APPDrawString(
					APPDrwCtx			pTextCtx,	// @parm Pass thru context.
					const scGlyphArray* ga,			// @parm <t scGlyphArray> array.
					short				num,		// @parm Number of glyphs in array.
					MicroPoint			x,			// @parm X origin of string.
					MicroPoint			y,			// @parm Y origin of string.
					const scGlyphInfo&	gi );		// @parm <t scGlyphInfo>

// @CALLBACK  Called at the end  of drawing a line.
void APPDrawEndLine(
					APPDrwCtx pTextCtx); 	// @parm <t APPDrwCtx> drawing context.


void APPDrawRule(
					const scMuPoint&,
					const scMuPoint&,
					const scGlyphInfo&,
					APPDrwCtx pTextCtx);

class clField
{
public:
	virtual uint8	id() const = 0;
	virtual void	release() = 0;
	virtual void	content(USTR& str, APPColumn pTextCol, TypeSpec& ts) = 0;
};

clField& APPCreateField(scStream* str, uint8 id);
