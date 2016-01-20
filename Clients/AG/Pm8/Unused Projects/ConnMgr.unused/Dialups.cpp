#include "stdafx.h"
#include <afxinet.h>    // For WININet calls
#include <raserror.h>   // for RAS API Errors
#include "util.h"       // For FileExists check
#include "dialups.h"
#include <regstr.h>


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

// Check if connected for a specific connection
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

// Check if connected for any Dialup entry
BOOL  CDialupConnections::IsConnected(void)
{
   CDialupConnections::ErrorCode dialupError;
   BOOL        bConnected = FALSE;

   dialupError = EnumerateConnections();
   if(dialupError == CDialupConnections::errorNone)
   {
      DWORD dwNumConnections = GetConnectionCount();
      const RASCONN *pConnections = GetConnections();
      // Loop thru connections 
      for(DWORD dwCurConnection=0;dwCurConnection < dwNumConnections && !bConnected;dwCurConnection++)
      {
         if(pConnections && IsConnected(pConnections[dwCurConnection].hrasconn))
            bConnected = TRUE;
      }
   }
   return bConnected;
}

BOOL CDialupConnections::HangUp(void)
{
   ErrorCode      errorCode;

   errorCode = Init();

   if(errorCode != errorNone)
      return errorCode;


   errorCode = EnumerateConnections();
   if(errorCode == CDialupConnections::errorNone)
   {
      errorCode = errorFail;

      DWORD dwNumConnections = GetConnectionCount();
      const RASCONN *pConnections = GetConnections();

      // Loop thru connections 
      BOOL bSuccess=FALSE;
      for(DWORD dwCurConnection=0;dwCurConnection < dwNumConnections && !bSuccess;dwCurConnection++)
      {
         if(pConnections && IsConnected(pConnections[dwCurConnection].hrasconn))
         {
            errorCode = HangUp(pConnections[dwCurConnection].hrasconn);
            bSuccess=TRUE;
         }
      }
   }

   return (errorCode == errorNone);
}

CDialupConnections::ErrorCode CDialupConnections::HangUp(HRASCONN hConnection)
{
   ErrorCode      errorCode;

   errorCode = Init();

   ASSERT(hConnection);
   if(errorCode != errorNone)
      return errorCode;

   if(hConnection == NULL)
      return errorFail;

   m_dwRasError = m_pProcRasHangUp( hConnection );

   if(m_dwRasError == 0)
      errorCode = errorNone;
   else
      errorCode = errorRAS;
   return errorCode;
}

