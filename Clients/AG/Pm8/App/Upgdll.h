/*
// $Workfile: UPGDLL.H $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGDLL.H $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 2     1/21/98 4:25p Fredf
// Year 2000 compliance.
// 
//    Rev 1.0   14 Aug 1997 15:26:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:36:24   Fred
// Initial revision.
// 
//    Rev 1.3   12 Jun 1997 16:46:58   Fred
// Changes for 4.0 instant upgrade
// 
//    Rev 1.2   07 May 1996 12:45:10   Jay
// From PMW2
// 
//    Rev 1.3   24 Apr 1996 16:44:44   JAY
// New demo support
// 
//    Rev 1.2   27 Mar 1996 14:59:56   JAY
// Fixed upgrade parent window problem
// 
//    Rev 1.1   12 Feb 1996 14:16:30   JAY
// Added ProductLevel param to Upgrade() call.
// 
//    Rev 1.0   02 Feb 1996 13:10:28   JAY
// Initial revision.
*/ 

#ifndef __UPGDLL_H__
#define __UPGDLL_H__

#include "pmwdlg.h"
#include "upgrade\iupgrade.h"

/////////////////////////////////////////////////////////////////////////////
// CUpgradeDLL class

class CUpgradeDLL
{
	typedef UPGRESULT (FAR PASCAL *FN_UPGRADE)(HWND hWnd, EVENT_NOTIFY_FUNCTION pNotify, int nProductLevel, DWORD dwPPN, DWORD dwKey);
public:
	CUpgradeDLL();
	virtual ~CUpgradeDLL();

	BOOL Startup(void);
	void Shutdown(void);

	UPGRESULT Upgrade(CWnd* pWnd = NULL);

	BOOL IsValid(void) const
		{ return m_hLibrary != 0; }

#ifdef WIN32
	static void PASCAL EventNotify(int nEvent, HWND hWndCurrent, int nID, DWORD dwData);
#else
	static void PASCAL __export EventNotify(int nEvent, HWND hWndCurrent, int nID, DWORD dwData);
#endif

protected:
	void GetEntryPoints(void);

	FN_UPGRADE m_Upgrade;
	HINSTANCE m_hLibrary;
};

#endif
