#include "stdafx.h"
#include "PreferencePropertypage.h"
#include "Optionspropertysheet.h"
#include "HelperFunctions.h"
#include "RegKeys.h"


CPreferencePropPage::CPreferencePropPage(COptionsPropertySheet* psParentSheet) : CPropertyPageImpl<CPreferencePropPage>(_T("Account"))
{
	m_psParentSheet	= psParentSheet;
	m_psp.hInstance = _AtlBaseModule.GetResourceInstance();
	m_nPromoStatus		= 0;
}
/////////////////////////////////////////////////////////////////////////////
CPreferencePropPage::~CPreferencePropPage(void)
{
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CPreferencePropPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_Auth.ReadUserID(m_szRegEmailAddr);

	bool bEnable = m_Auth.IsUserPaidMember();
	if (bEnable)
		m_nPromoStatus = m_Auth.IsPromoFooterOn();
	else
		m_nPromoStatus = true;

	CWindow wndPromo = GetDlgItem(IDC_PROMO_CHECK);
	wndPromo.EnableWindow(bEnable);

	DoDataExchange(DDX_LOAD);

    return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPreferencePropPage::OnApply()
{
    if (!DoDataExchange(DDX_SAVE))
	   return PSNRET_INVALID;

	// Update Promo Status
   m_Auth.WriteDWORDStatus(REGVAL_PROMOSTATUS, m_nPromoStatus);

   return PSNRET_NOERROR;
}
/////////////////////////////////////////////////////////////////////////////
int CPreferencePropPage::OnSetActive()
{
    return CPropertyPageImpl<CPreferencePropPage>::OnSetActive(); //this returns TRUE
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CPreferencePropPage::OnUnlock(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (GetKeyState(VK_CONTROL) < 0)
		m_psParentSheet->HandleAuth(MODE_SILENT);
	else
		m_psParentSheet->HandleAuth(MODE_SIGNIN);

	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CPreferencePropPage::OnMyAccount(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_psParentSheet->HandleMyAccount();
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CPreferencePropPage::OnHelpMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPHELPINFO lpHI = (LPHELPINFO)lParam;

	// Verify validity of input parameter
    if (!lpHI || (sizeof(HELPINFO) != lpHI->cbSize))
		return S_OK;
	
	
	int	iStringID=0;
	switch (lpHI->iCtrlId)
	{
		case IDC_EMAILADDRESS_STATIC:
		case IDC_REGISTEREDEMAIL:
			iStringID = IDS_REGEMAIL_TIP;
			break;

		case IDC_UNLOCK_STATIC:
		case IDC_UNLOCK_BTN:
			iStringID = IDS_UNLOCK_TIP;
			break;

		case IDC_ACCOUNT_STATIC:
		case IDC_MYACCOUNT_BTN:
			iStringID = IDS_MYACCOUNT_TIP;
			break;

		case IDC_PREFERENCES_STATIC:
		case IDC_PROMO_CHECK:
			iStringID = IDS_PROMO_TIP;
			break;

		default:
			return S_OK;
	}
	
	int iXPos = lpHI->MousePos.x + 50;
    int iYPos = lpHI->MousePos.y + 10;
	m_psParentSheet->ShowContextHelp(GetDlgItem(lpHI->iCtrlId), iStringID, iXPos, iYPos);
	
	return S_OK;
}