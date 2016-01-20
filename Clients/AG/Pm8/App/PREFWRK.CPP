/*
// $Workfile: PREFWRK.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:10p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/PREFWRK.CPP $
// 
// 1     3/03/99 6:10p Gbeddow
// 
// 3     9/21/98 5:27p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 2     8/07/98 5:11p Mwilson
// added helpful hint to workspace dialog
// 
//    Rev 1.0   14 Aug 1997 15:24:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:34:14   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:30:34   FRED
// Initial revision.
// 
//    Rev 1.3   24 Jan 1996 10:03:34   JAY
// New string management in Configuration
// 
//    Rev 1.2   02 Sep 1995 15:07:22   FRED
// More changes to make help work
// 
//    Rev 1.1   01 Sep 1995 17:50:00   FRED
// Removed workdlg.h include
// 
//    Rev 1.0   16 Jul 1995 15:08:18   FRED
// Initial revision.
*/

#include "stdafx.h"

#include "pmw.h"
#include "prefwrk.h"

#include "system.h"
#include "util.h"
#include "pmwcfg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define IDM_VALIDATE_WORKSPACE_SIZE	(WM_USER+1)

/////////////////////////////////////////////////////////////////////////////
// CPreferencesWorkspacePage property page

CPreferencesWorkspacePage::CPreferencesWorkspacePage()
	: CPmwPropertyPage(CPreferencesWorkspacePage::IDD), m_pCache(NULL)
{
	//{{AFX_DATA_INIT(CPreferencesWorkspacePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	m_dwWorkspaceSize = 0;
	m_fRedrawWorkspaceSize = FALSE;
	m_fRedrawDiskRequirements = FALSE;
}

CPreferencesWorkspacePage::~CPreferencesWorkspacePage()
{
}

void CPreferencesWorkspacePage::DoDataExchange(CDataExchange* pDX)
{
	CPmwPropertyPage::DoDataExchange(pDX);
	
	//{{AFX_DATA_MAP(CPreferencesWorkspacePage)
	DDX_Control(pDX, IDC_WORKSPACE_SIZE, m_workspace_edit);
	//}}AFX_DATA_MAP
	
	if (pDX->m_bSaveAndValidate)
	{
		if (!OnValidateWorkspaceSize(0, 0L))
		{
			pDX->Fail();
		}
	}
	else
	{
	}
}

BEGIN_MESSAGE_MAP(CPreferencesWorkspacePage, CPmwPropertyPage)
	//{{AFX_MSG_MAP(CPreferencesWorkspacePage)
	ON_MESSAGE(IDM_VALIDATE_WORKSPACE_SIZE, OnValidateWorkspaceSize)
	ON_WM_VSCROLL()
	ON_EN_SETFOCUS(IDC_WORKSPACE_SIZE, OnSetfocusWorkspaceSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPreferencesWorkspacePage::FreeAllocatedData(void)
{
	// free up any allocated memory for exception processing
	m_csWorkspaceDirectory.Empty();
}

/*
// Get the recommended workspace size according to the current environment.
*/

DWORD CPreferencesWorkspacePage::GetRecommendedWorkspaceSize(CString& csDirectory, DWORD dwCachePhysicalSize)
{
	/*
	// Get the recommended size for the workspace as follows:
	//
	// 1) If the installation directory is on a drive for which
	//		we cannot determine the free disk space, use the default
	//		size of 9 Mb.
	//
	// 2) Otherwise, use half of the free disk space after subtracting
	//		out the program size. This value is clipped to a minimum of
	//		1 MB and a maximum of 9 Mb. The value is rounded to the
	//		nearest .5 Mb.
	*/

	DWORD dwResult = DEFAULT_WORKSPACE_SIZE;

	// get the available free disk space
	DWORD dwAvailableSize = Util::GetAvailableDiskSpace(csDirectory);

	if (dwAvailableSize != DISK_SPACE_NOT_AVAILABLE)
	{
	/* Get the space currently in use by the CD cache. */

		dwAvailableSize += dwCachePhysicalSize;

		// use half of the available space
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

	return dwResult;
}

void CPreferencesWorkspacePage::RedrawInterface(void)
{
	/*
	// Redraw those interface elements that have changed.
	*/

	CString csSize;
	CString csValue;

	TRY
	{
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
			if (m_dwWorkspaceSize != DISK_SPACE_NOT_AVAILABLE)
			{
				if (m_pCache != NULL)
				{
					DWORD dwAvailableSize = Util::GetAvailableDiskSpace(m_csWorkspaceDirectory);
					DWORD dwRecommendedSize = GetRecommendedWorkspaceSize(m_csWorkspaceDirectory, m_pCache->get_physical_size());
	
					// display the recommended size
					double dRecommendedSize = MAKE_DOUBLE(dwRecommendedSize);
					Util::FormatDouble(dRecommendedSize, 2, csValue);
					csValue += " Mb";
					SetDlgItemText(IDC_RECOMMENDED_SIZE, csValue);
	
					// display available disk space
					double dAvailableSize;
	
					if (dwAvailableSize == DISK_SPACE_NOT_AVAILABLE)
					{
						LoadConfigurationString(IDS_DiskSpaceNotAvailable, csValue);
					}
	
					else
					{
						dAvailableSize = MAKE_DOUBLE(dwAvailableSize);
						Util::FormatDouble(dAvailableSize, 2, csValue);
						csValue += " Mb";
					}
	
					SetDlgItemText(IDC_CURRENT_DISK_SPACE, csValue);
				}
			}

			m_fRedrawDiskRequirements = FALSE;
		}
	}
	CATCH_ALL(e)
	{
		csSize.Empty();
		csValue.Empty();

		THROW_LAST();
	}
	END_CATCH_ALL
}

void CPreferencesWorkspacePage::SetWorkspaceSize(DWORD dwSize)
{
	if (dwSize != DISK_SPACE_NOT_AVAILABLE)
	{
		// update object members
		m_dwWorkspaceSize = dwSize;
		m_fRedrawWorkspaceSize = TRUE;
		m_fRedrawDiskRequirements = TRUE;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CPreferencesWorkspacePage message handlers

BOOL CPreferencesWorkspacePage::OnSetActive()     // called when this page gets the focus
{
	BOOL fResult = CPmwPropertyPage::OnSetActive();

	// Redraw the disk space values in case they have changed beneath us.
		
	m_fRedrawDiskRequirements = TRUE;
	RedrawInterface();
	
	return fResult;
}

BOOL CPreferencesWorkspacePage::OnInitDialog()
{
	// Create the directory name.

	TRY
	{
		if (m_pCache != NULL)
		{
			m_csWorkspaceDirectory = GetGlobalPathManager()->ExpandPath(m_pCache->cache_name());
	
			// We want to strip off the base name.
			// We assume that there is a base name to begin with.
	
			int nIndex;
	
			if ((nIndex = m_csWorkspaceDirectory.ReverseFind('\\')) != -1)
			{
				// Leave the slash off.
				nIndex--;
			}
			else
			{
				nIndex = m_csWorkspaceDirectory.ReverseFind(':');
			}
	
			// We want to strip off the base name.
	
			m_csWorkspaceDirectory = m_csWorkspaceDirectory.Left(nIndex+1);
		}
	}
	CATCH_ALL(e)
	{
		// Make sure we're empty.
		m_csWorkspaceDirectory.Empty();
	}
	END_CATCH_ALL

	// Get the current size.

	if (m_pCache != NULL)
	{
		SetWorkspaceSize(m_pCache->cache_size());
	}

	// Do the default initialization.

	CPmwPropertyPage::OnInitDialog();
	
	// Limit the text size.

	m_workspace_edit.LimitText(6);		// a reasonable limit
	m_workspace_edit.SetFocus();
	GET_PMWAPP()->ShowHelpfulHint("Preferences", this);
	
	return FALSE;		// return TRUE  unless you set the focus to a control
}

void CPreferencesWorkspacePage::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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
			m_workspace_edit.SetSel(0, -1);
		}
	}
}

LRESULT CPreferencesWorkspacePage::OnValidateWorkspaceSize(WPARAM wParam, LPARAM lParam)
{
	CString csSize;
	CString csFormat;
	CString csMessage;
	CString csError;

	LRESULT lResult = 0;

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
			GetConfiguration()->MessageBox(IDS_ErrInvalidWorkspaceSize, 0, MB_OK|MB_ICONEXCLAMATION);
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
			}
			else
			{
				// Check if the requested size is less that the avilable space.

				if (m_pCache != NULL)
				{
					DWORD dwAvailableSize = Util::GetAvailableDiskSpace(m_csWorkspaceDirectory);

					dwAvailableSize += m_pCache->get_physical_size();

					if (dwSize > dwAvailableSize)
					{
						LoadConfigurationString(IDS_ErrWorkspaceExceedsDiskSpace, csMessage);
						fIsValid = FALSE;
					}
				}
			}

			if (!fIsValid)
			{
				AfxMessageBox(csMessage, MB_OK|MB_ICONEXCLAMATION);
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
			GetDlgItem(IDC_WORKSPACE_SIZE)->SetFocus();
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

	return lResult;
}

void CPreferencesWorkspacePage::OnSetfocusWorkspaceSize()
{
	m_workspace_edit.SetSel(0, -1);
}
