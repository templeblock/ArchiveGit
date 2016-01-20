/****************************************************************************
 *
 *  $Header: /PM8/TPSLibs/Include/DBDEVICE.H 1     3/03/99 6:35p Gbeddow $
 *
 *  Header Name: DBDEVICE.H
 *  Description: Header file for debug device startup and shutdown
 *  Version:     v0.00
 *  Author:      
 *  Date:        
 *
 *  Copyright (c) 1994, 1995 by Turning Point Software, Inc.  All Rights Reserved.
 *
 *  $Log: /PM8/TPSLibs/Include/DBDEVICE.H $
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

#ifndef _DEBUGDEVICE_
#define _DEBUGDEVICE_

/*
 * Platform independent types
 */


/*
 * Prototypes...
 */
	TDBAPI(BOOLEAN)	_tdb_InitDebugDevice( DEBUGPARMS FAR *lpDebugParms, uLONG ulFlags );
	TDBAPI(BOOLEAN)	_tdb_DoStackCrawl( VOID );
	TDBAPI(VOID)	_tdb_DeinitDebugDevice( VOID );

#endif /*_DEBUGDEVICE_ */
