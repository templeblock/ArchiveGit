#include <windowsx.h>
#include "proto.h"
#include "cllsn.h"
#include "Opponents.h"
#include "Cells.h"		// Cells object
#include "CarShots.h"	// Car shots object

#ifdef _DEBUG
//	#define _DEBUG_BUILDSHOT
#endif

//***********************************************************************
COpponentGroup::COpponentGroup()
//***********************************************************************
{
	// Initialize
	m_pFirstOpp = NULL;
	m_pLastOpp = NULL;
	m_pRecs = NULL;
	m_iNumRecs = 0;
	m_pShotCell = NULL;
	m_szCurShot[0] = NULL;
	m_szNextShot[0] = NULL;
	m_bEmptyShot = TRUE;

	// Create the car shot object 
	FNAME szExpFileName;
	if ( !GetApp()->FindContent( "cargates.prn", szExpFileName ) )
		*szExpFileName = '\0';

	m_pCarShots = new CCarShots();
	m_pCarShots->Load(szExpFileName);

	m_bPaused = TRUE;

	// Make sure shot car list is cleared
	m_iCarShotCnt = 0;
	m_iEngaged = 0;

	m_pFirstGen = NULL;

	// Set up engaged circular list
	m_pEngaged = &m_EngageRecs[11];
	for(int i=0; i<12; i++)
	{
		m_EngageRecs[i].iDir = i+1;
		m_EngageRecs[i].pOpp = NULL;
		if (i==0)
			m_EngageRecs[i].pPrev = &m_EngageRecs[11];
		else
			m_EngageRecs[i].pPrev = &m_EngageRecs[i-1];
		if (i==11)
			m_EngageRecs[i].pNext = &m_EngageRecs[0];
		else
			m_EngageRecs[i].pNext = &m_EngageRecs[i+1];
	}

	CCar::SetGroup(this);
}

//***********************************************************************
COpponentGroup::~COpponentGroup()
//***********************************************************************
{
	// Free the opponent records
	if (m_pRecs)
		FreeUp(m_pRecs);

	// Delete any generators
	DeleteGenerators();

	// Delete the opponents
	DeleteOpps();

	// Delete the car shot object
	if (m_pCarShots)
		delete m_pCarShots;
}

//***********************************************************************
BOOL COpponentGroup::LoadRecs(char *pszFilename)
//***********************************************************************
{
	HFILE hFile;
	UINT uRet;

	FNAME szExpFileName;
	if ( GetApp()->FindContent( pszFilename, szExpFileName ) )
		hFile =  _lopen(szExpFileName, OF_READ);
	else	
		hFile = HFILE_ERROR;

	if (hFile == HFILE_ERROR)
	{
		Print("No records loaded. Could not open opponent data file.");
		return FALSE;
	}

	// Figure number of records
	long lEnd =  _llseek(hFile, 0, FILE_END);
	m_iNumRecs = lEnd / sizeof(OpponentRec);
	_llseek(hFile, 0, FILE_BEGIN);

	UINT uSize = m_iNumRecs * sizeof(OpponentRec);

	// Allocate memory for array
	if (m_pRecs)
		FreeUp(m_pRecs);
		
	m_pRecs = (OpponentRec *)Alloc(uSize);
	if (m_pRecs == NULL)
	{
		Print("Could not allocate memory for opponent records.");
		_lclose(hFile);
		return FALSE;
	}

	// Read the records
	uRet = _lread( hFile, m_pRecs, uSize);

	// If the record was read ok, then inc counter
	if (uRet == HFILE_ERROR || uRet < uSize)
		Print( "Could not read opponent records.");

	_lclose(hFile);

	return TRUE;
}

//***********************************************************************
COpp *COpponentGroup::CreateOpp(short iId, short iCellx, short iCelly)
//***********************************************************************
{
	// Records are zero based, so adjust iId
	--iId;

	// Check id (index)
	if (iId >= m_iNumRecs || iId < 0)
		return NULL;

	// Create opponent
	COpp *pOpp = new COpp();
	if (!pOpp)
		return NULL;

	// Set opponent attributes
	pOpp->m_xCell = iCellx;
	pOpp->m_yCell = iCelly;
	pOpp->pntLastDraw.x = -1;
	pOpp->pntLastDraw.y = -1;
	pOpp->m_iDir = m_pRecs[iId].nStartDirection;
	pOpp->m_iRecIdx = iId;
	pOpp->m_dwLastMoveTime = timeGetTime();
	pOpp->m_nVideoPos = -1;
	pOpp->SetDriveMode(DRIVEMODE_NORMAL);
	pOpp->m_iPoints = m_pRecs[iId].nPoints;
	pOpp->m_iStrength = m_pRecs[iId].nStrength;
	pOpp->m_iSpeed = m_pRecs[iId].nSpeed;
	pOpp->m_cCarType = m_pRecs[iId].cCarType;
	pOpp->m_iClass = m_pRecs[iId].nClass;
	pOpp->m_pszName = m_pRecs[iId].szName;

	// Put the opponent in it's home cell
	pOpp->SetHome();

	// Fine tune the opponents tick position in case there are other
	// opponents already sitting on that tick position
	pOpp->AdjustTick();

	//
	// Add opponent to the end of the linked list
	//

	// If first opponent
	if (m_pFirstOpp == NULL)
	{
		m_pFirstOpp = pOpp;
		pOpp->pPrev = NULL;
	}

	// If first last opponent
	if (m_pLastOpp == NULL)
	{
		m_pLastOpp = pOpp;
		pOpp->pPrev = NULL;
	}

	// Else add to the end of the list
	else
	{
		m_pLastOpp->pNext = pOpp;
		pOpp->pPrev = m_pLastOpp;
		m_pLastOpp = pOpp;
	}

	pOpp->pNext = NULL;

	// Return the opponent just created
	return pOpp;
	
}

//***********************************************************************
void COpponentGroup::DeleteOpps(void)
//***********************************************************************
{
	COpp *pCurr = m_pFirstOpp;	// Point to first opponent

	// Make sure engaged list is cleared
	ReleaseEngaged();

	// Make sure shot car list is cleared
	m_iCarShotCnt = 0;
	m_iEngaged = 0;
	m_pShotCell = NULL;

	// For all opponents
	while(pCurr)
	{
		COpp * pNext = pCurr->pNext;

		// Delete it
		delete pCurr;

		pCurr = pNext;
	}

	// Reset first and last links 
	m_pFirstOpp = NULL;
	m_pLastOpp = NULL;
}

//***********************************************************************
void COpponentGroup::CreateGenerator(CCell *pCell, DWORD dwFrequency, 
									 int iFirstOpp, int iLastOpp)
//***********************************************************************
{
	COppGenerator *pGen = new COppGenerator(pCell, dwFrequency, this,
		iFirstOpp, iLastOpp);

	if (!pGen)
		return;

	if ( m_pFirstGen != NULL)
	{
		pGen->m_pNext = m_pFirstGen;
	}

	m_pFirstGen = pGen;
}

//***********************************************************************
void COpponentGroup::DeleteGenerators(void)
//***********************************************************************
{
	COppGenerator *pGen = m_pFirstGen;
	while(pGen)
	{
		m_pFirstGen = pGen->m_pNext;
		delete pGen;
		pGen = m_pFirstGen;
	}
}

//***********************************************************************
void COpponentGroup::CheckGenerators(void)
//***********************************************************************
{
	COppGenerator *pGen = m_pFirstGen;
	DWORD dwTime = timeGetTime();
	while(pGen)
	{
		pGen->Check(dwTime, this);
		pGen = pGen->m_pNext;
	}
}

//***********************************************************************
void COpponentGroup::Pause(void)
//***********************************************************************
{
	m_bPaused = TRUE;
}

//***********************************************************************
void COpponentGroup::UnPause(void)
//***********************************************************************
{
	COpp *pCurr = GetFirst();

	while(pCurr)
	{
		pCurr->StartMove();
		pCurr = pCurr->GetNext();
	}

	m_bPaused = FALSE;
}

//***********************************************************************
BOOL COpponentGroup::MoveNormal(void)
//***********************************************************************
{
	COpp *pCurr = m_pFirstOpp;		// Point to first opponent
	DWORD dwTime = timeGetTime();	// Get the current time

	if (m_bPaused)
		return FALSE;

	// For all opponents
	while(pCurr)
	{
		// Move the car - given the current time
		pCurr->Move(dwTime);

		// Next car
		pCurr = pCurr->pNext;

	}

	//Debug("Time to move %d cars = %ld\n", iCnt, timeGetTime() - dwTime);
	//if (dwTickTurnCount > 0)
	//	Debug("Avg test time = %f\n", (float)(dwTickTurnTime / dwTickTurnCount));

	CheckGenerators();

	return TRUE;
}

//***********************************************************************
void COpponentGroup::ReleaseShotCars(void)
//***********************************************************************
{
	// 
	// For all the cars that are currently in the shot car list
	// make sure their driving mode is restored, unless they
	// are currently engaged with the player.
	//
	for(int i=0; i<m_iCarShotCnt; i++)
	{
		// If the car in the shot list has an active state 
		// (such as CARSHOT_STATE_INSHOT) and the car is not engaged
		// then set the driving mode to restore
		if (m_ShotCars[i].nState != 0 && 
			m_ShotCars[i].pOpp->GetDriveMode() != DRIVEMODE_ENGAGED)
		{
			m_ShotCars[i].pOpp->SetDriveMode(DRIVEMODE_RESTORE);
		}
		
	}

	// Rest the car shot list to none
	m_iCarShotCnt = 0;
}

//***********************************************************************
void COpponentGroup::AddEngaged(COpp *pOpp)
//***********************************************************************
{
	int iDir = pOpp->GetDir();

	// Adjust dir relative to player's dir, player's dir is treated
	// as always 12 o'clock in shots
	//int iAdjust = 12 - App.pGame->lpWorld->GetPlayer()->GetDir();
	iDir -= App.pGame->lpWorld->GetPlayer()->GetDir();
	if (iDir < 1)
		iDir += 12;

	// Now flip directions
	iDir += 6;
	if (iDir > 12)
		iDir -= 12;

	// make zero based
	--iDir;

	m_EngageRecs[iDir].pOpp = pOpp;

	Debug("Add engaged at %d", iDir+1);
}

//***********************************************************************
void COpponentGroup::ReleaseEngaged(void)
//***********************************************************************
{
	// Release all opponents that are engaged
	for(int i=0; i<12; i++)
	{
		// Set the opponent to restore mode 
		// NOTE: It might be better to implement disengage drive mode
		if (m_EngageRecs[i].pOpp != NULL)
			m_EngageRecs[i].pOpp->SetDriveMode(DRIVEMODE_RESTORE);
			m_EngageRecs[i].pOpp = NULL;
		// TEMPORARY - DESTROY FOR TESTING
		//m_Engaged[i]->SetDriveMode(DRIVEMODE_DESTROYED);

	}
}

//***********************************************************************
EngageRec *COpponentGroup::FindEngaged(int idx)
//***********************************************************************
{
	// First find car
	int iCnt = -1;
	int i=0;
	EngageRec *pCur = m_pEngaged->pPrev->pPrev->pPrev;
	while(i < 12)
	{
		if (pCur->pOpp)
			++iCnt;

		if (iCnt == idx)
			break;

		pCur = pCur->pNext;
		++i;
	}

	// Not Found!
	if (iCnt < idx)
		return NULL;
	else
		return pCur;
}

//***********************************************************************
int COpponentGroup::HitEngaged(int idx, int iDamage)
//***********************************************************************
{
	int iPoints;

	EngageRec *pCur = FindEngaged(idx);
	if (!pCur)
		return -1;

	COpp *pOpp = pCur->pOpp;
	if (!pOpp)
		return -1;

	pOpp->m_iStrength -= iDamage;
	if (pOpp->m_iStrength < 1)
	{
		iPoints = 0;

		pOpp->SetDriveMode(DRIVEMODE_DESTROYED);

		// Opponent is dead
		iPoints += pOpp->m_iPoints;

		// Remove opponent from engaged list
		pCur->pOpp = NULL;

		return iPoints;
	}
	else
		// Opponent has strength left - not dead
		return 0;
}

//***********************************************************************
int COpponentGroup::EngagedCrashStrength(int idx)
//***********************************************************************
{
	EngageRec *pCur = FindEngaged(idx);
	if (!pCur)
		return 0;

	COpp *pOpp = pCur->pOpp;
	if (!pOpp)
		return 0;

	return (pOpp->m_iStrength * 100);
}

//***********************************************************************
int COpponentGroup::EngagedShooter(int idx)
//***********************************************************************
{
	EngageRec *pCur = FindEngaged(idx);
	if (!pCur)
		return FALSE;

	COpp *pOpp = pCur->pOpp;
	if (!pOpp)
		return FALSE;

	// Test for classes that fire back
	switch(pOpp->m_iClass)
	{
	case OPPCLASS_CONDRONE:
	case OPPCLASS_HELLION:
	case OPPCLASS_NIGHTRON:
	case OPPCLASS_PARHELION:
	case OPPCLASS_DOGGER:
	case OPPCLASS_ELECTROSHARK:
	case OPPCLASS_REINDEER:
	case OPPCLASS_DREADNAUGHT:

		return pOpp->m_iClass;

	default:
		return FALSE;
	}

	return FALSE;
}

//***********************************************************************
char *COpponentGroup::BuildShot(void)
//***********************************************************************
{
	OppCarShot *pCarShot;

	//
	// Build a potential shot name given what the shot cell is (the cell
	// in front of the player) and given what opponents are around the
	// shot cell.
	//

	// Make sure current car shots are cleared
	ReleaseShotCars();

	// Point to the first opponent
	COpp *pCurr = m_pFirstOpp;

	// For all cars
	while(pCurr)
	{

		// If the car is a shot candidate then add it to the list
		if ( (pCarShot = pCurr->CheckForShot()) )
		{
			// Make sure limit has not been reached before adding opp to the list
			if (m_iCarShotCnt < MaxShotCars)
				memcpy(&m_ShotCars[m_iCarShotCnt++], pCarShot, sizeof(OppCarShot));
		}

		pCurr = pCurr->pNext;
	}

	// Build prefix (example whould be H01)
	char *pszPrefix = GetShotCellPrefix();
	m_pszCarGates = NULL;
	int i = 0;
	int iLen;

	// If there is a prefix...
	if (pszPrefix)
	{

		if (pszPrefix[0] == 'H' || pszPrefix[0] == 'U' || pszPrefix[0] == 'T')
		{
				
			// Build an optimal shot given the prefix and the cars around the shot cell
			m_pszCarGates = m_pCarShots->GetOptimalShot(pszPrefix, m_ShotCars, m_iCarShotCnt);

			// Add the prefix to the next shot string
			lstrcpy(m_szNextShot, pszPrefix);

			// Add the car gates (if any) to the next shot string 
			if (m_pszCarGates)
			{
				lstrcat(m_szNextShot, m_pszCarGates);
				m_bEmptyShot = FALSE;
			}
			else
			{
				m_bEmptyShot = TRUE;
			}

			// Fill in the blanks with the '_' character
			iLen = lstrlen(m_szNextShot);
			for(i=iLen; i<11; i++)
			{
				m_szNextShot[i] = '_';
			}
		}
		else
		{
			lstrcpy(m_szNextShot, pszPrefix);
			i += strlen(pszPrefix);
		}
	}

	m_szNextShot[i] = NULL;

	#ifdef _DEBUG_BUILDSHOT
		Debug("next shot <%s>\n", m_szNextShot);
	#endif

	// Return the next shot string (Remember, this is just a potential shot that will
	// be used only if the player enters this cell. It will be thrown away if the
	// player turns and a new shot cell is designated.)
	return m_szNextShot;
}

//***********************************************************************
//char *COpponentGroup::AddBuildShot(void)
//***********************************************************************
//{
	//
	// Add to the build shot. This method is called when an opponent
	// goes into a cell adjacent to the shot cell
	//

	// If the shot is not empty then return
//	if (!m_bEmptyShot)
//		return m_szNextShot;
//}

//***********************************************************************
void COpponentGroup::SendInTheClowns(void)
//***********************************************************************
{

	//
	// For all cars in the potential car shot
	// 
	for(int i=0; i<m_iCarShotCnt; i++)
	{
		// If the car has been tagged as in the shot
		// then send it in to the shot
		if (m_ShotCars[i].nState == CARSHOT_STATE_INSHOT)
			m_ShotCars[i].pOpp->SendIn();
	}
}

//***********************************************************************
char *COpponentGroup::GetShotCellPrefix(void)
//***********************************************************************
{
	static char szPre[4];

	// 
	// Figure the cell shot prefix for the shot name.
	//

	// If there is a shot cell
	if (m_pShotCell)
	{

		// Get the player direction
		int iDir = App.pGame->lpWorld->GetPlayer()->GetDir();

		// If the cell is not on then generate a culdesac shot
		if (!m_pShotCell->IsOn())
		{
			szPre[0] = 'C';
		}
		else
		{

			// Cell type such as H or U
			szPre[0] = m_pShotCell->Type();

			// Convert tunnels
			if ( (szPre[0] == '`' && (iDir == 10 || iDir == 4)) || 
				 (szPre[0] == '/' && (iDir == 2  || iDir == 8)) ||
				 (szPre[0] == '|' && (iDir == 12 || iDir == 6)) )
			{
				szPre[0] = 'T';

				// Cell look such as 0 - normal, 1 - hell, etc.
			}
			else if (szPre[0] == '`' || szPre[0] == '/' || szPre[0] == '|')
				szPre[0] = 'C';

			szPre[1] = m_pShotCell->Look();
		}
		//
		// Now we need to figure out what gate the player
		// is facing
		//

		// If the shot cell is in a hex or radioactive or safe cell.
 		if (szPre[0] == 'H' || szPre[0] == 'R' || szPre[0] == 'S')
		{
			// Facing green gate?
			if (iDir == 12 || iDir == 4 || iDir == 8)
				szPre[2] = '1';

			// or facing between green and yellow gate?
			else if (iDir == 1 || iDir == 5 || iDir == 9)
				szPre[2] = '2';

			// or facing yellow gate?
			else if (iDir == 2 || iDir == 6 || iDir == 10)
				szPre[2] = '3';

			// or facing between yellow and green gate?
			else if (iDir == 3 || iDir == 7 || iDir == 11)
				szPre[2] = '4';
		}

		// else is the shot cell a union jack?
		else if (szPre[0] == 'U')
		{
			// Facing the big wall between a read and green gate?
			if (iDir == 3 || iDir == 9)
				szPre[2] = '1';

			// or facing green gate?
			else if (iDir == 4 || iDir == 10)
				szPre[2] = '2';

			// or facing between green and yellow gate?
			else if (iDir == 5 || iDir == 11)
				szPre[2] = '3';

			// or facing yellow gate?
			else if (iDir == 6 || iDir == 12)
				szPre[2] = '4';

			// or facing between yellow and red gate?
			else if (iDir == 7 || iDir == 1)
				szPre[2] = '5';

			// or facing red gate?
			else if (iDir == 8 || iDir == 2)
				szPre[2] = '6';
		}
		// else is it a tunnel?
		else if (szPre[0] == 'T')
		{
			if (iDir == 4 || iDir == 12)
				szPre[2] = '1';
			else
				szPre[2] = '2';
		}

		// Is the cell a cul-de-sac?
		else if (szPre[0] == 'C')
		{
			int iNum = (App.pGame->lpWorld->GetPlayer()->GetDir() / 2) + 3 - 1;
			if (iNum > 5)
				iNum -= 6;

			unsigned char cType = m_pShotCell->PortalType(iNum);
		
			if ( !App.pGame->lpWorld->IsValidCulDeSac( cType ) )
				cType = 'M'; // empty

			szPre[1] = (char)cType;
			szPre[2] = NULL;
		}
		// Else assume it is a cell type that we can't deal with
		else
		{
			return NULL;
		}

		szPre[3] = NULL;

		// Return the prefix
		return szPre;
	}

	// else no prefix since there is not a shot cell
	else
		return NULL;
}

//***********************************************************************
char *COpponentGroup::SetCurShot(void)
//***********************************************************************
{
	// Make the potential next shot the actual curren shot
	lstrcpy(m_szCurShot, m_szNextShot);
	
	// Set last car shot found so it doesn't repeat
	m_pCarShots->SetLastCarGate(m_pszCarGates);

	// return the current shot
	return m_szCurShot;
}

//***********************************************************************
void COpponentGroup::DestroyOpp(COpp *pOpp)
//***********************************************************************
{
	// Tell the level state machine that an opponent is being destroyed
	App.pGame->Levels.SetState(LEVENT_KILL, (DWORD)pOpp);

	// If first in the linked list
	if (pOpp == m_pFirstOpp)
	{
		// Set the new header link
		m_pFirstOpp = pOpp->pNext;
		if (m_pFirstOpp)
			m_pFirstOpp->pPrev = NULL;
	}

	// If last in the list
	if (pOpp == m_pLastOpp)
	{
		// Set the new tail link
		m_pLastOpp = pOpp->pPrev;
		if (m_pLastOpp)
			m_pLastOpp->pNext = NULL;
	}
	// else in the middle of the list
	else
	{
		// Set the previous opponent next link
		// to the next opponent
		if (pOpp->pPrev)
			pOpp->pPrev->pNext = pOpp->pNext;

		// Set the next opponent previous link
		// to the previous opponent
		if (pOpp->pNext)
			pOpp->pNext->pPrev = pOpp->pPrev;
	}

	// Delete the opponent
	delete pOpp;
}

//***********************************************************************
int COpponentGroup::OppsInCell(CCell *pCell)
//***********************************************************************
{
	if (!pCell)
		return 0;

	int iCnt = 0;
	COpp *pOpp = m_pFirstOpp;

	while(pOpp)
	{
		CCell * pOppCell = pOpp->GetCell();
		if (pOppCell)
		{
			if (pOppCell->Getx() == pCell->Getx() &&
				pOppCell->Gety() == pCell->Gety())
				++iCnt;
		}
		pOpp = pOpp->Next();
	}

	return iCnt;
}

//***********************************************************************
void COpponentGroup::OppGenOff(int iGen)
//***********************************************************************
{
	COppGenerator *pGen = m_pFirstGen;
	int iCnt = 1;

	while(pGen)
	{
		if (iCnt == iGen)
		{
			pGen->Off();
			break;
		}
		++iCnt;
		pGen = pGen->m_pNext;
	}
}

//***********************************************************************
void COpponentGroup::AdjustEngaged(int iNum)
//***********************************************************************
{
	// Adjusts the main engaged pointer (this is the direction the player
	// is facing)

	if (iNum > 0)
	{
		for(int i=0; i<iNum; i++)
			m_pEngaged = m_pEngaged->pNext;
	}
	else
	{
		iNum *= -1;
		for(int i=0; i<iNum; i++)
			m_pEngaged = m_pEngaged->pPrev;
	}

	BuildEngagedCars();
}

//***********************************************************************
void COpponentGroup::ResetEngaged(void)
//***********************************************************************
{
	m_pEngaged = &m_EngageRecs[11];  // Point to the 12 o'clock

	m_szEngagedCars[0] = NULL;		 // Null the engaged car string
									 // it gets built when the player car
									 // turns
	Debug("ResetEngaged");
}

//***********************************************************************
void COpponentGroup::BuildEngagedCars(void)
//***********************************************************************
{
	// Build the current string of engaged cars to be used in a shot
	// name. This method is used to produce shots when turning.
	//

	int iPos=0;
	char cGate ;
	m_szEngagedCars[0] = NULL;

	// Start search at 2 gates to the left
	EngageRec *pCur = m_pEngaged->pPrev->pPrev;

	Debug("build engage start=%d",pCur->iDir);
	for(int i=0; i<5; i++)
	{
		// If there is an opponent in this position
		if (pCur->pOpp)
		{
			// figure gate
			if (i<3)
				cGate = 'A' + i;
			else
				cGate = '1' + i - 3;

			m_szEngagedCars[iPos++] = pCur->pOpp->m_cCarType;
			m_szEngagedCars[iPos++] = cGate;

			if (iPos > 6)
				break;
		}

		pCur = pCur->pNext;
	}
	
	m_szEngagedCars[iPos] = NULL;
	Debug("szEngagedCars=%s",m_szEngagedCars);
		
}

//***********************************************************************
int COpponentGroup::GetStrength(int idx)
//***********************************************************************
{

	EngageRec *pCur = FindEngaged(idx);
	if (!pCur)
		return -1;

	COpp *pOpp = pCur->pOpp;
	if (!pOpp)
		return -1;

	return pOpp->m_iStrength;
}

//***********************************************************************
char *COpponentGroup::GetName(int idx)
//***********************************************************************
{
	EngageRec *pCur = FindEngaged(idx);
	if (!pCur)
		return NULL;

	COpp *pOpp = pCur->pOpp;
	if (!pOpp)
		return NULL;

	return pOpp->m_pszName;
}

//***********************************************************************
void COpponentGroup::DestroyArea(CCell *pPlayCell, int iDir)
//***********************************************************************
{
	CCell *pCell = pPlayCell;
	CCell *pNextCell = pCell;
	int i;
	POINT pnt;

	// Get the cell that is 4 cell in front of the players cell
	for(i=0; i<4; i++)
	{
		if (!pNextCell)
			break;

		pCell = pNextCell;

		pnt = App.pGame->lpWorld->GetPlayer()->NextCellFromDir(pCell->Getx(), 
			pCell->Gety(), iDir);
		pNextCell = App.pGame->lpWorld->GetCell(pnt.x, pnt.y);
	}

	if (i == 4 && pNextCell)
		pCell = pNextCell;
	
	Debug("d cell x=%d,y=%d", pCell->Getx(), pCell->Gety());

	// Now that we found the center cell blow out all the opponents in 
	// and  around the cell
	DestroyOpponents(pCell);
	if (pCell->Type() == 'H' || pCell->Type() == 'U')
		pCell->SetType('S');
	pCell->SetVisible(TRUE);

	for(i=2; i<=12; i+=2)
	{
		pnt = App.pGame->lpWorld->GetPlayer()->NextCellFromDir(pCell->Getx(), 
			pCell->Gety(), i);
		pNextCell = App.pGame->lpWorld->GetCell(pnt.x, pnt.y);
		if (pNextCell)
		{
			Debug("d cell x=%d,y=%d", pNextCell->Getx(), pNextCell->Gety());
	
			DestroyOpponents(pNextCell);
			if (pNextCell->Type() == 'H' || pNextCell->Type() == 'U')
				pNextCell->SetType('S');
			pNextCell->SetVisible(TRUE);
		}
	}

	App.pGame->lpWorld->CenterView(pCell, TRUE);
}

//***********************************************************************
void COpponentGroup::DestroyOpponents(CCell *pCell)
//***********************************************************************
{
	COpp *pCurr = m_pFirstOpp;	// Point to first opponent
	COpp *pNext;
	// For all opponents
	while(pCurr)
	{
		// test for destruction
		if (pCell == pCurr->GetCell())
		{
			pNext = pCurr->pNext;
			Debug("DestroyOpp");
			DestroyOpp(pCurr);
			pCurr = pNext;
		}
		else
		{
			pCurr = pCurr->pNext;
		}
	}
}

//***********************************************************************
void COpponentGroup::Resurrect(int nFirst, int nLast)
//***********************************************************************
{
	// See if already alive
	COpp *pOpp;
	BOOL bFound;

	int nID;
	for( nID = nFirst; nID <= nLast; nID++)
	{
		bFound = FALSE;
		pOpp = m_pFirstOpp;
		while(pOpp)
		{
			if ( (pOpp->m_iRecIdx+1) == nID)
			{
				bFound = TRUE;
				break;
			}
			pOpp = pOpp->pNext;
		}

		if (!bFound && App.pGame->lpWorld)
		{
			// Find place to put opponent (random)

			// Get cells and count of cells
			CCell **pCells = App.pGame->lpWorld->GetAllCells();
			int nCount = App.pGame->lpWorld->GetCellMapWidth() *
				App.pGame->lpWorld->GetCellMapHeight();

			int nPlace;
			CCell *pCell;

			// look for a place to put it
			while(!bFound)
			{
				nPlace = GetRandomNumber(nCount);
				pCell = pCells[nPlace];
				if (pCell->Type() == 'H' || pCell->Type() == 'U')
				{
					bFound = TRUE;
					CreateOpp(nID, pCell->Getx(), pCell->Gety());
				} // end if type hex or union
			} // end while cell not found
		} // end if opp not found
	} // end for all opponents to resurrect
}


//***********************************************************************
void COpponentGroup::Clone(COpp *pOpp)
//***********************************************************************
{
	if ( App.pGame->lpWorld)
	{
		// Get cells and count of cells
		CCell **pCells = App.pGame->lpWorld->GetAllCells();
		int nCount = App.pGame->lpWorld->GetCellMapWidth() *
			App.pGame->lpWorld->GetCellMapHeight();

		int nPlace;
		CCell *pCell;
		BOOL bFound = FALSE;

		// look for a place to put the cloned opponent
		while(!bFound)
		{
			nPlace = GetRandomNumber(nCount);
			pCell = pCells[nPlace];
			if (pCell->Type() == 'H' || pCell->Type() == 'U')
			{
				bFound = TRUE;
				CreateOpp(pOpp->GetIndex()+1, pCell->Getx(), pCell->Gety());
			} // end if type hex or union
		} // end while cell not found
	} // end if world
}

//***********************************************************************
int COpponentGroup::CountOpps(void)
//***********************************************************************
{
	COpp *pOpp = m_pFirstOpp;
	int nCount = 0;
	while(pOpp)
	{
		++nCount;
		pOpp = pOpp->pNext;
	}
	return nCount;
}

//***********************************************************************
COpponentGroup *CCar::m_pGroup = NULL;
//***********************************************************************


//***********************************************************************
CCar::CCar()
//***********************************************************************
{
	// Initialize
	m_iDir = 12;
	m_iTrackTick = 2;	// Start in middle of track (the turn tick)
	m_pCell = NULL;
	m_bStayOnTick = FALSE;
}

//***********************************************************************
void CCar::ChangeDirRel(short iDir)
//***********************************************************************
{
	//
	// Change relative direction
	//

	// Add the relative direction
	m_iDir += iDir;

	// Adjust the new direction to fit on the clock (1 - 12)
	if (m_iDir < 1)
		m_iDir += 12;
	else
	if (m_iDir > 12)
		m_iDir -= 12;
}

//***********************************************************************
void CCar::ChangeDirAbs(short iDir)
//***********************************************************************
{
	//
	// Change absolute direction
	//

	m_iDir = iDir;

	// Adjust the new direction to fit on the clock (1 - 12)
	if (m_iDir < 1)
		m_iDir += 12;
	else
	if (m_iDir > 12)
		m_iDir -= 12;
}

//***********************************************************************
short CCar::IncTick(void)
//***********************************************************************
{
	//
	// Increment the car track tick (position on the current track)
	// The ticks on a track are from 0 - 4
	//

	// If the car is supposed to stay on the current tick then return
	if (m_bStayOnTick)
		return m_iTrackTick;

	// Increment the tick
	++m_iTrackTick;

	// If the tick is greater than the last tick then leave it there.
	if (m_iTrackTick > TICKEND)
		m_iTrackTick = TICKEND;

	return m_iTrackTick;
}

//***********************************************************************
void CCar::StartTick(void)
//***********************************************************************
{
	// 
	// Start the car on a new tick on a new track. Each cell is a new 
	// track so the car will be entering a new cell.
	//

	// If the car is supposed to stay on the current tick then return
	if (m_bStayOnTick)
		return;

	// Try to enter a new cell
	if (EnterCell())
		m_iTrackTick = 0;

	// If the car could not enter a cell then decrement the tick
	// since a call to IncTick() usually follows. The net effect
	// should be that the car does not move
	else
		--m_iTrackTick;
}


//***********************************************************************
POINT CCar::GetCurPos(void)
//***********************************************************************
{
	POINT pos;

	// Return the current pixel position of the car
	pos.x = m_xPixel;
	pos.y = m_yPixel;

	return pos;
}

//***********************************************************************
POINT CCar::GetXYCell(void)
//***********************************************************************
{
	POINT Cell;

	// Return the x,y coordinate of the current car position in the cell
	Cell.x = m_xCell;
	Cell.y = m_yCell;

	return Cell;
}

//***********************************************************************
POINT CCar::GetNewPos(void)
//***********************************************************************
{
	//
	// Get a new x,y  position based off of the current track tick.
	// This x,y position is relative to the cell.
	//

	POINT pos;

	// Get the track index based on the current direction
	int idx = m_iDir - 1;
	if (idx<0 || idx>11)
	{
		m_xPixel = pos.x = 0;
		m_yPixel = pos.y = 0;
		return pos;
	}
		
	// Track deltas           1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12
	static short xDelta[] = {+6, +9, +9, +9, +6, +0, -6, -9, -9, -9, -6, +0};
	static short yDelta[] = {-9, -6, +0, +6, +9, +9, +9, +6, +0, -6, -9, -9};

	// Set the current x,y postion
	int iTick = m_iTrackTick - 2;
	m_xPixel = pos.x = iTick * xDelta[idx];
	m_yPixel = pos.y = iTick * yDelta[idx];

	return pos;
}


//***********************************************************************
POINT CCar::NextCellFromDir(int iCellx, int iCelly, int nDir)
//***********************************************************************
{
	// Figure what the next cell x,y position is given a
	// cell position and a direction.

	// Adjust the new direction to fit on the clock (1 - 12)
	if (nDir < 1)
		nDir += 12;
	else
	if (nDir > 12)
		nDir -= 12;

	// If direction is even (not odd) then we can enter a cell
	// (in other words we are going through a gate)

	if (!(nDir & 1))
	{
		// if the cell is odd, then adjacent cells are above
		if (iCellx & 1)
		{
			if (nDir == 4 || nDir == 6 || nDir == 8)
				++iCelly;
			else
			if (nDir == 12)
				--iCelly;
		}
		// else if the cell is even, adjacent cells are below
		else
		{
			if (nDir == 10 || nDir == 12 || nDir == 2)
				--iCelly;
			else
			if (nDir == 6)
				++iCelly;
		}

		// Figure the cell x,y
		if (nDir == 2 || nDir == 4)
			++iCellx;
		else
		if (nDir == 8 || nDir == 10)
			--iCellx;
	}

	// Return the cell x,y
	POINT pnt;
	pnt.x = iCellx;
	pnt.y = iCelly;

	return pnt;
}

//***********************************************************************
BOOL CCar::IsVisible(void)
//***********************************************************************
{
	if (m_pCell)
		return m_pCell->IsVisible();
	else
		return FALSE;
}

//***********************************************************************
CPlayer::CPlayer()
//***********************************************************************
{
	SetXYCell(0, 0);
	ChangeDirAbs(12);
}

//***********************************************************************
void CPlayer::SetXYCell(int xCell, int yCell)
//***********************************************************************
{
	// Set the player position and direction
	m_xCell = xCell;
	m_yCell = yCell;
	m_iTrackTick = 2;

	CCell *pCell = App.pGame->lpWorld->GetCell(m_xCell, m_yCell);
	m_pCell = pCell;

	if (pCell)
	{
		if (!pCell->IsVisible())
			pCell->SetVisible(TRUE);

		// Make sure lead cell is visible
		if (m_pGroup && (pCell = m_pGroup->GetShotCell()))
			pCell->SetVisible(TRUE);
	}
}

//***********************************************************************
void CPlayer::SetInGate(void)
//***********************************************************************
{
	// 
	// Position the player car so that it looks like it is in the gate.
	// 
	m_iTrackTick = 0;
}

//***********************************************************************
void CPlayer::FixDir(void)
//***********************************************************************
{
	// fix the direction when entering a new cell
	// if it's odd it's wrong (you would hit a wall)
	// this shouldn't be necessary,
	// but a collision might steer you off course
	int iDir = GetDir();
	if ( iDir & 1 )
	{ // back it up
		if ( !(--iDir) )
			iDir = 12;
		ChangeDirAbs(iDir);
	}
}

//***********************************************************************
short CPlayer::IncTick(void)
//***********************************************************************
{
	//
	// Increment the car track tick (position on the current track)
	// The ticks on a track are from 0 - 4
	//

	// The player always is positioned on tick 2 (the middle tick)
	m_iTrackTick = 2;

	return m_iTrackTick;
}

//***********************************************************************
void CPlayer::Forward(void)
//***********************************************************************
{
	//
	// Move the player forward a tick on the track
	//

	short iType = GetTickType();
	if (iType == TICKEND)
		StartTick();
	if ( !(m_iDir & 1))
		IncTick();
}

//***********************************************************************
void CPlayer::TurnLeft(char cSpecial)
//***********************************************************************
{
	// 
	// Turn the player left and set a new shot cell
	//

	int iChange = -1;

	// If union jack
	if (cSpecial == 'U' && 
		(m_iDir == 12 || m_iDir == 2 || m_iDir == 6 || m_iDir == 8))
	{
		iChange = -2;	//(note: uncomment when UJ turn events are in)	
	}

	ChangeDirRel(iChange);

	SetNewShotCell();
	//m_pGroup->AdjustEngaged(-1);
}

//***********************************************************************
void CPlayer::TurnRight(char cSpecial)
//***********************************************************************
{
	// 
	// Turn the player right and set a new shot cell
	//

	int iChange = 1;

	// If union jack
	if (cSpecial == 'U' && 
		(m_iDir == 12 || m_iDir == 10 || m_iDir == 6 || m_iDir == 4))
	{
		iChange = 2;	// (note: uncomment when UJ turn events are in)
	}

	ChangeDirRel(iChange);
	SetNewShotCell();
	//m_pGroup->AdjustEngaged(1);
}

//***********************************************************************
void CPlayer::TurnBack(void)
//***********************************************************************
{
	// 
	// Turn the player around and set a new shot cell
	//

	// Make a track tick adjustment so the player car does not jump
	// across the cell
	m_iTrackTick = 5 - m_iTrackTick;

	ChangeDirRel(6);
	SetNewShotCell();
	//m_pGroup->AdjustEngaged(6);
}

//***********************************************************************
BOOL CPlayer::EnterCell(void)
//***********************************************************************
{
	//
	// Player car enters a new cell
	//

	// Get the next x,y cell
	POINT pnt = NextCellFromDir(m_xCell, m_yCell, m_iDir);

	m_pGroup->ResetEngaged();

	// Clear opponent codes from current cell
	if (m_pCell)
		m_pCell->ClearOpps();

	// Set new player cell
	m_xCell = (short)pnt.x;
	m_yCell = (short)pnt.y;
	m_pCell = App.pGame->lpWorld->GetCell(m_xCell, m_yCell);
	if (m_pCell && !m_pCell->IsVisible() && m_pCell->IsOn())
		m_pCell->SetVisible(TRUE);

	// Make sure cells around player are visible
	for(int i=2; i<=12; i+=2)
	{
		POINT pt = NextCellFromDir(m_xCell, m_yCell, i);
		CCell* pCell = App.pGame->lpWorld->GetCell(pt.x, pt.y);
		if (pCell && !pCell->IsVisible() && pCell->IsOn())
			pCell->SetVisible(TRUE);
	}

	// If there are any opponents currently engaged then set them to
	// disengaged.
	m_pGroup->ReleaseEngaged();

	// If there are any opponents currently positioned for the shot
	// them send them in 
	m_pGroup->SendInTheClowns();

	// Set the current shot from the positioned next shot that was based
	// of the shot cell (which we are now entering)
	char *pszShot = m_pGroup->SetCurShot();

#ifdef	_DEBUG_BUILDSHOT
	// Debug info...
	if (pszShot)
		Debug(pszShot);
#endif

	// Set the new shot cell
	CCell *pCell = SetNewShotCell();

	// Get the next set of cars ready for a potential new shot
	//m_pGroup->BuildShot();

	// Make sure all unqualified opponents clear out of the player's cell
	// and the shot cell
	COpp *pOpp = m_pGroup->GetFirst();
	while(pOpp)
	{
		short iMode = pOpp->GetDriveMode();
		if (iMode != DRIVEMODE_ENGAGED)
		{
			if (m_pCell == pOpp->GetCell())
				pOpp->GetOut();
		}
		pOpp = pOpp->Next();
	}

	// Entered cell
	return TRUE;
}

//***********************************************************************
CCell *CPlayer::SetNewShotCell(void)
//***********************************************************************
{
	// Set a new shot cell based on the current cell and the direction

	// Get the cell that the player car is facing and establish it as
	// the shot cell
	POINT pnt = NextCellFromDir(m_xCell, m_yCell, m_iDir);
	CCell *pCell = App.pGame->lpWorld->GetCell(pnt.x, pnt.y);
	m_pGroup->SetShotCell(pCell);

	// Make sure there are no opponents in the shot cell
	COpp *pOpp = m_pGroup->GetFirst();
	while(pOpp)
	{
		// If the opponent is not engaged then tell it to get out of the
		// the cell.
		if (pOpp->GetDriveMode () != DRIVEMODE_ENGAGED && pCell == pOpp->GetCell())
			pOpp->GetOut();
		pOpp = pOpp->Next();
	}

	// Any current opponents that are waiting to enter a
	// cell from a previous BuildShot() can now be released
	m_pGroup->ReleaseShotCars();

	// Since we changed the shot cell build a new shot in case
	// the player enters the shot cell later.
	m_pGroup->BuildShot();

	return pCell;
}

//***********************************************************************
COpp::COpp()
//***********************************************************************
{
	// Initialize
	m_iInstPos = 0;
	m_pszDrivePat = NULL;
	m_iDriveMode = 0;
	m_iLastDriveMode = 0;
}

//***********************************************************************
void COpp::AdjustTick(void)
//***********************************************************************
{
	// 
	// Fine tune the tick the opponent car is on. If there is already
	// another car on the tick in the cell then find a new tick to be
	// on.  This method is usually just used when initializing the 
	// opponents so that we can see all of them on the radar. 
	//

	// If there is another car on the tick then try to find another tick
	if (OnTick(m_iTrackTick, m_xCell, m_yCell))
	{
		// try 0
		if (!OnTick(0, m_xCell, m_yCell))
			m_iTrackTick = 0;

		// try 1
		else if (!OnTick(1, m_xCell, m_yCell))
			m_iTrackTick = 1;

		// try 3
		else if (!OnTick(3, m_xCell, m_yCell))
			m_iTrackTick = 3;

		// try 4
		else if (!OnTick(4, m_xCell, m_yCell))
			m_iTrackTick = 4;

		// else stay in the middle
		else 
			m_iTrackTick = 2;
	}
}

//***********************************************************************
BOOL COpp::OnTick(int iTick, int xCell, int yCell)
//***********************************************************************
{
	//
	// Tests to see if there is an opponent on the given tick in
	// in the given cell
	//

	// For all opponents
	COpp *pOpp = m_pGroup->GetFirst();
	while(pOpp)
	{
		// Get the cell x,y
		POINT pnt = pOpp->GetXYCell();

		// If there is already someone there then return true
		if (pnt.x == xCell && 
			pnt.y == yCell &&
			iTick == pOpp->GetTick())
			return TRUE;

		pOpp = pOpp->Next();
	}

	// No one is on the given tick in the given cell
	return FALSE;
}

//***********************************************************************
short COpp::IncTick(void)
//***********************************************************************
{
	// 
	// Opponent version of increment the tick on the track.
	//

	// If the car is supposed to stay on the tick then just return
	if (m_bStayOnTick)
		return m_iTrackTick;

	// Increment possible next tick
	int m_iTick = m_iTrackTick + 1;
	if (m_iTick > TICKEND)
		m_iTick = TICKEND;

	// Make sure there are no other opponents on the next
	// tick.
	// NOTE: This loop may be ineffecient since IncTick() gets call
	// a lot. We may need to find a better way to test if another
	// opponent is on the same tick.
	COpp *pOpp = m_pGroup->GetFirst();
	while(pOpp)
	{
		POINT pnt = pOpp->GetXYCell();

		// If there is already someone there then don't inc tick
		if (pnt.x == m_xCell && 
			pnt.y == m_yCell &&
			m_iTick == pOpp->GetTick())
			return m_iTrackTick;

		pOpp = pOpp->Next();
	}

	// No one is on the tick so go ahead
	m_iTrackTick = m_iTick;

	return m_iTrackTick;
}

//***********************************************************************
void COpp::StartMove(void)
//***********************************************************************
{
	// 
	// Initialize the moving of the opponent
	//

	// Set the current time as the time 'last moved'
	m_dwLastMoveTime = timeGetTime();

	// Execute the first move instruction
	ExecuteMoveInstruct();

	// Setup for the next move instruction
	SetNextMoveInstruct();

	// Move the car a tick
	IncTick();
}

//***********************************************************************
BOOL COpp::Move(DWORD dwTime)
//***********************************************************************
{
	//
	// Move the opponent
	//

	// See first if it is time to move
	DWORD dwElapseTime = dwTime - m_dwLastMoveTime ;
	if (dwElapseTime < 1)
		return FALSE;

	// See how many ticks we should move based on the
	// elapsed time
	int iTicks = dwElapseTime / m_iSpeed;	
	if (iTicks < 1)
		return FALSE;

	// Temporary (safety check)
	if (iTicks > 5)
		iTicks = 5;

	// For all ticks
	for(int i=0; i<iTicks; i++)
	{
		// If the car is a drive mode that is MOVE, in other words one that
		// is following a driving pattern and is not in some special mode
		// such as engaged or positioned
		if (DRIVEMODETEST_MOVE(m_iDriveMode))
		{

			//Debug("move imode=%d\n",m_iDriveMode);

			// Get current tick type
			short iType = GetTickType();

			switch(iType)
			{

			// Turn tick = 2, This is the center of the cell. The car can
			// move a new direction on this tick.
			case TICKTURN:

				// If the car is in the proximity of the player and
				// we have not reached the maximum number of cars
				// that can be in proximity mode...
				if (InProximity() && !MaxProxim())
				{
					// If drive mode is not already proximity then set it
					if (m_iDriveMode != DRIVEMODE_PROXIM)
						SetDriveMode(DRIVEMODE_PROXIM);
				}

				// else not in proximity
				else
				{
					// If in proximity mode then set to restore drive mode
					if (m_iDriveMode == DRIVEMODE_PROXIM)
					{
						SetDriveMode(DRIVEMODE_RESTORE);
					}

					// if in restore drive mode...
					else if (m_iDriveMode == DRIVEMODE_RESTORE)
					{
						// If car reachs home then set mode to normal
						if (CheckHome())
							SetDriveMode(DRIVEMODE_NORMAL);
					}
				}

				// Execute the next move instruction
				ExecuteMoveInstruct();

				// Set up for a new move instruction for next time
				SetNextMoveInstruct();
				break;

			// End tick = 4. The car has reached the end of the cell
			// and probably needs to enter a new cell
			case TICKEND:
				StartTick();
				break;
			} // end switch eType
		
			// Move the car a tick
			IncTick();

		} // end if drivemodetest_move

		// Else the car is in a special drive mode and is not following
		// a normal driving pattern
		else
		{
			// if the car is trying to get ready to enter a player
			// cell then position it towards the entry gate
			if (m_iDriveMode == DRIVEMODE_POSITION)
			{
				MoveToShotCell();
			}
		}

	} // end for all ticks

	// Update the new last moved time for the next move
	m_dwLastMoveTime = timeGetTime();

	return TRUE;
}

//***********************************************************************
void COpp::MoveToShotCell(void)
//***********************************************************************
{
	//
	// Moves to the shot cell
	//

	// Set the direction to the shot cell
	CCell *pShotCell = m_pGroup->GetShotCell();
	if (pShotCell)
		// Set the direction
		SetBestDir(m_xCell, m_yCell, pShotCell->Getx(), pShotCell->Gety());

	// Move to the end of the track (to the gate)
	if (m_iTrackTick < TICKEND)
		++m_iTrackTick;
}

//***********************************************************************
void COpp::SetDriveMode(int iMode)
//***********************************************************************
{
	//
	// Sets the driving mode
	//

	// Get the opponent record for this opponent
	OpponentRec *pRec = m_pGroup->Rec(m_iRecIdx);
	
	// Set the driving mode
	m_iDriveMode = iMode;

	// If the driving mode is proximity...
	if (iMode == DRIVEMODE_PROXIM)

		// Set the proximity driving pattern
		m_pszDrivePat = pRec->szProximityDrivingPat;

	// else if the driving mode is restore
	else if (iMode == DRIVEMODE_RESTORE)
	{
		// Make sure that we aren't stuck on a tick
		m_bStayOnTick = FALSE;
		// Set the restore driving pattern
		m_pszDrivePat = pRec->szNormalRestore;
	}

	// Else set the normal driving pattern
	else
	{
		m_pszDrivePat = pRec->szNormalDrivingPat;
	}

	// Set for the first instruction
	m_iInstPos = 0;

	// Set the length of the instruction string
	m_iInstLen = lstrlen(m_pszDrivePat);

	// If the current instruction string is null then
	// default to the normal driving mode
	if (m_iInstLen == 0)
	{
		iMode = DRIVEMODE_NORMAL;
		m_pszDrivePat = pRec->szNormalDrivingPat;
		m_iInstLen = lstrlen(m_pszDrivePat);
	}

	// Set last move time to current time in case a lot
	// of time has passed since switching drive modes
	m_dwLastMoveTime = timeGetTime();
}

//***********************************************************************
void COpp::SetPrevDriveMode(void)
//***********************************************************************
{
	//
	// Restore the previous driving mode
	//
	short m_iTemp = m_iLastDriveMode;
	m_iLastDriveMode = m_iDriveMode;
	m_iDriveMode = m_iTemp;
}

//***********************************************************************
void COpp::SetNextMoveInstruct(void)
//***********************************************************************
{
	//
	// Set up for the next move instruction
	//
	++m_iInstPos;
	if (m_iInstPos >= m_iInstLen)
		m_iInstPos = 0;
}

//***********************************************************************
char COpp::GetMoveInstruct(void)
//***********************************************************************
{
	//
	// Returns the current move instruction
	//
	return m_pszDrivePat[m_iInstPos];
}

//***********************************************************************
BOOL COpp::ExecuteMoveInstruct(void)
//***********************************************************************
{
	//
	// Executes the current moving instruction
	//

	// Get the instruction
	char c = GetMoveInstruct();

	//Debug("%c ", c);

	// Make sure we aren't stuck on a tick, this is only set if the 'S'
	// (Stay) instruction is executed
	m_bStayOnTick = FALSE;

	switch(c)
	{
	// Forward, direction does not change so do nothing
	case 'F':
		break;

	// Forward Left
	case 'L':
		ChangeDirRel(-2);
		break;

	// Back Left
	case 'E':
		ChangeDirRel(-4);
		break;

	// Forward Right
	case 'R':
		ChangeDirRel(2);
		break;

	// Back Right
	case 'I':
		ChangeDirRel(4);
		break;

	// Back
	case 'B':
		//Debug("Back\n");
		ChangeDirRel(6);
		break;

	// Attack
	case 'A':
		SetAttackDir();
		break;

	// Home
	case 'H':
		SetHomeDir();
		break;

	// Retreat (coward)
	case 'C':
		SetCowardDir();
		break;

	// Clockwise perimeter
	case 'P':
		SetClockwisePerimDir();
		break;

	// Stay
	case 'S':
		m_bStayOnTick = TRUE;
		break;

	// Restores previous driving mode
	case 'M':
		SetPrevDriveMode();
		break;
	}

	return TRUE;
}

//***********************************************************************
BOOL COpp::EnterCell(void)
//***********************************************************************
{
	// 
	// Opponent tries to enter a cell from it's current cell and
	// direction.
	//

	// Get the next potential cell to travel to
	POINT pnt = NextCellFromDir(m_xCell, m_yCell, m_iDir);

	if (m_pGroup)
	{
		CCell* pCell = App.pGame->lpWorld->GetCell(pnt.x, pnt.y);

		// If this is a valid cell to move to...
		if (ValidMoveCell(pCell))
		{
			// Set the opopnent in that cell
			m_xCell = (short)pnt.x;
			m_yCell = (short)pnt.y;
			m_pCell = pCell;

			// NOW, see if this new cell is adjacent to 
			// the shot cell if it is then rebuild the 
			// shot
			if (CheckForShot())
				m_pGroup->BuildShot();

			return TRUE;
		}
		// Else couldn't enter intended cell
		else
		{
			//Set driving mode to restore
			SetDriveMode(DRIVEMODE_RESTORE);
			// This finds a valid direction to move to
			SetValidMoveDir();
		}
	}

	return FALSE;
}

//***********************************************************************
BOOL COpp::GetOut(void)
//***********************************************************************
{
	//
	// Moves the opponent car out of the player cell
	//

	// Set a direction the same as the player's
	short iOppPlayDir = App.pGame->lpWorld->GetPlayer()->GetDir();
	// Used to set opposite of player direction for some reason
	//short iOppPlayDir = pPlayer->GetDir() + 6;
	//if (iOppPlayDir > 12)
	//	iOppPlayDir -= 12;

	// Try to enter a cell, keep tring different directions
	// until a cell is found that is safe to move to
	for(int i=0; i<13; i++)
	{
		if (m_iDir != iOppPlayDir)
			if (EnterCell())
				break;

		ChangeDirRel(1);
	}

	// I guess we didn't find a cell - whoops!
	if (i == 13)
		return FALSE;

	// Set to start of the track in the new cell
	m_iTrackTick = 0;

	return TRUE;
}

//***********************************************************************
BOOL COpp::CheckHome(void)
//***********************************************************************
{
	//
	// See if we are at our home cell
	if (m_xCell == m_xHomeCell &&
		m_yCell == m_yHomeCell)
	{
		// If we are then set the home direction
		m_iDir = m_iHomeDir;
		return TRUE;
	}

	return FALSE;
}

//***********************************************************************
void COpp::SetHomeDir(void)
//***********************************************************************
{
	// Set the direction towards home
	SetBestDir(m_xCell, m_yCell, m_xHomeCell, m_yHomeCell);
}

//***********************************************************************
void COpp::SetAttackDir(void)
//***********************************************************************
{
	//
	// Set the attack direction. 
	//
	
	// Get the shot cell
	CCell *pShotCell = m_pGroup->GetShotCell();
	if (pShotCell)
	{
		//POINT pt = NextCellFromDir(pShotCell->Getx(), pShotCell->Gety(), 
		//	App.pGame->lpWorld->GetPlayer()->GetDir());

		// Set the direction
		//SetBestDir(m_xCell, m_yCell, pt.x, pt.y);
		SetBestDir(m_xCell, m_yCell, pShotCell->Getx(), pShotCell->Gety());
	}
}

//***********************************************************************
void COpp::SetCowardDir(void)
//***********************************************************************
{
	//
	// Set the coward direction - away from the player
	//

	// Get the x,y of the player cell
	POINT pnt = App.pGame->lpWorld->GetPlayer()->GetXYCell();

	// Set the direction
	SetBestDir(m_xCell, m_yCell, pnt.x, pnt.y);

	// Then set the opposite of that direction
	ChangeDirRel(6);
}

//***********************************************************************
void COpp::SetClockwisePerimDir(void)
//***********************************************************************
{
	// 
	// Clockwise peremiter direction. All it really does is try to go left
	// and if it can't then it searchs for a direction in the clockwise
	// direction that it can go to.
	//

	// Set left dir
	ChangeDirRel(-2);

	// See if we can go that way
	POINT pnt = NextCellFromDir(m_xCell, m_yCell, m_iDir);
	CCell *pCell = App.pGame->lpWorld->GetCell(pnt.x, pnt.y);
	if (!ValidMoveCell(pCell))
	{
		// Couldn't move, so travel clockwise
		for(int i=0; i<6; i++)
		{
			ChangeDirRel(2);
			pnt = NextCellFromDir(m_xCell, m_yCell,  m_iDir);
			pCell = App.pGame->lpWorld->GetCell(pnt.x, pnt.y);
			if (ValidMoveCell(pCell))
				break;
		}
	}
}

//***********************************************************************
void COpp::SetBestDir(int xSrcCell, int ySrcCell, int xDestCell, int yDestCell)
//***********************************************************************
{	//
	// Figure best direction based off of current cell coords and
	// home cell coords
	//

	// Get the x and y difference of the destination and source cells
	int xDif = xDestCell - xSrcCell;
	int yDif = yDestCell - ySrcCell;

	// Now figure the direction based on the differences

	if (xDif==0)
	{
		if (yDif<0)
			m_iDir = 12;
		else if (yDif>0)
			m_iDir = 6;
	}
	else if (xDif<0)
	{
		if (yDif < 0)
			m_iDir = 10;
		else if (yDif > 0)
			m_iDir = 8;
		// if on same row and current cell is odd, then adjacent
		// cell is above
		else if (yDif == 0 && (m_xCell & 1))
			m_iDir = 10;
		else
			m_iDir = 8;
	}
	// x > 0
	else
	{
		if (yDif < 0)
			m_iDir = 2;
		else if (yDif > 0)
			m_iDir = 4;
		// if on same row and current cell is odd, then adjacent
		// cell is above
		else if (yDif == 0 && (m_xCell & 1))
			m_iDir = 2;
		else
			m_iDir = 4;
	}

	// Make sure direction is valid, if it is not then try to find
	// a new one by progressively looking on the left and then
	// the right of the desired direction.
	POINT pnt = NextCellFromDir(m_xCell, m_yCell,  m_iDir);
	CCell *pCell = App.pGame->lpWorld->GetCell(pnt.x, pnt.y);
	if (!ValidMoveCell(pCell))
	{
		int iSign=1;
		for(int i=2; i<=10; i+=2)
		{
			ChangeDirRel(iSign * i);
			iSign *= -1;
			pnt = NextCellFromDir(m_xCell, m_yCell,  m_iDir);
			pCell = App.pGame->lpWorld->GetCell(pnt.x, pnt.y);
			if (ValidMoveCell(pCell))
				break;
		}
	}
}

//***********************************************************************
BOOL COpp::ValidMoveCell(CCell *pCell)
//***********************************************************************
{	//
	// Tests a cell to see if an opponent can move there.
	//

	// If cell is null, then of course we can't go there
	if (pCell == NULL)
		return FALSE;

	// If the cell is the player and the opponent is not engaged with
	// the player then the opponent can not go there
	else
	if (pCell == App.pGame->lpWorld->GetPlayer()->GetCell() &&
		m_iDriveMode != DRIVEMODE_ENGAGED)
		return FALSE;

	// If the cell is the shot cell and the opponent is not in position
	// or not engaged then the opponent can not go there
	else if (pCell == m_pGroup->m_pShotCell &&
		m_iDriveMode != DRIVEMODE_POSITION &&
		m_iDriveMode != DRIVEMODE_ENGAGED)
		return FALSE;

	// If the cell is a hex cell then it is safe
	else if (pCell->Type() == 'H')
		return TRUE;

	// If the cell is a union jack then it is safe
	else if (pCell->Type() == 'U')
		return TRUE;

	// If the cell is a tunnel then it is safe
	else if (pCell->Type() == 'T')
		return TRUE;

	return FALSE;
}

//***********************************************************************
void COpp::SetValidMoveDir(void)
//***********************************************************************
{	//
	// Finds a valid move direction
	//

	// Get the next cell from the current cell and direction
	POINT pnt = NextCellFromDir(m_xCell, m_yCell,  m_iDir);
	CCell *pCell = App.pGame->lpWorld->GetCell(pnt.x, pnt.y);

	// If the car can not go there
	if (!ValidMoveCell(pCell))
	{
		// Progressively look to the left and to the right of the desired
		// direction until a valid direction is found
		int iSign=1;
		for(int i=2; i<=10; i+=2)
		{
			ChangeDirRel(iSign * i);
			iSign *= -1;
			pnt = NextCellFromDir(m_xCell, m_yCell,  m_iDir);
			pCell = App.pGame->lpWorld->GetCell(pnt.x, pnt.y);
			if (ValidMoveCell(pCell))
				break;
		}
	}
}

//***********************************************************************
BOOL COpp::InProximity(void)
//***********************************************************************
{	//
	// Test to see if the opponent is within it's proximity value of
	// the player.

	// Get the opponent record and the proximity value
	OpponentRec *pRec = m_pGroup->Rec(m_iRecIdx);
	int nProx = pRec->nProximityTolerance;

	// Get the player and his cell position
	POINT pnt = App.pGame->lpWorld->GetPlayer()->GetXYCell();

	// If the x and y distance are both within the proximity value
	// then return true
	if (abs(pnt.x - m_xCell) <= nProx)
		if (abs(pnt.y - m_yCell) <= nProx)
			return TRUE;
	return FALSE;
}

//***********************************************************************
BOOL COpp::MaxProxim(void)
//***********************************************************************
{	//
	// Makes sure that the maximum number of opponents in proximity mode
	// have not been reached.
	// NOTE: A counter would be more effecient then looping through all 
	// of the opponents
	//

	int nProximCnt = 0;
	COpp *pOpp = m_pGroup->GetFirst();

	// For all opponents
	while(pOpp)
	{
		// If the car is in proximity mode then count it
		if (pOpp->m_iDriveMode == DRIVEMODE_PROXIM)
			++nProximCnt;
		pOpp = pOpp->Next();
	}

	// If maximum in proximity then return true
	if (nProximCnt > 5)
		return TRUE;

	return FALSE;
}

//***********************************************************************
OppCarShot *COpp::CheckForShot(void)
//***********************************************************************
{	//
	// Check to see if the car is position for for a shot. If it is 
	// then an opponent type - gate position (like WC) information are
	// set and returned.
	//

	static OppCarShot Shot;
	int iCellPos = 0;

	// If engaged then not a candidate for a new shot
	if (m_iDriveMode == DRIVEMODE_ENGAGED)
		return NULL;

	// Get Shot Cell
	CCell *pShotCell = m_pGroup->GetShotCell();
	if (pShotCell)
	{
		int xDiff = m_xCell - pShotCell->Getx();
		int yDiff = m_yCell - pShotCell->Gety();

		// if current shot cell is x odd, then adjacent cells
		// are above
		if (pShotCell->Getx() & 1)
		{
			// In 12 cell?
			if (xDiff == 0 && yDiff == -1)
				iCellPos = 12;
			
			// In 2 cell?
			else if (xDiff == 1 && yDiff == 0)
				iCellPos = 2;

			// In 4 cell?
			else if (xDiff == 1 && yDiff == 1)
				iCellPos = 4;

			// In 6 cell?
			else if (xDiff == 0 && yDiff == 1)
				iCellPos = 6;

			// In 8 cell?
			else if (xDiff == -1 && yDiff == 1)
				iCellPos = 8;

			// In 10 cell?
			else if (xDiff == -1 && yDiff == 0)
				iCellPos = 10;
		}
		// in even x cell
		else
		{
			// In 12 cell?
			if (xDiff == 0 && yDiff == -1)
				iCellPos = 12;

			// In 2 cell?
			else if (xDiff == 1 && yDiff == -1)
				iCellPos = 2;

			// In 4 cell?
			else if (xDiff == 1 && yDiff == 0)
				iCellPos = 4;

			// In 6 cell?
			else if (xDiff == 0 && yDiff == 1)
				iCellPos = 6;

			// In 8 cell?
			else if (xDiff == -1 && yDiff == 0)
				iCellPos = 8;

			// In 10 cell?
			else if (xDiff == -1 && yDiff == -1)
				iCellPos = 10;
		}
	}

	// If the car is in an adjacent cell to the shot cell
	if (iCellPos)
	{
		// Figure gate and convert gate to an acceptable character
		short iGate = iCellPos - App.pGame->lpWorld->GetPlayer()->GetDir();
		if (iGate < 1)
			iGate += 12;
		else
		if (iGate > 12)
			iGate -= 12;
		if (iGate > 9)
			iGate = 'A' + (iGate - 10);
		else
			iGate = '0' + iGate;

		// Fill in the shot car record info
 		Shot.cGate = (char)iGate;
		Shot.cType = m_cCarType;
		Shot.pOpp = this;
		Shot.nState = 0;

		return &Shot;
	}

	// The car was not a potential shot car
	return NULL;
}

//***********************************************************************
void COpp::SendIn(void)
//***********************************************************************
{	//
	// Sends the opponent into the player's cell for engagement...and
	// I'm not talking wedding...:-)
	// 

	// Set as engaged
	SetDriveMode(DRIVEMODE_ENGAGED);

	// Move car into cell. Assume the car is already positioned at
	// the portal of the adjacent cell.
	m_iTrackTick = 1;

	// Get player cell
	CCell *pCell = App.pGame->lpWorld->GetPlayer()->GetCell();
	if (pCell)
	{
		// Make sure the correct direction is set.
		SetBestDir(m_xCell, m_yCell, pCell->Getx(), pCell->Gety());

		// Set to the player's cell
		m_xCell = pCell->Getx();
		m_yCell = pCell->Gety();

		// Set the correct opponent gate code (Flip opponent dir)
		pCell->SetOppDir(m_iDir - 6, m_iRecIdx + '0');
	}

	// Add the opponent to the engaged list
	m_pGroup->AddEngaged(this);
}

//***********************************************************************
void COpp::FindNewHome(void)
//***********************************************************************
{
	// Finds a new home for the opponent by seeing where the player is
	// and making that the new home.
	
	if (m_pGroup)
	{
		CCell *pCell = App.pGame->lpWorld->GetPlayer()->GetCell();
		if (pCell)
		{
			m_iHomeDir = GetRandomNumber(12);
			if (m_iHomeDir == 0)
				m_iHomeDir = 12;
			m_iHomeDir &= 0xfffe; // make sure even
			m_xHomeCell = pCell->Getx();
			m_yHomeCell = pCell->Gety();
		}
	}
}

//***********************************************************************
COppGenerator::COppGenerator(CCell *pCell, DWORD dwFrequency, 
							 COpponentGroup *pGroup, 
							 int iFirstOpp, int iLastOpp)
//***********************************************************************
{
	m_pNext = NULL;
	m_dwFrequency = dwFrequency;
	m_dwLastCheck = timeGetTime();
	m_pCell = pCell;
	if (iFirstOpp == -1)
	{
		m_iFirstOpp = 0;
		m_iLastOpp = pGroup->m_iNumRecs - 1;
	}
	else
	{
		m_iFirstOpp = iFirstOpp;
		m_iLastOpp = iLastOpp;
	}
	m_iLastIndex = iLastOpp;
	m_bOn = TRUE;
}

//***********************************************************************
void COppGenerator::Check(DWORD dwTime, COpponentGroup *pGroup)
//***********************************************************************
{
	if (!m_bOn || !m_pCell)
		return;

	// If it is time to create a new opponent
	if ((dwTime - m_dwLastCheck) > m_dwFrequency)
	{
		m_dwLastCheck = dwTime;

		++m_iLastIndex;
		if (m_iLastIndex > m_iLastOpp)
			m_iLastIndex = m_iFirstOpp;
		
		// Make sure there are not too many opponents in the cell
		if (pGroup->OppsInCell(m_pCell) < 4)
		{
			COpp *pOpp = pGroup->CreateOpp(m_iLastIndex, 
				m_pCell->Getx(), m_pCell->Gety());

			// Find a home for the opponent
			if (pOpp)
				pOpp->FindNewHome();
		}
	}
}		
