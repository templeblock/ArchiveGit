#pragma once

#include "ContentBase.h"

static LPCTSTR TAG_PROJECT			= _T("project");
static LPCTSTR TAG_PROJECTPARENT	= _T("projectlist");

///////////////////////////////////////////////////////////////////////////////
class CPAHProject : public CContentBase
{
public:
	CPAHProject(LPCTSTR szFilePath, LPCTSTR szHost);
public:
	virtual ~CPAHProject(void);

protected:
	virtual bool UpdateAttributes(CXMLDocument2 &XMLDoc2, CComPtr<IXMLDOMNode> &spNode);

public:
	bool ProcessElement(IXMLDOMElement * pElem, CString& szId);
	bool LoadXML(CComBSTR& bstrXML);
	virtual bool Validate(CXMLDocument2 &XMLDoc2);


protected:
	

private:
	
};
