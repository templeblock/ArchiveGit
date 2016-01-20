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
	avi.c:
		initAVI -- initialize avi libraries
		termAVI -- Closes the opened AVI file and the opened device type
 */
#include <windows.h>
#include <mmsystem.h>
#include <digitalv.h>       
#include <viewer.h>

#include "hotspot.h"

/* 
	initAVI -- Opens the "avivideo" device with MCI_OPEN command
 */
BOOL initAVI(void)
{
    MCI_DGV_OPEN_PARMS  mciOpen;
         
    /* set up the open parameters */
    mciOpen.dwCallback = NULL;
    mciOpen.wDeviceID = mciOpen.wReserved0 =
             mciOpen.wReserved1 = 0;
    mciOpen.lpstrDeviceType = "avivideo";
    mciOpen.lpstrElementName = NULL;
    mciOpen.lpstrAlias = NULL;
    mciOpen.dwStyle = 0;
    mciOpen.hWndParent = NULL;
         
   /* try to open the driver */
   return (mciSendCommand(0, MCI_OPEN, (DWORD)(MCI_OPEN_TYPE), 
                          (DWORD)(LPMCI_DGV_OPEN_PARMS)&mciOpen) == 0);
}

/*
	termAVI -- gets device ID for "avivideo" from mciGetDeviceID,
		then closes the device with MCI_CLOSE.
*/
void termAVI(void)
{
    WORD               wID;
    MCI_GENERIC_PARMS  mciClose;

    wID = mciGetDeviceID("avivideo");
    mciSendCommand(wID, MCI_CLOSE, 0L, 
                   (DWORD)(LPMCI_GENERIC_PARMS)&mciClose);
}
