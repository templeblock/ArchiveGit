#pragma once
#include "Download.h"
#include "XMLDocument2.h"

static LPCTSTR ATTRIBUTE_ID				= _T("id");
static LPCTSTR ATTRIBUTE_NAME			= _T("name");
static LPCTSTR ATTRIBUTE_TYPE			= _T("type");
static LPCTSTR ATTRIBUTE_SEQ			= _T("seq");
static LPCTSTR ATTRIBUTE_KEYWORDS		= _T("keywords");
static LPCTSTR ATTRIBUTE_OCCASION		= _T("occasion");
static LPCTSTR ATTRIBUTE_THUMB			= _T("thumb");
static LPCTSTR ATTRIBUTE_PREVIEW		= _T("preview");
static LPCTSTR ATTRIBUTE_LIFESTYLE		= _T("lifestyle");
static LPCTSTR ATTRIBUTE_COLLECTIONPATH	= _T("collectionpath");
static LPCTSTR ATTRIBUTE_PROJECTPATH	= _T("projectpath");
static LPCTSTR ATTRIBUTE_ARTPATH		= _T("artpath");
static LPCTSTR ATTRIBUTE_LICENSE		= _T("license");
static LPCTSTR ATTRIBUTE_INSTRUCT		= _T("instructions");
static LPCTSTR ATTRIBUTE_DATE			= _T("date");
static LPCTSTR ATTRIBUTE_CREATOR		= _T("creator");
static LPCTSTR ATTRIBUTE_ERROR			= _T("error");



static LPCTSTR VALUE_OEM_AG				= _T("oem-ag");
static LPCTSTR VALUE_USER				= _T("user");

static LPCTSTR TAG_PROJECTREF			= _T("projectref");
static LPCTSTR TAG_ARTREF				= _T("artref");
static LPCTSTR TAG_VERSEREF				= _T("verseref");

static LPCTSTR ERROR_NONE				= _T("none");
static LPCTSTR ERROR_THUMB				= _T("thumb invalid;");
static LPCTSTR ERROR_PROJECT			= _T("project invalid;");
static LPCTSTR ERROR_PREVIEW			= _T("preview invalid;");
static LPCTSTR ERROR_COLLECTION			= _T("collection invalid;");
static LPCTSTR ERROR_LIFESTYLE			= _T("lifestyle invalid;");
static LPCTSTR ERROR_ART				= _T("art invalid;");
static LPCTSTR ERROR_VERSE				= _T("verse invalid;");





struct ITEMINFO
{
	CString szTagName;
	CString szSeq;
};
typedef std::map <CString, ITEMINFO> CONTAINERTYPE;
///////////////////////////////////////////////////////////////////////////////
class CContentBase
{
public:
	CContentBase(LPCTSTR szReposPath, LPCTSTR szHost);
	virtual ~CContentBase(void);

public:
	virtual bool ProcessElement(IXMLDOMElement * pElem, CString& szId)=0;
	virtual bool GetFilesToDownload(CDownload& downnload);
	virtual bool UpdateManifest(CXMLDocument2 &XMLDoc2);
	virtual bool Validate(CXMLDocument2 &XMLDoc2);



protected:
	virtual void Empty();
	virtual bool UpdateAttributes(CXMLDocument2 &XMLDoc2, CComPtr<IXMLDOMNode> &spNode);
	virtual bool UpdateReferences(CXMLDocument2 &XMLDoc2, CComPtr<IXMLDOMNode> &spNode);
	bool ReadReferences(IXMLDOMElement * pElem, LPCTSTR szTagName);
	bool UpdateLocalPaths();
	bool SetAttribute(IXMLDOMElement &XMLDOMElement, LPCTSTR szAttributeName, LPCTSTR szAttributeValue);
	bool FindElement(IXMLDOMElement &XMLDOMElement, LPCTSTR szSearchPattern, IXMLDOMNode ** pXMLFoundNode);
	bool Initialize();
	bool FileExists(LPCTSTR szFilePath);
	bool UpdateErrorField(CXMLDocument2 &XMLDoc2, LPCTSTR szError);



protected:
	static bool m_bFoldersCreated;
	CONTAINERTYPE m_Container;
	CComPtr<IXMLDOMNode> m_spXMLNode;
	CString m_szId;
	CString m_szName;
	CString m_szType;
	CString m_szSeq;
	CString m_szXML;
	CString m_szKeywords;
	CString m_szOccasion;
	CString m_szParentTag;
	CString m_szTagName;
	CString m_szDate;
	CString m_szHost;
	CString m_szLicense;
	CString m_szInstructions;
	CString m_szVersion;
	CString m_szCreator;
	CString m_szSourceThumbPath;
	CString m_szLocalThumbPath;
	CString m_szSourcePreviewPath;
	CString m_szLocalPreviewPath;
	CString m_szSourceLifeStylePath;
	CString m_szLocalLifeStylePath;
	CString m_szLocalCollectionPath;
	CString m_szSourceProjectPath;
	CString m_szLocalProjectPath;
	CString m_szSourceArtPath;
	CString m_szLocalArtPath;
	CString m_szRepositoryPath;
	CString m_szThumbsFolder;
	CString m_szPreviewFolder;
	CString m_szLifeStyleFolder;
	CString m_szCollectionsFolder;
	CString m_szProjectsFolder;
	CString m_szArtFolder;
	CString m_szError;
	

};
