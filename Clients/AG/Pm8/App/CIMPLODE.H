/*
// $Header: /PM8/App/CIMPLODE.H 1     3/03/99 6:04p Gbeddow $
//
// $Log: /PM8/App/CIMPLODE.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:02   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:43:54   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:36   FRED
// Initial revision.
// 
//    Rev 1.2   26 Dec 1995 10:23:08   FRED
// Project structure changes for mac port
// 
//    Rev 1.1   19 Sep 1995 11:30:54   JAY
// 32-bit version.
// 
//    Rev 1.0   06 Jun 1995 10:09:02   JAY
// Initial revision.
*/

/*
// PKWARE implode wrapper.
*/

#ifndef __CIMPLODE_H__
#define __CIMPLODE_H__

#ifdef WIN32
#include "implod32.h"
#else
#include "implod16.h"
#endif

class CImplode
{
public:
	CImplode();
	int Compress(LPBYTE pSource, DWORD dwSourceSize, LPBYTE pDestination, DWORD* pdwDestSize = NULL);
	// You can pass (DWORD)-1 for dwSourceSize here if you don't know it.
	int Decompress(LPBYTE pSource, DWORD dwSourceSize, LPBYTE pDestination, DWORD* pdwDestSize = NULL);
protected:
	static LPBYTE m_pSource;
	static DWORD m_dwSourceLeft;
	static LPBYTE m_pDestination;
	static DWORD m_dwDestWritten;
#ifdef WIN32
	static unsigned int ReadData(char* pBuffer, unsigned int *pSize, void* pParam);
	static void WriteData(char* pBuffer, unsigned int *pSize, void* pParam);
#else
	static unsigned far pascal ReadData(char far* pBuffer, unsigned short far *pSize);
	static unsigned far pascal WriteData(char far* pBuffer, unsigned short far *pSize);
#endif
};

#define CMP_MEMORY 100

#endif					// #ifndef __CIMPLODE_H__
