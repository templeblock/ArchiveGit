#include "stdafx.h"
#include "Generalpropertypage.h"
#include "Optionspropertysheet.h"
#include "HelperFunctions.h"
#include "RegKeys.h"
#include "Utility.h"


/////////////////////////////////////////////////////////////////////////////
CGeneralPropertyPage::CGeneralPropertyPage(COptionsPropertySheet* psParentSheet) : CPropertyPageImpl<CGeneralPropertyPage>(_T("General"))
{
	m_psParentSheet			= psParentSheet;
	m_nSmartCacheStatus		= 0;
	m_nTrayIconStatus		= 0;
	m_nAlwaysOnTop			= 0;
	m_nOutlookStatus		= 1;
	m_nExpressStatus		= 1;
	m_nWebMailStatus		= 1;
	m_nAOLStatus			= 0; // Temporarily disabled - JHC
	m_nIECat				= 0;
	m_nUseOutlookDefaultSta = 0;
	m_nUseExpressDefaultSta = 0;
	m_nUseIEDefaultSta		= 0;
	m_nUseAOLDefaultSta		= 0;
}
/////////////////////////////////////////////////////////////////////////////
CGeneralPropertyPage::~CGeneralPropertyPage(void)
{
	m_ImageList.Destroy();
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CGeneralPropertyPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	GetPropertySheet().SendMessage (UWM_CENTER_SHEET);


	m_nExpressStatus = m_Auth.IsClientRegistered(CLIENT_TYPE_EXPRESS);
	m_nOutlookStatus = m_Auth.IsClientRegistered(CLIENT_TYPE_OUTLOOK);
	m_nWebMailStatus = m_Auth.IsClientRegistered(CLIENT_TYPE_IE);
	//m_nAOLStatus = m_Auth.IsClientRegistered(CLIENT_TYPE_AOL); Temporary until Tim gives go ahead for AOL support - JHC
	m_nSmartCacheStatus = m_Auth.IsSmartCacheEnabled();
	m_nAlwaysOnTop = m_Auth.IsTopMost();
	
	DWORD dwShowIcon = 1;
	m_Auth.ReadShowIcon(dwShowIcon);
	m_nTrayIconStatus = dwShowIcon;

	// Default stationery
	m_nUseOutlookDefaultSta = m_Auth.IsOutlookDefaultStaEnabled();
	m_nUseExpressDefaultSta = m_Auth.IsExpressDefaultStaEnabled();
	m_nUseIEDefaultSta		= m_Auth.IsIEDefaultStaEnabled();
	//m_nUseAOLDefaultSta		= m_Auth.IsAOLDefaultStaEnabled(); // Temp, until AOL supported - JHC

	// Default stationery clear buttons
	CString szTemp;
	if (GetInstallPath(szTemp))
	{
		// Outlook
		CString szPath = szTemp;
		szPath += DEFAULT_OL_STATIONERY_PATH;
		if (IsFilePresent(szPath))
			GetDlgItem(IDC_OL_CLEAR_BUTTON).EnableWindow(true);
		else
			GetDlgItem(IDC_OL_CLEAR_BUTTON).EnableWindow(false);

		// Express
		szPath = szTemp;
		szPath += DEFAULT_OE_STATIONERY_PATH;
		if (IsFilePresent(szPath))
			GetDlgItem(IDC_OE_CLEAR_BUTTON).EnableWindow(true);
		else
			GetDlgItem(IDC_OE_CLEAR_BUTTON).EnableWindow(false);

		// IE
		szPath = szTemp;
		szPath += DEFAULT_IE_STATIONERY_PATH;
		if (IsFilePresent(szPath))
			GetDlgItem(IDC_IE_CLEAR_BUTTON).EnableWindow(true);
		else
			GetDlgItem(IDC_IE_CLEAR_BUTTON).EnableWindow(false);

		// AOL - Temporarily disable until Tim decides to support - JHC
		/*szPath = szTemp;
		szPath += DEFAULT_AOL_STATIONERY_PATH;
		if (IsFilePresent(szPath))
			GetDlgItem(IDC_AOL_CLEAR_BUTTON).EnableWindow(true);
		else*/
			GetDlgItem(IDC_AOL_CLEAR_BUTTON).EnableWindow(false);
	}


	// Combo box
	m_ctlIECatCombo.SubclassWindow(GetDlgItem(IDC_IECAT_COMBO));
	m_ctlIECatCombo.ModifyStyle(NULL, CBS_OWNERDRAWFIXED | CBS_HASSTRINGS);

	InitCombo();

	if (m_nWebMailStatus)
		m_ctlIECatCombo.EnableWindow(true);
	else
		m_ctlIECatCombo.EnableWindow(false);
	

	DoDataExchange(DDX_LOAD);
	ShowWindow(SW_SHOW);

    return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CGeneralPropertyPage::InitCombo()
{
	if (m_ImageList)
		m_ImageList.Destroy();

	if (!m_ImageList.Create(16,16, ILC_COLOR24 | ILC_MASK, 9, 9))
		return false;

	int nIndex=0;

	// Add combo images to image list
	for (int i=0; i<ITEM_COUNT; i++)
	{
		nIndex = AddComboImage(ITEM_INFO[i].BitmapID);
		if (nIndex < 0)
			return false;
	}

	m_ctlIECatCombo.SetImageList(&m_ImageList);
	m_ctlIECatCombo.SetColumnCount(1);
	
	// Add combo strings
	for (int i=0; i<ITEM_COUNT; i++)
	{
		m_ctlIECatCombo.AddString(ITEM_INFO[i].szText);
	}

	// Set combo images - last item has no image
	for (int i=0; i<ITEM_COUNT-1; i++)
	{
		m_ctlIECatCombo.SetItemImage(i, i);
	}

	m_nIECat = LAST_CAT_USED; // Initialize to "last used"
	m_Auth.ReadDWORDStatus(REGVAL_DEFAULT_IECAT, (DWORD&) m_nIECat);
	m_ctlIECatCombo.SetCurSel(m_nIECat);

	
	return true;
}
/////////////////////////////////////////////////////////////////////////////
int CGeneralPropertyPage::AddComboImage(DWORD dwID)
{
	if (dwID == -1)
		return 0x8000;

	CBitmap bmp;
	bmp.LoadBitmap(dwID);
	HBITMAP hBitmapMask = ReplaceBitmapColor(bmp.m_hBitmap, CLR_DEFAULT, RGB(255,255,255), RGB(0,0,0));

	return m_ImageList.Add(bmp.m_hBitmap, hBitmapMask);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CGeneralPropertyPage::OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (IDC_IECAT_COMBO == (DWORD)wParam)
	{
		LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT) lParam;
		m_ctlIECatCombo.DrawItem(lpDrawItem);
	}
	
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CGeneralPropertyPage::OnHelpMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPHELPINFO lpHI = (LPHELPINFO)lParam;

	// Verify validity of input parameter
    if (!lpHI || (sizeof(HELPINFO) != lpHI->cbSize))
		return S_OK;
	
	
	int	iStringID=0;
	switch (lpHI->iCtrlId)
	{
		case IDC_SMARTCACHE_CHECK:
			iStringID = IDS_SMARTCACHE_TIP;
			break;

		case IDC_TRAYICON_CHECK:
			iStringID = IDS_TRAYICON_TIP;
			break;

		case IDC_ALWAYSONTOP_CHECK:
			iStringID = IDS_ALWAYSONTOP_TIP;
			break;

		case IDC_ACTIVEMAIL_STATIC:
		case IDC_OUTLOOK_CHECK:
		case IDC_EXPRESS_CHECK:
		case IDC_WEBMAIL_CHECK:
		case IDC_AOL_CHECK:
			iStringID = IDS_CMENABLE_TIP;
			break;

		case IDC_DEFAULTSTATIONERY_STATIC:
		case IDC_OTLK_USEDEFAULT_CHECK:
		case IDC_EXPRESS_USEDEFAULT_CHECK:
		case IDC_IE_USEDEFAULT_CHECK:
		case IDC_AOL_USEDEFAULT_CHECK:
			iStringID = IDS_DEFSTA_TIP;
			break;

		case IDC_OL_CLEAR_BUTTON:
		case IDC_OE_CLEAR_BUTTON:
		case IDC_IE_CLEAR_BUTTON:
		case IDC_AOL_CLEAR_BUTTON:
			iStringID = IDS_CLEARSTA_TIP;
			break;


		case IDC_IE_STATIC:
		case IDC_IECAT_COMBO:
		case IDC_IECAT_STATIC:
			iStringID = IDS_IEONLY_TIP;
			break;

		default:
			return S_OK;
	}
	
	int iXPos = lpHI->MousePos.x + 50;
    int iYPos = lpHI->MousePos.y + 10;
	m_psParentSheet->ShowContextHelp(GetDlgItem(lpHI->iCtrlId), iStringID, iXPos, iYPos);
	
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CGeneralPropertyPage::OnSelEndOk(WORD uMsg, WORD idCtrl, HWND hWndControl, BOOL& bHandled)
{
	m_nIECat = m_ctlIECatCombo.GetCurSel();
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CGeneralPropertyPage::OnIESelect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CButton Btn(hWndCtl);
	if (BST_CHECKED == Btn.GetCheck())
	{
		m_ctlIECatCombo.EnableWindow(true);
		GetDlgItem(IDC_IE_USEDEFAULT_CHECK).EnableWindow(true);
	}
	else
	{
		m_ctlIECatCombo.EnableWindow(false);
		GetDlgItem(IDC_IE_USEDEFAULT_CHECK).EnableWindow(false);
	}

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CGeneralPropertyPage::OnOLSelect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CButton Btn(hWndCtl);
	if (BST_CHECKED == Btn.GetCheck())
		GetDlgItem(IDC_OTLK_USEDEFAULT_CHECK).EnableWindow(true);
	else
		GetDlgItem(IDC_OTLK_USEDEFAULT_CHECK).EnableWindow(false);

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CGeneralPropertyPage::OnOESelect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CButton Btn(hWndCtl);
	if (BST_CHECKED == Btn.GetCheck())
		GetDlgItem(IDC_EXPRESS_USEDEFAULT_CHECK).EnableWindow(true);
	else
		GetDlgItem(IDC_EXPRESS_USEDEFAULT_CHECK).EnableWindow(false);

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CGeneralPropertyPage::OnAOLSelect(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CButton Btn(hWndCtl);
	if (BST_CHECKED == Btn.GetCheck())
		GetDlgItem(IDC_AOL_USEDEFAULT_CHECK).EnableWindow(true);
	else
		GetDlgItem(IDC_AOL_USEDEFAULT_CHECK).EnableWindow(false);

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CGeneralPropertyPage::OnOLClear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (RemoveDefaultStationery(CLIENT_TYPE_OUTLOOK))
		GetDlgItem(IDC_OL_CLEAR_BUTTON).EnableWindow(false);

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CGeneralPropertyPage::OnOEClear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (RemoveDefaultStationery(CLIENT_TYPE_EXPRESS))
		GetDlgItem(IDC_OE_CLEAR_BUTTON).EnableWindow(false);

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CGeneralPropertyPage::OnIEClear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (RemoveDefaultStationery(CLIENT_TYPE_IE))
		GetDlgItem(IDC_IE_CLEAR_BUTTON).EnableWindow(false);

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CGeneralPropertyPage::OnAOLClear(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (RemoveDefaultStationery(CLIENT_TYPE_AOL))
		GetDlgItem(IDC_AOL_CLEAR_BUTTON).EnableWindow(false);

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CGeneralPropertyPage::OnApply()
{
   if (!DoDataExchange(DDX_SAVE))
	   return PSNRET_INVALID;

   if (m_psParentSheet == NULL)
	return PSNRET_INVALID;

   // Update Default IE Category
   m_Auth.WriteDWORDStatus(REGVAL_DEFAULT_IECAT, m_nIECat);

   // Mail clients
    DWORD dwStatus = 0;
	if (m_nOutlookStatus)
		dwStatus |= CLIENT_TYPE_OUTLOOK;

	if (m_nExpressStatus)
		dwStatus |= CLIENT_TYPE_EXPRESS;

	if (m_nWebMailStatus)
		dwStatus |= CLIENT_TYPE_IE;

	if (m_nAOLStatus)
		dwStatus |= CLIENT_TYPE_AOL;

	m_Auth.WriteDWORDStatus(REGVAL_CLIENTSTATUS, dwStatus);
	m_psParentSheet->UpdateClients(dwStatus);

	// Default Stationery
    dwStatus = 0;
	if (m_nUseOutlookDefaultSta)
		dwStatus |= CLIENT_TYPE_OUTLOOK;

	if (m_nUseExpressDefaultSta)
		dwStatus |= CLIENT_TYPE_EXPRESS;

	if (m_nUseIEDefaultSta)
		dwStatus |= CLIENT_TYPE_IE;

	if (m_nUseAOLDefaultSta)
		dwStatus |= CLIENT_TYPE_AOL;

	m_Auth.WriteDWORDStatus(REGVAL_DEFAULTSTA, dwStatus);

	// Tray Icon
	m_Auth.WriteShowIcon(m_nTrayIconStatus);
	m_psParentSheet->HandleShow((BOOL)m_nTrayIconStatus);

	// Smart Cache. 
	// Note: In CHtmlDialog::SetURL() a call to CSmartCache::IsUseSmartCache() is used to check smartcache status
	// and CSmartCache::SetSyncMode() is called to change mode to SmartCache. When CHtmlDialog goes out of scope
	// it calls CSmartCache::RestoreSyncMode() to change back to normal mode. So all we need to do is update the status here.
	m_Auth.WriteDWORDStatus(REGVAL_SMARTCACHE, m_nSmartCacheStatus);

	// Always on Top status
	m_Auth.WriteDWORDStatus(REGVAL_TOPMOST, m_nAlwaysOnTop);


   return PSNRET_NOERROR;
}
