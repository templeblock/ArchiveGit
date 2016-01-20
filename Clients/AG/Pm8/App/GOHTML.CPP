/*
// $Workfile: GOHTML.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:05p $
//
*/

/*
// Revision History:
//
// $Log: /PM8/App/GOHTML.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 2     9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
//    Rev 1.0   14 Aug 1997 15:21:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:38   Fred
// Initial revision.
// 
//    Rev 1.4   13 Jun 1997 10:12:02   johno
// Allow HTML file to pre exist (16 bit)
// 
//    Rev 1.3   06 Jun 1997 11:05:32   johno
// Got rid of GetFilePath - use path manager
// 
//    Rev 1.2   05 Jun 1997 14:34:24   johno
// Changes for multiple hub support
// 
//    Rev 1.1   12 May 1997 12:00:58   johno
// 
*/

#include "stdafx.h"

static const char BASED_CODE str1 [] = "PMWPAGE.HTM";
static const char BASED_CODE str2 [] = "<HTML><HEAD><TITLE></TITLE></HEAD><BODY><A HREF=http://www.printmaster.com/>Click </A>Here to go to the Printmaster home page.</BODY></HTML>";

#ifndef  WIN32

BOOL
MakeHTM (CString *cs)
{
   CString     Temp;
   CFile       FileHTM;

   *cs = path_manager.make_full_path(str1);

   if (path_manager.file_exists(*cs) == TRUE)
      return TRUE;
   
   if (FileHTM.Open ((const char *)*cs, CFile::modeCreate | CFile::modeReadWrite) != FALSE)
   {
      TRY
      {
         Temp = str2;
         FileHTM.WriteHuge ((const char *)Temp, Temp.GetLength ());
         FileHTM.Close ();
         
         return TRUE;
      }
      CATCH_ALL(e)
      {
         FileHTM.Close ();
      }
      END_CATCH_ALL
   }

   return FALSE;
}

#endif

BOOL
DoHTML (HWND hwnd, LPCSTR url, BOOL GotoPage)
{
#ifdef   WIN32
   HINSTANCE   hInst;
   CString        NetFind;

   if ((hInst = ShellExecute (hwnd, NULL, url, NULL, NULL, SW_SHOWNORMAL)) > (HINSTANCE)32)
      return TRUE;
   
   return FALSE;
#else
   TCHAR       EXEName [_MAX_PATH];
   HINSTANCE   hInst;
   CString        NetFind;

   if (MakeHTM (&NetFind) == TRUE)
   {
      // Test for EXE mapped to HTM files
      if ((hInst = FindExecutable (NetFind,  NULL, EXEName)) > (HINSTANCE)32)
      {
         // Try to launch by URL
         if ((hInst = ShellExecute (hwnd, NULL, url, NULL, NULL, SW_SHOWNORMAL)) > (HINSTANCE)32)
            return TRUE;
         // Try to launch by local htm file
         if (GotoPage == TRUE)
            if ((hInst = ShellExecute (hwnd, NULL, EXEName, (const char *)NetFind, NULL, SW_SHOWNORMAL)) > (HINSTANCE)32)
               return TRUE;
      }   
   }
   
   return FALSE;
#endif
}

