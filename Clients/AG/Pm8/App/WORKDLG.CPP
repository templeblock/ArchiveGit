// workdlg.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"

#ifdef CD_VERSION

#include "system.h"
#include "workdlg.h"
#include "util.h"
#include "pmwcfg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define IDM_VALIDATE_WORKSPACE_SIZE	(WM_USER+1)

/////////////////////////////////////////////////////////////////////////////
// CWorkspacePreferencesDialog dialog

CWorkspacePreferencesDialog::CWorkspacePreferencesDialog(CDCache *cache, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CWorkspacePreferencesDialog::IDD, pParent), m_cache(cache)
{
	//{{AFX_DATA_INIT(CWorkspacePreferencesDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_csAvailableDiskSpaceName = "";
	m_dwWorkspaceSize = 0;
	m_fWorkspaceSizeChanged = FALSE;
	m_fRedrawWorkspaceSize = FALSE;
	m_fRedrawDiskRequirements = FALSE;
}

void CWorkspacePreferencesDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWorkspacePreferencesDialog)
	DDX_Control(pDX, IDC_WORKSPACE_SIZE, m_workspace_edit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWorkspacePreferencesDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CWorkspacePreferencesDialog)
	ON_WM_ACTIVATE()
	ON_EN_KILLFOCUS(IDC_WORKSPACE_SIZE, OnKillfocusWorkspaceSize)
	ON_EN_CHANGE(IDC_WORKSPACE_SIZE, OnChangeWorkspaceSize)
	ON_MESSAGE(IDM_VALIDATE_WORKSPACE_SIZE, OnValidateWorkspaceSize)
	ON_WM_VSCROLL()
	ON_WM_PARENTNOTIFY()
	ON_EN_SETFOCUS(IDC_WORKSPACE_SIZE, OnSetfocusWorkspaceSize)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CWorkspacePreferencesDialog::FreeAllocatedData(void)
{
	// free up any allocated memory for exception processing
	m_csAvailableDiskSpaceName.Empty();
	m_csWorkspaceDirectory.Empty();
}

/////////////////////////////////////////////////////////////////////////////
// CWorkspacePreferencesDialog helper functions

/*
// Get the recommended workspace size according to the current environment.
*/

DWORD GetRecommendedWorkspaceSize(CString& csDirectory, DWORD dwCachePhysicalSize)
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

void CWorkspacePreferencesDialog::RedrawInterface(void)
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
				DWORD dwAvailableSize = Util::GetAvailableDiskSpace(m_csWorkspaceDirectory);
				DWORD dwRecommendedSize = GetRecommendedWorkspaceSize(m_csWorkspaceDirectory,
																						m_cache->get_physical_size());

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

void CWorkspacePreferencesDialog::SetWorkspaceSize(DWORD dwSize)
{
	if (dwSize != DISK_SPACE_NOT_AVAILABLE)
	{
		// update object member
		m_dwWorkspaceSize = dwSize;
		m_fRedrawWorkspaceSize = TRUE;
		m_fRedrawDiskRequirements = TRUE;
	}
}

void CWorkspacePreferencesDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
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

void CWorkspacePreferencesDialog::OnKillfocusWorkspaceSize()
{
	if (m_fWorkspaceSizeChanged)
	{
		PostMessage(IDM_VALIDATE_WORKSPACE_SIZE, 0, 0L);
	}
}

void CWorkspacePreferencesDialog::OnOK()
{
#if 0
	if (GetFocus() == GetDlgItem(IDC_WORKSPACE_SIZE))
	{
		GetDlgItem(IDOK)->SetFocus();
	}

	else
#endif
	if (OnValidateWorkspaceSize(0, 0L))
	{
	/* Check that the number they typed is valid. */
		DWORD dwAvailableSize = Util::GetAvailableDiskSpace(m_csWorkspaceDirectory);

		dwAvailableSize += m_cache->get_physical_size();

		if (m_dwWorkspaceSize > dwAvailableSize)
		{
		/*
		// This is not a good idea.
		// Inform the user that a lower number is required.
		*/

			GetConfiguration()->MessageBox(IDS_ErrWorkspaceExceedsDiskSpace, 0,
													 MB_OK | MB_ICONEXCLAMATION);
			GetDlgItem(IDC_WORKSPACE_SIZE)->SetFocus();
		}
		else
		{
		/* Go home. */
			CPmwDialog::OnOK();
		}
	}
}

void CWorkspacePreferencesDialog::OnChangeWorkspaceSize()
{
	m_fWorkspaceSizeChanged = TRUE;
}

void CWorkspacePreferencesDialog::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

	CPmwDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

LRESULT CWorkspacePreferencesDialog::OnValidateWorkspaceSize(WPARAM wParam, LPARAM lParam)
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

void CWorkspacePreferencesDialog::OnParentNotify(UINT message, LPARAM lParam)
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
			m_fWorkspaceSizeChanged = FALSE;
		}
	}
}

void CWorkspacePreferencesDialog::OnSetfocusWorkspaceSize()
{
	m_workspace_edit.SetSel(0, -1);
}

BOOL CWorkspacePreferencesDialog::OnInitDialog()
{
/* Create the directory name. */

	TRY
	{
		m_csWorkspaceDirectory = GetGlobalPathManager()->ExpandPath(m_cache->cache_name());

	/*
	// We want to strip off the base name.
	// We assume that there is a base name to begin with.
	*/

		int nIndex;

		if ((nIndex = m_csWorkspaceDirectory.ReverseFind('\\')) != -1)
		{
		/* Leave the slash off. */
			nIndex--;
		}
		else
		{
			nIndex = m_csWorkspaceDirectory.ReverseFind(':');
		}

	/* We want to strip off the base name. */

		m_csWorkspaceDirectory = m_csWorkspaceDirectory.Left(nIndex+1);
	}
	CATCH_ALL(e)
	{
	/* Make sure we're empty. */
		m_csWorkspaceDirectory.Empty();
	}
	END_CATCH_ALL

//	od("workspace directory: %s\r\n", (LPCSTR)m_csWorkspaceDirectory);

/* Get the current size. */

	SetWorkspaceSize(m_cache->cache_size());

/* Do the default init. */

	CPmwDialog::OnInitDialog();

/* Limit the text size. */

	m_workspace_edit.LimitText(6);		// a reasonable limit
	m_workspace_edit.SetFocus();

	return FALSE;		// return TRUE  unless you set the focus to a control
}

#endif
