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

#ifndef _FILLUP_H_
#define _FILLUP_H_

#include "cab.h"

#define MAX_NUMBER_OF_PARTS			20

typedef struct _parts
{
	int			x;
	int			y;
	int			iID;
	int			iClass;
	FNAME		szPartName;
	LPSPRITE	lpSprite;
} Container;


class FAR CFillupScene : public CGBScene
{
public:
	enum GameState
	{
		InProgress,
		WaitingForDone
	};

	// constructors/destructor
	CFillupScene(int nNextSceneNo);
	void OnSNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify);
	~CFillupScene();
	virtual void ToonInitDone();

private:
	// dialog procedure overridables
	BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
	BOOL OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
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
	BOOL LevelComplete();
	void RestartLevel();
	
	// Game specific functions
	void LoadPartSprites();
	void LoadCurrentPartSprite();
	void SetCurrentPart(int PartNum);
	void LoadConveyorSprites();
	void LoadWaterSprites();
	void SetWaterLevel(int Level);
	BOOL CorrectPartChosen(int x, int y);
	
public: // add variables to use with parser here
	int 		m_iNumParts; 	// read in from RC tracking
    int			m_iPart;
	int			m_iStartScore;
    int			m_iSpeed1, m_iSpeed2;
    Container 	m_Part[MAX_NUMBER_OF_PARTS];
    Container	m_CurrentPart;
	FNAME 		m_szIntroWave;
	FILE_SPEC	m_szSuccessWaves[MAX_SUCCESSWAVES];
	int			m_iNumberSuccessWaves;

private:

	LPSPRITE	m_lpWaterSprite;
	WORD		m_wCurrentScore;
	WORD		m_wFinalScore;	
	int			m_iLevel;
	HMCI		m_hSound;
	short		m_wIntroFlag;
	short		m_wLevelCompleteFlag;
	HCURSOR		m_hHotCursor;		// handle to the hot spot cursor
	GameState	m_GameState;
};

typedef CFillupScene FAR * LPFILLUPSCENE;
                            
#endif // _FILLUP_H_
