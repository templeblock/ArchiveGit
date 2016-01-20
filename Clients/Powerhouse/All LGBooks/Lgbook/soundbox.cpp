#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include "soundbox.h"
#include "sound.h"
#include "commonid.h"
#include "sprite.h"
#include "parser.h"
#include "transit.h"

int CSoundBoxScene::m_nSoundBoxScene = 0;

//************************************************************************
CSoundBoxScene::CSoundBoxScene(int nNextSceneNo) : CGBScene(nNextSceneNo)
//************************************************************************
{
}

//************************************************************************
BOOL CSoundBoxScene::OnInitDialog( HWND hWnd, HWND hWndControl, LPARAM lParam )
//************************************************************************
{
	if (m_nSceneNo == IDD_SOUNDBOX)
		m_nSoundBoxScene = m_nSceneNo;
	return(CGBScene::OnInitDialog(hWnd, hWndControl, lParam));
}

//************************************************************************
void CSoundBoxScene::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	switch (id)
	{            
		case IDC_BACK:
		{   
			StopEverything(hWnd);
			if (m_nSceneNo == IDD_SOUNDBOX)
				CGBScene::OnCommand(hWnd, id, hControl, codeNotify);
			else
				GetApp()->GotoScene(hWnd, m_nSoundBoxScene, m_nNextSceneNo);
			break;
		}
		default:
		{
			CGBScene::OnCommand(hWnd, id, hControl, codeNotify);
			break;
		}
	}
}

