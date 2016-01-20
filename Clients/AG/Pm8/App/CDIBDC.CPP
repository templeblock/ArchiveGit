/*
// $Header: /PM8/App/CDIBDC.CPP 1     3/03/99 6:04p Gbeddow $
//
// Dib DC.
//
// $Log: /PM8/App/CDIBDC.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:18:54   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:30   Fred
// Initial revision.
// 
//    Rev 1.1   21 Aug 1996 16:48:54   Jay
// Calls DeleteDC() in destructor if WIN32.
// 
//    Rev 1.0   14 Mar 1996 13:47:28   Jay
// Initial revision.
// 
//    Rev 1.1   13 Mar 1996 14:43:56   VSI
// _MAC specific changes just to compile -- not working on Mac yet.
// 
//    Rev 1.0   16 Feb 1996 12:09:22   FRED
// Initial revision.
// 
//    Rev 1.1   21 Sep 1995 13:45:44   JAY
// Now uses CreateDIBSection in Win32
// 
//    Rev 1.0   06 Jun 1995 10:08:44   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "cdibdc.h"
#include "file.h"

/////////////////////////////////////////////////////////////////////////////
// CDibDC

CDibDC::CDibDC()
{
#if defined(WIN32) && !defined(_MAC)
	m_hDibBitmap = NULL;
	m_hOldBitmap = NULL;
#endif
}

CDibDC::~CDibDC()
{
#if defined(WIN32) && !defined(_MAC)
	DeleteDC();

	if (m_hDibBitmap != NULL)
	{
		::DeleteObject(m_hDibBitmap);
	}
#endif
}

BOOL CDibDC::Create(LPBITMAPINFO pbi)
{
#if defined(WIN32) && !defined(_MAC)
	if (!CreateCompatibleDC(NULL))
	{
		return FALSE;
	}

	if ((m_hDibBitmap = CreateDIBSection(GetSafeHdc(),
													 pbi,
													 DIB_RGB_COLORS,
													 &m_pData,
													 NULL,
													 NULL)) == NULL)
	{
		DeleteDC();
		return FALSE;
	}

	m_hOldBitmap = (HBITMAP)::SelectObject(GetSafeHdc(), m_hDibBitmap);

	return TRUE;
#else
	return CreateDC(StorageFile::TranslateFileName("PMWDIB", FALSE), NULL, NULL, pbi);
#endif
}

#if defined(WIN32) && !defined(_MAC)
LPBYTE CDibDC::GetData(void)
{
	GdiFlush();					// For NT: make sure all drawing is finished.
	return (LPBYTE)m_pData;
}

BOOL CDibDC::DeleteDC(void)
{
// We need to select out the DIB bitmap before deleting.
	if (m_hOldBitmap != NULL)
	{
		::SelectObject(GetSafeHdc(), m_hOldBitmap);
		m_hOldBitmap = NULL;
	}
	return CDC::DeleteDC();
}

#endif
