#pragma once
#include "contentbase.h"

static LPCTSTR TAG_ART			= _T("art");
static LPCTSTR TAG_ARTPARENT	= _T("artlist");

///////////////////////////////////////////////////////////////////////////////
class CArt :
	public CContentBase
{
public:
	CArt(LPCTSTR szFilePath, LPCTSTR szHost);
	virtual ~CArt(void);

public:
	virtual bool ProcessElement(IXMLDOMElement * pElem, CString& szId);
	virtual bool Validate(CXMLDocument2 &XMLDoc2);


protected:
	virtual bool UpdateAttributes(CXMLDocument2 &XMLDoc2, CComPtr<IXMLDOMNode> &spNode);
};
