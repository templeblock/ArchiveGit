// BasicRequestInfo.cpp : implementation file
//

#include "stdafx.h"
#include "pmw.h"
#include "BasReqIn.h"

#include "pmwinet.h"   //for ContentMgr
#include <locale.h>
#include "GrLocReq.h"  //for ContentMgr
#include "AdLocReq.h"  //for ContentMgr
#include "LoginReq.h"  //for ContentMgr
#include "AcctInfo.h"  //for ContentMgr
#include "ContMgr.h"  //for ContentMgr

#include "uuencode.h"

#ifndef ARTSTORE
#include "PGSSess.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBasicRequestInfo

IMPLEMENT_DYNCREATE(CBasicRequestInfo, CCmdTarget)

CBasicRequestInfo::CBasicRequestInfo()
{
	m_strOperation = "";
	m_strAccountID = "";
	m_strPassword = "";
	m_strClientSKU = "";
	m_strClientVersion = "";
	m_strClientPGSVersion = "1.0";
	m_strClientLocale = "us-english";

	//char* pstr = m_strClientLocale.GetBuffer(10);
	//ltoa(GetUserDefaultLCID(), pstr, 10);
	//m_strClientLocale.ReleaseBuffer();

	m_fUnknownItemCheck	= TRUE;

	m_pHttpFile = NULL;
	m_pHttpConnection = NULL;

//	m_strBaseServerUrlAddress = GET_PMWAPP()->GetHiddenContentManager()->GetOnlineDataServerAddress(); //"http://pmgsdata.printmaster.com:31597/scripts/MindscapePGS";
	m_strBaseServerUrlAddress = "http://205.156.205.221:31597/scripts/MindscapePGS";
	m_strServerUrlAddress = m_strBaseServerUrlAddress;

	m_strHeaders =
		_T("Content-Type: application/x-www-form-urlencoded");
}

CBasicRequestInfo::~CBasicRequestInfo()
{
}

void CBasicRequestInfo::AddParameters()
{
	//m_strOperation is class-derived specific

	m_strAccountID = GET_PMWAPP()->GetHiddenContentManager()->GetAccountID();
	m_strPassword = GET_PMWAPP()->GetHiddenContentManager()->GetPassword();
//	char strTemp[100];
	m_strSessionID = GET_PMWAPP()->GetHiddenContentManager()->GetSessionID();
//	m_strRequestID = itoa((atoi(GET_PMWAPP()->GetHiddenContentManager()->GetRequestID()) + 1), strTemp, 10); //bump each time
	m_strRequestID = "1";
//	GET_PMWAPP()->GetHiddenContentManager()->SetRequestID(m_strRequestID);
	m_strClientSKU = GET_PMWAPP()->GetParentPartNumber();
	m_strClientVersion = GET_PMWAPP()->GetVersion();
	m_strClientPGSVersion = GetClientPGSVersion();
	m_strClientLocale = GetClientLocale();

}

void CBasicRequestInfo::ProcessRequest(CString& pString)
{
	CString str;
	
	str = "Acct=";
	str += GetAccountID();
	str += "&Pwd=";
	str += GetPassword();
	str += "&Sess=";
	str += GetSessionID();
	str += "&Req=";
	str += GetRequestID();
	str += "&SKU=";
	str += m_strClientSKU;
	str += "&Ver=";
	str += m_strClientVersion;
	str += "&PGS=";
	str += m_strClientPGSVersion;
	str += "&Loc=";
	str += m_strClientLocale;//*setlocale( LC_ALL, NULL );

	pString = str;

}


UINT CBasicRequestInfo::DoRequest()
{
#ifdef ARTSTORE
	return 1;
#else
	DWORD   dwAccessType = PRE_CONFIG_INTERNET_ACCESS;
	BOOL bGet = FALSE;

	//these are probably not necessary
	DWORD dwHttpRequestFlags =
		INTERNET_FLAG_ASYNC | INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT;

	if(m_strOperation == "PurchaseCollection" || m_strOperation == "PurchaseCD")
	{
		dwHttpRequestFlags = 
			INTERNET_FLAG_ASYNC | INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT | INTERNET_FLAG_SECURE;
	}

	UINT nRetCode = 6;  //default code for "try another server"

    AfxGetApp()->BeginWaitCursor();

	CString SourceUrlFile;
	ProcessRequest(SourceUrlFile);

	//test if in GET Mode
	if(bGet = UseGETMethod())
	{
		CString strNew;
		strNew = GetServerUrlAddress();
		strNew += "?";
		strNew += SourceUrlFile;
		SetServerUrlAddress(strNew);
	}

	CPGSSession InternetSession(_T("PGS"), dwAccessType);
	InternetSession.EnableStatusCallback(TRUE);

	try
	{
		// check to see if this is a reasonable URL

		CString strServerName;
		CString strObject;
		INTERNET_PORT nPort;
		DWORD dwServiceType;

		CString strTest = GetServerUrlAddress();

		if(m_strOperation == "PurchaseCollection" || m_strOperation == "PurchaseCD")
		{
			int i = strTest.Find("http");
			CString strtemp = strTest.Right(strTest.GetLength() - (i + 4));
			strTest = "https";
			strTest += strtemp;


		}

		if (!AfxParseURL(GetServerUrlAddress(), dwServiceType, strServerName, strObject, nPort) ||
			dwServiceType != INTERNET_SERVICE_HTTP)
		{
			AfxMessageBox(IDS_PGS_INVALID_URL);
		}

		//  Get a connection
		m_pHttpConnection = InternetSession.GetHttpConnection(strServerName, nPort);


		if(m_pHttpConnection)
		{
			if(bGet){
				//Formulate a request for a file
				m_pHttpFile = m_pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_GET, strObject,
									NULL, 1, NULL, NULL, dwHttpRequestFlags);
			}else{
				//Formulate a request for a file
				m_pHttpFile = m_pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject,
									NULL, 1, NULL, NULL, dwHttpRequestFlags);
			}			

			if(m_pHttpFile)
			{
				m_pHttpFile->AddRequestHeaders(m_strHeaders, HTTP_ADDREQ_FLAG_REPLACE);

				if(bGet){
					//Go get the file
					m_pHttpFile->SendRequest(m_strHeaders, NULL, 0);
				}else{
					//Go get the file
					LPSTR s = SourceUrlFile.GetBuffer(SourceUrlFile.GetLength());
					m_pHttpFile->SendRequest(m_strHeaders, s, SourceUrlFile.GetLength());
					SourceUrlFile.ReleaseBuffer();
				}

				DWORD dwRet;
				GetHttpFile()->QueryInfoStatusCode(dwRet);

				if (dwRet != HTTP_STATUS_OK)
				{
					AfxMessageBox(IDS_PGS_UNKNOWN_ERROR);
					nRetCode = ProcessResponse(*GetHttpFile());
				}
				else
				{

					nRetCode = ProcessResponse(*GetHttpFile());
				}

				m_pHttpFile->Close();
				delete m_pHttpFile;
			}

			m_pHttpConnection->Close();
			delete m_pHttpConnection;
		}
	}
	catch (CInternetException* pEx)
	{
		TCHAR szErr[1024];
		pEx->GetErrorMessage(szErr, 1024);

		nRetCode = 2;

		if(m_pHttpFile)
		{
			m_pHttpFile->Close();
			delete m_pHttpFile;
		}

		if(m_pHttpConnection)
		{
			m_pHttpConnection->Close();
			delete m_pHttpConnection;
		}
		
		pEx->Delete();
	}

    AfxGetApp()->EndWaitCursor();

	return nRetCode;
#endif
}

UINT CBasicRequestInfo::ProcessResponse(CHttpFile& pHttpFile)
{
#ifdef ARTSTORE
	return 1;
#else
	//Take the HttpFile and parse it.
	UINT nFileLength = pHttpFile.GetLength();
	
	CString strBuffer;
	LPSTR p = strBuffer.GetBuffer(nFileLength);
	pHttpFile.Read(p, nFileLength);
	strBuffer.ReleaseBuffer();

	CString strSearch = "Status=";
	ParseFileForValue(strBuffer, strSearch, GetStatusCode());

	if(GetStatusCode() == "-1")
		return 6;
	
	int StatusCode = -1;
	StatusCode = atoi((LPCTSTR)GetStatusCode());
	ProcessStatusCode(StatusCode);

	return StatusCode;
#endif
}

UINT CBasicRequestInfo::ProcessResponse(CString& strBuffer)
{

	CString strSearch = "Status=";
	ParseFileForValue(strBuffer, strSearch, GetStatusCode());
	
	int StatusCode = 0;
	StatusCode = atoi((LPCTSTR)GetStatusCode());
	ProcessStatusCode(StatusCode);

	return StatusCode;
}

void CBasicRequestInfo::ParseFileForValue(CString& strBuffer, CString& strSearch, CString& strValue)
{
	
	strValue = "-1";

	UINT index = strBuffer.Find(strSearch);
	
	if(index != -1)
	{
		int n;
		char c;
		int l = strBuffer.GetLength() - strSearch.GetLength() - (signed)index;
		for(n = 0; n < l; n++)
		{
			c = strBuffer.GetAt(index + strSearch.GetLength() + n);
			if(c != '\r'){
			  if(n == 0){
				strValue = c;
			  }else{
				strValue += c;
			  }
			}else{
				break;
			}
		}
	}

}

void CBasicRequestInfo::ParseFileForLineFeeds(CString& strBuffer, CString& strSearch, CString& strValue)
{
	
	UINT index = strBuffer.Find(strSearch);
	
	if(index != -1)
	{
		int n;
		char c, ch;
		int l = strBuffer.GetLength() - strSearch.GetLength() - (signed)index;
		for(n = 0; n < l; n++)
		{
			c = strBuffer.GetAt(index + strSearch.GetLength() + n);
			
			if(((signed)index + strSearch.GetLength() + n + 1) < strBuffer.GetLength())
			  ch = strBuffer.GetAt(index + strSearch.GetLength() + n + 1);
			else
			  ch = '\n';

			if((c != '\n') && (ch != '\n')){
			  if(n == 0){
				strValue = c;
			  }else{
				strValue += c;
			  }
			}else{
				break;
			}
		}
	}
}

void CBasicRequestInfo::UrlEncodeString(CString& strBuffer)
{
	
	int n;
	char c;
	
	int l = strBuffer.GetLength();
	for(n = 0; n < l; n++)
	{
		c = strBuffer.GetAt(n);
		if(c == ' '){
			strBuffer.SetAt(n, '+');
		}
	}
}


void CBasicRequestInfo::FillListFromDelimitedString(CString& strList, const char* seps, CStringList& strServerList)
{

	char* pstr = strList.GetBuffer(strList.GetLength());
	char* token = strtok( pstr, seps );

	while( token != NULL )
	{
	  /* While there are tokens in "string" */
	  strServerList.AddTail(token);
	  /* Get next token: */
	  token = strtok( NULL, seps );
	}  
	strList.ReleaseBuffer();
}

void CBasicRequestInfo::ProcessStatusCode(int nStatusCode)
{
	switch(nStatusCode)
	{
	case 0:  //Success
		//MessageBox(NULL, "Operation Completed Successfully", "Operation Status", MB_OK);
		break;
	case 1:  //IllFormedRequest
		AfxMessageBox(IDS_PGS_ILLFORMED_REQUEST);
		break;
	case 2:  //ServiceError
		AfxMessageBox(IDS_PGS_SERVICE_ERROR);
		break;
	case 3:  //ProtocolVersionNotSupported
		AfxMessageBox(IDS_PGS_PROTOCOL_NOT_SUPPORTED);
		break;
	case 4:  //ClientVersionNotSupported
		AfxMessageBox(IDS_PGS_CLIENT_VERSION_NOT_SUPPORTED);
		break;
	case 5:  //InvalidLoginOrPassword
		AfxMessageBox(IDS_PGS_INVALID_LOGIN);
		break;
	case 6:  //UseAlternateServer
		//MessageBox(NULL, "The current server is busy, trying another", "Operation Status", MB_OK);
		break;
	case 7:  //UnknownAdId
		AfxMessageBox(IDS_PGS_UNKNOWN_AD_ID);
		break;
	case 8:  //NoAdsMatchingCriteria
		AfxMessageBox(IDS_PGS_NOADS_MATCHING);
		break;
	case 9:  //UnknownCollectionId
		AfxMessageBox(IDS_PGS_UNKNOWN_COLLECTION_ID);
		break;
	case 10: //NoCollectionsMatchingCriteria
		AfxMessageBox(IDS_PGS_NOCOLLECTION_MATCHING);
		break;
	case 11: //UnknownCDID
		AfxMessageBox(IDS_PGS_UNKNOWN_CD_ID);
		break;
	case 12: //NoCDsMatching Criteria
		AfxMessageBox(IDS_PGS_NOCDS_MATCHING);
		break;
	case 13: //Unknown Item ID
		if(m_fUnknownItemCheck)
		  AfxMessageBox(IDS_PGS_UNKNOWN_ITEM);
		break;
	case 14: //ItemNotLicensed
		AfxMessageBox(IDS_PGS_ITEM_UNLICENSED);
		break;
	case 15: //The LoginID is already in use
		//AfxMessageBox(IDS_PGS_LOGINID_IN_USE);
		break;
	case 16: //CreditCardAuthFailed
		AfxMessageBox(IDS_PGS_CREDITCARD_AUTHORIZATION_FAILED);
		break;
	case 17: //CDPurchasingInProgress
		AfxMessageBox(IDS_PGS_CD_PURCHASE_INPROGRESS);
		break;
	case 18: //CollectionPurchaseInProgress
		AfxMessageBox(IDS_PGS_COLLECTION_PURCHASE_INPROGRESS);
		break;
	default:
		break;
	}
}

UINT CBasicRequestInfo::UUEncodeHeaders(CString& pstrLoginID, CString& pstrPassword, CString& pstrHeaders)
{
	CUUEncodeBuffer buff;

	char achUserAndPass[255];
	memset(achUserAndPass, '\0', 255);

	strcpy( achUserAndPass, pstrLoginID );
	strcat( achUserAndPass, ":" );
	strcat( achUserAndPass, pstrPassword );

	CUUEncoder UUEncoder;
	if (!UUEncoder.Encode( (BYTE *) achUserAndPass,
              strlen( achUserAndPass ),
              &buff ))
	{
		AfxMessageBox("EncodeHeaders: uuencode failed");
	}

	pstrHeaders = "Authorization: ";
	pstrHeaders += "Basic ";
	pstrHeaders += buff.GetPtr();
	pstrHeaders += "\r\n";

	return 1;
}

BOOL CBasicRequestInfo::UseGETMethod()
{
	if(m_strOperation == "Login" ||
		m_strOperation == "EstablishAccount"){

		m_strHeaders = "";
		CString strLoginID = "SGMP";
		CString strPassword = "saviM2Go";

		UUEncodeHeaders(strLoginID, strPassword, m_strHeaders);
		m_strHeaders += "\r\n";

		return TRUE;

	}
	return FALSE;
}


BEGIN_MESSAGE_MAP(CBasicRequestInfo, CCmdTarget)
	//{{AFX_MSG_MAP(CBasicRequestInfo)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBasicRequestInfo message handlers

