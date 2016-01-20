// GrLocReq.cpp : implementation file
//

#include "stdafx.h"
#include "pmwinet.h"
#include "pmw.h"
#include "BasReqIn.h"
#include "GrLocReq.h"
#include "GrphReq.h"

#include "resource.h"   // For progress bar

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetGraphicsLocsRequest

IMPLEMENT_DYNCREATE(CGetGraphicsLocsRequest, CBasicRequestInfo)

CGetGraphicsLocsRequest::CGetGraphicsLocsRequest()
{
	m_strItemID = "";
	m_strCollID = "";

	m_strFile = "";
	m_strLoginID = "";
	m_strPasswordID = "";
}

CGetGraphicsLocsRequest::CGetGraphicsLocsRequest(CString& strColl, CString& strItemId)
{
	m_strItemID = strItemId;
	m_strCollID = strColl;

	m_strFile = "";
	m_strLoginID = "";
	m_strPasswordID = "";

}

CGetGraphicsLocsRequest::~CGetGraphicsLocsRequest()
{
}

void CGetGraphicsLocsRequest::ProcessRequest(CString& pString)
{
	CString strChange = GetBaseServerUrlAddress();
	strChange += "/GetGraphicInfo";
	SetServerUrlAddress(strChange);

	CBasicRequestInfo::ProcessRequest(pString);

	CString str;

	str.Format("&ItemColl=%s", m_strCollID);
	pString += str;

	str.Format("&ItemName=%s", m_strItemID);
	pString += str;
}

UINT CGetGraphicsLocsRequest::ProcessResponse(CHttpFile& pHttpFile)
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
		strSearch = "Item=";
		ParseFileForValue(strBuffer, strSearch, m_strReturnedItemInfo);
		GetDownloadInfo(m_strReturnedItemInfo);
	}

	if(!GetStatusCode())
	  return 1;
	else
	  return 0;
#endif
}

void CGetGraphicsLocsRequest::GetDownloadInfo(CString& strString)
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

