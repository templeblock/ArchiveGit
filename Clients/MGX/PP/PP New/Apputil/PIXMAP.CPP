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

#include "stdafx.h"
#include "pixmap.h"
#include "imgutil.h"

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


