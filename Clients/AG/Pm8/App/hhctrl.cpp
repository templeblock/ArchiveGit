/*
// $Workfile: hhctrl.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
// 
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1998 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/hhctrl.cpp $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 7     2/04/99 11:55a Rgrenfel
// Fixed assertions which were causing the app to abort without the
// assertion dialogs every showing themselves.
// 
// 6     1/20/99 11:54a Mwilson
// fixed help window
// 
// 5     7/21/98 6:45p Hforman
// added call to CloseAll() in destructor
// 
// 4     7/01/98 6:21p Hforman
// fixed GetWinType() params
// 
// 3     4/14/98 7:59p Fredf
// First attempt at Help Window.
// 
// 2     4/14/98 4:04p Fredf
// Ported from test project.
// 
// 1     4/14/98 1:03p Fredf
*/

#include "stdafx.h"
#include "hhctrl.h"

// Constructor
CHtmlHelpControl::CHtmlHelpControl()
{
	m_hInstance = NULL;
	m_pHtmlHelp = NULL;
}

// Destructor
CHtmlHelpControl::~CHtmlHelpControl()
{
	// Shut down the help control
	CloseAll();
	// Unload the control if we have it loaded.
	Unload();
}

// Load the Html Help control.
BOOL CHtmlHelpControl::Load(void)
{
	BOOL fResult = FALSE;

	// Unload any current instance.
	Unload();

	ASSERT(m_hInstance == NULL);
	ASSERT(m_pHtmlHelp == NULL);

	// Load the control.
	m_hInstance = LoadLibrary("hhctrl.ocx");
	ASSERT(m_hInstance != NULL);

	if (m_hInstance != NULL)
	{
		// Get the address of the HtmlHelp() entry point.
		m_pHtmlHelp = (PFNHTMLHELP)GetProcAddress(m_hInstance, ATOM_HTMLHELP_API);
		if (m_pHtmlHelp != NULL)
		{
			fResult = TRUE;
		}
	}

	ASSERT(fResult);

	if (!fResult)
	{
		// Something went wrong, clean up.
		Unload();
	}

	return fResult;
}

// Unload any current instance of the Html Help control.
BOOL CHtmlHelpControl::Unload(void)
{
	if (m_hInstance != NULL)
	{
		if (m_pHtmlHelp != NULL)
		{
			// Close any windows associated with the current process.
			// Of course, any handles to that windows are now invalid
			// so let's hope there aren't any lying around.
			m_pHtmlHelp(NULL, NULL, HH_CLOSE_ALL, 0);
			m_pHtmlHelp = NULL;
		}

		// Free the control.
		FreeLibrary(m_hInstance);
		m_hInstance = NULL;
	}

	return TRUE;
}

// Generic entry point.
HWND CHtmlHelpControl::HtmlHelp(HWND hwndCaller, LPCSTR pszFile, UINT uCommand, DWORD dwData)
{
	// Make sure we have the control loaded.
//	ASSERT(m_hInstance != NULL);	// Removed assertions since it was causing PM not to shut down correctly
//	ASSERT(m_pHtmlHelp != NULL);	// which was causing massive memory leaks. -RIP-
	if ((m_hInstance == NULL) || (m_pHtmlHelp == NULL))
	{
		return NULL;
	}

	// Call the control.
	return m_pHtmlHelp(hwndCaller, pszFile, uCommand, dwData);
}

// Display a specific topic.
HWND CHtmlHelpControl::DisplayTopic(HWND hwndCaller, LPCSTR pszFile, LPCSTR pszTopic /*=NULL*/)
{
	return HtmlHelp(hwndCaller, pszFile, HH_DISPLAY_TOPIC, (DWORD)pszTopic);
}

// Display a context help.
HWND CHtmlHelpControl::HelpContext(HWND hwndCaller, LPCSTR pszFile, DWORD dwContext)
{
	return HtmlHelp(hwndCaller, pszFile, HH_HELP_CONTEXT, dwContext);
}

// Create or modify a window type.
HWND CHtmlHelpControl::SetWinType(HH_WINTYPE* pWinType)
{
	return HtmlHelp(NULL, NULL, HH_SET_WIN_TYPE, (DWORD)pWinType);
}

// Get a window type.
HWND CHtmlHelpControl::GetWinType(LPCSTR pszType, HH_WINTYPE** ppWinType)
{
	return HtmlHelp(NULL, pszType, HH_GET_WIN_TYPE, (DWORD)ppWinType);
}

// Get the handle for a particular window type.
HWND CHtmlHelpControl::GetWinHandle(LPCSTR pszHelpFile, LPCSTR pszType)
{
	return HtmlHelp(NULL, pszHelpFile, HH_GET_WIN_HANDLE, (DWORD)pszType);
}

// Close all help windows associated with the current process.
void CHtmlHelpControl::CloseAll(void)
{
	HtmlHelp(NULL, NULL, HH_CLOSE_ALL, 0);
}
