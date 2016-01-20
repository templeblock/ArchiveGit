/*
// $Workfile: Remind.h $
// $Revision: 1 $
// $Date: 3/03/99 6:27p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/Remind.h $
// 
// 1     3/03/99 6:27p Gbeddow
// 
// 1     6/22/98 10:04a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:58   Fred
// Initial revision.
// 
//    Rev 1.1   25 Feb 1997 09:53:36   Fred
// Support for sender events
// 
//    Rev 1.0   20 Feb 1997 13:56:28   Fred
// Initial revision.
*/

class CRemindApp;

#ifndef __REMIND_H__
#define __REMIND_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "addrbook.h"
#include "event.h"
#include "cctl3d.h"
#include "instdat.h"
#include "strclass.h"

// Standard section and key names.
#define SECTION_PMW_CONFIGURATION					"Configuration"
#define KEY_PMW_EVENT_REMINDER_DATE_FORMAT		"EventReminderDateFormat"
#define KEY_PMW_EVENT_REMINDER_USED_DEFAULT		"EventReminderUsedDefault"

// The following must match what's in PMW.CPP.
#define SECTION_PMW_PATHS								"Paths"
#define KEY_PMW_ADDRESSBOOKPATH						"AddressBookPathV3"

// Get CRemindApp pointer.
#define GETAPP() ((CRemindApp*)AfxGetApp())

// Macros for retrieving information from application.
#define PmwDirectory (GETAPP()->GetPmwDirectory())
#define AddressBookDirectory (GETAPP()->GetAddressBookDirectory())
#define SenderBookDirectory (GETAPP()->GetSenderBookDirectory())
#define NetworkUserName (GETAPP()->GetNetworkUserName())
#define NetworkUserId (GETAPP()->GetNetworkUserId())
#define EventIniFile (GETAPP()->GetEventIniFile())
#define DateFormat (GETAPP()->GetDateFormat())
#define AddressBook (GETAPP()->GetAddressBook())
#define CurrentEvents (GETAPP()->GetCurrentEvents())
#define DisabledEvents (GETAPP()->GetDisabledEvents())
#define UpcomingEvents (GETAPP()->GetUpcomingEvents())
#define NearestUpcomingEvent (GETAPP()->GetNearestUpcomingEvent()) 

/////////////////////////////////////////////////////////////////////////////
// CRemindApp:
// See remind.cpp for the implementation of this class
//

class CRemindApp : public CWinApp
{
public:
	CRemindApp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemindApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRemindApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
public:
	InstallationInfo m_installation_info;

	// Add an event to the event reminder list.
	void AddEvent(CEvent& Event);

	// Remove an event from the event reminder list.
	void DeleteEvent(CEvent& Event);

	// Return point to address book object.
	CAddressBook* GetAddressBook(void)
		{ return &m_AddressBook; }
		
	// Return event arrays.
	CEventArray& GetCurrentEvents(void)
		{ return m_CurrentEvents; }
	CEventArray& GetDisabledEvents(void)
		{ return m_DisabledEvents; }
	CEventArray& GetUpcomingEvents(void)
		{ return m_UpcomingEvents; }
		
	// Return a pointer to the nearest upcoming event.
	CEvent* GetNearestUpcomingEvent(void)
		{ return &m_NearestUpcomingEvent; }
		
	// Read and write event information.
	BOOL ReadEvents(void);
	BOOL WriteEvents(void);
	
	// Rebuild the upcoming event list from scratch.
	BOOL RebuildUpcomingEvents(void);
	void CheckUpcomingEvent(CFlatFileDatabaseCursor* pIdCursor, CFlatFileDatabaseField* pIdField, CEvent* pEvent);
		
	// Check if an event is disabled.
	BOOL IsEventDisabled(CEvent* pEvent);
		
	// Return directory information.
	const CString& GetPmwDirectory(void) const
		{ return m_csPmwDirectory; }
	const CString& GetAddressBookDirectory(void) const
		{ return m_csAddressBookDirectory; }
	const CString& GetSenderBookDirectory(void) const
		{ return m_csSenderBookDirectory; }
	
	// Return network information.
	const CString& GetNetworkUserName(void) const
		{ return m_csNetworkUserName; }
	int GetNetworkUserId(void) const
		{ return m_nNetworkUserId; }
		
	// Return event INI file.
	CEventIniFile& GetEventIniFile(void)
		{ return m_EventIniFile; }
		
	// Return preferred date format.	
	const CString& GetDateFormat(void) const
		{ return m_csDateFormat; }
		
	// Return start up flags.
	BOOL GetQuiet(void) const
		{ return m_fQuiet; }
		
	// Return a string value from a PrintMaster INI files (looks in nidded INI first.)
	CString GetPmwIniString(LPCSTR pszSection, LPCSTR pszEntry, LPCSTR pszDefault = NULL);
	
	// Write a string to the nidded PrintMaster INI file.
	void WritePmwIniString(LPCSTR pszSection, LPCSTR pszEntry, LPCSTR pszValue);

   LPCSTR
   GetResourceStringPointer (UINT id)
   {
      return m_ResourceString.GetPointer (id);
   }
		
protected:

	// 3D control support.
	
	void Init3DControls(void);
	CControl3D m_ctl3d;
	
protected:
	BOOL ReadInstDat(void);
	BOOL ConfigurationError(void);

	LPCSTR m_signature_file;
	CResourceString   m_ResourceString;

	// Address book object.
	
	CAddressBook m_AddressBook;
	
	// Event arrays.
	
	CEventArray m_CurrentEvents;
	CEventArray m_DisabledEvents;
	CEventArray m_UpcomingEvents;
	
	// Nearest upcoming event.
	
	CEvent m_NearestUpcomingEvent;

	// Command line parameter support.
	
	LPSTR LocateParameter(LPSTR lpszParamName, LPSTR lpszValue);
	BOOL FindParameter(LPSTR lpszParamName);
	LPSTR GetParameter(LPSTR lpszParamName, LPSTR lpszValue);
	
	// Directories and file names.
	
	BOOL InitDirectories(void);
	CString m_csPmwDirectory;
	CString m_csAddressBookDirectory;
	CString m_csSenderBookDirectory;
	
	// Network information.
	
	BOOL InitNetwork(void);
	BOOL GetNetworkUserName(LPSTR lpszNetworkUserName);
	BOOL GetNetworkUserId(LPSTR lplzNetworkUserName, LPSTR pszMessage);
	CString m_csNetworkUserName;
	int m_nNetworkUserId;
	int m_nNetworkUserCount;

	// Event INI file.
	
	CEventIniFile m_EventIniFile;

	// Date format String
	
	CString m_csDateFormat;
	
	// Start up flags.
	
	BOOL m_fQuiet;
};

#endif
