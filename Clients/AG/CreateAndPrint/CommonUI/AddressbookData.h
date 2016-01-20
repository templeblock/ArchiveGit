#pragma once
#include "XMLDocument2.h"
#include "HTMLDialogEx.h"


static LPCTSTR TAG_MAILIST					= _T("maillist");
static LPCTSTR TAG_MAILIST_ENTRY			= _T("entry");
static LPCTSTR ATTRIBUTE_ID					= _T("id");
static LPCTSTR ATTRIBUTE_FIRSTNAME			= _T("fname");
static LPCTSTR ATTRIBUTE_LASTNAME			= _T("lname");
static LPCTSTR ATTRIBUTE_NICKNAME			= _T("nickname");
static LPCTSTR ATTRIBUTE_ADDRESS1			= _T("addr1");
static LPCTSTR ATTRIBUTE_ADDRESS2			= _T("addr2");
static LPCTSTR ATTRIBUTE_CITY				= _T("city");
static LPCTSTR ATTRIBUTE_STATE				= _T("state");
static LPCTSTR ATTRIBUTE_ZIP				= _T("zip");
static LPCTSTR ATTRIBUTE_COUNTRY			= _T("country");

static const int MAILLIST_STATUS_ERROR			= 0x10000000;
static const int MAILLIST_STATUS_NOTEMPTY		= 0x00000001;
static const int MAILLIST_STATUS_EMPTY			= 0x00000002;
static const int MAILLIST_STATUS_NOFIRSTNAME	= 0x00000004;
static const int MAILLIST_STATUS_NOLASTNAME		= 0x00000008;
static const int MAILLIST_STATUS_NOADDRESS1		= 0x00000010;
static const int MAILLIST_STATUS_NOADDRESS2		= 0x00000020;
static const int MAILLIST_STATUS_NOCITY			= 0x00000040;
static const int MAILLIST_STATUS_NOSTATE		= 0x00000080;
static const int MAILLIST_STATUS_NOZIP			= 0x00000100;
static const int MAILLIST_STATUS_NOCOUNTRY		= 0x00000200;

static LPCTSTR ID_ADDRESSBOOK			= _T("addressbook");
static LPCTSTR DEFAULT_ADDRESSBOOK_URL  = _T("http://www.americangreetings.com/cnp/addressbook.pd");

class CCtp;
///////////////////////////////////////////////////////////////////////////////
class CAddressbookData
{
public:
	CAddressbookData(CCtp * pCtp);
public:
	virtual ~CAddressbookData(void);

public:
	static void _cdecl XMLCallback(int iLevel, bool bAttribute, LPCTSTR pNodeName, LPCTSTR pValueString, LPARAM lParam);
	static void CALLBACK MyBrowserCallback(HWND hwnd, UINT uMsgId, LPARAM lParam, LPCTSTR szValue);
	bool LoadXML(LPCTSTR szXML);
	bool ShowDlg(int nCmd);
	bool LaunchAddressbook(HWND hwndParent);
	bool RequestAddressbookSetup(IWebBrowser2* pBrowser=NULL);
	void ClosePopup();
	void FreeAddressbookPopup();


	bool IsMailMergeRequested() const
		{ return m_bMailMergeRequested; }
	bool IsMailListLoaded() const
		{ return m_bMailListloaded; }
	bool IsAddressbookDisplayed()
		{ return !!m_AddressbookDlg.IsWindow();}
	int NextAddress(CString &szAddress);
	long GetSize() const
		{return m_lTotalEntries; }
	CCtp* GetCtp()
		{return m_pCtp;}
	int GetMailingAddressByIndex(long lIndex, CString& szAddress);
	bool GetMailingListId(CString& szId);
	void SetDispatch(IDispatch* pDisp)
		{ m_pDisp = pDisp; }
	void SetAddressbookUrl(LPCTSTR szUrl);
	void SetMailMergeRequest(bool bRequest)
		{ m_bMailMergeRequested = bRequest;}
	void SetWindowShowCommand(int nCmd)
		{ m_AddressbookDlg.SetWindowShowCommand(nCmd);}
	void SetWindowSize(int iWidth, int iHeight)
		{ m_AddressbookDlg.SetWindowSize(iWidth, iHeight);}
	void SetDlgHandle(HWND hwnd)
		{ m_hAddressbookDlg = hwnd; }
	CString& GetMailListId()
		{ return m_szMailListId; }

protected:
	int GetMailingAddressFromXML(IXMLDOMNode * pNode, CString& szAddress);

protected:
	CXMLDocument2 m_XMLDoc;
	CComPtr<IXMLDOMNodeList> m_spMailList;
	IDispatch * m_pDisp;
	CString m_szMailListId;
	CString m_szAddressbookURL;
	long m_lTotalEntries;
	bool m_bMailMergeRequested;
	bool m_bMailListloaded;
	CHTMLDialogEx m_AddressbookDlg;
	HWND m_hAddressbookDlg;
	CCtp * m_pCtp;

};
