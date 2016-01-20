#include "StdAfx.h"
#include "ContentBase.h"
#include "XMLDocument2.h"
#include "Utility.h"

bool CContentBase::m_bFoldersCreated = false;
///////////////////////////////////////////////////////////////////////////////
CContentBase::CContentBase(LPCTSTR szReposPath, LPCTSTR szHost)
{
	m_szHost				= szHost;
	m_szRepositoryPath		= szReposPath;
	m_szThumbsFolder		= m_szRepositoryPath + _T("Thumbs\\");
	m_szPreviewFolder		= m_szRepositoryPath + _T("Preview\\");
	m_szLifeStyleFolder		= m_szRepositoryPath + _T("LifeStyle\\");
	m_szCollectionsFolder	= m_szRepositoryPath + _T("Collections\\");
	m_szProjectsFolder		= m_szRepositoryPath + _T("Projects\\");
	m_szArtFolder			= m_szRepositoryPath + _T("Art\\");
	m_szVersion				= _T("beta"); // temp - jhc
	
	m_szDate = GetTimeStamp(_T("%y%m%d"));
	
	if (!m_bFoldersCreated)
		Initialize();
}

CContentBase::~CContentBase(void)
{
	Empty();
}
///////////////////////////////////////////////////////////////////////////////
bool CContentBase::Initialize()
{
	// Create Folders
	int iCount = 0;
	if (CreateDir(m_szThumbsFolder))
		iCount++;

	if (CreateDir(m_szPreviewFolder))
		iCount++;

	if (CreateDir(m_szLifeStyleFolder))
		iCount++;

	if (CreateDir(m_szCollectionsFolder))
		iCount++;

	if (CreateDir(m_szProjectsFolder))
		iCount++;

	if (CreateDir(m_szArtFolder))
		iCount++;

	if (iCount < 6)
		return false;
	
	m_bFoldersCreated = true;

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CContentBase::ProcessElement(IXMLDOMElement * pElem, CString& szId)
{
	HRESULT hr;
	CComPtr<IXMLDOMElement> spElem(pElem);
	if (NULL == spElem)
		return false;

	try
	{
		// XML
		CComBSTR bstrXML;
		hr = spElem->get_xml(&bstrXML);
		m_szXML = bstrXML;
		if (m_szXML.IsEmpty())
			return false;

		// Id
		VARIANT varValue;
		hr = spElem->getAttribute(CComBSTR(ATTRIBUTE_ID), &varValue);
		if (FAILED(hr))
			return false;
		m_szId = varValue.bstrVal;


		// NAME
		hr = spElem->getAttribute(CComBSTR(ATTRIBUTE_NAME), &varValue);
		if (FAILED(hr))
			return false;
		m_szName = varValue.bstrVal;


		// Type
		hr = spElem->getAttribute(CComBSTR(ATTRIBUTE_TYPE), &varValue);
		if (SUCCEEDED(hr))
			m_szType = varValue.bstrVal;


		// Keywords
		hr = spElem->getAttribute(CComBSTR(ATTRIBUTE_KEYWORDS), &varValue);
		if (SUCCEEDED(hr))
			m_szKeywords = varValue.bstrVal;

		// Occasion
		hr = spElem->getAttribute(CComBSTR(ATTRIBUTE_OCCASION), &varValue);
		if (SUCCEEDED(hr))
			m_szOccasion = varValue.bstrVal;

		// License
		hr = spElem->getAttribute(CComBSTR(ATTRIBUTE_LICENSE), &varValue);
		if (SUCCEEDED(hr))
			m_szLicense = varValue.bstrVal;

		// Instructions (Special)
		hr = spElem->getAttribute(CComBSTR(ATTRIBUTE_INSTRUCT), &varValue);
		if (SUCCEEDED(hr))
			m_szInstructions = varValue.bstrVal;


		// Thumb
		hr = spElem->getAttribute(CComBSTR(ATTRIBUTE_THUMB), &varValue);
		if (SUCCEEDED(hr))
			m_szSourceThumbPath = m_szHost + CString(varValue.bstrVal);
		
	}
	catch(...)
	{
		return false;
	}

	szId = m_szId;
	

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CContentBase::ReadReferences(IXMLDOMElement * pElem, LPCTSTR szTagName)
{
	CComPtr<IXMLDOMElement> spElem(pElem);
	if (NULL == spElem)
		return false;

	
	HRESULT hr;
	CComPtr<IXMLDOMNodeList> spRefNodeList;
	hr = spElem->getElementsByTagName(CComBSTR(szTagName), &spRefNodeList);
	if (NULL == spRefNodeList)
		return true; // its legitimate if there are NO szTagName references
	

	CComPtr<IXMLDOMNode> spRefNode;
	long lLen=0;
	hr = spRefNodeList->get_length(&lLen);
	long i = 0;
	CString szId;
	CString szSeq;
	while (i < lLen)
	{
		hr = spRefNodeList->get_item(i, &spRefNode);
		if (FAILED(hr))
			return false;

		CComQIPtr<IXMLDOMElement> spRefElem(spRefNode);
		if (NULL == spRefElem)
			return false;
		
		// ref Id
		VARIANT varValue;
		hr = spRefElem->getAttribute(CComBSTR(ATTRIBUTE_ID), &varValue);
		szId = varValue.bstrVal;
		if (FAILED(hr) || szId.IsEmpty())
			return false;

		// ref Seq
		hr = spRefElem->getAttribute(CComBSTR(ATTRIBUTE_SEQ), &varValue);
		szSeq = varValue.bstrVal;
		if (FAILED(hr) || szSeq.IsEmpty())
			return false;

		// Add to container.
		m_Container[szId].szSeq = szSeq;
		m_Container[szId].szTagName = szTagName;

		spRefNode = NULL;
		i++;
	}

	return true;
}
///////////////////////////////////////////////////////////////////////////////
void CContentBase::Empty()
{
	m_Container.clear();
}
///////////////////////////////////////////////////////////////////////////////
// Local path is the native directory path for specific content,
// i.e. Projects, Art...
///////////////////////////////////////////////////////////////////////////////
bool CContentBase::UpdateLocalPaths()
{
	// Thumbs
	CString szFileName;
	if (!m_szSourceThumbPath.IsEmpty())
	{
		szFileName = StrFileName(m_szSourceThumbPath);
		if (!szFileName.IsEmpty())
			m_szLocalThumbPath = m_szThumbsFolder + szFileName;
	}

	// Collection Path
	if (!m_szId.IsEmpty())
	{
		m_szLocalCollectionPath = m_szCollectionsFolder + m_szId + _T("C.xml");
	}

	// Project Path
	if (!m_szSourceProjectPath.IsEmpty())
	{
		CString szFileName;
		szFileName = StrFileName(m_szSourceProjectPath);
		if (!szFileName.IsEmpty())
			m_szLocalProjectPath = m_szProjectsFolder + szFileName;
	}

	// Art Path
	if (!m_szSourceArtPath.IsEmpty())
	{
		CString szFileName;
		szFileName = StrFileName(m_szSourceArtPath);
		if (!szFileName.IsEmpty())
			m_szLocalArtPath = m_szArtFolder + szFileName;;
	}
		

	// Preview Image
	if (!m_szSourcePreviewPath.IsEmpty())
	{
		szFileName = StrFileName(m_szSourcePreviewPath);
		if (!szFileName.IsEmpty())
			m_szLocalPreviewPath = m_szPreviewFolder + szFileName;
	}

	// LifeStyle Image
	if (!m_szSourceLifeStylePath.IsEmpty())
	{
		szFileName = StrFileName(m_szSourceLifeStylePath);
		if (!szFileName.IsEmpty())
			m_szLocalLifeStylePath = m_szLifeStyleFolder + szFileName;
	}


	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CContentBase::GetFilesToDownload(CDownload& downnload)
{
	// Thumb
	if (!m_szLocalThumbPath.IsEmpty())
	{
		downnload.AddFile(m_szSourceThumbPath, m_szLocalThumbPath, NULL);
	}

	// Preview
	if (!m_szLocalPreviewPath.IsEmpty())
	{
		downnload.AddFile(m_szSourcePreviewPath, m_szLocalPreviewPath, NULL);
	}

	// LifeStyle
	if (!m_szLocalLifeStylePath.IsEmpty())
	{
		downnload.AddFile(m_szSourceLifeStylePath, m_szLocalLifeStylePath, NULL);
	}

	// Projects
	if (!m_szLocalProjectPath.IsEmpty())
	{
		downnload.AddFile(m_szSourceProjectPath, m_szLocalProjectPath, NULL);
	}

	// Art
	if (!m_szLocalArtPath.IsEmpty())
	{
		downnload.AddFile(m_szSourceArtPath, m_szLocalArtPath, NULL);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
bool CContentBase::UpdateManifest(CXMLDocument2 &XMLDoc2)
{
	if (m_szId.IsEmpty())
		return false;


	// See if this node exists. If it exist then overwrite.
	CString szSearchPattern = _T("//") + m_szParentTag;
	szSearchPattern += _T("//") + m_szTagName + _T("[@id='") + m_szId + _T("']"); 
	CComPtr<IXMLDOMNode> spXMLTargetNode;
	HRESULT hr = XMLDoc2.FindElement(szSearchPattern, &spXMLTargetNode);
	if (FAILED(hr))
		return false;

	// Otherwise if node does not exist, append to end of content list.
	// ex: szSearchPattern = "/projectlist/project[@id='123456']"
	if (NULL == spXMLTargetNode)
	{
		CComPtr<IXMLDOMNode> spXMLParentNode;
		szSearchPattern = _T("//") + m_szParentTag;
		hr = XMLDoc2.FindElement(szSearchPattern, &spXMLParentNode);
		if (NULL == spXMLParentNode)
			return false; // Error if manifest is missing parent tags.
		
		hr = XMLDoc2.AppendElement(m_szTagName, spXMLParentNode, spXMLTargetNode);
		if (FAILED(hr))
			return false;
	}

	// Update attributes for node and adding any reference elements.
	if (spXMLTargetNode)
	{
		UpdateAttributes(XMLDoc2, spXMLTargetNode);
		if (!m_Container.empty())
			UpdateReferences(XMLDoc2, spXMLTargetNode);
	}

	
	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CContentBase::UpdateAttributes(CXMLDocument2 &XMLDoc2, CComPtr<IXMLDOMNode> &spNode)
{
	CComQIPtr<IXMLDOMElement> spXMLElem(spNode);
	if (NULL == spXMLElem)
		return false;

	HRESULT hr;
	hr = XMLDoc2.SetAttribute(spXMLElem, ATTRIBUTE_ID, m_szId);
	if (FAILED(hr))
		return false;

	hr = XMLDoc2.SetAttribute(spXMLElem, ATTRIBUTE_NAME, m_szName);
	if (FAILED(hr))
		return false;

	hr = XMLDoc2.SetAttribute(spXMLElem, ATTRIBUTE_KEYWORDS, m_szKeywords);
	if (FAILED(hr))
		return false;

	hr = XMLDoc2.SetAttribute(spXMLElem, ATTRIBUTE_OCCASION, m_szOccasion);
	if (FAILED(hr))
		return false;

	hr = XMLDoc2.SetAttribute(spXMLElem, ATTRIBUTE_THUMB, m_szLocalThumbPath);
	if (FAILED(hr))
		return false;

	hr = XMLDoc2.SetAttribute(spXMLElem, ATTRIBUTE_DATE, m_szDate);
	if (FAILED(hr))
		return false;

	hr = XMLDoc2.SetAttribute(spXMLElem, ATTRIBUTE_LICENSE, m_szLicense);
	if (FAILED(hr))
		return false;

	hr = XMLDoc2.SetAttribute(spXMLElem, ATTRIBUTE_INSTRUCT, m_szInstructions);
	if (FAILED(hr))
		return false;

	hr = XMLDoc2.SetAttribute(spXMLElem, ATTRIBUTE_CREATOR, m_szCreator);
	if (FAILED(hr))
		return false;


	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CContentBase::UpdateReferences(CXMLDocument2 &XMLDoc2, CComPtr<IXMLDOMNode> &spNode)
{
	CComQIPtr<IXMLDOMElement> spXMLElem(spNode);
	if (NULL == spXMLElem)
		return false;

	HRESULT hr = S_OK;
	CComPtr<IXMLDOMNode> spXMLParentNode = spNode;
	CComPtr<IXMLDOMNode> spXMLTargetNode;

	CONTAINERTYPE::iterator itr;
	itr = m_Container.begin();
	CString szRefTagname;
	CString szSeq;
	CString szID;

	// Iterate thru container of references. A reference is a project, 
	// art, or verse referenced in either a collection or project.
	while(!m_Container.empty() && itr != m_Container.end())
	{
		szID = itr->first;
		szRefTagname = itr->second.szTagName;
		szSeq = itr->second.szSeq;

		// See if this node exists. If it exist then overwrite.
		// ex: szSearchPattern = "/collection[@id='301234']/projectref[@id='123456']"
		CString szSearchPattern = _T("//") + m_szTagName;
		szSearchPattern +=  _T("[@id='") + m_szId + _T("']"); 
		szSearchPattern += _T("//") + szRefTagname + _T("[@id='") + szID + _T("']"); 
		HRESULT hr = XMLDoc2.FindElement(szSearchPattern, &spXMLTargetNode);
		
		// Otherwise, if node does not exist, create/append to end of reference list.
		if (NULL == spXMLTargetNode)
		{
			hr = XMLDoc2.AppendElement(szRefTagname, spXMLParentNode, spXMLTargetNode);
		}

		// If we have reference element then update attributes (id and sequence).
		CComQIPtr<IXMLDOMElement> spXMLTargetElem(spXMLTargetNode);
		if (NULL != spXMLTargetElem && SUCCEEDED(hr))
		{
			hr = XMLDoc2.SetAttribute(spXMLTargetElem, ATTRIBUTE_ID, szID);
			if (SUCCEEDED(hr))
				hr = XMLDoc2.SetAttribute(spXMLTargetElem, ATTRIBUTE_SEQ, szSeq);	
		}	
		
		// Release node
		spXMLTargetNode = NULL;

		itr++;
	}


	if (FAILED(hr))
		return false;

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CContentBase::Validate(CXMLDocument2 &XMLDoc2)
{
	UpdateErrorField(XMLDoc2, m_szError);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
bool CContentBase::FileExists(LPCTSTR szFilePath)
{
	CFindFile File;
	if (!File.FindFile(szFilePath))
		return false;

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CContentBase::UpdateErrorField(CXMLDocument2 &XMLDoc2, LPCTSTR szError)
{
	// Find correct content node in manifest.
	CString szSearchPattern = _T("//") + m_szParentTag;
	szSearchPattern += _T("//") + m_szTagName + _T("[@id='") + m_szId + _T("']"); 
	CComPtr<IXMLDOMNode> spNode;
	HRESULT hr = XMLDoc2.FindElement(szSearchPattern, &spNode);
	if (FAILED(hr))
		return false;

	CComQIPtr<IXMLDOMElement> spXMLElem(spNode);
	if (NULL == spXMLElem)
		return false;

	hr = XMLDoc2.SetAttribute(spXMLElem, ATTRIBUTE_ERROR, szError);
	if (FAILED(hr))
		return false;


	return true;
}