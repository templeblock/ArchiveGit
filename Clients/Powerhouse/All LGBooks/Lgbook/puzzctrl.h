#ifndef _PUZZCTRL_H_
#define _PUZZCTRL_H_

long WINPROC EXPORT PuzzleControl( HWND hWindow, unsigned message, WPARAM wParam, LPARAM lParam);

#define GWL_PUZZLE	0
#define PUZZLE_EXTRA (GWL_PUZZLE+sizeof(long))
#define PUZZLE_CLASS "puzzle"

#define MAX_ROWS	10
#define MAX_COLS	10
#define MAX_PIECES (MAX_ROWS * MAX_COLS) // 8 x 8

class FAR CPuzzle
{
public:
	CPuzzle(HWND hWindow, HPALETTE hPal = NULL);
	~CPuzzle();

	BOOL Init(int iRows, int iCols);
	void Setup();
	int NextPuzzle();

	void OnDraw(HDC hDC, LPRECT lpRect, BOOL fGrid);
	void DrawPiece(int i, HDC hDC, PDIB pDibOut, LPPOINT ptDib, LPRECT lpPaint, BOOL fDrawGrid, LPPOINT ptPiece = NULL);
	void DrawHint(int i, HDC hDC, LPPOINT ptDib, LPRECT lpPaint);
	void ClipToRect(LPRECT lpSect, LPRECT lpDst, LPRECT lpSrc);
	BOOL GetSize(LPINT lpWidth, LPINT lpHeight);
	void OnLButtonDown(HWND hWindow, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void OnLButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
	void OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);
	BOOL IsSolved();
	void Resize();
	void SetHintMode(BOOL fHintMode) { m_bHintMode = fHintMode; }
	void PlaySound(UINT idResource);

	BOOL GetTracking() { return m_bTrack; }
	void SetTracking(BOOL bTrack) { m_bTrack = bTrack; }

private:
	BOOL		m_bTrack;
	LPLONG		m_lpSwitches;
	BOOL		m_bInRect;
	HWND		m_hWnd;
	FNAME		m_szDibFile;
	PDIB		m_pDib;
	int			m_iRows;
	int			m_iCols;
	int			m_iMap[MAX_PIECES];
	int			m_iSelect;
	POINT		m_ptSelect;
	POINT		m_ptLast;
	HPALETTE	m_hPal;
	int			m_idResource;
	BOOL		m_bHintMode;
};
typedef CPuzzle FAR *PPUZZLE;

PPUZZLE GetPuzzle(HWND hWindow);
void SetPuzzle(HWND hWindow, PPUZZLE pPuzzle);

#endif // _PUZZCTRL_H_
