#pragma once
#include "contentbase.h"

static LPCTSTR TAG_VERSE			= _T("verse");
static LPCTSTR TAG_VERSEPARENT		= _T("verselist");

///////////////////////////////////////////////////////////////////////////////
class CVerse :
	public CContentBase
{
public:
	CVerse(LPCTSTR szFilePath, LPCTSTR szHost);
	virtual ~CVerse(void);

public:
	virtual bool ProcessElement(IXMLDOMElement * pElem, CString& szId);
	virtual bool Validate(CXMLDocument2 &XMLDoc2);


protected:
	virtual bool UpdateAttributes(CXMLDocument2 &XMLDoc2, CComPtr<IXMLDOMNode> &spNode);

protected:
	CString m_szRawText;
	CString m_szText;


};
