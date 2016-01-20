/*
// $Workfile: TESTDLG.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/intl/TESTDLG.CPP $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/14/98 4:07p Jayn
// 
//    Rev 1.3   07 Oct 1996 14:03:24   FRED
// Added GetTaxInformation()
// 
//    Rev 1.2   04 Oct 1996 15:07:24   FRED
// Added GetWebSite()
// 
//    Rev 1.1   04 Oct 1996 10:03:54   FRED
// Added GetCountryName(); Country Id's to data
// 
//    Rev 1.0   03 Oct 1996 10:31:06   FRED
// Initial revision.
*/

#include "stdafx.h"
#include "test.h"
#include "testDlg.h"
#include "intldll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestDlg dialog

CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestDlg)
	DDX_Control(pDX, IDC_COUNTRY_LIST, m_CountryList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialog)
	//{{AFX_MSG_MAP(CTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COUNTRY_LIST, OnSelchangeCountryList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestDlg message handlers

BOOL CTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	CString strAboutMenu;
	strAboutMenu.LoadString(IDS_ABOUTBOX);
	if (!strAboutMenu.IsEmpty())
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	}

#ifdef WIN32                                                         
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
#endif
	
	// Initialize the combo box.
	CInternationalInformationDll Dll("");
	Dll.FillCountryComboBox(m_CountryList.GetSafeHwnd(), COUNTRY_Andorra);

	UpdateCountryInformation();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestDlg::OnPaint() 
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
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTestDlg::OnSelchangeCountryList() 
{
	UpdateCountryInformation();
}

void CTestDlg::UpdateCountryInformation(void)
{
	int nIndex = m_CountryList.GetCurSel();
	if (nIndex != CB_ERR)
	{
		DWORD dwCountryId = m_CountryList.GetItemData(nIndex);
		if (dwCountryId != (DWORD)CB_ERR)
		{
			CInternationalData Data;
			CInternationalInformationDll Dll("");
			CString csMsregIni;
			GetModuleFileName(AfxGetInstanceHandle(), csMsregIni.GetBuffer(_MAX_PATH+1), _MAX_PATH);
			csMsregIni.ReleaseBuffer();
			int nIndex = csMsregIni.ReverseFind('\\');
			if (nIndex >= 0)
			{
				csMsregIni = csMsregIni.Left(nIndex+1);
				csMsregIni += "MSREG.INI";

				Dll.GetInternationalData((WORD)dwCountryId, csMsregIni, &Data);

				CString csCountryName;
				Dll.GetCountryName(Data.m_wCountry, csCountryName);
				CString csWebSite;
				Dll.GetWebSite(Data.m_wCountry, csMsregIni, csWebSite);
				CString csTaxInformation;
				Dll.GetTaxInformation(csMsregIni, csTaxInformation);
				CString csCountry;
				csCountry.Format("#%d %s %s %s", Data.m_wCountry, (LPCSTR)csCountryName, (LPCSTR)csWebSite, (LPCSTR)csTaxInformation);
				SetDlgItemText(IDC_COUNTRY, csCountry);

				SetDlgItemText(IDC_IS_US_LOCAL, Data.m_fUnitedStatesLocal ? "Yes" : "No");
				SetDlgItemText(IDC_WEB_SITE, Data.m_szWebSite);
				SetDlgItemText(IDC_TAX_RATES, Data.m_szTaxInformation);

				SetDlgItemText(IDC_REGISTRATION_MODEM, FormatPhoneNumber(Dll, csMsregIni, &(Data.m_Registration.m_Modem)));
				SetDlgItemText(IDC_REGISTRATION_PHONE, FormatPhoneNumber(Dll, csMsregIni, &(Data.m_Registration.m_Phone)));
				SetDlgItemText(IDC_REGISTRATION_FAX, FormatPhoneNumber(Dll, csMsregIni, &(Data.m_Registration.m_Fax)));
				SetDlgItemText(IDC_REGISTRATION_HELP, FormatPhoneNumber(Dll, csMsregIni, &(Data.m_Registration.m_Help)));
				SetDlgItemText(IDC_REGISTRATION_MAIL, FormatMailAddress(Dll, csMsregIni, &(Data.m_Registration.m_Mail)));

				SetDlgItemText(IDC_SALES_MODEM, FormatPhoneNumber(Dll, csMsregIni, &(Data.m_InstantSales.m_Modem)));
				SetDlgItemText(IDC_SALES_PHONE, FormatPhoneNumber(Dll, csMsregIni, &(Data.m_InstantSales.m_Phone)));
				SetDlgItemText(IDC_SALES_FAX, FormatPhoneNumber(Dll, csMsregIni, &(Data.m_InstantSales.m_Fax)));
				SetDlgItemText(IDC_SALES_HELP, FormatPhoneNumber(Dll, csMsregIni, &(Data.m_InstantSales.m_Help)));
				SetDlgItemText(IDC_SALES_MAIL, FormatMailAddress(Dll, csMsregIni, &(Data.m_InstantSales.m_Mail)));
			}
		}
	}
}

LPCSTR CTestDlg::FormatPhoneNumber(CInternationalInformationDll& Dll, LPCSTR pszIniFile, CInternationalData::PhoneNumber* pData)
{
	static char szPhoneNumber[1024];

	sprintf(
		szPhoneNumber,
		"(#%d %s) %s %s %s [%s] %s",
		pData->m_wCountry,
		pData->m_szOffice,
		pData->m_fUnitedStatesLocal ? "US" : "Non-US",
		pData->m_fLocal ? "Local" : "Foreign",
		pData->m_fFree ? "Free" : "Toll",
		pData->m_szPrefix,
		pData->m_szNumber);

	return szPhoneNumber;
}

LPCSTR CTestDlg::FormatMailAddress(CInternationalInformationDll& Dll, LPCSTR pszIniFile, CInternationalData::MailAddress* pData)
{
	static char szMailAddress[1024];

	sprintf(
		szMailAddress,
		"(#%d %s) %s %s %s",
		pData->m_wCountry,
		pData->m_szOffice,
		pData->m_fUnitedStatesLocal ? "US" : "Non-US",
		pData->m_fLocal ? "Local" : "Foreign",
		pData->m_szAddress);

	return szMailAddress;
}