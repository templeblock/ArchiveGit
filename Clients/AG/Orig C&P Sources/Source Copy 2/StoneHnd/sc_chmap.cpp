/****************************************************************

	File:		SC-CharMap.c

	$Header: /Projects/Toolbox/ct/SC_CHMAP.CPP 6	 5/30/97 8:45a Wmanis $
	
	Contains:	Character mapping between client and toolbox.

	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

****************************************************************/

/**********************************************************************

	to turn off any fo the functions in thismodule define one or
	more of the following values int SCAPPTypes.h, they will turn
	off the appropriate function.

	noCMctToAPP
	noCMappToCT
	noCMmakeKeyRecordTwo
	noCMcontent

 **********************************************************************/

#include "sccharex.h"

#define CTL( ch )	( (ch) - '@' )

/****************************************************************************/
/* this provides character mapping between Mac-keyboard/application
 * to the Composition Toolboxt
 */
#ifndef noCMappToCT

UCS2 CMappToCT( UCS2	ch )
{
	switch ( ch ) {
		case 0x0D:	return scParaSplit; 			/* mac enter */
		case 0x08:	return scBackSpace; 			/* mac delete */
		case 0x09:	return scTabSpace;				/* mac tab */
		case 0x0a:	return scHardReturn;				/* mac return */

		default:	return (UCS2)ch;
	}
}

#endif /* noCMappToCT */

/****************************************************************************/
/* provides Compositon Toolboxt to application mapping,
 * used pre AppDrawString for rationalize character mapping,
 * used to control characters passed thru, typically used
 * to control things like show invisibles, may be used for
 * other types of character conversion depending on output device
 */

#ifndef noCMctToAPP

UCS2 CMctToAPP( UCS2 ch )
{
	switch ( ch ) {
		default:
			return ch;
		case scNoBreakHyph:
			return '-';

#if 1		
		case scTabSpace:
			return 0;
		case scParaEnd:
			return 0;
		case scEndStream:
			return 0;
		case scHardReturn:
			return 0;
#else
		case scTabSpace:
			return 0x00bb;
		case scParaEnd:
			return 0x00b6;
		case scEndStream:
			return 0x00a5;
		case scHardReturn:
			return 'H';
#endif			
			
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
	}
}

#endif /* noCMctToAPP */

/****************************************************************************/
/* defines whether keyboard input changes model & selection or selection
 * only. Called from within Composition Toolboxt prior to keyboard input
 * to determine what is about to happen
 */

#ifndef noCMcontent

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

#endif /* noCMcontent */

/****************************************************************************/
/* mapping on reading input buffer or file */

#ifndef noCMinputMap

UCS2 CMinputMap( ushort ch )
{
	return ch;
}

#endif /* noCMinputMap */

/****************************************************************************/

#ifndef noCMmakeKeyRecordTwo

void CMmakeKeyRecordTwo( scKeyRecord&		keyRecord,
						 UCS2				keyCode,
						 GlyphSize			val,
						 TypeSpec			spec,
						 Bool				restoreSelection,
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

#endif /* noCMmakeKeyRecordTwo */

/****************************************************************************/
