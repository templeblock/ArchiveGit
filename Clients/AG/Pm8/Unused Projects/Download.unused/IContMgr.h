
#ifndef __ICONTMGR_H__
#define __ICONTMGR_H__

// {BCF2DEC6-49D1-11d2-985D-00A0246D4780}
DEFINE_GUID(IID_IContentManager, 0xbcf2dec6, 0x49d1, 0x11d2, 0x98, 0x5d, 0x0, 0xa0, 0x24, 0x6d, 0x47, 0x80);

//
// The Content Manager interface.
//

#undef INTERFACE
#define INTERFACE   IContentManager

DECLARE_INTERFACE_(IContentManager, IUnknown)
{
	// *** IUnknown methods ***
	STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID FAR * lplpObj) PURE;
	STDMETHOD_(ULONG, AddRef) (THIS)  PURE;
	STDMETHOD_(ULONG, Release) (THIS) PURE;

	// *** IContentManager methods ***

	//
	// Perform the initial login and download of a graphic.
	//
	STDMETHOD_(UINT, GetInternetContent)(THIS_ LPCSTR strCollName, LPCSTR strFileName, CString& csBuf) PURE;
	
	//
	// Check to see if the internet item has been downloaded and locally cached.
	//
	STDMETHOD_(BOOL, IsFileLocallyCached)(THIS_ LPCSTR strCollName, LPCSTR strFileName) PURE;
	
	//
	// A Request to update the account on the data server.
	//
	STDMETHOD_(BOOL, UpdateAccount)(THIS) PURE;

	//
	// Check to see if an account exists and create one if not
	//
	STDMETHOD_(BOOL, CheckAccountStatus)(THIS) PURE;

	//
	// Create a new account on the data server
	//
	STDMETHOD_(BOOL, CreateNewAccount)(THIS) PURE;

	//
	// Change the internet account to a pre-existing one.
	//
	STDMETHOD_(BOOL, RequestExistingAccount)(THIS) PURE;

	//
	// Delete all locally cached internet files.
	//
	STDMETHOD_(BOOL, DeleteCachedDirectories)(THIS_ CWnd* pParent, LPCSTR pszCollection) PURE;

	//
	// Get the information for internet order form.
	//
	STDMETHOD_(int, LicenseProducts)(THIS_ LPCSTR pszProduct = NULL,  LPCSTR pszInitialSelection = NULL) PURE;

	//
	// Price the internet order.
	//
	STDMETHOD_(int, PriceInternetOrder)(THIS_ const CServerPriceOrderInfo& OrderInfo, CServerPriceInfo& PriceInfo) PURE;

	//
	// Get a list of collections.
	//
	STDMETHOD_(int, GetCollectionList)(THIS_ CServerCollectionInfoArray& CollectionArray, LPCSTR csSelector = "all", LPCSTR csIgnoreLocale = "f") PURE;

	//
	// Download a file.
	//
	STDMETHOD_(int, DownloadFile)(THIS_ const CServerDownloadInfo& DownloadInfo, IServerConnectionCallback* pConnectionCallback, IServerDownloadCallback* pDownloadCallback) PURE;

	//
	// Purchase the internet order.
	//
	STDMETHOD_(int, PurchaseInternetOrder)(THIS_ const CServerPurchaseInfo& PurchaseInfo, CServerPurchaseResult& PurchaseResult) PURE;

	//
	// Get the shipping methods for a particular location.
	//
	STDMETHOD_(int, GetShippingInfo)(THIS_ const CServerLocationInfo& LocationInfo, CServerShippingInfoArray& ShippingMethods) PURE;

	//
	// Send user account info.
	//
	STDMETHOD_(int, SendAccountInfo)(THIS_ const CServerAccountInfo& AccountInfo, LPCSTR csNewPassword = NULL) PURE;

	//
	// Update the online order form.
	//
	STDMETHOD_(BOOL, UpdateOnlineOrderForm)(THIS) PURE;
	
	//
	// Update when user last saw new internet content.
	//
	STDMETHOD_(void, UpdateWhenLastSeenInternetContent)(THIS) PURE;

	//
	// Check for new internet content.
	//
	STDMETHOD_(BOOL, CheckForNewInternetContent)(THIS) PURE;

	//
	// Delete any Directory
	//
	STDMETHOD_(BOOL, DeleteDirectory)(THIS_ LPCSTR pszDirectory) PURE;

	//
	// GetAccountID
	//
	STDMETHOD_(LPCSTR, GetAccountID)(THIS) PURE;

};

#endif
