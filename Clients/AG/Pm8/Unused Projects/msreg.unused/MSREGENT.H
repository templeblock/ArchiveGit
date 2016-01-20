/*
// MSREGX.DLL Entry point definitions
//
// $Workfile: MSREGENT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:23p $
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/MSREGENT.H $
// 
// 1     3/03/99 6:23p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
// 
//    Rev 1.2   05 Dec 1995 13:03:36   JAY
// Event notification.
// 
//    Rev 1.1   22 Nov 1995 13:06:42   JAY
//  
// 
//    Rev 1.0   22 Nov 1995 08:52:00   FRED
// Initial revision.
// 
//    Rev 1.0   16 Nov 1995 10:31:38   FRED
//  
*/

#ifndef __MSREGENT_H__
#define __MSREGENT_H__

// Prototype and flags for RegSendCommand() command (in MSREGnn.DLL)
typedef REGRESULT (FAR PASCAL *FN_REGSENDCOMMAND)(HWND hWnd, LPCSTR pszCommand, DWORD dwData, LPCSTR pszMsregxDllPath);

// Prototype and flags for Register() command (in MSREGXnn.DLL)
typedef REGRESULT (FAR PASCAL *FN_REGISTER)(HWND hWnd, DWORD dwData, LPCSTR pszIniFile, EVENT_NOTIFY_FUNCTION pNotify);

#endif
