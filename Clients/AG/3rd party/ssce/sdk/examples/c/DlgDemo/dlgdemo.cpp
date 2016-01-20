// Sentry Spelling Checker Engine
// DlgDemo: Demonstrate SSCE Windows features: built-in dialogs,
// as-you-type spell checking (as-you-type works in 32-bit version
// only)

// Copyright (c) 1994 Wintertree Software Inc.
// www.wintertree-software.com

// Use, duplication, and disclosure of this file is governed by
// a license agreement between Wintertree Software Inc. and
// the licensee.

// $Id: dlgdemo.c,v 5.14 2000/06/28 17:34:02 wsi Exp wsi $

#include <windows.h>
#include <windowsx.h>
#if defined(_WIN32)
	#include <richedit.h>
#endif
#include <assert.h>
#include <errno.h>
#include <commdlg.h>
#include <ssce.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "resource.h"

// IMPORTANT: Set licenseKey to your Sentry license key,
// provided by Wintertree Software. The demo won't build without it.
// Example: static SSCE_U32 licenseKey = 0x1234ABCD;
static SSCE_S32 licenseKey = 0x8359C65E;

#if defined(_MSC_VER)
  #include <io.h>	/* for mktemp */
#endif
#if defined(__BORLANDC__)
  #include <dir.h>	/* for mktemp */
#endif

#define APP_NAME "DlgDemo"
#define APP_TITLE "SSCE Demo"
#if !defined(MAX_PATH)
  #define MAX_PATH 512
#endif
#define MAX_FILE_NAME (8 + 1 + 3 + 1)
#if !defined(_WIN32)
  #define MAX_FILE_SIZE 30000
#endif

#if defined(_WIN32)

// Rich-edit definitions
#if !defined(RICHEDIT_DLL_NAME)
#define RICHEDIT_DLL_NAME "riched20.dll"
#define RICHEDIT_CLASS_NAME "RichEdit20A"
#endif

#define MAX_SUGGESTIONS 16
#endif

#define IDC_TEXT 1
#define FANCY_APOSTROPHE 0x92

// Safe strcpy: Like strncpy, but always leaves a terminating null.
#define safeStrCpy(dst, dstSz, src) strncpy(dst, src, (dstSz) - 1); \
  (dst)[(dstSz) - 1] = '\0'

#if defined(_WIN32)

// Return TRUE if a character can appear within a word
#define IS_WORD_CHAR(c) (IsCharAlphaNumeric(c) || '\'' == (c) || FANCY_APOSTROPHE == (c))

// Not defined in windowsx.h:
#define HANDLE_WM_NOTIFY(hwnd, wParam, lParam, fn) \
    ((fn)((hwnd), (int)(wParam), (NMHDR *)(lParam)), 0L)

#endif // _WIN32

static const char appName[] = APP_NAME;
static HWND textHWnd;
static HINSTANCE instance;

long FAR PASCAL WinProc(HWND win, WORD msgType, WPARAM wParam, LPARAM lParam);

// Forward references:
static void busy(BOOL on);
static BOOL getFileName(HWND parentWin, char *fileName, size_t fileNameSz);
static void onCommand(HWND win, int id, HWND ctrlWin, UINT codeNotify);
static BOOL onCreate(HWND win, LPCREATESTRUCT lpCreateStruct);
static void onDestroy(HWND hwnd);
static void onSetFocus(HWND win, HWND hwndOldFocus);
static void onSize(HWND hwnd, UINT state, int cx, int cy);
static void openFile(HWND mainWin, const char *fileName);
static void checkText(HWND mainWin, BOOL selectedOnly);
static void checkTextAlt(HWND mainWin);
static void saveFile(HWND mainWin, const char *fileName);

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE prevInstance, LPSTR cmdLine,
  int cmdShow) {
	MSG msg;
	HWND win;

#if defined(_WIN32)
	LoadLibrary(RICHEDIT_DLL_NAME);
#endif

	if (prevInstance == 0) {
		WNDCLASS winClass;

		winClass.style = CS_HREDRAW | CS_VREDRAW;
		winClass.lpfnWndProc = (WNDPROC)WinProc;
		winClass.cbClsExtra = 0;
		winClass.cbWndExtra = 0;
		winClass.hInstance = hInst;
		winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		winClass.hIcon = LoadIcon(hInst, appName);
		winClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		winClass.lpszMenuName = appName;
		winClass.lpszClassName = appName;

		RegisterClass(&winClass);
	}

	instance = hInst;
	// Create a window 3/4 wide, 1/2 high, centered on the screen.
	win = CreateWindow(appName, APP_TITLE, WS_OVERLAPPEDWINDOW,
	  GetSystemMetrics(SM_CXSCREEN) / 8, GetSystemMetrics(SM_CYSCREEN) / 4,
	  (GetSystemMetrics(SM_CXSCREEN) * 3) / 4, GetSystemMetrics(SM_CYSCREEN) / 2,
	  NULL, NULL, instance, cmdLine);
	ShowWindow(win, cmdShow);
	UpdateWindow(win);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (msg.wParam);
}

// Respond to Windows messages.
long FAR PASCAL WinProc(HWND win, WORD msgType, WPARAM wParam, LPARAM lParam) {
	switch (msgType) {
		HANDLE_MSG(win, WM_CREATE, onCreate);
		HANDLE_MSG(win, WM_SETFOCUS, onSetFocus);
		HANDLE_MSG(win, WM_SIZE, onSize);
		HANDLE_MSG(win, WM_COMMAND, onCommand);
		HANDLE_MSG(win, WM_DESTROY, onDestroy);
		default:
			return (DefWindowProc(win, msgType, wParam, lParam));
	}
}

// Indicates to the user whether we're busy (or not).
static void busy(BOOL on) {
	static HCURSOR origCursor;

	if (on) {
		origCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
		ShowCursor(TRUE);
	}
	else {
    	ShowCursor(FALSE);
		SetCursor(origCursor);
	}
}

// Interact with the user to select the name of a file to check. Returns TRUE
// if the user actually selected a file.
static BOOL getFileName(HWND parentWin, char *fileName, size_t fileNameSz) {
	char filePath[MAX_PATH] = {'\0'};
	OPENFILENAME openFileInfo;
	const char *filters = "Text Files (*.TXT)\0*.txt\0\0All Files (*.*)\0*.*\0";

	*fileName = '\0';

	// Present the "open file" common dialog.
	openFileInfo.lStructSize = sizeof(openFileInfo);
	openFileInfo.hwndOwner = parentWin;
	openFileInfo.hInstance = 0;
	openFileInfo.lpstrFilter = (LPSTR)filters;
	openFileInfo.lpstrCustomFilter = NULL;
	openFileInfo.nMaxCustFilter = 0;
	openFileInfo.nFilterIndex = 0;
	openFileInfo.lpstrFile = (LPSTR)fileName;
	openFileInfo.nMaxFile = fileNameSz;
	openFileInfo.lpstrFileTitle = NULL;
	openFileInfo.nMaxFileTitle = 0;
	openFileInfo.lpstrInitialDir = (LPSTR)filePath;
	openFileInfo.lpstrTitle = "Open File";
	openFileInfo.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
	openFileInfo.nFileOffset = 0;
	openFileInfo.nFileExtension = 0;
	openFileInfo.lpstrDefExt = 0;
	openFileInfo.lCustData = 0;
	openFileInfo.lpfnHook = NULL;
	openFileInfo.lpTemplateName = NULL;

	return (GetOpenFileName(&openFileInfo));
}

// Respond to a WM_COMMAND message.
static void onCommand(HWND win, int id, HWND ctrlWin, UINT codeNotify) {
    char verStr[20];
    char msg[256 + 1];
    static char fileName[MAX_PATH] = {'\0'};

	switch (id) {
	case IDM_FILE_OPEN:
		if (getFileName(win, fileName, sizeof(fileName))) {
        	openFile(win, fileName);
		}
        break;
	case IDM_FILE_SAVE:
		if (fileName[0] != '\0') {
			saveFile(win, fileName);
		}
        break;
    case IDM_FILE_SPELLING:
    	checkText(win, FALSE);
    	break;
	case IDM_FILE_SPELLING_SELECTED:
		checkText(win, TRUE);
		break;
    case IDM_FILE_SPELLING_ALT:
    	checkTextAlt(win);
    	break;
	case IDM_FILE_EXIT:
		PostQuitMessage(0);
        break;
	case IDM_OPTIONS_PREFERENCES:
		SSCE_OptionsDlg(win);
		break;
	case IDM_LEXICONS_EDIT:
		SSCE_EditLexDlg(win);
        break;
	case IDM_HELP_ABOUT:
		SSCE_Version(verStr, sizeof(verStr));
		wsprintf(msg, "Sentry Spelling-Checker Engine Version %s\r\n"
		  "Copyright © 1994-2000 Wintertree Software Inc.\r\n"
		  "http://www.wintertree-software.com", verStr);
		MessageBox(win, msg, APP_TITLE, MB_OK);
		break;
	case IDM_HELP_WEB_SITE:
#if defined(_WIN32)
	    ShellExecute(NULL, "open", "http://www.wintertree-software.com", NULL,
		  NULL, SW_SHOWNORMAL);
#else
		MessageBox(win, "Visit\r\nhttp://www.wintertree-software.com",
		  APP_TITLE, MB_OK);
#endif
		break;
	}
}

// Respond to a WM_CREATE message.
static BOOL onCreate(HWND win, LPCREATESTRUCT lpCreateStruct) {
	HINSTANCE instance = lpCreateStruct->hInstance;

	SSCE_SetKey(licenseKey);

#if defined(_WIN32)
	textHWnd = CreateWindow(RICHEDIT_CLASS_NAME, NULL, WS_CHILD | WS_VISIBLE |
	  WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE |
	  ES_AUTOVSCROLL | ES_NOHIDESEL, 0, 0, 0, 0, win, NULL, instance,
	  NULL);
	assert(textHWnd != NULL);
	SendMessage(textHWnd, EM_SETEVENTMASK, 0,
	  (LPARAM)(ENM_SELCHANGE | ENM_CHANGE | ENM_MOUSEEVENTS));
	SendMessage(textHWnd, EM_EXLIMITTEXT, 0, (LPARAM)0xffffff);
	struct _charformat cf;
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_COLOR | CFM_SIZE | CFM_FACE | CFM_BOLD | CFM_ITALIC |
	  CFM_STRIKEOUT | CFM_UNDERLINE;
	cf.dwEffects = 0;
	cf.yHeight = 12 * 20;
	cf.crTextColor = GetSysColor(COLOR_WINDOWTEXT);
	lstrcpy(cf.szFaceName, "Arial");
	SendMessage(textHWnd, EM_SETCHARFORMAT, (WPARAM)0, (LPARAM)&cf);

	// Check spelling of the rich-edit control in the background.
	SSCE_CheckCtrlBackground(textHWnd);

#else	// _WIN16
	textHWnd = CreateWindow("edit", NULL, WS_CHILD | WS_VISIBLE |
	  WS_VSCROLL | WS_BORDER | ES_LEFT | ES_MULTILINE |
	  ES_AUTOVSCROLL | ES_NOHIDESEL, 0, 0, 0, 0, win, (HMENU)IDC_TEXT, instance,
	  NULL);
#endif
	SetWindowText(win, APP_TITLE);

	return (TRUE);
}

// Respond to a WM_DESTROY message.
static void onDestroy(HWND hwnd) {
	PostQuitMessage(0);
}

// Respond to a WM_SETFOCUS message.
static void onSetFocus(HWND win, HWND hwndOldFocus) {
	SetFocus(textHWnd);
}

// Respond to a WM_SIZE message.
static void onSize(HWND hwnd, UINT state, int cx, int cy) {
	MoveWindow(textHWnd, 0, 0, cx, cy, TRUE);
}

// Load a text file into the edit control.
static void openFile(HWND mainWin, const char *fileName) {
	char *fileBfr = NULL;
	char winTitle[80 + 1];
	FILE *fp;
	int len;
	size_t bufLen;
	
	if ((fp = fopen(fileName, "rb")) == NULL) {
		MessageBox(mainWin, "Can't open file", fileName, MB_OK);
		return;
	}

#if !defined(_WIN32)	
	/* Make sure the file isn't too big. */
	if (filelength(fileno(fp)) > MAX_FILE_SIZE) {
		MessageBox(mainWin, "File is too large.", fileName, MB_OK);
		fclose(fp);
		return;
	}
#endif

	bufLen = (size_t)filelength(fileno(fp)) + 1;
	if ((fileBfr = (char *)malloc(bufLen)) == NULL) {
		MessageBox(mainWin, "Out of memory.", APP_NAME, MB_OK);
		fclose(fp);
		return;
	}
	
	/* Load the file's contents into the buffer. */
	len = fread(fileBfr, sizeof(char), bufLen, fp);
	fileBfr[len] = '\0';
	
	/* Present the file's contents in the edit control. */
	Edit_SetText(textHWnd, fileBfr);
	                      
	wsprintf(winTitle, "%s - %s", APP_TITLE, fileName);	                      
	SetWindowText(mainWin, winTitle);

	free(fileBfr);	
	fclose(fp);
}

// Check the spelling of words contained within the edit control.
static void checkText(HWND mainWin, BOOL selectedOnly) {
	SSCE_CheckCtrlDlg(mainWin, textHWnd, (SSCE_S16)selectedOnly);
}

// Check the spelling of words contained within the edit control, using
// alternative dialogs.
static void checkTextAlt(HWND mainWin) {
	SSCE_CheckCtrlDlgTmplt(mainWin, textHWnd, FALSE, instance,
	  "CheckSpellingDlg", "EditLexDlg", "OptionsDlg", "NewLexDlg");
}

// Save the context of the edit control to a text file.
static void saveFile(HWND mainWin, const char *fileName) {
	char *fileBfr;
	int textLen;
	FILE *fp;

	textLen = GetWindowTextLength(textHWnd);
	if ((fileBfr = (char *)malloc(textLen + 1)) == NULL) {
		MessageBox(mainWin, "Out of memory.", APP_NAME, MB_OK);
		return;
	}

	GetWindowText(textHWnd, fileBfr, textLen + 1);
	
	/* Save the contents to disk. */
	if ((fp = fopen(fileName, "wb")) == NULL) {	
		MessageBox(mainWin, "Can't open file", fileName, MB_OK);
		free(fileBfr);
		return;
	}

	fwrite(fileBfr, sizeof(char), textLen, fp);
		
	fclose(fp);
	free(fileBfr);
}

