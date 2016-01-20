/*
// $Header: /PM8/App/DLGMGR.H 1     3/03/99 6:04p Gbeddow $
//
// $Log: /PM8/App/DLGMGR.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:20:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:36   Fred
// Initial revision.
// 
//    Rev 1.4   11 Mar 1997 16:14:00   Jay
// Extensions to the workspace
// 
//    Rev 1.3   15 Jan 1997 10:55:40   Jay
// Added standard header; removed message pragma.
*/

#ifndef __DLGMGR_H__
#define __DLGMGR_H__

//#pragma message ("New DLGMGR.H")

#include "pagedefs.h"

struct CStandardElement
{
   LPCSTR pName;              // pointer to name
   DWORD dwFlags;             // layout element flags
   PBOX Bound;                // long rect (x0, y0, x1, y1) (0-1000)
};

struct CObjectElement
{
   enum
   {
      FLAG_Line = 1,
      FLAG_XFlipped = 2,
      FLAG_YFlipped = 4
   };

   PBOX  ObjectBound;         // long rect (x0, y0, x1, y1)
   PPNT  ObjectSize;          // long point (x, y)
   DWORD lFlags;              // object flags
   DWORD lAngle;              // CFixed angle
};

struct CPositionData
{
   PBOX PageBound;
   long lLayoutChosen;

   long lElementCount;
   CStandardElement* pElements;

   long lObjectCount;
   CObjectElement* pObjects;
};

class CDialogManagerState
{
public:
   HWND m_hWndDialog;
   long m_lHelpContext;             // For sound manager.

   WORD m_fExpandWindow;
   HWND m_hWndSpeakerIcon;
   HWND m_hWndStopSound;
   HWND m_hWndPlaySound;
   HWND m_hWndInspectSound;
   LPVOID m_pMyData;                // My callback data.
};

class CDialogManager
{
public:
   BOOL
   Open(LPCSTR pWorkingDirectory, LPCSTR pCDDirectory)
   {
      return TRUE;
   }
   
   int 
   DoPositionDialog(CPositionData* pData, int nStartPage = 0);
};

extern CDialogManager DialogManager;

#endif
