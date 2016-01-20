// Source file: offscren.cpp
void OffScreenCreate( void );
void OffScreenDestroy( void );
void OffScreenPaint( HWND hWnd );
void OffScreenDrawRect( HDC hDC, LPRECT lpRect );
void OffScreenResize( int dx, int dy );
void OffScreenLoad( LPTR lpFileName );
void OffScreenCopy( void );
PDIB OffScreenGetReadOnlyDIB(void);
PDIB OffScreenGetWritableDIB(void);
HDC OffScreenDC(void);
