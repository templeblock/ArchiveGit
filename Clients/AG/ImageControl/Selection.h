#pragma once

class CSelection  
{
public:
	CSelection();
	virtual ~CSelection();

	static void StartSelection( HWND hWnd, HDC hDC, LPRECT lpSelectRect, int fFlags, POINT ptCurrent, double fStatusScale );
	static void UpdateSelection( HWND hWnd, HDC hDC, LPRECT lpSelectRect, int fFlags, POINT ptCurrent, bool bConstrain, long AspectX, long AspectY, bool bMove, bool bFromCenter);
	static void InvertSelection(HWND hWnd, HDC hDC, LPRECT lpSelectRect, int fFlags);
	static void EndSelection( HWND hWnd, HDC hDC, LPRECT lpSelectRect, int fFlags, bool bRemove );
	static void OrderRect( LPRECT lpSrcRect, LPRECT lpDstRect );
	static void ConstrainSelection(LPRECT lpRect, long AspectX, long AspectY, bool bCenter);
	static void ConstrainHandles(POINT ptCurrent, LPRECT lpSelectRect);
};

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
