/*
// $Workfile: CARDFTP.H $
// $Revision: 1 $
// $Date: 3/03/99 6:03p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CARDFTP/CARDFTP.H $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 2     6/05/98 4:41p Jayn
// Now passes the URL to the DLL.
// 
//    Rev 1.0   14 Aug 1997 15:35:26   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:12:42   Fred
// Initial revision.
// 
//    Rev 1.0   10 Jul 1997 12:30:50   Jay
// Initial revision.
*/

#ifndef __CARDFTP_H__
#define __CARDFTP_H__

// Additional error messages.
#define WSAENOENT (WSABASEERR+2)			// 2 == ENOENT

// Card FTP DLL notifications.
enum
{
	CARDFTP_Initializing,
	CARDFTP_Connecting,
	CARDFTP_SendingTextFile,
	CARDFTP_SendingFront,
	CARDFTP_SendingInside,
	CARDFTP_Closing,
	CARDFTP_Aborting
};

// The prototype for a notify function (DLL back to caller).

typedef void (WINAPI* CARDFTP_NOTIFY)(int nNotify, LPVOID pNotifyData);

// The prototype for the DLL entry point.
// Note that pNotifyData is only used with pNotify (it is the data passed
// through to the callback function).
// pNotify can be NULL if no notifications are desired.

typedef int (WINAPI* CARDFTP_SENDFILES)(LPCSTR pTheDir, LPCSTR pszURL, CARDFTP_NOTIFY pNotify, LPVOID pNotifyData);

#endif
