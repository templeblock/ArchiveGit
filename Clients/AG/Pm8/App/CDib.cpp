/*
// $Workfile: CDib.cpp $
// $Revision: 1 $
// $Date: 3/03/99 6:04p $
//
//  "This unpublished source code contains trade secrets which are the
//   property of Mindscape, Inc.  Unauthorized use, copying or distribution
//   is a violation of international laws and is strictly prohibited."
// 
//        Copyright © 1999 Mindscape, Inc. All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/CDib.cpp $
// 
// 1     3/03/99 6:04p Gbeddow
// 
// 3     2/08/99 5:52p Guidom
// Changed CDib to CPDib
// 
// 2     2/08/99 2:47p Guidom
// Added color reduction and fixed up scale.
*/


#include "StdAfx.h"
#include "stdlib.h"

#include "CDib.h"


IMPLEMENT_DYNAMIC(CPDib, CObject)

CPDib::CPDib()
{
	m_ptrToDIBInfo = NULL;
	m_ptrToDIBHeader = NULL;
	m_ptrToData = NULL;
	m_nHeaderSize = 0;
	m_nDataSize = 0;
}

CPDib::~CPDib()
{
	DeleteData();
}

CPDib& CPDib::operator =(const CPDib &srcDib)
{
	if(this == &srcDib)
	{
		return(*this); // source and destination are the same.
							// no need to do any of this - just return...
	}							

	DeleteData();

	m_nHeaderSize = srcDib.m_nHeaderSize;
	m_nDataSize = srcDib.m_nDataSize;

	AllocateDIBandHeaderData(m_nHeaderSize, m_nDataSize);

	if(m_ptrToDIBInfo != NULL)
	{
		memcpy(m_ptrToDIBInfo, srcDib.m_ptrToDIBInfo, m_nHeaderSize);
	}

	if(m_ptrToData != NULL)
	{
		memcpy(m_ptrToData, srcDib.m_ptrToData, m_nDataSize);
	}

	return(*this);
}

CPDib::DeleteData()
{
	if(m_ptrToDIBInfo)
	{
		delete [] m_ptrToDIBInfo;
		m_ptrToDIBHeader = NULL;
		m_ptrToData = NULL;
		m_ptrToDIBInfo = NULL;
		m_nDataSize = 0;
		m_nHeaderSize = 0;
	}
}

int CPDib::GetDIBHeaderSize() const
{
	return(m_nHeaderSize);
}

int CPDib::GetDIBDataSize() const
{
	return(m_nDataSize);
}

int CPDib::GetWidth() const 
{
	int ret_val = 0;

	if(m_ptrToDIBInfo)
	{
		ret_val = m_ptrToDIBInfo->bmiHeader.biWidth;
	}

	return(ret_val);
}

int CPDib::GetHeight() const
{
	int ret_val = 0;

	if(m_ptrToDIBInfo)
	{
		ret_val = m_ptrToDIBInfo->bmiHeader.biHeight;
	}

	return(ret_val);
}

int CPDib::GetBitCount() const
{
	int ret_val = 0;

	if(m_ptrToDIBInfo)
	{
		ret_val = m_ptrToDIBInfo->bmiHeader.biBitCount;
	}

	return(ret_val);
}

BYTE *CPDib::GetColorMap() const
{
	return((BYTE *)m_ptrToDIBInfo->bmiColors);
}

BYTE *CPDib::GetDIBData() const
{
	return(m_ptrToData);
}

void CPDib::GetBitMapInfoHeader(BITMAPINFOHEADER &ptrBitMapHeader) const 
{
	if(m_ptrToDIBHeader)
	{
		memcpy(&ptrBitMapHeader, m_ptrToDIBHeader, m_ptrToDIBHeader->biSize);
	}else
	{
		memset(&ptrBitMapHeader, 0, sizeof(BITMAPINFOHEADER));
	}
}

void CPDib::GetBitMapInfo(BITMAPINFO &ptrBitMapInfo) const
{
	if(m_ptrToDIBInfo)
	{
		memcpy(&ptrBitMapInfo, m_ptrToDIBInfo, sizeof(BITMAPINFO));
	}else
	{
		memset(&ptrBitMapInfo, 0, sizeof(BITMAPINFO));
	}
}


//**************************************************************
// This function allocates the header and DIB data in one contiguous
// piece of memory.  The individual size of each piece needs to be
// passed in so that the appropriate pointers can be set correctly.
//**************************************************************

BOOL CPDib::AllocateDIBandHeaderData(int header_size, int data_size)
{
	BYTE *data_ptr = NULL;

	DeleteData();

	data_ptr = new BYTE[header_size + data_size];

	if(data_ptr == NULL)
	{
		ASSERT(0); // memory allocation failed
		return(FALSE);
	}

	m_ptrToDIBInfo = (BITMAPINFO *)data_ptr;
	m_ptrToDIBHeader = (BITMAPINFOHEADER *)data_ptr;
	m_ptrToData = data_ptr + header_size;
	m_nHeaderSize = header_size;
	m_nDataSize = data_size;

	return(TRUE);
}

BYTE *CPDib::ReleaseDIBData() const
{
	return((BYTE *)m_ptrToData);
}

BYTE *CPDib::ReleaseHeaderData() const 
{
	return((BYTE *)m_ptrToDIBInfo);
}

BYTE *CPDib::ReleaseDIBandHeaderData() const
{
	return((BYTE *)m_ptrToDIBInfo);
}

void CPDib::SetDIBHeader(BITMAPINFOHEADER *data_ptr, int data_len)
{
	int data_copy_len = __min (data_len, m_nHeaderSize);

	if(m_ptrToDIBHeader)
	{
		memcpy(m_ptrToDIBHeader, data_ptr, data_copy_len);
	}
}

void CPDib::SetDIBData(BYTE *data_ptr, int data_len)
{
	int data_copy_len = __min (data_len, m_nDataSize);

	if(m_ptrToData)
	{
		memcpy(m_ptrToData, data_ptr, data_copy_len);
	}
}

WORD CPDib::GetPaletteSize() const
{
	return (GetNumColors() * sizeof(RGBQUAD));
}

WORD CPDib::GetNumColors() const
{
   WORD wBitCount;

      // If this is a Windows style DIB, the number of colors in the
      //  color table can be less than the number of bits per pixel
      //  allows for (i.e. lpbi->biClrUsed can be set to some value).
      //  If this is the case, return the appropriate value.

	DWORD dwClrUsed;

	if(m_ptrToDIBHeader == NULL)
	{
		ASSERT(0);
		return(0);
	}

	dwClrUsed = m_ptrToDIBHeader->biClrUsed;

	if (dwClrUsed)
	{
		return (WORD)dwClrUsed;
	}

   // Calculate the number of colors in the color table based on
   //  the number of bits per pixel for the DIB.

   wBitCount = m_ptrToDIBHeader->biBitCount;

   switch (wBitCount)
   {
      case 1:
         return 2;

      case 4:
         return 16;

      case 8:
         return 256;

      default:
         return 0;
   }
}

DWORD CPDib::GetImageSize() const
{
	DWORD		dwSizeImage;

	if(m_ptrToDIBHeader == NULL)
	{
		ASSERT(0);
		return(0);
	}

	if(m_ptrToDIBHeader->biSizeImage == 0)
	{
		dwSizeImage = (((m_ptrToDIBHeader->biWidth * m_ptrToDIBHeader->biBitCount + 31) & ~31)
					>> 3) * abs(m_ptrToDIBHeader->biHeight);
	}
	else
	{
		dwSizeImage = m_ptrToDIBHeader->biSizeImage;
	}

	return(dwSizeImage);
}

