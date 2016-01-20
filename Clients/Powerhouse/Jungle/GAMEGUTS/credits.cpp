#include <windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include "credits.h"
#include "sprite.h"
#include "parser.h"

#define FACENAME "Arial"
#define PTSIZE	30
#define ITALIC TRUE
#define TEXT_COLOR RGB(0,0,0)

class CCreditsParser : public CParser
{
	public:
	CCreditsParser(LPSTR lpTableData)
		: CParser(lpTableData) {}

	protected:
	BOOL HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData);
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

	m_pAnimator->StopAll();
	RECT rect;
	SetRect( &rect, 40, 40, 600, 330 );
	m_pAnimator->SetClipRect(&rect);

	// read in the name information
	HGLOBAL hData;
	LPSTR lpTableData;
	if ( lpTableData = GetResourceData(m_nSceneNo, RT_RCDATA, &hData) )
	{
		CCreditsParser parser(lpTableData);
		if (m_nContributors = parser.GetNumEntries())
		{
			m_pContributors = new CContributor[m_nContributors];
			if (m_pContributors)
				parser.ParseTable((DWORD)m_pContributors);
		}
		if (hData)
		{
			UnlockResource( hData );
			FreeResource( hData );
		}
	}

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
	if ( !(pDib = TextToDib(m_pContributors[i].m_szName, FACENAME, PTSIZE,
					ITALIC, TEXT_COLOR, GetApp()->m_hPal)) )
		return(NO/*fStarted*/);

	POINT ptOrigin;
	ptOrigin.x = 0;
	ptOrigin.y = 0;
	LPSPRITE lpSprite = m_pAnimator->CreateSprite( &ptOrigin );
	lpSprite->AddCell( pDib, 0, 0 );
	lpSprite->SetSpeed( m_pContributors[i].m_iSpeed );
	lpSprite->SetNotifyProc(::OnSpriteNotify, (DWORD)this);
	lpSprite->Jump(m_pContributors[i].m_ptStart.x, m_pContributors[i].m_ptStart.y);
	lpSprite->Show(TRUE);
	lpSprite->AddCmdMove(m_pContributors[i].m_ptEnd.x, m_pContributors[i].m_ptEnd.y);
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
	m_szName[0] = '\0';
	m_ptStart.x = m_ptStart.y = 0;
	m_ptEnd.x = m_ptEnd.y = 0;
	m_dwDelayTime = 0L;
	m_iSpeed = 100;
}

//************************************************************************
BOOL CCreditsParser::HandleKey(LPSTR lpEntry, LPSTR lpKey, LPSTR lpValues, int nValues, int nIndex, DWORD dwUserData)
//************************************************************************
{
	LPCONTRIBUTOR pContributor = (LPCONTRIBUTOR)dwUserData;
	pContributor += nIndex;

	if (!lstrcmpi(lpKey, "start"))
		GetPoint(&pContributor->m_ptStart, lpEntry, lpValues, nValues);
	else
	if (!lstrcmpi(lpKey, "end"))
		GetPoint(&pContributor->m_ptEnd, lpEntry, lpValues, nValues);
	else
	if (!lstrcmpi(lpKey, "name"))
		GetString(pContributor->m_szName, lpEntry, lpValues, nValues);
	else
	if (!lstrcmpi(lpKey, "delay"))
	{
		STRING szTime;
		GetString(szTime, lpEntry, lpValues, nValues);
		pContributor->m_dwDelayTime = latol(szTime);
	}
	else
	if (!lstrcmpi(lpKey, "speed"))
	{
		STRING szTime;
		GetString(szTime, lpEntry, lpValues, nValues);
		pContributor->m_iSpeed = latoi(szTime);
	}
	else
		Print("'%ls'\n Unknown key '%s'", lpEntry, lpKey);
		
	return(TRUE);
}
