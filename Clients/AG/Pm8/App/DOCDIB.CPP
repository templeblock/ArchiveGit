/*
// $Workfile: DOCDIB.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
// These are the routines used to generate DIB images of documents or
// portions of a document. This is an extension of pmwdoc.cpp.
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1997 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/DOCDIB.CPP $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 12    1/28/99 6:19p Johno
// Got rid of goofy "CDIBInfo is a member of CPmwDoc (not really)" syntax
// 
// 11    5/26/98 11:42a Jayn
// Fixed background banding problem.
// 
// 10    2/24/98 9:15a Dennis
// Added Free() to CDIBInfo().  Corrected problems with 24-bit DIB
// creation in BuildDIB() when in non-palletized mode.
// 
// 9     1/13/98 11:15a Jayn
// Transparent GIF support.
// 
// 8     11/25/97 10:33a Johno
// Got rid of CDIBInfo::PrepareData
// 
// 7     11/21/97 11:28a Johno
// 
// 6     11/20/97 10:36a Johno
// Added CDIBInfo::PrepareData for saving DIBs in other format
// files
// 
// 5     11/18/97 1:59p Johno
// BuildPanelDIB and BuildDIB now return ERRORCODE
// 
// 4     11/18/97 1:14p Johno
// BuildDIB had an ambiguous return value - does it want to be a BOOL?
// 
// 3     11/14/97 11:56a Johno
// 
// 2     11/14/97 11:26a Dennis
// Added parameters for CPtrList support and 24bit support
// 
//    Rev 1.0   14 Aug 1997 15:20:18   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:10   Fred
// Initial revision.
// 
//    Rev 1.2   21 Apr 1997 09:29:22   Jay
// 16-bit changes for huge DIBs
// 
//    Rev 1.1   18 Apr 1997 17:11:34   Jay
// Card server stuff
// 
//    Rev 1.0   15 Apr 1997 15:44:42   Jay
// Card Server phase #1
*/

#include "stdafx.h"
#include "pmw.h"
#include "docdib.h"
#include "cdibdc.h"
#include <math.h>
#include "util.h"

#ifndef WIN32
#include <malloc.h>
#endif

extern BYTE far screen_gamma_curve[];
extern CPalette *pOurPal;
extern const RGBQUAD* GetIdentityPalette(void);

/////////////////////////////////////////////////////////////////////////////
// CDIBInfo (DIB generation)

CDIBInfo::CDIBInfo()
{
   m_pBI = NULL;
   m_pData = NULL;
}

CDIBInfo::~CDIBInfo()
{
   Free();
}

void CDIBInfo::Free()
{
   if(m_pBI)
      {
#ifdef WIN32
         delete [] (LPBYTE)m_pBI;
#else
         _hfree(m_pBI);
#endif
      }
   m_pBI = NULL;
   m_pData = NULL;
}

void CDIBInfo::Allocate(int nHeaderSize, DWORD dwDataSize)
{
   Free();

#ifdef WIN32
   LPBYTE p = new BYTE[nHeaderSize + dwDataSize];
#else
   BYTE huge* p = (BYTE huge*)_halloc(nHeaderSize + dwDataSize, 1);
#endif
   ASSERT(m_pBI == NULL);
   ASSERT(m_pData == NULL);
   m_pBI = (BITMAPINFO*)p;
   m_pData = p + nHeaderSize;
}

LONG
CDIBInfo::GetWidth(void)
{
	LONG	l = 0;
	BITMAPINFO *pb = BitmapInfo();
	if (pb != NULL)
		l = pb->bmiHeader.biWidth;
	return l;
}

LONG
CDIBInfo::GetHeight(void)
{
	LONG	l = 0;
	BITMAPINFO *pb = BitmapInfo();
	if (pb != NULL)
		l = pb->bmiHeader.biHeight;
	return l;
}

#if 0
struct Bytes3
{
   BYTE  b1;
   BYTE  b2;
   BYTE  b3;
};

void
CDIBInfo::PrepareData (DWORD Rows, DWORD RowBytes, BOOL BGRtoRGB /*FALSE*/, BOOL RemoveDWORDPad /*FALSE*/)
{
   if ((BGRtoRGB == FALSE) && (RemoveDWORDPad == FALSE))
      return;
   
   Bytes3   b3;
   LPBYTE   pRead = m_pData, pWrite = m_pData;
   for (DWORD i = 0; i < Rows; i ++)
   {
      for (DWORD j = 0; j < RowBytes; j += 3)
      {
         if (BGRtoRGB == TRUE)
         {
            b3.b1 = pRead [j + 2];
            b3.b2 = pRead [j + 1];
            b3.b3 = pRead [j + 0];
         }
         else
         {
            b3.b1 = pRead [j + 0];
            b3.b2 = pRead [j + 1];
            b3.b3 = pRead [j + 2];
         }

         pWrite [j + 0] = b3.b1;
         pWrite [j + 1] = b3.b2;
         pWrite [j + 2] = b3.b3;
      }

      pRead += Util::RoundToDWORD (RowBytes);
      if (RemoveDWORDPad == TRUE)
         pWrite += RowBytes;  // This will eliminate DIB DWORD row padding
      else
         pWrite += Util::RoundToDWORD (RowBytes);
   } 
}
#endif
