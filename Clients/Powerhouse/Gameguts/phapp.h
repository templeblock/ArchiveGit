#ifndef _PHAPP_H_
#define _PHAPP_H_

typedef struct _keymap
{
	HWND	hWnd;		// window requesting key
	UINT	vk;			// virtual key code
	UINT	msg;		// message to map to
	WPARAM	wParam;		// wParam of 'message'
	LPARAM	lParam;		// lParam of 'message'
	BOOL	fOnDown;	// on send 'msg' on WM_KEYDOWN
} KEYMAP, FAR *LPKEYMAP;

#define MAX_KEYS 60
#define MAX_EVENTS 4

typedef enum
{
	WV_WIN31,
	WV_WIN32S,
	WV_WIN95,
	WV_WINNT,
} WIN_VERSION;

typedef void (*EVENTCALLBACK)(DWORD dwEventData);

class FAR CPHApp
{
public:

	// Constructor
	CPHApp();
	// Destructor
	~CPHApp();

	// Implementation
	void WinInit(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					 LPTSTR lpCmdLine, int nCmdShow);
	HINSTANCE	GetInstance() { return m_hInstance; }
	HWND		GetMainWnd() { return m_hMainWnd; }
	void		CloseMCIDevices();
	void		ResetJoyMap(void);
	void		SetJoyMapEntry( UINT iButtonCode, HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam );
	void		SetCurrentScene(LPSCENE lpScene = NULL) { m_lpScene = lpScene; }
	LPSCENE		GetCurrentScene() { return m_lpScene; }
	BOOL		IsAppClosing() { return m_fAppClosing; }
	WIN_VERSION GetWinVersion() { return m_WinVer; }
	virtual LPSTR	GetContentDir(LPSTR lpContentDir);
	virtual BOOL 	FindContent( LPSTR lpFileName, LPSTR lpExpFileName, BOOL bBrowse = YES );
	virtual BOOL	IsPrevScene(int iScene, int iPrevScene) { return FALSE; }
	virtual BOOL	IsNextScene(int iScene, int iNextScene) { return FALSE; }

	// remove all entries from the keymap
	void		ResetKeyMap();
	// add an entry to the keymap
	BOOL		SetKeyMapEntry(HWND hWnd, UINT vk,
				UINT msg = 0, WPARAM wParam = 0, LPARAM lParam = 0, BOOL fOnDown = TRUE);
	// remove an entry from keymap
	BOOL		RemoveKeyMapEntry(HWND hWnd, UINT vk, BOOL fOnDown = TRUE);

	// Destroy the title bar window
	void DestroyTitleBar();
	// Remove or add the caption
	void ModifyCaption(HWND hWnd, BOOL fWantCaption);

	int Run();
	BOOL EventAdd( HANDLE hEvent, EVENTCALLBACK pEventProc, DWORD dwEventData );
	BOOL EventRemove( HANDLE hEvent );

	// override this to do any scene specific handling (always override)
	// Not pure so that a demo could be quickly thrown together without
	// any WM_COMMAND handling
	virtual BOOL CreateAdditionalWindows() { return TRUE; }
	virtual BOOL GotoScene( HWND hWndPreviousScene, int iScene );
	virtual BOOL GotoCurrentScene();
	virtual int RealizeOurPalette(HWND hWnd, BOOL bForceBackground);

	// the window proc
	virtual LONG WindowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

	// hooks for your initialization code
	virtual BOOL InitApplication();
	virtual BOOL InitInstance();

	// exiting
	virtual int ExitInstance(); // return app exit code

	// Initialization Operations - should be done in InitInstance
private:
	virtual BOOL GotoFirstScene() { return(GotoScene(NULL, 1)); }
	virtual BOOL RegisterTitleClasses() {return TRUE;}
	virtual BOOL OnIdle(LONG lCount); // return TRUE if more idle processing

	// overridables
	virtual BOOL OnCreate(HWND hWindow, LPCREATESTRUCT lpCreateStruct);
	virtual void OnActivate(HWND hWnd, UINT state, HWND hwndActDeact, BOOL fMinimized);
	#ifdef WIN32
	virtual void OnActivateApp(HWND hWnd, BOOL fActivate, ULONG htaskActDeact);
	#else
	virtual void OnActivateApp(HWND hWnd, BOOL fActivate, HTASK htaskActDeact);
	#endif
	virtual void OnClose(HWND hWnd);
	virtual void OnDestroy(HWND hWnd);
	virtual void OnSize(HWND hWnd, UINT state, int cx, int cy);
	virtual BOOL OnEraseBackground(HWND hWnd, HDC hDC);
	virtual void OnPaletteChanged(HWND hWnd, HWND hWndCausedBy);
	virtual BOOL OnQueryNewPalette(HWND hWnd);
	virtual void OnRButtonUp(HWND hWindow, int x, int y, UINT keyFlags);
	virtual BOOL OnJoyButtonDown(HWND hWindow, UINT iButtonCode, UINT x, UINT y);
	virtual BOOL OnJoyButtonUp(HWND hWindow, UINT iButtonCode, UINT x, UINT y);
	virtual void OnTimer(HWND hWnd, UINT idEvent);
	virtual BOOL OnJoyMove(HWND hWindow, UINT iButtonCode, UINT x, UINT y);
	virtual BOOL OnJoyZMove(HWND hWindow, UINT iButtonCode, UINT x, UINT y);
	virtual void OnCommand(HWND hWnd, int id, HWND hControl, UINT codeNotify) {}
	void OnDisplayChange(HWND hWnd, BYTE cBitsPerPixel, int cxScreen, int cyScreen);
	void OnNCDestroy(HWND hWnd);
	BOOL OnSetCursor(HWND hWnd, HWND hWndCursor, UINT codeHitTest, UINT msg);

	// Initialization functions 
	BOOL CheckForPreviousInstance();
	BOOL RegisterAllClasses();
	virtual BOOL CreateMainWindow();
	BOOL SetupDevices();
	// translate WM_MOUSEMOVE for hidden title bar
	BOOL TranslateTitleBar(LPMSG pMsg);
	// translate WM_KEYDOWN using keymap
	BOOL TranslateKeyMap(LPMSG pMsg);
	void CreateBubble();

	// running and idle processing
	BOOL PumpMessage(LPMSG pMsg);

public:
	// access functions should be created for these someday
	HMCI		m_hDeviceMID;
	HMCI		m_hDeviceAVI;
	HMCI		m_hDeviceWAV;
	HPALETTE	m_hPal;				
	BOOL		m_bAppNoErase;
private:
	STRING		m_szClass;
	HINSTANCE	m_hInstance;
	HINSTANCE	m_hPrevInstance;
	LPTSTR		m_lpCmdLine;
	int			m_nCmdShow;
	HWND		m_hMainWnd;
	MSG			m_msgCur;
	HACCEL		m_hAccel;	  
	LPSCENE		m_lpScene;
	KEYMAP		m_KeyMap[MAX_KEYS];
	int			m_iKeyMapEntries;
	HWND		m_hTitleBarWnd;
	BOOL		m_fAppClosing;
	UINT		m_idCaptionTimer;
	UINT		m_idHintTimer;
	HWND		m_hHintWnd;
	HWND		m_hBubbleWnd;
	WIN_VERSION	m_WinVer;
	int			m_iEvents;					// Number of events to wait on
	HANDLE		m_hEvent[MAX_EVENTS];		// Event handles to wait on
	DWORD		m_dwEventData[MAX_EVENTS];	// Data to pass to the event proc
	EVENTCALLBACK m_pEventProc[MAX_EVENTS];	// The event notification procs
};

typedef CPHApp FAR *LPPHAPP;
LPPHAPP GetApp(void);

#endif // _PHAPP_H_
