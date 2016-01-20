#ifndef _CREDITS_H_
#define _CREDITS_H_

#include "proto.h"

class FAR CContributor
{	  
public:
	CContributor();
	~CContributor(){};

public:
	FNAME	m_szName;
	POINT	m_ptStart;
	POINT	m_ptEnd;
	DWORD	m_dwDelayTime;
	int		m_iSpeed;
	int		m_iType;
};

typedef CContributor FAR *LPCONTRIBUTOR;

class FAR CCreditsScene : public CScene
{
public:
	// constructors/destructor
	CCreditsScene(int nNextSceneNo);
	~CCreditsScene();
	void OnSpriteNotify( LPSPRITE lpSprite, SPRITE_NOTIFY Notify );
	LPCONTRIBUTOR GetContributor(int nIndex) { return (m_pContributors+nIndex); }

private:
	// dialog procedure overridables
	BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
	LPSTR GetResourceData(int idResource, LPCSTR lpszType, HGLOBAL FAR *lphData);
	virtual void OnTimer(HWND hWnd, UINT id);
	BOOL StartNext();

public:
	STRING		m_szFaceName[2];
	int			m_ptSize[2];
	BOOL		m_fItalic[2];
	COLORREF 	m_Color[2];
	COLORREF	m_BackgrndColor;
	BOOL		m_fCenter;
	RECT		m_rGameArea;
	FILE_SPEC	m_szSoundTrack;

private:
	int			m_nNextSceneNo;
	int			m_nContributors;
	int			m_nNextContributor;
	LPCONTRIBUTOR m_pContributors;
	DWORD		m_dwLastTime;
};

typedef CCreditsScene FAR * LPCREDITSSCENE;
							
#endif // _CREDITS_H_
