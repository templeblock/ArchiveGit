//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
/*=======================================================================*\

	FRMMAIN.C - Frame Management routines.

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include <windows.h>
#include <stdio.h>
#include "framelib.h"
#include "macros.h"

//int Print( LPTR lpFormat, ... );

// Static prototypes
#ifndef NO_CACHE
static long AllocLinePool ( long lMemWanted, long lMemMinimum, LPLINEPOOL lpLinePool, long lx );
static void FreeLinePool ( LPLINEPOOL lpLinePool );
static int  Age( LPFRAME lpFrame, int age );
static BOOL CacheRelease( LPFRAME lpFrame, long lSize );
static BOOL InitFreeList( LPFRAME lpFrame );
static LPTR AllocLine( LPFRAME lpFrame );
static BOOL FreeLine( LPFRAME, LPTR lpMem );
#endif
static BOOL FrameFlush( LPFRAME lpFrame );

// These are now macros
//static void AddLink( LPFRAME lpFrame, int y );
//static void RemoveLink( LPFRAME lpFrame, int y );
//static void TouchLink( LPFRAME lpFrame, int y );

/* Garbage collection size multiplier */
#define TRASH_FACTOR 12
#define NO_BACKGROUND 1

#ifndef WIN32
#define MAX_SEGS_PER_ALLOC 254
#endif

/*=========================================================================*\

	Frame Library Local Functions:
	
\*=========================================================================*/

/*
	+--->		+---------------+  <--- Frame
	|			|  Frame Data	|	sizeof(FRAME) bytes
	|			|				|
lOverheadSize0	+---------------+  <--- Frame->BackScratch
	|			|  BackScratch	|	X bytes
	|			|				|
	|			+---------------+
	|			|  LinePtr		|
	|			| (for lineart) |	X bytes
	|			|				|
	+--->		+---------------+

	+--->		+---------------+  <--- Frame->LinkList
lOverHeadSize1	|  LinkList		|	Y * sizeof(LINK) bytes
	|			|				|
	|			+---------------+  <--- Frame->HeadLink
	|			|  HeadLink		|	sizeof(LINK) bytes
	+--->		+---------------+

	+--->		+---------------+  <--- Frame->FreePool
lOverHeadSize2	|				|	Y * sizeof(LPPTR)
	|			|				|
	|			|  FreePool		|	Initially all lines in LinePool
	|			|				|	After first top to bottom op. only
	|			|				|	a few lines.
	|			|				|
	+--->		+---------------+  <+-- Frame->FreePoolEnd

	+--->		+---------------+  <--- Frame->MemList
	|			|				|	Y * sizeof(LPTR) bytes
	|			|				|
lOverHeadSize3	|  MemList		|	Memory pointers for all
	|			|				|	lines in the image.
	|			|				|
	+--->		+---------------+

	+--->		+---------------+  <--- Frame->DiskList
	|			|				|	Y * sizeof(DWORD) bytes
	|			|				|
lOverHeadSize4	|  DiskList		|	Disk pointers for all
	|			|				|	lines in the image.
	|			|				|
	+--->		+---------------+

	+--->		+---------------+  <+-- Frame->LinePool
lMemSize_0		|  LinePool_0	|	whatever's allocated
	|			|				|
	+--->		+---------------+
				.
				.
				.
	+--->		+---------------+  <+-- Frame->LinePool
lMemSize_N		|  LinePool_N	|	whatever's allocated
	|			|				|
	+--->		+---------------+
*/

/*====================================================================*/
#ifndef NO_CACHE
static long AllocLinePool(
	long       lLinesWanted,   /* Number of lines wanted */
	long       lLinesMinimum,  /* Minimum number of lines */
	LPLINEPOOL lpLinePool,
	long       lx)             /* width of a line */
{
	BOOL bLastGasp;
	long lLinesPerSeg, lSegs, lMemWanted;

	// Compute no. lines that fit in each segment, subtracting space for pointers
	// at beginning of each 64K segment.  Only really need to do it for
	// first 64K block of each allocation, but we only waste 8 bytes, so
	// who cares?. (i.e. - each 64K segment has enough room for a linepool
	// link at the beginning)
	lLinesPerSeg = (65536L-sizeof(LINEPOOL))/lx;

	#ifndef WIN32
	// don't try to allocate more than Windows has the capability
	// of handling
	lSegs = lLinesWanted / lLinesPerSeg;
	if (lSegs > MAX_SEGS_PER_ALLOC)
		lLinesWanted = MAX_SEGS_PER_ALLOC * lLinesPerSeg;
	#endif

	// pass in <= 0 if caller doesn't "need" a certain amount
	// he wants to get as much as he can
	if (lLinesMinimum <= 0)
		lLinesMinimum = lLinesPerSeg;
	if (lLinesMinimum > lLinesWanted)
		lLinesMinimum = lLinesWanted;
	if (lLinesMinimum <= 0)
		return(0L);

	bLastGasp = NO;
	while ( lLinesWanted >= lLinesMinimum )
	{
		#ifdef WIN32
		lMemWanted = (lLinesWanted * lx) + sizeof(LINEPOOL);
		#else 
		// Compute number of full segments needed
		lSegs = lLinesWanted / lLinesPerSeg;

		// Compute memory required for number of full segments needed
		if (lSegs)
			lMemWanted = lSegs * 65536L; // this includes LINEPOOL
		else
			lMemWanted = sizeof(LINEPOOL); // no full segments

		// Add in memory for a partial segment
		lMemWanted += (lLinesWanted % lLinesPerSeg) * lx;
		#endif

		lpLinePool->Next = (LPLINEPOOL)AllocX( lMemWanted, GMEM_SHARE );

		if ( lpLinePool->Next )
		{

			lpLinePool->Next->Next = NULL;

			// amount of memory minus link to next block
			lpLinePool->Next->Size = lMemWanted - sizeof(LINEPOOL);
			return( lLinesWanted );
		}

		#ifdef WIN32
		return(0L);
		#else

		// next time try half as much
		lLinesWanted /= 2;
		// Compute number of full segments needed
		lSegs = lLinesWanted / lLinesPerSeg;
		lLinesWanted = lSegs * lLinesPerSeg;

		if ( !bLastGasp && (lLinesWanted <= lLinesMinimum) )
		{
			lLinesWanted = lLinesMinimum;
			bLastGasp = YES;
		}
#endif
	}

	return( 0L );
}

#ifdef THE_RIGHT_WAY
static long AllocLinePool(
	long       lLinesWanted,   /* Number of lines wanted */
	long       lLinesMinimum,  /* Minimum number of lines */
	LPLINEPOOL lpLinePool,
	long       lx)             /* width of a line */
{
	BOOL bLastGasp;
	long lLinesPerSeg, lSegs, lMemWanted;

	// Compute no. lines that fit in each segment, subtracting space for pointers
	// at beginning of each 64K segment.  Only really need to do it for
	// first 64K block of each allocation, but we only waste 8 bytes, so
	// who cares?. (i.e. - each 64K segment has enough room for a linepool
	// link at the beginning)
	lLinesPerSeg = (65536L-sizeof(LINEPOOL))/lx;

	// don't try to allocate more than Windows has the capability
	// of handling
	lSegs = lLinesWanted / lLinesPerSeg;
	if (lSegs > MAX_SEGS_PER_ALLOC)
		lLinesWanted = MAX_SEGS_PER_ALLOC * lLinesPerSeg;

	// pass in <= 0 if caller doesn't "need" a certain amount
	// he wants to get as much as he can
	if (lLinesMinimum <= 0)
		lLinesMinimum = lLinesPerSeg;
	if (lLinesMinimum > lLinesWanted)
		lLinesMinimum = lLinesWanted;
	bLastGasp = NO;
	while ( lLinesWanted >= lLinesMinimum )
	{
		// Compute number of full segments needed
		lSegs = lLinesWanted / lLinesPerSeg;

		// Compute memory required for number of full segments needed
		if (lSegs)
			lMemWanted = lSegs * 65536L; // this includes LINEPOOL
		else
			lMemWanted = sizeof(LINEPOOL); // no full segments

		// Add in memory for a partial segment
		lMemWanted += (lLinesWanted % lLinesPerSeg) * lx;

		lpLinePool->Next = (LPLINEPOOL)AllocX( lMemWanted, GMEM_SHARE );

		if ( lpLinePool->Next )
		{

			lpLinePool->Next->Next = NULL;

			// amount of memory minus link to next block
			lpLinePool->Next->Size = lMemWanted - sizeof(LINEPOOL);
			return( lLinesWanted );
		}

//		lLinesWanted -= lLinesPerSeg;
		lLinesWanted /= 2;
		if ( !bLastGasp && (lLinesWanted <= lLinesMinimum) )
		{
			lLinesWanted = lLinesMinimum;
			bLastGasp = YES;
		}
	}

	return( 0L );
}
#endif

/*====================================================================*/

static void FreeLinePool(LPLINEPOOL lpLinePool)
{
	LPLINEPOOL lpNextLinePool;

	while (lpLinePool)
	{
		lpNextLinePool = lpLinePool->Next;
		FreeUp ((LPTR) lpLinePool);
		lpLinePool = lpNextLinePool;
	}
}

/*======================================================================*\

	lpFrame->HeadLink is the same as lpFrame->LinkList[lpFrame->Ysize]
	The HeadLink structure is used to point to the oldest and newest lines in
	the map and is considered to be the entry point of the circular list
	HeadLink->Next points to the Oldest line in the map
	HeadLink->Prev points to the Newest line in the map

\*======================================================================*/

#define OLDEST  lpFrame->HeadLink->Next
#define NEWEST  lpFrame->HeadLink->Prev

#define AddLink( AL_FRAME, AL_Y ) \
{ \
	LPLINK lpLink; \
	int ytop; \
 \
	/* If the AL_Y to be added as newest is already in the list, */ \
	/* the caller should remove it before calling this add routine */ \
 \
	ytop = NEWEST; \
 \
	/* Point the guy who is currently newest at AL_Y */ \
	lpLink = &AL_FRAME->LinkList[ ytop ]; \
 \
	/* At this point, lpLink->Next should be the last AL_Y - AL_FRAME->Ysize */ \
	if ( lpLink->Next == AL_FRAME->Ysize ) /* The newest points to the HeadLink */ \
		lpLink->Next = AL_Y; \
 \
	/* Put AL_Y on the end by pointing him back at ytop */ \
	lpLink = &AL_FRAME->LinkList[ AL_Y ]; \
	lpLink->Prev = ytop; \
	lpLink->Next = AL_FRAME->Ysize; \
 \
	/* AL_Y is now the newest */ \
	NEWEST = AL_Y; \
}

/*====================================================================*/

#define RemoveLink( RL_LPFRAME, RL_Y ) \
{ \
	LPLINK lpLink, lpLink1; \
 \
	/* If the RL_Y is in the linked list, pull it out - the caller is responsible */ \
	/* for freeing the memory before this call is made */ \
 \
	lpLink = &RL_LPFRAME->LinkList[ RL_Y ]; \
	if ( lpLink->Prev || lpLink->Next ) \
	{ \
		/* If its not an empty linkage... */ \
 \
		/* Fix RL_Y's predecessor */ \
		lpLink1 = &RL_LPFRAME->LinkList[ lpLink->Prev ]; \
 \
		/* At this point, lpLink1->Next should be RL_Y */ \
		if ( lpLink1->Next == RL_Y ) \
			lpLink1->Next = lpLink->Next; /* point him to RL_Y's next */ \
 \
		/* Fix RL_Y's descendant */ \
		lpLink1 = &RL_LPFRAME->LinkList[ lpLink->Next ]; \
 \
		/* At this point, lpLink1->Prev should be RL_Y */ \
		if ( lpLink1->Prev == RL_Y ) \
			lpLink1->Prev = lpLink->Prev; /* point him to RL_Y's prev */ \
 \
		lpLink->Next = lpLink->Prev = 0; \
	} \
}

/*====================================================================*/

#define TouchLink( TL_LPFRAME, TL_Y ) \
{ \
	/* If TL_T is already the newest, then we're done */ \
	if ( TL_Y != NEWEST ) \
	{ \
 \
		/* Take it out of the middle (if it's there)... */ \
		RemoveLink( TL_LPFRAME, TL_Y ); \
 \
		/* and add it to the end */ \
		AddLink( TL_LPFRAME, TL_Y ); \
	} \
}

/*====================================================================*/

#ifdef UNUSED

static int Age( LPFRAME lpFrame, int age )
{
	int y;
	LPLINK lpLink;

	y = NEWEST;
	while ( --age >= 0 )
	{
		if ( y < 0 || y >= lpFrame->Ysize )
			return( OLDEST );
		lpLink = &lpFrame->LinkList[ y ];

		/* If its an empty linkage... */
		if ( !lpLink->Prev && !lpLink->Next )
			return( OLDEST );

		/* Get the predecessor */
		y = lpLink->Prev;
	}

	return( y );
}

#endif // UNUSED

/*====================================================================*/

static BOOL CacheRelease( LPFRAME lpFrame, long lSize )
{
	LPTR lpMemory;
	long lDisk;
	BOOL fFlush;
	int  y;
	int  age_y;
	int  age;
	LPLINK lpLink;

	fFlush = NO;

	if (lpFrame->ReleaseMode == RELEASE_NEWEST)
	{
		while ( lSize > 0 )
		{
			// This used to be in the function Age above...
			age_y = NEWEST;
			age   = TRASH_FACTOR;

			while ( --age >= 0 )
			{
				if ( age_y < 0 || age_y >= lpFrame->Ysize )
				{
					age_y = OLDEST;
					break;
				}
				lpLink = &lpFrame->LinkList[ age_y ];

				/* If its an empty linkage... */
				if ( !lpLink->Prev && !lpLink->Next )
				{
					age_y = OLDEST;
					break;
				}

				/* Get the predecessor */
				age_y = lpLink->Prev;
			}

			y = age_y;

			if ( y < 0 || y >= lpFrame->Ysize )
				break;

			if ( !(lpMemory = lpFrame->MemList[ y ]) )
				continue;

			/* If the line has been modified, or never cached out to disk */
			/* then write it out to disk */
			lDisk = lpFrame->DiskList[ y ];
			if ( !lDisk || lDisk & MODIFIED_ON )
			{
				if ( lDisk &= MODIFIED_OFF )
					lpFrame->DiskList[ y ] = FileSeek( lpFrame->CacheFile, lDisk, 0 );
				else
				{
					lpFrame->DiskList[ y ] = FileSeek( lpFrame->CacheFile,
								((long)lpFrame->Width * (long)y)+5L, 0 );
					fFlush = YES;
				}

				if ( FileWrite( lpFrame->CacheFile, lpMemory,
					lpFrame->Width ) != lpFrame->Width)
				{
//					Print("CacheRelease - lwrite failed");
					MessageBeep(0);
					FrameSetError( ERR_SCRWRITE );
					return( FALSE );
				}
			}

			if ( !FreeLine( lpFrame, lpMemory ) )
			{
//				dbg("Freeline didn't work");
				break;
			}

			RemoveLink( lpFrame, y );
			lpFrame->MemList[ y ] = NULL;
			lSize -= lpFrame->Width;
		}
	}
	else
	{
		while ( lSize > 0 )
		{
			y = OLDEST;

			if ( y < 0 || y >= lpFrame->Ysize )
				break;

			if ( !(lpMemory = lpFrame->MemList[ y ]) )
				continue;

			/* If the line has been modified, or never cached out to disk */
			/* then write it out to disk */
			lDisk = lpFrame->DiskList[ y ];
			if ( !lDisk || lDisk & MODIFIED_ON )
			{
				if ( lDisk &= MODIFIED_OFF )
					lpFrame->DiskList[ y ] = FileSeek( lpFrame->CacheFile, lDisk, 0 );
				else
				{
					lpFrame->DiskList[ y ] = FileSeek( lpFrame->CacheFile,
									((long)lpFrame->Width * (long)y)+5L , 0 );
					fFlush = YES;
				}

				if ( FileWrite( lpFrame->CacheFile, lpMemory,
					lpFrame->Width ) != lpFrame->Width)
				{
					FrameSetError( ERR_SCRWRITE );
					return( FALSE );
				}
			}

			if ( !FreeLine( lpFrame, lpMemory ) )
			{
//				dbg("Freeline didn't work");
				break;
			}

			RemoveLink( lpFrame, y );
			lpFrame->MemList[ y ] = NULL;
			lSize -= lpFrame->Width;
		}
	}

//	if ( fFlush )
//		FrameFlush( lpFrame );

	return( TRUE );
}

/*====================================================================*/

static BOOL FreeLine( LPFRAME lpFrame, LPTR lpMem )
{
	/* Back up the pointer and make sure we are not off the top of the list */
	if ( --lpFrame->FreeNext < lpFrame->FreePool )
	{
		lpFrame->FreeNext++;
		return( FALSE );
	}
	/* Copy the address into the list */
	*(lpFrame->FreeNext) = lpMem;

	return( TRUE );
}

/*====================================================================*/

static LPTR AllocLine( LPFRAME lpFrame )
{
	LPTR lpMem;

	/* Make sure we won't be off the end of the list if we advance the pointer */
	if ( lpFrame->FreeNext >= lpFrame->FreePoolEnd )
		return( NULL );

	/* Return the pointer and then advance it */
	lpMem = *(lpFrame->FreeNext);
	lpFrame->FreeNext++;

	return( lpMem );
}

/*====================================================================*/

static BOOL InitFreeList( REG LPFRAME lpFrame )
{
	REG LPTR lp, lpNext;
	REG LPPTR lppFree;
	LPTR lpEnd;
	BOOL bAllInited;
	long x;
	LPLINEPOOL lpLinePool;

	lppFree = lpFrame->FreePool;
	lpLinePool = lpFrame->LinePool;
	lp = ((LPTR)lpLinePool) + sizeof(LINEPOOL);
	lpEnd = BumpPtr( lp, lpLinePool->Size);
	x = lpFrame->Width;

	/* Stuff line pointers into the free list */
	while ( lppFree < lpFrame->FreePoolEnd )
	{
		/* Compute the next line pointer with the right pointer arithmetic */
		/* Make sure the line at 'lp' doesn't go past the end of the pool */
		lpNext = lp;

		lpNext = BumpPtr( lpNext, x );
		if ( (HPTR)lpNext > (HPTR)lpEnd )
		{
			if ( !(lpLinePool = lpLinePool->Next) )
			    break;
			lp = ((LPTR) lpLinePool) + sizeof(LINEPOOL);
			lpEnd = BumpPtr( lp, lpLinePool->Size);
			lpNext = BumpPtr( lp, x );
		}

		/* Make sure that no pointer goes into the freelist that */
		/* would cause a line of imagery to span a 64K boundary */
		/* That way, no one needs to use huge pointer math */
		if ( !SPAN64K( lp, x ) )
		{
			*lppFree++ = lp; /* Place lp into the list */
		}
		else    
		{
			// start at the beginning of a new segment
			lpNext = (LPTR)((long)lpNext & 0xFFFF0000L);
		}

		lp = lpNext;
	}

	/* The first open slot is the top of the list */
	lpFrame->FreeNext = lpFrame->FreePool;

	bAllInited = (lppFree >= lpFrame->FreePoolEnd);

	/* Put null pointers into the rest of the list */
	while ( lppFree < lpFrame->FreePoolEnd )
		*lppFree++ = NULL;

	return(bAllInited);
}
#endif

/*====================================================================*/

static BOOL FrameFlush(LPFRAME lpFrame)
{
#ifndef NO_CACHE
	if ( lpFrame->CacheFile == FILE_HANDLE_INVALID )
		return( FALSE );

	/* Close and reopen the cache file in case of a crash */
	FileClose( lpFrame->CacheFile );

	if ( (lpFrame->CacheFile = 
		FileOpen( FrameName( lpFrame ), FO_READ|FO_WRITE )) == FILE_HANDLE_INVALID)
	{
		FrameSetError( ERR_SCROPEN );
		return( FALSE );
	}
#endif

	return( TRUE );
}

/*====================================================================*\

	Clipping routine for horizontal vectors - no check is made to 
	ensure that the points are horizontal, or that they are ordered 
	correctly.

	return:
		YES if the points are clipped out completely, 
		NO  otherwise.

\*====================================================================*/

static BOOL FrameClip( LPFRAME lpFrame, int y, LPINT left, LPINT right )
{
	/* Are they above the top? */
	if ( y < 0 )
		return( YES );

	/* Are they below the bottom? */
	if ( y >= lpFrame->Ysize )
		return( YES );

	/* Are they too far left? */
	if ( *right < 0 )
		return( YES );

	/* Are they too far right? */
	if ( *left >= lpFrame->Xsize )
		return( YES );

	/* Is the left hand clipped? */
	if ( *left < 0 )
		*left = 0;

	/* Is the right point clipped? */
	if ( *right >= lpFrame->Xsize )
		*right = lpFrame->Xsize - 1;

	return( NO );
}

/*====================================================================*/

#ifndef WIN32
static WORD GetAHINCR()
{
	HINSTANCE hKernel;
	LPTR lpVar;
	BOOL bLibin;
	static WORD _AHINCR;

	if ( _AHINCR )
		return( _AHINCR );

	if ( !(hKernel = GetModuleHandle( "KERNEL" )) )  // see if already resident
	{
		if ( !(hKernel = LoadLibrary( "KERNEL.EXE" )) ) // not resident
			return( _AHINCR = 0x1000 );
		else
			bLibin = TRUE;
	}
	else
		bLibin = FALSE;

	if ( !(lpVar = (LPTR)GetProcAddress( hKernel, "__AHINCR" )) )
		_AHINCR = 0x1000;
	else
		_AHINCR = LOWORD( lpVar );

	if ( bLibin )  // free up the library if we loaded it
		FreeLibrary( hKernel );

	return( _AHINCR );
}
#endif
/*====================================================================*/

LPTR BumpPtr( LPTR lp, DWORD lOffset )
{
#ifdef WIN32
	lp += lOffset;
#else
	DWORD BigBumps;
	DWORD HI, LO;

	LO = (DWORD)lp & 0xFFFFL;
	LO += lOffset;
	BigBumps = LO & 0xFFFF0000L;
	if ( !BigBumps )
		lp += lOffset;
	else    {
		LO &= 0xFFFFL;
		HI = (DWORD)lp & 0xFFFF0000L;
		HI += ( BigBumps * (DWORD)GetAHINCR() );
		lp = (LPTR)(DWORD)( HI + LO );
	}

#endif
	return( lp );
}

/*=========================================================================*/

#ifdef USE_CFILE
int FramePreload( LPFRAME lpFrame, CFile *pFile, BOOL invert,
	                  L3PROC   lpStatusCallback)
#else
int FramePreload( LPFRAME lpFrame, FILE_HANDLE fp, BOOL invert,
	                  L3PROC   lpStatusCallback)
#endif
{
#ifdef NO_CACHE

	int      ClockCount, ClockTotal;
	unsigned dx;
	int      LinesPerRead, LinesLeft;
	DWORD    dwTotalBytes, dwBytesPerRead;
	LPTR     lpData, lpEmpty;


// NT_HACK
//	ASSERT(lpFrame);

	dx = lpFrame->Width;
	dwTotalBytes = (DWORD)dx * (DWORD)lpFrame->Ysize;

	ClockCount = 0;
	ClockTotal = (dwTotalBytes) / 1024L;

	// read a megabyte at a time - just so we can show progress
	LinesPerRead = (1024L * 1024L) / dx;
	dwBytesPerRead = LinesPerRead * dx;
	LinesLeft = lpFrame->Ysize;
	lpData = lpFrame->ImageBase;
	while (LinesLeft)
	{
		if (LinesPerRead > LinesLeft)
		{
			LinesPerRead = LinesLeft;
			dwBytesPerRead = LinesPerRead * dx;
		}
		LinesLeft -= LinesPerRead;
		ClockCount += (dwBytesPerRead/1024L);
		if (lpStatusCallback)
			if ((*lpStatusCallback)( ClockCount, ClockTotal, YES ))
				return(-3);
		#ifdef USE_CFILE
		TRY
		{
			if ( pFile->Read( lpData, dwBytesPerRead ) != dwBytesPerRead)
				return( -1 );
		}
		CATCH_ALL(e)
		{
			return(-1);
		}
		END_CATCH_ALL
		#else
		if ( FileRead( fp, lpData, dwBytesPerRead ) != dwBytesPerRead)
			return( -1 );
		#endif
		if ( invert )
			negate( lpData, dwBytesPerRead );
		lpData += dwBytesPerRead;
	}
	LinesLeft = lpFrame->Ysize;
	lpEmpty = lpFrame->EmptyList;
	while (--LinesLeft >= 0)
		*lpEmpty++ = 1;

	return( lpFrame->Ysize );
#else
	LPTR lpMemory;
	LPTR lpHead, lpBuf;
	unsigned dx, y;
	long lCacheRemainder, lCacheStart, lCachePosition;
	WORD size;
	int ClockCount, ClockTotal;
	unsigned long ClockExtra, done;
	BOOL fit_in_memory;
	LPTR buf;

	lpBuf = Alloc(65536L);
	if (!lpBuf)
		return(0); // not an error, let normal loading do it

	// The assumption for calling this routine is that the data in the file
	// is the EXACT same format as the frame - i.e., same packing, no line 
	// padding

	dx = lpFrame->Width;

	ClockCount = 0;
	ClockTotal = lpFrame->Ysize;
	ClockExtra = 0L;

	/* Stuff as many memory pointers as we can */
	for ( y=0; y<lpFrame->Ysize; y++ )
	{
 		/* Get one of the pointers in the freelist */
		if ( !(lpFrame->MemList[y] = AllocLine(lpFrame)) )
			break;
	}

	/* If we need a cache file, stuff the disk pointers for each line */
	if ( !( fit_in_memory = (y >= lpFrame->Ysize) ) )
	{
		lCacheStart = lCachePosition =FileSeek( lpFrame->CacheFile, 0L, 2 );

		for ( y=0; y<lpFrame->Ysize; y++ )
		{
			lpFrame->DiskList[y] = lCachePosition;
			lCachePosition += dx;
		}
		lCacheRemainder = lCachePosition - lCacheStart;
	}

	y = 0;

top:
	lpHead = NULL;
	size = 0;
	while ( y < lpFrame->Ysize )
	{
		/* Get one of the pointers in the freelist */
		if ( !(lpMemory = lpFrame->MemList[y]) )
			break;
		/* If you never set the head pointer, set it now */
		if ( !lpHead )
			lpHead = lpMemory;
		if ( lpMemory != lpHead + size )
			break; /* Not a contiguous line - break out and do a read */
		if ( SPAN64K( lpHead, (size + dx) ) )
			break; /* Would be too much data to read or write */
		/* Register the memory in the most recently used list */
		AddLink( lpFrame, y );
		y++;
		/* Keep track of the contigous block size */
		size += dx;
	}

	/* Do the file read if lpHead and size are kosher */
	if ( lpHead && size )
	{
		ClockExtra += size;
		if (ClockExtra >= dx)
		{
			done = (ClockExtra/dx);
			ClockExtra -= done*dx;
			ClockCount += done;
		}
		if (lpStatusCallback)
			if ((*lpStatusCallback)( ClockCount, ClockTotal, YES ))
			{
				FreeUp(lpBuf);
				return(-3);
			}

		#ifdef USE_CFILE
		TRY
		{
			if ( pFile->Read( lpHead, size ) != size)
			{
				FreeUp(lpBuf);
				return( -1 );
			}
		}
		CATCH_ALL(e)
		{
			FreeUp(lpBuf);
			return(-1);
		}
		END_CATCH_ALL
		#else
		if ( FileRead( fp, lpHead, size ) != size)
		{
			FreeUp(lpBuf);
			return( -1 );
		}
		#endif
		if ( invert )
			negate( lpHead, (long)size );
		if ( !fit_in_memory )
		{
			if ( FileWrite( lpFrame->CacheFile, lpHead, size ) != size)
			{
				FreeUp(lpBuf);
				return( -2 );
			}
			lCacheRemainder -= size;
		}
		/* Start over again with the last allocated line */
		goto top;
	}

	if ( fit_in_memory )
	{ /* all in memory; no lines in the disk cache file */
//		FrameFlush( lpFrame );
		FreeUp(lpBuf);
		return( lpFrame->Ysize );
	}

	/* Do a file copy from the remainder of the tif file to the cache file */
	size = 65535L;
	buf = lpBuf;

	while ( lCacheRemainder > 0 )
	{
		if ( lCacheRemainder < size )
			size = lCacheRemainder;
		ClockExtra += size;
		if (ClockExtra >= dx)
		{
			done = (ClockExtra/dx);
			ClockExtra -= done*dx;
			ClockCount += done;
		}
		if (lpStatusCallback)
			if ((*lpStatusCallback)( ClockCount, ClockTotal, YES ))
			{
				FreeUp(lpBuf);
				return(-3);
			}
		#ifdef USE_CFILE
		TRY
		{
			if ( pFile->Read( buf, size ) != size )
			{
				FreeUp(lpBuf);
				return(-1);
			}
		}
		CATCH_ALL(e)
		{
			FreeUp(lpBuf);
			return(-1);
		}
		END_CATCH_ALL
		#else
		if ( FileRead( fp, buf, size ) != size )
		{
			FreeUp(lpBuf);
			return(-1);
		}
		#endif
		if ( invert )
			negate( buf, (long)size );
		if ( FileWrite( lpFrame->CacheFile, buf, size ) != size)
		{
			FreeUp(lpBuf);
			return(-2);
		}
		lCacheRemainder -= size;
	}

//	FrameFlush( lpFrame );
	FreeUp(lpBuf);
	return( lpFrame->Ysize );
#endif
}

/*=========================================================================*\

	Frame Library API Functions:
	
\*=========================================================================*/

/* local (module) Variables */
typedef struct _settings
{
#ifndef NO_CACHE
	char    RamDisk[MAX_FNAME_LEN];   // The location of the ramdisk (cache file)
	int     MainMemFactor;  // The percentage of memory to allocate
	int     CacheSeq;       // The local cache sequence number.
#endif
	LPFRAME CurrentFrame;
	int     MainMemMin;     // The minimum amount of memory to leave behind
} FRAMESETTINGS;

static FRAMESETTINGS LocalFrameSettings = {
#ifndef NO_CACHE
"c:\\", 75, 0,
#endif
NULL, 1024};


/*====================================================================*/

LPFRAME FrameGetCurrent()
{
	return(LocalFrameSettings.CurrentFrame);
}

/*====================================================================*/

LPFRAME FrameSetCurrent(LPFRAME lpFrame)
{
	LPFRAME lpOldFrame;

	lpOldFrame = LocalFrameSettings.CurrentFrame;
	LocalFrameSettings.CurrentFrame = lpFrame;

	return(lpOldFrame);
}

/*====================================================================*/

void FrameSettings(
	LPSTR   lpRamDisk,
	int    iMainMemMin,
	int    iMainMemFactor)
{

	LocalFrameSettings.MainMemMin    = iMainMemMin;

#ifndef NO_CACHE
{
#ifdef WIN32
	int iTask = (int)GetCurrentProcess();
#else
	int iTask = (int)GetCurrentTask();
#endif
	LocalFrameSettings.MainMemFactor = iMainMemFactor;
	LocalFrameSettings.CacheSeq      = 
		((iTask&0xFF00) >> 8)|((iTask & 0x00FF)<<8);

	if (lpRamDisk)
	{
		lstrcpy( LocalFrameSettings.RamDisk, lpRamDisk );
	}
	else
	{
#ifdef WIN32
		GetTempPath(sizeof(LocalFrameSettings.RamDisk),
					LocalFrameSettings.RamDisk);		
		FixPath(LocalFrameSettings.RamDisk);
#else
		LocalFrameSettings.RamDisk[0] = GetTempDrive(0);
		LocalFrameSettings.RamDisk[1] = ':';
		LocalFrameSettings.RamDisk[2] = '\\';
		LocalFrameSettings.RamDisk[3] = 0;
#endif
	}
}
#endif
}

/*====================================================================*/

static int iFrameErrorCode;

void FrameSetError( int iErrorCode )
{
	iFrameErrorCode = iErrorCode;	
}

/*====================================================================*/

int FrameGetError( void )
{
	return( iFrameErrorCode );
}

/*====================================================================*/

LPFRAME FrameOpen(
	FRMDATATYPE DataType,
	int XSize,
	int YSize, 
	int Resolution)
{
FRMTYPEINFO TypeInfo;

FrameSetTypeInfo(&TypeInfo, DataType, NULL);
return(FrameOpen(TypeInfo, XSize, YSize, Resolution));
}

LPFRAME FrameOpen(
	FRMTYPEINFO TypeInfo,
	int XSize,
	int YSize, 
	int Resolution)
{
	int Depth;
	LPFRAME lpFrame;
	LPTR lp0, lp1, lp2, lp3, lp4;
	DWORD dwOverheadSize0, dwOverheadSize1, dwOverheadSize2;
	DWORD dwOverheadSize3, dwOverheadSize4, lMemNeeded;
	DWORD lx, ly, lMemSize, lLineSize;
	DWORD lMemLines, lMinLines;
	BOOL fNoCache;
#ifndef NO_CACHE
	DWORD lLinesPerSeg, lSegs, lLines;
	DWORD dwDiskSpace;
	LINEPOOL LinePool;
	LPLINEPOOL lpLinePool;
	BYTE byte;
#endif

	FRMDATATYPE DataType;

	DataType = TypeInfo.DataType;

	// if user entered no string for RamDisk don't use our
	// virtual memory
#ifdef NO_CACHE
	fNoCache = YES;
#else
	fNoCache = LocalFrameSettings.RamDisk[0] == '\0';
#endif

	switch(DataType)
	{
		case FDT_LINEART :
			Depth = 0;
		break;

		case FDT_GRAYSCALE :
			Depth = 1;
		break;
		
		case FDT_PALETTECOLOR :
			Depth = 1;
		break;

		case FDT_RGBCOLOR :
			Depth = 3;
		break;

		case FDT_CMYKCOLOR :
			Depth = 4;
		break;
	}

	/* Make sure all the arguments are cool */
	if ( XSize <= 0 || YSize <= 0 )
		return( NULL );

	if ( Resolution <= 0 )
		Resolution = 75;

	ly = YSize;

	// Special Case Line-Art
	if ( DataType == FDT_LINEART )
	{
//		lx = ((XSize + 31) / 32) * 4;
		// make this calculation the same as in preload
		lx = (XSize + 7) / 8;		
	} 
	else
	{
		lx = XSize * Depth;
	}

	if ( ly > MAX_IMAGE_HEIGHT )
	{
		FrameSetError( ERR_MAXIMAGEHEIGHT );
		return( NULL );
	}

	if ( XSize > MAX_IMAGE_WIDTH )
	{
		FrameSetError( ERR_MAXIMAGEWIDTH );
		return( NULL );
	}

	// The overhead includes enough memory for five different allocation areas
	// 0.) the frame struct, and the background line
	// 1.) the LRU linked list (4 bytes/line)
	// 2.) the free list (4 bytes/line)
	// 3.) the memory address list (4 bytes/line)
	// 4.) the disk address list (4 bytes/line)

	if (DataType == FDT_LINEART) 
	{
		lLineSize = (lx * 8) + 4;
	}
	else
	{
		lLineSize = 0;
	}

	dwOverheadSize0 = (long)sizeof(FRAME) + lx + lLineSize;
#ifdef NO_CACHE
	dwOverheadSize0 += ly; // Empty List
	dwOverheadSize1 = 0; // LRU linked list
	dwOverheadSize2 = 0; // Free lines list
	dwOverheadSize3 = 0; // Memory address list
	dwOverheadSize4 = 0; // Disk address list
#else
	dwOverheadSize1 = (ly+1) * sizeof(LINK); // LRU linked list
	dwOverheadSize2 = ly * sizeof(LPTR); // Free lines list
	dwOverheadSize3 = ly * sizeof(LPTR); // Memory address list
	dwOverheadSize4 = ly * sizeof(DWORD); // Disk address list
#endif
	
#ifndef WIN32
	if ( dwOverheadSize1 > 65536L || dwOverheadSize2 > 65536L ||
		 dwOverheadSize3 > 65536L || dwOverheadSize4 > 65536L )
	{
		FrameSetError( ERR_IMAGEOVERHEAD );
		return( NULL );
	}
#endif

	// figure out if we have enough memory to open this image
	if (fNoCache)
		lMinLines = ly;
	else
		lMinLines = TRASH_FACTOR * 2;
	lMemNeeded = dwOverheadSize0 + dwOverheadSize1 + dwOverheadSize2 +
				dwOverheadSize3 + dwOverheadSize4 + (lMinLines * lx);

	/* Figure out how much conventional memory is available for the line pool */
	/*      less the user defined Minimum to leave free */
	lMemSize = AvailableMemory() - (1024L * LocalFrameSettings.MainMemMin);

	// If not enough memory to open, fail cause we don't want to kill Windows
	if (lMemSize < lMemNeeded)
	{
		FrameSetError( ERR_MALLOC );
		return( NULL );
	}

	/* Adjust by the user defined percentage */
	// Always divide first to prevent long word overflow
#ifndef NO_CACHE
	if (!fNoCache)
		lMemSize = (lMemSize / 100) * LocalFrameSettings.MainMemFactor;
#endif

	if ( !(lp0 = AllocX( dwOverheadSize0, GMEM_SHARE )) )
	{
		FrameSetError( ERR_MALLOC );
		return( NULL );
	}
	lclr( lp0, dwOverheadSize0 );
	lMemSize -= dwOverheadSize0;

	if (dwOverheadSize1)
	{
		if ( !(lp1 = AllocX( dwOverheadSize1, GMEM_SHARE )) )
		{
			FrameSetError( ERR_MALLOC );
			FreeUp( lp0 );
			return( NULL );
		}
		lclr( lp1, dwOverheadSize1 );
		lMemSize -= dwOverheadSize1;
  	
		if ( !(lp2 = AllocX( dwOverheadSize2, GMEM_SHARE )) )
		{
			FrameSetError( ERR_MALLOC );
			FreeUp( lp0 );
			FreeUp( lp1 );
			return( NULL );
		}
		lclr( lp2, dwOverheadSize2 );
		lMemSize -= dwOverheadSize2;

		if ( !(lp3 = AllocX( dwOverheadSize3, GMEM_SHARE )) )
		{
			FrameSetError( ERR_MALLOC );
			FreeUp( lp0 );
			FreeUp( lp1 );
			FreeUp( lp2 );
			return( NULL );
		}
		lclr( lp3, dwOverheadSize3 );
		lMemSize -= dwOverheadSize3;

		if ( !(lp4 = AllocX( dwOverheadSize4, GMEM_SHARE )) )
		{
			FrameSetError( ERR_MALLOC );
			FreeUp( lp0 );
			FreeUp( lp1 );
			FreeUp( lp2 );
			FreeUp( lp3 );
			return( NULL );
		}
		lclr( lp4, dwOverheadSize4 );
		lMemSize -= dwOverheadSize4;
	}

	// Partition overhead block #0
	lpFrame = (LPFRAME)lp0;
	lp0 += sizeof(FRAME);
	lpFrame->BackScratch = lp0;
	lp0 += lx;
	lpFrame->LinePtr = lp0;

#ifdef NO_CACHE
	lp0 += lLineSize;
	lpFrame->EmptyList = lp0;	
#else
	// Partition overhead block #1
	lpFrame->LinkList = (LPLINK)lp1;
	lp1 += ly * sizeof(LINK);
	lpFrame->HeadLink = (LPLINK)lp1; // The extra LINK structure holds...
	lpFrame->HeadLink->Next = ly; // the OLDEST y, and...
	lpFrame->HeadLink->Prev = ly; // the NEWEST y
	//lp1 += sizeof(LINK);

	// Partition overhead block #2
	lpFrame->FreePool = (LPPTR)lp2;
	lp2 += ly * sizeof(LPPTR);
	lpFrame->FreePoolEnd = (LPPTR)lp2;

	// Partition overhead block #3
	lpFrame->MemList = (LPPTR)lp3;

	// Partition overhead block #4
	lpFrame->DiskList = (LPDWORD)lp4;
#endif

	// Initialize frame structure
	lpFrame->DataType    = DataType;
	lpFrame->Depth       = Depth;
	lpFrame->RealDepth   = Depth;
	lpFrame->Resolution  = Resolution;
	lpFrame->Xsize       = XSize;
	lpFrame->Ysize       = YSize;
	lpFrame->Width       = lx;
	lpFrame->ReleaseMode = RELEASE_NEWEST;
	lpFrame->Background  = NO_BACKGROUND;
	lpFrame->CacheLine   = -1;
#ifndef NO_CACHE
	lpFrame->CacheFile   = FILE_HANDLE_INVALID;
	// Force the generation of a cache sequence number
	FrameName(lpFrame);
#endif

	if (DataType == FDT_PALETTECOLOR)
	{
		if (TypeInfo.ColorMap)
			FrameSetColorMap(lpFrame, TypeInfo.ColorMap);
		else
			lpFrame->ColorMap = FrameCreateColorMap();
		if (!lpFrame->ColorMap)
			{
			FrameSetError( ERR_MALLOC );
#ifndef NO_CACHE
			FreeUp( (LPTR)lpFrame->FreePool );
			FreeUp( (LPTR)lpFrame->MemList );
			FreeUp( (LPTR)lpFrame->DiskList );
			FreeUp( (LPTR)lpFrame->LinkList );
#endif
			FreeUp( (LPTR)lpFrame );
			return( NULL );
			}
	}
	else
	{
		lpFrame->ColorMap = NULL;
	}


	set( lpFrame->BackScratch, (int)lx, lpFrame->Background );

#ifdef WIN32 // no segment bullcrap
	lMemLines = lMemSize / lx;
#else  
	// compute how many full segments we have
  	lSegs = lMemSize / 65536L;
  	// compute how may lines fit in a segment
  	lLinesPerSeg = (65536L-sizeof(LINEPOOL))/lx;
  	// compute how many lines in all full segments
  	lMemLines = lSegs * lLinesPerSeg;
	// add in lines for partial segment
	lMemLines += (lMemSize % 65536L) / lx;
#endif

	/* Don't take more than what we need for the whole image */
	if ( lMemLines > ly )
		lMemLines = ly;
	else if (fNoCache && lMemLines < ly)
	{
		FrameSetError( ERR_LINEPOOLALLOC );
		FrameDestroyColorMap(lpFrame->ColorMap);
#ifndef NO_CACHE
		FreeUp( (LPTR)lpFrame->FreePool );
		FreeUp( (LPTR)lpFrame->MemList );
		FreeUp( (LPTR)lpFrame->DiskList );
		FreeUp( (LPTR)lpFrame->LinkList );
#endif
		FreeUp( (LPTR)lpFrame );
		return( NULL );
	}

	/* It can't go negative or be too small */
	if ( lMemLines < lMinLines )
		lMemLines = lMinLines;

#ifdef NO_CACHE
	if (!(lpFrame->ImageBase = Alloc(lMemLines * lx)))
	{
		FrameSetError( ERR_LINEPOOLALLOC );
		FrameDestroyColorMap(lpFrame->ColorMap);
		FreeUp( (LPTR)lpFrame );
		return( NULL );
	}
#else
#ifdef WIN32		 // never allocate in multiple chunks in WIN32
	lMinLines = lMemLines;
#endif

	/* Time to allocate the line pool */
	/* Get first block of memory for Line Pool */
	if ( !(lLines = AllocLinePool (lMemLines, lMinLines, &LinePool, lx)) )
	{
		FrameSetError( ERR_LINEPOOLALLOC );
		FrameDestroyColorMap(lpFrame->ColorMap);
		FreeUp( (LPTR)lpFrame->FreePool );
		FreeUp( (LPTR)lpFrame->MemList );
		FreeUp( (LPTR)lpFrame->DiskList );
		FreeUp( (LPTR)lpFrame->LinkList );
		FreeUp( (LPTR)lpFrame );
		return( NULL );
	}

	/* Get more blocks of memory if needed and available */
	lpFrame->LinePool = lpLinePool = LinePool.Next;
	lMemLines -= lLines;
	if ( lMemLines > 0)
	{
		while ( lLines = AllocLinePool( lMemLines, 0, lpLinePool, lx ) )
		{
			lpLinePool = lpLinePool->Next;
			lMemLines -= lLines;
			if (lMemLines <= 0)
				break;
		}
	}

	if (fNoCache && lMemLines > 0)
	{
		FrameSetError( ERR_LINEPOOLALLOC );
		FrameDestroyColorMap(lpFrame->ColorMap);
		FreeLinePool( lpFrame->LinePool );
		FreeUp( (LPTR)lpFrame->FreePool );
		FreeUp( (LPTR)lpFrame->MemList );
		FreeUp( (LPTR)lpFrame->DiskList );
		FreeUp( (LPTR)lpFrame->LinkList );
		FreeUp( (LPTR)lpFrame );
		return( NULL );
	}

	/* Breakup the linepool memory by loading the freelist with each pointer */
	/* Open the cache file (if we'll need it) */
	if (!InitFreeList( lpFrame ))
	{
		// Make sure we have enough disk space (if we'll need some)
		// always leave at least two megabytes for others
		dwDiskSpace = DiskSpace(LocalFrameSettings.RamDisk);
		if (dwDiskSpace <= (1024L * 1024L * 2L))
			dwDiskSpace = 0;
		else
			dwDiskSpace -= (1024L * 1024L * 2L);
		if ( ((ly * lx)+6L) > dwDiskSpace )
		{
			FrameSetError( ERR_DISKCACHE );
			FrameDestroyColorMap(lpFrame->ColorMap);
			FreeLinePool( lpFrame->LinePool );
			FreeUp( (LPTR)lpFrame->FreePool );
			FreeUp( (LPTR)lpFrame->MemList );
			FreeUp( (LPTR)lpFrame->DiskList );
			FreeUp( (LPTR)lpFrame->LinkList );
			FreeUp( (LPTR)lpFrame );
			return( NULL );
		}

		if ( (lpFrame->CacheFile = FileOpen( FrameName( lpFrame ),
				FO_READ|FO_WRITE|FO_CREATE )) == FILE_HANDLE_INVALID )
		{
			FrameSetError( ERR_SCROPEN );
			FrameDestroyColorMap(lpFrame->ColorMap);
			FreeLinePool( lpFrame->LinePool );
			FreeUp( (LPTR)lpFrame->FreePool );
			FreeUp( (LPTR)lpFrame->MemList );
			FreeUp( (LPTR)lpFrame->DiskList );
			FreeUp( (LPTR)lpFrame->LinkList );
			FreeUp( (LPTR)lpFrame );
			return( NULL );
		}

		// seek to end of cache file
		if (FileSeek(lpFrame->CacheFile, (lx*ly)+5, 0) != ((lx*ly)+5) ||
			FileWrite(lpFrame->CacheFile, &byte, 1) != 1) 
		{
			FrameSetError( ERR_SCRWRITE );
			FrameDestroyColorMap(lpFrame->ColorMap);
			FileClose( lpFrame->CacheFile );
			FileDelete( FrameName( lpFrame ));
			FreeLinePool( lpFrame->LinePool );
			FreeUp( (LPTR)lpFrame->FreePool );
			FreeUp( (LPTR)lpFrame->MemList );
			FreeUp( (LPTR)lpFrame->DiskList );
			FreeUp( (LPTR)lpFrame->LinkList );
			FreeUp( (LPTR)lpFrame );
			return( NULL );
		}

		FileSeek(lpFrame->CacheFile, 0L, 0);

		/* Write a marker to the file so no record gets a disk address of 0 */
		if ( FileWrite( lpFrame->CacheFile, "CACHE", 5 ) != 5 )
		{
			FrameSetError( ERR_SCRWRITE );
			FrameDestroyColorMap(lpFrame->ColorMap);
			FileClose( lpFrame->CacheFile );
			FileDelete( FrameName( lpFrame ));
			FreeLinePool( lpFrame->LinePool );
			FreeUp( (LPTR)lpFrame->FreePool );
			FreeUp( (LPTR)lpFrame->MemList );
			FreeUp( (LPTR)lpFrame->DiskList );
			FreeUp( (LPTR)lpFrame->LinkList );
			FreeUp( (LPTR)lpFrame );
			return( NULL );
		}
		FrameFlush(lpFrame);
	}
#endif

	if (FrameGetCurrent() == NULL)
		FrameSetCurrent( lpFrame );

	return( lpFrame );
}

/*====================================================================*/

void FrameClose( LPFRAME lpFrame )
{
	if ( lpFrame )
	{
#ifndef NO_CACHE
		if ( lpFrame->CacheFile != FILE_HANDLE_INVALID )
		{
			FileClose( lpFrame->CacheFile );
			FileDelete( FrameName( lpFrame ));
		}
#endif

		if ( lpFrame->ColorMap )
			FreeUp( (LPTR)lpFrame->ColorMap );

#ifdef NO_CACHE
		FreeUp( (LPTR)lpFrame->ImageBase);
#else
		FreeLinePool( lpFrame->LinePool );
		FreeUp( (LPTR)lpFrame->FreePool );
		FreeUp( (LPTR)lpFrame->MemList );
		FreeUp( (LPTR)lpFrame->DiskList );
		FreeUp( (LPTR)lpFrame->LinkList );
#endif
		FreeUp( (LPTR)lpFrame );

		if (lpFrame == FrameGetCurrent())
			FrameSetCurrent(NULL);
	}
}

/*====================================================================*/

BOOL FrameRead(
	LPFRAME lpFrame,
	int  sx, 
	int  sy, 
	int  dx,
	LPTR pixels,
	int  count)
{
	REG LPTR ptr;
	int inskip, left, right;
	REG LFIXED rate;

	if (lpFrame == NULL) 
		return(FALSE);

	if ( count != dx )
		rate = FGET( dx, count );
	else
		rate = FUNITY;

	left  = sx;
	right = sx + dx - 1;

	if ( FrameClip( lpFrame, sy, &left, &right ) )
		return( TRUE );

	inskip = ( left - sx ); /* In case some pixels got clipped off */

	if ( !(ptr = FramePointer( lpFrame, left, sy, NO )) )
		return( FALSE );

	FrameSample( lpFrame, ptr, inskip, pixels, 0, count, rate );

	return( TRUE );
}

/*====================================================================*/

BOOL FrameWrite(
	LPFRAME lpFrame,
	int  sx,
	int  sy, 
	int  dx,
	LPTR pixels,
	int  count)
{
	REG LPTR ptr;
	int outskip, left, right;
	REG LFIXED rate;

	if (lpFrame == NULL) 
		return(FALSE);

	if ( count != dx )
		rate = FGET( count, dx );
	else
		rate = FUNITY;

	left  = sx;
	right = sx + dx - 1;

	if ( FrameClip( lpFrame, sy, &left, &right ) )
		return( TRUE );

	outskip = ( left - sx ); /* In case some pixels got clipped off */
	count = right - left + 1;

	if ( !(ptr = FramePointer( lpFrame, left, sy, YES )) )
		return( FALSE );

	FrameSample( lpFrame, pixels, 0, ptr, outskip, count, rate );

	return( TRUE );
}

/*====================================================================*/

int FrameMode( LPFRAME lpFrame, int ReleaseMode )
{
	int oldMode = -1;

	if ( lpFrame )
	{
		oldMode = lpFrame->ReleaseMode;
		lpFrame->ReleaseMode = ReleaseMode;
	}

	return(oldMode);
}

/*====================================================================*/

int FrameInMemory( LPFRAME lpFrame )
{

	if ( !lpFrame )
		return( 0 );

#ifdef NO_CACHE
		return(100);
#else
{
	int y, count;

	// If we do not have a cache file...
	if (lpFrame->CacheFile == FILE_HANDLE_INVALID)
	{
		return(100);
	}

	count = 0;

	for ( y=0; y<lpFrame->Ysize; y++ )
	{
		if ( lpFrame->MemList[y] )
			count++;
	}

	return( ((100L * count)+lpFrame->Ysize/2) / lpFrame->Ysize );
}
#endif
}

/*====================================================================*/

LPTR LineInMemory( LPFRAME lpFrame, int y)
{
	if (!lpFrame)
		return(NULL);

#ifdef NO_CACHE
	return(FramePointer(lpFrame, 0, y, NO));
#else
 	return(lpFrame->MemList[y]);
#endif
}


/*====================================================================*/

LPTR FramePointer( LPFRAME lpFrame, int x, int y, BOOL bModify )
{
	LPTR lpMemory;
	static CFrameTypeConvert TypeConvert;

	if (!lpFrame) 
		return(NULL);

	// does this frame require unpacking
	if (lpFrame->DataType == FDT_LINEART)
	{
		if (lpFrame->CacheLine >= 0) // do we have something dirty in buffer
		{
			if (lpFrame->CacheLine == y)	// do we already have it?
				return(lpFrame->LinePtr + x);
			if (!(lpMemory = FramePointerRaw( lpFrame, 0, lpFrame->CacheLine, YES )))
			{
				lpFrame->CacheLine = -1; // reset
				return(NULL);
			}
			// Convert the data back to lineart
			TypeConvert.GrayScaleToLineArt(lpFrame->LinePtr, lpMemory, 0, (lpFrame->Width * 8));

			lpFrame->CacheLine = -1; // reset
		}

		// Get the new frame scanline pointer
		if (!(lpMemory = FramePointerRaw( lpFrame, 0, y, bModify )))
		{
			return(NULL);
		}

		// if being modified keep track of this line as dirty
		if (bModify && (y >= 0) && (y <= lpFrame->Ysize))
		{
			lpFrame->CacheLine = y;
		}

		// convert lineart to 8-bit data
		TypeConvert.LineArtToGrayScale(lpMemory, lpFrame->LinePtr, 0, lpFrame->Width * 8);

		return(lpFrame->LinePtr + x);
	}
	else
	{
		return(FramePointerRaw(lpFrame, x, y, bModify));
	}
}

/*====================================================================*/

LPTR FramePointerRaw( LPFRAME lpFrame, int x, int y, BOOL bModify )
{
	LPTR lpMemory;

	if (!lpFrame)
		return(NULL);

	/* Out of bounds in the y direction - pass back the background */
	if ( y < 0 || y >= lpFrame->Ysize )
	{ 
		/* Always return a valid pointer to the caller */
		return( lpFrame->BackScratch );
	}

	switch( lpFrame->DataType )
	{
		case FDT_LINEART :
		case FDT_GRAYSCALE :
		case FDT_PALETTECOLOR : break;
		case FDT_RGBCOLOR  : x *= 3; break;
		case FDT_CMYKCOLOR : x *= 4; break;
	}

#ifdef NO_CACHE
	lpMemory = lpFrame->ImageBase + (y * lpFrame->Width);
	if (lpFrame->Background != NO_BACKGROUND)
		{
		LPTR lpEmpty;

		lpEmpty = lpFrame->EmptyList + y;
		if (!(*lpEmpty))
			{
			*lpEmpty = 1;
			set( lpMemory, lpFrame->Width, lpFrame->Background );
			}
		}
	return(lpMemory + x);
#else
{
	long lDisk;

	if ( bModify )
		lpFrame->DiskList[y] |= MODIFIED_ON;

	/* See if the line is in memory */
	if ( lpMemory = lpFrame->MemList[y] )
	{
		/* Memory hit in the cache for the line */
		TouchLink( lpFrame, y ); /* Make it the most recently used entry */
		return( lpMemory + x );
	}

	/* It's not in memory, so get one of the pointers in the freelist */
	if ( !(lpFrame->MemList[y] = AllocLine(lpFrame)) )
	{
		/* If we can't get a pointer, flush some lines out to the cache file */
		if ( !CacheRelease( lpFrame, (long)lpFrame->Width * TRASH_FACTOR ) )
		{
//			Print("CacheRelease failed");
			MessageBeep(0);
//			return( NULL );
			return( lpFrame->BackScratch );
		}

		/* Let's try again - there's no reason why this should fail */
		if ( !(lpFrame->MemList[y] = AllocLine(lpFrame)) )
		{
//			dbg("Line allocation failed after release; What happened?");
//			Print("AllocLine failed");
			MessageBeep(0);
//			return( NULL );
			return( lpFrame->BackScratch );
		}
	}

	/* At this point, we have some memory for the line */
	/* Put it in the most recently used list */
	lpMemory = lpFrame->MemList[y];
	AddLink( lpFrame, y );

	/* See if the image data for this line was ever cached out to disk */
	/* Mask off the high bit; a flag indicating that memory was modified */
	if ( lDisk = (lpFrame->DiskList[y] & MODIFIED_OFF) )
	{
		/* Find the line in the disk cache file */
		FileSeek( lpFrame->CacheFile, lDisk, 0 );

		if ( FileRead( lpFrame->CacheFile, lpMemory,
			lpFrame->Width ) != lpFrame->Width)
		{
			/* Error reading data from the cache */
//			Print("FramePointerRaw - Error reading %u bytes from cache files %ls",
//					lpFrame->Width, (LPTR)lpFrame->CacheFileName);
			MessageBeep(0);
//			return( NULL );
			return( lpFrame->BackScratch );
		}
	}
	else
	{
		/* Create a blank line (or leave it as garbage if no bkgnd color) */
		if ( lpFrame->Background != NO_BACKGROUND )
		{
			set( lpMemory, lpFrame->Width, lpFrame->Background );
		}
	}
	return( lpMemory + x );
}
#endif
}

/*====================================================================*/

LPSTR FrameName( LPFRAME lpFrame )
{
	if (!lpFrame)
		return(NULL);

#ifdef NO_CACHE
	lpFrame->CacheFileName[0] = '\0';
	return(lpFrame->CacheFileName);
#else
{
	int i;
	char buf[10];
	FNAME szFileName;
	BOOL bFileExists;

	// If we do not already have a frame name build it.
	if (lstrlen(lpFrame->CacheFileName) == 0)
	{
		// Make sure that we generate a unique file name for this new frame
		do
		{
			/* put it on the target drive */
			lstrcpy( szFileName, LocalFrameSettings.RamDisk );
#ifdef _MAC
			if ( (i = lstrlen(szFileName)) && szFileName[i-1] != ':' )
				lstrcat( szFileName, ":" );
#else
			if ( (i = lstrlen(szFileName)) && szFileName[i-1] != '\\' )
				lstrcat( szFileName, "\\" );
#endif				
			lstrcat( szFileName, "FRM" );
			lstrcat( szFileName, Int2Ascii(buf,LocalFrameSettings.CacheSeq));
			lstrcat( szFileName, ".TMP" );

			lpFrame->CacheSeq = LocalFrameSettings.CacheSeq;

			LocalFrameSettings.CacheSeq++;

			bFileExists = FileExists( szFileName );
		}
		while( bFileExists );

		lstrcpy(lpFrame->CacheFileName, szFileName);
	}

	return( lpFrame->CacheFileName );
}
#endif
}

/*====================================================================*/

FRMDATATYPE FrameType( LPFRAME lpFrame )
{
	if ( lpFrame )
	{
		return( lpFrame->DataType );
	}

	return( ( FRMDATATYPE )-1 );
}

/*====================================================================*/

FRMTYPEINFO FrameTypeInfo( LPFRAME lpFrame )
{
	FRMTYPEINFO TypeInfo;

	if ( lpFrame )
		FrameGetTypeInfo(lpFrame, &TypeInfo);
	else
		FrameSetTypeInfo(&TypeInfo, FDT_NONE, NULL);

	return( TypeInfo );
}

/*====================================================================*/

BOOL FrameTypeInfoEqual( FRMTYPEINFO TypeInfo1, FRMTYPEINFO TypeInfo2)
{
return((TypeInfo1.DataType == TypeInfo2.DataType) &&
		FrameColorMapsEqual(TypeInfo1.ColorMap, TypeInfo2.ColorMap));
}

/*====================================================================*/

int FrameDepth( LPFRAME lpFrame )
{
	if ( lpFrame )
	{
		return( lpFrame->Depth );
	}

	return( -1 );
}

/*====================================================================*/

LPFRMTYPEINFO FrameGetTypeInfo( LPFRAME lpFrame, LPFRMTYPEINFO lpTypeInfo)
{
	if ( lpFrame )
	{
		lpTypeInfo->DataType = lpFrame->DataType;
		lpTypeInfo->ColorMap = lpFrame->ColorMap;
		return( lpTypeInfo );
	}

	return( NULL );
}

/*====================================================================*/

LPFRMTYPEINFO FrameSetTypeInfo(LPFRMTYPEINFO lpTypeInfo,
								FRMDATATYPE DataType,
								LPCOLORMAP ColorMap)
{
	lpTypeInfo->DataType = DataType;
	lpTypeInfo->ColorMap = ColorMap;
	return( lpTypeInfo );
}

/*====================================================================*/

int FrameXSize( LPFRAME lpFrame )
{
	if ( lpFrame )
	{
		return( lpFrame->Xsize );
	}
	else
	{
		return( -1 );
	}
}

/*====================================================================*/

int FrameYSize( LPFRAME lpFrame )
{
	if ( lpFrame )
	{
		return( lpFrame->Ysize );
	}
	else
	{
		return( -1 );
	}
}

/*====================================================================*/

int FrameByteWidth( LPFRAME lpFrame )
{
	if ( lpFrame )
	{
		return(lpFrame->Width);
	}
	else
	{
		return( -1 );
	}
}

/*====================================================================*/

int FrameResolution( LPFRAME lpFrame )
{
	if ( lpFrame )
	{
		return( lpFrame->Resolution );
	}
	else
	{
		return( -1 );
	}
}

/*====================================================================*/

int FrameSetResolution( LPFRAME lpFrame, int Resolution )
{
	int OldResolution;

	if ( lpFrame )
	{
		OldResolution = lpFrame->Resolution;
		lpFrame->Resolution = Resolution;
		return(OldResolution);
	}
	else
	{
		return( -1 );
	}
}

/*====================================================================*/

void FrameSetBackground(LPFRAME lpFrame, BYTE background)
{
if (lpFrame)
	lpFrame->Background = (int)background;
}

/*====================================================================*/

BYTE FrameGetBackground(LPFRAME lpFrame)
{
if (!lpFrame)
	return(NO_BACKGROUND);
return((BYTE)lpFrame->Background);
}

/*====================================================================*/

BOOL FrameSetColorMap( LPFRAME lpFrame, LPCOLORMAP lpColorMap )
{
	int i;
	LPCOLORMAP lpNewMap;

	if (!lpFrame)
		return(FALSE);

	if (lpFrame->ColorMap)
	{
		// Update the existing Color Map
		lpNewMap = lpFrame->ColorMap;
	} 
	else 
	{
		// Allocate space for the new map
		lpNewMap = (LPCOLORMAP)Alloc(sizeof(COLORMAP)+(sizeof(RGBS)*256));

		if (!lpNewMap) return(FALSE);
	}

	lpNewMap->NumEntries = lpColorMap->NumEntries;

	for(i=0;i<lpNewMap->NumEntries;i++) {
		lpNewMap->RGBData[i] = lpColorMap->RGBData[i];
	}

	lpNewMap->LastRGB = lpNewMap->RGBData[0];
	lpNewMap->LastIndex = 0;
	lpFrame->ColorMap = lpNewMap;

	return(TRUE);
}

/*====================================================================*/

BYTE FrameGetNearestIndex(LPCOLORMAP lpColorMap, LPRGB lpRGB)
{
	long lBest, lDist, lRDist, lGDist, lBDist;
	BYTE BestIndex;
	int i, NumEntries;
	RGBS rgb1, rgb2;

	if (!lpColorMap)
		return(0);

	if (lpColorMap->LastIndex >= 0 &&
		lpColorMap->LastIndex < lpColorMap->NumEntries)
	{
		if (lpRGB->red == lpColorMap->LastRGB.red &&
		lpRGB->green == lpColorMap->LastRGB.green &&
		lpRGB->blue == lpColorMap->LastRGB.blue)
		return(lpColorMap->LastIndex);
	}

	lBest = 0x7FFFFFFF;
	rgb1 = *lpRGB;
	NumEntries = lpColorMap->NumEntries;
	for (i = 0; i < NumEntries; ++i)
	{
		rgb2 = lpColorMap->RGBData[i];
		lRDist = rgb1.red - rgb2.red;
		lGDist = rgb1.green - rgb2.green;
		lBDist = rgb1.blue - rgb2.blue;
		lDist = (lRDist*lRDist) + (lGDist*lGDist) + (lBDist*lBDist);
		if (lDist < lBest)
		{
			lBest = lDist;
			BestIndex = i;
		}
	}
	lpColorMap->LastRGB = rgb1;
	lpColorMap->LastIndex = BestIndex;

	return(BestIndex);
}

/*====================================================================*/

LPCOLORMAP FrameGetColorMap( LPFRAME lpFrame )
{
	if (!lpFrame)
		return(NULL);

	return(lpFrame->ColorMap);
}

/*====================================================================*/

LPCOLORMAP FrameCreateColorMap()
{
	LPCOLORMAP lpColorMap;

	lpColorMap = (LPCOLORMAP)Alloc(sizeof(COLORMAP)+(sizeof(RGBS)*256));
	if (lpColorMap)
	{
		lpColorMap->NumEntries = 0;
		lpColorMap->LastIndex = -1;
		clr((LPTR)lpColorMap->RGBData, sizeof(RGBS)*256);
	}
 	return(lpColorMap);
}

LPCOLORMAP FrameCopyColorMap(LPCOLORMAP lpColorMap)
{
	LPCOLORMAP lpNewMap;
	
	lpNewMap = FrameCreateColorMap();
	if (!lpNewMap)
		return(NULL);
	FrameCopyColorMap(lpColorMap, lpNewMap);
	return(lpNewMap);
}

void FrameCopyColorMap(LPCOLORMAP lpSrcMap, LPCOLORMAP lpDstMap)
{
	copy((LPTR)lpSrcMap->RGBData, (LPTR)lpDstMap->RGBData, sizeof(RGBS)*256);
	lpDstMap->NumEntries = lpSrcMap->NumEntries;
	lpDstMap->LastRGB = lpSrcMap->RGBData[0];
	lpDstMap->LastIndex = 0;
}

/*====================================================================*/

LPCOLORMAP FrameCreateFixedColorMap( void )
{
	LPCOLORMAP lpColorMap;
	int i;
	int r, g, b;

	lpColorMap = FrameCreateColorMap();

	if (!lpColorMap)
	{
		return(NULL);
	}

	for(i=0;i<256;i++)
	{
		// Put red in the range 0 - 7
		r = ((i&0xE0) >> 5);

		// Scale red back up to 0 - 255
		r = (r * 255) / 7;

		// Put green in the range 0 - 7
		g = ((i&0x1C) >> 2);

		// Scale green back up to 0 - 255
		g = (g * 255) / 7;

		// Put blue in the range 0 - 3
		b = (i&0x03);

		// Scale blue back up to 0 - 255
		b = (b * 255) / 3;

		lpColorMap->RGBData[i].red   = r;
		lpColorMap->RGBData[i].green = g;
		lpColorMap->RGBData[i].blue  = b;
	}
	lpColorMap->NumEntries = 256;
	return(lpColorMap);
}

/*====================================================================*/

void FrameDestroyColorMap(LPCOLORMAP lpColorMap)
{
	if (lpColorMap)
		FreeUp( (LPTR) lpColorMap );
}

/*====================================================================*/

BOOL  FrameColorMapsEqual(LPCOLORMAP lpColorMap1, LPCOLORMAP lpColorMap2)
{
	int i;

	if (lpColorMap1 == lpColorMap2)
		return(TRUE);
	if (!lpColorMap1)
		return(FALSE);
	if (!lpColorMap2)
		return(FALSE);
	if (lpColorMap1->NumEntries != lpColorMap2->NumEntries)
		return(FALSE);
	for (i = 0; i < lpColorMap1->NumEntries; ++i)
	{
		if (lpColorMap1->RGBData[i].red != lpColorMap2->RGBData[i].red)
			return(FALSE);
		if (lpColorMap1->RGBData[i].green != lpColorMap2->RGBData[i].green)
			return(FALSE);
		if (lpColorMap1->RGBData[i].blue != lpColorMap2->RGBData[i].blue)
			return(FALSE);
	}
	return(TRUE);
}

/*====================================================================*/

void FrameCopyLine(LPFRAME lpInFrame, LPFRAME lpOutFrame, int y)
{
LPTR lpOut, lpIn;

if (!lpInFrame || !lpOutFrame)
	return;

#ifdef NO_CACHE
	lpIn = FramePointerRaw(lpInFrame, 0, y, NO);	
	lpOut = FramePointerRaw(lpOutFrame, 0, y, YES);
	copy(lpIn, lpOut, lpInFrame->Width);
#else
{
LPTR lpMemoryIn, lpMemoryOut;
DWORD lDiskIn, lDiskOut;
unsigned dx;

lpMemoryOut = lpOutFrame->MemList[y];
dx = lpInFrame->Width;
if ( lpMemoryIn = lpInFrame->MemList[y] )
	{
	if ( lpMemoryOut = lpOutFrame->MemList[y] )
		{
		copy( lpMemoryIn, lpMemoryOut, dx );
		lpOutFrame->DiskList[y] |= MODIFIED_ON;
		}
	else
	if ( lDiskOut = (lpOutFrame->DiskList[y] & MODIFIED_OFF) ) // on disk?
		{
		FileSeek( lpOutFrame->CacheFile, lDiskOut, 0 );
		if ( FileWrite(lpOutFrame->CacheFile, lpMemoryIn, dx ) != dx)
			FrameSetError( ERR_SCRWRITE );
		}
	else
		{
		lpOut = FramePointerRaw(lpOutFrame, 0, y, YES);
		if (lpOut)
			copy(lpMemoryIn, lpOut, dx);
		}
	}
else
if ( lDiskIn = (lpInFrame->DiskList[y] & MODIFIED_OFF) ) // on disk?
	{
	if ( lpMemoryOut = lpOutFrame->MemList[y] )
		{
		FileSeek( lpInFrame->CacheFile, lDiskIn, 0 );
		if ( FileRead(lpInFrame->CacheFile, lpMemoryOut, dx ) != dx)
			FrameSetError( ERR_SCRREAD );
		lpOutFrame->DiskList[y] |= MODIFIED_ON;
		}
	else
	if ( lDiskOut = (lpOutFrame->DiskList[y] & MODIFIED_OFF) ) // on disk?
		{
		FileSeek( lpInFrame->CacheFile, lDiskIn, 0 );
		if ( FileRead(lpInFrame->CacheFile, lpOutFrame->BackScratch, dx ) != dx)
			FrameSetError( ERR_SCRREAD );
		FileSeek( lpOutFrame->CacheFile, lDiskOut, 0 );
		if ( FileWrite(lpOutFrame->CacheFile, lpOutFrame->BackScratch, dx ) != dx) 
			FrameSetError( ERR_SCRWRITE );
		}
	else
		{
		lpOut = FramePointerRaw(lpOutFrame, 0, y, YES);
		if (lpOut)
			{
			FileSeek( lpInFrame->CacheFile, lDiskIn, 0 );
			if ( FileRead(lpInFrame->CacheFile, lpOut, dx ) != dx)
				FrameSetError( ERR_SCRREAD );
			}
		}
	}
else
	{
	lpIn = FramePointerRaw(lpInFrame, 0, y, NO);
	if (!lpIn)
		{
//		Print("FrameCopyLine - NULL pointer returned from FramePointerRaw");
		MessageBeep(0);
		return;
		}
	if ( lpMemoryOut = lpOutFrame->MemList[y] )
		{
		copy( lpIn, lpMemoryOut, dx );
		lpOutFrame->DiskList[y] |= MODIFIED_ON;
		}
	else
	if ( lDiskOut = (lpOutFrame->DiskList[y] & MODIFIED_OFF) ) // on disk?
		{
		FileSeek( lpOutFrame->CacheFile, lDiskOut, 0 );
		if ( FileWrite(lpOutFrame->CacheFile, lpIn, dx ) != dx)
			FrameSetError( ERR_SCRWRITE );
		}
	else
		{
		lpOut = FramePointerRaw(lpOutFrame, 0, y, YES);
		if (lpOut)
			copy(lpIn, lpOut, dx);
		}
	}
}
#endif
}

/*====================================================================*/

void FrameExchLine(LPFRAME lpInFrame, LPFRAME lpOutFrame, int y)
{
LPTR lpOut, lpIn, lpTmp;
unsigned dx;

if (!lpInFrame || !lpOutFrame)
	return;
dx = lpInFrame->Width;
lpIn = FramePointerRaw(lpInFrame, 0, y, YES);
lpOut = FramePointerRaw(lpOutFrame, 0, y, YES);
if (lpIn && lpOut)
	{
	lpTmp = lpOutFrame->BackScratch;
	copy(lpOut, lpTmp, dx);
	copy(lpIn, lpOut, dx);
	copy(lpTmp, lpIn, dx);
	}
}

/*====================================================================*/

void FrameDuplicateLine( LPFRAME lpInFrame, LPFRAME lpOutFrame, int iny, int outy )
{
LPTR lpOut, lpIn;

if (!lpInFrame || !lpOutFrame)
	return;

#ifdef NO_CACHE
	lpIn = FramePointerRaw(lpInFrame, 0, iny, NO);	
	lpOut = FramePointerRaw(lpOutFrame, 0, outy, YES);
	copy(lpIn, lpOut, lpInFrame->Width);
#else
{
LPTR lpMemoryIn, lpMemoryOut;
DWORD lDiskIn, lDiskOut;
unsigned dx;

lpMemoryOut = lpOutFrame->MemList[outy];
dx = lpInFrame->Width;
if ( lpMemoryIn = lpInFrame->MemList[iny] )
	{
	if ( lpMemoryOut = lpOutFrame->MemList[outy] )
		{
		copy( lpMemoryIn, lpMemoryOut, dx );
		lpOutFrame->DiskList[outy] |= MODIFIED_ON;
		}
	else
	if ( lDiskOut = (lpOutFrame->DiskList[outy] & MODIFIED_OFF) ) // on disk?
		{
		FileSeek( lpOutFrame->CacheFile, lDiskOut, 0 );
		if ( FileWrite(lpOutFrame->CacheFile, lpMemoryIn, dx ) != dx)
			FrameSetError( ERR_SCRWRITE );
		}
	else
		{
		lpOut = FramePointerRaw(lpOutFrame, 0, outy, YES);
		if (lpOut)
			copy(lpMemoryIn, lpOut, dx);
		}
	}
else
if ( lDiskIn = (lpInFrame->DiskList[iny] & MODIFIED_OFF) ) // on disk?
	{
	if ( lpMemoryOut = lpOutFrame->MemList[outy] )
		{
		FileSeek( lpInFrame->CacheFile, lDiskIn, 0 );
		if ( FileRead(lpInFrame->CacheFile, lpMemoryOut, dx ) != dx)
			FrameSetError( ERR_SCRREAD );
		lpOutFrame->DiskList[outy] |= MODIFIED_ON;
		}
	else
	if ( lDiskOut = (lpOutFrame->DiskList[outy] & MODIFIED_OFF) ) // on disk?
		{
		FileSeek( lpInFrame->CacheFile, lDiskIn, 0 );
		if ( FileRead(lpInFrame->CacheFile, lpOutFrame->BackScratch, dx ) != dx)
			FrameSetError( ERR_SCRREAD );
		FileSeek( lpOutFrame->CacheFile, lDiskOut, 0 );
		if ( FileWrite(lpOutFrame->CacheFile, lpOutFrame->BackScratch, dx ) != dx)
			FrameSetError( ERR_SCRWRITE );
		}
	else
		{
		lpOut = FramePointerRaw(lpOutFrame, 0, outy, YES);
		if (lpOut)
			{
			FileSeek( lpInFrame->CacheFile, lDiskIn, 0 );
			if ( FileRead(lpInFrame->CacheFile, lpOut, dx ) != dx)
				FrameSetError( ERR_SCRREAD );
			}
		}
	}
else
	{
	lpIn = FramePointerRaw(lpInFrame, 0, iny, NO);
	if (!lpIn)
		{
//		Print("FrameDuplicateLine - NULL pointer returned from FramePointerRaw");
		MessageBeep(0);
		return;
		}
	if ( lpMemoryOut = lpOutFrame->MemList[outy] )
		{
		copy( lpIn, lpMemoryOut, dx );
		lpOutFrame->DiskList[outy] |= MODIFIED_ON;
		}
	else
	if ( lDiskOut = (lpOutFrame->DiskList[outy] & MODIFIED_OFF) ) // on disk?
		{
		FileSeek( lpOutFrame->CacheFile, lDiskOut, 0 );
		if ( FileWrite(lpOutFrame->CacheFile, lpIn, dx ) != dx)
			FrameSetError( ERR_SCRWRITE );
		}
	else
		{
		lpOut = FramePointerRaw(lpOutFrame, 0, iny, YES);
		if (lpOut)
			copy(lpIn, lpOut, dx);
		}
	}
}
#endif
}

/*====================================================================*/
LPFRAME FrameCopy( LPFRAME lpSrcFrame, LPRECT lpRect )
/*====================================================================*/
{
RECT rCopy;
LPFRAME lpDstFrame;
int y, Depth, bytes, YSize, fill;
int srcWidth, srcHeight;
LPTR lpSrc, lpDst;

if (!lpSrcFrame)
	return(NULL);

if (lpRect)
	{
	rCopy = *lpRect;
//	BoundRect(&rCopy, 0, 0, FrameXSize(lpSrcFrame)-1, FrameYSize(lpSrcFrame)-1);
	}
else
	{
	rCopy.left = rCopy.top = 0;
	rCopy.right  = FrameXSize(lpSrcFrame)-1;
	rCopy.bottom = FrameYSize(lpSrcFrame)-1;
	}

if ( !(lpDstFrame = FrameOpen(FrameType(lpSrcFrame), RectWidth(&rCopy),
	RectHeight(&rCopy), FrameResolution(lpSrcFrame))) )
		return(NULL);
YSize = FrameYSize(lpDstFrame);
if (lpRect)
	{
	if ( !(Depth = FrameDepth(lpSrcFrame)) )
		Depth = 1;
	srcWidth = 	FrameXSize(lpSrcFrame);
	srcHeight = FrameYSize(lpSrcFrame);
	y = RectWidth(&rCopy) * Depth;
	bytes = Min(y, srcWidth*Depth);
	fill = y-bytes;
	for (y = 0; y < YSize; ++y, ++rCopy.top)
		{
		lpDst = FramePointer( lpDstFrame, 0, y, YES );
		if (rCopy.top >= srcHeight)
		{
			if (lpDst)
				set((LPTR)lpDst, bytes+fill, 0xff);
			continue;
		}
		lpSrc = FramePointer( lpSrcFrame, rCopy.left, rCopy.top, NO );
		if (lpSrc && lpDst)
			{
			copy( (LPTR)lpSrc, (LPTR)lpDst, bytes );
			if (fill > 0)
				set((LPTR)lpDst + bytes, fill, 0xff);
			}
		}
	}
else
	{
	for (y = 0; y < YSize; ++y)
		FrameCopyLine(lpSrcFrame, lpDstFrame, y);
	}

return(lpDstFrame);
}



