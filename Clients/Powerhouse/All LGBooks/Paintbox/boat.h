#ifndef _BOAT_H_
#define _BOAT_H_

#include "pain.h"

#define NUM_LEVELS 3
#define MAX_SUCCESS 4
                        
#define LEVEL1_BIT 0x01
#define LEVEL2_BIT 0x02
#define LEVEL3_BIT 0x04
                       
typedef struct _StillSprites
{
	LPSTR		lpName;
	int			nCells;
	int 		x;
	int			y; 
	int 		nDelay;     
	WORD		fLevels;		
	LPSPRITE	lpSprite;
} STILLSPRITE, FAR *LPSTILLSPRITE;
                 
typedef struct _motorpool
{
	LPSTR		lpCarName;
	int			nCells;
	BOOL		fCarInUse;
	LPSPRITE	lpSprite;
} MOTORPOOL, FAR *LPMOTORPOOL;

class FAR CBoatScene : public CGBScene
{
public:
	// constructors/destructor
	CBoatScene(int nNextSceneNo);
	~CBoatScene();

	// frogger public functions
	void OnAppActivate(BOOL fActivate);
	void OnKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
	void OnNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify);
	virtual void ToonInitDone();

private:
	// dialog procedure overridables
	BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
	BOOL OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
	BOOL OnSetCursor (HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg);
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
	void InitLanes(void);
	void InitDoggerCar(void);
	void StartGame(void);
	void PlayIntro(void);
	void PlaySoundTrack(void);
	BOOL LoadStills(LPSTILLSPRITE lpStills, int iNumStills);
	void ShowStills(BOOL bShow);
	BOOL GetSpriteCenterPoint (LPSPRITE lpSprite, int *lpiX, int * lpiY);

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
	BOOL		m_fSuccessPlayed[MAX_SUCCESS];
	POINT		m_ptMouse;
	BOOL		m_fMovement;
};

typedef CBoatScene FAR * LPBOATSCENE;
                            
#endif // _BOAT_H_