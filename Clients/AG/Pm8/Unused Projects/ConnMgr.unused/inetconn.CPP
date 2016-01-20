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
// $Log: /PM8/ConnMgr/inetconn.CPP $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 2     10/21/98 12:36p Dennis
// 
// 1     10/19/98 11:18a Dennis
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
#include "inetconn.h"


// CInternetConnect  methods
CInternetConnect::CInternetConnect()
{
}

CInternetConnect::~CInternetConnect()
{
}

CInternetConnectStatus::Status CInternetConnect::Connect(
   HWND  hwndParent,
   BOOL bWatchForDialup,   /* FALSE */
   UINT uiTimerID,         /* = 0 */ 
   DWORD dwTimeOut,        /* = INETCONN_MINUTE * 2 */
   DWORD dwFirstCheck,     /* = INETCONN_SECOND * 10 */
   DWORD dwTimerInterval   /* = INETCONN_SECOND */ )
{
   CMySocketSettings::ConnectMethod connectMethod = CMySocketSettings::cmDialupNetworking;

   if(!bWatchForDialup)
      connectMethod = CMySocketSettings::cmDialupExternalApp;

   m_socketSettings.Init(
      hwndParent,
      connectMethod,
      NULL,
      dwTimeOut,
      dwTimerInterval,
      dwFirstCheck);
   m_MySocket.Init(m_socketSettings);
   m_MySocket.SetAutoAutodialRestore(FALSE);
   return m_MySocket.Connect();
}

BOOL CInternetConnect::Abort(void)
{
   m_MySocket.Close();
   return TRUE;
}

const  CInternetConnectStatus::Status CInternetConnect::GetStatus()
{
   return m_MySocket.GetStatus();
}

BOOL CInternetConnect::IsConnected(CMySocketSettings::ConnectMethod connectMethod, CWnd *pParent)
{
   m_socketSettings.SetParent(pParent->GetSafeHwnd());
   m_socketSettings.SetConnectMethod(connectMethod);
   m_MySocket.Init(m_socketSettings);  // Re-init
   return m_MySocket.IsConnected();
}

DWORD  CInternetConnect::GetTimeElapsed()
{
   return m_MySocket.GetTimeElapsed();
}

DWORD  CInternetConnect::GetTimerInterval()
{
   return m_socketSettings.GetTimerInterval();
}

void   CInternetConnect::RestoreAutodial()
{
   m_MySocket.RestoreAutodial();
}


