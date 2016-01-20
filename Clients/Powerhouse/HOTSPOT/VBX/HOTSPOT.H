#ifndef VB_VERSION
  #include <vbapi.h>
#define VBAPI FAR PASCAL
#endif  


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
    char szHotFileName[129];    
    
    HCTL hctl;
    BOOL bLoading;
    
    BOOL bShowHot;
    LONG lLastSeek;
    
} MOVIEINFO, FAR *PMOVIEINFO;

#ifdef VB_VERSION
typedef struct _CTLINFO         // only used for passing info, not storing!
{                               // use MOVIEINFO for storage.

    PMOVIEINFO pMovieInfo;          // currently unused...
    HSZ hszAVIFile;
    HSZ hszHotFile;
    SHORT fPlay;
    LONG  lFrame;
    LONG  lLength;
    ENUM  nShowHot;
} CTLINFO, FAR *PCTLINFO;
#endif

HANDLE __hMem;

#define ALLOCATE(s) ((__hMem = GlobalAlloc(GHND, (DWORD)(s))) ? GlobalLock(__hMem) : NULL)
#define FREE(p) (GlobalUnlock((HANDLE)HIWORD(p)),GlobalFree((HANDLE)HIWORD(p)))
#define SIZE(p) (GlobalSize((HANDLE)HIWORD(p)))

#define USHORT WORD

LONG CALLBACK LDLLHandler(UINT msg, LONG lParam1, LONG lParam2);
//BOOL CALLBACK LibMain(HANDLE hModule, int cbHeap, LPSTR lpchCmdLine);
LPSTR FindToken(LPSTR psz, char c);
int StrToInt(LPSTR lpstr);
BOOL InitHotspots(PMOVIEINFO pMovieInfo, LPSTR szHotFile);
void DeleteHotspot(PMOVIEINFO pMovieInfo, PHOTSPOT pHotspot);
void DeleteHotspotList(PMOVIEINFO pMovieInfo);
void AddHotspot(PMOVIEINFO pMovieInfo, PHOTSPOT pHotspot);
#ifdef VIEWER
 LONG CALLBACK __export hspPlayAVI(HWND hwnd, LPSTR  szAVIFile, LPSTR szHotFile);
#endif
#ifdef AVIHAPP
 LONG CALLBACK __export hspPlayAVI(HWND hwnd, LPSTR szAVIFile, LPSTR szHotFile, FARPROC);
#endif
LONG CALLBACK __export SbClsProc (HWND hwnd, USHORT msg, USHORT wp, LONG lp);

///// avi.c --
BOOL initAVI(void);
void termAVI(void);

///// movie.c --
void fileOpenMovie(PMOVIEINFO pMovieInfo, LPSTR szFilename, SHORT x, SHORT y);
void fileCloseMovie(PMOVIEINFO pMovieInfo, BOOL bUpdate);
VOID positionMovie(PMOVIEINFO pMovieInfo, SHORT x, SHORT y);
void playMovie(PMOVIEINFO pMovieInfo, WORD wDirection);
void seekMovie(PMOVIEINFO pMovieInfo, WORD wAction);
void stepMovie(PMOVIEINFO pMovieInfo, HWND hWnd, WORD wDirection);
DWORD GetMovieFrame(PMOVIEINFO pMovieInfo);
DWORD GetMovieLength(PMOVIEINFO pMovieInfo);
void setMovie(PMOVIEINFO pMovieInfo, DWORD dwFrame, HWND hCallback);
int OnButtonDown(PMOVIEINFO pMovieInfo, USHORT msg, USHORT wp, LONG lp);

#ifdef DEBUG

    DWORD   __dwEval;
    int __iDebugLevel;

    extern void SetDebugLevel(int i);
    extern void __AssertMsg(LPSTR exp ,LPSTR file, int line);
    extern void cdecl DbgOut(LPSTR lpFormat, ...);

    #define ASSERT(exp) \
        ((exp) ? (void)0 : __AssertMsg((LPSTR)(#exp), (LPSTR)__FILE__, __LINE__))
    #define EVAL(exp) \
        (__dwEval=(DWORD)(exp), (__dwEval) ? (void)0 : __AssertMsg((LPSTR)(#exp), (LPSTR)__FILE__, __LINE__), __dwEval)

    #define dprintf                          DbgOut
    #define dprintf1 if (__iDebugLevel >= 1) DbgOut
    #define dprintf2 if (__iDebugLevel >= 2) DbgOut
    #define dprintf3 if (__iDebugLevel >= 3) DbgOut
    #define dprintf4 if (__iDebugLevel >= 4) DbgOut

#else // not DEBUG

    #define ASSERT(exp) 0
    #define EVAL(exp) (exp)

    #define dprintf  if (0) ((int (*)(char *, ...)) 0)
    #define dprintf1 if (0) ((int (*)(char *, ...)) 0)
    #define dprintf2 if (0) ((int (*)(char *, ...)) 0)
    #define dprintf3 if (0) ((int (*)(char *, ...)) 0)
    #define dprintf4 if (0) ((int (*)(char *, ...)) 0)

#endif // DEBUG


    
#define ID_HOME     0
#define ID_END      1

#define ID_CONTINUE                     1017        // defined in ..\avihed\resource.h
#define ID_STOP                         1018        //
#define ID_JUMP                         1019        //
