// CtlPanelText.cpp: implementation of the CCtlPanelText class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CtlPanelText.h"
#include "Image.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCtlPanelText::CCtlPanelText(CCtp* pMainWnd) : CCtlPanel(pMainWnd, IDD_PANEL_TEXT)
{
}

//////////////////////////////////////////////////////////////////////
CCtlPanelText::~CCtlPanelText()
{
}

//////////////////////////////////////////////////////////////////////
LRESULT CCtlPanelText::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bool bTextPanel = (m_nResID == IDD_PANEL_TEXT);
	if (bTextPanel)
	{
		CCtlPanel::InitLabels();
		CCtlPanel::InitLineCombos(true);

		m_ColorComboFill.InitDialog(GetDlgItem(IDC_FILL_COLOR));
		m_ColorComboFill2.InitDialog(GetDlgItem(IDC_FILL_COLOR2));

		m_ComboFillType.InitDialog(GetDlgItem(IDC_FILL_TYPE));
		LPCSTR szFillTypes[] = { "None", "Solid", "Sweep Right", "Sweep Down" };
		for (int i = 0; i < sizeof(szFillTypes)/sizeof(LPCSTR); i++)
			m_ComboFillType.AddString(szFillTypes[i]);
		m_ComboFillType.SetCurSel(0);

		m_ComboTextSize.InitDialog(GetDlgItem(IDC_TEXT_SIZE));
		m_ComboFonts.InitDialog(GetDlgItem(IDC_FONT));

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

		m_ButtonBold.InitDialog(GetDlgItem(IDC_BOLD), IDB_BOLD);
		m_ButtonItalic.InitDialog(GetDlgItem(IDC_ITALIC), IDB_ITALIC);
		m_ButtonUnderline.InitDialog(GetDlgItem(IDC_UNDERLINE), IDB_UNDERLINE);

		m_ButtonLeft.InitDialog(GetDlgItem(IDC_LEFT), IDB_LEFT);
		m_ButtonCenter.InitDialog(GetDlgItem(IDC_CENTER), IDB_CENTER);
		m_ButtonRight.InitDialog(GetDlgItem(IDC_RIGHT), IDB_RIGHT);

		m_ButtonTop.InitDialog(GetDlgItem(IDC_TOP), IDB_TOP);
		m_ButtonMiddle.InitDialog(GetDlgItem(IDC_MIDDLE), IDB_MIDDLE);
		m_ButtonBottom.InitDialog(GetDlgItem(IDC_BOTTOM), IDB_BOTTOM);
		m_ButtonPrev.InitDialog(GetDlgItem(IDC_BACK), IDB_BACK);

#ifdef NOTUSED
		{
			m_ButtonPrev.SubclassWindow(GetDlgItem(IDC_BACK));
			CImage Image(_AtlBaseModule.GetResourceInstance(), IDB_BACK, "GIF");
			HBITMAP hBitmap = Image.GetBitmapHandle();
			BITMAP bm;

			::GetObject(hBitmap, sizeof(bm), &bm);
			int dx = bm.bmWidth/3;
			int dy = bm.bmHeight;

			m_ImageList.Create(dx, dy, ILC_COLOR24, 1, 1);
			m_ImageList.Add(hBitmap);
			m_ButtonPrev.SetImageList(m_ImageList.m_hImageList);
			m_ButtonPrev.SetImages(0, 1, 0, 0);
			m_ButtonPrev.SizeToImage();
			m_ButtonPrev.SetBitmapButtonExtendedStyle(BMPBTN_AUTO3D_DOUBLE);
		}
#endif NOTUSED

		SetFonts();

		m_ComboFonts.SetCurSel(-1);
		long lWidth = m_ComboFonts.GetDroppedWidth();
		long lRet = m_ComboFonts.SetDroppedWidth(dtoi(2.0 * lWidth));

		::ShowWindow(GetDlgItem(IDC_BACK), SW_HIDE);
	}

	bHandled = true;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CCtlPanelText::OnHorzJust(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
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
LRESULT CCtlPanelText::OnVertJust(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
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
void CCtlPanelText::UpdateControls()
{
	CAGSym* pSym = m_pCtp->GetDocWindow()->GetSelectedItem();
	if (!pSym)
		return;

	if (pSym->IsText() || (pSym->IsAddAPhoto() && m_pCtp->GetDocWindow()->SpecialsEnabled()))
	{
		CAGText* pText = (CAGText*)(CAGSymText*)pSym;
		CCtlPanel::UpdateTextControls(pText);
	}
	else if (pSym->IsCalendar())
	{
		CAGSymCalendar* pCalSym = (CAGSymCalendar*)pSym;
		UpdateCalendarTextControls(pCalSym);
	}
}

void CCtlPanelText::UpdateTextAlignCtrls(CAGSymCalendar* pCalSym)
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

//////////////////////////////////////////////////////////////////////
void CCtlPanelText::UpdateCalendarTextControls(CAGSymCalendar* pCalendarSym)
{
	CAGSpec* pAGSpec = pCalendarSym->GetActiveSpec();
	if (!pAGSpec)
		return;
	
	// Update the FillType control
	SetFillType(pAGSpec->m_FillType);

	// Update the FillColor control
	SetFillColor(pAGSpec->m_FillColor);

	// Update the FillColor2 control
	SetFillColor2(pAGSpec->m_FillColor2);

	// Update the LineWidth control
	SetLineWidth(pAGSpec->m_LineWidth);

	// Update the LineColor control
	SetLineColor(pAGSpec->m_LineColor);

	// Update the Title control
	SetTitleText(pCalendarSym->GetActivePanelTitle());

	ShowHideColors();

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

	::ShowWindow(GetDlgItem(IDC_BACK), SW_SHOW);
}

