/*
// $Header: /PM8/App/CIMPLODE.CPP 1     3/03/99 6:04p Gbeddow $
//
// PKWARE Imploder/Exploder
//
// $Log: /PM8/App/CIMPLODE.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:00   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:34   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:44:46   Jay
// Initial revision.
// 
//    Rev 1.1   13 Mar 1996 14:44:12   VSI
// _MAC specific changes just to compile -- not working on Mac yet.
// 
//    Rev 1.0   16 Feb 1996 12:09:56   FRED
// Initial revision.
// 
//    Rev 1.2   19 Sep 1995 11:31:02   JAY
// 32-bit version.
// 
//    Rev 1.1   13 Sep 1995 11:33:28   JAY
// New 32-bit stuff
// 
//    Rev 1.0   06 Jun 1995 10:09:02   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "cimplode.h"

/////////////////////////////////////////////////////////////////////////////
// CImplode

CImplode::CImplode()
{
}

#ifndef _MAC

int CImplode::Compress(LPBYTE pSource, DWORD dwSourceSize, LPBYTE pDestination, DWORD* pdwDestSize /*=NULL*/)
{
	int nRet = CMP_MEMORY;
	m_dwDestWritten = 0;

	TRY
	{
		LPBYTE pWorkSpace = new BYTE[36312];			// Larger value for 32-bit library

		m_pSource = pSource;
		m_dwSourceLeft = dwSourceSize;
		m_pDestination = pDestination;

#ifdef WIN32
		unsigned int wType, wDSize;
#else
		WORD wType, wDSize;
#endif
		wType = CMP_BINARY;
		if (dwSourceSize > 4096)
		{
			wDSize = 4096;
		}
		else if (dwSourceSize > 2048)
		{
			wDSize = 2048;
		}
		else
		{
			wDSize = 1024;
		}

#ifdef WIN32
		nRet = implode(ReadData, WriteData, (LPSTR)pWorkSpace, NULL, &wType, &wDSize);
#else
		nRet = implode(ReadData, WriteData, (LPSTR)pWorkSpace, &wType, &wDSize);
#endif

		delete [] pWorkSpace;
	}
	END_TRY

	if (pdwDestSize != NULL)
	{
		*pdwDestSize = m_dwDestWritten;
	}
	return nRet;
}

int CImplode::Decompress(LPBYTE pSource, DWORD dwSourceSize, LPBYTE pDestination, DWORD* pdwDestSize /*=NULL*/)
{
	int nRet = CMP_MEMORY;
	m_dwDestWritten = 0;

	TRY
	{
		LPBYTE pWorkSpace = new BYTE[12596];			// Larger value for 32-bit library

		m_pSource = pSource;
		m_dwSourceLeft = dwSourceSize;
		m_pDestination = pDestination;

#ifdef WIN32
		nRet = explode(ReadData, WriteData, (LPSTR)pWorkSpace, NULL);
#else
		nRet = explode(ReadData, WriteData, (LPSTR)pWorkSpace);
#endif
		delete [] pWorkSpace;
	}
	END_TRY

	if (pdwDestSize != NULL)
	{
		*pdwDestSize = 0;
	}
	return nRet;
}

LPBYTE CImplode::m_pSource;
DWORD CImplode::m_dwSourceLeft;
LPBYTE CImplode::m_pDestination;
DWORD CImplode::m_dwDestWritten;

#ifdef WIN32
unsigned int CImplode::ReadData(char* pBuffer, unsigned int* pSize, void* pParam)
{
	WORD wSize = *pSize;

	if (wSize > m_dwSourceLeft)
	{
		wSize = (WORD)m_dwSourceLeft;
	}

	if (wSize != 0)
	{
		memcpy(pBuffer, m_pSource, wSize);
		m_dwSourceLeft -= wSize;
		m_pSource += wSize;
	}
	return wSize;
}

void CImplode::WriteData(char* pBuffer, unsigned int* pSize, void* pParam)
{
	WORD wSize = *pSize;
	if (m_pDestination != NULL)
	{
		memcpy(m_pDestination, pBuffer, wSize);
		m_pDestination += wSize;
	}
	m_dwDestWritten += wSize;
}
#else
unsigned far pascal CImplode::ReadData(char far* pBuffer, unsigned short far *pSize)
{
	WORD wSize = *pSize;

	if (wSize > m_dwSourceLeft)
	{
		wSize = (WORD)m_dwSourceLeft;
	}

	if (wSize != 0)
	{
		memcpy(pBuffer, m_pSource, wSize);
		m_dwSourceLeft -= wSize;
		m_pSource += wSize;
	}
	return wSize;
}

unsigned far pascal CImplode::WriteData(char far* pBuffer, unsigned short far *pSize)
{
	WORD wSize = *pSize;
	if (m_pDestination != NULL)
	{
		memcpy(m_pDestination, pBuffer, wSize);
		m_pDestination += wSize;
	}
	m_dwDestWritten += wSize;
	return wSize;
}
#endif

#else // !_MAC

// TODO:MAC -- do we use PKWare for Mac?? Port It or what??
int CImplode::Compress(LPBYTE pSource, DWORD dwSourceSize, LPBYTE pDestination, DWORD* pdwDestSize /* = NULL */)
{ return 0;}

int CImplode::Decompress(LPBYTE pSource, DWORD dwSourceSize, LPBYTE pDestination, DWORD* pdwDestSize /* = NULL */)
{ return 0; }

unsigned int CImplode::ReadData(char* pBuffer, unsigned int *pSize, void* pParam)
{ return 0; }

void CImplode::WriteData(char* pBuffer, unsigned int *pSize, void* pParam)
{}

#endif // _MAC