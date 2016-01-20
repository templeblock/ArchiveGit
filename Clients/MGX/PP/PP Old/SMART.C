// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
//®PL1¯®FD1¯®TP0¯®BT0¯
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"

static LPTR SumLine0, SumLine1, NumLine0, NumLine1, lpLastInput;
static FIXED yrate, yweight, yraterate, yrateorig, xrateseed;
static LPROC lpSmartProc;
static int inWidth, outWidth, inHeight, outHeight;
static long outWidth4;
static int smartBPP, smartDepth;

/************************************************************************/
BOOL SmartSetup (xIn, xOut, yIn, yOut, depth)
/************************************************************************/
int xIn, xOut, yIn, yOut, depth;
{
    inWidth = xIn;
    outWidth = xOut;
    outWidth4 = 4L * (outWidth+1);
    inHeight = yIn;
    outHeight = yOut;
    smartDepth = depth;
    smartBPP = (depth == 1) ? 1 : 3;

/* filter width e.g., ++X++X++X++ in=11, out=3, width=5 */
/* a half filter is the distance between output pixels */
    if (inWidth > outWidth) {
	if (inHeight < outHeight) {
	    Print ("Can not both Shrink and Expand");
	    return (NO);
	}
	lpSmartProc = (depth == 1) ? SmartSizeDown : SmartSizeDownRGB;
	yrate = FGET (outHeight, inHeight);
	xrateseed = FGET (outWidth-1, inWidth);
    }
    else {
	if (inHeight > outHeight) {
	    Print ("Can not both Shrink and Expand");
	    return (NO);
	}
	lpSmartProc = (depth == 1) ? SmartSizeUp : SmartSizeUpRGB;
	yrate = FGET (inHeight, outHeight);
	xrateseed = FGET (inWidth-1, outWidth);
#ifdef QUADRATIC
	yrate = TMUL (yrate, yrate);
	yrateorig = yrate;
	yraterate = 2*yrate;
#endif
#ifdef XQUADRATIC
	xrateseed = TMUL (xrateseed, xrateseed);
#endif
    }

    yweight = UNITY;
    lpLastInput = NULL;

    SumLine0 = SumLine1 = NULL;
    NumLine0 = NumLine1 = NULL;
    SumLine0 = Alloc (outWidth4 * smartBPP);
    NumLine0 = Alloc (outWidth4 * smartBPP);
    SumLine1 = Alloc (outWidth4 * smartBPP);
    NumLine1 = Alloc (outWidth4 * smartBPP);
    if (!SumLine0 || !NumLine0 || !SumLine1 || !NumLine1) {
	FreeUp (SumLine0);
	FreeUp (NumLine0);
	FreeUp (SumLine1);
	FreeUp (NumLine1);
	return (NO);
    }
    lclr (SumLine0, outWidth4 * smartBPP);
    lclr (NumLine0, outWidth4 * smartBPP);
    lclr (SumLine1, outWidth4 * smartBPP);
    lclr (NumLine1, outWidth4 * smartBPP);
    return (YES);
}


/************************************************************************/
void SmartDone ()
/************************************************************************/
{
    FreeUp (SumLine0);
    FreeUp (NumLine0);
    FreeUp (SumLine1);
    FreeUp (NumLine1);
}


/************************************************************************/
BOOL SmartSize (lpIn, lpOut, lpNeedsNewLine)
/************************************************************************/
LPTR lpIn, lpOut;
LPINT lpNeedsNewLine;
{
    return ((*lpSmartProc) (lpIn, lpOut, lpNeedsNewLine));
}


/************************************************************************/
static BOOL SmartSizeUp (lpIn, lpOut, lpNeedsNewLine)
/************************************************************************/
LPTR lpIn, lpOut;
LPINT lpNeedsNewLine;
{
/* if it was never set, start with the next line */
    if (!lpLastInput) {
	lpLastInput = NumLine0;
	copy (lpIn, lpLastInput, inWidth);
    }

    DoSizeUp (lpLastInput, yweight, (LPLONG)SumLine0);
#ifdef QUADRATIC
    DoSizeUp (lpIn, UNITY- (yweight*2) + TMUL (yweight, yweight), (LPLONG)SumLine0);
#else
    DoSizeUp (lpIn, UNITY-yweight, (LPLONG)SumLine0);
#endif

/* An output line is always ready */
    fixed2byte ((LPLONG)SumLine0, lpOut, outWidth);
    lclr (SumLine0, outWidth4);

/* Setup y for the next line */
/* Advance the y weight & check it */
    if ((yweight -= yrate) < 0) {
	yweight += UNITY; /* y weight is always kept between 0 and UNITY */
	copy (lpIn, lpLastInput, inWidth);
	*lpNeedsNewLine = YES; /* need a new input line */
#ifdef QUADRATIC
	    yrate = yrateorig;
#endif
    }
    else {
	*lpNeedsNewLine = NO; /* don't need a new input line */
#ifdef QUADRATIC
	yrate += yraterate;
#endif
    }

    return (TRUE);
}


/************************************************************************/
static void DoSizeUp (lpIn, peak, lpLine)
/************************************************************************/
LPTR lpIn;
FIXED peak;
LPLONG lpLine; /* for storing intermediate pixel values */
{
    int out;
    FIXED xweight, xrate, xraterate, xrateorig;
    BYTE p1, p2;

    xrate = TMUL (xrateseed, peak);
#ifdef XQUADRATIC
    xrateorig = xrate;
    xraterate = 2*xrate;
#endif

    xweight = peak;

    p1 = *lpIn++;
    p2 = *lpIn++;

/* Run the loop over each output pixel */
    out = outWidth;
    while (--out >= 0) {
	/* Accumulate the product of the pixels times the weight */
	*lpLine += (xweight * p1);
	*lpLine += ((peak-xweight) * p2);
	lpLine++; /* on to the next output pixel */

    /* Decrement the x weight & check it */
	if ((xweight -= xrate) < 0) {
	    xweight += peak; /* xweight is always kept between 0 and peak */
	    p1 = p2; /* go on to the next input pixel */
	    p2 = *lpIn++;
#ifdef XQUADRATIC
	    xrate = xrateorig;
#endif
	}
#ifdef XQUADRATIC
	else	xrate += xraterate;
#endif
    }
}


/************************************************************************/
static BOOL SmartSizeDown (lpIn, lpOut, lpNeedsNewLine)
/************************************************************************/
LPTR lpIn, lpOut;
LPINT lpNeedsNewLine;
{
    LPTR lp;

    DoSizeDown (lpIn, yweight, (LPLONG)SumLine0, (LPLONG)NumLine0);
    DoSizeDown (lpIn, UNITY - yweight, (LPLONG)SumLine1, (LPLONG)NumLine1);

    *lpNeedsNewLine = YES; /* always need a new input line */

/* Setup y for the next line */
    if ((yweight -= yrate) < 0) /* Advance the y weight & check it */
	yweight += UNITY; /* y weight is always kept between 0 and UNITY */
    else
	return (FALSE); /* don't have a line to output */

/* An output line is ready */
    pair2byte ((LPLONG)SumLine0, (LPLONG)NumLine0, lpOut, outWidth);
    lp = SumLine0; SumLine0 = SumLine1; SumLine1 = lp;
    lp = NumLine0; NumLine0 = NumLine1; NumLine1 = lp;
    lclr (SumLine1, outWidth4);
    lclr (NumLine1, outWidth4);
    return (TRUE);
}


/************************************************************************/
static void DoSizeDown (lpIn, peak, lpLine, lpNumber)
/************************************************************************/
LPTR lpIn;
FIXED peak;
LPLONG lpLine, lpNumber; /* for storing intermediate pixel values */
{
    int in;
    FIXED xweight, xrate;
    BYTE p;
    WORD word;

xrate = TMUL (xrateseed, peak);
xweight = peak;

/* Run the loop over each input pixel */
    in = inWidth;
    while (--in >= 0) {
	p = *lpIn++;
    /* Accumulate the product of the pixels times the xweight */
	word = xweight >> 8;
	*lpLine += (word * p);
	*lpNumber += word;
	word = (peak-xweight) >> 8;
	* (lpLine+1) += (word * p);
	* (lpNumber+1) += word;

    /* If the filter's weight went negative */
	if ((xweight -= xrate) < 0) {
	    xweight += peak; /* Increase the weight */
	    lpLine++; lpNumber++; /* on to the next output pixel */
	}
    }
}


#ifdef UNUSED
/************************************************************************/
static int DoSizeDown (lpIn, IsOdd)
/************************************************************************/
LPTR lpIn;
int IsOdd;
{
int in, out;
LPLONG lpTemp;
FIXED peak, altpeak, lTemp, weight, step; /* The step is +UNITY or -UNITY */
BYTE pixel;
long l;
LPLONG lline, altline; /* for storing intermediate pixel values */
LPLONG number, altnumber; /* for storing intermediate pixel counts */

if (IsOdd)
	{
	peak = y;
	altpeak = iFilterHalf - y;
	lline = (LPLONG)Line1;
	altline = (LPLONG)Line0;
	number = (LPLONG)Line3;
	altnumber = (LPLONG)Line2;
	}
else	{
	peak = iFilterHalf - y;
	altpeak = y;
	lline = (LPLONG)Line0;
	altline = (LPLONG)Line1;
	number = (LPLONG)Line2;
	altnumber = (LPLONG)Line3;
	}

/* Whenever the weight goes negative, */
/* switch to the other filter, as well as to the other line accumulator */
weight = peak;
step = -UNITY;

out = outWidth;
in = inWidth;

/* Run the loop over each input pixel */
while (--in >= 0)
	{
	pixel = *lpIn++;
	/* Accumulate the product of the pixel times the weight */
	l = weight >> 8;
	*lline += (l * pixel);
	*number += l;

	weight += step;
	CheckIt:
	if (weight > peak)
		{ /* If we went past the filter's peak weight... */
		/* Start decreasing the weight */
		weight = peak - (weight - peak);
		step = -step;
		}
	if (weight < 0)
		{ /* If the filter's weight went negative */
		/* Start increasing the weight */
		weight = -weight;
		step = -step;
		/* Switch to the other filter */
		lTemp = peak; peak = altpeak; altpeak = lTemp;
		lpTemp = lline; lline = altline; altline = lpTemp;
		lpTemp = number; number = altnumber; altnumber = lpTemp;
		lline++; number++; /* on to the next output pixel */
		altline++; altnumber++; /* skip ahead on the alternate line */
		--out;
		goto CheckIt;
		}
	}
return (out);
}
#endif


/************************************************************************/
static void pair2byte (lpSum, lpNum, lp, iCount)
/************************************************************************/
LPLONG lpSum, lpNum;
LPTR lp;
int iCount;
{
    long l;

    while (--iCount >= 0) {
	if (*lpNum)
	    l = (*lpSum++) / (*lpNum++);
	else {
	    l = 128;
	    lpSum++;
	    lpNum++;
	}
	*lp++ = (l > 255) ? 255 : (BYTE)l;
    }
}


/************************************************************************/
static void fixed2byte (lpSum, lp, iCount)
/************************************************************************/
LPLONG lpSum;
LPTR lp;
int iCount;
{
    long l;
    WORD w;;

    while (--iCount >= 0) {
	l =  (*lpSum++) + (UNITY/2);
	w = HIWORD (l);
	*lp++ = (w > 255) ? 255 : (BYTE)w;
    }
}


/************************************************************************/
static void pair2byteRGB (lpSum, lpNum, lp, iCount)
/************************************************************************/
LPLONG lpSum, lpNum;
LPTR lp;
int iCount;
{
    long	l;
    int		i;
    RGBS	pixel;
    LPTR	tlptr;

    for (i = 0; i < iCount; i++) {
    /* red */
	if (*lpNum)
	    l = (*lpSum++) / (*lpNum++);
	else {
	    l = 128;
	    lpSum++;
	    lpNum++;
	}
	pixel.red = (l > 255) ? 255 : (BYTE)l;

    /* green */
	if (*lpNum)
	    l = (*lpSum++) / (*lpNum++);
	else {
	    l = 128;
	    lpSum++;
	    lpNum++;
	}
	pixel.green = (l > 255) ? 255 : (BYTE)l;

    /* blue */
	if (*lpNum)
	    l = (*lpSum++) / (*lpNum++);
	else {
	    l = 128;
	    lpSum++;
	    lpNum++;
	}
	pixel.blue = (l > 255) ? 255 : (BYTE)l;

    /* put pixel into output buffer */
	tlptr = lp + smartDepth * i;
	frame_putRGB (tlptr, &pixel);
    }
}


/************************************************************************/
static void DoSizeDownRGB (lpIn, peak, lpLine, lpNumber)
/************************************************************************/
LPTR lpIn;
FIXED peak;
LPLONG lpLine, lpNumber; /* for storing intermediate pixel values */
{
    int in;
    FIXED xweight, xrate;
    WORD word;

    RGBS pixel;
    LPTR tlptr;

    xrate = TMUL (xrateseed, peak);
    xweight = peak;

/* Run the loop over each input pixel */
    in = inWidth;
    for (in = 0; in < inWidth; in++) {
	tlptr = lpIn + smartDepth * in;
	frame_getRGB (tlptr, &pixel);

    /* Accumulate the product of the pixels times the xweight */
	word = xweight >> 8;
	*lpLine += (word * pixel.red);
	*lpNumber += word;
	*(lpLine+1) += (word * pixel.green);
	*(lpNumber+1) += word;
	*(lpLine+2) += (word * pixel.blue);
	*(lpNumber+2) += word;

	word = (peak-xweight) >> 8;
	*(lpLine+smartBPP) += (word * pixel.red);
	*(lpNumber+smartBPP) += word;
	*(lpLine+smartBPP+1) += (word * pixel.green);
	*(lpNumber+smartBPP+1) += word;
	*(lpLine+smartBPP+2) += (word * pixel.blue);
	*(lpNumber+smartBPP+2) += word;

    /* If the filter's weight went negative */
	if ((xweight -= xrate) < 0) {
	    xweight += peak; /* Increase the weight */
	/* on to the next output pixel */
	    lpLine += smartBPP;
	    lpNumber += smartBPP;
	}
    }
}


/************************************************************************/
static BOOL SmartSizeDownRGB (lpIn, lpOut, lpNeedsNewLine)
/************************************************************************/
LPTR lpIn, lpOut;
LPINT lpNeedsNewLine;
{
    LPTR lp;

    DoSizeDownRGB (lpIn, yweight, (LPLONG)SumLine0, (LPLONG)NumLine0);
    DoSizeDownRGB (lpIn, UNITY - yweight, (LPLONG)SumLine1, (LPLONG)NumLine1);

    *lpNeedsNewLine = YES; /* always need a new input line */

/* Setup y for the next line */
    if ((yweight -= yrate) < 0) /* Advance the y weight & check it */
	yweight += UNITY; /* y weight is always kept between 0 and UNITY */
    else
	return (FALSE); /* don't have a line to output */

/* An output line is ready */
    pair2byteRGB ((LPLONG)SumLine0, (LPLONG)NumLine0, lpOut, outWidth);
    lp = SumLine0; SumLine0 = SumLine1; SumLine1 = lp;
    lp = NumLine0; NumLine0 = NumLine1; NumLine1 = lp;
    lclr (SumLine1, smartBPP * outWidth4);
    lclr (NumLine1, smartBPP * outWidth4);
    return (TRUE);
}


/************************************************************************/
static void fixed2byteRGB (lpSum, lp, iCount)
/************************************************************************/
LPLONG lpSum;
LPTR lp;
int iCount;
{
    long	l;
    int		i;
    WORD	w;
    RGBS	pixel;

    for (i = 0; i < iCount; i++) {

    /* red */
	l =  (*lpSum++) + (UNITY/2);
	w = HIWORD (l);
	if (w > 255)
	    w = 255;
	pixel.red = (BYTE)w;

    /* green */
	l =  (*lpSum++) + (UNITY/2);
	w = HIWORD (l);
	if (w > 255)
	    w = 255;
	pixel.green = (BYTE)w;

    /* blue */
	l =  (*lpSum++) + (UNITY/2);
	w = HIWORD (l);
	if (w > 255)
	    w = 255;
	pixel.blue = (BYTE)w;

    /* put pixel into output buffer */
	frame_putRGB (lp, &pixel);
	lp += smartDepth;
    }
}


/************************************************************************/
static void DoSizeUpRGB (lpIn, peak, lpLine)
/************************************************************************/
LPTR lpIn;
FIXED peak;
LPLONG lpLine; /* for storing intermediate pixel values */
{
    int out;
    FIXED xweight, xrate, xraterate, xrateorig;
    RGBS pel1, pel2;

    xrate = TMUL (xrateseed, peak);
#ifdef XQUADRATIC
    xrateorig = xrate;
    xraterate = 2*xrate;
#endif

    xweight = peak;

    frame_getRGB (lpIn, &pel1);
    lpIn += smartDepth;
    frame_getRGB (lpIn, &pel2);
    lpIn += smartDepth;

    /* Run the loop over each output pixel */
    out = outWidth;
    while (--out >= 0) {
    /* Accumulate the product of the pixels times the weight */
	*lpLine += (xweight * pel1.red);
	*lpLine += ((peak-xweight) * pel2.red);
	lpLine++; /* on to the next output pixel */

	*lpLine += (xweight * pel1.green);
	*lpLine += ((peak-xweight) * pel2.green);
	lpLine++; /* on to the next output pixel */

	*lpLine += (xweight * pel1.blue);
	*lpLine += ((peak-xweight) * pel2.blue);
	lpLine++; /* on to the next output pixel */

	/* Decrement the x weight & check it */
	if ((xweight -= xrate) < 0) {
	    xweight += peak; /* xweight is always kept between 0 and peak */
	    pel1 = pel2; /* go on to the next input pixel */
	    frame_getRGB (lpIn, &pel2);
	    lpIn += smartDepth;
#ifdef XQUADRATIC
	    xrate = xrateorig;
#endif
	}
#ifdef XQUADRATIC
	else	xrate += xraterate;
#endif
    }
}


/************************************************************************/
static BOOL SmartSizeUpRGB (lpIn, lpOut, lpNeedsNewLine)
/************************************************************************/
LPTR lpIn, lpOut;
LPINT lpNeedsNewLine;
{
    /* if it was never set, start with the next line */
    if (!lpLastInput) {
	lpLastInput = NumLine0;
	copy (lpIn, lpLastInput, smartBPP * inWidth);
    }

    DoSizeUpRGB (lpLastInput, yweight, (LPLONG)SumLine0);
#ifdef QUADRATIC
    DoSizeUpRGB (lpIn, UNITY- (yweight*2) + TMUL (yweight, yweight), 
			 (LPLONG)SumLine0);
#else
    DoSizeUpRGB (lpIn, UNITY-yweight, (LPLONG)SumLine0);
#endif

/* An output line is always ready */
    fixed2byteRGB ((LPLONG)SumLine0, lpOut, outWidth);
    lclr (SumLine0, smartBPP * outWidth4);

/* Setup y for the next line */
/* Advance the y weight & check it */
    if ((yweight -= yrate) < 0) {
	yweight += UNITY; /* y weight is always kept between 0 and UNITY */
	copy (lpIn, lpLastInput, smartBPP * inWidth);
	*lpNeedsNewLine = YES; /* need a new input line */
#ifdef QUADRATIC
	yrate = yrateorig;
#endif
    }
    else {
	*lpNeedsNewLine = NO; /* don't need a new input line */
#ifdef QUADRATIC
	yrate += yraterate;
#endif
    }

    return (TRUE);
}
