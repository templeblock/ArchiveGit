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
	    
		InitMovieInfo, GetFreeMovieInfo, DestroyMovieInfo,
			PMovieInfoFromHCtl -- maintain list of MovieInfos
		Terminate -- stops playing the movie and closes it
		OnButtonDown -- if clicked point on a hotspot,
			a viewer command is executed
		CleanupStuff -- removes properties from windows before they
			are destroyed
*/
#include <windows.h>
#include <mmsystem.h>
#include <digitalv.h>
#include <string.h>

#ifndef VB_VERSION
  #include <vbapi.h>
#endif

#include "hotspot.h"

VOID NEAR FireHspClick(HCTL hctl, PHOTSPOT pHotspot);
extern char szMovieInfo[];
MOVIEINFO FAR MovieInfo[10];
static BOOL bInit = FALSE;

// these 4 functions are for keeping an array of MovieInfos
// This one inits it...
void InitMovieInfo()
{
	int i;
	for (i = 0; i < 10; i++)
	MovieInfo[i].hctl = NULL;
}

// ...this one finds a place to add a new one...
int GetFreeMovieInfo()
{
	static int i = 0;
	while (MovieInfo[i].hctl != NULL && i < 10)
	{
		i++;
	}
	return (i);
}

// ...and this one removes one...
void DestroyMovieInfo(int i)
{
	MovieInfo[i].hctl = NULL;
}

HANDLE hmodDLL;

// ... and this one gets the movieinfo that goes with this control.
PMOVIEINFO PMovieInfoFromHCtl(HCTL hctl)
{
	int i = 0;
	while (MovieInfo[i].hctl != hctl && i < 10)
		++i;
	if (i == 10)
		return (NULL);
	else
		return (&MovieInfo[i]);
}

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
				FireHspClick(pMovieInfo->hctl, pHotspot);
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
LONG FAR PASCAL _export SbClsProc (HWND hwnd, USHORT msg,
	USHORT wp, LONG lp)
{
	HANDLE hglb;
	PMOVIEINFO pMovieInfo;
	LRESULT lr;
	hglb = GetProp(hwnd, (LPSTR)szMovieInfo);
	pMovieInfo = (PMOVIEINFO)GlobalLock(hglb);
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		if (0 == (OnButtonDown(pMovieInfo, msg, wp, lp)))
		{
			Terminate(pMovieInfo);
			return 0L;
		}
		break;
	case MM_MCINOTIFY:
		switch(wp)
		{
		case MCI_NOTIFY_SUCCESSFUL:
			pMovieInfo->fPlaying = FALSE;
			Terminate(pMovieInfo);
			return 0L;
		}
		break;
	case WM_DESTROY:
		lr = CallWindowProc(pMovieInfo->lpfnOldProc, hwnd, msg, wp, lp);
		RemoveProp(hwnd, (LPSTR)szMovieInfo);
		if (pMovieInfo)
			DeleteHotspotList(pMovieInfo);
		if (pMovieInfo)
			SetWindowLong (hwnd, GWL_WNDPROC,
			(LONG) pMovieInfo->lpfnOldProc);
		GlobalUnlock(hglb);
		GlobalFree(hglb);
		hglb = NULL;
		pMovieInfo = NULL;
		break;
	}
	if (pMovieInfo)
	{
		lr = CallWindowProc(pMovieInfo->lpfnOldProc, hwnd, msg, wp, lp);
		if (hglb)
			GlobalUnlock(hglb);
		return (lr);
	}
	else
		return 0L;
}
