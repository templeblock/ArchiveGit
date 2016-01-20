#include "stdafx.h"
#include "Ctp.h"
#include "AGDoc.h"
#include "AGPage.h"
#include "PropertiesDlg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif _DEBUG

//////////////////////////////////////////////////////////////////////
CPropertiesDlg::CPropertiesDlg(CCtp* pCtp, CAGDoc* pAGDoc)
{
	m_pCtp = pCtp;
	m_pAGDoc = pAGDoc;
	m_nDocTypes = 0;
	m_nSelectedPage = 0;
	m_idDocType = DOC_DEFAULT;

	for (int i = 0; i < MAX_AGPAGE; i++)
	{
		m_strPageNamesOrig[i].Empty();
		m_fPageWidthsOrig[i] = 0.0;
		m_fPageHeightsOrig[i] = 0.0;

		m_strPageNames[i].Empty();
		m_fPageWidths[i] = 0.0;
		m_fPageHeights[i] = 0.0;
	}
}

//////////////////////////////////////////////////////////////////////
CPropertiesDlg::~CPropertiesDlg()
{
}

//////////////////////////////////////////////////////////////////////
LRESULT CPropertiesDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CenterWindow();

	SendDlgItemMessage(IDC_DOCTYPE, CB_RESETCONTENT, 0, 0);

	AddDocType(DOC_BANNER);
	AddDocType(DOC_BUSINESSCARD);
	AddDocType(DOC_BROCHURE);
	AddDocType(DOC_CDLABEL);
	AddDocType(DOC_CDBOOKLET);
	AddDocType(DOC_ENVELOPE);
	AddDocType(DOC_FULLSHEET);
	AddDocType(DOC_GIFTNAMECARD);
	AddDocType(DOC_HALFCARD);
	AddDocType(DOC_HOLIDAYCARD);
	AddDocType(DOC_IRONON);
	AddDocType(DOC_LABEL);
	AddDocType(DOC_NOTECARD);
	AddDocType(DOC_PHOTOCARD);
	AddDocType(DOC_POSTCARD);
	AddDocType(DOC_QUARTERCARD);
	AddDocType(DOC_TRIFOLD);

	m_idDocType = m_pAGDoc->GetDocType();
	int iIndex = FindDocType(m_idDocType);
	if (iIndex >= 0)
		SendDlgItemMessage(IDC_DOCTYPE, CB_SETCURSEL, iIndex, 0);

	SendDlgItemMessage(IDC_PAGENAMES, CB_RESETCONTENT, 0, 0);

	for (int i = 0; i < m_pAGDoc->GetNumPages(); i++)
	{
		CAGPage* pPage = m_pAGDoc->GetPage(i);
		if (!pPage)
			continue;
		SIZE PageSize = {0,0};
		pPage->GetPageSize(PageSize);
		m_fPageWidths[i] = DINCHES(PageSize.cx);
		m_fPageHeights[i] = DINCHES(PageSize.cy);
		pPage->GetPageName(m_strPageNames[i]);
		SendDlgItemMessage(IDC_PAGENAMES, CB_ADDSTRING, i, (LPARAM)(LPCSTR)m_strPageNames[i]);
	}

	m_nSelectedPage = 0;
	SendDlgItemMessage(IDC_PAGENAMES, CB_SETCURSEL, m_nSelectedPage, 0);

	CString strValue;
	strValue.Format("%0.5G", m_fPageWidths[m_nSelectedPage]);
	SetDlgItemText(IDC_WIDTH, strValue);

	strValue.Format("%0.5G", m_fPageHeights[m_nSelectedPage]);
	SetDlgItemText(IDC_HEIGHT, strValue);

	for (int i = 0; i < m_pAGDoc->GetNumPages(); i++)
	{
		m_strPageNamesOrig[i] = m_strPageNames[i];
		m_fPageWidthsOrig[i] = m_fPageWidths[i];
		m_fPageHeightsOrig[i] = m_fPageHeights[i];
	}

	return IDOK;
}

//////////////////////////////////////////////////////////////////////
void CPropertiesDlg::AddDocType(AGDOCTYPE idDocType)
{
	if (m_nDocTypes >= MAX_ITEMS)
		return;

	m_idDocTypes[m_nDocTypes] = idDocType;
	CString strDocTypeName = CAGDocTypes::Name(idDocType);
	SendDlgItemMessage(IDC_DOCTYPE, CB_ADDSTRING, m_nDocTypes, (LPARAM)(LPCSTR)strDocTypeName);
	m_nDocTypes++;
}

//////////////////////////////////////////////////////////////////////
int CPropertiesDlg::FindDocType(AGDOCTYPE idDocType)
{
	for (int iIndex = 0; iIndex < m_nDocTypes; iIndex++)
	{
		if (m_idDocTypes[iIndex] == idDocType)
			return iIndex;
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPropertiesDlg::OnDocType(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int iIndex = SendDlgItemMessage(IDC_DOCTYPE, CB_GETCURSEL, 0, 0);
	if (iIndex < 0)
		return E_FAIL;

	m_idDocType = m_idDocTypes[iIndex];
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPropertiesDlg::OnPageNames(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (wNotifyCode == CBN_SELCHANGE)
	{
		m_nSelectedPage = SendDlgItemMessage(IDC_PAGENAMES, CB_GETCURSEL, 0, 0);

		CString strValue;
		strValue.Format("%0.5G", m_fPageWidths[m_nSelectedPage]);
		SetDlgItemText(IDC_WIDTH, strValue);

		strValue.Format("%0.5G", m_fPageHeights[m_nSelectedPage]);
		SetDlgItemText(IDC_HEIGHT, strValue);
	}
	else
	if (wNotifyCode == CBN_EDITCHANGE)
	{
		char strValue[MAX_PATH];
		GetDlgItemText(IDC_PAGENAMES, strValue, MAX_PATH);
		m_strPageNames[m_nSelectedPage] = strValue;

		DWORD dwPosition = SendDlgItemMessage(IDC_PAGENAMES, CB_GETEDITSEL, 0, 0);
		SendDlgItemMessage(IDC_PAGENAMES, CB_DELETESTRING, m_nSelectedPage, 0);
		SendDlgItemMessage(IDC_PAGENAMES, CB_INSERTSTRING, m_nSelectedPage, (LPARAM)(LPCSTR)m_strPageNames[m_nSelectedPage]);
		SendDlgItemMessage(IDC_PAGENAMES, CB_SETCURSEL, m_nSelectedPage, 0);
		SendDlgItemMessage(IDC_PAGENAMES, CB_SETEDITSEL, 0, dwPosition);
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPropertiesDlg::OnWidth(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (wNotifyCode != EN_CHANGE)
		return S_OK;
		
	char strValue[MAX_PATH];
	GetDlgItemText(IDC_WIDTH, strValue, MAX_PATH);
	double fSize = atof(strValue);
	m_fPageWidths[m_nSelectedPage] = fSize;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPropertiesDlg::OnHeight(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if (wNotifyCode != EN_CHANGE)
		return S_OK;
		
	char strValue[MAX_PATH];
	GetDlgItemText(IDC_HEIGHT, strValue, MAX_PATH);
	double fSize = atof(strValue);
	m_fPageHeights[m_nSelectedPage] = fSize;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPropertiesDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	bool bModified = (m_idDocType != m_pAGDoc->GetDocType());
	for (int i = 0; i < m_pAGDoc->GetNumPages(); i++)
	{
		if (m_strPageNamesOrig[i] != m_strPageNames[i] ||
			m_fPageHeightsOrig[i] != m_fPageHeights[i] ||
			m_fPageWidthsOrig[i]  != m_fPageWidths[i])
		{
			bModified = true;
			break;
		}
	}

	if (bModified)
	{
		if (CMessageBox::Message("Are you sure you want to modify the project properties?", MB_YESNO) == IDNO)
			return S_OK;

		m_pAGDoc->SetDocType(m_idDocType);

		for (int i = 0; i < m_pAGDoc->GetNumPages(); i++)
		{
			CAGPage* pPage = m_pAGDoc->GetPage(i);
			if (!pPage)
				continue;
			SIZE PageSize = {0,0};
			PageSize.cx = INCHES(m_fPageWidths[i]);
			PageSize.cy = INCHES(m_fPageHeights[i]);
			pPage->SetPageSize(PageSize);
			pPage->SetPageName(m_strPageNames[i]);
		}
	}

	EndDialog(IDOK);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPropertiesDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(IDCANCEL);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
LRESULT CPropertiesDlg::OnCtlColor(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_pCtp)
		return m_pCtp->OnCtlColor(uMsg, wParam, lParam, bHandled);

	return NULL;
}

