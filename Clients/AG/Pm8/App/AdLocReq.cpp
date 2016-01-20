// AdLocReq.cpp : implementation file
//

#include "stdafx.h"
#include "pmwinet.h"
#include "pmw.h"
#include "BasReqIn.h"
#include "AdLocReq.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetAdInfoRequest

IMPLEMENT_DYNCREATE(CGetAdInfoRequest, CBasicRequestInfo)

CGetAdInfoRequest::CGetAdInfoRequest()
{
	
}

CGetAdInfoRequest::CGetAdInfoRequest(CString& strAdId)
{
	m_strAdId = strAdId;
}

CGetAdInfoRequest::~CGetAdInfoRequest()
{
}

/////////////////////////////////////////////////////////////////////////////
// CGetAdvtLocsListRequest

IMPLEMENT_DYNCREATE(CGetAdvtLocsListRequest, CBasicRequestInfo)

CGetAdvtLocsListRequest::CGetAdvtLocsListRequest()
{
	
	m_strSelector = "all"; //for internal use only
	m_strAdInfoList.RemoveAll();
	m_strIgnoreLocale = "F";
}

CGetAdvtLocsListRequest::CGetAdvtLocsListRequest(CString& strItemId)
{
	m_strSelector = "all";  //for internal use only
	m_strAdInfoList.RemoveAll();
	m_strIgnoreLocale = "F";
}

CGetAdvtLocsListRequest::~CGetAdvtLocsListRequest()
{

	m_strAdInfoList.RemoveAll();
}

void CGetAdvtLocsListRequest::ProcessRequest(CString& pString)
{
	CString strChange = GetServerUrlAddress();
	strChange += "/GetAdList.idc";
	SetServerUrlAddress(strChange);

	CBasicRequestInfo::ProcessRequest(pString);

	CString str;
	//str.Format("&Op=%s", GetOperation());
	//pString += str;

	str.Format("&List=%s", m_strSelector);
	pString += str;

	str.Format("&IgnoreLocale=%s", m_strIgnoreLocale);
	pString += str;
}

UINT CGetAdvtLocsListRequest::ProcessResponse(CHttpFile& pHttpFile)
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

	if(atoi((LPCTSTR)GetStatusCode()) == 0) //success
	{

		strSearch = "AdInfo=";
		ParseFileForValue(strBuffer, strSearch, strList);

		char seps [] = "\r\n";
		FillListFromDelimitedString(strList, seps, m_strAdInfoList);
		
		CString strPassword;
		CString strLocation;
		int n;
		for(n = 0; n < m_strAdInfoList.GetCount(); n++)
		{
			//ParseStringToLocationAndPassword(m_strLocationsList.GetAt(m_strLocationsList.FindIndex(n)), strLocation, strPassword);
			
			//Now...Go get the advertisement
		}
	}

	if(!GetStatusCode())
	  return 1;
	else
	  return 0;
#endif
}

#if 0
void CGetAdvtLocsListRequest::ParseStringToAdInfo(CString& strString, CString& strLocation, CString& strPassword)
{
	char seps [] = "\t";
	char* pstr = strString.GetBuffer(strString.GetLength());
	char* token = strtok( pstr, seps );

	while( token != NULL )
	{
		/* While there are tokens in "string" */
		strLocation = token;
		/* Get next token: */
		if ((token = strtok( NULL, seps )) != NULL)
		{
			strPassword = token;
		}
	}  
	strString.ReleaseBuffer();
}

#endif

/////////////////////////////////////////////////////////////////////////////
// CGetAdvtLocRequest

IMPLEMENT_DYNCREATE(CGetAdvtLocRequest, CBasicRequestInfo)

CGetAdvtLocRequest::CGetAdvtLocRequest()
{
	m_strItemID = "";
	m_strLocationsList.RemoveAll();
}

CGetAdvtLocRequest::CGetAdvtLocRequest(CString& strItemId)
{
	m_strItemID = "";
	m_strLocationsList.RemoveAll();
}

CGetAdvtLocRequest::~CGetAdvtLocRequest()
{
	m_strLocationsList.RemoveAll();
}

void CGetAdvtLocRequest::ProcessRequest(CString& pString)
{
	CString strChange = GetServerUrlAddress();
	strChange += "/GetAdInfo.idc";
	SetServerUrlAddress(strChange);

	CBasicRequestInfo::ProcessRequest(pString);

	CString str;
	str.Format("&Op=%s", GetOperation());
	pString += str;

	str.Format("&AdId=%s", m_strItemID);
	pString += str;
}

UINT CGetAdvtLocRequest::ProcessResponse(CHttpFile& pHttpFile)
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

	if(atoi((LPCTSTR)GetStatusCode()) == 0) //success
	{

		strSearch = "AdId=";
		ParseFileForValue(strBuffer, strSearch, m_strAdID);

		strSearch = "FileName=";
		ParseFileForValue(strBuffer, strSearch, m_strItemID);

		strSearch = "Description=";
		ParseFileForValue(strBuffer, strSearch, m_strDescription);

		strSearch = "URLLink=";
		ParseFileForValue(strBuffer, strSearch, m_strURLLink);

		strSearch = "Locale=";
		CString m_strList = "";
		ParseFileForValue(strBuffer, strSearch, m_strList);

		char seps [] = "\r\n";
		FillListFromDelimitedString(strList, seps, m_strLocationsList);
		
		//int n;
		//for(n = 0; n < m_strLocationsList.GetCount(); n++)
		//{
			//ParseStringToLocationAndPassword(m_strLocationsList.GetAt(m_strLocationsList.FindIndex(n)), strLocation, strPassword);
			
			//Now...Go get the advertisement
		//}
	}

	if(!GetStatusCode())
	  return 1;
	else
	  return 0;
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CMarkAdvtSeenRequest

IMPLEMENT_DYNCREATE(CMarkAdvtSeenRequest, CBasicRequestInfo)

CMarkAdvtSeenRequest::CMarkAdvtSeenRequest()
{
	m_strAdID = "";
}

CMarkAdvtSeenRequest::CMarkAdvtSeenRequest(CString& strItemId)
{
	m_strAdID = strItemId;
}

CMarkAdvtSeenRequest::~CMarkAdvtSeenRequest()
{
}

void CMarkAdvtSeenRequest::ProcessRequest(CString& pString)
{
	CString strChange = GetServerUrlAddress();
	strChange += "/MarkAdSeen";
	SetServerUrlAddress(strChange);

	CBasicRequestInfo::ProcessRequest(pString);

	CString str;
	str.Format("&Op=%s", GetOperation());
	pString += str;

	str.Format("&AdId=%s", m_strAdID);
	pString += str;
}

UINT CMarkAdvtSeenRequest::ProcessResponse(CHttpFile& pHttpFile)
{
	CBasicRequestInfo::ProcessResponse(pHttpFile);


	if(!GetStatusCode())
	  return 1;
	else
	  return 0;
}

