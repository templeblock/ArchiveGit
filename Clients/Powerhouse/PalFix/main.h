#ifndef _MAIN_H_
#define _MAIN_H_

#include "scene.h"

#ifdef MAIN
	#define DECLARE
#else
	#define DECLARE extern
#endif

class FAR CMainScene : public CScene
{
public:
	// constructors/destructor
	CMainScene() {};
	~CMainScene() {};

private:
	// dialog procedure overridables
	BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void OnDropFiles(HWND hWnd, HDROP hDrop);
	HPALETTE LoadPaletteFromDib( LPSTR lpFileName );
	HPALETTE LoadPaletteFromTarga( LPSTR lpFileName );
	BOOL OnEraseBkgnd(HWND hDlg, HDC hDC);
	void OnPaint( HWND hWnd );
	void Paint( HDC hDC, LPRECT lpRect );
	void DoProcessBGFile( HWND hWnd, LPSTR lpFileName );
	void DoProcessFile(LPSTR lpFileName);
	void OnShiftDown(int nCount);
	void OnShiftUp(int nCount);
	void OnTrimTail(int nCount);
	void OnAddSystemColors(void);
	void OnClrSystemColors(void);
	void OnAddReservedColors(void);
	void OnClrReservedColors(void);
	void OnAddTextColors(void);
	void OnAddMagenta(void);
	void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
	void OnClose(HWND hWnd);

private:
};

typedef CMainScene FAR * LPMAINSCENE;
                            
class CMainApp : public CPHApp
{
public:

	// Constructor
	CMainApp(){};
	// Destructor
	~CMainApp(){};

	// Implementation
	BOOL CreateMainWindow();
	BOOL InitInstance();
	BOOL GotoScene( HWND hWndPreviousScene, int iScene );
	void OnPaletteChanged(HWND hWnd, HWND hWndCausedBy);
	BOOL OnQueryNewPalette(HWND hWnd);
};

DECLARE CMainApp App;


#endif // _MAIN_H_
