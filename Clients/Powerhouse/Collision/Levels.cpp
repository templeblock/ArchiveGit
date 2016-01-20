#include <windows.h>
#include "proto.h"
#include "Levels.h"
#include "cllsnid.h"
#include "cllsndef.h"
#include "cllsn.h"
#include "opponents.h"

// Level 1 Maximums
#define MAX_LEVEL1_EVENTS		1
#define MAX_LEVEL1_STATES		2

// Level 1 Events
#define L1E_FOUNDKEY			0	// Found guitar key

// Level 1 States
#define L1S_START				0   // Start state
#define L1S_HAVE_KEY			1	// Have the guitar key

// Level 1 state table.
int Level1States[MAX_LEVEL1_STATES][MAX_LEVEL1_EVENTS] = 
{
/* Events ----> 0 */
/* State 0 */	1,
/*		 1 */   1
};

// Level 2 Maximums
#define MAX_LEVEL2_EVENTS		3
#define MAX_LEVEL2_STATES		4

// Level 2 Events
#define L2E_GET_FIREEXT			0	// Picked up fire extinguisher
#define L2E_GET_EXCAL			1	// Picked up Excalibur's ghost
#define L2E_LOSE_FIREEXT		2   // Lost fire extinguisher

// Level 2 States
#define L2S_START				0   // Start state
#define L2S_HAVE_EXT			1	// Have the fire extinguisher
#define L2S_HAVE_EXCAL			2	// Have Excalibur's ghost and fire extinguisher
#define L2S_HAVE_EXCALNOEXT		3	// Have Excalibur's ghost and no fire extinguisher

// Level 2 state table.
int Level2States[MAX_LEVEL2_STATES][MAX_LEVEL2_EVENTS] = 
{
/* Events ----> 0,  1,	2 */
/* State 0 */	1,	3,	0,
/*		 1 */   1,  2,	0,
/*       2 */   2,  2,	3,
/*		 3 */   2,  3,  3
};


// Level 3 Maximums
#define MAX_LEVEL3_EVENTS		3
#define MAX_LEVEL3_STATES		4

// Level 3 Events
#define L3E_GET_FIREEXT			0	// Picked up fire extinguisher
#define L3E_KILLEDALL			1	// Killed all opponents
#define L3E_LOSE_FIREEXT		2   // Lost fire extinguisher

// Level 3 States
#define L3S_START				0	// Start state
#define L3S_HAVE_EXT			1	// Have the fire extinguisher
#define L3S_ALL_DEAD			3	// All opponents have been killed and have fire ext.
#define L3S_ALL_DEAD_NE			4	// All dead but no fire ext.

// Level 3 state table
int Level3States[MAX_LEVEL3_STATES][MAX_LEVEL3_EVENTS] =
{
/* Events ----> 0,  1,	2 */
/* State 0 */	1,	3,  1,
/*		 1 */   1,  2,  1,
/*       2 */   2,  2,  3,
/*		 3 */	2,  3,	3
};

// Level 4 Maximums
#define MAX_LEVEL4_EVENTS		6
#define MAX_LEVEL4_STATES		7

// Level 4 Events
#define L4E_FOUNDNOTE1			0	// Found note 1
#define L4E_FOUNDNOTE2			1	// Found note 2
#define L4E_FOUNDNOTE3			2	// Found note 3
#define L4E_FOUNDNOTE4			3	// Found note 4
#define L4E_FOUNDNOTE5			4	// Found note 5
#define L4E_FOUNDNOTE6			5	// Found note 6


// Level 4 States
#define L4S_START				0   // Start state
#define L4S_HAVE_NOTE1			1	// Have note 1
#define L4S_HAVE_NOTE2			2	// Have note 2
#define L4S_HAVE_NOTE3			3	// Have note 3
#define L4S_HAVE_NOTE4			4	// Have note 4
#define L4S_HAVE_NOTE5			5	// Have note 5
#define L4S_HAVE_NOTE6			6	// Have note 6

// Level 4 state table.
int Level4States[MAX_LEVEL4_STATES][MAX_LEVEL4_EVENTS] = 
{
/* Events ----> 0	1	2	3	4	5 */
/* State 0 */	0,	1,	1,	1,	1,	1,
/*		 1 */   0,	2,	2,	2,	2,	2,
/*       2 */	0,	3,	3,	3,	3,	3,
/*       3 */	0,	4,	4,	4,	4,	4,
/*       4 */	0,	5,	5,	5,	5,	5,
/*       5 */	0,	6,	6,	6,	6,	6,
/*       6 */	0,	6,	6,	6,	6,	6
};

// Level 5 Maximums
#define MAX_LEVEL5_EVENTS		1
#define MAX_LEVEL5_STATES		4

// Level 5 Events
#define L5E_HILT				0	// Around the hilt 

// Level 5 States
#define L5S_START				0   // Start state
#define L5S_HILT_1				1	// Around the hilt once
#define L5S_HILT_2				2	// Around the hilt twice
#define L5S_HILT_3				3	// Around the hilt thrice

// Level 5 state table.
int Level5States[MAX_LEVEL5_STATES][MAX_LEVEL5_EVENTS] = 
{
/* Events ----> 0	*/
/* State 0 */	1,	
/*		 1 */   2,	
/*       2 */	3,	
/*       3 */	3
};

// Level 6 Maximums
#define MAX_LEVEL6_EVENTS		7
#define MAX_LEVEL6_STATES		8

// Level 6 Events
#define L6E_FOUNDLET1			0	// Found letters 1 - 7
#define L6E_FOUNDLET2			1	
#define L6E_FOUNDLET3			2	
#define L6E_FOUNDLET4			3	
#define L6E_FOUNDLET5			4	
#define L6E_FOUNDLET6			5	
#define L6E_FOUNDLET7			6	


// Level 6 States
#define L6S_START				0   // Start state
#define L6S_HAVE_LET1			1	// Have LET 1
#define L6S_HAVE_LET2			2	// Have LET 2
#define L6S_HAVE_LET3			3	// Have LET 3
#define L6S_HAVE_LET4			4	// Have LET 4
#define L6S_HAVE_LET5			5	// Have LET 5
#define L6S_HAVE_LET6			6	// Have LET 6
#define L6S_HAVE_LET7			7	// Have LET 7

// Level 6 state table.
int Level6States[MAX_LEVEL6_STATES][MAX_LEVEL6_EVENTS] = 
{
/* Events ----> 0	1	2	3	4	5	6 */
/* State 0 */	0,	1,	1,	1,	1,	1,  1,
/*		 1 */   0,	2,	2,	2,	2,	2,	2,
/*       2 */	0,	3,	3,	3,	3,	3,	3,
/*       3 */	0,	4,	4,	4,	4,	4,	4,
/*       4 */	0,	5,	5,	5,	5,	5,	5,
/*       5 */	0,	6,	6,	6,	6,	6,	6,
/*       6 */	0,	7,	7,	7,	7,	7,	7,
/*		 7 */   0,	7,	7,	7,	7,	7,	7
};

// Level 7 Maximums
#define MAX_LEVEL7_EVENTS		5
#define MAX_LEVEL7_STATES		6

// Level 7 Events
#define L7E_FOUND1			0	// Found pieces 1 - 5
#define L7E_FOUND2			1	
#define L7E_FOUND3			2	
#define L7E_FOUND4			3	
#define L7E_FOUND5			4	


// Level 7 States
#define L7S_START			0   // Start state
#define L7S_HAVE_1			1	
#define L7S_HAVE_2			2	
#define L7S_HAVE_3			3	
#define L7S_HAVE_4			4	
#define L7S_HAVE_5			5	

// Level 7 state table.
int Level7States[MAX_LEVEL7_STATES][MAX_LEVEL7_EVENTS] = 
{
/* Events ----> 0	1	2	3	4	*/
/* State 0 */	1,	1,	1,	1,	1,	
/*		 1 */   2,	2,	2,	2,	2,	
/*       2 */	3,	3,	3,	3,	3,	
/*       3 */	4,	4,	4,	4,	4,	
/*       4 */	5,	5,	5,	5,	5,	
/*       5 */	5,	5,	5,	5,	5
};

// Level 8 Maximums
#define MAX_LEVEL8_EVENTS		2
#define MAX_LEVEL8_STATES		9

// Level 8 Events
#define L8E_KILLREINDEER	0	// Killed one of the reindeer
#define L8E_KILLRUDOLPH		1	// Killed Rudolph


// Level 8 States
#define L8S_START			0   // Start state
#define L8S_KILLED_1		1	
#define L8S_KILLED_2		2	
#define L8S_KILLED_3		3	
#define L8S_KILLED_4		4	
#define L8S_KILLED_5		5	
#define L8S_KILLED_6		6	
#define L8S_KILLED_7		7	
#define L8S_KILLED_8		8	

// Level 8 state table.
int Level8States[MAX_LEVEL8_STATES][MAX_LEVEL8_EVENTS] = 
{
/* Events ----> 0	1	*/
/* State 0 */	1,	0,
/*		 1 */   2,	0,
/*       2 */	3,	0,
/*       3 */	4,	0,
/*       4 */	5,	0,
/*       5 */	6,	0,
/*       6 */	7,	0,
/*       7 */	8,	0,
/*       8 */	8,	0
};

// Level 9 Maximums
#define MAX_LEVEL9_EVENTS		2
#define MAX_LEVEL9_STATES		5

// Level 9 Events
#define L93_START			0   // start
#define L9E_FOUNDGOGIRL		1	// Found girl

// Level 9 States
#define L9S_START			0   // Start state
#define L9S_HAVE_1			1	
#define L9S_HAVE_2			2	
#define L9S_HAVE_3			3	
#define L9S_HAVE_4			4	

// Level 9 state table.
int Level9States[MAX_LEVEL9_STATES][MAX_LEVEL9_EVENTS] = 
{
/* Events ----> 0	1 */
/* State 0 */	0,	1,
/*		 1 */   0,	2,
/*       2 */	0,	3,
/*       3 */	0,	4,
/*       4 */	0,	5
};

// Level 10 Maximums
#define MAX_LEVEL10_EVENTS		1
#define MAX_LEVEL10_STATES		2

// Level 10 Events
#define L10E_SHOTGATEKEEPER	0	// Shot the gate keeper

// Level 10 States
#define L10S_START			0   // Start state
#define L10S_SHOTHIM		1	

// Level 10 state table.
int Level10States[MAX_LEVEL10_STATES][MAX_LEVEL10_EVENTS] = 
{
/* Events ----> 0	*/
/* State 0 */	1,	
/*		 1 */   1
};

// Level 11 Maximums
#define MAX_LEVEL11_EVENTS		1
#define MAX_LEVEL11_STATES		2

// Level 11 Events
#define L11E_GOTENOUGHSKS	 0	// Picked up enough spine killers

// Level 11 States
#define L11S_START			0   // Start state
#define L11S_HAVEENOUGH		1	// Have enouph spine killers

// Level 11 state table.
int Level11States[MAX_LEVEL11_STATES][MAX_LEVEL11_EVENTS] = 
{
/* Events ----> 0	*/
/* State 0 */	1,	
/*		 1 */   1,	
};

// Level 12 Maximums
#define MAX_LEVEL12_EVENTS		10
#define MAX_LEVEL12_STATES		11

// Level 12 Events
#define L12E_GOT1		0	// Collectable 1
#define L12E_GOT2		1	// Collectable 1
#define L12E_GOT3		2	// Collectable 1
#define L12E_GOT4		3	// Collectable 1
#define L12E_GOT5		4	// Collectable 1
#define L12E_GOT6		5	// Collectable 1
#define L12E_GOT7		6	// Collectable 1
#define L12E_GOT8		7	// Collectable 1
#define L12E_ALLDEAD	8   // All drones killed
#define L12E_SPINEDEAD	9	// Spine killed

// Level 12 States
#define L12S_START			0   // Start state
#define L12S_HAVE1			1	// Have collectable 1
#define L12S_HAVE2			2	// Have collectable 2
#define L12S_HAVE3			3	// Have collectable 3
#define L12S_HAVE4			4	// Have collectable 4
#define L12S_HAVE5			5	// Have collectable 5
#define L12S_HAVE6			6	// Have collectable 6
#define L12S_HAVE7			7	// Have collectable 7
#define L12S_HAVE8			8	// Have collectable 8
#define L12S_ALLDEAD	    9   // All drones killed
#define L12S_SPINEDEAD	   10	// Spined has been killed

// Level 12 state table.
int Level12States[MAX_LEVEL12_STATES][MAX_LEVEL12_EVENTS] = 
{
/* Events ----> 0	1	2	3	4	5	6	7	8	9	*/
/* State 0 */	1,	1,	1,	1,	1,	1,	1,	1,	0,	0,
/*		 1 */   2,	2,	2,	2,	2,	2,	2,	2,	1,	1,
/*		 2 */	3,	3,	3,	3,	3,	3,	3,	3,	2,	2,
/*       3 */	4,	4,	4,	4,	4,	4,	4,	4,	3,	3,
/*       4 */	5,	5,	5,	5,	5,	5,	5,	5,	4,	4,
/*       5 */	6,	6,	6,	6,	6,	6,	6,	6,	5,	5,
/*		 6 */	7,	7,	7,	7,	7,	7,	7,	7,	6,	6,
/*		 7 */	8,	8,	8,	8,	8,	8,	8,	8,	7,	7,
/*		 8 */	8,	8,	8,	8,	8,	8,	8,	8,	9,	8,
/*       9 */	9,  9,  9,  9,  9,  9,  9,  9,  9,  9
};


//***********************************************************************
CLevelState::CLevelState()
//***********************************************************************
{
	Levels[0]	= new CLevel1();
	Levels[1]	= new CLevel2();
	Levels[2]	= new CLevel3();
	Levels[3]	= new CLevel4();
	Levels[4]	= new CLevel5();
	Levels[5]	= new CLevel6();
	Levels[6]	= new CLevel7();
	Levels[7]	= new CLevel8();
	Levels[8]	= new CLevel9();
	Levels[9]	= new CLevel10();
	Levels[10]	= new CLevel11();
	Levels[11]	= new CLevel12();

	// Set start level
	SetLevel(1);
}

//***********************************************************************
CLevelState::~CLevelState()
//***********************************************************************
{
	for(int i=0; i<MAX_LEVELS; i++)
		delete Levels[i];
}

//***********************************************************************
BOOL CLevelState::SetState(int nEvent, DWORD dwValue)
//***********************************************************************
{
	if (nEvent < 0 || nEvent > nMaxEvents)
		return FALSE;

	return m_pLevel->OnEvent(GState, nEvent, dwValue);
}

//***********************************************************************
BOOL CLevelState::SetLevel(int nLevel)
//***********************************************************************
{
	// Zero based adjusment to level
	m_nLevel = nLevel - 1;

	if (m_nLevel < 0 || m_nLevel > MAX_LEVELS)
		return FALSE;

	m_pLevel = Levels[m_nLevel];

	GState.SetStateTbl(m_pLevel->GetTbl(), m_pLevel->GetEventMax(),
		m_pLevel->GetStateMax());

	GState.SetObserver(m_pLevel);

	return TRUE;
}

//***********************************************************************
int CLevelState::GetLevel(void)
//***********************************************************************
{
	return( m_nLevel );
}

//***********************************************************************
BOOL CLevelState::HaveFireExtinguisher(void)
//***********************************************************************
{
	return m_pLevel->HaveFireExtinguisher();
}

//***********************************************************************
BOOL CLevelState::HaveGateKey(void)
//***********************************************************************
{
	return m_pLevel->HaveGateKey();
}

//***********************************************************************
CLevel::CLevel(int nNum, int *pStateTbl, int nEventMax, int nStateMax)
//***********************************************************************
{
	m_nNum = nNum;
	m_pStateTbl = pStateTbl;
	m_nEventMax = nEventMax;
	m_nStateMax = nStateMax;

	m_bHaveExtinguisher = FALSE;
	m_bHaveGateKey = FALSE;
	m_nUseData1 = 0;
}

//***********************************************************************
void CLevel::ShowText(char *szText)
//***********************************************************************
{
	VideoFx *pfx = App.pGame->pVidFx;
	if (pfx && App.pGame->hVideo)
	{
		HWND hSceneWnd = GetParent(App.pGame->hVideo);
		pfx->TextOn(hSceneWnd, szText, RGB(255,255,  0), 18, 5000, 5);
	}
}

//***********************************************************************
CLevel1::CLevel1() : CLevel(1, (int *)Level1States, MAX_LEVEL1_EVENTS, MAX_LEVEL1_STATES)
//***********************************************************************
{
}

//***********************************************************************
BOOL CLevel1::OnEvent(GameState& State, int nEvent, DWORD dwValue)
//***********************************************************************
{
	switch(nEvent)
	{
		// Collectable 1 - Got the Guitar key (key to getting out)
		case LEVENT_GET_COLL:
		{
			if (dwValue == 1)
			{
				ShowText("You got the key!");
				return State.SetState(L1E_FOUNDKEY);
			}
			else
				return FALSE;
		}
	}

	return FALSE;
}

//***********************************************************************
void CLevel1::OnStateChange(int nState)
//***********************************************************************
{
	if (nState == L1S_HAVE_KEY)
		m_bHaveGateKey = TRUE;
}

//***********************************************************************
CLevel2::CLevel2() : CLevel(2, (int *)Level2States, MAX_LEVEL2_EVENTS, MAX_LEVEL2_STATES)
//***********************************************************************
{
}

//***********************************************************************
BOOL CLevel2::OnEvent(GameState& State, int nEvent, DWORD dwValue)
//***********************************************************************
{
	switch(nEvent)
	{
		// Collectable 1 - Excaliber's ghost (key to getting out)
		case LEVENT_GET_COLL:
		{
			if (dwValue == 1)
			{
				ShowText("You found Excaliber's ghost!");
				return State.SetState(L2E_GET_EXCAL);
			}
			else if (dwValue == 2)
			{
				ShowText("You got the fire extinguisher!");
				return State.SetState(L2E_GET_FIREEXT);
			}
			else
				return FALSE;
		}

		// Got fire extinguisher
		//case LEVENT_GET_FIREEXT:
		//	{
		//		ShowText("You got the fire extinguisher!");
		//		return State.SetState(L2E_GET_EXCAL);
		//	}

		// Lost fire extinguisher
		case LEVENT_LOSE_FIREEXT:
			return State.SetState(L2E_LOSE_FIREEXT);
	}

	return FALSE;
}

//***********************************************************************
void CLevel2::OnStateChange(int nState)
//***********************************************************************
{
	switch(nState)
	{
		// Got fire extinquisher
		case L2S_HAVE_EXT:
		{
			m_bHaveExtinguisher = TRUE;
			if (App.pGame)
				App.pGame->Collect.SetTimedLife(1, 6, 36000);
			break;
		}

		// Got excaliber while having extinguisher
		case L2S_HAVE_EXCAL:
		{
			m_bHaveGateKey = TRUE;
			m_bHaveExtinguisher = TRUE;
			break;
		}

		// Do have excaliber but do not have extinguisher
		case L2S_HAVE_EXCALNOEXT:
		{
			m_bHaveGateKey = TRUE;
			m_bHaveExtinguisher = FALSE;
			break;
		}

		case L2S_START:
		{
			m_bHaveGateKey = FALSE;
			m_bHaveExtinguisher = FALSE;
			break;
		}
	}
}


//***********************************************************************
CLevel3::CLevel3() : CLevel(3, (int *)Level3States, MAX_LEVEL3_EVENTS, MAX_LEVEL3_STATES)
//***********************************************************************
{
}

static int nKillCount = 0;
//***********************************************************************
BOOL CLevel3::OnEvent(GameState& State, int nEvent, DWORD dwValue)
//***********************************************************************
{
	switch(nEvent)
	{
		// Collectable 1 - Fire extinguisher (must have to get to excaliber's ghost)
		case LEVENT_GET_COLL:
		{
			if (dwValue == 1)
				return State.SetState(L3E_GET_FIREEXT);
			else
				return FALSE;
		}

		// Lost Collectable 1 - fire extinguisher
		case LEVENT_LOSE_COLL:
		{
			if (dwValue == 1)
				return State.SetState(L3E_LOSE_FIREEXT);
			else
				return FALSE;
		}

		case LEVENT_KILL:
		{
			COpp *pOpp = (COpp *)dwValue;
			if (pOpp)
			{
				// Normally it would be all opponents, but for now
				// it will only be three for testing... (mc)
				//if (!lpWorld->AnyOpponents())
				//if (nKillCount == 2)
				if (pOpp->GetGroup()->AllDead())
				{	
					nKillCount = 0;
					ShowText("You killed them all!");
					return State.SetState(L3E_KILLEDALL);
				}
				else
				{
					STRING szText;
					++nKillCount;

					wsprintf(szText, "%d down, %d to go", nKillCount, pOpp->GetGroup()->CountOpps());
					ShowText(szText);
				}
			}
			
			return FALSE;
		}
	}

	return FALSE;
}

//***********************************************************************
void CLevel3::OnStateChange(int nState)
//***********************************************************************
{
	switch(nState)
	{
		// Got fire extinquisher
		case L3S_HAVE_EXT:
		{
			m_bHaveExtinguisher = TRUE;
			if (App.pGame)
				App.pGame->Collect.SetTimedLife(1, 6, 36000);
			break;
		}

		// Got excaliber while having extinguisher
		case L3S_ALL_DEAD:
		{
			m_bHaveGateKey = TRUE;
			m_bHaveExtinguisher = TRUE;
			break;
		}

		// Do have excaliber but do not have extinguisher
		case L3S_ALL_DEAD_NE:
		{
			m_bHaveGateKey = TRUE;
			m_bHaveExtinguisher = FALSE;
			break;
		}

		case L3S_START:
		{
			m_bHaveGateKey = FALSE;
			m_bHaveExtinguisher = FALSE;
			break;
		}
	}
}

//***********************************************************************
CLevel4::CLevel4() : CLevel(4, (int *)Level4States, MAX_LEVEL4_EVENTS, 
							MAX_LEVEL4_STATES)
//***********************************************************************
{
}

//***********************************************************************
BOOL CLevel4::OnEvent(GameState& State, int nEvent, DWORD dwValue)
//***********************************************************************
{
	switch(nEvent)
	{
		// Collectables - Notes
		case LEVENT_GET_COLL:
		{
			if (dwValue >= 1 && dwValue <= 6)
			{
				STRING szText;
				wsprintf(szText, "You got note %d of 6!", (int)dwValue);
				ShowText(szText);
				return State.SetState(L4E_FOUNDNOTE1 + dwValue - 1);
			}
			else
				return FALSE;
		}
		case LEVENT_LOSE_ALL:
			return State.SetState(L4S_START);
	}

	return FALSE;
}

//***********************************************************************
void CLevel4::OnStateChange(int nState)
//***********************************************************************
{
	if (nState == L4S_HAVE_NOTE6)
		m_bHaveGateKey = TRUE;
}

//***********************************************************************
CLevel5::CLevel5() : CLevel(5, (int *)Level5States, MAX_LEVEL5_EVENTS, 
							MAX_LEVEL5_STATES)
//***********************************************************************
{
}

//***********************************************************************
BOOL CLevel5::OnEvent(GameState& State, int nEvent, DWORD dwValue)
//***********************************************************************
{
	switch(nEvent)
	{
		// Going around the hilt!
		case LEVENT_SWITCH_ON:
		{
			return State.SetState(L5E_HILT);
		}
	}

	return FALSE;
}

//***********************************************************************
void CLevel5::OnStateChange(int nState)
//***********************************************************************
{
	if (nState == L5S_HILT_3)
		m_bHaveGateKey = TRUE;
}

//***********************************************************************
CLevel6::CLevel6() : CLevel(6, (int *)Level6States, MAX_LEVEL6_EVENTS, 
							MAX_LEVEL6_STATES)
//***********************************************************************
{
}

//***********************************************************************
BOOL CLevel6::OnEvent(GameState& State, int nEvent, DWORD dwValue)
//***********************************************************************
{
	switch(nEvent)
	{
		// Collectables - Letters
		case LEVENT_GET_COLL:
		{
			if (dwValue >= 1 && dwValue <= 7)
			{
				STRING szText;
				m_nUseData1 |= (1 << (dwValue-1));

				int nCount = 0;
				for(int i=0; i<7; i++)
				{
					if (m_nUseData1 & (1 << i))
						++nCount;
				}

				if (nCount == 7)
					wsprintf(szText, "You found the last letter!");
				else
					wsprintf(szText, "You found a letter! Only %d left.", 7 - nCount);
				ShowText(szText);
				return State.SetState(L6E_FOUNDLET1 + dwValue - 1);
			}
			else
				return FALSE;
		case LEVENT_LOSE_ALL:
			return State.SetState(L6S_START);
		}
	}

	return FALSE;
}

//***********************************************************************
void CLevel6::OnStateChange(int nState)
//***********************************************************************
{
	if (nState == L6S_HAVE_LET6)
		m_bHaveGateKey = TRUE;
}

//***********************************************************************
CLevel7::CLevel7() : CLevel(7, (int *)Level7States, MAX_LEVEL7_EVENTS, 
							MAX_LEVEL7_STATES)
//***********************************************************************
{
}

//***********************************************************************
BOOL CLevel7::OnEvent(GameState& State, int nEvent, DWORD dwValue)
//***********************************************************************
{
	switch(nEvent)
	{
		// Collectables - Notes
		case LEVENT_GET_COLL:
		{
			if (dwValue >= 1 && dwValue <= 5)
			{
				STRING szText;
				m_nUseData1 |= (1 << (dwValue-1));

				int nCount = 0;
				for(int i=0; i<5; i++)
				{
					if (m_nUseData1 & (1 << i))
						++nCount;
				}

				if (nCount == 5)
					wsprintf(szText, "You found the last piece of the medallion!");
				else
					wsprintf(szText, "You found a piece of the medallion! Only %d left.", 5 - nCount);
				ShowText(szText);
				return State.SetState(L7E_FOUND1 + dwValue - 1);
			}
			else
				return FALSE;
		case LEVENT_LOSE_ALL:
			return State.SetState(L7S_START);
		}
	}

	return FALSE;
}

//***********************************************************************
void CLevel7::OnStateChange(int nState)
//***********************************************************************
{
	if (nState == L7S_HAVE_5)
		m_bHaveGateKey = TRUE;
}

//***********************************************************************
CLevel8::CLevel8() : CLevel(8, (int *)Level8States, MAX_LEVEL8_EVENTS, 
							MAX_LEVEL8_STATES)
//***********************************************************************
{
}

//***********************************************************************
BOOL CLevel8::OnEvent(GameState& State, int nEvent, DWORD dwValue)
//***********************************************************************
{
	switch(nEvent)
	{
		// Collectables - Notes
		case LEVENT_KILL:
		{
			COpp *pOpp = (COpp *)dwValue;
			if (pOpp)
			{
				// See if it is a reindeer
				int nClass = pOpp->GetClass();
				if (nClass == OPPCLASS_REINDEER)
				{
					STRING szText;
					++m_nUseData1;
					int iNum = 8 - m_nUseData1;
					if (iNum == 0)
						wsprintf(szText, "All eight reindeer have been killed!");
					else
						wsprintf(szText, "%d reindeer left!", iNum);
					ShowText(szText);
					return State.SetState(L8E_KILLREINDEER);
				}
				else
				if (nClass == OPPCLASS_RUDOLPH)
				{
					COpponentGroup *pGroup = pOpp->GetGroup();
					if (pGroup)
					{
						pGroup->Resurrect(OPPNUM_REINDEER1, OPPNUM_REINDEER9);
						pGroup->Clone(pOpp);
						ShowText("You killed Rudolph! All dead reindeer have been resurrected.");
						m_nUseData1 = 0;
					}
					return State.SetState(L8E_KILLRUDOLPH);
				}
			}
			else
				return FALSE;
		}
	}

	return FALSE;
}

//***********************************************************************
void CLevel8::OnStateChange(int nState)
//***********************************************************************
{
	if (nState == L8S_KILLED_8)
		m_bHaveGateKey = TRUE;
	else
	if (nState == L8S_START)
		m_bHaveGateKey = FALSE;
}

//***********************************************************************
CLevel9::CLevel9() : CLevel(9, (int *)Level9States, MAX_LEVEL9_EVENTS, 
							MAX_LEVEL9_STATES)
//***********************************************************************
{
	m_bHaveGateKey = FALSE;
}

//***********************************************************************
BOOL CLevel9::OnEvent(GameState& State, int nEvent, DWORD dwValue)
//***********************************************************************
{
	switch(nEvent)
	{
		// Collectables 
		case LEVENT_GET_COLL:
		{
			if (dwValue >= 1 && dwValue <= 4)
			{
				STRING szText;
				m_nUseData1 |= (1 << (dwValue-1));

				int nCount = 0;
				for(int i=0; i<4; i++)
				{
					if (m_nUseData1 & (1 << i))
						++nCount;
				}

				if (nCount == 4)
					wsprintf(szText, "You found the last Go Girl!");
				else
					wsprintf(szText, "You found a Go Girl! Only %d left.", 4 - nCount);
				ShowText(szText);
				return State.SetState(L9E_FOUNDGOGIRL);
			}
			else
				return FALSE;
		case LEVENT_LOSE_ALL:
			return State.SetState(L9S_START);
		}
	}

	return FALSE;
}

//***********************************************************************
void CLevel9::OnStateChange(int nState)
//***********************************************************************
{
	if (nState == L9S_HAVE_4)
		m_bHaveGateKey = TRUE;
}

//***********************************************************************
CLevel10::CLevel10() : CLevel(10, (int *)Level10States, MAX_LEVEL10_EVENTS, 
							MAX_LEVEL10_STATES)
//***********************************************************************
{
}

//***********************************************************************
BOOL CLevel10::OnEvent(GameState& State, int nEvent, DWORD dwValue)
//***********************************************************************
{
	switch(nEvent)
	{
		case LEVENT_KILL:
		{
			//COpp *pOpp = (COpp *)dwValue;
			//if (pOpp)
			//{
				// See if it is the gatekeeper
				//int nClass = pOpp->GetClass();
				//if (nClass == OPPCLASS_GATEKEEPER)
				//	return State.SetState(L10E_SHOTGATEKEEPER);
				//else
			//		return FALSE;
			//}
		}
	}

	return FALSE;
}

//***********************************************************************
void CLevel10::OnStateChange(int nState)
//***********************************************************************
{
	if (nState == L10S_SHOTHIM)
		m_bHaveGateKey = TRUE;
}

//***********************************************************************
CLevel11::CLevel11() : CLevel(11, (int *)Level11States, MAX_LEVEL11_EVENTS, 
							MAX_LEVEL11_STATES)
//***********************************************************************
{
}

//***********************************************************************
BOOL CLevel11::OnEvent(GameState& State, int nEvent, DWORD dwValue)
//***********************************************************************
{
	switch(nEvent)
	{
		case LEVENT_GOT_SK:
		{
			++m_nUseData1;

			STRING szText;
			if (m_nUseData1 > 19) 
			{
				ShowText("You got enough Spine Killers!");
				return State.SetState(L11E_GOTENOUGHSKS);
			}
			else
			{
				wsprintf(szText, "You need %d more Spine Killers.", 20 - m_nUseData1);
				ShowText(szText);
				return FALSE;
			}
		}
	}

	return FALSE;
}

//***********************************************************************
void CLevel11::OnStateChange(int nState)
//***********************************************************************
{
	if (nState == L11S_HAVEENOUGH)
		m_bHaveGateKey = TRUE;
}


//***********************************************************************
CLevel12::CLevel12() : CLevel(12, (int *)Level12States, MAX_LEVEL12_EVENTS, 
							MAX_LEVEL12_STATES)
//***********************************************************************
{
}

//***********************************************************************
BOOL CLevel12::OnEvent(GameState& State, int nEvent, DWORD dwValue)
//***********************************************************************
{
	switch(nEvent)
	{
		// Collectables - excaliber pics
		case LEVENT_GET_COLL:
		{
			if (dwValue >= 1 && dwValue <= 8)
			{
				STRING szText;

				wsprintf(szText, "You got %d of 8 Excaliber pics!", (int)dwValue);
				ShowText(szText);
				BOOL bRet = State.SetState(L12E_GOT1 + dwValue - 1);
				if (dwValue == 8 && m_nUseData1 == ALL_OPPS_DEAD)
					bRet = State.SetState(L12E_ALLDEAD);
				return bRet;
			}
			else
				return FALSE;
		}

		// Killed an opponent
		case LEVENT_KILL:
		{
			COpp *pOpp = (COpp *)dwValue;
			if (pOpp)
			{
				// Normally it would be all opponents, but for now
				// it will only be three for testing... (mc)
				//if (!lpWorld->AnyOpponents())
				//if (nKillCount == 2)
				if (pOpp->GetGroup()->AllDead())
				{	
					m_nUseData1 = ALL_OPPS_DEAD;
					ShowText("You killed all the drones!");
					return State.SetState(L12E_ALLDEAD);
				}
				else
					++m_nUseData1;
			}
			
			return FALSE;
		}

		// Killed Spine - Hooray!
		case LEVENT_SPINE_KILLED:
		{
			ShowText("Y O U  K I L L E D  S P I N E ! ! !");
			return State.SetState(L12E_SPINEDEAD);
		}
	}

	return FALSE;
}

//***********************************************************************
void CLevel12::OnStateChange(int nState)
//***********************************************************************
{
	if (nState == L12E_SPINEDEAD)
		m_bHaveGateKey = TRUE;
}

