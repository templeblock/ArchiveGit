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
	movie.c:
		positionMovie -- puts movie in upper left corner of window
		fileCloseMovie -- closes movie & clears fPlaying and fMovieOpen
		GetMovieLength -- returns number of frames in movie
		GetMovieFrame -- returns current frame of movie
		fileOpenMovie -- opens an AVI movie, optionally using a common
			dialog box.  Movie is displayed paused when opened.
		playMovie -- plays or pauses the movie
		setMovie -- sets movie to specified frame
		seekMovie -- go to beginning or end of movie
		stepMovie -- go forwards or backwards one frame
*/

#include <windows.h>
#include <mmsystem.h>
#include <digitalv.h>
#include <mciavi.h>

#include "hotspot.h"

LONG FAR PASCAL _export ICAVIDrawProc(DWORD id, HDRVR hDriver,
	UINT uiMessage, LPARAM lParam1, LPARAM lParam2);

/*
	positionMovie -- gets the client rect of the parent window, gets the
		size of the movie with MCI_WHERE, sets up a rect for the movie
		to go in, and calls MoveWindow to move the child window,
		and the movie, there.
*/
VOID positionMovie(PMOVIEINFO pMovieInfo, SHORT x, SHORT y)
{
	RECT rcClient, rcMovieBounds;
	MCI_DGV_RECT_PARMS   mciRect;
	BOOL retcode = TRUE;
	
	if (!pMovieInfo->fMovieOpen)
		return;
	GetClientRect(pMovieInfo->hwndParent, &rcClient);
	
	mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_WHERE,
		(DWORD)(MCI_DGV_WHERE_SOURCE),
		(DWORD)(LPMCI_DGV_RECT_PARMS)&mciRect);
		
	rcMovieBounds = mciRect.rc;
	pMovieInfo->rcMovie.left = x;
	pMovieInfo->rcMovie.top = y;
	pMovieInfo->rcMovie.right = pMovieInfo->rcMovie.left + rcMovieBounds.right +1;
	pMovieInfo->rcMovie.bottom = pMovieInfo->rcMovie.top + rcMovieBounds.bottom +1;
	
	MoveWindow(pMovieInfo->hwndMovie, pMovieInfo->rcMovie.left, pMovieInfo->rcMovie.top,
		rcMovieBounds.right, rcMovieBounds.bottom, TRUE);
}
/*
	fileCloseMovie -- uses MCI_CLOSE to close the movie, and
		InvalidateRect to make the movie actually disappear from
		the screen.  DeleteHotspotList is called to get rid of the
		hotspots.
*/
void fileCloseMovie(PMOVIEINFO pMovieInfo, BOOL bUpdate)
{
	MCI_GENERIC_PARMS  mciGeneric;
	
	pMovieInfo->fPlaying = FALSE;
	pMovieInfo->fMovieOpen = FALSE;
	RemoveProp(pMovieInfo->hwndMovie, (LPSTR) "DataLo");
	RemoveProp(pMovieInfo->hwndMovie, (LPSTR) "DataHi");
	RemoveProp(pMovieInfo->hwndParent, (LPSTR) "DataLo");
	RemoveProp(pMovieInfo->hwndParent, (LPSTR) "DataHi");
	
	mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_CLOSE, MCI_WAIT,
		(DWORD)(LPMCI_GENERIC_PARMS)&mciGeneric);
	
	if (bUpdate)
	{
		InvalidateRect(pMovieInfo->hwndParent, NULL, TRUE);
		UpdateWindow(pMovieInfo->hwndParent);
	}
	DeleteHotspotList(pMovieInfo);
}

/*
	GetMovieLength -- uses MCI_STATUS to find out number of frames in
		movie.
*/
DWORD GetMovieLength(PMOVIEINFO pMovieInfo)
{
	MCI_STATUS_PARMS mciStatus;
	
	mciStatus.dwTrack = 1;
	mciStatus.dwCallback = NULL;
	mciStatus.dwItem = MCI_STATUS_LENGTH;
	
	if (0 != (mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_STATUS, (DWORD)MCI_STATUS_ITEM,
					(DWORD)(LPMCI_STATUS_PARMS)&mciStatus)))
	{
		return 0;
	}
	return (mciStatus.dwReturn);
}

/*
	GetMovieFrame -- uses MCI_STATUS to get current frame of movie.
*/
DWORD GetMovieFrame(PMOVIEINFO pMovieInfo)
{
	MCI_STATUS_PARMS mciStatus;
	
	mciStatus.dwTrack = 1;
	mciStatus.dwCallback = NULL;
	mciStatus.dwItem = MCI_STATUS_POSITION;
	
	if (0 != (mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_STATUS, (DWORD)MCI_STATUS_ITEM,
					(DWORD)(LPMCI_STATUS_PARMS)&mciStatus)))
	{
		return 0;
	}
	return (mciStatus.dwReturn);
}

/*
	fileOpenMovie --
		uses MCI_OPEN to open the AVI file. Saves the device ID so we
			can do stuff like find the frame size.
		uses MCI_WINDOW to show the playback window
		uses MCI_DGV_STATUS_HWND to get a handle to the window
		gets the movie length with GetMovieLength, and
		positions the movie with positionMovie.
		Uses InvalidateRect to get window repainted (which will show
		the first frame)
		
		Gives user an annoying messagebox if this doesn't work.
		
*/
void fileOpenMovie(PMOVIEINFO pMovieInfo, LPSTR szFilename, SHORT x, SHORT y)
{
	if (pMovieInfo == NULL)
	{
		return;
	}
	if (lstrlen(szFilename))
	{
		MCI_DGV_OPEN_PARMS mciOpen;
		MCI_DGV_WINDOW_PARMS   mciWindow;
		MCI_DGV_STATUS_PARMS   mciStatus;
		
		if (pMovieInfo->fMovieOpen)
			fileCloseMovie(pMovieInfo, TRUE);

		mciOpen.dwCallback = NULL;
		mciOpen.wDeviceID = mciOpen.wReserved0 =
		mciOpen.wReserved1 = 0;
		mciOpen.lpstrDeviceType = NULL;
		mciOpen.lpstrElementName = szFilename;
		mciOpen.lpstrAlias = NULL;
		mciOpen.dwStyle = WS_CHILD;
		mciOpen.hWndParent = pMovieInfo->hwndParent;
	
		if (mciSendCommand(0, MCI_OPEN,
					(DWORD)(MCI_OPEN_ELEMENT|MCI_DGV_OPEN_PARENT|MCI_DGV_OPEN_WS),
				(DWORD)(LPMCI_DGV_OPEN_PARMS)&mciOpen) == 0)
		{
			pMovieInfo->wMCIDeviceID = mciOpen.wDeviceID;
			pMovieInfo->fMovieOpen = TRUE;

			mciWindow.dwCallback = NULL;
			mciWindow.hWnd = pMovieInfo->hwndParent;
			mciWindow.wReserved1 = mciWindow.wReserved2 = 0;
			mciWindow.nCmdShow = SW_SHOW;
			mciWindow.lpstrText = (LPSTR)NULL;

			mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_WINDOW,
				MCI_DGV_WINDOW_STATE | MCI_DGV_WINDOW_HWND,
				(DWORD)(LPMCI_DGV_WINDOW_PARMS)&mciWindow);

			mciStatus.dwItem = MCI_DGV_STATUS_HWND;

			mciSendCommand(pMovieInfo->wMCIDeviceID,
				MCI_STATUS, MCI_STATUS_ITEM,
				(DWORD)(LPMCI_STATUS_PARMS)&mciStatus);

			pMovieInfo->hwndMovie = (HWND)mciStatus.dwReturn;
			pMovieInfo->dwMovieLength = GetMovieLength(pMovieInfo);
			pMovieInfo->dwCurrentFrame = 0;

			SetProp(pMovieInfo->hwndMovie, (LPSTR) "DataLo", LOWORD(pMovieInfo));
			SetProp(pMovieInfo->hwndMovie, (LPSTR) "DataHi", HIWORD(pMovieInfo));
			SetProp(pMovieInfo->hwndParent, (LPSTR) "DataLo", LOWORD(pMovieInfo));
			SetProp(pMovieInfo->hwndParent, (LPSTR) "DataHi", HIWORD(pMovieInfo));

			positionMovie(pMovieInfo,x,y);
			{
				MCI_DGV_SETVIDEO_PARMS	dgv;
				UINT					uDevice;
				
				dgv.dwValue = (DWORD) ICAVIDrawProc;
				dgv.dwItem = MCI_AVI_SETVIDEO_DRAW_PROCEDURE;
				uDevice = pMovieInfo->wMCIDeviceID;
				if (uDevice)
				{
					DWORD dw;
					
					dw = mciSendCommand(uDevice,
						MCI_SETVIDEO,
						MCI_DGV_SETVIDEO_ITEM | MCI_DGV_SETVIDEO_VALUE,
						(DWORD) (MCI_DGV_SETVIDEO_PARMS FAR *)&dgv);
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
		}
		else
		{
			MessageBox(pMovieInfo->hwndParent, "Unable to open Movie", NULL,
				MB_ICONEXCLAMATION|MB_OK);
			pMovieInfo->fMovieOpen = FALSE;
		}
	}
	InvalidateRect(pMovieInfo->hwndParent, NULL, FALSE);
	UpdateWindow(pMovieInfo->hwndParent);
	pMovieInfo->lLastSeek = 0;
}

/*
	playMovie -- looks at wDirection parameter and
		sends MCI a MCI_PLAY command (optionally with
			MCI_DGV_PLAY_REVERSE flag set) OR
		sends MCI a MCI_PAUSE command
*/
void playMovie(PMOVIEINFO pMovieInfo, WORD wDirection)
{
	pMovieInfo->fPlaying = !pMovieInfo->fPlaying;
	if (wDirection == NULL)
		pMovieInfo->fPlaying = FALSE;
	if (pMovieInfo->fPlaying)
	{
		MCI_DGV_PLAY_PARMS   mciPlay;
		DWORD dwFlags = MCI_NOTIFY;
		
		mciPlay.dwCallback = MAKELONG(pMovieInfo->hwndMovie,0);
		mciPlay.dwFrom = mciPlay.dwTo = 0;
		if (wDirection == 2)
			dwFlags |= MCI_DGV_PLAY_REVERSE;
			
		mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_PLAY, dwFlags,
			(DWORD)(LPMCI_DGV_PLAY_PARMS)&mciPlay);
	}
	else
	{
		MCI_DGV_PAUSE_PARMS  mciPause;
		
		mciSendCommand(pMovieInfo->wMCIDeviceID,MCI_PAUSE,MCI_WAIT,
			(DWORD)(LPMCI_DGV_PAUSE_PARMS)&mciPause);
	}
}

/*
	setMovie -- stops movie if it is playing,
		sends MCI an MCI_SEEK command to the frame number passed in.
*/
void setMovie(PMOVIEINFO pMovieInfo, DWORD dwFrame, HWND hCallback)
{
	MCI_SEEK_PARMS mciSeek;
	
	if (pMovieInfo->fPlaying)
	{
		playMovie(pMovieInfo, NULL);
	}
	mciSeek.dwTo = dwFrame;
	mciSeek.dwCallback = MAKELONG(pMovieInfo->hwndMovie,0);
	
	mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_SEEK, MCI_TO | MCI_WAIT,
		(DWORD)(LPMCI_SEEK_PARMS)&mciSeek);
	pMovieInfo->lLastSeek = dwFrame;
}

/*
	seekMovie -- stops movie if playing, then goes to beginning or
		end of movie. wAction == 0 means beginning, == 1 means end.
		it does this my using MCI_SEEK with MCI_SEEK_TO_START or
		MCI_SEEK_TO_END.
*/
void seekMovie(PMOVIEINFO pMovieInfo, WORD wAction)
{
	if (pMovieInfo->fPlaying)
	{
		playMovie(pMovieInfo, NULL);
	}
	if (wAction == 0)
	{
		mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_SEEK, MCI_SEEK_TO_START,
			(DWORD)(LPVOID)NULL);
		pMovieInfo->lLastSeek = 0;
	}
	else if (wAction == 1)
	{
		mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_SEEK, MCI_SEEK_TO_END,
			(DWORD)(LPVOID)NULL);
	}
}

/*
	stepMovie -- stops movie if playing, then steps one frame:
		wDirection == MCI_STEP means forward, anything else means reverse.
		it does this by using MCI_STEP command with MCI_DGV_STEP_FRAMES,
		optionally with MCI_DGV_STEP_REVERSE flag set.
*/
void stepMovie(PMOVIEINFO pMovieInfo, HWND hWnd, WORD wDirection)
{
	MCI_DGV_STEP_PARMS   mciStep;
	
	if (pMovieInfo->fPlaying)
		playMovie(pMovieInfo, NULL);

	mciStep.dwFrames = 1L;

	if (wDirection == MCI_STEP)
	{
		mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_STEP, MCI_DGV_STEP_FRAMES,
			(DWORD)(LPMCI_DGV_STEP_PARMS)&mciStep);
	}
	else
	{
		mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_STEP,
			MCI_DGV_STEP_FRAMES|MCI_DGV_STEP_REVERSE,
			(DWORD)(LPMCI_DGV_STEP_PARMS)&mciStep);
	}
}
