/***************************************************************************
*  FILE:        INETCONN.H                                                 *
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
***************************************************************************/
#ifndef _INETCONN_INC
#define _INETCONN_INC

#define  INETCONN_SECOND   1000
#define  INETCONN_MINUTE   INETCONN_SECOND * 60

#include <ras.h>     // Remote Access Service functions
#include "inetconnstat.h"
#include "MySocket.h"

// This class is used to wait for an Internet connection to occur
class CInternetConnect
   {
   public:
   // Construction
      CInternetConnect();
      virtual ~CInternetConnect();

   // Operations
      const    CInternetConnectStatus::Status GetStatus();
      DWORD    GetTimeElapsed();
      DWORD    GetTimerInterval();
      void     RestoreAutodial();

   // Manditory Operations
      // Connect() defaults to attempting to connect without using a timer
      // If Timer ID is 0, then no timer is used.
      CInternetConnectStatus::Status Connect(
                  HWND hwndParent,
                  BOOL bWatchForDialup = FALSE, 
                  UINT uiTimerID = 0,
                  DWORD dwTimeOut = INETCONN_MINUTE * 2,
                  DWORD dwFirstCheck = INETCONN_SECOND * 10,
                  DWORD dwTimerInterval = INETCONN_SECOND * 1);

   // Optional Operations
      BOOL     Abort(void);
      BOOL     IsConnected(CMySocketSettings::ConnectMethod connectMethod, CWnd *pParent);
      void     SetURL(LPCTSTR szURL);
   protected:
      CMySocketSettings    m_socketSettings;
      CMySocket            m_MySocket;
   };

// Inline Section
inline void CInternetConnect::SetURL(LPCTSTR szURL)
{
   m_MySocket.SetURL(szURL);
}

#endif 

