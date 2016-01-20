#include "stdafx.h"
#include "Ctp.h"
#include "ToolsPanelText1.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CToolsPanelText1::CToolsPanelText1(CCtp* pMainWnd) : CToolsCtlPanelCommon(pMainWnd)
{
	m_nResID = IDD;
	m_iTextSize = POINTUNITS(12);

	m_LineWidth = POINTUNITS(0);
	m_CurrentFillType = FT_Solid;
	m_CurrentFillColor = RGB(0,0,0);
}

//////////////////////////////////////////////////////////////////////
CToolsPanelText1::~CToolsPanelText1()
{
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelText1::ActivateNewDoc()
{
	FillFontSizeCombo();
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelText1::UpdateTextControls(CAGText* pText)
{
	scTypeSpecList tsList;
	pText->GetSelTSList(tsList);
	int nNumItems = tsList.NumItems();
	if (!nNumItems)
		return;

	CAGSpec* pAGSpec = (CAGSpec*)(tsList[0].ptr());

	LOGFONT SymFont = pAGSpec->m_Font;
	int SymTextSize = abs(pAGSpec->m_Font.lfHeight);
	bool bSameFont = true;
	bool bSameBold = true;
	bool bSameItalic = true;
	bool bSameUnderline = true;
	bool bSameTextSize = true;
	
	for (int i = 1; i < nNumItems; i++)
	{
		pAGSpec = (CAGSpec*)(tsList[i].ptr());
		if (lstrcmp(SymFont.lfFaceName, pAGSpec->m_Font.lfFaceName))
			bSameFont = false;

		if (SymFont.lfWeight != pAGSpec->m_Font.lfWeight)
			bSameBold = false;

		if (!SymFont.lfItalic != !pAGSpec->m_Font.lfItalic)
			bSameItalic = false;

		if (!SymFont.lfUnderline != !pAGSpec->m_Font.lfUnderline)
			bSameUnderline = false;

		int nTextSize = abs(pAGSpec->m_Font.lfHeight);
		if (SymTextSize != nTextSize)
			bSameTextSize = false;
	}

	// Update the Text Control
	//SetTitleText("Text");

	//UpdateFillCtrls();
	//UpdateLineCtrls();
	//ShowHideColors();

	// Update the TextSize control
	if (!bSameTextSize)
		SymTextSize = -1;
	SetTextSize(SymTextSize);

	// Update the Font control
	LOGFONT* pFont = &SymFont;
	if (!bSameFont)
		pFont = NULL;
	SetFontControl(pFont, true/*bClearIfBad*/);

	// Update the text style controls
	m_ButtonBold.Check(bSameBold ? SymFont.lfWeight != FW_NORMAL : false);
	m_ButtonItalic.Check(bSameItalic ? !!SymFont.lfItalic : false);
	m_ButtonUnderline.Check(bSameUnderline ? !!SymFont.lfUnderline : false);

	// Update the Vertical adjustment controls
	eVertJust SpecVertJust = pText->GetVertJust();
	m_ButtonTop.Check(SpecVertJust == eVertTop);
	m_ButtonMiddle.Check(SpecVertJust == eVertCentered);
	m_ButtonBottom.Check(SpecVertJust == eVertBottom);

	// Get the spec to determine Horizontal alignment
	scTypeSpecList tsListPara;
	pText->GetSelParaTSList(tsListPara);
	nNumItems = tsListPara.NumItems();
	pAGSpec = (CAGSpec*)(tsListPara[0].ptr());

	// Update the Horizontal adjustment controls
	eTSJust SpecHorzJust = pAGSpec->m_HorzJust;
	for (int i = 1; i < nNumItems; i++)
	{
		pAGSpec = (CAGSpec*)(tsListPara[i].ptr());
		if (SpecHorzJust != pAGSpec->m_HorzJust)
		{
			SpecHorzJust = eNoRag;
			break;
		}
	}

	m_ButtonLeft.Check(SpecHorzJust == eFlushLeft);
	m_ButtonCenter.Check(SpecHorzJust == eRagCentered);
	m_ButtonRight.Check(SpecHorzJust == eFlushRight);
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelText1::UpdateControls()
{
	CAGSym* pSym = m_pCtp->GetDocWindow()->GetSelectedItem();
	if (!pSym)
		return;

	if (pSym->IsText() || (pSym->IsAddAPhoto() && m_pCtp->GetDocWindow()->SpecialsEnabled()))
	{
		CAGText* pText = (CAGText*)(CAGSymText*)pSym;
		UpdateTextControls(pText);
	}
	else if (pSym->IsCalendar())
	{
		CAGSymCalendar* pCalSym = (CAGSymCalendar*)pSym;
		UpdateCalendarTextControls(pCalSym);
	}
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelText1::InitToolTips()
{
	CToolsCtlPanel::InitToolTips();
	AddToolTip(IDC_TEXT_SIZE);
	AddToolTip(IDC_FONT);
	AddToolTip(IDC_BOLD);
	AddToolTip(IDC_ITALIC);
	AddToolTip(IDC_UNDERLINE);
	AddToolTip(IDC_LEFT);
	AddToolTip(IDC_CENTER);
	AddToolTip(IDC_RIGHT);
	AddToolTip(IDC_TOP);
	AddToolTip(IDC_MIDDLE);
	AddToolTip(IDC_BOTTOM);
	AddToolTip(IDC_SPELLCHECK, IDS_SPELLCHECK);
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelText1::FillFontSizeCombo()
{
	bool bDocAvery = ((m_pCtp->m_pDocWindow->GetDoc()) && 
					  (DOC_AVERYPOSTER == m_pCtp->m_pDocWindow->GetDoc()->GetDocType()));

	int iCount = m_ComboTextSize.GetCount();
	if (iCount > 0)
	{
		bool bBigFonts = POINTSIZE((int)m_ComboTextSize.GetItemData(iCount-1)) > 500;

		if ( bDocAvery && bBigFonts )
			return;

		if ( !bDocAvery && !bBigFonts )
			return;

		m_ComboTextSize.ResetContent();
	}

	CString strText;
	if ( bDocAvery )
	{
		int fSizes[] = {20, 28, 36, 48, 72, 108, 144, 180, 228, 300, 360, 432, 504, 576, 648, 720};
		int x = sizeof(fSizes)/sizeof(int);
		for (int i = 0; i < x; i++)
		{
			int iTextSize = fSizes[i];
			strText.Format("%d", iTextSize);
			int nItem = m_ComboTextSize.AddString(strText);
			m_ComboTextSize.SetItemData(nItem, POINTUNITS(iTextSize));
		}
	}
	else
	{
		int iIncrement = 1;
		for (int i = 1; i <= 72; i += iIncrement)
		{
			int iTextSize = i;
			strText.Format("%d", iTextSize);
			int nItem = m_ComboTextSize.AddString(strText);
			m_ComboTextSize.SetItemData(nItem, POINTUNITS(iTextSize));
			if (i == 12) iIncrement++;
		}
	}

}

//////////////////////////////////////////////////////////////////////
void CToolsPanelText1::InitToolCtrls()
{
	m_ComboFonts.InitDialog(GetDlgItem(IDC_FONT));
	m_ComboTextSize.InitDialog(GetDlgItem(IDC_TEXT_SIZE));

	CString strText;
	int iIncrement = 1;
	for (int i = 1; i <= 72; i += iIncrement)
	{
		int iTextSize = i;
		strText.Format("%d", iTextSize);
		int nItem = m_ComboTextSize.AddString(strText);
		m_ComboTextSize.SetItemData(nItem, POINTUNITS(iTextSize));
		if (i == 12) iIncrement++;
	}
	m_ComboTextSize.SetCurSel(-1);

	m_ButtonBold.InitDialog(GetDlgItem(IDC_BOLD), IDR_BOLD);
	m_ButtonItalic.InitDialog(GetDlgItem(IDC_ITALIC), IDR_ITALIC);
	m_ButtonUnderline.InitDialog(GetDlgItem(IDC_UNDERLINE), IDR_UNDERLINE);

	m_ButtonLeft.InitDialog(GetDlgItem(IDC_LEFT), IDR_LEFT);
	m_ButtonCenter.InitDialog(GetDlgItem(IDC_CENTER), IDR_CENTER);
	m_ButtonRight.InitDialog(GetDlgItem(IDC_RIGHT), IDR_RIGHT);

	m_ButtonTop.InitDialog(GetDlgItem(IDC_TOP), IDR_TOP);
	m_ButtonMiddle.InitDialog(GetDlgItem(IDC_MIDDLE), IDR_MIDDLE);
	m_ButtonBottom.InitDialog(GetDlgItem(IDC_BOTTOM), IDR_BOTTOM);

	m_ButtonSpellChk.InitDialog(GetDlgItem(IDC_SPELLCHECK), IDR_SPELLCHECK);
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelText1::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	InitToolCtrls();
	InitToolTips();

	SetFonts();

	m_ComboFonts.SetCurSel(-1);
	long lWidth = m_ComboFonts.GetDroppedWidth();
	long lRet = m_ComboFonts.SetDroppedWidth(dtoi(2.0 * lWidth));

	bHandled = true;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelText1::OnFont(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	static int wLastNotifyCode;
	bool bAfterCloseUp = (wLastNotifyCode == CBN_CLOSEUP);
	wLastNotifyCode = wNotifyCode;

	if (wNotifyCode != CBN_SELCHANGE)
	{
		wLastNotifyCode = wNotifyCode;
		return true;
	}

	int nItem = m_ComboFonts.GetCurSel();
	if (nItem >= 0)
	{
		CAGText* pText = m_pCtp->m_pDocWindow->GetText();
		CAGSymCalendar* pCalendar = m_pCtp->m_pDocWindow->GetCalendar();
		int nFont = m_ComboFonts.GetItemData(nItem);
		if (nFont >= 0)
		{
			LOGFONTEXLIST& FontList = m_pCtp->GetFontList().GetInstalledFonts();
			if (pText)
				pText->SetTypeface(FontList[nFont].lf);
			else
			if (pCalendar)
			{
				pCalendar->SetSpecTypeface(FontList[nFont].lf);
				m_pCtp->m_pDocWindow->SymbolInvalidate(pCalendar);
			}
			bool bBold = m_ButtonBold.IsChecked();
			bool bItalic = m_ButtonItalic.IsChecked();
			bool bUnderline = m_ButtonUnderline.IsChecked();
			if (pText)
				pText->SetEmphasis(bBold, bItalic, bUnderline);
			else
			if (pCalendar)
			{
				pCalendar->SetSpecEmphasis(bBold, bItalic, bUnderline);
				m_pCtp->m_pDocWindow->SymbolInvalidate(pCalendar);
			}
		}
	}

	if (bAfterCloseUp)
		m_pCtp->m_pDocWindow->SetFocus();

	return true;
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelText1::OnTextSize(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
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
		char szTextSize[20];
		szTextSize[0] = '\0';
		m_ComboTextSize.GetWindowText(szTextSize, sizeof(szTextSize));
		m_iTextSize = POINTUNITS(atof(szTextSize));
	}
	else
	{
		nItem = m_ComboTextSize.GetCurSel();
		m_iTextSize = (int)m_ComboTextSize.GetItemData(nItem);
	}

	if (nItem >= 0)
		m_pCtp->m_pDocWindow->SendMessage(WM_COMMAND, IDC_DOC_TEXTSIZECHANGE);

	if (m_ComboTextSize.GetAfterCloseUp())
		m_pCtp->m_pDocWindow->SetFocus();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelText1::OnHorzJust(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CAGText* pText = m_pCtp->GetDocWindow()->GetText();
	CAGSymCalendar* pCalendar = m_pCtp->GetDocWindow()->GetCalendar();
	switch (wID)
	{
		case IDC_LEFT:
			m_ButtonLeft.Check(true);
			m_ButtonCenter.Check(false);
			m_ButtonRight.Check(false);
			if (pText)
				pText->SetHorzJust(eFlushLeft);
			else
			if (pCalendar)
			{
				pCalendar->SetSpecHorzJust(eFlushLeft);
				m_pCtp->GetDocWindow()->SymbolInvalidate(pCalendar);
			}
			break;

		case IDC_CENTER:
			m_ButtonLeft.Check(false);
			m_ButtonCenter.Check(true);
			m_ButtonRight.Check(false);
			if (pText)
				pText->SetHorzJust(eRagCentered);
			else
			if (pCalendar)
			{
				pCalendar->SetSpecHorzJust(eRagCentered);
				m_pCtp->GetDocWindow()->SymbolInvalidate(pCalendar);
			}
			break;

		case IDC_RIGHT:
			m_ButtonLeft.Check(false);
			m_ButtonCenter.Check(false);
			m_ButtonRight.Check(true);
			if (pText)
				pText->SetHorzJust(eFlushRight);
			else
			if (pCalendar)
			{
				pCalendar->SetSpecHorzJust(eFlushRight);
				m_pCtp->GetDocWindow()->SymbolInvalidate(pCalendar);
			}
			break;
	}

	m_pCtp->GetDocWindow()->SetFocus();
	return true;
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelText1::OnSpellCheck(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_pCtp->GetDocWindow()->DoSpellCheck();
	m_pCtp->GetDocWindow()->SetFocus();
	return true;
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelText1::OnVertJust(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CAGText* pText = m_pCtp->GetDocWindow()->GetText();
	CAGSymCalendar* pCalendar = m_pCtp->GetDocWindow()->GetCalendar();
	switch (wID)
	{
		case IDC_TOP:
			m_ButtonMiddle.Check(false);
			m_ButtonBottom.Check(false);
			if (pText)
				pText->SetVertJust(eVertTop);
			else
			if (pCalendar)
			{
				pCalendar->SetSpecVertJust(eVertTop);
				m_pCtp->GetDocWindow()->SymbolInvalidate(pCalendar);
			}
			break;

		case IDC_MIDDLE:
			m_ButtonTop.Check(false);
			m_ButtonBottom.Check(false);
			if (pText)
				pText->SetVertJust(eVertCentered);
			else
			if (pCalendar)
			{
				pCalendar->SetSpecVertJust(eVertCentered);
				m_pCtp->GetDocWindow()->SymbolInvalidate(pCalendar);
			}
			break;

		case IDC_BOTTOM:
			m_ButtonTop.Check(false);
			m_ButtonMiddle.Check(false);
			if (pText)
				pText->SetVertJust(eVertBottom);
			else
			if (pCalendar)
			{
				pCalendar->SetSpecVertJust(eVertBottom);
				m_pCtp->GetDocWindow()->SymbolInvalidate(pCalendar);
			}
			break;
	}

	m_pCtp->GetDocWindow()->SetFocus();
	return true;
}

//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelText1::OnBoldItalicUnderline(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CAGText* pText = m_pCtp->m_pDocWindow->GetText();
	CAGSymCalendar* pCalendar = m_pCtp->m_pDocWindow->GetCalendar();

	bool bBold = m_ButtonBold.IsChecked();
	bool bItalic = m_ButtonItalic.IsChecked();
	bool bUnderline = m_ButtonUnderline.IsChecked();
	if (pText)
		pText->SetEmphasis(bBold, bItalic, bUnderline);
	else
	if (pCalendar)
	{
		pCalendar->SetSpecEmphasis(bBold, bItalic, bUnderline);
		m_pCtp->m_pDocWindow->SymbolInvalidate(pCalendar);
	}

	m_pCtp->m_pDocWindow->SetFocus();
	return true;
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelText1::SetFonts()
{
	m_ComboFonts.ResetContent();

	const LOGFONTEXLIST& FontList = m_pCtp->GetFontList().GetInstalledFonts();
	int nFonts = FontList.size();
	for (int i = 0; i < nFonts; i++)
	{
		bool bIsOurFont = m_pCtp->GetFontList().IsOurFont(FontList[i].lf);
		char strRegistryName[LF_FULLFACESIZE];
		if (bIsOurFont)
		{
			strRegistryName[0] = ' ';
			strRegistryName[1] = '\0';
		}
		else
			strRegistryName[0] = '\0';

		lstrcat(strRegistryName, FontList[i].strRegistryName);
		int nItem = m_ComboFonts.FindStringExact(-1, strRegistryName);
		if (nItem == CB_ERR)
		{
			nItem = m_ComboFonts.AddString(strRegistryName);
			m_ComboFonts.SetItemData(nItem, i);
		}
	}

	// Add a horizontal line item between our fonts and the users fonts
	int nItem = m_ComboFonts.AddString("@");
	m_ComboFonts.SetItemData(nItem, -1);
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelText1::SetTextSize(int iTextSize)
{
	if (iTextSize == -1)
	{
		m_ComboTextSize.SetCurSel(-1);
		return;
	}

	m_iTextSize = iTextSize;

	// Find a matching text size in the combobox
	int nItems = m_ComboTextSize.GetCount();
	int nItemBefore = 0;
	int nItem = 0;
	for (nItem = 0; nItem < nItems; nItem++)
	{
		int iItemData = (int)m_ComboTextSize.GetItemData(nItem);
		if (m_iTextSize == iItemData)
			break;

		if (m_iTextSize > iItemData)
			nItemBefore = nItem + 1;
	}

	if (nItem >= nItems)
	{
		CString strText;
		strText.Format("%0.5G", (double)POINTSIZE(m_iTextSize));
		if (nItemBefore < nItems)
			nItem = m_ComboTextSize.InsertString(nItemBefore, strText);
		else
			nItem = m_ComboTextSize.AddString(strText);
		m_ComboTextSize.SetItemData(nItem, m_iTextSize);
	}

	m_ComboTextSize.SetCurSel(nItem);
}

//////////////////////////////////////////////////////////////////////
//LRESULT CToolsPanelText1::OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//{
//	UINT idControl = wParam;
//	LPMEASUREITEMSTRUCT pMeasureItem = (LPMEASUREITEMSTRUCT)lParam;
//	if (!pMeasureItem)
//		return false;
//
//	if (pMeasureItem->CtlType != ODT_COMBOBOX)
//		return true;
//
//	HWND hCombo = GetDlgItem(IDC_LINE_SIZE);
//	if (hCombo == NULL)
//		hCombo = GetDlgItem(IDC_TEXT_SIZE);
//
//	int iHeight = 22;
//	if (hCombo)
//	{
//		RECT rect;
//		::GetWindowRect(hCombo, &rect);
//		iHeight = HEIGHT(rect);
//	}
//
//	hCombo = GetDlgItem(idControl);
//	if (pMeasureItem->itemID == -1) // the always visible part
//	{
//		pMeasureItem->itemHeight = iHeight - 6;
//	}
//	else	
//	{
//		pMeasureItem->itemHeight = iHeight - 6;
//		if (idControl == IDC_FONT)
//			pMeasureItem->itemHeight *= 2;
//	}
//
//	return true;
//}
//
//////////////////////////////////////////////////////////////////////
LRESULT CToolsPanelText1::OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT idControl = wParam;
	LPDRAWITEMSTRUCT pDrawItem = (LPDRAWITEMSTRUCT)lParam;
	if (!pDrawItem)
		return false;

	if (idControl == IDC_FONT)
	{
		if ((int)pDrawItem->itemID != -1)
		{
			LOGFONTEXLIST& FontList = m_pCtp->GetFontList().GetInstalledFonts();
			LOGFONT NewFont;
			memset(&NewFont, 0, sizeof(NewFont));
			NewFont.lfCharSet = ANSI_CHARSET; //j DEFAULT_CHARSET
			char strRegistryName[LF_FULLFACESIZE];

			int nFont = ::SendMessage(pDrawItem->hwndItem, CB_GETITEMDATA, pDrawItem->itemID, 0);
			if (nFont >= 0)
			{
				NewFont = FontList[nFont].lf;
				lstrcpy(strRegistryName, FontList[nFont].strRegistryName);
			}
			else
			{
				lstrcpy(strRegistryName, "------------------------------------------------------------");
			}

			NewFont.lfHeight = HEIGHT(pDrawItem->rcItem);
			NewFont.lfWidth = 0;

			bool bDropDownItem = !(pDrawItem->itemState & ODS_COMBOBOXEDIT);
			if (!bDropDownItem || NewFont.lfCharSet == SYMBOL_CHARSET)
			{
				lstrcpy(NewFont.lfFaceName, "Arial");
				NewFont.lfCharSet = ANSI_CHARSET;
				NewFont.lfPitchAndFamily = FF_SWISS;
				NewFont.lfHeight = -10;
			}

			::SaveDC(pDrawItem->hDC);
			::SetTextAlign(pDrawItem->hDC, TA_LEFT | TA_TOP | TA_NOUPDATECP);


			if (pDrawItem->itemState & ODS_SELECTED)
			{
				::SetTextColor(pDrawItem->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
				::SetBkColor(pDrawItem->hDC, GetSysColor(COLOR_HIGHLIGHT));
			}

			HFONT hFont = ::CreateFontIndirect(&NewFont);
			HFONT hOldFont = (HFONT)::SelectObject(pDrawItem->hDC, hFont);
			RECT rect = pDrawItem->rcItem;
			::ExtTextOut(pDrawItem->hDC, rect.left, rect.top, ETO_CLIPPED | ETO_OPAQUE, &rect, strRegistryName, lstrlen(strRegistryName), NULL);

		//j	if (pDrawItem->itemState & ODS_FOCUS || pDrawItem->itemState & ODS_SELECTED)
			if (pDrawItem->itemState & ODS_FOCUS)
				::DrawFocusRect(pDrawItem->hDC, &rect);

			::SelectObject(pDrawItem->hDC, hOldFont);
			::DeleteObject(hFont);
			::RestoreDC(pDrawItem->hDC, -1);
		}
	}
	else
	{
		bHandled = false;
		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelText1::SetFontControl(LOGFONT* pFont, bool bClearIfBad)
{
	if (pFont)
	{
		LOGFONTEXLIST& FontList = m_pCtp->GetFontList().GetInstalledFonts();
		int nFonts = FontList.size();
		int i = 0;
		for (i = 0; i < nFonts; i++)
		{
			CString str = FontList[i].lf.lfFaceName;
			if (!lstrcmp(pFont->lfFaceName, FontList[i].lf.lfFaceName))
				break;
		}

		if (i >= nFonts)
		{
			if (bClearIfBad)
				m_ComboFonts.SetCurSel(-1);
		}
		else
		{
			bool bIsOurFont = m_pCtp->GetFontList().IsOurFont(FontList[i].lf);
			char strRegistryName[LF_FULLFACESIZE];
			if (bIsOurFont)
			{
				strRegistryName[0] = ' ';
				strRegistryName[1] = '\0';
			}
			else
				strRegistryName[0] = '\0';
			lstrcat(strRegistryName, FontList[i].strRegistryName);

			int nItem = m_ComboFonts.FindStringExact(-1, strRegistryName);
			m_ComboFonts.SetCurSel(nItem);
		}
	}
	else
	if (bClearIfBad)
	{
		m_ComboFonts.SetCurSel(-1);
	}
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelText1::UpdateCalendarTextControls(CAGSymCalendar* pCalendarSym)
{
	CAGSpec* pAGSpec = pCalendarSym->GetActiveSpec();
	if (!pAGSpec)
		return;
	
	// Update the Title control
	//SetTitleText(pCalendarSym->GetActivePanelTitle());

	//UpdateFillCtrls();
	//UpdateLineCtrls();
	//ShowHideColors();

	// Update the TextSize control
	SetTextSize(abs(pAGSpec->m_Font.lfHeight));

	// Update the Font control
	LOGFONT& SymFont = pAGSpec->m_Font;
	SetFontControl(&pAGSpec->m_Font, true/*bClearIfBad*/);

	// Update the text style controls
	m_ButtonBold.Check(SymFont.lfWeight != FW_NORMAL);
	m_ButtonItalic.Check(!!SymFont.lfItalic);
	m_ButtonUnderline.Check(!!SymFont.lfUnderline);

	// Update the Vertical adjustment controls
	UpdateTextAlignCtrls(pCalendarSym);
}

//////////////////////////////////////////////////////////////////////
void CToolsPanelText1::UpdateTextAlignCtrls(CAGSymCalendar* pCalSym)
{
	if (pCalSym)
	{
		eVertJust SpecVertJust = (eVertJust)pCalSym->GetVertJust();
		m_ButtonTop.Check(SpecVertJust == eVertTop);
		m_ButtonMiddle.Check(SpecVertJust == eVertCentered);
		m_ButtonBottom.Check(SpecVertJust == eVertBottom);

		// Update the Horizontal adjustment controls
		eTSJust SpecHorzJust = (eTSJust)pCalSym->GetHorzJust();
		m_ButtonLeft.Check(SpecHorzJust == eFlushLeft);
		m_ButtonCenter.Check(SpecHorzJust == eRagCentered);
		m_ButtonRight.Check(SpecHorzJust == eFlushRight);
	}
}
