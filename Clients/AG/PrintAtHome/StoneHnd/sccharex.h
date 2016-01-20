/****************************************************************
Contains:	character exchange from toolbox to outside world
**********************************************************************/

#pragma once

#include "sctypes.h"

#define scLeftArrow 	((UCS2)1)
#define scRightArrow	((UCS2)2)
#define scUpArrow		((UCS2)3)
#define scDownArrow 	((UCS2)4)
#define scParaSplit 	((UCS2)5)
#define scBackSpace 	((UCS2)6)			// delete character backward
#define scForwardDelete ((UCS2)7)		// delete character forward

// the following are characters actually stored in the stream and do have
// real character codes
#define scEndStream 		0x0000
/* 0x0001 is taken */
/* 0x0002 is taken */
/* 0x0003 is taken */
/* 0x0004 is taken */
/* 0x0005 is taken */
/* 0x0006 is taken */
#define scEmptySpace		0x0008	/* a horizontal move that is meaningless to the user */
#define scTabSpace			0x0009	/* part of the mac character set */
#define scHardReturn		0x000a	/* part of the mac character set */
#define scVertTab			0x000b
#define scField 			0x000c	/* field character */
/* 0x000d is not taken */
/* 0x000e is not taken */
#define scRulePH			0x000f
/* 0x0010 is not taken */
#define scParaStart 		0x0011	/* this has no meaning outside of a report to the client of the cursor position */
#define scParaEnd			0x0012	/* para break */
#define scQuadCenter		0x0013
#define scQuadLeft			0x0014
#define scQuadRight 		0x0015
#define scQuadJustify		0x0016
#define scFixAbsSpace		0x0017	/* absolute fixed space */
#define scFixRelSpace		0x0018	/* relative fixed space stored in rlu's */
#define scFillSpace 		0x0019
#define scNoBreakHyph		0x001a
#define scDiscHyphen		0x001b
#define scFigureSpace		0x001c
#define scThinSpace 		0x001d
#define scEnSpace			0x001e
#define scEmSpace			0x001f
#define scWordSpace 		0x0020
#define scRomanWordSpace	scWordSpace 	// ' ' or 0x20 or 32
#define scKanjiWordSpace	0x8140
#define scBreakingHyphen	'-'
#define scNoBreakSpace		0x00a0	/* part of the mac character set */
#define scEnDash			0x00d0
#define scEmDash			0x00d1

inline BOOL IsBreakingCharacter( UCS2 ch ) 
	{ return ch == scBreakingHyphen || ch == scEnDash || ch == scEmDash; }

UCS2	CMinputMap( ushort );	/* from APP to Stonehand - on file importing */
UCS2	CMctToAPP( UCS2 );		/* from Stonehand to APP */
UCS2	CMappToCT( UCS2 );		/* from APP to Stonehand */
int 	CMcontent( UCS2 );		/* is keystroke a selection change or a real input of content */

void		CMmakeKeyRecordTwo( scKeyRecord&,
								UCS2,
								GlyphSize,
								TypeSpec, 
								BOOL,
								scStreamLocation& );
