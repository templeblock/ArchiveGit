#pragma once

#include "ContentBase.h"
#include "PAHProject.h"

static LPCTSTR TAG_COLLECTION			= _T("collection");
static LPCTSTR TAG_COLLECTIONPARENT		= _T("collectionlist");


///////////////////////////////////////////////////////////////////////////////
class CPAHCollection : public CContentBase
{
public:
	CPAHCollection(LPCTSTR szFilePath, LPCTSTR szHost);
	virtual ~CPAHCollection(void);

public:
	bool ProcessElement(IXMLDOMElement * pElem, CString& szId);
	virtual bool Validate(CXMLDocument2 &XMLDoc2);


protected:
	virtual bool UpdateAttributes(CXMLDocument2 &XMLDoc2, CComPtr<IXMLDOMNode> &spNode);
	void Empty();

private:
	
};
