//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

// Static prototypes
static  struct _frame far *rotate_image_hq(LPFRAME lpSrcFrame, LFIXED Angle, LPCOLORINFO lpBGColor);
static  struct _frame far *rotate_image_lq(LPFRAME lpSrcFrame, LFIXED Angle, LPCOLORINFO lpBGColor);
static  void frotate_point(int x,int y,LFIXED xc,LFIXED yc,LFIXED cosine,
                           LFIXED sine,LFIXED far *rx,LFIXED far *ry);
static  LPFRAME Rotate90CW1(LPFRAME lpSrcFrame);
static  LPFRAME Rotate90CCW1(LPFRAME lpSrcFrame);
static  LPFRAME Rotate90CW(LPFRAME lpSrcFrame);
static  LPFRAME Rotate180(LPFRAME lpSrcFrame);
static  LPFRAME Rotate90CCW(LPFRAME lpSrcFrame);
static  void RotateBitsCW(LPTR, int, LPTR, int);
static  void RotateBitsCCW(LPTR, int, LPTR, int);
LOCAL BOOL RotateFrameLQ(LPVOID lpParam, LPRECT lpArea);
LOCAL BOOL RotateFrameHQ(LPVOID lpParam, LPRECT lpArea);

#define WM 0xFFFF0000L
#define WHITE 0xFFFFFF00L
#define get_delta(p, p1) ((LFIXED)(p1 >= p ? abs((p + FUNITY) - p1) : abs((p1 + FUNITY) - p)))

#define rotate_x(cosx, sinyplusxc) POSFROUND((cosx) + (sinyplusxc))
#define rotate_y(sinx, ycminuscosy) POSFROUND((ycminuscosy) - (sinx))
#define frotate_x(cosx, sinyplusxc) ((cosx) + (sinyplusxc))
#define frotate_y(sinx, ycminuscosy) ((ycminuscosy) - (sinx))

typedef struct
{
	LPFRAME lpDataFrame;
	LPFRAME lpAlphaFrame;
} ROTATEDATA, FAR *LPROTATEDATA;

/************************************************************************/
BOOL RotateImage( LPIMAGE lpImage, LPROTATE_PARMS lpParms )
/************************************************************************/
{
	LPOBJECT lpBase, lpObject;
	int nPasses, nObjects, i, j;
	LPALPHA lpAlpha;
	RECT rAlpha;
	COLORINFO Color;
	LFIXED sine, cosine, sxc, syc, dxc, dyc;
	LPROTATEDATA lpRotData;
	LFIXED rx[4], ry[4], minx,maxy;

	if (!lpImage || !lpParms)
		return(FALSE);
	if ( !lpParms->Angle )
		return( TRUE ); // not a failure, just not necessary

	nObjects = nPasses = 0;
	lpObject = NULL;
	nObjects = nPasses = 0;
	lpObject = NULL;
	while (lpObject = ImgGetNextObject(lpImage, lpObject, TRUE, FALSE))
	{
		++nObjects;
		++nPasses;
		if (lpObject->lpAlpha)
			++nPasses;
	}

	lpRotData = (LPROTATEDATA)AllocX(sizeof(ROTATEDATA)*nObjects, GMEM_ZEROINIT);
	if (!lpRotData)
	{
		Message(IDS_EMEMALLOC);
		return(FALSE);
	}

	lpBase = ImgGetBase(lpImage);
	lpAlpha = lpBase->lpAlpha;
	ProgressBegin(nPasses, PROGRESS_ID(lpParms->Common.idDirty));
	if ( !ImgEditInit(lpImage, ET_ALLOBJECTS, UT_NEWDATA|UT_NEWALPHA|UT_OBJECTRECT,
						 	NULL) )
		{
		FreeUp(lpRotData);
		ProgressEnd();
		return( FALSE );
		}

	lpObject = NULL;
	i = 0;
	while (lpObject = ImgGetNextObject(lpImage, lpObject, TRUE, FALSE))
	{
		lpAlpha = ObjGetAlpha(lpObject);
		if (lpRotData[i].lpDataFrame = RotateFrame(ObjGetEditFrame(lpObject), lpParms->Angle, NULL,
							 		lpParms->fUseWeightedAvg))
		{
			if (lpAlpha)
 			{
				if (i == 0) // base object need's to check if we can change
				{			// the mask size
					if (!MaskSizeChange(lpAlpha,
								FrameXSize(lpRotData[i].lpDataFrame),
								FrameYSize(lpRotData[i].lpDataFrame)))
					{
						Message(IDS_EMEMALLOC);
						goto ErrorExit;
					}
				}
				if (lpAlpha->bInvert)
					Color.gray = 255;
				else
					Color.gray = 0;
				SetColorInfo(&Color, &Color, CS_GRAY);
				lpRotData[i].lpAlphaFrame = RotateFrame(AlphaGetEditFrame(lpAlpha), lpParms->Angle, &Color,
									 		lpParms->fUseWeightedAvg);
 			}
		}

		ErrorExit:

		if ( !lpRotData[i].lpDataFrame || (lpAlpha && !lpRotData[i].lpAlphaFrame))
		{
			while (i >= 0)
 			{
 				if (lpRotData[i].lpDataFrame)
 					FrameClose(lpRotData[i].lpDataFrame);
 				if (lpRotData[i].lpAlphaFrame)
 					FrameClose(lpRotData[i].lpAlphaFrame);
				--i;
 			}
 			FreeUp(lpRotData);
			ProgressEnd();
			return( FALSE );
		}
		++i;
	}

	/*****************************************************************/
	/* get cosine and sine values for rotating source to destination */
	/*****************************************************************/
	SinCos( lpParms->Angle, &sine, &cosine );

	/**********************************************/
	/* get source center and rotate corner points */
	/**********************************************/
	sxc = FGET(FrameXSize(ObjGetEditFrame(lpBase))-1, 2);
	syc = FGET(FrameYSize(ObjGetEditFrame(lpBase))-1, 2);
	dxc = FGET(FrameXSize(lpRotData[0].lpDataFrame)-1, 2);
	dyc = FGET(FrameYSize(lpRotData[0].lpDataFrame)-1, 2);

	/**********************************************************/
	/* calculate size of new image from rotated corner points */
	/**********************************************************/
	lpObject = NULL;
	i = 0;
	while (lpObject = ImgGetNextObject(lpImage, lpObject, TRUE, FALSE))
	{
		// fix object rectangle
		if (lpObject != lpBase)
		{
			frotate_point(lpObject->rObject.left, lpObject->rObject.top, sxc, syc, cosine, sine, &rx[0], &ry[0]);
			frotate_point(lpObject->rObject.right, lpObject->rObject.top, sxc, syc, cosine, sine, &rx[1], &ry[1]);
			frotate_point(lpObject->rObject.right, lpObject->rObject.bottom, sxc, syc, cosine, sine, &rx[2], &ry[2]);
			frotate_point(lpObject->rObject.left, lpObject->rObject.bottom, sxc, syc, cosine, sine, &rx[3], &ry[3]);

			minx = FGET(SHRT_MAX, 1); maxy = FGET(SHRT_MIN, 1);
			for(j=0; j< 4; j++)
			{
				if ((rx[j]) < minx)	minx = (rx[j]);
				if ((ry[j]) > maxy)	maxy = (ry[j]);
			}
			lpObject->rObject.left = FROUND(dxc+minx);
			lpObject->rObject.top = FROUND(dyc-maxy);
		}

		lpObject->rObject.right = lpObject->rObject.left + FrameXSize(lpRotData[i].lpDataFrame) - 1;
		lpObject->rObject.bottom = lpObject->rObject.top + FrameYSize(lpRotData[i].lpDataFrame) - 1;

		/* Setup the new image and bring up the new image window */
		ImgEditedObjectFrame(lpImage, lpObject, lpParms->Common.idDirty, NULL,
			lpRotData[i].lpDataFrame, lpRotData[i].lpAlphaFrame);

		// fix mask rectangle if necessary
		lpAlpha = ObjGetAlpha(lpObject);
		if (lpObject == lpBase && lpAlpha)
			MaskRectUpdate(lpAlpha, &rAlpha);

		++i;
	}

	FreeUp(lpRotData);
	lpParms->Common.UpdateType = UT_SIZE;
	ProgressEnd();
	return( TRUE );
}

/************************************************************************/
LPFRAME RotateFrame( LPFRAME lpSrcFrame, LFIXED Angle, LPCOLORINFO lpBGColor,
					BOOL bSmartSize )
/************************************************************************/
{
LPFRAME lpDstFrame;
COLORINFO Color;
FRMTYPEINFO TypeInfo;

TypeInfo = FrameTypeInfo(lpSrcFrame);
if ( TypeInfo.DataType == FDT_LINEART && Angle == TOFIXED(90)) // LineArt 90
	lpDstFrame = Rotate90CW1(lpSrcFrame);
else if ( TypeInfo.DataType == FDT_LINEART && Angle == TOFIXED(270)) // LineArt 270
	lpDstFrame = Rotate90CCW1(lpSrcFrame);
else if (Angle == TOFIXED(90))		// 90
	lpDstFrame = Rotate90CW(lpSrcFrame);
else if (Angle == TOFIXED(180))		// 180
	lpDstFrame = Rotate180(lpSrcFrame);
else if (Angle == TOFIXED(270))		// 270
	lpDstFrame = Rotate90CCW(lpSrcFrame);
else
	{
	if (!lpBGColor)
		{
		Color.gray = 255;
		SetColorInfo(&Color, &Color, CS_GRAY);
		lpBGColor = &Color;
		}
	GetColorFromTypeInfo(TypeInfo, lpBGColor);
	if ( bSmartSize && CANDOTRANSPARENCY(TypeInfo.DataType))
		lpDstFrame = rotate_image_hq( lpSrcFrame, Angle, lpBGColor );
	else
		lpDstFrame = rotate_image_lq( lpSrcFrame, Angle, lpBGColor );
	}
return(lpDstFrame);
}

typedef struct _rotateframe_parms
	{
	LPFRAME lpFrame;
	LPFRAME lpSrcFrame;
	LFIXED sine;
	LFIXED cosine;
	LFIXED dyc; 
	LFIXED sxc; 
	LFIXED syc;
	LPLFIXED cosx_tbl;
	LPLFIXED sinx_tbl;
	long bgPixel;
	} ROTATEFRAME_PARMS, FAR *LPROTATEFRAME_PARMS;

/************************************************************************/
static LPFRAME rotate_image_hq( LPFRAME lpSrcFrame, LFIXED Angle,
							LPCOLORINFO lpBGColor )
/************************************************************************/
{
	LFIXED cosine, sine, sxc, syc, dxc, dyc;
	LFIXED rx[4], ry[4], minx,miny,maxx,maxy;
	int dwidth, dheight, j, x, pix, lin, bottom, right;
	LPLFIXED cosx_tbl = NULL;
	LPLFIXED sinx_tbl = NULL;
	LPFRAME lpFrame = NULL;
	ROTATEFRAME_PARMS parms;
	RECT rArea;
	BOOL fRet;

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
	sxc = FGET(right, 2);
	syc = FGET(bottom, 2);
	frotate_point(0, 0, sxc, syc, cosine, sine, &rx[0], &ry[0]);
	frotate_point(right, 0, sxc, syc, cosine, sine, &rx[1], &ry[1]);
	frotate_point(right, bottom, sxc, syc, cosine, sine, &rx[2], &ry[2]);
	frotate_point(0, lin - 1, sxc, syc, cosine, sine, &rx[3], &ry[3]);

	/**********************************************************/
	/* calculate size of new image from rotated corner points */
	/**********************************************************/
	minx = FGET(SHRT_MAX,1); miny = FGET(SHRT_MAX,1); maxy = 0; maxx = 0;
	for(j=0; j< 4; j++)
	{
		if ((rx[j]) < minx)	minx = (rx[j]);
		if ((rx[j]) > maxx)	maxx = (rx[j]);
		if ((ry[j]) < miny)	miny = (ry[j]);
		if ((ry[j]) > maxy)	maxy = (ry[j]);
	}
	dwidth = FMUL(1, maxx - minx + FUNITY);
	dheight = FMUL(1, maxy - miny + FUNITY);
	dxc = FGET(dwidth-1, 2);
	dyc = FGET(dheight-1, 2);

	/**************************************************/
	/* build cosx and sinx tables for faster rotation */
	/* sine is inverted to invert angle (360-angle)   */
	/* because we are rotating back to the source     */
	/**************************************************/
	cosx_tbl = (LPLFIXED)Alloc((long)sizeof(LFIXED) * (long)dwidth);
	sinx_tbl = (LPLFIXED)Alloc((long)sizeof(LFIXED) * (long)dwidth);
	if (!cosx_tbl || !sinx_tbl)
	{
		Message (IDS_EMEMALLOC);
		goto ExitError;
	}
	sine = -sine;
	for (x = 0; x < dwidth; ++x)
	{
		cosx_tbl[x] = FIXMUL(FGET(x, 1) - dxc, cosine);
		sinx_tbl[x] = FIXMUL(dxc - FGET(x, 1), sine);
	}

	/****************************/
	/* open frame for new image */
	/****************************/
	if ( !(lpFrame = FrameOpen( FrameTypeInfo(lpSrcFrame), dwidth, dheight,
     	FrameResolution(lpSrcFrame) )) )
	{
		FrameError( IDS_EPROCESSOPEN );
		goto ExitError;
	}

	/************************************************************/
	/* find out what a white pixel is for rotation off of image */
	/************************************************************/
	parms.bgPixel = GetFrameColor(lpBGColor, FrameTypeInfo(lpFrame));

	parms.lpFrame = lpFrame;
	parms.lpSrcFrame = lpSrcFrame;
	parms.sine = sine;
	parms.cosine = cosine;
	parms.dyc = dyc;
	parms.sxc = sxc;
	parms.syc = syc;
	parms.cosx_tbl = cosx_tbl;
	parms.sinx_tbl = sinx_tbl;

	rArea.top = rArea.left = 0;
	rArea.right = dwidth-1;
	rArea.bottom = dheight-1;

	if (FrameType(lpFrame) == FDT_LINEART)
		fRet = RotateFrameHQ(&parms, &rArea);
	else
		fRet = SplitThreads(0, RotateFrameHQ, NULL, NULL, (LPVOID)&parms, &rArea);


	/*******************************************************/
	/* now free up temp memory and set new size for caller */
	/*******************************************************/
	FreeUp((LPTR)cosx_tbl);
	FreeUp((LPTR)sinx_tbl);
	ProgressEnd();
	if (fRet)
		return(lpFrame);
	else
		{
		FrameClose(lpFrame);
		return(NULL);
		}

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
LOCAL BOOL RotateFrameHQ(LPVOID lpParam, LPRECT lpArea)
/************************************************************************/
{
int ys, ye, xs, xe;
int dheight, dwidth;
LFIXED cosine, sine, sxc, syc, dyc;
LFIXED sinyplusxc, ycminuscosy;
LFIXED xp, yp;
LFIXED fstop, fsleft, fsbottom, fsright;
int x, y, depth, right, bottom;
LPTR   lpDstData;
LPRGB  lpDstRGB;
LPCMYK lpDstCMYK;
long bgPixel;
BYTE   bgGray;
RGBS   bgRGB;
CMYKS  bgCMYK;
LPLFIXED cosx_tbl;
LPLFIXED sinx_tbl;
LPFRAME lpFrame, lpSrcFrame;
LPROTATEFRAME_PARMS lpParms;

lpParms = (LPROTATEFRAME_PARMS)lpParam;

/*******************************************************************/
/* rotate the sucker, line by line rotating each destination pixel */
/* back to the source image and averaging based on the areas       */
/* of the four pixels which the rotated pixel will land on	 */
/*******************************************************************/

ys = lpArea->top;
ye = lpArea->bottom;
xs = lpArea->left;
xe = lpArea->right;
dheight = ye - ys + 1;
dwidth = xe - xs + 1;

lpFrame = lpParms->lpFrame;
lpSrcFrame = lpParms->lpSrcFrame;
sine = lpParms->sine;
cosine = lpParms->cosine;
dyc = lpParms->dyc;
sxc = lpParms->sxc;
syc = lpParms->syc;
cosx_tbl = lpParms->cosx_tbl;
sinx_tbl = lpParms->sinx_tbl;
bgPixel = lpParms->bgPixel;

/************************************************************/
/* get source extents which will allow edges to be averaged */
/************************************************************/
right = FrameXSize(lpSrcFrame)-1;
bottom = FrameYSize(lpSrcFrame)-1;
fsright = FGET(FrameXSize(lpSrcFrame), 1);
fsbottom = FGET(FrameYSize(lpSrcFrame), 1);
fsleft = fstop = FGET(-1, 1);
depth = FrameDepth( lpFrame );
if (depth == 0) depth = 1;

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

		for (y = ys; y <= ye; ++y)
		{
			if (AstralClockCursor( y-ys, dheight, YES))
				return(FALSE);

			/*******************************/
			/* set frame to source image   */
			/* and destination line buffer */
			/******************************/
			lpDstData = FramePointer(lpFrame, xs, y, YES);

			if (!lpDstData) 
				continue;

			/***********************************************************/
			/* calculate sine*y and cos*y here so inner loop is faster */
			/***********************************************************/
			sinyplusxc = FIXMUL(dyc - MAKEFIXED(y), sine) + sxc;
			ycminuscosy = syc - FIXMUL(dyc - MAKEFIXED(y), cosine);

			/******************************************/
			/* the inner loop where it all happens    */
			/* this loop must be as tight as possible */
			/* or this routine will be a dog          */
			/******************************************/
			for (x = xs; x <= xe; ++x)
			{
				/*************************************/
				/* rotate destination back to source */
				/*************************************/
				xp = frotate_x(cosx_tbl[x], sinyplusxc);
				yp = frotate_y(sinx_tbl[x], ycminuscosy);

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

		for (y = ys; y <= ye; ++y)
		{
			if (AstralClockCursor( y-ys, dheight, YES))
				return(FALSE);

			/*******************************/
			/* set frame to source image   */
			/* and destination line buffer */
			/******************************/
			lpDstRGB = (LPRGB)FramePointerRaw( lpFrame, xs, y, TRUE );

			if (!lpDstRGB)
				break;

			/***********************************************************/
			/* calculate sine*y and cos*y here so inner loop is faster */
			/***********************************************************/
			sinyplusxc = FIXMUL(dyc - MAKEFIXED(y), sine) + sxc;
			ycminuscosy = syc - FIXMUL(dyc - MAKEFIXED(y), cosine);

			/******************************************/
			/* the inner loop where it all happens    */
			/* this loop must be as tight as possible */
			/* or this routine will be a dog	  */
			/******************************************/
			for (x = xs; x <= xe; ++x)
			{
				/*************************************/
				/* rotate destination back to source */
				/*************************************/
				xp = frotate_x(cosx_tbl[x], sinyplusxc);
				yp = frotate_y(sinx_tbl[x], ycminuscosy);

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

		for (y = ys; y <= ye; ++y)
		{
			if (AstralClockCursor( y-ys, dheight, YES))
				return(FALSE);

			/*******************************/
			/* set frame to source image   */
			/* and destination line buffer */
			/*******************************/
			lpDstCMYK = (LPCMYK)FramePointerRaw(lpFrame, xs, y, TRUE);

			if (!lpDstCMYK)
				break;

			/***********************************************************/
			/* calculate sine*y and cos*y here so inner loop is faster */
			/***********************************************************/
			sinyplusxc = FIXMUL(dyc - MAKEFIXED(y), sine) + sxc;
			ycminuscosy = syc - FIXMUL(dyc - MAKEFIXED(y), cosine);

			/******************************************/
			/* the inner loop where it all happens    */
			/* this loop must be as tight as possible */
			/* or this routine will be a dog          */
			/******************************************/
			for (x = xs; x <= xe; ++x)
			{
				/*************************************/
				/* rotate destination back to source */
				/*************************************/
				xp = frotate_x(cosx_tbl[x], sinyplusxc);
				yp = frotate_y(sinx_tbl[x], ycminuscosy);

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
return(TRUE);
}

/************************************************************************/
static LPFRAME rotate_image_lq( LPFRAME lpSrcFrame, LFIXED Angle,
								LPCOLORINFO lpBGColor )
/************************************************************************/
{
LFIXED cosine, sine, sxc, syc, dxc, dyc;
LFIXED rx[4], ry[4], minx,miny,maxx,maxy;
int dwidth, dheight, j, x, pix, lin, right, bottom;
LPLFIXED cosx_tbl = NULL;
LPLFIXED sinx_tbl = NULL;
LPFRAME lpFrame = NULL;
ROTATEFRAME_PARMS parms;
RECT rArea;
BOOL fRet;

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
sxc = FGET(right, 2);
syc = FGET(bottom, 2);
frotate_point(0, 0, sxc, syc, cosine, sine, &rx[0], &ry[0]);
frotate_point(right, 0, sxc, syc, cosine, sine, &rx[1], &ry[1]);
frotate_point(right, bottom, sxc, syc, cosine, sine, &rx[2], &ry[2]);
frotate_point(0, bottom, sxc, syc, cosine, sine, &rx[3], &ry[3]);

/**********************************************************/
/* calculate size of new image from rotated corner points */
/**********************************************************/
minx = FGET(SHRT_MAX, 1); miny = FGET(SHRT_MAX,1); maxy = 0; maxx = 0;
for(j=0; j< 4; j++)
	{
	if ((rx[j]) < minx)	minx = (rx[j]);
	if ((rx[j]) > maxx)	maxx = (rx[j]);
	if ((ry[j]) < miny)	miny = (ry[j]);
	if ((ry[j]) > maxy)	maxy = (ry[j]);
	}
dwidth = FMUL(1, maxx - minx + FUNITY);
dheight = FMUL(1, maxy - miny + FUNITY);
dxc = FGET(dwidth-1, 2);
dyc = FGET(dheight-1, 2);

/**************************************************/
/* build cosx and sinx tables for faster rotation */
/* sine is inverted to invert angle (360-angle)   */
/* because we are rotating back to the source     */
/**************************************************/
cosx_tbl = (LPLFIXED)Alloc((long)sizeof(LFIXED) * (long)dwidth);
sinx_tbl = (LPLFIXED)Alloc((long)sizeof(LFIXED) * (long)dwidth);
if (cosx_tbl == NULL || sinx_tbl == NULL)
	{
	Message (IDS_EMEMALLOC);
	goto ExitError;
	}
sine = -sine;
for (x = 0; x < dwidth; ++x)
	{
	cosx_tbl[x] = FIXMUL(FGET(x, 1) - dxc, cosine);
	sinx_tbl[x] = FIXMUL(dxc - FGET(x, 1), sine);
	}

/****************************/
/* open frame for new image */
/****************************/
if ( !(lpFrame = FrameOpen( FrameTypeInfo(lpSrcFrame), dwidth, dheight,
     FrameResolution(lpSrcFrame) )) )
	{
	FrameError( IDS_EPROCESSOPEN );
	goto ExitError;
	}

/************************************************************/
/* find out what a white pixel is for rotation off of image */
/************************************************************/
parms.bgPixel = GetFrameColor(lpBGColor, FrameTypeInfo(lpFrame));

parms.lpFrame = lpFrame;
parms.lpSrcFrame = lpSrcFrame;
parms.sine = sine;
parms.cosine = cosine;
parms.dyc = dyc;
parms.sxc = sxc;
parms.syc = syc;
parms.cosx_tbl = cosx_tbl;
parms.sinx_tbl = sinx_tbl;

rArea.top = rArea.left = 0;
rArea.right = dwidth-1;
rArea.bottom = dheight-1;

if (FrameType(lpFrame) == FDT_LINEART)
	fRet = RotateFrameLQ(&parms, &rArea);
else
	fRet = SplitThreads(0, RotateFrameLQ, NULL, NULL, (LPVOID)&parms, &rArea);

/*******************************************************/
/* now free up temp memory and set new size for caller */
/*******************************************************/
FreeUp((LPTR)cosx_tbl);
FreeUp((LPTR)sinx_tbl);
ProgressEnd();
if (fRet)
	return(lpFrame);
else
	{
	FrameClose(lpFrame);
	return(NULL);
	}

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
LOCAL BOOL RotateFrameLQ(LPVOID lpParam, LPRECT lpArea)
/************************************************************************/
{
int ys, ye, xs, xe;
int dheight, dwidth;
LFIXED cosine, sine, sxc, syc, dyc;
LFIXED sinyplusxc, ycminuscosy;
int xp, yp, x, y, depth, right, bottom;
LPTR   lpDstData, lpSrcData;
LPRGB  lpDstRGB, lpSrcRGB;
LPCMYK lpDstCMYK, lpSrcCMYK;
long bgPixel;
BYTE   bgGray;
RGBS   bgRGB;
CMYKS  bgCMYK;
LPLFIXED cosx_tbl;
LPLFIXED sinx_tbl;
LPFRAME lpFrame, lpSrcFrame;
LPROTATEFRAME_PARMS lpParms;

lpParms = (LPROTATEFRAME_PARMS)lpParam;

/*******************************************************************/
/* rotate the sucker, line by line rotating each destination pixel */
/* back to the source image and averaging based on the areas       */
/* of the four pixels which the rotated pixel will land on	 */
/*******************************************************************/

ys = lpArea->top;
ye = lpArea->bottom;
xs = lpArea->left;
xe = lpArea->right;
dheight = ye - ys + 1;
dwidth = xe - xs + 1;

lpFrame = lpParms->lpFrame;
lpSrcFrame = lpParms->lpSrcFrame;
sine = lpParms->sine;
cosine = lpParms->cosine;
dyc = lpParms->dyc;
sxc = lpParms->sxc;
syc = lpParms->syc;
cosx_tbl = lpParms->cosx_tbl;
sinx_tbl = lpParms->sinx_tbl;
bgPixel = lpParms->bgPixel;

right = FrameXSize(lpSrcFrame)-1;
bottom = FrameYSize(lpSrcFrame)-1;
depth = FrameDepth( lpFrame );
if (depth == 0) depth = 1;

switch(depth)
	{
	/********/
	case 1:		// Grayscale and line art
	/********/
	bgGray = bgPixel;
	for (y = ys; y <= ye; ++y)
		{
		if (AstralClockCursor( y-ys, dheight, YES))
			return(FALSE);
		
		/*******************************/
		/* set frame to source image   */
		/* and destination line buffer */
		/******************************/
		lpDstData = FramePointer(lpFrame, xs, y, YES);
		if (!lpDstData) 
			continue;

		/***********************************************************/
		/* calculate sine*y and cos*y here so inner loop is faster */
		/***********************************************************/
		sinyplusxc = FIXMUL(dyc - MAKEFIXED(y), sine) + sxc;
		ycminuscosy = syc - FIXMUL(dyc - MAKEFIXED(y), cosine);

		/******************************************/
		/* the inner loop where it all happens    */
		/* this loop must be as tight as possible */
		/* or this routine will be a dog	  */
		/******************************************/
		for (x = xs; x <= xe; ++x)
			{
			/*************************************/
			/* rotate destination back to source */
			/*************************************/
			xp = rotate_x(cosx_tbl[x], sinyplusxc);
			yp = rotate_y(sinx_tbl[x], ycminuscosy);

			if (xp < 0 || yp < 0 || xp > right || yp > bottom)
				/***************************************************/
				/* pixel is off of source image by a pixel or more */
				/***************************************************/
				{
				*lpDstData = bgGray;
				}
			else
				/************************************************************/
				/* averaging code - this is where most of the time is spent */
				/************************************************************/
				{
				lpSrcData = FramePointer( lpSrcFrame, xp, yp, NO, 1);
				*lpDstData = *lpSrcData;
				}
			lpDstData ++;
			}
		}
	break;

	/********/
	case 3 :	// 24-bit RGB
	/********/
	CopyRGB(  &bgPixel, &bgRGB );
	for (y = ys; y <= ye; ++y)
		{
		if (AstralClockCursor( y-ys, dheight, YES))
			return(FALSE);
		
		/*******************************/
		/* set frame to source image   */
		/* and destination line buffer */
		/******************************/
		lpDstRGB = (LPRGB)FramePointer(lpFrame, xs, y, YES);
		if (!lpDstRGB) 
			continue;

		/***********************************************************/
		/* calculate sine*y and cos*y here so inner loop is faster */
		/***********************************************************/
		sinyplusxc = FIXMUL(dyc - MAKEFIXED(y), sine) + sxc;
		ycminuscosy = syc - FIXMUL(dyc - MAKEFIXED(y), cosine);

		/******************************************/
		/* the inner loop where it all happens    */
		/* this loop must be as tight as possible */
		/* or this routine will be a dog	  */
		/******************************************/
		for (x = xs; x <= xe; ++x)
			{
			/*************************************/
			/* rotate destination back to source */
			/*************************************/
			xp = rotate_x(cosx_tbl[x], sinyplusxc);
			yp = rotate_y(sinx_tbl[x], ycminuscosy);
			if (xp < 0 || yp < 0 || xp > right || yp > bottom)
				/***************************************************/
				/* pixel is off of source image by a pixel or more */
				/***************************************************/
				{
				*lpDstRGB = bgRGB;
				}
			else
				/************************************************************/
				/* averaging code - this is where most of the time is spent */
				/************************************************************/
				{
				lpSrcRGB = (LPRGB)FramePointer( lpSrcFrame, xp, yp, NO, 1);
				*lpDstRGB = *lpSrcRGB;
				}
			lpDstRGB++;
			}
		}
	break;

	/********/
	case 4 :	// 32-bit CMYK
	/********/
	CopyCMYK( &bgPixel, &bgCMYK );
	for (y = ys; y <= ye; ++y)
		{
		if (AstralClockCursor( y-ys, dheight, YES))
			return(FALSE);
		
		/*******************************/
		/* set frame to source image   */
		/* and destination line buffer */
		/******************************/
		lpDstCMYK = (LPCMYK)FramePointer(lpFrame, xs, y, YES);
		if (!lpDstCMYK) 
			continue;

		/***********************************************************/
		/* calculate sine*y and cos*y here so inner loop is faster */
		/***********************************************************/
		sinyplusxc = FIXMUL(dyc - MAKEFIXED(y), sine) + sxc;
		ycminuscosy = syc - FIXMUL(dyc - MAKEFIXED(y), cosine);

		/******************************************/
		/* the inner loop where it all happens    */
		/* this loop must be as tight as possible */
		/* or this routine will be a dog	  */
		/******************************************/
		for (x = xs; x <= xe; ++x)
			{
			/*************************************/
			/* rotate destination back to source */
			/*************************************/
			xp = rotate_x(cosx_tbl[x], sinyplusxc);
			yp = rotate_y(sinx_tbl[x], ycminuscosy);

			if (xp < 0 || yp < 0 || xp > right || yp > bottom)
				/***************************************************/
				/* pixel is off of source image by a pixel or more */
				/***************************************************/
				{
				*lpDstCMYK = bgCMYK;
				}
			else
				/************************************************************/
				/* averaging code - this is where most of the time is spent */
				/************************************************************/
				{
				lpSrcCMYK = (LPCMYK)FramePointer( lpSrcFrame, xp, yp, NO, 1);
				*lpDstCMYK = *lpSrcCMYK;
				}
			lpDstCMYK ++;
			}
		}
	break;
	}

return(TRUE);
}

/************************************************************************/
static VOID frotate_point( int x, int y, LFIXED xc, LFIXED yc,
                              LFIXED cosine, LFIXED sine,
                              LPLFIXED rx, LPLFIXED ry )
/************************************************************************/
{
*rx = FIXMUL(FGET(x,1) - xc, cosine) + FIXMUL(yc - FGET(y,1), sine);
*ry = FIXMUL(xc - FGET(x,1), sine) + FIXMUL(yc - FGET(y,1), cosine);
}

/************************************************************************/

long average_pixel( LPFRAME lpFrame, LFIXED xp, LFIXED yp, int depth,
                     int top, int left, int bottom, int right,
                     BOOL offimage_average, COLOR bgPixel )
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

void average_pixel8( LPFRAME lpFrame, LFIXED xp, LFIXED yp, int top, int left,
                              int bottom, int right,
                              BOOL offimage_average, BYTE bgPixel,
                              LPTR lpNewPixel )
{
	LFIXED16    dx1, dy1, dy2, value, area1, area2;
   LFIXED      x1, y1;
	int         iPixLoc;
	LPTR        lpBuf1;
	LPTR        lpBuf2;
	register    int sx1, sy1;

	/****************************************/
	/* get x and y to left and top of pixel */
	/****************************************/ 
#ifdef USING_FIXED16
	x1 = WHOLEFIX(xp);
	y1 = WHOLEFIX(yp);
	sx1 = WHOLE(x1);
	sy1 = WHOLE(y1);
#else
	x1 = sx1 = WHOLE(xp);
	y1 = sy1 = WHOLE(yp);
#endif

	if (x1 == xp && y1 == yp)
	{
		/**********************************/ 
		/* landed right on top of a pixel */
		/**********************************/ 
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
		dx1 = GETFIXED16( get_delta(xp, x1) );
		dy1 = GETFIXED16( get_delta(yp, y1) );
		dy2 = FONE - dy1;

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
					lpBuf1, lpBuf2,
					area1, dy1 - area1, area2, dy2 - area2,
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

			*lpNewPixel = fix2int(value);
		}
		else
		{
			LFIXED16 total_onimage_area = 0;

			if (iPixLoc == (SX1ON | SX2ON | SY1ON | SY2ON))
			{
				lpBuf1 = FramePointer( lpFrame, sx1, sy1, NO);
				lpBuf2 = FramePointer( lpFrame, sx1, sy1+1, NO);

				quick_average8(
					lpBuf1, lpBuf2,
					area1, dy1 - area1, area2, dy2 - area2,
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

void average_pixel24( LPFRAME lpFrame, LFIXED xp, LFIXED yp, int top,
                        int left, int bottom, int right,
                        BOOL offimage_average, RGBS bgPixel,
                        LPRGB lpNewPixel )
{
	LFIXED16    dx1, dy1, dy2, area1, area2;
	LFIXED16    valueR, valueG, valueB;
   LFIXED      x1, y1;  
	LPTR        lpBuf1;
	LPTR        lpBuf2;
	int         iPixLoc;
	register int sx1, sy1;

	/****************************************/
	/* get x and y to left and top of pixel */
	/****************************************/ 
#ifdef USING_FIXED16
	x1 = WHOLEFIX(xp);
	y1 = WHOLEFIX(yp);
	sx1 = WHOLE(x1);
	sy1 = WHOLE(y1);
#else
	x1 = sx1 = WHOLE(xp);
	y1 = sy1 = WHOLE(yp);
#endif

	if (x1 == xp && y1 == yp)
	{
		/**********************************/ 
		/* landed right on top of a pixel */
		/**********************************/ 

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
		dx1 = GETFIXED16( get_delta(xp, x1) );
		dy1 = GETFIXED16( get_delta(yp, y1) );
		dy2 = FONE - dy1;

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
			lpNewPixel->red   = valueR >> 16;
			lpNewPixel->green = valueG >> 16;
			lpNewPixel->blue  = valueB >> 16;
		}
		else
		{
			LFIXED16 total_onimage_area = 0;

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

void average_pixel32( LPFRAME lpFrame, LFIXED xp, LFIXED yp, int top,
                        int left, int bottom, int right,
                        BOOL offimage_average, CMYKS bgPixel,
                        LPCMYK lpNewPixel )
{
	LFIXED16    dx1, dy1, dy2, area1, area2;
	LFIXED16    valueC, valueM, valueY, valueK;
   LFIXED      x1, y1;
	LPTR        lpBuf1;
	LPTR        lpBuf2;
	int         iPixLoc;
	register int sx1, sy1;

	/****************************************/
	/* get x and y to left and top of pixel */
	/****************************************/ 
#ifdef USING_FIXED16
	x1 = WHOLEFIX(xp);
	y1 = WHOLEFIX(yp);
	sx1 = WHOLE(x1);
	sy1 = WHOLE(y1);
#else
	x1 = sx1 = WHOLE(xp);
	y1 = sy1 = WHOLE(yp);
#endif

	if (x1 == xp && y1 == yp)
	{
		/**********************************/ 
		/* landed right on top of a pixel */
		/**********************************/ 

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
		dx1 = GETFIXED16( get_delta(xp, x1) );
		dy1 = GETFIXED16( get_delta(yp, y1) );
		dy2 = FONE - dy1;

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
			lpNewPixel->c = valueC >> 16;
			lpNewPixel->m = valueM >> 16;
			lpNewPixel->y = valueY >> 16;
			lpNewPixel->k = valueK >> 16;
		}
		else
		{
			LFIXED16 total_onimage_area = 0;

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
	FrameTypeInfo(lpSrcFrame), 
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
	FrameTypeInfo(lpSrcFrame), 
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
	FrameTypeInfo(lpSrcFrame),
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
static LPFRAME Rotate90CW1(LPFRAME lpSrcFrame)
/************************************************************************/
{
LPTR lp, lpSrc, lpStart, lpOut;
BYTE Src[8];
int Width, lines, nSlices, xSlice, i, y, dy, ysize;
LPFRAME lpFrame = NULL;
LPTR lpBuffer = NULL;

ProgressBegin(1,0);
lpFrame = FrameOpen(
	FrameTypeInfo(lpSrcFrame), 
	FrameYSize(lpSrcFrame),
	FrameXSize(lpSrcFrame),
	FrameResolution(lpSrcFrame));

if (!lpFrame)
	goto ExitError;

Width = FrameByteWidth(lpFrame);
lpBuffer = Alloc((long)Width*8L);
if (!lpBuffer)
	{
	Message (IDS_EMEMALLOC);
	goto ExitError;
	}

nSlices = FrameByteWidth(lpSrcFrame);
dy = 0;
ysize = FrameYSize(lpSrcFrame);
for (xSlice = 0; xSlice < nSlices; ++xSlice)
	{
	if (AstralClockCursor(xSlice, nSlices, YES))
		goto ExitError;
	lpStart = lpBuffer;
	lines = 0;
	lpSrc = (LPTR)Src + 7;
	clr(lpBuffer, Width*8);
	for (y = FrameYSize(lpSrcFrame)-1; y >= 0; --y)
		{
		lp = FramePointerRaw(lpSrcFrame, xSlice, y, NO);
		if (!lp)
			*lpSrc-- = 0;
		else
			*lpSrc-- = *lp;
		++lines;
		if (lines != 8 && y != 0)
			continue;
		RotateBitsCW((LPTR)Src, 8, lpStart, Width);
		++lpStart;
		lines = 0;
		lpSrc = (LPTR)Src + 7;
		}
	lpStart = lpBuffer;
	for (i = 0; i < 8; ++i, ++dy)
		{
		lpOut = FramePointerRaw(lpFrame, 0, dy, YES);
		if (lpOut)
			copy(lpStart, lpOut, Width);
		lpStart += Width;
		}
	}
FreeUp(lpBuffer);
ProgressEnd();
return(lpFrame);

ExitError:
if (lpBuffer)
	FreeUp(lpBuffer);
if (lpFrame)
	FrameClose(lpFrame);
ProgressEnd();
return(NULL);
}


/************************************************************************/
static LPFRAME Rotate90CCW1( LPFRAME lpSrcFrame)
/************************************************************************/
{
LPTR lp, lpSrc, lpStart, lpOut, lpLast;
BYTE Src[8];
int Width, lines, nSlices, xSlice, i, y, dy;
LPFRAME lpFrame = NULL;
LPTR lpBuffer = NULL;

ProgressBegin(1,0);
lpFrame = FrameOpen(
	FrameTypeInfo(lpSrcFrame),
	FrameYSize(lpSrcFrame),
	FrameXSize(lpSrcFrame),
	FrameResolution(lpSrcFrame));

if (!lpFrame)
	goto ExitError;
	
Width = FrameByteWidth(lpFrame);
lpBuffer = Alloc((long)Width*8L);
if (!lpBuffer)
	{
	Message (IDS_EMEMALLOC);
	goto ExitError;
	}
lpLast = lpBuffer + (7L * Width);
nSlices = FrameByteWidth(lpSrcFrame);
dy = FrameYSize(lpFrame)-1;
for (xSlice = 0; xSlice < nSlices; ++xSlice)
	{
	if (AstralClockCursor(xSlice, nSlices, YES))
		goto ExitError;
	lpStart = lpBuffer;
	lines = 0;
	lpSrc = (LPTR)Src;
	clr(lpBuffer, Width*8);
	for (y = 0; y < FrameYSize(lpSrcFrame); ++y)
		{
		lp = FramePointerRaw( lpSrcFrame, xSlice, y, NO);
		if (!lp)
			*lpSrc++ = 0;
		else
			*lpSrc++ = *lp;
		++lines;
		if (lines != 8 && y != (FrameYSize(lpSrcFrame)-1))
			continue;
		RotateBitsCCW((LPTR)Src, 8, lpStart, Width);
		lpStart += 1;
		lines = 0;
		lpSrc = (LPTR)Src;
		}
	lpStart = lpLast;
	for (i = 0; i < 8; ++i)
		{
		lpOut = FramePointerRaw( lpFrame, 0, dy, YES);
		if (lpOut)
			copy(lpStart, lpOut, Width);
		lpStart -= Width;
		--dy;
		if (dy < 0)
			break;
		}
	}
FreeUp(lpBuffer);
ProgressEnd();
return(lpFrame);

ExitError:
if (lpBuffer)
	FreeUp(lpBuffer);
if (lpFrame)
	FrameClose(lpFrame);
ProgressEnd();
return(NULL);
}

#ifdef SLOW_ROTATE
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
		FrameTypeInfo(lpSrcFrame), 
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
		FrameTypeInfo(lpSrcFrame),
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
#endif

/************************************************************************/

static void RotateBitsCW( LPTR lpSrc, int lines, LPTR lpStart, int Width )
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
	push	ds
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
	pop	ds
	}
#endif
}

/************************************************************************/
static void RotateBitsCCW( LPTR lpSrc, int lines, LPTR lpStart, int Width )
/************************************************************************/
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
	push	ds
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
	pop	ds
	}
#endif
}





