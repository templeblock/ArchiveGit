/*************************************************************************
 * MVBTASK.C
 *
 * This module provides a generic set of task-management routines for
 * Viewer DLLs that need to keep track of unique session-specific data
 * for each Viewer instance using the DLL. The module stores a table of
 * VWRINFO data structures indexed by a task handle or instance handle,
 * whichever is appropriate for the DLL. Functions are provided to
 * initialize the table, add a VWRINFO structure for a new Viewer session,
 * remove a VWRINFO structure for a session being closed, and retrieve
 * a VWRINFO structure given a task handle or instance handle.
 *
 * To use this module, define a VWRINFO structure (including a far
 * pointer to that structure, LPVWRINFO) in a header file for your DLL,
 * then include that header file in this module. The functions provided
 * by the module are:
 *
 * InitTasks:  Initializes the table. Call from LibMain.
 *
 * AddTask:    Allocates a new VWRINFO structure, adds the structure to the
 *             table, and returns a pointer to that structure. Call from
 *             LDLLHandler when you receive the DW_INIT message.
 *
 * DeleteTask: Given a task handle or instance handle, deletes the VWRINFO
 *             structure for that task. Call from LDLLHandler when you
 *             receive the DW_TERM message.
 *
 * GetTaskData: Given a task handle or instance handle, returns a pointer
 *             to the VWRINFO structure for that task.
 *
 *************************************************************************/

#include <windows.h>
#include <windowsx.h>
#include <viewer.h>
#include <stdlib.h>

// Replace the following line with #include "yourdll.h"
#include "hotspot.h"

#include "mvbtask.h"

char szTaskTableFullMsg[] = "Can't add any more instances to the task table.";
char szMVBTASK[] = "MVBTASK";

int nTasks;

VWRTASK amvbTasks[MAX_TASKS];


/*************************************************************************
 * InitTasks
 *
 * Initializes the task information table by setting all values to NULL.
 *
 *************************************************************************/

void InitTasks(void)
{
    int i;

    for(i = 0; i < MAX_TASKS; i++)
    {
        amvbTasks[i].hTask = NULL;
        amvbTasks[i].lpData = NULL;
    }
    nTasks = 0;
}


/*************************************************************************
 * GetTask
 *
 * Given a task handle or instance handle, returns the index of that
 * table entry. Note: call this with a NULL handle to get the first
 * empty entry.
 *
 * Returns: index to first matching entry, or -1 if no entry exists.
 *
 *************************************************************************/

int GetTask(HANDLE hTask)
{
    int i;
    
    for(i = 0; i < MAX_TASKS; i++)
    {
        if(amvbTasks[i].hTask == hTask)
            return i;
    }
    return -1;
}


/*************************************************************************
 * DeleteTask
 *
 * Given a task handle or instance handle, removes that entry from the
 * table.
 *
 *************************************************************************/

void DeleteTask(HANDLE hTask)
{
    int i = GetTask(hTask);

    if(i == -1)
    {
        return;
    }

    if(amvbTasks[i].lpData)
    {
        GlobalFreePtr(amvbTasks[i].lpData);

        amvbTasks[i].lpData = NULL;
    }

    amvbTasks[i].hTask = NULL;

    nTasks--;
}



/*************************************************************************
 * GetTaskData
 *
 * Given a task handle or instance handle, returns a pointer to the
 * session-information structure for that Viewer instance.
 *
 * Returns: Pointer to VWRINFO structure or NULL if no such entry exists.
 *
 *************************************************************************/

LPVWRINFO GetTaskData(HANDLE hTask)
{
    int i = GetTask(hTask);

    if(i == -1)
        return NULL;
    else
        return amvbTasks[i].lpData;
}



/*************************************************************************
 * AddTask
 *
 * Given a task handle or instance handle, creates a new table entry
 * for that Viewer session, including allocating the VWRINFO structure.
 * The structure is allocated GMEM_ZEROINIT.
 *
 * Returns: Pointer to new VWRINFO structure or NULL if table is full or
 *          memory allocation error occurs.
 *
 *************************************************************************/

LPVWRINFO AddTask(HANDLE hTask)
{
    // Get the first empty entry in the table.

    int i = GetTask(NULL);

    // Return error if table is already full.

    if(i == -1)
        return NULL;

    amvbTasks[i].lpData = (LPVWRINFO)GlobalAllocPtr(
                                        GMEM_MOVEABLE | GMEM_ZEROINIT, 
                                        sizeof(VWRINFO));
    if(amvbTasks[i].lpData)
    {
        amvbTasks[i].hTask = hTask;
        nTasks++;
    }

    return amvbTasks[i].lpData;
}

