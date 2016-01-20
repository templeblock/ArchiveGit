#include "StdAfx.h"
#include "PAHCollection.h"

///////////////////////////////////////////////////////////////////////////////
CPAHCollection::CPAHCollection(LPCTSTR szFilePath, LPCTSTR szHost) : CContentBase(szFilePath, szHost)
{
	m_szParentTag			= TAG_COLLECTIONPARENT;
	m_szTagName				= TAG_COLLECTION;
}

CPAHCollection::~CPAHCollection(void)
{
	Empty();
}
///////////////////////////////////////////////////////////////////////////////
bool CPAHCollection::ProcessElement(IXMLDOMElement * pElem, CString& szId)
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
		// LifeStyle
		hr = spElem->getAttribute(CComBSTR(ATTRIBUTE_LIFESTYLE), &varValue);
		if (FAILED(hr))
			return false;
		m_szSourceLifeStylePath = m_szHost + CString(varValue.bstrVal);
		
		
		// Read all PROJECT References
		if (!ReadReferences(spElem, TAG_PROJECTREF))
			return false;

		// Read all ART References
		if (!ReadReferences(spElem, TAG_ARTREF))
			return false;

		// Read all VERSE References
		if (!ReadReferences(spElem, TAG_VERSEREF))
			return false;
		
	}
	catch(...)
	{
		return false;
	}

	if (!UpdateLocalPaths())
		return false;

	SaveXML(m_szLocalCollectionPath, m_szXML);

	return true;
}
///////////////////////////////////////////////////////////////////////////////
void CPAHCollection::Empty()
{
	m_Container.clear();
}
///////////////////////////////////////////////////////////////////////////////
bool CPAHCollection::UpdateAttributes(CXMLDocument2 &XMLDoc2, CComPtr<IXMLDOMNode> &spNode)
{
	CComQIPtr<IXMLDOMElement> spXMLElem(spNode);
	if (NULL == spXMLElem)
		return false;

	if (!CContentBase::UpdateAttributes(XMLDoc2, spNode))
		return false;

	HRESULT hr = XMLDoc2.SetAttribute(spXMLElem, ATTRIBUTE_LIFESTYLE, m_szLocalLifeStylePath);
	if (FAILED(hr))
		return false;

	hr = XMLDoc2.SetAttribute(spXMLElem, ATTRIBUTE_COLLECTIONPATH, m_szLocalCollectionPath);
	if (FAILED(hr))
		return false;


	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CPAHCollection::Validate(CXMLDocument2 &XMLDoc2)
{
	m_szError = _T("");

	if (m_szId.IsEmpty())
		return false;

	// Validate Collection thumb
	if (!FileExists(m_szLocalThumbPath))
		m_szError += ERROR_THUMB;

	// Validate Lifestyle image 
	if (!FileExists(m_szLocalLifeStylePath))
		m_szError += ERROR_LIFESTYLE;

	// Validate Collection Path
	if (!FileExists(m_szLocalCollectionPath))
		m_szError += ERROR_COLLECTION;

	
	CContentBase::Validate(XMLDoc2);

	return true;
}