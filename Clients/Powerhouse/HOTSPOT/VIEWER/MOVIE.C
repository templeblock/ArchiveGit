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
#include <viewer.h>

#include "hotspot.h"

/*
	positionMovie -- gets the client rect of the parent window, gets the
		size of the movie with MCI_WHERE, sets up a rect for the movie
		to go in, and calls MoveWindow to move the child window,
		and the movie, there.
*/
VOID positionMovie(PMOVIEINFO pMovieInfo)
{
   RECT rcClient, rcMovieBounds;
   MCI_DGV_RECT_PARMS   mciRect;
   BOOL retcode = TRUE;
   
   /* if there is no movie yet then just get out of here */
   if (!pMovieInfo->fMovieOpen)
       return;
   
   GetClientRect(pMovieInfo->hwndParent, &rcClient);  /* get the parent windows rect */
       
   /* get the original size of the movie */
   mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_WHERE, 
                  (DWORD)(MCI_DGV_WHERE_SOURCE), 
                  (DWORD)(LPMCI_DGV_RECT_PARMS)&mciRect);

   rcMovieBounds = mciRect.rc;  /* get it in the movie bounds rect */
   // commented-out code to center movie...
   //pMovieInfo->rcMovie.left = (rcClient.right/2) - (rcMovieBounds.right / 2);
   //pMovieInfo->rcMovie.top = (rcClient.bottom/2) - (rcMovieBounds.bottom / 2);
   pMovieInfo->rcMovie.left = 0;
   pMovieInfo->rcMovie.top = 0;
   pMovieInfo->rcMovie.right = pMovieInfo->rcMovie.left + rcMovieBounds.right;
   pMovieInfo->rcMovie.bottom = pMovieInfo->rcMovie.top + rcMovieBounds.bottom;
   
   /* reposition the playback (child) window */
   MoveWindow(pMovieInfo->hwndMovie, pMovieInfo->rcMovie.left, pMovieInfo->rcMovie.top,
       rcMovieBounds.right, rcMovieBounds.bottom, TRUE);
}

/*
	fileCloseMovie -- uses MCI_CLOSE to close the movie, and
		InvalidateRect to make the movie actually disappear from
		the screen.
*/
void fileCloseMovie(PMOVIEINFO pMovieInfo, BOOL bUpdate)
{
  MCI_GENERIC_PARMS  mciGeneric;
  
  pMovieInfo->fPlaying = FALSE; // can't be playing any longer
  pMovieInfo->fMovieOpen = FALSE;   // no more movies open
  
  mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_CLOSE, MCI_WAIT, 
                 (DWORD)(LPMCI_GENERIC_PARMS)&mciGeneric);
                 
  if (bUpdate)
    {    
    /* cause a total repaint to occur */
    InvalidateRect(pMovieInfo->hwndParent, NULL, TRUE);
    UpdateWindow(pMovieInfo->hwndParent);
    }
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
        dbg("Error getting movie length");
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
        dbg("Error getting movie frame");
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
void fileOpenMovie(PMOVIEINFO pMovieInfo, LPSTR szFilename)
{
   
    if (pMovieInfo == NULL)
        {
        dbg("No MOVIEINFO for specified movie when trying to open AVI file.");
        return;
        }

   if (lstrlen(szFilename)){
     MCI_DGV_OPEN_PARMS mciOpen;
     MCI_DGV_WINDOW_PARMS   mciWindow;
     MCI_DGV_STATUS_PARMS   mciStatus;

     /* we got a filename, now close any old movie and open */
     /* the new one.                    */
     if (pMovieInfo->fMovieOpen)
         fileCloseMovie(pMovieInfo, TRUE);
     
     /* we have a .AVI movie to open, use MCI */
     /* set up the open parameters */
     mciOpen.dwCallback = NULL;
     mciOpen.wDeviceID = mciOpen.wReserved0 =
         mciOpen.wReserved1 = 0;
     mciOpen.lpstrDeviceType = NULL;
     mciOpen.lpstrElementName = szFilename;
     mciOpen.lpstrAlias = NULL;
     mciOpen.dwStyle = WS_CHILD;
     mciOpen.hWndParent = pMovieInfo->hwndParent;

     /* try to open the file */
     if (mciSendCommand(0, MCI_OPEN, 
         (DWORD)(MCI_OPEN_ELEMENT|MCI_DGV_OPEN_PARENT|MCI_DGV_OPEN_WS), 
       (DWORD)(LPMCI_DGV_OPEN_PARMS)&mciOpen) == 0){

         /* we opened the file o.k., now set up to */
         /* play it.                   */
         pMovieInfo->wMCIDeviceID = mciOpen.wDeviceID;  /* save ID */
         pMovieInfo->fMovieOpen = TRUE; /* a movie was opened */

         /* show the playback window */
         mciWindow.dwCallback = NULL;
         mciWindow.hWnd = NULL;
         mciWindow.wReserved1 = mciWindow.wReserved2 = 0;
         mciWindow.nCmdShow = SW_SHOW;
         mciWindow.lpstrText = (LPSTR)NULL;
         mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_WINDOW, 
             MCI_DGV_WINDOW_STATE, 
             (DWORD)(LPMCI_DGV_WINDOW_PARMS)&mciWindow);

         /* get the window handle */
         mciStatus.dwItem = MCI_DGV_STATUS_HWND;
         mciSendCommand(pMovieInfo->wMCIDeviceID, 
             MCI_STATUS, MCI_STATUS_ITEM,
             (DWORD)(LPMCI_STATUS_PARMS)&mciStatus);
         pMovieInfo->hwndMovie = (HWND)mciStatus.dwReturn;
         
         /* get movie length */         
         pMovieInfo->dwMovieLength = GetMovieLength(pMovieInfo);         
         pMovieInfo->dwCurrentFrame = 0;
                  
         /* now get the movie centered */
         positionMovie(pMovieInfo);
     } else {
        /* generic error for open */
        MessageBox(pMovieInfo->hwndParent, "Unable to open Movie", NULL, 
                  MB_ICONEXCLAMATION|MB_OK);
        pMovieInfo->fMovieOpen = FALSE;
     }
   }
   
   /* cause an update to occur */
   InvalidateRect(pMovieInfo->hwndParent, NULL, FALSE);
   UpdateWindow(pMovieInfo->hwndParent);
}

/*
	playMovie -- looks at wDirection parameter and
		sends MCI a MCI_PLAY command (optionally with
			MCI_DGV_PLAY_REVERSE flag set) OR
		sends MCI a MCI_PAUSE command
*/
void playMovie(PMOVIEINFO pMovieInfo, WORD wDirection)
{
   pMovieInfo->fPlaying = !pMovieInfo->fPlaying;    /* swap the play flag */
   if (wDirection == NULL)
       pMovieInfo->fPlaying = FALSE;    /* wDirection == NULL means PAUSE */

   /* play/pause the AVI movie */
   if (pMovieInfo->fPlaying){       
       MCI_DGV_PLAY_PARMS   mciPlay;
       DWORD dwFlags = MCI_NOTIFY;       
           
       /* init to play all */
       mciPlay.dwCallback = MAKELONG(pMovieInfo->hwndMovie,0);
       mciPlay.dwFrom = mciPlay.dwTo = 0;       
       if (wDirection == 2)
           dwFlags |= MCI_DGV_PLAY_REVERSE;
       
       mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_PLAY, dwFlags,
                       (DWORD)(LPMCI_DGV_PLAY_PARMS)&mciPlay);
   } else {
       MCI_DGV_PAUSE_PARMS  mciPause;
                        
       /* tell it to pause */
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
    
    if (pMovieInfo->fPlaying){
                        playMovie(pMovieInfo, NULL);   
                        }                    
    
    mciSeek.dwTo = dwFrame;
    mciSeek.dwCallback = MAKELONG(pMovieInfo->hwndMovie,0);
    mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_SEEK, MCI_TO | MCI_WAIT,
                        (DWORD)(LPMCI_SEEK_PARMS)&mciSeek);                    
}

/*
	seekMovie -- stops movie if playing, then goes to beginning or
		end of movie. wAction == 0 means beginning, == 1 means end.
		it does this my using MCI_SEEK with MCI_SEEK_TO_START or
		MCI_SEEK_TO_END.
*/
void seekMovie(PMOVIEINFO pMovieInfo, WORD wAction)
{
   /* first stop the movie from playing if it is playing */
   if (pMovieInfo->fPlaying){
       playMovie(pMovieInfo, NULL);   
   }
   if (wAction == 0){
       /* home the movie */
       mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_SEEK, MCI_SEEK_TO_START, 
            (DWORD)(LPVOID)NULL);
            
   } else if (wAction == 1){
       /* go to the end of the movie */
       mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_SEEK, MCI_SEEK_TO_END, 
            (DWORD)(LPVOID)NULL);
   } 
}

/*
	stepMovie -- stops movie if playing, then steps one frame:
		wDirection == MCI_STEP means forward, anything else means reverse.
		it does this by using MCI_STEP command with MCI_DGV_STEP_FRAMES,
		optionally with MCI_DGV_STEP_REVERSE flag set
		
*/
void stepMovie(PMOVIEINFO pMovieInfo, HWND hWnd, WORD wDirection)
{
   MCI_DGV_STEP_PARMS   mciStep;
   
   if (pMovieInfo->fPlaying)
       playMovie(pMovieInfo, NULL);  /* turn off the movie */
   
       
   mciStep.dwFrames = 1L;
   if (wDirection == MCI_STEP)
       mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_STEP, MCI_DGV_STEP_FRAMES,
           (DWORD)(LPMCI_DGV_STEP_PARMS)&mciStep);
   else
       mciSendCommand(pMovieInfo->wMCIDeviceID, MCI_STEP, 
           MCI_DGV_STEP_FRAMES|MCI_DGV_STEP_REVERSE,
           (DWORD)(LPMCI_DGV_STEP_PARMS)&mciStep);
}
