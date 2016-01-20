#ifndef _CLLSN_H_
#define _CLLSN_H_

#include "scene.h"
#include "cllsnid.h"
#include "levels.h"
#include "worlds.h"
#include "collect.h"
#include "game.h"

#ifdef MAIN
  #define DECLARE
#else
  #define DECLARE extern
#endif

#define TITLE_SIZE 32

class CCollisionScene;
class CApp;
                            
class CCollisionScene : public CScene
{
public:
	// constructors/destructor
	CCollisionScene() {};
	~CCollisionScene() {};

	//void OnMouseMove(HWND hWindow, int x, int y, UINT keyFlags);

private:
	// dialog procedure overridables
	BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void OnCommand(HWND hWnd, int id, HWND loWord, UINT hiWord);
	void OnClose(HWND hWnd);
	void OnTimer(HWND hWnd, UINT id);
};


class CApp : public CPHApp
{
public:

	// Constructor
	CApp();
	// Destructor
	~CApp();

	// Implementation
	BOOL GotoScene( HWND hWndPreviousScene, int iScene );

public:
	CGame * pGame;

	// Option flags
	BOOL bOptionsfromGame;
	BOOL bOptionTransitions;	// Transitions
	BOOL bOptionAutoTarget;		// Auto Target
	BOOL bOptionMusic;			// Music
	BOOL bOptionBarricades;		// Barricades
	BOOL bOptionTaunts;			// Gatekeeper background audio
	BOOL bOptionFireBack;		// Bullet holes (enemy firing back)
	BOOL bOptionScript;			// Should we play a scripted list of shots?
	BOOL bMusicState;
	BOOL bEnergyCheat;
	BOOL bShowGrid;

	// Save/Resume scene stuff
	char szGame1Title[TITLE_SIZE + 1];
	char szGame2Title[TITLE_SIZE + 1];
	char szGame3Title[TITLE_SIZE + 1];
	char szGame4Title[TITLE_SIZE + 1];
	char szGame5Title[TITLE_SIZE + 1];
	STRING szGame1FilePath;
	STRING szGame2FilePath;
	STRING szGame3FilePath;
	STRING szGame4FilePath;
	STRING szGame5FilePath;
};

DECLARE CApp App;

#endif // _CLLSN_H_
