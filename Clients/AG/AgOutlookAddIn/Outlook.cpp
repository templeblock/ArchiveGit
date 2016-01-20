#include "stdafx.h"
#include "resource.h"
#include "Outlook.h"
#include "MapiAdmin.h"

//	Note: Namespace class member has changed to _Namespace in Outlook 2000 and 2002
//	#include "msoutl8.h"	// Outlook 97
//	#include "msoutl85.h"	// Outlook 98
//	#include "msoutl9.h"	// Outlook 2000
//	#include "msoutl.h"		// Outlook 2002
//	#include "msoe.h"		// Outlook Express

/////////////////////////////////////////////////////////////////////////////
COutlook::COutlook(IDispatch* pApplication)
{
	m_pApplication = pApplication;
}

/////////////////////////////////////////////////////////////////////////////
void COutlook::MapiSendmail() 
{
	CMapiAdmin mapi;
	// MAPI_NEW_SESSION|MAPI_TIMEOUT_SHORT
//j	HWND hWndParent = AfxGetMainWnd()->GetSafeHwnd();
	if (!mapi.Logon(NULL/*hWndParent*/, NULL/*lpszProfileName*/, NULL/*lpszPassword*/, 0/*flFlags*/))
		return;

	LPCTSTR ppRecipients[1] = {"jmccurdy@ag.com"};
	mapi.CreateMessage(1, ppRecipients, "About our meeting", "Here is a test message", true);
	if (mapi.SendMessage())
		MessageBox(NULL, "Message sent successfully", "Sent", MB_OK);
}

/////////////////////////////////////////////////////////////////////////////
void COutlook::AddContact() 
{
	Outlook::_ApplicationPtr pOutlook(m_pApplication); 

	// Logon. Doesn't hurt if you are already running and logged on
	_NameSpacePtr pOutlookNamespace;
	pOutlook->GetNamespace(CComBSTR("MAPI"), &pOutlookNamespace);
	CComVariant varOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	pOutlookNamespace->Logon(varOptional, varOptional, varOptional, varOptional);

	// Create a new contact
	IDispatchPtr pCreateDispatch;
	pOutlook->CreateItem(Outlook::olContactItem, &pCreateDispatch);

	Outlook::_ContactItemPtr pContact(pCreateDispatch); 

	// Setup Contact information
	pContact->put_FullName(CComBSTR("James Smith"));
//j	pContact->put_Birthday(CComDateTime(1975,9,15/*nYear,nMonth,nDay*/, 0,0,0/*nHour,nMin,nSec*/));
	pContact->put_CompanyName(CComBSTR("Microsoft"));
	pContact->put_HomeTelephoneNumber(CComBSTR("704-555-8888"));
	pContact->put_Email1Address(CComBSTR("someone@microsoft.com"));
	pContact->put_JobTitle(CComBSTR("Developer"));
	pContact->put_HomeAddress(CComBSTR("111 Main St.\nCharlotte, NC 28226"));

	// Save Contact
//j	pContact->Save();
	// Display
	pContact->Display(CComVariant((long)false/*bModal*/));

	pOutlookNamespace->Logoff();
}

/////////////////////////////////////////////////////////////////////////////
void COutlook::AddAppointment() 
{
	Outlook::_ApplicationPtr pOutlook(m_pApplication); 

	// Logon. Doesn't hurt if you are already running and logged on
	_NameSpacePtr pOutlookNamespace;
	pOutlook->GetNamespace(CComBSTR("MAPI"), &pOutlookNamespace);
	CComVariant varOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
	pOutlookNamespace->Logon(varOptional, varOptional, varOptional, varOptional);

	// Create a new appointment
	IDispatchPtr pCreateDispatch;
	pOutlook->CreateItem(Outlook::olAppointmentItem, &pCreateDispatch);

	Outlook::_AppointmentItemPtr pAppointment(pCreateDispatch); 

	// Schedule it for two minutes from now
	COleDateTime apptDate = COleDateTime::GetCurrentTime();   
	pAppointment->put_Start((DATE)apptDate + DATE(2.0/(24.0*60.0)));

	// Set other appointment info
	pAppointment->put_Duration(60);
	pAppointment->put_Subject(CComBSTR("Meeting to discuss plans"));

	pAppointment->put_Body(CComBSTR("Meeting with James to discuss plans."));
	pAppointment->put_Location(CComBSTR("Home Office"));
	pAppointment->put_ReminderMinutesBeforeStart(1);
	pAppointment->put_ReminderSet(TRUE);

	// Save the appointment
//j	pAppointment->Save();
	// Display
	pAppointment->Display(CComVariant((long)false/*bModal*/));

	pOutlookNamespace->Logoff();
}
