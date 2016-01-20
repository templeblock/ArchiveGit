/*
// $Workfile: INSTALL.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
// Copyright © 1998 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/INSTALL.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 37    10/30/98 8:54a Dennis
// Explorer is now attempted to be installed if install app. exists.
// Working directory now set to install directory instead of drive root.
// 
// 36    10/01/98 2:59p Jayn
// Changes for the standalone Art & More CD.
// 
// 35    9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 34    9/01/98 1:38p Dennis
// Changed Internet URL group item name of American Greetings
// 
// 33    9/01/98 11:27a Mwilson
// fixed upgrade to print premium
// 
// 32    8/05/98 10:56a Mwilson
// added compression option to installer
// 
// 31    7/26/98 2:02p Jayn
// Minor string/flow changes
// 
// 30    7/24/98 7:43p Psasse
// Added some lines of text to the progress dialog when DrawPlus and
// ArtStore are installed
// 
// 29    7/21/98 10:22a Cboggio
// Removed ISP install and Serif draw install for certain (international)
// configurations
// 
// 28    7/20/98 11:59a Jayn
// Skips "can't create file" error message if file type is "IgnoreIfInUse"
// 
// 27    7/18/98 12:08p Jayn
// 
// 26    7/09/98 5:06p Dennis
// Added  CPrintStandardConfiguration::UnsupportedDialog stub
// 
// 25    7/08/98 10:45a Dennis
// Changed some strings to from "PrintMaster" to use @@N thru
// configuration
// 
// 24    7/07/98 5:42p Dennis
// external uninstal.ico is no longer registered with program group
// 
// 23    7/07/98 11:06a Rlovejoy
// Added DeleteOldContent() to remove v4.0 content files.
// 
// 22    7/07/98 10:58a Mwilson
// changed to read url icon from ini file.  Defaults to pmwurl.ico
// 
// 21    7/06/98 4:19p Mwilson
// removed check for AG product.  It was no longer needed since I added
// support for different exe names.
// 
// 20    7/06/98 2:03p Mwilson
// changed to read ini file for exe and icon file name.  Defaults to
// pmw.exe and pmg.ico
// 
// 19    6/25/98 4:13p Jayn
// Got rid of a memory corruption error when copying a file failed.
// 
// 18    6/24/98 4:38p Dennis
// American Greetings specific changes
// 
// 17    6/24/98 8:26a Jayn
// 
// 16    6/10/98 4:45p Jayn
// AT&T install
// 
// 15    6/06/98 3:20p Fredf
// 
// 14    5/04/98 3:53p Jayn
// Changes for Deluxe.
// 
// 13    4/23/98 3:30p Dennis
// Install now uses "install.pfl" instead of pmw32.pfl.
// Install message now more generic by sourcing it from config object.
// 
// 12    4/17/98 11:56a Rlovejoy
// Need arguments for uninstall names.
// 
// 11    4/16/98 2:36p Dennis
// Added #ifdef AG_BUILD blocks for American Greetings specific changes
// 
// 10    4/15/98 5:17p Rlovejoy
// Updated to put uninstall program in working directory.
// 
// 9     4/13/98 10:33a Jayn
// Changes for DrawPlus installation.
// 
// 8     4/08/98 12:34p Fredf
// New explorer progress indicator.
// 
// 7     4/03/98 9:38a Jayn
// INSTALL.CPP?
// 
// 6     4/03/98 9:34a Jayn
// Fixed my last changes.
// 
// 5     3/27/98 1:09p Jayn
// Changed  MsgWaitForMultipleObjectsEx to  MsgWaitForMultipleObjects.
// 
// 4     3/26/98 10:29a Fredf
// Changes to install program to install Internet Explorer 4.0 and the Web
// Publishing Wizard.
// 
// 3     2/11/98 11:15a Dennis
// Removed GetRegistryString() and now use GetRegistryString() located in
// util.*
// 
// 2     1/21/98 4:50p Fredf
// Year 2000 compliance.
// 
// 2     1/19/98 1:20p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
//    Rev 1.0   14 Aug 1997 15:21:42   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:50   Fred
// Initial revision.
// 
//    Rev 1.40   06 Aug 1997 08:41:40   Jay
// Launch Guide for 32 and 16
// 
//    Rev 1.39   30 Jul 1997 12:20:18   Fred
// Mindscape Catalog
// 
//    Rev 1.38   28 Jul 1997 15:27:32   Fred
// Fixed installation bug where event reminder was not being added on upgrade
// 
//    Rev 1.37   18 Jul 1997 13:48:10   Jay
// Can't install to windows or system directory
// 
//    Rev 1.36   08 Jul 1997 12:42:30   Fred
// Moved strings to resource file
// 
//    Rev 1.35   20 Jun 1997 16:47:38   Fred
//  
// 
//    Rev 1.34   17 Jun 1997 11:05:48   Fred
// Fixed code that deals with destination errors.
// 
//    Rev 1.33   13 Jun 1997 13:53:40   Fred
// Deletes output file if it could not be copied
// 
//    Rev 1.32   13 Jun 1997 08:44:04   Jay
// Now writes workspace size to PMW.INI
// 
//    Rev 1.31   09 Jun 1997 08:47:00   Fred
//  
// 
//    Rev 1.30   02 Jun 1997 15:11:22   Fred
// Removed message box
// 
//    Rev 1.29   02 Jun 1997 10:44:38   Fred
// Changes for 4.0 install
// 
//    Rev 1.28   29 May 1997 15:09:34   Fred
//  
// 
//    Rev 1.27   21 May 1997 15:01:46   Fred
//  
// 
//    Rev 1.26   19 May 1997 13:56:14   Fred
// Event Reminder, etc.
// 
//    Rev 1.25   05 May 1997 08:54:16   Jay
// Now frees the resource DLL when done.
// 
//    Rev 1.24   25 Apr 1997 14:22:58   Fred
// Removed introduction
// 
//    Rev 1.23   25 Apr 1997 13:49:22   Jay
// 
//    Rev 1.22   21 Apr 1997 16:01:48   Jay
// 4.0 changes
// 
//    Rev 1.21   21 Mar 1997 11:10:36   Jay
// Print preview changes
// 
//    Rev 1.20   12 Dec 1996 13:16:40   Fred
// Quick hacks to install 4.0
// 
//    Rev 1.19   01 Nov 1996 09:28:00   Jay
// Moved over from 3.01.17
// 
//    Rev 1.20   29 Oct 1996 16:35:14   Fred
// Twain Tweaks
// 
//    Rev 1.19   29 Oct 1996 15:25:52   Fred
// Twain install
// 
//    Rev 1.18   01 Oct 1996 16:37:44   dennisp
// Almost forgot to comment out #define SPECIAL.
// 
//    Rev 1.17   01 Oct 1996 16:09:16   DennisP
// Install changes for Twain support.   Search for DGP to locate changes.
// 
//    Rev 1.16   11 Sep 1996 13:01:48   Fred
// Sets starting directory to root directory of PM volume
// 
//    Rev 1.15   08 Sep 1996 12:51:10   Fred
//  
// 
//    Rev 1.14   07 Sep 1996 17:14:56   Fred
//  
// 
//    Rev 1.13   05 Sep 1996 22:15:38   Fred
// New intro directory
// 
//    Rev 1.12   30 Aug 1996 20:05:18   Fred
// Does not tre to install notes icon
// 
//    Rev 1.11   30 Aug 1996 19:33:42   Fred
// Works with UNC and long file names
// 
//    Rev 1.10   30 Aug 1996 13:18:10   Fred
//  
// 
//    Rev 1.9   27 Aug 1996 11:42:16   Fred
// Uses GetTickCount() instead of CTime for PMINST.DAT
// 
//    Rev 1.8   22 Aug 1996 15:59:10   Fred
// Introduction
// 
//    Rev 1.7   20 Aug 1996 18:21:02   Fred
// AT&T Installation
// 
//    Rev 1.6   16 Aug 1996 12:36:38   Fred
// STARTF_USESHOWWINDOW
// 
//    Rev 1.5   18 Jul 1996 17:34:12   Fred
// Some improvements for PMW 3.0
// 
//    Rev 1.4   18 Jul 1996 14:48:22   Fred
// PMW 3 changes
// 
//    Rev 1.3   15 May 1996 08:46:00   Jay
// From PMW2
// 
//    Rev 2.53   13 May 1996 15:36:48   FRED
//  
// 
//    Rev 2.52   29 Apr 1996 13:21:18   FRED
// Checks for upgrading
// 
//    Rev 2.51   29 Apr 1996 10:18:02   JAY
// Now adds 1 for the zero terminator in RegSetValueEx calls of type REG_SZ.
// 
//    Rev 2.50   29 Apr 1996 09:55:04   JAY
// Fixed sound coding error.
// 
//    Rev 2.49   26 Apr 1996 18:09:38   FRED
// More demo stuff
// 
//    Rev 2.48   26 Apr 1996 13:08:46   JAY
// More demo code
// 
//    Rev 2.47   25 Apr 1996 14:47:00   JAY
// Now handles sound state correctly.
// 
//    Rev 2.46   25 Apr 1996 10:06:52   FRED
//  
// 
//    Rev 2.45   24 Apr 1996 16:43:36   JAY
// New upgradable demo support.
// 
//    Rev 2.44   19 Apr 1996 15:49:54   FRED
// Removes old group
// 
//    Rev 2.43   19 Apr 1996 10:05:02   JAY
// Now supports previous installation directory.
// 
//    Rev 2.42   19 Apr 1996 08:53:16   JAY
// New old/new version/level logic for autorun installations.
// 
//    Rev 2.41   03 Apr 1996 14:24:58   JAY
// Commented out "special" code.
// 
//    Rev 2.40   03 Apr 1996 12:50:26   JAY
// Now touches a directory before going past it.
// 
//    Rev 2.39   21 Mar 1996 11:15:50   FRED
// Properly computes time stamp.
// 
// 
//    Rev 2.38   07 Mar 1996 16:32:22   JAY
// Added User Guide icon
// 
//    Rev 2.37   12 Feb 1996 14:17:24   JAY
// Unregisters use if previous installation was of lesser level.
// 
//    Rev 2.36   12 Feb 1996 10:36:36   JAY
// Upgrade Icons
// 
//    Rev 2.35   12 Feb 1996 09:28:34   FRED
// Adds proper exec string for workstation uninstall
// 
//    Rev 2.34   12 Feb 1996 08:39:56   JAY
// Added param to pmOpen.
// 
//    Rev 2.33   08 Feb 1996 09:23:26   JAY
// Demo changes
// 
//    Rev 2.32   07 Feb 1996 18:01:46   JAY
// Only does the FileOS check for our program files.
// 
//    Rev 2.31   07 Feb 1996 09:51:14   JAY
// Now makes sure file OS (16 or 32 bit) of files are same before comparing
// version numbers (only relevant if the same).
// 
//    Rev 2.30   07 Feb 1996 09:29:54   JAY
// MSREGUSR.INI goes in Windows directory, System directory.
// 
//    Rev 2.29   05 Feb 1996 14:09:12   FRED
// Dynamically displays program title is about box bitmap
// 
//    Rev 2.28   05 Feb 1996 08:06:06   JAY
//  
// 
//    Rev 2.27   02 Feb 1996 13:09:24   JAY
//  
// 
//    Rev 2.26   31 Jan 1996 10:29:46   FRED
// New workstation install
// 
//    Rev 2.25   29 Jan 1996 16:53:20   JAY
// Variable configuration in INSTALL and PMW.
// 
//    Rev 2.24   26 Jan 1996 15:09:36   JAY
//  
// 
//    Rev 2.23   26 Jan 1996 08:27:56   FRED
// Added Autoplay support to installer.
// 
//    Rev 2.22   23 Jan 1996 12:30:44   JAY
// 
//    Rev 2.21   22 Jan 1996 08:50:22   JAY
// More demo changes
// 
//    Rev 2.20   19 Jan 1996 09:37:44   JAY
// Changes for the demo version.
// 
//    Rev 2.19   15 Jan 1996 14:46:38   JAY
// Some changes to speed up non-versionable files.
// 
//    Rev 2.18   15 Jan 1996 10:27:26   JAY
//  
// 
//    Rev 2.17   05 Dec 1995 16:47:32   FRED
//  
// 
//    Rev 2.16   05 Dec 1995 09:41:08   JAY
// Added sound command lines arguments.
// 
//    Rev 2.15   04 Dec 1995 17:08:48   JAY
// Now checks to see if any OLE (group) files are in use and won't install any
// unless all are not in use.
// 
//    Rev 2.14   20 Nov 1995 09:52:22   JAY
// Now zaps all VFL files in the PICTURES directory before installing.
// 
//    Rev 2.13   15 Nov 1995 14:40:16   FRED
//  
// 
//    Rev 2.12   18 Oct 1995 16:48:14   JAY
// 
//    Rev 2.11   17 Oct 1995 22:02:06   FRED
//  
// 
//    Rev 2.10   17 Oct 1995 15:42:28   JAY
// Fixes
// 
//    Rev 2.9   17 Oct 1995 14:13:54   FRED
//  
// 
//    Rev 2.8   16 Oct 1995 15:05:50   FRED
// More 32 bit changes
// 
//    Rev 2.7   13 Oct 1995 14:20:00   FRED
// 32 bit changes
// 
//    Rev 2.6   27 Aug 1995 18:30:54   FRED
// Changes to make install program not reference OLE
// 
//    Rev 2.5   09 Aug 1995 17:32:16   FRED
// Does not copy files if the versions are the same.
// 
//    Rev 2.4   09 Aug 1995 08:35:12   FRED
// Hookup with INSTICFG.DLL
// 
//    Rev 2.3   08 Aug 1995 09:00:58   FRED
// Publishing Suite Installation
// 
//    Rev 2.2   07 Aug 1995 17:14:54   FRED
// Publishing Suite Install
// 
//    Rev 2.1   08 Feb 1995 13:37:28   JAY
// Reverted. New series.
// 
//    Rev 1.22   23 Jan 1995 13:06:48   JAY
// Added CD Catalog.
// 
//    Rev 1.21   19 Jul 1994 09:03:50   JAY
// Sound directory is now shared in the root.
// 
//    Rev 1.20   18 Jul 1994 16:07:54   FRED
// Added "Audio Guide" dialog.
// 
// 
//    Rev 1.19   18 Jul 1994 12:14:52   JAY
// Now closes the sound manager.
// 
//    Rev 1.18   15 Jul 1994 16:18:44   FRED
// 
//    Rev 1.17   15 Jul 1994 14:10:38   JAY
// Moved the sounds for the installation program.
// 
//    Rev 1.16   15 Jul 1994 14:05:36   FRED
// No longer blabs when help is requested.
// 
// 
//    Rev 1.15   15 Jul 1994 10:39:58   JAY
// Support for monochrome sound buttons.
// 
//    Rev 1.14   24 Jun 1994 14:38:12   FRED
// VerFindFile() is only called for shared files.
// This is a work-around for a version of VER.DLL
// that returns the wrong directory for non-shared
// files.
// 
// 
//    Rev 1.13   24 Jun 1994 14:15:56   JAY
// Now constructs and passes a path to the sound manager.
// 
//    Rev 1.12   23 Jun 1994 13:28:02   FRED
// Main application is started after CTL3D is shut down.
// If not, Windows reports lost bitmaps and brushes.
// 
// 
//    Rev 1.11   22 Jun 1994 15:38:22   FRED
// Remoced some trace messages.
// 
// 
//    Rev 1.10   22 Jun 1994 14:50:36   FRED
// Added Exception trace message.
// 
// 
//    Rev 1.9   22 Jun 1994 13:44:12   FRED
// Fixed problem with CheckForPreviousInstance() trying
// to load undefined string resource. The program title
// is now passed in.
// 
// InstallProgram() now disables the main (gradient) window
// when the progress dialog is showing.
// 
//    Rev 1.8   22 Jun 1994 11:07:22   FRED
// Added code to removed warning on call to VerFindFile().
// 
// 
//    Rev 1.7   22 Jun 1994 09:05:02   FRED
// Does not allow F1 Help from about menu.
// This can change if help file is copied
// off CD to hard disk.
// 
// 
//    Rev 1.6   22 Jun 1994 08:55:14   FRED
// Uses CControl3d.
// Better error trapping.
// 
// 
//    Rev 1.5   21 Jun 1994 15:38:56   FRED
// Calls Util::MessageBox() and AfxMessageBox()
// with -1 help id's (to be filled in later if needed.
// 
// 
//    Rev 1.4   21 Jun 1994 14:42:06   FRED
// Does version checking.
// 
// 
//    Rev 1.3   17 Jun 1994 17:03:26   FRED
// Writes installation data.
// Other goodies.
// 
// 
//    Rev 1.2   16 Jun 1994 18:13:10   FRED
// Version that copies files.
// 
// 
//    Rev 1.1   15 Jun 1994 18:58:28   FRED
// Working interface, does not yet copy files.
// 
// 
//    Rev 1.0   15 Jun 1994 17:13:20   FRED
// Initial revision.
*/ 

#include "stdafx.h"

#include "misc.h"
#include "error.h"
#include "install.h"
#include "iabout.h"
#include "iaguide.h"
#include "idefault.h"
#include "idiskspc.h"
#include "iprogres.h"
#include "icongrat.h"
#include "iispdlg.h"
#include "chdirdlg.h"
#include "spinbutt.h"
#include "cprogbar.h"
#include "cpfl.h"
#include "util.h"
#include "ioptions.h"
#include "pmwcfg.h"
#include "globatom.h"
#include "iexplore.h"
#include "rfor.h"
#include <winver.h>
#include <regstr.h>
#include <io.h>
#include <direct.h>
#include <errno.h>
#include "cdeflate.h"

#ifdef _DEBUG
//#define SPECIAL
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern BOOL ddeInit(HINSTANCE hInst, HINSTANCE hPrevInst);
extern HWND pmOpen(LPCSTR szName, BOOL fDelete);
extern BOOL pmClose(LPCSTR szName);
extern BOOL pmDeleteGroup(LPCSTR lpszName);
extern BOOL pmReplaceItem(LPCSTR lpszName);
extern BOOL pmAddItem(LPCSTR szName, LPCSTR szCmd, LPCSTR szIconName, WORD wIconNumber, int nXPos = -1, int nYPos = -1, LPCSTR pszDefaultDirectory = NULL);
extern BOOL pmDeleteItem(LPCSTR lpszName);
extern BOOL pmGetGroupData(CString& csResult, LPCSTR pszGroupName = NULL);
extern void ddeShutdown(void);

/*
// Registry configuration keys.
*/

char far SECTION_Registry[] = "Registry";
char far ENTRY_Name[] = "Name";
char far ENTRY_Version[] = "Version";
char far ENTRY_Company[] = "Company";

char far SECTION_Configuration[] = "Configuration";
//char far ENTRY_Version[] = "Version";

/*
// MSREGUSR.INI keys.
*/

const char BASED_CODE ENTRY_AppPath[] = "AppPath";

DWORD IncrementSharedDllReferenceCount(LPCSTR pszName);

/*
// The system heap.
*/

BOOL g_fProcessorIs386Compatible = TRUE;

CStringArray csaSharedFiles;

void GetResourceString(CString& csString, LPCSTR pszString)
{
   csString.Empty();
   if (pszString != NULL)
   {
      if (HIWORD(pszString) == 0)
      {
         LoadConfigurationString((UINT)LOWORD(pszString), csString);
      }
      else
      {
         csString = pszString;
      }
   }
}

CString& GetShortName(const CString& csLongName, CString& csShortName)
{
   GetShortPathName(csLongName, csShortName.GetBuffer(_MAX_PATH+1), _MAX_PATH);
   csShortName.ReleaseBuffer();
   return csShortName;
}

void AddGroupItem(LPCSTR pszName, LPCSTR pszDirectory, LPCSTR pszFile, LPCSTR pszArguments, LPCSTR pszIconDirectory, LPCSTR pszIconFile, WORD wIconIndex)
{
   CString csName;
   CString csDirectory;
   CString csFile;
   CString csArguments;
   CString csIconDirectory;
   CString csIconFile;
   CString csPath;
   CString csCommand;
   CString csIconPath;
   CString csIcon;
   CString csDefaultDirectory;
   TRY
   {
      // Get the strings.
      GetResourceString(csName, pszName);
      GetResourceString(csDirectory, pszDirectory);
      GetResourceString(csFile, pszFile);
      GetResourceString(csArguments, pszArguments);
      GetResourceString(csIconDirectory, pszIconDirectory);
      GetResourceString(csIconFile, pszIconFile);

      Util::ConstructPath(csPath, csDirectory, csFile);

      GetShortName(csPath, csCommand);

	  csDefaultDirectory = csDirectory;

      if (!csArguments.IsEmpty())
      {
         csCommand += ' ';
         csCommand += csArguments;
      }

      if (!csIconFile.IsEmpty())
      {
         if (!csIconDirectory.IsEmpty())
         {
            Util::ConstructPath(csIconPath, csIconDirectory, csIconFile);
         }
         else
         {
            csIconPath = csIconFile;
         }
      }

      if (csIconPath.IsEmpty())
      {
         csIconPath = csPath;
      }

      if (!csIconPath.IsEmpty())
      {
         GetShortName(csIconPath, csIcon);
      }

      if (Util::FileExists(csIcon))
      {
         if (!pmAddItem(csName, csCommand, csIcon, wIconIndex, -1, -1, csDefaultDirectory))
         {
            Util::MessageBox(
               MB_OK,
               (UINT)-1,
               MAKEINTRESOURCE(IDS_ErrCantAddToProgramManagerGroup),
               (LPCSTR)csName,
               MAKEINTRESOURCE(IDS_ProgramManagerGroupName));
            AfxThrowUserException();
         }
      }
   }
   CATCH_ALL(e)
   {
      csName.Empty();
      csDirectory.Empty();
      csFile.Empty();
      csArguments.Empty();
      csIconDirectory.Empty();
      csIconFile.Empty();
      csPath.Empty();
      csCommand.Empty();
      csIconPath.Empty();

      THROW_LAST();
   }
   END_CATCH_ALL
}

/////////////////////////////////////////////////////////////////////////////
// CInstallMainFrame
		
BEGIN_MESSAGE_MAP(CInstallMainFrame, CGradientWindow)
	//{{AFX_MSG_MAP(CInstallMainFrame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInstallMainFrame message handlers


/////////////////////////////////////////////////////////////////////////////
// CInstallApp

BEGIN_MESSAGE_MAP(CInstallApp, CWinApp)
   //{{AFX_MSG_MAP(CInstallApp)
      // NOTE - the ClassWizard will add and remove mapping macros here.
      //    DO NOT EDIT what you see in these blocks of generated code!
   //}}AFX_MSG_MAP
   // Standard file based document commands
   // Global help commands
   ON_COMMAND(ID_HELP_INDEX, CWinApp::OnHelpIndex)
   ON_COMMAND(ID_HELP_USING, CWinApp::OnHelpUsing)
   ON_COMMAND(ID_HELP, CWinApp::OnHelp)
   ON_COMMAND(ID_CONTEXT_HELP, CWinApp::OnContextHelp)
   ON_COMMAND(ID_DEFAULT_HELP, CWinApp::OnHelpIndex)
	ON_WM_QUERYENDSESSION()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInstallApp construction

CInstallApp::CInstallApp()
{
   m_csInstallDirectory.Empty();
   m_dwWorkspaceSize = 0;
   m_fTrapHelpMessages = FALSE;
   m_fInstallIsp = FALSE;
   m_fCallRegEdit = FALSE;
   m_fOleInUse = FALSE;
   m_fWorkstationInstall = FALSE;
   m_fTwainInstall = FALSE;
   m_fForceSound8 = FALSE;
   m_fForceSoundOff = FALSE;
	m_fMustReboot = FALSE;
	m_InstDLL = NULL;

   memset(&m_PreviousInfo, 0, sizeof(m_PreviousInfo));
   memset(&m_InstallInfo, 0, sizeof(m_InstallInfo));
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CInstallApp object

CInstallApp NEAR theApp;

BOOL CInstallApp::OnQueryEndSession()
{
	// We no longer need to force a reboot.
	m_fMustReboot = FALSE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CInstallApp initialization

static BOOL SystemIsMonochrome(void)
{
   HDC hDC = ::GetDC(NULL);
   if (hDC != NULL)
   {
      BOOL fIsMonochrome =  (::GetDeviceCaps(hDC, BITSPIXEL)*::GetDeviceCaps(hDC, PLANES)) <= 1;
      ::ReleaseDC(NULL, hDC);
      return fIsMonochrome;
   }
/* Assume not. */
   return FALSE;
}

BOOL DoWinExec(LPCSTR pszCommandLine, LPCSTR pszWaitMessage = NULL, BOOL fWait = FALSE, CInstallProgressDialog* pProgressDialog = NULL, UINT nShow = SW_SHOWNORMAL)
{
   BOOL fSuccess;

	if (pszWaitMessage != NULL)
	{
		fWait = TRUE;
	}

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
		if (fWait)
		{
			// Disable our main window while we're waiting
         CWnd* pMainFrm = AfxGetMainWnd();
         pMainFrm->EnableWindow(FALSE);

			if (pszWaitMessage != NULL)
			{
				// Wait until we receive the specified message.
				UINT msgFinished = RegisterWindowMessage(pszWaitMessage);
				if (msgFinished != 0)
				{
					// Pump messages until we get finished message.
					for (;;)
					{
						MSG Msg;
						if (GetMessage(&Msg, NULL, 0, 0))
						{
							if (Msg.message == msgFinished)
							{
								break;
							}

							if (Msg.hwnd != NULL)
							{
								TranslateMessage(&Msg);
								DispatchMessage(&Msg);
							}
						}
						else
						{
							// GetMessage() returned FALSE.
							// That means a WM_QUIT is pending.
							// Just break out. We don't care whether it finished or not.
							// To make sure somebody gets this, though, we post it again.
							PostQuitMessage(0);
							break;
						}
					}
				}
			}
			else
			{
				// Wait for the process to finish.
				BOOL fContinue = TRUE;
				while (fContinue)
				{
					DWORD dwResult = MsgWaitForMultipleObjects(
												1,											// nCount
												&ProcessInformation.hProcess,		// pHandles
												FALSE,										// bWaitAll
												200,										// dwMilliseconds
												QS_ALLINPUT);							// dwWakeMask


					switch (dwResult)
					{
						case WAIT_OBJECT_0:
						{
							// The process has finished!
							fContinue = FALSE;
							break;
						}
						case WAIT_TIMEOUT:
						{
#if 0
							if (pProgressDialog != NULL)
							{
								DWORD dwMin = pProgressDialog->GetMin();
								DWORD dwMax = pProgressDialog->GetMax();
								DWORD dwPos = pProgressDialog->GetPos();

								dwPos++;
								if (dwPos > dwMax)
								{
									dwPos = dwMin;
								}

								pProgressDialog->SetMinMaxPos(dwMin, dwMax, dwPos);
							}
#endif
							// Fall through to...
						}
						case WAIT_OBJECT_0+1:
						{
							// Input is waiting, pump messages.
							MSG Msg;
							while (::PeekMessage(&Msg, NULL, 0, 0, PM_NOREMOVE))
							{
								if (!AfxGetApp()->PumpMessage())
								{
									::PostQuitMessage(0);
									fContinue = FALSE;
									break;
								}
							}

							break;
						}
						default:
						{
							// Unexpected return value.
							TRACE("MsgWaitForMultipleObjectsEx returned %08lx\n", dwResult);
							ASSERT(FALSE);
							break;
						}
					}
				}
			}

			// Re-enable the main window.
			pMainFrm->EnableWindow(TRUE);

			// Make sure we are on top and active.
			pMainFrm->SetForegroundWindow();
			pMainFrm->BringWindowToTop();
			pMainFrm->UpdateWindow();
      }
   }

   return fSuccess;
}

static BOOL GetNextLine(CString& csLines, CString& csLine)
{
   BOOL fResult = FALSE;
   TRY
   {
      csLine.Empty();

      // Find end of current line.
      int nIndex = csLines.Find('\n');

      if (nIndex == -1)
      {
         // Handle last line without carriage return.
         if (!csLines.IsEmpty())
         {
            csLine = csLines;
            csLines.Empty();
            fResult = TRUE;
         }
      }
      else
      {
         // Got a carriage return.
         fResult = TRUE;

         // Strip of trailing /n and /r
         int nLastChar = nIndex;
         while ((nLastChar >= 0)
              && ((csLines[nLastChar] == '\n') || (csLines[nLastChar] == '\r')))
         {
            nLastChar--;
         }
         csLine = csLines.Left(nLastChar+1);
         csLines = csLines.Mid(nIndex+1);
      }
   }
   END_TRY

   return fResult;
}

static BOOL GetNextField(CString& csFields, CString& csField, char cDelimiter)
{
   BOOL fResult = FALSE;
   TRY
   {
      csField.Empty();

      // Find end of current field.
      int nIndex = csFields.Find(cDelimiter);

      if (nIndex == -1)
      {
         // Handle last field without delimiter.
         if (!csFields.IsEmpty())
         {
            csField = csFields;
            csFields.Empty();
            fResult = TRUE;
         }
      }
      else
      {
         // Got a field.
         fResult = TRUE;
         csField = csFields.Left(nIndex);
         csFields = csFields.Mid(nIndex+1);

         // Strip off leading an trailing quotes.
         if (!csField.IsEmpty() && (csField[0] == '"'))
         {
            csField = csField.Mid(1);
         }
         if (!csField.IsEmpty() && (csField[csField.GetLength()-1] == '"'))
         {
            csField = csField.Left(csField.GetLength()-1);
         }
      }
   }
   END_TRY

   return fResult;
}

static BOOL GetField(const CString& csFields, CString& csField, char cDelimiter, int nField)
{
   CString csTemp;
   BOOL fResult = FALSE;
   TRY
   {
      csField.Empty();
      csTemp = csFields;

      fResult = TRUE;
      for (int i = 0; i <= nField; i++)
      {
         if (!GetNextField(csTemp, csField, cDelimiter))
         {
            fResult = FALSE;
            break;
         }
      }
   }
   END_TRY

   return fResult;
}

BOOL CInstallApp::DetermineConfiguration(void)
{
/*
// Determine the installation configuration.
*/

   CString csPath;

   TRY
   {
      for (int nConfiguration = CPrintMasterConfiguration::FirstConfiguration;
           nConfiguration < CPrintMasterConfiguration::LastConfiguration;
           nConfiguration++)
      {
			// Check the normal (run) signature for this configuration.

			// Build the path to the file.
         Util::ConstructPath(csPath,
                             GetSourceDirectory(),
                             CPrintMasterConfiguration::GetSignatureFile(nConfiguration));
			// See if the file exists.
         if (Util::FileExists(csPath))
         {
            m_InstallInfo.m_nInstallConfiguration = nConfiguration;
            break;
         }

			// Check the Install signature file for this configuration.
			// This is used when the Install and Run CDs are separate (as opposed)
			// to the combined case.

			// Build the path to the file.
         Util::ConstructPath(csPath,
                             GetSourceDirectory(),
                             CPrintMasterConfiguration::GetInstallSignatureFile(nConfiguration));

			// See if the file exists.
         if (Util::FileExists(csPath))
         {
            m_InstallInfo.m_nInstallConfiguration = nConfiguration;
            break;
         }
      }
   }
   END_TRY

   if (m_InstallInfo.m_nInstallConfiguration == 0)
   {
      return FALSE;
   }

/*
// Get the version from the MSREG.INI file.
*/

   GetVersion(&m_InstallInfo, m_MsregIniFile);

/*
// Determine the currently installed configuration.
*/

   CString csDirectory;

   TRY
   {
   /*
   // Get the path to the PMINST.DAT file.
   */
   /*
   // We get the app path from the registry.
   */
      Util::GetRegistryString(HKEY_LOCAL_MACHINE,
                        GetAppPathKey(),
                        NULL,
                        csPath);
      if (!csPath.IsEmpty())
      {
      /* Extract just the path part. */
         Util::SplitPath(csPath, &csDirectory, NULL);
      }

      if (!csDirectory.IsEmpty())
      {
         Util::ConstructPath(csPath, csDirectory, "PMINST.DAT");

         InstallationInfo Info;
         if (Info.Get(csPath, FALSE))
         {
         /*
         // Extract the previous configuration information.
         */

            m_PreviousInfo.m_nInstallConfiguration = Info.nInstallConfiguration;
            m_PreviousInfo.m_nCurrentConfiguration = Info.nCurrentConfiguration;

         /*
         // Only use the "current" configuration if it was installed
         // from the same level CD.
         */

            if (m_InstallInfo.m_nInstallConfiguration == m_PreviousInfo.m_nInstallConfiguration)
            {
               m_InstallInfo.m_nCurrentConfiguration = m_PreviousInfo.m_nCurrentConfiguration;
            }

         /*
         // We have successfully gotten information from a previous
         // installation. Remember this directory.
         */

            m_csPreviousInstallDirectory = csDirectory;
            Util::RemoveBackslashFromPath(m_csPreviousInstallDirectory);
         }

         if (m_PreviousInfo.m_nInstallConfiguration != 0)
         {
         // We have a previous configuration.
            Util::ConstructPath(csPath, csDirectory, "MSREG.INI");
            GetVersion(&m_PreviousInfo, csPath);
         }
      }
   }
   END_TRY

/*
// If we don't have a current configuration, then use the installation one.
*/

   if (m_InstallInfo.m_nCurrentConfiguration == 0)
   {
      m_InstallInfo.m_nCurrentConfiguration = m_InstallInfo.m_nInstallConfiguration;
   }

   return TRUE;
}

/*
// Get the version and stick it in the product info passed.
*/

void CInstallApp::GetVersion(CProductInfo* pInfo, CIniFile& IniFile)
{
   CString csValue;

   TRY
   {
      csValue = IniFile.GetString(SECTION_Configuration,
                                  ENTRY_Version);
      char cbBuffer[30];
      strcpy(cbBuffer, csValue);

      LPCSTR pVersionMajor;
      pVersionMajor = strtok(cbBuffer, ".");

      if (pVersionMajor != NULL)
      {
         LPCSTR pVersionMinor;
         pVersionMinor = strtok(NULL, ".");

         if (pVersionMinor != NULL)
         {
            LPCSTR pRevision;
            pRevision = strtok(NULL, ".");

            if (pRevision != NULL)
            {
               // Set the values.
               pInfo->m_nVersionMajor = atoi(pVersionMajor);
               pInfo->m_nVersionMinor = atoi(pVersionMinor);
               pInfo->m_nRevision = atoi(pRevision);
            }
         }
      }
   }
   END_TRY
}

/*
// Get the version from the passed INI file (passed by name).
*/

void CInstallApp::GetVersion(CProductInfo* pInfo, LPCSTR pSource)
{
   CIniFile IniFile(pSource);
   GetVersion(pInfo, IniFile);
}

BOOL CInstallApp::InitInstance()
{
   m_fMonochrome = SystemIsMonochrome();

   BOOL fSuccess = FALSE;

   CInstallMainFrame* pMainWindow = NULL;

   CString csApplicationTitle;
   CString csSoundManagerPath;
   CString csSound16Directory;
   CString csTemp;               // General purpose - no long-term value.
   CString csFormat;
   CString csUpgradeDirectory;
   CGlobalAtom Atom;

   TRY
   {
#ifdef INSTALL_FROM_HARD_DISK
		// See if we are being launched standalone.
		// If so, then we need to copy ourselves to the hard drive.
		BOOL fNeedsCopy = (GetParameter("/I", csTemp.GetBuffer(_MAX_PATH)) == NULL);
      csTemp.ReleaseBuffer();

		char cbParentProcess[32];

		if (fNeedsCopy)
		{
			LPCSTR pszSourceDirectory = GetSourceDirectory();

			// Find the temp dir
			char temp_dir[_MAX_PATH];
			GetTempPath(_MAX_PATH, temp_dir);
			DWORD dwTime = ::GetTickCount();
			CString csClone;
			csClone.Format("%s~%04lxTMP.EXE", (LPCSTR)temp_dir, dwTime & 0x0FFFF);

			ASSERT(!m_csProgramName.IsEmpty());
			ASSERT(!m_csSourceDirectory.IsEmpty());
			// Copy the install app to temp
			if (!::CopyFile((LPCTSTR)m_csProgramName, (LPCTSTR)csClone, FALSE))
				return FALSE;

			// Delete the clone upon reboot
			ReplaceFileOnReboot((LPCTSTR)csClone, NULL);

			// Launch the clone and return
			HANDLE hProcessOrig = ::OpenProcess(SYNCHRONIZE, TRUE, GetCurrentProcessId());

			PROCESS_INFORMATION ProcessInformation;

			STARTUPINFO StartupInfo;
			ZeroMemory(&StartupInfo, sizeof(StartupInfo));
			StartupInfo.cb = sizeof(StartupInfo);

			// Construct the command line.
			// Note the quotes around the source directory in case of file names
			// with spaces.

			CString csCmdLine;
			csCmdLine.Format("%s /H%d /I\"%s\"",
									(LPCSTR)csClone,
									(int)hProcessOrig,
									(LPCSTR)m_csSourceDirectory);

			if (!CreateProcess(
								 NULL,
								 csCmdLine.GetBuffer(256),
								 NULL,
								 NULL,
								 TRUE,
								 NORMAL_PRIORITY_CLASS,
								 NULL,
								 NULL,
								 &StartupInfo,
								 &ProcessInformation))
			{
				LPVOID lpMsgBuf;
				FormatMessage(     
					FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS,    
					NULL,
					GetLastError(),
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					(LPTSTR) &lpMsgBuf,    0,    NULL );		// Process any inserts in lpMsgBuf.
					// ...
					// Display the string.
					AfxMessageBox((LPCTSTR)lpMsgBuf);
					// Free the buffer.
					LocalFree( lpMsgBuf );
			}
			::CloseHandle(hProcessOrig);

			// And do not proceed.
			return FALSE;
		}
		else
		{
			// Remember the source directory.
			m_csSourceDirectory = csTemp;

			// Get the process id of the parent, and wait for it to close.
			GetParameter("/H", cbParentProcess);
			HANDLE hProcessOrig = (HANDLE)atoi(cbParentProcess);

			// Wait for it to finish...
			::MsgWaitForMultipleObjects(
								1,											// nCount
								&hProcessOrig,							// pHandles
								FALSE,									// bWaitAll
								INFINITE,								// dwMilliseconds
								0);										// dwWakeMask
			::CloseHandle(hProcessOrig);
		}
#else
		// Make sure we have a current directory.
		GetSourceDirectory();
#endif

      /*
      // Get the source directory for the install. We assume that INSTALL.EXE
      // is being run from the directory that is the root of the product
      // directory tree as shipped by MLS.
      */

      Util::ConstructPath(csTemp, GetSourceDirectory(), "PMW32\\PMWRES32.DLL");
      m_InstDLL = LoadLibrary(csTemp);
      if (m_InstDLL == NULL)
      {
         DWORD dwError = GetLastError();

         CString csMessage;
			CString csFormat;
			TRY
			{
				csFormat.LoadString(IDS_CantFindResourceDll);
				csMessage.Format(csFormat, (LPCSTR)csTemp, dwError);
			}
			END_TRY
         AfxMessageBox(csMessage);
         m_InstDLL = NULL;
         return FALSE;
      }

      m_ResourceString.SetInstanceHandle(m_InstDLL);

      /*
      // Setup the MSREG INI file.
      */

      Util::ConstructPath(csTemp, GetSourceDirectory(), "MSREG\\MSREG.INI");
      m_MsregIniFile.Name(csTemp);

      /*
      // Get the application name.
      // If we ever include MSREGINI.H, then change this to be right.
      */

      m_csApplicationName = m_MsregIniFile.GetString("Configuration", "Application");

      /*
      // Determine the installation product type.
      // Then, create our product configuration.
      */

      if (m_csApplicationName.IsEmpty()
       || !DetermineConfiguration()
       || !CPrintMasterConfiguration::Create(m_InstallInfo.m_nCurrentConfiguration))
      {
			CString csTitle;
         CPrintMasterConfiguration::SafeLoadString(IDS_PrintMasterGold, csTitle);
         CString csMessage;
         CPrintMasterConfiguration::SafeLoadString(IDS_NoConfiguration, csMessage);
         ::MessageBox(NULL, csMessage, csTitle, MB_OK | MB_ICONEXCLAMATION);
         return FALSE;
      }

      LoadConfigurationString(IDS_ApplicationTitle, csApplicationTitle);

      /* don't run more than one instance of this program */
      if (CheckForPreviousInstance(csApplicationTitle))
      {
         return FALSE;
      }

      // Register a class for ourselves.
      CString csClassName = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW, LoadCursor(IDC_ARROW), 0, LoadIcon(IDI_INSTALL));
   
      if (m_fMonochrome)
      {
         SetDialogBkColor(RGB(255,255,255));       // white background
      }
      else
      {
         SetDialogBkColor();        // set dialog background color to gray
      }

      /*
      // Register classes that are created via WNDCLASS.
      */

      CSpinButton::RegisterMyClass();
      CProgressBar::RegisterMyClass();

      /*
      // If we're installing a DEMO over a non-DEMO version of PrintMaster
      // which has the same or later version as the demo, then tell the user
      // they CANNOT do it. There is no reason for an end-user to do so since
      // they are already have a running PrintMaster that is better than
      // whatever is on the demo. Installing the demo will likely break the
      // registry entries which point to the retail version.
      */

      if (!FindParameter("/U"))
      {
         if ((m_InstallInfo.m_nInstallConfiguration == CPrintMasterConfiguration::Demo)
          && ((m_PreviousInfo.m_nCurrentConfiguration != 0)
           && (m_PreviousInfo.m_nCurrentConfiguration != CPrintMasterConfiguration::Demo))
          && ((m_PreviousInfo.m_nVersionMajor > m_InstallInfo.m_nVersionMajor)
           || ((m_PreviousInfo.m_nVersionMajor == m_InstallInfo.m_nVersionMajor)
            && (m_PreviousInfo.m_nVersionMinor >= m_InstallInfo.m_nVersionMinor))))
         {
            // Attempting to install demo over later PrintMaster.
            LoadConfigurationString(IDS_CantInstallDemo, csFormat);
            Util::MessageBox(
                  MB_OK|MB_DEFBUTTON1,
                  (UINT)-1,
                  (LPCSTR)csFormat,
                  (LPCSTR)m_csPreviousInstallDirectory);
            return FALSE;
         }
      }

		// Set up the autorun atom.
		csTemp = m_MsregIniFile.GetString("Configuration", "SkipAutoRunAtom");
		if (!csTemp.IsEmpty())
		{
			Atom.Name(csTemp);
		}

      /*
      // If we were launched by autorun (/A was passed), then check if
      // some component (PMW, MSREG, etc.) which uses the CD is
      // active. If so, then we DO NOT run.
      */

      BOOL fAutoRun = FindParameter("/A");

      if (fAutoRun)
      {
         // Autorun launched.
         // See if we should suppress in autorun mode.

			if (!Atom.Name().IsEmpty() && Atom.Exists())
			{
				return FALSE;
			}

         /*
         // See if the user is attempting to go backwards in time.
         // If the source CD is a demo, don't do this check.
         */

         if (m_InstallInfo.m_nInstallConfiguration != CPrintMasterConfiguration::Demo)
         {
            if (m_PreviousInfo > m_InstallInfo)
            {
					CString csMessage;
					CPrintMasterConfiguration::SafeLoadString(IDS_OlderVersion, csMessage);
               AfxMessageBox(csMessage);
               return FALSE;
            }
         }
      }

		Atom.Create();		// And if it fails, oh well...

      /*
      // Check if this is a network workstation install.
      */

      m_fWorkstationInstall = GetParameter("/N", m_csInstallDirectory.GetBuffer(_MAX_PATH)) != NULL;
      m_csInstallDirectory.ReleaseBuffer();

      if (!m_fWorkstationInstall)
      {
         /*
         // Check if this is a TWAIN install.
         */

         m_fTwainInstall = GetParameter("/T", m_csInstallDirectory.GetBuffer(_MAX_PATH)) != NULL;
         m_csInstallDirectory.ReleaseBuffer();
      }

      /*
      // Get the registry keys needed for installation, etc.
      */

      m_csRegistryName = m_MsregIniFile.GetString(SECTION_Registry, ENTRY_Name);
      m_csRegistryVersion = m_MsregIniFile.GetString(SECTION_Registry, ENTRY_Version);
      m_csRegistryCompany = m_MsregIniFile.GetString(SECTION_Registry, ENTRY_Company);

      if (m_csRegistryName.IsEmpty()
       || m_csRegistryVersion.IsEmpty()
       || m_csRegistryCompany.IsEmpty())
      {
         GetConfiguration()->MessageBox(IDS_ErrBadRegistryInfo, 0, MB_OK);
         AfxThrowUserException();
      }

      /*
      // Set up for doing DDE communications with PROGMAN for creating the program manager group.
      */

      if (!ddeInit(AfxGetInstanceHandle(), m_hPrevInstance))
      {
         GetConfiguration()->MessageBox(IDS_ErrCantInitializeDde, 0, MB_OK);
         AfxThrowUserException();
      }

      /*
      // Handle upgrade icon invocation now.
      // Note that we always return if /U is passed. The installation
      // procedure should NEVER run.
      */

      if (FindParameter("/U"))
      {
         BOOL fDoIt = GetParameter("/U", csUpgradeDirectory.GetBuffer(_MAX_PATH)) != NULL;
         csUpgradeDirectory.ReleaseBuffer();

         if (fDoIt && !csUpgradeDirectory.IsEmpty())
         {
         /* We need to upgrade our icons. */
            UpgradeIcons(csUpgradeDirectory);
         }

         /* And be done.*/
         return FALSE;
      }

      /*
      // Attempt to read the PrintMaster program file list from INSTALL.PFL in the root source directory.
      // The file must exist. INSTALL.PFL contains a listing of the files in the PMW product.
      */

      GetProgramFileList();

      /*
      // Create the main window.
      */
         
      if ((pMainWindow = new CInstallMainFrame) == NULL)
      {
         AfxThrowMemoryException();
      }

      if (!m_fMonochrome)
      {     
         pMainWindow->SetColor(RGB(0,0,255),RGB(0,0,0));
      }
      
      else
      {
         pMainWindow->SetColor(RGB(255,255,255),RGB(0,0,0));
      }
      
      if (!pMainWindow->Create(
            0,
            csClassName,
            csApplicationTitle,
            WS_OVERLAPPED|WS_CAPTION,
            CRect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)),
            NULL,
            0,
            NULL))
      {
         GetConfiguration()->MessageBox(IDS_ErrCantCreateMainWindow, 0, MB_OK);
         AfxThrowUserException();
      }

      pMainWindow->LoadAccelTable(MAKEINTRESOURCE(IDR_MAINFRAME));
      
      m_pMainWnd = pMainWindow;
      pMainWindow->ShowWindow(m_nCmdShow);
      pMainWindow->UpdateWindow();

      // Figure out if we're already installed.
      CString csPmwPath;

      if (fAutoRun)
      {
         Util::GetRegistryString(
            HKEY_LOCAL_MACHINE,
            GetAppPathKey(),
            NULL,
            csPmwPath);
      }

      BOOL fInstalled = !csPmwPath.IsEmpty()
                        && Util::FileExists(csPmwPath)
                        && (m_PreviousInfo == m_InstallInfo);

      // Figure out what the user wants to do...
      // install, run, uninstall, cancel?

      int nResult;

      // Branch on the type of installation.
      if (m_fTwainInstall)
      {
         CPmwDialog Dialog(IDD_INSTALL_TWAIN, m_pMainWnd);
         nResult = Dialog.DoModal();
      }
      else if (m_fWorkstationInstall)
      {
         // Workstation installation.

         CPmwDialog Dialog(IDD_INSTALL_WORKSTATION, m_pMainWnd);
         nResult = Dialog.DoModal();
      }
      else
      {
         // Normal installation.

         /*
         // Put up the fancy about box.
         */

         CInstallAboutDialog aboutDlg(fInstalled, m_pMainWnd, m_fMonochrome);
         nResult = aboutDlg.DoModal();
      }

      if (nResult == IDCANCEL)
      {
         // User wants to bail.
         return FALSE;
      }

      if (nResult == IDC_REMOVE)
      {
         // Run the uninstaller.
         CString csUninstallPath;

         BOOL fSuccess = FALSE;

         if (Util::GetRegistryString(
               HKEY_LOCAL_MACHINE,
               GetUninstallKey(),
               "UninstallString",
               csUninstallPath))
         {
            fSuccess = DoWinExec(csUninstallPath);
         }

         if (!fSuccess)
         {
            GetConfiguration()->MessageBox(IDS_CantUninstall);
         }

         return FALSE;
      }

      // Result must be IDOK so we install or run.

      if (nResult == IDC_RUN)
      {
         // Run PrintMaster.
         DoWinExec(csPmwPath);
         return FALSE;
      }

      // Continue with the installation.

      if (m_fTwainInstall)
      {
         // Installing TWAIN.

         /*
         // Do the main work of installing the program.
         */
         
         InstallProgram(m_pMainWnd);
         
         /*
         // Congratulations!
         */
            
			CPmwDialog Dialog(IDD_TWAIN_CONGRATULATIONS, m_pMainWnd);
			Dialog.DoModal();
      }
      else if (m_fWorkstationInstall)
      {
         // Installing for a network workstation.

         /*
         // Do the main work of installing the program.
         */
         
         InstallProgram(m_pMainWnd);

			/*
         // Congratulations!
			*/
         
			CPmwDialog Dialog(IDD_INSTALL_CONGRATULATIONS, m_pMainWnd);
			Dialog.DoModal();

         /*
         // Install the AT&T stuff if the user wants.
         */
            
         InstallIsp(m_pMainWnd);
      }
      else
      {
         // Normal installation.

         /*
         // Require the user to provide some personal information.
         */
         
         GetUserInformation(m_pMainWnd);
         
         /*
         // The user is legitimate, get the installation parameters:
         // installation directory and workspace size.
         */
            
         if (GetInstallationParameters(m_pMainWnd))
         {
            /*
            // Delete any old VFL files & content before installing in case they are
            // upgrading from a previous version.
            */


            DeleteOldVFLs();
				DeleteOldContent();

            /*
            // Do the main work of installing the program.
            */

            InstallProgram(m_pMainWnd);

				/*
	         // Congratulations!
				*/
            
				CPmwDialog Dialog(IDD_INSTALL_CONGRATULATIONS, m_pMainWnd);
				Dialog.DoModal();

				/*
				// Install the AT&T stuff if the user wants.
				*/
            
            InstallIsp(m_pMainWnd);
         }
      }
      
      // got all the way through with no exceptions
      fSuccess = TRUE;
   }
   CATCH(CMemoryException, e)
   {
      GetConfiguration()->MessageBox(IDS_ErrNotEnoughMemory, 0, MB_OK);
   }
   AND_CATCH(CNotSupportedException, e)
   {
      GetConfiguration()->MessageBox(IDS_ErrUnhandledNotSupportedException, 0, MB_OK);
   }
   AND_CATCH(CArchiveException, e)
   {
      GetConfiguration()->MessageBox(IDS_ErrUnhandledArchiveException, 0, MB_OK);
   }
   AND_CATCH(CFileException, e)
   {
      CString csCause;
      CString csError;
         
      TRY
      {
         LoadConfigurationString(GetFileExceptionCauseStringID(e), csCause);
         LoadConfigurationString(IDS_ErrUnhandledFileExceptionWithCause, csError);
         Util::MessageBox(
            MB_OK,
            (UINT)-1,
            (LPCSTR)csError,
            (LPCSTR)csCause);
      }
      CATCH_ALL(e)
      {
         csCause.Empty();
         csError.Empty();

         GetConfiguration()->MessageBox(IDS_ErrUnhandledFileException, 0, MB_OK);
      }
      END_CATCH_ALL
   }
   AND_CATCH(CResourceException, e)
   {
      GetConfiguration()->MessageBox(IDS_ErrNotEnoughResources, 0, MB_OK);
   }
   AND_CATCH(CUserException, e)
   {
      GetConfiguration()->MessageBox(IDS_ErrInstallationCantBeCompleted, 0, MB_OK);
   }
   AND_CATCH(CException, e)
   {
      GetConfiguration()->MessageBox(IDS_ErrUnhandledException, 0, MB_OK);
   }
   END_CATCH
   
   if (pMainWindow != NULL)
   {
      m_pMainWnd = NULL;
      pMainWindow->DestroyWindow();
      pMainWindow = NULL;
   }

   ddeShutdown();

   // if we weren't successful, dont't install the AT&T stuff.
   if (!fSuccess)
   {
      m_fInstallIsp = FALSE;
   }

   return FALSE;  // return false to terminate application
}

void CInstallApp::UpgradeIcons(LPCSTR pszDirectory)
{
   m_csInstallDirectory = pszDirectory;
   Util::RemoveBackslashFromPath(m_csInstallDirectory);
   MakeProgramManagerGroup();
}

CString CInstallApp::GetUninstallKey(void)
{
   CString csKey = REGSTR_PATH_UNINSTALL;
   csKey += '\\';
   csKey += m_csRegistryName;
   csKey += ' ';
   csKey += m_csRegistryVersion;

   return csKey;
}

CString CInstallApp::GetAppPathKey(void)
{
	
   CString csKey = REGSTR_PATH_APPPATHS "\\";
	csKey += GetExeName();

   return csKey;
}

CString CInstallApp::GetExeName()
{
	
	if(strlen(m_MsregIniFile.Name()) == 0)
	{
		//THE ini file should be inited before calling this funciton
		ASSERT(0);
		return "";
	}

   return m_MsregIniFile.GetString(SECTION_Configuration, "exe", "pmw.exe");
}

CString CInstallApp::GetMsregUsrIniFile(void)
{
   CString csPath;
   CString csDirectory;
   TRY
   {
      Util::GetWindowsDirectory(csDirectory);
      Util::ConstructPath(csPath, csDirectory, "MSREGUSR.INI");
   }
   CATCH_ALL(e)
   {
      csPath.Empty();
   }
   END_CATCH_ALL

   return csPath;
}

int CInstallApp::ExitInstance()
{
   if (m_fInstallIsp) 
   {  
      CString csStartupName;
      CString csPath;

		m_fMustReboot = FALSE;

      TRY
      {
         // Start up AT&T WorldNet Service installation.
         LoadConfigurationString(IDS_IspInstallName, csStartupName);
         Util::ConstructPath(csPath, GetSourceDirectory(), csStartupName);

			if (!DoWinExec(csPath))
         {
            Util::MessageBox(
               MB_OK,
               (UINT)-1,
               MAKEINTRESOURCE(IDS_ErrCantStartProgramFormat),
               (LPCSTR)csStartupName);
         }
      }
      END_TRY
   }

	if (m_InstDLL != NULL)
	{
		::FreeLibrary(m_InstDLL);
		m_InstDLL = NULL;
	}

	if (m_fMustReboot)
	{
		CDialog RebootDialog(IDD_MUST_REBOOT);
		if (RebootDialog.DoModal() == IDOK)
		{
			// On NT we need to do more than this... see help for ExitWindowsEx().
			::ExitWindowsEx(EWX_REBOOT, 0);
		}
	}

   return CWinApp::ExitInstance();     // Call the default.
}

void CInstallApp::WinHelp(DWORD dwData, UINT nCmd)
{
   if (dwData != 0x20000+IDD_INSTALL_ABOUT)
   {
#ifdef DOSOUNDS
      if (sound_manager.in_sound_inspector())
      {
         sound_manager.sound_inspect(dwData, nCmd);
         return;
      }
#endif
      if (m_fTrapHelpMessages)
      {
         char buffer[50];
         wsprintf(buffer, "Command: %x, Help id: %lx", nCmd, dwData);
         if (::MessageBox(
               NULL,
               buffer,
               "You Asked for Help?",
               MB_OKCANCEL | MB_ICONQUESTION) != IDOK)
         {
            return;
         }
      }
      CWinApp::WinHelp(dwData, nCmd);
   }
}

/////////////////////////////////////////////////////////////////////////////
// CInstallApp commands

BOOL CInstallApp::CheckForPreviousInstance(LPCSTR lpszTitle)
{
   HWND hWindow = ::FindWindow(lpszTitle, NULL);

   if (::IsWindow(hWindow))
   {
      HWND hWindow1;

      hWindow1 = ::GetLastActivePopup(hWindow);

      if (::IsWindow(hWindow1))
      {
         hWindow = hWindow1;
      }

      ::BringWindowToTop(hWindow);

      if (::IsIconic(hWindow))
      {
         ::ShowWindow(hWindow, SW_RESTORE);
      }
      return TRUE;
   }
   
   return FALSE;
}
   
BOOL CInstallApp::GetInstallationParameters(CWnd *pcwndParent)
{
   BOOL fHaveInstallationParameters = FALSE;
   
   m_csInstallDirectory.Empty();
   m_dwWorkspaceSize = 0;
            
   CInstallDefaultDialog defaultDlg(pcwndParent);
   CInstallDiskSpaceDialog diskspaceDlg(pcwndParent);
   CInstallOptionsDialog optionsDlg(pcwndParent);

   CString csDefaultDirectory;
   CString csFormat;
   
   TRY
   {
      /*
      // Ask the use if he wants to use the default parameters.
      */

      BOOL fAskForDefault = TRUE;

      // Get the default directory that will be used for installation.
      if (m_csPreviousInstallDirectory.IsEmpty())
      {
         LoadConfigurationString(IDS_DefaultInstallDirectory, csDefaultDirectory);
      }
      else
      {
         csDefaultDirectory = m_csPreviousInstallDirectory;
      }

      // If we have a demo, check if we can install into the default directory.
      if (m_InstallInfo.m_nInstallConfiguration == CPrintMasterConfiguration::Demo)
      {
         if (!DemoCanBeInstalledIntoDirectory(csDefaultDirectory))
         {
            csDefaultDirectory = "c:\\pmwdemo";
            if (!DemoCanBeInstalledIntoDirectory(csDefaultDirectory))
            {
               // Well, we can't find a good choice to install the
               // demo. So, don't ask the user if they want to install
               // into a default directory when it's going to fail.

               fAskForDefault = FALSE;
            }
         }
      }

      // Ask the user if they want to use the default directory.
      defaultDlg.m_csDirectory = csDefaultDirectory;
      optionsDlg.m_csPreviousInstallDirectory = csDefaultDirectory;

      int nResult = IDOK;

      if (fAskForDefault)
      {
         nResult = defaultDlg.DoModal();
      }
      
      /*
      // Repeat until we have legal parameters or the user wants to abort.
      */
      
      while (!fHaveInstallationParameters)
      {
         /*
         // nResult can have the following values at this point:
         //
         //    IDCANCEL                   means the user wants to exit
         //    IDOK                       means use the defaults
         //    IDC_CUSTOM_INSTALLATION    means get custom installation parameters
         */
                  
         if (nResult == IDCANCEL)
         {
            // exit installation
            break;
         }
                  
         if (nResult == IDOK)
         {
            // If we are installing a demo, check if we can
            // install into the directory.
            if ((m_InstallInfo.m_nInstallConfiguration == CPrintMasterConfiguration::Demo)
             && (!DemoCanBeInstalledIntoDirectory(defaultDlg.m_csDirectory)))
            {
               LoadConfigurationString(IDS_CantInstallDemoToDirectory, csFormat);
               Util::MessageBox(
                     MB_OK|MB_DEFBUTTON1,
                     (UINT)-1,
                     (LPCSTR)csFormat,
                     (LPCSTR)(defaultDlg.m_csDirectory));

               // use custom installation
               nResult = IDC_CUSTOM_INSTALLATION;
            }
            else
            {
               /*
               // The user wants to go with defaults. Make sure the disk
               // information for the default directroy is available.
               */
               
               if ((defaultDlg.m_dwProgramSize == DISK_SPACE_NOT_AVAILABLE)
                || (defaultDlg.m_dwWorkspaceSize == DISK_SPACE_NOT_AVAILABLE))
               {
                  /*
                  // The directory cannot be used for some reason.
                  // Report this to the user and go to the custom
                  // installation.
                  */
                  
                  Util::MessageBox(
                     MB_OK,
                     (UINT)-1,
                     MAKEINTRESOURCE(IDS_ErrCantUseDirectory),
                     (LPCSTR)defaultDlg.m_csDirectory);
                  
                  // use custom installation
                  nResult = IDC_CUSTOM_INSTALLATION;
               }
                        
               else
               {
                  /*
                  // The disk space information is available for the default
                  // directory, make sure there is enough disk space and that
                  // the directory can be created.
                  */
                  
                  if (diskspaceDlg.CheckDiskSpace(
                        defaultDlg.m_csDirectory,
                        defaultDlg.m_dwProgramSize+defaultDlg.m_dwWorkspaceSize) == IDOK)
                  {
                     /*
                     // The default options check out, return them.
                     */
                              
                     m_csInstallDirectory = defaultDlg.m_csDirectory;
                     m_dwWorkspaceSize = defaultDlg.m_dwWorkspaceSize;
                     
                     fHaveInstallationParameters = TRUE;
                  }
                           
                  else
                  {
                     /*
                     // The default options are not usable, try a custom installation.
                     */
                     
                     nResult = IDC_CUSTOM_INSTALLATION;
                  }
               }
            }
         }
                  
         if (nResult == IDC_CUSTOM_INSTALLATION)
         {
            /*
            // The user wants (or needs) to do a custom installation.
            */
            
            if ((nResult = optionsDlg.DoModal()) == IDOK)
            {
               /*
               // The custom parameters check out. Return them.
               */
                           
               m_csInstallDirectory = optionsDlg.m_csInstallDirectory;
               m_dwWorkspaceSize = optionsDlg.m_dwWorkspaceSize;
               
               fHaveInstallationParameters = TRUE;
            }
         }
      }
   }
   CATCH_ALL(e)
   {
      /*
      // Some exception occured. Clean up and pass the exception back up.
      */
      
      m_csInstallDirectory.Empty();
      m_dwWorkspaceSize = 0;
      
      THROW_LAST();
   }
   END_CATCH_ALL
   
   return fHaveInstallationParameters;
}

static BOOL ZapDirectory(LPCSTR pPath, LPCSTR pFileSpec)
{
   CString csPath;
   CString csSubpath;
   CFileIterator Iter;

   BOOL fSuccess = FALSE;
   BOOL fEmpty = TRUE;

   TRY
   {
   /*
   // First try and delete all files matching "pFileSpec".
   */
      Util::ConstructPath(csPath, pPath, pFileSpec);

      BOOL fRet;

      for (fRet = Iter.FindFirst(csPath, _A_NORMAL);
           fRet;
           fRet = Iter.FindNext())
      {
         LPCSTR pName = Iter.Name();
         Util::ConstructPath(csSubpath, pPath, pName);   // use csSubpath...
         if (remove(csSubpath) != 0)
         {
            fEmpty = FALSE;
         }
      }
      Iter.Close();

   /*
   // Second, recurse into all subdirectories.
   // We also compute our empty status at this time.
   */

      Util::ConstructPath(csPath, pPath, "*.*");

      for (fRet = Iter.FindFirst(csPath, _A_NORMAL | _A_SUBDIR);
           fRet;
           fRet = Iter.FindNext())
      {
         LPCSTR pName = Iter.Name();

         if (Iter.Attributes() & _A_SUBDIR)
         {
            if (pName[0] != '.')
            {
            /* Recurse and zap! */
               Util::ConstructPath(csSubpath, pPath, pName);
               if (!ZapDirectory(csSubpath, pFileSpec))
               {
                  fEmpty = FALSE;
               }
            }
         }
         else
         {
            fEmpty = FALSE;
         }
      }
      Iter.Close();

      /*
      // If we are now empty, try and delete us.
      // (If it fails, we aren't considered empty.)
      */

      if (fEmpty)
      {
      /* Try and delete us! */
         if (_rmdir(pPath) != 0)
         {
         /* Couldn't delete! */
            fEmpty = FALSE;
         }
      }
      fSuccess = TRUE;
   }
   END_TRY

   return (fSuccess && fEmpty);
}

/*
// Run through the picture directory (if it exists) and delete any
// VFL files that may exist from a previous installation.
*/

void CInstallApp::DeleteOldVFLs(void)
{
   CString csPath;
   TRY
   {
      Util::ConstructPath(csPath, m_csInstallDirectory, "PICTURES");
      ZapDirectory(csPath, "*.VFL");
   }
   END_TRY
}

/*
// Delete content from old versions
*/

void CInstallApp::DeleteOldContent(void)
{
   static CString csDeleteSpec[] = {
		"PMG4.INI",
		"PM4X.INI",
		"PMPL.INI",
		"PREMIER.INI",
		"OFFICE.INI",
		"CAT4ART.*",
		"PMG4ART.*",
		"PMG4PROJ.*",
		"PMG4SENT.*",
		"PMG4XPRN.*",
		"PMG4XART.*",
		"PREMIERA.*",
		"PREMIERG.*",
		"PMPLART*.*",
		"PMPLPHO*.*"
	};

	CString csPath;
   TRY
   {
      Util::ConstructPath(csPath, m_csInstallDirectory, "CONTENT");
		int nSpecs = sizeof(csDeleteSpec) / sizeof(CString);
		for (int i = 0; i < nSpecs; i++)
		{
			ZapDirectory(csPath, (const char*)csDeleteSpec[i]);
		}
   }
   END_TRY
}

void CInstallApp::InstallProgram(CWnd *pcwndParent)
{
   TRACE("Install Program\n");

   // disable ourselves so the user cannot click on us while installing
   m_pMainWnd->EnableWindow(FALSE);

   CInstallProgressDialog ProgressDlg(IDD_INSTALL_PROGRESS, pcwndParent);

   TRY
   {
      /*
      // Write the installation data out to the INST.DAT file.
      // Any existing network counts will be merged so that the
      // current user license will remain in effect.
      */

      if (!m_fWorkstationInstall && !m_fTwainInstall)
      {
         WriteInstallationInformation(pcwndParent, &ProgressDlg);
      }

      /*
      // Copy the program files.
      */

      CopyFiles(pcwndParent, &ProgressDlg);

      /*
      // Call Regedit if we copied over any new OLE files.
      */

      if (m_fCallRegEdit)
      {
         TRACE("Call REGEDIT\n");
         DoWinExec("REGEDIT.EXE /S OLE2.REG");
      }

      /*
      // Write the important keys to the PrintMaster INI files.
      */
      
      if (!m_fWorkstationInstall && !m_fTwainInstall)
      {
         WriteIniEntries(pcwndParent, &ProgressDlg);

         // If the user had a previous installation and it was of a lesser
         // level, unregister the user.

         if (m_InstallInfo.m_nInstallConfiguration > m_PreviousInfo.m_nInstallConfiguration)
         {
            CString csPath = GetMsregUsrIniFile();

            if (!csPath.IsEmpty())
            {
               CIniFile IniFile(csPath);

            /* Unregister them! */

               IniFile.RemoveEntry(m_csApplicationName, "Registered");
               IniFile.RemoveEntry(m_csApplicationName, "RegistrationCode");
            }
         }
      }
      
      if (!m_fTwainInstall)
      {
			// Register the PrintMaster automation.
			RegisterPrintMasterAutomation();

         CString csMessage;
			csMessage.LoadString(IDS_Install_Waiting_For_ArtStore);
         ProgressDlg.ShowItem(csMessage);
			ProgressDlg.HideProgress();

			// Install the Art & More Store.
			InstallArtStore(&ProgressDlg);
			ProgressDlg.ShowItem("");

			// Install Serif DrawPlus if required
			if (!GetConfiguration()->RemoveSerifDraw())
			{
				if(GetConfiguration()->Product() != CPrintMasterConfiguration::plAmericanGreetings)
				{
					CString csMessage;
					csMessage.LoadString(IDS_Install_Waiting_For_DrawPlus);
					ProgressDlg.ShowItem(csMessage);
					ProgressDlg.HideProgress();

					InstallDrawPlus(&ProgressDlg);
					ProgressDlg.ShowItem("");
				}
			}

			/*
			// Install Web Publishing Wizard if needed.
			*/

			HINSTANCE hInstance = LoadLibrary("WEBPOST.DLL");
			if (hInstance != NULL)
			{
				FreeLibrary(hInstance);
			}
			else
			{
				// Web Publishing Wizard is not installed on the computer.
				// Do a quiet installation of Web Publishing Wizard.
				CString csPath;
				Util::ConstructPath(
					csPath,
					GetSourceDirectory(),
					"INSTALL\\IE4\\WPIE415.EXE /Q:A");
				TRACE("Install Web Publishing Wizard: %s\n", (LPCSTR)csPath);
				ProgressDlg.EnableAbort(FALSE);
				ProgressDlg.HideProgress();
	         ProgressDlg.ShowAction("Installing Web Publishing Wizard");
	         ProgressDlg.ShowItem("Please wait, this may take several minutes...");
	         ProgressDlg.ShowAnimation(MAKEINTRESOURCE(ID_AVI_COPY_FILES));
				DoWinExec(csPath, NULL, TRUE, &ProgressDlg);
	         ProgressDlg.HideAnimation();
	         ProgressDlg.HideItem();
	         ProgressDlg.HideAction();
				ProgressDlg.EnableAbort(TRUE);
				TRACE("Back from Web Publishing Wizard installation\n");

				if (ProgressDlg.CheckForAbort())
				{
					AfxThrowUserException();
				}
			}

			/*
			// Install Explorer 4.0 if needed.
			*/

			CInternetExplorerHelper InternetExplorerHelper;
			const CInternetExplorerVersion& Version = InternetExplorerHelper.GetVersion();

			if (Version.IsVersion3() || Version.IsVersion4())
			{
				// Internet Explorer 3 or 4 is installed. Life is good.
				/*
				// Update HTML help if necessary.
				*/

				CString csPath;
				Util::ConstructPath(
					csPath,
					GetSourceDirectory(),
					"INSTALL\\IE4\\HHUPD.EXE");
				if(Util::FileExists(csPath))
				{
					// Add parameters to app path.
					csPath += " /Q:A";

					TRACE("Install HTML Help Update: %s\n", (LPCSTR)csPath);
					ProgressDlg.EnableAbort(FALSE);
					ProgressDlg.HideProgress();
					ProgressDlg.ShowAction("Updating HTML Help");
					ProgressDlg.ShowItem("Please wait, this may take several minutes...");
					ProgressDlg.ShowAnimation(MAKEINTRESOURCE(ID_AVI_COPY_FILES));
					DoWinExec(csPath, NULL, TRUE, &ProgressDlg);
					ProgressDlg.HideAnimation();
					ProgressDlg.HideItem();
					ProgressDlg.HideAction();
					ProgressDlg.EnableAbort(TRUE);
					TRACE("Back from HTML Help Update installation\n");

					if (ProgressDlg.CheckForAbort())
					{
						AfxThrowUserException();
					}
				}
			}
			else
			{
				CString csPath;
				Util::ConstructPath(
					csPath,
					GetSourceDirectory(),
					"INSTALL\\IE4\\IE4SETUP.EXE");
				if(Util::FileExists(csPath))
				{
					// Add command line parameters to app path
					csPath += " /Q:A /C:\"ie4wzd.exe /Q:A /X /I:N /R:N /S:\"\"#e\"\"\"";

					// Verify that the user wants to install IE 4.0
					CPmwDialog Dialog(IDD_IE_INSTALL, m_pMainWnd);
					if (Dialog.DoModal() == IDOK)
					{
						// Internet Explorer (3 or 4) is not installed on the computer.
						// Do a quiet installation of Explorer 4.0.
						TRACE("Install Internet Explorer 4.0: %s\n", (LPCSTR)csPath);
						ProgressDlg.EnableAbort(FALSE);
						ProgressDlg.HideProgress();
						ProgressDlg.ShowAction("Installing Internet Files");
						ProgressDlg.ShowItem("Please wait, this may take several minutes...");
						ProgressDlg.ShowAnimation(MAKEINTRESOURCE(ID_AVI_COPY_FILES));
						DoWinExec(csPath, NULL, TRUE, &ProgressDlg);
						ProgressDlg.HideAnimation();
						ProgressDlg.HideItem();
						ProgressDlg.HideAction();
						ProgressDlg.EnableAbort(TRUE);
						m_fMustReboot = TRUE;
						TRACE("Back from Internet Explorer installation\n");

						if (ProgressDlg.CheckForAbort())
						{
							AfxThrowUserException();
						}
					}
				}
			}

         /*
         // Convert any old address books that are around.
         */

         ConvertOldAddressBooks(&ProgressDlg);
   
         /*
         // Construct the PrintMaster Gold program manager group.
         */

         MakeProgramManagerGroup(&ProgressDlg);

         /*
         // Make sure we are correctly registered.
         */
   
         UpdateRegistry();
      }
   }
   CATCH_ALL(e)
   {
      ProgressDlg.DestroyWindow();
      
      // NOTE: the following line should be removed when we have REAL exceptions.
      
      ProgressDlg.~CInstallProgressDialog();
      
      THROW_LAST();
   }
   END_CATCH_ALL
   
   ProgressDlg.DestroyWindow();
   
   // reenable the main window
   m_pMainWnd->EnableWindow(TRUE);
   TRACE("End Install Program\n");
}

void CInstallApp::InstallDrawPlus(CWnd* pParent)
{
	// Inform the user what we are about to do.
	CPmwDialog Dialog(IDD_DRAWPLUS_INSTALL, pParent);
	Dialog.DoModal();

	// Launch the setup program.
	CString csPath;
	Util::ConstructPath(csPath, GetSourceDirectory(), "INSTALL\\DRAWPLUS\\SETUP.EXE");
   pParent->EnableWindow(FALSE);
	if (DoWinExec(csPath))
	{
		// The DrawPlus installation program has more than one process, so we
		// can not track it with the fancy DoWinExec. We need to wait for the
		// window to come and go before continuing.

		CString csDrawPlusInstallTitle;
		csDrawPlusInstallTitle.LoadString(IDS_DrawPlusSetupTitle);
		Util::WaitForWindow(csDrawPlusInstallTitle, 5*60000L);
	}
	pParent->EnableWindow(TRUE);

	// Make sure we are on top and active.
	pParent->SetForegroundWindow();
	pParent->BringWindowToTop();
	pParent->UpdateWindow();
}

void CInstallApp::InstallArtStore(CWnd* pParent)
{
	// Inform the user what we are about to do.
//	CPmwDialog Dialog(IDD_DRAWPLUS_INSTALL, pParent);
//	Dialog.DoModal();

	// Launch the setup program.
	CString csPath;
	Util::ConstructPath(csPath, GetSourceDirectory(), "ARTSTORE\\SETUP.EXE");
   pParent->EnableWindow(FALSE);
	if (DoWinExec(csPath))
	{
		// The Art Store installation program has more than one process, so we
		// can not track it with the fancy DoWinExec. We need to wait for the
		// window to come and go before continuing.

		CString csInstallTitle;
		csInstallTitle.LoadString(IDS_ArtStoreSetupTitle);
		Util::WaitForWindow(csInstallTitle, 5*60000L);
	}
	pParent->EnableWindow(TRUE);

	// Make sure we are on top and active.
	pParent->SetForegroundWindow();
	pParent->BringWindowToTop();
	pParent->UpdateWindow();
}

void CInstallApp::RegisterPrintMasterAutomation(void)
{
	// {83DC6B35-719A-11D1-9828-00A0246D4780}
	static const IID IID_IPrintMasterDispatch =
	{ 0x83dc6b35, 0x719a, 0x11d1, { 0x98, 0x28, 0x0, 0xa0, 0x24, 0x6d, 0x47, 0x80 } };
	static char cbServer[] = "PrintMaster.Application";

	// We need to do a bit of stuff by hand here.
	// Specifically, we need to set up these strings ourselves. The default
	// MFC strings would point the automation to the install program! So we
	// make local copies of the strings and change the %3 value ourselves
	// (which is the program executable name).

	static const TCHAR sz00[] = _T("%2\0") _T("%5");
	static const TCHAR sz01[] = _T("%2\\CLSID\0") _T("%1");
	static const TCHAR sz05[] = _T("CLSID\\%1\0") _T("%5");
	static const TCHAR sz06[] = _T("CLSID\\%1\\ProgID\0") _T("%2");
	static const TCHAR sz07[] = _T("CLSID\\%1\\InprocHandler32\0") _T("ole32.dll");
//	static const TCHAR sz08[] = _T("CLSID\\%1\\LocalServer32\0") _T("%3");
	static const TCHAR sz08[] = _T("CLSID\\%1\\LocalServer32");

	static TCHAR szNew08[_MAX_PATH+30];

	// registration for OAT_DISPATCH_OBJECT
	static LPCTSTR rglpszDispatchRegister[] =
	{
		sz00, sz05, NULL
	};
	// overwrite entries for OAT_DISPATCH_OBJECT
	static LPCTSTR rglpszDispatchOverwrite[] =
	{
		sz01, sz06, sz07, szNew08, NULL
	};

	// Get the path to the executable.
	CString csPath;
	CString csExeName = GetExeName();
   Util::ConstructPath(csPath, m_csInstallDirectory, csExeName);

	// Build szNew08.
	memcpy(szNew08, sz08, sizeof(sz08));
	strcpy(szNew08+sizeof(sz08), csPath);

	AfxOleRegisterServerClass(IID_IPrintMasterDispatch,
										cbServer,
										cbServer,
										cbServer,
										OAT_DISPATCH_OBJECT,
										rglpszDispatchRegister,
										rglpszDispatchOverwrite);
}

void CInstallApp::WriteInstallationInformation(CWnd *pcwndParent, CInstallProgressDialog *pProgressDlg)
{
   ASSERT(!m_fWorkstationInstall);

   TRACE("Write Installation Information\n");
   CString csResource;
   CString csPath;
   CString csFile;
   
   struct InstallationInfo *pCurrentInstallationInfo = NULL;
   
   TRY
   {
      if (pProgressDlg != NULL)
      {
         /*
         // Tell the user what's going on.
         */
         
         LoadConfigurationString(IDS_WritingInstallationInformation, csResource);
         pProgressDlg->ShowItem(csResource);
         
         if (pProgressDlg->CheckForAbort())
         {
            AfxThrowUserException();
         }
      }
      
      /*
      // If there is a current installation information file, load it to
      // get the network license count. Use max(new, current).
      */
      
      LoadConfigurationString(IDS_InstallationInformationFileName, csFile);
      Util::ConstructPath(csPath, m_csInstallDirectory, csFile);
      
      if ((pCurrentInstallationInfo = new struct InstallationInfo) == NULL)
      {
         AfxThrowMemoryException();
      }
      
      BYTE bCurrentNetCount = 0;
      
      if (pCurrentInstallationInfo->Get(csPath, FALSE))
      {
         bCurrentNetCount = pCurrentInstallationInfo->bNetCount;
      }
      
      struct InstallationInfo& NewInstallationInfo = GetUserInformation(pcwndParent);
      
      if (bCurrentNetCount > NewInstallationInfo.bNetCount)
      {
         NewInstallationInfo.bNetCount = bCurrentNetCount;
      }
      
      /*
      // Set the configurations.
      */

      NewInstallationInfo.nInstallConfiguration = m_InstallInfo.m_nInstallConfiguration;
      NewInstallationInfo.nCurrentConfiguration = m_InstallInfo.m_nCurrentConfiguration;

      /*
      // Write out the the new installation data file.
      */
      
      if (!NewInstallationInfo.Put(csPath))
      {
         Util::MessageBox(
            MB_OK,
            (UINT)-1,
            MAKEINTRESOURCE(IDS_ErrCantWriteInstallationInformationFormat),
            (LPCSTR)csPath);
         AfxThrowUserException();
      }
   }
   CATCH_ALL(e)
   {
      csResource.Empty();
      csPath.Empty();
      csFile.Empty();
      
      delete pCurrentInstallationInfo;
      pCurrentInstallationInfo = NULL;
      
      if (pProgressDlg != NULL)
      {
         pProgressDlg->HideItem();
      }
      
      THROW_LAST();
   }
   END_CATCH_ALL
   
   delete pCurrentInstallationInfo;
   pCurrentInstallationInfo = NULL;
   
   if (pProgressDlg != NULL)
   {
      pProgressDlg->HideItem();
   }
   TRACE("End Write Installation Information\n");
}

void CInstallApp::CopyFiles(CWnd *pcwndParent, CInstallProgressDialog *pProgressDlg)
{
   SetOleInUse();

   TRACE("Copy Files\n");
   CString csResource;
                              
   TRY
   {
      /*
      // Loop through each of the files in the program file list and copy them to the
      // destination directory.
      */
      
      const CProgramFileList &ProgramFileList = GetProgramFileList();
      
      /*
      // Set up the progress dialog for the copying process.
      */
      
      if (pProgressDlg != NULL)
      {
         pProgressDlg->SetMinMaxPos(0, ProgramFileList.GetFileSize((unsigned int)-1, 1), 0);
         pProgressDlg->ShowAction(LoadConfigurationString(IDS_CopyingProgramFiles, csResource));
         pProgressDlg->ShowProgress();
      }
      
      int nIndex;
      
      m_csLastSourceDirectory.Empty();

      for (nIndex = 0; nIndex < ProgramFileList.GetSize(); nIndex++)
      {
         const CProgramFile *pProgramFile;
         
         if ((pProgramFile = (CProgramFile *)ProgramFileList.GetAt(nIndex)) != NULL)
         {
            CopyFile(pProgramFile, pcwndParent, pProgressDlg);
         }
      }
      
      pProgressDlg->SetPosPercentage(100);
   }
   CATCH_ALL(e)
   {
      /*
      // Some exception occured. Clean up and pass the exception back up.
      */
      
      csResource.Empty();
      
      if (pProgressDlg != NULL)
      {
         pProgressDlg->HideAction();
         pProgressDlg->HideProgress();
      }
      
      THROW_LAST();
   }
   END_CATCH_ALL
      
   if (pProgressDlg != NULL)
   {
      pProgressDlg->HideAction();
      pProgressDlg->HideProgress();
   }
   TRACE("End Copy Files\n");
}

void CInstallApp::CopyFile(const CProgramFile *pProgramFile, CWnd *pcwndParent, CInstallProgressDialog *pProgressDlg)
{
   /*
   // Extract the various paths and file names.
   */
   
   CString csApplicationDirectory;
   CString csSourcePath;
   CString csDestinationPath;
   CString csDestinationDirectory;
   CString csDestinationFile;
   CString csCurrentDirectory;
   CString csInstallDirectory;
   CString csWindowsDirectory;
   CString csSystemDirectory;
   CString csExtension;
   CFile cfSource;
   CFile cfDestination;
   BYTE* lpBuffer = NULL;
   LPSTR lpSourceVersionInfo = NULL;
   LPSTR lpDestinationVersionInfo = NULL;
	BOOL fAbortOnError = TRUE;
	
   TRY
   {
      /*
      // Get a copy of the installation directory without a backslash
      // to pass to VerFindFile().
      */

      csApplicationDirectory = m_csInstallDirectory;
      Util::RemoveBackslashFromPath(csApplicationDirectory);

      /*
      // Figure out where the file is coming from and where it is going.
      // The destination file name is shown in the progress dialog.
      */
      
      Util::ConstructPath(csSourcePath, GetSourceDirectory(), pProgramFile->GetSource());
      
      /*
      // Touch the directory as necessary (for Packard Bell).
      */

      TouchSourceDirectory(csSourcePath);

      /*
      // Check for special cases where we want to override destination
      // directory specified in INSTALL.PFL:
      //
      //    OLE Group (16):                     use OLE destination directory (16 bit only)
      //    Otherwise:                          use installation directory
      */
      
      // If csDestinationFile is empty at the end of all this, then the default value is used.
      csDestinationFile.Empty();

      if ((pProgramFile->GetGroups() & CProgramFile::OleGroup) != 0)
      {
         Util::GetSystemDirectory(csDestinationDirectory);
      }
      else
      {
         csDestinationDirectory = m_csInstallDirectory;
      }
      
      if (csDestinationFile.IsEmpty())
      {
         csDestinationFile = pProgramFile->GetDestination();
      }

      // This code is necessary since the file name may actually have path
      // components at this point.     
      Util::ConstructPath(csDestinationPath, csDestinationDirectory, csDestinationFile);
      Util::SplitPath(csDestinationPath, &csDestinationDirectory, &csDestinationFile);
      Util::RemoveBackslashFromPath(csDestinationDirectory);
      
      if (pProgressDlg != NULL)
      {
         pProgressDlg->ShowItem(csDestinationFile);
      }

      TRACE("Copy File |%s| to |%s|\n", (LPCSTR)csSourcePath, (LPCSTR)csDestinationPath);

      // Figure out if we can skip a lot of the versioning stuff.

      BOOL fFastFile = FALSE; // Default to doing versioning - DGP

      Util::SplitPath(csDestinationFile, NULL, NULL, NULL, &csExtension);
      csExtension.MakeUpper();
      
      // Skip versioning for non .EXE, .DLL, .DRV, and .DAT files which are in the program group and are not shared.
      if ((csExtension != ".EXE")
       && (csExtension != ".DLL")
       && (csExtension != ".DRV")
       && (csExtension != ".DAT")
       && (pProgramFile->GetGroups() & CProgramFile::ProgramGroup) != 0
       && (pProgramFile->GetFlags() & CProgramFile::Shared) == 0)
      {
         fFastFile = TRUE;   // Skip versioning - DGP
      }

      // This flag is used to tell if we should actually install the file.

      BOOL fInstallFile = TRUE;

      // Only copy OLE files if OLE is not in use.

      if ((pProgramFile->GetGroups() & CProgramFile::OleGroup) != 0)
      {
         if (m_fOleInUse)
         {
            fInstallFile = FALSE;
         }
      }

      // If this is a workstation installation, only copy the OLE files 
      if (m_fWorkstationInstall)
      {
         if ((pProgramFile->GetGroups() & CProgramFile::OleGroup) == 0)
         {
            fInstallFile = FALSE;
         }
      }

      // If this is a TWAIN installation, only copy the TWAIN files 
      if (m_fTwainInstall)
      {
         if ((pProgramFile->GetGroups() & CProgramFile::TwainGroup) == 0)
         {
            fInstallFile = FALSE;
         }
      }
      else if ((pProgramFile->GetGroups() & CProgramFile::TwainGroup) != 0)
      {
         // If we're not doing a TWAIN install, then do not copy TWAIN files.
         fInstallFile = FALSE;
      }

      if (fInstallFile)
      {
         /*
         // Get the directory into which the file should be installed
         // using VerFindFile().
         */

         UINT uCurrentLength = _MAX_PATH+1;
         UINT uInstallLength = _MAX_PATH+1;
         UINT uResult;

         Util::GetWindowsDirectory(csWindowsDirectory);
         Util::GetSystemDirectory(csSystemDirectory);

         TRACE("Call VerFindFile\n");
         if (fFastFile)
         {
            uResult = 0;
         }
         else
         {
            uResult = VerFindFile(
               ((pProgramFile->GetFlags() & CProgramFile::Shared) != 0) ? VFFF_ISSHAREDFILE : 0,
               (LPSTR)(LPCSTR)csDestinationFile,
               (LPSTR)(LPCSTR)csWindowsDirectory,
               (LPSTR)(LPCSTR)csApplicationDirectory,
               csCurrentDirectory.GetBuffer(uCurrentLength),
               &uCurrentLength,
               csInstallDirectory.GetBuffer(uInstallLength),
               &uInstallLength);
         }

         TRACE("VerFindFile returned %04x\n", uResult);
         csCurrentDirectory.ReleaseBuffer();
         csInstallDirectory.ReleaseBuffer();

         if ((uResult & VFF_BUFFTOOSMALL) != 0)
         {
            TRACE("VerFindFile - VFF_BUFFTOOSMALL\n");
            // can't get directory information, buffer too small
            Util::MessageBox(
               MB_OK,
               (UINT)-1,
               MAKEINTRESOURCE(IDS_ErrVerFindFileFormat),
               (LPCSTR)csDestinationPath);
            AfxThrowUserException();
         }

         if ((pProgramFile->GetFlags() & CProgramFile::ForceWindowsDir) != 0)
         {
            csInstallDirectory = csWindowsDirectory;
         }
         else if ((pProgramFile->GetFlags() & CProgramFile::ForceSystemDir) != 0)
         {
            csInstallDirectory = csSystemDirectory;
         }

         Util::AppendBackslashToPath(csCurrentDirectory);
         Util::AppendBackslashToPath(csInstallDirectory);

         /*
         // Make sure VerFindFile() returned successfully.
         */

         if (((uResult & VFF_FILEINUSE) != 0) && (csDestinationDirectory.CompareNoCase(csCurrentDirectory) == 0))
         {
            TRACE("VerFindFile - VFF_FILEINUSE\n");
            // file is already in use, quit unless IgnoreIfInUse flag is set
            if ((pProgramFile->GetFlags() & CProgramFile::IgnoreIfInUse) == 0)
            {
               CString  csMessage;

               GetResourceString(csMessage, MAKEINTRESOURCE(IDS_ErrFileInUseFormat));
               Util::MessageBox(
                  MB_OK,
                  (UINT)-1,
                  (LPCSTR)csMessage,
                  (LPCSTR)csDestinationPath);
               AfxThrowUserException();
            }

            // don't install the file, ignore it
            fInstallFile = FALSE;
         }

         /*
         // There is a version of VER.DLL (5/22/92, 8736 Bytes) that likes to install everything
         // into the shared directory, usually C:\WINDOWS\SYSTEM, even if the file is not shared.
         // So, to defeat this unusual behavior, we only use the directory suggested by VerFindFile()
         // if the file is shared.
         */

         if ((pProgramFile->GetFlags() & CProgramFile::Shared) != 0) 
         {
            TRACE("File shared, use directory |%s|\n", (LPCSTR)csInstallDirectory);
            
            /*
            // Rebuild the destination information based on the directory returned
            // by VerFindFile().
            */
               
            Util::ConstructPath(csDestinationPath, csInstallDirectory, csDestinationFile);
            Util::SplitPath(csDestinationPath, &csDestinationDirectory, &csDestinationFile);
         }

         // The file was successfully copied. If it was shared, then record its
         // name so its reference count can be incremented in the registry.
      
            // TwainGroup files shouldn't get ref. count in registry
         if ((pProgramFile->GetFlags()  & CProgramFile::Shared) != 0 &&
            (pProgramFile->GetGroups() & CProgramFile::TwainGroup) == 0)
         {
            csaSharedFiles.Add(csDestinationPath);
         }
      }
      
      /*
      // If the IgnoreIfExists flag is set, then check if the file exists and,
      // if it does, skip it.
      */
      
      if ((pProgramFile->GetFlags() & CProgramFile::IgnoreIfExists) != 0)
      {
         if (Util::FileExists(csDestinationPath))
         {
            fInstallFile = FALSE;
         }
      }
         
      /*
      // Compare the versions.
      */
      
      BOOL fIsOlderVersion = FALSE;
      BOOL fIsSameVersion = FALSE;
      
      if (!fFastFile && fInstallFile)
      {
         // get size of source information
         DWORD dwSourceVersionInfoHandle;
         DWORD dwSourceVersionInfoSize;
         
         if ((dwSourceVersionInfoSize = GetFileVersionInfoSize((LPSTR)(LPCSTR)csSourcePath, &dwSourceVersionInfoHandle)) != NULL)
         {
            // get size of destination information
            DWORD dwDestinationVersionInfoHandle;
            DWORD dwDestinationVersionInfoSize;
            
            if ((dwDestinationVersionInfoSize = GetFileVersionInfoSize((LPSTR)(LPCSTR)csDestinationPath, &dwDestinationVersionInfoHandle)) != NULL)
            {
               // allocate buffer for source information
               if ((lpSourceVersionInfo = new far char[dwSourceVersionInfoSize]) == NULL)
               {
                  AfxThrowMemoryException();
               }
               
               // read source information
               if (GetFileVersionInfo((LPSTR)(LPCSTR)csSourcePath, dwSourceVersionInfoHandle, dwSourceVersionInfoSize, lpSourceVersionInfo))
               {
                  VS_FIXEDFILEINFO FAR *lpSourceInfo;
                  UINT uSourceInfoSize;
                  
                  // query source version information
                  if (VerQueryValue(lpSourceVersionInfo, "\\", (LPVOID FAR *)(&lpSourceInfo), &uSourceInfoSize))
                  {
                     // allocate buffer for destination information
                     if ((lpDestinationVersionInfo = new far char[dwDestinationVersionInfoSize]) == NULL)
                     {
                        AfxThrowMemoryException();
                     }
                     
                     // read destination information
                     if (GetFileVersionInfo((LPSTR)(LPCSTR)csDestinationPath, dwDestinationVersionInfoHandle, dwDestinationVersionInfoSize, lpDestinationVersionInfo))
                     {
                        VS_FIXEDFILEINFO FAR *lpDestinationInfo;
                        UINT uDestinationInfoSize;
                        
                        // query destination version information
                        if (VerQueryValue(lpDestinationVersionInfo, "\\", (LPVOID FAR *)(&lpDestinationInfo), &uDestinationInfoSize))
                        {
                           /*
                           // We have the version information for both files, make sure the
                           // one we are installing is newer than the one that is installed.
                           // Additional check: make sure the file OS is the same (16-bit vs. 32-bit)
                           // for our (program group) files.
                           */
                           
                           if ((pProgramFile->GetGroups() & CProgramFile::ProgramGroup) == 0 // Not us.
                               || lpSourceInfo->dwFileOS == lpDestinationInfo->dwFileOS) // Only if same OS.
                           {
                              if (lpSourceInfo->dwFileVersionMS < lpDestinationInfo->dwFileVersionMS)
                              {
                                 fIsOlderVersion = TRUE;
                              }
                              
                              else if (lpSourceInfo->dwFileVersionMS == lpDestinationInfo->dwFileVersionMS)
                              {
                                 fIsOlderVersion = lpSourceInfo->dwFileVersionLS < lpDestinationInfo->dwFileVersionLS;
                              }
                              
                              if ((lpSourceInfo->dwFileVersionMS == lpDestinationInfo->dwFileVersionMS)
                               && (lpSourceInfo->dwFileVersionLS == lpDestinationInfo->dwFileVersionLS))
                              {
                                 fIsSameVersion = TRUE;
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
      
      /*
      // If it is an older version, ask the user if they want to install it unless the
      // appropriate bit (CProgramFile::IgnoreIfOlder) is set in the program file flags.
      */
      
      if (fIsOlderVersion)
      {
         TRACE("New file is older than existing file.\n");
         
         // ignore the file instead of installing it
         fInstallFile = FALSE;
         
         if ((pProgramFile->GetFlags() & CProgramFile::IgnoreIfOlder) == 0)
         {
            if (Util::MessageBox(
                  MB_YESNO|MB_DEFBUTTON2,
                  (UINT)-1,
                  MAKEINTRESOURCE(IDS_InstallOlderFormat),
                  (LPCSTR)csDestinationFile,
                  (LPCSTR)csDestinationPath) == IDYES)
            {
               fInstallFile = TRUE;
            }
         }
      }
      
      /*
      // If the files are the same version, then do not install.
      */
      
      if (fIsSameVersion)
      {
         TRACE("New file is SAME version as existing file.\n");
         fInstallFile = FALSE;
      }

      /*
      // Intall the file if the fInstallFile flags is set. Otherwise, ignore the file
      // by simply adjusting the progress bar by its total size and finishing.
      //
      // We don't use VerInstallFile() because we cannot update the progress as
      // the file is copied.
      */
      
      if (!fInstallFile)
      {
         TRACE("Skip file |%s|\n", (LPCSTR)csSourcePath);
         if (pProgressDlg != NULL)
         {
            if (pProgressDlg->CheckForAbort())
            {
               AfxThrowUserException();
            }
            
            pProgressDlg->SetPos(pProgressDlg->GetPos()+pProgramFile->GetSize());
         }
      }
      
      else
      {
         /*
         // Make sure the directory exists.
         */
         
         if (!Util::MakeDirectory(csDestinationDirectory))
         {
            Util::MessageBox(
               MB_OK,
               (UINT)-1,
               MAKEINTRESOURCE(IDS_ErrCantCreateDirectory),
               (LPCSTR)csDestinationDirectory);
            AfxThrowUserException();
         }
         
         /*
         // Attempt to open the source file.
         */
         
         if (!cfSource.Open(csSourcePath, CFile::modeRead|CFile::shareDenyNone))
         {
            Util::MessageBox(
               MB_OK,
               (UINT)-1,
               MAKEINTRESOURCE(IDS_ErrCantOpenFile),
               (LPCSTR)csSourcePath);
            AfxThrowUserException();
         }
         
         /*
         // Attempt to create the destination file.
         */
         
	      if (!cfDestination.Open(csDestinationPath, CFile::modeCreate|CFile::modeWrite|CFile::shareExclusive))
         {
				if ((pProgramFile->GetFlags() & CProgramFile::IgnoreIfInUse) == 0)
				{
					Util::MessageBox(
										  MB_OK,
										  (UINT)-1,
										  MAKEINTRESOURCE(IDS_ErrCantCreateFile),
										  (LPCSTR)csDestinationPath);
				}
				fAbortOnError = FALSE;
            AfxThrowUserException();
         }
         
         /*
         // At this point, we have changed the destination file. If we are copying
         // a file in the OLE group, then set the "CallRegEdit" flag.
         */
         
         if ((pProgramFile->GetGroups() & CProgramFile::OleGroup) != 0)
         {
            m_fCallRegEdit = TRUE;
         }
         
			/*
			// Allocate a buffer for the copy.
			*/
			#define  BUFFER_SIZE (20*1024)
			DWORD dwBufSize = BUFFER_SIZE;
			BOOL bNeedsDecompress = FALSE;
			if(pProgramFile->GetCompression())
			{
				bNeedsDecompress = TRUE;
				dwBufSize = cfSource.GetLength();
			}
        
         if ((lpBuffer = new far BYTE[dwBufSize]) == NULL)
         {
            AfxThrowMemoryException();
         }
         
         UINT uNumberRead;
            
         do
         {
            if (pProgressDlg != NULL)
            {
               if (pProgressDlg->CheckForAbort())
               {
                  AfxThrowUserException();
               }
            }
               
            TRY
            {
               if ((uNumberRead = cfSource.Read(lpBuffer, dwBufSize)) > 0)
               {
						if(bNeedsDecompress && uNumberRead > 0)
						{
							BYTE*  pDecompressData;
							DWORD dwSize;
							CDeflate inflate;
							if(inflate.Decompress(lpBuffer, uNumberRead, &pDecompressData, &dwSize))
							{
								cfDestination.Write(pDecompressData, dwSize);
								inflate.FreeData(pDecompressData);
								uNumberRead = 0;
							}
							else
		                  AfxThrowUserException();

						}
						else
							cfDestination.Write(lpBuffer, uNumberRead);
                  
                  if (pProgressDlg != NULL)
                  {
                     pProgressDlg->SetPos(pProgressDlg->GetPos()+(DWORD)uNumberRead);
                  }
               }
            }
            CATCH(CFileException, e)
            {
               CString csCause;
            
               TRY
               {
                  LoadConfigurationString(GetFileExceptionCauseStringID(e), csCause);
                  Util::MessageBox(
                     MB_OK,
                     (UINT)-1,
                     MAKEINTRESOURCE(IDS_ErrCantCopyFileFormat),
                     (LPCSTR)csSourcePath,
                     (LPCSTR)csDestinationPath,
                     (LPCSTR)csCause);
               }
               CATCH_ALL(e)
               {
                  csCause.Empty();
                  
                  GetConfiguration()->MessageBox(IDS_ErrCantCopyFile, 0, MB_OK);
               }
               END_CATCH_ALL
               
					fAbortOnError = FALSE;
               AfxThrowUserException();
            }
            END_CATCH
         } while (uNumberRead == BUFFER_SIZE);

         // Close the source and destination files.

         cfSource.Close();
         cfDestination.Close();
         
         // Set the creation time for the file we are installing to that
         // of the source file.

			FILETIME CreateTime;
			FILETIME AccessTime;
			FILETIME ModifyTime;
			if (Util::GetFileTime(csSourcePath, &CreateTime, &AccessTime, &ModifyTime))
			{
				Util::SetFileTime(csDestinationPath, &CreateTime, &AccessTime, &ModifyTime);
			}
      }
   }
   CATCH_ALL(e)
   {
      TRY
      {
         if (cfSource.m_hFile != (UINT)CFile::hFileNull)
         {
            cfSource.Close();
         }
      }
      END_TRY

      TRY
      {                  
         if (cfDestination.m_hFile != (UINT)CFile::hFileNull)
         {
            cfDestination.Close();

				// Remove the partial file.
				ASSERT(!csDestinationPath.IsEmpty());
				remove(csDestinationPath);
         }
      }
      END_TRY
      
      if (pProgressDlg != NULL)
      {
         pProgressDlg->HideItem();
      }

      delete [] lpBuffer;
      delete [] lpSourceVersionInfo;
      delete [] lpDestinationVersionInfo;
      csSourcePath.Empty();
      csDestinationPath.Empty();
      csDestinationDirectory.Empty();
      csDestinationFile.Empty();
      csCurrentDirectory.Empty();
      csInstallDirectory.Empty();
      csWindowsDirectory.Empty();

		if (fAbortOnError)
		{
			THROW_LAST();
		}

		// And go back.
		return;
   }
   END_CATCH_ALL
   
   if (pProgressDlg != NULL)
   {
      pProgressDlg->HideItem();
   }
   
   delete [] lpBuffer;
   delete [] lpSourceVersionInfo;
   delete [] lpDestinationVersionInfo;

   TRY
   {  
      if (cfSource.m_hFile != (UINT)CFile::hFileNull)
      {
         cfSource.Close();
      }
   }
   END_TRY
   
   TRY
   {
      if (cfDestination.m_hFile != (UINT)CFile::hFileNull)
      {
         cfDestination.Close();
      }
   }
   END_TRY
}

/*
// Touch the directory of a file being installed.
// We remember the directory of the last file we installed. We need to
// touch all path components we have not touched so far. So we find the
// matching prefix of both (the part that is the same), and touch everything
// after that.
// "Touching" involves a complete directory search.
*/

void CInstallApp::TouchSourceDirectory(const CString& csFileName)
{
   CString csPath;
   CString csComponent;

   TRY
   {
      Util::SplitPath(csFileName, &csPath, NULL);
      csPath.MakeUpper();

      if (csPath != m_csLastSourceDirectory)
      {

         // Find the prefix of this path and the last source path.
         // That will determine how far we have descended so far.

         int nLength = csPath.GetLength();
         int nLastLength = m_csLastSourceDirectory.GetLength();
         int nSearchLength = (nLength < nLastLength) ? nLength : nLastLength;
         int nIndex;

         for (nIndex = 0; nIndex < nSearchLength; nIndex++)
         {
            if (csPath[nIndex] != m_csLastSourceDirectory[nIndex])
            {
               break;
            }
         }

         // Now, we have index of the first character that does not match.
         // Extract components and touch them.

         if ((nIndex > 0)
          && (nIndex != nLength)
          && (csPath[nIndex] == '\\')
          && (nIndex != nLastLength))
         {
         // Not at component end of last directory.
         // Backup to keep from moving off of this component.
            nIndex--;
         }

         while (nIndex < nLength)
         {
            // move to the next component

            nIndex++;

            while ((nIndex < nLength) && (csPath[nIndex] != '\\'))
            {
               nIndex++;
            }

            if (nIndex < nLength)
            {
               // Check if the current component exists. If not, create it.
                     
               csComponent = csPath.Left(nIndex);
                     
//             od("TouchSourceDirectory - Component |%s|\n\r", (LPCSTR)csComponent);
                     
#if 0
               CString csMessage;
               csMessage = "Touch Directory: ";
               csMessage += csComponent;
               AfxMessageBox(csMessage);
#endif

               TouchDirectory(csComponent);
            }
         }

         // Remember this source directory for next time.
         m_csLastSourceDirectory = csPath;
      }
   }
   END_TRY
}

void CInstallApp::TouchDirectory(const CString& csDirectory)
{
   CString csSearch;
   CFileIterator Iter;
   TRY
   {
      Util::ConstructPath(csSearch, csDirectory, "*.*");
      BOOL fRet;

#if 0
      CString csMessage;
      csMessage = "Find first: ";
      csMessage += csSearch;
      AfxMessageBox(csMessage);
#endif

      for (fRet = Iter.FindFirst(csSearch, _A_NORMAL | _A_RDONLY | _A_SUBDIR);
           fRet;
           fRet = Iter.FindNext())
      {
      // Nothing to do... so do nothing.
      }
      Iter.Close();
   }
   END_TRY
}

void CInstallApp::WriteIniEntries(CWnd *pcwndParent, CInstallProgressDialog *pProgressDlg)
{
   TRACE("Write Ini Entries\n");
   /*
   // Write out the entries for PMW to PMW.INI in the destination installation directory:
   //
   // \PMW\PMW.INI
   //    [Sound]
   //       SoundPath=[[H]]\PMW\SOUNDS
   //       SoundState=0|1|2 (SMS_... in soundmgr.h)
   //       SoundBits=8 (if not 16)
   //
   // \PMW\PMWWORK.INI
   //    [Sizes]
   //       PMW.WRK=<byte size of workspace>
   */
   
   CString csResource;
   CString csFile;
   CString csIniPath;
   CString csSection;
   CString csKey;
   CString csValue;
   
   TRY
   {
      if (pProgressDlg != NULL)
      {
         /*
         // Tell the user what's going on.
         */
         
         LoadConfigurationString(IDS_ConfiguringPrintMasterGold, csResource);
         pProgressDlg->ShowItem(csResource);
         
         if (pProgressDlg->CheckForAbort())
         {
            AfxThrowUserException();
         }
      }
      
      // \PMW\PMWWORK.INI:[Sizes]:PMW.WRK=<byte size of workspace>
      LoadConfigurationString(IDS_PmwIni, csFile);
      Util::ConstructPath(csIniPath, m_csInstallDirectory, csFile);

      LoadConfigurationString(IDS_WorkspaceSection, csSection);
      LoadConfigurationString(IDS_WorkspaceSizeEntry, csKey);

      _ltoa(m_dwWorkspaceSize, csValue.GetBuffer(48), 10);
      csValue.ReleaseBuffer();

      ::WritePrivateProfileString(csSection, csKey, csValue, csIniPath);

	  // If CDPath entry exists in MSREG.INI, expand it and save to PMW.INI 
      CString csCDPathEntry = "CDPath", csCDPath;
	  csCDPath = m_MsregIniFile.GetString(SECTION_Configuration, csCDPathEntry);
      Util::Trim(csCDPath);
	  if(!csCDPath.IsEmpty())
	  {
		// If application directory is specified, use install directory
		if(csCDPath.CompareNoCase(".") == 0)
		{
			// Write out CDPath entry so program can run without CD
			::WritePrivateProfileString(SECTION_Configuration, csCDPathEntry, m_csInstallDirectory,csIniPath);
		}
	  }
   }
   CATCH_ALL(e)
   {
      csResource.Empty();
      csFile.Empty();
      csIniPath.Empty();
      csSection.Empty();
      csKey.Empty();
      csValue.Empty();
      
      if (pProgressDlg != NULL)
      {
         pProgressDlg->HideItem();
      }
      
      THROW_LAST();
   }
   END_CATCH_ALL
   
   if (pProgressDlg != NULL)
   {
      pProgressDlg->HideItem();
   }
   TRACE("End Write Ini Entries\n");
}

void CInstallApp::MakeProgramManagerGroup(CInstallProgressDialog *pProgressDlg)
{
   TRACE("Make Program Manager Group\n");

   CString csPath;
   CString csFile;
   CString csArguments;
   CString csTitle;
   CString csMessage;
   CString csFormat;
   CString csGroupName;
   CString csIconPath;

#if 0
csMessage.Format("Install config = %d, current config = %d",
	m_InstallInfo.m_nInstallConfiguration,
	m_InstallInfo.m_nCurrentConfiguration);
AfxMessageBox(csMessage);
#endif

   TRY
   {
      LoadConfigurationString(IDS_ProgramManagerGroupName, csGroupName);

      if (pProgressDlg != NULL)
      {
         /*
         // Tell the user what's going on.
         */
         
         LoadConfigurationString(IDS_BuildingProgramManagerGroup, csFormat);
         wsprintf(csMessage.GetBuffer(1024), csFormat, (LPCSTR) csGroupName);
         csMessage.ReleaseBuffer();
         pProgressDlg->ShowItem(csMessage);
			pProgressDlg->HideProgress();

         if (pProgressDlg->CheckForAbort())
         {
            AfxThrowUserException();
         }
      }

      /*
      // Remove any old PrintMaster groups.
      */

      HandleOldGroups();

      /*
      // Open up the program manager group.
      */

      if (!pmOpen(csGroupName, TRUE))
      {
         // could not open the program manager group
         Util::MessageBox(
            MB_OK,
            (UINT)-1,
            MAKEINTRESOURCE(IDS_ErrCantOpenProgramManagerGroup),
            MAKEINTRESOURCE(IDS_ProgramManagerGroupName));
         AfxThrowUserException();
      }

      /*
      // Add the main program icon to the group.
      */
		CString csIcon = m_MsregIniFile.GetString(SECTION_Configuration, "icon", "pmg.ico");

      AddGroupItem(
         MAKEINTRESOURCE(IDS_ProgramTitle),
         m_csInstallDirectory,
         GetExeName(),
         NULL,
         m_csInstallDirectory,
         csIcon,
         0);

#ifdef AG_BUILD
		if (m_InstallInfo.m_nCurrentConfiguration >= CPrintMasterConfiguration::PrintStandard)
#else
		if (m_InstallInfo.m_nCurrentConfiguration >= CPrintMasterConfiguration::PublishingSuite)
#endif
		{
			/*
			// Add the Event Reminder program icon to the group.
			*/

			AddGroupItem(
				MAKEINTRESOURCE(IDS_EventReminderTitle),
				m_csInstallDirectory,
				MAKEINTRESOURCE(IDS_RemindExe),
				NULL,
				NULL,
				NULL,
				0);
		}

      /*
      // Add the uninstall/remove program icon to the group.
      */

      AddGroupItem(
         MAKEINTRESOURCE(IDS_UninstallTitle),
         m_csInstallDirectory,
         MAKEINTRESOURCE(m_fWorkstationInstall ? IDS_WorkstationUninstallName : IDS_UninstallName),
         MAKEINTRESOURCE(IDS_UninstallNameArguments),
         m_csInstallDirectory,
         NULL,
         0);

      /*
      // Add the User Guide icon to the group.
      */

#if 0
      AddGroupItem(
         MAKEINTRESOURCE(IDS_GuideTitle),
         m_csInstallDirectory,
         MAKEINTRESOURCE(IDS_GuideExe),
         MAKEINTRESOURCE(IDS_GuideExeArguments32),
         m_csInstallDirectory,
         "GUIDE.ICO",
         0);
#endif

#if 0
      /*
      // Add the Windows 95 help icon to the group.
      */

      AddGroupItem(
         MAKEINTRESOURCE(IDS_Win95HelpTitle),
         m_csInstallDirectory,
         MAKEINTRESOURCE(IDS_Win95HelpExe),
         NULL,
         NULL,
         NULL,
         0);
#endif

      /*
      // Add the Pmw URL icon to the group.
      */
      int   nUrlTitleID = IDS_PmwUrlTitle;

		csIcon = m_MsregIniFile.GetString(SECTION_Configuration, "UrlIcon", "pmwurl.ico");
      if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
         nUrlTitleID = IDS_AGUrlTitle;
      AddGroupItem(
         MAKEINTRESOURCE(nUrlTitleID),
         m_csInstallDirectory,
         MAKEINTRESOURCE(IDS_PmwUrlExe),
         MAKEINTRESOURCE(IDS_PmwUrlExeArguments),
         m_csInstallDirectory,
         csIcon,
         0);

      /*
      // Add the Product Catalog icon to the group.
      */

#if 0
      AddGroupItem(
         MAKEINTRESOURCE(IDS_ProductCatalogTitle),
         m_csInstallDirectory,
         MAKEINTRESOURCE(IDS_ProductCatalogExe),
         MAKEINTRESOURCE(IDS_ProductCatalogExeArguments),
         m_csInstallDirectory,
         "PRODCAT.ICO",
         0);
#endif

      /*
      // Add the TWAIN installation program to the group.
      */

      AddGroupItem(
         MAKEINTRESOURCE(IDS_TwainInstallTitle),
         m_csInstallDirectory,
         MAKEINTRESOURCE(IDS_TwainInstallExe),
         MAKEINTRESOURCE(IDS_TwainInstallExeArguments),
         m_csInstallDirectory,
         "TWAIN.ICO",
         0);

		/*
      // Add the ISP installation program to the group.
		// if required
      */

		if (!GetConfiguration()->RemoveISP())
		{
			AddGroupItem(
							 MAKEINTRESOURCE(IDS_IspInstallTitle),
							 m_csInstallDirectory,
							 MAKEINTRESOURCE(IDS_IspInstallExe),
							 MAKEINTRESOURCE(IDS_IspInstallExeArguments),
							 m_csInstallDirectory,
							 "ATT.ICO",
							 0);
		}

		/*
      // Add the Web Publishing Wizard installation program to the group.
      */

		AddGroupItem(
						 MAKEINTRESOURCE(IDS_WebPublishingWizardInstallTitle),
						 m_csInstallDirectory,
						 MAKEINTRESOURCE(IDS_WebPublishingWizardInstallExe),
						 MAKEINTRESOURCE(IDS_WebPublishingWizardInstallArguments),
						 m_csInstallDirectory,
						 NULL,
						 0);
	}
   CATCH_ALL(e)
   {
      csPath.Empty();
      csFile.Empty();
      csTitle.Empty();
      csMessage.Empty();
      csFormat.Empty();
      csGroupName.Empty();
      csIconPath.Empty();

      if (!csGroupName.IsEmpty())
      {
         pmClose(csGroupName);
         csGroupName.Empty();
      }
      
      if (pProgressDlg != NULL)
      {
         pProgressDlg->HideItem();
      }

      THROW_LAST();
   }
   END_CATCH_ALL
      
   if (!csGroupName.IsEmpty())
   {
      pmClose(csGroupName);
   }
   
   if (pProgressDlg != NULL)
   {
      pProgressDlg->HideItem();
   }

#ifdef AG_BUILD
	if (m_InstallInfo.m_nCurrentConfiguration >= CPrintMasterConfiguration::PrintStandard)
#else
   if (m_InstallInfo.m_nCurrentConfiguration >= CPrintMasterConfiguration::PublishingSuite)
#endif
	{
		// Add the event reminder to the startup group.
		CString csItems;
		CString csItem;
		CString csItemName;
		CString csItemMatchName;
		TRY
		{
			LoadConfigurationString(IDS_StartupGroupName, csGroupName);
			LoadConfigurationString(IDS_EventReminderTitle, csItemMatchName);

			// Get the data for the startup group.
			if (pmGetGroupData(csItems, csGroupName))
			{
				if (pmOpen(csGroupName, FALSE))
				{
					if (GetNextLine(csItems, csItem))
					{
						while (GetNextLine(csItems, csItem))
						{
							// Got a line, get the program name - field 0 (zero based).
							if (GetField(csItem, csItemName, ',', 0))
							{
								if (csItemName.CompareNoCase(csItemMatchName) == 0)
								{
									TRACE("DELETE |%s|!\n", (LPCSTR)csItemName);
									pmDeleteItem(csItemName);
								}
							}
						}
					}

					// Add the Event Reminder program icon to the group.
					AddGroupItem(
						MAKEINTRESOURCE(IDS_EventReminderTitle),
						m_csInstallDirectory,
						MAKEINTRESOURCE(IDS_RemindExe),
						MAKEINTRESOURCE(IDS_RemindExeQuietArguments),
						m_csInstallDirectory,
						NULL,
						0);

					pmClose(csGroupName);
   			}
			}
		}
		END_TRY;
	}

   TRACE("End Make Program Manager Group\n");
}

void CInstallApp::UpdateRegistry(void)
{
   HKEY hKey = NULL;
   DWORD dwDisposition;
   CString csKey;
   CString csValue;
   CString csPath;
   CString csUninstall;
   CString csUninstallArguments;
   
   TRY
   {
      // Create a place for ourselves in the registry:
      //
      //    HKEY_LOCAL_MACHINE\SOFTWARE\MicroLogic Software\PrintMaster Gold\x.xx

      csKey = "SOFTWARE\\";
      csKey += m_csRegistryCompany;
      csKey += '\\';
      csKey += m_csRegistryName;
      csKey += '\\';
      csKey += m_csRegistryVersion;

      if (RegCreateKeyEx(
               HKEY_LOCAL_MACHINE,
               csKey,
               0,
               "",
               REG_OPTION_NON_VOLATILE,
               KEY_ALL_ACCESS,
               NULL,
               &hKey,
               &dwDisposition) == ERROR_SUCCESS)
      {
         RegCloseKey(hKey);
         hKey = NULL;
      }

      // Register the paths for our application.
      //
      //    HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\PMW.EXE
      //       (Default)=<full path of program EXE file>
      //       Path=<full program directory>

      if (RegCreateKeyEx(
               HKEY_LOCAL_MACHINE,
               GetAppPathKey(),
               0,
               "",
               REG_OPTION_NON_VOLATILE,
               KEY_ALL_ACCESS,
               NULL,
               &hKey,
               &dwDisposition) == ERROR_SUCCESS)
      {
			CString csExeName = GetExeName();			
         Util::ConstructPath(csValue, m_csInstallDirectory, csExeName);

         RegSetValueEx(
               hKey,
               NULL,
               0,
               REG_SZ,
               (const unsigned char *)(LPCSTR)csValue,
               csValue.GetLength()+1);

         csValue = m_csInstallDirectory;
         Util::RemoveBackslashFromPath(csValue);

         RegSetValueEx(
               hKey,
               "Path",
               0,
               REG_SZ,
               (const unsigned char *)(LPCSTR)csValue,
               csValue.GetLength()+1);

         RegCloseKey(hKey);
         hKey = NULL;
      }

      // Register the uninstall information for our application.
      //
      //    HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\PrintMaster Gold x.xx
      //       DisplayName=<name that shows up in Windows uninstall list>
      //       UninstallString=<full command to run uninstall program>

      if (RegCreateKeyEx(
               HKEY_LOCAL_MACHINE,
               GetUninstallKey(),
               0,
               "",
               REG_OPTION_NON_VOLATILE,
               KEY_ALL_ACCESS,
               NULL,
               &hKey,
               &dwDisposition) == ERROR_SUCCESS)
      {
         csValue = m_csRegistryName;
         csValue += ' ';
         csValue += m_csRegistryVersion;

         RegSetValueEx(
               hKey,
               "DisplayName",
               0,
               REG_SZ,
               (const unsigned char *)(LPCSTR)csValue,
               csValue.GetLength()+1);

         LoadConfigurationString(IDS_UninstallName, csUninstall);
         Util::ConstructPath(csValue, m_csInstallDirectory, csUninstall);

         GetShortName(csValue, csUninstall);

         LoadConfigurationString(IDS_UninstallNameArguments, csUninstallArguments);
         if (!csUninstallArguments.IsEmpty())
         {
            csUninstall += ' ';
            csUninstall += csUninstallArguments;
         }

         RegSetValueEx(
               hKey,
               "UninstallString",
               0,
               REG_SZ,
               (const unsigned char *)(LPCSTR)csUninstall,
               csValue.GetLength()+1);

         RegCloseKey(hKey);
         hKey = NULL;
      }

      // Increment the reference counts of any shared DLLs.

      for (int i = 0; i < csaSharedFiles.GetSize(); i++)
      {
         IncrementSharedDllReferenceCount(csaSharedFiles[i]);
      }
   }
   CATCH_ALL(e)
   {
      csKey.Empty();
      csValue.Empty();
      csUninstall.Empty();

      if (hKey != NULL)
      {
         RegCloseKey(hKey);
         hKey = NULL;
      }

      THROW_LAST();
   }
   END_CATCH_ALL
}

void CInstallApp::ConvertOldAddressBooks(CInstallProgressDialog *pProgressDlg)
{
   TRACE("Start Convert Old Address Books\n");

   if (OldAddressBooksExist())
   {
      // Convert any old address books to the new format.
      if (pProgressDlg != NULL)
      {
         CString csMessage;
         TRY
         {
            LoadConfigurationString(IDS_LookingForOldAddressBooks, csMessage);
         }
         END_TRY
         pProgressDlg->ShowItem(csMessage);
         pProgressDlg->EnableWindow(FALSE);
         pProgressDlg->BeginWaitCursor();
      }

      CString csCommandLine;
      CString csOldDirectory;
      CString csNewDirectory;
      TRY
      {
         csCommandLine = GetSourceDirectory();
         csCommandLine += "INSTALL\\DBCONVRT.EXE";

         // Get the "old" directory.
         if (!m_csPreviousInstallDirectory.IsEmpty())
         {
            csOldDirectory = m_csPreviousInstallDirectory;
         }
         else
         {
            csOldDirectory = m_csInstallDirectory;
         }

         // Get the "new" directory.
         csNewDirectory = m_csInstallDirectory;

         // Make the command line.
         csCommandLine += " /O=\"";
         csCommandLine += csOldDirectory;
         csCommandLine += "\" /N=\"";
         csCommandLine += csNewDirectory;
         csCommandLine += "\"";

         // Run the conversion program.
         DoWinExec(csCommandLine, "PmwConvertOldAddressBooksFinished");
      }
      END_TRY

      if (pProgressDlg != NULL)
      {
         pProgressDlg->EndWaitCursor();
         pProgressDlg->HideItem();
         pProgressDlg->EnableWindow(TRUE);
      }
   }

   TRACE("End Convert Old Address Books\n");
}

BOOL CInstallApp::OldAddressBooksExist(void)
{
   BOOL fResult = FALSE;

   CString csOldDirectory;
   CString csOldAddressBookPath;
   CString csOldSenderBookPath;
   CFileIterator FileIterator;
   CString csSearch;
   TRY
   {
      // Compute the base directory of the old installation.
      if (!m_csPreviousInstallDirectory.IsEmpty())
      {
         csOldDirectory = m_csPreviousInstallDirectory;
      }
      else
      {
         csOldDirectory = m_csInstallDirectory;
      }

      // Compute the full address book and sender book path names for the old installation.
      csOldAddressBookPath = csOldDirectory;
      Util::AppendBackslashToPath(csOldAddressBookPath);
      csOldAddressBookPath += "BOOKS\\";
      csOldSenderBookPath = csOldAddressBookPath;
      csOldSenderBookPath += "SENDER\\";
      
      // Look for old address books.
      csSearch = csOldAddressBookPath+"*.DB";
      if (FileIterator.FindFirst(csSearch, _A_NORMAL))
      {
         fResult = TRUE;
      }
      else
      {
         // Look for old sender books.
         csSearch = csOldSenderBookPath+"*.DB";
         if (FileIterator.FindFirst(csSearch, _A_NORMAL))
         {
            fResult = TRUE;
         }
      }
   }
   END_TRY

   return fResult;
}

void CInstallApp::HandleOldGroups(void)
{
   TRACE("Handle old Groups\n");
   CString csGroups;
   CString csGroup;

   TRY
   {
		CString csCompare("PrintMaster");
		if(GetConfiguration()->Product() == CPrintMasterConfiguration::plAmericanGreetings)
			csCompare = "American Greetings";
      // Get the group names.
      if (pmGetGroupData(csGroups))
      {
         while (GetNextLine(csGroups, csGroup))
         {
            if (csGroup.Find(csCompare) >= 0)
            {
               HandleOldGroup(csGroup);
            }
         }
      }
   }
   END_TRY
}

void CInstallApp::HandleOldGroup(LPCSTR pszGroupName)
{
   TRACE("Handle old Group |%s|\n", pszGroupName);
   CString csItems;
   CString csItem;
   CString csName;
   CString csFileName;
   CString csPath;
   CString csShortInstallDirectory;
   TRY
   {
      GetShortName(m_csInstallDirectory, csShortInstallDirectory);

      if (pmGetGroupData(csItems, pszGroupName))
      {
         // Check if there are some items to delete. We check this
         // without deleting anything so we don't bring groups to
         // the front which are not going to be affected. We also check
         // if all the items are going to be deleted.

         BOOL fDeletedAllItems = TRUE;
         BOOL fDeletedSomeItems = FALSE;

         // Skip past the first line... it's the group itself.
         if (GetNextLine(csItems, csItem))
         {
            while (GetNextLine(csItems, csItem))
            {
               // Got a line, get the program path - field 1 (zero based).
               if (GetField(csItem, csFileName, ',', 1))
               {
                  TRY
                  {
                     Util::SplitPath(csFileName, &csPath, NULL);
                     Util::RemoveBackslashFromPath(csPath);
                     TRACE("Path for |%s| is |%s|\n", (LPCSTR)csFileName, (LPCSTR)csPath);
                     if (csPath.CompareNoCase(csShortInstallDirectory) == 0)
                     {
                        fDeletedSomeItems = TRUE;
                     }
                     else
                     {
                        fDeletedAllItems = FALSE;
                     }
                  }
                  CATCH_ALL(e)
                  {
                     fDeletedAllItems = FALSE;
                  }
                  END_CATCH_ALL
               }
            }

            if (fDeletedSomeItems)
            {
               if (fDeletedAllItems)
               {
                  // Everything goes, delete the entire enchilada.
                  TRACE("Delete the entire group |%s|\n", pszGroupName);
                  pmDeleteGroup(pszGroupName);
               }
               else
               {
                  // Trudge through the names and delete individual items.
                  // Note that this will bring the group to the top.
                  if (pmGetGroupData(csItems, pszGroupName))
                  {
                     if (pmOpen(pszGroupName, FALSE))
                     {
                        if (GetNextLine(csItems, csItem))
                        {
                           while (GetNextLine(csItems, csItem))
                           {
                              // Got a line, get the program path - field 1 (zero based).
                              if (GetField(csItem, csFileName, ',', 1))
                              {
                                 TRY
                                 {
                                    Util::SplitPath(csFileName, &csPath, NULL);
                                    Util::RemoveBackslashFromPath(csPath);
                                    TRACE("Path for |%s| is |%s|\n", (LPCSTR)csFileName, (LPCSTR)csPath);
                                    if (csPath.CompareNoCase(csShortInstallDirectory) == 0)
                                    {
                                       if (GetField(csItem, csName, ',', 0))
                                       {
                                          TRACE("DELETE |%s|!\n", (LPCSTR)csName);
                                          pmDeleteItem(csName);
                                       }
                                    }
                                 }
                                 END_TRY
                              }
                           }
                        }

                        pmClose(pszGroupName);
                     }
                  }
               }
            }
         }
      }
   }
   END_TRY
}

void CInstallApp::InstallIsp(CWnd *pcwndParent)
{
   if (GetConfiguration()->RemoveISP())
	   m_fInstallIsp = FALSE;
   else
   {
	// Ask the user if they want to install the ISP package.
	   CIspDialog IspDlg(pcwndParent);
	   m_fInstallIsp = (IspDlg.DoModal() == IDOK);
   }
}

/////////////////////////////////////////////////////////////////////////////
// Utility functions

UINT GetFileExceptionCauseStringID(CFileException *e)
{
   UINT uID = 0;
      
   switch (e->m_cause)
   {
      case CFileException::fileNotFound:
      {
         uID = IDS_ErrFileExceptionFileNotFound;
         break;
      }

      case CFileException::badPath:
      {
         uID = IDS_ErrFileExceptionBadPath;
         break;
      }

      case CFileException::tooManyOpenFiles:
      {
         uID = IDS_ErrFileExceptionTooManyOpenFiles;
         break;
      }

      case CFileException::accessDenied:
      {
         uID = IDS_ErrFileExceptionAccessDenied;
         break;
      }

      case CFileException::invalidFile:
      {
         uID = IDS_ErrFileExceptionInvalidFile;
         break;
      }

      case CFileException::removeCurrentDir:
      {
         uID = IDS_ErrFileExceptionRemoveCurrentDirectory;
         break;
      }

      case CFileException::directoryFull:
      {
         uID = IDS_ErrFileExceptionDirectoryFull;
         break;
      }

      case CFileException::badSeek:
      {
         uID = IDS_ErrFileExceptionSeekError;
         break;
      }

      case CFileException::hardIO:
      {
         uID = IDS_ErrFileExceptionHardwareIO;
         break;
      }

      case CFileException::sharingViolation:
      {
         uID = IDS_ErrFileExceptionSharingViolation;
         break;
      }

      case CFileException::lockViolation:
      {
         uID = IDS_ErrFileExceptionLockViolation;
         break;
      }

      case CFileException::diskFull:
      {
         uID = IDS_ErrFileExceptionDiskFull;
         break;
      }

      case CFileException::endOfFile:
      {
         uID = IDS_ErrFileExceptionEndOfFile;
         break;
      }
         
      default:
      {
         uID = IDS_ErrFileExceptionUnknown;
         break;
      }
   }
   
   return uID;
}
      
DWORD CInstallApp::GetProgramSize(const CString &csDirectory)
{
   DWORD dwResult = DISK_SPACE_NOT_AVAILABLE;
   
   /*
   // Get the available disk space and cluster size of the installation drive.
   // In some cases, (e.g., \\SERVER\SHARE), these numbers may not be available.
   */
   
   #define DEFAULT_CLUSTER_SIZE ((DWORD)16384L)
   
   DWORD dwClusterSize;
   
   if ((dwClusterSize = Util::GetClusterSize(csDirectory)) == CLUSTER_SIZE_NOT_AVAILABLE)
   {
      dwClusterSize = DEFAULT_CLUSTER_SIZE;
   }
   
   /*
   // Compute the disk size of the PrintMaster program files.
   */
   
   TRY
   {
      dwResult = GetProgramFileList().GetFileSize(CProgramFile::ProgramGroup, dwClusterSize);
      
      #define  PADDING_CLUSTERS  (10)
      
      // add in some padding
      
      dwResult += dwClusterSize*PADDING_CLUSTERS;
   }
   CATCH_ALL(e)
   {
      dwResult = DISK_SPACE_NOT_AVAILABLE;
   }
   END_CATCH_ALL
   
   return dwResult;
}

DWORD CInstallApp::GetRecommendedWorkspaceSize(const CString &csDirectory)
{
   /*
   // Get the recommended size for the workspace as follows:
   //
   // 1) If the installation directory is on a drive for which
   //    we cannot determine the free disk space, use the default
   //    size of 9 Mb.
   //
   // 2) Otherwise, use half of the free disk space after subtracting
   //    out the program size. This value is clipped to a minimum of
   //    1 MB and a maximum of 9 Mb. The value is rounded to the
   //    nearest .5 Mb.
   */
   
   DWORD dwResult = DEFAULT_WORKSPACE_SIZE;

   // get the available free disk space
   DWORD dwAvailableSize = Util::GetAvailableDiskSpace(csDirectory);
   
   if (dwAvailableSize != DISK_SPACE_NOT_AVAILABLE)
   {
      // get the space needed for the program
      DWORD dwProgramSize = GetProgramSize(csDirectory);
      
      if (dwProgramSize == DISK_SPACE_NOT_AVAILABLE)
      {
         dwResult = DISK_SPACE_NOT_AVAILABLE;
      }
      
      else
      {
         // check if minimum should be used
         if (dwProgramSize >= dwAvailableSize)
         {
            dwResult = MINIMUM_WORKSPACE_SIZE;
         }
         
         else
         {
            // use half of the available space
            dwAvailableSize -= dwProgramSize;
            dwAvailableSize /= 2;
            
            // clip to extremes
            if (dwAvailableSize < MINIMUM_WORKSPACE_SIZE)
            {
               dwAvailableSize = MINIMUM_WORKSPACE_SIZE;
            }
            
            if (dwAvailableSize > DEFAULT_WORKSPACE_SIZE)
            {
               dwAvailableSize = DEFAULT_WORKSPACE_SIZE;
            }
            
            // round to the nearest 1/2 megabyte
            dwResult = ((dwAvailableSize+250000L)/500000L)*500000L;
         }
      }
   }
   
   return dwResult;
}

const CString& CInstallApp::GetSourceDirectory(void)
{
   /*
   // The source directory is always returned with a trailing backslash.
   */

   /*
   // If we don't already have the source directory, get it.
   */

   if (m_csSourceDirectory.IsEmpty())
   {
      CString csPath;
      CString csFile;

      /*
      // Get the source directory for the install. We assume that INSTALL.EXE
      // is being run either from the INSTALL directory under the root of the
		// product directory tree on CD or under the root itself.
      */

      TRY
      {
         int nLength = ::GetModuleFileName(AfxGetInstanceHandle(), m_csProgramName.GetBuffer(_MAX_PATH), _MAX_PATH-1);
			m_csProgramName.ReleaseBuffer();

         if (nLength == 0)
         {
            // ::GetModuleFileName failed, force string to empty
				m_csProgramName.Empty();
         }
         else
         {
				m_csSourceDirectory = m_csProgramName;

				nLength = m_csSourceDirectory.GetLength();  // reget value to be DBCS safe

            // strip off module name to get directory
            while (--nLength >= 0)
            {
					if (m_csSourceDirectory[nLength] == '\\')
               {
                  break;
               }
            }

            if (nLength < 0)
            {
               m_csSourceDirectory.Empty();
            }

            else
            {
               m_csSourceDirectory = m_csSourceDirectory.Left(nLength+1);
            }

         /*
         // We are either in an INSTALL directory or we are not.
         // If we are not in the INSTALL directory, we will assume that we
         // are being run from the root of the installation tree.
         // Otherwise, we assume that we are one subdirectory down from the
         // root, and we will remove the INSTALL component to get to the root.
         //
         // We can either strip off the last subdirectory or append a "..".
         // Even though it's a little more complicated, I choose the former
         // approach since it produces better looking (simpler) filenames.
         */

            Util::RemoveBackslashFromPath(m_csSourceDirectory);
            Util::SplitPath(m_csSourceDirectory, &csPath, &csFile);
            if (csFile.CompareNoCase("INSTALL") == 0)
            {
            /*
            // The final directory component is INSTALL.
            // We want to remove it.
            */
               m_csSourceDirectory = csPath;
            }
            else
            {
            /*
            // Leave the source directory alone.
            */
            }
         }
      }
      CATCH_ALL(e)
      {
         m_csSourceDirectory.Empty();

         THROW_LAST();
      }
      END_CATCH_ALL

      // make sure we have a source directory
      if (m_csSourceDirectory.IsEmpty())
      {
         // report to the user
         AfxMessageBox(IDS_ErrCannotGetSourceDirectory);
         AfxThrowUserException();
      }

#ifdef SPECIAL
   m_csSourceDirectory = "Q:\\";
#endif
   }

   Util::AppendBackslashToPath(m_csSourceDirectory);
   return m_csSourceDirectory;
}

const CProgramFileList& CInstallApp::GetProgramFileList(void)
{
   /*
   // If we don't already have the source directory, get it.
   */

   if (m_ProgramFileList.GetSize() == 0)
   {
      /*
      // Read the program file list (INSTALL.PFL) from the root of the
      // source directory.
      */

      CString csResource;
      CString csProgramFileList;

      TRY
      {
         csProgramFileList = GetSourceDirectory();
         csProgramFileList += "install\\install.pfl";

         if (!m_ProgramFileList.Init(csProgramFileList))
         {
            m_ProgramFileList.DeleteAll();
         }
      }
      CATCH_ALL(e)
      {
         csProgramFileList.Empty();
         THROW_LAST();
      }
      END_CATCH_ALL
      
      // make sure we have a program file list
      if (m_ProgramFileList.GetSize() == 0)
      {
         // report to user
         csResource.Empty();
         csProgramFileList.Empty();
         GetConfiguration()->MessageBox(IDS_ErrCannotReadProgramFileList, 0, MB_OK);
         AfxThrowUserException();
      }
   }
   
   return m_ProgramFileList;
}

struct InstallationInfo &GetUserInformation(CWnd *pcwndParent)
{
   static struct InstallationInfo UserInformation;
   static BOOL fInitialized = FALSE;
   
   if (!fInitialized)
   {
      CString csSerialNumber;
      CString csUserName;
      CString csCompanyName;
      
      TRY
      {
         // copy registration information into user structure
            
         csSerialNumber = "PMW20-59YFD-3DE5RD";
         csUserName = "";
         csCompanyName = "";
         
         lstrcpyn(UserInformation.szSerialNumber, csSerialNumber, sizeof(UserInformation.szSerialNumber));
         UserInformation.szSerialNumber[sizeof(UserInformation.szSerialNumber)-1] = '\0';
         
         lstrcpyn(UserInformation.szUserName, csUserName, sizeof(UserInformation.szUserName));
         UserInformation.szUserName[sizeof(UserInformation.szUserName)-1] = '\0';
         
         lstrcpyn(UserInformation.szCompanyName, csCompanyName, sizeof(UserInformation.szCompanyName));
         UserInformation.szCompanyName[sizeof(UserInformation.szCompanyName)-1] = '\0';
            
         // put the time into the time stamp
#if 0
         CTime CurrentTime = CTime::GetCurrentTime();
         WORD wHourMinute = (CurrentTime.GetHour() << 8) | (CurrentTime.GetMinute() << 0) | 0x0101;
         WORD wSecondHundredth = (CurrentTime.GetSecond() << 8) | 0x0101;
         *((WORD*)(&(UserInformation.bTimeStamp[0]))) = wHourMinute;
         *((WORD*)(&(UserInformation.bTimeStamp[2]))) = wSecondHundredth;
#else
         DWORD dwTicks = GetTickCount();
         *((WORD*)(&(UserInformation.bTimeStamp[0]))) = LOWORD(dwTicks);
         *((WORD*)(&(UserInformation.bTimeStamp[2]))) = HIWORD(dwTicks);
#endif
            
         // initialize license count to zero (single user)
            
         UserInformation.bNetCount = 0;
            
         fInitialized = TRUE;
      }
      END_TRY
   }
   
   if (!fInitialized)
   {
      AfxThrowUserException();
   }
   
   return UserInformation;
}

DWORD IncrementSharedDllReferenceCount(LPCSTR pszName)
{
   TRACE("Increment reference count of %s\n", pszName);
   CString csKey;
   DWORD dwCount = 0;
   HKEY hKey = NULL;
   DWORD dwDisposition = 0;

   TRY
   {
      csKey = REGSTR_PATH_SETUP "\\SharedDLLs";

      if (RegCreateKeyEx(
               HKEY_LOCAL_MACHINE,
               csKey,
               0,
               "",
               REG_OPTION_NON_VOLATILE,
               KEY_ALL_ACCESS,
               NULL,
               &hKey,
               &dwDisposition) == ERROR_SUCCESS)
      {
         DWORD dwType;
         DWORD dwSize;

         dwCount = 0;
         dwSize = sizeof(dwCount);

         LONG lResult = RegQueryValueEx(
                              hKey,
                              pszName,
                              0,
                              &dwType,
                              (BYTE*)&dwCount,
                              &dwSize);

         BOOL fWriteNewCount = TRUE;

         if (lResult == ERROR_SUCCESS)
         {
            fWriteNewCount = FALSE;

            if ((dwType == REG_BINARY) || (dwType == REG_DWORD))
            {
               if (dwSize == sizeof(dwCount))
               {
                  fWriteNewCount = TRUE;
               }
            }
         }
         else
         {
            dwType = REG_DWORD;
            dwSize = sizeof(dwCount);
         }

         if (fWriteNewCount)
         {
            dwCount++;

            RegSetValueEx(
                  hKey,
                  pszName,
                  0,
                  dwType,
                  (const BYTE *)&dwCount,
                  dwSize);
         }

         RegCloseKey(hKey);
         hKey = NULL;
      }
   }
   END_TRY

   TRACE("Final count is %ld\n", dwCount);

   return dwCount;
}

void CInstallApp::SetOleInUse(void)
{
/*
// By default, it's not in use.
*/

   m_fOleInUse = FALSE;

/*
// Loop through each of the files in the program file list and copy them to the
// destination directory.
*/

   const CProgramFileList &ProgramFileList = GetProgramFileList();

   int nIndex;

   for (nIndex = 0; nIndex < ProgramFileList.GetSize(); nIndex++)
   {
      const CProgramFile *pProgramFile;

      if ((pProgramFile = (CProgramFile *)ProgramFileList.GetAt(nIndex)) != NULL)
      {
         if ((pProgramFile->GetGroups() & CProgramFile::OleGroup) != 0)
         {
            if (FileInUse(pProgramFile))
            {
               m_fOleInUse = TRUE;
               break;
            }
         }
      }
   }
}

BOOL CInstallApp::FileInUse(const CProgramFile *pProgramFile)
{
   BOOL fInUse = FALSE;

   /*
   // Extract the various paths and file names.
   */
   
   CString csApplicationDirectory;
   CString csSourcePath;
   CString csDestinationPath;
   CString csDestinationDirectory;
   CString csDestinationFile;
   CString csCurrentDirectory;
   CString csInstallDirectory;
   CString csWindowsDirectory;
   CFile cfFile;
   LPSTR lpBuffer = NULL;
   LPSTR lpSourceVersionInfo = NULL;
   LPSTR lpDestinationVersionInfo = NULL;

   TRY
   {
      /*
      // Get a copy of the installation directory without a backslash
      // to pass to VerFindFile().
      */

      csApplicationDirectory = m_csInstallDirectory;
      Util::RemoveBackslashFromPath(csApplicationDirectory);

      /*
      // Figure out where the file is coming from and where it is going.
      */
      
      Util::ConstructPath(csSourcePath, GetSourceDirectory(), pProgramFile->GetSource());
      
      /*
      // Check for special cases where we want to override destination
      // directory specified in INSTALL.PFL:
      //
      //    OLE Group (16):                     use OLE destination directory (16 bit only)
      //    Otherwise:                          use installation directory
      */
      
      // If csDestinationFile is empty at the end of all this, then the default value is used.
      csDestinationFile.Empty();
   
      if ((pProgramFile->GetGroups() & CProgramFile::OleGroup) != 0)
      {
         Util::GetSystemDirectory(csDestinationDirectory);
      }
      else
      {
         csDestinationDirectory = m_csInstallDirectory;
      }
      
      if (csDestinationFile.IsEmpty())
      {
         csDestinationFile = pProgramFile->GetDestination();
      }

      // This code is necessary since the file name may actually have path
      // components at this point.     
      Util::ConstructPath(csDestinationPath, csDestinationDirectory, csDestinationFile);
      Util::SplitPath(csDestinationPath, &csDestinationDirectory, &csDestinationFile);
      Util::RemoveBackslashFromPath(csDestinationDirectory);
      
      TRACE("File in use: |%s| to |%s|\n", (LPCSTR)csSourcePath, (LPCSTR)csDestinationPath);

      // This flag is used to tell if we should actually install the file.

      BOOL fInstallFile = TRUE;

      if (fInstallFile)
      {
         /*
         // Get the directory into which the file should be installed
         // using VerFindFile().
         */

         UINT uCurrentLength = _MAX_PATH+1;
         UINT uInstallLength = _MAX_PATH+1;
         UINT uResult;

         Util::GetWindowsDirectory(csWindowsDirectory);

         TRACE("Call VerFindFile\n");
         uResult = VerFindFile(
            ((pProgramFile->GetFlags() & CProgramFile::Shared) != 0) ? VFFF_ISSHAREDFILE : 0,
            (LPSTR)(LPCSTR)csDestinationFile,
            (LPSTR)(LPCSTR)csWindowsDirectory,
            (LPSTR)(LPCSTR)csApplicationDirectory,
            csCurrentDirectory.GetBuffer(uCurrentLength),
            &uCurrentLength,
            csInstallDirectory.GetBuffer(uInstallLength),
            &uInstallLength);

         TRACE("VerFindFile returned %04x\n", uResult);
         csCurrentDirectory.ReleaseBuffer();
         csInstallDirectory.ReleaseBuffer();

         Util::AppendBackslashToPath(csCurrentDirectory);
         Util::AppendBackslashToPath(csInstallDirectory);

         /*
         // Make sure VerFindFile() returned successfully.
         */

         if ((uResult & VFF_BUFFTOOSMALL) != 0)
         {
            TRACE("VerFindFile - VFF_BUFFTOOSMALL\n");
            /* can't get directory information, buffer too small */
            Util::MessageBox(
               MB_OK,
               (UINT)-1,
               MAKEINTRESOURCE(IDS_ErrVerFindFileFormat),
               (LPCSTR)csDestinationPath);
            AfxThrowUserException();
         }

         if ((uResult & VFF_FILEINUSE) != 0)
         {
            fInUse = TRUE;
         }
         else
         {
            /*
            // There is a version of VER.DLL (5/22/92, 8736 Bytes) that likes to install everything
            // into the shared directory, usually C:\WINDOWS\SYSTEM, even if the file is not shared.
            // So, to defeat this unusual behavior, we only use the directory suggested by VerFindFile()
            // if the file is shared.
            */

            if ((pProgramFile->GetFlags() & CProgramFile::Shared) != 0) 
            {
               TRACE("File shared, use directory |%s|\n", (LPCSTR)csInstallDirectory);
               /*
               // Rebuild the destination information based on the directory returned
               // by VerFindFile().
               */
                  
               Util::ConstructPath(csDestinationPath, csInstallDirectory, csDestinationFile);
               Util::SplitPath(csDestinationPath, &csDestinationDirectory, &csDestinationFile);
            }

         /*
         // Try to get exclusive access to the file.
         */

            CFileException e;
            TRACE("See if we can access file %s...\n", (LPCSTR)csDestinationPath);
            if (!cfFile.Open(csDestinationPath,
                            CFile::modeReadWrite | CFile::shareExclusive,
                            &e))
            {
               TRACE("*** ERROR %d ***\n", e.m_cause);
               switch (e.m_cause)
               {
                  case CFileException::accessDenied:
                  case CFileException::sharingViolation:
                  case CFileException::lockViolation:
                  {
                     fInUse = TRUE;
                     break;
                  }
                  default:
                  {
                     break;
                  }
               }
            }
         }
      }
   }
   CATCH_ALL(e)
   {
      csSourcePath.Empty();
      csDestinationPath.Empty();
      csDestinationDirectory.Empty();
      csDestinationFile.Empty();
      csCurrentDirectory.Empty();
      csInstallDirectory.Empty();
      csWindowsDirectory.Empty();

      THROW_LAST();
   }
   END_CATCH_ALL

   return fInUse;
}

/*
// Parse a parameter in a command line. A Parameter always begins
// at the start of the command line or after a space. No spaces
// are allowed within parameters, but quoted strings are handled
// properly and may contain spaces. All comparisons are case-insensitive.
*/

LPSTR CInstallApp::LocateParameter(LPSTR lpszParamName, LPSTR lpszValue)
{
   LPSTR lpszResult = NULL;
   
   LPSTR lpszCommandLine = m_lpCmdLine;
   int nParamNameLength = lstrlen(lpszParamName);
   int nCommandLineLength = lstrlen(lpszCommandLine);
   BOOL fInQuote = FALSE;
   BOOL fStartOfParameter = TRUE;
   BOOL fFoundParameter = FALSE;
   
   while ((!fFoundParameter) && (nCommandLineLength >= nParamNameLength))
   {
      if (fStartOfParameter)
      {
         // A parameter can start here, check for a match.
         
         int i = 0;
         
         for (;;)
         {
            char ch1 = lpszCommandLine[i];
            if ((ch1 > 'a') && (ch1 < 'z')) ch1 = (char)(ch1+'A'-'a');
            
            char ch2 = lpszParamName[i];
            if ((ch2 > 'a') && (ch2 < 'z')) ch2 = (char)(ch2+'A'-'a');
            
            if (ch2 == 0)
            {
               fFoundParameter = TRUE;
               break;
            }
            
            if (ch1 != ch2)
            {
               fStartOfParameter = FALSE;
               break;
            }
            
            i++;
         }
      }
      
      else
      {
         if (*lpszCommandLine == ' ')
         {
            fStartOfParameter = !fInQuote;
         }
         
         else if (*lpszCommandLine == '"')
         {
            fInQuote = !fInQuote;
         }
         
         lpszCommandLine++;
         nCommandLineLength--;
      }
   }
   
   if (fFoundParameter)
   {
      lpszResult = lpszCommandLine;
      
      if (lpszValue != NULL)
      {
         *lpszValue = 0;
         
         lpszCommandLine += nParamNameLength;

         if (*lpszCommandLine == '=')
         {
            lpszCommandLine++;
         }
         
         fInQuote = FALSE;
         
         for (;;)
         {
            if (*lpszCommandLine == 0)
            {
               break;
            }
            
            if ((!fInQuote) && (*lpszCommandLine == ' '))
            {
               break;
            }
            
            if (*lpszCommandLine == '"')
            {
               fInQuote = !fInQuote;
            }
            
            else
            {
               *(lpszValue++) = *lpszCommandLine;
            }
            
            lpszCommandLine++;
         }
         
         *lpszValue = 0;
      }
   }
   
   return lpszResult;
}

BOOL CInstallApp::FindParameter(LPSTR lpszParamName)
{
   return LocateParameter(lpszParamName, NULL) != NULL;
}

LPSTR CInstallApp::GetParameter(LPSTR lpszParamName, LPSTR lpszValue)
{
   if (LocateParameter(lpszParamName, lpszValue) == NULL)
   {
      return NULL;
   }
   
   else
   {
      return lpszValue;
   }
}

BOOL CInstallApp::DemoCanBeInstalledIntoDirectory(LPCSTR pszDirectory)
{
   BOOL fCanInstall = TRUE;   // Allow install on exceptions.

   CString csDirectory;
   CString csPath;
   CString csFile;

   TRY
   {
      csDirectory = pszDirectory;
      Util::LoadString(IDS_InstallationInformationFileName, csFile);

      // Build the path to the PMINST.DAT file.
      Util::ConstructPath(csPath, csDirectory, csFile);

      // Check if the file exists. If not, then allow the demo
      // to be installed.
      if (Util::FileExists(csPath))
      {
         // Read the installation information from PMINST.DAT.
         InstallationInfo Info;
         if (Info.Get(csPath, FALSE))
         {
            // Extract the previous configuration information.
            m_PreviousInfo.m_nInstallConfiguration = Info.nInstallConfiguration;
            m_PreviousInfo.m_nCurrentConfiguration = Info.nCurrentConfiguration;

            // Since we have a PMINST.DAT, we know there is some sort of
            // PrintMaster like think in the directory. In this case, we only
            // allow the user to install the demo into the directory if the
            // currrent resident of the directory is a demo (at the moment
            // we do not care what kind of demo.)

            fCanInstall = Info.nCurrentConfiguration == CPrintMasterConfiguration::Demo;
         }
      }
   }
   END_TRY

   return fCanInstall;
}

#ifdef SWITCH_CDS
//
// Make sure a CD is in a drive.
//

char CInstallApp::InsertCD(LPCSTR pszVolumeLabel, LPCSTR pszCDName, char& cDrive)
{
	for (;;)
	{
		if ((cDrive = FindCD(pszVolumeLabel)) != 0)
		{
			return TRUE;
		}

		// Prompt for the CD.
		if (!PromptForCD(pszCDName))
		{
			return FALSE;
		}
	}
}

//
// Get the volume label for a drive.
// cDrive is in the range 'A' to 'Z' for meaningful results.
// This uses an internal buffer which will be overwritten by multiple calls.
//

static LPCSTR GetVolumeLabel(char cDrive)
{
	// Our static volume buffer.
	static char cbVolume[32];

	// Get the volume label for the drive.
	static char cbPath[] = "X:\\";
	cbPath[0] = cDrive;

	// Get the volume information.
	if (!::GetVolumeInformation(cbPath,
										 cbVolume,
										 sizeof(cbVolume),
										 NULL,
										 NULL,
										 NULL,
										 NULL,
										 0))
	{
		return NULL;
	}
	return cbVolume;
}

//
// Find an inserted CD with the given volume label.
//

char CInstallApp::FindCD(LPCSTR pszVolumeLabel)
{
	//
	// There are two phases:
	// 1) Look on CD drives.
	// 2) Look on remote drives.
	// 3) Look on everything else.
	//

	for (int nPhase = 0; nPhase < 2; nPhase++)
	{
		for (char cDrive = 'A'; cDrive <= 'Z'; cDrive++)
		{
			// Get the type of the drive.
			UINT uType = Util::GetTypeOfDrive(cDrive);

			// See if we need to check.
			BOOL fCheck = FALSE;
			switch (nPhase)
			{
				case 0:
				{
					// Check CD drives.
					fCheck = (uType == DRIVE_CDROM);
					break;
				}
				case 1:
				{
					// Check remote drives.
					fCheck = (uType == DRIVE_REMOTE);
					break;
				}
				case 2:
				{
					// Check all remaining drive types.
					fCheck = (uType != DRIVE_CDROM
								 && uType != DRIVE_REMOTE
								 && uType != DRIVE_UNKNOWN
								 && uType != DRIVE_NO_ROOT_DIR
								 && (uType != DRIVE_REMOVABLE
										|| cDrive >= 'C'));
					break;
				}
				default:
				{
					ASSERT(FALSE);
					break;
				}
			}

			// If we want to check the volume, do it now.
			if (fCheck)
			{
				// Get the volume label to compare with.
				LPCSTR pszThisVolume = GetVolumeLabel(cDrive);
				if (pszThisVolume != NULL)
				{
					if (stricmp(pszThisVolume, pszVolumeLabel) == 0)
					{
						// We found it!
						return cDrive;
					}
				}
			}
		}
	}
	// No luck.
	return 0;
}

//
// Prompt for the insertion of a CD.
//

BOOL CInstallApp::PromptForCD(LPCSTR pszCDName)
{
	// Get the message template from the resource file.
	CString csTemplate;
	csTemplate.LoadString(IDS_PromptForCD);

	// Construct the message.
	CString csMessage;
	csMessage.Format(csTemplate, pszCDName);

	// Do the message box.
	return AfxMessageBox(csMessage) == IDOK;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// Install-specific versions of PMWCFG code which PMW cannot use.

BOOL CPrintMasterConfiguration::CanInstantUpgrade(void)
{
   ASSERT(FALSE);    // Should never happen!
   return FALSE;
}

void CDemoConfiguration::GetDemoIniFileName(CString& csFileName)
{
   TRY
      Util::ConstructPath(csFileName, GetInstallApp()->GetSourceDirectory(), "MSREG\\" DemoIniFileName);
   END_TRY
}

BOOL CDemoConfiguration::SupportsCardBack(BOOL fComplain /*=TRUE*/, CPmwDoc* pDoc /*=NULL*/)
{
   ASSERT(FALSE);    // Should never happen!
   return FALSE;
}

BOOL CDemoConfiguration::UnsupportedDialog(LPCSTR pMessage /*=NULL*/, LPCSTR pTitle /*=NULL*/)
{
   ASSERT(FALSE);    // Should never happen!
   return FALSE;
}

#ifndef AG_BUILD
BOOL CBonusPackConfiguration::UnsupportedDialog(LPCSTR pMessage /*=NULL*/, LPCSTR pTitle /*=NULL*/)
{
   ASSERT(FALSE);    // Should never happen!
   return FALSE;
}
#endif

BOOL CPrintStandardConfiguration::UnsupportedDialog(LPCSTR pMessage /*=NULL*/, LPCSTR pTitle /*=NULL*/)
{
   ASSERT(FALSE);    // Should never happen!
   return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CProductInfo

static inline int AdjustConfiguration(int nConfiguration)
{
   return 0;
//   return nConfiguration;
}

BOOL CProductInfo::operator > (const CProductInfo& OtherInfo)
{
   int nInstallConfiguration = AdjustConfiguration(m_nInstallConfiguration);
   int nOtherConfiguration = AdjustConfiguration(OtherInfo.m_nInstallConfiguration);

   return (m_nVersionMajor > OtherInfo.m_nVersionMajor)
      || ((m_nVersionMajor == OtherInfo.m_nVersionMajor)
       && ((m_nVersionMinor > OtherInfo.m_nVersionMinor)
        || ((m_nVersionMinor == OtherInfo.m_nVersionMinor)
         && ((nInstallConfiguration > nOtherConfiguration)
          || ((nInstallConfiguration == nOtherConfiguration)
           && (m_nRevision > OtherInfo.m_nRevision))))));
}

BOOL CProductInfo::operator == (const CProductInfo& OtherInfo)
{
   int nInstallConfiguration = AdjustConfiguration(m_nInstallConfiguration);
   int nOtherConfiguration = AdjustConfiguration(OtherInfo.m_nInstallConfiguration);

   return (nInstallConfiguration == nOtherConfiguration)
         && (m_nVersionMajor == OtherInfo.m_nVersionMajor)
         && (m_nVersionMinor == OtherInfo.m_nVersionMinor)
         && (m_nRevision == OtherInfo.m_nRevision);
}

BOOL CProductInfo::operator < (const CProductInfo& OtherInfo)
{
   int nInstallConfiguration = AdjustConfiguration(m_nInstallConfiguration);
   int nOtherConfiguration = AdjustConfiguration(OtherInfo.m_nInstallConfiguration);

   return (m_nVersionMajor < OtherInfo.m_nVersionMajor)
      || ((m_nVersionMajor == OtherInfo.m_nVersionMajor)
       && ((m_nVersionMinor < OtherInfo.m_nVersionMinor)
        || ((m_nVersionMinor == OtherInfo.m_nVersionMinor)
         && ((nInstallConfiguration < nOtherConfiguration)
          || ((nInstallConfiguration == nOtherConfiguration)
           && (m_nRevision < OtherInfo.m_nRevision))))));
}
