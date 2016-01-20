/****************************************************************************

	File:		SCCTYPE.C


	$Header: /Projects/Toolbox/ct/SCCTYPE.CPP 2     5/30/97 8:45a Wmanis $

	Contains:	Character types.

	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

***************************************************************************/

#include "sccharex.h"
#include "scctype.h"

unsigned short sc_CharType[258] = {
	0,									/* to let us use -1 as an index */
	0,									/* 0x00	0 " " */
	0,									/* 0x01	1 " " */
	0,									/* 0x02	2 " " */
	0,									/* 0x03	3 " " */
	0,									/* 0x04	4 " " */
	0,									/* 0x05	5 " " */
	0,									/* 0x06	6 " " */
	sc_ASCII|sc_SPACE,					/* 0x07	7 "indent space" */
	0,									/* 0x08	8 ""  */
	sc_ASCII|sc_SPACE,					/* 0x09   9 "\t" (HT) tab key */
	sc_ASCII|sc_SPACE,					/* 0x0a  10 "\n" (LF) line feed */
	sc_ASCII|sc_SPACE,					/* 0x0b  11 "\l"  (VT) vertical tab */
	0,									/* 0x0c  12 " " */
	sc_ASCII|sc_SPACE,					/* 0x0d  13 "\r" (CR) return key*/
	0,									/* 0x0e  14 " " */
	0,									/* 0x0f  15 " " */
	0,									/* 0x10  16 " " */
	0,									/* 0x11  17 " " */
	sc_ASCII,							/* 0x12  18 "paraEnd" */
	sc_ASCII|sc_SPACE,					/* 0x13  19 "quad center" */
	sc_ASCII|sc_SPACE,					/* 0x14  20 "quad left" */
	sc_ASCII|sc_SPACE,					/* 0x15  21 "quad right" */
	sc_ASCII|sc_SPACE,					/* 0x16  22 "quad just" */
	sc_ASCII|sc_SPACE,					/* 0x17  23 " " fix abs space */
	sc_ASCII|sc_SPACE,					/* 0x18  24 " " fix rel space */
	sc_ASCII|sc_SPACE,					/* 0x19  25 " " fill space */
	sc_ASCII,							/* 0x1a  26 " " no break hyphen */
	sc_ASCII|sc_SPACE,					/* 0x1b  27 " " discretionary hyphen */
	sc_ASCII|sc_SPACE,					/* 0x1c  28 " " figure space */
	sc_ASCII|sc_SPACE,					/* 0x1d  29 " " thin space */
	sc_ASCII|sc_SPACE,					/* 0x1e  30 " " en space */
	sc_ASCII|sc_SPACE,					/* 0x1f  31 " " em space */
	sc_ASCII|sc_SPACE,					// [0020] SPACE
	sc_ASCII|sc_PUNC,					// [0021] EXCLAMATION_MARK
	sc_ASCII|sc_PUNC,					// [0022] QUOTATION_MARK
	sc_ASCII|sc_SYMBOL,					// [0023] NUMBER_SIGN
	sc_ASCII|sc_SYMBOL,					// [0024] DOLLAR_SIGN
	sc_ASCII|sc_SYMBOL,					// [0025] PERCENT_SIGN
	sc_ASCII|sc_SYMBOL,					// [0026] AMPERSAND
	sc_ASCII|sc_PUNC,					// [0027] APOSTROPHE
	sc_ASCII|sc_SYMBOL,					// [0028] LEFT_PARENTHESIS
	sc_ASCII|sc_SYMBOL,					// [0029] RIGHT_PARENTHESIS
	sc_ASCII|sc_SYMBOL,					// [002A] ASTERISK
	sc_ASCII|sc_SYMBOL,					// [002B] PLUS_SIGN
	sc_ASCII|sc_PUNC,			 		// [002C] COMMA
	sc_ASCII|sc_PUNC,					// [002D] HYPHEN-MINUS
	sc_ASCII|sc_PUNC,					// [002E] FULL_STOP
	sc_ASCII|sc_SYMBOL,					// [002F] SOLIDUS
	sc_ASCII|sc_DIGIT,					// [0030] DIGIT_ZERO
	sc_ASCII|sc_DIGIT,					// [0031] DIGIT_ONE
	sc_ASCII|sc_DIGIT,					// [0032] DIGIT_TWO
	sc_ASCII|sc_DIGIT,					// [0033] DIGIT_THREE
	sc_ASCII|sc_DIGIT,					// [0034] DIGIT_FOUR
	sc_ASCII|sc_DIGIT,					// [0035] DIGIT_FIVE
	sc_ASCII|sc_DIGIT,					// [0036] DIGIT_SIX
	sc_ASCII|sc_DIGIT,					// [0037] DIGIT_SEVEN
	sc_ASCII|sc_DIGIT,					// [0038] DIGIT_EIGHT
	sc_ASCII|sc_DIGIT,					// [0039] DIGIT_NINE
	sc_ASCII|sc_PUNC,					// [003A] COLON
	sc_ASCII|sc_PUNC,					// [003B] SEMICOLON
	sc_ASCII|sc_SYMBOL,					// [003C] LESS-THAN_SIGN
	sc_ASCII|sc_SYMBOL,					// [003D] EQUALS_SIGN
	sc_ASCII|sc_SYMBOL,					// [003E] GREATER-THAN_SIGN
	sc_ASCII|sc_PUNC,					// [003F] QUESTION_MARK
	sc_ASCII|sc_SYMBOL,					// [0040] COMMERCIAL_AT
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [0041] LATIN_CAPITAL_LETTER_A
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [0042] LATIN_CAPITAL_LETTER_B
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [0043] LATIN_CAPITAL_LETTER_C
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [0044] LATIN_CAPITAL_LETTER_D
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [0045] LATIN_CAPITAL_LETTER_E
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [0046] LATIN_CAPITAL_LETTER_F
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [0047] LATIN_CAPITAL_LETTER_G
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [0048] LATIN_CAPITAL_LETTER_H
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [0049] LATIN_CAPITAL_LETTER_I
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [004A] LATIN_CAPITAL_LETTER_J
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [004B] LATIN_CAPITAL_LETTER_K
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [004C] LATIN_CAPITAL_LETTER_L
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [004D] LATIN_CAPITAL_LETTER_M
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [004E] LATIN_CAPITAL_LETTER_N
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [004F] LATIN_CAPITAL_LETTER_O
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [0050] LATIN_CAPITAL_LETTER_P
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [0051] LATIN_CAPITAL_LETTER_Q
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [0052] LATIN_CAPITAL_LETTER_R
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [0053] LATIN_CAPITAL_LETTER_S
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [0054] LATIN_CAPITAL_LETTER_T
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [0055] LATIN_CAPITAL_LETTER_U
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [0056] LATIN_CAPITAL_LETTER_V
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [0057] LATIN_CAPITAL_LETTER_W
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [0058] LATIN_CAPITAL_LETTER_X
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [0059] LATIN_CAPITAL_LETTER_Y
	sc_ASCII|sc_ALPHA|sc_UPCASE,		// [005A] LATIN_CAPITAL_LETTER_Z
	sc_ASCII|sc_SYMBOL,					// [005B] LEFT_SQUARE_BRACKET
	sc_ASCII|sc_SYMBOL,					// [005C] REVERSE_SOLIDUS
	sc_ASCII|sc_SYMBOL,					// [005D] RIGHT_SQUARE_BRACKET
	sc_ASCII|sc_SYMBOL,					// [005E] CIRCUMFLEX_ACCENT
	sc_ASCII|sc_SYMBOL,					// [005F] LOW_LINE
	sc_ASCII|sc_ACCENT,					// [0060] GRAVE_ACCENT
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [0061] LATIN_SMALL_LETTER_A
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [0062] LATIN_SMALL_LETTER_B
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [0063] LATIN_SMALL_LETTER_C
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [0064] LATIN_SMALL_LETTER_D
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [0065] LATIN_SMALL_LETTER_E
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [0066] LATIN_SMALL_LETTER_F
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [0067] LATIN_SMALL_LETTER_G
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [0068] LATIN_SMALL_LETTER_H
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [0069] LATIN_SMALL_LETTER_I
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [006A] LATIN_SMALL_LETTER_J
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [006B] LATIN_SMALL_LETTER_K
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [006C] LATIN_SMALL_LETTER_L
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [006D] LATIN_SMALL_LETTER_M
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [006E] LATIN_SMALL_LETTER_N
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [006F] LATIN_SMALL_LETTER_O
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [0070] LATIN_SMALL_LETTER_P
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [0071] LATIN_SMALL_LETTER_Q
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [0072] LATIN_SMALL_LETTER_R
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [0073] LATIN_SMALL_LETTER_S
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [0074] LATIN_SMALL_LETTER_T
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [0075] LATIN_SMALL_LETTER_U
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [0076] LATIN_SMALL_LETTER_V
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [0077] LATIN_SMALL_LETTER_W
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [0078] LATIN_SMALL_LETTER_X
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [0079] LATIN_SMALL_LETTER_Y
	sc_ASCII|sc_ALPHA|sc_LOCASE,		// [007A] LATIN_SMALL_LETTER_Z
	sc_ASCII|sc_SYMBOL,					// [007B] LEFT_CURLY_BRACKET
	sc_ASCII|sc_SYMBOL,					// [007C] VERTICAL_LINE
	sc_ASCII|sc_SYMBOL,					// [007D] RIGHT_CURLY_BRACKET
	sc_ASCII|sc_SYMBOL,					// [007E] TILDE
	
	0,								// 0x7f 127
	0,								// 0x80 128
	0,								// 0x81 129
	0,								// 0x82 130
	0,								// 0x83 131
	0,								// 0x84 132
	0,								// 0x85 133
	0,								// 0x86 134
	0,								// 0x87 135
	0,								// 0x88 136
	0,								// 0x89 137
	0,								// 0x8a 138
	0,								// 0x8b 139
	0,								// 0x8c 140
	0,								// 0x8d 141
	0,								// 0x8e 142
	0,								// 0x8f 143
	0,								// 0x90 144
	0,								// 0x91 145
	0,								// 0x92 146
	0,								// 0x93 147
	0,								// 0x94 148
	0,								// 0x95 149
	0,								// 0x96 150
	0,								// 0x97 151
	0,								// 0x98 152
	0,								// 0x99 153
	0,								// 0x9a 154
	0,								// 0x9b 155
	0,								// 0x9c 156
	0,								// 0x9d 157
	0,								// 0x9e 158
	0,								// 0x9f 159
	
	sc_SPACE,						// [00A0] NO-BREAK_SPACE
	sc_SYMBOL,						// [00A1] INVERTED_EXCLAMATION_MARK
	sc_SYMBOL,						// [00A2] CENT_SIGN
	sc_SYMBOL,						// [00A3] POUND_SIGN
	sc_SYMBOL,						// [00A4] CURRENCY_SIGN
	sc_SYMBOL,						// [00A5] YEN_SIGN
	sc_SYMBOL,						// [00A6] BROKEN_BAR
	sc_SYMBOL,						// [00A7] SECTION_SIGN
	sc_SYMBOL,						// [00A8] DIAERESIS
	sc_SYMBOL,						// [00A9] COPYRIGHT_SIGN
	sc_SYMBOL,						// [00AA] FEMININE_ORDINAL_INDICATOR
	sc_SYMBOL,						// [00AB] LEFT-POINTING_DOUBLE_ANGLE_QUOTATION_MARK
	sc_SYMBOL,						// [00AC] NOT_SIGN
	sc_SYMBOL,						// [00AD] SOFT_HYPHEN
	sc_SYMBOL,						// [00AE] REGISTERED_SIGN
	sc_SYMBOL,						// [00AF] MACRON
	sc_SYMBOL,						// [00B0] DEGREE_SIGN
	sc_SYMBOL,						// [00B1] PLUS-MINUS_SIGN
	sc_SYMBOL,						// [00B2] SUPERSCRIPT_TWO
	sc_SYMBOL,						// [00B3] SUPERSCRIPT_THREE
	sc_SYMBOL,						// [00B4] ACUTE_ACCENT
	sc_SYMBOL,						// [00B5] MICRO_SIGN
	sc_SYMBOL,						// [00B6] PILCROW_SIGN
	sc_SYMBOL,						// [00B7] MIDDLE_DOT
	sc_SYMBOL,						// [00B8] CEDILLA
	sc_SYMBOL,						// [00B9] SUPERSCRIPT_ONE
	sc_SYMBOL,						// [00BA] MASCULINE_ORDINAL_INDICATOR
	sc_SYMBOL,						// [00BB] RIGHT-POINTING_DOUBLE_ANGLE_QUOTATION_MARK
	sc_SYMBOL,						// [00BC] VULGAR_FRACTION_ONE_QUARTER
	sc_SYMBOL,						// [00BD] VULGAR_FRACTION_ONE_HALF
	sc_SYMBOL,						// [00BE] VULGAR_FRACTION_THREE_QUARTERS
	sc_SYMBOL,						// [00BF] INVERTED_QUESTION_MARK
	
	sc_ALPHA|sc_UPCASE,				// [00C0] LATIN_CAPITAL_LETTER_A_WITH_GRAVE
	sc_ALPHA|sc_UPCASE,				// [00C1] LATIN_CAPITAL_LETTER_A_WITH_ACUTE
	sc_ALPHA|sc_UPCASE,				// [00C2] LATIN_CAPITAL_LETTER_A_WITH_CIRCUMFLEX
	sc_ALPHA|sc_UPCASE,				// [00C3] LATIN_CAPITAL_LETTER_A_WITH_TILDE
	sc_ALPHA|sc_UPCASE,				// [00C4] LATIN_CAPITAL_LETTER_A_WITH_DIAERESIS
	sc_ALPHA|sc_UPCASE,				// [00C5] LATIN_CAPITAL_LETTER_A_WITH_RING_ABOVE
	sc_ALPHA|sc_UPCASE,				// [00C6] LATIN_CAPITAL_LIGATURE_AE
	sc_ALPHA|sc_UPCASE,				// [00C7] LATIN_CAPITAL_LETTER_C_WITH_CEDILLA
	sc_ALPHA|sc_UPCASE,				// [00C8] LATIN_CAPITAL_LETTER_E_WITH_GRAVE
	sc_ALPHA|sc_UPCASE, 			// [00C9] LATIN_CAPITAL_LETTER_E_WITH_ACUTE
	sc_ALPHA|sc_UPCASE,				// [00CA] LATIN_CAPITAL_LETTER_E_WITH_CIRCUMFLEX
	sc_ALPHA|sc_UPCASE,				// [00CB] LATIN_CAPITAL_LETTER_E_WITH_DIAERESIS
	sc_ALPHA|sc_UPCASE,				// [00CC] LATIN_CAPITAL_LETTER_I_WITH_GRAVE
	sc_ALPHA|sc_UPCASE,				// [00CD] LATIN_CAPITAL_LETTER_I_WITH_ACUTE
	sc_ALPHA|sc_UPCASE,				// [00CE] LATIN_CAPITAL_LETTER_I_WITH_CIRCUMFLEX
	sc_ALPHA|sc_UPCASE,				// [00CF] LATIN_CAPITAL_LETTER_I_WITH_DIAERESIS
	sc_ALPHA|sc_UPCASE, 			// [00D0] LATIN_CAPITAL_LETTER_ETH_(Icelandic)
	sc_ALPHA|sc_UPCASE, 			// [00D1] LATIN_CAPITAL_LETTER_N_WITH_TILDE
	sc_ALPHA|sc_UPCASE,				// [00D2] LATIN_CAPITAL_LETTER_O_WITH_GRAVE
	sc_ALPHA|sc_UPCASE,				// [00D3] LATIN_CAPITAL_LETTER_O_WITH_ACUTE
	sc_ALPHA|sc_UPCASE,				// [00D4] LATIN_CAPITAL_LETTER_O_WITH_CIRCUMFLEX
	sc_ALPHA|sc_UPCASE,				// [00D5] LATIN_CAPITAL_LETTER_O_WITH_TILDE
	sc_ALPHA|sc_UPCASE,				// [00D6] LATIN_CAPITAL_LETTER_O_WITH_DIAERESIS
	
	sc_SYMBOL,						// [00D7] MULTIPLICATION_SIGN
	
	sc_ALPHA|sc_UPCASE,				// [00D8] LATIN_CAPITAL_LETTER_O_WITH_STROKE
	sc_ALPHA|sc_UPCASE,				// [00D9] LATIN_CAPITAL_LETTER_U_WITH_GRAVE
	sc_ALPHA|sc_UPCASE,				// [00DA] LATIN_CAPITAL_LETTER_U_WITH_ACUTE
	sc_ALPHA|sc_UPCASE,				// [00DB] LATIN_CAPITAL_LETTER_U_WITH_CIRCUMFLEX
	sc_ALPHA|sc_UPCASE,				// [00DC] LATIN_CAPITAL_LETTER_U_WITH_DIAERESIS
	sc_ALPHA|sc_UPCASE,				// [00DD] LATIN_CAPITAL_LETTER_Y_WITH_ACUTE
	sc_ALPHA|sc_UPCASE,				// [00DE] LATIN_CAPITAL_LETTER_THORN_(Icelandic)
	
	sc_ALPHA|sc_LOCASE,				// [00DF] LATIN_SMALL_LETTER_SHARP_S_(German)
	
	sc_ALPHA|sc_LOCASE,				// [00E0] LATIN_SMALL_LETTER_A_WITH_GRAVE
	sc_ALPHA|sc_LOCASE, 			// [00E1] LATIN_SMALL_LETTER_A_WITH_ACUTE
	sc_ALPHA|sc_LOCASE, 			// [00E2] LATIN_SMALL_LETTER_A_WITH_CIRCUMFLEX
	sc_ALPHA|sc_LOCASE, 			// [00E3] LATIN_SMALL_LETTER_A_WITH_TILDE
	sc_ALPHA|sc_LOCASE,				// [00E4] LATIN_SMALL_LETTER_A_WITH_DIAERESIS
	sc_ALPHA|sc_LOCASE,				// [00E5] LATIN_SMALL_LETTER_A_WITH_RING_ABOVE
	sc_ALPHA|sc_LOCASE,				// [00E6] LATIN_SMALL_LIGATURE_AE
	sc_ALPHA|sc_LOCASE,				// [00E7] LATIN_SMALL_LETTER_C_WITH_CEDILLA
	sc_ALPHA|sc_LOCASE,				// [00E8] LATIN_SMALL_LETTER_E_WITH_GRAVE
	sc_ALPHA|sc_LOCASE,				// [00E9] LATIN_SMALL_LETTER_E_WITH_ACUTE
	sc_ALPHA|sc_LOCASE,				// [00EA] LATIN_SMALL_LETTER_E_WITH_CIRCUMFLEX
	sc_ALPHA|sc_LOCASE,				// [00EB] LATIN_SMALL_LETTER_E_WITH_DIAERESIS
	sc_ALPHA|sc_LOCASE,				// [00EC] LATIN_SMALL_LETTER_I_WITH_GRAVE
	sc_ALPHA|sc_LOCASE,				// [00ED] LATIN_SMALL_LETTER_I_WITH_ACUTE
	sc_ALPHA|sc_LOCASE,				// [00EE] LATIN_SMALL_LETTER_I_WITH_CIRCUMFLEX
	sc_ALPHA|sc_LOCASE,				// [00EF] LATIN_SMALL_LETTER_I_WITH_DIAERESIS
	sc_ALPHA|sc_LOCASE,				// [00F0] LATIN_SMALL_LETTER_ETH_(Icelandic)
	sc_ALPHA|sc_LOCASE,				// [00F1] LATIN_SMALL_LETTER_N_WITH_TILDE
	sc_ALPHA|sc_LOCASE,				// [00F2] LATIN_SMALL_LETTER_O_WITH_GRAVE
	sc_ALPHA|sc_LOCASE,				// [00F3] LATIN_SMALL_LETTER_O_WITH_ACUTE
	sc_ALPHA|sc_LOCASE,				// [00F4] LATIN_SMALL_LETTER_O_WITH_CIRCUMFLEX
	sc_ALPHA|sc_LOCASE,				// [00F5] LATIN_SMALL_LETTER_O_WITH_TILDE
	sc_ALPHA|sc_LOCASE,				// [00F6] LATIN_SMALL_LETTER_O_WITH_DIAERESIS
	
	sc_SYMBOL,						// [00F7] DIVISION_SIGN
	
	sc_ALPHA|sc_LOCASE,				// [00F8] LATIN_SMALL_LETTER_O_WITH_STROKE
	sc_ALPHA|sc_LOCASE,				// [00F9] LATIN_SMALL_LETTER_U_WITH_GRAVE
	sc_ALPHA|sc_LOCASE,				// [00FA] LATIN_SMALL_LETTER_U_WITH_ACUTE
	sc_ALPHA|sc_LOCASE,				// [00FB] LATIN_SMALL_LETTER_U_WITH_CIRCUMFLEX
	sc_ALPHA|sc_LOCASE,				// [00FC] LATIN_SMALL_LETTER_U_WITH_DIAERESIS
	sc_ALPHA|sc_LOCASE,				// [00FD] LATIN_SMALL_LETTER_Y_WITH_ACUTE
	sc_ALPHA|sc_LOCASE,				// [00FE] LATIN_SMALL_LETTER_THORN_(Icelandic)
	
	sc_ALPHA|sc_LOCASE,				// [00FF] LATIN_SMALL_LETTER_Y_WITH_DIAERESIS
	
	0
};


/* ==================================================================== */

static UCS2	CTChangeCase( UCS2 );

/* ==================================================================== */
/* return the lower case of a character */

UCS2 CTToLower( UCS2 ch )
{
	register ushort test;
	
	if ( ch < 256 ) {
		test = sc_CharType[ch+1];
		if ( test & sc_UPCASE ) {
			if ( ch != 0xDF )
				return (UCS2)(ch + 0x20);
		}
	}
	else
		; /* case may not be significant */
		
	return ch;	
}

/* ==================================================================== */

UCS2 CTToUpper( register UCS2	ch )
{
	register ushort test;
	
	if ( ch < 256 ) {
		test = sc_CharType[ch+1];	
		if ( test & sc_LOCASE ) {
			if ( ch != 0xFF )
				return (UCS2)(ch - 0x20);
		}
	}
	else
		; /* case may not be significant */
			
	return ch;	
}

/* ==================================================================== */

UCS2 CTToggleCase( register UCS2	ch )
{
	register ushort test;
	
	if ( ch < 256 ) {
		test = sc_CharType[ch+1];	
		if ( test & sc_LOCASE ) {
			if ( ch != 0xFF )
				return (UCS2)(ch - 0x20);
		}
		else if ( test & sc_UPCASE ) {
			if ( ch != 0xDF )
				return (UCS2)(ch + 0x20);
		}
	}
	else
		; /* case may not be significant */
	
	return ch;	
}

/* ==================================================================== */
/* determines whether or not to store just the character and its flags or
 * the character, its flags and its escapement
 */

Bool CTStoreAll( UCS2	ch )
{
	switch ( ch ) {
		default:	
			return false;
		case scFixAbsSpace:
		case scFixRelSpace:
			return true;
	}
}

/* ==================================================================== */
