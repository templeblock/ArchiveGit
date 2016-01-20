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

	dlg.c has:
		About -- Processes messages for "About" dialog box
        FrameDlg -- Modeless dialog; allows user to play the movie,
        	advance or reverse a frame, or jump to a specified
        	frame.  Also displays current frame (e.g. Frame x of y)
		SelectDlg -- Proc for display of current hotspot coords.
		HotspotDlg -- Modal dialog that appears when user double clicks in
			a hotspot rect so that user can set hotspot id,
			command, etc.
*/    

#include <windows.h>
#include <mmsystem.h>
#include <digitalv.h>
#include <viewer.h>
#include <string.h>
#include <commdlg.h>
#include <stdlib.h>

#include "hotspot.h"
#include "avihed.h"            /* specific to this program          */
#include "avihede.h"

/*
	About -- calls EndDialog when IDOK or IDCANCEL is recieved.
*/

BOOL __export CALLBACK About(hDlg, message, wParam, lParam)
HWND hDlg;               /* window handle of the dialog box */
unsigned message;        /* type of message                 */
WORD wParam;             /* message-specific information    */
LONG lParam;
{
    switch (message)
    {
        case WM_INITDIALOG:            /* message: initialize dialog box */
            return (TRUE);

        case WM_COMMAND:               /* message: received a command */
            if (wParam == IDOK         /* "OK" box selected?          */
                || wParam == IDCANCEL) /* System menu close command?  */
            {
                EndDialog(hDlg, TRUE); /* Exits the dialog box        */
                return (TRUE);
            }
            break;
    }
    return (FALSE);               /* Didn't process a message    */
}
/*
	FrameDlg -- responds when user presses play, stop, forward frame,
		reverse frame, and goto frame buttons on screen.  See each
		message handler for more comments.
*/
BOOL __export CALLBACK FrameDlg(hDlg, message, wParam, lParam)
HWND hDlg;               /* window handle of the dialog box */
unsigned message;        /* type of message                 */
WORD wParam;             /* message-specific information    */
LONG lParam;
{
    char szBuf[129];
    HDC hDC;
    
    switch (message)
    {
        case WM_INITDIALOG:            /* message: initialize dialog box */
            return (TRUE);

        case WM_SIZE:
        case WM_MOVE: 
            GetWindowRect(hDlg, &rcwndDlg);
            break;
            
        case WM_COMMAND:
            switch (wParam)
                {
                // ID_PLAY -- if playing, seek to current frame minus 1
                // and stop, by using MCI_SEEK.  Change button to "play".
                // If not playing, change button to stop and call playmovie().
                case ID_PLAY:
                    if (pMovieInfo)                        
                        {
                        if (pMovieInfo->fPlaying)
                            {
                            MCI_SEEK_PARMS mciSeek;
                            //playMovie(pMovieInfo, 0);
                            pMovieInfo->fPlaying = FALSE;
                            pMovieInfo->dwCurrentFrame = GetMovieFrame(pMovieInfo);
                            mciSeek.dwTo = pMovieInfo->dwCurrentFrame - 1;
                            mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_SEEK, MCI_TO | MCI_WAIT,
                                        (DWORD) (LPMCI_SEEK_PARMS)&mciSeek);
                    
                            
                            wsprintf(szBuf, "%ld of %ld", pMovieInfo->dwCurrentFrame,
                                                      pMovieInfo->dwMovieLength);
                            SetDlgItemText(hwndDlg, ID_CURRENTFRAME, szBuf);                        
                            SetDlgItemText(hwndDlg, ID_PLAY, "Play");
                            EnableWindow(GetDlgItem(hwndDlg, ID_FORWARD), TRUE);
                            EnableWindow(GetDlgItem(hwndDlg, ID_REVERSE), TRUE);                    
                            }
                        else
                            {                                                
                            SetDlgItemText(hwndDlg, ID_PLAY, "Stop");
                            EnableWindow(GetDlgItem(hwndDlg, ID_FORWARD), FALSE);
                            EnableWindow(GetDlgItem(hwndDlg, ID_REVERSE), FALSE);
                            playMovie(pMovieInfo, 1);
                            pMovieInfo->fPlaying = TRUE;
                            }
                        }
                    break;
                
				// ID_FORWARD -- increment dwCurrentFrame and call setMovie
				// to update the screen, call DrawRects to show hotspots,
				// call SetDlgItemText to show user new frame #.
                case ID_FORWARD:
                    if (pMovieInfo->dwCurrentFrame < pMovieInfo->dwMovieLength)
                        {
                        ++pMovieInfo->dwCurrentFrame;
                        setMovie(pMovieInfo, pMovieInfo->dwCurrentFrame, hDlg);
                        hDC = GetDC(pMovieInfo->hwndMovie);
                        DrawRects(hDC, pMovieInfo);
                        ReleaseDC(pMovieInfo->hwndMovie, hDC);
                        wsprintf(szBuf, "%ld of %ld", pMovieInfo->dwCurrentFrame,
                                                      pMovieInfo->dwMovieLength);
                        SetDlgItemText(hDlg, ID_CURRENTFRAME, szBuf);
                        }
                    break;
                    
				// ID_REVERSE -- decrement dwCurrentFrame and call setMovie
				// to update the screen, call DrawRects to show hotspots,
				// call SetDlgItemText to show user new frame #.
                case ID_REVERSE:
                    if (pMovieInfo->dwCurrentFrame > 0)         // 0 = first frame
                        {
                        --pMovieInfo->dwCurrentFrame;
                        setMovie(pMovieInfo, pMovieInfo->dwCurrentFrame, hDlg);
                        hDC = GetDC(pMovieInfo->hwndMovie);
                        DrawRects(hDC, pMovieInfo);
                        ReleaseDC(pMovieInfo->hwndMovie, hDC);                    
                        wsprintf(szBuf, "%ld of %ld", pMovieInfo->dwCurrentFrame,
                                                      pMovieInfo->dwMovieLength);
                        SetDlgItemText(hDlg, ID_CURRENTFRAME, szBuf);
                        }                                        
                    break;
                    
				// ID_GOFRAME -- get the frame number from GetDlgItemText,
				// then put it in dwCurrentFrame and do the same DrawRects
				// and SetDlgItemText stuff that ID_FORWARD and ID_REVERSE
				// (see) do.
                case ID_GOFRAME:
                    {                    
                    DWORD dwFrame;
                    
                    GetDlgItemText(hDlg, ID_FRAMEEDIT, szBuf, sizeof(szBuf) - 1);
                    dwFrame = atol(szBuf);
                                        
                    if (dwFrame != pMovieInfo->dwCurrentFrame &&
                            dwFrame <= pMovieInfo->dwMovieLength &&
                            dwFrame >= 0)
                        {                            
                        pMovieInfo->dwCurrentFrame = dwFrame;
                        setMovie(pMovieInfo, pMovieInfo->dwCurrentFrame, hDlg);
                        hDC = GetDC(pMovieInfo->hwndMovie);
                        DrawRects(hDC, pMovieInfo);
                        ReleaseDC(pMovieInfo->hwndMovie, hDC);                        
                        wsprintf(szBuf, "%ld of %ld", pMovieInfo->dwCurrentFrame,
                                                      pMovieInfo->dwMovieLength);
                        SetDlgItemText(hDlg, ID_CURRENTFRAME, szBuf);                        
                        }
                    }
                    return (TRUE);                                
                }
            break;
    }
    return (FALSE);               /* Didn't process a message    */
}


/*
	SelectDlg -- catches WM_MOVE messages and updates rcwndSelectDlg
		to the new coordinates.  This is so the rect can get saved
		and resored from the INI file.
*/
BOOL __export CALLBACK SelectDlg(hDlg, message, wParam, lParam)
HWND hDlg;               /* window handle of the dialog box */
unsigned message;        /* type of message                 */
WORD wParam;             /* message-specific information    */
LONG lParam;
{
    switch (message)
    {
        case WM_INITDIALOG:            /* message: initialize dialog box */
            return (TRUE);
            
        case WM_SIZE:
        case WM_MOVE:
            GetWindowRect(hDlg, &rcwndSelectDlg);
            break;
            
    }
    return (FALSE);               /* Didn't process a message    */
}

/*
	HotspotDlg -- this is for that big dialog box that pops up when
		you double click a hotspot.  Processes WM_INITDIALOG and
		IDOK messages.  WM_INITDIALOG transfers data from the MOVIEINFO
		into the dialog box, and the IDOK routine transfers the data
		from the dialog box back into the MOVIEINFO.
*/
BOOL __export CALLBACK HotspotDlg(hDlg, message, wParam, lParam)  
HWND hDlg;               /* window handle of the dialog box */
unsigned message;        /* type of message                 */
WORD wParam;             /* message-specific information    */
LONG lParam;
{
    char szBuf[129];
    static PHOTSPOT pHotspot;    
    BOOL bTranslated;
    
    switch (message)
    {
        case WM_INITDIALOG:            /* message: initialize dialog box */
            // pHotspot passed as lParam
            pHotspot = (PHOTSPOT) lParam;
            SetDlgItemInt(hDlg, ID_LEFT, pHotspot->rc.left, FALSE);
            SetDlgItemInt(hDlg, ID_TOP, pHotspot->rc.top, FALSE);
            SetDlgItemInt(hDlg, ID_RIGHT, pHotspot->rc.right, FALSE);
            SetDlgItemInt(hDlg, ID_BOTTOM, pHotspot->rc.bottom, FALSE);            
            SetDlgItemInt(hDlg, ID_BEGINFRAME, pHotspot->BeginFrame, FALSE);
            SetDlgItemInt(hDlg, ID_ENDFRAME, pHotspot->EndFrame, FALSE);
            CheckRadioButton(hDlg, ID_CONTINUE, ID_JUMP, pHotspot->OnClick);
            if (pHotspot->pszCommand)
                SetDlgItemText(hDlg, ID_COMMAND, pHotspot->pszCommand);
            if (pHotspot->pszHotspotID)
                SetDlgItemText(hDlg, ID_HOTSPOTID, pHotspot->pszHotspotID);
            SetDlgItemInt(hDlg, ID_TOFRAME, pHotspot->ToFrame, FALSE);
            return (TRUE);

        case WM_COMMAND:
            switch (wParam)
                {
                case IDOK:
                    //if (!GetDlgItemText(hDlg, ID_HOTSPOTID, szBuf, sizeof(szBuf) - 1))
                        //{
                        //MessageBox(hDlg, "You must enter Hotspot ID","Incomplete record:",
                                        //MB_OK | MB_ICONEXCLAMATION);
                        //return (TRUE);                                        
                        //}
                    //else            // yeah, this is pretty scary 
                        {
                        PHOTSPOT pHotspotList, pHsp;
                        pHotspotList = pHotspot;
                        
                        if (pHotspotList->pPrev)            // if there's a previous, then
                            {
                            while (pHotspotList->pPrev)     // get to beginning of list
                                {
                                pHotspotList = pHotspotList->pPrev;
                                if (pHotspotList)           // if not NULL, store it
                                    pHsp = pHotspotList;
                                }
                            }
                                                    else
                        pHsp = pHotspotList;            // if not, just set to the only one
                        while (pHsp)
                            {
                            //if (pHsp != pHotspot)           // if it's not this one
                                //{
                                //if (pHsp->pszHotspotID)     // if memory allocated
                                    //{                       // compare the strings
                                    //if (0 == (lstrcmp(pHsp->pszHotspotID, szBuf)))
                                        //{
                                        //MessageBox(hDlg, "Another Hotspot already has that ID",
                                                //"Error", MB_OK | MB_ICONEXCLAMATION);
                                        //return (TRUE);
                                        //}
                                    //}
                                //}
                            pHsp = pHsp->pNext;             // next one
                            }                        
                        }                        
                    pHotspot->rc.left = GetDlgItemInt(hDlg, ID_LEFT, &bTranslated, TRUE);
                    pHotspot->rc.top = GetDlgItemInt(hDlg, ID_TOP, &bTranslated, TRUE);
                    pHotspot->rc.right = GetDlgItemInt(hDlg, ID_RIGHT, &bTranslated, TRUE);
                    pHotspot->rc.bottom = GetDlgItemInt(hDlg, ID_BOTTOM, &bTranslated, TRUE);
                    pHotspot->BeginFrame = GetDlgItemInt(hDlg, ID_BEGINFRAME, &bTranslated, TRUE);
                    pHotspot->EndFrame = GetDlgItemInt(hDlg, ID_ENDFRAME, &bTranslated, TRUE);
                    if (IsDlgButtonChecked(hDlg, ID_CONTINUE))
                        pHotspot->OnClick = ID_CONTINUE;
                    else if (IsDlgButtonChecked(hDlg, ID_STOP))
                        pHotspot->OnClick = ID_STOP;
                    else if (IsDlgButtonChecked(hDlg, ID_JUMP))
                        {
                        pHotspot->OnClick = ID_JUMP;
                        pHotspot->ToFrame = GetDlgItemInt(hDlg, ID_TOFRAME, &bTranslated, TRUE);                            
                        }                            
                    else        // impossible case, but...
                        pHotspot->OnClick = ID_CONTINUE;                    
                    if (GetDlgItemText(hDlg, ID_COMMAND, szBuf, sizeof(szBuf) - 1))
                        {
                        if (pHotspot->pszCommand)           // free the old one if there is
                            FREE(pHotspot->pszCommand);                            
                        pHotspot->pszCommand = (LPSTR) ALLOCATE(lstrlen(szBuf) + 1);
                        lstrcpy(pHotspot->pszCommand, szBuf);
                        }
                    if (GetDlgItemText(hDlg, ID_HOTSPOTID, szBuf, sizeof(szBuf) - 1))
                        {
                        if (pHotspot->pszHotspotID)           // free the old one if there is
                            FREE(pHotspot->pszHotspotID);                        
                        pHotspot->pszHotspotID = (LPSTR) ALLOCATE(lstrlen(szBuf) + 1);
                        lstrcpy(pHotspot->pszHotspotID, szBuf);
                        }                        
                    bModified = TRUE;                        
                    EndDialog(hDlg, TRUE);
                    return (TRUE);

                case IDCANCEL:
                    EndDialog(hDlg, FALSE);
                    return (TRUE);                                        
                }
            break;
    }
    return (FALSE);               /* Didn't process a message    */
}
