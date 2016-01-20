//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/Download/CONTSRV.H 1     3/03/99 6:15p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/Download/CONTSRV.H $
// 
// 1     3/03/99 6:15p Gbeddow
// 
// 18    11/25/98 6:00p Psasse
// New parameter to GetOrderForm request
// 
// 17    11/06/98 4:52p Jayn
// 
// 16    11/02/98 5:14p Jayn
// 
// 15    10/27/98 4:47p Jayn
// More download manager and order wizard refinements.
// 
// 14    10/23/98 6:25p Jayn
// New License routines. Changes to content server API.
// 
// 13    10/22/98 4:00p Jayn
// 
// 12    10/19/98 5:41p Jayn
// Beginnings of the online order form.
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
// 8     10/12/98 4:33p Jayn
// Rearranged some includes to make extraction easier.
// 
// 7     10/11/98 8:55p Psasse
// continued conversion to COM interface
// 
// 6     10/09/98 5:54p Jayn
// New "get graphic" implementation.
// 
// 5     10/07/98 5:46p Jayn
// Changes to the content manager.
// 
// 4     10/03/98 9:18p Psasse
// Changed "ParseStringArray" to accept LPCSTR instead of char
// 
// 3     10/01/98 4:09p Jayn
// Added more function bodies.
// 
// 2     9/25/98 7:05p Jayn
// Beginnings of the content server.
// 
// 1     9/24/98 3:32p Jayn
// 
//////////////////////////////////////////////////////////////////////////////

#ifndef __CONTSRV_H__
#define __CONTSRV_H__

class CHttpFile;

#include "icontsrv.h"

/////////////////////////////////////////////////////////////////////////////
// A class to handle callbacks during a DoRequest call.
// Currently, we only have the response handling function.

class CContentServerResponse
{
public:
	virtual DWORD GetResponse(CHttpFile* pHttpFile, CStdioFile* pLogFile = NULL) = 0;
};

/////////////////////////////////////////////////////////////////////////////
// The standard "response-getter" that fills in a map.

class CStandardServerResponse : public CContentServerResponse
{
public:
	virtual DWORD GetResponse(CHttpFile* pHttpFile, CStdioFile* pLogFile = NULL);
	CMapStringToString m_mapResponse;
};

/////////////////////////////////////////////////////////////////////////////
// The download "response-getter" that calls a callback.

class CStdDownloadServerResponse : public CContentServerResponse
{
public:
	CStdDownloadServerResponse(IServerDownloadCallback* pCallback)
		{ m_pCallback = pCallback; }
	virtual DWORD GetResponse(CHttpFile* pHttpFile, CStdioFile* pLogFile = NULL);
protected:
	IServerDownloadCallback* m_pCallback;
};

/////////////////////////////////////////////////////////////////////////////
// CStdContentServer
//  Standard implementation of IContentServer
//

class CStdContentServer : public CCmdTarget
{
public:
	// These functions manage the log file.
	// To turn off logging, set the log file name to "" (default).
	// The file name must be fully qualified.
	void SetLogFileName(LPCSTR pszLogFileName);
	// Get the log file. (The file should be deleted when done.)
	CStdioFile* GetLogFile(void);
	// Interface handlers.
protected:
	CString m_csLogFileName;
protected:
	// To see comments on the functions, see the IContentServer definition
	// in ICONTSRV.H.
	void Login(CServerLoginRequest& Request);
	void Login2(CServerLogin2Request& Request);
	void GetGraphicInfo(CServerGetGraphicInfoRequest& Request);
	void GetGraphicInfo2(CServerGetGraphicInfo2Request& Request);
	void EstablishAccount(CServerEstablishAccountRequest& Request);
	void GetAccountInfo(CServerGetAccountInfoRequest& Request);
	void UpdateAccountInfo(CServerUpdateAccountInfoRequest& Request);
	void GetAdList(CServerGetAdListRequest& Request);
	void GetAdInfo(CServerGetAdInfoRequest& Request);
	void MarkAdSeen(CServerMarkAdSeenRequest& Request);
	void GetCollectionList(CServerGetCollectionListRequest& Request);
	void GetCollectionInfo(CServerGetCollectionInfoRequest& Request);
	void MarkCollectionSeen(CServerMarkCollectionSeenRequest& Request);
	void GetCDList(CServerGetCDListRequest& Request);
	void GetCDInfo(CServerGetCDInfoRequest& Request);
	void MarkCDSeen(CServerMarkCDSeenRequest& Request);
	void GetGraphic(CServerGetGraphicRequest& Request);
	void GetOrderForm(CServerGetOrderFormRequest& Request);
	void GetCountryInfo(CServerGetCountryInfoRequest& Request);
	void GetShippingInfo(CServerGetShippingInfoRequest& Request);
	void PriceOrder(CServerPriceOrderRequest& Request);
	void Purchase(CServerPurchaseRequest& Request);

// Implementation
protected:
	// Add the basic request parameters to the parameter map.
	void AddBasicParameters(const CServerBasicRequest& Request, CMapStringToString& mapRequest);

	// Add location info parameters to the parameter map.
	void AddLocationInfo(const CServerLocationInfo& LocationInfo, CMapStringToString& mapRequest);
	
	// Add account info parameters to the parameter map.
	void AddAccountInfo(const CServerAccountInfo& AccountInfo, CMapStringToString& mapRequest);
	
	// Add purchase item parameters to the parameter map.
	void AddPurchaseItemInfo(const CServerPurchaseItemInfo& ItemInfo, CMapStringToString& mapRequest);

	// Add shipping parameters to the parameter map.
	void AddShippingInfo(const CServerShippingInfo& ItemInfo, CMapStringToString& mapRequest);

	// Add payment parameters to the parameter map.
	void AddPaymentInfo(const CServerPaymentInfo& ItemInfo, CMapStringToString& mapRequest);

	// Build a URL-encode parameter string from a map.
	void BuildParameterString(const CMapStringToString* pmapRequest, CString& csParameterString);

	// URL-encode a string.
	void UrlEncodeString(CString& csString);

	// Construct a list to be added as a parameter.
	void ConstructList(CString& csList, const CStringArray& csaItems, char cSeparator);

	// Do an actual request.
	void DoRequest(CServerConnectionInfo& ConnectionInfo,
						int nVerb,
						LPCSTR pszObject,
						const CMapStringToString* pmapRequest,
						CContentServerResponse* pResponse,
						BOOL fSecure = FALSE);

	// Make sure we completed a request.
	BOOL CheckCompletion(CServerBasicRequest& Request);

	// Get the request status.
	void HandleStatus(const CMapStringToString& mapResponse, CServerBasicRequest& Request);

	// Split a URL of the form "URL:port" or "URL" into URL and port numbers.
	void SplitURL(const CString& csFullURL, CString* pcsURL, int* pPort);

	void GetResponse(CHttpFile* pHttpFile, CMapStringToString* pmapResponse);

// Line parsing routines.
protected:
	// Parse a download info line.
	void ParseDownloadInfo(LPCSTR pszValue, CServerDownloadInfo& Info);
	void ParseDownloadInfo2(LPCSTR pszValue, CServerDownloadInfo& Info);
	// Parse an ad info line.
	void ParseAdInfo(LPCSTR pszValue, CServerAdInfo& Info);
	void ParseAdInfo2(LPCSTR pszValue, CServerAdInfo& Info);
	// Parse a collection info line.
	void ParseCollectionInfo(LPCSTR pszValue, CServerCollectionInfo& Info);
	// Parse a cd info line.
	void ParseCDInfo(LPCSTR pszValue, CServerCDInfo& Info);
	// Parse an order form collection info line.
	void ParseOrderFormCollection(LPCSTR pszValue, CServerOrderFormCollectionInfo& Info);
	// Parse a line into a string array.
	void ParseStringArray(LPCSTR pszValue, CStringArray& csaValues, LPCSTR cSeparator = "\t");
	// Parse a line into a string list.
	void ParseStringList(LPCSTR pszValue, CStringList& cslValues, LPCSTR pszSeparator);
	// Parse a line into attribute-value pairs
	void ParseSubFields(LPCSTR pszLine, CMapStringToString& Map);
	// Parse country information.
	void ParseCountryInfo(LPCSTR pszValue, CServerCountryInfo& Info);
	// Parse shipping information.
	void ParseShippingInfo(LPCSTR pszValue, CServerShippingInfo& Info);

protected:
	DECLARE_INTERFACE_MAP()

	// This defines the interface that is passed to others.
	// It will have name m_XContentServer
	// To see comments on the functions, see the IContentServer definition
	// in ICONTSRV.H.
	BEGIN_INTERFACE_PART(ContentServer, IContentServer)
		INIT_INTERFACE_PART(CStdContentServer, ContentServer)
		STDMETHOD_(void, Login)(THIS_ CServerLoginRequest& Request);
		STDMETHOD_(void, Login2)(THIS_ CServerLogin2Request& Request);
		STDMETHOD_(void, GetGraphicInfo)(THIS_ CServerGetGraphicInfoRequest& Request);
		STDMETHOD_(void, GetGraphicInfo2)(THIS_ CServerGetGraphicInfo2Request& Request);
		STDMETHOD_(void, EstablishAccount)(THIS_ CServerEstablishAccountRequest& Request);
		STDMETHOD_(void, GetAccountInfo)(THIS_ CServerGetAccountInfoRequest& Request);
		STDMETHOD_(void, UpdateAccountInfo)(THIS_ CServerUpdateAccountInfoRequest& Request);
		STDMETHOD_(void, GetAdList)(THIS_ CServerGetAdListRequest& Request);
		STDMETHOD_(void, GetAdInfo)(THIS_ CServerGetAdInfoRequest& Request);
		STDMETHOD_(void, MarkAdSeen)(THIS_ CServerMarkAdSeenRequest& Request);
		STDMETHOD_(void, GetCollectionList)(THIS_ CServerGetCollectionListRequest& Request);
		STDMETHOD_(void, GetCollectionInfo)(THIS_ CServerGetCollectionInfoRequest& Request);
		STDMETHOD_(void, MarkCollectionSeen)(THIS_ CServerMarkCollectionSeenRequest& Request);
		STDMETHOD_(void, GetCDList)(THIS_ CServerGetCDListRequest& Request);
		STDMETHOD_(void, GetCDInfo)(THIS_ CServerGetCDInfoRequest& Request);
		STDMETHOD_(void, MarkCDSeen)(THIS_ CServerMarkCDSeenRequest& Request);
		STDMETHOD_(void, GetGraphic)(THIS_ CServerGetGraphicRequest& Request);
		STDMETHOD_(void, GetOrderForm)(THIS_ CServerGetOrderFormRequest& Request);
		STDMETHOD_(void, GetCountryInfo)(THIS_ CServerGetCountryInfoRequest& Request);
		STDMETHOD_(void, GetShippingInfo)(THIS_ CServerGetShippingInfoRequest& Request);
		STDMETHOD_(void, PriceOrder)(THIS_ CServerPriceOrderRequest& Request);
		STDMETHOD_(void, Purchase)(THIS_ CServerPurchaseRequest& Request);
	END_INTERFACE_PART(ContentServer)
};

#endif
