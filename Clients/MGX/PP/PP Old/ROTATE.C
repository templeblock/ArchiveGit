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

#define WM 0xFFFF0000L
#define WHITE 0xFFFFFF00L
#define get_delta(p, p1) ((FIXED)(p1 >= p ? abs((p + UNITY) - p1) : abs((p1 + UNITY) - p)))

/************************************************************************/
BOOL RotateImage( Angle, bSmartSize )
/************************************************************************/
int Angle;
BOOL bSmartSize;
{
int pix, lin;
LPFRAME lpFrame, lpOldFrame;

if (!CacheInit(YES, NO))
	return(FALSE);
if ( !Angle )
	return( FALSE );
if ( !lpImage )
	return( FALSE );
if ( !(lpOldFrame = frame_set( NULL )) )
	return( FALSE );

Edit.Angle = Angle;
Edit.SmartSize = bSmartSize;
Edit.Distortable = NO;
Edit.ScaleX = Edit.ScaleY = 100;
Edit.Resolution = Edit.CropResolution = lpImage->xres;
Edit.Depth = DEPTH;
Edit.Width  = FGET( CROP_WIDTH, Edit.CropResolution );
Edit.Height = FGET( CROP_HEIGHT, Edit.CropResolution );
if ( FMUL( Edit.Resolution, Edit.Width ) > MAX_IMAGE_LINE )
	{
	Message( IDS_ETOOWIDE );
	return( FALSE );
	}


pix = FMUL( Edit.Resolution, Edit.Width );
lin = FMUL( Edit.Resolution, Edit.Height );

if (lpImage->UndoFrame)
    {
    frame_close(lpImage->UndoFrame);
    lpImage->UndoFrame = NULL;
    }

if ( Edit.SmartSize )
     lpFrame = rotate_image_hq( &pix, &lin );
else lpFrame = rotate_image_lq( &pix, &lin );

if ( !lpFrame )
	{
	frame_set(lpOldFrame);
	return( FALSE );
        }
/* Setup the new image and bring up the new image window */
CacheNewFrame(lpFrame);
UpdateImageSize();
return( TRUE );
}


/************************************************************************/
LPFRAME rotate_image_hq( rpix, rlin )
/************************************************************************/
LPINT rpix, rlin;
{
FIXED cosine, sine, sxc, syc, dxc, dyc, cosy, siny;
FIXED fsright, fsleft, fstop, fsbottom, xp, yp, fxstart, fystart, xrate, yrate;
LPLONG  cosx_tbl, sinx_tbl;
int dwidth, dheight, swidth, sheight, sx1, sy1;
FIXED rx[4], ry[4];
FIXED minx,miny,maxx,maxy;
int j, x, y, pix, lin, xend, yend;
LPTR dest, lpBuffer[1];
LPFRAME lpFrame, lpOldFrame;
SPAN span;
BOOL xrate_small, yrate_small;
long whitePixel, newPixel;
RGBS whiteRGB;
BOOL xmirror, ymirror;

if (!(lpOldFrame = frame_set(NULL)))
	return(NULL);

pix = FMUL( Edit.Resolution, Edit.Width );
lin = FMUL( Edit.Resolution, Edit.Height );

yrate = FGET(( CROP_HEIGHT ), lin);
xrate = FGET(( CROP_WIDTH ), pix);

xmirror = ymirror = NO;
if ( !xmirror )
	fxstart = FGET(Edit.Crop.left, 1);
else
	{
	fxstart = FGET(Edit.Crop.right, 1);
	xrate = -xrate;
	}
if ( !ymirror )
	    fystart = FGET(Edit.Crop.top, 1);
else	
	{
	fystart = FGET(Edit.Crop.bottom, 1);
	yrate = -yrate;
	}

if (xrate == FONE)
	xrate = 0;
if (yrate == FONE)
	yrate = 0;
if (xrate && abs(xrate) <= HALF)
	xrate_small = TRUE;
else
	xrate_small = FALSE;
if (yrate && abs(yrate) <= HALF)
	yrate_small = TRUE;
else 
	yrate_small = FALSE;

/*****************************************************************/
/* get cosine and sine values for rotating source to destination */
/*****************************************************************/
SinCos( Edit.Angle, &sine, &cosine );

/**********************************************/
/* get source center and rotate corner points */
/**********************************************/
swidth = lpOldFrame->Xsize;
sheight = lpOldFrame->Ysize;
sxc = FGET(pix-1, 2);
syc = FGET(lin-1, 2);
frotate_point(0, 0, sxc, syc, cosine, sine, &rx[0], &ry[0]);
frotate_point(pix-1, 0, sxc, syc, cosine, sine, &rx[1], &ry[1]);
frotate_point(pix-1, lin-1, sxc, syc, cosine, sine, &rx[2], &ry[2]);
frotate_point(0, lin - 1, sxc, syc, cosine, sine, &rx[3], &ry[3]);

/**********************************************************/
/* calculate size of new image from rotated corner points */
/**********************************************************/
minx = FGET(32767,1); miny = FGET(32767,1); maxy = 0; maxx = 0;
for(j=0; j< 4; j++)
	{
	if ((rx[j]) < minx)	minx = (rx[j]);
	if ((rx[j]) > maxx)	maxx = (rx[j]);
	if ((ry[j]) < miny)	miny = (ry[j]);
	if ((ry[j]) > maxy)	maxy = (ry[j]);
	}
dwidth = FMUL(1, maxx - minx + FONE);
dheight = FMUL(1, maxy - miny + FONE);
dxc = FGET(dwidth-1, 2);
dyc = FGET(dheight-1, 2);

/**************************************************/    
/* build cosx and sinx tables for faster rotation */
/* sine is inverted to invert angle (360-angle)   */
/* because we are rotating back to the source     */
/**************************************************/
cosx_tbl = (LPLONG)Alloc((long)sizeof(FIXED) * (long)dwidth);
sinx_tbl = (LPLONG)Alloc((long)sizeof(FIXED) * (long)dwidth);
if (!cosx_tbl || !sinx_tbl)
	{
	if (cosx_tbl)
	    FreeUp((LPTR)cosx_tbl);
	if (sinx_tbl)
	    FreeUp((LPTR)sinx_tbl);
	return(NULL);
	}
sine = -sine;
for (x = 0; x < dwidth; ++x)
	{
	cosx_tbl[x] = (long)FMUL(FGET(x, 1) - dxc, cosine);
	sinx_tbl[x] = (long)FMUL(dxc - FGET(x, 1), sine);
	}
	
/************************************************************/
/* get source extents which will allow edges to be averaged */
/************************************************************/
fsright = FGET(Edit.Crop.right+1, 1);
fsbottom = FGET(Edit.Crop.bottom+1, 1);
fstop = FGET(Edit.Crop.top-1, 1);
fsleft = FGET(Edit.Crop.left-1, 1);

/****************************/
/* open frame for new image */
/****************************/
	if ( !(lpFrame = frame_open( lpOldFrame->Depth, dwidth, dheight, Edit.Resolution )) )
		{
		Message( IDS_ESCROPEN, (LPTR)Control.RamDisk );
		FreeUp((LPTR)cosx_tbl);
		FreeUp((LPTR)sinx_tbl);
		return( lpFrame );
		}

/*************************************/
/* setup stuff needed in inner loops */
/*************************************/
span.sx = 0;
span.dy = 1;
span.dx = dwidth;
if (!AllocLines(lpBuffer, 1, dwidth, 0))
	{
	FreeUp((LPTR)cosx_tbl);
	FreeUp((LPTR)sinx_tbl);
	frame_close(lpFrame);
	return( NULL );
	}

/************************************************************/
/* find out what a white pixel is for rotation off of image */
/************************************************************/
whiteRGB.red = whiteRGB.green = whiteRGB.blue = 255;
whitePixel = frame_tocache(&whiteRGB);

/*******************************************************************/
/* rotate the sucker, line by line rotating each destination pixel */
/* back to the source image and averaging based on the areas       */
/* of the four pixels which the rotated pixel will land on	 */
/*******************************************************************/
for (y = 0; y < dheight; ++y)
	{
	AstralClockCursor( y, dheight);
	
	/*******************************/
	/* set frame to source image   */
	/* and destination line buffer */
	/******************************/
	frame_set( lpOldFrame );
	dest = lpBuffer[0];
		
	/***********************************************************/
	/* calculate sine*y and cos*y here so inner loop is faster */
	/***********************************************************/
	siny = FMUL(dyc - FGET(y, 1), sine);
	cosy = FMUL(dyc - FGET(y, 1), cosine);
	
	/******************************************/
	/* the inner loop where it all happens    */
	/* this loop must be as tight as possible */
	/* or this routine will be a dog	  */
	/******************************************/
	for (x = 0; x < dwidth; ++x)
	    {
	    /*************************************/
	    /* rotate destination back to source */
	    /*************************************/
	    xp = frotate_x(cosx_tbl[x], siny, sxc);
	    yp = frotate_y(sinx_tbl[x], cosy, syc);
	    if (xrate)
		xp = FMUL(xp, xrate);
	    xp += fxstart;
	    if (xrate_small)
		xp = FGET(ROUND(xp), 1);
	    if (yrate)
		yp = FMUL(yp, yrate);
	    yp += fystart;
	    if (yrate_small)
		yp = FGET(ROUND(yp), 1);
	    
	    if (xp < fsleft || yp < fstop || xp > fsright || yp > fsbottom)
		/***************************************************/
		/* pixel is off of source image by a pixel or more */
		/***************************************************/
		{
		frame_putpixel(dest, whitePixel);
		}
	    else
		/************************************************************/
		/* averaging code - this is where most of the time is spent */
		/************************************************************/
		{
				newPixel = average_pixel(xp, yp, Edit.Crop.top, Edit.Crop.left, Edit.Crop.bottom, Edit.Crop.right, TRUE);
				frame_putpixel(dest, newPixel);
				}
		    dest += DEPTH;
			}
			
	    /************************************************************/
	    /* set to destination frame and write the line to the frame */
	    /************************************************************/
		frame_set( lpFrame );
		span.sy = y;
		if ( !frame_write( &span, lpBuffer[0], dwidth ) )
			{
			Message( IDS_ESCRWRITE, (LPTR)Control.RamDisk );
			frame_close( lpFrame );
			frame_set( lpOldFrame );
			FreeUp((LPTR)cosx_tbl);
			FreeUp((LPTR)sinx_tbl);
			FreeUp(lpBuffer[0]);
			return( NULL );
			}
	    }
	    
	/*******************************************************/
	/* now free up temp memory and set new size for caller */
	/*******************************************************/
	FreeUp((LPTR)cosx_tbl);
	FreeUp((LPTR)sinx_tbl);
FreeUp(lpBuffer[0]);
	*rpix = dwidth;
	*rlin = dheight;
	return(lpFrame);
	}


/************************************************************************/
LPFRAME rotate_image_lq( rpix, rlin )
/************************************************************************/
LPINT rpix, rlin;
{
FIXED cosine, sine, sxc, syc, dxc, dyc, cosy, siny;
FIXED xrate, yrate;
LPLONG cosx_tbl, sinx_tbl;
int xstart, ystart;
int xp, yp;
int dwidth, dheight, swidth, sheight, sx1, sy1;
FIXED rx[4], ry[4];
FIXED minx,miny,maxx,maxy;
int j, x, y, pix, lin, xend, yend;
LPTR lpBuffer[1], dest, source;
LPFRAME lpFrame, lpOldFrame;
RGBS whiteRGB;
long whitePixel;
SPAN span;
long pixel;
BOOL xmirror, ymirror;

if (!(lpOldFrame = frame_set(NULL)))
	return(NULL);

pix = FMUL( Edit.Resolution, Edit.Width );
lin = FMUL( Edit.Resolution, Edit.Height );

yrate = FGET(( CROP_HEIGHT ), lin);
xrate = FGET(( CROP_WIDTH ), pix);

xmirror = ymirror = NO;
if ( !xmirror )
	xstart = Edit.Crop.left;
else
	{
	xstart = Edit.Crop.right;
	xrate = -xrate;
	}

if ( !ymirror )
	    ystart = Edit.Crop.top;
else	
	{
	    ystart = Edit.Crop.bottom;
	    yrate = -yrate;
	    }

if (xrate == FONE)
	xrate = 0;
if (yrate == FONE)
	yrate = 0;

/*****************************************************************/
/* get cosine and sine values for rotating source to destination */
/*****************************************************************/
SinCos( Edit.Angle, &sine, &cosine );

/**********************************************/
/* get source center and rotate corner points */
/**********************************************/
swidth = lpOldFrame->Xsize;
sheight = lpOldFrame->Ysize;
sxc = FGET(pix-1, 2);
syc = FGET(lin-1, 2);
frotate_point(0, 0, sxc, syc, cosine, sine, &rx[0], &ry[0]);
frotate_point(pix-1, 0, sxc, syc, cosine, sine, &rx[1], &ry[1]);
frotate_point(pix-1, lin-1, sxc, syc, cosine, sine, &rx[2], &ry[2]);
frotate_point(0, lin - 1, sxc, syc, cosine, sine, &rx[3], &ry[3]);

/**********************************************************/
/* calculate size of new image from rotated corner points */
/**********************************************************/
	minx = FGET(32767, 1); miny = FGET(32767,1); maxy = 0; maxx = 0;
	for(j=0; j< 4; j++)
		{
		if ((rx[j]) < minx)	minx = (rx[j]);
		if ((rx[j]) > maxx)	maxx = (rx[j]);
		if ((ry[j]) < miny)	miny = (ry[j]);
		if ((ry[j]) > maxy)	maxy = (ry[j]);
		}
dwidth = FMUL(1, maxx - minx + FONE);
dheight = FMUL(1, maxy - miny + FONE);
dxc = FGET(dwidth-1, 2);
dyc = FGET(dheight-1, 2);

/**************************************************/    
/* build cosx and sinx tables for faster rotation */
/* sine is inverted to invert angle (360-angle)   */
/* because we are rotating back to the source     */
/**************************************************/
cosx_tbl = (LPLONG)Alloc((long)sizeof(FIXED) * (long)dwidth);
sinx_tbl = (LPLONG)Alloc((long)sizeof(FIXED) * (long)dwidth);
if (cosx_tbl == NULL || sinx_tbl == NULL)
	{
	if (cosx_tbl != NULL)
	    FreeUp((LPTR)cosx_tbl);
	if (sinx_tbl != NULL)
	    FreeUp((LPTR)sinx_tbl);
	return(NULL);
	}
sine = -sine;
for (x = 0; x < dwidth; ++x)
	{
	cosx_tbl[x] = FMUL(FGET(x, 1) - dxc, cosine);
	sinx_tbl[x] = FMUL(dxc - FGET(x, 1), sine);
	}
	
/****************************/
/* open frame for new image */
/****************************/
	if ( !(lpFrame = frame_open( lpOldFrame->Depth, dwidth, dheight, Edit.Resolution )) )
		{
		Message( IDS_ESCROPEN, (LPTR)Control.RamDisk );
		FreeUp((LPTR)cosx_tbl);
		FreeUp((LPTR)sinx_tbl);
		return( lpFrame );
		}

/*************************************/
/* setup stuff needed in inner loops */
/*************************************/
span.sx = 0;
span.dy = 1;
span.dx = dwidth;
if (!AllocLines(lpBuffer, 1, dwidth, 0))
	{
	FreeUp((LPTR)cosx_tbl);
	FreeUp((LPTR)sinx_tbl);
	frame_close(lpFrame);
	return( NULL );
	}
	   
/************************************************************/
/* find out what a white pixel is for rotation off of image */
/************************************************************/
whiteRGB.red = whiteRGB.green = whiteRGB.blue = 255;
whitePixel = frame_tocache(&whiteRGB);
	    
/*******************************************************************/
/* rotate the sucker, line by line rotating each destination pixel */
/* back to the source image and averaging based on the areas       */
/* of the four pixels which the rotated pixel will land on	 */
/*******************************************************************/
for (y = 0; y < dheight; ++y)
	{
	AstralClockCursor( y, dheight);
		
	/*******************************/
	/* set frame to source image   */
	/* and destination line buffer */
	/******************************/
	frame_set( lpOldFrame );
	dest = lpBuffer[0];
		
	/***********************************************************/
	/* calculate sine*y and cos*y here so inner loop is faster */
	/***********************************************************/
	siny = FMUL(dyc - FGET(y, 1), sine);
	cosy = FMUL(dyc - FGET(y, 1), cosine);
	
	/******************************************/
	/* the inner loop where it all happens    */
	/* this loop must be as tight as possible */
	/* or this routine will be a dog	  */
	/******************************************/
	for (x = 0; x < dwidth; ++x)
	    {
	    /*************************************/
	    /* rotate destination back to source */
	    /*************************************/
	    xp = rotate_x(cosx_tbl[x], siny, sxc);
	    yp = rotate_y(sinx_tbl[x], cosy, syc);
	    if (xrate)
		xp = FMUL(xp, xrate);
	    xp += xstart;
	    if (yrate)
		yp = FMUL(yp, yrate);
	    yp += ystart;
	    
	    if (xp < Edit.Crop.left || yp < Edit.Crop.top || xp > Edit.Crop.right || yp > Edit.Crop.bottom)
		/***************************************************/
		/* pixel is off of source image by a pixel or more */
		/***************************************************/
		{
		frame_putpixel(dest, whitePixel);
		}
	    else
		/************************************************************/
		/* averaging code - this is where most of the time is spent */
		/************************************************************/
		{
			    source = frame_ptr( 0, xp, yp, NO);
			    frame_getpixel(source, &pixel);
			    frame_putpixel(dest, pixel);
				}
		    dest += DEPTH;
			}
			
	    /************************************************************/
	    /* set to destination frame and write the line to the frame */
	    /************************************************************/
		frame_set( lpFrame );
		span.sy = y;
		if ( !frame_write( &span, lpBuffer[0], dwidth ) )
			{
			Message( IDS_ESCRWRITE, (LPTR)Control.RamDisk );
			frame_close( lpFrame );
			frame_set( lpOldFrame );
		    FreeUp((LPTR)cosx_tbl);
		    FreeUp((LPTR)sinx_tbl);
		    FreeUp(lpBuffer[0]);
			return( NULL );
			}
	    }
	    
	/*******************************************************/
	/* now free up temp memory and set new size for caller */
	/*******************************************************/
	FreeUp((LPTR)cosx_tbl);
	FreeUp((LPTR)sinx_tbl);
FreeUp(lpBuffer[0]);
	*rpix = dwidth;
	*rlin = dheight;
	return(lpFrame);
	}


/************************************************************************/
VOID frotate_point(x, y, xc, yc, cosine, sine, rx, ry)
/************************************************************************/
    int x, y;
    FIXED xc, yc, cosine, sine;
    LPLONG rx, ry;
    {
    *rx = FMUL(FGET(x,1) - xc, cosine) + FMUL(yc - FGET(y,1), sine);
    *ry = FMUL(xc - FGET(x,1), sine) + FMUL(yc - FGET(y,1), cosine);
    }

/************************************************************************/
long average_pixel(xp, yp, top, left, bottom, right, offimage_average)
/************************************************************************/
    FIXED xp, yp;
    int top, left, bottom, right;
    BOOL offimage_average;
    {
    if (DEPTH == 3)
		return(average_pixel24(xp, yp, top, left, bottom, right, offimage_average));
	else if (DEPTH == 2)
		return(average_pixel16(xp, yp, top, left, bottom, right, offimage_average));
    else
 		return(average_pixel8(xp, yp, top, left, bottom, right, offimage_average));
	}

/************************************************************************/
long average_pixel8(xp, yp, top, left, bottom, right, offimage_average)
/************************************************************************/
    FIXED xp, yp;
    int top, left, bottom, right;
    BOOL offimage_average;
    {
    FIXED dx1, dy1, dy2, x1, y1, value, area, total_onimage_area;
    int sx1, sy1, sx2, sy2;
    BOOL sx1on, sx2on, sy1on, sy2on;
    LPTR buffer;
    
    /****************************************/
    /* get x and y to left and top of pixel */
    /****************************************/ 
    x1 = xp & WM;
    y1 = yp & WM;
    
    if (x1 == xp && y1 == yp) 
	/**********************************/ 
	/* landed right on top of a pixel */
	/**********************************/ 
		{
	sx1 = fix2int(xp);
		sy1 = fix2int(yp);
		
	if (sx1 >= left && sy1 >= top && sx1 <= right && sy1 <= bottom)
	     {
			 buffer = frame_ptr( 0, 0, sy1, NO);
			 return(buffer[sx1]);
	     }
	else
	     /* this should probably never happen */
	     {
	     return(255);
	     }
	    }
    else
	    {
	/***************************************************************/
	/* get the x and y deltas between the four pixels we landed on */
	/***************************************************************/
	dx1 = get_delta(xp, x1);
	dy1 = get_delta(yp, y1);
	/*dx2 = FONE - dx1;*/
	dy2 = FONE - dy1;
		    
	/************************************************/
	/* get the pixel locations we will need to read */
	/************************************************/
	sx1 = fix2int(x1);
	sy1 = fix2int(y1);
	sx2 = sx1 + 1;
	sy2 = sy1 + 1;
		    
	    /*********************************************************/
	/* multiply pixel value by the area covered of the pixel */
	/* and add that to the running total - it is important   */
	/* include the pixels off of the source image for this   */
	/* averaging or edges will look jaggie!!! - I am assuming*/
	/* that frame_ptr() will return a background buffer      */
	/* pointer for y's off of image	      			 */
	/*********************************************************/
	sx1on = (sx1 < left || sx1 > right ? FALSE : TRUE);
	sx2on = (sx2 < left || sx2 > right ? FALSE : TRUE);
	sy1on = (sy1 < top || sy1 > bottom ? FALSE : TRUE);
	sy2on = (sy2 < top || sy2 > bottom ? FALSE : TRUE);
	value = 0;
		    
		area = fix2fix(dx1 * dy1);
		total_onimage_area = 0;
		if (sy1on)
		    {
		    buffer = frame_ptr( 0, 0, sy1, NO);
		    if (sx1on)
			{
			value += area * (long)buffer[sx1];
			total_onimage_area += area;
			}
		    else if (offimage_average)
			    value += area * (long)255;
				   
		    if (sx2on)
			{
			    value += (dy1 - area) * (long)buffer[sx2];
			    total_onimage_area += (dy1 - area);
			    }
		else if (offimage_average)
			    value += (dy1 - area) * (long)255;
			}
	    else if (offimage_average)
		{
		    value += area * (long)255;
			value += (dy1 - area) * (long)255;
			}
				
	    area = fix2fix(dx1 * dy2);
	    if (sy2on)
		{
		buffer = frame_ptr( 0, 0, sy2, NO);
		if (sx1on)
		    {
			    value += area * (long)buffer[sx1];
			    total_onimage_area += area;
			    }
		else if (offimage_average)
			    value += area * (long)255;
		if (sx2on)
		    {
			    value += (dy2 - area) * (long)buffer[sx2];
			    total_onimage_area += (dy2 - area);
			    }
		else if (offimage_average)
			    value += (dy2 - area) * (long)255;
			}
	    else if (offimage_average)
		{
			value += area * (long)255;
			value += (dy2 - area) * (long)255;
			}
	    if (offimage_average)
		return((BYTE)fix2int(value));
	    else if (total_onimage_area)
		return((BYTE)(value / total_onimage_area));
	    else
		{
		return(255);
		}
	    }
	}
	
/************************************************************************/
long average_pixel16(xp, yp, top, left, bottom, right, offimage_average)
/************************************************************************/
    FIXED xp, yp;
    int top, left, bottom, right;
    BOOL offimage_average;
    {
    FIXED dx1, dy1, dy2, x1, y1, rvalue, gvalue, bvalue, value, area, total_onimage_area, tarea;
    int sx1, sy1, sx2, sy2;
    BOOL sx1on, sx2on, sy1on, sy2on;
    LPWORD buffer;
    
    /****************************************/
    /* get x and y to left and top of pixel */
    /****************************************/ 
    x1 = xp & WM;
    y1 = yp & WM;
    
    if (x1 == xp && y1 == yp) 
	/**********************************/ 
	/* landed right on top of a pixel */
	/**********************************/ 
		{
	sx1 = fix2int(xp);
		sy1 = fix2int(yp);
		
	if (sx1 >= left && sy1 >= top && sx1 <= right && sy1 <= bottom)
	     {
			 buffer = (LPWORD)frame_ptr( 0, 0, sy1, NO);
			 return(buffer[sx1]);
	     }
	else
	     /* this should probably never happen */
	     {
	     return(0x7FFF);
	     }
	    }
    else
	    {
	/***************************************************************/
	/* get the x and y deltas between the four pixels we landed on */
	/***************************************************************/
	dx1 = get_delta(xp, x1);
	dy1 = get_delta(yp, y1);
	/*dx2 = FONE - dx1;*/
	dy2 = FONE - dy1;
		    
	/************************************************/
	/* get the pixel locations we will need to read */
	/************************************************/
	sx1 = fix2int(x1);
	sy1 = fix2int(y1);
	sx2 = sx1 + 1;
	sy2 = sy1 + 1;
		    
	    /*********************************************************/
	/* multiply pixel value by the area covered of the pixel */
	/* and add that to the running total - it is important   */
	/* include the pixels off of the source image for this   */
	/* averaging or edges will look jaggie!!! - I am assuming*/
	/* that frame_ptr() will return a background buffer      */
	/* pointer for y's off of image	      			 */
	/*********************************************************/
	sx1on = (sx1 < left || sx1 > right ? FALSE : TRUE);
	sx2on = (sx2 < left || sx2 > right ? FALSE : TRUE);
	sy1on = (sy1 < top || sy1 > bottom ? FALSE : TRUE);
	sy2on = (sy2 < top || sy2 > bottom ? FALSE : TRUE);
	rvalue = gvalue = bvalue = 0;
		    
		area = fix2fix(dx1 * dy1);
		total_onimage_area = 0;
		if (sy1on)
		    {
		    buffer = (LPWORD)frame_ptr( 0, 0, sy1, NO);
		    if (sx1on)
			{
			rvalue += area * MiniR(buffer[sx1]);
			gvalue += area * MiniG(buffer[sx1]);
			bvalue += area * MiniB(buffer[sx1]);
			total_onimage_area += area;
			}
		    else if (offimage_average)
			{
			value = area * (long)31;
			    rvalue += value;
			    gvalue += value;
			    bvalue += value;
			    }
				   
		    if (sx2on)
			{
			tarea = dy1 - area;
			rvalue += tarea * MiniR(buffer[sx2]);
			gvalue += tarea * MiniG(buffer[sx2]);
			bvalue += tarea * MiniB(buffer[sx2]);
			    total_onimage_area += tarea;
			    }
		else if (offimage_average)
		    {
			value = (dy1 - area) * (long)31;
			    rvalue += value;
			    gvalue += value;
			    bvalue += value;
			    }
			}
	    else if (offimage_average)
		{
		    value = area * (long)31;
			value += (dy1 - area) * (long)31;
			rvalue += value;
			gvalue += value;
			bvalue += value;
			}
				
	    area = fix2fix(dx1 * dy2);
	    if (sy2on)
		{
		buffer = (LPWORD)frame_ptr( 0, 0, sy2, NO);
		if (sx1on)
		    {
			rvalue += area * MiniR(buffer[sx1]);
			gvalue += area * MiniG(buffer[sx1]);
			bvalue += area * MiniB(buffer[sx1]);
			total_onimage_area += area;
			    }
		else if (offimage_average)
		    {
			    value = area * (long)31;
				rvalue += value;
				gvalue += value;
				bvalue += value;
			    }
		if (sx2on)
		    {
		    tarea = dy2 - area;
			rvalue += tarea * MiniR(buffer[sx2]);
			gvalue += tarea * MiniG(buffer[sx2]);
			bvalue += tarea * MiniB(buffer[sx2]);
			    total_onimage_area += tarea;
			    }
		else if (offimage_average)
		    {
			    value = (dy2 - area) * (long)31;
			    rvalue += value;
			    gvalue += value;
			    bvalue += value;
			    }
			}
	    else if (offimage_average)
		{
			value = area * (long)31;
			value += (dy2 - area) * (long)31;
			rvalue += value;
			gvalue += value;
			bvalue += value;
			}
	    if (offimage_average)
		{
		return((long)ToMiniRGB(fix2int(rvalue), fix2int(gvalue), fix2int(bvalue)));
		}
	    else if (total_onimage_area)
		{
		return((long)ToMiniRGB(rvalue/total_onimage_area, gvalue/total_onimage_area, bvalue/total_onimage_area));
		}
	    else
		{
		return(0x7FFF);
		}
	    }
	}
	
/************************************************************************/
long average_pixel24(xp, yp, top, left, bottom, right, offimage_average)
/************************************************************************/
    FIXED xp, yp;
    int top, left, bottom, right;
    BOOL offimage_average;
    {
    FIXED dx1, dy1, dy2, x1, y1, rvalue, gvalue, bvalue, value, area, total_onimage_area, tarea;
    int sx1, sy1, sx2, sy2;
    BOOL sx1on, sx2on, sy1on, sy2on;
    LPTR buffer;
    long newPixel;
    LPRGB rgbPixel;
    
    /****************************************/
    /* get x and y to left and top of pixel */
    /****************************************/ 
    x1 = xp & WM;
    y1 = yp & WM;
    
    if (x1 == xp && y1 == yp) 
	/**********************************/ 
	/* landed right on top of a pixel */
	/**********************************/ 
		{
	sx1 = fix2int(xp);
		sy1 = fix2int(yp);
		
	if (sx1 >= left && sy1 >= top && sx1 <= right && sy1 <= bottom)
	     {
			 buffer = frame_ptr( 0, sx1, sy1, NO);
			 frame_getpixel(buffer, &newPixel);
			 return(newPixel);
	     }
	else
	     /* this should probably never happen */
	     {
	     return(WHITE);
	     }
	    }
    else
	    {
	/***************************************************************/
	/* get the x and y deltas between the four pixels we landed on */
	/***************************************************************/
	dx1 = get_delta(xp, x1);
	dy1 = get_delta(yp, y1);
	/*dx2 = FONE - dx1;*/
	dy2 = FONE - dy1;
		    
	/************************************************/
	/* get the pixel locations we will need to read */
	/************************************************/
	sx1 = fix2int(x1);
	sy1 = fix2int(y1);
	sx2 = sx1 + 1;
	sy2 = sy1 + 1;
		    
	    /*********************************************************/
	/* multiply pixel value by the area covered of the pixel */
	/* and add that to the running total - it is important   */
	/* include the pixels off of the source image for this   */
	/* averaging or edges will look jaggie!!! - I am assuming*/
	/* that frame_ptr() will return a background buffer      */
	/* pointer for y's off of image	      			 */
	/*********************************************************/
	sx1on = sx1 >= left && sx1 <= right;
	sx2on = sx2 >= left && sx2 <= right;
	sy1on = sy1 >= top && sy1 <= bottom;
	sy2on = sy2 >= top && sy2 <= bottom;
	rvalue = gvalue = bvalue = 0;
		    
		area = fix2fix(dx1 * dy1);
		total_onimage_area = 0;
		if (sy1on)
		    {
		    buffer = (LPTR)frame_ptr( 0, 0, sy1, NO);
		    if (sx1on)
			{
			
			rgbPixel = (LPRGB)(buffer+(sx1*DEPTH));
			rvalue += area * rgbPixel->red;
			gvalue += area * rgbPixel->green;
			bvalue += area * rgbPixel->blue;
			total_onimage_area += area;
			}
		    else if (offimage_average)
			{
			value = area * (long)255;
			    rvalue += value;
			    gvalue += value;
			    bvalue += value;
			    }
				   
		    if (sx2on)
			{
			rgbPixel = (LPRGB)(buffer+(sx2*DEPTH));
			tarea = dy1 - area;
			rvalue += tarea * rgbPixel->red;
			gvalue += tarea * rgbPixel->green;
			bvalue += tarea * rgbPixel->blue;
			    total_onimage_area += tarea;
			    }
		else if (offimage_average)
		    {
			value = (dy1 - area) * 255L;
			    rvalue += value;
			    gvalue += value;
			    bvalue += value;
			    }
			}
	    else if (offimage_average)
		{
		    value = area * 255L;
			value += (dy1 - area) * 255L;
			rvalue += value;
			gvalue += value;
			bvalue += value;
			}
				
	    area = fix2fix(dx1 * dy2);
	    if (sy2on)
		{
		buffer = frame_ptr( 0, 0, sy2, NO);
		if (sx1on)
		    {
		    rgbPixel = (LPRGB)(buffer+(sx1*DEPTH));
			rvalue += area * rgbPixel->red;
			gvalue += area * rgbPixel->green;
			bvalue += area * rgbPixel->blue;
			total_onimage_area += area;
			    }
		else if (offimage_average)
		    {
			    value = area * 255L;
				rvalue += value;
				gvalue += value;
				bvalue += value;
			    }
		if (sx2on)
		    {
		    rgbPixel = (LPRGB)(buffer+(sx2*DEPTH));
		    tarea = dy2 - area;
			rvalue += tarea * rgbPixel->red;
			gvalue += tarea * rgbPixel->green;
			bvalue += tarea * rgbPixel->blue;
			    total_onimage_area += tarea;
			    }
		else if (offimage_average)
		    {
			    value = (dy2 - area) * 255L;
			    rvalue += value;
			    gvalue += value;
			    bvalue += value;
			    }
			}
	    else if (offimage_average)
		{
			value = area * 255L;
			value += (dy2 - area) * 255L;
			rvalue += value;
			gvalue += value;
			bvalue += value;
			}
	    if (offimage_average)
		{
		rgbPixel = (LPRGB)(&newPixel);
		rgbPixel->red = fix2int(rvalue);
		rgbPixel->green = fix2int(gvalue);
		rgbPixel->blue = fix2int(bvalue);
		return(newPixel);
		}
	    else if (total_onimage_area)
		{
		rgbPixel = (LPRGB)(&newPixel);
		rgbPixel->red = rvalue/total_onimage_area;
		rgbPixel->green = gvalue/total_onimage_area;
		rgbPixel->blue = bvalue/total_onimage_area;
		return(newPixel);
		}
	    else
		{
		return(WHITE);
		}
	    }
    }
