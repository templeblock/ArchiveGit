//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"


typedef BOOL (_far *LPOK2FLOOD)(int direction, BOOL off, int x, int y, int min, int max, LPINT lpLeft, LPINT lpRight);

// Static prototypes
static void FloodObject(LPIMAGE lpImage, LPOBJECT lpObject, int x, int y, int Range, int opacity, LPCOLORINFO lpColor, MERGE_MODE MergeMode, int idColorModel);
static BOOL CreateFillBuffers(int outdepth);
static void FreeFillBuffers(void);
static void ProcessLine(int x, int y, int nPixel);
static void MagicMaskObject(LPIMAGE lpImage, LPOBJECT lpObject, int x, int y, int Range, int idColorModel, int iModes, int dirty);

static LPOK2FLOOD GetOk2FillProc(int x, int y, int Range,int idColorModel);

static BOOL CheckMap8(int direction, BOOL off, int x, int y, int min, int max, LPINT lpLeft, LPINT lpRight);
static BOOL CheckMap24RGB(int direction, BOOL off, int x, int y, int minx, int miny, LPINT lpLeft, LPINT lpRight);
static BOOL CheckMap24HL(int direction, BOOL off, int x, int y, int minx, int miny, LPINT lpLeft, LPINT lpRight);;
static BOOL CheckMap32RGB(int direction, BOOL off, int x, int y, int minx, int miny, LPINT lpLeft, LPINT lpRight);;
static BOOL CheckMap32HL(int direction, BOOL off, int x, int y, int minx, int miny, LPINT lpLeft, LPINT lpRight);;

static void FloodFillArea(int x, int y,  LPOK2FLOOD lpOk2Flood);
static void do_region_fill(int in_x,int in_y,int in_dir,int in_pxl,int in_pxr, LPOK2FLOOD lpOk2Flood);
static void SaveFloodParams(int where);
static void RestoreFloodParams(void);

#define ID_PROGRESS1 ID_BALL1
#define ID_PROGRESS2 ID_BALL2
#define ID_PROGRESS3 ID_BALL3
#define ID_PROGRESS4 ID_BALL4

#define MIN_STACK_SIZE 1024
#define MAX_HUES 252 /* must be divisible by 6 */
#define FIXHUE(hue) \
	((hue<0) ? hue+MAX_HUES:((hue>=MAX_HUES) ? hue-MAX_HUES: hue))

typedef struct _floodparms
{
	int	x;
	int	y;
	int	dir;
	int	pxl;
	int	pxr;
	int	xl;
	int	xr;
	BOOL bLine;
	int	where;
}
FLOODPARMS, _far *LPFLOODPARMS;

static LPFLOODPARMS lpParms;
static long MaxCallDepth;
static long CallDepth;
static int x, y, dir, pxl, pxr, xl, xr, ret, where;
static BOOL bLine;

static minval, maxval;		// for Decide8's
static RGBS sRGB;			// for Decide24's
static CMYKS sCMYK;		// for Decide32's
static HSLS sHSL;		// for some HL decides
static long maxdistance;	// for Decide24's and Decide32's

static LPTR lpColorBuf;		// buffer to hold solid color to be written
static LPTR lpMaskBuf;		// buffer to hold mask for area under map
static RECT rFlood;			// the area covered by the flood
static LPPIXMAP lpOutPix;	// the pixmap to write into
static int OutWidth, OutHeight; // the width and height of the output pixmap;
static LPFRAME lpInFrame; 	// the frame to use when deciding in\out	
static LPMASK lpMask;		// Mask to use to combine fill to image
static BOOL bUseColorShield;	// Use mload ?
static LPFRAME lpMapFrame;	// map of what has been filled
static LPTR lpMap;			// current line of the map
static POINT offset;		// offset of map from input frame
static POINT maskOffset;	// offset of mask from input frame
static POINT outOffset;		// offset of input frame from the output frame
static RECT MapRect;		// area IN MAP coors on which to work; 
static int Opacity;			// Opacity for fill
static LPPROCESSPROC lpProcessProc;	// Process proc for writing filled data

//************************************************************************
//	flood fills the selected object hit by x,y, based on its color at x,y.
// 	fills the with the given opacity and range, with the given color.
// 	x&y are in file coords.
//************************************************************************
void MgxFlood(LPIMAGE lpImage, int x, int y, int Range, int opacity, LPCOLORINFO lpColor, MERGE_MODE MergeMode, int idColorModel)
//************************************************************************
{
	LPOBJECT lpObject;
	RECT rMask;
	
	if ( !(lpObject = ImgFindSelObject(lpImage, x, y, NO)) )
		return;
	ProgressBegin(1,IDS_PROGFLOOD);
	FloodObject(lpImage,lpObject,x,y,Range,opacity,lpColor,MergeMode, idColorModel);
	if (RectWidth(&rFlood) > 0)
		{
		OffsetRect(&rFlood, offset.x+maskOffset.x, offset.y+maskOffset.y);
		ImgGetMaskRect(lpImage, &rMask);
		AstralIntersectRect(&rFlood, &rMask, &rFlood);
		UpdateImage(&rFlood, YES);
		}
	ProgressEnd();
}


//************************************************************************
// 	fills the object with the given opacity and range, with the given color.
// 	x&y are in file coords.
// 	x&y MUST be in the visable part of the object
//	Does caching.
//************************************************************************
static void FloodObject(LPIMAGE lpImage, LPOBJECT lpObject, int x, int y, int Range, int opacity, LPCOLORINFO lpColor, MERGE_MODE MergeMode, int idColorModel)
//************************************************************************
{
	int depth;
	RECT rChange;
	LPOK2FLOOD lpOk2Flood;
	
	// the Pixmap
	lpOutPix = &(lpObject->Pixmap);
	PixmapGetInfo(lpOutPix, PMT_EDIT,&OutWidth, &OutHeight,&depth,NULL);
	// the input frame and the output framne are the same
	outOffset.x = outOffset.y = 0;
	lpInFrame = PixmapFrame(lpOutPix,PMT_EDIT);
	// move to object coords
	maskOffset.x = lpObject->rObject.left;
	maskOffset.y = lpObject->rObject.top;
	x -= maskOffset.x;
	y -= maskOffset.y;
	// the mask
	lpMask = ImgGetMask(lpImage);
	bUseColorShield = TRUE;
	// 	the Process proc
	depth = depth ? depth:1;
	lpProcessProc = GetProcessProc(MergeMode, depth);
	Opacity = opacity;
	// setup buffers
	if (!CreateFillBuffers(depth))
		return;
	LoadColor(PixmapFrame(lpOutPix,PMT_EDIT), lpColorBuf, RectWidth(&MapRect),
		lpColor);
	// Get Ok2Flood proc
	if ( !(lpOk2Flood = GetOk2FillProc(x,y,Range, idColorModel)) )
		goto Exit;
	// Prepare undo frame
	if (!ImgEditInit(lpImage, ET_OBJECT, UT_DATA, lpObject))
		goto Exit;
	FloodFillArea(x-offset.x, y-offset.y, lpOk2Flood);
	// set undo
	rChange = rFlood;
	OffsetRect(&rChange, offset.x+maskOffset.x, offset.y+maskOffset.y);
	ImgEditedObject(lpImage, lpObject, IDS_UNDOCHANGE, &rChange);
Exit:
	FreeFillBuffers();
}


//************************************************************************/
//	Flood fills area.
//
//	The folowing must be set up before call:
//			lpOutFrame, lpMask, lpMapFrame, lpMap, offset,	
//			MapRect, Opacity, maskOffset, lpColorBuf, lpMaskBuf, 
//			lpProcessProc.
//										
//	On returns rFlood will be the area of the map frame which was flooded.
// 	x&y are in file co-ords of lpMapFrame.
//************************************************************************/
static void FloodFillArea(int x, int y, LPOK2FLOOD lpOk2Flood)
//************************************************************************/
{
	long lParms;
	
	MaxCallDepth = (65536L / (long)sizeof(FLOODPARMS)) - 1;
	lParms = MaxCallDepth * (long)sizeof(FLOODPARMS);
	if ( !(lpParms = (LPFLOODPARMS)Alloc(lParms)) )
		return;
	rFlood.top = rFlood.left = 0x7fff;
	rFlood.bottom = rFlood.right = -1;
	do_region_fill( x, y, 1, x, x, lpOk2Flood);
	FreeUp((LPTR)lpParms);
}



//************************************************************************
// All the region fill routines below use an iterative approach to
// accomplishing the flood fill because of the restricted amount of
// stack we have makes the recursive approach inappropriate, even
// when the algorithm for this type of seed fill is obviously recursive.
//
// The approach uses is as follows:  Memory is allocated for a buffer
// which is used to save parameters in the region fill routine,
// essentially taking the place of the stack.  The buffer is currently
// set to 64K which allows for the equivalent of 3276 recursive calls,
// with recursion all we could get was 453 calls.  This amount could be
// increased, but you'd have to use something like BumpPtr() to
// calculate the addresses for saving and restoring the parameters.
// This buffer is pointed to by 'lpParms'.  Another variable
// 'CallDepth' is used to keep track of how deep we are in the calling
// sequence, similar to how many times we've pushed parameters on the
// stack without returning.  It's like our stack pointer.  The other
// key variable is 'where'.  This variable is used to keep track of
// where we are in the function.  It's like our instruction pointer.
// 'where' can take on the following values:
//	0 - This means a new call to the edge fill routine.  This is
//		just like making a recursive call.
//	1 - This means we are returning to the first place a recursive
//		call can be made from.  'ret' points to the return value.
//	2 - This means we are returning to the second place a recursive
//		call can be made from.  'ret' points to the return value.
//	3 - This means we are returning to the third place a recursive
//		call can be made from.  'ret' points to the return value.
//************************************************************************

/************************************************************************/
static void do_region_fill(
/************************************************************************/
int 	in_x,
int		in_y,
int		in_dir,
int		in_pxl,
int		in_pxr,
LPOK2FLOOD lpOkToFlood)
{
	CallDepth = 1;
	where = 0;
	x = in_x;
	y = in_y;
	dir = in_dir;
	pxl = in_pxl;
	pxr = in_pxr;
	while (CallDepth > 0)
	{
	StartCall:
		if (where == 0)
		{
			if ( y < MapRect.top || y > MapRect.bottom || CallDepth >= MaxCallDepth )
			{
				ret = -1;
				goto DoReturn;
			}
			AstralBeachCursor(ID_PROGRESS1);
			/* scan to determine endpts of seed line seg */
			bLine = TRUE;
			/* scan to the left to determine endpts of seed line seg */
			(*lpOkToFlood)(0,YES,x,y,MapRect.left,MapRect.right, &xl, &xr);
			++xl;
			--xr;
			ProcessLine(xl, y, (xr-xl+1));
			if (xl < rFlood.left)
				rFlood.left = xl;
			if (xr > rFlood.right)
				rFlood.right = xr;
			if (y < rFlood.top)
				rFlood.top = y;
			if (y > rFlood.bottom)
				rFlood.bottom = y;
		}
		if (where == 0 || where == 1)
		{
			/* find and fill adjacent line segs in same direction */
			if (where == 0)
			{
				bLine = ((y+dir) >= MapRect.top && (y+dir) <= MapRect.bottom);
				x = xl;
			}
			else
			{
				x = ret;
				if (x >= MapRect.left)
					++x;
			}
			// search right for ok area
			if (bLine && x >= MapRect.left && x<=Min(MapRect.right,xr))
				if ((*lpOkToFlood)(1,NO,x,y+dir,0,Min(MapRect.right,xr), NULL, &x))
				{
					SaveFloodParams(1);
					++CallDepth;
					where = 0;
					y = y+dir;
					pxl = xl;
					pxr = xr;
					goto StartCall;
				}
			where = 0;
		}
		if (where == 0 || where == 2)
		{
			if (where == 0)
			{
				/* find and fill adjacent line segs in opposite direction */
				bLine = ((y-dir) >= MapRect.top && (y-dir) <= MapRect.bottom);
				x = xl;
			}
			else
			{
				x = ret;
				if (x >= MapRect.left)
					++x;
			}
			// search right for ok area
			if (bLine && x >= MapRect.left && x<=Min(MapRect.right, pxl))
				if ((*lpOkToFlood)(1,NO,x,y-dir,0,Min(MapRect.right, pxl), NULL, &x))
				{
					SaveFloodParams(2);
					++CallDepth;
					where = 0;
					y = y-dir;
					dir = -dir;
					pxl = xl;
					pxr = xr;
					goto StartCall;
				}
			where = 0;
		}
		if (where == 0 || where == 3)
		{
			if (where == 0)
			{
				bLine = ((y-dir) >= MapRect.top && (y-dir) <= MapRect.bottom);
				x = pxr;
			}
			else
			{
				x = ret;
				if (x >= MapRect.left)
					++x;
			}
			// search right for ok area
			if (bLine && x >= MapRect.left && x<=Min(MapRect.right, xr))
				if ((*lpOkToFlood)(1,NO,x,y-dir,0,Min(MapRect.right, xr), NULL, &x))
				{
					SaveFloodParams(3);
					++CallDepth;
					where = 0;
					y = y-dir;
					dir = -dir;
					pxl = xl;
					pxr = xr;
					goto StartCall;
				}
			where = 0;
		}
		ret = xr;
DoReturn:
		--CallDepth;
		RestoreFloodParams();
	}	// end while
}



//************************************************************************
//	Fills the Given lpDst buffer with the fill color for nPixels
//	Takes Opacity and the mask into account.
// 	Updates the Map to exclude these points in the future.
// 	Note: x,y are in map coords.
//************************************************************************
static void ProcessLine(int x, int y, int nPixel)
//************************************************************************
{
	LPTR lpDst;
	LPTR lpInData;
	int depth, xin, yin;
	
	// set corresponding map bits  (must be done before bounding)
	lpMap = FramePointer(lpMapFrame, 0, y, YES);
	lpMap += x;
	set(lpMap, nPixel, 255);
	// check bounds
	yin = y+offset.y+outOffset.y;
	if (!WITHIN(yin, 0, OutHeight-1))
		return;
	xin = x+offset.x+outOffset.x;
	if (xin >= OutWidth)
		return;
	if (xin <0)
	{
		x -= xin;
		if (x >= MapRect.right)
			return;
		nPixel += xin;
		if (nPixel<=0)
			return;
		xin = 0;
	}
	if (xin+nPixel>OutWidth)
		nPixel = OutWidth-xin;
	// 	Get destination line 
	lpDst = PixmapPtr(lpOutPix,PMT_EDIT,xin, yin, YES);
	// get mask
	if (bUseColorShield)
	{
		// get source data for color shields
		depth = FrameDepth(lpInFrame);
		depth = depth ? depth:1;
		lpInData = FramePointer(lpInFrame, x+offset.x, y+offset.y, NO);
		mload(	xin,
				yin,
				nPixel,
				1, 
				lpMaskBuf, 
				lpInData, 
				depth, 
				lpMask);
	}	
	else if (lpMask)
		MaskLoad(lpMask, xin, yin, nPixel, lpMaskBuf);
	// add opacity
	if ((lpMask || bUseColorShield) && Opacity < 255)
		ScaleData8(lpMaskBuf, nPixel, (BYTE)Opacity);
	// put solid color into line (with mask)
	(*lpProcessProc)(lpDst, lpColorBuf, lpMaskBuf, nPixel);
}

//************************************************************************
// 	Returns appropriate function.
//	Sets up all globals for that function.
// 	Based on lpInFrame.
//	Returns NULL for failure.
//************************************************************************
static LPOK2FLOOD GetOk2FillProc(int x, int y, int Range, int idColorModel)
//************************************************************************
{
	int depth;
	LPTR lpLine;
	int delta;
	int val;
	
	depth = FrameDepth(lpInFrame);
	if ( !(lpLine = FramePointer(lpInFrame, x, y, NO)) )
		return(NULL);
	delta = TOGRAY(Range);
	
	switch (depth)
	{
		case 0:
		case 1:
			// setup globals
			val = *(LPTR)lpLine;
			minval = bound(val - delta, 0, 255 );
			maxval = bound(val + delta, 0, 255 );
			// return function
			return(&CheckMap8);
		break;
		case 3:
			// setup globals
			sRGB = *(LPRGB)lpLine;
			maxdistance = (long)delta*(long)delta;
			// return function
			switch (idColorModel+IDC_FIRST_MODEL)
			{
				case IDC_MODEL_RGB:
					maxdistance*=3;
					return(&CheckMap24RGB);
				break;
				case IDC_MODEL_HL:
					RGBtoHSL(sRGB.red,sRGB.green,sRGB.blue, &sHSL);
//					maxdistance*=2;
					return(&CheckMap24HL);
				break;
				default:
					maxdistance*=3;
					return(&CheckMap24RGB);
			}
		break;
		case 4:
			// setup globals
			sCMYK = *(LPCMYK)lpLine;
			maxdistance = (long)delta*(long)delta;
			// return function
			switch (idColorModel+IDC_FIRST_MODEL)
			{
				case IDC_MODEL_RGB:
					maxdistance*=4;
					return(&CheckMap32RGB);
				break;
				case IDC_MODEL_HL:
					CMYKtoHSL(sCMYK.c,sCMYK.m,sCMYK.y,sCMYK.k,&sHSL);
//					maxdistance*=2;
					return(&CheckMap32HL);
				break;
				default:
					maxdistance*=4;
					return(&CheckMap32RGB);
			}
		break;
		default:
			return(NULL);
	}
	return(NULL);
}


#define CHECKMAP(DATACHECK) \
{ \
	if (direction == 1 || !direction) \
	{ \
		pos = x + (direction == 0); \
		lpdata = lpData+pos+offset.x; \
		lpmap = lpMap+pos; \
		pos--; \
		while(++pos<=max) \
		{ \
			if (*lpmap++) \
				if (off) \
					break; \
				else \
				{ \
					lpdata++; \
					continue; \
				} \
			DATACHECK \
			lpdata++; \
		} \
		*lpRight = pos; \
		if (pos <= max) \
			bRet = TRUE; \
	} \
	if (direction == -1 || !direction)	 \
	{ \
		pos = x - (direction == 0); \
		lpdata = lpData+pos+offset.x; \
		lpmap = lpMap+pos; \
		pos++; \
		while(--pos>=min) \
		{ \
			if (*lpmap--) \
				if (off) \
					break; \
				else \
				{ \
					lpdata--; \
					continue; \
				} \
			DATACHECK \
			lpdata--; \
		} \
		*lpLeft = pos; \
		if (pos >= min) \
			bRet = TRUE; \
	} \
}



//************************************************************************
//	Use when lpInFrame is depth==8
// 	Checks the map to see if the location given is ok for filling.
//	Based on value of the lpMapFrame, and the value of lpInFrame;
//	Be sure x&y are in the map!
//************************************************************************
static BOOL CheckMap8(int direction, BOOL off, int x, int y, int min, int max, LPINT lpLeft, LPINT lpRight)
//************************************************************************
{
	LPTR lpMap, lpmap;
	LPTR lpData, lpdata;
	BOOL bRet = FALSE, bInRange;
	int pos;
	
	lpMap = FramePointer(lpMapFrame, 0, y, NO);
	lpData = FramePointer(lpInFrame, 0, y+offset.y, NO);
	CHECKMAP( if ( (*lpdata != 0) != off ) break; );
	CHECKMAP( bInRange = ( *lpdata != 0 );
		if ( bInRange != off ) break; );
	return(bRet);
}

//************************************************************************
//	direction = 1 for right, -1 for left, 0 for both.
// 	searches from the specified point to the edge or limits.
//	the values returned in lpLeft, lpRight are for the first pixels which match the off bool.
//	return TRUE if an edge was hit (as opposed to hitting the limits.)
//	doesnt check the x point if direction == 0;
//************************************************************************
static BOOL CheckMap24RGB(int direction, BOOL off, int x, int y, int min, int max, LPINT lpLeft, LPINT lpRight)
//************************************************************************
{
	LPTR lpMap, lpmap;
	LPRGB lpData, lpdata;
	BOOL bRet = FALSE, bInRange;
	int pos;

	lpMap = FramePointer(lpMapFrame, 0, y, NO);
	lpData = (LPRGB)FramePointer(lpInFrame, 0, y+offset.y, NO);
	CHECKMAP( bInRange = ( (lpdata->red + lpdata->green + lpdata->blue) != 0 );
		if ( bInRange != off ) break; );
	return(bRet);
}


//************************************************************************
static BOOL CheckMap24HL(int direction, BOOL off, int x, int y, int min, int max, LPINT lpLeft, LPINT lpRight)
//************************************************************************
{

	LPTR lpMap, lpmap;
	LPRGB lpData, lpdata;
	BOOL bRet = FALSE, bInRange;
	int pos;
	
	lpMap = FramePointer(lpMapFrame, 0, y, NO);
	lpData = (LPRGB)FramePointer(lpInFrame, 0, y+offset.y, NO);
	CHECKMAP( bInRange = ( lpdata->red == sRGB.red &&
		lpdata->green == sRGB.green && lpdata->blue == sRGB.blue );
		if ( bInRange != off ) break; );
	return(bRet);
}

//************************************************************************
static BOOL CheckMap32RGB(int direction, BOOL off, int x, int y, int min, int max, LPINT lpLeft, LPINT lpRight)
//************************************************************************
{
	LPTR lpMap, lpmap;
	LPCMYK lpData, lpdata;
	BOOL bRet = FALSE;
	int pos;
	long Error, tError;
	
	lpMap = FramePointer(lpMapFrame, 0, y, NO);
	// it it already ruled out?
	lpData = (LPCMYK)FramePointer(lpInFrame, 0, y+offset.y, NO);
	CHECKMAP(	{
				tError = (int)lpdata->c - (int)sCMYK.c;
			  	tError *= tError;	
				Error = (int)lpdata->m - (int)sCMYK.m;
			  	tError += Error*Error;	
				Error = (int)lpdata->y - (int)sCMYK.y;
			  	tError += Error*Error;	
				Error = (int)lpdata->k - (int)sCMYK.k;
				if(((Error*Error+tError) > maxdistance) == off) break;
				})
	return(bRet);
}

//************************************************************************
static BOOL CheckMap32HL(int direction, BOOL off, int x, int y, int min, int max, LPINT lpLeft, LPINT lpRight)
//************************************************************************
{
	LPTR lpMap, lpmap;
	LPCMYK lpData, lpdata;
	BOOL bRet = FALSE;
	int pos;
	long Error1, Error2;
	HSLS hsl;
	
	
	lpMap = FramePointer(lpMapFrame, 0, y, NO);
	// it it already ruled out?
	lpData = (LPCMYK)FramePointer(lpInFrame, 0, y+offset.y, NO);
	CHECKMAP({
				CMYKtoHL(lpdata,&hsl);
				Error1 = (int)hsl.hue - (int)sHSL.hue;
				Error2 = -Error1;
				// get into range [0,360) degrees
				Error1 = FIXHUE(Error1);
				Error2 = FIXHUE(Error2);
				Error1 = Min(Error1, Error2);
				Error1 *= Error1;
				Error2 = (int)hsl.lum - (int)sHSL.lum;
				Error2 *= Error2;
				if (((Error1+Error2) > maxdistance) == off) break;
			})
	return(bRet);
}

//************************************************************************
//	Sets buffers needed to fill, based on lpInFrame:
//		lpMapFrame, lpMaskFrame, lpColorBuf. 
//	Also sets offset & MapRect.
// 	outdepth should be the depth of the output frame.
//************************************************************************
static BOOL CreateFillBuffers(int outdepth)
//***********************************************************************
{
	FreeFillBuffers();
	if (!lpInFrame)
		return(FALSE);
	// MapRect can be made smaller in the future
	MapRect.left = MapRect.top = 0;
	MapRect.right = FrameXSize(lpInFrame)-1;
	MapRect.bottom = FrameYSize(lpInFrame)-1;
	offset.x = offset.y = 0;

	// setup lpMapFrame	
//	lpMapFrame = FrameOpen(FDT_LINEART, RectWidth(&MapRect), RectHeight(&MapRect), 1); 
	lpMapFrame = FrameOpen(FDT_GRAYSCALE, RectWidth(&MapRect), RectHeight(&MapRect), 1); 
	if (!lpMapFrame)
		return(FALSE);
	FrameSetBackground(lpMapFrame, 0);
	// setup lpMaskBuf
	lpMaskBuf = Alloc((long)FrameXSize(lpMapFrame));
	if (!lpMaskBuf)
		goto Exit;
	// setup color buffer 
	lpColorBuf = Alloc((long)FrameXSize(lpMapFrame)*outdepth);
	if (!lpColorBuf)
		goto Exit;
	return(TRUE);
Exit:
	FreeFillBuffers();
	return(FALSE);
}

//************************************************************************
//	Frees buffers needed to fill, based on lpInFrame:
//		lpMapFrame, lpMaskFrame, lpColorBuf. 
//************************************************************************
static void FreeFillBuffers()
//***********************************************************************
{
	if (lpMapFrame)
		FrameClose(lpMapFrame);
	lpMapFrame = NULL;
	if (lpMaskBuf)
		FreeUp(lpMaskBuf);
	lpMaskBuf = NULL;
	if (lpColorBuf)
		FreeUp(lpColorBuf);
	lpColorBuf = NULL;
}

/************************************************************************/
static void SaveFloodParams(int where)
/************************************************************************/
{
lpParms[CallDepth].x = x;
lpParms[CallDepth].y = y;
lpParms[CallDepth].dir = dir;
lpParms[CallDepth].pxl = pxl;
lpParms[CallDepth].pxr = pxr;
lpParms[CallDepth].xl = xl;
lpParms[CallDepth].xr = xr;
lpParms[CallDepth].bLine = bLine;
lpParms[CallDepth].where = where;
}

/************************************************************************/
static void RestoreFloodParams()
/************************************************************************/
{
x = lpParms[CallDepth].x;
y = lpParms[CallDepth].y;
dir = lpParms[CallDepth].dir;
pxl = lpParms[CallDepth].pxl;
pxr = lpParms[CallDepth].pxr;
xl = lpParms[CallDepth].xl;
xr = lpParms[CallDepth].xr;
bLine = lpParms[CallDepth].bLine;
where = lpParms[CallDepth].where;
}


//************************************************************************
// 	Does a magic mask.  
//	Flood fills into the mask channel based on the selected object hit by x,y. 
// 	Range is the range around the color at x,y to use.
// 	x&y are in file coords.
//************************************************************************
void MgxMagicMask(LPIMAGE lpImage, int x, int y, int Range, int idColorModel, int iMode,  int dirty)
//************************************************************************
{
	LPOBJECT lpObject;
	
	lpObject = ImgFindObject(lpImage, x, y, NO, NO);
	if (!lpObject)
		return;
	ProgressBegin(1,NULL);
	MagicMaskObject(lpImage,lpObject,x,y,Range,idColorModel, iMode, dirty);
	ProgressEnd();
}

//************************************************************************
//	Flood fills into the mask channel based on the given object and its
//		color at x,y. 
// 	x&y are in file coords of base image.
// 	x&y should be in the visable part of the object
//	Does caching.
//	Note::: MapRect could be set better. (ie objects off image etc.)
//************************************************************************
static void MagicMaskObject(LPIMAGE lpImage, LPOBJECT lpObject, int x, int y, int Range, int idColorModel, int iMode, int dirty)
//************************************************************************
{
	int outdepth;
	RECT rUpdate, rDummy;
	LPOK2FLOOD lpOk2Flood;
	LPPIXMAP lpInPixmap;
	BOOL fNewmask;

	// get or create mask channel	
	lpMask = ImgGetMaskEx(lpImage,iMode==SHAPE_SUBTRACT, Control.NoUndo,
						 &fNewmask);
	if (!lpMask)
		return;
	// the input frame comes from the object
	lpInPixmap = &(lpObject->Pixmap);
	lpInFrame = PixmapFrame(lpInPixmap,PMT_EDIT);
	// get the masks Pixmap to output on.
	lpOutPix = &(lpMask->Pixmap);
	PixmapGetInfo(lpOutPix, PMT_EDIT,&OutWidth, &OutHeight,&outdepth,NULL);
	outdepth = outdepth ? outdepth:1;
	outOffset.x = lpObject->rObject.left;
	outOffset.y = lpObject->rObject.top;
	// move to object coords
	maskOffset.x = lpObject->rObject.left;
	maskOffset.y = lpObject->rObject.top;
	x -= maskOffset.x;
	y -= maskOffset.y;
	// no masking
	lpMask = NULL;
	bUseColorShield = NO;
	// 	the Process proc
	switch (iMode)
	{
		case SHAPE_SUBTRACT:
			lpProcessProc = GetProcessProc(MM_DIFFERENCE, outdepth);
		break;
		case SHAPE_REVERSE:
		case SHAPE_ADD:
		default:
			lpProcessProc = GetProcessProc(MM_NORMAL, outdepth);
		break;
	}
	Opacity = 255;
	// setup buffers
	if (!CreateFillBuffers(outdepth))
		return;
	set(lpMaskBuf, RectWidth(&MapRect), 255);
	set(lpColorBuf, outdepth*RectWidth(&MapRect), 255); 
	// Get Ok2Flood proc
	lpOk2Flood = GetOk2FillProc(x,y,Range, idColorModel);
	if (!lpOk2Flood)
		goto Exit;
	// Prepare undo frame
	if ( dirty )
		if (!ImgEditInit(lpImage, ET_OBJECT, fNewmask ? UT_CREATEMASK : UT_ALPHA,
			 ImgGetBase(lpImage)))
			goto Exit;
	EnableMarquee(NO);
	RemoveMaskMarquee(lpImage);
	FloodFillArea(x-offset.x, y-offset.y, lpOk2Flood);
	// set undo
	rUpdate = rFlood;
	OffsetRect(&rUpdate, offset.x+outOffset.x, offset.y+outOffset.y);
	if ( dirty )
		ImgEditedObject(lpImage, ImgGetBase(lpImage), dirty,
			fNewmask ? (LPRECT)NULL : (LPRECT)&rUpdate);
	lpMask = ImgGetMask(lpImage);
	if (iMode!=SHAPE_ADD)
		MaskRectUpdate(lpMask, &rDummy);
	else
		AstralUnionRect(&lpMask->rMask, &lpMask->rMask, &rUpdate);
	if (ImgGetMaskUpdateRect(lpImage, fNewmask, NO, &rUpdate))
	{
		File2DispRect(&rUpdate, &rUpdate);
		++rUpdate.right; ++rUpdate.bottom;
		InvalidateRect(lpImage->hWnd, &rUpdate, FALSE);
	}
	EnableMarquee(YES);
Exit:
	FreeFillBuffers();
}
