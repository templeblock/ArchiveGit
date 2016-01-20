/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/
/*
	debug.c:
		DbgOut -- printf to the debug window
*/

#include <windows.h>

#include "hotspot.h"

/*
	DbgOut -- uses wvsprintf to simulate printf but putting the output
		into a string, then sends the string to the debug window with
		OutputDebugString.
*/
void cdecl DbgOut(LPSTR lpFormat, ...)
{
	char buf[256];
	wvsprintf(buf, lpFormat, (LPSTR)(&lpFormat+1));
	OutputDebugString(buf);
	OutputDebugString("\r\n");
}
