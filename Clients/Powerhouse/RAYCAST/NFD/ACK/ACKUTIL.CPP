/******************* ( Animation Construction Kit 3D ) ******************/
/*			    Utility Routines				     																*/
/* CopyRight (c) 1993	   Author: Lary Myers				     									*/
/* CopyRight (c) 1994/5	 John Lundy															     		*/
/* Code enhanced, modified, fixed and adapted to Windows by John Lundy	*/
/************************************************************************/

#include "stdafx.h"
#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"

/*****************************************************************************
** The application calls this routine to get the last object hit (from the	**
** AckMovePOV() or AckMoveObjectPOV() routines.				   				**
*****************************************************************************/
int AckGetObjectHit(void)
{

return(LastObjectHit);
}

/*****************************************************************************
** The application calls this routine to get the last wall hit (from the	**
** AckMovePOV() or AckMoveObjectPOV() routines.				   				**
*****************************************************************************/
int AckGetWallHit(void)
{

return(LastMapPosn);
}


/*****************************************************************************
** The application calls this routine to make an object invisible. The		**
** object and object bitmaps are NOT deleted. Currently the active flag		**
** is set to zero however, future versions may do more in the routine so	**
** it would be a good idea for the application to call here instead of		**
** setting the active flag to zero on its own.				   				**
*****************************************************************************/
int AckDeleteObject(ACKENG *ae,int ObjIndex)
{

if (!ae->ObjList[ObjIndex].Active)
    return(-1);

ae->ObjList[ObjIndex].Active = 0;

return(0);
}

/*****************************************************************************
** The purpose of this routine is to replace an existing bitmap with a		**
** new one. The old bitmap is freed and the new one is put into place in	**
** the specified bitmap array.						   						**
**									   										**
** index is the number of the bitmap array to replace			   			**
** Maps is the bitmap array						   							**
** NewBitmap is a pointer to the new bitmap to use			   				**
*****************************************************************************/
int AckSetNewBitmap(int index,UCHAR far **Maps,UCHAR far *NewBitmap)
{
    UCHAR   far	    *bPtr;

bPtr = Maps[index];
if (bPtr == NewBitmap)
	return(0);

free(bPtr);
Maps[index] = NewBitmap;

return(0);
}


/*****************************************************************************
** This routine should be called for all wall and object bitmaps.			**
*****************************************************************************/
UCHAR far *AckGetBitmapPtr(int index,UCHAR far **Maps)
{
    UCHAR   far *bPtr;

bPtr = Maps[index];

    return(bPtr);
}

/*****************************************************************************
** This routine returns the supplied pointer.								**
*****************************************************************************/
UCHAR far *AckLockPtr(UCHAR far *bPtr)
{
    return(bPtr);
}


/*****************************************************************************
** This routine does nothing.												**
*****************************************************************************/
int AckUnlockPtr(UCHAR far *bPtr)
{
    return(0);
}


/*****************************************************************************
** This routine creates a new bitmap buffer and copies the supplied bitmap	**
** into it.																	**
*****************************************************************************/
HGLOBAL AckCopyNewBitmap(UCHAR far *bPtr)
{
HGLOBAL		hbmp;
UCHAR		far *bmp;
hbmp = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, (long) BITMAP_SIZE);
if (hbmp)
{
	bmp = (unsigned char far *)GlobalLock(hbmp);
	memmove(bmp,bPtr,BITMAP_SIZE);
}
return(hbmp);
}
