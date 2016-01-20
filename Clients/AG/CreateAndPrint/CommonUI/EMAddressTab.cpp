#include "stdafx.h"
#include "EMAddressTab.h"
#include "EMTabControl.h"
#include "RegKeys.h"
#include "EMDlg.h"

LPCTSTR MAILMERGE_MSG1 = _T("You must select names from\nyour address book.");
LPCTSTR MAILMERGE_MSG2 = _T("The names from your address\nbook will print here.");
LPCTSTR DEFAULT_ADDRESS = _T("FirstName  LastName\nStreetAddress\nCity/Town,  ");
/////////////////////////////////////////////////////////////////////////////
CEMAddressTab::CEMAddressTab(CEMDlg * pEMDlg)
{
	m_pEMDlg = pEMDlg;

	// Set the default addresses and then check the registry for any saved ones.
	m_strSendAddress = DEFAULT_ADDRESS;
	m_strSendAddress += (!CAGDocSheetSize::GetMetric() ? "State  Zip" : "County  PostalCode  Country");
	m_strReturnAddress = m_strSendAddress;
	ReadRegistryString(REGVAL_SENDADDRESS, m_strSendAddress);
	ReadRegistryString(REGVAL_RETURNADDRESS, m_strReturnAddress);
	
}

/////////////////////////////////////////////////////////////////////////////
CEMAddressTab::~CEMAddressTab()
{
}

/////////////////////////////////////////////////////////////////////////////
BOOL CEMAddressTab::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	DoDataExchange(DDX_LOAD);
	m_pDocWindow = m_pEMDlg->GetDocWin();
	InitEnvelopeAddress();
	
	GetDlgItem(IDC_STATIC_ADDRESSHEADER).SetFont(m_LabelFont2);
	GetDlgItem(IDC_STATIC_MAILING).SetFont(m_LabelFont1);
	GetDlgItem(IDC_STATIC_RETURN).SetFont(m_LabelFont1);

	if (m_pDocWindow->m_pCtp->IsAmericanGreetings())
	{
		GetDlgItem(IDC_STATIC_ADDRESSOPTION2).ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RADIO_ADDRESSBOOK).ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ADDRESSBOOK_BUTTON).ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_STATIC_ADDRESSOPTION2).ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_ADDRESSBOOK).ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ADDRESSBOOK_BUTTON).ShowWindow(SW_HIDE);
	}

	SetMsgHandled(false);

	return true;  // return true  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
void CEMAddressTab::GetTextFromAddressDisplay(int nId, CString& szValue)
{
	TCHAR szTemp[MAX_PATH];
	szTemp[0] = _T('\0');
	LRESULT lResult = SendMessage(GetDlgItem(nId), WM_GETTEXT, MAX_PATH, (LPARAM)(LPCSTR)szTemp);
	szValue = szTemp;
	szValue.Replace("\r", "");
}
/////////////////////////////////////////////////////////////////////////////
void CEMAddressTab::SetTextToAddressDisplay(int nId, LPCTSTR szText)
{
	CString szAddress(szText);
	szAddress.Replace("\n", "\r\n");
	SendMessage(GetDlgItem(nId), WM_SETTEXT, szAddress.GetLength(), (LPARAM)(LPCSTR)szAddress);
}
/////////////////////////////////////////////////////////////////////////////
void CEMAddressTab::InitEnvelopeAddress()
{
	SetTextToAddressDisplay(IDC_SEND_ADDRESS, m_strSendAddress);
	SetTextToAddressDisplay(IDC_RETURN_ADDRESS, m_strReturnAddress);
	
	DoDataExchange(DDX_LOAD);
	m_btnAddressbook.EnableWindow(false);
	m_radioManualEntry.SetCheck(BST_CHECKED);
	m_radioReturnManualEntry.SetCheck(BST_CHECKED);
	GetDlgItem(IDC_SEND_ADDRESS).EnableWindow(true);

	return;
}
//////////////////////////////////////////////////////////////////////
bool CEMAddressTab::SetMailAddress(LPCTSTR szAddress, int nID, COLORREF clrFillColor)
{
	CAGSymText* pText = (CAGSymText*)m_pEMDlg->GetDoc()->FindSymbolAnywhereByID(nID, PROCESS_HIDDEN);
	SetAddressText(pText, szAddress, true, clrFillColor, false/*bFirstTime*/);
	m_pDocWindow->SymbolInvalidate(pText);

	return true;
}
//////////////////////////////////////////////////////////////////////
void CEMAddressTab::UpdateMailingAddress()
{
	CDocWindow * pDocWindow = m_pEMDlg->GetDocWin();
	CAGDoc * pDoc = m_pEMDlg->GetDoc();
	if (!pDocWindow || !pDoc)
		return;

	pDocWindow->SymbolUnselect(true/*bClearPointer*/);
	CAddressbookData * pAddressbookData = pDocWindow->m_pCtp->GetAddressbookData();
	CAGSymText* pSendText = (CAGSymText*)pDoc->FindSymbolAnywhereByID(ID_ENVELOPE_SEND, PROCESS_HIDDEN);
	if (BST_CHECKED == m_radioAddressbook.GetCheck())
	{
		bool bRequestMailMerge = false;
		m_btnAddressbook.EnableWindow(true);
		CString szMailAddress = MAILMERGE_MSG1;
		if (pAddressbookData && pAddressbookData->IsMailListLoaded())
		{
			bRequestMailMerge = true;
			szMailAddress = MAILMERGE_MSG2;
			if (pAddressbookData->IsAddressbookDisplayed())
			{
				CString szMsg;
				szMsg.LoadString(IDS_MAILMERGE_MSG1);
				CMessageBox::Message(szMsg);
			}
		}
		if (pAddressbookData)
			pAddressbookData->SetMailMergeRequest(bRequestMailMerge);

		SetTextToAddressDisplay(IDC_SEND_ADDRESS, szMailAddress);
		SetMailAddress(szMailAddress, ID_ENVELOPE_SEND, RGB(0x80,0x80,0x80));
		pSendText->SetSuppressTextEntry(true);
		GetDlgItem(IDC_SEND_ADDRESS).EnableWindow(false);
	}
	else if (BST_CHECKED == m_radioManualEntry.GetCheck())
	{
		if (pAddressbookData)
			pAddressbookData->SetMailMergeRequest(false);

		m_btnAddressbook.EnableWindow(false);
		pSendText->SetSuppressTextEntry(false);
		SetTextToAddressDisplay(IDC_SEND_ADDRESS, m_strSendAddress);
		SetMailAddress(m_strSendAddress, ID_ENVELOPE_SEND);
		GetDlgItem(IDC_SEND_ADDRESS).EnableWindow(true);
	}
	else if (BST_CHECKED == m_radioNoAddress.GetCheck())
	{
		if (pAddressbookData)
			pAddressbookData->SetMailMergeRequest(false);

		m_btnAddressbook.EnableWindow(false);
		//pSendText->SetLocked(false);
		pSendText->SetSuppressTextEntry(false);
		SetTextToAddressDisplay(IDC_SEND_ADDRESS, _T(" "));
		SetMailAddress(_T(" "), ID_ENVELOPE_SEND);
		GetDlgItem(IDC_SEND_ADDRESS).EnableWindow(false);
	}

	return;
}
//////////////////////////////////////////////////////////////////////
void CEMAddressTab::UpdateReturnAddress()
{
	CDocWindow * pDocWindow = m_pEMDlg->GetDocWin();
	if (!pDocWindow)
		return;

	pDocWindow->SymbolUnselect(true/*bClearPointer*/);
	if (BST_CHECKED == m_radioReturnManualEntry.GetCheck())
	{
		GetDlgItem(IDC_RETURN_ADDRESS).EnableWindow(true);
		SetTextToAddressDisplay(IDC_RETURN_ADDRESS, m_strReturnAddress);
		SetMailAddress(m_strReturnAddress, ID_ENVELOPE_RETURN);
	}
	else if (BST_CHECKED == m_radioReturnNoAddress.GetCheck())
	{
		SetTextToAddressDisplay(IDC_RETURN_ADDRESS, _T(" "));
		SetMailAddress(_T(" "), ID_ENVELOPE_RETURN);
		GetDlgItem(IDC_RETURN_ADDRESS).EnableWindow(false);
	}

	return;
}

//////////////////////////////////////////////////////////////////////
void CEMAddressTab::OnMailingAddress(UINT code, UINT id, HWND hWnd)
{
	UpdateMailingAddress();
    return;
}
//////////////////////////////////////////////////////////////////////
void CEMAddressTab::OnReturnAddress(UINT code, UINT id, HWND hWnd)
{
	UpdateReturnAddress();
    return;
}
//////////////////////////////////////////////////////////////////////
void CEMAddressTab::OnAddressbook(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	CDocWindow * pDocWindow = m_pEMDlg->GetDocWin();
	if (!pDocWindow)
		return;

	HWND hParent = NULL;
	if (pDocWindow->m_hWnd && pDocWindow->IsWindow())
		hParent = pDocWindow->m_hWnd;


	pDocWindow->m_pCtp->LaunchAddressbook(hParent);
	return;
}
////////////////////////////////////////////////////////////////////////
//void CEMAddressTab::OnSelectSendAddress(WORD wNotifyCode, WORD wID, HWND hWndCtl)
//{
//	return;
//}
//
////////////////////////////////////////////////////////////////////////
//void CEMAddressTab::OnSelectReturnAddress(WORD wNotifyCode, WORD wID, HWND hWndCtl)
//{
//	return;
//}
//////////////////////////////////////////////////////////////////////
void CEMAddressTab::OnApplySendAddress(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	GetTextFromAddressDisplay(IDC_SEND_ADDRESS, m_strSendAddress);
	if (m_strSendAddress.IsEmpty())
		m_strSendAddress = _T(" ");
	SetMailAddress(m_strSendAddress, ID_ENVELOPE_SEND);

	return;
}
//////////////////////////////////////////////////////////////////////
void CEMAddressTab::OnApplyReturnAddress(WORD wNotifyCode, WORD wID, HWND hWndCtl)
{
	GetTextFromAddressDisplay(IDC_RETURN_ADDRESS, m_strReturnAddress);
	if (m_strReturnAddress.IsEmpty())
		m_strReturnAddress = _T(" ");
	SetMailAddress(m_strReturnAddress, ID_ENVELOPE_RETURN);
	
	return;
}
/////////////////////////////////////////////////////////////////////////////
void CEMAddressTab::OnEnterTab(bool bFirstTime)
{
}

/////////////////////////////////////////////////////////////////////////////
void CEMAddressTab::OnLeaveTab()
{
}
