#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include "credits.h"
#include "sprite.h"
#include "parser.h"

#define FACENAME "Arial"
#define PTSIZE	15
#define ITALIC TRUE
#define TEXT_COLOR RGB(255,255,0)

class CCreditsParser : public CParser
{
	public:
	CCreditsParser(LPSTR lpTableData)
		: CParser(lpTableData) {}

	protected:
	BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
	BOOL GetColor(COLORREF FAR *lpColor, LPSTR lpEntry, LPSTR lpValues, int nValues);
};	

//************************************************************************
CCreditsScene::CCreditsScene(int nNextSceneNo)
//************************************************************************
{
	m_nNextSceneNo = nNextSceneNo; 
	m_nContributors = 0;
	m_nNextContributor = 0;
	m_pContributors = NULL;
	m_dwLastTime = 0;
	m_fCenter = TRUE;
 	SetRect( &m_rGameArea, 47, 30, 592, 433 );
	for (int i = 0; i < 2; ++i)
	{
		lstrcpy(m_szFaceName[i], FACENAME);
		m_ptSize[i] = PTSIZE;
		m_fItalic[i] = ITALIC;
		m_Color[i] = TEXT_COLOR;
	}
	m_BackgrndColor = RGB(255, 255, 255);
	m_szSoundTrack[0] = '\0';
}

//************************************************************************
CCreditsScene::~CCreditsScene()
//************************************************************************
{
	if ( m_pContributors )
		delete [] m_pContributors;
	m_pContributors = NULL;
}

//************************************************************************
LOCAL void CALLBACK OnSpriteNotify( LPSPRITE lpSprite, SPRITE_NOTIFY Notify, DWORD dwNotifyData )
//************************************************************************
{
	LPCREDITSSCENE lpScene = (LPCREDITSSCENE)dwNotifyData;
	lpScene->OnSpriteNotify(lpSprite, Notify);	
}

//************************************************************************
void CCreditsScene::OnSpriteNotify( LPSPRITE lpSprite, SPRITE_NOTIFY Notify )
//************************************************************************
{
	if (Notify != SN_MOVEDONE)
		return;
	lpSprite->Kill();
	if ( !m_pContributors )
		return;
	int i = m_nNextContributor;
	DWORD dwDelay = m_pContributors[i].m_dwDelayTime;
	if ( (dwDelay == 1) || (!dwDelay && !m_pAnimator->GetNumSprites()) )
		StartNext();
}

//************************************************************************
BOOL CCreditsScene::OnInitDialog( HWND hWnd, HWND hWndControl, LPARAM lParam )
//************************************************************************
{
	CScene::OnInitDialog(hWnd, hWndControl, lParam);
	if ( !m_pAnimator )
		return( TRUE );

	// read in the name information
	HGLOBAL hData;
	LPSTR lpTableData;
	if ( lpTableData = GetResourceData(m_nSceneNo, RT_RCDATA, &hData) )
	{
		CCreditsParser parser(lpTableData);
		if (m_nContributors = parser.GetNumEntries())
		{
			--m_nContributors;
			m_pContributors = new CContributor[m_nContributors];
			if (m_pContributors)
				parser.ParseTable((DWORD)this);
		}
	}
	if (hData)
	{
		UnlockResource( hData );
		FreeResource( hData );
	}

	m_pAnimator->StopAll();
	m_pAnimator->SetClipRect(&m_rGameArea);

//	if (GetSound() && lstrlen(m_szSoundTrack))
//	{
//		FNAME szFileName;
//
//		GetPathName(szFileName, m_szSoundTrack);
//		GetSound()->StartFile(szFileName, TRUE, -1); 
//	}

	StartNext();
	m_pAnimator->StartAll();
	return( TRUE );
}

//************************************************************************
void CCreditsScene::OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify)
//************************************************************************
{
	GetApp()->GotoScene(hWnd, m_nNextSceneNo);
}

//************************************************************************
void CCreditsScene::OnTimer(HWND hWnd, UINT id)
//************************************************************************
{
	CScene::OnTimer(hWnd, id);
	if ( !m_pContributors )
		return;
	int i = m_nNextContributor;
	if ( m_pContributors[i].m_dwDelayTime <= 1 )
		return;
	DWORD dwTime = timeGetTime();
	if ((dwTime - m_dwLastTime) > m_pContributors[i].m_dwDelayTime)
		StartNext();
}

//************************************************************************
BOOL CCreditsScene::StartNext()
//************************************************************************
{
	if ( !m_pContributors )
		return( NO );

	m_dwLastTime = timeGetTime();
	int i = m_nNextContributor;
	if ( ++m_nNextContributor >= m_nContributors )
		m_nNextContributor = 0;

	PDIB pDib;
	int iType = m_pContributors[i].m_iType;
	if ( !(pDib = TextToDib(m_pContributors[i].m_szName, m_szFaceName[iType], m_ptSize[iType],
					m_fItalic[iType], m_Color[iType], GetPalette(), m_BackgrndColor)) )
		return(NO/*fStarted*/);

	POINT ptOrigin;
	ptOrigin.x = 0;
	ptOrigin.y = 0;
	LPSPRITE lpSprite = m_pAnimator->CreateSprite( &ptOrigin );
	lpSprite->AddCell( pDib, 0, 0 );
	lpSprite->SetSpeed( m_pContributors[i].m_iSpeed );
	lpSprite->SetNotifyProc(::OnSpriteNotify, (DWORD)this);
	int x;
	if (m_fCenter)
	{
	   int iWidth, iHeight;
	   lpSprite->GetMaxSize(&iWidth, &iHeight);
	   x = (RectWidth(&m_rGameArea) - iWidth) / 2;
	   x += m_rGameArea.left;
	}
	else
		x = m_pContributors[i].m_ptStart.x;
	lpSprite->Jump(x, m_pContributors[i].m_ptStart.y);
	lpSprite->Show(TRUE);
	if (!m_fCenter)
		x = m_pContributors[i].m_ptEnd.x;
	lpSprite->AddCmdMove(x, m_pContributors[i].m_ptEnd.y);
	return(YES/*fStarted*/);
}

//************************************************************************
LPSTR CCreditsScene::GetResourceData(int idResource, LPCSTR lpszType, HGLOBAL FAR *lphData)
//************************************************************************
{
	HINSTANCE hInstance = GetApp()->GetInstance();
	if ( !idResource )
		return( NULL );
	HRSRC hResource;
	if ( !(hResource = FindResource( hInstance, MAKEINTRESOURCE(idResource), lpszType )) )
		return( NULL );
	DWORD dwSize;
	if ( !(dwSize = SizeofResource( hInstance, hResource )) )
		return( NULL );
	if ( !(*lphData = LoadResource( hInstance, hResource )) )
		return( NULL );
	LPSTR lpData;
	if ( !(lpData = (LPSTR)LockResource( *lphData )) )
		FreeResource( *lphData );
	return(lpData);
}
	
//************************************************************************
CContributor::CContributor()
//************************************************************************
{
	m_szName[0]   = '\0';
	m_ptStart.x = m_ptStart.y = 0;
	m_ptEnd.x = m_ptEnd.y = 300;
	m_dwDelayTime = 500L;
	m_iSpeed = 50;
	m_iType = 0;
}

/***********************************************************************/
BOOL CCreditsParser::GetColor(COLORREF FAR *lpColor, LPSTR lpEntry, LPSTR lpValues, int nValues)
/***********************************************************************/
{
	long l;
	LPSTR lpValue;
	BOOL bError;
	BYTE rgb[3];

	if (nValues != 3)
	{
		Print("'%s'\n Invalid color '%s'", lpEntry, lpValues);
		return(FALSE);
	}

	for (int i = 0; i < 3; ++i)
	{
		lpValue = GetNextValue(&lpValues);
		l = latol(lpValue, &bError);
		if ( bError || l < 0 )
		{
			Print("'%s'\n Bad number at '%s'", lpEntry, lpValue);
			return(FALSE);
		}
		rgb[i] = (BYTE)l;
	}
	*lpColor = RGB(rgb[0], rgb[1], rgb[2]);

	return(TRUE);
}

//************************************************************************
BOOL CCreditsParser::HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	static POINT ptStartDefault = {0,0}, ptEndDefault = {0,480};
	static DWORD dwDelayTimeDefault= 500L;
	static int iSpeedDefault = 50, iTypeDefault = 0;

	LPCREDITSSCENE lpScene = (LPCREDITSSCENE)dwUserData;
	if (nIndex == 0)
	{
		if (!lstrcmpi(lpKey, "face1"))
			GetString(lpScene->m_szFaceName[0], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "face2"))
			GetString(lpScene->m_szFaceName[1], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "size1"))
			GetInt(&lpScene->m_ptSize[0], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "size2"))
			GetInt(&lpScene->m_ptSize[1], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "italic1"))
			GetInt(&lpScene->m_fItalic[0], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "italic2"))
			GetInt(&lpScene->m_fItalic[1], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "color1"))
			GetColor(&lpScene->m_Color[0], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "color2"))
			GetColor(&lpScene->m_Color[1], lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "bgcolor"))
			GetColor(&lpScene->m_BackgrndColor, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "center"))
			GetInt(&lpScene->m_fCenter, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "cliprect") || !lstrcmpi(lpKey, "gamearea"))
			GetRect(&lpScene->m_rGameArea, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "soundtrack"))
			GetFilename(lpScene->m_szSoundTrack, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "start"))
			GetPoint(&ptStartDefault, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "end"))
			GetPoint(&ptEndDefault, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "delay"))
		{
			STRING szTime;
			GetString(szTime, lpEntry, lpValues, nValues);
			dwDelayTimeDefault = latol(szTime);
		}
		else
		if (!lstrcmpi(lpKey, "speed"))
			GetInt(&iSpeedDefault, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "type"))
		{
			GetInt(&iTypeDefault, lpEntry, lpValues, nValues);
			--iTypeDefault; // make zero relative
			if (iTypeDefault <= 0)
				iTypeDefault = 0;
			else
				iTypeDefault = 1;
		}
		else
			Print("'%ls'\n Unknown key '%s'", lpEntry, lpKey);
	}
	else
	{
		LPCONTRIBUTOR pContributor = lpScene->GetContributor(nIndex-1);;

		if (!lstrcmpi(lpKey, "start"))
			GetPoint(&pContributor->m_ptStart, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "end"))
			GetPoint(&pContributor->m_ptEnd, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "name"))
		{
			GetString(pContributor->m_szName, lpEntry, lpValues, nValues);
			pContributor->m_ptStart     = ptStartDefault;
			pContributor->m_ptEnd       = ptEndDefault;
			pContributor->m_dwDelayTime = dwDelayTimeDefault;
			pContributor->m_iSpeed      = iSpeedDefault;
			pContributor->m_iType       = iTypeDefault;
		}
		else
		if (!lstrcmpi(lpKey, "delay"))
		{
			STRING szTime;
			GetString(szTime, lpEntry, lpValues, nValues);
			pContributor->m_dwDelayTime = latol(szTime);
		}
		else
		if (!lstrcmpi(lpKey, "speed"))
				GetInt(&pContributor->m_iSpeed, lpEntry, lpValues, nValues);
		else
		if (!lstrcmpi(lpKey, "type"))
		{
			GetInt(&pContributor->m_iType, lpEntry, lpValues, nValues);
			--pContributor->m_iType; // make zero relative
			if (pContributor->m_iType <= 0)
				pContributor->m_iType = 0;
			else
				pContributor->m_iType = 1;
		}
		else
			Print("'%ls'\n Unknown key '%s'", lpEntry, lpKey);
	}
		
	return(TRUE);
}
