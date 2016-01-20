/*=======================================================================*\

	FRMMEM.C - Memory Management routines.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include <windows.h>
#include <windowsx.h>
#include "framelib.h"
#include "macros.h"

#ifndef WIN32
#define IsPmode         ( GetWinFlags() & WF_PMODE ? YES : NO )
#define IsEnchanted     ( GetWinFlags() & WF_ENHANCED ? YES : NO )
#define Is286           ( GetWinFlags() & WF_CPU286 ? YES : NO )
#endif

/************************************************************************/
LPTR Alloc( long lCount )
/************************************************************************/
{
return( AllocX( lCount, 0 ) );
}

/************************************************************************/
LPTR AllocX( long lCount, WORD wFlags )
/************************************************************************/
{
LPTR     lpMemory;
HGLOBAL  hMem;

// Round up to the next 16 byte boundary
lCount++; // Protection against a possible C7.0 bug
lCount = ((lCount + 15) / 16) * 16;

// Allocate the memory from the global heap - NOT limited to 64K
hMem = GlobalAlloc(GMEM_MOVEABLE | wFlags, lCount);
if (!hMem)
	return(NULL);

lpMemory = (LPTR)GlobalLock(hMem);
if (!lpMemory)
	GlobalFree(hMem);

return( lpMemory );
}

/************************************************************************/
LPTR AllocExtend( LPTR lpMemory, long lCount )
/************************************************************************/
{
LPTR lpNewMemory;
HGLOBAL hMem, hNewMem;

// Round up to the next 16 byte boundary
lCount = ((lCount + 15) / 16) * 16;

hMem = GlobalPtrHandle(lpMemory);
hNewMem = GlobalReAlloc(hMem, lCount, GMEM_MOVEABLE);
if (!hNewMem)
	return(NULL);

lpNewMemory = (LPTR)GlobalLock(hNewMem);
if (!lpNewMemory)
	GlobalFree(hNewMem);

return( lpNewMemory );
}

/************************************************************************/
void FreeUp( LPVOID lpMemory )
/************************************************************************/
{
if ( !lpMemory )
	return;
GlobalFreePtr(lpMemory);
}

/************************************************************************/
DWORD GlobalPtrSize( LPVOID lpMemory )
/************************************************************************/
{
if (!lpMemory)
	return(0);
else
	return(GlobalSize(GlobalPtrHandle(lpMemory)));
}

/************************************************************************/
DWORD AvailableMemory()
/************************************************************************/
{
#ifdef WIN32
MEMORYSTATUS ms;

GlobalMemoryStatus(&ms);
return(ms.dwAvailVirtual);
#else
return( GetFreeSpace(0) );
#endif
}


/************************************************************************/
BOOL AllocLines( LPPTR lpList, int nLines, int iWidth, int iDepth )
/************************************************************************/
{
long lCount;
LPTR lp;

if ( nLines <= 0 )
	nLines = 1;

if (!iDepth)
	iDepth = 1;

iWidth *= iDepth;
iWidth = ((iWidth + 15) / 16) * 16;
lCount = (long)nLines * iWidth;
if ( !(lp = Alloc( lCount )) )
	return( NO );
while ( --nLines >= 0 )
	{
	*lpList++ = lp;
	lp += iWidth;
	}
return( YES );
}


/************************************************************************/
LPTR GetBuffers16Kx4( LPPTR lppBuffers, BOOL bDosMemory )
/************************************************************************/
{
#ifndef WIN32
DWORD selseg;
#endif
static LPTR lpSegment, lpSelector;

if ( !lppBuffers )
	{ // Freeup the buffers and exit
	if ( lpSelector )
		{
#ifndef WIN32
		if ( lpSegment )
			GlobalDosFree( HIWORD(lpSelector) );
		else
#endif
			FreeUp( lpSelector );
		lpSelector = NULL;
		}
	return( NULL );
	}

if ( !lpSelector )
	{
#ifndef WIN32
	if ( bDosMemory )
		{ // Allocate the buffers from DOS memory
		if ( !(selseg = GlobalDosAlloc(65536L)) )
			return( NULL );
		lpSegment  = (LPTR)(selseg & 0xffff0000L);
		lpSelector = (LPTR)(selseg << 16);
		}
	else
#endif   
		 {
		lpSegment = NULL;
		if ( !(lpSelector = Alloc(65536L)) )
			return( NULL );
		}
	}

// Pass back the 4 contiguous 16K buffers
*lppBuffers++ = lpSelector;
*lppBuffers++ = lpSelector + (16384L * 1);
*lppBuffers++ = lpSelector + (16384L * 2);
*lppBuffers++ = lpSelector + (16384L * 3);
if ( lpSegment )
	return( lpSegment );
else    return( lpSelector );
}


