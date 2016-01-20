#include <windows.h>
#include "proto.h"
#include "puzzle.h"
#include "commonid.h"
#include "control.h"
#include "puzzleid.h"
#include "lgbookid.h"

/************************************************************************/
CPuzzleScene::CPuzzleScene(int nNextSceneNo) : CGBScene(nNextSceneNo)
/************************************************************************/
{
	m_bNoErase = NO;
	m_iSkill = 4;
	m_bHintMode = FALSE;
}

/************************************************************************/
BOOL CPuzzleScene::OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	// call base class on init first
	CScene::OnInitDialog(hWnd, hWndFocus, lParam);
	SetDlgItemInt(hWnd, IDC_SKILL, m_iSkill, FALSE);
	CheckDlgButton(hWnd, IDC_HINTMODE, m_bHintMode);
	if (GetDlgItem(hWnd, IDC_PUZZLE))
	{
		PPUZZLE lpPuzzle = GetPuzzle(GetDlgItem(hWnd, IDC_PUZZLE));
		if (lpPuzzle)
		{
			lpPuzzle->Init(m_iSkill, m_iSkill);
		 	lpPuzzle->SetHintMode(m_bHintMode);
		}
	}

   	return( TRUE );
}

//************************************************************************
void CPuzzleScene::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	BOOL Bool;
	int iSkill;

	switch (id)
	{
		case IDC_NEXTPUZZLE:
		{
			id = IDC_PUZZLE;
			if (GetDlgItem(hWnd, IDC_PUZZLE))
			{
				PPUZZLE lpPuzzle = GetPuzzle(GetDlgItem(hWnd, IDC_PUZZLE));
				if (lpPuzzle)
					id = lpPuzzle->NextPuzzle();
			}
			if (GetDlgItem(hWnd, IDC_PUZZLEVIEW))
			{
				SetWindowWord(GetDlgItem(hWnd, IDC_PUZZLEVIEW), GWW_ICONID, id-IDC_PUZZLE1+IDC_PUZZLEVIEW1);
				InvalidateRect(GetDlgItem(hWnd, IDC_PUZZLEVIEW), NULL, TRUE);
			}
			break;
		}

		case IDC_HINTMODE:
		{
			m_bHintMode = !m_bHintMode;
			CheckDlgButton(hWnd, id, m_bHintMode);
			if (GetDlgItem(hWnd, IDC_PUZZLE))
			{
				PPUZZLE lpPuzzle = GetPuzzle(GetDlgItem(hWnd, IDC_PUZZLE));
				if (lpPuzzle)
					lpPuzzle->SetHintMode(m_bHintMode);
			}
			break;
		}
		case IDC_SKILL:
		{
			if (GetFocus() != GetDlgItem(hWnd, id))
				break;
			if (codeNotify != EN_CHANGE)
				break;
			iSkill = GetDlgItemInt(hWnd, id, &Bool, NO);
			if (iSkill <= 0)
				break;
			m_iSkill = iSkill;

			if (GetDlgItem(hWnd, IDC_PUZZLE))
			{
				PPUZZLE lpPuzzle = GetPuzzle(GetDlgItem(hWnd, IDC_PUZZLE));
				if (lpPuzzle)
					lpPuzzle->Init(m_iSkill, m_iSkill);
			}
			break;
		}

		case IDC_MAINMENU:
		{
			break;
		}
		case IDC_BACK:
		{
			GetApp()->GotoScene(hWnd, m_nNextSceneNo);
			break;
		}
		default:
		break;
	}
}

