/***************************************************************************
*  FILE:        INETCONN.CPP                                               *
*  SUMMARY:     Class for establishing an internet connection with a       *
*                 non-dialup networking compliant application such as AOL. *
*  AUTHOR:      Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Initials Legend:                                                        *
*     DGP       Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Revision History                                                        *
*                                                                          *
*  Date     Initials  Description of Change                                *
*  ------------------------------------------------------------------------*
*  1/20/98  DGP      Created                                               *
// $Log: /PM8/App/inetconn.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 10    9/28/98 12:20p Johno
// ARTSTORE define to compile version without WININET
// 
// 9     9/28/98 10:54a Johno
// Use pmwinet.h in place of afxinet.h
// 
// 8     8/29/98 8:24a Dennis
// Changes for AOL 4.0 (hidden file detection and Autodial disable)
// 
// 7     7/16/98 5:16p Dennis
// Connect now checks connection before setting up timer of first check is
// 0.
// 
// 6     4/10/98 4:47p Rlovejoy
// Took out assert on "other error".
// 
// 5     3/02/98 5:04p Johno
// Compile update
***************************************************************************/
#include "stdafx.h"
#include "pmwinet.h"    // For WININet calls
#include <raserror.h>   // for RAS API Errors
#include "inetconn.h"
#include "util.h"       // For FileExists check
#include "regstr.h"

// Static class member initialization
CMapWordToPtr     CInternetConnect::sm_thisList;

// CAOLInfo methods
CAOLInfo::CAOLInfo()
{
   m_bInstalled = FALSE;
   SetOSVersion(osUnknown);
}

CAOLInfo::~CAOLInfo()
{
}
  
BOOL CAOLInfo::IsInstalled()
{
   HKEY        hKey = NULL;
   char        szBuffer[_MAX_PATH];
   BOOL        bDetectedWin95Version = FALSE;
   LPCSTR      pszEmptyString = "";

   m_bInstalled = FALSE;
   SetOSVersion(osUnknown);
   m_csAppPath.Empty();
   m_csAppFileName.Empty();

   // First Check registry to determine of WIN95 Version is Installed
   if (RegOpenKeyEx(
      HKEY_CURRENT_USER,
      "Software\\America Online",
      0,
      KEY_QUERY_VALUE,
      &hKey) == ERROR_SUCCESS)
   {
      TRY
      {
         CString  csTemp;
         if (Util::GetRegistryString(HKEY_LOCAL_MACHINE, 
                                    "SOFTWARE\\America Online\\AOL\\CurrentVersion", 
                                    "AppPath",
                                    csTemp) == TRUE)
         {
            csTemp += "\\waol.exe";
            // Check if AOL program file exists (hidden file for AOL 4.0)
            if (Util::FileExists(csTemp,0))
            {
               bDetectedWin95Version = TRUE;   
               m_csAppFileName = csTemp;
            }
         }
      }
      END_TRY

      RegCloseKey(hKey);
   }
   // Didn't find 95 version...
   if (m_csAppFileName.IsEmpty())
   {
      // Getting application path may change with next version of AOL (4.0)
      ::GetProfileString("WAOL", "AppPath", pszEmptyString, szBuffer, sizeof(szBuffer));
      m_csAppPath = szBuffer;
      if(!m_csAppPath.IsEmpty())
         m_csAppFileName = m_csAppPath + "\\WAOL.EXE";
   }

   if(!m_csAppFileName.IsEmpty())
   {
      SetOSVersion(osUnknown);
      // Check if OS version of installed .exe 
      if(Util::FileExists(m_csAppFileName,0))
      {
         CFile    cfileAOL;
         BYTE     bySegEXE;
         DWORD    dwSegHdrOffset;
         unsigned char  chaSignature[2];

         // Found Program, now version .exe type
         m_bInstalled = TRUE;

         TRY
         {
            if(cfileAOL.Open(m_csAppFileName,
               CFile::modeRead | CFile::typeBinary | CFile::shareDenyWrite))
            {
               cfileAOL.Seek(0x18, CFile::begin);
               cfileAOL.Read(&bySegEXE, sizeof(BYTE));

               // if value at this location == 40h, 
               // then we get value at 3Ch which is DWORD offset
               // to segmented EXE header
               // Otherwise, it's an old-style format EXE
               if(bySegEXE == 0x40)
               {
                  cfileAOL.Seek(0x3C, CFile::begin);
                  cfileAOL.Read(&dwSegHdrOffset, sizeof(DWORD));
                  // Seek to segmented EXE header
                  cfileAOL.Seek(dwSegHdrOffset, CFile::begin);
                  // Get EXE signature
                  cfileAOL.Read(&chaSignature, sizeof(chaSignature));
                  if(chaSignature[0] == 'P')
                     SetOSVersion(osWin32);
                  else if(chaSignature[0] == 'N')
                  {
                     ASSERT(!(GetOSVersion() == osWin32));
                     SetOSVersion(osWin16);
                  }
                  else
                     ASSERT(0);  // Cannot determine .exe type
                     
               }
               cfileAOL.Close();
            }
         }
         CATCH_ALL(e)
         {
            ASSERT(0);
         }
         END_CATCH_ALL
      }
   }

   if(bDetectedWin95Version && GetOSVersion() != osWin32)
   {
      // Discreprency between registry and .exe info
      // but ignore it since user may have installed
      // 16-bit version over 32-bit
//            ASSERT(0);
      if(GetOSVersion() == osUnknown)
         SetOSVersion(osWin32);
   }

   return m_bInstalled;
}

// CCompuServeInfo methods
CCompuServeInfo::CCompuServeInfo()
{
   m_bInstalled = FALSE;
}

CCompuServeInfo::~CCompuServeInfo()
{
}

BOOL CCompuServeInfo::IsInstalled()
{
   m_bInstalled = FALSE;
   m_csAppPath.Empty();
   m_csAppFileName.Empty();

   if(!Util::GetRegistryString(
      HKEY_LOCAL_MACHINE,
      REGSTR_PATH_APPPATHS "\\cs3.exe",
      NULL,
      m_csAppFileName))
         return m_bInstalled;

   if(!Util::FileExists(m_csAppFileName))
   {
      m_csAppFileName.Empty();
      return m_bInstalled;
   }

   m_bInstalled = TRUE;

   return m_bInstalled;
}

// CDialupConnections methods
CDialupConnections::CDialupConnections()
{
   InitVariables();
}

CDialupConnections::~CDialupConnections()
{
   DeInit();
}

void CDialupConnections::InitVariables()
{
   m_bInitialized = FALSE;
   m_prcaRasConn  = NULL;
   m_preaRasEntry = NULL;
   m_dwNumConnections = 0;
   m_dwRasError   = 0;

   // DLL Related
   m_hRasLibrary = NULL;
   // Init Proc Pointers
   m_pProcRasEnumEntries = NULL;
   m_pProcRasEnumConnections = NULL;
   m_pProcRasGetConnectStatus = NULL;
}

CDialupConnections::ErrorCode CDialupConnections::Init()
{
   // If already initialized, do nothing
   if(m_bInitialized)
      return errorNone;

   m_hRasLibrary = ::LoadLibrary("RASAPI32.DLL");
   if(m_hRasLibrary == NULL)
      return errorLibraryNotFound;

   m_pProcRasEnumEntries      = (TDProcRasEnumEntries)
      GetProcAddress(m_hRasLibrary, IdentifierToString(RasEnumEntries));
   m_pProcRasEnumConnections  = (TDProcRasEnumConnections)
      GetProcAddress(m_hRasLibrary, IdentifierToString(RasEnumConnections));
   m_pProcRasHangUp           = (TDProcRasHangUp)
      GetProcAddress(m_hRasLibrary, IdentifierToString(RasHangUp));
   m_pProcRasGetConnectStatus = (TDProcRasGetConnectStatus)
      GetProcAddress(m_hRasLibrary, IdentifierToString(RasGetConnectStatus));
   if(m_pProcRasEnumEntries == NULL || m_pProcRasEnumConnections == NULL ||
      m_pProcRasHangUp == NULL || m_pProcRasGetConnectStatus == NULL)
   {
      ASSERT(0);  // Missing function in library
      DeInit();
      return errorMissingFunction;
   }

   m_bInitialized = TRUE;
   return errorNone;
}

void CDialupConnections::DeInit()
{
   FreeConnections();
   FreeEntries();
   if(m_hRasLibrary)
   {
      ::FreeLibrary(m_hRasLibrary);
      m_hRasLibrary = NULL;
   }
   InitVariables();
}

void CDialupConnections::AllocateEntries(DWORD dwCount /* = 1 */)
{
   DWORD dwI;

   FreeEntries();
   m_preaRasEntry = new RASENTRYNAME [dwCount];
   m_dwNumEntries = dwCount;

   // Loop through connection data and fill in size
   for(dwI=0;dwI<m_dwNumEntries;dwI++)
   {
      // Set size as required by RAS API
      m_preaRasEntry[dwI].dwSize = sizeof( RASENTRYNAME );
   }
}

void CDialupConnections::FreeEntries()
{
   if(m_preaRasEntry)
      delete [] m_preaRasEntry;
   m_preaRasEntry = NULL;
   m_dwNumEntries = 0;
}

// This method retrieves all of the current dialup networking entries
CDialupConnections::ErrorCode CDialupConnections::EnumerateEntries()
{
   DWORD       dwEntriesFound = 0;
   DWORD       dwBytesRequired;
   ErrorCode   errorCode;

   errorCode = Init();

   if(errorCode != errorNone)
      return errorCode;

   // enumerate entries
   AllocateEntries(1);

   dwBytesRequired = sizeof(RASENTRYNAME) * GetEntryCount();
   m_dwRasError = m_pProcRasEnumEntries( NULL, NULL, m_preaRasEntry, &dwBytesRequired, &dwEntriesFound);

   // If not enough connections allocated, allocate more based on 
   // dwBytesRequired value returned
   if ( m_dwRasError == ERROR_BUFFER_TOO_SMALL )
   {
      AllocateEntries(dwBytesRequired / sizeof(RASENTRYNAME));
      m_dwRasError = m_pProcRasEnumEntries( NULL, NULL, m_preaRasEntry, &dwBytesRequired, &dwEntriesFound);
      if(m_dwRasError != 0)
      {
         // unable to Enumerate even after allocating proper size
         ASSERT(0);
      }
   }

   if(m_dwRasError == 0)
   {
      if(dwEntriesFound > 0)
         errorCode = errorNone;
      else
      {
         FreeEntries();
         errorCode = errorFail;
      }
   }
   else
   {
      FreeEntries();
      errorCode = errorRAS;
   }
   return errorCode;
}

void CDialupConnections::AllocateConnections(DWORD dwConnectionCount /* = 1 */)
{
   DWORD dwI;

   FreeConnections();
   m_prcaRasConn = new RASCONN [dwConnectionCount];
   m_dwNumConnections = dwConnectionCount;

   // Loop through connection data and fill in size
   for(dwI=0;dwI<m_dwNumConnections;dwI++)
   {
      // Set size as required by RAS API
      m_prcaRasConn[dwI].dwSize = sizeof( RASCONN );
   }
}

void CDialupConnections::FreeConnections()
{
   if(m_prcaRasConn)
      delete [] m_prcaRasConn;
   m_prcaRasConn = NULL;
   m_dwNumConnections = 0;
}

// This method retrieves all of the ACTIVE (connected) dialup networking connections
CDialupConnections::ErrorCode CDialupConnections::EnumerateConnections()
{
   DWORD       dwConnectionsFound = 0;
   DWORD       dwBytesRequired;
   ErrorCode   errorCode;
   
   errorCode = Init();

   if(errorCode != errorNone)
      return errorCode;

   // enumerate connections
   AllocateConnections(1);

   dwBytesRequired = sizeof(RASCONN) * GetConnectionCount();
   m_dwRasError = m_pProcRasEnumConnections( m_prcaRasConn, &dwBytesRequired, &dwConnectionsFound);

   // If not enough connections allocated, allocate more based on 
   // dwBytesRequired value returned
   if ( m_dwRasError == ERROR_BUFFER_TOO_SMALL )
   {
      AllocateConnections(dwBytesRequired / sizeof(RASCONN));
      m_dwRasError = m_pProcRasEnumConnections( m_prcaRasConn, &dwBytesRequired, &dwConnectionsFound );
      if(m_dwRasError != 0)
         {
            // Enable to Enumerate even after allocating proper size
            ASSERT(0);
         }
   }
   else 
   if ( m_dwRasError != 0 ) // other error
      ASSERT(0);

   if(m_dwRasError == 0)
   {
      if(dwConnectionsFound > 0)
         errorCode = errorNone;
      else
      {
         FreeConnections();
         errorCode = errorFail;
      }
   }
   else
   {
      FreeConnections();
      errorCode = errorRAS;
   }

   return errorCode;
}

DWORD CDialupConnections::GetConnectStatus(HRASCONN hConnection, LPRASCONNSTATUS pStatus)
{
   ErrorCode   errorCode;
   
   errorCode = Init();
   if(errorCode != errorNone)
      return FALSE;

   pStatus->dwSize = sizeof(RASCONNSTATUS);
   m_dwRasError = m_pProcRasGetConnectStatus(hConnection, pStatus);

   return m_dwRasError;
}

BOOL  CDialupConnections::IsConnected(HRASCONN hConnection)
{
   ErrorCode   errorCode;
   RASCONNSTATUS  connectionStatus;
   BOOL        bConnected = FALSE;
   
   errorCode = Init();

   if(errorCode != errorNone)
      return FALSE;

   if(GetConnectStatus(hConnection, &connectionStatus) == 0)
   {
      if(connectionStatus.rasconnstate == RASCS_Connected)
         bConnected = TRUE;
   }

   return bConnected;
}

CDialupConnections::ErrorCode CDialupConnections::HangUp(HRASCONN hConnection)
{
   ErrorCode      errorCode;

   errorCode = Init();

   if(errorCode != errorNone)
      return errorCode;

   if(hConnection)
   {
      m_dwRasError = m_pProcRasHangUp( hConnection );
   }
   else
   {
      DWORD dwCurConnection;
      const RASCONN *pConnections = GetConnections();

      for(dwCurConnection = 0;dwCurConnection < GetConnectionCount();dwCurConnection++)
      {
         ASSERT(pConnections[dwCurConnection].hrasconn);
         if(pConnections[dwCurConnection].hrasconn)
            HangUp(pConnections[dwCurConnection].hrasconn);
      }
   }

   if(m_dwRasError == 0)
      errorCode = errorNone;
   else
      errorCode = errorRAS;
   return errorCode;
}

// CInternetConnect  methods
CInternetConnect::CInternetConnect()
{
   Init();
}

CInternetConnect::~CInternetConnect()
{
   if(IsTimerActive())
      StopTimer();
}

void CInternetConnect::Init()
{
   m_dwTimeElapsed = m_dwStartTime = 0;
   m_bActiveTimer = FALSE;
   m_pTimerWnd = NULL;
   m_dwWinINetError = 0;
   m_csErrorMessage.Empty();
   m_bInTimerProc = FALSE;
   m_dwTimerInterval = INETCONN_SECOND;
   m_bWatchForDialup = FALSE;
   m_bSetAutodial = FALSE;
   SetURL();
}

CInternetConnectStatus::Status CInternetConnect::Connect(
   CWnd *pTimerWnd,
   BOOL bWatchForDialup,   /* FALSE */
   UINT uiTimerID,         /* = 0 */ 
   DWORD dwTimeOut,        /* = INETCONN_MINUTE * 2 */
   DWORD dwFirstCheck,     /* = INETCONN_SECOND * 10 */
   DWORD dwTimerInterval   /* = INETCONN_SECOND */ )
{
   m_bOriginalAutodialState = IsAutodialEnabled();

   m_bWatchForDialup = bWatchForDialup;
   // If TimerID supplied, connect using Timer
   if(uiTimerID > 0)
      {
         // Check if connected immediately if no first check delay
         if(dwFirstCheck == 0)
            {
               CheckConnection();
               if(m_status.GetStatus() == CInternetConnectStatus::statusConnected)
                  return m_status.GetStatus();
            }

         StartTimer(pTimerWnd, uiTimerID, dwTimeOut, dwFirstCheck, dwTimerInterval);
      }
   else
      CheckConnection();

   return m_status.GetStatus();
}

// Returns 0 if unable to start timer
UINT CInternetConnect::StartTimer(
   CWnd *pTimerWnd,
   UINT uiTimerID,      
   DWORD dwTimeOut,     
   DWORD dwFirstCheck,  
   DWORD dwTimerInterval)
{
   UINT  uiThisTimerID;

   m_status.Reset();

   ASSERT(pTimerWnd);

   if(pTimerWnd == NULL)
   {
      m_status.SetStatus(CInternetConnectStatus::statusTimerFailed);
      return 0;
   }

   // If timer already setup for this object, do nothing
   if(IsTimerActive())
   {
      ASSERT(0);
      return 0;
   }

   m_pTimerWnd = pTimerWnd;
   m_dwTimeOut = dwTimeOut;
   m_dwTimerInterval = dwTimerInterval;

   SetTimeOut(dwTimeOut);
   SetFirstCheckTime(dwFirstCheck);
   uiThisTimerID = m_pTimerWnd->SetTimer(uiTimerID, m_dwTimerInterval, CInternetConnect::TimerProc);
   m_uiTimerID = uiThisTimerID;
   if(uiThisTimerID > 0)
   {
      m_bActiveTimer = TRUE;
      sm_thisList.SetAt(m_uiTimerID, (LPVOID) this); // Store ID and This pointer
   }
   else
      m_status.SetStatus(CInternetConnectStatus::statusTimerFailed);
   return uiThisTimerID;
}

BOOL CInternetConnect::StopTimer(CInternetConnectStatus::Status enStatus)
{
   BOOL  bKilledTimer;

   if(m_pTimerWnd == NULL)
   {
      ASSERT(0);     // Invalid Parent Window
      return FALSE;
   }

   m_status.SetStatus(enStatus);

   if(!m_bActiveTimer)
      return TRUE;      // Lie!

   bKilledTimer = m_pTimerWnd->KillTimer(m_uiTimerID);

   sm_thisList.RemoveKey(m_uiTimerID);   // Remove this object from list
   Init();

   return bKilledTimer;
}

CInternetConnectStatus::Status CInternetConnect::CheckConnection()
{
   CDialupConnections            dialups;
   CDialupConnections::ErrorCode dialupError;

   m_status.SetStatus(CInternetConnectStatus::statusWaitingForConnect);
   if(m_bWatchForDialup)
   {
      dialupError = dialups.EnumerateConnections();
      if(dialupError == CDialupConnections::errorNone &&
         dialups.GetConnectionCount() > 0)
      {
         const RASCONN *pConnections = dialups.GetConnections();
         if(pConnections && dialups.IsConnected(pConnections[0].hrasconn))
            m_status.SetStatus(CInternetConnectStatus::statusConnected);
      }

   }
   else
      {
         BOOL bSetAutodial = FALSE;

         // If Connection type is not dialup networking type
         // and Autodial is enabled, disable it so dialup networking connect
         // dialog doesn't appear
         if(IsAutodialEnabled())
            {
               m_bOriginalAutodialState = EnableAutodial(FALSE);
               m_bSetAutodial = TRUE;
            }

         TryConnect();
      }
   return m_status.GetStatus();
}

BOOL CInternetConnect::EnableAutodial(BOOL bEnable)
   {
      BOOL bPrevAutodialState = FALSE;

      HKEY hKey = NULL;
      DWORD dwType, dwAutoDialEnabled;
      DWORD dwSize = sizeof(dwAutoDialEnabled);
      LONG lResult;
      TRY
      {
         if (RegOpenKeyEx(
                  HKEY_CURRENT_USER,
                  "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
                  0,
                  KEY_QUERY_VALUE,
                  &hKey) == ERROR_SUCCESS)
         {
            lResult = RegQueryValueEx(
                                 hKey,
                                 "EnableAutodial",
                                 0,
                                 &dwType,
                                 (BYTE *)&dwAutoDialEnabled,
                                 &dwSize);
            if(lResult == ERROR_SUCCESS && (dwType == REG_DWORD || dwType == REG_BINARY))
               {
                  bPrevAutodialState = (BOOL) dwAutoDialEnabled;

                  dwAutoDialEnabled = (DWORD) bEnable;
                  lResult = RegSetValueEx(
                     hKey,
                     "EnableAutodial",
                     0,
                     dwType, 
                     (CONST BYTE *)&dwAutoDialEnabled,
                     dwSize);
               }
         }
      }
      CATCH_ALL(e)
      {
      }
      END_CATCH_ALL
      return bPrevAutodialState;
   }

BOOL CInternetConnect::IsAutodialEnabled()
   {
      BOOL bAutodialEnabled = FALSE;

      HKEY  hKey = NULL;
      DWORD dwType, dwAutoDialEnabled;
      DWORD dwSize = sizeof(dwAutoDialEnabled);
      LONG  lResult;
      TRY
      {
         if (RegOpenKeyEx(
                  HKEY_CURRENT_USER,
                  "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
                  0,
                  KEY_QUERY_VALUE,
                  &hKey) == ERROR_SUCCESS)
         {
            lResult = RegQueryValueEx(
                                 hKey,
                                 "EnableAutodial",
                                 0,
                                 &dwType,
                                 (BYTE *)&dwAutoDialEnabled,
                                 &dwSize);
            if(lResult == ERROR_SUCCESS && (dwType == REG_DWORD || dwType == REG_BINARY))
               bAutodialEnabled = (BOOL) dwAutoDialEnabled;
         }
      }
      CATCH_ALL(e)
      {
      }
      END_CATCH_ALL
      return bAutodialEnabled;
   }

void CInternetConnect::RestoreAutodial()
   {
      // Restore original Autodial state
      if(m_bSetAutodial)
         {
            EnableAutodial(m_bOriginalAutodialState);
            m_bSetAutodial = FALSE;
         }
   }     

CInternetConnectStatus::Status CInternetConnect::TryConnect()
{
#ifdef ARTSTORE
	return m_status.GetStatus();
#else
//      BOOL              bSetOption;
   DWORD             dwAccessType = PRE_CONFIG_INTERNET_ACCESS;
   DWORD             dwHttpRequestFlags =
      INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT;
   CInternetSession  session(_T("PrintMaster for Windows 95"), 1, dwAccessType);
   CHttpConnection*  pServer = NULL;
   CHttpFile*        pFile = NULL;
   CInternetConnectStatus::Status  enStatus;

   m_status.SetStatus(CInternetConnectStatus::statusWaitingForConnect);
   enStatus = m_status.GetStatus();
	try
	{
		INTERNET_PORT     nPort;
		DWORD             dwServiceType;
		CString           strServerName;
		CString           strObject;

		if (!AfxParseURL(GetURL(), dwServiceType, strServerName, strObject, nPort) ||
			dwServiceType != INTERNET_SERVICE_HTTP)
		{
			m_status.SetStatus(CInternetConnectStatus::statusInvalidURL);
         return m_status.GetStatus();
		}

		pServer = session.GetHttpConnection(strServerName, nPort);

//         DWORD dwNewTimeOut = INETCONN_SECOND * 2;
//         bSetOption = session.QueryOption(INTERNET_OPTION_CONNECT_TIMEOUT, dwNewTimeOut);

      // Specify type of request desired
		pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET,
			strObject, NULL, 1, NULL, NULL, dwHttpRequestFlags);

      ASSERT(pFile);
      const TCHAR szHeaders[] =
	      _T("Accept: text/*\r\nUser-Agent: PrintMaster for Windows 95\r\n");
		pFile->AddRequestHeaders(szHeaders);

      // Call to SendRequest() throws exception on connect failure
		pFile->SendRequest();

      // If we made it here we can assume we're connected
      m_status.SetStatus(CInternetConnectStatus::statusConnected);
   }
	catch (CInternetException* pEx)
	{
		// catch errors from WinINet
      // see wininet.h for errorcodes beginning with ERROR_INTERNET
		TCHAR szErr[1024];

      m_dwWinINetError = pEx->m_dwError;

		pEx->GetErrorMessage(szErr, 1024);
      m_csErrorMessage = "CInternetException: ";
      m_csErrorMessage += szErr;

      switch(pEx->m_dwError)
      {
         case ERROR_INTERNET_CANNOT_CONNECT:
         case ERROR_INTERNET_NAME_NOT_RESOLVED:
         // If trying to connect through timer, 
         // ignore normal connect failures
         if(!IsTimerActive())
            enStatus = CInternetConnectStatus::statusCannotConnect;
         break;
         
         case ERROR_INTERNET_INVALID_URL:
         enStatus = CInternetConnectStatus::statusInvalidURL;
         break;
         
         default:
         enStatus = CInternetConnectStatus::statusOtherWinINetError;
         break;
      }
      m_status.SetStatus(enStatus);
		pEx->Delete();
	}

   if(pFile)
   {
   	pFile->Close();
	   delete pFile;
   }

   if(pServer)
   {
	   pServer->Close();
   	delete pServer;
   }

   session.Close();

   return m_status.GetStatus();
#endif
}

void CALLBACK EXPORT CInternetConnect::TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwCurTime)
{
   CInternetConnect  *pThis = NULL;

   // Lookup this pointer for object by Timer ID in CMapWordToPtr list
   if(sm_thisList.Lookup(nIDEvent, (LPVOID &) pThis) == 0)
   {
      // Failed to find Timer ID in thisList
      ASSERT(0);
      ::KillTimer(hWnd, nIDEvent);
      return;
   }

   ASSERT(pThis);

   // If already in timer proc, wait until whatever is going on completes
   if(pThis->m_bInTimerProc)
      return;

   pThis->m_bInTimerProc = TRUE;

   // Make sure window handle is valid before doing anything
   if(pThis->m_pTimerWnd == NULL)
   {
      ASSERT(0);
      pThis->StopTimer(CInternetConnectStatus::statusOtherError);
      return;
   }

   // If no start time has been set, then this must be first call to timer proc
   if(pThis->m_dwStartTime == 0)
   {
      pThis->m_dwStartTime = dwCurTime;
      pThis->m_dwTimeElapsed = pThis->m_dwTimerInterval;
   }
   else
   {
      // Compute time elapsed
      pThis->m_dwTimeElapsed = dwCurTime - pThis->m_dwStartTime + pThis->m_dwTimerInterval;
   }

   // Check if we should start checking for internet connection
   if(pThis->m_dwTimeElapsed < pThis->m_dwFirstCheck)
   {
      pThis->m_bInTimerProc = FALSE;
      return;                 // bail, not time yet!
   }

   // Check if Timed Out
   if(pThis->m_dwTimeElapsed > pThis->m_dwTimeOut)
   {
      pThis->StopTimer(CInternetConnectStatus::statusTimedOut);
      return;
   }

   pThis->CheckConnection();
   if(pThis->GetStatus() != CInternetConnectStatus::statusWaitingForConnect)
      pThis->StopTimer(pThis->GetStatus());

   pThis->m_bInTimerProc = FALSE;
}
