//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

#define C_CODE

typedef struct _smart
{
	LPTR     SumLine0;
	LPTR     SumLine1;
	LPTR     NumLine0;
	LPTR     NumLine1;
	LPTR     lpLastInput;
	LFIXED16 yrate;
	LFIXED16 yweight;
	LFIXED16 yraterate;
	LFIXED16 yrateorig;
	LFIXED16 xrateseed;
	int (FAR *lpSmartProc)(struct _smart FAR *lpsm, LPTR lpIn, LPTR lpOut, int far *lpNeedsNewLine);
	int      inWidth;
	int      outWidth;
	int      inHeight;
	int      outHeight;
	long     outWidth4;
} SMART, FAR * LPSMART;

// Static prototypes
LOCAL  void DoSizeUp (LPSMART lpsm, LPTR lpIn,long peak,long far *lpLine);
LOCAL  void DoSizeDown	(LPSMART lpsm, LPTR lpIn,long peak,long far *lpLine,long far *lpNumber);
LOCAL  int SmartSizeUp		(LPSMART lpsm, LPTR lpIn,LPTR lpOut,int far *lpNeedsNewLine);
LOCAL  int SmartSizeDown	(LPSMART lpsm, LPTR lpIn,LPTR lpOut,int far *lpNeedsNewLine);
LOCAL  void fixed2byte(long far *lpSum,LPTR lp,int iCount);

LOCAL  void DoSizeDownRGB(LPSMART lpsm, LPTR lpIn,long peak,long far *lpLine,long far *lpNumber);
LOCAL  void DoSizeUpRGB	(LPSMART lpsm, LPTR lpIn,long peak,long far *lpLine);
LOCAL  int SmartSizeUpRGB	(LPSMART lpsm, LPTR lpIn,LPTR lpOut,int far *lpNeedsNewLine);
LOCAL  int SmartSizeDownRGB(LPSMART lpsm, LPTR lpIn,LPTR lpOut,int far *lpNeedsNewLine);
LOCAL  void pair2byteRGB(LPLONG lpSum, LPLONG lpNum, LPTR lp, int iCount);
LOCAL  void fixed2byteRGB(long far *lpSum,LPTR lp,int iCount);

LOCAL  void DoSizeDownCMYK		(LPSMART lpsm, LPTR lpIn,long peak,long far *lpLine,long far *lpNumber);
LOCAL  int  SmartSizeDownCMYK	(LPSMART lpsm, LPTR lpIn,LPTR lpOut,int far *lpNeedsNewLine);
LOCAL  void DoSizeUpCMYK		(LPSMART lpsm, LPTR lpIn,long peak,long far *lpLine);
LOCAL  int  SmartSizeUpCMYK	(LPSMART lpsm, LPTR lpIn,LPTR lpOut,int far *lpNeedsNewLine);
LOCAL  void pair2byteCMYK(LPLONG lpSum, LPLONG lpNum, LPTR lp, int iCount);
LOCAL  void fixed2byteCMYK(LPLONG lpSum,LPTR lp,int iCount);


/************************************************************************/
LPVOID SmartSetup ( int xIn, int xOut, int yIn, int yOut, int depth )
/************************************************************************/
{
	LPSMART lpsm;

	lpsm = (LPSMART)Alloc(sizeof(SMART));
	if (!lpsm)
		return(NULL);

	lpsm->inWidth    = xIn;
	lpsm->outWidth   = xOut;
	lpsm->outWidth4  = 4L * (lpsm->outWidth+1);
	lpsm->inHeight   = yIn;
	lpsm->outHeight  = yOut;

/* filter width e.g., ++X++X++X++ in=11, out=3, width=5 */
/* a half filter is the distance between output pixels */
	if (lpsm->inWidth > lpsm->outWidth)
	{
		if (lpsm->inHeight < lpsm->outHeight)
		{
//	    	Print ("Cannot distort with SmartSizing");
			goto ExitError;
		}
		switch(depth)
		{
			case 1 : lpsm->lpSmartProc = SmartSizeDown; break;
			case 3 : lpsm->lpSmartProc = SmartSizeDownRGB; break;
			case 4 : lpsm->lpSmartProc = SmartSizeDownCMYK; break;
		}

		lpsm->yrate = FGET16 (lpsm->outHeight, lpsm->inHeight);
		lpsm->xrateseed = FGET16(lpsm->outWidth-1, lpsm->inWidth);
	}
   else
	{
		if (lpsm->inHeight > lpsm->outHeight)
		{
//	    	Print ("Cannot distort with SmartSizing");
			goto ExitError;
		}

		switch(depth)
		{
			case 1 : lpsm->lpSmartProc = SmartSizeUp; break;
			case 3 : lpsm->lpSmartProc = SmartSizeUpRGB; break;
			case 4 : lpsm->lpSmartProc = SmartSizeUpCMYK; break;
		}

		lpsm->yrate = FGET16 (lpsm->inHeight, lpsm->outHeight);
		lpsm->xrateseed = FGET16 (lpsm->inWidth-1, lpsm->outWidth);
#ifdef QUADRATIC
		lpsm->yrate = TMUL16 (lpsm->yrate, lpsm->yrate);
		lpsm->yrateorig = lpsm->yrate;
		lpsm->yraterate = 2*lpsm->yrate;
#endif
#ifdef XQUADRATIC
		lpsm->xrateseed = TMUL16 (lpsm->xrateseed, lpsm->xrateseed);
#endif
	}

	lpsm->yweight = UNITY;
	lpsm->lpLastInput = NULL;

	lpsm->SumLine0 = lpsm->SumLine1 = NULL;
	lpsm->NumLine0 = lpsm->NumLine1 = NULL;
	lpsm->SumLine0 = Alloc (lpsm->outWidth4 * depth);
	lpsm->NumLine0 = Alloc (lpsm->outWidth4 * depth);
	lpsm->SumLine1 = Alloc (lpsm->outWidth4 * depth);
	lpsm->NumLine1 = Alloc (lpsm->outWidth4 * depth);
	if (!lpsm->SumLine0 ||
		!lpsm->NumLine0 ||
		!lpsm->SumLine1 ||
		!lpsm->NumLine1)
	{
		FreeUp (lpsm->SumLine0);
		FreeUp (lpsm->NumLine0);
		FreeUp (lpsm->SumLine1);
		FreeUp (lpsm->NumLine1);
		goto ExitError;
   }
	lclr (lpsm->SumLine0, lpsm->outWidth4 * depth);
	lclr (lpsm->NumLine0, lpsm->outWidth4 * depth);
	lclr (lpsm->SumLine1, lpsm->outWidth4 * depth);
	lclr (lpsm->NumLine1, lpsm->outWidth4 * depth);
	return ((LPVOID)lpsm);
ExitError:
	if (lpsm)
		FreeUp((LPTR)lpsm);
	return(NULL);
}


/************************************************************************/
void SmartDone (LPVOID lpSmart)
/************************************************************************/
{
	if (!lpSmart)
		return;
	FreeUp (((LPSMART)lpSmart)->SumLine0);
	FreeUp (((LPSMART)lpSmart)->NumLine0);
	FreeUp (((LPSMART)lpSmart)->SumLine1);
	FreeUp (((LPSMART)lpSmart)->NumLine1);
}


/************************************************************************/
BOOL SmartSize ( LPVOID lpSmart, LPTR lpIn, LPTR lpOut, LPINT lpNeedsNewLine )
/************************************************************************/
{
	return ((*((LPSMART)lpSmart)->lpSmartProc) ((LPSMART)lpSmart, lpIn, lpOut, lpNeedsNewLine));
}


/************************************************************************/
LOCAL BOOL SmartSizeUp (LPSMART lpsm, LPTR lpIn, LPTR lpOut, LPINT lpNeedsNewLine )
/************************************************************************/
{
/* if it was never set, start with the next line */
	if (!lpsm->lpLastInput)
	{
		lpsm->lpLastInput = lpsm->NumLine0;
		copy (lpIn, lpsm->lpLastInput, lpsm->inWidth);
   }

	DoSizeUp (lpsm,
		lpsm->lpLastInput,
		lpsm->yweight,
		(LPLONG)lpsm->SumLine0);
#ifdef QUADRATIC
	DoSizeUp (lpsm,
		lpIn,
		UNITY-(lpsm->yweight*2) + TMUL16 (lpsm->yweight,
		lpsm->yweight),
		(LPLONG)lpsm->SumLine0);
#else
	DoSizeUp (lpsm,
		lpIn,
		UNITY- lpsm->yweight,
		(LPLONG)lpsm->SumLine0);
#endif

	// An output line is always ready 
	fixed2byte ((LPLONG)lpsm->SumLine0, lpOut, lpsm->outWidth);
	lclr (lpsm->SumLine0, lpsm->outWidth4);

	// Setup y for the next line
	// Advance the y weight & check it
	if ((lpsm->yweight -= lpsm->yrate) < 0)
	{
		lpsm->yweight += UNITY; // y weight is always kept between 0 and UNITY 
		copy (lpIn, lpsm->lpLastInput, lpsm->inWidth);
		*lpNeedsNewLine = YES; // need a new input line
#ifdef QUADRATIC
		lpsm->yrate = lpsm->yrateorig;
#endif
    }
	else
	{
		*lpNeedsNewLine = NO; // don't need a new input line 
#ifdef QUADRATIC
		lpsm->yrate += lpsm->yraterate;
#endif
	}
	return (TRUE);
}


/************************************************************************/
LOCAL void DoSizeUp (LPSMART lpsm, LPTR lpIn, LFIXED16 peak, LPLONG lpLine )
/************************************************************************/
{
int out;
LFIXED16 xweight, xrate;
BYTE p1, p2;

	xrate = TMUL16 (lpsm->xrateseed, peak);
#ifdef XQUADRATIC
	lpsm->xrateorig = xrate;
	lpsm->xraterate = 2*xrate;
#endif

	xweight = peak;

	p1 = *lpIn++;
	p2 = *lpIn++;

/* Run the loop over each output pixel */
	out = lpsm->outWidth;
	while (--out >= 0)
	{
		// Accumulate the product of the pixels times the weight 
		*lpLine += (xweight * p1);
		*lpLine += ((peak-xweight) * p2);
		lpLine++; // on to the next output pixel

		/* Decrement the x weight & check it */
		if ((xweight -= xrate) < 0)
		{
			xweight += peak; /* xweight is always kept between 0 and peak */
			p1 = p2; /* go on to the next input pixel */
			p2 = *lpIn++;
#ifdef XQUADRATIC
			xrate = lpsm->xrateorig;
#endif
		}
#ifdef XQUADRATIC
		else
			xrate += lpsm->xraterate;
#endif
	}
}


/************************************************************************/
LOCAL BOOL SmartSizeDown (LPSMART lpsm, LPTR lpIn, LPTR lpOut, LPINT lpNeedsNewLine )
/************************************************************************/
{
	LPTR lp;

	DoSizeDown (lpsm,
		lpIn,
		lpsm->yweight,
		(LPLONG)lpsm->SumLine0,
		(LPLONG)lpsm->NumLine0);
	DoSizeDown (lpsm,
		lpIn, UNITY -
		lpsm->yweight,
		(LPLONG)lpsm->SumLine1,
		(LPLONG)lpsm->NumLine1);

	*lpNeedsNewLine = YES; /* always need a new input line */

	// Setup y for the next line
	if ((lpsm->yweight -= lpsm->yrate) < 0) // Advance the y weight & check it 
		lpsm->yweight += UNITY; 	// y weight is always kept between 0 and UNITY 
	else
		return (FALSE); /* don't have a line to output */

/* An output line is ready */
	pair2byte ((LPLONG)lpsm->SumLine0, (LPLONG)lpsm->NumLine0, lpOut, lpsm->outWidth);
	lp = lpsm->SumLine0; lpsm->SumLine0 = lpsm->SumLine1; lpsm->SumLine1 = lp;
	lp = lpsm->NumLine0; lpsm->NumLine0 = lpsm->NumLine1; lpsm->NumLine1 = lp;
	lclr (lpsm->SumLine1, lpsm->outWidth4);
	lclr (lpsm->NumLine1, lpsm->outWidth4);
	return (TRUE);
}


/************************************************************************/
LOCAL void DoSizeDown (LPSMART lpsm, LPTR lpIn, LFIXED16 peak, LPLONG lpLine, LPLONG lpNumber )
/************************************************************************/
{
	LFIXED16  xweight, xrate;
#ifdef C_CODE
	int in;
	WORD word;
	BYTE p;
#endif

	xrate = TMUL16 (lpsm->xrateseed, peak);
	xweight = peak;

#ifdef C_CODE
	/* Run the loop over each input pixel */
	in = lpsm->inWidth;
	while (--in >= 0)
	{
		p = *lpIn++;
		/* Accumulate the product of the pixels times the xweight */
		word = xweight >> 8;
		*lpLine += (word * p);
		*lpNumber += word;
		word = (peak-xweight) >> 8;
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
THIS ASSEMBLY IS OUT OF DATE BECUSE IT REFERENCES OLD STATICS
INSTEAD OF lpsm!
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
; word = xweight >> 8;
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
LOCAL int DoSizeDown ( LPTR lpIn, int IsOdd )
/************************************************************************/
{
int in, out;
LPLONG lpTemp;
LFIXED16 peak, altpeak, lTemp, weight, step; /* The step is +UNITY or -UNITY */
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
void pair2byte ( LPLONG lpSum, LPLONG lpNum, LPTR lp, int iCount )
/************************************************************************/
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
LOCAL void fixed2byte ( LPLONG lpSum, LPTR lp, int iCount )
/************************************************************************/
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
LOCAL void pair2byteRGB ( LPLONG lpSum, LPLONG lpNum, LPTR lp, int iCount )
/************************************************************************/
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
LOCAL void DoSizeDownRGB (LPSMART lpsm, LPTR lpIn, LFIXED16 peak, LPLONG lpLine, LPLONG lpNumber )
/************************************************************************/
{
	int in;
	LFIXED16 xweight, xrate;
	WORD word;

	RGBS pixel;
	LPTR tlptr;
	int inWidth;

	xrate = TMUL16 (lpsm->xrateseed, peak);
	xweight = peak;

/* Run the loop over each input pixel */
	inWidth = lpsm->inWidth;
	for (in = 0; in < inWidth; in++)
	{
		tlptr = lpIn + 3 * in;
		pixel.red   = tlptr[0];
		pixel.green = tlptr[1];
		pixel.blue  = tlptr[2];
		//	frame_getRGB (tlptr, 3, &pixel);

		/* Accumulate the product of the pixels times the xweight */
		word = xweight >> 8;
		*lpLine += (word * pixel.red);
		*lpNumber += word;
		*(lpLine+1) += (word * pixel.green);
		*(lpNumber+1) += word;
		*(lpLine+2) += (word * pixel.blue);
		*(lpNumber+2) += word;

		word = (peak-xweight) >> 8;
		*(lpLine+3) += (word * pixel.red);
		*(lpNumber+3) += word;
		*(lpLine+3+1) += (word * pixel.green);
		*(lpNumber+3+1) += word;
		*(lpLine+3+2) += (word * pixel.blue);
		*(lpNumber+3+2) += word;

		/* If the filter's weight went negative */
		if ((xweight -= xrate) < 0)
		{
			xweight += peak; /* Increase the weight */
			/* on to the next output pixel */
			lpLine += 3;
			lpNumber += 3;
		}
	}
}


/************************************************************************/
LOCAL BOOL SmartSizeDownRGB (LPSMART lpsm, LPTR lpIn, LPTR lpOut, LPINT lpNeedsNewLine )
/************************************************************************/
{
	LPTR lp;

	DoSizeDownRGB (lpsm,
		lpIn,
		lpsm->yweight,
		(LPLONG)lpsm->SumLine0,
		(LPLONG)lpsm->NumLine0);
	DoSizeDownRGB (lpsm,
		lpIn,
		UNITY - lpsm->yweight,
		(LPLONG)lpsm->SumLine1,
		(LPLONG)lpsm->NumLine1);

	*lpNeedsNewLine = YES; /* always need a new input line */

/* Setup y for the next line */
	if ((lpsm->yweight -= lpsm->yrate) < 0) /* Advance the y weight & check it */
		lpsm->yweight += UNITY; /* y weight is always kept between 0 and UNITY */
	else
		return (FALSE); /* don't have a line to output */

/* An output line is ready */
	pair2byteRGB ((LPLONG)lpsm->SumLine0, (LPLONG)lpsm->NumLine0, lpOut, lpsm->outWidth);
	lp = lpsm->SumLine0; lpsm->SumLine0 = lpsm->SumLine1; lpsm->SumLine1 = lp;
	lp = lpsm->NumLine0; lpsm->NumLine0 = lpsm->NumLine1; lpsm->NumLine1 = lp;
	lclr (lpsm->SumLine1, 3 * lpsm->outWidth4);
	lclr (lpsm->NumLine1, 3 * lpsm->outWidth4);
	return (TRUE);
}


/************************************************************************/
LOCAL void fixed2byteRGB ( LPLONG lpSum, LPTR lp, int iCount )
/************************************************************************/
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
LOCAL void DoSizeUpRGB (LPSMART lpsm, LPTR lpIn, LFIXED16 peak, LPLONG lpLine )
/************************************************************************/
{
	int out;
	LFIXED16 xweight, xrate;
	RGBS pel1, pel2;

	xrate = TMUL16 (lpsm->xrateseed, peak);
#ifdef XQUADRATIC
	lpsm->xrateorig = xrate;
	lpsm->xraterate = 2*xrate;
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
	out = lpsm->outWidth;
	while (--out >= 0)
	{
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
		if ((xweight -= xrate) < 0)
		{
			xweight += peak; /* xweight is always kept between 0 and peak */
			pel1 = pel2; /* go on to the next input pixel */

			pel2.red   = lpIn[0];
			pel2.green = lpIn[1];
			pel2.blue  = lpIn[2];
//			frame_getRGB (lpIn, 3, &pel2);

			lpIn += 3;
#ifdef XQUADRATIC
			xrate = lpsm->xrateorig;
#endif
		}
#ifdef XQUADRATIC
		else
			xrate += lpsm->xraterate;
#endif
	}
}


/************************************************************************/
LOCAL BOOL SmartSizeUpRGB (LPSMART lpsm, LPTR lpIn, LPTR lpOut, LPINT lpNeedsNewLine )
/************************************************************************/
{
	/* if it was never set, start with the next line */
	if (!lpsm->lpLastInput)
	{
		lpsm->lpLastInput = lpsm->NumLine0;
		copy (lpIn, lpsm->lpLastInput, 3 * lpsm->inWidth);
	}

	DoSizeUpRGB (lpsm,
		lpsm->lpLastInput,
		lpsm->yweight,
		(LPLONG)lpsm->SumLine0);
#ifdef QUADRATIC
	DoSizeUpRGB (lpsm,
		lpIn,
		UNITY- (lpsm->yweight*2) + TMUL16 (lpsm->yweight, lpsm->yweight), 
		(LPLONG)lpsm->SumLine0);
#else
	DoSizeUpRGB (lpsm,
		lpIn,
		UNITY-lpsm->yweight,
		(LPLONG)lpsm->SumLine0);
#endif

	/* An output line is always ready */
	fixed2byteRGB ((LPLONG)lpsm->SumLine0, lpOut, lpsm->outWidth);
	lclr (lpsm->SumLine0, 3 * lpsm->outWidth4);

	/* Setup y for the next line */
	/* Advance the y weight & check it */
	if ((lpsm->yweight -= lpsm->yrate) < 0)
	{
		lpsm->yweight += UNITY; /* y weight is always kept between 0 and UNITY */
		copy (lpIn, lpsm->lpLastInput, 3 * lpsm->inWidth);
		*lpNeedsNewLine = YES; /* need a new input line */
#ifdef QUADRATIC
		lpsm->yrate = lpsm->yrateorig;
#endif
	}
	else
	{
		*lpNeedsNewLine = NO; /* don't need a new input line */
#ifdef QUADRATIC
		lpsm->yrate += lpsm->yraterate;
#endif
	}

	return (TRUE);
}


/************************************************************************/
LOCAL void pair2byteCMYK ( LPLONG lpSum, LPLONG lpNum, LPTR lp, int iCount )
/************************************************************************/
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
LOCAL void fixed2byteCMYK ( LPLONG lpSum, LPTR lp, int iCount )
/************************************************************************/
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
LOCAL void DoSizeDownCMYK (LPSMART lpsm, LPTR lpIn, LFIXED16 peak, LPLONG lpLine, LPLONG lpNumber )
/************************************************************************/
{
	int in;
	LFIXED16 xweight, xrate;
	WORD word;

	CMYKS pixel;
	LPTR  tlptr;
	int  inWidth;
	

	xrate = TMUL16 (lpsm->xrateseed, peak);
	xweight = peak;

	/* Run the loop over each input pixel */
	inWidth = lpsm->inWidth;

	for (in = 0; in < inWidth; in++)
	{
		tlptr = lpIn + 4 * in;

		pixel.c = tlptr[0];
		pixel.m = tlptr[1];
		pixel.y = tlptr[2];
		pixel.k = tlptr[3];

		/* Accumulate the product of the pixels times the xweight */
		word = xweight >> 8;
		*lpLine	   += (word * pixel.c);
		*lpNumber	 += word;
		*(lpLine  +1) += (word * pixel.m);
		*(lpNumber+1) += word;
		*(lpLine  +2) += (word * pixel.y);
		*(lpNumber+2) += word;
		*(lpLine  +3) += (word * pixel.k);
		*(lpNumber+3) += word;

		word = (peak-xweight) >> 8;
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
LOCAL BOOL SmartSizeDownCMYK (LPSMART lpsm, LPTR lpIn, LPTR lpOut, LPINT lpNeedsNewLine )
/************************************************************************/
{
	LPTR lp;

	DoSizeDownCMYK (lpsm,
		lpIn,
		lpsm->yweight,
		(LPLONG)lpsm->SumLine0,
		(LPLONG)lpsm->NumLine0);
	DoSizeDownCMYK (lpsm,
		lpIn,
		UNITY - lpsm->yweight,
		(LPLONG)lpsm->SumLine1,
		(LPLONG)lpsm->NumLine1);

	*lpNeedsNewLine = YES; /* always need a new input line */

	/* Setup y for the next line */
	if ((lpsm->yweight -= lpsm->yrate) < 0) /* Advance the y weight & check it */
		lpsm->yweight += UNITY; /* y weight is always kept between 0 and UNITY */
	else
		return (FALSE); /* don't have a line to output */

	/* An output line is ready */
	pair2byteCMYK ((LPLONG)lpsm->SumLine0,
		(LPLONG)lpsm->NumLine0,
		lpOut,
		lpsm->outWidth);
	lp = lpsm->SumLine0; lpsm->SumLine0 = lpsm->SumLine1; lpsm->SumLine1 = lp;
	lp = lpsm->NumLine0; lpsm->NumLine0 = lpsm->NumLine1; lpsm->NumLine1 = lp;
	lclr (lpsm->SumLine1, 4 * lpsm->outWidth4);
	lclr (lpsm->NumLine1, 4 * lpsm->outWidth4);
	return (TRUE);
}

/************************************************************************/
LOCAL void DoSizeUpCMYK ( LPSMART lpsm, LPTR lpIn, LFIXED16 peak, LPLONG lpLine )
/************************************************************************/
{
	int out;
	LFIXED16 xweight, xrate;
	CMYKS pel1, pel2;

	xrate = TMUL16 (lpsm->xrateseed, peak);
#ifdef XQUADRATIC
	lpsm->xrateorig = xrate;
	lpsm->xraterate = 2*xrate;
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
	out = lpsm->outWidth;
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
			xrate = lpsm->xrateorig;
#endif
		}
#ifdef XQUADRATIC
		else
		{
			xrate += lpsm->xraterate;
		}
#endif
	}
}

/************************************************************************/
LOCAL BOOL SmartSizeUpCMYK (LPSMART lpsm, LPTR lpIn, LPTR lpOut, LPINT lpNeedsNewLine )
/************************************************************************/
{
	/* if it was never set, start with the next line */
	if (!lpsm->lpLastInput)
	{
		lpsm->lpLastInput = lpsm->NumLine0;
		copy (lpIn, lpsm->lpLastInput, 4 * lpsm->inWidth);
	}

	DoSizeUpCMYK (lpsm,
		lpsm->lpLastInput,
		lpsm->yweight,
		(LPLONG)lpsm->SumLine0);
#ifdef QUADRATIC
	DoSizeUpCMYK (lpsm,
		lpIn,
		UNITY- (lpsm->yweight*2) + TMUL16 (lpsm->yweight, lpsm->yweight), 
		(LPLONG)lpsm->SumLine0);
#else
	DoSizeUpCMYK (lpsm,
		lpIn,
		UNITY-lpsm->yweight,
		(LPLONG)lpsm->SumLine0);
#endif

	/* An output line is always ready */
	fixed2byteCMYK ((LPLONG)lpsm->SumLine0, lpOut, lpsm->outWidth);
	lclr (lpsm->SumLine0, 4 * lpsm->outWidth4);

	/* Setup y for the next line */
	/* Advance the y weight & check it */
	if ((lpsm->yweight -= lpsm->yrate) < 0)
	{
		lpsm->yweight += UNITY; /* y weight is always kept between 0 and UNITY */
		copy (lpIn, lpsm->lpLastInput, 4 * lpsm->inWidth);
		*lpNeedsNewLine = YES; /* need a new input line */
#ifdef QUADRATIC
		lpsm->yrate = lpsm->yrateorig;
#endif
	}
	else
	{
		*lpNeedsNewLine = NO; /* don't need a new input line */
#ifdef QUADRATIC
		lpsm->yrate += lpsm->yraterate;
#endif
	}

	return (TRUE);
}


