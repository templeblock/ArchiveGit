#ifndef _COLORBOK_H_
#define _COLORBOK_H_

#include "lgbook.h"

class FAR CPage
{     
public:
	CPage();
	~CPage();

public:
	FILE_SPEC 	m_szThumb;
	FILE_SPEC	m_szPage;
	STRING		m_szName;
	int			m_nPanel;
	LPSPRITE	m_lpSprite;
	POINT		m_ptPos;
};

typedef CPage FAR *LPPAGE;

class FAR CColorbookScene : public CGBScene
{
public:
	// constructors/destructor
	CColorbookScene(int nNextSceneNo);
	~CColorbookScene();

	LPPAGE GetPage(int nPage) { return (m_pPages + nPage); }
	void ToonInitDone();
	BOOL IsOnHotSpot(LPPOINT lpPoint);

private:
	// dialog procedure overridables
	BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
	BOOL OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags);
	void OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags);

	void ShowCurrentPanel();
	void HighlightPage(int iPage, BOOL fHighlight);

public:
	FILE_SPEC	m_szHighlight;

private:
	int			m_nPages;
	LPPAGE	 	m_pPages;
	int			m_nPanels;
	int			m_nCurrentPanel;
	int			m_iHighlighted;
	LPSPRITE	m_lpHighlightSprite;
};

typedef CColorbookScene FAR * LPCOLORBOOKSCENE;
                            
#endif // _COLORBOK_H_