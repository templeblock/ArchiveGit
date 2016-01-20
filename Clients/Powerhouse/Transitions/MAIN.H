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
	BOOL InitInstance();
	BOOL GotoScene( HWND hWndPreviousScene, int iScene );
	void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
};

DECLARE CMainApp App;


#endif // _MAIN_H_
