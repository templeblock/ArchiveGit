/*
// $Workfile: CDEFLATE.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// Copyright © 1996 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CDEFLATE.CPP $
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
//    Rev 1.0   14 Aug 1997 09:37:30   Fred
// Initial revision.
// 
//    Rev 1.7   05 Nov 1996 09:54:38   Jay
// Include change
// 
//    Rev 1.6   07 Aug 1996 17:49:30   Jay
//  
// 
//    Rev 1.5   02 Aug 1996 16:21:54   Jay
// No more breathing!
// 
//    Rev 1.4   24 May 1996 16:15:54   Fred
// TRACEx
// 
//    Rev 1.3   13 May 1996 09:22:32   Jay
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
*/
#include "stdafx.h"
#include "cdeflate.h"

#pragma pack(8)
#include "al201\h\arclib.h"
#include "al201\h\memstore.h"
#include "al201\h\pkengn.h"
#include "al201\h\filestor.h"
#pragma pack()

/////////////////////////////////////////////////////////////////////////////
// CPmwWinMemory

class CPmwWinMemory : public ALWinMemory
{
public:
	AL_PROTO CPmwWinMemory(const char AL_DLL_FAR *buffer_name = "",
								  char AL_HUGE *user_buffer = 0,
								  DWORD user_buffer_size = 0,
								  ALCase name_case = AL_MIXED );

	// This is the crux of the whole matter.
	virtual void AL_PROTO YieldTime(void);
};

CPmwWinMemory::CPmwWinMemory(const char AL_DLL_FAR *buffer_name /*=""*/,
									  char AL_HUGE *user_buffer /*=0*/,
									  DWORD user_buffer_size /*=0*/,
									  ALCase name_case /*=AL_MIXED*/)
				  : ALWinMemory(buffer_name, user_buffer, user_buffer_size, name_case)
{
}

void CPmwWinMemory::YieldTime(void)
{
	// Do nothing. In particular, don't breathe.
}

/////////////////////////////////////////////////////////////////////////////
// CDeflate

CDeflate::CDeflate()
{
}

BOOL CDeflate::Compress(LPBYTE pSource, DWORD dwSourceSize, LPBYTE* ppDestination, DWORD* pdwDestination)
{
	BOOL fResult = FALSE;

	*ppDestination = NULL;
	*pdwDestination = 0;

	CPmwWinMemory Source("Input", (LPSTR)pSource, dwSourceSize);
	CPmwWinMemory Destination("Output");

	ALPkCompressor Compressor(9);

	if (Compressor.Compress(Source, Destination) == AL_SUCCESS)
	{
		Destination.mfUserOwnsBuffer = TRUE;
		*ppDestination = (LPBYTE)(Destination.mpcUserBuffer);
		*pdwDestination = Destination.GetSize();

		fResult = TRUE;
	}

	return fResult;
}

BOOL CDeflate::Compress(LPCSTR pszFileName, LPBYTE* ppDestination, DWORD* pdwDestination)
{
	BOOL fResult = FALSE;

	*ppDestination = NULL;
	*pdwDestination = 0;

	ALFile Source(pszFileName);
	CPmwWinMemory Destination("Output");

	ALPkCompressor Compressor(9);

	if (Compressor.Compress(Source, Destination) == AL_SUCCESS)
	{
		Destination.mfUserOwnsBuffer = TRUE;
		*ppDestination = (LPBYTE)(Destination.mpcUserBuffer);
		*pdwDestination = Destination.GetSize();

		fResult = TRUE;
	}

	return fResult;
}

BOOL CDeflate::Decompress(LPBYTE pSource, DWORD dwSourceSize, LPCSTR pszFileName)
{
	BOOL fResult = FALSE;

	CPmwWinMemory Source("Input", (LPSTR)pSource, dwSourceSize);
	ALFile Destination(pszFileName);

	ALPkDecompressor Decompressor;
	if (Decompressor.Decompress(Source, Destination) == AL_SUCCESS)
	{
		fResult = TRUE;
	}
 
	return fResult;
}

BOOL CDeflate::Decompress(LPBYTE pSource, DWORD dwSourceSize, LPBYTE* ppDestination, DWORD* pdwDestination, LPBYTE pUserBuffer /*=NULL*/, DWORD dwUserBufferSize /*=0*/)
{
	BOOL fResult = FALSE;

	*ppDestination = NULL;
	*pdwDestination = 0;

	CPmwWinMemory Source("Input", (LPSTR)pSource, dwSourceSize);
	CPmwWinMemory Destination("Output", (char*)pUserBuffer, dwUserBufferSize);

	ALPkDecompressor Decompressor;
	if (Decompressor.Decompress(Source, Destination) == AL_SUCCESS)
	{
		Destination.mfUserOwnsBuffer = TRUE;
		*ppDestination = (LPBYTE)(Destination.mpcUserBuffer);
		*pdwDestination = Destination.GetSize();

		fResult = TRUE;
	}

	return fResult;
}

void CDeflate::FreeData(LPBYTE pData)
{
	if (pData != NULL)
	{
#ifdef WIN32
		HGLOBAL hData = ::GlobalHandle(pData);
#else
		HGLOBAL hData = (HGLOBAL)LOWORD(::GlobalHandle(SELECTOROF(pData)));
#endif

		if (hData != NULL)
		{
			::GlobalUnlock(hData);
			::GlobalFree(hData);
		}
	}
}
