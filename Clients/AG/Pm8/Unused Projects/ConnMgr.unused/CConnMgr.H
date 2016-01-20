#ifndef _CONNMGR_INC
#define _CONNMGR_INC

#include <afxcmn.h>
#include "connset.h"
#include "progbar.h"
#include "MySocket.h"

class CConnectionManager
   {
   public:
   // Construction
      CConnectionManager();
      ~CConnectionManager();

   // Operations
		CInternetConnectStatus::Status   Connect();
		void                             HangUp(void);
      BOOL                             EditPrefs();

   // Information Operations
      BOOL                             IsConnected(void);
      BOOL                             IsIsConnectedCheckEnabled(void);
      BOOL                             IsHangUpPromptEnabled(void);
      HWND                             GetParent(void);

   // Set operations
      // Calling EnableIsConnectedCheck() with TRUE enables connections checking
      // with methods such as Connect().  You may want to disable this when
      // performing a call to IsConnected() before calling Connect() so
      // that there are not potentially multiple waits.
   	BOOL                             EnableIsConnectedCheck(BOOL bEnable);
	   BOOL                             EnableHangUpPrompt(BOOL bEnable);
      void                             SetURL(LPCTSTR szURL);
      void                             SetParent(HWND hwndParent);


   protected:
   // Helpers
		BOOL  InitializeProgressControl(CString& strMessage, DWORD dwRange);
      HWND                             GetOwner(void);
      HWND                             GetCurrentOwner();
   protected:
   // Data Members
      CConnectionSettings  m_connectionSettings;
		CPMWProgressBar      m_progressDialog;
      BOOL                 m_bEnableIsConnectedCheck;
      BOOL                 m_bEnableHangUpPrompt;
      HWND                 m_hwndParent;
      HWND                 m_hwndOwner;
      CMySocketSettings    m_socketSettings;
      CMySocket            m_MySocket;
   };

// Inline Section
inline BOOL CConnectionManager::EditPrefs()
{
   m_connectionSettings.Change();
   return TRUE;
}

inline void CConnectionManager::SetURL(LPCTSTR szURL)
{
   m_MySocket.SetURL(szURL);
}

inline BOOL CConnectionManager::IsIsConnectedCheckEnabled()
{
   return m_bEnableIsConnectedCheck;
}

inline BOOL CConnectionManager::IsHangUpPromptEnabled()
{
   return m_bEnableHangUpPrompt;
}

inline BOOL CConnectionManager::EnableIsConnectedCheck(BOOL bEnable)
{ 
   BOOL  bPreviousState = m_bEnableIsConnectedCheck;
   m_bEnableIsConnectedCheck = bEnable;
   return bPreviousState;
}

inline BOOL CConnectionManager::EnableHangUpPrompt(BOOL bEnable)
{
   BOOL  bPreviousState = m_bEnableHangUpPrompt; 
   m_bEnableHangUpPrompt = bEnable;
   return bPreviousState;
}

inline void CConnectionManager::SetParent(HWND hwndParent)
{
   m_hwndParent = hwndParent;
}

inline HWND CConnectionManager::GetParent(void)
{
   return m_hwndParent;
}


#endif   // End of _CONNMGR_INC


