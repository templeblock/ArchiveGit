/***************************************************************************
*  FILE:        PROCLOC.CPP                                                *
*  SUMMARY:     Class for Finding a running process under Windows 95 & NT  *
*  AUTHOR:      Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Initials Legend:                                                        *
*     DGP       Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Revision History                                                        *
*                                                                          *
*  Date     Initials  Description of Change                                *
*  ------------------------------------------------------------------------*
// $Log: /PM8/App/procloc.cpp $
// 
// 1     3/04/99 10:03a Gbeddow
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 4     3/02/98 5:04p Johno
// Compile update
// 
// 3     2/25/98 1:58p Johno
// test
***************************************************************************/
         
#include "stdafx.h"
#include "procloc.h"
#include "enumproc.h"   // For enumerating processes under Windows 95 & NT

#define  MAX_EVENT_ID   32767

// Static class member initialization
CMapWordToPtr     CProcessLocater::sm_thisList;

CProcessLocater::CProcessLocater()
   {
      m_bFoundProcess = FALSE;
   }

CProcessLocater::~CProcessLocater()
   {
   }

BOOL CProcessLocater::Find(LPCSTR  szProgramFileName,
   BOOL bBringWindowToTop /*=TRUE*/,
   WORD wUniqueEventID /* =1 */)
   {
      CProcessLocater *pFoundThis;
      char              szShortFileName[_MAX_PATH];
      DWORD             dwCharsCopied, dwError;

      m_wThisEventID = wUniqueEventID;
      dwCharsCopied = GetShortPathName(szProgramFileName, szShortFileName, _MAX_PATH);
      if(dwCharsCopied > 0)
      {
         m_csProcessToFind = szShortFileName;
      }
      else
      {
         m_csProcessToFind = szProgramFileName;
         dwError = GetLastError();
      }
      m_csProcessToFind.MakeUpper();
      m_bFoundProcess = FALSE;
      m_hwndProcess = NULL;

      ASSERT(!m_csProcessToFind.IsEmpty());

      // If callers ID is already used, try to find next sequential one that isn't
      while(sm_thisList.Lookup(m_wThisEventID, (LPVOID &) pFoundThis) && m_wThisEventID < MAX_EVENT_ID)
         {
            m_wThisEventID++;
         }
      if(m_wThisEventID == MAX_EVENT_ID)
         {
            ASSERT(0);     // Ran out of ID's
            return FALSE;
         }
      sm_thisList.SetAt(m_wThisEventID, (LPVOID) this); // Store ID and This pointer
      EnumProcs(enumProcessCallback, (LONG) m_wThisEventID);
      sm_thisList.RemoveKey(m_wThisEventID);   // Remove this object from list
      if(m_bFoundProcess && bBringWindowToTop)
         BringWindowToTop();
      else
         FindProcessWindow();
      return m_bFoundProcess;
   }

BOOL CProcessLocater::BringWindowToTop()
   {
      if(!m_bFoundProcess)
         return FALSE;

      // Get handle to window process if we don't already have one
      if(m_hwndProcess == NULL)
      {
         if(!FindProcessWindow())
            return FALSE;
      }

      if(IsIconic(m_hwndProcess))
         ShowWindow(m_hwndProcess,SW_RESTORE);
      else
      {
#ifdef WIN32          
         // Bring it to top
         SetForegroundWindow(m_hwndProcess);
#else
         ShowWindow(m_hwndProcess,SW_SHOW);
		   SetActiveWindow(m_hwndProcess);
#endif
  	 	}

      return TRUE;
   }

BOOL CProcessLocater::FindProcessWindow()
   {
      LONG           lStyle;
      DWORD          dwProcessId;
      HWND           hwndNext;

      m_hwndProcess = NULL;

      if(!m_bFoundProcess)
         return FALSE;

      // Run through all windows and find one where Process ID matches
	   hwndNext = GetWindow(GetDesktopWindow(),GW_CHILD);
	   while(hwndNext)
      {
   	   lStyle = GetWindowLong(hwndNext, GWL_STYLE);
	 	   if((lStyle & WS_VISIBLE) && !(lStyle & (WS_CHILD | WS_POPUP)))
         {
   #ifdef WIN32
	         // get Process ID associated with this window
	         GetWindowThreadProcessId(hwndNext, &dwProcessId);
   #else
   		   hInst = GetClassWord(hwndNext,GCW_HMODULE);
	   	   if(hInst)
            {
  	 	 	      GetModuleFileName((HINSTANCE)hInst,szbuf,sizeof szbuf);
   #endif  	 	 	
            // If Process ID Matches, 
  	 	 	   if(m_dwProcessID == dwProcessId)
            {
               m_hwndProcess = hwndNext;
  	 	 	 	   return TRUE;       
  	 	 	   }
   #ifndef WIN32
	 	   }
   #endif
		   }
		   hwndNext = GetWindow(hwndNext, GW_HWNDNEXT);
	   }
      return FALSE;
   }

// Static Members
BOOL CALLBACK CProcessLocater::enumProcessCallback(DWORD dwProcessID, WORD w16BitTask,
   LPSTR szEnumFileName, LPARAM lUserParam)
   {
      char              szShortPath[_MAX_PATH];
      CProcessLocater      *pThis = NULL;
      WORD              wEventID = (WORD) lUserParam;
      CString           csFileNameFound;

      // Lookup this pointer for object by Program FileName in CMapStringToPtr list
      if(sm_thisList.Lookup(wEventID, (LPVOID &) pThis) == 0)
         {
            // Failed to find ID in thisList
            ASSERT(0);
            return FALSE;  // Stop Enumerating
         }

      ASSERT(pThis);

      GetShortPathName(szEnumFileName, szShortPath, _MAX_PATH);
      csFileNameFound = szShortPath;
      csFileNameFound.MakeUpper();

      if(csFileNameFound.Find(pThis->m_csProcessToFind) >= 0)
         {
            pThis->m_dwProcessID = dwProcessID;
            pThis->m_bFoundProcess = TRUE;
            pThis->m_csLongProcessFileName = szEnumFileName;
            pThis->m_csShortProcessFileName = csFileNameFound;
            return FALSE;  // Done
         }

      return TRUE;   // Continue enumerating
   }
	 	 
