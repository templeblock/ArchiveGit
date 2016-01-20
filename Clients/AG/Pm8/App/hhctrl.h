/*
// $Workfile: hhctrl.h $
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
// $Log: /PM8/App/hhctrl.h $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 6     1/20/99 11:54a Mwilson
// fixed help window
// 
// 5     7/01/98 6:21p Hforman
// fixed GetWinType() params
// 
// 4     4/23/98 7:23p Fredf
// Added IsValid().
// 
// 3     4/14/98 7:59p Fredf
// First attempt at Help Window.
// 
// 2     4/14/98 4:04p Fredf
// Ported from test project.
// 
// 1     4/14/98 1:05p Fredf
*/

// This is the header file for the object that encapsulates the
// Html Help control, HHCTRL.OCX

#ifndef __HHCTRL_H__
#define __HHCTRL_H__

#include "htmlhelp.h"

// Prototype for HtmlHelp() entry point in HHCTRL.OCX
typedef HWND (WINAPI *PFNHTMLHELP)(HWND hwndCaller, LPCSTR pszFile, UINT uCommand, DWORD dwData);

class CHtmlHelpControl
{
public:

	// Construction
	CHtmlHelpControl();
	virtual ~CHtmlHelpControl();

public:

	// Initialization.
	BOOL Load(void);
	BOOL Unload(void);

	// Test if valid.
	BOOL IsValid(void)
		{ return (m_hInstance != NULL) && (m_pHtmlHelp != NULL); }

public:

	// Generic entry point.
	HWND HtmlHelp(HWND hwndCaller, LPCSTR pszFile, UINT uCommand, DWORD dwData);

	// Wrappers for common functions.

	// Display a specific topic.
	HWND DisplayTopic(HWND hwndCaller, LPCSTR pszFile, LPCSTR pszTopic = NULL);

	// Display the default topic.
	HWND DisplayContents(HWND hwndCaller, LPCSTR pszFile)
		{ return DisplayTopic(hwndCaller, pszFile); }

	// Display a context help.
	HWND HelpContext(HWND hwndCaller, LPCSTR pszFile, DWORD dwContext);

	// Create or modify a window type.
	HWND SetWinType(HH_WINTYPE* pWinType);

	// Get a window type.
	HWND GetWinType(LPCSTR pszType, HH_WINTYPE** ppWinType);

	// Get the handle for a particular window type.
	HWND GetWinHandle(LPCSTR pszHelpFile, LPCSTR pszType);

	// Close all help windows associated with the current process.
	void CloseAll(void);

protected:

	// Instance handle.
	HINSTANCE m_hInstance;

	// Pointer to HtmlHelp() entry point.
	PFNHTMLHELP m_pHtmlHelp;
};

#ifdef UNICODE
#define ATOM_HTMLHELP_API ATOM_HTMLHELP_API_UNICODE
#else
#define ATOM_HTMLHELP_API ATOM_HTMLHELP_API_ANSI
#endif

#endif
