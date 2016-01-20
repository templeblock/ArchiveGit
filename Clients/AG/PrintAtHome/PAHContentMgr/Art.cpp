#include "StdAfx.h"
#include "Art.h"

///////////////////////////////////////////////////////////////////////////////
CArt::CArt(LPCTSTR szFilePath, LPCTSTR szHost) : CContentBase(szFilePath, szHost)
{
	m_szParentTag		= TAG_ARTPARENT;
	m_szTagName			= TAG_ART;
}

CArt::~CArt(void)
{
}
///////////////////////////////////////////////////////////////////////////////
bool CArt::ProcessElement(IXMLDOMElement * pElem, CString& szId)
{
	CComPtr<IXMLDOMElement> spElem(pElem);
	if (NULL == spElem)
		return false;

	if (!CContentBase::ProcessElement(spElem, szId))
		return false;

	try
	{
		// Art Path
		VARIANT varValue;
		HRESULT hr = spElem->getAttribute(CComBSTR(ATTRIBUTE_ARTPATH), &varValue);
		if (FAILED(hr))
			return false;
		m_szSourceArtPath = m_szHost + CString(varValue.bstrVal);
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
bool CArt::UpdateAttributes(CXMLDocument2 &XMLDoc2, CComPtr<IXMLDOMNode> &spNode)
{
	CComQIPtr<IXMLDOMElement> spXMLElem(spNode);
	if (NULL == spXMLElem)
		return false;

	HRESULT hr;
	if (!CContentBase::UpdateAttributes(XMLDoc2, spNode))
		return false;

	hr = XMLDoc2.SetAttribute(spXMLElem, ATTRIBUTE_ARTPATH, m_szLocalArtPath);
	if (FAILED(hr))
		return false;

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CArt::Validate(CXMLDocument2 &XMLDoc2)
{
	m_szError = _T("");

	if (m_szId.IsEmpty())
		return false;

	// Validate art thumb
	if (!FileExists(m_szLocalThumbPath))
		m_szError += ERROR_THUMB;


	// Validate art Path
	if (!FileExists(m_szLocalArtPath))
		m_szError += ERROR_ART;

	CContentBase::Validate(XMLDoc2);

	return true;
}