// license.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "license.h"
#include "netusers.h"
#include "pmwcfg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define APPNAME	GetConfiguration()->ReplacementText('T')

/////////////////////////////////////////////////////////////////////////////
// CLicenseDlg dialog

CLicenseDlg::CLicenseDlg(PINSTALLATIONINFO pInfo, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CLicenseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLicenseDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pInfo = pInfo;
}

void CLicenseDlg::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLicenseDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLicenseDlg, CPmwDialog)
	//{{AFX_MSG_MAP(CLicenseDlg)
	ON_BN_CLICKED(IDC_LICENSE_ADD, OnClickedLicenseAdd)
	ON_BN_CLICKED(IDC_LICENSE_INSTALL, OnClickedLicenseInstall)
	ON_BN_CLICKED(IDC_LICENSE_USERS, OnClickedLicenseUsers)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLicenseDlg message handlers

void CLicenseDlg::OnClickedLicenseAdd()
{
	if (!GetConfiguration()->SupportsNetworkInterface())
	{
		return;
	}

	if (m_pInfo->bNetCount == 255)
	{
		// JRO ::MessageBox(GetSafeHwnd(), "No more users can be added to the network license.",
      ::MessageBox(GetSafeHwnd(), GET_PMWAPP()->GetResourceStringPointer (IDS_NET_USERS_FULL),
							APPNAME,
							MB_OK | MB_ICONSTOP);
		return;
	}

	CAddLicenseDialog adlg(255 - m_pInfo->bNetCount, this);

	if (adlg.DoModal() == IDOK)
	{
		char lrcbuffer[40];

		m_pInfo->GetLicenseRequestCode(adlg.m_new_users, lrcbuffer);

		CLicenseRequestDialog rdlg(m_pInfo->szSerialNumber,
								lrcbuffer,
								this);

		rdlg.DoModal();
	}
}

void CLicenseDlg::show_licensed_users()
{
	char buffer[80];

	if (m_pInfo->bNetCount == 0)
	{
 		// JRO strcpy(buffer, "This software is not currently licensed for use on a network.");
      strcpy(buffer, GET_PMWAPP()->GetResourceStringPointer (IDS_NET_NO_LICENSE));
	}
	else
	{
		sprintf(buffer,
			// JRO "This software is currently licensed for %d network user%s.",
         GET_PMWAPP()->GetResourceStringPointer (IDS_NET_USERS_NUM),
			m_pInfo->bNetCount,
			m_pInfo->bNetCount == 1
					? ""
					: GET_PMWAPP()->GetResourceStringPointer (IDS_USER_PLURAL));// JRO "s");
	}

	SetDlgItemText(IDC_LICENSE_INFO, buffer);
}

void CLicenseDlg::OnClickedLicenseInstall()
{
	if (!GetConfiguration()->SupportsNetworkInterface())
	{
		return;
	}

	CInstallLicenseDialog idlg(m_pInfo, this);

	if (idlg.DoModal() == IDOK)
	{
		show_licensed_users();

	/* Focus to OK. */

		GotoDlgCtrl(GetDlgItem(IDOK));
	}
}

void CLicenseDlg::OnClickedLicenseUsers()
{
	CNetworkUsersDialog udlg(this);

	udlg.DoModal();
}

BOOL CLicenseDlg::OnInitDialog()
{
	CPmwDialog::OnInitDialog();

/* Show the current license situation. */

	show_licensed_users();

/* Focus to OK. */

	GotoDlgCtrl(GetDlgItem(IDOK));

	return FALSE;			// Since we set the focus ourselves.
}

/////////////////////////////////////////////////////////////////////////////
// CAddLicenseDialog dialog

CAddLicenseDialog::CAddLicenseDialog(int max_users, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CAddLicenseDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddLicenseDialog)
	m_new_users = 5;
	//}}AFX_DATA_INIT

	m_max_users = max_users;

	if (m_new_users > max_users)
	{
	 	m_new_users = m_max_users;
	}
}

void CAddLicenseDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddLicenseDialog)
	DDX_Text(pDX, IDC_ADD_AMOUNT, m_new_users);
	//}}AFX_DATA_MAP

	DDV_MinMaxInt(pDX, m_new_users, 1, m_max_users);
}

BEGIN_MESSAGE_MAP(CAddLicenseDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CAddLicenseDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddLicenseDialog message handlers

BOOL CAddLicenseDialog::OnInitDialog()
{
	return CPmwDialog::OnInitDialog();
}

/////////////////////////////////////////////////////////////////////////////
// CInstallLicenseDialog dialog

CInstallLicenseDialog::CInstallLicenseDialog(PINSTALLATIONINFO pInfo, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CInstallLicenseDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInstallLicenseDialog)
	//}}AFX_DATA_INIT
	m_pInfo = pInfo;
}

void CInstallLicenseDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInstallLicenseDialog)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInstallLicenseDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CInstallLicenseDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInstallLicenseDialog message handlers

BOOL CInstallLicenseDialog::OnInitDialog()
{
	return CPmwDialog::OnInitDialog();
}

void CInstallLicenseDialog::OnOK()
{
	char szAccessCode[80];

	GetDlgItemText(IDC_INSTALL_LAC, szAccessCode, sizeof(szAccessCode));

	if (m_pInfo->ValidateLicenseAccessCode(szAccessCode))
	{
		WORD wCount = (szAccessCode[0]-'0')*100
				  +(szAccessCode[1]-'0')*10
				  +(szAccessCode[2]-'0')*1;
		m_pInfo->bNetCount += (BYTE)wCount;

		if (m_pInfo->Put())
		{
		/* Succeeded! */
			::MessageBox(GetSafeHwnd(),
						// JRO "The new network license is now installed.",
                  GET_PMWAPP()->GetResourceStringPointer (IDS_NET_INSTALLED),
						APPNAME,
						MB_OK | MB_ICONINFORMATION);
		}
		else
		{
		/* Failed! */
			::MessageBox(GetSafeHwnd(),
						// JRO "The new network license information could not be recorded.",
                  GET_PMWAPP()->GetResourceStringPointer (IDS_NET_NO_RECORD),
						APPNAME,
						MB_OK | MB_ICONEXCLAMATION);
		}

	/* And finish leaving. */

		CPmwDialog::OnOK();
	}
	else
	{
		::MessageBox(GetSafeHwnd(),
 							// JRO "Please enter a valid license access code.",
                     GET_PMWAPP()->GetResourceStringPointer (IDS_NET_ENTER_CODE),
							APPNAME,
							MB_OK | MB_ICONSTOP);

	/* Set the focus back on the type-in field. */

		GotoDlgCtrl(GetDlgItem(IDC_INSTALL_LAC));
	}
}

/////////////////////////////////////////////////////////////////////////////
// CLicenseRequestDialog dialog

CLicenseRequestDialog::CLicenseRequestDialog(LPSTR sn, LPSTR lrc, CWnd* pParent /*=NULL*/)
	: CPmwDialog(CLicenseRequestDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLicenseRequestDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_sn = sn;
	m_lrc = lrc;
}

void CLicenseRequestDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLicenseRequestDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLicenseRequestDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CLicenseRequestDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLicenseRequestDialog message handlers

BOOL CLicenseRequestDialog::OnInitDialog()
{
	CPmwDialog::OnInitDialog();

	SetDlgItemText(IDC_LICENSE_SN, m_sn);
	SetDlgItemText(IDC_LICENSE_LRC, m_lrc);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/////////////////////////////////////////////////////////////////////////////
// CLicenseExceededDialog dialog

CLicenseExceededDialog::CLicenseExceededDialog(CWnd* pParent /*=NULL*/)
	: CPmwDialog(CLicenseExceededDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLicenseExceededDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CLicenseExceededDialog::DoDataExchange(CDataExchange* pDX)
{
	CPmwDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLicenseExceededDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLicenseExceededDialog, CPmwDialog)
	//{{AFX_MSG_MAP(CLicenseExceededDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLicenseExceededDialog message handlers

BOOL CLicenseExceededDialog::OnInitDialog()
{
	CPmwApp *pApp = (CPmwApp *)AfxGetApp();
	BYTE bNetCount = pApp->m_installation_info.bNetCount;

	CPmwDialog::OnInitDialog();

	char buffer[256];

	const char *app_name = APPNAME;

	if (bNetCount == 0)
	{
		sprintf(buffer,
					// JRO "%s is licensed for a single user and is already in use.",
               GET_PMWAPP()->GetResourceStringPointer (IDS_NET_IN_USE),
					app_name);
	}
	else
	{
		sprintf(buffer, 
                     // JRO "%s is licensed for %d concurrent users."
                     GET_PMWAPP()->GetResourceStringPointer (IDS_NET_USERS_NUM_2),
	 						// JRO "You have exceeded the maximum user count.",
                     GET_PMWAPP()->GetResourceStringPointer (IDS_NET_USERS_MAXED),
					app_name,
					bNetCount);
	}

	SetDlgItemText(IDC_LICENSE_EXPLANATION, buffer);

	return TRUE;
}
