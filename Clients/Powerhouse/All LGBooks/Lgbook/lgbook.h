
#ifndef _LGBOOK_H_
#define _LGBOOK_H_

#include "proto.h"
#include "lgbookid.h"
#include "scene.h"
#include "toonctrl.h"
#include "sprite.h"

#define MAX_FILE_SPEC	32	// max length of filenames used w/o paths
typedef char FILE_SPEC[MAX_FILE_SPEC];

class FAR CGBScene : public CScene
{
public:
	// constructors/destructor
	CGBScene(int nNextSceneNo = 0);
	
	virtual PTOON GetToon();
	virtual void StopEverything(HWND hWnd, BOOL fExiting = FALSE);
	virtual BOOL PlaySound(LPCTSTR lpWaveFile, BOOL fHint);
	int GetNextSceneNo() { return m_nNextSceneNo; }

protected:
	virtual BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	virtual void OnTimer(HWND hWnd, UINT id);
	virtual void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);
	virtual void OnKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
	virtual void OnDestroy(HWND hWnd);
	void OnTitleCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify);

protected:
	BOOL GetDisplaySize(LPINT lpWidth, LPINT lpHeight);
	LPSTR GetResourceData(int idResource, LPCSTR lpszType, HGLOBAL FAR *lphData, BOOL fCheckLanguage = TRUE);
	class CLGBApp FAR * GetApp() { return (class CLGBApp FAR *)::GetApp(); }
	LPSTR GetPathName(LPSTR lpPathName, LPSTR lpFileName);

protected:
	int	m_nNextSceneNo;
	UINT 		m_idTimer;
};

typedef CGBScene FAR * LPGBSCENE;

class FAR CSceneEntry
{     
public:
	CSceneEntry();
	~CSceneEntry() {}

public:
	int			m_nSceneNo;
	int			m_iType;
};

typedef CSceneEntry FAR *LPSCENEENTRY;

class CLGBApp : public CPHApp
{
public:

	enum Language
	{ 
		English	=			0,
		Spanish	=			1
	};

	// Constructor
	CLGBApp();
	// Destructor
	~CLGBApp();

	// Implementation
	virtual LPSCENE CreateScene( int iScene, int iNextScene );
	virtual int GetSceneID(int id);
	virtual int GetFinalSceneID() = 0;
	virtual int GetFirstSceneNo() = 0;
	virtual int GetFinalSceneNo() = 0;

	virtual BOOL GotoFirstScene();
	virtual BOOL GotoScene( HWND hWndPreviousScene, int iScene );
	virtual BOOL GotoScene( HWND hWndPreviousScene, int iScene, int iNextScene);
	virtual BOOL GotoSceneID( HWND hWndPreviousScene, int id, int iNextScene = 0);
//	virtual BOOL FindContent( LPSTR lpFileName, LPSTR lpExpFileName );
	BOOL GotoCurrentScene();
	BOOL GotoNextScene(HWND hWndPreviousScene);
	BOOL GotoNextSkipScene(HWND hWndPreviousScene);
	BOOL GotoPrevScene(HWND hWndPreviousScene);
	BOOL IsPrevScene(int iScene, int iPrevScene);
	BOOL IsNextScene(int iScene, int iNextScene);

	int	GetSceneNo(HWND hDlg = NULL);
	int	GetLastSceneNo() { return m_nLastSceneNo; }
	LPSCENE GetScene();
	BOOL CreateAdditionalWindows();
	Language GetLanguage() { return m_Language; }
	void SetLanguage(Language NewLanguage) { m_Language = NewLanguage; }
	BOOL WriteSettingString(LPCTSTR lpEntry, LPCTSTR lpValue, BOOL fUserSetting = FALSE);
	BOOL WriteSettingInt(LPCTSTR lpEntry, int lpValue, BOOL fUserSetting = FALSE);
	int GetSettingString(LPCTSTR lpEntry, LPTSTR lpValue, LPTSTR lpDefault, BOOL fUserSetting = FALSE);
	UINT GetSettingInt(LPCTSTR lpEntry, int iDefault, BOOL fUserSetting = FALSE);

	// hooks for your initialization code
	BOOL InitInstance();
	int ExitInstance();

	// Initialization Operations - should be done in InitInstance
protected:
	void PlayOpeningTheme() {};
	void OnPaletteChanged(HWND hWnd, HWND hWndCausedBy);
	BOOL OnQueryNewPalette(HWND hWnd);
	#ifdef WIN32
	virtual void OnActivateApp(HWND hWnd, BOOL fActivate, ULONG htaskActDeact);
	#else
	virtual void OnActivateApp(HWND hWnd, BOOL fActivate, HTASK htaskActDeact);
	#endif
	BOOL GetPrevScene(int iScene);
	BOOL GetNextScene(int iScene);

	// Initialization functions 
	virtual BOOL RegisterTitleClasses();

public:
	BOOL		m_fReading;
	BOOL		m_bNewBookmark;
	BOOL		m_fPlaying;
protected:
	int			m_nSceneNo;
	int			m_nLastSceneNo;
	HWND		m_hToonWnd;
	Language	m_Language;
	int			m_nScenes;
	int			m_nBookmark;
	LPSCENEENTRY m_pSceneTable;
};

typedef CLGBApp FAR *LPLGBAPP;

#endif // _LGBOOK_H_  
                          