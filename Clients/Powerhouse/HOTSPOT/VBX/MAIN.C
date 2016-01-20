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
	main.c:
		LibMain -- called by windows loader
		FireAdjustSize -- fires a VB event when window is resized
		FireHspClick -- fires a VB event telling app
			which hotspot was clicked
		FireAVIDone -- tells VB app that the AVI is done playing.
		VBINITCC -- tells VB app our properties, events, and ctlproc
		VBTERMCC -- called right before VBX is unloaded
		AVICtlProc -- responds to messages such as setting properties
			or mouse moves
*/
#include <windows.h>
#include <mmsystem.h>
#include <digitalv.h>
#include <string.h>

#include "hotspot.h"
#include "avivbx.h"

// forward declars...
void Terminate(PMOVIEINFO pMovieInfo);
void InitMovieInfo();
int GetFreeMovieInfo();
void DestroyMovieInfo(int i);
PMOVIEINFO PMovieInfoFromHCtl(HCTL hctl);
static int cVbxUsers = 0;
HANDLE hmodDLL;
static BOOL bInit = FALSE;
extern MOVIEINFO FAR MovieInfo[10];

// LibMain just grabs our module handle
int FAR PASCAL LibMain
(
	HANDLE hModule,
	WORD   wDataSeg,
	WORD   cbHeapSize,
	LPSTR  lpszCmdLine
)
{
	wDataSeg	= wDataSeg;
	cbHeapSize	= cbHeapSize;
	lpszCmdLine = lpszCmdLine;
	hmodDLL = hModule;
	return TRUE;
}

// Here we put the new size into a structure and give it to
// VB with VBFireEvent.
VOID NEAR FireAdjustSize(HCTL hctl, WORD cx, WORD cy)
{
	SIZEADJUSTPARAMS params;
	
	params.CX = &cx;
	params.CY = &cy;
	VBFireEvent(hctl, IEVENT_AVI_SIZEADJUST, &params);
	return;
}

// We already know what hotspot was clicked, so we just pass its
// ID (which is a string set by the user in the hotspot editor)
// on to VB.  We have to call VB to create the string and destroy
// the string (after we fire the event) because VB uses different
// strings that we do.
VOID NEAR FireHspClick(HCTL hctl, PHOTSPOT pHotspot)
{
	HSPCLICKPARAMS params;
	
	params.hlszHotspotID = VBCreateHlstr(pHotspot->pszHotspotID,
		lstrlen(pHotspot->pszHotspotID));
	VBFireEvent(hctl, IEVENT_AVI_HSPCLICK, &params);
	VBDestroyHlstr(params.hlszHotspotID);
	return;
}

// Just fire the event, no params...
VOID NEAR FireAVIDone(HCTL hctl)
{
	AVIDONEPARAMS params;
	
	VBFireEvent(hctl,IEVENT_AVI_AVIDONE,&params);
	return;
}

// This is where we give VB our "Model" structure.  It tells VB all our
// properties, events, and gives VB a pointer to our control proc.
// We also call initAVI to open our AVI device.
BOOL FAR PASCAL _export VBINITCC(USHORT usVersion, BOOL fRuntime)
{
	fRuntime  = fRuntime;
	usVersion = usVersion;
	++cVbxUsers;
	initAVI();
	return VBRegisterModel(hmodDLL, &modelAvihot);
}

// We call termAVI to close our AVI device.  We don't have to do
// anything for VB.
VOID FAR PASCAL _export VBTERMCC(VOID)
{
	--cVbxUsers;
	termAVI();
	return;
}

// Here's our control proc.  It's just like a window proc, except VB
// also sends us some messages starting with VBM_.  More comments by
// each message.
LONG FAR PASCAL _export AVICtlProc (
	HCTL   hctl,
	HWND   hwnd,
	USHORT msg,
	USHORT wp,
	LONG   lp
)
{
	PMOVIEINFO pMovieInfo;

	switch (msg)
	{
	// Adds us to an array of MOVIEINFOs maintained by this VBX.  We
	// use PMovieInfoFromHCtl to get them back.
	case WM_CREATE:
		{
			int i;
			
			i = GetFreeMovieInfo();
			MovieInfo[i].bLoading = TRUE;
			MovieInfo[i].hctl = hctl;
			MovieInfo[i].bShowHot = FALSE;
		}
		break;
	// If the picture is still, and gets invalidated, it won't be properly
	// refreshed unless we tell MCI to MCI_UPDATE the window.
	case WM_PAINT:
		{
			HDC hDC;
			PAINTSTRUCT ps;
			MCI_DGV_UPDATE_PARMS mciUpdate;
			
			hDC = BeginPaint(hwnd, &ps);
			pMovieInfo = PMovieInfoFromHCtl(hctl);
			if (pMovieInfo)
			{
				mciUpdate.hDC = hDC;
				mciUpdate.rc = ps.rcPaint;
				mciUpdate.wReserved0 = 0;
				mciUpdate.dwCallback = NULL;
				mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_UPDATE,
					MCI_WAIT | MCI_DGV_RECT | MCI_DGV_UPDATE_PAINT | MCI_DGV_UPDATE_HDC,
					(DWORD)(LPMCI_DGV_UPDATE_PARMS)&mciUpdate);
			}
			EndPaint(hwnd, &ps);
		}
		return TRUE;
	// Button down!  Could mean a hotspot was clicked!
	case WM_LBUTTONDOWN:
		pMovieInfo = PMovieInfoFromHCtl(hctl);
		if (0 == (OnButtonDown(pMovieInfo, msg, wp, lp)))
		{
			Terminate(pMovieInfo);
			return 0L;
		}
		break;
	// Oh, it's over!  Let's close it and tell VB.
	case MM_MCINOTIFY:
		switch(wp)
		{
		case MCI_NOTIFY_SUCCESSFUL:
			{
				pMovieInfo = PMovieInfoFromHCtl(hctl);
				pMovieInfo->fPlaying = FALSE;
				Terminate(pMovieInfo);
				VBInvalidateRect(hctl, NULL, TRUE);
				VBUpdateControl(hctl);
				FireAVIDone(hctl);
				return 0L;
			}
		}
		break;
	// we're gone -- close up shop and nuke the hotspot list.
	case WM_DESTROY:
		{
			pMovieInfo = PMovieInfoFromHCtl(hctl);
			if (pMovieInfo)
			{
				Terminate(pMovieInfo);
				DeleteHotspotList(pMovieInfo);
				pMovieInfo->hctl = NULL;
			}
			if (pCtlInfoDEREF(hctl)->hszAVIFile)
				VBDestroyHsz(pCtlInfoDEREF(hctl)->hszAVIFile);
			if (pCtlInfoDEREF(hctl)->hszHotFile)
				VBDestroyHsz(pCtlInfoDEREF(hctl)->hszHotFile);
		}
		break;
	// this does nothing.
	case VBM_INITIALIZE:
		break;
	// We're loaded, lets remember our handle.
	case VBM_LOADED:
		{
			pMovieInfo = PMovieInfoFromHCtl(hctl);
			pMovieInfo->bLoading = FALSE;
		}
		break;
	// Here's where all the work is done...
	case VBM_SETPROPERTY:
		// get our instance info
		pMovieInfo = PMovieInfoFromHCtl(hctl);
		if (pMovieInfo)
		{
			pCtlInfoDEREF(hctl)->lLength = (LONG) GetMovieLength(pMovieInfo);
			pCtlInfoDEREF(hctl)->lFrame = (LONG) GetMovieFrame(pMovieInfo);
		}
		switch (wp)
		{
		// AVIFile set, let's load the file and resize the window to its
		// coordinates.  To do that, we pass its coordinates, converted
		// from twips, to fileOpenMovie.  We also delete the previous
		// hotspot list so hotspots don't accumulate.
		case IPROP_AVI_AVIFILE:
			{
				LPSTR lpstr = VBDerefHsz(pCtlInfoDEREF(hctl)->hszAVIFile);
				if (lpstr && lstrlen(lpstr))
				{
					float xtwips, ytwips;
					SHORT xpxls, ypxls;
					
					lstrcpy(pMovieInfo->szAVIFileName, lpstr);
					pMovieInfo->hwndParent = hwnd;
					VBGetControlProperty(hctl,(USHORT) IPROP_AVI_LEFT,&xtwips);
					VBGetControlProperty(hctl,(USHORT) IPROP_AVI_TOP,&ytwips);
					xpxls = VBXTwipsToPixels((LONG)xtwips);
					ypxls = VBYTwipsToPixels((LONG)ytwips);
					DeleteHotspotList(pMovieInfo);
					fileOpenMovie(pMovieInfo, pMovieInfo->szAVIFileName,xpxls,ypxls);
				}
			}
			break;
		// HOTFile was set, let's load in a hotspot list.
		case IPROP_AVI_HOTFILE:
			{
				LPSTR lpstr = VBDerefHsz(pCtlInfoDEREF(hctl)->hszHotFile);
				
				pMovieInfo = PMovieInfoFromHCtl(hctl);
				if (lpstr && lstrlen(lpstr))
				{
					lstrcpy(pMovieInfo->szHotFileName, lpstr);
					InitHotspots(pMovieInfo, pMovieInfo->szHotFileName);
				}
			}
			break;
		// Play was set.  The play property can play (==1), pause (==0)
		// or play in reverse (==2) the video.  Later we added close (==3)
		// because this was the simplest way to do it...  So we call
		// Terminate to close, and playMovie for everything else.
		case IPROP_AVI_PLAY:
			{
				WORD wPlay;
				
				wPlay = (WORD) pCtlInfoDEREF(hctl)->fPlay;
				if (wPlay > 2)
				{
					Terminate(pMovieInfo);
					VBInvalidateRect(hctl, NULL, TRUE);
					VBUpdateControl(hctl);
				}
				else
				{
					pMovieInfo = PMovieInfoFromHCtl(hctl);
					playMovie(pMovieInfo, wPlay);
				}
			}
			break;
		// Frame property set -- that means we should seek to that frame!
		// we get the new number, and call setMovie.
		case IPROP_AVI_FRAME:
			{
				LONG lFrame;
				
				lFrame = pCtlInfoDEREF(hctl)->lFrame;
				pMovieInfo = PMovieInfoFromHCtl(hctl);
				if (pMovieInfo) if (pMovieInfo->fMovieOpen)
				{
					setMovie(pMovieInfo, (DWORD) lp,NULL);
				}
			}
			break;
		// Showhot property changed -- change the flag in the MovieInfo
		// structure that tells the custom drawproc whether to draw the
		// hotspots or not.
		case IPROP_AVI_SHOWHOT:
			{
				pMovieInfo = PMovieInfoFromHCtl(hctl);
				pMovieInfo->bShowHot = pCtlInfoDEREF(hctl)->nShowHot;
			}
			break;
		}
		break;
	}
	return VBDefControlProc(hctl, hwnd, msg, wp, lp);
}
