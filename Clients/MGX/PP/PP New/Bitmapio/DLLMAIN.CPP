/*=========================================================================*\

	(c) Copyright 1993 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=========================================================================*/

/*=========================================================================*\
	dllmain.c
\*=========================================================================*/

#include "stdafx.h"
#include <process.h>
#include "dllmain.h"

CBitmapDllApp BitmapDllApp;

CBitmapDllApp::CBitmapDllApp()
{
	//::MessageBox(NULL, "Constructor", "BITMAPD.DLL", MB_OK | MB_ICONEXCLAMATION );
	m_hLeadMutex = NULL;
}

CBitmapDllApp::~CBitmapDllApp()
{
	//::MessageBox(NULL, "Destructor", "BITMAPD.DLL", MB_OK | MB_ICONEXCLAMATION );
	if (m_hLeadMutex)
		CloseHandle(m_hLeadMutex);
}

BOOL CBitmapDllApp::InitInstance()
{
	//::MessageBox(NULL, "InitInstance", "BITMAPD.DLL", MB_OK | MB_ICONEXCLAMATION );
	if (!m_hLeadMutex)
		m_hLeadMutex = CreateMutex(NULL, FALSE, NULL);
	return(TRUE);
}

int CBitmapDllApp::ExitInstance()
{
	//::MessageBox(NULL, "ExitInstance", "BITMAPD.DLL", MB_OK | MB_ICONEXCLAMATION );
	if (m_hLeadMutex)
		CloseHandle(m_hLeadMutex);
	m_hLeadMutex = NULL;
	return (CWinApp::ExitInstance());
}

void CBitmapDllApp::BlockJPGRead()
{
	if (m_hLeadMutex)
		WaitForSingleObject(m_hLeadMutex, INFINITE);
}

void CBitmapDllApp::ReleaseJPGRead()
{
	if (m_hLeadMutex)
		ReleaseMutex(m_hLeadMutex);
}

void CBitmapDllApp::BlockJPGWrite()
{
	if (m_hLeadMutex)
		WaitForSingleObject(m_hLeadMutex, INFINITE);
}

void CBitmapDllApp::ReleaseJPGWrite()
{
	if (m_hLeadMutex)
		ReleaseMutex(m_hLeadMutex);
}

#ifdef UNUSED
extern "C" BOOL WINAPI
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID)
{
	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
		hLeadMutex = CreateMutex(NULL, FALSE, NULL);
		//::MessageBox(NULL, "DLL_PROCESS_ATTACH", "BITMAPD.DLL", MB_OK | MB_ICONEXCLAMATION );
    	break;

		case DLL_PROCESS_DETACH:
		if (hLeadMutex)
			CloseHandle(hLeadMutex);
		//::MessageBox(NULL, "DLL_PROCESS_DETACH", "BITMAPD.DLL", MB_OK | MB_ICONEXCLAMATION );
		break;

		case DLL_THREAD_ATTACH:
		//::MessageBox(NULL, "DLL_THREAD_ATTACH", "BITMAPD.DLL", MB_OK | MB_ICONEXCLAMATION );
    	break;

		case DLL_THREAD_DETACH:
		//::MessageBox(NULL, "DLL_THREAD_DETACH", "BITMAPD.DLL", MB_OK | MB_ICONEXCLAMATION );
		break;

    	default:
     	break;
	}
	return TRUE;
}

#endif
