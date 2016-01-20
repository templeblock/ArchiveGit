#include "StdAfx.h"
#include "PAHProject.h"
#include "File.h"

///////////////////////////////////////////////////////////////////////////////
CPAHProject::CPAHProject(LPCTSTR szFilePath, LPCTSTR szHost) : CContentBase(szFilePath, szHost)
{
	m_szParentTag		= TAG_PROJECTPARENT;
	m_szTagName			= TAG_PROJECT;
}

CPAHProject::~CPAHProject(void)
{
}
///////////////////////////////////////////////////////////////////////////////
bool CPAHProject::ProcessElement(IXMLDOMElement * pElem, CString& szId)
{
	HRESULT hr;
	VARIANT varValue;
	CComPtr<IXMLDOMElement> spElem(pElem);
	if (NULL == spElem)
		return false;

	if (!CContentBase::ProcessElement(spElem, szId))
		return false;

	try
	{
		// Preview
		hr = spElem->getAttribute(CComBSTR(ATTRIBUTE_PREVIEW), &varValue);
		if (FAILED(hr))
			return false;
		m_szSourcePreviewPath = m_szHost + CString(varValue.bstrVal);

		// Projects
		hr = spElem->getAttribute(CComBSTR(ATTRIBUTE_PROJECTPATH), &varValue);
		if (FAILED(hr))
			return false;
		m_szSourceProjectPath = m_szHost + CString(varValue.bstrVal);

		

		// Read all ART References
		if (!ReadReferences(spElem, TAG_ARTREF))
			return false;

		// Read all VERSE References
		if (!ReadReferences(spElem, TAG_VERSEREF))
			return false;
	
		hr = spElem->cloneNode(true, &m_spXMLNode); 
		if (FAILED(hr) || (NULL == m_spXMLNode))
			return false;
	}
	catch(...)
	{
		return false;
	}

	if (!UpdateLocalPaths())
		return false;	

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CPAHProject::UpdateAttributes(CXMLDocument2 &XMLDoc2, CComPtr<IXMLDOMNode> &spNode)
{
	CComQIPtr<IXMLDOMElement> spXMLElem(spNode);
	if (NULL == spXMLElem)
		return false;

	if (!CContentBase::UpdateAttributes(XMLDoc2, spNode))
		return false;

	HRESULT hr;

	hr = XMLDoc2.SetAttribute(spXMLElem, ATTRIBUTE_TYPE, m_szType);
	if (FAILED(hr))
		return false;

	hr = XMLDoc2.SetAttribute(spXMLElem, ATTRIBUTE_PREVIEW, m_szLocalPreviewPath);
	if (FAILED(hr))
		return false;

	hr = XMLDoc2.SetAttribute(spXMLElem, ATTRIBUTE_PROJECTPATH, m_szLocalProjectPath);
	if (FAILED(hr))
		return false;
	

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CPAHProject::Validate(CXMLDocument2 &XMLDoc2)
{
	m_szError = _T("");

	if (m_szId.IsEmpty())
		return false;

	// Validate project thumb
	if (!FileExists(m_szLocalThumbPath))
		m_szError += ERROR_THUMB;

	// Validate preview path 
	if (!FileExists(m_szLocalPreviewPath))
		m_szError += ERROR_PREVIEW;

	// Validate project Path
	if (!FileExists(m_szLocalProjectPath))
		m_szError += ERROR_PROJECT;

	CContentBase::Validate(XMLDoc2);


	return true;
}