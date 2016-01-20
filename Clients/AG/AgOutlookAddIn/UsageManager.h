#pragma once
#include "Authenticate.h"
#include <map>
#include <vector>

struct DownloadParms;

static LPCTSTR TAG_MEMNUM		= _T("memnum");		// Customer #
static LPCTSTR TAG_EMAIL		= _T("fromemail");	// Userid
static LPCTSTR TAG_USAGE		= _T("usage");		// usage data
static LPCTSTR TAG_DELIVMETH	= _T("delivmeth");	// Outlook=1  or Outlook Express=2
static LPCTSTR TAG_VER_EMAIL	= _T("mua");	
static LPCTSTR TAG_VER_IE		= _T("browser");		
static LPCTSTR TAG_VER_OS		= _T("os");		
static LPCTSTR TAG_VER_JM		= _T("version");		
static LPCTSTR TAG_USAGE_DEBUG	= _T("debug");		// debug: off=0; on=1

static LPCTSTR USAGE_SUCCESS_SEND		=	"USAGE SEND SUCCESSFUL";
static LPCTSTR USAGE_ERROR_CONNECT		=	"USAGE CONNECT ERROR";
static LPCTSTR USAGE_ERROR_COCREATE		=	"USAGE COCREATE ERROR";
static LPCTSTR USAGE_ERROR_USERINFO		=	"USAGE USER INFO ERROR";
static LPCTSTR USAGE_ERROR_FILEREAD		=	"USAGE FILE READ ERROR";
static LPCTSTR USAGE_ERROR_FILEWRITE	=	"USAGE FILE WRITE ERROR";
static LPCTSTR USAGE_ERROR_FILEDELETE	=	"USAGE FILE DELETE ERROR";
static LPCTSTR USAGE_ERROR_POST			=	"USAGE HTTP POST ERROR";

static const DWORD METH_OUTLOOK		= 1;
static const DWORD METH_EXPRESS		= 2;
static const DWORD METH_HOTMAIL		= 3;
static const DWORD METH_YAHOO		= 4;
static const DWORD METH_AOL 		= 5;


struct PRODUCTINFOTYPE
{
	DWORD Count;
	DWORD PathPrice;
	DWORD Category;
	DWORD SendCount;
	DWORD AssetType;
};

typedef std::map <DWORD, PRODUCTINFOTYPE> ProductMapType;
typedef std::vector<CString> METATAGLIST;

class CUsageManager
{
public:
	CUsageManager();
	~CUsageManager(void);
	void Initialize();
	void Flush();
	void SetMethod(DWORD dwMeth);
	bool GetSendCount(IHTMLDocument2 *pDoc, DWORD &dwTotal);
	bool Add(DownloadParms& Parms, DWORD dwCount);
	bool AddSendCount(DownloadParms& Parms, DWORD dwCount);
	bool AddAsSendCandidate(IHTMLDocument2 *pDoc, DownloadParms& Parms);
	bool RefreshMetaData(IHTMLDocument2 *pDoc);
	bool GetMetaData(IHTMLDocument2 *pDoc);
	bool RestoreResidualMetaData(IHTMLDocument2 *pDoc);
	bool SaveToFile();
	bool SendIfReady(bool bForce=false);
	bool IsTimeToSend();
	bool ReadTimeStamp(CTime& tTime);
	bool WriteTimeStamp();
	bool ReadFromFile(CString &szBuffer);
	bool CloseFile(HANDLE hFile);
	bool GetOSVersion(CString& szVersion);
	bool GetMailClientVersion(CString& szVersion);
	bool GetIEVersion(CString& szVersion);
	bool GetJMVersion(CString& szVersion);
	bool CreateMetaElement(IHTMLDocument2 *pDoc, LPCTSTR lpszTag);
	bool GetMetaDataFromString(LPCTSTR lpszHtml, LPCTSTR lpszName, METATAGLIST &List);
	bool ClearSendCount();
	BOOL RemoveFile();
	LPCTSTR Send();

protected:
	void EraseAll();
	void CountProductId(LPCTSTR szBody, LPCTSTR szProductID, DWORD &dwCount);
	bool CreateFile(HANDLE &hFile);
	bool WriteToFile(HANDLE hFile, BYTE *pData, DWORD dwSize);
	bool IsJMUProductIDPresent(IHTMLDocument2 *pDoc, LPCTSTR szProductID);
	bool RemoveMetaData(IHTMLDocument2 *pDoc);
	DWORD GetTimeStamp();
	DWORD GetProductData(BYTE **pData);

private:
	std::vector<DownloadParms> m_ResidualMetaData;
	DWORD m_dwMethod;
	ProductMapType m_ProductList;
	DWORD m_dwRecordSize;
	CString m_szUsageFile;
	CAuthenticate m_Authenticate;
};
