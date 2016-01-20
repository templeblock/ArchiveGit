/*
    minerva.h
*/



#pragma warning(disable:4001 4514)
#if defined(NDEBUG) && defined(_MSC_VER) && !defined(_WIN32)
/* 16-bit microsoft c complains about assert being a "statement has no effect" */
#pragma warning(disable:4705)
#endif


#include <assert.h>

#ifndef NDEBUG
extern char szFalse[];
static char _szFILE[] = __FILE__;
#undef assert
#ifndef __BORLANDC__
#define assert(exp) ( (exp) ? (void) 0 : _assert(szFalse, _szFILE, __LINE__) )
#else
#define assert(exp) ( (exp) ? (void) 0 : __assertfail(szFalse, "", _szFILE, __LINE__) )
#endif
#endif 



/* eliminate warnings about unreferenced parameters
    I don't like warnings, but I do like the maximum warning level, but
    sometimes some API function parameters don't matter */
#define NOREFERENCE(x)              ( x = x )



/* Borland C 4.52 doesn't define hmemcpy */
#if defined(__FLAT__)
#if defined(hmemcpy)
#undef hmemcpy
#endif
#define hmemcpy MoveMemory
#endif



/* application_name only used now for the title for message boxes */
#define APPLICATION_NAME            "Minerva"

#define APPLICATION_INIFILENAME     "MINERVA.INI"
#define APPLICATION_HELPFILENAME    "MINERVA.HLP"
#define MDIFRAME_CLASSNAME          "MinervaFrameWindow"
#define MDICHILD_CLASSNAME          "MinervaChildWindow"



/* maximum number of mru files tracked can range from 1 .. 9
    0 will probably fail miserably
    10 and greater should work fine if you add IDM_FILE_MRUFILE10 .. IDM_FILE_MRUFILExx,
        (although the File menu will certainly be getting kind of busy */
#define MAX_MRUFILES                ( 4 )

/* maximum length of image comment and maximum length of image application data */
#define MAX_COMMENT                 ( 4096 )



/* see definitions in minerva.c */
extern HINSTANCE hinstThis;
extern HWND      hwndFrame;
extern HWND      hwndMDIClient;
extern HACCEL    hAcceleratorTable;
extern CTRLINITDATA NEAR InitTimingData[];
extern HCURSOR   hWaitCursor;
extern HCURSOR   hHandCursor;
extern HCURSOR   hArrowCursor;
extern int       nWaitCursor;
extern BOOL      bDisableRDTSC;

extern OPENOP      * NEAR pOpenOpTable[];
extern SAVEOP      * NEAR pSaveOpTable[];
extern TRANSFORMOP * NEAR pTransformOpTable[];
extern TOOLSOP     * NEAR pToolsOpTable[];
