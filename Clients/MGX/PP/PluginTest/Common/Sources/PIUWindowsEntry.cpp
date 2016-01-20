//-------------------------------------------------------------------------------
//
//	File:
//		PIUWindowsEntry.cpp
//
//	Copyright 1996-1998, Adobe Systems Incorporated.
//	All Rights Reserved.
//
//	Description:
//		This file contains the Windows DLL initialization and
//		termination code.
//
//	Use:
//		Use this as part of the Plug-in Utilities to automatically
//		load and dispatch your plug-in.
//
//	Version history:
//		Version 1.0.0	12/15/1997	Created for Photoshop 5.0.
//			Written by Andrew Coven.
//
//-------------------------------------------------------------------------------

#include <windows.h>
#include "PIUSuites.h"

/* ------------------------------------------------
 *   Code from Borland's window's dll example code.
 * ------------------------------------------------
 */
#if defined(__BORLANDC__)
// Turn off warning: Parameter '' is never used; effects next function only
#pragma argsused
#endif

#ifdef WIN32

// Every 32-Bit DLL has an entry point DLLInit

bool APIENTRY DllMain(HANDLE hInstance, DWORD fdwReason, LPVOID lpReserved)
{

	if (fdwReason == DLL_PROCESS_ATTACH)
		{
		sPSUIHooks.SetDLLInstance(hInstance);
		}

	return true;   // Indicate that the DLL was initialized successfully.
}

#else
// Every 16-Bit DLL has an entry point LibMain and an exit point WEP.
int FAR PASCAL LibMain( HANDLE hInstance, WORD wDataSegment,
								   WORD wHeapSize, LPSTR lpszCmdLine )
{
	// Required when using Zortech; causes blink to include startup code
	extern __acrtused_dll;

	// The startup code for the DLL initializes the local heap (if there is one)
	// with a call to LocalInit which locks the data segment.
	if ( wHeapSize != 0 )
		UnlockData( 0 );

	hDllInstance = hInstance;
	return 1;   // Indicate that the DLL was initialized successfully.
}

int FAR PASCAL WEP(int nParam)
{
	switch  (nParam) {
	  case  WEP_SYSTEM_EXIT: // System shutdown in progress
	  case  WEP_FREE_DLL   : // DLL use count is 0
	  default :              // Undefined;  ignore
			return  1;
	}
}
#endif // Win32

//-------------------------------------------------------------------------------
// end WindowsEntry
