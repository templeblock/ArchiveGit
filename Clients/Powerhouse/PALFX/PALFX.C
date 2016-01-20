/*
 *
 *   PROGRAM: palfx.c
 *
 *    (C) Copyright Microsoft Corp. 1991, 1992.  All rights reserved.
 *
 *    You have a royalty-free right to use, modify, reproduce and 
 *    distribute the Sample Files (and/or any modified version) in 
 *    any way you find useful, provided that you agree that 
 *    Microsoft has no warranty obligations or liability for any 
 *    Sample Application Files which are modified. 
 *
 */

#include <windows.h>
#include <commdlg.h>
#include <stdlib.h>
#include <string.h>
#include "palfx.h"
#include "dib.h"
#include "gmem.h"

HANDLE hInst;

OPENFILENAME ofn;
char        szFilters[] = "Bitmaps\0*.bmp;*.dib\0"
                          "All Files\0*.*\0"
                          "";

char    DIBFileName[_MAX_FNAME];
char    TargetFileName [_MAX_FNAME];
char    OpenName[_MAX_FNAME];
char    str[255];

#include <sys\types.h>
#include <sys\stat.h>


PALETTEENTRY	gapeOld[256];		/* original color table */
PALETTEENTRY	gapeNew[256];		/* modified color table */
int		gcpe;			/* size of color table */


HMENU hPalFxMenu;
HANDLE hHourGlass;                        /* handle to hourglass cursor    */
int    hFile;                                /* file handle                   */
OFSTRUCT OfStruct;                        /* information from OpenFile()   */
struct stat FileStatus;                   /* information from fstat()      */
BOOL bFade = FALSE;                        /* TRUE if the fade in progress  */
RECT Rect;                                /* dimension of the client window  */
HWND hwnd;                                /* handle to main window           */

HANDLE hCurrDib = NULL;                         /* DIB currently displayed */
HPALETTE hpalCurrent = NULL;                    /* Palette for hCurrDib */

char *szFxClass="PalFxWClass";	// window class name

/* Fade to Black data */
#define FADEPERIOD 100	// set lower to fade faster
#define FADETIMER_ID 1
#define MAXCOLORS 256
#define DARKENBY 10

#define LINESCROLL	1
#define PAGESCROLL	20


PALETTEENTRY FadePal[MAXCOLORS];
HANDLE hBaseDIB;
HPALETTE hpal;
int    nNumFades, nNumColors;

/* Fade to target DIB data */
#define TARGETTIMER_ID 2
int    nNumTargetColors;
HPALETTE hTargetPal;
PALETTEENTRY TargetPal[MAXCOLORS];
PALETTEENTRY TempPal[MAXCOLORS];
BOOL bFadeToDIB = FALSE;
HANDLE hTargetDIB;

PALETTEENTRY TestPal[MAXCOLORS];
PALETTEENTRY SysPal[MAXCOLORS];

/* Needed for saving system colors */
BOOL bStatic = TRUE;
int    SysPalIndex [] = 
{
    COLOR_ACTIVEBORDER,
    COLOR_ACTIVECAPTION,
    COLOR_APPWORKSPACE,
    COLOR_BACKGROUND,
    COLOR_BTNFACE,
    COLOR_BTNSHADOW,
    COLOR_BTNTEXT,
    COLOR_CAPTIONTEXT,
    COLOR_GRAYTEXT,
    COLOR_HIGHLIGHT,
    COLOR_HIGHLIGHTTEXT,
    COLOR_INACTIVEBORDER,
    COLOR_INACTIVECAPTION,
    COLOR_MENU,
    COLOR_MENUTEXT,
    COLOR_SCROLLBAR,
    COLOR_WINDOW,
    COLOR_WINDOWFRAME,
    COLOR_WINDOWTEXT
};


/* Define the black-and-white color settings */

#define NumSysColors (sizeof (SysPalIndex) / sizeof (SysPalIndex[0]))
DWORD OldColors [NumSysColors];

#define rgbBlack RGB(0,0,0)
#define rgbWhite RGB(255,255,255)

DWORD MonoColors[NumSysColors] = 
{
    rgbWhite,              //    ACTIVEBORDER
    rgbWhite,              //    ACTIVECAPTION
    rgbWhite,              //    APPWORKSPACE
    rgbWhite,              //    BACKGROUND
    rgbWhite,              //    BTNFACE
    rgbBlack,              //    BTNSHADOW
    rgbBlack,              //    BTNTEXT
    rgbBlack,              //    CAPTIONTEXT
    rgbBlack,              //    GRAYTEXT
    rgbBlack,              //    HIGHLIGHT
    rgbWhite,              //    HIGHLIGHTTEXT
    rgbWhite,              //    INACTIVEBORDER
    rgbWhite,              //    INACTIVECAPTION
    rgbWhite,              //    MENU
    rgbBlack,              //    MENUTEXT
    rgbWhite,              //    SCROLLBAR
    rgbWhite,              //    WINDOW
    rgbBlack,              //    WINDOWFRAME
    rgbBlack                //    WINDOWTEXT
};


void AppPaint (HWND hwnd, HDC hdc);
void FadeToBlack (HWND hWnd, HANDLE hDib);
void FadeOneStep ();
void BlackScreen ();
void InitBaseDIB (HWND hWnd, LPSTR FileName);
void InitTargetDIB (HWND hWnd, LPSTR FileName);
void InitTargetDisplay (HWND hWnd);
void BrightenOneStep ();
void StealColors ();
void AppActivate (BOOL fActive);


/*

    FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

    PURPOSE: calls initialization function, processes message loop

 */

int    PASCAL WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HANDLE hInstance;
HANDLE hPrevInstance;
LPSTR lpCmdLine;
int    nCmdShow;
{
    MSG msg;

    if (!hPrevInstance)
        if (!InitApplication(hInstance))
            return (FALSE);

    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    while (GetMessage(&msg, NULL, NULL, NULL)) 
    {

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (msg.wParam);
}


/*

    FUNCTION: InitApplication(HANDLE)

    PURPOSE: Initializes window data and registers window class

 */

BOOL InitApplication(hInstance)
HANDLE hInstance;
{
    WNDCLASS  wc;

    wc.style = NULL;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, "PalFxIcon");
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName =  MAKEINTRESOURCE(PALFXMENU);
    wc.lpszClassName = szFxClass;

    return (RegisterClass(&wc));
}


/****************************************************************************

    FUNCTION:  InitInstance(HANDLE, int)

    PURPOSE:  Saves instance handle and creates main window

****************************************************************************/

BOOL InitInstance(hInstance, nCmdShow)
HANDLE          hInstance;
int    nCmdShow;
{
    RECT            Rect;

    hInst = hInstance;

    hwnd = CreateWindow(
        szFxClass,
        "Palette FX Demo",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0,
        CW_USEDEFAULT, 0,
        NULL,
        NULL,
        hInstance,
        NULL
        );

    if (!hwnd)
        return (FALSE);

    GetClientRect(hwnd, (LPRECT) & Rect);

    /* Get an hourglass cursor to use during file transfers */

    hHourGlass = LoadCursor(NULL, IDC_WAIT);
    hPalFxMenu = GetMenu (hwnd);

    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
    return (TRUE);
}


long    FAR PASCAL MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    FARPROC lpProcAbout;


    HDC hdc;                                          /* context handle */
    PAINTSTRUCT ps;
    int    i;
    HPALETTE hpalPrev;

    switch (message)
    {
    	case WM_PALETTECHANGED:

	    /* if my window caused the palette change, do nothing */
	    if (wParam == hwnd)
		break;

	    /* fall through */

	 case WM_QUERYNEWPALETTE:

	     /* process WM_PALETTECHANGED or WM_QUERYNEWPALETTE:
	      * temporarily select and realize current palette to see
	      * if a full redraw is required (due to some of the physical
              * palette entries <hwnd> uses being changed)
              */
	     if (hpalCurrent == NULL)
		 return (long) FALSE;    // no palette to realize
		     
	     hdc = GetDC(hwnd);
	     hpalPrev = SelectPalette(hdc, hpalCurrent, FALSE);
	     i = RealizePalette(hdc);
	     SelectPalette(hdc, hpalPrev, FALSE);
	     ReleaseDC(hwnd, hdc);

	     if (i)
		 InvalidateRect(hwnd, NULL, TRUE);

	     return (LONG) i;



	 case WM_CREATE:
	     break;

	 case WM_ACTIVATEAPP:
	     AppActivate (TRUE);        /* Steal system colors. */
	     bStatic = FALSE;
	     break;

	 case WM_COMMAND:
	     switch (wParam)
	     {
		 case IDM_ABOUT:
		     lpProcAbout = MakeProcInstance(About, hInst);
		     DialogBox(hInst, "AboutBox", hWnd, lpProcAbout);
		     FreeProcInstance(lpProcAbout);
		     break;

		 case IDM_REVERT:
		     EnableMenuItem (hPalFxMenu, IDM_TOBLACK, MF_ENABLED);
		     EnableMenuItem (hPalFxMenu, IDM_TODIB, MF_ENABLED);
		     BlackScreen ();
		     InitBaseDIB (hwnd, DIBFileName);
		     EnableMenuItem (hPalFxMenu, IDM_REVERT, MF_GRAYED);
		     break;

		 case IDM_TOBLACK:
		     EnableMenuItem (hPalFxMenu, IDM_TOBLACK, MF_GRAYED);
		     EnableMenuItem (hPalFxMenu, IDM_TODIB, MF_GRAYED);
		     FadeToBlack (hwnd, hBaseDIB);
		     break;

		 case IDM_TODIB:
		     EnableMenuItem (hPalFxMenu, IDM_TOBLACK, MF_GRAYED);
		     EnableMenuItem (hPalFxMenu, IDM_TODIB, MF_GRAYED);

		     
		     OpenName[0] = 0;
		     ofn.lStructSize         = sizeof(ofn);
		     ofn.hwndOwner           = hwnd;
		     ofn.hInstance           = hInst;
		     ofn.lpstrFilter         = szFilters;
		     ofn.nFilterIndex        = 1;
		     ofn.lpstrCustomFilter   = NULL;
		     ofn.nMaxCustFilter      = 0;
		     ofn.lpstrFile           = OpenName;
		     ofn.nMaxFile            = sizeof(OpenName);
		     ofn.lpstrFileTitle      = "";
		     ofn.nMaxFileTitle       = 0;
		     ofn.lpstrInitialDir     = NULL;
		     ofn.lpstrTitle          = "Open File";
		     ofn.Flags               = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		     ofn.lCustData           = 0;
		     ofn.nFileOffset         = 0;
		     ofn.nFileExtension      = 0;
		     ofn.lpstrDefExt         = NULL;
		     ofn.lpfnHook            = NULL;
		     ofn.lpTemplateName      = NULL;

		     if (!GetOpenFileName(&ofn))
			 return 0L;

		     if ((hFile = OpenFile(OpenName, (LPOFSTRUCT) & OfStruct,
			 OF_READ)) == -1) 
		     {
			 wsprintf(str, "Error %d opening %s.",
			     OfStruct.nErrCode, (LPSTR)OpenName);
			 MessageBox(hwnd, str, NULL,
			     MB_OK | MB_ICONHAND);
			 return (NULL);
		     }
#if 0		     
		     lpOpenDlg = MakeProcInstance((FARPROC) OpenDlg, hInst);
		     /* Open the file and get its handle */
		     hFile = DialogBox(hInst, "Open", hWnd, lpOpenDlg);
		     FreeProcInstance(lpOpenDlg);
		     if (!hFile)
			 return (NULL);
#endif		     
		     lstrcpy( (LPSTR)TargetFileName, (LPSTR)OpenName);
		     _lclose (hFile);
		     InitTargetDIB (hwnd, TargetFileName);
		     bFadeToDIB = TRUE;
		     FadeToBlack (hwnd, hBaseDIB);
		     break;


		 case IDM_OPEN:
		     OpenName[0] = 0;
		     ofn.lStructSize         = sizeof(ofn);
		     ofn.hwndOwner           = hwnd;
		     ofn.hInstance           = hInst;
		     ofn.lpstrFilter         = szFilters;
		     ofn.nFilterIndex        = 1;
		     ofn.lpstrCustomFilter   = NULL;
		     ofn.nMaxCustFilter      = 0;
		     ofn.lpstrFile           = OpenName;
		     ofn.nMaxFile            = sizeof(OpenName);
		     ofn.lpstrFileTitle      = "";
		     ofn.nMaxFileTitle       = 0;
		     ofn.lpstrInitialDir     = NULL;
		     ofn.lpstrTitle          = "Open File";
		     ofn.Flags               = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		     ofn.lCustData           = 0;
		     ofn.nFileOffset         = 0;
		     ofn.nFileExtension      = 0;
		     ofn.lpstrDefExt         = NULL;
		     ofn.lpfnHook            = NULL;
		     ofn.lpTemplateName      = NULL;

		     if (!GetOpenFileName(&ofn))
			 return 0L;

		     if ((hFile = OpenFile(OpenName, (LPOFSTRUCT) & OfStruct,
			 OF_READ)) == -1) 
		     {
			 wsprintf(str, "Error %d opening %s.",
			     OfStruct.nErrCode, (LPSTR)OpenName);
			 MessageBox(hwnd, str, NULL,
			     MB_OK | MB_ICONHAND);
			 return (NULL);
		     }
#if 0		     
		     lpOpenDlg = MakeProcInstance((FARPROC) OpenDlg, hInst);

		     /* Open the file and get its handle */

		     hFile = DialogBox(hInst, "Open", hWnd, lpOpenDlg);
		     FreeProcInstance(lpOpenDlg);
		     if (!hFile)
			 return (NULL);
#endif		     
		     lstrcpy( DIBFileName, OpenName);
		     _lclose (hFile);
		     InitBaseDIB (hwnd, DIBFileName);
		     EnableMenuItem (hPalFxMenu, IDM_TOBLACK, MF_ENABLED);
		     EnableMenuItem (hPalFxMenu, IDM_TODIB, MF_ENABLED);
		     break;

		 case IDM_EXIT:
		     DestroyWindow(hWnd);
		     break;
		     
		default:
			break;

	    }
	    return NULL;

	case WM_LBUTTONDOWN:                    /* Mouse input */
	    if (bFade)
	    {
	    }
	    break;

	case WM_TIMER:
	    switch (wParam )
	    {
		case FADETIMER_ID:
		/* Perform fade if necessary. */
		if (nNumFades > 0)
		{
		    FadeOneStep ();
		    nNumFades--;
		}
		else
		{
		    KillTimer (hwnd, FADETIMER_ID);
		    if (bFadeToDIB)
		    {
			/* Display blackened target DIB */
			BlackScreen ();
			InitTargetDisplay (hwnd);
			nNumFades = 256 / DARKENBY + 1;
			SetTimer (hwnd, TARGETTIMER_ID, FADEPERIOD, NULL);
		    }
		    else
		    {
			EnableMenuItem (hPalFxMenu, IDM_REVERT, MF_ENABLED);
		    }
		}
		break;
		
		case TARGETTIMER_ID:
		    if (nNumFades > 0)
		    {
			BrightenOneStep ();
			nNumFades--;
		    }
		    else
		    {
			KillTimer (hwnd, TARGETTIMER_ID);
			bFadeToDIB = FALSE;
			EnableMenuItem (hPalFxMenu, IDM_REVERT, MF_ENABLED);
		    }
		    break;
	    }
	    return 0;

	case WM_PAINT:
	    hdc = BeginPaint(hWnd, &ps);
	    AppPaint (hWnd, hdc);
	    EndPaint(hWnd, &ps);
	    return 0;

	case WM_DESTROY:
	    AppActivate (FALSE);        /* Restore system colors. */
	    bStatic = TRUE;
	    if(hpalCurrent)
		DeleteObject (hpalCurrent);
	    if (hBaseDIB)
		GFree (hBaseDIB);
	    if(hTargetPal)
		DeleteObject (hTargetPal);
	    if (hTargetDIB)
		GFree (hTargetDIB);
	    PostQuitMessage(0);
	    break;

	default:
	    break;
    }
    return (DefWindowProc(hWnd, message, wParam, lParam));
}


/****************************************************************************

    FUNCTION: About(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages for "About" dialog box

    MESSAGES:

        WM_INITDIALOG - initialize dialog box
        WM_COMMAND    - Input received

****************************************************************************/

BOOL FAR PASCAL About(hDlg, message, wParam, lParam)
HWND hDlg;
unsigned    message;
WORD wParam;
LONG lParam;
{
    switch (message)
    {
    case WM_INITDIALOG:
        return (TRUE);

    case WM_COMMAND:
        if (wParam == IDOK
             || wParam == IDCANCEL)
        {
            EndDialog(hDlg, TRUE);
            return (TRUE);
        }
        break;
    }
    return (FALSE);
}


/****************************************************************************

    FUNCTION: AppPaint

    PURPOSE:  Repaint the current display surface.

    This function depends on GLOBALS!!! (ack)
    
****************************************************************************/

void AppPaint (HWND hwnd, HDC hdc)
{
    BITMAPINFOHEADER bi;
    WORD   xPos, yPos;
    RECT rc;

    if (hCurrDib)
    {
	DibInfo (hCurrDib, &bi);    /* Redisplay the current DIB */
        GetClientRect(hwnd, &rc);
        xPos = (WORD) ((rc.right  - bi.biWidth ) / 2);
        yPos = (WORD) ((rc.bottom - bi.biHeight) / 2);
        DrawDib(hdc, xPos, yPos, hCurrDib, hpalCurrent, DIB_PAL_COLORS);
    }
}


//    This function depends on GLOBALS!!! (ack)
void InitBaseDIB (HWND hWnd, LPSTR FileName)
{
    LPBITMAPINFOHEADER lpbi = NULL;
    RGBQUAD FAR * prgb;
    int    i;
    LOGPALETTE * pPal;
    HANDLE hDib;

    hDib = OpenDIB (FileName);
    /* Lock the DIB and get color info */
    lpbi = (LPBITMAPINFOHEADER) GLock (hDib);
    if (lpbi == NULL)
        return;

    prgb = (RGBQUAD FAR * )((LPBYTE)lpbi + lpbi->biSize);

    nNumColors = DibNumColors(lpbi);

    if (nNumColors)
    {
	/* Allocate a new color table, copy the palette entries to
         * it, and create the palette. 
         */
        pPal = (LOGPALETTE * )LocalAlloc(LPTR, 
            sizeof(LOGPALETTE) + nNumColors * sizeof(PALETTEENTRY));

        if (!pPal)
            return;

        pPal->palNumEntries = nNumColors;
        pPal->palVersion    = PALVERSION;

        for (i = 0; i < nNumColors; i++)
        {
            pPal->palPalEntry[i].peRed   = prgb->rgbRed;
            pPal->palPalEntry[i].peGreen = prgb->rgbGreen;
            pPal->palPalEntry[i].peBlue  = prgb->rgbBlue;
	    pPal->palPalEntry[i].peFlags = PC_RESERVED;

	    /* Create the palette to replace the original palette. */
            FadePal[i].peRed   = prgb->rgbRed;
            FadePal[i].peGreen = prgb->rgbGreen;
            FadePal[i].peBlue  = prgb->rgbBlue;
            FadePal[i].peFlags = PC_RESERVED;

            prgb++;
        }

        GUnlock(hDib);
	
        hpal = CreatePalette(pPal);
        LocalFree((HANDLE)pPal);

        SetDibUsage (hDib, hpal, DIB_PAL_COLORS);

        InvalidateRect(hWnd, NULL, TRUE);  // have apppaint do the work...

        hCurrDib = hDib;
        hBaseDIB = hDib;
	if(hpalCurrent)
	    DeleteObject (hpalCurrent);
	hpalCurrent = hpal;
    }
}


//    This function depends on GLOBALS!!! (ack)
void InitTargetDIB (HWND hWnd, LPSTR FileName)
{
    LPBITMAPINFOHEADER lpbi = NULL;
    RGBQUAD FAR * prgb;
    int    i;
    LOGPALETTE * pPal;
    HANDLE hDib;

    hDib = OpenDIB (FileName);

    /* Lock the DIB and get color info */
    lpbi = (LPBITMAPINFOHEADER) GLock (hDib);
    if (lpbi == NULL)
        return;

    // the next line was the problem...
    prgb = (RGBQUAD FAR * )((LPBYTE)lpbi + lpbi->biSize);

    nNumTargetColors = DibNumColors(lpbi);

    if (nNumTargetColors)
    {
	/* Allocate a new color table, copy the palette entries to
         * it, and create the palette. 
         */
        pPal = (LOGPALETTE * )LocalAlloc(LPTR, 
            sizeof(LOGPALETTE) + nNumTargetColors * sizeof(PALETTEENTRY));

        if (!pPal)
            return;

        pPal->palNumEntries = nNumTargetColors;
        pPal->palVersion    = PALVERSION;

        for (i = 0; i < nNumTargetColors; i++)
        {
            pPal->palPalEntry[i].peRed     = prgb->rgbRed;
            pPal->palPalEntry[i].peGreen = prgb->rgbGreen;
            pPal->palPalEntry[i].peBlue  = prgb->rgbBlue;
            pPal->palPalEntry[i].peFlags = PC_RESERVED;

	    /* Create the palette to replace the current palette. */
            TempPal[i].peRed = 0;
            TempPal[i].peGreen = 0;
            TempPal[i].peBlue = 0;
            TempPal[i].peFlags = PC_RESERVED;
            TargetPal[i].peRed     = prgb->rgbRed;
            TargetPal[i].peGreen = prgb->rgbGreen;
            TargetPal[i].peBlue  = prgb->rgbBlue;
            TargetPal[i].peFlags = PC_RESERVED;

            prgb++;
        }

        GUnlock(hDib);

	if(hTargetPal)
	    DeleteObject(hTargetPal);
        hTargetPal = CreatePalette(pPal);
        LocalFree((HANDLE)pPal);
	
        SetDibUsage (hDib, hTargetPal, DIB_PAL_COLORS);
        SetPaletteEntries(hTargetPal, 0, nNumTargetColors, TempPal);

	if (hTargetDIB)
	    GFree (hTargetDIB);

        hTargetDIB = hDib;
    }
}


void InitTargetDisplay (HWND hWnd)
{
    HDC hDC;
    BITMAPINFOHEADER bi;
    WORD  xPos, yPos;
    HPALETTE hOldPal;
    RECT rc;

    if (hTargetDIB)
    {
	/* Display the DIB */
        hDC = GetDC(hWnd);
        hOldPal = SelectPalette (hDC, hTargetPal, FALSE);
	
        UnrealizeObject(hTargetPal);

        RealizePalette (hDC);
    
	DibInfo (hTargetDIB, &bi);
        GetClientRect(hWnd, &rc);
        xPos =(WORD) ( (rc.right  - bi.biWidth ) / 2);
        yPos =(WORD) ( (rc.bottom - bi.biHeight) / 2);

        DrawDib(hDC, xPos, yPos, hTargetDIB, hTargetPal, DIB_PAL_COLORS);
        SelectPalette(hDC, hOldPal, FALSE);
        ReleaseDC(hWnd, hDC);

	if (hCurrDib)
	    GFree (hCurrDib);
	
        hCurrDib = hTargetDIB;
	
	if(hpalCurrent)
	    DeleteObject (hpalCurrent);
	    
        hpalCurrent = hTargetPal;
    }
}




/****************************************************************************

    FUNCTION: FadeToBlack

    PURPOSE:  Gradually fades the given DIB to black.

    This function depends on GLOBALS!!! (ack)
                     
****************************************************************************/
void FadeToBlack (HWND hWnd, HANDLE hDib)
{
    /* Start timer for fade. */
    nNumFades = 256 / DARKENBY +1;
    SetTimer (hwnd, FADETIMER_ID, FADEPERIOD, NULL);
}


/****************************************************************************

    FUNCTION: FadeOneStep                      

    PURPOSE:

    This function depends on GLOBALS!!! (ack)
                     
****************************************************************************/
void FadeOneStep ()
{
    int    i, r, g, b;
    HDC hdc;
    HPALETTE hOldPal;

    /* Do one step of palette fade. */
    for (i = 0; i < nNumColors; i++)
    {
#ifdef LINEARFADE	    
	// linear fade where 'bright' colors take longer to dissapear
        r = FadePal[i].peRed   - DARKENBY;
        g = FadePal[i].peGreen - DARKENBY;
        b = FadePal[i].peBlue  - DARKENBY;
#else	
	// with this fade, all the colors dissapear at once no matter
	// how 'bright' they were.
        r = FadePal[i].peRed   - min(FadePal[i].peRed/nNumFades,DARKENBY);
        g = FadePal[i].peGreen - min(FadePal[i].peGreen/nNumFades,DARKENBY);
        b = FadePal[i].peBlue  - min(FadePal[i].peBlue/nNumFades,DARKENBY);
#endif	

        FadePal[i].peRed   = (BYTE)max(r, 0);
        FadePal[i].peGreen = (BYTE)max(g, 0);
        FadePal[i].peBlue  = (BYTE)max(b, 0);
        FadePal[i].peFlags = PC_RESERVED;
    }

    hdc = GetDC(hwnd);
    hOldPal=SelectPalette(hdc, hpalCurrent, FALSE);
    RealizePalette(hdc);
    AnimatePalette (hpalCurrent, 0, nNumColors, FadePal);
    SelectPalette(hdc,hOldPal,FALSE);
    ReleaseDC(hwnd, hdc);
}


/****************************************************************************

    FUNCTION: BrightenOneStep                      

    PURPOSE:
    
    This function depends on GLOBALS!!! (ack)
                     
****************************************************************************/
void BrightenOneStep ()
{
    int    i, r, g, b;
    HDC hdc;
    HPALETTE hOldPal;

    /* Do one step of palette brighten. */
    for (i = 0; i < nNumTargetColors; i++)
    {
        r = max(0, TargetPal[i].peRed   - DARKENBY * nNumFades);
        g = max(0, TargetPal[i].peGreen - DARKENBY * nNumFades);
        b = max(0, TargetPal[i].peBlue  - DARKENBY * nNumFades);

        TempPal[i].peRed   = (BYTE)r;
        TempPal[i].peGreen = (BYTE)g;
        TempPal[i].peBlue  = (BYTE)b;
        TempPal[i].peFlags = PC_RESERVED;
    }

    hdc = GetDC(hwnd);
    hOldPal=SelectPalette(hdc, hTargetPal, FALSE);
    RealizePalette(hdc);
    AnimatePalette (hTargetPal, 0, nNumTargetColors, TempPal);
    SelectPalette(hdc,hOldPal,FALSE);
    ReleaseDC(hwnd, hdc);
}



/****************************************************************************

    FUNCTION: BlackScreen

    PURPOSE:                                         
                     
****************************************************************************/
void BlackScreen ()
{
    HDC hDC;
    RECT rc;

    /* Blackout screen to avoid showing palette change */
    hDC = GetDC(hwnd);
    SetBkColor(hDC, 0);
    GetClientRect(hwnd, &rc);
    ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, "", 0, NULL);
    ReleaseDC(hwnd, hDC);

}


/****************************************************************************

    FUNCTION: AppActivate

    PURPOSE:  Saves the current system color values  and obtains the 
                    use of the system colors when the application
                    becomes active and restores the original system colors
                    when the application indicates it is no longer active.
                     
****************************************************************************/
void AppActivate (BOOL fActive)
{
    return;
}


