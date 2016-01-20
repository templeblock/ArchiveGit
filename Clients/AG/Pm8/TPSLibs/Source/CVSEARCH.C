/****************************************************************************
 *
 * $Header: /Development/TPSLibs/Source/CVSEARCH.C 1     11/06/96 7:22a Mike_houle $
 *
 *	Module Name: cvfind.c
 *	Description: Test program -- Find specified routines from CodeView information.
 *	Version:     v0.00
 *	Author:      Robert L. Pyron
 *	Date:        950203
 *	Copyright:   (c) 1995 by Turning Point Software, Inc.  All Rights Reserved.
 *
 *	Revision History:
 *
 * $Log: /Development/TPSLibs/Source/CVSEARCH.C $
 * 
 * 1     11/06/96 7:22a Mike_houle
    
    3     2/28/95 5:49p Bob
    cleaned up use of global variables
    fixed potential crash of mouse/keyboard input
    no longer using SYM file for Win stack crawl; now strictly CodeView
    CodeView info is read into a global handle (should be fixed so we only
    save info we need)
    
    2     2/22/95 10:19a Mgd
    windows requires packed structures
    
    1     2/03/95 5:51p Bob
    find procname from CodeView information
 *
 ****************************************************************************
 */

#if defined(_WINDOWS)


/*
 * INCLUDE FILES
 */

#include <windows.h>
#include <toolhelp.h>
#include <stddef.h>		// for offsetof()
#include <memory.h>
#include <string.h>
#include "tpstypes.h"	// for READONLY
#include "codeview.h"
#include "StackCra.h"


/*
 * MACROS AND EQUATES
 */

#define		CV_OK				0
#define		CV_ERROR			-1
#define		CV_BAD_INDEX	0xFFFF

/*
 * TYPEDEFS
 */

	//
	// Structure to hold global variables for extracting 
	// CodeView information from an executable file.
	//

//
// Windows requires packed structures...
#pragma pack(1)

typedef	struct _CVFINDINFO
{
	HINSTANCE		hModule;				// instance handle for which this info applies
	HFILE				fhExe;				// file handle for which this info applies
	HGLOBAL			hCodeView;			// Cache CodeView info in memory
	DWORD				dwCodeViewLen;
	LONG           lfaBase;				// File location of symbolic info in EXE 
	EXESYMHDR      exesymhdr;			// Symbolic header 
	LONG           lfaDir;				// Location of symbolic info directories 
	DIRHDR         dirhdr;				// Directory header structure 
	LONG           lfaDirEntries;		// File location of directory entries 
	DWORD          dwDirEntryIdx;		// Current directory table entry index 
	DIRENTRY       direntry;			// Directory entry structure 
   LONG				lfaSubSection;    // File location of subsection data
	//
	// The following fields are used for scanning symbol tables.
	//
	DWORD				dwRecordOffset;	// offset of record from lfaSubSection
	SYMRECORDHDR	symrecordhdr;		// record header (size and type only)
	//
	// The following fields are ...
	//
	BOOLEAN			bMatch;
	WORD				wBestSegment;
	WORD				wBestOffset;
	BYTE				szBestSegmentName[64];
	BYTE				szBestProcName[64];
	WORD				wBestProcOffset;
	WORD				iSegName;			// index of segment name in sstSegName
}
	CVFINDINFO;


//
// restore default packing...
#pragma pack()

/*
 * GLOBAL VARIABLES
 */

static CVFINDINFO	gcv	= { 0 };		// all fields initialized to zero


/*
 * FUNCTION PROTOYPES
 */

TDBAPI( BOOLEAN )	_tdb_FindCodeViewSymbol( HINSTANCE hinst, HFILE fhExe, 
													WORD wSegment, WORD wOffset,
													LPSTR ProcName, WORD cbProcName,
													LPSTR SegmentName, WORD cbSegmentName,
													WORD FAR *pwProcOffset
													);

static	int	_SearchSymbolTable( CVFINDINFO *pcv, WORD wSegment, WORD wOffset );
static	int	_GetSegmentNameIndex( CVFINDINFO * pcv );
static	int	_GetSegmentName( CVFINDINFO * pcv );
static	int	_XRead( HFILE fhExe, long lOffset, LPVOID lpvBuffer, DWORD dwRequest );



/****************************************************************************
 *
 *	Function Name:
 *
 *	Description:
 *
 *	Parameters:
 *
 *	Return Values:
 *
 *	Revisions:
 *
 *	Notes:
 *
 ****************************************************************************
 */
TDBAPI( BOOLEAN )	_tdb_InitCodeViewSearch( HINSTANCE hModule )
{
	DWORD          dwExeLen;			// Executable file length 
	LONG				lfaExeSymStart;	// Offset in file to ExeSymStart
	EXESYMSTART    ExeSymStart;		// Symbolic information start 
	DWORD				dwPos;
	DWORD				dwRead;
	DWORD				dwCodeViewLen;
	char				szFileName[MAX_PATH]		= "";
	OFSTRUCT			of;
	HFILE				fhExe			= NULL;
	HGLOBAL			hCodeView	= NULL;
	LPVOID			pCodeView	= NULL;
	BOOLEAN			rc		= FALSE;		// return code; assume failure

	if (!hModule)
		return FALSE;

	if (hModule == gcv.hModule)
		return TRUE;

	//--------------------------------------
	// Clear out old information.
	//--------------------------------------
	_tdb_DeinitCodeViewSearch();

	//--------------------------------------
	// Get new module file name, and open file.
	//--------------------------------------
	if ( !GetModuleFileName(hModule, szFileName, sizeof(szFileName)) )
	{
		DebugMsg( "_tdb_InitCodeViewSearch: Can't get module file name.\n" );
		goto HELL;
	}
	fhExe = OpenFile((LPCSTR) szFileName, &of, OF_READ | OF_SHARE_DENY_WRITE);
	if (fhExe <= 0)
	{
		DebugMsg( "_tdb_InitCodeViewSearch: Can't open module file.\n" );
		goto HELL;
	}

	//--------------------------------------
	// Get the length of the executable file.
	//--------------------------------------
	dwExeLen = _llseek(fhExe, 0L, SEEK_END);
	if (dwExeLen > EWM_MAXEXESIZE)
	{
		DebugMsg( "_tdb_InitCodeViewSearch: Executable file exceeds 0x7FFFFFFE bytes" );
		goto HELL;
	}

	//--------------------------------------
	// Find the executable symbolic information base.
	// Read structure and check signature.
	//--------------------------------------
	lfaExeSymStart = - (LONG)sizeof(EXESYMSTART);	// sizeof() gives unsigned short

	dwPos = _llseek(fhExe, lfaExeSymStart, SEEK_END);
	if (dwPos == HFILE_ERROR)
	{
		DebugMsg( "_tdb_InitCodeViewSearch: bad seek 1.\n" );
		goto HELL;
	}

	dwRead = _hread(fhExe, &ExeSymStart, sizeof(EXESYMSTART) );
	if (dwRead != sizeof(EXESYMSTART))
	{
		DebugMsg( "_tdb_InitCodeViewSearch: bad read 1.\n" );
		goto HELL;
	}

	if ( ExeSymStart.dwSignature != EWM_VALIDNB )
	{
		DebugMsg( "_tdb_InitCodeViewSearch: Unsupported symbolic format in executable (not NB09).\n" );
		goto HELL;
	}

	//--------------------------------------
	// Allocate and lock a global handle to hold CodeView information.
	//--------------------------------------
	dwCodeViewLen = ExeSymStart.dwStartOffsetFromEOF;
	hCodeView = GlobalAlloc(GHND, dwCodeViewLen);
	if (!hCodeView)
	{
		DebugMsg( "_tdb_InitCodeViewSearch: can't allocate memory.\n" );
		goto HELL;
	}
	pCodeView = GlobalLock(hCodeView);
	if (!pCodeView)
	{
		DebugMsg( "_tdb_InitCodeViewSearch: can't lock memory.\n" );
		goto HELL;
	}

	//--------------------------------------
	// Seek to start of CodeView information, and read it in.
	//--------------------------------------
	dwPos = _llseek(fhExe, dwExeLen - dwCodeViewLen, SEEK_SET);
	if (dwPos == HFILE_ERROR)
	{
		DebugMsg( "_tdb_InitCodeViewSearch: bad seek 2.\n" );
		goto HELL;
	}

	dwRead = _hread(fhExe, pCodeView, dwCodeViewLen );
	if (dwRead != dwCodeViewLen)
	{
		DebugMsg( "_tdb_InitCodeViewSearch: bad read 2.\n" );
		goto HELL;
	}

	//--------------------------------------
	// Unlock pointer.
	//--------------------------------------
	GlobalUnlock(hCodeView);
	pCodeView = NULL;

	//--------------------------------------
	// Success. Fill in gcv.
	//--------------------------------------
	gcv.hModule			= hModule;
	gcv.fhExe			= fhExe;
	gcv.hCodeView		= hCodeView;
	gcv.dwCodeViewLen	= dwCodeViewLen;
	gcv.lfaBase			= 0;

	rc = TRUE;

HELL:

	if (!rc)
	{
		if (pCodeView)
			GlobalUnlock(hCodeView);

		if (hCodeView)
			GlobalFree(hCodeView);

		if (fhExe > 0)
			_lclose(fhExe);

		_tdb_DeinitCodeViewSearch();
	}

	return rc;
}


/****************************************************************************
 *
 *	Function Name:
 *
 *	Description:
 *
 *	Parameters:
 *
 *	Return Values:
 *
 *	Revisions:
 *
 *	Notes:
 *
 ****************************************************************************
 */
TDBAPI( VOID )	_tdb_DeinitCodeViewSearch( VOID )
{
	if (gcv.hCodeView)
	{
		GlobalFree(gcv.hCodeView);
	//	gcv.hCodeView = NULL;
	}

	if (gcv.fhExe)
	{
		_lclose(gcv.fhExe);
	//	gcv.fhExe = 0;
	}

	_fmemset( &gcv, 0, sizeof(gcv) );
}



/****************************************************************************
 *
 *	Function Name:
 *
 *	Description:
 *
 *	Parameters:
 *
 *	Return Values:
 *
 *	Revisions:
 *
 *	Notes:
 *
 ****************************************************************************
 */
TDBAPI( BOOLEAN )	_tdb_FindCodeViewSymbol( HINSTANCE hModule, HFILE fhExe, 
														WORD wSegment, WORD wOffset,
														LPSTR ProcName, WORD cbProcName,
														LPSTR SegmentName, WORD cbSegmentName,
														WORD FAR *pProcOffset
														)
{
	/*
	 * Variable definitions.
	 */
	LONG		lfaExeSymHdr;
	LONG		lfaDirHdr, lfaDirEntry;
	int		rc;

	//--------------------------------------
	// Set default return values, in case of failure.
	//--------------------------------------
	if (ProcName && cbProcName)			ProcName[0] = 0;
	if (SegmentName && cbSegmentName)	SegmentName[0] = 0;
	if (pProcOffset)							*pProcOffset = 0;

#if	1	// new code

	if ( !_tdb_InitCodeViewSearch(hModule) )
		return FALSE;

#else		// old code
//boob		if (hModule == 0)
//boob			return FALSE;
//boob	
//boob		if (fhExe <= 0)
//boob			return FALSE;
//boob	
//boob		//--------------------------------------
//boob		// Initialize CVFINDINFO structure.
//boob		//--------------------------------------
//boob	
//boob		if (hModule != gcv.hModule || fhExe != gcv.fhExe)
//boob		{
//boob		   DWORD          dwExeLen;			// Executable file length 
//boob			LONG				lfaExeSymStart;	// Offset in file to ExeSymStart
//boob			EXESYMSTART    ExeSymStart;		// Symbolic information start 
//boob			DWORD				dwPos		= 0;
//boob			DWORD				dwRead	= 0;
//boob			LONG				lfaBase;
//boob			DWORD				dwCodeViewLen;
//boob			HGLOBAL			hCodeView;
//boob			LPVOID			pCodeView;
//boob	
//boob			if (gcv.hCodeView)
//boob			{
//boob				GlobalFree(gcv.hCodeView);
//boob			//	gcv.hCodeView = NULL;
//boob			}
//boob	
//boob			_fmemset( &gcv, 0, sizeof(gcv) );
//boob	
//boob			//--------------------------------------
//boob			// Get the length of the executable file.
//boob			//--------------------------------------
//boob			dwExeLen = _llseek(fhExe, 0L, SEEK_END);
//boob			if (dwExeLen > EWM_MAXEXESIZE)
//boob			{
//boob				DebugMsg( "_tdb_SearchCodeView: Executable file exceeds 0x7FFFFFFE bytes" );
//boob				return FALSE;
//boob			}
//boob	
//boob			//--------------------------------------
//boob			// Find the executable symbolic information base.
//boob			//--------------------------------------
//boob			lfaExeSymStart = - (LONG)sizeof(EXESYMSTART);	// sizeof() gives unsigned short
//boob	
//boob			dwPos = _llseek(fhExe, lfaExeSymStart, SEEK_END);
//boob			if (dwPos == HFILE_ERROR)
//boob			{
//boob				DebugMsg( "_tdb_SearchCodeView: bad seek 1\n" );
//boob				return FALSE;
//boob			}
//boob	
//boob			dwRead = _hread(fhExe, &ExeSymStart, sizeof(EXESYMSTART) );
//boob			if (dwRead != sizeof(EXESYMSTART))
//boob			{
//boob				DebugMsg( "_tdb_SearchCodeView: bad read 1\n" );
//boob				return FALSE;
//boob			}
//boob	
//boob			if ( ExeSymStart.dwSignature != EWM_VALIDNB )
//boob			{
//boob				DebugMsg( "_tdb_SearchCodeView: Unsupported symbolic format in executable (not NB09)\n" );
//boob				return FALSE;
//boob			}
//boob	
//boob			dwCodeViewLen = ExeSymStart.dwStartOffsetFromEOF;
//boob			hCodeView = GlobalAlloc(GHND, dwCodeViewLen);
//boob			if (!hCodeView)
//boob			{
//boob				DebugMsg( "_tdb_SearchCodeView: can't allocate memory\n" );
//boob				return FALSE;
//boob			}
//boob	
//boob			pCodeView = GlobalLock(hCodeView);
//boob			if (!pCodeView)
//boob			{
//boob				DebugMsg( "_tdb_SearchCodeView: can't lock memory\n" );
//boob				GlobalFree(hCodeView);
//boob				return FALSE;
//boob			}
//boob	
//boob			lfaBase = dwExeLen - dwCodeViewLen;
//boob	
//boob			dwPos = _llseek(fhExe, lfaBase, SEEK_SET);
//boob			if (dwPos == HFILE_ERROR)
//boob			{
//boob				DebugMsg( "_tdb_SearchCodeView: bad seek 2\n" );
//boob				GlobalUnlock(hCodeView);
//boob				GlobalFree(hCodeView);
//boob				return FALSE;
//boob			}
//boob	
//boob			dwRead = _hread(fhExe, pCodeView, dwCodeViewLen );
//boob			if (dwRead != dwCodeViewLen)
//boob			{
//boob				DebugMsg( "_tdb_SearchCodeView: bad read 2\n" );
//boob				GlobalUnlock(hCodeView);
//boob				GlobalFree(hCodeView);
//boob				return FALSE;
//boob			}
//boob	
//boob			GlobalUnlock(hCodeView);
//boob	
//boob			gcv.hModule			= hModule;
//boob			gcv.fhExe			= fhExe;
//boob			gcv.hCodeView		= hCodeView;
//boob			gcv.dwCodeViewLen	= dwCodeViewLen;
//boob			gcv.lfaBase			= 0;
//boob		}
#endif	// new|old code

	//--------------------------------------
	// Initialize best-match variables.
	//--------------------------------------

	gcv.bMatch               = 0;
	gcv.wBestSegment         = 0;
	gcv.wBestOffset          = 0;
	gcv.szBestSegmentName[0] = 0;
	gcv.szBestProcName[0]    = 0;
	gcv.iSegName             = CV_BAD_INDEX;

	//--------------------------------------
	// Find the subsection information directory base.
	//--------------------------------------
	lfaExeSymHdr = gcv.lfaBase;

	rc = _XRead(	gcv.fhExe, lfaExeSymHdr, &gcv.exesymhdr, sizeof(EXESYMHDR) );
	if (rc != CV_OK)
		return FALSE;

	if ( gcv.exesymhdr.dwSignature != EWM_VALIDNB )
	{
		DebugMsg( "_tdb_SearchCodeView: Unsupported symbolic format in executable (not NB09)\n" );
		return FALSE;
	}

	gcv.lfaDir = gcv.exesymhdr.dwDirOffset;


	//--------------------------------------
	// Loop through the directories.
	//--------------------------------------
	while ( (gcv.lfaDir != 0L) && !gcv.bMatch )
	{
		//--------------------------------------
		// Read the directory header.
		//--------------------------------------
		lfaDirHdr = gcv.lfaBase + gcv.lfaDir;

		rc = _XRead(	gcv.fhExe, lfaDirHdr, &gcv.dirhdr, sizeof(DIRHDR) );
		if (rc != CV_OK)
			return FALSE;

		if ( gcv.dirhdr.wDirHdrLen != sizeof( DIRHDR ) )
		{
			DebugMsg( "_tdb_SearchCodeView: Executable file contains unsupported symbolic directory headers\n" );
			return FALSE;
		}

		if ( gcv.dirhdr.wDirEntryLen != sizeof( DIRENTRY ) )
		{
			DebugMsg( "_tdb_SearchCodeView: Executable file contains unsupported symbolic directory entries\n" );
			return FALSE;
		}

		gcv.lfaDirEntries = gcv.lfaBase + gcv.lfaDir + sizeof( DIRHDR );
		gcv.lfaDir = gcv.dirhdr.dwNextDirOffset;

		//--------------------------------------
		// Loop through the directory entries.
		//--------------------------------------
		for ( gcv.dwDirEntryIdx = 0; gcv.dwDirEntryIdx < gcv.dirhdr.dwDirEntries; ++gcv.dwDirEntryIdx )
		{
			//--------------------------------------
			// Read the next directory entry.
			//--------------------------------------

			lfaDirEntry = gcv.lfaDirEntries + gcv.dwDirEntryIdx * sizeof( DIRENTRY );

			rc = _XRead( gcv.fhExe, lfaDirEntry, &gcv.direntry, sizeof(DIRENTRY) );
			if (rc != CV_OK)
				return FALSE;

			gcv.lfaSubSection = gcv.lfaBase + gcv.direntry.dwSubSectionOffset;

			switch (gcv.direntry.wSst)
			{
				case sstAlignSym:
					//
					// If we have not yet found the procedure,
					// look in this symbol table.
					//
					rc = _SearchSymbolTable( &gcv, wSegment, wOffset );
					break;

				case sstSegMap:
					//
					// Extract iSegName index from this table.
					//	gcv.iSegName is an index into sstSegName.
					//
					rc = _GetSegmentNameIndex( &gcv );
					break;

				case sstSegName:
					//
					// If we have a valid iSegName, extract segment name.
					//
					_GetSegmentName( &gcv );
					break;
			}

		} // for ( dwDirEntryIdx ... 
	} // while ( lfaDir ... 

	//
	// Logical segment numbers are one-based, so if gcv.wBestSegment
	// is non-zero, we know we have a match (although it may not be
	// very good).
	//
	if (gcv.wBestSegment)
	{
		if (ProcName && cbProcName)
		{
			_fstrncpy( ProcName, gcv.szBestProcName, cbProcName-1 );
			ProcName[ cbProcName-1 ] = 0;
		}
		if (SegmentName && cbSegmentName)
		{
			_fstrncpy( SegmentName, gcv.szBestSegmentName, cbSegmentName-1 );
			SegmentName[ cbSegmentName-1 ] = 0;
		}
		if (pProcOffset)
		{
			*pProcOffset = gcv.wBestProcOffset;
		}

		return TRUE;
	}

	return FALSE;
	
}


/****************************************************************************
 *
 *	Function Name:
 *
 *	Description:
 *
 *	Parameters:
 *
 *	Return Values:
 *
 *	Revisions:
 *
 *	Notes:
 *
 ****************************************************************************
 */

static	int	_SearchSymbolTable( CVFINDINFO *pcv, WORD wSegment, WORD wOffset )
{
	HFILE				fhExe						= pcv->fhExe;
	LONG				lfaSymSegment			= pcv->lfaSubSection;
	DWORD				dwSymSegmentLength	= pcv->direntry.dwSubSectionLength;
	SYMSEGMENTHDR	SymSegmentHdr;
	LONG				lfaRecord;
	DWORD				dwRecordOffset;	// offset of record from lfaSubSection
	SYMRECORDHDR	SymRecordHdr;		// record header (size and type only)
	SYM_SSEARCH		SymSSearch;
	SYM_LPROC16		SymLProc16;
	SYM_LPROC32		SymLProc32;
	LONG				lfaProcName;
	WORD				cbProcName;
	int				rc;

	rc = _XRead( fhExe, lfaSymSegment, &SymSegmentHdr, sizeof(SYMSEGMENTHDR) );
	if (rc != CV_OK)
		return rc;

	if (SymSegmentHdr.dwSignature != EMW_VALIDSYMSEG)
	{
		DebugMsg( "_SearchSymbolTable: Invalid signature in symbol segment\n" );
		return CV_ERROR;
	}

	dwRecordOffset = sizeof( SYMSEGMENTHDR );
	while ((dwRecordOffset < dwSymSegmentLength) && !(pcv->bMatch))
	{
		lfaRecord = lfaSymSegment + dwRecordOffset;
		rc = _XRead( fhExe, lfaRecord, &(SymRecordHdr), sizeof(SYMRECORDHDR) );
		if (rc != CV_OK)
			return rc;

		switch (SymRecordHdr.wType)
		{
		case S_SSEARCH:

				rc = _XRead( fhExe, lfaRecord, &SymSSearch, sizeof(SYM_SSEARCH) );
				if (rc != CV_OK)
					return rc;

				//
				// If segment does not match, we skip all further processing
				// 	of this sstAlignSym direntry.
				// Otherwise, we go directly to the first proc entry.
				//
				if (SymSSearch.wSegment != wSegment)
					dwRecordOffset = dwSymSegmentLength;
				else if (SymSSearch.dwFirst)
					dwRecordOffset = SymSSearch.dwFirst;
				else
					dwRecordOffset += (DWORD)SymRecordHdr.wLength + sizeof(WORD);

				break;

		case S_LPROC16:
		case S_GPROC16:	

				rc = _XRead( fhExe, lfaRecord, &SymLProc16, sizeof(SYM_LPROC16) );
				if (rc != CV_OK)
					return rc;

				if (SymLProc16.wSegment == wSegment)
				{
					BOOLEAN	bRetrieve	= FALSE;

					if ( wOffset < SymLProc16.wOffset )
					{
						// keep looking
					}
					else if ( wOffset == SymLProc16.wOffset )
					{
						// exact match!
						bRetrieve = TRUE;
						pcv->bMatch = TRUE;
					}
					else if ( wOffset < (SymLProc16.wOffset + SymLProc16.wProcLength) )
					{
						// close enough!
						bRetrieve = TRUE;
						pcv->bMatch = TRUE;
					}
					else if (SymLProc16.wOffset > pcv->wBestOffset)
					{
						// It's better than what we had before.
						bRetrieve = TRUE;
					}

					if (bRetrieve)
					{
						cbProcName = (WORD)(BYTE) SymLProc16.cbName;
						cbProcName = min( cbProcName, sizeof(pcv->szBestProcName)-1 );

						lfaProcName = lfaRecord + sizeof(SYM_LPROC16);
						rc = _XRead( fhExe, lfaProcName, &pcv->szBestProcName, cbProcName );
						if (rc != CV_OK)
							return rc;

						pcv->wBestSegment = SymLProc16.wSegment;
						pcv->wBestOffset  = SymLProc16.wOffset;
						pcv->szBestProcName[ cbProcName ] = 0;
						pcv->wBestProcOffset = wOffset - SymLProc16.wOffset;

//						DebugPrintf(( "%04x:%04x  len=%i  \"%s\"\n",
//											(WORD)  pcv->wBestSegment,
//											(WORD)  pcv->wBestOffset,
//											(int)   cbProcName,
//											(LPSTR) pcv->szBestProcName
//										));
					}
				}

				if (SymLProc16.pNext)
					dwRecordOffset = SymLProc16.pNext;
				else if (SymLProc16.pEnd)
					dwRecordOffset = SymLProc16.pEnd;
				else
					dwRecordOffset += (DWORD)SymRecordHdr.wLength + sizeof(WORD);

				break;

		case S_LPROC32:
		case S_GPROC32:	

				rc = _XRead( fhExe, lfaRecord, &SymLProc32, sizeof(SYM_LPROC32) );
				if (rc != CV_OK)
					return rc;

				if (SymLProc32.wSegment == wSegment)
				{
					BOOLEAN	bRetrieve	= FALSE;

					if ( wOffset < SymLProc32.wOffset )
					{
						// keep looking
					}
					else if ( wOffset == SymLProc32.wOffset )
					{
						// exact match!
						bRetrieve = TRUE;
						pcv->bMatch = TRUE;
					}
					else if ( wOffset < (SymLProc32.wOffset + SymLProc32.dwProcLength) )
					{
						// close enough!
						bRetrieve = TRUE;
						pcv->bMatch = TRUE;
					}
					else if (SymLProc32.wOffset > pcv->wBestOffset)
					{
						// It's better than what we had before.
						bRetrieve = TRUE;
					}

					if (bRetrieve)
					{
						//
						// cbProcName is a WORD field aligned on an odd address. 
						// Since we can't be sure how the compiler is aligning
						// fields within structures, or how it computes sizeof() for
						// a structure with an odd size, we use a roundabout method
						// to extract the desired value.
						//
						lfaProcName = lfaRecord + offsetof(SYM_LPROC32,flags) + sizeof(SymLProc32.flags);
						rc = _XRead( fhExe, lfaProcName, &cbProcName, sizeof(WORD) );
						if (rc != CV_OK)
							return rc;

						lfaProcName += sizeof(WORD);
						cbProcName = min( cbProcName, sizeof(pcv->szBestProcName)-1 );

						rc = _XRead( fhExe, lfaProcName, pcv->szBestProcName, (DWORD)cbProcName );
						if (rc != CV_OK)
							return rc;

						pcv->wBestSegment = SymLProc32.wSegment;
						pcv->wBestOffset  = SymLProc32.wOffset;
						pcv->szBestProcName[ cbProcName ] = 0;
						pcv->wBestProcOffset = wOffset - SymLProc32.wOffset;
					}
				}

				if (SymLProc32.pNext)
					dwRecordOffset = SymLProc32.pNext;
				else if (SymLProc32.pEnd)
					dwRecordOffset = SymLProc32.pEnd;
				else
					dwRecordOffset += (DWORD)SymRecordHdr.wLength + sizeof(WORD);

				break;

		default:
				dwRecordOffset += (DWORD)SymRecordHdr.wLength + sizeof(WORD);
				break;
		}
	}

	return CV_OK;
}


/****************************************************************************
 *
 *	Function Name:
 *
 *	Description:
 *
 *	Parameters:
 *
 *	Return Values:
 *
 *	Revisions:
 *
 *	Notes:
 *
 ****************************************************************************
 */

static	int	_GetSegmentNameIndex( CVFINDINFO * pcv )
{
	HFILE			fhExe				= pcv->fhExe;
	LONG			lfaSubSection	= pcv->lfaSubSection;
	WORD			wSegment			= pcv->wBestSegment;
	SST_SEGMAP	SegMapHdr;
	LONG			lfaSegDesc;
	MYSEGDESC	SegDesc;
	int			rc;

	if ( pcv->direntry.wSst != sstSegMap )
		return CV_ERROR;

	if (wSegment == 0)
		return CV_ERROR;

	rc = _XRead( fhExe, lfaSubSection, &SegMapHdr, sizeof(SST_SEGMAP) );
	if (rc != CV_OK)
		return rc;

	if ( (wSegment - 1) < SegMapHdr.cSegLog)
	{
		lfaSegDesc = lfaSubSection + sizeof(SST_SEGMAP) + (wSegment - 1) * sizeof(MYSEGDESC);

		rc = _XRead( fhExe, lfaSegDesc, &SegDesc, sizeof(MYSEGDESC) );
		if (rc != CV_OK)
			return rc;

		pcv->iSegName = SegDesc.iSegName;
		return CV_OK;
	}

	return CV_ERROR;
}


/****************************************************************************
 *
 *	Function Name:
 *
 *	Description:
 *
 *	Parameters:
 *
 *	Return Values:
 *
 *	Revisions:
 *
 *	Notes:
 *
 ****************************************************************************
 */

static	int	_GetSegmentName( CVFINDINFO * pcv )
{
	HFILE			fhExe						= pcv->fhExe;
	LONG			lfaSubSection			= pcv->lfaSubSection;
	DWORD			dwSubSectionLength	= pcv->direntry.dwSubSectionLength;
	WORD			iSegName					= pcv->iSegName;
	DWORD			cbSegmentName;
	LONG			lfaSegmentName;
	int			rc;

	if ( pcv->direntry.wSst != sstSegName )
		return CV_ERROR;

	if (iSegName == CV_BAD_INDEX)
		return CV_ERROR;

	cbSegmentName = dwSubSectionLength;
	cbSegmentName = min( cbSegmentName, sizeof(pcv->szBestSegmentName)-1 );

	lfaSegmentName = lfaSubSection + iSegName;

	rc = _XRead( fhExe, lfaSegmentName, pcv->szBestSegmentName, cbSegmentName );
	if (rc != CV_OK)
		return rc;

	pcv->szBestSegmentName[ cbSegmentName ] = 0;

	return CV_OK;

}


/******************************************************************************

	Function Name: _XRead

	Description:

		Executable reading function.

	Parameters:

		fhExe                ==>   Executable file handle.
		lOffset              ==>   Offset into executable file.  
											If EWM__XReadCURRENT then continue reading
											from the current file position.
		lpvBuffer            <==   Pointer to the buffer to receive the data.
		dwBytesToRead        <==   Number of bytes to read.

	Return Values:

		Success: TRUE
		Failure: FALSE

	Revisions:

	Notes:

******************************************************************************/

static	int	_XRead( HFILE fhExe, long lOffset, LPVOID lpvBuffer, DWORD dwRequest )
{
	/*
	 * Variable definition.
	 */

	BYTE __huge *	pCodeView;

	//
	// Original version of this routine read from file,
	// and used special values for SEEK_CUR and SEEK_END.
	// Treat these as an error.
	//
	if (lOffset < 0)
	{
		DebugMsg( "_XRead: negative lOffset\n" );
		return CV_ERROR;
	}
	if (lOffset == EWM_XREADCURRENT)
	{
		DebugMsg( "_XRead: invalid lOffset\n" );
		return CV_ERROR;
	}

	//
	// Check bounds.
	//
	if ( (DWORD)lOffset + dwRequest > gcv.dwCodeViewLen )
	{
		DebugMsg( "_XRead: request goes past end of memory block.\n" );
		return CV_ERROR;
	}

	//
	// Can't copy more than 64K-1.
	//
	if (dwRequest > 0xFFFFL)
	{
		DebugMsg( "_XRead: request more than 64K.\n" );
		return CV_ERROR;
	}

	//
	// Lock CodeView data and copy to buffer.
	// Note that _fmemcpy supports huge pointers.
	//
	pCodeView = GlobalLock(gcv.hCodeView);
	if (!pCodeView)
	{
		DebugMsg( "_XRead: can't lock memory.\n" );
		return CV_ERROR;
	}

	_fmemcpy(lpvBuffer, pCodeView + lOffset, (size_t)dwRequest);

	//
	// Unlock CodeView data and return success.
	//
	GlobalUnlock(gcv.hCodeView);
	return CV_OK;
}



//===========================================================================
// WORK IN PROGRESS
//===========================================================================

#ifdef	WORK_IN_PROGRESS

typedef struct _SYMLOC
{
	//
	// Size of this structure must be a power of 2,
	// so we can use huge pointers without crossing
	// a 64K boundary.
	//
	WORD	wSegment;
	WORD	wOffset;
	DWORD	dwProcLength;
	WORD	cbProcName;
	WORD	wPad;
	LONG	Location;
} SYMLOC, FAR *LPSYMLOC;

typedef struct _SYMLOCLIST
{
	//
	// Size of this structure must be a multiple of sizeof(SYMLOC).
	//
	DWORD		lhModule;		// hModule for symbols in this list
	DWORD		lhExe;			// 
	DWORD		lhLink;
	DWORD		cSymbols;
} SYMLOCLIST, FAR *LPSYMLOCLIST;


static	int	_GetSymbols( CVFINDINFO *pcv )
{
	HFILE				fhExe						= pcv->fhExe;
	LONG				lfaSymSegment			= pcv->lfaSubSection;
	DWORD				dwSymSegmentLength	= pcv->direntry.dwSubSectionLength;
	SYMSEGMENTHDR	SymSegmentHdr;
	LONG				lfaRecord;
	DWORD				dwRecordOffset;	// offset of record from lfaSubSection
	SYMRECORDHDR	SymRecordHdr;		// record header (size and type only)
	SYM_LPROC16		SymLProc16;
	SYM_LPROC32		SymLProc32;
	LONG				lfaProcName;
	WORD				cbProcName;
	int				rc;
	SYMLOC			symloc[1];
	int				ii = 0;

	rc = _XRead( fhExe, lfaSymSegment, &SymSegmentHdr, sizeof(SYMSEGMENTHDR) );
	if (rc != CV_OK)
		return rc;

	if (SymSegmentHdr.dwSignature != EMW_VALIDSYMSEG)
	{
		DebugMsg( "_GetSymbols: Invalid signature in symbol segment\n" );
		return CV_ERROR;
	}

	dwRecordOffset = sizeof( SYMSEGMENTHDR );
	while ((dwRecordOffset < dwSymSegmentLength) && !(pcv->bMatch))
	{
		lfaRecord = lfaSymSegment + dwRecordOffset;
		rc = _XRead( fhExe, lfaRecord, &(SymRecordHdr), sizeof(SYMRECORDHDR) );

		// Bump offset before checking rc.
		dwRecordOffset += (DWORD)SymRecordHdr.wLength + sizeof(WORD);

		if (rc != CV_OK)
			continue;

		switch (SymRecordHdr.wType)
		{
		case S_LPROC16:
		case S_GPROC16:	

				rc = _XRead( fhExe, lfaRecord, &SymLProc16, sizeof(SYM_LPROC16) );
				if (rc != CV_OK)
					break;

				lfaProcName = lfaRecord + sizeof(SYM_LPROC16);

				symloc[ ii ].wSegment     = SymLProc16.wSegment;
				symloc[ ii ].wOffset      = SymLProc16.wOffset;
				symloc[ ii ].dwProcLength = (DWORD)(WORD) SymLProc16.wProcLength;
				symloc[ ii ].cbProcName   = (WORD) (BYTE) SymLProc16.cbName;
				symloc[ ii ].Location     = lfaProcName;
				++ii;
				break;

		case S_LPROC32:
		case S_GPROC32:	

				rc = _XRead( fhExe, lfaRecord, &SymLProc32, sizeof(SYM_LPROC32) );
				if (rc != CV_OK)
					break;

				//
				// SymLProc32.cbName is a WORD field aligned on an odd address. 
				// Since we can't be sure how the compiler is aligning
				// fields within structures, or how it computes sizeof() for
				// a structure with an odd size, we use a roundabout method
				// to extract the desired value.
				//
				lfaProcName = lfaRecord + offsetof(SYM_LPROC32,flags) + sizeof(SymLProc32.flags);
				rc = _XRead( fhExe, lfaProcName, &cbProcName, sizeof(WORD) );
				if (rc != CV_OK)
					return rc;

				lfaProcName += sizeof(WORD);
				cbProcName = min( cbProcName, sizeof(pcv->szBestProcName)-1 );

				symloc[ ii ].wSegment     = SymLProc32.wSegment;
				symloc[ ii ].wOffset      = SymLProc32.wOffset;
				symloc[ ii ].dwProcLength = (DWORD) SymLProc32.dwProcLength;
				symloc[ ii ].cbProcName   = (WORD)  cbProcName;
				symloc[ ii ].Location     = lfaProcName;
				++ii;
				break;

		}
	}

	return CV_OK;
}

#endif	//WORK_IN_PROGRESS


#endif /* _WINDOWS */

