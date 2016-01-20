/*
=======================================================================================
Example of use:

CHTTPClient *pClient=new CHTTPClient();

pClient->InitilizePostArguments();
pClient->AddPostArguments(__TAG_USRID, szUserID);
pClient->AddPostArguments(__TAG_JUMIN, szSocialIndex);
pClient->AddPostArguments(__TAG_SRC, szSource);
pClient->AddPostArguments(__TAG_DST, szDestination);			
pClient->AddPostArguments(__TAG_FORMAT, szFormat);
pClient->AddPostArguments(__TAG_SUBJECT, szMessage);

if(bCharge){
	pClient->AddPostArguments(__TAG_CHARGE, "Y");
}else{
	pClient->AddPostArguments(__TAG_CHARGE, "N");
}
pClient->AddPostArguments(__TAG_CPCODE, szCPCode);
pClient->AddPostArguments(__TAG_FILE, szFile, TRUE);

pClient->Request(szURL, CHTTPClient::RequestPostMethodMultiPartsFormData);
=======================================================================================
*/

#pragma once

// Because this will be linked into an ATL library, force the "consumer" to
// address CString's as CAtlString's so we can use the library in an MFC app
#ifdef _AFX
	#include <atlstr.h>
	#define CStringW ATL::CAtlStringW
	#define CStringA ATL::CAtlStringA
	#define CString ATL::CAtlString
#endif

#include <tchar.h>
#include <Exdisp.h>

// use stl
#include <vector>

// PRE-DEFINED BUFFER SIZE
#define	__SIZE_HTTP_ARGUMENT_NAME	256
#define __SIZE_HTTP_ARGUMENT_VALUE	1024

#define __HTTP_VERB_GET	"GET"
#define __HTTP_VERB_POST "POST"
#define __SIZE_HTTP_BUFFER	100000
#define __SIZE_HTTP_RESPONSE_BUFFER	100000
#define __SIZE_HTTP_HEAD_LINE	2048

#define __SIZE_BUFFER	1024
#define __SIZE_SMALL_BUFFER	256

static LPCTSTR DEFAULT_BOUNDARY = _T("--MULTI-PARTS-FORM-DATA-BOUNDARY");


class CHTTPClient 
{
public:					
	typedef struct __GENERIC_HTTP_ARGUMENT // ARGUMENTS STRUCTURE
	{							
		CString	szName;
		CString	szValue;
		DWORD	dwType;
		bool operator==(const __GENERIC_HTTP_ARGUMENT &argV)
		{
			return !_tcscmp(szName, argV.szName) && !_tcscmp(szValue, argV.szValue);
		}
	} GenericHTTPArgument;

	enum RequestMethod // REQUEST METHOD
	{															
		RequestUnknown=0,
		RequestGetMethod=1,
		RequestPostMethod=2,
		RequestPostMethodMultiPartsFormData=3
	};

	enum TypePostArgument // POST TYPE 
	{													
		TypeUnknown=0,
		TypeNormal=1,
		TypeBinary=2
	};

	// CONSTRUCTOR & DESTRUCTOR
	CHTTPClient(IWebBrowser2 *pBrowser);
	virtual ~CHTTPClient();
	
	VOID InitilizePostArguments();

	// HTTP Arguments handler	
	VOID AddPostArguments(LPCTSTR szName, DWORD nValue);
	VOID AddPostArguments(LPCTSTR szName, LPCTSTR szValue, BOOL bBinary = FALSE);

	// HTTP Method handler 
	BOOL Request(LPCTSTR szURL, 
				 int nMethod = CHTTPClient::RequestGetMethod,  
				 LPCTSTR szBoundary=DEFAULT_BOUNDARY, 
				 BOOL bWait=TRUE);
	
	// General Handler
	LPCTSTR GetContentType(LPCTSTR szName);
	VOID ParseURL(LPCTSTR szURL, LPTSTR szProtocol, LPTSTR szAddress, DWORD &dwPort, LPTSTR szURI);

	BOOL Wait(DWORD dwExpire=60000);

	
protected:				

	// HTTP Method handler
	DWORD GetPostArguments(BYTE **pData);
	DWORD GetPostArguments(CString &szData);
	BOOL RequestPost(LPCTSTR szURL);
	BOOL RequestPostMultiPartsFormData(LPCTSTR szURL, LPCTSTR szBoundary);
	BOOL RequestGet(LPCTSTR szURL);
	DWORD AllocMultiPartsFormData(PBYTE &pInBuffer, LPCTSTR szBoundary);
	VOID FreeMultiPartsFormData(PBYTE &pBuffer);
	DWORD GetMultiPartsFormDataLength();

private:
	std::vector<GenericHTTPArgument> _vArguments;			// POST ARGUMENTS VECTOR
	CComPtr<IWebBrowser2> m_pBrowser;
};


#ifdef _AFX
	#undef CStringW
	#undef CStringA
	#undef CString
#endif
