#include "StdAfx.h"
#include "Resource.h"
#include "Repository.h"
//#include "HelperFunctions.h"
#include "Utility.h"


LPCTSTR TAG_PACK		= _T("pack");
LPCTSTR ATTRIBUTE_HOST	= _T("host");
///////////////////////////////////////////////////////////////////////////////
CRepository::CRepository(void)
{
	bool bOK = GetSpecialFolderLocation(m_szPath, CSIDL_PERSONAL);
	m_szPath += REPOSITORY_ROOT;
	m_szLocalManifestPath = m_szPath + REPOSITORY_FILENAME;
	m_szBackupFolder = m_szPath + _T("Backup\\");
	m_szBackupManifestPath = m_szBackupFolder + REPOSITORY_FILENAME;
	m_szEmptyManifestXML = _T("<?xml version=\"1.0\" encoding=\"iso-8859-1\"?> <!--Copyright © 2006 AmericanGreetings.com-->\
							<manifest accesscode=\"\"><collectionlist></collectionlist><projectlist></projectlist><artlist></artlist><verselist></verselist></manifest>");
}

CRepository::~CRepository(void)
{
	Empty();
}
/////////////////////////////////////////////////////////////////////////////
void _cdecl CRepository::XMLCallback(int iLevel, bool bAttribute, LPCTSTR pNodeName, LPCTSTR pValueString, LPARAM lParam)
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
bool CALLBACK  CRepository::DownloadCallback(LPARAM lParam, void* pDownloadVoid, long lError)
{
	CRepository* pRepository = (CRepository*)lParam;
	DOWNLOAD* pDownload = (DOWNLOAD*)pDownloadVoid;
	if (!pDownload || lError > 0) // The final callback when there are no more files to download
	{
		return pRepository->DownloadComplete(lError);
	}

	return true;
}
////////////////////////////////////////////////////////////////////////////////
bool CRepository::DownloadComplete(long lError)
{
	::MessageBox(NULL, _T("Download complete!"), _T("Print at Home"), MB_OK);
	
	// Backup current manifest.
	BackupManifest();
	
	if (!LoadLocalManifest())
	{
		int IResp = ::MessageBox(NULL, _T("Repository Manifest is missing would you like to create?"), _T("PAH"), MB_YESNO|MB_ICONQUESTION);
		if (IDYES==IResp && !BuildManifest())
		{
			::MessageBox(NULL, _T("Repository Manifest build failure!"), _T("Error"), MB_OK|MB_ICONSTOP);
			return false;
		}
	}
	
	if (!UpdateRepositoryManifest())
	{
		::MessageBox(NULL, _T("Updating Repository not updated"), _T("Warning"), MB_OK|MB_ICONINFORMATION);
	}

	if (!ValidateRepositoryManifest())
	{
		::MessageBox(NULL, _T("Validating Repository Manifest failed!"), _T("Error"), MB_OK|MB_ICONSTOP);
	}
	
	if (!SaveManifest())
	{
		::MessageBox(NULL, _T("Saving Repository Manifest failed!"), _T("Error"), MB_OK|MB_ICONSTOP);
		return false;
	}


	//::SetEvent(m_hEventDownloadComplete);

	return true;
}
///////////////////////////////////////////////////////////////////////////////
// Load an XML Document from the specified file or URL.
///////////////////////////////////////////////////////////////////////////////
bool CRepository::LoadXML(LPCTSTR szURLFileName)
{
	if (CString(szURLFileName).IsEmpty())
		return false;

	HRESULT hr;

	// Load xml document from the given URL or file path
	hr = m_XMLDoc.RegisterCallback(XMLCallback, (LPARAM)this);
	if (FAILED(hr))
		return false;

	hr = m_XMLDoc.Load(szURLFileName);
	if (FAILED(hr))
		return false;

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CRepository::LoadLocalManifest()
{
	if (m_szLocalManifestPath.IsEmpty())
		return false;

	HRESULT hr;
	hr = m_XMLLocalManifest.RegisterCallback(XMLCallback, (LPARAM)this);
	hr = m_XMLLocalManifest.Load(m_szLocalManifestPath);
	if (FAILED(hr))
		return false;

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CRepository::BuildManifest()
{
	if (m_szLocalManifestPath.IsEmpty())
		return false;

	if (!SaveXML(m_szLocalManifestPath, m_szEmptyManifestXML))
		return false;

	HRESULT hr;
	hr = m_XMLLocalManifest.RegisterCallback(XMLCallback, (LPARAM)this);
	hr = m_XMLLocalManifest.Load(m_szLocalManifestPath);
	if (FAILED(hr))
		return false;

	return true;
}
////////////////////////////////////////////////////////////////////////////////
bool CRepository::ProcessXMLPack()
{
	try
	{
		CComPtr<IXMLDOMDocument> spXMLDoc = m_XMLDoc.GetDOMDocument();
		if (NULL == spXMLDoc)
			return false;

		HRESULT hr;
		CComPtr<IXMLDOMNodeList> spNodeList;
		hr = spXMLDoc->getElementsByTagName(CComBSTR(TAG_PACK), &spNodeList);
		if (FAILED(hr))
			return false;
		
		long lLen = 0;
		hr = spNodeList->get_length(&lLen);
		if (!lLen)
			return false;

		CComPtr<IXMLDOMNode> spNode;
		hr = spNodeList->get_item(0, &spNode);
		CComQIPtr<IXMLDOMElement> spElem(spNode);
		if (NULL == spElem)
			return false;

		// Host
		VARIANT varValue;
		hr = spElem->getAttribute(CComBSTR(ATTRIBUTE_HOST), &varValue);
		m_szHost = varValue.bstrVal;
		if (FAILED(hr) || m_szHost.IsEmpty())
			return false;


		// Read all Collections
		ReadItemsByTagName(spElem, TAG_COLLECTION);

		// Read all Projects
		ReadItemsByTagName(spElem, TAG_PROJECT);

		// Read all Art
		ReadItemsByTagName(spElem, TAG_ART);

		// Read all Verses
		ReadItemsByTagName(spElem, TAG_VERSE);

		
	}
	catch(...)
	{
		return false;
	}
	
	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CRepository::AddCollection(IXMLDOMNode * pNode)
{
	if (m_szPath.IsEmpty())
		return false;
	
	CComQIPtr<IXMLDOMElement> spElem(pNode);
	if (NULL == spElem)
		return false;


	CPAHCollection *pCollection = new CPAHCollection(m_szPath, m_szHost);
	if (!pCollection)
		return false;


	CString szId;
	if (pCollection->ProcessElement(spElem, szId) && !szId.IsEmpty())
	{
		m_Collections[szId] = pCollection;
	}
	else
	{
		delete pCollection;
		return false;
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CRepository::AddProject(IXMLDOMNode * pNode)
{
	if (m_szPath.IsEmpty())
		return false;

	CComQIPtr<IXMLDOMElement> spElem(pNode);
	if (NULL == spElem)
		return false;


	CPAHProject *pProject = new CPAHProject(m_szPath, m_szHost);
	if (!pProject)
		return false;


	CString szId;
	if (pProject->ProcessElement(spElem, szId) && !szId.IsEmpty())
	{
		m_Projects[szId] = pProject;
	}
	else
	{
		delete pProject;
		return false;
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CRepository::AddArt(IXMLDOMNode * pNode)
{
	if (m_szPath.IsEmpty())
		return false;
	
	CComQIPtr<IXMLDOMElement> spElem(pNode);
	if (NULL == spElem)
		return false;


	CArt *pArt = new CArt(m_szPath, m_szHost);
	if (!pArt)
		return false;


	CString szId;
	if (pArt->ProcessElement(spElem, szId) && !szId.IsEmpty())
	{
		m_Art[szId] = pArt;
	}
	else
	{
		delete pArt;
		return false;
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CRepository::AddVerse(IXMLDOMNode * pNode)
{
	if (m_szPath.IsEmpty())
		return false;
	
	CComQIPtr<IXMLDOMElement> spElem(pNode);
	if (NULL == spElem)
		return false;


	CVerse *pVerse = new CVerse(m_szPath, m_szHost);
	if (!pVerse)
		return false;


	CString szId;
	if (pVerse->ProcessElement(spElem, szId) && !szId.IsEmpty())
	{
		m_Verses[szId] = pVerse;
	}
	else
	{
		delete pVerse;
		return false;
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CRepository::ReadItemsByTagName(IXMLDOMElement * pElem, LPCTSTR szTagName)
{
	CComPtr<IXMLDOMElement> spElem(pElem);
	if (NULL == spElem)
		return false;

	CComPtr<IXMLDOMNodeList> spNodeList;
	HRESULT hr;
	hr = spElem->getElementsByTagName(CComBSTR(szTagName), &spNodeList);
	
	if (NULL != spNodeList)
	{
		CComPtr<IXMLDOMNode> spNode;
		long lLen=0;
		hr = spNodeList->get_length(&lLen);
		long i = 0;

		CString szTag = szTagName;
		if (szTag.IsEmpty())
			return false;

		while (i < lLen)
		{
			hr = spNodeList->get_item(i, &spNode);
			if (FAILED(hr))
				return false;

			if (0 == szTag.CompareNoCase(TAG_COLLECTION))
				AddCollection(spNode);
			else if (0 == szTag.CompareNoCase(TAG_PROJECT))
				AddProject(spNode);
			else if (0 == szTag.CompareNoCase(TAG_ART))
				AddArt(spNode);
			else if (0 == szTag.CompareNoCase(TAG_VERSE))
				AddVerse(spNode);
			else
			{
				::MessageBox(NULL, _T("Tag Name is undefined!" ), _T("Error"), MB_OK|MB_ICONSTOP);
				return false;
			}
			
			spNode = NULL;
			i++;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
void CRepository::EmptyCollections()
{
	COLLECTIONSTYPE::iterator itr;
	itr = m_Collections.begin();
	while (itr != m_Collections.end())
	{
		delete (itr->second);
		itr++;
	}
	m_Collections.clear();
}
///////////////////////////////////////////////////////////////////////////////
void CRepository::EmptyProjects()
{
	PROJECTSTYPE::iterator itr;
	itr = m_Projects.begin();
	while (itr != m_Projects.end())
	{
		delete (itr->second);
		itr++;
	}
	m_Projects.clear();
}
///////////////////////////////////////////////////////////////////////////////
void CRepository::EmptyArt()
{
	ARTTYPE::iterator itr;
	itr = m_Art.begin();
	while (itr != m_Art.end())
	{
		delete (itr->second);
		itr++;
	}
	m_Art.clear();
}
///////////////////////////////////////////////////////////////////////////////
void CRepository::EmptyVerses()
{
	VERSETYPE::iterator itr;
	itr = m_Verses.begin();
	while (itr != m_Verses.end())
	{
		delete (itr->second);
		itr++;
	}
	m_Verses.clear();
}
///////////////////////////////////////////////////////////////////////////////
void CRepository::Empty()
{
	EmptyVerses();
	EmptyArt();
	EmptyProjects();
	EmptyCollections();
}
///////////////////////////////////////////////////////////////////////////////
bool CRepository::SaveXMLDoc(CXMLDocument2 &XMLDoc2, LPCTSTR szDestPath)
{
	if (CString(szDestPath).IsEmpty())
		return false;

	HRESULT hr;

	if (!CreateDir(szDestPath))
	{
		GetError(_T("SaveXMLDoc::CreateDirectory()"));
	}
	try
	{
		hr = XMLDoc2.Save(szDestPath);
		if (FAILED(hr))
			return false;
	}
	catch(...)
	{
		return false;
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CRepository::SaveManifest()
{
	if (NULL == m_XMLLocalManifest.GetDOMDocument() || m_szLocalManifestPath.IsEmpty())
		return false;

	return SaveXMLDoc(m_XMLLocalManifest, m_szLocalManifestPath);
}
///////////////////////////////////////////////////////////////////////////////
bool CRepository::BackupManifest()
{
	if (CString(m_szBackupManifestPath).IsEmpty() || m_szLocalManifestPath.IsEmpty())
		return false;

	CreateDir(m_szBackupFolder);

	if (!::CopyFile(m_szLocalManifestPath, m_szBackupManifestPath, false))
	{
		GetError(_T("BackupManifest()"));
		return false;
	}
	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CRepository::DownloadFiles(void)
{
	if (!m_Download.Init(1/*iFileType*/, DownloadCallback, (LPARAM)this))
		return false;

	// Download Collections (thumb and lifestyle images)
	COLLECTIONSTYPE::iterator itrCollections;
	itrCollections = m_Collections.begin();
	while(!m_Collections.empty() && itrCollections != m_Collections.end())
	{
		itrCollections->second->GetFilesToDownload(m_Download);
		itrCollections++;
	}
	
	// Download Projects
	PROJECTSTYPE::iterator itrProjects;
	itrProjects = m_Projects.begin();
	while(!m_Projects.empty() && itrProjects != m_Projects.end())
	{
		itrProjects->second->GetFilesToDownload(m_Download);
		itrProjects++;
	}

	// Download Art
	ARTTYPE::iterator itrArt;
	itrArt = m_Art.begin();
	while(!m_Art.empty() && itrArt != m_Art.end())
	{
		itrArt->second->GetFilesToDownload(m_Download);
		itrArt++;
	}
	
	// Start downloading files
	if (!m_Download.Start(true/*bGoOnline*/, _T("Download in Progress")))
		return false;


	/*m_hEventDownloadComplete = ::CreateEvent(NULL, false, false, NULL);
	::WaitForSingleObject(m_hEventDownloadComplete, 1200000);
	if(::CloseHandle(m_hEventDownloadComplete))
			m_hEventDownloadComplete = NULL;*/

	return true;
}
///////////////////////////////////////////////////////////////////////////////
// The repository manifest is the local manifest that keeps track of all content
// owned on client machine. UpdateRepositoryManifest() iterates thru all content 
// elements of repository and updates attributes for each. For example, attributes
// may have changed due to purchase and download.
///////////////////////////////////////////////////////////////////////////////
bool CRepository::UpdateRepositoryManifest()
{
	if (NULL == m_XMLLocalManifest.GetDOMDocument())
		return false;


	// Update Collections
	long lUpdateCount=0;
	COLLECTIONSTYPE::iterator itrCollections;
	itrCollections = m_Collections.begin();
	while(!m_Collections.empty() && itrCollections != m_Collections.end())
	{
		if (itrCollections->second->UpdateManifest(m_XMLLocalManifest))
			lUpdateCount++;
		itrCollections++;
	}
	
	// Update Projects
	PROJECTSTYPE::iterator itrProjects;
	itrProjects = m_Projects.begin();
	while(!m_Projects.empty() && itrProjects != m_Projects.end())
	{
		if (itrProjects->second->UpdateManifest(m_XMLLocalManifest))
			lUpdateCount++;
		itrProjects++;
	}

	// Update Art
	ARTTYPE::iterator itrArt;
	itrArt = m_Art.begin();
	while(!m_Art.empty() && itrArt != m_Art.end())
	{
		if (itrArt->second->UpdateManifest(m_XMLLocalManifest))
			lUpdateCount++;
		itrArt++;
	}
	
	// Update Verses
	VERSETYPE::iterator itrVerse;
	itrVerse = m_Verses.begin();
	while(!m_Verses.empty() && itrVerse != m_Verses.end())
	{
		if (itrVerse->second->UpdateManifest(m_XMLLocalManifest))
			lUpdateCount++;
		itrVerse++;
	}

	if (lUpdateCount == 0)
		return false;

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CRepository::ValidateRepositoryManifest()
{
	if (NULL == m_XMLLocalManifest.GetDOMDocument())
		return false;


	// Validate Collections
	long lErrorCount=0;
	COLLECTIONSTYPE::iterator itrCollections;
	itrCollections = m_Collections.begin();
	while(!m_Collections.empty() && itrCollections != m_Collections.end())
	{
		if (!itrCollections->second->Validate(m_XMLLocalManifest))
			lErrorCount++;
		itrCollections++;
	}
	
	// Validate Projects
	PROJECTSTYPE::iterator itrProjects;
	itrProjects = m_Projects.begin();
	while(!m_Projects.empty() && itrProjects != m_Projects.end())
	{
		if (!itrProjects->second->Validate(m_XMLLocalManifest))
			lErrorCount++;
		itrProjects++;
	}

	// Validate Art
	ARTTYPE::iterator itrArt;
	itrArt = m_Art.begin();
	while(!m_Art.empty() && itrArt != m_Art.end())
	{
		if (!itrArt->second->Validate(m_XMLLocalManifest))
			lErrorCount++;
		itrArt++;
	}

	// Validate Verses
	VERSETYPE::iterator itrVerse;
	itrVerse = m_Verses.begin();
	while(!m_Verses.empty() && itrVerse != m_Verses.end())
	{
		if (!itrVerse->second->Validate(m_XMLLocalManifest))
			lErrorCount++;
		itrVerse++;
	}
	
	if (lErrorCount > 0)
		return false;

	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool CRepository::GetInstallPath(CString &szPath)
{
	//GetAppPath(szPath);

#ifdef _DEBUG
	szPath = _T("C:\\My Projects\\AgApplications\\PrintatHome\\");
#endif _DEBUG

	// Ensure path ends with '\'
	int iLen = szPath.GetLength();
	int i = szPath.ReverseFind(_T('\\')); 
	if (i < (iLen-1))
		szPath += _T('\\');

	return true;
}