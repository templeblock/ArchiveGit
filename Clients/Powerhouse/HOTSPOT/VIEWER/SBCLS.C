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
/*
	subcls.c:
		Terminate -- stops playing the movie and closes it
		OnButtonDown -- if clicked point on a hotspot,
			a viewer command is executed
		CleanupStuff -- removes properties from windows before they
			are destroyed
		SbClsProc -- subclasses the movie window
*/
#include <windows.h>
#include <mmsystem.h>
#include <digitalv.h>
#include <viewer.h>
#include <string.h>

#include "hotspot.h"

extern char szMovieInfo[];

/*
	Terminate -- pauses movie, then closes it with fileCloseMovie
*/ 
void Terminate(PMOVIEINFO pMovieInfo)
{
    playMovie(pMovieInfo, 0);
    fileCloseMovie(pMovieInfo, FALSE);
}

/*
	OnButtonDown -- walks through hotspot list.  For each hotspot,
		uses PtInRect to see if its in the rect.  If it is, it uses
		VwrCommand to transmit the hotspot's command to viewer.  Then
		it looks at the Stop/Continue/Jump to setting and uses the return
		value to handle Stop and Continue, or calls setMovie to do a
		jump.
*/ 
BOOL OnButtonDown(PMOVIEINFO pMovieInfo, USHORT msg, USHORT wp, LONG lp)
{
    PHOTSPOT pHotspot;
    DWORD dwFrame;                      
    POINT pt;
    HWND hwnd;
    
    if (!pMovieInfo)
        return FALSE;
    
    pHotspot = pMovieInfo->pHotspotList;
    pt = MAKEPOINT(lp);
    dwFrame = GetMovieFrame(pMovieInfo);
    
    while (pHotspot)
        {        
        if (dwFrame >= pHotspot->BeginFrame && dwFrame <= pHotspot->EndFrame)
            {
            if (PtInRect(&pHotspot->rc, pt) != 0)
                {                
                hwnd = GetParent(pMovieInfo->hwndParent);
                if (!hwnd)
                    hwnd = pMovieInfo->hwndParent;
                VwrCommand(VwrFromHinst(GetWindowWord(hwnd, GWW_HINSTANCE)),
                                NULL, pHotspot->pszCommand, cmdoptNONE);
                switch (pHotspot->OnClick)
                    {
                    case ID_CONTINUE:
                        return (TRUE);                        
                    case ID_STOP:
                        return (FALSE);
                    case ID_JUMP:
                        setMovie(pMovieInfo, pHotspot->ToFrame, NULL);
                        pMovieInfo->dwCurrentFrame = pHotspot->ToFrame;
                        pMovieInfo->fPlaying = FALSE;
                        playMovie(pMovieInfo, 1);
                        return (TRUE);                        
                    default:
                        return (TRUE);                                            
                    }                                
                }
            }
        pHotspot = pHotspot->pNext;                        
        }
    return (TRUE);        
}

/*
	CleanupStuff -- Removes properties and hotspots from windows in
		movieinfo structure.  Frees the handle to the movieinfo, too!
*/
void CleanupStuff(HWND hwnd, PMOVIEINFO pMovieInfo, HANDLE hglb)
{
	RemoveProp(hwnd, (LPSTR)szMovieInfo);
    if (pMovieInfo)
    {
        if (pMovieInfo->hwndParent)
        	RemoveProp(pMovieInfo->hwndParent, (LPSTR)szMovieInfo);
        DeleteHotspotList(pMovieInfo);                
    }
    if (pMovieInfo)               
        SetWindowLong (hwnd, GWL_WNDPROC,
                (LONG) pMovieInfo->lpfnOldProc);
    GlobalUnlock(hglb);
    GlobalFree(hglb);
}

/*
	SbClsProc -- catches WM_LBUTTONDOWN, MM_MCINOTIFY, and WM_DESTROY
		messages.  See each message for more info.
		Gets movieinfo ahead of time for all 3 message handlers.
*/ 
LONG FAR PASCAL _export SbClsProc (HWND hwnd, USHORT msg,
     USHORT wp, LONG lp)
{
    HANDLE hglb;
    PMOVIEINFO pMovieInfo;
    LRESULT lr;
    
    hglb = GetProp(hwnd, (LPSTR)szMovieInfo);
    if (!hglb)
        {
        dbg("ERROR: Invalid global memory handle (no window property)");
        pMovieInfo = NULL;
        }
    else pMovieInfo = (PMOVIEINFO)GlobalLock(hglb);

    switch (msg)
    {
    	/*
    		WM_LBUTTONDOWN -- passes point to OnButtonDown and
    			terminates movie if it returns FALSE.
    	 */
        case WM_LBUTTONDOWN:
            if (0 == (OnButtonDown(pMovieInfo, msg, wp, lp)))                
                {
                Terminate(pMovieInfo);
                return 0L;
                }        
            break;            
    
        case MM_MCINOTIFY:
            /* This is where we check the status of an AVI  */
            /* movie that might have been playing.  We do   */
            /* the play with MCI_NOTIFY on so we should get */
            /* a MCI_NOTIFY_SUCCESSFUL if the play      */
            /* completes on it's own.           */
            switch(wp){
                case MCI_NOTIFY_SUCCESSFUL:
                    /* the play finished, let's rewind */
                    /* and clear our flag.         */                
                    dbg("subclass MCI_NOTIFY_SUCCESSFUL");
                    pMovieInfo->fPlaying = FALSE;
                    Terminate(pMovieInfo); /* this will call our WM_DESTROY */
                    return 0L;
                }
            break;            

		/*
			WM_DESTROY -- calls the AVI's WM_DESTROY handler, so that
				AVI doesn't think this window exists when it acutally
				doesn't.  Then we Terminate() the movie and
				CleanupStuff() to free the movieinfo
		 */
        case WM_DESTROY:
        	lr = CallWindowProc(pMovieInfo->lpfnOldProc, hwnd, msg, wp, lp);
			Terminate(pMovieInfo);
 			CleanupStuff(hwnd,pMovieInfo,hglb);            
            hglb = NULL;
            pMovieInfo = NULL;
        break;
        default:
        	/* by default we want the AVI windowproc to do its normal thing */
        	lr = CallWindowProc(pMovieInfo->lpfnOldProc, hwnd, msg, wp, lp);
    }

	/* at this point we may or may not have a movieinfo.  If we do,
		we ought to unlock it.
	 */
    if (pMovieInfo)
        {
        if (hglb)
            GlobalUnlock(hglb);
        return (lr);
        }
    else
        return 0L;
}
