/*
//	TTDLL.CPP
//
//		Copyright (C) 1993 MicroLogic Software, Inc.
//
// Windows TrueType DLL object methods.
//
// $Header: /PM8/App/TTDLL.CPP 1     3/03/99 6:12p Gbeddow $
//
// $Log: /PM8/App/TTDLL.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:50   Fred
// Initial revision.
// 
//    Rev 1.5   03 Apr 1997 17:22:58   Fred
// Fixed some warnings in 16-bit
// 
//    Rev 1.4   03 Apr 1997 09:32:14   Fred
// More text cleanup
// 
//    Rev 1.3   26 Mar 1997 15:55:10   Fred
// Removed old geronimo stuff
// 
//    Rev 1.2   13 Jan 1997 15:25:36   Jay
// 16-bit now compiles.
// 
//    Rev 1.1   06 Jan 1997 10:16:38   Fred
// Start of new text object
// 
//    Rev 1.0   14 Mar 1996 13:44:20   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:58   FRED
// Initial revision.
// 
//    Rev 2.6   18 Oct 1995 18:39:32   FRED
// Fixed memory leak
// 
//    Rev 2.5   16 Oct 1995 15:14:06   FRED
// 32 bit DLL name is PMWTT.DLL
// 
//    Rev 2.4   04 Oct 1995 10:56:28   FRED
// 32 bit TrueType engine
// 
//    Rev 2.3   25 Sep 1995 11:38:24   FRED
// TrueType speedups
// 
//    Rev 2.2   20 Sep 1995 10:01:14   FRED
// PmwThunk support for PMWTT.DLL
// 
//    Rev 2.1   14 Sep 1995 09:32:26   JAY
// 32-bit changes
// 
//    Rev 2.0   07 Feb 1995 15:48:14   JAY
// Initial revision.
// 
//    Rev 1.6   30 Nov 1994 16:33:42   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.5   04 Aug 1994 07:32:46   JAY
// Large model and 32-bit changes
// 
//    Rev 1.4   11 May 1994 16:33:00   JAY
// made some LPSTRs into LPCSTRs
// 
//    Rev 1.4   11 May 1994 16:32:38   JAY
// 
//    Rev 1.3   30 Jan 1994 09:49:26   JAY
//  
// 
//    Rev 1.2   13 Jan 1994 17:00:54   JAY
//  
// 
//    Rev 1.1   30 Dec 1993 16:10:42   JAY
//  
// 
//    Rev 1.0   16 Dec 1993 16:25:28   JAY
//  
*/

#include "stdafx.h"

#include <stdlib.h>
#include <string.h>

#include "geronimo.h"
#include "truetype.h"
#include "tfengine.h"
#include "ttdll.h"


/*
// The destructor for a TrueTypeDLL object.
*/

TrueTypeDLL::~TrueTypeDLL()
{
	unload();
}

/*
// Load TrueTypeDLL.
*/

BOOL TrueTypeDLL::load(LPCSTR dll_path)
{
	char path[_MAX_PATH];

	strcpy(path, dll_path);
	strcat(path, "PMWTT.DLL");

	if ((library = (HINSTANCE)LoadLibrary(path)) != NULL)
	{
#ifdef WIN32
		if (((*(FARPROC far *)&lpOpenFonts				= GetProcAddress(library, "fs_OpenFonts"))         != NULL)
		 && ((*(FARPROC far *)&lpInitialize				= GetProcAddress(library, "fs_Initialize"))        != NULL)
		 && ((*(FARPROC far *)&lpNewSfnt					= GetProcAddress(library, "fs_NewSfnt"))           != NULL)
		 && ((*(FARPROC far *)&lpNewTransformation	= GetProcAddress(library, "fs_NewTransformation")) != NULL)
		 && ((*(FARPROC far *)&lpGetAdvanceWidth		= GetProcAddress(library, "fs_GetAdvanceWidth"))   != NULL)
		 && ((*(FARPROC far *)&lpNewGlyph				= GetProcAddress(library, "fs_NewGlyph"))          != NULL)
		 && ((*(FARPROC far *)&lpContourGridFit		= GetProcAddress(library, "fs_ContourGridFit"))    != NULL)
		 && ((*(FARPROC far *)&lpContourNoGridFit		= GetProcAddress(library, "fs_ContourNoGridFit"))  != NULL)
		 && ((*(FARPROC far *)&lpNewOutline				= GetProcAddress(library, "fs_NewOutline"))        != NULL)
		 && ((*(FARPROC far *)&lpFindBitmapSize		= GetProcAddress(library, "fs_FindBitMapSize"))    != NULL)
		 && ((*(FARPROC far *)&lpContourScan			= GetProcAddress(library, "fs_ContourScan"))       != NULL)
		 && ((*(FARPROC far *)&lpCloseFonts				= GetProcAddress(library, "fs_CloseFonts"))        != NULL))
#else
		if (((*(FARPROC far *)&lpOpenFonts				= GetProcAddress(library, "_FS_OPENFONTS"))         != NULL)
		 && ((*(FARPROC far *)&lpInitialize				= GetProcAddress(library, "_FS_INITIALIZE"))        != NULL)
		 && ((*(FARPROC far *)&lpNewSfnt					= GetProcAddress(library, "_FS_NEWSFNT"))           != NULL)
		 && ((*(FARPROC far *)&lpNewTransformation	= GetProcAddress(library, "_FS_NEWTRANSFORMATION")) != NULL)
		 && ((*(FARPROC far *)&lpGetAdvanceWidth		= GetProcAddress(library, "_FS_GETADVANCEWIDTH"))   != NULL)
		 && ((*(FARPROC far *)&lpNewGlyph				= GetProcAddress(library, "_FS_NEWGLYPH"))          != NULL)
		 && ((*(FARPROC far *)&lpContourGridFit		= GetProcAddress(library, "_FS_CONTOURGRIDFIT"))    != NULL)
		 && ((*(FARPROC far *)&lpContourNoGridFit		= GetProcAddress(library, "_FS_CONTOURNOGRIDFIT"))  != NULL)
		 && ((*(FARPROC far *)&lpNewOutline				= GetProcAddress(library, "_FS_NEWOUTLINE"))        != NULL)
		 && ((*(FARPROC far *)&lpFindBitmapSize		= GetProcAddress(library, "_FS_FINDBITMAPSIZE"))    != NULL)
		 && ((*(FARPROC far *)&lpContourScan			= GetProcAddress(library, "_FS_CONTOURSCAN"))       != NULL)
		 && ((*(FARPROC far *)&lpCloseFonts				= GetProcAddress(library, "_FS_CLOSEFONTS"))        != NULL))
#endif
		{
			return TRUE;
		}
	}
	else
	{                   
#ifdef WIN32
		DWORD dwError = GetLastError();
#endif
		library = NULL;
	}
	return FALSE;
}

/*
// Unload a TrueTypeDLL.
*/

VOID TrueTypeDLL::unload(VOID)
{
	if (library != NULL)
	{
		FreeLibrary(library);
	}
}

/*
// Patch-through routines.
*/

void FreeMemoryBase(tinp inp, tout out, int nFragment)
{
	delete [] inp->memoryBases[nFragment];
	inp->memoryBases[nFragment] = NULL;
}

void SetMemoryBase(tinp inp, tout out, int nFragment)
{
	FreeMemoryBase(inp, out, nFragment);
	ASSERT(inp->memoryBases[nFragment] == NULL);
	int nSize = (int)(out->memorySizes[nFragment]);
	if (nSize != 0)
	{
		// Special handling for area 6: The requested size is
		// mulitpled by the number of rows in the bitmap.

		if (nFragment == 6)
		{
			nSize *= (int)(inp->param.scan.topClip-inp->param.scan.bottomClip);
		}

		inp->memoryBases[nFragment] = new char[nSize];
	}
}

void TrueTypeDLL::FreeAllocatedMemory(tinp inp, tout out)
{
	TRACE("TTDLL.FreeAllocatedMemory()\n");
	for (int i = 0; i < MEMORYFRAGMENTS; i++)
	{
		FreeMemoryBase(inp, out, i);
	}
}

tTTResult TrueTypeDLL::fs_OpenFonts(tinp inp, tout out)
{
	tTTResult r = (*lpOpenFonts)(inp, out);
	if (r == 0)
	{
		for (int i = 0; i < MEMORYFRAGMENTS; i++)
		{
			inp->memoryBases[i] = NULL;
		}

		SetMemoryBase(inp, out, 0);
		SetMemoryBase(inp, out, 1);
		SetMemoryBase(inp, out, 2);
	}
	return r;
}

tTTResult TrueTypeDLL::fs_Initialize(tinp inp, tout out)
{
	tTTResult r = (*lpInitialize)(inp, out);
	return r;
}

tTTResult TrueTypeDLL::fs_NewSfnt(tinp inp, tout out)
{
	tTTResult r = (*lpNewSfnt)(inp, out);
	SetMemoryBase(inp, out, 3);
	SetMemoryBase(inp, out, 4);
	return r;
}

tTTResult TrueTypeDLL::fs_NewTransformation(tinp inp, tout out)
{
	tTTResult r = (*lpNewTransformation)(inp, out);
	return r;
}

tTTResult TrueTypeDLL::fs_GetAdvanceWidth(tinp inp, tout out)
{
	tTTResult r = (*lpGetAdvanceWidth)(inp, out);
	return r;
}

tTTResult TrueTypeDLL::fs_NewGlyph(tinp inp, tout out)
{
	tTTResult r = (*lpNewGlyph)(inp, out);
	return r;
}

tTTResult TrueTypeDLL::fs_ContourGridFit(tinp inp, tout out)
{
	tTTResult r = (*lpContourGridFit)(inp, out);
	return r;
}

tTTResult TrueTypeDLL::fs_ContourNoGridFit(tinp inp, tout out)
{
	tTTResult r = (*lpContourNoGridFit)(inp, out);
	return r;
}

tTTResult TrueTypeDLL::fs_NewOutline(tinp inp, tout out)
{
	tTTResult r = (*lpNewOutline)(inp, out);
	return r;
}

tTTResult TrueTypeDLL::fs_FindBitmapSize(tinp inp, tout out)
{
	tTTResult r = (*lpFindBitmapSize)(inp, out);
	return r;
}

tTTResult TrueTypeDLL::fs_ContourScan(tinp inp, tout out)
{
	SetMemoryBase(inp, out, 6);
	SetMemoryBase(inp, out, 7);
	tTTResult r = (*lpContourScan)(inp, out);
	return r;
}

tTTResult TrueTypeDLL::fs_CloseFonts(tinp inp, tout out)
{
	tTTResult r = (*lpCloseFonts)(inp, out);
	for (int i = 0; i < MEMORYFRAGMENTS; i++)
	{
		FreeMemoryBase(inp, out, i);
	}
	return r;
}

tTTResult TrueTypeDLL::fs_SetBreath(tinp inp, tout out)
{
	return TTResult_DLLNotInitialized;
}
