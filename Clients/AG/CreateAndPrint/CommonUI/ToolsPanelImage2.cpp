#include "stdafx.h"
#include "Ctp.h"
#include "ToolsPanelImage2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolsPanelImage2::CToolsPanelImage2(CCtp* pMainWnd) : CToolsCtlPanel(pMainWnd)
{
	m_nResID = IDD;
}

//////////////////////////////////////////////////////////////////////
CToolsPanelImage2::~CToolsPanelImage2()
{
}
//////////////////////////////////////////////////////////////////////
void CToolsPanelImage2::InitToolCtrls()
{
	m_ButtonColor.InitDialog(GetDlgItem(IDC_IME_COLORART_CHECK), IDR_IME_COLORART_GIF);
	m_ButtonCrop.InitDialog(GetDlgItem(IDC_IME_CROP_CHECK), IDR_IME_CROP_GIF);
	m_ButtonFlip.InitDialog(GetDlgItem(IDC_IME_FLIP_CHECK), IDR_IME_FLIP_GIF);
	m_ButtonContrast.InitDialog(GetDlgItem(IDC_IME_CONTRAST_CHECK), IDR_IME_CONTRAST_GIF);
	m_ButtonTint.InitDialog(GetDlgItem(IDC_IME_TINT_CHECK), IDR_IME_TINT_GIF);
	m_ButtonRedeye.InitDialog(GetDlgItem(IDC_IME_REDEYE_CHECK), IDR_IME_REDEYE_GIF);
	m_ButtonBorder.InitDialog(GetDlgItem(IDC_IME_BORDERS_CHECK), IDR_IME_BORDERS_GIF);
	m_ButtonEffects.InitDialog(GetDlgItem(IDC_IME_EFFECTS_CHECK), IDR_IME_EFFECTS_GIF);

	if (!m_pCtp->GetDocWindow()->GetSelect().SymIsClipArt())
		m_ButtonColor.EnableWindow(false);

}
//////////////////////////////////////////////////////////////////////
void CToolsPanelImage2::InitToolTips()
{
	CToolsCtlPanel::InitToolTips();

	AddToolTip(IDC_IME_COLORART_CHECK);
	AddToolTip(IDC_IME_CROP_CHECK);
	AddToolTip(IDC_IME_FLIP_CHECK);
	AddToolTip(IDC_IME_CONTRAST_CHECK);
	AddToolTip(IDC_IME_TINT_CHECK);
	AddToolTip(IDC_IME_REDEYE_CHECK);
	AddToolTip(IDC_IME_BORDERS_CHECK);
	AddToolTip(IDC_IME_EFFECTS_CHECK);

}
//////////////////////////////////////////////////////////////////////
void CToolsPanelImage2::ResetToolbarButtons()
{
	m_ButtonColor.Check(false);
	m_ButtonCrop.Check(false);
	m_ButtonFlip.Check(false);
	m_ButtonContrast.Check(false);
	m_ButtonTint.Check(false);
	m_ButtonRedeye.Check(false);
	m_ButtonBorder.Check(false);
	m_ButtonEffects.Check(false);
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelImage2::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	InitToolCtrls();
	InitToolTips();

	return IDOK;
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelImage2::UpdateControls()
{

	CDocWindow* pDocWindow = m_pCtp->GetDocWindow();
	CAGDoc* pAGDoc = (!pDocWindow ? NULL : pDocWindow->GetDoc());

	CAGSym* pSym = pDocWindow->GetSelectedItem();
	if (!pSym)
		return;

	m_ButtonColor.ShowWindow(SW_HIDE);
	m_ButtonRedeye.ShowWindow(SW_HIDE);
	if (pDocWindow->GetSelect().SymIsClipArt())
	{
		m_ButtonColor.EnableWindow(true);
		m_ButtonRedeye.EnableWindow(false);
	}
	else
	{
		m_ButtonColor.EnableWindow(false);
		m_ButtonRedeye.EnableWindow(true);
	}
	m_ButtonColor.ShowWindow(SW_SHOW);
	m_ButtonRedeye.ShowWindow(SW_SHOW);

	if (pSym->IsImage())
	{
		CAGSymImage* pImageSym = (CAGSymImage*)pSym;

		COLORREF Color = pImageSym->GetLastHitColor();
		if (Color == CLR_NONE)
			return;

		//Get the text panel and set the color
		CToolsPanelContainer* pCtlPanel = m_pCtp->GetCtlPanel(TEXT_PANEL);
		if (pCtlPanel)
		{
			CToolsPanelText2* pBotTxtToolsPanel = (CToolsPanelText2*)pCtlPanel->GetBotToolPanel();
			if (pBotTxtToolsPanel)
				pBotTxtToolsPanel->SetFillColor(Color);
		}
	}

	
}
//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelImage2::OnIMEToolbar(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	ResetToolbarButtons();
	UINT nTabID = IDD_IME_MAIN_TAB;

	if (IDC_IME_COLORART_CHECK == wID)
	{
		m_ButtonColor.Check(true);
		nTabID = IDD_IME_RECOLOR_TAB;
	}
	else if (IDC_IME_CROP_CHECK == wID)
	{
		m_ButtonCrop.Check(true);
		nTabID = IDD_IME_CROP_TAB;
	}
	else if (IDC_IME_FLIP_CHECK == wID)
	{
		m_ButtonFlip.Check(true);
		nTabID = IDD_IME_FLIP_ROTATE_TAB;
	}
	else if (IDC_IME_CONTRAST_CHECK == wID)
	{
		m_ButtonContrast.Check(true);
		nTabID = IDD_IME_CONTRAST_BRIGHTNESS_TAB;
	}
	else if (IDC_IME_TINT_CHECK == wID)
	{
		m_ButtonTint.Check(true);
		nTabID = IDD_IME_COLORS_TAB;
	}
	else if (IDC_IME_REDEYE_CHECK == wID)
	{
		m_ButtonRedeye.Check(true);
		nTabID = IDD_IME_REDEYE_TAB;
	}
	else if (IDC_IME_BORDERS_CHECK == wID)
	{
		m_ButtonBorder.Check(true);
		nTabID = IDD_IME_BORDERS_TAB;
	}
	else if (IDC_IME_EFFECTS_CHECK == wID)
	{
		m_ButtonEffects.Check(true);
		nTabID = IDD_IME_EFFECTS_TAB;
	}

	m_pCtp->GetDocWindow()->GetSelect().LaunchImageEditor(nTabID);
	ResetToolbarButtons();

	return S_OK;
}
