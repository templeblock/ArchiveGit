#include "stdafx.h"
#include "MsAddInBase.h"
#include "AboutDlg.h"
#include "FieldArray.h"
#include "Version.h"
#include "Utility.h"
#include "messagebox.h"

#pragma comment(lib, "oleacc.lib")

//#import "progid:JMSrvr.JMBrkr"
//using namespace JMSrvrLib;
/////////////////////////////////////////////////////////////////////////////
CMsAddInBase::CMsAddInBase() :
	m_HTMLDialog(this),
	m_PreviewDialog(this),
	m_StationeryMgr(this)
{
	m_bExpress = false;
	m_bSrvrShutdown = false;
	m_hJMSrvrUtilWnd = NULL;
	m_hClientWnd = NULL;
	m_hTopParent = NULL;
	
	// Explicitly load MSAA so we know if it's installed
	m_hMSAAInst = ::LoadLibrary( _T("OLEACC.DLL") );
	if (!m_hMSAAInst)
		::MessageBox(NULL, _T("MSAA library is missing. Contact technical support!"), g_szAppName, MB_ICONERROR | MB_OK);
	
	//m_Authenticate.SetBaseURL((LPCTSTR)POPUP_BASEURL);
}

/////////////////////////////////////////////////////////////////////////////
CMsAddInBase::~CMsAddInBase()
{
	m_bsBookmark.Empty();
#ifdef _DEBUG
	//::MessageBox(NULL, "CMsAddInBase Destroyed", g_szAppName, MB_OK);
#endif _DEBUG
}

/////////////////////////////////////////////////////////////////////////////
 int CMsAddInBase::DisplayMessage(DWORD dwMsgID, UINT uType)
{
	CString szMsg;
	szMsg.LoadString(dwMsgID);
	::MessageBox(NULL, szMsg, g_szAppName, uType);
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::GetTrayIconStatus(BOOL& bShow)
{
	return m_Authenticate.ReadShowIcon((DWORD&)bShow);
}

/////////////////////////////////////////////////////////////////////////////
DWORD CMsAddInBase::GetClientType()
{
	return m_dwClient;
}

/////////////////////////////////////////////////////////////////////////////
CSmartCache& CMsAddInBase::GetSmartCache()
{
	return m_SmartCache;
}
/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::SetTrayIconStatus(BOOL bShow)
{
	return m_Authenticate.WriteShowIcon((DWORD)bShow);
}

/////////////////////////////////////////////////////////////////////////////
void CMsAddInBase::SetClient(DWORD dwType)
{
	m_dwClient = dwType;
	m_StationeryMgr.SetClientType(dwType);
}

/////////////////////////////////////////////////////////////////////////////
void CMsAddInBase::SetExpress(bool bExpress)
{
	m_bExpress = bExpress;
}

/////////////////////////////////////////////////////////////////////////////
void CMsAddInBase::SetDownloadParms(DownloadParms &dParms)
{
	m_DownloadParms = dParms;
}

/////////////////////////////////////////////////////////////////////////////
void CMsAddInBase::SetLastCommand(WORD wCmd)
{
	m_wLastCommand = wCmd;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::IsDefaultStationeryNeeded()
{
	return m_StationeryMgr.NeedToAdd();
}
/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::IsUserPaidMember()
{
	return m_Authenticate.IsUserPaidMember();
}
/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::WriteHTMLFile(const CString& strHTMLFileName, LPCTSTR pstrHtml)
{
	HANDLE hFile = ::CreateFile(strHTMLFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;
	DWORD dwBytesWritten = 0;
	bool bFailed = !::WriteFile(hFile, (LPCVOID)(LPCSTR)pstrHtml, strlen(pstrHtml), &dwBytesWritten, NULL);
	::CloseHandle(hFile);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
CString CMsAddInBase::ReadHTML(const CString strDownloadSrc, const CString strDownloadDst, bool bPreview)
{
	CString strHTMLText = ReadHTMLFile(strDownloadDst);
	if (!strHTMLText.IsEmpty())
		strHTMLText = PrepareHTML(strHTMLText, strDownloadSrc, bPreview);
	return strHTMLText;
}

/////////////////////////////////////////////////////////////////////////////
CString CMsAddInBase::ReadHTMLFile(const CString& strHTMLFileName)
{
	HANDLE hFile = ::CreateFile(strHTMLFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return "";
	DWORD dwLength = ::GetFileSize(hFile, NULL);
	if (dwLength <= 0)
		return "";
	TCHAR* pBuffer = new TCHAR[dwLength];
	if (!pBuffer)
		return "";
	DWORD dwBytesRead = 0;
	bool bFailed = !::ReadFile(hFile, pBuffer, dwLength, &dwBytesRead, NULL);
	CString strHTML = pBuffer;
	delete [] pBuffer;
	::CloseHandle(hFile);

	// Remove any garbage beyond the last HTML tag
	int index = strHTML.ReverseFind('>');
	if (index >= 0)
		strHTML = strHTML.Left(index+1);
		
	return strHTML;
}

/////////////////////////////////////////////////////////////////////////////////////////////
 BOOL CMsAddInBase::WriteHtmlToDocument(BSTR bstrHtml, IHTMLDocument2Ptr spDoc)
{
	CFuncLog log(g_pLog, "CMsAddinBase::WriteHtmlToDocument()");

	if (spDoc == NULL)
		return false;	

	// Creates a new one-dimensional array
	CComSafeArray<VARIANT> sfArray;
	sfArray.Create(1,0);
	sfArray[0] = CComBSTR(bstrHtml);
	
#ifdef _DEBUG
	if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready in WriteHtmlToDocument - part 1", g_szAppName, MB_OK);
#endif _DEBUG

	// Write the new HTML to the document.
	HRESULT hr = spDoc->write(sfArray);
	sfArray.Destroy();
	spDoc->close(); // Flushes the output stream

	bool bOK = IsDocLoadedWait(spDoc);

#ifdef _DEBUG
	if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready in WriteHtmlToDocument - part 2", g_szAppName, MB_OK);
#endif _DEBUG

	// Set character set. This works around Outlook Express bug where
	// the "ÿþ" shows up in html making the html source unreadable. Although
	// the html appears ok to sender it appears as garbage to recepient (chinese).
	spDoc->put_charset(CComBSTR("Windows-1252"));
	
#ifdef _DEBUG
	if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready in WriteHtmlToDocument - part 3", g_szAppName, MB_OK);
#endif _DEBUG

	// Set Unselectable attribute to "on" for all IMG elements.
	SetUnselectableAttribute(spDoc, _T("img"), _T("on"));

	return SUCCEEDED(hr);
}

/////////////////////////////////////////////////////////////////////////////
// Extract any html that will be retained from current message
BOOL CMsAddInBase::GetHtmlText(IHTMLDocument2Ptr spDoc, CString& szHtml)
{
	if (spDoc == NULL)
		return FALSE;

	// Get any usage meta data that may need to be preserved.
	m_Usage.GetMetaData(spDoc);
	
	HRESULT hr;
	CComPtr<IHTMLElement> spElement;
	hr = spDoc->get_body(&spElement);
	if (!spElement)
		return FALSE;
	
	// Remove all occurences of "DoubleClickTo".
	// This also removes JM attribute of parent element,
	ReplaceAllText(spDoc, "DoubleClickTo", "");

	CComPtr<IHTMLElementCollection> spAll;
	hr = spDoc->get_all(&spAll);
	if (!spAll)
		return FALSE;

	long nCnt = 0;
	hr = spAll->get_length(&nCnt);
	if (FAILED(hr))
		return FALSE;

	CComPtr<IHTMLElement> spPrevElem;
	CString strText;
	for (long i=0; i < nCnt; i++)
	{
		CComPtr<IDispatch> spTagDisp;
		hr = spAll->item(CComVariant(i), CComVariant(i), &spTagDisp);
		if (FAILED(hr) || spTagDisp == NULL)
			return FALSE;

		CComQIPtr<IHTMLElement> spElem(spTagDisp);
		if (!spElem)
			continue;

		// Only interested in elements that have been marked for text preservation,
		// those that have JM attribute set.
		if (!IsAttributePresent(spElem, JM_ATTRIBUTE))
			continue;

		// Ignore elements whose parent element has already been preserved,
		// child elements are included with parent.
		if (spPrevElem)
		{
			VARIANT_BOOL bChild = VARIANT_FALSE;
			spPrevElem->contains(spElem, &bChild);
			if (bChild)
				continue;
		}

		CComBSTR bstrHtml;
		hr = spElem->get_innerHTML(&bstrHtml);
		CString strInnerHTML(bstrHtml);

		// Check to see if this is a embedable Creata Mail object. 
		// Need this for scenarios where you insert just embedable
		// object with no text present. For example, if you insert 
		// smiley or sounds on blank message and then insert stationery.
		IsJazzyMailTag(spElem, "img", strInnerHTML);
		IsJazzyMailTag(spElem, "object", strInnerHTML);

		// Check inner html, this may include <BR>
		if (strInnerHTML.IsEmpty())
			continue;

		spPrevElem = spElem;
		strText += strInnerHTML;
	}

	if (!strText.IsEmpty())
	{
		// To maintain formatting on blank lines in eCards
		Replace(strText, "<div>&nbsp;</div>", "<br>");
		Replace(strText, "<br>", "&shy;<br>&shy;");
		Replace(strText, "&shy;&shy;", "&shy;");
		szHtml = "<span ";
		szHtml += JM_ATTRIBUTE; 
		szHtml += "=\"1\">";
		szHtml += strText;
		szHtml += "</span>";
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Restore any text that was retained from another message
BOOL CMsAddInBase::SetHtmlText(IHTMLDocument2Ptr spDoc, const CString& szText)
{
	CFuncLog log(g_pLog, "CMsAddinBase::SetHtmlText()");

	if (szText.IsEmpty())
		return false;
	
	if (spDoc == NULL)
		return false;

	CComPtr<IHTMLElement> spBodyElement;
	HRESULT hr = spDoc->get_body(&spBodyElement);
	if (!spBodyElement)
		return false;

	long lBodyIndex = 0;
	hr = spBodyElement->get_sourceIndex(&lBodyIndex);
	if (FAILED(hr))
		return false;

	CComPtr<IDispatch> spDisp;
	hr = spBodyElement->get_all(&spDisp);
	if (FAILED(hr) || spDisp == NULL)
		return false;

	CComQIPtr<IHTMLElementCollection> spAll(spDisp);
	if (!spAll)
		return false;

	long nCnt = 0;
	hr = spAll->get_length(&nCnt);
	if (FAILED(hr))
		return FALSE;

	CComPtr<IHTMLElement> spTargetElem;
	CComPtr<IHTMLElement> spLastEditElem;
	for (long i=0; i < nCnt; i++)
	{
		CComPtr<IDispatch> spTagDisp;
		hr = spAll->item(CComVariant(i), CComVariant(i), &spTagDisp);
		if (FAILED(hr) || spTagDisp == NULL)
			return false;

		CComQIPtr<IHTMLElement> spElem(spTagDisp);
		if (!spElem)
			continue;

		long lIndex = 0;
		hr = spElem->get_sourceIndex(&lIndex);
		if (FAILED(hr) || lIndex == lBodyIndex)
			continue;

		// If element is marked with Insert attribute then
		// this is a potential target element where we will insert 
		// szText. However, we keep checking just in case there
		// might be multiple elements marked - we'll use the last one.
		if (IsAttributePresent(spElem, JMINSERT_ATTRIBUTE))
		{
			spTargetElem = spElem;
			continue;
		}

		// Once we have a target element then we can ignore other editable
		// elements. However, we keep looking to ensure we have the very
		// last target candidate. That's because there can be elements with 
		// JM Insert attribute intermingled with other elements.
		if (spTargetElem)
			continue;

		CComQIPtr<IHTMLElement3> spElem3(spElem);
		if (spElem3)
		{
			VARIANT_BOOL bVal = VARIANT_FALSE;
			hr = spElem3->get_isContentEditable(&bVal);
			if (FAILED(hr) || !bVal)
				continue;
		}
		else // spElem3 will be NULL with IE less than IE5.5.
		{
			if (!IsElementContentEditable(spElem))
				continue;
		}

		CComBSTR bstrHtml;
		hr = spElem->get_innerHTML(&bstrHtml);
		CString szInner = bstrHtml;
		if (FAILED(hr))
			continue;

		hr = spElem->get_outerHTML(&bstrHtml);
		CString szOuter = bstrHtml;
		szOuter.MakeLower();
		bool bImage = (szOuter.Find("<img") >= 0);

		if (szInner.IsEmpty() && !bImage)
			continue;
		
		// This is the default element that will be used if there were 
		// no elements found that had the JM Insert attribute.
		spLastEditElem = spElem;
	}

#ifdef _DEBUG
	if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready in SetHtmlText - part 1", g_szAppName, MB_OK);
#endif _DEBUG

	// If target element was found then insert html in this element. 
	// Otherwise, if an editible element was found insert there.
	// Otherwise, as last resort insert at end of body element.
	if (spTargetElem)
	{
		hr = spTargetElem->insertAdjacentHTML(CComBSTR("beforeEnd"), CComBSTR(szText));
		if (SUCCEEDED(hr))
			return true;
	}

	if (spLastEditElem)
	{
		hr = spLastEditElem->insertAdjacentHTML(CComBSTR("beforeEnd"), CComBSTR(szText));
		if (SUCCEEDED(hr))
			return true;
	}

	hr = spBodyElement->insertAdjacentHTML(CComBSTR("beforeEnd"), CComBSTR(szText));		

#ifdef _DEBUG
	if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready in SetHtmlText - part 2", g_szAppName, MB_OK);
#endif _DEBUG

	return SUCCEEDED(hr);
}

/////////////////////////////////////////////////////////////////////////////
// Sets text insertion point to the start of the first element that contains szText
bool CMsAddInBase::SetFocusToText(IHTMLDocument2Ptr spDoc, LPCSTR szText)
{
	CFuncLog log(g_pLog, "CMsAddinBase::SetFocusToText()");

	if (spDoc == NULL)
		return false;
	
	CComPtr<IHTMLElement> spElement;
	HRESULT hr = spDoc->get_body(&spElement);
	if (!spElement)
		return false;

	CComQIPtr<IHTMLBodyElement> spBody(spElement);
	if (!spBody)
		return false;
	
	IHTMLTxtRangePtr spTxtRange;
	hr = spBody->createTextRange(&spTxtRange);
	if (spTxtRange == NULL)
		return false;

	CComBSTR bsText(szText);
	VARIANT_BOOL bSuccess = VARIANT_FALSE;
	hr = spTxtRange->findText(bsText, 0, 0, &bSuccess);
	if (FAILED(hr) || !bSuccess)
		return false;

	// Set insertion point to end of text.
	long lActual=0;
	VARIANT_BOOL bOk = VARIANT_FALSE;
	hr = spTxtRange->expand(CComBSTR("word"), &bOk);			// expand to include rest of text, i.e "DoubleClickTOSTARTMESSAGE".
	hr = spTxtRange->expand(CComBSTR("character"), &bOk);		// expand to include period ".". NOTE: Did not use "sentence" as Uint because it sometimes included text after period.
	//hr = spTxtRange->put_text(CComBSTR(""));					// Repace text range text.
	hr = spTxtRange->collapse(VARIANT_FALSE);					// Move insertion point to end of selection, i.e. starting point for new text range.
	hr = spTxtRange->select();									// Change selection from text to insertion point.
	
	// Update bookmark to point to this text range.
	m_bsBookmark.Empty();
	hr = spTxtRange->getBookmark(&m_bsBookmark);

#ifdef _DEBUG
	if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready in SetFocusToText", g_szAppName, MB_OK);
#endif _DEBUG

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::SetJazzyMailAttribute(IHTMLDocument2Ptr spDoc)
{
	CFuncLog log(g_pLog, "CMsAddinBase::SetJazzyMailAttribute()");

	if (spDoc == NULL)
		return false;

	IHTMLElementPtr spElement;
	HRESULT hr = spDoc->get_body(&spElement);
	if (spElement == NULL)
		return false;

	hr = spElement->setAttribute(CComBSTR(JAZZYMAIL_ATTRIBUTE), CComVariant(1, VT_INT));

#ifdef _DEBUG
	if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready in SetJazzyMailAttribute", g_szAppName, MB_OK);
#endif _DEBUG

	return SUCCEEDED(hr);
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::SetJMAttribute(IHTMLElement* pElem)
{
	CFuncLog log(g_pLog, "CMsAddinBase::SetJMAttribute()");

	CComPtr<IHTMLElement> spElem(pElem);
	if (spElem == NULL)
		return false; 

	HRESULT hr = spElem->setAttribute(CComBSTR(JM_ATTRIBUTE), CComVariant(1, VT_INT));
	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::RemoveJazzyMailAttribute(IHTMLDocument2Ptr spDoc)
{
	CFuncLog log(g_pLog, "CMsAddinBase::RemoveJazzyMailAttribute()");

	if (spDoc == NULL)
		return false;

	CComPtr<IHTMLElement> spElement;
	HRESULT hr = spDoc->get_body(&spElement);
	if (FAILED(hr) || spElement == NULL)
		return false;

	VARIANT_BOOL bSuccess = VARIANT_FALSE;
	hr = spElement->removeAttribute(CComBSTR(JAZZYMAIL_ATTRIBUTE), 1, &bSuccess);

#ifdef _DEBUG
	if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready in RemoveJazzyMailAttribute", g_szAppName, MB_OK);
#endif _DEBUG

	if (FAILED(hr) || !bSuccess)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMsAddInBase::ScrollIntoView(IHTMLDocument2Ptr spDoc)
{
	if (spDoc == NULL)
		return false;

	CComPtr<IHTMLElement> spBodyElement;
	HRESULT hr = spDoc->get_body(&spBodyElement);
	if (!spBodyElement)
		return false;

	long lBodyIndex = 0;
	hr = spBodyElement->get_sourceIndex(&lBodyIndex);
	if (FAILED(hr))
		return false;

	CComPtr<IDispatch> spDisp;
	hr = spBodyElement->get_all(&spDisp);
	if (FAILED(hr) || spDisp == NULL)
		return false;

	CComQIPtr<IHTMLElementCollection> spAll(spDisp);
	if (!spAll)
		return false;

	long nCnt = 0;
	hr = spAll->get_length(&nCnt);
	if (FAILED(hr))
		return false;

	for (int i=0; i<nCnt; i++)
	{
		CComPtr<IDispatch> spTagDisp;
		hr = spAll->item(CComVariant(i), CComVariant(i), &spTagDisp);
		if (FAILED(hr) || !spTagDisp)
			continue;

		CComQIPtr<IHTMLElement> spElem(spTagDisp);
		if (!spElem)
			continue;

		long lIndex = 0;
		hr = spElem->get_sourceIndex(&lIndex);
		if (lIndex != lBodyIndex)
		{
			hr = spElem->scrollIntoView(CComVariant(true));
			break;
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// Clears all occurences of szText in document.
bool CMsAddInBase::ReplaceAllText(IHTMLDocument2Ptr spDoc, LPCTSTR szText, LPCTSTR szReplace, bool bExpand,  bool bRemoveJMAttribute)
{
	CFuncLog log(g_pLog, "CMsAddinBase::ReplaceAllText()");

	if (spDoc == NULL)
		return false;
	
	CComPtr<IHTMLElement> spElement;
	HRESULT hr = spDoc->get_body(&spElement);
	if (!spElement)
		return false;

	CComQIPtr<IHTMLBodyElement> spBody(spElement);
	if (!spBody)
		return false;
	
	CComPtr<IHTMLTxtRange> spTxtRange;
	hr = spBody->createTextRange(&spTxtRange);
	if (spTxtRange == NULL)
		return false;

	long lActual;
	CComBSTR bsText(szText);
	VARIANT_BOOL bSuccess = VARIANT_TRUE;
	VARIANT_BOOL bOk = VARIANT_FALSE;
	hr = spTxtRange->findText(bsText, 0, 0, &bSuccess);
	while (SUCCEEDED(hr) && bSuccess)
	{
		CComPtr<IHTMLElement> spElem;
		hr = spTxtRange->parentElement(&spElem);

		CComBSTR bsHtml;
		spElem->get_outerHTML(&bsHtml);
		CString szHtml(bsHtml);

		if (bRemoveJMAttribute)
			RemoveJMAttribute(spElem);

		if (bExpand)
		{
			hr = spTxtRange->expand(CComBSTR("word"), &bOk);		 // expand to include rest of text, i.e "DoubleClickTOSTARTMESSAGE".
			hr = spTxtRange->expand(CComBSTR("character"), &bOk);	 // expand to include period ".". NOTE: Did not use "sentence" as Uint because it sometimes included text after period.
		}
		hr = spTxtRange->put_text(CComBSTR(szReplace));				 // Repace text range text.
		hr = spTxtRange->select();									 // Select the found text range.
		hr = spTxtRange->collapse(VARIANT_FALSE);					 // Move insertion point to end of selection, i.e. starting point for new text range.
		hr = spTxtRange->moveEnd(CComBSTR("textedit"), 1, &lActual); // Move range end point to original end, i.e. end of body.
		hr = spTxtRange->findText(bsText, 0, 0, &bSuccess);			 // Now look for next occurrence.
		
		spElem->get_outerHTML(&bsHtml);
		szHtml = bsHtml;
	}

#ifdef _DEBUG
	if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready in ReplaceAllText", g_szAppName, MB_OK);
#endif _DEBUG

	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////////////
BOOL CMsAddInBase::RemoveJMAttribute(IHTMLElement* pElem)
{	
	CFuncLog log(g_pLog, "CMsAddinBase::RemoveJMAttribute()");

	CComPtr<IHTMLElement> spElem(pElem);
	if (!spElem)
		return false;

	if (!IsAttributePresent(spElem, JM_ATTRIBUTE))
		return true;

	VARIANT_BOOL bSuccess = VARIANT_FALSE;
	HRESULT hr = spElem->removeAttribute(CComBSTR(JM_ATTRIBUTE), 1, &bSuccess);
	if (FAILED(hr) || !bSuccess)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::IsAttributePresent(IHTMLElement* pElem, LPCTSTR szAttribute)
{
	CFuncLog log(g_pLog, "CMsAddinBase::IsAttributePresent()");

	CComPtr<IHTMLElement> spElem(pElem);
	if (!spElem)
		return false; 

	CComVariant varValue; 
	varValue.vt = VT_NULL;
	HRESULT hr = spElem->getAttribute(CComBSTR(szAttribute), 0, &varValue);
	if (FAILED(hr) || (varValue.vt == VT_NULL))
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::IsComment(IHTMLElement* pElem, LPCTSTR szCommentTag)
{
	CFuncLog log(g_pLog, "CMsAddinBase::IsComment()");

	CComPtr<IHTMLElement> spElem(pElem);
	if (!spElem)
		return false; 

	CComBSTR bsOuterHtml;
	HRESULT hr = spElem->get_outerHTML(&bsOuterHtml);
	CString szOuter(bsOuterHtml);
	if (FAILED(hr) || szOuter.CompareNoCase(szCommentTag) != 0)
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::IsJazzyMailTag(IHTMLElement* pElem, LPCTSTR szName, CString &szOuterHtml)
{
	if (szName == NULL)
		return false;

	CComPtr<IHTMLElement> spElem(pElem);
	if (!spElem)
		return false; 

	CComBSTR bsTagName;
	HRESULT hr = spElem->get_tagName(&bsTagName);
	CString szTagName(bsTagName);
	if (FAILED(hr) || szTagName.IsEmpty()) 
		return false;

	if (szTagName.CompareNoCase(szName) != 0)
		return false;

	if (!IsAttributePresent(spElem, JM_ATTRIBUTE))
		return false;

	CComBSTR bstrTemp;
	hr = spElem->get_outerHTML(&bstrTemp);
	CString szTemp(bstrTemp);
	if (FAILED(hr) || szTemp.IsEmpty()) 
		return false;

	szOuterHtml = szTemp;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::VerifySecurity(IHTMLDocument2Ptr spDoc, const CString &szHtml)
{
	CFuncLog log(g_pLog, "CMsAddinBase::VERIFYSECURITY()");
	if (spDoc == NULL)
		return false;
	
	// Security check is only applicable for content with object tags.
	CString szTemp = szHtml;
	szTemp.MakeLower();

	if (szTemp.IsEmpty())
		return true; // Security check passed.

	// Verify html settings
	//m_Security.FixupHtmlSettings(false/*bSilent*/, m_dwClient); // Tim no longer wants prompt.

	if (szTemp.Find("<object") < 0)
		return true; // Security check passed.

	if (m_Security.FixupSecurityZone(false/*bSilent*/, m_dwClient))
		return false;
	if (m_Security.FixupRunActiveX(false/*bSilent*/, m_dwClient))
		return false;

	IOleWindowPtr spOleWin = spDoc;
	if (spOleWin == NULL)
	{
		log.LogString(LOG_ERROR, "FAILED, spOleWin == NULL");
		return false;
	}

	HWND hWndParent = NULL;
	if (FAILED(spOleWin->GetWindow(&hWndParent)))
	{
		CString szMsg;
		szMsg.Format(_T("FAILED, spOleWin->GetWindow(%X)"), hWndParent); 
		GetError(szMsg);
		return false;
	}

	if (!m_Security.IsUrlActionAllowed(hWndParent, URLACTION_ACTIVEX_RUN, DEFAULT_HOSTW))
	{
		DisplayMessage(IDS_SECURITY_ACTIVEX_FAIL);		
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// This function looks for all elements in a document and makes sure that
// all elements that contain user editable fields have JM attribute set.
// Also set text insertion to first user field.
BOOL CMsAddInBase::SetDocEditFields(IHTMLDocument2Ptr spDoc)
{		
	CFuncLog log(g_pLog, "CMsAddinBase::SetDocEditFields()");
	if (spDoc == NULL)
		return false;
	
	CComPtr<IHTMLElement> spElement;
	HRESULT hr = spDoc->get_body(&spElement);
	if (!spElement)
		return false;

	CComQIPtr<IHTMLBodyElement> spBody(spElement);
	if (!spBody)
		return false;
	
	CComPtr<IHTMLTxtRange> spTxtRange;
	hr = spBody->createTextRange(&spTxtRange);
	if (spTxtRange == NULL)
		return false;

	CComBSTR bsText("DoubleClickTo");
	VARIANT_BOOL bSuccess = VARIANT_TRUE;
	hr = spTxtRange->findText(bsText, 0, 0, &bSuccess);
	CComPtr<IHTMLTxtRange> spPrevTxtRange;
	long lActual;
	while (SUCCEEDED(hr) && bSuccess)
	{
		CComPtr<IHTMLElement> spElem;
		hr = spTxtRange->parentElement(&spElem);
		// Once we have the parent element that contains "DoubleClickTo" then
		// 1) Set JM Attribute.
		// 2) Move text range to start after current one.
		// 3) Look for another occurrence of "DoubleClickTo".
		if (spElem)
		{
			SetJMAttribute(spElem);
			hr = spTxtRange->select();					// Select found text
			hr = spTxtRange->collapse(VARIANT_FALSE);	// Move insertion point to end of selection
			hr = spTxtRange->moveEnd(CComBSTR("textedit"), 1, &lActual); // Move range end point to orig end
			hr = spTxtRange->findText(bsText, 0, 0, &bSuccess);	// look for next occurrence
		}
		else
			bSuccess = false;
	}

#ifdef _DEBUG
if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready in SetDocEditFields", g_szAppName, MB_OK);
#endif _DEBUG

	SetFocusToText(spDoc, "DoubleClickTo");

	// Remove canned message from stationery.
	ReplaceAllText(spDoc, MSG_STATIONERY_STRING, "", false /*bExpand*/, false/*bRemoveJMAttribute*/);

	ScrollIntoView(spDoc);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////
HRESULT CMsAddInBase::HandleDownloadRequest(VARIANT varProductId, VARIANT varName, VARIANT varDownload, VARIANT varFlags, VARIANT varWidth, VARIANT varHeight, VARIANT varFlashPath, VARIANT varPathPrice, VARIANT varExternalLink)
{
	CFuncLog log(g_pLog, "CMsAddinBase::HandleDownloadRequest()");
	
	// Check User status
	if (!VerifyUserStatus())
		return S_OK;

	m_HTMLMenu.Close();

	CMenu Menu;
	bool bOK = !!Menu.LoadMenu((LPCTSTR)MAKEINTRESOURCE(IDR_CONTENTMENU));
	if (!bOK)
		return S_OK;

	CMenu Popup(Menu.GetSubMenu(0));
	if (!Popup.IsMenu())
		return S_OK;

	// Get current user status and status of content.
	bool bPayingMember = m_Authenticate.IsUserPaidMember() || CONTROL;
	int iFlags = varFlags.intVal;
	bool bFreeContent = !(iFlags & CONTENT_PAY);
	bool bCanAccess = (bPayingMember || bFreeContent);
	int iCategory = HIBYTE(HIWORD(iFlags));

	// If the content is a smiley, remove the "Preview" menu item
	bool bSmiley = (iCategory == TYPE_SMILEYS);
	if (bSmiley)
		Popup.RemoveMenu(ID_CONTENT_PREVIEW, MF_BYCOMMAND);

	// If the content is not stationery, remove the "Set as Default" menu item
	bool bStationery = (iCategory == TYPE_STATIONERY);
	if (!bStationery /*|| !bCanAccess*/)
		Popup.RemoveMenu(ID_CONTENT_SETDEFAULT, MF_BYCOMMAND);

	if (bCanAccess)
		Popup.RemoveMenu(ID_OPTIONS_UPGRADENOW, MF_BYCOMMAND);
	else
	{
		//Popup.RemoveMenu(ID_CONTENT_ADDTOFAVORITES, MF_BYCOMMAND);
		//Popup.RemoveMenu(ID_CONTENT_REMOVEFROMFAVORITES, MF_BYCOMMAND);
	}

	// Future menu item to always launch a new compose message
	Popup.RemoveMenu(ID_CONTENT_COMPOSENEW, MF_BYCOMMAND);

	POINT point;
	::GetCursorPos(&point);
	point.x -= 5;
	point.y -= 5;
	HWND hWnd = ::WindowFromPoint(point);
	WORD wCommand = Popup.TrackPopupMenu(TPM_RETURNCMD|TPM_NONOTIFY|TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON, point.x, point.y, hWnd);
	Menu.DestroyMenu();
	::SetCursor(::LoadCursor(NULL, IDC_ARROW)); // Fix for IE 5.0 issues

	if (!wCommand)
		return S_OK;

	ExecuteDownload(wCommand, iCategory, bFreeContent, bPayingMember,
		varProductId.intVal, CString(varName.bstrVal), CString(varDownload.bstrVal), 
		varWidth.intVal, varHeight.intVal, CString(varFlashPath.bstrVal), 
		varPathPrice.intVal, CString(varExternalLink.bstrVal));

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::HandleOptionsButton()
{
	CFuncLog log(g_pLog, "CMsAddinBase::HandleOptionsButton()");

	return HandleBrowserOptionsButton(); // For now, Tim wants toolbar button to be the same as browser options.- jhc
}
//////////////////////////////////////////////////////////////////////////////
WORD CMsAddInBase::TrackOptionsMenu(WORD wDisableFlags)
{
	// If not registered, ignore button clicks and show Register prompt
	if (!VerifyUserStatus())
		return false;

	CMenu Menu;
	bool bOK = !!Menu.LoadMenu(IDR_OPTIONS_MENU2);
	if (!bOK)
		return false;

	CMenu Popup(Menu.GetSubMenu(0));
	if (!Popup.IsMenu())
		return false;

	
	bool bDisable = wDisableFlags & OPTIONS_DISABLE_CLEAR;
	WORD wMenuFlags = (bDisable ? MF_DISABLED | MF_GRAYED : MF_ENABLED);
	Popup.EnableMenuItem(ID_OPTIONS_CLEARSTATIONERY, MF_BYCOMMAND | wMenuFlags); 

	if (m_Authenticate.IsUserPaidMember())
		Popup.RemoveMenu(ID_OPTIONS_UPGRADENOW, MF_BYCOMMAND);


	POINT Point;
	::GetCursorPos(&Point);
	HWND hWnd = ::WindowFromPoint(Point);
	CRect rRect;
	::GetWindowRect(hWnd, rRect);
	WORD wCommand = Popup.TrackPopupMenu(TPM_RETURNCMD|TPM_NONOTIFY|TPM_LEFTALIGN|TPM_TOPALIGN|TPM_LEFTBUTTON, Point.x, Point.y, hWnd);
	Menu.DestroyMenu();

	return wCommand;

}
//////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::HandleGamesButton()
{
	CFuncLog log(g_pLog, "CMsAddinBase::HandleGamesButton()");

	// If not registered, ignore button clicks and show Register prompt
	if (!VerifyUserStatus())
		return false;

	CString szUrl;
	szUrl.LoadString(IDS_GAMES_URL);
	DWORD dwResult = (DWORD)::ShellExecute(NULL, "open", szUrl, NULL, NULL, SW_SHOWNORMAL);
	return true;
}

//////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::HandleECardsButton()
{
	CFuncLog log(g_pLog, "CMsAddinBase::HandleECardsButton()");

	// If not registered, ignore button clicks and show Register prompt
	if (!VerifyUserStatus())
		return false;
		
	m_ECardDlg.Display();

	return true;
}

//////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::HandleBrowserOptionsButton()
{
	CFuncLog log(g_pLog, "CMsAddinBase::HandleBrowserOptionsButton()");

	WORD wCommand = TrackOptionsMenu();
	
	if (!wCommand)
		return false;

	return HandleOptionsCommand(wCommand, SOURCE_TOOLBAR);
}

//////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::HandleOptionsCommand(WORD wCommand, int iSource)
{
	CFuncLog log(g_pLog, "CMsAddinBase::HandleOptionsCommand()");
	m_wLastCommand = wCommand;
	switch (wCommand)
	{
		case ID_OPTIONS_CHECKFORUPDATES:
		{
			//m_AutoUpdate.Start(false/*bSilent*/);
			break;
		}
		case ID_OPTIONS_SIGNIN:
		{
			m_Authenticate.ProcessRegistration(MODE_SIGNIN);
			break;
		}
		case ID_OPTIONS_MYACCOUNT:
		{
			ShowMyAccount();
			break;
		}
		case ID_OPTIONS_SHOWTRAYICON:
		{
			BOOL bShow = true;
			GetTrayIconStatus(bShow);
			bShow = !bShow;
			SetTrayIconStatus(bShow);
			ShowTrayIcon();
			break;
		}
		case ID_OPTIONS_GIVEFEEDBACK:
		{
			//LaunchFeedbackMail(wCommand); This is a Timonism.
			if (GetKeyState(VK_CONTROL) < 0)
				m_Usage.SendIfReady(true);
			else
				ShowFeedbackPage();
			break;
		}
		case ID_OPTIONS_UPGRADENOW:
		{
			ShowUpgradePage(iSource);
			break;
		}
		case ID_OPTIONS_JAZZYMAILHOME:
		{
			CString szUrl;
			m_Authenticate.ReadHost(szUrl);
			szUrl += DEFAULT_HOME;
			DWORD dwResult = (DWORD)::ShellExecute(NULL, "open", szUrl, NULL, NULL, SW_SHOWNORMAL);
			break;
		}
		case ID_OPTIONS_HELPCENTER:
		{
			ShowHelp();
			break;
		}
		case ID_OPTIONS_ABOUTJAZZYMAIL:
		{
			CAboutDlg dlgAbout(VER_PRODUCT_VERSION_STR);
			dlgAbout.DoModal();
			break;
		}
		case ID_OPTIONS_CLEARSTATIONERY:
		{
			ClearCurrentStationery();
			break;
		}
		case ID_OPTIONS_CLEARDEFAULTSTATIONERY:
		{
			ClearDefaultStationery();
			break;
		}
		case ID_OPTIONS_REFERAFRIEND:
		{
			ReferAFriend();
			break;
		}
		case ID_OPTIONS_OPTIMIZE:
		{
			/*COptimizeDlg dlgOptimize;
			dlgOptimize.DoModal();*/
			break;
		}
		case ID_FILE_SMARTCACHE:
		{
			m_SmartCache.UpdateSmartCache();
			break;
		}

		case ID_OPTIONS_SETTINGS:
		{
			if (::IsWindow(m_hJMSrvrUtilWnd) || ServerRegister(m_hClientWnd, m_dwClient))
				::PostMessage(m_hJMSrvrUtilWnd, UWM_OPTIONS, 0, 0);
			else
				DisplayMessage(IDS_OPTIONS_ERROR, MB_OK | MB_ICONSTOP);
			break;
		}
	}

	return true;
}
/////////////////////////////////////////////////////////////////////////////
void CMsAddInBase::HandleShutdown()
{
	CFuncLog log(g_pLog, "CMsAddinBase::HandleShutdown()");
	m_HTMLDialog.Close();
	m_HTMLMenu.Close();
	m_PreviewDialog.Close();

	m_Usage.Flush();

	m_Extend.Unload();

	m_spHotmailDoc = NULL;

	if (m_hMSAAInst)
		::FreeLibrary(m_hMSAAInst);
}
/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::ProcessExecuteCommand(int nCmdId)
{
	CFuncLog log(g_pLog, "CMsAddInBase::ExecuteCommand()");
	
	if (nCmdId == CMD_SHOW_OPTIONSMENU)
	{
		HandleBrowserOptionsButton();
	}
	else if (nCmdId == CMD_SHOW_ECARDMENU)
	{
		HandleECardsButton();
	}
	else if (nCmdId == CMD_SHOW_GAMESMENU)
	{
		HandleGamesButton();
	}

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::CanAccess()
{
	bool bCanAccess = (m_DownloadParms.bPayingMember || m_DownloadParms.bFreeContent);
	if (bCanAccess)
		return true;

	CString strMsg;
	strMsg.LoadString(IDS_UPGRADE_PROMPT);
	if (::MessageBox(m_HTMLDialog.m_hWnd, strMsg, g_szAppName, MB_YESNO) == IDYES)
	{
		int iPath = GetPath(m_DownloadParms.iPathPrice);
		ShowUpgradePage(SOURCE_PRODUCTSELECT, iPath, m_DownloadParms.iProductId);
	}
	
	return false;
}	

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::ShowTrayIcon()
{
	BOOL bShowIcon = TRUE;
	GetTrayIconStatus(bShowIcon);
	CComPtr<IJMBrkr> spBrkr;
	HRESULT hr = spBrkr.CoCreateInstance(CComBSTR("JMSrvr.JMBrkr"), NULL, CLSCTX_LOCAL_SERVER);
	if (FAILED(hr) || spBrkr == NULL)
		return false;

	hr = spBrkr->Visible((VARIANT_BOOL)bShowIcon);

	return SUCCEEDED(hr);
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::ExecuteDownload(WORD wCommand, int iCategory, bool bFreeContent, bool bPayingMember,
	int iProductId, CString& strName, CString& strDownload,
	int iWidth, int iHeight, CString& strFlashPath,
	int iPathPrice, CString& strExternalLink)
{
	// Stuff the download parms
	m_DownloadParms.iGroup = ASSET_GROUP_NORMAL;
	m_DownloadParms.iSource = SOURCE_PRODUCTSELECT;
	m_DownloadParms.iProductId = iProductId;
	m_DownloadParms.iPathPrice = iPathPrice;
	m_DownloadParms.iWidth = iWidth;
	m_DownloadParms.iHeight = iHeight;
	m_DownloadParms.iCategory = iCategory;
	m_DownloadParms.bFreeContent = bFreeContent;
	m_DownloadParms.bPayingMember = bPayingMember;
	m_DownloadParms.strFlashPath = strFlashPath;
	m_DownloadParms.strExternalLink = strExternalLink;
	m_DownloadParms.strCategory = "Category Placeholder";
	m_DownloadParms.strName = strName;
	Replace(m_DownloadParms.strName, "<br>", " ");
	m_DownloadParms.strURL = strDownload;
	m_DownloadParms.strSrc = "";
	m_DownloadParms.strDst = "";
	m_DownloadParms.strFile = "";

	// Compute the download type based on the extension
	CString strType = strDownload;
	strType.MakeLower();
	m_DownloadParms.Type = DT_Invalid;
	if (strType.Find(".htm") >= 0)
		m_DownloadParms.Type = DT_DownloadAndSetBackground;
	else
	if (strType.Find(".txt") >= 0)
		m_DownloadParms.Type = DT_DownloadAndEmbedContents;
	else
		m_DownloadParms.Type = DT_EmbedReference;

	HandleDownloadCommand(wCommand, NULL/*pOverrideDownloadParms*/);

	SaveDownloadInfo();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::HandleDownloadCommand(WORD wCommand, DownloadParms* pOverrideDownloadParms)
{
	CFuncLog log(g_pLog, "CMsAddinBase::HandleDownloadCommand()");

	if (pOverrideDownloadParms)
		m_DownloadParms = *pOverrideDownloadParms;

	m_DownloadParms.iGroup = ASSET_GROUP_NORMAL;

	m_wLastCommand = wCommand;
	switch (wCommand)
	{
		case ID_CONTENT_SEND:
		case ID_CONTENT_COMPOSENEW:
		{	
			/*if (m_dwClient == CLIENT_TYPE_IE && m_DownloadParms.iCategory ==  TYPE_ADDAPHOTO)
			{
				DisplayMessage(IDS_ADDAPHOTO_INFO_PROMPT, MB_OK);
				break;
			}*/
			if (!CanAccess())
				break;

			if (m_DownloadParms.Type == DT_DownloadAndSetBackground || m_DownloadParms.Type == DT_DownloadAndEmbedContents)
				DownloadFiles();
			else
			if (m_DownloadParms.Type == DT_EmbedReference)
				EmbedReference();
			break;
		}
		case ID_CONTENT_PREVIEW:
		{
			if (m_DownloadParms.Type == DT_EmbedReference)
			{
				CString strHTMLText = CreateReference();
				if (!strHTMLText.IsEmpty())
					ShowPreview(CString(""), strHTMLText);
			}
			else
			{
				log.LogString(LOG_INFO, "ID_CONTENT_PREVIEW Selected: DownloadFiles() requested");
				DownloadFiles();
				log.LogString(LOG_INFO, "ID_CONTENT_PREVIEW Selected: DownloadFiles() completed");
				
			}
			break;
		}
		/*case ID_CONTENT_ADDTOFAVORITES:
		{
			m_DownloadParms.iGroup = ASSET_GROUP_FAVORITES;
			DownloadFiles();
			break;
		}
		case ID_CONTENT_REMOVEFROMFAVORITES:
		{
			RemoveFromFavorites(m_DownloadParms.strCategory, m_DownloadParms.strName);
			break;
		}*/
		case ID_CONTENT_SETDEFAULT:
		{
			if (!CanAccess())
				break;

			m_DownloadParms.iGroup = ASSET_GROUP_DEFAULTSTA;
			DownloadFiles();
			break;
		}
		case ID_OPTIONS_UPGRADENOW:
		{
			int iPath = GetPath(m_DownloadParms.iPathPrice);
			ShowUpgradePage(m_DownloadParms.iSource, iPath, m_DownloadParms.iProductId);
			break;
		}
	}

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::SaveDownloadInfo()
{
	if (m_DownloadParms.iCategory < 0 || m_DownloadParms.iCategory >= MAX_CATS)
		return false;

	int iCategory = WebToPluginCatConvert[m_DownloadParms.iCategory];
	if (iCategory < 0 || iCategory >= LAST_CAT_USED)
		return false;

	if (m_dwClient == CLIENT_TYPE_IE)
		m_Authenticate.WriteDWORDStatus(REGVAL_LAST_IE_CAT, iCategory); 
	else if (m_dwClient == CLIENT_TYPE_EXPRESS)
		m_Authenticate.WriteDWORDStatus(REGVAL_LAST_OE_CAT, iCategory); 
	else if (m_dwClient == CLIENT_TYPE_OUTLOOK)
		m_Authenticate.WriteDWORDStatus(REGVAL_LAST_OL_CAT, iCategory); 
	else if (m_dwClient == CLIENT_TYPE_AOL)
		m_Authenticate.WriteDWORDStatus(REGVAL_LAST_AOL_CAT, iCategory); 

	return true;
}
/////////////////////////////////////////////////////////////////////////////
void CMsAddInBase::DownloadFiles()
{
	CFuncLog log(g_pLog, "CMsAddinBase::DownloadFiles()");

	// Create the main content folder and the sub folder
	CString strFolder;
	if (m_wLastCommand == ID_CONTENT_SETDEFAULT)
	{
		strFolder = m_StationeryMgr.GetPath();
	}
	else
	{
		TCHAR szTempPath[MAX_PATH];
		::GetTempPath(sizeof(szTempPath), szTempPath);
		strFolder = CString(szTempPath);
		strFolder += g_szAppName;
		strFolder += _T("\\");
		::CreateDirectory(strFolder, NULL);

		CString strName = m_DownloadParms.strName;
		CleanFileName(strName);

		if (!strName.IsEmpty())
		{
			strFolder += strName + '\\';
			::CreateDirectory(strFolder, NULL);
		}
	}

	// Extract the URL path from the download string, leaving the file list
	CString strDownload = m_DownloadParms.strURL;
	int index = max(strDownload.ReverseFind('/'), strDownload.ReverseFind('\\'));
	CString strURL = strDownload.Left(index+1);
	CString strFileList = strDownload.Mid(index+1) + ',';

	if (!m_Download.Init(1/*iFileType*/, DownloadCallback, (LPARAM)this))
		return;

	// Get each URL from the list
	bool bFirstFile = true;
	while ((index = strFileList.Find(',')) >= 0)
	{
		CString strFile = strFileList.Left(index);
		strFileList = strFileList.Mid(index+1);
		strFile.Trim();
		CString strSrcPath = strURL + strFile;
		CString strDstPath = strFolder + strFile;
		m_Download.AddFile(strSrcPath, strDstPath, NULL);
		if (bFirstFile)
		{
			m_DownloadParms.strSrc = strSrcPath;
			m_DownloadParms.strDst = strDstPath;
			m_DownloadParms.strFile = strFile;
			bFirstFile = false;
		}
	}

	m_Download.Start(true/*bGoOnline*/);
}
///////////////////////////////////////////////////////////////////////////////
void CMsAddInBase::AutoUpdateDone(DWORD dwResult)
{
	if (dwResult & UPDATE_RESOURCES)
	{
		m_Extend.Reload();
		CString szMsg;
		szMsg.LoadString(IDS_VERSIONUPDATED_PROMPT);
		::MessageBox(NULL, szMsg, g_szAppName, MB_OK);
	}
}
/////////////////////////////////////////////////////////////////////////////
bool CALLBACK  CMsAddInBase::DownloadCallback(LPARAM lParam, void* pDownloadVoid)
{
	CMsAddInBase* pAddin = (CMsAddInBase*)lParam;
	DOWNLOAD* pDownload = (DOWNLOAD*)pDownloadVoid;
	if (!pDownload) // The final callback when there are no more files to download
	{
		return pAddin->DownloadComplete();
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::DownloadComplete()
{
	CFuncLog log(g_pLog, "CMsAddinBase::DownloadComplete()");

	// If Creata Mail content was inserted then update product usage.
	// NOTE: this must be done prior to the LaunchMail() call
	// below.
	if (m_wLastCommand == ID_CONTENT_SEND || m_wLastCommand == ID_CONTENT_COMPOSENEW)
		m_Usage.Add(m_DownloadParms, 1);

	if (m_wLastCommand == ID_CONTENT_PREVIEW)
		ShowPreview(m_DownloadParms.strSrc, m_DownloadParms.strDst);
	else
	if (m_wLastCommand == ID_CONTENT_SETDEFAULT)
		DefaultStationery();
	else
	if (m_wLastCommand == ID_CONTENT_ADDTOFAVORITES)
		AddToFavorites(m_DownloadParms.strDst, m_DownloadParms.strCategory, m_DownloadParms.strName);
	else
	if (m_DownloadParms.Type == DT_DownloadAndSetBackground)
	{
		bool bAllowReuse = (m_wLastCommand != ID_CONTENT_COMPOSENEW);
		CString strSubject;
		if (m_DownloadParms.iCategory == TYPE_REFERAFRIEND)
			strSubject = m_DownloadParms.strName;
		LaunchMail(bAllowReuse, m_DownloadParms.strSrc, m_DownloadParms.strDst, strSubject);
	}
	else
	if (m_DownloadParms.Type == DT_DownloadAndEmbedContents)
		EmbedHTMLFile(m_DownloadParms.strDst);

	// Most likely online, so check if its time to send usage data.
	m_Usage.SendIfReady();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::SetFlashPlayParam(IHTMLDocument2Ptr spDoc)
{
	// Do only for non wsShell types.
	if (m_FlashManager.IsWsShellUsed())
		return false;
	
	if (spDoc == NULL)
		return false;
	
	// Get HTML body
	IHTMLElementPtr spElement = NULL;
	spDoc->get_body(&spElement);
	if (spElement == NULL)
		return false;

	CComBSTR bstrBody;
	HRESULT hr = spElement->get_innerHTML(&bstrBody);
	if (FAILED(hr))
		return false;

	// Replace all occurrences of Play Param in HTML body
	CString strBody(bstrBody);
	if (Replace(strBody, "name=\"Play\" value=\"0\"", "name=\"Play\" value=\"-1\""))
		hr = spElement->put_innerHTML(CComBSTR(strBody));

#ifdef _DEBUG
if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready in SetFlashPlayParam", g_szAppName, MB_OK);
#endif _DEBUG

	return SUCCEEDED(hr);
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::ResetContentEditable(IHTMLDocument2 *pDoc)
{
	CComPtr<IHTMLDocument2> spDoc(pDoc);
	if (!spDoc)
		return false;
	
	// Get HTML body
	IHTMLElementPtr spElement = NULL;
	spDoc->get_body(&spElement);
	if (spElement == NULL)
		return false;

	CComBSTR bstrBody;
	HRESULT hr = spElement->get_innerHTML(&bstrBody);
	if (FAILED(hr))
		return false;

	// Remove all occurrences of "contentEditable=true"
	CString strBody(bstrBody);
	if (Replace(strBody, "contenteditable=true", ""))
		hr = spElement->put_innerHTML(CComBSTR(strBody));
	if (Replace(strBody, "contenteditable=false", ""))
		hr = spElement->put_innerHTML(CComBSTR(strBody));
//	if (Replace(strBody, "unselectable=\"on\"", ""))
//		hr = spElement->put_innerHTML(CComBSTR(strBody));
//	if (Replace(strBody, "unselectable=on", ""))
//		hr = spElement->put_innerHTML(CComBSTR(strBody));

#ifdef _DEBUG
if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready in ResetContentEditable", g_szAppName, MB_OK);
#endif _DEBUG

	return SUCCEEDED(hr);
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::HandleOnItemClose()
{
	CFuncLog log(g_pLog, "CMsAddinBase::HandleOnItemClose()");
	m_Usage.Flush();
	return true;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMsAddInBase::HandleOnItemSend(IHTMLDocument2Ptr spDoc)
{
	CFuncLog log(g_pLog, "CMsAddinBase::HandleOnItemSend()");
	
	if (spDoc == NULL)
		return false;

	// Process usage data.
	DWORD dwTotalSends = 0;
	m_Usage.GetSendCount(spDoc, dwTotalSends);
	m_Usage.SendIfReady();
	m_Usage.Flush();

	// Ensure this is a Creata Mail doc
	if (!IsJazzyMail(spDoc))
		return false;

	// Remove Creata Mail attribute from body tag
	RemoveJazzyMailAttribute(spDoc);

	// Ensure any contentEditable=true attributes are removed
	ResetContentEditable(spDoc);

	// If no Creata Mail content is included in message, then we're done
	if (!dwTotalSends)
		return false;

	// Set Play Param for all non-wsShell flash types.
	SetFlashPlayParam(spDoc);

	// Add promo to end of e-mail message for non-paying members
	// or members that heve elected to leave it on
	bool bOK = IsDocLoadedWait(spDoc);
	bool bPayingMember = m_Authenticate.IsUserPaidMember();
	bool bPromoFooterOn = m_Authenticate.IsPromoFooterOn();
	bool bPromoAdded = !bPayingMember || bPromoFooterOn;
	if (bPromoAdded)
	{
		CString strHTMLText;
		if (m_Extend.GetString(MAKEINTRESOURCE(IDH_TRAILER), RT_HTML, strHTMLText))
			EmbedHTMLAtEnd(strHTMLText);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::EmbedHTMLAtEndBase(IHTMLDocument2Ptr spDoc, const CString& strHTMLText)
{
	CFuncLog log(g_pLog, "CMsAddInBase::EmbedHTMLAtEnd()");

	if (spDoc == NULL)
		return false;

	IHTMLElementPtr spBodyElement;
	HRESULT hr = spDoc->get_body(&spBodyElement);
	if (spBodyElement == NULL)
		return false;

#ifdef NOTUSED //j
	// See if the body has a nested div tag, and if so, use it
	IHTMLElementPtr spElement = spBodyElement;
	IDispatchPtr spDisp;
	hr = spBodyElement->get_all(&spDisp);
	IHTMLElementCollectionPtr spAll(spDisp);
	if (spAll)
	{
		long nCnt = 0;
		hr = spAll->get_length(&nCnt);
		if (nCnt > 0)
		{
			IDispatchPtr spTagDisp;
			hr = spAll->item(CComVariant(0), CComVariant(0), &spTagDisp);
			IHTMLElementPtr spElem(spTagDisp);
			if (spElem)
			{
				CComBSTR bsTagName;
				hr = spElem->get_tagName(&bsTagName);
				CString szTagName(bsTagName);
				if (!szTagName.IsEmpty()) 
				{
					if (!szTagName.CompareNoCase("div"))
					{
						CComPtr<IHTMLStyle> spStyle;
						hr = spElem->get_style(&spStyle);
						if (spStyle)
						{
							hr = spStyle->put_marginBottom(CComVariant("40px"));
						}
					}
				}
			}
		}
	}
#endif NOTUSED //j

	// Append HTML to end of the body
	hr = spBodyElement->insertAdjacentHTML(CComBSTR("beforeEnd"), CComBSTR(strHTMLText));

#ifdef _DEBUG
	if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready in EmbedHTMLAtEndBase", g_szAppName, MB_OK);
#endif _DEBUG

	return SUCCEEDED(hr);
}

/////////////////////////////////////////////////////////////////////////////
CString CMsAddInBase::PrepareHTML(CString& strHTMLBody, const CString& strDownloadSrc, bool bPreview)
{
	CFuncLog log(g_pLog, "CMsAddinBase::PrepareHTML()");

	// Replace line endings and white space to make global text replacement possible
	Replace(strHTMLBody, "\n", "");
	Replace(strHTMLBody, "\r", "");
	Replace(strHTMLBody, "\t", " ");

	if (!strDownloadSrc.IsEmpty())
	{
		int index = max(strDownloadSrc.ReverseFind('/'), strDownloadSrc.ReverseFind('\\'));
		CString strContentPath = strDownloadSrc.Left(index + 1);
		if (!strContentPath.IsEmpty())
		{
			// Replace the image names with the full path
			Replace(strHTMLBody, "background-image:url(", "background-image:url(" + strContentPath);
			Replace(strHTMLBody, "background=\"", "background=\"" + strContentPath);
			Replace(strHTMLBody, "src=\"", "src=\"" + strContentPath);
			Replace(strHTMLBody, "\"Movie\" value=\"", "\"Movie\" value=\"" + strContentPath);
			Replace(strHTMLBody, "\"Src\" value=\"", "\"Src\" value=\"" + strContentPath);
		}
	}

	// Fixup the body tags for ecards
	int iStart = strHTMLBody.Find("<body");
	if (iStart < 0)
		iStart = strHTMLBody.Find("<BODY");
	if (iStart >= 0)
	{
		int iEnd = strHTMLBody.Find(">", iStart);
		if (iEnd > iStart)
		{
			int iCount = iEnd - iStart + 1;
			CString strTagLower = strHTMLBody.Mid(iStart, iCount);
			strTagLower.MakeLower();
			if (strTagLower.Find("color") >= 0 || strTagLower.Find("background") >= 0)
			{
				Replace(strHTMLBody, "<body", "<body style=\"margin:0px;\"><table cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" height=\"100%\"><tr><td");
				Replace(strHTMLBody, "</body", "</td></tr></table></body");
			}
		}
	}

	// Fixup the internal links for ecards
//j	Replace(strHTMLBody, "<a", "<span contenteditable=\"false\"><a");
//j	Replace(strHTMLBody, "</a", "</a></span");
//j	Replace(strHTMLBody, "<span contentEditable=\"false\"><a name", "<span><a name");

	// Replace the default stationery message
	CString strInsertMarker = "<span ";
	strInsertMarker += JMINSERT_ATTRIBUTE;
	strInsertMarker += "=\"1\"></span>";
	CString strMessage1;
	CString strMessage2;
	CString strMessage3;
	if (!bPreview)
	{
		strMessage1 = "<span unselectable=\"on\" >&shy;</span>" + strInsertMarker; //j This helps, but maybe we can avoid doing this
		strMessage2 = MSG_STATIONERY_STRING;
		strMessage2 +=  strInsertMarker;
		strMessage3 = "[msg]" + strInsertMarker;
	}

	Replace(strHTMLBody, "[message]", strMessage1);
	Replace(strHTMLBody, "[textmessage]", strMessage2);
	Replace(strHTMLBody, "Text body block", strMessage2);
	Replace(strHTMLBody, "[msg]", strMessage3);

	// Replace an erroneous stationery font size - Temporary
	Replace(strHTMLBody, "font-size:12px", "font-size:10pt");

	// Remove this obsolete stationery message - Temporary
	if (Replace(strHTMLBody, "Hi,<br>Your personal", strMessage2))
	{
 		Replace(strHTMLBody, "message will appear here. Write as much as you'd like --", "");
		Replace(strHTMLBody, "your message space is unlimited.", "");
	}

	// Remove this unwanted ecard message
	if (Replace(strHTMLBody, "A card created for [NameFirstTo]", ""))
		Replace(strHTMLBody, "by [NameFirstFrom]", "");

	// Replace the ecard form fields
	if (strHTMLBody.Find("[") >= 0)
	{
		int iCount = sizeof(FieldArray) / sizeof(LPCSTR);
		CString strPrefix = CString("DoubleClickToAddThe");
		CString strSuffix = ".";
		for (int i = 0; i < iCount; i += 2)
		{
			CString strReplace = FieldArray[i];
			if (!bPreview && !strReplace.IsEmpty())
			{
				Replace(strReplace, " ", "");
				strReplace = strPrefix + strReplace + strSuffix;
			}
			CString strFind = CString("[") + FieldArray[i+1] + CString("]");
			Replace(strHTMLBody, strFind, strReplace);
		}
	}

	if (strHTMLBody.Find(CString("strExternalLink1")) >= 0)
		Replace(strHTMLBody, "strExternalLink1", m_DownloadParms.strExternalLink);

	if (strHTMLBody.Find(CString("strFlashHTML")) < 0)
		return strHTMLBody;

	int iWidth = m_DownloadParms.iWidth;
	if (iWidth <= 0) iWidth = 550;
	int iHeight = m_DownloadParms.iHeight;
	if (iHeight <= 0) iHeight = 320;
	CString strWidth;
	strWidth.Format("width='%d' ", iWidth);
	CString strHeight;
	strHeight.Format("height='%d' ", iHeight);

	CString strHTMLText = CreateFlashTag(m_DownloadParms.strFlashPath, strWidth, strHeight, false/*bAddJMAttribute*/);
	strHTMLBody.Replace("strFlashHTML", CString(strHTMLText) + "<br>");

	return strHTMLBody;
}

/////////////////////////////////////////////////////////////////////////////
CString CMsAddInBase::CreateFlashTag(const CString& strURLIn, const CString& strWidth, const CString& strHeight, bool bAddJMAttribute) 
{
	CFuncLog log(g_pLog, "CMsAddinBase::CreateFlashTag()");
	CString strURL = strURLIn;
	if (strURL.Find("animPath") >= 0)
	{
		strURL += "&NameFirstFrom=sender";
		strURL += "&NameFirstTo=recipient";
		strURL += "&MemoEmailFrom=nb%40nb.com";
		strURL += "&EmailTo=nb%40nb.com";
		strURL += "&extraVar1=";
		strURL += "&extraVar2=";
		strURL += "&extraVar3=";
		strURL += "&IMGSRVR=";
		strURL += "&MultiLineTextOptional=";
	}

	//j contentEditable="false": makes it so the FlashManager doesn't select it for editing and screw up the Format bar
	//j unselectable="on": not sure of the impact of this attribute
	CString strHTMLText = "<span contentEditable=false unselectable=\"on\">";
	strHTMLText += "<object ";
	if (bAddJMAttribute)
		strHTMLText += CString(JM_ATTRIBUTE) + "=\"1\" ";
	strHTMLText += "id=\"flash\" ";
	strHTMLText += "unselectable=\"on\" ";
	strHTMLText += "codebase=\"http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,40,0\" ";
	strHTMLText += strWidth;
	strHTMLText += strHeight;
	strHTMLText += "align=\"absmiddle\" ";
	strHTMLText += "classid=\"clsid:D27CDB6E-AE6D-11cf-96B8-444553540000\">";
	strHTMLText += "<param name=\"Movie\" value=\"" + strURL + "\">";
	strHTMLText += "<param name=\"Quality\" value=\"High\">";
	strHTMLText += "<param name=\"BGColor\" value=\"#FFFFFF\">";
	strHTMLText += "<param name=\"Play\" value=\"-1\">";
	strHTMLText += "<param name=\"Loop\" value=\"0\">";
	strHTMLText += "<param name=\"Menu\" value=\"-1\">";
//	strHTMLText += "<param name=\"Autostart\" value=\"-1\">";
//	strHTMLText += "<param name=\"WMode\" value=\"Opaque\">"; //Window, Transparent
//	strHTMLText += "<param name=\"EmbedMovie\" value=\"true\">";
//	strHTMLText += "<param name=\"PercentLoaded\" value=\"true\">";
//	strHTMLText += "<param name=\"LoadMovie\" value=\"true\">";
//	strHTMLText += "<param name=\"SWRemote\" value=\"swPicon='http://directory/picon.bmp' swSaveEnabled='true' swVolume='true' swRestart='true' swPausePlay='true' swFastForward='true' swContextMenu='true' \">";
//	strHTMLText += "<param name=\"Align\" value=\"\">";
//	strHTMLText += "<param name=\"SAlign\" value=\"\">";
//	strHTMLText += "<param name=\"Scale\" value=\"ShowAll\">";
//	strHTMLText += "<param name=\"Base\" value=\"\">";
//	strHTMLText += "<param name=\"DeviceFont\" value=\"0\">";
//	strHTMLText += "<param name=\"_cx\" value=\"14552\">";
//	strHTMLText += "<param name=\"_cy\" value=\"10583\">";
//	strHTMLText += "<param name=\"FlashVars\" value=\"14552\">";

	strHTMLText += "<embed src=\"" + strURL + "\" ";
	strHTMLText += "quality=\"high\" ";
	strHTMLText += "bgcolor=\"#FFFFFF\" ";
	strHTMLText += "width=\"" + strWidth + "\" ";
	strHTMLText += "height=\"" + strHeight + "\" ";
	strHTMLText += "name=\"flash\" ";
//	strHTMLText += "=\"align\" ";
	strHTMLText += "type=\"application/x-shockwave-flash\" ";
	strHTMLText += "pluginspage=\"http://www.macromedia.com/go/getflashplayer\">";
	strHTMLText += "</embed>";

	strHTMLText += "</object>";
	strHTMLText += "</span>";

	return strHTMLText;
}

/////////////////////////////////////////////////////////////////////////////////////
CString CMsAddInBase::CreateReference()
{
	CFuncLog log(g_pLog, "CMsAddinBase::CreateReference()");
	int iWidth = m_DownloadParms.iWidth;
	if (iWidth <= 0) iWidth = 50;

	int iHeight = m_DownloadParms.iHeight;
	if (iHeight <= 0) iHeight = 50;

	CString strWidth;
	strWidth.Format("width='%d' ", iWidth);
	CString strHeight;
	strHeight.Format("height='%d' ", iHeight);

	CString strURL = m_DownloadParms.strURL;
	CString strType = strURL;
	strType.MakeLower();

	CString strHTMLText;
	if (strType.Find(".gif") >= 0 || strType.Find(".jpg") >= 0)
	{
		strHTMLText += "<img ";
		strHTMLText += JM_ATTRIBUTE;
		strHTMLText += "=\"1\" src=\"";
		strHTMLText += strURL;
		strHTMLText += "\" ";
		strHTMLText += strWidth;
		strHTMLText += strHeight;
		strHTMLText += "align=\"absmiddle\" ";
		strHTMLText += "border=\"0\" ";
		strHTMLText += "alt=\"\">";
	}
	else
	if (strType.Find(".swf") >= 0)
		strHTMLText += CreateFlashTag(strURL, strWidth, strHeight, true/*bAddJMAttribute*/);

	return strHTMLText;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::EmbedReference()
{
	CFuncLog log(g_pLog, "CMsAddinBase::EmbedReference()");
	CString strHTMLText = CreateReference();
	if (strHTMLText.IsEmpty())
		return false;

	// If Creata Mail content was inserted then update product usage. 
	// NOTE: this must be done prior to the EmbedHTMLAtSelection() call
	// below.
	if (m_wLastCommand == ID_CONTENT_SEND || m_wLastCommand == ID_CONTENT_COMPOSENEW)
		m_Usage.Add(m_DownloadParms, 1);

	bool bAllowReuse = (m_wLastCommand != ID_CONTENT_COMPOSENEW);
	bool bOK = false;
	if (bAllowReuse)
		bOK = EmbedHTMLAtSelection(strHTMLText);

	if (!bOK)
		bOK = LaunchMail(false, "", strHTMLText, "");

	if (!bOK)
		return false;

	// Most likely online, so check if its time to send usage data.
	m_Usage.SendIfReady();
	
	return bOK;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::EmbedHTMLFile(const CString& strHTMLFileName)
{
	CFuncLog log(g_pLog, "CMsAddinBase::EmbedHTMLFile()");
	CString strHTMLText = ReadHTMLFile(strHTMLFileName);
	if (strHTMLText.IsEmpty())
		return false;

	bool bAllowReuse = (m_wLastCommand != ID_CONTENT_COMPOSENEW);
	bool bOK = false;
	if (bAllowReuse)
		bOK = EmbedHTMLAtSelection(strHTMLText);
	if (!bOK)
		bOK = LaunchMail(bAllowReuse, "", strHTMLText, "");
	return bOK;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::EmbedHTMLAtSelectionBase(const CString& strHTMLText, IHTMLDocument2Ptr spDoc)
{
	CFuncLog log(g_pLog, "CMsAddinBase::EmbedHTMLAtSelectionBase()");
	if (spDoc == NULL)
		return false;

	if (!IsDocInEditMode(spDoc))
		return false;

	// Check Security policies
	if (!VerifySecurity(spDoc, strHTMLText))
		return true; // suppress launch mail.

	// Identify non Creata Mail text to preserve, such as signatures
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
			bNeedNewTxtRange = (szOuter.Find("<body") >= 0);
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
	if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready in EmbedHTMLAtSelectionBase - part 1", g_szAppName, MB_OK);
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
		if (!bSuccess)
			bSuccess = SetHtmlText(spDoc, strHTMLText);

	}

	// If embed has failed at this point there is not need to proceed.
	if (!bSuccess)
		return false;

#ifdef _DEBUG
	if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready in EmbedHTMLAtSelectionBase - part 2", g_szAppName, MB_OK);
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
	if (spParentElem)
		spParentElem->scrollIntoView(CComVariant(true));

	ActivateJazzyMailDocument(spDoc, true/*bSetAttribute*/);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CMsAddInBase::ActivateJazzyMailDocument(IHTMLDocument2Ptr spDoc, bool bSetAttribute)
{
	CFuncLog log(g_pLog, "CMsAddinBase::ActivateJazzyMailDocument()");
	if (spDoc == NULL)
		return;

	// Refresh usage meta.
	m_Usage.RefreshMetaData(spDoc);

	bool bIsJazzyMail = IsJazzyMail(spDoc);

	// If requested, tag this document with the Creata Mail attribute
	if (bSetAttribute && !bIsJazzyMail)
	{
		SetJazzyMailAttribute(spDoc);
		bIsJazzyMail = true;
	}

	// If this is one of ours, setup document handling
	if (bIsJazzyMail)
		TurnOnDOMHandling(spDoc, true);

	ActivateDocumentWindow(spDoc);
}

/////////////////////////////////////////////////////////////////////////////
void CMsAddInBase::DeactivateJazzyMailDocument(IHTMLDocument2Ptr spDoc)
{
	CFuncLog log(g_pLog, "CMsAddinBase::DeactivateJazzyMailDocument()");
	//Shutdown document handling
	TurnOnDOMHandling(spDoc, false);
}

/////////////////////////////////////////////////////////////////////////////
void CMsAddInBase::ActivateDocumentWindow(IHTMLDocument2Ptr spDoc)
{
	CFuncLog log(g_pLog, "CMsAddinBase::ActivateDocumentWindow()");
	if (spDoc == NULL)
		return;

	IOleWindowPtr spOleWin = spDoc;
	if (spOleWin == NULL)
		return;

	HWND hWndParent = NULL;
	if (FAILED(spOleWin->GetWindow(&hWndParent)))
		return;

	// Walk up the parent chain to find a window with a caption; this should be the app
	while (hWndParent)
	{
		DWORD dwStyle = ::GetWindowLong(hWndParent, GWL_STYLE);
		bool bIsApp = ((dwStyle & WS_CAPTION) == WS_CAPTION);
		if (bIsApp)
			break;

		hWndParent = ::GetParent(hWndParent);
	}
	
	if (!hWndParent)
		return;

	::BringWindowToTop(hWndParent);
	::ShowWindow(hWndParent, ::IsIconic(hWndParent) ? SW_RESTORE : SW_SHOW);
}

/////////////////////////////////////////////////////////////////////////////
void CMsAddInBase::DocLoadStartTimer(LPTIMECALLBACK pTimerProc, DWORD dwData1, DWORD dwData2, DWORD dwData3)
{
	CFuncLog log(g_pLog, "CMsAddinBase::DocLoadStartTimer()");
	CTimer* pTimer = new CTimer();
	if (!pTimer)
		return;

	UINT msInterval = 500;
	int iCountdown = (30 /*seconds*/ * 1000) / msInterval;
	pTimer->Start(msInterval, false/*bOneShot*/, pTimerProc, iCountdown, dwData1, dwData2, dwData3);
}
	
//////////////////////////////////////////////////////////////////////////////
BOOL CMsAddInBase::DefaultStationery()
{
	CFuncLog log(g_pLog, "CMsAddinBase::DefaultStationery()");
		
	// Update default stationery status.
	DWORD dwStatus=0;
	m_Authenticate.ReadDWORDStatus(REGVAL_DEFAULTSTA, dwStatus);
	if (!(dwStatus & m_dwClient))
	{
		dwStatus |= m_dwClient;
		m_Authenticate.WriteDWORDStatus(REGVAL_DEFAULTSTA, dwStatus);
	}

	m_StationeryMgr.SetDefault(m_DownloadParms);
	// Used to update active message with new stationery. 
	// Tim is unsure if he wants to this so comment out for now - JHC
	//m_DownloadParms.iCategory = TYPE_STATIONERY;
	//LaunchMail(true/*bAllowReuse*/, "", m_StationeryMgr.GetHTML(), ""); 
	return true;
}

//////////////////////////////////////////////////////////////////////////////
BOOL CMsAddInBase::ClearCurrentStationery()
{
	CFuncLog log(g_pLog, "CMsAddinBase::ClearCurrentStationery()");
	m_DownloadParms.iCategory = TYPE_STATIONERY;
	LaunchMail(true/*bAllowReuse*/, "", "", "");
	return true;
}

//////////////////////////////////////////////////////////////////////////////
BOOL CMsAddInBase::ClearDefaultStationery()
{
	CFuncLog log(g_pLog, "CMsAddinBase::ClearDefaultStationery()");
	m_StationeryMgr.ClearDefault();
	//m_DownloadParms.iCategory = TYPE_STATIONERY;
	//LaunchMail(true/*bAllowReuse*/, "", "", ""); //For now, Tim does not want clear current stationery
	return true;
}

//////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::ReferAFriend()
{
	CFuncLog log(g_pLog, "CMsAddinBase::ReferAFriend()");

	CString strUrl;
	m_Authenticate.ReadHost(strUrl);
	int iDot = strUrl.Find(".");
	strUrl = (iDot >= 0 ? strUrl.Left(iDot) : "http://www");
	strUrl += ".imgag.com/product/full/ap/3066668/index.html";

	return ExecuteDownload(ID_CONTENT_COMPOSENEW, TYPE_REFERAFRIEND, true/*bFreeContent*/, true/*bPayingMember*/,
		3066668/*iProductId*/, CString("Have you heard about Creata Mail?")/*strName*/,
		strUrl/*strDownload*/,
		0/*iWidth*/, 0/*iHeight*/, CString("")/*strFlashPath*/,
		45954048/*iPathPrice*/, CString("")/*strExternalLink*/);
}

//////////////////////////////////////////////////////////////////////////////
BOOL CMsAddInBase::AddToFavorites(const CString& strSrcPath, const CString& strCategory, const CString& strNameIn)
{
	CString strName = strNameIn;
	CleanFileName(strName);

	if (strCategory.IsEmpty() || strName.IsEmpty())
		return FALSE;

	// Get Favorites folder. If it doesn't exist it is created
	CString strDstPath;
	if (!GetFavoritesPath(strDstPath))
		return FALSE;

	if (!strCategory.IsEmpty())
	{
		strDstPath += _T("\\") + strCategory;
		if (!CreateDir(strDstPath, NULL))
			return FALSE;
	}
	
	if (!strName.IsEmpty())
	{
		strDstPath += _T("\\") + strName;
		if (!CreateDir(strDstPath, NULL))
			return FALSE;
	}

	// Copy favorite from download folder to favorite folder.
	strDstPath += _T("\\") + m_DownloadParms.strFile;
	if (!::CopyFile(strSrcPath, strDstPath, FALSE))
	{
		GetError("::CopyFile(strSrcPath, strDstPath, FALSE)");
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
BOOL CMsAddInBase::RemoveFromFavorites(const CString& strCategory, const CString& strNameIn)
{
	CString strName = strNameIn;
	CleanFileName(strName);

	if (strCategory.IsEmpty() || strName.IsEmpty())
		return FALSE;

	// Get Favorites folder. If it doesn't exist it is created
	CString strFavPath;
	if (!GetFavoritesPath(strFavPath))
		return FALSE;
	
	CString strCategoryPath;
	if (!strCategory.IsEmpty())
	{
		strFavPath += _T("\\") + strCategory;
		strCategoryPath = strFavPath;
	}
		
	if (!strName.IsEmpty())
		strFavPath += _T("\\") + strName;

	// Delete all html files in favorite folder
	WIN32_FIND_DATA FileData;
	HANDLE hFind;
	CString strFindPath = strFavPath;
	strFindPath +=  _T("\\*.htm?");
	hFind = FindFirstFile(strFindPath, &FileData); 
	if (hFind == INVALID_HANDLE_VALUE) 
	{ 
		GetError("FindFirstFile(strFavPath, &FileData)");
	} 
	else
	{
		CString strFilePath;
		bool bDone = false;
		while (!bDone)
		{
			strFilePath = strFavPath;
			strFilePath += _T("\\");
			strFilePath += FileData.cFileName;
			if (!DeleteFile(strFilePath))
				GetError("DeleteFile(strFilePath)");

			if (!FindNextFile(hFind, &FileData)) 
			{
				if (GetLastError() != ERROR_NO_MORE_FILES) 
					GetError("FindNextFile(hFind, &FileData)");

				bDone = true; 
			}
		}
	}

	FindClose(hFind);

	// Delete folder for this favorite.
	if (!RemoveDirectory(strFavPath))
			GetError("RemoveDirectory(strFavPath)");

	// Category folder is removed only if it is empty
	if (!RemoveDirectory(strCategoryPath))
	{
		if (GetLastError() != ERROR_DIR_NOT_EMPTY) 
			GetError("RemoveDirectory(strCategoryPath)");
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
BOOL CMsAddInBase::ShowPreview(const CString& strDownloadSrc, const CString& strDownloadDst)
{
	CFuncLog log(g_pLog, "CMsAddinBase::ShowPreview()");
	CString strHTMLText;
	if (strDownloadSrc.IsEmpty())
		strHTMLText = strDownloadDst;
	else
		strHTMLText = ReadHTML(strDownloadSrc, strDownloadDst, true/*bPreview*/);

	if (strHTMLText.IsEmpty())
		return FALSE;

	if (!m_PreviewDialog.Display(NULL, m_DownloadParms))
		return FALSE;

	// Add html to the Preview.
	if (!m_PreviewDialog.AddHtmlToPreview(strHTMLText))
		return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::TurnOnDOMHandling(IHTMLDocument2Ptr spDoc, bool bOn)
{
	CFuncLog log(g_pLog, "CMsAddinBase::TurnOnDOMHandling()");

	CString strHost;
	m_Authenticate.ReadHost(strHost);

	// spDoc could be NULL if the document is being closed
	if (bOn)
	{
		m_FlashManager.Activate(spDoc, true);
		m_PhotoManager.Activate(spDoc, true, strHost);

	}
	else
	{
		m_FlashManager.Activate(spDoc, false);
		m_PhotoManager.Activate(spDoc, false, strHost);
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::IsJazzyMail(IHTMLDocument2Ptr spDoc)
{
	if (spDoc == NULL)
		return false;

	IHTMLElementPtr spElement;
	HRESULT hr = spDoc->get_body(&spElement);
	if (spElement == NULL)
		return false;

	CComVariant varValue; 
	varValue.vt = VT_NULL;
	hr = spElement->getAttribute(CComBSTR(JAZZYMAIL_ATTRIBUTE), 1, &varValue);
	if (FAILED(hr))
		return false;

	// Test for the mere presence of an attribute value
	bool bOn = (varValue.vt != VT_NULL);
	return bOn;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMsAddInBase::GetFavoritesPath(CString& strFavoritesPath)
{
	// Create folder for favorite.
	TCHAR szTempPath[MAX_PATH];
	::GetTempPath(sizeof(szTempPath), szTempPath);
	CString strDstPath = CString(szTempPath) + g_szAppName;
	
	if (!CreateDir(strDstPath, NULL))
		return FALSE;
	
	strDstPath += _T("\\Favorites");
	if (!CreateDir(strDstPath, NULL))
		return FALSE;

	strFavoritesPath = strDstPath;

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// The purpose of this function is to mark text in NON-CREATA MAIL documents
// for preservation. For example, if a smiley is inserted in a non Creata Mail
// message any pre-existing text (such as signatures) needs to be marked
// for preservation. So that if stationery, ecards... are subsequently inserted
// this text will be preserved.
BOOL CMsAddInBase::MarkNonJazzyMailTextForPreservation(IHTMLDocument2Ptr spDoc)
{	
	CFuncLog log(g_pLog, "CMsAddinBase::MarkNonJazzyMailTextForPreservation()");

	if (spDoc == NULL)
		return false;

	if (!IsDocInEditMode(spDoc))
		return false;

	if (IsJazzyMail(spDoc))
		return false;

	CComPtr<IHTMLElement> spElement;
	HRESULT hr = spDoc->get_body(&spElement);
	if (FAILED(hr) || spElement == NULL)
		return false;

	// Determine if body should be marked.
	Mark(spElement);

#ifdef _DEBUG
if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready - MarkNonJazzyMailTextForPreservation", g_szAppName, MB_OK);
#endif _DEBUG

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::Mark(IHTMLElement * pElem)
{
	CFuncLog log(g_pLog, "CMsAddinBase::Mark()");

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

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::IsRegistrationdNeeded()
{
	CFuncLog log(g_pLog, "CMsAddinBase::IsRegistrationdNeeded()");
	DWORD dwFail = 0;
	return !m_Authenticate.IsRegistered(dwFail);
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::IsAuthenticateNeeded()
{
	CFuncLog log(g_pLog, "CMsAddinBase::IsAuthenticateNeeded()");
	return m_Authenticate.IsExpired();
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::VerifyUserStatus()
{
	CFuncLog log(g_pLog, "CMsAddinBase::VerifyUserStatus()");
	if (IsRegistrationdNeeded())
	{
		m_Authenticate.ProcessRegistration(MODE_REGPROMPT);
		return false;
	}
	else
	if (IsAuthenticateNeeded())
	{
		m_Authenticate.ProcessRegistration(MODE_SILENT);
		return true;
	}
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
void CMsAddInBase::ShowMyAccount()
{		
	CFuncLog log(g_pLog, "CMsAddinBase::ShowMyAccount()");
	CString szUrl;
	CString szMyAccount;
	m_Authenticate.ReadMyAccountPage(szMyAccount);
	szUrl = szMyAccount;
	DWORD dwResult = (DWORD)::ShellExecute(NULL, "open", szUrl, NULL, NULL, SW_SHOWNORMAL);
}

///////////////////////////////////////////////////////////////////////////////
void CMsAddInBase::ShowFeedbackPage()
{		
	CFuncLog log(g_pLog, "CMsAddinBase::ShowFeedbackPage()");
	CString szUrl;
	m_Authenticate.ReadHost(szUrl);
	szUrl += DEFAULT_FEEDBACK;
	DWORD dwResult = (DWORD)::ShellExecute(NULL, "open", szUrl, NULL, NULL, SW_SHOWNORMAL);
}

///////////////////////////////////////////////////////////////////////////////
void CMsAddInBase::ShowUpgradePage(int iSource, int iPath, int iProdnum)
{		
	CFuncLog log(g_pLog, "CMsAddinBase::ShowUpgradePage()");
	CString szUrl;
	CString szUpgrade;
	m_Authenticate.ReadHost(szUrl);
	m_Authenticate.ReadUpgradePage(szUpgrade, iSource, iPath, iProdnum);
	if (szUpgrade.Find("http") >= 0)
		szUrl = szUpgrade;
	else
		szUrl += szUpgrade;
	DWORD dwResult = (DWORD)::ShellExecute(NULL, "open", szUrl, NULL, NULL, SW_SHOWNORMAL);
}

/////////////////////////////////////////////////////////////////////////////
void CMsAddInBase::ShowHelp(DWORD dwHelpID)
{
	CFuncLog log(g_pLog, "CMsAddinBase::ShowHelp()");

	CString szHelp;
	if (!dwHelpID)
		m_Authenticate.ReadHelpPage(szHelp);
	else
		szHelp.LoadString(dwHelpID);
	
	CString szUrl;
	m_Authenticate.ReadHost(szUrl);
	if (szHelp.Find("http") >= 0)
		szUrl = szHelp;
	else
		szUrl += szHelp;
	DWORD dwResult = (DWORD)::ShellExecute(NULL, "open", szUrl, NULL, NULL, SW_SHOWNORMAL);
}

/////////////////////////////////////////////////////////////////////////////
CComPtr<ITextDocument> CMsAddInBase::GetPlainTextDocument(HWND hCompose)
{
	CFuncLog log(g_pLog, "CMsAddinBase::GetPlainTextDocument()");
	if (!::IsWindow(hCompose))
		return NULL;

	HWND hwndRTE = NULL;
	GetRichTextWindow(hCompose, hwndRTE);
	if (!hwndRTE)
	{
		log.LogString(LOG_ERROR, log.FormatString("GetIEServerWindow() FAILED, hCompose == %X", hCompose));
		return NULL;
	}

	// Get a pointer to the doc
	IUnknownPtr pUnk;
	if (!::SendMessage(hwndRTE, EM_GETOLEINTERFACE, 0, (LPARAM)&pUnk))
	{
		log.LogString(LOG_ERROR, log.FormatString("SendMessageTimeout() FAILED, hwndRTE == %X", hwndRTE));
		return NULL;
	}

	CComQIPtr<ITextDocument> spDoc = pUnk;
	return spDoc;
}

/////////////////////////////////////////////////////////////////////////////
IHTMLDocument2Ptr CMsAddInBase::GetHTMLDocument2(HWND hCompose)
{
	CFuncLog log(g_pLog, "CMsAddinBase::GetHTMLDocument2()");
	if (!::IsWindow(hCompose))
		return NULL;

	// Get Internet Explorer_Server
	HWND hwndIEServer = NULL;
	GetIEServerWindow(hCompose, hwndIEServer);
	if (!hwndIEServer)
	{
		log.LogString(LOG_ERROR, log.FormatString("GetIEServerWindow() FAILED, hCompose == %X", hCompose));
		return NULL;
	}

	// Get a pointer to the doc
	LRESULT lResult = NULL;
	if (!::SendMessageTimeout(hwndIEServer, UWM_HTML_GETOBJECT, 0L, 0L, SMTO_ABORTIFHUNG, 5000, (DWORD*)&lResult))
	{
		log.LogString(LOG_ERROR, log.FormatString("SendMessageTimeout() FAILED, hwndIEServer == %X", hwndIEServer));
		return NULL;
	}

	LPFNOBJECTFROMLRESULT pfObjectFromLresult = (LPFNOBJECTFROMLRESULT)::GetProcAddress(m_hMSAAInst, _T("ObjectFromLresult"));
	if (!pfObjectFromLresult)
	{
		log.LogString(LOG_ERROR, log.FormatString("FAILED: pfObjectFromLresult == NULL, m_hMSAAInst == %X", m_hMSAAInst));
		return NULL;
	}

	IHTMLDocument2Ptr spDoc;
	HRESULT hr = (*pfObjectFromLresult)(lResult, IID_IHTMLDocument2, 0, (void**)&spDoc);
	//HRESULT hr = ::ObjectFromLresult(lResult, IID_IHTMLDocument2, 0, reinterpret_cast< void ** >(&spDoc));
	if (FAILED(hr) || spDoc == NULL)
	{
		log.LogString(LOG_ERROR, log.FormatString("FAILED: Call to pfObjectFromLresult(), hr == [%X], spDoc == %X", hr, spDoc));
		return NULL;
	}

	return spDoc;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::SetUnselectableAttribute(IHTMLDocument2* pDoc, LPCTSTR szTagName, LPCTSTR szValue)
{
	CComQIPtr<IHTMLDocument3> spDoc3(pDoc);
	if (!spDoc3)
		return false;

	CComPtr<IHTMLElementCollection> spAll;
	HRESULT hr;

	// Get all szTagName elements
	spAll = NULL;
	hr = spDoc3->getElementsByTagName(CComBSTR(szTagName), &spAll);
	if (spAll)
	{
		// Get enumerator to collection.
		CComPtr<IUnknown> pUnk;
		hr = spAll->get__newEnum(&pUnk);
		if (!pUnk)
			return false;

		CComQIPtr<IEnumVARIANT> spItr(pUnk);
		if (!spItr)
			return false;

		// Iterate thru each element and add unselectable = szValue;
		CComVariant varData;
		ULONG uCount=0;
		bool bDone = false;
		while (!bDone)
		{
			hr = spItr->Next(1, &varData, &uCount);
			if (FAILED(hr) || uCount<=0)
			{
				bDone = true;
				continue;
			}

			CComQIPtr<IHTMLElement> spElem(varData.pdispVal);
			if (!spElem)
				return false;
			
			if (!SetAttribute(spElem, CComBSTR("unselectable"), CComVariant(szValue)))
				return false;
		}

	}
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::AddDefaultStationery(IHTMLDocument2 *pDoc)
{
	CFuncLog log(g_pLog, "CMsAddinBase::AddDefaultStationery()");
	
	if (!m_StationeryMgr.NeedToAdd())
		return false;

	CComPtr<IHTMLDocument2> spDoc(pDoc);
	if (spDoc == NULL)
		return false;

	if (HasProductContent(spDoc))
		return false;

	DownloadParms Parms;
	if (!m_StationeryMgr.AddStationery(spDoc, Parms))
		return false;
		
	m_Usage.Add(Parms,1);

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::HasProductContent(IHTMLDocument2 *pDoc)
{	
	CComPtr<IHTMLDocument2> spDoc(pDoc);
	if (!spDoc)
		return false;

	CComPtr<IHTMLElement> spBody;
	HRESULT hr = spDoc->get_body(&spBody);
	if (!spBody)
		return false;

	CComBSTR bsHtml;
	hr = spBody->get_outerHTML(&bsHtml);
	CString szHtml(bsHtml);
	if (szHtml.IsEmpty())
		return false;

	// If image server is referenced then must contain Creata Mail
	// content.
	CString szImageServer;
	szImageServer.LoadString(IDS_IMAGE_SERVER);
	szImageServer.MakeLower();
	szHtml.MakeLower();
	if (szHtml.Find(szImageServer) <0)
		return false;

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::ServerRegister(HWND hwnd, DWORD dwClientType)
{
	if (::IsWindow(m_hJMSrvrUtilWnd))
		return true;

	if (!::IsWindow(hwnd))
		return false;

	// Register this mail client with JMSrvr.
	CComPtr<IJMBrkr> spBrkr;
	HRESULT hr = spBrkr.CoCreateInstance(CComBSTR("JMSrvr.JMBrkr"), NULL, CLSCTX_LOCAL_SERVER);
	if (FAILED(hr))
		return false;

	hr = spBrkr->RegisterClient((OLE_HANDLE)hwnd, dwClientType, (OLE_HANDLE*)&m_hJMSrvrUtilWnd);
	if (FAILED(hr))
		return false;


	if (!::IsWindow(m_hJMSrvrUtilWnd))
		return false;

	return true;
}
/////////////////////////////////////////////////////////////////////////////
bool CMsAddInBase::ServerUnregister(HWND hwnd)
{
	if (NULL == hwnd)
		return false;

	// Unregister this mail client from JMSrvr.
	CComPtr<IJMBrkr> spBrkr;
	HRESULT hr = spBrkr.CoCreateInstance(CComBSTR("JMSrvr.JMBrkr"), NULL, CLSCTX_LOCAL_SERVER);
	if (FAILED(hr))
		return false;

	hr = spBrkr->UnregisterClient((OLE_HANDLE)hwnd);

	return SUCCEEDED(hr);
}