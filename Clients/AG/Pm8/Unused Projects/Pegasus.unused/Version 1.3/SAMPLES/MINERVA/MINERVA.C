/******************************************************************************
 *
 * MINERVA.C - Minerva initialization/exit
 *
 * Sample application to demonstrate the use of Pegasus Imaging Corporation
 * development kits for image compression, expansion and transformation
 *
 * Copyright (C) 1996, 1997 Pegasus Imaging Corporation
 *
 * This source code is provided to you as sample source code.  Pegasus Imaging
 * Corporation grants you the right to use this source code in any way you
 * wish. Pegasus Imaging Corporation assumes no responsibility and explicitly
 * disavows any responsibility for any and every use you make of this source
 * code.
 *
 * Global variables:
 *   hinstThis             - instance handle for this minerva instance
 *   hwndFrame             - MDI frame window handle
 *   hwndMDIClient         - MDI client window handle
 *   pOpenOpTable          - array of pointers to open opcode definitions
 *   pSaveOpTable          - array of pointers to save opcode definitions
 *   pTransformOpTable     - array of pointers to transform opcode definitions
 *   pToolsOpTable         - array of pointers to tool opcode definitions
 *
 * Global functions:
 *   WinMain               - windows initialization and exit
 *
 * Local functions:
 *   RegisterWindowClasses - register MDI frame and MDI child window classes
 *   CreateFrameWindow
 *   InitOpList            - { LoadOpDlls(); CallOpInits(); SetupOpMenus(); }
 *   LoadOpDlls            - PegasusLoad all available opcode dll's
 *   CallOpInits           - opcode-specific module initialization
 *   SetupOpMenus          - set File/Default Options menu, Transform menu and Tools menu
 *                           table-driven by opcode definitions and opcode dll presence
 *   CleanupOpList         - PegasusUnload all loaded opcode dll's
 *
 * Revision History:
 *   12-19-96  1.00.03  jrb  once-over and additional commenting
 *   12-07-96  1.00.01  jrb  added New Window to popup menu
 *                           added & to transform menu item in popup
 *   11-10-96 jrb    created
 ******************************************************************************/



#define STRICT
/* system includes */
#pragma warning(disable:4115 4201 4214)
#include <windows.h>
#include <windowsx.h>
#pragma warning(default:4115 4201 4214)
#include <memory.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#pragma warning(disable:4001)
#include "ctl3d.h"

/* PIC includes */
#include "pic.h"
#include "errors.h"

/* minerva includes */
#include "resource.h"
#include "mopcodes.h"
#include "mctrl.h"
#include "minerva.h"
#include "merror.h"
#include "mframe.h"
#include "mchild.h"
#include "mmru.h"
#include "mmisc.h"



/******************************************************************************
 *      global variables
 ******************************************************************************/



HINSTANCE hinstThis     = NULL;         /* handle for this minerva instance */
HWND      hwndFrame     = NULL;         /* MDI frame window handle          */
HWND      hwndMDIClient = NULL;         /* MDI client window handle         */
HACCEL    hAcceleratorTable;
HCURSOR   hWaitCursor   = NULL;
HCURSOR   hHandCursor   = NULL;
HCURSOR   hArrowCursor  = NULL;
int       nWaitCursor   = 0;
char 	  szFalse[] = "is FALSE";
BOOL      bDisableRDTSC = FALSE;



/* specifying image options timing controls' parameters and default values */
CTRLINITDATA NEAR InitTimingData[] =
    {
    /*
    pszIniKey, eType, nCtrlID, bPushed or bPassword, wLimitText, pszText or pszIniValue
    */

    { "Display Timing", eCheckBox, IDC_TIMING_CHECK_DISPLAYTIMING, FALSE },
    { "Yield",          eCheckBox, IDC_TIMING_CHECK_YIELD,         FALSE },
    
    { "InputK",         eEdit,     IDC_TIMING_EDIT_INPUTK,  FALSE, 4,   "0" },
    { "OutputK",        eEdit,     IDC_TIMING_EDIT_OUTPUTK, FALSE, 4,   "0" },

    { 0,                eText,     IDC_TIMING_STATIC_INPUTK  },
    { 0,                eText,     IDC_TIMING_STATIC_OUTPUTK },

    { 0, eEnd, -1 }
    };



/******************************************************************************
    OPENOP's define open opcodes.  OPENOP is declared in mopcodes.h.
    An open opcode is an opcode whose input is a file image and whose output is an
    equivalent (roughly) DIB.
*/

extern OPENOP OpenDefSequentialJpeg;    /* moseqjpg.c */
extern OPENOP OpenDefEnhancedSequentialJpeg; /* moseqjpg.c */
extern OPENOP OpenDefProgressiveJpeg;   /* moprgjpg.c */
extern OPENOP OpenDefPNG;               /* mopng.c    */
extern OPENOP OpenDefIM4;               /* moim4.c    */
extern OPENOP OpenDefIMStar3Plus;		/* moimstar.c */
extern OPENOP OpenDefIMStar3;           /* moimstar.c */
extern OPENOP OpenDefIMStar;            /* moimstar.c */
extern OPENOP OpenDefBMP81;             /* moutil81.c */
extern OPENOP OpenDefPCX81;             /* moutil81.c */
extern OPENOP OpenDefTIF81;             /* moutil81.c */
extern OPENOP OpenDefTGA81;             /* moutil81.c */
extern OPENOP OpenDefGIF85;             /* moutil81.c */
extern OPENOP OpenDefBMP;               /* mobmp.c    */
extern OPENOP OpenDefWavelet;           /* mowavlet.c */

OPENOP * NEAR pOpenOpTable[] =
    {
    &OpenDefSequentialJpeg,
    &OpenDefEnhancedSequentialJpeg,
    &OpenDefProgressiveJpeg,
    &OpenDefWavelet,
    &OpenDefPNG,
    &OpenDefIMStar3,
	&OpenDefIMStar3Plus,
    &OpenDefIMStar,
    &OpenDefIM4,
    &OpenDefBMP81,
    &OpenDefPCX81,
    &OpenDefTIF81,
    &OpenDefTGA81,
    &OpenDefGIF85,
    &OpenDefBMP,
    0
    };



/******************************************************************************
    SAVEOP's define save opcodes.  SAVEOP is declared in mopcodes.h.
    A save opcode is an opcode whose input is a DIB and whose output is an
    equivalent (roughly) file image.
*/

extern SAVEOP SaveDefSequentialJpeg;    /* msseqjpg.c */
extern SAVEOP SaveDefEnhancedSequentialJpeg; /* msseqjpg.c */
extern SAVEOP SaveDefD2JSequentialJpeg; /* msprgjpg.c */
extern SAVEOP SaveDefProgressiveJpeg;   /* msprgjpg.c */
extern SAVEOP SaveDefWavelet;           /* mswavlet.c */
extern SAVEOP SaveDefIMStar;            /* msimstar.c */
extern SAVEOP SaveDefIMStarPlus;        /* msimstar.c */
extern SAVEOP SaveDefBMP80;             /* msutil80.c */
extern SAVEOP SaveDefPCX80;             /* msutil80.c */
extern SAVEOP SaveDefTIF80;             /* msutil80.c */
extern SAVEOP SaveDefGIF84;             /* msutil80.c */
extern SAVEOP SaveDefTGA80;             /* msutil80.c */

SAVEOP * NEAR pSaveOpTable[] =
    {
    &SaveDefSequentialJpeg,
    &SaveDefEnhancedSequentialJpeg,
    &SaveDefProgressiveJpeg,
    &SaveDefWavelet,
    &SaveDefIMStar,
	&SaveDefIMStarPlus,
    &SaveDefBMP80,
    &SaveDefPCX80,
    &SaveDefTIF80,
    &SaveDefTGA80,
    &SaveDefGIF84,
    0
    };    



/******************************************************************************
    TRANSFORMOP's define transformation opcodes.  TRANSFORMOP is declared in mopcodes.h
    A transformation opcode is an opcode whose input and output are images, but which
    isn't appropriate as an open opcode or as a save opcode.
*/

extern TRANSFORMOP TransformDefZoom;    /* mxzoom.c   */
extern TRANSFORMOP TransformDefRotate;  /* mxrotate.c */
extern TRANSFORMOP TransformDefRotateEnhanced;  /* mxrotate.c */
extern TRANSFORMOP TransformDefP2S;     /* mxp2s.c    */
extern TRANSFORMOP TransformDefS2P;     /* mxs2p.c    */

TRANSFORMOP * NEAR pTransformOpTable[] =
    {       
    &TransformDefZoom,
    &TransformDefRotate,
    &TransformDefRotateEnhanced,
    &TransformDefP2S,
    &TransformDefS2P,
    0
    };    



/******************************************************************************
    TOOLOP's define tool opcodes.  TOOLOP is declared in mopcodes.h
    A tool opcode is an opcode whose input or output are not images, or which otherwise
    isn't appropriate as an open, save or transform opcode.
*/

extern TOOLSOP ToolDefUtility;          /* mtutil82.c */

TOOLSOP * NEAR pToolsOpTable[] =
    {
    &ToolDefUtility,
    0
    };    



/******************************************************************************
 *  local functions declared
 ******************************************************************************/



static BOOL RegisterWindowClasses(void);
static BOOL CreateFrameWindow(int nCmdShow);
static void InitOpList(void);
static void LoadOpDlls(void);
static void CallOpInits(void);
static void SetupOpMenus(void);
static void CleanupOpList(void);



/******************************************************************************
 *  global functions
 ******************************************************************************/



/******************************************************************************
 *  int WinMain(
 *      HINSTANCE hinstCurrent,
 *      HINSTANCE hinstPrevious,
 *      LPSTR     lpszCmdLine,
 *      int       nCmdShow);
 * 
 *  windows initialization and exit
 *
 *  parameters:
 *      hinstCurrent - handle to this instance of minerva
 *      hinstPrevious - handle to previous instance of minerva. If hinstPrevious is
 *          NULL, no other instance is active (this is the first and only instance).
 *      lpszCmdLine - command line parameters to this instance
 *      nCmdShow - see ShowWindow() documentation
 *
 *  returns:  
 *      value to be set as exit code for application
 *
 *  notes:
 *      WinMain is called as the application is starting up.  When WinMain returns,
 *      the application is terminated.
 ******************************************************************************/
int PASCAL WinMain(
    HINSTANCE hinstCurrent,     /* handle to this instance */
    HINSTANCE hinstPrevious,    /* handle to previous instance or NULL */
    LPSTR     lpszCmdLine,      /* command line */
    int       nCmdShow)         /* see ShowWindow() documentation */
{
    MSG msg;
	LPCSTR psz;
    
    NOREFERENCE(lpszCmdLine);

    /* make SURE that (LPBITMAPINFO)&PIC_PARM.Head is valid */
    assert(offsetof(PIC_PARM,   ColorTable) - offsetof(PIC_PARM,   Head) ==
           offsetof(BITMAPINFO, bmiColors)  - offsetof(BITMAPINFO, bmiHeader));
    
    hinstThis = hinstCurrent;   /* instance handle is globally accessible */

    /* allow ctl3d to hook us */
    Ctl3dRegister(hinstThis);
    /* dialogs and controls are automatically 3d */
    Ctl3dAutoSubclass(hinstThis);
    
    if ( hinstPrevious == NULL )
        {
        /* register window classes for first instance only
            hinstPrevious == NULL if and only if this is the only active
            instance of minerva */
        if ( !RegisterWindowClasses() )
            {
            Ctl3dUnregister(hinstCurrent);
            return ( 0 );
            }
        }

    /* keyboard accelerators for menu commands */
    hAcceleratorTable = LoadAccelerators(hinstThis, "MinervaAccelerators");
    if ( hAcceleratorTable == NULL )
        {
        ErrorMessage(STYLE_FATAL, IDS_LOADACCELERATORS);
        /* "An unexpected LoadAccelerators error occurred. Minerva cannot continue." */
        Ctl3dUnregister(hinstCurrent);
        return ( 0 );
        }

    if ( !CreateFrameWindow(nCmdShow) )
        {
        Ctl3dUnregister(hinstCurrent);
        return ( 0 );
        }

    /* load MRU file list from minerva.ini and update File menu */
    if ( !MruLoadList(hwndFrame, APPLICATION_INIFILENAME) )
        {
        DestroyWindow(hwndFrame);
        Ctl3dUnregister(hinstCurrent);
        return ( 0 );
        }

    hWaitCursor  = LoadCursor(NULL, IDC_WAIT);
    hArrowCursor = LoadCursor(NULL, IDC_ARROW);
    hHandCursor  = LoadCursor(hinstThis, MAKEINTRESOURCE(IDC_HAND));
    assert(hWaitCursor != NULL && hArrowCursor != NULL && hHandCursor != NULL);
    
    /* load PIC opcode DLL's */
    InitOpList();

    bDisableRDTSC = GetPrivateProfileInt("Settings", "DisableRDTSC", 0, APPLICATION_INIFILENAME);
	psz = lpszCmdLine + _fstrspn(lpszCmdLine, " ");
	while ( psz != 0 && ( *psz == '-' || *psz == '/' || *psz == '+' ) )
	{
		if ( _fstrnicmp(psz + 1, "RDTSC", sizeof("RDTSC") - 1) == 0 && psz[sizeof("RDTSC")] <= ' ' )
			bDisableRDTSC = *psz != '+';
		psz = _fstrpbrk(psz, " ");
		if ( psz != 0 )
			psz += _fstrspn(psz, " ");
	}
	if ( !bDisableRDTSC )
		MiscTickCount();	// calibrate the RDTSC ticks

    while ( GetMessage(&msg, NULL, 0, 0) )
        {
        if ( !TranslateMDISysAccel(hwndMDIClient, &msg) &&
             !TranslateAccelerator(hwndFrame, hAcceleratorTable, &msg) )
            {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            }
        }
        
    DestroyCursor(hHandCursor);
    /* unload PIC opcode DLL's */
    CleanupOpList();

    /* save MRU file list to minerva.ini */
    MruSaveList(APPLICATION_INIFILENAME);

    Ctl3dUnregister(hinstCurrent);

    return ( msg.wParam );
}



/******************************************************************************
 *      local functions
 ******************************************************************************/



/******************************************************************************
 *  BOOL RegisterWindowClasses(void)
 * 
 *  register MDI frame and MDI child window classes
 *
 *  returns:  
 *      FALSE if any error occurs else TRUE
 *
 *  notes:
 *      Registers MDI frame window class and MDI child window class.
 *
 *      no error is expected unless windows is _very_ low on resources
 ******************************************************************************/
static BOOL RegisterWindowClasses(void)
{
    WNDCLASS wcFrame;
    WNDCLASS wcChild;

    memset(&wcFrame, 0, sizeof(wcFrame));
    wcFrame.style         = 0;  
    wcFrame.lpfnWndProc   = FrameWndProc;       /* defined in mframe.c */
    wcFrame.cbClsExtra    = 0;
    wcFrame.cbWndExtra    = 0;
    wcFrame.hInstance     = hinstThis;
    wcFrame.hIcon         = LoadIcon(hinstThis, MAKEINTRESOURCE(IDI_MINERVA_FRAME));
                            /* pegasus icon */
    wcFrame.hCursor       = NULL;
    wcFrame.hbrBackground = (HBRUSH)( COLOR_APPWORKSPACE + 1 );
                            /* standard system color for MDI frame background */
    wcFrame.lpszMenuName  = "MinervaMenu";
    wcFrame.lpszClassName = MDIFRAME_CLASSNAME; /* = "MinervaFrameWindow" */

    memset(&wcChild, 0, sizeof(wcChild));
    wcChild.style         = 0;  
    wcChild.lpfnWndProc   = ChildWndProc;       /* defined in mchild.c */
    wcChild.cbClsExtra    = 0;
    wcChild.cbWndExtra    = sizeof(void PICFAR*);
                            /* each MDI child has a pointer to instance data */
    wcChild.hInstance     = hinstThis;
    wcChild.hIcon         = LoadIcon(hinstThis, MAKEINTRESOURCE(IDI_MINERVA_CHILD));
                            /* tsobig.pic made into a 256 color icon */
    wcChild.hCursor       = NULL;
    wcChild.hbrBackground = NULL;
                            /* standard system color is (HBRUSH)( COLOR_WINDOW + 1 )
                                but the presentation looks better if the background
                                isn't ever colored behid the image -- having no background
                                brush lets us completely control that */
    wcChild.lpszMenuName  = NULL;
    wcChild.lpszClassName = MDICHILD_CLASSNAME; /* = MinervaChildWindow" */

    if ( !RegisterClass(&wcFrame) || !RegisterClass(&wcChild) )
        {
        ErrorMessage(STYLE_FATAL, IDS_REGISTERCLASS);
        /* "An unexpected RegisterClass error occurred. Minerva cannot continue." */
        return ( FALSE );
        }

    return ( TRUE );
}



/******************************************************************************
 *  BOOL CreateFrameWindow(int nCmdShow)
 * 
 *  parameters:
 *      nCmdShow - as passed to WinMain by windows (see ShowWindow() doc)
 *       
 *  returns:  
 *      FALSE if any error occurs else TRUE
 *
 *  notes:
 *      MDIFrameWndProc in mframe.c creates the MDI client window during frame
 *      WM_CREATE messgae processing so hwndMDIClient should be set before
 *      CreateWindow returns.
 *
 *      also shows the frame window according to WinMain's nCmdShow parameter
 *
 *      no error is expected unless windows is very low on resources
 ******************************************************************************/
static BOOL CreateFrameWindow(int nCmdShow)
{
    hwndFrame = CreateWindow(
        "MinervaFrameWindow",           /* class                    */
        APPLICATION_NAME,               /* title = "Minerva"        */
        WS_OVERLAPPEDWINDOW,            /* style.                   */
        CW_USEDEFAULT,                  /* left                     */
        CW_USEDEFAULT,                  /* top                      */
        CW_USEDEFAULT,                  /* width                    */
        CW_USEDEFAULT,                  /* height                   */
        NULL,                           /* parent                   */
        NULL,                           /* class menu.              */
        hinstThis,                      /* owner module             */
        NULL);                          /* additional creation data */
    if ( hwndFrame == NULL || hwndMDIClient == NULL )
        {
        ErrorMessage(STYLE_FATAL, IDS_CREATEWINDOW_FRAME);
        /* "An unexpected CreateWindow error occurred. Minerva cannot continue." */
        return (FALSE);
        }

    ShowWindow(hwndFrame, nCmdShow);
    UpdateWindow(hwndFrame);
    return ( TRUE );
}



/******************************************************************************
 *  void InitOpList(void)
 *
 *  opcode dll specific initialization and setup based on the particular
 *  opcode dll's which are present
 ******************************************************************************/
static void InitOpList(void)
{
    LoadOpDlls();   /* load opcode dll's */
    CallOpInits();  /* call the opcode-specific module's init functions */
    SetupOpMenus(); /* setup menus which list opcodes */
}



/******************************************************************************
 *  void LoadOpDlls(void)
 * 
 *  PegasusLoad all available opcode dll's
 *
 *  notes:
 *      OpTable.pszAbout is <> 0 if and only if we should attempt to PegasusLoad
 *      the opcode.
 *
 *      Only the current opcode version (CURRENT_PARMVER) is loaded according to
 *      the PIC.H file we're compiling with
 *       
 *      No path is specified in PegasusLoad, so the opcode DLL could be loaded
 *      from anywhere in LoadLibrary's directory search order
 ******************************************************************************/
static void LoadOpDlls(void)
{
    int i;
    LONG result;

    for ( i = 0; pOpenOpTable[i] != 0; i++ )
        {
        assert(pOpenOpTable[i]->lOpcode != 0);
        if ( pOpenOpTable[i]->pszAbout )
            {
            result = PegasusLoad(pOpenOpTable[i]->lOpcode, CURRENT_PARMVER, NULL);
            if ( result == ERR_NONE )
                {
                pOpenOpTable[i]->nFoundParmVer = CURRENT_PARMVER;
                /* leave it loaded as a performance optimization */
                }
            else
                assert(result == ERR_OPCODE_DLL_NOT_FOUND);
                /* unexpected errors are ERR_OPCODE_DLL_INVALID and ERR_TOO_MANY_OPCODES_LOADED
                   ERR_OPCODE_DLL_INVALID - a file with the correct name exists,
                    but it's not a valid DLL or doesn't have a Pegasus entry point exported
                   ERR_TOO_MANY_OPCODES_LOADED if more than 100 different opcode+version
                    combinations are loaded (_barely_ conceivable in 16-bit windows at some
                    time in the future) */
            }
        }
    for ( i = 0; pSaveOpTable[i] != 0; i++ )
        {
        assert(pSaveOpTable[i]->lOpcode != 0);
        if ( pSaveOpTable[i]->pszAbout != 0 )
            {
            result = PegasusLoad(pSaveOpTable[i]->lOpcode, CURRENT_PARMVER, NULL);
            if ( result == ERR_NONE )
                {
                pSaveOpTable[i]->nFoundParmVer = CURRENT_PARMVER;
                // leave it loaded as a performance optimization
                }
            else
                assert(result == ERR_OPCODE_DLL_NOT_FOUND);
                /* unexpected errors are ERR_OPCODE_DLL_INVALID and ERR_TOO_MANY_OPCODES_LOADED
                   ERR_OPCODE_DLL_INVALID - a file with the correct name exists,
                    but it's not a valid DLL or doesn't have a Pegasus entry point exported
                   ERR_TOO_MANY_OPCODES_LOADED if more than 100 different opcode+version
                    combinations are loaded (_barely_ conceivable in 16-bit windows at some
                    time in the future) */
            }
        }
    for ( i = 0; pTransformOpTable[i] != 0; i++ )
        {
        assert(pTransformOpTable[i]->lOpcode != 0);
        if ( pTransformOpTable[i]->pszAbout != 0 )
            {
            result = PegasusLoad(pTransformOpTable[i]->lOpcode, CURRENT_PARMVER, NULL);
            if ( result == ERR_NONE )
                {
                pTransformOpTable[i]->nFoundParmVer = CURRENT_PARMVER;
                // leave it loaded as a performance optimization
                }
            else
                assert(result == ERR_OPCODE_DLL_NOT_FOUND);
                /* unexpected errors are ERR_OPCODE_DLL_INVALID and ERR_TOO_MANY_OPCODES_LOADED
                   ERR_OPCODE_DLL_INVALID - a file with the correct name exists,
                    but it's not a valid DLL or doesn't have a Pegasus entry point exported
                   ERR_TOO_MANY_OPCODES_LOADED if more than 100 different opcode+version
                    combinations are loaded (_barely_ conceivable in 16-bit windows at some
                    time in the future) */
            }
        }
    for ( i = 0; pToolsOpTable[i] != 0; i++ )
        {
        assert(pToolsOpTable[i]->lOpcode != 0);
        if ( pToolsOpTable[i]->pszAbout != 0 )
            {
            result = PegasusLoad(pToolsOpTable[i]->lOpcode, CURRENT_PARMVER, NULL);
            if ( result == ERR_NONE )
                {
                pToolsOpTable[i]->nFoundParmVer = CURRENT_PARMVER;
                // leave it loaded as a performance optimization
                }
            else
                assert(result == ERR_OPCODE_DLL_NOT_FOUND);
                /* unexpected errors are ERR_OPCODE_DLL_INVALID and ERR_TOO_MANY_OPCODES_LOADED
                   ERR_OPCODE_DLL_INVALID - a file with the correct name exists,
                    but it's not a valid DLL or doesn't have a Pegasus entry point exported
                   ERR_TOO_MANY_OPCODES_LOADED if more than 100 different opcode+version
                    combinations are loaded (_barely_ conceivable in 16-bit windows at some
                    time in the future) */
            }
        }
}



/******************************************************************************
 *  void CallOpInits(void)
 * 
 *  opcode-specific module initialization
 *
 *  notes:
 *      Each opcode definition could have defined an Init entry point to be called
 *      if the opcode DLL is successfully loaded but before menu items are added.
 *       
 *      The init entry points currently allow an opcode DLL duplicating functionality
 *      in another to replace the second.  Thus Sequential JPEG compress is implemented
 *      in OP_D2J and also in OP_S2D.  Minerva prefers OP_D2J if present, but uses
 *      OP_S2D otherwise.  SaveInitProgressiveJpeg (msprgjpg.c) handles this.  Also
 *      the functionality to open a windows bitmap is built in to Minerva -- but we
 *      prefer to use Opcode 81 if available.  OpenInitUtility (moutil81.c) handles
 *      this as well as marking all the various file types implemented in opcode 81
 *      as present (nFoundParmVer != 0).  Similarly, SaveInitUtility (moutil80.c)
 *      marks all the various file types implemented in opcode 80 as present.
 ******************************************************************************/
static void CallOpInits(void)
{
    int i;

    for ( i = 0; pOpenOpTable[i] != 0; i++ )
        {
        if ( pOpenOpTable[i]->nFoundParmVer != 0 && pOpenOpTable[i]->pfnInit != 0 )
            (*pOpenOpTable[i]->pfnInit)();
        }
    for ( i = 0; pSaveOpTable[i] != 0; i++ )
        {
        if ( pSaveOpTable[i]->nFoundParmVer != 0 && pSaveOpTable[i]->pfnInit != 0 )
            (*pSaveOpTable[i]->pfnInit)();
        }
    for ( i = 0; pTransformOpTable[i] != 0; i++ )
        {
        if ( pTransformOpTable[i]->nFoundParmVer != 0 && pTransformOpTable[i]->pfnInit != 0 )
            (*pTransformOpTable[i]->pfnInit)();
        }
    for ( i = 0; pToolsOpTable[i] != 0; i++ )
        {
        if ( pToolsOpTable[i]->nFoundParmVer != 0 && pToolsOpTable[i]->pfnInit != 0 )
            (*pToolsOpTable[i]->pfnInit)();
        }
}



/******************************************************************************
 *  void SetupOpMenus(void)
 * 
 *  set File/Default Options menu, Transform menu and Tools menu
 *  as table-driven by opcode definitions and opcode dll presence
 *
 *  notes:
 *      Each opcode definition can specify a File/Default Options menu item.
 *       
 *      Each transform opcode definition can specify a Transform menu item.
 *      Currently Zoom and Rotate are the Transform menu items.  P2S and S2P
 *      are defined as transform opcodes, but aren't accessible from the
 *      Transform menu as currently implemented.  They are used implicitly when
 *      an unmodified progressive image is Save As'ed sequential and vice versa.
 *
 *      Each tool opcode definition can specify a Tool menu item.  Currently
 *      Make Palette is the only tool opcode and the only Tool menu item.
 ******************************************************************************/
static void SetupOpMenus(void)
{
    int i;
    HMENU hMenu;
    HMENU hDefaultsMenu;
    int nMenuID = IDM_FILE_DEFAULTOPTIONS;

    hMenu = GetMenu(hwndFrame);
    assert(hMenu != NULL);
    hMenu = GetSubMenu(hMenu, FILE_MENU_POS);
    assert(hMenu != NULL);
    hMenu = GetSubMenu(hMenu, FILE_DEFAULTOPTIONS_MENU_POS);
    assert(hMenu != NULL);
    /* hMenu = menu handle for File/Default Options menu */

    hDefaultsMenu = GetSubMenu(hMenu, FILE_DEFAULTOPTIONS_OPEN_MENU_POS);
    /* hDefaultsMenu = menu handle for File/Default Options/Open menu */
    
    /* the File/Default Options/Open menu as loaded from the resource has a single
        menu item which is a separator.  It's there because App Studio changes
        the Default Options menu from a popup unless there's at least one menu item.
        The following deletes that separator prior to our adding Default Options
        menu items from the opcode definitions */
    DeleteMenu(hDefaultsMenu, 0, MF_BYPOSITION);
    
    /**************************************************************************
       For each opcode definition, if the opcode implements a default options dialog,
        add the pszDefaultsMenu string to the File/Default Options menu.  If the
        opcode DLL is present, enable the menu item, else gray it.  The menu item
        ID's start at IDM_FILE_DEFAULTOPTIONS, incremented each time a menu item
        is added. */
    for ( i = 0; pOpenOpTable[i] != 0; i++ )
        {        
        if ( pOpenOpTable[i]->pfnDefaultsDialog != 0 )
            {
            assert(pOpenOpTable[i]->pszDefaultsMenu != NULL);
            if ( pOpenOpTable[i]->nFoundParmVer != 0 )
                AppendMenu(
                    hDefaultsMenu,
                    MF_STRING | MF_ENABLED,
                    nMenuID,
                    pOpenOpTable[i]->pszDefaultsMenu);
            else
                AppendMenu(
                    hDefaultsMenu,
                    MF_STRING | MF_GRAYED,
                    nMenuID,
                    pOpenOpTable[i]->pszDefaultsMenu);
            nMenuID++;
            }
        }

    hDefaultsMenu = GetSubMenu(hMenu, FILE_DEFAULTOPTIONS_SAVE_MENU_POS);
    /* hMenu = menu handle for File/Default Options/Save menu */
    
    /* the File/Default Options/Save menu as loaded from the resource has a single
        menu item which is a separator.  It's there because App Studio changes
        the Default Options menu from a popup unless there's at least one menu item.
        The following deletes that separator prior to our adding Default Options
        menu items from the opcode definitions */
    DeleteMenu(hDefaultsMenu, 0, MF_BYPOSITION);
    for ( i = 0; pSaveOpTable[i] != 0; i++ )
        {
        if ( pSaveOpTable[i]->pfnDefaultsDialog != 0 )
            {
            if ( pSaveOpTable[i]->nFoundParmVer != 0 )
                AppendMenu(
                    hDefaultsMenu,
                    MF_STRING | MF_ENABLED,
                    nMenuID,
                    pSaveOpTable[i]->pszDefaultsMenu);
            else
                AppendMenu(
                    hDefaultsMenu,
                    MF_STRING | MF_GRAYED,
                    nMenuID,
                    pSaveOpTable[i]->pszDefaultsMenu);
            nMenuID++;
            }
        }

    hDefaultsMenu = GetSubMenu(hMenu, FILE_DEFAULTOPTIONS_TRANSFORM_MENU_POS);
    /* hMenu = menu handle for File/Default Options/Transform menu */
    
    /* the File/Default Options/Transform menu as loaded from the resource has a single
        menu item which is a separator.  It's there because App Studio changes
        the Default Options menu from a popup unless there's at least one menu item.
        The following deletes that separator prior to our adding Default Options
        menu items from the opcode definitions */
    DeleteMenu(hDefaultsMenu, 0, MF_BYPOSITION);
    for ( i = 0; pTransformOpTable[i] != 0; i++ )
        {
        if ( pTransformOpTable[i]->pfnDefaultsDialog != 0 )
            {
            if ( pTransformOpTable[i]->nFoundParmVer != 0 )
                AppendMenu(
                    hDefaultsMenu,
                    MF_STRING | MF_ENABLED,
                    nMenuID,
                    pTransformOpTable[i]->pszDefaultsMenu);
            else
                AppendMenu(
                    hDefaultsMenu,
                    MF_STRING | MF_GRAYED,
                    nMenuID,
                    pTransformOpTable[i]->pszDefaultsMenu);
            nMenuID++;
            }
        }

    hDefaultsMenu = GetSubMenu(hMenu, FILE_DEFAULTOPTIONS_TOOLS_MENU_POS);
    /* hMenu = menu handle for File/Default Options/Tools menu */
    
    /* the File/Default Options/Tools menu as loaded from the resource has a single
        menu item which is a separator.  It's there because App Studio changes
        the Default Options menu from a popup unless there's at least one menu item.
        The following deletes that separator prior to our adding Default Options
        menu items from the opcode definitions */
    DeleteMenu(hDefaultsMenu, 0, MF_BYPOSITION);
    for ( i = 0; pToolsOpTable[i] != 0; i++ )
        {
        if ( pToolsOpTable[i]->pfnDefaultsDialog != 0 )
            {
            if ( pToolsOpTable[i]->nFoundParmVer == 0 )
                AppendMenu(
                    hDefaultsMenu,
                    MF_STRING | MF_GRAYED,
                    nMenuID,
                    pToolsOpTable[i]->pszDefaultsMenu);
            else
                AppendMenu(
                    hDefaultsMenu,
                    MF_STRING | MF_ENABLED,
                    nMenuID,
                    pToolsOpTable[i]->pszDefaultsMenu);
            nMenuID++;
            }
        }

    /**************************************************************************
       For each transform opcode definition, if the opcode has a transform menu
        item string, add it to the Transform menu. The menu item id will be
        IDM_TRANSFORM plus the index of the transform opcode in pTransformOpTable */

    hMenu = GetMenu(hwndFrame);
    assert(hMenu != 0);
    hMenu = GetSubMenu(hMenu, TRANSFORM_MENU_POS);
    assert(hMenu != 0);
    /* hMenu = menu handle for Transform menu */

    /* the Transform menu as loaded from the resource has a single menu item
        which is a separator.  It's there because App Studio changes the Transform
        menu from a popup unless there's at least one menu item.  Following
        deletes that separator prior to our adding Transform menu items from the
        opcode definitions */
    DeleteMenu(hMenu, 0, MF_BYPOSITION);

    for ( i = 0; pTransformOpTable[i] != 0; i++ )
        {
        if ( pTransformOpTable[i]->pszMenuItem != 0 )
            {
            if ( pTransformOpTable[i]->nFoundParmVer != 0 )
                AppendMenu(
                    hMenu,
                    MF_STRING | MF_ENABLED,
                    IDM_TRANSFORM + i,
                    pTransformOpTable[i]->pszMenuItem);
            else
                AppendMenu(
                    hMenu,
                    MF_STRING | MF_GRAYED,
                    IDM_TRANSFORM + i,
                    pTransformOpTable[i]->pszMenuItem);
            }
        }

    /**************************************************************************
       For each tool opcode definition, if the opcode has a tool menu
        item string, add it to the Tools menu. The menu item id will be
        IDM_TOOLS plus the index of the tool opcode in pToolsOpTable */

    hMenu = GetMenu(hwndFrame);
    assert(hMenu != NULL);
    hMenu = GetSubMenu(hMenu, TOOLS_MENU_POS);
    assert(hMenu != NULL);
    /* hMenu = menu handle for Tools menu */
    
    /* the Tool menu as loaded from the resource has a single menu item
        which is a separator.  It's there because App Studio changes the Tool
        menu from a popup unless there's at least one menu item.  Following
        deletes that separator prior to our adding Tool menu items from the
        opcode definitions */
    DeleteMenu(hMenu, 0, MF_BYPOSITION);

    for ( i = 0; pToolsOpTable[i] != 0; i++ )
        {
        if ( pToolsOpTable[i]->pszMenuItem != NULL )
            {
            if ( pToolsOpTable[i]->nFoundParmVer != 0 )
                AppendMenu(
                    hMenu,
                    MF_STRING | MF_ENABLED,
                    IDM_TOOLS + i,
                    pToolsOpTable[i]->pszMenuItem);
            else
                AppendMenu(
                    hMenu,
                    MF_STRING | MF_GRAYED,
                    IDM_TOOLS + i,
                    pToolsOpTable[i]->pszMenuItem);
            }                    
        }   
}



/******************************************************************************
 *  void CleanupOpList(void)
 *
 *  PegasusUnload all loaded opcode dll's
 *
 *  notes:
 *      calls PegasusUnload for each DLL loaded by LoadOp
 ******************************************************************************/
static void CleanupOpList(void)
{
    int i;

    for ( i = 0; pOpenOpTable[i] != 0; i++ )
        {
        if ( pOpenOpTable[i]->pszAbout != 0 && pOpenOpTable[i]->nFoundParmVer != 0 )
            PegasusUnload(pOpenOpTable[i]->lOpcode, CURRENT_PARMVER);
        }
    for ( i = 0; pSaveOpTable[i] != 0; i++ )
        {
        if ( pSaveOpTable[i]->pszAbout != 0 && pSaveOpTable[i]->nFoundParmVer != 0 )
            PegasusUnload(pSaveOpTable[i]->lOpcode, CURRENT_PARMVER);
        }
    for ( i = 0; pTransformOpTable[i] != 0; i++ )
        {
        if ( pTransformOpTable[i]->pszAbout != 0 && pTransformOpTable[i]->nFoundParmVer != 0 )
            PegasusUnload(pTransformOpTable[i]->lOpcode, CURRENT_PARMVER);
        }
    for ( i = 0; pToolsOpTable[i] != 0; i++ )
        {
        if ( pToolsOpTable[i]->pszAbout != 0 && pToolsOpTable[i]->nFoundParmVer != 0 )
            PegasusUnload(pToolsOpTable[i]->lOpcode, CURRENT_PARMVER);
        }
}
