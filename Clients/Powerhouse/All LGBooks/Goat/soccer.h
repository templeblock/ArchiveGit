#ifndef _SOCCER_H_
#define _SOCCER_H_

#include "goat.h"
               
#define MAX_BALL_ZONES  13
#define BALL_ZONE_START 120
#define BALL_ZONE_END   380

typedef struct _bmpfiles  
{

	FNAME szName;
} bmpfiles;
      
#define BMP_GOAT		0
#define BMP_GOALEE		1 
#define BMP_BALL		2   
#define BMP_NET			3
#define BMP_SCORES		4
#define MAX_BMPS		5		// This should be the last one
   
#define BOTTOM_SCREEN		370
                   
#define HIGH_SCORE			6
                   
#define ANIM_AREA_LEFT 		47
#define ANIM_AREA_TOP  		30
#define ANIM_AREA_RIGHT 	592
#define ANIM_AREA_BOTTOM	433 


#define NET_LEFT			206		// Left side of net
#define NET_RIGHT			420		// Right side of net
                                 
// Game states
#define GS_LEVEL1_START			0	// Start level1
#define GS_LEVEL1_PLAYING		1   // Playing in level 1
#define GS_LEVEL1_DONE			2	// Completed level 1
#define GS_LEVEL1_LOST          3	// Lost

// Level 1 Goalie States
#define L1S_GOALIE_START		0	// Boy start position
#define L1S_GOALIE_MOVE_OUT 	1	// Boy moves out in front of net
#define L1S_GOALIE_THROW_BALL	2	// Boy throws ball
#define L1S_GOALIE_MOVE_BACK	3	// Boy moves back to side of net
#define L1S_GOALIE_MOVE_AGAIN	4   // Boy moves again if goat lined up 1st time
#define L1S_GOALIE_GAURDLEFT	5	// Boy moves to left side of net
#define L1S_GOALIE_GAURDRIGHT	6	// Boy moves to right side of net
#define L1S_GOALIE_MISSED		7	// Boy misses ball
#define L1S_GOALIE_GETBALL		8	// Boy goes to get ball
#define L1S_GOALIE_DANCE		9	// Boy victory dance

// Level 1 Ball States
#define L1S_BALL_START 		0		// Ball start position
#define L1S_BALL_THROWN		1		// Ball is thrown
#define L1S_BALL_HIT		2		// Ball is hit by the goat
#define L1S_BALL_MISSED		3		// Ball is missed by the goat
                    
// Level 1 Goat States
#define L1S_GOAT_START		0		// Goat head start position
#define L1S_GOAT_HEAD_DOWN	1		// Goat head moves down
#define L1S_GOAT_HEAD_BACK	2		// Goat head moves back
                    
                    
class FAR CSoccerScene : public CGBScene
{
public:
	// constructors/destructor
	CSoccerScene(int nNextSceneNo);
	~CSoccerScene();
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
	BOOL OnSetCursor (HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg);
	UINT OnMCINotify( HWND hWindow, UINT codeNotify, HMCI hDevice );
	void PlayIntro(void);
	void InitGame();
	int GetLevel();
	void ChangeLevel(int iLevel);
	void SetNextGoalieState();
	void SetNextGoatState();
	void SetNextBallState(WORD wNotify);
	void PlaySuccessWave(LPSTR lpWave, BOOL bWait);
	void ThrowBall();       
	void NextGameState();
	POINT BallDestination(int iMoveIndex, LPRECT lprGoatPos, LPRECT lprBallPos, int iGoatWidth);
	void GoalieMoveOut();
	                     
	//id InitZones();
	int  GetBallThrowCellFromZone(int iY);
	int  GetBallHitCellFromZone(int iY);      
	
	void ResetScores();
	void IncGoatScore();
	void IncBoyScore();
	void BoyWon();
		                     
public: // add variables to use with parser here
	bmpfiles szBmps[MAX_BMPS];
	FNAME szIntroWave;	
	FNAME szSuccessWave;	
	int wIncr;
	int wDiff;
	int GoaleeSpeed;
	int	ballspeed;

private:
	HMCI		m_hSound; 
	LPSPRITE 	m_lpGoatSprite; 
	LPSPRITE 	m_lpGoaleeSprite; 
	LPSPRITE 	m_lpBallSprite;  
	LPSPRITE 	m_lpNetSprite;  
	LPSPRITE    m_lpGoatScore;
	LPSPRITE	m_lpBoyScore;
	
	short 		wPoints;
	int 		wOrig; 
	        
    int iGameState;	        
	int iBallState;
	int iGoalieState;
	int iGoatState;

	RECT rSoccerArena;    
	
	int m_nBoyScore;		// Goalie scroe
	int m_nGoatScore;		// Goat score        
	
	int m_nLastCell;		// Last cell set for ball

	BOOL m_bIntroPlaying;	// Intro is playing
	
};

typedef CSoccerScene FAR * LPSOCCERSCENE;
                            
#endif // _SOCCER_H_#d