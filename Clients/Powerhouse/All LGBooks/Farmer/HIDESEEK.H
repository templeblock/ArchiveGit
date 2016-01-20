///////////////////////////////////////////////////////////////////////////////
//
// Hideseek.h
//
// Copyright (C) 1996 - Powerhouse Entertainment, Inc.
//
// Change History
//-----------------------------------------------------------------------------
//
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _HIDESEEK_H_
#define _HIDESEEK_H_

#include "farmer.h"


typedef struct _animals
{
	FNAME		szAnimal;
	FNAME		szAnimalLite;
	FNAME		szWave;
} FarmAnimals;


typedef struct _timingstruct
{
	short wCount;
	short wFunc;

} TimingStructure;


typedef struct _doors
{
	int			x;
	int			y;

} Doors;

#define TOTAL_ANIMAL_BMPS		9
#define NUMBER_BARN_DOORS		4

#define MOVE_UP_SPEED 			250

#define GROUND_LEVEL			431

#define INTSTATE_INTRO			0
#define INTSTATE_INIT			1
#define INTSTATE_HIDDEN         2  
#define INTSTATE_HIDE           3 
#define INTSTATE_RESTART        4
#define INTSTATE_RESTARTSCENE	5

#define CODE_NOOP				0
#define CODE_PLAYSOUND			1

#define STATE_NOTIFY			1
#define STATE_TIMER             2
#define STATE_NAILED			3

#define LEVEL_ONE_FUNC			1
#define LEVEL_TWO_FUNC			2
#define LEVEL_THREE_FUNC		3

#define BARN_DOOR_BMP			"DOOR.BMP"
#define BARN_LOPEN_BMP			"OPEN1.BMP"
#define BARN_ROPEN_BMP			"OPEN2.BMP"

#define SPACING					75
#define OPEN_RIGHT_DOOR_OFFSET	85
#define OPEN_LEFT_DOOR_OFFSET	30

	
class FAR CHideSeekScene : public CGBScene
{
public:
	// constructors/destructor
	CHideSeekScene(int nNextSceneNo);
	BOOL IsOnHotSpot(LPPOINT lpPoint);
	void OnSNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify);
	~CHideSeekScene();
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
	void OnWomDone(HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr);
	UINT OnMCINotify( HWND hWindow, UINT codeNotify, HMCI hDevice );
	LPSTR GetPathName(LPSTR lpPathName, LPSTR lpFileName);
	void CreateAnimalSprites(void);
	void ShowAnimalInLoft(LPSPRITE lpSprite);
	void ShowAnimal(int iIndex, int iDoor);
	void OnTimer(HWND hWnd, UINT id);
	short ChooseAnimalToShow();
	void SelectItemToSearchFor();
	void ShutTheBarnDoor();
	void PlayIntro();
	int GetLevel();
	void ChangeLevel(int iLevel);
	void PlayLevelSuccessWave();
	void RestartLevel();
	void CreateBarndoorSprites();
	void MixupAnimalLocations();

public: // add variables to use with parser here

	FarmAnimals TheArk[TOTAL_ANIMAL_BMPS];
	FNAME 		szIntroWave;
	FILE_SPEC	m_szSuccessWaves[MAX_SUCCESSWAVES];
	int			m_iNumberSuccessWaves;

private:
	HWND		m_hWnd;
	int			m_iLevel;
	LPSPRITE	m_lpDoorOpenSprite[NUMBER_BARN_DOORS];
	LPSPRITE	m_lpDoorShutSprite[NUMBER_BARN_DOORS];

	LPSPRITE	m_lpLoftSprite;
	LPSPRITE	m_lpAnimalSprite[TOTAL_ANIMAL_BMPS];
	LPSPRITE	m_lpSpriteLite[TOTAL_ANIMAL_BMPS];
	
	short 		wItemSelected;

	BOOL		bPlayIntro;

	short 		wAnimalUp; 
	short 		wNailed;
	
	short 		wSpacer;
	HCURSOR		m_hHotCursor;		// handle to the hot spot cursor
};

typedef CHideSeekScene FAR * LPHIDESEEKSCENE;
                            
#endif // _HIDESEEK_H_
