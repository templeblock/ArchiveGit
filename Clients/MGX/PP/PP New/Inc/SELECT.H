
// Source file: select.c

#define SL_BOX     1		// Draw a rectanglar frame
#define SL_BLOCK   2		// Draw a solid (filled shape)
#define SL_ELLIPSE 3		// Draw an elliptical frame
#define SL_BOXHANDLES 4		// Draw a box with handles
#define SL_LINE	   5		// Draw a line
#define SL_GRID	   6		// Draw a grid
#define SL_TYPE    0x00FF	// Mask out everything but the type flags
#define SL_SPECIAL 0x0100	// special flag, draw rect on StartSelection
#define SL_NOLIMIT 0x0200	// flag to allow user to go outside the image

#define CLOSENESS  3

void StartSelection( HWND hWnd, HDC hDC, LPRECT lpSelectRect, int fFlags, POINT ptCurrent, LFIXED fStatusScale );
void UpdateSelection( HWND hWnd, HDC hDC, LPRECT lpSelectRect, int fFlags, POINT ptCurrent, BOOL bConstrain, long AspectX, long AspectY, BOOL bMove, BOOL bFromCenter);
void MoveInsideRect( LPRECT lpRect, LPRECT lpBoundRect );
void InvertSelection(HWND hWnd, HDC hDC, LPRECT lpSelectRect, int fFlags);
void EndSelection( HWND hWnd, HDC hDC, LPRECT lpSelectRect, int fFlags, BOOL bRemove );
void OrderRect( LPRECT lpSrcRect, LPRECT lpDstRect );
void ConstrainSelection(LPRECT lpRect, long AspectX, long AspectY, BOOL bCenter);
VOID ConstrainHandles(POINT ptCurrent, LPRECT lpSelectRect);
