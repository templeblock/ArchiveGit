///////////////////////////////////////////////////////////////////////////////
//
// Cleanup.h
//
// Copyright (C) 1996 - Powerhouse Entertainment, Inc.
//
// Change History
//-----------------------------------------------------------------------------
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CLEANUP_H_
#define _CLEANUP_H_

#include "farmer.h"

#define MAX_NUMBER_OF_PARTS			20
#define MAX_CLEAN_ANIMALS			10

typedef struct _parts
{
	int		x;
	int		y;
	FNAME	szPartName;

} Nuggets;


typedef struct _meal
{
	Nuggets	pAnimal[MAX_NUMBER_OF_PARTS];
	int		wParts;
	short	wComplete;
	FNAME	szSound;

} ValueMeal;


class FAR CCleanupScene : public CGBScene
{
public:
	enum GameState
	{
		InProgress,
		WaitingForDone
	};

	// constructors/destructor
	CCleanupScene(int nNextSceneNo);
	void OnSNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify);
	~CCleanupScene();
	virtual void ToonInitDone();

private:
	// dialog procedure overridables
	BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
	BOOL OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	void OnWomDone(HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr);	
	void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
	BOOL OnSetCursor(HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg);
	UINT OnMCINotify( HWND hWindow, UINT codeNotify, HMCI hDevice );
	LPSTR GetPathName(LPSTR lpPathName, LPSTR lpFileName);
	void PlayIntro();
	void OnTimer(HWND hWnd, UINT id);
	int GetLevel();
	void ChangeLevel(int iLevel);
	void SetNextDot();
	void ButcherAnimal();
	void SelectDot();
	void SetNextAnimal();
	BOOL LevelComplete();
	void RestartLevel();

public: // add variables to use with parser here
	int 		wPart;  	// read in from RC tracking
	int 		wAnimal;	// read in from RC tracking

    ValueMeal 	Meal[MAX_CLEAN_ANIMALS];
	FNAME 		szIntroWave;
	FILE_SPEC	m_szSuccessWaves[MAX_SUCCESSWAVES];
	int			m_iNumberSuccessWaves;

	int 		SPARKLE_ANIM_CELLS;
	int 		SPARKLE_X_OFFSET;
	int 		SPARKLE_Y_OFFSET;
	int 		TIME_UP;

	STRING 		SPARKLE_BMP;
	
	int 		NUMBER_OF_CLEAN_ANIMALS;

private:
	
	int			m_iLevel;
	HMCI		m_hSound;
	LPSPRITE	m_lpAnimalSprite[MAX_NUMBER_OF_PARTS];     
	LPSPRITE	m_lpDotSprite[MAX_NUMBER_OF_PARTS];     
	LPSPRITE	m_lpSparkSprite;
	LPSPRITE	m_lpFieldSprite[MAX_CLEAN_ANIMALS];     
	short 		wPartNum; 
	short 		wAnimalNum;
	short 		wCount;
	BOOL 		wParts[MAX_NUMBER_OF_PARTS];     
	short		wIntroFlag;
	short		wLevelCompleteFlag;
	HCURSOR		m_hHotCursor;		// handle to the hot spot cursor
	GameState	m_GameState;
};

typedef CCleanupScene FAR * LPCLEANUPSCENE;
                            
#endif // _CLEANUP_H_
