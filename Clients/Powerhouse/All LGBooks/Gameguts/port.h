#ifndef _PORT_H_
#define _PORT_H_

#if defined(WIN32) || defined(_WIN32)
	#define GetCurrentInstance()	GetModuleHandle(NULL)
#else
	#define GetCurrentInstance()	(HINSTANCE)SELECTOROF((LPVOID)&of)
#endif

#define HANDLE_DLGRET(hwnd, message, fn)	\
	case (message): return (BOOL)HANDLE_##message((hwnd), (wParam), (lParam), (fn))
#define HANDLE_DLGMSG(hwnd, message, fn)	\
	case (message): HANDLE_##message((hwnd), (wParam), (lParam), (fn)); return TRUE;
#define MONITOR_DLGMSG(hwnd, message, fn)	 \
	case (message): HANDLE_##message((hwnd), (wParam), (lParam), (fn)); return FALSE;

/* void Cls_OnDropFiles(HWND hwnd, HDROP hdrop); */
#define HANDLE_WM_DROPFILES(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (HDROP)(wParam)), 0L)
/* void Cls_OnSetState(HWND hwnd, BOOL fState) */
#define HANDLE_BM_SETSTATE(hwnd, wParam, lParam, fn) \
	((fn)(hwnd, (BOOL)wParam), 0L)
/* void Cls_OnSetCheck(HWND hwnd, BOOL fCheck) */
#define HANDLE_BM_SETCHECK(hwnd, wParam, lParam, fn) \
	((fn)(hwnd, (BOOL)wParam), 0L)
/* BOOL Cls_OnGetCheck(HWND hwnd) */
#define HANDLE_BM_GETCHECK(hwnd, wParam, lParam, fn) \
	MAKELRESULT((BOOL)(fn)(hwnd), 0L)
/*  */
#define HANDLE_BM_SETCOLOR(hwnd, wParam, lParam, fn) \
	((fn)(hwnd, (COLORREF)wParam, (BOOL)lParam), 0L)
/*  */
#define HANDLE_BM_GETCOLOR(hwnd, wParam, lParam, fn) \
	MAKELRESULT((COLORREF)(fn)(hwnd), 0L)


#define BM_GETCHECK16		(WM_USER+0)
#define BM_SETCHECK16		(WM_USER+1)
#define BM_GETSTATE16		(WM_USER+2)
#define BM_SETSTATE16		(WM_USER+3)

#ifndef WIN32
#define WM_DISPLAYCHANGE				0x007E
#define COLOR_INFOTEXT			23
#define COLOR_INFOBK			24
#define WF_WINNT         0x4000
#endif

/* void Cls_OnDisplayChange(HWND hWnd, BYTE cBitsPerPixel, int cxScreen, int cyScreen) */
#define HANDLE_WM_DISPLAYCHANGE(hwnd, wParam, lParam, fn) \
	((fn)(hwnd, (BYTE)wParam, (int)LOWORD(lParam), (int)HIWORD(lParam)), 0L)
/* void Cls_OnWomDone(HWND hWnd, HWAVEOUT hDevice, LPWAVEHDR lpWaveHdr) */
#define HANDLE_MM_WOM_DONE(hwnd, wParam, lParam, fn) \
	((fn)(hwnd, (HWAVEOUT)wParam, (LPWAVEHDR)lParam), 0L)

/* void Cls_OnSetState(HWND hwnd, BOOL fState) */
#define HANDLE_BM_SETSTATE16(hwnd, wParam, lParam, fn) \
	((fn)(hwnd, (BOOL)wParam), 0L)
/* void Cls_OnSetCheck(HWND hwnd, BOOL fCheck) */
#define HANDLE_BM_SETCHECK16(hwnd, wParam, lParam, fn) \
	((fn)(hwnd, (BOOL)wParam), 0L)
/* BOOL Cls_OnGetCheck(HWND hwnd) */
#define HANDLE_BM_GETCHECK16(hwnd, wParam, lParam, fn) \
	MAKELRESULT((BOOL)(fn)(hwnd), 0L)

/* UINT Cls_OnMCINotify(HWND hwnd, UINT notifyCode, HMCI hDevice) */
#define HANDLE_MM_MCINOTIFY(hwnd, wParam, lParam, fn) \
	(LRESULT)(DWORD)(UINT)(fn)((hwnd), (UINT)(wParam), (HMCI)(LOWORD(lParam)))

/* UINT Cls_OnJoy1ButtonDown(HWND hwnd, UINT iButtonCode, int x, int y) */
#define HANDLE_MM_JOY1BUTTONDOWN(hwnd, wParam, lParam, fn) \
	(LRESULT)(DWORD)(UINT)(fn)((hwnd), (UINT)(wParam), (UINT)(LOWORD(lParam)), (UINT)(HIWORD(lParam)))

/* UINT Cls_OnJoy1ButtonUp(HWND hwnd, UINT iButtonCode, int x, int y) */
#define HANDLE_MM_JOY1BUTTONUP(hwnd, wParam, lParam, fn) \
	(LRESULT)(DWORD)(UINT)(fn)((hwnd), (UINT)(wParam), (UINT)(LOWORD(lParam)), (UINT)(HIWORD(lParam)))

/* UINT Cls_OnJoy1Move(HWND hwnd, UINT iButtonCode, int x, int y) */
#define HANDLE_MM_JOY1MOVE(hwnd, wParam, lParam, fn) \
	(LRESULT)(DWORD)(UINT)(fn)((hwnd), (UINT)(wParam), (UINT)(LOWORD(lParam)), (UINT)(HIWORD(lParam)))

/* UINT Cls_OnJoy1ZMove(HWND hwnd, UINT iButtonCode, int x, int y) */
#define HANDLE_MM_JOY1ZMOVE(hwnd, wParam, lParam, fn) \
	(LRESULT)(DWORD)(UINT)(fn)((hwnd), (UINT)(wParam), (UINT)(LOWORD(lParam)), (UINT)(HIWORD(lParam)))

#ifdef WIN32

#define MyCharNext(lp) CharNext(lp)
 #define GET_WINDOW_ID(hWindow) \
	GetWindowLong(hWindow, GWL_ID)
 #define GET_WINDOW_INSTANCE(hWindow) \
	GetWindowLong(hWindow, GWL_HINSTANCE)
 #define GET_WINDOW_PARENT(hWindow) \
	GetWindowLong(hWindow, GWL_HWNDPARENT)

  #define SET_CLASS_STYLE(hWnd,style) \
	SetClassLong(hWnd, GCL_STYLE, (long)style)
 #define SET_CLASS_CURSOR(hWnd,hCursor) \
	SetClassLong(hWnd, GCL_HCURSOR, (long)hCursor)
 #define SET_CLASS_HBRBACKGROUND(hWnd,hBrush) \
	SetClassLong(hWnd, GCL_HBRBACKGROUND, (long)hBrush)
 #define SET_CLASS_CBCLSEXTRA(hWnd,extra) \
	SetClassLong(hWnd, GCL_CBCLSEXTRA, (long)extra)

 #define GET_CLASS_STYLE(hWnd) \
	GetClassLong(hWnd, GCL_STYLE)
 #define GET_CLASS_CURSOR(hWnd) \
	GetClassLong(hWnd, GCL_HCURSOR)
 #define GET_CLASS_HBRBACKGROUND(hWnd) \
	GetClassLong(hWnd, GCL_HBRBACKGROUND)
 #define GET_CLASS_CBCLSEXTRA(hWnd) \
	GetClassLong(hWnd, GCL_CBCLSEXTRA)

 POINT MAKEPOINT(long l);	// POINT is 32bit values, POINTS is 16bit values	

#else

 #define MyCharNext(lp) AnsiNext(lp)
 #define GET_WINDOW_ID(hWindow) \
	GetWindowWord(hWindow, GWW_ID)
 #define GET_WINDOW_INSTANCE(hWindow) \
	GetWindowWord(hWindow, GWW_HINSTANCE)
 #define GET_WINDOW_PARENT(hWindow) \
	GetWindowWord(hWindow, GWW_HWNDPARENT)
	
#define SET_CLASS_STYLE(hWnd,style) \
	SetClassWord(hWnd, GCW_STYLE, (WORD)style)
 #define SET_CLASS_CURSOR(hWnd,hCursor) \
	SetClassWord(hWnd, GCW_HCURSOR, (WORD)hCursor)
 #define SET_CLASS_HBRBACKGROUND(hWnd,hBrush) \
	SetClassWord(hWnd, GCW_HBRBACKGROUND, (WORD)hBrush)
 #define SET_CLASS_CBCLSEXTRA(hWnd,extra) \
	SetClassWord(hWnd, GCW_CBCLSEXTRA, (WORD)extra)

 #define GET_CLASS_STYLE(hWnd) \
	GetClassWord(hWnd, GCW_STYLE)
 #define GET_CLASS_CURSOR(hWnd) \
	GetClassWord(hWnd, GCW_HCURSOR)
 #define GET_CLASS_HBRBACKGROUND(hWnd) \
	GetClassWord(hWnd, GCW_HBRBACKGROUND)
 #define GET_CLASS_CBCLSEXTRA(hWnd) \
	GetClassWord(hWnd, GCW_CBCLSEXTRA)
	
#endif

#endif // _PORT_H_
