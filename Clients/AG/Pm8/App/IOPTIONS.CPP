/*
// $Workfile: IOPTIONS.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/IOPTIONS.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 2     6/24/98 8:26a Jayn
// 
//    Rev 1.0   14 Aug 1997 15:21:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:52   Fred
// Initial revision.
// 
//    Rev 1.2   08 Jul 1997 12:42:32   Fred
// Moved strings to resource file
// 
//    Rev 1.1   23 Apr 1996 08:25:24   Jay
//  
// 
//    Rev 2.6   19 Apr 1996 10:05:08   JAY
// Now suuports previous installation directory.
// 
//    Rev 2.5   02 Feb 1996 13:07:28   JAY
//  
// 
//    Rev 2.4   30 Jan 1996 08:35:34   JAY
// Now calls RedrawInterface() at end of OnInitDialog.
// 
//    Rev 2.3   24 Jan 1996 08:43:42   JAY
//  
// 
//    Rev 2.2   07 Aug 1995 17:14:56   FRED
// Publishing Suite Install
// 
//    Rev 2.1   08 Feb 1995 13:37:36   JAY
// Reverted. New series.
// 
//    Rev 1.5   22 Jun 1994 11:22:36   FRED
// Fixed bug where dialog was not properly initialized
// if the source and default destination were on the
// same drive.
// 
// 
//    Rev 1.4   22 Jun 1994 08:33:46   FRED
// Does not move focus to "continue" when enter
// is pressed on workspace size edit field.
// 
// 
//    Rev 1.3   21 Jun 1994 15:37:38   FRED
// Confirms Cancel.
// 
// 
//    Rev 1.2   16 Jun 1994 13:45:32   FRED
// Removed calls to ls().
// 
// 
//    Rev 1.1   15 Jun 1994 18:57:40   FRED
// Uses Util::
// 
// 
//    Rev 1.0   15 Jun 1994 14:44:50   FRED
// Initial revision.
*/ 

#include "stdafx.h"

#include "install.h"
#include "chdirdlg.h"
#include "idiskspc.h"
#include "spinbutt.h"
#include "pmwdlg.h"
#include "cpfl.h"
#include "util.h"
#include "ioptions.h"
#include "pmwcfg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define IDM_VALIDATE_WORKSPACE_SIZE	(WM_USER+1)

/////////////////////////////////////////////////////////////////////////////
// CInstallOptionsDialog dialog


CInstallOptionsDialog::CInstallOptionsDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CInstallOptionsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInstallOptionsDialog)
	//}}AFX_DATA_INIT
	
	m_csAvailableDiskSpaceName = "";
	m_csInstallDirectory = "";
	m_dwProgramSize = 0;
	m_dwWorkspaceSize = 0;
	m_fValidateWorkspaceSize = TRUE;
	m_fCancelPressed = FALSE;
	m_fRedrawInstallDirectory = FALSE;
	m_fRedrawWorkspaceSize = FALSE;
	m_fRedrawDiskRequirements = FALSE;
}

void CInstallOptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstallOptionsDialog)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInstallOptionsDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CInstallOptionsDialog)
	ON_BN_CLICKED(IDC_SET_INSTALL_DIRECTORY, OnSetInstallDirectory)
	ON_WM_ACTIVATE()
	ON_EN_KILLFOCUS(IDC_WORKSPACE_SIZE, OnKillfocusWorkspaceSize)
	ON_MESSAGE(IDM_VALIDATE_WORKSPACE_SIZE, OnValidateWorkspaceSize)
	ON_WM_VSCROLL()
	ON_WM_PARENTNOTIFY()
	ON_EN_SETFOCUS(IDC_WORKSPACE_SIZE, OnSetfocusWorkspaceSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CInstallOptionsDialog::FreeAllocatedData(void)
{
	// free up any allocated memory for exception processing
	m_csAvailableDiskSpaceName.Empty();
	m_csInstallDirectory.Empty();
}

/////////////////////////////////////////////////////////////////////////////
// CInstallOptionsDialog message handlers

BOOL CInstallOptionsDialog::OnInitDialog()
{
	CPmwDialog::OnInitDialog();

	CString csDefaultDirectory;
	
	TRY
	{
		((CEdit *)GetDlgItem(IDC_WORKSPACE_SIZE))->LimitText(6);		// a reasonable limit
	
		csDefaultDirectory = m_csPreviousInstallDirectory;
		if (csDefaultDirectory.IsEmpty())
		{
			LoadConfigurationString(IDS_DefaultInstallDirectory, csDefaultDirectory);
		}
		SetInstallDirectory(csDefaultDirectory, TRUE);
	}
	CATCH_ALL(e)
	{
		csDefaultDirectory.Empty();
		
		THROW_LAST();
	}
	END_CATCH_ALL

	RedrawInterface();

	return TRUE;  // return TRUE  unless you set the focus to a control
}


/////////////////////////////////////////////////////////////////////////////
// CInstallOptionsDialog helper functions

void CInstallOptionsDialog::RedrawInterface(void)
{
	/*
	// Redraw those interface elements that have changed.
	*/
	
	CString csDrive;
	CString csFormat;
	CString csName;
	CString csSize;
	CString csValue;
	CString csSizeFormat;
	
	TRY
	{
		// installation directory changed
		if (m_fRedrawInstallDirectory)
		{
			SetDlgItemText(IDC_INSTALL_DIRECTORY, m_csInstallDirectory);
				
			if (Util::DirectoryDrive(m_csInstallDirectory, csDrive))
			{
				if ((csDrive.GetLength() == 2) && (csDrive[1] == ':'))
				{
					csDrive.MakeUpper();
					char cDriveLetter = csDrive[0];
					LoadConfigurationString(IDS_DriveFormat, csFormat);
					wsprintf(csDrive.GetBuffer(256), csFormat, cDriveLetter);
					csDrive.ReleaseBuffer();
				}
							
				LoadConfigurationString(IDS_AvailableDiskSpaceNameFormat, csFormat);
				wsprintf(csName.GetBuffer(256), csFormat, (LPCSTR)csDrive);
				csName.ReleaseBuffer();
				SetDlgItemText(IDC_AVAILABLE_DISK_SPACE_NAME, csName);
			}
				
			m_fRedrawInstallDirectory = FALSE;
		}

		// workspace size changed
		if (m_fRedrawWorkspaceSize)
		{
			if (m_dwWorkspaceSize != DISK_SPACE_NOT_AVAILABLE)
			{
				// update the interface
				Util::FormatDouble(((double)m_dwWorkspaceSize)/1000000.0, -2, csSize);
				SetDlgItemText(IDC_WORKSPACE_SIZE, csSize);
			}
				
			m_fRedrawWorkspaceSize = FALSE;
		}
			
		// disk requirements changed
		if (m_fRedrawDiskRequirements)
		{
			if ((m_dwProgramSize != DISK_SPACE_NOT_AVAILABLE)
			 && (m_dwWorkspaceSize != DISK_SPACE_NOT_AVAILABLE))
			{
				LoadConfigurationString(IDS_DiskSpaceFormat, csSizeFormat);

				DWORD dwAvailableSize = Util::GetAvailableDiskSpace(m_csInstallDirectory);
						
				// display the program size
				double dProgramSize = MAKE_DOUBLE(m_dwProgramSize);
				Util::FormatDouble(dProgramSize, 2, csValue);
				csSize.Format(csSizeFormat, csValue);
				SetDlgItemText(IDC_PROGRAM_DISK_SPACE, csSize);
						
				// display the workspace size
				double dWorkspaceSize = MAKE_DOUBLE(m_dwWorkspaceSize);
				Util::FormatDouble(dWorkspaceSize, 2, csValue);
				csSize.Format(csSizeFormat, csValue);
				SetDlgItemText(IDC_WORKSPACE_DISK_SPACE, csSize);
			
				// display total size			
				double dTotalSize = dProgramSize+dWorkspaceSize;
				Util::FormatDouble(dTotalSize, 2, csValue);
				csSize.Format(csSizeFormat, csValue);
				SetDlgItemText(IDC_TOTAL_DISK_SPACE, csSize);
						
				// display available disk space
				double dAvailableSize;
						
				if (dwAvailableSize == DISK_SPACE_NOT_AVAILABLE)
				{
					LoadConfigurationString(IDS_DiskSpaceNotAvailable, csSize);
				}
						
				else
				{
					dAvailableSize = MAKE_DOUBLE(dwAvailableSize);
					Util::FormatDouble(dAvailableSize, 2, csValue);
					csSize.Format(csSizeFormat, csValue);
				}
						
				SetDlgItemText(IDC_AVAILABLE_DISK_SPACE, csSize);
						
				// if available is less than required, put up an indicator
				if ((dwAvailableSize == DISK_SPACE_NOT_AVAILABLE) || (dTotalSize < dAvailableSize))
				{
					csSize.Empty();
				}
						
				else
				{
					LoadConfigurationString(IDS_NotEnoughDiskSpaceIndicator, csSize);
				}
						
				SetDlgItemText(IDC_NOT_ENOUGH_AVAILABLE_DISK_SPACE, csSize);
			}
				
			m_fRedrawDiskRequirements = FALSE;
		}
	}
	CATCH_ALL(e)
	{
		csDrive.Empty();
		csFormat.Empty();
		csName.Empty();
		csSize.Empty();
		csValue.Empty();
		csSizeFormat.Empty();
				
		THROW_LAST();
	}
	END_CATCH_ALL
}

void CInstallOptionsDialog::SetInstallDirectory(const CString &csDirectory, BOOL fForceDriveChange /* = FALSE*/)
{
	/*
	// Figure out if the drive changed. If so, then we want to recompute the recommended
	// workspace size.
	*/
					
	CString csOldDrive;
	CString csNewDrive;
	CString csDrive;
	CString csFormat;
	CString csName;
	
	TRY
	{
		BOOL fDriveChanged = TRUE;
					
		if ((!fForceDriveChange)
		 && (Util::DirectoryDrive(m_csInstallDirectory, csOldDrive))
		 && (Util::DirectoryDrive(csDirectory, csNewDrive))
		 && (csOldDrive.CompareNoCase(csNewDrive) == 0))
		{
			fDriveChanged = FALSE;
		}
		
		/*
		// Update the installation directory and other dependent items in the dialog.
		*/
		
		// update object member
		m_csInstallDirectory = csDirectory;
		m_fRedrawInstallDirectory = TRUE;
		
		if (fDriveChanged)
		{
			// since the drive changed, recompute some items that are based on cluster size
			DWORD dwProgramSize = GetInstallApp()->GetProgramSize(m_csInstallDirectory);
				
			if (dwProgramSize != DISK_SPACE_NOT_AVAILABLE)
			{
				m_dwProgramSize = dwProgramSize;
				m_fRedrawDiskRequirements = TRUE;
			}
				
			SetWorkspaceSize(GetInstallApp()->GetRecommendedWorkspaceSize(m_csInstallDirectory));
		}
	}
	CATCH_ALL(e)
	{
		csOldDrive.Empty();
		csNewDrive.Empty();
		csDrive.Empty();
		csFormat.Empty();
		csName.Empty();
		
		THROW_LAST();
	}
	END_CATCH_ALL
}

void CInstallOptionsDialog::SetWorkspaceSize(DWORD dwSize)
{
	if (dwSize != DISK_SPACE_NOT_AVAILABLE)
	{
		// update object member
		m_dwWorkspaceSize = dwSize;
		m_fRedrawWorkspaceSize = TRUE;
		m_fRedrawDiskRequirements = TRUE;
	}
}

void CInstallOptionsDialog::OnSetInstallDirectory()
{
	/*
	// Ask the user for a new directory.
	*/
	
	CChooseDirectoryDialog ChooseDirectoryDlg(IDD_CHOOSE_DIRECTORY, this);
	
	TRY
	{
		ChooseDirectoryDlg.m_csDirectory = m_csInstallDirectory;
		
		if (ChooseDirectoryDlg.GetDirectory(
											CChooseDirectoryDialog::OPTION_no_exists_check |
											CChooseDirectoryDialog::OPTION_expand))
		{
			SetInstallDirectory(ChooseDirectoryDlg.m_csDirectory);
			RedrawInterface();
		}
	}
	CATCH_ALL(e)
	{
		ChooseDirectoryDlg.FreeAllocatedData();
		
		THROW_LAST();
	}
	END_CATCH_ALL
}

void CInstallOptionsDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	/*
	// The window has been activated. Update the disk space requirements
	// in case they changed.
	*/
	
	CPmwDialog::OnActivate(nState, pWndOther, bMinimized);
	
	if ((nState == WA_ACTIVE) || (nState == WA_CLICKACTIVE))
	{
		m_fRedrawDiskRequirements = TRUE;
		RedrawInterface();
	}
}

void CInstallOptionsDialog::OnKillfocusWorkspaceSize()
{
	if (m_fCancelPressed)
	{
		m_fCancelPressed = FALSE;
	}
	
	else
	{
		PostMessage(IDM_VALIDATE_WORKSPACE_SIZE, 0, 0L);
	}
}

void CInstallOptionsDialog::OnOK()
{
	if (OnValidateWorkspaceSize(0, 0L))
	{
		CInstallDiskSpaceDialog diskspaceDialog;
			
		if (diskspaceDialog.CheckDiskSpace(m_csInstallDirectory, m_dwProgramSize+m_dwWorkspaceSize) == IDOK)
		{
			CPmwDialog::OnOK();
		}
	}
}

void CInstallOptionsDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	DWORD dwNewSize = ((m_dwWorkspaceSize+500000L)/1000000L)*1000000L;
	
	if (nSBCode == SB_LINEUP)
	{
		dwNewSize += 1000000L;
	}
	
	if (nSBCode == SB_LINEDOWN)
	{
		if (dwNewSize >= 1000000L)
		{
			dwNewSize -= 1000000L;
		}
	}
	
	if (dwNewSize < MINIMUM_WORKSPACE_SIZE)
	{
		dwNewSize = MINIMUM_WORKSPACE_SIZE;
	}
	
	if (dwNewSize > MAXIMUM_WORKSPACE_SIZE)
	{
		dwNewSize = MAXIMUM_WORKSPACE_SIZE;
	}

	if (m_dwWorkspaceSize != dwNewSize)
	{
		SetWorkspaceSize(dwNewSize);
		RedrawInterface();
		
		if (GetFocus() == GetDlgItem(IDC_WORKSPACE_SIZE))
		{
			((CEdit *)GetDlgItem(IDC_WORKSPACE_SIZE))->SetSel(0, -1);
		}
	}
	
	CPmwDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

LRESULT CInstallOptionsDialog::OnValidateWorkspaceSize(WPARAM wParam, LPARAM lParam)
{
	if (!m_fValidateWorkspaceSize)
	{
		return (LRESULT)0;
	}
	
	m_fValidateWorkspaceSize = FALSE;
	
	LRESULT lResult = 0;
	
	CString csSize;
	CString csFormat;
	CString csMessage;
	
	TRY
	{
		GetDlgItemText(IDC_WORKSPACE_SIZE, csSize.GetBuffer(100), 100-1);
		csSize.ReleaseBuffer();

		// get the value that the user entered		
		BOOL fIsValid = TRUE;
		BOOL fSeenDigit = FALSE;
		BOOL fSeenDecimal = FALSE;
		DWORD dwSize = 0L;
		DWORD dwFraction = 1000000L;
				
		for (int i = 0; i < csSize.GetLength(); i++)
		{
			if ((csSize[i] >= '0') && (csSize[i] <= '9'))
			{
				if (!fSeenDecimal)
				{
					dwSize = (dwSize*10L)+(1000000L*(csSize[i]-'0'));
				}
				
				else
				{
					dwFraction /= 10L;
					dwSize += dwFraction*(csSize[i]-'0');
				}
				
				fSeenDigit = TRUE;
			}
			
			else if (csSize[i] == '.')
			{
				if (fSeenDecimal)
				{
					fIsValid = FALSE;
					break;
				}
				
				fSeenDecimal = TRUE;
			}
			
			else if (csSize[i] != ' ')
			{
				fIsValid = FALSE;
				break;
			}
		}
		
		if (!fSeenDigit)
		{
			fIsValid = FALSE;
		}
		
		if (!fIsValid)
		{
			GetConfiguration()->MessageBox(IDS_ErrInvalidWorkspaceSize);
		}
		
		else
		{
			// a number was entered, check if it is with the legal range
			
			DWORD dwMinMaxSize;
				
			if (dwSize < MINIMUM_WORKSPACE_SIZE)
			{
				LoadConfigurationString(IDS_ErrWorkspaceSizeTooSmall, csFormat);
				dwMinMaxSize = MINIMUM_WORKSPACE_SIZE;
				fIsValid = FALSE;
			}
				
			if (dwSize > MAXIMUM_WORKSPACE_SIZE)
			{
				LoadConfigurationString(IDS_ErrWorkspaceSizeTooLarge, csFormat);
				dwMinMaxSize = MAXIMUM_WORKSPACE_SIZE;
				fIsValid = FALSE;
			}
				
			if (!fIsValid)
			{
				wsprintf(csMessage.GetBuffer(1024), csFormat, (int)(dwMinMaxSize/1000000L));
				csMessage.ReleaseBuffer();
				AfxMessageBox(csMessage);
			}
		}
		
		if (fIsValid)
		{
			// the number entered was valid, display it
			SetWorkspaceSize(dwSize);
			RedrawInterface();
			lResult = 1;
		}
		
		else
		{
			// restore the number to its previous value
			m_fRedrawWorkspaceSize = TRUE;
			RedrawInterface();
			
			// reset the focus back to the workspace edit
			if (GetFocus() != GetDlgItem(IDC_WORKSPACE_SIZE))
			{
				GetDlgItem(IDC_WORKSPACE_SIZE)->SetFocus();
			}
			
			else
			{
				OnSetfocusWorkspaceSize();
			}
		}
	}
	CATCH_ALL(e)
	{
		csSize.Empty();
		csFormat.Empty();
		csMessage.Empty();
		
		THROW_LAST();
	}
	END_CATCH_ALL
	
	m_fValidateWorkspaceSize = TRUE;
	
	return lResult;
}

void CInstallOptionsDialog::OnParentNotify(UINT message, LPARAM lParam)
{
	CPmwDialog::OnParentNotify(message, lParam);
	
	if (message == WM_LBUTTONDOWN)
	{
		CRect crCancelButton;
		
		((CButton *)GetDlgItem(IDCANCEL))->GetWindowRect(&crCancelButton);
		
		CPoint pCursor((DWORD)lParam);
		ClientToScreen(&pCursor);
		
		if (crCancelButton.PtInRect(pCursor))
		{
			m_fCancelPressed = TRUE;
		}
	}
}

void CInstallOptionsDialog::OnSetfocusWorkspaceSize()
{
	((CEdit *)GetDlgItem(IDC_WORKSPACE_SIZE))->SetSel(0, -1);
}

void CInstallOptionsDialog::OnCancel()
{
	/*
	// The user wants to cancel.
	//
	// Temporarily disable validating the workspace size until we have confirmed
	// that the user really want to exit. If so, then we leave without validating.
	// Otherwise, we do the validation.
	*/
	
	m_fValidateWorkspaceSize = FALSE;
	
	/*
	// Confirm that the user really does want to cancel.
	*/
	
	if (GetConfiguration()->MessageBox(IDS_ConfirmExitInstallation, 0, MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION) == IDYES)
	{
		CPmwDialog::OnCancel();
	}
	
	else
	{
		m_fValidateWorkspaceSize = TRUE;
		OnValidateWorkspaceSize(0, 0L);
	}
}
