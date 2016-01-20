#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include "lgbook.h"
#include "sound.h"
#include "commonid.h"
#include "sprite.h"
#include "parser.h"
#include "belongs.h"
#include "credits.h"
#include "options.h"
#include "colorbok.h"
#include "soundbox.h"
#include "lgbres.h"

#define NEXT_PAGE			1
#define NEXT_PAGE_DELAY		2000	// 2 seconds

class CSceneTableParser : public CParser
{
public:
	CSceneTableParser(LPSTR lpTableData)
		: CParser(lpTableData) {}

protected:
	BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
};  

//************************************************************************
LOCAL LPSTR GetResourceData(int idResource, LPCSTR lpszType, HGLOBAL FAR *lphData)
//************************************************************************
{
	DWORD dwSize;
	LPSTR lpData;
	HINSTANCE hInstance;
	HRSRC hResource;

	dwSize = 0;
	*lphData = NULL;
	lpData = NULL;
	hInstance = GetApp()->GetInstance();
	if ( idResource && (hResource = FindResource( hInstance, MAKEINTRESOURCE(idResource), "gametbl" )) )
	{
		if ( (dwSize = SizeofResource( hInstance, hResource )) )
		{
			if ( (*lphData = LoadResource( hInstance, hResource )) )
			{
				if ( !(lpData = (LPSTR)LockResource( *lphData )) )
  				{
					FreeResource( *lphData );
					*lphData = NULL;
  				}
			}
		}
	}
	return(lpData);
}
	
//************************************************************************
CGBScene::CGBScene(int nNextSceneNo)
//************************************************************************
{
	m_nNextSceneNo = nNextSceneNo;
	m_idTimer = 0;
	m_bNoErase = YES;
}

//************************************************************************
PTOON CGBScene::GetToon()
//************************************************************************
{
	HWND hToon = FindClassDescendent(m_hWnd, "toon");
	if (hToon)
		return(::GetToon(hToon));
	else
		return(NULL);
}

/************************************************************************/
BOOL CGBScene::OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam)
/************************************************************************/
{
	GetApp()->SetKeyMapEntry(hWnd, VK_SPACE);
	GetApp()->SetKeyMapEntry(hWnd, VK_ESCAPE);
	m_idTimer = 0;

	// Set up some info. for the toon
	if (GetApp()->m_fReading && GetToon())
	{
		GetToon()->EnableClickables(FALSE);
		GetToon()->m_fReadOnly = TRUE;
	}

	BOOL fRet = CScene::OnInitDialog(hWnd, hWndFocus, lParam);
	return (fRet);
}

//************************************************************************
void CGBScene::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
 	if (m_idTimer)
 	{
 		KillTimer(hWnd, m_idTimer);
 		m_idTimer = 0;
 	}
	switch (id)
	{    
		case IDC_TOON:
		{
			PTOON pToon = GetToon();
			if (codeNotify == CToon::ClickedOnHotSpot)
				StopEverything(hWnd);
			else
			if (codeNotify == CToon::AutoDone) // Auto-Play Toon Done
			{
				if (GetApp()->m_fReading)
				{
					if (!pToon || !pToon->PlayConclusion())
					{
						SetTimer( hWnd, NEXT_PAGE, NEXT_PAGE_DELAY, NULL );
					}
				}
			}
			if (codeNotify == CToon::ConclusionDone) // Conclusion Toon done
				GetApp()->GotoNextScene(m_hWnd);
			break;
		}
		case IDC_BACK:
		{
			ITEMID id;
			if (GetApp()->m_bNewBookmark)
 				id = GetApp()->GetSceneID(IDC_BOOKMARK);
			else
				id = GetApp()->GetFirstSceneNo();
 
			GetApp()->GotoScene(hWnd, id);
			break;
		}
		case IDC_READ:
			GetApp()->m_fReading = (id == IDC_READ);
		case IDC_NEXT:
		{
			GetApp()->m_fPlaying = TRUE;
			StopEverything(hWnd, TRUE);
			PTOON pToon = GetToon();
			if (!pToon || !pToon->PlayConclusion())
				GetApp()->GotoNextScene(m_hWnd);
			break;
		}
		case IDC_SKIPNEXT:
		{
			GetApp()->m_fPlaying = TRUE;
			StopEverything(hWnd, TRUE);
			PTOON pToon = GetToon();
			if (!pToon || !pToon->PlayConclusion())
				GetApp()->GotoNextSkipScene(m_hWnd);
			break;
		}
		case IDC_PREVIOUS:
		{
			StopEverything(hWnd, TRUE);
			GetApp()->GotoPrevScene(m_hWnd);
			break;
		}
		case IDC_GOTOSCENE:
 		{
			StopEverything(hWnd, TRUE);
			GetApp()->GotoScene(hWnd, codeNotify);
			break;
		}

		case IDC_QUIT:
		{
			StopEverything(hWnd, TRUE);
			FORWARD_WM_CLOSE(GetApp()->GetMainWnd(), SendMessage);
			break;	    					
		}

		case IDC_ACTIVITIES:
		case IDC_OPTIONS:
		case IDC_OPTIONS2:
		case IDC_SOUNDBOX:
		{
	 		StopEverything(hWnd, TRUE);
			GetApp()->GotoSceneID(hWnd, id, m_nSceneNo);
			break;
		}
		case IDC_MAINMENU:
		case IDC_EXITSTORY:		
		{
	 		StopEverything(hWnd, TRUE);
			GetApp()->GotoSceneID(hWnd, id);
			break;
		}
		case IDC_ACTIVITY1:
		case IDC_ACTIVITY2:
		case IDC_ACTIVITY3:
		case IDC_ACTIVITY4:
		case IDC_ACTIVITY5:
		case IDC_ACTIVITY6:
		case IDC_ACTIVITY7:
		case IDC_ACTIVITY8:
		case IDC_ACTIVITY9:
		case IDC_ACTIVITY10:
		case IDC_MUSIC:
		case IDC_SINGALONG:
		case IDC_SOUNDS:
		case IDC_THEATRE1:
		case IDC_THEATRE2:
		case IDC_THEATRE3:
		{
			GetApp()->m_fPlaying = FALSE;
	 		StopEverything(hWnd, TRUE);
			GetApp()->GotoSceneID(hWnd, id, m_nNextSceneNo);
			break;
		}
		default:
		{
			CScene::OnCommand(hWnd, id, hControl, codeNotify);
			break;
		}
	}
}

//************************************************************************
void CGBScene::OnKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
//************************************************************************
{
	// ignore any auto-repeat keystrokes
	if (flags & 0x4000)
		return;

	switch (vk)
	{
		case VK_SPACE:
		{
			if ( m_nSceneNo == IDD_POWERSCREEN )
				GetApp()->GotoNextSkipScene(hWnd);
			else
			if ( m_nSceneNo != IDD_MAINMENU )
				OnCommand(hWnd, IDC_MAINMENU, NULL, 0);
			break;
		}
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
LPSTR CGBScene::GetPathName(LPSTR lpPathName, LPSTR lpFileName)
//************************************************************************
{
	GetToon()->GetContentDir(lpPathName);
	lstrcat(lpPathName, lpFileName);
	return(lpPathName);
}

//************************************************************************
BOOL CGBScene::GetDisplaySize(LPINT lpWidth, LPINT lpHeight)
//************************************************************************
{
	PDIB pDib = NULL;

	if (GetOffScreen())
		pDib = GetOffScreen()->GetWritableDIB();
	else
	if (GetToon())
		pDib = GetToon()->GetWinGDib();
		
	if (pDib)
	{
		*lpWidth = pDib->GetWidth();
		*lpHeight = abs(pDib->GetHeight());
		return(TRUE);
	}
	else
	{
		*lpWidth = 0;
		*lpHeight = 0;
		return(FALSE);
	}
}

//************************************************************************
LPSTR CGBScene::GetResourceData(int idResource, LPCSTR lpszType, HGLOBAL FAR *lphData, BOOL fCheckLanguage)
//************************************************************************
{
	int nLanguageOffset = 0;

	if (fCheckLanguage)
	{
		if (GetApp()->GetLanguage() == CLGBApp::Spanish)
			nLanguageOffset = IDD_SPANISHOFFSET;
		if (idResource && nLanguageOffset)
		{
			idResource += nLanguageOffset;
			if (FindResource( GetApp()->GetInstance(), MAKEINTRESOURCE(idResource), lpszType) == NULL)
				idResource -= nLanguageOffset;
		}
	}
	return(::GetResourceData(idResource, lpszType, lphData));
}
	
//************************************************************************
void CGBScene::OnTimer(HWND hWnd, UINT id)
//************************************************************************
{
	CScene::OnTimer(hWnd, id);
	if (GetToon())
		GetToon()->OnTimer(id);
	if (id == NEXT_PAGE)
	{
		StopEverything(hWnd, TRUE);
		KillTimer( hWnd, id );
		GetApp()->GotoNextScene(hWnd);
	}
}

//************************************************************************
BOOL CGBScene::PlaySound(LPCTSTR lpWaveFile, BOOL fHint)
//************************************************************************
{
	LPSOUND pSound = GetSound();
	if ( !pSound )
		return( FALSE );

	BOOL fRet;
	if ( !pSound->IsMixing() )
		fRet = pSound->StartFile((LPSTR)lpWaveFile, FALSE/*bLoop*/, -1, TRUE/*bWait*/);
	else
	{
		// this nonsense is here because in order to make buttons
		// in action strip play the click wave when they are clicked
		// on we need to synchronously play the sound so the click
		// happens while the button is down.  WaveMix does not play
		// sounds synchronously, so we deactivate WaveMix and use
		// sndPlaySound stuff.
		CSound sound;
		pSound->Activate(FALSE);
		fRet = sound.StartFile((LPSTR)lpWaveFile, FALSE/*bLoop*/, -1, TRUE/*bWait*/);
		pSound->Activate(TRUE);
	}

	return(fRet);
}

//************************************************************************
void CGBScene::StopEverything(HWND hWnd, BOOL fExiting)
//************************************************************************
{
	PTOON pToon = GetToon();
	if (pToon)
		pToon->StopPlaying(TRUE, fExiting);
}

//************************************************************************
void CGBScene::OnDestroy(HWND hWnd)
//************************************************************************
{
	if (m_idTimer)
		KillTimer(hWnd, m_idTimer);
	CScene::OnDestroy(hWnd);
}

//***********************************************************************
CSceneEntry::CSceneEntry()
//***********************************************************************
{
	m_nSceneNo = 0;
	m_iType = ST_NORMAL;
}

//***********************************************************************
CLGBApp::CLGBApp()
//***********************************************************************
{
	m_fReading = FALSE; 
	m_nSceneNo = -1;
	m_lpScene = NULL;
	m_nLastSceneNo = -1;
	m_hToonWnd = NULL;
	m_Language = English;
	m_nBookmark = 0;
	m_bNewBookmark = NO;
	m_nScenes = 0;
	m_pSceneTable = NULL;
	m_fPlaying = TRUE;
 }

//***********************************************************************
CLGBApp::~CLGBApp()
//***********************************************************************
{
	if (m_pSceneTable)
		delete [] m_pSceneTable;
}

//***********************************************************************
BOOL CLGBApp::InitInstance()
//***********************************************************************
{
	HGLOBAL hData;
	LPSTR lpTableData;

	// load the scene table
	lpTableData = GetResourceData(IDR_SCENETABLE, "gametbl", &hData);
	if (lpTableData)
	{
		CSceneTableParser parser(lpTableData);
		if (m_nScenes = parser.GetNumEntries())
		{
			m_pSceneTable = new CSceneEntry[m_nScenes];
			if (m_pSceneTable)
				parser.ParseTable((DWORD)m_pSceneTable);
		}
	}
	if (hData)
	{
		UnlockResource( hData );
		FreeResource( hData );
	}
	if (!m_pSceneTable)
		return(FALSE);
	
	m_Language = (Language)GetSettingInt("Language", English);
	m_nBookmark = GetSettingInt("Bookmark", 0);
	if (m_Language != English && m_Language != Spanish)
		m_Language = English;
	if (!CPHApp::InitInstance())
		return(FALSE);
	m_hAccel = LoadAccelerators(GetInstance(), MAKEINTRESOURCE(IDR_ACCEL)); 

	return TRUE;
}

//***********************************************************************
BOOL CLGBApp::CreateAdditionalWindows()
//***********************************************************************
{
    if (!(m_hToonWnd = CreateWindow("toon",
    				"",
					WS_CHILD,
    				0, 0, 640, 480,
    				GetMainWnd(),
    				(HMENU)IDC_TOON,
    				GetInstance(),
    				NULL)))
	{
		Print("Not enough memory");
		return(FALSE);
	}
	return(TRUE);
}

//***********************************************************************
int CLGBApp::ExitInstance()
//***********************************************************************
{
	WriteSettingInt("Bookmark", m_nBookmark);
	WriteSettingInt("Language", (int)m_Language);
	return(CPHApp::ExitInstance());
}

//***********************************************************************
BOOL CLGBApp::RegisterTitleClasses()
//***********************************************************************
{
    WNDCLASS WndClass;

	WndClass.hCursor		= LoadCursor( GetInstance(), MAKEINTRESOURCE(IDC_TITLE_CURSOR));
	if (!WndClass.hCursor)
	    WndClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
    WndClass.hIcon          = NULL;
    WndClass.lpszMenuName   = NULL;
    WndClass.lpszClassName  = (LPSTR)"toon";
    WndClass.hbrBackground  = (HBRUSH)GetStockObject( WHITE_BRUSH );
    WndClass.hInstance      = m_hInstance;
    WndClass.style          = CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc    = ToonControl; 
    WndClass.cbClsExtra     = 0;
    WndClass.cbWndExtra     = TOON_EXTRA;

    if (!RegisterClass( &WndClass ) )
        return FALSE;

    WndClass.hCursor        = NULL;
    WndClass.hIcon          = NULL;
    WndClass.lpszMenuName   = NULL;
    WndClass.lpszClassName  = (LPSTR)"toonholder";
    WndClass.hbrBackground  = NULL;
    WndClass.hInstance      = m_hInstance;
    WndClass.style          = CS_HREDRAW | CS_VREDRAW;
    WndClass.lpfnWndProc    = ToonHolderControl; 
    WndClass.cbClsExtra     = 0;
    WndClass.cbWndExtra     = 0;

    if (!RegisterClass( &WndClass ) )
        return FALSE;

    return(TRUE);
}

//***********************************************************************
BOOL CLGBApp::GotoCurrentScene()
//***********************************************************************
{
	LPGBSCENE lpScene = (LPGBSCENE)GetCurrentScene();
	if (lpScene)
		return(GotoScene(lpScene->GetWindow(), lpScene->GetSceneNo(), lpScene->GetNextSceneNo()));
	else
		return(FALSE);
}

//***********************************************************************
BOOL CLGBApp::GotoScene( HWND hWndPreviousScene, int iScene )
//***********************************************************************
{
	return(GotoScene(hWndPreviousScene, iScene, 0));
}

//***********************************************************************
LPSCENE CLGBApp::GetScene()
//***********************************************************************
{
	return (m_lpScene);
}

//***********************************************************************
int CLGBApp::GetSceneNo( HWND hDlg )
//***********************************************************************
{
	if (m_nSceneNo >= 0)
		return m_nSceneNo;
	return(DialogGetID(hDlg));
}

//************************************************************************
BOOL CLGBApp::WriteSettingString(LPCTSTR lpEntry, LPCTSTR lpValue, BOOL fUserSetting)
//************************************************************************
{
	FNAME szPath;
	STRING szIni, szSection;

	szIni[0] = '\0';
	LoadString( GetApp()->GetInstance(), IDS_INI, szIni, sizeof(szIni) );
	szSection[0] = '\0';
	LoadString( GetApp()->GetInstance(), IDS_SECTION, szSection, sizeof(szSection) );

	GetWindowsDirectory(szPath, sizeof(szPath));
	FixPath(szPath);

	lstrcat(szPath, szIni);
	return(WritePrivateProfileString(szSection, lpEntry, lpValue, szPath));
}

//************************************************************************
BOOL CLGBApp::WriteSettingInt(LPCTSTR lpEntry, int iValue, BOOL fUserSetting)
//************************************************************************
{
	FNAME szPath;
	STRING szValue;
	STRING szIni, szSection;

	szIni[0] = '\0';
	LoadString( GetApp()->GetInstance(), IDS_INI, szIni, sizeof(szIni) );
	szSection[0] = '\0';
	LoadString( GetApp()->GetInstance(), IDS_SECTION, szSection, sizeof(szSection) );

	wsprintf(szValue, "%d", iValue);
	GetWindowsDirectory(szPath, sizeof(szPath));
	FixPath(szPath);
	lstrcat(szPath, szIni);
	return(WritePrivateProfileString(szSection, lpEntry, szValue, szPath));
}

//************************************************************************
int CLGBApp::GetSettingString(LPCTSTR lpEntry, LPTSTR lpValue, LPTSTR lpDefault, BOOL fUserSetting)
//************************************************************************
{
	FNAME szPath;
	STRING szIni, szSection;

	szIni[0] = '\0';
	LoadString( GetApp()->GetInstance(), IDS_INI, szIni, sizeof(szIni) );
	szSection[0] = '\0';
	LoadString( GetApp()->GetInstance(), IDS_SECTION, szSection, sizeof(szSection) );

	GetWindowsDirectory(szPath, sizeof(szPath));
	FixPath(szPath);
	lstrcat(szPath, szIni);
	return(GetPrivateProfileString(szSection, lpEntry, lpDefault, lpValue, MAX_STR_LEN, szPath));
}

//************************************************************************
UINT CLGBApp::GetSettingInt(LPCTSTR lpEntry, int iDefault, BOOL fUserSetting)
//************************************************************************
{
	FNAME szPath;
	STRING szIni, szSection;

	szIni[0] = '\0';
	LoadString( GetApp()->GetInstance(), IDS_INI, szIni, sizeof(szIni) );
	szSection[0] = '\0';
	LoadString( GetApp()->GetInstance(), IDS_SECTION, szSection, sizeof(szSection) );

	GetWindowsDirectory(szPath, sizeof(szPath));
	FixPath(szPath);
	lstrcat(szPath, szIni);
	return(GetPrivateProfileInt(szSection, lpEntry, iDefault, szPath));
}

/***********************************************************************/
void CLGBApp::OnPaletteChanged(HWND hWnd, HWND hWndCausedBy)
/***********************************************************************/
{ // Some app caused (or is causing) the palette to change
	PTOON pToon = NULL;
	LPSCENE lpScene = GetScene();
	if (lpScene)
	    pToon = lpScene->GetToon();
    if (pToon)
	{
        pToon->OnPaletteChanged(hWnd, hWndCausedBy);
	}
    else           
	{
		CPHApp::OnPaletteChanged(hWnd, hWndCausedBy);
	}
}

/***********************************************************************/
BOOL CLGBApp::OnQueryNewPalette(HWND hWnd)
/***********************************************************************/
{ // Receiving focus; chance to select and realize our palette
    // first see if there is a toon control somewhere
	PTOON pToon = NULL;
	LPSCENE lpScene = GetScene();
	if (lpScene)
	    pToon = lpScene->GetToon();
    if (pToon)
	{
        return(pToon->OnQueryNewPalette(hWnd));
	}
    else
	{
		return(CPHApp::OnQueryNewPalette(hWnd));
	}
}

//************************************************************************
#ifdef WIN32
void CLGBApp::OnActivateApp(HWND hWnd, BOOL fActivate, ULONG htaskActDeact)
#else
void CLGBApp::OnActivateApp(HWND hWnd, BOOL fActivate, HTASK htaskActDeact)
#endif
//************************************************************************
{
	CPHApp::OnActivateApp(hWnd, fActivate, htaskActDeact);
	// we need to restart the scene to make sure all is cool
	if (fActivate)
		GotoCurrentScene();
}

/***********************************************************************/
BOOL CLGBApp::GotoFirstScene()
/***********************************************************************/
{
	return(GotoScene(NULL, m_pSceneTable[0].m_nSceneNo));
}

/***********************************************************************/
BOOL CLGBApp::GotoNextScene(HWND hWnd)
/***********************************************************************/
{
	int iScene = GetNextScene(m_nSceneNo);
	if (iScene)
		return(GotoScene(hWnd, iScene));
	else
		return(FALSE);
}

/***********************************************************************/
BOOL CLGBApp::GotoNextSkipScene(HWND hWnd)
/***********************************************************************/
{
	int iScene = GetNextScene(GetNextScene(m_nSceneNo));
	if (iScene)
		return(GotoScene(hWnd, iScene));
	else
		return(FALSE);
}

/***********************************************************************/
BOOL CLGBApp::IsNextScene(int iScene, int iPrevScene)
/***********************************************************************/
{
	return(GetNextScene(iScene) == iPrevScene);
}

/***********************************************************************/
int CLGBApp::GetNextScene(int iScene)
/***********************************************************************/
{
	int iType;
	if (!m_fReading)
		iType = ST_NOPLAY;
	else
		iType = ST_NOREADTOME;

	for (int i = 0; i < (m_nScenes-1); ++i)
	{
		if (m_pSceneTable[i].m_nSceneNo == iScene)
		{
			++i;						
			while ((i < (m_nScenes)) && (m_pSceneTable[i].m_iType & iType))
				++i;
			if (i < m_nScenes)
				return(m_pSceneTable[i].m_nSceneNo);
			break;
		}
	}
	return(0);
}

/***********************************************************************/
BOOL CLGBApp::GotoPrevScene(HWND hWnd)
/***********************************************************************/
{
	int iScene = GetPrevScene(m_nSceneNo);
	if (iScene)
		return(GotoScene(hWnd, iScene));
	else
		return(FALSE);
}

/***********************************************************************/
BOOL CLGBApp::IsPrevScene(int iScene, int iPrevScene)
/***********************************************************************/
{
	return(GetPrevScene(iScene) == iPrevScene);
}

/***********************************************************************/
int CLGBApp::GetPrevScene(int iScene)
/***********************************************************************/
{
	int iType;
	if (!m_fReading)
		iType = ST_NOPREVIOUS | ST_NOPLAY;
	else
		iType = ST_NOPREVIOUS | ST_NOREADTOME;

	for (int i = 1; i < m_nScenes; ++i)
	{
		if (m_pSceneTable[i].m_nSceneNo == iScene)
		{
			--i;
			while ((i >= 0) && (m_pSceneTable[i].m_iType & iType))
				--i;
			if (i >= 0)
				return(m_pSceneTable[i].m_nSceneNo);
			break;
		}
	}
	return(0);
}

#ifdef UNUSED
/************************************************************************/
BOOL CLGBApp::FindContent( LPSTR lpFileName, LPSTR lpExpFileName )
/************************************************************************/
{
	if ( !lpFileName )
		return( FALSE );

	while ( TRUE )
	{
		STRING szString;

		// 1.) Try the file name as passed in
		if ( FileExistsExpand( lpFileName, lpExpFileName ) )
			return( TRUE );

		// 2.) Prompt to insert the CDROM disk...
		szString[0] = '\0';
		LoadString( GetInstance(), IDS_CDNAME, szString, sizeof(szString) );
		UINT uRet = PrintOKCancel( "Please insert '%s' CD-ROM.", (LPSTR)szString );
		if ( uRet == IDCANCEL )
			break;
		Delay( 5000L ); // wait in case the CD is not up to speed
	}
	//#ifdef _DEBUG
	Print("Cannot find file '%s'", lpFileName);
	//#endif

	// Post the close, so that we don't cause crashes
	// trying to execute code after windows have been
	// destroyed and classes have been destructed
	PostMessage( GetMainWnd(), WM_CLOSE, 0, 0L );

	return( FALSE );
}
#endif

//***********************************************************************
BOOL CLGBApp::GotoScene( HWND hWndPreviousScene, int iScene, int iNextScene )
//***********************************************************************
{    
	HTOON hToon = NULL;

    if ( !iScene )
        return( NO );

	// reset the key map for every scene
	ResetKeyMap();

	// close down the current scene
	if ( hWndPreviousScene )
		SendMessage( hWndPreviousScene, WM_CLOSE, 0, 0L );
          
	// if next scene not specified, set next scene
	if (!iNextScene)
		iNextScene = iScene + 1;

	// create the new scene
	m_lpScene = CreateScene(iScene, iNextScene);
    if (!m_lpScene)
		return(FALSE);

	// if this is the main menu, disable reading mode
	if (iScene == IDD_MAINMENU)
		m_fReading = FALSE;

	// save last scene# and set new scene#
	m_nLastSceneNo = m_nSceneNo;
    m_nSceneNo = iScene;

	// set bookmark if in valid range
	if (iScene >= GetFirstSceneNo() && iScene <= GetFinalSceneNo())
	{
		m_nBookmark = iScene;
		m_bNewBookmark = YES;
	}

	if (!m_lpScene->Create(YES, GetInstance(), GetMainWnd(), iScene))
		return(FALSE);
    return( TRUE );
}

//***********************************************************************
LPSCENE CLGBApp::CreateScene(int iScene, int iNextScene)
//***********************************************************************
{    
	LPSCENE lpScene;

	if (iScene == IDD_BELONGS)
	    lpScene = new CBelongsScene(iNextScene);
	else
	if (iScene == IDD_CREDITS)
	    lpScene = new CCreditsScene(IDD_OPTIONS);
	else
	if (iScene == IDD_OPTIONS ||
		iScene == IDD_OPTIONS ||
		iScene == IDD_DEMO ||
		iScene == IDD_DEMO1 ||
		iScene == IDD_DEMO2 ||
		iScene == IDD_DEMO3 ||
		iScene == IDD_DEMO4 ||
		iScene == IDD_DEMO5 ||
		iScene == IDD_DEMO6 ||
		iScene == IDD_DEMO7 ||
		iScene == IDD_LANGUAGE)
		lpScene = new COptionsScene(iNextScene);
	else
	if (iScene == IDD_COLORBOOK)
		lpScene = new CColorbookScene(iNextScene);
	else               
	if (iScene == IDD_SOUNDBOX ||
		iScene == IDD_MUSIC ||
		iScene == IDD_SINGALONG ||
		iScene == IDD_SOUNDS ||
		iScene == IDD_THEATRE1 ||
		iScene == IDD_THEATRE2 ||
		iScene == IDD_THEATRE3)
		lpScene = new CSoundBoxScene(iNextScene);
	else     
	    lpScene = new CGBScene(iNextScene);
	return(lpScene);
}

//***********************************************************************
BOOL CLGBApp::GotoSceneID( HWND hWndPreviousScene, int id, int iNextScene )
//***********************************************************************
{
	id = GetSceneID(id);
	if (id && id != m_nSceneNo)
		return(GotoScene(hWndPreviousScene, id, iNextScene));
	else
		return(FALSE);
}

//***********************************************************************
int CLGBApp::GetSceneID( int id )
//***********************************************************************
{
	if (id == IDC_BOOKMARK)
	{
		id = m_nBookmark;
		if (!id)
			id = GetFirstSceneNo();
	}
	else
	if (id == IDC_MAINMENU)
		id = IDD_MAINMENU;
	else
	if (id == IDC_EXITSTORY)
		id = IDD_EXITSTORY;
	else
	if (id == IDC_LANGUAGE)
		id = IDD_LANGUAGE;
	else
	if (id == IDC_CREDITS)
		id = IDD_CREDITS;
	else
	if (id == IDC_DEMO)
		id = IDD_DEMO;
	else
	if (id == IDC_DEMO1)
		id = IDD_DEMO1;
	else
	if (id == IDC_DEMO2)
		id = IDD_DEMO2;
	else
	if (id == IDC_DEMO3)
		id = IDD_DEMO3;
	else
	if (id == IDC_DEMO4)
		id = IDD_DEMO4;
	else
	if (id == IDC_DEMO5)
		id = IDD_DEMO5;
	else
	if (id == IDC_DEMO6)
		id = IDD_DEMO6;
	else
	if (id == IDC_DEMO7)
		id = IDD_DEMO7;
	else
	if (id == IDC_OPTIONS)
		id = IDD_OPTIONS;
	else
	if (id == IDC_OPTIONS2)
		id = IDD_OPTIONS2;
	else
	if (id == IDC_ACTIVITIES)
		id = IDD_ACTIVITY;
	else
	if (id == IDC_ACTIVITY5)
		id = IDD_COLORBOOK;
	else
	if (id == IDC_SOUNDBOX)
		id = IDD_SOUNDBOX;
	else
	if (id == IDC_MUSIC)
		id = IDD_MUSIC;
	else
	if (id == IDC_SINGALONG)
		id = IDD_SINGALONG;
	else
	if (id == IDC_THEATRE1)
		id = IDD_THEATRE1;
	else
	if (id == IDC_THEATRE2)
		id = IDD_THEATRE2;
	else
	if (id == IDC_THEATRE3)
		id = IDD_THEATRE3;
	else
	if (id == IDC_SOUNDS)
		id = IDD_SOUNDS;
	else
	if (id >= IDC_SCENE1 && id <= GetFinalSceneID())
		id = id - IDC_SCENE1 + GetFirstSceneNo();
	else
		id = 0;
	return(id);
}

//************************************************************************
BOOL CSceneTableParser::HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	LPSCENEENTRY pEntry = (LPSCENEENTRY)dwUserData;
	pEntry += nIndex;
	if (!lstrcmpi(lpKey, "scene"))
		GetInt(&pEntry->m_nSceneNo, lpEntry, lpValues, nValues);
	else
	if (!lstrcmpi(lpKey, "type"))
		GetInt(&pEntry->m_iType, lpEntry, lpValues, nValues);
	else
		Print("'%ls'\n Unknown key '%ls'", lpEntry, lpKey);
	return(TRUE);
}

