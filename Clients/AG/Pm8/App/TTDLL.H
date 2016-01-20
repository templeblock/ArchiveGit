/*
// $Header: /PM8/App/TTDLL.H 1     3/03/99 6:12p Gbeddow $
//
// TrueType DLL wrapper object definitions.
// ** INCLUDE THIS FILE INSTEAD OF TRUETYPE.H **
//
// $Log: /PM8/App/TTDLL.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:40   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:20   Fred
// Initial revision.
// 
//    Rev 1.3   03 Apr 1997 09:32:44   Fred
// More text cleanup
// 
//    Rev 1.2   26 Mar 1997 15:55:10   Fred
// Removed old geronimo stuff
// 
//    Rev 1.1   06 Jan 1997 10:16:38   Fred
// Start of new text object
// 
//    Rev 1.0   14 Mar 1996 13:42:54   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:20   FRED
// Initial revision.
// 
//    Rev 2.4   22 Dec 1995 15:23:44   JAY
// 
//    Rev 2.3   18 Oct 1995 18:39:30   FRED
// Fixed memory leak
// 
//    Rev 2.2   04 Oct 1995 10:56:32   FRED
// 32 bit TrueType engine
// 
//    Rev 2.1   20 Sep 1995 10:01:16   FRED
// PmwThunk support for PMWTT.DLL
// 
//    Rev 2.0   07 Feb 1995 15:49:06   JAY
// Initial revision.
// 
//    Rev 1.2   11 May 1994 16:33:02   JAY
// made some LPSTRs into LPCSTRs
// 
//    Rev 1.1   30 Jan 1994 09:49:52   JAY
//  
// 
//    Rev 1.0   16 Dec 1993 16:26:16   JAY
//  
*/

#ifndef __TTDLL_H__
#define __TTDLL_H__

#include "truetype.h"					// Generic TrueType definitions
#include "file.h"


/*
// Windows TrueType object
*/

class TrueTypeDLL
{
private:

// Data

	HINSTANCE	library;

// DLL management.

	VOID unload(VOID);

	tTTResult (far * lpOpenFonts)(tinp inp, tout out);
	tTTResult (far * lpInitialize)(tinp inp, tout out);
	tTTResult (far * lpNewSfnt)(tinp inp, tout out);
	tTTResult (far * lpNewTransformation)(tinp inp, tout out);
	tTTResult (far * lpGetAdvanceWidth)(tinp inp, tout out);
	tTTResult (far * lpNewGlyph)(tinp inp, tout out);
	tTTResult (far * lpContourGridFit)(tinp inp, tout out);
	tTTResult (far * lpContourNoGridFit)(tinp inp, tout out);
	tTTResult (far * lpNewOutline)(tinp inp, tout out);
	tTTResult (far * lpFindBitmapSize)(tinp inp, tout out);
	tTTResult (far * lpContourScan)(tinp inp, tout out);
	tTTResult (far * lpCloseFonts)(tinp inp, tout out);

public:
// Methods.

	TrueTypeDLL() : library(NULL) {}
	~TrueTypeDLL();

	BOOL load(LPCSTR dll_path);

// Standard methods.

	void FreeAllocatedMemory(tinp inp, tout out);

	tTTResult fs_OpenFonts(tinp inp, tout out);
	tTTResult fs_Initialize(tinp inp, tout out);
	tTTResult fs_NewSfnt(tinp inp, tout out);
	tTTResult fs_NewTransformation(tinp inp, tout out);
	tTTResult fs_GetAdvanceWidth(tinp inp, tout out);
	tTTResult fs_NewGlyph(tinp inp, tout out);
	tTTResult fs_ContourGridFit(tinp inp, tout out);
	tTTResult fs_ContourNoGridFit(tinp inp, tout out);
	tTTResult fs_NewOutline(tinp inp, tout out);
	tTTResult fs_FindBitmapSize(tinp inp, tout out);
	tTTResult fs_ContourScan(tinp inp, tout out);
	tTTResult fs_CloseFonts(tinp inp, tout out);
	tTTResult fs_SetBreath(tinp inp, tout out);
};

#endif					// #ifndef __TTDLL_H__
