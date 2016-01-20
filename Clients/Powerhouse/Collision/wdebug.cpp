#include <windows.h>
#include <commctrl.h>
#include "proto.h"
#include "weapon.h"
#include "cllsnid.h"
#include "cllsn.h"
#include "control.h"
#include "wdebug.p"
#include "author.p"
#include "res_wdebug.h"
#include "res_author.h"
#include "line.p"

#define EMPTY_POS 999

static HWND hWndDialog = NULL;
static CWeapon *pWeapon = NULL;
static HWND hSceneWindow = NULL;
static LPVIDEO lpVid = NULL;
static LPSHOT lpCurShot = NULL;
static BOOL bTestEvent = FALSE;
static BOOL bTestEvent2 = FALSE;
static long lWCurrFrame = 0;			// Current frame (used for testing event shots)
static long lTestEventFrom = 0;	// Test event from frame
static long lTestEventTo = 0;	// Test event to frame

static long lWStartFrame = 0;	// Current start frame
static long lWEndFrame = 0;		// Current end frame

static char szCopyFile[200];	// File name used to copy and paste data

static int g_iCurCoords = -1;	// Current set of coords to manipulate

BOOL WDebugOnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
void WDebugOnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
BOOL WINPROC EXPORT WDebugDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL WDebugOnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
static void SetCurrentFrame(void);
static void SetCurrentShotFromTree(NM_TREEVIEW FAR *pView);
static void Refresh(void);
static void ClearWallTweenFrInfo(void);
static void ClearPlayerTweenFrInfo(void);
static void ClearAllFrInfo(void);
static void TweenPlayerZ(void);
static void GetCurFrames(void);
static void SetFrameInfo(void);
static void Save3DShotData(void);
static void Get3DShotData(void);
static BOOL WallEmpty(WALL *pWall);
static void Play(void);
static  void TweenWalls(void);
static BOOL EnemyEmpty(ENEMY *pEnemy);
static void SetEnemy(HWND hWnd);
static void SetWalls(HWND hWnd);
static void ClearEnemyTweenFrInfo(void);
static void TweenEnemy(void);
static void ClearPlayerK(void);
static void ClearEnemyK(void);
static void ClearWallK(void);
static void CopyRec(void);
static void PasteRec(void);
static void AllZs(void);
static void ChangeZ(int iZ);
static void ChangeX(int iX);

static const int nMaxPoints = nMaxWalls * 2;	// Maximum # of points
//static WALL Walls[nMaxWalls];

static int nLastFrame = 0;
static FRAMEINFO FrInfo[nMaxFrame];

/************************************************************************/
void WeaponDebugStart(HWND hWnd, CWeapon *_pWeapon, LPVIDEO _lpVid)
/************************************************************************/
{
	if (hWndDialog)
		return;

	pWeapon = _pWeapon;
	hSceneWindow = hWnd;
	lpVid = _lpVid;
	hWndDialog = DialogStart( YES, GetWindowInstance(hWnd), hWnd, IDD_WDEBUG, WDebugDlgProc);
	DialogShow( IDD_WDEBUG);
	pWeapon->SetShowGrid(TRUE);
}

/************************************************************************/
void WeaponDebugEnd(void)
/************************************************************************/
{
	if (hWndDialog)
		DialogEnd( hWndDialog, IDOK);
	hWndDialog = NULL;
}


/************************************************************************/
BOOL WDebugOnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	SendDlgItemMessage(hWnd, IDC_SHOWGRID, BM_SETCHECK, 1, 0);
	SendDlgItemMessage(hWnd, IDC_USEAVI, BM_SETCHECK, 0, 0);
	pWeapon->SetUseDebug(TRUE);

	HWND hTree = GetDlgItem(hWnd, IDC_WDSHOTLIST);

	LoadTree(hTree, lpVid);

	// Size everything and lay it out for maximum use of space
	int nCxScreen = GetSystemMetrics(SM_CXSCREEN);
	int nCyScreen = GetSystemMetrics(SM_CYFULLSCREEN);

	RECT Rect;
	GetWindowRect(hWnd, &Rect);

	// Set collision window into upper left corner
	MoveWindow(GetParent(hSceneWindow), 0, 0, 640, 480, TRUE);
	MoveWindow(hSceneWindow, 0, 0, 640, 480, TRUE);

	// Set author dialog in the lower right corner
	MoveWindow(hWnd, nCxScreen - (Rect.right - Rect.left), nCyScreen - (Rect.bottom - Rect.top),
		Rect.right - Rect.left, Rect.bottom - Rect.top, TRUE);

	SetDlgItemInt(hWnd, IDC_DIFAMOUNT, 1, TRUE);

	return TRUE;
}

/************************************************************************/
void WDebugOnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
	if (!App.pGame)
		return;

	BOOL bTranslated;

	pWeapon = App.pGame->Weapons.CurWeapon();

	if (id == IDOK) // Set enemy
	{
		int x = GetDlgItemInt(hWnd, IDC_ENEMYX, &bTranslated, TRUE);
		int z = GetDlgItemInt(hWnd, IDC_ENEMYZ, &bTranslated, TRUE);
		int y = GetDlgItemInt(hWnd, IDC_ENEMYY, &bTranslated, TRUE);
		int nWidth = GetDlgItemInt(hWnd, IDC_ENEMYWIDTH, &bTranslated, FALSE);
		int nHeight = GetDlgItemInt(hWnd, IDC_ENEMYHEIGHT, &bTranslated, FALSE);
		int nDepth = GetDlgItemInt(hWnd, IDC_ENEMYDEPTH, &bTranslated, FALSE);
		//pWeapon->SetTestEnemy(0, x, y, z, nWidth, nHeight, nDepth);

		//pWeapon->SetEnemy(0, nFrameDist, nStartX, nStartY, nStartZ, nEndX, nEndZ, nWidth, nHeight, nDepth);
		App.pGame->VWorld.SetEnemyTest(TRUE);
		Refresh();
	}
	else if (id == IDC_TWEENPLAYER)
	{
		TweenPlayerZ();
	}
	else if (id == IDC_TWEENWALLS)
	{
		TweenWalls();
	}
	else if (id == IDC_TWEENENEMY)
	{
		TweenEnemy();
	}
	else if (id == IDC_CLEARPLAYERK)
	{
		ClearPlayerK();
	}
	else if (id == IDC_CLEARENEMYK)
	{
		ClearEnemyK();
	}
	else if (id == IDC_CLEARWALLK)
	{
		ClearWallK();
	}
	else if (id == IDC_CLEARPLAYERTWEEN)
	{
		ClearPlayerTweenFrInfo();
	}
	else if (id == IDC_CLEARWALLTWEEN)
	{
		ClearWallTweenFrInfo();
	}
	else if (id == IDC_CLEARTWEENENEMY)
	{
		ClearEnemyTweenFrInfo();
	}
	else if (id == IDC_SETPLAYER)
	{	
		BOOL bTrans;

		// Make sure frames are current
		GetCurFrames();

		// Figure offset
		long lOffset = lWCurrFrame - lWStartFrame;
		if (lOffset >= 0 && lOffset < (long)nMaxFrame)
		{
			// Set z value
			FrInfo[(int)lOffset].nPlayerZ = GetDlgItemInt(hWnd, IDC_PLAYERZ, &bTrans, TRUE);
			FrInfo[(int)lOffset].fType |= FRTYPE_PLAYERZKEY;
			Refresh();
		}

	}
	else if (id == IDC_SHOWGRID)
	{
		long lCheck = SendDlgItemMessage(hWnd, IDC_SHOWGRID, BM_GETCHECK, 0, 0);
		if (lCheck == 0)
			pWeapon->SetShowGrid(FALSE);
		else
			pWeapon->SetShowGrid(TRUE);

		App.pGame->VWorld.SetEnemyTest(FALSE);
		Refresh();
	}
	else if (id == IDC_USEAVI)
	{
		long lCheck = SendDlgItemMessage(hWnd, IDC_USEAVI, BM_GETCHECK, 0, 0);
		pWeapon->SetUseDebug((BOOL)!lCheck);
	}
	else if (id == IDC_SETENEMY)
	{
		SetEnemy(hWnd);
	}
	else if (id == IDC_SETWORLD)
	{
		SetWalls(hWnd);
	}
	else if (id == IDC_JUMPSHOT)
	{
		STRING szString;

		HWND hVideo = GetDlgItem( hSceneWindow, IDC_VIDEO );
		if (!hVideo)
			return;

		LPVIDEO lpVideo = (LPVIDEO)GetWindowLong(hVideo, GWL_DATAPTR);
		if (!lpVideo)
			return;

		GetDlgItemText(hWnd, IDC_SHOTNAME, szString, sizeof(szString));
  
		LPSHOT lpShot = Video_FindShotName( hVideo, szString );
		if ( !lpShot )
			return;

		Video_GotoShot( hVideo, lpVideo, lpShot->lShotID, 1/*lCount*/, 0/*iFrames*/, 0 );
	}
	else if (id == IDC_SETTESTWORLD)
	{
		long lTestWorld = GetDlgItemInt(hWnd, IDC_TESTWORLD, &bTranslated, TRUE);
		App.pGame->VWorld.SetTestWorld(lTestWorld);
	}
	else if (id == IDC_SETPLAYER)
	{
		int pz = GetDlgItemInt(hWnd, IDC_PLAYERZ, &bTranslated, TRUE);
		Refresh();
	}
	else if (id == IDC_WDPLAY)
	{
		Play();
  	}
	else if (id == IDC_WDSTEPNEXT)
	{
		MCIStepForward( lpVid->hDevice, 1 );
		PostMessage(hWnd, WM_COMMAND, IDC_SETFRAMEINFO, 0);
	}
	else if (id == IDC_WDSTEPPREV)
	{
		MCIStepReverse( lpVid->hDevice );
		PostMessage(hWnd, WM_COMMAND, IDC_SETFRAMEINFO, 0);
	}
	else if (id == IDC_SETFRAMEINFO)
	{
		// Wait on avi to be done playing or 1 second
		// This is necessary to make sure the current frame
		// information is accurate
		DWORD dwMode = MCIGetMode( lpVid->hDevice );
		DWORD dwTime = timeGetTime();
		DWORD dwDif  = 0;
		MSG msg;
		while (dwMode != MCI_MODE_STOP && dwDif < 1000)
		{
			PeekMessage(&msg, (HWND)NULL, 0, 0, PM_NOREMOVE);
			dwMode = MCIGetMode( lpVid->hDevice);
			dwDif = timeGetTime() - dwTime;
		}

		GetCurFrames();

		SetCurrentFrame();
	}
	else if (id == IDC_FIRE)
	{
		pWeapon->Start();
	}
	else if (id == IDC_SAVEREC)
	{
		Save3DShotData();
	}
	else if (id == IDC_COPYREC)
	{
		CopyRec();
	}
	else if (id == IDC_PASTEREC)
	{
		PasteRec();
	}
	else if (id == IDC_WALLCRASH || id == IDC_CARCRASH)
	{
		int iRec = lWCurrFrame - lWStartFrame;

		long lCheck = SendDlgItemMessage(hWnd, IDC_WALLCRASH, BM_GETCHECK, 0, 0);
		if (lCheck)
		{
			FrInfo[iRec].nCommand = CMND_WALLCRASH;
		}
		else
		{
			lCheck = SendDlgItemMessage(hWnd, IDC_CARCRASH, BM_GETCHECK, 0, 0);
			if (lCheck)
				FrInfo[iRec].nCommand = CMND_CARCRASH;
			else
				FrInfo[iRec].nCommand = 0;
		}
	}
	else if (id == IDC_OTHERCODE)
	{
		int iRec = lWCurrFrame - lWStartFrame;
		int iCode = GetDlgItemInt(hWnd, IDC_OTHERCODE, &bTranslated, TRUE);
		if (iCode > -1)
			FrInfo[iRec].nCommand = iCode;
			

	}
	else if (id >= IDC_KEYFOC_1 && id <= IDC_KEYFOC_8)
	{
		g_iCurCoords = id - IDC_KEYFOC_1;
	}
	else if (id == IDC_ALLZ_100)
	{
		AllZs();
	}
	else if (id == IDC_ZP)
	{
		BOOL bTranslated;
		int iDif = GetDlgItemInt(hWndDialog, IDC_DIFAMOUNT, 
			&bTranslated, TRUE);
		ChangeZ(iDif);
	}
	else if (id == IDC_ZM)
	{
		BOOL bTranslated;
		int iDif = GetDlgItemInt(hWndDialog, IDC_DIFAMOUNT, 
			&bTranslated, TRUE);
		ChangeZ(-1 * iDif);
	}
	else if (id == IDC_XP)
	{
		BOOL bTranslated;
		int iDif = GetDlgItemInt(hWndDialog, IDC_DIFAMOUNT, 
			&bTranslated, TRUE);
		ChangeX(iDif);
	}
	else if (id == IDC_XM)
	{
		BOOL bTranslated;
		int iDif = GetDlgItemInt(hWndDialog, IDC_DIFAMOUNT, 
			&bTranslated, TRUE);
		ChangeX(-1 * iDif);
	}
}

/***********************************************************************/
BOOL WallEmpty(WALL *pWall)
/***********************************************************************/
{
	if (pWall->p1.x == 0 &&
		pWall->p1.z == 0 &&
		pWall->p2.x == 0 &&
		pWall->p2.z == 0)
		return TRUE;

	return FALSE;
}

/***********************************************************************/
void WDebugOnNotify(HWND hWnd, int iCtrl, NMHDR FAR* pHdr)
/***********************************************************************/
{
	if (iCtrl == IDC_WDSHOTLIST)
	{
		NM_TREEVIEW *pNMView = (NM_TREEVIEW *)pHdr;

		// If there was a selection change
		if (pNMView->hdr.code == TVN_SELCHANGED)
		{
			ClearAllFrInfo();
			SetCurrentShotFromTree(pNMView);
			Get3DShotData();
			BOOL bTranslated;
			long lFrom = GetDlgItemInt(hWnd, IDC_WDSTARTFRAME, &bTranslated, FALSE);
			long lTo = GetDlgItemInt(hWnd, IDC_WDENDFRAME, &bTranslated, FALSE);
			if (bTranslated)
			{
				// Show the 5th frame (get past the fog in most cases)
				if (lTo > (lFrom + 5))
					lTo = 5;
				else
					lTo = 0;
				MCIPlay(lpVid->hDevice, hWnd, lFrom, lFrom+lTo);
			}
		}
	}
}

/***********************************************************************/
BOOL WDebugOnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{			  
	switch(msg)
	{
	case WM_GETDLGCODE:
		return (BOOL)DLGC_WANTARROWS;
		break;
	}

	return (FALSE);
}


/***********************************************************************/
UINT WDebugOnMCINotify(HWND hwnd, UINT notifyCode, HMCI hDevice)
/***********************************************************************/
{
	if (notifyCode == MCI_NOTIFY_SUCCESSFUL)
	{

		GetCurFrames();
		SetCurrentFrame();

		// If we are in the middle of testing an event...
		//if (bTestEvent)
		//{
		//	bTestEvent = FALSE;
		//	bTestEvent2 = TRUE;
			// Play next sequence in the test
		//	MCIPlay( lpVid->hDevice, hWndDialog, lTestEventFrom, lTestEventTo );

		//}
		// Second half of test is done playing
		//else if (bTestEvent2)
		//{
		//	bTestEvent2 = FALSE;
		//}
		//else
		//{
		//}

		long lCheck = SendDlgItemMessage(hWndDialog, IDC_WDLOOP, BM_GETCHECK, 0, 0);
		if (lCheck == 1)
			Play();
	}
	return 1;
}


/***********************************************************************/
BOOL WINPROC EXPORT WDebugDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch (msg)
	{
		HANDLE_DLGMSG(hDlg, WM_INITDIALOG, WDebugOnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, WDebugOnCommand);
		HANDLE_DLGMSG(hDlg, WM_NOTIFY, WDebugOnNotify);
		HANDLE_MSG(hDlg, MM_MCINOTIFY, WDebugOnMCINotify);

		default:
			return(WDebugOnMessage(hDlg, msg, wParam, lParam));
		break;
	}
	return(FALSE);
}

/***********************************************************************/
void SetCurrentFrame(void)
/***********************************************************************/
{
	STRING szString;
	wsprintf( szString, "%ld", MCIGetPosition( lpVid->hDevice ) );
	SetDlgItemText( hWndDialog, IDC_CURRFRAME, szString );

	SetFrameInfo();
}

/***********************************************************************/
void SetCurrentShotFromTree(NM_TREEVIEW FAR *pView)
/***********************************************************************/
{
	LPSHOT lpShot;
	char szText[80];

	MCIStop( lpVid->hDevice, YES/*bWait*/ );
	pView->itemNew.mask = TVIF_TEXT;
	pView->itemNew.pszText = szText;
	pView->itemNew.cchTextMax = 80;

	BOOL bRet = TreeView_GetItem(pView->hdr.hwndFrom, &pView->itemNew);	
 
	// Get shot
	if (bRet)
	{
		lpShot = Video_FindShotName( lpVid->hWnd, pView->itemNew.pszText );
		if (lpShot)
		{
			//HWND hWnd;

			// Set shot name
			SetDlgItemText(hWndDialog, IDC_SHOTNAME, pView->itemNew.pszText);

			if (pWeapon)
				pWeapon->NewCell(pView->itemNew.pszText[1]);

			// Set start frame
			SetDlgItemInt(hWndDialog, IDC_WDSTARTFRAME, lpShot->lStartFrame, FALSE); 

			// Set end frame
			SetDlgItemInt(hWndDialog, IDC_WDENDFRAME, lpShot->lEndFrame, FALSE);

			// Set comment
			//SetDlgItemText(hWndDialog, IDC_DESCRIPTION, COMMENT(lpShot->lShotID-1));
		}
	}

	// Set current shot
	lpCurShot = lpShot;
	if (lpShot)
		Video_GotoShot( lpVid->hWnd, lpVid, lpShot->lShotID, 1/*lCount*/, 0/*iFrames*/, 0 );

}

/***********************************************************************/
void Refresh(void)
/***********************************************************************/
{
	if (!lpVid)
		return;

	InvalidateRect(lpVid->hWnd, NULL, FALSE);
	UpdateWindow(lpVid->hWnd);
}

/***********************************************************************/
void ClearWallTweenFrInfo(void)
/***********************************************************************/
{
	int i;
	int j;
	for (i=0; i<nMaxFrame; i++)
	{
		if ( !(FrInfo[i].fType & FRTYPE_WALLKEY) )
		{
			FrInfo[i].yGround = 0;
			FrInfo[i].nNumWalls = 0;
			for(j=0; j<nMaxWalls; j++)
			{
				FrInfo[i].Walls[j].p1.x=0;
				FrInfo[i].Walls[j].p1.z=0;
				FrInfo[i].Walls[j].p2.x=0;
				FrInfo[i].Walls[j].p2.z=0;
			}
		}
	}
}

/***********************************************************************/
void ClearPlayerK(void)
/***********************************************************************/
{
	int iRec = (int)lWCurrFrame - lWStartFrame;
	
	FrInfo[iRec].fType &= ~FRTYPE_PLAYERZKEY;
	FrInfo[iRec].nPlayerZ = EMPTY_POS;
	SetFrameInfo();
}

/***********************************************************************/
void ClearEnemyK(void)
/***********************************************************************/
{
	int iRec = (int)lWCurrFrame - lWStartFrame;
	
	FrInfo[iRec].fType &= ~FRTYPE_ENEMYKEY;
	for(int i=0; i<FrInfo[iRec].nNumEnemy; i++)
	{
		FrInfo[iRec].Enemy[i].x = 0;
		FrInfo[iRec].Enemy[i].y = 0;
		FrInfo[iRec].Enemy[i].z = 0;
		FrInfo[iRec].Enemy[i].w = 0;
		FrInfo[iRec].Enemy[i].h = 0;
		FrInfo[iRec].Enemy[i].d = 0;
	}
	FrInfo[iRec].nNumEnemy = 0;
	SetFrameInfo();
}

/***********************************************************************/
void ClearWallK(void)
/***********************************************************************/
{
	int iRec = (int)lWCurrFrame - lWStartFrame;
	
	FrInfo[iRec].fType &= ~FRTYPE_WALLKEY;
	for(int i=0; i<FrInfo[iRec].nNumWalls; i++)
	{
		FrInfo[iRec].Walls[i].p1.x=0;
		FrInfo[iRec].Walls[i].p1.z=0;
		FrInfo[iRec].Walls[i].p2.x=0;
		FrInfo[iRec].Walls[i].p2.z=0;
	}
	FrInfo[iRec].yGround = 0;
	FrInfo[iRec].nNumWalls = 0;
	SetFrameInfo();
}

/***********************************************************************/
void ClearPlayerTweenFrInfo(void)
/***********************************************************************/
{
	int i;
	for (i=0; i<nMaxFrame; i++)
	{
		if ( !(FrInfo[i].fType & FRTYPE_PLAYERZKEY) )
			FrInfo[i].nPlayerZ = EMPTY_POS;
	}
}

/***********************************************************************/
void ClearEnemyTweenFrInfo(void)
/***********************************************************************/
{
	int i;
	int j;
	for (i=0; i<nMaxFrame; i++)
	{
		if ( !(FrInfo[i].fType & FRTYPE_ENEMYKEY) )
		{
			FrInfo[i].nNumEnemy = 0;
			for(j=0; j<nMaxEnemies; j++)
			{
				FrInfo[i].Enemy[j].x=0;
				FrInfo[i].Enemy[j].z=0;
				FrInfo[i].Enemy[j].y=0;
				FrInfo[i].Enemy[j].w=0;
				FrInfo[i].Enemy[j].h=0;
				FrInfo[i].Enemy[j].d=0;
			}
		}
	}
}

/***********************************************************************/
void ClearAllFrInfo(void)
/***********************************************************************/
{
	int i;
	int j;
	for (i=0; i<nMaxFrame; i++)
	{
		FrInfo[i].fType = 0;
		FrInfo[i].nPlayerZ = EMPTY_POS;
		FrInfo[i].nNumWalls = 0;
		FrInfo[i].yGround = 0;
		for(j=0; j<nMaxWalls; j++)
		{
			FrInfo[i].Walls[j].p1.x=0;
			FrInfo[i].Walls[j].p1.z=0;
			FrInfo[i].Walls[j].p2.x=0;
			FrInfo[i].Walls[j].p2.z=0;
			FrInfo[i].Walls[j].L2 = 0.0;
		}
		FrInfo[i].nNumEnemy = 0;
		for(j=0; j<nMaxEnemies; j++)
		{
			FrInfo[i].Enemy[j].x = 0;
			FrInfo[i].Enemy[j].z = 0;
			FrInfo[i].Enemy[j].y = 0;
			FrInfo[i].Enemy[j].w = 0;
			FrInfo[i].Enemy[j].h = 0;
			FrInfo[i].Enemy[j].d = 0;
		}
	}
}

/***********************************************************************/
void TweenPlayerZ(void)
/***********************************************************************/
{
	int iFirst = -1;
	int iLast = -1;
	int iNum = 0;
	int iDelta = 0;
	int i;

	BOOL bDone = FALSE;
	BOOL bFirstTime = TRUE;

	double dDelta = 0.0;
	double dAccum = 0.0;

	nLastFrame = lWEndFrame - lWStartFrame + 1;
	if (nLastFrame >= nMaxFrame)
	{
		nLastFrame = nMaxFrame - 1;
		Print("Warning: Frames exceeded.");
	}

	while (!bDone)
	{
		// If first time through...
		if (bFirstTime)
		{
			bFirstTime = FALSE;

			// Find First
			for (i=iLast+1; i<nLastFrame; i++)
			{
				if ( FrInfo[i].nPlayerZ != EMPTY_POS)
				{
					iFirst = i;
					break;
				}

				// else fill with last value
				else
				{
					FrInfo[i].nPlayerZ = (int)dAccum;
				}
			}

		}
		else
			iFirst = iLast;

		// Find last
		for (i=iFirst+1; i<nLastFrame; i++)
		{
			if (FrInfo[i].nPlayerZ != EMPTY_POS)
			{
				iLast = i;
				break;
			}
		}

		// If that was the last frame then we are about done
		if (i == nLastFrame)
		{
			bDone = TRUE;
			iLast = iFirst + 1;

			// Replace all the remaing values with the last values
			// (dAccum)
			for(i=iLast; i<nLastFrame; i++)
			{
				FrInfo[i].nPlayerZ = (int)dAccum;
			}
		}
		else
		{
			// How many frames we working with?
			iNum = iLast - iFirst;

			dDelta = (double)(FrInfo[iLast].nPlayerZ - FrInfo[iFirst].nPlayerZ) / (double)iNum;
			dAccum = (double)FrInfo[iFirst].nPlayerZ;

			// Tween the values between the first and last frames
			for(i=iFirst+1; i<=iLast; i++)
			{
				dAccum += dDelta;
				FrInfo[i].nPlayerZ = (int)dAccum;
			}
		}
	}
}

/***********************************************************************/
void TweenWalls(void)
/***********************************************************************/
{
	WALL TempWalls[nMaxWalls];
	int nSaveYGround = 0;
	int nSaveNumWalls = 0;

	memset(TempWalls, 0, sizeof(TempWalls));

	nLastFrame = lWEndFrame - lWStartFrame + 1;
	if (nLastFrame >= nMaxFrame)
	{
		nLastFrame = nMaxFrame - 1;
		Print("Warning: Frames exceeded.");
	}

	int i;

	// Find first good set of wall info
	for(i=0; i<nLastFrame; i++)
	{
		// If there is wall information
		if ( !WallEmpty( &FrInfo[i].Walls[0] ) )
		{		
			// Save wall information
			memcpy( TempWalls, FrInfo[i].Walls, sizeof(TempWalls));
			nSaveYGround = FrInfo[i].yGround;
			nSaveNumWalls  = FrInfo[i].nNumWalls;
			break;
		}
	}

	// Now, for all frame infos
	for(i=0; i<nLastFrame; i++)
	{
		// If there is wall information
		if ( !WallEmpty( &FrInfo[i].Walls[0] ) )
		{		
			// Save wall information
			memcpy( TempWalls, FrInfo[i].Walls, sizeof(TempWalls));
			nSaveYGround = FrInfo[i].yGround;
			nSaveNumWalls  = FrInfo[i].nNumWalls;
		}
		// else
		{
			// Copy saved wall information
			memcpy( FrInfo[i].Walls, TempWalls, sizeof(TempWalls));
			FrInfo[i].yGround = nSaveYGround;
			FrInfo[i].nNumWalls = nSaveNumWalls;
		}
	}
}

/***********************************************************************/
int GetWDPlayerZ( long lFrame )
/***********************************************************************/
{
	// Compute relative frame
	if ((lFrame < lWStartFrame) || (lFrame > lWEndFrame))
		return 0;

	// Get the correct record
	int iRec = (int)lFrame - lWStartFrame;

	// Return the z coord for this frame
	if (FrInfo[iRec].nPlayerZ == EMPTY_POS)
		return 0;
	else
		return FrInfo[iRec].nPlayerZ;
}


/***********************************************************************/
void GetWDWalls(long lFrame, int *yGround, int *nNumWalls, WALL *pWalls)
/***********************************************************************/
{
	// Compute relative frame
	if ((lFrame < lWStartFrame) || (lFrame > lWEndFrame))
	{
		*nNumWalls = 0;
		return;
	}

	// Get the correct record
	int iRec = (int)lFrame - lWStartFrame;

	// Return the walls for this frame
	*nNumWalls  = FrInfo[iRec].nNumWalls;
	*yGround = FrInfo[iRec].yGround;
	memcpy(pWalls, FrInfo[iRec].Walls, sizeof(WALL)* (*nNumWalls));
}

/***********************************************************************/
void GetWDEnemy(long lFrame, int *nNumEnemy, ENEMY *pEnemy)
/***********************************************************************/
{
	// Compute relative frame
	if ((lFrame < lWStartFrame) || (lFrame > lWEndFrame))
	{
		*nNumEnemy = 0;
		return;
	}

	// Get the correct record
	int iRec = (int)lFrame - lWStartFrame;

	// Return the walls for this frame
	*nNumEnemy  = FrInfo[iRec].nNumEnemy;
	memcpy(pEnemy, FrInfo[iRec].Enemy, sizeof(ENEMY)* (*nNumEnemy));
}

/***********************************************************************/
void GetWDCommand(long lFrame, int *nCommand)
/***********************************************************************/
{
	GetCurFrames();

	int iRec = (int)lFrame - lWStartFrame;
	if (iRec < 0)
		return;

	if (iRec >= nMaxFrame)
		return;

	*nCommand = FrInfo[iRec].nCommand;
}

/***********************************************************************/
void SetFrameInfo(void)
/***********************************************************************/
{
	// Compute relative frame
	if ((lWCurrFrame < lWStartFrame) || (lWCurrFrame > lWEndFrame))
		return;

	// Get the correct record
	int iRec = (int)lWCurrFrame - lWStartFrame;

	if (FrInfo[iRec].nCommand == CMND_WALLCRASH)
		SendDlgItemMessage(hWndDialog, IDC_WALLCRASH, BM_SETCHECK, BST_CHECKED, 0);
	else
		SendDlgItemMessage(hWndDialog, IDC_WALLCRASH, BM_SETCHECK, BST_UNCHECKED, 0);

	if (FrInfo[iRec].nCommand == CMND_CARCRASH)
		SendDlgItemMessage(hWndDialog, IDC_CARCRASH, BM_SETCHECK, BST_CHECKED, 0);
	else
		SendDlgItemMessage(hWndDialog, IDC_CARCRASH, BM_SETCHECK, BST_UNCHECKED, 0);
	
	if (FrInfo[iRec].nCommand > CMND_ACTION)
		SetDlgItemInt(hWndDialog, IDC_OTHERCODE, FrInfo[iRec].nCommand, TRUE);
	else
		SetDlgItemInt(hWndDialog, IDC_OTHERCODE, (UINT)-1, TRUE);

	// Set the player z
	SetDlgItemInt(hWndDialog, IDC_PLAYERZ, FrInfo[iRec].nPlayerZ, TRUE);

	int iCtrl = IDC_W1_X1;
	int i;
	// Set the wall information
	for(i=0; i<FrInfo[iRec].nNumWalls; i++)
	{
		WALL *pWall = &FrInfo[iRec].Walls[i];

		// Point 1 - x
		SetDlgItemInt(hWndDialog, iCtrl++, pWall->p1.x, TRUE);
		// Point 1 - y
		SetDlgItemInt(hWndDialog, iCtrl++, pWall->p1.z , TRUE);
		// Point 2 - x
		SetDlgItemInt(hWndDialog, iCtrl++, pWall->p2.x, TRUE);
		// Point 2 - y
		SetDlgItemInt(hWndDialog, iCtrl++, pWall->p2.z, TRUE);
	}

	// Wall Y position
	SetDlgItemInt(hWndDialog, IDC_YGROUND, FrInfo[iRec].yGround, TRUE);

	int j;
	// Blank the rest
	for(j=i; j<nMaxWalls; j++)
	{
		WALL *pWall = &FrInfo[iRec].Walls[j];

		// Point 1 - x
		SetDlgItemInt(hWndDialog, iCtrl++, 0, TRUE);
		// Point 1 - y
		SetDlgItemInt(hWndDialog, iCtrl++, 0 , TRUE);
		// Point 2 - x
		SetDlgItemInt(hWndDialog, iCtrl++, 0, TRUE);
		// Point 2 - y
		SetDlgItemInt(hWndDialog, iCtrl++, 0, TRUE);
	}

	iCtrl = IDC_EX1;

	// Set the enemy information
	for(i=0; i<FrInfo[iRec].nNumEnemy; i++)
	{
		ENEMY *pEnemy = &FrInfo[iRec].Enemy[i];

		// x
		SetDlgItemInt(hWndDialog, iCtrl++, pEnemy->x, TRUE);
		// z
		SetDlgItemInt(hWndDialog, iCtrl++, pEnemy->z, TRUE);
		// y
		SetDlgItemInt(hWndDialog, iCtrl++, pEnemy->y, TRUE);
		// w
		SetDlgItemInt(hWndDialog, iCtrl++, pEnemy->w, TRUE);
		// h
		SetDlgItemInt(hWndDialog, iCtrl++, pEnemy->h, TRUE);
		// d
		SetDlgItemInt(hWndDialog, iCtrl++, pEnemy->d, TRUE);
	}

	// Blank the rest of the enemy

	for(j=i; j<nMaxEnemies; j++)
	{
		ENEMY *pEnemy = &FrInfo[iRec].Enemy[j];

		// x
		SetDlgItemInt(hWndDialog, iCtrl++, 0, TRUE);
		// z
		SetDlgItemInt(hWndDialog, iCtrl++, 0, TRUE);
		// y
		SetDlgItemInt(hWndDialog, iCtrl++, 0, TRUE);
		// w
		SetDlgItemInt(hWndDialog, iCtrl++, 0, TRUE);
		// h
		SetDlgItemInt(hWndDialog, iCtrl++, 0, TRUE);
		// d
		SetDlgItemInt(hWndDialog, iCtrl++, 0, TRUE);
	}

	// Set key frame indicators
	if (FrInfo[iRec].fType & FRTYPE_PLAYERZKEY)
		 SetDlgItemText(hWndDialog, IDC_PLAYERK, "K");
	else
		SetDlgItemText(hWndDialog, IDC_PLAYERK, " ");

	if (FrInfo[iRec].fType & FRTYPE_WALLKEY)
		SetDlgItemText(hWndDialog, IDC_WALLK, "K");
	else
		SetDlgItemText(hWndDialog, IDC_WALLK, " ");

	if (FrInfo[iRec].fType & FRTYPE_ENEMYKEY)
		SetDlgItemText(hWndDialog, IDC_ENEMYK, "K");
	else
		SetDlgItemText(hWndDialog, IDC_ENEMYK, " ");
}

/***********************************************************************/
void GetCurFrames(void)
/***********************************************************************/
{
	BOOL bTrans;
	long lTest = MCIGetPosition(lpVid->hDevice);

	// Get current frame
	lWCurrFrame = MCIGetPosition( lpVid->hDevice );
	lWStartFrame = GetDlgItemInt(hWndDialog, IDC_WDSTARTFRAME, &bTrans, FALSE);
	lWEndFrame = GetDlgItemInt(hWndDialog, IDC_WDENDFRAME, &bTrans, FALSE);
}

/***********************************************************************/
void CopyRec(void)
/***********************************************************************/
{
	char szShot[100];

	GetDlgItemText(hWndDialog, IDC_SHOTNAME, szShot, 100);
	if (szShot[0] == NULL)
		return;

	wsprintf(szCopyFile, "Data3D\\%s.3di", szShot);
}

/***********************************************************************/
void PasteRec(void)
/***********************************************************************/
{
	char szFile[200];
	char szShot[100];
	char szText[200];

	GetDlgItemText(hWndDialog, IDC_SHOTNAME, szShot, 100);
	if (szShot[0] == NULL)
		return;

	wsprintf(szFile, "Data3D\\%s.3di", szShot);

	wsprintf(szText, "Paste data from %s to %s", szCopyFile, szFile);
	int iRet = MessageBox( hWndDialog, szText, "Past Data", MB_YESNO|MB_ICONQUESTION);
	if (iRet == IDNO)
		return;

	// Read in data

	// Open file to copy
	HFILE hFile = _lopen(szCopyFile, 0);
	if (hFile == HFILE_ERROR)
		return;

	// Read header info
	long lTempStartFrame;
	long lTempEndFrame;
	int  nTempLastFrame;
	_lread(hFile, &lTempStartFrame, sizeof(lTempStartFrame));
	_lread(hFile, &lTempEndFrame, sizeof(lTempEndFrame));
	_lread(hFile, &nTempLastFrame, sizeof(nTempLastFrame));

	// Read all the frames
	_lread(hFile, FrInfo, sizeof(FRAMEINFO) * nLastFrame);

	// Close file
	_lclose(hFile);

	// Save to new file

	// Open file
	hFile = _lcreat(szFile, 0);
	if (hFile == HFILE_ERROR)
		return;

	// Save header info
	_lwrite(hFile, (LPCSTR)&lWStartFrame, sizeof(lWStartFrame));
	_lwrite(hFile, (LPCSTR)&lWEndFrame, sizeof(lWEndFrame));
	_lwrite(hFile, (LPCSTR)&nLastFrame, sizeof(nLastFrame));

	// Save all the frames
	_lwrite(hFile, (LPCSTR)FrInfo, sizeof(FRAMEINFO) * nLastFrame);

	// Close file
	_lclose(hFile);
}


/***********************************************************************/
void Save3DShotData(void)
/***********************************************************************/
{
	char szFile[200];
	char szShot[100];

	GetDlgItemText(hWndDialog, IDC_SHOTNAME, szShot, 100);
	if (szShot[0] == NULL)
		return;

	wsprintf(szFile, "Data3D\\%s.3di", szShot);

	GetCurFrames();

	// Open file
	HFILE hFile = _lcreat(szFile, 0);
	if (hFile == HFILE_ERROR)
		return;

	// Save header info
	_lwrite(hFile, (LPCSTR)&lWStartFrame, sizeof(lWStartFrame));
	_lwrite(hFile, (LPCSTR)&lWEndFrame, sizeof(lWEndFrame));
	_lwrite(hFile, (LPCSTR)&nLastFrame, sizeof(nLastFrame));

	// Save all the frames
	_lwrite(hFile, (LPCSTR)FrInfo, sizeof(FRAMEINFO) * nLastFrame);

	// Close file
	_lclose(hFile);
}

/***********************************************************************/
void Get3DShotData(void)
/***********************************************************************/
{
	char szFile[200];
	char szShot[100];

	GetDlgItemText(hWndDialog, IDC_SHOTNAME, szShot, 100);
	if (szShot[0] == NULL)
		return;

	wsprintf(szFile, "Data3D\\%s.3di", szShot);

	// Open file
	HFILE hFile = _lopen(szFile, 0);
	if (hFile == HFILE_ERROR)
		return;

	// Read header info
	_lread(hFile, &lWStartFrame, sizeof(lWStartFrame));
	_lread(hFile, &lWEndFrame, sizeof(lWEndFrame));
	_lread(hFile, &nLastFrame, sizeof(nLastFrame));

	// Read all the frames
	_lread(hFile, FrInfo, sizeof(FRAMEINFO) * nLastFrame);

	// Close file
	_lclose(hFile);

}

/***********************************************************************/
void Play(void)
/***********************************************************************/
{
	BOOL bReturn;
	DWORD dwMode = MCIGetMode( lpVid->hDevice );
	if ( dwMode == MCI_MODE_PAUSE )
		bReturn = MCIResume( lpVid->hDevice );
	else if ( dwMode == MCI_MODE_PLAY )
	{
		bReturn = MCIPause( lpVid->hDevice );
		GetCurFrames();
		SetCurrentFrame();
		//if (!bTestEvent && !bTestEvent2)
		//{
		//	// Save current frame for testing
		//	lWCurrFrame = MCIGetPosition( lpVid->hDevice );
		//}
	}
	else
	{
		// Play current shot
		BOOL bTranslated;
		long lFrom = GetDlgItemInt(hWndDialog, IDC_WDSTARTFRAME, &bTranslated, FALSE);
		long lTo = GetDlgItemInt(hWndDialog, IDC_WDENDFRAME, &bTranslated, FALSE);
		MCIPlay( lpVid->hDevice, hWndDialog, lFrom, lTo );
	}
}

/***********************************************************************/
void SetWalls(HWND hWnd)
/***********************************************************************/
{
	int iCtrl = IDC_W1_X1;
	int i;

	GetCurFrames();
	int iRec = 	(int)lWCurrFrame - lWStartFrame;
	if (iRec < 0 || iRec >= nMaxFrame)
	{
		Print("Record index too large for internal Frame Info storage.");
	}
	else
	{
		BOOL bTranslated;

		FrInfo[iRec].yGround = GetDlgItemInt(hWnd, IDC_YGROUND, &bTranslated, TRUE);

		for(i=0; i<nMaxWalls; i++)
		{
			WALL *pWall = &FrInfo[iRec].Walls[i];

			// Point 1 - x
			pWall->p1.x = GetDlgItemInt(hWnd, iCtrl++, &bTranslated, TRUE);
			// Point 1 - y
			pWall->p1.z = GetDlgItemInt(hWnd, iCtrl++, &bTranslated, TRUE);
			// Point 2 - x
			pWall->p2.x = GetDlgItemInt(hWnd, iCtrl++, &bTranslated, TRUE);
			// Point 2 - y
			pWall->p2.z = GetDlgItemInt(hWnd, iCtrl++, &bTranslated, TRUE);

			if (WallEmpty(pWall))
				break;

			// Precompute L2
			pWall->L2 = LineLengthSquared(pWall->p1.x, pWall->p1.z,
				pWall->p2.x, pWall->p2.z );

			FrInfo[iRec].fType |= FRTYPE_WALLKEY;
		}

		FrInfo[iRec].nNumWalls = i;
		//pWeapon->SetWorld(i, FrInfo[iRec].Walls, nYGround);
		//if (App.pGame)
		//	App.pGame->VWorld.SetLength(nLength);

		Refresh();
	}
}

/***********************************************************************/
BOOL EnemyEmpty(ENEMY *pEnemy)
/***********************************************************************/
{
	if (pEnemy->x == 0 &&
		pEnemy->z == 0 &&
		pEnemy->y == 0 &&
		pEnemy->w == 0 &&
		pEnemy->h == 0 &&
		pEnemy->d == 0)
		return TRUE;

	return FALSE;
}

/***********************************************************************/
void SetEnemy(HWND hWnd)
/***********************************************************************/
{
	int iCtrl = IDC_EX1;
	int i;

	GetCurFrames();
	int iRec = 	(int)lWCurrFrame - lWStartFrame;
	if (iRec < 0 || iRec >= nMaxFrame)
	{
		Print("Record index too large for internal Frame Info storage.");
	}
	else
	{
		BOOL bTranslated;

		for(i=0; i<nMaxEnemies; i++)
		{
			ENEMY *pEnemy = &FrInfo[iRec].Enemy[i];

			// x
			pEnemy->x = GetDlgItemInt(hWnd, iCtrl++, &bTranslated, TRUE);
			// z
			pEnemy->z = GetDlgItemInt(hWnd, iCtrl++, &bTranslated, TRUE);
			// y
			pEnemy->y = GetDlgItemInt(hWnd, iCtrl++, &bTranslated, TRUE);
			// w
			pEnemy->w = GetDlgItemInt(hWnd, iCtrl++, &bTranslated, TRUE);
			// h
			pEnemy->h = GetDlgItemInt(hWnd, iCtrl++, &bTranslated, TRUE);
			// d
			pEnemy->d = GetDlgItemInt(hWnd, iCtrl++, &bTranslated, TRUE);

			if (EnemyEmpty(pEnemy))
				break;

			FrInfo[iRec].fType |= FRTYPE_ENEMYKEY;
		}

		FrInfo[iRec].nNumEnemy = i;

		Refresh();
	}
}

/***********************************************************************/
void TweenEnemy(void)
/***********************************************************************/
{
	int iFirst = -1;
	int iLast = -1;
	int iNum = 0;
	int iDelta = 0;
	int i, j;

	BOOL bDone = FALSE;
	BOOL bFirstTime = TRUE;

	double dxDelta[nMaxEnemies] = {0.0,0.0,0.0};
	double dxAccum[nMaxEnemies] = {0.0,0.0,0.0};
	double dzDelta[nMaxEnemies] = {0.0,0.0,0.0};
	double dzAccum[nMaxEnemies] = {0.0,0.0,0.0};
	double dyDelta[nMaxEnemies] = {0.0,0.0,0.0};
	double dyAccum[nMaxEnemies] = {0.0,0.0,0.0};
	double dwDelta[nMaxEnemies] = {0.0,0.0,0.0};
	double dwAccum[nMaxEnemies] = {0.0,0.0,0.0};
	double dhDelta[nMaxEnemies] = {0.0,0.0,0.0};
	double dhAccum[nMaxEnemies] = {0.0,0.0,0.0};
	double ddDelta[nMaxEnemies] = {0.0,0.0,0.0};
	double ddAccum[nMaxEnemies] = {0.0,0.0,0.0};

	int nSaveNum = 0;

	nLastFrame = lWEndFrame - lWStartFrame + 1;
	if (nLastFrame >= nMaxFrame)
	{
		nLastFrame = nMaxFrame - 1;
		Print("Warning: Frames exceeded.");
	}

	while (!bDone)
	{
		// If first time through...
		if (bFirstTime)
		{
			bFirstTime = FALSE;

			// Find First
			for (i=iLast+1; i<nLastFrame; i++)
			{
				// Check if first enemy empty
				if (!EnemyEmpty(&FrInfo[i].Enemy[0]))
				{
					iFirst = i;
					nSaveNum = FrInfo[i].nNumEnemy;
					break;
				}

				// else fill with last value
				else
				{
					for(j=0; j<nMaxEnemies; j++)
					{
						FrInfo[i].Enemy[j].x = (int)dxAccum[j];
						FrInfo[i].Enemy[j].z = (int)dzAccum[j];
						FrInfo[i].Enemy[j].y = (int)dyAccum[j];
						FrInfo[i].Enemy[j].w = (int)dwAccum[j];
						FrInfo[i].Enemy[j].h = (int)dhAccum[j];
						FrInfo[i].Enemy[j].d = (int)ddAccum[j];
						FrInfo[i].nNumEnemy = nSaveNum;
					}

				Debug("finding first, copying last accum to %d\n", i);
				}
			}

		}
		else
		{
			iFirst = iLast;
			nSaveNum = FrInfo[iFirst].nNumEnemy;
		}

		// Find last
		for (i=iFirst+1; i<nLastFrame; i++)
		{
			if (!EnemyEmpty(&FrInfo[i].Enemy[0]))
			{
				iLast = i;
				break;
			}
		}

		// If that was the end of the frames then we are about done
		if (i == nLastFrame)
		{
			bDone = TRUE;

			// Replace all the remaing values with the last values
			// (dAccum)
			for(i=iFirst+1; i<nLastFrame; i++)
			{
				FrInfo[i].nNumEnemy = FrInfo[iFirst].nNumEnemy;
				for(j=0; j<nMaxEnemies; j++)
				{
					FrInfo[i].Enemy[j].x = FrInfo[iFirst].Enemy[j].x;
					FrInfo[i].Enemy[j].z = FrInfo[iFirst].Enemy[j].z;
					FrInfo[i].Enemy[j].y = FrInfo[iFirst].Enemy[j].y;
					FrInfo[i].Enemy[j].w = FrInfo[iFirst].Enemy[j].w;
					FrInfo[i].Enemy[j].h = FrInfo[iFirst].Enemy[j].h;
					FrInfo[i].Enemy[j].d = FrInfo[iFirst].Enemy[j].d;
				}

			Debug("last frame not found, copying last accum to %d\n", i);
			}
		}
		else
		{
			// How many frames we working with?
			iNum = iLast - iFirst;

			for(j=0; j<nMaxEnemies; j++)
			{
				dxDelta[j] = (double)(FrInfo[iLast].Enemy[j].x - FrInfo[iFirst].Enemy[j].x) / (double)iNum;
				dxAccum[j] = (double)FrInfo[iFirst].Enemy[j].x;
				dzDelta[j] = (double)(FrInfo[iLast].Enemy[j].z - FrInfo[iFirst].Enemy[j].z) / (double)iNum;
				dzAccum[j] = (double)FrInfo[iFirst].Enemy[j].z;
				dyDelta[j] = (double)(FrInfo[iLast].Enemy[j].y - FrInfo[iFirst].Enemy[j].y) / (double)iNum;
				dyAccum[j] = (double)FrInfo[iFirst].Enemy[j].y;
				dwDelta[j] = (double)(FrInfo[iLast].Enemy[j].w - FrInfo[iFirst].Enemy[j].w) / (double)iNum;
				dwAccum[j] = (double)FrInfo[iFirst].Enemy[j].w;
				dhDelta[j] = (double)(FrInfo[iLast].Enemy[j].h - FrInfo[iFirst].Enemy[j].h) / (double)iNum;
				dhAccum[j] = (double)FrInfo[iFirst].Enemy[j].h;
				ddDelta[j] = (double)(FrInfo[iLast].Enemy[j].d - FrInfo[iFirst].Enemy[j].d) / (double)iNum;
				ddAccum[j] = (double)FrInfo[iFirst].Enemy[j].d;
			}

			Debug("--- iFirst=%d, iLast=%d ---\n", iFirst,iLast);
			Debug("dxDelta=%f, dzDelta=%f, dyDelta=%f\n", dxDelta[0], dzDelta[0], dyDelta[0]);
			Debug("dwDelta=%f, dhDelta=%f, ddDelta=%f\n", dwDelta[0], dhDelta[0], ddDelta[0]);

			// Tween the values between the first and last frames
			for(i=iFirst+1; i<iLast; i++)
			{
				for(j=0; j<nMaxEnemies; j++)
				{
					dxAccum[j] += dxDelta[j];
					dzAccum[j] += dzDelta[j];
					dyAccum[j] += dyDelta[j];
					dwAccum[j] += dwDelta[j];
					dhAccum[j] += dhDelta[j];
					ddAccum[j] += ddDelta[j];

					FrInfo[i].Enemy[j].x = (int)dxAccum[j];
					FrInfo[i].Enemy[j].z = (int)dzAccum[j];
					FrInfo[i].Enemy[j].y = (int)dyAccum[j];
					FrInfo[i].Enemy[j].w = (int)dwAccum[j];
					FrInfo[i].Enemy[j].h = (int)dhAccum[j];
					FrInfo[i].Enemy[j].d = (int)ddAccum[j];
				}

			Debug("fr[%d] dxAccum=%f, dzAccum=%f, dyAccum=%f\n", i, dxAccum[0], dzAccum[0], dyAccum[0]);
			Debug("dwAccum=%f, dhAccum=%f, ddAccum=%f\n", dwAccum[0], dhAccum[0], ddAccum[0]);
			FrInfo[i].nNumEnemy = nSaveNum;
			}
		}
	}
}

/***********************************************************************/
void AllZs(void)
/***********************************************************************/
{
	// Adjust player z
	int i;
	int j;

	BOOL bTranslated;
	int iZDif = GetDlgItemInt(hWndDialog, IDC_DIFAMOUNT, &bTranslated, TRUE);

	for(i=0; i<nMaxFrame; i++)
	{
		if (FrInfo[i].nPlayerZ != EMPTY_POS)
			FrInfo[i].nPlayerZ += iZDif;

		for(j=0; j<nMaxWalls; j++)
		{
			if (!WallEmpty(&FrInfo[i].Walls[j]))
			{
				FrInfo[i].Walls[j].p1.z += iZDif;
				FrInfo[i].Walls[j].p2.z += iZDif;
			}
		}
		for(j=0; j<nMaxEnemies; j++)
		{
			if (!EnemyEmpty(&FrInfo[i].Enemy[j]))
			{
				FrInfo[i].Enemy[j].z += iZDif;
			}
		}
	}

	PostMessage(hWndDialog, WM_COMMAND, IDC_SETFRAMEINFO, 0);
}

/***********************************************************************/
void ChangeZ(int iZ)
/***********************************************************************/
{
	int i;
	int iRec;

	if (g_iCurCoords < 0)
		Print("Need to set a radio button for the set of coordinates to adjust.");

	iRec = (int)lWCurrFrame - lWStartFrame;

	// Enemies
	if (g_iCurCoords >= 0 && g_iCurCoords <= 2)
	{
		i = g_iCurCoords;

		int iCtrl = IDC_EZ1 + (i * 6);

		FrInfo[iRec].Enemy[i].z += iZ;
		SetDlgItemInt(hWndDialog, iCtrl, 
			FrInfo[iRec].Enemy[i].z, TRUE);
	}
	// Walls
	else if (g_iCurCoords >= 3 && g_iCurCoords <= 7)
	{
		int iCtrl;

		i = g_iCurCoords - 3;

		short nShiftState = GetKeyState(VK_SHIFT);
		short nCtrlState = GetKeyState(VK_CONTROL);

		nShiftState &= 0x8000;
		nCtrlState &= 0x8000;
		
		// If control key not pressed	
		if (!nCtrlState)
		{
			iCtrl = IDC_W1_Y1 + (i * 4);

			FrInfo[iRec].Walls[i].p1.z += iZ;

			SetDlgItemInt(hWndDialog, iCtrl, 
				FrInfo[iRec].Walls[i].p1.z, TRUE);
		}

		// If shift key not pressed
		if (!nShiftState)
		{
			iCtrl = IDC_W1_Y2 + (i * 4);

			FrInfo[iRec].Walls[i].p2.z += iZ;

			SetDlgItemInt(hWndDialog, iCtrl,
				FrInfo[iRec].Walls[i].p2.z, TRUE);
		}
	}

	Refresh();
}


/***********************************************************************/
void ChangeX(int iX)
/***********************************************************************/
{
	int i;
	int iRec;

	if (g_iCurCoords < 0)
		Print("Need to set a radio button for the set of coordinates to adjust.");

	iRec = (int)lWCurrFrame - lWStartFrame;

	// Enemies
	if (g_iCurCoords >= 0 && g_iCurCoords <= 2)
	{
		i = g_iCurCoords;

		int iCtrl = IDC_EX1 + (i * 6);

		FrInfo[iRec].Enemy[i].x += iX;
		SetDlgItemInt(hWndDialog, iCtrl, 
			FrInfo[iRec].Enemy[i].x, TRUE);
	}
	// Walls
	else if (g_iCurCoords >= 3 && g_iCurCoords <= 7)
	{
		int iCtrl;

		i = g_iCurCoords - 3;

		short nShiftState = GetKeyState(VK_SHIFT);
		short nCtrlState = GetKeyState(VK_CONTROL);

		nShiftState &= 0x8000;
		nCtrlState &= 0x8000;
		
		// If control key not pressed	
		if (!nCtrlState)
		{
			iCtrl = IDC_W1_X1 + (i * 4);

			FrInfo[iRec].Walls[i].p1.x += iX;

			SetDlgItemInt(hWndDialog, iCtrl, 
				FrInfo[iRec].Walls[i].p1.x, TRUE);
		}

		// If shift key not pressed
		if (!nShiftState)
		{
			iCtrl = IDC_W1_X2 + (i * 4);

			FrInfo[iRec].Walls[i].p2.x += iX;

			SetDlgItemInt(hWndDialog, iCtrl,
				FrInfo[iRec].Walls[i].p2.x, TRUE);
		}
	}

	Refresh();
}


