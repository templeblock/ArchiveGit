#include "stdafx.h"
#include "ToolsCtlPanelCommon.h"
#include "Ctp.h"
#include "Image.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolsCtlPanelCommon::CToolsCtlPanelCommon(CCtp* pMainWnd) : CToolsCtlPanel(pMainWnd)
{
	m_nResID = IDD;
	m_CurrentLineColor = RGB(0,0,0);
	m_CurrentFillColor = MYRGB(0xB0C4DE); //RGB(135,206,250);
	m_CurrentFillColor2 = MYRGB(0x87CEFA); //RGB(238,130,238);
	m_CurrentFillColor3 = RGB(255,255,255);
	m_CurrentFillType = FT_None;
	m_LineWidth = POINTUNITS(2);
}

//////////////////////////////////////////////////////////////////////
CToolsCtlPanelCommon::~CToolsCtlPanelCommon()
{
}

//////////////////////////////////////////////////////////////////////
void CToolsCtlPanelCommon::UpdateControls()
{
}

//////////////////////////////////////////////////////////////////////
void CToolsCtlPanelCommon::InitLineCombos(bool bTextPanel)
{
	m_ComboLineWidth.InitDialog(GetDlgItem(IDC_LINE_SIZE));
	m_ColorComboLine.InitDialog(GetDlgItem(IDC_LINE_COLOR));

	int nItem = NULL;
	nItem = m_ComboLineWidth.AddString("Off");
	m_ComboLineWidth.SetItemData(nItem, LT_None);

	nItem = m_ComboLineWidth.AddString("Thin");
	m_ComboLineWidth.SetItemData(nItem, LT_Hairline);

	CString strText;
	if (bTextPanel)
	{
		double fIncrement = 0.2;
		for (double fLineWidth = 0.2; fLineWidth < 5.1; fLineWidth += fIncrement)
		{
			strText.Format("%0.3G", fLineWidth);
			nItem = m_ComboLineWidth.AddString(strText);
			m_ComboLineWidth.SetItemData(nItem, POINTUNITS(fLineWidth));
			if (fLineWidth == 1.0) fIncrement = 0.5;
		}
	}
	else
	{
		int iIncrement = 1;
		for (int iLineWidth = 1; iLineWidth <= 50; iLineWidth += iIncrement)
		{
			strText.Format("%d", iLineWidth);
			nItem = m_ComboLineWidth.AddString(strText);
			m_ComboLineWidth.SetItemData(nItem, POINTUNITS(iLineWidth));
			if (iLineWidth == 10) iIncrement = 5;
		}

		m_ComboLineWidth.SetCurSel(-1);
	}
}

//////////////////////////////////////////////////////////////////////
void CToolsCtlPanelCommon::InitToolTips()
{
	CToolsCtlPanel::InitToolTips();
	AddToolTip(IDC_LINE_SIZE);
	AddToolTip(IDC_FILL_COLOR);
	AddToolTip(IDC_FILL_COLOR2);
	AddToolTip(IDC_FILL_COLOR3);
	AddToolTip(IDC_LINE_COLOR);
	AddToolTip(IDC_FILL_TYPE);
}

//////////////////////////////////////////////////////////////////////
void CToolsCtlPanelCommon::InitToolCtrls(bool bFillType)
{
	m_ColorComboFill.InitDialog(GetDlgItem(IDC_FILL_COLOR), true);
	m_ColorComboFill2.InitDialog(GetDlgItem(IDC_FILL_COLOR2));
	if (bFillType)
	{
		m_ComboFillType.InitDialog(GetDlgItem(IDC_FILL_TYPE));

		LPCSTR szFillTypes[] = { "None", "Solid", "Sweep Right", "Sweep Down" };
		for (int i = 0; i < sizeof(szFillTypes)/sizeof(LPCSTR); i++)
			m_ComboFillType.AddString(szFillTypes[i]);

		m_ComboFillType.SetCurSel(0);
	}
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsCtlPanelCommon::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	InitToolCtrls(false);
	InitToolTips();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsCtlPanelCommon::OnFillType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (!(wNotifyCode & COMBO_NOTIFCATION_FLAG))
	{ // Let the combo class take care of it
		bHandled = false;
		return S_OK;
	}

	wNotifyCode &= ~COMBO_NOTIFCATION_FLAG;

	FillType fItem = (FillType)m_ComboFillType.GetCurSel();
	if (fItem >= 0)
	{
		m_CurrentFillType = fItem;
		ShowHideColors();
	}

	m_pCtp->m_pDocWindow->SendMessage(WM_COMMAND, IDC_DOC_FILLTYPECHANGE);

	if (m_ComboFillType.GetAfterCloseUp())
		m_pCtp->m_pDocWindow->SetFocus();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsCtlPanelCommon::OnLineWidth(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (!(wNotifyCode & COMBO_NOTIFCATION_FLAG))
	{ // Let the combo class take care of it
		bHandled = false;
		return S_OK;
	}

	wNotifyCode &= ~COMBO_NOTIFCATION_FLAG;

	int nItem = -1;

	if (wNotifyCode == CBN_EDITUPDATE)
	{
		nItem = 0;
		char szLineWidth[20];
		szLineWidth[0] = '\0';
		m_ComboLineWidth.GetWindowText(szLineWidth, sizeof(szLineWidth));
		m_LineWidth = POINTUNITS(atof(szLineWidth));
	}
	else
	{
		nItem = m_ComboLineWidth.GetCurSel();
		m_LineWidth = (int)m_ComboLineWidth.GetItemData(nItem);
	}
	
	ShowHideColors();

	if (nItem >= 0)
		m_pCtp->m_pDocWindow->SendMessage(WM_COMMAND, IDC_DOC_LINEWIDTHCHANGE);

	if (m_ComboLineWidth.GetAfterCloseUp())
		m_pCtp->m_pDocWindow->SetFocus();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsCtlPanelCommon::OnColorComboChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (!(wNotifyCode & COMBO_NOTIFCATION_FLAG))
	{ // Let the combo class take care of it
		bHandled = false;
		return S_OK;
	}

	wNotifyCode &= ~COMBO_NOTIFCATION_FLAG;
	
	bool bAfterCloseUp = false;
	if (wID == IDC_FILL_COLOR)
	{
		bAfterCloseUp = m_ColorComboFill.GetAfterCloseUp();
		m_CurrentFillColor = m_ColorComboFill.GetColor();
		m_pCtp->m_pDocWindow->OnDocChangeFillColor(m_CurrentFillColor);

		if (bAfterCloseUp && m_nResID == IDD_PANEL_IMAGE)
			CMessageBox::Message("Hint: Color samples picked from the selected photo or image, can be used to color text boxes and shapes.");
	}
	else
	if (wID == IDC_FILL_COLOR2)
	{
		bAfterCloseUp = m_ColorComboFill2.GetAfterCloseUp();
		m_CurrentFillColor2 = m_ColorComboFill2.GetColor();
		m_pCtp->m_pDocWindow->OnDocChangeFillColor2(m_CurrentFillColor2);
	}
	else
	if (wID == IDC_FILL_COLOR3)
	{
		bAfterCloseUp = m_ColorComboFill3.GetAfterCloseUp();
		m_CurrentFillColor3 = m_ColorComboFill3.GetColor();
		m_pCtp->m_pDocWindow->OnDocChangeFillColor3(m_CurrentFillColor3);
	}
	else
	if (wID == IDC_LINE_COLOR)
	{
		bAfterCloseUp = m_ColorComboLine.GetAfterCloseUp();
		m_CurrentLineColor = m_ColorComboLine.GetColor();
		m_pCtp->m_pDocWindow->OnDocChangeLineColor(m_CurrentLineColor);
	}
	
	if (bAfterCloseUp)
		m_pCtp->m_pDocWindow->SetFocus();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
void CToolsCtlPanelCommon::SetFillType(FillType Fill)
{
	if (Fill == -1)
	{
		m_ComboFillType.SetCurSel(-1);
		return;
	}
	m_CurrentFillType = Fill;
	int nItems = m_ComboFillType.GetCount();
	m_ComboFillType.SetCurSel((int)Fill < nItems ? Fill : -1);
}

//////////////////////////////////////////////////////////////////////
void CToolsCtlPanelCommon::SetFillColor(COLORREF Color)
{
	if (Color == CLR_NONE)
	{
		m_ColorComboFill.SetCurSel(-1);
		return;
	}

	m_CurrentFillColor = Color;
	m_ColorComboFill.SelectColor(Color, true/*bCustomColorsOnly*/);
}

//////////////////////////////////////////////////////////////////////
void CToolsCtlPanelCommon::SetFillColor2(COLORREF Color)
{
	if (Color == CLR_NONE)
	{
		m_ColorComboFill2.SetCurSel(-1);
		return;
	}

	m_CurrentFillColor2 = Color;
	m_ColorComboFill2.SelectColor(Color, true/*bCustomColorsOnly*/);
}

//////////////////////////////////////////////////////////////////////
void CToolsCtlPanelCommon::SetFillColor3(COLORREF Color)
{
	if (Color == CLR_NONE)
	{
		m_ColorComboFill3.SetCurSel(-1);
		return;
	}

	m_CurrentFillColor3 = Color;
	m_ColorComboFill3.SelectColor(Color, true/*bCustomColorsOnly*/);
}

//////////////////////////////////////////////////////////////////////
void CToolsCtlPanelCommon::SetLineWidth(int iLineWidth)
{
	if (iLineWidth == -999)
	{
		m_ComboLineWidth.SetCurSel(-1);
		return;
	}

	m_LineWidth = iLineWidth;

	// Find a matching line width in the combobox
	int nItems = m_ComboLineWidth.GetCount();
	int nItemBefore = 0;
	for (int nItem = 0; nItem < nItems; nItem++)
	{
		int iItemData = (int)m_ComboLineWidth.GetItemData(nItem);
		if (m_LineWidth == iItemData)
			break;

		if (m_LineWidth > iItemData)
			nItemBefore = nItem + 1;
	}

	if (nItem >= nItems)
	{
		CString strText;
		strText.Format("%0.5G", (double)POINTSIZE(m_LineWidth));
		if (nItemBefore < nItems)
			nItem = m_ComboLineWidth.InsertString(nItemBefore, strText);
		else
			nItem = m_ComboLineWidth.AddString(strText);
		m_ComboLineWidth.SetItemData(nItem, m_LineWidth);
	}
	
	m_ComboLineWidth.SetCurSel(nItem);
}

//////////////////////////////////////////////////////////////////////
void CToolsCtlPanelCommon::SetLineColor(COLORREF Color)
{
	if (Color == CLR_NONE)
	{
		m_ColorComboLine.SetCurSel(-1);
		return;
	}

	m_CurrentLineColor = Color;
	m_ColorComboLine.SelectColor(Color, true/*bCustomColorsOnly*/);
}

//////////////////////////////////////////////////////////////////////
void CToolsCtlPanelCommon::ShowHideColors()
{
	//if (m_nResID != IDD_PANEL_GRAPHIC && m_nResID != IDD_PANEL_TEXT && m_nResID != IDD_PANEL_CALENDAR)
	//	return;

	if (m_nResID != IDD_TOOLSPANEL_GRAPHIC && m_nResID != IDD_TOOLSPANEL_TEXT && m_nResID != IDD_PANEL_CALENDAR)
		return;

	bool bCalendar = (m_nResID == IDD_PANEL_CALENDAR);

	if (::IsWindow(m_ColorComboFill.m_hWnd))
		m_ColorComboFill.EnableWindow(m_CurrentFillType == FT_None && !bCalendar ? false : true);
		//m_ColorComboFill.ShowWindow(m_CurrentFillType == FT_None && !bCalendar ? SW_HIDE : SW_SHOW);
	if (::IsWindow(m_ColorComboFill2.m_hWnd))
		m_ColorComboFill2.EnableWindow((m_CurrentFillType == FT_None || m_CurrentFillType == FT_Solid) && !bCalendar ? false : true);
		//m_ColorComboFill2.ShowWindow((m_CurrentFillType == FT_None || m_CurrentFillType == FT_Solid) && !bCalendar ? SW_HIDE : SW_SHOW);
	if (::IsWindow(m_ColorComboFill3.m_hWnd))
		m_ColorComboFill3.ShowWindow(SW_SHOW);
	if (::IsWindow(m_ColorComboLine.m_hWnd))
		m_ColorComboLine.EnableWindow(m_LineWidth == LT_None ? false : true);
		//m_ColorComboLine.ShowWindow(m_LineWidth == LT_None ? SW_HIDE : SW_SHOW);

	if (::IsWindow(m_ComboFillType.m_hWnd))
	{
		CAGSym* pSym = m_pCtp->m_pDocWindow->GetSelectedItem();
		bool bIsLine = (pSym && pSym->IsLine());
		m_ComboFillType.EnableWindow(!bIsLine);
	}
}


