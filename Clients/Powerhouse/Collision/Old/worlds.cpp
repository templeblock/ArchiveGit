// MOVE all of the opponent shit to OPPONENTS.CPP

#include <windows.h>
#include <math.h>
#include "proto.h"
#include "control.h"
#include "worlds.h"		
#include "cllsnid.h"
										 
// The map data is organized as follows.
//
//     -  U 0  -  H 0  -  
//   /   `   /   `   /   `
// -  H 1  -  U 0  -  H 0 
//   `   /   `   /   `   /
//     -  U 1  -  H    -  
//   /   `   /   `   /   `
// -  H 1  -  U 1  -  H 0 
//   `   /   `   /   `   /
//     -  U 0  -  H 0  -  
//   /   `   /   `   /   `
// -       -       -      
//
// If a cell is to be part of the map, enter its basic cell type to
// the left of center.  The cell "look" is to the right of center.
// The other codes used in the sample above, are placed around
// the center to give the map structure, but are not read
// or interpreted as anthing.
//
// The basic cell type codes are as follows:
//		H for a hex
//		U for a union jack
//		C for a cul-de-sac
//		| for a vertical tunnel
//		/ for a 2to8 tunnel
//		` for a 10to4 tunnel
//		B for a black hole
//		R for a radioactive hex
// The code for the "looks" vary based on the disk
//		0 for vanilla
//		1 for fire
//		2 for ice
// The 6 codes around the perimeter are for portal and cul-de-sac codes:
// The portal codes are as follows (for use BETWEEN 2 cells):
//		B or b for barricade (drawn in blue)
//		E or e for energy (drawn in yellow)
// The cul-de-sac codes are as follows (for use on the PERIMETER of the map)
//		M or m for an empty cul-de-sac
//		A or a for a cul-de-sac with machine gun ammo (drawn in magenta)
//		R or r for a cul-de-sac with rockets (drawn in magenta)
//		S or s for a cul-de-sac with spine killers (drawn in magenta)
//		1-9 for a cul-de-sac with a collectible (level specific) (drawn in red)
//		N for the entrance to a map gatekeeper cul-de-sac
//		X for the exit from a map gatekeeper cul-de-sac
// The 6 spaces above and below the center code (cell type) are used
// for planting opponents in the cells at specific locations
// The opponent codes are as follows:
//		0-9 indicates an opponent at that location in the cell (level specific)


// The following macros used used to help extract the desired
// info from the map, given the x,y cell location

#define CELLTYPE(x,y)    GetCellUnitCode(x,y,-1, 0)
#define LOOKTYPE(x,y)    GetCellUnitCode(x,y, 1, 0)

#define PORTTYPE_02(x,y) GetCellUnitCode(x,y, 2,-1)
#define PORTTYPE_04(x,y) GetCellUnitCode(x,y, 2, 1)
#define PORTTYPE_06(x,y) GetCellUnitCode(x,y, 0, 2)
#define PORTTYPE_08(x,y) GetCellUnitCode(x,y,-2, 1)
#define PORTTYPE_10(x,y) GetCellUnitCode(x,y,-2,-1)
#define PORTTYPE_12(x,y) GetCellUnitCode(x,y, 0,-2)

#define OPPOTYPE_02(x,y) GetCellUnitCode(x,y, 1,-1)
#define OPPOTYPE_04(x,y) GetCellUnitCode(x,y, 1, 1)
#define OPPOTYPE_06(x,y) GetCellUnitCode(x,y, 0, 1)
#define OPPOTYPE_08(x,y) GetCellUnitCode(x,y,-1, 1)
#define OPPOTYPE_10(x,y) GetCellUnitCode(x,y,-1,-1)
#define OPPOTYPE_12(x,y) GetCellUnitCode(x,y, 0,-1)

// The CONSTRUCTOR
//************************************************************************
CWorld::CWorld()
//************************************************************************
{
	lpOpponent = NULL;
	hWndDraw = NULL;
	lpMap = NULL;
	xCellUnits = yCellUnits = 0;
	ptPlayerLoc.x = 0;
	ptPlayerLoc.y = 0;
	iPlayerDir = 12;
	ptViewLocation.x = 0;
	ptViewLocation.y = 0;
	ptViewLast = ptViewLocation;
	iZoomFactor = 2;
	SetZoomFactor( iZoomFactor );
	hPenG	= CreatePen( PS_SOLID, 1, RGB(  0,255,  0) );
	hPenR	= CreatePen( PS_SOLID, 1, RGB(255,  0,  0) );
	hPenY	= CreatePen( PS_SOLID, 1, RGB(255,255,  0) );
	hPenW	= CreatePen( PS_SOLID, 1, RGB(255,255,255) );
	hPenM	= CreatePen( PS_SOLID, 1, RGB(255,  0,255) );
	hPenC	= CreatePen( PS_SOLID, 1, RGB(  0,255,255) );
	hPenB	= CreatePen( PS_SOLID, 1, RGB(  0,  0,255) );
	hBrushR	= CreateSolidBrush( RGB(255,  0,  0) );
	hBrushY	= CreateSolidBrush( RGB(255,255,  0) );
	hBrushB	= CreateSolidBrush( RGB(  0,  0,255) );
	hBrushM	= CreateSolidBrush( RGB(255,  0,255) );
}

// The DESTRUCTOR
//************************************************************************
CWorld::~CWorld()
//************************************************************************
{
	if ( hPenG )
		DeletePen( hPenG );
	if ( hPenR )
		DeletePen( hPenR );
	if ( hPenY )
		DeletePen( hPenY );
	if ( hPenW )
		DeletePen( hPenW );
	if ( hPenM )
		DeletePen( hPenM );
	if ( hPenC )
		DeletePen( hPenC );
	if ( hPenB )
		DeletePen( hPenB );
	if ( hBrushR )
		DeleteBrush( hBrushR );
	if ( hBrushY )
		DeleteBrush( hBrushY );
	if ( hBrushB )
		DeleteBrush( hBrushB );
	if ( hBrushM )
		DeleteBrush( hBrushM );

	// Delete any existing map and opponent list
	if ( lpMap )
		FreeUp( lpMap );
	DeleteOpponents();
}

//*********************************************************************************************
void CWorld::MapCellUnitToCell( LPINT lpX, LPINT lpY )
//*********************************************************************************************
{ // lose the fractional resolution
	if ( !lpX || !lpY )
		return;

	*lpX = (*lpX + 2) / 4;
	if ( *lpX & 1 )
		*lpY -= 2;
	*lpY = (*lpY + 2) / 4;
}

//*********************************************************************************************
void CWorld::MapCellToCellUnit( LPINT lpX, LPINT lpY )
//*********************************************************************************************
{ // map to the center cell unit
	if ( !lpX || !lpY )
		return;

	*lpY = (*lpY * 4);
	if ( *lpX & 1 )
		*lpY += 2;
	*lpX = (*lpX * 4);
}

//*********************************************************************************************
void CWorld::MapCellUnitToCellCenter( LPINT lpX, LPINT lpY )
//*********************************************************************************************
{
	MapCellUnitToCell( lpX, lpY );
	MapCellToCellUnit( lpX, lpY );
}

//*********************************************************************************************
void CWorld::MapCellUnitToView( LPINT lpX, LPINT lpY )
//*********************************************************************************************
{		
	if ( !lpX || !lpY )
		return;

	*lpX = ((*lpX * xDispDelta) + 2 ) / 4;
	*lpY = ((*lpY * iDispHeight) + 2 ) / 4;
}

//*********************************************************************************************
void CWorld::MapViewToCellUnit( LPINT lpX, LPINT lpY )
//*********************************************************************************************
{		
	if ( !lpX || !lpY )
		return;

	*lpX = ((*lpX * 4) + (xDispDelta/2) ) / xDispDelta;
	*lpY = ((*lpY * 4) + (iDispHeight/2) ) / iDispHeight;
}

//*********************************************************************************************
void CWorld::MapViewToCell( LPINT lpX, LPINT lpY )
//*********************************************************************************************
{		
	if ( !lpX || !lpY )
		return;

	*lpX = (*lpX + (xDispDelta/2) ) / xDispDelta;
	if ( *lpX & 1 )
		*lpY -= (iDispHeight/2);
	*lpY = (*lpY + (iDispHeight/2) ) / iDispHeight;
}

//*********************************************************************************************
void CWorld::MapCellUnitToRadar( LPINT lpX, LPINT lpY )
//*********************************************************************************************
{		
	if ( !lpX || !lpY )
		return;

	MapCellUnitToView( lpX, lpY );
	MapViewToRadar( lpX, lpY );
}

//*********************************************************************************************
void CWorld::MapViewToRadar( LPINT lpX, LPINT lpY )
//*********************************************************************************************
{		
	if ( !lpX || !lpY )
		return;

	RECT rDraw;
	GetClientRect( hWndDraw, &rDraw );

	*lpX -= (ptViewLocation.x - RectWidth(&rDraw)/2);
	*lpY -= (ptViewLocation.y - RectHeight(&rDraw)/2);	
}

//*********************************************************************************************
void CWorld::MapRadarToView( LPINT lpX, LPINT lpY )
//*********************************************************************************************
{		
	if ( !lpX || !lpY )
		return;

	RECT rDraw;
	GetClientRect( hWndDraw, &rDraw );

	*lpX += (ptViewLocation.x - RectWidth(&rDraw)/2);
	*lpY += (ptViewLocation.y - RectHeight(&rDraw)/2);	
}

//*********************************************************************************************
char CWorld::GetCellUnitCode( int x, int y, int xOffset, int yOffset )
//*********************************************************************************************
{		
	MapCellUnitToCellCenter( &x, &y );

	x += xOffset;
	y += yOffset;

	if ( x >= xCellUnits )	return( NULL );
	if ( y >= yCellUnits )	return( NULL );
	if ( x < 0 )			return( NULL );
	if ( y < 0 )			return( NULL );

	char c = *(lpMap + ((x) + ((y) * xCellUnits)));
	if ( c == ' ' ) c = NULL;
	return( c );
}

//*********************************************************************************************
void CWorld::ZapCellUnitCode( int x, int y, int xOffset, int yOffset, char c )
//*********************************************************************************************
{		
	MapCellUnitToCellCenter( &x, &y );

	x += xOffset;
	y += yOffset;

	if ( x >= xCellUnits )	return;
	if ( y >= yCellUnits )	return;
	if ( x < 0 )			return;
	if ( y < 0 )			return;

	LPSTR lpZapMap = lpMap + ((x) + ((y) * xCellUnits));
	*lpZapMap = c;
}

//*********************************************************************************************
BOOL CWorld::IsValidCulDeSac( char c )
//*********************************************************************************************
{
	return( c == 'M' || c == 'm' || c == 'N' || c == 'n' || c == 'X' || c == 'x' ||
		    c == 'R' || c == 'r' || c == 'A' || c == 'a' || c == 'S' || c == 's' ||
		   (c >= '1' && c <= '9') );
}

/***********************************************************************/
char CWorld::GetCulDeSacType( LPCELLINFO lpCellInfo, int iDir )
/***********************************************************************/
{
	if ( !lpCellInfo )
		return( NULL );

	char cType = NULL;
	switch ( iDir )
	{
		case 2:
			cType = lpCellInfo->szPortType_08;
			break;
		case 4:
			cType = lpCellInfo->szPortType_10;
			break;
		case 6:
			cType = lpCellInfo->szPortType_12;
			break;
		case 8:
			cType = lpCellInfo->szPortType_02;
			break;
		case 10:
			cType = lpCellInfo->szPortType_04;
			break;
		case 12:
			cType = lpCellInfo->szPortType_06;
			break;
	}

	if ( !IsValidCulDeSac( cType ) )
		cType = 'M'; // empty

	return( cType );
}

//*********************************************************************************************
BOOL CWorld::CellUnitSetIfCulDeSac( int x, int y )
//*********************************************************************************************
{		
	int dx = x;
	int dy = y;
	MapCellUnitToCellCenter( &x, &y );
	dx -= x;
	dy -= y;
	if ( dx || dy )
		return( NO ); // only process cell centers

	if ( GetCellUnitCode( x, y, -1, 0) )
		return( NO );

	if ( IsValidCulDeSac( PORTTYPE_12(x, y) ) ||
		 IsValidCulDeSac( PORTTYPE_02(x, y) ) ||
		 IsValidCulDeSac( PORTTYPE_04(x, y) ) ||
		 IsValidCulDeSac( PORTTYPE_06(x, y) ) ||
		 IsValidCulDeSac( PORTTYPE_08(x, y) ) ||
		 IsValidCulDeSac( PORTTYPE_10(x, y) ) )
	{
		ZapCellUnitCode( x, y, -1, 0, 'C' );
		return( YES );
	}

	return( NO );
}

//*********************************************************************************************
BOOL CWorld::CellUnitIsOpponent( int x, int y, LPINT lpiType, LPINT lpiDirection )
//*********************************************************************************************
{		
	int dx = x;
	int dy = y;
	MapCellUnitToCellCenter( &x, &y );
	dx -= x;
	dy -= y;

	char c;

	// Check the location at 2 o'clock
	if ( dx == 1 && dy == -1 )
	{
		if ( !(c = OPPOTYPE_02(x,y)) )
			return( NO );
		*lpiType = c - '0';
		*lpiDirection = 8;
		return( YES );
	}

	// Check the location at 4 o'clock
	if ( dx == 1 && dy == 1 )
	{
		if ( !(c = OPPOTYPE_04(x,y)) )
			return( NO );
		*lpiType = c - '0';
		*lpiDirection = 10;
		return( YES );
	}

	// Check the location at 6 o'clock
	if ( dx == 0 && dy == 1 )
	{
		if ( !(c = OPPOTYPE_06(x,y)) )
			return( NO );
		*lpiType = c - '0';
		*lpiDirection = 12;
		return( YES );
	}

	// Check the location at 8 o'clock
	if ( dx == -1 && dy == 1 )
	{
		if ( !(c = OPPOTYPE_08(x,y)) )
			return( NO );
		*lpiType = c - '0';
		*lpiDirection = 2;
		return( YES );
	}

	// Check the location at 10 o'clock
	if ( dx == -1 && dy == -1 )
	{
		if ( !(c = OPPOTYPE_10(x,y)) )
			return( NO );
		*lpiType = c - '0';
		*lpiDirection = 4;
		return( YES );
	}

	// Check the location at 12 o'clock
	if ( dx == 0 && dy == -1 )
	{
		if ( !(c = OPPOTYPE_12(x,y)) )
			return( NO );
		*lpiType = c - '0';
		*lpiDirection = 6;
		return( YES );
	}

	return( NO );
}

//***********************************************************************
BOOL CWorld::Init( HWND hWnd )
//***********************************************************************
{
	if ( hWnd ) 
		hWndDraw = hWnd;
	
	return( LoadLevel(3) );
}

//***********************************************************************
BOOL CWorld::LoadLevel( int iLevel )
//***********************************************************************
{
	HINSTANCE hInstance = GetWindowInstance( hWndDraw ); // GetApp()->GetInstance();
	if ( !hInstance )
		return( NO );

	// Load and lock the map resource data
	HGLOBAL hResource;
	if ( !(hResource = (HGLOBAL)FindResource( hInstance, MAKEINTRESOURCE(iLevel), "MAPLEVEL")) )
		return( NO );
	long lLength = SizeofResource( hInstance, (HRSRC)hResource );
	if ( !(hResource = LoadResource( hInstance, (HRSRC)hResource )) )
		return( NO );
	LPWORD lpMapData;
	if ( !(lpMapData = (LPWORD)LockResource( hResource )) )
	{
		FreeResource( hResource );
		return( NO );
	}

	// Delete any existing map and opponent list
	if ( lpMap )
		FreeUp( lpMap );
	lpMap = NULL;
	DeleteOpponents();

	// Process the map data
	xCellUnits = *lpMapData++; // Cell units have 2 bits of fractional data
	yCellUnits = *lpMapData++;
	POINT pt;
	pt.x = *lpMapData++;
	pt.y = *lpMapData++;
	SetPlayerLoc( &pt, NO/*bScroll*/ );
	if ( lpMap = (LPSTR)Alloc( lLength ) )
		hmemcpy( lpMap, lpMapData, lLength );
	UnlockResource( hResource );
	FreeResource( hResource );	

	// Find all of the opponents in the cell map and add them to the opponent list
	LPOPPONENT lpPrevOpponent = NULL;
	for ( int y=0; y<yCellUnits; y++ )
	{
		for ( int x=0; x<xCellUnits; x++ )
		{
			CellUnitSetIfCulDeSac( x, y );

			int iType, iDir;
			if ( !CellUnitIsOpponent( x, y, &iType, &iDir ) )
				continue;
				
			LPOPPONENT lpNextOpponent = new COpponent;
			if ( !lpNextOpponent )
				continue;

			POINT ptCellUnit;
			ptCellUnit.x = x;
			ptCellUnit.y = y;
			lpNextOpponent->Init( iDir, ptCellUnit, iType );

			if ( !lpPrevOpponent ) // if no head pointer...
				lpOpponent = lpNextOpponent;
			else
			{  
				lpPrevOpponent->lpNextOpp = lpNextOpponent; 
				lpNextOpponent->lpPrevOpp = lpPrevOpponent; 
			} 
			lpPrevOpponent = lpNextOpponent;
		}
	}

	iCurrentLevel = iLevel;
	return( YES );
}

//*********************************************************************************************
int CWorld::GetLevel( void )
//*********************************************************************************************
{
	return(iCurrentLevel);
}

//***********************************************************************
void CWorld::NewCellLocation( LPCELLINFO lpCellInfo )
//***********************************************************************
{
	// Force player to get to final loc. before changing location
	GetPlayerLoc( &ptPlayerLoc );

	// fix the direction if it's odd (otherwise you would hit a wall)
	// this shouldn't be necessary
	if ( iPlayerDir & 1 )
	{ // back it up
		if ( !(--iPlayerDir) )
			iPlayerDir = 12;
	}

	char cType = MoveNewDirection( &ptPlayerLoc, iPlayerDir );
	SetPlayerLoc( &ptPlayerLoc, YES/*bScroll*/ );

	if ( lpCellInfo )
		GetCellData( lpCellInfo );
}

//***********************************************************************
void CWorld::SetPlayerLoc( LPPOINT lpPoint, BOOL bScroll )
//***********************************************************************
{
	if ( !lpPoint )
		return;

	int x = lpPoint->x;
	int y = lpPoint->y;
	if ( x >= xCellUnits ) x = xCellUnits - 1;
	if ( y >= yCellUnits ) y = yCellUnits - 1;
	if ( x < 0 ) x = 0;
	if ( y < 0 ) y = 0;
	ptPlayerLoc.x = x;
	ptPlayerLoc.y = y;
	
	// Center the view on this cell
	MapCellUnitToView( &x, &y );
	POINT pt;
	pt.x = x;
	pt.y = y;
	SetViewLocation( &pt, bScroll );
}

//***********************************************************************
void CWorld::GetPlayerLoc( LPPOINT lpPoint )
//***********************************************************************
{
	if ( !lpPoint )
		return;
	*lpPoint = ptPlayerLoc;
	// Make sure the player's location is his final resting place
	MoveSameDirection( lpPoint, iPlayerDir );
	// call twice to get all the way there
	MoveSameDirection( lpPoint, iPlayerDir );
}

//***********************************************************************
void CWorld::SetViewLocation( LPPOINT lpPoint, BOOL bScroll )
//***********************************************************************
{
	if ( bScroll ) // If scrolling to the new point, remember the old point
		ptViewLast = ptViewLocation;

	ptViewLocation = *lpPoint;

	if ( !bScroll ) // If not scrolling, set the old point to the new point
		ptViewLast = ptViewLocation;

	UpdateView();
}

//***********************************************************************
void CWorld::GetViewLocation( LPPOINT lpPoint )
//***********************************************************************
{
	if ( !lpPoint )
		return;
	*lpPoint = ptViewLocation;
}

//***********************************************************************
void CWorld::SetZoomFactor( int iNewZoomFactor )
//***********************************************************************
{
	if ( !iNewZoomFactor )
		return;

	iZoomFactor = iNewZoomFactor;

	#define MIN_DELTAX	12 // width - 25%
	#define MIN_WIDTH	16
	#define MIN_HEIGHT	14

	// Compute how big a single cell is on the display
	if ( iZoomFactor <= -3 )
	{
		xDispDelta = 2;
		iDispWidth = 2;
		iDispHeight = 2;
	}
	else
	if ( iZoomFactor <= -2 )
	{
		xDispDelta = 6;
		iDispWidth = 8;
		iDispHeight = 7;
	}
	else
	{
		xDispDelta = ScaleInt( MIN_DELTAX, iZoomFactor );
		iDispWidth = ScaleInt( MIN_WIDTH, iZoomFactor );
		iDispHeight = ScaleInt( MIN_HEIGHT, iZoomFactor );
	}

	SetPlayerLoc( &ptPlayerLoc, NO/*bScroll*/ );
}

//***********************************************************************
int CWorld::GetZoomFactor( void )
//***********************************************************************
{
	return( iZoomFactor );
}

//***********************************************************************
void CWorld::SetPlayerDir( int iNewDirection )
//***********************************************************************
{
	// Force player to get to final loc. before changing direction
	GetPlayerLoc( &ptPlayerLoc );

	if ( iNewDirection >= 1 && iNewDirection <= 12 )
		iPlayerDir = iNewDirection;
}

//***********************************************************************
int CWorld::GetPlayerDir( void )
//***********************************************************************
{
	return( iPlayerDir );
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

/***********************************************************************/
void CWorld::Draw( HDC hDC, LPRECT lpClipRect )
/***********************************************************************/
{
	if ( !hDC )
		return;

	HWND hWindow = hWndDraw;
	
	// Get the size of the drawing rectangle
	RECT rDraw;
	if ( lpClipRect )
		rDraw = *lpClipRect;
	else
		GetClientRect( hWindow, &rDraw );

	int x1 = rDraw.left;
	int y1 = rDraw.top;
	MapRadarToView( &x1, &y1 );
	MapViewToCell( &x1, &y1 );

	int x2 = rDraw.right;
	int y2 = rDraw.bottom;
	MapRadarToView( &x2, &y2 );
	MapViewToCell( &x1, &y1 );

	// pull in the hanging cells
	if ( x1 & 1 ) y2++; else y1--;
	x1--; x2++;

	// Loop on all of the cells in the draw rectangle, and paint them
	HPEN hOldPen = SelectPen( hDC, hPenG );
	POINT ptCell;
	for ( ptCell.y = y1; ptCell.y <= y2; ptCell.y++ )
	{
		if ( ptCell.y < 0 || ptCell.y >= yCellUnits/4 )
			continue;
		
		for ( ptCell.x = x1; ptCell.x <= x2; ptCell.x++ )
		{
			if ( ptCell.x < 0 || ptCell.x >= xCellUnits/4 )
				continue;

			DrawCell( hDC, ptCell );
		}
	}
	
	SelectPen( hDC, hOldPen ); 
	DrawOpponents( hDC, YES );
	DrawPlayer( hDC, YES );	
}

/***********************************************************************/
void CWorld::DrawCell( HDC hDC, POINT ptCell )
/***********************************************************************/
{
	int x = ptCell.x;
	int y = ptCell.y;
	MapCellToCellUnit( &x, &y ); // the center cell unit
	ptCell.x = x;
	ptCell.y = y;

	char c = CELLTYPE(ptCell.x,ptCell.y);
	if ( !c )
		return;

	MapCellUnitToRadar( &x, &y );
	// offset by half a cell
	x -= (iDispWidth/2);
	y -= (iDispHeight/2);

	DrawPortals( hDC, x, y, ptCell );

	switch ( c )
	{		
		case 'H':
		{				 
			DrawHex( hDC, x, y );
			break;
		}
		case 'U':
		{						 
			DrawUnionJack( hDC, x, y);
			break;
		}
		case 'C':
		{
			DrawCulDeSacs( hDC, x, y, ptCell );
			break;
		}
		case '|':
		{
			DrawVerticalTunnel( hDC, x, y );
			break;
		}
		case '`':
		{
			Draw10to4Tunnel( hDC, x, y );
			break;
		}
		case '/':
		{		
			Draw2to8Tunnel( hDC, x, y );
			break;
		}
		case 'B':
		{								 
			DrawBlackHole( hDC, x, y );
			break;
		}
		case 'R':
		{								 
			DrawRadiationRoom( hDC, x, y );
			break;
		}
	}
}

/***********************************************************************/
void DrawPolygon( HDC hDC, LPPOINT lpPoints, int iCount, HPEN hPen, HBRUSH hBrush )
/***********************************************************************/
{				
	HPEN hOldPen;
	if ( hPen )
		hOldPen = SelectPen( hDC, hPen );
	HBRUSH hOldBrush;
	if ( hBrush )
		hOldBrush = SelectBrush( hDC, hBrush );
	Polygon( hDC, lpPoints, iCount );
	if ( hPen )
		SelectPen( hDC, hOldPen );
	if ( hBrush )
		SelectBrush( hDC, hOldBrush );
}

/***********************************************************************/
void DrawPolyline( HDC hDC, LPPOINT lpPoints, int iCount, HPEN hPen )
/***********************************************************************/
{				
	HPEN hOldPen;
	if ( hPen )
		hOldPen = SelectPen( hDC, hPen );
	Polyline( hDC, lpPoints, iCount );
	if ( hPen )
		SelectPen( hDC, hOldPen );
}

/***********************************************************************/
void CWorld::DrawPlayer( HDC hDC, BOOL bOn )
/***********************************************************************/
{
	static int iDir;
	static int x, y;

	if ( bOn )
	{
		iDir = iPlayerDir;
		x = ptPlayerLoc.x;
		y = ptPlayerLoc.y;
		MapCellUnitToRadar( &x, &y );
	}

	POINT pt[3];
	if ( iDir == 1 )
	{
		pt[0].x =  1;	pt[0].y = -2;
		pt[1].x = -2;	pt[1].y =  1;
		pt[2].x =  1;	pt[2].y =  2;
	}
	else
	if ( iDir == 2 )
	{
		pt[0].x =  2;	pt[0].y = -1;
		pt[1].x = -1;	pt[1].y =  2;
		pt[2].x = -2;	pt[2].y = -1;
	}
	else
	if ( iDir == 3 )
	{
		pt[0].x =  2;	pt[0].y =  0;
		pt[1].x = -2;	pt[1].y =  2;
		pt[2].x = -2;	pt[2].y = -2;
	}
	else
	if ( iDir == 4 )
	{
		pt[0].x =  2;	pt[0].y =  1;
		pt[1].x = -2;	pt[1].y =  1;
		pt[2].x = -1;	pt[2].y = -2;
	}
	else
	if ( iDir == 5 )
	{
		pt[0].x =  1;	pt[0].y =  2;
		pt[1].x =  1;	pt[1].y = -2;
		pt[2].x = -2;	pt[2].y = -1;
	}
	else
	if ( iDir == 6 )
	{
		pt[0].x = 0;	pt[0].y =  2;
		pt[1].x = -1;	pt[1].y = -2;
		pt[2].x =  1;	pt[2].y = -2;
	}
	else
	if ( iDir == 7 )
	{
		pt[0].x = -1;	pt[0].y =  2;
		pt[1].x =  2;	pt[1].y = -1;
		pt[2].x = -1;	pt[2].y = -2;
	}
	else
	if ( iDir == 8 )
	{
		pt[0].x = -2;	pt[0].y =  1;
		pt[1].x =  1;	pt[1].y = -2;
		pt[2].x =  2;	pt[2].y =  1;
	}
	else
	if ( iDir == 9 )
	{
		pt[0].x = -2;	pt[0].y = 0;
		pt[1].x =  2;	pt[1].y = -2;
		pt[2].x =  2;	pt[2].y =  2;
	}
	else
	if ( iDir == 10 )
	{
		pt[0].x = -2;	pt[0].y = -1;
		pt[1].x =  2;	pt[1].y = -1;
		pt[2].x =  1;	pt[2].y =  2;
	}
	else
	if ( iDir == 11 )
	{
		pt[0].x = -1;	pt[0].y = -2;
		pt[1].x = -1;	pt[1].y =  2;
		pt[2].x =  2;	pt[2].y =  1;
	}
	else
	if ( iDir == 12 )
	{
		pt[0].x = 0;	pt[0].y = -2;
		pt[1].x =  1;	pt[1].y =  2;
		pt[2].x = -1;	pt[2].y =  2;
	}                             
	else
		return;

	int iZoom = iZoomFactor;
	if ( iZoom < 1 ) iZoom = 1;
	int iCount = sizeof(pt) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		pt[i].x = x + ScaleInt( pt[i].x, iZoom );
		pt[i].y = y + ScaleInt( pt[i].y, iZoom );
	}

	HPEN hPen = (bOn ? hPenR : GetStockPen(BLACK_PEN));
	HBRUSH hBrush = (bOn ? hBrushR : GetStockBrush(BLACK_BRUSH));
	DrawPolygon( hDC, pt, iCount, hPen, hBrush );
}

/***********************************************************************/
void CWorld::DeleteOpponents( void )
/***********************************************************************/
{				
	LPOPPONENT pOpp;
	while (lpOpponent)
	{							
		pOpp = lpOpponent->lpNextOpp;
		delete lpOpponent;
		lpOpponent = pOpp;
	}
}

/***********************************************************************/
void CWorld::DrawOpponents( HDC hDC, BOOL bOn )
/***********************************************************************/
{				
	LPOPPONENT pOpp = lpOpponent;
	while (pOpp)
	{							
		DrawOpponent( hDC, bOn, pOpp );
		pOpp = pOpp->lpNextOpp;
	}
}

/***********************************************************************/
void CWorld::DrawOpponent( HDC hDC, BOOL bOn, LPOPPONENT pOpp )
/***********************************************************************/
{				
	int x, y;
	if ( !bOn )
	{
		x = pOpp->pntLastDraw.x;
		y = pOpp->pntLastDraw.y;
	}
	else
	{
		x = pOpp->ptCell.x;
		y = pOpp->ptCell.y;
		MapCellUnitToRadar( &x, &y );
		pOpp->pntLastDraw.x = x;
		pOpp->pntLastDraw.y = y;
	}

	HPEN hPen;
	HPEN hOldPen = NULL;
	switch (pOpp->iBehavior)
	{
		case 1:
		{
			hPen = (bOn ? hPenW : GetStockPen(BLACK_PEN));
			hOldPen = SelectPen( hDC, hPen );
			break;
		}
		case 2:
		{
			hPen = (bOn ? hPenM : GetStockPen(BLACK_PEN));
			hOldPen = SelectPen( hDC, hPen );
			break;			
		}
		case 3:
		{																														
			hPen = (bOn ? hPenM : GetStockPen(BLACK_PEN));
			hOldPen = SelectPen( hDC, hPen );
			break;
		}
		case 4:
		{																														
			hPen = (bOn ? hPenC : GetStockPen(BLACK_PEN));
			hOldPen = SelectPen( hDC, hPen );
			break;
		}
		case 5:
		{																														
			hPen = (bOn ? hPenB : GetStockPen(BLACK_PEN));
			hOldPen = SelectPen( hDC, hPen );
			break;
		}
		case 6:
		{																														
			hPen = (bOn ? hPenY : GetStockPen(BLACK_PEN));
			hOldPen = SelectPen( hDC, hPen );
			break;
		}
		default:
			return;
	}

	static POINT ptFr[] = { -1,-1,  -1, 1,   1, 1,   1,-1 };
	static POINT ptTo[] = { -1, 1,   1, 1,   1,-1,  -1,-1 };

	int iCount = sizeof(ptFr) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr[i].x, iZoomFactor),
						y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );		
		LineTo(	  hDC,	x + ScaleInt( ptTo[i].x, iZoomFactor),
						y + ScaleInt( ptTo[i].y, iZoomFactor) );
	}
	SelectPen( hDC, hOldPen );
}

//***********************************************************************
void CWorld::UpdateView( void )
//***********************************************************************
{
	int sdx = ptViewLocation.x - ptViewLast.x;
	int sdy = ptViewLocation.y - ptViewLast.y;

	HDC hDC = GetDC( hWndDraw );
	if ( !hDC )
		return;

	DrawOpponents( hDC, NO );
	DrawPlayer( hDC, NO );
		
	if ( sdx || sdy )
	{
		InvalidateRect( hWndDraw, NULL, TRUE );
		UpdateWindow( hWndDraw );
//		RECT rect;
//		HRGN hRegion = CreateRectRgn( 0, 0, 0, 0 );
//		BOOL b = ScrollDC( hDC, -sdx, -sdy, &rect/*lprcScroll*/,
//			&rect/*lprcCLip*/, hRegion, NULL/*lprcUpdate*/ );
//		InvalidateRgn( hWindow, hRegion, TRUE );
//		UpdateWindow( hWindow );
//		DeleteRgn( hRegion );			
	}

	DrawOpponents( hDC, YES );
	DrawPlayer( hDC, YES );
		
	ReleaseDC( hWndDraw, hDC );
	ptViewLast = ptViewLocation;
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

	static POINT ptFr[] = { 15, 9,16, 7,13, 2,12, 0, 6, 0, 4, 0, 1, 5, 0, 7, 3,12, 4,14,10,14,12,14 };
	static POINT ptTo[] = { 16, 7,15, 5,12, 0,10, 0, 4, 0, 3, 2, 0, 7, 1, 9, 4,14, 6,14,12,14,13,12 };

	int iCount = sizeof(ptFr) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr[i].x, iZoomFactor),
						y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );
		LineTo(	  hDC,	x + ScaleInt( ptTo[i].x, iZoomFactor),
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

	static POINT ptFr[] = { 15, 9,10, 7,13, 2, 9, 5, 6, 0, 7, 5, 1, 5, 6, 7, 3,12, 7, 9,10,14, 9, 9 };
	static POINT ptTo[] = { 10, 7,15, 5, 9, 5,10, 0, 7, 5, 3, 2, 6, 7, 1, 9, 7, 9, 6,14, 9, 9,13,12 };

	int iCount = sizeof(ptFr) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr[i].x, iZoomFactor),
						y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );
		LineTo(	  hDC,	x + ScaleInt( ptTo[i].x, iZoomFactor),
						y + ScaleInt( ptTo[i].y, iZoomFactor) );
	}
}

/***********************************************************************/
void CWorld::DrawBlackHole( HDC hDC, int x, int y )
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

	static POINT ptFr[] = { 15, 9,16, 7,13, 2,12, 0, 6, 0, 4, 0, 1, 5, 0, 7, 3,12, 4,14,10,14,12,14 };
	static POINT ptTo[] = { 16, 7,15, 5,12, 0,10, 0, 4, 0, 3, 2, 0, 7, 1, 9, 4,14, 6,14,12,14,13,12 };

	int iCount = sizeof(ptFr) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr[i].x, iZoomFactor),
						y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );
		LineTo(	  hDC,	x + ScaleInt( ptTo[i].x, iZoomFactor),
						y + ScaleInt( ptTo[i].y, iZoomFactor) );
	}
	
	static POINT ptFr1[] = { 5, 1, 2, 7, 5,13,11,13,14, 7,11, 1};
	static POINT ptTo1[] = { 2, 7, 5,13,11,13,14, 7,11, 1, 5, 1};

	iCount = sizeof(ptFr1) / sizeof(POINT);
	for (i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr1[i].x, iZoomFactor),
						y + ScaleInt( ptFr1[i].y, iZoomFactor), NULL );
		LineTo(	  hDC,	x + ScaleInt( ptTo1[i].x, iZoomFactor),
						y + ScaleInt( ptTo1[i].y, iZoomFactor) );
	}
	
	static POINT ptFr2[] = { 6, 3, 4, 7, 6,11,10,11,12, 7,10, 3};
	static POINT ptTo2[] = { 4, 7, 6,11,10,11,12, 7,10, 3, 6, 3};

	iCount = sizeof(ptFr2) / sizeof(POINT);
	for (i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr2[i].x, iZoomFactor),
						y + ScaleInt( ptFr2[i].y, iZoomFactor), NULL );
		LineTo(	  hDC,	x + ScaleInt( ptTo2[i].x, iZoomFactor),
						y + ScaleInt( ptTo2[i].y, iZoomFactor) );
	}
	
	static POINT ptFr3[] = { 6, 7, 7, 9, 9, 9,10, 7, 9, 5, 7, 5};
	static POINT ptTo3[] = { 7, 9, 9, 9,10, 7, 9, 5, 7, 5, 6, 7};

	iCount = sizeof(ptFr3) / sizeof(POINT);
	for (i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr3[i].x, iZoomFactor),
						y + ScaleInt( ptFr3[i].y, iZoomFactor), NULL );
		LineTo(	  hDC,	x + ScaleInt( ptTo3[i].x, iZoomFactor),
						y + ScaleInt( ptTo3[i].y, iZoomFactor) );
	}
}

/***********************************************************************/
void CWorld::DrawRadiationRoom( HDC hDC, int x, int y )
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

	static POINT ptFr[] = { 15, 9,16, 7,13, 2,12, 0, 6, 0, 4, 0, 1, 5, 0, 7, 3,12, 4,14,10,14,12,14 };
	static POINT ptTo[] = { 16, 7,15, 5,12, 0,10, 0, 4, 0, 3, 2, 0, 7, 1, 9, 4,14, 6,14,12,14,13,12 };

	int iCount = sizeof(ptFr) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr[i].x, iZoomFactor),
						y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );
		LineTo(		hDC,	x + ScaleInt( ptTo[i].x, iZoomFactor),
						y + ScaleInt( ptTo[i].y, iZoomFactor) );
	}
	
	static POINT ptFr1[] = { 4, 0,12, 0, 0, 7};
	static POINT ptTo1[] = {13,14, 4,14,16, 7};

	iCount = sizeof(ptFr1) / sizeof(POINT);
	for (i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr1[i].x, iZoomFactor),
						y + ScaleInt( ptFr1[i].y, iZoomFactor), NULL );
		LineTo(	  hDC,	x + ScaleInt( ptTo1[i].x, iZoomFactor),
						y + ScaleInt( ptTo1[i].y, iZoomFactor) );
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

	static POINT ptFr[] = {	3,2, 13,12 };
	static POINT ptTo[] = { 15,9, 1,5 };

	int iCount = sizeof(ptFr) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr[i].x, iZoomFactor),
						y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );
		LineTo(	  hDC,	x + ScaleInt( ptTo[i].x, iZoomFactor),
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

	static POINT ptFr[] = { 13,2,  3,12 };
	static POINT ptTo[] = {	1,9,  15,5 };

	int iCount = sizeof(ptFr) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr[i].x, iZoomFactor),
						y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );
		LineTo(	  hDC,	x + ScaleInt( ptTo[i].x, iZoomFactor),
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

	static POINT ptFr[] = {	6,0,  10,0 };
	static POINT ptTo[] = {	6,14, 10,14 };

	int iCount = sizeof(ptFr) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr[i].x, iZoomFactor),
						y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );
		LineTo(	  hDC,	x + ScaleInt( ptTo[i].x, iZoomFactor),
						y + ScaleInt( ptTo[i].y, iZoomFactor) );
	}
}

/***********************************************************************/
void CWorld::DrawPortals( HDC hDC, int x, int y, POINT ptCell )
/***********************************************************************/
{
	static POINT ptList2[]  = { 15,5,  14,5,  13,3,  13,2, 14,2,  15,4 };
	static POINT ptList4[]  = { 13,12, 13,11, 14,9,  15,9, 15,10, 14,12 };
	static POINT ptList6[]  = { 6,14,   7,13,  9,13, 10,14, 9,15,  7,15 };
	static POINT ptList8[]  = {  1,9,   2,9,   3,11,  3,12, 2,12,  1,10 };
	static POINT ptList10[] = {  3,2,   3,3,   2,5,   1,5,  1,4,   2,2 };
	static POINT ptList12[] = {  6,0,   7,-1,  9,-1, 10,0,  9,1,   7,1 };

	if ( iZoomFactor <= -3 )
		return;
	if ( iZoomFactor <= -2 )
		return;

	char c;

	BOOL bOdd = (ptCell.x & 4);
	if ( bOdd )
	{ // draw on ODD cells
		// Check 2 o'clock
		c = PORTTYPE_02(ptCell.x, ptCell.y);
		if ( c == 'E' || c == 'e' )
			DrawEnergy( hDC, x, y, ptList2 );
		if ( c == 'B' || c == 'b' )
			DrawBarricade( hDC, x, y, ptList2 );
		// Check 8 o'clock
		c = PORTTYPE_08(ptCell.x, ptCell.y);
		if ( c == 'E' || c == 'e' )
			DrawEnergy( hDC, x, y, ptList8 );
		if ( c == 'B' || c == 'b' )
			DrawBarricade( hDC, x, y, ptList8 );
	}
	else
	{ // draw on EVEN cells
		// Check 4 o'clock
		c = PORTTYPE_04(ptCell.x, ptCell.y);
		if ( c == 'E' || c == 'e' )
			DrawEnergy( hDC, x, y, ptList4 );
		if ( c == 'B' || c == 'b' )
			DrawBarricade( hDC, x, y, ptList4 );
		// Check 10 o'clock
		c = PORTTYPE_10(ptCell.x, ptCell.y);
		if ( c == 'E' || c == 'e' )
			DrawEnergy( hDC, x, y, ptList10 );
		if ( c == 'B' || c == 'b' )
			DrawBarricade( hDC, x, y, ptList10 );
	}

	bOdd = (ptCell.y & 4);
	if ( bOdd )
	{ // draw on ODD cells
		// Check 12 o'clock
		c = PORTTYPE_12(ptCell.x, ptCell.y);
		if ( c == 'E' || c == 'e' )
			DrawEnergy( hDC, x, y, ptList12 );
		if ( c == 'B' || c == 'b' )
			DrawBarricade( hDC, x, y, ptList12 );
		// Check 6 o'clock
		c = PORTTYPE_06(ptCell.x, ptCell.y);
		if ( c == 'E' || c == 'e' )
			DrawEnergy( hDC, x, y, ptList6 );
		if ( c == 'B' || c == 'b' )
			DrawBarricade( hDC, x, y, ptList6 );
	}
}

/***********************************************************************/
void CWorld::DrawBarricade( HDC hDC, int x, int y, LPPOINT ptList )
/***********************************************************************/
{
	POINT pt[6];

	int iZoom = iZoomFactor;
	int iCount = sizeof(pt) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		pt[i].x = x + ScaleInt( ptList[i].x, iZoom );
		pt[i].y = y + ScaleInt( ptList[i].y, iZoom );
	}

	DrawPolygon( hDC, pt, iCount, hPenB, hBrushB );
}

/***********************************************************************/
void CWorld::DrawEnergy( HDC hDC, int x, int y, LPPOINT ptList )
/***********************************************************************/
{
	POINT pt[6];

	int iZoom = iZoomFactor;
	int iCount = sizeof(pt) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		pt[i].x = x + ScaleInt( ptList[i].x, iZoom );
		pt[i].y = y + ScaleInt( ptList[i].y, iZoom );
	}

	DrawPolygon( hDC, pt, iCount, hPenY, hBrushY );
}

/***********************************************************************/
void CWorld::DrawCulDeSacs( HDC hDC, int x, int y, POINT ptCell )
/***********************************************************************/
{
	static POINT ptList2[]  = { 13,2,   10,4,   12,7,   15,5  };
	static POINT ptList4[]  = { 15,9,   12,7,   10,10,  13,12 };
	static POINT ptList6[]  = { 10,14,  10,10,   6,10,   6,14 };
	static POINT ptList8[]  = {  3,12,   6,10,   4,7,    1,9  };
	static POINT ptList10[] = {  1,5,    4,7,    6,4,    3,2  };
	static POINT ptList12[] = {  6,0,    6,4,   10,4,   10,0  };

	if ( iZoomFactor <= -3 )
		return;
	if ( iZoomFactor <= -2 )
		return;

	char c;
	// Check 12 o'clock
	c = PORTTYPE_12(ptCell.x, ptCell.y);
	if ( c == 'M' || c == 'm' )
		DrawEmptyCulDeSac( hDC, x, y, ptList12 );
	if ( c == 'N' || c == 'n' )
		DrawEntryCulDeSac( hDC, x, y, ptList12 );
	if ( c == 'X' || c == 'x' )
		DrawExitCulDeSac( hDC, x, y, ptList12 );
	if ( c == 'R' || c == 'r' )
		DrawRocketCulDeSac( hDC, x, y, ptList12 );
	if ( c == 'A' || c == 'a' )
		DrawAmmoCulDeSac( hDC, x, y, ptList12 );
	if ( c == 'S' || c == 's' )
		DrawKillerCulDeSac( hDC, x, y, ptList12 );
	if ( c >= '1' && c <= '9' )
		DrawCollectibleCulDeSac( hDC, x, y, ptList12 );
	// Check 2 o'clock
	c = PORTTYPE_02(ptCell.x, ptCell.y);
	if ( c == 'M' || c == 'm' )
		DrawEmptyCulDeSac( hDC, x, y, ptList2 );
	if ( c == 'N' || c == 'n' )
		DrawEntryCulDeSac( hDC, x, y, ptList2 );
	if ( c == 'X' || c == 'x' )
		DrawExitCulDeSac( hDC, x, y, ptList2 );
	if ( c == 'R' || c == 'r' )
		DrawRocketCulDeSac( hDC, x, y, ptList2 );
	if ( c == 'A' || c == 'a' )
		DrawAmmoCulDeSac( hDC, x, y, ptList2 );
	if ( c == 'S' || c == 's' )
		DrawKillerCulDeSac( hDC, x, y, ptList2 );
	if ( c >= '1' && c <= '9' )
		DrawCollectibleCulDeSac( hDC, x, y, ptList2 );
	// Check 4 o'clock
	c = PORTTYPE_04(ptCell.x, ptCell.y);
	if ( c == 'M' || c == 'm' )
		DrawEmptyCulDeSac( hDC, x, y, ptList4 );
	if ( c == 'N' || c == 'n' )
		DrawEntryCulDeSac( hDC, x, y, ptList4 );
	if ( c == 'X' || c == 'x' )
		DrawExitCulDeSac( hDC, x, y, ptList4 );
	if ( c == 'R' || c == 'r' )
		DrawRocketCulDeSac( hDC, x, y, ptList4 );
	if ( c == 'A' || c == 'a' )
		DrawAmmoCulDeSac( hDC, x, y, ptList4 );
	if ( c == 'S' || c == 's' )
		DrawKillerCulDeSac( hDC, x, y, ptList4 );
	if ( c >= '1' && c <= '9' )
		DrawCollectibleCulDeSac( hDC, x, y, ptList4 );
	// Check 6 o'clock
	c = PORTTYPE_06(ptCell.x, ptCell.y);
	if ( c == 'M' || c == 'm' )
		DrawEmptyCulDeSac( hDC, x, y, ptList6 );
	if ( c == 'N' || c == 'n' )
		DrawEntryCulDeSac( hDC, x, y, ptList6 );
	if ( c == 'X' || c == 'x' )
		DrawExitCulDeSac( hDC, x, y, ptList6 );
	if ( c == 'R' || c == 'r' )
		DrawRocketCulDeSac( hDC, x, y, ptList6 );
	if ( c == 'A' || c == 'a' )
		DrawAmmoCulDeSac( hDC, x, y, ptList6 );
	if ( c == 'S' || c == 's' )
		DrawKillerCulDeSac( hDC, x, y, ptList6 );
	if ( c >= '1' && c <= '9' )
		DrawCollectibleCulDeSac( hDC, x, y, ptList6 );
	// Check 8 o'clock
	c = PORTTYPE_08(ptCell.x, ptCell.y);
	if ( c == 'M' || c == 'm' )
		DrawEmptyCulDeSac( hDC, x, y, ptList8 );
	if ( c == 'N' || c == 'n' )
		DrawEntryCulDeSac( hDC, x, y, ptList8 );
	if ( c == 'X' || c == 'x' )
		DrawExitCulDeSac( hDC, x, y, ptList8 );
	if ( c == 'R' || c == 'r' )
		DrawRocketCulDeSac( hDC, x, y, ptList8 );
	if ( c == 'A' || c == 'a' )
		DrawAmmoCulDeSac( hDC, x, y, ptList8 );
	if ( c == 'S' || c == 's' )
		DrawKillerCulDeSac( hDC, x, y, ptList8 );
	if ( c >= '1' && c <= '9' )
		DrawCollectibleCulDeSac( hDC, x, y, ptList8 );
	// Check 10 o'clock
	c = PORTTYPE_10(ptCell.x, ptCell.y);
	if ( c == 'M' || c == 'm' )
		DrawEmptyCulDeSac( hDC, x, y, ptList10 );
	if ( c == 'N' || c == 'n' )
		DrawEntryCulDeSac( hDC, x, y, ptList10 );
	if ( c == 'X' || c == 'x' )
		DrawExitCulDeSac( hDC, x, y, ptList10 );
	if ( c == 'R' || c == 'r' )
		DrawRocketCulDeSac( hDC, x, y, ptList10 );
	if ( c == 'A' || c == 'a' )
		DrawAmmoCulDeSac( hDC, x, y, ptList10 );
	if ( c == 'S' || c == 's' )
		DrawKillerCulDeSac( hDC, x, y, ptList10 );
	if ( c >= '1' && c <= '9' )
		DrawCollectibleCulDeSac( hDC, x, y, ptList10 );
}

/***********************************************************************/
void CWorld::DrawEmptyCulDeSac( HDC hDC, int x, int y, LPPOINT ptList )
/***********************************************************************/
{
	POINT pt[4];

	int iZoom = iZoomFactor;
	int iCount = sizeof(pt) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		pt[i].x = x + ScaleInt( ptList[i].x, iZoom );
		pt[i].y = y + ScaleInt( ptList[i].y, iZoom );
	}

	DrawPolyline( hDC, pt, iCount, hPenG );
}

/***********************************************************************/
void CWorld::DrawEntryCulDeSac( HDC hDC, int x, int y, LPPOINT ptList )
/***********************************************************************/
{
	POINT pt[4];	
	int iZoom = iZoomFactor;
	int iCount = sizeof(pt) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		pt[i].x = x + ScaleInt( ptList[i].x, iZoom );
		pt[i].y = y + ScaleInt( ptList[i].y, iZoom );
	}

	DrawPolyline( hDC, pt, iCount, hPenC );
}

/***********************************************************************/
void CWorld::DrawExitCulDeSac( HDC hDC, int x, int y, LPPOINT ptList )
/***********************************************************************/
{
	POINT pt[4];

	int iZoom = iZoomFactor;
	int iCount = sizeof(pt) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		pt[i].x = x + ScaleInt( ptList[i].x, iZoom );
		pt[i].y = y + ScaleInt( ptList[i].y, iZoom );
	}

	DrawPolyline( hDC, pt, iCount, hPenC );
}


/***********************************************************************/
void CWorld::DrawCollectibleCulDeSac( HDC hDC, int x, int y, LPPOINT ptList )
/***********************************************************************/
{
	POINT pt[4];

	int iZoom = iZoomFactor;
	int iCount = sizeof(pt) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		pt[i].x = x + ScaleInt( ptList[i].x, iZoom );
		pt[i].y = y + ScaleInt( ptList[i].y, iZoom );
	}

	DrawPolygon( hDC, pt, iCount, hPenR, hBrushR );
}

/***********************************************************************/
void CWorld::DrawRocketCulDeSac( HDC hDC, int x, int y, LPPOINT ptList )
/***********************************************************************/
{
	POINT pt[4];

	int iZoom = iZoomFactor;
	int iCount = sizeof(pt) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		pt[i].x = x + ScaleInt( ptList[i].x, iZoom );
		pt[i].y = y + ScaleInt( ptList[i].y, iZoom );
	}

	DrawPolygon( hDC, pt, iCount, hPenM, hBrushM );
}

/***********************************************************************/
void CWorld::DrawAmmoCulDeSac( HDC hDC, int x, int y, LPPOINT ptList )
/***********************************************************************/
{
	POINT pt[4];

	int iZoom = iZoomFactor;
	int iCount = sizeof(pt) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		pt[i].x = x + ScaleInt( ptList[i].x, iZoom );
		pt[i].y = y + ScaleInt( ptList[i].y, iZoom );
	}

	DrawPolygon( hDC, pt, iCount, hPenM, hBrushM );
}

/***********************************************************************/
void CWorld::DrawKillerCulDeSac( HDC hDC, int x, int y, LPPOINT ptList )
/***********************************************************************/
{
	POINT pt[4];

	int iZoom = iZoomFactor;
	int iCount = sizeof(pt) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		pt[i].x = x + ScaleInt( ptList[i].x, iZoom );
		pt[i].y = y + ScaleInt( ptList[i].y, iZoom );
	}

	DrawPolygon( hDC, pt, iCount, hPenM, hBrushM );
}

/***********************************************************************/
void CWorld::DrawBlob2x2( HDC hDC, int x, int y )
/***********************************************************************/
{
	MoveToEx( hDC, x, y, NULL );
	LineTo( hDC, x+xDispDelta, y );
	LineTo( hDC, x+xDispDelta, y+iDispHeight );
	LineTo( hDC, x, y+iDispHeight );
	LineTo( hDC, x, y );
//	MoveToEx( hDC, x, y, NULL );
//	LineTo( hDC, x+1, y );
//	LineTo( hDC, x+1, y+1 );
//	LineTo( hDC, x,	  y+1 );
//	LineTo( hDC, x,	  y );
}

/***********************************************************************/
void CWorld::DrawBlob8x7( HDC hDC, int x, int y )
/***********************************************************************/
{
	MoveToEx( hDC, x+1, y, NULL );
	LineTo( hDC, x+xDispDelta, y );

	MoveToEx( hDC, x+xDispDelta, y+1, NULL );
	LineTo( hDC, x+xDispDelta, y+(iDispHeight/2) );

	MoveToEx( hDC, x+xDispDelta, y+(iDispHeight/2)+1, NULL );
	LineTo( hDC, x+xDispDelta, y+iDispHeight );

	MoveToEx( hDC, x+xDispDelta-1, y+iDispHeight, NULL );
	LineTo( hDC, x, y+iDispHeight );

	MoveToEx( hDC, x, y+iDispHeight-1, NULL );
	LineTo( hDC, x, y+(iDispHeight/2) );

	MoveToEx( hDC, x, y+(iDispHeight/2)-1, NULL );
	LineTo( hDC, x, y );

//	MoveToEx( hDC, x, y, NULL );
//	LineTo( hDC, x+3, y );
//	MoveToEx( hDC, x, y+3, NULL );
//	LineTo( hDC, x+3, y+3 );
//	MoveToEx( hDC, x-1, y+1, NULL );
//	LineTo( hDC, x-1, y+3 );
//	MoveToEx( hDC, x+3, y+1, NULL );
//	LineTo( hDC, x+3, y+3 );
}

//*********************************************************************************************
char CWorld::Move( LPPOINT lpPoint, int xTest, int yTest, int xDelta, int yDelta )
//*********************************************************************************************
{
	if ( !lpPoint )
		return( 0 );

	// Make sure this point is a cell center - the only place to turn
	int x = lpPoint->x;
	int y = lpPoint->y;
	MapCellUnitToCellCenter( &x, &y );
	if ( x != lpPoint->x || y != lpPoint->y )
		return( 0 );

	// Test the cell we're about to head for, to see if it exists
	char c;
	if ( !(c = CELLTYPE( lpPoint->x + xTest, lpPoint->y + yTest )) )
		c = 'C'; // cul de sac

	lpPoint->x += xDelta;
	lpPoint->y += yDelta;
	return( c );
}

//*********************************************************************************************
char CWorld::Move2( LPPOINT lpPoint )	
//*********************************************************************************************
{
	return( Move( lpPoint, 4, -2, 1, -1 ) );
}

//*********************************************************************************************
char CWorld::Move4( LPPOINT lpPoint )
//*********************************************************************************************
{					
	return( Move( lpPoint, 4, 2, 1, 1 ) );
}
		
//*********************************************************************************************
char CWorld::Move6( LPPOINT lpPoint )
//*********************************************************************************************
{															
	return( Move( lpPoint, 0, 4, 0, 1 ) );
}

//*********************************************************************************************
char CWorld::Move8( LPPOINT lpPoint )
//*********************************************************************************************
{		
	return( Move( lpPoint, -4, 2, -1, 1 ) );
} 
	
//*********************************************************************************************
char CWorld::Move10( LPPOINT lpPoint )
//*********************************************************************************************
{		
	return( Move( lpPoint, -4, -2, -1, -1 ) );
} 
	
//*********************************************************************************************
char CWorld::Move12( LPPOINT lpPoint )
//*********************************************************************************************
{														
	return( Move( lpPoint, 0, -4, 0, -1 ) );
}

/***********************************************************************/
char CWorld::MoveNewDirection( LPPOINT lpPoint, int iDir )
/***********************************************************************/
{
	if ( !lpPoint )
		return( 0 );

	switch( iDir )
	{
		case 2:
			return( Move2( lpPoint ) );
		case 4:
			return( Move4( lpPoint ) );
		case 6:
			return( Move6( lpPoint ) );
		case 8:
			return( Move8( lpPoint ) );
		case 10:
			return( Move10( lpPoint ) );
		case 12:
			return( Move12( lpPoint ) );
	}

	return( 0 );
}

/***********************************************************************/
BOOL CWorld::MoveSameDirection( LPPOINT lpPoint, int iDir )
/***********************************************************************/
{
	if ( !lpPoint )
		return( NO );

	// Make the point relative to the upper-left corner of the cell
	int x = lpPoint->x;
	int y = lpPoint->y;
	MapCellUnitToCellCenter( &x, &y );
	x = lpPoint->x - (x-2);
	y = lpPoint->y - (y-2);

	// Legalize the position within the cell - 
	// the only legal positions in the cell (for us or the opponents) are:
	// (1,1) (2,1) (3,1) (2,2) (1,3) (2,3) (3,3)
	if ( x < 1 ) { lpPoint->x -= (x-1); x = 1; } else
	if ( x > 3 ) { lpPoint->x -= (x-3); x = 3; }
	if ( y < 1 ) { lpPoint->y -= (y-1); y = 1; } else
	if ( y > 3 ) { lpPoint->y -= (y-3); y = 3; }
	if ( x < 2 && y == 2 ) { lpPoint->x -= (x-2); x = 2; } else
	if ( x > 2 && y == 2 ) { lpPoint->x -= (x-2); x = 2; }

	if ( x == 2 && y == 2 )
		return( NO ); // get out and allow a new direction to be calculated

	// Move the point along the desired direction
	int iPosition = (x*10) + y;
	switch ( iPosition )
	{ // some directions are not possible
		case 11:
		{
			if ( iDir ==  4 ) { lpPoint->x += 1; lpPoint->y += 1; } else
			if ( iDir == 10 ) { lpPoint->x -= 2; lpPoint->y += 0; }
			break;
		}
		case 21:
		{
			if ( iDir == 12 ) { lpPoint->x += 0; lpPoint->y -= 2; } else
			if ( iDir ==  6 ) { lpPoint->x += 0; lpPoint->y += 1; }
			break;
		}
		case 31:
		{
			if ( iDir ==  2 ) { lpPoint->x += 2; lpPoint->y += 0; } else
			if ( iDir ==  8 ) { lpPoint->x -= 1; lpPoint->y += 1; }
			break;
		}
		case 13:
		{
			if ( iDir ==  2 ) { lpPoint->x += 1; lpPoint->y -= 1; } else
			if ( iDir ==  8 ) { lpPoint->x -= 2; lpPoint->y += 0; }
			break;
		}
		case 23:
		{
			if ( iDir == 12 ) { lpPoint->x += 0; lpPoint->y -= 1; } else
			if ( iDir ==  6 ) { lpPoint->x += 0; lpPoint->y += 2; }
			break;
		}
		case 33:
		{
			if ( iDir ==  4 ) { lpPoint->x += 2; lpPoint->y+=0; } else
			if ( iDir == 10 ) { lpPoint->x -= 1; lpPoint->y-=1; }
			break;
		}
	}

	return( YES );
}

/***********************************************************************/
void CWorld::MoveDrone( LPOPPONENT lpOpp )
/***********************************************************************/
{
	if ( MoveSameDirection( &lpOpp->ptCell, lpOpp->iDirection ) )
		return;

	WORD w = GetRandomNumber(3);
	if ( w == 0 )
		lpOpp->iDirection += 2; // right
	else
	if ( w == 1 )
		lpOpp->iDirection += 10; // left
	// else straight (no change)

	if ( lpOpp->iDirection > 12 )
		 lpOpp->iDirection -= 12;

	if ( MoveNewDirection( &lpOpp->ptCell, lpOpp->iDirection ) )
		return;

	for ( int i = 0; i < 6; i++ )
	{
		lpOpp->iDirection += 2;

		if ( lpOpp->iDirection > 12 )
			 lpOpp->iDirection -= 12;

		if ( MoveNewDirection( &lpOpp->ptCell, lpOpp->iDirection ) )
			break;
	}
}

/***********************************************************************/
void CWorld::MoveSentryCCW( LPOPPONENT lpOpp )
/***********************************************************************/
{
	if ( MoveSameDirection( &lpOpp->ptCell, lpOpp->iDirection ) )
		return;

	lpOpp->iDirection += 2;
	if ( lpOpp->iDirection > 12 )
		lpOpp->iDirection -= 12;

	if ( !MoveNewDirection( &lpOpp->ptCell, lpOpp->iDirection ) )
	{	
		lpOpp->iDirection += 6;
		if ( lpOpp->iDirection > 12 )
			lpOpp->iDirection -= 12;

		lpOpp->iBehavior = 3; // Sentry CW
	}
}

/***********************************************************************/
void CWorld::MoveSentryCW( LPOPPONENT lpOpp )
/***********************************************************************/
{
	if ( MoveSameDirection( &lpOpp->ptCell, lpOpp->iDirection ) )
		return;

	lpOpp->iDirection += 10;
	if ( lpOpp->iDirection > 12 )
		lpOpp->iDirection -= 12;

	if ( !MoveNewDirection( &lpOpp->ptCell, lpOpp->iDirection ) )
	{	
		lpOpp->iDirection += 6;
		if ( lpOpp->iDirection > 12 )
			lpOpp->iDirection -= 12;

		lpOpp->iBehavior = 2; // Sentry CCW
	}
}

/***********************************************************************/
void CWorld::MoveHunter( LPOPPONENT lpOpp )
/***********************************************************************/
{
	if ( MoveSameDirection( &lpOpp->ptCell, lpOpp->iDirection ) )
		return;

	POINT pntOpponent = lpOpp->ptCell;
	POINT pntMove[6];				 
	pntMove[0] = pntOpponent;	if ( !Move2( &pntMove[0]) )	pntMove[0].x = 9999;
	pntMove[1] = pntOpponent;	if ( !Move4( &pntMove[1]) )	pntMove[1].x = 9999;
	pntMove[2] = pntOpponent;	if ( !Move6( &pntMove[2]) )	pntMove[2].x = 9999;
	pntMove[3] = pntOpponent;	if ( !Move8( &pntMove[3]) )	pntMove[3].x = 9999;
	pntMove[4] = pntOpponent;	if ( !Move10(&pntMove[4]) )	pntMove[4].x = 9999;
	pntMove[5] = pntOpponent;	if ( !Move12(&pntMove[5]) )	pntMove[5].x = 9999;
	
	POINT pntPlayer = ptPlayerLoc;
	int iDistFlag;
	long lMinDist = 9999;
	for ( int iLoop = 0; iLoop < 6; iLoop++ )
	{
		char c = CELLTYPE(pntMove[iLoop].x, pntMove[iLoop].y);
		if ( !c || c == 'C' || c == 'T' )
			continue;
		long lx = pntPlayer.x - pntMove[iLoop].x;
		long ly = pntPlayer.y - pntMove[iLoop].y;
		long lDistance = (lx * lx) + (ly * ly);
		if (lDistance < lMinDist)
		{
			iDistFlag = iLoop;
			lMinDist = lDistance;
		}					
	}
	
	lpOpp->iDirection = (iDistFlag * 2) + 2;
	if ( lpOpp->iDirection > 12 )
		lpOpp->iDirection -= 12;

	MoveNewDirection( &lpOpp->ptCell, lpOpp->iDirection );
}

/***********************************************************************/
void CWorld::MoveTerrorist( LPOPPONENT lpOpp )
/***********************************************************************/
{
	if ( MoveSameDirection( &lpOpp->ptCell, lpOpp->iDirection ) )
		return;

	POINT pntOpponent = lpOpp->ptCell;
	POINT pntMove[6];				 
	pntMove[0] = pntOpponent;	if ( !Move2( &pntMove[0]) )	pntMove[0].x = 9999;
	pntMove[1] = pntOpponent;	if ( !Move4( &pntMove[1]) )	pntMove[1].x = 9999;
	pntMove[2] = pntOpponent;	if ( !Move6( &pntMove[2]) )	pntMove[2].x = 9999;
	pntMove[3] = pntOpponent;	if ( !Move8( &pntMove[3]) )	pntMove[3].x = 9999;
	pntMove[4] = pntOpponent;	if ( !Move10(&pntMove[4]) )	pntMove[4].x = 9999;
	pntMove[5] = pntOpponent;	if ( !Move12(&pntMove[5]) )	pntMove[5].x = 9999;
	
	POINT pntPlayer	= ptPlayerLoc;
	long lx = pntPlayer.x - pntOpponent.x;
	long ly = pntPlayer.y - pntOpponent.y;
	long lDistance = (lx * lx) + (ly * ly);
	if ( lDistance >= 27 ) // Player is not withing 3 cells - opponents degenerates to a drone
	{
		MoveDrone( lpOpp );
		return;
	}

	int iDistFlag;
	long lMinDist = 9999;
	for ( int iLoop = 0; iLoop < 6; iLoop++ )
	{
		char c = CELLTYPE(pntMove[iLoop].x, pntMove[iLoop].y);
		if ( !c || c == 'C' || c == 'T' )
			continue;
		long lx = pntPlayer.x - pntMove[iLoop].x;
		long ly = pntPlayer.y - pntMove[iLoop].y;
		long lDistance = (lx * lx) + (ly * ly);
		if (lDistance < lMinDist)
		{
			iDistFlag = iLoop;
			lMinDist = lDistance;
		}					
	}

	lpOpp->iDirection = (iDistFlag * 2) + 2;
	if ( lpOpp->iDirection > 12 )
		lpOpp->iDirection -= 12;

	MoveNewDirection( &lpOpp->ptCell, lpOpp->iDirection );
}

/***********************************************************************/
void CWorld::MoveCoward( LPOPPONENT lpOpp )
/***********************************************************************/
{
	if ( MoveSameDirection( &lpOpp->ptCell, lpOpp->iDirection ) )
		return;

	POINT pntOpponent = lpOpp->ptCell;
	POINT pntMove[6];				 
	pntMove[0] = pntOpponent;	if ( !Move2( &pntMove[0]) )	pntMove[0].x = 9999;
	pntMove[1] = pntOpponent;	if ( !Move4( &pntMove[1]) )	pntMove[1].x = 9999;
	pntMove[2] = pntOpponent;	if ( !Move6( &pntMove[2]) )	pntMove[2].x = 9999;
	pntMove[3] = pntOpponent;	if ( !Move8( &pntMove[3]) )	pntMove[3].x = 9999;
	pntMove[4] = pntOpponent;	if ( !Move10(&pntMove[4]) )	pntMove[4].x = 9999;
	pntMove[5] = pntOpponent;	if ( !Move12(&pntMove[5]) )	pntMove[5].x = 9999;
	
	POINT pntPlayer	= ptPlayerLoc;
	int iDistFlag;
	long lMinDist = 9999;
	for ( int iLoop = 0; iLoop < 6; iLoop++ )
	{
		char c = CELLTYPE(pntMove[iLoop].x, pntMove[iLoop].y);
		if ( !c || c == 'C' || c == 'T' )
			continue;
		long lx = pntPlayer.x - pntMove[iLoop].x;
		long ly = pntPlayer.y - pntMove[iLoop].y;
		long lDistance = (lx * lx) + (ly * ly);
		if (lDistance < lMinDist)
		{
			iDistFlag = iLoop;
			lMinDist = lDistance;
		}					
	}
	
	lpOpp->iDirection = (iDistFlag * 2) + 2;
	if ( lpOpp->iDirection > 12 )
		lpOpp->iDirection -= 12;

	MoveNewDirection( &lpOpp->ptCell, lpOpp->iDirection );
}

/***********************************************************************/
void CWorld::MoveOpponents()
/***********************************************************************/
{
	// Advance the player's location, but don't enter a new cell
	MoveSameDirection( &ptPlayerLoc, iPlayerDir );

	LPOPPONENT lpOpp = lpOpponent;														
	while (lpOpp)
	{																			
		switch (lpOpp->iBehavior)
		{														 
			case 1: // Drone - Proceeds in straight paths until cannot move and then randomly changes direction
			{					
				MoveDrone( lpOpp );
				break;	
			}			

			case 2: // Sentry - Travels in counter - clockwise circle *** 
			{ 
				MoveSentryCCW( lpOpp );
				break;		
			}			

			case 3:	// Sentry - Travels in Clockwise circle ***
			{ 
				MoveSentryCW( lpOpp );
				break;		
			}			

			case 4:	// Hunter - Minimizes distance between player and himself ***
			{
				MoveHunter( lpOpp );
				break;
			}			
			case 5:	// Terrorist - Chases player when within two hexes.	Destroys player upon impact *** 
			{		 
				MoveTerrorist( lpOpp );
				break;
			}			
			case 6:	// Coward - Maximizes distance between player and himself.	Must be cornered to destroy ***
			{
				MoveCoward( lpOpp );
				break;
			} 
		}

		lpOpp = lpOpp->lpNextOpp;
	}

	UpdateView();  
}
	
//*********************************************************************************************
BOOL CWorld::GetCellData( LPCELLINFO lpCellInfo )
//*********************************************************************************************
{
	// Force player to get to final loc. looking up the cell info
	POINT pt;
	GetPlayerLoc( &pt );

	char c = CELLTYPE( pt.x, pt.y );
	if ( !c )
		c = 'C'; // cul-de-sac
	lpCellInfo->szCellType    = c;
	lpCellInfo->ptPlayerLoc   = pt;
	lpCellInfo->szLookType    = LOOKTYPE(   pt.x, pt.y);
	lpCellInfo->szPortType_02 = PORTTYPE_02(pt.x, pt.y);
	lpCellInfo->szPortType_04 = PORTTYPE_04(pt.x, pt.y);
	lpCellInfo->szPortType_06 = PORTTYPE_06(pt.x, pt.y);
	lpCellInfo->szPortType_08 = PORTTYPE_08(pt.x, pt.y);
	lpCellInfo->szPortType_10 = PORTTYPE_10(pt.x, pt.y);
	lpCellInfo->szPortType_12 = PORTTYPE_12(pt.x, pt.y);
	lpCellInfo->szOppoType_02 = OPPOTYPE_02(pt.x, pt.y);
	lpCellInfo->szOppoType_04 = OPPOTYPE_04(pt.x, pt.y);
	lpCellInfo->szOppoType_06 = OPPOTYPE_06(pt.x, pt.y);
	lpCellInfo->szOppoType_08 = OPPOTYPE_08(pt.x, pt.y);
	lpCellInfo->szOppoType_10 = OPPOTYPE_10(pt.x, pt.y);
	lpCellInfo->szOppoType_12 = OPPOTYPE_12(pt.x, pt.y);
	return( YES );
}
