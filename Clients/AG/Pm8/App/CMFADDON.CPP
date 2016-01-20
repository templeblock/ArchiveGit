/*
// CMFADDON.CPP
//
//		Copyright (C) 1993 MicroLogic Software, Inc.
//
//	C++ interface to MFADDON.DLL
//
// $Header: /PM8/App/CMFADDON.CPP 1     3/03/99 6:04p Gbeddow $
//
// $Log: /PM8/App/CMFADDON.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:26:16   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:27:30   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:35:44   JAY
// Reverted. New series.
// 
//    Rev 1.1   04 Aug 1994 07:34:42   JAY
// Large model and 32-bit changes
// 
//    Rev 1.0   13 Jan 1994 17:04:20   JAY
//  
// 
//    Rev 1.0   29 Dec 1993 18:01:50   MACDUFF
// Initial revision.
*/


#include "stdafx.h"

#ifndef STRICT
#ifndef NOSTRICT
#define STRICT
#endif
#endif

#include <windows.h>
#include <string.h>
#include "cmfaddon.h"

CMFAddonDll::CMFAddonDll(LPCSTR lpszDllPath)
{
	strcpy(m_szDllPath, lpszDllPath);
	m_hLibrary = NULL;
	m_lpContext = NULL;
	 
	if ((WORD)(m_hLibrary = LoadLibrary(m_szDllPath)) >= 32)
	{
		m_lpMFAddonInit = (MFADDON_INIT_FN FAR *)GetProcAddress(m_hLibrary, "MFADDONINIT");
		m_lpMFAddonDone = (MFADDON_DONE_FN FAR *)GetProcAddress(m_hLibrary, "MFADDONDONE");
		m_lpMFAddonProcessFonts = (MFADDON_PROCESSFONTS_FN FAR *)GetProcAddress(m_hLibrary, "MFADDONPROCESSFONTS");

		if (m_lpMFAddonInit != NULL &&
			 m_lpMFAddonDone != NULL &&
			 m_lpMFAddonProcessFonts != NULL)
		{
			m_lpContext = (*m_lpMFAddonInit)();
		}
	}
}

CMFAddonDll::~CMFAddonDll()
{
	if (m_lpContext != NULL)
	{
		(*m_lpMFAddonDone)(m_lpContext);
		m_lpContext = NULL;
	}
	if (m_hLibrary != NULL)
	{
		FreeLibrary(m_hLibrary);
		m_hLibrary = NULL;
	}
}

BOOL CMFAddonDll::ProcessFonts(
		HWND hWindow,
		rAddonProcessFontsData FAR *lpProcessFontsData)
{
	lpProcessFontsData->TrueTypeFontsInstalled = 0;
	lpProcessFontsData->PostScriptFontsInstalled = 0;

	if (Valid())
	{
		return (*m_lpMFAddonProcessFonts)(m_lpContext, hWindow, lpProcessFontsData);
	}
	else
	{
		return FALSE;
	}
}

