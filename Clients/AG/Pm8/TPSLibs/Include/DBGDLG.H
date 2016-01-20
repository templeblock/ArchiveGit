/****************************************************************************
 *
 *  $Header: /PM8/TPSLibs/Include/DBGDLG.H 1     3/03/99 6:35p Gbeddow $
 *
 *  Header Name: DBGDLG.H
 *  Description: Header file for debug dialog
 *  Version:     v0.00
 *  Author:      
 *  Date:        
 *
 *  Copyright (c) 1994, 1995 by Turning Point Software, Inc.  All Rights Reserved.
 *
 *  $Log: /PM8/TPSLibs/Include/DBGDLG.H $
// 
// 1     3/03/99 6:35p Gbeddow
 * 
 * 1     11/06/96 7:21a Mike_houle
    
    4     1/25/95 5:01p Bob
    fixed file headers
 *
 *
 ****************************************************************************
 */

#ifndef _DBGDLG_
#define _DBGDLG_

/*
 * Constants
 */
#define ID_OK					100
#define ID_TERMINATE			101
#define ID_DEBUGGER			102
#define ID_DISABLEASSERTS	103
#define ID_DISPLAYASHEX		104

/*
 * Prototypes
 */
TDBAPI(BOOLEAN)	_tdb_InitAssertDlg( DEBUGPARMS FAR* lpvPlatformParms, uLONG ulSystemFlags );
TDBAPI(VOID)		_tdb_DeinitAssertDlg( VOID );
TDBAPI(int)			_tdb_DoAssertDlg( LPSZ lpszTest, LPSZ lpszReason, LPSZ lpszFilename,
												uLONG ulLineNo, uLONG ulCount, LPVOID lpvData, uLONG ulFormat );

#endif /* DBGDLG */
