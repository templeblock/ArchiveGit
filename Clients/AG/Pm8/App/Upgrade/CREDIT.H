/*
// $Workfile: CREDIT.H $
// $Revision: 1 $
// $Date: 3/03/99 6:13p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/UPGRADE/CREDIT.H $
// 
// 1     3/03/99 6:13p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:41:38   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 10:32:36   Fred
// Initial revision.
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
