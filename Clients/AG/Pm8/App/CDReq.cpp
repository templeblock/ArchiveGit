// CDReq.cpp : implementation file
//

#include "stdafx.h"
#include "pmwinet.h"
#include "pmw.h"
#include "BasReqIn.h"
#include "CDReq.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetCDListRequest

IMPLEMENT_DYNCREATE(CGetCDListRequest, CBasicRequestInfo)

CGetCDListRequest::CGetCDListRequest()
{
	m_strSelector = "all";
	m_strList.RemoveAll();
	m_strIgnoreLocale = "F";
}

CGetCDListRequest::CGetCDListRequest(CString& strItemId)
{
	m_strSelector = "all";
	m_strList.RemoveAll();
	m_strIgnoreLocale = "F";
}

CGetCDListRequest::~CGetCDListRequest()
{
	m_strList.RemoveAll();
}

void CGetCDListRequest::ProcessRequest(CString& pString)
{
	CString strChange = GetServerUrlAddress();
	strChange += "/GetCDList";
	SetServerUrlAddress(strChange);

	CBasicRequestInfo::ProcessRequest(pString);

	CString str;

	str.Format("&List=%s", m_strSelector);
	pString += str;

	str.Format("&IgnoreLocale=%s", m_strIgnoreLocale);
	pString += str;
}

UINT CGetCDListRequest::ProcessResponse(CHttpFile& pHttpFile)
{
#ifdef ARTSTORE
	return 1;
#else	//Take the HttpFile and parse it.
	UINT nFileLength = pHttpFile.GetLength();
	
	CString strBuffer;
	CString strSearch;
	CString strList;
	LPSTR b = strBuffer.GetBuffer(nFileLength);
	pHttpFile.Read(b, nFileLength);
	strBuffer.ReleaseBuffer();

	CBasicRequestInfo::ProcessResponse(strBuffer);

	if(atoi((LPCTSTR)GetStatusCode()) == 0) //success
	{

		strSearch = "CDInfo=";
		ParseFileForValue(strBuffer, strSearch, strList);

		char seps [] = "\r\n\t";
		FillListFromDelimitedString(strList, seps, m_strList);

	}

	if(!GetStatusCode())
	  return 1;
	else
	  return 0;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CGetCDInfoRequest

IMPLEMENT_DYNCREATE(CGetCDInfoRequest, CBasicRequestInfo)

CGetCDInfoRequest::CGetCDInfoRequest()
{
	m_strCDID = "";
	m_pCDInfo = NULL;
	 
	m_pCDInfo = new CCDInfo;
}

CGetCDInfoRequest::CGetCDInfoRequest(CString& strCDID)
{
	m_strCDID = strCDID;
	m_pCDInfo = NULL;
	 
	m_pCDInfo = new CCDInfo;
}

CGetCDInfoRequest::~CGetCDInfoRequest()
{
	delete m_pCDInfo;
}

void CGetCDInfoRequest::ProcessRequest(CString& pString)
{
	CString strChange = GetServerUrlAddress();
	strChange += "/GetCDInfo";
	SetServerUrlAddress(strChange);

	CBasicRequestInfo::ProcessRequest(pString);

	CString str;
	str.Format("&CDID=%s", m_strCDID);
	pString += str;
}

UINT CGetCDInfoRequest::ProcessResponse(CHttpFile& pHttpFile)
{
#ifdef ARTSTORE
	return 1;
#else	//Take the HttpFile and parse it.
	UINT nFileLength = pHttpFile.GetLength();
	
	CString strBuffer;
	CString strSearch;
	CString strList;
	LPSTR b = strBuffer.GetBuffer(nFileLength);
	pHttpFile.Read(b, nFileLength);
	strBuffer.ReleaseBuffer();

	CBasicRequestInfo::ProcessResponse(strBuffer);

	if(atoi((LPCTSTR)GetStatusCode()) == 0) //success
	{

		CString strValue;

		strSearch = "CDID=";
		ParseFileForValue(strBuffer, strSearch, strValue);
		m_pCDInfo->SetCDID(strValue);

		strSearch = "Name=";
		ParseFileForValue(strBuffer, strSearch, strValue);
		m_pCDInfo->SetName(strValue);

		strSearch = "Locale=";
		ParseFileForValue(strBuffer, strSearch, strValue);
		m_pCDInfo->SetLocale(strValue);

		strSearch = "Description=";
		ParseFileForValue(strBuffer, strSearch, strValue);
		m_pCDInfo->SetDescription(strValue);

		strSearch = "AvailabilityDate=";
		ParseFileForValue(strBuffer, strSearch, strValue);
		m_pCDInfo->SetAvailabilityDate(strValue);
		
		strSearch = "Price=";
		ParseFileForValue(strBuffer, strSearch, strValue);
		m_pCDInfo->SetPrice(strValue);

	}

	if(!GetStatusCode())
	  return 1;
	else
	  return 0;
#endif
}


/////////////////////////////////////////////////////////////////////////////
// CMarkCDSeenRequest

IMPLEMENT_DYNCREATE(CMarkCDSeenRequest, CBasicRequestInfo)

CMarkCDSeenRequest::CMarkCDSeenRequest()
{
	m_strCDID = "";
}

CMarkCDSeenRequest::CMarkCDSeenRequest(CString& strCDID)
{
	m_strCDID = strCDID;
}

CMarkCDSeenRequest::~CMarkCDSeenRequest()
{
}

void CMarkCDSeenRequest::ProcessRequest(CString& pString)
{
	CString strChange = GetServerUrlAddress();
	strChange += "/MarkCDSeen";
	SetServerUrlAddress(strChange);

	CBasicRequestInfo::ProcessRequest(pString);

	CString str;
	str.Format("&CDID=%s", m_strCDID);
	pString += str;
}

UINT CMarkCDSeenRequest::ProcessResponse(CHttpFile& pHttpFile)
{
	CBasicRequestInfo::ProcessResponse(pHttpFile);


	if(!GetStatusCode())
	  return 1;
	else
	  return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CPriceCDOrderRequest

IMPLEMENT_DYNCREATE(CPriceCDOrderRequest, CBasicRequestInfo)

CPriceCDOrderRequest::CPriceCDOrderRequest()
{
	m_strCDIDList = "";
	m_strShipMethod = "";
	m_strStreet1 = "";
	m_strStreet2 = "";
	m_strCity = "";
	m_strCountry = "";
	m_strZip = "";

}

CPriceCDOrderRequest::CPriceCDOrderRequest(LPCSTR strCDIDList)
{
	m_strCDIDList = strCDIDList;
	m_strShipMethod = "";
	m_strStreet1 = "";
	m_strStreet2 = "";
	m_strCity = "";
	m_strCountry = "";
	m_strZip = "";

}

CPriceCDOrderRequest::~CPriceCDOrderRequest()
{
}

void CPriceCDOrderRequest::ProcessRequest(CString& pString)
{
	CString strChange = GetServerUrlAddress();
	strChange += "/PriceCDOrder";
	SetServerUrlAddress(strChange);

	CBasicRequestInfo::ProcessRequest(pString);

	CString str;
	str.Format("&CDIDList=%s", m_strCDIDList);
	pString += str;

	str.Format("&ShipMethod=%s", m_strShipMethod);
	pString += str;

	str.Format("&Street1=%s", m_strStreet1);
	pString += str;

	str.Format("&Street2=%s", m_strStreet2);
	pString += str;

	str.Format("&City=%s", m_strCity);
	pString += str;

	str.Format("&Country=%s", m_strCountry);
	pString += str;

	str.Format("&Zip=%s", m_strZip);
	pString += str;
}

UINT CPriceCDOrderRequest::ProcessResponse(CHttpFile& pHttpFile)
{
#ifdef ARTSTORE
	return 1;
#else	//Take the HttpFile and parse it.
	UINT nFileLength = pHttpFile.GetLength();
	
	CString strBuffer;
	CString strSearch;
	LPSTR b = strBuffer.GetBuffer(nFileLength);
	pHttpFile.Read(b, nFileLength);
	strBuffer.ReleaseBuffer();

	CBasicRequestInfo::ProcessResponse(pHttpFile);

	if(atoi((LPCTSTR)GetStatusCode()) == 0) //success
	{

		strSearch = "ItemTotal=";
		ParseFileForValue(strBuffer, strSearch, m_strItemTotal);

		strSearch = "Discount=";
		ParseFileForValue(strBuffer, strSearch, m_strDiscount);

		strSearch = "Shipping=";
		ParseFileForValue(strBuffer, strSearch, m_strShipping);

		strSearch = "Handling=";
		ParseFileForValue(strBuffer, strSearch, m_strHandling);

		strSearch = "SalesTax=";
		ParseFileForValue(strBuffer, strSearch, m_strSalesTax);

		strSearch = "OrderTotal=";
		ParseFileForValue(strBuffer, strSearch, m_strOrderTotal);
	}


	if(!GetStatusCode())
	  return 1;
	else
	  return 0;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CPurchaseCDRequest

IMPLEMENT_DYNCREATE(CPurchaseCDRequest, CBasicRequestInfo)

CPurchaseCDRequest::CPurchaseCDRequest()
{
	m_strCDIDList = "";
	m_strAmount = "";
	m_strShipMethod = "";
	m_strRecipientName = "";
	m_strStreet1 = "";
	m_strStreet2 = "";
	m_strCity = "";
	m_strState = "";
	m_strCountry = "";
	m_strZip = "";
	m_strCardType = "";
	m_strCardNumber = "";
	m_strExpirationDate = "";
	m_strCardHolder = "";
	m_strCardStreet1 = "";
	m_strCardStreet2 = "";
	m_strCardCity = "";
	m_strCardCountry = "";
	m_strCardZip = "";

}

CPurchaseCDRequest::CPurchaseCDRequest(LPCSTR strCDIDList)
{
	m_strCDIDList = strCDIDList;
	m_strAmount = "";
	m_strShipMethod = "";
	m_strRecipientName = "";
	m_strStreet1 = "";
	m_strStreet2 = "";
	m_strCity = "";
	m_strState = "";
	m_strCountry = "";
	m_strZip = "";
	m_strCardType = "";
	m_strCardNumber = "";
	m_strExpirationDate = "";
	m_strCardHolder = "";
	m_strCardStreet1 = "";
	m_strCardStreet2 = "";
	m_strCardCity = "";
	m_strCardCountry = "";
	m_strCardZip = "";

}

CPurchaseCDRequest::~CPurchaseCDRequest()
{
}

void CPurchaseCDRequest::ProcessRequest(CString& pString)
{
	CString strChange = GetServerUrlAddress();
	strChange += "/PurchaseCD";
	SetServerUrlAddress(strChange);

	CBasicRequestInfo::ProcessRequest(pString);

	CString str;
	str.Format("&CDIDList=%s", m_strCDIDList);
	pString += str;

	str.Format("&Amount=%s", m_strAmount);
	pString += str;

	str.Format("&ShipMethod=%s", m_strShipMethod);
	pString += str;

	str.Format("&RecipientName=%s", m_strRecipientName);
	pString += str;

	str.Format("&Street1=%s", m_strStreet1);
	pString += str;

	str.Format("&Street2=%s", m_strStreet2);
	pString += str;

	str.Format("&City=%s", m_strCity);
	pString += str;

	str.Format("&State=%s", m_strState);
	pString += str;

	str.Format("&Country=%s", m_strCountry);
	pString += str;

	str.Format("&Zip=%s", m_strZip);
	pString += str;

	str.Format("&CardType=%s", m_strCardType);
	pString += str;

	str.Format("&CardNumber=%s", m_strCardNumber);
	pString += str;

	str.Format("&ExpirationDate=%s", m_strExpirationDate);
	pString += str;

	str.Format("&CardHolder=%s", m_strCardHolder);
	pString += str;

	str.Format("&CardStreet1=%s", m_strCardStreet1);
	pString += str;

	str.Format("&CardStreet2=%s", m_strCardStreet2);
	pString += str;

	str.Format("&CardCity=%s", m_strCardCity);
	pString += str;

	str.Format("&CardCountry=%s", m_strCardCountry);
	pString += str;

	str.Format("&CardZip=%s", m_strCardZip);
	pString += str;

}

UINT CPurchaseCDRequest::ProcessResponse(CHttpFile& pHttpFile)
{

	CBasicRequestInfo::ProcessResponse(pHttpFile);


	if(!GetStatusCode())
	  return 1;
	else
	  return 0;
}

