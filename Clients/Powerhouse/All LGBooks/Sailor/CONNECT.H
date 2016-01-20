#ifndef _CONNECT_H_
#define _CONNECT_H_

#include "sailor.h"

#define MAX_POINTS 50
#define MAX_WAVES 2
#define NUM_LEVELS 3

class FAR CConstellation
{     
public:
	CConstellation();
	~CConstellation();
	int FindPoint(int x, int y, int closeness, BOOL bUseLast);
	int GetFirst(BOOL bUseLast) { return (bUseLast ? 1 : 0); }
	int GetLast(BOOL bUseLast) { return (bUseLast ? m_nPoints-1 : m_nPoints-2); }
	BOOL IsPointConnected(int i);
	void AdjustPoints();
	void Reset();

public:
	int			m_nPoints;
	POINT		m_pts[MAX_POINTS];
	BOOL		m_fConnected[MAX_POINTS];
	FILE_SPEC 	m_szBackground1;
	FILE_SPEC 	m_szBackground2;
	int			m_nWaves;
	FILE_SPEC 	m_szWaveFile[MAX_WAVES];
	FILE_SPEC	m_szNoiseFile;
	FILE_SPEC 	m_szGuide;
	int			m_iLevel;
	POINT		m_ptOffset;
	BOOL		m_fCompleted;
};

typedef CConstellation FAR *LPCONSTELLATION;

class FAR CConnectTransition
{
public:
	CConnectTransition();
	~CConnectTransition() {}
public:
	UINT	m_uTransition;
	int		m_nStepSize;
	int		m_nTicks;
	DWORD	m_dwTotalTime;
};

typedef CConnectTransition FAR *LPCONNECTTRANSITION;
                
class FAR CConnectScene : public CGBScene
{
public:
	// constructors/destructor
	CConnectScene(int nNextSceneNo);
	~CConnectScene();
	virtual void ToonInitDone();
	LPCONSTELLATION GetConstellation(int nIndex) { return m_pConstellations+nIndex; }
	void ShowGuideSprite(BOOL fShow);

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
	void PlayIntro();
	void PlaySoundTrack();
	BOOL PlaySound(LPCTSTR lpWaveFile, BOOL fHint);
	void PlaySoundEffect(LPSTR lpEffect);
	BOOL IsSolved();
	int GetLevel();
	void ChangeLevel(int iLevel, BOOL fPlayIntro = TRUE);
	int GetNumConstellations();
	LPCONSTELLATION GetNextConstellation();
	void DrawLine(int sx, int sy, int dx, int dy);
	void ShowStarSprite(BOOL fShow);
	void MoveBoat();

public:
	int			m_nColorIndex;
	int			m_nLineThickness;
	int			m_nCloseness;
	int			m_nStarCells;
	POINT		m_ptStart;
	POINT		m_ptEnd;
	RECT		m_rTransitionClip;
	CConnectTransition m_Trans1;
	CConnectTransition m_Trans2;
	DWORD		m_dwDelay;
	BOOL		m_bUseLast;
	FILE_SPEC	m_szIntros[NUM_LEVELS];
	FILE_SPEC	m_szSuccess[NUM_LEVELS];
	FILE_SPEC	m_szSoundTrack;
	FILE_SPEC	m_szClick;
	RECT		m_rGameArea;

private:
	int			m_iLevel;
	int			m_nCurrent;
	int			m_nConstellations;
	int			m_nCompleted;
	LPCONSTELLATION m_pConstellations;
	LPSPRITE	m_lpStarSprite;
	LPSPRITE	m_lpGuideSprite;
	LPSOUND		m_pSound;
	BOOL		m_fCapture;
	BOOL		m_fStarOn;
	BOOL		m_fIntroPlaying;
	HCURSOR		m_hHotSpotCursor;
};

typedef CConnectScene FAR * LPCONNECTSCENE;
                            
#endif // _CONNECT_H_