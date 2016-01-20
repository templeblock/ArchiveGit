//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/CDReq.h 1     3/03/99 6:04p Gbeddow $
// 
//  "This unpublished source code contains trade secrets which are the
//	property of Mindscape, Inc.  Unauthorized use, copying or distribution
//	is a violation of international laws and is strictly prohibited."
// 
//		  Copyright © 1998 Mindscape, Inc. All rights reserved.
//
//
// $Log: /PM8/App/CDReq.h $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 2     9/15/98 4:15p Psasse
// initial add of CD Requests for online graphics server
// 
// 1     9/15/98 3:07p Psasse
// 
// 
//////////////////////////////////////////////////////////////////////////////
#ifndef __CDREQ_H__
#define __CDREQ_H__

class CBasicRequestInfo;

class CCDInfo
{
private:

	CString m_strCDID;
	CString m_strName;
	CString m_strLocale;
	CString m_strDescription;
	CString m_strAvailabilityDate;
	CString m_strPrice;

public:

	const CString& GetCDID (void)
	{  return m_strCDID;}
	void SetCDID (const CString& str)
	{  m_strCDID = str;}

	const CString& GetName (void)
	{  return m_strName;}
	void SetName (const CString& str)
	{  m_strName = str;}

	const CString& GetLocale (void)
	{  return m_strLocale;}
	void SetLocale (const CString& str)
	{  m_strLocale = str;}

	const CString& GetDescription (void)
	{  return m_strDescription;}
	void SetDescription (const CString& str)
	{  m_strDescription = str;}

	const CString& GetAvailabilityDate (void)
	{  return m_strAvailabilityDate;}
	void SetAvailabilityDate (const CString& str)
	{  m_strAvailabilityDate = str;}

	const CString& GetPrice (void)
	{  return m_strPrice;}
	void SetPrice (const CString& str)
	{  m_strPrice = str;}

};

/////////////////////////////////////////////////////////////////////////////
// CGetCDInfoRequest command target

class CGetCDInfoRequest : public CBasicRequestInfo
{
	DECLARE_DYNCREATE(CGetCDInfoRequest)

	CGetCDInfoRequest();           // protected constructor used by dynamic creation
	CGetCDInfoRequest(CString& strItemId);   

// Attributes
public:

protected:
	CString m_strCDID;
	CCDInfo* m_pCDInfo;

	// Operations
public:
	CString& GetCDID (void)
	{  return m_strCDID;}
	void SetCDID (const CString& str)
	{  m_strCDID = str;}

	CCDInfo* GetCDInfo (void)
	{  return m_pCDInfo;}
	void SetCDInfo (CCDInfo* pInfo)
	{  m_pCDInfo = pInfo;}
	

// Implementation
public:
	virtual ~CGetCDInfoRequest();
	void ProcessRequest(CString& pString);
	UINT ProcessResponse(CHttpFile& pHttpFile);

	void FillCDParams(CString& strList, const char* seps);

	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CGetCDListRequest command target

class CGetCDListRequest : public CBasicRequestInfo
{
	DECLARE_DYNCREATE(CGetCDListRequest)

	CGetCDListRequest();           // protected constructor used by dynamic creation
	CGetCDListRequest(CString& strItemId);   

// Attributes
public:

protected:
	CStringList m_strList;
	CString m_strIgnoreLocale;
	CString m_strSelector;
	
	CCDInfo* m_pCDInfo;
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

	CStringList& GetCDList (void)
	{  return m_strList;}

	CCDInfo* GetCDInfo (void)
	{  return m_pCDInfo;}

// Implementation
public:
	virtual ~CGetCDListRequest();
	void ProcessRequest(CString& pString);
	UINT ProcessResponse(CHttpFile& pHttpFile);
	void FillCollParams(CString& strList, const char* seps);

	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMarkCDSeenRequest command target

class CMarkCDSeenRequest : public CBasicRequestInfo
{
	DECLARE_DYNCREATE(CMarkCDSeenRequest)

	CMarkCDSeenRequest();           // protected constructor used by dynamic creation
	CMarkCDSeenRequest(CString& strItemId);   

// Attributes
public:

protected:
	CString m_strCDID;

	// Operations
public:
	
// Implementation
public:
	virtual ~CMarkCDSeenRequest();
	void ProcessRequest(CString& pString);
	UINT ProcessResponse(CHttpFile& pHttpFile);

	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPriceCDOrderRequest command target

class CPriceCDOrderRequest : public CBasicRequestInfo
{
	DECLARE_DYNCREATE(CPriceCDOrderRequest)

	CPriceCDOrderRequest();           // protected constructor used by dynamic creation
	CPriceCDOrderRequest(LPCSTR strItemId);   

// Attributes
public:

protected:
	CString m_strCDIDList;
	CString m_strShipMethod;
	CString m_strStreet1;
	CString m_strStreet2;
	CString m_strCity;
	CString m_strState;
	CString m_strCountry;
	CString m_strZip;

	CString m_strItemTotal;
	CString m_strDiscount;
	CString m_strShipping;
	CString m_strHandling;
	CString m_strSalesTax;
	CString m_strOrderTotal;

	// Operations
public:

	CString& GetCDIDList (void)
	{  return m_strCDIDList;}
	void SetCDIDList (const CString& str)
	{  m_strCDIDList = str;}

	CString& GetShipMethod (void)
	{  return m_strShipMethod;}
	void SetShipMethod (const CString& str)
	{  m_strShipMethod = str;}

	CString& GetStreet1 (void)
	{  return m_strStreet1;}
	void SetStreet1 (const CString& str)
	{  m_strStreet1 = str;}

	CString& GetStreet2 (void)
	{  return m_strStreet2;}
	void SetStreet2 (const CString& str)
	{  m_strStreet2 = str;}

	CString& GetCity (void)
	{  return m_strCity;}
	void SetCity (const CString& str)
	{  m_strCity = str;}

	CString& GetCountry (void)
	{  return m_strCountry;}
	void SetCountry (const CString& str)
	{  m_strCountry = str;}

	CString& GetState (void)
	{  return m_strState;}
	void SetState (const CString& str)
	{  m_strState = str;}

	CString& GetZip (void)
	{  return m_strZip;}
	void SetZip (const CString& str)
	{  m_strZip = str;}

	
// Implementation
public:
	virtual ~CPriceCDOrderRequest();
	void ProcessRequest(CString& pString);
	UINT ProcessResponse(CHttpFile& pHttpFile);

	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPurchaseCDRequest command target

class CPurchaseCDRequest : public CBasicRequestInfo
{
	DECLARE_DYNCREATE(CPurchaseCDRequest)

	CPurchaseCDRequest();           // protected constructor used by dynamic creation
	CPurchaseCDRequest(LPCSTR strItemId);   

// Attributes
public:

protected:
	CString m_strCDIDList;
	CString m_strAmount;
	CString m_strShipMethod;
	CString m_strRecipientName;
	CString m_strStreet1;
	CString m_strStreet2;
	CString m_strCity;
	CString m_strState;
	CString m_strCountry;
	CString m_strZip;
	CString m_strCardType;
	CString m_strCardNumber;
	CString m_strExpirationDate;
	CString m_strCardHolder;
	CString m_strCardStreet1;
	CString m_strCardStreet2;
	CString m_strCardCity;
	CString m_strCardState;
	CString m_strCardCountry;
	CString m_strCardZip;

	// Operations
public:
	CString& GetCDIDList (void)
	{  return m_strCDIDList;}
	void SetCDIDList (const CString& str)
	{  m_strCDIDList = str;}

	CString& GetAmount (void)
	{  return m_strAmount;}
	void SetAmount (const CString& str)
	{  m_strAmount = str;}

	CString& GetShipMethod (void)
	{  return m_strShipMethod;}
	void SetShipMethod (const CString& str)
	{  m_strShipMethod = str;}

	CString& GetRecipientName (void)
	{  return m_strRecipientName;}
	void SetRecipientName (const CString& str)
	{  m_strRecipientName = str;}

	CString& GetStreet1 (void)
	{  return m_strStreet1;}
	void SetStreet1 (const CString& str)
	{  m_strStreet1 = str;}

	CString& GetStreet2 (void)
	{  return m_strStreet2;}
	void SetStreet2 (const CString& str)
	{  m_strStreet2 = str;}

	CString& GetCity (void)
	{  return m_strCity;}
	void SetCity (const CString& str)
	{  m_strCity = str;}

	CString& GetState (void)
	{  return m_strState;}
	void SetState (const CString& str)
	{  m_strState = str;}

	CString& GetCountry (void)
	{  return m_strCountry;}
	void SetCountry (const CString& str)
	{  m_strCountry = str;}

	CString& GetZip (void)
	{  return m_strZip;}
	void SetZip (const CString& str)
	{  m_strZip = str;}

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

	CString& GetCardState (void)
	{  return m_strCardState;}
	void SetCardState (const CString& str)
	{  m_strCardState = str;}

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
	virtual ~CPurchaseCDRequest();
	void ProcessRequest(CString& pString);
	UINT ProcessResponse(CHttpFile& pHttpFile);

	//DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif