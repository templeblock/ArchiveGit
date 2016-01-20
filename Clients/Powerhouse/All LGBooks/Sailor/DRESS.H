#ifndef _DRESS_H_
#define _DRESS_H_

#include "sailor.h"
#include "dressdef.h"

class FAR COutfit
{     
public:
	COutfit();
	~COutfit();

public:
	FILE_SPEC 	m_szTop;
	FILE_SPEC 	m_szMid;
	FILE_SPEC 	m_szBot;
	LPSPRITE	m_lpTopSprite;
	LPSPRITE	m_lpMidSprite;
	LPSPRITE	m_lpBotSprite;
	FILE_SPEC	m_szWave3;
	FILE_SPEC	m_szSuccess;
};

typedef COutfit FAR *LPOUTFIT;

class FAR CDressScene : public CGBScene
{
public:
	enum SoundPlaying
	{ 
		NotPlaying,
		IntroPlaying,
		CluePlaying,
		SuccessPlaying
	};
	// constructors/destructor
	CDressScene(int nNextSceneNo);
	~CDressScene();

	LPOUTFIT GetOutfit(int nOutfit) { return (m_pOutfits + nOutfit); }
	void ToonInitDone();
	BOOL PlaySound(LPCTSTR lpWaveFile, BOOL fHint);
	BOOL IsOnHotSpot(LPPOINT lpPoint);

private:
	// dialog procedure overridables
	BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
	BOOL OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
	void OnWomDone(HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr);

	LPSTR GetPathName(LPSTR lpPathName, LPSTR lpFileName);
	int GetLevel();
	void ChangeLevel(int iNewLevel);
	void ResetTop();
	void ResetMid();
	void ResetBot();
	void ResetExtra();
	LPBYTE GetTop(int i) { return m_pRandomTable + i; }
	LPBYTE GetMid(int i) { return m_pRandomTable + (i+m_nOutfits); }
	LPBYTE GetBot(int i) { return m_pRandomTable + (i+(2*m_nOutfits)); }
	LPBYTE GetExtra(int i) { return m_pRandomTable + (i+(3*m_nOutfits)); }
	int GetRandom(LPBYTE lpTable);
	void ResetTable(LPBYTE lpTable);
	void StartGame(BOOL fPlayIntro = FALSE);
	void PlayIntro();
	void PlayOutfitIntro();
	void PlaySoundTrack();
	void PlaySuccess(LPSTR lpSuccess);
	void PlayClick();

public:
	RECT		m_rTop;
	RECT		m_rMid;
	RECT		m_rBot;
	FILE_SPEC	m_szIntros[NUM_LEVELS];
	FILE_SPEC 	m_szSuccess[NUM_LEVELS];
	FILE_SPEC	m_szSoundTrack;

private:
	int			m_iLevel;
	int			m_nOutfits;
	LPOUTFIT 	m_pOutfits;
	LPBYTE		m_pRandomTable;
	LPOUTFIT	m_pTopOutfit;
	LPOUTFIT	m_pMidOutfit;
	LPOUTFIT	m_pBotOutfit;
	LPOUTFIT	m_pDstOutfit;
	LPSOUND		m_pSound;
	SoundPlaying m_SoundPlaying;
};

typedef CDressScene FAR * LPDRESSSCENE;
                            
#endif // _DRESS_H_