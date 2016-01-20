//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"

// Static prototypes
static LPTR CreateEdgeTable(int table_size, int FuzzyPixels);
static void BuildCircleMask(HPTR hpBrushMask, int BrushSize, int Feather);
static void BuildSquareMask(HPTR hpBrushMask, int BrushSize, int Feather);
static void BuildHorzMask(HPTR hpBrushMask, int BrushSize, int Feather);
static void BuildVertMask(HPTR hpBrushMask, int BrushSize, int Feather);
static void BuildSlashMask(HPTR hpBrushMask, int BrushSize, int Feather);
static void BuildBackSlashMask(HPTR hpBrushMask, int BrushSize, int Feather);

#define BRMASK(lpM,BrushSize,x,y) lpM[ ((DWORD)BrushSize * (DWORD)(y)) + (DWORD)(x) ]

#define THREAD_BASE	__declspec(thread)
//#define THREAD_BASE
static THREAD_BASE LPTR lpEdgeTable;

/************************************************************************/
void SetupBrushMask(LPMGXBRUSH lpBrush, FRMDATATYPE Type)
/************************************************************************/
{
int Feather;
int BrushVal;

if (!lpBrush)
	return;

if ( lpBrush->Shape == IDC_BRUSHCUSTOM )
	{
	SetupCustomBrushMask( lpBrush, lpBrush->ShapeName );
	return;
	}

//if (Type == FDT_LINEART || Type == FDT_PALETTECOLOR)
//	Feather = 0;
//else
	Feather = lpBrush->Feather;
	
if ( lpBrush->Shape == IDC_BRUSHSQUARE || lpBrush->Size == 1 ||
   ( lpBrush->Shape == IDC_BRUSHCIRCLE && lpBrush->Size < 3 ) )
	BuildSquareMask(lpBrush->BrushMask, lpBrush->Size, Feather);
else
if ( lpBrush->Shape == IDC_BRUSHCIRCLE )
	BuildCircleMask(lpBrush->BrushMask, lpBrush->Size, Feather);
else
if ( lpBrush->Shape == IDC_BRUSHHORIZONTAL )
	BuildHorzMask(lpBrush->BrushMask, lpBrush->Size, Feather);
else
if ( lpBrush->Shape == IDC_BRUSHVERTICAL )
	BuildVertMask( lpBrush->BrushMask, lpBrush->Size, BrushVal );
else
if ( lpBrush->Shape == IDC_BRUSHSLASH )
	BuildSlashMask( lpBrush->BrushMask, lpBrush->Size, BrushVal );
else
if ( lpBrush->Shape == IDC_BRUSHBACKSLASH )
	BuildBackSlashMask( lpBrush->BrushMask, lpBrush->Size, BrushVal );
}

/************************************************************************/
BOOL SetupCustomBrushMask( LPMGXBRUSH lpBrush, LPSTR lpExtName )
/************************************************************************/
{
int x, y, sx, sy, dx, dy, bx, by;
LPTR lpLine;
HPTR lpBM;
LPFRAME lpFrame;
FNAME szFileName;
FRMTYPEINFO TypeInfo;
static THREAD_BASE int CustomBrushSize;

if ( lpBrush->Shape != IDC_BRUSHCUSTOM )
	return( FALSE );

if ( !LookupExtFile( lpExtName, szFileName, IDN_CUSTOM ) )
	{
	return(FALSE);
//	lpFrame = NULL;
	}
else if (!FileExists(szFileName))
	{
	return(FALSE);
	}
else 
	{
	FrameSetTypeInfo(&TypeInfo, FDT_GRAYSCALE, NULL);
	CReadImage read(szFileName);
	if ( !(lpFrame = read.ReadFrame(TypeInfo)))
		{
		return(FALSE);
		}
	}

if ( !lpFrame )
	{
	dx = MAX_BRUSH_SIZE;
	dy = MAX_BRUSH_SIZE;
	}
else	{
	dx = FrameXSize(lpFrame);
	dy = FrameYSize(lpFrame);
	if ( dx > MAX_BRUSH_SIZE || dy > MAX_BRUSH_SIZE )
		Message( IDS_ECUSTOMBRUSH, MAX_BRUSH_SIZE );
	sx = sy = 0;
	if ( dx > MAX_BRUSH_SIZE )
		{ // take the center of the image if it is too big
		sx = ( dx - MAX_BRUSH_SIZE ) / 2;
		dx = MAX_BRUSH_SIZE;
		}
	if ( dy > MAX_BRUSH_SIZE )
		{ // take the center of the image if it is too big
		sy = ( dy - MAX_BRUSH_SIZE ) / 2;
		dy = MAX_BRUSH_SIZE;
		}
	}

CustomBrushSize = max( dx, dy );
lpBrush->Size = CustomBrushSize;
if (lpBrush->BrushMask)
	FreeUp(lpBrush->BrushMask);
lpBrush->BrushMask = Alloc((long)lpBrush->Size*(long)lpBrush->Size);
if (!lpBrush->BrushMask)
	{
	FrameClose(lpFrame);
	return(FALSE);
	}
hclr( lpBrush->BrushMask, (long)lpBrush->Size*(long)lpBrush->Size);

if ( lpFrame )
	{
	bx = ( CustomBrushSize - dx ) / 2;
	by = ( CustomBrushSize - dy ) / 2;
	lpBM = lpBrush->BrushMask;
	for (y = 0; y < dy; ++y)
		{
		if ( !(lpLine = FramePointer( lpFrame, sx, y+sy, NO)) )
			continue;
		for (x = 0; x < dx; ++x, lpLine++)
			BRMASK(lpBM, CustomBrushSize, bx+x, by+y) = *lpLine ^ 255;
		}
	FrameClose(lpFrame);
	}
return(TRUE);
}

/************************************************************************/
static LPTR CreateEdgeTable(int table_size, int FuzzyPixels)
/************************************************************************/
{
int	i, nEdges, iEntry, iLastEntry, value1, value2, delta;
LFIXED rate, offset, fraction;
static THREAD_BASE int lastFuzzyPixels = -1;
static THREAD_BASE BYTE Edge[] =	{	254,253,251,250,248,245,243,240,237,233,
								229,225,220,214,209,202,196,189,181,174,
								166,157,149,140,132,123,114,106,97,89,
								81,74,66,59,53,46,41,35,30,26,
								22,18,15,12,10,7,5,4,2,1};

if (FuzzyPixels == lastFuzzyPixels)
	return(lpEdgeTable);
lastFuzzyPixels = FuzzyPixels;

nEdges = sizeof(Edge);
iLastEntry = nEdges - 1;
rate = FGET(nEdges, FuzzyPixels);
offset = FDIV2(rate);

for (i = 0; i < FuzzyPixels; ++i)
	{
	iEntry = WHOLE(offset);
	value1 = 255 - Edge[iEntry];

	// if not last entry interpolate
	if (iEntry < iLastEntry)
		{
		value2 = 255 - Edge[iEntry+1];
		delta = value2 - value1;
		fraction = FRACTION(offset);
		delta = FMUL(delta, fraction);
		value1 += delta;
		}
	lpEdgeTable[i] = value1;	
	offset += rate;
	}
for (; i < table_size; ++i)
	lpEdgeTable[i] = 255;
return(lpEdgeTable);
}

/************************************************************************/
BOOL SetupBrushMaskBuffers(BOOL fInit)
/************************************************************************/
{
if (fInit)
	{
	lpEdgeTable = Alloc((long)MAX_BRUSH_SIZE);
	if (!lpEdgeTable)
		return(FALSE);
	return(TRUE);
	}
else
	{
	if (lpEdgeTable)
		{
		FreeUp(lpEdgeTable);
		lpEdgeTable = NULL;
		}
	return(TRUE);
	}
}

/************************************************************************/
static void BuildCircleMask(HPTR hpBrushMask, int BrushSize, int Feather)
/************************************************************************/
{
int x, y;
unsigned int ed, edlast;
LPTR lpEdgeTbl;
int FuzzyPixels;
double root, xd, yd;
double fed, fcenter, fraction, radius; 
int value1, value2, delta;

fcenter = ((double)BrushSize-1.0)/2.0;
radius = (double)BrushSize/2;

FuzzyPixels = (int)((( (double)Feather * radius ) / 100.0)+0.5);
lpEdgeTbl = CreateEdgeTable(MAX_BRUSH_SIZE, FuzzyPixels);

edlast = MAX_BRUSH_SIZE - 1;
for (y=0; y<BrushSize; y++)
	{
	yd = fcenter - (double)y;
	yd *= yd;
	for (x=0; x<BrushSize; x++)
		{
		xd = fcenter - (double)x;
		root = sqrt((xd*xd)+yd);

		if (root > radius)
			*hpBrushMask++ =  0;
		else
			{
			fed = fcenter - root;
			if (fed < 0)
				fed = 0;
			ed = (unsigned)fed;
			value1 = lpEdgeTable[ed];
			if (ed < edlast)
				{
				value2 = lpEdgeTable[ed+1];
				delta = value2 - value1;
				fraction = fed - (double)ed;
				delta = (int)((delta * fraction) + 0.5);
				value1 += delta;
				}
			*hpBrushMask++ = value1;
			}
		}
	}
}

/************************************************************************/
static void BuildSquareMask(HPTR hpBrushMask, int BrushSize, int Feather)
/************************************************************************/
{
int x, y;
unsigned int ed, edlast;
LPTR lpEdgeTbl;
int FuzzyPixels;
LFIXED xd, yd, ydiff, xdiff;
LFIXED fed, fcenter, fraction; 
int value1, value2, delta;

FuzzyPixels = ( (long)Feather * (BrushSize/2) ) / 100;
lpEdgeTbl = CreateEdgeTable(MAX_BRUSH_SIZE, FuzzyPixels);

fcenter = FGET(BrushSize-1, 2);
edlast = MAX_BRUSH_SIZE - 1;
for (y=0; y<BrushSize; y++)
	{
	yd = abs(fcenter - MAKEFIXED(y));
	ydiff = abs(fcenter - yd);
	for (x=0; x<BrushSize; x++)
		{
		xd = abs(fcenter - MAKEFIXED(x));
		xdiff = abs(fcenter - xd);
		if (xdiff < ydiff)
			fed = xdiff;
		else
			fed = ydiff;
		if (fed < 0)
			fed = 0;
		ed = WHOLE(fed);
		value1 = lpEdgeTbl[ed];
		if (ed < edlast)
			{
			value2 = lpEdgeTbl[ed+1];
			delta = value2 - value1;
			fraction = FRACTION(fed);
			delta = FMUL(delta, fraction);
			value1 += delta;
			}
		*hpBrushMask++ = value1;
		}
	}
}

/************************************************************************/
static void BuildHorzMask( HPTR hpBrushMask, int BrushSize, int Feather )
/************************************************************************/
{
int y, iCount;

hclr( hpBrushMask, (DWORD)BrushSize*(DWORD)BrushSize );
y = BrushSize / 2;
hpBrushMask += ((DWORD)y * (DWORD)BrushSize);
iCount = BrushSize;
while (--iCount >= 0)
	*hpBrushMask++ = 255;
}

/************************************************************************/
static void BuildVertMask( HPTR hpBrushMask, int BrushSize, int Feather )
/************************************************************************/
{
int x, iCount;

hclr( hpBrushMask, (DWORD)BrushSize*(DWORD)BrushSize );
x = BrushSize / 2;
hpBrushMask += x;
iCount = BrushSize;
while (--iCount >= 0)
	{
	*hpBrushMask = 255;
	hpBrushMask += BrushSize;
	}
}

/************************************************************************/
static void BuildSlashMask( HPTR hpBrushMask, int BrushSize, int Feather )
/************************************************************************/
{
int x, y;

hclr( hpBrushMask, (DWORD)BrushSize*(DWORD)BrushSize );
x = BrushSize - 1;
for ( y = 1; y < BrushSize; y++ )
	{
	BRMASK( hpBrushMask, BrushSize, x, y ) = 255;
	BRMASK( hpBrushMask, BrushSize, --x, y ) = 255;
	}
}

/************************************************************************/
static void BuildBackSlashMask( HPTR hpBrushMask, int BrushSize, int Feather )
/************************************************************************/
{
int x, y;

hclr( hpBrushMask, (DWORD)BrushSize*(DWORD)BrushSize );
x = 0;
for ( y = 1; y < BrushSize; y++ )
	{
	BRMASK( hpBrushMask, BrushSize, x, y ) = 255;
	BRMASK( hpBrushMask, BrushSize, ++x, y ) = 255;
	}
}



