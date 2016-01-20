#pragma once

class CEmail
{
public:
	CEmail(){};
	~CEmail(){};

public:
	BOOL GetExpressEmailAddr(CString& strEmailAddr);
	BOOL GetOutlookPopEmailAddr(CString& strEmailAddr);
	BOOL GetExchangeServerEmailAddr(CString& strEmailAddr);
};
