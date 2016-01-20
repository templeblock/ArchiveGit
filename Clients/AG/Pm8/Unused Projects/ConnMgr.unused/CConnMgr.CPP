// $Workfile: CConnMgr.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:14p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
//
// $Log: /PM8/ConnMgr/CConnMgr.CPP $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 3     10/21/98 2:39p Dennis
// Control now sets apps main window to parent.  
// 
// 2     10/21/98 12:36p Dennis
// 
// 1     10/19/98 11:18a Dennis
// 
// 19    8/29/98 8:24a Dennis
// Changes for AOL 4.0 (hidden file detection and Autodial disable)
// 
// 18    7/21/98 3:26p Dennis
// More Detailed connection failure message
// 
// 17    7/17/98 1:48p Dennis
// Connection settings always updated on Connect call.   Top level window
// disabled before Connection attempt.
// 
// 16    7/16/98 5:16p Dennis
// AOL Fixes
// 
// 15    7/16/98 2:57p Dennis
// AOL Fixes (when user exits AOL and only show confirm if app not
// running).
// 
// 14    7/15/98 12:08p Dennis
// Added Online Program confirm dialog.  Added delay for launching AOL
// program.
// 
// 13    7/14/98 11:03a Jayn
// INI stuff
// 
// 12    7/12/98 5:31p Jayn
// Improved graphic download. Cleanup of code in general.
// 
// 11    5/26/98 6:37p Psasse
// Helpful hints sound support
// 
// 10    5/26/98 1:57p Dennis
// Added call to Configuration Manager for replacing app. text.
// 
// 9     5/16/98 9:19p Psasse
// fix multiple creations of "attempt to connect" dialog
// 
// 8     5/09/98 1:55p Psasse
// added a 2 second delay until progress bar for connecting to internet is
// shown
// 
// 7     4/29/98 9:47p Psasse
// 
// 6     3/02/98 5:04p Johno
// Compile update
// 
// 5     2/27/98 1:53p Johno
// Compile update

#include "stdafx.h"
#include "resource.h"
#include "progbar.h"
#include "cconnmgr.h"
#include "util.h"
#include "procloc.h"
#include "mmsystem.h"
#include "prefonl.h"

CConnectionManager::CConnectionManager()
   {
      m_hwndOwner = NULL;
      m_hwndParent = NULL;
      EnableIsConnectedCheck(TRUE);
      EnableHangUpPrompt(TRUE);
   }

CConnectionManager::~CConnectionManager()
   {
   }

CInternetConnectStatus::Status CConnectionManager::Connect()
   {
      if(m_connectionSettings.GetType() == CConnectionSettings::typeManual)
				return CInternetConnectStatus::statusConnected;

		BOOL                 bFoundProcess, bCreated = FALSE;
      DWORD                dwTimerID, dwTimeOut, dwFirstCheck, dwRange = 2;
      DWORD                dwTimerInterval = INETCONN_SECOND;
      CString              csMessage, csResource;
      CProcessLocater      processLocater;

      dwFirstCheck = dwTimeOut = dwTimerID = 0;

      // Make sure connection settings are up to date while getting them
      if(!m_connectionSettings.Update())
      {
         return CInternetConnectStatus::statusInvalidSettings;
      }

      // Check if connected first   
      if(IsIsConnectedCheckEnabled())
      {
         if(IsConnected())
            return m_MySocket.GetStatus();
      }

      if(GetParent() == NULL || GetOwner() == NULL)
         return CInternetConnectStatus::statusOtherError;

      bCreated = InitializeProgressControl(csMessage, 1);
      ASSERT(bCreated);
      if(!bCreated)
         return CInternetConnectStatus::statusOtherError;

      BOOL		bBringWindowToTop=FALSE;

      // If connection settings require application launch, do it
      if(m_connectionSettings.GetType() != CConnectionSettings::typeModemDialup &&
         m_connectionSettings.GetType() != CConnectionSettings::typeDirect)
         {
            CString		csAppPath;
            int         nResult;
            // Get Confirm setting from Online Settings
            COnlineProgramConfirmStart dlgConfirm;

            Util::SplitPath(m_connectionSettings.GetAppFileName(), &csAppPath, NULL);
            bFoundProcess = processLocater.Find(
               m_connectionSettings.GetAppFileName(),
               FALSE,
               1);

            if( !bFoundProcess )
            {
               nResult = dlgConfirm.DoModal();
               if(nResult != IDOK)
               {
                  m_progressDialog.DestroyWindow();
                  return CInternetConnectStatus::statusUserAbort;
               }

               // Program Window will be brought to top be ShellExecute call
               bBringWindowToTop = TRUE;

               dwFirstCheck = m_connectionSettings.GetFirstCheck() * INETCONN_SECOND;
               HINSTANCE hInst = ShellExecute(
                  NULL,
                  "open",
                  m_connectionSettings.GetAppFileName(),
                  NULL,
                  csAppPath,
                  SW_SHOWNORMAL);
               if(hInst <= (HINSTANCE)32)
               {
                  csResource.LoadString(IDS_CONNMGR_APPEXE_ERROR);
                  ASSERT(!csResource.IsEmpty());
                  csMessage.Format(csResource,
                     m_connectionSettings.GetAppFileName());
                  AfxMessageBox(csMessage);
                  m_connectionSettings.Change();
                  m_progressDialog.DestroyWindow();
                  return CInternetConnectStatus::statusConnectFailed;
               }
            }
            else
               {
                  dwFirstCheck = INETCONN_SECOND * 0;
                  // Process is running but we have not brought it to foreground yet
                  bBringWindowToTop = FALSE;
               }
         }

      // Use timer for this connection type
      dwTimerID = 1000;
      dwTimeOut = m_connectionSettings.GetTimeOut() * INETCONN_SECOND;
      dwTimerInterval = m_connectionSettings.GetTimerInterval() * INETCONN_SECOND;
      if(dwTimeOut == 0)
         dwTimeOut = INETCONN_SECOND * 120;
      if(dwTimerInterval == 0 || dwTimerInterval > (INETCONN_SECOND * 10))
         dwTimerInterval = INETCONN_SECOND;


      // If using timer, compute range, otherwise use default range
      if(dwTimerInterval > 0)
         dwRange = dwTimeOut / dwTimerInterval;
      ASSERT(dwRange);
		m_progressDialog.SetRange(1,dwRange);     
      m_progressDialog.ShowWindow(SW_SHOW);

		DWORD dwStartTime = timeGetTime();

      // Disable Main Window
      ::EnableWindow(GetCurrentOwner(), FALSE);

      // Init Socket Settings
      CMySocketSettings::ConnectMethod connectMethod = CMySocketSettings::cmDialupNetworking;
      if(!m_connectionSettings.WatchForDialup())
         connectMethod = CMySocketSettings::cmDialupExternalApp;
      m_socketSettings.Init(
         GetCurrentOwner(),
         connectMethod,
         NULL,             // Status Message ID
         dwTimeOut,
         dwTimerInterval,
         dwFirstCheck);
      m_MySocket.Init(m_socketSettings);
      m_MySocket.SetAutoAutodialRestore(FALSE);

      // Establish connection
      // If Connect is called with dwTimerID == 0, then no timer is used
      // (no timer used for Direct or DialUp networking connections)
      if(m_MySocket.Connect() == CInternetConnectStatus::statusWaitingForConnect)
         {
            int   nCurPos = 1, nNewPos;

            // If connection type is a dialup and
            // we didn't bring window to foreground before connection attempt, 
            // bring it to the foreground now for user to login
            if(m_connectionSettings.GetType() != CConnectionSettings::typeModemDialup &&
               m_connectionSettings.GetType() != CConnectionSettings::typeDirect &&
               !bBringWindowToTop)
            {
               bFoundProcess = processLocater.Find(m_connectionSettings.GetAppFileName(),
                  TRUE,
                  1);
               bBringWindowToTop = TRUE;
            }

            // Loop while waiting for connect
            while(m_MySocket.GetStatus() == 
               CInternetConnectStatus::statusWaitingForConnect &&
               !m_progressDialog.CancelHit())
               {
                  Util::YieldToWindows();
                  // If connection type is America Online and
                  // user closed app., stop looking for a connection
                  if(m_connectionSettings.GetType() == CConnectionSettings::typeModemAOL)
                  {
                     bFoundProcess = processLocater.Find(
                        m_connectionSettings.GetAppFileName(),
                        FALSE,
                        1);
                     if(!bFoundProcess)
                        m_MySocket.Close();
                  }
                  if(m_socketSettings.GetTimerInterval() > 0)
                     nNewPos = m_MySocket.GetTimeElapsed() / m_socketSettings.GetTimerInterval();
                  if(nNewPos != nCurPos)
                  {
               		m_progressDialog.SetPos(nNewPos);
                     nCurPos = nNewPos;
                  }
               }
            // If user wants to abort connecting, tell CInternetConnect object to stop looking
            if(m_progressDialog.CancelHit())
               {
                  m_MySocket.Close();
               }
         }

      m_progressDialog.DestroyWindow();

      // re-enable main window
      ::EnableWindow(GetCurrentOwner(), TRUE);

		// Make sure we are on top.
      CWnd  *pOwner = CWnd::FromHandle(GetCurrentOwner());
      ASSERT(pOwner);
      if(pOwner)
      {
		   pOwner->SetForegroundWindow();
		   pOwner->BringWindowToTop();
      }

      if(m_MySocket.GetStatus() != CInternetConnectStatus::statusConnected)
         {
            int      nIDDetailedError;
            CString  csResource;

            csMessage.LoadString(IDS_CONNECT_FAILED);
            ASSERT(!csMessage.IsEmpty());

            switch(m_MySocket.GetStatus())
            {
            case CInternetConnectStatus::statusUserAbort:
               nIDDetailedError = IDS_CONNMGR_USER_ABORT;
               break;
            case CInternetConnectStatus::statusTimedOut:
               nIDDetailedError = IDS_CONNMGR_TIMEOUT;
               break;
            case CInternetConnectStatus::statusConnectFailed:
               nIDDetailedError = IDS_CONNMGR_CONNECT_FAILED;
               break;
            case CInternetConnectStatus::statusInvalidURL:
               nIDDetailedError = IDS_CONNMGR_INVALID_URL;
               break;
#if 0
            case CInternetConnectStatus::statusOtherWinINetError:
               nIDDetailedError = IDS_CONNMGR_OTHER_ERROR;
               break;
#endif
            case CInternetConnectStatus::statusTimerFailed:
               nIDDetailedError = IDS_CONNMGR_TIMER_FAILED;
               break;
            case CInternetConnectStatus::statusInvalidSettings:
               nIDDetailedError = IDS_CONNMGR_INVALID_CONFIG;
               break;
            default:
               nIDDetailedError = IDS_CONNMGR_OTHER_ERROR;
            }
            csResource.LoadString(nIDDetailedError);
            csMessage += csResource;
            AfxMessageBox(csMessage);
         }

      return m_MySocket.GetStatus();
   }

BOOL CConnectionManager::IsConnected()
{
   CMySocketSettings::ConnectMethod connectMethod = CMySocketSettings::cmDialupNetworking;

   m_connectionSettings.GetSettings();
   if(!m_connectionSettings.WatchForDialup())
      connectMethod = CMySocketSettings::cmDialupExternalApp;

   m_socketSettings.SetParent(GetOwner());
   m_socketSettings.SetConnectMethod(connectMethod);
   m_MySocket.Init(m_socketSettings);
   return m_MySocket.IsConnected();
}

void CConnectionManager::HangUp()
   {
      BOOL                 bFoundProcess;
      CProcessLocater      processLocater;
      CDialupConnections   dialups;
      CDialupConnections::ErrorCode dialupError;
      CString              csHangUp;

      if(m_MySocket.GetStatus() != CInternetConnectStatus::statusConnected ||
         m_connectionSettings.GetType() == CConnectionSettings::typeDirect ||
         m_connectionSettings.GetType() == CConnectionSettings::typeNone)
            return;

      csHangUp.LoadString(IDS_CONNMGR_HANGUP);
      ASSERT(!csHangUp.IsEmpty());
      if(m_connectionSettings.GetType() != CConnectionSettings::typeModemDialup)
         {
            if(m_connectionSettings.RestoreAutodial())
               m_MySocket.RestoreAutodial();

            bFoundProcess = processLocater.Find(
               m_connectionSettings.GetAppFileName(),
               FALSE);
            if(bFoundProcess)
               {
                  int   nRetVal = IDYES;

                  if(IsHangUpPromptEnabled())
                     nRetVal = AfxMessageBox(csHangUp, MB_YESNO);
                  if(nRetVal == IDYES)
                  {
                     processLocater.BringWindowToTop();

                     // Tell program to go away
                     processLocater.Close();
                  }
               }
         }
      else
         {
            dialupError = dialups.EnumerateConnections();
            if(dialupError == CDialupConnections::errorNone &&
               dialups.GetConnectionCount() > 0)
               {
                  int   nRetVal = IDYES;

                  if(IsHangUpPromptEnabled())
                     nRetVal = AfxMessageBox(csHangUp, MB_YESNO);
                  if(nRetVal == IDYES)
                  {
                     // Disconnect
                     dialups.HangUp();
                  }
               }
         }
   }

HWND CConnectionManager::GetOwner()
{
   // Note: pOwner should be assumed to be a temporary CWnd Object
   CWnd *pOwner = CWnd::GetSafeOwner(NULL);
   if(pOwner == NULL || pOwner->GetSafeHwnd() == NULL)
      return NULL;

   m_hwndOwner = pOwner->GetSafeHwnd();
   return m_hwndOwner;
}

HWND CConnectionManager::GetCurrentOwner()
{
   return m_hwndOwner;
}

BOOL CConnectionManager::InitializeProgressControl(CString& csMessage, DWORD dwRange)
{	  
  BOOL bCreated = FALSE;

  // Create Progress Bar control
  if(GetParent() == NULL || GetOwner() == NULL)
     return FALSE;

  bCreated = m_progressDialog.Create(IDD_PROGRESS_BAR, CWnd::FromHandle(GetOwner()));
  ASSERT(bCreated);
  if(bCreated)
	 {
		// Re-enable progress dialog
		m_progressDialog.EnableWindow(TRUE);     
		csMessage.LoadString(IDS_CONNECT_WAITING);
		ASSERT(!csMessage.IsEmpty());
		m_progressDialog.SetWindowText(csMessage);
		m_progressDialog.SetRange(1,dwRange);     
	 }
  else
	 {
		csMessage.LoadString(IDS_PROGRESS_CREATE_ERROR);
		ASSERT(!csMessage.IsEmpty());
		AfxMessageBox(csMessage);
	 }
	return bCreated;
}
