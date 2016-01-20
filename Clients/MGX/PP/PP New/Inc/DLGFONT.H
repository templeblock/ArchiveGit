// (c) Copyright 1995 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX

#ifndef _DLGFONT_H_
#define _DLGFONT_H_

#define PP_MULTIBYTE

#ifdef PP_JAPANESE
 	#define PP_FONT 12, _T("ÇlÇr ÉSÉVÉbÉN")
 	#define PP_TOOLBOXFLOATFONT 5, _T("ÇlÇr ÉSÉVÉbÉN")
 	#define PP_TASKMANFONT 14, _T("ÇlÇr ÉSÉVÉbÉN")
#else 
	#undef PP_MULTIBYTE
	#define PP_FONT 8, _T("MS Sans Serif")
	#define PP_TOOLBOXFLOATFONT 5, _T("MS Sans Serif")
	#define PP_TASKMANFONT 14, _T("MS Sans Serif")
#endif

#endif	// _DLGFONT_H


