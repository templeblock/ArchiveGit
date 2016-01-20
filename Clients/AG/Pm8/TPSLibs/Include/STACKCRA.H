/****************************************************************************
 *
 *  $Header: /PM8/TPSLibs/Include/STACKCRA.H 1     3/03/99 6:35p Gbeddow $
 *
 *  Module Name: STACKCRA.H
 *  Description: Header file for stack crawl.
 *  Version:     v0.00
 *  Author:      
 *  Date:        
 *
 *  Copyright (c) 1994, 1995 by Turning Point Software, Inc.  All Rights Reserved.
 *
 *  $Log: /PM8/TPSLibs/Include/STACKCRA.H $
// 
// 1     3/03/99 6:35p Gbeddow
// 
// 4     12/09/98 1:17p Lwilson
// Converted all references from WIN to _WINDOWS.
 * 
 * 1     11/06/96 7:21a Mike_houle
    
    8     2/28/95 5:49p Bob
    cleaned up use of global variables
    fixed potential crash of mouse/keyboard input
    no longer using SYM file for Win stack crawl; now strictly CodeView
    CodeView info is read into a global handle (should be fixed so we only
    save info we need)
    
    7     2/22/95 10:19a Mgd
    windows requires packed structures
    
    6     2/03/95 5:50p Bob
    Added windows stack crawl from CodeView information
    
    5     1/26/95 7:24p Bob
    Added actual stack-crawl code for Win. No we just have to call it...
    
    4     1/25/95 5:01p Bob
    fixed file headers
 *
 *
 ****************************************************************************
 */

#ifndef _STACKCRA_
#define _STACKCRA_

/*
 * Platform independent types
 */

//
// Windows requires packed structures
//
#if defined(_WINDOWS)
#pragma pack(1)
#endif

	/*
	 * structure of the symbol list
	 */
	typedef struct tagSYMLIST
	{
		uWORD	wItemCnt;				/* # Entries */
		uWORD	padding;					/* RMS - long word (PowerPC) alignment */
		uLONG	byteCount;				/* actual # bytes used in symbols array */
		char	symbols[1];				/*	RMS - actually variable length */
	} SYMLIST, FAR* LPSYMLIST;

	MacCode( typedef LPSYMLIST*	HSYMLIST; )
	WinCode( typedef HGLOBAL		HSYMLIST; )

//
// restore default packing...
//
#if defined(_WINDOWS)
#pragma pack()
#endif

/*
 * Prototypes...
 */
	TDBAPI( BOOLEAN ) _tdb_InitStackCrawl( DEBUGPARMS FAR *lpDebugParms, uLONG ulFlags );
	TDBAPI( BOOLEAN ) _tdb_DoStackCrawl( VOID );
	TDBAPI( VOID )    _tdb_DeinitStackCrawl( VOID );



	MacCode( TDBAPI(VOID) _tdb_DisplayStackCrawl(WindowPtr window,Rect* prcDisplay,short sTop); )


	WinCode( TDBAPI( BOOLEAN )	_tdb_InitCodeViewSearch( HINSTANCE hModule ); )
	WinCode( TDBAPI( VOID )	   _tdb_DeinitCodeViewSearch( VOID ); )
	WinCode( TDBAPI( BOOLEAN )	_tdb_FindCodeViewSymbol( HINSTANCE hinst, HFILE fhExe, 
													WORD wSegment, WORD wOffset,
													LPSTR ProcName, WORD cbProcName,
													LPSTR SegmentName, WORD cbSegmentName,
													WORD FAR *pProcOffset
													); )

#endif /* _STACKCRA_ */
