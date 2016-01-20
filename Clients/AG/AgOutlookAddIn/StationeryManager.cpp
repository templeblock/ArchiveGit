#include "stdafx.h"
#include "StationeryManager.h"
#include "HelperFunctions.h"
#include "MsAddInBase.h"

/////////////////////////////////////////////////////////////////////////////
CStationeryManager::CStationeryManager(CMsAddInBase* pAddin)
{
	m_pAddin = pAddin;
}

/////////////////////////////////////////////////////////////////////////////
CStationeryManager::~CStationeryManager(void)
{
}

/////////////////////////////////////////////////////////////////////////////
void CStationeryManager::SetClientType(DWORD dwType)
{
	m_dwClientType = dwType;
	CreateFolder();
	LoadDefaultStationery();
}

/////////////////////////////////////////////////////////////////////////////
bool CStationeryManager::CreateFolder()
{
	CString szTemp;
	if (!GetInstallPath(szTemp))
		return false;

	szTemp += DEFAULT_STATIONERY_FOLDER;
	szTemp += '\\';

	if (!CreateDir(szTemp, NULL))
		return false;

	if (m_dwClientType == CLIENT_TYPE_EXPRESS)
		szTemp += CString(_T("Express"));
	else if (m_dwClientType == CLIENT_TYPE_OUTLOOK)
		szTemp += CString(_T("Outlook"));
	else if (m_dwClientType == CLIENT_TYPE_IE)
		szTemp += CString(_T("IE"));
	else if (m_dwClientType == CLIENT_TYPE_AOL)
		szTemp += CString(_T("AOL"));
	else
		szTemp += CString(_T("All"));

	szTemp += '\\';
	if (!CreateDir(szTemp, NULL))
		return false;
	
	m_szFolder = szTemp; 
	m_szFile = m_szFolder + CString(_T("Default.html"));

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CStationeryManager::NeedToAdd()
{
	bool bEnabled = false;
	CAuthenticate Auth;
	if (m_dwClientType == CLIENT_TYPE_EXPRESS)
		bEnabled = Auth.IsExpressDefaultStaEnabled();
	else if (m_dwClientType == CLIENT_TYPE_OUTLOOK)
		bEnabled = Auth.IsOutlookDefaultStaEnabled();
	else if (m_dwClientType == CLIENT_TYPE_IE)
		bEnabled = Auth.IsIEDefaultStaEnabled();
	else if (m_dwClientType == CLIENT_TYPE_AOL)
		bEnabled = Auth.IsAOLDefaultStaEnabled();


	return bEnabled && !m_szHTML.IsEmpty();
}

/////////////////////////////////////////////////////////////////////////////
bool CStationeryManager::AddStationery(IHTMLDocument2 *pDoc, DownloadParms &Parms)
{
	LoadDefaultStationery(); // Tim wants to experiment.
	
	if (m_szHTML.IsEmpty())
		return false;
	
	IHTMLDocument2Ptr spDoc(pDoc);
	if (spDoc == NULL) 
		return false;

	if (!IsDocInEditMode(spDoc))
		return false;

	// If default stationery is no longer permissable
	// with current security settings don't add stationery.
	if (!m_pAddin->VerifySecurity(spDoc, m_szHTML))
		return false;

	IHTMLElementPtr spBodyElement;
	HRESULT hr = spDoc->get_body(&spBodyElement);
	if (spBodyElement == NULL)
		return false;
	
	CString szHtmlText;
	m_pAddin->GetHtmlText(spDoc, szHtmlText);
	if (!m_pAddin->WriteHtmlToDocument(CComBSTR(m_szHTML), spDoc))
		return false;

	// Wait for the document to become ready
	bool bOK = IsDocLoadedWait(spDoc);

	m_pAddin->SetHtmlText(spDoc, szHtmlText);
	m_pAddin->SetDocEditFields(spDoc);
	m_pAddin->SetJazzyMailAttribute(spDoc);


#ifdef _DEBUG
	if (!IsDocLoaded(spDoc)) ::MessageBox(NULL, "Doc not ready in AddStationery()", "Creata Mail", MB_OK);
#endif _DEBUG
	
	// Get product info for default stationery from meta tags.
	GetDefaultStationeryProductInfo(spDoc);
	Parms = m_DefaultProductInfo;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CStationeryManager::LoadDefaultStationery()
{
	if (m_szFile.IsEmpty())
		return false;

	// The HTML file has already been pumped thru m_pAddin->PrepareHTML()
	m_szHTML = m_pAddin->ReadHTMLFile(m_szFile);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CStationeryManager::GetDefaultStationeryProductInfo(IHTMLDocument2 *pDoc)
{
	CFuncLog log(g_pLog, "CStationeryManager::GetDefaultStationeryProductInfo()");
	
	CComQIPtr<IHTMLDocument3> spDoc3(pDoc);
	if (!spDoc3)
		return false;

	// Get a collection of element with JMU attribute, i.e. meta product info.
	CComPtr<IHTMLElementCollection> spAll;
	HRESULT hr = spDoc3->getElementsByName(CComBSTR(JMU_ATTRIBUTE), &spAll);
	if (!spAll)
		return false;

	// Get enumerator to collection.
	CComPtr<IUnknown> pUnk;
	hr = spAll->get__newEnum(&pUnk);
	if (!pUnk)
		return false;

	CComQIPtr<IEnumVARIANT> spItr(pUnk);
	if (!spItr)
		return false;

	// Iterate thru each Meta element with JMU attribute
	// until we find default stationery product.
	VARIANT varData;
	ULONG uCount=0;
	bool bDone = false;
	DownloadParms Parms;
	CString szProdID;
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

		VARIANT varValue; 
		varValue.vt = VT_NULL;
		HRESULT hr = spElem->getAttribute(CComBSTR("content"), 0, &varValue);
		if (FAILED(hr) || (varValue.vt == VT_NULL))
			continue;
		
		// Get product info of default stationery
		CString szValue(varValue.bstrVal);
		GetProductInfoFromContent(szValue, szProdID, m_DefaultProductInfo);
		
		if (m_DefaultProductInfo.iGroup == ASSET_GROUP_DEFAULTSTA)
			bDone = true;	
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CStationeryManager::ClearDefault()
{
	m_szHTML.Empty();

	if (m_szFolder.IsEmpty())
		return false;

	// Delete all html files in default stationery folder.
	WIN32_FIND_DATA FileData;
	HANDLE hFind;
	CString strFindPath = m_szFolder;
	strFindPath +=  _T("*.htm?");
	hFind = FindFirstFile(strFindPath, &FileData); 
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		FindClose(hFind);
		return false;
	}
	
	
	CString strFilePath;
	bool bDone = false;
	while (!bDone)
	{
		strFilePath = m_szFolder;
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
	

	FindClose(hFind);
	return true;
}

/////////////////////////////////////////////////////////////////////////////
CString& CStationeryManager::GetPath()
{
	if (m_szFolder.IsEmpty())
		CreateFolder();
	
	return m_szFolder; 
}

/////////////////////////////////////////////////////////////////////////////
CString& CStationeryManager::GetFile()
{
	return m_szFile; 
}

/////////////////////////////////////////////////////////////////////////////
bool CStationeryManager::SetDefault(DownloadParms& Parms)
{
	CString szHTML;
	if (Parms.strSrc.IsEmpty())
		szHTML = Parms.strDst;
	else
		szHTML = m_pAddin->ReadHTML(Parms.strSrc, Parms.strDst, false/*bPreview*/);

	if (szHTML.IsEmpty())
		return false;

	// Update product info for default stationery.
	m_DefaultProductInfo = Parms;

	// Add Meta data to html
	CString szMetaTag;
	szMetaTag.Format("<META name=\"%s\" content=\"%d,%d,%d,%d\">", JMU_ATTRIBUTE, Parms.iCategory, Parms.iProductId, Parms.iPathPrice, Parms.iGroup);
	CString szReplace = szMetaTag;
	szReplace += "</head";
	Replace(szHTML, "</head", szReplace);

	if (!m_pAddin->WriteHTMLFile(m_szFile, szHTML))
		return false;

	m_szHTML = szHTML;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
const CString& CStationeryManager::GetHTML()
{
	return m_szHTML;
}
