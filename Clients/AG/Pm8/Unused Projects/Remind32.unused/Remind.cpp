/*
// $Workfile: Remind.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:27p $
//
// Copyright © 1997 Mindscape, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/REMIND32/Remind.cpp $
// 
// 1     3/03/99 6:27p Gbeddow
// 
// 4     11/03/98 9:55a Mwilson
// added craft deluxe config
// 
// 3     9/02/98 11:06a Mwilson
// changed message box
// 
// 2     8/31/98 12:57p Mwilson
// added message box when trying to access Event Reminder from Print
// Standard
// 
// 1     6/22/98 10:04a Mwilson
// 
//    Rev 1.1   11 Jul 1997 11:03:30   Fred
// Uses new PMG 4.0 user directory structure
// 
//    Rev 1.0   25 Apr 1997 09:24:36   Fred
// Initial revision.
// 
//    Rev 1.2   25 Feb 1997 15:22:28   Fred
// Checks for existance of sender books
// 
//    Rev 1.1   25 Feb 1997 09:53:34   Fred
// Support for sender events
// 
//    Rev 1.0   20 Feb 1997 13:56:02   Fred
// Initial revision.
*/

#include "stdafx.h"
#include "palette.h"
#include "util.h"
#include "remind.h"
#include "eventwiz.h"
#include "upcoming.h"
#include "eventrem.h"
#include "pmwcfg.h"

#include <afxpriv.h>		// For AfxGetModuleShortFileName()
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Class name used for main window. Using another class name allows us to
// find a previous instance and register a dialog class with our icon.
// IMPORTANT: This name must match what is in the RC file for the dialog
// template.
#define	REMINDER_CLASS_NAME	"PMGEVENTREMINDER"

// The system heap.
HeapPtr system_heap;

/////////////////////////////////////////////////////////////////////////////
// CRemindApp

BEGIN_MESSAGE_MAP(CRemindApp, CWinApp)
	//{{AFX_MSG_MAP(CRemindApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRemindApp construction

CRemindApp::CRemindApp()
{
	system_heap = new StandardHeap;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRemindApp object

CRemindApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRemindApp initialization

BOOL CRemindApp::InitInstance()
{
	// See if a previous instance is running.
	// We use this generic method since hPrevInstance is unusable under 32 bits.
	// We need the window anyway, so we just do it.

	HWND hPrevWnd;
	if ((hPrevWnd = FindWindow(REMINDER_CLASS_NAME, NULL)) != NULL)
	{
		if (IsIconic(hPrevWnd))
		{
			ShowWindow(hPrevWnd, SW_SHOWNORMAL);
		}
		else
		{
			HWND hWndLastActive = GetLastActivePopup(hPrevWnd);
#ifdef WIN32
			SetForegroundWindow(hPrevWnd);
#endif
			BringWindowToTop(hPrevWnd);

			if (hWndLastActive != NULL)
			{
#ifdef WIN32
				SetForegroundWindow(hWndLastActive);
#endif
				BringWindowToTop(hWndLastActive);
			}
		}
		return 0;
	}
   m_ResourceString.SetInstanceHandle (m_hInstance);

	// Create a new window class with our icon. This class will be used for the
	// main dialog (Upcoming Events.) The class name is set in the template
	// in the RC file.
	BOOL fClassConstructed = FALSE;
	
	// Base the class on the normal dialog class (0x8002)
	WNDCLASS wc;
	if (::GetClassInfo(NULL, MAKEINTRESOURCE(0x8002), &wc))
	{
		wc.hInstance = m_hInstance;
		wc.hIcon = ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));
		wc.lpszClassName = REMINDER_CLASS_NAME;
		if (::RegisterClass(&wc) != NULL)
		{
			fClassConstructed = TRUE;
		}
	}

	// Only continue if we properly constructed the class.	
	if (fClassConstructed)
	{
		TRY
		{
			// Standard initialization
			// If you are not using these features and wish to reduce the size
			// of your final executable, you should remove from the following
			// the specific initialization routines you do not need.
		
			::SetHandleCount(100);
			
			if (system_heap == NULL)
			{
				// We really need a heap...
				return FALSE;
			}
		
			Init3DControls();
			
			// Check for start up flags.
			//
			//		/Q		Quiet mode. Only shows dialog if there
			//				are upcoming events. Otherwise, it goes
			//				away silently.
			
			m_fQuiet = FindParameter("/Q");
			
			// Initialize the palette.
			initialize_palette();
			
			// Figure out where we are.
			if (InitDirectories())
			{
				// Figure out who we are.
				if (InitNetwork())
				{
				/*
				// Read the installation data.
				*/
				   if (CPrintMasterConfiguration::IsAddOn () == TRUE)
				   {
					  m_installation_info.nInstallConfiguration = CPrintMasterConfiguration::AddOn;
					  m_installation_info.nCurrentConfiguration = CPrintMasterConfiguration::AddOn;   
				   }
				   else
				   {
					  if (!ReadInstDat())
					  {
						 return FALSE;
					  }
				   }
				/*
				// Validate that we have some kind of configuration.
				*/

				   if (m_installation_info.nInstallConfiguration == 0
						 || m_installation_info.nCurrentConfiguration == 0)
				   {
					  return ConfigurationError();
				   }
				   m_signature_file = CPrintMasterConfiguration::GetSignatureFile(m_installation_info.nInstallConfiguration);

				   if (m_signature_file == NULL)
				   {
					  return ConfigurationError();
				   }
				/*
				// Create our product configuration.
				*/

				   if (!CPrintMasterConfiguration::Create(m_installation_info.nCurrentConfiguration))
				   {
					  return ConfigurationError();
				   }

					if(GetConfiguration()->ProductLevel() == CPrintMasterConfiguration::PrintStandard)
					{
						CString csMessage;
						CString csTitle("@@R");
						GetConfiguration()->ReplaceText(csTitle);
						LoadConfigurationString(IDS_NOT_SUPPORTED, csMessage);
						::MessageBox(NULL, csMessage, csTitle, MB_OK | MB_ICONEXCLAMATION);
						return FALSE;
					}

					// Build the address book directory. This has to be done after the network ID
					// has been deteremined since it can be overridden in the nidded PMW INI file.
					m_csAddressBookDirectory = GetPmwIniString(SECTION_PMW_PATHS, KEY_PMW_ADDRESSBOOKPATH);
					if (m_csAddressBookDirectory.IsEmpty())
					{
						m_csAddressBookDirectory = m_csPmwDirectory;
						m_csAddressBookDirectory += "BOOKS";
					}
					Util::AppendBackslashToPath(m_csAddressBookDirectory);
					TRACE("Address Book Directory |%s|\n", (LPCSTR)m_csAddressBookDirectory);
					
					// Build the sender book directory.
					m_csSenderBookDirectory = m_csAddressBookDirectory+"SENDER";
					Util::AppendBackslashToPath(m_csSenderBookDirectory);
					TRACE("Sender Book Directory |%s|\n", (LPCSTR)m_csSenderBookDirectory);
					
					// Construct the INI file object for the event reminder information.
					char szFileName[_MAX_PATH];
					sprintf(szFileName, "%sUSER%03d\\REMIND.INI", (LPCSTR)(GetPmwDirectory()), GetNetworkUserId());
					TRACE("Event INI file is %s\n", szFileName);
					GetEventIniFile().Name(szFileName);
					
					// Figure out the preferred date format. This is specified by
					// the EventReminderDateFormat string in the [Configuration]
					// section of the PMW.INI file. The default value is from the RC
					// so it can be localized.
					m_csDateFormat = GetPmwIniString(SECTION_PMW_CONFIGURATION, KEY_PMW_EVENT_REMINDER_DATE_FORMAT);
					if (m_csDateFormat.IsEmpty())
					{
						m_csDateFormat.LoadString(IDS_DEFAULT_DATE_FORMAT);
					}
					m_csDateFormat.MakeUpper();
					TRACE("Preferred event reminder date format = |%s|\n", (LPCSTR)(GetDateFormat()));
					
					// Check for there case where there are no address or sender books.
					if (!CAddressBook::AddressBooksExist() && !CAddressBook::SenderBooksExist())
					{
						// Report the error unless we're in "Quiet" mode.
						if (!m_fQuiet)
						{
							CNoAddressBooksDialog Dialog;
							m_pMainWnd = &Dialog;
							Dialog.DoModal();
							m_pMainWnd = NULL;
						}
					}
					else
					{
						// Read in the defined events.
						ReadEvents();
						TRACE("%d current events, %d disabled events\n", CurrentEvents.GetSize(), DisabledEvents.GetSize());
						
						// Figure out which upcoming events should be reported.
						RebuildUpcomingEvents();
						
						// If there are no enabled events and we're in "quiet" mode, then finish.
						if ((UpcomingEvents.GetEnabledCount() > 0) || !GetQuiet())
						{
							CUpcomingEventsDialog UpcomingEventsDialog;
							m_pMainWnd = &UpcomingEventsDialog;
							int nResponse = UpcomingEventsDialog.DoModal();
							m_pMainWnd = NULL;
							
							// Check if the user wants to save their events.
							if (UpcomingEventsDialog.GetSaveEvents())
							{
								// Write out the events.
								WriteEvents();
							}
						}
					}
				}
			}
		}
		END_TRY
	}
	
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CRemindApp::ExitInstance()
{
	// Free up the palette.
	free_palette();
	
	return CWinApp::ExitInstance();		// Call the default.
}

	// Standard initialization
// Initialize the 3D controls.

void CRemindApp::Init3DControls(void)
{
	SetDialogBkColor();        // set dialog background color to gray

	TRY
	{
		m_ctl3d.Startup(AfxGetInstanceHandle(), NULL);
	}
	END_TRY
}

// Parse a parameter in a command line. A Parameter always begins
// at the start of the command line or after a space. No spaces
// are allowed within parameters, but quoted strings are handled
// properly and may contain spaces. All comparisons are case-insensitive.

LPSTR CRemindApp::LocateParameter(LPSTR lpszParamName, LPSTR lpszValue)
{
	LPSTR lpszResult = NULL;
	
	LPSTR lpszCommandLine = m_lpCmdLine;
	int nParamNameLength = lstrlen(lpszParamName);
	int nCommandLineLength = lstrlen(lpszCommandLine);
	BOOL fInQuote = FALSE;
	BOOL fStartOfParameter = TRUE;
	BOOL fFoundParameter = FALSE;
	
	while ((!fFoundParameter) && (nCommandLineLength >= nParamNameLength))
	{
		if (fStartOfParameter)
		{
			// A parameter can start here, check for a match.
			
			int i = 0;
			
			for (;;)
			{
				char ch1 = lpszCommandLine[i];
				if ((ch1 > 'a') && (ch1 < 'z')) ch1 = (char)(ch1+'A'-'a');
				
				char ch2 = lpszParamName[i];
				if ((ch2 > 'a') && (ch2 < 'z')) ch2 = (char)(ch2+'A'-'a');
				
				if (ch2 == 0)
				{
					fFoundParameter = TRUE;
					break;
				}
				
				if (ch1 != ch2)
				{
					fStartOfParameter = FALSE;
					break;
				}
				
				i++;
			}
		}
		
		else
		{
			if (*lpszCommandLine == ' ')
			{
				fStartOfParameter = !fInQuote;
			}
			
			else if (*lpszCommandLine == '"')
			{
				fInQuote = !fInQuote;
			}
			
			lpszCommandLine++;
			nCommandLineLength--;
		}
	}
	
	if (fFoundParameter)
	{
		lpszResult = lpszCommandLine;
		
		if (lpszValue != NULL)
		{
			*lpszValue = 0;
			
			lpszCommandLine += nParamNameLength;

			if (*lpszCommandLine == '=')
			{
				lpszCommandLine++;
			}
			
			fInQuote = FALSE;
			
			for (;;)
			{
				if (*lpszCommandLine == 0)
				{
					break;
				}
				
				if ((!fInQuote) && (*lpszCommandLine == ' '))
				{
					break;
				}
				
				if (*lpszCommandLine == '"')
				{
					fInQuote = !fInQuote;
				}
				
				else
				{
					*(lpszValue++) = *lpszCommandLine;
				}
				
				lpszCommandLine++;
			}
			
			*lpszValue = 0;
		}
	}
	
	return lpszResult;
}

BOOL CRemindApp::FindParameter(LPSTR lpszParamName)
{
	return LocateParameter(lpszParamName, NULL) != NULL;
}

LPSTR CRemindApp::GetParameter(LPSTR lpszParamName, LPSTR lpszValue)
{
	if (LocateParameter(lpszParamName, lpszValue) == NULL)
	{
		return NULL;
	}
	
	else
	{
		return lpszValue;
	}
}

BOOL CRemindApp::InitDirectories(void)
{
	BOOL fResult = FALSE;
	
	CString csPath;
	CString csDrive;
	CString csDirectory;
	
	TRY
	{
		// Get path name of ourselves. This assumes we're running from the PMW directory.
#ifdef WIN32
		AfxGetModuleShortFileName(AfxGetInstanceHandle(), csPath);
#else
		LPSTR pPath = csPath.GetBuffer(_MAX_PATH);
		::GetModuleFileName(AfxGetInstanceHandle(), pPath, _MAX_PATH);
		csPath.ReleaseBuffer();
#endif
      
      // We have the module name, now get the drive and directory.
		Util::SplitPath(csPath, &csDrive, &csDirectory, NULL, NULL);
		   
		// Save PMW directory.
		m_csPmwDirectory = csDrive+csDirectory;		// should have backslash on end
		
		fResult = TRUE;
		
		TRACE("PMW Directory |%s|\n", (LPCSTR)m_csPmwDirectory, (LPCSTR)m_csAddressBookDirectory);
	}
	CATCH_ALL(e)
	{
		fResult = FALSE;
	}
	END_CATCH_ALL
	
	ASSERT(fResult);
	
	return fResult;
}

// Read in the current and disabled events from the event INI file.
BOOL CRemindApp::ReadEvents(void)
{
	BOOL fResult = FALSE;
	LPSTR pEvents = NULL;
	
	TRY
	{
		// Remove any existing events.
		CurrentEvents.DeleteAll();
		DisabledEvents.DeleteAll();
		
		// Read in the current events.
		pEvents = EventIniFile.GetSectionKeys(CURRENT_EVENTS_SECTION);
		if (pEvents != NULL)
		{
			LPCSTR pszEvent = pEvents;
			
			// We have a list of events, add them to the array.
			while (*pszEvent != '\0')
			{
				// Create current event and add it to the array.
				CEvent* pEvent = new CEvent(pszEvent);
				if (pEvent != NULL)
				{
					// Add the event, Add() is responsible for deleting the event
					// if it is not inserted.
					CurrentEvents.Add(pEvent);
					pEvent = NULL;
				}
				
				// Move to the next event.
				pszEvent += strlen(pszEvent)+1;
			}
			
			delete [] pEvents;
			pEvents = NULL;
		}
		
		// If there are no current events and we have not tried this before,
		// then use some default events.
		if (CurrentEvents.GetSize() == 0)
		{
			CString csUsedDefaultEvents = GetPmwIniString(SECTION_PMW_CONFIGURATION, KEY_PMW_EVENT_REMINDER_USED_DEFAULT);
			if (!csUsedDefaultEvents.IsEmpty() == 0)
			{
				CString csEvent;
				CEvent* pEvent;
				CMlsStringArray csaDefaultEvents;
				
				TRY
				{
					// Get default events.
					csaDefaultEvents.SetNames(IDS_DEFAULT_EVENTS, ':');
					for (int nEvent = 0; nEvent < csaDefaultEvents.GetSize(); nEvent++)
					{
						csEvent = csaDefaultEvents[nEvent];
						if (!csEvent.IsEmpty())
						{
							pEvent = NULL;
							
							TRY
							{
								// Add default event.
								pEvent = new CEvent(csEvent);
								CurrentEvents.Add(pEvent);
								pEvent = NULL;
							}
							END_TRY
							
							// If we didn't use the event, then delete it.
							delete pEvent;
							pEvent = NULL;
						}
					}
				}
				END_TRY
			}
		}
		
		// Mark the INI file to show we've used the default events. If the user wants to
		// get rid of them, we don't want to put them back in.
		WritePmwIniString(SECTION_PMW_CONFIGURATION, KEY_PMW_EVENT_REMINDER_USED_DEFAULT, "True");

		// Read in the disabled events.
		pEvents = EventIniFile.GetSectionKeys(DISABLED_EVENTS_SECTION);
		if (pEvents != NULL)
		{
			LPCSTR pszEvent = pEvents;
			
			// Get the current time. Valid disabled events must have a time before this.
			CDay CurrentTime;
			
			// We have a list of events, add them to the array.
			while (*pszEvent != '\0')
			{
				// Create current event and add it to the array.
				CEvent* pEvent = new CEvent(pszEvent);
				if ((pEvent != NULL) && (pEvent->IsValid()))
				{
					// Check if the disabled date has passed. If so, then we are going
					// to get rid of the disabled event.
					if (pEvent->GetTime() >= CurrentTime.GetTime())
					{
						DisabledEvents.Add(pEvent);
						pEvent = NULL;
					}
				}
					
				// If we did not use the event, release it.
				delete pEvent;
				pEvent = NULL;
				
				// Move to the next event.
				pszEvent += strlen(pszEvent)+1;
			}
			
			delete [] pEvents;
			pEvents = NULL;
		}
		
		// Success.
		fResult = TRUE;
	}
	CATCH_ALL(e)
	{
		delete [] pEvents;
		pEvents = NULL;
		
		fResult = FALSE;
	}
	END_CATCH_ALL
	
	return fResult;
}

// Write out the current and disabled events to the event INI file.
BOOL CRemindApp::WriteEvents(void)
{
	BOOL fResult = FALSE;
	
	CString csKey;
	
	TRY
	{
		// Remove the existing event sections.
		EventIniFile.RemoveSection(CURRENT_EVENTS_SECTION);
		EventIniFile.RemoveSection(DISABLED_EVENTS_SECTION);
		
		int nEvent;
		
		// Write out the current events.
		for (nEvent = 0; nEvent < CurrentEvents.GetSize(); nEvent++)
		{
			CEvent* pEvent = CurrentEvents[nEvent];
			if ((pEvent != NULL) && (pEvent->IsValid()))
			{
				if (pEvent->GetIniString(csKey))
				{
					EventIniFile.WriteString(CURRENT_EVENTS_SECTION, csKey, "");
				}
			}
		}
		
		// Write out the disabled events.
		for (nEvent = 0; nEvent < DisabledEvents.GetSize(); nEvent++)
		{
			CEvent* pEvent = DisabledEvents[nEvent];
			if ((pEvent != NULL) && (pEvent->IsValid()))
			{
				if (pEvent->GetIniString(csKey))
				{
					EventIniFile.WriteString(DISABLED_EVENTS_SECTION, csKey, "");
				}
			}
		}
		
		// Success.
		fResult = TRUE;
	}
	END_TRY
	
	return fResult;
}

// Rebuild the upcoming event list from scratch.
BOOL CRemindApp::RebuildUpcomingEvents(void)
{
	BOOL fResult = FALSE;
	
	// Get rid of any current upcoming events.
	UpcomingEvents.DeleteAll();
	
	// Invalidate any "nearest" upcoming event.
	NearestUpcomingEvent->Invalidate();
	
	// Keep going until all events are done.
	int nCurrentEvent = 0;
	while (nCurrentEvent < CurrentEvents.GetSize())
	{
		// Find the starting and ending events which all have the
		// same address book. In the process, also find out if any
		// event in the group is an "All Names" event.
		BOOL fSeenAllEvent = FALSE;
		int nStartEvent = nCurrentEvent;
		for (nCurrentEvent = nStartEvent; nCurrentEvent < CurrentEvents.GetSize(); nCurrentEvent++)
		{
			// Compare address book names to make sure we're still in the same group.
			if (strcmpi(CurrentEvents[nStartEvent]->GetAddressBookUserName(), CurrentEvents[nCurrentEvent]->GetAddressBookUserName()) != 0)
			{
				break;
			}
			
			// Check for an "All Names" event.
			if (CurrentEvents[nCurrentEvent]->GetNameId() == (DWORD)-1)
			{
				fSeenAllEvent = TRUE;
			}
		}
		
		TRY
		{
			// Open the address book and search for matching events.
			if (AddressBook->OpenAddressBook(CurrentEvents[nStartEvent]->GetAddressBookUserName(), CurrentEvents[nStartEvent]->GetIsSenderDatabase()) == ERRORCODE_None)
			{
				CFlatFileDatabaseCursor* pIdCursor = AddressBook->GetIdCursor();
				if (pIdCursor != NULL)
				{
					CFlatFileDatabaseField* pIdField = pIdCursor->GetField(AddressBook->GetIdField());
					if (pIdField != NULL)
					{
						// Check if we have an "All Names" event. If so, it's more efficient to
						// look at each record once and compare it against the specified event
						// reminders.
						if (fSeenAllEvent)
						{
							// Examine each record starting.
							pIdCursor->First();
							while (!pIdCursor->AtEOF())
							{
								// Check each event in the group against the current record.
								for (int nEvent = nStartEvent; nEvent < nCurrentEvent; nEvent++)
								{
									// Check the current event.
									CheckUpcomingEvent(pIdCursor, pIdField, CurrentEvents[nEvent]);
								}
								
								// Move to the next record.
								pIdCursor->Next();
							}
						}
						else
						{
							// There is no "All Names" event, so we can just search to each
							// of the event record and check them directly.
							
							// Check each event in the group against the current record.
							for (int nEvent = nStartEvent; nEvent < nCurrentEvent; nEvent++)
							{
								// Move to the record associated with the event.
								BOOL fExact;
								pIdField->SetAsUnsigned32(CurrentEvents[nEvent]->GetNameId());
								pIdCursor->Find(&fExact);
								ASSERT(fExact);
								
								if (fExact)
								{
									// Check the current event.
									CheckUpcomingEvent(pIdCursor, pIdField, CurrentEvents[nEvent]);
								}
							}
						}
					}
				}
			}
		}
		END_TRY
	}
	
	return fResult;
}

// Check if an event should be added to the upcoming event list.
void CRemindApp::CheckUpcomingEvent(CFlatFileDatabaseCursor* pIdCursor, CFlatFileDatabaseField* pIdField, CEvent* pEvent)
{
	// See if the name ID matches.
	if ((pEvent->GetNameId() == (DWORD)-1)
	 || (pEvent->GetNameId() == pIdField->GetAsUnsigned32()))
	{
		CFlatFileDatabaseField* pEventField = pIdCursor->GetField(pEvent->GetEventTypeId());
		if (pEventField != NULL)
		{
			// Convert the value of the event field into a date.
			CDay Day(pEventField->GetAsString());
			if (Day.IsValid())
			{
				// Get the current day.
				CDay Today;
				
				// Compute the number of days away.
				int nDaysAway = Day-Today;
				ASSERT(nDaysAway >= 0);
				
				// Create a new upcoming event!
				// We need to create it now in case we need to remember it
				// as the "nearest" upcoming event.
				CEvent* pUpcomingEvent = NULL;
				TRY
				{
					pUpcomingEvent = new CEvent(
													pEvent->GetAddressBookUserName(),
													pEvent->GetIsSenderDatabase(),
													pEvent->GetEventTypeId(),
													pIdField->GetAsUnsigned32(),
													nDaysAway,
													&Day);
				}
				END_TRY;
				
				// Update nearest upcoming event.
				BOOL fUpdateNearestUpcomingEvent = FALSE;
				
				if (!NearestUpcomingEvent->IsValid())
				{
					// First one, grab it.
					fUpdateNearestUpcomingEvent = TRUE;
				}
				else
				{
					// Have to compare dates.
					CDay NearestDay(NearestUpcomingEvent->GetTime());
					if (nDaysAway < (NearestDay-Today))
					{
						fUpdateNearestUpcomingEvent = TRUE;
					}
				}
				
				if (fUpdateNearestUpcomingEvent)
				{
					*NearestUpcomingEvent = *pUpcomingEvent;
				}
				
				// Ok, now we can finally check if this event is within time limit
				// for reporting to the user.
				
				// Check if it's time to remind the user of this event.
				if ((nDaysAway >= 0) && (nDaysAway <= pEvent->GetDays()))
				{
					// Create a new upcoming event!
					TRY
					{
						UpcomingEvents.Add(pUpcomingEvent);
						
						// It belongs to the event array now.
						pUpcomingEvent = NULL;
					}
					END_TRY
				}
				
				// If we didn't use the upcoming event, then get rid of it.
				delete pUpcomingEvent;
				pUpcomingEvent = NULL;
			}
		}
	}
}


// Check if an event is disabled.
BOOL CRemindApp::IsEventDisabled(CEvent* pEvent)
{
	BOOL fDisabled = FALSE;
	
	ASSERT(pEvent->IsValid());
	if (pEvent->IsValid())
	{
		// Check if the event is disabled.
		for (int nIndex = 0; nIndex < DisabledEvents.GetSize(); nIndex++)
		{
			CEvent* pDisabledEvent = DisabledEvents.ElementAt(nIndex);
			ASSERT(pDisabledEvent->IsValid());
			if (pDisabledEvent->IsValid())
			{
				if (pDisabledEvent->ComparePartial(*pEvent) == 0)
				{
					fDisabled = TRUE;
					break;
				}
			}
		}
	}
	
	return fDisabled;
}
		
// Add a current event reminder.
void CRemindApp::AddEvent(CEvent& Event)
{
	int nIndex = CurrentEvents.Find(&Event);
	if (nIndex != -1)
	{
		CurrentEvents.Delete(&Event);
	}
	CurrentEvents.Add(&Event);
}

// Delete a current event reminder.
void CRemindApp::DeleteEvent(CEvent& Event)
{
	CurrentEvents.Delete(&Event);
}

// Return a string value from a PrintMaster INI files (looks in nidded INI first.)
CString CRemindApp::GetPmwIniString(LPCSTR pszSection, LPCSTR pszEntry, LPCSTR pszDefault /*=NULL*/)
{
	CString csDefault;
	CIniFile IniFile;
	char szFileName[_MAX_PATH];
	
	// Check in the main PMW.INI to get default.
	sprintf(szFileName, "%sPMW.INI",  (LPCSTR)GetPmwDirectory());
	IniFile.Name(szFileName);
	csDefault = IniFile.GetString(pszSection, pszEntry, pszDefault);
		
	// Check in the nidded PMW.INI using value from main INI as default.
	sprintf(szFileName, "%sUSER%03d\\USER.INI",  (LPCSTR)(GetPmwDirectory()), GetNetworkUserId());
	IniFile.Name(szFileName);
	
	return IniFile.GetString(pszSection, pszEntry, csDefault);
}

// Write a string to the nidded PrintMaster INI file.
void CRemindApp::WritePmwIniString(LPCSTR pszSection, LPCSTR pszEntry, LPCSTR pszValue)
{
	CIniFile IniFile;
	char szFileName[_MAX_PATH];
	
	// Get the name of the nidded PMW.INI file.
	sprintf(szFileName, "%sUSER%03d\\USER.INI",  (LPCSTR)(GetPmwDirectory()), GetNetworkUserId());
	IniFile.Name(szFileName);
	
	// Write the value.
	IniFile.WriteString(pszSection, pszEntry, pszValue);
}

BOOL CRemindApp::ReadInstDat(void)
{
   CString csInstdatName;

   TRY
      LoadConfigurationString(IDS_INSTDAT_NAME, csInstdatName);
   END_TRY

	char path[MAX_PATH];
   GetModuleFileName(NULL, path, MAX_PATH);
   CString csPath(path);
   csPath = csPath.Left(csPath.ReverseFind('\\') + 1) + csInstdatName;

   if (!m_installation_info.Get(csPath, FALSE))
   {
      CString message;

      TRY
      {
         char buffer[_MAX_PATH];
         LoadConfigurationString(IDS_INSTDAT_MISSING, message);
         sprintf(buffer, message, csPath);
         ::MessageBox(0,
                     buffer,
                     AfxGetAppName(),
                     MB_OK | MB_ICONSTOP | MB_TASKMODAL);
      }
      CATCH_ALL(e)
      {
      }
      END_CATCH_ALL
   /* Regardless. */
      return FALSE;
   }

   return TRUE;
}

BOOL CRemindApp::ConfigurationError(void)
{
   AfxMessageBox(IDS_CONFIG_NO_SEE, MB_OK | MB_ICONEXCLAMATION);
   return FALSE;
}

BOOL CPrintStandardConfiguration::UnsupportedDialog(LPCSTR pMessage /*=NULL*/, LPCSTR pTitle /*=NULL*/)
{
/*
// Prevent re-entrancy.
*/
#if 0
   static BOOL fInHere = FALSE;
   CBoolSemaphore Sem(fInHere);
   if (Sem.Lock())
   {
      if (pMessage == NULL)
      {
         // JRO pMessage = szFeatureNotAvailable;
         pMessage = GET_PMWAPP()->GetResourceStringPointer (uFeatureNotAvailable);
      }

      if (CanInstantUpgrade())
      {
         CNeedToUpgradeDialog Dialog(pMessage, pTitle);
         
         if (Dialog.DoModal() == IDOK)
         {
            return ((CMainFrame*)AfxGetMainWnd())->DoUpgrade();
         }
      }
      else
      {
         CDialog Dialog(IDD_NON_INSTANT_UPGRADE);

         Dialog.DoModal();
      }
   }
#endif
   return FALSE;
}


