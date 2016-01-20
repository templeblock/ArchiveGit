/*
// $Workfile: KEY.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/KEY.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 1     11/06/97 6:01p Hforman
// generates CRCs
// 
//    Rev 1.0   22 Nov 1995 10:26:50   FRED
// Initial revision.
// 
//    Rev 1.0   10 Nov 1995 16:29:42   FRED
// Initial revision.
*/ 

#ifndef __KEY_H__
#define __KEY_H__

unsigned long CalculateBlock( const unsigned char *buffer, unsigned int count, unsigned long crc );
unsigned long KeyFromString(LPCSTR pString);

#endif
