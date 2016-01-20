/*
// $Header: /PM8/App/DLGMGR.CPP 1     3/03/99 6:04p Gbeddow $
//
//
// $Log: /PM8/App/DLGMGR.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:08   Fred
// Initial revision.
// 
//    Rev 1.6   11 Mar 1997 16:13:48   Jay
// Extensions to the workspace
// 
//    Rev 1.5   19 Dec 1996 14:49:06   johno
// Position dialog in C++
// 
//    Rev 1.0   19 Dec 1996 14:43:44   johno
// Initial revision.
// 
*/

#include "pmw.h"
#include "stdafx.h"
#include "resource.h"
#include "Position.h"

int 
CDialogManager::DoPositionDialog(CPositionData* pData, int nStartPage /*=0*/)
{
   int                     ret;

   CPositionObjectDialog   PositionDialog (IDS_Confirm, AfxGetMainWnd (), nStartPage);

   ret = PositionDialog.DoModal (pData);

   //return IDCANCEL;
   return ret;
}

CDialogManager DialogManager;
