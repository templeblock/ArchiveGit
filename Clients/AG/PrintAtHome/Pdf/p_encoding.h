/*---------------------------------------------------------------------------*
 |              PDFlib - A library for generating PDF on the fly             |
 +---------------------------------------------------------------------------+
 | Copyright (c) 1997-2002 PDFlib GmbH and Thomas Merz. All rights reserved. |
 +---------------------------------------------------------------------------+
 |    This software is NOT in the public domain.  It can be used under two   |
 |    substantially different licensing terms:                               |
 |                                                                           |
 |    The commercial license is available for a fee, and allows you to       |
 |    - ship a commercial product based on PDFlib                            |
 |    - implement commercial Web services with PDFlib                        |
 |    - distribute (free or commercial) software when the source code is     |
 |      not made available                                                   |
 |    Details can be found in the file PDFlib-license.pdf.                   |
 |                                                                           |
 |    The "Aladdin Free Public License" doesn't require any license fee,     |
 |    and allows you to                                                      |
 |    - develop and distribute PDFlib-based software for which the complete  |
 |      source code is made available                                        |
 |    - redistribute PDFlib non-commercially under certain conditions        |
 |    - redistribute PDFlib on digital media for a fee if the complete       |
 |      contents of the media are freely redistributable                     |
 |    Details can be found in the file aladdin-license.pdf.                  |
 |                                                                           |
 |    These conditions extend to ports to other programming languages.       |
 |    PDFlib is distributed with no warranty of any kind. Commercial users,  |
 |    however, will receive warranty and support statements in writing.      |
 *---------------------------------------------------------------------------*/

/* $Id: p_encoding.h,v 1.2.2.1 2002/01/07 18:26:29 tm Exp $
 *
 * WinAnsi, MacRoman, and EBCDIC standard encodings
 *
 */

#ifndef P_ENCODING_H
#define P_ENCODING_H

static unsigned short pdf_unicodes_winansi[256] =
{
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
    0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
    0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x2022,
    0x20AC, 0x2022, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
    0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0x2022, 0x017D, 0x2022,
    0x2022, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
    0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x2022, 0x017E, 0x0178,
    0x0020, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
    0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x002D, 0x00AE, 0x00AF,
    0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x03BC, 0x00B6, 0x00B7,
    0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
    0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
    0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
    0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
    0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
    0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
    0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
    0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
    0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF
};

static pdf_encodingvector pdf_core_enc_winansi =
{
    "winansi",
    "WinAnsiEncoding",
    pdf_unicodes_winansi,

    {
	NULL,		/* 0x00		0 */
	NULL,		/* 0x01		1 */
	NULL,		/* 0x02		2 */
	NULL,		/* 0x03		3 */
	NULL,		/* 0x04		4 */
	NULL,		/* 0x05		5 */
	NULL,		/* 0x06		6 */
	NULL,		/* 0x07		7 */
	NULL,		/* 0x08		8 */
	NULL,		/* 0x09		9 */
	NULL,		/* 0x0A		10 */
	NULL,		/* 0x0B		11 */
	NULL,		/* 0x0C		12 */
	NULL,		/* 0x0D		13 */
	NULL,		/* 0x0E		14 */
	NULL,		/* 0x0F		15 */
	NULL,		/* 0x10		16 */
	NULL,		/* 0x11		17 */
	NULL,		/* 0x12		18 */
	NULL,		/* 0x13		19 */
	NULL,		/* 0x14		20 */
	NULL,		/* 0x15		21 */
	NULL,		/* 0x16		22 */
	NULL,		/* 0x17		23 */
	NULL,		/* 0x18		24 */
	NULL,		/* 0x19		25 */
	NULL,		/* 0x1A		26 */
	NULL,		/* 0x1B		27 */
	NULL,		/* 0x1C		28 */
	NULL,		/* 0x1D		29 */
	NULL,		/* 0x1E		30 */
	NULL,		/* 0x1F		31 */
	"space",	/* 0x20		32 */
	"exclam",	/* 0x21		33 */
	"quotedbl",	/* 0x22		34 */
	"numbersign",	/* 0x23		35 */
	"dollar",	/* 0x24		36 */
	"percent",	/* 0x25		37 */
	"ampersand",	/* 0x26		38 */
	"quotesingle",	/* 0x27		39 */
	"parenleft",	/* 0x28		40 */
	"parenright",	/* 0x29		41 */
	"asterisk",	/* 0x2A		42 */
	"plus",		/* 0x2B		43 */
	"comma",	/* 0x2C		44 */
	"hyphen",	/* 0x2D		45 */
	"period",	/* 0x2E		46 */
	"slash",	/* 0x2F		47 */
	"zero",		/* 0x30		48 */
	"one",		/* 0x31		49 */
	"two",		/* 0x32		50 */
	"three",	/* 0x33		51 */
	"four",		/* 0x34		52 */
	"five",		/* 0x35		53 */
	"six",		/* 0x36		54 */
	"seven",	/* 0x37		55 */
	"eight",	/* 0x38		56 */
	"nine",		/* 0x39		57 */
	"colon",	/* 0x3A		58 */
	"semicolon",	/* 0x3B		59 */
	"less",		/* 0x3C		60 */
	"equal",	/* 0x3D		61 */
	"greater",	/* 0x3E		62 */
	"question",	/* 0x3F		63 */
	"at",		/* 0x40		64 */
	"A",		/* 0x41		65 */
	"B",		/* 0x42		66 */
	"C",		/* 0x43		67 */
	"D",		/* 0x44		68 */
	"E",		/* 0x45		69 */
	"F",		/* 0x46		70 */
	"G",		/* 0x47		71 */
	"H",		/* 0x48		72 */
	"I",		/* 0x49		73 */
	"J",		/* 0x4A		74 */
	"K",		/* 0x4B		75 */
	"L",		/* 0x4C		76 */
	"M",		/* 0x4D		77 */
	"N",		/* 0x4E		78 */
	"O",		/* 0x4F		79 */
	"P",		/* 0x50		80 */
	"Q",		/* 0x51		81 */
	"R",		/* 0x52		82 */
	"S",		/* 0x53		83 */
	"T",		/* 0x54		84 */
	"U",		/* 0x55		85 */
	"V",		/* 0x56		86 */
	"W",		/* 0x57		87 */
	"X",		/* 0x58		88 */
	"Y",		/* 0x59		89 */
	"Z",		/* 0x5A		90 */
	"bracketleft",	/* 0x5B		91 */
	"backslash",	/* 0x5C		92 */
	"bracketright",	/* 0x5D		93 */
	"asciicircum",	/* 0x5E		94 */
	"underscore",	/* 0x5F		95 */
	"grave",	/* 0x60		96 */
	"a",		/* 0x61		97 */
	"b",		/* 0x62		98 */
	"c",		/* 0x63		99 */
	"d",		/* 0x64		100 */
	"e",		/* 0x65		101 */
	"f",		/* 0x66		102 */
	"g",		/* 0x67		103 */
	"h",		/* 0x68		104 */
	"i",		/* 0x69		105 */
	"j",		/* 0x6A		106 */
	"k",		/* 0x6B		107 */
	"l",		/* 0x6C		108 */
	"m",		/* 0x6D		109 */
	"n",		/* 0x6E		110 */
	"o",		/* 0x6F		111 */
	"p",		/* 0x70		112 */
	"q",		/* 0x71		113 */
	"r",		/* 0x72		114 */
	"s",		/* 0x73		115 */
	"t",		/* 0x74		116 */
	"u",		/* 0x75		117 */
	"v",		/* 0x76		118 */
	"w",		/* 0x77		119 */
	"x",		/* 0x78		120 */
	"y",		/* 0x79		121 */
	"z",		/* 0x7A		122 */
	"braceleft",	/* 0x7B		123 */
	"bar",		/* 0x7C		124 */
	"braceright",	/* 0x7D		125 */
	"asciitilde",	/* 0x7E		126 */
	"bullet",	/* 0x7F		127 */
	"Euro",		/* 0x80		128 */	/* new in PDF 1.3 */
	"bullet",	/* 0x81		129 */
	"quotesinglbase",/* 0x82	130 */
	"florin",	/* 0x83		131 */
	"quotedblbase",	/* 0x84		132 */
	"ellipsis",	/* 0x85		133 */
	"dagger",	/* 0x86		134 */
	"daggerdbl",	/* 0x87		135 */
	"circumflex",	/* 0x88		136 */
	"perthousand",	/* 0x89		137 */
	"Scaron",	/* 0x8A		138 */
	"guilsinglleft",/* 0x8B		139 */
	"OE",		/* 0x8C		140 */
	"bullet",	/* 0x8D		141 */
	"Zcaron",	/* 0x8E		142 */	/* new in PDF 1.3 */
	"bullet",	/* 0x8F		143 */
	"bullet",	/* 0x90		144 */
	"quoteleft",	/* 0x91		145 */
	"quoteright",	/* 0x92		146 */
	"quotedblleft",	/* 0x93		147 */
	"quotedblright",/* 0x94		148 */
	"bullet",	/* 0x95		149 */
	"endash",	/* 0x96		150 */
	"emdash",	/* 0x97		151 */
	"tilde",	/* 0x98		152 */
	"trademark",	/* 0x99		153 */
	"scaron",	/* 0x9A		154 */
	"guilsinglright",/* 0x9B	155 */
	"oe",		/* 0x9C		156 */
	"bullet",	/* 0x9D		157 */
	"zcaron",	/* 0x9E		158 */	/* new in PDF 1.3 */
	"Ydieresis",	/* 0x9F		159 */
	"space",	/* 0xA0		160 */
	"exclamdown",	/* 0xA1		161 */
	"cent",		/* 0xA2		162 */
	"sterling",	/* 0xA3		163 */
	"currency",	/* 0xA4		164 */
	"yen",		/* 0xA5		165 */
	"brokenbar",	/* 0xA6		166 */
	"section",	/* 0xA7		167 */
	"dieresis",	/* 0xA8		168 */
	"copyright",	/* 0xA9		169 */
	"ordfeminine",	/* 0xAA		170 */
	"guillemotleft",/* 0xAB		171 */
	"logicalnot",	/* 0xAC		172 */
	"hyphen",	/* 0xAD		173 */
	"registered",	/* 0xAE		174 */
	"macron",	/* 0xAF		175 */
	"degree",	/* 0xB0		176 */
	"plusminus",	/* 0xB1		177 */
	"twosuperior",	/* 0xB2		178 */
	"threesuperior",/* 0xB3		179 */
	"acute",	/* 0xB4		180 */
	"mu",		/* 0xB5		181 */
	"paragraph",	/* 0xB6		182 */
	"periodcentered",/* 0xB7	183 */
	"cedilla",	/* 0xB8		184 */
	"onesuperior",	/* 0xB9		185 */
	"ordmasculine",	/* 0xBA		186 */
	"guillemotright",/* 0xBB	187 */
	"onequarter",	/* 0xBC		188 */
	"onehalf",	/* 0xBD		189 */
	"threequarters",/* 0xBE		190 */
	"questiondown",	/* 0xBF		191 */
	"Agrave",	/* 0xC0		192 */
	"Aacute",	/* 0xC1		193 */
	"Acircumflex",	/* 0xC2		194 */
	"Atilde",	/* 0xC3		195 */
	"Adieresis",	/* 0xC4		196 */
	"Aring",	/* 0xC5		197 */
	"AE",		/* 0xC6		198 */
	"Ccedilla",	/* 0xC7		199 */
	"Egrave",	/* 0xC8		200 */
	"Eacute",	/* 0xC9		201 */
	"Ecircumflex",	/* 0xCA		202 */
	"Edieresis",	/* 0xCB		203 */
	"Igrave",	/* 0xCC		204 */
	"Iacute",	/* 0xCD		205 */
	"Icircumflex",	/* 0xCE		206 */
	"Idieresis",	/* 0xCF		207 */
	"Eth",		/* 0xD0		208 */
	"Ntilde",	/* 0xD1		209 */
	"Ograve",	/* 0xD2		210 */
	"Oacute",	/* 0xD3		211 */
	"Ocircumflex",	/* 0xD4		212 */
	"Otilde",	/* 0xD5		213 */
	"Odieresis",	/* 0xD6		214 */
	"multiply",	/* 0xD7		215 */
	"Oslash",	/* 0xD8		216 */
	"Ugrave",	/* 0xD9		217 */
	"Uacute",	/* 0xDA		218 */
	"Ucircumflex",	/* 0xDB		219 */
	"Udieresis",	/* 0xDC		220 */
	"Yacute",	/* 0xDD		221 */
	"Thorn",	/* 0xDE		222 */
	"germandbls",	/* 0xDF		223 */
	"agrave",	/* 0xE0		224 */
	"aacute",	/* 0xE1		225 */
	"acircumflex",	/* 0xE2		226 */
	"atilde",	/* 0xE3		227 */
	"adieresis",	/* 0xE4		228 */
	"aring",	/* 0xE5		229 */
	"ae",		/* 0xE6		230 */
	"ccedilla",	/* 0xE7		231 */
	"egrave",	/* 0xE8		232 */
	"eacute",	/* 0xE9		233 */
	"ecircumflex",	/* 0xEA		234 */
	"edieresis",	/* 0xEB		235 */
	"igrave",	/* 0xEC		236 */
	"iacute",	/* 0xED		237 */
	"icircumflex",	/* 0xEE		238 */
	"idieresis",	/* 0xEF		239 */
	"eth",		/* 0xF0		240 */
	"ntilde",	/* 0xF1		241 */
	"ograve",	/* 0xF2		242 */
	"oacute",	/* 0xF3		243 */
	"ocircumflex",	/* 0xF4		244 */
	"otilde",	/* 0xF5		245 */
	"odieresis",	/* 0xF6		246 */
	"divide",	/* 0xF7		247 */
	"oslash",	/* 0xF8		248 */
	"ugrave",	/* 0xF9		249 */
	"uacute",	/* 0xFA		250 */
	"ucircumflex",	/* 0xFB		251 */
	"udieresis",	/* 0xFC		252 */
	"yacute",	/* 0xFD		253 */
	"thorn",	/* 0xFE		254 */
	"ydieresis"	/* 0xFF		255 */
    }
};

/*
 * ---------------------------------------------------------------
 * MacRoman encoding vector
 * ---------------------------------------------------------------
 */

static unsigned short pdf_unicodes_macroman[256] =
{
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
    0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
    0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x0000,
    0x00C4, 0x00C5, 0x00C7, 0x00C9, 0x00D1, 0x00D6, 0x00DC, 0x00E1,
    0x00E0, 0x00E2, 0x00E4, 0x00E3, 0x00E5, 0x00E7, 0x00E9, 0x00E8,
    0x00EA, 0x00EB, 0x00ED, 0x00EC, 0x00EE, 0x00EF, 0x00F1, 0x00F3,
    0x00F2, 0x00F4, 0x00F6, 0x00F5, 0x00FA, 0x00F9, 0x00FB, 0x00FC,
    0x2020, 0x00B0, 0x00A2, 0x00A3, 0x00A7, 0x2022, 0x00B6, 0x00DF,
    0x00AE, 0x00A9, 0x2122, 0x00B4, 0x00A8, 0x0000, 0x00C6, 0x00D8,
    0x0000, 0x00B1, 0x0000, 0x0000, 0x00A5, 0x03BC, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x00AA, 0x00BA, 0x0000, 0x00E6, 0x00F8,
    0x00BF, 0x00A1, 0x00AC, 0x0000, 0x0192, 0x0000, 0x0000, 0x00AB,
    0x00BB, 0x2026, 0x0020, 0x00C0, 0x00C3, 0x00D5, 0x0152, 0x0153,
    0x2013, 0x2014, 0x201C, 0x201D, 0x2018, 0x2019, 0x00F7, 0x0000,
    0x00FF, 0x0178, 0x2044, 0x00A4, 0x2039, 0x203A, 0xFB01, 0xFB02,
    0x2021, 0x00B7, 0x201A, 0x201E, 0x2030, 0x00C2, 0x00CA, 0x00C1,
    0x00CB, 0x00C8, 0x00CD, 0x00CE, 0x00CF, 0x00CC, 0x00D3, 0x00D4,
    0x0000, 0x00D2, 0x00DA, 0x00DB, 0x00D9, 0x0131, 0x02C6, 0x02DC,
    0x00AF, 0x02D8, 0x02D9, 0x02DA, 0x00B8, 0x02DD, 0x02DB, 0x02C7
};

static pdf_encodingvector pdf_core_enc_macroman =
{
    "macroman",
    "MacRomanEncoding",
    pdf_unicodes_macroman,

    {
	NULL,		/* 0x00		0 */
	NULL,		/* 0x01		1 */
	NULL,		/* 0x02		2 */
	NULL,		/* 0x03		3 */
	NULL,		/* 0x04		4 */
	NULL,		/* 0x05		5 */
	NULL,		/* 0x06		6 */
	NULL,		/* 0x07		7 */
	NULL,		/* 0x08		8 */
	NULL,		/* 0x09		9 */
	NULL,		/* 0x0A		10 */
	NULL,		/* 0x0B		11 */
	NULL,		/* 0x0C		12 */
	NULL,		/* 0x0D		13 */
	NULL,		/* 0x0E		14 */
	NULL,		/* 0x0F		15 */
	NULL,		/* 0x10		16 */
	NULL,		/* 0x11		17 */
	NULL,		/* 0x12		18 */
	NULL,		/* 0x13		19 */
	NULL,		/* 0x14		20 */
	NULL,		/* 0x15		21 */
	NULL,		/* 0x16		22 */
	NULL,		/* 0x17		23 */
	NULL,		/* 0x18		24 */
	NULL,		/* 0x19		25 */
	NULL,		/* 0x1A		26 */
	NULL,		/* 0x1B		27 */
	NULL,		/* 0x1C		28 */
	NULL,		/* 0x1D		29 */
	NULL,		/* 0x1E		30 */
	NULL,		/* 0x1F		31 */
	"space",	/* 0x20		32 */
	"exclam",	/* 0x21		33 */
	"quotedbl",	/* 0x22		34 */
	"numbersign",	/* 0x23		35 */
	"dollar",	/* 0x24		36 */
	"percent",	/* 0x25		37 */
	"ampersand",	/* 0x26		38 */
	"quotesingle",	/* 0x27		39 */
	"parenleft",	/* 0x28		40 */
	"parenright",	/* 0x29		41 */
	"asterisk",	/* 0x2A		42 */
	"plus",		/* 0x2B		43 */
	"comma",	/* 0x2C		44 */
	"hyphen",	/* 0x2D		45 */
	"period",	/* 0x2E		46 */
	"slash",	/* 0x2F		47 */
	"zero",		/* 0x30		48 */
	"one",		/* 0x31		49 */
	"two",		/* 0x32		50 */
	"three",	/* 0x33		51 */
	"four",		/* 0x34		52 */
	"five",		/* 0x35		53 */
	"six",		/* 0x36		54 */
	"seven",	/* 0x37		55 */
	"eight",	/* 0x38		56 */
	"nine",		/* 0x39		57 */
	"colon",	/* 0x3A		58 */
	"semicolon",	/* 0x3B		59 */
	"less",		/* 0x3C		60 */
	"equal",	/* 0x3D		61 */
	"greater",	/* 0x3E		62 */
	"question",	/* 0x3F		63 */
	"at",		/* 0x40		64 */
	"A",		/* 0x41		65 */
	"B",		/* 0x42		66 */
	"C",		/* 0x43		67 */
	"D",		/* 0x44		68 */
	"E",		/* 0x45		69 */
	"F",		/* 0x46		70 */
	"G",		/* 0x47		71 */
	"H",		/* 0x48		72 */
	"I",		/* 0x49		73 */
	"J",		/* 0x4A		74 */
	"K",		/* 0x4B		75 */
	"L",		/* 0x4C		76 */
	"M",		/* 0x4D		77 */
	"N",		/* 0x4E		78 */
	"O",		/* 0x4F		79 */
	"P",		/* 0x50		80 */
	"Q",		/* 0x51		81 */
	"R",		/* 0x52		82 */
	"S",		/* 0x53		83 */
	"T",		/* 0x54		84 */
	"U",		/* 0x55		85 */
	"V",		/* 0x56		86 */
	"W",		/* 0x57		87 */
	"X",		/* 0x58		88 */
	"Y",		/* 0x59		89 */
	"Z",		/* 0x5A		90 */
	"bracketleft",	/* 0x5B		91 */
	"backslash",	/* 0x5C		92 */
	"bracketright",	/* 0x5D		93 */
	"asciicircum",	/* 0x5E		94 */
	"underscore",	/* 0x5F		95 */
	"grave",	/* 0x60		96 */
	"a",		/* 0x61		97 */
	"b",		/* 0x62		98 */
	"c",		/* 0x63		99 */
	"d",		/* 0x64		100 */
	"e",		/* 0x65		101 */
	"f",		/* 0x66		102 */
	"g",		/* 0x67		103 */
	"h",		/* 0x68		104 */
	"i",		/* 0x69		105 */
	"j",		/* 0x6A		106 */
	"k",		/* 0x6B		107 */
	"l",		/* 0x6C		108 */
	"m",		/* 0x6D		109 */
	"n",		/* 0x6E		110 */
	"o",		/* 0x6F		111 */
	"p",		/* 0x70		112 */
	"q",		/* 0x71		113 */
	"r",		/* 0x72		114 */
	"s",		/* 0x73		115 */
	"t",		/* 0x74		116 */
	"u",		/* 0x75		117 */
	"v",		/* 0x76		118 */
	"w",		/* 0x77		119 */
	"x",		/* 0x78		120 */
	"y",		/* 0x79		121 */
	"z",		/* 0x7A		122 */
	"braceleft",	/* 0x7B		123 */
	"bar",		/* 0x7C		124 */
	"braceright",	/* 0x7D		125 */
	"asciitilde",	/* 0x7E		126 */
	NULL,		/* 0x7F		127 */
	"Adieresis",	/* 0x80		128 */
	"Aring",	/* 0x81		129 */
	"Ccedilla",	/* 0x82		130 */
	"Eacute",	/* 0x83		131 */
	"Ntilde",	/* 0x84		132 */
	"Odieresis",	/* 0x85		133 */
	"Udieresis",	/* 0x86		134 */
	"aacute",	/* 0x87		135 */
	"agrave",	/* 0x88		136 */
	"acircumflex",	/* 0x89		137 */
	"adieresis",	/* 0x8A		138 */
	"atilde",	/* 0x8B		139 */
	"aring",	/* 0x8C		140 */
	"ccedilla",	/* 0x8D		141 */
	"eacute",	/* 0x8E		142 */
	"egrave",	/* 0x8F		143 */
	"ecircumflex",	/* 0x90		144 */
	"edieresis",	/* 0x91		145 */
	"iacute",	/* 0x92		146 */
	"igrave",	/* 0x93		147 */
	"icircumflex",	/* 0x94		148 */
	"idieresis",	/* 0x95		149 */
	"ntilde",	/* 0x96		150 */
	"oacute",	/* 0x97		151 */
	"ograve",	/* 0x98		152 */
	"ocircumflex",	/* 0x99		153 */
	"odieresis",	/* 0x9A		154 */
	"otilde",	/* 0x9B		155 */
	"uacute",	/* 0x9C		156 */
	"ugrave",	/* 0x9D		157 */
	"ucircumflex",	/* 0x9E		158 */
	"udieresis",	/* 0x9F		159 */
	"dagger",	/* 0xA0		160 */
	"degree",	/* 0xA1		161 */
	"cent",		/* 0xA2		162 */
	"sterling",	/* 0xA3		163 */
	"section",	/* 0xA4		164 */
	"bullet",	/* 0xA5		165 */
	"paragraph",	/* 0xA6		166 */
	"germandbls",	/* 0xA7		167 */
	"registered",	/* 0xA8		168 */
	"copyright",	/* 0xA9		169 */
	"trademark",	/* 0xAA		170 */
	"acute",	/* 0xAB		171 */
	"dieresis",	/* 0xAC		172 */
	NULL,		/* 0xAD		173 */
	"AE",		/* 0xAE		174 */
	"Oslash",	/* 0xAF		175 */
	NULL,		/* 0xB0		176 */
	"plusminus",	/* 0xB1		177 */
	NULL,		/* 0xB2		178 */
	NULL,		/* 0xB3		179 */
	"yen",		/* 0xB4		180 */
	"mu",		/* 0xB5		181 */
	NULL,		/* 0xB6		182 */
	NULL,		/* 0xB7		183 */
	NULL,		/* 0xB8		184 */
	NULL,		/* 0xB9		185 */
	NULL,		/* 0xBA		186 */
	"ordfeminine",	/* 0xBB		187 */
	"ordmasculine",	/* 0xBC		188 */
	NULL,		/* 0xBD		189 */
	"ae",		/* 0xBE		190 */
	"oslash",	/* 0xBF		191 */
	"questiondown",	/* 0xC0		192 */
	"exclamdown",	/* 0xC1		193 */
	"logicalnot",	/* 0xC2		194 */
	NULL,		/* 0xC3		195 */
	"florin",	/* 0xC4		196 */
	NULL,		/* 0xC5		197 */
	NULL,		/* 0xC6		198 */
	"guillemotleft",/* 0xC7		199 */
	"guillemotright",/* 0xC8	200 */
	"ellipsis",	/* 0xC9		201 */
	"space",	/* 0xCA		202 */
	"Agrave",	/* 0xCB		203 */
	"Atilde",	/* 0xCC		204 */
	"Otilde",	/* 0xCD		205 */
	"OE",		/* 0xCE		206 */
	"oe",		/* 0xCF		207 */
	"endash",	/* 0xD0		208 */
	"emdash",	/* 0xD1		209 */
	"quotedblleft",	/* 0xD2		210 */
	"quotedblright",/* 0xD3		211 */
	"quoteleft",	/* 0xD4		212 */
	"quoteright",	/* 0xD5		213 */
	"divide",	/* 0xD6		214 */
	NULL,		/* 0xD7		215 */
	"ydieresis",	/* 0xD8		216 */
	"Ydieresis",	/* 0xD9		217 */
	"fraction",	/* 0xDA		218 */
	"currency",	/* 0xDB		219 */	/* Euro in MacOS */
	"guilsinglleft",/* 0xDC		220 */
	"guilsinglright",/* 0xDD	221 */
	"fi",		/* 0xDE		222 */
	"fl",		/* 0xDF		223 */
	"daggerdbl",	/* 0xE0		224 */
	"periodcentered",/* 0xE1	225 */
	"quotesinglbase",/* 0xE2	226 */
	"quotedblbase",	/* 0xE3		227 */
	"perthousand",	/* 0xE4		228 */
	"Acircumflex",	/* 0xE5		229 */
	"Ecircumflex",	/* 0xE6		230 */
	"Aacute",	/* 0xE7		231 */
	"Edieresis",	/* 0xE8		232 */
	"Egrave",	/* 0xE9		233 */
	"Iacute",	/* 0xEA		234 */
	"Icircumflex",	/* 0xEB		235 */
	"Idieresis",	/* 0xEC		236 */
	"Igrave",	/* 0xED		237 */
	"Oacute",	/* 0xEE		238 */
	"Ocircumflex",	/* 0xEF		239 */
	NULL,		/* 0xF0		240 */
	"Ograve",	/* 0xF1		241 */
	"Uacute",	/* 0xF2		242 */
	"Ucircumflex",	/* 0xF3		243 */
	"Ugrave",	/* 0xF4		244 */
	"dotlessi",	/* 0xF5		245 */
	"circumflex",	/* 0xF6		246 */
	"tilde",	/* 0xF7		247 */
	"macron",	/* 0xF8		248 */
	"breve",	/* 0xF9		249 */
	"dotaccent",	/* 0xFA		250 */
	"ring",		/* 0xFB		251 */
	"cedilla",	/* 0xFC		252 */
	"hungarumlaut",	/* 0xFD		253 */
	"ogonek",	/* 0xFE		254 */
	"caron"		/* 0xFF		255 */
    }
};

/*
 * ---------------------------------------------------------------
 * EBCDIC encoding vector, code page 1047
 * ---------------------------------------------------------------
 */

static unsigned short pdf_unicodes_ebcdic[256] =
{
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0020, 0x0020, 0x00E2, 0x00E4, 0x00E0, 0x00E1, 0x00E3, 0x00E5,
    0x00E7, 0x00F1, 0x00A2, 0x002E, 0x003C, 0x0028, 0x002B, 0x007C,
    0x0026, 0x00E9, 0x00EA, 0x00EB, 0x00E8, 0x00ED, 0x00EE, 0x00EF,
    0x00EC, 0x00DF, 0x0021, 0x0024, 0x002A, 0x0029, 0x003B, 0x005E,
    0x2212, 0x002F, 0x00C2, 0x00C4, 0x00C0, 0x00C1, 0x00C3, 0x00C5,
    0x00C7, 0x00D1, 0x00A6, 0x002C, 0x0025, 0x005F, 0x003E, 0x003F,
    0x00F8, 0x00C9, 0x00CA, 0x00CB, 0x00C8, 0x00CD, 0x00CE, 0x00CF,
    0x00CC, 0x0060, 0x003A, 0x0023, 0x0040, 0x0027, 0x003D, 0x0022,
    0x00D8, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x00AB, 0x00BB, 0x00F0, 0x00FD, 0x00FE, 0x00B1,
    0x00B0, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F, 0x0070,
    0x0071, 0x0072, 0x00AA, 0x00BA, 0x00E6, 0x00B8, 0x00C6, 0x00A4,
    0x03BC, 0x007E, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077, 0x0078,
    0x0079, 0x007A, 0x00A1, 0x00BF, 0x00D0, 0x005B, 0x00DE, 0x00AE,
    0x00AC, 0x00A3, 0x00A5, 0x02D9, 0x00A9, 0x00A7, 0x00B6, 0x00BC,
    0x00BD, 0x00BE, 0x00DD, 0x00A8, 0x00AF, 0x005D, 0x00B4, 0x00D7,
    0x007B, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x002D, 0x00F4, 0x00F6, 0x00F2, 0x00F3, 0x00F5,
    0x007D, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F, 0x0050,
    0x0051, 0x0052, 0x00B9, 0x00FB, 0x00FC, 0x00F9, 0x00FA, 0x00FF,
    0x005C, 0x00F7, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057, 0x0058,
    0x0059, 0x005A, 0x00B2, 0x00D4, 0x00D6, 0x00D2, 0x00D3, 0x00D5,
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x00B3, 0x00DB, 0x00DC, 0x00D9, 0x00DA, 0x0000
};

static pdf_encodingvector pdf_core_enc_ebcdic =
{
    "ebcdic",
    NULL,
    pdf_unicodes_ebcdic,

    {
	NULL,		/* 0x0		0 */
	NULL,		/* 0x1		1 */
	NULL,		/* 0x2		2 */
	NULL,		/* 0x3		3 */
	NULL,		/* 0x4		4 */
	NULL,		/* 0x5		5 */
	NULL,		/* 0x6		6 */
	NULL,		/* 0x7		7 */
	NULL,		/* 0x8		8 */
	NULL,		/* 0x9		9 */
	NULL,		/* 0xA		10 */
	NULL,		/* 0xB		11 */
	NULL,		/* 0xC		12 */
	NULL,		/* 0xD		13 */
	NULL,		/* 0xE		14 */
	NULL,		/* 0xF		15 */
	NULL,		/* 0x10		16 */
	NULL,		/* 0x11		17 */
	NULL,		/* 0x12		18 */
	NULL,		/* 0x13		19 */
	NULL,		/* 0x14		20 */
	NULL,		/* 0x15		21 */
	NULL,		/* 0x16		22 */
	NULL,		/* 0x17		23 */
	NULL,		/* 0x18		24 */
	NULL,		/* 0x19		25 */
	NULL,		/* 0x1A		26 */
	NULL,		/* 0x1B		27 */
	NULL,		/* 0x1C		28 */
	NULL,		/* 0x1D		29 */
	NULL,		/* 0x1E		30 */
	NULL,		/* 0x1F		31 */
	NULL,		/* 0x20		32 */
	NULL,		/* 0x21		33 */
	NULL,		/* 0x22		34 */
	NULL,		/* 0x23		35 */
	NULL,		/* 0x24		36 */
	NULL,		/* 0x25		37 */
	NULL,		/* 0x26		38 */
	NULL,		/* 0x27		39 */
	NULL,		/* 0x28		40 */
	NULL,		/* 0x29		41 */
	NULL,		/* 0x2A		42 */
	NULL,		/* 0x2B		43 */
	NULL,		/* 0x2C		44 */
	NULL,		/* 0x2D		45 */
	NULL,		/* 0x2E		46 */
	NULL,		/* 0x2F		47 */
	NULL,		/* 0x30		48 */
	NULL,		/* 0x31		49 */
	NULL,		/* 0x32		50 */
	NULL,		/* 0x33		51 */
	NULL,		/* 0x34		52 */
	NULL,		/* 0x35		53 */
	NULL,		/* 0x36		54 */
	NULL,		/* 0x37		55 */
	NULL,		/* 0x38		56 */
	NULL,		/* 0x39		57 */
	NULL,		/* 0x3A		58 */
	NULL,		/* 0x3B		59 */
	NULL,		/* 0x3C		60 */
	NULL,		/* 0x3D		61 */
	NULL,		/* 0x3E		62 */
	NULL,		/* 0x3F		63 */
	"space",	/* 0x40		64 */
	"space",	/* 0x41		65 */
	"acircumflex",	/* 0x42		66 */
	"adieresis",	/* 0x43		67 */
	"agrave",	/* 0x44		68 */
	"aacute",	/* 0x45		69 */
	"atilde",	/* 0x46		70 */
	"aring",	/* 0x47		71 */
	"ccedilla",	/* 0x48		72 */
	"ntilde",	/* 0x49		73 */
	"cent",		/* 0x4A		74 */
	"period",	/* 0x4B		75 */
	"less",		/* 0x4C		76 */
	"parenleft",	/* 0x4D		77 */
	"plus",		/* 0x4E		78 */
	"bar",		/* 0x4F		79 */
	"ampersand",	/* 0x50		80 */
	"eacute",	/* 0x51		81 */
	"ecircumflex",	/* 0x52		82 */
	"edieresis",	/* 0x53		83 */
	"egrave",	/* 0x54		84 */
	"iacute",	/* 0x55		85 */
	"icircumflex",	/* 0x56		86 */
	"idieresis",	/* 0x57		87 */
	"igrave",	/* 0x58		88 */
	"germandbls",	/* 0x59		89 */
	"exclam",	/* 0x5A		90 */
	"dollar",	/* 0x5B		91 */
	"asterisk",	/* 0x5C		92 */
	"parenright",	/* 0x5D		93 */
	"semicolon",	/* 0x5E		94 */
	"asciicircum",	/* 0x5F		95 */
	"minus",	/* 0x60		96 */
	"slash",	/* 0x61		97 */
	"Acircumflex",	/* 0x62		98 */
	"Adieresis",	/* 0x63		99 */
	"Agrave",	/* 0x64		100 */
	"Aacute",	/* 0x65		101 */
	"Atilde",	/* 0x66		102 */
	"Aring",	/* 0x67		103 */
	"Ccedilla",	/* 0x68		104 */
	"Ntilde",	/* 0x69		105 */
	"brokenbar",	/* 0x6A		106 */
	"comma",	/* 0x6B		107 */
	"percent",	/* 0x6C		108 */
	"underscore",	/* 0x6D		109 */
	"greater",	/* 0x6E		110 */
	"question",	/* 0x6F		111 */
	"oslash",	/* 0x70		112 */
	"Eacute",	/* 0x71		113 */
	"Ecircumflex",	/* 0x72		114 */
	"Edieresis",	/* 0x73		115 */
	"Egrave",	/* 0x74		116 */
	"Iacute",	/* 0x75		117 */
	"Icircumflex",	/* 0x76		118 */
	"Idieresis",	/* 0x77		119 */
	"Igrave",	/* 0x78		120 */
	"grave",	/* 0x79		121 */
	"colon",	/* 0x7A		122 */
	"numbersign",	/* 0x7B		123 */
	"at",		/* 0x7C		124 */
	"quotesingle",	/* 0x7D		125 */
	"equal",	/* 0x7E		126 */
	"quotedbl",	/* 0x7F		127 */
	"Oslash",	/* 0x80		128 */
	"a",		/* 0x81		129 */
	"b",		/* 0x82		130 */
	"c",		/* 0x83		131 */
	"d",		/* 0x84		132 */
	"e",		/* 0x85		133 */
	"f",		/* 0x86		134 */
	"g",		/* 0x87		135 */
	"h",		/* 0x88		136 */
	"i",		/* 0x89		137 */
	"guillemotleft",/* 0x8A		138 */
	"guillemotright",/* 0x8B	139 */
	"eth",		/* 0x8C		140 */
	"yacute",	/* 0x8D		141 */
	"thorn",	/* 0x8E		142 */
	"plusminus",	/* 0x8F		143 */
	"degree",	/* 0x90		144 */
	"j",		/* 0x91		145 */
	"k",		/* 0x92		146 */
	"l",		/* 0x93		147 */
	"m",		/* 0x94		148 */
	"n",		/* 0x95		149 */
	"o",		/* 0x96		150 */
	"p",		/* 0x97		151 */
	"q",		/* 0x98		152 */
	"r",		/* 0x99		153 */
	"ordfeminine",	/* 0x9A		154 */
	"ordmasculine",	/* 0x9B		155 */
	"ae",		/* 0x9C		156 */
	"cedilla",	/* 0x9D		157 */
	"AE",		/* 0x9E		158 */
	"currency",	/* 0x9F		159 */
	"mu",		/* 0xA0		160 */
	"asciitilde",	/* 0xA1		161 */
	"s",		/* 0xA2		162 */
	"t",		/* 0xA3		163 */
	"u",		/* 0xA4		164 */
	"v",		/* 0xA5		165 */
	"w",		/* 0xA6		166 */
	"x",		/* 0xA7		167 */
	"y",		/* 0xA8		168 */
	"z",		/* 0xA9		169 */
	"exclamdown",	/* 0xAA		170 */
	"questiondown",	/* 0xAB		171 */
	"Eth",		/* 0xAC		172 */
	"bracketleft",	/* 0xAD		173 */
	"Thorn",	/* 0xAE		174 */
	"registered",	/* 0xAF		175 */
	"logicalnot",	/* 0xB0		176 */
	"sterling",	/* 0xB1		177 */
	"yen",		/* 0xB2		178 */
	"dotaccent",	/* 0xB3		179 */
	"copyright",	/* 0xB4		180 */
	"section",	/* 0xB5		181 */
	"paragraph",	/* 0xB6		182 */
	"onequarter",	/* 0xB7		183 */
	"onehalf",	/* 0xB8		184 */
	"threequarters",/* 0xB9		185 */
	"Yacute",	/* 0xBA		186 */
	"dieresis",	/* 0xBB		187 */
	"macron",	/* 0xBC		188 */
	"bracketright",	/* 0xBD		189 */
	"acute",	/* 0xBE		190 */
	"multiply",	/* 0xBF		191 */
	"braceleft",	/* 0xC0		192 */
	"A",		/* 0xC1		193 */
	"B",		/* 0xC2		194 */
	"C",		/* 0xC3		195 */
	"D",		/* 0xC4		196 */
	"E",		/* 0xC5		197 */
	"F",		/* 0xC6		198 */
	"G",		/* 0xC7		199 */
	"H",		/* 0xC8		200 */
	"I",		/* 0xC9		201 */
	"hyphen",	/* 0xCA		202 */
	"ocircumflex",	/* 0xCB		203 */
	"odieresis",	/* 0xCC		204 */
	"ograve",	/* 0xCD		205 */
	"oacute",	/* 0xCE		206 */
	"otilde",	/* 0xCF		207 */
	"braceright",	/* 0xD0		208 */
	"J",		/* 0xD1		209 */
	"K",		/* 0xD2		210 */
	"L",		/* 0xD3		211 */
	"M",		/* 0xD4		212 */
	"N",		/* 0xD5		213 */
	"O",		/* 0xD6		214 */
	"P",		/* 0xD7		215 */
	"Q",		/* 0xD8		216 */
	"R",		/* 0xD9		217 */
	"onesuperior",	/* 0xDA		218 */
	"ucircumflex",	/* 0xDB		219 */
	"udieresis",	/* 0xDC		220 */
	"ugrave",	/* 0xDD		221 */
	"uacute",	/* 0xDE		222 */
	"ydieresis",	/* 0xDF		223 */
	"backslash",	/* 0xE0		224 */
	"divide",	/* 0xE1		225 */
	"S",		/* 0xE2		226 */
	"T",		/* 0xE3		227 */
	"U",		/* 0xE4		228 */
	"V",		/* 0xE5		229 */
	"W",		/* 0xE6		230 */
	"X",		/* 0xE7		231 */
	"Y",		/* 0xE8		232 */
	"Z",		/* 0xE9		233 */
	"twosuperior",	/* 0xEA		234 */
	"Ocircumflex",	/* 0xEB		235 */
	"Odieresis",	/* 0xEC		236 */
	"Ograve",	/* 0xED		237 */
	"Oacute",	/* 0xEE		238 */
	"Otilde",	/* 0xEF		239 */
	"zero",		/* 0xF0		240 */
	"one",		/* 0xF1		241 */
	"two",		/* 0xF2		242 */
	"three",	/* 0xF3		243 */
	"four",		/* 0xF4		244 */
	"five",		/* 0xF5		245 */
	"six",		/* 0xF6		246 */
	"seven",	/* 0xF7		247 */
	"eight",	/* 0xF8		248 */
	"nine",		/* 0xF9		249 */
	"threesuperior",/* 0xFA		250 */
	"Ucircumflex",	/* 0xFB		251 */
	"Udieresis",	/* 0xFC		252 */
	"Ugrave",	/* 0xFD		253 */
	"Uacute",	/* 0xFE		254 */
	NULL,		/* 0xFF		255 */
    }
};

/*
 * The predefined encodings must not be changed or rearranged.
 * The order of encodings here must match that of the
 * pdf_encoding enum in p_font.h.
 * More encoding definitions can easily be added here.
 */

static pdf_encodingvector *pdf_core_encodings[] = {
    &pdf_core_enc_winansi,
    &pdf_core_enc_macroman,
    &pdf_core_enc_ebcdic,
};

#define SIZEOF_CORE_ENCODINGS	\
	((int) (sizeof (pdf_core_encodings) / sizeof (pdf_encodingvector *)))

#endif	/* P_ENCODING_H */
