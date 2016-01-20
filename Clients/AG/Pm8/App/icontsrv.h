/////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/Download/icontsrv.h 1     3/03/99 6:15p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/Download/icontsrv.h $
// 
// 1     3/03/99 6:15p Gbeddow
// 
// 20    11/25/98 6:01p Psasse
// CServerOrderFormInfo gets a new parameter - CDLocation
// 
// 19    10/30/98 5:25p Jayn
// 
// 18    10/28/98 1:48p Jayn
// More polish for ordering and downloading.
// 
// 17    10/27/98 4:47p Jayn
// More download manager and order wizard refinements.
// 
// 16    10/26/98 9:08p Psasse
// response strings to CServerPurchaseInfo
// 
// 15    10/23/98 6:25p Jayn
// New License routines. Changes to content server API.
// 
// 14    10/22/98 4:00p Jayn
// 
// 13    10/19/98 5:41p Jayn
// Beginnings of the online order form.
// 
// 12    10/19/98 11:05a Psasse
// API changes
// 
// 11    10/15/98 5:40p Psasse
// Implemented Purchase request
// 
// 10    10/15/98 4:51p Psasse
// Added Login2,GetGraphicInfo2,PriceOrder requests
// 
// 9     10/14/98 9:16p Psasse
// Added the GetOrderForm request
// 
// 8     10/11/98 8:55p Psasse
// continued conversion to COM interface
// 
// 7     10/09/98 5:54p Jayn
// New "get graphic" implementation.
// 
// 6     10/07/98 5:46p Jayn
// Changes to the content manager.
// 
// 5     10/03/98 9:18p Psasse
// m_csIgnoreLocale added to GetCollectionList
// 
// 4     10/02/98 7:10p Jayn
// More content manager changes
// 
// 3     10/01/98 4:09p Jayn
// Added more function bodies.
// 
// 2     9/25/98 7:05p Jayn
// Beginnings of the content server.
// 
// 1     9/24/98 3:32p Jayn
// 
// 2     5/31/98 5:19p Hforman
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __ICONTSRV_H__
#define __ICONTSRV_H__

//// {BCF2DEC4-49D1-11d2-985D-00A0246D4780}
DEFINE_GUID(IID_IContentServer, 0xbcf2dec4, 0x49d1, 0x11d2, 0x98, 0x5d, 0x0, 0xa0, 0x24, 0x6d, 0x47, 0x80);

/////////////////////////////////////////////////////////////////////////////
// Status codes the content server can return.
// These are defined in the "PGS" interface doc. 

enum
{
	CSSTATUS_Success								= 0,
	CSSTATUS_IllFormedRequest					= 1,
	CSSTATUS_ServiceError						= 2,
	CSSTATUS_ProtocolVersionNotSupported	= 3,
	CSSTATUS_ClientVersionNotSupported		= 4,
	CSSTATUS_InvalidLoginOrPassword			= 5,
	CSSTATUS_UseAlternateServer				= 6,
	CSSTATUS_UnknownAdID							= 7,
	CSSTATUS_NoAdsMatchingCriteria			= 8,
	CSSTATUS_UnknownCollectionID				= 9,
	CSSTATUS_NoCollectionsMatchingCriteria	= 10,
	CSSTATUS_UnknownCDID							= 11,
	CSSTATUS_NoCDsMatchingCriteria			= 12,
	CSSTATUS_UnknownItemID						= 13,
	CSSTATUS_ItemNotLicensed					= 14,
	CSSTATUS_LoginInUse							= 15,

	CSSTATUS_PurchaseInProgress				= 17,

	CSSTATUS_UnknownShippingMethod			= 20,
	CSSTATUS_AddressIncorrect					= 21,
	CSSTATUS_InvalidStateCode					= 22,
	CSSTATUS_InvalidCountryCode				= 23,
	CSSTATUS_TooManyItems						= 24,

	CSSTATUS_FieldTooShort						= 30,
	CSSTATUS_FieldTooLong						= 31,
	CSSTATUS_FieldTooSmall						= 32,
	CSSTATUS_FieldTooBig							= 33,
	CSSTATUS_FieldContainsInvalid				= 34,

	CSSTATUS_InvalidCreditCardNumber			= 40,
	CSSTATUS_InvalidExpirationDateFormat	= 41,
	CSSTATUS_InvalidExpirationDate			= 42,
	CSSTATUS_InvalidCreditCardType			= 43,
	CSSTATUS_PaymentServerNoResponse			= 44,
	CSSTATUS_CreditCardRejected				= 45,
	CSSTATUS_PaymentServerError				= 46,

	// Additional results we can generate internally.
	CSSTATUS_UserAbort							= 4000,
	CSSTATUS_CommunicationException			= 4001,
	CSSTATUS_ServerException					= 4002,
	CSSTATUS_InternalError						= 4003,
	CSSTATUS_MemoryError							= 4004,
	CSSTATUS_WriteError							= 4005,
};

/////////////////////////////////////////////////////////////////////////////
// Callback classes.
// These are not COM interfaces currently, but they could be.

struct IServerConnectionCallback
{
	// The status has changed.
	virtual DWORD OnStatusChange(DWORD dwInternetStatus) = 0;
};

struct IServerDownloadCallback
{
	// Data is available.
	virtual DWORD OnReceiveData(LPVOID pData, DWORD dwDataSize, DWORD dwDataTotal) = 0;
};

/////////////////////////////////////////////////////////////////////////////
// Component info structures.
// These are used by the requests (which follow).

//
// Server connection info.
//

struct CServerConnectionInfo
{
	// Connection variables (request)
	CStringList* m_pServerURLs;			// strings of the form
													// "URL" or "URL:port"
													// (e.g. "pmgsdata.printmaster.com:31597")
													// The server is free to rearrange
													// or otherwise modify this list.
	CString m_csBasePath;					// The path we append our stuff to
													// (e.g. "scripts/MindscapePGS")
	CString m_csAuthenticationLogin;		// The login name for authentication
	CString m_csAuthenticationPassword;	// The password for authentication

	// Communication status
	DWORD m_dwResult;							// Communication result
	// Server status (response)
	DWORD m_dwServerStatusCode;			// HTTP code from server (500, etc).

	// Callback member.
	IServerConnectionCallback* m_pConnectionCallback;

	CServerConnectionInfo() : m_pConnectionCallback(NULL) {}
};

//
// A server account struct.
// This does not include the accound ID or password of the basic request.
//

struct CServerAccountInfo
{
	CString m_csCustLastName;
	CString m_csCustFirstName;
	CString m_csStreet1;
	CString m_csStreet2;
	CString m_csCity;
	CString m_csState;
	CString m_csCountry;
	CString m_csZip;
	CString m_csEMailAddr;
	CString m_csPhone;
};

//
// A server item struct.
// This contains the item collection and id.
//

struct CServerItemInfo
{
	CString m_csItemCollection;
	CString m_csItemID;
};

//
// A server download struct
// This contains the filename, plus the login name and password.
// The filename is combined with a download server to form a complete URL.
//

struct CServerDownloadInfo
{
	CString m_csFile;						// URL (filename) of the item
	CString m_csLoginID;					// Login name for downloading item
	CString m_csPassword;				// Password for downloading item
};

//
// Ad information.
//

struct CServerAdInfo
{
	CString m_csAdID;
	CString m_csFileName;
	CString m_csLocale;
	CString m_csDescription;
	CString m_csURLLink;
};

//
// An ad info array.
//

typedef CArray<CServerAdInfo, CServerAdInfo&> CServerAdInfoArray;

//
// Collection information.
//

struct CServerCollectionInfo
{
	CString m_csCollectionID;
	CString m_csName;
	CString m_csFileName;
	CString m_csLocale;
	CString m_csDescription;
	CString m_csPrice;
	CString m_csProductCode;
	CString m_csSKU;
	CString m_csIconFileName;
	CString m_csBrochureFileName;
	CString m_csCDLocation;
};

//
// A collection info array.
//

typedef CArray<CServerCollectionInfo, CServerCollectionInfo&> CServerCollectionInfoArray;

//
// CD information.
//

struct CServerCDInfo
{
	CString m_csCDID;
	CString m_csName;
	CString m_csLocale;
	CString m_csDescription;
	CString m_csAvailabilityDate;
	CString m_csPrice;
};

//
// A collection info array.
//

typedef CArray<CServerCDInfo, CServerCDInfo&> CServerCDInfoArray;


//
// OrderFormCollection information.
//

struct CServerOrderFormCollectionInfo
{
	CString m_csCDID;
	CString m_csCDName;
	CString m_csCDDescription;
	CString m_csCDPrice;
	CString m_csCDSKU;
	CString m_csCDPurchased;
	CString m_csCollectionID;
	CString m_csCollectionName;
	CString m_csCollectionDescription;
	CString m_csCollectionPrice;
	CString m_csCollectionSKU;
	CString m_csCollectionPurchased;
	CString m_csCDLocation;
};

//
// A collection info array.
//

typedef CArray<CServerOrderFormCollectionInfo, CServerOrderFormCollectionInfo&> CServerOrderFormCollectionInfoArray;

//
// Order form information.
//

struct CServerOrderFormInfo
{
	CString	m_csOrderHeader;				// the information header to display

	CServerOrderFormCollectionInfoArray m_Collections;		// the returned Collections

	CStringArray m_csaCardTypes;			// Credit cards accepted
};

//
// Purchase item information.
struct CServerPurchaseItemInfo
{
	CStringArray m_csaCDs;
	CStringArray m_csaCollections;
	CServerPurchaseItemInfo& operator=(const CServerPurchaseItemInfo& Info)
		{ m_csaCDs.Copy(Info.m_csaCDs); m_csaCollections.Copy(Info.m_csaCollections); return *this; }
};

//
// Price information returned by the server.
//

struct CServerPriceInfo
{
	CString m_csCollectionTotal;
	CString m_csCollectionCount;
	CString m_csCDTotal;
	CString m_csCDCount;
	CString m_csItemTotal;
	CString m_csDiscount;
	CString m_csSalesTax;
	CString m_csShipping;
	CString m_csOrderTotal;
};

//
// Server payment information.
// Any request using this info should be sent via a secure means.
//

struct CServerPaymentInfo
{
	CString m_csCardType;
	CString m_csCardNumber;
	CString m_csCardExpiration;
};

//
// Server shipping information.
// This is the info returned by the server.
//

struct CServerShippingInfo
{
	CString m_csName;				// The name of this shipping method
	CString m_csDescription;	// The description of this shipping method
	CString m_csPrice;			// The price for this shipping method
	CString m_csShipID;			// The server code for this shipping method
};

//
// A shipping info array.
//

typedef CArray<CServerShippingInfo, CServerShippingInfo&> CServerShippingInfoArray;

//
// A small location struct for computing shipping.
//

struct CServerLocationInfo
{
	CString m_csState;
	CString m_csCountry;
	CString m_csZip;
};

//
// Info used to price an order.
//

struct CServerPriceOrderInfo
{
	// Location info.
	CServerLocationInfo m_LocationInfo;

	// Item info.
	CServerPurchaseItemInfo m_ItemInfo;

	// Shipping info.
	CString m_csShipID;
};

//
// Purchase information.
//

struct CServerPurchaseInfo
{
	// Account info.
	CServerAccountInfo m_AccountInfo;

	// Item info.
	CServerPurchaseItemInfo m_ItemInfo;

	// Shipping info.
	CString m_csShipID;

	// Payment info.
	CServerPaymentInfo m_PaymentInfo;
};

//
// Purchase result.
//

struct CServerPurchaseResult
{
	CString m_csOrderNumber;
	CString m_csResultText;
};

//
// Country information.
//

struct CServerCountryInfo
{
	CString m_csName;			// The name of the country
	CString m_csCode;			// The two-letter code for the country
};

//
// A country info array.
//

typedef CArray<CServerCountryInfo, CServerCountryInfo&> CServerCountryInfoArray;

/////////////////////////////////////////////////////////////////////////////
// The requests.

//
// The basic request.
// These parameters apply to all requests.
//

struct CServerBasicRequest
{
	// Connection info
	CServerConnectionInfo m_ConnectionInfo;

	// Request variables
	CString m_csOperation;					// Unused?
	CString m_csAccountID;
	CString m_csPassword;
	CString m_csSessionID;
	CString m_csRequestID;
	CString m_csClientSKU;
	CString m_csClientVersion;
	CString m_csClientPGSVersion;
	CString m_csClientLocale;

	// Response variables
	CString m_csStatus;						// Status response from server.
	int m_nStatus;								// Numeric version of above.
};

//
// A "login" request.
//

struct CServerLoginRequest : public CServerBasicRequest
{
	// Request variables.
	CServerItemInfo m_ItemInfo;			// (optional)

	// Response variables.
	CString m_csNewSessionID;				// The session ID to use

	CServerDownloadInfo m_DownloadInfo;	// Download info for above item

	CStringList m_cslDataServers;			// Data servers to use
	CStringList m_cslDownloadServers;	// Download servers to use

	CServerAdInfo m_AdInfo;					// Ad info
	CServerCollectionInfo m_CollectionInfo; // Collection info

	CStringArray m_csaShipMethods;		// Shipping methods available
	CStringArray m_csaCardTypes;			// Credit cards accepted
};

//
// A "login2" request.
//

struct CServerLogin2Request : public CServerBasicRequest
{
	// Request variables.
	CServerItemInfo m_ItemInfo;			// (optional)

	// Response variables.
	CString m_csNewSessionID;				// The session ID to use

	CServerDownloadInfo m_DownloadInfo;	// Download info for above item

	CStringList m_cslDataServers;			// Data servers to use
	CStringList m_cslDownloadServers;	// Download servers to use

	CServerAdInfo m_AdInfo;					// Ad info
	CServerCollectionInfo m_CollectionInfo; // Collection info

	CString m_csRevisionID;					// The revision ID
	CString m_csGlobalRevisionID;			// The global revision ID

};

//
// A "get graphic info" request.
//

struct CServerGetGraphicInfoRequest : public CServerBasicRequest
{
	// Request variables.
	CServerItemInfo m_ItemInfo;			// The item to get info for

	// Response variables.
	CServerDownloadInfo m_DownloadInfo;	// Download info for above item
};

//
// A "get graphic info2" request.
//

struct CServerGetGraphicInfo2Request : public CServerBasicRequest
{
	// Request variables.
	CServerItemInfo m_ItemInfo;			// The item to get info for

	// Response variables.
	CServerDownloadInfo m_DownloadInfo;	// Download info for above item
};

//
// An "establish account" request.
//

struct CServerEstablishAccountRequest : public CServerBasicRequest
{
	// Request variables.
	CServerAccountInfo m_AccountInfo;	// The account information

	// Response variables.
	CString m_csSuggestedAccountID;		// If already in use
};

//
// A "get account info" request.
//

struct CServerGetAccountInfoRequest : public CServerBasicRequest
{
	// Request variables.
	// None additional

	// Response variables.
	CServerAccountInfo m_AccountInfo;	// The account information
};

//
// An "update account info" request.
//

struct CServerUpdateAccountInfoRequest : public CServerBasicRequest
{
	// Request variables.
	CString m_csNewPassword;				// The new password
	CServerAccountInfo m_AccountInfo;	// The new account information

	// Response variables.
	// None additional
};

//
// A "get ad list" request.
//

struct CServerGetAdListRequest : public CServerBasicRequest
{
	// Request variables.
	CString m_csListType;					// "all", "unseen" or "seen"
	CString m_csIgnoreLocale;				// do we ignore the locale?

	// Response variables.
	CServerAdInfoArray m_Ads;				// the ads
};

//
// A "get ad info" request.
//

struct CServerGetAdInfoRequest : public CServerBasicRequest
{
	// Request variables.
	CString m_csAdID;					// which ad to get info for

	// Response variables.
	CServerAdInfo m_AdInfo;			// the info for that ad
};

//
// A "mark ad seen" request.
//

struct CServerMarkAdSeenRequest : public CServerBasicRequest
{
	// Request variables.
	CString m_csAdID;					// which ad to get info for

	// Response variables.
	// None additional
};

//
// A "get collection list" request.
//

struct CServerGetCollectionListRequest : public CServerBasicRequest
{
	// Request variables.
	CString m_csListType;			// "all", "seen", "unseen", "purchased" or "unpurchased"
	CString m_csIgnoreLocale;				// do we ignore the locale?

	// Response variables.
	CServerCollectionInfoArray m_Collections;		// the returned collections
};

//
// A "get collection info" request.
//

struct CServerGetCollectionInfoRequest : public CServerBasicRequest
{
	// Request variables.
	CString m_csCollectionID;						// which collection we want

	// Response variables.
	CServerCollectionInfo m_CollectionInfo;	// the returned info
};

//
// A "mark collection seen" request.
//

struct CServerMarkCollectionSeenRequest : public CServerBasicRequest
{
	// Request variables.
	CString m_csCollectionID;					// which collection to mark

	// Response variables.
	// None additional
};

//
// A "get CD list" request.
//

struct CServerGetCDListRequest : public CServerBasicRequest
{
	// Request variables.
	CString m_csListType;					// "all", "seen", "unseen", "purchased" or "unpurchased"
	CString m_csIgnoreLocale;				// do we ignore the locale?

	// Response variables.
	CServerCDInfoArray m_CDs;		// the returned CDs
};

//
// A "get CD info" request.
//

struct CServerGetCDInfoRequest : public CServerBasicRequest
{
	// Request variables.
	CString m_csCDID;						// which CD we want

	// Response variables.
	CServerCDInfo m_CDInfo;	// the returned info
};

//
// A "mark collection seen" request.
//

struct CServerMarkCDSeenRequest : public CServerBasicRequest
{
	// Request variables.
	CString m_csCDID;					// which collection to mark

	// Response variables.
	// None additional
};

//
// A "get graphic" request.
//

struct CServerGetGraphicRequest : public CServerBasicRequest
{
	// Request variables.
	CServerDownloadInfo m_DownloadInfo;	// Download info for above item

	// Response variables.

	// Callback member.
	IServerDownloadCallback* m_pDownloadCallback;

	// Constructor.
	CServerGetGraphicRequest() : m_pDownloadCallback(NULL) {}
};


//
// A "get order form" request.
//

struct CServerGetOrderFormRequest : public CServerBasicRequest
{
	// Request variables.
	// None additional

	// Response variables.
	CServerOrderFormInfo* m_pInfo;		// This is a pointer for convenience.
};

//
// A "price order" request.
//

struct CServerPriceOrderRequest : public CServerBasicRequest
{
	// Request variables.
	CServerPriceOrderInfo m_OrderInfo;

	// Response variables.
	CServerPriceInfo m_PriceInfo;
};

//
// A "purchase" request.
//

struct CServerPurchaseRequest : public CServerBasicRequest
{
	// Request variables.
	CServerPurchaseInfo m_PurchaseInfo;

	// Response variables.
	CServerPurchaseResult m_PurchaseResult;
};

//
// A "get country info" request.
//

struct CServerGetCountryInfoRequest : public CServerBasicRequest
{
	// Request variables.
	// None additional

	// Response variables.
	CServerCountryInfoArray m_Countries;	// The country information
};

//
// A "get shipping info" request.
//

struct CServerGetShippingInfoRequest : public CServerBasicRequest
{
	// Request variables.
	// Location info.
	CServerLocationInfo m_LocationInfo;

	// Response variables.
	CServerShippingInfoArray m_ShippingMethods;
};

/*
// The content server interface.
*/

#undef INTERFACE
#define INTERFACE   IContentServer

DECLARE_INTERFACE_(IContentServer, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * lplpObj) PURE;
	STDMETHOD_(ULONG, AddRef) (THIS) PURE;
	STDMETHOD_(ULONG, Release) (THIS) PURE;

	// *** IContentServer methods ***

	//
	// Login to the server.
	//

	STDMETHOD_(void, Login)(THIS_ CServerLoginRequest& Request) PURE;

	//
	// Login2 to the server.
	//

	STDMETHOD_(void, Login2)(THIS_ CServerLogin2Request& Request) PURE;

	//
	// Get graphic info.
	//

	STDMETHOD_(void, GetGraphicInfo)(THIS_ CServerGetGraphicInfoRequest& Request) PURE;

	//
	// Get graphic info2.
	//

	STDMETHOD_(void, GetGraphicInfo2)(THIS_ CServerGetGraphicInfo2Request& Request) PURE;

	//
	// Establish an account.
	//

	STDMETHOD_(void, EstablishAccount)(THIS_ CServerEstablishAccountRequest& Request) PURE;

	//
	// Get account info.
	//

	STDMETHOD_(void, GetAccountInfo)(THIS_ CServerGetAccountInfoRequest& Request) PURE;

	//
	// Update account info.
	//

	STDMETHOD_(void, UpdateAccountInfo)(THIS_ CServerUpdateAccountInfoRequest& Request) PURE;

	//
	// Get info for a list of ads.
	//

	STDMETHOD_(void, GetAdList)(THIS_ CServerGetAdListRequest& Request) PURE;

	//
	// Get info for a particular ad.
	//

	STDMETHOD_(void, GetAdInfo)(THIS_ CServerGetAdInfoRequest& Request) PURE;

	//
	// Mark an ad as seen.
	//

	STDMETHOD_(void, MarkAdSeen)(THIS_ CServerMarkAdSeenRequest& Request) PURE;

	//
	// Get a list of collections.
	//

	STDMETHOD_(void, GetCollectionList)(THIS_ CServerGetCollectionListRequest& Request) PURE;

	//
	// Get collection info.
	//

	STDMETHOD_(void, GetCollectionInfo)(THIS_ CServerGetCollectionInfoRequest& Request) PURE;

	//
	// Mark collection seen.
	//

	STDMETHOD_(void, MarkCollectionSeen)(THIS_ CServerMarkCollectionSeenRequest& Request) PURE;

	//
	// Get a list of CDs.
	//

	STDMETHOD_(void, GetCDList)(THIS_ CServerGetCDListRequest& Request) PURE;

	//
	// Get CD info.
	//

	STDMETHOD_(void, GetCDInfo)(THIS_ CServerGetCDInfoRequest& Request) PURE;

	//
	// Mark CD seen.
	//

	STDMETHOD_(void, MarkCDSeen)(THIS_ CServerMarkCDSeenRequest& Request) PURE;

	//
	// Get graphic (content).
	//

	STDMETHOD_(void, GetGraphic)(THIS_ CServerGetGraphicRequest& Request) PURE;

	//
	// Get Order Form information.
	//

	STDMETHOD_(void, GetOrderForm)(THIS_ CServerGetOrderFormRequest& Request) PURE;

	//
	// Get the country information.
	//
	
	STDMETHOD_(void, GetCountryInfo)(THIS_ CServerGetCountryInfoRequest& Request) PURE;

	//
	// Get the shipping information.
	//
	
	STDMETHOD_(void, GetShippingInfo)(THIS_ CServerGetShippingInfoRequest& Request) PURE;

	//
	// Price Order information.
	//

	STDMETHOD_(void, PriceOrder)(THIS_ CServerPriceOrderRequest& Request) PURE;

	//
	// Purchase information.
	//

	STDMETHOD_(void, Purchase)(THIS_ CServerPurchaseRequest& Request) PURE;
};

#endif
