/*
// Revision History:
//
// $Log: /PM8/App/LICDLL/LICDLL.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:36:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:20:24   Fred
// Initial revision.
// 
//    Rev 1.0   10 Jun 1997 15:44:20   Fred
// Cleanup
// 
//    Rev 1.0   10 Jun 1997 14:47:06   Fred
// Initial revision.
// 
//    Rev 1.0   08 Sep 1996 09:40:50   Fred
// Initial revision.
// 
//    Rev 1.2   24 Apr 1996 16:42:40   JAY
// Now gets PPN from entry. Demo handling.
// 
//    Rev 1.1   12 Feb 1996 13:53:46   JAY
// Added product level param for correct product
// 
//    Rev 1.0   02 Feb 1996 08:59:12   JAY
// Initial revision.
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

#ifndef __LICENSE_H__
#define __LICENSE_H__

/*
// Event notification function.
*/

typedef void (FAR PASCAL* EVENT_NOTIFY_FUNCTION)(int nEvent, HWND hWndCurrent, int nID, DWORD dwData);

enum LicenseEventNotifications
{
	LICENSE_EVENT_DialogStartup			= 1,	/* A dialog is starting up. */
	LICENSE_EVENT_DialogShutdown			= 2,	/* A dialog is shutting down. */
	LICENSE_EVENT_DialogActivate			= 3,	/* A dialog is being activated. */
	LICENSE_EVENT_DialogDeactivate		= 4,	/* A dialog is being deactivated. */
	LICENSE_EVENT_Help						= 5	/* The user has pressed help. */
															/* (dwData is the help value) */
};

/*
// Register results.
*/

typedef long LICRESULT;
#define LICRESULT_Error							1
#define LICRESULT_NotLicensed					2
#define LICRESULT_NowLicensed					3

/*
// Upgrade stuff.
*/

#ifndef LICENTRYPOINT
#define LICENTRYPOINT LICRESULT PASCAL FAR
#endif

#endif
