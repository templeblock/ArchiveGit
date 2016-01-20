#ifndef _DANCE_H_
#define _DANCE_H_

#include "goat.h"

#define NUM_STEPS				8 

#define BUTTON_PRESS			0 
#define INIT_LEVEL3				1
                 
typedef struct _Buttons
{
	int x;
	int y;              
	int iStart;				// Start frame of play segment	
	int iEnd;       		// End frame
	FNAME	szButton;
} ButtonStruct;
       
typedef struct
{
	int iStart;
	int iEnd;
} PlaySegStruct;
       
class FAR CDanceScene : public CGBScene
{
public:
	// constructors/destructor
	CDanceScene(int nNextSceneNo);
	~CDanceScene();
	virtual void ToonInitDone();
	void OnSNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify);

private:
	// dialog procedure overridables
	BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
	BOOL OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);
	void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags);
	UINT OnMCINotify( HWND hWindow, UINT codeNotify, HMCI hDevice );
	void PlayIntro();
	void InitGame();
	int GetLevel();
	void ChangeLevel(int iLevel);         
	void ClearSteps(void);
	void PostDrawGoatCmd(HWND hWnd);		
	void Play(HWND hWnd);

public: // add variables to use with parser here

	int DANCE_SPEED;
	int DANCE_STEPS;
	
	LPSPRITE	m_lpStepSprites[NUM_STEPS];
	LPSPRITE	m_lpButSprites[NUM_STEPS];
	LPSPRITE    m_lpPlaySprite;
	LPSPRITE    m_lpStopSprite;
	LPSPRITE 	m_lpClearSprite;
	LPSPRITE    m_lpGoatSprite;
	  
	ButtonStruct SrcButt[8];  
	ButtonStruct DstButt[8];    
	
	ButtonStruct PlayBut;
	ButtonStruct StopBut;
	ButtonStruct ClearBut;    
	
	FNAME		 szIntro;
	FNAME		 szSuccess;
	FNAME		 szGoat;
	FNAME		 szSndTrack;
                                
	int nGoatX;		// Goat sprite x pos
	int nGoatY;     // y pos
private:
	HMCI		m_hSound;  
	
	int 		m_nNumSteps;				// Number of steps selected
	int			m_nCurStep;					// Current step to play        
	WORD		m_wDeviceId;				// Avi device id    
	
	PlaySegStruct m_PlaySegs[NUM_STEPS];
	                                          
	BOOL		m_bPlaying;					// Flag indicates if dance is playing	   
	BOOL		m_bIntroPlaying;			// Is intro wave playing?                                       
};

typedef CDanceScene FAR * LPDANCESCENE;
                            
#endif // _DANCE_H_