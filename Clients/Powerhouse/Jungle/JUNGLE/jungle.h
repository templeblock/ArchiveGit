#ifndef _JUNGLE_H_
#define _JUNGLE_H_

#include "proto.h"
#include "jungleid.h"
#include "scene.h"

#ifdef MAIN
  #define DECLARE
#else
  #define DECLARE extern
#endif

typedef struct _continfo
{
	int		iShotNum;
	ITEMID	idLeft;
	ITEMID	idUp;
	ITEMID	idRight;
	ITEMID	idDown;
	ITEMID	idHome;
} CONTINFO;

// Instinct meter info.
#define DEFAULT_INSTINCT	    0	// Default instinct meter setting
#define GOOD_RESPONSE		 2000	// Good response points added
#define BAD_RESPONSE		-2000	// Bad response points subtracted

// Puzzle sound info
#define WRONG_SOUND_NAME	"Wrong"
#define NUM_WRONG_SOUNDS	5
#define RIGHT_SOUND_NAME	"Right"
#define NUM_RIGHT_SOUNDS	5
#define SUCCESS_SOUND_NAME	"Success"
#define NUM_SUCCESS_SOUNDS	5

//************************************************************************
// CJungleScene class
//************************************************************************

class FAR CJungleScene : public CScene
{
public:
	// constructors/destructor
	CJungleScene();
	~CJungleScene() {};

	// Game scene stuff
//	BOOL	m_bUseVoiceReco;
	BOOL	m_bPuzzleButtonsOn;

private:
	// dialog procedure overridables
	BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam );
	void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify );
	void OnTimer( HWND hWnd, UINT id );
	void OnClose( HWND hWnd );

	BOOL OnGameSceneInit( HWND hWnd, HWND hWndControl, LPARAM lParam  );
	void OnGameSceneCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify );
	void RandomizePuzzleButtons( HWND hWnd  );
	void DisplayPuzzleButtons(  HWND hWnd, LPSHOT lpShot  );
	void ShowPuzzleButtons( HWND hWnd, LPSHOT lpShot, BOOL bShow  );
	void ShowPuzzle1Buttons( HWND hWnd, BOOL bShow  );
	void ShowPuzzle2Buttons( HWND hWnd, BOOL bShow  );
	void ShowPuzzle3Buttons( HWND hWnd, BOOL bShow  );
	void ShowPuzzle4Buttons( HWND hWnd, BOOL bShow  );
	void ShowPuzzle5Buttons( HWND hWnd, BOOL bShow  );
	void HandlePuzzleButtons( HWND hWnd, ITEMID id  );
	void HandlePuzzle1Buttons( HWND hWnd, ITEMID id  );
	void HandlePuzzle2Buttons( HWND hWnd, ITEMID id  );
	void HandlePuzzle3Buttons( HWND hWnd, ITEMID id  );
	void HandlePuzzle4Buttons( HWND hWnd, ITEMID id  );
	void HandlePuzzle4Timer( HWND hWnd  );
	void HandlePuzzle5Buttons( HWND hWnd, ITEMID id  );
	void DisplayDecisionButtons( HWND hWnd, LPSHOT lpShot );
	void DisplayComboControls( HWND hWnd, LPSHOT lpShot );
	void ClearBackground(  HWND hWnd ) ;
	void ChangeInstinctMeter( HWND hWnd, int iAdjustByValue );
	void VoiceRecoStart( HWND hWnd );
	void VoiceRecoStop( HWND hWnd );
	void VoiceRecoProcess( HWND hWnd, UINT uCode );
	void PlaySound(LPCTSTR lpSoundName, int nIndex, BOOL bWait);
	void PlayWrongSound();
	void PlayRightSound();
	void PlaySuccessSound();

	// The options scene
	void OnOptionsSceneCommand( HWND hWnd, int id, HWND loWord, UINT hiWord  );
	BOOL OnOptionsSceneInit( HWND hWnd, HWND hWndControl, LPARAM lParam  );

    // The Resume scene
    void OnResumeSceneCommand( HWND hWnd, int id, HWND hControl, UINT codeNotify );
    BOOL OnResumeSceneInit( HWND hWnd, HWND hWndControl, LPARAM lParam );
    void ResumeGame( HWND hWnd, int iGameNumber );
    void SaveGame( HWND hWnd, int iGameNumber );
    void SaveGameTitles( HWND hWnd, int iGameNumber );

	// The save game scene
	BOOL OnSaveGameSceneInit( HWND hWnd, HWND hWndControl, LPARAM lParam );
    void OnSaveGameSceneCommand( HWND hWnd, int id, HWND hControl, UINT codeNotify );

	// Helper functions
	void HandleIdleExpired();
	void SetIdleTimeout(DWORD dwIdleDelay, ITEMID idIdleItem = 0);
	void GotoHelperShot( HWND hWnd, LPSTR lpString );

private:
	DWORD	m_dwIdleStartTime;
	DWORD   m_dwIdleDelay;
	ITEMID	m_idIdleItem;
	int		m_iWrongSound;
	int		m_iRightSound;
	int		m_iSuccessSound;
};

typedef CJungleScene FAR * LPJUNGLESCENE;
							
class CJungleApp : public CPHApp
{
public:
	// Constructor
	CJungleApp(){};
	// Destructor
	~CJungleApp(){};

	// Over-rides
	BOOL InitInstance( );
	void OnClose( HWND hWnd );

	// Implementation
	BOOL GotoScene( HWND hWndPreviousScene, int iScene );

	BOOL GetExpert( void ) { return m_bExpert; }
	void SetExpert( BOOL bExpert ) { m_bExpert = bExpert; }

private:
	BOOL m_bExpert;
};

typedef CJungleApp FAR * LPJUNGLEAPP;
							
DECLARE CJungleApp App;


#endif // _JUNGLE_H_
