//////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/Download/CONTSRV.CPP 1     3/03/99 6:15p Gbeddow $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/Download/CONTSRV.CPP $
// 
// 1     3/03/99 6:15p Gbeddow
// 
// 34    11/25/98 6:00p Psasse
// New parameter to GetOrderForm request
// 
// 33    11/17/98 9:55a Johno
// Added #include "pmwinet.h" for release build
// I'm not sure why this is needed...
// 
// 32    11/11/98 5:39p Johno
// Added debug message box
// 
// 31    11/06/98 4:52p Jayn
// 
// 30    11/04/98 4:56p Jayn
// 
// 29    11/02/98 5:14p Jayn
// 
// 28    10/28/98 1:48p Jayn
// More polish for ordering and downloading.
// 
// 27    10/27/98 7:08p Jayn
// 
// 26    10/27/98 4:47p Jayn
// More download manager and order wizard refinements.
// 
// 25    10/26/98 9:08p Psasse
// attempt making purchase request secure and read response variables
// 
// 24    10/26/98 6:47p Jayn
// Download manager.
// 
// 23    10/24/98 9:10p Psasse
// Changed mapresponse variable from zip to zipcode
// 
// 22    10/24/98 7:34p Jayn
// Download Manager and Download Dialog
// 
// 21    10/23/98 6:25p Jayn
// New License routines. Changes to content server API.
// 
// 20    10/22/98 4:00p Jayn
// 
// 19    10/21/98 5:30p Jayn
// Added a read buffer to the CHttpFile.
// 
// 18    10/21/98 4:14p Jayn
// More improvements to the order process.
// 
// 17    10/20/98 9:04p Psasse
// Worked on "ConfirmOrder" dialog of online wizard
// 
// 16    10/20/98 5:32p Jayn
// Improving the online order form.
// 
// 15    10/19/98 5:41p Jayn
// Beginnings of the online order form.
// 
// 14    10/19/98 11:04a Psasse
// API changes
// 
// 13    10/15/98 5:40p Psasse
// Implemented Purchase request
// 
// 12    10/15/98 4:51p Psasse
// Added Login2,GetGraphicInfo2,PriceOrder requests
// 
// 11    10/14/98 9:16p Psasse
// Added the GetOrderForm request
// 
// 10    10/12/98 4:33p Jayn
// Rearranged some includes to make extraction easier.
// 
// 9     10/11/98 8:55p Psasse
// continued conversion to COM interface
// 
// 8     10/09/98 5:54p Jayn
// New "get graphic" implementation.
// 
// 7     10/07/98 8:50p Psasse
// Fixed the header problem associated with a post request
// 
// 6     10/07/98 5:46p Jayn
// Changes to the content manager.
// 
// 5     10/03/98 9:17p Psasse
// Added functionality for Ads and Collections. Changed
// FillArrayFromDelimited String
// 
// 4     10/01/98 4:09p Jayn
// Added more function bodies.
// 
// 3     9/30/98 9:27p Psasse
// conversion in progress
// 
// 2     9/25/98 7:05p Jayn
// Beginnings of the content server.
// 
// 1     9/24/98 3:32p Jayn
// 
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "contsrv.h"
#include "util.h"
#include "pmwinet.h"
/////////////////////////////////////////////////////////////////////////////
// CStdInternetSession
// Our override of CInternetSession that allows callback notification.

class CStdInternetSession : public CInternetSession
{
public:
	CStdInternetSession(IServerConnectionCallback* pCallback = NULL)
	{
		if ((m_pCallback = pCallback) != NULL)
		{
			EnableStatusCallback(TRUE);
		}
	}
protected:
	// Overridden callback function.
	virtual void OnStatusCallback(DWORD dwContext,
											DWORD dwInternetStatus,
											LPVOID lpvStatusInfomration,
											DWORD dwStatusInformationLen)
	{
		ASSERT(m_pCallback != NULL);
		if (m_pCallback != NULL)
			m_pCallback->OnStatusChange(dwInternetStatus);
	}
protected:
	IServerConnectionCallback* m_pCallback;
};

/////////////////////////////////////////////////////////////////////////////
// CStdContentServer

void CStdContentServer::SetLogFileName(LPCSTR pszLogFilename)
{
	m_csLogFileName = pszLogFilename;
	CStdioFile* pFile = GetLogFile();
	if (pFile != NULL)
	{
		// Write out the initial header.
		pFile->WriteString("New session\n");
		delete pFile;
	}
}

CStdioFile* CStdContentServer::GetLogFile(void)
{
	CStdioFile* pFile = NULL;
	if (!m_csLogFileName.IsEmpty())
	{
		pFile = new CStdioFile;
		if (pFile->Open(m_csLogFileName,
							 CFile::modeCreate
							 | CFile::modeNoTruncate
								| CFile::modeWrite
								| CFile::shareDenyWrite
								| CFile::typeText))
		{
			// Seek to the end of the file (append).
			pFile->SeekToEnd();

			// Write the current time.
			SYSTEMTIME SystemTime;
			GetLocalTime(&SystemTime);
			CString csTime;
			csTime.Format("*** At %02d:%02d:%02d on %02d/%02d/%02d ***\n",
							  SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond,
							  SystemTime.wMonth, SystemTime.wDay, SystemTime.wYear % 100);
			pFile->WriteString(csTime);
		}
	}
	return pFile;
}

//
// Login to the server.
//

void CStdContentServer::Login(CServerLoginRequest& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Add login-specific parameters.
	if (!Request.m_ItemInfo.m_csItemCollection.IsEmpty()
				&& !Request.m_ItemInfo.m_csItemID.IsEmpty())
	{
		// We have a requested item. Add it in.
		mapRequest.SetAt("ItemColl", Request.m_ItemInfo.m_csItemCollection);
		mapRequest.SetAt("ItemName", Request.m_ItemInfo.m_csItemID);
	}

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_GET,
				 "Login",
				 &mapRequest,
				 &Response);

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		CString csValue;

		// Handle the status.
		HandleStatus(mapResponse, Request);

		// Get the new session ID.
		mapResponse.Lookup("sessionid", Request.m_csNewSessionID);
		// Get the item download info.
		if (mapResponse.Lookup("item", csValue))
		{
			ParseDownloadInfo(csValue, Request.m_DownloadInfo);
		}
		// Get the ad info.
		if (mapResponse.Lookup("adinfo", csValue))
		{
			ParseAdInfo(csValue, Request.m_AdInfo);
		}
		// Get the collection info.
		if (mapResponse.Lookup("collinfo", csValue))
		{
			ParseCollectionInfo(csValue, Request.m_CollectionInfo);
		}
		// Get the data servers.
		if (mapResponse.Lookup("dataservers", csValue))
		{
			ParseStringList(csValue, Request.m_cslDataServers, "\t");
		}
		// Get the download servers.
		if (mapResponse.Lookup("graphicservers", csValue))
		{
			ParseStringList(csValue, Request.m_cslDownloadServers, "\t");
		}
		// Get the shipping methods
		if (mapResponse.Lookup("shipmethods", csValue))
		{
			ParseStringArray(csValue, Request.m_csaShipMethods, "\t");
		}
		// Get the credit card types
		if (mapResponse.Lookup("cardtypes", csValue))
		{
			ParseStringArray(csValue, Request.m_csaCardTypes, "\t");
		}
	}
}

//
// Login2 to the server.
//

void CStdContentServer::Login2(CServerLogin2Request& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Add login-specific parameters.
	if (!Request.m_ItemInfo.m_csItemCollection.IsEmpty()
				&& !Request.m_ItemInfo.m_csItemID.IsEmpty())
	{
		// We have a requested item. Add it in.
		mapRequest.SetAt("ItemColl", Request.m_ItemInfo.m_csItemCollection);
		mapRequest.SetAt("ItemName", Request.m_ItemInfo.m_csItemID);
	}

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_GET,
				 "Login2",
				 &mapRequest,
				 &Response);

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		CString csValue;

		// Handle the status.
		HandleStatus(mapResponse, Request);

		// Get the new session ID.
		mapResponse.Lookup("sessionid", Request.m_csNewSessionID);
		// Get the item download info.
		if (mapResponse.Lookup("item", csValue))
		{
			ParseDownloadInfo2(csValue, Request.m_DownloadInfo);
		}
		// Get the ad info.
		if (mapResponse.Lookup("adinfo", csValue))
		{
			ParseAdInfo2(csValue, Request.m_AdInfo);
		}
		// Get the collection info.
		if (mapResponse.Lookup("collinfo", csValue))
		{
			ParseCollectionInfo(csValue, Request.m_CollectionInfo);
		}
		// Get the data servers.
		if (mapResponse.Lookup("dataservers", csValue))
		{
			ParseStringList(csValue, Request.m_cslDataServers, "\t");
		}
		// Get the download servers.
		if (mapResponse.Lookup("graphicservers", csValue))
		{
			ParseStringList(csValue, Request.m_cslDownloadServers, "\t");
		}
		// Get the new revision ID.
		mapResponse.Lookup("revisionid", Request.m_csRevisionID);
		// Get the new global revision ID.
		mapResponse.Lookup("globalrevisionid", Request.m_csGlobalRevisionID);
	}
}

//
// Get graphic info.
//

void CStdContentServer::GetGraphicInfo(CServerGetGraphicInfoRequest& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Add GetGraphicInfo-specific parameters.
	mapRequest.SetAt("ItemColl", Request.m_ItemInfo.m_csItemCollection);
	mapRequest.SetAt("ItemName", Request.m_ItemInfo.m_csItemID);

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_POST,
				 "GetGraphicInfo",
				 &mapRequest,
				 &Response);

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		CString csValue;

		// Handle the status.
		HandleStatus(mapResponse, Request);

		// Get the item download info.
		if (mapResponse.Lookup("item", csValue))
		{
			ParseDownloadInfo(csValue, Request.m_DownloadInfo);
		}
	}
}

//
// Get graphic info2.
//

void CStdContentServer::GetGraphicInfo2(CServerGetGraphicInfo2Request& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Add GetGraphicInfo2-specific parameters.
	mapRequest.SetAt("ItemColl", Request.m_ItemInfo.m_csItemCollection);
	mapRequest.SetAt("ItemName", Request.m_ItemInfo.m_csItemID);

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_POST,
				 "GetGraphicInfo2",
				 &mapRequest,
				 &Response);

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		CString csValue;

		// Handle the status.
		HandleStatus(mapResponse, Request);

		// Get the item download info.
		if (mapResponse.Lookup("item", csValue))
		{
			ParseDownloadInfo2(csValue, Request.m_DownloadInfo);
		}
	}
}

//
// Establish an account.
//

void CStdContentServer::EstablishAccount(CServerEstablishAccountRequest& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Add EstablishAccount-specific parameters.
	AddAccountInfo(Request.m_AccountInfo, mapRequest);

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_GET,
				 "EstablishAccount",
				 &mapRequest,
				 &Response);

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		// Handle the status.
		HandleStatus(mapResponse, Request);

		// Get the status.
		mapResponse.Lookup("accountid", Request.m_csSuggestedAccountID);
	}
}

//
// Get account info.
//

void CStdContentServer::GetAccountInfo(CServerGetAccountInfoRequest& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_POST,
				 "GetAccountInfo",
				 &mapRequest,
				 &Response);

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		// Handle the status.
		HandleStatus(mapResponse, Request);

		// Get the account info.
		mapResponse.Lookup("first", Request.m_AccountInfo.m_csCustFirstName);
		mapResponse.Lookup("last", Request.m_AccountInfo.m_csCustLastName);
		mapResponse.Lookup("street1", Request.m_AccountInfo.m_csStreet1);
		mapResponse.Lookup("street2", Request.m_AccountInfo.m_csStreet2);
		mapResponse.Lookup("city", Request.m_AccountInfo.m_csCity);
		mapResponse.Lookup("state", Request.m_AccountInfo.m_csState);
		mapResponse.Lookup("country", Request.m_AccountInfo.m_csCountry);
		mapResponse.Lookup("zipcode", Request.m_AccountInfo.m_csZip);
		mapResponse.Lookup("email", Request.m_AccountInfo.m_csEMailAddr);
		mapResponse.Lookup("phone", Request.m_AccountInfo.m_csPhone);
	}
}

//
// Update account info.
//

void CStdContentServer::UpdateAccountInfo(CServerUpdateAccountInfoRequest& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Add UpdateAccountInfo-specific parameters.
	AddAccountInfo(Request.m_AccountInfo, mapRequest);
	mapRequest.SetAt("PwdNew", Request.m_csNewPassword);

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_POST,
				 "UpdateAccount",
				 &mapRequest,
				 &Response);

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		// Handle the status.
		HandleStatus(mapResponse, Request);
	}
}

//
// Get info for a list of ads.
//

void CStdContentServer::GetAdList(CServerGetAdListRequest& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Add GetAdList-specific parameters.
	mapRequest.SetAt("List", Request.m_csListType);
	mapRequest.SetAt("IgnoreLocale", Request.m_csIgnoreLocale);

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_POST,
				 "GetAdList",
				 &mapRequest,
				 &Response);

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		CString csValue;

		// Handle the status.
		HandleStatus(mapResponse, Request);

      int  nCurrent = 1;

      for (;;)
      {
			CString csLookup;
			csLookup.Format("adinfo%d", nCurrent);

			// Get the list of collections.
			if (mapResponse.Lookup(csLookup, csValue))
			{

				CServerAdInfo Info;
			
				ParseAdInfo(csValue, Info);
				Request.m_Ads.Add(Info);

				nCurrent++;
			}
			else
			{
				break;
			}
         
      }
	}
}

//
// Get info for a particular ad.
//

void CStdContentServer::GetAdInfo(CServerGetAdInfoRequest& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Add GetAdInfo-specific parameters.
	mapRequest.SetAt("AdId", Request.m_csAdID);

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_POST,
				 "GetAdInfo",
				 &mapRequest,
				 &Response);

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		// Handle the status.
		HandleStatus(mapResponse, Request);

		mapResponse.Lookup("adid", Request.m_AdInfo.m_csAdID);
		mapResponse.Lookup("filename", Request.m_AdInfo.m_csFileName);
		mapResponse.Lookup("locale", Request.m_AdInfo.m_csLocale);
		mapResponse.Lookup("description", Request.m_AdInfo.m_csDescription);
		mapResponse.Lookup("urllink", Request.m_AdInfo.m_csURLLink);
	}
}

//
// Mark an ad as seen.
//

void CStdContentServer::MarkAdSeen(CServerMarkAdSeenRequest& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Add MarkAdSeen-specific parameters.
	mapRequest.SetAt("AdId", Request.m_csAdID);

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_POST,
				 "MarkAdSeen",
				 &mapRequest,
				 &Response);

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		// Handle the status.
		HandleStatus(mapResponse, Request);

	}
}

//
// Get a list of collections.
//

void CStdContentServer::GetCollectionList(CServerGetCollectionListRequest& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Add GetCollectionList-specific parameters.
	mapRequest.SetAt("List", Request.m_csListType);
	mapRequest.SetAt("IgnoreLocale", Request.m_csIgnoreLocale);

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_POST,
				 "GetCollectionList",
				 &mapRequest,
				 &Response);

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		CString csValue;

		// Handle the status.
		HandleStatus(mapResponse, Request);

      int  nCurrent = 1;

      for (;;)
      {
			CString csLookup;
			csLookup.Format("collinfo%d", nCurrent);

			// Get the list of collections.
			if (mapResponse.Lookup(csLookup, csValue))
			{

				CServerCollectionInfo Info;
			
				ParseCollectionInfo(csValue, Info);
				Request.m_Collections.Add(Info);

				nCurrent++;
			}
			else
			{
				break;
			}
         
      }
	}
}

//
// Get collection info.
//

void CStdContentServer::GetCollectionInfo(CServerGetCollectionInfoRequest& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Add GetCollectionInfo-specific parameters.
	mapRequest.SetAt("CollId", Request.m_csCollectionID);

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_POST,
				 "GetCollectionInfo",
				 &mapRequest,
				 &Response);

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		// Handle the status.
		HandleStatus(mapResponse, Request);

		mapResponse.Lookup("collid", Request.m_CollectionInfo.m_csCollectionID);
		mapResponse.Lookup("name", Request.m_CollectionInfo.m_csName);
		mapResponse.Lookup("filename", Request.m_CollectionInfo.m_csFileName);
		mapResponse.Lookup("locale", Request.m_CollectionInfo.m_csLocale);
		mapResponse.Lookup("description", Request.m_CollectionInfo.m_csDescription);
		mapResponse.Lookup("price", Request.m_CollectionInfo.m_csPrice);
		mapResponse.Lookup("productcode", Request.m_CollectionInfo.m_csProductCode);
		mapResponse.Lookup("sku", Request.m_CollectionInfo.m_csSKU);
		mapResponse.Lookup("iconfilename", Request.m_CollectionInfo.m_csIconFileName);
		mapResponse.Lookup("brochurefilename", Request.m_CollectionInfo.m_csBrochureFileName);
		mapResponse.Lookup("cd", Request.m_CollectionInfo.m_csCDLocation);
	}
}

//
// Mark collection seen.
//

void CStdContentServer::MarkCollectionSeen(CServerMarkCollectionSeenRequest& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Add MarkCollectionSeen-specific parameters.
	mapRequest.SetAt("CollID", Request.m_csCollectionID);

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_POST,
				 "MarkCollectionSeen",
				 &mapRequest,
				 &Response);

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		// Handle the status.
		HandleStatus(mapResponse, Request);
	}
}

//
// Get a list of collections.
//

void CStdContentServer::GetCDList(CServerGetCDListRequest& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Add GetCDList-specific parameters.
	mapRequest.SetAt("List", Request.m_csListType);
	mapRequest.SetAt("IgnoreLocale", Request.m_csIgnoreLocale);

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_POST,
				 "GetCDList",
				 &mapRequest,
				 &Response);

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		CString csValue;

		// Handle the status.
		HandleStatus(mapResponse, Request);

      int  nCurrent = 1;

      for (;;)
      {
			CString csLookup;
			csLookup.Format("cdinfo%d", nCurrent);

			// Get the list of collections.
			if (mapResponse.Lookup(csLookup, csValue))
			{

				CServerCDInfo Info;
			
				ParseCDInfo(csValue, Info);
				Request.m_CDs.Add(Info);

				nCurrent++;
			}
			else
			{
				break;
			}
         
      }
	}
}

//
// Get CD info.
//

void CStdContentServer::GetCDInfo(CServerGetCDInfoRequest& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Add GetCDInfo-specific parameters.
	mapRequest.SetAt("CDID", Request.m_csCDID);

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_POST,
				 "GetCDInfo",
				 &mapRequest,
				 &Response);

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		// Handle the status.
		HandleStatus(mapResponse, Request);

		mapResponse.Lookup("cdid", Request.m_CDInfo.m_csCDID);
		mapResponse.Lookup("name", Request.m_CDInfo.m_csName);
		mapResponse.Lookup("locale", Request.m_CDInfo.m_csLocale);
		mapResponse.Lookup("description", Request.m_CDInfo.m_csDescription);
		mapResponse.Lookup("availabilitydate", Request.m_CDInfo.m_csAvailabilityDate);
		mapResponse.Lookup("price", Request.m_CDInfo.m_csPrice);
	}
}

//
// Mark cd seen.
//

void CStdContentServer::MarkCDSeen(CServerMarkCDSeenRequest& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Add MarkCDSeen-specific parameters.
	mapRequest.SetAt("CDID", Request.m_csCDID);

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_POST,
				 "MarkCDSeen",
				 &mapRequest,
				 &Response);

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		// Handle the status.
		HandleStatus(mapResponse, Request);
	}
}

//
// Get a graphic (content).
//

void CStdContentServer::GetGraphic(CServerGetGraphicRequest& Request)
{
	// Set up to do the request call.
	CMapStringToString mapRequest;

	Request.m_ConnectionInfo.m_csAuthenticationLogin = Request.m_DownloadInfo.m_csLoginID;
	Request.m_ConnectionInfo.m_csAuthenticationPassword = Request.m_DownloadInfo.m_csPassword;

	// Our download response handler.
	ASSERT(Request.m_pDownloadCallback != NULL);
	CStdDownloadServerResponse Response(Request.m_pDownloadCallback);

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_GET,
				 Request.m_DownloadInfo.m_csFile,
				 NULL,
				 &Response);

	if (CheckCompletion(Request))
	{
		// Handle the status.
		Request.m_csStatus = '0';
		Request.m_nStatus = 0;
	}
}

//
// Get a order form information.
//

void CStdContentServer::GetOrderForm(CServerGetOrderFormRequest& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_POST,
				 "GetOrderFormInfo",
				 &mapRequest,
				 &Response);

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		CString csValue;

		// Handle the status.
		HandleStatus(mapResponse, Request);

		CServerOrderFormInfo* pInfo = Request.m_pInfo;

		if (pInfo != NULL)
		{
			mapResponse.Lookup("orderheader", pInfo->m_csOrderHeader);

			// Get the credit card types
			if (mapResponse.Lookup("cardtypes", csValue))
			{
				ParseStringArray(csValue, pInfo->m_csaCardTypes, "\t");
			}

			for (int nCurrent = 1;; nCurrent++)
			{
				CString csLookup;
				csLookup.Format("collinfo%d", nCurrent);

				// Get the list of collections.
				if (mapResponse.Lookup(csLookup, csValue))
				{
					CServerOrderFormCollectionInfo Info;

					ParseOrderFormCollection(csValue, Info);
					pInfo->m_Collections.Add(Info);
				}
				else
				{
					break;
				}
			}

		}
	}
}

//
// Get country info.
//

void CStdContentServer::GetCountryInfo(CServerGetCountryInfoRequest& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_POST,
				 "GetCountryInfo",
				 &mapRequest,
				 &Response);

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		// Handle the status.
		HandleStatus(mapResponse, Request);

		// Get the country info.
		for (int nCurrent = 1;; nCurrent++)
		{
			CString csLookup;
			csLookup.Format("country%d", nCurrent);

			// Get the list of countries.
			CString csValue;
			if (mapResponse.Lookup(csLookup, csValue))
			{
				CServerCountryInfo Info;

				ParseCountryInfo(csValue, Info);
				Request.m_Countries.Add(Info);
			}
			else
			{
				break;
			}
		}
	}
}

//
// Get shipping info.
//

void CStdContentServer::GetShippingInfo(CServerGetShippingInfoRequest& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Add the location parameters.
	AddLocationInfo(Request.m_LocationInfo, mapRequest);

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_POST,
				 "GetShippingInfo",
				 &mapRequest,
				 &Response);

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		// Handle the status.
		HandleStatus(mapResponse, Request);

		// Get the shipping info.
		for (int nCurrent = 1;; nCurrent++)
		{
			CString csLookup;
			csLookup.Format("ship%d", nCurrent);

			// Get the list of shipping methods.
			CString csValue;
			if (mapResponse.Lookup(csLookup, csValue))
			{
				CServerShippingInfo Info;

				ParseShippingInfo(csValue, Info);
				Request.m_ShippingMethods.Add(Info);
			}
			else
			{
				break;
			}
		}
	}
}

//
// Price order information.
//

void CStdContentServer::PriceOrder(CServerPriceOrderRequest& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Add the account info.
	AddLocationInfo(Request.m_OrderInfo.m_LocationInfo, mapRequest);

	// Add the purchase item info.
	AddPurchaseItemInfo(Request.m_OrderInfo.m_ItemInfo, mapRequest);

	// Add the shipping info.
	mapRequest.SetAt("ShipMethod", Request.m_OrderInfo.m_csShipID);

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_POST,
				 "PriceOrder",
				 &mapRequest,
				 &Response);

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		CString csValue;

		// Handle the status.
		HandleStatus(mapResponse, Request);

		mapResponse.Lookup("colltotal", Request.m_PriceInfo.m_csCollectionTotal);
		mapResponse.Lookup("collcount", Request.m_PriceInfo.m_csCollectionCount);
		mapResponse.Lookup("cdtotal", Request.m_PriceInfo.m_csCDTotal);
		mapResponse.Lookup("cdcount", Request.m_PriceInfo.m_csCDCount);
		mapResponse.Lookup("itemtotal", Request.m_PriceInfo.m_csItemTotal);
		mapResponse.Lookup("discount", Request.m_PriceInfo.m_csDiscount);
		mapResponse.Lookup("salestax", Request.m_PriceInfo.m_csSalesTax);
		mapResponse.Lookup("shipping", Request.m_PriceInfo.m_csShipping);
		mapResponse.Lookup("ordertotal", Request.m_PriceInfo.m_csOrderTotal);

	}
}

//
// Price order information.
//

void CStdContentServer::Purchase(CServerPurchaseRequest& Request)
{
	CMapStringToString mapRequest;

	// Add the basic parameters.
	AddBasicParameters(Request, mapRequest);

	// Add the account info.
	AddAccountInfo(Request.m_PurchaseInfo.m_AccountInfo, mapRequest);

	// Add the purchase item info.
	AddPurchaseItemInfo(Request.m_PurchaseInfo.m_ItemInfo, mapRequest);

	// Add the shipping info.
	mapRequest.SetAt("ShipMethod", Request.m_PurchaseInfo.m_csShipID);

	// Add the payment info.
	AddPaymentInfo(Request.m_PurchaseInfo.m_PaymentInfo, mapRequest);

	// Our response.
	CStandardServerResponse Response;

	// Send the request.
	DoRequest(Request.m_ConnectionInfo,
				 CHttpConnection::HTTP_VERB_POST,
				 "Purchase",
				 &mapRequest,
				 &Response,
				 TRUE);  //secure

	if (CheckCompletion(Request))
	{
		CMapStringToString& mapResponse = Response.m_mapResponse;

		CString csValue;

		// Handle the status.
		HandleStatus(mapResponse, Request);

		mapResponse.Lookup("ordernumber", Request.m_PurchaseResult.m_csOrderNumber);
		mapResponse.Lookup("resulttext", Request.m_PurchaseResult.m_csResultText);
	}
}


//
// Add the basic parameters to a map.
//

void CStdContentServer::AddBasicParameters(const CServerBasicRequest& Request, CMapStringToString& mapRequest)
{
	mapRequest.SetAt("Acct", Request.m_csAccountID);
	mapRequest.SetAt("Pwd", Request.m_csPassword);
	mapRequest.SetAt("Sess", Request.m_csSessionID);
	mapRequest.SetAt("Req", Request.m_csRequestID);
	mapRequest.SetAt("SKU", Request.m_csClientSKU);
	mapRequest.SetAt("Ver", Request.m_csClientVersion);
	mapRequest.SetAt("PGS", Request.m_csClientPGSVersion);
	mapRequest.SetAt("Loc", Request.m_csClientLocale);
}

void CStdContentServer::AddAccountInfo(const CServerAccountInfo& AccountInfo, CMapStringToString& mapRequest)
{
	mapRequest.SetAt("First", AccountInfo.m_csCustFirstName);
	mapRequest.SetAt("Last", AccountInfo.m_csCustLastName);
	mapRequest.SetAt("Street1", AccountInfo.m_csStreet1);
	mapRequest.SetAt("Street2", AccountInfo.m_csStreet2);
	mapRequest.SetAt("City", AccountInfo.m_csCity);
	mapRequest.SetAt("State", AccountInfo.m_csState);
	mapRequest.SetAt("Country", AccountInfo.m_csCountry);
	mapRequest.SetAt("Zip", AccountInfo.m_csZip);
	mapRequest.SetAt("EMail", AccountInfo.m_csEMailAddr);
	mapRequest.SetAt("Phone", AccountInfo.m_csPhone);
}

//
// Add location info.
//

void CStdContentServer::AddLocationInfo(const CServerLocationInfo& LocationInfo, CMapStringToString& mapRequest)
{
	mapRequest.SetAt("State", LocationInfo.m_csState);
	mapRequest.SetAt("Zip", LocationInfo.m_csZip);
	mapRequest.SetAt("Country", LocationInfo.m_csCountry);
}

//
// Add purchase item info.
//

void CStdContentServer::AddPurchaseItemInfo(const CServerPurchaseItemInfo& ItemInfo, CMapStringToString& mapRequest)
{
	CString csList;
	// Add the CDs requested.
	ConstructList(csList, ItemInfo.m_csaCDs, ',');
	mapRequest.SetAt("CDIDs", csList);

	// Add the collections requested.
	csList.Empty();
	ConstructList(csList, ItemInfo.m_csaCollections, ',');
	mapRequest.SetAt("CollIDs", csList);
}

//
// Add payment info.
//

void CStdContentServer::AddPaymentInfo(const CServerPaymentInfo& PaymentInfo, CMapStringToString& mapRequest)
{
	mapRequest.SetAt("CardType", PaymentInfo.m_csCardType);
	mapRequest.SetAt("CardNumber", PaymentInfo.m_csCardNumber);
	mapRequest.SetAt("ExpirationDate", PaymentInfo.m_csCardExpiration);
}

//
// Do a request.
//

void CStdContentServer::DoRequest(CServerConnectionInfo& ConnectionInfo,
											 int nVerb,
											 LPCSTR pszObject,
											 const CMapStringToString* pmapRequest,
											 CContentServerResponse* pResponse,
											 BOOL fSecure /*=FALSE*/)
{
	// Verify the connection server URL list.

	if (ConnectionInfo.m_pServerURLs == NULL
		 || ConnectionInfo.m_pServerURLs->IsEmpty())
	{
#ifdef	_DEBUG
			AfxMessageBox("Debug: CStdContentServer::DoRequest()\nConnectionInfo.m_pServerURLs NULL or empty");
#endif
		ConnectionInfo.m_dwResult = ERROR_INVALID_PARAMETER;
		return;
	}

	// The result from this call.
	// This will be the Windows error result (e.g. via GetLastError()).

	DWORD dwResult;

	// Initialize the server status code.
	ConnectionInfo.m_dwServerStatusCode = 0;		// Set it to something.

	// Set the request flags.
	DWORD dwRequestFlags = INTERNET_FLAG_NO_CACHE_WRITE;
	if (fSecure)
	{
		dwRequestFlags |= INTERNET_FLAG_SECURE;
	}

	AfxGetApp()->BeginWaitCursor();

	// Build the parameter string. pmapRequest may be NULL.
	CString csParameters;
	BuildParameterString(pmapRequest, csParameters);

	CString csObject = ConnectionInfo.m_csBasePath;
	if (!csObject.IsEmpty())
	{
		// We need a '/' between components.
		csObject += '/';
	}
	csObject += pszObject;

	// If we are doing a "GET", we need to append the parameters.
	if (nVerb == CHttpConnection::HTTP_VERB_GET && !csParameters.IsEmpty())
	{
		csObject += '?';
		csObject += csParameters;
	}

	// Set the user name and password if we have them.
	// We assume that a non-empty user name means we have valid info for both.
	LPCSTR pszUserName = NULL;
	LPCSTR pszPassword = NULL;
	if (!ConnectionInfo.m_csAuthenticationLogin.IsEmpty())
	{
		pszUserName = ConnectionInfo.m_csAuthenticationLogin;
		pszPassword = ConnectionInfo.m_csAuthenticationPassword;
	}

	// Set up our internet session.
	CStdInternetSession InternetSession(ConnectionInfo.m_pConnectionCallback);

	CStdioFile* pLogFile = GetLogFile();
	CString csLine;
	if (pLogFile != NULL)
	{
		// Write this request.
		csLine.Format("Request type: %s\n",
						  (nVerb == CHttpConnection::HTTP_VERB_GET)
								? "GET" : "POST");
		pLogFile->WriteString(csLine);
		// Write the requested object.
		csLine.Format("Object: %s\n", (LPCSTR)csObject);
		pLogFile->WriteString(csLine);
		// If a POST, write the separate parameters.
		if (nVerb == CHttpConnection::HTTP_VERB_POST)
		{
			csLine.Format("POST Parameters: %s\n", (LPCSTR)csParameters);
			pLogFile->WriteString(csLine);
		}
		if (pszUserName != NULL)
		{
			csLine.Format("Authentication: Name='%s', Password='%s'\n",
							  pszUserName, pszPassword);
			pLogFile->WriteString(csLine);
		}
		csLine.Format("Secure transaction: %s\n", fSecure ? "Yes" : "No");
		pLogFile->WriteString(csLine);
	}

	// Get the number of servers. That is the most we try.
	int nServers = ConnectionInfo.m_pServerURLs->GetCount();
	for (int nServer = 0; nServer < nServers; nServer++)
	{
		// Get the first in the list (current).
		CString csServerURL = ConnectionInfo.m_pServerURLs->GetHead();
		TRACE("Try server: %s\n", (LPCSTR)csServerURL);

		// Split it into the server and port numbers.
		CString csServer;
		int nPort;
		SplitURL(csServerURL, &csServer, &nPort);
		if(fSecure)
			nPort = 443;

		if (pLogFile != NULL)
		{
			csLine.Format("Trying server '%s' on port %d\n", (LPCSTR)csServer, nPort);
			pLogFile->WriteString(csLine);
		}

		CHttpConnection* pHttpConnection = NULL;
		CHttpFile* pHttpFile = NULL;

		TRY
		{
			//  Get a connection
			if ((pHttpConnection = InternetSession.GetHttpConnection(csServer, nPort, pszUserName, pszPassword)) != NULL)
			{
				// Open the request file.
				if ((pHttpFile = pHttpConnection->OpenRequest(nVerb,
																			 csObject,
																			 NULL,
																			 1,
																			 NULL,
																			 NULL,
																			 dwRequestFlags)) != NULL)
				{
					// Do the send.
					switch (nVerb)
					{
						case CHttpConnection::HTTP_VERB_GET:
						{
							// Do a send without data.
							pHttpFile->SendRequest();
							break;
						}
						case CHttpConnection::HTTP_VERB_POST:
						{
							// Do a send with data.
							pHttpFile->SendRequest(CString("Content-Type: application/x-www-form-urlencoded"),
														  (LPVOID)(LPCSTR)csParameters,
														  csParameters.GetLength());
							break;
						}
						default:
						{
							// What kind of post is this?
							ASSERT(FALSE);
							break;
						}
					}

					// Success (no exceptions). Get the server status code.
					pHttpFile->QueryInfoStatusCode(ConnectionInfo.m_dwServerStatusCode);
					dwResult = 0;

					// Get the response if no error.
					if (ConnectionInfo.m_dwServerStatusCode == HTTP_STATUS_OK)
					{
						// Handle the response.
						if (pResponse != NULL)
						{
							dwResult = pResponse->GetResponse(pHttpFile, pLogFile);
						}
					}
					else
					{
						// Log this error.
						if (pLogFile != NULL)
						{
							csLine.Format("ERROR: Server responded with %ld\n", ConnectionInfo.m_dwServerStatusCode);
							pLogFile->WriteString(csLine);
						}
					}
				}
			}
		}
		CATCH(CInternetException, pEx)
		{
			dwResult = pEx->m_dwError;
			// Log this error.
			if (pLogFile != NULL)
			{
				csLine.Format("ERROR: CInternetException thrown (%ld)\n", dwResult);
				pLogFile->WriteString(csLine);
			}
		}
		AND_CATCH_ALL(e)
		{
			ASSERT(FALSE);
			dwResult = (DWORD)-1;
		}
		END_CATCH_ALL

		// Clean up any objects we created.
		if (pHttpConnection != NULL)
		{
			if (pHttpFile != NULL)
			{
				// Free the file.
				pHttpFile->Close();
				delete pHttpFile;
			}

			// Free the connection.
			pHttpConnection->Close();
			delete pHttpConnection;
		}

		// See if we can break now.
		if (dwResult == 0)
		{
			// Success! Leave this server on top (current) for next time.
			break;
		}

		// We got an error. Rotate the server list. This server goes to the end.
		ConnectionInfo.m_pServerURLs->AddTail(ConnectionInfo.m_pServerURLs->RemoveHead());

		// And loop back to the next server.
	}

	// Delete the log file if it has not been deleted already.
	delete pLogFile;

	AfxGetApp()->EndWaitCursor();

	// Set the result.
	ConnectionInfo.m_dwResult = dwResult;
}

//
// Make sure we completed a request.
//

BOOL CStdContentServer::CheckCompletion(CServerBasicRequest& Request)
{
	BOOL fSuccess = FALSE;

	if (Request.m_ConnectionInfo.m_dwResult != 0)
	{
		// Communication error.
		Request.m_nStatus = CSSTATUS_CommunicationException;
		Request.m_csStatus.Format("%d", Request.m_nStatus);
	}
	else if (Request.m_ConnectionInfo.m_dwServerStatusCode != HTTP_STATUS_OK)
	{
		// Server error.
		Request.m_nStatus = CSSTATUS_ServerException;
		Request.m_csStatus.Format("%d", Request.m_nStatus);
	}
	else
	{
		// No "talking to the server" problem.
		fSuccess = TRUE;
	}
	return fSuccess;
}

//
// Split a URL of the form "URL:port" or "URL" into URL and port numbers.
// It is *not* prefixed by "http://".
//

void CStdContentServer::SplitURL(const CString& csFullURL, CString* pcsURL, int* pPort)
{
	// Break the address into URL and port.
	int nIndex = csFullURL.Find(':');
	if (nIndex == -1)
	{
		*pcsURL = csFullURL;
		*pPort = 0;
	}
	else
	{
		*pcsURL = csFullURL.Left(nIndex);
		*pPort = atoi(csFullURL.Mid(nIndex+1));
	}

	// Fix the port if 0.
	if (*pPort == 0)
	{
		*pPort = INTERNET_INVALID_PORT_NUMBER;		// default
	}
}

// Handle the status.
void CStdContentServer::HandleStatus(const CMapStringToString& mapResponse, CServerBasicRequest& Request)
{
	if (mapResponse.Lookup("status", Request.m_csStatus))
	{
		Request.m_nStatus = atoi(Request.m_csStatus);
	}
	else
	{
		// Error of unknown type.
		Request.m_nStatus = CSSTATUS_InvalidLoginOrPassword;
	}
}

//
// Build a URL-encode parameter string from a map.
//

void CStdContentServer::BuildParameterString(const CMapStringToString* pmapRequest, CString& csParameterString)
{
	if (pmapRequest != NULL)
	{
		POSITION pos = pmapRequest->GetStartPosition();
		while (pos != NULL)
		{
			// Get the next pair.
			CString rKey;
			CString rValue;
			pmapRequest->GetNextAssoc(pos, rKey, rValue);

			// If we already have something in the string, separate appropriately.
			if (!csParameterString.IsEmpty())
			{
				csParameterString += '&';
			}

			// Append the new attribute/value pair.
			csParameterString += rKey;
			csParameterString += '=';
			csParameterString += rValue;
		}
	}
	// URL-encode the string.
	UrlEncodeString(csParameterString);
}

//
// URL-encode a string.
//

void CStdContentServer::UrlEncodeString(CString& csString)
{
	int l = csString.GetLength();
	for (int n = 0; n < l; n++)
	{
		if (csString.GetAt(n) == ' ')
		{
			csString.SetAt(n, '+');
		}
	}
}

//
// Construct a delimited list from an array of strings.
//

void CStdContentServer::ConstructList(CString& csList, const CStringArray& csaItems, char cSeparator)
{
	for (int i = 0; i < csaItems.GetSize(); i++)
	{
		// If we have something in the list already, separate it.
		if (!csList.IsEmpty())
		{
			csList += cSeparator;
		}
		// Add the next item.
		csList += csaItems.GetAt(i);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Response item parsing.

//
// Parse an item download line.
//

void CStdContentServer::ParseDownloadInfo(LPCSTR pszValue, CServerDownloadInfo& Info)
{
	// Split the line into fields.
	CStringArray csaFields;
	ParseStringArray(pszValue, csaFields, "\t");
	// If we have enough fields, fill out the download info.
	if (csaFields.GetSize() >= 3)
	{
		Info.m_csFile = csaFields.GetAt(0);
		Info.m_csLoginID = csaFields.GetAt(1);
		Info.m_csPassword = csaFields.GetAt(2);
	}
}

void CStdContentServer::ParseDownloadInfo2(LPCSTR pszValue, CServerDownloadInfo& Info)
{
	CMapStringToString mapFields;
	ParseSubFields(pszValue, mapFields);

	mapFields.Lookup("filename", Info.m_csFile);
	mapFields.Lookup("loginid", Info.m_csLoginID);
	mapFields.Lookup("passwordid", Info.m_csPassword);
}

void CStdContentServer::ParseAdInfo(LPCSTR pszValue, CServerAdInfo& Info)
{
	// Split the line into fields.
	CStringArray csaFields;
	ParseStringArray(pszValue, csaFields, "\t");
	// If we have enough fields, fill out the download info.
	if (csaFields.GetSize() >= 5)
	{
		Info.m_csAdID = csaFields.GetAt(0);
		Info.m_csFileName = csaFields.GetAt(1);
		Info.m_csLocale = csaFields.GetAt(2);
		Info.m_csDescription = csaFields.GetAt(3);
		Info.m_csURLLink = csaFields.GetAt(4);
	}
}

void CStdContentServer::ParseAdInfo2(LPCSTR pszValue, CServerAdInfo& Info)
{
	CMapStringToString mapFields;
	ParseSubFields(pszValue, mapFields);

	mapFields.Lookup("adid", Info.m_csAdID);
	mapFields.Lookup("filename", Info.m_csFileName);
	mapFields.Lookup("locale", Info.m_csLocale);
	mapFields.Lookup("description", Info.m_csDescription);
	mapFields.Lookup("urllink", Info.m_csURLLink);
}

//
// Parse a collection info line.
//

void CStdContentServer::ParseCollectionInfo(LPCSTR pszValue, CServerCollectionInfo& Info)
{
	CMapStringToString mapFields;
	ParseSubFields(pszValue, mapFields);

	mapFields.Lookup("collid", Info.m_csCollectionID);
	mapFields.Lookup("name", Info.m_csName);
	mapFields.Lookup("filename", Info.m_csFileName);
	mapFields.Lookup("locale", Info.m_csLocale);
	mapFields.Lookup("description", Info.m_csDescription);
	mapFields.Lookup("collprice", Info.m_csPrice);
	mapFields.Lookup("productcode", Info.m_csProductCode);
	mapFields.Lookup("sku", Info.m_csSKU);
	mapFields.Lookup("iconfilename", Info.m_csIconFileName);
	mapFields.Lookup("brochurefilename", Info.m_csBrochureFileName);
}

//
// Parse a CD info line.
//

void CStdContentServer::ParseCDInfo(LPCSTR pszValue, CServerCDInfo& Info)
{
	CMapStringToString mapFields;
	ParseSubFields(pszValue, mapFields);

	mapFields.Lookup("cdid", Info.m_csCDID);
	mapFields.Lookup("name", Info.m_csName);
	mapFields.Lookup("locale", Info.m_csLocale);
	mapFields.Lookup("description", Info.m_csDescription);
	mapFields.Lookup("availabilitydate", Info.m_csAvailabilityDate);
	mapFields.Lookup("price", Info.m_csPrice);
}

//
// Parse order form info line.
//

void CStdContentServer::ParseOrderFormCollection(LPCSTR pszValue, CServerOrderFormCollectionInfo& Info)
{
	CMapStringToString mapFields;
	ParseSubFields(pszValue, mapFields);

	mapFields.Lookup("cdid", Info.m_csCDID);
	mapFields.Lookup("cdname", Info.m_csCDName);
	mapFields.Lookup("cddescription", Info.m_csCDDescription);
	mapFields.Lookup("cdprice", Info.m_csCDPrice);
	mapFields.Lookup("cdsku", Info.m_csCDSKU);
	mapFields.Lookup("cdpurchased", Info.m_csCDPurchased);
	mapFields.Lookup("collid", Info.m_csCollectionID);
	mapFields.Lookup("collname", Info.m_csCollectionName);
	mapFields.Lookup("colldescription", Info.m_csCollectionDescription);
	mapFields.Lookup("collprice", Info.m_csCollectionPrice);
	mapFields.Lookup("collsku", Info.m_csCollectionSKU);
	mapFields.Lookup("collpurchased", Info.m_csCollectionPurchased);
	mapFields.Lookup("cdlocation", Info.m_csCDLocation);
}

//
// Parse a string array from a line.
//

void CStdContentServer::ParseStringArray(LPCSTR pszValue, CStringArray& csaValues, LPCSTR pszSeparator /*='\t'*/)
{
	// Split the line into fields.
	Util::FillArrayFromDelimitedString(pszValue, pszSeparator, csaValues);
}

//
// Parse a string list from a line.
//

void CStdContentServer::ParseStringList(LPCSTR pszValue, CStringList& cslValues, LPCSTR pszSeparator)
{
	// Split the line into fields.
	Util::FillListFromDelimitedString(pszValue, pszSeparator, cslValues);
}

void CStdContentServer::ParseSubFields(LPCSTR pszLine, CMapStringToString& Map)
{
	CStringArray csaFields;
	ParseStringArray(pszLine, csaFields);
	for (int n = 0; n < csaFields.GetSize(); n++)
	{
		CString csField = csaFields.GetAt(n);
		// Look for the separator.
		int nIndex = csField.Find('|');
		if (nIndex != -1)
		{
			// Get the attribute (make it lower case).
			CString csAttribute = csField.Left(nIndex);
			csAttribute.MakeLower();
			// Get the value.
			CString csValue = csField.Mid(nIndex+1);
			// Set the pair.
			Map.SetAt(csAttribute, csValue);
		}
	}
}

//
// Parse country information.
//

void CStdContentServer::ParseCountryInfo(LPCSTR pszValue, CServerCountryInfo& Info)
{
	CMapStringToString mapFields;
	ParseSubFields(pszValue, mapFields);

	mapFields.Lookup("name", Info.m_csName);
	mapFields.Lookup("code", Info.m_csCode);
}

//
// Parse shipping information.
//

void CStdContentServer::ParseShippingInfo(LPCSTR pszValue, CServerShippingInfo& Info)
{
	CMapStringToString mapFields;
	ParseSubFields(pszValue, mapFields);

	mapFields.Lookup("name", Info.m_csName);
	mapFields.Lookup("description", Info.m_csDescription);
	mapFields.Lookup("price", Info.m_csPrice);
	mapFields.Lookup("shipid", Info.m_csShipID);
}

/////////////////////////////////////////////////////////////////////////////
// CStandardServerResponse

//
// Get the response from a successful standard request.
//

DWORD CStandardServerResponse::GetResponse(CHttpFile* pHttpFile, CStdioFile* pLogFile /*=NULL*/)
{
	DWORD dwResult = 0;

	//Take the HttpFile and parse it.
	CString csBuffer;

	pHttpFile->SetReadBufferSize(4096);

	for (;;)
	{
		char cbData[256];
		TRY
		{
			int nRead = pHttpFile->Read(cbData, sizeof(cbData)-1);
			if (nRead == 0)
			{
				break;
			}

			// Append the data.
			cbData[nRead] = '\0';
			csBuffer += cbData;
		}
		CATCH(CInternetException, e)
		{
			dwResult = e->m_dwError;
			break;
		}
		AND_CATCH_ALL(e)
		{
			dwResult = ERROR_READ_FAULT;
			break;
		}
		END_CATCH_ALL
	}

	if (pLogFile != NULL)
	{
		CString csLine;
		if (dwResult == 0)
		{
			// We got some data.
			csLine = ">>>> Beginning of response from server <<<<\n";
			csLine += csBuffer;
			csLine += ">>>> End of response from server <<<<\n";
		}
		else
		{
			csLine.Format("ERROR: Got %ld reading response\n", dwResult);
		}
		pLogFile->WriteString(csLine);
	}

	// If we read the data, parse it for attribute/value pairs.
	if (dwResult == 0)
	{
		// Parse the string for lines.
		while (!csBuffer.IsEmpty())
		{
			CString csLine;
			// Break out the next line feed.
			// We assume there are always '\n's.
			int nEOL = csBuffer.Find('\n');
			if (nEOL == -1)
			{
				// No more line breaks. Use the whole string.
				csLine = csBuffer;
				csBuffer.Empty();
			}
			else
			{
				// Separate out the next line.
				int n = nEOL;
				if (n > 1 && csBuffer[n-1] == '\r')
				{
					n--;
				}

				csLine = csBuffer.Left(n);
				csBuffer = csBuffer.Mid(nEOL+1);
			}
//			TRACE("Line: |%s|\n", (LPCSTR)csLine);
			// Process the line.
			// Look for the equals '='.
			int nEquals = csLine.Find('=');
			if (nEquals == -1)
			{
				// A line without an equals. Ignore it.
			}
			else
			{
				// Get the attribute (make it lower case).
				CString csAttribute = csLine.Left(nEquals);
				csAttribute.MakeLower();
				// Get the value.
				CString csValue = csLine.Mid(nEquals+1);
				// Set the pair.
				m_mapResponse.SetAt(csAttribute, csValue), 
				TRACE("Set %s = %s\n", (LPCSTR)csAttribute, (LPCSTR)csValue);
			}
		}
	}
	return dwResult;
}

/////////////////////////////////////////////////////////////////////////////
// CStdDownloadServerResponse

//
// Get the response from a successful download request.
//

DWORD CStdDownloadServerResponse::GetResponse(CHttpFile* pHttpFile, CStdioFile* pLogFile /*=NULL*/)
{
	// Verify we have a callback.
	if (m_pCallback == NULL)
	{
		ASSERT(FALSE);
		return ERROR_INVALID_PARAMETER;
	}

	// Query for the length of the content.
	DWORD dwSize;
	DWORD dwBufferSize = sizeof(dwSize);
	if (pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH
										| HTTP_QUERY_FLAG_NUMBER,
									 &dwSize,
									 &dwBufferSize))
	{
		TRACE("Download data size: %ld\n", dwSize);
		if (pLogFile != NULL)
		{
			CString csLine;
			csLine.Format("Downloading %ld bytes of data...\n", dwSize);
			pLogFile->WriteString(csLine);
		}
	}
	else
	{
		// We need the size.
		return ::GetLastError();
	}

	// A buffer to use.
	char Buffer[1024];

	// Prime the callback. We call it with no data.
	DWORD dwResult = m_pCallback->OnReceiveData(Buffer, 0, dwSize);
	if (dwResult == 0)
	{
		// Read until we can't read no mo'.
		TRY
		{
			for (;;)
			{
				// Read the data until done.
				int nRead = pHttpFile->Read(Buffer, sizeof(Buffer));
				if (nRead <= 0)
				{
					break;
				}

				// Process the data.
				if ((dwResult = m_pCallback->OnReceiveData(Buffer, nRead, dwSize)) != 0)
				{
					break;
				}
			}
		}
		CATCH(CInternetException, e)
		{
			dwResult = e->m_dwError;
		}
		AND_CATCH_ALL(e)
		{
			dwResult = ERROR_READ_FAULT;
		}
		END_CATCH_ALL
	}

	if (pLogFile != NULL)
	{
		CString csLine;
		if (dwResult == 0)
		{
			csLine.Format("Download complete\n");
		}
		else
		{
			csLine.Format("ERROR: Download got %ld\n", dwResult);
		}
		pLogFile->WriteString(csLine);
	}

	return dwResult;
}

/////////////////////////////////////////////////////////////////////////////
// Interface map for CStdContentServer.

BEGIN_INTERFACE_MAP(CStdContentServer, CCmdTarget)
	INTERFACE_PART(CStdContentServer, IID_IContentServer, ContentServer)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Implementations for the IUnknown interface for CStdContentServer.
// This is standard stuff brought over from the docs.

STDMETHODIMP_(ULONG) CStdContentServer::XContentServer::AddRef()
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CStdContentServer::XContentServer::Release()
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	return pThis->ExternalRelease();
}

STDMETHODIMP CStdContentServer::XContentServer::QueryInterface(
    REFIID iid, void FAR* FAR* ppvObj)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

/////////////////////////////////////////////////////////////////////////////
// Implementations for the IContentServer interface for CStdContentServer.
// We pass all functions through to the main class. That allows us to
// get rid of this interface in the future if so desired without breaking
// the class in general.

//
// Login to the server.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::Login(CServerLoginRequest& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->Login(Request);
}

//
// Login2 to the server.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::Login2(CServerLogin2Request& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->Login2(Request);
}

//
// Get graphic info.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::GetGraphicInfo(CServerGetGraphicInfoRequest& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->GetGraphicInfo(Request);
}

//
// Get graphic info2.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::GetGraphicInfo2(CServerGetGraphicInfo2Request& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->GetGraphicInfo2(Request);
}

//
// Establish an account.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::EstablishAccount(CServerEstablishAccountRequest& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->EstablishAccount(Request);
}

//
// Get account info.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::GetAccountInfo(CServerGetAccountInfoRequest& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->GetAccountInfo(Request);
}

//
// Update account info.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::UpdateAccountInfo(CServerUpdateAccountInfoRequest& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->UpdateAccountInfo(Request);
}

//
// Get info for a list of ads.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::GetAdList(CServerGetAdListRequest& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->GetAdList(Request);
}

//
// Get info for a particular ad.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::GetAdInfo(CServerGetAdInfoRequest& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->GetAdInfo(Request);
}

//
// Mark an ad as seen.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::MarkAdSeen(CServerMarkAdSeenRequest& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->MarkAdSeen(Request);
}

//
// Get a list of collections.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::GetCollectionList(CServerGetCollectionListRequest& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->GetCollectionList(Request);
}

//
// Get collection info.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::GetCollectionInfo(CServerGetCollectionInfoRequest& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->GetCollectionInfo(Request);
}

//
// Mark collection seen.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::MarkCollectionSeen(CServerMarkCollectionSeenRequest& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->MarkCollectionSeen(Request);
}

//
// Get a list of cds.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::GetCDList(CServerGetCDListRequest& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->GetCDList(Request);
}

//
// Get cd info.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::GetCDInfo(CServerGetCDInfoRequest& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->GetCDInfo(Request);
}

//
// Mark cd seen.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::MarkCDSeen(CServerMarkCDSeenRequest& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->MarkCDSeen(Request);
}

//
// Get a graphic (content).
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::GetGraphic(CServerGetGraphicRequest& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->GetGraphic(Request);
}

//
// Get order form.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::GetOrderForm(CServerGetOrderFormRequest& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->GetOrderForm(Request);
}

//
// Get country information.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::GetCountryInfo(CServerGetCountryInfoRequest& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->GetCountryInfo(Request);
}

//
// Get shipping information.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::GetShippingInfo(CServerGetShippingInfoRequest& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->GetShippingInfo(Request);
}

//
// Price order.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::PriceOrder(CServerPriceOrderRequest& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->PriceOrder(Request);
}

//
// Purchase.
//

STDMETHODIMP_(void) CStdContentServer::XContentServer::Purchase(CServerPurchaseRequest& Request)
{
	METHOD_PROLOGUE(CStdContentServer, ContentServer)
	pThis->Purchase(Request);
}

