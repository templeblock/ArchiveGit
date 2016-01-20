// FLAGS for DrawObject
#define DRAWOBJ_WHITE    		0x00000001
#define DRAWOBJ_LIGHT_GRAY 		0x00000002
#define DRAWOBJ_MEDIUM_GRAY		0x00000004
#define DRAWOBJ_DARK_GRAY  		0x00000008
#define DRAWOBJ_BLACK      		0x00000010
#define DRAWOBJ_BUTTON_DOWN		0x00000020
				
// Source file: winimage.c
int EnablePaintAbort( BOOL fEnable );
void AstralUpdateWindow(HWND hWnd);
void AstralUpdateWindowNow(HWND hWnd);
CHookHandler *SetDisplayHook(HWND hWnd, CHookHandler *pDisplayHook);
CHookHandler *GetDisplayHook(HWND hWnd);
void SetImageHook(HWND hWnd, CHookHandler *pImageHook);
CHookHandler* GetImageHook(HWND hWnd);
long WINPROC WndImageProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void UpdateImage(LPIMAGE lpImage, LPRECT lpRepairRect, BOOL fInvalidate );
void UpdateImageEx(LPIMAGE lpImage, LPRECT lpRepairRect, BOOL fInvalidate, BOOL fUpdateActive, BOOL fUpdateNonActive );
void UpdateMarquee(HWND hWnd, LPRECT lpRepairRect, BOOL fInvalidate );
void UpdateImageSize( LPIMAGE lpImage );
BOOL AbortUpdate( void );
int PaintImage( HDC hDC, LPRECT lpRepairRect, LPRECT lpInvalidRect, LPMSG lpMsg );
int DrawFrame( LPFRAME lpFrame, HDC hDC, BOOL bToScreen, LPRECT lpSourceRect, LPRECT lpDestRect, int xDiva, int yDiva, int yTop, int yTotal, int nTotalBits );
int DrawFrameEx( LPFRAME lpFrame, HDC hDC, LPRECT lpSourceRect, LPRECT lpDestRect, LPRECT lpRepairRect, LPVOID lpCmsXform = NULL );
int DrawObject( HDC hDC, LPOBJECT lpObject, LPRECT lpSourceRect, LPRECT lpDestRect, DWORD dwFlags);
int DrawObjList( HDC hDC, LPOBJECT lpObject, LPRECT lpSourceRect, LPRECT lpDestRect, LPVOID lpCmsXform = NULL);
// SetImageTitleBar is Obsolete should not be used
void SetImageTitleBar( LPIMAGE lpImage );
void Display2File( HWND hWnd, LPLONG lpx, LPLONG lpy );
void Display2File( HWND hWnd, LPINT lpx, LPINT lpy );
void Display2File1( HWND hWnd, LPINT lpx, LPINT lpy );
void File2Display( HWND hWnd, LPINT lpx, LPINT lpy );
void File2DisplayEx( HWND hWnd, LPLONG lpx, LPLONG lpy, BOOL fCenter );
void File2DisplayEx( HWND hWnd, LPINT lpx, LPINT lpy, BOOL fCenter );
void File2DispRect( HWND hWnd, LPRECT fileRect, LPRECT dispRect );
void File2DispRectEx( HWND hWnd, LPRECT fileRect, LPRECT dispRect );
void File2DispRectExact( HWND hWnd, LPRECT fileRect, LPRECT dispRect );
void Display2File1( HWND hWnd, LPINT lpx, LPINT lpy );
void File2Display( HWND hWnd, LPINT lpx, LPINT lpy );
void File2DispRect( HWND hWnd, LPRECT fileRect, LPRECT dispRect );
void File2DispTForm(HWND hWnd, LPTFORM lpTForm);
void File2DispTFormEx(HWND hWnd, LPTFORM lpTForm, BOOL fCenter);
void Disp2FileTForm(HWND hWnd, LPTFORM lpTForm);
