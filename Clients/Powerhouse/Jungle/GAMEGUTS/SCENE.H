#ifndef _SCENE_H_
#define _SCENE_H_

#include "offscren.h"
#include "sprite.h"
#include "toon.h"
#include "sound.h"

class FAR CScene
{
public:
	// constructors/destructor
	CScene();
	virtual ~CScene();

	// implementation
	BOOL Create(int iModeless, HINSTANCE hInstance, HWND hWndParent, int iScene);
	LPOFFSCREEN GetOffScreen() { return m_lpOffScreen; }
	virtual PTOON GetToon() { return NULL; }
	virtual BOOL IsOnHotSpot(LPPOINT lpPoint) { return FALSE; }
	HWND GetWindow() { return m_hWnd; }
	int GetSceneNo() { return m_nSceneNo; }
	LPSOUND GetSound() { return m_pSound; }
	HPALETTE GetPalette();
	LPANIMATOR GetAnimator() { return m_pAnimator; }

	virtual void OnAppActivate(BOOL fActivate);

	// window procedure overridables - to override other messages you must override
	// WindowProc
	virtual LONG WindowProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	virtual BOOL OnNCCreate(HWND hwnd, CREATESTRUCT FAR* lpCreateStruct);
	virtual void OnNCDestroy(HWND hwnd);

	// dialog procedure overridables - to override other messages you must override
	// DialogProc
	virtual BOOL DialogProc( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	virtual BOOL OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	virtual BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	virtual void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
	virtual void OnClose(HWND hWnd);
	virtual void OnDestroy(HWND hWnd);
	virtual void OnSize(HWND hWnd, UINT state, int cx, int cy);
	virtual BOOL OnEraseBkgnd(HWND hDlg, HDC hDC);
	virtual void OnPaint(HWND hWnd);
	virtual void Paint(HDC hDC, LPRECT lpRect, LPPOINT lpDstPt = NULL);
	virtual BOOL GetDIBs( LPPDIB lppCleanDIB, LPPDIB lppDirtyDIB, HDC FAR *lpDC );
	virtual void OnRButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
	virtual void OnTimer(HWND hWnd, UINT id);
	virtual void OnKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags) {};
	virtual BOOL PlayHint(LPCTSTR lpHintWaveFile) { return FALSE; }
	virtual void ToonInitDone() {}

	static CScene FAR *GetScene(HWND hWnd);

protected:
	void Attach(HWND hWnd);
	void Detach();

public:
	static WNDPROC m_lpOldWndProc;
	static CScene FAR *m_lpScene;
	static HMCI m_hMidiTheme;
   
protected:
	HWND		m_hWnd;
	int			m_nSceneNo;
	LPOFFSCREEN m_lpOffScreen;
	BOOL		m_bSceneNoErase; // don't want the app background to paint in between
	BOOL		m_bNoErase;
	LPANIMATOR	m_pAnimator;
	UINT		m_idAnimatorTimer;
	LPSOUND		m_pSound;
	BOOL		m_bDestructing;
};

typedef CScene FAR * LPSCENE;

LONG WINPROC EXPORT SceneWindowProc( HWND hWnd, UINT msg, WPARAM wParam,
							  LPARAM lParam );
BOOL WINPROC EXPORT SceneDialogProc( HWND hDlg, UINT msg, WPARAM wParam,
							  LPARAM lParam );

#endif //_SCENE_H_
