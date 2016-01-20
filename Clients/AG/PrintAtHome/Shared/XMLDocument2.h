#pragma once

typedef void (__cdecl *XMLCALLBACKFUNC)(int iLevel, bool bAttribute, LPCTSTR pNameString, LPCTSTR pValueString, LPARAM lParam);

class CXMLDocument2  
{
public:
	CXMLDocument2();
	virtual ~CXMLDocument2();

public:
	HRESULT RegisterCallback(XMLCALLBACKFUNC pCallbackFunction, LPARAM lParam);
	HRESULT Load(CString strFileName, bool bAsync=false);
	HRESULT LoadStream(CString strFileName);
	HRESULT Save(CString strFileName);
	HRESULT PersistToMemory(PBYTE* ppBuffer, ULONG *pLen);
	HRESULT LoadFromMemory(PBYTE pData, ULONG ulLen);
	HRESULT WalkTree(CString& strSearchPattern);
	IXMLDOMDocumentPtr GetDOMDocument() {return m_pDoc;}
	HRESULT SetAttribute(CComPtr<IXMLDOMElement> &spXMLDOMElement, LPCTSTR szAttributeName, LPCTSTR szAttributeValue);
	HRESULT FindElement(LPCTSTR szSearchPattern, IXMLDOMNode ** pXMLFoundNode);
	HRESULT AppendElement(LPCTSTR szTagName, CComPtr<IXMLDOMNode> &spXMLRefNode, CComPtr<IXMLDOMNode> &spOutXMLNode);


protected:
	HRESULT BuildSample();
	HRESULT CheckHR(HRESULT hr, LPCTSTR pstrErrorText);
	HRESULT ReportError(IXMLDOMParseError *pXMLError);
	HRESULT CheckLoad();
	HRESULT WalkTree(int iLevel, CComPtr<IXMLDOMNode> pNode);
	HRESULT WaitForCompletion();

protected:
	IXMLDOMDocumentPtr m_pDoc;
	XMLCALLBACKFUNC m_pCallbackFunction;
	LPARAM m_pCallbackParam;
};
