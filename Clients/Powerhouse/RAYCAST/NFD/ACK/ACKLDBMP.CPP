/******************* ( Animation Construction Kit 3D ) ******************/
/*			  Load Bitmap Routines				     															*/
/* CopyRight (c) 1993	   Author: Lary Myers				     									*/
/* CopyRight (c) 1994/5	 John Lundy															     		*/
/* Code enhanced, modified, fixed and adapted to Windows by John Lundy	*/
/************************************************************************/

#include "stdafx.h"
#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"

char *GetExtent(char *s);
HGLOBAL AckReadiff(char *s);

/****************************************************************************
** The application should call this routine to load a bitmap into			*
** memory.																	*
**																			*
** BitmapNumber is the index number to place the file into					*
** BitmapType is either TYPE_WALL or TYPE_OBJECT							*
** BitmapName is the filename of the bitmap to load.						*
**																			*
** Bitmaps used by the ACK engine are 64x64 in size and may be either raw	*
** image (.IMG) files or Deluxe Paint brushes (.LBM) files.					*
****************************************************************************/
int AckLoadBitmap(ACKENG *ae,int BitmapNumber,int BitmapType,char *BitmapName)
{
int	    	bFlag=0;
int	    	x, y;
int	    	sPos, dPos;
unsigned char ch;
HGLOBAL		hbuf;
HGLOBAL		hbmp;
UCHAR    	far *buf;
UCHAR    	far *bmp;
FILE		*handle;

	if (!(stricmp(GetExtent(BitmapName),"LBM")))
    {
    	hbuf = AckReadiff(BitmapName);
    	if (hbuf == NULL)
			return(ERR_LOADINGBITMAP);
		buf = (unsigned char far *)GlobalLock(hbuf);
    	memmove(buf,&buf[4],BITMAP_SIZE);
    	bFlag = 1;
    }
	else
    {
    	hbuf = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, BITMAP_SIZE);
    	if (hbuf == NULL)
			return(ERR_NOMEMORY);
		buf = (unsigned char far *)GlobalLock(hbuf);
    }

	hbmp = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, BITMAP_SIZE);
	if (hbmp == NULL)
    {
    	GlobalUnlock(hbuf);
    	GlobalFree(hbuf);
    	return(ERR_NOMEMORY);
    }
	bmp = (unsigned char far *)GlobalLock(hbmp);

	if (BitmapType == TYPE_WALL)
	{
    	hbMaps[BitmapNumber] = hbmp;
    	bMaps[BitmapNumber] = bmp;
    }
	else
	if (BitmapType == TYPE_OBJECT)
	{
	    hoMaps[BitmapNumber] = hbmp;
	    oMaps[BitmapNumber] = bmp;
	}

	if (!bFlag)
    {
    	handle = fopen(BitmapName,"r+b");
    	if (handle == NULL)
		{
    		GlobalUnlock(hbuf);
    		GlobalFree(hbuf);
    		GlobalUnlock(hbmp);
    		GlobalFree(hbmp);
			return(ERR_BADFILE);
		}
		fseek(handle,4,SEEK_CUR);		/* Skip width and height for now */
    	fread(buf,BITMAP_SIZE,1,handle);
    	fclose(handle);
    }

	for (y = 0; y < BITMAP_HEIGHT; y++)
    {
    	sPos = y;
    	dPos = y * BITMAP_WIDTH;
    	for (x = 0; x < BITMAP_WIDTH; x++)
		{
			ch = buf[sPos];
			bmp[dPos++] = ch;
			sPos += BITMAP_WIDTH;
		}
    }

GlobalUnlock(hbuf);
GlobalFree(hbuf);
return(0);
}

/****************************************************************************
** Internal routine to get the file extent from a filename.					*
****************************************************************************/
char *GetExtent(char *s)
{
    char    *e;

e = strchr(s,'.');
if (e == NULL)
    return(s);
e++;

return(e);
}


/****************************************************************************
** Simplified call to load a wall bitmap.									*
****************************************************************************/
int AckLoadWall(ACKENG *ae,int WallNumber,char *bmFileName)
{

return( AckLoadBitmap(ae,WallNumber,TYPE_WALL,bmFileName) );
}


/****************************************************************************
** Simplified call to load an object bitmap.								*
****************************************************************************/
int AckLoadObject(ACKENG *ae,int BmpNumber,char *bmFileName)
{

return( AckLoadBitmap(ae,BmpNumber,TYPE_OBJECT,bmFileName) );
}


/****************************************************************************
** The application should make this call to perform the necessary setup on	*
** an object.																*
**																			*
** ObjNumber is the object index											*
** NumBitmaps is the total number of bitmaps that the object has (min 1)	*
** bmNums is a char array of the bitmap numbers the object uses.			*
**																			*
** If an object has more than 1 bitmap and the OF_ANIMATE flag is NOT set	*
** then it is assumed the object has more than one side and will be seen	*
** from various angles. If the OF_ANIMATE flag IS set then the object will	*
** animate through the multiple bitmaps in the supplied list.				*
****************************************************************************/
int AckCreateObject(ACKENG *ae,int ObjNumber,int NumBitmaps,UCHAR *bmNums,char *mvFlgs, char *bmDsps)
{
    int	    i;

if (NumBitmaps >= MAX_VIEWS)
    return(ERR_TOMANYVIEWS);

if (!NumBitmaps)    /* Force to at least one bitmap */
    NumBitmaps++;

/* Keep the maximum number current */
if (ObjNumber >= ae->MaxObjects && ObjNumber < MAX_OBJECTS)
    ae->MaxObjects = ObjNumber + 1;

/* Setup index to which bitmap is displayed */
ObjList[ObjNumber]->CurNum = 0;

/* Calculate sides if a multi-sided object */
ObjList[ObjNumber]->Sides = 0;

/* Transfer bitmap numbers to object structure */
for (i = 0; i < NumBitmaps; i++)
    ObjList[ObjNumber]->bmNum[i] = bmNums[i];

/* Transfer movement allowed flag to object structure */
for (i = 0; i < NumBitmaps; i++)
    ObjList[ObjNumber]->mvFlg[i] = mvFlgs[i];

/* Transfer display bitmap count to object structure */
for (i = 0; i < NumBitmaps; i++)
    ObjList[ObjNumber]->bmDsp[i] = bmDsps[i];

// Point the active information at the static stuff
ae->ObjList[ObjNumber].bmNum = ObjNumber;

return(0);
}
