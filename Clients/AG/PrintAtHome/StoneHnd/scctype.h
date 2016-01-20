/***************************************************************************
Contains:	Character types.
****************************************************************************/
 
#pragma once

#include "sctypes.h"

#define sc_ASCII			0x0001
#define sc_SPACE			0x0002	
#define sc_PUNC 			0x0004
#define sc_DIGIT			0x0008
#define sc_ALPHA			0x0010
#define sc_ACCENT			0x0020
#define sc_SYMBOL			0x0040
#define sc_LOCASE			0x0080
#define sc_UPCASE			0x0100
#define sc_LIGATR			0x0200

extern unsigned short sc_CharType[];

/* for now we assume everything above 255 is alpha, with release of kanji and other versions this will change */
#define CTIsAlpha(ch)		( (ch) < 256 ? ( sc_CharType[(ch)+1] & (sc_ALPHA) ) : true )
#define CTIsSelectable(ch)	( (ch) < 256 ? ( sc_CharType[(ch)+1] & (sc_ALPHA|sc_DIGIT) ) : true )
#define CTIsDigit(ch)		( (ch) < 256 ? ( sc_CharType[(ch)+1] & (sc_DIGIT) ) : false )
#define CTIsPunc(ch)		( (ch) < 256 ? ( sc_CharType[(ch)+1] & (sc_PUNC) ) : false )
#define CTIsUpperCase(ch)	( (ch) < 256 ? ( sc_CharType[(ch)+1] & (sc_UPCASE) ) : false )
#define CTIsLowerCase(ch)	( (ch) < 256 ? ( sc_CharType[(ch)+1] & (sc_LOCASE) ) : false )
#define CTIsSpace(ch)		( (ch) < 256 ? ( sc_CharType[(ch)+1] & (sc_SPACE) ) : false )
#define CTIsSymbol(ch)		( (ch) < 256 ? ( sc_CharType[(ch)+1] & (sc_SYMBOL) ) : false )
#define CTIsVisible(ch) 	( (ch) < 256 ? ( sc_CharType[(ch)+1] & (sc_ALPHA|sc_DIGIT|sc_ACCENT|sc_PUNC|sc_SYMBOL) ) : true )

BOOL		CTStoreAll( UCS2 );
BOOL		CTIsFracBar( UCS2 );
UCS2		CTToLower( UCS2 );
UCS2		CTToUpper( UCS2 );
UCS2		CTToggleCase( UCS2 );
