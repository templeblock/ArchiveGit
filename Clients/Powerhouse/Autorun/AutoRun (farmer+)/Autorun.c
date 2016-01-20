#define APPNAME "LGBAutoRun"

/******************************************************************************\
    PROGRAM:    AutoRun.C
    Author:     Robert B. Hess
    Date:       1-Oct-94
\******************************************************************************/
// The program will be executed relative to the root of the CD,
// no drive letter is necessary.  We strongly recommend that the
// autorun application be a small fast loading executable that
// immediatly presents the option to discontiue execution of the
// autorun.  This is to give the user an easy way out if they did
// not expect, or intend to launch an application.  Autorun CD's
// should always present an uninstall option to clean up the
// hardrive at the users request.  In general Autorun CD's should
// be very cautious about leaving anything on the users hardrive
// on closing.

/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1994 Microsoft Corporation.
*       All rights reserved.
*
*       THE INFORMATION AND CODE PROVIDED HEREUNDER (COLLECTIVELY REFERRED TO
*       AS "SOFTWARE") IS PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND, EITHER
*       EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
*       WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. IN
*       NO EVENT SHALL MICROSOFT CORPORATION OR ITS SUPPLIERS BE LIABLE FOR
*       ANY DAMAGES WHATSOEVER INCLUDING DIRECT, INDIRECT, INCIDENTAL,
*       CONSEQUENTIAL, LOSS OF BUSINESS PROFITS OR SPECIAL DAMAGES, EVEN IF
*       MICROSOFT CORPORATION OR ITS SUPPLIERS HAVE BEEN ADVISED OF THE
*       POSSIBILITY OF SUCH DAMAGES. SOME STATES DO NOT ALLOW THE EXCLUSION OR
*       LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES SO THE
*       FOREGOING LIMITATION MAY NOT APPLY.
*
\******************************************************************************/

// Windows Header Files:
#include <windows.h>
#include <windowsx.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>

// Local Header Files
#include "AUTORUN.h"

/////////////////////////////////////////////////////////////////////////
// Global Variables:

HINSTANCE hInst; // current instance
char szAppName[] = APPNAME; // The name of this application
char szTitle[64]; // The title bar text
char szPath[_MAX_PATH];
char szTitleExe[_MAX_PATH];
char szAppTitle[64]; // The title bar of the app
char szAppClass[64]; // The class name of the app

/////////////////////////////////////////////////////////////////////////
// Foward declarations of functions included in this code module:

BOOL InitApplication(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CenterWindow (HWND, HWND);

// The following functions were added to Generic for this AutoRun sample app:
int OnCreate (HWND hWnd);
BOOL LaunchApplication(void);
BOOL Launch(LPSTR lpAppName);



/************************************************************************/
BOOL FileExists( LPSTR lpFileName )
/************************************************************************/
{
	OFSTRUCT of;
	HFILE hFile;
	if ( (hFile = OpenFile( lpFileName, &of, OF_EXIST )) != HFILE_ERROR )
		return( TRUE );
	return( FALSE );
}

//************************************************************************
LPCSTR FileName( LPCSTR lpPath )
//************************************************************************
{
	LPCSTR lp;

	lp = lpPath;
	lp += lstrlen( lpPath );
	while( *lp != '\\' && *lp != ':' )
	{
		if ( (--lp) < lpPath )
		    return( lpPath + lstrlen(lpPath) ); // null string
	}
	lp++;
	return( lp ); // the file name
}

//************************************************************************
LPSTR StripFile( LPSTR lpPath )
//************************************************************************
{
	LPSTR lp;

	if ( lp = (LPSTR)FileName( lpPath ) )
		*lp = '\0';
	return( lpPath );
}

BOOL ActivateApp()
{
	HWND hwnd;

	hwnd = FindWindow (szAppName, NULL);
	if (hwnd) {
		// We found another version of ourself. Lets defer to it:
		if (IsIconic(hwnd)) {
			ShowWindow(hwnd, SW_RESTORE);
		}
		#ifdef WIN32
		SetForegroundWindow (GetLastActivePopup (hwnd));	
		#else
		SetActiveWindow (GetLastActivePopup (hwnd));	
		#endif

		// If this app actually had any functionality, we would
		// also want to communicate any action that our 'twin'
		// should now perform based on how the user tried to
		// execute us.
		return TRUE;
	}
	return(FALSE);
}

/************************************************************************\
 *    FUNCTION: WinMain
\************************************************************************/

#ifdef WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	MSG msg;
	HANDLE hAccelTable;
	char szSetup[_MAX_PATH];

	// Lets replace the default window title, with the name of the
	// actual application we want to launch...
	GetModuleFileName(hInstance, szPath, sizeof(szPath));
	StripFile(szPath);
	// remove line below
	//lstrcpy(szPath, "e:\\");

	lstrcpy(szSetup, szPath);
	// remove line below
	//lstrcpy(szSetup, "d:\\dev\\src\\sailor\\");
	lstrcat(szSetup, "setup.lst");

	GetPrivateProfileString("Setup", "Title", "", szTitle, sizeof(szTitle), szSetup);
	GetPrivateProfileString("Setup", "TitleExe", "", szTitleExe, sizeof(szTitleExe), szSetup);
	GetPrivateProfileString("Setup", "AppTitle", "", szAppTitle, sizeof(szAppTitle), szSetup);
	GetPrivateProfileString("Setup", "AppClass", "", szAppClass, sizeof(szAppClass), szSetup);

	// see if application we launch is already running
	if (FindWindow(szAppClass, szAppTitle))
		return(FALSE);

	if (!hPrevInstance) { // Will always be TRUE on Windows 95
		// Perform instance initialization:
		if (!InitApplication(hInstance)) {
			return (FALSE);
		}
	}
	else
	{
		ActivateApp();
		return(FALSE);
	}

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow)) {
		return (FALSE);
	}

	hAccelTable = LoadAccelerators (hInstance, szAppName);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!TranslateAccelerator (msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (msg.wParam);

	lpCmdLine; // This will prevent 'unused formal parameter' warnings
}


/************************************************************************\
 *    FUNCTION: InitApplication
\************************************************************************/

BOOL InitApplication(HINSTANCE hInstance)
{
	WNDCLASS  wc;

	// Win32 will always set hPrevInstance to NULL, so lets check
	// things a little closer. This is because we only want a single
	// version of this app to run at a time
	#ifdef WIN32
	if (ActivateApp())
		return(FALSE);
	#endif

	// Fill in window class structure with parameters that describe
	// the main window.
	wc.lpszClassName = szAppName;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = (WNDPROC)WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.lpszMenuName  = NULL;
	wc.hIcon         = LoadIcon (hInstance, szAppName);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);

	return RegisterClass(&wc);
}


/************************************************************************\
 *    FUNCTION: InitInstance
\************************************************************************/

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	
	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szAppName, szTitle, 
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		0, 0,
		0, 0,
		NULL, NULL, hInstance, NULL);

	if (!hWnd) {
		return (FALSE);
	}

	// Set the small icon
	#ifdef WIN32
#if !defined(WM_SETICON)
#define WM_SETICON 0x0080
#endif
	SendMessage(hWnd, WM_SETICON, FALSE, (LPARAM)LoadIcon(hInst, "SMALL"));
	#endif

	ShowWindow(hWnd, nCmdShow);

	UpdateWindow(hWnd);
	return(TRUE);
}


/************************************************************************\
 *    FUNCTION: WndProc
\************************************************************************/

HPALETTE hpal;
HBITMAP hbmAutoRun;
BITMAP bmAutoRun;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	UINT uMappedColors; // Used in WM_QUERYNEWPALETTE
	RECT rect; // Used in WM_PAINT
	HDC  hdcSrc; // Used in WM_PAINT
	HBITMAP hbmOld; // Used in WM_PAINT


	switch (message) {

		case WM_CREATE:
			return OnCreate (hWnd);

		case WM_PALETTECHANGED:
			if ((HWND)wParam == hWnd) {
				break;
			}

			// fall through to WM_QUERYNEWPALETTE

		case WM_QUERYNEWPALETTE:
			hdc = GetDC(hWnd);

			if (hpal) {
				SelectPalette(hdc, hpal, FALSE);
			}

			uMappedColors = RealizePalette(hdc);
			ReleaseDC(hWnd,hdc);

			if (uMappedColors>0) {
				InvalidateRect(hWnd,NULL,TRUE);
				return TRUE;
			} else {
				return FALSE;
			}
			break;

		case WM_COMMAND:
			wmId    = LOWORD(wParam); // Remember, these are...
			wmEvent = HIWORD(wParam); // ...different for Win32!
		
			switch (wmId) {

				case IDM_CONTINUE:
					if (!LaunchApplication()) {
						// failed to launch your application, you need
						// to decide what to do in this case and add code
						// to do that here. You can use GetLastError() to
						// determine the cause of this failure.
					} else {
						PostMessage (hWnd, WM_CLOSE, 0, 0);
					}
					break;

				case IDM_INSTALL:
					if (!Launch("setup.exe")) {
						// failed to launch your application, you need
						// to decide what to do in this case and add code
						// to do that here. You can use GetLastError() to
						// determine the cause of this failure.
					} else {
						PostMessage (hWnd, WM_CLOSE, 0, 0);
					}
					break;

				case IDM_DEMO:
					if (!Launch("lgbdemo\\lgbdemo.exe")) {
						// failed to launch your application, you need
						// to decide what to do in this case and add code
						// to do that here. You can use GetLastError() to
						// determine the cause of this failure.
					} else {
						PostMessage (hWnd, WM_CLOSE, 0, 0);
					}
					break;

				case IDM_EXIT:
					DestroyWindow (hWnd);
					break;

				default:
					return (DefWindowProc(hWnd, message, wParam, lParam));
			}
			break;

		case WM_PAINT:
			hdc = BeginPaint (hWnd, &ps);
			SelectPalette(hdc, hpal, FALSE);
			RealizePalette(hdc);
			
			GetClientRect (hWnd, &rect);
			hdcSrc = CreateCompatibleDC (hdc);
			hbmOld = SelectObject (hdcSrc, hbmAutoRun);
			SelectPalette(hdcSrc, hpal, FALSE);
			RealizePalette(hdcSrc);
			if (!BitBlt (hdc, 0, 0, bmAutoRun.bmWidth, bmAutoRun.bmHeight, hdcSrc, 0, 0, SRCCOPY)) {
				MessageBeep(0);
			}
			SelectObject (hdcSrc, hbmOld);
			DeleteDC (hdcSrc);
			EndPaint (hWnd, &ps);
			break;        

		case WM_DESTROY:
			// Tell WinHelp we don't need it any more...
			WinHelp (hWnd, APPNAME".HLP", HELP_QUIT,(DWORD)0);
			DeleteObject (hpal);
			DeleteObject (hbmAutoRun);
			PostQuitMessage(0);
			break;

		default:
			return (DefWindowProc(hWnd, message, wParam, lParam));
	}
	return (0);
}


/************************************************************************\
 *    FUNCTION: CenterWindow
\************************************************************************/
// This is a 'utility' function I find usefull. It will center one
// window over another. It also makes sure that the placement is within
// the 'working area', meaning that it is both within the display limits
// of the screen, -and- not obscured by the tray or other frameing
// elements of the desktop.
BOOL CenterWindow (HWND hwndChild, HWND hwndParent)
{
	RECT    rChild, rParent, rWorkArea = {0,0,0,0};
	int     wChild, hChild, wParent, hParent;
	int     wScreen, hScreen, xScreen, yScreen, xNew, yNew;

	// Get the Height and Width of the child window
	GetWindowRect (hwndChild, &rChild);
	wChild = rChild.right - rChild.left;
	hChild = rChild.bottom - rChild.top;

	// Get the Height and Width of the parent window
	GetWindowRect (hwndParent, &rParent);
	wParent = rParent.right - rParent.left;
	hParent = rParent.bottom - rParent.top;

	// Get the limits of the 'workarea'
	#ifdef WIN32
#if !defined(SPI_GETWORKAREA)
#define SPI_GETWORKAREA 48
#endif
	bResult = SystemParametersInfo(
    	SPI_GETWORKAREA,	// system parameter to query or set
    	sizeof(RECT),	// depends on action to be taken
    	&rWorkArea,	// depends on action to be taken
    	0);	
	wScreen = rWorkArea.right - rWorkArea.left;
	hScreen = rWorkArea.bottom - rWorkArea.top;
	xScreen = rWorkArea.left;
	yScreen = rWorkArea.top;

    #else 
    wScreen = hScreen = xScreen = yScreen = 0;
    #endif

	// On Windows NT, the above metrics aren't valid (yet), so they all return
	// '0'. Lets deal with that situation properly:
	if (wScreen==0 && hScreen==0) {
		wScreen = GetSystemMetrics(SM_CXSCREEN);
		hScreen = GetSystemMetrics(SM_CYSCREEN);
		xScreen = 0; // These values should already be '0', but just in case
		yScreen = 0;
	}

	// Calculate new X position, then adjust for screen
	xNew = rParent.left + ((wParent - wChild) /2);
	if (xNew < xScreen) {
		xNew = xScreen;
	} else if ((xNew+wChild) > wScreen) {
		xNew = (xScreen + wScreen) - wChild;
	}

	// Calculate new Y position, then adjust for screen
	yNew = rParent.top  + ((hParent - hChild) /2);
	if (yNew < yScreen) {
		yNew = yScreen;
	} else if ((yNew+hChild) > hScreen) {
		yNew = (yScreen + hScreen) - hChild;
	}

	// Set it, and return
	return SetWindowPos (hwndChild, NULL, xNew, yNew, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}


//************************************************************************
LPSTR FixPath( LPSTR lpPath )
//************************************************************************
{
int length;

if ( !(length = lstrlen(lpPath)) )
	return( lpPath );
while ( length && (lpPath[length-1] == '\n' || lpPath[length-1] == '\r') )
	lpPath[--length] = '\0';
if ( !length )
	return( lpPath );
if ( lpPath[length-1] != '\\' )
	lstrcat( lpPath, "\\" );
return( lpPath );
}

BOOL IsInstalled()
{
	char szIniName[32];
	char szSection[32];
	char szIniPath[_MAX_PATH];
	char szLoader[_MAX_PATH];

	// get name of ini to look in
	LoadString (hInst, ID_ININAME, szIniName, sizeof(szIniName));
	GetWindowsDirectory(szIniPath, sizeof(szIniPath));
	FixPath(szIniPath);
	lstrcat(szIniPath, szIniName);

	LoadString (hInst, ID_SECTION, szSection, sizeof(szSection));

	szLoader[0] = '\0';
	GetPrivateProfileString(szSection, szTitle, "", szLoader, sizeof(szLoader), szIniPath);

	if (lstrlen(szLoader))
		return(FileExists(szLoader));
	else
		return(FALSE);
}

LPBITMAPFILEHEADER ReadBitmap(LPSTR lpFileName)
{
	HFILE fh;
	OFSTRUCT of;
	DWORD dwSize;
	LPBITMAPFILEHEADER lp;

	if ( (fh = OpenFile(lpFileName, &of, OF_READ)) < 0 )
		return(NULL);
		
	dwSize = _llseek(fh, 0, 2);	
	_llseek(fh, 0, 0);

	lp = (LPBITMAPFILEHEADER)GlobalAllocPtr(GPTR, dwSize);
	if (!lp)
		return(NULL);

	// read in the bits
	_hread( fh, lp, dwSize );
	_lclose( fh );
	return(lp);
}

/************************************************************************\
 *    FUNCTION: OnCreate
\************************************************************************/
// This function will be called when the WM_CREATE message is processed
// by our application. This is where we will load the bitmap to be
// displayed, create a palette for the image, resize the window to fit
// the bitmap, and put in our control buttons that will allow the user
// to tell us how to proceed.

int OnCreate(HWND hWnd)
{
	HDC hdc;
	RECT rect;
	HFONT hfontBtn;
	HWND hwndBtnOk, hwndBtnExit, hwndBtnDemo;

	int i;
	LPBITMAPINFO lpbmi;
	LPBITMAPFILEHEADER lpbfh;

	int id;
	char szFileName[_MAX_PATH];

	LOGPALETTE FAR *plgpl;
	int cColors;
	LPVOID pimage;
	TEXTMETRIC tm;
	int iHeight, iWidth, x, y;
	char szString[80];

	hdc = GetDC(hWnd);

	hpal = NULL;

	lstrcpy(szFileName, szPath);
	lstrcat(szFileName, "title.bmp");
	
	lpbfh = ReadBitmap(szFileName);
	if (!lpbfh)
		return(-1);

	lpbmi = (LPBITMAPINFO)((LPSTR)lpbfh + sizeof(BITMAPFILEHEADER));

	// How many colors does it use?:
	cColors = (int)lpbmi->bmiHeader.biClrUsed;
	if (cColors == 0)
		cColors = (1 << (int)(lpbmi)->bmiHeader.biBitCount);

	// Use that to determine where the actual bitmap image starts:
	pimage = &(lpbmi->bmiColors[cColors]);

	// Now lets create a palette based on the image we loaded:
	plgpl = (LPVOID)GlobalAllocPtr (GPTR, sizeof(LOGPALETTE) + (cColors-1)*sizeof(PALETTEENTRY));
	if (!plgpl) return -1; // failure

	plgpl->palVersion = 0x300;
	plgpl->palNumEntries = cColors;
	for (i=0; i<cColors; i++)
	{
		plgpl->palPalEntry[i].peRed   = lpbmi->bmiColors[i].rgbRed;
		plgpl->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
		plgpl->palPalEntry[i].peBlue  = lpbmi->bmiColors[i].rgbBlue;
		//plgpl->palPalEntry[i].peFlags = PC_NOCOLLAPSE; // is this needed?
	}
	hpal = CreatePalette(plgpl);

	// And free up the memory we allocated:
	GlobalFreePtr (plgpl);

	if (!hpal) return -1; // Fail on no palette

	// Now create a true DIBitmap from all of this:
	// First assign the palette to the DC:
	SelectPalette(hdc, hpal, FALSE);
	RealizePalette(hdc);

	// Now create a DIB based off the the DC with the bitmap image from the resource:
	hbmAutoRun = CreateDIBitmap (hdc, (BITMAPINFOHEADER FAR *)lpbmi,
		CBM_INIT, pimage, lpbmi, DIB_RGB_COLORS);

	GlobalFreePtr(lpbfh);

	if (!hbmAutoRun)
		return(-1);

	// And get the header data:
	GetObject (hbmAutoRun, sizeof(BITMAP), &bmAutoRun);

	SetWindowPos (hWnd, NULL, 0, 0, bmAutoRun.bmWidth, bmAutoRun.bmHeight,
		SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	// Since what we really want to do is size the 'Client' window,
	// Lets find out how far off we were, adjust our values, and
	// try it again...
	GetClientRect (hWnd, &rect);
	x = bmAutoRun.bmWidth - (rect.right-rect.left);
	y = bmAutoRun.bmHeight - (rect.bottom-rect.top);
	SetWindowPos (hWnd, NULL, 0, 0, bmAutoRun.bmWidth+x,
		bmAutoRun.bmHeight+y, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

	CenterWindow (hWnd, GetDesktopWindow ());

	// Ok, our image is loaded, and our window properly sized
	// Lets add in some controls: The text to use for these
	// buttons is coming out of the resource of this app
	if (IsInstalled())
		id = IDM_CONTINUE;
	else
		id = IDM_INSTALL;

	LoadString (hInst, id, szString, sizeof(szString));
	hwndBtnOk = CreateWindow ("BUTTON", szString,
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | BS_DEFPUSHBUTTON,
		0, 0, 0, 0,
		hWnd, (HMENU)id, hInst, NULL);

	LoadString (hInst, IDM_EXIT, szString, sizeof(szString));
	hwndBtnExit = CreateWindow ("BUTTON", szString, 
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | BS_PUSHBUTTON,
		0, 0, 0, 0,
		hWnd, (HMENU)IDM_EXIT, hInst, NULL);

	if ( id == IDM_CONTINUE )
	{
		LoadString (hInst, IDM_DEMO, szString, sizeof(szString));
		hwndBtnDemo = CreateWindow ("BUTTON", szString, 
			WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0,
			hWnd, (HMENU)IDM_DEMO, hInst, NULL);
	}

	hfontBtn = GetStockObject (ANSI_VAR_FONT);
	SendMessage (hwndBtnOk, WM_SETFONT, (WPARAM)hfontBtn,(LPARAM)TRUE);
	SendMessage (hwndBtnExit, WM_SETFONT, (WPARAM)hfontBtn, (LPARAM)TRUE);
	if ( id == IDM_CONTINUE )
		SendMessage (hwndBtnDemo, WM_SETFONT, (WPARAM)hfontBtn, (LPARAM)TRUE);
	SelectObject (hdc, hfontBtn);
	GetTextMetrics (hdc, &tm);

	iHeight = (tm.tmHeight + tm.tmExternalLeading) * 2;
	iWidth = (bmAutoRun.bmWidth / 2) - (iHeight * 2);
			
	x = (bmAutoRun.bmWidth/2 - iWidth)/2;
	y = bmAutoRun.bmHeight - (iHeight*3);

	SetWindowPos (hwndBtnOk, NULL, x, y, iWidth, iHeight,
		SWP_NOZORDER | SWP_NOACTIVATE);

	if ( id == IDM_CONTINUE )
		SetWindowPos (hwndBtnDemo, NULL, x, y+((iHeight*3)/2), bmAutoRun.bmWidth-2*x, iHeight,
			SWP_NOZORDER | SWP_NOACTIVATE);

	x += bmAutoRun.bmWidth/2;
	SetWindowPos (hwndBtnExit, NULL, x, y, iWidth, iHeight,
		SWP_NOZORDER | SWP_NOACTIVATE);

	ShowWindow (hWnd, SW_SHOW);

	ReleaseDC (hWnd, hdc);

	return 0; // Sucess
}

/************************************************************************\
 *    FUNCTION: 
\************************************************************************/
// This function is called to launch the 'real' application when the
// user instructs us to. The name of the application to be run is coming
// out of the resource of this app.
BOOL LaunchApplication(void)
{
	char szImagePath[_MAX_PATH];
	char szCmdLine[40];

	#ifdef WIN32
	STARTUPINFO         si;
	PROCESS_INFORMATION pi;
	#else
	UINT err;
	#endif

	lstrcpy(szImagePath, szPath);
	lstrcat(szImagePath, szTitleExe);
	LoadString (hInst, ID_CMDLINE, szCmdLine, sizeof(szCmdLine));
	lstrcat(szImagePath, " ");
	lstrcat(szImagePath, szCmdLine);

	#ifdef WIN32
	si.cb = sizeof(STARTUPINFO);
	// Yeah, this could be done with a 'memset', but this is more illustrative:
	si.lpReserved =
		si.lpDesktop =
		si.lpTitle	=
		si.dwX	=
		si.dwY =
		si.dwXSize	=
		si.dwYSize =
		si.dwXCountChars =
		si.dwYCountChars =
		si.dwFillAttribute =
		si.dwFlags =
		si.wShowWindow =
		si.cbReserved2 =
		si.lpReserved2 =
		si.hStdInput =
		si.hStdOutput =
		si.hStdError =
		NULL;

	return CreateProcess (NULL, szImagePath, NULL, NULL, FALSE,
		CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pi);
	#else
	err = WinExec(szImagePath, SW_SHOW);
	return (err >= 32);
	#endif
}

/************************************************************************\
 *    FUNCTION: 
\************************************************************************/
// This function is called to launch the 'real' application when the
// user instructs us to. The name of the application to be run is coming
// out of the resource of this app.
BOOL Launch(LPSTR lpAppName)
{
	char szImagePath[_MAX_PATH];

	#ifdef WIN32
	STARTUPINFO         si;
	PROCESS_INFORMATION pi;
	#else
	UINT err;
	#endif

	lstrcpy(szImagePath, szPath);
	lstrcat(szImagePath, lpAppName);

	#ifdef WIN32
	si.cb = sizeof(STARTUPINFO);
	// Yeah, this could be done with a 'memset', but this is more illustrative:
	si.lpReserved =
		si.lpDesktop =
		si.lpTitle	=
		si.dwX	=
		si.dwY =
		si.dwXSize	=
		si.dwYSize =
		si.dwXCountChars =
		si.dwYCountChars =
		si.dwFillAttribute =
		si.dwFlags =
		si.wShowWindow =
		si.cbReserved2 =
		si.lpReserved2 =
		si.hStdInput =
		si.hStdOutput =
		si.hStdError =
		NULL;

	return CreateProcess (NULL, szImagePath, NULL, NULL, FALSE,
		CREATE_NEW_PROCESS_GROUP, NULL, NULL, &si, &pi);
	#else
	err = WinExec(szImagePath, SW_SHOW);
	return (err >= 32);
	#endif
}
