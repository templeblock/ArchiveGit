#pragma once

#import "msxml.dll" named_guids raw_interfaces_only

typedef bool (__cdecl *XMLCALLBACKFUNC)(int iLevel, LPCSTR pNodeName, LPCSTR pAttrName, LPCSTR pValueString, LPARAM lParam);

class CXMLDocument  
{
public:
	CXMLDocument();
	virtual ~CXMLDocument();

public:
	HRESULT RegisterCallback(XMLCALLBACKFUNC pCallbackFunction, LPARAM lParam);
	HRESULT Load(CString strFileName, bool bAsync);
	HRESULT LoadString(CString strXML);
	HRESULT LoadStream(CString strFileName);
	HRESULT Save(CString strFileName);
	HRESULT Build();
	HRESULT PersistToMemory(PBYTE* ppBuffer, ULONG *pLen);
	HRESULT LoadFromMemory(PBYTE pData, ULONG ulLen);
	HRESULT WalkTree(CString& strSearchPattern);

	MSXML::IXMLDOMDocumentPtr GetDOMDocument() {return m_pDoc;}
	HRESULT SetFYSError();
	CString GetErrorMsg() { return m_strErrorMessage; }
	CString GetStringValue(LPCSTR szSrchPattern, LPCSTR szAttrib);

protected:
	HRESULT CheckHR(HRESULT hr, LPCSTR pstrErrorText);
	HRESULT ReportError(MSXML::IXMLDOMParseError *pXMLError);
	HRESULT CheckLoad();
	HRESULT WalkTree(int iLevel, MSXML::IXMLDOMNode* pNode);
	HRESULT WaitForCompletion();

	CString GetStringValue(LPCSTR szSrchPattern, LPCSTR szAttrib, MSXML::IXMLDOMNode* pNode);
	CString GetStringValue(LPCSTR szAttrib, MSXML::IXMLDOMNode* pNode);

protected:
	MSXML::IXMLDOMDocumentPtr m_pDoc;
	XMLCALLBACKFUNC m_pCallbackFunction;
	LPARAM m_pCallbackParam;
	CString m_strErrorNode;
	CString m_strErrorMessage;
};
