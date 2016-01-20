/////////////////////////////////////////////////////////////////////////////
// $Header: /PM8/App/ContMgr.cpp 2     3/08/99 5:45p Johno $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/App/ContMgr.cpp $
// 
// 2     3/08/99 5:45p Johno
// CreateNewAccount() no longer calls UpdateLicensedCollections() if the
// user cancels the operation.
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 98    2/08/99 8:48p Psasse
// New improved scheduling of Online Greetings
// 
// 97    12/17/98 4:05p Psasse
// hopefulle, randomize the data servers better
// 
// 96    12/16/98 6:33p Psasse
// No more prompting OK/Cancel with every directory deletion (on multiple
// selection)
// 
// 95    11/25/98 6:29p Psasse
// point the server back to the "live" address
// 
// 94    11/25/98 5:59p Psasse
// RequestExistingAccount no longer takes a parameter
// 
// 93    11/23/98 10:19p Psasse
// Fix crash bug on Cancellation of Account Dlg
// 
// 92    11/23/98 8:27p Psasse
// Update licensed info when creating new account
// 
// 91    11/23/98 3:58p Johno
// Changes for initial checked order item
// 
// 90    11/22/98 12:17a Psasse
// allow ads with no URL association to be seen
// 
// 89    11/22/98 12:14a Psasse
// 
// 88    11/21/98 10:45p Psasse
// 
// 87    11/18/98 10:34p Psasse
// DeleteAllCachedDirectories becomes DeleteCachedDirectories(individual)
// 
// 86    11/17/98 6:07p Psasse
// stripped build number from version string going to server
// 
// 85    11/16/98 9:56p Psasse
// overwrite all files when downloading.
// 
// 84    11/15/98 6:47p Psasse
// Two new functions for Updating Flag and Checking for new internet
// content
// 
// 83    11/11/98 5:38p Johno
// Added debug message box
// 
// 82    11/06/98 4:52p Jayn
// 
// 81    11/05/98 9:17p Psasse
// possible launching of artstore from ad info
// 
// 80    11/04/98 4:56p Jayn
// 
// 79    11/02/98 5:14p Jayn
// 
// 78    10/30/98 5:25p Jayn
// 
// 77    10/29/98 4:04p Jayn
// 
// 76    10/28/98 9:11p Psasse
// added SendAccount to the interface
// 
// 75    10/28/98 4:56p Jayn
// 
// 74    10/28/98 1:48p Jayn
// More polish for ordering and downloading.
// 
// 73    10/27/98 4:47p Jayn
// More download manager and order wizard refinements.
// 
// 72    10/26/98 9:08p Psasse
// 
// 71    10/26/98 6:47p Jayn
// Download manager.
// 
// 70    10/24/98 9:12p Psasse
// Added purchase internet order
// 
// 69    10/24/98 7:34p Jayn
// Download Manager and Download Dialog
// 
// 68    10/23/98 6:25p Jayn
// New License routines. Changes to content server API.
// 
// 67    10/22/98 4:00p Jayn
// 
// 66    10/21/98 9:06p Psasse
// added PriceOrder support
// 
// 65    10/21/98 7:01p Jayn
// 
// 64    10/21/98 5:29p Jayn
// Changed "Sort Order" to "SortOrder"
// 
// 63    10/21/98 4:14p Jayn
// More improvements to the order process.
// 
// 62    10/20/98 5:32p Jayn
// Improving the online order form.
// 
// 61    10/19/98 9:18p Psasse
// added user information functionality to online purchasing
// 
// 60    10/19/98 5:41p Jayn
// Beginnings of the online order form.
// 
// 59    10/19/98 11:21a Jayn
// 
// 58    10/19/98 11:17a Psasse
// 
// 57    10/19/98 10:48a Jayn
// More COM restructuring changes.
// 
// 56    10/15/98 3:45p Dennis
// New OCX Connection Manager
// 
// 55    10/12/98 4:33p Jayn
// Rearranged some includes to make extraction easier.
// 
// 54    10/12/98 4:14p Johno
// Added checks for NULL m_pContentServer
// 
// 53    10/11/98 8:55p Psasse
// continued conversion to COM interface
// 
// 52    10/09/98 5:53p Jayn
// New "get graphic" implementation.
// 
// 51    10/07/98 5:53p Jayn
// Changes to the content manager.
// 
// 50    10/05/98 9:03p Psasse
// Convert status codes to server-defined
// 
// 49    10/05/98 3:03p Jayn
// More content manager cleanup.
// 
// 48    10/03/98 9:15p Psasse
// Added CServerBasicRequest parameter to "Open"
// 
// 47    10/02/98 9:08p Psasse
// Correct return code from GetInternetContent
// 
// 46    10/02/98 7:10p Jayn
// More content manager changes
// 
// 45    9/28/98 10:54a Johno
// Use pmwinet.h in place of afxinet.h
// 
// 44    9/26/98 8:10p Psasse
// Major overhaul of client-side responsibilities. GetInternetContent,
// IsFileLocallyCached and MakeCachePath have been simplified
// 
// 43    9/23/98 10:27a Jayn
// Converted CollectionManager to COM-style object.
// 
// 42    9/22/98 8:58p Psasse
// cleaned  up code
// 
// 41    9/22/98 3:31p Psasse
// converting LPCSTR'S to CString&
// 
// 40    9/21/98 5:26p Jayn
// Broke PathManager out into a COM-style interface.
// 
// 2     5/31/98 5:19p Hforman
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "mainfrm.h"

#include "pmwinet.h"
#include <io.h>			// for _A_NORMAL, etc.

#include "file.h"
#include "util.h"
#include "progbar.h"
#include "inifile.h"

#include "progbar.h"
#include "OnAdDlg.h"
#include "ContMgr.h"
#include "srvacdlg.h"
#include "actacdlg.h"
#include "pmwini.h"
#include "CrPGSAcc.h"
#include "DlgURL.h"
#include "pmwcfg.h"  // For LoadConfigurationString
#include "artstore.h"
#include "uuencode.h"
#include "progstat.h"
#include "cdeflate.h"
#include "collfile.h"		// for CCompressInfo
#include "PGSOrWiz.h"
#include "glicense.h"
#include "browser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//  Server address in PMGS.ini.
static char BASED_CODE SECTION_ServerAddress[] = "Online Server";
static char BASED_CODE ENTRY_Address[] = "Address";
static char BASED_CODE ENTRY_BasePath[] = "BasePath";

// Sections/entries in user settings.
char far SECTION_ServerAccount[] = "Online Server Account";
char far ENTRY_ServerCacheDir[] = "Cache Directory";
char far ENTRY_ServerAccountName[] = "Account Name";
char far ENTRY_ServerPassword[] = "Password";

// The name of the online order file.
static char BASED_CODE OnlineOrderFile[] = "[[S]]\\ONLINE.ORD";
// Bump this whenever the order form changes format.
static char BASED_CODE OrderFormVersion[] = "1.04";

// Sections/entries used in the online order file.
static char BASED_CODE SECTION_Online[] = "Online";
static char BASED_CODE ENTRY_Revision[] = "Revision";
static char BASED_CODE ENTRY_LastSeenRevision[] = "LastSeenRevision";
static char BASED_CODE ENTRY_TimeStamp[] = "TimeStamp";

static char BASED_CODE SECTION_ArtStore[] = "Artstore";
static char BASED_CODE ENTRY_FileName[] = "FileName";
static char BASED_CODE ENTRY_TabImageFileName[] = "TabImageFileName";
static char BASED_CODE ENTRY_ProductName[] = "Product Name";
static char BASED_CODE ENTRY_SortOrder[] = "SortOrder";
static char BASED_CODE ENTRY_IsInternet[] = "IsInternet";

static char BASED_CODE SECTION_Offer[] = "Offer";
static char BASED_CODE ENTRY_Products[] = "Products";

// Entries within a collection section in the online order file.
static char BASED_CODE ENTRY_Code[] = "Code";
static char BASED_CODE ENTRY_Name[] = "Name";
static char BASED_CODE ENTRY_Price[] = "Price";
static char BASED_CODE ENTRY_CollectionCode[] = "CollectionCode";
static char BASED_CODE ENTRY_CollectionName[] = "CollectionName";
static char BASED_CODE ENTRY_CollectionBaseName[] = "CollectionBaseName";
static char BASED_CODE ENTRY_Type[] = "Type";
static char BASED_CODE ENTRY_Image[] = "Image";

extern BOOL DoHTML (HWND hwnd, LPCSTR url, BOOL GotoPage = FALSE);

IMPLEMENT_DYNAMIC(CContentCommunicationException, CException);
IMPLEMENT_DYNAMIC(CContentServerException, CException);

/////////////////////////////////////////////////////////////////////////////
// CContentManager

CContentManager::CContentManager()
{
	m_pPathManager = NULL;
	m_pUserSettings = NULL;
	m_pConnectionManager = NULL;
	m_pContentServer = NULL;
}

CContentManager::~CContentManager()
{
	Close();
}

void CContentManager::Open(IPathManager* pPathManager, IUserSettings* pUserSettings, IConnectionManager* pConnectionManager, IContentServer* pContentServer, LPCSTR pszClientSKU, LPCSTR pszClientVersion)
{
	// Set the path manager.
	m_pPathManager = pPathManager;
	m_pPathManager->AddRef();

	// Set the user settings.
	m_pUserSettings = pUserSettings;
	m_pUserSettings->AddRef();

	// Set the connection manager.
	m_pConnectionManager = pConnectionManager;
	if (m_pConnectionManager != NULL)
		m_pConnectionManager->AddRef();

	// Set the content server.
	m_pContentServer = pContentServer;
	if (m_pContentServer	!= NULL)
		m_pContentServer->AddRef();

	// Set the client parameters.
	if (pszClientSKU != NULL)
	{
		m_csClientSKU = pszClientSKU;
	}
	if (pszClientVersion != NULL)
	{
		m_csClientVersion = pszClientVersion;
	}

	// Get the list of data servers from the global INI file.
	CIniFile IniFile(m_pPathManager->ExpandPath("PMGS.INI"));
	CString csServerURLs = IniFile.GetString(SECTION_ServerAddress,
														  ENTRY_Address,
														  DEFAULT_SERVER_URL);
//														  "pmgs-data.mindscape.com:31597");

	// Read into an array.
	CStringArray csaServers;
	Util::FillArrayFromDelimitedString(csServerURLs, ";,", csaServers);

	// Then, shuffle into the main list.
	for (int n = 0; n < csaServers.GetSize(); n++)
	{
		CString csServer = csaServers.GetAt(n);
	   
		srand( (unsigned)time( NULL ) );
	
		if (rand() & 1)
		{
			// Odd: add at the tail of the list.
			m_cslDataServers.AddTail(csServer);
		}
		else
		{
			// Even: add at the head of the list.
			m_cslDataServers.AddHead(csServer);
		}
	}

	// Set the data server base path.
	m_csDataServerBasePath = IniFile.GetString(SECTION_ServerAddress,
														ENTRY_BasePath,
														DEFAULT_SERVER_BASE_PATH);

//	m_fNoAd = TRUE;
	m_fNoAd = FALSE;

	// Read the paths from the INI file.

	m_csCacheDirectory = m_pPathManager->ExpandPath(m_pUserSettings->GetString(SECTION_ServerAccount, ENTRY_ServerCacheDir, "Cache"));
	m_csCacheDirectory.MakeUpper();

	CUUEncoder UUEncoder;
	m_csAccountID = UUEncoder.DecodeString(m_pUserSettings->GetString(SECTION_ServerAccount, ENTRY_ServerAccountName, ""));
	m_csPassword = UUEncoder.DecodeString(m_pUserSettings->GetString(SECTION_ServerAccount, ENTRY_ServerPassword, ""));

	// No requests yet!
	m_nRequest = 0;
}

void CContentManager::Close(void)
{
	// Release the interfaces we hold.
	if (m_pConnectionManager != NULL)
	{
		m_pConnectionManager->Release();
		m_pConnectionManager = NULL;
	}
	if (m_pContentServer != NULL)
	{
		m_pContentServer->Release();
		m_pContentServer = NULL;
	}
	if (m_pPathManager != NULL)
	{
		m_pPathManager->Release();
		m_pPathManager = NULL;
	}
	if (m_pUserSettings != NULL)
	{
		m_pUserSettings->Release();
		m_pUserSettings = NULL;
	}
}

//
// Stuff parameters into a basic request.
//

void CContentManager::FillBasicRequest(CServerBasicRequest& Request, BOOL fBumpRequest /*=TRUE*/)
{
	// Connection info.
	// Default is to use the data server URLs (as opposed to the download ones)
	Request.m_ConnectionInfo.m_pServerURLs = &m_cslDataServers;
	Request.m_ConnectionInfo.m_csBasePath = m_csDataServerBasePath;

	// Basic request info.
	Request.m_csAccountID = m_csAccountID;
	Request.m_csPassword = m_csPassword;
	Request.m_csSessionID = m_csSessionID;
	Request.m_csRequestID.Format("%d", m_nRequest);
	Request.m_csClientSKU = m_csClientSKU;
	CString strClientVersion = m_csClientVersion;
	//do not include the build number
	int num = m_csClientVersion.ReverseFind('.');
	Request.m_csClientVersion = strClientVersion.Left(num);
	Request.m_csClientPGSVersion = "1.0";
	// Generalize this for localisation someday.
	Request.m_csClientLocale = "us-english";

	if (fBumpRequest)
	{
		m_nRequest++;
	}
}

//
// Check whether a request completed or not.
// Throws the appropriate exception in the event of an error.
//

void CContentManager::CheckRequestCompletion(const CServerBasicRequest& Request)
{
#ifndef	ARTSTORE
	if (Request.m_ConnectionInfo.m_dwResult != 0)
	{
		THROW(new CContentCommunicationException(Request.m_ConnectionInfo.m_dwResult));
	}
	if (Request.m_ConnectionInfo.m_dwServerStatusCode != HTTP_STATUS_OK)
	{
		THROW(new CContentServerException(Request.m_ConnectionInfo.m_dwServerStatusCode));
	}
#endif
	// Otherwise, we are all OK!
}

// Check a request completion; issues standard message boxes for errors.
int CContentManager::StandardCheckRequestCompletion(const CServerBasicRequest& Request)
{
	TRY
	{
		// Check the results. Throws the appropriate exception in the event
		// of serious failure.
		CheckRequestCompletion(Request);
	}
	CATCH(CContentCommunicationException, e)
	{
		// Communication error
		// Dummy message for now. Change this!
		AfxMessageBox("A connection with the server could not be established. Please try again later.");
		ASSERT(Request.m_nStatus == CSSTATUS_CommunicationException);
	}
	AND_CATCH(CContentServerException, e)
	{
		// Server error.
		// The following map uses the actual error number (not the #define
		// in wininet.h) because the connection is clearer and unbreakable.
		static struct
		{
			int m_nError;
			UINT m_uMessage;
		} ErrorMap[] =
		{
			{ 300, IDS_HTTP300 },
			{ 301, IDS_HTTP301 },
			{ 302, IDS_HTTP302 },
			{ 303, IDS_HTTP303 },
			{ 304, IDS_HTTP304 },
			{ 400, IDS_HTTP400 },
			{ 401, IDS_HTTP401 },
			{ 402, IDS_HTTP402 },
			{ 403, IDS_HTTP403 },
			{ 404, IDS_HTTP404 },
			{ 405, IDS_HTTP405 },
			{ 406, IDS_HTTP406 },
			{ 407, IDS_HTTP407 },
			{ 408, IDS_HTTP408 },
			{ 409, IDS_HTTP409 },
			{ 410, IDS_HTTP410 },
			{ 411, IDS_HTTP411 },
			{ 500, IDS_HTTP500 },
			{ 501, IDS_HTTP501 },
			{ 502, IDS_HTTP502 },
			{ 503, IDS_HTTP503 },
			{ 504, IDS_HTTP504 },
			{ 0, 0}
		};

		UINT uMessage = IDS_HTTPUnknown;
		for (int i = 0; ErrorMap[i].m_nError != 0; i++)
		{
			if (ErrorMap[i].m_nError == (signed)e->m_dwError)
			{
				uMessage = ErrorMap[i].m_uMessage;
				break;
			}
		}

		// Load the format.
		CString csFormat;
		csFormat.LoadString(uMessage);

		// Create the error message.
		CString csMessage;
		csMessage.Format(csFormat, e->m_dwError);

		// Show the error message.
		AfxMessageBox(csMessage);

		ASSERT(Request.m_nStatus == CSSTATUS_ServerException);
	}
	AND_CATCH_ALL(e)
	{
		ASSERT(FALSE);
	}
	END_CATCH_ALL

	return Request.m_nStatus;
}

BOOL CContentManager::IsFileLocallyCached(LPCSTR strColl, LPCSTR strFileName)
{
	return Util::FileExists(MakeCachePath(strColl, strFileName));
}

UINT CContentManager::GetInternetContent(LPCSTR pszCollection, LPCSTR pszFileName, CString& csOutBuffer)
{
	UINT nRet = 1;
	UINT nGrRet = CSSTATUS_Success;
	BOOL fNonCollectionFile = FALSE;

	// Assign to local CString so it can be modified.
	if (pszCollection[0] == '!')
	{
		// Skip past the leading '!'.
		pszCollection++;
		fNonCollectionFile = TRUE;
	}

	// Compute where the file should live.
	csOutBuffer = MakeCachePath(pszCollection, pszFileName);

	if(!IsFileLocallyCached(pszCollection, pszFileName))
	{
		if(!((CMainFrame*)AfxGetMainWnd())->GetAppClosingDown())
		{
			// Lock the temp maps since we may be coming up at any time.
			AfxLockTempMaps();
			if (CheckAccountStatus()) //success
			{
				if (CheckForValidConnection())
				{
					//we should have a valid existing account now

					CServerDownloadInfo DownloadInfo;

					//the server is NOT referring to the files with their extensions
					CString strNormalizedID = pszFileName;
					Util::NormalizeContentServerID(strNormalizedID);

					if (m_csSessionID.IsEmpty())
					{
						if (fNonCollectionFile)
						{
							pszCollection = "";
							strNormalizedID = "";
						}

						CServerAdInfo AdInfo;
						nRet = Login(pszCollection, strNormalizedID, &DownloadInfo, &AdInfo);

						if (fNonCollectionFile)
						{
							DownloadInfo.m_csFile = pszFileName;
						}

						if((nRet == CSSTATUS_Success)
							|| ((nRet == CSSTATUS_UnknownItemID) && (fNonCollectionFile)))
						{
							if(!m_fNoAd && !AdInfo.m_csFileName.IsEmpty())
							{
								ShowAdvertisement(AdInfo);
							}

							nGrRet = GetGraphic(DownloadInfo, csOutBuffer);
						}
						else if(nRet == CSSTATUS_UseAlternateServer)//change this message?!!!!
						{
							AfxMessageBox(IDS_PGS_SERVICE_ERROR);
						}
					}
					else
					{
						if(!fNonCollectionFile)
						{
							CServerItemInfo ItemInfo;
							ItemInfo.m_csItemCollection = pszCollection;
							ItemInfo.m_csItemID = strNormalizedID;

							nRet = GetGraphicInfo(ItemInfo, &DownloadInfo);
						}
						else
						{
						  	DownloadInfo.m_csFile = pszFileName;
							nRet = CSSTATUS_Success;
						}

						if(nRet == CSSTATUS_Success)
							nGrRet = GetGraphic(DownloadInfo, csOutBuffer);
					}
				
					if(nGrRet == 2)
						AfxMessageBox(IDS_PGS_UNABLE_TO_DOWNLOAD);
	
				}
			}
			else
			{
				nRet = USER_ESCAPE;  //account cancellation
			}

			AfxUnlockTempMaps(FALSE);
		}
	}
	else
	{
		nRet = CSSTATUS_Success;   //success
	}

	return nRet;
}

BOOL CContentManager::CheckAccountStatus()
{
	if (!HaveAccountID())
	{
		// We need to create an account or find an existing one.
		if(QueryNewAccount()) //success
			return HaveAccountID();
	}
	return TRUE;
}

BOOL CContentManager::QueryNewAccount()
{
	BOOL fResult = FALSE;

	CAccountAccessDlg Dlg;

	Dlg.m_fNew = TRUE;

	if (Dlg.DoModal() == IDOK)
	{
		if (Dlg.m_fNew)
			fResult = CreateNewAccount();
		else
		{
			fResult = RequestExistingAccount();
		}
	}

	return fResult;
}

BOOL CContentManager::DoAccountInfoDlg(CServerAccountInfo& AccountInfo, CString& csAccountID, CString& csPassword)
{
	BOOL fResult = FALSE;

	CCreatePGSAccount pDlg;

	pDlg.m_strAccountID = csAccountID;
	pDlg.m_strEMail = AccountInfo.m_csEMailAddr;
	pDlg.m_strPassword = csPassword;
	pDlg.m_strVerifyPassword = csPassword;

	if (pDlg.DoModal() == IDOK)
	{
		csAccountID = pDlg.m_strAccountID;
		AccountInfo.m_csEMailAddr = pDlg.m_strEMail;
		csPassword = pDlg.m_strPassword;
		ASSERT(pDlg.m_strPassword == pDlg.m_strVerifyPassword);

		fResult = TRUE;
	}

	return fResult;
}

//
// All the user to update the account info on the server.
//

BOOL CContentManager::UpdateAccount(void)
{
	BOOL fResult = FALSE;

	if (CheckAccountStatus()) //success
	{
		if (CheckForValidConnection())
		{
			if (CheckIfSessionActive())
			{
				// Get the current account info.
				CServerAccountInfo AccountInfo;
				if (GetAccountInfo(AccountInfo))
				{
					// Show the info to the user so it can be changed.
					CServerAccountDlg pDlg(CServerAccountDlg::UpdateAcct);

					pDlg.m_strAccountID = m_csAccountID;
					pDlg.m_strPassword = m_csPassword;
					pDlg.m_strVerifyPassword = m_csPassword;
					pDlg.FromAccountInfo(AccountInfo);

					if (pDlg.DoModal() == IDOK)
					{
						// Write the info back to the server.
						pDlg.ToAccountInfo(AccountInfo);

						if (SendAccountInfo(AccountInfo, pDlg.m_strPassword))
						{
							// We need to create a new CString to pass in (via LPCSTR pointer).
							CString csAccountID = GetAccountID();
							SetLoginInfo(csAccountID, pDlg.m_strPassword);

							fResult = TRUE;
						}
					}
					else
					{
						// This is OK.
						fResult = TRUE;
					}
				}
			}
		}
	}

	return fResult;
}

BOOL CContentManager::CreateNewAccount()
{
	BOOL fSuccess = FALSE;

	CString csNewAccountID;	// =""
	CString csNewPassword;	// =""
	CServerAccountInfo AccountInfo;

	BOOL fTryAgain, fDlgReturn;

	do
	{
		fTryAgain = FALSE;

		fDlgReturn = DoAccountInfoDlg(AccountInfo, csNewAccountID, csNewPassword);
		
		if (fDlgReturn == TRUE)
		{
			if (CheckForValidConnection())
			{
				if (EstablishAccount(AccountInfo, csNewAccountID, csNewPassword) == CSSTATUS_LoginInUse)
				{
					fTryAgain = TRUE;
				}
				else
				{
					fTryAgain = FALSE;
					fSuccess = FALSE;
				}

			}
		}

	} while (fTryAgain);

	// Update licensed collections.
	if (fDlgReturn == TRUE)
		UpdateLicensedCollections(TRUE);

	return fSuccess;
}

BOOL CContentManager::RequestExistingAccount()
{
	BOOL fResult = FALSE;

	// Ask the user for the account name and password.
	CServerAccountDlg pDlg(CServerAccountDlg::ExistingAcct);

	int nRet = 0;
	if (pDlg.DoModal() == IDOK)
	{
		if (CheckForValidConnection())
		{
			// Attempt to login with this account and password.
			if (nRet = Login(NULL, NULL, NULL, NULL, pDlg.m_strAccountID, pDlg.m_strPassword) == CSSTATUS_Success)
			{
				// The login information is good. Remember it.
				SetLoginInfo(pDlg.m_strAccountID, pDlg.m_strPassword);

				// We have now logged in, and the session is established.

				// Update licensed collections.
				UpdateLicensedCollections(TRUE);

				// Go home happy.
					fResult = TRUE;
			}
			else //(nRet == CSSTATUS_InvalidLoginOrPassword)
			{
				AfxMessageBox(IDS_PGS_INVALID_ACCOUNT_CHOSEN);
			}
		}
	}

	return fResult;
}

BOOL CContentManager::CheckForValidConnection(void)
{
	if (m_pUserSettings->GetInteger("Online", "SkipConnectionCheck", FALSE))
	{
		return TRUE;
	}
	else
	{
		// Try to establish connection
		return (m_pConnectionManager != NULL && m_pConnectionManager->Connect() == CInternetConnectStatus::statusConnected);
	}
}

BOOL CContentManager::CheckIfSessionActive(void)
{
	if (!HaveSessionID())
	{
		Login();
	}
	return HaveSessionID();
}

int CContentManager::Login(LPCSTR pszCollection /*=""*/, LPCSTR pszID /*=""*/, CServerDownloadInfo* pDownloadInfo /*=NULL*/, CServerAdInfo* pAdInfo /*=NULL*/, LPCSTR pszAccountID /*=NULL*/, LPCSTR pszPassword /*=NULL*/)
{
	// Setup a login request.
	CServerLogin2Request Request;

	// Fill the basic parameters.
	FillBasicRequest(Request);

	// If we have an overriding account and password, use them.
	if (pszAccountID != NULL)
	{
		ASSERT(pszPassword != NULL);		// If one, then both.
		Request.m_csAccountID = pszAccountID;
		Request.m_csPassword = pszPassword;
	}

	// The session ID and request ID are 0 for a login.
	Request.m_csSessionID = "0";
	Request.m_csRequestID = "0";

	// Set the authentication information needed for a login.
	Request.m_ConnectionInfo.m_csAuthenticationLogin = "SGMP";
	Request.m_ConnectionInfo.m_csAuthenticationPassword = "saviM2Go";

	// Set the item to lookup. These may be empty.
	Request.m_ItemInfo.m_csItemCollection = pszCollection;
	Request.m_ItemInfo.m_csItemID = pszID;

	// Do the login call.
	if (m_pContentServer	!= NULL)
		m_pContentServer->Login2(Request);

	// Now, see how the server responded.
	int nStatus;
	switch (nStatus = StandardCheckRequestCompletion(Request))
	{
		case CSSTATUS_UseAlternateServer:
		{
			// We need to try another server.
			break;
		}
		case CSSTATUS_UnknownItemID:
		{
			nStatus = CSSTATUS_Success;
			// Fall through to...
		}
		case CSSTATUS_Success:
		{
			// We have logged in!
			// Set the new session ID.
			m_csSessionID = Request.m_csNewSessionID;

			// Add the data and download servers to our lists.
			AddServers(Request.m_cslDataServers, m_cslDataServers);
			AddServers(Request.m_cslDownloadServers, m_cslDownloadServers);

			// Return the download info if desired.
			if (pDownloadInfo != NULL)
			{
				*pDownloadInfo = Request.m_DownloadInfo;
			}

			// Return the ad info if desired.
			if (pAdInfo != NULL)
			{
				*pAdInfo = Request.m_AdInfo;
			}

			// Global collection revision.
			m_csCollectionRevision = Request.m_csGlobalRevisionID;

			// User collection revision.
			m_csUserCollectionRevision = Request.m_csRevisionID;

			// Update known collections.
			UpdateCollections();

			// Update licensed collections.
			UpdateLicensedCollections(FALSE);

			break;
		}
		case CSSTATUS_InvalidLoginOrPassword:
		{
			nStatus = CSSTATUS_InvalidLoginOrPassword;
			break;
		}
		default:
		{
			// Some other sort of error.
			StandardProcessResponseStatus(nStatus, Request.m_csStatus);
			break;
		}
	}

	return nStatus;
}

//
// Log out. This just resets the "logged in" status.
//

void CContentManager::Logout(void)
{
	m_csSessionID.Empty();
}

//
// Add servers to a list, preventing duplicates.
// New servers are added at the head of the list.
//

void CContentManager::AddServers(const CStringList& cslNewServers, CStringList& cslServers)
{
	// Loop through the new servers and add them to the existing ones.
	// Duplicate servers are weeded out. New servers are added at the head
	// (which is why we loop from tail to head).
	for (POSITION pos = cslNewServers.GetTailPosition(); pos != NULL;)
	{
		CString csServer = cslNewServers.GetPrev(pos);
		if (cslServers.Find(csServer) == NULL)
		{
			// Not currently in the list. Add it.
			cslServers.AddHead(csServer);
		}
	}
}

//
// Set new login info.
// This writes it to the user settings as well.
//

void CContentManager::SetLoginInfo(LPCSTR pszAccountID, LPCSTR pszPassword)
{
	// Set our local variables for the next login.
	m_csAccountID = pszAccountID;
	m_csPassword = pszPassword;

	// Write the new info to the user settings.
	CUUEncoder UUEncoder;
	m_pUserSettings->WriteString(SECTION_ServerAccount,
										  ENTRY_ServerAccountName,
										  UUEncoder.EncodeString(m_csAccountID));
	m_pUserSettings->WriteString(SECTION_ServerAccount,
										  ENTRY_ServerPassword,
										  UUEncoder.EncodeString(m_csPassword));
}

void CContentManager::StandardProcessResponseStatus(int nStatus, LPCSTR pszStatus)
{
	switch (nStatus)
	{
		case CSSTATUS_Success:
			//MessageBox(NULL, "Operation Completed Successfully", "Operation Status", MB_OK);
			break;
		case CSSTATUS_IllFormedRequest:
			AfxMessageBox(IDS_PGS_ILLFORMED_REQUEST);
			break;
		case CSSTATUS_ServiceError:
			AfxMessageBox(IDS_PGS_SERVICE_ERROR);
			break;
		case CSSTATUS_ProtocolVersionNotSupported:
			AfxMessageBox(IDS_PGS_PROTOCOL_NOT_SUPPORTED);
			break;
		case CSSTATUS_ClientVersionNotSupported:
			AfxMessageBox(IDS_PGS_CLIENT_VERSION_NOT_SUPPORTED);
			break;
		case CSSTATUS_InvalidLoginOrPassword:
			AfxMessageBox(IDS_PGS_INVALID_LOGIN);
			break;
		case CSSTATUS_UseAlternateServer:
			//MessageBox(NULL, "The current server is busy, trying another", "Operation Status", MB_OK);
			break;
		case CSSTATUS_UnknownAdID:
			AfxMessageBox(IDS_PGS_UNKNOWN_AD_ID);
			break;
		case CSSTATUS_NoAdsMatchingCriteria:
			AfxMessageBox(IDS_PGS_NOADS_MATCHING);
			break;
		case CSSTATUS_UnknownCollectionID:
			AfxMessageBox(IDS_PGS_UNKNOWN_COLLECTION_ID);
			break;
		case CSSTATUS_NoCollectionsMatchingCriteria:
#ifdef	_DEBUG
			// JRO This was remarked out...
			// Should there ever be a case where the user clicks, and NOTHING happens?
			AfxMessageBox(IDS_PGS_NOCOLLECTION_MATCHING);
#endif
			break;
		case CSSTATUS_UnknownCDID:
			AfxMessageBox(IDS_PGS_UNKNOWN_CD_ID);
			break;
		case CSSTATUS_NoCDsMatchingCriteria:
			AfxMessageBox(IDS_PGS_NOCDS_MATCHING);
			break;
		case CSSTATUS_UnknownItemID:
			AfxMessageBox(IDS_PGS_UNKNOWN_ITEM);
			break;
		case CSSTATUS_ItemNotLicensed:
			AfxMessageBox(IDS_PGS_ITEM_UNLICENSED);
			break;
		case CSSTATUS_LoginInUse:
			//AfxMessageBox(IDS_PGS_LOGINID_IN_USE);
			break;
//		case CSSTATUS_CreditCardAuthFailed:
		case CSSTATUS_PurchaseInProgress:
			AfxMessageBox(IDS_PGS_PURCHASE_INPROGRESS);
			break;
		case CSSTATUS_AddressIncorrect:
		{
			AfxMessageBox(IDS_PGS_ADDRESS_INCORRECT);
			break;
		}
		case CSSTATUS_InvalidStateCode:
		{
			AfxMessageBox(IDS_PGS_INVALID_STATE);
			break;
		}
		case CSSTATUS_InvalidCountryCode:
		{
			AfxMessageBox(IDS_PGS_INVALID_COUNTRY);
			break;
		}
		case CSSTATUS_InvalidCreditCardNumber:
		{
			AfxMessageBox(IDS_PGS_INVALID_CREDIT_CARD);
			break;
		}
		case CSSTATUS_InvalidExpirationDateFormat:
		{
			AfxMessageBox(IDS_PGS_INVALID_EXP_DATE_FORMAT);
			break;
		}
		case CSSTATUS_InvalidExpirationDate:
		{
			AfxMessageBox(IDS_PGS_INVALID_EXP_DATE);
			break;
		}
		case CSSTATUS_PaymentServerNoResponse:
		{
			AfxMessageBox(IDS_PGS_SERVER_NO_RESPONSE);
			break;
		}
		case CSSTATUS_CreditCardRejected:
		{
			AfxMessageBox(IDS_PGS_CREDITCARD_AUTHORIZATION_FAILED);
			break;
		}
		case CSSTATUS_PaymentServerError:
		{
			AfxMessageBox(IDS_PGS_SERVER_NO_RESPONSE);
			break;
		}
		default:
		{
			// We do not know what this code is.
			// Put up the message part of the status return.
			// Normal status messages look like "n message", where 'n' is the
			// status number, and 'message' is the corresponding error message.

			CString csStatus = pszStatus;
			int nIndex = csStatus.Find(' ');
			if (nIndex >= 0)
			{
				// We have the break in the message
				CString csMessage = csStatus.Mid(nIndex+1);
				Util::Trim(csMessage);
				if (!csMessage.IsEmpty())
				{
					AfxMessageBox(csMessage);
				}
			}
			break;
		}
	}
}

int CContentManager::GetGraphicInfo(const CServerItemInfo& ItemInfo, CServerDownloadInfo* pDownloadInfo)
{
	CServerGetGraphicInfo2Request Request;

	// Set the basic parameters.
	FillBasicRequest(Request);

	// Set the graphic item parameter.
	Request.m_ItemInfo = ItemInfo;

	// Get the account info.
	if (m_pContentServer	!= NULL)
		m_pContentServer->GetGraphicInfo2(Request);

	// Now, see how the server responded.
	int nStatus;
	switch (nStatus = StandardCheckRequestCompletion(Request))
	{
		case CSSTATUS_Success:
		{
			*pDownloadInfo = Request.m_DownloadInfo;
			break;
		}
		default:
		{
			// Some other sort of error.
			StandardProcessResponseStatus(Request.m_nStatus, Request.m_csStatus);
			break;
		}
	}

	return nStatus;
}

int CContentManager::GetGraphic(const CServerDownloadInfo& DownloadInfo, LPCSTR pszDestinationName)
{
	CDownloadStatusDialog Dialog;
	Dialog.Create(CDownloadStatusDialog::IDD, CWnd::GetSafeOwner(NULL));
	Dialog.Show();

	// Create our callback objects.
	CContentManagerConnectionCallback ConnectionCallback(&Dialog);
	CContentManagerDownloadCallback DownloadCallback(&Dialog);

	// Do the download.
	int nStatus = DownloadFile(DownloadInfo, &ConnectionCallback, &DownloadCallback);

	// See how we did.
	if (nStatus == CSSTATUS_Success)
	{
		LPBYTE pBuffer = DownloadCallback.GetDownloadedData();

		// We have the data. Decompress it and write it to the file.
		CCompressInfo* pCompressInfo = (CCompressInfo*)pBuffer;
		if (!CheckForBadHeader(pCompressInfo))
		{
			// We have a good compression header.
			// Allocate memory for the decompression.
			LPBYTE pUncompressedData = NULL;
			DWORD dwUncompressedSize = pCompressInfo->GetUnCompressedSize();
			TRY
				pUncompressedData = new BYTE[dwUncompressedSize];
			END_TRY

			if (pUncompressedData == NULL)
			{
				// Could not allocate memory for the decompression.
				nStatus = CSSTATUS_MemoryError;
			}
			else
			{
				// Decompress the data.
				DecompressData(pCompressInfo, pBuffer+sizeof(CCompressInfo), pUncompressedData);

				// Make sure the directory exists.
				CString csPath;
				Util::SplitPath(pszDestinationName, &csPath, NULL);
				Util::MakeDirectory(csPath);

				//default is to overwrite everything!
				if(Util::FileExists(pszDestinationName))
				{
									
					CFile::Remove(pszDestinationName);
				}

				// Write the data out to the file.
				StorageFile File(pszDestinationName);
				if (File.initialize() == ERRORCODE_None
					 && File.seek(0, ST_DEV_SEEK_SET) == ERRORCODE_None
					 && File.huge_write(pUncompressedData, dwUncompressedSize) == ERRORCODE_None)
				{
					// We are happy!
				}
				else
				{
					nStatus = CSSTATUS_WriteError;
				}

				delete [] pUncompressedData;
			}
		}
	}
	Dialog.Hide();
	return nStatus;
}

//
// Download a file.
//

int CContentManager::DownloadFile(const CServerDownloadInfo& DownloadInfo, IServerConnectionCallback* pConnectionCallback, IServerDownloadCallback* pDownloadCallback)
{
	CServerGetGraphicRequest Request;

	// Fill out the list of servers for the download request.
	Request.m_ConnectionInfo.m_pServerURLs = &m_cslDownloadServers;

	// Set the download info.
	Request.m_DownloadInfo = DownloadInfo;

	// Fill out the callbacks.
	Request.m_ConnectionInfo.m_pConnectionCallback = pConnectionCallback;
	Request.m_pDownloadCallback = pDownloadCallback;

	// Make the request.
	if (m_pContentServer	!= NULL)
		m_pContentServer->GetGraphic(Request);

	// See how we did.
	return StandardCheckRequestCompletion(Request);
}

BOOL CContentManager::CheckForBadHeader(CCompressInfo* pCompressInfo)
{
	BOOL fRet = FALSE;

	unsigned short uType = pCompressInfo->GetType();

	if ((uType != CThumbnailDataHeader::COMPRESSION_ReducedDeflate) &&
		 (uType != CThumbnailDataHeader::COMPRESSION_Deflate) &&
		 (uType != CThumbnailDataHeader::COMPRESSION_None) &&
		 (uType != CThumbnailDataHeader::COMPRESSION_JPEG))
	{
		 fRet = TRUE;
	}

	if (pCompressInfo->GetCompressedSize() <= 0
		|| pCompressInfo->GetUnCompressedSize() <= 0)
	{
		fRet = TRUE;
	}

	return fRet;
}

//
// Decompress downloaded data.
//

BOOL CContentManager::DecompressData(CCompressInfo* pCompressInfo, LPBYTE pInBuffer, LPBYTE pOutBuffer)
{
   if (pCompressInfo != NULL)
   {
		switch (pCompressInfo->GetType())
		{
			case CThumbnailDataHeader::COMPRESSION_ReducedDeflate:
			{
				//fReduced24To8Bit = TRUE;
				// Fall through to...
			}
			case CThumbnailDataHeader::COMPRESSION_Deflate:
			{
				// Decompress the data.
				LPBYTE pNewData = NULL;
				DWORD dwNewData = 0;

				CDeflate Deflator;

				Deflator.Decompress(pInBuffer,
										  pCompressInfo->GetCompressedSize(),
										  &pNewData,
										  &dwNewData,
										  pOutBuffer,
										  pCompressInfo->GetUnCompressedSize());
				ASSERT(pNewData == pOutBuffer);
				break;
			}
			default:
			{
				// Just copy the data.
				ASSERT(pCompressInfo->GetType() == CThumbnailDataHeader::COMPRESSION_None);
				ASSERT(pCompressInfo->GetUnCompressedSize() == pCompressInfo->GetCompressedSize());
				memcpy(pOutBuffer, pInBuffer, pCompressInfo->GetUnCompressedSize());
				break;
			}
		}
	}

   return TRUE;
}

void CContentManager::ParseStringToLocationAndPassword(CString& strString, CString& strLocation, CString& strPassword)
{
	char seps [] = "\t";
	char* pstr = strString.GetBuffer(strString.GetLength());
	char* token = strtok( pstr, seps );

	while ( token != NULL )
	{
		/* While there are tokens in "string" */
		strLocation = token;
		/* Get next token: */
		if ((token = strtok( NULL, seps )) != NULL)
		{
			strPassword = token;
		}
	}  
	strString.ReleaseBuffer();
}


CString CContentManager::MakeCachePath(LPCSTR strColl, LPCSTR strFileName)
{
	CString strPath;

	strPath = GetCacheDirectory();
	strPath += "\\";
	strPath += strColl;
	if(lstrcmp(strColl, "") != 0)
	  strPath += "\\";
	strPath += strFileName;

	//search for backwards backslashes and correct them
	UINT index;
	while ((index = strPath.Find("/")) != -1)
	{
		strPath.SetAt(index, '\\');
	}
	//end backslashes

	return strPath;
}

BOOL CContentManager::DeleteCachedDirectories(CWnd* pParent, LPCSTR pszCollections)
{
	CString strPath;

	strPath = GetCacheDirectory();
	
	if(pszCollections != NULL)
	{
		strPath += "\\";
		strPath += pszCollections;
	}

	DeleteDirectory(strPath);
	
	//repaint the browser window
	if(pParent)
		pParent->InvalidateRect(NULL, FALSE);

	return TRUE;
}

BOOL CContentManager::DeleteDirectory(LPCSTR pDirectory)
{
	//SetItem(pDirectory);

	if(Util::DirectoryExists(pDirectory))
	{
		CString csProgramFile;
		CStringArray csaSkipFiles;

		csaSkipFiles.RemoveAll();

		CString csMessage;
		CString csDrive;

		if ((!Util::DirectoryDrive(pDirectory, csDrive))
		 || (csDrive == pDirectory)
		 || ((csDrive + '\\') == pDirectory))
		{
			TRY
			{
				//csMessage.LoadString(IDS_BAD_DIRECTORY);
				csMessage = "Bad Directory";
			}
			END_TRY

			Util::MessageBox(MB_OK | MB_ICONSTOP, (UINT)-1, csMessage, pDirectory);
			return FALSE;
		}

		CFileIterator It;

		CString csSearchPath;
		// Do we use "*.*" under NT or "*"?

		Util::ConstructPath(csSearchPath, pDirectory, "*.*");

		for (BOOL fRet = It.FindFirst(csSearchPath, _A_NORMAL | _A_SUBDIR | _A_HIDDEN); fRet; fRet = It.FindNext())
		{
			CString csPath;
			Util::ConstructPath(csPath, pDirectory, It.Name());
	//		TRACE("Found file %s (%08lx)\n", (LPCSTR)It.Name(), It.Attributes());

			if (It.Attributes() & _A_SUBDIR)
			{
				if ((strcmp(It.Name(), ".") != 0)
				 && (strcmp(It.Name(), "..") != 0))
				{
					// This is a subdirectory.
					if (!DeleteDirectory(csPath))
					{
						return FALSE;
					}
				}
			}
			else
			{
				// This is a file.
				BOOL fSkipFile = FALSE;

				for (int nSkipFile = 0; nSkipFile < csaSkipFiles.GetSize(); nSkipFile++)
				{
					if (csaSkipFiles[nSkipFile].CompareNoCase(csPath) == 0)
					{
						fSkipFile = TRUE;
						break;
					}
				}

				if (!fSkipFile)
				{
					TRY
					{
						CFileStatus FileStatus;
						if (CFile::GetStatus(csPath, FileStatus))
						{
							if ((FileStatus.m_attribute & CFile::readOnly) != 0)
							{
								TRACE("Make file %s writable\n", (LPCSTR)csPath);
								FileStatus.m_attribute &= ~CFile::readOnly;
								CFile::SetStatus(csPath, FileStatus);
							}
						}
					}
					END_TRY

					TRY
					{
						TRACE("Delete file %s\n", (LPCSTR)csPath);
						CFile::Remove(csPath);
					}
					CATCH_ALL(e)
					{
						TRY
						{
							//csMessage.LoadString(IDS_CANT_REMOVE_FILE);
							csMessage = "Can't remove file";

						}
						END_TRY

						Util::MessageBox(MB_OK | MB_ICONEXCLAMATION, (UINT)-1, csMessage, (LPCSTR)csPath);
						return FALSE;
					}
					END_CATCH_ALL
				}
			}
		}

		It.Close();

	// NT won't delete a directory if someone's using it.
		CString csRootDirectory = csDrive;
		Util::AppendBackslashToPath(csRootDirectory);
	#ifdef WIN32
		SetCurrentDirectory(csRootDirectory);
	#else
		chdir(csRootDirectory);
	#endif

	// And finally, remove the directory itself.
		CString csDirectory = pDirectory;
		Util::RemoveBackslashFromPath(csDirectory);

		TRACE("Delete directory %s\n", (LPCSTR)csDirectory);
	 #ifdef WIN32
		if (RemoveDirectory(csDirectory))
	 #else
		if (rmdir(csDirectory) == 0)
	 #endif
		{
			return TRUE;
		}

		TRY
		{
			//csMessage.LoadString(IDS_CANT_REMOVE_DIRECTORY);
			csMessage = "Can't Remove Directory";

		}
		END_TRY
		Util::MessageBox(MB_OK | MB_ICONEXCLAMATION, (UINT)-1, csMessage, (LPCSTR)csDirectory,
	#ifdef WIN32
						  GetLastError());
	#else
						  errno);
	#endif
	}

	return FALSE;
}

UINT CContentManager::ShowAdvertisement(const CServerAdInfo& AdInfo)
{
	//Get the bitmap for the ad

	CString strCachePath = MakeCachePath("", AdInfo.m_csFileName);

	CServerDownloadInfo DownloadInfo;
	DownloadInfo.m_csFile = AdInfo.m_csFileName;
	if(GetGraphic(DownloadInfo, strCachePath) == 0) //success
	{
		COnlineAdvertisementDlg Dlg;

		Dlg.m_strAdText = AdInfo.m_csDescription;
		
		Dlg.m_strbm = strCachePath;
		
		CString csUrl;
		csUrl = AdInfo.m_csURLLink;
		
		if(csUrl != "")
			Dlg.m_fURLLink = TRUE;
		else
			Dlg.m_fURLLink = FALSE;

		//THE FOLLOWING NEEDS TO BE TESTED!
		
		if(AdInfo.m_csURLLink.Find("http") != -1)
		{
			if(Dlg.DoModal() == IDOK)
			{
				if (DoHTML (NULL, csUrl, TRUE) == FALSE)
				{
					CString     csMessage, csResource;

					LoadConfigurationString(IDS_WEB_BROWSER_START_FAILED, csResource);
					csMessage.Format(csResource, csUrl);
					AfxMessageBox(csMessage);
				}
			}
		}
		else if(AdInfo.m_csURLLink.Find("artstore") == 0)
		{
			if(Dlg.DoModal() == IDOK)
			{
				CArtStoreDialog   storeDialog;
				storeDialog.DoModal();
			}
		}
		else
		{
			Dlg.DoModal();
		}
		
		MarkAdSeen(AdInfo.m_csAdID);

		TRY
			CFile::Remove(strCachePath);
		END_TRY
	}

	return 0;
}

int CContentManager::GetAdList(CServerAdInfoArray& AdArray, LPCSTR csSelector, LPCSTR csIgnoreLocale)
{

	CServerGetAdListRequest Request;

	// Set the basic parameters.
	FillBasicRequest(Request);

	// Set the graphic item parameter.
	Request.m_csListType = csSelector;
	Request.m_csIgnoreLocale = csIgnoreLocale;

	// Get the ad list.
	if (m_pContentServer	!= NULL)
		m_pContentServer->GetAdList(Request);

	// Now, see how the server responded.
	int nStatus;
	switch (nStatus = StandardCheckRequestCompletion(Request))
	{
		case CSSTATUS_Success:
		{
			AdArray.Copy(Request.m_Ads);
			break;
		}
		default:
		{
			// Some other sort of error.
			StandardProcessResponseStatus(Request.m_nStatus, Request.m_csStatus);
			break;
		}
	}

	return nStatus;

}

int CContentManager::GetAdInfo(CServerAdInfo& Info, LPCSTR csAdID)
{
	CServerGetAdInfoRequest Request;

	// Set the basic parameters.
	FillBasicRequest(Request);

	// Set the graphic item parameter.
	Request.m_csAdID = csAdID;

	// Get the ad list.
	if (m_pContentServer	!= NULL)
		m_pContentServer->GetAdInfo(Request);

	// Now, see how the server responded.
	int nStatus;
	switch (nStatus = StandardCheckRequestCompletion(Request))
	{
		case CSSTATUS_Success:
		{
			Info = Request.m_AdInfo;
			break;
		}
		default:
		{
			// Some other sort of error.
			StandardProcessResponseStatus(Request.m_nStatus, Request.m_csStatus);
			break;
		}
	}

	return nStatus;

}


int CContentManager::MarkAdSeen(LPCSTR csAdID)
{
	CServerMarkAdSeenRequest Request;

	// Set the basic parameters.
	FillBasicRequest(Request);

	// Set the graphic item parameter.
	Request.m_csAdID = csAdID;

	// Get the ad info.
	if (m_pContentServer	!= NULL)
		m_pContentServer->MarkAdSeen(Request);

	// Now, see how the server responded.
	int nStatus;
	switch (nStatus = StandardCheckRequestCompletion(Request))
	{
		case CSSTATUS_Success:
		{
			break;
		}
		default:
		{
			// Some other sort of error.
			StandardProcessResponseStatus(Request.m_nStatus, Request.m_csStatus);
			break;
		}
	}

	return nStatus;
}

int CContentManager::GetCollectionList(CServerCollectionInfoArray& CollectionArray, LPCSTR csSelector, LPCSTR csIgnoreLocale)
{
	CServerGetCollectionListRequest Request;

	// Set the basic parameters.
	FillBasicRequest(Request);

	// Set the graphic item parameter.
	Request.m_csListType = csSelector;
	Request.m_csIgnoreLocale = csIgnoreLocale;

	// Get the collection list.
	if (m_pContentServer	!= NULL)
		m_pContentServer->GetCollectionList(Request);

	// Now, see how the server responded.
	int nStatus;
	switch (nStatus = StandardCheckRequestCompletion(Request))
	{
		case CSSTATUS_Success:
		{
			CollectionArray.Copy(Request.m_Collections);
			break;
		}
		case CSSTATUS_NoCollectionsMatchingCriteria:
		{
			// This isn't an error. We just don't return any collections.
			nStatus = CSSTATUS_Success;
			break;
		}
		default:
		{
			// Some other sort of error.
			StandardProcessResponseStatus(Request.m_nStatus, Request.m_csStatus);
			break;
		}
	}

	return nStatus;
}

int CContentManager::GetCollectionInfo(CServerCollectionInfo& Info, LPCSTR csCollectionID)
{
	CServerGetCollectionInfoRequest Request;

	// Set the basic parameters.
	FillBasicRequest(Request);

	// Set the graphic item parameter.
	Request.m_csCollectionID = csCollectionID;

	// Get the collection item.
	if (m_pContentServer	!= NULL)
		m_pContentServer->GetCollectionInfo(Request);

	// Now, see how the server responded.
	int nStatus;
	switch (nStatus = StandardCheckRequestCompletion(Request))
	{
		case CSSTATUS_Success:
		{
			// Return the resulting info.
			Info = Request.m_CollectionInfo;
			break;
		}
		default:
		{
			// Some other sort of error.
			StandardProcessResponseStatus(Request.m_nStatus, Request.m_csStatus);
			break;
		}
	}

	return nStatus;
}

int CContentManager::MarkCollectionSeen(LPCSTR csCollID)
{
	CServerMarkCollectionSeenRequest Request;

	// Set the basic parameters.
	FillBasicRequest(Request);

	// Set the graphic item parameter.
	Request.m_csCollectionID = csCollID;

	// Get the collection info.
	if (m_pContentServer	!= NULL)
		m_pContentServer->MarkCollectionSeen(Request);

	// Now, see how the server responded.
	int nStatus;
	switch (nStatus = StandardCheckRequestCompletion(Request))
	{
		case CSSTATUS_Success:
		{
			break;
		}
		default:
		{
			// Some other sort of error.
			StandardProcessResponseStatus(Request.m_nStatus, Request.m_csStatus);
			break;
		}
	}

	return nStatus;
}

int CContentManager::GetCDList(CServerCDInfoArray& CDArray, LPCSTR csSelector, LPCSTR csIgnoreLocale)
{
	CServerGetCDListRequest Request;

	// Set the basic parameters.
	FillBasicRequest(Request);

	// Set the graphic item parameter.
	Request.m_csListType = csSelector;
	Request.m_csIgnoreLocale = csIgnoreLocale;

	// Get the CD list.
	if (m_pContentServer	!= NULL)
		m_pContentServer->GetCDList(Request);

	// Now, see how the server responded.
	int nStatus;
	switch (nStatus = StandardCheckRequestCompletion(Request))
	{
		case CSSTATUS_Success:
		{
			CDArray.Copy(Request.m_CDs);
			break;
		}
		default:
		{
			// Some other sort of error.
			StandardProcessResponseStatus(Request.m_nStatus, Request.m_csStatus);
			break;
		}
	}

	return nStatus;
}

int CContentManager::GetCDInfo(CServerCDInfo& CDInfo, LPCSTR csCDID)
{
	CServerGetCDInfoRequest Request;

	// Set the basic parameters.
	FillBasicRequest(Request);

	// Set the graphic item parameter.
	Request.m_csCDID = csCDID;

	// Get the CD item.
	if (m_pContentServer	!= NULL)
		m_pContentServer->GetCDInfo(Request);

	// Now, see how the server responded.
	int nStatus;
	switch (nStatus = StandardCheckRequestCompletion(Request))
	{
		case CSSTATUS_Success:
		{
			CDInfo = Request.m_CDInfo;
			break;
		}
		default:
		{
			// Some other sort of error.
			StandardProcessResponseStatus(Request.m_nStatus, Request.m_csStatus);
			break;
		}
	}

	return nStatus;
}

int CContentManager::GetCountryInfo(CServerCountryInfoArray& CountryArray)
{
	CServerGetCountryInfoRequest Request;

	// Set the basic parameters.
	FillBasicRequest(Request);

	// Get the country list.
	if (m_pContentServer	!= NULL)
		m_pContentServer->GetCountryInfo(Request);

	// Now, see how the server responded.
	int nStatus;
	switch (nStatus = StandardCheckRequestCompletion(Request))
	{
		case CSSTATUS_Success:
		{
			CountryArray.Copy(Request.m_Countries);
			break;
		}
		default:
		{
			// Some other sort of error.
			StandardProcessResponseStatus(Request.m_nStatus, Request.m_csStatus);
			break;
		}
	}

	return nStatus;
}

int CContentManager::GetShippingInfo(const CServerLocationInfo& LocationInfo, CServerShippingInfoArray& ShippingMethods)
{
	CServerGetShippingInfoRequest Request;

	// Set the basic parameters.
	FillBasicRequest(Request);

	// Set the location parameters.
	Request.m_LocationInfo = LocationInfo;

	// Get the shipping list.
	if (m_pContentServer	!= NULL)
		m_pContentServer->GetShippingInfo(Request);

	// Now, see how the server responded.
	int nStatus;
	switch (nStatus = StandardCheckRequestCompletion(Request))
	{
		case CSSTATUS_Success:
		{
			ShippingMethods.Copy(Request.m_ShippingMethods);
			break;
		}
		default:
		{
			// Some other sort of error.
			StandardProcessResponseStatus(Request.m_nStatus, Request.m_csStatus);
			break;
		}
	}

	return nStatus;
}

void CContentManager::UpdateCollectionsOrderFile(const CServerCollectionInfoArray& CollectionArray, LPCSTR pszRevision)
{
	CString csOrdFile;
	CString csMessage;
	CString csProducts;
	CString csSection;
	CString csValue;
	CIniFile OrdFile;

	TRY
   {
      // Create the data (.ord) file.
      csOrdFile = m_pPathManager->ExpandPath(OnlineOrderFile);
		// Make sure it does not exist now.
		if (Util::FileExists(csOrdFile))
		{
			// If this throws an exception, we just leave.
			CFile::Remove(csOrdFile);
		}

		// Set the ORD file name.
		OrdFile.Name(csOrdFile);

		// Write the "Online" section.
		OrdFile.WriteString(SECTION_Online, ENTRY_Revision, pszRevision);	// Set the revision.

		// Write the "Artstore" section.
		OrdFile.WriteString(SECTION_ArtStore, ENTRY_FileName, OnlineOrderFile);
		OrdFile.WriteString(SECTION_ArtStore, ENTRY_TabImageFileName, "Online.bmp");
		CString csOnline;
		csOnline.LoadString(IDS_OnlineTabName);
		OrdFile.WriteString(SECTION_ArtStore, ENTRY_ProductName, csOnline);
		OrdFile.WriteString(SECTION_ArtStore, ENTRY_SortOrder, "100");
		OrdFile.WriteInteger(SECTION_ArtStore, ENTRY_IsInternet, 1);	// Mark us as online.

		// Write the "Offer" section
		csProducts = "(ONLINE (";

		for (int n = 0; n < CollectionArray.GetSize(); n++)
		{
			CServerCollectionInfo CollInfo = CollectionArray.GetAt(n);
			if (!CollInfo.m_csSKU.IsEmpty())
			{
				CString csProduct;
#if 1
				csProduct.Format("Coll%d", n);
#else
				csProduct = CollInfo.m_csSKU;
#endif

				// Add this product to the products string.
				if(n > 0)
					csProducts += " ";

				csProducts += csProduct;

				// Now, process this section.
				csSection.Format("%s_ID", (LPCSTR)csProduct);

				OrdFile.WriteString(csSection, ENTRY_Code, CollInfo.m_csSKU);
				OrdFile.WriteString(csSection, ENTRY_Name, CollInfo.m_csDescription);
				// Format the price.
				double dPrice = atof(CollInfo.m_csPrice);
				csValue.Format("%ld", (long)(dPrice*100));
				OrdFile.WriteString(csSection, ENTRY_Price, csValue);

				OrdFile.WriteString(csSection, ENTRY_CollectionCode, "N/A");
				OrdFile.WriteString(csSection, ENTRY_CollectionName, "N/A");
				OrdFile.WriteString(csSection, ENTRY_CollectionBaseName, CollInfo.m_csName);
				OrdFile.WriteInteger(csSection, ENTRY_Type, 1);

				csValue.Format("[[I:!Brochures]]\\%s", (LPCSTR)CollInfo.m_csBrochureFileName);
				OrdFile.WriteString(csSection, ENTRY_Image, csValue);
			}
		}

		// Close off the products line.
		csProducts += "))";
		// Now, write the final products string.
		OrdFile.WriteString(SECTION_Offer, ENTRY_Products, csProducts);
	}
	CATCH_ALL(e)
   {
      csMessage.Format("Abnormal termination");
      AfxMessageBox(csMessage);
   }
   END_CATCH_ALL
}

//
//Establish a new account

int CContentManager::EstablishAccount(CServerAccountInfo& AccountInfo, CString& csNewAccountID, LPCSTR csNewPassword)
{
	BOOL fResult = FALSE;

	CServerEstablishAccountRequest Request;

	// Fill the basic parameters.
	FillBasicRequest(Request);

	// Set the new account information.
	Request.m_csAccountID = csNewAccountID;
	Request.m_csPassword = csNewPassword;
	Request.m_AccountInfo = AccountInfo;
	Request.m_csSessionID = "0";
	Request.m_csRequestID = "0";

	// Set the authentication information needed for a login.
	Request.m_ConnectionInfo.m_csAuthenticationLogin = "SGMP";
	Request.m_ConnectionInfo.m_csAuthenticationPassword = "saviM2Go";

	if (m_pContentServer	!= NULL)
		m_pContentServer->EstablishAccount(Request);

	// Now, see how the server responded.
	int iReturn = StandardCheckRequestCompletion(Request);
	switch (iReturn)
	{
		case CSSTATUS_LoginInUse:
		{
			// We need to try another name.
			CString strResult;
			CString strNotify;
			strNotify.LoadString(IDS_PGS_LOGINID_IN_USE);
			strResult.Format(strNotify,
								  csNewAccountID,
								  Request.m_csSuggestedAccountID);
			AfxMessageBox(strResult);

			// Set the suggested ID for the next go around.
			csNewAccountID = Request.m_csSuggestedAccountID;

			break;
		}
		case CSSTATUS_Success:
		{
			// We have a new account!
			SetLoginInfo(csNewAccountID, csNewPassword);

			CString strResult;
			CString strNotify;
			strNotify.LoadString(IDS_PGS_ACCOUNTID_NOTIFICATION);
			strResult.Format(strNotify, (LPCSTR)csNewAccountID);
			AfxMessageBox(strResult, MB_ICONINFORMATION);

			// Start a new session next time.
			m_csSessionID.Empty();

			break;
		}
		default:
		{
			// Some other sort of error.
			StandardProcessResponseStatus(Request.m_nStatus, Request.m_csStatus);
			break;
		}
	}

	return iReturn;
}




//
// Get the current account info.
//

BOOL CContentManager::GetAccountInfo(CServerAccountInfo& AccountInfo)
{
	BOOL fResult = FALSE;

	// Setup a request to get the account info.
	CServerGetAccountInfoRequest Request;

	// Set the basic parameters.
	FillBasicRequest(Request);

	// Get the account info.
	if (m_pContentServer	!= NULL)
		m_pContentServer->GetAccountInfo(Request);

	// Now, see how the server responded.
	switch (StandardCheckRequestCompletion(Request))
	{
		case CSSTATUS_Success:
		{
			AccountInfo = Request.m_AccountInfo;
			fResult = TRUE;
			break;
		}
		default:
		{
			// Some other sort of error.
			StandardProcessResponseStatus(Request.m_nStatus, Request.m_csStatus);
			break;
		}
	}

	return fResult;
}

//
// Send new account info to the server.
//

BOOL CContentManager::SendAccountInfo(const CServerAccountInfo& AccountInfo, LPCSTR pszNewPassword /*=NULL*/)
{
	BOOL fResult = FALSE;

	// A NULL password maps to the current password.
	if (pszNewPassword == NULL)
	{
		pszNewPassword = m_csPassword;
	}
	ASSERT(pszNewPassword[0] != '\0');

	// Setup a request to send the account info.
	CServerUpdateAccountInfoRequest Request;

	// Set the basic parameters.
	FillBasicRequest(Request);

	// Fill out the specific fields.
	Request.m_csNewPassword = pszNewPassword;
	Request.m_AccountInfo = AccountInfo;

	// Get the account info.
	if (m_pContentServer	!= NULL)
		m_pContentServer->UpdateAccountInfo(Request);

	// Now, see how the server responded.
	switch (StandardCheckRequestCompletion(Request))
	{
		case CSSTATUS_Success:
		{
			fResult = TRUE;
			break;
		}
		default:
		{
			// Some other sort of error.
			StandardProcessResponseStatus(Request.m_nStatus, Request.m_csStatus);
			break;
		}
	}

	return fResult;
}

int CContentManager::LicenseProducts(LPCSTR pszProducts /*=NULL*/, LPCSTR InitialSelection /*NULL*/)
{
	if (CheckAccountStatus())
	{
		if (CheckForValidConnection())
		{
			if (CheckIfSessionActive())
			{
				// Get the order form.
				CServerOrderFormInfo OrderFormInfo;
				if (GetOrderForm(OrderFormInfo) == CSSTATUS_Success)
				{
					// Get the current account info.
					CServerAccountInfo AccountInfo;
					if (GetAccountInfo(AccountInfo))
					{
						// Get the country list.
						CServerCountryInfoArray Countries;
						if (GetCountryInfo(Countries) == CSSTATUS_Success)
						{
							CPGSOrderWizard Wizard(&OrderFormInfo, AccountInfo, Countries,
														  (IContentManager*)GetInterface(&IID_IContentManager),
														  m_pPathManager,
														  InitialSelection);

							if (Wizard.DoModal() == ID_WIZFINISH)
							{
								// Got all the way to the end. Continue on with the order.
							}
						}
					}
				}
			}
		}
	}

	return 1;
}

int CContentManager::GetOrderForm(CServerOrderFormInfo& Info)
{
	// Create the request. The info we point to is the one passed in.
	CServerGetOrderFormRequest Request;
	Request.m_pInfo = &Info;

	// Set the basic parameters.
	FillBasicRequest(Request);

	// Get the account info.
	if (m_pContentServer	!= NULL)
		m_pContentServer->GetOrderForm(Request);

	// Now, see how the server responded.
	int nStatus;
	switch (nStatus = StandardCheckRequestCompletion(Request))
	{
		case CSSTATUS_Success:
		{
			break;
		}
		default:
		{
			// Some other sort of error.
			StandardProcessResponseStatus(Request.m_nStatus, Request.m_csStatus);
			break;
		}
	}

	return nStatus;
}

int CContentManager::PriceInternetOrder(const CServerPriceOrderInfo& OrderInfo, CServerPriceInfo& PriceInfo)
{
	// Create the request.
	CServerPriceOrderRequest Request;
	Request.m_OrderInfo = OrderInfo;

	// Set the basic parameters.
	FillBasicRequest(Request);

	// Price the order.
	if (m_pContentServer	!= NULL)
		m_pContentServer->PriceOrder(Request);

	// Now, see how the server responded.
	int nStatus;
	switch (nStatus = StandardCheckRequestCompletion(Request))
	{
		case CSSTATUS_Success:
		{
			PriceInfo = Request.m_PriceInfo;
			break;
		}
		default:
		{
			// Some other sort of error.
			StandardProcessResponseStatus(Request.m_nStatus, Request.m_csStatus);
			break;
		}
	}

	return nStatus;
}

int CContentManager::PurchaseInternetOrder(const CServerPurchaseInfo& PurchaseInfo, CServerPurchaseResult& PurchaseResult)
{
	// Create the request.
	CServerPurchaseRequest Request;
	Request.m_PurchaseInfo = PurchaseInfo;

	// Set the basic parameters.
	FillBasicRequest(Request);

	// Price the order.
	if (m_pContentServer	!= NULL)
		m_pContentServer->Purchase(Request);

	// Now, see how the server responded.
	int nStatus;
	switch (nStatus = StandardCheckRequestCompletion(Request))
	{
		case CSSTATUS_Success:
		{
			PurchaseResult = Request.m_PurchaseResult;
			break;
		}
		default:
		{
			// Some other sort of error.
			StandardProcessResponseStatus(Request.m_nStatus, Request.m_csStatus);
			break;
		}
	}

	return nStatus;
}

void CContentManager::OnStatusCallback(DWORD /* dwContext */, DWORD dwInternetStatus,
	LPVOID /* lpvStatusInfomration */, DWORD /* dwStatusInformationLen */)
{
	//if (!bProgressMode)
	//	return;

	//if (dwInternetStatus == INTERNET_STATUS_CONNECTED_TO_SERVER)
		//cerr << _T("Connection made!") << endl;
}

//
// Update the purchasable collections.
//

void CContentManager::UpdateCollections(void)
{
	// Setup the online order file.
	CIniFile OrdFile(m_pPathManager->ExpandPath(OnlineOrderFile));

	// Check the revisions to see if we need to update the collection list.
	CString csLastRevision = OrdFile.GetString("Online", "Revision");
	CString csRevision;
	GetOrderFormRevision(csRevision, m_csCollectionRevision);
	if (csRevision != csLastRevision)
	{
		// Get the list of collections.
		CServerCollectionInfoArray CollectionArray;

		if (GetCollectionList(CollectionArray) == CSSTATUS_Success)
		{
			// Create the order file from that.
			UpdateCollectionsOrderFile(CollectionArray, csRevision);
		}
	}
}

static inline void FileTimeToString(FILETIME ft, CString& csFileTime)
{
	csFileTime.Format("%08lx%08lx", ft.dwHighDateTime, ft.dwLowDateTime);
}

// Update licensed collections.
void CContentManager::UpdateLicensedCollections(BOOL fOverideVersionCheck)
{
	// Construct the name of the INI file for the license file.
	CString csLicenseFile = LicenseData.FileName();

	// Change the extension to INI
	CString csDrive;
	CString csDirectory;
	CString csFileName;
	
	Util::SplitPath(csLicenseFile, &csDrive, &csDirectory, &csFileName, NULL);

	CString csIniFile;
	Util::ConstructPath(csIniFile, csDrive, csDirectory, csFileName, ".INI");

	// Setup the INI file to use.
	CIniFile IniFile(csIniFile);
	CString csLastRevision = IniFile.GetString(SECTION_Online, ENTRY_Revision);
	CString csLastTimeStamp = IniFile.GetString(SECTION_Online, ENTRY_TimeStamp);

	//
	// We do not need to update the file if
	// 1) the revisions match, and
	// 2) the timestamps match
	//

	CTimeStamp TimeStamp;
	CString csCurrentTimeStamp;
	if (TimeStamp.SetFileModify(csLicenseFile))
	{
		// Generate a string from the timestamp.
		FileTimeToString(TimeStamp.GetTime(), csCurrentTimeStamp);
	}

	if (fOverideVersionCheck
			|| csLastRevision != m_csUserCollectionRevision
			|| csLastTimeStamp != csCurrentTimeStamp)
	{
		// Get the array of collections the user has licensed.
		CServerCollectionInfoArray CollectionArray;

		if (GetCollectionList(CollectionArray, "purchased") == CSSTATUS_Success)
		{
			// Build a map with the licensed products.
			CMapStringToPtr LicensedMap;
			for (int n = 0; n < CollectionArray.GetSize(); n++)
			{
				LicensedMap.SetAt(CollectionArray.GetAt(n).m_csSKU, NULL);
			}
			LicenseData.UpdateOnlineProducts(LicensedMap);
		}

		// Update the INI file to keep this from hitting next time.
		IniFile.WriteString(SECTION_Online, ENTRY_Revision, m_csUserCollectionRevision);

		// Get the new time stamp (after changes written).
		if (TimeStamp.SetFileModify(csLicenseFile))
		{
			// Generate a string from the timestamp.
			FileTimeToString(TimeStamp.GetTime(), csCurrentTimeStamp);
			IniFile.WriteString(SECTION_Online, ENTRY_TimeStamp, csCurrentTimeStamp);
		}
	}
}

BOOL CContentManager::CheckForNewInternetContent()
{
	// Construct the name of the INI file for the license file.
	CString csLicenseFile = LicenseData.FileName();

	// Change the extension to INI
	CString csDrive;
	CString csDirectory;
	CString csFileName;
	
	Util::SplitPath(csLicenseFile, &csDrive, &csDirectory, &csFileName, NULL);

	CString csIniFile;
	Util::ConstructPath(csIniFile, csDrive, csDirectory, csFileName, ".INI");

	// Setup the INI file to use.
	CIniFile IniFile(csIniFile);
	CString csLastRevision = IniFile.GetString(SECTION_Online, ENTRY_Revision);
	CString csLastSeenRevision = IniFile.GetString(SECTION_Online, ENTRY_LastSeenRevision);

	if (csLastRevision != csLastSeenRevision)
	{
		return TRUE;
	}

	return FALSE;

}

void CContentManager::UpdateWhenLastSeenInternetContent(void)
{
	// Construct the name of the INI file for the license file.
	CString csLicenseFile = LicenseData.FileName();

	// Change the extension to INI
	CString csDrive;
	CString csDirectory;
	CString csFileName;
	
	Util::SplitPath(csLicenseFile, &csDrive, &csDirectory, &csFileName, NULL);

	CString csIniFile;
	Util::ConstructPath(csIniFile, csDrive, csDirectory, csFileName, ".INI");

	// Setup the INI file to use.
	CIniFile IniFile(csIniFile);

	CString csLastRevision = IniFile.GetString(SECTION_Online, ENTRY_Revision);
	
	// Update the INI file to keep this from hitting next time.
	IniFile.WriteString(SECTION_Online, ENTRY_LastSeenRevision, csLastRevision);

}


//
// Build a revision string from the order form version and the
// category revision.
//

void CContentManager::GetOrderFormRevision(CString& csRevision, LPCSTR pszCategoryRevision)
{
	csRevision.Format("%s-%s", OrderFormVersion, pszCategoryRevision);
}

/////////////////////////////////////////////////////////////////////////////
// CContentManagerConnectionCallback
// The callback during a request. We typically use this during a download
// request when we have a progress dialog on the screen.

CContentManagerConnectionCallback::CContentManagerConnectionCallback(CDownloadStatusDialog* pDialog)
{
	m_pDialog = pDialog;	
}

DWORD CContentManagerConnectionCallback::OnStatusChange(DWORD dwInternetStatus)
{
	DWORD dwStatus = CSSTATUS_Success;

	if (m_pDialog != NULL)
	{
		UINT uID = 0;
#ifndef	ARTSTORE
		switch (dwInternetStatus)
		{
			case INTERNET_STATUS_CONNECTING_TO_SERVER:
			{
				// "Connecting to server..."
				uID = IDS_InternetConnecting;
				break;
			}
			case INTERNET_STATUS_SENDING_REQUEST:
			{
				// "Sending request..."
				uID = IDS_InternetSending;
				break;
			}
			case INTERNET_STATUS_RECEIVING_RESPONSE:
			{
				// "Receiving data..."
//				uID = IDS_InternetReceiving;
				break;
			}
			case INTERNET_STATUS_CLOSING_CONNECTION:
			{
				// "Closing connection..."
				uID = IDS_InternetDisconnecting;
				break;
			}
//			case INTERNET_STATUS_RESOLVING_NAME:
//			case INTERNET_STATUS_NAME_RESOLVED:
//			case INTERNET_STATUS_CONNECTED_TO_SERVER:
//			case INTERNET_STATUS_REQUEST_SENT:
//			case INTERNET_STATUS_RESPONSE_RECEIVED:
//			case INTERNET_STATUS_CTL_RESPONSE_RECEIVED:
//			case INTERNET_STATUS_PREFETCH:
//			case INTERNET_STATUS_CONNECTION_CLOSED:
//			case INTERNET_STATUS_HANDLE_CREATED:
//			case INTERNET_STATUS_HANDLE_CLOSING:
//			case INTERNET_STATUS_REQUEST_COMPLETE:
//			case INTERNET_STATUS_REDIRECT:
			default:
			{
				break;
			}
		}
#endif
		if (uID != 0)
		{
			CString csStatus;
			LoadConfigurationString(uID, csStatus);
			m_pDialog->SetStatusText(csStatus);
		}

		// Check if we need to abort.
		if (m_pDialog->GetAbortStatus())
		{
			dwStatus = CSSTATUS_UserAbort;
		}
	}
	return dwStatus;
}

/////////////////////////////////////////////////////////////////////////////
// CContentManagerDownloadCallback
// The callback during a download.

CContentManagerDownloadCallback::CContentManagerDownloadCallback(CDownloadStatusDialog* pDialog)
{
	m_pDialog = pDialog;
	m_dwTotalRead = 0;
	m_pBuffer = NULL;
	m_dwBufferSize = 0;
}

CContentManagerDownloadCallback::~CContentManagerDownloadCallback()
{
	delete [] m_pBuffer;
}

DWORD CContentManagerDownloadCallback::OnReceiveData(LPVOID pData, DWORD dwDataSize, DWORD dwDataTotal)
{
	DWORD dwStatus = CSSTATUS_Success;

	if (m_pDialog != NULL)
	{
		if (dwDataSize == 0)
		{
			// First time!
			m_pDialog->SetStartTime(timeGetTime());

			if (dwDataTotal == 0)
			{
				ASSERT(FALSE);
				dwStatus = CSSTATUS_InternalError;
			}
			else
			{
				// Allocate the buffer if we have not already done so.
				if (m_pBuffer == NULL)
				{
					TRY
					{
						m_pBuffer = new BYTE[m_dwBufferSize = dwDataTotal];
					}
					END_TRY

					if (m_pBuffer == NULL)
					{
						// Not enough memory.
						dwStatus = CSSTATUS_MemoryError;
					}
				}
			}
		}

		if (dwDataSize != 0)
		{
			// Update the progress dialog.
			m_pDialog->OnProgress(m_dwTotalRead, dwDataTotal);

			// Stick the data in the buffer.
			if (m_pBuffer != NULL)
			{
				if (m_dwTotalRead + dwDataSize > m_dwBufferSize)
				{
					// Ran off the end of the buffer.
					ASSERT(FALSE);
					dwStatus = CSSTATUS_InternalError;
				}
				else
				{
					// Copy the data to the next spot in the buffer.
					memcpy(m_pBuffer+m_dwTotalRead, pData, dwDataSize);
					m_dwTotalRead += dwDataSize;
				}
			}
		}

		// Check if we need to abort.
		if (m_pDialog->GetAbortStatus())
		{
			dwStatus = CSSTATUS_UserAbort;
		}
	}

	return dwStatus;
}

/////////////////////////////////////////////////////////////////////////////
// Interface map for CContentManager.

BEGIN_INTERFACE_MAP(CContentManager, CCmdTarget)
	INTERFACE_PART(CContentManager, IID_IContentManager, ContentManager)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Implementations for the IUnknown interface for CContentManager.
// This is standard stuff brought over from the docs.

STDMETHODIMP_(ULONG) CContentManager::XContentManager::AddRef()
{
	METHOD_PROLOGUE(CContentManager, ContentManager)
	return pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CContentManager::XContentManager::Release()
{
	METHOD_PROLOGUE(CContentManager, ContentManager)
	return pThis->ExternalRelease();
}

STDMETHODIMP CContentManager::XContentManager::QueryInterface(
    REFIID iid, void FAR* FAR* ppvObj)
{
	METHOD_PROLOGUE(CContentManager, ContentManager)
	return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

/////////////////////////////////////////////////////////////////////////////
// Implementations for the IContentManager interface for CContentManager.

//
// Perform the initial login and download of a graphic.
//

STDMETHODIMP_(UINT) CContentManager::XContentManager::GetInternetContent(LPCSTR strCollName, LPCSTR strFileName, CString& csBuf)
{
	METHOD_PROLOGUE(CContentManager, ContentManager)
	return pThis->GetInternetContent(strCollName, strFileName, csBuf);
}

//
// Check to see if the internet item has been downloaded and locally cached.
//

STDMETHODIMP_(BOOL) CContentManager::XContentManager::IsFileLocallyCached(LPCSTR strCollName, LPCSTR strFileName)
{
	METHOD_PROLOGUE(CContentManager, ContentManager)
	return pThis->IsFileLocallyCached(strCollName, strFileName);
}

//
// A Request to update the account on the data server.
//

STDMETHODIMP_(BOOL) CContentManager::XContentManager::UpdateAccount(void)
{
	METHOD_PROLOGUE(CContentManager, ContentManager)
	return pThis->UpdateAccount();
}

//
// Check if account exists and create a new one if not on the data server
//

STDMETHODIMP_(BOOL) CContentManager::XContentManager::CheckAccountStatus(void)
{
	METHOD_PROLOGUE(CContentManager, ContentManager)
	return pThis->CheckAccountStatus();
}

//
// Create a new account on the data server
//

STDMETHODIMP_(BOOL) CContentManager::XContentManager::CreateNewAccount(void)
{
	METHOD_PROLOGUE(CContentManager, ContentManager)
	return pThis->CreateNewAccount();
}

//
// Change the internet account to a pre-existing one.
//

STDMETHODIMP_(BOOL) CContentManager::XContentManager::RequestExistingAccount(void)
{
	METHOD_PROLOGUE(CContentManager, ContentManager)
	return pThis->RequestExistingAccount();
}

//
// Delete all locally cached internet files.
//

STDMETHODIMP_(BOOL) CContentManager::XContentManager::DeleteCachedDirectories(CWnd* pParent, LPCSTR pszCollection)
{
	METHOD_PROLOGUE(CContentManager, ContentManager)
	return pThis->DeleteCachedDirectories(pParent, pszCollection);
}

//
// Get the internet order form for collections
//

STDMETHODIMP_(int) CContentManager::XContentManager::LicenseProducts(LPCSTR pszProduct /*=NULL*/, LPCSTR InitialSelection /*NULL*/)
{
	METHOD_PROLOGUE(CContentManager, ContentManager)
	return pThis->LicenseProducts(pszProduct, InitialSelection);
}

//
// Price the internet order 
//

STDMETHODIMP_(int) CContentManager::XContentManager::PriceInternetOrder(const CServerPriceOrderInfo& OrderInfo, CServerPriceInfo& PriceInfo)
{
	METHOD_PROLOGUE(CContentManager, ContentManager)
	return pThis->PriceInternetOrder(OrderInfo, PriceInfo);
}

//
// Get the Account ID 
//

STDMETHODIMP_(LPCSTR) CContentManager::XContentManager::GetAccountID(void)
{
	METHOD_PROLOGUE(CContentManager, ContentManager)
	return pThis->GetAccountID();
}

//
// Get a collection list.
//

STDMETHODIMP_(int) CContentManager::XContentManager::GetCollectionList(CServerCollectionInfoArray& CollectionArray, LPCSTR csSelector, LPCSTR csIgnoreLocale)
{
	METHOD_PROLOGUE(CContentManager, ContentManager)

	int nResult = CSSTATUS_ServiceError;		// Not really...

	if (pThis->CheckAccountStatus()) //success
	{
		if (pThis->CheckForValidConnection())
		{
			if (pThis->CheckIfSessionActive())
			{
				nResult = pThis->GetCollectionList(CollectionArray, csSelector, csIgnoreLocale);
			}
		}
	}
	return nResult;
}

//
// Download a file.
//

STDMETHODIMP_(int) CContentManager::XContentManager::DownloadFile(const CServerDownloadInfo& DownloadInfo, IServerConnectionCallback* pConnectionCallback, IServerDownloadCallback* pDownloadCallback)
{
	METHOD_PROLOGUE(CContentManager, ContentManager)

	int nResult = CSSTATUS_ServiceError;		// Not really...

	if (pThis->CheckAccountStatus()) //success
	{
		if (pThis->CheckForValidConnection())
		{
			if (pThis->CheckIfSessionActive())
			{
				nResult = pThis->DownloadFile(DownloadInfo, pConnectionCallback, pDownloadCallback);
			}
		}
	}
	return nResult;
}

//
// Purchase the internet order 
//

STDMETHODIMP_(int) CContentManager::XContentManager::PurchaseInternetOrder(const CServerPurchaseInfo& PurchaseInfo, CServerPurchaseResult& PurchaseResult)
{
	METHOD_PROLOGUE(CContentManager, ContentManager)
   return pThis->PurchaseInternetOrder(PurchaseInfo, PurchaseResult);
}

//
// Get shipping methods for a location.
//

STDMETHODIMP_(int) CContentManager::XContentManager::GetShippingInfo(const CServerLocationInfo& LocationInfo, CServerShippingInfoArray& ShippingMethods)
{
	METHOD_PROLOGUE(CContentManager, ContentManager)
   return pThis->GetShippingInfo(LocationInfo, ShippingMethods);
}

//
// Send account info.
//

STDMETHODIMP_(int) CContentManager::XContentManager::SendAccountInfo(const CServerAccountInfo& AccountInfo, LPCSTR csNewPassword /*=NULL*/)
{
	METHOD_PROLOGUE(CContentManager, ContentManager)
   return pThis->SendAccountInfo(AccountInfo, csNewPassword);
}

STDMETHODIMP_(BOOL) CContentManager::XContentManager::UpdateOnlineOrderForm()
{
	METHOD_PROLOGUE(CContentManager, ContentManager)

	BOOL fRet = FALSE;

	// Get the name of the online.ord file.
	CIniFile OrdFile(pThis->m_pPathManager->ExpandPath(OnlineOrderFile));
	
	CString csRevision = OrdFile.GetString(SECTION_Online, ENTRY_Revision);

	// Log out.
	pThis->Logout();

	// And log in again.
	if (pThis->CheckAccountStatus()) //success
	{
		if (pThis->CheckForValidConnection())
		{
			pThis->CheckIfSessionActive();		// Does a login which
															// updates the order form.
		}
	}
	
	fRet = csRevision != OrdFile.GetString(SECTION_Online, ENTRY_Revision);

	return fRet;
}

STDMETHODIMP_(void) CContentManager::XContentManager::UpdateWhenLastSeenInternetContent(void)
{
	METHOD_PROLOGUE(CContentManager, ContentManager)
   pThis->UpdateWhenLastSeenInternetContent();
}

STDMETHODIMP_(BOOL) CContentManager::XContentManager::CheckForNewInternetContent(void)
{
	METHOD_PROLOGUE(CContentManager, ContentManager)
   return pThis->CheckForNewInternetContent();
}

STDMETHODIMP_(BOOL) CContentManager::XContentManager::DeleteDirectory(LPCSTR pDirectory)
{
	METHOD_PROLOGUE(CContentManager, ContentManager)
   return pThis->DeleteDirectory(pDirectory);
}