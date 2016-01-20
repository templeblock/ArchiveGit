#include <windows.h>
#include "proto.h"
#include "control.h"
#include "worlds.h"

#define MAPCODE(x,y) *(lpMap + ((x) + ((y) * xCells)))

// The CONSTRUCTOR
//************************************************************************
CWorld::CWorld()
//************************************************************************
{
	hWndDraw = NULL;
	lpMap = NULL;
	xCells = yCells = 0;
	lViewWidth = lViewHeight = 0;
	ptCellLocation.x = 0;
	ptCellLocation.y = 0;
	ptViewLocation.x = 0;
	ptViewLocation.y = 0;
	ptTempLocation = ptViewLocation;
	iZoomFactor = 2;
	iDirection = 12;
	hPenG = CreatePen( PS_SOLID, 1, RGB(0,255,0) );
	hPenR = CreatePen( PS_SOLID, 1, RGB(255,0,0) );
}

// The DESTRUCTOR
//************************************************************************
CWorld::~CWorld()
//************************************************************************
{
	if ( lpMap )
		FreeUp( lpMap );
	if ( hPenG )
		DeleteObject( hPenG );
	if ( hPenR )
		DeleteObject( hPenR );
}

//***********************************************************************
BOOL CWorld::Init( HWND hWnd, ITEMID idMapData )
//***********************************************************************
{
	if ( hWnd ) 
		hWndDraw = hWnd;

	if ( !idMapData )
		if ( !(idMapData = GetWindowWord( hWndDraw, GWW_ICONID )) )
			idMapData = GET_WINDOW_ID( hWndDraw );

	HINSTANCE hInstance = GetWindowInstance( hWndDraw ); // GetApp()->GetInstance();
	if ( !hInstance )
		return( NO );

	// Load and lock the map resource data
	HGLOBAL hResource;
	if ( !(hResource = (HGLOBAL)FindResource( hInstance, MAKEINTRESOURCE(idMapData), "MAP" )) )
		return( NO );
	if ( !(hResource = LoadResource( hInstance, (HRSRC)hResource )) )
		return( NO );
	LPWORD lpMapData;
	if ( !(lpMapData = (LPWORD)LockResource( hResource )) )
		{
		FreeResource( hResource );
		return( NO );
		}

	xCells = *lpMapData++;
	yCells = *lpMapData++;
	ComputeMapDimensions();

	POINT pt;
	pt.x = *lpMapData++;
	pt.y = *lpMapData++;
	SetCellLocation( &pt, NO/*bScroll*/ );

	long lLength = xCells * yCells;
	if ( lpMap = (LPSTR)Alloc( lLength ) )
		hmemcpy( lpMap, lpMapData, lLength );

	UnlockResource( hResource );
	FreeResource( hResource );
	return( YES );
}

//***********************************************************************
void CWorld::ComputeCellSizes( LPINT lpxDelta, LPINT lpiWidth, LPINT lpiHeight )
//***********************************************************************
{
	#define MIN_DELTAX	12 // width - 25%
	#define MIN_WIDTH	16
	#define MIN_HEIGHT	14

	if ( iZoomFactor <= -3 )
	{
		*lpxDelta = 2;
		*lpiWidth = 2;
		*lpiHeight = 2;
	}
	else
	if ( iZoomFactor <= -2 )
	{
		*lpxDelta = 6;
		*lpiWidth = 8;
		*lpiHeight = 7;
	}
	else
	{
		*lpxDelta = ScaleInt( MIN_DELTAX, iZoomFactor );
		*lpiWidth = ScaleInt( MIN_WIDTH, iZoomFactor );
		*lpiHeight = ScaleInt( MIN_HEIGHT, iZoomFactor );
	}
}

//***********************************************************************
void CWorld::ComputeMapDimensions( void )
//***********************************************************************
{
	int xDelta, iWidth, iHeight;
	ComputeCellSizes( &xDelta, &iWidth, &iHeight );
	lViewWidth = (xCells * xDelta) + (iWidth - xDelta);
	lViewHeight = yCells * iHeight;
}

//***********************************************************************
void CWorld::SetCellLocation( LPPOINT lpPoint, BOOL bScroll )
//***********************************************************************
{
	POINT pt = *lpPoint;
	if ( pt.x >= xCells )	pt.x = xCells - 1;
	if ( pt.y >= yCells )	pt.y = yCells - 1;
	if ( pt.x < 0 )			pt.x = 0;
	if ( pt.y < 0 )			pt.y = 0;
	ptCellLocation = pt;
	
	// Center the view on this cell
	long l;
	l = (lViewWidth * pt.x) + lViewWidth/2;
	pt.x = (int)(l / xCells);
	l = (lViewHeight * pt.y) + lViewHeight/2;
	pt.y = (int)(l / yCells);
	SetViewLocation( &pt, bScroll );
}

//***********************************************************************
int CWorld::MoveCellLocation( void )
//***********************************************************************
{
	POINT pt;
	GetCellLocation( &pt );
	
	switch ( iDirection )
	{ // fix the direction; shouldn't be necessary
		case 1:
			iDirection = 12;
			break;
		case 3:
			iDirection = 2;
			break;
		case 5:
			iDirection = 4;
			break;
		case 7:
			iDirection = 6;
			break;
		case 9:
			iDirection = 8;
			break;
		case 11:
			iDirection = 10;
			break;
	}

	switch ( iDirection )
	{ // remember, odd columns in the map get pushed down
		case 2:
			pt.x++;
			if ( pt.x & 1 )
				pt.y--;
			break;
		case 4:
			if ( pt.x & 1 )
				pt.y++;
			pt.x++;
			break;
		case 6:
			pt.y++;
			break;
		case 8:
			if ( pt.x & 1 )
				pt.y++;
			pt.x--;
			break;
		case 10:
			pt.x--;
			if ( pt.x & 1 )
				pt.y--;
			break;
		case 12:
			pt.y--;
			break;
	}

	SetCellLocation( &pt, YES/*bScroll*/ );
	return( MAPCODE(pt.x,pt.y) );
}

//***********************************************************************
void CWorld::SetDirection( int iNewDirection )
//***********************************************************************
{
	RECT rect;
	GetClientRect( hWndDraw, &rect );
	int dx = RectWidth(&rect)/2;
	int dy = RectHeight(&rect)/2;
	HDC hDC = GetDC( hWndDraw );
	if ( hDC )
		DrawDirection( hDC, NO, dx, dy );
	if ( iNewDirection >= 1 && iNewDirection <= 12 )
		iDirection = iNewDirection;
	if ( hDC )
	{
		DrawDirection( hDC, YES, dx, dy );
		ReleaseDC( hWndDraw, hDC );
	}
}

//***********************************************************************
int CWorld::GetDirection( void )
//***********************************************************************
{
	return( iDirection );
}

//***********************************************************************
void CWorld::GetCellLocation( LPPOINT lpPoint )
//***********************************************************************
{
	*lpPoint = ptCellLocation;
}

//***********************************************************************
void CWorld::SetViewLocation( LPPOINT lpPoint, BOOL bScroll )
//***********************************************************************
{
	if ( bScroll ) // If scrolling to the new point, remember the old point
		ptTempLocation = ptViewLocation;

	ptViewLocation = *lpPoint;

	if ( !bScroll ) // If not scrolling, set the old point to the new point
		ptTempLocation = ptViewLocation;
}

//***********************************************************************
void CWorld::GetViewLocation( LPPOINT lpPoint )
//***********************************************************************
{
	*lpPoint = ptViewLocation;
}

//***********************************************************************
int CWorld::ScaleInt( int iValue, int iScale )
//***********************************************************************
{
	if ( iScale > 1 )
		return( iValue * iScale );
	if ( iScale < -1 )
	{
		iScale = -iScale;
		return( (iValue + (iScale/2)) / iScale );
	}
	return( iValue );
}

//***********************************************************************
void CWorld::SetZoomFactor( int iNewZoomFactor )
//***********************************************************************
{
	if ( !iNewZoomFactor )
		return;

	iZoomFactor = iNewZoomFactor;
	ComputeMapDimensions();
	SetCellLocation( &ptCellLocation, NO/*bScroll*/ );
}

//***********************************************************************
int CWorld::GetZoomFactor( void )
//***********************************************************************
{
	return( iZoomFactor );
}

/***********************************************************************/
void CWorld::Draw( HDC hDC, LPRECT lpClipRect )
/***********************************************************************/
{
	if ( !hDC )
		return;

//	LPSCENE lpScene = CScene::GetScene( GetParent(hWndDraw) );
//	if ( lpScene )
//	{
//		RECT rect = *lpClipRect;
//		MapWindowPoints( hWndDraw, GetParent(hWndDraw), (LPPOINT)&rect, 2 );
//		lpScene->Paint( hDC, &rect, (LPPOINT)lpClipRect );
//	}

	// Do clipping to the background monitor
//	POINT pt[4];
//	pt[0].x =  20; pt[0].y =   0;
//	pt[1].x =   0; pt[1].y = 159;
//	pt[2].x = 168; pt[2].y = 188;
//	pt[3].x = 196; pt[3].y =  32;
//	HRGN hRegion = CreatePolygonRgn( pt, 4, ALTERNATE );
//	SelectClipRgn( hDC, hRegion );
//	DeleteObject( hRegion );

	RECT rDraw;
	GetClientRect( hWndDraw, &rDraw );
	int dx = RectWidth(&rDraw)/2;
	int dy = RectHeight(&rDraw)/2;

	HPEN hOldPen = (HPEN)SelectObject( hDC, hPenG );

	OffsetRect( &rDraw, ptViewLocation.x - dx, ptViewLocation.y - dy );

	POINT ptCell;
	GrowViewRect( &rDraw, &ptCell );
	OffsetRect( &rDraw, dx - ptViewLocation.x, dy - ptViewLocation.y );

	int xDelta, iWidth, iHeight;
	ComputeCellSizes( &xDelta, &iWidth, &iHeight );

	int xCellSaved = ptCell.x;
	for ( int y=rDraw.top; y<rDraw.bottom; y+=iHeight, ptCell.y++ )
	{
		if ( ptCell.y < 0 || ptCell.y >= yCells )
			continue;
		
		ptCell.x = xCellSaved;
		for ( int x=rDraw.left; x<rDraw.right; x+=xDelta, ptCell.x++ )
		{
			if ( ptCell.x < 0 || ptCell.x >= xCells )
				continue;

			// odd columns in the map get pushed down
			int yDelta = ( (ptCell.x & 1) ? iHeight/2 : 0 );
			int y1 = y + yDelta;

			char c = MAPCODE(ptCell.x,ptCell.y);
			switch ( c )
			{
				case 'H':
				{
					DrawHex( hDC, x, y1 );
					break;
				}
				case 'X':
				{
					DrawUnionJack( hDC, x, y1);
					break;
				}
				case ' ':
				{
					DrawCulDeSacs( hDC, x, y1, ptCell, yDelta, 1|2|4|8|16|32 );
					break;
				}
				case '|':
				{
					DrawVerticalTunnel( hDC, x, y1 );
					DrawCulDeSacs( hDC, x, y1, ptCell, yDelta, 2|4|16|32 );
					break;
				}
				case '.':
				{
					Draw10to4Tunnel( hDC, x, y1 );
					DrawCulDeSacs( hDC, x, y1, ptCell, yDelta, 1|2|8|16 );
					break;
				}
				case '/':
				{
					Draw2to8Tunnel( hDC, x, y1 );
					DrawCulDeSacs( hDC, x, y1, ptCell, yDelta, 1|4|8|32 );
					break;
				}
			}
		}
	}
	
	SelectObject( hDC, hOldPen );
	DrawDirection( hDC, YES, dx, dy );
}

   
/***********************************************************************/
void CWorld::DrawDirection( HDC hDC, BOOL bOn, int x, int y )
/***********************************************************************/
{
	static POINT pt1, pt2, pt3;
	static int iDir, yDelta;

	if ( bOn )
	{
		iDir = iDirection;
		if ( ptCellLocation.x & 1)
		{
			int xDelta, iWidth, iHeight;
			ComputeCellSizes( &xDelta, &iWidth, &iHeight );
			yDelta = iHeight/2;
		}
		else
			yDelta = 0;
	}

	y += yDelta;

	if ( iDir == 1 )
	{
		pt1.x = x+1;	pt1.y = y-2;
		pt2.x = x;		pt2.y = y+2;
		pt3.x = x-2;	pt3.y = y;
	}
	else
	if ( iDir == 2 )
	{
		pt1.x = x+2;	pt1.y = y-1;
		pt2.x = x-1;	pt2.y = y+2;
		pt3.x = x-2;	pt3.y = y-1;
	}
	else
	if ( iDir == 3 )
	{
		pt1.x = x+2;	pt1.y = y;
		pt2.x = x-2;	pt2.y = y+1;
		pt3.x = x-2;	pt3.y = y-1;
	}
	else
	if ( iDir == 4 )
	{
		pt1.x = x+2;	pt1.y = y+1;
		pt2.x = x-2;	pt2.y = y+1;
		pt3.x = x-1;	pt3.y = y-2;
	}
	else
	if ( iDir == 5 )
	{
		pt1.x = x+1;	pt1.y = y+2;
		pt2.x = x-2;	pt2.y = y;
		pt3.x = x;		pt3.y = y-2;
	}
	else
	if ( iDir == 6 )
	{
		pt1.x = x;		pt1.y = y+2;
		pt2.x = x-1;	pt2.y = y-2;
		pt3.x = x+1;	pt3.y = y-2;
	}
	else
	if ( iDir == 7 )
	{
		pt1.x = x-1;	pt1.y = y+2;
		pt2.x = x;		pt2.y = y-2;
		pt3.x = x+2;	pt3.y = y;
	}
	else
	if ( iDir == 8 )
	{
		pt1.x = x-2;	pt1.y = y+1;
		pt2.x = x+1;	pt2.y = y-2;
		pt3.x = x+2;	pt3.y = y+1;
	}
	else
	if ( iDir == 9 )
	{
		pt1.x = x-2;	pt1.y = y;
		pt2.x = x+2;	pt2.y = y-1;
		pt3.x = x+2;	pt3.y = y+1;
	}
	else
	if ( iDir == 10 )
	{
		pt1.x = x-2;	pt1.y = y-1;
		pt2.x = x+2;	pt2.y = y-1;
		pt3.x = x+1;	pt3.y = y+2;
	}
	else
	if ( iDir == 11 )
	{
		pt1.x = x-1;	pt1.y = y-2;
		pt2.x = x+2;	pt2.y = y;
		pt3.x = x;		pt3.y = y+2;
	}
	else
	if ( iDir == 12 )
	{
		pt1.x = x;		pt1.y = y-2;
		pt2.x = x+1;	pt2.y = y+2;
		pt3.x = x-1;	pt3.y = y+2;
	}
	else
		return;

	HPEN hPen = (bOn ? hPenR : (HPEN)GetStockObject(BLACK_PEN));
	HPEN hOldPen = (HPEN)SelectObject( hDC, hPen );
	MoveToEx( hDC, pt1.x, pt1.y, NULL );
	LineTo( hDC, pt2.x, pt2.y );
	LineTo( hDC, pt3.x, pt3.y );
	LineTo( hDC, pt1.x, pt1.y );
	SelectObject( hDC, hOldPen );
}

//***********************************************************************
void CWorld::UpdateView( void )
//***********************************************************************
{
	int sdx = ptViewLocation.x - ptTempLocation.x;
	int sdy = ptViewLocation.y - ptTempLocation.y;
	if ( !sdx && !sdy )
		return;

	HDC hDC = GetDC( hWndDraw );
	if ( hDC )
	{
		RECT rect;
		GetClientRect( hWndDraw, &rect );
		int dx = RectWidth(&rect)/2;
		int dy = RectHeight(&rect)/2;
		DrawDirection( hDC, NO, dx, dy );
		HRGN hRegion = CreateRectRgn( 0, 0, 0, 0 );
		BOOL b = ScrollDC( hDC, -sdx, -sdy, &rect/*lprcScroll*/,
			&rect/*lprcCLip*/, hRegion, NULL/*lprcUpdate*/ );
		InvalidateRgn( hWndDraw, hRegion, TRUE );
		UpdateWindow( hWndDraw );
		DeleteObject( hRegion );
		DrawDirection( hDC, YES, dx, dy );
		ReleaseDC( hWndDraw, hDC );
	}
	ptTempLocation = ptViewLocation;
}

//***********************************************************************
void CWorld::GrowViewRect( LPRECT lpRect, LPPOINT lpCellPoint )
//***********************************************************************
{
	if ( !lpRect )
		return;

	int xDelta, iWidth, iHeight;
	ComputeCellSizes( &xDelta, &iWidth, &iHeight );

	// make it one cell bigger to avoid missing pieces of a cell
	InflateRect( lpRect, iWidth, iHeight );

	// Compute the upper-left cell point
	POINT ptCell;
	ptCell.x = lpRect->left / xDelta;
	// odd columns in the map get pushed down
	int yDelta = ( (ptCell.x & 1) ? iHeight/2 : 0 );
	ptCell.y = (lpRect->top - yDelta) / iHeight;

	// Recalculate the upper-left view point
	lpRect->left = ptCell.x * xDelta;
	lpRect->top  = ptCell.y * iHeight;

	// Copy the cell point back to the caller
	if  ( lpCellPoint )
		*lpCellPoint = ptCell;

	// Compute the lower-right cell point
	ptCell.x = lpRect->right / xDelta;
	// odd columns in the map get pushed down
	yDelta = ( (ptCell.x & 1) ? iHeight/2 : 0 );
	ptCell.y = (lpRect->bottom - yDelta) / iHeight;

	// Recalculate the lower-right view point
	lpRect->right  = ++ptCell.x * xDelta;
	lpRect->bottom = ++ptCell.y * iHeight;
}

/***********************************************************************/
void CWorld::DrawHex( HDC hDC, int x, int y )
/***********************************************************************/
{
	if ( iZoomFactor <= -3 )
	{
		DrawBlob2x2( hDC, x, y );
		return;
	}
	if ( iZoomFactor <= -2 )
	{
		DrawBlob8x7( hDC, x, y );
		return;
	}
						//	 x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y
	static POINT ptFr[] = {	15, 9,16, 7,13, 2,12, 0, 6, 0, 4, 0, 1, 5, 0, 7, 3,12, 4,14,10,14,12,14 };
	static POINT ptTo[] = {	16, 7,15, 5,12, 0,10, 0, 4, 0, 3, 2, 0, 7, 1, 9, 4,14, 6,14,12,14,13,12 };

	int iCount = sizeof(ptFr) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr[i].x, iZoomFactor),
						y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );
		LineTo(   hDC,	x + ScaleInt( ptTo[i].x, iZoomFactor),
						y + ScaleInt( ptTo[i].y, iZoomFactor) );
	}
}

/***********************************************************************/
void CWorld::DrawUnionJack( HDC hDC, int x, int y )
/***********************************************************************/
{
	if ( iZoomFactor <= -3 )
	{
		DrawBlob2x2( hDC, x, y );
		return;
	}
	if ( iZoomFactor <= -2 )
	{
		DrawBlob8x7( hDC, x, y );
		return;
	}
						//	 x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y
	static POINT ptFr[] = {	15, 9,10, 7,13, 2, 9, 5, 6, 0, 7, 5, 1, 5, 6, 7, 3,12, 7, 9,10,14, 9, 9 };
	static POINT ptTo[] = {	10, 7,15, 5, 9, 5,10, 0, 7, 5, 3, 2, 6, 7, 1, 9, 7, 9, 6,14, 9, 9,13,12 };

	int iCount = sizeof(ptFr) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr[i].x, iZoomFactor),
						y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );
		LineTo(   hDC,	x + ScaleInt( ptTo[i].x, iZoomFactor),
						y + ScaleInt( ptTo[i].y, iZoomFactor) );
	}
}

/***********************************************************************/
void CWorld::Draw10to4Tunnel( HDC hDC, int x, int y )
/***********************************************************************/
{
	if ( iZoomFactor <= -3 )
	{
		DrawBlob2x2( hDC, x, y );
		return;
	}
	if ( iZoomFactor <= -2 )
	{
		MoveToEx( hDC, x, y, NULL );
		LineTo( hDC, x+3, y+2 );
		MoveToEx( hDC, x-1, y+1, NULL );
		LineTo( hDC, x+2, y+3 );
		return;
	}
						//	 x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y
	static POINT ptFr[] = {	 3, 2,13,12 };
	static POINT ptTo[] = {	15, 9, 1, 5 };

	int iCount = sizeof(ptFr) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr[i].x, iZoomFactor),
						y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );
		LineTo(   hDC,	x + ScaleInt( ptTo[i].x, iZoomFactor),
						y + ScaleInt( ptTo[i].y, iZoomFactor) );
	}
}

/***********************************************************************/
void CWorld::Draw2to8Tunnel( HDC hDC, int x, int y )
/***********************************************************************/
{
	if ( iZoomFactor <= -3 )
	{
		DrawBlob2x2( hDC, x, y );
		return;
	}
	if ( iZoomFactor <= -2 )
	{
		MoveToEx( hDC, x+2, y, NULL );
		LineTo( hDC, x-1, y+2 );
		MoveToEx( hDC, x+3, y+1, NULL );
		LineTo( hDC, x, y+3 );
		return;
	}
						//	 x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y
	static POINT ptFr[] = {	13, 2, 3,12 };
	static POINT ptTo[] = {	 1, 9,15, 5 };

	int iCount = sizeof(ptFr) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr[i].x, iZoomFactor),
						y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );
		LineTo(   hDC,	x + ScaleInt( ptTo[i].x, iZoomFactor),
						y + ScaleInt( ptTo[i].y, iZoomFactor) );
	}
}

/***********************************************************************/
void CWorld::DrawVerticalTunnel( HDC hDC, int x, int y )
/***********************************************************************/
{
	if ( iZoomFactor <= -3 )
	{
		DrawBlob2x2( hDC, x, y );
		return;
	}
	if ( iZoomFactor <= -2 )
	{
		MoveToEx( hDC, x, y, NULL );
		LineTo( hDC, x, y+4 );
		MoveToEx( hDC, x+2, y, NULL );
		LineTo( hDC, x+2, y+4 );
		return;
	}
						//	 x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y
	static POINT ptFr[] = {	 6, 0,10, 0 };
	static POINT ptTo[] = {	 6,14,10,14 };

	int iCount = sizeof(ptFr) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr[i].x, iZoomFactor),
						y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );
		LineTo(   hDC,	x + ScaleInt( ptTo[i].x, iZoomFactor),
						y + ScaleInt( ptTo[i].y, iZoomFactor) );
	}
}

/***********************************************************************/
void CWorld::DrawCulDeSacs( HDC hDC, int x, int y, POINT ptCell, int yDelta, int iBitmask )
/***********************************************************************/
{
	if ( iZoomFactor <= -3 )
		return;
	if ( iZoomFactor <= -2 )
		return;

	int yPlus = (yDelta != 0);
	char c;
	if ( (iBitmask & 1) && (ptCell.y-1 >= 0) )
	{
		// Check 12 o'clock
		c = MAPCODE(ptCell.x,ptCell.y-1);
		if ( c == 'H' || c == 'X' )
			Draw12CulDeSac( hDC, x, y );
	}
	if ( (iBitmask & 2) && (ptCell.x+1 < xCells && ptCell.y+yPlus-1 > 0) )
	{
		// Check 2 o'clock
		c = MAPCODE(ptCell.x+1,ptCell.y+yPlus-1);
		if ( c == 'H' || c == 'X' )
			Draw2CulDeSac( hDC, x, y );
	}
	if ( (iBitmask & 4) && (ptCell.x+1 < xCells && ptCell.y+yPlus < yCells) )
	{
		// Check 4 o'clock
		c = MAPCODE(ptCell.x+1,ptCell.y+yPlus);
		if ( c == 'H' || c == 'X' )
			Draw4CulDeSac( hDC, x, y );
	}
	if ( (iBitmask & 8) && (ptCell.y+1 < yCells) )
	{
		// Check 6 o'clock
		c = MAPCODE(ptCell.x,ptCell.y+1);
		if ( c == 'H' || c == 'X' )
			Draw6CulDeSac( hDC, x, y );
	}
	if ( (iBitmask & 16) && (ptCell.x-1 > 0 && ptCell.y+yPlus < yCells) )
	{
		// Check 8 o'clock
		c = MAPCODE(ptCell.x-1,ptCell.y+yPlus);
		if ( c == 'H' || c == 'X' )
			Draw8CulDeSac( hDC, x, y );
	}
	if ( (iBitmask & 32) && (ptCell.x-1 > 0 && ptCell.y+yPlus-1 > 0) )
	{
		// Check 10 o'clock
		c = MAPCODE(ptCell.x-1,ptCell.y+yPlus-1);
		if ( c == 'H' || c == 'X' )
			Draw10CulDeSac( hDC, x, y );
	}
}

/***********************************************************************/
void CWorld::Draw2CulDeSac( HDC hDC, int x, int y )
/***********************************************************************/
{
						//	 x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y
	static POINT ptFr[] = {	13, 2,10, 4,12, 7 };
	static POINT ptTo[] = {	10, 4,12, 7,15, 5 };

	int iCount = sizeof(ptFr) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr[i].x, iZoomFactor),
						y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );
		LineTo(   hDC,	x + ScaleInt( ptTo[i].x, iZoomFactor),
						y + ScaleInt( ptTo[i].y, iZoomFactor) );
	}
}

/***********************************************************************/
void CWorld::Draw4CulDeSac( HDC hDC, int x, int y )
/***********************************************************************/
{
						//	 x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y
	static POINT ptFr[] = {	15, 9,12, 7,10,10 };
	static POINT ptTo[] = {	12, 7,10,10,13,12 };

	int iCount = sizeof(ptFr) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr[i].x, iZoomFactor),
						y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );
		LineTo(   hDC,	x + ScaleInt( ptTo[i].x, iZoomFactor),
						y + ScaleInt( ptTo[i].y, iZoomFactor) );
	}
}

/***********************************************************************/
void CWorld::Draw6CulDeSac( HDC hDC, int x, int y )
/***********************************************************************/
{
						//	 x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y
	static POINT ptFr[] = {	10,14,10,10, 6,10 };
	static POINT ptTo[] = {	10,10, 6,10, 6,14 };

	int iCount = sizeof(ptFr) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr[i].x, iZoomFactor),
						y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );
		LineTo(   hDC,	x + ScaleInt( ptTo[i].x, iZoomFactor),
						y + ScaleInt( ptTo[i].y, iZoomFactor) );
	}
}

/***********************************************************************/
void CWorld::Draw8CulDeSac( HDC hDC, int x, int y )
/***********************************************************************/
{
						//	 x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y
	static POINT ptFr[] = {	 3,12, 6,10, 4, 7 };
	static POINT ptTo[] = {	 6,10, 4, 7, 1, 9 };

	int iCount = sizeof(ptFr) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr[i].x, iZoomFactor),
						y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );
		LineTo(   hDC,	x + ScaleInt( ptTo[i].x, iZoomFactor),
						y + ScaleInt( ptTo[i].y, iZoomFactor) );
	}
}

/***********************************************************************/
void CWorld::Draw10CulDeSac( HDC hDC, int x, int y )
/***********************************************************************/
{
						//	 x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y
	static POINT ptFr[] = {	 1, 5, 4, 7, 6, 4 };
	static POINT ptTo[] = {	 4, 7, 6, 4, 3, 2 };

	int iCount = sizeof(ptFr) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr[i].x, iZoomFactor),
						y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );
		LineTo(   hDC,	x + ScaleInt( ptTo[i].x, iZoomFactor),
						y + ScaleInt( ptTo[i].y, iZoomFactor) );
	}
}


/***********************************************************************/
void CWorld::Draw12CulDeSac( HDC hDC, int x, int y )
/***********************************************************************/
{
						//	 x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y  x  y
	static POINT ptFr[] = {	 6, 0, 6, 4,10, 4 };
	static POINT ptTo[] = {	 6, 4,10, 4,10, 0 };

	int iCount = sizeof(ptFr) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr[i].x, iZoomFactor),
						y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );
		LineTo(   hDC,	x + ScaleInt( ptTo[i].x, iZoomFactor),
						y + ScaleInt( ptTo[i].y, iZoomFactor) );
	}
}

/***********************************************************************/
void CWorld::DrawBlob2x2( HDC hDC, int x, int y )
/***********************************************************************/
{
	MoveToEx( hDC, x, y, NULL );
	LineTo( hDC, x+1, y );
	LineTo( hDC, x+1, y+1 );
	LineTo( hDC, x,   y+1 );
	LineTo( hDC, x,   y );
}

/***********************************************************************/
void CWorld::DrawBlob8x7( HDC hDC, int x, int y )
/***********************************************************************/
{
	MoveToEx( hDC, x, y, NULL );
	LineTo( hDC, x+3, y );
	MoveToEx( hDC, x, y+3, NULL );
	LineTo( hDC, x+3, y+3 );
	MoveToEx( hDC, x-1, y+1, NULL );
	LineTo( hDC, x-1, y+3 );
	MoveToEx( hDC, x+3, y+1, NULL );
	LineTo( hDC, x+3, y+3 );
}
