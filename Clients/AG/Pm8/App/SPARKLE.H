/*
// $Workfile: SPARKLE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:11p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/SPARKLE.H $
// 
// 1     3/03/99 6:11p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:25:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:35:58   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:31:00   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:36:32   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:33:52   JAY
// Reverted. New series.
// 
//    Rev 1.0   15 Jun 1994 19:00:08   FRED
// Initial revision.
*/ 

#ifndef _SPARKLE_H_
#define _SPARKLE_H_

#include <windows.h>

class far CSparkle
{
public:
	CSparkle(void);
	~CSparkle(void);
	BOOL Init(
		HWND hWindow,					// window in which to draw sparkle
		HINSTANCE hInstance,			// instance containing resource
		LPCSTR lpResource,			// cell bitmap resource identifier
		COLORREF crMask,				// masking color
		HBITMAP hBackground,			// handle of background bitmap
		HPALETTE hPalette,			// palette to use
		int nCellWidth,				// cell width in pixels
		int nX,							// X position of sparkle
		int nY);							// Y position of sparkle
	BOOL DrawCell(void);
	BOOL DrawCell(int nCell);
	BOOL AtLastCell(void);
	BOOL IsValid(void);
private:
	BOOL Release(BOOL fConstructing = FALSE);
	HBITMAP m_hBitmap;
	HWND m_hWindow;
	HPALETTE m_hPalette;
	int m_nX;
	int m_nY;
	int m_nNumberOfCells;
	int m_nCellWidth;
	int m_nCellHeight;
	int m_nCurrentCell;
};


typedef struct
{
	LPCSTR lpResource;			// cell bitmap resource identifier
	COLORREF crMask;				// masking color
	int nCellWidth;				// cell width in pixels
	int nX;							// X position of sparkle
	int nY;							// Y position of sparkle
	int nCellDelay;				// ticks between sparkle cells
	int nSparkleDelay;			// ticks to wait after sparkle is finished
} SPARKLEINFO;

typedef SPARKLEINFO FAR *LPSPARKLEINFO;


class far CSparklers
{
public:
	CSparklers(void);
	~CSparklers(void);
	BOOL Init(
		HINSTANCE hInstance,					// instance containing resource
		HWND hWindow,							// window in which to draw sparkle
		HBITMAP hBackground,					// handle of background bitmap
		HPALETTE hPalette,					// palette to use
		UINT wSparkleCount,					// number of sparkles in sparkle array
		LPSPARKLEINFO lpSparkleInfo);		// array of sparkle information structures
	BOOL Reset(void);
	BOOL Tick(void);
	BOOL IsValid(void);
private:
	int GetDelay(int nDelay);
	BOOL Release(BOOL fConstructing = FALSE);
	HINSTANCE m_hInstance;
	HWND m_hWindow;
	HBITMAP m_hBackground;
	HPALETTE m_hPalette;
	UINT m_wSparkleCount;
	LPSPARKLEINFO m_pSparkleInfo;
	UINT m_wSparkleIndex;
	CSparkle FAR *m_pSparkle;
	int m_nCellDelay;
	int m_nSparkleDelay;
};

#endif		// ifndef _SPARKLE_H_
