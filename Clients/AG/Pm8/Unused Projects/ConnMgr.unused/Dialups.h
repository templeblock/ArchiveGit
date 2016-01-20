#ifndef _DIALUPS_INC_
#define _DIALUPS_INC_

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
      BOOL                          IsConnected(void);
      const DWORD                   GetConnectionCount() const;
      const RASCONN *               GetConnections() const;

      BOOL                          HangUp(void);
      ErrorCode                     HangUp(HRASCONN hConnection);

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

#endif
