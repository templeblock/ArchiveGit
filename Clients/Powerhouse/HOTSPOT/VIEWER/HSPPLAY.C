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
	hspplay.c:
		hspPlayAVI -- plays an AVI given a window, an AVI file,
			and an INI file
*/
#include <windows.h>
#include <mmsystem.h>
#include <digitalv.h>
#include <viewer.h>
#include <string.h>
#include <direct.h>

#include "hotspot.h"
#include "mciavi.h"

extern char szMovieInfo[];

LONG FAR PASCAL _export ICAVIDrawProc(DWORD id, HDRVR hDriver,
	UINT uiMessage, LPARAM lParam1, LPARAM lParam2);

/*
	hspPlayAVI -- allocates a movieinfo structure, stores the window
		handle you gave it in the hwndParent entry in the movieinfo,
		and stores the callback you provide in the movieinfo -- these will
		be used by the subclassproc.

		It next calls InitHotspots to get a hotspot list, and
		fileOpenMovie which will open the AVI file and create a child
		window (a child of the window that you pass in to this function).
		The window is returned in the movieinfo.  It then subclasses
		the window fileOpenMovie made, by changing the pointer to its
		WindowProc to ours (via SetWindowLong).

		It uses SetProp to hook this movieinfo up to both windows.
		The subclassed wndproc will get the movieinfo from the child
		window when a button is clicked, and the parent windowproc will
		get the movieinfo from the parent when Viewer wants to know
		the pane size.

		Finally, playMovie is called to start the movie.
*/
LONG CALLBACK __export hspPlayAVI(HWND hwnd, LPSTR szAVIFile, 
					LPSTR szIniFile, FARPROC lpfnhspAVICallback)
{
    HGLOBAL hglb;
    PMOVIEINFO pMovieInfo;
    
    hglb = GlobalAlloc(GHND, sizeof(MOVIEINFO));
    pMovieInfo = (PMOVIEINFO)GlobalLock(hglb);
    
    if (!pMovieInfo)
        return (0L);
    
    pMovieInfo->hwndParent = hwnd;
    pMovieInfo->lpfnhspAVICallback = lpfnhspAVICallback;
    
    if (!hwnd)
        {
        Message("Invalid window specified for playing AVI file.");
        return 0L;
        }
        
    InitHotspots(pMovieInfo, szIniFile);
    
	OutputDebugString("calling fileOpenMovie\n");
    fileOpenMovie(pMovieInfo, szAVIFile);
    GlobalUnlock(hglb);
   
    pMovieInfo->lpfnOldProc = (FARPROC) GetWindowLong (pMovieInfo->hwndMovie, GWL_WNDPROC);
    SetWindowLong (pMovieInfo->hwndMovie, GWL_WNDPROC, (LONG) SbClsProc);

    SetProp(pMovieInfo->hwndMovie, (LPSTR) szMovieInfo, hglb);
    SetProp(hwnd, (LPSTR) szMovieInfo, hglb);    
    
	OutputDebugString("calling positionMovie\n");
    positionMovie(pMovieInfo);
	OutputDebugString("calling playMovie\n");
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
		playMovie(pMovieInfo, 1);
	}    
    return (0L);
}

