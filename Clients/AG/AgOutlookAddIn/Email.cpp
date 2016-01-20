#include "stdafx.h"
#include "Email.h"
#include "RegKeys.h"

/////////////////////////////////////////////////////////////////////////////////////////
BOOL CEmail::GetExpressEmailAddr(CString& strEmailAddr)
{
	CRegKey regkey;
	if (regkey.Open(HKEY_CURRENT_USER, REGKEY_INTERNET_ACCOUNTMGR) != ERROR_SUCCESS)
		return FALSE;

	DWORD dwLength = MAX_PATH;
	char szDefaultAccount[MAX_PATH];
	if (regkey.QueryStringValue(REGVAL_DEFAULTMAILACCOUNT, szDefaultAccount, &dwLength) != ERROR_SUCCESS)
		return FALSE;

	CString strDefaultAccountKey = REGKEY_INTERNET_ACCOUNTS;
	strDefaultAccountKey += szDefaultAccount;
	if (regkey.Open(HKEY_CURRENT_USER, strDefaultAccountKey) != ERROR_SUCCESS)
		return FALSE;

	dwLength = MAX_PATH;
	char szEmailAddr[MAX_PATH];
	if (regkey.QueryStringValue(REGVAL_EMAILADDRESS, szEmailAddr, &dwLength) != ERROR_SUCCESS)
		return FALSE;

	strEmailAddr = szEmailAddr;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
BOOL CEmail::GetOutlookPopEmailAddr(CString& strEmailAddr)
{
	CRegKey regkey;
	if (regkey.Open(HKEY_CURRENT_USER, REGKEY_OUTLOOK_ACCOUNTMGR) != ERROR_SUCCESS)
		return FALSE;

	DWORD dwLength = MAX_PATH;
	char szDefaultAccount[MAX_PATH];
	if (regkey.QueryStringValue(REGVAL_DEFAULTMAILACCOUNT, szDefaultAccount, &dwLength) != ERROR_SUCCESS)
		return FALSE;

	CString strDefaultAccountKey = REGKEY_OUTLOOK_ACCOUNTS;
	strDefaultAccountKey += szDefaultAccount;
	if (regkey.Open(HKEY_CURRENT_USER, strDefaultAccountKey) != ERROR_SUCCESS)
		return FALSE;

	dwLength = MAX_PATH;
	char szEmailAddr[MAX_PATH];
	if (regkey.QueryStringValue(REGVAL_EMAILADDRESS, szEmailAddr, &dwLength) != ERROR_SUCCESS)
		return FALSE;

	strEmailAddr = szEmailAddr;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////////////////
BOOL CEmail::GetExchangeServerEmailAddr(CString& strEmailAddr)
{
	return FALSE;
}
