// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
/*®FD1¯®PL1¯®TP0¯®BT0¯*/
#include <windows.h>
#include "astral.h"
#include "data.h"
#include "routines.h"

/************************************************************************/
LPTR GetResource( idResource, lphResource, lpHotx, lpHoty, lpSizex, lpSizey )
/************************************************************************/
int idResource;
LPHANDLE lphResource;
LPINT lpHotx, lpHoty, lpSizex, lpSizey;
{
HANDLE hResource;
LPTR lpResource, lp;
BYTE c;
int i;
extern HANDLE hInstAstral;

if ( Control.HalfPint )
	idResource++;
if ( !(hResource = FindResource( hInstAstral, MAKEINTRESOURCE(idResource),
								RT_RCDATA ))
  || !(hResource = LoadResource( hInstAstral, hResource ))
  || !(lpResource = LockResource( hResource )) )
	{
	*lphResource = NULL;
	return( NULL );
	}

/* Pass back the resource handle, so the caller can unlock it later */
*lphResource = hResource;

/* Copy out the hotspot */
*lpHotx = *(LPINT)lpResource; lpResource += sizeof(int);
*lpHoty = *(LPINT)lpResource; lpResource += sizeof(int);

/* Copy out the resource size */
*lpSizex = *(LPINT)lpResource; lpResource += sizeof(int);
*lpSizey = *(LPINT)lpResource; lpResource += sizeof(int);

/* Subtract out '0' if we haven't already done so on a previous activation */
lp = lpResource;
if ( *lp >= ' ' )
	{
	i = (*lpSizex) * (*lpSizey);
	while( --i >= 0 )
		{
		if ( (c = *lp) <= '0' )
			*lp++ = 0;
		else	*lp++ = c - '0';
		}
	}

return( lpResource );
}


static int sx, sy, hx, hy, ix, iy;
static int idCursor;
static BOOL CursorOn, SavedInArea1;
static BYTE Cursor[32*32], SaveArea1[32*32], SaveArea2[32*32];

/************************************************************************/
int NewCursor( idNewCursor )
/************************************************************************/
int idNewCursor;
{
int idPrevCursor;
LPTR lpCursor;
HANDLE hCursor;

if ( idNewCursor == idCursor )
	return( idCursor );
idPrevCursor = idCursor;

/* Get the cursor resource (32 x 32 max) */
if ( !(lpCursor = GetResource( idNewCursor, &hCursor, &hx, &hy, &ix, &iy )) )
	return( idPrevCursor );
copy( lpCursor, Cursor, ix*iy );
UnlockResource( hCursor );
idCursor = idNewCursor;

if ( CursorOn )
	cursor_move( m3, m4 ); /* display the new cursor */
return( idPrevCursor );
}


/************************************************************************/
void cursor_move(cx, cy)
/************************************************************************/
int cx, cy;
{
int x, y, nx, ny, delta, x1, x2, y1, y2;
LPTR lpOldSaveArea, lpNewSaveArea, lpCur;
LPTR OldSaveArea, NewSaveArea;
BYTE c;

if ( !CursorOn )
	{
	cursor_on( cx, cy );
	return;
	}

/* Update cx and cy relative to the cursor's hotspot */
cx -= hx;
cy -= hy;

if ( SavedInArea1 )
	{ OldSaveArea = SaveArea1; NewSaveArea = SaveArea2; }
else	{ OldSaveArea = SaveArea2; NewSaveArea = SaveArea1; }

// Save under the new cursor location
nx = cx + ix;
ny = cy + iy;
lpNewSaveArea = NewSaveArea;
lpCur = Cursor;
for ( y=cy; y<ny; y++ )
    for ( x=cx; x<nx; x++ )
	if ( *lpCur++ && ONSCREEN( x, y ) )
		*lpNewSaveArea++ = scrread( x, y );
	else	*lpNewSaveArea++ = EMPTY;

/* Find the overlap area between the old save area and the new one (if any) */
if ( cx > sx )
	{ x1 = cx; x2 = sx+ix; }
else	{ x1 = sx; x2 = cx+ix; }
if ( cy > sy )
	{ y1 = cy; y2 = sy+iy; }
else	{ y1 = sy; y2 = cy+iy; }

/* Go through the old save area (within the overlap) */
/* - if we'll draw a pixel with the new cursor */
/* copy it to the new save area and zap it in the old save area */
delta = ix - (x2-x1);
lpOldSaveArea = OldSaveArea + (iy * (y1-sy)) + (x1-sx);
lpNewSaveArea = NewSaveArea + (iy * (y1-cy)) + (x1-cx);
for ( y=y1; y<y2; y++ )
	{
	for ( x=x1; x<x2; x++ )
		{
		if ( *lpNewSaveArea != EMPTY && *lpOldSaveArea != EMPTY )
			{
			*lpNewSaveArea = *lpOldSaveArea;
			*lpOldSaveArea = EMPTY;
			}
		lpNewSaveArea++;
		lpOldSaveArea++;
		}
	lpOldSaveArea += delta;
	lpNewSaveArea += delta;
	}

// Restore the old save area
lpOldSaveArea = OldSaveArea;
nx = sx + ix;
ny = sy + iy;
for ( y=sy; y<ny; y++ )
    for ( x=sx; x<nx; x++ )
	if ( (c = *lpOldSaveArea++) != EMPTY )
		screen( x, y, c );

// Draw the new cursor
lpCur = Cursor;
nx = cx + ix;
ny = cy + iy;
for ( y=cy; y<ny; y++ )
    for ( x=cx; x<nx; x++ )
	if ( (c = *lpCur++) && ONSCREEN( x, y ) )
		screen( x, y, c-1 );

SavedInArea1 = !SavedInArea1;
sx = cx; sy = cy;
CursorOn = ON;
}


/************************************************************************/
void cursor_on(cx, cy)
/************************************************************************/
int cx, cy;
{
REG int x, y, nx, ny;
REG LPTR lpSaveArea, lpCur;
REG BYTE c;

if ( CursorOn )
	cursor_off();

/* Update cx and cy relative to the cursor's hotspot */
cx -= hx;
cy -= hy;

/* Draw the new cursor and save what is under it */
lpSaveArea = SaveArea1;
SavedInArea1 = YES;
lpCur = Cursor;
nx = cx + ix;
ny = cy + iy;
for ( y=cy; y<ny; y++ )
    for ( x=cx; x<nx; x++ )
	{
	if ( (c = *lpCur++) && ONSCREEN( x, y ) )
		{
		*lpSaveArea++ = scrread( x, y );
		screen( x, y, c-1 );
		}
	else	*lpSaveArea++ = EMPTY;
	}

sx = cx; sy = cy;
CursorOn = ON;
}


/************************************************************************/
void cursor_off()
/************************************************************************/
{
REG int x, y, nx, ny;
REG LPTR lpSaveArea;
REG BYTE c;

if ( !CursorOn )
	return;

/* Restore what was under the current cursor */
lpSaveArea = ( SavedInArea1 ? SaveArea1 : SaveArea2 );
nx = sx + ix;
ny = sy + iy;
for ( y=sy; y<ny; y++ )
    for ( x=sx; x<nx; x++ )
	if ( (c = *lpSaveArea++) != EMPTY )
		screen( x, y, c );
CursorOn = OFF;
}


/************************************************************************/
void DrawControl( idControl, flag, cBackColor )
/************************************************************************/
int idControl, flag;
BYTE cBackColor;
{
int x, y, cx, cy, nx, ny;
REG LPTR lpControl, lpLine;
BYTE cHiliteColor, Line[MAX_STR_LEN], c;
HANDLE hControl;

if ( !(lpControl = GetResource( idControl, &hControl, &cx, &cy, &nx, &ny )) )
	return;

if ( cBackColor == MENUCOLOR )
	cHiliteColor = MENUREVERSE;
else	cHiliteColor = LRED;

/* The hotspot is the screen location */
if ( cx < 0 ) cx += (DISP_RIGHT+1);
if ( cy < 0 ) cy += (DISP_BOTTOM+1);
nx += cx;
ny += cy;
for ( y=cy; y<ny; y++ )
	{
	lpLine = Line;
	for ( x=cx; x<nx; x++ )
		{
		if ( !(c = *lpControl++) )
			c = ( flag ? cHiliteColor : cBackColor );
		else
		if ( !(c -= 1) )
			c = ( flag ? TEXTREVERSE : TEXTCOLOR );
		*lpLine++ = c;
		}
	scrwrite( cx, y, Line, x-cx, 0 );
	}

UnlockResource( hControl );
}


/************************************************************************/
BOOL HitControl( idControl )
/************************************************************************/
int idControl;
{
int x, y, nx, ny;
LPTR lpControl;
HANDLE hControl;

if ( !(lpControl = GetResource( idControl, &hControl, &x, &y, &nx, &ny )) )
	return( NO );
UnlockResource( hControl );

/* The hotspot is the screen location */
if ( x < 0 ) x += DISP_RIGHT+1;
if ( y < 0 ) y += DISP_BOTTOM+1;
nx += x;
ny += y;

return( m3 >= x && m3 <= nx && m4 >= y && m4 <= ny );
}


