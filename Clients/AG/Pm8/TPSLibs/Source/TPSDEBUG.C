/****************************************************************************
 *
 *  $Header: /Development/TPSLibs/Source/TPSDEBUG.C 1     11/06/96 7:22a Mike_houle $
 *
 *  Module Name: TPSDEBUG.C
 *  Description: Platform Independent Debug/Assertion Library
 *  Version:     v0.00
 *  Author:      
 *  Date:        
 *
 *  Copyright (c) 1994, 1995 by Turning Point Software, Inc.  All Rights Reserved.
 *
 *  $Log: /Development/TPSLibs/Source/TPSDEBUG.C $
 * 
 * 1     11/06/96 7:22a Mike_houle
 * 
 * 2     9/23/96 4:55 PM Richh
 * fixed a warning that had been bugging me
    
    11    2/28/95 5:49p Bob
    cleaned up use of global variables
    fixed potential crash of mouse/keyboard input
    no longer using SYM file for Win stack crawl; now strictly CodeView
    CodeView info is read into a global handle (should be fixed so we only
    save info we need)
    
    10    1/25/95 6:49p Bob
    added code for DebugPrintf
    
    9     1/25/95 5:01p Bob
    fixed file headers
 *
 *
 ****************************************************************************
 */

#ifdef	TPSDEBUG

/*
 *  Platform independent includes...
 */
#include "tpsdefs.h"
#include "stackcra.h"
#include "dbdevice.h"
#include "dbgdlg.h"
#include "dbgglbl.h"

/*
 *  Platform dependent includes...
 */
#if	  defined( MAC )
	#include <TextUtils.h>
	#include <string.h>
#elif defined( _WINDOWS )
#elif defined( DOS )
	#include <stdio.h>
	#include <malloc.h>
#else
	#error	"MAC, _WINDOWS or DOS not specified"
#endif


/*
 *  Platform dependent statics...
 */
 
 
/*
 *  Platform independent API...
 */

/*--------------------------------------------------------------------------
 * Function: _tdb_StartupDebugLib
 * Expects:  DEBUGPARMS FAR *   lpvPlatformParms
 *           uLONG              ulSystemFlags
 * Returns:  BOOLEAN            bSuccessful    TRUE if successful else FALSE
 * Purpose:  startup the debug library.  Each platform has its own
 *           requirements for the startup parms, and so applications should
 *           initialize the platform specific structure and pass it in
 *           to this function.  The platform specific implementation of
 *           this function will deal with it as necessary.  The system
 *           flags currently only specify the memory model, which should be
 *           kDbgFlatModel for systems such as the Macintosh, where there
 *           is no such concept.
 *--------------------------------------------------------------------------
 */
TDBAPI( BOOLEAN )  _tdb_StartupDebugLib( DEBUGPARMS FAR * lpvPlatformParms, uLONG ulSystemFlags )
{
	if( !g_tdb_ulInitialized )
		{
		/*
		 * Initialize the Assertion Failed dialog
		 */
		if( !_tdb_InitAssertDlg(lpvPlatformParms,ulSystemFlags) )
			return FALSE;

#ifndef	WIN32
		/*
		 * Initialize the stack tracer
		 */
		if( !_tdb_InitStackCrawl(lpvPlatformParms,ulSystemFlags) )
			{
			_tdb_DeinitAssertDlg();
			return FALSE;
			}
#endif

		/*
		 * Initialize the Debug Output Device
		 */
		if( !_tdb_InitDebugDevice(lpvPlatformParms,ulSystemFlags) )
			{
#ifndef	WIN32
			_tdb_DeinitStackCrawl();
#endif
			_tdb_DeinitAssertDlg();
			return FALSE;
			}
		}

	/*
	 * Success!
	 * Bump the initialized count
	 */
	++g_tdb_ulInitialized;
	return TRUE;
}


/*--------------------------------------------------------------------------
 * Function: tdb_ShutdownDebugLib
 * Expects:  nothing
 * Returns:  nothing
 * Purpose:  
 * History:  10/28/94  MGD - Initial Revision
 *--------------------------------------------------------------------------
 */
TDBAPI( VOID ) tdb_ShutdownDebugLib( VOID )
{
	if( g_tdb_ulInitialized )
		{
		if( !--g_tdb_ulInitialized )

			{
			/* Really shut it down...
			 */
			_tdb_DeinitDebugDevice();
#ifndef	WIN32
			_tdb_DeinitStackCrawl();
#endif
			_tdb_DeinitAssertDlg();
			}
		}
}


/*--------------------------------------------------------------------------
 * Function: tdb_GetDebugFlags
 * Expects:  nothing
 * Returns:  uLONG       ulFlags
 * Purpose:  return the current debug settings
 *--------------------------------------------------------------------------
 */
TDBAPI( uLONG ) tdb_GetDebugFlags( VOID )

{
	return g_tdb_ulFlags;
}

/*--------------------------------------------------------------------------
 * Function: tdb_SetDebugFlags
 * Expects:  uLONG       ulNewFlags
 * Returns:  uLONG       ulOldFlags
 * Purpose:  set the debug flags, and return the old settings
 *--------------------------------------------------------------------------
 */
TDBAPI( uLONG ) tdb_SetDebugFlags( uLONG ulNewFlags )
{
	uLONG	ulOldFlags = g_tdb_ulFlags;
	g_tdb_ulFlags = ulNewFlags;
	return ulOldFlags;
}



/*--------------------------------------------------------------------------
 * Function: tdb_ClearDebugFlags
 * Expects:  uLONG       ulFlagsToClear
 * Returns:  uLONG       ulPreviousSettings
 * Purpose:  clear the specified flags, and return the previous settings
 *           of those flags.
 *--------------------------------------------------------------------------
 */
TDBAPI( uLONG ) tdb_ClearDebugFlags( uLONG ulFlagsToClear )
{
	uLONG	ulOldFlags = g_tdb_ulFlags;
	g_tdb_ulFlags &= ~(ulFlagsToClear);
	return ulOldFlags;
}


/*--------------------------------------------------------------------------
 * Function: tdb_ToggleDebugFlags
 * Expects:  uLONG       ulFlagsToToggle
 * Returns:  uLONG       ulNewFlagStates
 * Purpose:  toggle the specified flags, and return the new states of
 *           those flags
 *--------------------------------------------------------------------------
 */
TDBAPI( uLONG ) tdb_ToggleDebugFlags( uLONG ulFlagsToToggle )
{
	g_tdb_ulFlags ^= ulFlagsToToggle;
	return tdb_GetDebugFlags();
}




/*--------------------------------------------------------------------------
 * Function: _tdb_AssertProc
 * Expects:  char FAR *  lpszFailedTest
 *           char FAR *  lpszTestExplanation
 *           char FAR *  lpszFileContainingText
 *           uLONG       ulSourceLineOfTest
 *           void FAR *  lpvUserDataToDump
 *           uLONG       ulDataTypeAndFormat
 *           uLONG       ulDataCountToDump
 * Returns:  nothing
 * Purpose:  Display a message box with the title "Assert Failed: <lpszTestFailed>",
 *           a single line of "Reason: <lpszTestExplanation>"
 *           and optionally a dump of user data (if lpvUserDataToDump is NULL, or
 *           ulDataCountToDump is 0, then no data is displayed).
 *           The message box contains a stack dump (if implemented on the
 *           current platform), and options to Ignore the assertion, Break into
 *           the debugger, or terminate the application.  There is also a checkbox
 *           to allow the disabling of assertions from that point on.
 *           If assertions are currently disabled the message box will not be
 *           displayed, but if the current platform has the functionality implemented
 *           the file, lineno, and explanation will be dumped to the debug terminal
 *           or file.
 *--------------------------------------------------------------------------
 */
TDBAPI( BOOLEAN ) _tdb_AssertProc( LPSZ lpszFailedTest, LPSZ lpszReason, LPSZ lpszFilename,
										uLONG ulLineNo, LPVOID lpvData, uLONG ulFormat, uLONG ulCount )
{
	int	iUserSelection;

	/*
	 * Can't do assertion dialog if we haven't been initialized!
	 */
	if( !g_tdb_ulInitialized )
		return TRUE;


	if( g_tdb_ulFlags & kDbgAssertDisabled )
		{
		char	szTmp1[ 200 ];
		DEBUGSTRING( _fmt_, "%s:%ld  %s\n" )

#if		defined( MAC )
		char*				p = szTmp1;
		unsigned char	szNum[ 12 ];
		int				length;

//		Do Nothing for the macintosh yet...  We don't want to have to include stdlib.
//		sprintf(szTmp1,_fmt_,lpszFilename,(long)ulLineNo,lpszReason);
//		You don't have to include stdlib, dummy - RAH
		length = strlen( lpszFilename );
		BlockMove( lpszFilename, p, length );
		p += length;
		*p = ':';
		p++;
		NumToString( ulLineNo, szNum );
		p2cstr( szNum );
		length = strlen( (Ptr)szNum );
		BlockMove( szNum, p, length );
		p += length;
		*p = ' ';
		p++;
		length = strlen( lpszReason );
		BlockMove( lpszReason, p, length );
		p += length;
		*p = 0;
		_tdb_MsgOut( szTmp1 );

#elif	defined( _WINDOWS )

		wsprintf(szTmp1,_fmt_,lpszFilename,(long)ulLineNo,lpszReason);
		_tdb_MsgOut( szTmp1 );

#elif	defined( DOS )

		sprintf(szTmp1,_fmt_,lpszFilename,(long)ulLineNo,lpszReason);
		_tdb_MsgOut( szTmp1 );

#endif	// MAC|_WINDOWS|DOS

		return TRUE;
		}

	/*
	 * Build the call stack.
	 * Places a series of nul terminated strings in the _tdb_StackStr global.
	 * List is ended by an empty string (two nuls in a row)
	 */
#ifndef	WIN32
	_tdb_DoStackCrawl();
#endif
	iUserSelection = _tdb_DoAssertDlg( lpszFailedTest, lpszReason, lpszFilename, ulLineNo, ulCount, lpvData, ulFormat );

	/*
	 * Act on user selection from dialog...
	 */
	switch( iUserSelection )
		{
		case ID_TERMINATE:
			/*
			 * Terminate application
			 */							   
			MacCode( ExitToShell(); )
			WinCode( TerminateProcess( GetCurrentProcess( ), 666 ); ) //TerminateApp(GetCurrentTask(),NO_UAE_BOX); )
			DosCode( exit(-1); )
			break;

		case ID_DEBUGGER:
			return FALSE;
		}

	return TRUE;
}

#endif	//	TPSDEBUG



