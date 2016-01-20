// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

/* Garbage collection size multiplier */
#define TRASH_FACTOR 12

/* Local variables */
static LPFRAME Frame;

/* FRAME COMMANDS - frame_preload(), frame_open(), frame_close() */
/* frame_set(), frame-ptr(), frame_write(), frame_read() */

/*
	+--->	+---------------+  <--- Frame
	|	|  Frame Data	|	sizeof(FRAME) bytes
	|	|		|
	|	+---------------+  <--- Frame->LinkList
	|	|  LinkList	|	Y * sizeof(LINK) bytes
	|	|		|
	|	+---------------+  <--- Frame->HeadLink
	|	|  HeadLink	|	sizeof(LINK) bytes
	|	|		|
lOverheadSize	+---------------+  <--- Frame->BackScratch
	|	|  BackScratch	|	X bytes
	|	|		|
	|	+---------------+  <--- Frame->FreePool
	|	|  FreePool	|	Y * sizeof(LPPTR)
	|	|		|
	|	|		|	Initially all lines in LinePool
	|	|		|	After first top to bottom op. only
	|	|		|	a few lines.
	|	|		|
	+--->	+---------------+  <+-- Frame->FreePoolEnd

	+--->	+---------------+  <--- Frame->AddrList
lOverHeadSize1	|  AddrList	|	Y * sizeof(ADDR) bytes
	|	|		|
	|	|		|	Disk or Memory pointer for all
	|	|		|	lines in the image.
	|	|		|
	+--->	+---------------+

	+--->	+---------------+  <+-- Frame->LinePool
lMemSize_0	|  LinePool_0	|	whatever's allocated
	|	|		|
	+--->	+---------------+

	+--->	+---------------+  <+-- Frame->LinePool
lMemSize_1	|  LinePool_1	|	whatever's allocated
	|	|		|
	+--->	+---------------+
			.
			.
			.
	+--->	+---------------+  <+-- Frame->LinePool
lMemSize_N	|  LinePool_N	|	whatever's allocated
	|	|		|
	+--->	+---------------+
*/


/************************************************************************/
void FreeLinePool (lpLinePool)
/************************************************************************/
LPLINEPOOL lpLinePool;
{
LPLINEPOOL lpNextLinePool;

while (lpLinePool)
	{
//	dbg("freeing %lx", lpLinePool);dbg(0);
	lpNextLinePool = lpLinePool->Next;
	FreeUp ((LPTR) lpLinePool);
	lpLinePool = lpNextLinePool;
	}
}


/************************************************************************/
BOOL AllocLinePool (UseEMS, lMemWanted, lMemMinimum, lpLinePool)
/************************************************************************/
BOOL	UseEMS;		/* Use EMS memory */
long	lMemWanted;	/* Size of memory block wanted */
long	lMemMinimum;	/* Minimum size of memory block */
LPLINEPOOL lpLinePool;
{
//dbg("want to alloc %ld", lMemWanted);
while ( lMemWanted+sizeof(LINEPOOL) >= lMemMinimum )
	{
	//dbg("trying alloc %ld", lMemWanted);
	if ( UseEMS )
		lpLinePool->Next = (LPLINEPOOL)
			EMSalloc (lMemWanted + sizeof(LINEPOOL));
	else	lpLinePool->Next = (LPLINEPOOL)
			Alloc (lMemWanted + sizeof(LINEPOOL));

	if ( lpLinePool->Next )
		{
		//dbg("alloc linepool %lx-%ld", lpLinePool->Next,
//			lMemWanted);dbg(0);
		lpLinePool->Next->Next = NULL;
		lpLinePool->Next->Size = lMemWanted;
		return( TRUE );
		}

	lMemWanted -= 65536L;
	}

//dbg("alloc linepool failed");dbg(0);
return( FALSE );
}

/************************************************************************/
LPFRAME frame_open( Depth, x_size, y_size, resolution )
/************************************************************************/
WORD	Depth, x_size, y_size, resolution;
{
LPFRAME	frame;
LPTR	lp,lpz;
long	lx, ly, lOverheadSize, lOverheadSize1, lMemSize, lMaxSize, lLineSize;
BOOL	UseEMS, MustUnpack;
LOCAL int seq = 0;
long	lMemWanted, lMemMinimum;
LINEPOOL LinePool;
LPLINEPOOL lpLinePool;

/* Make sure all the arguments are cool */
if ( Depth < 0 || x_size <= 0 || y_size <= 0 )
	return( NULL );

ly = y_size;
if ( MustUnpack = !Depth )
	{
	lx = (x_size + 7) / 8;
	Depth = 1;
	}
else	lx = x_size * Depth;

if ( ly > 32767L )
	{
	Message( IDS_EMAXIMAGEHEIGHT );
	return( NULL );
	}
if ( x_size > 32767L )
	{
	Print("Can't allocate a picture wider than 32K pixels");
	return( NULL );
	}

/* The overhead allocation includes enough memory for the frame structure, */
/* address list, linked list, head link, background line, and free list */
/* create two different allocation areas, one for the frame struct, linked */
/* list, headlink, background line and the free list( approx 8 bytes/line),*/
/* the other for the address list 8 bytes/line */

if (MustUnpack)
	lLineSize = lx * 8;
else	lLineSize = 0;

lOverheadSize = (long)sizeof(FRAME) +
		ly * sizeof(LINK) + sizeof(LINK) + lx +
		ly * sizeof(LPPTR) +
		lLineSize;
lOverheadSize1 = ly * sizeof(ADDR);

if ( lOverheadSize > 65536L || lOverheadSize1 > 65536L )
	{
	Message( IDS_EIMAGEOVERHEAD );
	return( NULL );
	}

if ( !(lp = Alloc( lOverheadSize )) )
	{
	Message( IDS_EMEMALLOC );
	return( NULL );
	}
lclr( lp, lOverheadSize );

if ( !(lpz = Alloc( lOverheadSize1 )) )
	{
	Message( IDS_EMEMALLOC );
	FreeUp( lp );
	return( NULL );
	}
lclr( lpz, lOverheadSize1 );

/* Setup all of the important pointers and partition the overhead space */
frame = (LPFRAME)lp;
lp += sizeof(FRAME);
frame->LinkList = (LPLINK)lp;
lp += ly * sizeof(LINK);
frame->HeadLink = (LPLINK)lp;
lp += sizeof(LINK);
frame->BackScratch = lp;
lp += lx;
frame->FreePool = (LPPTR)lp;
lp += ly * sizeof(LPPTR);
frame->FreePoolEnd = (LPPTR)lp;
frame->AddrList = (LPADDR)lpz;
/*
if ( lp != (LPTR)frame + lOverheadSize )
	Print("OOPS %lx != %lx", lp, (LPTR)frame+lOverheadSize );
if ( frame->HeadLink != &frame->LinkList[ly] )
	Print("FrameOpen: Something is wrong %lx=%lx", frame->HeadLink,
		&frame->LinkList[ly] );
*/
frame->HeadLink->Next = ly;
frame->HeadLink->Prev = ly;

/* Use expanded memory if the user wants us to, and if we have some left */
EMSstatus( &Control.EMStotal, &Control.EMSavail );
UseEMS = ( !Control.EMSdisable && Control.EMStotal && Control.EMSavail );

/* Figure out how much conventional memory is available for the line pool */
/*	less the user defined Minimum to leave free */
lMemSize = ConventionalMemory() - 1024L * Control.MainMemMin;

/* Adjust by the user defined percentage */
lMemSize = (lMemSize * Control.MainMemFactor) / 100;

if ( UseEMS )
	{
	if ( Control.EMSavail >= 20 )
		lMemSize = 1024L * Control.EMSavail;
	else	UseEMS = NO;
	}

/* Figure out how much memory we'd need for the entire picture */
lMaxSize = lx * ly;

/* We'll need a little more if we have any wasted lines on 64K boundaries */
lMaxSize += ( lx * (1+(lMaxSize/65536L)) );

/* Don't take more than what we need for the whole image */
if ( lMemSize > lMaxSize )
	lMemSize = lMaxSize;

//dbg("frame open (%ld,%ld) %ld", lx, ly, lMaxSize );dbg(0);
/* It can't go negative or be too small */
lMemMinimum = TRASH_FACTOR * 2 * lx;
if ( lMemSize < lMemMinimum )
	lMemSize = lMemMinimum;

/* Time to allocate the line pool */
/* Get first block of memory for Line Pool */
if ( !AllocLinePool (UseEMS, lMemSize, lMemMinimum, &LinePool) )
	{
	Message( IDS_ELINEPOOLALLOC, lMemSize,
		(LPTR)(UseEMS? "EMS":"conventional") );
	FreeUp( (LPTR)frame->AddrList );
	FreeUp( (LPTR)frame );
	return( NULL );
	}

/* Get more blocks of memory if needed and available */
frame->LinePool = lpLinePool = LinePool.Next;
lMemWanted = lMemSize - LinePool.Next->Size;
lMemSize = LinePool.Next->Size;
//lMemMinimum -= lMemSize;
//if (lMemMinimum < 0)
//	lMemMinimum = 0;
if ( !UseEMS )
	{
	while ( AllocLinePool( UseEMS, lMemWanted, lMemMinimum, lpLinePool ) )
		{
		lpLinePool = lpLinePool->Next;
		lMemWanted -= lpLinePool->Size;
		lMemSize += lpLinePool->Size;
		}
	}

/* Open the cache file */
if ( (frame->CacheFile = _lcreat( cache_name( seq ), 0 )) < 0 )
	{
	Message( IDS_ESCROPEN, (LPTR)cache_name( seq ) );
	FreeLinePool( frame->LinePool );
	FreeUp( (LPTR)frame->AddrList );
	FreeUp( (LPTR)frame );
	return( NULL );
	}

/* Write a marker to the file so no record gets a disk address of 0 */
if ( _lwrite( frame->CacheFile, "CACHE", 5 ) != 5 )
	{
	Message( IDS_ESCRWRITE, (LPTR)Control.RamDisk );
	_lclose( frame->CacheFile );
	FreeLinePool( frame->LinePool );
	FreeUp( (LPTR)frame->AddrList );
	FreeUp( (LPTR)frame );
	return( NULL );
	}

frame->CacheSeq = seq++;
frame->Resolution = resolution;
frame->Xsize = x_size;
frame->Ysize = y_size;
frame->Width = lx;
frame->MustUnpack = MustUnpack;
frame->ReleaseMode = RELEASE_NEWEST;
frame->ReadMask = NULL;
frame->WriteMask = NULL;
frame->Background = 1;
frame->CacheLine = -1;

switch ( frame->Depth = Depth )
	{
	case 1:
	frame->DoSample = sample8;
	frame->DoSampleMW = sample8MW;
	frame->DoMirror = mirror;
	frame->DoToCache = ToCache8;
	frame->DoGetPixelHSL = getpixelHSL8;
	frame->DoGetPixelRGB = getpixelRGB8;
	frame->DoKernal = kernal8;
	frame->DoSharpSmooth = sharpsmooth8;
	frame->DoFlood = (LPROC)Flood8;
	frame->DoEdgeValue = EdgeValue8;
	frame->DoSetPixels = (LPROC)set;
	frame->DoMapPixels = MapPixels8;
	break;

	case 2:
	frame->DoSample = sample16;
	frame->DoSampleMW = sample16MW;
	frame->DoMirror = mirror16;
	frame->DoToCache = ToCache16;
	frame->DoGetPixelHSL = getpixelHSL16;
	frame->DoGetPixelRGB = getpixelRGB16;
	frame->DoKernal = kernal16;
	frame->DoSharpSmooth = sharpsmooth16;
	frame->DoFlood = (LPROC)Flood16;
	frame->DoEdgeValue = EdgeValue16;
	frame->DoSetPixels = (LPROC)set16;
	frame->DoMapPixels = MapPixels16;
	break;

	case 3:
	frame->DoSample = sample24;
	frame->DoSampleMW = sample24MW;
	frame->DoMirror = mirror24;
	frame->DoToCache = ToCache24;
	frame->DoGetPixelHSL = getpixelHSL24;
	frame->DoGetPixelRGB = getpixelRGB24;
	frame->DoKernal = kernal24;
	frame->DoSharpSmooth = sharpsmooth24;
	frame->DoFlood = (LPROC)Flood24;
	frame->DoEdgeValue = EdgeValue24;
	frame->DoSetPixels = (LPROC)set24;
	frame->DoMapPixels = MapPixels24;
	break;
	}

set( frame->BackScratch, (int)lx, frame->Background );

/* Breakup the linepool memory by loading the freelist with each pointer */
InitFreeList( frame );

// Make sure we have enough disk space (if we'll need some)
if ( lMaxSize > DiskSpace(Control.RamDisk) && (lMaxSize > lMemSize) )
	{
	Message( IDS_EDISKCACHE, (lMaxSize+512)/1024,
		(DiskSpace(Control.RamDisk)+512)/1024,
		(LPTR)Control.RamDisk );
	frame_close( frame );
	return( NULL );
	}

/* If we haven't got an active frame, activate this one (implicit frame_set) */
if ( Frame == NULL )
	Frame = frame;
return( frame );
}


/* HeadLink is the same as LinkList[Frame->Ysize] */
/* The HeadLink structure is used to point to the oldest and newest lines in */
/* the map and is considered to be the entry point of the circular list */
/* HeadLink->Next points to the Oldest line in the map */
/* HeadLink->Prev points to the Newest line in the map */
#define OLDEST	Frame->HeadLink->Next
#define NEWEST	Frame->HeadLink->Prev

/************************************************************************/
LOCAL void AddLink( y )
/************************************************************************/
int y;
{
REG LPLINK lpLink;
int ytop;

if ( y < 0 || y >= Frame->Ysize )
	{
	Print("AddLink OOB %d", y);
	return;
	}

/* If the y to be added as newest is already in the list, */
/* the caller should remove it before calling this add routine */

ytop = NEWEST;

/* Point the guy who is currently newest at y */
lpLink = &Frame->LinkList[ ytop ];
/* At this point, lpLink->Next should be the last y - Frame->Ysize */
if ( lpLink->Next == Frame->Ysize )
	lpLink->Next = y;
else	Print("Line linkage error");

/* Put y on the end by pointing him back at ytop */
lpLink = &Frame->LinkList[ y ];
lpLink->Prev = ytop;
lpLink->Next = Frame->Ysize;

/* y is now the newest */
NEWEST = y;
}


/************************************************************************/
LOCAL void RemoveLink( y )
/************************************************************************/
int y;
{
REG LPLINK lpLink, lpLink1;

if ( y < 0 || y >= Frame->Ysize )
	{
	Print("RemoveLink OOB %d", y);
	return;
	}

/* If the y is in the linked list, pull it out - the caller is responsible */
/* for freeing the memory before this call is made */

lpLink = &Frame->LinkList[ y ];
if ( lpLink->Prev | lpLink->Next )
	{ /* If its not an empty linkage... */
	/* Fix y's predecessor */
	lpLink1 = &Frame->LinkList[ lpLink->Prev ];
	/* At this point, lpLink1->Next should be y */
	if ( lpLink1->Next == y )
		/* - point him to y's next */
		lpLink1->Next = lpLink->Next;
	else	Print("Line linkage error :1 on line %d=%d",
			y, lpLink1->Next);

	/* Fix y's descendant */
	lpLink1 = &Frame->LinkList[ lpLink->Next ];
	/* At this point, lpLink1->Prev should be y */
	if ( lpLink1->Prev == y )
		/* - point him to y's prev */
		lpLink1->Prev = lpLink->Prev;
	else	Print("Line linkage error :2 on line %d=%d",
			y, lpLink1->Prev);

	lpLink->Next = lpLink->Prev = 0;
	}
}


/************************************************************************/
LOCAL void TouchLink( y )
/************************************************************************/
int y;
{
/* If y is already the newest, then we're done */
if ( y == NEWEST )
	return;

/* Take it out of the middle (if it's there)... */
RemoveLink( y );
/* and add it to the end */
AddLink( y );
}


/************************************************************************/
int Age( age )
/************************************************************************/
int age;
{
int y;
LPLINK lpLink;

y = NEWEST;
while ( --age >= 0 )
	{
	if ( y < 0 || y >= Frame->Ysize )
		return( OLDEST );
	lpLink = &Frame->LinkList[ y ];

	/* If its an empty linkage... */
	if ( !lpLink->Prev && !lpLink->Next )
		return( OLDEST );

	/* Get the predecessor */
	y = lpLink->Prev;
	}

return( y );
}


/************************************************************************/
LOCAL BOOL CacheRelease( lSize )
/************************************************************************/
long	lSize;
{
LPADDR	lpAddr;
long lDisk;
BOOL fFlush;
int y;

if ( !Frame )
	return( FALSE );

fFlush = NO;
while ( lSize > 0 )
	{
	if (Frame->ReleaseMode == RELEASE_NEWEST)
		y = Age( TRASH_FACTOR );
	else	y = OLDEST;
	if ( y < 0 || y >= Frame->Ysize )
		break;
	lpAddr = &Frame->AddrList[ y ];
	if ( !lpAddr->Memory )
		continue;
	/* If the line has been modified, or never cached out to disk */
	/* then write it out to disk */
	lDisk = lpAddr->Disk;
	if ( !lDisk || lDisk & MODIFIED_ON )
		{
		if ( lDisk &= MODIFIED_OFF )
			lpAddr->Disk =_llseek( Frame->CacheFile, lDisk, 0 );
		else	{
			lpAddr->Disk =_llseek( Frame->CacheFile, 0L, 2 );
			fFlush = YES;
			}
		if ( _lwrite( Frame->CacheFile, EMSmap(lpAddr->Memory),
			Frame->Width ) != Frame->Width )
			{
			Message( IDS_ESCRWRITE, (LPTR)Control.RamDisk );
			return( FALSE );
			}
		}
	if ( !FreeLine( lpAddr->Memory ) )
		{
		Print("Freeline didn't work");
		break;
		}
	RemoveLink( y );
	lpAddr->Memory = NULL;
	lSize -= Frame->Width;
	}

if ( fFlush )
	frame_flush();
return( TRUE );
}


/************************************************************************/
LOCAL BOOL FreeLine( lpMem )
/************************************************************************/
LPTR lpMem;
{
/* Back up the pointer and make sure we are not off the top of the list */
if ( --Frame->FreeNext < Frame->FreePool )
	{
	Frame->FreeNext++;
	return( FALSE );
	}
/* Copy the address into the list */
*(Frame->FreeNext) = lpMem;
return( TRUE );
}


/************************************************************************/
LOCAL LPTR AllocLine()
/************************************************************************/
{
LPTR lpMem;

/* Make sure we won't be off the end of the list if we advance the pointer */
if ( Frame->FreeNext >= Frame->FreePoolEnd )
	return( NULL );
/* Return the pointer and then advance it */
lpMem = *(Frame->FreeNext);
Frame->FreeNext++;
return( lpMem );
}


/************************************************************************/
void InitFreeList( frame )
/************************************************************************/
REG LPFRAME frame;
{
REG LPTR lp, lpNext;
REG LPPTR lppFree;
LPTR lpEnd;
BOOL ems;
long x;
LPLINEPOOL lpLinePool;

lppFree = frame->FreePool;
lpLinePool = frame->LinePool;
lp = ((LPTR)lpLinePool) + sizeof(LINEPOOL);
lpEnd = BumpPtr( lp, lpLinePool->Size);
x = frame->Width;
ems = IsEMS( lp );

//dbg("init freelist");
/* Stuff line pointers into the free list */
while ( lppFree < frame->FreePoolEnd )
	{
	/* Compute the next line pointer with the right pointer arithmetic */
	/* Make sure the line at 'lp' doesn't go past the end of the pool */
	lpNext = lp;
	if ( ems )
		{
		(DWORD)lpNext += x;
		if ( (DWORD)lpNext > (DWORD)lpEnd )
			break;
		}
	else	{
		lpNext = BumpPtr( lpNext, x );
		if ( (HPTR)lpNext > (HPTR)lpEnd )
			{
			if ( !(lpLinePool = lpLinePool->Next) )
			    break;
			lp = ((LPTR) lpLinePool) + sizeof(LINEPOOL);
			lpEnd = BumpPtr( lp, lpLinePool->Size);
			lpNext = BumpPtr( lp, x );
			}
		}

	/* Make sure that no pointer goes into the freelist that */
	/* would cause a line of imagery to span a 64K boundary */
	/* That way, no one needs to use huge pointer math */
	if ( !SPAN64K( lp, x ) )
		{
		*lppFree++ = lp; /* Place lp into the list */
//		dbg("using %lx", lp );
		}
//	else	dbg("skipping %lx", lp );

	lp = lpNext;
	}
//dbg(0);

/* The first open slot is the top of the list */
frame->FreeNext = frame->FreePool;

/* Put null pointers into the rest of the list */
while ( lppFree < frame->FreePoolEnd )
	*lppFree++ = NULL;
}


/************************************************************************/
void frame_mode( ReleaseMode )
/************************************************************************/
int ReleaseMode;
{
if ( Frame )
	Frame->ReleaseMode = ReleaseMode;
}


/************************************************************************/
void frame_close( lpFrame )
/************************************************************************/
LPFRAME lpFrame;
{
if ( lpFrame )
	{
	if ( lpFrame->CacheFile >= 0 )
		{
		_lclose( lpFrame->CacheFile );
		unlink( cache_name( lpFrame->CacheSeq ) );
		}
	if ( lpFrame->ReadMask )
		mask_close( lpFrame->ReadMask );
	if ( lpFrame->WriteMask )
		mask_close( lpFrame->WriteMask );
	FreeLinePool( lpFrame->LinePool );
	FreeUp( (LPTR)lpFrame->AddrList );
	FreeUp( (LPTR)lpFrame );
	if ( Frame == lpFrame )
		Frame = NULL;
	}
}


/************************************************************************/
BOOL frame_flush()
/************************************************************************/
{
if ( !Frame )
	return( FALSE );
if ( Frame->CacheFile < 0 )
	return( FALSE );

/* Close and reopen the cache file in case of a crash */
_lclose( Frame->CacheFile );
if ( (Frame->CacheFile = _lopen( cache_name( Frame->CacheSeq ), OF_READWRITE )) < 0 )
	{
	Message( IDS_ESCROPEN, (LPTR)cache_name( Frame->CacheSeq ) );
	return( FALSE );
	}

return( TRUE );
}


/************************************************************************/
LPFRAME frame_set( lpNewFrame )
/************************************************************************/
LPFRAME	lpNewFrame;
{
LPFRAME	lpOldFrame;

lpOldFrame = Frame;
if ( lpNewFrame )
	Frame = lpNewFrame;
return( lpOldFrame );
}


/************************************************************************/
int frame_inmemory()
/************************************************************************/
{
LPADDR lpAddr;
int y, count;

if ( !Frame )
	return( 0 );
count = 0;
for ( y=0; y<Frame->Ysize; y++ )
	{
	lpAddr = &Frame->AddrList[y];
	if ( lpAddr->Memory )
		count++;
	}

return( ((100L * count)+Frame->Ysize/2) / Frame->Ysize );
}


/************************************************************************/
BOOL frame_write( span, pixels, count )
/************************************************************************/
LPSPAN	span;
LPTR	pixels;
int	count;
{
if ( span->dy == 1 )	/* Horizontal */
	if ( Frame->WriteMask )
		return( horiz_Mwrite( span, pixels, count ) );
	else	return( horiz_write( span, pixels, count ) );

Print("frame_write: bad span!");
}


/************************************************************************/
BOOL frame_read( span, pixels, count )
/************************************************************************/
LPSPAN	span;
LPTR	pixels;
int	count;
{
if ( span->dy == 1 )	/* Horizontal */
	if ( Frame->ReadMask )
		return( horiz_Mread( span, pixels, count ) );
	else	return( horiz_read( span, pixels, count ) );

Print("frame_read: bad span!");
}


/************************************************************************/
LPTR frame_ptr( Depth, x, y, bModify )
/************************************************************************/
int Depth, x, y;
BOOL bModify;
{
LPADDR	lpAddr;
long lDisk;

if ( y < 0 || y >= Frame->Ysize )
	{ /* Out of bounds in the y direction - pass back the background */
/*	Print("OUT OF BOUNDS Y (%d,%d)", x, y); */
	/* Always return a valid pointer to the caller */
//	dbg( "fp %d yout",y);
	return( Frame->BackScratch );
	}

lpAddr = &Frame->AddrList[y];
if ( bModify )
	lpAddr->Disk |= MODIFIED_ON;

x *= Frame->Depth;

/* See if the line is in memory */
if ( lpAddr->Memory )
	{ /* Memory hit in the cache for the line */
	TouchLink( y ); /* Make it the most recently used entry */
//	dbg( "fp %d hit %lx", y, EMSmap(lpAddr->Memory) );
	return( EMSmap( lpAddr->Memory + x ) );
	}

/* It's not in memory, so get one of the pointers in the freelist */
if ( !(lpAddr->Memory = AllocLine()) )
	{
	/* If we can't get a pointer, flush some lines out to the cache file */
	if ( !CacheRelease( (long)Frame->Width * TRASH_FACTOR ) )
		{
//		dbg(" fp %d no rel",y );
		return( NULL );
		}
	/* Let's try again - there's no reason why this should fail */
	if ( !(lpAddr->Memory = AllocLine()) )
		{
		Print("Line allocation failed after release; What happened?");
		return( NULL );
		}
	}

/* At this point, we have some memory for the line */
/* Put it in the most recently used list */
AddLink( y );

/* See if the image data for this line was ever cached out to disk */
/* Mask off the high bit; a flag indicating that memory was modified */
if ( lDisk = (lpAddr->Disk & MODIFIED_OFF) )
	{ /* Find the line in the disk cache file */
	_llseek( Frame->CacheFile, lDisk, 0 );
//	dbg( "fp %y read %lx pos %ld", y,EMSmap(lpAddr->Memory), lDisk );
	if ( _lread( Frame->CacheFile, EMSmap(lpAddr->Memory),
		Frame->Width ) != Frame->Width )
		{ /* Error reading data from the cache */
		MessageBeep(0);
		return( NULL );
		}
	}
else	{ /* Create a blank line (or leave it as garbage if no bkgnd color) */
	if ( Frame->Background != 1 )
		set( EMSmap(lpAddr->Memory), Frame->Width, Frame->Background );
	}

//dbg( "fp %d ret %lx", y,EMSmap(lpAddr->Memory) );
return( EMSmap( lpAddr->Memory + x ) );
}


/************************************************************************/
PTR cache_name( n )
/************************************************************************/
int	n;
{
BYTE buf[10];
LOCAL BYTE name[128];
extern HANDLE hInstAstral;

strcpy( name, Control.RamDisk ); /* put it on the target drive*/
if (name [strlen (name) - 1] != '\\')
    strcat( name, "\\" );
strcat( name, "P" );
strcat( name, itoa( hInstAstral, buf, 10 ) );
strcat( name, itoa( n, buf, 10 ) );
strcat( name, ".TMP" );
return( name );
}


/************************************************************************/
LOCAL BOOL horiz_write( span, pixels, count )
/************************************************************************/
LPSPAN	span;
LPTR	pixels;
int	count;
{
REG LPTR ptr;
int outskip, left, right;
REG FIXED rate;

if ( count != span->dx )
	rate = FGET( count, span->dx );
else	rate = UNITY;

left = span->sx;
right = span->sx + span->dx - 1;
if ( frame_clip( span->sy, &left, &right ) )
	return( TRUE );

outskip = ( left - span->sx ); /* In case some pixels got clipped off */
count = right - left + 1;

if ( !(ptr = frame_ptr( 0, left, span->sy, YES )) )
	return( FALSE );

(*Frame->DoSample)( pixels, 0, ptr, outskip, count, rate );
return( TRUE );
}


/************************************************************************/
LOCAL BOOL horiz_Mwrite( span, pixels, count )
/************************************************************************/
LPSPAN	span;
LPTR	pixels;
int	count;
{
REG LPTR ptr;
int outskip, left, right;
REG FIXED rate;
REG LPTR m;

if ( count != span->dx )
	rate = FGET( count, span->dx );
else	rate = UNITY;

left = span->sx;
right = span->sx + span->dx - 1;
if ( frame_clip( span->sy, &left, &right ) )
	return( TRUE );

outskip = ( left - span->sx ); /* In case some pixels got clipped off */
count = right - left + 1;

if ( !(ptr = frame_ptr( 0, left, span->sy, YES )) )
	return( FALSE );

mask_compute( Frame->WriteMask, span->sy, Mask.IsOutside );
m = mask_point( Frame->WriteMask, left );
(*Frame->DoSampleMW)( pixels, 0, ptr, outskip, count, rate, m );
return( TRUE );
}


/************************************************************************/
LOCAL BOOL horiz_read( span, pixels, count )
/************************************************************************/
LPSPAN	span;
LPTR	pixels;
int	count;
{
REG LPTR ptr;
int inskip, left, right;
REG FIXED rate;

if ( count != span->dx )
	rate = FGET( span->dx, count );
else	rate = UNITY;

left = span->sx;
right = span->sx + span->dx - 1;
if ( frame_clip( span->sy, &left, &right ) )
	return( TRUE );

inskip = ( left - span->sx ); /* In case some pixels got clipped off */

if ( !(ptr = frame_ptr( 0, left, span->sy, NO )) )
	return( FALSE );

(*Frame->DoSample)( ptr, inskip, pixels, 0, count, rate );
return( TRUE );
}


/************************************************************************/
LOCAL BOOL horiz_Mread( span, pixels, count )
/************************************************************************/
LPSPAN	span;
LPTR	pixels;
int	count;
{
REG LPTR ptr;
int inskip, left, right;
REG FIXED rate;

if ( count != span->dx )
	rate = FGET( span->dx, count );
else	rate = UNITY;

left = span->sx;
right = span->sx + span->dx - 1;
if ( frame_clip( span->sy, &left, &right ) )
	return( TRUE );

inskip = ( left - span->sx ); /* In case some pixels got clipped off */

if ( !(ptr = frame_ptr( 0, left, span->sy, NO )) )
	return( FALSE );

(*Frame->DoSample)( ptr, inskip, pixels, 0, count, rate );
return( TRUE );
}


/************************************************************************/
BOOL frame_mask( span, pixels, count )
/************************************************************************/
LPSPAN	span;
LPTR	pixels;
int	count;
{
int inskip, left, right;
REG FIXED rate;
REG LPTR m;

if ( count != span->dx )
	rate = FGET( span->dx, count );
else	rate = UNITY;

left = span->sx;
right = span->sx + span->dx - 1;
if ( frame_clip( span->sy, &left, &right ) )
	return( TRUE );

inskip = ( left - span->sx ); /* In case some pixels got clipped off */

mask_compute( Frame->ReadMask, span->sy, Mask.IsOutside );
m = mask_point( Frame->ReadMask, left );
sample8( m, inskip, pixels, 0, count, rate );
return( TRUE );
}


/************************************************************************/
int frame_preload( fp, invert )
/************************************************************************/
int fp;
BOOL invert;
{
LPADDR lpAddr;
LPTR lpHead;
unsigned dx, y;
long lCacheStart, lCachePosition, lCacheRemainder, lTopTIF;
WORD size;
int ClockCount, ClockTotal;
BOOL fit_in_memory;
LPTR buf;

// The assumption for calling this routine is that the data in the file
// is the EXACT same format as the frame - i.e., same packiong, no line padding
dx = Frame->Width;
ClockCount = 0;
ClockTotal = ((long)dx * Frame->Ysize) / 1024L;
lTopTIF =_llseek( fp, 0L, 1 );

//dbg("preload");
/* Stuff as many memory pointers as we can */
for ( y=0; y<Frame->Ysize; y++ )
	{
	lpAddr = &Frame->AddrList[y];
	/* Get one of the pointers in the freelist */
	lpAddr->Memory = AllocLine();
//	dbg("ptr=%lx", lpAddr->Memory);
	if (!lpAddr->Memory)
		break;
	}

/* If we need a cache file, stuff the disk pointers for each line */
if ( !( fit_in_memory = (y >= Frame->Ysize) ) )
	{
	lCacheStart = lCachePosition =_llseek( Frame->CacheFile, 0L, 2 );
	for ( y=0; y<Frame->Ysize; y++ )
		{
		lpAddr = &Frame->AddrList[y];
		lpAddr->Disk = lCachePosition;
//		dbg("disk=%ld", lCachePosition);
		lCachePosition += dx;
		}
	lCacheRemainder = lCachePosition - lCacheStart;
	}

y = 0;

top:
lpHead = NULL;
size = 0;
while ( y < Frame->Ysize )
	{
	lpAddr = &Frame->AddrList[y];
	/* Get one of the pointers in the freelist */
	if ( !lpAddr->Memory )
		break;
	/* If you never set the head pointer, set it now */
	if ( !lpHead )
		lpHead = lpAddr->Memory;
	if ( lpAddr->Memory != lpHead + size )
		break; /* Not a contiguous line - break out and do a read */
	if ( SPAN64K( lpHead, (size + dx) ) )
		break; /* Would be too much data to read or write */
	/* Register the memory in the most recently used list */
	AddLink( y );
	y++;
	/* Keep track of the contigous block size */
	size += dx;
	}

/* Do the file read if lpHead and size are kosher */
if ( lpHead && size )
	{
	ClockCount += (size/1024L);
	AstralClockCursor( ClockCount, ClockTotal );
	lpHead = EMSmap( lpHead );
//	dbg("reading into %lx - %ld bytes", lpHead, size );
	if ( _lread( fp, lpHead, size ) != size )
		return( -1 );
	if ( invert )
		negate( lpHead, (long)size );
	if ( !fit_in_memory )
		{
		if ( _lwrite( Frame->CacheFile, lpHead, size ) != size )
			return( -2 );
		lCacheRemainder -= size;
		}
	/* Start over again with the last allocated line */
	goto top;
	}

if ( fit_in_memory )
	{ /* all in memory; no lines in the disk cache file */
//	dbg("fit in memory"); dbg(0);
	frame_flush();
	return( Frame->Ysize );
	}

/* Do a file copy from the remainder of the tif file to the cache file */
size = 65535L;
buf = LineBuffer[0];

while ( lCacheRemainder > 0 )
	{
	if ( lCacheRemainder < size )
		size = lCacheRemainder;
	ClockCount += (size/1024L);
	AstralClockCursor( ClockCount, ClockTotal );
//	dbg("reading2 into %lx - %ld bytes", buf, size );
	if ( _lread( fp, buf, size ) != size )
		return(-1);
	if ( invert )
		negate( buf, (long)size );
	if ( _lwrite( Frame->CacheFile, buf, size ) != size )
		return(-2);
	lCacheRemainder -= size;
	}

//dbg(0);
frame_flush();
return( Frame->Ysize );
}


/************************************************************************/
void frame_sample( input, inskip, output, outskip, count, rate )
/************************************************************************/
LPTR	input, output;
int	inskip, outskip;
int	count;
FIXED	rate;
{
(*Frame->DoSample)(input, inskip, output, outskip, count, rate );
}

/************************************************************************/
long frame_tocache(lpFullRGB)
/************************************************************************/
LPRGB lpFullRGB;
{
return((*Frame->DoToCache)(lpFullRGB));
}

/************************************************************************/
void frame_getpixelHSL( dummy, color, lpHSL )
/************************************************************************/
int dummy;
long color;
LPHSL lpHSL;
{
(*Frame->DoGetPixelHSL)( color, lpHSL );
}

/************************************************************************/
void frame_getpixelRGB( dummy, color, lpRGB )
/************************************************************************/
int dummy;
long color;
LPRGB lpRGB;
{
(*Frame->DoGetPixelRGB)( color, lpRGB );
}


/************************************************************************/
void frame_mirror( buf, count )
/************************************************************************/
LPTR buf;
int count;
{
(*Frame->DoMirror)( buf, count );
}


/************************************************************************/
void frame_kernal(line1, line2, line3, iCount, pKernal, Output)
/************************************************************************/
LPTR line1, line2, line3;
int iCount;
KERNAL *pKernal;
LPTR Output;
{
(*Frame->DoKernal)(line1, line2, line3, iCount, pKernal, Output);
}


/************************************************************************/
long frame_sharpsmooth(int Type, int xc, LPTR buf1, LPTR buf2, LPTR buf3, int Strength)
/************************************************************************/
{
return((*Frame->DoSharpSmooth)(Type, xc, buf1, buf2, buf3, Strength));
}


/************************************************************************/
VOID  frame_flood( HDC hDC, int x, int y )
/************************************************************************/
{
(*Frame->DoFlood)(hDC, x, y);
}


/************************************************************************/
long frame_edgevalue(int x, int y, int FromDir, int dx, int dy, int depth )
/************************************************************************/
{
return((*Frame->DoEdgeValue)(x, y, FromDir, dx, dy, depth));
}


/************************************************************************/
void frame_setpixels(LPTR ptr,int count, long value)
/************************************************************************/
{
(*Frame->DoSetPixels)(ptr, count, value);
}

/************************************************************************/
void frame_mappixels(LPTR lpSrc, LPTR lpMask, int iCount)
/************************************************************************/
{
(*Frame->DoMapPixels)(lpSrc, lpMask, iCount);
}


/************************************************************************/
int frame_width()
/************************************************************************/
{
if ( Frame )
	return( Frame->Width );
return( 0 );
}


/************************************************************************/
int frame_depth()
/************************************************************************/
{
if ( Frame )
	return( Frame->Depth );
return( 1 );
}


/************************************************************************/
BOOL frame_clip( y, left, right )
/************************************************************************/
// Clipping routine for horizontal vectors - no check is made to ensure
// that the points are horizontal, or that they are ordered correctly
// return YES if the points are clipped out completely, otherwise returns NO
int y;
LPINT left, right;
{
if ( y < 0 )			/* Are they above the top? */
	return( YES );
if ( y >= Frame->Ysize )	/* Are they below the bottom? */
	return( YES );
if ( *right < 0 )		/* Are they too far left? */
	return( YES );
if ( *left >= Frame->Xsize )	/* Are they too far right? */
	return( YES );
if ( *left < 0 )		/* Is the left hand clipped? */
	*left = 0;
if ( *right >= Frame->Xsize )	/* Is the right point clipped? */
	*right = Frame->Xsize - 1;
return( NO );
}


/************************************************************************/
LPTR BumpPtr( lp, lOffset )
/************************************************************************/
LPTR lp;
DWORD lOffset;
{
DWORD BigBumps;
DWORD HI, LO;

/* EMS pointer math or conventional pointer math */
if ( IsEMS( lp ) )
	return( (LPTR)((DWORD)lp + lOffset) );
LO = (DWORD)lp & 0xFFFFL;
LO += lOffset;
BigBumps = LO & 0xFFFF0000L;
if ( !BigBumps )
	lp += lOffset;
else	{
	LO &= 0xFFFFL;
	HI = (DWORD)lp & 0xFFFF0000L;
	HI += ( BigBumps * (DWORD)GetAHINCR() );
	lp = (LPTR)(DWORD)( HI + LO );
	}
return( lp );
}

/************************************************************************/
long ToCache8(lpFullRGB)
/************************************************************************/
LPRGB lpFullRGB;
{
return((long)RGBtoL(lpFullRGB));
}


/************************************************************************/
long ToCache16(lpFullRGB)
/************************************************************************/
LPRGB lpFullRGB;
{
return((long)RGBtoMiniRGB(lpFullRGB));
}


/************************************************************************/
long ToCache24(lpFullRGB)
/************************************************************************/
LPRGB lpFullRGB;
{
long color = 0;

CopyRGB(lpFullRGB, &color);
return(color);
}


/************************************************************************/
void getpixelHSL8( color, lpHSL )
/************************************************************************/
long color;
LPHSL lpHSL;
{
lpHSL->hue = 0;
lpHSL->sat = 0;
lpHSL->lum = color;
}

/************************************************************************/
void getpixelRGB8( color, lpRGB )
/************************************************************************/
long color;
LPRGB lpRGB;
{
lpRGB->red = color;
lpRGB->green = color;
lpRGB->blue = color;
}

/************************************************************************/
void getpixelHSL16( color, lpHSL )
/************************************************************************/
long color;
LPHSL lpHSL;
{
RGBtoHSL( MaxiR(color), MaxiG(color), MaxiB(color), lpHSL );
}

/************************************************************************/
void getpixelRGB16( color, lpRGB )
/************************************************************************/
long color;
LPRGB lpRGB;
{
lpRGB->red = MaxiR(color);
lpRGB->green = MaxiG(color);
lpRGB->blue = MaxiB(color);
}

/************************************************************************/
void getpixelHSL24( color, lpHSL )
/************************************************************************/
long color;
LPHSL lpHSL;
{
RGBS rgb;

copy((LPTR)&color, (LPTR)&rgb, 3);
RGBtoHSL( rgb.red, rgb.green, rgb.blue, lpHSL );
}

/************************************************************************/
void getpixelRGB24( color, lpRGB )
/************************************************************************/
long color;
LPRGB lpRGB;
{
copy((LPTR)&color, (LPTR)lpRGB, 3);
}

/************************************************************************/
int MapPixels8(lpSrc, lpMask, iCount)
/************************************************************************/
LPTR	lpSrc;
LPTR	lpMask;
int 	iCount;
{
BYTE dummy;

if (lpMask)
	{
	while (--iCount >= 0)
		{
		if (*lpMask++)
			MapRGB(&dummy, lpSrc, &dummy);
		++lpSrc;
		}
	}
else
	{
	while (--iCount >= 0)
		{
		MapRGB(&dummy, lpSrc, &dummy);
		++lpSrc;
		}
	}
}

/************************************************************************/
int MapPixels16(lpSrc, lpMask, iCount)
/************************************************************************/
LPWORD	lpSrc;
LPTR	lpMask;
int 	iCount;
{
WORD w;
BYTE r, g, b;

if (lpMask)
	{
	while (--iCount >= 0)
		{
		if (*lpMask++)
			{
			w = *lpSrc;
			r = MaxiR(w);
			g = MaxiG(w);
			b = MaxiB(w);
			MapRGB(&r, &g, &b);
			*lpSrc++ = ToMiniRGB(r, g, b);
			}
		else
			++lpSrc;
		}
	}
else
	{
	while (--iCount >= 0)
		{
		w = *lpSrc;
		r = MaxiR(w);
		g = MaxiG(w);
		b = MaxiB(w);
		MapRGB(&r, &g, &b);
		*lpSrc++ = ToMiniRGB(r, g, b);
		}
	}
}

/************************************************************************/
int MapPixels24(lpSrc, lpMask, iCount)
/************************************************************************/
LPRGB	lpSrc;
LPTR	lpMask;
int 	iCount;
{
if (lpMask)
	{
	while (--iCount >= 0)
		{
		if (*lpMask++)
			MapRGB(&lpSrc->red, &lpSrc->green, &lpSrc->blue);
		++lpSrc;
		}
	}
else
	{
	while (--iCount >= 0)
		{
		MapRGB(&lpSrc->red, &lpSrc->green, &lpSrc->blue);
		++lpSrc;
		}
	}
}

#ifdef UNUSED
/************************************************************************/
LPFRAME frame_copy(lpFrame)
/************************************************************************/
LPFRAME lpFrame;
{
LPFRAME lpNewFrame, lpSaveFrame;
unsigned y, dx, ynew, ytemp;
BOOL fit_in_memory;
long lDisk, lStart, lNextDisk;
LPADDR lpAddr;
ULONG lSize;
int count, lines, dCount, linesLeft;
LPTR buf, bufPtr;

lpSaveFrame = frame_set(NULL);
lpNewFrame = frame_open(lpFrame->Depth, lpFrame->Xsize, lpFrame->Ysize,
		lpFrame->Resolution);
if (!lpNewFrame)
	return( NULL );
frame_set(lpNewFrame);

/* Stuff as many memory pointers as we can */
for ( y=0; y<lpNewFrame->Ysize; y++ )
	{
	lpAddr = &lpNewFrame->AddrList[y];
	/* Get one of the pointers in the freelist */
	lpAddr->Memory = AllocLine();
	if (!lpAddr->Memory)
		break;
	AddLink(y);
	}

dx = lpNewFrame->Width;

/* If we need a cache file, stuff the disk pointers for each line */
if ( !( fit_in_memory = (y >= lpNewFrame->Ysize) ) )
	{
	lDisk = lStart = _llseek( lpNewFrame->CacheFile, 0L, 2 );
	for ( y=0; y<lpNewFrame->Ysize; y++ )
		{
		lpAddr = &lpNewFrame->AddrList[y];
		lpAddr->Disk = lDisk;
		lDisk += dx;
		}
	}

frame_set(lpSaveFrame);

lSize = 65535L;
lSize = (lSize / (long)dx) * (long)dx;
buf = LineBuffer[0];

for ( y=0, ynew=0; y<lpFrame->Ysize; )
	{
	AstralClockCursor(y, lpFrame->Ysize);
	bufPtr = buf;
	count = (int)(lSize / (long)dx);
	linesLeft = lpFrame->Ysize - y;
	if (count > linesLeft)
		count = linesLeft;
	lines = 0;
	while (lines < count)
		{
		lpAddr = &lpFrame->AddrList[y];
		if (lpAddr->Memory)
			{
			copy(lpAddr->Memory, bufPtr, dx);
			bufPtr += dx;
			++lines;
			++y;
			}
		else if ( lDisk = (lpAddr->Disk & MODIFIED_OFF) )
			{
			dCount = 1;
			lNextDisk = lDisk;
			++lines;
			++y;
			while (lines < count)
				{
				lpAddr = &lpFrame->AddrList[y];
				if (lpAddr->Memory)
					{
					break;
					}
				else if ((lNextDisk + dx) != (lpAddr->Disk & MODIFIED_OFF))
					{
					break;
					}
				else	{
					lNextDisk += dx;
					++dCount;
					++y;
					++lines;
					}
				}
			_llseek( lpFrame->CacheFile, lDisk, 0 );
			if ( _lread(lpFrame->CacheFile, bufPtr, (long)dx * dCount ) != (long)dx * dCount )
				{
				frame_close(lpNewFrame);
				Print("error reading data from the cache");
				return(NULL);
				}
			bufPtr += (long)dx*(long)dCount;
			}
		else	{
			++lines;
			++y;
			bufPtr += dx;
			}
		}
	/* now write buf to new frame */
	if (!fit_in_memory)
   	    {
	    if ( _lwrite(lpNewFrame->CacheFile, buf, (long)dx * count ) != (long)dx * count )
		{
		frame_close(lpNewFrame);
		Print("error reading data from the cache");
		return(NULL);
		}
	    }

	ytemp = ynew;
	ynew += count;
	bufPtr = buf;
	while (--count >= 0)
		{
		lpAddr = &lpNewFrame->AddrList[ytemp];
		if (lpAddr->Memory)
		     copy(bufPtr, lpAddr->Memory, dx);
		++ytemp;
		bufPtr += dx;
		}
	}

return(lpNewFrame);
}
#endif

#ifdef UNUSED
/************************************************************************/
int frame_bufwrite(fp, writeProc)
/************************************************************************/
int fp;
LPROC writeProc;
{ // pre-put
LPADDR lpAddr;
unsigned dx, y;
long size;
int count, lines, dCount, linesLeft;
long lDisk, lNextDisk;
LPTR buf, bufPtr;

dx = Frame->Width;

size = 65535L;
size = (size / (long)dx) * (long)dx;
buf = LineBuffer[0];

for (y = 0; y < Frame->Ysize;)
	{
	AstralClockCursor(y, Frame->Ysize);
	bufPtr = buf;
	count = (int)(size / (long)dx);
	linesLeft = Frame->Ysize - y;
	if (count > linesLeft)
		count = linesLeft;
	lines = 0;
	while (lines < count)
		{
		lpAddr = &Frame->AddrList[y];
		if (lpAddr->Memory)  /* in memory? */
			{ /* copy it into buffer */
			copy(lpAddr->Memory, bufPtr, dx);
			bufPtr += dx;
			++lines;
			++y;
			}
		else
		if ( lDisk = (lpAddr->Disk & MODIFIED_OFF) )  /* on disk? */
			{
			dCount = 1;
			lNextDisk = lDisk;
			++lines;
			++y;
			while (lines < count)
				{
				lpAddr = &Frame->AddrList[y];
				if (lpAddr->Memory)
					{
					break;
					}
				else if ((lNextDisk + dx) != (lpAddr->Disk & MODIFIED_OFF))
					{
					break;
					}
				else
					{
					lNextDisk += dx;
					++dCount;
					++y;
					++lines;
					}
				}
			_llseek( Frame->CacheFile, lDisk, 0 );
			if ( _lread(Frame->CacheFile, bufPtr, (long)dx * dCount ) != (long)dx * dCount )
				{
				Print("error reading data from the cache");
				return(-1);
				}
			bufPtr += (long)dx*(long)dCount;
			}
		else
			{
			++lines;
			++y;
			bufPtr += dx;
			}
		}
	if (((*writeProc)((LPTR)buf, (unsigned)dx, (int)count, (int)fp)) != lines)
		{
		Print("Error writing image data");
		return(-2);
		}
	}

return( 0 );
}
#endif


/************************************************************************/
VOID frame_copyline(lpInFrame, lpOutFrame, y)
/************************************************************************/
LPFRAME lpInFrame, lpOutFrame;
int y;
{
LPFRAME lpOldFrame;
LPADDR lpInAddr, lpOutAddr;
LPTR lpIn, lpOut;
unsigned dx;
long lInDisk, lOutDisk;

lpInAddr = &lpInFrame->AddrList[y];
lpOutAddr = &lpOutFrame->AddrList[y];
dx = lpInFrame->Width;
if (lpInAddr->Memory)
	{
	if (lpOutAddr->Memory)
		{
		copy(lpInAddr->Memory, lpOutAddr->Memory, dx);
		lpOutAddr->Disk |= MODIFIED_ON;
		}
	else
	if ( lOutDisk = (lpOutAddr->Disk & MODIFIED_OFF) )  /* on disk? */
		{
		_llseek( lpOutFrame->CacheFile, lOutDisk, 0 );
		if ( _lwrite(lpOutFrame->CacheFile, lpInAddr->Memory, dx ) !=dx)
			Print("Error writing to frame");
		}
	else	{
		lpOldFrame = frame_set(lpOutFrame);
		lpOut = frame_ptr(0, 0, y, YES);
		if (lpOut)
			copy(lpInAddr->Memory, lpOut, dx);
		frame_set(lpOldFrame);
		}
	}
else
if ( lInDisk = (lpInAddr->Disk & MODIFIED_OFF) )  /* on disk? */
	{
	if (lpOutAddr->Memory)
		{
		_llseek( lpInFrame->CacheFile, lInDisk, 0 );
		if ( _lread(lpInFrame->CacheFile, lpOutAddr->Memory, dx ) != dx)
			Print("Error reading from frame");
		lpOutAddr->Disk |= MODIFIED_ON;
		}
	else
	if ( lOutDisk = (lpOutAddr->Disk & MODIFIED_OFF) )  /* on disk? */
		{
		_llseek( lpInFrame->CacheFile, lInDisk, 0 );
		if ( _lread(lpInFrame->CacheFile, lpOutFrame->BackScratch, dx ) != dx )
			Print("Error reading from frame");
		_llseek( lpOutFrame->CacheFile, lOutDisk, 0 );
		if ( _lwrite(lpOutFrame->CacheFile, lpOutFrame->BackScratch, dx ) != dx )
			Print("Error writing to frame");
		}
	else	{
		lpOldFrame = frame_set(lpOutFrame);
		lpOut = frame_ptr(0, 0, y, YES);
		if (lpOut)
			{
			_llseek( lpInFrame->CacheFile, lInDisk, 0 );
			if ( _lread(lpInFrame->CacheFile, lpOut, dx ) != dx )
				Print("Error reading from frame");
			}
		frame_set(lpOldFrame);
		}
	}
else	{
	Print("Line in input frame does not exist");
	}
}


/************************************************************************/
WORD GetAHINCR()
/************************************************************************/
{
HANDLE hKernel;
LPTR lpVar;
static WORD _AHINCR;

if ( _AHINCR )
	return( _AHINCR );

if ( !(hKernel = LoadLibrary( "KERNEL.EXE" )) )
	return( _AHINCR = 0x1000 );

if ( !(lpVar = (LPTR)GetProcAddress( hKernel, "__AHINCR" )) )
	_AHINCR = 0x1000;
else	_AHINCR = LOWORD( lpVar );

FreeLibrary( hKernel );
return( _AHINCR );
}


/************************************************************************/
int readimage(lineno,xstart,xend,numpix,buffer)
/************************************************************************/
unsigned int lineno,xstart,xend,numpix;
LPTR buffer;
{
SPAN span;

span.sx = xstart;
span.sy = lineno;
span.dx = xend - xstart + 1;
span.dy = 1;

return( frame_read( &span, buffer, numpix ) > 0 );
}


/************************************************************************/
int writeimage(lineno,xstart,numpix,buffer)
/************************************************************************/
unsigned int lineno,xstart,numpix;
LPTR buffer;
{
SPAN span;

span.sx = xstart;
span.sy = lineno;
span.dx = numpix;
span.dy = 1;

return( frame_write( &span, buffer, numpix ) > 0 );
}
