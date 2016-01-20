/****************************************************************************
 *
 * $Header: /Development/TPSLibs/Source/WINSTACK.C 1     11/06/96 7:22a Mike_houle $
 *
 *  Module Name: WINSTACK.C
 *  Description: Stack crawl routines for TPS debug library (Windows).
 *  Version:     v0.00
 *  Author:      Robert L. Pyron
 *  Client:      Turning Point Software
 *               One Gateway Center, Suite 800
 *               Newton, MA 02158
 *               (617) 332-0202
 *
 * Copyright (C) 1994, 1995 Turning Point Software, Inc. All Rights Reserved.
 *
 * Description:
 *
 *    This file started as a concatenation of three files taken from the
 *    Microsoft Developer Network CD (GETSYM.H, GETSYM.C, and GETTRACE.C
 *    from the STKTRACE application).  The text has been reformatted to 
 *    suit my own style, and extensive modifications have been made to 
 *    the code.
 *
 * $Log: /Development/TPSLibs/Source/WINSTACK.C $
 * 
 * 1     11/06/96 7:22a Mike_houle
    
    11    2/28/95 5:49p Bob
    cleaned up use of global variables
    fixed potential crash of mouse/keyboard input
    no longer using SYM file for Win stack crawl; now strictly CodeView
    CodeView info is read into a global handle (should be fixed so we only
    save info we need)
    
    10    2/22/95 10:19a Mgd
    windows requires packed structures
    
    9     2/03/95 5:50p Bob
    Added windows stack crawl from CodeView information
    
    8     1/30/95 12:53p Bob
    stack trace stuff
    
    7     1/27/95 3:00p Bob
    
    5     1/26/95 7:24p Bob
    Added actual stack-crawl code for Win. Now we just have to call it...
    
    4     1/25/95 5:01p Bob
    fixed file headers
 *
 ****************************************************************************
 */

#if defined(_WINDOWS)

#include <windows.h>
#include <windowsx.h>

#ifndef WIN32
	#include <toolhelp.h>
#endif

#include <string.h>
#include "tpsdefs.h"	// for READONLY
#include "stackcra.h"
#include "dbgglbl.h"



#define MAXSYMSIZE		256
#define MAXDIFFERENCE	0x0500

//
// Windows requires packed structures...
#pragma pack(1)

//***  Structure declarations for SEGDEF, MAPDEF, SYMDEF borrowed
//***  from the Vol. 4: Resources docs.
//***
//***  The structure members that are needed in this program are
//***  marked as USED for convenience, so that the rest can be ignored

typedef struct tagMAPDEF
{
	WORD ppNextMap;	/* paragraph pointer to next map			USED */
	BYTE bFlags;		/* symbol types								USED */
	BYTE bReserved1;
	WORD pSegEntry;	/* segment entry-point value				USED */
	WORD cConsts;		/* count of constants in map					  */
	WORD pConstDef;	/* pointer to the chain of constants		  */
	WORD cSegs;			/* count of segments in the map			USED */
	WORD ppSegDef;		/* paragraph pointer to first SEGDEF	USED */
	BYTE cbMaxSym;		/* maximum symbol-name length					  */
	BYTE cbModName;	/* length of module name						  */
} MAPDEF;


//***	Note that the wReserved1 member has been renamed as SegNum for
//***	clarity, as the docs do not explicitly say that this member is
//***	the segment number.


typedef struct tagSEGDEF
{
	WORD ppNextSeg;	/* paragraph pointer to next SEGDEF		USED */
	WORD cSymbols;		/* count of symbols							USED */
	WORD pSymDef;		/* offset to the chain of symbols		USED */
	WORD SegNum;		/* Renamed from wReserved1					USED */
	WORD wReserved2;
	WORD wReserved3;
	WORD wReserved4;
	BYTE bFlags;		/* symbol types									  */
	BYTE bReserved1;
	WORD ppLineDef;	/* offset of line-number record				  */
	BYTE bReserved2;
	BYTE bReserved3;
	BYTE cbSegName;	/* length of segment name						  */
} SEGDEF;

typedef struct tagSYMDEF
{
	WORD wSymVal;		/* symbol address or constant				USED */
	BYTE cbSymName;	/* length of symbol name					USED */
} SYMDEF;

//
// restore default packing...
#pragma pack()


//
//	Local Variables and Data
//

static char READONLY	_szUnknown[]		= "Unknown";
static char READONLY	_szSym[]				= ".SYM";
static char READONLY	_szExe[]				= ".EXE";
static char READONLY	_szEol[]				= "\r\n";

//#define	UNTABBED_FORMAT_STRINGS
#ifdef	UNTABBED_FORMAT_STRINGS
	static char READONLY	_szFmtOffset[]			= "%s + %#04x";
	static char READONLY	_szFmtModule[]			= "%-9.9s  %-40s";
	static char READONLY	_szFmtProcOffset[]	= "%-9.9s  %s  + 0x%04x";
	static char READONLY	_szFmtSegIP[]			= "%-9.9s  %04x:%04x";
	static char READONLY	_szFmtUnknown[]		= "???????  %04x:%04x";
#else		//UNTABBED_FORMAT_STRINGS
	static char READONLY	_szFmtOffset[]			= "%s\t+ %#04x";
	static char READONLY	_szFmtModule[]			= "%-9.9s\t%-40s";
	static char READONLY	_szFmtProcOffset[]	= "%-9.9s\t%s\t+ 0x%04x";
	static char READONLY	_szFmtSegIP[]			= "%-9.9s\t%04x:%04x";
	static char READONLY	_szFmtUnknown[]		= "???????\t%04x:%04x";
#endif	//UNTABBED_FORMAT_STRINGS


//
// Prototypes
//
static int     NEAR PASCAL _tdb_ValidSymFile(HFILE symfile, MAPDEF FAR *mapdef);
static int     NEAR PASCAL _tdb_GetSEGDEF(HFILE symfile, MAPDEF FAR *mapdef, SEGDEF FAR *segdef, WORD segment, LPLONG pos);
static BOOLEAN NEAR PASCAL _tdb_GetSYMDEF(	HFILE symfile,
															SEGDEF FAR *segdef,
															LONG pos,
															WORD wOffset, 
															LPSTR ProcName, WORD cbProcName,
															LPWORD pwProcOffset);
static BOOLEAN NEAR PASCAL _tdb_FindSymbol(	HINSTANCE hinst, HFILE symfile,
															WORD wSegment, WORD wOffset,
															LPSTR ProcName, WORD cbProcName,
															WORD FAR *pwProcOffset 
															);
static BOOLEAN NEAR PASCAL _tdb_GetStackTrace( VOID );



/*---------------------------------------------------------------------------
 * Function: _tdb_InitStackCrawl
 * Expects:  nothing
 * Returns:  BOOLEAN bSuccess
 * Purpose:  Allocates the memory needed to get & display a stack crawl
 *           Must be called before the stack crawl function is used.
 * Globals:  g_tdb_hStackData -- allocated and initialized if successful
 *---------------------------------------------------------------------------
 */
TDBAPI( BOOLEAN ) _tdb_InitStackCrawl( DEBUGPARMS FAR *lpDebugParms, uLONG ulFlags )
{
	g_tdb_DebugFlags		= ulFlags;

#if	0	// done by _tdb_InitAssertDlg
	if( lpDebugParms )
	{
		g_tdb_hinstApp	= lpDebugParms->hinstApp;
		g_tdb_hwndApp	= lpDebugParms->hwndApp;
	}
#endif	// done by _tdb_InitAssertDlg

	if (!g_tdb_hStackData)
	{
		g_tdb_hStackData = GlobalAlloc(GHND, 8 * 1024L);
	}

	return TRUE;
}


/*---------------------------------------------------------------------------
 * Function: _tdb_DeinitStackCrawl
 * Expects:  nothing
 * Returns:  nothing
 * Purpose:  This routine disposes of the memory allocated by
 *           _tdb_InitStackCrawl.
 *           Needs to be called when your thru with the StackCrawl.
 * Globals:  g_tdb_hStackData - free'd and reset to NULL
 *---------------------------------------------------------------------------
 */
TDBAPI( VOID ) _tdb_DeinitStackCrawl( VOID )
{
	if (g_tdb_hStackData)
	{
		GlobalFree(g_tdb_hStackData);
		g_tdb_hStackData = NULL;
	}
}


/*---------------------------------------------------------------------------
 *
 * Function:	_tdb_DoStackCrawl
 *	Purpose:
 *
 *	   This routine logs stack trace info into a buffer. It uses the ToolHelp
 *	   APIs to walk the stack and the .SYM files to get the symbol names.
 *
 *	Parameters:	none
 * Returns:  	BOOLEAN bSuccess
 * Globals:
 *
 *		g_tdb_hStackData -- fills it in...
 *
 *---------------------------------------------------------------------------
 */
TDBAPI( BOOLEAN ) _tdb_DoStackCrawl( VOID )
{
	//
	// NOTE BOB -- The stack trace needs to begin from a NEAR routine.
	// Since _tdb_DoStackCrawl() is FAR by default, we pass the actual
	// work on to a routine that is explicitly declared NEAR.
	//
	return _tdb_GetStackTrace();
}


/*---------------------------------------------------------------------------
 *
 *	_tdb_ValidSymFile
 *
 *	Purpose:
 *
 *		Confirm the validity of the .SYM before proceeding further.
 *		The following checks are done:
 *		  - Contains only 16-bit symbols (bFlags) and not 32-bit symbols
 *		  - No more than 255 segments (cSegs)
 *		  - And hence the entry point segment is less than 256
 *		  - the last MAPDEF record must have a NULL for its NextMap pointer
 *
 *	Parameters:
 *		HFILE symfile
 *		MAPDEF FAR *mapdef
 *
 *	Return: (int)
 *
 *---------------------------------------------------------------------------
 */
#ifdef	USE_SYM_FILE
//bob950228	static int NEAR PASCAL _tdb_ValidSymFile(HFILE symfile, MAPDEF FAR *mapdef)
//bob950228	{
//bob950228		WORD	pLastMap;
//bob950228	
//bob950228		//
//bob950228		//	Read the MAPDEF at the beginning of the file.
//bob950228		//
//bob950228		_llseek(symfile, 0, SEEK_SET);
//bob950228		_lread(symfile, mapdef, sizeof(MAPDEF));
//bob950228	
//bob950228		if ((mapdef->bFlags) || (mapdef->cSegs > 255) || (mapdef->pSegEntry > 255))
//bob950228			return 0;
//bob950228	
//bob950228		_llseek(symfile, mapdef->ppNextMap*16L, SEEK_SET);
//bob950228		_lread(symfile, &pLastMap, sizeof(WORD));
//bob950228		if (pLastMap)
//bob950228			return 0;
//bob950228	
//bob950228		return 1;
//bob950228	
//bob950228	} //*** _tdb_ValidSymFile
#endif	// USE_SYM_FILE

/*---------------------------------------------------------------------------
 *
 *	_tdb_GetSEGDEF
 *
 *	Purpose:
 *
 *		This routine fetches the SEGDEF which contains the correct
 *		segment number that was passed, by iterating through all
 *		SEGDEFs.  The file pointer is saved for future use
 *
 *	Parameters:
 *		HFILE symfile
 *		MAPDEF FAR *mapdef
 *		SEGDEF FAR *segdef
 *		WORD segment
 *
 *	Return: (int)
 *
 *---------------------------------------------------------------------------
 */
#ifdef	USE_SYM_FILE
//bob950228	static int NEAR PASCAL _tdb_GetSEGDEF(HFILE symfile, MAPDEF FAR *mapdef, SEGDEF FAR *segdef, WORD wSegment, LPLONG pos)
//bob950228	{
//bob950228		WORD	ppSegDef = mapdef->ppSegDef;
//bob950228	
//bob950228		//
//bob950228		//	loop through all segments
//bob950228		//
//bob950228		while (mapdef->cSegs--)
//bob950228		{
//bob950228			//
//bob950228			//	The offset from the beginning of the file to the first
//bob950228			//	SEGDEF is given by multiplying ppSegDef of the MAPDEF
//bob950228			//	struct by 16.  Read the SEGDEFs at this offset.
//bob950228			//
//bob950228			(*pos) =  ppSegDef * 16L;              // save this file pointer too
//bob950228			_llseek(symfile, (*pos), SEEK_SET);
//bob950228			_lread(symfile, segdef, sizeof(SEGDEF));
//bob950228	
//bob950228			//
//bob950228			//	Check if it's the right SEGDEF.  Remember the wSegNum
//bob950228			//	field (corresponding to the wReserved1 field of SEGDEF in
//bob950228			//	the SYM file documentation) gives the segment number
//bob950228			//
//bob950228			if (segdef->SegNum == wSegment)
//bob950228				return 1;
//bob950228	
//bob950228			//	move the next SEGDEF
//bob950228			ppSegDef = segdef->ppNextSeg;
//bob950228		}
//bob950228	
//bob950228		//
//bob950228		//	If we got here, we didn't find the correct SEGDEF.
//bob950228		//
//bob950228		return 0;
//bob950228	
//bob950228	} //*** _tdb_GetSEGDEF
#endif	// USE_SYM_FILE


/*---------------------------------------------------------------------------
 *
 *	_tdb_GetSYMDEF
 *
 *	Purpose:
 *
 *		This routine looks for the nearest symbol whose offset is less
 *		than or equal to  the given offset.  Note that the SEGDEF
 *		contains the pointer to the the array which contains pointers
 *		to SYMDEFs. This offset array is used to obtain the nearest
 *		symbol. If a nearest symbol is not found it returns 0.
 *
 *	Parameters:
 *		HFILE symfile
 *		SEGDEF FAR *segdef
 *		WORD offset
 *
 *	Return: (char *)
 *
 *---------------------------------------------------------------------------
 */
#ifdef	USE_SYM_FILE
//bob950228	static BOOLEAN NEAR PASCAL _tdb_GetSYMDEF(	HFILE symfile,
//bob950228																SEGDEF FAR *segdef,
//bob950228																LONG pos,
//bob950228																WORD wOffset, 
//bob950228																LPSTR ProcName, WORD cbProcName,
//bob950228																LPWORD pwProcOffset)
//bob950228	{
//bob950228		LPWORD		pTable;
//bob950228		WORD			i, cbSymName;
//bob950228		SYMDEF		symdef;
//bob950228		BOOLEAN		rc		= FALSE;
//bob950228	
//bob950228		//
//bob950228		//	Allocate space for the offset table.
//bob950228		//
//bob950228		pTable = (LPWORD) GlobalAllocPtr( GPTR, segdef->cSymbols * sizeof(WORD) );
//bob950228		if (!pTable)
//bob950228			return FALSE;
//bob950228	
//bob950228		//
//bob950228		//	Read the offset array from the .SYM file.
//bob950228		//
//bob950228		_llseek( symfile, pos + segdef->pSymDef, SEEK_SET );
//bob950228		_lread( symfile, pTable, segdef->cSymbols * sizeof(WORD) );
//bob950228	
//bob950228		//
//bob950228		//	Read each SYMDEF using the offsets given in the offset array.
//bob950228		//
//bob950228		for (i=0; i < segdef->cSymbols; i++)
//bob950228		{
//bob950228			_llseek(symfile, pos + pTable[i], SEEK_SET);
//bob950228			_lread(symfile, &symdef, sizeof(SYMDEF));
//bob950228	
//bob950228			if (symdef.wSymVal > wOffset)
//bob950228				break;
//bob950228		}
//bob950228	
//bob950228		//
//bob950228		//	If i == 0, we did not find the symbol as the given offset is
//bob950228		//	before any symbols.  Otherwise, we just passed the symbol
//bob950228		//	that we wanted, so back up and get that SYMDEF.
//bob950228		//
//bob950228		if (i != 0)
//bob950228		{
//bob950228			if (cbProcName > 1)
//bob950228			{
//bob950228				cbSymName = min(symdef.cbSymName, cbProcName-1);
//bob950228	
//bob950228				_llseek(symfile, pos + pTable[i-1], SEEK_SET);
//bob950228				_lread(symfile, &symdef, sizeof(SYMDEF));
//bob950228				_lread(symfile, ProcName, cbSymName);
//bob950228				ProcName[ cbSymName ] = 0;
//bob950228				*pwProcOffset = wOffset - symdef.wSymVal;
//bob950228				rc = TRUE;
//bob950228			}
//bob950228		}
//bob950228	
//bob950228		GlobalFreePtr(pTable);
//bob950228	
//bob950228		return rc;
//bob950228	
//bob950228	} //*** _tdb_GetSYMDEF
#endif	// USE_SYM_FILE

/*---------------------------------------------------------------------------
 *
 *	_tdb_FindSymbol
 *
 *	Purpose:
 *
 *		Given a handle to an open SYM file and the segment:offset of the
 *		logical address, this routine finds the nearest symbol name in
 *		SYM file and retruns it in 'buffer'.  If no matching symbol found,
 *		it returns null.
 *
 *	Parameters:
 *		HFILE symfile
 *		WORD segment
 *		WORD offset
 *
 *	Return: (char *)
 *
 *---------------------------------------------------------------------------
 */
#ifdef	USE_SYM_FILE
//bob950228	static BOOLEAN NEAR PASCAL _tdb_FindSymbol(	HINSTANCE hinst, HFILE symfile,
//bob950228																WORD wSegment, WORD wOffset,
//bob950228																LPSTR ProcName, WORD cbProcName,
//bob950228																WORD FAR *pwProcOffset 
//bob950228																)
//bob950228	{
//bob950228		MAPDEF	mapdef;
//bob950228		SEGDEF	segdef;
//bob950228		long		pos;
//bob950228	
//bob950228		//
//bob950228		//	Check if it is a valid .SYM file; return if not.
//bob950228		//
//bob950228		if (!_tdb_ValidSymFile(symfile, &mapdef))
//bob950228			return FALSE;
//bob950228	
//bob950228		//
//bob950228		//	Get to the correct SEGDEF which contains the given segment number.
//bob950228		//
//bob950228		if (!_tdb_GetSEGDEF(symfile, &mapdef, &segdef, wSegment, &pos))
//bob950228			return FALSE;
//bob950228	
//bob950228		//
//bob950228		//	Get to the correct SYMDEF and use the given offset to get the
//bob950228		//	"nearest" symbol name.
//bob950228		//
//bob950228		return _tdb_GetSYMDEF(	symfile, &segdef, pos, wOffset, 
//bob950228										ProcName, cbProcName, pwProcOffset);
//bob950228	
//bob950228	
//bob950228	} //*** _tdb_FindSymbol
#endif	// USE_SYM_FILE


/*---------------------------------------------------------------------------
 *
 * Function:	_tdb_GetStackTrace
 *	Purpose:
 *
 *	   This routine logs stack trace info into a buffer. It uses the ToolHelp
 *	   APIs to walk the stack and the .SYM files to get the symbol names.
 *
 *	Parameters:	none
 * Returns:  	BOOLEAN bSuccess
 * Globals:
 *
 *		g_tdb_hStackData -- fills it in...
 *
 *---------------------------------------------------------------------------
 */
static BOOLEAN	PASCAL NEAR	_tdb_GetStackTrace( VOID )
{
	WORD					bSuccess = FALSE;		// assume failure
	WORD					wCS, wSS, wIP, wBP;
	STACKTRACEENTRY	ste;
	MODULEENTRY			me;
	char 					szModuleOld[MAX_PATH]	= "";
	char					szFileName[MAX_PATH]		= "";
	char					szFrameBuf[64];		// buffer for output text
	DWORD					lenFrameBuf;			// string length
	LPSTR					lpDataBuf	= NULL;
	DWORD					dwDataSize;				// stack buffer size
	DWORD					dwDataUsed	= 0;		// stack buffer bytes actually used
	int					framecount	= 0;
	char					szProcName[ MAXSYMSIZE ];
	WORD					wProcOffset;

#ifdef	USE_SYM_FILE
//bob950228		OFSTRUCT				of;
//bob950228		HFILE					symfile		= 0;
#endif	// USE_SYM_FILE

	//--------------------------------------------
	//	Get the register values from the previous stack frame.
	//	Note the way BP and IP are accessed.
	// NOTE BOB: This only works if the current routine is NEAR.
	//--------------------------------------------
	_asm
	{
			mov wCS, cs
			mov wSS, ss
			mov ax,  [bp]	; since we need the old value of BP
			mov wBP, ax
			mov ax, [bp+2]	; bp+2 points to the return addr or the
			mov wIP, ax		; ... IP value for a NEAR call
	}

	//--------------------------------------------
	// Make sure we have a buffer into which we can place stack info.
	//--------------------------------------------
	if (!g_tdb_hStackData)
		return FALSE;			// error

	lpDataBuf = GlobalLock(g_tdb_hStackData);
	if (!lpDataBuf)
		goto FAIL;

	dwDataSize = GlobalSize(g_tdb_hStackData);
	dwDataSize = min(dwDataSize, 0x10000L);		// don't wrap segment
	if (dwDataSize < 2)
		goto FAIL;	// failure

	lpDataBuf[0] = 0;
	lpDataBuf[1] = 0;

	//--------------------------------------------
	//	Initialize ToolHelp structures.
	//--------------------------------------------
	_fmemset( &ste, 0, sizeof(STACKTRACEENTRY) );
	ste.dwSize = sizeof(STACKTRACEENTRY);

	_fmemset( &me, 0, sizeof(me) );
	me.dwSize = sizeof(me);

	//--------------------------------------------
	//	Begin the stack walk by obtaining the first stack frame.
	//	Note that StackTraceFirst() can't be used for the current task.
	//--------------------------------------------
	if ( 0 == StackTraceCSIPFirst(&ste, wSS, wCS, wIP, wBP) )
		goto FAIL;	// failure

	//--------------------------------------------
	// Initialize CodeView search.
	//--------------------------------------------
	_tdb_InitCodeViewSearch( ste.hModule );

	//--------------------------------------------
	//	Walk the stack.
	// For each stack frame, we write an output line to szFrameBuf, then
	// append szFrameBuf to the global stackdata handle.  Each entry is
	// terminated with a NUL byte, and the completed list is terminated
	// with another NUL byte.
	//--------------------------------------------
	while ( (dwDataUsed + 1) < dwDataSize )
	{

		//--------------------------------------------
		// NOTE BOB -- This routine is always called from
		// _tdb_DoStackCrawl(). We don't want that routine
		// to appear in the list of routines.
		//--------------------------------------------
		++framecount;
		if (framecount <= 1)
			goto NEXTFRAME;

		//----------------------------------------------------
		//	Extract the .EXE or .SYM file name from the HMODULE
		//----------------------------------------------------
		if (ModuleFindHandle(&me, ste.hModule))
		{
			//--------------------------------------------
			// We may already have the correct file open.
			// If not, close the previous file, then open
			// the desired file.
			//--------------------------------------------

			if (0 == lstrcmpi((LPSTR)szModuleOld, (LPSTR)me.szModule))
			{
				// Same module, and file is already open.
			}
			else
			{
				//
				// Module change; close current file, and open another.
				//

				_fstrncpy( (LPSTR)szModuleOld, (LPSTR)me.szModule, sizeof(szModuleOld)-1 );
				szModuleOld[ sizeof(szModuleOld)-1 ] = 0;

#ifdef	USE_SYM_FILE

//bob950228					if (symfile > 0)
//bob950228						_lclose(symfile);
//bob950228					symfile = 0;
//bob950228					//
//bob950228					// Open .MAP file, in case there is no CodeView information.
//bob950228					//
//bob950228					lstrcpy((LPSTR) szFileName, (LPSTR) me.szModule);
//bob950228					lstrcat((LPSTR) szFileName, (LPSTR) _szSym);			// ".SYM"
//bob950228					symfile = OpenFile((LPCSTR) szFileName, &of, OF_READ | OF_SHARE_DENY_WRITE);

#endif	// USE_SYM_FILE

			}

			//--------------------------------------------
			// Search CodeView information for proc name.
			//--------------------------------------------

			if (_tdb_FindCodeViewSymbol( ste.hModule, NULL,
				 									ste.wSegment, ste.wIP,
													szProcName, sizeof(szProcName),
													NULL, 0, &wProcOffset ))
			{
				// _szFmtProcOffset[] = "%-9.9s\t%s\t+ 0x%04x"
				wsprintf(szFrameBuf, _szFmtProcOffset,
							(LPSTR)me.szModule, (LPSTR)szProcName, (WORD)wProcOffset);
			}

#ifdef	USE_SYM_FILE

//bob950228				//--------------------------------------------
//bob950228				// If no CV info, search mapfile for proc name.
//bob950228				//--------------------------------------------
//bob950228				else if ( (symfile > 0)
//bob950228					&& _tdb_FindSymbol( ste.hModule, symfile,
//bob950228												ste.wSegment, ste.wIP,
//bob950228												szProcName, sizeof(szProcName),
//bob950228												&wProcOffset ))
//bob950228				{
//bob950228					// _szFmtProcOffset[] = "%-9.9s\t%s\t+ 0x%04x"
//bob950228					wsprintf(szFrameBuf, _szFmtProcOffset,
//bob950228								(LPSTR)me.szModule, (LPSTR)szProcName, (WORD)wProcOffset);
//bob950228				}

#endif	// USE_SYM_FILE

			//--------------------------------------------
			// Proc name not found.
			//--------------------------------------------
			else
			{
				//	_szFmtSegIP[] = "%-9.9s\t%04x:%04x"
				wsprintf(szFrameBuf, _szFmtSegIP,
							(LPSTR)me.szModule, ste.wSegment, ste.wIP);
			}

		}
		else
		{
				// _szFmtUnknown[]	= "???????\t%04x:%04x";
				wsprintf(szFrameBuf, _szFmtUnknown,
							ste.wSegment, ste.wIP );
		}

		//--------------------------------------------
		// Clip lenFrameBuf to fit within stack buffer.
		//--------------------------------------------
		lenFrameBuf = (DWORD)lstrlen((LPSTR) szFrameBuf);
		if (lenFrameBuf+1 > sizeof(szFrameBuf))
		{
			DebugMsg( "szFrameBuf overflow...\n");
			_tdb_Break();
		}
		if (dwDataUsed + lenFrameBuf + 2 > dwDataSize)
		{
			lenFrameBuf = dwDataSize - dwDataUsed - 2;
		}

		//--------------------------------------------
		// Append szFrameBuf to lpDataBuf.
		// Append trailing NUL.
		// Update dwDataUsed.
		//--------------------------------------------
		_fmemcpy(lpDataBuf+dwDataUsed, szFrameBuf, (size_t)lenFrameBuf);
		dwDataUsed += lenFrameBuf;
		lpDataBuf[ dwDataUsed++ ] = 0;

	NEXTFRAME:

		//--------------------------------------------
		// Get more stack trace info.
		//--------------------------------------------
		if ( 0 == StackTraceNext(&ste) )
			break;

	}

	//--------------------------------------------
	// Append trailing NUL.
	//--------------------------------------------
	if (dwDataUsed < dwDataSize)
		lpDataBuf[ dwDataUsed ] = 0;

	//--------------------------------------------
	// Make absolutely sure the data buffer ends with two NULs.
	//--------------------------------------------
	lpDataBuf[ dwDataSize - 1 ] = 0;
	lpDataBuf[ dwDataSize - 2 ] = 0;

	bSuccess = TRUE;

FAIL:

#ifdef	USE_SYM_FILE
//bob950228		if (symfile > 0)
//bob950228			_lclose(symfile);
#endif	// USE_SYM_FILE

	_tdb_DeinitCodeViewSearch();

	if (lpDataBuf)
		GlobalUnlock(g_tdb_hStackData);

	return bSuccess;

}


#endif /* _WINDOWS */


