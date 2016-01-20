#include <windows.h>
#include "proto.h"
#include "cllsn.h"
#include "worlds.h"		
#include "Cells.h"
#include "control.h"

static BOOL bFogOfWar = YES;

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

// The CONSTRUCTOR
//************************************************************************
CWorld::CWorld()
//************************************************************************
{
	m_pCells = NULL;
	m_nHeight = -1;
	m_nWidth = -1;

	m_pGroup = NULL;
	m_pPlayer = NULL;

	m_bVirus = NO;

	hWndDraw = NULL;
	xCellUnits = yCellUnits = 0;
	ptViewLocation.x = 0;
	ptViewLocation.y = 0;
	ptViewLast = ptViewLocation;
	SetZoomFactor( 1 );
	hPenG	= CreatePen( PS_SOLID, 1, RGB(  0,255,  0) );
	hPenR	= CreatePen( PS_SOLID, 1, RGB(255,  0,  0) );
	hPenY	= CreatePen( PS_SOLID, 1, RGB(255,255,  0) );
	hPenW	= CreatePen( PS_SOLID, 1, RGB(255,255,255) );
	hPenM	= CreatePen( PS_SOLID, 1, RGB(255,  0,255) );
	hPenC	= CreatePen( PS_SOLID, 1, RGB(  0,255,255) );
	hPenB	= CreatePen( PS_SOLID, 1, RGB(  0,  0,255) );

	hPenCar[0]  = CreatePen( PS_SOLID, 1, RGB(255, 255, 255)); // grunt - white
	hPenCar[1]  = CreatePen( PS_SOLID, 1, RGB(  0, 255, 255)); // detonator	- cyan
	hPenCar[2]  = CreatePen( PS_SOLID, 1, RGB(  0, 255,   0)); // con drone	- green
	hPenCar[3]  = CreatePen( PS_SOLID, 1, RGB(  0,   0, 255)); // nightron - blue
	hPenCar[4]  = CreatePen( PS_SOLID, 1, RGB(255,   0,   0)); // hellion - red
	hPenCar[5]  = CreatePen( PS_SOLID, 1, RGB(  0, 255,   0)); // flea - green
	hPenCar[6]  = CreatePen( PS_SOLID, 1, RGB(255, 255,   0)); // parhelion	- yellow
	hPenCar[7]  = CreatePen( PS_SOLID, 1, RGB(255,   0, 255)); // thwart - purple
	hPenCar[8]  = CreatePen( PS_SOLID, 1, RGB(255, 255,   0)); // flasher - yellow 
	hPenCar[9]  = CreatePen( PS_SOLID, 1, RGB(  0,   0, 255)); // dogger - blue
	hPenCar[10] = CreatePen( PS_SOLID, 1, RGB(  0, 128,  0));  // electroshark - dk green   
	hPenCar[11] = CreatePen( PS_SOLID, 1, RGB(255, 255, 255)); // medicator - white
	hPenCar[12] = CreatePen( PS_SOLID, 1, RGB(  0, 128, 128)); // viralion - dk cyan
	hPenCar[13] = CreatePen( PS_SOLID, 1, RGB(128, 128, 128)); // drainiac - grey
	hPenCar[14] = CreatePen( PS_SOLID, 1, RGB(128,   0,   0)); // killer - dk red         
	hPenCar[15] = CreatePen( PS_SOLID, 1, RGB(255,   0,   0)); // reindeer - red
	hPenCar[16] = CreatePen( PS_SOLID, 1, RGB(255,   0,   0)); // rudolph - red   
	hPenCar[17] = CreatePen( PS_SOLID, 1, RGB(128,   0, 128)); // slamatron - dk purple   
	hPenCar[18] = CreatePen( PS_SOLID, 1, RGB(  0,   0, 128)); // dreadnought - dk blue
	hPenCar[19] = CreatePen( PS_SOLID, 1, RGB(  0, 128, 128)); // psychotech - dk cyan
	hPenCar[20] = CreatePen( PS_SOLID, 1, RGB(128, 128,   0)); // painulator - dk yellow 
	hPenCar[21] = CreatePen( PS_SOLID, 1, RGB(128,   0, 128)); // paragon - dk purlple
	hPenCar[22] = CreatePen( PS_SOLID, 1, RGB(255, 255, 255)); // spine immortacon - white
	hPenCar[23] = CreatePen( PS_SOLID, 1, RGB(255, 255, 255)); // spine - white

	// Load the radar DIB's
	int iMax = sizeof(pDib)/sizeof(PDIB);
	for ( int i = 0; i < iMax; i++ )
	{
		ITEMID id = IDC_RADAR_FIRST + 1 + i;
		if ( id < (ITEMID)IDC_RADAR_LAST )
			pDib[i] = CDib::LoadDibFromResource( App.GetInstance(),
						MAKEINTRESOURCE(id), App.m_hPal);
		else
			pDib[i] = NULL;
	}

	lScore = 0L;
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
		int nCellCount = m_nHeight * m_nWidth;
		for(int i=0; i<nCellCount; i++)
		{
			CCell * pCell = (CCell *)*(m_pCells + i);
			if (pCell)
				delete pCell;
		}

		FreeUp(m_pCells);
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

	int i;
	for(i=0; i<MAX_CLASS; i++)
		DeletePen( hPenCar[i] );

	// Delete the radar DIB's
	int iMax = sizeof(pDib)/sizeof(PDIB);
	for (i = 0; i < iMax; i++ )
	{
		if ( pDib[i] )
			delete pDib[i];
	}
}

//***********************************************************************
BOOL CWorld::Init( HWND hWnd )
//***********************************************************************
{
	if ( hWnd ) 
		hWndDraw = hWnd;
	
	m_bVirus = NO;

	// Create opponent group object
	m_pGroup = new COpponentGroup();
	// Create player object
	m_pPlayer = new CPlayer();

	return( App.pGame->LoadLevelFile(hWnd, "level01.txt", 1) );
}

//***********************************************************************
BOOL CWorld::LoadLevel( ifstream& in, int iLevel )
//***********************************************************************
{
	HINSTANCE hInstance = GetWindowInstance( hWndDraw ); // GetApp()->GetInstance();
	if ( !hInstance )
		return( NO );

	// Delete any existing cells
	if (m_pCells)
	{
		int nCellCount = m_nHeight * m_nWidth;
		for(int i=0; i<nCellCount; i++)
		{
			CCell * pCell = (CCell *)*(m_pCells + i);
			if (pCell)
				delete pCell;
		}

		FreeUp(m_pCells);
		m_pCells = NULL;
	}

	// Delete any existing opponents
	m_pPlayer->SetCell(NULL);
	m_pGroup->DeleteOpps();
	m_pGroup->DeleteGenerators();

	// Load this level's opponent records
	char szName[80];
	wsprintf(szName, "oppl%d.dat", iLevel);
	m_pGroup->LoadRecs(szName);

	// Start reading the level map file
	in.seekg(0, ios::end);
	long lSize = in.tellg();
	in.seekg(0, ios::beg);

	LPSTR lpMapData = (LPSTR)Alloc(lSize);
	if (!lpMapData)
		return FALSE;

	// Get header info
	POINT ptPlayer;

	char c;
	in >> c;
	in >> xCellUnits;
	in >> c;
	in.eatwhite();
	in >> yCellUnits;
	in >> c;
	in.eatwhite();
	in >> ptPlayer.x;
	in >> c;
	in.eatwhite();
	in >> ptPlayer.y;

	MapCellUnitToCell((LPINT)&ptPlayer.x, (LPINT)&ptPlayer.y);
	int iPlayerDir = 12;

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

	// Calculate width and height of the cell map
	m_nHeight = yCellUnits;
	m_nWidth = xCellUnits;
	MapCellUnitToCell( &m_nWidth, &m_nHeight );
	// Adjust width and height by 1 (because of 0)
	++m_nWidth;
	++m_nHeight;

	// Allocate memory for the cell list
	m_pCells = (CCell **)Alloc(sizeof(CCell *) * m_nWidth * m_nHeight);

	// Build the cell map
	if ( m_pCells )
	{
		int nCellCount = 0;
		for ( int y=0; y<m_nHeight; y++ )
		{
			for ( int x=0; x<m_nWidth; x++ )
			{
				CCell *pCell = new CCell;
				*(m_pCells + nCellCount) = pCell; // even if pCell is NULL
				++nCellCount;

				if ( !pCell )
					continue;

				// Stuff the cell member data from the ascii map data
				StuffCellData(pCell, lpMapData, x, y);

				// Look to see if we have to create an opponent generator
				if (pCell->Type() == 'O')
				{
					// See if a first and last opponent has been specified
					// for the opponent generator. This is done in the editor
					// by specifying an opponent in the first combo (10 o'clock 
					// position) and the second combo box (12 o'clock). The two
					// opponents specify a range, so the first one must be smaller
					// then the second one.
					int iFirst = pCell->OppType(4) - '0';
					int iLast  = pCell->OppType(5) - '0';
					if ((iFirst == 0 && iLast == 0) || (iFirst > iLast))
					{
						iFirst = -1;
						iLast = -1;
					}
					m_pGroup->CreateGenerator(pCell, 10000, iFirst, iLast);
				}

				else
				{
					// Loop through all of the portals
					for ( int p=0; p<MAX_GATE; p++ )
					{
						char c;
						// Look to see if we have to create an opponent
						if (c = pCell->OppType(p))
							m_pGroup->CreateOpp(c - '0', x, y);
	//j					#ifndef _DEBUG
						// Look to see if we have to set the player position
						c = pCell->PortalType(p);
						if (c == 'N' && pCell->Type() == 'C')
						{ // it's an entry cul-de-sac
							ptPlayer.x = x;
							ptPlayer.y = y;
							iPlayerDir = (p+1) * 2;
						}
	//j					#endif
					}
				}
			}
		}
	}

	FreeUp(lpMapData);
	lpMapData = NULL;

	// Set up the player
	m_pPlayer->SetXYCell(ptPlayer.x, ptPlayer.y);
	m_pPlayer->ChangeDirAbs(iPlayerDir);
	m_pPlayer->ChangeDirRel(6);

	// Compute where the bounds of the level are,
	// and make them cul-de-sac's
	SetBoundaries();

	CenterPlayerView( NO/*bScroll*/ );

	// Turn off cells that should be off
	AllCellsOff(iLevel);

	iCurrentLevel = iLevel;

	return( YES );
}

//*********************************************************************************************
void CWorld::SetBoundaries(void)
//*********************************************************************************************
{
	// Loop through all the cells; if any cell has a NULL type,
	// but has a neighbor, make it a "cul-de-sac" type cell

	if ( !m_pCells || !GetPlayer() )
		return;

	for ( int y=0; y<m_nHeight; y++ )
	{
		for ( int x=0; x<m_nWidth; x++ )
		{
			CCell* pCell = GetCell(x, y);
			if (!pCell || pCell->Type())
				continue;
			// See if any of the cell's neighbors exist
			for( int i=2; i<=12; i+=2 )
			{
				POINT pt = GetPlayer()->NextCellFromDir(x, y, i);
				CCell* pCellNeighbor = GetCell(pt.x, pt.y);
				if (!pCellNeighbor)
					continue;
				char cType = pCellNeighbor->Type();
				if (!cType || cType == 'C')
					continue;
				pCell->SetType('C');
				break;
			}
		}
	}
}

//*********************************************************************************************
char CWorld::GetCellUnitCode( LPSTR lpMap, int x, int y )
//*********************************************************************************************
{		
	if ( x < 0 || x >= xCellUnits )	return( NULL );
	if ( y < 0 || y >= yCellUnits )	return( NULL );

	char c = *(lpMap + ((x) + ((y) * xCellUnits)));
	if ( c == ' ' ) c = NULL;
	return( c );
}

//*********************************************************************************************
void CWorld::StuffCellData( CCell* pCell, LPSTR lpMap, int x, int y )
//*********************************************************************************************
{
	if (!pCell)
		return;

	pCell->Setx(x);
	pCell->Sety(y);

	MapCellToCellUnit(&x, &y);
	MapCellUnitToCellCenter(&x, &y);

	pCell->SetType(          GetCellUnitCode(lpMap, x-1, y+0 ) );
	pCell->SetLook(          GetCellUnitCode(lpMap, x+1, y+0 ) );
	pCell->SetPortalType( 0, GetCellUnitCode(lpMap, x+2, y-1 ) );
	pCell->SetPortalType( 1, GetCellUnitCode(lpMap, x+2, y+1 ) );
  	pCell->SetPortalType( 2, GetCellUnitCode(lpMap, x+0, y+2 ) );
	pCell->SetPortalType( 3, GetCellUnitCode(lpMap, x-2, y+1 ) );
	pCell->SetPortalType( 4, GetCellUnitCode(lpMap, x-2, y-1 ) );
	pCell->SetPortalType( 5, GetCellUnitCode(lpMap, x+0, y-2 ) );
	pCell->SetOppType(    0, GetCellUnitCode(lpMap, x+1, y-1 ) );
	pCell->SetOppType(    1, GetCellUnitCode(lpMap, x+1, y+1 ) );
	pCell->SetOppType(    2, GetCellUnitCode(lpMap, x+0, y+1 ) );
	pCell->SetOppType(    3, GetCellUnitCode(lpMap, x-1, y+1 ) );
	pCell->SetOppType(    4, GetCellUnitCode(lpMap, x-1, y-1 ) );
	pCell->SetOppType(    5, GetCellUnitCode(lpMap, x+0, y-1 ) );

	// Look to see if any portals contain a collectible
	if ( !pCell->Type() )
	{
		for ( int p=0; p<6; p++ )
		{
			char c = pCell->PortalType(p);
			if ( IsValidCulDeSac(c) )
				pCell->SetType('C'); // make the cell a "cul-de-sac"
		}
	}
}

//*********************************************************************************************
int CWorld::GetLevel( void )
//*********************************************************************************************
{
	return(iCurrentLevel);
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

//*********************************************************************************************
BOOL CWorld::IsValidCulDeSac( unsigned char c )
//*********************************************************************************************
{
	return( c == 'M' || c == 'm' || c == 'N' || c == 'n' || c == 'X' || c == 'x' ||
		    c == 'R' || c == 'r' || c == 'A' || c == 'a' || c == 'S' || c == 's' ||
			c == 'h' || c == 'i' || c == 'j' || c == 'k' || c == 'l' || c == 'm' ||
			c == 'n' ||
		   (c >= '1' && c <= '9') || ( c >= PORT_START && c <= PORT_END));
}

//***********************************************************************
void CWorld::CenterPlayerView( BOOL bScroll )
//***********************************************************************
{
	if (m_pPlayer && m_pPlayer->GetCell())
		CenterView( m_pPlayer->GetCell(), bScroll );
}

//***********************************************************************
void CWorld::CenterView( CCell *pCell, BOOL bScroll )
//***********************************************************************
{
	if (!pCell)
		return;

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

	#define MIN_DELTAX	36 // width - 25%
	#define MIN_WIDTH	48
	#define MIN_HEIGHT	42

	// Compute how big a single cell is on the display
	xDispDelta  = ScaleInt( MIN_DELTAX, iZoomFactor );
	iDispWidth  = ScaleInt( MIN_WIDTH,  iZoomFactor );
	iDispHeight = ScaleInt( MIN_HEIGHT, iZoomFactor );

	CenterPlayerView( NO/*bScroll*/ );
}

//***********************************************************************
int CWorld::GetZoomFactor( int iIncrement )
//***********************************************************************
{
	if ( !iIncrement )
		return( iZoomFactor );

	int iZoom = iZoomFactor;

	while ( iIncrement )
	{
		if ( iIncrement < 0 )
		{
			iIncrement++;
			if ( iZoom == -6 )
				iZoom = 3;
			else
			{
				iZoom--;
				if ( iZoom == 0 )
					iZoom -= 2;
			}
		}
		else
		{
			iIncrement--;
			if ( iZoom == 3 )
				iZoom = -6;
			else
			{
				iZoom++;
				if ( iZoom == -1 )
					iZoom += 2;
			}
		}
	}

	return( iZoom );
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
	}

	DrawOpponents( hDC, YES );
	DrawPlayer( hDC, YES );
		
	ReleaseDC( hWndDraw, hDC );
	ptViewLast = ptViewLocation;
}

/***********************************************************************/
void CWorld::Draw( HDC hDC, LPRECT lpRect )
/***********************************************************************/
{
	if (!lpRect)
		return;

	RECT Rect;
	GetClientRect(hWndDraw, &Rect );

	if (RectWidth(&Rect) >= 640 && RectHeight(&Rect) >= 480)
		ClearOffScreen( lpRect );
	else
		RepairOffScreen( lpRect );
	DrawOffScreen( lpRect );
	DrawOnScreen( hDC, lpRect );
}

/***********************************************************************/
void CWorld::ClearOffScreen( LPRECT lpRect )
/***********************************************************************/
{
	if (!lpRect)
		return;

	HWND hWindow = hWndDraw;
	HWND hParent = GetParent(hWindow);

	LPSCENE lpScene = CScene::GetScene(hParent);
	if (!lpScene)
		return;

	LPOFFSCREEN lpOffScreen = lpScene->GetOffScreen();
	if (!lpOffScreen)
		return;

	RECT rect = *lpRect;
	MapWindowPoints( hWindow, hParent, (LPPOINT)&rect, 2 );

	HDC hDC = lpOffScreen->GetDC();
	if (!hDC)
		return;

	FillRect( hDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH) );
}

/***********************************************************************/
void CWorld::RepairOffScreen( LPRECT lpRect )
/***********************************************************************/
{
	if (!lpRect)
		return;

	HWND hWindow = hWndDraw;
	HWND hParent = GetParent(hWindow);

	LPSCENE lpScene = CScene::GetScene(hParent);
	if (!lpScene)
		return;

	LPOFFSCREEN lpOffScreen = lpScene->GetOffScreen();
	if (!lpOffScreen)
		return;

	RECT rect = *lpRect;
	MapWindowPoints( hWindow, hParent, (LPPOINT)&rect, 2 );

	lpOffScreen->CopyBits( &rect );
}

/***********************************************************************/
void CWorld::DrawOnScreen( HDC hDC, LPRECT lpRect )
/***********************************************************************/
{
	if (!hDC || !lpRect)
		return;

	HWND hWindow = hWndDraw;
	HWND hParent = GetParent(hWindow);

	LPSCENE lpScene = CScene::GetScene(hParent);
	if (!lpScene)
		return;

	LPOFFSCREEN lpOffScreen = lpScene->GetOffScreen();
	if (!lpOffScreen)
		return;

	RECT rect = *lpRect;
	MapWindowPoints( hWindow, hParent, (LPPOINT)&rect, 2 );

	// Blt the offscreen into the DC
	POINT ptDst;
	ptDst.x = lpRect->left;
	ptDst.y = lpRect->top;

	lpOffScreen->DrawRect( hDC, &rect, &ptDst );
}

/***********************************************************************/
void CWorld::DrawOffScreen( LPRECT lpRect )
/***********************************************************************/
{
	if (!lpRect)
		return;

	HWND hWindow = hWndDraw;
	HWND hParent = GetParent(hWindow);

	LPSCENE lpScene = CScene::GetScene(hParent);
	if (!lpScene)
		return;

	LPOFFSCREEN lpOffScreen = lpScene->GetOffScreen();
	if (!lpOffScreen)
		return;

	HDC hDC = lpOffScreen->GetDC();
	PDIB pDibDst = lpOffScreen->GetWritableDIB();

	RECT rect = *lpRect;

	int x1 = rect.left;
	int y1 = rect.top;
	MapRadarToView( &x1, &y1 );
	MapViewToCell( &x1, &y1 );

	int x2 = rect.right;
	int y2 = rect.bottom;
	MapRadarToView( &x2, &y2 );
	MapViewToCell( &x2, &y2 );

	// Be sure the Offscreen DC is clipped to the radar's rectangle
	MapWindowPoints( hWindow, hParent, (LPPOINT)&rect, 2 );
	//IntersectClipRect( hDC, rect.left, rect.top, rect.right, rect.bottom );
	RECT rSaved;
	GetClipBox(hDC, &rSaved);
	HRGN hRegion = CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
	if (hRegion)
	{
		SelectClipRgn(hDC, hRegion);
		DeleteObject(hRegion);
	}

	// pull in the hanging cells
	if ( x1 & 1 ) y2++; else y1--;
	x1--; x2++;

	// Loop on all of the cells in the draw rectangle, and paint them
	HPEN hOldPen = SelectPen( hDC, hPenG );
	POINT ptCell;
	for ( ptCell.y = y1; ptCell.y <= y2; ptCell.y++ )
	{
		if ( ptCell.y < 0 || ptCell.y >= m_nHeight )
			continue;
		
		for ( ptCell.x = x1; ptCell.x <= x2; ptCell.x++ )
		{
			if ( ptCell.x < 0 || ptCell.x >= m_nWidth )
				continue;

			CCell *pCell = GetCell(ptCell.x, ptCell.y);
			if (pCell && (pCell->IsVisible() || !bFogOfWar))
				DrawCell( hDC, pDibDst, ptCell );
		}
	}
	
	SelectPen( hDC, hOldPen ); 

	// Reset the DC clip rect
	//IntersectClipRect( hDC, rect.left, rect.top, rect.right, rect.bottom );
	hRegion = CreateRectRgn(rSaved.left, rSaved.top, rSaved.right, rSaved.bottom);
	if (hRegion)
	{
		//SetRectRgn(hRegion, rSaved.left, rSaved.top, rSaved.right, rSaved.bottom);
		SelectClipRgn(hDC, hRegion);
		DeleteObject(hRegion);
	}

	// For the time being, get a regular DC to draw these into
	hDC = GetDC( hWindow );
	if ( hDC )
	{
		HPEN hOldPen = SelectPen( hDC, hPenG );
		//HPALETTE hOldPal = SelectPalette( hDC, App.m_hPal, FALSE);
		//UnrealizeObject(App.m_hPal);
		//RealizePalette( hDC );
		DrawOpponents( hDC, YES );
		DrawPlayer( hDC, YES );	
		SelectPen( hDC, hOldPen ); 
		//SelectPalette( hDC, hOldPal, FALSE);
		ReleaseDC( hWindow, hDC );
	}
}

/***********************************************************************/
void CWorld::DrawDib( PDIB pDibSrc, HDC hDC, PDIB pDibDst, int x, int y )
/***********************************************************************/
{
	if (!pDibSrc)
		return;

	RECT rSrc;
	SetRect( &rSrc, 0, 0, pDibSrc->GetWidth(),
						  pDibSrc->GetHeight() );

	RECT rDst;
	SetRect( &rDst, x, y, x + ScaleInt(pDibSrc->GetWidth(), iZoomFactor),
						  y + ScaleInt(pDibSrc->GetHeight(), iZoomFactor) );

	RECT rDstClip;
	GetClientRect( hWndDraw, &rDstClip );
	MapWindowPoints( hWndDraw, GetParent(hWndDraw), (LPPOINT)&rDstClip, 2 );

	if ( !pDibSrc->DibClipRect( pDibDst, &rDst, &rDstClip,
								pDibSrc, &rSrc, NULL/*prSrcClip*/ ) )
		return;

	// Blt it into the offscreen
	if ( !pDibDst || pDibDst->GetCompression() == BI_RLE8 )
	{ // must use GDI blt into compressed destination DIBs (lose transparency ability)
		pDibSrc->DCBlt( hDC,
			rDst.left, rDst.top, rDst.right - rDst.left, rDst.bottom - rDst.top,
			rSrc.left, rSrc.top, rSrc.right - rSrc.left, rSrc.bottom - rSrc.top );
	}
	else
	{
		pDibSrc->DibBlt( pDibDst,
			rDst.left, rDst.top, rDst.right - rDst.left, rDst.bottom - rDst.top,
			rSrc.left, rSrc.top, rSrc.right - rSrc.left, rSrc.bottom - rSrc.top,
			YES/*bTransparent*/ );
	}
}

/***********************************************************************/
void CWorld::DrawCell( HDC hDC, PDIB pDibDst, POINT ptCell )
/***********************************************************************/
{
	if (m_bVirus) // Onboard system virus
		return;

	int x = ptCell.x;
	int y = ptCell.y;
	MapCellToCellUnit( &x, &y ); // the center cell unit
	MapCellUnitToRadar( &x, &y );

	// offset by half a cell
	x -= (iDispWidth/2);
	y -= (iDispHeight/2);

	// Now make the (x,y) draw coordinates relative to the offscreen
	POINT pt;
	pt.x = x;
	pt.y = y;
	MapWindowPoints( hWndDraw, GetParent(hWndDraw), &pt, 1 );
	x = pt.x;
	y = pt.y;

	CCell *pCell = GetCell(ptCell.x, ptCell.y);
	if (!pCell)
		return;

	ITEMID id = IDC_RADAR_FIRST + 1;
	int iLook = pCell->Look() - '0';

	char c = pCell->Type();

	switch ( c )
	{		
		case 'H':
		{				 
			DrawDib( pDib[IDC_RADAR_HEX-id], hDC, pDibDst, x, y );
			break;
		}
		case 'U':
		{						 
			DrawDib( pDib[IDC_RADAR_UNJ-id], hDC, pDibDst, x, y );
			break;
		}
		case '|':
		{
			DrawDib( pDib[IDC_RADAR_VERTTUNNEL-id], hDC, pDibDst, x, y );
			break;
		}
		case '`':
		{
			DrawDib( pDib[IDC_RADAR_10TO4TUNNEL-id], hDC, pDibDst, x, y );
			break;
		}
		case '/':
		{		
			DrawDib( pDib[IDC_RADAR_2TO8TUNNEL-id], hDC, pDibDst, x, y );
			break;
		}
		case 'B':
		{								 
			DrawDib( pDib[IDC_RADAR_BLACKHOLE-id], hDC, pDibDst, x, y );
			break;
		}
		case 'S':
		{								 
			DrawDib( pDib[IDC_RADAR_SAFECELL-id], hDC, pDibDst, x, y );
			break;
		}
		case 'R':
		{								 
			DrawDib( pDib[IDC_RADAR_RADIATION-id], hDC, pDibDst, x, y );
			break;
		}
		case 'O':
		{
			DrawDib( pDib[IDC_RADAR_OPPGENERATOR-id], hDC, pDibDst, x, y );
			break;
		}
		case 'C':
		{
			DrawDib( pDib[IDC_RADAR_CUL-id], hDC, pDibDst, x, y );
			DrawCulDeSacs( hDC, pDibDst, x, y, pCell );
			break;
		}
	}

	DrawPortals( hDC, pDibDst, x, y, pCell );
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

		pnt.x += Cellpnt.x;
		pnt.y += Cellpnt.y;

		iDir = m_pPlayer->GetDir();
	}

	POINT pt[3];
	if ( iDir == 1 )
	{
		pt[0].x =  3;	pt[0].y = -6;
		pt[1].x = -6;	pt[1].y =  3;
		pt[2].x =  3;	pt[2].y =  6;
	}
	else
	if ( iDir == 2 )
	{
		pt[0].x =  6;	pt[0].y = -3;
		pt[1].x = -3;	pt[1].y =  6;
		pt[2].x = -6;	pt[2].y = -3;
	}
	else
	if ( iDir == 3 )
	{
		pt[0].x =  6;	pt[0].y =  0;
		pt[1].x = -6;	pt[1].y =  6;
		pt[2].x = -6;	pt[2].y = -6;
	}
	else
	if ( iDir == 4 )
	{
		pt[0].x =  6;	pt[0].y =  3;
		pt[1].x = -6;	pt[1].y =  3;
		pt[2].x = -3;	pt[2].y = -6;
	}
	else
	if ( iDir == 5 )
	{
		pt[0].x =  3;	pt[0].y =  6;
		pt[1].x =  3;	pt[1].y = -6;
		pt[2].x = -6;	pt[2].y = -3;
	}
	else
	if ( iDir == 6 )
	{
		pt[0].x =  0;	pt[0].y =  6;
		pt[1].x = -3;	pt[1].y = -6;
		pt[2].x =  3;	pt[2].y = -6;
	}
	else
	if ( iDir == 7 )
	{
		pt[0].x = -3;	pt[0].y =  6;
		pt[1].x =  6;	pt[1].y = -3;
		pt[2].x = -3;	pt[2].y = -6;
	}
	else
	if ( iDir == 8 )
	{
		pt[0].x = -6;	pt[0].y =  3;
		pt[1].x =  3;	pt[1].y = -6;
		pt[2].x =  6;	pt[2].y =  3;
	}
	else
	if ( iDir == 9 )
	{
		pt[0].x = -6;	pt[0].y =  0;
		pt[1].x =  6;	pt[1].y = -6;
		pt[2].x =  6;	pt[2].y =  6;
	}
	else
	if ( iDir == 10 )
	{
		pt[0].x = -6;	pt[0].y = -3;
		pt[1].x =  6;	pt[1].y = -3;
		pt[2].x =  3;	pt[2].y =  6;
	}
	else
	if ( iDir == 11 )
	{
		pt[0].x = -3;	pt[0].y = -6;
		pt[1].x = -3;	pt[1].y =  6;
		pt[2].x =  6;	pt[2].y =  3;
	}
	else
	if ( iDir == 12 )
	{
		pt[0].x = 0;	pt[0].y = -6;
		pt[1].x =  3;	pt[1].y =  6;
		pt[2].x = -3;	pt[2].y =  6;
	}                             
	else
		return;

	int iZoom = iZoomFactor;
	if ( iZoom < 1 ) iZoom = 1;
	int iCount = sizeof(pt) / sizeof(POINT);
	for ( int i = 0; i < iCount; i++ )
	{
		pt[i].x = pnt.x + ScaleInt( pt[i].x, iZoom );
		pt[i].y = pnt.y + ScaleInt( pt[i].y, iZoom );
	}

	HPEN hPen;
	HBRUSH hBrush;
	if ( bOn )
	{
		hBrush = CreateSolidBrush( RGB(255,0,0) );
		hPen = hPenR;
	}
	else
	{
		hBrush = GetStockBrush(BLACK_BRUSH);
		hPen = GetStockPen(BLACK_PEN);
	}

	DrawPolygon( hDC, pt, iCount, hPen, hBrush );

	if ( bOn && hBrush )
		DeleteBrush( hBrush );
}

/***********************************************************************/
void CWorld::DrawOpponents( HDC hDC, BOOL bOn )
/***********************************************************************/
{				
	if (m_bVirus) // Onboard system virus
		return;

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
	}
	else
	{
		// If the opponent was marked for destruction...
		if (pOpp->JustDestroyed())
		{
			// Go ahead and remove it, and don't draw it
			m_pGroup->DestroyOpp(pOpp);
			return;
		}

		Cellpnt = pOpp->GetXYCell();
		pnt = pOpp->GetNewPos();
		pnt.x = ScaleInt( pnt.x, iZoomFactor );
		pnt.y = ScaleInt( pnt.y, iZoomFactor );
		
		MapCellToCellUnit((int *)&Cellpnt.x, (int *)&Cellpnt.y);
		MapCellUnitToRadar((int *)&Cellpnt.x, (int *)&Cellpnt.y);

		pnt.x += Cellpnt.x;
		pnt.y += Cellpnt.y;

		pOpp->SetLastDraw(pnt);
	}

	HPEN hPen;
	HPEN hOldPen = NULL;
	int iClass = pOpp->GetClass();
	if (iClass < MAX_CLASS)
	{
		hPen = (bOn ? hPenCar[iClass-1] : GetStockPen(BLACK_PEN));
		hOldPen = SelectPen( hDC, hPen );
	}
	else
	{
		hPen = (bOn ? hPenM : GetStockPen(BLACK_PEN));
		hOldPen = SelectPen( hDC, hPen );
	}

	static POINT ptFr[] = { -2,-2,  -2,+2,  +2,+2,  +2,-2 };
	static POINT ptTo[] = { -2,+2,  +2,+2,  +2,-2,  -2,-2 };

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


/***********************************************************************/
void CWorld::DrawPortals( HDC hDC, PDIB pDibDst, int x, int y, CCell * pCell )
/***********************************************************************/
{
	if ( !pCell )
		return;

	#define HALF_WIDTH 24
	#define HALF_HEIGHT 21
	#define HALF_BITMAP 9
	#define XOFF 18
	#define YOFF 12
	#define YVOFF 19

	// go back to the cell center, and offset by half a bitmap
	int xOffset = HALF_WIDTH - HALF_BITMAP;
	int yOffset = HALF_HEIGHT - HALF_BITMAP;

	char c;
	int x0, y0;
	ITEMID id = IDC_RADAR_FIRST + 1;

	BOOL bOdd = (pCell->Getx() & 1);
	if ( !bOdd )
	{ // Draw 2 and 8 only on EVEN cells
		// Check 2 o'clock
		c = pCell->PortalType(0);
		x0 = x + ScaleInt(xOffset + XOFF, iZoomFactor);
		y0 = y + ScaleInt(yOffset - YOFF, iZoomFactor);
		if ( c == 'E' || c == 'e' )
			DrawDib( pDib[IDC_RADAR_ENERGY-id],    hDC, pDibDst, x0, y0 );
		if ( c == 'B' || c == 'b' )
			DrawDib( pDib[IDC_RADAR_BARRICADE-id], hDC, pDibDst, x0, y0 );

		// Check 8 o'clock
		c = pCell->PortalType(3);
		x0 = x + ScaleInt(xOffset - XOFF, iZoomFactor);
		y0 = y + ScaleInt(yOffset + YOFF, iZoomFactor);
		if ( c == 'E' || c == 'e' )
			DrawDib( pDib[IDC_RADAR_ENERGY-id],    hDC, pDibDst, x0, y0 );
		if ( c == 'B' || c == 'b' )
			DrawDib( pDib[IDC_RADAR_BARRICADE-id], hDC, pDibDst, x0, y0 );
	}

	if (1)
	{ // Always draw 10 and 12
		// Check 10 o'clock
		c = pCell->PortalType(4);
		x0 = x + ScaleInt(xOffset - XOFF, iZoomFactor);
		y0 = y + ScaleInt(yOffset - YOFF, iZoomFactor);
		if ( c == 'E' || c == 'e' )
			DrawDib( pDib[IDC_RADAR_ENERGY-id],    hDC, pDibDst, x0, y0 );
		if ( c == 'B' || c == 'b' )
			DrawDib( pDib[IDC_RADAR_BARRICADE-id], hDC, pDibDst, x0, y0 );

		// Check 12 o'clock
		c = pCell->PortalType(5);
		x0 = x + ScaleInt(xOffset,         iZoomFactor);
		y0 = y + ScaleInt(yOffset - YVOFF, iZoomFactor);
		if ( c == 'E' || c == 'e' )
			DrawDib( pDib[IDC_RADAR_ENERGY-id],    hDC, pDibDst, x0, y0 );
		if ( c == 'B' || c == 'b' )
			DrawDib( pDib[IDC_RADAR_BARRICADE-id], hDC, pDibDst, x0, y0 );
	}
}

/***********************************************************************/
void CWorld::DrawCulDeSacs( HDC hDC, PDIB pDibDst, int x, int y, CCell * pCell )
/***********************************************************************/
{
	if ( !pCell )
		return;

	#define HALF_WIDTH 24
	#define HALF_HEIGHT 21
	#define HALF_BITMAP 9
	#define CXOFF 10
	#define CYOFF 7
	#define CYVOFF 10

	// go back to the cell center, and offset by half a bitmap
	int xOffset = HALF_WIDTH - HALF_BITMAP;
	int yOffset = HALF_HEIGHT - HALF_BITMAP;

	unsigned char c;
	int x0, y0;
	ITEMID id = IDC_RADAR_FIRST + 1;

	// Check 2 o'clock
	c = pCell->PortalType(0);
	x0 = x + ScaleInt(xOffset + CXOFF, iZoomFactor);
	y0 = y + ScaleInt(yOffset - CYOFF, iZoomFactor);
	if ( c == 'M' )
		DrawDib( pDib[IDC_RADAR_CUL_EMPTY-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'N' )
		DrawDib( pDib[IDC_RADAR_CUL_ENTRY-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'X' )
		DrawDib( pDib[IDC_RADAR_CUL_EXIT-id], hDC, pDibDst, x0, y0);
	else
	if ( c == 'R' )
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'A' )
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'S' )
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if (c == 'h' || c == 'i' || c == 'j' || c == 'k' || c == 'l' || c == 'm' || c == 'n')
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if ( c >= '1' && c <= '9' )
		DrawDib( pDib[IDC_RADAR_CUL_COLLECT-id], hDC, pDibDst, x0, y0 );
	else
	if ( c >= PORT_SWITCH_START && c <= PORT_SWITCH_END )
		DrawDib( pDib[IDC_RADAR_CUL_SWITCH-id], hDC, pDibDst, x0, y0 );

	// Check 4 o'clock
	c = pCell->PortalType(1);
	x0 = x + ScaleInt(xOffset + CXOFF, iZoomFactor);
	y0 = y + ScaleInt(yOffset + CYOFF, iZoomFactor);
	if ( c == 'M' )
		DrawDib( pDib[IDC_RADAR_CUL_EMPTY-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'N' )
		DrawDib( pDib[IDC_RADAR_CUL_ENTRY-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'X' )
		DrawDib( pDib[IDC_RADAR_CUL_EXIT-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'R' )
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'A' )
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'S' )
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if (c == 'h' || c == 'i' || c == 'j' || c == 'k' || c == 'l' || c == 'm' || c == 'n')
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if ( c >= '1' && c <= '9' )
		DrawDib( pDib[IDC_RADAR_CUL_COLLECT-id], hDC, pDibDst, x0, y0 );
	else
	if ( c >= PORT_SWITCH_START && c <= PORT_SWITCH_END )
		DrawDib( pDib[IDC_RADAR_CUL_SWITCH-id], hDC, pDibDst, x0, y0 );

	// Check 6 o'clock
	c = pCell->PortalType(2);
	x0 = x + ScaleInt(xOffset,          iZoomFactor);
	y0 = y + ScaleInt(yOffset + CYVOFF, iZoomFactor);
	if ( c == 'M' )
		DrawDib( pDib[IDC_RADAR_CUL_EMPTY-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'N' )
		DrawDib( pDib[IDC_RADAR_CUL_ENTRY-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'X' )
		DrawDib( pDib[IDC_RADAR_CUL_EXIT-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'R' )
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'A' )
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'S' )
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if (c == 'h' || c == 'i' || c == 'j' || c == 'k' || c == 'l' || c == 'm' || c == 'n')
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if ( c >= '1' && c <= '9' )
		DrawDib( pDib[IDC_RADAR_CUL_COLLECT-id], hDC, pDibDst, x0, y0 );
	else
	if ( c >= PORT_SWITCH_START && c <= PORT_SWITCH_END )
		DrawDib( pDib[IDC_RADAR_CUL_SWITCH-id], hDC, pDibDst, x0, y0 );

	// Check 8 o'clock
	c = pCell->PortalType(3);
	x0 = x + ScaleInt(xOffset - CXOFF, iZoomFactor);
	y0 = y + ScaleInt(yOffset + CYOFF, iZoomFactor);
	if ( c == 'M' )
		DrawDib( pDib[IDC_RADAR_CUL_EMPTY-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'N' )
		DrawDib( pDib[IDC_RADAR_CUL_ENTRY-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'X' )
		DrawDib( pDib[IDC_RADAR_CUL_EXIT-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'R' )
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'A' )
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'S' )
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if (c == 'h' || c == 'i' || c == 'j' || c == 'k' || c == 'l' || c == 'm' || c == 'n')
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if ( c >= '1' && c <= '9' )
		DrawDib( pDib[IDC_RADAR_CUL_COLLECT-id], hDC, pDibDst, x0, y0 );
	else
	if ( c >= PORT_SWITCH_START && c <= PORT_SWITCH_END )
		DrawDib( pDib[IDC_RADAR_CUL_SWITCH-id], hDC, pDibDst, x0, y0 );

	// Check 10 o'clock
	c = pCell->PortalType(4);
	x0 = x + ScaleInt(xOffset - CXOFF, iZoomFactor);
	y0 = y + ScaleInt(yOffset - CYOFF, iZoomFactor);
	if ( c == 'M' || c == 'm' )
		DrawDib( pDib[IDC_RADAR_CUL_EMPTY-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'N' )
		DrawDib( pDib[IDC_RADAR_CUL_ENTRY-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'X' )
		DrawDib( pDib[IDC_RADAR_CUL_EXIT-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'R' )
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'A' )
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'S' )
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if (c == 'h' || c == 'i' || c == 'j' || c == 'k' || c == 'l' || c == 'm' || c == 'n')
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if ( c >= '1' && c <= '9' )
		DrawDib( pDib[IDC_RADAR_CUL_COLLECT-id], hDC, pDibDst, x0, y0 );
	else
	if ( c >= PORT_SWITCH_START && c <= PORT_SWITCH_END )
		DrawDib( pDib[IDC_RADAR_CUL_SWITCH-id], hDC, pDibDst, x0, y0 );

	// Check 12 o'clock
	c = pCell->PortalType(5);
	x0 = x + ScaleInt(xOffset,          iZoomFactor);
	y0 = y + ScaleInt(yOffset - CYVOFF, iZoomFactor);
	if ( c == 'M' )
		DrawDib( pDib[IDC_RADAR_CUL_EMPTY-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'N' )
		DrawDib( pDib[IDC_RADAR_CUL_ENTRY-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'X' )
		DrawDib( pDib[IDC_RADAR_CUL_EXIT-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'R' )
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'A' )
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if ( c == 'S' )
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if (c == 'h' || c == 'i' || c == 'j' || c == 'k' || c == 'l' || c == 'm' || c == 'n')
		DrawDib( pDib[IDC_RADAR_CUL_AMMO-id], hDC, pDibDst, x0, y0 );
	else
	if ( c >= '1' && c <= '9' )
		DrawDib( pDib[IDC_RADAR_CUL_COLLECT-id], hDC, pDibDst, x0, y0 );
	else
	if ( c >= PORT_SWITCH_START && c <= PORT_SWITCH_END )
		DrawDib( pDib[IDC_RADAR_CUL_SWITCH-id], hDC, pDibDst, x0, y0 );
}

//***********************************************************************
int CWorld::HitOpponent(int iIndex, int iDamage)
//***********************************************************************
{
	int iRet = 0;

	// First check special case of gatekeeper on level 10
	if (GetLevel() == 10)
	{
		// See if we are in the gatekeeper shot
		LPSHOT lpShot = Video_GetCurrentShot(App.pGame->lpVideo);
		if (lstrcmp(lpShot->szShotName, "/CXA") == 0)
		{
			iRet = 1000;
			LPSCENE lpCurrentScene = App.GetCurrentScene();
			if ( lpCurrentScene )
			{
				HWND hWnd = lpCurrentScene->GetWindow();
				PostMessage( hWnd, WM_KEYDOWN, VK_F11, 0); // Need to change key later
			}

		}
		else 
			iRet = 0;
	}
	else
	{
		iRet = m_pGroup->HitEngaged(iIndex, iDamage);
	}

	if (iRet > 0)
	{
		lScore += iRet;
		Debug("Score = %ld\n", lScore);
	}

	return iRet;
}

// Level 2, Cell sets
static POINT g_pL2N1Cells[] = {{1,9},{1,10},{1,11},{2,9},{2,10},{2,11},{2,12},{3,10},{3,11},{3,12},{4,10},{4,11},{4,12},{4,13},{5,11},{5,12},{5,13},{6,12},{6,13},{6,14},{7,12},{7,13},{7,14},{8,12},{8,13},{9,11},{9,12},{-1,-1}};
static POINT g_pL2N2Cells[] = {{10,12},{10,13},{11,12},{11,13},{12,11},{12,12},{12,13},{12,14},{13,11},{13,12},{13,13},{14,11},{14,12},{14,13},{14,14},{15,10},{15,11},{15,12},{16,9},{16,10},{16,11},{16,12},{17,10},{17,11},{18,10},{-1,-1}};
static POINT g_pL2N3Cells[] = {{1,17},{1,18},{1,19},{2,17},{2,18},{2,19},{2,20},{3,16},{3,17},{3,18},{4,16},{4,17},{4,18},{5,14},{5,15},{5,16},{5,17},{6,15},{6,16},{6,17},{7,15},{7,16},{8,16},{-1,-1}};
static POINT g_pL2N4Cells[] = {{8,14},{8,15},{8,16},{9,13,},{9,14},{9,15},{9,16},{10,14},{10,15},{10,16},{11,14},{11,15},{11,16},{12,15},{12,16},{12,17},{13,14},{13,15},{13,16},{13,17},{14,16},{14,17},{14,18},{15,16},{15,17},{15,18},{16,16},{16,17},{16,18},{16,19},{16,20},{17,17},{17,18},{18,19},{-1,-1}};

// Level 5 cell sets
static POINT g_pL5N1Cells[] = {{8,31},{9,31},{10,31},{-1,-1}};
static POINT g_pL5N2Cells[] = {{7,30},{8,30},{9,30},{10,30},{11,30},{-1,-1}};
static POINT g_pL5N3Cells[] = {{8,29},{9,29},{10,29},{-1,-1}};
static POINT g_pL5N4Cells[] = {{8,28},{9,28},{10,28},{-1,-1}};
static POINT g_pL5N5Cells[] = {{8,27},{9,27},{10,27},{-1,-1}};
static POINT g_pL5N6Cells[] = {{8,26},{9,26},{10,26},{-1,-1}};
static POINT g_pL5N7Cells[] = {{8,25},{9,25},{10,25},{-1,-1}};
static POINT g_pL5N8Cells[] = {{8,24},{9,24},{10,24},{-1,-1}};
static POINT g_pL5N9Cells[] = {{8,23},{9,23},{10,23},{-1,-1}};

// Level 6 cell sets
static POINT g_pL6N1Cells[] = 
	{{12,4},{12,5},{12,6},{12,7},{12,8},{12,9},{12,10},{12,11},{12,12},{12,13},
	 {13,3},{13,4},{13,5},{13,6},{13,7},{13,8},{13,9},{13,10},{13,11},{13,12},
	 {14,3},{14,4},{14,5},{14,6},{14,7},{14,8},{14,9},{14,10},{14,11},{14,12},
	 {15,2},{15,3},{15,4},{15,5},{15,6},{15,7},{15,8},{15,9},{15,10},{15,11},
	 {16,2},{16,3},{16,4},{16,5},{16,6},{16,7},{16,8},{16,9},{16,10},{16,11},
	 {17,1},{17,2},{17,3},{17,4},{17,5},{17,6},{17,7},{17,8},{17,9},{17,10},
	 {18,2},{18,3},{18,4},{18,5},{18,6},{18,7},{18,8},{18,9},{18,10},
	 {19,2},{19,3},{19,4},{19,5},{19,6},{19,7},{19,8},{19,9},{19,10},
	 {20,1},{20,2},{20,3},{20,4},{20,5},{20,6},{20,7},{20,8},{20,9},{20,10},{20,11},
	 {21,1},{21,2},{21,3},{21,4},{21,5},{21,6},{21,7},{21,8},{21,9},{21,10},{21,11},
	 {22,2},{22,3},{22,4},{22,5},{22,6},{22,7},{22,8},{22,9},{22,10},{22,11},
	 {23,2},{23,3},{23,4},{23,5},{23,6},{23,7},{23,8},{23,9},{23,10},{23,11},{-1,-1}};

/*
static POINT g_pL6N2Cells[] = {{13,3},{13,4},{13,5},{13,6},{13,7},{13,8},{13,9},{13,10},{13,11},{13,12},{-1,-1}};
static POINT g_pL6N3Cells[] = {{14,3},{14,4},{14,5},{14,6},{14,7},{14,8},{14,9},{14,10},{14,11},{14,12},{-1,-1}};
static POINT g_pL6N4Cells[] = {{15,2},{15,3},{15,4},{15,5},{15,6},{15,7},{15,8},{15,9},{15,10},{15,11},{-1,-1}};
static POINT g_pL6N5Cells[] = {{16,2},{16,3},{16,4},{16,5},{16,6},{16,7},{16,8},{16,9},{16,10},{16,11},{-1,-1}};
static POINT g_pL6N6Cells[] = {{17,1},{17,2},{17,3},{17,4},{17,5},{17,6},{17,7},{17,8},{17,9},{17,10},{-1,-1}};
static POINT g_pL6N7Cells[] = {{18,2},{18,3},{18,4},{18,5},{18,6},{18,7},{18,8},{18,9},{18,10},{-1,-1}};
static POINT g_pL6N8Cells[] = {{19,2},{19,3},{19,4},{19,5},{19,6},{19,7},{19,8},{19,9},{19,10},{-1,-1}};
static POINT g_pL6N9Cells[] = {{20,1},{20,2},{20,3},{20,4},{20,5},{20,6},{20,7},{20,8},{20,9},{20,10},{20,11},{-1,-1}};
static POINT g_pL6N10Cells[] = {{21,1},{21,2},{21,3},{21,4},{21,5},{21,6},{21,7},{21,8},{21,9},{21,10},{21,11},{-1,-1}};
static POINT g_pL6N11Cells[] = {{22,2},{22,3},{22,4},{22,5},{22,6},{22,7},{22,8},{22,9},{22,10},{22,11},{-1,-1}};
static POINT g_pL6N12Cells[] = {{23,2},{23,3},{23,4},{23,5},{23,6},{23,7},{23,8},{23,9},{23,10},{23,11},{-1,-1}};
*/

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
		case 5:
			CellsOff(g_pL5N1Cells);
			CellsOff(g_pL5N2Cells);
			CellsOff(g_pL5N3Cells);
			CellsOff(g_pL5N4Cells);
			CellsOff(g_pL5N5Cells);
			CellsOff(g_pL5N6Cells);
			CellsOff(g_pL5N7Cells);
			CellsOff(g_pL5N8Cells);
			CellsOff(g_pL5N9Cells);
			break;
		case 6:
			CellsOff(g_pL6N1Cells);
			/*
			CellsOff(g_pL6N2Cells);
			CellsOff(g_pL6N3Cells);
			CellsOff(g_pL6N4Cells);
			CellsOff(g_pL6N5Cells);
			CellsOff(g_pL6N6Cells);
			CellsOff(g_pL6N7Cells);
			CellsOff(g_pL6N8Cells);
			CellsOff(g_pL6N9Cells);
			CellsOff(g_pL6N10Cells);
			CellsOff(g_pL6N11Cells);
			CellsOff(g_pL6N12Cells);
			*/
			break;
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
		case 5:
		{
			// See if switch has already been set (special level 5 logic)
			BOOL bFoundCells = FALSE;

			while(!bFoundCells)
			{
				if (iNum == 1)
					pPoints = g_pL5N1Cells;
				else
				if (iNum == 2)
					pPoints = g_pL5N2Cells;
				else
				if (iNum == 3)
					pPoints = g_pL5N3Cells;
				else
				if (iNum == 4)
					pPoints = g_pL5N4Cells;
				else
				if (iNum == 5)
					pPoints = g_pL5N5Cells;
				else
				if (iNum == 6)
					pPoints = g_pL5N6Cells;
				else
				if (iNum == 7)
					pPoints = g_pL5N7Cells;
				else
				if (iNum == 8)
					pPoints = g_pL5N8Cells;
				else
				if (iNum == 9)
					pPoints = g_pL5N9Cells;
				else
					return; // All cells on?

				// See if cells already on
				CCell *pCell = GetCell(pPoints->x, pPoints->y);
				if (pCell->IsOn())
					iNum += 3;
				else
					bFoundCells = TRUE;

			}
		}
			break;
		case 6:
			pPoints = g_pL6N1Cells;
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

