//®FD1¯®PL1¯®TP0¯®BT0¯®RM200¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#ifndef _PCDUI_H_
#define _PCDUI_H_

LPFRAME ReadPCDThumbnail(LPTSTR ifile, short nImgNo, int outdepth, LPINT lpDataType);
BOOL IsPCDImage(LPTSTR lpFileName);
BOOL GetPCDOverview(LPTSTR lpFileName, LPTSTR lpOverview);
BOOL GetPCDLocation(LPTSTR lpSpec);
void DisplayPCDInfo(HWND hDlg, LPTSTR lpFileName);
BOOL WINPROC DlgPCDFileInfoProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL WINPROC DlgPCDOptionsProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
VOID WINPROC PhotoCDProgress(short nNum, short nDenom, long lData);
BOOL WINPROC PhotoCDAbort(long lData);

#endif // _PCDUI_H_
