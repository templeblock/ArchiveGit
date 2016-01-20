#pragma once

#import "msxml4.dll" named_guids

class CXmlNodeWrapper  
{
public:
	CString NodeType();
	CString GetAttribVal(int index);
	CString GetAttribName(int index);
	int NumAttributes();
	void ReplaceNode(MSXML2::IXMLDOMNode* pOldNode,MSXML2::IXMLDOMNode* pNewNode);
	CString GetText();
	void SetText(LPCTSTR text);
	MSXML2::IXMLDOMNode* InsertAfter(MSXML2::IXMLDOMNode *refNode, MSXML2::IXMLDOMNode *pNode);
	CString Name();
	MSXML2::IXMLDOMNodeList* FindNodes(LPCTSTR searchStr);
	MSXML2::IXMLDOMNode* Parent();
	void RemoveNodes(LPCTSTR searchStr);
	MSXML2::IXMLDOMNode* InsertAfter(MSXML2::IXMLDOMNode* refNode, LPCTSTR nodeName);
	MSXML2::IXMLDOMNode* InsertBefore(MSXML2::IXMLDOMNode* refNode, LPCTSTR nodeName);
	MSXML2::IXMLDOMNode* Interface();
	MSXML2::IXMLDOMDocument* ParentDocument();
	CString GetXML();
	MSXML2::IXMLDOMNode* RemoveNode(MSXML2::IXMLDOMNodePtr pNode);
	MSXML2::IXMLDOMNode* InsertNode(int index,LPCTSTR nodeName);
	MSXML2::IXMLDOMNode* InsertNode(int index,MSXML2::IXMLDOMNodePtr pNode);
	long NumNodes();
	MSXML2::IXMLDOMNode* Detach();
	MSXML2::IXMLDOMNode* GetNode(LPCTSTR nodeName);
	MSXML2::IXMLDOMNode* GetNode(int nodeIndex);
	MSXML2::IXMLDOMNode* FindNode(LPCTSTR searchString);
	MSXML2::IXMLDOMNode* GetPrevSibling();
	MSXML2::IXMLDOMNode* GetNextSibling();
	void SetValue(LPCTSTR valueName,LPCTSTR value);
	void SetValue(LPCTSTR valueName,int value);
	void SetValue(LPCTSTR valueName,short value);
	void SetValue(LPCTSTR valueName,double value);
	void SetValue(LPCTSTR valueName,float value);
	void SetValue(LPCTSTR valueName,bool value);
	BOOL IsValid();
	CString GetValue(LPCTSTR valueName);

	CXmlNodeWrapper();
	CXmlNodeWrapper(MSXML2::IXMLDOMNodePtr pNode,BOOL bAutoRelease = TRUE);
	void operator=(MSXML2::IXMLDOMNodePtr pNode);
	virtual ~CXmlNodeWrapper();
private:
	BOOL m_bAutoRelease;
	MSXML2::IXMLDOMNodePtr m_xmlnode;
};

class CXmlDocumentWrapper
{
public:
	CString GetUrl();
	CString GetXML();
	BOOL IsValid();
	BOOL Load(LPCTSTR path);
	BOOL LoadXML(LPCTSTR xml);
	BOOL Save(LPCTSTR path = "");
	MSXML2::IXMLDOMDocument* Detach();
	MSXML2::IXMLDOMDocument* Clone();
	CXmlDocumentWrapper();
	CXmlDocumentWrapper(MSXML2::IXMLDOMDocumentPtr pDoc);
	MSXML2::IXMLDOMNode* AsNode();
	virtual ~CXmlDocumentWrapper();
private:
	MSXML2::IXMLDOMDocumentPtr m_xmldoc;
};

class CXmlNodeListWrapper
{
public:
	MSXML2::IXMLDOMDocument* AsDocument();
	MSXML2::IXMLDOMNode* Node(int index);
	void Start();
	MSXML2::IXMLDOMNode* Next();
	BOOL IsValid();
	int Count();
	CXmlNodeListWrapper();
	CXmlNodeListWrapper(MSXML2::IXMLDOMNodeListPtr pList);
	void operator=(MSXML2::IXMLDOMNodeListPtr pList);
	virtual ~CXmlNodeListWrapper();

private:
	MSXML2::IXMLDOMNodeListPtr m_xmlnodelist;
};
