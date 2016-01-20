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
    
    LONG lLastSeek;    
    
} MOVIEINFO, FAR *PMOVIEINFO;


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
#ifdef VIEWER
 LONG CALLBACK __export hspPlayAVI(HWND hwnd, LPSTR  szAVIFile, LPSTR szIniFile);
#endif
#ifdef AVIHAPP
 LONG CALLBACK __export hspPlayAVI(HWND hwnd, LPSTR szAVIFile, LPSTR szIniFile, FARPROC);
#endif
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

#define Message(xx) MessageBox(GetFocus(),xx,"Hotspot Editor",MB_OK)

#define szDrawPropName szMovieInfo
extern char szMovieInfo[];

