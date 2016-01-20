//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _TASKMAN_H_
#define _TASKMAN_H_
BOOL WINPROC DlgTaskManagerProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL AddTaskItem(LPTHREADDATA lpThreadData);
BOOL ModifyTaskItem(LPTHREADDATA lpThreadData);
BOOL DeleteTaskItem(LPTHREADDATA lpThreadData);
#endif // _TASKMAN_H_

