//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

static int clockcount;
static RECT rMask;

static int Convolve( LPFRAME lpFrame, KERNAL *pKernal, int total );
static void kernal8( LPTR Line1, LPTR Line2, LPTR Line3, int iCount, KERNAL *pKernal, LPTR Output );
static void kernal24( LPTR startLine1, LPTR startLine2, LPTR startLine3, int startiCount, KERNAL *pKernal, LPTR startOutput );
static void kernal32( LPTR startLine1, LPTR startLine2, LPTR startLine3, int startiCount, KERNAL *pKernal, LPTR startOutput );
static void BuildLut( int iValue, LPINT Lut, int iCount );
static void BuildOutLut( int iValue1, int iValue2, int iValue3, int iValue4, char *Outlut, int iCount );
static BOOL ConvolveLocal( LPFRAME lpFrame, LPRECT lpRect, LPTR lpOutout, KERNAL *pKernal );

/************************************************************************/
BOOL ConvolveData( LPFRAME lpFrame, LPINT lpValues )
/************************************************************************/
{
KERNAL	sKernal, *pKernal;

/* Build lookup tables (to make the multiplies faster) */
/* from the 9 coefficients */
pKernal = &sKernal;
BuildLut( lpValues[1], pKernal->lut1, sizeof( pKernal->lut1 ) );
BuildLut( lpValues[2], pKernal->lut2, sizeof( pKernal->lut2 ) );
BuildLut( lpValues[3], pKernal->lut3, sizeof( pKernal->lut3 ) );
BuildLut( lpValues[4], pKernal->lut4, sizeof( pKernal->lut4 ) );
BuildLut( lpValues[5], pKernal->lut5, sizeof( pKernal->lut5 ) );
BuildLut( lpValues[6], pKernal->lut6, sizeof( pKernal->lut6 ) );
BuildLut( lpValues[7], pKernal->lut7, sizeof( pKernal->lut7 ) );
BuildLut( lpValues[8], pKernal->lut8, sizeof( pKernal->lut8 ) );
BuildLut( lpValues[9], pKernal->lut9, sizeof( pKernal->lut9 ) );

/* Load the absolute value flag; either 0 (bound at 0) or 1 (abs) */
pKernal->absflag = lpValues[10];;

/* Load the kernal rotation count; either 0, 1, or 2 */
pKernal->rotation = lpValues[11];

/* Load the edge type flag; either 0 (no edge checking) or 1 */
pKernal->edgetype = lpValues[12];

/* Build the output lut based on 2 range pairs */
BuildOutLut( lpValues[13], lpValues[14], lpValues[15], lpValues[16],
	pKernal->outlut, sizeof( pKernal->outlut ) );

/* Do the job */
return( Convolve( lpFrame, pKernal, lpValues[0] /* # filters */ ) );
}

/************************************************************************/
BOOL FindEdges( LPFRAME lpFrame, LPRECT lpRect, LPTR lpOutput)
/************************************************************************/
{
#define MAXVALUES 17
KERNAL	sKernal, *pKernal;
int	iValues[MAXVALUES];

iValues[0] = 1;
iValues[1] = 1000;
iValues[2] = 1414;
iValues[3] = 1000;
iValues[4] = 0;
iValues[5] = 0;
iValues[6] = 0;
iValues[7] = -1000;
iValues[8] = -1414;
iValues[9] = -1000;
iValues[10] = 1;
iValues[11] = 2;
iValues[12] = 0;
iValues[13] = 0;
iValues[14] = 255;
iValues[15] = 0;
iValues[16] = 0;

/* Build lookup tables (to make the multiplies faster) */
/* from the 9 coefficients */
pKernal = &sKernal;
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
return(ConvolveLocal( lpFrame, lpRect, lpOutput, pKernal ));
}

/************************************************************************/
static BOOL ConvolveLocal( LPFRAME lpFrame, LPRECT lpRect, LPTR lpOutput, KERNAL *pKernal )
/************************************************************************/
{
int y1, y2, x1, x2, dx, dy, sy;
LPTR lpBuffer[3], Line1, Line2, Line3, Output, Temp;
WORD wWidth;
void (far *lpFilterProc)(LPTR,LPTR,LPTR,int,KERNAL*,LPTR);

if ( !lpFrame )
	return(FALSE);

switch ( FrameDepth(lpFrame))
    {
	 case 0:
    case 1:
	lpFilterProc = kernal8;
	break;
    case 3:
	lpFilterProc = kernal24;
	break;
	 case 4:
	lpFilterProc = kernal32;
	break;
    default:
	return( FALSE );
    }

y1 = bound( lpRect->top,    0, FrameYSize(lpFrame)-1 );
y2 = bound( lpRect->bottom, 0, FrameYSize(lpFrame)-1 );
x1 = bound( lpRect->left,   0, FrameXSize(lpFrame)-1 );
x2 = bound( lpRect->right,  0, FrameXSize(lpFrame)-1 );

dx = x2 - x1 + 1;
dy = y2 - y1 + 1;

if (!AllocLines(lpBuffer, 3, dx, 
	(FrameType(lpFrame) == FDT_LINEART) ? 1 : FrameDepth(lpFrame)))
	return(FALSE);

Line1  = lpBuffer[0];
Line2  = lpBuffer[1];
Line3  = lpBuffer[2];
Output = lpOutput;

wWidth = FrameDepth( lpFrame );
if (wWidth == 0) wWidth = 1;
wWidth *= (WORD)dx;

if ( !FrameRead( lpFrame, x1, y1, dx, Line1, dx ) )
	{
	FreeUp( lpBuffer[0] );
	return( FALSE );
	}
copy(Line1, Output, wWidth);
Output += wWidth;
if ( !FrameRead( lpFrame, x1, y1+1, dx, Line2, dx ) )
	{
	FreeUp( lpBuffer[0] );
	return( FALSE );
	}
for ( sy=y1+2; sy<=y2; sy++ )
	{
	/* Read the new line into the third buffer */
	if ( !FrameRead( lpFrame, x1, sy, dx, Line3, dx ) )
		{
		FreeUp( lpBuffer[0] );
		return( FALSE );
		}
	/* Run the kernal over the second buffer */
	/* Put the results in the output buffer */
	(*lpFilterProc)( Line1, Line2, Line3, dx, pKernal, Output );

	/* Write the line from the output buffer */
	Output += wWidth;
	Temp  = Line1;
	Line1 = Line2;
	Line2 = Line3;
	Line3 = Temp;
	}
copy(Line2, Output, wWidth);
FreeUp( lpBuffer[0] );
return( TRUE );
}

/************************************************************************/
static BOOL Convolve( LPFRAME lpFrame, KERNAL *pKernal, int total )
/************************************************************************/
{
int y1, y2, x1, x2, dx, dy, depth, sy;
LPTR lpBuffer[4], Line1, Line2, Line3, Output, Temp;
void (far *lpFilterProc)(LPTR,LPTR,LPTR,int,KERNAL*,LPTR);

if ( !lpFrame )
	return(FALSE);

lpBuffer[0] = NULL;	
ProgressBegin(1,0);

switch ( depth = FrameDepth(lpFrame) )
    {
	 case 0:
 	depth = 1;
    case 1:
	lpFilterProc = kernal8;
	break;
    case 3:
	lpFilterProc = kernal24;
	break;
	 case 4:
	lpFilterProc = kernal32;
	break;
    default:
		goto ExitFalse;
    }

y1 = 0;
y2 = FrameYSize(lpFrame)-1;
x1 = 0;
x2 = FrameXSize(lpFrame)-1;

dx = x2 - x1 + 1;
dy = y2 - y1 + 1;
total *= dy;

if (!AllocLines(lpBuffer, 4, dx, depth))
	goto ExitFalse;

Line1  = lpBuffer[0];
Line2  = lpBuffer[1];
Line3  = lpBuffer[2];
Output = lpBuffer[3];

if ( !FrameRead( lpFrame, x1, y1, dx, Line1, dx ) )
	goto ExitFalse;
if ( !FrameRead( lpFrame, x1, y1+1, dx, Line2, dx ) )
	goto ExitFalse;
for ( sy=y1+2; sy<=y2; sy++ )
	{
	AstralClockCursor( clockcount++, total, NO );
	/* Read the new line into the third buffer */
	if ( !FrameRead( lpFrame, x1, sy, dx, Line3, dx ) )
		goto ExitFalse;
	/* Run the kernal over the second buffer */
	/* Put the results in the output buffer */
	(*lpFilterProc)( Line1, Line2, Line3, dx, pKernal, Output );

	/* Write the line from the output buffer */
	sy--;
	if ( !FrameWrite( lpFrame, x1, sy, dx, Output, dx ) )
		goto ExitFalse;
	sy++;
	Temp  = Line1;
	Line1 = Line2;
	Line2 = Line3;
	Line3 = Temp;
	}

FreeUp( lpBuffer[0] );
ProgressEnd();
return( TRUE );

ExitFalse:
if (lpBuffer[0])
	FreeUp(lpBuffer[0]);
ProgressEnd();
return(FALSE);
}


/************************************************************************/
static VOID kernal8( LPTR Line1, LPTR Line2, LPTR Line3, int iCount,
                        KERNAL *pKernal, LPTR Output )
/************************************************************************/
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


/************************************************************************/
static VOID kernal24( LPTR startLine1, LPTR startLine2, LPTR startLine3,
                        int startiCount, KERNAL *pKernal, LPTR startOutput )
/************************************************************************/
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
static VOID kernal32( LPTR startLine1, LPTR startLine2, LPTR startLine3,
                        int startiCount, KERNAL *pKernal, LPTR startOutput )
/************************************************************************/
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
    Output += 4;
    iCount -= 2;

    Line10 = *Line1;
    Line11 = *(Line1+4);
    Line20 = *Line2;
    Line21 = *(Line2+4);
    Line30 = *Line3;
    Line31 = *(Line3+4);
    while ( --iCount >= 0 )
	    {
	    Line12 = *(Line1+8);
	    Line22 = *(Line2+8);
	    Line32 = *(Line3+8);
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
			    Output += 4;
			    Line1 += 4;		/* bump the line pointers */
			    Line2 += 4;
			    Line3 += 4;
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
	    Output += 4;
            Line10 = Line11;
            Line11 = Line12;
            Line20 = Line21;
            Line21 = Line22;
            Line30 = Line31;
            Line31 = Line32;
	    }

    /* Output the last value unchanged */
    Line2 += 4;
    *Output = *Line2;
    }
}

/************************************************************************/
static VOID BuildLut( int iValue, LPINT Lut, int iCount )
/************************************************************************/
{
int	i;
long	v;

v = (long)iValue << 3;
iCount /= sizeof( *Lut );
for ( i=0; i<iCount; i++ )
	*Lut++ = ( ((long)i * v) + 500L ) / 1000L;
}


/************************************************************************/
static VOID BuildOutLut( int iValue1, int iValue2, int iValue3, int iValue4,
                           char *Outlut, int iCount )
/************************************************************************/
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


