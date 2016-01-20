// LoginReq.cpp : implementation file
//

#include "stdafx.h"
#include "pmwinet.h"
#include "pmw.h"
#include "BasReqIn.h"
#include "GrLocReq.h"
#include "AdLocReq.h"  //for ContentMgr
#include "LoginReq.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoginRequest

IMPLEMENT_DYNCREATE(CLoginRequest, CBasicRequestInfo)

CLoginRequest::CLoginRequest()
{
	Initialize();
}

void CLoginRequest::Initialize( void )
{
	m_strItemID = "";
	m_strCollID = "";
	m_strReturnedItemInfo = "";
	m_strFile = "";
	m_strLoginID = "";
	m_strPasswordID = "";

	m_strServerList.RemoveAll();
	m_strGraphicLocationList.RemoveAll();
	m_strShipMethodsList.RemoveAll();
	m_strCreditCardTypesList.RemoveAll();

}

CLoginRequest::~CLoginRequest()
{

	m_strServerList.RemoveAll();
	m_strGraphicLocationList.RemoveAll();
	m_strShipMethodsList.RemoveAll();
	m_strCreditCardTypesList.RemoveAll();

}

void CLoginRequest::ProcessRequest(CString& pString)
{
	
	CString strServer = GetBaseServerUrlAddress();

	strServer += "/Login"; 
	SetServerUrlAddress(strServer);

	SetSessionID("0");

	CBasicRequestInfo::ProcessRequest(pString);

	CString str;

	str.Format("&ItemColl=%s", m_strCollID);
	pString += str;

	str.Format("&ItemName=%s", m_strItemID);
	pString += str;
}

UINT CLoginRequest::ProcessResponse(CHttpFile& pHttpFile)
{
#ifdef ARTSTORE
	return 1;
#else	
	//Take the HttpFile and parse it.
	UINT nFileLength = pHttpFile.GetLength();
	
	CString strBuffer;
	CString strSearch;
	CString strList;
	LPSTR b = strBuffer.GetBuffer(nFileLength);
	pHttpFile.Read(b, nFileLength);
	strBuffer.ReleaseBuffer();

	CBasicRequestInfo::ProcessResponse(strBuffer);
	
	UINT nRet = -1;
	nRet = atoi((LPCTSTR)GetStatusCode());

	if(nRet == 6 || nRet == 0) //Use Alternate server
	{
		//the session ID should be zero
		strSearch = "DataServers=";
		ParseFileForValue(strBuffer, strSearch, strList);

		char seps [] = "\t";
		FillListFromDelimitedString(strList, seps, m_strServerList);

	}


	if(nRet == 0) //success
	{

		strSearch = "GraphicServers=";
		ParseFileForLineFeeds(strBuffer, strSearch, strList);

		if(strList != "-1")
		{
			char seps [] = "\r\n\t";
			FillListFromDelimitedString(strList, seps, m_strGraphicLocationList);

			strSearch = "SessionID=";
			ParseFileForValue(strBuffer, strSearch, GetSessionID());

			strSearch = "Item=";
			ParseFileForValue(strBuffer, strSearch, m_strReturnedItemInfo);
			GetDownloadInfo(m_strReturnedItemInfo);

			strSearch = "AdInfo=";
			ParseFileForLineFeeds(strBuffer, strSearch, m_strAdInfo);

			FillAdParams(m_strAdInfo, seps);

			strSearch = "CollInfo=";
			ParseFileForLineFeeds(strBuffer, strSearch, m_strCollInfo);

			strSearch = "ShipMethods=";
			ParseFileForLineFeeds(strBuffer, strSearch, strList);

			FillListFromDelimitedString(strList, seps, m_strShipMethodsList);

			strSearch = "CardTypes=";
			ParseFileForLineFeeds(strBuffer, strSearch, strList);

			FillListFromDelimitedString(strList, seps, m_strCreditCardTypesList);
		}
		else
		{
			nRet = 2; //server not responding with valid data
		}
	}

	  return nRet;
#endif
}

void CLoginRequest::GetDownloadInfo(CString& strString)
{
	char seps [] = "\t";
	char* pstr = strString.GetBuffer(strString.GetLength());
	char* token = strtok( pstr, seps );

	if (token != NULL)
	{
		SetFile(token);

		/* Get next token: */
		if ((token = strtok( NULL, seps )) != NULL)
		{
			SetLoginID(token);

			/* Get next token: */
			if ((token = strtok( NULL, seps )) != NULL)
			{
				SetPasswordID(token);
			}
		}
	}  
	strString.ReleaseBuffer();
}

void CLoginRequest::FillAdParams(CString& strList, const char* seps)
{

	char* pstr = strList.GetBuffer(strList.GetLength());
	char* token = strtok( pstr, seps );

	if( token != NULL )
	{
		/* While there are tokens in "string" */
		m_AdInfo.SetAdID(token);
		/* Get next token: */
		if ((token = strtok( NULL, seps )) != NULL)
		{
			m_AdInfo.SetFileName(token);
			/* Get next token: */
			if ((token = strtok( NULL, seps )) != NULL)
			{
				m_AdInfo.SetLocale(token);
				/* Get next token: */
				if ((token = strtok( NULL, seps )) != NULL)
				{
					m_AdInfo.SetDescription(token);
					/* Get next token: */
					if ((token = strtok( NULL, seps )) != NULL)
					{
						m_AdInfo.SetURLLink(token);
#if 0
						CString csMessage;
						csMessage.Format("Ad ID: %s; File: %s\nDescription: %s\nURL: %s",
											  (LPCSTR)m_AdInfo.GetAdID(),
											  (LPCSTR)m_AdInfo.GetFileName(),
											  (LPCSTR)m_AdInfo.GetDescription(),
											  (LPCSTR)m_AdInfo.GetURLLink());
						AfxMessageBox(csMessage);
#endif
					}
				}
			}
		}
	}  
	strList.ReleaseBuffer();
}

