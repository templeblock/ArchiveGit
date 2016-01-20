#pragma once
#include "HelperFunctions.h"

class CMsAddInBase;

class CStationeryManager
{
public:
	CStationeryManager(CMsAddInBase* pAddin);
	virtual ~CStationeryManager(void);
	void SetClientType(DWORD dwType);
	bool SetDefault(DownloadParms& Parms);
	bool NeedToAdd();
	bool AddStationery(IHTMLDocument2 *pDoc, DownloadParms &Parms);
	bool ClearDefault();
	bool GetDefaultStationeryProductInfo(IHTMLDocument2 *pDoc);
	bool LoadDefaultStationery();
	const CString& GetHTML();
	CString& GetPath();
	CString& GetFile();

protected:
	bool CreateFolder();

protected:
	CMsAddInBase* m_pAddin;
	CString m_szFolder;
	CString m_szFile;
	CString m_szHTML;
	DownloadParms m_DefaultProductInfo;
	DWORD m_dwClientType;
};