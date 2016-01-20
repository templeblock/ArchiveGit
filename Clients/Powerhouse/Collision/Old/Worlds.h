//************************************************************************
// The World class definition
//************************************************************************
        
typedef struct cellinfo
{
	POINT ptPlayerLoc;
	char szCellType;
	char szLookType;
	char szPortType_02;
	char szPortType_04;
	char szPortType_06;
	char szPortType_08;
	char szPortType_10;
	char szPortType_12;
	char szOppoType_02;
	char szOppoType_04;
	char szOppoType_06;
	char szOppoType_08;
	char szOppoType_10;
	char szOppoType_12;
} CELLINFO, FAR *LPCELLINFO;

class far COpponent;
typedef COpponent FAR * LPOPPONENT;  

class far COpponent
{
// CONSTRUCTORS
  public:
    COpponent(void);

// DESTRUCTOR
  public:
    ~COpponent(void);

// ATTRIBUTES
  public:
    POINT       ptCell;			// Current cell position of opponent
    int         iDirection;	    // Current direction of travel (2, 4, 6, 8, 10, 12)
    int         iBehavior;      // Type of opponent
    POINT       pntLastDraw;    // Record of last drawing location
    LPOPPONENT  lpPrevOpp;      // Previous opponent
    LPOPPONENT  lpNextOpp;      // Next opponent   
  private:      
	static int	iNumOpponents;  // for index tracking
    int         iHealth;        // Percentage health remaining 0-100

// FUNCTIONS
  public:
	BOOL Init( int iInitDir, POINT pntInitCell, int iBehave );
	BOOL Delete(void);                                         
};                              

class far CWorld;
typedef class CWorld FAR * LPWORLD;

class far CWorld // : public BaseClass
{
// CONSTRUCTORS
  public:
    CWorld(void);

// DESTRUCTOR
  public:
    ~CWorld(void);

// ATTRIBUTES
  public:
  	LPOPPONENT lpOpponent;
  private:
  	POINT ptPlayerLoc;
    int iPlayerDir;

    HWND hWndDraw;
    LPSTR lpMap;
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
    HBRUSH hBrushR;
    HBRUSH hBrushY;
    HBRUSH hBrushB;
    HBRUSH hBrushM;

// FUNCTIONS
  public:
    // Static means it can be called without a class object
    BOOL Init( HWND hWnd );
	BOOL LoadLevel( int iLevel);
    void ComputeDispSizes( void );
    void SetViewLocation( LPPOINT lpPoint, BOOL bScroll );
    void GetViewLocation( LPPOINT lpPoint );
    void SetPlayerLoc( LPPOINT lpPoint, BOOL bScroll );
    void SetPlayerDir( int iNewDirection );
    int  GetPlayerDir( void );
    void NewCellLocation( LPCELLINFO lpCellInfo );
	int  GetLevel( void );
    void GetPlayerLoc( LPPOINT lpPoint );
    void SetZoomFactor( int iZoomFactor );
    int  GetZoomFactor( void );
    void Draw( HDC hDC, LPRECT lpClipRect );
	void DrawCell( HDC hDC, POINT ptCell );
    void DrawPlayer( HDC hDC, BOOL bOn );
	void DeleteOpponents( void );
	void DrawOpponents( HDC hDC, BOOL bOn );
    void DrawOpponent( HDC hDC, BOOL bOn, LPOPPONENT pOpp );
	BOOL GetCellData(CELLINFO * ciCurrentCell);
    void UpdateView( void );
    char Move(LPPOINT lpPoint, int xTest, int yTest, int xDelta, int yDelta);
    char Move2(LPPOINT lpPoint);
    char Move4(LPPOINT lpPoint);
    char Move6(LPPOINT lpPoint);
    char Move8(LPPOINT lpPoint);
    char Move10(LPPOINT lpPoint);
    char Move12(LPPOINT lpPoint);
	char GetCellUnitCode( int x, int y, int dx, int dy );
	void ZapCellUnitCode( int x, int y, int dx, int dy, char c );
	void MapCellUnitToCell( LPINT lpX, LPINT lpY );
	void MapCellToCellUnit( LPINT lpX, LPINT lpY );
	void MapCellUnitToCellCenter( LPINT lpX, LPINT lpY );
	void MapCellUnitToView( LPINT lpX, LPINT lpY );
	void MapViewToCellUnit( LPINT lpX, LPINT lpY );
	void MapViewToCell( LPINT lpX, LPINT lpY );
    void MapCellUnitToRadar( LPINT lpX, LPINT lpY );  
	void MapViewToRadar( LPINT lpX, LPINT lpY );
	void MapRadarToView( LPINT lpX, LPINT lpY );
	BOOL IsValidCulDeSac( char c );
	char GetCulDeSacType( LPCELLINFO lpCellInfo, int iDir );
	BOOL CellUnitSetIfCulDeSac( int x, int y );
	BOOL CellUnitIsOpponent( int x, int y, LPINT lpiType, LPINT lpiDirection );
    char MoveNewDirection( LPPOINT lpPoint, int iDir );
	BOOL MoveSameDirection( LPPOINT lpPoint, int iDir );
	void MoveDrone( LPOPPONENT lpOpp );
	void MoveSentryCCW( LPOPPONENT lpOpp );
	void MoveSentryCW( LPOPPONENT lpOpp );
	void MoveHunter( LPOPPONENT lpOpp );
	void MoveTerrorist( LPOPPONENT lpOpp );
	void MoveCoward( LPOPPONENT lpOpp );
    void MoveOpponents( void );
  private:
    int ScaleInt( int iValue, int iScale );
    void DrawHex( HDC hDC, int x, int y );
    void DrawUnionJack( HDC hDC, int x, int y );
    void DrawBlackHole( HDC hDC, int x, int y );   
    void DrawRadiationRoom( HDC hDC, int x, int y );
    void DrawVerticalTunnel( HDC hDC, int x, int y );
    void Draw10to4Tunnel( HDC hDC, int x, int y );
    void Draw2to8Tunnel( HDC hDC, int x, int y );
	void DrawPortals( HDC hDC, int x, int y, POINT ptCell );
    void DrawBarricade( HDC hDC, int x, int y, LPPOINT lpList );
    void DrawEnergy( HDC hDC, int x, int y, LPPOINT lpList );
    void DrawCulDeSacs( HDC hDC, int x, int y, POINT ptCell );
    void DrawEmptyCulDeSac( HDC hDC, int x, int y, LPPOINT lpList );
    void DrawCollectibleCulDeSac( HDC hDC, int x, int y, LPPOINT lpList );
	void DrawEntryCulDeSac( HDC hDC, int x, int y, LPPOINT ptList );
	void DrawExitCulDeSac( HDC hDC, int x, int y, LPPOINT ptList );
    void DrawRocketCulDeSac( HDC hDC, int x, int y, LPPOINT lpList );
    void DrawAmmoCulDeSac( HDC hDC, int x, int y, LPPOINT lpList );
    void DrawKillerCulDeSac( HDC hDC, int x, int y, LPPOINT lpList );
    void DrawBlob2x2( HDC hDC, int x, int y );
    void DrawBlob8x7( HDC hDC, int x, int y );
};
