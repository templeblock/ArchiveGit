//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/ContMgr.h 1     3/03/99 6:04p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/ContMgr.h $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 51    2/08/99 8:48p Psasse
// New improved scheduling of Online Greetings
// 
// 50    11/25/98 5:59p Psasse
// RequestExistingAccount no longer takes a parameter
// 
// 49    11/23/98 3:59p Johno
// Changes for initial checked order item
// 
// 48    11/21/98 10:45p Psasse
// 
// 47    11/18/98 10:34p Psasse
// DeleteAllCachedDirectories becomes DeleteCachedDirectories(individual)
// 
// 46    11/15/98 6:47p Psasse
// Two new functions for Updating Flag and Checking for new internet
// content
// 
// 45    10/30/98 5:25p Jayn
// 
// 44    10/29/98 4:04p Jayn
// 
// 43    10/28/98 9:11p Psasse
// added SendAccount to the interface
// 
// 42    10/28/98 1:48p Jayn
// More polish for ordering and downloading.
// 
// 41    10/27/98 4:47p Jayn
// More download manager and order wizard refinements.
// 
// 40    10/24/98 9:13p Psasse
// added purchase internet order
// 
// 39    10/24/98 7:34p Jayn
// Download Manager and Download Dialog
// 
// 38    10/22/98 4:00p Jayn
// 
// 37    10/21/98 9:06p Psasse
// added PriceOrder support
// 
// 36    10/21/98 4:14p Jayn
// More improvements to the order process.
// 
// 35    10/19/98 5:41p Jayn
// Beginnings of the online order form.
// 
// 34    10/19/98 11:21a Jayn
// 
// 33    10/19/98 11:17a Psasse
// 
// 32    10/19/98 10:48a Jayn
// More COM restructuring changes.
// 
// 31    10/12/98 4:33p Jayn
// Rearranged some includes to make extraction easier.
// 
// 30    10/11/98 8:55p Psasse
// continued conversion to COM interface
// 
// 29    10/09/98 5:53p Jayn
// New "get graphic" implementation.
// 
// 28    10/07/98 5:46p Jayn
// Changes to the content manager.
// 
// 27    10/05/98 3:03p Jayn
// More content manager cleanup.
// 
// 26    10/03/98 9:16p Psasse
// Added CServerBasicRequest member and parameter to "open"
// 
// 25    10/02/98 7:10p Jayn
// More content manager changes
// 
// 24    9/26/98 8:10p Psasse
// Major overhaul of client-side responsibilities. GetInternetContent,
// IsFileLocallyCached and MakeCachePath have been simplified
// 
// 23    9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 22    9/22/98 8:58p Psasse
// cleaned  up code
// 
// 21    9/22/98 3:32p Psasse
// converting LPCSTR'S to CString&
// 
// 20    9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 2     5/31/98 5:19p Hforman
// 
//////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTENTMANAGER_H__4ECA5A01_BB35_11D1_9909_00A0240C7400__INCLUDED_)
#define AFX_CONTENTMANAGER_H__4ECA5A01_BB35_11D1_9909_00A0240C7400__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "iuserset.h"			// IUserSettings
#include "ipathmgr.h"			// IPathManager
#include "icontsrv.h"			// IContentServer
#include "icontmgr.h"			// IContentManager
#include "iconnmgr.h"			// IConnectionManager

#define USER_ESCAPE  175

class CDownloadStatusDialog;
class CCompressInfo;

#ifdef _DEBUG
#define DEFAULT_SERVER_URL "205.156.205.221:31597"
#else
#define DEFAULT_SERVER_URL "pmgsdata.printmaster.com:31597"
#endif
#define DEFAULT_SERVER_BASE_PATH "scripts2/MindscapePGS"

/////////////////////////////////////////////////////////////////////////////
// CContentCommunicationException
// This indicates communication could not be established with the server
// for some reason. The error code is a standard Windows (GetError()) error.

class CContentCommunicationException : public CException
{
	DECLARE_DYNAMIC(CContentCommunicationException);
public:
	DWORD m_dwError;
	CContentCommunicationException(DWORD dwError)
		{ m_dwError = dwError; }
};

/////////////////////////////////////////////////////////////////////////////
// CContentServerException
// This indicates the server reported an error before the database code
// could process the request. The error code is a standard HTTP server result
// code (500, 100, etc.)

class CContentServerException : public CException
{
	DECLARE_DYNAMIC(CContentServerException);
public:
	DWORD m_dwError;
	CContentServerException(DWORD dwError)
		{ m_dwError = dwError; }
};

/////////////////////////////////////////////////////////////////////////////
// CContentManagerConnectionCallback
// The callback during a request. We typically use this during a download
// request when we have a progress dialog on the screen.

class CContentManagerConnectionCallback : public IServerConnectionCallback
{
public:
	CContentManagerConnectionCallback(CDownloadStatusDialog* pDialog);
	virtual DWORD OnStatusChange(DWORD dwInternetStatus);
protected:
	CDownloadStatusDialog* m_pDialog;
};

/////////////////////////////////////////////////////////////////////////////
// CContentManagerDownloadCallback
// The callback during a download. It stores the data in memory.

class CContentManagerDownloadCallback : public IServerDownloadCallback
{
public:
	CContentManagerDownloadCallback(CDownloadStatusDialog* pDialog);
	~CContentManagerDownloadCallback();

	// Get the downloaded data when done.
	LPBYTE GetDownloadedData(void)
		{ return m_pBuffer; }
	DWORD GetDownloadedDataSize(void) const
		{ return m_dwBufferSize; }

protected:
	virtual DWORD OnReceiveData(LPVOID pData, DWORD dwDataSize, DWORD dwDataTotal);
protected:
	CDownloadStatusDialog* m_pDialog;
	DWORD m_dwTotalRead;
	CString m_csFileName;
	LPBYTE m_pBuffer;
	DWORD m_dwBufferSize;
};

/////////////////////////////////////////////////////////////////////////////
// CContentManager object

class CContentManager : public CCmdTarget
{
// Operations
public:

	CContentManager();
	virtual ~CContentManager();

	void Open(IPathManager* pPathManager,
				 IUserSettings* pUserSettings,
				 IConnectionManager* pConnectionManager,
				 IContentServer* pContentServer,
				 LPCSTR pszClientSKU,
				 LPCSTR pszClientVersion);
	void Close(void);

	//called from PathManager
	UINT GetInternetContent(LPCSTR pszColl, LPCSTR pszFileName, CString& csBuf);

	//called from Browser
	BOOL IsFileLocallyCached(LPCSTR strCollName, LPCSTR strFileName);
	BOOL UpdateAccount(void);
	BOOL CreateNewAccount(void);
	BOOL RequestExistingAccount(void);
	BOOL DeleteCachedDirectories(CWnd* pParent, LPCSTR strCollection);

	//called from artstore
	int LicenseProducts(LPCSTR pszProduct = NULL, LPCSTR pszInitialSelection = NULL);

	//called from PGSOrderWizard
	int PriceInternetOrder(const CServerPriceOrderInfo& OrderInfo, CServerPriceInfo& PriceInfo);
	int PurchaseInternetOrder(const CServerPurchaseInfo& PurchaseInfo, CServerPurchaseResult& PurchaseResult);

	//called from the browser(?)
	BOOL CheckForNewInternetContent();
	void UpdateWhenLastSeenInternetContent(void);

	LPCSTR GetAccountID ( void ) const
		{  return m_csAccountID;}
// Attributes
private:
	friend class CBasicRequestInfo;

	CString			 m_csCacheDirectory;

	BOOL m_fNoAd;

	// Data server connection parameters
	CStringList m_cslDataServers;
	CString m_csDataServerBasePath;

	// Download server connection parameters.
	CStringList m_cslDownloadServers;

	// Basic request parameters.
	CString m_csAccountID;		// User account ID (stored in the user settings)
	CString m_csPassword;		// User password (stored in the user settings)
	CString m_csSessionID;		// Our session ID.
	int m_nRequest;				// What request are we on?
	CString m_csClientSKU;		// Client SKU
	CString m_csClientVersion;	// Client version

	// Purchase info (returned by Login)
	CStringArray m_csaShipMethods;		// Shipping methods available
	CStringArray m_csaCardTypes;			// Credit cards accepted

	// Collection revision.
	CString m_csCollectionRevision;

	// User collection revision.
	CString m_csUserCollectionRevision;

	// Components we use.
	IPathManager* m_pPathManager;
	IUserSettings* m_pUserSettings;
	IConnectionManager* m_pConnectionManager;
	IContentServer* m_pContentServer;

protected:
	// new internal to the content manager.
	void SetServerURLAddress(LPCSTR pszServerURLAddress);
	void FillBasicRequest(CServerBasicRequest& Request, BOOL fBumpRequest = TRUE);
	void AddServers(const CStringList& cslNewServers, CStringList& cslServers);
	void SetLoginInfo(LPCSTR pszAccountID, LPCSTR pszPassword);
	void StandardProcessResponseStatus(int nStatus, LPCSTR pszStatus);

	void UpdateCollections(void);
	void UpdateLicensedCollections(BOOL fOverideVersionCheck);

	// Build a revision string from the order form version and the
	// category revision.
	void GetOrderFormRevision(CString& csRevision, LPCSTR pszCategoryRevision);

	// Error handling.
	// Check a request completion. Throws appropriate exception.
	void CheckRequestCompletion(const CServerBasicRequest& Request);
	// Check a request completion; issues standard message boxes for errors.
	int StandardCheckRequestCompletion(const CServerBasicRequest& Request);

	//internal to the ContentManager
	BOOL CheckAccountStatus(void);
	BOOL QueryNewAccount(void);
	BOOL DoAccountInfoDlg(CServerAccountInfo& AccountInfo, CString& csAccountID, CString& csPassword);
	BOOL CheckIfSessionActive(void);
	BOOL CheckForValidConnection(void);
   void ParseStringToLocationAndPassword(CString& strString, CString& strLocation, CString& strPassword);
	CString MakeCachePath(LPCSTR strColl, LPCSTR strFileName);
	BOOL DeleteDirectory(LPCSTR pDirectory);

	UINT ShowAdvertisement(const CServerAdInfo& AdInfo);

	virtual void OnStatusCallback(DWORD dwContext, DWORD dwInternetStatus,
	LPVOID lpvStatusInfomration, DWORD dwStatusInformationLen);

	void UpdateCollectionsOrderFile(const CServerCollectionInfoArray& CollectionArray, LPCSTR pszRevision);

   //the server-defined requests
	int Login(LPCSTR pszCollection = "", LPCSTR pszID = "", CServerDownloadInfo* pDownloadInfo = NULL, CServerAdInfo* pAdInfo = NULL, LPCSTR pszAccountID = NULL, LPCSTR pszPassword = NULL);
	void Logout(void);			// Simply allows a re-login.
	int GetGraphicInfo(const CServerItemInfo& ItemInfo, CServerDownloadInfo* pDownloadInfo);
   int GetGraphic(const CServerDownloadInfo& DownloadInfo, LPCSTR pszDestinationName);
	int EstablishAccount(CServerAccountInfo& AccountInfo, CString& csNewAccountID, LPCSTR csNewPassword);
	BOOL GetAccountInfo(CServerAccountInfo& AccountInfo);
	int GetOrderForm(CServerOrderFormInfo& Info);
	//alias "UpdateAccount"
	BOOL SendAccountInfo(const CServerAccountInfo& AccountInfo, LPCSTR pszNewPassword = NULL);
	int GetAdList(CServerAdInfoArray& AdArray, LPCSTR csSelector = "all", LPCSTR csIgnoreLocale = "f");
	int GetAdInfo(CServerAdInfo& Info, LPCSTR csAdID);
	int MarkAdSeen(LPCSTR csAdID);
	int GetCollectionList(CServerCollectionInfoArray& CollectionArray, LPCSTR csSelector = "all", LPCSTR csIgnoreLocale = "f");
	int GetCollectionInfo(CServerCollectionInfo& Info, LPCSTR csCollID);
	int MarkCollectionSeen(LPCSTR csCollID);
	int GetCDList(CServerCDInfoArray& CDArray, LPCSTR csSelector = "all", LPCSTR csIgnoreLocale = "f");
	int GetCDInfo(CServerCDInfo& CDInfo, LPCSTR csCDID);
	int MarkCDSeen(LPCSTR csCDID);

	int GetCountryInfo(CServerCountryInfoArray& CountryArray);
	int GetShippingInfo(const CServerLocationInfo& LocationInfo, CServerShippingInfoArray& ShippingArray);

	int DownloadFile(const CServerDownloadInfo& DownloadInfo, IServerConnectionCallback* pConnectionCallback, IServerDownloadCallback* pDownloadCallback);

	static BOOL CheckForBadHeader(CCompressInfo* pCompressInfo);
	static BOOL DecompressData(CCompressInfo* pCompressInfo, LPBYTE pInBuffer, LPBYTE pOutBuffer);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContentManager)
	//}}AFX_VIRTUAL

	LPCSTR GetSessionID ( void ) const
		{  return m_csSessionID;}
	void SetSessionID ( LPCSTR pszSessionID )
		{  m_csSessionID = pszSessionID;}
	BOOL HaveSessionID(void) const
		{ return !m_csSessionID.IsEmpty(); }
	
	void SetAccountID ( LPCSTR pszAccountID )
		{  m_csAccountID = pszAccountID;}
	BOOL HaveAccountID(void) const
		{ return !m_csAccountID.IsEmpty(); }

	LPCSTR GetPassword (void) const
		{  return m_csPassword;}
	void SetPassword (LPCSTR pszPassword)
		{  m_csPassword = pszPassword;}

	CString& GetCacheDirectory (void)
	{  return m_csCacheDirectory;}
	void SetCacheDirectory (const CString& NewString)
	{  m_csCacheDirectory = NewString;}

// Implementation
public:

protected:
	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(ContentManager, IContentManager)
		INIT_INTERFACE_PART(CContentManager, ContentManager)
		STDMETHOD_(UINT, GetInternetContent)(THIS_ LPCSTR strColl, LPCSTR strFileName, CString& csBuf);
		STDMETHOD_(BOOL, IsFileLocallyCached)(THIS_ LPCSTR strCollName, LPCSTR strFileName);
		STDMETHOD_(BOOL, UpdateAccount)(THIS);
		STDMETHOD_(BOOL, CheckAccountStatus)(THIS);
		STDMETHOD_(BOOL, CreateNewAccount)(THIS);
		STDMETHOD_(BOOL, RequestExistingAccount)(THIS);
		STDMETHOD_(BOOL, DeleteCachedDirectories)(THIS_ CWnd* pParent, LPCSTR strCollection);
		STDMETHOD_(int, LicenseProducts)(THIS_ LPCSTR pszProduct = NULL, LPCSTR pszInitialSelection = NULL);
		STDMETHOD_(int, PriceInternetOrder)(THIS_ const CServerPriceOrderInfo& OrderInfo, CServerPriceInfo& PriceInfo);
		STDMETHOD_(int, GetCollectionList)(THIS_ CServerCollectionInfoArray& CollectionArray, LPCSTR csSelector = "all", LPCSTR csIgnoreLocale = "f");
		STDMETHOD_(int, DownloadFile)(THIS_ const CServerDownloadInfo& DownloadInfo, IServerConnectionCallback* pConnectionCallback, IServerDownloadCallback* pDownloadCallback);
		STDMETHOD_(int, PurchaseInternetOrder)(THIS_ const CServerPurchaseInfo& PurchaseInfo, CServerPurchaseResult& PurchaseResult);
		STDMETHOD_(int, GetShippingInfo)(THIS_ const CServerLocationInfo& LocationInfo, CServerShippingInfoArray& ShippingMethods);
		STDMETHOD_(int, SendAccountInfo)(THIS_ const CServerAccountInfo& LocationInfo, LPCSTR csNewPassword = NULL);
		STDMETHOD_(BOOL, UpdateOnlineOrderForm)(THIS);
		STDMETHOD_(void, UpdateWhenLastSeenInternetContent)(THIS);
		STDMETHOD_(BOOL, CheckForNewInternetContent)(THIS);
		STDMETHOD_(BOOL, DeleteDirectory)(THIS_ LPCSTR pDirectory);
		STDMETHOD_(LPCSTR, GetAccountID)(THIS);
	END_INTERFACE_PART(ContentManager)
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTENTMANAGER_H__4ECA5A01_BB35_11D1_9909_00A0240C7400__INCLUDED_)
