/*
// $Workfile: credit.h $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/
/*
// Revision History:
//
// $Log: /PM8/App/credit.h $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 1     6/09/98 10:26a Mwilson
// 
//    Rev 1.0   20 May 1997 10:40:32   johno
// Initial revision.
//
//	  Rev 2.0   08 Apr 1997  Aaron Evans (aaron@situs.com)
// Extracted files/functions from \catalog and \upgrade
// from PrintMaster 3.0 to create a generic unlock DLL. 
//
*/

/*
// Revision History:
//
// $Log: /PM8/App/credit.h $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 1     6/09/98 10:26a Mwilson
// 
//    Rev 1.0   08 Sep 1996 09:40:46   Fred
// Initial revision.
// 
//    Rev 1.1   01 Mar 1996 13:44:34   FRED
// IL sales Tax, and type-in info on mail upgrade
// 
//    Rev 1.0   02 Feb 1996 08:58:42   JAY
// Initial revision.
// 
//    Rev 1.0   31 Jan 1996 14:12:50   FRED
// Initial revision.
// 
//    Rev 1.0   18 Jan 1996 16:40:14   FRED
// Initial revision.
*/ 

class CCreditCard;

#ifndef __CREDIT_H__
#define __CREDIT_H__

class CCreditCard
{
public:
	enum
	{
		VISA					=	0,
		MASTERCARD			=	1,
		AMERICANEXPRESS	=	2,
		DISCOVER				=	3
	};
	
public:
	static BOOL Validate(int nType, const CString& csNumber, const CString& csDate);
	static BOOL NumberValid(int nType, const CString& csNumber);
	static BOOL DateValid(const CString& csDate);
	static BOOL CC2121(const CString& csNumber);
	static void PrettyCreditCardNumber(int nType, CString& csNumber);
};

#endif
