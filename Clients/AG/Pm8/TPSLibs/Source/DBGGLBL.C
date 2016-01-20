/****************************************************************************
 *
 *  $Header: /Development/TPSLibs/Source/DBGGLBL.C 1     11/06/96 7:22a Mike_houle $
 *
 *  Module Name: DBGGLBL.C
 *  Description: TPS Debug/Assertion Manager global data module.
 *  Version:     v0.00
 *  Author:      Michael D. Houle
 *  Date:        9/21/94
 *
 *  Copyright (c) 1994, 1995 by Turning Point Software, Inc.  All Rights Reserved.
 *
 *  $Log: /Development/TPSLibs/Source/DBGGLBL.C $
 * 
 * 1     11/06/96 7:22a Mike_houle
    
    5     2/28/95 5:49p Bob
    cleaned up use of global variables
    fixed potential crash of mouse/keyboard input
    no longer using SYM file for Win stack crawl; now strictly CodeView
    CodeView info is read into a global handle (should be fixed so we only
    save info we need)
    
    4     1/25/95 5:01p Bob
    fixed file headers
 *
 *  Revision History:
 *
 *  Date     Who      What
 *  ----     ---      ----
 *	9/19/94	MDH		Initial version.
 *
 ****************************************************************************
 */

/*---------------------------------------------------------------------------
 * INCLUDE FILES
 *---------------------------------------------------------------------------
 */

	#include "tpsdefs.h"
	#include "StackCra.h"
	#include "dbgglbl.h"

/*---------------------------------------------------------------------------
 * PLATFORM-DEPENDENT GLOBAL VARIABLES
 *---------------------------------------------------------------------------
 */

#if   defined( MAC )

#elif defined( _WINDOWS )

	//
	//
	//
	uLONG			g_tdb_DebugFlags;
	HWND			g_tdb_hwndApp		= NULL;
	HINSTANCE	g_tdb_hinstApp		= NULL;
	HGLOBAL		g_tdb_hStackData	= NULL;

	//
	// temporary output buffer used by _DebugPrintf
	//
	char	_tdb_szAssertBuf[ CBASSERTBUF ];


#elif defined( DOS )

#else

	#error	"MAC, _WINDOWS or DOS not specified"

#endif	//	MAC, _WINDOWS, or DOS


/*---------------------------------------------------------------------------
 * CROSS-PLATFORM GLOBAL VARIABLES
 *---------------------------------------------------------------------------
 */

uLONG		g_tdb_ulFlags			= (uLONG) 0;
uLONG		g_tdb_ulInitialized	= (uLONG) 0;
HSYMLIST	g_tdb_hSymList			= (HSYMLIST) NULL;




