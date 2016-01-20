#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include <math.h>
#include "goat.h"
#include "sound.h"
#include "commonid.h"
#include "sprite.h"
#include "parser.h"
#include "transit.h"
#include "soccer.h"
#include "soccdef.h"

//#define DEBUG_STATES		// Debug states - uncomment this define to trace the
							// various states 
           
#ifdef DEBUG_STATES            
#define TRACE(s)  OutputDebugString(s);
#else
#define TRACE(s)  
#endif
         

// Callback used to mostly drive states
void CALLBACK OnGoalNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData);


// Used to parse soccer entries in the rc file
class FAR CSoccerParser : public CParser
{
public:
	CSoccerParser(LPSTR lpTableData)
		: CParser(lpTableData) {}

protected:
	BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
};  


//************************************************************************
CSoccerScene::CSoccerScene(int nNextSceneNo) : CGBScene(nNextSceneNo)
//************************************************************************
{                       
	m_nBoyScore = 0;
	m_nGoatScore = 0;
	m_hSound = NULL;
	wOrig = TRUE;
	iBallState 	= L1S_BALL_START; 
	iGoalieState = -1;  
	iGoatState	= L1S_GOAT_START;
	SetRect(&rSoccerArena, ANIM_AREA_LEFT, ANIM_AREA_TOP,
		ANIM_AREA_RIGHT, ANIM_AREA_BOTTOM);  

	m_lpGoatSprite = NULL;
	m_lpGoatScore = NULL;
	m_lpBoyScore = NULL;
	m_lpBallSprite = NULL;
	m_lpGoaleeSprite = NULL;
	m_lpNetSprite = NULL;
	                           
	m_pSound = NULL;	                           	
}

//************************************************************************
CSoccerScene::~CSoccerScene()
//************************************************************************
{      
	delete m_pSound;
	delete m_lpGoatSprite;
	delete m_lpGoatScore;
	delete m_lpBoyScore;
	delete m_lpBallSprite;
	delete m_lpGoaleeSprite;
	delete m_lpNetSprite;
}

//************************************************************************
int CSoccerScene::GetLevel()
//************************************************************************
{
	if (m_nSceneNo == IDD_SOCCERI)
		return(0);
	else
		return(m_nSceneNo - IDD_SOCCER1);
}

//************************************************************************
BOOL CSoccerScene::OnInitDialog( HWND hWnd, HWND hWndControl, LPARAM lParam )
//************************************************************************
{
	CGBScene::OnInitDialog (hWnd, hWndControl, lParam);
	
	m_pSound = new CSound(TRUE);
	if (m_pSound)
		m_pSound->Open( NULL );
	
	if (m_pAnimator)
	{
		HGLOBAL hData;
		LPSTR lpTableData;

		if (GetToon())
			GetToon()->SetSendMouseMode(TRUE);

		// read in the connect point information
		lpTableData = GetResourceData(m_nSceneNo, "gametbl", &hData);
		if (lpTableData)
		{
			CSoccerParser parser(lpTableData);
			parser.ParseTable((DWORD)this);
		}
		if (hData)
		{
			UnlockResource( hData );
			FreeResource( hData );
		}
	}
	return( TRUE );
}

//************************************************************************
void CSoccerScene::ToonInitDone()
//************************************************************************
{
	GetToon()->SetHintState(TRUE);
	InitGame();
	PlayIntro();
}


//************************************************************************
void CSoccerScene::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	switch (id)
	{
		case IDC_HELP:
		{
			FNAME szFileName;
			PlaySound( GetPathName(szFileName, szIntroWave), NO/*bHint*/ );
			break;
		}

		case IDC_LEVEL1:
		case IDC_LEVEL2:
		case IDC_LEVEL3:
		{
			int iLevel = id-IDC_LEVEL1;
		 	if (iLevel >= NUM_LEVELS)
		 		iLevel = NUM_LEVELS-1;
			ChangeLevel(iLevel);
			break;
		}

		case IDC_TOON:
		{
			CGBScene::OnCommand(hWnd, id, hControl, codeNotify);
			break;
		}

		default:
		{
			CGBScene::OnCommand(hWnd, id, hControl, codeNotify);
			break;
		}
	}
}

//************************************************************************
BOOL CSoccerScene::OnSetCursor (HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg)
//************************************************************************
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(hWnd, &pt);

	// set cursor visibility based on game area
	// only turn off cursor when we have capture
	if ( GetToon() )
	{
		if (PtInRect(&GetToon()->m_rGlobalClip, pt))
		{
			SetCursor(NULL);
			// must set real return value for us to take over the cursor
	   		SetDlgMsgResult(hWnd, WM_SETCURSOR, TRUE);
			return(TRUE);
		}
	}
	return(FALSE);
}

//************************************************************************
BOOL CSoccerScene::OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************
{
	switch (msg)
	{
        HANDLE_DLGMSG(hDlg, WM_LBUTTONDOWN, OnLButtonDown);  
        HANDLE_DLGMSG(hDlg, WM_LBUTTONUP, OnLButtonUp);  
        HANDLE_DLGMSG(hDlg, WM_MOUSEMOVE, OnMouseMove);  
		HANDLE_DLGMSG(hDlg, WM_SETCURSOR, OnSetCursor);
		HANDLE_DLGMSG(hDlg, MM_MCINOTIFY, OnMCINotify);

		default:
			return(FALSE);
	}
}

//************************************************************************
void CSoccerScene::OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
	// If intro is playing shut it off
	if ( iGoalieState == -1 )
	{                    
		m_lpGoaleeSprite->ClearPauseAfterCell();
		
		m_pSound->StopChannel(HINT_CHANNEL);  
		m_bIntroPlaying = FALSE;
		
		// Go ahead and start playing
		iGoalieState = L1S_GOALIE_START;
		SetNextGoalieState();		
	}
        
    // If the player clicks and the current level is done
    // then goto the next level        
	if (iGameState == GS_LEVEL1_DONE )
	{
	    iGameState = GS_LEVEL1_START;
	    NextGameState();
		ResetScores();
		SetNextGoalieState();		
	}
}

//************************************************************************
void CSoccerScene::OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
}

//************************************************************************
void CSoccerScene::OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{       
	// Don't move the goat if he is bobbing his head                 
	if (iGoatState > L1S_GOAT_START)
		return;
		
	int iWidth, iTemp;
	m_lpGoatSprite->GetMaxSize(&iWidth, &iTemp);
	iTemp = x - (iWidth / 2);
	m_lpGoatSprite->Jump(iTemp,BOTTOM_SCREEN);
}

//***************************************************************************
UINT CSoccerScene::OnMCINotify( HWND hWindow, UINT codeNotify, HMCI hDevice )
//***************************************************************************
{                    

	return(TRUE);
}

//************************************************************************
void CSoccerScene::PlayIntro()
//************************************************************************
{
	if (GetSound() && szIntroWave[0] != '\0')
	{
		GetSound()->StopChannel (INTRO_CHANNEL);
		FNAME szFileName;
		GetSound()->StartFile( GetPathName(szFileName, szIntroWave), NO/*bLoop*/, INTRO_CHANNEL/*iChannel*/, FALSE );
		m_bIntroPlaying = TRUE;
	}
}

//************************************************************************
void CSoccerScene::InitGame()
//************************************************************************
{
	FNAME szFileName;
	POINT ptOrigin;
	short i;
	FNAME szBmpFile;

	GetRandomNumber(100);
	
	ptOrigin.x = 0;
	ptOrigin.y = 0; 
                          
	// Create goat sprite                          
	m_lpGoatSprite = m_pAnimator->CreateSprite( &ptOrigin );  
	
	for (i=1; i<=4; i++)
	{         
		wsprintf( szBmpFile, "%s%d.bmp", szBmps[BMP_GOAT].szName, i);
		GetPathName(szFileName, szBmpFile); 
		m_lpGoatSprite->AddCell( szFileName, NULL );
	}   
	
	m_lpGoatSprite->SetNotifyProc(OnGoalNotify, (DWORD)this);
	m_lpGoatSprite->Jump(200,BOTTOM_SCREEN);
	m_lpGoatSprite->ActivateCells(1, 3, FALSE);  
	m_lpGoatSprite->SetCurrentCell(0);      

	m_lpGoatSprite->Show(TRUE);
                    
	// Create ball sprite                        
	m_lpBallSprite = m_pAnimator->CreateSprite( &ptOrigin );      
	m_lpBallSprite->SetId(6);  // Used for debugging a sprite

	for( i=1; i<=9; i++)
	{
		wsprintf( szBmpFile, "%s0%d.bmp", szBmps[BMP_BALL].szName, i);
		GetPathName(szFileName, szBmpFile);       
		m_lpBallSprite->AddCell( szFileName, NULL );
	}    
	
	for( i=10; i<=13; i++)
	{
		wsprintf( szBmpFile, "%s%d.bmp", szBmps[BMP_BALL].szName, i);
		GetPathName(szFileName, szBmpFile);       
		m_lpBallSprite->AddCell( szFileName, NULL );
	}      
	                           
	m_lpBallSprite->SetCurrentCell(0);	                           
	m_lpBallSprite->Show(FALSE);
	m_lpBallSprite->SetNotifyProc(OnGoalNotify, (DWORD)this);
                    
	// Create goalie sprite                        
	m_lpGoaleeSprite = m_pAnimator->CreateSprite( &ptOrigin );
	m_lpGoaleeSprite->SetId(4);  // Used for debugging a sprite
	    
	for( i=1; i<=9; i++)
	{		
		wsprintf( szBmpFile, "%s0%d.bmp", szBmps[BMP_GOALEE].szName, i);
		GetPathName(szFileName, szBmpFile);       
		m_lpGoaleeSprite->AddCell( szFileName, NULL );
	}            
	
	for( i=10; i<=28; i++)
	{
		wsprintf( szBmpFile, "%s%d.bmp", szBmps[BMP_GOALEE].szName, i);
		GetPathName(szFileName, szBmpFile);       
		m_lpGoaleeSprite->AddCell( szFileName, NULL );
	}      
	        
    // Create net sprite
    m_lpNetSprite = m_pAnimator->CreateSprite( &ptOrigin );
    GetPathName(szFileName, szBmps[BMP_NET].szName);
    m_lpNetSprite->AddCell(szFileName, NULL);
    
    m_lpNetSprite->Jump(205,28);
    m_lpNetSprite->Show(FALSE);
    
	// Create score sprites
	m_lpGoatScore = m_pAnimator->CreateSprite( &ptOrigin );
	m_lpGoatScore->SetCellsPerSec(0);
	m_lpBoyScore = m_pAnimator->CreateSprite( &ptOrigin );
	m_lpBoyScore->SetCellsPerSec(0);
	for ( i=1; i<=7; i++ )
	{
		wsprintf( szBmpFile, "%s0%d.bmp", szBmps[BMP_SCORES].szName, i);
		GetPathName(szFileName, szBmpFile);       
		m_lpGoatScore->AddCell( szFileName, NULL );
		m_lpBoyScore->AddCell( szFileName, NULL );		
	}

	ResetScores();
	
	m_pAnimator->SetClipRect(&rSoccerArena);	
	
    iGameState = GS_LEVEL1_START;
    
    NextGameState();
}

//************************************************************************
void CSoccerScene::ChangeLevel(int iLevel)
//************************************************************************
{
	if (iLevel >= NUM_LEVELS)
		iLevel = NUM_LEVELS-1;
	
	App.GotoScene(m_hWnd, IDD_SOCCER1+iLevel, m_nNextSceneNo); 
	
	iGameState = GS_LEVEL1_START;
}


//************************************************************************
BOOL CSoccerParser::HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	LPSOCCERSCENE lpScene = (LPSOCCERSCENE)dwUserData;
	

	if (!lstrcmpi(lpKey, "bmp"))
		GetFilename(lpScene->szBmps[nIndex].szName, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "intro"))
		GetFilename(lpScene->szIntroWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "success"))
		GetFilename(lpScene->szSuccessWave, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "speedincrement"))
		GetInt(&lpScene->wIncr, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "differential"))
		GetInt(&lpScene->wDiff, lpEntry, lpValues, nValues);

	if (!lstrcmpi(lpKey, "goaleespeed"))
		GetInt(&lpScene->GoaleeSpeed, lpEntry, lpValues, nValues);
              
	if (!lstrcmpi(lpKey, "ballspeed"))
		GetInt(&lpScene->ballspeed, lpEntry, lpValues, nValues);
    
	return(TRUE);
}

//************************************************************************
void CALLBACK OnGoalNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData)
//************************************************************************
{
	if (dwNotifyData)
	{
		LPSOCCERSCENE lpScene = (LPSOCCERSCENE)dwNotifyData;
		lpScene->OnSNotify(lpSprite, Notify);
	}
}

//************************************************************************
void CSoccerScene::OnSNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify)
//************************************************************************
{
	BOOL fCrash = FALSE;
	             
	// If goalie sprite    
	if(lpSprite == m_lpGoaleeSprite)
	{                  
		// Set next goalie state only on MOVEDONE or PAUSEDONE
		if (Notify == SN_MOVEDONE || Notify == SN_PAUSEDONE)
    	{                           
    		SetNextGoalieState();   
			return;
		}
	}
	
	// If the sprite is the ball sprite and we are not in the start state
	if (lpSprite == m_lpBallSprite && iBallState > 0)
	{                                       
		// Set the next ball state (all messages)
		SetNextBallState(Notify);                 
		return;
	}
	                                             
	// If this is the goat sprite and the message is PAUSEAFTER
	// then set the next goat state	                                             
	if (lpSprite == m_lpGoatSprite && Notify == SN_PAUSEAFTER)
	{   
		SetNextGoatState();
	}
	             
	return;
		      
}
    
//************************************************************************
void CSoccerScene::SetNextGoalieState()
//************************************************************************
{         
	RECT rPos;
	RECT rPos2;     
	int iRight;
	int iLeft;
	int iNum;
	int iLevel = GetLevel();
		                 
	switch(iGoalieState)
	{                  
		case -1:
   		case L1S_GOALIE_START:         
   		{            
    			     
			TRACE("L1S_GOALIE_START\n");
			    			        
			// If first time play music
			if(wOrig)
			{	           
				FNAME szFileName;
				wOrig = FALSE;
				GetSound()->StopChannel(2);
				GetSound()->StopChannel(HINT_CHANNEL);
				
				if (GetSound())
					GetSound()->StartFile( GetPathName(szFileName, "sndtrack.wav"), YES, 3, TRUE );
			}
    			  
    		GoalieMoveOut();
			break;
		}		 
			
		case L1S_GOALIE_MOVE_OUT:     
		
			TRACE("L1S_GOALIE_MOVE_OUT\n");
			
			// See if we need to move again
			m_lpGoaleeSprite->Location(&rPos);
			m_lpGoatSprite->Location(&rPos2);
			
			iRight = rPos2.left + 50;
			iLeft  = rPos2.left - 50;
			                       
			// If goat is sort of in front of the goalie			                       
			if ( rPos.left > iLeft && rPos.left < iRight )
			{          
				int iDist1, iDist2; 
				int iWidth, iHeight;
				iGoalieState = L1S_GOALIE_MOVE_AGAIN;   
				
				// Figure out new spot to move to
				m_lpGoaleeSprite->GetMaxSize(&iWidth, &iHeight);				
				iDist1 = rPos.left - NET_LEFT;
				iDist2 = NET_RIGHT - rPos.left;
				if (iDist2 > iDist1)
					iDist1 = iDist2;
				else
					iDist1 *= -1; 
					
				iNum = GetRandomNumber(2);
				if (iNum == 1)
                	iDist1 = (int)((float)iDist1 * 0.5);
                else 
                	iDist1 = (int)((float)iDist1 * 0.75);                	
								                    
				m_lpGoaleeSprite->SetCycleRepeat(TRUE);
				//m_lpGoaleeSprite->ActivateCells(0, 8, TRUE);
				m_lpGoaleeSprite->ActivateCells(20, 28, TRUE);
			    m_lpGoaleeSprite->Jump(rPos.left, 50);
			    m_lpGoaleeSprite->SetCurrentCell(20);
				m_lpGoaleeSprite->Move(rPos.left + iDist1,50);          
				break;
			}
			
			iGoalieState = L1S_GOALIE_THROW_BALL;              
			m_lpGoaleeSprite->ActivateCells(0, 28, FALSE);
			m_lpGoaleeSprite->ActivateCells(11, 17, TRUE);	          
			m_lpGoaleeSprite->Jump(rPos.left,50);       
			m_lpGoaleeSprite->SetCurrentCell(11);
			m_lpGoaleeSprite->SetCycleRepeat(FALSE);
			m_lpGoaleeSprite->PauseAfterCell(17, 1);
			//m_lpGoaleeSprite->Move(280,50);
			break;                             
			        
		case L1S_GOALIE_MOVE_AGAIN:
			TRACE("L1S_GOALIE_MOVE_AGAIN\n");
			iGoalieState = L1S_GOALIE_THROW_BALL;              
			m_lpGoaleeSprite->Location(&rPos);
			m_lpGoaleeSprite->ActivateCells(0, 28, FALSE);
			m_lpGoaleeSprite->ActivateCells(11, 17, TRUE);	          
			m_lpGoaleeSprite->Jump(rPos.left,50);       
			m_lpGoaleeSprite->SetCurrentCell(11);
			m_lpGoaleeSprite->SetCycleRepeat(FALSE);
			m_lpGoaleeSprite->PauseAfterCell(17, 1);
			break;
						        
		case L1S_GOALIE_THROW_BALL: 
			TRACE("L1S_GOALIE_THROW_BALL\n");   
			SetNextBallState(0);
			if (iLevel == 2)
			{
				iGoalieState = L1S_GOALIE_GAURDLEFT;
	    		m_lpGoaleeSprite->Location(&rPos);
				m_lpGoaleeSprite->SetCycleRepeat(TRUE);
	    		m_lpGoaleeSprite->ActivateCells(9, 28, FALSE);
	    		m_lpGoaleeSprite->ActivateCells(0, 8, TRUE); 
	    		m_lpGoaleeSprite->Jump(rPos.left, rPos.top);
	    		m_lpGoaleeSprite->Move(NET_LEFT, rPos.top);
			}
			else
			{
				iGoalieState = L1S_GOALIE_MOVE_BACK;
			
				m_lpGoaleeSprite->SetCycleRepeat(TRUE);
				m_lpGoaleeSprite->ActivateCells(0, 8, TRUE);
				m_lpGoaleeSprite->ActivateCells(9, 28, FALSE);	          
				m_lpGoaleeSprite->SetCycleRepeat(TRUE);
				m_lpGoaleeSprite->Location(&rPos);
				m_lpGoaleeSprite->Jump(rPos.left,50);       
				m_lpGoaleeSprite->SetCurrentCell(0);
				m_lpGoaleeSprite->Move(440,50);    	
			}
			break;
			
		case L1S_GOALIE_MOVE_BACK:   
			TRACE("L1S_GOALIE_MOVE_BACK\n");
			iGoalieState = L1S_GOALIE_START;
			m_lpGoaleeSprite->ActivateCells(0, 28, FALSE);	          
			m_lpGoaleeSprite->ActivateCell(18, TRUE);
			m_lpGoaleeSprite->Jump(440,50);
    		break;            
    		
    	case L1S_GOALIE_GAURDLEFT:               
			TRACE("L1S_GOALIE_GAURDLEFT\n");
    		iGoalieState = L1S_GOALIE_GAURDRIGHT;
    		m_lpGoaleeSprite->Location(&rPos);
			m_lpGoaleeSprite->SetCycleRepeat(TRUE);
    		m_lpGoaleeSprite->ActivateCells(9, 28, FALSE);
    		m_lpGoaleeSprite->ActivateCells(0, 8, TRUE); 
    		m_lpGoaleeSprite->Jump(rPos.left, rPos.top);
    		m_lpGoaleeSprite->Move(NET_RIGHT, rPos.top);
    		break;
    		
    	case L1S_GOALIE_GAURDRIGHT:               
			TRACE("L1S_GOALIE_GAURDRIGHT\n");
    		iGoalieState = L1S_GOALIE_GAURDLEFT;
    		m_lpGoaleeSprite->Location(&rPos);
			m_lpGoaleeSprite->SetCycleRepeat(TRUE);
    		m_lpGoaleeSprite->ActivateCells(9, 28, FALSE);
    		m_lpGoaleeSprite->ActivateCells(0, 8, TRUE); 
    		m_lpGoaleeSprite->Jump(rPos.left, rPos.top);
    		m_lpGoaleeSprite->Move(NET_LEFT, rPos.top);
    		break;       
    		
		case L1S_GOALIE_MISSED:
			TRACE("L1S_GOALIE_MISSED\n");
			iGoalieState = L1S_GOALIE_GETBALL;
			m_lpGoaleeSprite->ActivateCells(0, 8, TRUE);
			m_lpGoaleeSprite->ActivateCells(9, 28, FALSE);	          
			m_lpGoaleeSprite->SetCycleRepeat(TRUE);
			m_lpGoaleeSprite->Location(&rPos);
			m_lpGoaleeSprite->Jump(rPos.left,50);       
			m_lpGoaleeSprite->SetCurrentCell(0);
			m_lpGoaleeSprite->Move(440,50);    	
			break;                           
			
		case L1S_GOALIE_GETBALL:    
			TRACE("L1S_GOALIE_GETBALL\n");
			GoalieMoveOut();
			break;        
			
		case L1S_GOALIE_DANCE:
		{
			static int nDanceCnt = 0;   
			static BOOL bRight = TRUE;
			if (nDanceCnt == 8)
			{
				nDanceCnt = 0;
				iGameState = GS_LEVEL1_START;
				NextGameState();
			}                 
			else
			{   
				RECT rPos;
				++nDanceCnt;
						
				m_lpGoaleeSprite->Location(&rPos);
				m_lpGoaleeSprite->SetCycleRepeat(TRUE);		                 
	    		m_lpGoaleeSprite->ActivateCells(9, 28, FALSE);
	    		m_lpGoaleeSprite->ActivateCells(0, 8, TRUE); 
	    		m_lpGoaleeSprite->Jump(rPos.left, rPos.top);
				if (bRight)
		    		m_lpGoaleeSprite->Move(rPos.left+50, rPos.top);
				else
		    		m_lpGoaleeSprite->Move(rPos.left-50, rPos.top);
				bRight = !bRight;	
			}
			break;
		}
	}
}
                
//************************************************************************
void CSoccerScene::SetNextGoatState()
//************************************************************************
{
	RECT rPos;
	                   
	switch(iGoatState)
	{	                   
		
		case L1S_GOAT_START:
			TRACE("L1S_GOAT_START\n");
			// Start head butt
			iGoatState = L1S_GOAT_HEAD_DOWN;     
			m_lpGoatSprite->Location(&rPos);
			m_lpGoatSprite->Jump(rPos.left, rPos.top);
			m_lpGoatSprite->ActivateCells(0, 3, TRUE); 
				
			m_lpGoatSprite->SetCurrentCell(0);     
			m_lpGoatSprite->SetCycleRepeat(FALSE); 
			//m_lpGoatSprite->SetCycleBack(TRUE);
			m_lpGoatSprite->PauseAfterCell(3, 1);
			break;
	                                      
		// Head butt down complete, reverse
		case L1S_GOAT_HEAD_DOWN:
			TRACE("L1S_GOAT_HEAD_DOWN\n");
			iGoatState = L1S_GOAT_HEAD_BACK;
			m_lpGoatSprite->Location(&rPos);
			m_lpGoatSprite->Jump(rPos.left, rPos.top);
			m_lpGoatSprite->ReverseCellOrder();
			m_lpGoatSprite->SetCurrentCell(0); 
			m_lpGoatSprite->SetCycleRepeat(FALSE); 
			m_lpGoatSprite->PauseAfterCell(3, 1);
			break;
				    
		case L1S_GOAT_HEAD_BACK:				    
			TRACE("L1S_GOAT_HEAD_BACK\n");
			iGoatState = L1S_GOAT_START;                  
			m_lpGoatSprite->ReverseCellOrder();
			m_lpGoatSprite->ActivateCells(1, 3, FALSE);  
			m_lpGoatSprite->SetCurrentCell(0);
			break;      
	}
}                

//************************************************************************
void CSoccerScene::PlaySuccessWave( LPSTR lpWave, BOOL bWait )
//************************************************************************
{
	if (!lpWave)
		return;

	if (!GetSound())
		return;

	GetSound()->StopAndFree();
	FNAME szFileName;
	if ( bWait )
	{ // Play this wave without wavemix
		LPSOUND lpSound = new CSound;
		if ( lpSound )
		{
			GetSound()->Activate (FALSE);
			lpSound->StartFile( GetPathName(szFileName, lpWave),
				NO/*bLoop*/, 2/*iChannel*/, TRUE/*bWait*/, m_hWnd);
			delete lpSound;
			GetSound()->Activate (TRUE);
		}
	}
	else
		GetSound()->StartFile( GetPathName(szFileName, lpWave), 
			NO/*bLoop*/, 2/*iChannel*/, FALSE/*bWait*/, m_hWnd);
}

//************************************************************************
void CSoccerScene::SetNextBallState(WORD wNotify)
//************************************************************************
{
	RECT rPos;
	int iLevel = GetLevel();
	     
	// If intro is playing shut it off and play the music
	if ( m_bIntroPlaying == TRUE )
	{
		FNAME szFileName;
		GetSound()->StopChannel(2);
		m_pSound->StopChannel(HINT_CHANNEL);
		if (GetSound())
			GetSound()->StartFile( GetPathName(szFileName, "sndtrack.wav"), YES, 3, TRUE );   
		m_bIntroPlaying = FALSE;
	}
	                  
	switch(iBallState)
	{
		// Ball start state
		case L1S_BALL_START:		                  
			TRACE("L1S_BALL_START\n");
			iBallState = L1S_BALL_THROWN;     
			m_lpBallSprite->SetCurrentCell(0);   
			m_nLastCell = 0;
			
			ThrowBall();     
			break;
			
		// Ball has been thrown
		case L1S_BALL_THROWN:		                   
		{
			int iMiddle;
			int iCell;
			
			TRACE("L1S_BALL_THROWN\n");  
			m_lpBallSprite->Location(&rPos);                   
			iMiddle = rPos.top + (rPos.bottom - rPos.top) / 2;
			
			iCell = GetBallThrowCellFromZone(iMiddle);
			
			// This code makes the ball spin
			// The last two frames are the same size but one is rotated,
			// Also, the cell before the one we want has to be set
			if (m_nLastCell == 11) 
				iCell = 9;
				          
			m_lpBallSprite->SetCurrentCell(iCell);
			m_nLastCell = iCell;
				
			// Did goat hit it?
			if(m_pAnimator->CheckCollision(m_lpGoatSprite,m_lpBallSprite))
			{                
				RECT rGoatPos;
                              
				// double check since we don't want to count the ears
				m_lpGoatSprite->Location(&rGoatPos);
				if ( (rPos.right - 6) < (rGoatPos.left + 43) )
					break;
				if ( (rPos.left - 6) > (rGoatPos.right - 50) )
					break;
					
				iBallState = L1S_BALL_HIT;               
				m_lpBallSprite->Location(&rPos);	
				m_lpBallSprite->Jump(rPos.left,rPos.top);	

				if (iLevel == 0)  
				{
					if (GetSound())
					{
						FNAME szFileName;
						m_pSound->StopChannel(4);
						GetSound()->StartFile( GetPathName(szFileName, "hdgood.wav"), 
							NO/*bLoop*/, 4/*iChannel*/, FALSE, m_hWnd);
					}
				
					m_lpBallSprite->Move(rPos.left,20);
				}
				else
				{
					int iGoatWidth, iGoatHeight, iGoatLeft;                  
					int iBallWidth, iBallHeight, iBallCenter;
					RECT rGoatPos;   
					int iDivFactor;
					int iMoveIndex;  
					POINT pt;
					
					m_lpGoatSprite->GetMaxSize(&iGoatWidth, &iGoatHeight);
					m_lpBallSprite->GetMaxSize(&iBallWidth, &iBallHeight);
					m_lpGoatSprite->Location(&rGoatPos);        
					               
					iBallCenter = rPos.left + iBallWidth / 2;
					iGoatLeft = rGoatPos.left + 44;	// Offset by 44 to be more on head
										
					// Find div factor given 9 points on goat's head
					iDivFactor = 62 / 9;	// 62 is the width of the top of the head
					
					// Now normalize centers to factor
					iBallCenter /= iDivFactor;
					iGoatLeft /= iDivFactor;
					
					// Compute index
					iMoveIndex = iBallCenter - iGoatLeft;
					 
					pt = BallDestination(iMoveIndex, &rGoatPos, &rPos, iGoatWidth);
					                           
					if (GetSound())
					{
						FNAME szFileName;
						m_pSound->StopChannel(4);    
						if (iMoveIndex <= 0 || iMoveIndex >= 8)
							GetPathName(szFileName, "head00.wav");
						else if (iMoveIndex == 1 || iMoveIndex == 7)
							GetPathName(szFileName, "head01.wav");
						else if (iMoveIndex == 2 || iMoveIndex == 6)
							GetPathName(szFileName, "head02.wav");
						else
							GetPathName(szFileName, "hdgood.wav");
						GetSound()->StartFile( szFileName, 
							NO/*bLoop*/, 4/*iChannel*/, FALSE, m_hWnd);
					}
					                           
					// Adjust ball speed based on angle
					int iDelta, n;
					if (iMoveIndex < 4)
						n = 4 - iMoveIndex;
					else
						n = iMoveIndex - 4;
						
					iDelta =  (int)pow(n,2) * 10;
					m_lpBallSprite->SetSpeed(ballspeed+iDelta);
					                           
					// Move ball           
					m_lpBallSprite->Move(pt.x, pt.y);					
					
				}
				
				SetNextGoatState();
			}                            
			
			// Did Goat miss it?
			else if (wNotify == SN_MOVEDONE)
			{                      
				iBallState = L1S_BALL_START;
				if (GetSound())
				{
					FNAME szFileName;
					m_pSound->StopChannel(4);
					GetSound()->StartFile( GetPathName(szFileName, "miss.wav"), 
						NO/*bLoop*/, 4/*iChannel*/, FALSE, m_hWnd);
				}
				IncBoyScore();	
				if (m_nBoyScore == HIGH_SCORE)
				{            
					iGameState = GS_LEVEL1_LOST;
					NextGameState();
				}
				else
				{
					m_lpBallSprite->ActivateCells(0, 18, FALSE);
					m_lpBallSprite->Show(FALSE);  
					// Make sure goalie is in the correct state
					iGoalieState = L1S_GOALIE_START;
					SetNextGoalieState();				
				}
			}  
			break; 
		}
			
		// Ball has been hit and is flying back
		case L1S_BALL_HIT:		   
		{
			int iMiddle;             
			int iCell;
			
			TRACE("L1S_BALL_HIT\n");
			m_lpBallSprite->Location(&rPos);                   
			iMiddle = rPos.top + (rPos.bottom - rPos.top) / 2;
			iCell = GetBallHitCellFromZone(iMiddle);
			
			// This code makes the ball spin.
			// The first two frames are the same size but one is rotated,
			// Also, the cell before the one we want has to be set
			//if (m_nLastCell == 0)
			//	iCell = 2;
				
			m_lpBallSprite->SetCurrentCell(iCell);
			m_nLastCell = iCell;
			if (wNotify == SN_MOVED)
			{          
				// Did the ball hit the net? (It always should in level 1)            
				if(m_pAnimator->CheckCollision(m_lpBallSprite, m_lpNetSprite))
				{               
					iBallState = L1S_BALL_START;	         
					//m_lpBallSprite->ReverseCellOrder();  
					m_lpBallSprite->ActivateCells(0, 18, FALSE);
					m_lpBallSprite->Show(FALSE);
					              
					if (GetSound())
					{
						FNAME szFileName;
						//GetSound()->StopAndFree();
						GetSound()->StopChannel(4);
						GetSound()->StartFile( GetPathName(szFileName, "net.wav"), 
							NO/*bLoop*/, 4/*iChannel*/, FALSE, m_hWnd);
					}
					IncGoatScore();
					if(m_nGoatScore == HIGH_SCORE)
					{                 
						PlaySuccessWave(szSuccessWave, FALSE/*bWait*/);
						NextGameState();
					} 
					else					
					{                      
						if (iLevel == 2)
							iGoalieState = L1S_GOALIE_MISSED;
							
						SetNextGoalieState();	
					}
				}                   
				
				// Else did the ball hit the goalie?
				else if (m_pAnimator->CheckCollision(m_lpBallSprite, m_lpGoaleeSprite))
				{
					if (GetSound())
					{
						FNAME szFileName;
						//GetSound()->StopAndFree();
						m_pSound->StopChannel(4);
						GetSound()->StartFile( GetPathName(szFileName, "catch.wav"), 
							NO/*bLoop*/, 4/*iChannel*/, FALSE, m_hWnd);
					}
					IncBoyScore();
					if (m_nBoyScore == HIGH_SCORE)
					{
						iGameState = GS_LEVEL1_LOST;
						NextGameState();
					}
					else
					{
						iGoalieState = L1S_GOALIE_MOVE_OUT;
						iBallState = L1S_BALL_START;	         
						//m_lpBallSprite->ReverseCellOrder();  
						m_lpBallSprite->ActivateCells(0, 18, FALSE);
						m_lpBallSprite->Show(FALSE);
						SetNextGoalieState();	                                               
					}
				}
				 
			}
			// Did ball miss the net?
			else if (wNotify == SN_MOVEDONE)
			{                        
				IncBoyScore();
				if (m_nBoyScore == HIGH_SCORE)
				{
					iGameState = GS_LEVEL1_LOST;
					NextGameState();
				}
				else
				{
					iBallState = L1S_BALL_START;
					m_lpBallSprite->ActivateCells(0, 18, FALSE);
					m_lpBallSprite->Show(FALSE); 
					if (iLevel == 2)
						iGoalieState = L1S_GOALIE_MISSED;	
					SetNextGoalieState();				
				}
			}           
		}

		break;
	}
}

//************************************************************************
void CSoccerScene::ThrowBall()
//************************************************************************
{
	RECT rPos;
	int level = GetLevel();
	
	m_lpGoaleeSprite->Location(&rPos);	    
	m_lpBallSprite->SetCycleRepeat(FALSE);
	m_lpBallSprite->Jump(rPos.left,rPos.top);
	m_lpBallSprite->ActivateCells(0, 18, TRUE);
	m_lpBallSprite->SetCurrentCell(0); 
	if (level == 3)
		m_lpBallSprite->SetSpeed(ballspeed + m_nBoyScore * 20);
	else
		m_lpBallSprite->SetSpeed(ballspeed);
	
	
	m_lpBallSprite->SetCellsPerSec(ballspeed/8);    
	                                                 
	if (GetSound())
	{
		FNAME szFileName;
		//GetSound()->StopAndFree();
		m_pSound->StopChannel(4);
		GetSound()->StartFile( GetPathName(szFileName, "throw.wav"), 
			NO/*bLoop*/, 4/*iChannel*/, FALSE, m_hWnd);
	}
	m_lpBallSprite->Show(TRUE);
	m_lpBallSprite->Move(rPos.left,BOTTOM_SCREEN + 100); 		 	
}
          
          
//************************************************************************
void CSoccerScene::NextGameState()
//************************************************************************
{                  
	switch(iGameState)
	{                        
		case GS_LEVEL1_START: 
			iGameState = GS_LEVEL1_PLAYING;
			iBallState =   L1S_BALL_START;
			iGoatState = L1S_GOAT_START;
			iGoalieState = -1;

			ResetScores();
						
			m_lpGoaleeSprite->ActivateCells(0, 28, FALSE);	          
			m_lpGoaleeSprite->ActivateCell(18, TRUE);
			m_lpGoaleeSprite->Jump(440,50);    
			m_lpGoaleeSprite->SetCurrentCell(18);
			m_lpGoaleeSprite->SetNotifyProc(OnGoalNotify, (DWORD)this);
		    m_lpGoaleeSprite->SetSpeed(GoaleeSpeed);
			m_lpGoaleeSprite->SetCellsPerSec(10);		 
			
			m_lpGoaleeSprite->Show(TRUE);   
			break;    
			
		case GS_LEVEL1_PLAYING:
			iGameState = GS_LEVEL1_DONE;
			m_nBoyScore = 0;
			m_nGoatScore = 0;
			wOrig = TRUE;

			// If we are in Play mode, go to the next scene
			if (m_nSceneNo == IDD_SOCCERI)
			{
				PlaySuccessWave(szSuccessWave, TRUE/*bWait*/);
				FORWARD_WM_COMMAND(m_hWnd, IDC_NEXT, NULL, m_nNextSceneNo, PostMessage);
				return;
			}
			else
			{
				PlaySuccessWave(szSuccessWave, FALSE/*bWait*/);
			}

			break;        
			
		case GS_LEVEL1_LOST:
			PlaySuccessWave("lose.wav", FALSE/*bWait*/);
			iGameState = GS_LEVEL1_START; 
			BoyWon();     
			iGoalieState = L1S_GOALIE_DANCE;
			SetNextGoalieState();
			wOrig = TRUE;
		    break;			
	}
}

// Tangent array uses for the various angles that the ball hits goat's head       
static double dTan[9] = {0.3639702342662, 1.0, 0.3639702342662,  0.1763269807085, 1.0, 
						 0.1763269807085, 0.3639702342662, 1.0, 0.3639702342662};

//************************************************************************
POINT CSoccerScene::BallDestination(int iMoveIndex, LPRECT lprGoatPos, LPRECT lprBallPos, int iGoatWidth)
//************************************************************************
{                      
	POINT pt;      
	
	double dAdjacent = (double)lprGoatPos->left - (double)ANIM_AREA_LEFT;       
	
	if (iMoveIndex < 0)
		iMoveIndex = 0;
	else if (iMoveIndex > 8)
	    iMoveIndex = 8;
	    
	double dOpposite = dAdjacent *  dTan[iMoveIndex]; // tan 20      
	
	switch(iMoveIndex)
	{
		case 0:
		case 1:
			pt.x = ANIM_AREA_LEFT;
			pt.y = lprGoatPos->top - (int)dOpposite;
			break;
			
		case 2:
		case 3:       
			pt.x = lprBallPos->left - (int)dOpposite;
			pt.y = 10;
			break;
                         
		case 5:       
		case 6:
			pt.x = lprBallPos->left + (int)dOpposite;
			pt.y = 10;
			break;
			
		case 7:
		case 8:
			pt.x = ANIM_AREA_RIGHT;
			pt.y = lprGoatPos->top - (int)dOpposite;
			break;		
			                         
		// Straight ahead is the default			   
		default:
			pt.x = lprBallPos->left;
			pt.y = 10;
			break;
	}
	   
	return pt;
}
                  

//************************************************************************
void CSoccerScene::GoalieMoveOut()
//************************************************************************
{                  
	int iNum;
	iGoalieState = L1S_GOALIE_MOVE_OUT;              
	m_lpGoaleeSprite->SetCycleRepeat(TRUE);
	//m_lpGoaleeSprite->ActivateCells(0, 8, TRUE);
	//m_lpGoaleeSprite->ActivateCells(9, 18, FALSE);	          
	m_lpGoaleeSprite->ActivateCells(0, 18, FALSE);	          
	m_lpGoaleeSprite->ActivateCells(20, 28, TRUE);	          
	m_lpGoaleeSprite->Jump(440,50);
	m_lpGoaleeSprite->Show(TRUE);
    m_lpGoaleeSprite->SetSpeed(GoaleeSpeed);     
			    
    // Randomize spot boy throws from
    iNum = GetRandomNumber(8) * 24;
	m_lpGoaleeSprite->SetCellsPerSec(10);
	m_lpGoaleeSprite->Move(210+iNum,50);
}


//************************************************************************
int  CSoccerScene::GetBallThrowCellFromZone(int iY)
//************************************************************************
{    
	static int iLast = 0;      
	int iDelta = (BALL_ZONE_END - BALL_ZONE_START) / MAX_BALL_ZONES;
	int i;
	int iYGrow = BALL_ZONE_START + iDelta;
		               
	for(i=0; i<MAX_BALL_ZONES; i++)
	{
		if (iY < iYGrow )
		{           
			iLast = i;
			return i;
		}  
		iYGrow += iDelta;
	}                   
	    
	// If we didn't find one then return last
	return iLast;
}

//************************************************************************
int  CSoccerScene::GetBallHitCellFromZone(int iY)
//************************************************************************
{    
	static int iLast = 0;    
	//int iLastZone = MAX_BALL_ZONES - 2;  
	int iDelta = (BALL_ZONE_END - BALL_ZONE_START) / MAX_BALL_ZONES;
	//int iDelta = (BALL_ZONE_END - BALL_ZONE_START) / iLastZone;
	int i;
	int iYGrow = BALL_ZONE_START + iDelta + 50;                       
	//int iHalfZones = MAX_BALL_ZONES / 2;

	if ( iY < BALL_ZONE_START )
	{
		iLast = 0;		               
		return 0;
	}
		         
	for(i=0; i<MAX_BALL_ZONES; i++)
	{
		if (iY < iYGrow )
		{           
			iLast = i;
			return i;
		}  
		iYGrow += iDelta;
	}                   
 
	// If we didn't find one then return biggest
	return i;
}

//************************************************************************
void CSoccerScene::ResetScores()
//************************************************************************
{
	m_nGoatScore = 0;
	m_lpGoatScore->Jump(516, 38);
	m_lpGoatScore->ActivateCells(0, 6, FALSE);	          
	m_lpGoatScore->SetCurrentCell(0);
	m_lpGoatScore->ActivateCell(0, TRUE);	          
	m_lpGoatScore->Show(TRUE);
	m_lpGoatScore->Draw();
	
	m_nBoyScore = 0;
	m_lpBoyScore->Jump(550, 38);
	m_lpBoyScore->ActivateCells(0, 6, FALSE);	          
	m_lpBoyScore->SetCurrentCell(0);
	m_lpBoyScore->ActivateCell(0, TRUE);	          
	m_lpBoyScore->Show(TRUE);
	m_lpBoyScore->Draw();
}
		             
//************************************************************************
void CSoccerScene::IncGoatScore()
//************************************************************************
{            
	if (m_nGoatScore >= 6)
		return;
		
	++m_nGoatScore;
	m_lpGoatScore->ActivateCells(0, 6, FALSE);	          
	m_lpGoatScore->SetCurrentCell(m_nGoatScore);
	m_lpGoatScore->ActivateCell(m_nGoatScore, TRUE);	          
	m_lpGoatScore->Draw();
}		                                              

//************************************************************************
void CSoccerScene::IncBoyScore()
//************************************************************************
{            
	if (m_nBoyScore >= 6)
		return;
		
	++m_nBoyScore;
	m_lpBoyScore->ActivateCells(0, 6, FALSE);	          
	m_lpBoyScore->SetCurrentCell(m_nBoyScore);
	m_lpBoyScore->ActivateCell(m_nBoyScore, TRUE);	          
	m_lpBoyScore->Draw();
}		                                              

//************************************************************************
void CSoccerScene::BoyWon()
//************************************************************************
{   
	m_lpBallSprite->Show(FALSE);
}       
