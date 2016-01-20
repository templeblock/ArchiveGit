#include "stdafx.h"
#include ".\hotmail.h"

CHotmail::CHotmail(CJMExplorerBand * pExplorerBar) : CWebMailBase(pExplorerBar)
{
	m_Usage.SetMethod(METH_HOTMAIL);
}

CHotmail::~CHotmail(void)
{
}
//=============================================================================
//	WriteHtmlToDocument
//=============================================================================
BOOL CHotmail::WriteHtmlToDocument(BSTR bstrHtml, IHTMLDocument2Ptr spDoc)
{
	CFuncLog log(g_pLog, "CHotmail::WriteHtmlToDocument()");	
	return CWebMailBase::WriteHtmlToDocument(bstrHtml);
}

///////////////////////////////////////////////////////////////////////////
//   CHotmail::GetComposeInterfaces()   
///////////////////////////////////////////////////////////////////////////
bool CHotmail::GetComposeInterfaces(bool bSilent)
{
	if (CWebMailBase::GetComposeInterfaces(true))
		return true;

	m_spComposeDoc = NULL;
	m_spComposeBrowser = NULL;

	CComPtr<IWebBrowser2> spInWB = m_spWebBrowser;
	CComPtr<IHTMLDocument2> spOutDoc;
	CComPtr<IWebBrowser2> spOutWB;

	// Hotmail's Compose view is contained in a EditBox IFrame nested inside the
	// richedit IFrame;
	bool bFound = false;
	CString szFrameId;
	szFrameId.LoadString(IDS_HOTMAIL_FRAMEID_01);
	if (GetIEFrameDoc(szFrameId, spInWB, &spOutDoc, &spOutWB))
	{
		bFound = true;
		spInWB = spOutWB;
		spOutDoc = NULL;
		spOutWB = NULL;

		szFrameId.LoadString(IDS_HOTMAIL_FRAMEID_02);
		if (!GetIEFrameDoc(szFrameId, spInWB, &spOutDoc, &spOutWB))
			bFound = false;
	}

	if (bFound)
	{
		m_spComposeDoc = spOutDoc;
		m_spComposeBrowser = spOutWB;
	}
	else if (!bSilent)
		DisplayMessage(IDS_HOTMAIL_RTE_WARN, MB_ICONWARNING | MB_OK | MB_HELP | MB_TOPMOST);
	
	return true;
}
///////////////////////////////////////////////////////////////////////////
//   CHotmail::IsNewComposeMessage()   
///////////////////////////////////////////////////////////////////////////
// Determines if the documents is a compose message as opposed to a 
// Reply/Forward message. This is used to assess whether or not default 
// stationery should be added.
bool CHotmail::IsNewComposeMessage()
{
	if (!m_spWebBrowser)
		return false;

	CComPtr<IDispatch> spDocDisp;
	HRESULT hr = m_spWebBrowser->get_Document(&spDocDisp);
	if (!spDocDisp)
		return false;

	CComQIPtr<IHTMLDocument2> spDoc(spDocDisp);
	if (!spDoc)
		return false;
	
	CComBSTR bsTitle;
	hr = spDoc->get_title(&bsTitle);
	CString szTitle(bsTitle);
	if (szTitle.IsEmpty())
		return false;

	szTitle.MakeLower();
	if (szTitle.Find(_T("new message")) < 0)
		return false;
	
	return true;
}
//=============================================================================
//  CHotmail::IsSendEvent()
//=============================================================================
bool CHotmail::IsSendEvent(IHTMLDocument2 *pDoc)
{
	CComPtr<IHTMLDocument2> spDoc(pDoc);
	if (!spDoc)
		return false;

	CComPtr<IHTMLElement> spBodyElement;
	HRESULT hr = spDoc->get_body(&spBodyElement);
	if (!spBodyElement)
		return false;

	CComBSTR bsHtml;
	hr = spBodyElement->get_innerHTML(&bsHtml);
	CString szHtml(bsHtml);
	if (szHtml.IsEmpty())
		return false;

	CString szMark1;
	szMark1.LoadString(IDS_HOTMAIL_SEND_MARK_01);
	if (szMark1.IsEmpty())
		return false;

	szHtml.MakeLower();
	szMark1.MakeLower();
	if (szHtml.Find(szMark1) < 0)
		return false;

	return true;
}