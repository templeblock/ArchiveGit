/***************************************************************************
*  FILE:        CONNSET.H                                                  *
*  SUMMARY:     Classes for configuring connection settings, retrieving    *
*                 connection info.
*  AUTHOR:      Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Initials Legend:                                                        *
*     DGP       Dennis Plotzke                                             *
*  ------------------------------------------------------------------------*
*  Revision History                                                        *
*                                                                          *
*  Date     Initials  Description of Change                                *
*  ------------------------------------------------------------------------*
*  1/27/98  DGP      Created                                               *
***************************************************************************/
#ifndef _CONNSET_INC
#define _CONNSET_INC

#include "inetconn.h"
#include "dialups.h"

class CConnectionSettings
   {
   public:
   // Construction
      CConnectionSettings();
      ~CConnectionSettings();

   // Operations
      enum  Type
         {
				typeManual = -1,		// User connects. Do nothing.
            typeNone = 0,
            typeModemAOL,
            typeModemCustom,
            typeModemDialup,
            typeModemCompuServe,
            typeDirect
         };
   // Get Members
      const Type GetType() const
         { return m_enType; }
      const LPCSTR GetName() const
         { return m_csName; }
      const LPCSTR GetAppFileName() const
         { return m_csAppFileName; }
      DWORD        GetTimeOut() const
         { return m_dwTimeOut; }
      DWORD        GetFirstCheck() const
         { return m_dwFirstCheck; }
      DWORD        GetTimerInterval() const
         { return m_dwTimerInterval; }
      BOOL         WatchForDialup() const
         { return m_bWatchForDialup; }
      BOOL         RestoreAutodial() const
         { return m_bRestoreAutodial; }

   // Set Members
      void SetAppFileName(LPCSTR szAppFileName)
         { m_csAppFileName = szAppFileName; }
      void        SetTimeOut(DWORD dwTimeOut) 
         { m_dwTimeOut = dwTimeOut; }
      void        SetFirstCheck(DWORD dwFirstCheck)
         { m_dwFirstCheck = dwFirstCheck; }
      void        SetTimerInterval(DWORD dwInterval)
         { m_dwTimerInterval = dwInterval; }
      void SetType(Type enNewType);
      void SetName(LPCSTR szName)
         {
            m_csName = szName;
         }
      void         SetWatchForDialup(BOOL bWatch)
         { m_bWatchForDialup = bWatch; }
      void SetRestoreAutodial(BOOL bRestoreAutodial)
         {
            m_bRestoreAutodial = bRestoreAutodial;
         }


      void GetSettings(Type enType = typeNone);
      void SaveSettings();
      BOOL Update(BOOL bRunDialogIfNeeded=TRUE, BOOL bWarn=TRUE);
      BOOL Change();
      BOOL                 IsCompuServeInstalled()
         { return (m_CompuServeInfo.IsInstalled()); }
      BOOL                 IsAOL95Installed()
         { return (m_aolInfo.IsInstalled() && m_aolInfo.GetOSVersion() == CAOLInfo::osWin32); }
      CDialupConnections * GetDialups()
         { return &m_dialups; }

   protected:
   // Helpers
      void        InitVars();

   protected:
   //Data Members
      CString     m_csName;            // Connection Name
      CString     m_csAppFileName;     // Full path to comm. program
      Type        m_enType;            // Connection Type
      BOOL        m_bAOLInstalled;
      BOOL        m_bCompuServeInstalled;
      BOOL        m_bUpdated;          // Flag indicating whether settings
                                       // have been updated
      BOOL        m_bWatchForDialup;   // Flag to indicate how to check for connect
      BOOL        m_bRestoreAutodial;  // Flag indicating if autodial setting should be restored
      CAOLInfo::OSVersion  m_enAOLOsVersion;
      DWORD       m_dwTimeOut;         // TimeOut value
      DWORD       m_dwFirstCheck;      // Time value when to start checking for connection
      DWORD       m_dwTimerInterval;   // Timer interval
      DWORD       m_dwDialupCount;
      CAOLInfo    m_aolInfo;
      CCompuServeInfo m_CompuServeInfo;
      CDialupConnections   m_dialups;
   };
#endif

