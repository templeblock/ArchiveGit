// AccountInfo.cpp : implementation file
//

#include "stdafx.h"
#include "pmwinet.h"
#include "pmw.h"
#include "pmwini.h"
#include "progbar.h"
#include "file.h"
#include "BasReqIn.h"
#include "AcctInfo.h"
#include "uuencode.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char far SECTION_ServerAccount[];
extern char far ENTRY_ServerAccountName[];
extern char far ENTRY_ServerPassword[];

/////////////////////////////////////////////////////////////////////////////
// CAccountInfo

IMPLEMENT_DYNCREATE(CAccountInfo, CBasicRequestInfo)

CAccountInfo::CAccountInfo()
{
	m_strRequestedAccountID = "";
	m_strServerSuggestedAccountID = "";
	m_strCustLastName = "";
	m_strCustFirstName = "";
	m_strStreet1 = "";
	m_strStreet2 = "";
	m_strCity = "";
	m_strState = "";
	m_strCountry = "";
	m_strZip = "";
	m_strEMailAddr = "";
	m_strPhone = "";
}

CAccountInfo::~CAccountInfo()
{
}

void CAccountInfo::ProcessRequest(CString& pString)
{
	CString strServer = GetBaseServerUrlAddress();
	strServer += "/EstablishAccount"; //.idc";
	SetServerUrlAddress(strServer);

	SetAccountID(m_strRequestedAccountID);

	CBasicRequestInfo::ProcessRequest(pString);

	CString str;

	str.Format("&Op=%s",m_strOperation);
	pString += str;

	str.Format("&Last=%s", m_strCustLastName);
	pString += str;

	str.Format("&First=%s", m_strCustFirstName);
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

	str.Format("&EMail=%s", m_strEMailAddr);
	pString += str;

	str.Format("&Phone=%s", m_strPhone);
	pString += str;

	UrlEncodeString(pString);

}

UINT CAccountInfo::ProcessResponse(CHttpFile& pHttpFile)
{
#ifdef	ARTSTORE
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

	UINT uStatusCode = atoi((LPCTSTR)GetStatusCode());
	if(uStatusCode == 0 || uStatusCode == 15) //success or in use
	{

		CString strSearch;
		
		strSearch = "AccountID=";
		CString strTemp = "";
		ParseFileForValue(strBuffer, strSearch, strTemp);
		m_strServerSuggestedAccountID = strTemp;

		if(m_strRequestedAccountID == strTemp){
			
			SetAccountID(strTemp);

			CString strResult;
			CString strNotify;
			strNotify.LoadString(IDS_PGS_ACCOUNTID_NOTIFICATION);
			strResult.Format(strNotify, GetAccountID());
			AfxMessageBox(strResult, MB_ICONINFORMATION);

			//at this point, the server has created our accountID (if new)

			CUUEncoder UUEncoder;

			GET_PMWAPP()->WriteProfileString(SECTION_ServerAccount,
														ENTRY_ServerAccountName,
														UUEncoder.EncodeString(GetAccountID()));
			GET_PMWAPP()->WriteProfileString(SECTION_ServerAccount,
														ENTRY_ServerPassword,
														UUEncoder.EncodeString(GetPassword()));
		}else{
			//try to establish account again
			return 100;
		}

	}

	if(!GetStatusCode())
	  return 1;
	else
	  return 0;
#endif
}

BEGIN_MESSAGE_MAP(CAccountInfo, CBasicRequestInfo)
	//{{AFX_MSG_MAP(CAccountInfo)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAccountInfo message handlers


/////////////////////////////////////////////////////////////////////////////
// CGetAccountInfoRequest

IMPLEMENT_DYNCREATE(CGetAccountInfoRequest, CAccountInfo)

CGetAccountInfoRequest::CGetAccountInfoRequest()
{
}

CGetAccountInfoRequest::~CGetAccountInfoRequest()
{
}

void CGetAccountInfoRequest::ProcessRequest(CString& pString)
{
	CString strServer = GetBaseServerUrlAddress();
	strServer += "/GetAccountInfo"; //.idc";
	SetServerUrlAddress(strServer);

	CBasicRequestInfo::ProcessRequest(pString);
}

UINT CGetAccountInfoRequest::ProcessResponse(CHttpFile& pHttpFile)
{
#ifdef	ARTSTORE
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
	
	UINT nRet = atoi((LPCTSTR)GetStatusCode());
	if(nRet == 0) //success
	{
		CString strSearch;
		
		strSearch = "Last=";
		ParseFileForValue(strBuffer, strSearch, GetCustLastName());

		strSearch = "First=";
		ParseFileForValue(strBuffer, strSearch, GetCustFirstName());

		strSearch = "Street1=";
		ParseFileForValue(strBuffer, strSearch, GetStreet1());

		strSearch = "Street2=";
		ParseFileForValue(strBuffer, strSearch, GetStreet2());

		strSearch = "City=";
		ParseFileForValue(strBuffer, strSearch, GetCity());

		strSearch = "State=";
		ParseFileForValue(strBuffer, strSearch, GetState());

		strSearch = "Country=";
		ParseFileForValue(strBuffer, strSearch, GetCountry());

		strSearch = "ZipCode=";
		ParseFileForValue(strBuffer, strSearch, GetZipCode());

		strSearch = "EMail=";
		ParseFileForValue(strBuffer, strSearch, GetEMail());

		strSearch = "Phone=";
		ParseFileForValue(strBuffer, strSearch, GetPhone());

	}

	  return nRet;
#endif
}



/////////////////////////////////////////////////////////////////////////////
// CUpdateAccountRequest

IMPLEMENT_DYNCREATE(CUpdateAccountRequest, CAccountInfo)

CUpdateAccountRequest::CUpdateAccountRequest()
{
	m_strNewPassword = "";
}

CUpdateAccountRequest::~CUpdateAccountRequest()
{
}

void CUpdateAccountRequest::ProcessRequest(CString& pString)
{
	CString strServer = GetBaseServerUrlAddress();
	strServer += "/UpdateAccount"; //.idc";
	SetServerUrlAddress(strServer);

	CBasicRequestInfo::ProcessRequest(pString);

	CString str;
	CString TempString;

	TempString = pString;

	//str.Format("&Op=%s", m_strOperation);
	//TempString += str;

	str.Format("&Last=%s", m_strCustLastName);
	TempString += str;

	str.Format("&First=%s", m_strCustFirstName);
	TempString += str;

	str.Format("&Street1=%s", m_strStreet1);
	TempString += str;

	str.Format("&Street2=%s", m_strStreet2);
	TempString += str;

	str.Format("&City=%s", m_strCity);
	TempString += str;

	str.Format("&State=%s", m_strState);
	TempString += str;

	str.Format("&Country=%s", m_strCountry);
	TempString += str;

	str.Format("&Zip=%s", m_strZip);
	TempString += str;

	str.Format("&EMail=%s", m_strEMailAddr);
	TempString += str;

	str.Format("&Phone=%s", m_strPhone);
	TempString += str;

	str.Format("&PwdNew=%s", m_strNewPassword);
	TempString += str;

	pString = TempString;

	UrlEncodeString(pString);
}

UINT CUpdateAccountRequest::ProcessResponse(CHttpFile& pHttpFile)
{
#ifdef	ARTSTORE
	return 1;
#else
	//Take the HttpFile and parse it.
	UINT uRet = 1;
	UINT nFileLength = pHttpFile.GetLength();
	
	CString strBuffer;
	CString strSearch;
	CString strList;
	LPSTR b = strBuffer.GetBuffer(nFileLength);
	pHttpFile.Read(b, nFileLength);
	strBuffer.ReleaseBuffer();

	uRet = CBasicRequestInfo::ProcessResponse(strBuffer);
#if 0
	if(uRet == 0)
	{
		if(GetNewPassword() != "")
		{
			if(GetNewPassword() != GetPassword())
			{
				LPSTR s = GetNewPassword().GetBuffer(255);
				//WritePrivateProfileString("Server Account", "Password", s, "Pmw.ini");
				GET_PMWAPP()->WritePrivateProfileString(SECTION_ServerAccount, ENTRY_ServerPassword, s, GET_PMWAPP()->main_ini);
				GetNewPassword().ReleaseBuffer();
			}
		}
	}
#endif

	return uRet;
#endif
}

