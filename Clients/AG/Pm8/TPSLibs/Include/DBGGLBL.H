/****************************************************************************
 *
 *  $Header: /PM8/TPSLibs/Include/DBGGLBL.H 1     3/03/99 6:35p Gbeddow $
 *
 *  Module Name: DBGGLBL.H
 *  Description: TPS Debug/Assertion Manager global data declarations.
 *  Version:     v0.00
 *  Author:      Robert L. Pyron
 *  Date:        2/22/95
 *
 *  Copyright (c) 1995 by Turning Point Software, Inc.  All Rights Reserved.
 *
 *  $Log: /PM8/TPSLibs/Include/DBGGLBL.H $
// 
// 1     3/03/99 6:35p Gbeddow
// 
// 4     12/09/98 1:17p Lwilson
// Converted all references from WIN to _WINDOWS.
 * 
 * 1     11/06/96 7:21a Mike_houle
    
    1     3/01/95 3:57p Bob
    
 *
 *

 ****************************************************************************
 */

#ifndef	_DBGGLBL_
#define	_DBGGLBL_

#include "tpsdefs.h"
#include "stackcra.h"

#define	CBASSERTBUF		1024

/*---------------------------------------------------------------------------
 * PLATFORM-DEPENDENT GLOBAL VARIABLES
 *---------------------------------------------------------------------------
 */

#if   defined( MAC )

#elif defined( _WINDOWS )

	//
	// Miscellaneous globals.
	//
	uLONG			g_tdb_DebugFlags;
	HWND			g_tdb_hwndApp;
	HINSTANCE	g_tdb_hinstApp;
	HGLOBAL		g_tdb_hStackData;
	//
	// Temporary output buffer used by _DebugPrintf (and other routines).
	//
	char			g_tdb_szAssertBuf[ CBASSERTBUF ];


#elif defined( DOS )

#else

	#error	"MAC, _WINDOWS or DOS not specified"

#endif	//	MAC, _WINDOWS, or DOS


/*---------------------------------------------------------------------------
 * CROSS-PLATFORM GLOBAL VARIABLES
 *---------------------------------------------------------------------------
 */

extern uLONG		g_tdb_ulFlags;
extern uLONG		g_tdb_ulInitialized;
extern HSYMLIST	g_tdb_hSymList;


#endif	//_DBGGLBL_


