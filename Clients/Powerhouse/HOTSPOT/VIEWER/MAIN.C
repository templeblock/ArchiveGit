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
		HotspotWP -- WindowProc for viewer pane
		LibMain -- called when DLL is loaded
		LDLLHandler -- gets module-related messages from Viewer
		WEP -- called when DLL is unloaded
*/
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <digitalv.h>
#include <viewer.h>
#include "hotspot.h"

extern char szMovieInfo[];

LPSTR szHotspotClassName = "Hotspot";
HANDLE hModule;

/*
	HotspotWP -- this is the WindowProc for the viewer pane.
		Answers the WM_CREATE and EWM_QUERYSIZE messages -- see
		comments next to the messages.
*/
LONG CALLBACK HotspotWP(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LPEWDATA lpew;
	LPCREATESTRUCT lpCreate;
	LPSTR szAviFile, szIniFile;

	switch (msg)
	{
	/* WM_CREATE -- filenames to use were handed in on the
		LPCREATESTRUCT, so we have to unpack them.  That's what
		all this string stuff does.  Once we have the filenames,
		we just hand them to hspPlayAVI, which handles all the
		subclassing and stuff for us.
	*/
	case WM_CREATE:
		lpCreate = (LPCREATESTRUCT)lParam;
		lpew = (LPEWDATA)lpCreate->lpCreateParams;

		szIniFile = lpew->szAuthorData;
		while (*szIniFile != ',') szIniFile++;
		*szIniFile = '\0';
		szIniFile++;
		szAviFile = lpew->szAuthorData;
		if ((*szAviFile == '\140') || *szAviFile == '\042' );
			szAviFile++; 
		if ((szIniFile[lstrlen(szIniFile)-1] == '\047') ||
		    (szIniFile[lstrlen(szIniFile)-1] == '\042') )
			szIniFile[lstrlen(szIniFile)-1] = '\0';
        hspPlayAVI(hwnd, szAviFile,szIniFile,NULL);
		//{
			//PMOVIEINFO pMovieInfo;
			//HANDLE hglb;
		    
			//hglb = GetProp(hwnd, (LPSTR)szMovieInfo);
			//if (hglb)
	        //{
		    	//pMovieInfo = (PMOVIEINFO)GlobalLock(hglb);
		 		//if (pMovieInfo && pMovieInfo->fMovieOpen) 
				//{
					//MCI_DGV_SETVIDEO_PARMS	dgv;
					//UINT					uDevice;
					//dgv.dwValue = (DWORD)NULL;//ICAVIDrawProc;
						////MakeProcInstance((FARPROC) ICAVIDrawProc,hInstApp);
					//dgv.dwItem = MCI_AVI_SETVIDEO_DRAW_PROCEDURE;
					//uDevice = pMovieInfo->wMCIDeviceID;
					//if (uDevice)
					//{
						//DWORD dw;
								
						//dw = mciSendCommand(uDevice,
						//MCI_SETVIDEO,
						//MCI_DGV_SETVIDEO_ITEM | MCI_DGV_SETVIDEO_VALUE,
						//(DWORD) (MCI_DGV_SETVIDEO_PARMS FAR *)&dgv);
						//OutputDebugString("set draw proc!\n");
						//if (dw != 0)
						//{
							//MessageBox(GetFocus(),
								//"The currently installed MCIAVI does not "
								//"support the MCI_AVI_SETVIDEO_DRAW_PROCEDURE "
								//"command during play.","MCI Problem",
								//MB_OK | MB_ICONHAND);
						//}
					//}
					//else
					//{
						//MessageBox(GetFocus(),"movie info has no device id",
							//"real bummer",MB_OK);
					//}
					//playMovie(pMovieInfo, 1);
				//}
				//else
				//{
					//OutputDebugString(
						//"no pMovieInfo && pMovieInfo->fMovieOpen\n");
				//}
			//}
			//else
			//{
				//OutputDebugString("NO WINDOW PROPERTY!!\n");
			//}
		//}
		return 0;
	/* EWM_QUERYSIZE -- the viewer wants to know the pane size.
		hspPlayAVI already glued a handle to the movieinfo structure
		onto this window with SetProp. So, we use GetProp to get the
		movieinfo, which has a deviceID we can then use to ask
		MCI for the size numbers.(with the MCI_WHERE) command.
	 */
	case EWM_QUERYSIZE:
		dbg("HotspotWP EWM_QUERYSIZE");
	{
		LPPOINT lpPoint = (LPPOINT) lParam;
		HANDLE hglb;
		PMOVIEINFO pMovieInfo;
		MCI_DGV_RECT_PARMS   mciRect;
    
		hglb = GetProp(hwnd, (LPSTR)szMovieInfo);
		if (!hglb)
        {
        	return FALSE;
        }
    	pMovieInfo = (PMOVIEINFO)GlobalLock(hglb);
 		if (!pMovieInfo->fMovieOpen) 
		{
			GlobalUnlock(hglb);
			return FALSE;
		}
		mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_WHERE, 
                  (DWORD)(MCI_DGV_WHERE_SOURCE), 
                  (DWORD)(LPMCI_DGV_RECT_PARMS)&mciRect);
		lpPoint->x = mciRect.rc.right - mciRect.rc.left; 
		lpPoint->y = mciRect.rc.bottom - mciRect.rc.top; 
		GlobalUnlock(hglb);
		return TRUE;
	}
	/* now we just refuse a few viewer questions... */
	case EWM_ASKPALETTE:
		return NULL;
	case EWM_RENDER:
		return NULL;
	case EWM_COPY:
		return NULL;
	case EWM_PRINT:
		return FALSE;
	case WM_PARENTNOTIFY:
		OutputDebugString("WM_PARENTNOTIFY !!!!!\n");
		if (wParam == WM_LBUTTONDOWN)
		{
			OutputDebugString("WM_LBUTTONDOWN even !!!!!\n");
		}
		else
		{
			{char x[256]; wsprintf(x,"value %X\n",wParam);
				OutputDebugString(x);   }
			//if (wParam == 1)
			//{
				//PMOVIEINFO pMovieInfo;
				//HANDLE hglb;
		    
				//hglb = GetProp(hwnd, (LPSTR)szMovieInfo);
				//if (hglb)
		        //{
			    	//pMovieInfo = (PMOVIEINFO)GlobalLock(hglb);
			 		//if (pMovieInfo && pMovieInfo->fMovieOpen) 
					//{
						//MCI_DGV_SETVIDEO_PARMS	dgv;
						//UINT					uDevice;
						//dgv.dwValue = (DWORD)ICAVIDrawProc;
							////MakeProcInstance((FARPROC) ICAVIDrawProc,hInstApp);
						//dgv.dwItem = MCI_AVI_SETVIDEO_DRAW_PROCEDURE;
						//uDevice = pMovieInfo->wMCIDeviceID;
						//if (uDevice)
						//{
							//DWORD dw;
								
							//dw = mciSendCommand(uDevice,
							//MCI_SETVIDEO,
							//MCI_DGV_SETVIDEO_ITEM | MCI_DGV_SETVIDEO_VALUE,
							//(DWORD) (MCI_DGV_SETVIDEO_PARMS FAR *)&dgv);
							//OutputDebugString("set draw proc!\n");
							//if (dw != 0)
							//{
								//MessageBox(GetFocus(),
									//"The currently installed MCIAVI does not "
									//"support the MCI_AVI_SETVIDEO_DRAW_PROCEDURE "
									//"command during play.","MCI Problem",
									//MB_OK | MB_ICONHAND);
							//}
						//}
						//else
						//{
							//MessageBox(GetFocus(),"movie info has no device id",
								//"real bummer",MB_OK);
						//}
					//}
					//else
					//{
						//OutputDebugString(
							//"no pMovieInfo && pMovieInfo->fMovieOpen\n");
					//}
				//}
				//else
				//{
					//OutputDebugString("NO WINDOW PROPERTY!!\n");
				//}
			//}
			break;
		//case MCIWNDM_NOTIFYSIZE:
			//OutputDebugString("MCIWNDM_NOTIFYMEDIA\n");
			//if (hwndMCI == 0)
			//{
				//hwndMCI = (HWND) wParam;
			//}
			//GetWindowRect(hwndMCI, &rc);
			//AdjustWindowRect(&rc, GetWindowLong(hwnd, GWL_STYLE), TRUE);
			//SetWindowPos(hwnd, NULL, 0, 0, rc.right - rc.left,
				//rc.bottom - rc.top,
				//SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
			//break;
		}
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return FALSE;
}

/*
	LibMain -- sets global hModule variable, and registers the
		window class for our viewer pane so Windows can get a pointer
		to our windowproc (i.e. hotspotWP above).  We also call
		initAVI.
*/
BOOL CALLBACK LibMain(HANDLE hLibrary, int cbHeap, LPSTR szCmdLine)
{
	WNDCLASS wc;

	hModule = hLibrary;
    initAVI();
	wc.lpszClassName = szHotspotClassName;
	wc.style = CS_GLOBALCLASS;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = NULL;
	wc.lpszMenuName = NULL;
	wc.hbrBackground = COLOR_WINDOW + 1;
	wc.hInstance = hModule;
	wc.lpfnWndProc = HotspotWP;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = NULL;

	return RegisterClass(&wc);
}

/*
	LDLLHandler -- a message handler that talks to viewer on behalf
		of our whole DLL (rather than one window of it)
		
		handles DW_WHATMSG, DW_INIT, DW_TERM. 
*/
LONG CALLBACK LDLLHandler(UINT msg, LONG lParam1, LONG lParam2)
{
    switch (msg)
    {
    
    case DW_WHATMSG:        
        return DC_INITTERM;
        
    case DW_INIT:    

		return TRUE; 
        
    case DW_TERM:
        return 0;
    }
	return 0;
}

 
/*
	WEP -- windows exit procedure
		Calls termAVI to close AVI device drivers
*/
int CALLBACK WEP(int nExitType)
{
    termAVI();    
    
    return (TRUE);
}	
