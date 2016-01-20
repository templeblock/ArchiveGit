// MySocket.h: interface for the CMySocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYSOCKET_H__9091CFA2_5779_11D2_8BEF_00A0C9B12C3D__INCLUDED_)
#define AFX_MYSOCKET_H__9091CFA2_5779_11D2_8BEF_00A0C9B12C3D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include <winsock2.h>
#include <AfxSock.h>
#include "INetConnStat.h"
#include "TimerThread.h"
#include "SocketHelperThread.h"

#define  TIMER_SECOND      1000
#define  TIMER_MINUTE      TIMER_SECOND * 60

class CMySocketWnd;

class CMySocketSettings
{
public:
   enum    ConnectMethod
   {
      cmDialupNetworking,  // Connecting using Dialup Networking
      cmDialupExternalApp, // Using external app. to connect that doesn't use Windows Dialup Networking
      cmDirect             // Using a direct connection 
   };

// Construction
   CMySocketSettings();
   CMySocketSettings(
      HWND           hParent, 
      ConnectMethod  enConnectMethod,
      UINT           uiCompleteMessageID = 0,
      DWORD          dwTimeOut = TIMER_SECOND * 60,
      DWORD          dwTimerInterval = TIMER_SECOND * 1,
      DWORD          dwFirstCheck = TIMER_SECOND * 10,
      UINT           uiTimerID = 1000);
   ~CMySocketSettings();

// Initialization
   void Init(
      HWND           hParent, 
      ConnectMethod  enConnectMethod,
      UINT           uiCompleteMessageID = 0,
      DWORD          dwTimeOut = TIMER_MINUTE * 2,
      DWORD          dwTimerInterval = TIMER_SECOND * 1,
      DWORD          dwFirstCheck = TIMER_SECOND * 10,
      UINT           uiTimerID = 1000);

// Operations
   ConnectMethod     GetConnectMethod();
   HWND              GetParent();
   UINT              GetCompleteMessageID();
   DWORD             GetTimeOut();
   DWORD             GetTimerInterval();
   // GetFirstCheck() returns the time interval before connect attempt should be tried
   // This setting only applies to cmDialupExternalApp type connections
   DWORD             GetFirstCheck();
   UINT              GetTimerID();

   void              SetConnectMethod(CMySocketSettings::ConnectMethod enConnectMethod);
   void              SetParent(HWND hParent);
   void              SetCompleteMessageID(UINT uiID);
   void              SetTimeOut(DWORD dwTimeOut);
   void              SetTimerInterval(DWORD dwTimerInterval);
   // SetFirstCheck() specifies a time interval before connect attempt should be tried
   // This setting only applies to cmDialupExternalApp type connections
   void              SetFirstCheck(DWORD dwFirstCheck);
   void              SetTimerID(UINT uiTimerID);
protected:
   HWND                    m_hwndParent;
   ConnectMethod           m_enConnectMethod;
   UINT                    m_uiCompleteMessageID;
   DWORD                   m_dwTimeOut;
   DWORD                   m_dwTimerInterval;
   DWORD                   m_dwFirstCheck;
   UINT                    m_uiTimerID;
};

class CMySocket  
{
public:
// Construction
	CMySocket();
	virtual ~CMySocket();
   // Call to Init is mandatory
   CInternetConnectStatus::Status   Init(CMySocketSettings &refSettings);

// Operations
   // Connect starts asynchronous connection (non-blocking)
   // Caller can be notified with message
   CInternetConnectStatus::Status   Connect();
   void                             Close();
   void                             RestoreAutodial(void);

   // Set operations

   // SetAutoAutodialRestore() indicates whether this object should
   // restore Autodial back to its original setting upon destruction
   void                             SetAutoAutodialRestore(BOOL bAutoRestore=TRUE);
   void                             SetURL(LPCTSTR szURL);

   // Information functions
   DWORD                            GetTimeElapsed(void);
	BOOL                             IsConnected(BOOL bNotify=TRUE);

   // Status Members
   CInternetConnectStatus::Status   GetStatus(void);
   BOOL                             WaitingForConnect(void);

   // Error
   int                              GetWinsockError();

// Helpers
protected:
   // Startup
   void     InitConnectState(void);
   BOOL     StartTimer();
   BOOL     StopTimer();
   BOOL     GetHostByName(LPCSTR szAddress,LPCSTR szServiceName="http",LPCSTR szProtocol="tcp");
   BOOL     StopConnecting();
   BOOL     KillConnectThread();
   // Information
   BOOL     IsRequestPending(void);
   BOOL     IsTimerActive(void);
   BOOL     IsAutodialEnabled(void);
   // operations
   CInternetConnectStatus::Status SetStatus(CInternetConnectStatus::Status enStatus, BOOL bSendClientMessage=TRUE);
   BOOL     EnableAutodial(BOOL bEnable);
   int      FinishConnect(LPCSTR szHost=NULL, LPCSTR szProtocol ="tcp");
   // Notification
   void     OnWinsockNotify(int nError);
   void     OnTimerNotify(WPARAM wParam, LPARAM lParam);
   void     SendClientMessage(void);

   friend   CMySocketWnd;

// Data Members
protected:
   // Init (from client)
   CMySocketSettings       m_SocketSettings;
   // State members
   BOOL                    m_bInitialized;
   BOOL                    m_bEstablishedSocketConnection;
   BOOL                    m_bTimerIsActive;
   BOOL                    m_bCheckingIfConnected;
   BOOL                    m_bSetAutodial;
   BOOL                    m_bAutodialRestore;
   BOOL                    m_bOriginalAutodialState;
   BOOL                    m_bSettingUpWinsock;
   CString                 m_csURL;
   // Winsock API data
   SOCKADDR_IN             m_saSockAddr;         // endpoint address
   SOCKADDR_IN             m_saCtrlAddr;
   SOCKET                  m_Socket;
   int                     m_nLastError;
   char                    m_chaHostEntry[MAXGETHOSTSTRUCT];
   HANDLE                  m_hAsyncTaskHandle;
   // Status members
   CInternetConnectStatus::Status   m_enStatus;
   int                     m_nWinsockError;
   DWORD                   m_dwTimeElapsed;
   // Newed up stuff
   CSocket                 m_csocketConnection;
   CMySocketWnd            *m_pSocketWnd;
   CTimerThread            *m_pTimerThread;
   CSocketHelperThread     *m_pConnectThread;
};

// CMySocketWnd is an invisible window that is used to receive messages from Winsock
class CMySocketWnd : public CWnd
{
// Construction
public:
	CMySocketWnd(CMySocket *pSocket);

// Operations
   int   GetWinsockError();

protected:
	//{{AFX_MSG(CMySocketWnd)
	LRESULT OnSocketNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnTimerNotify(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Helpers
protected:

// Data Members
protected:
      CMySocket               *m_pSocket;
      BOOL                    m_bReceivedNotify;
      int                     m_nError;
};

////////////////////////////////////////////////////////////////
// Inline Section

// CMySocketSettings inline methods
inline CMySocketSettings::ConnectMethod  CMySocketSettings::GetConnectMethod()
{ return m_enConnectMethod; }

inline HWND CMySocketSettings::GetParent()
{ return m_hwndParent; }

inline UINT CMySocketSettings::GetCompleteMessageID()
{ return m_uiCompleteMessageID; }

inline DWORD CMySocketSettings::GetTimeOut()
{ return m_dwTimeOut; }

inline DWORD CMySocketSettings::GetTimerInterval()
{ return m_dwTimerInterval; }

inline UINT CMySocketSettings::GetTimerID()
{ return m_uiTimerID; }

inline DWORD CMySocketSettings::GetFirstCheck(void)
{ return m_dwFirstCheck; }

inline void CMySocketSettings::SetConnectMethod(CMySocketSettings::ConnectMethod enConnectMethod)
{ m_enConnectMethod = enConnectMethod; }

inline void CMySocketSettings::SetParent(HWND hwndParent)
{ m_hwndParent = hwndParent; }

inline void CMySocketSettings::SetCompleteMessageID(UINT uiMessageID)
{ m_uiCompleteMessageID = uiMessageID; }

inline void CMySocketSettings::SetTimeOut(DWORD dwTimeOut)
{ m_dwTimeOut = dwTimeOut; }

inline void CMySocketSettings::SetTimerInterval(DWORD dwTimerInterval)
{ m_dwTimerInterval = dwTimerInterval; }

inline void CMySocketSettings::SetFirstCheck(DWORD dwFirstCheck)
{ m_dwFirstCheck = dwFirstCheck; }

inline void CMySocketSettings::SetTimerID(UINT uiTimerID)
{ m_uiTimerID = uiTimerID; }

// CMySocket inline methods
inline CInternetConnectStatus::Status CMySocket::GetStatus(void)
{ return m_enStatus; }

inline BOOL CMySocket::WaitingForConnect(void)
{ return (IsTimerActive() && GetStatus() == CInternetConnectStatus::statusWaitingForConnect); }

inline int CMySocket::GetWinsockError(void)
{ return m_nWinsockError; }

inline DWORD CMySocket::GetTimeElapsed(void)
{ return m_dwTimeElapsed; }

inline void CMySocket::SetAutoAutodialRestore(BOOL bAutodialRestore)
{ m_bAutodialRestore = bAutodialRestore; }

inline void CMySocket::SetURL(LPCTSTR szURL)
{
   m_csURL = szURL;
}


// CMySocketWnd inline methods
inline int CMySocketWnd::GetWinsockError(void)
{ return m_nError; }

#endif // !defined(AFX_MYSOCKET_H__9091CFA2_5779_11D2_8BEF_00A0C9B12C3D__INCLUDED_)
