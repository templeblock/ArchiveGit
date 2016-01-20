#include <windows.h>
#include "proto.h"
#include "cllsn.h"
#include "cllsnid.h"
#include "cllsndef.h"
#include "worlds.h"		
#include "Cells.h"
#include "control.h"
#include "collect.h"

/**********************************************************************************************/
CCollectibles::CCollectibles()
/**********************************************************************************************/
{
	Init();
}

/**********************************************************************************************/
CCollectibles::~CCollectibles()
/**********************************************************************************************/
{
}

/**********************************************************************************************/
void CCollectibles::Init()
/**********************************************************************************************/
{
	for ( int i=0; i<MAX_COLLECTIBLES; i++ )
	{
		Collectible[i].id = NULL;
		Collectible[i].bTimedLife = FALSE;
		Collectible[i].iWarningCount = 0;
		Collectible[i].lLifeTime = 0;
		Collectible[i].lStartTime = 0;
	}
}

/**********************************************************************************************/
void CCollectibles::ReInit(HWND hWnd)
/**********************************************************************************************/
{
	Init();
	for (int i=0; i<MAX_COLLECTIBLES; i++)
	{ // The collectible won't paint if id is invalid (i.e., hidden)
		HWND hControl = GetDlgItem(hWnd, IDC_COLLECT01 + i);
		SendMessage( hControl, SM_SETICONID, (WPARAM)-1, 0L );
	}
}

/**********************************************************************************************/
void CCollectibles::AddItem(HWND hWnd, int iLevel, int iCollectible)
/**********************************************************************************************/
{
	ITEMID idCollectible = (IDC_LEVEL_BASE + ((iLevel - 1) * 100) + (10 + iCollectible));
	Add(hWnd, idCollectible, NO/*bFromBottom*/);

//	Levels.SetState(LEVENT_GET_COLL, iCollectible);
}

/**********************************************************************************************/
void CCollectibles::Add(HWND hWnd, ITEMID idCollectible, BOOL bFromBottom)
/**********************************************************************************************/
{
	int iOpenSlot = -1;
	for (int i=0; i<MAX_COLLECTIBLES; i++)
	{
		if (Collectible[i].id == idCollectible)
			return;
		if (!Collectible[i].id && (iOpenSlot == -1))
			iOpenSlot = i;
	}

	if ( iOpenSlot == -1 )
		return;

	Collectible[iOpenSlot].id = idCollectible;
	HWND hControl = GetDlgItem(hWnd, IDC_COLLECT01 + iOpenSlot);
	SendMessage( hControl, SM_SETICONID, idCollectible, 0L );
}

/**********************************************************************************************/
void CCollectibles::RemoveItem(HWND hWnd, int iLevel, int iCollectible)
/**********************************************************************************************/
{
	ITEMID idCollectible = (IDC_LEVEL_BASE + ((iLevel - 1) * 100) + (10 + iCollectible));
	Remove(hWnd, idCollectible);

//	Levels.SetState(LEVENT_LOSE_COLL, iCollectible);
}

/**********************************************************************************************/
void CCollectibles::Remove(HWND hWnd, ITEMID idCollectible)
/**********************************************************************************************/
{
	for (int i=0; i<MAX_COLLECTIBLES; i++)
	{
		if (Collectible[i].id == idCollectible)
		{ // The collectible won't paint if id is invalid (i.e., hidden)
			HWND hControl = GetDlgItem(hWnd, IDC_COLLECT01 + i);
			SendMessage( hControl, SM_SETICONID, (WPARAM)-1, 0L );

			Collectible[i].id = NULL;
			Collectible[i].bTimedLife = FALSE;
			Collectible[i].iWarningCount = 0;
			Collectible[i].lLifeTime = 0;
			Collectible[i].lStartTime = 0;
		}
	}
}

/**********************************************************************************************/
void CCollectibles::SaveAll(void)
/**********************************************************************************************/
{
	for (int i=0; i<MAX_COLLECTIBLES; i++)
	{
		ITEMID id = Collectible[i].id;
		// Save the collected item's id
		char szKey[32];
		char szValue[32];
		wsprintf (szKey, "Collect%d", i);
		wsprintf (szValue, "%d", id);
		WritePrivateProfileString (OPTIONS_SECTION, szKey, szValue, INI_FILE);
	}
}

/**********************************************************************************************/
ITEMID CCollectibles::GetID(int index)
/**********************************************************************************************/
{
	if ( index < 0 || index > MAX_COLLECTIBLES )
		return NULL;
	return Collectible[index].id;
}


/**********************************************************************************************/
void CCollectibles::SetTimedLife(short iCollNum, short iWarningCount, long lTime)
/**********************************************************************************************/
{
	if ( iCollNum >= 1 && iCollNum <= MAX_COLLECTIBLES )
	{
		int i = iCollNum - 1;
		Collectible[i].bTimedLife = TRUE;
		Collectible[i].iWarningCount = iWarningCount;
		Collectible[i].lLifeTime = lTime;
		Collectible[i].lStartTime = timeGetTime();
	}
}

/**********************************************************************************************/
void CCollectibles::AdjustTimedLife(HWND hWnd)
/**********************************************************************************************/
{
	for (int i=0; i<MAX_COLLECTIBLES; i++)
	{
		if (!Collectible[i].bTimedLife)
			continue;

		long lDuration = timeGetTime() - Collectible[i].lStartTime;
		if (lDuration <= Collectible[i].lLifeTime)
			continue;

		// Our collectible's timed life expired...

		if (--Collectible[i].iWarningCount > 0)
		{ // If we should be warning the player...
			// Play warning sound
			// Flash Icon
			// The remaining warnings will be 1.5 seconds apart
			Collectible[i].lLifeTime = 1500;
			Collectible[i].lStartTime = timeGetTime();
		}
		else
		{ // No more warnings; player loses the collectible
			Collectible[i].bTimedLife = FALSE;
			RemoveItem(hWnd, App.pGame->lpWorld->GetLevel(), i);
		}
	}
}
