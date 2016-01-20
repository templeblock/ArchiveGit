//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯				  
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _APPSTYLE_H_
#define _APPSTYLE_H_

// Image control styles
#define IS_NOSAMPLING	0x0001L
#define IS_STRETCHABLE	0x0002L
#define IS_MOVEABLE		0x0004L
//save room for WS_TEXT 0x0010L
#define IS_RATIO_CONSTRAIN  0x0020L
#define IS_LIMITSIZE	0x0040L
#define IS_OBJECT		0x0080L
#define IS_CMS			0x0100L
// save room for WS_NOBGND	0x0200L
#define IS_ACTIVEIMAGECMS 0x0400L
#define IS_HASCHANGED	0x0800L

//***************************************************

// Ruler control styles
#define RS_HORZ		1L
#define RS_VERT		2L

// Panel control styles
#define PS_BLACKLINE	1L
#define PS_VDIVIDER	2L
#define PS_HDIVIDER	4L

// ComboBox styles
#define CBS_THINFONT	0x1000L

#endif // _APPSTYLE_H_


