/*
// $Header: /PM8/App/COMPRESS.H 1     3/03/99 6:04p Gbeddow $
//
// Compress/uncompress routines defined in COMP32.CPP.
//
// $Log: /PM8/App/COMPRESS.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:28   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:14   Fred
// Initial revision.
// 
//    Rev 1.0   08 Nov 1996 08:57:48   Jay
// Removed 16-bit compress.obj
*/

#ifndef __COMPRESS_H__
#define __COMPRESS_H__

typedef BOOL (*BYTE_FUNC)(LPBYTE);

extern SHORT uncompress(BYTE_FUNC NextByte, LPBYTE pBuffer, SHORT nBits);
extern BOOL unPackBits(BYTE_FUNC NextByte, LPBYTE pBuffer, SHORT nBytes);

#endif
