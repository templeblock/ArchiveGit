#include "stdafx.h"
#include "WebMailBase.h"
#include "HelperFunctions.h"
#include "JMExplorerBand.h"


/////////////////////////////////////////////////////////////////////////////
CWebMailBase::CWebMailBase(CJMExplorerBand * pExplorerBar)
{
	SetClient(CLIENT_TYPE_IE); 
	m_pExplorerBar		= pExplorerBar;
	m_hWnd				= NULL;
	m_spWebBrowser		= NULL;
	m_spComposeDoc		= NULL;
	m_spComposeBrowser	= NULL;
	m_bOverride			= false;
	
	// Set download info.
	m_Download.SetAppInfo(m_pExplorerBar->m_spClientSite, CLIENT_TYPE_IE);
}

/////////////////////////////////////////////////////////////////////////////
CWebMailBase::~CWebMailBase(void)
{
	m_hWnd				= NULL;
	m_spWebBrowser		= NULL;
	m_spComposeDoc		= NULL;
	m_spComposeBrowser	= NULL;
}

//=============================================================================
//	SetWebbrowser
//=============================================================================
void CWebMailBase::SetWebbrowser(IWebBrowser2 * pWB)
{
	m_spWebBrowser = pWB;
	return;
}
//=============================================================================
//	GetPreviewDialog
//=============================================================================
CPreviewDialog& CWebMailBase::GetPreviewDialog()
{
	return m_PreviewDialog;
}
//=============================================================================
//	GetStationeryMgr
//=============================================================================
CStationeryManager& CWebMailBase::GetStationeryMgr()
{
	return m_StationeryMgr;
}
//=============================================================================
//	GetStationeryMgr
//=============================================================================
CSmartCache& CWebMailBase::GetSmartCache()
{
	return m_SmartCache;
}
//=============================================================================
//	WriteHtmlToDocument
//=============================================================================
BOOL CWebMailBase::WriteHtmlToDocument(BSTR bstrHtml, IHTMLDocument2Ptr spDoc)
{
	CFuncLog log(g_pLog, "CWebMailBase::WriteHtmlToDocument()");
	HRESULT hr;

	if (m_spComposeDoc == NULL)
		return false;


	// Since we have to use put_innerHtml() instead of write() the meta data
	// present on canned content (default stationery) needs to be extracted from
	// html string then re-inserted into document. Here is wehere we extract the
	// meta data (usage data).
	METATAGLIST MetaList;
	m_Usage.GetMetaDataFromString(CString(bstrHtml), JMU_ATTRIBUTE, MetaList);

	// Extract body from html.
	CComBSTR bsHtml = bstrHtml;
	CString szHtml;
	if (GetBody(CString(bstrHtml), szHtml))
	{
		Replace(szHtml, "contenteditable=\"false\"", "unselectable=\"on\"; contenteditable=false");
		bsHtml = CComBSTR(szHtml);
	}

	// Remove JM Preserve (JM_P) attribute that may have been set during GetHtmlText() 
	// (Preserve logic). This ensures that the new body does not erroneously contain 
	// the JM attribute since it is NOT overwritten by WriteHtmlToDocument(). 
    // Note: this only applicable to Web based clients since their WriteHtmlToDocument() uses 
	// put_innerHtml() on the body. Outlook and Express overwrite the whole document. 
	CComPtr<IHTMLElement> spBodyElement;
	hr = m_spComposeDoc->get_body(&spBodyElement);
	if (!spBodyElement)
		return false;
	RemoveJMAttribute(spBodyElement);


	// Set margins to "0px".
	CComPtr<IHTMLStyle> spStyle;
	hr = spBodyElement->get_style(&spStyle);
	if (spStyle)
		hr = spStyle->put_margin(CComBSTR("0px"));


	CComBSTR bsOuterHtml;
	hr = spBodyElement->get_outerHTML(&bsOuterHtml);
	CString szTmp = bsOuterHtml;


	// Insert new body into compose message. We have to use put_innerHTML()
	// instead of write() since the write() evidently interferes with the web
	// client's editor behavior, i.e. Yahoo Mail's smileys can't be inserted.
	//GetBodyFromSelection(m_spComposeDoc, spBodyElement);
	hr = spBodyElement->put_innerHTML(bsHtml);
	if (FAILED(hr))
		return false;

	bool bOK = IsDocLoadedWait(m_spComposeDoc);

	// Here is where we insert any meta data extracted from html
	// string above.
	AddMetaDataToDoc(MetaList);
	MetaList.clear();

	bOK = IsDocLoadedWait(m_spComposeDoc);

	// Set Unselectable attribute to "on" for all IMG elements.
	SetUnselectableAttribute(m_spComposeDoc, _T("img"), _T("on"));


	return true;
}


/**************************************************************************

   GetIEFrameDoc::GetIEFrameDoc()
   
**************************************************************************/
bool CWebMailBase::GetIEFrameDoc(LPCTSTR szFrameId, IWebBrowser2 * pBrowser, IHTMLDocument2** pOutDoc, IWebBrowser2** pOutBrowser)
{
	CComPtr<IWebBrowser2> spWebBrowser(pBrowser);
	if (!spWebBrowser)
		return false;
	
	CComPtr<IDispatch> spDocDisp;
	HRESULT hr = spWebBrowser->get_Document(&spDocDisp);
	if (!spDocDisp)
		return false;

	CComQIPtr<IHTMLDocument2> spDoc2(spDocDisp);
	if (!spDoc2)
		return false;
	
	CComQIPtr<IOleContainer> spContainer(spDoc2);
	if (!spContainer)
		return FALSE;

	// Get an enumerator for the frames
	CComPtr<IEnumUnknown> spEnumerator;   
	hr = spContainer->EnumObjects(OLECONTF_EMBEDDINGS | OLECONTF_LINKS | OLECONTF_OTHERS, &spEnumerator);
	if (FAILED(hr) || spEnumerator == NULL)
		return FALSE;

	
	// Enumerate all the frames
	CComPtr<IUnknown> spUnk;
	CComQIPtr<IWebBrowser2> spWB;
	ULONG uFetched;
	bool bFound = false;
	while (spEnumerator->Next(1, &spUnk, &uFetched) == S_OK && !bFound)
	{
		// For each frame QI for IWebBrowser here to see if we have 
		// an embedded browser
		CComQIPtr<IWebBrowser2> spWB(spUnk);
		spUnk = NULL;
		if (!spWB)
			continue;

		CComQIPtr<IHTMLFrameBase> spFrameBase(spWB);
		if (!spFrameBase)
			continue;

		CComQIPtr<IHTMLElement> spFrameElem(spFrameBase);
		if (!spFrameElem)
			continue;
		
		// Ensure that we have szFrameId IFrame, then get doc.
		CComBSTR bsId;
		spFrameElem->get_id(&bsId);
		CString szId = bsId;

#ifdef _DEBUG
		IDispatchPtr spDisp;
		hr = spWB->get_Document(&spDisp);
		if (SUCCEEDED(hr) && spDisp != NULL)
		{
			CComQIPtr<IHTMLDocument2> spDoc2(spDisp);
			if (spDoc2)
			{
				CComPtr<IHTMLElement> spBodyElement;
				HRESULT hr = spDoc2->get_body(&spBodyElement);
				if (!spBodyElement)
					return false;

				CComBSTR bsHtml;
				hr = spBodyElement->get_outerHTML(&bsHtml);
				CString szTmp = bsHtml;
	
				// Find Head element.
				CComPtr<IHTMLElement> spHeadElem;
				GetHeadElement(spDoc2, spHeadElem);
				if (spHeadElem)
					hr = spHeadElem->get_outerHTML(&bsHtml);
				szTmp = bsHtml;
				if (FAILED(hr))
					return false;
			}
		}
#endif _DEBUG


		szId.MakeLower();
		CString szFrame(szFrameId);
		szFrame.MakeLower();
		if (szId.Find(szFrame) >= 0)
		{
			IDispatchPtr spDocDisp;
			hr = spWB->get_Document(&spDocDisp);
			if (SUCCEEDED(hr) && spDocDisp != NULL)
			{
				CComQIPtr<IHTMLDocument2> spTempDoc(spDocDisp);
				if (spTempDoc)
				{
					CComPtr<IWebBrowser2> spBrowser(spWB);
					*pOutDoc = spTempDoc.p;
					(*pOutDoc)->AddRef();
					*pOutBrowser = spBrowser.p;
					(*pOutBrowser)->AddRef();
					bFound = true;
				}
			}
		}
	}

	return bFound;
}
//=============================================================================
//  GetLastIEWindow()   
//=============================================================================
bool CWebMailBase::GetLastIEWindow(IWebBrowser2 * pBrowser, HWND &hLastIEServer, bool bCheckParent)
{
	CComPtr<IWebBrowser2> spWebBrowser(pBrowser);
	if (!spWebBrowser)
		return false;
	
	// Get web document
	CComPtr<IDispatch> spDocDisp;
	HRESULT hr = spWebBrowser->get_Document(&spDocDisp);
	if (!spDocDisp)
		return false;


	//Get the window handle.
	CComQIPtr<IOleWindow>  spOleWindow(spDocDisp);
    HWND hwnd = NULL;
	hr = spOleWindow->GetWindow(&hwnd);
	if (!hwnd)
		return false;

	// Get last Internet Explorer_Server window
	HWND hwndIEServer = NULL;
	HWND hParent = hwnd;
	bool bFound = false;
	while (hParent)
	{
		GetIEServerWindow(hParent, hwndIEServer, bCheckParent);
		if (hwndIEServer)
			hLastIEServer = hwndIEServer;
		hParent = hwndIEServer;
		hwndIEServer = NULL;
		bFound = true;
	}

	return bFound;

}
//=============================================================================
//  GetDocFromWindow()   
//=============================================================================
bool CWebMailBase::GetDocFromWindow(HWND hwnd, CComPtr<IHTMLDocument2> &spDoc, CComPtr<IWebBrowser2> &spBrowser)
{
	if (!hwnd)
		return false;

	// Get web document
	IHTMLDocument2Ptr spDoc2 = GetHTMLDocument2(hwnd);
	if (NULL == spDoc2)
		return false;

	
	// Get web browser
	CComQIPtr<IWebBrowser2> spWB;
	CComQIPtr<IServiceProvider> spServiceProvider(spDoc2);
    if (FAILED(spServiceProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, (void**)&spWB)))
        return false;

	spBrowser = spWB;
	spDoc = spDoc2;
	
	return true;

}

//=============================================================================
//  GetIEObjectDoc()   
//=============================================================================
bool CWebMailBase::GetIEObjectDoc(IWebBrowser2 * pBrowser, CComPtr<IHTMLDocument2> &spDoc, CComPtr<IWebBrowser2> &spBrowser)
{
	// Get Last child IE Server Window
	HWND hwndIEServer = NULL;
	if (!GetLastIEWindow(pBrowser, hwndIEServer, false))
		return false;

	// Get doc and browser objects.
	if (!GetDocFromWindow(hwndIEServer, spDoc, spBrowser))
		return false;

	return true;
}
//=============================================================================
//	LaunchMail
//=============================================================================
bool CWebMailBase::LaunchMail(bool bAllowReuse, const CString& strDownloadSrc, const CString& strDownloadDst, const CString& strSubject)
{
	CFuncLog log(g_pLog, "CWebMailBase::LaunchMail()");

	if (!m_spWebBrowser)
		return false;

	if (!GetComposeInterfaces(false))
		return false;

	if (m_spComposeDoc == NULL)
		return false;

	bool bSuccess = false;
	HRESULT hr;

	CString strHTMLText;
	if (strDownloadSrc.IsEmpty())
		strHTMLText = strDownloadDst;
	else
		strHTMLText = ReadHTML(strDownloadSrc, strDownloadDst, false/*bPreview*/);

	CComPtr<IDispatch> spDocDisp;
	hr = m_spWebBrowser->get_Document(&spDocDisp);
	if (!spDocDisp)
		return false;

	CComQIPtr<IHTMLDocument2> spDoc2(spDocDisp);
	if (!spDoc2)
		return false;


	if (IsDocInEditMode(m_spComposeDoc))
	{
		// Wait for the document to become ready
		if (!IsDocLoadedWait(m_spComposeDoc))
			return false;

		// Make sure the Add_a_Photo dialog is not up
		if (m_PhotoManager.HasOpenDialog())
			return false;

		// Check Security policies
		if (!VerifySecurity(m_spComposeDoc.p, strHTMLText))
			return false;

		// Reset current hooks.
		DeactivateJazzyMailDocument(m_spComposeDoc.p);

		// Identify non Creata Mail text to preserve, such as signatures
		MarkNonJazzyMailTextForPreservation(m_spComposeDoc.p); 

		// Extract any html that will be retained from current message
		CString strPrevHtmlText;
		GetHtmlText(m_spComposeDoc.p, strPrevHtmlText);

		if (!m_spComposeDoc)
			return false;


#ifdef _DEBUG
		CComPtr<IHTMLElement> spBodyElement;
		HRESULT hr = m_spComposeDoc->get_body(&spBodyElement);
		if (!spBodyElement)
			return false;

		CComBSTR bsHtml;
		hr = spBodyElement->get_outerHTML(&bsHtml);
		if (FAILED(hr))
			return false;
#endif _DEBUG
		

		// Write the html to message window
		if (WriteHtmlToDocument(CComBSTR(strHTMLText)))
		{					
			// Write saved html text from previous message
			SetHtmlText(m_spComposeDoc.p, strPrevHtmlText);

			AddPromo();

			// Wait for the document to become ready
			bool bOK = IsDocLoadedWait(m_spComposeDoc);


			// Setup edit fields to identify which fields are editble 
			// by user and update text insertion point
			SetDocEditFields(m_spComposeDoc.p);

			// Activate the new document to set hooks
			ActivateJazzyMailDocument(m_spComposeDoc.p, true);

			bSuccess = true;
		}

	}

	return bSuccess;
}
//=============================================================================
//	DownloadComplete
//=============================================================================
bool CWebMailBase::DownloadComplete()
{
	CFuncLog log(g_pLog, "CWebMailBase::DownloadComplete()");

	CMsAddInBase::DownloadComplete();

	if (!m_spComposeDoc)
		return false;

	CComPtr<IHTMLElement> spBodyElement;
	HRESULT hr = m_spComposeDoc->get_body(&spBodyElement);
	CComQIPtr<IHTMLElement2> spElem2(spBodyElement);
	if (spElem2)
		hr = spElem2->focus();

	return true;
}
//=============================================================================
//	Mark
//=============================================================================
bool CWebMailBase::Mark(IHTMLElement * pElem)
{
	CFuncLog log(g_pLog, "CWebMailBase::Mark()");

	HRESULT hr;
	CComQIPtr<IHTMLElement> spElem(pElem);
	if (!spElem)
		return false;

	CComBSTR bstrHtml;
	hr = spElem->get_innerHTML(&bstrHtml);
	CString szBuffer(bstrHtml);
	if (FAILED(hr) || szBuffer.IsEmpty())
		return false;
	
	return SetJMAttribute(spElem);
}
//=============================================================================
//	DisplayMessage
//=============================================================================
int CWebMailBase::DisplayMessage(DWORD dwMsgID, UINT uType)
{
	int iResult=-1;
	if (m_pExplorerBar)
		iResult = m_pExplorerBar->DisplayMessage(dwMsgID, uType);

	return iResult;
}
//=============================================================================
//	LaunchFeedbackMail
//=============================================================================
bool CWebMailBase::LaunchFeedbackMail(DWORD dwCmd)
{
	CFuncLog log(g_pLog, "CWebMailBase::LaunchFeedbackMail()");
	CString strUrl = "/mailurl:mailto:tsoltis@ag.com?cc=jmccurdy@ag.com&subject=Creata Mail%20";
	strUrl += (dwCmd == ID_OPTIONS_GIVEFEEDBACK ? "Feedback" : "Problem%20Report");
	CString strTargetPath = "msimn.exe";
	GetAppPath(strTargetPath);
	DWORD dwResult = (DWORD)::ShellExecute(NULL, "open", strTargetPath, strUrl, NULL, SW_SHOWNORMAL);
	return true;
}
//=============================================================================
//	EmbedHTMLAtEnd
//=============================================================================
bool CWebMailBase::EmbedHTMLAtEnd(const CString& strHTMLText)
{
	if (m_spComposeDoc == NULL)
		return false;

	return EmbedHTMLAtEndBase(m_spComposeDoc.p, strHTMLText);
}

//=============================================================================
//	EmbedHTMLAtSelection
//=============================================================================
bool CWebMailBase::EmbedHTMLAtSelection(const CString& strHTMLText)
{
	CFuncLog log(g_pLog, "CWebMailBase::EmbedHTMLAtSelection()");
	
	GetComposeInterfaces(false);

	if (!m_spComposeDoc)
		return false;

	IHTMLDocument2Ptr spDoc2 = m_spComposeDoc.p;
	if (spDoc2 == NULL)
		return false;
	

	bool bSuccess = EmbedHTMLAtSelectionBase(strHTMLText, spDoc2);
	
	AddPromo();

	CComPtr<IHTMLElement> spBodyElement;
	HRESULT hr = m_spComposeDoc->get_body(&spBodyElement);
	CComQIPtr<IHTMLElement2> spElem2(spBodyElement);
	if (spElem2)
		hr = spElem2->focus();

	return bSuccess;
}
/////////////////////////////////////////////////////////////////////////////
bool CWebMailBase::EmbedHTMLAtSelectionBase(const CString& strHTMLText, IHTMLDocument2Ptr spDoc)
{
	CFuncLog log(g_pLog, "CWebMailBase::EmbedHTMLAtSelectionBase()");
	if (spDoc == NULL)
		return false;

	if (!IsDocInEditMode(spDoc))
		return false;

	// Check Security policies
	if (!VerifySecurity(spDoc, strHTMLText))
		return true; // suppress launch mail.

	// Identify non JazzyMail text to preserve, such as signatures
	MarkNonJazzyMailTextForPreservation(spDoc);

	// Get the area that is selected
	CComPtr<IHTMLSelectionObject> spSelectObject;
	HRESULT hr = spDoc->get_selection(&spSelectObject);
	if (FAILED(hr) || spSelectObject == NULL)
		return false;


	// Check selection type;
	CComBSTR bsType;
	hr = spSelectObject->get_type(&bsType);
	CString szType(bsType);
	szType.MakeLower();
	
	// Turn the selection into a range
	CComPtr<IDispatch> spRangeDisp;
	hr = spSelectObject->createRange(&spRangeDisp);
	if (spRangeDisp == NULL)
		return false;

	CComPtr<IHTMLElement> spParentElem;
	CComQIPtr<IHTMLTxtRange> spTxtRange(spRangeDisp);
	CComQIPtr<IHTMLControlRange> spCtlRange(spRangeDisp);
	bool bNeedNewTxtRange = false;
	VARIANT_BOOL bSuccess = VARIANT_TRUE;
	if (szType.Find("none") >= 0 || szType.Find("text") >= 0 )
	{
		// This reduces the selection to just the insertion
        // point. The parentElement method will then return the
        // element directly under the mouse pointer.
		if (spTxtRange)
		{
			//spTxtRange->collapse(VARIANT_FALSE);
			spTxtRange->parentElement(&spParentElem);
		}
	
		// If the parent element is the body element
		// then we need a different text range. This
		// fixes the problem where a smiley is sometimes
		// inserted above stationery (with flash).
		if (spParentElem)
		{
			CComBSTR bstrHtml;
			hr = spParentElem->get_outerHTML(&bstrHtml);
			CString szOuter = bstrHtml;
			szOuter.MakeLower();
			bNeedNewTxtRange = (szOuter.Find("<body") >= 0 || szOuter.Find("<input") >= 0) ||
							   IsAttributeValuePresent(spParentElem, "name", "to")		   || 
							   IsAttributeValuePresent(spParentElem, "name", "subject");
		}
	}
	else if (szType.Find("control") >= 0)
	{
		// A form or image is selected. The commonParentElement
        // will return the selected element.
		CComQIPtr<IHTMLControlRange> spCtlRange(spRangeDisp);
		if (spCtlRange)
			spCtlRange->commonParentElement(&spParentElem);
	}

	// If the current selection does not contain a text range 
	// then find the previous text range by looking for bookmark.
	CComPtr<IHTMLTxtRange> spTxtRange2;
	if (spTxtRange == NULL || bNeedNewTxtRange)
	{
		CComPtr<IHTMLElement> spBodyElement;
		hr = spDoc->get_body(&spBodyElement);
		if (!spBodyElement)
			return false;

		CComQIPtr<IHTMLBodyElement> spBody(spBodyElement);
		if (!spBody)
			return false;

		hr = spBody->createTextRange(&spTxtRange2);
		if (FAILED(hr) || spTxtRange2 == NULL)
			return false;

		spTxtRange = spTxtRange2;
		
		// Move to previous book marked position. 
		if (spTxtRange)
			hr = spTxtRange->moveToBookmark(m_bsBookmark, &bSuccess);

#ifdef _DEBUG
if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready in EmbedHTMLAtSelectionBase - part 1", "Creata Mail", MB_OK);
#endif _DEBUG
			
	}
	
	// If spTxtRange is invalid (!bSuccess) then use SetHtmlText()
	// to embed in a safe place. Otherwise, Insert in the text range.
	if (!bSuccess)
		bSuccess = SetHtmlText(spDoc, strHTMLText);
	else if (spTxtRange)
	{
		hr = spTxtRange->pasteHTML(CComBSTR(strHTMLText));
		bSuccess = SUCCEEDED(hr);
	}

	// If embed has failed at this point there is not need to proceed.
	if (!bSuccess)
		return false;


#ifdef _DEBUG
if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready in EmbedHTMLAtSelectionBase - part 2", "Creata Mail", MB_OK);
#endif _DEBUG

	// Update bookmark to point to this text range.
	if (spTxtRange)
	{
		m_bsBookmark.Empty();
		hr = spTxtRange->getBookmark(&m_bsBookmark);
	}

	
	// Scroll element that contains embed into view. This is particularly
	// useful when embedding into content with multiple pages. For example,
	// "click to" type invitations.
	/*if (spParentElem)
		spParentElem->scrollIntoView(CComVariant(true));*/


	ActivateJazzyMailDocument(spDoc, true/*bSetAttribute*/);

	
	return true;
}
//=============================================================================
//	AddMetaDataToDoc
//=============================================================================
bool CWebMailBase::AddMetaDataToDoc(METATAGLIST &List)
{
	CFuncLog log(g_pLog, "CWebMailBase::AddMetaDataToDoc()");

	if (List.empty())
		return false;

	if (!m_spComposeDoc)
		return false;

	CString szTag;
	METATAGLIST::iterator itr;
	itr = List.begin();
	while(itr != List.end() && !List.empty())
	{
		szTag = (*itr);
		if (!m_Usage.CreateMetaElement(m_spComposeDoc, szTag))
			return false;
		itr++;
	}	
	return true;
}
//=============================================================================
//	OnNewMessage
//=============================================================================
bool CWebMailBase::OnNewMessage()
{
	if (!GetComposeInterfaces(true))
		return false;
	
	if (!m_spComposeDoc)
		return false;

	// Identify non JazzyMail text to preserve, such as signatures
	MarkNonJazzyMailTextForPreservation(m_spComposeDoc.p);

	// If this is a new compose message and there isn't any text 
	// then add the default stationery, if any. 
	// NOTE: The presence of text indicates that the message is most 
	// likely a draft that has been reopened - so don't add stationery.
	// Hotmail and Yahoo do not add signatures or other canned text 
	// at the start of a new message.
	if (IsNewComposeMessage() && !IsTextPresent(m_spComposeDoc))
	{
		if (AddDefaultStationery(m_spComposeDoc))
			AddPromo();
	}

	// Activate the new document to set hooks
	ActivateJazzyMailDocument(m_spComposeDoc.p, false);

	// Now is an another good time to attempt to send usage.
	m_Usage.SendIfReady();

	return true;
}
//=============================================================================
//  CWebMailBase::OnSendEvent()
//=============================================================================
bool CWebMailBase::OnSendEvent()
{
	if (m_spComposeDoc == NULL)
		return false;

	// Process usage data.
	DWORD dwTotalSends = 0;
	m_Usage.GetSendCount(m_spComposeDoc, dwTotalSends);
	m_Usage.SendIfReady();

	return true;
}
//=============================================================================
//	UndoSendEvent
//=============================================================================
bool CWebMailBase::UndoSendEvent()
{
	bool bSuccess = m_Usage.ClearSendCount();
	return bSuccess;
}
//=============================================================================
//	AddPromo
//=============================================================================
bool CWebMailBase::AddPromo()
{
	if (m_spComposeDoc == NULL)
		return false;

	CComPtr<IHTMLElement> spBodyElement;
	HRESULT hr = m_spComposeDoc->get_body(&spBodyElement);
	if (!spBodyElement)
		return false;

	CComBSTR bsHtml;
	hr = spBodyElement->get_outerHTML(&bsHtml);
	CString szHtml(bsHtml);
	if (szHtml.IsEmpty())
		return false;

	// If trailer promo is already present then we're done.
	CString szTrailer;
	szTrailer.LoadString(IDS_TRAILER_NAME);
//j	szTrailer.MakeLower();
//j	szHtml.MakeLower();
	if (szHtml.Find(szTrailer) >= 0)
		return true;

	// Add promo to end of e-mail message for non-paying members
	// or members that heve elected to leave it on
	bool bOK = IsDocLoadedWait(m_spComposeDoc);
	bool bPayingMember = m_Authenticate.IsUserPaidMember();
	bool bPromoFooterOn = m_Authenticate.IsPromoFooterOn();
	bool bPromoAdded = !bPayingMember || bPromoFooterOn;
	if (bPromoAdded)
	{
		CString strHTMLText;
		if (m_Extend.GetString(MAKEINTRESOURCE(IDH_TRAILER), RT_HTML, strHTMLText))
			bPromoAdded = EmbedHTMLAtEnd(strHTMLText);
	}

	return bPromoAdded;
}
//=============================================================================
//	IsTextPresent
//=============================================================================
bool CWebMailBase::IsTextPresent(IHTMLDocument2 *pDoc)
{
	CComPtr<IHTMLDocument2> spDoc(pDoc);
	if (!spDoc)
		return false;

	CComPtr<IHTMLElement> spBody;
	HRESULT hr = spDoc->get_body(&spBody);
	if (!spBody)
		return false;

	CComBSTR bsText;
	hr = spBody->get_innerText(&bsText);
	CString szText(bsText);
	if (szText.IsEmpty())
		return false;

	return true;

}
//=============================================================================
//	GetBodyFromSelection
//=============================================================================
bool CWebMailBase::GetBodyFromSelection(IHTMLDocument2* pDoc, CComPtr<IHTMLElement> &spBodyElem)
{
	CComPtr<IHTMLDocument2> spDoc(pDoc);
	if (!spDoc)
		return false;

	// Get the area that is selected
	CComPtr<IHTMLSelectionObject> spSelectObject;
	HRESULT hr = spDoc->get_selection(&spSelectObject);
	if (FAILED(hr) || spSelectObject == NULL)
		return false;

	
	// Turn the selection into a range
	CComPtr<IDispatch> spRangeDisp;
	hr = spSelectObject->createRange(&spRangeDisp);
	if (spRangeDisp == NULL)
		return false;

	// Check selection type;
	CComBSTR bsType;
	hr = spSelectObject->get_type(&bsType);
	CString szType(bsType);
	szType.MakeLower();

	// Determine if selected object is text range.
	CComPtr<IHTMLElement> spParentElem;
	if (szType.Find("none") >= 0 || szType.Find("text") >= 0 )
	{
		CComQIPtr<IHTMLTxtRange> spTxtRange(spRangeDisp);
		if (spTxtRange)
			spTxtRange->parentElement(&spParentElem);
	}
	// The selected object must be a control
	else
	{
		CComQIPtr<IHTMLControlRange> spCtlRange(spRangeDisp);
		spCtlRange->commonParentElement(&spParentElem);
	}

	// Find body element.
	CComPtr<IHTMLElement> spParentElement2;
	spParentElement2 = spParentElem;
	CString szOuter;
	bool bFound = false;
	while (NULL != spParentElement2.p && !bFound)
	{
		CComPtr<IHTMLElement> spTempParent;
		CComBSTR bstrHtml;
		hr = spParentElement2->get_outerHTML(&bstrHtml);
		szOuter = bstrHtml;
		szOuter.MakeLower();
		if (szOuter.Find("<body") >= 0)
		{
			bFound = true;
			spBodyElem = spParentElement2;
		}
		else
		{
			spTempParent = spParentElement2;
			spParentElement2 = NULL;
			spTempParent->get_parentElement(&spParentElement2);
		}
	}

	return bFound;

}
//=============================================================================
//	GetDocFromSelection
//=============================================================================
bool CWebMailBase::GetDocFromSelection(IHTMLDocument2* pMainDoc, CComPtr<IHTMLDocument2> &spTargetDoc)
{
	CComPtr<IHTMLElement> spBodyElement;
	if (!GetBodyFromSelection(pMainDoc, spBodyElement))
		return false;

	if (!spBodyElement)
		return false;

	CComPtr<IDispatch> spDocDisp;
	HRESULT hr = spBodyElement->get_document(&spDocDisp);
	CComQIPtr<IHTMLDocument2> spDoc2(spDocDisp);
	if (FAILED(hr) || !spDoc2)
		return false;
	
	spTargetDoc = spDoc2;

	return true;
}
//=============================================================================
//	GetComposeInterfaces
//=============================================================================
bool CWebMailBase::GetComposeInterfaces(bool bSilent)
{
	// Check if override conditions exist
	if (GetKeyState(VK_CONTROL) >= 0)
		return false;
	
	m_spComposeDoc = NULL;
	m_spComposeBrowser = NULL;

	CComPtr<IDispatch> spDocDisp;
	m_spWebBrowser->get_Document(&spDocDisp);
	if (!spDocDisp)
		return false;

	CComQIPtr<IHTMLDocument2> spDoc2(spDocDisp);
	if (!spDoc2)
		return false;

	if (!GetDocFromSelection(spDoc2, m_spOverrideComposeDoc))
		return false;

	m_bOverride = true;
	m_spComposeDoc = m_spOverrideComposeDoc;
	m_spComposeBrowser = m_spWebBrowser;

	return true;
}