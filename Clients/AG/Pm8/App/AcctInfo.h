#if !defined(AFX_ACCOUNTINFO_H__4ECA5A02_BB35_11D1_9909_00A0240C7400__INCLUDED_)
#define AFX_ACCOUNTINFO_H__4ECA5A02_BB35_11D1_9909_00A0240C7400__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AccountInfo.h : header file
//

class CBasicRequestInfo;
/////////////////////////////////////////////////////////////////////////////
// CAccountInfo command target

class CAccountInfo : public CBasicRequestInfo
{
	DECLARE_DYNCREATE(CAccountInfo)

	CAccountInfo();           // protected constructor used by dynamic creation
	CAccountInfo(BOOL fUpdate);     

// Attributes
public:

protected:
	CString  m_strRequestedAccountID;
	CString  m_strServerSuggestedAccountID;
	CString  m_strCustLastName;
	CString  m_strCustFirstName;
	CString  m_strStreet1;
	CString  m_strStreet2;
	CString  m_strCity;
	CString  m_strState;
	CString  m_strCountry;
	CString  m_strZip;
	CString  m_strEMailAddr;
	CString  m_strPhone;

	BOOL	 m_fUpdate;
// Operations
public:

	CString& GetCustLastName (void)
	{  return m_strCustLastName;}
	void SetCustLastName (const CString& NewString)
	{  m_strCustLastName = NewString;}
	
	CString& GetCustFirstName (void)
	{  return m_strCustFirstName;}
	void SetCustFirstName (const CString& NewString)
	{  m_strCustFirstName = NewString;}

	CString& GetStreet1 (void)
	{  return m_strStreet1;}
	void SetStreet1 (const CString& NewString)
	{  m_strStreet1 = NewString;}

	CString& GetStreet2 (void)
	{  return m_strStreet2;}
	void SetStreet2 (const CString& NewString)
	{  m_strStreet2 = NewString;}

	CString& GetCity (void)
	{  return m_strCity;}
	void SetCity (const CString& NewString)
	{  m_strCity = NewString;}

	CString& GetState (void)
	{  return m_strState;}
	void SetState (const CString& NewString)
	{  m_strState = NewString;}

	CString& GetCountry (void)
	{  return m_strCountry;}
	void SetCountry (const CString& NewString)
	{  m_strCountry = NewString;}

	CString& GetZipCode (void)
	{  return m_strZip;}
	void SetZipCode (const CString& NewString)
	{  m_strZip = NewString;}

	CString& GetEMail (void)
	{  return m_strEMailAddr;}
	void SetEMail (const CString& NewString)
	{  m_strEMailAddr = NewString;}

	CString& GetPhone (void)
	{  return m_strPhone;}
	void SetPhone (const CString& NewString)
	{  m_strPhone = NewString;}

	CString& GetRequestedAccountID (void)
	{  return m_strRequestedAccountID;}
	void SetRequestedAccountID (const CString& NewString)
	{  m_strRequestedAccountID = NewString;}

	CString& GetServerSuggestedAccountID (void)
	{  return m_strServerSuggestedAccountID;}
	void SetServerSuggestedAccountID (const CString& NewString)
	{  m_strServerSuggestedAccountID = NewString;}

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAccountInfo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAccountInfo();
	UINT ProcessResponse(CHttpFile& pHttpFile);
    void ProcessRequest(CString& pString);
	//UINT DoRequest();

	// Generated message map functions
	//{{AFX_MSG(CAccountInfo)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


class CGetAccountInfoRequest : public CAccountInfo
{
	DECLARE_DYNCREATE(CGetAccountInfoRequest)

	CGetAccountInfoRequest();           // protected constructor used by dynamic creation

// Attributes
public:

protected:
// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetAccountInfoRequest)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGetAccountInfoRequest();
	UINT ProcessResponse(CHttpFile& pHttpFile);
    void ProcessRequest(CString& pString);
	//UINT DoRequest();

};

class CUpdateAccountRequest : public CAccountInfo
{
	DECLARE_DYNCREATE(CUpdateAccountRequest)

	CUpdateAccountRequest();           // protected constructor used by dynamic creation

// Attributes
public:

protected:
	CString m_strNewPassword;

// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUpdateAccountRequest)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUpdateAccountRequest();
	UINT ProcessResponse(CHttpFile& pHttpFile);
    void ProcessRequest(CString& pString);
	//UINT DoRequest();

	CString& GetNewPassword (void)
	{  return m_strNewPassword;}
	void SetNewPassword (const CString& NewString)
	{  m_strNewPassword = NewString;}
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCOUNTINFO_H__4ECA5A02_BB35_11D1_9909_00A0240C7400__INCLUDED_)

