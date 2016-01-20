#ifndef _OFFSCREN_H_
#define _OFFSCREN_H_

#include "dib.h"

class FAR COffScreen
{
public:
	// constructors/destructor
	COffScreen(HPALETTE hPal = NULL);
	~COffScreen();

	// implementation
	void DrawRect( HDC hDC, LPRECT lpRect, LPPOINT lpDstPt = NULL );
	void Resize( int dx, int dy );
	void Load( LPSTR lpFileName ); 
	void Load( HINSTANCE hInstance, int idResource, LPSTR lpSwitchString = NULL ); 
	void CopyColors( void );
	void CopyBits( LPRECT lpRect = NULL );
	PDIB GetReadOnlyDIB(void);
	PDIB GetWritableDIB(void); 
	HDC	 GetDC(void); 
	HPALETTE GetPalette(void) { return m_hPal; }
	void SetTransition(int iTransition = 0, int nTicks = -1, int nStepSize = 0);
   
protected:
	PDIB m_pdibClean;			// the Clean DIB
	PDIB m_pdibStage;			// the Offscreen DIB
	HDC m_hDC;					// connected to the Offscreen DIB
	HBITMAP m_hbmOldMonoBitmap; // the DC's original bitmap
	HBITMAP m_hbmWinG;			// WinG bitmap
	LPLONG m_lpSwitches;		// Switch value longs from 'a'-'z' (like 'junk.bmp/z=2')
	HPALETTE m_hPal;			// Palette to be used
	int		m_iTransition;		// Transition override
	int		m_nStepSize;		// Transition step size override
	int		m_nTicks;			// Transition speed override
	BOOL	m_fCreatedPal;		// Flag to signify that we created a palette
};

typedef COffScreen FAR * LPOFFSCREEN;

#endif //_OFFSCREN_H_
