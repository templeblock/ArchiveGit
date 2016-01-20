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

class CAOLInfo
   {
   public:
   // Construction
      CAOLInfo();
      ~CAOLInfo();

   // Operations
      enum  OSVersion
         {
            osUnknown, 
            osWin16, 
            osWin32
         };
      BOOL IsInstalled();

      // GetOSVersion() returns the Operating System executable type 
      // for the program if installed
      enum OSVersion GetOSVersion();

      const CString & GetAppFileName() const;
      const CString & GetAppPath() const;
      
      void SetOSVersion(enum OSVersion enNewOSVersion);

   protected:
   // Data members
      BOOL        m_bInstalled;
      OSVersion   m_enOSVersion;
      CString     m_csAppPath;
      CString     m_csAppFileName;
   };

class CCompuServeInfo
   {
   public:
   // Construction
      CCompuServeInfo();
      ~CCompuServeInfo();

   // Operations
      BOOL IsInstalled();

      const CString & GetAppFileName() const;

   protected:
   // Data members
      BOOL        m_bInstalled;
      CString     m_csAppPath;
      CString     m_csAppFileName;
   };

typedef DWORD (APIENTRY * TDProcRasEnumEntries) ( LPSTR, LPSTR, LPRASENTRYNAME, LPDWORD, LPDWORD);
typedef DWORD (APIENTRY * TDProcRasEnumConnections) ( LPRASCONN, LPDWORD, LPDWORD );
typedef DWORD (APIENTRY * TDProcRasHangUp) ( HRASCONN );
typedef DWORD (APIENTRY * TDProcRasGetConnectStatus) (HRASCONN, LPRASCONNSTATUS);

class CDialupConnections
   {
   public:
   // Construction
      CDialupConnections();
      ~CDialupConnections();

   // Operations
      enum ErrorCode
         {
            errorNone,
            errorFail,                 // Operation failed (logical failure)
            errorLibraryNotFound,
            errorMissingFunction,
            errorRAS                   // Call GetRASError() to get error code
         };
      ErrorCode                     Init();
      void                          DeInit();

      // EnumerateEntries() retrieves the current entries in the dialup
      // networking 
      ErrorCode                     EnumerateEntries();
      const RASENTRYNAME *          GetEntries() const;
      const DWORD                   GetEntryCount() const;
      LPCSTR                        GetEntryName(DWORD dwIndex)
         {
            ASSERT(dwIndex < GetEntryCount());
            return m_preaRasEntry[dwIndex].szEntryName;
         }

      // Get get a list of connections, call EnumerateConnections() 
      // if success an Enumeration, then GetConnectionCount() and
      // use that to index into array returned by GetConnections()
      ErrorCode                     EnumerateConnections();
      DWORD                         GetConnectStatus(HRASCONN hConnection, LPRASCONNSTATUS pStatus);
      BOOL                          IsConnected(HRASCONN hConnection);
      const DWORD                   GetConnectionCount() const;
      const RASCONN *               GetConnections() const;

      ErrorCode                     HangUp(HRASCONN hConnection = 0);

      // GetRASError() returns the errorcode returned by a call to the 
      // RAS DLL.  See RAS.H for errorcodes
      const DWORD    GetRASError() const;


   protected:
   // Helpers
      void InitVariables();
      void AllocateEntries(DWORD dwCount=1);
      void FreeEntries();
      void AllocateConnections(DWORD dwConnectionCount=1);
      void FreeConnections();

   protected:
   // Data Members
      BOOL              m_bInitialized;      // Init. state
      DWORD             m_dwNumEntries;      // Number of Entry Names
      DWORD             m_dwNumConnections;  // Number of connections
      DWORD             m_dwRasError;        // ErrorCode of of RAS Error
      RASCONN *         m_prcaRasConn;       // Array of RASCONN structures
      RASENTRYNAME *    m_preaRasEntry;      // Array of RASENTRYNAME structures
      HINSTANCE         m_hRasLibrary;

      // Function pointers to RAS API from RASAPI32.DLL
      TDProcRasEnumEntries       m_pProcRasEnumEntries;
      TDProcRasEnumConnections   m_pProcRasEnumConnections;
      TDProcRasHangUp            m_pProcRasHangUp;
      TDProcRasGetConnectStatus  m_pProcRasGetConnectStatus;
   };

// CPMWInternetConnectStatus provides info. about the CInternetConnect object
class CInternetConnectStatus
   {
   public:
      // Construction
      CInternetConnectStatus()
         {
            SetStatus(statusWaitingForConnect);
         }

      // Operations
      enum Status
         {
            statusWaitingForConnect,
            statusConnected,
            statusCannotConnect,
            statusTimedOut, 
            statusInvalidURL,
            statusOtherError,
            statusOtherWinINetError,
            statusTimerFailed,
            statusInvalidSettings,
            statusUserAbort
         };
      const Status GetStatus() const
         {
            return m_enStatus;
         }
      void SetStatus(Status enStatus)
         {
            m_enStatus = enStatus;
         }
      void Reset()
         {
            SetStatus(statusWaitingForConnect);
         }
   protected:
      // Data members
      Status      m_enStatus;
   };

// This class is used to wait for an Internet connection to occur
class CInternetConnect
   {
   public:
   // Construction
      CInternetConnect();
      virtual ~CInternetConnect();

   // Operations

   // Information Operations
      const    CInternetConnectStatus::Status GetStatus();
      BOOL     IsConnected();
      BOOL     IsTimerActive();
      DWORD    GetTimeElapsed();
      DWORD    GetTimerInterval();
      DWORD    GetTimeOut();
      LPCSTR   GetURL();

   // Manditory Operations
      // Connect() defaults to attempting to connect without using a timer
      // If Timer ID is 0, then no timer is used.
      CInternetConnectStatus::Status Connect(
                  CWnd *pWnd=NULL,
                  BOOL bWatchForDialup = FALSE, 
                  UINT uiTimerID = 0,
                  DWORD dwTimeOut = INETCONN_MINUTE * 2,
                  DWORD dwFirstCheck = INETCONN_SECOND * 10,
                  DWORD dwTimerInterval = INETCONN_SECOND * 1);

   // Optional Operations
      // AbortConnect() tells object to stop looking for a connection
      // when a timer is used
      BOOL     Abort(CInternetConnectStatus::Status
         enNewStatus=CInternetConnectStatus::statusCannotConnect);

      void     SetURL(LPCSTR szURL = "http://www.printmaster.com");
      void     SetFirstCheckTime(DWORD dwFirstCheck = INETCONN_SECOND * 10);
      void     SetTimeOut(DWORD dwTimeOut = INETCONN_MINUTE * 2);
      void     SetTimerInterval(DWORD dwTimerInterval = INETCONN_SECOND * 1);
      void     RestoreAutodial();
   protected:
   // Helpers
      void     Init(void);
      CInternetConnectStatus::Status CheckConnection();
      CInternetConnectStatus::Status TryConnect();
      UINT     StartTimer(CWnd *pTimerWnd,
                  UINT uiTimerID,
                  DWORD dwTimeOut,
                  DWORD dwFirstCheck,
                  DWORD dwTimerInterval);
      BOOL     StopTimer(CInternetConnectStatus::Status
                  enNewStatus=CInternetConnectStatus::statusCannotConnect);
      BOOL     EnableAutodial(BOOL bEnable);
      BOOL     IsAutodialEnabled();

      // Static Methods
      static void CALLBACK EXPORT TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwCurTime);
   protected:
      // Data Members
      CWnd *      m_pTimerWnd;     // Window that owns timer
      BOOL        m_bActiveTimer;  // Is Timer Active?
      BOOL        m_bInTimerProc;
      BOOL        m_bWatchForDialup;   // Look for Dial-Up Networking connection
      BOOL        m_bOriginalAutodialState;
      BOOL        m_bAutodialDisabled;
      BOOL        m_bSetAutodial;   // Flag indicates if Autodial setting was changed
      UINT        m_uiTimerID;
      DWORD       m_dwStartTime;   // Time 1st call to timer occurred
      DWORD       m_dwTimeOut;     // when to TimeOut connect attempt
      DWORD       m_dwTimerInterval; // How often to check connection
      DWORD       m_dwFirstCheck;  // When to attempt first connect
      DWORD       m_dwTimeElapsed; // How much time has elapsed since
                                    // first timer call
      DWORD       m_dwWinINetError;// Contains WinINet error if any
      CString     m_csURL;         // URL to test connection with
      CString     m_csErrorMessage;// WinINet error text
      CInternetConnectStatus m_status;  // status object for polling state

      // Static Data Members
      static CMapWordToPtr sm_thisList;
   };

// Inline Section

// CAOLInfo inline methods
inline CAOLInfo::OSVersion CAOLInfo::GetOSVersion()
   {
      return m_enOSVersion;
   }

inline void CAOLInfo::SetOSVersion(enum OSVersion enNewOSVersion)
   {
      m_enOSVersion = enNewOSVersion;
   }

inline const CString & CAOLInfo::GetAppFileName() const
   {
      return m_csAppFileName;
   }

inline const CString & CAOLInfo::GetAppPath() const
   {
      return m_csAppPath;
   }

// CCompuServeInfo
inline const CString & CCompuServeInfo::GetAppFileName() const
   {
      return m_csAppFileName;
   }

// CDialupConnections inline methods
inline const DWORD CDialupConnections::GetEntryCount() const
   {
      return m_dwNumEntries;
   }

inline const DWORD CDialupConnections::GetConnectionCount() const
   {
      return m_dwNumConnections;
   }

inline const DWORD CDialupConnections::GetRASError() const
   {
      return m_dwRasError;
   }

inline const RASCONN * CDialupConnections::GetConnections() const
   {
      return m_prcaRasConn;
   }

inline const RASENTRYNAME * CDialupConnections::GetEntries() const
   {
      return m_preaRasEntry;
   }

// CInternetConnect inline methods

inline BOOL CInternetConnect::Abort(CInternetConnectStatus::Status enStatus)
   { return StopTimer(enStatus); }

inline BOOL CInternetConnect::IsTimerActive()
   { return m_bActiveTimer; }

inline DWORD CInternetConnect::GetTimeElapsed()
   { return m_dwTimeElapsed;}

inline DWORD CInternetConnect::GetTimerInterval()
   { return m_dwTimerInterval;}

inline DWORD CInternetConnect::GetTimeOut()
   { return m_dwTimeOut;}

inline LPCSTR CInternetConnect::GetURL()
   { return m_csURL;}

inline BOOL CInternetConnect::IsConnected()
{ return m_status.GetStatus() == CInternetConnectStatus::statusConnected; }

inline const CInternetConnectStatus::Status  CInternetConnect::GetStatus()
   { return m_status.GetStatus();}

inline void CInternetConnect::SetFirstCheckTime(DWORD dwFirstCheck)
   { m_dwFirstCheck = dwFirstCheck; }

inline void CInternetConnect::SetTimeOut(DWORD dwTimeOut)
   { m_dwTimeOut = dwTimeOut; }

inline void CInternetConnect::SetTimerInterval(DWORD dwTimerInterval)
   { m_dwTimerInterval = dwTimerInterval; }

inline void CInternetConnect::SetURL(LPCSTR szURL)
   {
      m_csURL = szURL;
   }

#endif 

