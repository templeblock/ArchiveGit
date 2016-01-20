// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®FD1¯®PL1¯®TP0¯®BT0¯*/
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"
#include "mclib.h"

/************************************************************************/
BOOL CutImage( lpClipBoard )
/************************************************************************/
LPTR lpClipBoard;
{
FNAME ClipFile;
LPFRAME lpFrame;
LPMASK lpMask;
LPSHAPE lpShape;
LPPOINT	lpPoints;
LPTR lpBuffer;
RECT ClipRect;
int fp, width, i;

if ( !( lpFrame = frame_set( NULL ) ) )
	return( NO );
if ( !( lpMask = lpFrame->WriteMask ) )
	return( NO );
if ( !( lpShape = lpMask->lpHeadShape ) )
	return(NO );
if ( !( lpPoints = lpShape->lpPoints ) )
	return(NO );

/* Copy bounding rectangle from mask structure */
mask_rect( &ClipRect );
ClipRect.top    = bound( ClipRect.top, 0, lpImage->nlin-1 );
ClipRect.bottom = bound( ClipRect.bottom, 0, lpImage->nlin-1 );
ClipRect.left   = bound( ClipRect.left, 0, lpImage->npix-1 );
ClipRect.right  = bound( ClipRect.right, 0, lpImage->npix-1 );

// Allocate a line buffer
width = RectWidth( &ClipRect );
if ( !AllocLines( &lpBuffer, 1, width, DEPTH ) )
	return( NO );

// Open the clipboard file
lstrcpy( ClipFile, Control.ProgHome );
lstrcat( ClipFile, lpClipBoard );
if ( (fp = _lcreat(ClipFile, 0)) < 0)
	{
	FreeUp( lpBuffer );
	return( NO );
	}

/* The clipboard format is as follows:
	bounding rectangle for the object
	the mask information
		the number of points in shape 1 (-1 if end of shape list)
		the circle flag for shape 1
		the points for shape 1
		... etc. for additional shapes
	the image information
		the depth of the image
		the image data
*/

/* Write out the bounding rectangle */
if ( _lwrite( fp, (LPTR)&ClipRect, sizeof(RECT) ) < 0 )
	goto errc;

/* Walk through all of the shapes in the mask, and write out all the points */
while ( lpShape )
	{
	if ( !( lpPoints = lpShape->lpPoints ) || !lpShape->nPoints )
		{
		lpShape = lpShape->lpNext;
		continue;
		}
	if ( _lwrite( fp, (LPTR)&lpShape->nPoints, sizeof(int) ) < 0 )
		goto errc;
	if ( _lwrite( fp, (LPTR)&lpShape->fCircle, sizeof(int) ) < 0 )
		goto errc;
	if ( _lwrite( fp, (LPTR)lpPoints, lpShape->nPoints*sizeof(POINT) ) < 0 )
		goto errc;
	lpShape = lpShape->lpNext;
	}

/* Write out the end of shapes indicator */
i = -1;
if ( _lwrite( fp, (LPTR)&i, sizeof(int) ) < 0 )
	goto errc;

/* Write out the depth of the data */
i = DEPTH;
if ( _lwrite( fp, (LPTR)&i, sizeof(int) ) < 0 )
	goto errc;

/* Read the high resolution image data and store it in the file */
for ( i=ClipRect.top; i<=ClipRect.bottom; i++ )
	{
	AstralClockCursor( i-ClipRect.top, RectHeight( &ClipRect ) );
	if ( !readimage( i, ClipRect.left, ClipRect.right, width, lpBuffer ) )
		goto errc;
	if ( _lwrite( fp, lpBuffer, width * DEPTH ) < 0 )
		goto errc;
	}

FreeUp( lpBuffer );
_lclose( fp );
return( YES );

errc:
FreeUp( lpBuffer );
_lclose( fp );
return( NO );
}


#ifdef UNUSED
/***********************************************************************/
void WindowsBitmap( hWindow )
/***********************************************************************/
HWND	hWindow;
{
HBITMAP	hBitmap;
BITMAP	bm;
LPTR	lpbmapmem,lpmovbmap;
DWORD	dwLength;
char	stry[20],clipfile[MAX_FNAME_LEN];
int i,sfh;
LPWORD	o_b_ptr;

OpenClipboard(hWindow);
if ( !(hBitmap = GetClipboardData(CF_BITMAP)) )
	{
	CloseClipboard();
	Message( IDS_CLIPEMPTY );
	goto bmap_error;
	}
GetObject( hBitmap, sizeof(BITMAP), (LPSTR) &bm) ;
bm.bmBits = 0;
if ((bm.bmPlanes != 1) || (bm.bmBitsPixel != 1))
	{
	CloseClipboard();
	Message( IDS_ECLIPMULTIBIT );
	goto bmap_error;
	}
dwLength = (DWORD) (bm.bmWidthBytes * bm.bmHeight);
if ( !(lpbmapmem = Alloc( dwLength )) )
	{
	CloseClipboard();
	Message( IDS_EMEMALLOC );
	goto bmap_error;
	}
GetBitmapBits(hBitmap, (DWORD) (bm.bmWidthBytes * bm.bmHeight),	lpbmapmem);
strcpy(clipfile,Control.ClipDisk); /* get dest drv */
strcat(clipfile,"PICTURE.CL");
strcat(clipfile,itoa(Control.Clipboard-IDM_CLIP1+1, stry, 10 ));
if ( (sfh = _lcreat( clipfile, 0 )) < 0)
	{
	Message( IDS_ECLIPOPEN );
	FreeUp(lpbmapmem);
	goto bmap_error;
	}
o_b_ptr = (LPWORD)LineBuffer[0];
*o_b_ptr++ = 0;  /* write clip file min x*/
*o_b_ptr++ = bm.bmWidth-1; /* write clip file max x*/
*o_b_ptr++ = 0;  /* write clip file min y*/
*o_b_ptr++ = bm.bmHeight-1; /* write clip file max y*/
*o_b_ptr++ = 8; /* write the veccnt */
/* write the edges */
*o_b_ptr++ = 0; /* write xstart */
*o_b_ptr++ = 0; /* write ystart */
*o_b_ptr++ = bm.bmWidth-1; /* write xend */
*o_b_ptr++ = 0; /* write ystart */
*o_b_ptr++ = bm.bmWidth-1; /* write xend */
*o_b_ptr++ = 0; /* write ystart */
*o_b_ptr++ = bm.bmWidth-1; /* write xend */
*o_b_ptr++ = bm.bmHeight-1; /* write yend */
*o_b_ptr++ = bm.bmWidth-1; /* write xend */
*o_b_ptr++ = bm.bmHeight-1; /* write yend */
*o_b_ptr++ = 0; /* write xstart */
*o_b_ptr++ = bm.bmHeight-1; /* write yend */
*o_b_ptr++ = 0; /* write xstart */
*o_b_ptr++ = bm.bmHeight-1; /* write yend */
*o_b_ptr++ = 0; /* write xstart */
*o_b_ptr++ = 0; /* write ystart */

if (_lwrite(sfh,LineBuffer[0],42) != 42) /* write clip file header */
	{
	Message( IDS_ECLIPWRITE );
	_lclose(sfh);
	FreeUp(lpbmapmem);
	goto bmap_error;
	}

lpmovbmap = lpbmapmem; /* set moving output pointer equal to the bitmap */
for (i=0; i<bm.bmHeight; i++)
	{
	AstralClockCursor( i, bm.bmHeight );
	la2con(lpmovbmap, bm.bmWidth, LineBuffer[0],1);
	if (_lwrite(sfh,LineBuffer[0],bm.bmWidth) != bm.bmWidth )
		{
		Message( IDS_ECLIPWRITE );
		FreeUp(lpbmapmem);
		_lclose(sfh);
		goto bmap_error;
		}
	lpmovbmap += bm.bmWidthBytes;
	}

_lclose(sfh);
FreeUp(lpbmapmem);
bmap_error:
CloseClipboard();
}
#endif
