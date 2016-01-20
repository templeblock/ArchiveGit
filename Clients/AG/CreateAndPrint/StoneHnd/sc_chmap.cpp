#include "sccharex.h"

// this provides character mapping between keyboard/application to the Composition Toolboxt
UCS2 CMappToCT( UCS2 ch )
{
	switch ( ch ) {
		case 0x0D:	return scParaSplit;		/* enter */
		case 0x08:	return scBackSpace;		/* delete */
		case 0x09:	return scTabSpace;		/* tab */
		case 0x0a:	return scHardReturn;	/* return */

		default:	return (UCS2)ch;
	}
}

/* provides Compositon Toolboxt to application mapping,
 * used pre AppDrawString for rationalize character mapping,
 * used to control characters passed thru, typically used
 * to control things like show invisibles, may be used for
 * other types of character conversion depending on output device
 */
UCS2 CMctToAPP( UCS2 ch )
{
	switch ( ch ) {
		case scNoBreakHyph:
			return '-';
		case scTabSpace:
			return 0; // return 0x00bb;
		case scParaEnd:
			return 0; // return 0x00b6;
		case scEndStream:
			return 0; // return 0x00a5;
		case scHardReturn:
			return 0;
		case scEmSpace:
		case scEnSpace:
		case scFigureSpace:
		case scThinSpace:
		case scFixRelSpace:
		case scFixAbsSpace:
		case scFillSpace:
		case scVertTab:
//		case scNoBreakSpace:
		case scQuadCenter:
		case scQuadLeft:
		case scQuadRight:
		case scQuadJustify:
		case scEmptySpace:
			return ' ';
		default:
			return ch;
	}
}

/* defines whether keyboard input changes model & selection or selection
 * only. Called from within Composition Toolboxt prior to keyboard input
 * to determine what is about to happen
 */
int CMcontent( UCS2 ch )
{
	switch ( ch ) {
		case scBackSpace:
		case scForwardDelete:
			return -1;

		default:
			return 1;

		case scLeftArrow:
		case scRightArrow:
		case scUpArrow:
		case scDownArrow:
			return false;
	}
}

// mapping on reading input buffer or file
UCS2 CMinputMap( ushort ch )
{
	return ch;
}

/****************************************************************************/
void CMmakeKeyRecordTwo( scKeyRecord&		keyRecord,
						 UCS2				keyCode,
						 GlyphSize			val,
						 TypeSpec			spec,
						 BOOL				restoreSelection,
						 scStreamLocation&	mark )
{
	keyRecord.keycode() 		= keyCode;
	keyRecord.replacedchar()	= 0;
	keyRecord.escapement()		= val;
	keyRecord.spec()			= spec;
	keyRecord.noop()			= false;
	keyRecord.restoreselect()	= restoreSelection;
	keyRecord.mark()			= mark;
}
