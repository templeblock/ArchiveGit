/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1993 - 1994.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                   animbtns.c

******************************************************************************
******************************************************************************

  Description:  Animated buttons.
 Date Created:  2/25/94
       Author:  Ray

*****************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <math.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "animate.h"

/************************************************************************/

#define DO_ANIMATE_BTNS  TRUE

LPAOBJECT   AButtonSession = NULL;
int         hButtonObject = 0;

HINSTANCE LoadAnimatedButtonToy(HWND hWnd);

HBITMAP hVirginBitmap = 0;
HBITMAP GetVirginBackground(HWND hWnd);
/************************************************************************/

void AnimateButtonStart(HWND hWnd, BOOL bCopyBackground)
{
// Start playing the AnimateButton characters
    HINSTANCE   hToy;
    int         i;
    HWND        hParent;
    RECT        rCtrl;
    POINT       pt;
    
    if (AButtonSession) return;       // we already have one.    
    
    AButtonSession = ACreateNewAnimator();
    if (!AButtonSession) return;

// Animate into the parent window
    hParent = GetParent(hWnd);
    if (bCopyBackground)
        ACopyBackgroundScreen(AButtonSession, hParent, NULL);    

    GetWindowRect(hWnd, &rCtrl);
    pt.x = rCtrl.left;
    pt.y = rCtrl.top;
    ScreenToClient(hParent, &pt);
    
    for (i=0; i<20; i++)
        ShowCursor(FALSE);
    hToy = LoadAnimatedButtonToy(hWnd);
    if (hToy >= (HINSTANCE)HINSTANCE_ERROR)
    {
        // Load this toy and all its children, scripts, etc.
        hButtonObject = LoadToy(AButtonSession, NULL, 1, pt.x, pt.y, hToy);
        if (hButtonObject)
            AReExecuteScript(AButtonSession, hButtonObject);
        FreeLibrary(hToy);
    }
    for (i=0; i<20; i++)
        ShowCursor(TRUE);
}

void AnimateButtonPlay()
{
    // Keep the motion going
    if (!AButtonSession) return;
    if (!hButtonObject) return;
    
    AMoveAllObjects(AButtonSession);
}

void AnimateButtonEnd()
{
    // Stop and delete the animations
    if (!AButtonSession) return;

    SoundStopAndFree();
	
#ifdef USEWAVEMIX
	if (Control.bUseWaveMix)
		mixSoundStopAll();
#endif // USEWAVEMIX
		
    ADeleteAllObjects(AButtonSession);
    ADeleteAnimator(AButtonSession);
    AButtonSession = NULL;
    hButtonObject = NULL;
}


void AnimateButtonDoAnimation(HWND hWnd)
{
    HWND    hParent;
	BOOL	bSoundOn = FALSE;
	int		ID;
	MSG		msg;
	RECT	rRepair;
	RECT	rSrcRepair;
	HBITMAP	hPrevious;
    
    if (!DO_ANIMATE_BTNS) return;

// This is a special case JUST for the horn button.
// Make sure the sound is on while playing this animation.	
	ID = GET_WINDOW_ID(hWnd);
	if (ID == HS_HORN && !SoundIsOn())
	{	
		bSoundOn = TRUE;
		SoundToggle();
	}
//	if (ID == IDC_ADDONROOM)
//		ShowWindow(hWnd, SW_HIDE);		
    
    hParent = GetParent(hWnd);
    AnimateButtonStart(hWnd, FALSE);
    if (!AButtonSession)  return;

// Hide and update virgin background screen
//    ShowWindow(hWnd, SW_HIDE);
//    UpdateWindow(hParent);
	GetWindowRect(hWnd, &rRepair);
	ScreenToClient(hParent, (LPPOINT)&rRepair.left );
	ScreenToClient(hParent, (LPPOINT)&rRepair.right );

    ACopyBackgroundScreen(AButtonSession, hParent, NULL);
	hPrevious = ARepairBackgroundBitmap(
		AButtonSession, 
		GetRoomBitmap(), 
		rRepair, 
		rRepair, 
		TRUE);

// Immediately get something on the screen - NOW!
	AMoveAllObjects(AButtonSession);
	AMoveAllObjects(AButtonSession);
	AMoveAllObjects(AButtonSession);
	AMoveAllObjects(AButtonSession);
	AMoveAllObjects(AButtonSession);
	AMoveAllObjects(AButtonSession);

	while ( AWhere( AButtonSession, hButtonObject, NULL ) && !LBUTTON  )
	{
        AMoveAllObjects(AButtonSession);
#ifdef USEWAVEMIX
		if (Control.bUseWaveMix)
		{
    		if (PeekMessage(&msg, NULL, MM_WOM_DONE, MM_WOM_DONE, PM_REMOVE))
    		{	
        		TranslateMessage(&msg);
        		DispatchMessage (&msg);
			}
		}
#endif // USEWAVEMIX		
	}

// Put the animation bitmap back to where it was before the animation took place.
	if (hPrevious)
	{
		if (ID != IDC_ADDONROOM)
		{
			GetClientRect(hWnd, &rSrcRepair);
			ARepairBackgroundBitmap(
				AButtonSession, 
				hPrevious, 
				rSrcRepair, 
				rRepair, 
				FALSE);
		}
		DeleteObject(hPrevious);
	}

// Kill objects an session, show original button.
    AnimateButtonEnd();

	if (ID != IDC_ADDONROOM)
	{
		InvalidateRect(hWnd, NULL, TRUE);
    	UpdateWindow(hWnd);
	}

// This was on just for the HORN button to play when the sound is off
	if (bSoundOn)
		SoundToggle();
}


BOOL AnimateButtonGetIsRunning()
{
    return (AButtonSession && hButtonObject);
}





HBITMAP AnimateButtonGetBitmap(HWND hWnd)
{
    HBITMAP     hBitmap;
    LPFRAME     lpFrame;
    HINSTANCE   hToy;

    hToy = LoadAnimatedButtonToy(hWnd);
    if (hToy >= (HINSTANCE)HINSTANCE_ERROR)
    {
        BOOL bCursor;

        bCursor = AstralCursorEnable (FALSE);
        lpFrame = TiffResource2Frame(hToy, "TIF_1a" );
        AstralCursorEnable (bCursor);
        FreeLibrary(hToy);
        if (!lpFrame)
            return 0;
        hBitmap = FrameToBitmap( lpFrame, NULL /*lpRect*/ );
        return hBitmap;
    }
    return 0;
}




void AnimateButtonPaint(HWND hWnd)
{
    HDC         hDC;
    PAINTSTRUCT ps;
    HBITMAP     hBitmap, hOldBitmap;
    LPFRAME     lpFrame;
    HINSTANCE   hToy;
    BITMAP      bm;
    HDC         hMemoryDC;
    HBITMAP     hMergeBitmap;

    if (!hVirginBitmap)
        hVirginBitmap = GetVirginBackground(hWnd);
    
// Begin painting    
    hDC = BeginPaint(hWnd, &ps);

    hToy = LoadAnimatedButtonToy(hWnd);
    if (hToy >= (HINSTANCE)HINSTANCE_ERROR)
    {
        BOOL bCursor;

        bCursor = AstralCursorEnable (FALSE);
        lpFrame = TiffResource2Frame(hToy, "TIF_1a" );
        AstralCursorEnable (bCursor);
        FreeLibrary(hToy);
        if (!lpFrame)
        {
            EndPaint(hWnd, &ps);
            return;
        }
        hMemoryDC = CreateCompatibleDC(hDC);
        hBitmap = FrameToBitmap( lpFrame, NULL /*lpRect*/ );
        hMergeBitmap = AMergeToBackground(hVirginBitmap, hBitmap);
        hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hMergeBitmap);

        GetObject(hBitmap, sizeof(bm), &bm);
        BitBlt(
            hDC,
            0,
            0,
            bm.bmWidth,
            bm.bmHeight,
            hMemoryDC,
            0,
            0,
            SRCCOPY);
        SelectObject(hMemoryDC, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteObject(hMergeBitmap);
        FrameClose(lpFrame);
        DeleteDC(hMemoryDC);
    }

    EndPaint(hWnd, &ps);
}


BOOL IsButtonAnimated(HWND hWnd)
{
// Check to see if the window text is a number.
// If so, then try to use the number to get an animation toy filename.
    STRING  szTemp;
    int     iToy;
    
    GetWindowText(hWnd, szTemp, sizeof(szTemp));
    iToy = Ascii2Int(szTemp);
    return (iToy > 0); 
}


void ResizeAnimatedButton(HWND hWnd)
{
    LPFRAME     lpFrame;
    HINSTANCE   hToy;
	RECT		r;

// The size may already be set.	
	GetClientRect(hWnd, &r);
	if ((RectWidth(&r) > 2) && (RectHeight(&r) > 2))
		return;
    
    hToy = LoadAnimatedButtonToy(hWnd);
    if (hToy >= (HINSTANCE)HINSTANCE_ERROR)
    {
        BOOL bCursor;

        bCursor = AstralCursorEnable (FALSE);
        lpFrame = TiffResource2Frame(hToy, "TIF_1a" );
        AstralCursorEnable (bCursor);
        FreeLibrary(hToy);
        if (!lpFrame)
            return;
        SetWindowPos(hWnd,
            NULL,
            0,
            0,
            FrameXSize(lpFrame),
            FrameYSize(lpFrame),
            SWP_NOMOVE | SWP_NOZORDER);
        FrameClose( lpFrame );
    }
}


HINSTANCE LoadAnimatedButtonToy(HWND hWnd)
{
	STRING szString;
    FNAME szFileName;

    GetWindowText(hWnd, szString, sizeof(szString));
    GetToyFileName( szString, szFileName );
    return LoadLibrary(szFileName);
}



HBITMAP GetVirginBackground(HWND hWnd)
{
// Take a snapshot of the screen where the control will be placed.
// This "virgin" snapshot of the background screen will be used to 
// merge the bitmap of the object onto the screen.  The bitmap
// will appear to be placed naturally onto its background screen.
 
    HBITMAP     hBitmap, hOldBitmap;
    RECT        rCtrl;
    HDC         hMemoryDC;
    HDC         hParentDC;
    HWND        hParent;
    
// Get control rect
    hParent = GetParent(hWnd);    
    GetWindowRect(hWnd, &rCtrl);
    ScreenToClient(hParent, (LPPOINT)&rCtrl.left);
    ScreenToClient(hParent, (LPPOINT)&rCtrl.right);
    
// Create DC and Bitmap    
    hMemoryDC = CreateCompatibleDC(NULL);
    if (!hMemoryDC) return 0;
    hBitmap = CreateCompatibleBitmap(hMemoryDC, 
        rCtrl.right  - rCtrl.left,
        rCtrl.bottom - rCtrl.top);
    if (!hBitmap)
    {
        DeleteDC(hMemoryDC);
        return 0;
    }
    
// Blt to the new bitmap    
    hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);
    hParentDC = GetDC(hParent);
    BitBlt(
        hMemoryDC,
        0,
        0,
        rCtrl.right  - rCtrl.left,
        rCtrl.bottom - rCtrl.top,
        hParentDC,
        rCtrl.left,
        rCtrl.top,
        SRCCOPY);
        
// Clean up        
    ReleaseDC(hParent, hParentDC);
    SelectObject(hMemoryDC, hOldBitmap);
    DeleteDC(hMemoryDC);
    
    return hBitmap;
}


/****************************************************************************/

