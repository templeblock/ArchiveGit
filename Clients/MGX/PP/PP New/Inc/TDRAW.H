// Source file: tdraw.c

BOOL WINPROC DlgDrawProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void DrawSquareProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void StrokeMask( LPFRAME lpFrame, LPMASK lpMask );
BOOL DrawShapeImage(LPIMAGE lpImage, LPDRAWSHAPE_PARMS lpParms);
BOOL DrawFreeImage(LPIMAGE lpImage, LPDRAWFREE_PARMS lpParms);
void GetDrawBrushSettings(LPDRAWBRUSH lpBrush);
STATUS_CODE BrushShapeObj(LPIMAGE lpImage,LPCPolyList lpPolyList,HMGXBRUSH hBrush, BOOL bCacheInit, int dirty);
