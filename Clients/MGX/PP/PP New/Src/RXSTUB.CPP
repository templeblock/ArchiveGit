//®PL1¯®FD1¯®TP0¯®BT0¯
// (c) Copyright 1991 MICROGRAFX, Inc., All Rights Reserved.
// This material is confidential and a trade secret.
// Permission to use this work for any purpose must be obtained
// in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081
#include "pp.h"
#include "id.h"
#include "basetype.h"
#include "mgxrx.h"

HANDLE far pascal  RxCreateDialog (HANDLE hModule,HWND hParent,
    const char far * pResName,RECT far * pAlignArea,RXCALLBACK Callback,
    void far * pData)
{
return(NULL);
}

int far pascal  RxDialogBox (HANDLE hModule,HWND hParent,
    const char far * pResName,RECT far * pAlignArea,RXCALLBACK Callback,
    void far * pData)
{
return(0);
}

void far pascal  RxOnDestroyDialog (HWND hDlg)
{
}

void far pascal  RxEndDialog (HWND hDlg,int ReturnCode)
{
}

bool far pascal  RxOnMoveDialog (HWND hDlg,int X,int Y)
{
return(FALSE);
}

bool far pascal  RxOnSizeDialog (HWND hDlg,int W,int H)
{
return(FALSE);
}

bool far pascal  RxResizeDialog (HWND hDlg,RECT far * pAlignArea)
{
return(FALSE);
}

int far pascal  RxIndexedSize (HWND hWnd,int SizeIdx)
{
return(0);
}

bool far pascal  RxRegisterClassIndex (HANDLE hModule,int Idx,
    const char far * pName,enum RXCLASS Type,RXWNDCREATE pCreate)
{
return(FALSE);
}

