#if !defined(AFX_BASICREQUESTINFO_H__4ECA5A03_BB35_11D1_9909_00A0240C7400__INCLUDED_)
#define AFX_BASICREQUESTINFO_H__4ECA5A03_BB35_11D1_9909_00A0240C7400__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// BasicRequestInfo.h : header file
//

class CHttpFile;
class CHttpConnection;

/////////////////////////////////////////////////////////////////////////////
// CBasicRequestInfo command target

class CBasicRequestInfo : public CCmdTarget
{
	DECLARE_DYNCREATE(CBasicRequestInfo)

	CBasicRequestInfo();           // protected constructor used by dynamic creation

// Attributes
public:
    //CBasicRequestInfo(CHttpConnection* pHttpConnection);

protected:
	CString m_strOperation;
	CString	m_strAccountID;
	CString m_strPassword;
	CString m_strVerifyPassword;
	CString m_strClientSKU;
	CString m_strClientVersion;
	CString m_strClientPGSVersion;
	CString m_strClientLocale;
	
	CString m_strSessionID;
	CString m_strRequestID;

	CHttpConnection* m_pHttpConnection;
	CHttpFile*		 m_pHttpFile;

	CString m_strBaseServerUrlAddress;
	CString m_strServerUrlAddress;
	CString m_strStatusCode;

	CString m_strHeaders;
	BOOL m_fUnknownItemCheck;

// Operations
public:

	CString& GetOperation ( void )
	{  return m_strOperation;}
	void SetOperation ( const CString& NewID )
	{  m_strOperation = NewID;}
	
	CString& GetSessionID ( void )
	{  return m_strSessionID;}
	void SetSessionID ( const CString& NewID )
	{  m_strSessionID = NewID;}
	
	CString& GetRequestID ( void )
	{  return m_strRequestID;}
	void SetRequestID ( const CString& NewID )
	{  m_strRequestID = NewID;}

	CString& GetAccountID ( void )
	{  return m_strAccountID;}
	void SetAccountID ( const CString& NewID )
	{  m_strAccountID = NewID;}

	CString& GetPassword (void)
	{  return m_strPassword;}
	void SetPassword (const CString& NewString)
	{  m_strPassword = NewString;}

	CString& GetVerifyPassword (void)
	{  return m_strVerifyPassword;}
	void SetVerifyPassword (const CString& NewString)
	{  m_strVerifyPassword = NewString;}

	CString& GetClientSKU (void)
	{  return m_strClientSKU;}
	void SetClientSKU (const CString& NewString)
	{  m_strClientSKU = NewString;}

	CString& GetClientVersion (void)
	{  return m_strClientVersion;}
	void SetClientVersion (const CString& NewString)
	{  m_strClientVersion = NewString;}

	CString& GetClientPGSVersion (void)
	{  return m_strClientPGSVersion;}
	void SetClientPGSVersion (const CString& NewString)
	{  m_strClientPGSVersion = NewString;}

	CString& GetClientLocale (void)
	{  return m_strClientLocale;}
	void SetClientLocale (const CString& NewString)
	{  m_strClientLocale = NewString;}

	CHttpConnection* GetHttpConnection (void)
	{  return m_pHttpConnection;}
	void SetHttpConnection (CHttpConnection* pNewConnection)
	{  m_pHttpConnection = pNewConnection;}

	CHttpFile* GetHttpFile (void)
	{  return m_pHttpFile;}
	void SetHttpFile (CHttpFile* NewFile)
	{  m_pHttpFile = NewFile;}

	CString& GetServerUrlAddress (void)
	{  return m_strServerUrlAddress;}
	void SetServerUrlAddress (const CString& NewString)
	{  m_strServerUrlAddress = NewString;}

	CString& GetBaseServerUrlAddress (void)
	{  return m_strBaseServerUrlAddress;}
	void SetBaseServerUrlAddress (const CString& NewString)
	{  m_strBaseServerUrlAddress = NewString;}

	CString& GetStatusCode (void)
	{  return m_strStatusCode;}
	void SetStatusCode (const CString& NewString)
	{  m_strStatusCode = NewString;}

	CString& GetHeaders (void)
	{  return m_strHeaders;}
	void SetHeaders (const CString& NewString)
	{  m_strHeaders = NewString;}

	BOOL GetUnknownItemCheck (void)
	{  return m_fUnknownItemCheck;}
	void SetUnknownItemCheck (const BOOL NewCheck)
	{  m_fUnknownItemCheck = NewCheck;}

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBasicRequestInfo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBasicRequestInfo();
	virtual void AddParameters();
	virtual UINT ProcessResponse(CHttpFile&);
	virtual UINT ProcessResponse(CString& strBuffer);
	virtual UINT DoRequest();
	virtual void ProcessRequest(CString&);
	virtual void ParseFileForValue(CString&, CString&, CString&);
    virtual void ParseFileForLineFeeds(CString&, CString&, CString&);
	virtual void FillListFromDelimitedString(CString& strList, const char* seps, CStringList& strServerList);
	virtual void ProcessStatusCode(int);
    virtual void UrlEncodeString(CString& strBuffer);

	virtual UINT UUEncodeHeaders(CString& pstrLoginID, CString& pstrPassword, CString& pstrHeaders);
	virtual BOOL UseGETMethod();

protected:

	// Generated message map functions
	//{{AFX_MSG(CBasicRequestInfo)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASICREQUESTINFO_H__4ECA5A03_BB35_11D1_9909_00A0240C7400__INCLUDED_)
