/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1993 - 1994.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                   intermsn.c

******************************************************************************
******************************************************************************

  Description:  Intermission animations.
 Date Created:  2/21/94
       Author:  Ray

*****************************************************************************/

#include <windows.h>
#include <math.h>
#include "id.h"
#include "data.h"
#include "routines.h"
#include "animate.h"

extern BOOL bPaintAppActive;
static BOOL bWaveMixOn = TRUE;

/************************************************************************/

#define DEF_INTERMISSION_TIMEOUT    60000        // Measured in milliseconds.
//#define DEF_INTERMISSION_TIMEOUT    2000        // Measured in milliseconds.
#define CHANGE_RUN_TIME             30000
#define MAX_INTERMSN_TOYS  10

LPAOBJECT   AIntermissionSession = NULL;
int         hIntermsnObject = 0;
DWORD       TimeOut = DEF_INTERMISSION_TIMEOUT;
DWORD       dwLastCheckTime = 0xFFFFFFFF;
bool        bIsIntermissionOn = TRUE;
bool        bKillOnMessages = TRUE;
int         iLastToy = 1;


void ChooseAnimationFileName(bool bSmallToys, LPSTR szFileName);
/************************************************************************/

void IntermissionStart(bool bSmallToys)
{
// Start playing the intermission characters
    HWND        hWnd;
    HINSTANCE   hToy;
    FNAME       szFileName;
    int         x, y;
    int         i;
    
// Temporary
if ( bSmallToys )
	return;
// Temporary

	if (AnimateIsOn()) return;				// Batch animation in play

    if (AIntermissionSession) return;       // we already have one.    
    
    AIntermissionSession = ACreateNewAnimator();
    if (!AIntermissionSession) return;
	
	hWnd = AstralDlgGet(IDD_MAIN); 
	if (!bPaintAppActive)
    	hWnd = AstralDlgGet(idCurrentRoom);
    if (!hWnd) return;
    
    bIsIntermissionOn = TRUE;
#ifdef USEWAVEMIX
	bWaveMixOn = Control.bUseWaveMix;
	Control.bUseWaveMix = FALSE;		// We don't use the wave mixer for intermissions.
#endif
    ACopyBackgroundScreen(AIntermissionSession, hWnd, NULL);

    x = 0;
    y = 0;
    
    for (i=0; i<20; i++)
        ShowCursor(FALSE);
    ChooseAnimationFileName(bSmallToys, szFileName);
    if ((hToy = LoadLibrary(szFileName)) >= (HINSTANCE)HINSTANCE_ERROR)
    {
        // Load this toy and all its children, scripts, etc.
        hIntermsnObject = LoadToy(AIntermissionSession, NULL, 1, x, y, hToy);
        if (hIntermsnObject)
        {
            AReExecuteScript(AIntermissionSession, hIntermsnObject);
        }
        FreeLibrary(hToy);
    }
    for (i=0; i<20; i++)
        ShowCursor(TRUE);
}

void IntermissionPlay()
{
    // Keep the motion going
    if (!AIntermissionSession) return;
    if (!bIsIntermissionOn) return;
    if (!hIntermsnObject) return;
    
    AMoveAllObjects(AIntermissionSession);
}

void IntermissionEnd()
{
    // Stop and delete the animations
    if (!AIntermissionSession) return;

    SoundStopAndFree();
#ifdef USEWAVEMIX
	if (Control.bUseWaveMix)
		mixSoundStopAll();

	Control.bUseWaveMix = bWaveMixOn;
#endif // USEWAVEMIX

    ADeleteAllObjects(AIntermissionSession);
    ADeleteAnimator(AIntermissionSession);
    AIntermissionSession = NULL;
    hIntermsnObject = NULL;
}

void IntermissionMsg(LPMSG lpMsg, bool bMsg)
{
    DWORD   dwTickCount;
    bool    bStop = FALSE;

// This function is called during the PeekMessage loop.
// It checks to see if it is time to enable the intermission animations.
// If enough time has elapsed since the last message, then it will start.
// It also will stop the animations if there is a message.
    
    if (!bIsIntermissionOn) return;
    
// Check to see if there is any intermission stopper messages.
// Reset the time if there is.        
    if (bMsg)
    {
        switch (lpMsg->message)
        {
            case WM_LBUTTONDOWN:
            case WM_RBUTTONDOWN:
            case WM_MOUSEMOVE:
            case WM_PAINT:
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
                bStop = TRUE;
        }
        if (bStop)
        {
            dwLastCheckTime = GetTickCount();
            IntermissionEnd();
        }
    } 

// Check to see if a new intermisssion should be started.
// If enough time has elaped, then start the animation session.
    dwTickCount = GetTickCount();
    if (!AIntermissionSession)
    {
        if ((dwTickCount > dwLastCheckTime) &&
            ((dwTickCount - dwLastCheckTime) > TimeOut))
        {
            dwLastCheckTime = dwTickCount;
            IntermissionStart(FALSE);
        }
    }
    else
    {
// An animation has been running for some time now.  Should the Object be changed.    
        if ((dwTickCount > dwLastCheckTime) &&
            ((dwTickCount - dwLastCheckTime) > CHANGE_RUN_TIME))
        {
            dwLastCheckTime = dwTickCount;
            IntermissionEnd();
            IntermissionStart(FALSE);
        }
    }
    
    IntermissionPlay();
}

void IntermissionSetKillOnMessages(bool bKill) 
{
    // Should it kill animation when a message arrives
    bKillOnMessages = bKill;
}


void IntermissionSetTimeout(DWORD msTimeout)      // Time before it starts
{
    TimeOut = msTimeout;
}

DWORD IntermissionGetTimeout()
{
    return TimeOut;
}


void IntermissionSetIsOn(bool bOn)
{
    // Is is checking for non-activity
    bIsIntermissionOn = bOn;
}

bool IntermissionGetIsOn()
{
    return bIsIntermissionOn;
}

bool IntermissionGetIsRunning()
{
    return (AIntermissionSession && hIntermsnObject);
}





void ChooseAnimationFileName(bool bSmallToys, LPSTR szFileName)
{
    FNAME   szTemp;
    int     iToy = iLastToy++;
    
    if (iLastToy > MAX_INTERMSN_TOYS)
        iLastToy = 1;
    
//  if (bSmallToys)
//      lstrcpy(szTemp, "toy16.toy");
//  else
    {
// lstrcpy(szTemp, "tbweed01.toy");    
        lstrcpy(szTemp, GetString (IDS_INTERMSN_TOY, NULL));    // "tbweed10.toy"
        wsprintf(szTemp, szTemp, iToy);
    }
        
    PathCat(szFileName, Control.PouchPath, szTemp);
}


/****************************************************************************/

