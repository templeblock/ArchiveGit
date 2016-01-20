/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/IUPGRADE.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
// 2     1/21/98 4:14p Fredf
// Year 2000 compliance.
// 
//    Rev 1.0   14 Aug 1997 15:41:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:42   Fred
// Initial revision.
// 
//    Rev 1.1   12 Jun 1997 16:30:44   Fred
// PMG 4.0 changes. Time passed in.
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

#ifndef __IUPGRADE_H__
#define __IUPGRADE_H__

/*
// Event notification function.
*/

typedef void (FAR PASCAL* EVENT_NOTIFY_FUNCTION)(int nEvent, HWND hWndCurrent, int nID, DWORD dwData);

enum UpgradeEventNotifications
{
	UPGRADE_EVENT_DialogStartup			= 1,	/* A dialog is starting up. */
	UPGRADE_EVENT_DialogShutdown			= 2,	/* A dialog is shutting down. */
	UPGRADE_EVENT_DialogActivate			= 3,	/* A dialog is being activated. */
	UPGRADE_EVENT_DialogDeactivate		= 4,	/* A dialog is being deactivated. */
	UPGRADE_EVENT_Help						= 5	/* The user has pressed help. */
															/* (dwData is the help value) */
};

/*
// Register results.
*/

typedef long UPGRESULT;
#define UPGRESULT_Error							1
#define UPGRESULT_NotUpgraded					2
#define UPGRESULT_NowUpgraded					3

/*
// Upgrade stuff.
*/

#ifndef UPGENTRYPOINT
#define UPGENTRYPOINT UPGRESULT PASCAL FAR
#endif

extern "C" UPGENTRYPOINT Upgrade(HWND hWnd, EVENT_NOTIFY_FUNCTION pNotify, int nProductLevel, DWORD dwPPN, DWORD dwKey);

#endif
