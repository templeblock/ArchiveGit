#pragma once

typedef void (__cdecl *XMLCALLBACKFUNC)(int iLevel, LPCSTR pNameString, LPCSTR pValueString, LPARAM lParam);

class CXMLDocument  
{
public:
	CXMLDocument();
	virtual ~CXMLDocument();

public:
	HRESULT RegisterCallback(XMLCALLBACKFUNC pCallbackFunction, LPARAM lParam);
	HRESULT Load(CString strFileName, bool bAsync);
	HRESULT LoadStream(CString strFileName);
	HRESULT Save(CString strFileName);
	HRESULT Build();
	HRESULT PersistToMemory(PBYTE* ppBuffer, ULONG *pLen);
	HRESULT LoadFromMemory(PBYTE pData, ULONG ulLen);
	HRESULT WalkTree(CString& strSearchPattern);
	MSXML::IXMLDOMDocumentPtr GetDOMDocument() {return m_pDoc;}
protected:
	HRESULT CheckHR(HRESULT hr, LPCSTR pstrErrorText);
	HRESULT ReportError(MSXML::IXMLDOMParseError *pXMLError);
	HRESULT CheckLoad();
	HRESULT WalkTree(int iLevel, MSXML::IXMLDOMNode* pNode);
	HRESULT WaitForCompletion();

protected:
	MSXML::IXMLDOMDocumentPtr m_pDoc;
	XMLCALLBACKFUNC m_pCallbackFunction;
	LPARAM m_pCallbackParam;
};
