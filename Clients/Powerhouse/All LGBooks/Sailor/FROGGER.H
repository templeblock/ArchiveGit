#ifndef _FROGGER_H_
#define _FROGGER_H_

#include "sailor.h"

#define NUM_LEVELS 3
#define MAX_SUCCESS 4

typedef struct _motorpool
{
	LPSTR		lpCarName;
	int			nCells;
	BOOL		fCarInUse;
	LPSPRITE	lpSprite;
} MOTORPOOL, FAR *LPMOTORPOOL;

class FAR CFroggerScene : public CGBScene
{
public:
	// constructors/destructor
	CFroggerScene(int nNextSceneNo);
	~CFroggerScene();

	// frogger public functions
	void OnAppActivate(BOOL fActivate);
	void OnKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
	void OnNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify);
	BOOL PlaySound(LPCTSTR lpWaveFile, BOOL fHint);
	virtual void ToonInitDone();

private:
	// dialog procedure overridables
	BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
	BOOL OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	void OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void OnRButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void OnTimer(HWND hWnd, UINT id);
	void OnWomDone(HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr);
	void SetNextStart(int i, BOOL fInit = FALSE);
	BOOL IsStartTime(int i);
	BOOL MoveCar(int dx, int dy);
	BOOL LoadMotorPool(LPMOTORPOOL lpMotorPool, int iNumCars);
	BOOL ResetCarInUse(LPSPRITE lpSprite, LPMOTORPOOL lpMotorPool, int iNumCars);
	BOOL ChangeLevel(int iLevel);
	void HideAllCars(LPMOTORPOOL lpMotorPool, int iNumCars);
	void InitLanes();
	void InitDoggerCar(BOOL fMakeVisibleNow);
	void StartGame();
	void PlayIntro();
	void PlaySoundTrack();

public:
	FILE_SPEC	m_szIntros[NUM_LEVELS];
	FILE_SPEC	m_szSuccess[NUM_LEVELS][MAX_SUCCESS];
	int			m_nSuccess[NUM_LEVELS];

private:
	LPSPRITE	m_lpCarSprite;
	int			m_iLevel;
	DWORD		m_dwDeactivateTime;
	BOOL		m_bReady;
	BOOL		m_bSpritesLoaded;
	BOOL		m_bCarStopped;
	LPSOUND		m_pSound;
	BOOL		m_fSuccessPlayed[MAX_SUCCESS];
};

typedef CFroggerScene FAR * LPFROGGERSCENE;
                            
#endif // _FROGGER_H_