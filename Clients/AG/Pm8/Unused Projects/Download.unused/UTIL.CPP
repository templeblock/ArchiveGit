/*
// $Workfile: UTIL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:34p $
*/

/*
// Revision History:
//
// $Log: /PM8/Toolbars/Printmaster/UTIL.CPP $
// 
// 1     3/03/99 6:34p Gbeddow
// 
// 30    2/25/99 5:23p Johno
// Moved AppendNamedApplicationRegistryPath() from UTIL.CPP to PMW.CPP,
// as some shared versions of UTIL.CPP could not find needed H file
// 
// 29    2/25/99 3:36p Johno
// Added AppendNamedApplicationRegistryPath() for finding component EXEs
// in the registry.
// 
// 28    2/15/99 3:18p Gbeddow
// performance improvements related to inserting categories in the Art
// Gallery tree control
// 
// 27    2/12/99 5:42p Gbeddow
// replace category drop-down control with tree control in the art gallery
// and web art gallery dialogs
// 
// 26    10/22/98 3:59p Jayn
// Added DrawClientBorder
// 
// 25    10/21/98 1:40p Johno
// Added RemoveExtraMenuSeparators()
// 
// 24    10/20/98 5:33p Jayn
// Improving the online order form.
// 
// 23    10/12/98 4:33p Jayn
// Rearranged some includes to make extraction easier.
// 
// 22    10/02/98 7:10p Jayn
// More content manager changes
// 
// 21    8/26/98 4:56p Dennis
// Added File Attribute exclusion flags parameter to FileExists.
// 
// 20    8/20/98 4:12p Jayn
// LICENSE.DAT is now in the SHARED directory.
// 
// 19    8/03/98 12:20p Jayn
// GetFileTime now does conversion from UTC to local time.
// 
// 18    7/08/98 1:04p Jayn
// Simplified filtering. Added Array variant to FillxxxFromList.
// 
// 17    6/25/98 7:34p Psasse
// added "FillListFromDelimitedString
// 
// 16    6/12/98 11:22a Cboggio
// Use CharUpper instead of MakeUpper for localization
// 
// 15    6/11/98 9:34a Dennis
// Added instance handle parameter to LoadResourceBitmap()
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

#include <math.h>
#include <direct.h>
#include <dos.h>

#ifndef WIN32
#include <io.h>
#include <fcntl.h>
#include <malloc.h>
#include <sys\stat.h>
#include <sys\locking.h>
#endif

#ifdef LOCALIZE
#include "clocale.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifndef WIN32
extern "C" void FAR PASCAL DOS3Call(void);
extern "C" int isalnum(int);
extern "C" int isalpha(int);
extern "C" int isdigit(int);
#endif

void Util::CenterWindowInWindow(CWnd *pcwndWindow, CWnd *pcwndInWindow)
{
   if (pcwndWindow != NULL)
   {
      if (pcwndInWindow == NULL)
      {
         if ((pcwndInWindow = pcwndWindow->GetParent()) == NULL)
         {
            pcwndInWindow = CWnd::GetDesktopWindow();
         }
      }

      if (pcwndInWindow != NULL)
      {
         CRect crRect;
         pcwndWindow->GetWindowRect(&crRect);
         
         CRect crInRect;
         pcwndInWindow->GetWindowRect(&crInRect);

         crRect.OffsetRect(
            ((crInRect.right-crRect.right)-(crRect.left-crInRect.left))/2,
            ((crInRect.bottom-crRect.bottom)-(crRect.top-crInRect.top))/2);
         // JRO Check that rectangle is not off screen.
         // This can happen if the window is larger than the the one it is being centered "in".
         int x = 0, y = 0, z;
         if (crRect.left < 0)
            x = -crRect.left;
         else
            if (crRect.right > (z = GetSystemMetrics (SM_CXSCREEN)))
               x = z - crRect.right;
            
         if (crRect.top < 0)
            y = -crRect.top;
         else
            if (crRect.bottom > (z = GetSystemMetrics (SM_CYSCREEN)))
               y = z - crRect.bottom;
         
         crRect.OffsetRect (x, y);

         pcwndWindow->MoveWindow((LPRECT)crRect, TRUE);
      }
   }
}
      
/*
// Format a double precision value into a string with a maximum
// number of places after the decimal point.
//
// If the precision is negative, then trailing zeros are stripped.
*/

void Util::FormatDouble(double dNumber, int nPrecision, CString& csOutput)
{
   csOutput.Empty();
   
   // if precision is negative, trim off trailing zeroes
   BOOL fTrim;
   
   if (fTrim = (nPrecision < 0))
   {
      nPrecision = -nPrecision;
   }
   
   // save sign
   BOOL bNegative = FALSE;
   
   if (dNumber < 0.0)
   {
      bNegative = TRUE;
      dNumber = -dNumber;
   }
   
   // compute minimum value that can be represented.  
   double dMin = 1.0;
   int i;

   for (i = 0; i < nPrecision; i++)
   {
      dMin *= 10.0;
   }
   
   // do one divide to minimize roundoff errors.
   dMin = 1.0/dMin;
   
   // round value for given precision
   dNumber += dMin/2.0;
      
   // if value is too small, return a zero
   if (dNumber < dMin)
   {
      csOutput = "0";
   }
   
   else
   {
      // check for negative number and add sign if necessary
      if (bNegative)
      {
         csOutput += '-';
      }
   
      // put leading zero before decimal point.
      if (dNumber < 1.0)
      {
         csOutput += '0';
      }
      
      // normalize number (<1.0) and count digits to the left
      int nCount = 0;
      
      while (dNumber >= 1.0)
      {
         dNumber /= 10.0;
         nCount++;
      }
      
      // place digits into string
      
      int nTrim = csOutput.GetLength();
      
      nPrecision = -nPrecision;
      
      while (nCount > nPrecision)
      {
         if (nCount == 0)
         {
#ifdef LOCALIZE
            csOutput += CurrentLocale.DecimalPoint();
#else
            csOutput += '.';
#endif
         }
         
         double dDigit;
         char cDigit;
         
         dNumber = modf(dNumber*10.0, &dDigit);
         csOutput += (cDigit = (char)((int)dDigit)+'0');
         
         if ((nCount > 0) || (cDigit != '0'))
         {
            nTrim = csOutput.GetLength();
         }
         
         nCount--;
      }
   
      if (fTrim)
      {  
         csOutput = csOutput.Left(nTrim);
      }
   }
}

DWORD Util::GetAvailableDiskSpace(const CString &csDirectory)
{
   CString csExpanded;

   DWORD dwResult = DISK_SPACE_NOT_AVAILABLE;

   TRY
   {
      csExpanded = csDirectory;
   }
   CATCH_ALL(e)
   {
      csExpanded.Empty();
   }
   END_CATCH_ALL

   if (ExpandDirectory(csExpanded, FALSE))
   {
#ifdef WIN32
      CString csDrive;

      TRY
      {
         DirectoryDrive(csExpanded, csDrive);
         AppendBackslashToPath(csDrive);
      }
      CATCH_ALL(e)
      {
         csDrive.Empty();
      }
      END_CATCH_ALL

      DWORD dwSectorsPerCluster;
      DWORD dwBytesPerSector;
      DWORD dwFreeClusters;
      DWORD dwTotalClusters;

      if (GetDiskFreeSpace(csDrive,
                           &dwSectorsPerCluster,
                           &dwBytesPerSector,
                           &dwFreeClusters,
                           &dwTotalClusters))
      {
         dwResult = dwFreeClusters * dwSectorsPerCluster * dwBytesPerSector;
      }
#else

      if ((csExpanded.GetLength() >= 2) && (csExpanded[1] == ':'))
      {
         dwResult = GetAvailableDiskSpace(csExpanded[0]);
      }
         
      else if ((csExpanded.GetLength() >= 2) && (csExpanded[0] == '\\') && (csExpanded[1] == '\\'))
      {
         /*
         // We need to connect, find a drive to use.
         */
            
         for (char cDrive = 0; cDrive < 26; cDrive++)
         {
            if (GetTypeOfDrive(cDrive) == 0)
            {
               break;
            }
         }
            
         if (cDrive < 26)
         {
            /*
            // Try to make the connection.
            */
               
            char szDestinationPath[] = "@:";
            szDestinationPath[0] = cDrive+'A';
               
            UINT uResult;
               
            if ((uResult = ::WNetAddConnection((LPSTR)((LPCSTR)csDirectory), "", szDestinationPath)) == WN_SUCCESS)
            {
               /*
               // Get the disk space.
               */
                  
               dwResult = GetAvailableDiskSpace(szDestinationPath[0]);

               /*
               // Done with the connection.
               */
                  
               ::WNetCancelConnection(szDestinationPath, TRUE);
            }
         }
      }
#endif
   }
   
   return dwResult;
}

DWORD Util::GetAvailableDiskSpace(char cDriveLetter)
{
   DWORD dwSpaceAvailable = (DWORD)(-1L);

#ifdef WIN32
   DWORD dwSectorsPerCluster;
   DWORD dwBytesPerSector;
   DWORD dwFreeClusters;
   DWORD dwTotalClusters;
   char cbName[] = "X:\\";

   cbName[0] = cDriveLetter;

   if (GetDiskFreeSpace(cbName,
                        &dwSectorsPerCluster,
                        &dwBytesPerSector,
                        &dwFreeClusters,
                        &dwTotalClusters))
   {
      dwSpaceAvailable = dwFreeClusters * dwSectorsPerCluster * dwBytesPerSector;
   }

#else
   /*
   // Make sure there is enough disk space available on the
   // chosen drive.
   */

   _asm
   {
      mov   dl,cDriveLetter
      cmp   dl,'a'
      jb    L0
      cmp   dl,'z'
      ja    L0
      add   dl,'A'-'a'
   L0:
      cmp   dl,'A'
      jb    L1
      cmp   dl,'Z'
      ja    L1
      sub   dl,'A'-1
      mov   ah,036h
      call  DOS3Call
      cmp   ax,-1
      je    L1
      mul   cx
      mov   cx,ax
      mov   ax,dx
      mul   bx
      xchg  ax,cx
      mul   bx
      add   dx,cx
      jmp   L2
   L1:
      mov   ax,-1
      mov   dx,-1
   L2:
      mov   word ptr dwSpaceAvailable+0,ax
      mov   word ptr dwSpaceAvailable+2,dx
   }
#endif
   
   return dwSpaceAvailable;
}

DWORD Util::GetClusterSize(const CString &csDirectory)
{
   CString csExpanded;
   
   DWORD dwResult = CLUSTER_SIZE_NOT_AVAILABLE;
   
   TRY
   {
      csExpanded = csDirectory;
   }
   CATCH_ALL(e)
   {
      csExpanded.Empty();
   }
   END_CATCH_ALL
   
   if (ExpandDirectory(csExpanded, FALSE))
   {
#ifdef WIN32
      DWORD dwSectorsPerCluster;
      DWORD dwBytesPerSector;
      DWORD dwFreeClusters;
      DWORD dwTotalClusters;

      if (GetDiskFreeSpace(csExpanded,
                           &dwSectorsPerCluster,
                           &dwBytesPerSector,
                           &dwFreeClusters,
                           &dwTotalClusters))
      {
         dwResult = dwSectorsPerCluster * dwBytesPerSector;
      }

#else
      if ((csExpanded.GetLength() >= 2) && (csExpanded[1] == ':'))
      {
         dwResult = GetClusterSize(csExpanded[0]);
      }
         
      else if ((csExpanded.GetLength() >= 2) && (csExpanded[0] == '\\') && (csExpanded[1] == '\\'))
      {
         /*
         // We need to connect, find a drive to use.
         */
            
         for (char cDrive = 0; cDrive < 26; cDrive++)
         {
            if (GetTypeOfDrive(cDrive) == 0)
            {
               break;
            }
         }
            
         if (cDrive < 26)
         {
            /*
            // Try to make the connection.
            */
               
            char szDestinationPath[] = "@:";
            szDestinationPath[0] = cDrive+'A';
               
            UINT uResult;
               
            if ((uResult = ::WNetAddConnection((LPSTR)((LPCSTR)csDirectory), "", szDestinationPath)) == WN_SUCCESS)
            {
               /*
               // Get the disk space.
               */
                  
               dwResult = GetClusterSize(szDestinationPath[0]);

               /*
               // Done with the connection.
               */
                  
               ::WNetCancelConnection(szDestinationPath, TRUE);
            }
         }
      }
#endif
   }
   
   return dwResult;
}

DWORD Util::GetClusterSize(char cDriveLetter)
{
   DWORD dwClusterSize = (DWORD)(-1L);

#ifdef WIN32
   DWORD dwSectorsPerCluster;
   DWORD dwBytesPerSector;
   DWORD dwFreeClusters;
   DWORD dwTotalClusters;
   char cbName[] = "X:\\";

   cbName[0] = cDriveLetter;

   if (GetDiskFreeSpace(cbName,
                        &dwSectorsPerCluster,
                        &dwBytesPerSector,
                        &dwFreeClusters,
                        &dwTotalClusters))
   {
      dwClusterSize = dwSectorsPerCluster * dwBytesPerSector;
   }
#else

   /*
   // Make sure there is enough disk space available on the
   // chosen drive.
   */
   
   _asm
   {
      mov   dl,cDriveLetter
      cmp   dl,'a'
      jb    L0
      cmp   dl,'z'
      ja    L0
      add   dl,'A'-'a'
   L0:
      cmp   dl,'A'
      jb    L1
      cmp   dl,'Z'
      ja    L1
      sub   dl,'A'-1
      mov   ah,036h
      call  DOS3Call
      cmp   ax,-1
      je    L1
      mul   cx
      jmp   L2
   L1:
      mov   ax,-1
      mov   dx,-1
   L2:
      mov   word ptr dwClusterSize+0,ax
      mov   word ptr dwClusterSize+2,dx
   }
#endif
   
   return dwClusterSize;
}

BOOL Util::MakeDirectory(const CString &csDirectory)
{
   BOOL fResult = FALSE;
   
   CString csExpanded;
   CString csComponent;
   
//   od("MakeDirectory(%s)\n\r", (LPCSTR)csDirectory);
   
   TRY
   {
      // Expand the directory name.
      
      csExpanded = csDirectory;
      
      if (ExpandDirectory(csExpanded))
      {
         csExpanded.AnsiToOem();
         
         // Skip the first component (the root directory); drive: or \\server\share
         
         int nIndex = 0;
         
         // Collect the drive: or \\server\share portion of the path.
         
         if ((csExpanded.GetLength() >= 2) && (csExpanded[0] == '\\') && (csExpanded[1] == '\\'))
         {
            nIndex = 2;
            
            // Skip past server name.
            
            while ((nIndex < csExpanded.GetLength()) && (csExpanded[nIndex] != '\\'))
            {
               nIndex++;
            }
            
            nIndex++;
                   
            // Skip past share name.
            
            while ((nIndex < csExpanded.GetLength()) && (csExpanded[nIndex] != '\\'))
            {
               nIndex++;
            }
            
            fResult = TRUE;
         }
         
         else if ((csExpanded.GetLength() >= 3) && (csExpanded[1] == ':') && (csExpanded[2] == '\\'))
         {
            nIndex = 2;
            fResult = TRUE;
         }
            
         // Attempt to create each component in the path name.
         
         if (fResult)
         {
            while (nIndex < csExpanded.GetLength())
            {
               // move to the next component
                  
               nIndex++;
                  
               while ((nIndex < csExpanded.GetLength()) && (csExpanded[nIndex] != '\\'))
               {
                  nIndex++;
               }
               
               if (nIndex < csExpanded.GetLength())
               {
                  // Check if the current component exists. If not, create it.
                     
                  csComponent = csExpanded.Left(nIndex);
                     
//                  od("MakeDirectory - Component |%s|\n\r", (LPCSTR)csComponent);
                     
                  if (!DirectoryExists(csComponent))
                  {
                     if (_mkdir(csComponent) != 0)
                     {
                        fResult = FALSE;
                        break;
                     }
                  }
               }
            }
         }
      }
   }
   CATCH_ALL(e)
   {
      csExpanded.Empty();
      csComponent.Empty();
      
      THROW_LAST();
   }
   END_CATCH_ALL
   
   return fResult;
}

BOOL Util::DirectoryExists(const CString& csDirectory)
{
//   od("DirectoryExists(%s) --> ", (LPCSTR)csDirectory);

   BOOL fResult = FALSE;
   
   CString csExpanded;
   
   TRY
   {
      csExpanded = csDirectory;
      
      if (ExpandDirectory(csExpanded))
      {
         csExpanded += "*.*";
         csExpanded.AnsiToOem();
         
#ifdef WIN32
         CFileIterator Iter;

         fResult = Iter.FindFirst(csExpanded, _A_NORMAL)
                        || GetLastError() == ERROR_NO_MORE_FILES;
#else
         struct _find_t FileInfo;
         int nResult = _dos_findfirst(csExpanded, _A_NORMAL, &FileInfo);
         fResult = ((nResult == 0) || (nResult == 0x12));   // 0x12 means no more files
#endif
         
      }
   }
   CATCH_ALL(e)
   {
      csExpanded.Empty();

      THROW_LAST();
   }
   END_CATCH_ALL

//   od("%d\n\r", fResult);

   return fResult;
}

BOOL Util::DirectoryIsWritable(const CString& csDirectory)
{
   BOOL fResult = FALSE;
   
   CString csTestFile;
   CString csExpanded;
   
   TRY
   {
      csExpanded = csDirectory;
      
      if (ExpandDirectory(csExpanded))
      {
         csTestFile = "$$$$$$$$.$$$";  /* hope this doesn't already exist, should ensure unique name */
         csExpanded += csTestFile;
         csExpanded.AnsiToOem();

         LPCSTR lpszFileName = csExpanded;

         HFILE hFile;
         if ((hFile = _lcreat(csExpanded, 0)) != NULL)
         {
            _lclose(hFile);
            remove(csExpanded);
            fResult = TRUE;
         }
#ifdef OLD
         _asm
         {
            /*
            // Attempt to create the file.
            */
                           
            push  ds
                           
            lds   dx,dword ptr lpszFileName  // DS:DX <-- pointer to file name
                           
            xor   cx,cx                      // normal file
                  
            mov   ah,03ch
            call  DOS3Call
                           
            pop   ds
                  
            jc    l0                         // error, directory not writable
                  
            /*
            // The file was created, close it and erase it.
            */
            
            mov   bx,ax
            
            mov   ah,03eh
            call  DOS3Call
            
            push  ds
            
            lds   dx,dword ptr lpszFileName  // DS:DX <-- pointer to file name
                           
            mov   ah,041h
            call  DOS3Call
                           
            pop   ds
         }

         fResult = TRUE;
         
      l0: ;
#endif
      }
   }
   CATCH_ALL(e)
   {
      csTestFile.Empty();
      csExpanded.Empty();
      
      THROW_LAST();
   }
   END_CATCH_ALL
   
   return fResult;
}

BOOL Util::ExpandDirectory(CString &csDirectory, BOOL fAppendBackslash /* = TRUE */)
{
// od("ExpandDirectory(%s) --> ", (LPCSTR)csDirectory);
   
   BOOL fResult = FALSE;

   CString csWork;
   CString csExpandedDirectory;
   
   TRY
   {
      csWork = csDirectory;
      csWork.AnsiToOem();
      csExpandedDirectory.Empty();

      int nIndex = 0;
      int nEndOfDriveIndex = 0;
      
      /*
      // Collect the drive: or \\server\share portion of the path.
      */
      
      if ((csWork.GetLength() >= 2) && (csWork[0] == '\\') && (csWork[1] == '\\'))
      {
         nIndex = 2;
         
         // skip past server name
         
         while ((nIndex < csWork.GetLength()) && (csWork[nIndex] != '\\'))
         {
            nIndex++;
         }
         
         nIndex++;
                
         // skip past share name
         
         while ((nIndex < csWork.GetLength()) && (csWork[nIndex] != '\\'))
         {
            nIndex++;
         }
         
         if (nIndex >= csWork.GetLength())
         {
            nIndex = csWork.GetLength();
         }

         csExpandedDirectory = csWork.Left(nIndex);
         nEndOfDriveIndex = csExpandedDirectory.GetLength();
      }
      
      else
      {
         WORD wDrive;
         
         // build the drive:
         
         if ((csWork.GetLength() >= 2) && (csWork[1] == ':'))
         {
            nIndex = 2;
            csExpandedDirectory = csWork.Left(2);
            
            wDrive = (WORD)csWork[0];
            if ((wDrive >= 'a') && (wDrive <= 'z'))
            {
               wDrive += (WORD)('A'-'a');
            }
            wDrive -= '@';
         }
         
         else
         {
#ifdef NOT_PORTABLE
            _asm
            {
               mov   ah,019h
               call  DOS3Call
               xor   ah,ah
               inc   ax
      
               mov   wDrive,ax
            }
#else
         /* Portable! (JN) */
            wDrive = _getdrive();
#endif
            
            csExpandedDirectory += (char)(wDrive+'@');
            csExpandedDirectory += ':';
         }
         
         nEndOfDriveIndex = csExpandedDirectory.GetLength();
      
         // if the path is relative, append the current path
         
         if ((nIndex >= csWork.GetLength()) || (csWork[nIndex] != '\\'))
         {
            char szCurrentDirectory[256];
            
            *szCurrentDirectory = '\0';
            
#ifdef NOT_PORTABLE
            _asm
            {
               mov   dx,wDrive
               lea   si,word ptr szCurrentDirectory
      
               mov   ah,047h
               call  DOS3Call
            }
#else
         /* Portable! (JN) */
            _getdcwd(wDrive, szCurrentDirectory, sizeof(szCurrentDirectory));

            if (szCurrentDirectory[0] != '\0'
                  && szCurrentDirectory[1] == ':'
                  && szCurrentDirectory[2] == '\\')
            {
            /* We have some kind of drive letter thingamabob on the front. Strip it. */
               strcpy(szCurrentDirectory, szCurrentDirectory+3);
            }
#endif
            
            if (*szCurrentDirectory != '\0')
            {
               csExpandedDirectory += '\\';
               csExpandedDirectory += szCurrentDirectory;
               
               // Set the current directory to the same case as the drive letter.
               
               if ((csExpandedDirectory[0] >= 'a') && (csExpandedDirectory[0] <= 'z'))
               {
                  csExpandedDirectory.MakeLower();
               }
               
               else
               {
                  csExpandedDirectory.MakeUpper();
               }
            }
         }
      }
      
      /*
      // Copy each directory element.
      */
      
      while (nIndex < csWork.GetLength())
      {
         // find the start of the path element
         
         while ((nIndex < csWork.GetLength()) && (csWork[nIndex] == '\\'))
         {
            nIndex++;
         }
         
         int nStart = nIndex;
         
         // find the end of the path element
         
         while ((nIndex < csWork.GetLength()) && (csWork[nIndex] != '\\'))
         {
            nIndex++;
         }
         
         // process the component (".", "..", or other)
         
         if (nStart != nIndex)
         {
            if (((nIndex-nStart) > 1) || (csWork[nStart] != '.'))
            {
               if (((nIndex-nStart) == 2) && (csWork[nStart] == '.') && (csWork[nStart+1] == '.'))
               {
                  // move back to start of last component
                  
                  int nBackupIndex = csExpandedDirectory.GetLength()-1;
                  
                  while ((nBackupIndex >= nEndOfDriveIndex) && (csExpandedDirectory[nBackupIndex] != '\\'))
                  {
                     nBackupIndex--;
                  }
                  
                  if (nBackupIndex >= nEndOfDriveIndex)
                  {
                     csExpandedDirectory = csExpandedDirectory.Left(nBackupIndex);
                  }
               }
               
               else
               {
                  // copy the component
                  
                  csExpandedDirectory += '\\';
                  csExpandedDirectory += csWork.Mid(nStart, nIndex-nStart);
               }
            }
         }
      }
      
      /*
      // Trailing backslash processing:
      //
      //    At this point, the current expanded path should not have a backslash
      //    on the end.
      //
      //    If the entire expanded path is drive:, then always append a
      //    backslash (even if fAppendBackslash is FALSE) to indicate the
      //    root directory.
      //
      //    Otherwise, append a backslash onto the end if the user has requested
      //    (fAppendBackslash is TRUE).
      */
      
      if ((csExpandedDirectory.GetLength() == 2) && (csExpandedDirectory[1] == ':'))
      {
         csExpandedDirectory += '\\';
      }
      
      else if (fAppendBackslash)
      {
         // make sure there is no backslash, just in case -- we don't want a double one.
         
         if ((csExpandedDirectory.GetLength() < 1) || (csExpandedDirectory[csExpandedDirectory.GetLength()-1] != '\\'))
         {
            csExpandedDirectory += '\\';
         }
      }
      
      csDirectory = csExpandedDirectory;
      
      fResult = TRUE;
   }
   CATCH_ALL(e)
   {
      csWork.Empty();
      csExpandedDirectory.Empty();
      
      THROW_LAST();
   }
   END_CATCH_ALL
   
// od(fResult ? "|%s|\n\r" : "FALSE\n\r", (LPCSTR)csDirectory);

   return fResult;
}

BOOL Util::DirectoryDrive(const CString csDirectory, CString &csDrive)
{
   BOOL fResult = FALSE;
   
   /*
   // Return the drive portion of a directory path. This will
   // have the form drive: for a normal connected path and
   // \\server\share for an unconnected path.
   */
   
   CString csExpanded;

   TRY
   {
      csDrive.Empty();
      csExpanded = csDirectory;

      if (ExpandDirectory(csExpanded, TRUE))
      {
         if ((csExpanded.GetLength() >= 2) && (csExpanded[0] == '\\') && (csExpanded[1] == '\\'))
         {
            BOOL fSeenServer = FALSE;
            BOOL fSeenShare = FALSE;
            
            int nIndex = 2;
            
            // skip past server name
               
            while ((nIndex < csExpanded.GetLength()) && (csExpanded[nIndex] != '\\'))
            {
               fSeenServer = TRUE;
               nIndex++;
            }
            
            nIndex++;
                      
            // skip past share name
               
            while ((nIndex < csExpanded.GetLength()) && (csExpanded[nIndex] != '\\'))
            {
               fSeenShare = TRUE;
               nIndex++;
            }
            
            // save name

            if (fSeenServer && fSeenShare)
            {
               csDrive = csExpanded.Left(nIndex);
               fResult = TRUE;
            }
         }
         
         else if ((csExpanded.GetLength() >= 2) && (csExpanded[1] == ':'))
         {
            csDrive = csExpanded.Left(2);
            fResult = TRUE;
         }
      }
      
      if (!fResult)
      {
         if (!csDirectory.IsEmpty())
         {
            csDrive = csDirectory;
            fResult = TRUE;
         }
      }
   }
   CATCH_ALL(e)
   {
      csDrive.Empty();
      csExpanded .Empty();
      
      THROW_LAST();
   }
   END_CATCH_ALL
   
   return fResult;
}

BOOL Util::FileExists(const CString& csFile, DWORD dwFilteredFileAttrib)
{
   BOOL fResult = FALSE;
   
   CString csExpanded;
   
   TRY
   {
      csExpanded = csFile;
      
      if (ExpandDirectory(csExpanded, FALSE))
      {
         csExpanded.AnsiToOem();
      
         LPCSTR lpszDirectory = csExpanded;
      
#ifdef WIN32
         WIN32_FIND_DATA ffd;
         HANDLE hSearch;

         if ((hSearch = FindFirstFile(csExpanded, &ffd)) != INVALID_HANDLE_VALUE)
         {
            if ((ffd.dwFileAttributes & dwFilteredFileAttrib) == 0)
            {
               fResult = TRUE;
            }
            FindClose(hSearch);
         }
#else
         _asm
         {
            /*
            // Check if the file exists using the DOS Get/Set
            // file attributes escape.
            */
                     
            push  ds
                     
            lds   dx,dword ptr lpszDirectory // DS:DX <-- pointer to directory name
                     
            xor   al,al                      // get attributes
            
            mov   ah,043h
            call  DOS3Call
                     
            pop   ds
            
            jc    l1
            
            /*
            // The file name was found, make sure it is a file.
            */
            
            test  cx,00018h
            jz l0
            
            /*
            // The file name exists, but it is not a file, return
            // an error.
            */
            
            jmp   l1
         }
         
      l0:
         
         fResult = TRUE;
         
      l1: ;
#endif
      
      }
   }
   CATCH_ALL(e)
   {
      csExpanded.Empty();
      
      THROW_LAST();
   }
   END_CATCH_ALL
   
   return fResult;
}

void Util::GetWindowsDirectory(CString& csDir)
{
	::GetWindowsDirectory(csDir.GetBuffer(_MAX_PATH), _MAX_PATH-1);
	csDir.ReleaseBuffer();
}

void Util::GetSystemDirectory(CString& csDir)
{
	::GetSystemDirectory(csDir.GetBuffer(_MAX_PATH), _MAX_PATH-1);
	csDir.ReleaseBuffer();
}

BOOL Util::IsWindowsDirectory(const CString& csDir)
{
	CString csWindowsDir;
	GetWindowsDirectory(csWindowsDir);
	return csDir.CompareNoCase(csWindowsDir) == 0;
}

BOOL Util::IsSystemDirectory(const CString& csDir)
{
	CString csSystemDir;
	GetSystemDirectory(csSystemDir);
	return csDir.CompareNoCase(csSystemDir) == 0;
}

/*
// LoadString()
//  throw( CMemoryException, CResourceException);
//
// Loads the string resource identified by 'nID' into CString 'csString'.
// Returns 'csString'.
*/

CString& Util::LoadString(UINT nID, CString& csString)
{
/* Try to load the string using the standard method. */

   if (!csString.LoadString(nID))
   {
   /* Resource not found. */
      AfxThrowResourceException();
   }

/* We got it! Return the result. */

   return csString;
}

/*
// SplitPath()
//  throw( CMemoryException )
//
// Split a full file name into its two components (path and file name).
*/

void Util::SplitPath(const CString &csFullPath, CString *pcsPath, CString *pcsFilename)
{
   int nIndex;

   if ((nIndex = csFullPath.ReverseFind('\\')) == -1)
   {
      nIndex = csFullPath.ReverseFind(':');
   }
   
   nIndex++;
   
   if (pcsPath != NULL)
   {
      *pcsPath = csFullPath.Left(nIndex);
   }
   
   if (pcsFilename != NULL)
   {
      *pcsFilename = csFullPath.Mid(nIndex);
   }
}

/*
// SplitPath()
//  throw( CMemoryException )
//
// Split a full file name into four components (drive, directory, file, extension).
//
// Comments:
//
//    The path is not expanded before parsing.
//    If the path does not contain a file name, then it must end with a backslash.
*/

void Util::SplitPath(const CString &csPath, CString *pcsDrive, CString *pcsDirectory, CString *pcsFile, CString *pcsExtension)
{
//TRACE1("SplitPath(%Fs) -> ", (LPCSTR)csPath);
   
   TRY
   {
      // clear out the result strings
      if (pcsDrive != NULL)      pcsDrive->Empty();
      if (pcsDirectory != NULL)  pcsDirectory->Empty();
      if (pcsFile != NULL)       pcsFile->Empty();
      if (pcsExtension != NULL)  pcsExtension->Empty();

      int nDirectoryIndex = 0;
         
      // collect the drive
         
      // check for connected syntax: \\server\share\ 
      if ((csPath.GetLength() >= 2) && (csPath[0] == '\\') && (csPath[1] == '\\'))
      {
         BOOL fSeenServer = FALSE;
         BOOL fSeenShare = FALSE;
               
         nDirectoryIndex = 2;
               
         // skip past server name
                  
         while ((nDirectoryIndex < csPath.GetLength()) && (csPath[nDirectoryIndex] != '\\'))
         {
            fSeenServer = TRUE;
            nDirectoryIndex++;
         }
               
         nDirectoryIndex++;
                         
         // skip past share name
                  
         while ((nDirectoryIndex < csPath.GetLength()) && (csPath[nDirectoryIndex] != '\\'))
         {
            fSeenShare = TRUE;
            nDirectoryIndex++;
         }
               
         // save drive name
   
         if (!fSeenServer || !fSeenShare)
         {
            nDirectoryIndex = 0;
         }
      }
               
      else if ((csPath.GetLength() >= 2) && (csPath[1] == ':'))
      {
         nDirectoryIndex = 2;
      }
            
      if (pcsDrive != NULL)
      {
         *pcsDrive = csPath.Left(nDirectoryIndex);
      }
         
      // collect the directory
      
      int nFileIndex = csPath.ReverseFind('\\')+1;
      
      if (nFileIndex < nDirectoryIndex)
      {
         nFileIndex = nDirectoryIndex;
      }
      
      if (pcsDirectory != NULL)
      {
         *pcsDirectory = csPath.Mid(nDirectoryIndex, nFileIndex-nDirectoryIndex);
      }
      
      // collect the file name
      
      int nExtensionIndex = csPath.ReverseFind('.');
      
      if (nExtensionIndex == -1)
      {
         nExtensionIndex = csPath.GetLength();
      }
      
      if (nExtensionIndex < nFileIndex)
      {
         nExtensionIndex = csPath.GetLength();
      }
      
      if (pcsFile != NULL)
      {
         *pcsFile = csPath.Mid(nFileIndex, nExtensionIndex-nFileIndex);
      }
      
      // collect the extension
      
      if (pcsExtension != NULL)
      {
         *pcsExtension = csPath.Mid(nExtensionIndex);
      }
   }
   CATCH_ALL(e)
   {
      if (pcsDrive != NULL)      pcsDrive->Empty();
      if (pcsDirectory != NULL)  pcsDirectory->Empty();
      if (pcsFile != NULL)       pcsFile->Empty();
      if (pcsExtension != NULL)  pcsExtension->Empty();
      
      THROW_LAST();
   }
   END_CATCH_ALL
//if (pcsDrive != NULL) TRACE1("Drive=|%Fs| ", (LPCSTR)(*pcsDrive));
//if (pcsDirectory != NULL) TRACE1("Directory=|%Fs| ", (LPCSTR)(*pcsDirectory));
//if (pcsFile != NULL) TRACE1("File=|%Fs| ", (LPCSTR)(*pcsFile));
//if (pcsExtension != NULL) TRACE1("Extension=|%Fs| ", (LPCSTR)(*pcsExtension));
//TRACE0("\n");
}

/*
// AppendBackslashToPath()
//  throw( CMemoryException )
//
// Make sure a path ends with a backslash.
*/

CString& Util::AppendBackslashToPath(CString &csPath)
{
   if ((!csPath.IsEmpty()) && (csPath[csPath.GetLength()-1] != '\\'))
   {
      csPath += '\\';
   }
   
   return csPath;
}

/*
// RemoveBackslashFromPath()
//  throw( CMemoryException )
//
// Make sure a path ends with a backslash.
*/

CString& Util::RemoveBackslashFromPath(CString &csPath, BOOL fKeepRootDirectoryBackslash /* = TRUE */)
{
   if ((!csPath.IsEmpty()) && (csPath[csPath.GetLength()-1] == '\\'))
   {
      csPath = csPath.Left(csPath.GetLength()-1);
   }
   
   if (fKeepRootDirectoryBackslash)
   {
      if ((csPath.GetLength() == 2) && (csPath[1] == ':'))
      {
         csPath += '\\';
      }
   }
   
   return csPath;
}

/*
// ConstructPath()
//  throw( CMemoryException )
//
// Construct a full file name from its two components (path and file name).
*/

CString& Util::ConstructPath(CString &csFullPath, const CString &csPath, const CString &csFileName)
{
   csFullPath = csPath;
   AppendBackslashToPath(csFullPath);
   csFullPath += csFileName;
   
   return csFullPath;
}


/*
// ConstructPath()
//  throw( CMemoryException )
//
// Construct a full file name from four components (drive, directory, file, and extension)
//
// Assumptions:
//
//    csDrive does not end with a backslash
//    csFile does not start with a backslash
//    csFile does not end with a dot
*/

CString& Util::ConstructPath(CString &csPath, const CString &csDrive, const CString &csDirectory, const CString &csFile, const CString &csExtension)
{
   // start with drive
   csPath = csDrive;
   
   // append directory
   csPath += csDirectory;
   
   // append file
   AppendBackslashToPath(csPath);
   csPath += csFile;
   
   // append extension
   if (!csExtension.IsEmpty())
   {
      if (csExtension[0] != '.')
      {
         csPath += '.';
      }
      
      csPath += csExtension;
   }
   
   return csPath;
}

/*
// FormatString(), FormatStringV()
//    throw( CMemoryException, CResourceException )
//
// Primitive printf() formatting utility. Can take resource ID's in place
// of string pointers.
//
// Currently supported formats:
//
//    %d - signed integer (sizeof(int) bytes on stack)
//    %s - FAR string (sizeof(LPSTR) bytes on stack)
//          if high word is zero, then low word is resource ID
*/

const CString& Util::FormatString(CString &csOutput, LPCSTR lpszFormat, ...)
{
   return FormatStringV(csOutput, lpszFormat, (LPCSTR)(&lpszFormat)+sizeof(lpszFormat));
}

const CString& Util::FormatStringV(CString &csOutput, LPCSTR lpszFormat, LPCSTR lpArgs)
{
   #define  IS_RESOURCE_ID(s)       (HIWORD((LPVOID)(s))==0)
   #define  EXTRACT_RESOURCE_ID(s)  (LOWORD((LPVOID)(s)))
   
   CString csResource;
   CString csFormat;
   
   TRY
   {
      /*
      // Get the format string into a CString.
      */
      
      if (IS_RESOURCE_ID(lpszFormat))
      {
         LoadString(EXTRACT_RESOURCE_ID(lpszFormat), csFormat);
      }
      
      else
      {
         // convert LPCSTR into CString
         csFormat = lpszFormat;
      }
      
      /*
      // Now, march through the format string and do the "printf" operation.
      */
      
      int nIndex = 0;
      
      while (nIndex < csFormat.GetLength())
      {
         if (csFormat[nIndex] == '%')
         {
            // format specifier
            
            nIndex++;
            
            if (nIndex < csFormat.GetLength())
            {
               switch (csFormat[nIndex])
               {
                  case 'd':
                  {
                     // integer
                     int nArg = *((LPINT)lpArgs);
                     lpArgs += sizeof(int);
                     char szBuffer[20];
                     itoa(nArg, szBuffer, 10);
                     csOutput += szBuffer;
                     break;
                  }
                  
                  case 's':
                  {
                     // string
                     LPCSTR lpszArg = *((LPCSTR FAR *)lpArgs);
                     lpArgs += sizeof(LPCSTR);
                     
                     if (IS_RESOURCE_ID(lpszArg))
                     {
                        LoadString(EXTRACT_RESOURCE_ID(lpszArg), csResource);
                        csOutput += csResource;
                     }
                     
                     else
                     {
                        csOutput += lpszArg;
                     }
                     
                     break;   
                  }
                  
                  default:
                  {
                     csOutput += csFormat[nIndex];
                     break;
                  }
               }
            }
         }
         
         else
         {
            // normal character
            csOutput += csFormat[nIndex];
         }
         
         nIndex++;
      }
   }
   CATCH_ALL(e)
   {
      csResource.Empty();
      csFormat.Empty();
      
      THROW_LAST();
   }
   END_CATCH_ALL
   
   return csOutput;
}

/*
// MessageBox()
//    throw( CMemoryException, CResourceException )
//
// Message box function that takes printf() style arguments.
*/

int Util::MessageBox(UINT uType, UINT uHelpID, LPCSTR lpszFormat, ...)
{
   CString csMessage;
   int nResult;
   
   TRY
   {
      FormatStringV(csMessage, lpszFormat, (LPCSTR)(&lpszFormat)+sizeof(lpszFormat));
      nResult = AfxMessageBox(csMessage, uType, uHelpID);
   }
   CATCH_ALL(e)
   {
      csMessage.Empty();
      
      THROW_LAST();
   }
   END_CATCH_ALL
   
   return nResult;
}
// Only for WIN32 _DEBUG (else like ASSERT)
// Provides a diagnostic message box that displays return from GetLastError
// and a system supplied string
#ifdef ERRORBOX_ALLOWED
void  Util::ErrorBox (const char * source)
{
   DWORD   len, le;
   char    buff [81], num [33];
   CString text = '#';

   le = GetLastError ();
  
   len = FormatMessage (
                           FORMAT_MESSAGE_FROM_SYSTEM,
                           NULL,
                           le,
                           MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                           buff,
                           80,
                           NULL
                        );
  
   text += ltoa (le, num, 10);
   if (len > 0)
   {
      text += ' ';
      text += buff;
   }
  
   ::MessageBox  (
                     NULL,
                     (const char *)text,
                     source,
                     MB_OK | MB_APPLMODAL
                  );
}
#endif

/*
// GetTypeOfDrive()
//
// Get the type for a drive.
*/

UINT Util::GetTypeOfDrive(char cDrive)
{
#ifdef WIN32
   char cbRoot[] = "X:\\";
   cbRoot[0] = cDrive;
   return ::GetDriveType(cbRoot);
#else
   return ::GetDriveType(cDrive - 'A');
#endif
}

/*
// IsRootDirectory(const CString& csDirectory)
//
// Return TRUE if the directory is a root directory.
*/

BOOL Util::IsRootDirectory(const CString& csDirectory)
{
   BOOL fResult = FALSE;
   
   TRY
   {
      CString csDrive;

      if ((!DirectoryDrive(csDirectory, csDrive))
       || (csDrive == csDirectory)
       || ((csDrive + '\\') == csDirectory))
      {
         fResult = TRUE;
      }
   }
   END_TRY
   
   return fResult;
}

/*
// Huge memory move, supporting overlapping memory blocks.
// Thanks Dr. GUI!
*/

#ifndef WIN32
void Util::HMemMove(
   char __huge  *hpSource,        // Source buffer pointer
   char __huge  *hpDest,          // Destination buffer pointer
   unsigned long  lCount)         // Count of bytes to move
{

   UINT         ucount;           // Temporary short int
   BOOL         bReverse = FALSE; // Bottom up?

// If we are moving less than 64K, we can do it all in one call.
   if (HIWORD(lCount) == 0) {
      _fmemmove(hpDest, hpSource, LOWORD(lCount));
      return;
   }

// We are now dealing with >64K of data. If the destination 
// is lower in memory than the source, we can repetitively 
// copy 64K blocks from the top of each block. If the destination
// is higher in memory than the source, we must repetitively move 
// 64K blocks from the bottom of the source and destination to
// prevent overwriting the source. We start by adjusting the source
// and destination pointers to point at the last 64K chunk in the
// block and set a flag (bReverse). After each block move, we move
// the source and destination pointers up or down appropriately.

   if (hpDest > hpSource) {
      hpSource += (lCount - 65535);  
      hpDest += (lCount - 65535);
      bReverse = TRUE;
   }
   while (TRUE) {
      if (HIWORD(lCount))
         ucount = 65535;       // >64K left; set ucount to 64K
      else
         ucount = LOWORD(lCount);
      _fmemmove(hpDest, hpSource, ucount);
      lCount -= ucount;        // Reduce the amount left to move
      if (lCount == 0)
         return;               // All done; return
      if (bReverse) {          // Update source/destination pointers
         if (HIWORD(lCount)) {
            hpDest -= ucount;
            hpSource -= ucount;
         } else {
            hpDest -= LOWORD(lCount);
            hpSource -= LOWORD(lCount);
         }
      } else {
         hpDest += ucount;
         hpSource += ucount;
      }
   }
   return;
}
#endif

BOOL Util::Draw3dBorder(
      CDC* pDC,
      const CRect& crBorder,
      COLORREF clHighlight,
      COLORREF clShadow,
      COLORREF clBorderHighlight,
      COLORREF clBorderShadow,
      COLORREF clBackground,
      int nDepth /*=2*/,
      BOOL fDrawInsideBorder /*=FALSE*/,
      BOOL fDrawBackground /*=FALSE*/,
      BOOL fMask /*=FALSE*/)
{
   BOOL fResult = FALSE;
   int nContext = 0;
   CPen cpHighlight;
   CPen cpShadow;
   CPen cpBorderHighlight;
   CPen cpBorderShadow;
   CBrush cbBackground;
   
   TRY
   {
      if (pDC != NULL)
      {
         nContext = pDC->SaveDC();
         
         if (nContext != 0)
         {
            int i;
            
            // Draw 3D border.
            
            if (nDepth > 0)
            {
               // Draw shadow.
                        
               cpShadow.CreatePen(PS_SOLID, 0, fMask ? RGB(0,0,0) : clShadow);
               pDC->SelectObject(&cpShadow);
               
               for (i = 0; i < nDepth; i++)
               {
                  pDC->MoveTo(crBorder.left, crBorder.top+i);
                  pDC->LineTo(crBorder.right-1-i, crBorder.top+i);
               
                  pDC->MoveTo(crBorder.left+i, crBorder.top+nDepth);
                  pDC->LineTo(crBorder.left+i, crBorder.bottom-1-i);
               }
               
               // Draw highlight.
               
               cpHighlight.CreatePen(PS_SOLID, 0, fMask ? RGB(0,0,0) : clHighlight);
               pDC->SelectObject(&cpHighlight);
               
               for (i = 0; i < nDepth; i++)
               {
                  pDC->MoveTo(crBorder.left+i, crBorder.bottom-1-i);
                  pDC->LineTo(crBorder.right, crBorder.bottom-1-i);
               
                  pDC->MoveTo(crBorder.right-1-i, crBorder.top+i);
                  pDC->LineTo(crBorder.right-1-i, crBorder.bottom-nDepth);
               }
            }
            
            if (fDrawInsideBorder)
            {
               // Draw inside border.
            
               cpBorderShadow.CreatePen(PS_SOLID, 0, fMask ? RGB(0,0,0) : clBorderShadow);
               pDC->SelectObject(&cpBorderShadow);
            
               pDC->MoveTo(crBorder.left+nDepth, crBorder.top+nDepth);
               pDC->LineTo(crBorder.right-1-nDepth, crBorder.top+nDepth);
               
               pDC->MoveTo(crBorder.left+nDepth, crBorder.top+nDepth+1);
               pDC->LineTo(crBorder.left+nDepth, crBorder.bottom-nDepth-1);
               
               cpBorderHighlight.CreatePen(PS_SOLID, 0, fMask ? RGB(0,0,0) : clBorderHighlight);
               pDC->SelectObject(&cpBorderHighlight);
               
               pDC->MoveTo(crBorder.left+nDepth, crBorder.bottom-nDepth-1);
               pDC->LineTo(crBorder.right-nDepth, crBorder.bottom-nDepth-1);
               
               pDC->MoveTo(crBorder.right-nDepth-1, crBorder.top+nDepth);
               pDC->LineTo(crBorder.right-nDepth-1, crBorder.bottom-nDepth-1);
            }
            
            if (fDrawBackground)
            {
               // Fill background.
               
               if (cbBackground.CreateSolidBrush(clBackground))
               {
                  CRect crBackground(crBorder);
                  int nOffset = nDepth+(fDrawInsideBorder ? 1 : 0);
                  crBackground.InflateRect(-nOffset, -nOffset);
                  pDC->FillRect(crBackground, &cbBackground);
               }
            }
            
            pDC->RestoreDC(nContext);
            nContext = 0;
            
            fResult = TRUE;
         }
      }
   }
   CATCH_ALL(e)
   {
      fResult = FALSE;
      
      if (nContext != 0)
      {
         pDC->RestoreDC(nContext);
         nContext = 0;
      }
   }
   END_CATCH_ALL
   
   return fResult;
}

BOOL Util::Draw3dButton(CDC* pDC, const CRect& crButton, COLORREF clBorder, COLORREF clHighlight, COLORREF clShadow, COLORREF clFace, int nDepth /*=2*/, BOOL fMask /*=FALSE*/)
{
   BOOL fResult = FALSE;
   int nContext = 0;
   CPen cpBorder;
   CPen cpHighlight;
   CPen cpShadow;
   CBrush cbFace;
   
   TRY
   {
      if (pDC != NULL)
      {
         nContext = pDC->SaveDC();
         
         if (nContext != 0)
         {
            int i;
            
            /* draw outside border */
            
            cpBorder.CreatePen(PS_SOLID, 0, fMask ? RGB(0,0,0) : clBorder);
            pDC->SelectObject(&cpBorder);
            
            pDC->MoveTo(crButton.left+1, crButton.top);
            pDC->LineTo(crButton.right-1, crButton.top);
            
            pDC->MoveTo(crButton.left+1, crButton.bottom-1);
            pDC->LineTo(crButton.right-1, crButton.bottom-1);
            
            pDC->MoveTo(crButton.left, crButton.top+1);
            pDC->LineTo(crButton.left, crButton.bottom-1);
            
            pDC->MoveTo(crButton.right-1, crButton.top+1);
            pDC->LineTo(crButton.right-1, crButton.bottom-1);
            
            if (nDepth > 0)
            {
               /* draw highlight */
                        
               cpHighlight.CreatePen(PS_SOLID, 0, fMask ? RGB(0,0,0) : clHighlight);
               pDC->SelectObject(&cpHighlight);
               
               for (i = 0; i < nDepth; i++)
               {
                  pDC->MoveTo(crButton.left+1, crButton.top+1+i);
                  pDC->LineTo(crButton.right-2-i, crButton.top+1+i);
               
                  pDC->MoveTo(crButton.left+1+i, crButton.top+2);
                  pDC->LineTo(crButton.left+1+i, crButton.bottom-2-i);
               }
               
               /* draw shadow */
               
               cpShadow.CreatePen(PS_SOLID, 0, fMask ? RGB(0,0,0) : clShadow);
               pDC->SelectObject(&cpShadow);
               
               for (i = 0; i < nDepth; i++)
               {
                  pDC->MoveTo(crButton.left+1+i, crButton.bottom-2-i);
                  pDC->LineTo(crButton.right-1, crButton.bottom-2-i);
               
                  pDC->MoveTo(crButton.right-2-i, crButton.top+1+i);
                  pDC->LineTo(crButton.right-2-i, crButton.bottom-2);
               }
            }
            
            /* draw face */
            
            cbFace.CreateSolidBrush(fMask ? RGB(0,0,0) : clFace);
            CRect crFace(crButton.left+nDepth+1, crButton.top+nDepth+1, crButton.right-nDepth-1, crButton.bottom-nDepth-1);
            
            pDC->FillRect(crFace, &cbFace);
            
            pDC->RestoreDC(nContext);
            nContext = 0;
            
            fResult = TRUE;
         }
      }
   }
   CATCH_ALL(e)
   {
      fResult = FALSE;
      
      if (nContext != 0)
      {
         pDC->RestoreDC(nContext);
         nContext = 0;
      }
   }
   END_CATCH_ALL
   
   return fResult;
}


void Util::FitAspect(int* pnAspectX, int* pnAspectY, int nTargetX, int nTargetY)
{
   if (((long)(((long)(*pnAspectX))*((long)nTargetY))) < ((long)(((long)(*pnAspectY))*((long)nTargetX))))
   {
      *pnAspectX = MulDiv(*pnAspectX, nTargetY, *pnAspectY);
      *pnAspectY = nTargetY;
   }
   else
   {
      *pnAspectY = MulDiv(*pnAspectY, nTargetX, *pnAspectX);
      *pnAspectX = nTargetX;
   }
}

BOOL Util::ValidSignedNumber(LPCSTR pString, long lMinimum /*=-0x7fffffff*/, long lMaximum /*=0x7fffffff*/)
{
   BOOL fNegative = FALSE;

   while ((*pString == ' ') || (*pString == '\t'))
   {
      pString++;
   }
   
   if (*pString == '-')
   {
      fNegative = TRUE;
      pString++;
   }
   
   if (*pString == '+')
   {
      pString++;
   }
   
   BOOL fSeenDigit = FALSE;
   unsigned long u = 0;
   
   while ((*pString >= '0') && (*pString <= '9'))
   {
      fSeenDigit = TRUE;
      
      u = (u*10)+(*pString-'0');
      
      if (u > 0x7fffffff)
      {
         return FALSE;
      }
      
      pString++;
   }
   
   if (!fSeenDigit)
   {
      return FALSE;
   }
   
   while ((*pString == ' ') || (*pString == '\t'))
   {
      pString++;
   }
   
   if (*pString != '\0')
   {
      return FALSE;
   }
   
   long c = (long)u;
   
   if (fNegative)
   {
      c = -c;
   }
   
   if ((c < lMinimum) || (c > lMaximum))
   {
      return FALSE;
   }
   
   return TRUE;
}

BOOL Util::ValidUnsignedNumber(LPCSTR pString, unsigned long lMinimum /*=0*/, unsigned long lMaximum /*=0xffffffff*/)
{
   while ((*pString == ' ') || (*pString == '\t'))
   {
      pString++;
   }
   
   BOOL fSeenDigit = FALSE;
   unsigned long c = 0;
   
   while ((*pString >= '0') && (*pString <= '9'))
   {
      fSeenDigit = TRUE;
      
      unsigned long n = (c*10)+(*pString-'0');
      
      if (((n-(*pString-'0'))/10) != c)
      {
         return FALSE;
      }
      
      c = n;
      pString++;
   }
   
   if (!fSeenDigit)
   {
      return FALSE;
   }
   
   while ((*pString == ' ') || (*pString == '\t'))
   {
      pString++;
   }
   
   if (*pString != '\0')
   {
      return FALSE;
   }
   
   if ((c < lMinimum) || (c > lMaximum))
   {
      return FALSE;
   }
   
   return TRUE;
}

BOOL Util::ValidDouble(LPCSTR pString, double dMinimum /*=-1e308*/, double dMaximum /*=1e308*/)
{
   double d;
   
   TRY
   {
      d = atof(pString);
   }
   CATCH_ALL(e)
   {
      return FALSE;
   }
   END_CATCH_ALL
   
   while ((*pString == ' ') || (*pString == '\t'))
   {
      pString++;
   }
   
   if (*pString == '-')
   {
      pString++;
   }
   
   if (*pString == '+')
   {
      pString++;
   }
   
   BOOL fSeenDigit = FALSE;
   
   while ((*pString >= '0') && (*pString <= '9'))
   {
      fSeenDigit = TRUE;
      pString++;
   }

#ifdef LOCALIZE   
   if (*pString == CurrentLocale.DecimalPoint())
#else
   if (*pString == '.')
#endif
   {
      pString++;
      
      while ((*pString >= '0') && (*pString <= '9'))
      {
         fSeenDigit = TRUE;
         pString++;
      }
   }
   
   if (!fSeenDigit)
   {
      return FALSE;
   }
   
   if ((*pString == 'd')
    || (*pString == 'D')
    || (*pString == 'e')
    || (*pString == 'E'))
   {
      pString++;
      
      if (*pString == '-')
      {
         pString++;
      }
      
      if (*pString == '+')
      {
         pString++;
      }
   
      while ((*pString >= '0') && (*pString <= '9'))
      {
         fSeenDigit = TRUE;
         pString++;
      }
   }
   
   while ((*pString == ' ') || (*pString == '\t'))
   {
      pString++;
   }
   
   if (*pString != '\0')
   {
      return FALSE;
   }
   
   if ((d < dMinimum) || (d > dMaximum))
   {
      return FALSE;
   }
   
   return TRUE;
}

void Util::PriceString(CString& csPrice, long lPrice, BOOL fDollarSign /*=TRUE*/, LPCSTR pszBlankString /*=NULL*/)
{
	if ((lPrice == 0) && (pszBlankString != NULL))
	{
		csPrice = pszBlankString;
	}
		
	else
	{
		char szPrice[20];

		long lAbsPrice = (lPrice >= 0) ? lPrice : -lPrice;
			
		sprintf(szPrice, fDollarSign ? "$%ld.%02ld" : "%ld.%02ld", lPrice/100L, lAbsPrice % 100L);
		csPrice = szPrice;
	}
}

BOOL Util::Trim(CString& csString, BOOL fTrimFront /*=TRUE*/, BOOL fTrimBack /*=TRUE*/)
{
   int nLength = csString.GetLength();
   int nStart = 0;
   int nEnd = nLength-1;
   
   if (fTrimFront)
   {
      for (; nStart < nLength; nStart++)
      {
         if ((csString[nStart] != ' ') && (csString[nStart] != '\t'))
         {
            break;
         }
      }
   }
   
   if (fTrimBack)
   {
      for (; nEnd >= 0; nEnd--)
      {
         if ((csString[nEnd] != ' ') && (csString[nEnd] != '\t'))
         {
            break;
         }
      }
   }
   
   if (nStart > nEnd)
   {
      csString.Empty();
   }
   else
   {
      csString = csString.Mid(nStart, nEnd-nStart+1);
   }
   
   return !csString.IsEmpty();
}

BOOL Util::TrimQuotes(CString& csString, BOOL fTrimFront /*=TRUE*/, BOOL fTrimBack /*=TRUE*/)
{
   int nLength = csString.GetLength();
   int nStart = 0;
   int nEnd = nLength-1;
   
   if (fTrimFront)
   {
      for (; nStart < nLength; nStart++)
      {
         if (csString[nStart] != '"')
         {
            break;
         }
      }
   }
   
   if (fTrimBack)
   {
      for (; nEnd >= 0; nEnd--)
      {
         if (csString[nEnd] != '"')
         {
            break;
         }
      }
   }
   
   if (nStart > nEnd)
   {
      csString.Empty();
   }
   else
   {
      csString = csString.Mid(nStart, nEnd-nStart+1);
   }
   
   return !csString.IsEmpty();
}

void Util::Capitalize(CString &strToCapitalize, BOOL fRemoveExtraSpaces /*=TRUE*/)
   {
      int               i, nNumWords, nLastWord;
      char              chCharToChange;
		CStringArray		csaWordFields;

      // Capitalize words by parsing words for this field into string array 
      // then rebuilding complete string after turning first letter of each word
      // to uppercase

#ifdef LOCALIZE
// cb use CharLower instead of MakeLower for localization-friendly results
      LPSTR lpszTemp = strToCapitalize.GetBuffer(strlen(strToCapitalize)*2);
      CharLower(lpszTemp);
		strToCapitalize.ReleaseBuffer(-1);
#else
      strToCapitalize.MakeLower();
#endif
		FillArrayFromDelimitedString(strToCapitalize, " ", csaWordFields);
      strToCapitalize.Empty();
      for(i=0, nNumWords = csaWordFields.GetSize(), nLastWord = nNumWords-1;
         i<nNumWords;i++)
      {
         if (csaWordFields[i].IsEmpty())
         {
            // Word is empty. Eliminate extra spaces if that's what the user wants.
            if (fRemoveExtraSpaces)
            {
               continue;
            }
         }
         else
         {
            // Uppercase the first character.
            chCharToChange = csaWordFields[i][0];
            csaWordFields[i].SetAt(0, toupper(chCharToChange));
            strToCapitalize += csaWordFields[i];
         }

         // Separate words with a space.
         if(i != nLastWord)
            strToCapitalize += ' ';
      }
   }

// Replaces all characters of a given value with another one
void Util::ReplaceChar(CString &strToUse, char chCharToReplace, char chNewChar)
   {
      int      nCharIndex;
      char *   pStrBuffer;

      while( (nCharIndex = strToUse.Find(chCharToReplace)) >= 0)
         {
            pStrBuffer = strToUse.GetBuffer(1);
            if(pStrBuffer)
               pStrBuffer[nCharIndex] = chNewChar;
            strToUse.ReleaseBuffer();
         }
   }

// Replace all characters of a given value 
void Util::RemoveChar(CString& csWord, enum CharType enChType, enum Position enPosition)
   {         
      
      int   nLength = csWord.GetLength();
      int   (*pIsFunction)(int ch);

      ASSERT(nLength);
      if(nLength <= 0)
         return;

      // Allocate working buffer
      char *   pBuffer = new char [nLength+1];
      ASSERT(pBuffer);
      // Init working char pointer to working buffer start
      char *   pChar = pBuffer;

      switch(enChType)
         {
            case NotAlphaNumeric:
            default:
               pIsFunction = isalnum;
               break;
            case NotAlpha:
               pIsFunction = isalpha;
               break;
            case NotDigit:
               pIsFunction = isdigit;
               break;
         }
      for (int nChar = 0; nChar < nLength; nChar++)
      {
         if (pIsFunction(csWord[nChar]))
            *(pChar++) = csWord[nChar];
      }
      *pChar = '\0';
      // Copy working buffer contents to callers string
      csWord = pBuffer;
      // Release working buffer
      delete pBuffer;
   }

// Copies CString contents to a char * destination for a max of n characters
// including NULL termination
char * Util::SafeStringCopy(char *szDest, CString strSource, size_t nDestBuffSize)
   {
      ASSERT(szDest);
      ASSERT(nDestBuffSize);

      if(nDestBuffSize <= 0)
         return szDest;
      strncpy(szDest, strSource.GetBuffer(1), nDestBuffSize-1);
      strSource.ReleaseBuffer();
      szDest[nDestBuffSize-1] = NULL;
      return szDest;
   }

BOOL Util::IsStrInArray(const CStringArray& strArray, const CString& str, BOOL fNoCase/*=TRUE*/)
{
	for (int i = 0; i < strArray.GetSize(); i++)
	{
		if (fNoCase)
		{
			if (strArray[i].CompareNoCase(str) == 0)
				return TRUE;
		}
		else
		{
			if (strArray[i].Compare(str) == 0)
				return TRUE;
		}
	}

	return FALSE;
}

void Util::SwapWORDS(WORD* pData, long lCount)
{
#ifdef WIN32
   WORD* p = pData;
#else
   WORD huge* p = (WORD huge*)pData;
#endif
   while (lCount-- > 0)
   {
#ifdef _WIN32
      BYTE tmp = ((BYTE*)p)[0];
      ((BYTE*)p)[0] = ((BYTE*)p)[1];
      ((BYTE*)p)[1] = tmp;
#else
      _asm
      {
         les   bx, dword ptr p
         mov   ax, es:[bx]
         xchg  ah, al
         mov   es:[bx], ax
      }
#endif
      p++;
   }
}

void Util::SwapWORD(WORD* w)
{
#ifdef _WIN32
   BYTE tmp = ((BYTE*)w)[0];
   ((BYTE*)w)[0] = ((BYTE*)w)[1];
   ((BYTE*)w)[1] = tmp;
#else
   _asm
   {
      les   bx, dword ptr w
      mov   ax, es:[bx]
      xchg  ah, al
      mov   es:[bx], ax
   }
#endif
}

/*
// Swap a long word of data.
*/

void Util::SwapDWORD(DWORD* l)
{
#ifdef _WIN32
   WORD tmp = ((WORD*)l)[0];
   ((WORD*)l)[0] = ((WORD*)l)[1];
   ((WORD*)l)[1] = tmp;

   SwapWORD(((WORD *)l)+0);
   SwapWORD(((WORD *)l)+1);
#else
   _asm
   {
      les   bx, dword ptr l

      mov   ax, es:[bx+0]
      mov   dx, es:[bx+2]
      xchg  ah, al
      xchg  dh, dl
      mov   es:[bx+2], ax
      mov   es:[bx+0], dx
   }
#endif
}

BOOL Util::FileSharingAvailable(void)
{
#ifdef WIN32
   return TRUE;   // must be 95, NT, or something even better.
#else
   BOOL fSharingAvailable = TRUE;

   // File sharing is automatically available on Windows for Workgroups, Windows NT,
   // and Windows 95. We only need to check if we have Windows 3.1 or 3.0.
   
   if ((LOWORD(GetVersion()) == 0x0003) || (LOWORD(GetVersion()) == 0x0a03))
   {
      // To check if sharing is available, we need create a file and try to lock
      // it. If successful, then we assume sharing is available.
      
      fSharingAvailable = FALSE;    // expect the worst.
      
      // To create a file, we need a writable directory on a local hard drive.

      int nDrive;
      
      for (nDrive = 0; nDrive < 26; nDrive++)
      {
         // Check if the drive type is fixed.
         if (GetDriveType(nDrive) == DRIVE_FIXED)
         {
            // Found a fixed drive, try creating the temp file in the root
            // directory of the drive.
            
            BOOL fFoundName = FALSE;

            // Generate a unique file name to use.
            char szName[_MAX_PATH];
            for (int nCount = 0; nCount < 500; nCount++)
            {
               wsprintf(szName, "%c:\\PMW%05d.$$$", nDrive+'A', nCount);
               if (!Util::FileExists(szName))
               {
                  fFoundName = TRUE;
                  break;
               }
            }
            
            // Make sure we have a name to use.
            if (fFoundName)
            {
               // We have a test file name, attempt to create it and lock it.
               int nFile = _open(szName, _O_BINARY|_O_CREAT|_O_EXCL|_O_RDWR, _S_IREAD|_S_IWRITE);
               if (nFile != -1)
               {
                  // We created the file, attempt to lock it.
                  BOOL fLocked = _locking(nFile, _LK_NBLCK, 1) != -1;
                  
                  // Get rid of the file. Closing the file should remove any locks.
                  _close(nFile);
                  _unlink(szName);
                  
                  if (fLocked)
                  {
                     // We were able to lock the file. File sharing is available.
                     fSharingAvailable = TRUE;
                     break;
                  }
               }
            }
         }
      }
   }
   
   return fSharingAvailable;
#endif
}

void Util::FillRectangleWithColor(CDC& dc, const CRect& crRect, COLORREF clColor)
{
   COLORREF clOld = dc.SetBkColor(clColor);
   dc.ExtTextOut(0, 0, ETO_OPAQUE, crRect, NULL, 0, NULL);
   dc.SetBkColor(clOld);
}

void Util::DrawBorderWithColor(CDC& dc, const CRect& crRect, COLORREF clColor, int nSize /*=1*/)
{
   if ((nSize*2 >= crRect.Width()) || (nSize*2 >= crRect.Height()))
   {
      // Border fills the entire rectangle.
      FillRectangleWithColor(dc, crRect, clColor);
   }
   else
   {
      // Draw each side.
      CRect crSide;

      // Top
      crSide.SetRect(crRect.left, crRect.top, crRect.right, crRect.top+nSize);
      FillRectangleWithColor(dc, crSide, clColor);

      // Bottom
      crSide.OffsetRect(0, crRect.Height()-nSize);
      FillRectangleWithColor(dc, crSide, clColor);

      // Left
      crSide.SetRect(crRect.left, crRect.top+nSize, crRect.left+nSize, crRect.bottom-nSize);
      FillRectangleWithColor(dc, crSide, clColor);

      // Right
      crSide.OffsetRect(crRect.Width()-nSize, 0);
      FillRectangleWithColor(dc, crSide, clColor);
   }
}

#define ROP_PSDPxax  0x00B8074AL    // S is 1: D unchanged, S is 0: D <- P (fInvertMask = FALSE)
#define ROP_DSPDxax  0x00E20746L    // S is 0: D unchanged, S is 1: D <- P (fInvertMask = TRUE)

void Util::DrawBrushThroughMask(CDC& dcDestination, const CRect& crDestination, CDC& dcMask, const CRect& crMask, CBrush& cbBrush, BOOL fInvertMask /*=FALSE*/)
{
   // Figure out the ROP to use. Normally, we draw where the mask is 0. The user can
   // invert this behavior by passing TRUE for fInvertMask. Note that if we were sure
   // the mask bitmap was monochrome, we could swap the text and background color settings
   // to invert the meaning of the mask. I doubt if there is an advantage one way or the other.
   DWORD dwRop = fInvertMask ? ROP_DSPDxax : ROP_PSDPxax;

   if (dcDestination.SaveDC() != 0)
   {
      // We want to set the foreground and background colors since the
      // mask is probably a monochrome bitmap.
      dcDestination.SetTextColor(RGB(0,0,0));      // 0's in mask will be 0's for ROP
      dcDestination.SetBkColor(RGB(255,255,255));  // 1's in mask will be 1's for ROP
      dcDestination.SetStretchBltMode(STRETCH_DELETESCANS);

      // Select the brush into the DC.
      dcDestination.SelectObject(&cbBrush);

      // Copy the data from the source to the destination. The StretchBlt/ROP does all the magic.
      dcDestination.StretchBlt(
         crDestination.left,
         crDestination.top,
         crDestination.Width(),
         crDestination.Height(),
         &dcMask,
         crMask.left,
         crMask.top,
         crMask.Width(),
         crMask.Height(),
         dwRop);

      dcDestination.RestoreDC(-1);
   }
}

void Util::DrawColorThroughMask(CDC& dcDestination, const CRect& crDestination, CDC& dcMask, const CRect& crMask, COLORREF clColor, BOOL fInvertMask /*=FALSE*/)
{
   // Create a brush with the specified color and call DrawBrushThroughMask().
   CBrush cbColor;
   if (cbColor.CreateSolidBrush(clColor))
   {
      DrawBrushThroughMask(dcDestination, crDestination, dcMask, crMask, cbColor, fInvertMask);
   }
}

void Util::CreateMask(CDC& dcMask, CBitmap& bmMask, CRect& crMask, CDC& dcSource, const CRect& crSource, COLORREF clColor, BOOL fInitializeMask /*=TRUE*/)
{
   if (fInitializeMask)
   {
      // Delete any current objects.
      dcMask.DeleteDC();
      bmMask.DeleteObject();

      // Create a memory DC for the mask bitmap.
      dcMask.CreateCompatibleDC(&dcSource);

      // Create a rectangle describing the mask dimensions.
      crMask.SetRect(0, 0, crSource.Width(), crSource.Height());

      // Create a new monochrome bitmap.
      bmMask.CreateBitmap(crMask.Width(), crMask.Height(), 1, 1, NULL);
   }

   // Make sure we have a DC and a bitmap.
   if ((dcMask.m_hDC != NULL) && (bmMask.m_hObject != NULL))
   {
      CBitmap* pOldBitmap;
      if ((pOldBitmap = dcMask.SelectObject(&bmMask)) != NULL)
      {
         // Set the background color to the color we want for the mask. When the
         // color bitmap is copied to the monochrome bitmap, the pixels that match
         // the specified color will be set to 1 and the other pixels will be
         // set to 0.
         COLORREF clOld = dcSource.SetBkColor(clColor);

         // Copy the data. The monochrome mask will be created.
         dcMask.BitBlt(
            crMask.left,
            crMask.top,
            crMask.Width(),
            crMask.Height(),
            &dcSource,
            crSource.left,
            crSource.top,
            fInitializeMask ? SRCCOPY : SRCPAINT);

         // Restore the previous DC states.
         dcSource.SetBkColor(clOld);
         dcMask.SelectObject(pOldBitmap);
      }
   }
}

void Util::Emboss(CDC& dc, const CRect& crRect, COLORREF clBackground, COLORREF clHighlight, COLORREF clEmbossShadow, COLORREF clEmbossHighlight)
{
   CDC dcMask;
   CBitmap bmMask;
   CRect crMask;

   // Create a DC and monochrome bitmap for the mask. The mask will have 1's for pixels
   // matching clBackground and clHighlight, and 0's everywhere else.
   CreateMask(dcMask, bmMask, crMask, dc, crRect, clBackground, TRUE);
   CreateMask(dcMask, bmMask, crMask, dc, crRect, clHighlight, FALSE);

   // Erase the background of the embossed area.
   FillRectangleWithColor(dc, crRect, clBackground);

   CBitmap* pOldBitmap;
   if ((pOldBitmap = dcMask.SelectObject(&bmMask)) != NULL)
   {
      // Draw the embossed highlight.
      CRect crHighlight(crRect);
      crHighlight.OffsetRect(1, 1);
      DrawColorThroughMask(dc, crHighlight, dcMask, crMask, clEmbossHighlight, FALSE);
   
      // Draw the embossed shadow.
      DrawColorThroughMask(dc, crRect, dcMask, crMask, clEmbossShadow, FALSE);
      
      dcMask.SelectObject(pOldBitmap);
   }
}

void Util::TransparentBlt(CDC& dcDestination, const CRect& crDestination, CDC& dcSource, const CRect& crSource, COLORREF clTransparent)
{
   // Create a copy of the source bitmap.
   CDC dcBitmap;
   if (dcBitmap.CreateCompatibleDC(&dcSource))
   {
      CBitmap bmBitmap;
      CRect crBitmap(0, 0, crSource.Width(), crSource.Height());
      if (bmBitmap.CreateCompatibleBitmap(&dcSource, crBitmap.Width(), crBitmap.Height()))
      {
         CBitmap* pOldBitmap;
         if ((pOldBitmap = dcBitmap.SelectObject(&bmBitmap)) != NULL)
         {
            dcBitmap.BitBlt(
               crBitmap.left, crBitmap.top, crBitmap.Width(), crBitmap.Height(),
               &dcSource, crSource.left, crSource.top,
               SRCCOPY);
            
            // Create a mask.
            CDC dcMask;
            CBitmap bmMask;
            CRect crMask;
            Util::CreateMask(dcMask, bmMask, crMask, dcSource, crSource, clTransparent, TRUE);
            
            CBitmap* pOldMask;
            if ((pOldMask = dcMask.SelectObject(&bmMask)) != NULL)
            {
               // Set all the transparent pixels in the bitmap to 0's (black).
               COLORREF clOldBitmapTextColor = dcBitmap.SetTextColor(RGB(255,255,255));   // 0's in mask (opaque) go to 1's
               COLORREF clOldBitmapBkColor = dcBitmap.SetBkColor(RGB(0,0,0));             // 1's in mask (transparent) go to 0's
               dcBitmap.BitBlt(
                  crBitmap.left, crBitmap.top, crBitmap.Width(), crBitmap.Height(),
                  &dcMask, crMask.left, crMask.top,
                  SRCAND);
               dcBitmap.SetBkColor(clOldBitmapBkColor);
               dcBitmap.SetTextColor(clOldBitmapTextColor);
               
               // Erase to 0's (black) the areas of the desintation where the opaque portions
               // of the bitmap will be drawn.
               int nOldDestinationStretchBltMode = dcDestination.SetStretchBltMode(STRETCH_DELETESCANS);
               COLORREF clOldDestinationTextColor = dcDestination.SetTextColor(RGB(0,0,0));
               COLORREF clOldDestinationBkColor = dcDestination.SetBkColor(RGB(255,255,255));
               dcDestination.StretchBlt(
                  crDestination.left, crDestination.top, crDestination.Width(), crDestination.Height(),
                  &dcMask, crMask.left, crMask.top, crMask.Width(), crMask.Height(),
                  SRCAND);
               dcDestination.SetBkColor(clOldDestinationBkColor);
               dcDestination.SetTextColor(clOldDestinationTextColor);
                  
               // Finally, "or" the bitmap data onto the destination bitmap.
               dcDestination.StretchBlt(
                  crDestination.left, crDestination.top, crDestination.Width(), crDestination.Height(),
                  &dcBitmap, crBitmap.left, crBitmap.top, crBitmap.Width(), crBitmap.Height(),
                  SRCPAINT);
               dcDestination.SetStretchBltMode(nOldDestinationStretchBltMode);
               
               dcMask.SelectObject(pOldMask);
               pOldMask = NULL;
            }
            
            dcBitmap.SelectObject(pOldBitmap);
            pOldBitmap = NULL;
         }
      }
   }
}

void Util::TransparentBltBitmap(CDC& dcDestination, const CRect& crDestination, CBitmap& bmBitmap, LPCRECT pSourceRect /*=NULL*/, CPalette* pPalette /*=NULL*/, UINT uTransparentX /*=0*/, UINT uTransparentY /*=(UINT)-1*/)
{
   // Create source DC.
   CDC dcBitmap;
   if (dcBitmap.CreateCompatibleDC(&dcDestination))
   {
      // Select the palette into the source bitmap.
      CPalette* pOldPalette = NULL;
      if (pPalette != NULL)
      {
         pOldPalette = dcBitmap.SelectPalette(pPalette, TRUE);
      }

      // Select the bitmap into the DC.
      CBitmap* pOldBitmap;
      if ((pOldBitmap = dcBitmap.SelectObject(&bmBitmap)) != NULL)
      {
         // Get the dimensions of the bitmap.
         BITMAP BitmapInfo;
         if (GetBitmap(&bmBitmap, &BitmapInfo))
         {
            CRect crBitmap(0, 0, BitmapInfo.bmWidth, BitmapInfo.bmHeight);
            if (!crBitmap.IsRectEmpty())
            {
               if (uTransparentX >= (UINT)(crBitmap.Width()))
               {
                  uTransparentX = crBitmap.Width()-1;
               }
               if (uTransparentY >= (UINT)(crBitmap.Height()))
               {
                  uTransparentY = crBitmap.Height()-1;
               }
               
               // Get the transparent color.
               COLORREF clTransparent = dcBitmap.GetPixel(uTransparentX, uTransparentY);

               // If the caller has specified a source rectangle, use it.
               if (pSourceRect != NULL)
               {
                  // Use the entire bitmap.
                  crBitmap = *pSourceRect;
               }
                  
               // Do the blit.
               TransparentBlt(dcDestination, crDestination, dcBitmap, crBitmap, clTransparent);
            }
         }
         
         dcBitmap.SelectObject(pOldBitmap);
         pOldBitmap = NULL;
      }

      if (pOldPalette != NULL)
      {
         dcBitmap.SelectPalette(pOldPalette, TRUE);
         pOldPalette = FALSE;
      }
   }
}

BOOL Util::LoadResourceBitmap(
CBitmap &   Bitmap,
LPCSTR      pszResource,
CPalette *  pPalette,
HINSTANCE   hInst,      /*=AfxGetResourceHandle()*/
LPCSTR      pszType     /*=RT_BITMAP*/)
{
	// Load a bitmap from the resource file and map it to our palette.
   BOOL fResult = FALSE;

	// Free any existing bitmap.
	Bitmap.DeleteObject();

   HRSRC hBitmapResource;
   if ((hBitmapResource = FindResource(hInst, pszResource, pszType)) != NULL)
   {
      HGLOBAL hBitmapData;
      if ((hBitmapData = LoadResource(hInst, hBitmapResource)) != NULL)
      {
         LPBITMAPINFOHEADER pBitmapData;
         if ((pBitmapData = (LPBITMAPINFOHEADER)LockResource(hBitmapData)) != NULL)
         {
				// Compute the number of colors in the color table.
				int nColors = 0;
				if (pBitmapData->biBitCount < 16)
				{
					if (pBitmapData->biClrUsed != 0)
					{
						nColors = pBitmapData->biClrUsed;
					}
					else
					{
						nColors = 1 << pBitmapData->biBitCount;
					}
				}
            HDC hDC;
            if ((hDC = ::GetDC(NULL)) != NULL)
            {
               CDC dc;
               if (dc.Attach(hDC))
               {
                  CPalette* pOldPal;
                  if ((pOldPal = dc.SelectPalette(pPalette, FALSE)) != NULL)
                  {
                     dc.RealizePalette();
                     HBITMAP hBitmap = CreateDIBitmap(
                        dc.GetSafeHdc(),
                        pBitmapData,
                        CBM_INIT,
                        ((LPBYTE)pBitmapData)+pBitmapData->biSize+(nColors*sizeof(RGBQUAD)),
                        (LPBITMAPINFO)pBitmapData,
                        DIB_RGB_COLORS);
                     if (hBitmap != NULL)
                     {
                        Bitmap.Attach(hBitmap);
                        fResult = TRUE;
                     }
                     dc.SelectPalette(pOldPal, FALSE);
                     pOldPal = NULL;
                     dc.RealizePalette();
                  }
                  dc.Detach();
               }
               ::ReleaseDC(NULL, hDC);
               hDC = NULL;
            }
            UnlockResource(hBitmapData);
            pBitmapData = NULL;
         }
         FreeResource(hBitmapData);
         hBitmapData = NULL;
      }
   }

   return fResult;
}

void Util::DrawClientBorder(CDC& dc, CRect& crRect)
{
#ifdef WIN32
	//   D---------------W
	//   |B------------BL|
	//   ||             ||
	//   ||             ||
	//   |B             ||
	//   |L-------------L|
	//   W---------------W

	CRect crSide;

	// D horizontal
	crSide.SetRect(crRect.left, crRect.top, crRect.right-1, crRect.top+1);
	Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNSHADOW));

	// D vertical
	crSide.SetRect(crRect.left, crRect.top+1, crRect.left+1, crRect.bottom-1);
	Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNSHADOW));

	// B horizontal
	crSide.SetRect(crRect.left+1, crRect.top+1, crRect.right-2, crRect.top+2);
	Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNTEXT));

	// B vertical
	crSide.SetRect(crRect.left+1, crRect.top+2, crRect.left+2, crRect.bottom-2);
	Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNTEXT));

	// W horizontal
	crSide.SetRect(crRect.left, crRect.bottom-1, crRect.right, crRect.bottom);
	Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNHILIGHT));

	// W vertical
	crSide.SetRect(crRect.right-1, crRect.top, crRect.right, crRect.bottom-1);
	Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNHILIGHT));

	// L horizontal
	crSide.SetRect(crRect.left+1, crRect.bottom-2, crRect.right-1, crRect.bottom-1);
	Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNFACE));

	// L vertical
	crSide.SetRect(crRect.right-2, crRect.top+1, crRect.right-1, crRect.bottom-2);
	Util::FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNFACE));

	crRect.InflateRect(-2, -2);
#else
	//   B---------------B
	//   |               |
	//   |               |
	//   |               |
	//   |               |
	//   |               |
	//   B---------------B

	// Draw each side.
	CRect crSide;

	// Top
	crSide.SetRect(crRect.left, crRect.top, crRect.right, crRect.top+1);
	FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNTEXT));

	// Bottom
	crSide.OffsetRect(0, crRect.Height()-1);
	FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNTEXT));

	// Left
	crSide.SetRect(crRect.left, crRect.top+1, crRect.left+1, crRect.bottom-1);
	FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNTEXT));

	// Right
	crSide.OffsetRect(crRect.Width()-1, 0);
	FillRectangleWithColor(dc, crSide, GetSysColor(COLOR_BTNTEXT));

	crRect.InflateRect(-1, -1);
#endif
}

/*
// Check if a message is a CHAR or KEYDOWN for a particular key.
*/

BOOL Util::IsKeyMessage(LPMSG pMsg, UINT uKey /*=0*/, ...)
{
   if (pMsg != NULL)
   {
      // Check for a key message.
      if ((pMsg->message == WM_CHAR) || (pMsg->message == WM_KEYDOWN))
      {
         // special case: if no arguments, return TRUE if WM_CHAR or WM_KEYDOWN.
         if (uKey == 0)
         {
            return TRUE;
         }

         // Have to search list.
         va_list KeyList;
         va_start(KeyList, uKey);
         while (uKey != 0)
         {
            // Check the current key,
            if (pMsg->wParam == uKey)
            {
               // Found a match.
               va_end(KeyList);
               return TRUE;
            }

            // Get the next key.
            uKey = va_arg(KeyList, UINT);
         }

         va_end(KeyList);
      }
   }

   // Not one of the keys we were looking for.
   return FALSE;
}

LRESULT Util::SendWmCommand(CWnd* pWindow, UINT uID, CWnd* pControl, WORD wNotifyCode)
{
#ifdef WIN32
   return pWindow->SendMessage(WM_COMMAND, MAKEWPARAM(uID, wNotifyCode), (LPARAM)(pControl->GetSafeHwnd()));
#else
   return pWindow->SendMessage(WM_COMMAND, (WPARAM)uID, MAKELPARAM(pControl->GetSafeHwnd(), wNotifyCode));
#endif
}

BOOL Util::PostWmCommand(CWnd* pWindow, UINT uID, CWnd* pControl, WORD wNotifyCode)
{
#ifdef WIN32
   return pWindow->PostMessage(WM_COMMAND, MAKEWPARAM(uID, wNotifyCode), (LPARAM)(pControl->GetSafeHwnd()));
#else
   return pWindow->PostMessage(WM_COMMAND, (WPARAM)uID, MAKELPARAM(pControl->GetSafeHwnd(), wNotifyCode));
#endif
}

int Util::GetMenuString(CMenu* pMenu, UINT nIDItem, CString& csString, UINT nFlags)
{
#ifdef WIN32
   return pMenu->GetMenuString(nIDItem, csString, nFlags);
#else
   int nResult = 0;
   TRY
   {
      nResult = pMenu->GetMenuString(nIDItem, csString.GetBuffer(256), 256-1, nFlags);
      csString.ReleaseBuffer();
   }
   END_TRY
   
   return nResult;
#endif
}

int Util::DrawText(CDC* pDC, const CString& csString, LPRECT pRect, UINT nFormat)
{
#ifdef WIN32
   return pDC->DrawText(csString, pRect, nFormat);
#else
   return pDC->DrawText((LPCSTR)csString, csString.GetLength(), pRect, nFormat);
#endif
}

CSize Util::GetTextExtent(CDC* pDC, const CString& csString)
{
#ifdef WIN32
   return pDC->GetTextExtent(csString);
#else
   return pDC->GetTextExtent((LPCSTR)csString, csString.GetLength());
#endif
}

int Util::GetBitmap(CBitmap* pBitmap, BITMAP* pBitmapInfo)
{
#ifdef WIN32
   return pBitmap->GetBitmap(pBitmapInfo);
#else
   return pBitmap->GetObject(sizeof(*pBitmapInfo), pBitmapInfo);
#endif
}

BOOL Util::ReadString(CStdioFile* pFile, CString& csString)
{
#ifdef WIN32
   return pFile->ReadString(csString);
#else
   static char chNil = '\0';
   csString = &chNil;    // empty string without deallocating
   const int nMaxSize = 128;
   LPTSTR lpsz = csString.GetBuffer(nMaxSize);
   LPTSTR lpszResult;
   int nLen;
   for (;;)
   {
      lpszResult = pFile->ReadString(lpsz, nMaxSize+1);
      csString.ReleaseBuffer();

      // if string is read completely or EOF
      if (lpszResult == NULL ||
         (nLen = lstrlen(lpsz)) < nMaxSize ||
         lpsz[nLen-1] == '\n')
      {
         break;
      }

      nLen = csString.GetLength();
      lpsz = csString.GetBuffer(nMaxSize + nLen) + nLen;
   }

   // remove '\n' from end of string if present
   lpsz = csString.GetBuffer(0);
   nLen = csString.GetLength();
   if (nLen != 0 && lpsz[nLen-1] == '\n')
      csString.GetBufferSetLength(nLen-1);

   return lpszResult != NULL;
#endif
}
   
int Util::GetDlgItemText(CWnd* pWnd, int nID, CString& csString)
{
#ifdef WIN32
   return pWnd->GetDlgItemText(nID, csString);
#else
   int nResult = 0;
   TRY
   {
      nResult = pWnd->GetDlgItemText(nID, csString.GetBuffer(256), 256-1);
      csString.ReleaseBuffer();
   }
   END_TRY
   
   return nResult;
#endif
}

HWND Util::ChildWindowFromPointEx(HWND hWnd, POINT pt)
{
#ifdef WIN32
   return ::ChildWindowFromPointEx(hWnd, pt, CWP_SKIPINVISIBLE);
#else
   CWnd* pWnd = CWnd::FromHandle(hWnd);
   pWnd->ClientToScreen(&pt);

   CWnd* pChild;
   for (pChild = pWnd->GetWindow(GW_CHILD); pChild != NULL; pChild = pChild->GetWindow(GW_HWNDNEXT))
   {
      CRect crWindow;
      pChild->GetWindowRect(&crWindow);
      if (crWindow.PtInRect(pt) && pChild->IsWindowVisible())
      {
         break;
      }
   }

   return pChild->GetSafeHwnd();
#endif
}

BOOL Util::DoWinExec(LPCSTR pszCommandLine, UINT nShow /*=SW_SHOW*/, BOOL fModalProcess /*=FALSE*/)
{
   BOOL fSuccess;
#ifdef WIN32
/*
// 32-bit version.
*/

   PROCESS_INFORMATION ProcessInformation;

   STARTUPINFO StartupInfo;
   StartupInfo.cb = sizeof(StartupInfo);
   StartupInfo.lpReserved = NULL;
   StartupInfo.lpDesktop = NULL;
   StartupInfo.lpTitle = NULL;
   StartupInfo.dwX = 0;
   StartupInfo.dwY = 0;
   StartupInfo.dwXSize = 0;
   StartupInfo.dwYSize = 0;
   StartupInfo.dwXCountChars = 0;
   StartupInfo.dwYCountChars = 0;
   StartupInfo.dwFillAttribute = 0;
   StartupInfo.dwFlags = STARTF_FORCEOFFFEEDBACK|STARTF_USESHOWWINDOW;
   StartupInfo.wShowWindow = nShow;
   StartupInfo.cbReserved2 = 0;
   StartupInfo.lpReserved2 = NULL;
   StartupInfo.hStdInput = NULL;
   StartupInfo.hStdOutput = NULL;
   StartupInfo.hStdError = NULL;

   fSuccess = CreateProcess(
                            NULL,
                            (LPTSTR)pszCommandLine,      // use command line for 16 bit process in NT
                            NULL,
                            NULL,
                            FALSE,
                            NORMAL_PRIORITY_CLASS,
                            NULL,
                            NULL,
                            &StartupInfo,
                            &ProcessInformation);
   if (fSuccess)
   {
      if (fModalProcess)
      {
         // Wait until the process has finished its initialization (or 10 seconds, whichever is less).
         WaitForInputIdle(ProcessInformation.hProcess, 10*1000);

         AttachThreadInput(ProcessInformation.dwThreadId,
                           GetCurrentThreadId(),
                           TRUE);
      }
   }

#else
/*
// 16-bit version.
*/
   UINT nResult = WinExec(pszCommandLine, nShow);
   fSuccess = (nResult > 31);
#endif

   return fSuccess;
}

BOOL Util::WaitForWindow(UINT uTitle, DWORD dwMSecsToWait)
{
   BOOL fResult = FALSE;

   CString csTitle;
   TRY
   {
      csTitle.LoadString(uTitle);
      fResult = WaitForWindow(csTitle, dwMSecsToWait);
   }
   END_TRY

   return fResult;
}

BOOL Util::WaitForWindow(LPCSTR pszTitle, DWORD dwMSecsToWait)
{
   BOOL fResult = FALSE;

   // Wait for the window to come up.
   MSG Msg;
   DWORD dwStartTime = GetTickCount();
   while (GetTickCount() < dwStartTime + dwMSecsToWait)
   {
      if (::PeekMessage(&Msg, NULL, NULL, NULL, PM_NOREMOVE))
      {
         AfxGetApp()->PumpMessage();
      }
      if (FindWindowByTitle(pszTitle) != NULL)
      {
         // Found it!
         fResult = TRUE;
         break;
      }
   }

   // Now, wait for the window to go away.
   // If it never came up, then we will leave immediately.

   while (FindWindowByTitle(pszTitle) != NULL)
   {
      // Loop forever!
      if (::PeekMessage(&Msg, NULL, NULL, NULL, PM_NOREMOVE))
      {
         AfxGetApp()->PumpMessage();
      }
   }

   return fResult;
}

BOOL Util::WindowHasTitle(HWND hWnd, LPCSTR pszTitle, BOOL fPrefix /*=TRUE*/)
{
   BOOL fResult = FALSE;

   char cbText[128];
   ::GetWindowText(hWnd, cbText, sizeof(cbText)-1);

   if (fPrefix)
   {
      int nTitleLen = strlen(pszTitle);
      fResult = strncmp(pszTitle, cbText, nTitleLen) == 0;
   }
   else
   {
      fResult = strcmp(pszTitle, cbText) == 0;
   }

   return fResult;
}

HWND Util::FindWindowByTitle(LPCSTR pszTitle, BOOL fPrefix /*=TRUE*/)
{
   HWND hWndDesktop = ::GetDesktopWindow();
   HWND hWndChild;

   // For some reason, it misses the window sometimes under NT.
   // So, I try a couple of times.
   for (int i = 0; i < 2; i++)
   {
      // Look through all the children of the desktop.
      for (hWndChild = ::GetWindow(hWndDesktop, GW_CHILD);
           hWndChild != NULL;
           hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT))
      {
         if (IsWindowVisible(hWndChild) && WindowHasTitle(hWndChild, pszTitle, fPrefix))
         {
            return hWndChild;
         }
      }
   }
   return NULL;
}

void Util::YieldToWindows()
{
	MSG msg;

	while (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
	{
		if (!AfxGetApp()->PumpMessage())
      {
         PostQuitMessage(0);
			return;               // Bag out if WM_QUIT received
      }
	}
}

LPVOID Util::HugeAlloc(DWORD dwSize)
{
   LPVOID pResult = NULL;
#ifdef WIN32
   TRY
   {
      pResult = new BYTE[dwSize];
   }
   END_TRY
#else
   pResult = _halloc(dwSize, 1);
#endif
   return pResult;
}

void Util::HugeFree(LPVOID pData)
{
#ifdef WIN32
   delete [] (LPBYTE)pData;
#else
   if (pData != NULL)
   {
      _hfree(pData);
   }
#endif
}

void Util::GetOperatingSystem(BOOL& fWin3, BOOL& fWin95, BOOL& fWinNT, WORD& wVersion)
{
   fWin3 = FALSE;
   fWin95 = FALSE;
   fWinNT = FALSE;

   DWORD dwVersion = GetVersion();
   WORD wMajorVersion = (WORD)(LOBYTE(LOWORD(dwVersion)));
   WORD wMinorVersion = (WORD)(HIBYTE(LOWORD(dwVersion)));
   
   wVersion = MAKEWORD(wMajorVersion, wMinorVersion);

#ifdef WIN32
   BOOL fHighBit = (dwVersion & 0x80000000L) != 0;
   fWin3 = fHighBit && (wMajorVersion == 3);
   fWin95 = fHighBit && (wMajorVersion == 4);
   fWinNT = !fHighBit && ((wMajorVersion == 3) || (wMajorVersion == 4));
#else
   DWORD dwFlags = GetWinFlags();
   fWinNT = (dwFlags & 0x4000) != 0;
   fWin95 = !fWinNT && (wVersion >= 0x035f);
   fWin3 = !fWinNT && !fWin95 && (wMajorVersion == 3);
#endif
}

BOOL Util::IsWin3(void)
{
   BOOL fWin3, fWin95, fWinNT;
   WORD wVersion;
   GetOperatingSystem(fWin3, fWin95, fWinNT, wVersion);
   return fWin3;
}

BOOL Util::IsWin95(void)
{
   BOOL fWin3, fWin95, fWinNT;
   WORD wVersion;
   GetOperatingSystem(fWin3, fWin95, fWinNT, wVersion);
   return fWin95;
}

BOOL Util::IsWinNT(void)
{
   BOOL fWin3, fWin95, fWinNT;
   WORD wVersion;
   GetOperatingSystem(fWin3, fWin95, fWinNT, wVersion);
   return fWinNT;
}

BOOL Util::GetFileTime(LPCSTR pszFileName, LPFILETIME pCreationTime, LPFILETIME pLastAccessTime, LPFILETIME pLastWriteTime)
{
	BOOL fResult = FALSE;

	if (pszFileName != NULL)
	{
		HANDLE hFile = CreateFile(
			pszFileName,
			GENERIC_READ,
			FILE_SHARE_READ|FILE_SHARE_WRITE,//FILE_SHARE_DELETE is NT only
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			if (::GetFileTime(hFile, pCreationTime, pLastAccessTime, pLastWriteTime))
			{
				if (pCreationTime != NULL)
				{
					FILETIME t = *pCreationTime;
					FileTimeToLocalFileTime(&t, pCreationTime);
				}
				if (pLastAccessTime != NULL)
				{
					FILETIME t = *pLastAccessTime;
					FileTimeToLocalFileTime(&t, pLastAccessTime);
				}
				if (pLastWriteTime != NULL)
				{
					FILETIME t = *pLastWriteTime;
					FileTimeToLocalFileTime(&t, pLastWriteTime);
				}
				fResult = TRUE;
			}
			CloseHandle(hFile);
		}
	}

	return fResult;
}

BOOL Util::SetFileTime(LPCSTR pszFileName, LPFILETIME pCreationTime, LPFILETIME pLastAccessTime, LPFILETIME pLastWriteTime)
{
	BOOL fResult = FALSE;

	if (pszFileName != NULL)
	{
		HANDLE hFile = CreateFile(
			pszFileName,
			GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,//FILE_SHARE_DELETE is NT only
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
			if (::SetFileTime(hFile, pCreationTime, pLastAccessTime, pLastWriteTime))
			{
				fResult = TRUE;
			}
			CloseHandle(hFile);
		}
	}

	return fResult;
}

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

void Util::FillListFromDelimitedString(LPCSTR pszInput, LPCSTR seps, CStringList& cslStrings)
{
	CString csStrings = pszInput;
	CString csString;

	while (!csStrings.IsEmpty())
	{
		int nIndex = csStrings.FindOneOf(seps);
		if (nIndex == -1)
		{
			csString = csStrings;
			csStrings.Empty();
		}
		else
		{
			csString = csStrings.Left(nIndex);
			csStrings = csStrings.Mid(nIndex+1);
		}
		Util::Trim(csString);
		cslStrings.AddTail(csString);
	}
}

void Util::FillArrayFromDelimitedString(LPCSTR pszInput, LPCSTR seps, CStringArray& csaStrings)
{
	CString csStrings = pszInput;
	CString csString;

	while (!csStrings.IsEmpty())
	{
		int nIndex = csStrings.FindOneOf(seps);
		if (nIndex == -1)
		{
			csString = csStrings;
			csStrings.Empty();
		}
		else
		{
			csString = csStrings.Left(nIndex);
			csStrings = csStrings.Mid(nIndex+1);
		}
		Util::Trim(csString);
		csaStrings.Add(csString);
	}
}

void Util::GetProgramFilesDir(CString& csDir)
{
	long lSize = sizeof(_MAX_PATH);
	LPSTR pBuffer = csDir.GetBuffer(lSize);

	// Look in the registry to see what the "program files" directory is.
	int nResult = ::RegQueryValue(HKEY_LOCAL_MACHINE,
							  "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\ProgramFilesDir",
							  pBuffer,
							  &lSize);
	csDir.ReleaseBuffer();

	if (nResult != ERROR_SUCCESS)
	{
		// Default to the C drive.
		// We could default to the same drive as Windows, but I do not know
		// how this impacts network installations.
		CString csDrive;
#if 0
		CString csWindowsDir;
		GetWindowsDirectory(csWindowsDir);
		SplitPath(csWindowsDir, &csDrive, NULL, NULL, NULL);
#else
		csDrive = "c:";
#endif
		// Build the path.
		ConstructPath(csDir, csDrive, "\\", "Program Files", "");
	}
}

//
// Convert a file name into an "internet" file name.
//

void Util::FileNameToURLForm(CString& csName)
{
	// Turn all backslashes into slashes.
	int nLength = csName.GetLength();
	for (int i = 0; i < nLength; i++)
	{
		char c = csName.GetAt(i);
		if (c == '\\')
		{
			csName.SetAt(i, '/');
		}
	}
}

//
// Convert a content ID into what the server wants it to be.
//

void Util::NormalizeContentServerID(CString& csID)
{
	// Strip off the extension if it is part of the file name.
	int nSlash = csID.ReverseFind('/');
	int nBackslash = csID.ReverseFind('\\');
	int nExtension = csID.ReverseFind('.');
	if ((nExtension > nSlash) && (nExtension > nBackslash))
	{
		csID = csID.Left(nExtension);
	}

	// Turn all slashes into 'X'
	int nLength = csID.GetLength();
	for (int i = 0; i < nLength; i++)
	{
		char c = csID.GetAt(i);
		if ((c == '\\') || (c == '/'))
		{
			csID.SetAt(i, 'X');
		}
	}
}
////
void Util::RemoveExtraMenuSeparators(CMenu *pMenu)
{
	ASSERT(pMenu != NULL);
	if (pMenu != NULL)
	{
		BOOL	LastItemWasSeparator = FALSE;
		for (UINT i = 0; i < pMenu->GetMenuItemCount(); i ++)
		{
			if (pMenu->GetMenuItemID(i) == 0)
			{
				if (LastItemWasSeparator == TRUE)
				{
					pMenu->DeleteMenu(i, MF_BYPOSITION);
					i --;
				}
				else
					LastItemWasSeparator = TRUE;
			}	
			else
				LastItemWasSeparator = FALSE;
		}
	}
}

// return the HTREEITEM in treeCtrl whose parent is hTreeCtrlParent and that matches cString, else NULL
//
HTREEITEM Util::FindTreeCtrlStringExact(
	const CTreeCtrl& treeCtrl, HTREEITEM hTreeCtrlParent, const char* pszString )
{
	for (HTREEITEM hTreeItem = treeCtrl.GetChildItem( hTreeCtrlParent ); hTreeItem;
		hTreeItem = treeCtrl.GetNextSiblingItem( hTreeItem ) )
	{
		const int kMaxBuf = 200;
		char itemText[kMaxBuf];
		TVITEM tvItem;
		tvItem.mask = TVIF_TEXT;
		tvItem.pszText = itemText;
		tvItem.cchTextMax = kMaxBuf;
		tvItem.hItem = hTreeItem;
		if (treeCtrl.GetItem( &tvItem ))
			if (::strcmp(itemText, pszString) == 0)
				return hTreeItem;
	}
	return NULL;
}

// recursively sort treeCtrl, starting from hParent
//
void Util::SortTreeCtrl( CTreeCtrl& treeCtrl, HTREEITEM hParent /* = NULL */ )
{
	if (hParent == NULL)
		hParent = treeCtrl.GetRootItem();
	treeCtrl.SortChildren( hParent );
	for (HTREEITEM hChild = treeCtrl.GetChildItem( hParent ); hChild;
		hChild = treeCtrl.GetNextSiblingItem( hChild ) )
	{
		SortTreeCtrl( treeCtrl, hChild );
	}
}

// add '|' delimited strings to csCategory that represent the full category "path"
// of hTreeItem in treeCtrl, for instance "Backgrounds|Floral"
//
void Util::AddCategorySelectionStrings( const CTreeCtrl& treeCtrl, HTREEITEM hTreeItem, CString& csCategories )
{
	// recursively add category selection string with '|' delimiters from parent levels
	HTREEITEM hParent = treeCtrl.GetParentItem( hTreeItem );
	if (hParent == NULL)
		return;
	else
		AddCategorySelectionStrings( treeCtrl, hParent, csCategories );

	// add category string from current level
	if (!csCategories.IsEmpty())
		csCategories += '|';
	csCategories += treeCtrl.GetItemText( hTreeItem );
}

// set selected item in treeCtrl from '|' delimited full category "path" string
//
void Util::SetTreeCtrlSelection( CTreeCtrl& treeCtrl, HTREEITEM hParent, const CString& csCategories )
{
	int index = csCategories.Find( '|' );
	if (index < 0)
		index = csCategories.GetLength();
	CString thisCategory = csCategories.Left( index );
	CString remainingCategories = csCategories.Right( csCategories.GetLength() - index - 1 );

	for (; hParent; hParent = treeCtrl.GetNextSiblingItem( hParent ) )
	{
		HTREEITEM hChild = FindTreeCtrlStringExact( treeCtrl, hParent, thisCategory );
		if (hChild)
		{
			treeCtrl.Expand( hParent, TVE_EXPAND );
			treeCtrl.EnsureVisible( hChild );
			treeCtrl.SelectItem( hChild );
			SetTreeCtrlSelection( treeCtrl, hChild, remainingCategories );
			return;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// CBoolSemaphore

CBoolSemaphore::CBoolSemaphore(BOOL &rBool) : m_rBool(rBool)
{
   m_fLocked = FALSE;
}

CBoolSemaphore::~CBoolSemaphore(void)
{
   Unlock();
}

BOOL CBoolSemaphore::Lock(void)
{
   if (m_rBool)
   {
   /* Somebody else has it. */
      return FALSE;
   }
   m_rBool = m_fLocked = TRUE;
   return TRUE;
}

void CBoolSemaphore::Unlock(void)
{
   if (m_fLocked)
   {
      m_rBool = m_fLocked = FALSE;
   }
}

/////////////////////////////////////////////////////////////////////////////
// CFileIterator

CFileIterator::CFileIterator()
{
#ifdef WIN32
   m_hSearch = NULL;
#endif
}

CFileIterator::~CFileIterator()
{
   Close();
}

void CFileIterator::Close(void)
{
#ifdef WIN32
   if (m_hSearch != NULL)
   {
      FindClose(m_hSearch);
      m_hSearch = NULL;
   }
#endif
}

BOOL CFileIterator::FindFirst(LPCSTR pName, DWORD dwDOSAttributes)
{
#ifdef WIN32
   Close();
   if (m_hSearch != NULL)
   {
   /* m_hSearch is already not NULL! Can't begin search. */
      ASSERT(FALSE);
      return FALSE;
   }

   /*
   // Construct the attribute bits to search for.
   */

   m_dwExclude = (FILE_ATTRIBUTE_DIRECTORY
                        | FILE_ATTRIBUTE_SYSTEM
                        | FILE_ATTRIBUTE_HIDDEN);

   if (dwDOSAttributes & _A_SUBDIR)
   {
      m_dwExclude &= ~FILE_ATTRIBUTE_DIRECTORY;
   }

   if (dwDOSAttributes & _A_HIDDEN)
   {
      m_dwExclude &= ~FILE_ATTRIBUTE_HIDDEN;
   }

   if (dwDOSAttributes & _A_SYSTEM)
   {
      m_dwExclude &= ~FILE_ATTRIBUTE_SYSTEM;
   }

   if (dwDOSAttributes & _A_RDONLY)
   {
      m_dwExclude &= ~FILE_ATTRIBUTE_READONLY;
   }

   if ((m_hSearch = FindFirstFile(pName, &m_ffd)) == INVALID_HANDLE_VALUE)
   {
      m_hSearch = NULL;
      return FALSE;
   }

   /*
   // If this file matches our attributes, then use it.
   */

   if ((m_ffd.dwFileAttributes & m_dwExclude) == 0)
   {
      return TRUE;
   }

   /* The first file doesn't match; keep looking. */

   return FindNext();
#else
   return _dos_findfirst(pName, (WORD)dwDOSAttributes, &m_buffer) == 0;
#endif
}

BOOL CFileIterator::FindNext(void)
{
#ifdef WIN32
   if (m_hSearch == NULL)
   {
   /* m_hSearch is NULL. What are we doing here? */
      ASSERT(FALSE);
      return FALSE;
   }

/* Look for the next file that matches our attributes. */

   while (FindNextFile(m_hSearch, &m_ffd))
   {
      if ((m_ffd.dwFileAttributes & m_dwExclude) == 0)
      {
         return TRUE;
      }
   }
   return FALSE;
#else
   return _dos_findnext(&m_buffer) == 0;
#endif
}

LPCSTR CFileIterator::Name(void)
{
#ifdef WIN32
   return m_ffd.cFileName;
#else
   return m_buffer.name;
#endif
}

DWORD CFileIterator::Attributes(void)
{
#ifdef WIN32
   DWORD dwAttributes = _A_NORMAL;
   if (m_ffd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
   {
      dwAttributes |= _A_RDONLY;
   }
   if (m_ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
   {
      dwAttributes |= _A_SUBDIR;
   }
   if (m_ffd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
   {
      dwAttributes |= _A_HIDDEN;
   }
   if (m_ffd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
   {
      dwAttributes |= _A_SYSTEM;
   }
   if (m_ffd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
   {
      dwAttributes |= _A_ARCH;
   }
   return dwAttributes;
#else
   return (DWORD)(unsigned char)m_buffer.attrib;
#endif
}

// CComboFileNames
CComboFileNames::CComboFileNames(DWORD bt)
{
   mdwBadTribs = bt;
}

CString
CComboFileNames::GetFriendlyName (LPCSTR istr)
{
   CString cstr;
   Util::SplitPath(istr, NULL, NULL, &cstr, NULL);

   return cstr;
}

int
CComboFileNames::AddFoundFileName(WIN32_FIND_DATA *pData)
{
   if (pData != NULL)
   {
      if ((pData->dwFileAttributes & mdwBadTribs) == 0)       // Acceptable file?
      {
         CString  FileName = pData->cFileName;
         int idx = AddString (GetFriendlyName (FileName));  // Add friendly name to listbox
         if ((idx != LB_ERR) && (idx != LB_ERRSPACE))
         {
            int x = m_fileNames.GetSize();    
            m_fileNames.Add(FileName);                      // Add real name to array
            SetItemData(idx, x);                            // Tie array index to friendly name
         }
         return idx;
      }
   }

   return LB_ERR;
}

BOOL
CComboFileNames::Fill(LPCSTR FileSpec)
{
   ResetContent();
   WIN32_FIND_DATA   Dater;
   HANDLE            Handle;
   if ((Handle = FindFirstFile (FileSpec, &Dater)) != INVALID_HANDLE_VALUE)
   {
      AddFoundFileName (&Dater);
      SetCurSel (0);
      while (FindNextFile (Handle, &Dater) != FALSE)
      {
         AddFoundFileName (&Dater);
      }

      FindClose (Handle);
   }
   // Did we find any?
   if (GetCount() < 1)
   {
      return FALSE;
   }
   else return TRUE;
}
// Global function usable even by those not including util.h
#pragma function(memset)
void HMemSet(LPVOID pDst, int nValue, long lCount)
{
#ifdef WIN32
   memset(pDst, nValue, lCount);
#else
              
   BYTE huge* hpDest = (BYTE huge*)pDst;

   while (TRUE)
   {
      UINT         ucount;           // Temporary short int

      if (HIWORD(lCount))
         ucount = 65535;       // >64K left; set ucount to 64K
      else
         ucount = LOWORD(lCount);

      memset(hpDest, nValue, ucount);

      if ((lCount -= ucount) == 0)        // Reduce the amount left to move
      {
         break;
      }

      // Advance the pointers.
      hpDest += ucount;
   }
#endif
}
// memset is now "function" (not intrinsic) from here on...

