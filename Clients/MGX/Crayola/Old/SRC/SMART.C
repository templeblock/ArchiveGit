//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

typedef BOOL (*LPSMARTPROC) (LPTR, LPTR, LPINT);

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif	/* __cplusplus */

void pair2byte (LPLONG lpSum, LPLONG lpNum, LPTR lp, int iCount);

#ifdef __cplusplus
}            /* Assume C declarations for C++ */
#endif	/* __cplusplus */

// Static prototypes
static  int SmartSizeUp(LPTR lpIn,LPTR lpOut,int far *lpNeedsNewLine);
static  void DoSizeUp(LPTR lpIn,long peak,long far *lpLine);
static  int SmartSizeDown(LPTR lpIn,LPTR lpOut,int far *lpNeedsNewLine);
static  void DoSizeDown(LPTR lpIn,long peak,long far *lpLine,long far *lpNumber);
static  void fixed2byte(long far *lpSum,LPTR lp,int iCount);
static  void pair2byteRGB(LPLONG lpSum, LPLONG lpNum, LPTR lp, int iCount);
static  void DoSizeDownRGB(LPTR lpIn,long peak,long far *lpLine,long far *lpNumber);
static  int SmartSizeDownRGB(LPTR lpIn,LPTR lpOut,int far *lpNeedsNewLine);
static  void fixed2byteRGB(long far *lpSum,LPTR lp,int iCount);
static  void DoSizeUpRGB(LPTR lpIn,long peak,long far *lpLine);
static  int SmartSizeUpRGB(LPTR lpIn,LPTR lpOut,int far *lpNeedsNewLine);

static  void pair2byteCMYK(LPLONG lpSum, LPLONG lpNum, LPTR lp, int iCount);
static  void fixed2byteCMYK(LPLONG lpSum,LPTR lp,int iCount);
static  void DoSizeDownCMYK(   LPTR lpIn,long peak,long far *lpLine,long far *lpNumber);
static  int  SmartSizeDownCMYK(LPTR lpIn,LPTR lpOut,int far *lpNeedsNewLine);
static  void DoSizeUpCMYK(     LPTR lpIn,long peak,long far *lpLine);
static  int  SmartSizeUpCMYK(  LPTR lpIn,LPTR lpOut,int far *lpNeedsNewLine);

static LPTR SumLine0, SumLine1, NumLine0, NumLine1, lpLastInput;
static LFIXED yrate, yweight, yraterate, yrateorig, xrateseed;
static LPSMARTPROC lpSmartProc;
static int inWidth, outWidth, inHeight, outHeight;
static long outWidth4;

/************************************************************************/
LPFRAME DoSmartSize(LPFRAME lpSrcFrame, int pix, int lin, int res)
/************************************************************************/
{
int	yline, depth, srcpix, srclin;
LPTR	lpBuffer, lp;
int	sy;
LPFRAME	lpFrame;
BOOL	fNeedNewLine;

srcpix = FrameXSize(lpSrcFrame);
srclin = FrameYSize(lpSrcFrame);
yline = 0;

ProgressBegin(1,0);
if ( !(lpFrame = FrameOpen( FrameType(lpSrcFrame), pix, lin, res )) )
	{
	FrameError( IDS_EPROCESSOPEN );
	ProgressEnd();
	return( NULL );
	}

depth = FrameDepth(lpFrame);
if (!depth)
	depth = 1;
if ( !(AllocLines( &lpBuffer, 1, pix, depth )) )
	{
	FrameClose( lpFrame );
	Message( IDS_EMEMALLOC );
	ProgressEnd();
	return( NULL );
	}

if ( !SmartSetup( srcpix, pix, srclin, lin, depth ) )
	{
	FrameClose( lpFrame );
	FreeUp( lpBuffer );
	ProgressEnd();
	return( NULL );
	}

for ( sy=0; sy<lin; )
	{
	if (AstralClockCursor( sy, lin, YES ))
		goto ErrorExit;

	if ( !(lp = FramePointer( lpSrcFrame, 0, yline, NO)) )
		{
		FrameError(IDS_EFRAMEREAD);
		goto ErrorExit;
		}
	if ( SmartSize( lp, lpBuffer, &fNeedNewLine ) )
		{
		if ( !FrameWrite( lpFrame, 0, sy, pix, lpBuffer, pix) )
			{
			FrameError(IDS_EFRAMEREAD);
			goto ErrorExit;
			}
		sy++;
		}
	if ( fNeedNewLine )
		yline += 1;
	}

SmartDone();
FreeUp( lpBuffer );
ProgressEnd();
return( lpFrame );

ErrorExit:

SmartDone();
FrameClose( lpFrame );
FreeUp( lpBuffer );
ProgressEnd();
return( NULL );
}


/************************************************************************/
BOOL SmartSetup (
/************************************************************************/
int xIn,
int	xOut,
int	yIn,
int	yOut,
int	depth)
{
    inWidth    = xIn;
    outWidth   = xOut;
    outWidth4  = 4L * (outWidth+1);
    inHeight   = yIn;
    outHeight  = yOut;

/* filter width e.g., ++X++X++X++ in=11, out=3, width=5 */
/* a half filter is the distance between output pixels */
    if (inWidth > outWidth) {
	if (inHeight < outHeight) {
//	    Print ("Cannot distort with SmartSizing");
	    return (NO);
	}
	switch(depth)
	{
		case 1 : lpSmartProc = (LPSMARTPROC)SmartSizeDown; break;
		case 3 : lpSmartProc = (LPSMARTPROC)SmartSizeDownRGB; break;
		case 4 : lpSmartProc = (LPSMARTPROC)SmartSizeDownCMYK; break;
	}

	yrate = FGET (outHeight, inHeight);
	xrateseed = FGET (outWidth-1, inWidth);
    }
    else {
	if (inHeight > outHeight) {
//	    Print ("Cannot distort with SmartSizing");
	    return (NO);
	}

	switch(depth)
	{
		case 1 : lpSmartProc = (LPSMARTPROC)SmartSizeUp; break;
		case 3 : lpSmartProc = (LPSMARTPROC)SmartSizeUpRGB; break;
		case 4 : lpSmartProc = (LPSMARTPROC)SmartSizeUpCMYK; break;
	}

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
    SumLine0 = Alloc (outWidth4 * depth);
    NumLine0 = Alloc (outWidth4 * depth);
    SumLine1 = Alloc (outWidth4 * depth);
    NumLine1 = Alloc (outWidth4 * depth);
    if (!SumLine0 || !NumLine0 || !SumLine1 || !NumLine1) {
	FreeUp (SumLine0);
	FreeUp (NumLine0);
	FreeUp (SumLine1);
	FreeUp (NumLine1);
	return (NO);
    }
    lclr (SumLine0, outWidth4 * depth);
    lclr (NumLine0, outWidth4 * depth);
    lclr (SumLine1, outWidth4 * depth);
    lclr (NumLine1, outWidth4 * depth);
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
BOOL SmartSize (
/************************************************************************/
LPTR 	lpIn,
LPTR	lpOut,
LPINT 	lpNeedsNewLine)
{
    return ((*lpSmartProc) (lpIn, lpOut, lpNeedsNewLine));
}


/************************************************************************/
static BOOL SmartSizeUp (
/************************************************************************/
LPTR 	lpIn,
LPTR	lpOut,
LPINT 	lpNeedsNewLine)
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
static void DoSizeUp (
/************************************************************************/
LPTR 	lpIn,
LFIXED 	peak,
LPLONG 	lpLine) /* for storing intermediate pixel values */
{
int out;
LFIXED xweight, xrate;
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
static BOOL SmartSizeDown (
/************************************************************************/
LPTR 	lpIn,
LPTR	lpOut,
LPINT 	lpNeedsNewLine)
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
static void DoSizeDown (
/************************************************************************/
LPTR 	lpIn,
LFIXED 	peak,
LPLONG 	lpLine,
LPLONG	lpNumber) /* for storing intermediate pixel values */
{
LFIXED xweight, xrate;
#ifdef C_CODE
int in;
WORD word;
LONG p;
#endif

xrate = TMUL (xrateseed, peak);
xweight = peak;

#ifdef C_CODE
/* Run the loop over each input pixel */
in = inWidth;
while (--in >= 0)
	{
	p = *lpIn++;
	/* Accumulate the product of the pixels times the xweight */
	word = (long)xweight >> 8;
	*lpLine += (word * p);
	*lpNumber += word;
	word = (long)(peak-xweight) >> 8;
	* (lpLine+1) += (word * p);
	* (lpNumber+1) += word;

	/* If the filter's weight went negative */
	if ((xweight -= xrate) < 0)
		{
		xweight += peak; /* Increase the weight */
		lpLine++; lpNumber++; /* on to the next output pixel */
		}
	}
#else
__asm	{
	push	ds
	push	si
	push	di
	pushf
	mov	cx,inWidth
	or	cx,cx
	je	DSD_END
	lds	si,lpIn
	cld
DSD_1:
; p = *lpIn++
	lodsb			; p = *lpIn++
	sub	ah,ah		; cast to word
	push	ax		; save p
; word = (long)xweight >> 8;
	mov	bl,BYTE PTR xweight+1	; get HIBYTE of LOWORD 
	mov	bh,BYTE PTR xweight+2	; get LPBYTE of HIWORD
; *lpLine += (word * p);
	mul	bx
	les	di,lpLine
	add	WORD PTR es:[di],ax
	adc	WORD PTR es:[di+2],dx
; *lpNumber += word;
	les	di,lpNumber
	add	WORD PTR es:[di],bx
	adc	WORD PTR es:[di+2],0
; word = (peak-xweight) >> 8;
	mov	ax,WORD PTR peak		;peak
	mov	dx,WORD PTR peak+2
	sub	ax,WORD PTR xweight		;xweight
	sbb	dx,WORD PTR xweight+2
	mov	bl,ah
	mov	bh,dl
; * (lpNumber+1) += word;
	add	WORD PTR es:[di+4],bx
	adc	WORD PTR es:[di+6],0
; * (lpLine+1) += (word * p);
	les	di,lpLine
	add	di,4
	pop	ax
	mul	bx
	add	WORD PTR es:[di],ax
	adc	WORD PTR es:[di+2],dx
; if ((xweight -= xrate) < 0)
	mov	ax,WORD PTR xrate	;xrate
	mov	dx,WORD PTR xrate+2
	sub	WORD PTR xweight,ax	;xweight
	sbb	WORD PTR xweight+2,dx
	mov	dx,WORD PTR xweight+2
	or	dx,dx
	jge	DSD_2
; xweight += peak; /* Increase the weight */
	mov	ax,WORD PTR xweight	;xweight
	add	ax,WORD PTR peak	;peak
	adc	dx,WORD PTR peak+2
	mov	WORD PTR xweight,ax	;xweight
	mov	WORD PTR xweight+2,dx
; lpLine++; lpNumber++; /* on to the next output pixel */
	mov	WORD PTR lpLine,di		;lpLine
	add	WORD PTR lpNumber,4
DSD_2:
	loop	DSD_1

DSD_END:
	popf
	pop	di
	pop	si
	pop	ds
	}
#endif
}


#ifdef UNUSED
/************************************************************************/
static int DoSizeDown (
/************************************************************************/
LPTR 	lpIn,
int 	IsOdd)
{
int in, out;
LPLONG lpTemp;
LFIXED peak, altpeak, lTemp, weight, step; /* The step is +UNITY or -UNITY */
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

#ifdef C_CODE
/************************************************************************/
void pair2byte (
/************************************************************************/
LPLONG 	lpSum,
LPLONG	lpNum,
LPTR 	lp,
int 	iCount)
{
long l;

while (--iCount >= 0)
	{
	if (*lpNum)
		l = (*lpSum++) / (*lpNum++);
	else
		{
		l = 128;
		lpSum++;
		lpNum++;
		}
	*lp++ = (l > 255) ? 255 : (BYTE)l;
	}
}
#endif

/************************************************************************/
static void fixed2byte (
/************************************************************************/
LPLONG 	lpSum,
LPTR 	lp,
int 	iCount)
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
static void pair2byteRGB (
/************************************************************************/
LPLONG 	lpSum,
LPLONG	lpNum,
LPTR 	lp,
int 	iCount)
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
	tlptr = lp + 3 * i;
	frame_putRGB (tlptr, 3, &pixel);
    }
}


/************************************************************************/
static void DoSizeDownRGB (
/************************************************************************/
LPTR 	lpIn,
LFIXED 	peak,
LPLONG 	lpLine,
LPLONG	lpNumber)	 /* for storing intermediate pixel values */
{
    int in;
    LFIXED xweight, xrate;
    WORD word;

    RGBS pixel;
    LPTR tlptr;

    xrate = TMUL (xrateseed, peak);
    xweight = peak;

/* Run the loop over each input pixel */
    in = inWidth;
    for (in = 0; in < inWidth; in++) {
	tlptr = lpIn + 3 * in;
	pixel.red   = tlptr[0];
	pixel.green = tlptr[1];
	pixel.blue  = tlptr[2];
//	frame_getRGB (tlptr, 3, &pixel);

    /* Accumulate the product of the pixels times the xweight */
	word = (long)xweight >> 8;
	*lpLine += (word * pixel.red);
	*lpNumber += word;
	*(lpLine+1) += (word * pixel.green);
	*(lpNumber+1) += word;
	*(lpLine+2) += (word * pixel.blue);
	*(lpNumber+2) += word;

	word = (long)(peak-xweight) >> 8;
	*(lpLine+3) += (word * pixel.red);
	*(lpNumber+3) += word;
	*(lpLine+3+1) += (word * pixel.green);
	*(lpNumber+3+1) += word;
	*(lpLine+3+2) += (word * pixel.blue);
	*(lpNumber+3+2) += word;

    /* If the filter's weight went negative */
	if ((xweight -= xrate) < 0) {
	    xweight += peak; /* Increase the weight */
	/* on to the next output pixel */
	    lpLine += 3;
	    lpNumber += 3;
	}
    }
}


/************************************************************************/
static BOOL SmartSizeDownRGB (
/************************************************************************/
LPTR 	lpIn,
LPTR	lpOut,
LPINT 	lpNeedsNewLine)
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
    lclr (SumLine1, 3 * outWidth4);
    lclr (NumLine1, 3 * outWidth4);
    return (TRUE);
}


/************************************************************************/
static void fixed2byteRGB (
/************************************************************************/
LPLONG 	lpSum,
LPTR 	lp,
int 	iCount)
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
	frame_putRGB (lp, 3, &pixel);
	lp += 3;
    }
}


/************************************************************************/
static void DoSizeUpRGB (
/************************************************************************/
LPTR 	lpIn,
LFIXED 	peak,
LPLONG 	lpLine) /* for storing intermediate pixel values */
{
int out;
LFIXED xweight, xrate;
RGBS pel1, pel2;

    xrate = TMUL (xrateseed, peak);
#ifdef XQUADRATIC
    xrateorig = xrate;
    xraterate = 2*xrate;
#endif

    xweight = peak;

	 pel1.red   = lpIn[0];
	 pel1.green = lpIn[1];
	 pel1.blue  = lpIn[2];
//  frame_getRGB (lpIn, 3, &pel1);
    lpIn += 3;

	 pel2.red   = lpIn[0];
	 pel2.green = lpIn[1];
	 pel2.blue  = lpIn[2];
//  frame_getRGB (lpIn, 3, &pel2);
    lpIn += 3;

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

		pel2.red   = lpIn[0];
		pel2.green = lpIn[1];
		pel2.blue  = lpIn[2];
//	    frame_getRGB (lpIn, 3, &pel2);

	    lpIn += 3;
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
static BOOL SmartSizeUpRGB (
/************************************************************************/
LPTR 	lpIn,
LPTR	lpOut,
LPINT 	lpNeedsNewLine)
{
    /* if it was never set, start with the next line */
    if (!lpLastInput) {
	lpLastInput = NumLine0;
	copy (lpIn, lpLastInput, 3 * inWidth);
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
    lclr (SumLine0, 3 * outWidth4);

/* Setup y for the next line */
/* Advance the y weight & check it */
    if ((yweight -= yrate) < 0) {
	yweight += UNITY; /* y weight is always kept between 0 and UNITY */
	copy (lpIn, lpLastInput, 3 * inWidth);
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
static void pair2byteCMYK (
/************************************************************************/
LPLONG 	lpSum,
LPLONG	lpNum,
LPTR 	lp,
int 	iCount)
{
	long  l;
	int   i;
	CMYKS pixel;
	LPTR  tlptr;

	for (i = 0; i < iCount; i++)
	{
		/* cyan */
		if (*lpNum)
			l = (*lpSum++) / (*lpNum++);
		else
		{
			l = 128;
			lpSum++;
			lpNum++;
		}
		pixel.c = (l > 255) ? 255 : (BYTE)l;

		/* magenta */
		if (*lpNum)
			l = (*lpSum++) / (*lpNum++);
		else
		{
			l = 128;
			lpSum++;
			lpNum++;
		}
		pixel.m = (l > 255) ? 255 : (BYTE)l;

		/* yellow */
		if (*lpNum)
			l = (*lpSum++) / (*lpNum++);
		else
		{
			l = 128;
			lpSum++;
			lpNum++;
		}
		pixel.y = (l > 255) ? 255 : (BYTE)l;

		/* black */
		if (*lpNum)
			l = (*lpSum++) / (*lpNum++);
		else
		{
			l = 128;
			lpSum++;
			lpNum++;
		}
		pixel.k = (l > 255) ? 255 : (BYTE)l;

		/* put pixel into output buffer */
		tlptr = lp + 4 * i;

		tlptr[0] = pixel.c;
		tlptr[1] = pixel.m;
		tlptr[2] = pixel.y;
		tlptr[3] = pixel.k;
	}
}

/************************************************************************/
static void fixed2byteCMYK (
/************************************************************************/
LPLONG 	lpSum,
LPTR 	lp,
int 	iCount)
{
	long  l;
	int   i;
	WORD  w;
	CMYKS pixel;

	for (i = 0; i < iCount; i++)
	{
		/* cyan */
		l = (*lpSum++) + (UNITY/2);
		w = HIWORD (l);

		if (w > 255)
			w = 255;
		pixel.c = (BYTE)w;

		/* magenta */
		l = (*lpSum++) + (UNITY/2);
		w = HIWORD (l);

		if (w > 255)
			w = 255;
		pixel.m = (BYTE)w;

		/* yellow */
		l = (*lpSum++) + (UNITY/2);
		w = HIWORD (l);

		if (w > 255)
			w = 255;
		pixel.y = (BYTE)w;

		/* black */
		l = (*lpSum++) + (UNITY/2);
		w = HIWORD (l);

		if (w > 255)
			w = 255;
		pixel.k = (BYTE)w;

		/* put pixel into output buffer */
		lp[0] = pixel.c;
		lp[1] = pixel.m;
		lp[2] = pixel.y;
		lp[3] = pixel.k;
		lp += 4;
	}
}

/************************************************************************/
static void DoSizeDownCMYK (
/************************************************************************/
LPTR 	lpIn,
LFIXED 	peak,
LPLONG 	lpLine,
LPLONG	lpNumber)	 /* for storing intermediate pixel values */
{
	int in;
	LFIXED xweight, xrate;
	WORD word;

	CMYKS pixel;
	LPTR  tlptr;

	xrate = TMUL (xrateseed, peak);
	xweight = peak;

	/* Run the loop over each input pixel */
	in = inWidth;

	for (in = 0; in < inWidth; in++)
	{
		tlptr = lpIn + 4 * in;

		pixel.c = tlptr[0];
		pixel.m = tlptr[1];
		pixel.y = tlptr[2];
		pixel.k = tlptr[3];

		/* Accumulate the product of the pixels times the xweight */
		word = (long)xweight >> 8;
		*lpLine       += (word * pixel.c);
		*lpNumber     += word;
		*(lpLine  +1) += (word * pixel.m);
		*(lpNumber+1) += word;
		*(lpLine  +2) += (word * pixel.y);
		*(lpNumber+2) += word;
		*(lpLine  +3) += (word * pixel.k);
		*(lpNumber+3) += word;

		word = (long)(peak-xweight) >> 8;
		*(lpLine  +4) += (word * pixel.c);
		*(lpNumber+4) += word;
		*(lpLine  +5) += (word * pixel.m);
		*(lpNumber+5) += word;
		*(lpLine  +6) += (word * pixel.y);
		*(lpNumber+6) += word;
		*(lpLine  +7) += (word * pixel.k);
		*(lpNumber+7) += word;

		/* If the filter's weight went negative */
		if ((xweight -= xrate) < 0)
		{
		    xweight += peak; /* Increase the weight */
			/* on to the next output pixel */
		    lpLine += 4;
		    lpNumber += 4;
		}
	}
}


/************************************************************************/
static BOOL SmartSizeDownCMYK (
/************************************************************************/
LPTR 	lpIn,
LPTR	lpOut,
LPINT 	lpNeedsNewLine)
{
	LPTR lp;

	DoSizeDownCMYK (lpIn, yweight, (LPLONG)SumLine0, (LPLONG)NumLine0);
	DoSizeDownCMYK (lpIn, UNITY - yweight, (LPLONG)SumLine1, (LPLONG)NumLine1);

	*lpNeedsNewLine = YES; /* always need a new input line */

	/* Setup y for the next line */
	if ((yweight -= yrate) < 0) /* Advance the y weight & check it */
		yweight += UNITY; /* y weight is always kept between 0 and UNITY */
	else
		return (FALSE); /* don't have a line to output */

	/* An output line is ready */
	pair2byteCMYK ((LPLONG)SumLine0, (LPLONG)NumLine0, lpOut, outWidth);
	lp = SumLine0; SumLine0 = SumLine1; SumLine1 = lp;
	lp = NumLine0; NumLine0 = NumLine1; NumLine1 = lp;
	lclr (SumLine1, 4 * outWidth4);
	lclr (NumLine1, 4 * outWidth4);
	return (TRUE);
}

/************************************************************************/
static void DoSizeUpCMYK (
/************************************************************************/
LPTR 	lpIn,
LFIXED 	peak,
LPLONG 	lpLine) /* for storing intermediate pixel values */
{
	int out;
	LFIXED xweight, xrate;
	CMYKS pel1, pel2;

	xrate = TMUL (xrateseed, peak);
#ifdef XQUADRATIC
	xrateorig = xrate;
	xraterate = 2*xrate;
#endif

	xweight = peak;

	pel1.c = lpIn[0];
	pel1.m = lpIn[1];
	pel1.y = lpIn[2];
	pel1.k = lpIn[3];

	lpIn += 4;

	pel2.c = lpIn[0];
	pel2.m = lpIn[1];
	pel2.y = lpIn[2];
	pel2.k = lpIn[3];

	lpIn += 4;

	/* Run the loop over each output pixel */
	out = outWidth;
	while (--out >= 0)
	{
		/* Accumulate the product of the pixels times the weight */
		*lpLine += (xweight * pel1.c);
		*lpLine += ((peak-xweight) * pel2.c);
		lpLine++; /* on to the next output pixel */

		*lpLine += (xweight * pel1.m);
		*lpLine += ((peak-xweight) * pel2.m);
		lpLine++; /* on to the next output pixel */

		*lpLine += (xweight * pel1.y);
		*lpLine += ((peak-xweight) * pel2.y);
		lpLine++; /* on to the next output pixel */

		*lpLine += (xweight * pel1.k);
		*lpLine += ((peak-xweight) * pel2.k);
		lpLine++; /* on to the next output pixel */

		/* Decrement the x weight & check it */
		if ((xweight -= xrate) < 0)
		{
			xweight += peak; /* xweight is always kept between 0 and peak */
			pel1 = pel2; /* go on to the next input pixel */

			pel2.c = lpIn[0];
			pel2.m = lpIn[1];
			pel2.y = lpIn[2];
			pel2.k = lpIn[3];

			lpIn += 4;
#ifdef XQUADRATIC
			xrate = xrateorig;
#endif
		}
#ifdef XQUADRATIC
		else
		{
			xrate += xraterate;
		}
#endif
	}
}

/************************************************************************/
static BOOL SmartSizeUpCMYK (
/************************************************************************/
LPTR 	lpIn,
LPTR	lpOut,
LPINT 	lpNeedsNewLine)
{
	/* if it was never set, start with the next line */
	if (!lpLastInput)
	{
		lpLastInput = NumLine0;
		copy (lpIn, lpLastInput, 4 * inWidth);
	}

	DoSizeUpCMYK (lpLastInput, yweight, (LPLONG)SumLine0);
#ifdef QUADRATIC
	DoSizeUpCMYK (lpIn, UNITY- (yweight*2) + TMUL (yweight, yweight), 
			 (LPLONG)SumLine0);
#else
	DoSizeUpCMYK (lpIn, UNITY-yweight, (LPLONG)SumLine0);
#endif

	/* An output line is always ready */
	fixed2byteCMYK ((LPLONG)SumLine0, lpOut, outWidth);
	lclr (SumLine0, 4 * outWidth4);

	/* Setup y for the next line */
	/* Advance the y weight & check it */
	if ((yweight -= yrate) < 0)
	{
		yweight += UNITY; /* y weight is always kept between 0 and UNITY */
		copy (lpIn, lpLastInput, 4 * inWidth);
		*lpNeedsNewLine = YES; /* need a new input line */
#ifdef QUADRATIC
		yrate = yrateorig;
#endif
	}
	else
	{
		*lpNeedsNewLine = NO; /* don't need a new input line */
#ifdef QUADRATIC
		yrate += yraterate;
#endif
	}

	return (TRUE);
}
