//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/CollReq.h 1     3/03/99 6:04p Gbeddow $
// 
//  "This unpublished source code contains trade secrets which are the
//	property of Mindscape, Inc.  Unauthorized use, copying or distribution
//	is a violation of international laws and is strictly prohibited."
// 
//		  Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/CollReq.h $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 1     9/15/98 4:18p Psasse
// initial add of Collection Requests for online graphics server
// 
// 
//////////////////////////////////////////////////////////////////////////////
#ifndef __COLLREQ_H__
#define __COLLREQ_H__

class CBasicRequestInfo;

class CCollectionInfo
{
private:

	CString m_strCollID;
	CString m_strName;
	CString m_strFileName;
	CString m_strLocale;
	CString m_strDescription;
	CString m_strPrice;
	CString m_strIconFileName;
	CString m_strBrochureFileName;

public:

	const CString& GetCollID (void)
	{  return m_strCollID;}
	void SetCollID (const CString& str)
	{  m_strCollID = str;}

	const CString& GetName (void)
	{  return m_strName;}
	void SetName (const CString& str)
	{  m_strName = str;}

	const CString& GetFileName (void)
	{  return m_strFileName;}
	void SetFileName (const CString& str)
	{  m_strFileName = str;}

	const CString& GetLocale (void)
	{  return m_strLocale;}
	void SetLocale (const CString& str)
	{  m_strLocale = str;}

	const CString& GetDescription (void)
	{  return m_strDescription;}
	void SetDescription (const CString& str)
	{  m_strDescription = str;}

	const CString& GetPrice (void)
	{  return m_strPrice;}
	void SetPrice (const CString& str)
	{  m_strPrice = str;}

	const CString& GetIconFileName (void)
	{  return m_strIconFileName;}
	void SetIconFileName (const CString& str)
	{  m_strIconFileName = str;}

	const CString& GetBrochureFileName (void)
	{  return m_strBrochureFileName;}
	void SetBrochureFileName (const CString& str)
	{  m_strBrochureFileName = str;}


};

/////////////////////////////////////////////////////////////////////////////
// CGetCollLocsListRequest command target

class CGetCollInfoRequest : public CBasicRequestInfo
{
	DECLARE_DYNCREATE(CGetCollInfoRequest)

	CGetCollInfoRequest();           // protected constructor used by dynamic creation
	CGetCollInfoRequest(CString& strItemId);   

// Attributes
public:

protected:
	CString m_strCollID;
	CCollectionInfo* m_pCollInfo;

	// Operations
public:
	CString& GetCollID (void)
	{  return m_strCollID;}
	void SetCollID (const CString& str)
	{  m_strCollID = str;}

	CCollectionInfo* GetCollInfo (void)
	{  return m_pCollInfo;}
	void SetCollInfo (CCollectionInfo* pInfo)
	{  m_pCollInfo = pInfo;}

	

// Implementation
public:
	virtual ~CGetCollInfoRequest();
	void ProcessRequest(CString& pString);
	UINT ProcessResponse(CHttpFile& pHttpFile);

	void FillCollParams(CString& strList, const char* seps);

	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CGetCollLocRequest command target

class CGetCollListRequest : public CBasicRequestInfo
{
	DECLARE_DYNCREATE(CGetCollListRequest)

	CGetCollListRequest();           // protected constructor used by dynamic creation
	CGetCollListRequest(CString& strItemId);   

// Attributes
public:

protected:
	CStringList m_strList;
	CString m_strIgnoreLocale;
	CString m_strSelector;
	
	CCollectionInfo* m_pCollInfo;
	// Operations
public:
	
	const CString& GetSelector (void)
	{  return m_strSelector;}
	void SetSelector (const CString& str)
	{  m_strSelector = str;}

	const CString& GetIgnoreLocale (void)
	{  return m_strIgnoreLocale;}
	void SetIgnoreLocale (const CString& str)
	{  m_strIgnoreLocale = str;}

	const CStringList* GetCollList (void)
	{  return &m_strList;}

	CCollectionInfo* GetCollInfo (void)
	{  return m_pCollInfo;}

// Implementation
public:
	virtual ~CGetCollListRequest();
	void ProcessRequest(CString& pString);
	UINT ProcessResponse(CHttpFile& pHttpFile);
	void FillCollParams(CString& strList, const char* seps);
	void FillListFromDelimitedString(CString& strList, const char* seps, CStringList& strServerList);

	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMarkCollSeenRequest command target

class CMarkCollSeenRequest : public CBasicRequestInfo
{
	DECLARE_DYNCREATE(CMarkCollSeenRequest)

	CMarkCollSeenRequest();           // protected constructor used by dynamic creation
	CMarkCollSeenRequest(CString& strItemId);   

// Attributes
public:

protected:
	CString m_strCollID;

	// Operations
public:
	
// Implementation
public:
	virtual ~CMarkCollSeenRequest();
	void ProcessRequest(CString& pString);
	UINT ProcessResponse(CHttpFile& pHttpFile);

	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPurchaseCollectionRequest command target

class CPurchaseCollectionRequest : public CBasicRequestInfo
{
	DECLARE_DYNCREATE(CPurchaseCollectionRequest)

	CPurchaseCollectionRequest();           // protected constructor used by dynamic creation
	CPurchaseCollectionRequest(CString& strItemId);   

// Attributes
public:

protected:
	CString m_strCollIDList;
	CString m_strAmount;
	CString m_strCardType;
	CString m_strCardNumber;
	CString m_strExpirationDate;
	CString m_strCardHolder;
	CString m_strCardStreet1;
	CString m_strCardStreet2;
	CString m_strCardCity;
	CString m_strCardCountry;
	CString m_strCardZip;

	// Operations
public:
	
	CString& GetCollIDList (void)
	{  return m_strCollIDList;}
	void SetCollIDList (const CString& str)
	{  m_strCollIDList = str;}

	CString& GetAmount (void)
	{  return m_strAmount;}
	void SetAmount (const CString& str)
	{  m_strAmount = str;}

	CString& GetCardType (void)
	{  return m_strCardType;}
	void SetCardType (const CString& str)
	{  m_strCardType = str;}

	CString& GetCardNumber (void)
	{  return m_strCardNumber;}
	void SetCardNumber (const CString& str)
	{  m_strCardNumber = str;}

	CString& GetExpirationDate (void)
	{  return m_strExpirationDate;}
	void SetExpirationDate (const CString& str)
	{  m_strExpirationDate = str;}

	CString& GetCardHolder (void)
	{  return m_strCardHolder;}
	void SetCardHolder (const CString& str)
	{  m_strCardHolder = str;}

	CString& GetCardStreet1 (void)
	{  return m_strCardStreet1;}
	void SetCardStreet1 (const CString& str)
	{  m_strCardStreet1 = str;}

	CString& GetCardStreet2 (void)
	{  return m_strCardStreet2;}
	void SetCardStreet2 (const CString& str)
	{  m_strCardStreet2 = str;}

	CString& GetCardCity (void)
	{  return m_strCardCity;}
	void SetCardCity (const CString& str)
	{  m_strCardCity = str;}

	CString& GetCardCountry (void)
	{  return m_strCardCountry;}
	void SetCardCountry (const CString& str)
	{  m_strCardCountry = str;}

	CString& GetCardZip (void)
	{  return m_strCardZip;}
	void SetCardZip (const CString& str)
	{  m_strCardZip = str;}


// Implementation
public:
	virtual ~CPurchaseCollectionRequest();
	void ProcessRequest(CString& pString);
	UINT ProcessResponse(CHttpFile& pHttpFile);

	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif