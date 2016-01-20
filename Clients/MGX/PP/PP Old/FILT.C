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
#include "filt.h"

extern HANDLE hInstAstral;
extern HWND hWndAstral;
static int clockcount;
static LPTR lpMask;
static RECT rMask;
LPFRAME lpDstFrame, lpSrcFrame;

/************************************************************************/
int FilterImage( lpFilterFile )
/************************************************************************/
LPSTR	lpFilterFile;
{
FNAME szExtension;
char *p;

lstrcpy(szExtension, lpFilterFile);
if ((p = strchr(szExtension, '.')))
	{
	if (StringsEqual(p, ".flt"))
		return(FilterImageKernal(lpFilterFile));
	}
return(FilterImageDLL(lpFilterFile));
}

/************************************************************************/
int FilterImageDLL( lpFilterFile )
/************************************************************************/
LPSTR	lpFilterFile;
{
HANDLE hModule;
LPROC lpStartProc;
LPROC lpProcessProc;
LPROC lpEndProc;
int y;
FILTPARMS parms;
BOOL UsingMask, fSameSize, fSuccess = FALSE;

if ((hModule = AstralLoadLibrary(lpFilterFile)) < 32)
	return(FALSE);
lpStartProc = (LPROC)GetProcAddress(hModule, "_FiltStart");
lpProcessProc = (LPROC)GetProcAddress(hModule, "_FiltProcess");
lpEndProc = (LPROC)GetProcAddress(hModule, "_FiltEnd");
if (!lpStartProc || !lpProcessProc || !lpEndProc)
	{
	FreeLibrary(hModule);
	return(FALSE);
	}
if (!CacheInit(NO, YES))
	{
	FreeLibrary(hModule);
	return(FALSE);
	}
CacheSyncLines();

parms.hInstance = hInstAstral;
parms.hWnd = GetActiveWindow();
parms.AstralClockCursor = (LPROC)MakeProcInstance((FARPROC)DLL_AstralClockCursor, hInstAstral);
parms.frame_set = (LPTRPROC)MakeProcInstance((FARPROC)DLL_frame_set, hInstAstral);
parms.CachePtr = (LPTRPROC)MakeProcInstance((FARPROC)DLL_CachePtr, hInstAstral);
parms.FilterWrite = (LPROC)MakeProcInstance((FARPROC)DLL_FilterWrite, hInstAstral);
parms.frame_getRGB = (LPROC)MakeProcInstance((FARPROC)DLL_frame_getRGB, hInstAstral);
parms.frame_putRGB = (LPROC)MakeProcInstance((FARPROC)DLL_frame_putRGB, hInstAstral);
parms.ActiveRGB = Palette.ActiveRGB;
parms.AlternateRGB = Palette.AlternateRGB;
lpSrcFrame = lpImage->EditFrame;
parms.lpSrcFrame = (LPTR)lpSrcFrame;
parms.lpDstFrame = NULL;
parms.src_npix = lpImage->npix;
parms.src_nlin = lpImage->nlin;
parms.src_resolution = lpImage->xres;
parms.dst_npix = lpImage->npix;
parms.dst_nlin = lpImage->nlin;
parms.dst_resolution = lpImage->xres;
parms.Depth = DEPTH;
parms.lpOut = NULL;
mask_rect(&parms.rMask);
parms.rOut = parms.rMask;

lpDstFrame = NULL;
lpMask = NULL;
if ((*lpStartProc)((LPFILTPARMS)&parms))
	{
	lpDstFrame = frame_open(DEPTH, parms.dst_npix, parms.dst_nlin, parms.dst_resolution);
	parms.lpDstFrame = (LPTR)lpDstFrame;
	fSameSize = parms.dst_npix == parms.src_npix && parms.dst_nlin == parms.src_nlin && parms.dst_resolution == parms.src_resolution;
	UsingMask = ((lpImage->EditFrame->WriteMask != NULL) || (ColorMask.Mask && !ColorMask.Off)) && EqualRect(&parms.rMask, &parms.rOut);
	if (lpDstFrame)
		{
		if (fSameSize)
			{
			if (UsingMask)
				{
				mask_rect(&rMask);
				lpMask = Alloc((long)RectWidth(&rMask));
				}
			lpDstFrame->WriteMask = lpSrcFrame->WriteMask;
			lpSrcFrame->WriteMask = NULL;
			lpImage->EditFrame = lpDstFrame;
			}
		parms.lpOut = Alloc((long)RectWidth(&parms.rOut)*(long)DEPTH);
		if (parms.lpOut)
			{
			if ((*lpProcessProc)((LPFILTPARMS)&parms))
				{
				fSuccess = TRUE;
				(*lpEndProc)((LPFILTPARMS)&parms);
				}
			}
		}
	}
if (fSuccess)
	{
	if (lpMask)
		{
		for (y = 0; y < rMask.top; ++y)
			frame_copyline(lpSrcFrame, lpDstFrame, y);
		for (y = rMask.bottom+1; y < lpDstFrame->Ysize; ++y)
			frame_copyline(lpSrcFrame, lpDstFrame, y);
		}
	if (fSameSize)
		frame_set(lpImage->EditFrame);
	else
		{
		lpImage->EditFrame = lpImage->UndoFrame;
		lpImage->UndoFrame = NULL;
		if (lpImage->EditFrame && !lpImage->fNewFrame)
			{
			lpImage->EditFrame->WriteMask = lpSrcFrame->WriteMask;
			lpSrcFrame->WriteMask = NULL;
			}
		CacheNewFrame(lpDstFrame);
		UpdateImageSize();
		}
	frame_close(lpSrcFrame);
	}
else
	{
	if (lpDstFrame)
		{
		if (fSameSize)
			{
			lpSrcFrame->WriteMask = lpDstFrame->WriteMask;
		 	lpDstFrame->WriteMask = NULL;
			lpImage->EditFrame = lpSrcFrame;
			frame_set(lpImage->EditFrame);
			}
		frame_close(lpDstFrame);
		}
	}
if (parms.lpOut)
	FreeUp(parms.lpOut);
if (lpMask)
	FreeUp(lpMask);
FreeProcInstance((FARPROC)parms.AstralClockCursor);
FreeProcInstance((FARPROC)parms.frame_set);
FreeProcInstance((FARPROC)parms.CachePtr);
FreeProcInstance((FARPROC)parms.FilterWrite);
FreeProcInstance((FARPROC)parms.frame_getRGB);
FreeProcInstance((FARPROC)parms.frame_putRGB);
FreeLibrary(hModule);
return(fSuccess);
}

/************************************************************************/
int FilterImageKernal( lpFilterFile )
/************************************************************************/
LPSTR	lpFilterFile;
{
#define MAXVALUES 17
BYTE	*buf, buffer[512];
FNAME	szFilterFile;
int	iValues[MAXVALUES], *pValues, count, length, numvals, fh, ret;
KERNAL	sKernal, *pKernal;
RECT rMask;

AstralCursor( IDC_WAIT );	/* Put up the wait cursor */

lstrcpy( (LPSTR)szFilterFile, (LPSTR)lpFilterFile );
if ( ( fh = _lopen( szFilterFile, OF_READ ) ) < 0 )
	return( FALSE );

/* Make one big file read */
if ( ( count = _lread( fh, buffer, sizeof(buffer)-1 ) ) <= 0 )
	{
	_lclose( fh );
	return( FALSE );
	}

_lclose( fh );
buffer[count] = '\0'; /* Null terminate the buffer */
/* Make sure its a valid filter file */
if ( strncmp( "FLT", buffer, 3 ) )
	return( FALSE );
buf = buffer + 3; /* skip over the "FLT" */
count -= 3; /* don't count the "FLT" */
nullify( buf, count );

numvals = 0;
pValues = iValues;
pKernal = &sKernal;
ret = FALSE;
clockcount = 0;

while ( 1 )
	{
	/* If we ran short of data, break away */
	if ( count <= 0 )
		goto Exit;

	/* Skip any leading nulls */
	while ( *buf == '\0' )
		{
		buf++;
		/* If we ran short of data, break away */
		if ( --count <= 0 )
			goto Exit;
		}

	/* Get the next integer coefficient and stuff it into the kernal */
	*pValues++ = atoi( buf );

	/* Get ready for the next value */
	length = strlen( buf ) + 1;
	buf += length;
	count -= length; /* Check count at the beginning of the next loop */

	/* Skip the code to do the work until we have enough values */
	if ( ++numvals < MAXVALUES )
		continue;

	/* Start the convolution since we have enough coefficients! */

	/* Build lookup tables (to make the multiplies faster) */
	/* from the 9 coefficients */
	BuildLut( iValues[1], pKernal->lut1, sizeof( pKernal->lut1 ) );
	BuildLut( iValues[2], pKernal->lut2, sizeof( pKernal->lut2 ) );
	BuildLut( iValues[3], pKernal->lut3, sizeof( pKernal->lut3 ) );
	BuildLut( iValues[4], pKernal->lut4, sizeof( pKernal->lut4 ) );
	BuildLut( iValues[5], pKernal->lut5, sizeof( pKernal->lut5 ) );
	BuildLut( iValues[6], pKernal->lut6, sizeof( pKernal->lut6 ) );
	BuildLut( iValues[7], pKernal->lut7, sizeof( pKernal->lut7 ) );
	BuildLut( iValues[8], pKernal->lut8, sizeof( pKernal->lut8 ) );
	BuildLut( iValues[9], pKernal->lut9, sizeof( pKernal->lut9 ) );

	/* Load the absolute value flag; either 0 (bound at 0) or 1 (abs) */
	pKernal->absflag = iValues[10];;

	/* Load the kernal rotation count; either 0, 1, or 2 */
	pKernal->rotation = iValues[11];

	/* Load the edge type flag; either 0 (no edge checking) or 1 */
	pKernal->edgetype = iValues[12];

	/* Build the output lut based on 2 range pairs */
	BuildOutLut( iValues[13], iValues[14], iValues[15], iValues[16],
		pKernal->outlut, sizeof( pKernal->outlut ) );

	/* Do the job */
	ret = Convolve( pKernal, iValues[0] /* # filters */ );

	/* Get ready to go check for another filter */
	numvals = 0;
	pValues = iValues;
	}

Exit:
AstralCursor( NULL );		/* Revert back to the old cursor */
return( ret );
}


/************************************************************************/
BOOL Convolve( pKernal, total )
/************************************************************************/
KERNAL	*pKernal;
int total;
{
int y, y1, y2, x1, x2, dx, dy;
LPTR lpBuffer[4], Line1, Line2, Line3, Output, Temp;
RECT rMask;
SPAN span;

mask_rect( &rMask );
y1 = bound( rMask.top, 0, lpImage->nlin-1 );
y2 = bound( rMask.bottom, 0, lpImage->nlin-1 );
x1 = bound( rMask.left, 0, lpImage->npix-1 );
x2 = bound( rMask.right, 0, lpImage->npix-1 );

dx = x2 - x1 + 1;
dy = y2 - y1 + 1;
total *= dy;

if (!AllocLines(lpBuffer, 4, dx, DEPTH))
	return(FALSE);

Line1  = lpBuffer[0];
Line2  = lpBuffer[1];
Line3  = lpBuffer[2];
Output = lpBuffer[3];

span.sx = x1;
span.dx = dx;
span.dy = 1;
span.sy = y1;
if ( !CacheRead( &span, Line1, dx ) )
	{
	FreeUp( lpBuffer[0] );
	return( FALSE );
	}
span.sy = y1 + 1;
if ( !CacheRead( &span, Line2, dx ) )
	{
	FreeUp( lpBuffer[0] );
	return( FALSE );
	}
for ( span.sy=y1+2; span.sy<=y2; span.sy++ )
	{
	AstralClockCursor( clockcount++, total );
	/* Read the new line into the third buffer */
	if ( !CacheRead( &span, Line3, dx ) )
		{
		FreeUp( lpBuffer[0] );
		return( FALSE );
		}
	/* Run the kernal over the second buffer */
	/* Put the results in the output buffer */
	frame_kernal( Line1, Line2, Line3, dx, pKernal, Output );

	/* Write the line from the output buffer */
	--span.sy;
	if ( !CacheWrite( &span, Output, dx ) )
		{
		FreeUp( lpBuffer[0] );
		return( FALSE );
		}
	++span.sy;
	Temp  = Line1;
	Line1 = Line2;
	Line2 = Line3;
	Line3 = Temp;
	}

FreeUp( lpBuffer[0] );
return( TRUE );
}

/************************************************************************/
VOID kernal8( Line1, Line2, Line3, iCount, pKernal, Output )
/************************************************************************/
LPTR Line1, Line2, Line3;
int	iCount;
KERNAL *pKernal;
LPTR Output;
{
REG long sum, sum1;
long	bias;
BOOL	edgetype, absflag, rotation;
char	*outlut;
int	*lut1, *lut2, *lut3, *lut4, *lut5, *lut6, *lut7, *lut8, *lut9;
BYTE	center;
int	edge, diff;

/* Make it faster to access some important variables */
edgetype = pKernal->edgetype;
absflag = pKernal->absflag;
if ( absflag == 0 || absflag == 1 )
	bias = 0;
else	{
	bias = (long)absflag << 3;
	absflag = 0;
	}
rotation = pKernal->rotation;
outlut = pKernal->outlut;
lut1 = pKernal->lut1; lut2 = pKernal->lut2; lut3 = pKernal->lut3;
lut4 = pKernal->lut4; lut5 = pKernal->lut5; lut6 = pKernal->lut6;
lut7 = pKernal->lut7; lut8 = pKernal->lut8; lut9 = pKernal->lut9;

/* Output the first value unchanged */
*Output++ = *Line2;
iCount -= 2;

while ( --iCount >= 0 )
	{
	if ( edgetype )			/* edge lookup the edge size */
		{			/* before running the filter */
		edge = 0;
		center = *(Line2+1);
		if ( ( diff = center - *(Line1+1) ) < 0 ) /* pixel above */
			diff = -diff;
		if ( diff > edge ) edge = diff;
		if ( ( diff = center - *(Line2) ) < 0 )   /* pixel to left */
			diff = -diff;
		if ( diff > edge ) edge = diff;
		if ( ( diff = center - *(Line2+2) ) < 0 ) /* pixel to right */
			diff = -diff;
		if ( diff > edge ) edge = diff;
		if ( ( diff = center - *(Line3+1) ) < 0 ) /* pixel below */
			diff = -diff;
		if ( diff > edge ) edge = diff;
		if ( !outlut[ edge ] )		/* not outputing this size.. */
			{
			*Output++ = center;	/* value is unchanged */
			Line1++;		/* bump the line pointers */
			Line2++;
			Line3++;
			continue;
			}
		}

	sum = bias;				/*  1  2  3  */
	sum += lut1[*(Line1)];			/*  4  5  6  */
	sum += lut2[*(Line1+1)];		/*  7  8  9  */
	sum += lut3[*(Line1+2)];
	sum += lut4[*(Line2)];
	sum += lut5[*(Line2+1)];
	sum += lut6[*(Line2+2)];
	sum += lut7[*(Line3)];
	sum += lut8[*(Line3+1)];
	sum += lut9[*(Line3+2)];
	if ( sum < 0 )
		if ( absflag )
			sum = -sum;
		else	sum = 0;

	if ( rotation == 1 )			/*  4  1  2  */
		{				/*  7  5  3  */
		sum1 = bias;			/*  8  9  6  */
		sum1 += lut4[*(Line1)];
		sum1 += lut1[*(Line1+1)];
		sum1 += lut2[*(Line1+2)];
		sum1 += lut7[*(Line2)];
		sum1 += lut5[*(Line2+1)];
		sum1 += lut3[*(Line2+2)];
		sum1 += lut8[*(Line3)];
		sum1 += lut9[*(Line3+1)];
		sum1 += lut6[*(Line3+2)];
		if ( sum1 < 0 )
			if ( absflag )
				sum1 = -sum1;
			else	sum1 = 0;
		sum += sum1;
		}
	else
	if ( rotation == 2 )			/*  7  4  1  */
		{				/*  8  5  2  */
		sum1 = bias;			/*  9  6  3  */
		sum1 += lut7[*(Line1)];
		sum1 += lut4[*(Line1+1)];
		sum1 += lut1[*(Line1+2)];
		sum1 += lut8[*(Line2)];
		sum1 += lut5[*(Line2+1)];
		sum1 += lut2[*(Line2+2)];
		sum1 += lut9[*(Line3)];
		sum1 += lut6[*(Line3+1)];
		sum1 += lut3[*(Line3+2)];
		if ( sum1 < 0 )
			if ( absflag )
				sum1 = -sum1;
			else	sum1 = 0;
		sum += sum1;
		}

	++Line1; ++Line2; ++Line3;	/* bump the line pointers */

	sum += 4;			/* rounding */
	sum >>= 3;			/* get rid of the fractional bits */

	if ( sum > 255 )		/* bound the high end */
		sum = 255;

	if ( absflag )
		sum = 255 - sum;

	if ( edgetype || outlut[ sum ] )
		*Output++ = sum;	/* output the new value */
	else	*Output++ = *Line2;	/* value is unchanged */
	}

/* Output the last value unchanged */
++Line2;
*Output = *Line2;
}


#ifdef UNUSED
/************************************************************************/
VOID kernal16L( Line1, Line2, Line3, iCount, pKernal, Output )
/************************************************************************/
LPWORD Line1, Line2, Line3;
int	iCount;
KERNAL	*pKernal;
LPWORD Output;
{
REG long sum, sum1;
long	bias;
BOOL	edgetype, absflag, rotation;
char	*outlut;
int	*lut1, *lut2, *lut3, *lut4, *lut5, *lut6, *lut7, *lut8, *lut9;
BYTE	Lum1Plus1, Lum2, Lum2Plus2, Lum3Plus1;
BYTE	center;
int	edge, diff;
HSLS hsl;

/* Make it faster to access some important variables */
edgetype = pKernal->edgetype;
absflag = pKernal->absflag;
if ( absflag == 0 || absflag == 1 )
	bias = 0;
else	{
	bias = (long)absflag << 3;
	absflag = 0;
	}
rotation = pKernal->rotation;
outlut = pKernal->outlut;
lut1 = pKernal->lut1; lut2 = pKernal->lut2; lut3 = pKernal->lut3;
lut4 = pKernal->lut4; lut5 = pKernal->lut5; lut6 = pKernal->lut6;
lut7 = pKernal->lut7; lut8 = pKernal->lut8; lut9 = pKernal->lut9;

/* Output the first value unchanged */
*Output++ = *Line2;
iCount -= 2;

while ( --iCount >= 0 )
	{
	center = MiniRGBtoL(*(Line2+1));
	Lum1Plus1 = MiniRGBtoL(*(Line1+1));
	Lum2 = MiniRGBtoL(*(Line2));
	Lum2Plus2 = MiniRGBtoL(*(Line2+2));
	Lum3Plus1 = MiniRGBtoL(*(Line3+1));
	if ( edgetype )			/* edge lookup the edge size */
		{			/* before running the filter */
		edge = 0;
		if ( ( diff = center - Lum1Plus1 ) < 0 ) /* pixel above */
			diff = -diff;
		if ( diff > edge ) edge = diff;
		if ( ( diff = center -  Lum2) < 0 )   /* pixel to left */
			diff = -diff;
		if ( diff > edge ) edge = diff;
		if ( ( diff = center -  Lum2Plus2) < 0 ) /* pixel to right */
			diff = -diff;
		if ( diff > edge ) edge = diff;
		if ( ( diff = center - Lum3Plus1 ) < 0 ) /* pixel below */
			diff = -diff;
		if ( diff > edge ) edge = diff;
		if ( !outlut[ edge ] )		/* not outputing this size.. */
			{
			*Output++ = *(Line2+1);	/* value is unchanged */
			Line1++;		/* bump the line pointers */
			Line2++;
			Line3++;
			continue;
			}
		}

	sum = bias;					/*  1  2  3  */
	sum += lut1[MiniRGBtoL(*(Line1))];		/*  4  5  6  */
	sum += lut2[Lum1Plus1];				/*  7  8  9  */
	sum += lut3[MiniRGBtoL(*(Line1+2))];
	sum += lut4[Lum2];
	sum += lut5[center];
	sum += lut6[Lum2Plus2];
	sum += lut7[MiniRGBtoL(*(Line3))];
	sum += lut8[Lum3Plus1];
	sum += lut9[MiniRGBtoL(*(Line3+2))];
	if ( sum < 0 )
		if ( absflag )
			sum = -sum;
		else	sum = 0;

	if ( rotation == 1 )			/*  4  1  2  */
		{				/*  7  5  3  */
		sum1 = bias;			/*  8  9  6  */
		sum1 += lut4[MiniRGBtoL(*(Line1))];
		sum1 += lut1[Lum1Plus1];
		sum1 += lut2[MiniRGBtoL(*(Line1+2))];
		sum1 += lut7[Lum2];
		sum1 += lut5[center];
		sum1 += lut3[Lum2Plus2];
		sum1 += lut8[MiniRGBtoL(*(Line3))];
		sum1 += lut9[Lum3Plus1];
		sum1 += lut6[MiniRGBtoL(*(Line3+2))];
		if ( sum1 < 0 )
			if ( absflag )
				sum1 = -sum1;
			else	sum1 = 0;
		sum += sum1;
		}
	else
	if ( rotation == 2 )			/*  7  4  1  */
		{				/*  8  5  2  */
		sum1 = bias;			/*  9  6  3  */
		sum1 += lut7[MiniRGBtoL(*(Line1))];
		sum1 += lut4[Lum1Plus1];
		sum1 += lut1[MiniRGBtoL(*(Line1+2))];
		sum1 += lut8[Lum2];
		sum1 += lut5[center];
		sum1 += lut2[Lum2Plus2];
		sum1 += lut9[MiniRGBtoL(*(Line3))];
		sum1 += lut6[Lum3Plus1];
		sum1 += lut3[MiniRGBtoL(*(Line3+2))];
		if ( sum1 < 0 )
			if ( absflag )
				sum1 = -sum1;
			else	sum1 = 0;
		sum += sum1;
		}

	++Line1; ++Line2; ++Line3;	/* bump the line pointers */

	sum += 4;			/* rounding */
	sum >>= 3;			/* get rid of the fractional bits */

	if ( sum > 255 )		/* bound the high end */
		sum = 255;

	if ( absflag )
		sum = 255 - sum;

	if ( edgetype || outlut[ sum ] )
		{
		MiniRGBtoHSL(*Line2, &hsl);
		hsl.lum = sum;
		*Output++ = HSLtoMiniRGB(&hsl);	/* output the new value */
		}
	else	*Output++ = *Line2;	/* value is unchanged */
	}

/* Output the last value unchanged */
++Line2;
*Output = *Line2;
}
#endif


/************************************************************************/
VOID kernal16( startLine1, startLine2, startLine3, startiCount, pKernal, startOutput )
/************************************************************************/
LPWORD startLine1, startLine2, startLine3;
int	startiCount;
KERNAL	*pKernal;
LPWORD	startOutput;
{
REG long sum, sum1;
long	bias;
char	edgetype, absflag, rotation;
char	*outlut;
int	*lut1, *lut2, *lut3, *lut4, *lut5, *lut6, *lut7, *lut8, *lut9;
BYTE	center, Line10, Line11, Line12, Line20, Line21, Line22, Line30, Line31, Line32;
int	edge, diff, i, iCount;
LPWORD	Line1, Line2, Line3, Output;
RGBS	rgb;

/* Make it faster to access some important variables */
edgetype = pKernal->edgetype;
absflag = pKernal->absflag;
if ( absflag == 0 || absflag == 1 )
	bias = 0;
else	{
	bias = (long)absflag << 3;
	absflag = 0;
	}
rotation = pKernal->rotation;
outlut = pKernal->outlut;
lut1 = pKernal->lut1; lut2 = pKernal->lut2; lut3 = pKernal->lut3;
lut4 = pKernal->lut4; lut5 = pKernal->lut5; lut6 = pKernal->lut6;
lut7 = pKernal->lut7; lut8 = pKernal->lut8; lut9 = pKernal->lut9;

for (i = 0; i < 3; ++i)
    {
    Line1 = startLine1;
    Line2 = startLine2;
    Line3 = startLine3;
    Output = startOutput;
    iCount = startiCount;
    
    /* Output the first value unchanged */
    *Output = *Line2;
    Output += 1;
    iCount -= 2;

    if (i == 0)
        {
        Line10 = MaxiR(*Line1);
        Line11 = MaxiR(*(Line1+1));
        Line20 = MaxiR(*Line2);
        Line21 = MaxiR(*(Line2+1));
        Line30 = MaxiR(*Line3);
        Line31 = MaxiR(*(Line3+1));
        }
    else if (i == 1)
        {
        Line10 = MaxiG(*Line1);
        Line11 = MaxiG(*(Line1+1));
        Line20 = MaxiG(*Line2);
        Line21 = MaxiG(*(Line2+1));
        Line30 = MaxiG(*Line3);
        Line31 = MaxiG(*(Line3+1));
        }
    else 
        {
        Line10 = MaxiB(*Line1);
        Line11 = MaxiB(*(Line1+1));
        Line20 = MaxiB(*Line2);
        Line21 = MaxiB(*(Line2+1));
        Line30 = MaxiB(*Line3);
        Line31 = MaxiB(*(Line3+1));
        }
    while ( --iCount >= 0 )
	    {
	    if (i == 0)
	        {
	        Line12 = MaxiR(*(Line1+2));
	        Line22 = MaxiR(*(Line2+2));
	        Line32 = MaxiR(*(Line3+2));
	        }
	    else if (i == 1)
	        {
	        Line12 = MaxiG(*(Line1+2));
	        Line22 = MaxiG(*(Line2+2));
	        Line32 = MaxiG(*(Line3+2));
	        }
	    else
	        {
	        Line12 = MaxiB(*(Line1+2));
	        Line22 = MaxiB(*(Line2+2));
	        Line32 = MaxiB(*(Line3+2));
	        }
	    if ( edgetype )			/* edge lookup the edge size */
	    	{			/* before running the filter */
	    	edge = 0;
		    center = Line21;
		    if ( ( diff = center - Line11 ) < 0 ) /* pixel above */
			    diff = -diff;
		    if ( diff > edge ) edge = diff;
		    if ( ( diff = center - Line20 ) < 0 )   /* pixel to left */
			    diff = -diff;
		    if ( diff > edge ) edge = diff;
		    if ( ( diff = center - Line22 ) < 0 ) /* pixel to right */
			    diff = -diff;
		    if ( diff > edge ) edge = diff;
		    if ( ( diff = center - Line31 ) < 0 ) /* pixel below */
			    diff = -diff;
		    if ( diff > edge ) edge = diff;
		    if ( !outlut[ edge ] )		/* not outputing this size.. */
			    {
			    *Output++ = *(Line2+1);	/* value is unchanged */
			    Line1++;		/* bump the line pointers */
			    Line2++;
			    Line3++;
                Line10 = Line11;
                Line11 = Line12;
                Line20 = Line21;
                Line21 = Line22;
                Line30 = Line31;
                Line31 = Line32;
			    continue;
			    }
		    }

	    sum = bias;				/*  1  2  3  */
	    sum += lut1[Line10];		/*  4  5  6  */
	    sum += lut2[Line11];		/*  7  8  9  */
	    sum += lut3[Line12];
 	    sum += lut4[Line20];
	    sum += lut5[Line21];
	    sum += lut6[Line22];
	    sum += lut7[Line30];
	    sum += lut8[Line31];
	    sum += lut9[Line32];
	    if ( sum < 0 )
		    if ( absflag )
			    sum = -sum;
		    else	sum = 0;

	    if ( rotation == 1 )		/*  4  1  2  */
		    {				/*  7  5  3  */
		    sum1 = bias;		/*  8  9  6  */
		    sum1 += lut4[Line10];
		    sum1 += lut1[Line11];
		    sum1 += lut2[Line12];
		    sum1 += lut7[Line20];
		    sum1 += lut5[Line21];
		    sum1 += lut3[Line22];
		    sum1 += lut8[Line30];
		    sum1 += lut9[Line31];
		    sum1 += lut6[Line32];
		    if ( sum1 < 0 )
		    	if ( absflag )
		    		sum1 = -sum1;
			    else	sum1 = 0;
		    sum += sum1;
		    }
	    else
	    if ( rotation == 2 )		/*  7  4  1  */
		    {				/*  8  5  2  */
		    sum1 = bias;		/*  9  6  3  */
		    sum1 += lut7[Line10];
		    sum1 += lut4[Line11];
		    sum1 += lut1[Line12];
		    sum1 += lut8[Line20];
		    sum1 += lut5[Line21];
		    sum1 += lut2[Line22];
		    sum1 += lut9[Line30];
		    sum1 += lut6[Line31];
		    sum1 += lut3[Line32];
		    if ( sum1 < 0 )
			    if ( absflag )
				    sum1 = -sum1;
			    else	sum1 = 0;
		    sum += sum1;
		    }

	    Line1++; Line2++; Line3++;	/* bump the line pointers */

	    sum += 4;			/* rounding */
	    sum >>= 3;			/* get rid of the fractional bits */

	    if ( sum > 255 )		/* bound the high end */
		    sum = 255;

	    if ( absflag )
		    sum = 255 - sum;

	    if ( edgetype || outlut[ sum ] )
	        {
	        if (i == 0)
	            {
	            rgb.red = sum;
	            rgb.green = MaxiG(*Line2);
	            rgb.blue = MaxiB(*Line2);
	            *Output++ = RGBtoMiniRGB(&rgb);
	            }
	        else if (i == 1)
	            {
	            rgb.red = MaxiR(*Output);
	            rgb.green = sum;
	            rgb.blue = MaxiB(*Line2);
	            *Output++ = RGBtoMiniRGB(&rgb);
	            }
	        else 
	            {
	            rgb.red = MaxiR(*Output);
	            rgb.green = MaxiG(*Output);
	            rgb.blue = sum;
	            *Output++ = RGBtoMiniRGB(&rgb);
	            }
		    }
	    else    *Output++ = *Line2;	/* value is unchanged */
        Line10 = Line11;
        Line11 = Line12;
        Line20 = Line21;
        Line21 = Line22;
        Line30 = Line31;
        Line31 = Line32;
	    }

    /* Output the last value unchanged */
    Line2++;
    *Output = *Line2;
    }
}

/************************************************************************/
VOID kernal24( startLine1, startLine2, startLine3, startiCount, pKernal, startOutput )
/************************************************************************/
LPTR startLine1, startLine2, startLine3;
int	startiCount;
KERNAL	*pKernal;
LPTR startOutput;
{
REG long sum, sum1;
long	bias;
BOOL	edgetype, absflag, rotation;
char	*outlut;
int	*lut1, *lut2, *lut3, *lut4, *lut5, *lut6, *lut7, *lut8, *lut9;
BYTE	center, Line10, Line11, Line12, Line20, Line21, Line22, Line30, Line31, Line32;
int	edge, diff, i, iCount;
LPTR Line1, Line2, Line3, Output;

/* Make it faster to access some important variables */
edgetype = pKernal->edgetype;
absflag = pKernal->absflag;
if ( absflag == 0 || absflag == 1 )
	bias = 0;
else	{
	bias = (long)absflag << 3;
	absflag = 0;
	}
rotation = pKernal->rotation;
outlut = pKernal->outlut;
lut1 = pKernal->lut1; lut2 = pKernal->lut2; lut3 = pKernal->lut3;
lut4 = pKernal->lut4; lut5 = pKernal->lut5; lut6 = pKernal->lut6;
lut7 = pKernal->lut7; lut8 = pKernal->lut8; lut9 = pKernal->lut9;

for (i = 0; i < 3; ++i)
    {
    Line1 = startLine1++;
    Line2 = startLine2++;
    Line3 = startLine3++;
    Output = startOutput++;
    iCount = startiCount;
    
    /* Output the first value unchanged */
    *Output = *Line2;
    Output += 3;
    iCount -= 2;

    Line10 = *Line1;
    Line11 = *(Line1+3);
    Line20 = *Line2;
    Line21 = *(Line2+3);
    Line30 = *Line3;
    Line31 = *(Line3+3);
    while ( --iCount >= 0 )
	    {
	    Line12 = *(Line1+6);
	    Line22 = *(Line2+6);
	    Line32 = *(Line3+6);
	    if ( edgetype )		/* edge lookup the edge size */
		    {			/* before running the filter */
	    	    edge = 0;
		    center = Line21;
		    if ( ( diff = center - Line11 ) < 0 ) /* pixel above */
			    diff = -diff;
		    if ( diff > edge ) edge = diff;
		    if ( ( diff = center - Line20 ) < 0 )   /* pixel to left */
			    diff = -diff;
		    if ( diff > edge ) edge = diff;
		    if ( ( diff = center - Line22 ) < 0 ) /* pixel to right */
			    diff = -diff;
		    if ( diff > edge ) edge = diff;
		    if ( ( diff = center - Line31 ) < 0 ) /* pixel below */
			    diff = -diff;
		    if ( diff > edge ) edge = diff;
		    if ( !outlut[ edge ] )	/* not outputing this size.. */
			    {
			    *Output = center;	/* value is unchanged */
			    Output += 3;
			    Line1 += 3;		/* bump the line pointers */
			    Line2 += 3;
			    Line3 += 3;
	                    Line10 = Line11;
	                    Line11 = Line12;
	                    Line20 = Line21;
	                    Line21 = Line22;
	                    Line30 = Line31;
	                    Line31 = Line32;
			    continue;
			    }
		    }

	    sum = bias;				/*  1  2  3  */
	    sum += lut1[Line10];		/*  4  5  6  */
	    sum += lut2[Line11];		/*  7  8  9  */
	    sum += lut3[Line12];
 	    sum += lut4[Line20];
	    sum += lut5[Line21];
	    sum += lut6[Line22];
	    sum += lut7[Line30];
	    sum += lut8[Line31];
	    sum += lut9[Line32];
	    if ( sum < 0 )
		    if ( absflag )
			    sum = -sum;
		    else	sum = 0;

	    if ( rotation == 1 )		/*  4  1  2  */
		    {				/*  7  5  3  */
		    sum1 = bias;		/*  8  9  6  */
		    sum1 += lut4[Line10];
		    sum1 += lut1[Line11];
		    sum1 += lut2[Line12];
		    sum1 += lut7[Line20];
		    sum1 += lut5[Line21];
		    sum1 += lut3[Line22];
		    sum1 += lut8[Line30];
		    sum1 += lut9[Line31];
		    sum1 += lut6[Line32];
		    if ( sum1 < 0 )
		    	if ( absflag )
		    		sum1 = -sum1;
			    else	sum1 = 0;
		    sum += sum1;
		    }
	    else
	    if ( rotation == 2 )		/*  7  4  1  */
		    {				/*  8  5  2  */
		    sum1 = bias;		/*  9  6  3  */
		    sum1 += lut7[Line10];
		    sum1 += lut4[Line11];
		    sum1 += lut1[Line12];
		    sum1 += lut8[Line20];
		    sum1 += lut5[Line21];
		    sum1 += lut2[Line22];
		    sum1 += lut9[Line30];
		    sum1 += lut6[Line31];
		    sum1 += lut3[Line32];
		    if ( sum1 < 0 )
			    if ( absflag )
				    sum1 = -sum1;
			    else	sum1 = 0;
		    sum += sum1;
		    }

	    Line1 += 3; Line2 += 3; Line3 += 3;	/* bump the line pointers */

	    sum += 4;			/* rounding */
	    sum >>= 3;			/* get rid of the fractional bits */

	    if ( sum > 255 )		/* bound the high end */
		    sum = 255;

	    if ( absflag )
		    sum = 255 - sum;

	    if ( edgetype || outlut[ sum ] )
		    *Output = sum;	/* output the new value */
	    else    *Output = *Line2;	/* value is unchanged */
	    Output += 3;
            Line10 = Line11;
            Line11 = Line12;
            Line20 = Line21;
            Line21 = Line22;
            Line30 = Line31;
            Line31 = Line32;
	    }

    /* Output the last value unchanged */
    Line2 += 3;
    *Output = *Line2;
    }
}


/************************************************************************/
VOID BuildLut( iValue, Lut, iCount )
/************************************************************************/
int	iValue, iCount;
LPINT Lut;
{
int	i;
long	v;

v = (long)iValue << 3;
iCount /= sizeof( *Lut );
for ( i=0; i<iCount; i++ )
	*Lut++ = ( ((long)i * v) + 500L ) / 1000L;
}


/************************************************************************/
VOID BuildOutLut( iValue1, iValue2, iValue3, iValue4, Outlut, iCount )
/************************************************************************/
int	iValue1, iValue2, iValue3, iValue4, iCount;
char	*Outlut;
{
int	i;

iCount /= sizeof( *Outlut );
for ( i=0; i<iCount; i++ )
	{
	if ( i >= iValue1 && i <= iValue2 )
		*Outlut++ = 1;
	else
	if ( i >= iValue3 && i <= iValue4 )
		*Outlut++ = 1;
	else	*Outlut++ = 0;
	}
}


/************************************************************************/
VOID nullify( pBuffer, iCount )
/************************************************************************/
char	*pBuffer;
int	iCount;
{
int	flag;

flag = ON;
while ( --iCount >= 0 )
	{
	if ( *pBuffer == '(' )
		 flag = OFF;
	else
	if ( *pBuffer == ')' )
		 flag = ON;
	if ( flag && 
	   ( *pBuffer == '-' || ( *pBuffer >= '0' && *pBuffer <= '9' ) ) )
		 pBuffer++;
	else	*pBuffer++ = '\0';
	}
}

/************************************************************************/
int DLL_AstralClockCursor( part, whole )
/************************************************************************/
int part, whole;
{
return(AstralClockCursor(part, whole));
}

/************************************************************************/
LPTR DLL_CachePtr( Depth, x, y, bModify )
/************************************************************************/
int Depth, x, y;
BOOL bModify;
{
return(CachePtr(Depth, x, y, bModify));
}

/************************************************************************/
LPTR DLL_frame_set( lpFrame )
/************************************************************************/
LPTR lpFrame;
{
return((LPTR)frame_set((LPFRAME)lpFrame));
}

/************************************************************************/
int DLL_FilterWrite( span, pixels, count )
/************************************************************************/
LPSPAN	span;
LPTR	pixels;
int	count;
{
LPFRAME lpOldFrame;
LPTR lpSrc, lpDst;
int dx, depth, mdx;

if (lpMask)
	{
	depth = DEPTH;
	dx = rMask.left * depth;
	mdx = RectWidth(&rMask);
	lpOldFrame = frame_set(lpSrcFrame);
	lpSrc = CachePtr(0, 0, span->sy, NO);
	frame_set(lpOldFrame);
	lpDst = CachePtr(0, 0, span->sy, YES);
	if (lpSrc && lpDst)
		{
		copy(lpSrc, lpDst, lpSrcFrame->Xsize*depth);
		lpDst += dx;
		mload(rMask.left, span->sy, mdx, 1, lpMask, lpDst, lpDstFrame);
		setifset(lpMask, mdx, 255);
		ProcessData(lpDst, pixels, lpMask, mdx, depth);
		return(TRUE);
		}
	return(FALSE);
	}
else
	{
        return(CacheWrite(span, pixels, count));
	}
}

/************************************************************************/
int DLL_frame_getRGB( lpData, lpRGB )
/************************************************************************/
LPTR lpData;
LPRGB lpRGB;
{
frame_getRGB(lpData, lpRGB);
}

/************************************************************************/
int DLL_frame_putRGB( lpData, lpRGB )
/************************************************************************/
LPTR lpData;
LPRGB lpRGB;
{
frame_putRGB(lpData, lpRGB);
}

