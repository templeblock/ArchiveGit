/*
// $Workfile: WSINST.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:14p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/WSINST.CPP $
// 
// 1     3/03/99 6:14p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:27:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:41:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:35:48   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:31:54   FRED
// Initial revision.
// 
//    Rev 2.7   02 Feb 1996 13:09:30   JAY
//  
*/ 

#include <windows.h>

// Main entry point.

int PASCAL far WinMain(HINSTANCE hInstance,
							  HINSTANCE hPrevInstance,
							  LPSTR pszCommandLine,
							  int nCmdShow)
{
	return WinExec("msrun WorkstationInstall", SW_SHOWNORMAL) >= 32;
}

