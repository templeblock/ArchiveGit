//®FD1¯®PL1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

/************************************************************************/
long Measure(
/************************************************************************/
LPRECT 		lpRect,
LPCOLORINFO lpColorInfo,
BOOL 		fFileCoords)
{
LPTR lpLine, lpDataBuf;
int y;
long csum, msum, ysum, ksum;
long rsum, gsum, bsum, count;
LPFRAME lpFrame;
RECT	iRect;
int iCount, iWidth, depth;

if (!lpImage)
	return(0L);
if (!fFileCoords)
	{
	Display2File( (LPINT)&lpRect->left,  (LPINT)&lpRect->top );
	Display2File( (LPINT)&lpRect->right, (LPINT)&lpRect->bottom );
	}
lpFrame = ImgGetBaseEditFrame(lpImage);
iRect.left = iRect.top = 0;
iRect.right  = FrameXSize(lpFrame)-1;
iRect.bottom = FrameYSize(lpFrame)-1;

if (!AstralIntersectRect(lpRect, lpRect, &iRect))
	return(0L);

iWidth = RectWidth(lpRect);
if (!iWidth)
	return(0L);
depth = FrameDepth(lpFrame);
if (!depth) depth = 1;
if (!(lpDataBuf = Alloc((long)iWidth*depth)))
	{
	Message(IDS_EMEMALLOC);
	return(0L);
	}

csum = msum = ysum = ksum = 0;
rsum = gsum = bsum = 0;
count = 0;

switch(FrameType(lpFrame))
{
	case FDT_LINEART:
	case FDT_GRAYSCALE:
	case FDT_PALETTECOLOR:
		for ( y = lpRect->top; y <= lpRect->bottom; y++ )
		{
			if ( !ImgGetLine(lpImage, NULL, lpRect->left, y, iWidth, lpDataBuf))
				continue;
			lpLine = lpDataBuf;
			iCount = (lpRect->right - lpRect->left) + 1;
			count += iCount;

			while( iCount-- > 0)
			{
				ksum += *lpLine++;
			}
		}

		if (count)
		{
			lpColorInfo->gray = (ksum / count);
		}
		else
		{
			lpColorInfo->gray = 255;
		}

		SetColorInfo( lpColorInfo, lpColorInfo, CS_GRAY );
	break;

	case FDT_RGBCOLOR:
		for ( y = lpRect->top; y <= lpRect->bottom; y++ )
		{
			if ( !ImgGetLine(lpImage, NULL, lpRect->left, y, iWidth,
							lpDataBuf))
				continue;
			lpLine = lpDataBuf;

			iCount = (lpRect->right - lpRect->left) + 1;
			count += iCount;

			while( iCount-- > 0)
			{
				rsum += *lpLine++;
				gsum += *lpLine++;
				bsum += *lpLine++;
			}
		}

		if (count)
		{
			lpColorInfo->rgb.red   = (rsum / count);
			lpColorInfo->rgb.green = (gsum / count);
			lpColorInfo->rgb.blue  = (bsum / count);
		}
		else
		{
			lpColorInfo->rgb.red   = 255;
			lpColorInfo->rgb.green = 255;
			lpColorInfo->rgb.blue  = 255;
		}

		SetColorInfo( lpColorInfo, lpColorInfo, CS_RGB );
	break;

	case FDT_CMYKCOLOR:
		for ( y = lpRect->top; y <= lpRect->bottom; y++ )
		{
			if ( !ImgGetLine(lpImage, NULL, lpRect->left, y, iWidth,
							lpDataBuf))
				continue;
			lpLine = lpDataBuf;

			iCount = (lpRect->right - lpRect->left) + 1;
			count += iCount;

			while( iCount-- > 0)
			{
				csum += *lpLine++;
				msum += *lpLine++;
				ysum += *lpLine++;
				ksum += *lpLine++;
			}
		}

		if (count)
		{
			lpColorInfo->cmyk.c = (csum / count);
			lpColorInfo->cmyk.m = (msum / count);
			lpColorInfo->cmyk.y = (ysum / count);
			lpColorInfo->cmyk.k = (ksum / count);
		}
		else
		{
			lpColorInfo->cmyk.c = 0;
			lpColorInfo->cmyk.m = 0;
			lpColorInfo->cmyk.y = 0;
			lpColorInfo->cmyk.k = 0;
		}

		SetColorInfo( lpColorInfo, lpColorInfo, CS_CMYK );
	break;
}
FreeUp(lpDataBuf);
return( count );
}

/************************************************************************/
long MeasureRGB(
/************************************************************************/
LPRECT lpRect,
LPRGB  lpRGB,
BOOL   fFileCoords)
{
	long lReturn;
	COLORINFO ColorInfo;

	lReturn = Measure( lpRect, &ColorInfo, YES );

	*lpRGB = ColorInfo.rgb;

	return(lReturn);
}
