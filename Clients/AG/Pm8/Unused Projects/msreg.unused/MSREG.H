
/*
// Mindscape Registration Program definitions.
//
// (* Note: A tabstop == 3 spaces. *)
//
// $Workfile: MSREG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/MSREG.H $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.5   07 Dec 1995 10:53:08   FRED
// Added RegisterInstall and RegisterStandAlone variants
// 
//    Rev 1.4   05 Dec 1995 13:03:48   JAY
// Event notification.
// 
//    Rev 1.3   28 Nov 1995 09:53:02   FRED
//  
// 
//    Rev 1.2   28 Nov 1995 09:47:28   FRED
// IsBonusEnabled and GetRegistrationData
// 
//    Rev 1.1   22 Nov 1995 13:06:44   JAY
//  
// 
//    Rev 1.0   22 Nov 1995 08:51:58   FRED
// Initial revision.
// 
//    Rev 1.3   16 Nov 1995 10:31:28   FRED
//  
// 
//    Rev 1.2   09 Nov 1995 09:29:12   FRED
// DOS Changes.
// 
// 
//    Rev 1.0   09 Nov 1995 09:20:50   JAY
// Initial revision.
*/

#ifndef __MSREG_H__
#define __MSREG_H__

/*
// Register results.
*/

typedef long REGRESULT;
#define REGRESULT_CommandError				1
#define REGRESULT_Error							2
#define REGRESULT_NotRegistered				3
#define REGRESULT_AlreadyRegistered			4
#define REGRESULT_NowRegistered				5

/*
// Command strings.
*/

#define REGCOMMAND_Register					"Register"
#define REGCOMMAND_RegisterInstall			"Register /C"
#define REGCOMMAND_RegisterStandAlone		"Register /C /A"
#define REGCOMMAND_Register					"Register"
#define REGCOMMAND_IsRegistered				"IsRegistered"
#define REGCOMMAND_IsBonusEnabled			"IsBonusEnabled"
#define REGCOMMAND_GetRegistrationData		"GetRegistrationData"
#define REGCOMMAND_SetEventNotify			"SetEventNotify"

/*
// Windows support.
// Determine if we are compiling for Windows. If so, enable the prototype.
*/

/* Most compilers set this. */
#ifdef _WINDOWS
#define MSREG_WINDOWS
#endif
/* Some compilers set this. */
#ifdef _Windows
#define MSREG_WINDOWS
#endif

#ifdef MSREG_WINDOWS

#ifndef _INC_WINDOWS
#define STRICT						/* strict type-checking by default */
#include <windows.h>
#endif

/*
// Field access structure for REGCOMMAND_GetRegistrationData.
*/

typedef struct
{
	LPCSTR	m_pszField;			// Field to retrieve
	LPSTR		m_pszData;			// Place to put the data
	int		m_nData;				// Size of buffer pointed to by m_pszData
} REGDATA_GetRegistrationData;

/*
// Event notification function.
*/

typedef void (FAR PASCAL* EVENT_NOTIFY_FUNCTION)(int nEvent, HWND hWndCurrent, int nID, DWORD dwData);

enum EventNotifications
{
	EVENT_DialogStartup			= 1,			/* A dialog is starting up. */
	EVENT_DialogShutdown			= 2,			/* A dialog is shutting down. */
	EVENT_DialogActivate			= 3,			/* A dialog is being activated. */
	EVENT_DialogDeactivate		= 4,			/* A dialog is being deactivated. */
	EVENT_Help						= 5			/* The user has pressed help. */
														/* (dwData is the help value) */
};

/*
// Register entry point protocol.
*/

#ifndef REGENTRYPOINT
#define REGENTRYPOINT REGRESULT PASCAL FAR
#endif

/*
// Main entry point.
*/

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif		/* #ifdef __cplusplus */

/*
// RegSendCommand()
//
// hWnd					- Parent window (used as modal dialog parent/owner)
// pszCommand			- Command to be executed
// dwData				- Command-specific data
// pszMsregxDllPath	- path to MsregxDLL
//
// If pszMsregxDllPath is not NULL, it must be fully-qualified. The path
// must exist (e.g. the CD must be in the drive).
*/

REGENTRYPOINT RegSendCommand(HWND hWnd, LPCSTR pszCommand, DWORD dwData, LPCSTR pszMsregxDllPath);

#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif		/* #ifdef __cplusplus */
#endif		/* #ifdef MSREG_WINDOWS */

#endif		/* #ifndef __MSREG_H__ */
