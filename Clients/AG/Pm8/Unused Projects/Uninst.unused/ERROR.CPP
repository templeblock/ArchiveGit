/*
// $Header: /PM8/UNINST/ERROR.CPP 1     3/03/99 6:36p Gbeddow $
//
// $Log: /PM8/UNINST/ERROR.CPP $
// 
// 1     3/03/99 6:36p Gbeddow
// 
// 1     6/24/98 7:38a Mwilson
// 
// 1     6/22/98 10:03a Mwilson
// 
//    Rev 1.0   25 Apr 1997 09:24:26   Fred
// Initial revision.
// 
//    Rev 1.0   20 Feb 1997 13:55:46   Fred
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
