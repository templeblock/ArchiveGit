#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include "options.h"
#include "sound.h"
#include "commonid.h"
#include "sprite.h"
#include "parser.h"
#include "transit.h"

//************************************************************************
COptionsScene::COptionsScene(int nNextSceneNo) : CGBScene(nNextSceneNo)
//************************************************************************
{
	m_nSelectedSceneID = IDC_SCENE1;
}

//************************************************************************
BOOL COptionsScene::OnInitDialog( HWND hWnd, HWND hWndControl, LPARAM lParam )
//************************************************************************
{
	return(CGBScene::OnInitDialog(hWnd, hWndControl, lParam));
}

//************************************************************************
void COptionsScene::ToonInitDone()
//************************************************************************
{
}

//************************************************************************
void COptionsScene::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	switch (id)
	{
		case IDC_LANGUAGE1:
		case IDC_LANGUAGE2:
		case IDC_LANGUAGE3:
		case IDC_LANGUAGE4:
		case IDC_LANGUAGE5:
		{
			LPLGBAPP pApp = (LPLGBAPP)GetApp();
			pApp->SetLanguage((CLGBApp::Language)(id-IDC_LANGUAGE1));
			pApp->GotoScene(hWnd, IDD_OPTIONS/*pApp->GetLastSceneNo()*/, m_nNextSceneNo);
			break;
		}
		case IDC_DEMO:
		case IDC_DEMO1:
		case IDC_DEMO2:
		case IDC_DEMO3:
		case IDC_DEMO4:
		case IDC_DEMO5:
		case IDC_DEMO6:
		case IDC_DEMO7:
		case IDC_BOOKMARK:
		case IDC_LANGUAGE:
		case IDC_CREDITS:
		{
			GetApp()->GotoSceneID(hWnd, id, m_nNextSceneNo);
			break;
		}
		case IDC_SCENEUP:
		case IDC_SCENEDOWN:
		{
			int iSceneID = m_nSelectedSceneID;
			if (id == IDC_SCENEUP)
				iSceneID += 1;
			else
				iSceneID -= 1;
			if (iSceneID < IDC_SCENE1)
				iSceneID = IDC_SCENE1;
			LPLGBAPP pApp = (LPLGBAPP)GetApp();
			if (iSceneID > pApp->GetFinalSceneID())
				iSceneID = pApp->GetFinalSceneID();
			if (iSceneID != m_nSelectedSceneID)
			{
				GetToon()->ModifyHotSpot(m_nSelectedSceneID, FALSE, TRUE);
				GetToon()->ModifyHotSpot(m_nSelectedSceneID-IDC_SCENE1+IDC_SELECTSCENE1, FALSE, TRUE);
				m_nSelectedSceneID = iSceneID;
				GetToon()->ModifyHotSpot(m_nSelectedSceneID, TRUE, TRUE);
				GetToon()->ModifyHotSpot(m_nSelectedSceneID-IDC_SCENE1+IDC_SELECTSCENE1, TRUE, TRUE);
			}

			break;
		}
		default:
		{
			if (id >= IDC_SELECTSCENE1 && id <= IDC_SELECTSCENE35)
			{
				GetApp()->m_fPlaying = TRUE;
				GetApp()->GotoSceneID (hWnd, m_nSelectedSceneID);
			}
			else
				CGBScene::OnCommand(hWnd, id, hControl, codeNotify);
			break;
		}
	}
}

