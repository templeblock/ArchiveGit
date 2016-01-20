/*
// $Workfile: Util.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:26p $
*/

/*
// Revision History:
//
// $Log: /PM8/PrintDll/Util.cpp $
// 
// 1     3/03/99 6:26p Gbeddow
// 
// 1     6/18/98 3:59p Mwilson
// 
// 14    6/08/98 4:13p Johno
// Got rid of FILE_SHARE_DELETE, an NT only flag
// 
// 13    5/20/98 5:56p Cboggio
// Decimal point changes in ValidDouble and FormatDouble for localization
// 
// 12    5/19/98 2:31p Hforman
// add IsStrInArray()
// 
// 11    3/06/98 4:00p Jayn
// Got rid of some extraneous semi-colons.
// 
// 10    2/26/98 4:06p Dennis
// Added ScalePBOXWithRounding
// 
// 9     2/19/98 5:06p Johno
// Added CComboFileNames
// 
// 8     2/10/98 1:28p Dennis
// Added GetRegistryString()
// 
// 7     1/21/98 4:49p Fredf
// Added SetFileTime()
// 
// 6     1/20/98 3:44p Fredf
// Got rid of GetNowTime(), use GetLocalTime(SYSTEMTIME) instead.
// 
// 5     1/20/98 10:09a Dennis
// Added YieldToWindows
// 
// 4     1/20/98 8:55a Fredf
// Moved CTimeStamp from UTIL to TIMESTMP.
// 
// 3     1/19/98 1:21p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
// 2     12/24/97 10:31a Jayn
// 
//    Rev 1.0   14 Aug 1997 15:26:50   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:54   Fred
// Initial revision.
// 
//    Rev 1.35   30 Jul 1997 14:24:02   Jay
// Fixed GetNowTime().
// 
//    Rev 1.34   30 Jul 1997 14:01:50   Jay
// GetNowTime function in Util
// 
//    Rev 1.33   18 Jul 1997 13:48:32   Jay
// Windows and System directory functions
// 
//    Rev 1.32   14 Jul 1997 13:11:46   Jay
// Now makes sure found window is visible in FindWindowByTitle.
// 
//    Rev 1.31   11 Jul 1997 11:45:12   johno
// Added DEBUG_ERRORBOX macro / function
// 
// 
//    Rev 1.30   09 Jul 1997 10:54:38   Fred
// Does not strip extra spaces out of category names
// 
//    Rev 1.29   09 Jun 1997 09:12:00   Fred
//  
// 
//    Rev 1.28   05 Jun 1997 17:09:40   Fred
// Operating system queries
// 
//    Rev 1.27   30 May 1997 15:07:38   Fred
// WindowHasTitle and FindWindowByTitle
// 
//    Rev 1.26   15 May 1997 16:49:40   Fred
// Added SafeToConvertPbox and Ppnt
// 
//    Rev 1.25   06 May 1997 17:19:00   dennisp
// Added ComputeTimeStamp()
// 
//    Rev 1.24   02 May 1997 11:49:32   johno
// Added check and correction for off screen
// window rect in CenterWindowInWindow.
// 
//    Rev 1.23   25 Apr 1997 10:07:52   Jay
// Added some huge functions
// 
//    Rev 1.22   24 Apr 1997 15:07:52   Jay
// Got rid of some TRACEs
// 
//    Rev 1.21   17 Apr 1997 13:06:04   Fred
// Added source rectangle to TransparentBltBitmap
// 
//    Rev 1.20   17 Apr 1997 12:12:30   Fred
// Added palette argument to TransparentBltBitmap
// 
//    Rev 1.19   15 Apr 1997 14:50:54   dennisp
// Added SafeStringCopy
// 
//    Rev 1.18   09 Apr 1997 17:12:44   Jay
// DoWinExec
// 
//    Rev 1.17   25 Mar 1997 14:12:44   Fred
// Changes for 16-bit
// 
//    Rev 1.16   19 Mar 1997 10:11:06   dennisp
// Added Util::Capitalize(), Util::ReplaceChar(), and Util::RemoveChar()
// 
//    Rev 1.15   13 Mar 1997 13:17:14   Jay
// Changes for 16-bit and 5.0 builds
// 
//    Rev 1.14   13 Jan 1997 15:26:44   Jay
// Now compiles under 16-bit
// 
//    Rev 1.13   07 Sep 1996 17:36:38   Fred
// 16-bit changes
// 
//    Rev 1.12   06 Sep 1996 18:15:08   Jay
// Added ChildWindowFromPointEx
// 
//    Rev 1.11   03 Sep 1996 12:35:18   Jay
// Added TrimQuotes.
// 
//    Rev 1.10   30 Aug 1996 19:34:04   Fred
//  
// 
//    Rev 1.9   15 Aug 1996 15:56:36   Fred
// LoadResourceBitmap()
// 
//    Rev 1.8   09 Aug 1996 16:51:06   Fred
// More graphic utility programs
// 
//    Rev 1.7   02 Aug 1996 13:12:10   Fred
// More 16-bit changes
// 
//    Rev 1.6   31 Jul 1996 11:58:44   Fred
// 16-bit compatibility
// 
//    Rev 1.5   24 Jun 1996 00:27:40   Fred
// Added some GDI functions and IsKeyMessage()
// 
//    Rev 1.4   24 May 1996 16:15:58   Fred
// TRACEx
// 
//    Rev 1.3   15 May 1996 08:51:36   Jay
// Changes from PMW2
// 
//    Rev 2.20   08 May 1996 16:59:40   FRED
// IsRootDirectory
// 
//    Rev 2.19   18 Apr 1996 16:44:18   JAY
// Close now NULLs out m_hSearch in FileIterator class.
// 
//    Rev 2.18   15 Nov 1995 14:38:14   FRED
// Added FileSharingAvailable
// 
//    Rev 2.17   18 Oct 1995 15:58:12   FRED
// Fixes for 16 bit version after 32 bit changes
// 
//    Rev 2.16   13 Oct 1995 14:15:12   FRED
// Fixed GetAvailableDiskSpace() and FileExists()
// 
//    Rev 2.15   21 Sep 1995 14:23:06   JAY
// Fixed DirectoryIsWritable().
// 
//    Rev 2.14   21 Sep 1995 13:46:16   JAY
// Made DirectoryExists and MakeDirectory work for WIN32.
// 
//    Rev 2.13   15 Sep 1995 14:04:20   JAY
// More 32-bit changes.
// 
//    Rev 2.12   13 Sep 1995 11:33:32   JAY
// New 32-bit stuff
// 
//    Rev 2.11   30 Aug 1995 11:00:54   FRED
// Added Trim(). Fixed FormatDouble(negative).
// 
//    Rev 2.10   29 Aug 1995 10:53:10   FRED
// Added ValidDouble()
// 
//    Rev 2.9   24 Aug 1995 17:57:56   FRED
// Number string validation functions
// 
//    Rev 2.8   22 Aug 1995 07:57:58   JAY
// Added CBoolSemaphore.
// 
//    Rev 2.7   18 Aug 1995 07:30:02   FRED
// Removed testing code.
// util.cpp -> L:\PMW2\DATABASE\UTIL.CPV
// Enter description of changes made.
// 
//    Rev 2.6   18 Aug 1995 07:24:02   FRED
// 
//    Rev 2.5   14 Aug 1995 10:59:30   FRED
// Added FitAspect
// 
//    Rev 2.4   01 Jul 1995 09:24:52   FRED
// Draw3dBorder can now draw optional background.
// 
// 
//    Rev 2.3   30 Jun 1995 10:14:22   FRED
// Added Draw3dBorder()
// 
//    Rev 2.2   05 Jun 1995 16:50:48   JAY
// Added Draw3dButton.
// 
//    Rev 2.1   09 May 1995 08:03:58   JAY
// Added HMemMove.
// 
//    Rev 2.0   07 Feb 1995 15:48:16   JAY
// Initial revision.
// 
//    Rev 1.0   30 Nov 1994 16:40:06   JAY
// Initial revision.
// 
//    Rev 1.12   14 Nov 1994 14:53:00   JAY
// Brought over changes from CATALOG.EXE (including skipping root dir check and
// fix of a "PORTABLE" bug I introduced by using _getdcwd()).
// 
//    Rev 1.2   11 Nov 1994 17:03:10   JAY
// Now doesn't check the file state of a root directory (since the results
// were unpredictable).
// 
//    Rev 1.1   10 Nov 1994 17:44:46   JAY
//  
// 
//    Rev 1.0   03 Nov 1994 12:54:24   FRED
//  
// 
//    Rev 1.0   25 Oct 1994 16:21:50   JAY
// Initial revision.
// 
//    Rev 1.11   04 Aug 1994 14:29:38   JAY
// GetDriveType (WIN16) wants a drive number (0=A, 1=B, etc).
// 
//    Rev 1.10   04 Aug 1994 08:30:56   JAY
// Large model and 32-bit changes
// 
//    Rev 1.9   23 Jun 1994 11:46:34   FRED
// RemoveBackslashFromPath() now preserves the
// trailing backslash on a root directory by default.
// 
// 
//    Rev 1.8   21 Jun 1994 15:41:12   FRED
// Fixed typo.
// 
// 
//    Rev 1.7   21 Jun 1994 14:39:34   FRED
// Removed non-functioning MessageBox() variations.
// Added SplitPath() and ConstructPath() that do drive, dir, file, and ext.
// 
// 
//    Rev 1.6   17 Jun 1994 15:08:14   FRED
// No change.
// 
//    Rev 1.5   17 Jun 1994 14:57:30   FRED
// Added FormatString(), FormatStringV(), and MessageBox().
// 
// 
//    Rev 1.4   16 Jun 1994 18:13:48   FRED
// Added AppendBackslashToPath(), RemoveBackslashFromPath(),
// and ConstructPath().
// 
// 
//    Rev 1.3   16 Jun 1994 15:11:32   FRED
// Changed SplitPath() to take pointers to CStrings.
// 
// 
//    Rev 1.2   16 Jun 1994 14:38:08   JAY
// Added SplitPath().
// 
//    Rev 1.1   16 Jun 1994 09:32:10   JAY
// Added LoadString().
// 
//    Rev 1.0   15 Jun 1994 17:13:04   FRED
// Initial revision.
*/ 
 
#include "stdafx.h"
#include "util.h"



#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


BOOL Util::GetRegistryString(HKEY hRootKey, LPCTSTR pszSubkey, LPCTSTR pszValue, CString& csValue)
{
   csValue.Empty();

   HKEY hKey = NULL;

   TRY
   {
      if (RegOpenKeyEx(
               hRootKey,
               pszSubkey,
               0,
               KEY_QUERY_VALUE,
               &hKey) == ERROR_SUCCESS)
      {
         DWORD dwLength = 0;
         if (RegQueryValueEx(
                  hKey,
                  pszValue,
                  NULL,
                  NULL,
                  NULL,
                  &dwLength) == ERROR_SUCCESS)
         {
            if (dwLength > 0)
            {
               long lResult = RegQueryValueEx(
                        hKey,
                        pszValue,
                        NULL,
                        NULL,
                        (LPBYTE)(csValue.GetBuffer(dwLength)),
                        &dwLength);

               csValue.ReleaseBuffer();

               if (lResult != ERROR_SUCCESS)
               {
                  csValue.Empty();
               }
            }
         }
      }

      RegCloseKey(hKey);
   }
   CATCH_ALL(e)
   {
      if (hKey != NULL)
      {
         RegCloseKey(hKey);
      }
      csValue.Empty();
   }
   END_CATCH_ALL

   return !csValue.IsEmpty();
}
