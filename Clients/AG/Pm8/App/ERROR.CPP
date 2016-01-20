/*
// $Header: /PM8/Download/ERROR.CPP 1     3/03/99 6:15p Gbeddow $
//
// $Log: /PM8/Download/ERROR.CPP $
// 
// 1     3/03/99 6:15p Gbeddow
// 
// 2     10/12/98 3:54p Jayn
// Changes for download app.
// 
//    Rev 1.0   14 Aug 1997 15:20:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:22   Fred
// Initial revision.
// 
//    Rev 1.1   28 Aug 1996 21:25:02   Fred
// ThrowErrorcodeException is now a function
// 
//    Rev 1.0   11 Apr 1996 15:35:54   Jay
//  
*/

#include "stdafx.h"
#include "error.h"

IMPLEMENT_DYNAMIC(CErrorcodeException, CException);

void ThrowErrorcodeException(ERRORCODE error)
{
	THROW(new CErrorcodeException(error));
}
