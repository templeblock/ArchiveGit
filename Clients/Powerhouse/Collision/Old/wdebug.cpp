#include "proto.h"
#include "wdebug.h"
#include "wdebug.p"

// global video world
extern VideoWorld VWorld;

static HWND hWndDialog = NULL;
static CWeapon *pWeapon = NULL;
static HWND hSceneWindow = NULL;

BOOL WDebugOnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
void WDebugOnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
BOOL WINPROC EXPORT WDebugDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
BOOL WDebugOnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );

/************************************************************************/
void WeaponDebugStart(HWND hWnd, CWeapon *_pWeapon)
/************************************************************************/
{
	pWeapon = _pWeapon;
	hSceneWindow = hWnd;
	hWndDialog = DialogStart( YES, GetWindowInstance(hWnd), hWnd, IDD_WDEBUG, WDebugDlgProc);
	DialogShow( IDD_WDEBUG);
}

/************************************************************************/
void WeaponDebugEnd(void)
/************************************************************************/
{
	if (hWndDialog)
		DialogEnd( hWndDialog, IDOK);
}


/************************************************************************/
BOOL WDebugOnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	SendDlgItemMessage(hWnd, IDC_SHOWGRID, BM_SETCHECK, 1, 0);
	return TRUE;
}

/************************************************************************/
void WDebugOnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
/************************************************************************/
{
	BOOL bTranslated;

	pWeapon = GetCurrentWeapon();

	if (id == IDOK)
	{
		int nStartX = GetDlgItemInt(hWnd, IDC_STARTPOSX, &bTranslated, TRUE);
		int nStartZ = GetDlgItemInt(hWnd, IDC_STARTPOSZ, &bTranslated, TRUE);
		int nStartY = GetDlgItemInt(hWnd, IDC_STARTPOSY, &bTranslated, TRUE);
		int nEndX = GetDlgItemInt(hWnd, IDC_ENDPOSX, &bTranslated, TRUE);
		int nEndZ = GetDlgItemInt(hWnd, IDC_ENDPOSZ, &bTranslated, TRUE);
		int nFrameDist = GetDlgItemInt(hWnd, IDC_FRAMEDIST, &bTranslated, TRUE);
		int nWidth = GetDlgItemInt(hWnd, IDC_WIDTH, &bTranslated, FALSE);
		int nHeight = GetDlgItemInt(hWnd, IDC_HEIGHT, &bTranslated, FALSE);
		int nDepth = GetDlgItemInt(hWnd, IDC_DEPTH, &bTranslated, FALSE);
		pWeapon->SetEnemy(0, nFrameDist, nStartX, nStartY, nStartZ, nEndX, nEndZ, nWidth, nHeight, nDepth);
		VWorld.SetEnemyTest(TRUE);
	}
	else if (id == IDC_SHOWGRID)
	{
		long lCheck = SendDlgItemMessage(hWnd, IDC_SHOWGRID, BM_GETCHECK, 0, 0);
		if (lCheck == 0)
			pWeapon->SetShowGrid(FALSE);
		else
			pWeapon->SetShowGrid(TRUE);

		VWorld.SetEnemyTest(FALSE);
	}
	else if (id == IDC_SETWORLD)
	{
		int nLength = GetDlgItemInt(hWnd, IDC_WLENGTH, &bTranslated, FALSE);
		int nWidth = GetDlgItemInt(hWnd, IDC_WWIDTH, &bTranslated, FALSE);
		int nYGround = GetDlgItemInt(hWnd, IDC_YGROUND, &bTranslated, TRUE);

		pWeapon->SetWorld(nLength, nWidth, nYGround);
		VWorld.SetLength(nLength);
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
		long lTestWorld = GetDlgItemInt(hWnd, IDC_TESTWORLD, &bTranslated, FALSE);
		VWorld.SetTestWorld(lTestWorld);
	}
}

/***********************************************************************/
BOOL WDebugOnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{			  
	return (FALSE);
}

/***********************************************************************/
BOOL WINPROC EXPORT WDebugDlgProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
/***********************************************************************/
{
	switch (msg)
	{
		HANDLE_DLGMSG(hDlg, WM_INITDIALOG, WDebugOnInitDialog);  
		HANDLE_DLGMSG(hDlg, WM_COMMAND, WDebugOnCommand);

		default:
			return(WDebugOnMessage(hDlg, msg, wParam, lParam));
		break;
	}
	return(FALSE);
}

