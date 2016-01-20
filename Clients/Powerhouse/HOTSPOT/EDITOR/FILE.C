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
    file.c has:
    	FileOpen -- Prompts for AVI and HOT files; loads selected files.
             Returns FALSE if didn't get an AVI file, Returns TRUE
             even if didn't get a HOT file but got an AVI file.
		FileSave -- saves a HOT file, optionally prompting for filename
		

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
#include "avihede.h"            // extern vars

#include "mciavi.h"
LONG FAR PASCAL _export ICAVIDrawProc(DWORD id, HDRVR hDriver,
	UINT uiMessage, LPARAM lParam1, LPARAM lParam2);


/*
	FileOpen -- uses GetOpenFileName to prompt user for AVI and HOT
		filenames.  Uses fileOpenMovie to load the AVI file, and
		playMovie and setMovie and MCI_WHERE to get it into the window
		and on the first frame.  Uses InitHotspots() to load
		hotspot file.
*/
BOOL FileOpen(HWND hWnd)
{    
    static OPENFILENAME ofn;
    char szBuf[129];
    char szFileName[MAXFILENAME] = "";
    char szFileTitle[MAXFILENAME] = "";
    char szFilterSpec [128] =                       /* file type filters */
             "AVI Files (*.AVI)\0*.AVI\0All Files (*.*)\0*.*\0";
    char szFilterSpec2 [128] = 
            "Hotspot Files (*.HOT)\0*.HOT\0All Files (*.*)\0*.*\0";

    ofn.lStructSize       = sizeof(OPENFILENAME);
    ofn.hwndOwner         = hWnd;
    ofn.lpstrFilter       = szFilterSpec;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0;
    ofn.nFilterIndex      = 1;
    ofn.lpstrFile         = szFileName;
    ofn.nMaxFile          = MAXFILENAME;
    ofn.lpstrInitialDir   = NULL;
    ofn.lpstrFileTitle    = szFileTitle;
    ofn.nMaxFileTitle     = MAXFILENAME;
    ofn.lpstrTitle        = "Open AVI File";
    ofn.lpstrDefExt       = "AVI";
    ofn.Flags             = 0;
    
    if (!GetOpenFileName ((LPOPENFILENAME)&ofn))
            return FALSE;    
    
    if (pMovieInfo)                                     // get rid of old stuff if there is
        {
        fileCloseMovie(pMovieInfo, TRUE);
        DeleteHotspotList(pMovieInfo);
        FREE(pMovieInfo);
        }
            
    lstrcpy(szWindowTitle, szBaseWindowTitle);
                    lstrcat(szWindowTitle, " - ");
                    lstrcat(szWindowTitle, szFileTitle);
    
    SetWindowText(hWnd, szWindowTitle);                
    
    {	// added SetProp to accomodate new drawproc
    	HANDLE hglb;
    	
    	hglb = GlobalAlloc(GHND,(DWORD)(sizeof(MOVIEINFO)));
    	if (hglb)
    	{
    		pMovieInfo = (PMOVIEINFO) GlobalLock(hglb);
    	}
    	else
    	{
    		pMovieInfo = NULL;
    	}
    	//pMovieInfo = (PMOVIEINFO) ((hglb = GlobalAlloc(GHND, (DWORD)(sizeof(MOVIEINFO)))) ? GlobalLock(hglb) : NULL);

	    if (!pMovieInfo)
	        return (FALSE);

	    pMovieInfo->hwndParent = hWnd;
	    lstrcpy(pMovieInfo->szAVIFileName, ofn.lpstrFile);

	    fileOpenMovie(pMovieInfo, ofn.lpstrFile);    
   	    SetProp(hWnd, (LPSTR) szMovieInfo,hglb);    
	    SetProp(pMovieInfo->hwndMovie, (LPSTR) szMovieInfo, hglb);
{ char a[256];wsprintf(a,"wnd1 %X wnd2 %X\n",hWnd,pMovieInfo->hwndMovie); OutputDebugString(a);}
    }
    //playMovie(pMovieInfo, 0);		// screws up drawproc for some reason
	{
		MCI_DGV_SETVIDEO_PARMS	dgv;
		UINT					uDevice;
		dgv.dwValue = (DWORD) ICAVIDrawProc;
			//MakeProcInstance((FARPROC) ICAVIDrawProc,hInstApp);
		dgv.dwItem = MCI_AVI_SETVIDEO_DRAW_PROCEDURE;
		uDevice = pMovieInfo->wMCIDeviceID;
		if (uDevice)
		{
			DWORD dw;
								
			dw = mciSendCommand(uDevice,
			MCI_SETVIDEO,
			MCI_DGV_SETVIDEO_ITEM | MCI_DGV_SETVIDEO_VALUE,
			(DWORD) (MCI_DGV_SETVIDEO_PARMS FAR *)&dgv);
			OutputDebugString("set draw proc!\n");
			if (dw != 0)
			{
				MessageBox(GetFocus(),
					"The currently installed MCIAVI does not "
					"support the MCI_AVI_SETVIDEO_DRAW_PROCEDURE "
					"command during play.","MCI Problem",
					MB_OK | MB_ICONHAND);
			}
		}
		else
		{
			MessageBox(GetFocus(),"movie info has no device id",
				"real bummer",MB_OK);
		}
	}    
    setMovie(pMovieInfo, pMovieInfo->dwCurrentFrame, NULL);
    {
    RECT rcClient, rcWindow;      //rcMovieBounds, rc
    MCI_DGV_RECT_PARMS   mciRect;
    BOOL retcode = TRUE;
    
    /* if there is no movie yet then just get out of here */
    GetClientRect(hWnd, &rcClient);  /* get the parent windows rect */
    GetWindowRect(hWnd, &rcWindow);
    mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_WHERE,
                  (DWORD)(MCI_DGV_WHERE_SOURCE),
                  (DWORD)(LPMCI_DGV_RECT_PARMS)&mciRect);
    
    }
    
    pMovieInfo->fPlaying = FALSE;

    bFileLoaded = TRUE;
    wsprintf(szBuf, "%ld of %ld", pMovieInfo->dwCurrentFrame,
                    pMovieInfo->dwMovieLength);
    SetDlgItemText(hwndDlg, ID_CURRENTFRAME, szBuf);
    
    // now try opening a HOT file
    
    memset(szFileName, 0, sizeof(szFileName));
    memset(szFileTitle, 0, sizeof(szFileTitle));
    memset(&ofn, 0, sizeof(OPENFILENAME));
    
    ofn.lStructSize       = sizeof(OPENFILENAME);
    ofn.hwndOwner         = hWnd;
    ofn.lpstrFilter       = szFilterSpec2;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter    = 0;
    ofn.nFilterIndex      = 1;
    ofn.lpstrFile         = szFileName;
    ofn.nMaxFile          = MAXFILENAME;
    ofn.lpstrInitialDir   = NULL;
    ofn.lpstrFileTitle    = szFileTitle;
    ofn.nMaxFileTitle     = MAXFILENAME;
    ofn.lpstrTitle        = "Open Hotspot File";
    ofn.lpstrDefExt       = "HOT";
    ofn.Flags             = 0;
    
    if (!GetOpenFileName ((LPOPENFILENAME)&ofn))
            return TRUE;
    
    lstrcpy(pMovieInfo->szIniFileName, ofn.lpstrFile);
    InitHotspots(pMovieInfo, ofn.lpstrFile);
    if (pMovieInfo->pHotspotList && pMovieInfo->hwndMovie)                    
        {
        HDC hDC;
        
        hDC = GetDC(pMovieInfo->hwndMovie);
        DrawRects(hDC, pMovieInfo);            
        ReleaseDC(pMovieInfo->hwndMovie, hDC);
        }
    return TRUE;
}

/*
	FileSave -- saves hotspot into file.  Uses GetSaveFileName to prompt
		user for filename if the MOVIEINFO doesn't have a filename, or
		if bGetFileName is set.
*/
BOOL FileSave(HWND hWnd, BOOL bGetFileName)
{
	int hotcount;
	HFILE hFile;
    PHOTSPOT pHotspot;
    static OPENFILENAME ofn;
    static OFSTRUCT of;
    char szFileTitle[MAXFILENAME] = "";
    char szFileName[MAXFILENAME] = "";
    char szFilterSpec [128] =                       /* file type filters */
             "Hotspot Files (*.HOT)\0*.HOT\0All Files (*.*)\0*.*\0";
    BOOL bGet = TRUE;

    if (pMovieInfo->szIniFileName)
        {
        if (lstrlen(pMovieInfo->szIniFileName))            
            {
            bGet = FALSE;            
            lstrcpy(szFileName, pMovieInfo->szIniFileName);
            }
        }
        
    if (bGetFileName)
        bGet = TRUE;
    if (bGet)
        {
        ofn.lStructSize       = sizeof(OPENFILENAME);
        ofn.hwndOwner     = hWnd;
        ofn.lpstrFilter   = szFilterSpec;
        ofn.lpstrCustomFilter = NULL;
        ofn.nMaxCustFilter    = 0;
        ofn.nFilterIndex      = 1;
        ofn.lpstrFile         = pMovieInfo->szIniFileName;
        ofn.nMaxFile          = MAXFILENAME;
        ofn.lpstrInitialDir   = NULL;
        ofn.lpstrFileTitle    = szFileTitle;
        ofn.nMaxFileTitle     = MAXFILENAME;
        ofn.lpstrTitle        = NULL;
        ofn.lpstrDefExt       = "HOT";
        ofn.Flags             = OFN_OVERWRITEPROMPT;
    
        if (!GetSaveFileName ((LPOPENFILENAME)&ofn))
            {
            DWORD dw;
            dw = CommDlgExtendedError();
            }
        }    
    hotcount = 0;
    pHotspot = pMovieInfo->pHotspotList;
    while (pHotspot)	// we'll save a hotspot count in the file to
    					// make life easy for the load routine.
        {
        hotcount++;
        pHotspot = pHotspot->pNext;
        }

    pHotspot = pMovieInfo->pHotspotList;
    
    if (bGet)
        {
        lstrcpy(pMovieInfo->szIniFileName, ofn.lpstrFile);
        lstrcpy(szFileName, ofn.lpstrFile);
        }

    hFile = _lcreat(szFileName,0);
    if (!hFile == HFILE_ERROR)
    {
    	MessageBox(GetFocus(),szFileName,"Can't write this file:",MB_OK);
    }
    
	_lwrite(hFile,"HH",2);
	_lwrite(hFile,&hotcount,sizeof(hotcount));
        
    while (pHotspot)
    {
        int x;
        	
        _lwrite(hFile,&pHotspot->rc.left,sizeof(pHotspot->rc.left));
        _lwrite(hFile,&pHotspot->rc.top,sizeof(pHotspot->rc.top));
        _lwrite(hFile,&pHotspot->rc.right,sizeof(pHotspot->rc.right));
        _lwrite(hFile,&pHotspot->rc.bottom,sizeof(pHotspot->rc.bottom));
        
    	x = pHotspot->pszHotspotID ? lstrlen(pHotspot->pszHotspotID) : 0;
    	_lwrite(hFile,&x,sizeof(x));
    	if (x && pHotspot->pszHotspotID) _lwrite(hFile,pHotspot->pszHotspotID,x);
        
		x = pHotspot->pszCommand ? lstrlen(pHotspot->pszCommand) : 0;
    	_lwrite(hFile,&x,sizeof(x));
    	if (x && pHotspot->pszCommand) _lwrite(hFile,pHotspot->pszCommand,x);

        _lwrite(hFile,&pHotspot->BeginFrame,sizeof(pHotspot->BeginFrame));
        _lwrite(hFile,&pHotspot->EndFrame,sizeof(pHotspot->EndFrame));
        _lwrite(hFile,&pHotspot->OnClick,sizeof(pHotspot->OnClick));
        _lwrite(hFile,&pHotspot->ToFrame,sizeof(pHotspot->ToFrame));
        pHotspot = pHotspot->pNext;            
    }
	_lclose(hFile);
    bModified = FALSE;           
    
    lstrcpy(szWindowTitle, szBaseWindowTitle);
    lstrcat(szWindowTitle, " - ");
    lstrcat(szWindowTitle, szFileTitle);
    SetWindowText(hWnd, szWindowTitle);
    
    return TRUE;
}                
                                  

