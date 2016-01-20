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

	m_PanelBkgdColor = RGB(0, 0, 0);
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
	AddToolTip(IDC_FILL_TYPE);
	AddToolTip(IDC_LINE_SIZE);

	AddToolTip(IDC_FILL_COLOR);
	AddToolTip(IDC_FILL_COLOR2);
	AddToolTip(IDC_FILL_COLOR3);
	AddToolTip(IDC_LINE_COLOR);
	//AddToolTip(IDC_COLOR_1);
	//AddToolTip(IDC_COLOR_2);
	//AddToolTip(IDC_COLOR_3);
	//AddToolTip(IDC_COLOR_LINE);
}

//////////////////////////////////////////////////////////////////////
void CToolsCtlPanelCommon::InitToolCtrls(bool bFillType)
{
	m_ColorComboFill.InitDialog(GetDlgItem(IDC_FILL_COLOR));
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
		//UpdateFillCtrls();
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
	
	//UpdateLineCtrls();
	ShowHideColors();

	if (nItem >= 0)
		m_pCtp->m_pDocWindow->SendMessage(WM_COMMAND, IDC_DOC_LINEWIDTHCHANGE);

	if (m_ComboLineWidth.GetAfterCloseUp())
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
	if (m_nResID != IDD_BOTTOOLSPANEL_GRAPHIC && m_nResID != IDD_BOTTOOLSPANEL_TEXT && m_nResID != IDD_BOTTOOLSPANEL_CALENDAR)
		return;

	bool bCalendar = (m_nResID == IDD_BOTTOOLSPANEL_CALENDAR);

	if (::IsWindow(m_ColorComboFill.m_hWnd))
		m_ColorComboFill.ShowWindow(m_CurrentFillType == FT_None && !bCalendar ? SW_HIDE : SW_SHOW);
	if (::IsWindow(m_ColorComboFill2.m_hWnd))
		m_ColorComboFill2.ShowWindow((m_CurrentFillType == FT_None || m_CurrentFillType == FT_Solid) && !bCalendar ? SW_HIDE : SW_SHOW);
	if (::IsWindow(m_ColorComboFill3.m_hWnd))
		m_ColorComboFill3.ShowWindow(SW_SHOW);
	if (::IsWindow(m_ColorComboLine.m_hWnd))
		m_ColorComboLine.ShowWindow(m_LineWidth == LT_None ? SW_HIDE : SW_SHOW);

	if (::IsWindow(m_ComboFillType.m_hWnd))
	{
		CAGSym* pSym = m_pCtp->m_pDocWindow->GetSelectedItem();
		bool bIsLine = (pSym && pSym->IsLine());
		m_ComboFillType.EnableWindow(!bIsLine);
	}
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
	int nItem;
	for (nItem = 0; nItem < nItems; nItem++)
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
LRESULT CToolsCtlPanelCommon::OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT idControl = wParam;
	LPMEASUREITEMSTRUCT pMeasureItem = (LPMEASUREITEMSTRUCT)lParam;
	if (!pMeasureItem)
		return false;

	if (pMeasureItem->CtlType != ODT_COMBOBOX)
		return true;

	HWND hCombo = GetDlgItem(IDC_LINE_SIZE);
	//if (hCombo == NULL)
	//	hCombo = GetDlgItem(IDC_TEXT_SIZE);

	int iHeight = 22;
	if (hCombo)
	{
		RECT rect;
		::GetWindowRect(hCombo, &rect);
		iHeight = HEIGHT(rect);
	}

	hCombo = GetDlgItem(idControl);
	if (pMeasureItem->itemID == -1) // the always visible part
	{
		pMeasureItem->itemHeight = iHeight - 6;
	}
	else	
	{
		pMeasureItem->itemHeight = iHeight - 6;
		//if (idControl == IDC_FONT)
		//	pMeasureItem->itemHeight *= 2;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
//void CToolsCtlPanelCommon::SetFillColor(COLORREF Color)
//{
//	if (Color == CLR_NONE)
//	{
//		m_ColorBtnFill.SetColor(m_PanelBkgdColor);
//		return;
//	}
//
//	m_CurrentFillColor = Color;
//	m_ColorBtnFill.SetColor(Color);
//
//	if (::IsWindow(m_ColorFillDsp.m_hWnd))
//	{
//		m_ColorFillDsp.SetColor1(Color);
//		m_ColorFillDsp.Invalidate();
//	}
//}
//
//////////////////////////////////////////////////////////////////////
//void CToolsCtlPanelCommon::SetFillColor2(COLORREF Color)
//{
//	if (Color == CLR_NONE)
//	{
//		m_ColorBtnFill2.SetColor(m_PanelBkgdColor);
//		return;
//	}
//
//	m_CurrentFillColor2 = Color;
//	m_ColorBtnFill2.SetColor(Color);
//
//	if (::IsWindow(m_ColorFillDsp.m_hWnd))
//	{
//		m_ColorFillDsp.SetColor2(Color);
//		m_ColorFillDsp.Invalidate();
//	}
//}

//////////////////////////////////////////////////////////////////////
//void CToolsCtlPanelCommon::SetFillColor3(COLORREF Color)
//{
//	if (Color == CLR_NONE)
//	{
//		m_ColorBtnFill3.SetColor(m_PanelBkgdColor);
//		return;
//	}
//
//	m_CurrentFillColor3 = Color;
//	m_ColorBtnFill3.SetColor(Color);
//}
//
//////////////////////////////////////////////////////////////////////
//void CToolsCtlPanelCommon::SetLineColor(COLORREF Color)
//{
//	if (Color == CLR_NONE)
//	{
//		m_ColorBtnLine.SetColor(m_PanelBkgdColor);
//		return;
//	}
//
//	m_CurrentLineColor = Color;
//	m_ColorBtnLine.SetColor(Color);
//
//	m_ColorLineDsp.SetColor1(Color);
//	m_ColorLineDsp.SetColor2(Color);
//	m_ColorLineDsp.Invalidate();
//}
//
//////////////////////////////////////////////////////////////////////
//void CToolsCtlPanelCommon::ShowHideColors()
//{
//	if (m_nResID != IDD_TOOLSPANEL_GRAPHIC && m_nResID != IDD_TOOLSPANEL_TEXT && m_nResID != IDD_TOOLSPANEL_CALENDAR)
//		return;
//
//	bool bCalendar = (m_nResID == IDD_TOOLSPANEL_CALENDAR);
//
//	if (::IsWindow(m_ColorBtnFill.m_hWnd))
//		m_ColorBtnFill.ShowWindow(m_CurrentFillType == FT_None && !bCalendar ? false : true);
//	if (::IsWindow(m_ColorBtnFill2.m_hWnd))
//		m_ColorBtnFill2.ShowWindow((m_CurrentFillType == FT_None || m_CurrentFillType == FT_Solid) && !bCalendar ? false : true);
//	if (::IsWindow(m_ColorBtnFill3.m_hWnd))
//		m_ColorBtnFill3.ShowWindow(SW_SHOW);
//
//	if (::IsWindow(m_ColorFillDsp.m_hWnd))
//		m_ColorFillDsp.ShowWindow(m_CurrentFillType == FT_None && !bCalendar ? false : true);
//	if (::IsWindow(m_ColorLineDsp.m_hWnd))
//		m_ColorLineDsp.ShowWindow(m_LineWidth == LT_None ? false : true);
//
//	if (::IsWindow(m_ColorBtnLine.m_hWnd))
//		m_ColorBtnLine.ShowWindow(m_LineWidth == LT_None ? false : true);
//
//	if (::IsWindow(m_ComboFillType.m_hWnd))
//	{
//		CAGSym* pSym = m_pCtp->m_pDocWindow->GetSelectedItem();
//		bool bIsLine = (pSym && pSym->IsLine());
//		m_ComboFillType.EnableWindow(!bIsLine);
//	}
//}
//
//////////////////////////////////////////////////////////////////////
//LRESULT CToolsCtlPanelCommon::OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	if ( (wParam == IDC_COLOR_1) || (wParam == IDC_COLOR_2) || (wParam == IDC_COLOR_3) || (wParam == IDC_COLOR_LINE) )
//	{
//		NMCOLORBUTTON* nmclr = (NMCOLORBUTTON*)lParam;
//		bool bAfterCloseUp = (nmclr->hdr.code == CPN_SELENDOK);
//		if (!bAfterCloseUp)
//		{
//			bHandled = false;
//			return 0;
//		}
//
//		if (nmclr->fColorValid)
//		{
//			if (wParam == IDC_COLOR_1)
//			{
//				m_CurrentFillColor = nmclr->clr;
//				m_ColorFillDsp.SetColor1(m_CurrentFillColor);
//				m_pCtp->m_pDocWindow->OnDocChangeFillColor(m_CurrentFillColor);
//
//				if (m_nResID == IDD_PANEL_IMAGE)
//					CMessageBox::Message("Hint: Color samples picked from the selected photo or image, can be used to color text boxes and shapes.");
//			}
//			else
//			if (wParam == IDC_COLOR_2)
//			{
//				m_CurrentFillColor2 = nmclr->clr;
//				m_ColorFillDsp.SetColor2(m_CurrentFillColor2);
//				m_pCtp->m_pDocWindow->OnDocChangeFillColor2(m_CurrentFillColor2);
//			}
//			else
//			if (wParam == IDC_COLOR_3)
//			{
//				m_CurrentFillColor3 = nmclr->clr;
//				m_pCtp->m_pDocWindow->OnDocChangeFillColor3(m_CurrentFillColor3);
//			}
//			else
//			if (wParam == IDC_COLOR_LINE)
//			{
//				// need a display for line color
//				m_CurrentLineColor = nmclr->clr;
//				m_ColorLineDsp.SetColor1(m_CurrentLineColor);
//				m_ColorLineDsp.SetColor2(m_CurrentLineColor);
//				m_pCtp->m_pDocWindow->OnDocChangeLineColor(m_CurrentLineColor);
//			}
//
//			UpdateFillCtrls();
//			UpdateLineCtrls();
//			m_pCtp->m_pDocWindow->SetFocus();
//			return 0;
//		}
//	}
//
//	bHandled = false;
//	return 0;
//}
//

//////////////////////////////////////////////////////////////////////
//void CToolsCtlPanelCommon::ShowHideColors()
//{
//	//if (m_nResID != IDD_PANEL_GRAPHIC && m_nResID != IDD_PANEL_TEXT && m_nResID != IDD_PANEL_CALENDAR)
//	//	return;
//
//	if (m_nResID != IDD_TOOLSPANEL_GRAPHIC && m_nResID != IDD_TOOLSPANEL_TEXT && m_nResID != IDD_PANEL_CALENDAR)
//		return;
//
//	bool bCalendar = (m_nResID == IDD_PANEL_CALENDAR);
//
//	if (::IsWindow(m_ColorComboFill.m_hWnd))
//		m_ColorComboFill.EnableWindow(m_CurrentFillType == FT_None && !bCalendar ? false : true);
//		//m_ColorComboFill.ShowWindow(m_CurrentFillType == FT_None && !bCalendar ? SW_HIDE : SW_SHOW);
//	if (::IsWindow(m_ColorComboFill2.m_hWnd))
//		m_ColorComboFill2.EnableWindow((m_CurrentFillType == FT_None || m_CurrentFillType == FT_Solid) && !bCalendar ? false : true);
//		//m_ColorComboFill2.ShowWindow((m_CurrentFillType == FT_None || m_CurrentFillType == FT_Solid) && !bCalendar ? SW_HIDE : SW_SHOW);
//	if (::IsWindow(m_ColorComboFill3.m_hWnd))
//		m_ColorComboFill3.ShowWindow(SW_SHOW);
//	if (::IsWindow(m_ColorComboLine.m_hWnd))
//		m_ColorComboLine.EnableWindow(m_LineWidth == LT_None ? false : true);
//		//m_ColorComboLine.ShowWindow(m_LineWidth == LT_None ? SW_HIDE : SW_SHOW);
//
//	if (::IsWindow(m_ComboFillType.m_hWnd))
//	{
//		CAGSym* pSym = m_pCtp->m_pDocWindow->GetSelectedItem();
//		bool bIsLine = (pSym && pSym->IsLine());
//		m_ComboFillType.EnableWindow(!bIsLine);
//	}
//}
//
//////////////////////////////////////////////////////////////////////
//LRESULT CToolsCtlPanelCommon::OnColorComboChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
//{
//	if (!(wNotifyCode & COMBO_NOTIFCATION_FLAG))
//	{ // Let the combo class take care of it
//		bHandled = false;
//		return S_OK;
//	}
//
//	wNotifyCode &= ~COMBO_NOTIFCATION_FLAG;
//	
//	bool bAfterCloseUp = false;
//	if (wID == IDC_FILL_COLOR)
//	{
//		bAfterCloseUp = m_ColorComboFill.GetAfterCloseUp();
//		m_CurrentFillColor = m_ColorComboFill.GetColor();
//		m_pCtp->m_pDocWindow->OnDocChangeFillColor(m_CurrentFillColor);
//
//		if (bAfterCloseUp && m_nResID == IDD_PANEL_IMAGE)
//			CMessageBox::Message("Hint: Color samples picked from the selected photo or image, can be used to color text boxes and shapes.");
//	}
//	else
//	if (wID == IDC_FILL_COLOR2)
//	{
//		bAfterCloseUp = m_ColorComboFill2.GetAfterCloseUp();
//		m_CurrentFillColor2 = m_ColorComboFill2.GetColor();
//		m_pCtp->m_pDocWindow->OnDocChangeFillColor2(m_CurrentFillColor2);
//	}
//	else
//	if (wID == IDC_FILL_COLOR3)
//	{
//		bAfterCloseUp = m_ColorComboFill3.GetAfterCloseUp();
//		m_CurrentFillColor3 = m_ColorComboFill3.GetColor();
//		m_pCtp->m_pDocWindow->OnDocChangeFillColor3(m_CurrentFillColor3);
//	}
//	else
//	if (wID == IDC_LINE_COLOR)
//	{
//		bAfterCloseUp = m_ColorComboLine.GetAfterCloseUp();
//		m_CurrentLineColor = m_ColorComboLine.GetColor();
//		m_pCtp->m_pDocWindow->OnDocChangeLineColor(m_CurrentLineColor);
//	}
//	
//	if (bAfterCloseUp)
//		m_pCtp->m_pDocWindow->SetFocus();
//
//	return S_OK;
//}
//////////////////////////////////////////////////////////////////////
//void CToolsCtlPanelCommon::UpdateLineCtrls()
//{
//	//if (m_LineWidth == LT_None)
//	//{
//	//	m_ColorLineDsp.SetTransparent(true);
//	//	m_ColorLineDsp.Invalidate();
//	//}
//	//else
//	if (m_LineWidth != LT_None)
//	{
//		//m_ColorLineDsp.SetTransparent(false);
//		m_ColorLineDsp.SetColor1(m_CurrentLineColor);
//		m_ColorLineDsp.SetColor2(m_CurrentLineColor);
//		m_ColorLineDsp.Invalidate();
//	}
//}
//
//////////////////////////////////////////////////////////////////////
//void CToolsCtlPanelCommon::UpdateFillCtrls()
//{
//	if (m_CurrentFillType == FT_Solid)
//	{
//		m_CurrentFillColor2 = m_CurrentFillColor;
//		m_pCtp->m_pDocWindow->OnDocChangeFillColor2(m_CurrentFillColor2);
//
//		m_ColorBtnFill2.SetColor(m_CurrentFillColor2);
//		m_ColorBtnFill2.Invalidate();
//
//		if (::IsWindow(m_ColorFillDsp.m_hWnd))
//		{
//			m_ColorFillDsp.SetColor2(m_CurrentFillColor2);
//			//m_ColorFillDsp.SetTransparent(false);
//			m_ColorFillDsp.SetHorzGradient();
//			m_ColorFillDsp.Invalidate();
//		}
//	}
//	else
//	if (m_CurrentFillType == FT_SweepRight)
//	{
//		if (::IsWindow(m_ColorFillDsp.m_hWnd))
//		{
//			m_ColorFillDsp.SetColor1(m_CurrentFillColor);
//			m_ColorFillDsp.SetColor2(m_CurrentFillColor2);
//			//m_ColorFillDsp.SetTransparent(false);
//			m_ColorFillDsp.SetHorzGradient();
//			m_ColorFillDsp.Invalidate();
//		}
//	}
//	else
//	if (m_CurrentFillType == FT_SweepDown)
//	{
//		if (::IsWindow(m_ColorFillDsp.m_hWnd))
//		{
//			m_ColorFillDsp.SetColor1(m_CurrentFillColor);
//			m_ColorFillDsp.SetColor2(m_CurrentFillColor2);
//			m_ColorFillDsp.SetVertGradient();
//			m_ColorFillDsp.Invalidate();
//		}
//	}
//	else
//	if (m_CurrentFillType == FT_None)
//	{
//		if (::IsWindow(m_ColorFillDsp.m_hWnd))
//			m_ColorFillDsp.Invalidate();
//	}
//}
//
//////////////////////////////////////////////////////////////////////
//void CToolsCtlPanelCommon::InitLineCombos(bool bTextPanel)
//{
//	m_ComboLineWidth.InitDialog(GetDlgItem(IDC_LINE_SIZE));
//	m_ColorBtnLine.SubclassWindow(GetDlgItem(IDC_COLOR_LINE));
//
//	m_ColorBtnLine.SetDefaultText(_T(""));
//
//	CRect Rect;
//	m_ColorBtnLine.GetWindowRect(&Rect);
//	this->ScreenToClient(&Rect);
//	CRect dspRect(Rect.left, 2, (Rect.left+WIDTH(Rect)), Rect.top-1);
//	m_ColorLineDsp.Create(m_hWnd, dspRect, "STATIC", WS_CHILD|WS_VISIBLE, 0);
//
//	m_ColorLineDsp.SetColor1(m_PanelBkgdColor);
//	m_ColorLineDsp.SetColor2(m_PanelBkgdColor);
//
//	int nItem = NULL;
//	nItem = m_ComboLineWidth.AddString("Off");
//	m_ComboLineWidth.SetItemData(nItem, LT_None);
//
//	nItem = m_ComboLineWidth.AddString("Thin");
//	m_ComboLineWidth.SetItemData(nItem, LT_Hairline);
//
//	CString strText;
//	if (bTextPanel)
//	{
//		double fIncrement = 0.2;
//		for (double fLineWidth = 0.2; fLineWidth < 5.1; fLineWidth += fIncrement)
//		{
//			strText.Format("%0.3G", fLineWidth);
//			nItem = m_ComboLineWidth.AddString(strText);
//			m_ComboLineWidth.SetItemData(nItem, POINTUNITS(fLineWidth));
//			if (fLineWidth == 1.0) fIncrement = 0.5;
//		}
//	}
//	else
//	{
//		int iIncrement = 1;
//		for (int iLineWidth = 1; iLineWidth <= 50; iLineWidth += iIncrement)
//		{
//			strText.Format("%d", iLineWidth);
//			nItem = m_ComboLineWidth.AddString(strText);
//			m_ComboLineWidth.SetItemData(nItem, POINTUNITS(iLineWidth));
//			if (iLineWidth == 10) iIncrement = 5;
//		}
//
//		m_ComboLineWidth.SetCurSel(-1);
//	}
//}
//
//////////////////////////////////////////////////////////////////////
//void CToolsCtlPanelCommon::InitToolCtrls(bool bFillType)
//{
//	m_ColorBtnFill.SubclassWindow (GetDlgItem (IDC_COLOR_1));
//	m_ColorBtnFill2.SubclassWindow (GetDlgItem (IDC_COLOR_2));
//
//	m_ColorBtnFill.SetDefaultText(_T(""));
//	m_ColorBtnFill2.SetDefaultText(_T(""));
//
//	CRect Rect;
//	m_ColorBtnFill.GetWindowRect(&Rect);
//	this->ScreenToClient(&Rect);
//	CRect dspRect(Rect.left, 2, (Rect.left+(WIDTH(Rect)*2)+2), Rect.top-1);
//	m_ColorFillDsp.Create(m_hWnd, dspRect, "STATIC", WS_CHILD|WS_VISIBLE, 0);
//
//	m_ColorFillDsp.SetColor1(m_PanelBkgdColor);
//	m_ColorFillDsp.SetColor2(m_PanelBkgdColor);
//
//	if (bFillType)
//	{
//		m_ComboFillType.InitDialog(GetDlgItem(IDC_FILL_TYPE));
//
//		LPCSTR szFillTypes[] = { "None", "Solid", "Sweep Right", "Sweep Down" };
//		for (int i = 0; i < sizeof(szFillTypes)/sizeof(LPCSTR); i++)
//			m_ComboFillType.AddString(szFillTypes[i]);
//
//		m_ComboFillType.SetCurSel(0);
//	}
//}
