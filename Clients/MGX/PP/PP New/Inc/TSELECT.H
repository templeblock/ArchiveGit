// Source file: tselect.c

#define POINTWND(hDlg) GetSafeChildWindow(hDlg, IDC_POINT_WND)


BOOL WINPROC DlgRectangularProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgEllipticalProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgFreeProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgMagicProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void SquareProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void CircleProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void FreeProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void MagicProc( HWND hWindow, UINT msg, int x, int y, UINT32 Option );
void DrawGuideLine(HWND hWnd, HDC hDC, int x1, int y1, int x2, int y2, BOOL on);
int MaskRedrawProc(HWND hWnd, HDC hDC, LPRECT lpRect, BOOL on );
LPFPOINT Int2FixedPoints(LPCPolyList lpList, LPINT lpiPoints, int res);
int Fixed2IntPoints(LPCPolyList lpOutList, LPFPOINT lpInPoints, int nPoints, int res);
LPPOINT PolyList2IntPoints(LPCPolyList lpList, LPINT lpiPoints);
int IntPoints2PolyList(LPCPolyList lpOutList, LPPOINT lpPoints, int nPoints, int iSrcRes, int iDstRes);

// for bringing up save/load shape dialogs
VOID DoShapeSaveDlg(HWND hWnd, LPCPolyList lpList);
LPCPolyList DoShapeLoadDlg(HWND hWnd);

// for point edit functions - uses locals - use only by tdraw & tselect!
void SetPointerDlgItems(HWND hDlg);	// uses local vars! 
WORD HandlePointerFn(HWND hWnd, WPARAM wParam, LPARAM lParam); // uses local vars! 

// for point edit functions - GENERIC -used by tdraw, dlgtools (pointer) & tselect!
void SetPointerFn(HWND hWnd, int mode);
void SetPointerCursor(int PointerFunction, BOOL bLeft);
