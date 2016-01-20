/*=================================================================
 
	File:		charbyte.c

	$Header: /Projects/Toolbox/ct/SCDBCSDT.CPP 2	 5/30/97 8:45a Wmanis $

	Contains:	DBCS code.

	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

=================================================================*/


#include "scdbcsdt.h"
#include "scstcach.h"

/* ==================================================================== */
/* ==================================================================== */

scDBCSDetector::scDBCSDetector( TypeSpec ts )
	: dbcs_(0)
{
	setDBCS( ts );
}

/* ==================================================================== */

void scDBCSDetector::setDBCS( TypeSpec ts )
{
	dbcs_ = ts.ptr() ? scCachedStyle::GetCachedStyle( ts ).GetBreakLang() : false;
}

/* ==================================================================== */

long scDBCSDetector::StrLen( const char* str ) const
{
	long len = 0;
	
	for ( ; *str; ) {
		switch( ByteType ( *str++ ) ) {
			case eOnlyByte:
			case eLastByte:
				len++;
				break;
		}
	}
	return len;
}

/* ==================================================================== */


scDBCSDetector::eByteType scDBCSDetector::shiftjis_[] = {
	eOnlyByte, /* 0x00 */ 
	eOnlyByte, /* 0x01 */ 
	eOnlyByte, /* 0x02 */ 
	eOnlyByte, /* 0x03 */ 
	eOnlyByte, /* 0x04 */ 
	eOnlyByte, /* 0x05 */ 
	eOnlyByte, /* 0x06 */ 
	eOnlyByte, /* 0x07 */ 
	eOnlyByte, /* 0x08 */ 
	eOnlyByte, /* 0x09 */ 
	eOnlyByte, /* 0x0a */ 
	eOnlyByte, /* 0x0b */ 
	eOnlyByte, /* 0x0c */ 
	eOnlyByte, /* 0x0d */ 
	eOnlyByte, /* 0x0e */ 
	eOnlyByte, /* 0x0f */ 
	eOnlyByte, /* 0x10 */ 
	eOnlyByte, /* 0x11 */ 
	eOnlyByte, /* 0x12 */ 
	eOnlyByte, /* 0x13 */ 
	eOnlyByte, /* 0x14 */ 
	eOnlyByte, /* 0x15 */ 
	eOnlyByte, /* 0x16 */ 
	eOnlyByte, /* 0x17 */ 
	eOnlyByte, /* 0x18 */ 
	eOnlyByte, /* 0x19 */ 
	eOnlyByte, /* 0x1a */ 
	eOnlyByte, /* 0x1b */ 
	eOnlyByte, /* 0x1c */ 
	eOnlyByte, /* 0x1d */ 
	eOnlyByte, /* 0x1e */ 
	eOnlyByte, /* 0x1f */ 
	eOnlyByte, /* 0x20 */ 
	eOnlyByte, /* 0x21 */ 
	eOnlyByte, /* 0x22 */ 
	eOnlyByte, /* 0x23 */ 
	eOnlyByte, /* 0x24 */ 
	eOnlyByte, /* 0x25 */ 
	eOnlyByte, /* 0x26 */ 
	eOnlyByte, /* 0x27 */ 
	eOnlyByte, /* 0x28 */ 
	eOnlyByte, /* 0x29 */ 
	eOnlyByte, /* 0x2a */ 
	eOnlyByte, /* 0x2b */ 
	eOnlyByte, /* 0x2c */ 
	eOnlyByte, /* 0x2d */ 
	eOnlyByte, /* 0x2e */ 
	eOnlyByte, /* 0x2f */ 
	eOnlyByte, /* 0x30 */ 
	eOnlyByte, /* 0x31 */ 
	eOnlyByte, /* 0x32 */ 
	eOnlyByte, /* 0x33 */ 
	eOnlyByte, /* 0x34 */ 
	eOnlyByte, /* 0x35 */ 
	eOnlyByte, /* 0x36 */ 
	eOnlyByte, /* 0x37 */ 
	eOnlyByte, /* 0x38 */ 
	eOnlyByte, /* 0x39 */ 
	eOnlyByte, /* 0x3a */ 
	eOnlyByte, /* 0x3b */ 
	eOnlyByte, /* 0x3c */ 
	eOnlyByte, /* 0x3d */ 
	eOnlyByte, /* 0x3e */ 
	eOnlyByte, /* 0x3f */ 
	eOnlyByte, /* 0x40 */ 
	eOnlyByte, /* 0x41 */ 
	eOnlyByte, /* 0x42 */ 
	eOnlyByte, /* 0x43 */ 
	eOnlyByte, /* 0x44 */ 
	eOnlyByte, /* 0x45 */ 
	eOnlyByte, /* 0x46 */ 
	eOnlyByte, /* 0x47 */ 
	eOnlyByte, /* 0x48 */ 
	eOnlyByte, /* 0x49 */ 
	eOnlyByte, /* 0x4a */ 
	eOnlyByte, /* 0x4b */ 
	eOnlyByte, /* 0x4c */ 
	eOnlyByte, /* 0x4d */ 
	eOnlyByte, /* 0x4e */ 
	eOnlyByte, /* 0x4f */ 
	eOnlyByte, /* 0x50 */ 
	eOnlyByte, /* 0x51 */ 
	eOnlyByte, /* 0x52 */ 
	eOnlyByte, /* 0x53 */ 
	eOnlyByte, /* 0x54 */ 
	eOnlyByte, /* 0x55 */ 
	eOnlyByte, /* 0x56 */ 
	eOnlyByte, /* 0x57 */ 
	eOnlyByte, /* 0x58 */ 
	eOnlyByte, /* 0x59 */ 
	eOnlyByte, /* 0x5a */ 
	eOnlyByte, /* 0x5b */ 
	eOnlyByte, /* 0x5c */ 
	eOnlyByte, /* 0x5d */ 
	eOnlyByte, /* 0x5e */ 
	eOnlyByte, /* 0x5f */ 
	eOnlyByte, /* 0x60 */ 
	eOnlyByte, /* 0x61 */ 
	eOnlyByte, /* 0x62 */ 
	eOnlyByte, /* 0x63 */ 
	eOnlyByte, /* 0x64 */ 
	eOnlyByte, /* 0x65 */ 
	eOnlyByte, /* 0x66 */ 
	eOnlyByte, /* 0x67 */ 
	eOnlyByte, /* 0x68 */ 
	eOnlyByte, /* 0x69 */ 
	eOnlyByte, /* 0x6a */ 
	eOnlyByte, /* 0x6b */ 
	eOnlyByte, /* 0x6c */ 
	eOnlyByte, /* 0x6d */ 
	eOnlyByte, /* 0x6e */ 
	eOnlyByte, /* 0x6f */ 
	eOnlyByte, /* 0x70 */ 
	eOnlyByte, /* 0x71 */ 
	eOnlyByte, /* 0x72 */ 
	eOnlyByte, /* 0x73 */ 
	eOnlyByte, /* 0x74 */ 
	eOnlyByte, /* 0x75 */ 
	eOnlyByte, /* 0x76 */ 
	eOnlyByte, /* 0x77 */ 
	eOnlyByte, /* 0x78 */ 
	eOnlyByte, /* 0x79 */ 
	eOnlyByte, /* 0x7a */ 
	eOnlyByte, /* 0x7b */ 
	eOnlyByte, /* 0x7c */ 
	eOnlyByte, /* 0x7d */ 
	eOnlyByte, /* 0x7e */ 
	eOnlyByte, /* 0x7f */ 
	eOnlyByte, /* 0x80 */ 
	eFirstByte, /* 0x81 */ 
	eFirstByte, /* 0x82 */ 
	eFirstByte, /* 0x83 */ 
	eFirstByte, /* 0x84 */ 
	eFirstByte, /* 0x85 */ 
	eFirstByte, /* 0x86 */ 
	eFirstByte, /* 0x87 */ 
	eFirstByte, /* 0x88 */ 
	eFirstByte, /* 0x89 */ 
	eFirstByte, /* 0x8a */ 
	eFirstByte, /* 0x8b */ 
	eFirstByte, /* 0x8c */ 
	eFirstByte, /* 0x8d */ 
	eFirstByte, /* 0x8e */ 
	eFirstByte, /* 0x8f */ 
	eFirstByte, /* 0x90 */ 
	eFirstByte, /* 0x91 */ 
	eFirstByte, /* 0x92 */ 
	eFirstByte, /* 0x93 */ 
	eFirstByte, /* 0x94 */ 
	eFirstByte, /* 0x95 */ 
	eFirstByte, /* 0x96 */ 
	eFirstByte, /* 0x97 */ 
	eFirstByte, /* 0x98 */ 
	eFirstByte, /* 0x99 */ 
	eFirstByte, /* 0x9a */ 
	eFirstByte, /* 0x9b */ 
	eFirstByte, /* 0x9c */ 
	eFirstByte, /* 0x9d */ 
	eFirstByte, /* 0x9e */ 
	eFirstByte, /* 0x9f */ 
	eOnlyByte, /* 0xa0 */ 
	eOnlyByte, /* 0xa1 */ 
	eOnlyByte, /* 0xa2 */ 
	eOnlyByte, /* 0xa3 */ 
	eOnlyByte, /* 0xa4 */ 
	eOnlyByte, /* 0xa5 */ 
	eOnlyByte, /* 0xa6 */ 
	eOnlyByte, /* 0xa7 */ 
	eOnlyByte, /* 0xa8 */ 
	eOnlyByte, /* 0xa9 */ 
	eOnlyByte, /* 0xaa */ 
	eOnlyByte, /* 0xab */ 
	eOnlyByte, /* 0xac */ 
	eOnlyByte, /* 0xad */ 
	eOnlyByte, /* 0xae */ 
	eOnlyByte, /* 0xaf */ 
	eOnlyByte, /* 0xb0 */ 
	eOnlyByte, /* 0xb1 */ 
	eOnlyByte, /* 0xb2 */ 
	eOnlyByte, /* 0xb3 */ 
	eOnlyByte, /* 0xb4 */ 
	eOnlyByte, /* 0xb5 */ 
	eOnlyByte, /* 0xb6 */ 
	eOnlyByte, /* 0xb7 */ 
	eOnlyByte, /* 0xb8 */ 
	eOnlyByte, /* 0xb9 */ 
	eOnlyByte, /* 0xba */ 
	eOnlyByte, /* 0xbb */ 
	eOnlyByte, /* 0xbc */ 
	eOnlyByte, /* 0xbd */ 
	eOnlyByte, /* 0xbe */ 
	eOnlyByte, /* 0xbf */ 
	eOnlyByte, /* 0xc0 */ 
	eOnlyByte, /* 0xc1 */ 
	eOnlyByte, /* 0xc2 */ 
	eOnlyByte, /* 0xc3 */ 
	eOnlyByte, /* 0xc4 */ 
	eOnlyByte, /* 0xc5 */ 
	eOnlyByte, /* 0xc6 */ 
	eOnlyByte, /* 0xc7 */ 
	eOnlyByte, /* 0xc8 */ 
	eOnlyByte, /* 0xc9 */ 
	eOnlyByte, /* 0xca */ 
	eOnlyByte, /* 0xcb */ 
	eOnlyByte, /* 0xcc */ 
	eOnlyByte, /* 0xcd */ 
	eOnlyByte, /* 0xce */ 
	eOnlyByte, /* 0xcf */ 
	eOnlyByte, /* 0xd0 */ 
	eOnlyByte, /* 0xd1 */ 
	eOnlyByte, /* 0xd2 */ 
	eOnlyByte, /* 0xd3 */ 
	eOnlyByte, /* 0xd4 */ 
	eOnlyByte, /* 0xd5 */ 
	eOnlyByte, /* 0xd6 */ 
	eOnlyByte, /* 0xd7 */ 
	eOnlyByte, /* 0xd8 */ 
	eOnlyByte, /* 0xd9 */ 
	eOnlyByte, /* 0xda */ 
	eOnlyByte, /* 0xdb */ 
	eOnlyByte, /* 0xdc */ 
	eOnlyByte, /* 0xdd */ 
	eOnlyByte, /* 0xde */ 
	eOnlyByte, /* 0xdf */ 
	eFirstByte, /* 0xe0 */ 
	eFirstByte, /* 0xe1 */ 
	eFirstByte, /* 0xe2 */ 
	eFirstByte, /* 0xe3 */ 
	eFirstByte, /* 0xe4 */ 
	eFirstByte, /* 0xe5 */ 
	eFirstByte, /* 0xe6 */ 
	eFirstByte, /* 0xe7 */ 
	eFirstByte, /* 0xe8 */ 
	eFirstByte, /* 0xe9 */ 
	eFirstByte, /* 0xea */ 
	eFirstByte, /* 0xeb */ 
	eFirstByte, /* 0xec */ 
	eFirstByte, /* 0xed */ 
	eFirstByte, /* 0xee */ 
	eFirstByte, /* 0xef */ 
	eFirstByte, /* 0xf0 */ 
	eFirstByte, /* 0xf1 */ 
	eFirstByte, /* 0xf2 */ 
	eFirstByte, /* 0xf3 */ 
	eFirstByte, /* 0xf4 */ 
	eFirstByte, /* 0xf5 */ 
	eFirstByte, /* 0xf6 */ 
	eFirstByte, /* 0xf7 */ 
	eFirstByte, /* 0xf8 */ 
	eFirstByte, /* 0xf9 */ 
	eFirstByte, /* 0xfa */ 
	eFirstByte, /* 0xfb */ 
	eFirstByte, /* 0xfc */ 
	eOnlyByte, /* 0xfd */ 
	eOnlyByte, /* 0xfe */ 
	eOnlyByte /* 0xff */ 
};
