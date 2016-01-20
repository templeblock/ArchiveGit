// CollReq.cpp : implementation file
//

#include "stdafx.h"
#include "pmwinet.h"
#include "pmw.h"
#include "BasReqIn.h"
#include "CollReq.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetCollListRequest

IMPLEMENT_DYNCREATE(CGetCollListRequest, CBasicRequestInfo)

CGetCollListRequest::CGetCollListRequest()
{
	m_strSelector = "all";
	m_strList.RemoveAll();
	m_strIgnoreLocale = "F";
}

CGetCollListRequest::CGetCollListRequest(CString& strItemId)
{
	m_strSelector = "all";
	m_strList.RemoveAll();
	m_strIgnoreLocale = "F";
}

CGetCollListRequest::~CGetCollListRequest()
{
	m_strList.RemoveAll();
}

void CGetCollListRequest::ProcessRequest(CString& pString)
{
	CString strChange = GetServerUrlAddress();
	strChange += "/GetCollectionList";
	SetServerUrlAddress(strChange);

	CBasicRequestInfo::ProcessRequest(pString);

	CString str;

	str.Format("&List=%s", m_strSelector);
	pString += str;

	str.Format("&IgnoreLocale=%s", m_strIgnoreLocale);
	pString += str;
}

UINT CGetCollListRequest::ProcessResponse(CHttpFile& pHttpFile)
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

		int num = strBuffer.Find("CollInfo=");
		strList = strBuffer.Right(strBuffer.GetLength() - num);

		char seps [] = "\r\n";
		FillListFromDelimitedString(strList, seps, m_strList);

	}

	if(!GetStatusCode())
	  return 1;
	else
	  return (atoi((LPCTSTR)GetStatusCode()));
#endif
}

void CGetCollListRequest::FillListFromDelimitedString(CString& strList, const char* seps, CStringList& strServerList)
{

	char* pstr = strList.GetBuffer(strList.GetLength());
	char* token = strtok( pstr, seps );
	CString str;

	while( token != NULL )
	{
	  /* While there are tokens in "string" */
	   str = token + strlen("CollInfo=");

		strServerList.AddTail(str);
	  /* Get next token: */
	  token = strtok( NULL, seps );
	}  
	strList.ReleaseBuffer();
}


/////////////////////////////////////////////////////////////////////////////
// CGetCollInfoRequest

IMPLEMENT_DYNCREATE(CGetCollInfoRequest, CBasicRequestInfo)

CGetCollInfoRequest::CGetCollInfoRequest()
{
	m_strCollID = "";
	m_pCollInfo = NULL;
	 
	m_pCollInfo = new CCollectionInfo;
}

CGetCollInfoRequest::CGetCollInfoRequest(CString& strCollID)
{
	m_strCollID = strCollID;
	m_pCollInfo = NULL;
	 
	m_pCollInfo = new CCollectionInfo;
}

CGetCollInfoRequest::~CGetCollInfoRequest()
{
	delete m_pCollInfo;
}

void CGetCollInfoRequest::ProcessRequest(CString& pString)
{
	CString strChange = GetServerUrlAddress();
	strChange += "/GetCollectionInfo";
	SetServerUrlAddress(strChange);

	CBasicRequestInfo::ProcessRequest(pString);

	CString str;
	str.Format("&CollID=%s", m_strCollID);
	pString += str;
}

UINT CGetCollInfoRequest::ProcessResponse(CHttpFile& pHttpFile)
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

		strSearch = "CollID=";
		ParseFileForValue(strBuffer, strSearch, strValue);
		m_pCollInfo->SetCollID(strValue);

		strSearch = "Name=";
		ParseFileForValue(strBuffer, strSearch, strValue);
		m_pCollInfo->SetName(strValue);

		strSearch = "FileName=";
		ParseFileForValue(strBuffer, strSearch, strValue);
		m_pCollInfo->SetFileName(strValue);

		strSearch = "Locale=";
		ParseFileForValue(strBuffer, strSearch, strValue);
		m_pCollInfo->SetLocale(strValue);

		strSearch = "Description=";
		ParseFileForValue(strBuffer, strSearch, strValue);
		m_pCollInfo->SetDescription(strValue);

		strSearch = "Price=";
		ParseFileForValue(strBuffer, strSearch, strValue);
		m_pCollInfo->SetPrice(strValue);

		strSearch = "IconFileName=";
		ParseFileForValue(strBuffer, strSearch, strValue);
		m_pCollInfo->SetIconFileName(strValue);

		strSearch = "BrochureFileName=";
		ParseFileForValue(strBuffer, strSearch, strValue);
		m_pCollInfo->SetBrochureFileName(strValue);

	}

	if(!GetStatusCode())
	  return 1;
	else
	  return (atoi((LPCTSTR)GetStatusCode()));
#endif
}

void CGetCollInfoRequest::FillCollParams(CString& strList, const char* seps)
{

	char* pstr = strList.GetBuffer(strList.GetLength());
	char* token = strtok( pstr, seps );

	if( token != NULL )
	{
		/* While there are tokens in "string" */
		m_pCollInfo->SetCollID(token);
		/* Get next token: */
		if ((token = strtok( NULL, seps )) != NULL)
		{
			m_pCollInfo->SetName(token);
			/* Get next token: */
			if ((token = strtok( NULL, seps )) != NULL)
			{
				m_pCollInfo->SetFileName(token);
				/* Get next token: */
				if ((token = strtok( NULL, seps )) != NULL)
				{
					m_pCollInfo->SetLocale(token);
					/* Get next token: */
					if ((token = strtok( NULL, seps )) != NULL)
					{
						m_pCollInfo->SetDescription(token);
						/* Get next token: */
						//if ((token = strtok( NULL, seps )) != NULL)
						//{
							//m_pCollInfo->SetPrice(token);
							/* Get next token: */
							if ((token = strtok( NULL, seps )) != NULL)
							{
								m_pCollInfo->SetIconFileName(token);
								/* Get next token: */
								if ((token = strtok( NULL, seps )) != NULL)
								{
									m_pCollInfo->SetBrochureFileName(token);
								}
							}
						//}
					}
				}
			}
		}
	}  
	strList.ReleaseBuffer();
}


/////////////////////////////////////////////////////////////////////////////
// CMarkCollSeenRequest

IMPLEMENT_DYNCREATE(CMarkCollSeenRequest, CBasicRequestInfo)

CMarkCollSeenRequest::CMarkCollSeenRequest()
{
	m_strCollID = "";
}

CMarkCollSeenRequest::CMarkCollSeenRequest(CString& strCollID)
{
	m_strCollID = strCollID;
}

CMarkCollSeenRequest::~CMarkCollSeenRequest()
{
}

void CMarkCollSeenRequest::ProcessRequest(CString& pString)
{
	CString strChange = GetServerUrlAddress();
	strChange += "/MarkCollectionSeen";
	SetServerUrlAddress(strChange);

	CBasicRequestInfo::ProcessRequest(pString);

	CString str;
	str.Format("&CollId=%s", m_strCollID);
	pString += str;
}

UINT CMarkCollSeenRequest::ProcessResponse(CHttpFile& pHttpFile)
{
	CBasicRequestInfo::ProcessResponse(pHttpFile);


	if(!GetStatusCode())
	  return 1;
	else
	  return (atoi((LPCTSTR)GetStatusCode()));
}

/////////////////////////////////////////////////////////////////////////////
// CPurchaseCollectionRequest

IMPLEMENT_DYNCREATE(CPurchaseCollectionRequest, CBasicRequestInfo)

CPurchaseCollectionRequest::CPurchaseCollectionRequest()
{
	m_strCollIDList = "";
	m_strAmount = "";
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

CPurchaseCollectionRequest::CPurchaseCollectionRequest(CString& strCollIDList)
{
	m_strCollIDList = strCollIDList;
	m_strAmount = "";
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

CPurchaseCollectionRequest::~CPurchaseCollectionRequest()
{
}

void CPurchaseCollectionRequest::ProcessRequest(CString& pString)
{
	CString strChange = GetServerUrlAddress();
	strChange += "/PurchaseCollection";
	SetServerUrlAddress(strChange);

	CBasicRequestInfo::ProcessRequest(pString);

	CString str;
	str.Format("&CollIDList=%s", m_strCollIDList);
	pString += str;

	str.Format("&Amount=%s", m_strAmount);
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

UINT CPurchaseCollectionRequest::ProcessResponse(CHttpFile& pHttpFile)
{

	CBasicRequestInfo::ProcessResponse(pHttpFile);


	if(!GetStatusCode())
	  return 1;
	else
	  return (atoi((LPCTSTR)GetStatusCode()));
}
