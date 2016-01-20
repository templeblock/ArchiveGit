#include "stdafx.h"
#include "CtlPanel.h"
#include "Ctp.h"
#include "AGPage.h"
#include "AGLayer.h"
#include "AGText.h"
#include <CommDlg.h>
#include "Version.h"
#include "resource.h"
#include "Image.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

#define MyB(rgb)	((BYTE)(rgb))
#define MyG(rgb)	((BYTE)(((WORD)(rgb)) >> 8))
#define MyR(rgb)	((BYTE)((rgb)>>16))
#define MYRGB(hex)	RGB(MyR(hex), MyG(hex), MyB(hex))

//////////////////////////////////////////////////////////////////////
CCtlPanel::CCtlPanel(CCtp* pMainWnd, UINT nResID)
{
	m_nResID = nResID;
	m_pCtp = pMainWnd;
	m_CurrentLineColor = RGB(0,0,0);
	m_CurrentFillColor = MYRGB(0xB0C4DE); //RGB(135,206,250);
	m_CurrentFillColor2 = MYRGB(0x87CEFA); //RGB(238,130,238);
	m_CurrentFillColor3 = RGB(255,255,255);
	m_CurrentFillType = FT_None;
	m_LineWidth = POINTUNITS(2);
	m_iTextSize = POINTUNITS(12);
	m_iCalendarMonth = 1;
	m_iCalendarYear = 2004;
	m_iCurrentShape = -1;
	
	if (m_nResID == IDD_PANEL_TEXT)
	{
		m_LineWidth = POINTUNITS(0);
		m_CurrentFillType = FT_Solid;
		m_CurrentFillColor = RGB(0,0,0);
	}

	COLORREF ColorText = RGB(0,0,0); //j ::GetSysColor(COLOR_WINDOWTEXT);
	COLORREF ColorBgDialog = RGB(255,255,255); //j ::GetSysColor(COLOR_BTNFACE);
	COLORREF ColorBgControl = RGB(240,240,255); //j ::GetSysColor(COLOR_WINDOW);

	m_ColorScheme.m_ColorTextDialog		= ColorText;
	m_ColorScheme.m_ColorTextStatic		= ColorText;
	m_ColorScheme.m_ColorTextMsgBox		= ColorText;
	m_ColorScheme.m_ColorTextEdit		= ColorText;
	m_ColorScheme.m_ColorTextListbox	= ColorText;
	m_ColorScheme.m_ColorTextButton		= ColorText;
	m_ColorScheme.m_ColorBgDialog		= ColorBgDialog;
	m_ColorScheme.m_ColorBgStatic		= ColorBgDialog;
	m_ColorScheme.m_ColorBgMsgBox		= ColorBgDialog;
	m_ColorScheme.m_ColorBgEdit			= ColorBgControl;
	m_ColorScheme.m_ColorBgListbox		= ColorBgControl;
	m_ColorScheme.m_ColorBgButton		= ColorBgControl;
	m_ColorScheme.m_ColorBgScrollbar	= ColorBgDialog;
}

//////////////////////////////////////////////////////////////////////
CCtlPanel::~CCtlPanel()
{
}

//////////////////////////////////////////////////////////////////////
HWND CCtlPanel::Create(HWND hWndParent)
{
	_ASSERTE(m_hWnd == NULL);
	_AtlWinModule.AddCreateWndData(&m_thunk.cd, (CDialogImplBase*)this);

	HDC hDC = ::CreateIC("DISPLAY", NULL, NULL, NULL);
	int nLogPixelsY = ::GetDeviceCaps(hDC, LOGPIXELSY);
	::DeleteDC(hDC);

	HINSTANCE hInst = _AtlBaseModule.GetResourceInstance();
	LPCSTR idDialog = MAKEINTRESOURCE(m_nResID);
	HWND hWnd = NULL;
	if (nLogPixelsY > 96)
	{
		do // A convenient block we can break out of
		{
			HRSRC hResource = ::FindResource(hInst, idDialog, RT_DIALOG);
			if (!hResource)
				break;
			HGLOBAL hTemplate = ::LoadResource(hInst, hResource);
			if (!hTemplate)
				break;
			LPVOID pTemplate = ::LockResource(hTemplate);
			if (pTemplate)
			{
				int nSize = ::SizeofResource(hInst, hResource);
				LPVOID pNewTemplate = (DLGTEMPLATE*)malloc(nSize);
				if (pNewTemplate)
				{
					memcpy(pNewTemplate, pTemplate, nSize);
					WORD* pwPointsize = (WORD*)_DialogSizeHelper::GetFontSizeField((DLGTEMPLATE*)pNewTemplate);
					if (pwPointsize)
						*pwPointsize = ((*pwPointsize * 96) / nLogPixelsY);

					hWnd = ::CreateDialogIndirectParam(hInst, (DLGTEMPLATE*)pNewTemplate, hWndParent, (DLGPROC)CCtlPanel::StartDialogProc, NULL);

					free(pNewTemplate);
				}

				UnlockResource(hTemplate);
			}

			::FreeResource(hTemplate);

		} while (0);
	}

	if (!hWnd)
		hWnd = ::CreateDialogParam(hInst, idDialog, hWndParent, (DLGPROC)CCtlPanel::StartDialogProc, NULL);

	m_ToolTip.Create(m_hWnd);
	m_ToolTip.Activate(true);
	DWORD c = m_ToolTip.GetDelayTime(TTDT_AUTOPOP); // How long if remains stationary
	DWORD a = m_ToolTip.GetDelayTime(TTDT_INITIAL); // before coming up
	DWORD b = m_ToolTip.GetDelayTime(TTDT_RESHOW); // between reappearances
//j	m_ToolTip.SetDelayTime(TTDT_INITIAL, 250);

	AddToolTip(IDC_TEXT_SIZE);
	AddToolTip(IDC_LINE_SIZE);
	AddToolTip(IDC_FILL_COLOR);
	AddToolTip(IDC_FILL_COLOR2);
	AddToolTip(IDC_FILL_COLOR3);
	AddToolTip(IDC_LINE_COLOR);
	AddToolTip(IDC_FONT);
	AddToolTip(IDC_FILL_TYPE);
	AddToolTip(IDC_BOLD);
	AddToolTip(IDC_ITALIC);
	AddToolTip(IDC_UNDERLINE);
	AddToolTip(IDC_LEFT);
	AddToolTip(IDC_CENTER);
	AddToolTip(IDC_RIGHT);
	AddToolTip(IDC_TOP);
	AddToolTip(IDC_MIDDLE);
	AddToolTip(IDC_BOTTOM);
	AddToolTip(IDC_CALENDAR_MONTH);
	AddToolTip(IDC_CALENDAR_YEAR);

	_ASSERTE(m_hWnd == hWnd);
	return hWnd;
}

void CCtlPanel::InitLabels()
{
	bool bHpUI = m_pCtp->IsHpUI();
	COLORREF rgbLabelBackground = (bHpUI ? RGB(147,165,182) : RGB(171,170,202));
	COLORREF rgbLabelText = (bHpUI ? RGB(255,255,255) : RGB(0,0,0));
	LPCSTR pFont = (bHpUI ? "Arial" : "Verdana");
	bool bBold = (bHpUI ? false : true);
	int iSize = (bHpUI ? 14 : 12);

	m_LabelSmall1.InitDialog(GetDlgItem(IDC_SMALLLABEL1));
	m_LabelSmall1.SetTextColor(RGB(0,0,0));
	m_LabelSmall1.SetBackgroundColor(RGB(255,255,255));
	m_LabelSmall1.SetFontName("Arial");

	m_LabelBig1.InitDialog(GetDlgItem(IDC_BIGLABEL1));
	m_LabelBig1.SetTextColor(rgbLabelText);
	m_LabelBig1.SetBackgroundColor(rgbLabelBackground);
	m_LabelBig1.SetFontName(pFont);
	m_LabelBig1.SetFontBold(bBold);
	m_LabelBig1.SetFontSize(iSize);

	m_LabelBig2.InitDialog(GetDlgItem(IDC_BIGLABEL2));
	m_LabelBig2.SetTextColor(rgbLabelText);
	m_LabelBig2.SetBackgroundColor(rgbLabelBackground);
	m_LabelBig2.SetFontName(pFont);
	m_LabelBig2.SetFontBold(bBold);
	m_LabelBig2.SetFontSize(iSize);

	m_LabelBig3.InitDialog(GetDlgItem(IDC_BIGLABEL3));
	m_LabelBig3.SetTextColor(rgbLabelText);
	m_LabelBig3.SetBackgroundColor(rgbLabelBackground);
	m_LabelBig3.SetFontName(pFont);
	m_LabelBig3.SetFontBold(bBold);
	m_LabelBig3.SetFontSize(iSize);

	m_LabelBig4.InitDialog(GetDlgItem(IDC_BIGLABEL4));
	m_LabelBig4.SetTextColor(rgbLabelText);
	m_LabelBig4.SetBackgroundColor(rgbLabelBackground);
	m_LabelBig4.SetFontName(pFont);
	m_LabelBig4.SetFontBold(bBold);
	m_LabelBig4.SetFontSize(iSize);

	m_LabelType.InitDialog(GetDlgItem(IDC_TYPELABEL));
	m_LabelType.SetTextColor(rgbLabelText);
	m_LabelType.SetBackgroundColor(rgbLabelBackground);
	m_LabelType.SetFontName(pFont);
	m_LabelType.SetFontBold(bBold);
	m_LabelType.SetFontSize(iSize);
}

//////////////////////////////////////////////////////////////////////
void CCtlPanel::InitLineCombos(bool bTextPanel)
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
LRESULT CCtlPanel::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bool bDefaultPanel =	(m_nResID == IDD_PANEL_DEFAULT);
	bool bImagePanel =		(m_nResID == IDD_PANEL_IMAGE);
	bool bGraphicsPanel =	(m_nResID == IDD_PANEL_GRAPHIC);
	bool bFillColor =		(m_nResID == IDD_PANEL_IMAGE)	||
							(m_nResID == IDD_PANEL_GRAPHIC);
	bool bFillColor2 =		(m_nResID == IDD_PANEL_GRAPHIC);
	bool bLineColor =		(m_nResID == IDD_PANEL_GRAPHIC);

	InitLabels();

	if (bDefaultPanel || bImagePanel)
	{
		m_LabelInstruct.InitDialog(GetDlgItem(IDC_INSTRUCTLABEL));
		m_LabelInstruct.SetTextColor(RGB(0,0,0));
		m_LabelInstruct.SetBackgroundColor(RGB(255,255,255));
		m_LabelInstruct.SetFontName("Arial");
		m_LabelInstruct.SetFontBold(false);
		m_LabelInstruct.SetFontSize(14);
	}

//j	if (bImagePanel && !m_pCtp->m_pDocWindow->SpecialsEnabled())
	if (bImagePanel)
		m_LabelBig1.ShowWindow(SW_HIDE);

	if (bFillColor)
	{
		m_ColorComboFill.InitDialog(GetDlgItem(IDC_FILL_COLOR), !bImagePanel/*bDefaultColors*/);
//j		if (bImagePanel && !m_pCtp->m_pDocWindow->SpecialsEnabled())
		if (bImagePanel)
			m_ColorComboFill.ShowWindow(SW_HIDE);
	}

	if (bFillColor2)
	{
		m_ColorComboFill2.InitDialog(GetDlgItem(IDC_FILL_COLOR2));
	}

	if (bLineColor)
	{
		InitLineCombos(false);
	}

	if (bGraphicsPanel)
	{
		m_ComboShapeName.InitDialog(GetDlgItem(IDC_SHAPE_NAME));
		int iShapeCount = CShape::GetShapeCount();
		for (int i = 0; i < iShapeCount; i++)
			m_ComboShapeName.AddString(CShape::GetShapeName(i));
		m_ComboShapeName.SetCurSel(-1);
	}

	if (bGraphicsPanel)
	{
		m_ComboFillType.InitDialog(GetDlgItem(IDC_FILL_TYPE));
		LPCSTR szFillTypes[] = {
			"None", "Solid", "Sweep Right", "Sweep Down" };
//j			"None", "Solid", "Sweep Right", "Sweep Down", "Radial Center", "Radial Corner" };
		for (int i = 0; i < sizeof(szFillTypes)/sizeof(LPCSTR); i++)
			m_ComboFillType.AddString(szFillTypes[i]);
		m_ComboFillType.SetCurSel(0);
	}

	return IDOK;
}

//////////////////////////////////////////////////////////////////////
void CCtlPanel::AddToolTip(UINT idControl)
{
	TOOLINFO ti;
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hinst = _AtlBaseModule.GetResourceInstance();
	ti.hwnd = GetDlgItem(idControl);
	ti.uId = 0;
	::GetClientRect(ti.hwnd, &ti.rect);
	ti.lpszText = MAKEINTRESOURCE(idControl);
	m_ToolTip.AddTool(&ti);
}

//////////////////////////////////////////////////////////////////////
void CCtlPanel::ActivateNewDoc()
{
}

//////////////////////////////////////////////////////////////////////
void CCtlPanel::UpdateControls()
{
	CDocWindow* pDocWindow = m_pCtp->GetDocWindow();
	CAGDoc* pAGDoc = (!pDocWindow ? NULL : pDocWindow->GetDoc());

	CAGSym* pSym = m_pCtp->m_pDocWindow->GetSelectedItem();
	if (!pSym)
		return;

	if (pSym->IsImage())
	{
		CAGSymImage* pImageSym = (CAGSymImage*)pSym;

		// Update the Color control
		SetFillColor(pImageSym->GetLastHitColor());
	}
	else
	if (pSym->IsGraphic())
	{
		CAGSymGraphic* pGraphicSym = (CAGSymGraphic*)pSym;

		if (pSym->IsRectangle())
			SetShapeName("Rectangle");
		else
		if (pSym->IsEllipse())
			SetShapeName("Circle/Ellipse");
		else
		if (pSym->IsLine())
			SetShapeName("Line");
		else
		if (pSym->IsShape())
		{
			SetShapeName(((CAGSymDrawing*)pSym)->GetShapeName());
		}

		// Update the Fill controls
		SetFillType(pGraphicSym->GetFillType());
		SetFillColor(pGraphicSym->GetFillColor());
		SetFillColor2(pGraphicSym->GetFillColor2());

		// Update the Line control
		SetLineWidth(pGraphicSym->GetLineWidth());
		SetLineColor(pGraphicSym->GetLineColor());

		ShowHideColors();
	}
}

//////////////////////////////////////////////////////////////////////
void CCtlPanel::UpdateTextControls(CAGText* pText)
{
	scTypeSpecList tsList;
	pText->GetSelTSList(tsList);
	int nNumItems = tsList.NumItems();
	if (!nNumItems)
		return;

	CAGSpec* pAGSpec = (CAGSpec*)(tsList[0].ptr());

	LOGFONT SymFont = pAGSpec->m_Font;
	int SymTextSize = abs(pAGSpec->m_Font.lfHeight);
	FillType Fill = pAGSpec->m_FillType;
	COLORREF FillColor = pAGSpec->m_FillColor;
	COLORREF FillColor2 = pAGSpec->m_FillColor2;
	int LineWidth = pAGSpec->m_LineWidth;
	COLORREF LineColor = pAGSpec->m_LineColor;
	bool bSameFont = true;
	bool bSameBold = true;
	bool bSameItalic = true;
	bool bSameUnderline = true;
	bool bSameTextSize = true;
	bool bSameFillType = true;
	bool bSameFillColor = true;
	bool bSameFillColor2 = true;
	bool bSameLineWidth = true;
	bool bSameLineColor = true;
	
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

		if (Fill != pAGSpec->m_FillType)
			bSameFillType = false;

		if (FillColor != pAGSpec->m_FillColor)
			bSameFillColor = false;

		if (FillColor2 != pAGSpec->m_FillColor2)
			bSameFillColor2 = false;

		if (LineWidth != pAGSpec->m_LineWidth)
			bSameLineWidth = false;

		if (LineColor != pAGSpec->m_LineColor)
			bSameLineColor = false;
	}

	// Update the FillType control
	if (!bSameFillType)
		Fill = (FillType)-1;
	SetFillType(Fill);

	// Update the FillColor control
	if (!bSameFillColor)
		FillColor = CLR_NONE;
	SetFillColor(FillColor);

	// Update the FillColor2 control
	if (!bSameFillColor2)
		FillColor2 = CLR_NONE;
	SetFillColor2(FillColor2);

	// Update the LineWidth control
	if (!bSameLineWidth)
		LineWidth = -999;
	SetLineWidth(LineWidth);

	// Update the LineColor control
	if (!bSameLineColor)
		LineColor = CLR_NONE;
	SetLineColor(LineColor);

	// Update the Text Control
	SetTitleText("Text");

	ShowHideColors();

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
	for (i = 1; i < nNumItems; i++)
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

	::ShowWindow(GetDlgItem(IDC_BACK), SW_HIDE);
}

//////////////////////////////////////////////////////////////////////
void CCtlPanel::SetFonts()
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
LRESULT CCtlPanel::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDC hDC = (HDC)wParam;
	if (!hDC)
	{
		bHandled = false;
		return S_OK;
	}

	CImage Image(_AtlBaseModule.GetResourceInstance(), (m_pCtp->IsHpUI() ? IDB_CTLPANELBG_HP : IDB_CTLPANELBG), "GIF");
	HBITMAP hBitmap = Image.GetBitmapHandle();
//j	HBITMAP hBitmap = ::LoadBitmap(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(IDB_CTLPANELBG));
	if (!hBitmap)
	{
		bHandled = false;
		return S_OK;
	}

	HDC hMemDC = ::CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);

	BITMAP bm;
	::GetObject(hBitmap, sizeof(bm), &bm);

	RECT rect;
	::GetClientRect(m_hWnd, &rect);

	int dx1 = bm.bmWidth / 3;
	int dx3 = dx1;
	int dx2 = bm.bmWidth - dx1 - dx3;

	for (int y=0; y < HEIGHT(rect); y += bm.bmHeight)
	{
		for (int x=0; x < WIDTH(rect); x += dx2)
			::BitBlt(hDC, x, y, dx2, bm.bmHeight, hMemDC, dx1, 0, SRCCOPY);
		break; // Only do one row of background tiles
	}
	
	// Paint the ends
	::BitBlt(hDC, 0, 0, dx1, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);
	::BitBlt(hDC, WIDTH(rect)-dx3, 0, dx3, bm.bmHeight, hMemDC, dx1+dx2, 0, SRCCOPY);

	rect.top = bm.bmHeight;
	::FillRect(hDC, &rect, CAGBrush(WHITE_BRUSH, eStock));

	::SelectObject(hMemDC, hOldBitmap);
//j	::DeleteObject(hBitmap);
	::DeleteDC(hMemDC);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CCtlPanel::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return m_ColorScheme.OnCtlColor(uMsg, wParam);
}

//////////////////////////////////////////////////////////////////////
LRESULT CCtlPanel::OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT idControl = wParam;
	LPMEASUREITEMSTRUCT pMeasureItem = (LPMEASUREITEMSTRUCT)lParam;
	if (!pMeasureItem)
		return false;

	if (pMeasureItem->CtlType != ODT_COMBOBOX)
		return true;

	// For some reason, owner draw comboboxes should be 3 pixels shorter
	HWND hCombo = GetDlgItem(IDC_LINE_SIZE);
	if (hCombo == NULL)
		hCombo = GetDlgItem(IDC_TEXT_SIZE);

	int iHeight = 22;
	if (hCombo)
	{
		RECT rect;
		::GetWindowRect(hCombo, &rect);
		iHeight = HEIGHT(rect);
	}

//j	DWORD dwMajor = 0;
//j	DWORD dwMinor = 0;
//j	AtlGetCommCtrlVersion(&dwMajor, &dwMinor);
//j	bool bUsesVisualThemes = (dwMajor >= 6);

	hCombo = GetDlgItem(idControl);
	if (pMeasureItem->itemID == -1) // the always visible part
	{
		pMeasureItem->itemHeight = iHeight - 6;
//j		if (idControl == IDC_FONT)
//j		{
//j			if (bUsesVisualThemes)
//j			{
//j			}
//j		}
	}
	else	
	{
		pMeasureItem->itemHeight = iHeight - 6;
		if (idControl == IDC_FONT)
			pMeasureItem->itemHeight *= 2;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
LRESULT CCtlPanel::OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
void CCtlPanel::SetShapeName(LPCSTR pShapeName)
{
	m_iCurrentShape = m_ComboShapeName.FindString(-1, pShapeName);
	if (m_iCurrentShape >= 0)
		m_ComboShapeName.SetCurSel(m_iCurrentShape);
	else
		m_ComboShapeName.SetWindowText(pShapeName);
}

//////////////////////////////////////////////////////////////////////
void CCtlPanel::SetFillType(FillType Fill)
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
void CCtlPanel::SetFillColor(COLORREF Color)
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
void CCtlPanel::SetFillColor2(COLORREF Color)
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
void CCtlPanel::SetFillColor3(COLORREF Color)
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
void CCtlPanel::SetLineWidth(int LineWidth)
{
	if (LineWidth == -999)
	{
		m_ComboLineWidth.SetCurSel(-1);
		return;
	}

	m_LineWidth = LineWidth;

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

void CCtlPanel::SetTitleText(LPCSTR Str)
{
	m_LabelType.SetText(Str);
}

//////////////////////////////////////////////////////////////////////
void CCtlPanel::SetLineColor(COLORREF Color)
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
void CCtlPanel::SetTextSize(int iTextSize)
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
	for (int nItem = 0; nItem < nItems; nItem++)
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
void CCtlPanel::SetFontControl(LOGFONT* pFont, bool bClearIfBad)
{
	if (pFont)
	{
		LOGFONTEXLIST& FontList = m_pCtp->GetFontList().GetInstalledFonts();
		int nFonts = FontList.size();
		for (int i = 0; i < nFonts; i++)
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
LRESULT CCtlPanel::OnBoldItalicUnderline(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
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
LRESULT CCtlPanel::OnFont(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	static wLastNotifyCode;
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
LRESULT CCtlPanel::OnFillType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
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
void CCtlPanel::ShowHideColors()
{
	if (m_nResID != IDD_PANEL_GRAPHIC && m_nResID != IDD_PANEL_TEXT && m_nResID != IDD_PANEL_CALENDAR)
		return;

	bool bCalendar = (m_nResID == IDD_PANEL_CALENDAR);

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
LRESULT CCtlPanel::OnLineWidth(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
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
LRESULT CCtlPanel::OnTextSize(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
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
LRESULT CCtlPanel::OnColorComboChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
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
LRESULT CCtlPanel::OnShapeNameComboChanged(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (!(wNotifyCode & COMBO_NOTIFCATION_FLAG))
	{ // Let the combo class take care of it
		bHandled = false;
		return S_OK;
	}

	wNotifyCode &= ~COMBO_NOTIFCATION_FLAG;

	char strShapeName[256];
	strShapeName[0] = '\0';
	m_ComboShapeName.GetWindowText(strShapeName, sizeof(strShapeName));
	m_iCurrentShape = CShape::GetShapeIndex(strShapeName);
	if (m_iCurrentShape >= 0)
		m_pCtp->m_pDocWindow->SendMessage(WM_COMMAND, IDC_DOC_SHAPENAMECHANGE);

	if (m_ComboShapeName.GetAfterCloseUp())
		m_pCtp->m_pDocWindow->SetFocus();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CCtlPanel::OnBackButton(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAGSym* pSym = m_pCtp->m_pDocWindow->GetSelectedItem();
	if (!pSym->IsCalendar())
		return S_OK;

	CAGSymCalendar* pCalendarSym = (CAGSymCalendar*)pSym;
	pCalendarSym->SetMode(-1);
	m_pCtp->SelectPanel(CALENDAR_PANEL);

	return S_OK;
}
