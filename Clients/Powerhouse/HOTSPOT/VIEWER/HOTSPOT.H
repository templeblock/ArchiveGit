/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/
#ifndef HOTSPOT_H
#define HOTSPOT_H

typedef struct _HOTSPOT
{
    struct _HOTSPOT FAR *pNext;
    struct _HOTSPOT FAR *pPrev;
    
    RECT rc;
    LPSTR pszCommand;
    LPSTR pszHotspotID;
    DWORD BeginFrame;
    DWORD EndFrame;
    int OnClick;
    int ToFrame;                // if OnClick == ID_JUMP, ToFrame should be set
    
} HOTSPOT, FAR *PHOTSPOT;

typedef struct _MOVIEINFO       // associate this with a window
{
    PHOTSPOT pHotspotList;
    
    WORD wMCIDeviceID;
    HWND hwndMovie;
    HWND hwndParent;            // set this before OPEN
    RECT rcMovie;
    BOOL fPlaying;
    BOOL fMovieOpen;
    
    DWORD dwMovieLength;
    DWORD dwCurrentFrame;
    FARPROC     lpfnOldProc;                // far ptr to old proc before subclass
    FARPROC     lpfnhspAVICallback;
    
    char szAVIFileName[129];
    char szIniFileName[129];    
    
} MOVIEINFO, FAR *PMOVIEINFO;

/* typedef struct
{
    HWND   hwndViewer;
} VWRINFO, FAR *LPVWRINFO; */

/*
 * MVMCI keeps a linked list of all currently opened ViewerMCI windows.
 * This is used to allow linking of hotspots to control media elements,
 * either opened by another hotspot or in an embedded window.
 * This list is also used in printing.
 */
/* typedef struct _WindowNode {		// Node for list of ViewerMCI windows
	HANDLE		hTask;							// Task that opened it
	HWND		hwndParent;						// Parent window handle
	HWND		hwndViewerMCI;					// Handle to the ViewerMCI
	BOOL		fEmbedded;						// TRUE=embedded window
	HANDLE		hNext;							// Next node in list
} WindowNode, FAR * PWindowNode; */

//extern HANDLE	ghWindowList;	// List of window nodes

HANDLE __hMem;

#define ALLOCATE(s) ((__hMem = GlobalAlloc(GHND, (DWORD)(s))) ? GlobalLock(__hMem) : NULL)
#define FREE(p) (GlobalUnlock((HANDLE)HIWORD(p)),GlobalFree((HANDLE)HIWORD(p)))
#define SIZE(p) (GlobalSize((HANDLE)HIWORD(p)))

#define USHORT WORD

LONG CALLBACK LDLLHandler(UINT msg, LONG lParam1, LONG lParam2);
BOOL CALLBACK LibMain(HANDLE hModule, int cbHeap, LPSTR lpchCmdLine);
LPSTR FindToken(LPSTR psz, char c);
int StrToInt(LPSTR lpstr);
BOOL InitHotspots(PMOVIEINFO pMovieInfo, LPSTR szIniFile);
void DeleteHotspot(PMOVIEINFO pMovieInfo, PHOTSPOT pHotspot);
void DeleteHotspotList(PMOVIEINFO pMovieInfo);
void AddHotspot(PMOVIEINFO pMovieInfo, PHOTSPOT pHotspot);
LONG CALLBACK __export hspPlayAVI(HWND hwnd, LPSTR szAVIFile, LPSTR szIniFile, FARPROC);
LONG CALLBACK __export SbClsProc (HWND hwnd, USHORT msg, USHORT wp, LONG lp);

///// avi.c --
BOOL initAVI(void);
void termAVI(void);

///// movie.c --
void fileOpenMovie(PMOVIEINFO pMovieInfo, LPSTR szFilename);
void fileCloseMovie(PMOVIEINFO pMovieInfo, BOOL bUpdate);
VOID positionMovie(PMOVIEINFO pMovieInfo);
void playMovie(PMOVIEINFO pMovieInfo, WORD wDirection);
void seekMovie(PMOVIEINFO pMovieInfo, WORD wAction);
void stepMovie(PMOVIEINFO pMovieInfo, HWND hWnd, WORD wDirection);
DWORD GetMovieFrame(PMOVIEINFO pMovieInfo);
DWORD GetMovieLength(PMOVIEINFO pMovieInfo);
void setMovie(PMOVIEINFO pMovieInfo, DWORD dwFrame, HWND hCallback);
int OnButtonDown(PMOVIEINFO pMovieInfo, USHORT msg, USHORT wp, LONG lp);

#define ID_HOME     0
#define ID_END      1

#define ID_CONTINUE                     1017        // defined in ..\avihed\resource.h
#define ID_STOP                         1018        //
#define ID_JUMP                         1019        //

#define Message(xx) OutputDebugString(xx); OutputDebugString("\n"); MessageBox(GetFocus(),xx,"Hotspot Message:",MB_OK)
#define Messages(yy,xx) OutputDebugString(yy); OutputDebugString(": "); OutputDebugString(xx); OutputDebugString("\n"); MessageBox(GetFocus(),xx,yy,MB_OK)

//#define Message(xx) OutputDebugString(xx); OutputDebugString("\n")
//#define Messages(yy,xx) OutputDebugString(yy); OutputDebugString(": "); OutputDebugString(xx); OutputDebugString("\n")

void cdecl DbgOut(LPSTR lpFormat, ...); 
#define dprint(x) DbgOut x
#define dbg(x) DbgOut(x)

#define szDrawPropName szMovieInfo
extern char szMovieInfo[];

#endif
