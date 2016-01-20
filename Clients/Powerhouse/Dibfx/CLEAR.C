/*************************************************************************
	clear.c

	DibClear function

	This file is Copyright (c) 1994 Microsoft Corp
*/

#ifndef _INC_WINDOWS
#include <windows.h>
#include <windowsx.h>
#endif

#include <assert.h>
#include "dib.h"
#include "dibfx.h"

/*************************************************************************
	The real memset function from clear32.asm
*/

extern VOID FAR PASCAL ClearMem32(WORD selDst, DWORD offDst, DWORD cb, BYTE value);

/*************************************************************************
	The wrapper for DibClear
*/

BOOL FAR PASCAL DibClear(PDIB pdibDst, LPVOID pbitsDst, BYTE value)
{
	WORD selDst;
	DWORD offDst;

	if ((pdibDst == NULL)
	||  (pbitsDst == NULL))
		return FALSE;

	if ((long)pbitsDst & 3)
		return FALSE;	// we must be dword aligned

	selDst = HIWORD(pbitsDst);
	offDst = LOWORD(pbitsDst);

	ClearMem32(selDst, offDst, DibSizeImage(pdibDst), value);
	return TRUE;
}

