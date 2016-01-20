// MySocket.cpp: implementation of the CMySocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MySocket.h"
#include "dialups.h"
#include "util.h"       // for YieldToWindows()

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define WM_SOCKET_NOTIFY   WM_USER+1
#define WM_TIMER_NOTIFY    WM_USER+2


// CMySocketSettings members
CMySocketSettings::CMySocketSettings()
{
}

CMySocketSettings::CMySocketSettings(
   HWND           hParent, 
   ConnectMethod  enConnectMethod,
   UINT           uiCompleteMessageID,
   DWORD          dwTimeOut,
   DWORD          dwTimerInterval,
   DWORD          dwFirstCheck,
   UINT           uiTimerID)
{
   Init(hParent,enConnectMethod,uiCompleteMessageID,dwTimeOut,dwTimerInterval,dwFirstCheck,uiTimerID);
}

CMySocketSettings::~CMySocketSettings()
{
}

void CMySocketSettings::Init(
   HWND           hParent, 
   ConnectMethod  enConnectMethod,
   UINT           uiCompleteMessageID /* = 0 */,
   DWORD          dwTimeOut /* = TIMER_MINUTE * 2 */,
   DWORD          dwTimerInterval /* = TIMER_SECOND * 1 */,
   DWORD          dwFirstCheck /* = TIMER_SECOND * 10 */, 
   UINT           uiTimerID /* = 1000 */)
{
   m_hwndParent = hParent;
   m_enConnectMethod = enConnectMethod;
   m_uiCompleteMessageID = uiCompleteMessageID;
   m_dwTimeOut = dwTimeOut;
   m_dwTimerInterval = dwTimerInterval;
   m_dwFirstCheck = dwFirstCheck;
   m_uiTimerID = uiTimerID;
}


// CMySocket Methods

//////////////////////////////////////////////////////////////////////
// CMySocket Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMySocket::CMySocket()
{
   m_bInitialized = FALSE;
   SetStatus(CInternetConnectStatus::statusInitFailed,FALSE);
   m_pSocketWnd = NULL;
   m_bSetAutodial = FALSE;
   // Get current Autodial setting
   m_bOriginalAutodialState = IsAutodialEnabled();
   m_pConnectThread = NULL;
   m_bAutodialRestore = TRUE;
   SetURL("www.printmaster.com");
   InitConnectState();
}

CMySocket::~CMySocket()
{
   Close();
   if(m_bAutodialRestore)
      RestoreAutodial();
}

void CMySocket::InitConnectState()
{
   m_bEstablishedSocketConnection = FALSE;
   m_nLastError = 0;
   m_hAsyncTaskHandle = NULL;
   m_bTimerIsActive = FALSE;
   m_Socket = INVALID_SOCKET;
   m_bCheckingIfConnected = FALSE;
   m_pTimerThread = NULL;
   m_nWinsockError = 0;
   m_dwTimeElapsed = 0;
   m_bSettingUpWinsock = FALSE;
   m_Socket = INVALID_SOCKET;
   // Notes
   // m_bSetAutodial and m_bOriginalAutodialState is not reset here 
   // since we want state retained across new connect attempts
}

CInternetConnectStatus::Status CMySocket::Init(CMySocketSettings &refSocketSettings)
{
   Close();

   m_SocketSettings = refSocketSettings;  // Copy Callers settings to our object

   // Need a parent for Timer and Invisible Socket Window for
   // message handling
   ASSERT(m_SocketSettings.GetParent());
   if(m_SocketSettings.GetParent() == NULL)
      return SetStatus(CInternetConnectStatus::statusInitFailed);

   m_bInitialized = TRUE;

   // Create hidden window to receive Socket notifications and Timer Messages
   if(m_pSocketWnd == NULL)
   {
	   m_pSocketWnd = new CMySocketWnd(this);
	   m_pSocketWnd->m_hWnd = NULL;
	   if (!m_pSocketWnd->CreateEx(0, AfxRegisterWndClass(0),
		   _T("MySocket Notification Sink"),
		   WS_OVERLAPPED, 0, 0, 0, 0, m_SocketSettings.GetParent(), NULL))
	   {
		   TRACE0("Warning: unable to create socket notify window!\n");
         return SetStatus(CInternetConnectStatus::statusInitFailed);
	   }
   }
   return SetStatus(CInternetConnectStatus::statusSuccess, FALSE);
}

BOOL CMySocket::IsTimerActive()
{
   return (m_bTimerIsActive || m_pTimerThread);
}

CInternetConnectStatus::Status CMySocket::Connect()
{
   BOOL     bConnected = FALSE;
   CString  csMessage;

   if(!m_bInitialized)
      return SetStatus(CInternetConnectStatus::statusInitFailed);

   // If using Dialup Networking, check if already connected since
   // connection check is quick through RAS API
   if(m_SocketSettings.GetConnectMethod() == CMySocketSettings::cmDialupNetworking)
   {
      CInternetConnectStatus::Status   enSaveStatus = GetStatus();
      if(IsConnected(FALSE))
      {
         SendClientMessage();
      	return GetStatus();
      }
      // If Connect Failed, restore previous status since we may be waiting to
      // connect via a timer
      SetStatus(enSaveStatus,FALSE);
      // If made one attempt to connect through Winsock, then use RAS from now
      // on to determine if a connection has been established.
      if(WaitingForConnect())
         return GetStatus();
   }

   // If first attempt to connect (not re-attempt as a result of timer)
   // then init object state
   if(!WaitingForConnect())
      InitConnectState();

   // If already waiting for Winsock connect response, don't try another connect
   if(IsRequestPending())
      return GetStatus();

   // If connect method is using an external application (such as AOL)
   // to make connection that doesn't support dialup networking
   // or using a direct connection,
   // then we need to disable Autodial (if installed) so that
   // Windows Autodial Connect Dialog doesn't appear
   if(m_SocketSettings.GetConnectMethod() != CMySocketSettings::cmDialupNetworking)
   {
      if(IsAutodialEnabled())
      {
         BOOL bPreviousAutodialState = EnableAutodial(FALSE);
         // If previously disabled, don't update original state
         if(!m_bSetAutodial)
         {
            m_bOriginalAutodialState = bPreviousAutodialState;
            m_bSetAutodial = TRUE;
         }
      }
   }
   else
   {
      // Since we want to use dialup networking, enable autodial
      if(!IsAutodialEnabled())
      {
         BOOL bPreviousAutodialState = EnableAutodial(TRUE);
         // If previously disabled, don't update original state
         if(!m_bSetAutodial)
         {
            m_bOriginalAutodialState = bPreviousAutodialState;
            m_bSetAutodial = TRUE;
         }
      }
   }

   // TODO-Allow client to specify URL
   BOOL  bStartConnecting = TRUE;
   // If client is using an external app. to connect and specified a delay
   // interval before first connect check, then wait under specified time
   // elapses before we start initiating connect check

   // Note: A delay interval is required when external apps such as AOL may
   // change the Autodial setting on startup which could interfere with
   // this connect check causing the Connect dialog to appear when it shouldn't
   if(m_SocketSettings.GetConnectMethod() == CMySocketSettings::cmDialupExternalApp &&
      m_SocketSettings.GetFirstCheck() > 0)
   {
      // Since bStartConnecting defaults to true, no need to set it to TRUE
      if(GetTimeElapsed() < m_SocketSettings.GetFirstCheck())
         bStartConnecting = FALSE;  // Wait
      else
         bStartConnecting = TRUE;
   }
   // Set Status without sending message to client if GetHostByName succeeds
   // If it's O.K. to actually start checking connection, initiate it
   SetStatus(CInternetConnectStatus::statusWaitingForConnect,FALSE);
   if(bStartConnecting)
   {
#ifdef USE_ASYNC_CONNECT
      if(!GetHostByName((LPCSTR)m_csURL))
      {
         // Failure to do host name lookup
         ASSERT(0);
         SetStatus(CInternetConnectStatus::statusWinsockError);
         Close();
      }
#else
      BOOL bSuccess = m_csocketConnection.Create();
      ASSERT(bSuccess);
      if(!bSuccess)
      {
         SetStatus(CInternetConnectStatus::statusWinsockError);
         Close();
      }
      m_pConnectThread = new CSocketHelperThread(m_pSocketWnd->GetSafeHwnd(),WM_SOCKET_NOTIFY);

      m_pConnectThread->SetSocket(m_csocketConnection.m_hSocket);
      m_pConnectThread->CreateThread(); // Run The Connect Thread
#endif
   }
   // If Timer is in not running and no errors occurred, start a new timer
   if(!IsTimerActive() && GetStatus() == CInternetConnectStatus::statusWaitingForConnect)
      StartTimer();

	return GetStatus();
}

BOOL CMySocket::IsConnected(BOOL bNotifyClient)
{
   if(!m_bInitialized)
      return SetStatus(CInternetConnectStatus::statusInitFailed);

   // Don't allow re-entry
   if(m_bCheckingIfConnected)
      return (GetStatus() == CInternetConnectStatus::statusConnected);

   m_bCheckingIfConnected = TRUE;
   if(m_SocketSettings.GetConnectMethod() == CMySocketSettings::cmDialupNetworking)
   {
      CDialupConnections   dialups;
      // Set status with message possibly sent to client
      BOOL bIsConnected = dialups.IsConnected();
      if(bIsConnected)
         SetStatus(CInternetConnectStatus::statusConnected, bNotifyClient);
      else if(!WaitingForConnect())
         SetStatus(CInternetConnectStatus::statusConnectFailed, bNotifyClient);
   }
   else if(!WaitingForConnect())
   {
      SetStatus(CInternetConnectStatus::statusWaitingForConnect, FALSE);
      // Disable Clients window

      CWnd *pParent = CWnd::FromHandle(m_SocketSettings.GetParent());
      ASSERT(pParent);
      if(pParent)
         pParent->EnableWindow(FALSE);

      // Save current socket settings
      CMySocketSettings    socketSettingsClient = m_SocketSettings;

      // Setup settings specific to this connect attempt
      // Inherit Callers settings
      CMySocketSettings    socketSettingsIsConnected = m_SocketSettings;
      socketSettingsIsConnected.SetTimeOut(4 * TIMER_SECOND);
      socketSettingsIsConnected.SetTimerInterval(1 * TIMER_SECOND);
      socketSettingsIsConnected.SetFirstCheck(0);

      m_SocketSettings = socketSettingsIsConnected;

      // Start Connection process
      Connect();

      // For non-dialup networking type connections,
      // Setup short timer for connection attempt
      // and wait for completion (simulate blocking)
      // TODO-Get this to work!
//      HCURSOR hCursor = LoadCursor(NULL,IDC_WAIT);
//      HCURSOR hOldCursor = ::SetCursor(hCursor);

      // Wait for connect or timeout
      while(GetStatus() == CInternetConnectStatus::statusWaitingForConnect)
      {
         // Process Messages
         Util::YieldToWindows();
      }

      // Restore clients settings
      m_SocketSettings = socketSettingsClient;

      // Re-enable Clients window
      pParent = CWnd::FromHandle(m_SocketSettings.GetParent());
      if(pParent)
         pParent->EnableWindow(TRUE);
//      ::SetCursor(hOldCursor);
   }
   m_bCheckingIfConnected = FALSE;
   return (GetStatus() == CInternetConnectStatus::statusConnected);
}

// user close routine
void CMySocket::Close()
{
   BOOL  bWaitingForConnect = WaitingForConnect();

   StopConnecting();

   if(IsRequestPending())
      WSACancelAsyncRequest(m_hAsyncTaskHandle);
//   if(m_bEstablishedSocketConnection)
   if(m_Socket != INVALID_SOCKET)
   {
	   if(WSAIsBlocking())
		   WSACancelBlockingCall();
	   shutdown(m_Socket,  2); // SD_BOTH
	   closesocket(m_Socket);
   }
   if(m_pSocketWnd)
   {
      m_pSocketWnd->DestroyWindow();
      delete m_pSocketWnd;
      m_pSocketWnd = NULL;
   }

   // If Client is canceling connect request, send back message
   if(bWaitingForConnect)
      SetStatus(CInternetConnectStatus::statusUserAbort);

   InitConnectState();

   return;
}

BOOL CMySocket::IsRequestPending()
{
#ifdef USE_ASYNC_CONNECT
   return m_hAsyncTaskHandle != 0;
#else
   return m_csocketConnection.m_hSocket != INVALID_SOCKET;
#endif
}

////////////////////////////////////////////////////////////////////
// Helper Functions
////////////////////////////////////////////////////////////////////
void CMySocket::SendClientMessage()
{
   if(m_SocketSettings.GetCompleteMessageID())
	   ::SendMessage(m_SocketSettings.GetParent(), m_SocketSettings.GetCompleteMessageID(), 0, GetStatus());
}

BOOL CMySocket::StartTimer()
{
   ASSERT(!IsTimerActive());
   if(IsTimerActive())
      return TRUE;

   m_pTimerThread = new CTimerThread; 

   m_bTimerIsActive = TRUE;

   // TODO-Check for Timer Creation Failure
   m_pTimerThread->Init(
      m_SocketSettings.GetTimeOut(), 
      m_SocketSettings.GetTimerInterval(), 
      m_pSocketWnd->GetSafeHwnd(), 
      WM_TIMER_NOTIFY);
   m_pTimerThread->CreateThread();

   return TRUE;
}

BOOL CMySocket::StopConnecting()
{
   StopTimer();
#ifndef USE_ASYNC_CONNECT
   KillConnectThread();
#endif
   return TRUE;
}

BOOL CMySocket::KillConnectThread()
{
   if(m_pConnectThread == NULL)
      return TRUE;

   m_pConnectThread->KillThread();
   // Wait for thread to signal that its been destroyed
   WaitForSingleObject(m_pConnectThread->GetEventDead(), INFINITE);
   delete m_pConnectThread;
   m_pConnectThread = NULL;
   m_csocketConnection.Close();

   return TRUE;
}

BOOL CMySocket::StopTimer()
{
   if(m_pTimerThread == NULL)
      return TRUE;

   m_pTimerThread->KillThread();
   // Wait for thread to signal that its been destroyed
	WaitForSingleObject(m_pTimerThread->GetEventDead(), INFINITE);
   delete m_pTimerThread;
   m_pTimerThread = NULL;
   m_bTimerIsActive = FALSE;

   return TRUE;
}

CInternetConnectStatus::Status CMySocket::SetStatus(CInternetConnectStatus::Status enStatus, BOOL bSendClientMessage)
{ 
   m_enStatus = enStatus; 
   if(bSendClientMessage)
      SendClientMessage();
   return m_enStatus;
}

void CMySocket::OnWinsockNotify(int nError)
{
   // If some other event changed connect status (such as TimeOut), ignore this message
   if(GetStatus() != CInternetConnectStatus::statusWaitingForConnect)
      return;

   // If Host Name lookup was successful, proceed to verify if connected
   if(nError == 0)
   {
#ifdef USE_ASYNC_CONNECT
      BOOL bSuccess = (FinishConnect() == 0);
#else
      BOOL bSuccess=TRUE;
#endif
      // Allow Retry if connect failed and time is active
      if(IsTimerActive()  && !bSuccess)
      {
#ifdef USE_ASYNC_CONNECT
         if(m_Socket != INVALID_SOCKET)
            closesocket(m_Socket);
         m_hAsyncTaskHandle = 0;
#else
#endif
         return;
      }

      StopConnecting();

      // Double Check if we are connected when using dialup networking
      // since Winsock doesn't come back with an error when
      // user successfully connect once, then re-attempts connection
      // and cancels out of dialup dialog
      if(m_SocketSettings.GetConnectMethod() == CMySocketSettings::cmDialupNetworking)
      {
         if(IsConnected(FALSE))
            SetStatus(CInternetConnectStatus::statusConnected, FALSE);
         else
            SetStatus(CInternetConnectStatus::statusConnectFailed, FALSE);
      }
      else
         SetStatus(CInternetConnectStatus::statusConnected, FALSE);
      SendClientMessage();
   }
   else
   {
      // If Timer Active, allow connect retry on next timer event in OnTimerNotify()
      if(!IsTimerActive())
      {
         // Set status with message sent to client
         SetStatus(CInternetConnectStatus::statusConnectFailed);
      }
   }
   if(IsRequestPending())
      WSACancelAsyncRequest(m_hAsyncTaskHandle);
   m_hAsyncTaskHandle = 0;

   // If connection status is now known, cleanup and shutdown
   if(GetStatus() != CInternetConnectStatus::statusWaitingForConnect)
      Close();
}

void CMySocket::OnTimerNotify(WPARAM wParam, LPARAM lParam)
{
   // If already waiting for connect reply, wait until next timer event 
   // to determine if we should try connecting again
   if(GetStatus() != CInternetConnectStatus::statusWaitingForConnect)
      return;

   m_dwTimeElapsed = lParam;
   switch(wParam)
   {
      case CTimerThread::tsSuccess:
         // Try Connecting Again
         Connect();
         break;
      case CTimerThread::tsTimerExpired:
         StopConnecting();
         SetStatus(CInternetConnectStatus::statusTimedOut);
         break;
      case CTimerThread::tsUserAbort:
         // If this object initiated timer thread kill, ignore message
         break;
      default:
         ASSERT(0);
         StopConnecting();
         SetStatus(CInternetConnectStatus::statusTimerFailed);
         break;
   }
}

BOOL CMySocket::EnableAutodial(BOOL bEnable)
   {
      BOOL bPrevAutodialState = FALSE;

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

BOOL CMySocket::IsAutodialEnabled()
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

void CMySocket::RestoreAutodial()
   {
      // Restore original Autodial state
      if(m_bSetAutodial)
         {
            EnableAutodial(m_bOriginalAutodialState);
            m_bSetAutodial = FALSE;
         }
   }     

int CMySocket::GetHostByName(LPCSTR szAddress,LPCSTR szServiceName/*=HTTP*/,LPCSTR szProtocol/*=TCP*/)
{
	int      nIndex;
   CString  csAddress = szAddress;
   UINT     uiHttpPort = 80;

   // Check if port was specified in address
	nIndex = csAddress.Find(":");
	if (nIndex >= 0)
	{
      CString  csPortAddress;

      // Port Address follows colon
      csPortAddress = csAddress.Mid(nIndex + 2);
		uiHttpPort = atoi((LPCSTR)csPortAddress);
      
      // Remove ":port" from string
      csAddress.GetBufferSetLength(nIndex);
	}

	memset((char *)&m_saSockAddr,0,sizeof(m_saSockAddr));
	m_saSockAddr.sin_family=AF_INET;

	SERVENT FAR *pServiceEntry;   // pointer to service entry
   // retrieve service information corresponding to a service name and protocol.
	if(pServiceEntry=getservbyname(szServiceName,szProtocol))
	{
      // If Caller requesting connection to port other than HTTP, 
      // convert callers port number to TCP/IP numeric value
		if (uiHttpPort != 80)
         // convert a u_short from host to TCP/IP network byte order (which is big-endian).
			m_saSockAddr.sin_port=htons((u_short)uiHttpPort);
		else 
         m_saSockAddr.sin_port=pServiceEntry->s_port;   // Use TCP/IP port value from SERVENT
	}
	else if((m_saSockAddr.sin_port=htons((u_short)atoi(szServiceName)))==0)
	{
		if (uiHttpPort != 80)
			m_saSockAddr.sin_port=htons((u_short)uiHttpPort);
		else
		{
			if(strcmpi(szServiceName,"HTTP")==0)
				m_saSockAddr.sin_port=htons(80);
			else
			{
				m_nLastError = WSAGetLastError();
            return m_nLastError;
			}
		}
	}
	
   // If first character is Alphabetic (not numeric), assume client supplied 
   // Host Name
	if (isalpha(szAddress[0]))
		m_hAsyncTaskHandle = WSAAsyncGetHostByName(
         m_pSocketWnd->GetSafeHwnd(),        // Window to receive notification
         (unsigned int)WM_SOCKET_NOTIFY,     // Message to send to window on completion
         (LPCSTR)csAddress,
	 		(char FAR *)m_chaHostEntry,
         MAXGETHOSTSTRUCT);
	else
		m_hAsyncTaskHandle  = WSAAsyncGetHostByAddr(
         m_pSocketWnd->GetSafeHwnd(),        // Window to receive notification
         (unsigned int)WM_SOCKET_NOTIFY,     // Message to send to window on completion
         (LPCSTR)csAddress,
	 		4,                                  // Always 4 for Internet Addresses
         PF_INET,
         (char FAR *)m_chaHostEntry,
         MAXGETHOSTSTRUCT);

   if(m_hAsyncTaskHandle == 0)
      m_nWinsockError = WSAGetLastError();
	return IsRequestPending();
}

/////////////////////////////////////////////////////////////////////////////
// CMySocketWnd implementation

CMySocketWnd::CMySocketWnd(CMySocket *pSocket)
{
   ASSERT(pSocket);  // Must not be NULL
   m_bReceivedNotify = FALSE;
   m_nError = 0;
   m_pSocket = pSocket;
}

/////////////////////////////////////////////////////////////////////////////
// Message table implementation

BEGIN_MESSAGE_MAP(CMySocketWnd, CWnd)
	//{{AFX_MSG_MAP(CWnd)
	ON_MESSAGE(WM_SOCKET_NOTIFY, OnSocketNotify)
	ON_MESSAGE(WM_TIMER_NOTIFY, OnTimerNotify)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Message Handlers
LRESULT CMySocketWnd::OnSocketNotify(WPARAM wParam, LPARAM lParam)
{
   m_nError = HIWORD(lParam);
   m_bReceivedNotify = TRUE;
   if(m_pSocket)
      m_pSocket->OnWinsockNotify(m_nError);

	return 0L;
}

LRESULT CMySocketWnd::OnTimerNotify(WPARAM wParam, LPARAM lParam)
{
   if(m_pSocket)
      m_pSocket->OnTimerNotify(wParam, lParam);

	return 0L;
}


// Integrate the code below when an actual SOCKET connection is needed

int CMySocket::FinishConnect(LPCSTR szHost, LPCSTR szProtocol /* =tcp */)
{
	int iErr = 0;

   ASSERT(m_hAsyncTaskHandle != 0);
   if(m_hAsyncTaskHandle == 0)
      return -1;

	if (strlen(m_chaHostEntry))
	{
		memcpy((char *)&m_saSockAddr.sin_addr,((struct hostent *)m_chaHostEntry)->h_addr, ((struct hostent *)m_chaHostEntry)->h_length);
	}
	else
	{
		if ((m_saSockAddr.sin_addr.s_addr=inet_addr(szHost)) == INADDR_NONE)
		{
			iErr = WSAGetLastError();
			if (iErr == 0)
				return -1;
			else
				return iErr;
		}
	}
	struct protoent FAR *pProtoEntry;    // pointer to protocol entry
	short pproto;
	if((pProtoEntry=getprotobyname(szProtocol))==0)
	{
		if(strcmpi(szProtocol,"TCP")==0)
			pproto=6;
		else
		{
			iErr = WSAGetLastError();
			if (iErr == 0)
				return -1;
			else
				return iErr;
		}
	}
	else
		pproto=pProtoEntry->p_proto;

	// Compute the socket type.
	int nSocketType;                 // socket type
	if(strcmp(szProtocol,"udp")==0)
		nSocketType=SOCK_DGRAM;
	else
		nSocketType=SOCK_STREAM;

	// Allocate a new socket.
	SOCKET sSocket=socket(AF_INET, nSocketType, pproto);
	if (sSocket==INVALID_SOCKET) 
	{
		iErr = WSAGetLastError();
		if (iErr == 0)
			return -1;
		else
			return iErr;
	}

	int iFlag=1;
	if (setsockopt(sSocket,SOL_SOCKET,SO_REUSEADDR,
         (char *)&iFlag,sizeof(iFlag)) == SOCKET_ERROR)
	{
		iErr = WSAGetLastError();
		closesocket(sSocket);
		if (iErr == 0)
			return -1;
		else
			return iErr;
	}
   
	if (connect(sSocket,(struct sockaddr *)&m_saSockAddr,sizeof(m_saSockAddr)) == SOCKET_ERROR)
	{
		iErr = WSAGetLastError();
		closesocket(sSocket);
		if (iErr == 0)
			return -1;
		else
			return iErr;
	}

	// get information about local end of the connection
	int iLength = sizeof (m_saCtrlAddr);
	
	iFlag=1;
	if (getsockname(sSocket,(struct sockaddr *)&m_saCtrlAddr, &iLength) == SOCKET_ERROR
		 || setsockopt(sSocket, SOL_SOCKET, SO_OOBINLINE, (LPSTR)&iFlag, sizeof(iFlag)) == SOCKET_ERROR)
	{
		iErr = WSAGetLastError();
		closesocket(sSocket);
		if (iErr == 0)
			return -1;
		else
			return iErr;
	}

	// Return the socket we opened with no error.
	m_Socket = sSocket;
	return 0;
} 
