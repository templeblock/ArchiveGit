/*
// $Workfile: KEY.H $
// $Revision: 1 $
// $Date: 3/03/99 6:22p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/msreg/KEY.H $
// 
// 1     3/03/99 6:22p Gbeddow
// 
// 1     7/09/98 2:27p Jayn
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
