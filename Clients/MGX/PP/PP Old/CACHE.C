// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®TP0¯®BT0¯®PL1¯®FD1¯*/
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

extern HANDLE hInstAstral;
extern HWND hWndAstral;

#define PERCENT_WORTHWHILE 10

/***********************************************************************/
void CacheNewFrame(lpNewFrame)
/***********************************************************************/
LPFRAME lpNewFrame;
{
if ( !lpNewFrame ) /* this frame should be different than EditFrame */
	return;    /* this should happen for any type of process function */
		   /* where are undo frame willl become different than the */
		   /* edit frame (i.e. different size, mirrored, etc.  */

/* reset variables use for undo */
lpImage->fNewFrame = TRUE;
lpImage->dirty = NO;
lpImage->UndoNotRedo = TRUE;
lpImage->UndoRect.left = lpImage->UndoRect.top = 32767;
lpImage->UndoRect.right = lpImage->UndoRect.bottom = -32767;

/* if we have an undo frame, throw it away, it's useless now */
if (lpImage->UndoFrame)
	frame_close(lpImage->UndoFrame);
lpImage->UndoFrame = NULL;

/* make the current EditFrame the UndoFrame for future undo's */
if (lpImage->EditFrame)
	lpImage->UndoFrame = lpImage->EditFrame;
lpImage->EditFrame = lpNewFrame;

/* set up new EditFrame for editing */
frame_set(lpImage->EditFrame);

/* throw away EditFlags for old frame */
if (lpImage->EditFlags)
	{
	FreeUp(lpImage->EditFlags);
	lpImage->EditFlags = NULL;
	}
}


/***********************************************************************/
void CacheFree(lpImage)
/***********************************************************************/
LPIMAGE lpImage;
{
/* free up EditFrame */
if (lpImage->EditFrame)
	{
	frame_close(lpImage->EditFrame);
	lpImage->EditFrame = NULL;
	}

/* free up UndoFrame */
if (lpImage->UndoFrame)
	{
	frame_close(lpImage->UndoFrame);
	lpImage->UndoFrame = NULL;
	}

/* free up the EditFlags */
if (lpImage->EditFlags)
	{
	FreeUp(lpImage->EditFlags);
	lpImage->EditFlags = NULL;
	}

/* reset NoUndo, just to be safe */
lpImage->NoUndo = FALSE;
}


/***********************************************************************/
void CacheUndo()
/***********************************************************************/
{
int y, ytotal, lines;
LPFRAME tempFrame;
BOOL UndoNotRedo;

/* if no EditFrame or no UndoFrame, we can't do an Undo */
if ( !lpImage->EditFrame || !lpImage->UndoFrame )
	return;

/* an Undo when using manual apply works differently, */
/* the Undo's are permanent */
if (Control.UseApply)
	{
	if  (!lpImage->fNewFrame) /* undo and edit different? */
		{
		/* if same, copy dirty lines from UndoFrame to EditFrame */
		for (y = 0, ytotal = 0; y < lpImage->EditFrame->Ysize; ++y)
			if (lpImage->EditFlags[y] & LINE_EDITED)
				++ytotal;
		AstralCursor(IDC_WAIT);
		for (y = 0, lines = 0; y < lpImage->EditFrame->Ysize; ++y)
			{
			if (lpImage->EditFlags[y] & LINE_EDITED)
				{
				AstralClockCursor(lines++, ytotal);
				frame_copyline(lpImage->UndoFrame,
					lpImage->EditFrame, y);
				lpImage->EditFlags[y] ^= LINE_EDITED;
				}
			}
		AstralCursor(NULL);
		if (lpImage->UndoRect.right >= lpImage->UndoRect.left)
			UpdateImage(&lpImage->UndoRect, YES);
		}
	else	{
		/* if different, throw away EditFrame and go permanently */
		/* back to the UndoFrame for editing */
		frame_close(lpImage->EditFrame);
		lpImage->EditFrame = lpImage->UndoFrame;
		lpImage->UndoFrame = NULL;
		lpImage->fNewFrame = FALSE;
		frame_set(lpImage->EditFrame);
		UpdateImageSize();
		lpImage->UndoRect.left = lpImage->UndoRect.top = 32767;
		lpImage->UndoRect.right = lpImage->UndoRect.bottom = -32767;
		}
	lpImage->dirty = NO;
	}
else	
	{
	/* Sync up the dirty lines in the EditFrame */
	if (!lpImage->fNewFrame)
		CacheSyncLines();

	/* just swap the UndoFrame and EditFrame for the Undo */
	tempFrame = lpImage->EditFrame;
	lpImage->EditFrame = lpImage->UndoFrame;
	lpImage->UndoFrame = tempFrame;
	frame_set(lpImage->EditFrame);
	lpImage->UndoNotRedo = !lpImage->UndoNotRedo;
	if (lpImage->fNewFrame)
		{
		/* if new frame, cause window to be redisplayed */
		UndoNotRedo = lpImage->UndoNotRedo;
		UpdateImageSize();
		lpImage->UndoNotRedo = UndoNotRedo;
		}
	else 
		{
		if (lpImage->UndoFrame->WriteMask)
			{
			/* make sure to maintain mask for new EditFrame */
			if (lpImage->EditFrame->WriteMask)
				mask_close(lpImage->EditFrame->WriteMask);
			lpImage->EditFrame->WriteMask = lpImage->UndoFrame->WriteMask;
			lpImage->UndoFrame->WriteMask = NULL;
			}
		if (lpImage->UndoRect.right >= lpImage->UndoRect.left)
			UpdateImage(&lpImage->UndoRect, YES);
		}
	}
}


/***********************************************************************/
LPTR CachePtr(Depth, x, y, bModify )
/***********************************************************************/
int Depth, x, y;
BOOL bModify;
{
LPFRAME lpFrame;

if (lpImage->EditFrame) /* if no EditFrame, this works the same as */
			/* frame_ptr */
	{
	lpFrame = frame_set(NULL);
	/* if current frame is not EditFrame or the intent is not */
	/* to modify or no UndoFrame, just call frame_ptr */
	if (lpFrame == lpImage->EditFrame && bModify && lpImage->UndoFrame && lpImage->EditFlags)
		{
		/* check x and y boundary conditions */
		if (y < 0 || y >= lpFrame->Ysize)
			return(NULL);
		if (x < 0 || x >= lpFrame->Xsize)
			return(NULL);
		/* if a line is marked as dirty, copy it to the UndoFrame */
		CacheSyncLine(y);
		lpImage->EditFlags[y] |= LINE_EDITED;
		lpImage->fChanged = YES;
		}
	else
	if ( lpFrame == lpImage->UndoFrame && lpImage->EditFlags )
		{
		/* check x and y boundary conditions */
		if (y < 0 || y >= lpFrame->Ysize)
			return(NULL);
		if (x < 0 || x >= lpFrame->Xsize)
			return(NULL);
		/* if a line is marked as dirty, copy it to the UndoFrame */
		CacheSyncLine(y);
		}
	}

return(frame_ptr(0, x, y, bModify));
}


/***********************************************************************/
BOOL CacheInitUndo()
/***********************************************************************/
{
BOOL status = TRUE;

/* if we have an UndoFrame and we also have a NewFrame, then */
/* UndoFrame and EditFrame are different and must be synced */
/* by first getting rid of the UndoFrame */ 
if (lpImage->UndoFrame && lpImage->fNewFrame)
	{
	frame_close(lpImage->UndoFrame);
	lpImage->UndoFrame = NULL;
	lpImage->fNewFrame = FALSE;
	if (lpImage->EditFlags)
		{
		FreeUp(lpImage->EditFlags);
		lpImage->EditFlags = NULL;
		}
	}

/* If we don't have an UndoFrame, and we haven't failed creating */
/* one before, create one */
if (!lpImage->NoUndo && !lpImage->UndoFrame)
        {
	//StatusBox(IDS_CREATEUNDO);
	//AstralCursor(IDC_WAIT);
	//status = FALSE;
	/* copy the EditFrame into the UndoFrame */
	if (!(lpImage->UndoFrame = frame_open(lpImage->EditFrame->Depth,
		lpImage->EditFrame->Xsize, lpImage->EditFrame->Ysize,
		lpImage->EditFrame->Resolution)))
	//if (!(lpImage->UndoFrame = frame_copy(lpImage->EditFrame)))
		{
		Print("Undo will not work on some operations");
		lpImage->NoUndo = TRUE;
		}
	//StatusBox(IDS_CREATEUNDO);
	/* if we have EditFlags, get rid of them */
	if (lpImage->EditFlags)
		{
		FreeUp(lpImage->EditFlags);
		lpImage->EditFlags = NULL;
		}
	}
/* if we have an UndoFrame and no EditFlags, create the EditFlags */
if (lpImage->UndoFrame && !lpImage->EditFlags)
	{
	lpImage->EditFlags = Alloc((long)lpImage->UndoFrame->Ysize);
	if (!lpImage->EditFlags)
		{
		Message(IDS_NOUNDO);
		lpImage->NoUndo = TRUE;
		/* get rid of the UndoFrame */
		frame_close(lpImage->UndoFrame);
		lpImage->UndoFrame = NULL;
		}
	else	
		{
		//clr(lpImage->EditFlags, lpImage->UndoFrame->Ysize);
		set(lpImage->EditFlags, lpImage->UndoFrame->Ysize, LINE_DIRTY);
		lpImage->UndoNotRedo = TRUE;
		}
	}
return(status);
}


/***********************************************************************/
void CacheApply()
/***********************************************************************/
{
int y, ytotal, lines;
LPFRAME lpFrame;
int EditInMemory, UndoInMemory;
BOOL fNewFrame;

if (!lpImage->EditFrame)
	return;

/* save the fNewFrame state, because CacheInitUndo's job is to */
/* get EditFrame and UndoFrame in sync, and will clear the flag */
fNewFrame = lpImage->fNewFrame;

/* get EditFrame and UndoFrame in sync.  This means either to create */
/* an UndoFrame originally or make sure they are not different */
CacheInitUndo();
if (lpImage->UndoFrame)
	{
	lpFrame = lpImage->EditFrame;
	/* copy all edited lines to to UndoFrame */
	for (y = 0, ytotal = 0; y < lpFrame->Ysize; ++y)
		if (lpImage->EditFlags[y] & LINE_EDITED)
			++ytotal;
	AstralCursor(IDC_WAIT);
	for (y = 0, lines = 0; y < lpFrame->Ysize; ++y)
		{
		if (Control.UseApply)
			{
			if (lpImage->EditFlags[y] & LINE_EDITED)
				{
				AstralClockCursor(lines++, ytotal);
				frame_copyline( lpImage->EditFrame,
						lpImage->UndoFrame, y );
				lpImage->EditFlags[y] ^= LINE_EDITED;
				}
			}
		else
			{
			lpImage->EditFlags[y] ^= LINE_EDITED;
			lpImage->EditFlags[y] |= LINE_DIRTY;
			}
		}
	AstralCursor(NULL);
	lpImage->dirty = NO;
	lpImage->UndoNotRedo = TRUE;
	lpImage->UndoRect.left = lpImage->UndoRect.top = 32767;
	lpImage->UndoRect.right = lpImage->UndoRect.bottom = -32767;
	frame_set(lpImage->UndoFrame);
	UndoInMemory = frame_inmemory();
	frame_set(lpImage->EditFrame);
	EditInMemory = frame_inmemory() + PERCENT_WORTHWHILE;
	/* here we check which frame has the most memory, we want */
	/* our EditFrame to always have the most */
        if (UndoInMemory > (EditInMemory)) /* make sure its worth our while */
		{
		CacheSyncLines();
		if (lpImage->UndoFrame->WriteMask)
			mask_close(lpImage->UndoFrame->WriteMask);
		lpImage->UndoFrame->WriteMask = lpImage->EditFrame->WriteMask;
		lpImage->EditFrame->WriteMask = NULL;

		/* if we had a new frame, there is probably extra memory */
		/* we could take for the EditFrame */
		if (fNewFrame)
			{
			frame_close(lpImage->EditFrame);
			lpImage->EditFrame = lpImage->UndoFrame;
			lpImage->UndoFrame = NULL;
			CacheInitUndo();
			}
		else	{
			/* swap frames, so EditFrame has the most memory */
			lpFrame = lpImage->EditFrame;
			lpImage->EditFrame = lpImage->UndoFrame;
			lpImage->UndoFrame = lpFrame;
			}
		frame_set(lpImage->EditFrame);
		}
	}
}


/***********************************************************************/
BOOL CacheInit(fForceApplyUndo, fNeedUndo)
/***********************************************************************/
BOOL fForceApplyUndo;
BOOL fNeedUndo;
{
if ( !lpImage )
	return( FALSE );
if (!lpImage->EditFrame)
	return(FALSE);

if (fForceApplyUndo)
	if (!CacheCheckApplyUndo())
		return(FALSE);

if (!fNeedUndo)
	return(TRUE);

/* Make sure the EditFrame and UndoFrame are in sync */
if (!CacheInitUndo())
    return(FALSE);
/* If not manual apply, automatically apply any changes */
if (!Control.UseApply && lpImage->UndoFrame)
	CacheApply();
return(TRUE);
}

/***********************************************************************/
VOID CacheSyncLines()
/***********************************************************************/
{
int y;

if (!lpImage->fNewFrame && lpImage->EditFrame && lpImage->UndoFrame && lpImage->EditFlags)
	{
	for (y = 0; y < lpImage->EditFrame->Ysize; ++y)
		CacheSyncLine(y);
	}
}
/***********************************************************************/
VOID CacheSyncLine(y)
/***********************************************************************/
int y;
{
if (lpImage->EditFlags[y] & LINE_DIRTY)
	{
	frame_copyline(lpImage->EditFrame, lpImage->UndoFrame, y );
	lpImage->EditFlags[y] ^= LINE_DIRTY;
	}
}

/************************************************************************/
BOOL CacheWrite( span, pixels, count )
/************************************************************************/
LPSPAN	span;
LPTR	pixels;
int	count;
{
LPFRAME lpFrame;

lpFrame = frame_set(NULL);
if (!lpFrame)
	return(FALSE);
if (lpFrame == lpImage->EditFrame && lpImage->UndoFrame && lpImage->EditFlags && span->sy >= 0 && span->sy < lpFrame->Ysize)
	{
	CacheSyncLine(span->sy);
	lpImage->EditFlags[span->sy] |= LINE_EDITED;
	lpImage->fChanged = YES;
	}
return(frame_write(span, pixels, count));
}

/************************************************************************/
BOOL CacheRead( span, pixels, count )
/************************************************************************/
LPSPAN	span;
LPTR	pixels;
int	count;
{
return(frame_read(span, pixels, count));
}


/************************************************************************/
BOOL CacheCheckApplyUndo()
/************************************************************************/
{
if (Control.UseApply && lpImage->UndoFrame && lpImage->fNewFrame)
	{
	/* tell user he must force frames to be the same for editing */
	return( AstralDlg( NO, hInstAstral, hWndAstral, IDD_APPLYORUNDO,
		DlgApplyUndoProc ) );
	}
return(TRUE);
}
