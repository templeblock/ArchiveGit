#include "stdafx.h"
#include "NewDocWiz.h"
#include "AGDC.h"
#include "AGSym.h"
#include "PaperTemplates.h"
#include <math.h> // for atof()

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

//////////////////////////////////////////////////////////////////////
CNewDocData::CNewDocData()
{
	Init();
}

//////////////////////////////////////////////////////////////////////
void CNewDocData::Init()
{
	m_DocType = DOC_DEFAULT;
	m_PaperType = -1;
	m_nSelectedItem = -1;
	m_bFinished = false;
	m_bIsPortrait = true;
	m_bStdSize = true;
	m_bStdNumPages = true;
	m_bStdPageNames = true;

	m_fRibbonHoleEdgeDistance = 1.25;
	m_fRibbonHoleSpacing = 1.5;

	m_fStdWidth = m_fWidth = CAGDocSheetSize::GetWidth();
	m_fStdHeight = m_fHeight = CAGDocSheetSize::GetHeight();
	m_iStdNumPages = m_iNumPages = 1;

	for (int i = 0; i < MAX_AGPAGE; i++)
	{
		m_strStdPageNames[i] = "";
		m_strPageNames[i] = "";
	}
}

//////////////////////////////////////////////////////////////////////
void CNewDocData::SetNewDocPageData()
{
	for (int i = 0; i < MAX_AGPAGE; i++)
		m_strPageNames[i] = "";

	switch (m_nSelectedItem)
	{
		case IDI_HALFCARD:
		case IDI_QUARTERCARD:
		case IDI_NOTECARD:
		case IDI_CDBOOKLET:
		{
			m_iNumPages = 4;
			m_strPageNames[0] = "Front";
			m_strPageNames[1] = "Inside ";
			m_strPageNames[1] += (m_bIsPortrait ? "Left" : "Top");
			m_strPageNames[2] = "Inside ";
			m_strPageNames[2] += (m_bIsPortrait ? "Right" : "Bottom");
			m_strPageNames[3] = "Back";
			break;
		}
		case IDI_BANNER:
		{
			m_iNumPages = 1;
			m_strPageNames[0] = "Banner 1";
			break;
		}
		case IDI_BUSINESSCARD:
		{
			m_iNumPages = 1;
			m_strPageNames[0] = "Business Card 1";
			break;
		}
		case IDI_BROCHURE:
		{
			m_iNumPages = 2;
			m_strPageNames[0] = "Brochure Front";
			m_strPageNames[1] = "Brochure Back ";
			break;
		}
		case IDI_TRIFOLD:
		{
			m_iNumPages = 6;
			m_strPageNames[0] = "Trifold Front";
			m_strPageNames[1] = "Inside ";
			m_strPageNames[1] += (m_bIsPortrait ? "Left" : "Top");
			m_strPageNames[2] = "Inside ";
			m_strPageNames[2] += (m_bIsPortrait ? "Center" : "Middle");
			m_strPageNames[3] = "Inside ";
			m_strPageNames[3] += (m_bIsPortrait ? "Right" : "Bottom");
			m_strPageNames[4] = "Back ";
			m_strPageNames[4] += (m_bIsPortrait ? "Left" : "Top");
			m_strPageNames[5] = "Back ";
			m_strPageNames[5] += (m_bIsPortrait ? "Center" : "Middle");
			break;
		}
		case IDI_HOLIDAYCARD:
		{
			m_iNumPages = 1;
			m_strPageNames[0] = "4x8 Photo Card 1";
			break;
		}
		case IDI_CDLABEL:
		{
			m_iNumPages = 1;
			m_strPageNames[0] = "CD Label 1";
			break;
		}
		case IDI_ENVELOPE:
		{
			m_iNumPages = 1;
			m_strPageNames[0] = "Envelope 1";
			break;
		}
		case IDI_FULLSHEET:
		{
			m_iNumPages = 1;
			m_strPageNames[0] = "Full Sheet 1";
			break;
		}
		case IDI_GIFTNAMECARD:
		{
			m_iNumPages = 1;
			m_strPageNames[0] = "Gift/Name Card 1";
			break;
		}
		case IDI_LABEL: 
		{
			m_iNumPages = 1;
			m_strPageNames[0] = "Label 1";
			break;
		}
		case IDI_POSTCARD:
		{
			m_iNumPages = 2;
			m_strPageNames[0] = "Postcard Front";
			m_strPageNames[1] = "Postcard Back";
			break;
		}
		case IDI_IRONON:
		{
			m_iNumPages = 1;
			m_strPageNames[0] = "Iron-on Transfer 1";
			break;
		}
		case IDI_AVERYPOSTER:
		{
			m_iNumPages = 1;
			m_strPageNames[0] = "Avery Poster 1";
		}
		default:
		{
			break;
		}
	}

	m_iStdNumPages = m_iNumPages;
	for (int i = 0; i < MAX_AGPAGE; i++)
		m_strStdPageNames[i] = m_strPageNames[i];
}

//////////////////////////////////////////////////////////////////////
void CNewDocData::SetNewDocSizeData()
{
	double fSheetWidth = CAGDocSheetSize::GetWidth();
	double fSheetHeight = CAGDocSheetSize::GetHeight();

	switch (m_nSelectedItem)
	{
		case IDI_BANNER:
		{
			m_DocType = DOC_BANNER;
			m_fWidth = 0;
			m_fHeight = 0;
			break;
		}
		case IDI_BUSINESSCARD:
		{
			m_DocType = DOC_BUSINESSCARD;
			m_fWidth = 0;
			m_fHeight = 0;
			break;
		}
		case IDI_BROCHURE:
		{
			m_DocType = DOC_BROCHURE;
			m_fWidth = fSheetWidth;
			m_fHeight = fSheetHeight;
			break;
		}
		case IDI_CDBOOKLET:
		{
			m_DocType = DOC_CDBOOKLET;
			m_fWidth = 0;
			m_fHeight = 0;
			break;
		}
		case IDI_CDLABEL:
		{
			m_DocType = DOC_CDLABEL;
			m_fWidth = 0;
			m_fHeight = 0;
			break;
		}
		case IDI_ENVELOPE:
		{
			m_DocType = DOC_ENVELOPE;
			m_fWidth = 0;
			m_fHeight = 0;
			break;
		}
		case IDI_FULLSHEET:
		{
			m_DocType = DOC_FULLSHEET;
			m_fWidth = fSheetWidth;
			m_fHeight = fSheetHeight;
			break;
		}
		case IDI_GIFTNAMECARD:
		{
			m_DocType = DOC_GIFTNAMECARD;
			m_fWidth = 0;
			m_fHeight = 0;
			break;
		}
		case IDI_HALFCARD:
		{
			m_DocType = DOC_HALFCARD;
			m_fWidth = fSheetHeight/2;
			m_fHeight = fSheetWidth;
			break;
		}
		case IDI_HOLIDAYCARD:
		{
			m_DocType = DOC_HOLIDAYCARD;
			m_fWidth = 0;
			m_fHeight = 0;
			break;
		}
		case IDI_IRONON:
		{
			m_DocType = DOC_IRONON;
			m_fWidth = 0;
			m_fHeight = 0;
			break;
		}
		case IDI_LABEL: 
		{
			m_DocType = DOC_LABEL;
			m_fWidth = 0;
			m_fHeight = 0;
			break;
		}
		case IDI_NOTECARD:
		{
			m_DocType = DOC_NOTECARD;
			m_fWidth = 0;
			m_fHeight = 0;
			break;
		}
		case IDI_PHOTOCARD:
		{
			m_DocType = DOC_PHOTOCARD;
			m_fWidth = 0;
			m_fHeight = 0;
			break;
		}
		case IDI_POSTCARD:
		{
			m_DocType = DOC_POSTCARD;
			m_fWidth = 0;
			m_fHeight = 0;
			break;
		}
		case IDI_QUARTERCARD:
		{
			m_DocType = DOC_QUARTERCARD;
			m_fWidth = fSheetWidth/2;
			m_fHeight = fSheetHeight/2;
			break;
		}
		case IDI_TRIFOLD:
		{
			m_DocType = DOC_TRIFOLD;
			m_fWidth = fSheetWidth;
			m_fHeight = fSheetHeight/3;
			break;
		}
		case IDI_AVERYPOSTER:
		{
			m_DocType = DOC_AVERYPOSTER;
			m_fWidth  = 15.25; //16.25;
			m_fHeight = 20.25; //21.25;
			break;
		}
		default:
		{
			Init();
			break;
		}
	}

	if (m_fWidth && m_fHeight)
		m_bIsPortrait = (m_fWidth <= m_fHeight);

	m_bStdSize = true;
	m_bStdNumPages = true;
	m_bStdPageNames = true;
	m_fStdWidth = m_fWidth;
	m_fStdHeight = m_fHeight;
}

//////////////////////////////////////////////////////////////////////
void CDocList::Reset()
{
	m_nItems = 0;
	SendMessage(LB_RESETCONTENT, 0, 0);
}

//////////////////////////////////////////////////////////////////////
int CDocList::GetSelectedItem()
{
	int i = SendMessage(LB_GETCURSEL, 0, 0);
	if (i < 0 || i >= MAX_ITEMS)
		return -1;

	if (!m_bIconsEnabled[i])
		return -1;

	return m_idIcons[i];
}

//////////////////////////////////////////////////////////////////////
void CDocList::SelectItem(int idIcon)
{
	for (int i=0; i<m_nItems; i++)
	{
		if (m_idIcons[i] == idIcon)
		{
			if (!m_bIconsEnabled[i])
				i = -1;
			SendMessage(LB_SETCURSEL, i, 0);
			return;
		}
	}

	SendMessage(LB_SETCURSEL, -1, 0);
}

//////////////////////////////////////////////////////////////////////
void CDocList::AddItem(AGDOCTYPE idDocType, int idIcon, bool bEnable)
{
	if (m_nItems >= MAX_ITEMS)
		return;

	m_idIcons[m_nItems] = idIcon;
	m_bIconsEnabled[m_nItems] = bEnable;
	CString strDocTypeName = CAGDocTypes::Name(idDocType);
	SendMessage(LB_ADDSTRING, m_nItems, (LPARAM)(LPCSTR)strDocTypeName);
	m_nItems++;
}

//////////////////////////////////////////////////////////////////////
LRESULT CDocList::OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT idCtl = wParam;
	LPDRAWITEMSTRUCT lpDraw = (LPDRAWITEMSTRUCT)lParam;
	if (!lpDraw || idCtl != IDC_NEWDOC_LIST)
		return S_OK;

	if (lpDraw->itemID < 0 || lpDraw->itemID >= MAX_ITEMS)
		return S_OK;

	bool bGrayed    = !!((lpDraw->itemState & (ODS_GRAYED | ODS_DISABLED)));
	bool bHilighted = !!((lpDraw->itemState & (ODS_SELECTED | ODS_CHECKED)) && !bGrayed);
	bool bFocus     = !!((lpDraw->itemState & (ODS_FOCUS)) && !bGrayed);

	int idIcon = m_idIcons[lpDraw->itemID];
	bool bEnabled = m_bIconsEnabled[lpDraw->itemID];
	char szString[MAX_PATH];
	SendMessage(LB_GETTEXT, lpDraw->itemID, (LPARAM)szString);

	// Draw the background
	FillRect(lpDraw->hDC, &lpDraw->rcItem, CAGBrush(COLOR_WINDOW, eSystem));

	// Draw the icon
	RECT rect = lpDraw->rcItem;
	InflateRect(&rect, -4, -2);
	HICON hIcon = ::LoadIcon(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(idIcon));
	::DrawIconEx(lpDraw->hDC, rect.left, rect.top, hIcon, 0/*cxWidth*/, 0/*cyWidth*/, 0/*istepIfAniCur*/, NULL/*hbrFlickerFreeDraw*/, DI_NORMAL);
	::DeleteObject(hIcon);

	// Draw the text
	int iOldMode = ::SetBkMode(lpDraw->hDC, TRANSPARENT );
	COLORREF lOldColor = ::SetTextColor(lpDraw->hDC, (bEnabled ? ::GetSysColor(COLOR_WINDOWTEXT) : GetSysColor(COLOR_BTNSHADOW)));
	rect.left += (ITEM_HEIGHT+2);
	::DrawText(lpDraw->hDC, szString, lstrlen(szString), &rect, DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX);
	::SetBkMode(lpDraw->hDC, iOldMode);
	::SetTextColor(lpDraw->hDC, lOldColor);

	// Draw the selection rectangle
	if (bHilighted)
	{
		if (bEnabled)
			::FrameRect(lpDraw->hDC, &lpDraw->rcItem, CAGBrush(BLACK_BRUSH, eStock));
		else
			int a = 0;
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CDocList::OnMeasureItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT idCtl = wParam;
	LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT)lParam;

	if (!lpmis || idCtl != IDC_NEWDOC_LIST)
		return S_OK;

	int iHeight = lpmis->itemHeight;
	lpmis->itemWidth = ITEM_WIDTH;
	lpmis->itemHeight = ITEM_HEIGHT;

	m_nItems = 0;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CDocList::OnKeyUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = false; // Allow the default behavior to be executed
	::PostMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CDocList::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = false; // Allow the default behavior to be executed
	::PostMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CDocList::OnLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::PostMessage(GetParent(), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), 999/*BN_DBLCLK*/), (LPARAM)m_hWnd);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
CNewDocWizIntro::CNewDocWizIntro()
{
	m_Data = &m_NewDocData;
}

//////////////////////////////////////////////////////////////////////
BOOL CNewDocWizIntro::OnSetActive()
{
	::SetWindowLong(GetParent(), GWL_USERDATA, (long)m_Data);

	CWindow Parent(GetParent());
	Parent.CenterWindow();

	m_DocList.SubclassWindow(GetDlgItem(IDC_NEWDOC_LIST));

	m_DocList.Reset();
	m_DocList.AddItem(DOC_BANNER, IDI_BANNER, false);
	m_DocList.AddItem(DOC_BUSINESSCARD, IDI_BUSINESSCARD, true);
	m_DocList.AddItem(DOC_BROCHURE, IDI_BROCHURE, true);
	m_DocList.AddItem(DOC_HALFCARD, IDI_HALFCARD, true);
	m_DocList.AddItem(DOC_QUARTERCARD, IDI_QUARTERCARD, true);
	m_DocList.AddItem(DOC_CDBOOKLET, IDI_CDBOOKLET, true);
	m_DocList.AddItem(DOC_CDLABEL, IDI_CDLABEL, true);
	m_DocList.AddItem(DOC_ENVELOPE, IDI_ENVELOPE, true); 
	m_DocList.AddItem(DOC_FULLSHEET, IDI_FULLSHEET, true);
	m_DocList.AddItem(DOC_GIFTNAMECARD, IDI_GIFTNAMECARD, true);
	m_DocList.AddItem(DOC_HOLIDAYCARD, IDI_HOLIDAYCARD, true);
	m_DocList.AddItem(DOC_IRONON, IDI_IRONON, true);
	m_DocList.AddItem(DOC_LABEL, IDI_LABEL, true);
	m_DocList.AddItem(DOC_NOTECARD, IDI_NOTECARD, true);
	m_DocList.AddItem(DOC_PHOTOCARD, IDI_PHOTOCARD, true);
	m_DocList.AddItem(DOC_POSTCARD, IDI_POSTCARD, true);
	m_DocList.AddItem(DOC_TRIFOLD, IDI_TRIFOLD, true);
	m_DocList.AddItem(DOC_AVERYPOSTER, IDI_AVERYPOSTER, true);
	m_DocList.SelectItem(m_Data->m_nSelectedItem);

	SetWizardButtons(m_Data->m_nSelectedItem >= 0 ? PSWIZB_NEXT : 0);
	return true;
}

//////////////////////////////////////////////////////////////////////
BOOL CNewDocWizIntro::OnKillActive()
{
	m_Data->m_nSelectedItem = m_DocList.GetSelectedItem();
	m_Data->SetNewDocSizeData();

	m_DocList.UnsubclassWindow();

	return true;
}

//////////////////////////////////////////////////////////////////////
BOOL CNewDocWizIntro::OnWizardNext()
{
	return true;
}

//////////////////////////////////////////////////////////////////////
LRESULT CNewDocWizIntro::OnClickList(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_Data->m_nSelectedItem = m_DocList.GetSelectedItem();
	m_Data->SetNewDocSizeData();
	m_Data->m_PaperType = -1;

	SetWizardButtons(m_Data->m_nSelectedItem >= 0 ? PSWIZB_NEXT : 0);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CNewDocWizIntro::OnDoubleClickList(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (m_Data->m_nSelectedItem >= 0)
		PressButton(PSBTN_NEXT);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
CNewDocWizStep1::CNewDocWizStep1()
{
	::SetRectEmpty(&m_SampleRect);
}

//////////////////////////////////////////////////////////////////////
BOOL CNewDocWizStep1::OnSetActive()
{
	m_Data = (CNewDocData*)::GetWindowLong(GetParent(), GWL_USERDATA);

	// Some templates require the paper type combo, others don't
	if (m_Data->m_DocType == DOC_BUSINESSCARD	||
		m_Data->m_DocType == DOC_CDBOOKLET		||
		m_Data->m_DocType == DOC_CDLABEL		||
		m_Data->m_DocType == DOC_ENVELOPE		||
		m_Data->m_DocType == DOC_GIFTNAMECARD	||
		m_Data->m_DocType == DOC_HOLIDAYCARD	||
		m_Data->m_DocType == DOC_IRONON			||
		m_Data->m_DocType == DOC_LABEL			||
		m_Data->m_DocType == DOC_NOTECARD		||
		m_Data->m_DocType == DOC_PHOTOCARD		||
		m_Data->m_DocType == DOC_POSTCARD)
	{
		::ShowWindow(GetDlgItem(IDC_NEWDOC_STDSIZE), SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_NEWDOC_PAPERTYPE), SW_SHOW);

		bool bFirstTime = (m_Data->m_PaperType < 0);
		if (bFirstTime)
			m_Data->m_PaperType = 0;

		const PaperTemplate* pPaper = NULL;
		CPaperTemplates::StuffCombo(m_Data->m_DocType, m_Data->m_PaperType, &pPaper, GetDlgItem(IDC_NEWDOC_PAPERTYPE), 0, 0);
		if (pPaper)
		{
			m_Data->m_fWidth = pPaper->fWidth;
			m_Data->m_fHeight = pPaper->fHeight;
			// Once we set the paper type the first time, the user's desired orientation is maintained
			if (!bFirstTime)
				SwapSizeCheck(false/*bSetControls*/);
			else
				SwapOrientationCheck(false/*bSetControls*/);
		}

		m_Data->m_bStdSize = true;
	}
	else
	{
		::ShowWindow(GetDlgItem(IDC_NEWDOC_STDSIZE), SW_SHOW);
		::ShowWindow(GetDlgItem(IDC_NEWDOC_PAPERTYPE), SW_HIDE);
		CheckDlgButton(IDC_NEWDOC_STDSIZE, m_Data->m_bStdSize);
	}

	CheckRadioButton(IDC_NEWDOC_LANDSCAPE, IDC_NEWDOC_PORTRAIT, IDC_NEWDOC_LANDSCAPE + m_Data->m_bIsPortrait);

	SendDlgItemMessage(IDC_NEWDOC_WIDTH,  EM_SETREADONLY, m_Data->m_bStdSize, 0);
	SendDlgItemMessage(IDC_NEWDOC_HEIGHT, EM_SETREADONLY, m_Data->m_bStdSize, 0);

	CString strBuffer;
	strBuffer.Format("%0.5G", m_Data->m_fWidth);
	SetDlgItemText(IDC_NEWDOC_WIDTH, strBuffer);
	strBuffer.Format("%0.5G", m_Data->m_fHeight);
	SetDlgItemText(IDC_NEWDOC_HEIGHT, strBuffer);

	SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	return true;
}

//////////////////////////////////////////////////////////////////////
BOOL CNewDocWizStep1::OnKillActive()
{
	m_Data->SetNewDocPageData();
	return true;
}

//////////////////////////////////////////////////////////////////////
BOOL CNewDocWizStep1::OnWizardNext()
{
	return true;
}

//////////////////////////////////////////////////////////////////////
void CNewDocWizStep1::SwapSizeCheck(bool bSetControls)
{
	bool bSwap = false;
	if (m_Data->m_bIsPortrait && (m_Data->m_fWidth > m_Data->m_fHeight))
		bSwap = true;
	else
	if (!m_Data->m_bIsPortrait && (m_Data->m_fWidth < m_Data->m_fHeight))
		bSwap = true;
		
	if (bSwap)
	{
		double d = m_Data->m_fHeight;
		m_Data->m_fHeight = m_Data->m_fWidth;
		m_Data->m_fWidth = d;

		if (bSetControls)
		{
			CString strBuffer;
			strBuffer.Format("%0.5G", m_Data->m_fWidth);
			SetDlgItemText(IDC_NEWDOC_WIDTH, strBuffer);
			strBuffer.Format("%0.5G", m_Data->m_fHeight);
			SetDlgItemText(IDC_NEWDOC_HEIGHT, strBuffer);
		}
	}
}

//////////////////////////////////////////////////////////////////////
LRESULT CNewDocWizStep1::OnLandscape(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_Data->m_bIsPortrait = !IsDlgButtonChecked(wID);
	SwapSizeCheck(true/*bSetControls*/);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CNewDocWizStep1::OnPortrait(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_Data->m_bIsPortrait = !!IsDlgButtonChecked(wID);
	SwapSizeCheck(true/*bSetControls*/);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CNewDocWizStep1::OnPaperType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int iIndex = ::SendMessage(hWndCtl, CB_GETCURSEL, 0, 0);
	m_Data->m_PaperType = ::SendMessage(hWndCtl, CB_GETITEMDATA, iIndex, 0);
	const PaperTemplate* pPaper = NULL;
	CPaperTemplates::GetTemplate(m_Data->m_DocType, m_Data->m_PaperType, &pPaper);
	if (pPaper)
	{
		m_Data->m_fWidth = pPaper->fWidth;
		m_Data->m_fHeight = pPaper->fHeight;

		CString strBuffer;
		strBuffer.Format("%0.5G", m_Data->m_fWidth);
		SetDlgItemText(IDC_NEWDOC_WIDTH, strBuffer);
		strBuffer.Format("%0.5G", m_Data->m_fHeight);
		SetDlgItemText(IDC_NEWDOC_HEIGHT, strBuffer);
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CNewDocWizStep1::OnStdSize(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_Data->m_bStdSize = !!IsDlgButtonChecked(wID);
	SendDlgItemMessage(IDC_NEWDOC_WIDTH,  EM_SETREADONLY, m_Data->m_bStdSize, 0);
	SendDlgItemMessage(IDC_NEWDOC_HEIGHT, EM_SETREADONLY, m_Data->m_bStdSize, 0);

	if (m_Data->m_bStdSize)
	{
		m_Data->m_fWidth = m_Data->m_fStdWidth;
		m_Data->m_fHeight = m_Data->m_fStdHeight;

		CString strBuffer;
		strBuffer.Format("%0.5G", m_Data->m_fWidth);
		SetDlgItemText(IDC_NEWDOC_WIDTH, strBuffer);
		strBuffer.Format("%0.5G", m_Data->m_fHeight);
		SetDlgItemText(IDC_NEWDOC_HEIGHT, strBuffer);
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
void CNewDocWizStep1::SwapOrientationCheck(bool bSetControls)
{
	bool bSwap = false;
	if (m_Data->m_bIsPortrait && (m_Data->m_fWidth > m_Data->m_fHeight))
		bSwap = true;
	else
	if (!m_Data->m_bIsPortrait && (m_Data->m_fWidth < m_Data->m_fHeight))
		bSwap = true;
		
	if (bSwap)
	{
		m_Data->m_bIsPortrait = !m_Data->m_bIsPortrait;
		if (bSetControls)
			CheckRadioButton(IDC_NEWDOC_LANDSCAPE, IDC_NEWDOC_PORTRAIT, IDC_NEWDOC_LANDSCAPE + m_Data->m_bIsPortrait);
	}
}

//////////////////////////////////////////////////////////////////////
void CNewDocWizStep1::UpdateSample(int idSample)
{
	HWND hWnd = GetDlgItem(idSample);
	if (!hWnd)
		return;

	// If it is our first time in, initialize the sample rectangle
	if (::IsRectEmpty(&m_SampleRect))
	{
		::GetClientRect(hWnd, &m_SampleRect);
		if (::GetParent(hWnd))
			::MapWindowPoints(hWnd, ::GetParent(hWnd), (LPPOINT)&m_SampleRect.left, 2);
	}

	SIZE DocSize = {(int)(m_Data->m_fWidth * 1000), (int)(m_Data->m_fHeight * 1000)};
	RECT NewRect;
	::SetRect(&NewRect, 0, 0, DocSize.cx, DocSize.cy);
	CAGMatrix Matrix;
	Matrix.ScaleToFit(m_SampleRect, NewRect);
	Matrix.Transform(NewRect);
	::MoveWindow(hWnd, NewRect.left, NewRect.top, WIDTH(NewRect), HEIGHT(NewRect), true/*bRepaint*/);
}

//////////////////////////////////////////////////////////////////////
LRESULT CNewDocWizStep1::OnWidth(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (wNotifyCode != EN_CHANGE)
		return S_OK;
		
	CComBSTR strValue;
	DWORD dwResult = GetDlgItemText(wID, strValue.m_str);
	if (NULL == strValue.m_str)
		return S_OK;

	USES_CONVERSION;
	m_Data->m_fWidth = atof(OLE2T(strValue));

	SwapOrientationCheck(true/*bSetControls*/);
	UpdateSample(IDC_NEWDOC_SAMPLE);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CNewDocWizStep1::OnHeight(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (wNotifyCode != EN_CHANGE)
		return S_OK;
		
	CComBSTR strValue;
	DWORD dwResult = GetDlgItemText(wID, strValue.m_str);
	if (NULL == strValue.m_str)
		return S_OK;

	USES_CONVERSION;
	m_Data->m_fHeight = atof(OLE2T(strValue));

	SwapOrientationCheck(true/*bSetControls*/);
	UpdateSample(IDC_NEWDOC_SAMPLE);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
CNewDocWizStep2::CNewDocWizStep2()
{
}

//////////////////////////////////////////////////////////////////////
BOOL CNewDocWizStep2::OnSetActive()
{
	m_Data = (CNewDocData*)::GetWindowLong(GetParent(), GWL_USERDATA);

	CheckDlgButton(IDC_NEWDOC_STDNUMPAGES, m_Data->m_bStdNumPages);
	SendDlgItemMessage(IDC_NEWDOC_NUMPAGES, EM_SETREADONLY, m_Data->m_bStdNumPages, 0);
	SetDlgItemInt(IDC_NEWDOC_NUMPAGES, m_Data->m_iNumPages, false/*bSigned*/);

	CheckDlgButton(IDC_NEWDOC_STDPAGENAMES, m_Data->m_bStdPageNames);
	int i=0;
	for (i = 0; i < MAX_AGPAGE; i++)
		SendDlgItemMessage(IDC_NEWDOC_PAGENAME1 + i, EM_SETREADONLY, m_Data->m_bStdPageNames, 0);

	for (i = 0; i < MAX_AGPAGE; i++)
	{
		int iShowFlag = (i < m_Data->m_iNumPages ? SW_SHOW : SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_NEWDOC_PAGENAME1 + i), iShowFlag);
		::ShowWindow(GetDlgItem(IDC_NEWDOC_PAGELABEL1 + i), iShowFlag);
	}

	for (i = 0; i < MAX_AGPAGE; i++)
		SetDlgItemText(IDC_NEWDOC_PAGENAME1 + i, m_Data->m_strPageNames[i]);

	bool bOKtoGoOn = true;
	if (bOKtoGoOn)
		SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);
	else
		SetWizardButtons(PSWIZB_BACK);

	return true;
}

//////////////////////////////////////////////////////////////////////
BOOL CNewDocWizStep2::OnKillActive()
{
	return true;
}

//////////////////////////////////////////////////////////////////////
BOOL CNewDocWizStep2::OnWizardNext()
{
	return true;
}

//////////////////////////////////////////////////////////////////////
LRESULT CNewDocWizStep2::OnStdNumPages(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_Data->m_bStdNumPages = !!IsDlgButtonChecked(wID);
	SendDlgItemMessage(IDC_NEWDOC_NUMPAGES, EM_SETREADONLY, m_Data->m_bStdNumPages, 0);

	if (m_Data->m_bStdNumPages)
	{
		m_Data->m_iNumPages = m_Data->m_iStdNumPages;
		SetDlgItemInt(IDC_NEWDOC_NUMPAGES, m_Data->m_iNumPages, false/*bSigned*/);
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CNewDocWizStep2::OnNumPages(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (wNotifyCode != EN_CHANGE)
		return S_OK;
		
	m_Data->m_iNumPages = GetDlgItemInt(wID, false/*bSigned*/);
	if (m_Data->m_iNumPages > MAX_AGPAGE)
	{
		m_Data->m_iNumPages = MAX_AGPAGE;
		SetDlgItemInt(wID, m_Data->m_iNumPages, false/*bSigned*/);
	}
	if (!m_Data->m_iNumPages)
	{
		m_Data->m_iNumPages = 1;
		SetDlgItemInt(wID, m_Data->m_iNumPages, false/*bSigned*/);
	}

	for (int i = 0; i < MAX_AGPAGE; i++)
	{
		int iShowFlag = (i < m_Data->m_iNumPages ? SW_SHOW : SW_HIDE);
		::ShowWindow(GetDlgItem(IDC_NEWDOC_PAGENAME1 + i), iShowFlag);
		::ShowWindow(GetDlgItem(IDC_NEWDOC_PAGELABEL1 + i), iShowFlag);
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CNewDocWizStep2::OnStdPageNames(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_Data->m_bStdPageNames = !!IsDlgButtonChecked(wID);
	for (int i = 0; i < MAX_AGPAGE; i++)
		SendDlgItemMessage(IDC_NEWDOC_PAGENAME1 + i, EM_SETREADONLY, m_Data->m_bStdPageNames, 0);

	if (m_Data->m_bStdPageNames)
	{
		for (int i = 0; i < MAX_AGPAGE; i++)
		{
			m_Data->m_strPageNames[i] = m_Data->m_strStdPageNames[i];
			SetDlgItemText(IDC_NEWDOC_PAGENAME1 + i, m_Data->m_strPageNames[i]);
		}
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CNewDocWizStep2::OnPageName1(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (wNotifyCode != EN_CHANGE)
		return S_OK;
		
    char str[MAX_PATH];
	GetDlgItemText(wID, str, MAX_PATH);
	m_Data->m_strPageNames[0] = str;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CNewDocWizStep2::OnPageName2(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (wNotifyCode != EN_CHANGE)
		return S_OK;
		
    char str[MAX_PATH];
	GetDlgItemText(wID, str, MAX_PATH);
	m_Data->m_strPageNames[1] = str;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CNewDocWizStep2::OnPageName3(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (wNotifyCode != EN_CHANGE)
		return S_OK;
		
    char str[MAX_PATH];
	GetDlgItemText(wID, str, MAX_PATH);
	m_Data->m_strPageNames[2] = str;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CNewDocWizStep2::OnPageName4(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (wNotifyCode != EN_CHANGE)
		return S_OK;
		
    char str[MAX_PATH];
	GetDlgItemText(wID, str, MAX_PATH);
	m_Data->m_strPageNames[3] = str;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CNewDocWizStep2::OnPageName5(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (wNotifyCode != EN_CHANGE)
		return S_OK;
		
    char str[MAX_PATH];
	GetDlgItemText(wID, str, MAX_PATH);
	m_Data->m_strPageNames[4] = str;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CNewDocWizStep2::OnPageName6(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (wNotifyCode != EN_CHANGE)
		return S_OK;
		
    char str[MAX_PATH];
	GetDlgItemText(wID, str, MAX_PATH);
	m_Data->m_strPageNames[5] = str;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CNewDocWizStep2::OnPageName7(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (wNotifyCode != EN_CHANGE)
		return S_OK;
		
    char str[MAX_PATH];
	GetDlgItemText(wID, str, MAX_PATH);
	m_Data->m_strPageNames[6] = str;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CNewDocWizStep2::OnPageName8(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (wNotifyCode != EN_CHANGE)
		return S_OK;
		
    char str[MAX_PATH];
	GetDlgItemText(wID, str, MAX_PATH);
	m_Data->m_strPageNames[7] = str;
	return S_OK;
}


//////////////////////////////////////////////////////////////////////
CNewDocWizEnd::CNewDocWizEnd()
{
}

//////////////////////////////////////////////////////////////////////
BOOL CNewDocWizEnd::OnSetActive()
{
	m_Data = (CNewDocData*)::GetWindowLong(GetParent(), GWL_USERDATA);

	SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
	return true;
}

//////////////////////////////////////////////////////////////////////
BOOL CNewDocWizEnd::OnKillActive()
{
	return true;
}

//////////////////////////////////////////////////////////////////////
BOOL CNewDocWizEnd::OnWizardFinish()
{
	m_Data->m_bFinished = true;
	return true;
}
//////////////////////////////////////////////////////////////////////
CDocWizRibbonHolesPage::CDocWizRibbonHolesPage()
{

}
//////////////////////////////////////////////////////////////////////
BOOL CDocWizRibbonHolesPage::OnSetActive()
{
	m_Data = (CNewDocData*)::GetWindowLong(GetParent(), GWL_USERDATA);

	if (m_Data->m_DocType == DOC_HALFCARD		||
		m_Data->m_DocType == DOC_FULLSHEET		||
		m_Data->m_DocType == DOC_NOTECARD		||
		m_Data->m_DocType == DOC_PHOTOCARD		||
		m_Data->m_DocType == DOC_POSTCARD)
	{
		GetDlgItem(IDC_CHECK_RH_PRINT).EnableWindow(true);
		GetDlgItem(IDC_COMBO_RH_SHAPES).EnableWindow(true);
		GetDlgItem(IDC_EDIT_RH_EDGEDISTANCE).EnableWindow(true);
		GetDlgItem(IDC_EDIT_RH_HOLESPACING).EnableWindow(true);
		GetDlgItem(IDC_RADIO_RH_VERT).EnableWindow(true);
		GetDlgItem(IDC_RADIO_RH_HORZ).EnableWindow(true);
	}
	else
	{
		GetDlgItem(IDC_CHECK_RH_PRINT).EnableWindow(false);
		GetDlgItem(IDC_COMBO_RH_SHAPES).EnableWindow(false);
		GetDlgItem(IDC_EDIT_RH_EDGEDISTANCE).EnableWindow(false);
		GetDlgItem(IDC_EDIT_RH_HOLESPACING).EnableWindow(false);
		GetDlgItem(IDC_RADIO_RH_VERT).EnableWindow(false);
		GetDlgItem(IDC_RADIO_RH_HORZ).EnableWindow(false);
	}

	CheckRadioButton(IDC_RADIO_RH_VERT, IDC_RADIO_RH_HORZ, IDC_RADIO_RH_VERT);

	SendDlgItemMessage(IDC_EDIT_RH_EDGEDISTANCE, EM_SETREADONLY, m_Data->m_fRibbonHoleEdgeDistance, 0);
	SendDlgItemMessage(IDC_EDIT_RH_HOLESPACING, EM_SETREADONLY, m_Data->m_fRibbonHoleSpacing, 0);

	CString szBuffer;
	szBuffer.LoadString(IDS_DOCWIZ_HOLES);
	SetDlgItemText(IDC_TEXT_RH_INSTRUCT, szBuffer);
	SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

	return true;
}
//////////////////////////////////////////////////////////////////////
LRESULT CDocWizRibbonHolesPage::OnHolesVertical(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_Data->m_bRibbonHolesVertical = !!IsDlgButtonChecked(wID);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CDocWizRibbonHolesPage::OnHolesHorizontal(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	m_Data->m_bRibbonHolesVertical = !IsDlgButtonChecked(wID);
	return S_OK;
}
//////////////////////////////////////////////////////////////////////
LRESULT CDocWizRibbonHolesPage::OnEdgeDistance(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (wNotifyCode != EN_CHANGE)
		return S_OK;
		
	CComBSTR strValue;
	DWORD dwResult = GetDlgItemText(wID, strValue.m_str);
	if (NULL == strValue.m_str)
		return S_OK;

	USES_CONVERSION;
	m_Data->m_fRibbonHoleEdgeDistance = atof(OLE2T(strValue));

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
LRESULT CDocWizRibbonHolesPage::OnHoleSpacing(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (wNotifyCode != EN_CHANGE)
		return S_OK;
		
	CComBSTR strValue;
	DWORD dwResult = GetDlgItemText(wID, strValue.m_str);
	if (NULL == strValue.m_str)
		return S_OK;

	USES_CONVERSION;
	m_Data->m_fRibbonHoleSpacing = atof(OLE2T(strValue));

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
LRESULT CDocWizRibbonHolesPage::OnHoleShape(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
LRESULT CDocWizRibbonHolesPage::OnPrintRibbonHoles(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
BOOL CDocWizRibbonHolesPage::OnWizardNext()
{

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
BOOL CDocWizRibbonHolesPage::OnKillActive()
{

	return S_OK;
}