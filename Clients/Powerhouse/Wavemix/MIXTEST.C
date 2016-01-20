/****************************************************************************

    PROGRAM: MixTest

****************************************************************************/

/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993, 1994  Microsoft Corporation.  All Rights Reserved.
 *
 ***************************************************************************/

#include <windows.h>		    /* required for all Windows applications */
#include <windowsx.h>		 /* for message crackers */
#include <mmsystem.h>
#include <stdlib.h>
#include <memory.h>
#include "wavemix.h"
#include "mixtest.h"
#include "resource.h"

/************************* GLOBAL VARIABLES ***********************************/
HANDLE ghInst;
HWND ghWnd;
char gszAppName[] = "MixTest";

HANDLE ghMixSession;
LPMIXWAVE glpMix1;
LPMIXWAVE glpMix2;
LPMIXWAVE glpMix3;
LPMIXWAVE glpMix4;
LPMIXWAVE glpMix5;
LPMIXWAVE glpMix6;
LPMIXWAVE glpMix7;

BOOL gfRepeatPlay=FALSE;
BOOL gfDebug=FALSE;

HMENU ghMenu1,ghMenu2;
BOOL gfAuto=FALSE;

/************************* GLOBAL VARIABLES ***********************************/


/****************************** start helper functions ************************/

void ODSN(LPSTR sz)
{
	if (!gfDebug)
		return;
	OutputDebugString(sz);
	OutputDebugString("\n\r");
}

void UserMessage (LPSTR szMsg)
{
	MessageBox(ghWnd,szMsg,gszAppName,MB_OK|MB_ICONINFORMATION);
}

void NiceWait(void)
{
	MSG msg;

    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage (&msg);
	}
}

void CloseWaveStuff(void)
{
	if (!ghMixSession)
		return;
	WaveMixCloseChannel(ghMixSession,0,WMIX_ALL);

	if (glpMix1) WaveMixFreeWave(ghMixSession,glpMix1);
	if (glpMix2) WaveMixFreeWave(ghMixSession,glpMix2);
	if (glpMix3) WaveMixFreeWave(ghMixSession,glpMix3);
	if (glpMix4) WaveMixFreeWave(ghMixSession,glpMix4);
	if (glpMix5) WaveMixFreeWave(ghMixSession,glpMix5);
	if (glpMix6) WaveMixFreeWave(ghMixSession,glpMix6);
	if (glpMix7) WaveMixFreeWave(ghMixSession,glpMix7);

	glpMix1 = glpMix2 = glpMix3 = glpMix4 = glpMix5 = glpMix6 = glpMix7 = NULL;

	WaveMixCloseSession(ghMixSession);
	ghMixSession=NULL;
}

/*
	This function loads a resource into memory and then passes it to WaveMixOpenWave
	as a memory file to demonstrate how to open a memory file.  If you had wanted to
	just open the resource you would just call:
		  WaveMixOpenWave(ghMixSession,"WaveResource",ghInst,WMIX_RESOURCE);
*/
LPMIXWAVE OpenMemoryFile(HANDLE hMixSession)
{
	HRSRC	hRsrc = NULL;
	HGLOBAL hMem = NULL;
	HPSTR	lp = NULL;
	LPMIXWAVE lpWav=NULL;
	MMIOINFO mmioInfo;

	// Note:  the memory buffer needs to be read-write for
	//        this operation to succeed.  Since I am just
	//        loading a resource and using  that as my memory
	//        file I need to mark the resource as writeable when
	//        I link the program.  If you use GlobalAlloc and copy
	//        your data into it, you should be fine.
	//        
	hRsrc = FindResource(ghInst,"OneWave","WAVE");
	if (!hRsrc)
		goto cleanup;
	hMem = LoadResource(ghInst, hRsrc);
	if (!hMem)
		goto cleanup;

	lp=LockResource(hMem);
	if (!lp)
		goto cleanup;

	_fmemset((LPSTR)&mmioInfo,0,sizeof(MMIOINFO));
	mmioInfo.pchBuffer=lp;
#if defined(WIN32)
	mmioInfo.cchBuffer=SizeofResource(ghInst,hRsrc);
#else
	mmioInfo.cchBuffer=GlobalSize(hMem);
#endif
	mmioInfo.fccIOProc=FOURCC_MEM;
	mmioInfo.adwInfo[0]=(DWORD)NULL;

	lpWav = WaveMixOpenWave(hMixSession,(LPSTR)&mmioInfo,ghInst,WMIX_MEMORY);

cleanup:
	if (lp)
		UnlockResource(hMem);
	if (hMem)
		FreeResource(hMem);
	return lpWav;
}

/****************************** end helper functions **************************/

/****************************** start Methods *********************************/

void MixTest_OnPaint(HWND hWnd)
{
	HDC hDC;
	PAINTSTRUCT ps;

	ODSN("MixTest_OnPaint");

	hDC = BeginPaint(hWnd,&ps);

	SetTextColor(hDC,RGB(255,255,255));
	SetBkColor(hDC,0);
	TextOut(hDC,10,10,"Press 1,2,3,4 or mouse keys",27);

	EndPaint(hWnd,&ps);
}

void MixTest_OnKey(HWND hWnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	MIXPLAYPARAMS MixPlayParams;

	ODSN("MixTest_OnKey");

	if (!fDown)
		return;

	MixPlayParams.wSize = sizeof(MIXPLAYPARAMS);
	MixPlayParams.hMixSession = ghMixSession;
	MixPlayParams.hWndNotify=NULL;
	MixPlayParams.dwFlags=WMIX_CLEARQUEUE|WMIX_HIPRIORITY;
	MixPlayParams.wLoops=0;

	switch(vk)
	{
	case '1':
		MixPlayParams.iChannel=1;
		MixPlayParams.lpMixWave=glpMix1;
		break;
	case '2':
		MixPlayParams.iChannel=2;
		MixPlayParams.lpMixWave=glpMix2;
		break;
	case '3':
		MixPlayParams.iChannel=3;
//		MixPlayParams.dwFlags=WMIX_HIPRIORITY;
		MixPlayParams.lpMixWave=glpMix3;
		break;
	case '4':
		MixPlayParams.iChannel=4;
		MixPlayParams.lpMixWave=glpMix4;
		break;
	default:
		if (GetKeyState(VK_CAPITAL)<0)
		{
			gfRepeatPlay=TRUE;
			MixPlayParams.iChannel=7;
			MixPlayParams.lpMixWave=glpMix7;
			MixPlayParams.hWndNotify=hWnd;
		}
		else
		{
			gfRepeatPlay=FALSE;
			return;
		}
		break;
	}

	WaveMixPlay(&MixPlayParams);
}


BOOL MixTest_OnCreate(HWND hWnd, CREATESTRUCT FAR* lpCreateStruct)
{
	BOOL fErr=FALSE;
	WAVEMIXINFO Info;
	MIXCONFIG config;

	ODSN("MixTest_OnCreate");

	ghMenu1 = LoadMenu(ghInst,"MixTestMenu1");
	ghMenu2 = LoadMenu(ghInst,"MixTestMenu2");
	SetMenu(hWnd,ghMenu1);
	srand(gfDebug ? 1 : LOWORD(GetTickCount()));	// 1 reintializes, Any other number sets random

	Info.wSize=sizeof(WAVEMIXINFO);
	if (WaveMixGetInfo(&Info))
		return FALSE;

	config.wSize = sizeof(MIXCONFIG);
	config.dwFlags = WMIX_CONFIG_CHANNELS;
	config.wChannels = 2;  // give us stereo!
	if (!(ghMixSession = WaveMixConfigureInit(&config)))
		return FALSE;

	glpMix1=OpenMemoryFile(ghMixSession);
	// note: under windows NT the resource must be marked as a Read-Write
	//       resource or mmioOpen will fail in WaveMixOpenWave.  You can
	//       do this by specifing -section:.rsrc,rw on the link command line. (ms linker)
	glpMix2=WaveMixOpenWave(ghMixSession,"TwoWave",ghInst,WMIX_RESOURCE);
	glpMix3=WaveMixOpenWave(ghMixSession,"3.wav",NULL,0);
	glpMix4=WaveMixOpenWave(ghMixSession,"4.wav",NULL,0);
	glpMix5=WaveMixOpenWave(ghMixSession,"5.wav",NULL,0);
	glpMix6=WaveMixOpenWave(ghMixSession,"6.wav",NULL,0);
	glpMix7=WaveMixOpenWave(ghMixSession,"7.wav",NULL,0);

	fErr=WaveMixOpenChannel(ghMixSession,8,WMIX_OPENCOUNT);

	if (fErr || !(glpMix1 && glpMix2 && glpMix3 && glpMix4 && glpMix5 && glpMix6 && glpMix7))
	{
		CloseWaveStuff();
		return FALSE;
	}

	return TRUE;
}

void MixTest_OnActivate(HWND hwnd, UINT state, HWND hwndActDeact, BOOL fMinimized)
{
	BOOL fActivate = (state != WA_INACTIVE && !fMinimized);
	WaveMixActivate(ghMixSession, fActivate);
}

void MixTest_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	MIXPLAYPARAMS MixPlayParams;

	MixPlayParams.wSize = sizeof(MIXPLAYPARAMS);
	MixPlayParams.hMixSession = ghMixSession;
	MixPlayParams.hWndNotify=NULL;
	MixPlayParams.dwFlags=WMIX_CLEARQUEUE|WMIX_HIPRIORITY;
	MixPlayParams.wLoops=0;
	MixPlayParams.iChannel=5;
	MixPlayParams.lpMixWave=glpMix5;

	WaveMixFlushChannel(ghMixSession,6,WMIX_NOREMIX);
	WaveMixPlay(&MixPlayParams);
}

void MixTest_OnTimer(HWND hwnd, UINT id)
{
	static LPMIXWAVE * waves[7]={&glpMix1,&glpMix2,&glpMix3,&glpMix4,&glpMix5,&glpMix6,&glpMix7};
	int i;
	MIXPLAYPARAMS MixPlayParams;

	i = 1+rand()%7;
	

	MixPlayParams.wSize = sizeof(MIXPLAYPARAMS);
	MixPlayParams.hMixSession = ghMixSession;
	MixPlayParams.hWndNotify=NULL;
	MixPlayParams.dwFlags=WMIX_CLEARQUEUE|WMIX_HIPRIORITY;
	MixPlayParams.wLoops=0;
	MixPlayParams.iChannel=i;
	MixPlayParams.lpMixWave=*waves[i-1];

	WaveMixPlay(&MixPlayParams);
}


void MixTest_OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	MIXPLAYPARAMS MixPlayParams;

	MixPlayParams.wSize = sizeof(MIXPLAYPARAMS);
	MixPlayParams.hMixSession = ghMixSession;
	MixPlayParams.hWndNotify=NULL;
	MixPlayParams.dwFlags=WMIX_CLEARQUEUE|WMIX_HIPRIORITY|WMIX_WAIT;
	MixPlayParams.wLoops=15;
	MixPlayParams.iChannel=6;
	MixPlayParams.lpMixWave=glpMix6;

	WaveMixPlay(&MixPlayParams);  // note WMIX_WAIT is set

	MixPlayParams.iChannel=7;
	MixPlayParams.wLoops=3;
	MixPlayParams.lpMixWave=glpMix7;
	WaveMixPlay(&MixPlayParams);  // note WMIX_WAIT is set

	MixPlayParams.iChannel=1;
	MixPlayParams.lpMixWave=glpMix1;
	WaveMixPlay(&MixPlayParams);  // note WMIX_WAIT is set

	MixPlayParams.iChannel=3;
	MixPlayParams.wLoops=0;
	MixPlayParams.lpMixWave=glpMix3;
	MixPlayParams.dwFlags=WMIX_CLEARQUEUE|WMIX_HIPRIORITY; // note WMIX_WAIT is cleared so playing can start
	WaveMixPlay(&MixPlayParams);
}

void MixTest_OnMM_WOM_DONE(HWND hWnd, int iChannel, LPMIXWAVE lpMixWave)
{
	MIXPLAYPARAMS MixPlayParams;

	if (!gfRepeatPlay)
		return;

	MixPlayParams.wSize = sizeof(MIXPLAYPARAMS);
	MixPlayParams.hMixSession = ghMixSession;
	MixPlayParams.hWndNotify=hWnd;
	MixPlayParams.dwFlags=WMIX_QUEUEWAVE;
	MixPlayParams.wLoops=0;
	MixPlayParams.iChannel=iChannel;
	MixPlayParams.lpMixWave=lpMixWave;

	WaveMixPlay(&MixPlayParams);
}

void MixTest_OnDestroy(HWND hWnd)
{
	ODSN("MixTest_OnDestroy");

	CloseWaveStuff();

	if (gfAuto)
		KillTimer(hWnd,1);

	DestroyMenu(ghMenu1);
	DestroyMenu(ghMenu2);

	PostQuitMessage(0);
}


void MixTest_OnCommand(HWND hWnd, int id, HWND hwndCtl, UINT codeNotify)
{
	ODSN("MixTest_OnCommand");

	switch (id)
	{
	case IDM_AUTO:
		gfAuto = !gfAuto;
		if (gfAuto)
		{
			SetMenu(hWnd,ghMenu2);
			SetTimer(hWnd,1,100,NULL);
		}
		else
		{
			KillTimer(hWnd,1);
			SetMenu(hWnd,ghMenu1);
		}
		break;
	case IDM_ABOUT:
		DialogBox(ghInst,"AboutBox",hWnd,AboutDlgProc);
		break;
	default:
		FORWARD_WM_COMMAND(hWnd, id, hwndCtl, codeNotify, DefWindowProc);
	}	
}

/****************************** end MixTest_ Methods ******************************************/

/****************************** start AboutMixTest_ Methods ***********************************/

BOOL AboutMixTest_OnInitDialog(HWND hDlg, HWND hwndFocus, LPARAM lParam)
{
	return TRUE;
}

BOOL AboutMixTest_OnCommand(HWND hDlg, int id, HWND hwndCtl, UINT codeNotify)
{
	switch (id)
	{
	case IDOK:
	case IDCANCEL:
		EndDialog(hDlg, TRUE); /* Exits the dialog box	      */
		return TRUE;
	}
	return FALSE;
}

/****************************** end AboutMixTest_ Methods ***********************************/

/************************ start Program Initialization Functions **************************/


BOOL InitApplication(HINSTANCE hInstance)
{
    WNDCLASS  wc;

    wc.style 			= CS_BYTEALIGNWINDOW;
    wc.lpfnWndProc 		= MixTestWndProc;
    wc.cbClsExtra 		= 0;
    wc.cbWndExtra 		= 0;
    wc.hInstance  		= hInstance;
    wc.hIcon      		= LoadIcon(hInstance, "MixTest");
    wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground	= GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName 	= NULL; // "MixTestMenu";
    wc.lpszClassName 	= gszAppName;

    return (RegisterClass(&wc));
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	ghInst = hInstance;

	ghWnd = CreateWindow(
			/* address of registered class name		*/	gszAppName,
			/* address of window text				*/	gszAppName,
			/* window style							*/	WS_OVERLAPPEDWINDOW,
			/* horizontal position of window		*/	CW_USEDEFAULT,
			/* vertical position of window			*/	CW_USEDEFAULT,
			/* window width							*/	CW_USEDEFAULT,
			/* window height						*/	CW_USEDEFAULT,
			/* handle of parent window				*/	NULL,
			/* handle of menu or child-window id	*/	NULL,
			/* handle of application instance		*/	hInstance,
			/* address of window-creation data		*/	NULL);
	
	if (!ghWnd)
		return (FALSE);
	
	/* Make the window visible; update its client area; and return "success" */
	
	ShowWindow(ghWnd, nCmdShow);  /* Show the window                        */
	UpdateWindow(ghWnd);          /* Sends WM_PAINT message                 */

	return (TRUE);
}

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	if (!hPrevInstance)
		if (!InitApplication(hInstance))
	return (FALSE);
	
	if (!InitInstance(hInstance, nCmdShow))
		return (FALSE);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

/************************ end Program Initialization Functions **************************/

/************************ start exported window Procedures ******************************/

BOOL EXPORT CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return (BOOL)HANDLE_WM_INITDIALOG(hDlg, wParam, lParam, AboutMixTest_OnInitDialog);
	
	case WM_COMMAND:
		HANDLE_WM_COMMAND(hDlg,	wParam, lParam, AboutMixTest_OnCommand);
		return TRUE;
	}
	return (FALSE);			      /* Didn't process a message    */
}

LRESULT EXPORT CALLBACK MixTestWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	    HANDLE_MSG (hWnd, WM_CREATE,		MixTest_OnCreate);
	    HANDLE_MSG (hWnd, WM_DESTROY,		MixTest_OnDestroy);
	    HANDLE_MSG (hWnd, WM_PAINT,			MixTest_OnPaint);
	    HANDLE_MSG (hWnd, WM_COMMAND,		MixTest_OnCommand);
		HANDLE_MSG (hWnd, WM_KEYDOWN,		MixTest_OnKey);
		HANDLE_MSG (hWnd, WM_LBUTTONDOWN,	MixTest_OnLButtonDown);
		HANDLE_MSG (hWnd, WM_RBUTTONDOWN,	MixTest_OnRButtonDown);
		HANDLE_MSG (hWnd, MM_WOM_DONE,		MixTest_OnMM_WOM_DONE);
		HANDLE_MSG (hWnd, WM_ACTIVATE,		MixTest_OnActivate);
		HANDLE_MSG (hWnd, WM_TIMER,			MixTest_OnTimer);
	default:
	    return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

/************************ end exported window Procedures ******************************/

