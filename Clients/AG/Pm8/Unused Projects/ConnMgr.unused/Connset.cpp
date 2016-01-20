/***************************************************************************
*  FILE:        CONNSET.CPP                                                *
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
// $Log: /PM8/ConnMgr/Connset.cpp $
// 
// 1     3/03/99 6:14p Gbeddow
// 
// 1     10/19/98 11:18a Dennis
// 
// 11    8/29/98 8:24a Dennis
// Changes for AOL 4.0 (hidden file detection and Autodial disable)
// 
// 10    7/25/98 1:46p Jayn
// Actually implemented my message change.
// 
// 9     7/21/98 8:35a Jayn
// Added "first time" message to Update()
// 
// 8     7/14/98 11:03a Jayn
// INI stuff
// 
// 7     5/26/98 1:58p Dennis
// Added call to GetConfiguration()->ReplaceText() so app. name can be
// substituted in string.
// 
// 6     3/02/98 5:04p Johno
// Compile update
// 
// 5     2/27/98 1:52p Johno
// Compile update
***************************************************************************/
// ConnCfg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ConnSet.h"
#include "inifile.h"
#include "PrefOnl.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Connection Section entries
#define SZCONNECTION_SECTION              "Connection"
#define SZCONNECTION_CUR_CONN_ENTRY       "CurEntry"
#define SZCONNECTION_TYPE_ENTRY           "Type"
// CurEntry values
#define SZCONNECTION_DIRECT_SECTION       "DirectConnection"
#define SZCONNECTION_DIALUP_SECTION       "DialupConnection"
#define SZCONNECTION_AOL_SECTION          "AOLConnection"
#define SZCONNECTION_COMPUSERVE_SECTION   "CompuServeConnection"
#define SZCONNECTION_CUSTOM_SECTION       "CustomConnection"

// Entries common to connections
#define SZCONNECTION_NAME_ENTRY           "Name"
#define SZCONNECTION_APPFILENAME_ENTRY    "AppFileName"
#define SZCONNECTION_TIMEOUT_ENTRY        "TimeOut"
#define SZCONNECTION_FIRSTCHECK_ENTRY     "FirstCheck"
#define SZCONNECTION_TIMERINTERVAL_ENTRY  "TimerInterval"
#define SZCONNECTION_WATCHFORDIALUP_ENTRY "WatchDialup"
#define SZCONNECTION_RESTOREAUTODIAL_ENTRY "RestoreAutodial"

// Entries common to application connections
#define SZCONNECTION_INSTALLED_ENTRY  "Installed"

// Dialup Specific
#define SZCONNECTION_DIALUPCOUNT_ENTRY    "DialupCount"

// AOL Specific
#define SZCONNECTION_AOL_OSVERSION_ENTRY  "AOLOsVersion"

#define DWTIMEOUT_DEFAULT                 INETCONN_MINUTE * 2 / INETCONN_SECOND
#define DWTIMER_INTERVAL_DEFAULT          1
#define DWTIMER_FIRSTCHECK_DEFAULT        10

CConnectionSettings::CConnectionSettings()
   {
      InitVars();
   }

CConnectionSettings::~CConnectionSettings()
   {
   }

void CConnectionSettings::InitVars()
   {
      m_bUpdated = FALSE;
      m_csName.Empty();
      m_csAppFileName.Empty();
      m_enType = typeNone;
      m_dwTimeOut = DWTIMEOUT_DEFAULT;
      m_dwFirstCheck = DWTIMER_FIRSTCHECK_DEFAULT;
      m_dwTimerInterval = DWTIMER_INTERVAL_DEFAULT;
      m_dwDialupCount = 0;
      m_bAOLInstalled = FALSE;
      m_bCompuServeInstalled = FALSE;
      m_enAOLOsVersion = CAOLInfo::osUnknown;
      m_bWatchForDialup = FALSE;
      m_bRestoreAutodial = FALSE;
   }

void CConnectionSettings::GetSettings(Type enType/* = typeNone*/)
   {
      CIniFile       iniFile;
      CString        csValue, csConnectionSection, csDefaultAppFileName;
      CString        csDefaultAppTitle;
      BOOL           bGetAOLSettings = FALSE, bGetDialupSettings = FALSE;
      BOOL           bGetAppFileName = FALSE, bGetCompuServeSettings = FALSE;
      BOOL           bDefaultWatchForDialup = FALSE;
      CString        csFileName;

      Util::GetWindowsDirectory(csFileName);
      csFileName += "\\ConnMgr.ini";
      iniFile.Name(csFileName);

      // If no type is specified, load settings for current type (if any)
      // If there is no current type and no type is specified, 
      // all settings are retrieved from INI file

      // Note that if Update() has been called, followed by a call to
      // GetSettings() with typeNone, updated system settings will 
      // be lost.  A call to SaveSettings() in-between calls would need to be
      // made.
      if(enType == typeNone)
         {
            // If no current type, get from INI
            // Otherwise get settings for current type
            if(m_enType == typeNone)
               {
                  // If INI file has no connection type, then no connection
                  // specific settings are retrieved
                  InitVars();
                  bGetAOLSettings = bGetDialupSettings = bGetCompuServeSettings = TRUE;
                  m_enType = (CConnectionSettings::Type) iniFile.GetInteger(
                     SZCONNECTION_SECTION,
                     SZCONNECTION_TYPE_ENTRY,
                     typeNone);
               }
         }
      else
         m_enType = enType;

      switch(m_enType)
         {
            case typeModemDialup:
               csConnectionSection = SZCONNECTION_DIALUP_SECTION;
               bGetDialupSettings = TRUE;
               bDefaultWatchForDialup = TRUE;
               break;
            case typeModemAOL:
               csConnectionSection = SZCONNECTION_AOL_SECTION;
               bGetAppFileName = TRUE;
               csDefaultAppFileName = m_aolInfo.GetAppFileName();
               csDefaultAppTitle.LoadString(IDS_AOL);
               bGetAOLSettings = TRUE;
               break;
            case typeModemCompuServe:
               csConnectionSection = SZCONNECTION_COMPUSERVE_SECTION;
               bGetAppFileName = TRUE;
               csDefaultAppFileName = m_CompuServeInfo.GetAppFileName();
               bGetCompuServeSettings = TRUE;
               bDefaultWatchForDialup = TRUE;
               csDefaultAppTitle.LoadString(IDS_COMPUSERVE);
               break;
            case typeModemCustom:
               csConnectionSection = SZCONNECTION_CUSTOM_SECTION;
               bGetAppFileName = TRUE;
               break;
            case typeDirect:
               csConnectionSection = SZCONNECTION_DIRECT_SECTION;
               break;
            case typeNone:
               bGetAOLSettings = bGetDialupSettings = bGetCompuServeSettings = TRUE;
               break;
            default:
               ASSERT(0);
               break;
         }

      if(bGetAppFileName)
         {
            m_csAppFileName = iniFile.GetString(
               csConnectionSection,
               SZCONNECTION_APPFILENAME_ENTRY,
               csDefaultAppFileName);
            if(m_csAppFileName.IsEmpty())
               m_csAppFileName = csDefaultAppFileName;
         }

      // Only get AOL settings when request to load current settings or
      // specific request for AOL
      // If settings were updated, don't reload from INI file
      if(bGetAOLSettings && !m_bUpdated)
         {
            m_bAOLInstalled = iniFile.GetInteger(
               SZCONNECTION_AOL_SECTION,
               SZCONNECTION_INSTALLED_ENTRY,
               FALSE);
            m_enAOLOsVersion = (CAOLInfo::OSVersion)iniFile.GetInteger(
               SZCONNECTION_AOL_SECTION,
               SZCONNECTION_AOL_OSVERSION_ENTRY,
               CAOLInfo::osUnknown);
         }
      // Only get CompuServe settings when request to load current settings or
      // specific request for CompuServe
      // If settings were updated, don't reload from INI file
      if(bGetCompuServeSettings && !m_bUpdated)
         {
            m_bCompuServeInstalled = iniFile.GetInteger(
               SZCONNECTION_COMPUSERVE_SECTION,
               SZCONNECTION_INSTALLED_ENTRY,
               FALSE);
         }
      // Only get Dialup settings when request to load current settings or
      // specific request for Dialup
      // If settings were updated, don't reload from INI file
      if(bGetDialupSettings && !m_bUpdated)
         {
            m_dwDialupCount = iniFile.GetDword(
               SZCONNECTION_DIALUP_SECTION,
               SZCONNECTION_DIALUPCOUNT_ENTRY,
               0);
         }
      if(m_enType != typeNone)
         {
            m_csName = iniFile.GetString(
               csConnectionSection,
               SZCONNECTION_NAME_ENTRY,
               csDefaultAppTitle);
            m_dwTimeOut = iniFile.GetDword(
               csConnectionSection,
               SZCONNECTION_TIMEOUT_ENTRY,
               DWTIMEOUT_DEFAULT);
            m_dwTimerInterval = iniFile.GetDword(
               csConnectionSection,
               SZCONNECTION_TIMERINTERVAL_ENTRY,
               DWTIMER_INTERVAL_DEFAULT);
            m_dwFirstCheck = iniFile.GetDword(
               csConnectionSection,
               SZCONNECTION_FIRSTCHECK_ENTRY,
               DWTIMER_FIRSTCHECK_DEFAULT);
            m_bWatchForDialup = iniFile.GetInteger(
               csConnectionSection,
               SZCONNECTION_WATCHFORDIALUP_ENTRY,
               bDefaultWatchForDialup);
            m_bRestoreAutodial = iniFile.GetInteger(
               csConnectionSection,
               SZCONNECTION_RESTOREAUTODIAL_ENTRY,
               FALSE);
         }
   }

void CConnectionSettings::SaveSettings()
   {
      CIniFile       iniFile;
      CString        csConnectionSection;
      BOOL           bSaveAdvanced = FALSE;
      CString        csFileName;

      Util::GetWindowsDirectory(csFileName);
      csFileName += "\\ConnMgr.ini";
      iniFile.Name(csFileName);

      iniFile.WriteInteger(
         SZCONNECTION_SECTION,
         SZCONNECTION_TYPE_ENTRY,
         (CConnectionSettings::Type) m_enType);

      switch(m_enType)
         {
            case typeModemDialup:
               csConnectionSection = SZCONNECTION_DIALUP_SECTION;
               break;
            case typeModemAOL:
               csConnectionSection = SZCONNECTION_AOL_SECTION;
               bSaveAdvanced = TRUE;
               break;
            case typeModemCompuServe:
               csConnectionSection = SZCONNECTION_COMPUSERVE_SECTION;
               bSaveAdvanced = TRUE;
               break;
            case typeModemCustom:
               csConnectionSection = SZCONNECTION_CUSTOM_SECTION;
               bSaveAdvanced = TRUE;
               break;
            case typeDirect:
               csConnectionSection = SZCONNECTION_DIRECT_SECTION;
               break;
         }
      iniFile.WriteString(
         csConnectionSection,
         SZCONNECTION_NAME_ENTRY,
         m_csName);
      if(bSaveAdvanced)
         {
            iniFile.WriteString(
               csConnectionSection,
               SZCONNECTION_APPFILENAME_ENTRY,
               m_csAppFileName);
            iniFile.WriteDword(
               csConnectionSection,
               SZCONNECTION_TIMEOUT_ENTRY,
               m_dwTimeOut);
            iniFile.WriteDword(
               csConnectionSection,
               SZCONNECTION_TIMERINTERVAL_ENTRY,
               m_dwTimerInterval);
            iniFile.WriteDword(
               csConnectionSection,
               SZCONNECTION_FIRSTCHECK_ENTRY,
               m_dwFirstCheck);
            iniFile.WriteInteger(
               csConnectionSection,
               SZCONNECTION_WATCHFORDIALUP_ENTRY,
               m_bWatchForDialup);
            iniFile.WriteInteger(
               csConnectionSection,
               SZCONNECTION_RESTOREAUTODIAL_ENTRY,
               m_bRestoreAutodial);
         }
      iniFile.WriteDword(
         SZCONNECTION_DIALUP_SECTION,
         SZCONNECTION_DIALUPCOUNT_ENTRY,
         m_dwDialupCount);
      iniFile.WriteInteger(
         SZCONNECTION_AOL_SECTION,
         SZCONNECTION_INSTALLED_ENTRY,
         m_bAOLInstalled);
      iniFile.WriteInteger(
         SZCONNECTION_COMPUSERVE_SECTION,
         SZCONNECTION_INSTALLED_ENTRY,
         m_bCompuServeInstalled);
      iniFile.WriteInteger(
         SZCONNECTION_AOL_SECTION,
         SZCONNECTION_AOL_OSVERSION_ENTRY,
         (CAOLInfo::OSVersion) m_enAOLOsVersion);
   }

void CConnectionSettings::SetType(Type enNewType)
   {
      m_enType = enNewType;
      if(m_enType == typeModemDialup)
         SetWatchForDialup(TRUE);
   }

BOOL CConnectionSettings::Update(BOOL bRunDialogIfNeeded /*=TRUE*/, BOOL bWarn /*=TRUE*/)
{
   DWORD    dwDialupCount = 0;
   BOOL     bAOLInstalled, bCompuServeInstalled;
   BOOL     bDetectedNewAOL = FALSE, bDetectedNewDialups = FALSE;
   BOOL     bDetectedNewCompuServe = FALSE, bConnectionChanged = FALSE;
   int      nRetVal;
   CString  csResource;

   InitVars();

   // Get current state of AOL install
   bAOLInstalled = m_aolInfo.IsInstalled();

   // Get Current state of CompuServe install
   bCompuServeInstalled = m_CompuServeInfo.IsInstalled();

   GetSettings(); // Get current INI File Settings

   // Check if we need to warn user about 16-bit AOL
   // Warn only first time a new 16-bit AOL install is detected
   if(m_enAOLOsVersion != CAOLInfo::osWin16 &&
      m_aolInfo.GetOSVersion() == CAOLInfo::osWin16 && bWarn)
   {
      csResource.LoadString(IDS_AOL16WARNING);
      ASSERT(!csResource.IsEmpty());
//GetConfiguration()->ReplaceText(csResource);
      AfxMessageBox(csResource);
      bDetectedNewAOL = TRUE;
   }

   // Check if AOL was installed/removed or OS version of AOL changed
   if(bAOLInstalled == m_bAOLInstalled)
   {
      if(bAOLInstalled && m_enAOLOsVersion != m_aolInfo.GetOSVersion())
         bDetectedNewAOL = TRUE;
   }
   else
      bDetectedNewAOL = TRUE;

   if(bCompuServeInstalled != m_bCompuServeInstalled)
      bDetectedNewCompuServe = TRUE;

   // Check if dialups changed
   if(m_dialups.EnumerateEntries() == CDialupConnections::errorNone)
      dwDialupCount  = m_dialups.GetEntryCount();

   if(dwDialupCount != m_dwDialupCount)
      bDetectedNewDialups = TRUE;

   // Update CConnectionSettings members from current system state
   m_bAOLInstalled = bAOLInstalled;
   m_enAOLOsVersion = m_aolInfo.GetOSVersion();
   m_bCompuServeInstalled = bCompuServeInstalled;
   m_dwDialupCount = dwDialupCount;
   m_bUpdated = TRUE;

   // Check if connection environment has changed
   if(bDetectedNewDialups || bDetectedNewAOL || bDetectedNewCompuServe)
      bConnectionChanged = TRUE;

   // If connection changed, tell user
   if((bConnectionChanged) || (GetType() == typeNone))
   {
		csResource.Empty();		// Empty unless we put something in it.
		if (GetType() == typeNone)
		{
			// The user has never chosen an Internet connection, and we're
			// about to show the Preferences dialog.
			// Put up a little message explaining what's about to happen.
         csResource.LoadString(IDS_FIRST_INTERNET);
         ASSERT(!csResource.IsEmpty());
		}
      else if(bConnectionChanged)
      {
			// The connection has changed. Inform the user.
         csResource.LoadString(IDS_CONNECTION_CHANGED);
         ASSERT(!csResource.IsEmpty());
      }

		// If we have a message for the user, show it now.
		if (!csResource.IsEmpty() && bWarn)
		{
			// We have a message to put up.
         AfxMessageBox(csResource);
		}

      if(bRunDialogIfNeeded)
      {
         CPreferencesOnlinePage  prefsOnlineDlg;

         nRetVal = prefsOnlineDlg.DoModal(this);
         if(nRetVal != IDOK)
            m_bUpdated = FALSE;
      }
   }
   
   return m_bUpdated;
}

BOOL CConnectionSettings::Change()
   {
      int      nRetVal = IDOK;
      CPreferencesOnlinePage  prefsOnlineDlg;

      Update(FALSE,FALSE);

      nRetVal = prefsOnlineDlg.DoModal(this);
      if(nRetVal != IDOK)
         m_bUpdated = FALSE;
      return (nRetVal == IDOK) && m_bUpdated;
   }


