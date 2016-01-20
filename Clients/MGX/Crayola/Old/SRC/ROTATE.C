//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

// Static prototypes
static  struct _frame far *rotate_image_hq(LPFRAME lpSrcFrame, int Angle, LPCOLORINFO lpBGColor);
static  struct _frame far *rotate_image_lq(LPFRAME lpSrcFrame, int Angle, LPCOLORINFO lpBGColor);
static void frotate_point(
int 	x,
int		y,
LFIXED 	xc,
LFIXED	yc,
LFIXED	cosine,
LFIXED	sine,
LPLONG 	rx,
LPLONG	ry);
static  LPFRAME Rotate90CW1(LPFRAME lpSrcFrame);
static  LPFRAME Rotate90CCW1(LPFRAME lpSrcFrame);
static  LPFRAME Rotate90CW(LPFRAME lpSrcFrame);
static  LPFRAME Rotate180(LPFRAME lpSrcFrame);
static  LPFRAME Rotate90CCW(LPFRAME lpSrcFrame);
static  void RotateBitsCW(LPTR, int, LPTR, int);
static  void RotateBitsCCW(LPTR, int, LPTR, int);

#define WM 0xFFFF0000L
#define WHITE 0xFFFFFF00L
#define get_delta(p, p1) ((LFIXED)(p1 >= p ? abs((p + UNITY) - p1) : abs((p1 + UNITY) - p)))

/************************************************************************/
BOOL RotateImage( int Angle, BOOL bSmartSize, int dirty )
/************************************************************************/
{
LPFRAME lpDataFrame, lpAlphaFrame;
LPOBJECT lpBase;
LPALPHA lpAlpha;
RECT rAlpha;
COLORINFO Color;

if ( !Angle )
	return( FALSE );
if ( !lpImage )
	return( FALSE );
if (!ConfirmMaskRemove(lpImage))
	return(FALSE);
lpBase = ImgGetBase(lpImage);
lpAlpha = lpBase->lpAlpha;
lpAlphaFrame = NULL;
ProgressBegin(lpAlpha ? 2 : 1, dirty-IDS_UNDOFIRST+IDS_PROGFIRST);
if ( !ImgEditInit(lpImage, ET_OBJECT, UT_NEWDATA|UT_NEWALPHA|UT_OBJECTRECT,
						 lpBase) )
	{
	ProgressEnd();
	return( FALSE );
	}

if (lpDataFrame = RotateFrame(lpBase->Pixmap.EditFrame, Angle, NULL,
							 bSmartSize))
	{
	if (lpAlpha)
		{
		if (lpAlpha->bInvert)
			Color.gray = 255;
		else
			Color.gray = 0;
		SetColorInfo(&Color, &Color, CS_GRAY);
		lpAlphaFrame = RotateFrame(lpAlpha->Pixmap.EditFrame, Angle, &Color,
									 bSmartSize);
		}
	}

if ( !lpDataFrame || (lpAlpha && !lpAlphaFrame))
	{
	if (lpDataFrame)
		FrameClose(lpDataFrame);
	if (lpAlphaFrame)
		FrameClose(lpAlphaFrame);
	ProgressEnd();
	return( FALSE );
	}

/* Setup the new image and bring up the new image window */
ImgEditedObjectFrame(lpImage, lpBase, dirty, NULL, lpDataFrame, lpAlphaFrame);

// fix object rectangle and mask rectangle if necessary
lpBase->rObject.right = FrameXSize(lpDataFrame)-1;
lpBase->rObject.bottom = FrameYSize(lpDataFrame)-1;
if (lpAlpha)
	MaskRectUpdate(lpAlpha, &rAlpha);
UpdateImageSize();
ProgressEnd();
return( TRUE );
}

/************************************************************************/
LPFRAME RotateFrame( LPFRAME lpSrcFrame, int Angle, LPCOLORINFO lpBGColor,
					BOOL bSmartSize )
/************************************************************************/
{
LPFRAME lpDstFrame;
COLORINFO Color;

if ( !FrameDepth(lpSrcFrame) && Angle == 90) // LineArt 90
	lpDstFrame = Rotate90CW1(lpSrcFrame);
else if ( !FrameDepth(lpSrcFrame) && Angle == 270) // LineArt 270
	lpDstFrame = Rotate90CCW1(lpSrcFrame);
else if (Angle == 90)		// 90
	lpDstFrame = Rotate90CW(lpSrcFrame);
else if (Angle == 180)		// 180
	lpDstFrame = Rotate180(lpSrcFrame);
else if (Angle == 270)		// 270
	lpDstFrame = Rotate90CCW(lpSrcFrame);
else
	{
	if (!lpBGColor)
		{
		Color.gray = 255;
		SetColorInfo(&Color, &Color, CS_GRAY);
		lpBGColor = &Color;
		}
	if ( bSmartSize )
		lpDstFrame = rotate_image_hq( lpSrcFrame, Angle, lpBGColor );
	else
		lpDstFrame = rotate_image_lq( lpSrcFrame, Angle, lpBGColor );
	}
return(lpDstFrame);
}

/************************************************************************/
static LPFRAME rotate_image_hq( LPFRAME lpSrcFrame, int Angle,
							LPCOLORINFO lpBGColor )
/************************************************************************/
{
	LFIXED cosine, sine, sxc, syc, dxc, dyc, cosy, siny;
	LFIXED rx[4], ry[4], minx,miny,maxx,maxy;
	LFIXED fsright, fsleft, fstop, fsbottom, xp, yp;
	int dwidth, dheight, j, x, y, pix, lin, depth, bottom, right;
	LPTR   lpDstData;
	LPRGB  lpDstRGB;
	LPCMYK lpDstCMYK;
	BYTE   bgGray;
	RGBS   bgRGB;
	CMYKS  bgCMYK;
	long   bgPixel;

	LPLONG cosx_tbl = NULL;
	LPLONG sinx_tbl = NULL;
	LPFRAME lpFrame = NULL;

	ProgressBegin(1,0);

	pix = FrameXSize(lpSrcFrame);
	lin = FrameYSize(lpSrcFrame);
	bottom = lin-1;
	right = pix-1;

	/*****************************************************************/
	/* get cosine and sine values for rotating source to destination */
	/*****************************************************************/
	SinCos( Angle, &sine, &cosine );

	/**********************************************/
	/* get source center and rotate corner points */
	/**********************************************/
	sxc = FGET(pix-1, 2);
	syc = FGET(lin-1, 2);
	frotate_point(0,     0,     sxc, syc, cosine, sine, (LPLONG)&rx[0], (LPLONG)&ry[0]);
	frotate_point(pix-1, 0,     sxc, syc, cosine, sine, (LPLONG)&rx[1], (LPLONG)&ry[1]);
	frotate_point(pix-1, lin-1, sxc, syc, cosine, sine, (LPLONG)&rx[2], (LPLONG)&ry[2]);
	frotate_point(0, lin - 1,   sxc, syc, cosine, sine, (LPLONG)&rx[3], (LPLONG)&ry[3]);

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
	cosx_tbl = (LPLONG)Alloc((long)sizeof(LFIXED) * (long)dwidth);
	sinx_tbl = (LPLONG)Alloc((long)sizeof(LFIXED) * (long)dwidth);
	if (!cosx_tbl || !sinx_tbl)
	{
		Message (IDS_EMEMALLOC);
		goto ExitError;
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
	fsright = FGET(pix, 1);
	fsbottom = FGET(lin, 1);
	fsleft = fstop = FGET(-1, 1);

	/****************************/
	/* open frame for new image */
	/****************************/
	if ( !(lpFrame = FrameOpen( FrameType(lpSrcFrame), dwidth, dheight,
     	FrameResolution(lpSrcFrame) )) )
	{
		FrameError( IDS_EPROCESSOPEN );
		goto ExitError;
	}

	/*************************************/
	/* setup stuff needed in inner loops */
	/*************************************/
	depth = FrameDepth( lpFrame );
	if (depth == 0) depth = 1;

	/************************************************************/
	/* find out what a white pixel is for rotation off of image */
	/************************************************************/
	bgPixel = GetFrameColor(lpBGColor, FrameType(lpFrame));

	/*******************************************************************/
	/* rotate the sucker, line by line rotating each destination pixel */
	/* back to the source image and averaging based on the areas       */
	/* of the four pixels which the rotated pixel will land on	 */
	/*******************************************************************/
	switch(depth)
	{
		// Grayscale and lineart images
		case 1 :
			bgGray = bgPixel;

			for (y = 0; y < dheight; ++y)
			{
				if (AstralClockCursor( y, dheight, YES))
					goto ExitError;

				/*******************************/
				/* set frame to source image   */
				/* and destination line buffer */
				/******************************/
				lpDstData = FramePointer(lpFrame, 0, y, YES);

				if (!lpDstData) 
					continue;

				/***********************************************************/
				/* calculate sine*y and cos*y here so inner loop is faster */
				/***********************************************************/
				siny = FMUL(dyc - FGET(y, 1), sine);
				cosy = FMUL(dyc - FGET(y, 1), cosine);

				/******************************************/
				/* the inner loop where it all happens    */
				/* this loop must be as tight as possible */
				/* or this routine will be a dog          */
				/******************************************/
				for (x = 0; x < dwidth; ++x)
				{
					/*************************************/
					/* rotate destination back to source */
					/*************************************/
					xp = frotate_x(cosx_tbl[x], siny, sxc);
					yp = frotate_y(sinx_tbl[x], cosy, syc);

					/***************************************************/
					/* pixel is off of source image by a pixel or more */
					/***************************************************/
					if (xp < fsleft  ||
						yp < fstop   ||
						xp > fsright ||
						yp > fsbottom)
					{
						*lpDstData = bgGray;
					}
					else
					{
						average_pixel8(
							lpSrcFrame, xp, yp, 0, 0, bottom, right,
							TRUE, bgGray, lpDstData);
					}

					lpDstData++;
				}
			}
		break;

		case 3 :	// 24-bit RGB
			CopyRGB(  &bgPixel, &bgRGB );

			for (y = 0; y < dheight; ++y)
			{
				if (AstralClockCursor( y, dheight, YES))
					goto ExitError;

				/*******************************/
				/* set frame to source image   */
				/* and destination line buffer */
				/******************************/
				lpDstRGB = (LPRGB)FramePointerRaw( lpFrame, 0, y, TRUE );

				if (!lpDstRGB)
					break;

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

					if (xp < fsleft || yp < fstop || xp > fsright || yp > fsbottom)
					{
						/***************************************************/
						/* pixel is off of source image by a pixel or more */
						/***************************************************/
						*lpDstRGB = bgRGB;
					}
					else
					{
						/************************************************************/
						/* averaging code - this is where most of the time is spent */
						/************************************************************/
						average_pixel24(lpSrcFrame, xp, yp, 0,
    						0, bottom, right, TRUE, bgRGB, lpDstRGB);
					}
					lpDstRGB++;
				}
			}
		break;

		case 4 :	// 32-bit CMYK
			CopyCMYK( &bgPixel, &bgCMYK );

			for (y = 0; y < dheight; ++y)
			{
				if (AstralClockCursor( y, dheight, YES))
					goto ExitError;

				/*******************************/
				/* set frame to source image   */
				/* and destination line buffer */
				/*******************************/
				lpDstCMYK = (LPCMYK)FramePointerRaw(lpFrame, 0, y, TRUE);

				if (!lpDstCMYK)
					break;

				/***********************************************************/
				/* calculate sine*y and cos*y here so inner loop is faster */
				/***********************************************************/
				siny = FMUL(dyc - FGET(y, 1), sine);
				cosy = FMUL(dyc - FGET(y, 1), cosine);

				/******************************************/
				/* the inner loop where it all happens    */
				/* this loop must be as tight as possible */
				/* or this routine will be a dog          */
				/******************************************/
				for (x = 0; x < dwidth; ++x)
				{
					/*************************************/
					/* rotate destination back to source */
					/*************************************/
					xp = frotate_x(cosx_tbl[x], siny, sxc);
					yp = frotate_y(sinx_tbl[x], cosy, syc);

					if (xp < fsleft || yp < fstop || xp > fsright || yp > fsbottom)
					{
						/***************************************************/
						/* pixel is off of source image by a pixel or more */
						/***************************************************/
						*lpDstCMYK = bgCMYK;
					}
					else
					{
						/************************************************************/
						/* averaging code - this is where most of the time is spent */
						/************************************************************/
						average_pixel32(lpSrcFrame, xp, yp, 0,
    						0, bottom, right, TRUE, bgCMYK, lpDstCMYK);
					}
					lpDstCMYK++;
				}
			}
		break;
	}

	/*******************************************************/
	/* now free up temp memory and set new size for caller */
	/*******************************************************/
	FreeUp((LPTR)cosx_tbl);
	FreeUp((LPTR)sinx_tbl);
	ProgressEnd();
	return(lpFrame);

ExitError:

	if (cosx_tbl)
		FreeUp((LPTR)cosx_tbl);

	if (sinx_tbl)
		FreeUp((LPTR)sinx_tbl);

	if (lpFrame)
		FrameClose(lpFrame);

	ProgressEnd();

	return(NULL);
}

/************************************************************************/
static LPFRAME rotate_image_lq( LPFRAME lpSrcFrame, int Angle,
								LPCOLORINFO lpBGColor )
/************************************************************************/
{
LFIXED cosine, sine, sxc, syc, dxc, dyc, cosy, siny;
LFIXED rx[4], ry[4], minx,miny,maxx,maxy;
int xp, yp, dwidth, dheight, j, x, y, pix, lin, depth, right, bottom;
LPTR lpBuffer[1], dest, source;
long bgPixel;
LPLONG cosx_tbl = NULL;
LPLONG sinx_tbl = NULL;
LPFRAME lpFrame = NULL;

lpBuffer[0] = NULL;

ProgressBegin(1,0);

pix = FrameXSize(lpSrcFrame);
lin = FrameYSize(lpSrcFrame);
bottom = lin-1;
right = pix-1;

/*****************************************************************/
/* get cosine and sine values for rotating source to destination */
/*****************************************************************/
SinCos( Angle, &sine, &cosine );

/**********************************************/
/* get source center and rotate corner points */
/**********************************************/
sxc = FGET(pix-1, 2);
syc = FGET(lin-1, 2);
frotate_point(0,     0,       sxc, syc, cosine, sine, (LPLONG)&rx[0], (LPLONG)&ry[0]);
frotate_point(pix-1, 0,       sxc, syc, cosine, sine, (LPLONG)&rx[1], (LPLONG)&ry[1]);
frotate_point(pix-1, lin-1,   sxc, syc, cosine, sine, (LPLONG)&rx[2], (LPLONG)&ry[2]);
frotate_point(0,     lin - 1, sxc, syc, cosine, sine, (LPLONG)&rx[3], (LPLONG)&ry[3]);

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
cosx_tbl = (LPLONG)Alloc((long)sizeof(LFIXED) * (long)dwidth);
sinx_tbl = (LPLONG)Alloc((long)sizeof(LFIXED) * (long)dwidth);
if (cosx_tbl == NULL || sinx_tbl == NULL)
	{
	Message (IDS_EMEMALLOC);
	goto ExitError;
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
if ( !(lpFrame = FrameOpen( FrameType(lpSrcFrame), dwidth, dheight,
     FrameResolution(lpSrcFrame) )) )
	{
	FrameError( IDS_EPROCESSOPEN );
	goto ExitError;
	}

/*************************************/
/* setup stuff needed in inner loops */
/*************************************/
depth = FrameDepth( lpFrame );
if (depth == 0) depth = 1;
if (!AllocLines(lpBuffer, 1, dwidth, depth))
	{
	Message (IDS_EMEMALLOC);
	goto ExitError;
	}

/************************************************************/
/* find out what a white pixel is for rotation off of image */
/************************************************************/
bgPixel = GetFrameColor(lpBGColor, FrameType(lpFrame));

/*******************************************************************/
/* rotate the sucker, line by line rotating each destination pixel */
/* back to the source image and averaging based on the areas       */
/* of the four pixels which the rotated pixel will land on	 */
/*******************************************************************/

for (y = 0; y < dheight; ++y)
	{
	if (AstralClockCursor( y, dheight, YES))
		goto ExitError;
		
	/*******************************/
	/* set frame to source image   */
	/* and destination line buffer */
	/******************************/
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

	switch(depth)
	{
	/********/
	case 1:		// Grayscale and line art
	/********/
	for (x = 0; x < dwidth; ++x)
		{
		/*************************************/
		/* rotate destination back to source */
		/*************************************/
		xp = rotate_x((long)cosx_tbl[x], (long)siny, (long)sxc);
		yp = rotate_y((long)sinx_tbl[x], (long)cosy, (long)syc);

		if (xp < 0 || yp < 0 || xp > right || yp > bottom)
			/***************************************************/
			/* pixel is off of source image by a pixel or more */
			/***************************************************/
			{
			*dest = (BYTE)bgPixel;
			}
		else
			/************************************************************/
			/* averaging code - this is where most of the time is spent */
			/************************************************************/
			{
			source = FramePointer( lpSrcFrame, xp, yp, NO);
			*dest = *source;
			}
		dest ++;
		}
	break;

	/********/
	case 3 :	// 24-bit RGB
	/********/
	for (x = 0; x < dwidth; ++x)
		{
		/*************************************/
		/* rotate destination back to source */
		/*************************************/
		xp = rotate_x((long)cosx_tbl[x], (long)siny, (long)sxc);
		yp = rotate_y((long)sinx_tbl[x], (long)cosy, (long)syc);
		if (xp < 0 || yp < 0 || xp > right || yp > bottom)
			/***************************************************/
			/* pixel is off of source image by a pixel or more */
			/***************************************************/
			{
			CopyRGB( &bgPixel, dest );
			}
		else
			/************************************************************/
			/* averaging code - this is where most of the time is spent */
			/************************************************************/
			{
			source = FramePointer( lpSrcFrame, xp, yp, NO);
			CopyRGB( source, dest );
			}
		dest += 3;
		}
	break;

	/********/
	case 4 :	// 32-bit CMYK
	/********/
	for (x = 0; x < dwidth; ++x)
		{
		/*************************************/
		/* rotate destination back to source */
		/*************************************/
		xp = rotate_x((long)cosx_tbl[x], (long)siny, (long)sxc);
		yp = rotate_y((long)sinx_tbl[x], (long)cosy, (long)syc);

		if (xp < 0 || yp < 0 || xp > right || yp > bottom)
			/***************************************************/
			/* pixel is off of source image by a pixel or more */
			/***************************************************/
			{
			CopyCMYK( &bgPixel, dest );
			}
		else
			/************************************************************/
			/* averaging code - this is where most of the time is spent */
			/************************************************************/
			{
			source = FramePointer( lpSrcFrame, xp, yp, NO);
			CopyCMYK( source, dest );
			}
		dest += 4;
		}
	break;
	}

	/************************************************************/
	/* set to destination frame and write the line to the frame */
	/************************************************************/
	if ( !FrameWrite( lpFrame, 0, y, dwidth, lpBuffer[0], dwidth ) )
		{
		FrameError(IDS_EFRAMEREAD);
		goto ExitError;
		}
	}

/*******************************************************/
/* now free up temp memory and set new size for caller */
/*******************************************************/
FreeUp((LPTR)cosx_tbl);
FreeUp((LPTR)sinx_tbl);
FreeUp(lpBuffer[0]);
ProgressEnd();
return(lpFrame);

ExitError:
if (cosx_tbl)
	FreeUp((LPTR)cosx_tbl);
if (sinx_tbl)
	FreeUp((LPTR)sinx_tbl);
if (lpBuffer[0])
	FreeUp(lpBuffer[0]);
if (lpFrame)
	FrameClose(lpFrame);
ProgressEnd();
return(NULL);
}


/************************************************************************/
static void frotate_point(
/************************************************************************/
int 	x,
int		y,
LFIXED 	xc,
LFIXED	yc,
LFIXED	cosine,
LFIXED	sine,
LPLONG 	rx,
LPLONG	ry)
{
*rx = FMUL(FGET(x,1) - xc, cosine) + FMUL(yc - FGET(y,1), sine);
*ry = FMUL(xc - FGET(x,1), sine) + FMUL(yc - FGET(y,1), cosine);
}

/************************************************************************/

long average_pixel(
	LPFRAME lpFrame,
	LFIXED 	xp,
	LFIXED	yp,
	int 	depth,
	int		top,
	int		left,
	int		bottom,
	int		right,
	BOOL 	offimage_average,
	COLOR 	bgPixel)
{
	long lValue;

	switch(depth)
	{
		case 0 :
		case 1 :
		{
			BYTE NewPixel;

			average_pixel8(lpFrame, xp, yp, top, left, bottom, right,
				offimage_average, (BYTE)bgPixel, &NewPixel);

			lValue = NewPixel;
		}
		break;

		case 3 :
		{
			RGBS NewPixel;
			RGBS backPixel;

			CopyRGB( &bgPixel, &backPixel );

			average_pixel24(lpFrame, xp, yp, top, left, bottom, right,
				offimage_average, backPixel, &NewPixel);

			CopyRGB( &NewPixel, &lValue );
		}
		break;

		case 4 :
		{
			CMYKS NewPixel;
			CMYKS backPixel;

			CopyCMYK( &bgPixel, &backPixel );

			average_pixel32(lpFrame, xp, yp, top, left, bottom, right,
				offimage_average, backPixel, &NewPixel);

			CopyCMYK( &NewPixel, &lValue );
		}
		break;
	}

	return(lValue);
}

/************************************************************************/

#define SX1ON 1
#define SX2ON 2
#define SY1ON 4
#define SY2ON 8

void average_pixel8(
	LPFRAME lpFrame,
	LFIXED  xp,
	LFIXED	yp,
	int     top,
	int		left,
	int		bottom,
	int		right,
	BOOL    offimage_average,
	BYTE    bgPixel,
	LPTR    lpNewPixel)
{
	LFIXED dx1, dy1, dy2, x1, y1, value, area1, area2;
	int    iPixLoc;
	LPTR   lpBuf1;
	LPTR   lpBuf2;
	register int sx1, sy1;

	/****************************************/
	/* get x and y to left and top of pixel */
	/****************************************/ 
	x1 = (long)xp & WM;
	y1 = (long)yp & WM;

	if (x1 == xp && y1 == yp)
	{
		/**********************************/ 
		/* landed right on top of a pixel */
		/**********************************/ 
		sx1 = fix2int((long)xp);
		sy1 = fix2int((long)yp);

		if (sx1 >= left && sy1 >= top && sx1 <= right && sy1 <= bottom)
		{
			lpBuf1 = FramePointer( lpFrame, sx1, sy1, NO);
			*lpNewPixel = *lpBuf1;
		}
		else
		{ /* this should probably never happen */
			*lpNewPixel = bgPixel;
		}
	}
	else
	{
		/***************************************************************/
		/* get the x and y deltas between the four pixels we landed on */
		/***************************************************************/
		dx1 = get_delta(xp, x1);
		dy1 = get_delta(yp, y1);
		dy2 = FONE - dy1;

		/************************************************/
		/* get the pixel locations we will need to read */
		/************************************************/
		sx1 = fix2int((long)x1);
		sy1 = fix2int((long)y1);

		/*********************************************************/
		/* multiply pixel value by the area covered of the pixel */
		/* and add that to the running total - it is important   */
		/* include the pixels off of the source image for this   */
		/* averaging or edges will look jaggie!!! - I am assuming*/
		/* that frame_ptr() will return a background buffer      */
		/* pointer for y's off of image                          */
		/*********************************************************/

		iPixLoc = 
			(sx1     < left || sx1     > right  ? 0 : SX1ON) | // sx1on
			((sx1+1) < left || (sx1+1) > right  ? 0 : SX2ON) | // sx2on
			(sy1     < top  || sy1     > bottom ? 0 : SY1ON) | // sy1on
			((sy1+1) < top  || (sy1+1) > bottom ? 0 : SY2ON);  // sy2on

		area1 = (unsigned long)(quick_lmul(dx1,dy1)) >> 16;
		area2 = (unsigned long)(quick_lmul(dx1,dy2)) >> 16;

		value = 0;

		if (offimage_average)
		{
			if (iPixLoc == (SX1ON | SX2ON | SY1ON | SY2ON))
			{
				lpBuf1 = FramePointer( lpFrame, sx1, sy1, NO);
				lpBuf2 = FramePointer( lpFrame, sx1, sy1+1, NO);

 				quick_average8(
					lpBuf1, 
					lpBuf2,
					(long)area1, 
					(long)(dy1 - area1), 
					(long)area2, 
					(long)(dy2 - area2),
					lpNewPixel );

				return;
			}
			else
			{
				if ( iPixLoc & SY1ON )
				{
					lpBuf1 = FramePointer( lpFrame, sx1, sy1, NO);
					value += quick_lmul2(
						(iPixLoc & SX1ON) ? lpBuf1[0] : bgPixel, area1,
						(iPixLoc & SX2ON) ? lpBuf1[1] : bgPixel, dy1 - area1);
				}
				else
				{
					value += quick_lmul(dy1, bgPixel);
				}

				if ( iPixLoc & SY2ON )
				{
					lpBuf2 = FramePointer( lpFrame, sx1, sy1+1, NO);

					value += quick_lmul2(
						(iPixLoc & SX1ON) ? lpBuf2[0] : bgPixel, area2,
						(iPixLoc & SX2ON) ? lpBuf2[1] : bgPixel, dy2 - area2);
				}
				else
				{
					value += quick_lmul(dy2, bgPixel);
				}
			}

			*lpNewPixel = fix2int((long)value);
		}
		else
		{
			LFIXED total_onimage_area = 0;

			if (iPixLoc == (SX1ON | SX2ON | SY1ON | SY2ON))
			{
				lpBuf1 = FramePointer( lpFrame, sx1, sy1, NO);
				lpBuf2 = FramePointer( lpFrame, sx1, sy1+1, NO);

				quick_average8(
					lpBuf1, 
					lpBuf2,
					(long)area1, 
					(long)(dy1 - area1), 
					(long)area2, 
					(long)(dy2 - area2),
					lpNewPixel );

				return;
			}
			else
			{
				if (iPixLoc & SY1ON)
				{
					lpBuf1 = FramePointer( lpFrame, sx1, sy1, NO);

					if (iPixLoc & SX1ON)
					{
						value += quick_lmul(area1, lpBuf1[0]); 
						total_onimage_area += area1;
					}

					if (iPixLoc & SX2ON)
					{
						value += quick_lmul(dy1 - area1, lpBuf1[1]);
						total_onimage_area += dy1 - area1;
					}
				}

				if (iPixLoc & SY2ON)
				{
					lpBuf2 = FramePointer( lpFrame, sx1, sy1+1, NO);

					if (iPixLoc & SX1ON)
					{
						value += quick_lmul(area2, lpBuf2[0]);
						total_onimage_area += area2;
					}

					if (iPixLoc & SX2ON)
					{
						value += quick_lmul(dy2 - area2, lpBuf2[1]);
						total_onimage_area += dy2 - area2;
					}
				}
			}
			
			if (total_onimage_area)
			{
				*lpNewPixel = (value / total_onimage_area);
			}
			else
			{
				*lpNewPixel = (BYTE)bgPixel;
			}
		}
	}
}

/************************************************************************/

void average_pixel24(
	LPFRAME lpFrame,
	LFIXED  xp,
	LFIXED	yp,
	int     top,
	int		left,
	int		bottom,
	int		right,
	BOOL    offimage_average,
	RGBS    bgPixel,
	LPRGB   lpNewPixel)
{
	LFIXED dx1, dy1, dy2, x1, y1, area1, area2;
	LFIXED valueR, valueG, valueB;
	LPTR   lpBuf1;
	LPTR   lpBuf2;
	int    iPixLoc;
	register int sx1, sy1;

	/****************************************/
	/* get x and y to left and top of pixel */
	/****************************************/ 
	x1 = (long)xp & WM;
	y1 = (long)yp & WM;

	if (x1 == xp && y1 == yp)
	{
		/**********************************/ 
		/* landed right on top of a pixel */
		/**********************************/ 
		sx1 = fix2int((long)xp);
		sy1 = fix2int((long)yp);

		if (sx1 >= left && sy1 >= top && sx1 <= right && sy1 <= bottom)
		{
			lpBuf1 = FramePointerRaw( lpFrame, sx1, sy1, NO);

			*lpNewPixel = *(LPRGB)(lpBuf1);
		}
		else
		{ /* this should probably never happen */
			*lpNewPixel = bgPixel;
		}
	}
	else
	{
		/***************************************************************/
		/* get the x and y deltas between the four pixels we landed on */
		/***************************************************************/
		dx1 = get_delta(xp, x1);
		dy1 = get_delta(yp, y1);
		dy2 = FONE - dy1;

		/************************************************/
		/* get the pixel locations we will need to read */
		/************************************************/
		sx1 = fix2int((long)x1);
		sy1 = fix2int((long)y1);

		/*********************************************************/
		/* multiply pixel value by the area covered of the pixel */
		/* and add that to the running total - it is important   */
		/* include the pixels off of the source image for this   */
		/* averaging or edges will look jaggie!!! - I am assuming*/
		/* that frame_ptr() will return a background buffer      */
		/* pointer for y's off of image                          */
		/*********************************************************/

		iPixLoc = 
			(sx1     < left || sx1     > right  ? 0 : SX1ON) | // sx1on
			((sx1+1) < left || (sx1+1) > right  ? 0 : SX2ON) | // sx2on
			(sy1     < top  || sy1     > bottom ? 0 : SY1ON) | // sy1on
			((sy1+1) < top  || (sy1+1) > bottom ? 0 : SY2ON);  // sy2on

		area1 = (unsigned long)(quick_lmul(dx1,dy1)) >> 16;
		area2 = (unsigned long)(quick_lmul(dx1,dy2)) >> 16;

		valueR = 0;
		valueG = 0;
		valueB = 0;

		if (offimage_average)
		{
			if (iPixLoc == (SX1ON | SX2ON | SY1ON | SY2ON))
			{
				lpBuf1 = FramePointerRaw( lpFrame, sx1, sy1, NO);
				lpBuf2 = FramePointerRaw( lpFrame, sx1, sy1+1, NO);

				quick_average24(
					lpBuf1, lpBuf2,
					area1, dy1 - area1, area2, dy2 - area2,
					(LPTR)lpNewPixel);

				return;
			}
			else
			{
				if ( iPixLoc & SY1ON )
				{
					lpBuf1 = FramePointerRaw( lpFrame, sx1, sy1, NO);

					valueR += quick_lmul2(
						(iPixLoc & SX1ON) ? lpBuf1[0] : bgPixel.red, area1,
						(iPixLoc & SX2ON) ? lpBuf1[3] : bgPixel.red, dy1 - area1);

					valueG += quick_lmul2(
						(iPixLoc & SX1ON) ? lpBuf1[1] : bgPixel.green, area1,
						(iPixLoc & SX2ON) ? lpBuf1[4] : bgPixel.green, dy1 - area1);

					valueB += quick_lmul2(
						(iPixLoc & SX1ON) ? lpBuf1[2] : bgPixel.blue, area1,
						(iPixLoc & SX2ON) ? lpBuf1[5] : bgPixel.blue, dy1 - area1);
				}
				else
				{
					valueR += quick_lmul(dy1, bgPixel.red);
					valueG += quick_lmul(dy1, bgPixel.green);
					valueB += quick_lmul(dy1, bgPixel.blue);
				}

				if ( iPixLoc & SY2ON )
				{
					lpBuf2 = FramePointerRaw( lpFrame, sx1, sy1+1, NO);

					valueR += quick_lmul2(
						(iPixLoc & SX1ON) ? lpBuf2[0] : bgPixel.red, area2,
						(iPixLoc & SX2ON) ? lpBuf2[3] : bgPixel.red, dy2 - area2);

					valueG += quick_lmul2(
						(iPixLoc & SX1ON) ? lpBuf2[1] : bgPixel.green, area2,
						(iPixLoc & SX2ON) ? lpBuf2[4] : bgPixel.green, dy2 - area2);

					valueB += quick_lmul2(
						(iPixLoc & SX1ON) ? lpBuf2[2] : bgPixel.blue, area2,
						(iPixLoc & SX2ON) ? lpBuf2[5] : bgPixel.blue, dy2 - area2);
				}
				else
				{
					valueR += quick_lmul(dy2, bgPixel.red);
					valueG += quick_lmul(dy2, bgPixel.green);
					valueB += quick_lmul(dy2, bgPixel.blue);
				}
			}

			// Now bring back down to 0 - 255
			lpNewPixel->red   = (long)valueR >> 16;
			lpNewPixel->green = (long)valueG >> 16;
			lpNewPixel->blue  = (long)valueB >> 16;
		}
		else
		{
			LFIXED total_onimage_area = 0;

			if (iPixLoc == (SX1ON | SX2ON | SY1ON | SY2ON))
			{
				lpBuf1 = FramePointerRaw( lpFrame, sx1, sy1, NO);
				lpBuf2 = FramePointerRaw( lpFrame, sx1, sy1+1, NO);

				quick_average24(
					lpBuf1, lpBuf2,
					area1, dy1 - area1, area2, dy2 - area2,
					(LPTR)lpNewPixel);

				return;
			}
			else
			{
				if (iPixLoc & SY1ON)
				{
					lpBuf1 = FramePointerRaw( lpFrame, sx1, sy1, NO);

					if (iPixLoc & SX1ON)
					{
						valueR += quick_lmul(area1, lpBuf1[0]);
						valueG += quick_lmul(area1, lpBuf1[1]);
						valueB += quick_lmul(area1, lpBuf1[2]);
						total_onimage_area += area1;
					}

					if (iPixLoc & SX2ON)
					{
						valueR += quick_lmul(dy1 - area1, lpBuf1[3]);
						valueG += quick_lmul(dy1 - area1, lpBuf1[4]);
						valueB += quick_lmul(dy1 - area1, lpBuf1[5]);
						total_onimage_area += dy1 - area1;
					}
				}

				if (iPixLoc & SY2ON)
				{
					lpBuf2 = FramePointerRaw( lpFrame, sx1, sy1+1, NO);

					if (iPixLoc & SX1ON)
					{
						valueR += quick_lmul(area2, lpBuf2[0]);
						valueG += quick_lmul(area2, lpBuf2[1]);
						valueB += quick_lmul(area2, lpBuf2[2]);
						total_onimage_area += area2;
					}

					if (iPixLoc & SX2ON)
					{
						valueR += quick_lmul(dy2 - area2, lpBuf2[3]);
						valueG += quick_lmul(dy2 - area2, lpBuf2[4]);
						valueB += quick_lmul(dy2 - area2, lpBuf2[5]);
						total_onimage_area += dy2 - area2;
					}
				}
			}
			
			if (total_onimage_area)
			{
				lpNewPixel->red   = valueR / total_onimage_area;
				lpNewPixel->green = valueG / total_onimage_area;
				lpNewPixel->blue  = valueB / total_onimage_area;
			}
			else
			{
				*lpNewPixel = bgPixel;
			}
		}
	}
}

/************************************************************************/

void average_pixel32(
	LPFRAME lpFrame,
	LFIXED  xp,
	LFIXED	yp,
	int     top,
	int		left,
	int		bottom,
	int		right,
	BOOL    offimage_average,
	CMYKS   bgPixel,
	LPCMYK  lpNewPixel)
{
	LFIXED dx1, dy1, dy2, x1, y1, area1, area2;
	LFIXED valueC, valueM, valueY, valueK;
	LPTR   lpBuf1;
	LPTR   lpBuf2;
	int    iPixLoc;
	register int sx1, sy1;

	/****************************************/
	/* get x and y to left and top of pixel */
	/****************************************/ 
	x1 = (long)xp & WM;
	y1 = (long)yp & WM;

	if (x1 == xp && y1 == yp)
	{
		/**********************************/ 
		/* landed right on top of a pixel */
		/**********************************/ 
		sx1 = fix2int((long)xp);
		sy1 = fix2int((long)yp);

		if (sx1 >= left && sy1 >= top && sx1 <= right && sy1 <= bottom)
		{
			lpBuf1 = FramePointerRaw( lpFrame, sx1, sy1, NO);

			*lpNewPixel = *(LPCMYK)(lpBuf1);
		}
		else
		{ /* this should probably never happen */
			*lpNewPixel = bgPixel;
		}
	}
	else
	{
		/***************************************************************/
		/* get the x and y deltas between the four pixels we landed on */
		/***************************************************************/
		dx1 = get_delta(xp, x1);
		dy1 = get_delta(yp, y1);
		dy2 = FONE - dy1;

		/************************************************/
		/* get the pixel locations we will need to read */
		/************************************************/
		sx1 = fix2int((long)x1);
		sy1 = fix2int((long)y1);

		/*********************************************************/
		/* multiply pixel value by the area covered of the pixel */
		/* and add that to the running total - it is important   */
		/* include the pixels off of the source image for this   */
		/* averaging or edges will look jaggie!!! - I am assuming*/
		/* that frame_ptr() will return a background buffer      */
		/* pointer for y's off of image                          */
		/*********************************************************/

		iPixLoc = 
			(sx1     < left || sx1     > right  ? 0 : SX1ON) | // sx1on
			((sx1+1) < left || (sx1+1) > right  ? 0 : SX2ON) | // sx2on
			(sy1     < top  || sy1     > bottom ? 0 : SY1ON) | // sy1on
			((sy1+1) < top  || (sy1+1) > bottom ? 0 : SY2ON);  // sy2on

		area1 = (unsigned long)(quick_lmul(dx1,dy1)) >> 16;
		area2 = (unsigned long)(quick_lmul(dx1,dy2)) >> 16;

		valueC = 0;
		valueM = 0;
		valueY = 0;
		valueK = 0;

		if (offimage_average)
		{
			if (iPixLoc == (SX1ON | SX2ON | SY1ON | SY2ON))
			{
				lpBuf1 = FramePointerRaw( lpFrame, sx1, sy1, NO);
				lpBuf2 = FramePointerRaw( lpFrame, sx1, sy1+1, NO);

				quick_average32(
					lpBuf1, lpBuf2,
					area1, dy1 - area1, area2, dy2 - area2,
					(LPTR)lpNewPixel);

				return;
			}
			else
			{
				if ( iPixLoc & SY1ON )
				{
					lpBuf1 = FramePointerRaw( lpFrame, sx1, sy1, NO);

					valueC += quick_lmul2(
						(iPixLoc & SX1ON) ? lpBuf1[0] : bgPixel.c, area1,
						(iPixLoc & SX2ON) ? lpBuf1[4] : bgPixel.c, dy1 - area1);

					valueM += quick_lmul2(
						(iPixLoc & SX1ON) ? lpBuf1[1] : bgPixel.m, area1,
						(iPixLoc & SX2ON) ? lpBuf1[5] : bgPixel.m, dy1 - area1);

					valueY += quick_lmul2(
						(iPixLoc & SX1ON) ? lpBuf1[2] : bgPixel.y, area1,
						(iPixLoc & SX2ON) ? lpBuf1[6] : bgPixel.y, dy1 - area1);

					valueK += quick_lmul2(
						(iPixLoc & SX1ON) ? lpBuf1[3] : bgPixel.k, area1,
						(iPixLoc & SX2ON) ? lpBuf1[7] : bgPixel.k, dy1 - area1);
				}
				else
				{
					valueC += quick_lmul(dy1, bgPixel.c);
					valueM += quick_lmul(dy1, bgPixel.m);
					valueY += quick_lmul(dy1, bgPixel.y);
					valueK += quick_lmul(dy1, bgPixel.k);
				}

				if ( iPixLoc & SY2ON )
				{
					lpBuf2 = FramePointerRaw( lpFrame, sx1, sy1+1, NO);

					valueC += quick_lmul2(
						(iPixLoc & SX1ON) ? lpBuf2[0] : bgPixel.c, area2,
						(iPixLoc & SX2ON) ? lpBuf2[4] : bgPixel.c, dy1 - area2);

					valueM += quick_lmul2(
						(iPixLoc & SX1ON) ? lpBuf2[1] : bgPixel.m, area2,
						(iPixLoc & SX2ON) ? lpBuf2[5] : bgPixel.m, dy1 - area2);

					valueY += quick_lmul2(
						(iPixLoc & SX1ON) ? lpBuf2[2] : bgPixel.y, area2,
						(iPixLoc & SX2ON) ? lpBuf2[6] : bgPixel.y, dy1 - area2);

					valueK += quick_lmul2(
						(iPixLoc & SX1ON) ? lpBuf2[3] : bgPixel.k, area1,
						(iPixLoc & SX2ON) ? lpBuf2[7] : bgPixel.k, dy2 - area2);
				}
				else
				{
					valueC += quick_lmul(dy2, bgPixel.c);
					valueM += quick_lmul(dy2, bgPixel.m);
					valueY += quick_lmul(dy2, bgPixel.y);
					valueK += quick_lmul(dy2, bgPixel.k);
				}
			}

			// Now bring back down to 0 - 255
			lpNewPixel->c = (long)valueC >> 16;
			lpNewPixel->m = (long)valueM >> 16;
			lpNewPixel->y = (long)valueY >> 16;
			lpNewPixel->k = (long)valueK >> 16;
		}
		else
		{
			LFIXED total_onimage_area = 0;

			if (iPixLoc == (SX1ON | SX2ON | SY1ON | SY2ON))
			{
				lpBuf1 = FramePointerRaw( lpFrame, sx1, sy1, NO);
				lpBuf2 = FramePointerRaw( lpFrame, sx1, sy1+1, NO);

				quick_average32(
					lpBuf1, lpBuf2,
					area1, dy1 - area1, area2, dy2 - area2,
					(LPTR)lpNewPixel);

				return;
			}
			else
			{
				if (iPixLoc & SY1ON)
				{
					lpBuf1 = FramePointerRaw( lpFrame, sx1, sy1, NO);

					if (iPixLoc & SX1ON)
					{
						valueC += quick_lmul(area1, lpBuf1[0]);
						valueM += quick_lmul(area1, lpBuf1[1]);
						valueY += quick_lmul(area1, lpBuf1[2]);
						valueK += quick_lmul(area1, lpBuf1[3]);
						total_onimage_area += area1;
					}

					if (iPixLoc & SX2ON)
					{
						valueC += quick_lmul(dy1 - area1, lpBuf1[4]);
						valueM += quick_lmul(dy1 - area1, lpBuf1[5]);
						valueY += quick_lmul(dy1 - area1, lpBuf1[6]);
						valueK += quick_lmul(dy1 - area1, lpBuf1[7]);
						total_onimage_area += dy1 - area1;
					}
				}

				if (iPixLoc & SY2ON)
				{
					lpBuf2 = FramePointerRaw( lpFrame, sx1, sy1+1, NO);

					if (iPixLoc & SX1ON)
					{
						valueC += quick_lmul(area2, lpBuf2[0]);
						valueM += quick_lmul(area2, lpBuf2[1]);
						valueY += quick_lmul(area2, lpBuf2[2]);
						valueK += quick_lmul(area2, lpBuf2[3]);
						total_onimage_area += area2;
					}

					if (iPixLoc & SX2ON)
					{
						valueC += quick_lmul(dy2 - area2, lpBuf2[4]);
						valueM += quick_lmul(dy2 - area2, lpBuf2[5]);
						valueY += quick_lmul(dy2 - area2, lpBuf2[6]);
						valueK += quick_lmul(dy2 - area2, lpBuf2[7]);
						total_onimage_area += dy2 - area2;
					}
				}
			}
			
			if (total_onimage_area)
			{
				lpNewPixel->c = valueC / total_onimage_area;
				lpNewPixel->m = valueM / total_onimage_area;
				lpNewPixel->y = valueY / total_onimage_area;
				lpNewPixel->k = valueK / total_onimage_area;
			}
			else
			{
				*lpNewPixel = bgPixel;
			}
		}
	}
}

/************************************************************************/

static LPFRAME Rotate90CW( LPFRAME lpSrcFrame)
{
LPTR lp;
int	Width, nSlices, xSlice;
int i, x, y, depth, dy, xleft, pixels;
LPTR lpBuffer[8], lpStart[8];
LPFRAME lpFrame = NULL;

ProgressBegin(1,0);

for (i = 0; i < 8; ++i)
	lpBuffer[i] = NULL;
lpFrame = FrameOpen(
	FrameType(lpSrcFrame), 
	FrameYSize(lpSrcFrame),
	FrameXSize(lpSrcFrame), 
	FrameResolution(lpSrcFrame));

if (!lpFrame)
	{
	FrameError( IDS_EPROCESSOPEN );
	goto ExitError;
	}

Width = FrameByteWidth(lpFrame);
depth = FrameDepth(lpFrame);
if (depth == 0) depth = 1;

for (i = 0; i < 8; ++i)
	{
	lpBuffer[i] = Alloc((long)Width);
	if (!lpBuffer[i])
		{
		Message (IDS_EMEMALLOC);
		goto ExitError;
		}
	}

nSlices = (FrameXSize(lpSrcFrame)+7)/8;
xleft = FrameXSize(lpSrcFrame);
x = 0;
dy = 0;
for (xSlice = 0; xSlice < nSlices; ++xSlice, x += 8)
	{
	pixels = 8;
	if (pixels > xleft)
		pixels = xleft;
	xleft -= pixels;
	if (AstralClockCursor(xSlice, nSlices, YES))
		goto ExitError;
	for (i = 0; i < pixels; ++i)
		lpStart[i] = lpBuffer[i] + Width - depth;
	for (y = 0; y < FrameYSize(lpSrcFrame); ++y)
		{
		lp = FramePointer(lpSrcFrame, x, y, NO);
		if (lp)
			{
			switch(depth)
				{
				case 1 :
					for (i = 0; i < pixels; ++i)
						*(lpStart[i]) = *lp++;
					break;

				case 3 :
					for (i = 0; i < pixels; ++i)
						{
						CopyRGB(lp, lpStart[i]);
						lp += 3;
						}
					break;

				case 4 :
					for (i = 0; i < pixels; ++i)
						{
						CopyCMYK(lp, lpStart[i]);
						lp += 4;
						}
					break;
				}
			}
		for (i = 0; i < pixels; ++i)
			lpStart[i] -= depth;
		}
	for (i = 0; i < pixels; ++i, ++dy)
		FrameWrite( lpFrame, 0, dy, FrameXSize(lpFrame), lpBuffer[i],
					FrameXSize(lpFrame));
	}
for (i = 0; i < 8; ++i)
	FreeUp(lpBuffer[i]);
ProgressEnd();
return(lpFrame);

ExitError:
for (i = 0; i < 8; ++i)
	if (lpBuffer[i])
		FreeUp(lpBuffer[i]);
if (lpFrame)
	FrameClose(lpFrame);
ProgressEnd();
return(NULL);
}


/************************************************************************/
static LPFRAME Rotate180( LPFRAME lpSrcFrame)
/************************************************************************/
{
LPFRAME lpFrame;
int ySrc, yDst, pix, lin, depth;
LPTR lp;
void (far *lpMirrorProc)(LPTR,int);

ProgressBegin(1,0);

switch ( depth = FrameDepth(lpSrcFrame) )
    {
    case 0:
		depth = 1;
    case 1:
		lpMirrorProc = mirror8;
		break;
    case 3:
		lpMirrorProc = mirror24;
		break;
    case 4:
		lpMirrorProc = mirror32;
		break;
    default:
		ProgressEnd();
		return( NULL );
    }

lpFrame = FrameOpen(
	FrameType(lpSrcFrame), 
	FrameXSize(lpSrcFrame),
	FrameYSize(lpSrcFrame), 
	FrameResolution(lpSrcFrame));

if (!lpFrame)
{
	ProgressEnd();
	FrameError( IDS_EPROCESSOPEN );
	return( NULL );
}
pix = FrameXSize(lpFrame);
lin = FrameYSize(lpFrame);

yDst = lin-1;
for (ySrc = 0; ySrc < lin; ++ySrc)
	{
	AstralClockCursor(ySrc, lin, NO);
	
	if (!(lp = FramePointer(lpFrame, 0, yDst, YES)))
		{
		FrameClose(lpFrame);
		FrameError(IDS_EFRAMEREAD);
		ProgressEnd();
		return(NULL);
		}
		
	if ( !FrameRead( lpSrcFrame, 0, ySrc, pix, lp, pix) )
		{
		FrameClose(lpFrame);
		FrameError(IDS_EFRAMEREAD);
		ProgressEnd();
		return(FALSE);
		}
	(*lpMirrorProc)( lp, pix );
	yDst--;
	}
ProgressEnd();
return(lpFrame);
}


/************************************************************************/
static LPFRAME Rotate90CCW(LPFRAME lpSrcFrame)
/************************************************************************/
{
LPTR lp;
int	Width, nSlices, xSlice;
int i, x, y, depth, dy, xleft, pixels;
LPTR lpBuffer[8], lpStart[8];
LPFRAME lpFrame;

ProgressBegin(1,0);

lpFrame = FrameOpen(
	FrameType(lpSrcFrame),
	FrameYSize(lpSrcFrame),
	FrameXSize(lpSrcFrame),
	FrameResolution(lpSrcFrame));

if (!lpFrame)
{
	ProgressEnd();
	FrameError( IDS_EPROCESSOPEN );
	return(NULL);
}
Width = FrameByteWidth(lpFrame);
depth = FrameDepth(lpFrame);
if (depth == 0) depth = 1;
for (i = 0; i < 8; ++i)
	{
	lpBuffer[i] = Alloc((long)Width);
	if (!lpBuffer[i])
		{
		while (--i >= 0)
			FreeUp(lpBuffer[i]);
		Message (IDS_EMEMALLOC);
		FrameClose(lpFrame);
		ProgressEnd();
		return(NULL);
		}
	}

nSlices = (FrameXSize(lpSrcFrame)+7)/8;
xleft = FrameXSize(lpSrcFrame);
x = FrameXSize(lpSrcFrame)-1;
dy = 0;
for (xSlice = 0; xSlice < nSlices; ++xSlice, x -= 8)
	{
	pixels = 8;
	if (pixels > xleft)
		pixels = xleft;
	xleft -= pixels;
	if (AstralClockCursor(xSlice, nSlices, YES))
		{
		for (i = 0; i < 8; ++i)
			FreeUp(lpBuffer[i]);
		FrameClose(lpFrame);
		ProgressEnd();
		return(NULL);
		}
	for (i = 0; i < pixels; ++i)
		lpStart[i] = lpBuffer[i];
	for (y = 0; y < FrameYSize(lpSrcFrame); ++y)
		{
		lp = FramePointer( lpSrcFrame, x, y, NO);
		if (lp)
			{
			switch(depth)
				{
				case 1 :
					for (i = 0; i < pixels; ++i)
						*(lpStart[i]) = *lp--;
					break;

				case 3 :
					for (i = 0; i < pixels; ++i)
						{
						CopyRGB(lp, lpStart[i]);
						lp -= 3;
						}
					break;

				case 4 :
					for (i = 0; i < pixels; ++i)
						{
						CopyCMYK(lp, lpStart[i]);
						lp -= 4;
						}
					break;
				}
			}
		for (i = 0; i < pixels; ++i)
			lpStart[i] += depth;
		}
	for (i = 0; i < pixels; ++i, ++dy)
		FrameWrite( lpFrame, 0, dy, FrameXSize(lpFrame), lpBuffer[i],
				 FrameXSize(lpFrame));
	}
for (i = 0; i < 8; ++i)
	FreeUp(lpBuffer[i]);
ProgressEnd();
return(lpFrame);
}

/************************************************************************/

static LPFRAME Rotate90CW1( LPFRAME lpSrcFrame)
{
	LPTR lp;
	int  Width, nSlices, xSlice;
	int  i, x, y, depth, dy, xleft, pixels;
	LPTR lpBuffer[8], lpStart[8];
	LPFRAME lpFrame = NULL;

	ProgressBegin(1,0);

	for (i = 0; i < 8; ++i)
		lpBuffer[i] = NULL;

	lpFrame = FrameOpen(
		FrameType(lpSrcFrame), 
		FrameYSize(lpSrcFrame),
		FrameXSize(lpSrcFrame), 
		FrameResolution(lpSrcFrame));

	if (!lpFrame)
		{
		FrameError( IDS_EPROCESSOPEN );
		goto ExitError;
		}

	Width = FrameXSize(lpFrame);
	depth = FrameDepth(lpFrame);
	if (depth == 0) depth = 1;

	for (i = 0; i < 8; ++i)
	{
		lpBuffer[i] = Alloc((long)Width);
		if (!lpBuffer[i])
		{
			Message (IDS_EMEMALLOC);
			goto ExitError;
		}
	}

	nSlices = (FrameXSize(lpSrcFrame)+7)/8;
	xleft = FrameXSize(lpSrcFrame);
	x = 0;
	dy = 0;

	for (xSlice = 0; xSlice < nSlices; ++xSlice, x += 8)
	{
		pixels = 8;
		if (pixels > xleft)
			pixels = xleft;
		xleft -= pixels;
		if (AstralClockCursor(xSlice, nSlices, YES))
			goto ExitError;
		for (i = 0; i < pixels; ++i)
			lpStart[i] = lpBuffer[i] + Width - depth;

		for (y = 0; y < FrameYSize(lpSrcFrame); ++y)
		{
			lp = FramePointer(lpSrcFrame, x, y, NO);
			if (lp)
			{
				for (i = 0; i < pixels; ++i)
					*(lpStart[i]) = *lp++;
			}
			for (i = 0; i < pixels; ++i)
				lpStart[i] -= depth;
		}
		for (i = 0; i < pixels; ++i, ++dy)
			FrameWrite( lpFrame, 0, dy, FrameXSize(lpFrame), lpBuffer[i],
				FrameXSize(lpFrame));
	}

	for (i = 0; i < 8; ++i)
		FreeUp(lpBuffer[i]);

	ProgressEnd();
	return(lpFrame);

ExitError:
	for (i = 0; i < 8; ++i)
		if (lpBuffer[i])
			FreeUp(lpBuffer[i]);
	if (lpFrame)
		FrameClose(lpFrame);
	ProgressEnd();
	return(NULL);
}

/************************************************************************/

static LPFRAME Rotate90CCW1(LPFRAME lpSrcFrame)
{
	LPTR lp;
	int  Width, nSlices, xSlice;
	int  i, x, y, depth, dy, xleft, pixels;
	LPTR lpBuffer[8], lpStart[8];
	LPFRAME lpFrame;

	ProgressBegin(1,0);

	lpFrame = FrameOpen(
		FrameType(lpSrcFrame),
		FrameYSize(lpSrcFrame),
		FrameXSize(lpSrcFrame),
		FrameResolution(lpSrcFrame));

	if (!lpFrame)
	{
		ProgressEnd();
		FrameError( IDS_EPROCESSOPEN );
		return(NULL);
	}

	Width = FrameXSize(lpFrame);
	depth = FrameDepth(lpFrame);
	if (depth == 0) depth = 1;

	for (i = 0; i < 8; ++i)
	{
		lpBuffer[i] = Alloc((long)Width);
		if (!lpBuffer[i])
		{
			while (--i >= 0)
				FreeUp(lpBuffer[i]);
			Message (IDS_EMEMALLOC);
			FrameClose(lpFrame);
			ProgressEnd();
			return(NULL);
		}
	}

	nSlices = (FrameXSize(lpSrcFrame)+7)/8;
	xleft   = FrameXSize(lpSrcFrame);
	x       = FrameXSize(lpSrcFrame)-1;
	dy      = 0;

	for (xSlice = 0; xSlice < nSlices; ++xSlice, x -= 8)
	{
		pixels = 8;
		if (pixels > xleft)
			pixels = xleft;
		xleft -= pixels;
		if (AstralClockCursor(xSlice, nSlices, YES))
		{
			for (i = 0; i < 8; ++i)
				FreeUp(lpBuffer[i]);
			FrameClose(lpFrame);
			ProgressEnd();
			return(NULL);
		}
		for (i = 0; i < pixels; ++i)
			lpStart[i] = lpBuffer[i];
		for (y = 0; y < FrameYSize(lpSrcFrame); ++y)
		{
			lp = FramePointer( lpSrcFrame, x, y, NO);
			if (lp)
			{
				for (i = 0; i < pixels; ++i)
					*(lpStart[i]) = *lp--;
			}
			for (i = 0; i < pixels; ++i)
				lpStart[i] += depth;
		}
		for (i = 0; i < pixels; ++i, ++dy)
			FrameWrite( lpFrame, 0, dy, FrameXSize(lpFrame), lpBuffer[i],
				FrameXSize(lpFrame));
	}

	for (i = 0; i < 8; ++i)
		FreeUp(lpBuffer[i]);
	ProgressEnd();
	return(lpFrame);
}

/************************************************************************/

static void RotateBitsCW(
LPTR	lpSrc,
int		lines,
LPTR	lpStart,
int		Width)
{
#ifdef C_CODE
BYTE	dbit, sbit, byte;
int	iCount;
LPTR	lpDst;

dbit = 1;
while (--lines >= 0)
	{
	sbit = 128;
	byte = *lpSrc++;
	lpDst = lpStart;
	iCount = 8;
	while (--iCount >= 0)
		{
		if (byte & sbit)
			*lpDst |= dbit;
		sbit >>= 1;
		lpDst += Width;
		}
	dbit <<= 1;
	}
#else
__asm	{
	mov	cx, lines
	lds	si, lpSrc
	cld
	mov	bl, 1			; BL = dbit
	mov	dx, Width
	jmp	RBCW_10

RBCW_loop1:
	mov	bh, 128			; BH = sbit
	lodsb				; al = *lpSrc++
	les	di, lpStart		; lpDst = lpStart
	mov	ch, 8			; iCount
	jmp	RBCW_6

RBCW_loop2:
	test	al, bh			; byte & sbit
	je	RBCW_1
	or	es:[di], bl		; *lpDst |= dbit
RBCW_1:
	shr	bh, 1			; sbit>>=1
	add	di, dx			; lpDst
RBCW_6:
	dec	ch			; iCount
	jns	RBCW_loop2

	shl	bl, 1			; dbit
RBCW_10:
	dec	cl			; lines
	jns	RBCW_loop1
	}
#endif
}

/************************************************************************/
static void RotateBitsCCW(
/************************************************************************/
LPTR	lpSrc,
int		lines,
LPTR	lpStart,
int		Width)
{
#ifdef C_CODE
BYTE	dbit, sbit, byte;
int	iCount;
LPTR	lpDst;

dbit = 128;
while (--lines >= 0)
	{
	sbit = 1;
	byte = *lpSrc++;
	lpDst = lpStart;
	iCount = 8;
	while (--iCount >= 0)
		{
		if (byte & sbit)
			*lpDst |= dbit;
		sbit <<= 1;
		lpDst += Width;
		}
	dbit >>= 1;
	}
#else
__asm	{
	mov	cx, lines
	lds	si, lpSrc
	cld
	mov	bl, 128			; BL = dbit
	mov	dx, Width
	jmp	RBCCW_10

RBCCW_loop1:
	mov	bh, 1			; BH = sbit
	lodsb				; al = *lpSrc++
	les	di, lpStart		; lpDst = lpStart
	mov	ch, 8			; iCount
	jmp	RBCCW_6

RBCCW_loop2:
	test	al, bh			; byte & sbit
	je	RBCCW_1
	or	es:[di], bl		; *lpDst |= dbit
RBCCW_1:
	shl	bh, 1			; sbit>>=1
	add	di, dx			; lpDst
RBCCW_6:
	dec	ch			; iCount
	jns	RBCCW_loop2

	shr	bl, 1			; dbit
RBCCW_10:
	dec	cl			; lines
	jns	RBCCW_loop1
	}
#endif
}
