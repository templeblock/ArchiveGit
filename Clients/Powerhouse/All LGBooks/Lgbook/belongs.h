#ifndef _BELONGS_H_
#define _BELONGS_H_

#include "proto.h"
#include "lgbook.h"
#include "lgbookid.h"
#include "sprite.h"

#define MAX_NAME_LEN 15
#define MAX_FILE_LEN 13

class FAR CBelongsName
{     
public:
	CBelongsName();
	~CBelongsName();

public:
	char		m_szName[MAX_NAME_LEN];
	char		m_szFileName[MAX_FILE_LEN];
};

typedef CBelongsName FAR *LPBELONGSNAME;

class FAR CBelongsScene : public CGBScene
{
public:
	// constructors/destructor
	CBelongsScene(int nNextSceneNo);
	virtual ~CBelongsScene();
	virtual void ToonInitDone();

private:
	// dialog procedure overridables
	virtual BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	virtual void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
	BOOL OnMessage( HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam );
	UINT OnMCINotify(HWND hWnd, UINT message, HMCI hDeviceID);
	virtual void OnAppActivate(BOOL fActivate);
	virtual void OnDestroy(HWND hWnd);
	void OnKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
	// helper functions
	virtual void OnTimer(HWND hWnd, UINT id);
	void PlayBelongsTo(BOOL fNameOnly);
	void StripLeadingSpaces(LPSTR lpString);

private:
	int				m_nNames;
	LPBELONGSNAME 	m_pNames;
	HMCI			m_hSound;
	HFONT 			m_hFont;
};

typedef CBelongsScene FAR * LPBELONGSSCENE;
                            
#endif // _BELONGS_H_