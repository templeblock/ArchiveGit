//®PL1¯®FD1¯®TP0¯®BT0¯®RM250¯
/*=======================================================================*\

	PIXMAP.C - Undoable pixmap(bitmap) routines

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/

#include <windows.h>
#include "id.h"
#include "routines.h"

#define FRAME_NEW			0x0001
#define FRAME_NEWDEPTH		0x0002
#define PERCENT_WORTHWHILE	10

static VOID PixmapSyncLine( LPPIXMAP lpPixmap, int y );
static VOID PixmapExchLines( LPPIXMAP lpPixmap, BYTE flag );
static BOOL PixmapCreateUndo( LPPIXMAP lpPixmap );
static BOOL PixmapCreateFlags( LPPIXMAP lpPixmap, BYTE bFlag );

/***********************************************************************/
void PixmapSetup( LPPIXMAP lpPixmap, LPFRAME lpFrame, BOOL NoUndo)
/***********************************************************************/
{
lpPixmap->EditFrame = lpFrame;
lpPixmap->UndoFrame = NULL;
lpPixmap->UndoFailed = NO;
lpPixmap->EditFlags = NULL;
AstralSetRectEmpty( &lpPixmap->UndoRect );
lpPixmap->fNewFrame = FALSE;
}

/***********************************************************************/
BOOL PixmapInitUndo( LPPIXMAP lpPixmap )
/***********************************************************************/
{
/* if we have an UndoFrame and we also have a NewFrame, then */
/* UndoFrame and EditFrame are different sizes and must be synced */
/* by first getting rid of the UndoFrame */ 
if (lpPixmap->UndoFrame && lpPixmap->fNewFrame)
	PixmapFreeUndo(lpPixmap);

/* If we don't have an UndoFrame, and we haven't failed creating */
/* one before, create one */
if (!lpPixmap->UndoFailed && !lpPixmap->UndoFrame)
	if (!PixmapCreateUndo(lpPixmap))
		return(FALSE);

/* if we have an UndoFrame and no EditFlags, create the EditFlags */
if (lpPixmap->UndoFrame && !lpPixmap->EditFlags)
	if (!PixmapCreateFlags(lpPixmap, UNDO_EMPTY))
		return(FALSE);

return( TRUE );
}

/***********************************************************************/
BOOL PixmapGetInfo(LPPIXMAP lpPixmap, PIXMAP_TYPE pmType,
					LPINT lpXSize, LPINT lpYSize, LPINT lpDepth,
					LPFRMDATATYPE lpType)
/***********************************************************************/
{
LPFRAME lpFrame;

if ( !lpPixmap )
	return(FALSE);

if (pmType == PMT_EDIT)
	lpFrame = lpPixmap->EditFrame;
else if (pmType == PMT_UNDO)
	lpFrame = lpPixmap->UndoFrame;
else
	lpFrame = NULL;

if (!lpFrame)
	return(FALSE);

if (lpXSize)
	*lpXSize = FrameXSize(lpFrame);
if (lpYSize)
	*lpYSize = FrameYSize(lpFrame);
if (lpDepth)
	{
	*lpDepth = FrameDepth(lpFrame);
	if (*lpDepth == 0)
		*lpDepth = 1;
	}
if (lpType)
	*lpType = FrameType(lpFrame);
return( TRUE );
}

/***********************************************************************/
LPTR PixmapPtr( LPPIXMAP lpPixmap, PIXMAP_TYPE pmType,
			int x, int y, BOOL bModify )
/***********************************************************************/
{
LPFRAME lpFrame;

if ( !lpPixmap )
	return(NULL);

if (pmType == PMT_EDIT)
	lpFrame = lpPixmap->EditFrame;
else if (pmType == PMT_UNDO)
	lpFrame = lpPixmap->UndoFrame;
else
	lpFrame = NULL;

if (!lpFrame)
	return(NULL);

if (lpPixmap->UndoFrame && lpPixmap->EditFlags)
	{
	/* check x and y boundary conditions */
	if (y < 0 || y >= FrameYSize(lpFrame))
		return(NULL);
	if (x < 0 || x >= FrameXSize(lpFrame))
		return(NULL);

	if (pmType == PMT_UNDO)
		// make sure undo is up to date if caller want's undo
		PixmapSyncLine(lpPixmap, y);
	else if (bModify)
		{ // make sure undo is up to date for edited lines
		PixmapSyncLine(lpPixmap, y);
		lpPixmap->EditFlags[y] |= LINE_EDITED;
		}
	}

return( FramePointer(lpFrame, x, y, bModify) );
}

/***********************************************************************/
void PixmapCheck( LPPIXMAP lpPixmap, PIXMAP_TYPE pmType,
			int x, int y, BOOL bModify )
/***********************************************************************/
{
LPFRAME lpFrame;

if ( !lpPixmap )
	return;

if (pmType == PMT_EDIT)
	lpFrame = lpPixmap->EditFrame;
else if (pmType == PMT_UNDO)
	lpFrame = lpPixmap->UndoFrame;
else
	lpFrame = NULL;

if (!lpFrame)
	return;

if (lpPixmap->UndoFrame && lpPixmap->EditFlags)
	{
	/* check x and y boundary conditions */
	if (y < 0 || y >= FrameYSize(lpFrame))
		return;
	if (x < 0 || x >= FrameXSize(lpFrame))
		return;

	if (pmType == PMT_UNDO)
		// make sure undo is up to date if caller want's undo
		PixmapSyncLine(lpPixmap, y);
	else if (bModify)
		{ // make sure undo is up to date for edited lines
		PixmapSyncLine(lpPixmap, y);
		lpPixmap->EditFlags[y] |= LINE_EDITED;
		}
	}
}

/************************************************************************/
BOOL PixmapWrite( LPPIXMAP lpPixmap, PIXMAP_TYPE pmType,
					int sx, int sy, int dx, LPTR pixels, int count )
/************************************************************************/
{
LPFRAME lpFrame;

if ( !lpPixmap )
	return(FALSE);

if (pmType == PMT_EDIT)
	lpFrame = lpPixmap->EditFrame;
else if (pmType == PMT_UNDO)
	lpFrame = lpPixmap->UndoFrame;
else
	lpFrame = NULL;

if (!lpFrame)
	return(FALSE);

if (lpPixmap->UndoFrame && lpPixmap->EditFlags)
	{
	/* check x and y boundary conditions */
	if (sy < 0 || sy >= FrameYSize(lpFrame))
		return(FALSE);
	PixmapSyncLine(lpPixmap, sy);
	if (pmType == PMT_EDIT)
		lpPixmap->EditFlags[sy] |= LINE_EDITED;
	}
return( FrameWrite(lpFrame, sx, sy, dx, pixels, count) );
}

/***********************************************************************/
VOID PixmapSyncLines( LPPIXMAP lpPixmap )
/***********************************************************************/
{
int y, ylast;
LPTR lpFlags;

if ( !lpPixmap || lpPixmap->fNewFrame )
	return;
/* if no EditFrame or no UndoFrame, we can't sync lines */
if ( !lpPixmap->EditFrame || !lpPixmap->UndoFrame || !lpPixmap->EditFlags )
	return;
lpFlags = &lpPixmap->EditFlags[0];
ylast = FrameYSize(lpPixmap->EditFrame);
for ( y = 0; y < ylast; ++y )
	{ // Same as PixmapSyncLine()
	AstralBeachCursor( ID_BALL1 );
	if ( *lpFlags & UNDO_EMPTY )
		{
		FrameCopyLine( lpPixmap->EditFrame, lpPixmap->UndoFrame, y );
		*lpFlags ^= UNDO_EMPTY;
		}
	lpFlags++;
	}
AstralCursor(NULL);
}

/***********************************************************************/
VOID PixmapExchLines( LPPIXMAP lpPixmap, BYTE flag )
/***********************************************************************/
{
int y, ylast;
LPTR lpFlags;

if ( !lpPixmap || lpPixmap->fNewFrame )
	return;
/* if no EditFrame or no UndoFrame, we can't sync lines */
if ( !lpPixmap->EditFrame || !lpPixmap->UndoFrame || !lpPixmap->EditFlags )
	return;
lpFlags = &lpPixmap->EditFlags[0];
ylast = FrameYSize(lpPixmap->EditFrame);
for ( y = 0; y < ylast; ++y )
	{ // Same as PixmapSyncLine()
	AstralBeachCursor( ID_BALL1 );
	if ( *lpFlags & flag )
		FrameExchLine( lpPixmap->EditFrame, lpPixmap->UndoFrame, y );
	lpFlags++;
	}
AstralCursor(NULL);
}

/************************************************************************/
LPFRAME CALLBACK EXPORT PixmapFrame(LPPIXMAP lpPixmap, PIXMAP_TYPE pmType)
/************************************************************************/
{
if (!lpPixmap)
	return(NULL);
if (pmType == PMT_EDIT)
	return(lpPixmap->EditFrame);
else
if (pmType == PMT_UNDO)
	return(lpPixmap->UndoFrame);
else
	return(NULL);
}

/***********************************************************************/
void PixmapFree( LPPIXMAP lpPixmap )
/***********************************************************************/
{
/* free up EditFrame */
if (lpPixmap->EditFrame)
	{
	FrameClose(lpPixmap->EditFrame);
	lpPixmap->EditFrame = NULL;
	}
PixmapFreeUndo(lpPixmap);
}

/***********************************************************************/
void PixmapFreeUndo( LPPIXMAP lpPixmap )
/***********************************************************************/
{
/* free up UndoFrame */
if (lpPixmap->UndoFrame)
	{
	FrameClose(lpPixmap->UndoFrame);
	lpPixmap->UndoFrame = NULL;
	}

/* if we have EditFlags, get rid of them */
if (lpPixmap->EditFlags)
	{
	FreeUp(lpPixmap->EditFlags);
	lpPixmap->EditFlags = NULL;
	}
lpPixmap->fNewFrame = NO;
//lpPixmap->dirty = NO;
}

/************************************************************************/
void PixmapAddUndoArea(LPPIXMAP lpPixmap, LPRECT lpUndo)
/************************************************************************/
{
if (!lpPixmap)
	return;
AstralUnionRect(&lpPixmap->UndoRect, &lpPixmap->UndoRect, lpUndo);
}

/***********************************************************************/
BOOL PixmapEditInit( LPPIXMAP lpPixmap, BOOL UseApply)
/***********************************************************************/
{
if ( !lpPixmap || !lpPixmap->EditFrame )
	return( FALSE );

// Make sure the UndoFrame exists
if ( !lpPixmap->UndoFrame )
	return( PixmapInitUndo(lpPixmap) );

// If not manual apply, or manual apply and an undo was just done,
// then apply any changes (after syncing the frames)
if ( !UseApply /*|| ( UseApply && !lpPixmap->UndoNotRedo )*/ )
	PixmapEditApply(lpPixmap);

/*lpPixmap->UndoNotRedo = TRUE;*/
return(TRUE);
}

/***********************************************************************/
void PixmapEditUndo( LPPIXMAP lpPixmap )
/***********************************************************************/
{
LPFRAME tempFrame;
BOOL fSwapFrames = YES;
int y, ylast;
int nUndoEmpty, nLineEdited;
LPTR lpFlags;
BYTE flag;

/* if no EditFrame or no UndoFrame, we can't do an Undo */
if ( !lpPixmap->EditFrame || !lpPixmap->UndoFrame )
	return;

AstralCursor(IDC_WAIT);
MessageStatus( IDS_SWAPPINGUNDO );

// Sync up the UndoFrame with the EditFrame, since we'll be switching over to it
if (!lpPixmap->fNewFrame)
	{
	nUndoEmpty = 0;
	nLineEdited = 0;
	lpFlags = &lpPixmap->EditFlags[0];
	ylast = FrameYSize(lpPixmap->EditFrame);
	for ( y = 0; y < ylast; ++y )
		{
		flag = *lpFlags++;
		if ( flag & UNDO_EMPTY )
			++nUndoEmpty;
		else
		if ( flag & LINE_EDITED )
			++nLineEdited;
		}
	if (nLineEdited >= nUndoEmpty)
		PixmapSyncLines(lpPixmap);
	else
		{
		PixmapExchLines(lpPixmap, LINE_EDITED);
		fSwapFrames = NO;
		}
	}


// Undo by just switching the UndoFrame and EditFrame
if (fSwapFrames)
	{
	tempFrame = lpPixmap->EditFrame;
	lpPixmap->EditFrame = lpPixmap->UndoFrame;
	lpPixmap->UndoFrame = tempFrame;
	}

AstralCursor(NULL);
MessageStatus(NULL);

/*lpPixmap->UndoNotRedo = !lpPixmap->UndoNotRedo;*/
}


/***********************************************************************/
void PixmapEditApply( LPPIXMAP lpPixmap )
/***********************************************************************/
{
int y, ylast;
LPFRAME lpFrame;
int EditInMemory, UndoInMemory;
BOOL fNewFrame;
LPTR lpFlags;

/* if no EditFrame or no UndoFrame, we can't do an Apply */
if ( !lpPixmap || !lpPixmap->EditFrame || !lpPixmap->UndoFrame )
	return;

/* save the fNewFrame state, because PixmapInitUndo's job is to */
/* get EditFrame and UndoFrame in sync, and will clear the flag */
fNewFrame = lpPixmap->fNewFrame;

/* get EditFrame and UndoFrame in sync.  This means either to create */
/* an UndoFrame originally or make sure they are not different sizes */
PixmapInitUndo(lpPixmap);
if ( !lpPixmap->UndoFrame || !lpPixmap->EditFlags )
	return;

// Set the EditFlags
lpFlags = &lpPixmap->EditFlags[0];
ylast = FrameYSize(lpPixmap->EditFrame);
for ( y = 0; y < ylast; ++y )
	{
	if ( *lpFlags & LINE_EDITED )
		{ // if the line is marked as editted ...
		// force the undo buffer to get the line when next we sync
		*lpFlags |= UNDO_EMPTY;
		*lpFlags ^= LINE_EDITED;
		}
	lpFlags++;
	}

//lpPixmap->dirty = NO;
/*lpPixmap->UndoNotRedo = TRUE;*/
lpPixmap->UndoRect.left = lpPixmap->UndoRect.top = 32767;
lpPixmap->UndoRect.right = lpPixmap->UndoRect.bottom = -32767;

// Check to see if we should flip the frames to maximize memory usage
UndoInMemory = FrameInMemory( lpPixmap->UndoFrame );
EditInMemory = FrameInMemory( lpPixmap->EditFrame ) + PERCENT_WORTHWHILE;
/* here we check which frame has the most memory, we want */
/* our EditFrame to always have the most */
if ( UndoInMemory > EditInMemory ) /* make sure its worth our while */
	{
	PixmapSyncLines(lpPixmap); // Make two duplicate frames

	/* if we had a new frame, there is probably extra memory */
	/* we could take for the EditFrame */
	if (fNewFrame)
		{
		FrameClose(lpPixmap->EditFrame);
		lpPixmap->EditFrame = lpPixmap->UndoFrame;
		lpPixmap->UndoFrame = NULL;
		PixmapInitUndo(lpPixmap);
		}
	else
		{
		/* swap frames, so EditFrame has the most memory */
		lpFrame = lpPixmap->EditFrame;
		lpPixmap->EditFrame = lpPixmap->UndoFrame;
		lpPixmap->UndoFrame = lpFrame;
		}
	}
}


/***********************************************************************/
void PixmapNewFrame( LPPIXMAP lpPixmap, LPFRAME lpNewFrame, LPRECT lpEditRect,
					BOOL fUseApply)
/***********************************************************************/
{
int	y;
BOOL fNotNewFrame;

if ( !lpNewFrame )	/* this frame should be different than EditFrame */
	return;			/* this should be called for any type of process function */
					/* where the undo frame will become different than the */
					/* edit frame (i.e. different size, mirrored, etc. */

if (Control.NoUndo && lpPixmap->UndoFrame)
	PixmapFreeUndo(lpPixmap);
if ( fUseApply )
	{
	/* if we have an undo frame, leave it & throw away the edit frame */
	if (lpPixmap->UndoFrame)
		{
		PixmapSyncLines(lpPixmap); // sync up undo before getting rid of edit
		if (lpPixmap->EditFrame)
			{
			FrameClose(lpPixmap->EditFrame);
			lpPixmap->EditFrame = NULL;
			}
		}
	else
		{
		if (Control.NoUndo && lpPixmap->EditFrame)
			FrameClose(lpPixmap->EditFrame);
		else
			lpPixmap->UndoFrame = lpPixmap->EditFrame;
//		lpImage->UndoDataType = lpImage->DataType;
		}
	}
else
	{
	/* if we have an undo frame, throw it away, and use the edit frame */
	if (lpPixmap->UndoFrame)
		FrameClose(lpPixmap->UndoFrame);
	if (Control.NoUndo && lpPixmap->EditFrame)
		FrameClose(lpPixmap->EditFrame);
	else
		lpPixmap->UndoFrame = lpPixmap->EditFrame;
//	lpImage->UndoDataType = lpImage->DataType;
	}

/* set up new EditFrame for editing */
lpPixmap->EditFrame = lpNewFrame;

/* if we have EditFlags, get rid of them */
if (lpPixmap->EditFlags)
	{
	FreeUp(lpPixmap->EditFlags);
	lpPixmap->EditFlags = NULL;
	}

// reset some undo variables that are the same for both cases
//lpPixmap->dirty = NO;
//lpPixmap->UndoNotRedo = TRUE;

// if a rectangle is passed in, it means that changes have been made
// to a copy of the current EditFrame. This means a normal undo can be used
// instead of the fNewFrame case, which assumes the frames are different sizes
// If lpEditRect is not NULL, the frames should be the same size, however
// we will do the check anyways.
fNotNewFrame = lpEditRect &&
		FrameXSize(lpPixmap->EditFrame) == FrameXSize(lpPixmap->UndoFrame) &&
		FrameYSize(lpPixmap->EditFrame) == FrameYSize(lpPixmap->UndoFrame);
if (fNotNewFrame && 
	(FrameType(lpPixmap->EditFrame) == FrameType(lpPixmap->UndoFrame)) &&
	PixmapCreateFlags(lpPixmap, 0))
	{
	/* reset variables use for undo */
	lpPixmap->fNewFrame = 0;
	lpPixmap->UndoRect = *lpEditRect;
	BoundRect(&lpPixmap->UndoRect, 0, 0,
		FrameXSize(lpPixmap->EditFrame)-1, FrameYSize(lpPixmap->EditFrame)-1);
	for (y = lpPixmap->UndoRect.top; y <= lpPixmap->UndoRect.bottom; ++y)
		lpPixmap->EditFlags[y] |= LINE_EDITED;
	}
else
	{
	/* reset variables use for undo */
	lpPixmap->fNewFrame = FRAME_NEW;
	if (fNotNewFrame &&
		(FrameType(lpPixmap->EditFrame) != FrameType(lpPixmap->UndoFrame)))
		lpPixmap->fNewFrame |= FRAME_NEWDEPTH;
	lpPixmap->UndoRect.left = lpPixmap->UndoRect.top = 32767;
	lpPixmap->UndoRect.right = lpPixmap->UndoRect.bottom = -32767;
	}
}

/***********************************************************************/
static BOOL PixmapCreateUndo( LPPIXMAP lpPixmap )
/***********************************************************************/
{
// open the UndoFrame
if (!(lpPixmap->UndoFrame = FrameOpen(FrameType(lpPixmap->EditFrame),
	FrameXSize(lpPixmap->EditFrame), FrameYSize(lpPixmap->EditFrame),
	FrameResolution(lpPixmap->EditFrame))))
	{
//	lpPixmap->UndoFailed = TRUE;
	}
// if we have EditFlags, get rid of them
if (lpPixmap->EditFlags)
	{
	FreeUp(lpPixmap->EditFlags);
	lpPixmap->EditFlags = NULL;
	}
if (!lpPixmap->UndoFrame)
	return(FALSE);
//lpImage->UndoPixmapType = lpImage->PixmapType;
return(TRUE);
}

/***********************************************************************/
static BOOL PixmapCreateFlags( LPPIXMAP lpPixmap, BYTE bFlags )
/***********************************************************************/
{
if ( lpPixmap->EditFlags = Alloc((long)FrameYSize(lpPixmap->UndoFrame)) )
	{ // Set all undo flags to empty
	set(lpPixmap->EditFlags, FrameYSize(lpPixmap->UndoFrame), bFlags);
//	lpPixmap->UndoNotRedo = TRUE;
	return(TRUE);
	}
else
	{
//	lpPixmap->UndoFailed = TRUE;
	/* get rid of the UndoFrame */
	FrameClose(lpPixmap->UndoFrame);
	lpPixmap->UndoFrame = NULL;
	return(FALSE);
	}
}


/***********************************************************************/
static VOID PixmapSyncLine( LPPIXMAP lpPixmap, int y )
/***********************************************************************/
{
LPTR lpFlags;

if ( !lpPixmap || lpPixmap->fNewFrame )
	return;
/* if no EditFrame or no UndoFrame, we can't sync lines */
if ( !lpPixmap->EditFrame || !lpPixmap->UndoFrame || !lpPixmap->EditFlags )
	return;
lpFlags = &lpPixmap->EditFlags[y];
if ( *lpFlags & UNDO_EMPTY )
	{
	FrameCopyLine( lpPixmap->EditFrame, lpPixmap->UndoFrame, y );
	*lpFlags ^= UNDO_EMPTY;
	}
}



