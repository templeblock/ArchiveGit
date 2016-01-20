/*
// $Workfile: CDEFLATE.H $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CDEFLATE.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 2     8/20/98 5:52p Mwilson
// added routines for compressing and decompressing to a file
// 
// 1     6/18/98 4:01p Mwilson
// 
//    Rev 1.0   14 Aug 1997 15:18:52   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:32:58   Fred
// Initial revision.
// 
//    Rev 1.3   07 Aug 1996 17:49:30   Jay
//  
// 
//    Rev 1.2   10 May 1996 16:00:28   Jay
// Added FreeData()
// 
//    Rev 1.1   10 May 1996 14:32:42   Fred
//  
// 
//    Rev 1.0   10 May 1996 14:05:26   Fred
// CDeflate
// 

/*
// Greenleaf Deflate (PKWARE 2.0 compatible) wrapper.
*/

#ifndef __CDEFLATE_H__
#define __CDEFLATE_H__ 

class CDeflate
{
public:
	CDeflate();
	int Compress(LPBYTE pSource, DWORD dwSourceSize, LPBYTE* ppDestination, DWORD* pdwDestination);
	int Compress(LPCSTR pszFileName, LPBYTE* ppDestination, DWORD* pdwDestination);
	int Decompress(LPBYTE pSource, DWORD dwSourceSize, LPBYTE* ppDestination, DWORD* pdwDestination, LPBYTE pUserBuffer = NULL, DWORD dwUserBufferSize = 0);
	int Decompress(LPBYTE pSource, DWORD dwSourceSize, LPCSTR pszFileName);

	static void FreeData(LPBYTE pData);
};

#endif					// #ifndef __CDEFLATE_H__
