// netusers.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "netusers.h"

#include "system.h"
#include "file.h"

#include <io.h>
#include <fcntl.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNetworkUsersDialog dialog

CNetworkUsersDialog::CNetworkUsersDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CNetworkUsersDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNetworkUsersDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CNetworkUsersDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetworkUsersDialog)
	DDX_Control(pDX, IDC_NETWORK_USERS, m_user_list);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNetworkUsersDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CNetworkUsersDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkUsersDialog message handlers

#define HEADER_PAD 32
#define NAME_SIZE 48

/*
// Try to lock a byte.
*/

static BOOL WeCanLock(CFile* pFile, DWORD dwWhere)
{
	TRY
	{
		pFile->LockRange(dwWhere, 1L);
		pFile->UnlockRange(dwWhere, 1L);
		return TRUE;
	}
	END_TRY
	return FALSE;
}

BOOL CNetworkUsersDialog::OnInitDialog()
{
	// Do the default initialization (like setting up m_user_list).

	CPmwDialog::OnInitDialog();

/*
// Build the list.
*/

	int rgTabStops[2] = {30, 70};		// Matches x coordinates in dialog
	CPmwApp *pApp = (CPmwApp *)AfxGetApp();

	m_user_list.SetTabStops(2, rgTabStops);

	// This will be our file.

	ReadOnlyFile user_file("PMUSERS.DAT");

	// Try to use some buffering.

	user_file.set_read_buffer(1024);

	// Attempt to read the header.

	char buffer[256];
	ST_DEV_IO_SIZE len;
	char *p;

	if (user_file.read(buffer, sizeof(buffer), &len) != ERRORCODE_None
			|| (p = (char *)memchr(buffer, 0x1a, len)) == NULL)
	{
		EndDialog(IDABORT);
		::MessageBox(GetSafeHwnd(),
					// JRO "Invalid user file.",
               GET_PMWAPP()->GetResourceStringPointer (IDS_INVALID_USER_FILE),
					AfxGetAppName(),
					MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

/* We found the end of the header. */

	user_file.seek((p+1)-buffer + HEADER_PAD, ST_DEV_SEEK_SET);

/*
// Try to open the lock file. If we don't get it, we just ignore it.
*/

	CFile* pFile = &(GET_PMWAPP()->m_cfLockFile);
	CFile OurFile;
	if (pFile->m_hFile == CFile::hFileNull)
	{
		// We need to open the file locally.
		strcpy(buffer, GetGlobalPathManager()->ExpandPath("NETL.PM"));

		pFile = &OurFile;
		pFile->Open(buffer, CFile::modeRead | CFile::shareDenyWrite, NULL);
	}

	// Run through all users.
	for (int user_number = 0; user_number < 256; user_number++)
	{
		char name[NAME_SIZE];

		if (user_number == 0)
		{
		/* Put this in first. */
			// JRO strcpy(name, "Unknown");
         strcpy(name, GET_PMWAPP()->GetResourceStringPointer (IDS_UNKNOWN));
		}
		else
		{
		/* Read the next name. */
			user_file.seek(1, ST_DEV_SEEK_CUR);		// Skip the size byte
			if (user_file.read(name, sizeof(name)) != ERRORCODE_None)
			{
			/* All done! */
				break;
			}
		}

		char cStatus;

		// See if the user is currently active.
		if (pFile->m_hFile == NULL)
		{
			// Cannot do locking.
			cStatus = '?';
		}
		else
		{
			cStatus = WeCanLock(pFile, (DWORD)user_number)
						? 'N'
						: 'Y';
		}

		// Build the name to add.
		char line[1 + 3 + 1 + 2 + 1 + 1 + NAME_SIZE];

		sprintf(line, " %03d\t  %c\t%s", user_number, cStatus, name);

		// Add the name we built.
		m_user_list.AddString(line);
	}

	m_user_list.SetCurSel(pApp->m_nUserID);

	return TRUE;  // return TRUE  unless you set the focus to a control
}
