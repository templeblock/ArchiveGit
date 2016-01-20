#pragma once

// For some reason, the 'new' debug macro gets redefined during imports and some includes
#pragma push_macro("new")
#import "msxml4.dll" named_guids raw_interfaces_only
#pragma pop_macro("new")

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

	MSXML2::IXMLDOMDocumentPtr GetDOMDocument() {return m_pDoc;}
	HRESULT SetFYSError();
	CString GetErrorMsg() { return m_strErrorMessage; }
	CString GetStringValue(LPCSTR szSrchPattern, LPCSTR szAttrib);

protected:
	HRESULT CheckHR(HRESULT hr, LPCSTR pstrErrorText);
	HRESULT ReportError(MSXML2::IXMLDOMParseError *pXMLError);
	HRESULT CheckLoad();
	HRESULT WalkTree(int iLevel, MSXML2::IXMLDOMNode* pNode);
	HRESULT WaitForCompletion();

	CString GetStringValue(LPCSTR szSrchPattern, LPCSTR szAttrib, MSXML2::IXMLDOMNode* pNode);
	CString GetStringValue(LPCSTR szAttrib, MSXML2::IXMLDOMNode* pNode);

protected:
	MSXML2::IXMLDOMDocumentPtr m_pDoc;
	XMLCALLBACKFUNC m_pCallbackFunction;
	LPARAM m_pCallbackParam;
	CString m_strErrorNode;
	CString m_strErrorMessage;
};
