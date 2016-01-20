/*
// $Workfile: IDSKSPC.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/IDSKSPC.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:46   Fred
// Initial revision.
// 
//    Rev 1.6   18 Jul 1997 13:48:14   Jay
// Can't install to windows or system directory
// 
//    Rev 1.5   08 Jul 1997 12:42:28   Fred
// Moved strings to resource file
// 
//    Rev 1.4   21 Apr 1997 16:01:48   Jay
// 4.0 changes
// 
//    Rev 1.3   30 Aug 1996 19:33:24   Fred
// Works with UNC and long file names
// 
//    Rev 1.2   15 May 1996 08:45:52   Jay
// From PMW2
// 
//    Rev 2.5   08 May 1996 17:03:36   FRED
// Can't install into root directory
// 
//    Rev 2.4   26 Apr 1996 18:09:34   FRED
// More demo stuff
// 
//    Rev 2.3   24 Jan 1996 08:43:42   JAY
//  
// 
//    Rev 2.2   07 Aug 1995 17:14:54   FRED
// Publishing Suite Install
// 
//    Rev 2.1   08 Feb 1995 13:37:00   JAY
// Reverted. New series.
// 
//    Rev 1.2   22 Jun 1994 08:53:30   FRED
// Uses Util::MessageBox().
// 
// 
//    Rev 1.1   16 Jun 1994 13:44:46   FRED
// Removed calls to ls().
// 
// 
//    Rev 1.0   15 Jun 1994 19:01:38   FRED
// Initial revision.
*/ 

#include "stdafx.h"

#include "install.h"
#include "chdirdlg.h"
#include "util.h"
#include "pmwdlg.h"
#include "idiskspc.h"
#include "pmwcfg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInstallDiskSpaceDialog dialog


CInstallDiskSpaceDialog::CInstallDiskSpaceDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CInstallDiskSpaceDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInstallDiskSpaceDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CInstallDiskSpaceDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstallDiskSpaceDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

int CInstallDiskSpaceDialog::CheckDiskSpace(const CString &csDirectory, DWORD dwNeededSize)
{
	int nResult = IDOK;
	
	CString csFormat;
	
	TRY
	{
		// If we are installing a demo, check if we can install into the directory.
		if ((GetInstallApp()->GetInstallInfo()->m_nInstallConfiguration == CPrintMasterConfiguration::Demo)
		 && (!GetInstallApp()->DemoCanBeInstalledIntoDirectory(csDirectory)))
		{
			LoadConfigurationString(IDS_CantInstallDemoToDirectory, csFormat);
			Util::MessageBox(
					MB_OK|MB_DEFBUTTON1,
					(UINT)-1,
					(LPCSTR)csFormat,
					(LPCSTR)csDirectory);
			nResult = IDCANCEL;
		}
		
		// Make sure the directory is not a root directory.
		else if (Util::IsRootDirectory(csDirectory))
		{
			LoadConfigurationString(IDS_CantInstallToRootDirectory, csFormat);
			Util::MessageBox(
					MB_OK|MB_DEFBUTTON1,
					(UINT)-1,
					(LPCSTR)csFormat,
					(LPCSTR)csDirectory);
			nResult = IDCANCEL;
		}

		// Make sure the directory is not the Windows directory.
		else if (Util::IsWindowsDirectory(csDirectory))
		{
			LoadConfigurationString(IDS_CantInstallToWindowsDirectory, csFormat);
			Util::MessageBox(
					MB_OK|MB_DEFBUTTON1,
					(UINT)-1,
					(LPCSTR)csFormat,
					(LPCSTR)csDirectory);
			nResult = IDCANCEL;
		}

		// Make sure the directory is not the Windows system directory.
		else if (Util::IsSystemDirectory(csDirectory))
		{
			LoadConfigurationString(IDS_CantInstallToSystemDirectory, csFormat);
			Util::MessageBox(
					MB_OK|MB_DEFBUTTON1,
					(UINT)-1,
					(LPCSTR)csFormat,
					(LPCSTR)csDirectory);
			nResult = IDCANCEL;
		}

		else
		{
			// If we are installing into a 
			// get the available disk space
			DWORD dwAvailableSize = Util::GetAvailableDiskSpace(csDirectory);
		
			if ((dwAvailableSize != DISK_SPACE_NOT_AVAILABLE)
			 && (dwNeededSize != DISK_SPACE_NOT_AVAILABLE))
			{
				// if the available space is less than we need, tell the user
				if (dwAvailableSize < dwNeededSize)
				{
					m_dwNeededSize = dwNeededSize;
					m_dwAvailableSize = dwAvailableSize;
					m_csDirectory = csDirectory;
					
					if (Util::DirectoryDrive(m_csDirectory, m_csDrive))
					{
						if ((m_csDrive.GetLength() >= 2) && (m_csDrive[1] == ':'))
						{
							m_csDrive.MakeUpper();
							char cDriveLetter = m_csDrive[0];
							LoadConfigurationString(IDS_DriveFormatLowercase, csFormat);
							wsprintf(m_csDrive.GetBuffer(1024), csFormat, cDriveLetter);
							m_csDrive.ReleaseBuffer();
							m_csDrive += ':';
						}
					}
					
					if ((!m_csDirectory.IsEmpty())
					 && (!m_csDrive.IsEmpty()))
					{
						nResult = DoModal();
					}
				}
			}
			
			if (nResult == IDOK)
			{
				// check if the directory exists
				if (Util::DirectoryExists(csDirectory))
				{
					// the directory already exists, the user must confirm overwrite
					LoadConfigurationString(IDS_DirectoryAlreadyExistsFormat, csFormat);
					if (Util::MessageBox(
							MB_YESNO|MB_DEFBUTTON1,
							(UINT)-1,
							(LPCSTR)csFormat,
							(LPCSTR)csDirectory,
							(LPCSTR)csDirectory) == IDNO)
					{
						nResult = IDCANCEL;
					}
				}
			
				if (nResult == IDOK)
				{
					// check if the directory can be created
					if (!Util::MakeDirectory(csDirectory))
					{
						LoadConfigurationString(IDS_ErrCantCreateDirectory, csFormat);
						Util::MessageBox(
							MB_OK,
							(UINT)-1,
							(LPCSTR)csFormat,
							(LPCSTR)csDirectory);
						nResult = IDCANCEL;
					}
				}
			}
		}
	}
	CATCH_ALL(e)
	{
		csFormat.Empty();
		
		THROW_LAST();
	}
	END_CATCH_ALL
	
	return nResult;
}

void CInstallDiskSpaceDialog::FreeAllocatedData(void)
{
	// free up any allocated memory for exception processing
	m_csDirectory.Empty();
	m_csDrive.Empty();
}

BEGIN_MESSAGE_MAP(CInstallDiskSpaceDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CInstallDiskSpaceDialog)
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CInstallDiskSpaceDialog::UpdateDiskSpace(void)
{
	CString csFormat;
	CString csMessage;
	CString csAdditional;
	
	TRY
	{
		LoadConfigurationString(IDS_NotEnoughDiskSpaceFormat1, csFormat);
		wsprintf(csMessage.GetBuffer(1024), csFormat, (LPCSTR)m_csDrive);
		csMessage.ReleaseBuffer();
		SetDlgItemText(IDC_NOT_ENOUGH_DISK_SPACE_1, csMessage);
		
		LoadConfigurationString(IDS_NotEnoughDiskSpaceFormat2, csFormat);
		Util::FormatDouble(MAKE_DOUBLE(m_dwNeededSize-m_dwAvailableSize), 2, csAdditional);
		wsprintf(csMessage.GetBuffer(1024), csFormat, (LPCSTR)csAdditional, (LPCSTR)m_csDrive);
		csMessage.ReleaseBuffer();
		SetDlgItemText(IDC_NOT_ENOUGH_DISK_SPACE_2, csMessage);
	}
	CATCH_ALL(e)
	{
		csFormat.Empty();
		csMessage.Empty();
		csAdditional.Empty();
		
		THROW_LAST();
	}
	END_CATCH_ALL
}

/////////////////////////////////////////////////////////////////////////////
// CInstallDiskSpaceDialog message handlers

BOOL CInstallDiskSpaceDialog::OnInitDialog()
{
	CPmwDialog::OnInitDialog();
	
	m_fAborted = FALSE;
	
	UpdateDiskSpace();
		
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CInstallDiskSpaceDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CPmwDialog::OnActivate(nState, pWndOther, bMinimized);
	
	/*
	// The user may have left and freed some disk space, so we need to get the new
	// disk space and check.
	*/
	
	if (!m_fAborted)
	{
		if ((nState == WA_ACTIVE) || (nState == WA_CLICKACTIVE))
		{
			DWORD dwAvailableSize = Util::GetAvailableDiskSpace(m_csDirectory);
				
			if (dwAvailableSize == DISK_SPACE_NOT_AVAILABLE)
			{
				/*
				// What da hey? Perhaps a connected drive became
				// unconnected. Do the default cancel action.
				*/
				
				OnCancel();
			}
			
			m_dwAvailableSize = dwAvailableSize;
					
			if (m_dwAvailableSize < m_dwNeededSize)
			{
				/*
				// There is still not enough disk space. Update the current dialog
				// with any new numbers.
				*/
				
				UpdateDiskSpace();
			}
			
			else
			{
				/*
				// Amazing! There is now enough disk space! Inform the user and
				// wait for confirmation to proceed.
				*/
				
				CString csFormat;
				CString csMessage;
							
				TRY
				{
					LoadConfigurationString(IDS_NowEnoughDiskSpaceFormat, csFormat);
					wsprintf(csMessage.GetBuffer(1024), csFormat, (LPCSTR)m_csDrive, (LPCSTR)m_csDrive);
					csMessage.ReleaseBuffer();
				}
				CATCH_ALL(e)
				{
					csFormat.Empty();
					csMessage.Empty();
					
					THROW_LAST();
				}
				END_CATCH_ALL
				
				m_fAborted = TRUE;
				
				if (AfxMessageBox(csMessage, MB_YESNO) == IDYES)
				{
					EndDialog(IDOK);
				}
				
				else
				{
					EndDialog(IDCANCEL);
				}
			}
		}
	}
}
