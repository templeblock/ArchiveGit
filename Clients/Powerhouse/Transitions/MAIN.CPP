#include <windows.h>
#define MAIN
#include <mmsystem.h>
#include "proto.h"
#include "sound.h"
#include "transit.h"
#include "main.h"
#include "mainid.h"
#include "commonid.h"

static int idTheTransition;
static int nStepSize = 0;
static int nTicks = 20;


LPMAINSCENE lpScene = NULL;

/************************************************************************/
BOOL CMainScene::OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{ // Parcel out init's to each scene that needs it
	CScene::OnInitDialog(hWnd, hWndFocus, lParam);
 	switch (m_nSceneNo)
	{
		case 1:
		{
			LPOFFSCREEN lpOffScreen = GetOffScreen();
			if (lpOffScreen)
				lpOffScreen->SetTransition();
			SetDlgItemInt(hWnd, IDC_EDIT1, nTicks, FALSE);
			SetDlgItemInt(hWnd, IDC_EDIT2, nStepSize, FALSE);
			break;
		}

		case 3:
		{
			LPOFFSCREEN lpOffScreen = GetOffScreen();
			if (lpOffScreen)
				lpOffScreen->SetTransition(idTheTransition, nTicks, nStepSize);
			break;
		}
				
		default:
			break;
	}
   	return( FALSE );
}

//************************************************************************
void CMainScene::OnCommand(HWND hWnd, int id, HWND loWord, UINT hiWord)
//************************************************************************
{ // Parcel out commands to each scene
	BOOL Bool;

	switch (m_nSceneNo)
	{
		case 1:
			if (id >= IDC_BUTTON1 && id <= IDC_BUTTON69)
			{
				nTicks = GetDlgItemInt(hWnd, IDC_EDIT1, &Bool, FALSE);
				nStepSize = GetDlgItemInt(hWnd, IDC_EDIT2, &Bool, FALSE);
				idTheTransition = id - IDC_BUTTON1 + 1;
				App.GotoScene(hWnd, 2);
				break;
			}
		default:
			break;
	}
}

//***********************************************************************
void CMainScene::OnClose(HWND hWnd)
//***********************************************************************
{ // Parcel out the close to each scene
	CScene::OnClose( hWnd );
	switch (m_nSceneNo)
	{
		default:
			break;
	}
}

//***********************************************************************
BOOL CMainApp::InitInstance()
//***********************************************************************
{
	if (!CPHApp::InitInstance())
		return(FALSE);
	
	m_hAccel = LoadAccelerators(GetInstance(), MAKEINTRESOURCE(IDR_ACCEL)); 

    return TRUE;
}

//***********************************************************************
BOOL CMainApp::GotoScene( HWND hWndPreviousScene, int iScene )
//***********************************************************************
{ // Standard fare for most titles
	if ( !iScene )
		{
		PostMessage( GetMainWnd(), WM_CLOSE, 0, 0L );
		return( TRUE );
		}

    if ( hWndPreviousScene )
        SendMessage( hWndPreviousScene, WM_CLOSE, 0, 0L );
          
    lpScene = new CMainScene();
    if (!lpScene)
		return(FALSE);
	if (!lpScene->Create(YES, App.GetInstance(), App.GetMainWnd(), iScene))
		return(FALSE);
    return( TRUE );
}

//***********************************************************************
void CMainApp::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//***********************************************************************
{
	switch (id)
	{
		case IDC_MAINMENU:
			if (lpScene && lpScene->GetSceneNo() != 1)
				App.GotoScene(lpScene->GetWindow(), 1);
			break;
		default:
			break;
	}
}
