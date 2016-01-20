#include "stdafx.h"
#include "Maintpropertypage.h"
#include "Optionspropertysheet.h"
#include "HelperFunctions.h"
#include "RegKeys.h"


/////////////////////////////////////////////////////////////////////////////
CMaintPropertyPage::CMaintPropertyPage(COptionsPropertySheet* psParentSheet) : CPropertyPageImpl<CMaintPropertyPage>(_T("Maintenance"))
{
	m_psParentSheet	= psParentSheet;
}
/////////////////////////////////////////////////////////////////////////////
CMaintPropertyPage::~CMaintPropertyPage(void)
{
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMaintPropertyPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DoDataExchange(DDX_LOAD);
    return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMaintPropertyPage::OnApply()
{
     if (!DoDataExchange(DDX_SAVE))
	   return PSNRET_INVALID;


   return PSNRET_NOERROR;
}
//////////////////////////////////////////////////////////////////////
LRESULT CMaintPropertyPage::OnCheckUpdates(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_psParentSheet->HandleCheckForUpdates();
	return S_OK;
}
//////////////////////////////////////////////////////////////////////
LRESULT CMaintPropertyPage::OnOptimize(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_psParentSheet->HandleOptimize();
	return S_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CMaintPropertyPage::OnHelpMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPHELPINFO lpHI = (LPHELPINFO)lParam;

	// Verify validity of input parameter
    if (!lpHI || (sizeof(HELPINFO) != lpHI->cbSize))
		return S_OK;
	
	
	int	iStringID=0;
	switch (lpHI->iCtrlId)
	{
		case IDC_UPDATES_STATIC:
		case IDC_CHECKUPDATES_BTN:
			iStringID = IDS_CHECKUPDATES_TIP;
			break;

		case IDC_OPTIMIZE_STATIC:
		case IDC_OPTIMIZE_BTN:
			iStringID = IDS_OPTIMIZE_TIP;
			break;

		default:
			return S_OK;
	}
	
	int iXPos = lpHI->MousePos.x + 50;
    int iYPos = lpHI->MousePos.y + 10;
	m_psParentSheet->ShowContextHelp(GetDlgItem(lpHI->iCtrlId), iStringID, iXPos, iYPos);
	
	return S_OK;
}