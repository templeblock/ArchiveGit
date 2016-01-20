#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include <string.h>
#include "goat.h"
#include "sound.h"
#include "commonid.h"
#include "sprite.h"
#include "parser.h"
#include "transit.h"
#include "dance.h"
#include "dancedef.h"   
#include "toon.h"

ITEMID idSoundTrack;

// Callback used to get sprite messages
void CALLBACK OnSpriteNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData);

class FAR CDanceParser : public CParser
{
public:
	CDanceParser(LPSTR lpTableData)
		: CParser(lpTableData) {}

protected:
	BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
};  



//************************************************************************
CDanceScene::CDanceScene(int nNextSceneNo) : CGBScene(nNextSceneNo)
//************************************************************************
{
	//m_hSound = NULL;
	m_pSound = NULL;
	m_bPlaying = FALSE;  
	lstrcpy(szSndTrack, "sndtrack.wav");  
	idSoundTrack = 9900;
	m_bIntroPlaying	= FALSE;   
	m_lpGoatSprite	 = NULL;   
	
	szGoat[0] = '\0';
	szIntro[0] = '\0';
	szSuccess[0] = '\0';
	
	for(int i=0;i<NUM_STEPS;i++)
	{
		m_lpButSprites[i] = NULL; 
	}     
}

//************************************************************************
CDanceScene::~CDanceScene()
//************************************************************************
{
	if (m_pSound)
		delete m_pSound;  
		
	if (m_lpGoatSprite)
	{
		delete m_lpGoatSprite;
		m_lpGoatSprite = NULL;
	}
		
	for(int i=0;i<NUM_STEPS;i++)
	{               
		if (m_lpButSprites[i])
			delete m_lpButSprites[i]; 
	}     
}

//************************************************************************
int CDanceScene::GetLevel()
//************************************************************************
{                                          
	if (m_nSceneNo == IDD_DANCEI)
		return(0);
	else
		return(m_nSceneNo - IDD_DANCE1);
}

//************************************************************************
BOOL CDanceScene::OnInitDialog( HWND hWnd, HWND hWndControl, LPARAM lParam )
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
			CDanceParser parser(lpTableData);
			short i = parser.GetNumEntries();
			
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
void CDanceScene::ToonInitDone()
//************************************************************************
{
	GetToon()->SetHintState(TRUE);
	InitGame();
	PlayIntro();
}

//************************************************************************
void CDanceScene::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	if (m_bIntroPlaying)
	{
		m_bIntroPlaying = FALSE;
		GetSound()->StopChannel(2);
	}
	
	switch (id)
	{   
		// Source dance move buttons
		case IDC_SRCMOVE1:
		case IDC_SRCMOVE2:
		case IDC_SRCMOVE3:
		case IDC_SRCMOVE4:
		case IDC_SRCMOVE5:
		case IDC_SRCMOVE6:
		case IDC_SRCMOVE7:
		case IDC_SRCMOVE8: 
		                   
			// If playing a dance then don't set steps
			if (m_bPlaying)
				break;
						                   
			// If we can add another dance step/move
			if (m_nNumSteps < NUM_STEPS)
			{     
				POINT ptOrigin ;     
				FNAME szFileName;  
				int i = id - IDC_SRCMOVE1; 
				int iLevel = GetLevel();
					
				// Set the play segments
				m_PlaySegs[m_nNumSteps].iStart = SrcButt[i].iStart;
				m_PlaySegs[m_nNumSteps].iEnd   = SrcButt[i].iEnd;
					                    
				if (iLevel > 0)
				{					                    
					// Create a step sprite and move to top of the screen
					ptOrigin.x = 0;
					ptOrigin.y = 0;
					m_lpStepSprites[m_nNumSteps] = m_pAnimator->CreateSprite( &ptOrigin );
					GetPathName(szFileName, SrcButt[i].szButton);
			 		m_lpStepSprites[m_nNumSteps]->AddCell( szFileName, NULL );
			 		m_lpStepSprites[m_nNumSteps]->SetSpeed(300);  
			 		m_lpStepSprites[m_nNumSteps]->Jump(SrcButt[i].x, SrcButt[i].y);
					m_lpStepSprites[m_nNumSteps]->Show(TRUE);    
					                                    
					// Play the sound effect for the button flying up	
					STRING szUp;	
					wsprintf(szUp, "butup%d.bmp", i+1);		
					GetSound()->StopChannel(1);
					GetSound()->StartFile( GetPathName(szFileName, szUp), NO, NORMAL_CHANNEL, TRUE );
					m_lpStepSprites[m_nNumSteps]->Move(DstButt[m_nNumSteps].x, DstButt[m_nNumSteps].y);  
					GetSound()->Pump();
		                 
					// Get destination bitmap name
					STRING szName;  
					switch (i)
					{                  
						case 0:
							lstrcpy(szName, "done.bmp");     
							break;
						case 1:
							lstrcpy(szName, "dtwo.bmp");
							break;
						case 2:
							lstrcpy(szName, "dthree.bmp");
							break;
						case 3:
							lstrcpy(szName, "dfour.bmp");
							break;
						case 4:
							lstrcpy(szName, "dfive.bmp");
							break;
						case 5:
							lstrcpy(szName, "dsix.bmp");
							break;
						case 6:
							lstrcpy(szName, "dseven.bmp");
							break;
						case 7:
							lstrcpy(szName, "deight.bmp");
							break;
						default:
							szName[0] = NULL;
						
					}                            
					       
					// Get the x,y for where the destination button is supposed to be				                                                          	                              
					int x = DstButt[m_nNumSteps].x+1;
					int y = DstButt[m_nNumSteps].y+1;
		
					// Set the right bitmap for the destination button/hotspot								                                                                                        
					GetToon()->SetHotSpotFG(x, y, szName);
													           
					++m_nNumSteps;   
				}
				else
				{    
					++m_nNumSteps;
					Play(hWnd);
				}
					
				return;
			}
			break;
			       
		case IDC_PLAY: 
			Play(hWnd);
			break;  
			
		case IDC_STOP:
			if (m_bPlaying)
			{                               
				int iLevel = GetLevel();
				
				MCIStop(m_wDeviceId, TRUE);
				GetSound()->StopChannel(3);
				m_bPlaying = FALSE;   
				
				// Set play button to play
				if (iLevel > 0)
				{
					int x = PlayBut.x + 1;
					int y = PlayBut.y + 1;
					GetToon()->SetHotSpotFG(x, y, PlayBut.szButton);
					GetToon()->DrawHotSpot(x, y, TRUE, TRUE);
				}
				
				// Make sure the background get's repainted, this will clear the last frame out
				GetToon()->RestoreBackgroundArea(NULL, TRUE);
				// Now paint the goat sprite 
				PostDrawGoatCmd(hWnd);		
			}
			break;
			
		case IDC_CLEAR:   
		{       
			OnCommand(hWnd, IDC_STOP, hControl, codeNotify);

			FNAME szFileName;
			GetSound()->Stop();
			GetSound()->StartFile( GetPathName(szFileName, "clear.wav"), NO, 4, FALSE );
			ClearSteps();
			GetSound()->Pump();
			break;
		}

		case IDC_HELP:
		{
			PlayIntro();
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

		case IDC_MUSIC1:
		case IDC_MUSIC2:
		case IDC_MUSIC3:
		{               
			int iMusic = id-IDC_MUSIC1;
			if (iMusic == 0)
			{
				lstrcpy(szSndTrack, "sndtrack.wav");
				idSoundTrack = 9900;
			}
			else if (iMusic == 1)
			{
				lstrcpy(szSndTrack, "sndtrck1.wav");
				idSoundTrack = 9901;
			}
			else if (iMusic == 2)
			{
				lstrcpy(szSndTrack, "sndtrck2.wav");    
				idSoundTrack = 9902;
			}
			if ( GetSound() )
			{
				FNAME szFileName;
				GetSound()->Stop();
				if (m_bPlaying)            
					// Start playing the dance music
					GetSound()->StartResource( (LPSTR)MAKEINTRESOURCE(idSoundTrack), YES/*bLoop*/, 3, GetApp()->GetInstance(), FALSE/*bWait*/ );
//					GetSound()->StartFile( GetPathName(szFileName, szSndTrack), YES/*bLoop*/, 3, FALSE/*bWait*/ );
				else
					GetSound()->StartFile( GetPathName(szFileName, "click.wav"), NO/*bLoop*/, 3, TRUE/*bWait*/ );
			}
			break;
		}                 
		    
		// Bogus control (doesn't exist) used for redrawing goat sprite
		case IDC_DRAWGOAT:       
		{
				m_lpGoatSprite->Jump(nGoatX, nGoatY);
				RECT rect;
				m_lpGoatSprite->Location(&rect);
				InflateRect( &rect, 10, 10 );
				m_pAnimator->DrawRect( &rect );
			break;
		}

		case IDC_TOON:
		{
			CGBScene::OnCommand (hWnd, id, hControl, codeNotify);
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
BOOL CDanceScene::OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************
{
	switch (msg)
	{
        HANDLE_DLGMSG(hDlg, WM_LBUTTONDOWN, OnLButtonDown);  
        HANDLE_DLGMSG(hDlg, WM_LBUTTONUP, OnLButtonUp);  
        HANDLE_DLGMSG(hDlg, WM_MOUSEMOVE, OnMouseMove);  
		HANDLE_DLGMSG(hDlg, MM_MCINOTIFY, OnMCINotify);

		default:
			return(FALSE);
	}
}

//************************************************************************
void CDanceScene::OnLButtonDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
//************************************************************************
{
}
            
//************************************************************************
void CDanceScene::ClearSteps(void)
//************************************************************************
{     
	int i;                               
	int x, y;
	
	for( i=0; i<m_nNumSteps; i++ )
	{
		m_pAnimator->DeleteSprite(m_lpStepSprites[i]);
		
		x = DstButt[i].x+1;
		y = DstButt[i].y+1;
		GetToon()->DrawHotSpot(x, y, FALSE);
	}
	
	m_nNumSteps = 0;
}
            
//************************************************************************
void CDanceScene::OnLButtonUp(HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{
}

//************************************************************************
void CDanceScene::OnMouseMove(HWND hWnd, int x, int y, UINT keyFlags)
//************************************************************************
{

}

//***************************************************************************
UINT CDanceScene::OnMCINotify( HWND hWindow, UINT codeNotify, HMCI hDevice )
//***************************************************************************
{
	if (codeNotify == MCI_NOTIFY_SUCCESSFUL)
	{
		 if (hDevice == m_wDeviceId)
		{       
		
			// If there are more dance steps/moves                  
			if (m_nCurStep < m_nNumSteps)
			{                                     
			
				// Play the next move
				BOOL bRet = MCIPlay(m_wDeviceId, hWindow, m_PlaySegs[m_nCurStep].iStart, 
					m_PlaySegs[m_nCurStep].iEnd);
				++m_nCurStep;
			}                        
			
			// Otherwise, the last step has been played
			else                       
			{              
				FNAME szFileName;  
				int iLevel = GetLevel();
				
				m_nCurStep = 0;         
				m_bPlaying = FALSE;
				               
				if (iLevel > 0)
				{				                  
					int x = PlayBut.x + 1;
					int y = PlayBut.y + 1;
					GetToon()->SetHotSpotFG(x, y, PlayBut.szButton);
					GetToon()->DrawHotSpot(x, y, TRUE, TRUE);
				}
				                  
				// Start playing applause
				GetSound()->StartFile( GetPathName(szFileName, szSuccess), 
					NO/*bLoop*/, 2/*iChannel*/, FALSE, m_hWnd);

				// Now stop the dance music ()
				GetSound()->StopChannel(3);   
					                                                            
				// Fix the background so the goat is standing there like
				// he is supposed to be. Otherwise he will have to be
				// beaten by the old man's hickory stick			                                                            
				GetToon()->RestoreBackgroundArea(NULL, FALSE);
				PostDrawGoatCmd(hWindow);	
				
				if (GetLevel() == 0)
					m_nNumSteps = 0;			
			}
		}             
	}
	return(TRUE);
}

//************************************************************************
void CDanceScene::PlayIntro()
//************************************************************************
{
	if (GetSound())
	{
		GetSound()->StopChannel (INTRO_CHANNEL);
		FNAME szFileName;
		GetSound()->StartFile( GetPathName(szFileName, szIntro), NO/*bLoop*/, INTRO_CHANNEL/*iChannel*/, FALSE );
		m_bIntroPlaying = TRUE;
	}
}
	                           
//************************************************************************
void CDanceScene::InitGame()
//************************************************************************
{
	// Create goat sprite   
	if (m_lpGoatSprite == NULL)
	{		       
		POINT ptOrigin;
		short i;
	              
		ptOrigin.x = 0;
		ptOrigin.y = 0;

		m_lpGoatSprite = m_pAnimator->CreateSprite( &ptOrigin );
		FNAME szFileName;
		GetPathName(szFileName, szGoat);
		m_lpGoatSprite->AddCell(szFileName, NULL);
		m_lpGoatSprite->Jump(nGoatX, nGoatY);
		m_lpGoatSprite->Show(TRUE);
	
		// Create button sprites
		for(i=0;i<NUM_STEPS;i++)
		{
			m_lpButSprites[i] = m_pAnimator->CreateSprite( &ptOrigin ); 
			m_lpButSprites[i]->SetNotifyProc(OnSpriteNotify, (DWORD)this);
			GetPathName(szFileName, SrcButt[i].szButton);
	 		m_lpButSprites[i]->AddCell( szFileName, NULL );
			m_lpButSprites[i]->Jump(SrcButt[i].x,SrcButt[i].y);
			m_lpButSprites[i]->Show(TRUE);
		}     
	}
		
	m_nNumSteps = 0;
	m_nCurStep = 0;
	                             
	m_pAnimator->StartAll();	

}

//************************************************************************
void CDanceScene::ChangeLevel(int iLevel)
//************************************************************************
{
	if (iLevel >= NUM_LEVELS)
		iLevel = NUM_LEVELS-1; 
			
	App.GotoScene(m_hWnd, IDD_DANCE1+iLevel, m_nNextSceneNo);
}

     
//************************************************************************
void CDanceScene::PostDrawGoatCmd(HWND hWnd)	
//************************************************************************  
{
	PostMessage(hWnd, WM_COMMAND, IDC_DRAWGOAT, 0);
}
     
//************************************************************************  
void CDanceScene::Play(HWND hWnd)
//************************************************************************  
{
 	FNAME szFileName;  
	int x, y, i;  
	int iLevel = GetLevel();
			                 
	// Something to play?			                 
	if (m_nNumSteps == 0)
		return;
				         
	// If currently playing then stop							         
	if (m_bPlaying)            
	{
		MCIStop(m_wDeviceId, TRUE);     
		m_bPlaying = FALSE;
		GetSound()->StopChannel(3);
				                         
		if (iLevel > 0)
		{
			// Set play button bitmap to play
			x = PlayBut.x + 1;
			y = PlayBut.y + 1;
			GetToon()->SetHotSpotFG(x, y, PlayBut.szButton);  
			GetToon()->DrawHotSpot(x, y, TRUE, TRUE);
		}
		
		// Make sure the background get's repainted, this will clear the last frame out
		GetToon()->RestoreBackgroundArea(NULL, TRUE);
		// Now paint the goat sprite 
		PostDrawGoatCmd(hWnd);	
				    
		if (iLevel > 0)				    
			return;	
	}
					      
	// Set play button bitmap to stop
	if (iLevel > 0)
	{
		x = PlayBut.x + 1;
		y = PlayBut.y + 1;
		GetToon()->SetHotSpotFG(x, y, StopBut.szButton);
		GetToon()->DrawHotSpot(x, y, TRUE, TRUE);
	}
				
	// Show all destination buttons so they won't get painted over by the video 
	if (iLevel > 0)
	{
		for( i=0; i<m_nNumSteps; i++ )
		{
			x = DstButt[i].x+1;
			y = DstButt[i].y+1;
			GetToon()->DrawHotSpot(x, y, TRUE);
		}
	}
						
	// Start playing the dance music 					                         
	GetSound()->Stop();
	GetSound()->StartResource( (LPSTR)MAKEINTRESOURCE(idSoundTrack), YES, 3, GetApp()->GetInstance(), FALSE );
//	GetSound()->StartFile( GetPathName(szFileName, szSndTrack), YES, 3, FALSE );
			                         
	GetPathName(szFileName, "toon.avi");
	m_wDeviceId = GetToon()->GetToonMovie();

	// Can't hurt to let wavemix have a little time	
	GetSound()->Pump();
			
	if (m_wDeviceId)
	{                                              
		// Place the toon
		BOOL bRet = MCISetDestination(m_wDeviceId, 0, 0);                       
		
		// Clip the toon
		bRet = MCIClip(m_wDeviceId, NULL, &GetToon()->m_rGlobalClip, 1);
				
		// Play the first dance move
		bRet = MCIPlay(m_wDeviceId, hWnd, m_PlaySegs[0].iStart, m_PlaySegs[0].iEnd);
		++m_nCurStep;
		m_bPlaying = TRUE;	         
				
	}
}     

//************************************************************************
BOOL CDanceParser::HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	LPDANCESCENE lpScene = (LPDANCESCENE)dwUserData;

	if (!lstrcmpi(lpKey, "intro"))
	{
		GetFilename(lpScene->szIntro, lpEntry, lpValues, nValues);
		return TRUE;
	}
	
	if (!lstrcmpi(lpKey, "success"))
	{
		GetFilename(lpScene->szSuccess, lpEntry, lpValues, nValues);
		return TRUE;
	}
	
	if (!lstrcmpi(lpKey, "goatbmp"))
	{
		GetFilename(lpScene->szGoat, lpEntry, lpValues, nValues);
		return TRUE;
	}
	
	if (!lstrcmpi(lpKey, "buttonone"))
	{
		GetFilename(lpScene->SrcButt[0].szButton, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "buttontwo"))
	{
		GetFilename(lpScene->SrcButt[1].szButton, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "buttonthree"))
	{
		GetFilename(lpScene->SrcButt[2].szButton, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "buttonfour"))
	{
		GetFilename(lpScene->SrcButt[3].szButton, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "buttonfive"))
	{
		GetFilename(lpScene->SrcButt[4].szButton, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "buttonsix"))
	{
		GetFilename(lpScene->SrcButt[5].szButton, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "buttonseven"))
	{
		GetFilename(lpScene->SrcButt[6].szButton, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "buttoneight"))
	{
		GetFilename(lpScene->SrcButt[7].szButton, lpEntry, lpValues, nValues);
		return TRUE;
	}
		
	if (!lstrcmpi(lpKey, "play"))
	{
		GetFilename(lpScene->PlayBut.szButton, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "stop"))
	{
		GetFilename(lpScene->StopBut.szButton, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "clear"))
	{
		GetFilename(lpScene->ClearBut.szButton, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "playx"))
	{
		GetInt(&lpScene->PlayBut.x, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "playy"))
	{
		GetInt(&lpScene->PlayBut.y, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "stopx"))
	{
		GetInt(&lpScene->StopBut.x, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "stopy"))
	{
		GetInt(&lpScene->StopBut.y, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "clearx"))
	{
		GetInt(&lpScene->ClearBut.x, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "cleary"))
	{
		GetInt(&lpScene->ClearBut.y, lpEntry, lpValues, nValues);
		return TRUE;
	}
	
   	if (!lstrcmpi(lpKey, "step1"))
	{
		GetPoint((LPPOINT)&lpScene->SrcButt[0].iStart, lpEntry, lpValues, nValues);
		return TRUE;
	}

    if (!lstrcmpi(lpKey, "step2"))
	{
		GetPoint((LPPOINT)&lpScene->SrcButt[1].iStart, lpEntry, lpValues, nValues);
		return TRUE;
	}

    if (!lstrcmpi(lpKey, "step3"))
	{
		GetPoint((LPPOINT)&lpScene->SrcButt[2].iStart, lpEntry, lpValues, nValues);
		return TRUE;
	}

    if (!lstrcmpi(lpKey, "step4"))
	{
		GetPoint((LPPOINT)&lpScene->SrcButt[3].iStart, lpEntry, lpValues, nValues);
		return TRUE;
	}

    if (!lstrcmpi(lpKey, "step5"))
	{
		GetPoint((LPPOINT)&lpScene->SrcButt[4].iStart, lpEntry, lpValues, nValues);
		return TRUE;
	}

    if (!lstrcmpi(lpKey, "step6"))
	{
		GetPoint((LPPOINT)&lpScene->SrcButt[5].iStart, lpEntry, lpValues, nValues);
		return TRUE;
	}

    if (!lstrcmpi(lpKey, "step7"))
	{
		GetPoint((LPPOINT)&lpScene->SrcButt[6].iStart, lpEntry, lpValues, nValues);
		return TRUE;
	}

    if (!lstrcmpi(lpKey, "step8"))
	{
		GetPoint((LPPOINT)&lpScene->SrcButt[7].iStart, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "srcbut1x"))
	{
		GetInt(&lpScene->SrcButt[0].x, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "srcbut1y"))
	{
		GetInt(&lpScene->SrcButt[0].y, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "srcbut2x"))
	{
		GetInt(&lpScene->SrcButt[1].x, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "srcbut2y"))
	{
		GetInt(&lpScene->SrcButt[1].y, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "srcbut3x"))
	{
		GetInt(&lpScene->SrcButt[2].x, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "srcbut3y"))
	{
		GetInt(&lpScene->SrcButt[2].y, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "srcbut4x"))
	{
		GetInt(&lpScene->SrcButt[3].x, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "srcbut4y"))
	{
		GetInt(&lpScene->SrcButt[3].y, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "srcbut5x"))
	{
		GetInt(&lpScene->SrcButt[4].x, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "srcbut5y"))
	{
		GetInt(&lpScene->SrcButt[4].y, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "srcbut6x"))
	{
		GetInt(&lpScene->SrcButt[5].x, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "srcbut6y"))
	{
		GetInt(&lpScene->SrcButt[5].y, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "srcbut7x"))
	{
		GetInt(&lpScene->SrcButt[6].x, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "srcbut7y"))
	{
		GetInt(&lpScene->SrcButt[6].y, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "srcbut8x"))
	{
		GetInt(&lpScene->SrcButt[7].x, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "srcbut8y"))
	{
		GetInt(&lpScene->SrcButt[7].y, lpEntry, lpValues, nValues);
		return TRUE;
	}
		
	if (!lstrcmpi(lpKey, "dstbut1x"))
	{
		GetInt(&lpScene->DstButt[0].x, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "dstbut1y"))
	{
		GetInt(&lpScene->DstButt[0].y, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "dstbut2x"))
	{
		GetInt(&lpScene->DstButt[1].x, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "dstbut2y"))
	{
		GetInt(&lpScene->DstButt[1].y, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "dstbut3x"))
	{
		GetInt(&lpScene->DstButt[2].x, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "dstbut3y"))
	{
		GetInt(&lpScene->DstButt[2].y, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "dstbut4x"))
	{
		GetInt(&lpScene->DstButt[3].x, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "dstbut4y"))
	{
		GetInt(&lpScene->DstButt[3].y, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "dstbut5x"))
	{
		GetInt(&lpScene->DstButt[4].x, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "dstbut5y"))
	{
		GetInt(&lpScene->DstButt[4].y, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "dstbut6x"))
	{
		GetInt(&lpScene->DstButt[5].x, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "dstbut6y"))
	{
		GetInt(&lpScene->DstButt[5].y, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "dstbut7x"))
	{
		GetInt(&lpScene->DstButt[6].x, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "dstbut7y"))
	{
		GetInt(&lpScene->DstButt[6].y, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "dstbut8x"))
	{
		GetInt(&lpScene->DstButt[7].x, lpEntry, lpValues, nValues);
		return TRUE;
	}

	if (!lstrcmpi(lpKey, "dstbut8y"))
	{
		GetInt(&lpScene->DstButt[7].y, lpEntry, lpValues, nValues);
		return TRUE;
	}
	
	if (!lstrcmpi(lpKey, "goatx"))
	{
		GetInt(&lpScene->nGoatX, lpEntry, lpValues, nValues);
		return TRUE;
	}
	
	if (!lstrcmpi(lpKey, "goaty"))
	{
		GetInt(&lpScene->nGoatY, lpEntry, lpValues, nValues);
		return TRUE;
	}
		
	return TRUE;
}

//************************************************************************
void CALLBACK OnSpriteNotify(LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData)
//************************************************************************
{                                    
	if (dwNotifyData)
	{
		// Make sure wave mix keeps busy
		CDanceScene *lpScene = (CDanceScene *)dwNotifyData;
		lpScene->GetSound()->Pump();
	}
	
}
