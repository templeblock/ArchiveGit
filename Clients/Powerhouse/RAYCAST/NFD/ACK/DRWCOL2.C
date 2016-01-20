/****************************************************************************
*   This is an UNTESTED port of the DrawCol2 routine from ACK3D file ACKASM *
* to allow conversion to other compilers and platforms. It is released into *
* the public domain as an accompanying file to the ACK3D series of source   *
* files and can be freely used under the same restrictions as the original  *
* ACK source.								     																						*
*									     																											*
* Copyright (c) 1994							     																			*
* Author: Lary Myers							     																			*
*****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include <mem.h>
#include <alloc.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys\stat.h>
#include "ack3d.h"
#include "ackeng.h"
#include "ackext.h"


/*****************************************************************************
*									     																											*
*****************************************************************************/
void draw_col2(int Col,int slice,int dist,int width,int ht,UCHAR far *Wall,
	    UCHAR far *Screen,UCHAR far *Pal,int light,int offset)
{
    UINT    height,UpperHeight,bmOffset,bmByte;
    int	    PalDist,VidOffset;
    int	    DCangle = 0;
    UCHAR   far *Screen2;

PalDist = dist >> 6;	/* Divide by 64 to get palette range */
if (PalDist > 15)
    PalDist = 15;


if (light)		    /* If shading is ON */
    Pal += (PalDist << 8);  /* Point to correct palette table */

Screen += (offset + Col);   /* Point to center row + current column */
Screen2 = Screen + width;   /* Point one row down from screen */

slice = slice << 6;	    /* Point to correct bitmap row */
height = 0;

while (DCangle++ < ht)
    {
    UpperHeight = height >> 8;	/* Get high byte of current height */
    if (UpperHeight >= 32)	/* We've done both halves of bitmap */
	break;

    bmOffset = slice + (31 - UpperHeight);
    if ((bmByte = Wall[bmOffset]) != 0)
	{
	*Screen = Pal[bmByte];
	}

    bmOffset = slice + 32 + UpperHeight;
    if ((bmByte = Wall[bmOffset]) != 0)
	{
	*Screen2 = Pal[bmByte];
	}

    Screen -= width;
    Screen2 += width;
    height += dist;
    }


}

