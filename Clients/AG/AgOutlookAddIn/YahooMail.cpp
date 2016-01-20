#include "stdafx.h"
#include "yahoomail.h"
#include "HelperFunctions.h"


CYahooMail::CYahooMail(CJMExplorerBand * pExplorerBar) : CWebMailBase(pExplorerBar)
{
	m_Usage.SetMethod(METH_YAHOO);
}

CYahooMail::~CYahooMail(void)
{
}
//=============================================================================
//	WriteHtmlToDocument
//=============================================================================
BOOL CYahooMail::WriteHtmlToDocument(BSTR bstrHtml, IHTMLDocument2Ptr spDoc)
{
	CFuncLog log(g_pLog, "CYahooMail::WriteHtmlToDocument()");
	return CWebMailBase::WriteHtmlToDocument(bstrHtml);
}
//=============================================================================
//  CYahooMail::GetComposeInterfaces()
//=============================================================================
bool CYahooMail::GetComposeInterfaces(bool bSilent)
{
	if (CWebMailBase::GetComposeInterfaces(true))
		return true;

	m_spComposeDoc = NULL;
	m_spComposeBrowser = NULL;

	CComPtr<IWebBrowser2> spInWB = m_spWebBrowser;
	CComPtr<IHTMLDocument2> spOutDoc;
	CComPtr<IWebBrowser2> spOutWB;

	// Yahoo's Compose view is contained in an IFrame. If this is the second version of Yahoo
	// then the compose view is in rteEditorComposition0 IFrame nested inside the ymailmain Iframe.
	CString szFrameId;
	if (m_dwVersion == YAHOO_VERSION_01)
		szFrameId.LoadString(IDS_YAHOO_FRAMEID_01);
	else
		szFrameId.LoadString(IDS_YAHOO_FRAMEID_01_ver2);


	bool bFound = false;
	if (GetIEFrameDoc(szFrameId, spInWB, &spOutDoc, &spOutWB))
	{
		bFound = true;
		if (m_dwVersion == YAHOO_VERSION_02)
		{
			spInWB = spOutWB;
			spOutDoc = NULL;
			spOutWB = NULL;

			szFrameId.LoadString(IDS_YAHOO_FRAMEID_02_ver2);
			if (!GetIEFrameDoc(szFrameId, spInWB, &spOutDoc, &spOutWB))
				bFound = false;
		}
	}
	else if (GetIEObjectDoc(spInWB, spOutDoc, spOutWB))
	{
		bFound = true;
	}


	if (bFound)
	{
		m_spComposeDoc = spOutDoc;
		m_spComposeBrowser = spOutWB;
	}
	else if (!bSilent)
		DisplayMessage(IDS_YAHOO_CG_WARN, MB_ICONWARNING | MB_OK | MB_HELP | MB_TOPMOST);
	

	return bFound;
}

//=============================================================================
//  CYahooMail::IsComposeView()   
//=============================================================================
bool CYahooMail::IsComposeView()
{
	if (!GetComposeInterfaces(true))
		return false;

	return true;
}
//=============================================================================
//  CYahooMail::IsNewComposeMessage()   
//=============================================================================
///////////////////////////////////////////////////////////////////////////
// Determines if the documents is a compose message as opposed to a 
// Reply/Forward message. This is used to assess whether or not default 
// stationery should be added.
bool CYahooMail::IsNewComposeMessage()
{
	if (!m_spWebBrowser)
		return false;

	if (!m_spComposeDoc)
		return false;

	return true;
}
//=============================================================================
//  CYahooMail::IsSendEvent()
//=============================================================================
bool CYahooMail::IsSendEvent(IHTMLDocument2 *pDoc)
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
	szMark1.LoadString(IDS_YAHOO_SEND_MARK_01);
	if (szMark1.IsEmpty())
		return false;

	CString szMark2;
	szMark2.LoadString(IDS_YAHOO_SEND_MARK_02);
	if (szMark2.IsEmpty())
		return false;

	szHtml.MakeLower();
	szMark1.MakeLower();
	szMark2.MakeLower();
	if ((szHtml.Find(szMark1) < 0) && (szHtml.Find(szMark2) < 0))
		return false;

	return true;
}
