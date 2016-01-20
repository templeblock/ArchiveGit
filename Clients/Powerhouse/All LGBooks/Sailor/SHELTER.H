#ifndef _SHELTER_H_
#define _SHELTER_H_

#include "sailor.h"

#define CLICK_WAVE	"CLICK.WAV"
#define MAX_SHELTERS 3
#define MAX_SUCCESS	4

#define INTRO_CHANNEL 		0
#define SOUNDTRACK_CHANNEL	0
#define SUCCESS_CHANNEL		0
#define SFX_CHANNEL			1
#define HINT_CHANNEL		2
#define MAX_PIECES			30

class FAR CShelterPiece
{     
public:
	CShelterPiece();
	~CShelterPiece();

public:
	FILE_SPEC		m_szFileName;
	FILE_SPEC		m_szHintName;
	FILE_SPEC		m_szStartName;
	POINT		m_ptStart;
	POINT		m_ptEnd;
	POINT		m_ptFill;
	LPSPRITE	m_lpSprite;
	LPSPRITE	m_lpHintSprite;
	LPSPRITE	m_lpStartSprite;
	BOOL		m_fInPosition;
};

typedef CShelterPiece FAR *LPSHELTERPIECE;

class FAR CShelter
{
public:
	CShelter();
	~CShelter();

public:
	FILE_SPEC		m_szBackground;
	FILE_SPEC		m_szSuccess;
	int	 			m_nPieces;
	BOOL			m_fCompleted;
	CShelterPiece 	m_pPieces[MAX_PIECES];
};

typedef CShelter FAR *LPSHELTER;

class FAR CShelterScene : public CGBScene
{
public:
	enum SoundPlaying
	{ 
		NotPlaying,
		IntroPlaying,
		SuccessPlaying,
		SoundtrackPlaying
	};
	// constructors/destructor
	CShelterScene(int nNextSceneNo);
	~CShelterScene();
	virtual void ToonInitDone();
	virtual BOOL IsOnHotSpot(LPPOINT lpPoint);
	BOOL PlaySound(LPCTSTR lpWaveFile, BOOL fHint);
	void OnNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify);
	LPSHELTER GetShelter(int nIndex) { return m_pShelters + nIndex; }

private:
	// dialog procedure overridables
	BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
	BOOL OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
	void OnWomDone(HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr);

	void PlayIntro();
	void PlaySuccess();
	void PlayFinale();
	void PlaySoundTrack();
	void PlaySoundEffect(LPSTR lpEffect);
	BOOL InPosition();
	int GetLevel();
	void ChangeLevel(int iLevel);
	void HandleDrop(BOOL fRealDrop = TRUE);
	LPSHELTER GimmeShelter();
	BOOL LoadShelter();
	BOOL UnloadShelter();
	LPSHELTERPIECE FindPiece(LPPOINT lpPoint);
	BOOL IsSolved();
	void ResetLevel();

public:
	FILE_SPEC		m_szIntro;
	int				m_iSpeed;
	int				m_iCloseness;
	RECT			m_rGameArea;
	FILE_SPEC		m_szSoundTrack;
	FILE_SPEC		m_szHammer;
	FILE_SPEC		m_szPickup;
	int				m_nShelters;
	int				m_nSuccess;
	FILE_SPEC		m_szSuccess[MAX_SUCCESS];
	
private:
	int				m_iLevel;
	LPSHELTER		m_pShelters;
	LPSHELTER		m_pCurrent;
	LPSHELTERPIECE 	m_pSelectedPiece;
	LPSHELTERPIECE	m_pMovingPiece;
	LPSOUND			m_pSound;
	BOOL			m_fCapture;
	BOOL			m_fDrag;
	POINT			m_ptAnchor;
	POINT			m_ptStart;
	HCURSOR			m_hHotCursor;
	RECT			m_rOldClip;
	SoundPlaying	m_SoundPlaying;
	BOOL			m_fSuccessPlayed[MAX_SUCCESS];
};

typedef CShelterScene FAR * LPSHELTERSCENE;
                            
#endif // _SHELTER_H_