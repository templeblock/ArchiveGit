#include "StdAfx.h"
#include "Verse.h"

///////////////////////////////////////////////////////////////////////////////
CVerse::CVerse(LPCTSTR szFilePath, LPCTSTR szHost) : CContentBase(szFilePath, szHost)
{
	m_szParentTag		= TAG_VERSEPARENT;
	m_szTagName			= TAG_VERSE;
}

CVerse::~CVerse(void)
{
}
///////////////////////////////////////////////////////////////////////////////
bool CVerse::ProcessElement(IXMLDOMElement * pElem, CString& szId)
{
	CComPtr<IXMLDOMElement> spElem(pElem);
	if (NULL == spElem)
		return false;

	if (!CContentBase::ProcessElement(spElem, szId))
		return false;

	try
	{
		// Text
		CComBSTR bstrText;
		HRESULT hr = spElem->get_text(&bstrText);
		if (FAILED(hr))
			return false;
		m_szText = CString(bstrText);

		// Raw Text
		CComBSTR bstrXML;
		CComPtr<IXMLDOMNode> spCDataNode;
		hr = spElem->get_firstChild(&spCDataNode);
		if (spCDataNode)
			hr = spCDataNode->get_xml(&bstrXML);
		if (FAILED(hr))
			return false;
		
		m_szRawText = bstrXML;
	
	}
	catch(...)
	{
		return false;
	}


	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CVerse::UpdateAttributes(CXMLDocument2 &XMLDoc2, CComPtr<IXMLDOMNode> &spNode)
{
	CComQIPtr<IXMLDOMElement> spXMLElem(spNode);
	if (NULL == spXMLElem)
		return false;

	HRESULT hr;
	if (!CContentBase::UpdateAttributes(XMLDoc2, spNode))
		return false;

	// Set Text - includes CDATA.
	CComBSTR bstrText;
	hr = spXMLElem->put_text(CComBSTR(m_szRawText));
	if (FAILED(hr))
		return false;

	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool CVerse::Validate(CXMLDocument2 &XMLDoc2)
{
	m_szError = _T("");

	if (m_szId.IsEmpty())
		return false;


	// Validate Verse text
	if (m_szText.IsEmpty())
		m_szError += ERROR_VERSE;

	CContentBase::Validate(XMLDoc2);

	return true;
}