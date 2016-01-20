#ifdef UNUSED

static int sx, sy, hx, hy, ix, iy;
static BOOL fBitmapDisplayed, SavedInArea1;
static BYTE Cursor[32*32], SaveArea1[32*32], SaveArea2[32*32];

/************************************************************************/
void StartDragBitmap(cx, cy)
/************************************************************************/
int cx, cy;
{
REG int x, y, nx, ny;
REG LPTR lpSaveArea, lpCur;
REG BYTE c;

if ( fBitmapDisplayed )
	EndDragBitmap();

/* Update cx and cy relative to the cursor's hotspot */
cx -= hx;
cy -= hy;

/* Draw the new cursor and save what is under it */
lpSaveArea = SaveArea1;
SavedInArea1 = YES;
lpCur = Cursor;
nx = cx + ix;
ny = cy + iy;

BitBlt( Screen, cx, cy, nx, ny, lpSaveArea, 0, 0, nx, ny );
BitBlt( lpCur, 0, 0, nx, ny, Screen, cx, cy, nx, ny );
//for ( y=cy; y<ny; y++ )
//    for ( x=cx; x<nx; x++ )
//	{
//	if ( (c = *lpCur++) && ONSCREEN( x, y ) )
//		{
//		*lpSaveArea++ = scrread( x, y );
//		screen( x, y, c-1 );
//		}
//	else	*lpSaveArea++ = EMPTY;
//	}

sx = cx; sy = cy;
fBitmapDisplayed = ON;
}


/************************************************************************/
void UpdateDragBitmap(cx, cy)
/************************************************************************/
int cx, cy;
{
int x, y, nx, ny, delta, x1, x2, y1, y2;
LPTR lpOldSaveArea, lpNewSaveArea, lpCur;
LPTR OldSaveArea, NewSaveArea;
BYTE c;

if ( !fBitmapDisplayed )
	{
	StartDragBitmap( cx, cy );
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

BitBlt( Screen, cx, cy, nx, ny, lpNewSaveArea, 0, 0, nx, ny );
//for ( y=cy; y<ny; y++ )
//    for ( x=cx; x<nx; x++ )
//	if ( *lpCur++ && ONSCREEN( x, y ) )
//		*lpNewSaveArea++ = scrread( x, y );
//	else	*lpNewSaveArea++ = EMPTY;

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

BitBlt( Screen, cx, cy, nx, ny, lpNewSaveArea, 0, 0, nx, ny );
//for ( y=y1; y<y2; y++ )
//	{
//	for ( x=x1; x<x2; x++ )
//		{
//		if ( *lpNewSaveArea != EMPTY && *lpOldSaveArea != EMPTY )
//			{
//			*lpNewSaveArea = *lpOldSaveArea;
//			*lpOldSaveArea = EMPTY;
//			}
//		lpNewSaveArea++;
//		lpOldSaveArea++;
//		}
//	lpOldSaveArea += delta;
//	lpNewSaveArea += delta;
//	}

// Restore the old save area
lpOldSaveArea = OldSaveArea;
nx = sx + ix;
ny = sy + iy;

BitBlt( lpOldSaveArea, 0, 0, nx, ny, Screen, sx, sy, nx, ny );
//for ( y=sy; y<ny; y++ )
//    for ( x=sx; x<nx; x++ )
//	if ( (c = *lpOldSaveArea++) != EMPTY )
//		screen( x, y, c );

// Draw the new cursor
lpCur = Cursor;
nx = cx + ix;
ny = cy + iy;

BitBlt( lpCur, 0, 0, nx, ny, Screen, cx, cy, nx, ny );
//for ( y=cy; y<ny; y++ )
//    for ( x=cx; x<nx; x++ )
//	if ( (c = *lpCur++) && ONSCREEN( x, y ) )
//		screen( x, y, c-1 );

SavedInArea1 = !SavedInArea1;
sx = cx; sy = cy;
fBitmapDisplayed = ON;
}


/************************************************************************/
void EndDragBitmap()
/************************************************************************/
{
REG int x, y, nx, ny;
REG LPTR lpSaveArea;
REG BYTE c;

if ( !fBitmapDisplayed )
	return;

/* Restore what was under the current cursor */
lpSaveArea = ( SavedInArea1 ? SaveArea1 : SaveArea2 );
nx = sx + ix;
ny = sy + iy;

BitBlt( lpSaveArea, 0, 0, nx, ny, Screen, sx, sy, nx, ny );
//for ( y=sy; y<ny; y++ )
//    for ( x=sx; x<nx; x++ )
//	if ( (c = *lpSaveArea++) != EMPTY )
//		screen( x, y, c );
fBitmapDisplayed = OFF;
}


#endif
