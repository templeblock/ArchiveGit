/*****************************************************************************

       Copyright (c) Micrografx, Inc., 1991 - 1992.  All rights reserved.
    Permission to use this work for any purpose must by obtained in writing
           from Micrografx, 1303 Arapaho Rd, Richardson, Texas 75081.

******************************************************************************
******************************************************************************

                                   progmgr.c

******************************************************************************
******************************************************************************

  Description:  DDE interface to Windows Program Manager.
 Date Created:  02/20/91
       Author:  Kevin McFarland

*****************************************************************************/

#ifdef BORLANDC
#pragma option -zC_DDE
#endif

#include <windows.h>

#include "data.h"
#include "routines.h"
#include "id.h"

//#include "mgxdde.h"
//#include "progmgr.h"

/********************************* Constants ********************************/

#ifdef DEBUG
    #define Debug(Statement)    {Statement;}
    #define LOCAL
#else
    #define Debug(Statement)
    #define LOCAL               static
#endif

/****************************************************************************/

#define MAXSTR 256

/****************************** Public Functions ****************************/

BOOL FAR PASCAL add_progmgr_item (hProgMgr,lpName,lpCmd,lpExe,iIcon)
/*  Append a new item to a Group file. hProgMgr is the program manager 
    handle from open_progmgr_group. lpName is the name of item to add.
    lpCmd is the item's command line. lpExe is the EXE file to grab icon from,
    if NULL then get the icon from lpCmd param. iIcon is the icon # in lpExe
    to get. The Function returns TRUE if successful, FALSE otherwise */
HWND  hProgMgr;
LPSTR lpName;
LPSTR lpCmd;
LPSTR lpExe;
int   iIcon;
{
    char szBuffer[MAXSTR];

    wsprintf (szBuffer,"[replaceitem(\"%s\")]",lpName);
    dde_exec (hProgMgr,szBuffer);
    wsprintf (szBuffer,"[additem(%s,\"%s\",%s,%d)]",lpCmd,lpName,lpExe,iIcon);
    return dde_exec (hProgMgr,szBuffer);
}

/****************************************************************************/

BOOL FAR PASCAL del_progmgr_item (hProgMgr,lpItem)
/*  deletes a program manager item. */
HWND  hProgMgr;
LPSTR lpItem;
{
    char szBuffer[MAXSTR];

	wsprintf (szBuffer,"[deleteitem(%s)]",lpItem);
	return dde_exec (hProgMgr,szBuffer);
}

/****************************************************************************/

BOOL FAR PASCAL add_progmgr_group (hProgMgr,lpGroup)
/*  creates a program manager group. */
HWND  hProgMgr;
LPSTR lpGroup;
{
    char szBuffer[MAXSTR];

	wsprintf (szBuffer,"[creategroup(%s)]",lpGroup);
	return dde_exec (hProgMgr,szBuffer);
}

/****************************************************************************/

BOOL FAR PASCAL del_progmgr_group (hProgMgr,lpGroup,bOnlyIfEmpty)
/*  deletes a program manager group. */
HWND  hProgMgr;
LPSTR lpGroup;
BOOL  bOnlyIfEmpty;
{
    BOOL bDelete = TRUE;
    char szBuffer[MAXSTR];

    if (bOnlyIfEmpty && dde_request (hProgMgr,lpGroup,szBuffer,sizeof szBuffer))
    {
        int nIcons = 1;

        sscanf (szBuffer,"%*[^,],%*[^,],%Fd",(LPINT)&nIcons);
        if (nIcons > 0)
            bDelete = FALSE;
    }
    if (bDelete)
    {
		wsprintf (szBuffer,"[deletegroup(%s)]",lpGroup);
        bDelete = dde_exec (hProgMgr,szBuffer);
    }
	return bDelete;
}

/****************************************************************************/

BOOL FAR PASCAL shw_progmgr_group (hProgMgr,lpGroup,wShow)
/*  Activates a program manager group. lpGroup is the name of the group.
    wShow is the show command that determines how ProgMan will act on
    the group. There are 8 predefined SG_ types in the header file. */
HWND  hProgMgr;
LPSTR lpGroup;
WORD  wShow;
{
   char szBuffer[MAXSTR];

   // 12/7/93 JD- 
   // workaround for ProgMgr bug. Sending a ShowGroup message 
   // to a group that is not maximized or minimized but is not
   // active doesn't work in Windows 3.1.  This workaround
   // puts the group in a minimized state, from which 
   // Sending ShowGroup(szGroup, SG_SHOWNORMAL) works.  
   wsprintf (szBuffer,"[showgroup(%s,%u)]",lpGroup, SG_MINIMIZE);
   dde_exec(hProgMgr, szBuffer);

	wsprintf (szBuffer,"[showgroup(%s,%u)]",lpGroup,wShow);
	return dde_exec (hProgMgr,szBuffer);
}

/****************************************************************************/

BOOL FAR PASCAL init_progmgr_session (lphProgMgr)
/*  Initiates a ProgMan dde session. The lphProgMgr shold point to
    NULL or the current Program manager handle. If NULL it will be set.
    The function returns TRUE if successful.*/
HWND FAR *lphProgMgr;
{
    if (!*lphProgMgr)
    {
        if (!(*lphProgMgr = dde_initiate ("PROGMAN","PROGMAN")))
            return FALSE;

        BringWindowToTop (*lphProgMgr);
        ShowWindow (*lphProgMgr,SW_RESTORE);
        EnableWindow (*lphProgMgr, FALSE);
    }
    return TRUE;
}

/****************************************************************************/

BOOL FAR PASCAL term_progmgr_session (hProgMgr,bExit)
/*  Terminates the ProgMan dde session. If bExit the function will 
    close down progman otherwise it will just leave */
HWND hProgMgr;
BOOL bExit;
{
    EnableWindow (hProgMgr,TRUE);
    if (bExit)
        dde_exec (hProgMgr,"[exitprogman(1)]");
    dde_terminate (hProgMgr);
    hProgMgr = NULL;
    return TRUE;
}

/****************************************************************************/


