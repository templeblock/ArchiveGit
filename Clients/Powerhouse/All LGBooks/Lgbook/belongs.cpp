#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include <ctype.h>
#include "belongs.h"
#include "sound.h"
#include "commonid.h"
#include "sprite.h"
#include "parser.h"
#include "transit.h"
#include "belondef.h"

class CBelongsParser : public CParser
{
public:
	CBelongsParser(LPSTR lpTableData)
		: CParser(lpTableData) {}

protected:
	BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
};  

//************************************************************************
CBelongsScene::CBelongsScene(int nNextSceneNo) : CGBScene(nNextSceneNo)
//************************************************************************
{
	m_nNames = 0;
	m_pNames = NULL;
	m_hSound = NULL;
	m_hFont  = NULL;
}

//************************************************************************
CBelongsScene::~CBelongsScene()
//************************************************************************
{
	if ( m_pNames )
		delete [] m_pNames;
	m_pNames = NULL;

	if (m_hFont)
		DeleteObject (m_hFont);
}

//************************************************************************
BOOL CBelongsScene::OnInitDialog( HWND hWnd, HWND hWndControl, LPARAM lParam )
//************************************************************************
{
	HGLOBAL hData;
	LPSTR lpTableData;

	// we don't call CGBScene::OnInitDialog, because it remaps
	// VK_SPACE.  It's strange if you type a space in the edit
	// box and it causes you to go to the main menu.
	CScene::OnInitDialog(hWnd, hWndControl, lParam);
	GetApp()->SetKeyMapEntry(hWnd, VK_ESCAPE);
	GetApp()->SetKeyMapEntry(hWnd, VK_RETURN, WM_COMMAND, IDC_PLAYBELONGS, 0L, YES/*fOnDown*/);
#ifdef _DEBUG
	GetApp()->SetKeyMapEntry(hWnd, VK_SPACE, WM_COMMAND, VK_SPACE, 0L, YES/*fOnDown*/);
#endif

	// hide the buttons until the scene comes up
	ShowDlgItem(hWnd, IDC_EDIT, FALSE);

	// Set a font for the edit control based on a font with a known size, fixes the
	// problem where a PC has some weird large font for the system font (SMS).
	m_hFont = (HFONT)GetStockObject (ANSI_VAR_FONT);
	LOGFONT lf;
	GetObject (m_hFont, sizeof(LOGFONT), &lf);
	lf.lfHeight *= 2;
	lf.lfWeight = FW_BOLD;
	m_hFont = CreateFontIndirect (&lf);
	SendDlgItemMessage (hWnd, IDC_EDIT, WM_SETFONT, (WPARAM)m_hFont, 0L);
	
	// read in the name information
	lpTableData = GetResourceData(m_nSceneNo, "gametbl", &hData);
	if (lpTableData)
	{
		CBelongsParser parser(lpTableData);
		if (m_nNames = parser.GetNumEntries())
		{
			m_pNames = new CBelongsName[m_nNames];
			if (m_pNames)
				parser.ParseTable((DWORD)m_pNames);
		}
	}
	if (hData)
	{
		UnlockResource( hData );
		FreeResource( hData );
	}

	return( TRUE );
}

//************************************************************************
void CBelongsScene::OnDestroy(HWND hWnd)
//************************************************************************
{
	STRING szString;
	GetDlgItemText(hWnd, IDC_EDIT, szString, sizeof(szString));
	StripLeadingSpaces(szString);
	GetApp()->WriteSettingString("Name", szString, FALSE);
	CGBScene::OnDestroy(hWnd);
	if (m_hSound)
	{
		MCIStop(m_hSound,YES);
		MCIClose(m_hSound);
		m_hSound = NULL;
	}
}

//************************************************************************
void CBelongsScene::OnKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
//************************************************************************
{
	switch (vk)
	{
		case VK_ESCAPE:
		{
			// just eat the escape so it does not cause
			// a WM_CLOSE for the dialog
			break;
		}
		default:
			break;
	}
}

//************************************************************************
void CBelongsScene::OnAppActivate(BOOL fActivate)
//************************************************************************
{
	CGBScene::OnAppActivate(fActivate);
	if (fActivate)
		SetFocus(GetDlgItem(m_hWnd, IDC_EDIT));
}

//************************************************************************
void CBelongsScene::ToonInitDone()
//************************************************************************
{
	STRING szName;

	ShowDlgItem(m_hWnd, IDC_EDIT, TRUE);
	UpdateWindow(GetDlgItem(m_hWnd, IDC_EDIT));
	SetFocus(GetDlgItem(m_hWnd, IDC_EDIT));
	//GetApp()->GetSettingString("Name", szName, "", FALSE);
	szName[0] = '\0';
	if (lstrlen(szName))
	{
		SetDlgItemText(m_hWnd, IDC_EDIT, szName);
		Edit_SetSel(GetDlgItem(m_hWnd, IDC_EDIT), 0, -1);
		Edit_SetSel(GetDlgItem(m_hWnd, IDC_EDIT), -1, 0);
		UpdateWindow(GetDlgItem(m_hWnd, IDC_EDIT));
		PlayBelongsTo(NO);
		FORWARD_WM_COMMAND(m_hWnd, IDC_NEXT, m_hWnd, 0, PostMessage);
	}
	else
	{
		FNAME szFileName;

		GetPathName(szFileName, ENTER_NAME_WAVE);
		if (m_hSound)
		{
			MCIStop(m_hSound,YES);
			MCIClose(m_hSound);
			m_hSound = NULL;
		}
		m_hSound = MCIOpen(GetApp()->m_hDeviceWAV, szFileName, NULL); 
		if (m_hSound)
		{
			if (!MCIPlay(m_hSound, m_hWnd))
			{
				MCIClose(m_hSound);
				m_hSound = NULL;
			}
		}
	}
}

//************************************************************************
void CBelongsScene::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	switch (id)
	{
		case IDC_EDIT:
		{
			if (codeNotify == EN_CHANGE)
			{
			}
			break;
		}

#ifdef _DEBUG
		case VK_SPACE:
		{
			PlayBelongsTo(YES);
			SetDlgItemText(hWnd, IDC_EDIT, "");
			break;
		}
#endif

		case IDC_PLAYBELONGS:
		{
			EnableWindow(GetDlgItem(hWnd, IDC_EDIT), FALSE);
			PlayBelongsTo(NO);
			FORWARD_WM_COMMAND(m_hWnd, IDC_NEXT, m_hWnd, 0, PostMessage);
			break;
		}

		case IDC_NEXT:
		{
			if (m_hSound)
			{
				MCIStop(m_hSound,YES);
				MCIClose(m_hSound);
				m_hSound = NULL;
			}
			ShowDlgItem(hWnd, IDC_EDIT, FALSE);
			UpdateWindow(hWnd);
		}

		default:
		{
			CGBScene::OnCommand(hWnd, id, hControl, codeNotify);
			break;
		}
	}
}

//************************************************************************
void CBelongsScene::OnTimer(HWND hWnd, UINT id)
//************************************************************************
{
	CGBScene::OnTimer(hWnd, id);
	if (id == EDIT_TIMER)
	{
		KillTimer( hWnd, id );
		PlayBelongsTo(NO);
		FORWARD_WM_COMMAND(m_hWnd, IDC_NEXT, m_hWnd, 0, PostMessage);
	}
}

//************************************************************************
void CBelongsScene::PlayBelongsTo(BOOL fNameOnly)
//************************************************************************
{
	STRING szName;
	FNAME szNameWave;
	FNAME szFileName;
	BOOL fGotName = FALSE;

	GetDlgItemText(m_hWnd, IDC_EDIT, szName, sizeof(szName));
	StripLeadingSpaces(szName);
	if (!lstrlen(szName))
		return;

	// See if the name is in the table (for names longer than 8)
	STRING szTheName;
	int len = lstrlen(szName);
	for (int n = 0; n <= len; ++n)
	{
		if (isspace(szName[n]))
			szTheName[n] = '-';
		else
			szTheName[n] = szName[n];
	}

	for (int i = 0; i < m_nNames; ++i)
	{
		if (!lstrcmpi(m_pNames[i].m_szName, szTheName))
		{ // found it, now play the wav
			GetPathName(szNameWave, m_pNames[i].m_szFileName);
			fGotName = FileExistsExpand(szNameWave, NULL);
			break;
		}
	}

	// See if file for this name exists
	// in normal content directory
	if (!fGotName)
	{
		GetPathName(szNameWave, szName);
		lstrcat(szNameWave, ".wav");
		fGotName = FileExistsExpand(szNameWave, NULL);
	}
			
	// Try the windows directory for a custom name
	if (!fGotName)
	{
		GetWindowsDirectory(szNameWave, sizeof(szNameWave));
		FixPath(szNameWave);
		lstrcat(szNameWave, szName);
		lstrcat(szNameWave, ".wav");
		fGotName = FileExistsExpand(szNameWave, NULL);
	}

	if (fNameOnly && !fGotName)
		return;

	if (m_hSound)
	{
		MCIStop(m_hSound,YES);
		MCIClose(m_hSound);
		m_hSound = NULL;
	}

	if (!fNameOnly)
	{
		GetPathName(szFileName, BELONGS_TO_WAVE);
		if ( m_hSound = MCIOpen(GetApp()->m_hDeviceWAV, szFileName, NULL) )
		{
			MCIPlay(m_hSound, NULL);
			MCIClose(m_hSound);
			m_hSound = NULL;
		}
	}

	if (fGotName)
	{	// If we have a name play it
		if ( m_hSound = MCIOpen(GetApp()->m_hDeviceWAV, szNameWave, NULL) )
		{
			MCIPlay(m_hSound, NULL);
			MCIClose(m_hSound);
			m_hSound = NULL;
		}
	}
	else
	{	// Otherwise, play it letter by letter
		STRING szName;
		GetDlgItemText(m_hWnd, IDC_EDIT, szName, sizeof(szName));
		StripLeadingSpaces(szName);

		STRING szLetter;
		FNAME szFileName;
		BOOL fExists = FALSE;
		int i = 0;
		char c;
		while ( c = szName[i++] )
		{
			szLetter[0] = c;
			szLetter[1] = '\0';
			lstrcat(szLetter, ".wav");
			GetPathName(szFileName, szLetter);
			if ( !FileExistsExpand(szFileName, NULL) )
				continue;
			if ( m_hSound = MCIOpen(GetApp()->m_hDeviceWAV, szFileName, NULL) )
			{
				MCIPlay(m_hSound, NULL);
				MCIClose(m_hSound);
				m_hSound = NULL;
			}
		}
	}
}

//************************************************************************
BOOL CBelongsScene::OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam )
//************************************************************************
{
	switch (msg)
	{
		HANDLE_DLGMSG(hDlg, MM_MCINOTIFY, OnMCINotify);
		default:
			return(FALSE);
	}
}

//************************************************************************
UINT CBelongsScene::OnMCINotify(HWND hWnd, UINT message, HMCI hDeviceID)
//************************************************************************
{

	if ( message != MCI_NOTIFY_SUCCESSFUL	&&
		 message != MCI_NOTIFY_ABORTED		&&
		 message != MCI_NOTIFY_SUPERSEDED	&&
		 message != MCI_NOTIFY_FAILURE )
			return FALSE;

	MCIClose(m_hSound);
	m_hSound = NULL;
	return TRUE;
}		

//************************************************************************
void CBelongsScene::StripLeadingSpaces(LPSTR lpString)
//************************************************************************
{
	LPSTR lp1 = lpString;
	LPSTR lp2 = lpString;

	// find first character in string that is not a space
	while (isspace(*lp1))
		++lp1;
	// copy from first non-space character to end of string
	while (*lp1)
		*lp2++ = *lp1++;
	*lp2 = '\0';
	// if any non-space characters in string
	if (lstrlen(lpString))
	{
		// find last character in string that is not a space
		lp1 = &lpString[lstrlen(lpString)-1];
		while (isspace(*lp1))
			--lp1;
		// null terminate string one after last character that is not a space
		++lp1;
		*lp1 = '\0';
	}
}

//************************************************************************
CBelongsName::CBelongsName()
//************************************************************************
{
	m_szName[0] = '\0';
	m_szFileName[0] = '\0';
}

//************************************************************************
CBelongsName::~CBelongsName()
//************************************************************************
{
}

//************************************************************************
BOOL CBelongsParser::HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	LPSTR lpValue;

	LPBELONGSNAME pName = (LPBELONGSNAME)dwUserData;
	pName += nIndex;

	if (nValues > 1)
	{
		Print("Too many filenames specified");
		return(TRUE);
	}
	if (lstrlen(lpKey) >= MAX_NAME_LEN)
	{
		Print("Name too long");
		return(TRUE);
	}
	lpValue = GetNextValue(&lpValues);
	if (lstrlen(lpValue) >= MAX_FILE_LEN)
	{
		Print("Filename too long");
		return(TRUE);
	}
	lstrcpy(pName->m_szName, lpKey);
	lstrcpy(pName->m_szFileName, lpValue);

	return(TRUE);
}

