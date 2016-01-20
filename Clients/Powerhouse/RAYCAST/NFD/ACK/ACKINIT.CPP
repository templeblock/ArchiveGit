/******************* ( Animation Construction Kit 3D ) ******************/
/*			 Initialization Routines			     															*/
/* CopyRight (c) 1993	   Author: Lary Myers				     									*/
/* CopyRight (c) 1994/5	 John Lundy															     		*/
/* Code enhanced, modified, fixed and adapted to Windows by John Lundy	*/
/************************************************************************/

#include "stdafx.h"
#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"

#include <math.h>

extern int			ViewLowRes;

int BuildAckTables(ACKENG *ae);
int ReadAckMapFile(char *fName);
void AckBuildHeightTables(ACKENG *ae);
void BuildAckGrid(ACKENG *ae);

/*****************************************************************************
* This **MUST** be the first routine called by the application to setup			*
* the ACK engine. See the file ACK3D.DOC for what fields need to be					*
* setup by the application prior to making this call.												*
*****************************************************************************/
int AckInitialize(ACKENG *ae)
{
int		result = 0;

result = BuildAckTables(ae);  /* Read in TRIG.DAT and allocate tables */
if (result)
    return(result);

AckBuildHeightTables(ae);   /* Build height and adjustment tables */

return(result);
}

/*****************************************************************************
* Internal call to read in the TRIG.DAT file.																*
*****************************************************************************/
int BuildAckTables(ACKENG *ae)
{
int					ca,len,idx;
long				grs=0;
double			a, b;
long				c;

hLongTanTable = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(long) * INT_ANGLE_360);
LongTanTable = (long far *)GlobalLock(hLongTanTable);

hLongInvTanTable = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(long) * INT_ANGLE_360);
LongInvTanTable = (long far *)GlobalLock(hLongInvTanTable);

hCosTable = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(long) * INT_ANGLE_360);
CosTable = (long far *)GlobalLock(hCosTable);

hSinTable = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(long) * INT_ANGLE_360);
SinTable = (long far *)GlobalLock(hSinTable);

hInvSinTable = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(long) * INT_ANGLE_360);
InvSinTable = (long far *)GlobalLock(hInvSinTable);

hInvCosTable = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(long) * INT_ANGLE_360);
InvCosTable = (long far *)GlobalLock(hInvCosTable);

hArcCosTable = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(int) * 257);
ArcCosTable = (int far *)GlobalLock(hArcCosTable);

hArcSinTable = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(int) * 257);
ArcSinTable = (int far *)GlobalLock(hArcSinTable);

hxNextTable = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(long) * INT_ANGLE_360);
xNextTable = (long far *)GlobalLock(hxNextTable);

hyNextTable = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(long) * INT_ANGLE_360);
yNextTable = (long far *)GlobalLock(hyNextTable);

hViewCosTable = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(long) * DEF_VIEW_WIDTH);
ViewCosTable = (long far *)GlobalLock(hViewCosTable);

hAdjustTable = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(long) * MAX_DISTANCE + 1);
AdjustTable = (long far *)GlobalLock(hAdjustTable);

hDeflectTable = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(int) * INT_ANGLE_360);
DeflectTable = (int far *)GlobalLock(hDeflectTable);

hBmScrn = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(char) * 85680L);
pBmScrn[0].BS_Pointer = (char far *)GlobalLock(hBmScrn);

for (idx=0 ; idx < INT_ANGLE_360 ; idx++)
{
	a = (double) idx * 0.003272492347;
	b = sin(a) * 65536.0;
	c = (long) ceil(b);
	SinTable[idx] = c;
	b = cos(a) * 65536.0;
	c = (long) ceil(b);
	CosTable[idx] = c;
	b = tan(a) * 65536.0;
	c = (long) ceil(b);
	LongTanTable[idx] = c;
	b = sin(a);
	if (b != 0.0) b = (1.0 / b) * 65536.0;
	c = (long) ceil(b);
	InvSinTable[idx] = c;
	b = cos(a);
	if (b != 0.0) b = (1.0 / b) * 65536.0;
	c = (long) ceil(b);
	InvCosTable[idx] = c;
	b = tan(a);
	if (b != 0.0) b = (1.0 / b) * 65536.0;
	c = (long) ceil(b);
	LongInvTanTable[idx] = c;
}

for (idx=0 ; idx <= 256 ; idx++)
{
	a = (double) idx * 0.00390625;
	ArcCosTable[idx] = (int)ceil(acos(a) * 305.577490736);
	ArcSinTable[idx] = (int)ceil(asin(a) * 305.577490736);
}

for (len = 0; len < INT_ANGLE_360; len++)
{
  yNextTable[len] = (long)GRID_SIZE * LongTanTable[len];
	if (len < INT_ANGLE_270 && len > INT_ANGLE_90)
		yNextTable[len] = -yNextTable[len];

  xNextTable[len] = (long)GRID_SIZE * LongInvTanTable[len];
	if (len > INT_ANGLE_180)
		xNextTable[len] = -xNextTable[len];

	if (len < INT_ANGLE_45)		ca = INT_ANGLE_90;
	else
	if (len < INT_ANGLE_90)		ca = 0;
	else
	if (len < INT_ANGLE_135)	ca = INT_ANGLE_180;
	else
	if (len < INT_ANGLE_180)	ca = INT_ANGLE_90;
	else
	if (len < INT_ANGLE_225)	ca = INT_ANGLE_270;
	else
	if (len < INT_ANGLE_270)	ca = INT_ANGLE_180;
	else
	if (len < INT_ANGLE_315)	ca = 0;
	else
	if (len < INT_ANGLE_360)	ca = INT_ANGLE_270;
	DeflectTable[len] = ca;
}

return(0);
}

/*****************************************************************************
* This routine is called by the application to read in an ACK map file.			*
* The map file consist of 4096 integers for the walls followed by 4096			*
* integers for the objects.																									*
*****************************************************************************/
int AckReadMapFile(ACKENG *ae,char *fName)
{
    int	    len,rdlen;
    FILE	*handle;

hGrid = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(int) * (GRID_MAX * 2) + 1);
Grid = (unsigned int far *)GlobalLock(hGrid);

hObjGrid = GlobalAlloc(GMEM_MOVEABLE|GMEM_SHARE, sizeof(int) * (GRID_MAX * 2) + 1);
ObjGrid = (unsigned int far *)GlobalLock(hObjGrid);

handle = fopen(fName,"r+b");
if (handle == NULL)
    return(ERR_BADMAPFILE);

rdlen = GRID_MAX * 2;

len = fread(Grid,1,rdlen,handle);

if (len == rdlen)
    len = fread(ObjGrid,1,rdlen,handle);

fclose(handle);

if (len != rdlen)
    return(ERR_READINGMAP);

BuildAckGrid(ae);	    /* Build wall and object XY grids	  */

if (hGrid)
{
	GlobalUnlock(hGrid);
	GlobalFree(hGrid);
	hGrid = NULL;
}

if (hObjGrid)
{
	GlobalUnlock(hObjGrid);
	GlobalFree(hObjGrid);
	hObjGrid = NULL;
}

return(0);
}

/****************************************************************************
* Internal call used to create the distance tables. Currently the distance	*
* table is only used for the widths of objects.															*
****************************************************************************/
void AckBuildHeightTables(ACKENG *ae)
{
char				BS[2048];
char huge		*bs;
int	    		i;
int					ca,na,sa,len,idx;
int					top, bot, s;
long				grs=0;
long    		height;

ca = INT_ANGLE_30;
na = -(1 + ViewLowRes);

for (len = 0; len < ViewWidth; len++)
{
  ViewCosTable[len] = CosTable[ca];
  ca += na;
  if (ca <= 0)
	{
		ca = -ca;
		na = -na;
	}
}

bs = pBmScrn[0].BS_Pointer;

//	idx == distance; 16 is minimum distance for any wall/object
for (idx=13 ; idx < MAX_DISTANCE ; idx++)
{
	top = 31;
	bot = 32;
	len = ca = s = 0;
	sa = 1024; na = sa - 1;
	while (-1)
	{
		BS[na] = top;
		BS[sa] = bot;
		len += 2;
		ca += idx;
		s = ca >> (8 - ViewLowRes);
		if (s >= 32) break;
		top = 31 - s;
		bot = 32 + s;
		na--; sa++;
	}

	pBmScrn[idx].BS_Pointer = bs;
	//	offset to pixel in bitmap
	for (bot=na,top=0 ; top < len ; bot++,top++) *bs++ = BS[bot];
	grs += len;
	len >>= 1;
	//	first row to put pixel (can exceed screen boundaries)
	pBmScrn[idx].BS_Start = len;
	//	number of pixels in column (can be greater than screen height)
	pBmScrn[idx].BS_Length = len;

	BS_Floor[idx] = (ViewHeight >> 1) + len;
	BS_Ceiling[idx] = (ViewHeight >> 1) - len - 1;
}
TRACE("Magic lookup tables used %ld bytes of memory\n", grs);

height = 8192 / (INT_ANGLE_60 / ViewWidth);	//	22000 - (ViewHeight * 70);	//	18000

DistanceTable[0] = MAX_HEIGHT;

/************** 64 * 65536 ************/
AdjustTable[0] = 4194304L / height;

for (i = 1; i < MAX_DISTANCE; i++)
{
  DistanceTable[i] = (int)height / i;
//  if (height - (DistanceTable[i] * i) > (i / 2)) DistanceTable[i]++;
//  if (DistanceTable[i] < MIN_HEIGHT) DistanceTable[i] = MIN_HEIGHT;
//  if (DistanceTable[i] > MAX_HEIGHT) DistanceTable[i] = MAX_HEIGHT;
  AdjustTable[i] = 2097152L / DistanceTable[i];
}

}

/****************************************************************************
* Internal call used to process the map file into the wall and object				*
* arrays. The application can use the map file for initial placement of			*
* objects if desired or can setup objects itself once the map file is				*
* processed.								   																							*
*****************************************************************************/
void BuildAckGrid(ACKENG *ae)
{
    int	    i,j,CurIndex,pos;
    UINT    MapCode, TmpCode, TopCode;

for (i = 0; i < MAX_DOORS; i++)
{
    ae->Door[i].ColOffset = 0;
    ae->Door[i].mPos = ae->Door[i].mPos1 = -1;
}

i = (GRID_WIDTH+1) * (GRID_HEIGHT+1) * sizeof(UINT);
memset(ae->xGrid,0,i);
memset(ae->yGrid,0,i);
memset(ae->LightMap,0,GRID_MAX);

CurIndex     = 1;
TotalSpecial = 0;
TotalSecret  = 0;

for (i = 0; i < GRID_HEIGHT; i++)
{
    for (j = 0; j < GRID_WIDTH; j++)
	{
		pos	= (i * GRID_WIDTH) + j;
		MapCode = Grid[pos];
		TmpCode = Grid[pos] & 0xFF;
		TopCode = Grid[pos] & 0xFF00;

		if (TopCode == MAP_STARTCODE)
		{
			ae->ObjList[PLAYER_OBJECT].mPos = pos;
	    ae->ObjList[PLAYER_OBJECT].y = pos & 0xFFC0;
	    ae->ObjList[PLAYER_OBJECT].x = (pos & 0x3F) << 6;
	    ae->ObjList[PLAYER_OBJECT].y += 32;
	    ae->ObjList[PLAYER_OBJECT].x += 32;
	    ae->ObjList[PLAYER_OBJECT].Dir = (MapCode & 0xFF) << 3;
	    ObjList[PLAYER_OBJECT]->CheckDist = 32;
	  }

		if (TopCode == MAP_STARTCODE ||
				TopCode == MAP_UPCODE ||
	    	TopCode == MAP_DOWNCODE ||
	    	TopCode == MAP_GOALCODE)
    {
    	SpecialCodes[TotalSpecial].mPos = pos;
    	SpecialCodes[TotalSpecial++].mCode = TopCode;
    	Grid[pos] = 0;
    	continue;
    }

		if (MapCode & WALL_SECRET_DOOR)
		{
	    	ae->yGrid[pos] = MapCode;
			ae->yGrid[pos+GRID_WIDTH] = MapCode;
	    	ae->xGrid[pos] = MapCode;
			ae->xGrid[pos+1] = MapCode;
			continue;
		}

		if (TmpCode)		    /* Something is in map */
		{
	    	if (!(TmpCode >= DOOR_YCODE))
			{
				if (ae->xGrid[pos] < DOOR_SIDECODE)
			    	ae->xGrid[pos] = MapCode;
				ae->xGrid[pos+1] = MapCode;
			}
	    	else
			{
				ae->xGrid[pos]		= TmpCode - DOOR_YSIDE_DELTA;
				ae->xGrid[pos+1]	= TmpCode - DOOR_YSIDE_DELTA;
			}

	    	if (!(TmpCode >= DOOR_XCODE && TmpCode < DOOR_YCODE))
			{
				if (ae->yGrid[pos] < DOOR_SIDECODE)
		    		ae->yGrid[pos] = MapCode;
				ae->yGrid[pos+GRID_WIDTH] = MapCode;
			}
	    	else
			{
				ae->yGrid[pos]				= TmpCode - DOOR_XSIDE_DELTA;
				ae->yGrid[pos+GRID_WIDTH]	= TmpCode - DOOR_XSIDE_DELTA;
			}
		}
		else
		{
			ae->LightMap[pos] = (TopCode >> 8) & 0x0F;
		}

		MapCode = ObjGrid[pos];
		if (MapCode)
		{
	    	//	CurIndex = MapCode & 0x7F;
	    	if (CurIndex < MAX_OBJECTS)
			{
				ObjList[CurIndex]->bmNum[0]	= MapCode & 0xFF;
				ae->ObjList[CurIndex].x			= (j << 6) + 32;/* Place in */
				ae->ObjList[CurIndex].y			= (i << 6) + 32;/* center */
				ae->ObjList[CurIndex].mPos		= pos;
				ae->ObjList[CurIndex].Dir		= (MapCode & 0xFF00) >> 5;
				ae->ObjList[CurIndex++].Active	= 1;
			}
		}
	}
}
}
