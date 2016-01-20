#ifndef _WORLDS_H
#define _WORLDS_H

#include "Opponents.h"
#include <fstream.h>
   
//************************************************************************
// The World class definition
//************************************************************************
     
#define MAX_CURR_OPPONENTS 6	// Maximum number of opponents that can be in 
								// the player's cell

// Portal definitions
#define PORT_SWITCH_GEN1OFF 128 // Opponent generator 1 off
#define PORT_SWITCH_GEN2OFF 129 // Opponent generator 2 off
#define PORT_SWITCH_GEN3OFF 130 // Opponent generator 3 off
#define PORT_SWITCH_GEN4OFF 131 // Opponent generator 4 off
#define PORT_SWITCH_GEN5OFF 132 // Opponent generator 5 off

#define PORT_SWITCH_CELLSON1 133 // Cells on 1
#define PORT_SWITCH_CELLSON2 134 // Cells on 1
#define PORT_SWITCH_CELLSON3 135 // Cells on 1
#define PORT_SWITCH_CELLSON4 136 // Cells on 1
#define PORT_SWITCH_CELLSON5 137 // Cells on 1


#define PORT_SWITCH_START PORT_SWITCH_GEN1OFF	// Start of portal switchs
#define PORT_SWITCH_END	  PORT_SWITCH_CELLSON5	// End of portal switchs

#define PORT_START		  PORT_SWITCH_START
#define PORT_END		  PORT_SWITCH_END

#define MAX_CLASS 24	// maximum number of car classes

class CWorld
{
// CONSTRUCTORS
  public:
    CWorld(void);

// DESTRUCTOR
  public:
    ~CWorld(void);

// ATTRIBUTES
  private:
	CCell **m_pCells;
	int m_nWidth;
	int m_nHeight;

	COpponentGroup * m_pGroup;
	CPlayer * m_pPlayer;

    HWND hWndDraw;
    int xCellUnits;
    int yCellUnits;
    int iZoomFactor;
	int xDispDelta;
	int iDispWidth;
	int iDispHeight;
	int iCurrentLevel;

    POINT ptViewLocation;
    POINT ptViewLast;
    HPEN hPenG;
    HPEN hPenR;
    HPEN hPenY;
    HPEN hPenW;
    HPEN hPenM;
    HPEN hPenC;
    HPEN hPenB;

	HPEN hPenCar[MAX_CLASS];

	PDIB pDib[50];

	long lScore;

	BOOL m_bVirus;

// FUNCTIONS
  public:
	CCell * GetCell(int x, int y);
    BOOL Init( HWND hWnd );
    void ComputeDispSizes( void );
    void SetViewLocation( LPPOINT lpPoint, BOOL bScroll );
    void GetViewLocation( LPPOINT lpPoint );
	void CenterPlayerView( BOOL bScroll );
	void CenterView( CCell *pCell, BOOL bScroll );
	int  GetLevel( void );
    void SetZoomFactor( int iZoomFactor );
    int  GetZoomFactor( int iIncrement = NULL );
    void Draw( HDC hDC, LPRECT lpRect );
	void ClearOffScreen( LPRECT lpRect );
	void RepairOffScreen( LPRECT lpRect );
	void DrawOffScreen( LPRECT lpRect );
	void DrawOnScreen( HDC hDC, LPRECT lpRect );
	void DrawDib( PDIB pDib, HDC hDC, PDIB pDibDst, int x, int y );
	void DrawCell( HDC hDC, PDIB pDibDst, POINT ptCell );
    void DrawPlayer( HDC hDC, BOOL bOn );
	void DrawOpponents( HDC hDC, BOOL bOn );
	void DrawOpponent( HDC hDC, BOOL bOn, COpp *pOpp );
	CPlayer *GetPlayer(void) { return m_pPlayer; }
	COpponentGroup *GetGroup(void) { return m_pGroup; }
	void StuffCellData( CCell* pCell, LPSTR lpMap, int xCell, int yCell );
    void UpdateView( void );
	void SetBoundaries(void);
	char GetCellUnitCode( LPSTR lpMap, int x, int y );
	void MapCellUnitToCell( LPINT lpX, LPINT lpY );
	void MapCellToCellUnit( LPINT lpX, LPINT lpY );
	void MapCellUnitToCellCenter( LPINT lpX, LPINT lpY );
	void MapCellUnitToView( LPINT lpX, LPINT lpY );
	void MapViewToCellUnit( LPINT lpX, LPINT lpY );
	void MapViewToCell( LPINT lpX, LPINT lpY );
    void MapCellUnitToRadar( LPINT lpX, LPINT lpY );  
	void MapViewToRadar( LPINT lpX, LPINT lpY );
	void MapRadarToView( LPINT lpX, LPINT lpY );
	BOOL IsValidCulDeSac( unsigned char c );

	BOOL LoadLevel( ifstream& in, int iLevel );
	int HitOpponent(int iIndex, int iDamage);
	long GetScore(void) { return lScore; }

	void AllCellsOff(int iLevel);
	void SwitchCellsOn(int iLevel, int iNum);
	void CellsOff(POINT *pPoints);

	CCell **GetAllCells(void) {return m_pCells;}
	int  GetCellMapWidth(void) {return m_nWidth;}
	int  GetCellMapHeight(void) {return m_nHeight;}

	void SetVirus(BOOL bOn) {m_bVirus = bOn;}
	BOOL HaveVirus(void) {return m_bVirus;}

  private:
    int ScaleInt( int iValue, int iScale );
	void DrawPortals( HDC hDC, PDIB pDibDst, int x, int y, CCell * pCell );
    void DrawCulDeSacs( HDC hDC, PDIB pDibDst, int x, int y, CCell * pCell );
};

#endif
