// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®FD1¯®PL1¯®TP0¯®BT0¯*/
#include "pp.h"
#include "id.h"

/* Directional flags for automasking */
#define N	1
#define NE	2
#define E	4
#define SE	8
#define S	16
#define SW	32
#define W	64
#define NW	128
#define NOT_N	SW|S|SE
#define NOT_NE	W|SW|S
#define NOT_E	NW|W|SW
#define NOT_SE	W|NW|N
#define NOT_S	NW|N|NE
#define NOT_SW	N|NE|E
#define NOT_W	NE|E|SE
#define NOT_NW	E|SE|S

#define MAX_HUES 252 /* must be divisible by 6 */
#define FIXHUE(hue) \
	((hue<0) ? hue+MAX_HUES:((hue>=MAX_HUES) ? hue-MAX_HUES: hue))

// proc type for distance computation
typedef long (FAR *LPDISTPROC)(LPTR lpV1, LPTR lpV2);

static BOOL fAbortAutoMask;
static LPFRAME lpFrame;
static LPIMAGE lpImage;
static BOOL fMultipleObjects;
static LPTR lpData1, lpData2;
static LPRGB lpRGB;
static LPDISTPROC pDistance;
static int XSize, YSize;

long EdgeValue8( int x, int y, int FromDir, int dx, int dy, int depth);
long EdgeValue24( int x, int y, int FromDir, int dx, int dy, int depth);
long EdgeValue32( int x, int y, int FromDir, int dx, int dy, int depth);

/************************************************************************/
BOOL automask(HWND hWnd, int x, int y, LPCPoly lpPoly, int idColorModel, BOOL bUpdateVisual)
/************************************************************************/
{
	int x1, x2, y1, y2, depth;
	int lmx, lmy, mvx, mvy, fdx, fdy, fmx, fmy;
	int Direction, LastDirection, MinVectorSize;
	POINT pt1, pt2;
	long (far *lpEdgeValueProc)(int,int,int,int,int,int);
	
	lpData1 = lpData2 = NULL;
	if (!lpPoly)
		return(FALSE);
	// get last point  
	if (!lpPoly->LastPoint(&pt1))
	{
		pt1.x = x;
		pt1.y = y;
		lpPoly->AddPoint(&pt1);
		return(TRUE); 
	}
	fmx = pt1.x;
	fmy = pt1.y;
	
	if (!(lpImage = GetImagePtr(hWnd)))
		return(FALSE);
	lpFrame = ImgGetBaseEditFrame(lpImage);
	switch(FrameType(lpFrame))
	{
		case FDT_LINEART :
		case FDT_GRAYSCALE :
			depth = 1;
			lpEdgeValueProc = EdgeValue8;
		break;
	
	   	case FDT_RGBCOLOR:
			depth = 3;
			lpEdgeValueProc = EdgeValue24;
			switch (idColorModel+IDC_FIRST_MODEL)
			{
				case IDC_MODEL_RGB:
					pDistance = Distance24;
				break;
				case IDC_MODEL_HSL:
					pDistance = Distance24HSL;
				break;
				case IDC_MODEL_HUE:
					pDistance = Distance24H;
				break;
				case IDC_MODEL_SAT:
					pDistance = Distance24S;
				break;
				case IDC_MODEL_LUM:
					pDistance = Distance24L;
				break;
				case IDC_MODEL_HS:
					pDistance = Distance24HS;
				break;
				case IDC_MODEL_SL:
					pDistance = Distance24SL;
				break;
				case IDC_MODEL_HL:
					pDistance = Distance24HL;
				break;
				default:
					pDistance = Distance24;
				break;
			}
		break;
	
		case FDT_CMYKCOLOR:
			depth = 4;
			lpEdgeValueProc = EdgeValue32;
			switch (idColorModel+IDC_FIRST_MODEL)
			{
				case IDC_MODEL_RGB:
					pDistance = Distance32;
				break;
				case IDC_MODEL_HSL:
					pDistance = Distance32HSL;
				break;
				case IDC_MODEL_HUE:
					pDistance = Distance32H;
				break;
				case IDC_MODEL_SAT:
					pDistance = Distance32S;
				break;
				case IDC_MODEL_LUM:
					pDistance = Distance32L;
				break;
				case IDC_MODEL_HS:
					pDistance = Distance32HS;
				break;
				case IDC_MODEL_SL:
					pDistance = Distance32SL;
				break;
				case IDC_MODEL_HL:
					pDistance = Distance32HL;
				break;
				default:
					pDistance = Distance32;
				break;
			}
		break;
	
	    default:
		return( FALSE );
	    }
	
	XSize = FrameXSize(lpFrame);
	YSize = FrameYSize(lpFrame);
	fMultipleObjects = ImgMultipleObjects(lpImage);
	if (fMultipleObjects)
	{
		if (!(lpData1 = Alloc((long)XSize*depth)))	
			goto ExitError;
		if (!(lpData2 = Alloc((long)XSize*depth)))
			goto ExitError;
	}
	
	fAbortAutoMask = FALSE;
	while (LBUTTON)
		;  // make user let up button so we can use it for cancel
	
	
	/* Compute how far to the end point */
	fdx = x - fmx;
	fdy = y - fmy;
	Direction = GoingAway( fdx, fdy );
	LastDirection = direction( fdx, fdy );
	MinVectorSize = Mask.AutoMinEdge;
	lmx = lmy = 9999;
	while ( (abs(fdx) > 3 || abs(fdy) > 3) && !fAbortAutoMask)
	{
		Direction = (*lpEdgeValueProc)( fmx, fmy, Direction, fdx, fdy,
					     Mask.AutoSearchDepth);
		if ( !Direction )
		        Direction = LastDirection;
		else	LastDirection = Direction;
	
		if ( Direction == N )
			{ fmy--; fdy++;            Direction = NOT_N; } 
		else
		if ( Direction == NE )
			{ fmy--; fdy++; fmx++; fdx--; Direction = NOT_NE; } 
		else
		if ( Direction == E )
			{ fmx++; fdx--; Direction = NOT_E; } 
		else
		if ( Direction == SE )
			{ fmy++; fdy--; fmx++; fdx--; Direction = NOT_SE; } 
		else
		if ( Direction == S )
			{ fmy++; fdy--;            Direction = NOT_S; } 
		else
		if ( Direction == SW )
			{ fmy++; fdy--; fmx--; fdx++; Direction = NOT_SW; } 
		else
		if ( Direction == W )
			{ fmx--; fdx++; Direction = NOT_W; } 
		else
		if ( Direction == NW )
			{ fmy--; fdy++; fmx--; fdx++; Direction = NOT_NW; }
	
		fmx = mbound(fmx, 0, lpImage->npix-1);
	       	fmy = mbound(fmy, 0, lpImage->nlin-1);
		if ( --MinVectorSize > 0 )
			continue;
	
		lpPoly->LastPoint(&pt1);
		if (fmx != pt1.x || fmy != pt1.y)
		{
			mvx = fmx - pt1.x;
			mvy = fmy - pt1.y;
	
			if ( mvx == lmx && mvy == lmy )
			{ /* Replace the last vector */
				if (lpPoly->NumPoints() > 1)
				{
					lpPoly->PreviousPoint(&pt2);
					// last point
					x2 = pt1.x; y2 = pt1.y;
					x1 = pt2.x; y1 = pt2.y;
	           		if (bUpdateVisual)
		           		DrawGuideLine(hWnd, 0, x1, y1, x2, y2, OFF );
	           		if (bUpdateVisual)
			        	DrawGuideLine(hWnd, 0, x1, y1, fmx, fmy, ON );
				}
				pt1.x = fmx;
				pt1.y = fmy;
				lpPoly->LastPoint();
				lpPoly->SetCurrentPoint(&pt1);
			}
			else	
			{ 
				/* Capture the new vector */
				x1 = pt1.x; y1 = pt1.y;
	           	if (bUpdateVisual)
			        DrawGuideLine(hWnd, 0, x1, y1, fmx, fmy, ON );
			        /* If the new vector extends the last one, replace it */
				lmx = mvx;
				lmy = mvy;
				pt1.x = fmx;
				pt1.y = fmy;
				if (!lpPoly->AddPoint(&pt1))
					goto ExitError;
			}
		}
	}
	/* Draw a new guide line */
	//lpPoly->GetLastPoint(&pt1);
	//fmx = pt1.x;
	//fmy = pt1.y;
	//if (bUpdateVisual)
	//	DrawGuideLine( hWnd, 0, fmx, fmy, x, y, ON );

	if (fMultipleObjects)
	{
		FreeUp(lpData1);
		FreeUp(lpData2);
	}
	return(fAbortAutoMask);

ExitError:
	Message(IDS_EMEMALLOC);
	if (lpData1)
		FreeUp(lpData1);
	if (lpData2)
		FreeUp(lpData2);
	return(FALSE);
}

#define GetNewValue8(x1,y1,x2,y2) 									\
{																	\
if ((x1) >= 0 && (x1) <= XSize && (y1) >= 0 && (y1) <= YSize &&		\
	(x2) >= 0 && (x2) <= XSize && (y2) >= 0 && (y2) <= YSize)		\
	{																\
	if (fMultipleObjects)											\
		{															\
		ImgGetLine(lpImage, NULL, x1, y1, 1, lpData1);				\
		ImgGetLine(lpImage, NULL, x2, y2, 1, lpData2);				\
		}															\
	else															\
		{															\
 	   	lpData1 = FramePointer(lpFrame, x1, y1, NO);				\
    	lpData2 = FramePointer(lpFrame, x2, y2, NO);				\
		}															\
	NewValue = abs( (int)*lpData1 - (int)*lpData2 );				\
	}																\
else																\
	NewValue = 1;													\
}

/************************************************************************/
long EdgeValue8( int x, int y, int FromDir, int dx, int dy, int depth )
/************************************************************************/
{
int DontGo, Value, Direction, NewValue, Go1, Go2;

//if (fAbortAutoMask || AstralAbort())
if (fAbortAutoMask || LBUTTON)
	{
	fAbortAutoMask = TRUE;
	return(0);
	}
DontGo = FromDir | GoingAway( dx, dy );
Value = 0;
Direction = 0;

if ( !(DontGo & N) )
	{
	GetNewValue8(x-1, y-1, x+1, y-1);
	if ( depth )
		NewValue += EdgeValue8( x, y-1, NOT_N, dx, dy+1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = N;
		}
	else
	if ( NewValue == Value )
		Direction |= N;
	}
if ( !(DontGo & NE) )
	{
	GetNewValue8(x, y-1, x+1, y);
	if ( depth )
		NewValue += EdgeValue8( x+1, y-1, NOT_NE, dx-1, dy+1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = NE;
		}
	else
	if ( NewValue == Value )
		Direction |= NE;
	}
if ( !(DontGo & E) )
	{
	GetNewValue8(x+1, y-1, x+1, y+1);
	if ( depth )
		NewValue += EdgeValue8( x+1, y, NOT_E, dx-1, dy, depth-1 );
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = E;
		}
	else
	if ( NewValue == Value )
		Direction |= E;
	}
if ( !(DontGo & SE) )
	{
	GetNewValue8(x+1, y, x, y+1);
	if ( depth )
		NewValue += EdgeValue8( x+1, y+1, NOT_SE, dx-1, dy-1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = SE;
		}
	else
	if ( NewValue == Value )
		Direction |= SE;
	}
if ( !(DontGo & S) )
	{
	GetNewValue8(x+1, y+1, x-1, y+1);
	if ( depth )
		NewValue += EdgeValue8( x, y+1, NOT_S, dx, dy-1, depth-1 );
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = S;
		}
	else
	if ( NewValue == Value )
		Direction |= S;
	}
if ( !(DontGo & SW) )
	{
	GetNewValue8(x, y+1, x-1, y);
	if ( depth )
		NewValue += EdgeValue8( x-1, y+1, NOT_SW, dx+1, dy-1, depth-1 );
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = SW;
		}
	else
	if ( NewValue == Value )
		Direction |= SW;
	}
if ( !(DontGo & W) )
	{
	GetNewValue8(x-1, y+1, x-1, y-1);
	if ( depth )
		NewValue += EdgeValue8( x-1, y, NOT_W, dx+1, dy, depth-1 );
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = W;
		}
	else
	if ( NewValue == Value )
		Direction |= W;
	}
if ( !(DontGo & NW) )
	{
	GetNewValue8(x, y-1, x-1, y);
	if ( depth )
		NewValue += EdgeValue8( x-1, y-1, NOT_NW, dx+1, dy+1, depth-1 );
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = NW;
		}
	else
	if ( NewValue == Value )
		Direction |= NW;
	}

if ( depth == Mask.AutoSearchDepth )
	{ /* The search is over */
	/* Break any ties by heading for the destination */
	Go1 = direction( dx, dy );
	if ( Value < 1 ) return( Go1 );
	Go2 = Go1;			  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	if ( (Go1*=2) == 256 ) Go1 = 1;	  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	if ( (Go1*=2) == 256 ) Go1 = 1;	  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	if ( (Go1*=2) == 256 ) Go1 = 1;	  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	}

return( Value );
}

#define GetNewValue24(x1,y1,x2,y2) 									\
{																	\
if ((x1) >= 0 && (x1) <= XSize && (y1) >= 0 && (y1) <= YSize &&		\
	(x2) >= 0 && (x2) <= XSize && (y2) >= 0 && (y2) <= YSize)		\
	{																\
	if (fMultipleObjects)											\
		{															\
		ImgGetLine(lpImage, NULL, x1, y1, 1, lpData1);				\
		ImgGetLine(lpImage, NULL, x2, y2, 1, lpData2);				\
		}															\
	else															\
		{															\
 	   	lpData1 = FramePointer(lpFrame, x1, y1, NO);				\
    	lpData2 = FramePointer(lpFrame, x2, y2, NO);				\
		}															\
	NewValue = (*pDistance)(lpData1, lpData2);					\
	}																\
else																\
	NewValue = 1;													\
}


/************************************************************************/
long EdgeValue24( int x, int y, int FromDir, int dx, int dy, int depth )
/************************************************************************/
{
int DontGo, Direction, Go1, Go2;
long NewValue, Value;

//if (fAbortAutoMask || AstralAbort())
if (fAbortAutoMask || LBUTTON)
    {
    fAbortAutoMask = TRUE;
    return(0);
    }
DontGo = FromDir | GoingAway( dx, dy );
Value = 0;
Direction = 0;

if ( !(DontGo & N) )
	{
	GetNewValue24(x-1, y-1, x+1, y-1);
	if ( depth )
		NewValue += EdgeValue24( x, y-1, NOT_N, dx, dy+1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = N;
		}
	else
	if ( NewValue == Value )
		Direction |= N;
	}
if ( !(DontGo & NE) )
	{
	GetNewValue24(x, y-1, x+1, y);
	if ( depth )
		NewValue += EdgeValue24( x+1, y-1, NOT_NE, dx-1, dy+1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = NE;
		}
	else
	if ( NewValue == Value )
		Direction |= NE;
	}
if ( !(DontGo & E) )
	{
	GetNewValue24(x+1, y-1, x+1, y+1);
	if ( depth )
		NewValue += EdgeValue24( x+1, y, NOT_E, dx-1, dy, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = E;
		}
	else
	if ( NewValue == Value )
		Direction |= E;
	}
if ( !(DontGo & SE) )
	{
	GetNewValue24(x+1, y, x, y+1);
	if ( depth )
		NewValue += EdgeValue24( x+1, y+1, NOT_SE, dx-1, dy-1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = SE;
		}
	else
	if ( NewValue == Value )
		Direction |= SE;
	}
if ( !(DontGo & S) )
	{
	GetNewValue24(x+1, y+1, x-1, y+1);
	if ( depth )
		NewValue += EdgeValue24( x, y+1, NOT_S, dx, dy-1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = S;
		}
	else
	if ( NewValue == Value )
		Direction |= S;
	}
if ( !(DontGo & SW) )
	{
	GetNewValue24(x, y+1, x-1, y);
	if ( depth )
		NewValue += EdgeValue24( x-1, y+1, NOT_SW, dx+1, dy-1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = SW;
		}
	else
	if ( NewValue == Value )
		Direction |= SW;
	}
if ( !(DontGo & W) )
	{
	GetNewValue24(x-1, y+1, x-1, y-1);
	if ( depth )
		NewValue += EdgeValue24( x-1, y, NOT_W, dx+1, dy, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = W;
		}
	else
	if ( NewValue == Value )
		Direction |= W;
	}
if ( !(DontGo & NW) )
	{
	GetNewValue24(x, y-1, x-1, y);
	if ( depth )
		NewValue += EdgeValue24( x-1, y-1, NOT_NW, dx+1, dy+1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = NW;
		}
	else
	if ( NewValue == Value )
		Direction |= NW;
	}

if ( depth == Mask.AutoSearchDepth )
	{ /* The search is over */
	/* Break any ties by heading for the destination */
	Go1 = direction( dx, dy );
	if ( Value < 1 ) return( Go1 );
	Go2 = Go1;			  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	if ( (Go1*=2) == 256 ) Go1 = 1;	  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	if ( (Go1*=2) == 256 ) Go1 = 1;	  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	if ( (Go1*=2) == 256 ) Go1 = 1;	  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	}

return( Value );
}

/************************************************************************/
long Distance24( LPTR lpV1, LPTR lpV2 )
/************************************************************************/
{
long rError, gError, bError;

rError = (int)*lpV1++ - (int)*lpV2++;
rError *= rError;
gError = (int)*lpV1++ - (int)*lpV2++;
gError *= gError;
bError = (int)*lpV1 - (int)*lpV2;
bError *= bError;
return(rError+gError+bError);
}

/************************************************************************/
long Distance24HSL( LPTR lpV1, LPTR lpV2 )
/************************************************************************/
{
long hError1, hError2, sError, lError;
HSLS hsl1, hsl2;


RGBtoHSL(((LPRGB)lpV1)->red,((LPRGB)lpV1)->green,((LPRGB)lpV1)->blue, &hsl1);
RGBtoHSL(((LPRGB)lpV2)->red,((LPRGB)lpV2)->green,((LPRGB)lpV2)->blue, &hsl2);
hError1 = (int)hsl1.hue - (int)hsl2.hue;
hError2 = -hError1;
// get into range [0,360) degrees
hError1 = FIXHUE(hError1);
hError2 = FIXHUE(hError2);
hError1 = Min(hError1, hError2);
hError1 = hError1*hError1;
sError = (int)hsl1.sat - (int)hsl2.sat;
sError *= sError;
lError = (int)hsl1.lum - (int)hsl2.lum;
lError *= lError;
return(hError1+sError+lError);
}



/************************************************************************/
long Distance24H( LPTR lpV1, LPTR lpV2 )
/************************************************************************/
{
long hError1, hError2;
HSLS hsl1, hsl2;


RGBtoH(((LPRGB)lpV1)->red,((LPRGB)lpV1)->green,((LPRGB)lpV1)->blue, &hsl1);
RGBtoH(((LPRGB)lpV2)->red,((LPRGB)lpV2)->green,((LPRGB)lpV2)->blue, &hsl2);
hError1 = (int)hsl1.hue - (int)hsl2.hue;
hError2 = -hError1;
// get into range [0,360) degrees
hError1 = FIXHUE(hError1);
hError2 = FIXHUE(hError2);
hError1 = Min(hError1, hError2);
return(hError1*hError1);
}

/************************************************************************/
long Distance24L( LPTR lpV1, LPTR lpV2 )
/************************************************************************/
{
long lError;
int light1, light2;


light1 = RGBtoL((LPRGB)lpV1);
light2 = RGBtoL((LPRGB)lpV2);
lError = light1-light2;
return(lError*lError);
}

/************************************************************************/
long Distance24SL( LPTR lpV1, LPTR lpV2 )
/************************************************************************/
{
long sError, lError;
HSLS hsl1, hsl2;

RGBtoSL(((LPRGB)lpV1)->red,((LPRGB)lpV1)->green,((LPRGB)lpV1)->blue, &hsl1);
RGBtoSL(((LPRGB)lpV2)->red,((LPRGB)lpV2)->green,((LPRGB)lpV2)->blue, &hsl2);
sError = (int)hsl1.sat - (int)hsl2.sat;
sError *= sError;
lError = (int)hsl1.lum - (int)hsl2.lum;
lError *= lError;
return(sError+lError);
}

/************************************************************************/
long Distance24HS( LPTR lpV1, LPTR lpV2 )
/************************************************************************/
{
long hError1, hError2, sError;
HSLS hsl1, hsl2;

RGBtoHS(((LPRGB)lpV1)->red,((LPRGB)lpV1)->green,((LPRGB)lpV1)->blue, &hsl1);
RGBtoHS(((LPRGB)lpV2)->red,((LPRGB)lpV2)->green,((LPRGB)lpV2)->blue, &hsl2);

hError1 = (int)hsl1.hue - (int)hsl2.hue;
hError2 = -hError1;

// get into range [0,360) degrees
hError1 = FIXHUE(hError1);
hError2 = FIXHUE(hError2);
hError1 = Min(hError1, hError2);
hError1 = hError1*hError1;
sError = (int)hsl1.sat - (int)hsl2.sat;
sError *= sError;
return(hError1+sError);
}

/************************************************************************/
long Distance24HL( LPTR lpV1, LPTR lpV2 )
/************************************************************************/
{
long hError1, hError2, lError;
HSLS hsl1, hsl2;

RGBtoHL(((LPRGB)lpV1)->red,((LPRGB)lpV1)->green,((LPRGB)lpV1)->blue, &hsl1);
RGBtoHL(((LPRGB)lpV2)->red,((LPRGB)lpV2)->green,((LPRGB)lpV2)->blue, &hsl2);

hError1 = (int)hsl1.hue - (int)hsl2.hue;
hError2 = -hError1;
// get into range [0,360) degrees
hError1 = FIXHUE(hError1);
hError2 = FIXHUE(hError2);
hError1 = Min(hError1, hError2);
hError1 = hError1*hError1;
lError = (int)hsl1.lum - (int)hsl2.lum;
lError *= lError;
return(hError1+lError);
}

/************************************************************************/
long Distance24S( LPTR lpV1, LPTR lpV2 )
/************************************************************************/
{
long sError;
HSLS hsl1, hsl2;

RGBtoS(((LPRGB)lpV1)->red,((LPRGB)lpV1)->green,((LPRGB)lpV1)->blue, &hsl1);
RGBtoS(((LPRGB)lpV2)->red,((LPRGB)lpV2)->green,((LPRGB)lpV2)->blue, &hsl2);

sError = (int)hsl1.sat - (int)hsl2.sat;
return(sError*sError);
}

#define GetNewValue32(x1,y1,x2,y2) 									\
{																	\
if ((x1) >= 0 && (x1) <= XSize && (y1) >= 0 && (y1) <= YSize &&		\
	(x2) >= 0 && (x2) <= XSize && (y2) >= 0 && (y2) <= YSize)		\
	{																\
	if (fMultipleObjects)											\
		{															\
		ImgGetLine(lpImage, NULL, x1, y1, 1, lpData1);				\
		ImgGetLine(lpImage, NULL, x2, y2, 1, lpData2);				\
		}															\
	else															\
		{															\
 	   	lpData1 = FramePointer(lpFrame, x1, y1, NO);				\
    	lpData2 = FramePointer(lpFrame, x2, y2, NO);				\
		}															\
	NewValue = Distance32(lpData1, lpData2);						\
	}																\
else																\
	NewValue = 1;													\
}

/************************************************************************/
long Distance32( LPTR lpV1, LPTR lpV2 )
/************************************************************************/
{
long cError, mError, yError, kError;

cError = (int)*lpV1++ - (int)*lpV2++;
cError *= cError;

mError = (int)*lpV1++ - (int)*lpV2++;
mError *= mError;

yError = (int)*lpV1++ - (int)*lpV2++;
yError *= yError;

kError = (int)*lpV1++ - (int)*lpV2++;
kError *= kError;

return(cError+mError+yError+kError);
}


/************************************************************************/
long Distance32HSL( LPTR lpV1, LPTR lpV2 )
/************************************************************************/
{
long hError1, hError2, sError, lError;
HSLS hsl1, hsl2;

CMYKtoHSL(
	((LPCMYK)lpV1)->c,
	((LPCMYK)lpV1)->m,
	((LPCMYK)lpV1)->y,
	((LPCMYK)lpV1)->k,
	&hsl1);

CMYKtoHSL(
	((LPCMYK)lpV2)->c,
	((LPCMYK)lpV2)->m,
	((LPCMYK)lpV2)->y,
	((LPCMYK)lpV2)->k,
	&hsl2);

hError1 = (int)hsl1.hue - (int)hsl2.hue;
hError2 = -hError1;
// get into range [0,360) degrees
hError1 = FIXHUE(hError1);
hError2 = FIXHUE(hError2);
hError1 = Min(hError1, hError2);
hError1 = hError1*hError1;
sError = (int)hsl1.sat - (int)hsl2.sat;
sError *= sError;
lError = (int)hsl1.lum - (int)hsl2.lum;
lError *= lError;
return(hError1+sError+lError);
}

/************************************************************************/
long Distance32H( LPTR lpV1, LPTR lpV2 )
/************************************************************************/
{
long hError1, hError2;
HSLS hsl1, hsl2;

CMYKtoH((LPCMYK)lpV1, &hsl1);
CMYKtoH((LPCMYK)lpV2, &hsl2);

hError1 = (int)hsl1.hue - (int)hsl2.hue;
hError2 = -hError1;
// get into range [0,360) degrees
hError1 = FIXHUE(hError1);
hError2 = FIXHUE(hError2);
hError1 = Min(hError1, hError2);
return(hError1*hError1);
}

/************************************************************************/
long Distance32L( LPTR lpV1, LPTR lpV2 )
/************************************************************************/
{
long lError;
int light1, light2;

light1 = CMYKtoL((LPCMYK)lpV1);
light2 = CMYKtoL((LPCMYK)lpV2);

lError = light1-light2;
return(lError*lError);
}

/************************************************************************/
long Distance32SL(LPTR lpV1, LPTR lpV2 )
/************************************************************************/
{
long sError, lError;
HSLS hsl1, hsl2;

CMYKtoSL((LPCMYK)lpV1, &hsl1);
CMYKtoSL((LPCMYK)lpV2, &hsl2);

sError = (int)hsl1.sat - (int)hsl2.sat;
sError *= sError;
lError = (int)hsl1.lum - (int)hsl2.lum;
lError *= lError;
return(sError+lError);
}

/************************************************************************/
long Distance32HS(LPTR lpV1, LPTR lpV2 )
/************************************************************************/
{
long hError1, hError2, sError;
HSLS hsl1, hsl2;

CMYKtoHS((LPCMYK)lpV1, &hsl1);
CMYKtoHS((LPCMYK)lpV2, &hsl2);

hError1 = (int)hsl1.hue - (int)hsl2.hue;
hError2 = -hError1;
// get into range [0,360) degrees
hError1 = FIXHUE(hError1);
hError2 = FIXHUE(hError2);
hError1 = Min(hError1, hError2);
hError1 = hError1*hError1;
sError = (int)hsl1.sat - (int)hsl2.sat;
sError *= sError;
return(hError1+sError);
}

/************************************************************************/
long Distance32HL(LPTR lpV1, LPTR lpV2 )
/***********************************************************************/
{
long hError1, hError2, lError;
HSLS hsl1, hsl2;

CMYKtoHL((LPCMYK)lpV1, &hsl1);
CMYKtoHL((LPCMYK)lpV2, &hsl2);

hError1 = (int)hsl1.hue - (int)hsl2.hue;
hError2 = -hError1;
// get into range [0,360) degrees
hError1 = FIXHUE(hError1);
hError2 = FIXHUE(hError2);
hError1 = Min(hError1, hError2);
hError1 = hError1*hError1;
lError = (int)hsl1.lum - (int)hsl2.lum;
lError *= lError;
return(hError1+lError);
}


/************************************************************************/
long Distance32S(LPTR lpV1, LPTR lpV2 )
/************************************************************************/
{
long sError;
HSLS hsl1, hsl2;

CMYKtoS((LPCMYK)lpV1, &hsl1);
CMYKtoS((LPCMYK)lpV2, &hsl2);

sError = (int)hsl1.sat - (int)hsl2.sat;
return(sError*sError);
}

/************************************************************************/
long EdgeValue32( int x, int y, int FromDir, int dx, int dy, int depth)
/************************************************************************/
{
int DontGo, Direction, Go1, Go2;
long NewValue, Value;

//if (fAbortAutoMask || AstralAbort())
if (fAbortAutoMask || LBUTTON)
    {
    fAbortAutoMask = TRUE;
    return(0);
    }
DontGo = FromDir | GoingAway( dx, dy );
Value = 0;
Direction = 0;

if ( !(DontGo & N) )
	{
	GetNewValue32(x-1, y-1, x+1, y-1);
	if ( depth )
		NewValue += EdgeValue32( x, y-1, NOT_N, dx, dy+1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = N;
		}
	else
	if ( NewValue == Value )
		Direction |= N;
	}
if ( !(DontGo & NE) )
	{
	GetNewValue32(x, y-1, x+1, y);
	if ( depth )
		NewValue += EdgeValue32( x+1, y-1, NOT_NE, dx-1, dy+1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = NE;
		}
	else
	if ( NewValue == Value )
		Direction |= NE;
	}
if ( !(DontGo & E) )
	{
	GetNewValue32(x+1, y-1, x+1, y+1);
	if ( depth )
		NewValue += EdgeValue32( x+1, y, NOT_E, dx-1, dy, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = E;
		}
	else
	if ( NewValue == Value )
		Direction |= E;
	}
if ( !(DontGo & SE) )
	{
	GetNewValue32(x+1, y, x, y+1);
	if ( depth )
		NewValue += EdgeValue32( x+1, y+1, NOT_SE, dx-1, dy-1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = SE;
		}
	else
	if ( NewValue == Value )
		Direction |= SE;
	}
if ( !(DontGo & S) )
	{
	GetNewValue32(x+1, y+1, x-1, y+1);
	if ( depth )
		NewValue += EdgeValue32( x, y+1, NOT_S, dx, dy-1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = S;
		}
	else
	if ( NewValue == Value )
		Direction |= S;
	}
if ( !(DontGo & SW) )
	{
	GetNewValue32(x, y+1, x-1, y);
	if ( depth )
		NewValue += EdgeValue32( x-1, y+1, NOT_SW, dx+1, dy-1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = SW;
		}
	else
	if ( NewValue == Value )
		Direction |= SW;
	}
if ( !(DontGo & W) )
	{
	GetNewValue32(x-1, y+1, x-1, y-1);
	if ( depth )
		NewValue += EdgeValue32( x-1, y, NOT_W, dx+1, dy, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = W;
		}
	else
	if ( NewValue == Value )
		Direction |= W;
	}
if ( !(DontGo & NW) )
	{
	GetNewValue32(x, y-1, x-1, y);
	if ( depth )
		NewValue += EdgeValue32( x-1, y-1, NOT_NW, dx+1, dy+1, depth-1);
	if ( NewValue > Value )
		{
		Value = NewValue;
		Direction = NW;
		}
	else
	if ( NewValue == Value )
		Direction |= NW;
	}

if ( depth == Mask.AutoSearchDepth )
	{ /* The search is over */
	/* Break any ties by heading for the destination */
	Go1 = direction( dx, dy );
	if ( Value < 1 ) return( Go1 );
	Go2 = Go1;			  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	if ( (Go1*=2) == 256 ) Go1 = 1;	  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	if ( (Go1*=2) == 256 ) Go1 = 1;	  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	if ( (Go1*=2) == 256 ) Go1 = 1;	  if ( Direction & Go1 ) return( Go1 );
	if ( (Go2/=2) == 0 )   Go2 = 128; if ( Direction & Go2 ) return( Go2 );
	}

return( Value );
}

/************************************************************************/
int GoingAway( int dx, int dy )
/************************************************************************/
{
int direction;

if (abs(dy) > abs(dx))
    {
    if (dx > 0)
        {
        if (dy > 0)
            direction = NOT_S | W;   		/* not S */
        else if (dy < 0)
        	direction = NOT_NE | SE; 		/* not NE */
        else 
        	direction = NOT_E | N | S;		/* not E*/
        }
    else if (dx < 0)
        {
        if (dy > 0)
            direction = NOT_SW | NW;		/* not SW */
        else if (dy < 0)
            direction = NOT_N | E;			/* not N*/
        else
            direction = NOT_W | N | S; 		/* not W */
        }
    else
        {
        if (dy > 0)
            direction = NOT_S | E | W;		/* not S */
        else
            direction = NOT_N | E | W;	 	/* not N */
        }
    }
else
    {
    if (dx > 0)
        {
        if (dy > 0)
            direction = NOT_SE | SW;		/* not SE */
        else if (dy < 0)
        	direction = NOT_E | S;			/* not E */
        else 
        	direction = NOT_E | N | S;		/* not E*/
        }
    else if (dx < 0)
        {
        if (dy > 0)
            direction = NOT_W | N;			/* not W */
        else if (dy < 0)
            direction = NOT_NW | NE;		/* not NW */
        else
            direction = NOT_W | N | S;		/* not W*/
        }
    else
        {
        if (dy > 0)
            direction = NOT_S | E | W;		/* not S */
        else
            direction = NOT_N | E | W;		/* not N */
        }
    }
return(direction);
}


/************************************************************************/
int direction( int dx, int dy )
/************************************************************************/
{
int direction;

if (abs(dy) > abs(dx))
    {
    if (dx > 0)
        {
        if (dy > 0)
            direction = S;
        else if (dy < 0)
        	direction = NE;
        else 
        	direction = E;
        }
    else if (dx < 0)
        {
        if (dy > 0)
            direction = SW;
        else if (dy < 0)
            direction = N;
        else
            direction = W;
        }
    else
        {
        if (dy > 0)
            direction = S;
        else
            direction = N;
        }
    }
else
    {
    if (dx > 0)
        {
        if (dy > 0)
            direction = SE;
        else if (dy < 0)
        	direction = E;
        else 
        	direction = E;
        }
    else if (dx < 0)
        {
        if (dy > 0)
            direction = W;
        else if (dy < 0)
            direction = NW;
        else
            direction = W;
        }
    else
        {
        if (dy > 0)
            direction = S;
        else
            direction = N;
        }
    }
return(direction);
}
