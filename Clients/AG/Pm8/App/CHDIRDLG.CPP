/*
// $Workfile: CHDIRDLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/CHDIRDLG.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:34   Fred
// Initial revision.
// 
//    Rev 1.3   21 Apr 1997 16:01:46   Jay
// 4.0 changes
// 
//    Rev 1.2   30 Aug 1996 19:46:44   Fred
// Some directory validation done only for WIN16
// 
//    Rev 1.1   30 Aug 1996 13:18:12   Fred
//  
// 
//    Rev 1.0   14 Mar 1996 13:25:56   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:27:14   FRED
// Initial revision.
// 
//    Rev 2.6   24 Jan 1996 08:44:02   JAY
//  
// 
//    Rev 2.5   23 Jan 1996 12:32:40   JAY
//  
// 
//    Rev 2.4   13 Oct 1995 14:20:14   FRED
// 32 bit changes
// 
//    Rev 2.3   07 Aug 1995 17:13:56   FRED
// Publishing Suite Install
// 
//    Rev 2.2   09 Jun 1995 07:43:56   FRED
// Sound Manager changes
// 
//    Rev 2.1   08 Feb 1995 13:34:32   JAY
// Reverted. New series.
// 
//    Rev 1.1   17 Jun 1994 16:59:32   FRED
// Adds sound buttons.
// 
// 
//    Rev 1.0   15 Jun 1994 19:01:38   FRED
// Initial revision.
*/ 

#include <dos.h>

#include "stdafx.h"
#include "installr.h"	// needed for IDS_... (string resource ID's)
#include "util.h"
#include "pmwdlg.h"
#include "chdirdlg.h"
#include "pmwcfg.h"
#include <afxpriv.h>		// for WM_HELPHITTEST

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChooseDirectoryDialog dialog

CChooseDirectoryDialog::CChooseDirectoryDialog(int IDD, CWnd* pParent /*=NULL*/)
	: CFileDialog(
			TRUE, 						// bOpenFileDialog
			NULL, 						// lpszDefExt
			"dummy",						// lpszFileName
			OFN_ENABLETEMPLATE   |	// dwFlags
			OFN_HIDEREADONLY     |
			OFN_NOTESTFILECREATE |
			OFN_NOVALIDATE,
			NULL,							// lpszFilter
			pParent)						// pParentWnd
{
	//{{AFX_DATA_INIT(CChooseDirectoryDialog)
	//}}AFX_DATA_INIT
	
#ifdef WIN32
	m_ofn.Flags &= ~OFN_EXPLORER;
#endif

	/*
	// Enable our template.
	*/

	m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD);
	m_ofn.hInstance = AfxGetInstanceHandle();
	
	/*
	// Initialize our data.
	*/
	
	m_csDirectory.Empty();
	m_fSuccess = FALSE;
	m_nIDD = IDD;
}

void CChooseDirectoryDialog::DoDataExchange(CDataExchange* pDX)
{
	CFileDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChooseDirectoryDialog)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChooseDirectoryDialog, CFileDialog)
	//{{AFX_MSG_MAP(CChooseDirectoryDialog)
	ON_WM_DESTROY()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
END_MESSAGE_MAP()

void CChooseDirectoryDialog::OnLBSelChangedNotify(UINT nIDBox, UINT iCurSel, UINT nCode)
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

BOOL CChooseDirectoryDialog::OnFileNameOK()
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

BOOL CChooseDirectoryDialog::GetDirectory(WORD wFlags /* = 0 */)
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

BOOL CChooseDirectoryDialog::ValidateDirectory(const CString &csDirectory, WORD wFlags)
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
					IDS_ErrCantCreateDirectory,
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
	
	fResult = TRUE;
	
cleanup:

	return fResult;
}

BOOL CChooseDirectoryDialog::DirectoryNameIsValid(const CString &csDirectory)
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

WORD CChooseDirectoryDialog::CheckDirectory(const CString &csDirectory, WORD wFlags)
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

int CChooseDirectoryDialog::DirectoryMessageBox(CWnd *pcwParent, const CString &csDirectory, int nFormatID, int nTitleID, UINT nType, WORD wFlags /* = 0 */, int nDefaultResult /* = IDOK */)
{
	int nResult = nDefaultResult;
	
	if ((wFlags & OPTION_quiet) == 0)
	{
		CString csTitle;
		CString csFormat;
		CString csMessage;
				
		TRY
		{
			LoadConfigurationString(nTitleID, csTitle);
			LoadConfigurationString(nFormatID, csFormat);
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

void CChooseDirectoryDialog::FreeAllocatedData(void)
{
	// free up any allocated memory for exception processing
	m_csDirectory.Empty();
}

/////////////////////////////////////////////////////////////////////////////
// CChooseDirectoryDialog message handlers

BOOL CChooseDirectoryDialog::OnInitDialog()
{
	CFileDialog::OnInitDialog();
	
	standard_dialog_startup(this, IDD_CHOOSE_DIRECTORY);
	
	((CEdit *)GetDlgItem(IDC_CHOOSE_DIRECTORY_NAME))->LimitText(256-1);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CChooseDirectoryDialog::OnDestroy()
{
	standard_dialog_shutdown(this, IDD_CHOOSE_DIRECTORY);

	CFileDialog::OnDestroy();
}

LRESULT CChooseDirectoryDialog::OnHelpHitTest(WPARAM, LPARAM lParam)
{
	return standard_dialog_hit_test(this, IDD_CHOOSE_DIRECTORY, lParam);
}


void CChooseDirectoryDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	standard_dialog_activate(m_nIDD, nState, pWndOther, bMinimized);
	CFileDialog::OnActivate(nState, pWndOther, bMinimized);
}
