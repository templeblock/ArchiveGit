#include "stdafx.h"
#include "AddressbookData.h"
#include "Ctp.h"
#include "WebPage.h"

/////////////////////////////////////////////////////////////////////////////
CAddressbookData::CAddressbookData(CCtp * pCtp)
{
	m_pCtp					= pCtp;
	m_lTotalEntries			= 0;
	m_bMailMergeRequested	= false;
	m_bMailListloaded		= false;
	m_hAddressbookDlg		= NULL;
	m_szAddressbookURL		= DEFAULT_ADDRESSBOOK_URL;
}

CAddressbookData::~CAddressbookData(void)
{
	FreeAddressbookPopup();
}
/////////////////////////////////////////////////////////////////////////////
void _cdecl CAddressbookData::XMLCallback(int iLevel, bool bAttribute, LPCTSTR pNodeName, LPCTSTR pValueString, LPARAM lParam)
{
	CString szNodeName = pNodeName;
	szNodeName.MakeUpper();
	
	if (szNodeName.Find(_T("ERROR")) >= 0)
	{
		::MessageBox(NULL, _T(pNodeName), _T("Error"), MB_OK|MB_ICONSTOP);
		return;
	}
	
	return;
}
/////////////////////////////////////////////////////////////////////////////
void CALLBACK CAddressbookData::MyBrowserCallback(HWND hwnd, UINT uMsgId, LPARAM lParam, LPCTSTR szValue)
{
	CAddressbookData * AddressData = (CAddressbookData*)lParam;
	if (!AddressData)
		return;

	if (!hwnd)
		return;

	CString szID(szValue);
	if (WM_INITDIALOG == uMsgId)
	{
		if (szID.CompareNoCase(ID_ADDRESSBOOK) == 0)
		{
			AddressData->SetDlgHandle(hwnd);
			AddressData->GetCtp()->SetWindowIcon(hwnd);
		}
	}
	else if (WM_CLOSE == uMsgId)
	{
		if (szID.CompareNoCase(ID_ADDRESSBOOK) == 0)
		{
			AddressData->FreeAddressbookPopup();
		}
	}
}
//////////////////////////////////////////////////////////////////////
bool CAddressbookData::LoadXML(LPCTSTR szXML)
{
	if (CString(szXML).IsEmpty())
		return false;

	HRESULT hr;
	m_spMailList = NULL;

	// Register callback
	hr = m_XMLDoc.RegisterCallback(XMLCallback, (LPARAM)this);
	if (FAILED(hr))
		return false;


	// Load xml string
	bool bSuccess = false;
	hr = m_XMLDoc.LoadXML(szXML, bSuccess);
	if (!bSuccess)
		return false;

	// Get Mail List id
	GetMailingListId(m_szMailListId);

	// Get Mail List collection
	hr = m_XMLDoc.GetDOMDocument()->getElementsByTagName(CComBSTR(TAG_MAILIST_ENTRY), &m_spMailList);
	if (NULL == m_spMailList)
		return false;
 
	// Get Total entries
	long lLen=0;
	hr = m_spMailList->get_length(&lLen);
	m_lTotalEntries = lLen;

	// if no contacts were added then cancel mail merge.
	if (m_lTotalEntries <= 0)
	{
		m_bMailListloaded = false;
		m_bMailMergeRequested = false;
		CMessageBox::Message(_T("No contacts have been selected for mail merge"));
		return false;
	}
	

	m_bMailListloaded = true;

	return true;
}
////////////////////////////////////////////////////////////////////////
bool CAddressbookData::GetMailingListId(CString& szId)
{
	CString szSearchPattern = _T("//");
	szSearchPattern += TAG_MAILIST; 
	CComPtr<IXMLDOMNode> spMailListNode;
	HRESULT hr = m_XMLDoc.FindElement(szSearchPattern, &spMailListNode);
	CComQIPtr<IXMLDOMElement> spMailListElem(spMailListNode);
	if (!spMailListElem)
		return false;

	VARIANT varValue;
	hr = spMailListElem->getAttribute(CComBSTR(ATTRIBUTE_ID), &varValue);
	if (FAILED(hr))
		return false;

	szId = varValue.bstrVal;

	return true;
}
//////////////////////////////////////////////////////////////////////
int CAddressbookData::GetMailingAddressFromXML(IXMLDOMNode * pNode, CString& szAddress)
{
	if (NULL == m_spMailList)
		return MAILLIST_STATUS_ERROR;

	CComQIPtr<IXMLDOMElement> spMailListElem(pNode);
	if (NULL == spMailListElem)
		return MAILLIST_STATUS_ERROR;


	HRESULT hr;
	VARIANT varValue;
	CString szTemp;
	int nStatus=0;

	// First Name
	varValue.bstrVal = CComBSTR(_T(""));
	hr = spMailListElem->getAttribute(CComBSTR(ATTRIBUTE_FIRSTNAME), &varValue);
	szTemp = varValue.bstrVal;
	if (szTemp.IsEmpty())
		nStatus |= MAILLIST_STATUS_NOFIRSTNAME;

	szAddress = szTemp;
	
	// Last Name
	varValue.bstrVal = CComBSTR(_T(""));
	hr = spMailListElem->getAttribute(CComBSTR(ATTRIBUTE_LASTNAME), &varValue);
	szTemp = varValue.bstrVal;;
	if (szTemp.IsEmpty())
		nStatus |= MAILLIST_STATUS_NOLASTNAME;

	szAddress += _T(" ") + szTemp +_T("\n");

	// Address 1
	varValue.bstrVal = CComBSTR(_T(""));
	hr = spMailListElem->getAttribute(CComBSTR(ATTRIBUTE_ADDRESS1), &varValue);
	szTemp = varValue.bstrVal;;
	if (szTemp.IsEmpty())
	{
		nStatus |= MAILLIST_STATUS_NOADDRESS1;
		return nStatus;
	}

	szAddress += szTemp +_T("\n");

	// Address 2
	varValue.bstrVal = CComBSTR(_T(""));
	hr = spMailListElem->getAttribute(CComBSTR(ATTRIBUTE_ADDRESS2), &varValue);
	szTemp = varValue.bstrVal;;
	if (szTemp.IsEmpty())
		nStatus |= MAILLIST_STATUS_NOADDRESS2;
	else
		szAddress += szTemp +_T("\n");


	// City
	varValue.bstrVal = CComBSTR(_T(""));
	hr = spMailListElem->getAttribute(CComBSTR(ATTRIBUTE_CITY), &varValue);
	szTemp = varValue.bstrVal;;
	if (szTemp.IsEmpty())
	{
		nStatus |= MAILLIST_STATUS_NOCITY;
		return nStatus;
	}

	szAddress += szTemp +_T(", ");

	// State
	varValue.bstrVal = CComBSTR(_T(""));
	hr = spMailListElem->getAttribute(CComBSTR(ATTRIBUTE_STATE), &varValue);
	szTemp = varValue.bstrVal;;
	if (szTemp.IsEmpty())
	{
		nStatus |= MAILLIST_STATUS_NOSTATE;
		return nStatus;
	}
	szAddress += szTemp +_T(" ");

	// Zip
	varValue.bstrVal = CComBSTR(_T(""));
	hr = spMailListElem->getAttribute(CComBSTR(ATTRIBUTE_ZIP), &varValue);
	szTemp = varValue.bstrVal;
	if (szTemp.IsEmpty())
	{
		nStatus |= MAILLIST_STATUS_NOZIP;
		return nStatus;
	}

	szAddress += szTemp;

	return nStatus;
}
//////////////////////////////////////////////////////////////////////
int CAddressbookData::GetMailingAddressByIndex(long lIndex, CString& szAddress)
{
	if (NULL == m_spMailList || lIndex >= m_lTotalEntries)
		return MAILLIST_STATUS_ERROR;

	HRESULT hr;
	int nStatus=0;
	CComPtr<IXMLDOMNode> spNode;
	hr = m_spMailList->get_item(lIndex, &spNode);
	nStatus = GetMailingAddressFromXML(spNode, szAddress);

	return nStatus;
}
//////////////////////////////////////////////////////////////////////
void CAddressbookData::SetAddressbookUrl(LPCTSTR szUrl)
{
	m_szAddressbookURL = szUrl;
	m_AddressbookDlg.SetURL(szUrl, true);
}
//////////////////////////////////////////////////////////////////////
void CAddressbookData::ClosePopup()
{
	if (m_AddressbookDlg.m_hWnd && ::IsWindow(m_AddressbookDlg.m_hWnd))
	{
		::SendMessage(m_AddressbookDlg.m_hWnd, WM_CLOSE, 0, 0);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CAddressbookData::FreeAddressbookPopup()
{
	m_hAddressbookDlg = NULL;
}
/////////////////////////////////////////////////////////////////////////////
bool CAddressbookData::RequestAddressbookSetup(IWebBrowser2* pBrowser)
{
	if (NULL == pBrowser)
		return false;

	CComPtr<IDispatch> spDisp;
	HRESULT hr = pBrowser->get_Document(&spDisp);
	CComQIPtr<IHTMLDocument2> spHtmlDoc(spDisp);
	if (!spHtmlDoc)
		return false;

	CWebPage WebPage;
	
	if (!WebPage.SetDocument(spHtmlDoc))
		return false;

	
	if (!WebPage.CallJScript(_T("SetupAddressbookForCnP")))
		return false;
	

	return true;
	
}
/////////////////////////////////////////////////////////////////////////////
bool CAddressbookData::LaunchAddressbook(HWND hwndParent)
{
	CWaitCursor Wait;
	m_AddressbookDlg.SetDispatch(m_pDisp);
	m_AddressbookDlg.SetURL(m_szAddressbookURL, false);
	m_AddressbookDlg.SetWindowTitle(_T("Create & Print - Mail Merge"));
	m_AddressbookDlg.RegisterCallback(CAddressbookData::MyBrowserCallback, (LPARAM)this, ID_ADDRESSBOOK);
	m_AddressbookDlg.DoModeless(hwndParent);

	return true;
}
//////////////////////////////////////////////////////////////////////
int CAddressbookData::NextAddress(CString &szAddress)
{
	if (NULL == m_spMailList)
		return MAILLIST_STATUS_ERROR;

	HRESULT hr;
	CString szTemp;
	int nStatus=0;
	CComPtr<IXMLDOMNode> spMailListNode;
	hr = m_spMailList->nextNode(&spMailListNode);
	if (NULL == spMailListNode)
		return MAILLIST_STATUS_EMPTY;

	nStatus = GetMailingAddressFromXML(spMailListNode, szTemp);
	szAddress = szTemp;

	return nStatus;
}