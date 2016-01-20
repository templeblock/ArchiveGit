#include "stdafx.h"
#include "HTTPClient.h"
#include "Utility.h"
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib") // for ::timeGetTime
#include <wininet.h>
#pragma comment(lib, "wininet.lib")


//////////////////////////////////////////////////////////////////////
CHTTPClient::CHTTPClient(IWebBrowser2 *pBrowser)
{
	m_pBrowser = pBrowser;
}

//////////////////////////////////////////////////////////////////////
CHTTPClient::~CHTTPClient()
{
}

//////////////////////////////////////////////////////////////////////
BOOL CHTTPClient::Request(LPCTSTR szURL, int nMethod, LPCTSTR szBoundary, BOOL bWait)
{
	BOOL bSuccess = TRUE;
	
	try
	{
		switch(nMethod)
		{
			case CHTTPClient::RequestGetMethod:
			default:
				bSuccess = RequestGet(szURL);
				break;
			case CHTTPClient::RequestPostMethod:		
				bSuccess = RequestPost(szURL);
				break;
			case CHTTPClient::RequestPostMethodMultiPartsFormData:
				bSuccess = RequestPostMultiPartsFormData(szURL, szBoundary);
				break;
		}
	}
	catch(...)
	{
		CString szMsg;
		szMsg.Format(_T("Request(%s, %d)"), szURL, nMethod); 
		GetError(szMsg);
		return FALSE;
	}
	
	// Wait for request to complete.
	if (bSuccess && bWait)
		bSuccess = Wait();

	return bSuccess;
}

//////////////////////////////////////////////////////////////////////
BOOL CHTTPClient::RequestGet(LPCTSTR szURL)
{
	// Send the form data To URL As a Get request
	CComVariant varFlags =  navNoHistory |  navNoReadFromCache | navNoWriteToCache;
	CComVariant varTargetFrameName = "_self";

	HRESULT hr = m_pBrowser->Navigate(CComBSTR(szURL), &varFlags, &varTargetFrameName, NULL, NULL);
	return SUCCEEDED(hr);
}

//////////////////////////////////////////////////////////////////////
BOOL CHTTPClient::RequestPost(LPCTSTR szURL)
{
	LPCTSTR szContentType = TEXT("Content-Type: application/x-www-form-urlencoded\r\n");
	CString szData;
	DWORD dwBufferSize = GetPostArguments(szData);
	if (!dwBufferSize)
		return FALSE;
	
	BYTE* pBuffer = (BYTE*)malloc(dwBufferSize+1);
	if (!pBuffer)
		return FALSE;

	::memcpy(pBuffer, szData.GetBuffer(dwBufferSize), dwBufferSize);

	// Send the form data To URL As a POST request
	CComVariant varFlags = navNoHistory |  navNoReadFromCache | navNoWriteToCache;
	CComVariant varTargetFrameName = "_self";

	CComSafeArray<BYTE> Array;
	Array.Add(dwBufferSize, pBuffer);
	CComVariant varPostData = Array;
	
	CComVariant varHeaders = CComBSTR(szContentType);
	HRESULT hr = m_pBrowser->Navigate(CComBSTR(szURL), &varFlags, &varTargetFrameName, &varPostData, &varHeaders);
	if (FAILED(hr))
		return FALSE;

	free(pBuffer);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
BOOL CHTTPClient::RequestPostMultiPartsFormData(LPCTSTR szURL, LPCTSTR szBoundary)
{
	CString szContentType = _T("Content-Type: multipart/form-data; boundary=") + CString(szBoundary) + _T("\r\n");
	
	// Allocate post multi-parts form data arguments
	PBYTE pPostBuffer = NULL;
	DWORD dwPostBufferLength = AllocMultiPartsFormData(pPostBuffer, szBoundary);
	if (dwPostBufferLength == 0)
		return FALSE;
	
	// Send the form data To URL As a POST request
	CComVariant varFlags =  navNoHistory |  navNoReadFromCache | navNoWriteToCache;
	CComVariant varTargetFrameName = "_self";
	CComSafeArray<BYTE> Array;
	Array.Add(dwPostBufferLength, pPostBuffer);
	CComVariant varPostData = Array;

	CComVariant varHeaders = CComBSTR(szContentType);
	HRESULT hr = m_pBrowser->Navigate(CComBSTR(szURL), &varFlags, &varTargetFrameName, &varPostData, &varHeaders);
	if (FAILED(hr))
		return FALSE;

	// Free post multi-parts form data arguments
	FreeMultiPartsFormData(pPostBuffer);

	return TRUE;
}

//////////////////////////////////////////////////////////////////////
VOID CHTTPClient::AddPostArguments(LPCTSTR szName, LPCTSTR szValue, BOOL bBinary)
{
	GenericHTTPArgument	arg;

	arg.szName = szName;
	arg.szValue = szValue;

	if (!bBinary)
		arg.dwType = CHTTPClient::TypeNormal;
	else
		arg.dwType = CHTTPClient::TypeBinary;	

	_vArguments.push_back(arg);
}

//////////////////////////////////////////////////////////////////////
VOID CHTTPClient::AddPostArguments(LPCTSTR szName, DWORD nValue)
{
	GenericHTTPArgument	arg;

	arg.szName = szName;
	arg.szValue.Format(_T("%d"), nValue);
	arg.dwType = CHTTPClient::TypeNormal;

	_vArguments.push_back(arg);
}

//////////////////////////////////////////////////////////////////////
DWORD CHTTPClient::GetPostArguments(BYTE **pData)
{
	DWORD dwSize = 0; // include terminating null
	DWORD dwLen = 0;
	DWORD dwEqLen = 0;
	DWORD dwNameLen = 0;
	DWORD dwValueLen = 0;
	DWORD dwAmperLen = 0;
	BYTE *pBuffer = NULL;

	std::vector<GenericHTTPArgument>::iterator itArg;
	for (itArg=_vArguments.begin(); itArg<_vArguments.end(); )
	{
		dwNameLen = itArg->szName.GetLength();
		dwEqLen = 1; // "=";
		dwValueLen = itArg->szValue.GetLength();
		dwLen = dwNameLen + dwEqLen +  dwValueLen;

		if (!pBuffer)
			pBuffer = (BYTE*)malloc(dwLen); // include terminating null
		else
			pBuffer = (BYTE*)realloc(pBuffer, dwSize + dwLen);

		if (!pBuffer)
			return 0;
	
		::memcpy(pBuffer + dwSize, itArg->szName.GetBuffer(dwNameLen), dwNameLen);
		dwSize += dwNameLen;
		::memcpy(pBuffer + dwSize, "=", dwEqLen);
		dwSize += dwEqLen;
		::memcpy(pBuffer + dwSize, itArg->szValue.GetBuffer(dwValueLen), dwValueLen);
		dwSize += dwValueLen;
		
		if ((++itArg)<_vArguments.end())
		{
			dwAmperLen = 1; //"&"
			pBuffer = (BYTE*)realloc(pBuffer, dwSize + dwAmperLen);
			::memcpy(pBuffer + dwSize, "&", dwAmperLen);
			dwSize += dwAmperLen;
		}
	}

	pBuffer = (BYTE*)realloc(pBuffer, dwSize + 1); // Add  null
	pBuffer[dwSize] = '\0';
	*pData = pBuffer; // Caller must free pData.

	return dwSize;
}

//////////////////////////////////////////////////////////////////////
DWORD CHTTPClient::GetPostArguments(CString &szData)
{
	std::vector<GenericHTTPArgument>::iterator itArg;
	for (itArg=_vArguments.begin(); itArg<_vArguments.end(); )
	{
		szData += itArg->szName;
		szData += "=";
		szData += itArg->szValue;
	
		if ((++itArg)<_vArguments.end())
			szData += "&";
	}

	return szData.GetLength();
}

//////////////////////////////////////////////////////////////////////
VOID CHTTPClient::InitilizePostArguments()
{
	_vArguments.clear();
}

//////////////////////////////////////////////////////////////////////
VOID CHTTPClient::FreeMultiPartsFormData(PBYTE &pBuffer)
{
	if (pBuffer)
	{
		::LocalFree(pBuffer);
		pBuffer = NULL;
	}
}

//////////////////////////////////////////////////////////////////////
DWORD CHTTPClient::AllocMultiPartsFormData(PBYTE &pInBuffer, LPCTSTR szBoundary)
{
	if (pInBuffer)
	{
		::LocalFree(pInBuffer);
		pInBuffer = NULL;
	}

	pInBuffer=(PBYTE)::LocalAlloc(LPTR, GetMultiPartsFormDataLength());	
	std::vector<GenericHTTPArgument>::iterator itArgv;

	DWORD dwPosition=0;
	DWORD dwBufferSize=0;

	for (itArgv=_vArguments.begin(); itArgv<_vArguments.end(); ++itArgv)
	{
		PBYTE pBuffer=NULL;

		// SET MULTI_PARTS FORM DATA BUFFER
		switch(itArgv->dwType)
		{
			case CHTTPClient::TypeNormal:
				{
					pBuffer=(PBYTE)::LocalAlloc(LPTR, __SIZE_HTTP_HEAD_LINE*4);

					_stprintf_s(	(TCHAR*)pBuffer,	
									__SIZE_HTTP_HEAD_LINE*4,
									"--%s\r\n"
									"Content-Disposition: form-data; name=\"%s\"\r\n"
									"\r\n"
									"%s\r\n",
									szBoundary,
									itArgv->szName,
									itArgv->szValue);

					dwBufferSize=_tcslen((TCHAR*)pBuffer);

					break;
				}
			case CHTTPClient::TypeBinary:
				{
					DWORD	dwNumOfBytesRead=0;
					DWORD	dwTotalBytes=0;

					HANDLE hFile=::CreateFile(itArgv->szValue, 
											GENERIC_READ,		// desired access
											FILE_SHARE_READ,	// share mode
											NULL,				// security attribute
											OPEN_EXISTING,		// create disposition
											FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, // flag and attributes
											NULL);				// template file handle

					if (hFile == INVALID_HANDLE_VALUE)
					{
						CString szMsg;
						szMsg.Format(_T("AllocMultiPartsFormData()=>CreateFile(%s)"), itArgv->szValue); 
						GetError(szMsg);
						return 0;
					}

					DWORD	dwSize=::GetFileSize(hFile, NULL);

					if (dwSize == INVALID_FILE_SIZE)
					{
						CString szMsg;
						szMsg.Format(_T("AllocMultiPartsFormData()=>GetFileSize(%X, NULL)"), hFile); 
						GetError(szMsg);
						return 0;
					}
					
					SIZE_T uSize = __SIZE_HTTP_HEAD_LINE*3+dwSize+1;
					
					pBuffer=(PBYTE)::LocalAlloc(LPTR, uSize);
					BYTE pBytes[__SIZE_BUFFER+1]="";
					
					_stprintf_s( (TCHAR*)pBuffer,
									uSize,
									"--%s\r\n"
									"Content-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n"
									"Content-Type: %s\r\n"
									"\r\n",
									szBoundary,
									itArgv->szName, itArgv->szValue,
									GetContentType(itArgv->szValue));

					DWORD	dwBufPosition=_tcslen((TCHAR*)pBuffer);	
			
					BOOL bReadFileSuccess = ::ReadFile(hFile, pBytes, __SIZE_BUFFER, &dwNumOfBytesRead, NULL);
					while(bReadFileSuccess && dwNumOfBytesRead>0 && dwTotalBytes<=dwSize)
					{
						::CopyMemory((pBuffer+dwBufPosition+dwTotalBytes), pBytes, dwNumOfBytesRead);
						::ZeroMemory(pBytes, __SIZE_BUFFER+1);
						dwTotalBytes+=dwNumOfBytesRead;	
						bReadFileSuccess = ::ReadFile(hFile, pBytes, __SIZE_BUFFER, &dwNumOfBytesRead, NULL);
					}

					if (!bReadFileSuccess)
					{
						CString szMsg;
						szMsg.Format(_T("AllocMultiPartsFormData()=>::ReadFile(%X, NULL)"), hFile); 
						GetError(szMsg);
						return 0;
					}

					::CopyMemory((pBuffer+dwBufPosition+dwTotalBytes), "\r\n", _tcslen("\r\n"));
					::CloseHandle(hFile);

					dwBufferSize=dwBufPosition+dwTotalBytes+_tcslen("\r\n");
					break;	
				}
		}
	
		::CopyMemory((pInBuffer+dwPosition), pBuffer, dwBufferSize);
		dwPosition+=dwBufferSize;

		if (pBuffer)
		{
			::LocalFree(pBuffer);
			pBuffer = NULL;
		}
	}

	::CopyMemory((pInBuffer+dwPosition), "--", 2);
	::CopyMemory((pInBuffer+dwPosition+2), szBoundary, _tcslen(szBoundary));
	::CopyMemory((pInBuffer+dwPosition+2+_tcslen(szBoundary)), "--\r\n", 3);

	return dwPosition+5+_tcslen(szBoundary);
}

//////////////////////////////////////////////////////////////////////
DWORD CHTTPClient::GetMultiPartsFormDataLength()
{
	std::vector<GenericHTTPArgument>::iterator itArgv;

	DWORD	dwLength=0;

	for (itArgv=_vArguments.begin(); itArgv<_vArguments.end(); ++itArgv)
	{
		switch(itArgv->dwType)
		{
			case CHTTPClient::TypeNormal:
				dwLength+=__SIZE_HTTP_HEAD_LINE*4;
				break;
			case CHTTPClient::TypeBinary:
			{
				HANDLE hFile=::CreateFile(itArgv->szValue, 
										GENERIC_READ, // desired access
										FILE_SHARE_READ, // share mode
										NULL, // security attribute
										OPEN_EXISTING, // create disposition
										FILE_ATTRIBUTE_NORMAL, // flag and attributes
										NULL); // template file handle

				dwLength+=__SIZE_HTTP_HEAD_LINE*3+::GetFileSize(hFile, NULL);
				::CloseHandle(hFile);
				break;
			}
			default:
				break;
		}

	}

	return dwLength;
}

//////////////////////////////////////////////////////////////////////
LPCTSTR CHTTPClient::GetContentType(LPCTSTR szName)
{
	static TCHAR	szReturn[1024]="";
	LONG	dwLen=1024;
	DWORD	dwDot=0;

	for (dwDot=_tcslen(szName);dwDot>0;dwDot--)
	{
		if (!_tcsncmp((szName+dwDot),".", 1))
			break;
	}

	HKEY	hKEY;
	LPTSTR	szWord=(char*)(szName+dwDot);

	// Check registered file types;
	TCHAR szDefaultType[] = _T("application/octet-stream");
	int nDefaultLen =  _tcslen(szDefaultType) + 1;
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT, szWord, 0, KEY_QUERY_VALUE, &hKEY)==ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hKEY, TEXT("Content Type"), NULL, NULL, (LPBYTE)szReturn, (unsigned long*)&dwLen)!=ERROR_SUCCESS)
			_tcsncpy_s(szReturn, dwLen, szDefaultType, nDefaultLen);
		RegCloseKey(hKEY);
	}
	else
	{
		_tcsncpy_s(szReturn, dwLen, szDefaultType, nDefaultLen);
	}

	return szReturn;
}

//////////////////////////////////////////////////////////////////////
VOID CHTTPClient::ParseURL(LPCTSTR szURL, LPTSTR szProtocol, LPTSTR szAddress, DWORD &dwPort, LPTSTR szURI){

	TCHAR szPort[__SIZE_BUFFER]="";
	DWORD dwPosition=0;
	DWORD dwProtocolLen=0;
	BOOL bFlag=FALSE;

	while(_tcslen(szURL)>0 && dwPosition<_tcslen(szURL) && _tcsncmp((szURL+dwPosition), ":", 1))
		++dwPosition;

	if (szProtocol)
		dwProtocolLen = _tcslen(szProtocol);

	if (!_tcsncmp((szURL+dwPosition+1), "/", 1)) // is PROTOCOL
	{	
		if (szProtocol && dwPosition <= dwProtocolLen+1)
		{
			_tcsncpy_s(szProtocol, dwProtocolLen, szURL, dwPosition);
			szProtocol[dwPosition]=0;
		}
		bFlag=TRUE;
	}
	else // is HOST 
	{	
		if (szProtocol && dwProtocolLen >= 4)
		{
			_tcsncpy_s(szProtocol, dwProtocolLen, "http", 4);
			szProtocol[5]=0;
		}
	}

	DWORD dwStartPosition=0;
	
	if (bFlag)
	{
		dwStartPosition=dwPosition+=3;				
	}
	else
	{
		dwStartPosition=dwPosition=0;
	}

	bFlag=FALSE;
	while(_tcslen(szURL)>0 && dwPosition<_tcslen(szURL) && _tcsncmp((szURL+dwPosition), "/", 1))
			++dwPosition;

	DWORD dwFind=dwStartPosition;

	for (;dwFind<=dwPosition;dwFind++)
	{
		if (!_tcsncmp((szURL+dwFind), ":", 1)) // find PORT
		{
			bFlag=TRUE;
			break;
		}
	}

	int nAddressSize = _tcslen(szAddress)+1;
	int nURISize = _tcslen(szURI)+1; 
	if (bFlag)
	{
		TCHAR sztmp[__SIZE_BUFFER]="";
		_tcsncpy_s(sztmp, __SIZE_BUFFER, (szURL+dwFind+1), dwPosition-dwFind);
		dwPort=_ttol(sztmp);
		_tcsncpy_s(szAddress, nAddressSize, (szURL+dwStartPosition), dwFind-dwStartPosition);
	}
	else if (!_tcscmp(szProtocol,"https"))
	{
		dwPort=INTERNET_DEFAULT_HTTPS_PORT;
		_tcsncpy_s(szAddress, nAddressSize, (szURL+dwStartPosition), dwPosition-dwStartPosition);
	}
	else 
	{
		dwPort=INTERNET_DEFAULT_HTTP_PORT;
		_tcsncpy_s(szAddress, nAddressSize, (szURL+dwStartPosition), dwPosition-dwStartPosition);
	}

	if (dwPosition<_tcslen(szURL))  // find URI
	{
		_tcsncpy_s(szURI, nURISize, (szURL+dwPosition), _tcslen(szURL)-dwPosition);
	}
	else
	{
		szURI[0]=0;
	}
}

//////////////////////////////////////////////////////////////////////
BOOL CHTTPClient::Wait(DWORD dwExpire)
{
	// Wait for the document to become ready, or time out
	VARIANT_BOOL bBusy = true; 
	DWORD dwMaxTime = ::timeGetTime() + dwExpire;
	while (bBusy)
	{
		m_pBrowser->get_Busy(&bBusy);
		MSG msg;
		BOOL bRet = ::GetMessage(&msg, NULL, 0, 0);
		if (!bRet)
			return false; // WM_QUIT, exit message loop

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);

		if (::timeGetTime() > dwMaxTime)
			return false;
	}

	return true;//(!(BOOL)bBusy);
}
