//®FD1¯®PL1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include <windows.h>
#include "id.h"
#include "data.h"
#include "routines.h"

extern void sample_table( LPBYTE, int, int );

/************************************************************************/
/* Constants								*/
/************************************************************************/
#define FADE_LEN_MULT 5

typedef struct _brushrect
	{
	WORD	xTile;
	WORD	yTile;
	RECT	rTile;
	RECT	rEdit;
	BOOL	fEmpty;
	} BRUSHRECT, *PBRUSHRECT;

/************************************************************************/
/* Local Functions							*/
/************************************************************************/
static void BrushStrokeDDA(int dx, int dy);
static void BrushStroke(int dx, int dy);
static void ProcessRectList(int x, int y);
static void BlitRect(LPRECT lpRect, LPTR lpBrushMask);
static void BlitToFrame( int x, int y, int dh, int dv, LPTR lpData,
				LPRECT lpRect);
static void BlitToDisplay(LPRECT lpRect);
static void SetBrushOpacity(LPTR lpSrc, LPTR lpDst, WORD Opacity, int iCount);
static BOOL AllocBrushBuffers(LPMGXBRUSH lpBrush);
static void FreeUpBrushBuffers( void );
static void SetupBrush( LPMGXBRUSH lpBrush );
static BOOL SetupFrames(LPMGXBRUSH lpBrush, BOOL fBegin);
static void SetupFade( LPMGXBRUSH lpBrush );
static void FadeBrush( BOOL fSetupMask );
static void FadeOutBrush( BOOL fSetupMask );
static void ResetBrush( LPMGXBRUSH lpBrush );
void glitterRGB( LPRGB lpSrc, int count, int brush_size );
void glitter1( LPRGB lpRGB, ULONG offset_brush, int count, int brush_size );
void glitter2( LPRGB lpRGB, ULONG offset_brush, int count, int brush_size );
void swirlRGB( LPRGB lpSrc, int count, int brush_size );
void swirl( LPRGB lpRGB, BYTE color_val );
void spin_the_brush( void );
void save_fade_values( void );
void restore_fade_values( void );

/************************************************************************/
/* Static variables							*/
/************************************************************************/
static HWND		hWnd;		// the window containing the image
static LPPIXMAP	lpDstPixmap;
static PIXMAP	SourcePixmap;
static LPMASK	lpMask;
static int		iBltFrameDepth;
static int		iBltFrameXsize;
static int		iBltFrameYsize;
static int		xDstOffset;
static int		yDstOffset;
static FRMDATATYPE iBltFrameType;
static WORD		Spacing;
static BOOL		UsingMask;	// Using any mask?
static BOOL		fFirstStroke;	// First stroke?
static BOOL		fRight;
static BOOL		fBottom;
static int		BrushFull;	// Brush Size
static int		BrushHalf;	// Half Brush Size
static int		bh;			// brush buffer size horizontally
static int		bv;			// brush buffer size vertically
static WORD		bcount;		// brush buffer size in bytes
static int		BrushSize;	// Original Brush Size
static LPTR		lpSrc;		// Source buffer pointer
static LPTR		lpDst;		// Destination buffer pointer
static LPTR		lpMsk;		// Mask buffer pointer
static LPTR		lpBM;		// BrushMask pointer
static void	   ( *lpMaskProc )( int, int, int, int, HPTR, HPTR, LPMASK );	// Proc to load mask data
static void	   ( FAR *lpProcessProc )( LPTR, LPTR, LPTR, int );	// Proc to process image data
static void    ( *StrokeProc )( int, int );	// Proc to handle stroking
static RECT		rMax;		// Max area stroke can affect
static RECT		rUpdate;
static LPMGXBRUSH lpBrush;	// The brush description
static BRUSHRECT	bRects[4];
static int lr[4] = {0, 1, 3, 2};
static int ur[4] = {3, 0, 2, 1};
static int ul[4] = {2, 1, 3, 0};
static int ll[4] = {1, 0, 2, 3};
static int * pPtTable;

static int last_x_processed;
static int last_y_processed;
static int last_dx, last_dy;
static int drop_red, drop_green, drop_blue;

// stuff for handling mirror mode
static BRUSHRECT    bMRects[4];
static int mlast_x_processed;
static int mlast_y_processed;
static int iScreenX;
static int * pMPtTable;

// used by BlitRect
static RECT rLast;
static int iStartPattern;
static int iPattern = -1;
static int iDirection = -2;
static int iDirection2;
static BOOL fLastStroke;
static RECT rLRect;

// added for scatter and fade 
static LPINT	lpErr;		        // error buffer for scatter style
static void    ( *StylizeProc )( LPTR, LPTR, LPINT, int, int, int );	// Proc to handle brush style
static int		StylePressure;	    // original brush pressure
static LFIXED	FadeOut;	        // fade in/out value
static LFIXED	FadeBrushSize;	    // current brush size for fade
static LFIXED	FadeOpacity;	    // fade value for opacity
static LFIXED	FadeOpacityValue;   // current opacity for fade
static int		Opacity;	        // original brush opacity
static int		LastOpacity;	    // original brush opacity
static int 		MaxOpacity;
static int		brush_x;	        // Brush x-position
static int		brush_y;	        // Brush y-position

// mirrored fade variables
static int		MBrushSize;	        
static LFIXED	MFadeBrushSize;	    
static int		MLastOpacity;	    
static LFIXED	MFadeOpacityValue;  
static int		MBrushFull;	        
static int		MBrushHalf;	        

/************************************************************************/
HMGXBRUSH CreateSimpleBrush( HMGXBRUSH hBrush )
/************************************************************************/
{
LPMGXBRUSH lpBrush;

if (lpImage)
	DisplayBrush(0, 0, 0, OFF);
if ( hBrush )
	DestroySimpleBrush( hBrush );
// Allocate memory for brush
if (!(lpBrush = (LPMGXBRUSH)Alloc((long)sizeof(MGXBRUSH))))
	{
	Message(IDS_EMEMALLOC);
	return(NULL);
	}
clr((LPTR)lpBrush, sizeof(MGXBRUSH));

lpBrush->SourceColor = SC_ACTIVE_COLOR;
lpBrush->SourceFrame = NULL;
lpBrush->ColorStruct.gray = 0;
lpBrush->ColorStruct.ColorSpace = CS_GRAY;
SetColorInfo(&lpBrush->ColorStruct, &lpBrush->ColorStruct,
			lpBrush->ColorStruct.ColorSpace);
lpBrush->StrokeMethod = SM_CONNECTED;
lpBrush->Shape = IDC_BRUSHCIRCLE;
lpBrush->ShapeName[0] = '\0';
lpBrush->Size = 20;
lpBrush->Feather = 0;
lpBrush->Opacity = 255;
lpBrush->MergeMode = MM_NORMAL;
lpBrush->Spacing = 0;
lpBrush->fMirrorMode = NO;
lpBrush->StylePressure = 50;
lpBrush->FadeSize = 0;
lpBrush->FadeOpacity = 0;
lpBrush->OpacityScale = 100;

if (lpImage && Window.hCursor == Window.hNullCursor)
	DisplayBrush(lpImage->hWnd, 32767, 32767, ON);

return( (HMGXBRUSH)lpBrush );
}

/************************************************************************/
void DestroySimpleBrush(HMGXBRUSH hBrush)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (!hBrush)
	return;
lpBrush = (LPMGXBRUSH)hBrush;
if (lpBrush->SourceFrame)
	FrameClose(lpBrush->SourceFrame);
FreeUp((LPTR)hBrush);
}

/************************************************************************/
BOOL SimpleBrushBegin(HWND hWndIn, LPPIXMAP lpPixmapIn,
     				  int xOffset, int yOffset,
					  HMGXBRUSH hBrush)
/************************************************************************/
{
int i;

fLastStroke = NO;

// Call End function to make sure things get reset
SimpleBrushEnd(NO);
//SimpleBrushEnd(YES);

// Convert brush handle to brush pointer for our use
if ( !(lpBrush = (LPMGXBRUSH)hBrush) )
	return(FALSE);

// Save passed in values for later use
hWnd = hWndIn;
lpDstPixmap = lpPixmapIn;
xDstOffset = xOffset;
yDstOffset = yOffset;

// Make sure there are no outstanding WM_PAINT's to avoid strange display
AstralUpdateWindow(hWnd);

if (!ImgPaintSetup(lpImage, NO))
	{
	lpBrush = NULL;
	return(FALSE);
	}
AstralSetRectEmpty(&rUpdate);

// Set frame mode to free oldest lines first instead of newest
// Assumption is that your typically brushing in the same area
if (!SetupFrames(lpBrush, YES))
	{
	lpBrush = NULL;
	return(FALSE);
	}

// setup fade for brush size and brush opacity
SetupFade( lpBrush );

// Allocate all memory needed for brushing
if (!AllocBrushBuffers(lpBrush))
	{
	lpBrush = NULL;
	return(FALSE);
	}

// Setup the Depth and Brush dependent procs
if (iBltFrameDepth == 3)
	lpMaskProc = ( void ( * )( int, int, int, int, HPTR, HPTR, LPMASK ) )mload24;
// OLD	lpMaskProc = mload24;
else
if (iBltFrameDepth == 4)
	lpMaskProc = ( void ( * )( int, int, int, int, HPTR, HPTR, LPMASK ) )mload32;
// OLD	lpMaskProc = mload32;
else
	lpMaskProc = ( void ( * )( int, int, int, int, HPTR, HPTR, LPMASK ) )mload8;
// OLD	lpMaskProc = mload8;
lpProcessProc = GetProcessProc(lpBrush->MergeMode, iBltFrameDepth);

if (lpBrush->StrokeMethod == SM_CONNECTED)
	StrokeProc = BrushStrokeDDA;
else
if (lpBrush->StrokeMethod == SM_UNCONNECTED)
	StrokeProc = BrushStroke;
else
	StrokeProc = BrushStrokeMyDDA;

if (lpBrush->Style == IDC_BRUSHSCATTER)
	StylizeProc = ScatterMask;
else
	StylizeProc = NULL;

// Load up the source color
if (lpBrush->SourceColor == SC_ACTIVE_COLOR)
	GetActiveColorFromType(iBltFrameType, &lpBrush->ColorStruct);
else if (lpBrush->SourceColor == SC_ALTERNATE_COLOR)
	GetAlternateColorFromType(iBltFrameType, &lpBrush->ColorStruct);
LoadColor(PixmapFrame(lpDstPixmap, PMT_EDIT), lpSrc, bcount,
    &lpBrush->ColorStruct);

// Setup mask state, geometrical mask and color shield
lpMask = ImgGetMask(lpImage);
UsingMask = (lpMask != NULL) || (ColorMask.Mask && ColorMask.On);

// Setup the brush mask
SetupBrush(lpBrush);

// Indicate the first stroke is coming
fFirstStroke = YES;

SetRect(&rMax, 0, 0, iBltFrameXsize-1, iBltFrameYsize-1);

if ( lpBrush->fSourceDirectional )
	{
	GridSave();
	i = lpBrush->Size;
	Grid( ON/*bGrid*/, i/2, i/2, i, i );
	}

return(TRUE);
}

/************************************************************************/
void SimpleBrushStroke(int dx, int dy, int curs_x, int curs_y)
/************************************************************************/
{
if (!lpBrush)
	return;

dx -= xDstOffset;
dy -= yDstOffset;
//ImgPaintSetup(lpImage, NO);

brush_x = curs_x;
brush_y = curs_y;

if ( Window.hCursor == Window.hNullCursor )
	DisplayBrush(0, 0, 0, OFF);

(*StrokeProc)(dx, dy);

if ( Window.hCursor == Window.hNullCursor )
	DisplayBrush(hWnd, curs_x, curs_y, ON);
}

/************************************************************************/
BOOL SimpleBrushActive()
/************************************************************************/
{
return(lpBrush != NULL);
}

/************************************************************************/
void SimpleBrushEnd(BOOL fNoBrushDisplay)
/************************************************************************/
{
int i, fx, x, fy, y;

if (!lpBrush)
	return;

if (! fNoBrushDisplay && lpBrush->fSourceEndPatterns)
{
	switch (iDirection2)
	{
	case 0:		// left
		x = last_dx -24;
		y = last_dy;
		break;
	case 1:		// upper-left
		x = last_dx -24;
		y = last_dy -24;
		break;
	case 2:		// up
		y = last_dy -24;
		x = last_dx;
		break;
	case 3:		// upper-right
		y = last_dy +24;
		x = last_dx +24;
		break;
	case 4:		// right
		x = last_dx +24;
		y = last_dy;
		break;
	case 5:		// lower-right
		x = last_dx +24;
		y = last_dy +24;
		break;
	case 6:		// down
		y = last_dy -24;
		x = last_dx;
		break;
	case 7:		// lower-left
		y = last_dy -24;
		x = last_dx -24;
		break;
	default:
		x = last_dx +24;
		y = last_dy;
		break;
	}
	fx = x;
	fy = y;
	Display2File (&fx, &fy);
	fLastStroke = YES;
	SimpleBrushStroke (fx, fy, x, y);
	fLastStroke = NO;
}

if ( lpBrush->fSourceDirectional )
	GridRestore();

BlitRect( NULL, NULL );
for (i = 0; i < 4; ++i)
	{
	if (bRects[i].fEmpty)
		continue;
	BlitToDisplay(&bRects[i].rEdit);
	bRects[i].fEmpty = YES;
	}
ResetBrush( lpBrush );
SetupFrames(lpBrush, NO);
FreeUpBrushBuffers();
lpBrush = NULL;
if (!AstralIsRectEmpty(&rUpdate))
	UpdateImageEx(lpImage, &rUpdate, YES, NO, YES);
}

/************************************************************************/
void SetSimpleBrushSize(HMGXBRUSH hBrush, int BrushSize)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	lpBrush->Size = BrushSize;
	}
}

/************************************************************************/
void SetSimpleBrushDDA(HMGXBRUSH hBrush, int iDDA)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (!hBrush)
	return;
lpBrush = (LPMGXBRUSH)hBrush;
lpBrush->iDDA = iDDA;
}

/************************************************************************/
void SetSimpleBrushFeather(HMGXBRUSH hBrush, int Feather)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	lpBrush->Feather = Feather;
	}
}

/************************************************************************/
BOOL SetSimpleBrushShape(HMGXBRUSH hBrush, int BrushShape)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (!hBrush)
	return(FALSE);

lpBrush = (LPMGXBRUSH)hBrush;
lpBrush->Shape = BrushShape;
return(TRUE);
}

/************************************************************************/
void SetSimpleBrushSource(HMGXBRUSH hBrush, LPFRAME lpFrame, LPSTR lpOptions )
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
int iSize, dx, dy;
//int i, y, iDataType;
LPSTR lp; 
LPTR lpData;

if ( !lpFrame )
	return;
if ( !hBrush )
	{
	FrameClose( lpFrame );
	return;
	}

lpBrush = (LPMGXBRUSH)hBrush;
if ( lpBrush->SourceFrame )
	FrameClose( lpBrush->SourceFrame );
lpBrush->fSourceUseDropoutMask = YES;
lpBrush->fSourceRandom         = 
lpBrush->fSourceDirectional    = 
lpBrush->fSourceStartPatterns  = 
lpBrush->fSourceEndPatterns    = NO;
lpBrush->SourceFrame           = lpFrame;
SetSourceFrameColors( lpBrush->SourceFrame );
PixmapSetup( &SourcePixmap, lpBrush->SourceFrame, YES );
dx = FrameXSize(lpBrush->SourceFrame);
dy = FrameYSize(lpBrush->SourceFrame);
iSize = min( dx, dy );
if ( iSize > MAX_BRUSH_SIZE )
	iSize = MAX_BRUSH_SIZE;

if ( lpOptions )
{
	if ( lp = lstrfind( lpOptions, "/n" ) )
		lpBrush->fSourceUseDropoutMask = NO;
	if ( lp = lstrfind( lpOptions, "/r" ) )
	{
		lpBrush->fSourceRandom = YES;
		lpBrush->nSourcePatterns = 1;
	}
	else
//	if ( lp = lstrfind( lpOptions, "/d" ) )
	{
		lpBrush->fSourceDirectional = YES;
		if ( lp = lstrfind( lpOptions, "/s" ) )
			lpBrush->fSourceStartPatterns = YES;
		if ( lp = lstrfind( lpOptions, "/e" ) )
			lpBrush->fSourceEndPatterns = YES;
		if ( lpBrush->fSourceStartPatterns && lpBrush->fSourceEndPatterns )
			dy /= 3;
		else
		if ( lpBrush->fSourceStartPatterns || lpBrush->fSourceEndPatterns )
			dy /= 2;

		lpBrush->nSourcePatterns = 6;
	}
}
else
{
	if ( !(lpBrush->nSourcePatterns = (dy/iSize)) )
		lpBrush->nSourcePatterns = 1;
}

// save the drop-out color for later use by LoadDropoutBrushMask
if (lpBrush->SourceFrame != NULL)
{
	if ( lpData = FramePointer (lpBrush->SourceFrame, 0, dx-1, FALSE) )
		{
		drop_red = *(lpData);
		drop_green = *(lpData+1);
		drop_blue = *(lpData +2);
		}
}

if ( lpBrush->fSourceUseDropoutMask || lpBrush->Size > iSize )
	lpBrush->Size = iSize;
if ( lpBrush->fSourceDirectional )
	lpBrush->Spacing = 100;
}

//////////////////////////////////////////////////////////////////////////////
int SetSimpleBrushStyle( HMGXBRUSH hBrush, int BrushStyle )
//////////////////////////////////////////////////////////////////////////////
{
    LPMGXBRUSH	lpBrush;
    int OldStyle = IDC_BRUSHSOLID;

    if( hBrush )
    {
	    lpBrush = ( LPMGXBRUSH )hBrush;
	    OldStyle = lpBrush->Style;
	    lpBrush->Style = BrushStyle;
    }
    return( OldStyle );
}

//////////////////////////////////////////////////////////////////////////////
int SetSimpleBrushStylePressure( HMGXBRUSH hBrush, int StylePressure )
//////////////////////////////////////////////////////////////////////////////
{
    LPMGXBRUSH	lpBrush;
    int OldPressure = 255;

    if( hBrush )
	{
	    lpBrush = ( LPMGXBRUSH )hBrush;
    	OldPressure = lpBrush->StylePressure;
	    lpBrush->StylePressure = StylePressure;
        lpBrush->StylePressure = bound( TOGRAY( lpBrush->StylePressure ), 0, 255 );
	}
    return( OldPressure );
}

//////////////////////////////////////////////////////////////////////////////
int SetSimpleBrushFade( HMGXBRUSH hBrush, int FadeSize )
//////////////////////////////////////////////////////////////////////////////
{
    LPMGXBRUSH	lpBrush;
    int OldFadeSize = 0;

    if( hBrush )
	{
	    lpBrush = ( LPMGXBRUSH )hBrush;
    	OldFadeSize = lpBrush->FadeSize;
	    lpBrush->FadeSize = FadeSize;
        lpBrush->FadeSize = bound( lpBrush->FadeSize, -99, 99 );
	}
    return( OldFadeSize );
}


/************************************************************************/
static void SetSourceFrameColors( LPFRAME lpSourceFrame )
/************************************************************************/
{
int x, y, dx, dy, r, g, b;
LPTR lp;
RGBS rgbActive, rgbAlternate;

if ( !lpSourceFrame )
	return;
if ( FrameDepth(lpSourceFrame) != 3 )
	return;

GetActiveRGB( &rgbActive );
GetAlternateRGB( &rgbAlternate );

dx = FrameXSize(lpSourceFrame);
dy = FrameYSize(lpSourceFrame);
for (y = 0; y < dy; ++y)
	{
	if ( !(lp = FramePointer( lpSourceFrame, 0, y, YES)) )
		continue;
	for (x = 0; x < dx; ++x)
		{
		r = *lp++;
		g = *lp++;
		b = *lp++;
		if ( r == 153 && g == 153 && b == 153 )
			{ // replace with the active color
			lp -= 3;
			*lp++ = rgbActive.red;
			*lp++ = rgbActive.green;
			*lp++ = rgbActive.blue;
			}
		else
		if ( r == 102 && g == 102 && b == 102 )
			{ // replace with the alternate color
			lp -= 3;
			*lp++ = rgbAlternate.red;
			*lp++ = rgbAlternate.green;
			*lp++ = rgbAlternate.blue;
			}
		}
	}
}


/************************************************************************/
int SetSimpleBrushSpacing(HMGXBRUSH hBrush, int Spacing)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
int OldSpacing = 0;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	OldSpacing = lpBrush->Spacing;
	lpBrush->Spacing = Spacing;
	}
return(OldSpacing);
}

/************************************************************************/
int SetSimpleBrushOpacity(HMGXBRUSH hBrush, int Opacity)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
int OldOpacity = 255;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	OldOpacity = lpBrush->Opacity;
	lpBrush->Opacity = Opacity;
	}
return(OldOpacity);
}

/************************************************************************/
void SetSimpleBrushColor(HMGXBRUSH hBrush, LPCOLORINFO lpColorStruct)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	lpBrush->ColorStruct = *lpColorStruct;
	SetColorInfo(&lpBrush->ColorStruct, &lpBrush->ColorStruct,
			lpBrush->ColorStruct.ColorSpace);
	}
}

/************************************************************************/
MERGE_MODE SetSimpleBrushMergeMode(HMGXBRUSH hBrush, MERGE_MODE MergeMode)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
MERGE_MODE oldMode = MM_NORMAL;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	oldMode = lpBrush->MergeMode;
	lpBrush->MergeMode = MergeMode;
	}
return(oldMode);
}


/************************************************************************/
int SetSimpleBrushStrokeMethod(HMGXBRUSH hBrush, int idStrokeMethod)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;
int idOldMethod = 0;

if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	idOldMethod = lpBrush->StrokeMethod;
	lpBrush->StrokeMethod = idStrokeMethod;
	}
return(idOldMethod);
}


/************************************************************************/
void SetSimpleBrushMirrorMode(HMGXBRUSH hBrush, BOOL fMirror)
/************************************************************************/
{
	LPMGXBRUSH	lpBrush;

	if (hBrush)
	{
		lpBrush = (LPMGXBRUSH)hBrush;
		if (fMirror)
			lpBrush->fMirrorMode = YES;
		else
			lpBrush->fMirrorMode = NO;
	}
}


/************************************************************************/
int GetSimpleBrushSize(HMGXBRUSH hBrush)
/************************************************************************/
{
if (hBrush)
	return(((LPMGXBRUSH)hBrush)->Size);
else
	return(0);
}

/************************************************************************/
int GetSimpleBrushFeather(HMGXBRUSH hBrush)
/************************************************************************/
{
if (hBrush)
	return(((LPMGXBRUSH)hBrush)->Feather);
else
	return(0);
}

/************************************************************************/
int GetSimpleBrushShape(HMGXBRUSH hBrush)
/************************************************************************/
{
if (hBrush)
	return(((LPMGXBRUSH)hBrush)->Shape);
else
	return(IDC_BRUSHCIRCLE);
}

/************************************************************************/
int GetSimpleBrushSpacing(HMGXBRUSH hBrush)
/************************************************************************/
{
if (hBrush)
	return(((LPMGXBRUSH)hBrush)->Spacing);
return(0);
}

/************************************************************************/
int GetSimpleBrushOpacity(HMGXBRUSH hBrush)
/************************************************************************/
{
if (hBrush)
	return(((LPMGXBRUSH)hBrush)->Opacity);
return(255);
}

/************************************************************************/
MERGE_MODE GetSimpleBrushMergeMode(HMGXBRUSH hBrush, LPINT lpReadOnly)
/************************************************************************/
{
LPMGXBRUSH	lpBrush;

if (lpReadOnly)
	*lpReadOnly = NO;
if (hBrush)
	{
	lpBrush = (LPMGXBRUSH)hBrush;
	return(lpBrush->MergeMode);
	}
else
	{
	return(MM_NORMAL);
	}
}

/************************************************************************/
int GetSimpleBrushStyle(HMGXBRUSH hBrush)
/************************************************************************/
{
    if( hBrush )
	    return((( LPMGXBRUSH )hBrush)->Style );

    return( IDC_BRUSHSOLID );
}

/************************************************************************/
int GetSimpleBrushStylePressure(HMGXBRUSH hBrush)
/************************************************************************/
{
    if( hBrush )
	    return((( LPMGXBRUSH )hBrush)->StylePressure );

    return( 255 );
}

/************************************************************************/
int GetSimpleBrushFade(HMGXBRUSH hBrush)
/************************************************************************/
{
    if( hBrush )
	    return((( LPMGXBRUSH )hBrush)->FadeSize );

    return( 0 );
}

/************************************************************************/
static void SetupFade( LPMGXBRUSH lpBrush )
/************************************************************************/
{
    int		Delta;

    // Setup the Fade In/Out values
    if ( lpBrush->Shape == IDC_BRUSHCUSTOM || lpBrush->FadeSize == 0)
	    FadeOut = 0L;
    else	{
	    Delta = ( lpBrush->FadeSize > 0 ? lpBrush->Size :
		    MAX_BRUSH_SIZE - lpBrush->Size + 1 );
	    FadeOut = FGET( Delta, (100-abs(lpBrush->FadeSize))*FADE_LEN_MULT);
	    if (lpBrush->FadeSize < 0)
		    FadeOut = -FadeOut;
	    }
    BrushSize = lpBrush->Size;
    FadeBrushSize = TOFIXED(BrushSize);

    // Setup values for fading opacity
    FadeOpacity = 0L;
    Opacity = lpBrush->Opacity;
    FadeOpacityValue = TOFIXED(Opacity);

    MaxOpacity = ((lpBrush->Opacity * lpBrush->OpacityScale) + 50) / 100;

    // Setup pressure values
    StylePressure = lpBrush->StylePressure;
}

/************************************************************************/
static void ResetBrush( LPMGXBRUSH lpBrush )
/************************************************************************/
{
    if( FadeOut || FadeOpacity )
	{
	    DisplayBrush( 0, 0, 0, OFF );

	    lpBrush->Size = BrushSize;
	    lpBrush->Opacity = Opacity;
	    lpBrush->StylePressure = StylePressure;

	    SetupBrush( lpBrush );

	    DisplayBrush( hWnd, brush_x, brush_y, ON );
	}
}

/************************************************************************/
void FadeBrush(BOOL fSetupMask)
{
int	iNewOpacity;

if (FadeOut)
	{
	lpBrush->Size = FMUL(1, FadeBrushSize);
	lpBrush->Size = bound(lpBrush->Size, 0, BrushSize);
	if (fSetupMask)
		SetupBrush(lpBrush);
	else
		{
		BrushFull = bound(lpBrush->Size, 0, BrushSize);
		BrushHalf = BrushFull / 2;
		}
	}
if (FadeOpacity)
	{
	lpBrush->Opacity = FMUL(1, FadeOpacityValue);
	lpBrush->Opacity = bound(lpBrush->Opacity, 0, Opacity);
	iNewOpacity = ((lpBrush->Opacity * lpBrush->OpacityScale) + 50) / 100;
	if (iNewOpacity != LastOpacity)
		{
		SetBrushOpacity(lpBrush->BrushMask, lpBM, iNewOpacity,
				 BrushFull*BrushFull);
		LastOpacity = iNewOpacity;
		}
	}
}

/************************************************************************/
void FadeOutBrush(BOOL fSetupMask)
{
if (FadeOut || FadeOpacity)
	{
	if (FadeOut)
		FadeBrushSize -= FadeOut;
	if (FadeOpacity && fSetupMask)
		FadeOpacityValue -= FadeOpacity;
	if (FadeBrushSize > 0)
		FadeBrush(fSetupMask);
	}
}

/************************************************************************/
static void BrushStroke(int dx, int dy)
/************************************************************************/
{
RECT rBrush;
POINT pt;

if (!BrushFull)
	return;

pt.x = dx; pt.y = dy;
GridPoint( &pt );
dx = pt.x; dy = pt.y;

if ( fFirstStroke )
	fFirstStroke = NO;
else
if ( abs(last_dx - dx) < Spacing && abs(last_dy - dy) < Spacing )
	return; // has not moved far enough (or not at all) - don't do anything

last_dx = dx;
last_dy = dy;

rBrush.left = dx - BrushHalf;
rBrush.right = rBrush.left + BrushFull - 1;
rBrush.top = dy - BrushHalf;
rBrush.bottom = rBrush.top + BrushFull - 1;
BlitRect(&rBrush, lpBM);
BlitToDisplay( &rBrush );
}

/************************************************************************/
static void BrushStrokeMyDDA(int dx, int dy)
/************************************************************************/
{
static int last_dx, last_dy;

// Initialize for first stroke that we get
if (fFirstStroke)
	{
	BrushStroke( dx, dy );
	fFirstStroke = FALSE; // no longer first stroke
	}
else
	ddaline( last_dx, last_dy, dx, dy, (LPPOINTFUNC)BrushStroke );

last_dx = dx;
last_dy = dy;
}

/************************************************************************/
static void BrushStrokeDDA(int dx, int dy)
/************************************************************************/
{
int deltax, deltay, i;
POINT pt;
static int start_dx, start_dy, end_dx, end_dy;
static int mstart_dx, mstart_dy, mend_dx, mend_dy;	// mirror versions

pt.x = dx; pt.y = dy;
GridPoint( &pt );
dx = pt.x; dy = pt.y;

// Initialize for first stroke that we get
if (fFirstStroke)
	{
	BlitRect( NULL, NULL );
	start_dx = dx;		 // setup initial starting point
	start_dy = dy;
	if (lpBrush->fMirrorMode)
	{
		if (lpImage != NULL)
			iScreenX = lpImage->lpDisplay->DispRect.right;
		else
			iScreenX = 479;
    	mstart_dx = iScreenX - dx;
    	mstart_dy = dy;
    	mlast_x_processed = 32767;
    	mlast_y_processed = 32767;
	}
	last_x_processed = 32767;
	last_y_processed = 32767;
	fFirstStroke = FALSE; // no longer first stroke
	for (i = 0; i < 4; ++i)
		bRects[i].fEmpty = YES;
		if (lpBrush->fMirrorMode)
        	bMRects[i].fEmpty = YES;
	}
else if (dx == start_dx && dy == start_dy)
	return; // point has moved - don't do anything

// setup new ending point - start point was the last point
end_dx = dx;
end_dy = dy;
if (lpBrush->fMirrorMode)
{
	mend_dx = iScreenX - dx;
	mend_dy = dy;
}

deltax = abs(end_dx-start_dx);
deltay = abs(end_dy-start_dy);

// Save direction of stroke based on type of strips being processed
fRight = (start_dx <= end_dx);
fBottom = (start_dy <= end_dy);

if (fRight && fBottom)
{
	pPtTable = lr;
    pMPtTable = ll;	// mirror versions
}
else if (fRight)
{
	pPtTable = ur;
    pMPtTable = ul;
}
else if (fBottom)
{
	pPtTable = ll;
    pMPtTable = lr;
}
else
{
	pPtTable = ul;
    pMPtTable = ur;
}

if (deltax || deltay)
	{
	if (deltax > 1 || deltay > 1)
		{
		// if mirroring
		if (lpBrush->fMirrorMode)
            {
            if( lpBrush->FadeSize )     // save current state before fade
                save_fade_values();

           	ddaline (mstart_dx, mstart_dy, mend_dx, mend_dy, (LPPOINTFUNC)MProcessRectList);

            if( lpBrush->FadeSize )     // restore current state before fading original brush
                restore_fade_values();
            }
		ddaline(start_dx, start_dy, end_dx, end_dy, (LPPOINTFUNC)ProcessRectList);
		}
	else
		{
		if (lpBrush->fMirrorMode)
			{
            if( lpBrush->FadeSize )
                save_fade_values();

			MProcessRectList(mstart_dx, mstart_dy);
			MProcessRectList(mend_dx, mend_dy);

            if( lpBrush->FadeSize )
                restore_fade_values();
			}
		ProcessRectList(start_dx, start_dy);
		ProcessRectList(end_dx, end_dy);
		}
	}
else
{
	if (lpBrush->fMirrorMode)
    {
        if( lpBrush->FadeSize )
            save_fade_values();

		MProcessRectList(mstart_dx, mstart_dy);

        if( lpBrush->FadeSize )
            restore_fade_values();
    }
	ProcessRectList(start_dx, start_dy);
}

// next strokes start becomes this strokes end
start_dx = end_dx;
start_dy = end_dy;
mstart_dx = mend_dx;
mstart_dy = mend_dy;

for (i = 0; i < 4; ++i)
	{
	if (bRects[i].fEmpty)
		continue;
	BlitToDisplay(&bRects[i].rEdit);
	bRects[i].fEmpty = YES;
	}
if (lpBrush->fMirrorMode)
	{
	for (i = 0 ; i < 4 ; ++i)
		{
		if (bMRects[i].fEmpty)
			continue;
		BlitToDisplay(&bMRects[i].rEdit);
		bMRects[i].fEmpty = YES;
		}
	}
}


/************************************************************************/
static void ProcessRectList(int x, int y)
{
RECT bRect, iRect, rSect;
BOOL fOnImage, fAnyEmpty, fAllEmpty;
int i, j, empty, pt, pi;
WORD	xTile, yTile;
POINT pts[4];
PBRUSHRECT pbRect;

brush_x = x;
brush_y = y;

if (abs(last_x_processed-x) < Spacing && abs(last_y_processed-y) < Spacing)
{
    FadeOutBrush( YES );
	return;
}

last_x_processed = x;
last_y_processed = y;
if (!BrushFull)
{
    FadeOutBrush( YES );
	return;
}

// get rectangle this brush stroke encompasses
bRect.top = y - BrushHalf;
bRect.bottom = bRect.top + BrushFull - 1;
bRect.left = x - BrushHalf;
bRect.right = bRect.left + BrushFull - 1;

// merge brush in with current brush mask
if (fOnImage = AstralIntersectRect(&iRect, &rMax, &bRect))
	BlitRect(&bRect, lpBM);

// See where this brush intersects and output any rectangles that
// the brush no longer intersects
fAnyEmpty = NO;
fAllEmpty = YES;
for (i = 0; i < 4; ++i)
	{
	if (bRects[i].fEmpty)
		{
		fAnyEmpty = YES;
		continue;
		}
	if (fOnImage && AstralIntersectRect(&rSect, &bRects[i].rTile, &iRect))
		{
		fAllEmpty = NO;
		AstralUnionRect(&bRects[i].rEdit, &bRects[i].rEdit, &rSect);
		}
	else
		{
		BlitToDisplay(&bRects[i].rEdit);
		bRects[i].fEmpty = YES;
		fAnyEmpty = YES;
		}
	}

// move all rects to front of list
if (!fAllEmpty && fAnyEmpty)
	{
	for (i = 0; i < 4; ++i)
		{
		if (!bRects[i].fEmpty)
			{
			for (j = 0; j < i; ++j)
				{
				if (bRects[j].fEmpty)
					{
					bRects[j] = bRects[i];
					bRects[i].fEmpty = YES;
					break;
					}
				}
			}
		}
	}

// if not on image we are all done
if (!fOnImage || !fAnyEmpty)
{
    FadeOutBrush( YES );
	return;
}

pts[0].x = iRect.left;
pts[0].y = iRect.top;
pts[1].x = iRect.right;
pts[1].y = iRect.top;
pts[2].x = iRect.right;
pts[2].y = iRect.bottom;
pts[3].x = iRect.left;
pts[3].y = iRect.bottom;
for (pt = 0; pt < 4; ++pt)
	{
	pi = pPtTable[pt];
	xTile = pts[pi].x / bh;
	yTile = pts[pi].y / bv;
	empty = -1;
	for (i = 0; i < 4; ++i)
		{
		if (bRects[i].fEmpty)
			{
			if (empty < 0)
				empty = i;
			continue;
			}
		if (bRects[i].xTile == xTile && bRects[i].yTile == yTile)
			break;
		}
	if (i >= 4) // not found in rect list
		{
		if (empty >= 0)
			{
			pbRect = &bRects[empty];
			pbRect->xTile = xTile;
			pbRect->yTile = yTile;
			pbRect->rTile.left = xTile * bh;
			pbRect->rTile.right = pbRect->rTile.left + bh - 1;
			pbRect->rTile.top = yTile * bv;
			pbRect->rTile.bottom = pbRect->rTile.top + bv - 1;
			AstralIntersectRect(&pbRect->rEdit, &pbRect->rTile,
								 &iRect);
			pbRect->fEmpty = NO;
			}
//		else
//			Print("Tell Ted you got empty rectangle message");
		}
	}
FadeOutBrush( YES );
}


/************************************************************************/
// This version is only for the mirror option
static void MProcessRectList (int x, int y)
/************************************************************************/
{
RECT bRect, iRect, rSect;
BOOL fOnImage, fAnyEmpty, fAllEmpty;
int i, j, empty, pt, pi;
WORD	xTile, yTile;
POINT pts[4];
PBRUSHRECT pbRect;

if (abs(mlast_x_processed-x) < Spacing && abs(mlast_y_processed-y) < Spacing)
{
    FadeOutBrush( YES );
	return;
}

mlast_x_processed = x;
mlast_y_processed = y;
if (!BrushFull)
{
    FadeOutBrush( YES );
	return;
}

// get rectangle this brush stroke encompasses
bRect.top = y - BrushHalf;
bRect.bottom = bRect.top + BrushFull - 1;
bRect.left = x - BrushHalf;
bRect.right = bRect.left + BrushFull - 1;

// merge brush in with current brush mask
if (fOnImage = AstralIntersectRect(&iRect, &rMax, &bRect))
	BlitRect(&bRect, lpBM);

// See where this brush intersects and output any rectangles that
// the brush no longer intersects
fAnyEmpty = NO;
fAllEmpty = YES;
for (i = 0; i < 4; ++i)
	{
	if (bMRects[i].fEmpty)
		{
		fAnyEmpty = YES;
		continue;
		}
	if (fOnImage && AstralIntersectRect(&rSect, &bMRects[i].rTile, &iRect))
		{
		fAllEmpty = NO;
		AstralUnionRect(&bMRects[i].rEdit, &bMRects[i].rEdit, &rSect);
		}
	else
		{
		BlitToDisplay(&bMRects[i].rEdit);
		bMRects[i].fEmpty = YES;
		fAnyEmpty = YES;
		}
	}

// move all rects to front of list
if (!fAllEmpty && fAnyEmpty)
	{
	for (i = 0; i < 4; ++i)
		{
		if (!bMRects[i].fEmpty)
			{
			for (j = 0; j < i; ++j)
				{
				if (bMRects[j].fEmpty)
					{
					bMRects[j] = bMRects[i];
					bMRects[i].fEmpty = YES;
					break;
					}
				}
			}
		}
	}

// if not on image we are all done
if (!fOnImage || !fAnyEmpty)
{
    FadeOutBrush( YES );
	return;
}

pts[0].x = iRect.left;
pts[0].y = iRect.top;
pts[1].x = iRect.right;
pts[1].y = iRect.top;
pts[2].x = iRect.right;
pts[2].y = iRect.bottom;
pts[3].x = iRect.left;
pts[3].y = iRect.bottom;
for (pt = 0; pt < 4; ++pt)
	{
	pi = pMPtTable[pt];
	xTile = pts[pi].x / bh;
	yTile = pts[pi].y / bv;
	empty = -1;
	for (i = 0; i < 4; ++i)
		{
		if (bMRects[i].fEmpty)
			{
			if (empty < 0)
				empty = i;
			continue;
			}
		if (bMRects[i].xTile == xTile && bMRects[i].yTile == yTile)
			break;
		}
	if (i >= 4) // not found in rect list
		{
		if (empty >= 0)
			{
			pbRect = &bMRects[empty];
			pbRect->xTile = xTile;
			pbRect->yTile = yTile;
			pbRect->rTile.left = xTile * bh;
			pbRect->rTile.right = pbRect->rTile.left + bh - 1;
			pbRect->rTile.top = yTile * bv;
			pbRect->rTile.bottom = pbRect->rTile.top + bv - 1;
			AstralIntersectRect(&pbRect->rEdit, &pbRect->rTile,
								 &iRect);
			pbRect->fEmpty = NO;
			}
//		else
//			Print("Tell Ted you got empty rectangle message");
		}
	}
FadeOutBrush( YES );
}

static int color_index0 = 255, color_index1 = 102, color_index2 = 153, color_index3 = 204;
/************************************************************************/
static void BlitRect(LPRECT lpRect, LPTR lpBrushMask)
{
RECT rDest, rSource, rOutput;
int x, y, sy, w, h;
LPTR lpMskData;
BOOL fx, fy;

if ( !lpRect )
	{ // reinitialize the direction flag
	if (! fLastStroke)
		iDirection = -2;
	else
		iDirection = iDirection2;
	iPattern = -1;
	iStartPattern = 0;
	return;
	}
if (fLastStroke)
	iDirection = iDirection2;
if ( lpBrush->fSourceDirectional )
	{ // always bliting one rectangle behind
	if ( iDirection == -2 )
		{
		rLast = *lpRect;
		iDirection = -1;
		return;
		}
	rOutput = rLast;
	rLast = *lpRect;
	*lpRect = rOutput;
	}

// get destination x, y, width, and height of rect to be blt'ed
x = lpRect->left;
y = lpRect->top;
w = RectWidth(lpRect);
h = RectHeight(lpRect);

// Load the destination buffer with destination data
if (!LoadSourceData(lpDstPixmap, PMT_EDIT, x, y, w, h, lpDst,
	&rDest, NO/*Horz*/, NO/*Vert*/))
		return;

if ( lpBrush->SourceFrame )
	{
	if (! fLastStroke)
		{
		if ( lpBrush->fSourceRandom )
			{
			if (lpBrush->nSourcePatterns == 1)
				iPattern = 0;
			else
				iPattern = rand() % lpBrush->nSourcePatterns;
			fx = NO;
			fy = NO;
			}
		else
		if ( lpBrush->fSourceDirectional )
			{
			if ( (iPattern = ComputePattern( x, y, rLast.left, rLast.top,
				&iDirection, &fx, &fy) ) < 0)
					return;
			}
		else
			{
			if ( ++iPattern >= lpBrush->nSourcePatterns )
				iPattern = 0;
			fx = NO;
			fy = NO;
			}
		}
	iPattern = bound( iPattern, 0, lpBrush->nSourcePatterns-1 );
	iDirection2 = iDirection;

	// set up to add end piece
	if (fLastStroke && lpBrush->fSourceEndPatterns)
	{
		fx = NO;
		fy = NO;
		if (iDirection == 4)		// right
		{
			iStartPattern = 9;
			fx = YES;
			fy = YES;
		}
		else if (iDirection == 0)	// left
		{
			iStartPattern = 9;
		}
		else if (iDirection == 2)	// up
		{
			iStartPattern = 8;
			fx = YES;
		}
		else if (iDirection == 6)	// down
		{
			iStartPattern = 8;
			fy = YES;
		}
		iPattern = iStartPattern;
	}
	// set up to add start piece
	else if (! iStartPattern && lpBrush->fSourceStartPatterns)
	{
		fx = NO;
		fy = NO;
		if (iDirection == 4)		// right
		{
			iStartPattern = 7;
			fx = YES;
			fy = YES;
		}
		else if (iDirection == 0)	// left
		{
			iStartPattern = 7;
		}
		else if (iDirection == 2)	// up
		{
			iStartPattern = 6;
			fx = YES;
		}
		else if (iDirection == 6)	// down
		{
			iStartPattern = 6;
			fy = YES;
		}
		iPattern = iStartPattern;
	}
	if (! fy)
		sy = iPattern * lpBrush->Size;
	else
		sy = FrameYSize(lpBrush->SourceFrame) - ((iPattern+1) * lpBrush->Size);
	if (!LoadMirroredData( &SourcePixmap, PMT_EDIT, 0/*sx*/, sy, w, h, lpSrc,
		&rSource, fx, fy ) )
			return;
	if ( lpBrush->fSourceUseDropoutMask )
		LoadDropoutBrushMask( lpSrc, w, h, lpBrushMask );
	}
else
	rSource = rDest;

// If an image mask is being used, combine it with the Brush Mask
if (UsingMask)
	{
	// load image mask
	(*lpMaskProc)( x+xDstOffset, y+yDstOffset, w, h, lpMsk, lpDst, lpMask );
	copyifset( lpBrushMask, lpMsk, w * h );
	lpMskData = lpMsk;
	}
else 
if( !StylizeProc )
	lpMskData = lpBrushMask;
else
{
	copy( lpBrushMask, lpMsk, w * h );
	lpMskData = lpMsk;
}

// Do style modifications to the mask, such as scatter
if( StylizeProc )
	( *StylizeProc )( lpMskData, lpMskData, lpErr, w, h, lpBrush->StylePressure );

// add glitter to the brush pattern
if( iBltFrameDepth == 3 && ( lpBrush->Shape == IDC_BRUSHGLITTERCIRCLE || 
                             lpBrush->Shape == IDC_BRUSHSWIRLCIRCLE ))
{
    LoadColor(PixmapFrame(lpDstPixmap, PMT_EDIT), lpSrc, bcount,
                          &lpBrush->ColorStruct);

    if( lpBrush->Shape == IDC_BRUSHGLITTERCIRCLE )
        glitterRGB(( LPRGB )lpSrc, BrushFull * BrushFull, BrushFull );
    else
        swirlRGB(( LPRGB )lpSrc, BrushFull * BrushFull, BrushFull );
}

// Process the data by combining destination, source, and mask
(*lpProcessProc)(lpDst, lpSrc, lpMskData, w * h );

// Blt the new data to the frame
if (AstralIntersectRect(&rOutput, &rSource, &rDest))
	BlitToFrame(x, y, w, h, lpDst, &rOutput);
}

/************************************************************************/
static void LoadDropoutBrushMask( LPTR lpSrc, int dx, int dy, LPTR lpBrushMask )
/************************************************************************/
{
int iSize, r, g, b, dr, dg, db, i, x, y;
//int iCount;
//LPTR lp;
#define BRMASK(lpM,BrushSize,x,y) lpM[ (BrushSize * (y)) + (x) ]

if ( dy > MAX_BRUSH_SIZE )
	dy = MAX_BRUSH_SIZE;
if ( dx > MAX_BRUSH_SIZE )
	dx = MAX_BRUSH_SIZE;
iSize = max( dx, dy );

// Set up by SetSimpleBrushSource to fix a problem with the
// crazy line tool brush dropout color
dr = drop_red;
dg = drop_green;
db = drop_blue;
//dr = *(lpSrc+0);
//dg = *(lpSrc+1);
//db = *(lpSrc+2);

for (y = 0; y < dy; ++y)
	{
	for (x = 0; x < dx; ++x)
		{
		r = *lpSrc++;
		g = *lpSrc++;
		b = *lpSrc++;
		if ( r == dr && g == dg && b == db )
				i = 0;
		else	i = 255;
		BRMASK( lpBrushMask, iSize, x, y ) = i;
		}
	}
}


/************************************************************************/
static void BlitToFrame( int x, int y, int dh, int dv, LPTR lpData,
		LPRECT lpRect)
/************************************************************************/
{
int sy;
WORD bwidth, width;
LPTR lp;
RECT StrokeRect;

StrokeRect = *lpRect;

// get width of buffer in bytes
bwidth = dh*iBltFrameDepth;

// advance in the number of lines and pixels
lpData += ((WORD)StrokeRect.top * bwidth) +
			((WORD)StrokeRect.left * iBltFrameDepth);
// get width of area to be blt'ed
width = (StrokeRect.right-StrokeRect.left+1) * iBltFrameDepth;

// offset stroked area from buffer to frame
StrokeRect.left += x;
StrokeRect.right += x;
StrokeRect.top += y;
StrokeRect.bottom += y;
for (sy = StrokeRect.top; sy <= StrokeRect.bottom; ++sy)
	{
	if ((lp = PixmapPtr(lpDstPixmap, PMT_EDIT, StrokeRect.left, sy, YES)))
		copy(lpData, lp, width);
	lpData += bwidth;
	}

PixmapAddUndoArea(lpDstPixmap, &StrokeRect);
}

/************************************************************************/
static void BlitToDisplay(LPRECT lpRect)
/************************************************************************/
{
RECT StrokeRect, DispRect;

StrokeRect = *lpRect;
OffsetRect(&StrokeRect, xDstOffset, yDstOffset);
AstralUnionRect(&rUpdate, &rUpdate, &StrokeRect);
File2DispRect(&StrokeRect, &DispRect);
ImgPaintRaster(lpImage, Window.hDC, &DispRect, NULL);
}

/************************************************************************/
static void SetupBrush(LPMGXBRUSH lpBrush)
/************************************************************************/
{
SetupBrushMask(lpBrush);
BrushFull = lpBrush->Size;
BrushHalf = BrushFull / 2;
Spacing = (((WORD)lpBrush->Spacing * (WORD)BrushFull) + 50)/100;
if (!Spacing)
	Spacing = 1;
LastOpacity = ((lpBrush->Opacity * lpBrush->OpacityScale) + 50) / 100;

SetBrushOpacity(lpBrush->BrushMask, lpBM, LastOpacity, BrushFull*BrushFull);
}

/************************************************************************/
static void SetBrushOpacity(LPTR lpSrc, LPTR lpDst, WORD Opacity,
			int iCount)
/************************************************************************/
{
#ifdef C_CODE
while (--iCount >= 0)
	*lpDst++ = ((WORD)*lpSrc++ * Opacity)/255;
#else
__asm	{
	push	ds
	push	si
	push	di
	pushf
	lds	si, lpSrc
	les	di, lpDst
	mov	cx, iCount
	or	cx,cx
	je	SBO_END
	mov	bx,Opacity
	cld
	cmp	bx,127
	jbe	SBO_1
	inc	bx
SBO_1:
	lodsb
	sub	ah,ah
	mul	bx
	mov	es:[di],ah
	inc	di
	loop	SBO_1
SBO_END:
	popf
	pop	di
	pop	si
	pop	ds
	}
#endif
}

/************************************************************************/
static BOOL AllocBrushBuffers(LPMGXBRUSH lpBrush)
/************************************************************************/
{
long	lSize;

// "NULL" out all buffers
lpSrc = lpDst = lpMsk = lpBM = NULL;
lpErr = NULL;

// Calculate size of buffers to be used
// double-wide strips seem to be fastest for the most brush sizes
bh = lpBrush->Size;
if (bh < 10)
	bh = 10;
// make sure we fit in 64K
if (iBltFrameDepth <= 1)
	bh = min(bh, 255);
else
if (iBltFrameDepth == 3)
	bh = min(bh, 147);
else
	bh = min(bh, 127);
bv = bh;

bcount = bh * bv;

// Allocate memory for buffers used for all style brushes
lSize = iBltFrameDepth;
if (lSize == 0) lSize = 1;
lSize *= (long)bcount;
if (!(lpSrc = Alloc( lSize )))
	goto MemError;
if (!(lpDst = Alloc( lSize )))
	goto MemError;
if (!(lpMsk = Alloc( (long)bcount)))
	goto MemError;
if (!(lpBM = Alloc( (long)sizeof(lpBrush->BrushMask) )))
	goto MemError;

// Allocate an error buffer for scatter style
if( lpBrush->Style == IDC_BRUSHSCATTER &&
	!( lpErr = ( LPINT )Alloc(( long )( bh+1 ) * 3L * 2L )))
	goto MemError;

return(TRUE);

MemError:
Message(IDS_EMEMALLOC);
// freeup buffers
FreeUpBrushBuffers();
return(FALSE);
}

/************************************************************************/
static void FreeUpBrushBuffers()
/************************************************************************/
{
FreeUp( lpSrc );
FreeUp( lpDst );
FreeUp( lpMsk );
FreeUp( lpBM );
FreeUp(( LPTR )lpErr );
}

/************************************************************************/
static BOOL SetupFrames(LPMGXBRUSH lpBrush, BOOL fBegin)
/************************************************************************/
{
int	mode;
//int iDataType;

if (fBegin) // called from SimpleBrushBegin
	{
	mode = RELEASE_OLDEST; // faster mode for retouching
	PixmapGetInfo(lpDstPixmap, PMT_EDIT, &iBltFrameXsize, &iBltFrameYsize,
					&iBltFrameDepth, &iBltFrameType);
	}
else // called from SimpleBrushEnd
	{
	mode = RELEASE_NEWEST; // normal mode
	}

// Set mode for Blt frame
FrameMode(PixmapFrame(lpDstPixmap, PMT_EDIT), mode);

return(TRUE);
}

#define X 64
#define Y 128
#define TANGENT225  27146L
#define TANGENT675 158218L
// Directions:	1 2 3		Patterns:	0 = upperleft corner
//				0 x 4					1 = up-down pointing left
//				7 6 5					2 = left-right pointing up

static BYTE Table[8][8] = {
//  Out		0		1 		2	 	3	 	4	 	5 		6 		7
// In
/* 0 */		5|X|Y,	-1,		3,		3,		5,		0|X|Y,	0|X|Y,	-1,
/* 1 */		-1,		2,		-1,		-1,		2|X|Y,	-1,		0|X|Y,	-1,
/* 2 */		1|X|Y,	-1,		-1,		-1,		2|X|Y,	2|X|Y,	4|X|Y,	1|X|Y,
/* 3 */		1|X|Y,	-1,		-1,		-1,		-1,		-1,		3|X|Y,	-1,
/* 4 */		5|X|Y,	0,		0,		-1,		5,		-1,		3|X|Y,	3|X|Y,
/* 5 */		2,		-1,		0,		-1,		-1,		-1,		-1,		-1,
/* 6 */		2,		2,		4,		1,		1,		-1,		4|X|Y,	-1,
/* 7 */		-1,		-1,		3,		-1,		1,		-1,		-1,		-1,
};

/************************************************************************/
static int ComputePattern( int xLast, int yLast, int x, int y, LPINT lpDirection, LPINT lpFlipX, LPINT lpFlipY )
/************************************************************************/
{
int xNeg, yNeg, iLastDirection, iDir, iPat;
LFIXED f;

x -= xLast;
y -= yLast;

if ( xNeg = ( x < 0 ) )
	x = -x;
if ( yNeg = ( y < 0 ) )
	y = -y;

if ( !x )
	f = 0x7FFFFFFF;
else
	f = FGET( y, x );

if ( !x && !y )
	iDir = *lpDirection;
else
if ( f > TANGENT675 )
	iDir = ( yNeg ? 2 : 6 );
else
if ( f < TANGENT225 )
	iDir = ( xNeg ? 0 : 4 );
else
	{
	if ( xNeg )
		iDir = ( yNeg ? 1 : 7 );
	else
		iDir = ( yNeg ? 3 : 5 );
	}

if ( (iLastDirection = *lpDirection) < 0 )
	{
	iLastDirection = iDir;
	if ( (iLastDirection += 4) >= 8 )
		iLastDirection -= 8;
	}
iPat = Table[iLastDirection][iDir];
if ( iPat != -1 )
	{
	if ( *lpFlipX = (iPat & X) )
		iPat -= X;
	if ( *lpFlipY = (iPat & Y) )
		iPat -= Y;
	}

if ( (iDir += 4) >= 8 )
	iDir -= 8;
*lpDirection = iDir;
return( iPat );
}


//////////////////////////////////////////////////////////////////////////////
void glitterRGB( LPRGB lpRGB, int count, int brush_size )
//////////////////////////////////////////////////////////////////////////////
{
    static ULONG lSeed = 0xDEADBABA;     
    int icount;

    if( brush_size > MAX_BRUSH_SIZE )
        brush_size = MAX_BRUSH_SIZE;

	for( icount = 0; icount < count; icount++, lpRGB )
	{
        // create random number
		if( lSeed & BIT18 )
		{
			lSeed += lSeed;
			lSeed ^= BITMASK;
		}
		else	
			lSeed += lSeed;

        // glitterize point
        if( !( icount % ( count / max( brush_size, brush_size ))))   
        {
            switch( icount % 2 )
            {
                case 0:
                    glitter1( lpRGB, lSeed % count , count, brush_size );
                    break;

                case 1:
                    glitter2( lpRGB, lSeed % count , count, brush_size );
                    break;
            }
        }
	}
}

//////////////////////////////////////////////////////////////////////////////
void glitter1( LPRGB lpRGB, ULONG offset_brush, int count, int brush_size )
//////////////////////////////////////////////////////////////////////////////
{
    int direct_row = 1, direct_col = 1;     // default is to draw from l-r and t-b
    int gh = 3, gv = 3;                     // 3x3 glitter rect

    if( brush_size < gh || brush_size < gv )                // if too small, quit
        return;

    if(( offset_brush % brush_size ) > brush_size - gh )    // if too close to right, flip x
        direct_col = -1;

    if( brush_size - ( offset_brush / brush_size ) < gv )   // if too close to bottom, flip y
        direct_row = -1;

	( lpRGB + offset_brush )->red   = 153;
    ( lpRGB + offset_brush )->green = 153;
	( lpRGB + offset_brush )->blue  = 153;
	(( lpRGB + offset_brush ) + 1 * direct_col )->red   = 204;
    (( lpRGB + offset_brush ) + 1 * direct_col )->green = 204;
	(( lpRGB + offset_brush ) + 1 * direct_col )->blue  = 204;
	(( lpRGB + offset_brush ) + 2 * direct_col )->red   = 153;
    (( lpRGB + offset_brush ) + 2 * direct_col )->green = 153;
	(( lpRGB + offset_brush ) + 2 * direct_col )->blue  = 153;

	(( lpRGB + offset_brush ) + brush_size * direct_row )->red   = 204;
    (( lpRGB + offset_brush ) + brush_size * direct_row )->green = 204;
	(( lpRGB + offset_brush ) + brush_size * direct_row )->blue  = 204;
	(( lpRGB + offset_brush ) + brush_size * direct_row + 1 * direct_col )->red   = 255;
    (( lpRGB + offset_brush ) + brush_size * direct_row + 1 * direct_col )->green = 255;
	(( lpRGB + offset_brush ) + brush_size * direct_row + 1 * direct_col )->blue  = 255;
	(( lpRGB + offset_brush ) + brush_size * direct_row + 2 * direct_col )->red   = 204;
    (( lpRGB + offset_brush ) + brush_size * direct_row + 2 * direct_col )->green = 204;
	(( lpRGB + offset_brush ) + brush_size * direct_row + 2 * direct_col )->blue  = 204;

	(( lpRGB + offset_brush ) + 2 * brush_size * direct_row )->red   = 153;
    (( lpRGB + offset_brush ) + 2 * brush_size * direct_row )->green = 153;
	(( lpRGB + offset_brush ) + 2 * brush_size * direct_row )->blue  = 153;
	(( lpRGB + offset_brush ) + 2 * brush_size * direct_row + 1 * direct_col )->red   = 204;
    (( lpRGB + offset_brush ) + 2 * brush_size * direct_row + 1 * direct_col )->green = 204;
	(( lpRGB + offset_brush ) + 2 * brush_size * direct_row + 1 * direct_col )->blue  = 204;
    (( lpRGB + offset_brush ) + 2 * brush_size * direct_row + 2 * direct_col )->red   = 153;
    (( lpRGB + offset_brush ) + 2 * brush_size * direct_row + 2 * direct_col )->green = 153;
	(( lpRGB + offset_brush ) + 2 * brush_size * direct_row + 2 * direct_col )->blue  = 153;
}

//////////////////////////////////////////////////////////////////////////////
void glitter2( LPRGB lpRGB, ULONG offset_brush, int count, int brush_size )
//////////////////////////////////////////////////////////////////////////////
{
    int direct_row = 1, direct_col = 1;     // default is to draw from l-r and t-b
    int gh = 3, gv = 3;                     // 3x3 glitter rect

    if( brush_size < gh || brush_size < gv )                // if too small, quit
        return;

    if(( offset_brush % brush_size ) > brush_size - gh )    // if too close to right, flip x
        direct_col = -1;

    if( brush_size - ( offset_brush / brush_size ) < gv )   // if too close to bottom, flip y
        direct_row = -1;

	( lpRGB + offset_brush )->red   = 204;
    ( lpRGB + offset_brush )->green = 204;
	( lpRGB + offset_brush )->blue  = 204;
	(( lpRGB + offset_brush ) + 1 * direct_col )->red   = 153;
    (( lpRGB + offset_brush ) + 1 * direct_col )->green = 153;
	(( lpRGB + offset_brush ) + 1 * direct_col )->blue  = 153;
	(( lpRGB + offset_brush ) + 2 * direct_col )->red   = 204;
    (( lpRGB + offset_brush ) + 2 * direct_col )->green = 204;
	(( lpRGB + offset_brush ) + 2 * direct_col )->blue  = 204;

	(( lpRGB + offset_brush ) + brush_size * direct_row )->red   = 153;
    (( lpRGB + offset_brush ) + brush_size * direct_row )->green = 153;
	(( lpRGB + offset_brush ) + brush_size * direct_row )->blue  = 153;
	(( lpRGB + offset_brush ) + brush_size * direct_row + 1 * direct_col )->red   = 255;
    (( lpRGB + offset_brush ) + brush_size * direct_row + 1 * direct_col )->green = 255;
	(( lpRGB + offset_brush ) + brush_size * direct_row + 1 * direct_col )->blue  = 255;
	(( lpRGB + offset_brush ) + brush_size * direct_row + 2 * direct_col )->red   = 153;
    (( lpRGB + offset_brush ) + brush_size * direct_row + 2 * direct_col )->green = 153;
	(( lpRGB + offset_brush ) + brush_size * direct_row + 2 * direct_col )->blue  = 153;

	(( lpRGB + offset_brush ) + 2 * brush_size * direct_row )->red   = 204;
    (( lpRGB + offset_brush ) + 2 * brush_size * direct_row )->green = 204;
	(( lpRGB + offset_brush ) + 2 * brush_size * direct_row )->blue  = 204;
	(( lpRGB + offset_brush ) + 2 * brush_size * direct_row + 1 * direct_col )->red   = 153;
    (( lpRGB + offset_brush ) + 2 * brush_size * direct_row + 1 * direct_col )->green = 153;
	(( lpRGB + offset_brush ) + 2 * brush_size * direct_row + 1 * direct_col )->blue  = 153;
    (( lpRGB + offset_brush ) + 2 * brush_size * direct_row + 2 * direct_col )->red   = 204;
    (( lpRGB + offset_brush ) + 2 * brush_size * direct_row + 2 * direct_col )->green = 204;
	(( lpRGB + offset_brush ) + 2 * brush_size * direct_row + 2 * direct_col )->blue  = 204;
}

//////////////////////////////////////////////////////////////////////////////
void swirlRGB( LPRGB lpRGB, int count, int brush_size )
//////////////////////////////////////////////////////////////////////////////
{
    LPBYTE lpSampleBuffer;
    BYTE color_val;
    int index;
    int gh = 6, gv = 6;                     // h x v swirl rect

    if( brush_size < gh || brush_size < gv )                // if too small, quit!
        return;

    if( brush_size > MAX_BRUSH_SIZE )
        brush_size = MAX_BRUSH_SIZE;

    // allocate buffer for sampled swirl data
    if( !( lpSampleBuffer = Alloc( count )))
    {
	    Message( IDS_EMEMALLOC );
	    return;
    }

    clr( lpSampleBuffer, count );

    sample_table( lpSampleBuffer, count, brush_size );

    for( index = 0; index < count; index++, lpRGB++, lpSampleBuffer++ )
    {
        if( *lpSampleBuffer == 0 )
            continue;

        color_val = *lpSampleBuffer % 4;    // four color values used for spiral

        switch( color_val )
        {
            case 1:
                color_val = ( BYTE )color_index1;
                break;

            case 2:
                color_val = ( BYTE )color_index2;
                break;

            case 3:
                color_val = ( BYTE )color_index3;
                break;

            case 0:
                color_val = ( BYTE )color_index0;
                break;
        }
        swirl( lpRGB, color_val );
    }
    FreeUp( lpSampleBuffer );

    spin_the_brush();
}

//////////////////////////////////////////////////////////////////////////////
void swirl( LPRGB lpRGB, BYTE color_val )
//////////////////////////////////////////////////////////////////////////////
{
	lpRGB->red   =  color_val;
    lpRGB->green =  color_val;
	lpRGB->blue  =  color_val;
}

//////////////////////////////////////////////////////////////////////////////
void spin_the_brush( void )
//////////////////////////////////////////////////////////////////////////////
{
    //bump up the spiral colors
    if(( color_index1 += 51 ) > 255 )
        color_index1 = 102;

    if(( color_index2 += 51 ) > 255 )
        color_index2 = 102;

    if(( color_index3 += 51 ) > 255 )
        color_index3 = 102;

    if(( color_index0 += 51 ) > 255 )
        color_index0 = 102;
}

//////////////////////////////////////////////////////////////////////////////
void save_fade_values()
//////////////////////////////////////////////////////////////////////////////
{
    MBrushSize        =  BrushSize; 
    MFadeBrushSize    =  FadeBrushSize; 
    MLastOpacity      =  LastOpacity;
    MFadeOpacityValue =  FadeOpacityValue;
    MBrushFull        =  BrushFull;
    MBrushHalf        =  BrushHalf;
}

//////////////////////////////////////////////////////////////////////////////
void restore_fade_values()
//////////////////////////////////////////////////////////////////////////////
{
    BrushSize         =  MBrushSize;
    FadeBrushSize     =  MFadeBrushSize;
    LastOpacity       =  MLastOpacity;
    FadeOpacityValue  =  MFadeOpacityValue;
    BrushFull         =  MBrushFull;
    BrushHalf         =  MBrushHalf;
}

