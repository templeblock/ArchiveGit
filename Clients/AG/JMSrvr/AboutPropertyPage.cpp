#include "stdafx.h"
#include "Aboutpropertypage.h"
#include "Optionspropertysheet.h"
#include "Version.h"


/////////////////////////////////////////////////////////////////////////////
CAboutPropertyPage::CAboutPropertyPage(COptionsPropertySheet* psParentSheet) : CPropertyPageImpl<CAboutPropertyPage>(_T("About"))
{
	m_psParentSheet	= psParentSheet;
}
/////////////////////////////////////////////////////////////////////////////
CAboutPropertyPage::~CAboutPropertyPage(void)
{
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CAboutPropertyPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	HINSTANCE hinst = _AtlBaseModule.GetResourceInstance();
	CString szHost;
	CString szUrl;
	m_Auth.ReadHost(szHost);

	CString szLicense;
	szLicense.LoadString(IDS_LICENSE_URL);
	szUrl = szHost;
	szUrl += szLicense;
	m_LicenseLabel.SetUrl(szUrl);
	m_LicenseLabel.InitDialog(GetDlgItem(IDC_LICENSE_STATIC));
	m_LicenseLabel.SetTextColor(RGB(0x00,0x00,0xFF));
	m_LicenseLabel.SetFontName("Arial");
	m_LicenseLabel.SetBackgroundColor(CLR_NONE);
	m_LicenseLabel.SetFontUnderline(true);
	m_LicenseLabel.SetLink(true);
	m_LicenseLabel.SetLinkCursor(LoadCursor(hinst, MAKEINTRESOURCE(ID_HAND)));

	
	
	CString szSales;
	szSales.LoadString(IDS_SERVICES_URL);
	szUrl = szHost;
	szUrl += szSales;
	m_SalesLabel.SetUrl(szUrl);
	m_SalesLabel.InitDialog(GetDlgItem(IDC_TERMSOFSERVICE_STATIC));
	m_SalesLabel.SetTextColor(RGB(0x00,0x00,0xFF));
	m_SalesLabel.SetFontName("Arial");
	m_SalesLabel.SetBackgroundColor(CLR_NONE);
	m_SalesLabel.SetFontUnderline(true);
	m_SalesLabel.SetLink(true);
	m_SalesLabel.SetLinkCursor(LoadCursor(hinst, MAKEINTRESOURCE(ID_HAND)));

	CString szPrivacy;
	szPrivacy.LoadString(IDS_PRIVACY_URL);
	szUrl = szHost;
	szUrl += szPrivacy;
	m_PivacyLabel.SetUrl(szUrl);
	m_PivacyLabel.InitDialog(GetDlgItem(IDC_PRIVACY_STATIC));
	m_PivacyLabel.SetTextColor(RGB(0x00,0x00,0xFF));
	m_PivacyLabel.SetFontName("Arial");
	m_PivacyLabel.SetBackgroundColor(CLR_NONE);
	m_PivacyLabel.SetFontUnderline(true);
	m_PivacyLabel.SetLink(true);
	m_PivacyLabel.SetLinkCursor(LoadCursor(hinst, MAKEINTRESOURCE(ID_HAND)));


	m_HomeUrlLabel.InitDialog(GetDlgItem(IDC_HOME_STATIC));
	m_HomeUrlLabel.SetTextColor(RGB(0x00,0x00,0xFF));
	m_HomeUrlLabel.SetFontName("Arial");
	m_HomeUrlLabel.SetBackgroundColor(CLR_NONE);
	m_HomeUrlLabel.SetFontUnderline(true);
	m_HomeUrlLabel.SetLink(true);
	m_HomeUrlLabel.SetLinkCursor(LoadCursor(hinst, MAKEINTRESOURCE(ID_HAND)));


	UpdateVersion();

    return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CAboutPropertyPage::OnApply()
{
     if (!DoDataExchange(DDX_SAVE))
	   return PSNRET_INVALID;


   return PSNRET_NOERROR;
}
/////////////////////////////////////////////////////////////////////////////
void CAboutPropertyPage::UpdateVersion()
{
	HWND hWnd = GetDlgItem(IDC_VERSION_STATIC);
	if (hWnd)
	{
		TCHAR szLabel[MAX_PATH];
		::GetWindowText(hWnd, szLabel, sizeof(szLabel));
		CString strLabel = szLabel;
		strLabel += VER_PRODUCT_VERSION_STR;
		::SetWindowText(hWnd, strLabel);
	}
}