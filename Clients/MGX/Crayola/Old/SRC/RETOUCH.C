//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "math.h"
#include "id.h"
#include "data.h"
#include "routines.h"

#define RAD 57.29577951

extern int HollowBrushScale;    // set in tcrayon.c
extern int CircleScale;    // set in tcrayon.c 

// Static prototypes
static  void vertical_mask(LPTR lpBrushMask, int BrushSize, int BrushVal);
static  void horizontal_mask(LPTR lpBrushMask, int BrushSize, int BrushVal);
static  void slash_mask(LPTR lpBrushMask, int BrushSize, int BrushVal);
static  void backslash_mask(LPTR lpBrushMask, int BrushSize, int BrushVal);
static  void verthair_mask(LPTR lpBrushMask, int BrushSize, int BrushVal);
static  void horzhair_mask(LPTR lpBrushMask, int BrushSize, int BrushVal);
static  void slashhair_mask(LPTR lpBrushMask, int BrushSize, int BrushVal);
static  void backslashhair_mask(LPTR lpBrushMask, int BrushSize, int BrushVal);
static  void squarehair_mask(LPTR lpBrushMask, int BrushSize, int BrushVal);
static  void circle_mask(LPTR lpBrushMask, int BrushSize, int BrushVal);
static  void hexcircle_mask( LPTR lpBrushMask, int BrushSize, int BrushVal, int CircleScale );
static  void hollowcircle_mask( LPTR lpBrushMask, int BrushSize, int BrushVal, int HollowBrushScale );
static  BOOL BuildThresholdTable(LPTR lpBrushThreshold);
static  void DrawBrushOutline(int xcenter,int ycenter,int xSize,int ySize,LPTR lpBrushOutline);
static  void set4pixels(int xp,int yp,LPTR lpBrushOutline,int bEven);
static  void FillBrushOutline(int xcenter,int ycenter,LPTR lpBrushOutline);
static void CreateEdgeTable(PTR fuzz, int table_size, int FuzzyPixels);

#define SPOT(x,y) ((x) + ((y)*(MAX_BRUSH_SIZE+1)))
#define BRMASK(lpM,BrushSize,x,y) lpM[ (BrushSize * (y)) + (x) ]

static LPLFIXED lpSqrt1, lpSqrt2;
static int iTblWidth;
static LPTR lpBrushThreshold;

/************************************************************************/
void SetupBrushMask(LPMGXBRUSH lpBrush)
/************************************************************************/
{
BYTE BrushVal;

if (!lpBrush)
	return;

if ( lpBrush->Shape == IDC_BRUSHCUSTOM )
	return;


BrushVal = 255;

if ( lpBrush->Size == 1  || lpBrush->Shape == IDC_BRUSHSQUARE || 
  (( lpBrush->Shape == IDC_BRUSHCIRCLE || lpBrush->Shape == IDC_BRUSHGLITTERCIRCLE || 
     lpBrush->Shape == IDC_BRUSHSWIRLCIRCLE || lpBrush->Shape == IDC_BRUSHHOLLOWCIRCLE ) && 
     lpBrush->Size < 3 ) || ( lpBrush->Shape == IDC_BRUSHHEXCIRCLE && 
     lpBrush->Size < 8 ))
	set( lpBrush->BrushMask, sizeof(lpBrush->BrushMask), BrushVal );
else
	clr( lpBrush->BrushMask, sizeof(lpBrush->BrushMask) );

if ( lpBrush->Shape == IDC_BRUSHCIRCLE || 
     lpBrush->Shape == IDC_BRUSHGLITTERCIRCLE ||
     lpBrush->Shape == IDC_BRUSHSWIRLCIRCLE )
	circle_mask( lpBrush->BrushMask, lpBrush->Size, BrushVal );
else
if ( lpBrush->Shape == IDC_BRUSHHEXCIRCLE )
	hexcircle_mask( lpBrush->BrushMask, lpBrush->Size, BrushVal, CircleScale );
else
if ( lpBrush->Shape == IDC_BRUSHHOLLOWCIRCLE )
	hollowcircle_mask( lpBrush->BrushMask, lpBrush->Size, BrushVal, HollowBrushScale );
else
if ( lpBrush->Shape == IDC_BRUSHVERTICAL )
	vertical_mask( lpBrush->BrushMask, lpBrush->Size, BrushVal );
else
if ( lpBrush->Shape == IDC_BRUSHHORIZONTAL )
	horizontal_mask( lpBrush->BrushMask, lpBrush->Size, BrushVal );
else
if ( lpBrush->Shape == IDC_BRUSHSLASH )
	slash_mask( lpBrush->BrushMask, lpBrush->Size, BrushVal );
else
if ( lpBrush->Shape == IDC_BRUSHBACKSLASH )
	backslash_mask( lpBrush->BrushMask, lpBrush->Size, BrushVal );
else
if ( lpBrush->Shape == IDC_BRUSHVERTHAIR )
	verthair_mask( lpBrush->BrushMask, lpBrush->Size, BrushVal );
else
if ( lpBrush->Shape == IDC_BRUSHHORZHAIR )
	horzhair_mask( lpBrush->BrushMask, lpBrush->Size, BrushVal );
else
if ( lpBrush->Shape == IDC_BRUSHSLASHHAIR )
	slashhair_mask( lpBrush->BrushMask, lpBrush->Size, BrushVal );
else
if ( lpBrush->Shape == IDC_BRUSHBACKSLASHHAIR )
	backslashhair_mask( lpBrush->BrushMask, lpBrush->Size, BrushVal );
else
if ( lpBrush->Shape == IDC_BRUSHSQUAREHAIR )
	squarehair_mask( lpBrush->BrushMask, lpBrush->Size, BrushVal );

SetupFuzzyBrush(lpBrush);
}

/************************************************************************/
BOOL SetupCustomBrushMask(LPMGXBRUSH lpBrush, LPSTR lpExtName)
{
int x, y, sx, sy, dx, dy, bx, by;
int bpp;
LPTR lpLine, lpBM;
LPFRAME lpFrame;
FNAME szFileName;
static int CustomBrushSize;

if ( lpBrush->Shape != IDC_BRUSHCUSTOM )
	return( FALSE );

if ( !LookupExtFile( lpExtName, szFileName, IDN_CUSTOM ) )
	{
	return(FALSE);
//	lpFrame = NULL;
	}
else if (!FileExists(szFileName))
	{
	return(FALSE);
	}
else if ( !(lpFrame = AstralFrameLoad(szFileName, 1, &bpp, NULL )))
	{
	return(FALSE);
	}

if ( !lpFrame )
	{
	dx = MAX_BRUSH_SIZE;
	dy = MAX_BRUSH_SIZE;
	}
else	{
	dx = FrameXSize(lpFrame);
	dy = FrameYSize(lpFrame);
	if ( dx > MAX_BRUSH_SIZE || dy > MAX_BRUSH_SIZE )
		Message( IDS_ECUSTOMBRUSH, MAX_BRUSH_SIZE );
	sx = sy = 0;
	if ( dx > MAX_BRUSH_SIZE )
		{ // take the center of the image if it is too big
		sx = ( dx - MAX_BRUSH_SIZE ) / 2;
		dx = MAX_BRUSH_SIZE;
		}
	if ( dy > MAX_BRUSH_SIZE )
		{ // take the center of the image if it is too big
		sy = ( dy - MAX_BRUSH_SIZE ) / 2;
		dy = MAX_BRUSH_SIZE;
		}
	}

CustomBrushSize = max( dx, dy );
lpBrush->Size = CustomBrushSize;
set( lpBrush->BrushMask, sizeof(lpBrush->BrushMask), 0);

if ( lpFrame )
	{
	bx = ( CustomBrushSize - dx ) / 2;
	by = ( CustomBrushSize - dy ) / 2;
	lpBM = lpBrush->BrushMask;
	for (y = 0; y < dy; ++y)
		{
		if ( !(lpLine = FramePointer( lpFrame, sx, y+sy, NO)) )
			continue;
		for (x = 0; x < dx; ++x, lpLine++)
			BRMASK(lpBM, CustomBrushSize, bx+x, by+y) = *lpLine ^ 255;
		}
	FrameClose(lpFrame);
	}
return(TRUE);
}


/************************************************************************/
static void vertical_mask( LPTR lpBrushMask, int BrushSize, int BrushVal )
/************************************************************************/
{
int x, iCount;

x = BrushSize / 2;
lpBrushMask += x;
iCount = BrushSize;
while (--iCount >= 0)
	{
	*lpBrushMask = BrushVal;
	lpBrushMask += BrushSize;
	}
}

/************************************************************************/
static void horizontal_mask( LPTR lpBrushMask, int BrushSize, int BrushVal )
/************************************************************************/
{
int y, iCount;

y = BrushSize / 2;
lpBrushMask += (y * BrushSize);
iCount = BrushSize;
while (--iCount >= 0)
	*lpBrushMask++ = BrushVal;
}

/************************************************************************/
static void slash_mask( LPTR lpBrushMask, int BrushSize, int BrushVal )
/************************************************************************/
{
int x, y;

x = BrushSize - 1;
for ( y = 1; y < BrushSize; y++ )
	{
	BRMASK( lpBrushMask, BrushSize, x, y ) = BrushVal;
	BRMASK( lpBrushMask, BrushSize, --x, y ) = BrushVal;
	}
}

/************************************************************************/
static void backslash_mask( LPTR lpBrushMask, int BrushSize, int BrushVal )
/************************************************************************/
{
int x, y;

x = 0;
for ( y = 1; y < BrushSize; y++ )
	{
	BRMASK( lpBrushMask, BrushSize, x, y ) = BrushVal;
	BRMASK( lpBrushMask, BrushSize, ++x, y ) = BrushVal;
	}
}

/************************************************************************/
static void verthair_mask( LPTR lpBrushMask, int BrushSize, int BrushVal )
/************************************************************************/
{
int x, iCount, iPiece, iPiecePaint, iPieceGap;
BOOL bOn;

x = BrushSize / 2;
lpBrushMask += x;

// 3/5th of the brushsize is painted
if ( !(iPiecePaint = (BrushSize+2)/5) )
	iPiecePaint = 1;

// 2/5th of the brushsize is a gap
if ( !(iPieceGap = (BrushSize - (iPiecePaint * 3)) / 2) )
	iPieceGap = 1;

iPiece = iPiecePaint;
bOn = YES;
iCount = BrushSize;
while (--iCount >= 0)
	{
	if ( iPiece <= 0 )
		{
		if ( bOn )
				iPiece = iPieceGap; // switching from paint to gap
		else	iPiece = iPiecePaint; // switching from gap to paint
		bOn = !bOn;
		}
	iPiece--;
	if ( bOn )
		*lpBrushMask = BrushVal;
	lpBrushMask += BrushSize;
	}
}


/************************************************************************/
static void horzhair_mask( LPTR lpBrushMask, int BrushSize, int BrushVal )
/************************************************************************/
{
int y, iCount, iPiece, iPiecePaint, iPieceGap;
BOOL bOn;

y = BrushSize / 2;
lpBrushMask += (y * BrushSize);

// 3/5th of the brushsize is painted
if ( !(iPiecePaint = (BrushSize+2)/5) )
	iPiecePaint = 1;

// 2/5th of the brushsize is a gap
if ( !(iPieceGap = (BrushSize - (iPiecePaint * 3)) / 2) )
	iPieceGap = 1;

iPiece = iPiecePaint;
bOn = YES;
iCount = BrushSize;
while (--iCount >= 0)
	{
	if ( iPiece <= 0 )
		{
		if ( bOn )
				iPiece = iPieceGap; // switching from paint to gap
		else	iPiece = iPiecePaint; // switching from gap to paint
		bOn = !bOn;
		}
	iPiece--;
	if ( bOn )
		*lpBrushMask = BrushVal;
	lpBrushMask++;
	}
}

/************************************************************************/
static void slashhair_mask( LPTR lpBrushMask, int BrushSize, int BrushVal )
/************************************************************************/
{
    int x, y;
    int iCount, iPiece, iPiecePaint, iPieceGap;
    BOOL bOn;

    // 3/5th of the brushsize is painted
    if( !( iPiecePaint = (( BrushSize - 1 ) + 2 ) / 5 ))
	    iPiecePaint = 1;

    // 2/5th of the brushsize is a gap
    if( !( iPieceGap = (( BrushSize - 1 ) - ( iPiecePaint * 3 )) / 2 ))
	    iPieceGap = 1;

    iPiece = iPiecePaint;
    bOn = YES;
    iCount = BrushSize;

    x = BrushSize - 1;
    y = 1;

    while( --iCount >= 0 )
    {
	    if( iPiece <= 0 )
	    {
		    if( bOn )
                iPiece = iPieceGap;     // switching from paint to gap
		    else 
                iPiece = iPiecePaint;   // switching from gap to paint

		    bOn = !bOn;
	    }

	    iPiece--;

	    if( bOn )
        {
            if( x > 0 )
            {
	            BRMASK( lpBrushMask, BrushSize, x, y )   = BrushVal;
   	            BRMASK( lpBrushMask, BrushSize, x-1, y ) = BrushVal;
            }
	    }
        x--; y++;
    }
}


/************************************************************************/
static void backslashhair_mask( LPTR lpBrushMask, int BrushSize, int BrushVal )
/************************************************************************/
{
    int x, y;
    int iCount, iPiece, iPiecePaint, iPieceGap;
    BOOL bOn;

    // 3/5th of the brushsize is painted
    if( !( iPiecePaint = (( BrushSize - 1 ) + 2 ) / 5 ))
	    iPiecePaint = 1;

    // 2/5th of the brushsize is a gap
    if( !( iPieceGap = (( BrushSize - 1 ) - ( iPiecePaint * 3 )) / 2 ))
	    iPieceGap = 1;

    iPiece = iPiecePaint;
    bOn = YES;
    iCount = BrushSize;

    x = 0;
    y = 1;

    while( --iCount >= 0 )
    {
	    if( iPiece <= 0 )
	    {
		    if( bOn )
                iPiece = iPieceGap;     // switching from paint to gap
		    else 
                iPiece = iPiecePaint;   // switching from gap to paint

		    bOn = !bOn;
	    }

	    iPiece--;

	    if( bOn )
        {
    	    BRMASK( lpBrushMask, BrushSize, x, y )   = BrushVal;
    	    BRMASK( lpBrushMask, BrushSize, x+1, y )   = BrushVal;
	    }
        x++; y++;
    }
}


/************************************************************************/
static void squarehair_mask( LPTR lpBrushMask, int BrushSize, int BrushVal )
/************************************************************************/
{
    int x = 0, y = 0;
    int iCount, iPiece, iPiecePaint, iPieceGap, iRange, iColumnStart, iOffset;
    BOOL bOn;

    // 3/5th of the brushsize is painted
    if( !( iPiecePaint = ( BrushSize + 2 ) / 5 ))
	    iPiecePaint = 1;

    // 2/5th of the brushsize is a gap
    if( !( iPieceGap = ( BrushSize - ( iPiecePaint * 3 )) / 2 ))
	    iPieceGap = 1;

    iPiece = iPiecePaint;
    bOn = YES;
    iColumnStart = x; 

    if( BrushSize > 5 )
        iRange = ( 3 * iPiecePaint ) + ( 2 * iPieceGap );
    else 
        iRange = BrushSize;

    iCount = iRange;

    while( --iCount >= 0 )
    {
	    if( iPiece <= 0 )
	    {
            iColumnStart = x;                 

		    if( bOn )
                iPiece = iPieceGap;     // switching from paint to gap
		    else 
                iPiece = iPiecePaint;   // switching from gap to paint

		    bOn = !bOn;
	    }

	    iPiece--;

	    if( bOn )
        {
            iOffset = 0;

            while( iOffset < iPiecePaint )
            {
	            BRMASK( lpBrushMask, BrushSize, iColumnStart + iOffset, y ) = BrushVal;
	            BRMASK( lpBrushMask, BrushSize, iColumnStart + iOffset++, iRange - 1 - y ) = BrushVal;
            }
        }
        x++; y++;
    }
}


/************************************************************************/
static void circle_mask( LPTR lpBrushMask, int BrushSize, int BrushVal )
/************************************************************************/
{
int xcenter, ycenter, xpos, ypos, x, y, thresh, f, HalfBrush;

// if no lpBrushThreshold, we're fucked!!!
if (!lpBrushThreshold)
	return;

xcenter = (MAX_BRUSH_SIZE+1)/2;
ycenter = (MAX_BRUSH_SIZE+1)/2;
HalfBrush = BrushSize/2;
ypos = ycenter - HalfBrush;
f = BrushSize - 1;

// Only read the upper left quad of the lpBrushThreshold table to
// maintain symetry for odd and even sized circular brushes
for (y = 0; y <= HalfBrush; ++y, ++ypos)
	{
	xpos = xcenter - HalfBrush;
	for (x = 0; x <= HalfBrush; ++x, ++xpos)
		{
		if ( !(thresh = lpBrushThreshold[ SPOT(xpos,ypos) ]) )
			continue;
		if ( thresh > BrushSize )
			continue;
		BRMASK( lpBrushMask, BrushSize, x,   y   ) = BrushVal;
		BRMASK( lpBrushMask, BrushSize, f-x, y   ) = BrushVal;
		BRMASK( lpBrushMask, BrushSize, f-x, f-y ) = BrushVal;
		BRMASK( lpBrushMask, BrushSize, x,   f-y ) = BrushVal;
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
static void hexcircle_mask( LPTR lpBrushMask, int BrushSize, int BrushVal, int Scale )
//////////////////////////////////////////////////////////////////////////////
{
    int xcenter, ycenter, xpos, ypos, x, y, thresh, f, HalfBrush;
    int CircleSize;
    int brush_ctr;
    int offset_x1, offset_y1, offset_x2, offset_y2;
    int offset_x3, offset_y3, offset_x4, offset_y4;
    int offset_x5, offset_y5, offset_x6, offset_y6;
    int radius;

    if( !lpBrushThreshold )     // not good!!!
	    return;

    if( Scale > 100 )
        Scale = 100;
    else
    if( Scale < 1 )
        Scale = 1;

    xcenter = ( MAX_BRUSH_SIZE + 1 ) / 2;
    ycenter = ( MAX_BRUSH_SIZE + 1 ) / 2;
    brush_ctr = BrushSize / 2;

    CircleSize = Scale * BrushSize / 100;

    // circle size must be even to maintain precision
    if( CircleSize % 2 )
        CircleSize--;

    HalfBrush = CircleSize / 2;

    radius = brush_ctr - HalfBrush;

    f = CircleSize - 1;

    // Only read the upper left quad of the lpBrushThreshold table to
    // maintain symetry for odd and even sized circular brushes
   
    ypos = ycenter - HalfBrush;

    // create one little circle
    for( y = 0; y <= HalfBrush; ++y, ++ypos )
	{
	    xpos = xcenter - HalfBrush;

	    for( x = 0; x <= HalfBrush; ++x, ++xpos )
		{
		    if( !( thresh = lpBrushThreshold[ SPOT( xpos, ypos )] ))
			    continue;

		    if( thresh > CircleSize )
			    continue;

            // map little circle to six locations within brush
            offset_x1 = 0;
            offset_y1 = brush_ctr - HalfBrush;

            offset_x2 = BrushSize - CircleSize;
            offset_y2 = offset_y1;

            offset_x3 = ( int )( brush_ctr - (( double )radius * sin( 30 / RAD )) - HalfBrush );
            offset_y3 = ( int )( brush_ctr - (( double )radius * cos( 30 / RAD )) - HalfBrush );

            offset_x4 = BrushSize - offset_x3 - CircleSize;
            offset_y4 = offset_y3;

            offset_x5 = offset_x3;
            offset_y5 = BrushSize - offset_y3 - CircleSize;

            offset_x6 = offset_x4;
            offset_y6 = offset_y5;

		    BRMASK( lpBrushMask, BrushSize, offset_x1 + x,     offset_y1 + y     ) = BrushVal;
		    BRMASK( lpBrushMask, BrushSize, offset_x1 + (f-x), offset_y1 + y     ) = BrushVal;
		    BRMASK( lpBrushMask, BrushSize, offset_x1 + (f-x), offset_y1 + (f-y) ) = BrushVal;
		    BRMASK( lpBrushMask, BrushSize, offset_x1 + x,     offset_y1 + (f-y) ) = BrushVal;

            // mask circle opposite hand
		    BRMASK( lpBrushMask, BrushSize, offset_x2 + x,     offset_y2 + y     ) = BrushVal;
		    BRMASK( lpBrushMask, BrushSize, offset_x2 + (f-x), offset_y2 + y     ) = BrushVal;
		    BRMASK( lpBrushMask, BrushSize, offset_x2 + (f-x), offset_y2 + (f-y) ) = BrushVal;
		    BRMASK( lpBrushMask, BrushSize, offset_x2 + x,     offset_y2 + (f-y) ) = BrushVal;

		    BRMASK( lpBrushMask, BrushSize, offset_x3 + x,     offset_y3 + y     ) = BrushVal;
		    BRMASK( lpBrushMask, BrushSize, offset_x3 + (f-x), offset_y3 + y     ) = BrushVal;
		    BRMASK( lpBrushMask, BrushSize, offset_x3 + (f-x), offset_y3 + (f-y) ) = BrushVal;
		    BRMASK( lpBrushMask, BrushSize, offset_x3 + x,     offset_y3 + (f-y) ) = BrushVal;

		    BRMASK( lpBrushMask, BrushSize, offset_x4 + x,     offset_y4 + y     ) = BrushVal;
		    BRMASK( lpBrushMask, BrushSize, offset_x4 + (f-x), offset_y4 + y     ) = BrushVal;
		    BRMASK( lpBrushMask, BrushSize, offset_x4 + (f-x), offset_y4 + (f-y) ) = BrushVal;
		    BRMASK( lpBrushMask, BrushSize, offset_x4 + x,     offset_y4 + (f-y) ) = BrushVal;

		    BRMASK( lpBrushMask, BrushSize, offset_x5 + x,     offset_y5 + y     ) = BrushVal;
		    BRMASK( lpBrushMask, BrushSize, offset_x5 + (f-x), offset_y5 + y     ) = BrushVal;
		    BRMASK( lpBrushMask, BrushSize, offset_x5 + (f-x), offset_y5 + (f-y) ) = BrushVal;
		    BRMASK( lpBrushMask, BrushSize, offset_x5 + x,     offset_y5 + (f-y) ) = BrushVal;

		    BRMASK( lpBrushMask, BrushSize, offset_x6 + x,     offset_y6 + y     ) = BrushVal;
		    BRMASK( lpBrushMask, BrushSize, offset_x6 + (f-x), offset_y6 + y     ) = BrushVal;
		    BRMASK( lpBrushMask, BrushSize, offset_x6 + (f-x), offset_y6 + (f-y) ) = BrushVal;
		    BRMASK( lpBrushMask, BrushSize, offset_x6 + x,     offset_y6 + (f-y) ) = BrushVal;
	    }
    }
}


//////////////////////////////////////////////////////////////////////////////
static void hollowcircle_mask( LPTR lpBrushMask, int BrushSize, int BrushVal, int Scale )
//////////////////////////////////////////////////////////////////////////////
{
    int xcenter, ycenter, xpos, ypos, x, y, thresh, f, HalfBrush;
    int offset_x, offset_y;     // offsets for inner circle
    int num_circles;            // loop counter
    int xstart, ystart;
    int scale;

    if( !lpBrushThreshold )     // not good!!!
	    return;

    if( Scale > 100 )
        Scale = 100;
    else
    if( Scale < 1 )
        Scale = 1;

    xcenter = ( MAX_BRUSH_SIZE + 1 ) / 2;
    ycenter = ( MAX_BRUSH_SIZE + 1 ) / 2;

    HalfBrush = BrushSize / 2;

    xstart = xcenter - HalfBrush;
    ystart = ycenter - HalfBrush;

    f = BrushSize - 1;

    // Only read the upper left quad of the lpBrushThreshold table to
    // maintain symetry for odd and even sized circular brushes
   
    for( num_circles = 1; num_circles <= 2; num_circles++ )
    {
        if( num_circles == 1 )          // draw larger circle first
        {
            offset_x = offset_y = 0;    
            scale = 100;
        }
        else                            // draw inner circle next
        {
            HalfBrush = ( Scale * BrushSize / 100 ) / 2;

	        offset_x = ( xcenter - HalfBrush ) - xstart;
	        offset_y = ( ycenter - HalfBrush ) - ystart;
            scale = Scale;

            BrushVal = abs( BrushVal - 255 );   // inner brush is hollow
        }

        ypos = ycenter - HalfBrush;

        for( y = offset_y; y <= HalfBrush + offset_y; ++y, ++ypos )
	    {
	        xpos = xcenter - HalfBrush;

	        for( x = offset_x; x <= HalfBrush + offset_x; ++x, ++xpos )
		    {
		        if( !( thresh = lpBrushThreshold[ SPOT( xpos, ypos )] ))
			        continue;

		        if( thresh > ( scale * BrushSize / 100 ))
			        continue;

		        BRMASK( lpBrushMask, BrushSize, x,   y   ) = BrushVal;
		        BRMASK( lpBrushMask, BrushSize, f-x, y   ) = BrushVal;
		        BRMASK( lpBrushMask, BrushSize, f-x, f-y ) = BrushVal;
		        BRMASK( lpBrushMask, BrushSize, x,   f-y ) = BrushVal;
	        }
        }
    }
}


/************************************************************************/
static BOOL BuildThresholdTable(LPTR lpBrushThreshold)
/************************************************************************/
{
LPTR lpBrushOutline;
int BrushSize, xcenter, ycenter;
int iTotalSize, i;

iTotalSize = (MAX_BRUSH_SIZE+1)*(MAX_BRUSH_SIZE+1);
if (!(lpBrushOutline = Alloc((long)iTotalSize)))
	return(FALSE);
xcenter = (MAX_BRUSH_SIZE+1)/2;
ycenter = (MAX_BRUSH_SIZE+1)/2;
clr(lpBrushThreshold, iTotalSize);
for (BrushSize = 1; BrushSize <= (MAX_BRUSH_SIZE+1); ++BrushSize)
    {
    clr(lpBrushOutline, iTotalSize);
    DrawBrushOutline(xcenter, ycenter, BrushSize, BrushSize, lpBrushOutline);
    if (BrushSize > 1)
        FillBrushOutline(xcenter, ycenter, lpBrushOutline);
    for (i = 0; i < iTotalSize; ++i)
		{
		if (lpBrushOutline[i])
	    	if (!lpBrushThreshold[i])
				lpBrushThreshold[i] = BrushSize;
		}
    }
FreeUp(lpBrushOutline);
return(TRUE);
}


static int xc, yc;
/************************************************************************/
static void DrawBrushOutline(
/************************************************************************/
int 	xcenter, 
int		ycenter, 
int		xSize,
int		ySize,
LPTR 	lpBrushOutline)
{
int x, y;
long a, b, asqrd, twoasqrd, bsqrd, twobsqrd, d, dx, dy;
BOOL bEven;

xc = xcenter;
yc = ycenter;
x = 0;
y = ySize/2;
a = xSize/2;
b = ySize/2;
asqrd = a * a;
twoasqrd = 2 * asqrd;
bsqrd = b * b;
twobsqrd = 2 * bsqrd;
d = bsqrd - asqrd*b + asqrd/4L;
dx = 0;
dy = twoasqrd * b;
bEven = !(xSize & 1);

while(dx<dy)
	{
	set4pixels( x, y, lpBrushOutline, bEven );
	if(d > 0L)
		{
		--y;
		dy -= twoasqrd;
		d -= dy;
		}
	++x;
	dx += twobsqrd;
	d += bsqrd + dx;
	}

d += (3L*(asqrd-bsqrd)/2L - (dx+dy)) / 2L;

while(y>=0)
	{
	set4pixels( x, y, lpBrushOutline, bEven );
	if(d < 0L)
		{
		++x;
		dx += twobsqrd;
		d += dx;
		}
	--y;
	dy -= twoasqrd;
	d += asqrd - dy;
	}
}


/************************************************************************/
static void set4pixels(
/************************************************************************/
int 	xp,
int		yp,
LPTR 	lpBrushOutline,
BOOL 	bEven)
{
int x1, x2, y1, y2;

if ( xp < 0 ) xp = -xp;
if ( yp < 0 ) yp = -yp;
x1 = xc - xp; x2 = xc + xp - bEven;
y1 = yc - yp; y2 = yc + yp - bEven;

lpBrushOutline[ SPOT(x2,y2) ] = ON;
if ( x1 != x2 ) lpBrushOutline[ SPOT(x1,y2) ] = ON;
if ( y1 != y2 ) lpBrushOutline[ SPOT(x2,y1) ] = ON;
if ( x1 != x2 && y1 != y2 ) lpBrushOutline[ SPOT(x1,y1) ] = ON;
}


/************************************************************************/
static void FillBrushOutline(
/************************************************************************/
int 	xcenter,
int 	ycenter,
LPTR 	lpBrushOutline)
{
int x, y;

for (y = ycenter; y >= 0; --y)
    {
    if (lpBrushOutline[ SPOT(xcenter,y) ])
break;
    for (x = xcenter; x >= 0; --x)
	{
        if (lpBrushOutline[ SPOT(x,y) ])
	    break;
	lpBrushOutline[ SPOT(x,y) ] = ON;
        }
    for (x = xcenter+1; x < (MAX_BRUSH_SIZE+1); ++x)
	{
        if (lpBrushOutline[ SPOT(x,y) ])
	    break;
	lpBrushOutline[ SPOT(x,y) ] = ON;
        }
    }
for (y = ycenter+1; y < (MAX_BRUSH_SIZE+1); ++y)
    {
    if (lpBrushOutline[ SPOT(xcenter,y) ])
	break;
    for (x = xcenter; x >= 0; --x)
	{
        if (lpBrushOutline[ SPOT(x,y) ])
	    {
	    break;
	    }
	lpBrushOutline[ SPOT(x,y) ] = ON;
        }
    for (x = xcenter+1; x < (MAX_BRUSH_SIZE+1); ++x)
	{
        if (lpBrushOutline[ SPOT(x,y) ])
	    break;
	lpBrushOutline[ SPOT(x,y) ] = ON;
        }
    }
}

/************************************************************************/
static void CreateEdgeTable(PTR fuzz, int table_size, int FuzzyPixels)
/************************************************************************/
{
int	i, nEdges, iEntry, iLastEntry, value1, value2, delta;
LFIXED rate, offset, fraction;
static BYTE Edge[] =	{	254,253,251,250,248,245,243,240,237,233,
								229,225,220,214,209,202,196,189,181,174,
								166,157,149,140,132,123,114,106,97,89,
								81,74,66,59,53,46,41,35,30,26,
								22,18,15,12,10,7,5,4,2,1};

nEdges = sizeof(Edge);
iLastEntry = nEdges - 1;
rate = FGET(nEdges, FuzzyPixels);
offset = (long)rate >> 1;

for (i = 0; i < FuzzyPixels; ++i)
	{
#ifdef WIN32
	iEntry = WHOLE(offset);
#else
	iEntry = HIWORD(offset);
#endif
	value1 = 255 - Edge[iEntry];

	// if not last entry interpolate
	if (iEntry < iLastEntry)
		{
		value2 = 255 - Edge[iEntry+1];
		delta = value2 - value1;
		fraction = (long)offset & 0x0000FFFF;
		delta = FMUL(delta, fraction);
		value1 += delta;
		}
	fuzz[i] = value1;	
	offset += rate;
	}
for (; i < table_size; ++i)
	fuzz[i] = 255;
}

/************************************************************************/
BOOL SetupBrushMaskBuffers(BOOL fInit)
/************************************************************************/
{
double dx, dy, v;
long lSize;
int x, y;
LPLFIXED lpTbl;
extern double sqrt(double);

if (fInit)
	{
	lpBrushThreshold = Alloc((long)(MAX_BRUSH_SIZE+1) * (MAX_BRUSH_SIZE+1));
	if (!lpBrushThreshold)
		return(FALSE);
	if (!BuildThresholdTable( lpBrushThreshold ))
		{
		FreeUp(lpBrushThreshold);
		lpBrushThreshold = NULL;
		return(FALSE);
		}

	iTblWidth = (MAX_BRUSH_SIZE+1)/2;
	lSize = (iTblWidth * iTblWidth * sizeof(LFIXED) * 2);
	if (!(lpSqrt1 = (LPLFIXED)Alloc(lSize)))
		{
		FreeUp(lpBrushThreshold);
		return(FALSE);
		}
	lpSqrt2 = lpSqrt1 + (iTblWidth * iTblWidth);
	lpTbl = lpSqrt1;
	dy = 0;
	for (y = 0; y < iTblWidth; ++y)
		{
		dx = 0;
		for (x = 0; x < iTblWidth; ++x)
			{
			v = sqrt((dx*dx)+(dy*dy));
			*lpTbl++ = (LFIXED)(v * 65536.);
			dx += 1;
			}
		dy += 1;
		}
	lpTbl = lpSqrt2;
	dy = 0.5;
	for (y = 0; y < iTblWidth; ++y)
		{
		dx = 0.5;
		for (x = 0; x < iTblWidth; ++x)
			{
			v = sqrt((dx*dx)+(dy*dy));
			*lpTbl++ = (LFIXED)(v * 65536.);
			dx += 1;
			}
		dy += 1;
		}
	return(TRUE);
	}
else
	{
	if (lpSqrt1)
		{
		FreeUp((LPTR)lpSqrt1);
		lpSqrt1 = NULL;
		}
	if (lpBrushThreshold)
		{
		FreeUp(lpBrushThreshold);
		lpBrushThreshold = NULL;
		}
	return(TRUE);
	}
}

/************************************************************************/
void SetupFuzzyBrush(LPMGXBRUSH lpBrush)
/************************************************************************/
{
int x, y;
unsigned int ed, edlast;
LPTR lpBrushMask;
static BYTE fuzz[MAX_BRUSH_SIZE];
static int fuzzOpacity = -1, fuzzFuzzyPixels = -1;
int FuzzyPixels;
int BrushSize;
int BrushShape;
BYTE Opacity;
int	xi, yi;
LFIXED root, xd, yd, ydiff, xdiff;
LFIXED fed, fcenter, fraction; 
int value1, value2, delta;
LPLFIXED lpTbl, lpOrigTbl;

lpBrushMask = lpBrush->BrushMask;
BrushShape = lpBrush->Shape;
BrushSize = lpBrush->Size;
Opacity = 255;

FuzzyPixels = ( (long)lpBrush->Feather * (BrushSize/2) ) / 100;
if ((FuzzyPixels <= 0 && Opacity == 255) ||
    (lpImage && FrameDepth(frame_set(NULL)) == 0))
	return;

if (!lpSqrt1 || FuzzyPixels == 0 || (BrushShape != IDC_BRUSHCIRCLE &&
	BrushShape != IDC_BRUSHSQUARE))
	{
	for (y=0; y<BrushSize; y++)
		{
		for (x=0; x<BrushSize; x++, ++lpBrushMask)
			{
			if (*lpBrushMask)
				*lpBrushMask = Opacity;
			}
		}
	return;
	}

if (Opacity != fuzzOpacity || FuzzyPixels != fuzzFuzzyPixels)
	{
	CreateEdgeTable(fuzz, MAX_BRUSH_SIZE, FuzzyPixels);
	fuzzOpacity = Opacity;
	fuzzFuzzyPixels = FuzzyPixels;
	}

fcenter = FGET(BrushSize-1, 2);
edlast = sizeof(fuzz) - 1;
if ((long)fcenter & 0x0000FFFF)
	lpOrigTbl = lpSqrt2;
else
	lpOrigTbl = lpSqrt1;
if (BrushShape == IDC_BRUSHCIRCLE)
	{
	for (y=0; y<BrushSize; y++)
		{
		yd = abs(fcenter - TOFIXED(y));
#ifdef WIN32				
		yi = WHOLE(yd);
#else
		yi = HIWORD(yd);
#endif
		lpTbl = lpOrigTbl + (yi * iTblWidth);
		for (x=0; x<BrushSize; x++, ++lpBrushMask)
			{
			if (*lpBrushMask)
				{
				xd = abs(fcenter - TOFIXED(x));
#ifdef WIN32				
				xi = WHOLE(xd);
#else
				xi = HIWORD(xd);
#endif
				root = *(lpTbl + xi);
				fed = fcenter - root;
				if (fed < 0)
					fed = 0;
#ifdef WIN32
				ed = WHOLE(fed);
#else
				ed = HIWORD(fed);
#endif
				value1 = fuzz[ed];
				if (ed < edlast)
					{
					value2 = fuzz[ed+1];
					delta = value2 - value1;
					fraction = (long)fed & 0x0000FFFFL;
					delta = FMUL(delta, fraction);
					value1 += delta;
					}
				*lpBrushMask = value1;
				}
			}
		}
	}
else if (BrushShape == IDC_BRUSHSQUARE)
	{
	for (y=0; y<BrushSize; y++)
		{
		yd = abs(fcenter - TOFIXED(y));
		ydiff = abs(fcenter - yd);
		for (x=0; x<BrushSize; x++, ++lpBrushMask)
			{
			if (*lpBrushMask)
				{
				xd = abs(fcenter - TOFIXED(x));
				xdiff = abs(fcenter - xd);
				if (xdiff < ydiff)
					fed = xdiff;
				else
					fed = ydiff;
				if (fed < 0)
					fed = 0;
#ifdef WIN32
				ed = WHOLE(fed);
#else
				ed = HIWORD(fed);
#endif
				value1 = fuzz[ed];
				if (ed < edlast)
					{
					value2 = fuzz[ed+1];
					delta = value2 - value1;
					fraction = (long)fed & 0x0000FFFFL;
					delta = FMUL(delta, fraction);
					value1 += delta;
					}
				*lpBrushMask = value1;
				}
			}
		}
	}
}

