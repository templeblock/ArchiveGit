/****************************************************************************
 *
 *  $Header: /Development/TPSLibs/Source/WINDEV.C 1     11/06/96 7:22a Mike_houle $
 *
 *  Module Name: WINDEV.C
 *  Description: Device startup and shutdown for TPS debug library (Windows).
 *  Version:     v0.00
 *  Author:      
 *  Date:        
 *
 *  Copyright (c) 1994, 1995 by Turning Point Software, Inc.  All Rights Reserved.
 *
 *  $Log: /Development/TPSLibs/Source/WINDEV.C $
 * 
 * 1     11/06/96 7:22a Mike_houle
 * 
 * 2     2/15/96 3:23p Steve
 * Fixed a couple of warnings. Use the variable argument stuff defined in
 * stdarg.h
    
    6     2/28/95 5:49p Bob
    cleaned up use of global variables
    fixed potential crash of mouse/keyboard input
    no longer using SYM file for Win stack crawl; now strictly CodeView
    CodeView info is read into a global handle (should be fixed so we only
    save info we need)
    
    5     1/25/95 6:49p Bob
    added code for DebugPrintf
    
    4     1/25/95 5:01p Bob
    fixed file headers
 *
 *
 ****************************************************************************
 */

#if defined(_WINDOWS)

#include "tpsdefs.H"
#include "stackcra.h"
#include "dbdevice.h"
#include "dbgglbl.h"

#ifdef WIN32
	#include <stdarg.h>
#endif

//
// Public API
//

/*--------------------------------------------------------------------------
 * Function: _tdb_InitDebugDevice
 * Expects:  nothing
 * Returns:  BOOLEAN bSuccess
 * Purpose:  Allocates the memory needed to get & display a stack crawl
 *           Must be called before the stack crawl function is used.
 *           It moves memory. - We could pass in a depth parameter to determine
 *           how much memory gets allocated. 
 * Globals:  g_tdb_hSymList -- allocated and initialized if successful
 *--------------------------------------------------------------------------
 */
TDBAPI( BOOLEAN ) _tdb_InitDebugDevice( DEBUGPARMS FAR *lpDebugParms, uLONG ulFlags )
{
	return TRUE;
}


/*--------------------------------------------------------------------------
 * Function: _tdb_DeinitDebugDevice
 * Expects:  nothing
 * Returns:  nothing
 * Purpose:  This routine disposes of the memory allocated by
 *           _tdb_InitStackCrawl.
 *           Needs to be called when your thru with the StackCrawl.
 * Globals:  g_tdb_hSymList - free'd and reset to NULL
 *--------------------------------------------------------------------------
 */
TDBAPI( VOID ) _tdb_DeinitDebugDevice( void )
{
}


/*--------------------------------------------------------------------------
 * Function: _tdb_AuxPrintf
 * Expects:  
 * Returns:  nothing
 * Purpose:  Print formatted string to aux.
 *--------------------------------------------------------------------------
 */
TPSAPI_DECLC( void )	_tdb_AuxPrintf(char FAR *fmt, ... )
{
#ifdef WIN32
	va_list argList;
	va_start( argList, fmt );

	if (wvsprintf(g_tdb_szAssertBuf, (LPCSTR)fmt, argList ) )
	{
		OutputDebugString(g_tdb_szAssertBuf);
	}
	return;
#else
	if (wvsprintf(g_tdb_szAssertBuf, (LPCSTR)fmt, ((LPCSTR FAR *)&fmt)+1))
	{
		OutputDebugString(g_tdb_szAssertBuf);
	}
	return;
#endif	// WIN32
}


/*--------------------------------------------------------------------------
 * Function: _tdb_AuxDump
 * Expects:  
 * Returns:  nothing
 * Purpose:  
 *--------------------------------------------------------------------------
 */
TPSAPI_DECLP( void )	_tdb_AuxDump(char FAR *fmt, void FAR *ptr, long count)
{
	return;
}


#endif /* _WINDOWS */
