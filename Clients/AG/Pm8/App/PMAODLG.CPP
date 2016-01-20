/*
// $Workfile: PMAODLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
//
// Copyright © 1996 by Mindscape.
// All Rights Reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PMAODLG.CPP $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 2     2/11/98 11:15a Dennis
// Removed GetRegistryString() and now use GetRegistryString() located in
// util.*
// 
//    Rev 1.0   14 Aug 1997 15:23:44   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:42   Fred
// Initial revision.
// 
//    Rev 1.3   16 Jun 1997 13:55:32   johno
// 
//    Rev 1.2   12 Jun 1997 18:06:32   johno
// 
//    Rev 1.1   12 Jun 1997 14:34:02   johno
// 
//    Rev 1.0   11 Jun 1997 16:35:04   johno
// Initial revision.
// 
//    Rev 1.0   16 Dec 1996 16:01:30   Fred
// Initial revision.
*/ 

#include "stdafx.h"
#include "pmaoDlg.h"
#include "chdirdlg.h"
#include "util.h"

#ifdef WIN32
#include <winreg.h>
#include <regstr.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define  VER_INSTALL_MIN   4
#define  VER_INSTALL       4
#define  VER_INSTALL_MAX   4

/////////////////////////////////////////////////////////////////////////////
// CAddonInstallDlg dialog

CString  CAddonInstallDlg::s_csPmDirectory;

UINT DialogIds[] =
{
   IDD_ADDONINST_CANT_FIND_PRINTMASTER,
   IDD_ADDONINST_TOO_OLD,
   IDD_ADDONINST_WRONG_BITNESS_16,
   IDD_ADDONINST_WRONG_BITNESS_32,
   IDD_ADDONINST_TOO_NEW,
   IDD_ADDONINST_DIALOG
};

UINT ErrorIds[] =
{
   IDS_BROWSE_NOT_FOUND,
   IDS_BROWSE_TOO_OLD,
   IDS_BROWSE_WRONG_BITNESS_16,
   IDS_BROWSE_WRONG_BITNESS_32,
   IDS_BROWSE_TOO_NEW,
   0
};

CAddonInstallDlg::CAddonInstallDlg(int nFindResult, const CString& csPmDirectory, CWnd* pParent /*=NULL*/)
   : CDialog(DialogIds[nFindResult], pParent)
{
   //{{AFX_DATA_INIT(CAddonInstallDlg)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
   // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
   m_nFindResult = nFindResult;
   m_csPmDirectory = csPmDirectory;
}

void CAddonInstallDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CAddonInstallDlg)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAddonInstallDlg, CDialog)
   //{{AFX_MSG_MAP(CAddonInstallDlg)
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   ON_BN_CLICKED(IDC_ADDONINST_BROWSE, OnBrowse)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddonInstallDlg message handlers

BOOL CAddonInstallDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

#ifdef WIN32
   // Set the icon for this dialog.  The framework does this automatically
   //  when the application's main window is not a dialog
   SetIcon(m_hIcon, TRUE);       // Set big icon
   SetIcon(m_hIcon, FALSE);      // Set small icon
#endif

   // Set up directory edit.
   CEdit*   pEdit;
   if ((pEdit = (CEdit*)GetDlgItem(IDC_ADDONINST_DIRECTORY)) != NULL)
   {
      pEdit->SetWindowText (m_csPmDirectory);
      pEdit->LimitText(_MAX_PATH);
   }
   
#ifndef WIN32
   Util::CenterWindowInWindow(this);
#endif

   return UpdateInterface();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAddonInstallDlg::OnPaint() 
{
   if (IsIconic())
   {
      CPaintDC dc(this); // device context for painting

      SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

      // Center icon in client rectangle
      int cxIcon = GetSystemMetrics(SM_CXICON);
      int cyIcon = GetSystemMetrics(SM_CYICON);
      CRect rect;
      GetClientRect(&rect);
      int x = (rect.Width() - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;

      // Draw the icon
      dc.DrawIcon(x, y, m_hIcon);
   }
   else
   {
      CDialog::OnPaint();
   }
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAddonInstallDlg::OnQueryDragIcon()
{
   return (HCURSOR) m_hIcon;
}

void CAddonInstallDlg::OnBrowse() 
{
   // Let the user choose a directory.
   CAddOnChooseDirectoryDialog Dialog(IDD_CHOOSE_DIRECTORY, this);
   TRY
   {
      // Run the dialog.
      Dialog.m_csDirectory = m_csPmDirectory;
      Dialog.m_pClientValidation = ValidateChooseDirectory;
      Dialog.m_dwClientValidation = 0;
      
      if (Dialog.GetDirectory(
                  CAddOnChooseDirectoryDialog::OPTION_no_create |
                  CAddOnChooseDirectoryDialog::OPTION_no_ask_to_create |
                  CAddOnChooseDirectoryDialog::OPTION_expand))
      {
         // Use the directory the user selected.
         m_csPmDirectory = Dialog.m_csDirectory;
         if (GetDlgItem(IDC_ADDONINST_DIRECTORY) != NULL)
         {
            SetDlgItemText(IDC_ADDONINST_DIRECTORY, m_csPmDirectory);
         }
      }
   }
   END_TRY
}

BOOL CAddonInstallDlg::ValidateChooseDirectory(DWORD dwData, LPCSTR pszDirectory, HWND hParent)
{
   // The user selected a directory, check if it contains
   // a PrintMaster that can be upgraded.
   //int nResult = ((CPmtwinstApp*)AfxGetApp())->CheckPrintMasterDirectory(pszDirectory);
   int nResult = CAddonInstallDlg::CheckPrintMasterDirectory(pszDirectory);
   UINT uId = ErrorIds[nResult];
   if (uId != 0)
   {
      CString csFormat;
      CString csMessage;
      TRY
      {
         csFormat.LoadString(uId);
         csMessage.Format(csFormat, pszDirectory);
         AfxMessageBox(csMessage);
      }
      END_TRY
   }

   //return nResult == CPmtwinstApp::FIND_Found;
   return nResult == FIND_Found;
}

void CAddonInstallDlg::OnOK()
{
   UpdateInterface();

   // Make sure the directory contains a PrintMaster that can be upgraded.
   if (!ValidateChooseDirectory(0, m_csPmDirectory, GetSafeHwnd()))
   {
      return;
   }
   
   CDialog::OnOK();
}

void CAddonInstallDlg::OnCancel() 
{
   CDialog::OnCancel();
}

void CAddonInstallDlg::UpdateDirectoryString(void)
{
   m_csPmDirectory.Empty();
   if (GetDlgItem(IDC_ADDONINST_DIRECTORY) != NULL)
   {
      GetDlgItem(IDC_ADDONINST_DIRECTORY)->GetWindowText(m_csPmDirectory);
   }
}

BOOL CAddonInstallDlg::UpdateInterface(void)
{
   UpdateDirectoryString();

   int nResult = CheckPrintMasterDirectory(m_csPmDirectory);

   EnableControl(IDOK, nResult == FIND_Found);

   // If we could not find a PrintMaster directory, set the initial
   // focus to the edit control so the user can start typing.
   if (nResult != FIND_Found)
   {
      if (GetDlgItem(IDC_DIRECTORY) != NULL)
      {
         GetDlgItem(IDC_DIRECTORY)->SetFocus();
         return FALSE;
      }
   }

   return TRUE;
}

void CAddonInstallDlg::EnableControl(UINT nId, BOOL fEnable)
{
   CWnd* pControl = GetDlgItem(nId);
   if (pControl->GetSafeHwnd() != NULL)
   {
      if (pControl->IsWindowVisible())
      {
         if ((!fEnable) && (GetFocus() == pControl))
         {
            NextDlgCtrl();
         }
         pControl->EnableWindow(fEnable);
      }
   }
}
/////////////////////////////////////////////////////////////////////////////
int 
CAddonInstallDlg::FindPrintMasterDirectory(void)
{
   int nCheckResult;
   CString csCheckPath;
   CString csPath;
   CString csDirectory;

   TRY
   {
      nCheckResult = FIND_NotFound;
      csCheckPath.Empty();

#ifdef WIN32
      if (nCheckResult != FIND_Found)
      {
         // Try to get the app path from the registry.
         Util::GetRegistryString(HKEY_LOCAL_MACHINE,
                           GetAppPathKey(),
                           NULL,
                           csPath);
         if (!csPath.IsEmpty())
         {
            // Extract just the path part.
            Util::SplitPath(csPath, &csDirectory, NULL);

            // Check if it's a valid directory.
            int nResult = CheckPrintMasterDirectory(csDirectory);
            if (nResult > nCheckResult)
            {
               nCheckResult = nResult;
               csCheckPath = csDirectory;
            }
         }
      }
#endif

      if (nCheckResult != FIND_Found)
      {
         // Try to get the app path from the MSREGUSR.INI file.
         GetMsregUsrIniFileName(csPath);

         if (!csPath.IsEmpty())
         {
            GetPrivateProfileString("PrintMaster Gold 3.0",
                                    "AppPath",
                                    "",
                                    csDirectory.GetBuffer(_MAX_PATH),
                                    _MAX_PATH,
                                    csPath);
            csDirectory.ReleaseBuffer();

            // Check if it's a valid directory.
            int nResult = CheckPrintMasterDirectory(csDirectory);
            if (nResult > nCheckResult)
            {
               nCheckResult = nResult;
               csCheckPath = csDirectory;
            }
         }
      }

      if (nCheckResult != FIND_Found)
      {
         // Hmmm, still no luck, try \PMW on local hard drives.
         for (char cDrive = 'A'; cDrive <= 'Z'; cDrive++)
         {
            UINT uDriveType = Util::GetTypeOfDrive(cDrive);
            if ((uDriveType == DRIVE_FIXED) || (uDriveType == DRIVE_REMOTE))
            {
               // Check the PMW directory on the specified drive.
               static char szPath[] = "@:\\PMW";
               szPath[0] = cDrive;
               int nResult = CheckPrintMasterDirectory(szPath);
               if (nResult > nCheckResult)
               {
                  nCheckResult = nResult;
                  csCheckPath = szPath;
               }
               if (nCheckResult == FIND_Found)
               {
                  break;
               }
            }
         }
      }
   }
   END_TRY

   s_csPmDirectory = csCheckPath;
   Util::RemoveBackslashFromPath(s_csPmDirectory);

   return nCheckResult;
}

#ifdef WIN32
CString 
CAddonInstallDlg::GetAppPathKey(void)
{
   CString csKey = REGSTR_PATH_APPPATHS "\\pmw.exe";
   return csKey;
}
#endif

// Check if a directory contains an installation of PrintMaster which can
// be upgraded with TWAIN support. Such a directory must meet the following
// conditions:
//
//    1) Contain PMW.EXE
//    2) Contain MSREG.INI
//    3) Version in MSREG.INI must be 3.XX.XX
//    4) If 16-bit, must NOT contain PMWTHUNK.EXE
//    5) If 32-bit, must contain PMWTHUNK.EXE

int 
CAddonInstallDlg::CheckPrintMasterDirectory(LPCSTR pszPath)
{
   int nResult = FIND_NotFound;

   s_csPmDirectory.Empty();

   CString csFileName;
   CString csVersion;
   CString csVersionPart;
   int nVersionMajor;
   int nVersionMinor;
   int nVersionBuild;

   TRY
   {
      // Make sure the directory exists.
      if (Util::DirectoryExists(pszPath))
      {
         // Make sure PMW.EXE exists.
         Util::ConstructPath(csFileName, pszPath, "PMW.EXE");
         if (Util::FileExists(csFileName))
         {
            // Make sure MSREG.INI exists.
            Util::ConstructPath(csFileName, pszPath, "MSREG.INI");
            if (Util::FileExists(csFileName))
            {
               // Get the version number from MSREG.INI.
               csVersion.Empty();
               GetPrivateProfileString(
                  "Configuration",
                  "Version",
                  "",
                  csVersion.GetBuffer(128),
                  128-1,
                  csFileName);
               csVersion.ReleaseBuffer();

               // Break version number up into its components.
               nVersionMajor = 0;
               nVersionMinor = 0;
               nVersionBuild = 0;

               int nIndex;

               // Get major version number.
               nIndex = csVersion.Find('.');
               if (nIndex != -1)
               {
                  csVersionPart = csVersion.Left(nIndex);
                  csVersion = csVersion.Mid(nIndex+1);
               }
               else
               {
                  csVersionPart = csVersion;
                  csVersion.Empty();
               }
               nVersionMajor = atoi(csVersionPart);

               // Get minor version number.
               nIndex = csVersion.Find('.');
               if (nIndex != -1)
               {
                  csVersionPart = csVersion.Left(nIndex);
                  csVersion = csVersion.Mid(nIndex+1);
               }
               else
               {
                  csVersionPart = csVersion;
                  csVersion.Empty();
               }
               nVersionMinor = atoi(csVersionPart);

               // Get build number.
               nIndex = csVersion.Find('.');
               if (nIndex != -1)
               {
                  csVersionPart = csVersion.Left(nIndex);
                  csVersion = csVersion.Mid(nIndex+1);
               }
               else
               {
                  csVersionPart = csVersion;
                  csVersion.Empty();
               }
               nVersionBuild = atoi(csVersionPart);
od ("Found version %d.%d.%d\r\n", nVersionMajor, nVersionMinor, nVersionBuild);
od ("It's in %s\r\n", (const char *)csFileName);
               // Check the version numbers.
               if (nVersionMajor == VER_INSTALL)
               {
#if 0
                  // Make sure the presence (or absence) of PMWTHUNK.EXE matches
                  // the "bitness" of ourselves.
                  Util::ConstructPath(csFileName, pszPath, "PMWRES.DLL");
#ifdef WIN32
                  if (!Util::FileExists(csFileName))
#else
                  if (Util::FileExists(csFileName))
#endif
                  {
                     // Wow! All criteria were met, so this looks like
                     // something we can upgrade with TWAIN support.
                     nResult = FIND_Found;
                  }
                  else
                  {
#ifdef WIN32
                     nResult = FIND_WrongBitness32;
#else
                     nResult = FIND_WrongBitness16;
#endif
                  }
#else
                  Util::ConstructPath(csFileName, pszPath, "SMACKW32.DLL");
                  if (Util::FileExists(csFileName))
                     nResult = FIND_Found;
                  else
                  {
                     Util::ConstructPath(csFileName, pszPath, "SMACKW16.DLL");
                     if (Util::FileExists(csFileName))
                        nResult = FIND_Found;
                  }   
#endif
               }
               else
               {
                  if (nVersionMajor < VER_INSTALL_MIN)
                  {
                     nResult = FIND_Before400;
                  }
                  if (nVersionMajor > VER_INSTALL_MAX) 
                  {
                     nResult = FIND_After400;
                  }
               }
            }
         }
      }
   }
   END_TRY

   return nResult;
}

BOOL 
CAddonInstallDlg::GetMsregUsrIniFileName(CString& csFileName)
{
   CString csDirectory;
   TRY
   {
      csFileName.Empty();

      ::GetWindowsDirectory(csDirectory.GetBuffer(256), 256-1);
      csDirectory.ReleaseBuffer();

      Util::ConstructPath(csFileName, csDirectory, "MSREGUSR.INI");
   }
   CATCH_ALL(e)
   {
      csFileName.Empty();
   }
   END_CATCH_ALL

   return !csFileName.IsEmpty();
}
/////////////////////////////////////////////////////////////////////////////
// CAddOnChooseDirectoryDialog dialog

CAddOnChooseDirectoryDialog::CAddOnChooseDirectoryDialog(int IDD, CWnd* pParent /*=NULL*/)
   : CFileDialog(
         TRUE,                   // bOpenFileDialog
         NULL,                   // lpszDefExt
         "dummy",                // lpszFileName
         OFN_ENABLETEMPLATE   |  // dwFlags
         OFN_HIDEREADONLY     |
         OFN_NOTESTFILECREATE |
         OFN_NOVALIDATE,
         NULL,                   // lpszFilter
         pParent)                // pParentWnd
{
   //{{AFX_DATA_INIT(CAddOnChooseDirectoryDialog)
   //}}AFX_DATA_INIT
   
#ifdef WIN32
   m_ofn.Flags &= ~OFN_EXPLORER;
#endif

#ifndef WIN32
   m_ofn.Flags &= ~OFN_SHOWHELP;
#endif

   /*
   // Enable our template.
   */

   m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD);
   //m_ofn.hInstance = AfxGetInstanceHandle();
   m_ofn.hInstance = AfxGetResourceHandle();
   
   /*
   // Initialize our data.
   */
   
   m_csDirectory.Empty();
   m_pClientValidation = NULL;
   m_dwClientValidation = 0;
   m_fSuccess = FALSE;
   m_nIDD = IDD;
}

void CAddOnChooseDirectoryDialog::DoDataExchange(CDataExchange* pDX)
{
   CFileDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CAddOnChooseDirectoryDialog)
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAddOnChooseDirectoryDialog, CFileDialog)
   //{{AFX_MSG_MAP(CAddOnChooseDirectoryDialog)
   ON_WM_DESTROY()
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAddOnChooseDirectoryDialog::OnLBSelChangedNotify(UINT nIDBox, UINT iCurSel, UINT nCode)
{
   /*
   // The directory selection has changed, copy the directory to our edit control.
   */
   
   CString csDirectory;
   
   TRY
   {
      if (m_fFirstChangeNotification)
      {
         m_fFirstChangeNotification = FALSE;
         csDirectory = m_csDirectory;
      }
      
      else
      {
         GetDlgItemText(IDC_CHOOSE_DIRECTORY_BUFFER, csDirectory.GetBuffer(256), 256);
         csDirectory.ReleaseBuffer();
      }
      
      SetDlgItemText(IDC_CHOOSE_DIRECTORY_NAME, csDirectory);
   }
   CATCH_ALL(e)
   {
      csDirectory.Empty();
      
      THROW_LAST();
   }
   END_CATCH_ALL
}

BOOL CAddOnChooseDirectoryDialog::OnFileNameOK()
{
   GetDlgItemText(IDC_CHOOSE_DIRECTORY_NAME, m_csDirectory.GetBuffer(256), 256);
   m_csDirectory.ReleaseBuffer();
      
   if (((m_wFlags & OPTION_expand) == 0) || (m_csDirectory.IsEmpty()) || (Util::ExpandDirectory(m_csDirectory, FALSE)))
   {
      m_fSuccess = ValidateDirectory(m_csDirectory, m_wFlags);
   }
      
   if (!m_fSuccess)
   {
      ((CEdit *)GetDlgItem(IDC_CHOOSE_DIRECTORY_NAME))->SetSel(0, -1);
   }
   
   return !m_fSuccess;
}

BOOL CAddOnChooseDirectoryDialog::GetDirectory(WORD wFlags /* = 0 */)
{
   /*
   // Initialize our data.
   */

   m_wFlags = wFlags;   
   m_fSuccess = FALSE;
   
   if (m_csDirectory.IsEmpty())
   {
      m_fFirstChangeNotification = FALSE;
      m_ofn.lpstrInitialDir = NULL;
   }
   
   else
   {
      m_fFirstChangeNotification = TRUE;
      m_ofn.lpstrInitialDir = m_csDirectory;
   }
   
   DoModal();
   
   return m_fSuccess;
}

BOOL CAddOnChooseDirectoryDialog::ValidateDirectory(const CString &csDirectory, WORD wFlags)
{
   BOOL fResult = FALSE;
   BOOL fTryAgain = TRUE;
   
   if ((wFlags & OPTION_no_validate) != 0)
   {
      fResult = TRUE;
      goto cleanup;
   }
   
check_directory:

   /*
   // Empty directory names are rejected with a special dialog box.
   */
   
   if (csDirectory.IsEmpty())
   {
      DirectoryMessageBox(
         this,
         csDirectory,
         IDS_DirectoryIsEmpty,
         IDS_Error,
         MB_ICONEXCLAMATION|MB_OK|MB_APPLMODAL,
         wFlags);
      goto cleanup;
   }
   
   /*
   // Invalid directory names are rejected with a special dialog box.
   */
   
   if (!DirectoryNameIsValid(csDirectory))
   {
      DirectoryMessageBox(
         this,
         csDirectory,
         IDS_DirectoryNameIsInvalid,
         IDS_Error,
         MB_ICONEXCLAMATION|MB_OK|MB_APPLMODAL,
         wFlags);
      goto cleanup;
   }
   
   /*
   // Get the flags for the selected directory.
   */
   
   WORD wDirectoryFlags;
   
   wDirectoryFlags = CheckDirectory(csDirectory, wFlags);
   
   /*
   // Check if the directory is a file.
   */
   
   if ((wDirectoryFlags & DIRECTORY_is_file) != 0)
   {
      DirectoryMessageBox(
         this,
         csDirectory,
         IDS_DirectoryIsFile,
         IDS_Error,
         MB_ICONEXCLAMATION|MB_OK|MB_APPLMODAL,
         wFlags);
      goto cleanup;
   }
   
   /*
   // If the user cares if the directory exists, then check.
   */
   
   if (((wFlags & OPTION_no_exists_check) == 0)
    && ((wDirectoryFlags & DIRECTORY_exists) == 0))
   {
      /*
      // The directory does not exist and the user cares.
      */
      
      if (!fTryAgain)
      {
         // we already tried once, give up
         goto cleanup;
      }
      
      /*
      // The directory does not exist, check if the user wants to create it.
      // If not, then simply report that the directory does not exist and therefore
      // cannot be used.
      */
      
      if ((wFlags & OPTION_no_create) != 0)
      {
         DirectoryMessageBox(
            this,
            csDirectory,
            IDS_DirectoryDoesNotExistChooseAnother,
            IDS_Error,
            MB_ICONEXCLAMATION|MB_OK|MB_APPLMODAL,
            wFlags);
         goto cleanup;
      }
      
      /*
      // The user wants to attempt to create the directory. Ask the user
      // if they want to do this unless the OPTION_no_ask_to_create option
      // has been specified.
      */
         
      if (((wFlags & OPTION_no_ask_to_create) != 0)
       || (DirectoryMessageBox(
            this,
            csDirectory,
            IDS_DirectoryDoesNotExistCreate,
            IDS_Confirm,
            MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2|MB_APPLMODAL,
            wFlags,
            IDYES) == IDYES))
      {
         if (Util::MakeDirectory(csDirectory))
         {
            fTryAgain = FALSE;
            goto check_directory;
         }
         
         else
         {
            /*
            // The directory could not be created, report the error to the user.
            */
            
            DirectoryMessageBox(
               this,
               csDirectory,
               IDS_ErrCreateDirectory,
               IDS_Error,
               MB_ICONEXCLAMATION|MB_OK|MB_APPLMODAL,
               wFlags);
         }
      }
      
      /*
      // The user does not want to try and create the directory.
      */
      
      goto cleanup;
   }
   
   /*
   // If the directory exists and the caller cares if the directory is writable,
   // then check if the directory is writable.
   */
   
   if (((wDirectoryFlags & DIRECTORY_exists) != 0)
    && ((wFlags & OPTION_no_writable_check) == 0)
    && ((wDirectoryFlags & DIRECTORY_writable) == 0))
   {
      DirectoryMessageBox(
         this,
         csDirectory,
         IDS_DirectoryNotWritableChooseAnother,
         IDS_Error,
         MB_ICONEXCLAMATION|MB_OK|MB_APPLMODAL,
         wFlags);
      goto cleanup;
   }

   // Perform any extra validation required by the client.
   if (m_pClientValidation != NULL)
   {
      if (!((*m_pClientValidation)(m_dwClientValidation, (LPCSTR)csDirectory, GetSafeHwnd())))
      {
         goto cleanup;
      }
   }

   fResult = TRUE;
   
cleanup:

   return fResult;
}

BOOL CAddOnChooseDirectoryDialog::DirectoryNameIsValid(const CString &csDirectory)
{
#ifdef WIN32
   return TRUE;
#else
/*
// See if this has any illegal characters in it.
*/
   static char cbBadChar[] = " ?*[],\"\';/.";
   for (LPCSTR p = cbBadChar; *p != '\0'; )
   {
      if (csDirectory.Find(*p++) != -1)
      {
         return FALSE;
      }
   }

/*
// Make sure all the components are valid.
*/

   CString csCurrent;
   CString csPath;
   CString csName;
   CString csExtension;

   TRY
   {
      csCurrent = csDirectory;
      while (!csCurrent.IsEmpty() && csCurrent != "\\")
      {
         Util::RemoveBackslashFromPath(csCurrent);

         Util::SplitPath(csCurrent, NULL, &csPath, &csName, &csExtension);
         if (csName.GetLength() > 8 || csExtension.GetLength() > 3)
         {
            return FALSE;
         }
         csCurrent = csPath;
      }
   }
   CATCH_ALL(e)
   {
      return FALSE;
   }
   END_CATCH_ALL
   return TRUE;
#endif
}

WORD CAddOnChooseDirectoryDialog::CheckDirectory(const CString &csDirectory, WORD wFlags)
{
   WORD wDirectoryFlags = 0;

   UINT wOldSem = SetErrorMode(SEM_FAILCRITICALERRORS);
      
   if (Util::FileExists(csDirectory))
   {
      wDirectoryFlags = DIRECTORY_is_file;
   }
      
   else if (!Util::DirectoryExists(csDirectory))
   {
      wDirectoryFlags = 0;
   }
      
   else
   {
      wDirectoryFlags = DIRECTORY_exists;
            
      if ((wFlags & OPTION_no_writable_check) == 0)
      {
         if (Util::DirectoryIsWritable(csDirectory))
         {
            wDirectoryFlags |= DIRECTORY_writable;
         }
      }
   }
   
   SetErrorMode(wOldSem);

   return wDirectoryFlags;
}

int CAddOnChooseDirectoryDialog::DirectoryMessageBox(CWnd *pcwParent, const CString &csDirectory, int nFormatID, int nTitleID, UINT nType, WORD wFlags /* = 0 */, int nDefaultResult /* = IDOK */)
{
   int nResult = nDefaultResult;
   
   if ((wFlags & OPTION_quiet) == 0)
   {
      CString csTitle;
      CString csFormat;
      CString csMessage;
            
      TRY
      {
         csTitle.LoadString(nTitleID);
         csFormat.LoadString(nFormatID);
         wsprintf(csMessage.GetBuffer(1024), csFormat, (LPCSTR)csDirectory);
         csMessage.ReleaseBuffer();
      }
      CATCH_ALL(e)
      {
         csTitle.Empty();
         csFormat.Empty();
         csMessage.Empty();
         
         THROW_LAST();
      }
      END_CATCH_ALL
            
      nResult = pcwParent->MessageBox(csMessage, csTitle, nType);
   }
   
   return nResult;
}

void CAddOnChooseDirectoryDialog::FreeAllocatedData(void)
{
   // free up any allocated memory for exception processing
   m_csDirectory.Empty();
}

/////////////////////////////////////////////////////////////////////////////
// CAddOnChooseDirectoryDialog message handlers

BOOL CAddOnChooseDirectoryDialog::OnInitDialog()
{
   CFileDialog::OnInitDialog();
   
   ((CEdit *)GetDlgItem(IDC_CHOOSE_DIRECTORY_NAME))->LimitText(256-1);
   
   return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAddOnChooseDirectoryDialog::OnDestroy()
{
   CFileDialog::OnDestroy();
}
