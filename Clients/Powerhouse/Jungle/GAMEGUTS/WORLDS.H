//************************************************************************
// The World class definition
//************************************************************************

class far CWorld;
typedef class CWorld far * LPWORLD;

class far CWorld // : public BaseClass
{
// CONSTRUCTORS
	public:
		CWorld(void);

// DESTRUCTOR
	public:
		~CWorld(void);

// ATTRIBUTES
	private:
		// Static means one copy for all class objects
		HWND hWndDraw;
		LPSTR lpMap;
		int xCells;
		int yCells;
		int iDirection;
		long lViewWidth;
		long lViewHeight;
		POINT ptCellLocation;
		POINT ptViewLocation;
		POINT ptTempLocation;
		int iZoomFactor;
		HPEN hPenG;
		HPEN hPenR;

// FUNCTIONS
	public:
		// Static means it can be called without a class object
		BOOL Init( HWND hWnd, ITEMID idMapData );
		void ComputeCellSizes( LPINT lpxDelta, LPINT lpiWidth, LPINT lpiHeight );
		void ComputeMapDimensions( void );
		void SetViewLocation( LPPOINT lpPoint, BOOL bScroll );
		void GetViewLocation( LPPOINT lpPoint );
		void SetCellLocation( LPPOINT lpPoint, BOOL bScroll );
		void SetDirection( int iNewDirection );
		int GetDirection( void );
		int MoveCellLocation( void );
		void GetCellLocation( LPPOINT lpPoint );
		void SetZoomFactor( int iZoomFactor );
		int GetZoomFactor( void );
		void Draw( HDC hDC, LPRECT lpClipRect );
		void DrawDirection( HDC hDC, BOOL bOn, int x, int y );
		void UpdateView( void );
	private:
		int ScaleInt( int iValue, int iScale );
		void GrowViewRect( LPRECT lpRect, LPPOINT lpCellPoint );
		void DrawHex( HDC hDC, int x, int y );
		void DrawUnionJack( HDC hDC, int x, int y );
		void DrawVerticalTunnel( HDC hDC, int x, int y );
		void Draw10to4Tunnel( HDC hDC, int x, int y );
		void Draw2to8Tunnel( HDC hDC, int x, int y );
		void DrawCulDeSacs( HDC hDC, int x, int y, POINT ptCell, int yDelta, int iBitMask );
		void Draw2CulDeSac( HDC hDC, int x, int y );
		void Draw4CulDeSac( HDC hDC, int x, int y );
		void Draw6CulDeSac( HDC hDC, int x, int y );
		void Draw8CulDeSac( HDC hDC, int x, int y );
		void Draw10CulDeSac( HDC hDC, int x, int y );
		void Draw12CulDeSac( HDC hDC, int x, int y );
		void DrawBlob2x2( HDC hDC, int x, int y );
		void DrawBlob8x7( HDC hDC, int x, int y );
};
