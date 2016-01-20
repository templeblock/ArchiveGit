#include <windows.h>
#include "proto.h"
#include "cllsn.h"
#include "worlds.h"		
#include "Cells.h"

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

//************************************************************************
char CWorld::PortType(LPCELLINFO lpc, int iPortNum)
//************************************************************************
{
	// Adjust portal number to the clock
	while (iPortNum < 1)
		iPortNum += 12;

	while (iPortNum > 12)
		iPortNum -= 12;

	// If odd then not a valid portal (direction)
	if (iPortNum & 1)
		return 0;

	int i = iPortNum / 2 - 1;
	return lpc->cPortType[i];
}

//************************************************************************
char CWorld::OppoType(LPCELLINFO lpc, int iPortNum)
//************************************************************************
{
	// Adjust portal number to the clock
	while (iPortNum < 1)
		iPortNum += 12;

	while (iPortNum > 12)
		iPortNum -= 12;

	// If odd then not a valid portal (direction)
	if (iPortNum & 1)
		return 0;

	int i = iPortNum / 2 - 1;
	return lpc->cOppoType[i];
}

// The CONSTRUCTOR
//************************************************************************
CWorld::CWorld()
//************************************************************************
{
	m_pCells = NULL;
	m_nHeight = -1;
	m_nWidth = -1;
	m_nCellCount = 0;

	m_pGroup = NULL;
	m_pPlayer = NULL;

	hWndDraw = NULL;
	lpMap = NULL;
	xCellUnits = yCellUnits = 0;
	ptViewLocation.x = 0;
	ptViewLocation.y = 0;
	ptViewLast = ptViewLocation;
	iZoomFactor = 3;
	SetZoomFactor( iZoomFactor );
	hPenG	= CreatePen( PS_SOLID, 1, RGB(  0,255,  0) );
	hPenR	= CreatePen( PS_SOLID, 1, RGB(255,  0,  0) );
	hPenY	= CreatePen( PS_SOLID, 1, RGB(255,255,  0) );
	hPenW	= CreatePen( PS_SOLID, 1, RGB(255,255,255) );
	hPenM	= CreatePen( PS_SOLID, 1, RGB(255,  0,255) );
	hPenC	= CreatePen( PS_SOLID, 1, RGB(  0,255,255) );
	hPenB	= CreatePen( PS_SOLID, 1, RGB(  0,  0,255) );
	hPenHell = CreatePen(PS_SOLID, 2, RGB(255,  0,  0) );
	hPenIce = CreatePen( PS_SOLID, 2, RGB(  0,255,255) );
	hBrushR	= CreateSolidBrush( RGB(255,  0,  0) );
	hBrushY	= CreateSolidBrush( RGB(255,255,  0) );
	hBrushB	= CreateSolidBrush( RGB(  0,  0,255) );
	hBrushM	= CreateSolidBrush( RGB(255,  0,255) );

	lScore = 0L;
}

//***********************************************************************
CCell *CWorld::GetCell(int x, int y)
//***********************************************************************
{
	// Get the cell from the cell map given the x,y coordinate

	if (x < 0 || y < 0) 
		return NULL;
	if (x >= m_nWidth || y >= m_nHeight)
		return NULL;

	if (!m_pCells)
		return NULL;

	int nOffset = (y * m_nWidth) + x;

	return *(m_pCells + nOffset);
}

// The DESTRUCTOR
//************************************************************************
CWorld::~CWorld()
//************************************************************************
{
	// Delete opponent group
	if (m_pGroup)
		delete m_pGroup;

	if (m_pPlayer)
		delete m_pPlayer;
	
	// Delete the cells and the cell map
	if (m_pCells)
	{
		Debug("Delete %d cells\n", m_nCellCount);
		for(int i=0; i<m_nCellCount; i++)
			delete (CCell *)*(m_pCells + i);
		GlobalFreePtr(m_pCells);
	}

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
	if (hPenHell)
		DeletePen( hPenHell );
	if (hPenIce)
		DeletePen( hPenIce );
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
		GlobalFreePtr( lpMap );
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
BOOL CWorld::IsValidCulDeSac( unsigned char c )
//*********************************************************************************************
{
	return( c == 'M' || c == 'm' || c == 'N' || c == 'n' || c == 'X' || c == 'x' ||
		    c == 'R' || c == 'r' || c == 'A' || c == 'a' || c == 'S' || c == 's' ||
		   (c >= '1' && c <= '9') || ( c >= PORT_START && c <= PORT_END));
}

/***********************************************************************/
char CWorld::GetCulDeSacType( LPCELLINFO lpCellInfo, int iDir )
/***********************************************************************/
{
	if ( !lpCellInfo )
		return( NULL );

	// Need to look behind us
	iDir += 6;
	if (iDir > 12)
		iDir -= 12;

	char cType = PortType(lpCellInfo, iDir);

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

	if ( CELLTYPE(x, y) )
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
BOOL CWorld::CellIsGenerator( int x, int y )
//*********************************************************************************************
{
	int dx = x;
	int dy = y;
	MapCellUnitToCellCenter( &x, &y );
	dx -= x;
	dy -= y;
	if ( dx || dy )
		return( NO ); // only process cell centers

	char c = CELLTYPE(x, y);

	if (c == 'O')
		return( YES );

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
	
	// Create opponent group object
	m_pGroup = new COpponentGroup();
	// Create player object
	m_pPlayer = new CPlayer();

	ifstream in;

	in.open("level01.txt");

	if (in.bad())
		return FALSE;

	BOOL bRet = LoadLevel( in, 1 );

	in.close();

	m_pPlayer->SetNewShotCell();

	return bRet;
}

//***********************************************************************
BOOL CWorld::LoadLevel( int iLevel )
//***********************************************************************
{
	return(FALSE);
}

//*********************************************************************************************
int CWorld::GetLevel( void )
//*********************************************************************************************
{
	return(iCurrentLevel);
}

//***********************************************************************
void CWorld::CenterPlayerView( BOOL bScroll )
//***********************************************************************
{
	if (m_pPlayer)
	{
		POINT pt = m_pPlayer->GetXYCell();
		int x = pt.x;
		int y = pt.y;
		MapCellToCellUnit(&x, &y);
		MapCellUnitToView(&x, &y);
		pt.x = x;
		pt.y = y;
		SetViewLocation(&pt, bScroll);
	}
}

//***********************************************************************
void CWorld::CenterView( CCell *pCell, BOOL bScroll )
//***********************************************************************
{
	int x = pCell->Getx();
	int y = pCell->Gety();

	MapCellToCellUnit(&x, &y);
	MapCellUnitToView(&x, &y);

	POINT pt;

	pt.x = x;
	pt.y = y;
	SetViewLocation(&pt, bScroll);
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

	CenterPlayerView( NO/*bScroll*/ );
}

//***********************************************************************
int CWorld::GetZoomFactor( void )
//***********************************************************************
{
	return( iZoomFactor );
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

			CCell *pCell = GetCell(ptCell.x, ptCell.y);
			if (pCell && pCell->IsVisible())
				DrawCell( hDC, ptCell );
		}
	}
	
	SelectPen( hDC, hOldPen ); 
	DrawOpponents( hDC, YES );
	DrawPlayer( hDC, YES );	
}

/***********************************************************************/
void CWorld::DrawCell(int x, int y)
/***********************************************************************/
{	//
	// Force drawing of a single cell (used in fog of war feature)
	//

	HDC hDC = GetDC( hWndDraw );
	if ( !hDC )
		return;

	POINT pt;
	pt.x = x;
	pt.y = y;

	HPEN hOldPen = SelectPen( hDC, hPenG );

	DrawCell(hDC, pt);

	SelectPen( hDC, hOldPen ); 

	ReleaseDC( hWndDraw, hDC );
}

// Create a COffScreen object when CWorld is constructed; size as radar
// Load the COffScreen repair dib with a background texture
// When painting the radar, copy the repair dib over the COffScreen main dib
// Draw all the objects into the COffScreen main dib (with transparency)
// Blt the entire COffScreen main dib to the radar screen's DC

/***********************************************************************/
void CWorld::TestDrawDib( HDC hDC, int x, int y, ITEMID id )
/***********************************************************************/
{
	// Use the DIB's GDI copying method
	PDIB pDib = CDib::LoadDibFromResource(
		App.GetInstance(),
		MAKEINTRESOURCE(id),
		App.m_hPal);

	if (!pDib)
		return;

	int iSrcWidth  = pDib->GetWidth();
	int iSrcHeight = pDib->GetHeight();

	int iZoom = iZoomFactor - 2;
	if (iZoom == 0 || iZoom == -1)
		iZoom--;

	int iDstWidth  = ScaleInt( iSrcWidth, iZoom );
	int iDstHeight = ScaleInt( iSrcHeight, iZoom );

	pDib->DCBlt( hDC,
		/*Dst*/ x, y, iDstWidth, iDstHeight,
		/*Src*/ 0, 0, iSrcWidth, iSrcHeight );
}

/***********************************************************************/
void CWorld::DrawCell( HDC hDC, POINT ptCell )
/***********************************************************************/
{
	POINT ptSaveCell = ptCell;
	int x = ptCell.x;
	int y = ptCell.y;
	MapCellToCellUnit( &x, &y ); // the center cell unit
	ptCell.x = x;
	ptCell.y = y;

	MapCellUnitToRadar( &x, &y );
	// offset by half a cell
	x -= (iDispWidth/2);
	y -= (iDispHeight/2);

	DrawPortals( hDC, x, y, ptCell );

	CCell *pCell = GetCell(ptSaveCell.x, ptSaveCell.y);
	if (!pCell)
		return;

	char c = pCell->Type();
	//char c = CELLTYPE(ptCell.x, ptCell.y);

	int iLook = pCell->Look() - '0';
	//int iLook = LOOKTYPE(ptCell.x, ptCell.y) - '0';

	switch ( c )
	{		
		case 'H':
		{				 
			TestDrawDib( hDC, x, y, IDC_RADAR_HEX0 );
			//DrawHex( hDC, x, y, iLook);
			break;
		}
		case 'U':
		{						 
			TestDrawDib( hDC, x, y, IDC_RADAR_UNJ0 );
			//DrawUnionJack( hDC, x, y, iLook );
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
		case 'O':
		{
			DrawOppGenerator( hDC, x, y );
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
	static POINT pnt;
	POINT Cellpnt;

	if ( bOn )
	{
		Cellpnt = m_pPlayer->GetXYCell();
		pnt = m_pPlayer->GetNewPos();
		pnt.x = ScaleInt( pnt.x, iZoomFactor );
		pnt.y = ScaleInt( pnt.y, iZoomFactor );

		MapCellToCellUnit((int *)&Cellpnt.x, (int *)&Cellpnt.y);
		MapCellUnitToRadar((int *)&Cellpnt.x, (int *)&Cellpnt.y);

		Cellpnt.x -= (iDispWidth/2);
		Cellpnt.y -= (iDispHeight/2);

		x = pnt.x + Cellpnt.x;
		y = pnt.y + Cellpnt.y;

		iDir = m_pPlayer->GetDir();
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
void CWorld::DrawOpponents( HDC hDC, BOOL bOn )
/***********************************************************************/
{				
	COpp *pNext;
	COpp *pCurr = m_pGroup->GetFirst();
	while(pCurr)
	{
		pNext = pCurr->GetNext();
		//if (pCurr->IsVisible())
			DrawOpponent(hDC, bOn, pCurr);
		pCurr = pNext;
	}
	return;
}

/***********************************************************************/
void CWorld::DrawOpponent( HDC hDC, BOOL bOn, COpp *pOpp )
/***********************************************************************/
{				
	POINT pnt;
	POINT Cellpnt;

	if ( !bOn )
	{
		pnt = pOpp->GetLastDraw();
		//x = pOpp->pntLastDraw.x;
		//y = pOpp->pntLastDraw.y;
	}
	else
	{
		// If the opponent was marked for destruction...
		if (pOpp->JustDestroyed())
		{
			// Go ahead and remove it
			m_pGroup->DestroyOpp(pOpp);

			// Return (don't draw it)
			return;
		}

		Cellpnt = pOpp->GetXYCell();

		pnt = pOpp->GetNewPos();

		pnt.x = ScaleInt( pnt.x, iZoomFactor );
		pnt.y = ScaleInt( pnt.y, iZoomFactor );
		
		MapCellToCellUnit((int *)&Cellpnt.x, (int *)&Cellpnt.y);
		MapCellUnitToRadar((int *)&Cellpnt.x, (int *)&Cellpnt.y);

		Cellpnt.x -= (iDispWidth/2);
		Cellpnt.y -= (iDispHeight/2);

		pnt.x += Cellpnt.x;
		pnt.y += Cellpnt.y;

		//x = pOpp->ptCell.x;
		//y = pOpp->ptCell.y;
		//MapCellUnitToRadar( &x, &y );
		//pOpp->pntLastDraw.x = x;
		//pOpp->pntLastDraw.y = y;
		pOpp->SetLastDraw(pnt);
	}

	HPEN hPen;
	HPEN hOldPen = NULL;
	switch (pOpp->GetIndex()+1)
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
		case 7:
		case 8:
		case 9:
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
		MoveToEx( hDC,	pnt.x + ScaleInt( ptFr[i].x, iZoomFactor),
						pnt.y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );		
		LineTo(	  hDC,	pnt.x + ScaleInt( ptTo[i].x, iZoomFactor),
						pnt.y + ScaleInt( ptTo[i].y, iZoomFactor) );
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
void CWorld::DrawHex( HDC hDC, int x, int y, int iLook )
/***********************************************************************/
{
	HPEN hOldPen = NULL;

	if ( iLook == 1)
		hOldPen = SelectPen( hDC, hPenHell );
	else if (iLook == 2)
		hOldPen = SelectPen( hDC, hPenIce );

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

	static POINT ptFr[] = { 15,9, 16,7, 13,2, 12,0, 6,0, 4,0, 1,5, 0,7, 3,12, 4,14, 10,14, 12,14 };
	static POINT ptTo[] = { 16,7, 15,5, 12,0, 10,0, 4,0, 3,2, 0,7, 1,9, 4,14, 6,14, 12,14, 13,12 };

	int iCount = sizeof(ptFr) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		MoveToEx( hDC,	x + ScaleInt( ptFr[i].x, iZoomFactor),
						y + ScaleInt( ptFr[i].y, iZoomFactor), NULL );
		LineTo(	  hDC,	x + ScaleInt( ptTo[i].x, iZoomFactor),
						y + ScaleInt( ptTo[i].y, iZoomFactor) );
	}

	if (hOldPen)
		SelectPen(hDC, hOldPen);
}

/***********************************************************************/
void CWorld::DrawUnionJack( HDC hDC, int x, int y, int iLook )
/***********************************************************************/
{
	HPEN hOldPen = NULL;

	if ( iLook == 1)
		hOldPen = SelectPen( hDC, hPenR );
	else if (iLook == 2)
		hOldPen = SelectPen( hDC, hPenIce );

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

	if (hOldPen)
		SelectPen(hDC, hOldPen);
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
void CWorld::DrawOppGenerator( HDC hDC, int x, int y )
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

	unsigned char c;
	// Check 12 o'clock
	c = PORTTYPE_12(ptCell.x, ptCell.y);
	if ( c == 'M' || c == 'm' )
		DrawEmptyCulDeSac( hDC, x, y, ptList12 );
	else
	if ( c == 'N' || c == 'n' )
		DrawEntryCulDeSac( hDC, x, y, ptList12 );
	else
	if ( c == 'X' || c == 'x' )
		DrawExitCulDeSac( hDC, x, y, ptList12 );
	else
	if ( c == 'R' || c == 'r' )
		DrawRocketCulDeSac( hDC, x, y, ptList12 );
	else
	if ( c == 'A' || c == 'a' )
		DrawAmmoCulDeSac( hDC, x, y, ptList12 );
	else
	if ( c == 'S' || c == 's' )
		DrawKillerCulDeSac( hDC, x, y, ptList12 );
	else
	if ( c >= '1' && c <= '9' )
		DrawCollectibleCulDeSac( hDC, x, y, ptList12 );
	else
	if ( c >= PORT_SWITCH_START && c <= PORT_SWITCH_END )
		DrawSwitch( hDC, x, y, ptList12);

	// Check 2 o'clock
	c = PORTTYPE_02(ptCell.x, ptCell.y);
	if ( c == 'M' || c == 'm' )
		DrawEmptyCulDeSac( hDC, x, y, ptList2 );
	else
	if ( c == 'N' || c == 'n' )
		DrawEntryCulDeSac( hDC, x, y, ptList2 );
	else
	if ( c == 'X' || c == 'x' )
		DrawExitCulDeSac( hDC, x, y, ptList2 );
	else
	if ( c == 'R' || c == 'r' )
		DrawRocketCulDeSac( hDC, x, y, ptList2 );
	else
	if ( c == 'A' || c == 'a' )
		DrawAmmoCulDeSac( hDC, x, y, ptList2 );
	else
	if ( c == 'S' || c == 's' )
		DrawKillerCulDeSac( hDC, x, y, ptList2 );
	else
	if ( c >= '1' && c <= '9' )
		DrawCollectibleCulDeSac( hDC, x, y, ptList2 );
	else
	if ( c >= PORT_SWITCH_START && c <= PORT_SWITCH_END )
		DrawSwitch( hDC, x, y, ptList2);

	// Check 4 o'clock
	c = PORTTYPE_04(ptCell.x, ptCell.y);
	if ( c == 'M' || c == 'm' )
		DrawEmptyCulDeSac( hDC, x, y, ptList4 );
	else
	if ( c == 'N' || c == 'n' )
		DrawEntryCulDeSac( hDC, x, y, ptList4 );
	else
	if ( c == 'X' || c == 'x' )
		DrawExitCulDeSac( hDC, x, y, ptList4 );
	else
	if ( c == 'R' || c == 'r' )
		DrawRocketCulDeSac( hDC, x, y, ptList4 );
	else
	if ( c == 'A' || c == 'a' )
		DrawAmmoCulDeSac( hDC, x, y, ptList4 );
	else
	if ( c == 'S' || c == 's' )
		DrawKillerCulDeSac( hDC, x, y, ptList4 );
	else
	if ( c >= '1' && c <= '9' )
		DrawCollectibleCulDeSac( hDC, x, y, ptList4 );
	else
	if ( c >= PORT_SWITCH_START && c <= PORT_SWITCH_END )
		DrawSwitch( hDC, x, y, ptList4);

	// Check 6 o'clock
	c = PORTTYPE_06(ptCell.x, ptCell.y);
	if ( c == 'M' || c == 'm' )
		DrawEmptyCulDeSac( hDC, x, y, ptList6 );
	else
	if ( c == 'N' || c == 'n' )
		DrawEntryCulDeSac( hDC, x, y, ptList6 );
	else
	if ( c == 'X' || c == 'x' )
		DrawExitCulDeSac( hDC, x, y, ptList6 );
	else
	if ( c == 'R' || c == 'r' )
		DrawRocketCulDeSac( hDC, x, y, ptList6 );
	else
	if ( c == 'A' || c == 'a' )
		DrawAmmoCulDeSac( hDC, x, y, ptList6 );
	else
	if ( c == 'S' || c == 's' )
		DrawKillerCulDeSac( hDC, x, y, ptList6 );
	else
	if ( c >= '1' && c <= '9' )
		DrawCollectibleCulDeSac( hDC, x, y, ptList6 );
	else
	if ( c >= PORT_SWITCH_START && c <= PORT_SWITCH_END )
		DrawSwitch( hDC, x, y, ptList6);

	// Check 8 o'clock
	c = PORTTYPE_08(ptCell.x, ptCell.y);
	if ( c == 'M' || c == 'm' )
		DrawEmptyCulDeSac( hDC, x, y, ptList8 );
	else
	if ( c == 'N' || c == 'n' )
		DrawEntryCulDeSac( hDC, x, y, ptList8 );
	else
	if ( c == 'X' || c == 'x' )
		DrawExitCulDeSac( hDC, x, y, ptList8 );
	else
	if ( c == 'R' || c == 'r' )
		DrawRocketCulDeSac( hDC, x, y, ptList8 );
	else
	if ( c == 'A' || c == 'a' )
		DrawAmmoCulDeSac( hDC, x, y, ptList8 );
	else
	if ( c == 'S' || c == 's' )
		DrawKillerCulDeSac( hDC, x, y, ptList8 );
	else
	if ( c >= '1' && c <= '9' )
		DrawCollectibleCulDeSac( hDC, x, y, ptList8 );
	else
	if ( c >= PORT_SWITCH_START && c <= PORT_SWITCH_END )
		DrawSwitch( hDC, x, y, ptList8);

	// Check 10 o'clock
	c = PORTTYPE_10(ptCell.x, ptCell.y);
	if ( c == 'M' || c == 'm' )
		DrawEmptyCulDeSac( hDC, x, y, ptList10 );
	else
	if ( c == 'N' || c == 'n' )
		DrawEntryCulDeSac( hDC, x, y, ptList10 );
	else
	if ( c == 'X' || c == 'x' )
		DrawExitCulDeSac( hDC, x, y, ptList10 );
	else
	if ( c == 'R' || c == 'r' )
		DrawRocketCulDeSac( hDC, x, y, ptList10 );
	else
	if ( c == 'A' || c == 'a' )
		DrawAmmoCulDeSac( hDC, x, y, ptList10 );
	else
	if ( c == 'S' || c == 's' )
		DrawKillerCulDeSac( hDC, x, y, ptList10 );
	else
	if ( c >= '1' && c <= '9' )
		DrawCollectibleCulDeSac( hDC, x, y, ptList10 );
	else
	if ( c >= PORT_SWITCH_START && c <= PORT_SWITCH_END )
		DrawSwitch( hDC, x, y, ptList10);
}

/***********************************************************************/
void CWorld::DrawSwitch( HDC hDC, int x, int y, LPPOINT ptList )
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

	DrawPolyline( hDC, pt, iCount, hPenY );
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
BOOL CWorld::GetCellData( int xCell, int yCell, LPCELLINFO lpCellInfo )
//*********************************************************************************************
{
	MapCellToCellUnit(&xCell, &yCell);
	char c = CELLTYPE( xCell, yCell );
	if ( !c )
		c = 'C'; // cul-de-sac
	lpCellInfo->cCellType    = c;
	lpCellInfo->cLookType    = LOOKTYPE(xCell, yCell);
	lpCellInfo->cPortType[0] = PORTTYPE_02(xCell, yCell);
	lpCellInfo->cPortType[1] = PORTTYPE_04(xCell, yCell);
  	lpCellInfo->cPortType[2] = PORTTYPE_06(xCell, yCell);
	lpCellInfo->cPortType[3] = PORTTYPE_08(xCell, yCell);
	lpCellInfo->cPortType[4] = PORTTYPE_10(xCell, yCell);
	lpCellInfo->cPortType[5] = PORTTYPE_12(xCell, yCell);
	lpCellInfo->cOppoType[0] = OPPOTYPE_02(xCell, yCell);
	lpCellInfo->cOppoType[1] = OPPOTYPE_04(xCell, yCell);
	lpCellInfo->cOppoType[2] = OPPOTYPE_06(xCell, yCell);
	lpCellInfo->cOppoType[3] = OPPOTYPE_08(xCell, yCell);
	lpCellInfo->cOppoType[4] = OPPOTYPE_10(xCell, yCell);
	lpCellInfo->cOppoType[5] = OPPOTYPE_12(xCell, yCell);
	return( YES );
}

//*********************************************************************************************
BOOL CWorld::GetCellData( CCell * pCell, LPCELLINFO lpCellInfo )
//*********************************************************************************************
{
	if ( !pCell )
	{
		if ( !m_pPlayer )
			pCell = NULL;
		else
			pCell = m_pPlayer->GetCell();
	}

	lpCellInfo->cCellType    = (pCell ? pCell->Type()        : NULL);
	lpCellInfo->cLookType    = (pCell ? pCell->Look()        : NULL);
	lpCellInfo->cPortType[0] = (pCell ? pCell->PortalType(0) : NULL);
	lpCellInfo->cPortType[1] = (pCell ? pCell->PortalType(1) : NULL);
  	lpCellInfo->cPortType[2] = (pCell ? pCell->PortalType(2) : NULL);
	lpCellInfo->cPortType[3] = (pCell ? pCell->PortalType(3) : NULL);
	lpCellInfo->cPortType[4] = (pCell ? pCell->PortalType(4) : NULL);
	lpCellInfo->cPortType[5] = (pCell ? pCell->PortalType(5) : NULL);

	lpCellInfo->cOppoType[0] = (pCell ? pCell->OppType(0) : NULL);
	lpCellInfo->cOppoType[1] = (pCell ? pCell->OppType(1) : NULL);;
	lpCellInfo->cOppoType[2] = (pCell ? pCell->OppType(2) : NULL);;
	lpCellInfo->cOppoType[3] = (pCell ? pCell->OppType(3) : NULL);;
	lpCellInfo->cOppoType[4] = (pCell ? pCell->OppType(4) : NULL);;
	lpCellInfo->cOppoType[5] = (pCell ? pCell->OppType(5) : NULL);;
	return( YES );
}

//***********************************************************************
BOOL CWorld::LoadLevel( ifstream& in, int iLevel )
//***********************************************************************
{
	HINSTANCE hInstance = GetWindowInstance( hWndDraw ); // GetApp()->GetInstance();
	if ( !hInstance )
		return( NO );

	in.seekg(0, ios::end);
	long lSize = in.tellg();
	in.seekg(0, ios::beg);

	LPSTR lpMapData = (LPSTR)GlobalAllocPtr(GHND, lSize);
	if (!lpMapData)
		return FALSE;

	// Get header info
	POINT pt;
	pt.x = 0;
	pt.y = 0;

	char c;

	in >> c;
	in >> xCellUnits;
	in >> c;
	in.eatwhite();
	in >> yCellUnits;
	in >> c;
	in.eatwhite();
	in >> pt.x;
	in >> c;
	in.eatwhite();
	in >> pt.y;

	long lLength = 0;
	long lCount = 0;
	LPSTR lpDest = lpMapData;
	while( !in.eof() )
	{
		c = in.get();
		switch(c)
		{
		case '{':
		case '\n':
		case '\"':
		case ',':
		case '}':
			break;
		default:
			*(lpDest++) = c;
			++lLength;
		}

		++lCount;

		// safety
		if (lCount >= lSize)
			break;
	}

	// Delete any existing map and opponent list
	if ( lpMap )
		GlobalFreePtr( lpMap );
	lpMap = NULL;

	// Delete any opponents currently in the group
	m_pPlayer->SetCell(NULL);
	m_pGroup->DeleteOpps();
	m_pGroup->DeleteGenerators();

	// Process the map data
	//m_pPlayer->SetInGate();
	CenterPlayerView( NO/*bScroll*/ );

	if ( lpMap = (LPSTR)GlobalAllocPtr(GHND, lLength ) )
		hmemcpy( lpMap, lpMapData, lLength );

	GlobalFreePtr(lpMapData);

	// Delete any previous cells
	if (m_pCells)
	{
		Debug("Delete %d cells\n", m_nCellCount);
		for(int i=0; i<m_nCellCount; i++)
			delete (CCell *)*(m_pCells + i);
		GlobalFreePtr(m_pCells);
		m_pCells = NULL;
	}

	// Calculate width and height of the cell map
	m_nHeight = yCellUnits;
	m_nWidth = xCellUnits;
	MapCellUnitToCell( &m_nWidth, &m_nHeight );
	// Adjust width and height by 1 (because of 0)
	++m_nWidth;
	++m_nHeight;

	// Create a new set of cells
{
	int x=-1;
	int y=-1;

	int xTemp;
	int yTemp;

	// Allocate initial memory
	m_pCells = (CCell **)GlobalAllocPtr(GHND, sizeof(CCell *) * xCellUnits * yCellUnits);

	// Build map
	for (int j=0; j<yCellUnits; j++)
	{
		yTemp = j;
		xTemp = 0;

		MapCellUnitToCell( &xTemp, &yTemp );

		if ( y != yTemp )
		{
			y = yTemp;

			for(int i=0; i<xCellUnits; i++)
			{
				xTemp = i;

				MapCellUnitToCell( &xTemp, &yTemp );

				// If new cell
				if (x != xTemp)
				{
					x = xTemp;

					CELLINFO CellInfo;
					GetCellData(x, y, &CellInfo);
					CCell *pCell = new CCell(x, y, &CellInfo);

					if (!pCell)
						DebugBreak();
					*(m_pCells + m_nCellCount) = pCell;

					++m_nCellCount;
				}
			}
		}
	}

	m_pCells = (CCell **)GlobalReAllocPtr(m_pCells, sizeof(CCell *) *
		m_nCellCount, 0);
	Debug("Realloced for  %d cells\n", m_nCellCount);
}

	MapCellUnitToCell((LPINT)&pt.x, (LPINT)&pt.y);
	m_pPlayer->SetXYCell(pt.x, pt.y);
	m_pPlayer->ChangeDirAbs(12);

	// Turn off cells that should be off
	AllCellsOff(iLevel);

	// Find all of the opponents in the cell map and add them to the opponent list
	for ( int y=0; y<yCellUnits; y++ )
	{
		for ( int x=0; x<xCellUnits; x++ )
		{
			CellUnitSetIfCulDeSac( x, y );

			if ( CellIsGenerator(x, y) )
			{				
				int xNew = x;
				int yNew = y;
				MapCellUnitToCell(&xNew, &yNew);

				CCell *pCell = GetCell(xNew, yNew);
				m_pGroup->CreateGenerator(pCell, 10000);
				continue;
			}

			int iType, iDir;
			if ( !CellUnitIsOpponent( x, y, &iType, &iDir ) )
				continue;
				
			// Create opponent
			int xNew = x;
			int yNew = y;
			MapCellUnitToCell(&xNew, &yNew);

			m_pGroup->CreateOpp(iType, xNew, yNew);
		}
	}


	iCurrentLevel = iLevel;

	return( YES );
}


//***********************************************************************
int CWorld::HitOpponent(int iIndex, int iDamage)
//***********************************************************************
{
	int iRet = m_pGroup->HitEngaged(iIndex, iDamage);
	if (iRet > 0)
	{
		lScore += iRet;
		Debug("Score = %ld\n", lScore);
	}

	return iRet;
}

////////////////////////
#include "offcells.h" // Defines the hard coded cell points 
////////////////////////

//***********************************************************************
void CWorld::AllCellsOff(int iLevel)
//***********************************************************************
{
	switch(iLevel)
	{
		case 2:
			CellsOff(g_pL2N1Cells);
			CellsOff(g_pL2N2Cells);
			CellsOff(g_pL2N3Cells);
			CellsOff(g_pL2N4Cells);
			break;
	}
}

//***********************************************************************
void CWorld::SwitchCellsOn(int iLevel, int iNum)
//***********************************************************************
{	//
	// Turns on cells
	//

	POINT *pPoints = NULL;

	switch(iLevel)
	{
		case 2:
			if (iNum == 1)
				pPoints = g_pL2N1Cells;
			else if (iNum == 2)
				pPoints = g_pL2N2Cells;
			else if (iNum == 3)
				pPoints = g_pL2N3Cells;
			else if (iNum == 4)
				pPoints = g_pL2N4Cells;
			break;
	}

	if (pPoints)
	{
		while(pPoints->x != -1)
		{
			CCell *pCell = GetCell(pPoints->x, pPoints->y);
			if (pCell)
			{
				pCell->SetOn();
				pCell->SetVisible(TRUE);
			}
			pPoints++;
		}
	}
}

//***********************************************************************
void CWorld::CellsOff(POINT *pPoints)
//***********************************************************************
{
	if (pPoints)
	{
		while(pPoints->x != -1)
		{
			CCell *pCell = GetCell(pPoints->x, pPoints->y);
			if (pCell)
				pCell->SetOff();
			++pPoints;
		}
	}
}
