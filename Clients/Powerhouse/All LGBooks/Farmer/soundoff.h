///////////////////////////////////////////////////////////////////////////////
//
// Soundoff.h
//
// Copyright (C) 1996 - Powerhouse Entertainment, Inc.
//
// Change History
//-----------------------------------------------------------------------------
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _SOUNDOFF_H_
#define _SOUNDOFF_H_

#include "farmer.h"

#define SND_TRACK_CHANNEL       1
#define START_RND_CHANNEL       2
#define STOP_RND_CHANNEL        4


#define     INTRO_PLAY          1
#define     PLAYING             2
#define     FINISH_PLAY         3
#define     MAX_MATCHES         30
#define     NUMBER_OF_ANIMALS   9
#define 	SOUNDOFF_TIMER 		5555


typedef struct _xy
{
	short Animal;
	LPSPRITE    m_lpHeadSprite;      
	LPSPRITE    m_lpBadSprite;       
	LPSPRITE    m_lpBodySprite;     
	short wGuess;
} xystruct;

typedef struct _barnlayout
{
	xystruct Stalls[9]; 
} Barn;


typedef struct _bhead
{
	FNAME szHeadRight; 
	int LCells;
	int X; 
	int Y;
	FNAME szHeadWrong;
	int RCells;
	int len;
	int wid;
} BHead;


typedef struct _beast
{
	FNAME szBeastSound;
	FNAME szBeastBody;
	BHead beastHead;
	int bodyx;
	int bodyy;
	int x; 
	int y;
	int len;
	int wid;
} Beast;



class FAR CSoundOffScene : public CGBScene
{
public:
	// constructors/destructor
	CSoundOffScene(int nNextSceneNo);
	~CSoundOffScene();
	virtual void ToonInitDone();
	BOOL IsOnHotSpot(LPPOINT lpPoint);
	void OnSNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify);
	LPSOUND GetSound() { return m_pSound; }
private:
	// dialog procedure overridables
	void OnTimer(HWND hWnd, UINT id);
	BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
	BOOL OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
	UINT OnMCINotify( HWND hWindow, UINT codeNotify, HMCI hDevice );
	LPSTR GetPathName(LPSTR lpPathName, LPSTR lpFileName);
	void PlayIntro();
	int GetLevel();
	void ChangeLevel(int iLevel);
	void SetPeckingOrder();
	void PlayOrder ();
	xystruct *GetSide(int wCount);
	BOOL MCIPlaySound (LPSTR lpWaveFile);
	BOOL IsOnAnimal (int i, int x, int y);

public: // add variables to use with parser here
	int         wMatches;
	int         wAnims;
	int         ANIMSPEED;
	FNAME       szIntroWave;
	FNAME       szExitWave;
	FNAME       szSuccessWave;
	FNAME       szFailWave;
	Beast       HeadOfCattle[NUMBER_OF_ANIMALS];
	int         boWrongReset;
private:
	HWND		m_hWnd;							// Dialog window handle
	HMCI		m_hWave;						// Current MCI wave handle
	int         m_iLevel;
	xystruct    *pPeckingOrder[MAX_MATCHES];
	int         soundOff;
	xystruct    *pWrongGuess; 
	xystruct    *pCorrectGuess;
	short       wGuess;
	BOOL        fIgnore;
	short       wActiveStalls;
	short       wState;
	int         wChannel;
};

typedef CSoundOffScene FAR * LPSOUNDOFFSCENE;

                            
#endif // _SOUNDOFF_H_

