/*
// Thumbnail record.
//
// $Workfile: THUMBREC.CPP $
// $Revision: 1 $
// $Date: 3/03/99 6:12p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/THUMBREC.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 2     9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:26:30   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:48   Fred
// Initial revision.
// 
//    Rev 1.10   05 Nov 1996 14:06:58   Jay
// Got rid of warnings, etc.
// 
//    Rev 1.9   13 Sep 1996 09:54:18   Fred
// Profile Stuff -- will be removed
// 
//    Rev 1.8   21 Aug 1996 16:39:14   Jay
// 
//    Rev 1.7   09 Aug 1996 14:36:44   Jay
// Ugh! Missed some __huge stuff.
// 
//    Rev 1.6   09 Aug 1996 14:28:46   Fred
//  
// 
//    Rev 1.5   09 Aug 1996 14:27:14   Jay
// Changed new __huge stuff to _halloc, etc.
// 
//    Rev 1.4   08 Aug 1996 18:10:26   Jay
// JPEG, etc!
// 
//    Rev 1.3   07 Aug 1996 17:50:10   Jay
//  
// 
//    Rev 1.2   31 Jul 1996 11:57:16   Fred
// 16-bit compatibility
// 
//    Rev 1.1   10 May 1996 16:03:40   Jay
// New compression method
// 
//    Rev 1.0   14 Mar 1996 13:35:06   Jay
// Initial revision.
// 
//    Rev 1.1   06 Mar 1996 10:31:44   Jay
// Changes from 2.1
// 
//    Rev 2.7   27 Feb 1996 16:49:06   JAY
// Fixed sign extension problem.
// 
//    Rev 2.6   19 Jun 1995 17:19:16   JAY
// Item extensions
// 
//    Rev 2.5   14 Jun 1995 07:49:58   JAY
// Passes full RGB colors while drawing (PAL_COLORS didn't work).
// 
//    Rev 2.4   09 Jun 1995 10:49:28   JAY
// Compression of thumbnail.
// 
//    Rev 2.3   06 Jun 1995 16:55:12   JAY
// Project previews in the album.
// 
//    Rev 2.2   06 Jun 1995 10:12:24   JAY
// Moved more stuff over from CATALOG.
// 
//    Rev 1.4   24 Feb 1995 13:37:10   JAY
// Fixed a logic error.
// 
//    Rev 1.3   24 Feb 1995 11:12:12   JAY
//  
// 
//    Rev 1.2   23 Dec 1994 07:47:38   JAY
// Sets the stretch blt mode before blting.
// 
//    Rev 1.1   15 Nov 1994 08:37:30   JAY
// Made static variables near to prevent multiple data segments.
// 
//    Rev 1.0   03 Nov 1994 15:53:52   JAY
// Initial revision.
// 
//    Rev 1.0   03 Nov 1994 12:56:56   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "pmw.h"
#include "thumbrec.h"
#include "cimplode.h"
#include "cdeflate.h"
#include "cdrawcon.h"
#include "dither.h"
#include "jpegimpl.h"

#ifndef WIN32
#include <malloc.h>
#endif

extern const RGBQUAD* GetIdentityPalette(void);

/*****************************************************************************/
/*                             ThumbNail record                              */
/*****************************************************************************/

/*
// The creator for a thumbnail record.
*/

CStandardDataRecord* CThumbNailData::Create()
{
	return new CThumbNailData;
}

/*
// The initialization routine for the thumbnail record.
*/

BOOL CThumbNailData::Init(DATAID id, DATATYPE type, CStandardDataManager* pManager, LPVOID pData, ST_DEV_POSITION* in_where)
{
	if (CStandardDataRecord::Init(id, type, pManager, NULL, in_where))
	{
		if (pData != NULL)
		{
			if (!SetBitmap((CThumbNailCreationData*)pData))
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

/*
// The constructor for a thumbnail record.
*/

CThumbNailData::CThumbNailData()
{
	memset(&m_Record, 0, sizeof(m_Record));

	m_Record.m_nCompression = COMPRESSION_Unknown;
	m_pBitmap = NULL;
	m_dwTryCompression = TRY_COMPRESSION_ALL;
}

/*
// The destructor.
*/

CThumbNailData::~CThumbNailData()
{
	FreeBitmap();
}

/*
// Create ourselves from a creation structure.
*/

BOOL CThumbNailData::SetBitmap(CThumbNailCreationData* pData)
{
	ASSERT(pData != NULL);

	m_Record.m_wWidth = (WORD)pData->m_nWidth;
	m_Record.m_wHeight = (WORD)pData->m_nHeight;
	m_Record.m_wBPP = (WORD)pData->m_nBPP;
	// Force it to recompute next time it writes.
	m_Record.m_nCompression = COMPRESSION_Unknown;
	m_dwTryCompression = pData->m_dwTryCompression;

	DWORD dwSize = UncompressedSize();
//	ASSERT(dwSize < 0x10000L);

/* Make sure we're clean. */

	FreeBitmap();

	if (dwSize != 0)
	{
		if (AllocateBitmap(dwSize) != ERRORCODE_None)
		{
			return FALSE;
		}
#ifdef WIN32
		if (pData->m_pBitmap != NULL)
		{
			memcpy(m_pBitmap, pData->m_pBitmap, dwSize);
		}
#else
		hmemcpy(m_pBitmap, pData->m_pBitmap, dwSize);
#endif
	}
	return TRUE;
}

DWORD CThumbNailData::BitmapSize(void)
{
	DWORD dwSize = UncompressedSize();

	switch (m_Record.m_nCompression)
	{
		case COMPRESSION_Unknown:
		{
		/* Figure it out. */
			m_Record.m_nCompression = COMPRESSION_None;

			if (m_pBitmap != NULL)
			{
#if 0
			/* Try PackBits. */
				DWORD dwPackBitsSize = PackBitsSize();

				if (dwPackBitsSize < dwSize)
				{
					record.nCompression = COMPRESSION_PackBits;
					dwSize = dwPackBitsSize;
				}
#endif

				if (m_dwTryCompression & TRY_COMPRESSION_Deflate)
				{
					// Try Deflate.
					DWORD dwDeflateSize = DeflateSize();

					if (dwDeflateSize < dwSize)
					{
						m_Record.m_nCompression = COMPRESSION_Deflate;
						dwSize = dwDeflateSize;
					}
				}

				if (m_dwTryCompression & TRY_COMPRESSION_JPEG)
				{
					// Try JPEG.
					DWORD dwJPEGSize = JPEGSize();

					if (dwJPEGSize < dwSize)
					{
						m_Record.m_nCompression = COMPRESSION_JPEG;
						dwSize = dwJPEGSize;
					}
				}
			}
			m_Record.m_dwCompressedSize = dwSize;
			break;
		}
		default:
		{
			if (m_Record.m_dwCompressedSize != 0)
			{
				dwSize = m_Record.m_dwCompressedSize;
			}
			break;
		}
	}

	return dwSize;
}

DWORD CThumbNailData::UncompressedSize(void)
{
	int nByteWidth;

	switch (m_Record.m_wBPP)
	{
		case 1:
		{
			nByteWidth = (m_Record.m_wWidth+7)/8;
			break;
		}
		case 8:
		{
			nByteWidth = m_Record.m_wWidth;
			break;
		}
		case 24:
		{
			nByteWidth = m_Record.m_wWidth*3;
			break;
		}
		default:
		{
			ASSERT(FALSE);
			return 0;
		}
	}
	return (DWORD)((nByteWidth+3) & ~3) * (DWORD)m_Record.m_wHeight;
}

DWORD CThumbNailData::DeflateSize(void)
{
	CDeflate Deflator;
	LPBYTE pData = NULL;
	DWORD dwDestWritten = 0;

	Deflator.Compress(m_pBitmap, UncompressedSize(), &pData, &dwDestWritten);
	CDeflate::FreeData(pData);

	return dwDestWritten;
}

DWORD CThumbNailData::JPEGSize(void)
{
	DWORD dwDestWritten = (DWORD)-1;			// Mucho big value.
	if (m_Record.m_wBPP == 24)
	{
		CJPEGCompressor Compressor;

		if (Compressor.Init(NULL, m_Record.m_wWidth, m_Record.m_wHeight, 3) == ERRORCODE_None
			 && Compressor.WriteLines(m_pBitmap, m_Record.m_wHeight) == ERRORCODE_None
			 && Compressor.Finish() == ERRORCODE_None)
		{
			dwDestWritten = Compressor.BytesWritten();
		}
	}
	return dwDestWritten;
}

/*
// The dib header we use for drawing.
*/

CThumbNailData::CDibHeader /*far*/ CThumbNailData::Header =
{
	{
		sizeof(BITMAPINFOHEADER),			// biSize
		0,											// biWidth (must be set)
		0,											// biHeight (must be set)
		1,											// biPlanes
		0,											// biBitCount (must be set)
		BI_RGB,									// biCompression == uncompressed
		0,											// biSizeImage (must be set)
		0,											// biXPelsPerMeter
		0,											// biYPelsPerMeter
		0,											// biClrUsed
		0											// biClrImportant
	}
};

/*
// Draw()
//
// Draw this thumbnail into a DC.
*/

BOOL CThumbNailData::Draw(CDC* pDC, CDrawContext* pContext, const CRect& crBounds)
{
	GET_PMWAPP()->ProfileAdd("Start THUMBNAIL LoadBitmap");
	LoadBitmap();
	GET_PMWAPP()->ProfileAdd("End THUMBNAIL LoadBitmap");

	if (m_pBitmap == NULL)
	{
	/* This is only OK if the bitmap size is 0. */
		return UncompressedSize() == 0;
	}

	// Extract the size.
	int nX = crBounds.left;
	int nY = crBounds.top;
	int nDX = crBounds.Width();
	int nDY = crBounds.Height();

	// Default to the bitmap in the record.
	LPBYTE pBitmap = m_pBitmap;
	Header.bmiHeader.biWidth = m_Record.m_wWidth;
	Header.bmiHeader.biHeight = m_Record.m_wHeight;
	Header.bmiHeader.biBitCount = m_Record.m_wBPP;
	Header.bmiHeader.biSizeImage = UncompressedSize();

	// See if we want to convert to something closer to the screen.
	if (m_Record.m_wBPP == 24 && pContext->Info().m_type == CRASTER_TYPE_PALETTE)
	{
		// Convert the bitmap to an 8-bit bitmap in the PrintMaster palette.
		DWORD dwNewSize = (DWORD)((nDX + 3) & ~3) * (DWORD)nDY;
		pBitmap = NULL;
		GET_PMWAPP()->ProfileAdd("Start THUMBNAIL _halloc");
		TRY
		{
#ifdef WIN32
			pBitmap = new BYTE[dwNewSize];
#else
//			pBitmap = new __huge BYTE[dwNewSize];		// This does not work!
			pBitmap = (LPBYTE)_halloc(dwNewSize, 1);
#endif
		}
		END_TRY
		GET_PMWAPP()->ProfileAdd("End THUMBNAIL _halloc");
		if (pBitmap == NULL)
		{
			// Oh, well. Continue on.
			return TRUE;
		}

		// Do the dither.
		GET_PMWAPP()->ProfileAdd("Start THUMBNAIL Dither");
		static BOOL fDither = TRUE;
		if (fDither)
		{
			Dither24To8(m_pBitmap, m_Record.m_wWidth, m_Record.m_wHeight, pBitmap, nDX, nDY);
		}
		else
		{
			ErrDiff24To8(m_pBitmap, m_Record.m_wWidth, m_Record.m_wHeight, pBitmap, nDX, nDY);
		}
		GET_PMWAPP()->ProfileAdd("End THUMBNAIL Dither");

		// Update the parameters to reference our new bitmap.
		Header.bmiHeader.biWidth = nDX;
		Header.bmiHeader.biHeight = nDY;
		Header.bmiHeader.biBitCount = 8;
		Header.bmiHeader.biSizeImage = dwNewSize;
	}

/*
// If this is the first time drawing, we need to fill out the static
// DIB structures.
*/
	switch (Header.bmiHeader.biBitCount)
	{
		case 1:
		{
			Header.bmiColors[0].rgbBlue =
			Header.bmiColors[0].rgbGreen =
			Header.bmiColors[0].rgbRed = 0;
			Header.bmiColors[0].rgbReserved = 0;

			Header.bmiColors[1].rgbBlue =
			Header.bmiColors[1].rgbGreen =
			Header.bmiColors[1].rgbRed = 255;
			Header.bmiColors[1].rgbReserved = 0;

			// Monochrome bitmaps are stored upside-down.
			nY += nDY-1;
			nDY = -nDY;
			break;
		}
		case 8:
		{
			memcpy(Header.bmiColors, GetIdentityPalette(), 256*sizeof(RGBQUAD));
			break;
		}
		default:
		{
			break;
		}
	}

	int nOldStretchMode = pDC->SetStretchBltMode(STRETCH_DELETESCANS);

	GET_PMWAPP()->ProfileAdd("Start THUMBNAIL StretchDIBits");
	::StretchDIBits(pDC->GetSafeHdc(),
						 nX,
						 nY,
						 nDX,
						 nDY,
						 0,
						 0,
						 (int)Header.bmiHeader.biWidth,
						 (int)Header.bmiHeader.biHeight,
						 pBitmap,
						 (BITMAPINFO*)&Header,
						 DIB_RGB_COLORS,
						 SRCCOPY);
	GET_PMWAPP()->ProfileAdd("End THUMBNAIL StretchDIBits");

	pDC->SetStretchBltMode(nOldStretchMode);

	if (pBitmap != m_pBitmap && pBitmap != NULL)
	{
#ifdef WIN32
		delete [] pBitmap;
#else
//		delete [] (char __huge*)pBitmap;
		_hfree(pBitmap);
#endif
	}

	// A test!
//	FreeBitmap();
	return TRUE;
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE CThumbNailData::ReadData(StorageDevicePtr pDevice)
{
	ERRORCODE error;

	if ((error = CStandardDataRecord::ReadData(pDevice)) == ERRORCODE_None
		 && (error = pDevice->read_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
		pDevice->tell(&m_lBitmapPosition);

		// Force it to recompute next time it writes.
//		m_Record.m_nCompression = COMPRESSION_Unknown;
	}
	return error;
}

/*
// WriteData()
//
// Write the record.
// Only the record is written. The actual data should already have been
// written.
*/

ERRORCODE CThumbNailData::WriteData(StorageDevicePtr pDevice)
{
	ERRORCODE error;

	if ((error = CStandardDataRecord::WriteData(pDevice)) == ERRORCODE_None
		 && (error = pDevice->write_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
		DWORD dwSize = BitmapSize();

		switch (m_Record.m_nCompression)
		{
			case COMPRESSION_Unknown:
			{
				ASSERT(FALSE);
				return ERRORCODE_IntError;
			}
			case COMPRESSION_None:
			{
				error = pDevice->huge_write(m_pBitmap, dwSize);
				break;
			}
#if 0
			case COMPRESSION_PackBits:
			{
				DWORD dwUncompressedSize = UncompressedSize();
				ASSERT(dwUncompressedSize > dwSize);	// Else, why bother?

				LPBYTE pData = NULL;
				TRY
					pData = new BYTE[dwSize];
				END_TRY
				if (pData == NULL)
				{
					error = ERRORCODE_Memory;
				}
				else
				{
					pBytePtr = pData;

					PackBits(WritingWrite, (LPBYTE)bitmap, (int)dwUncompressedSize);

					error = device->write(pData, (ST_DEV_IO_SIZE)dwSize);

					delete [] pData;
				}

				break;
			}
#endif
			case COMPRESSION_Implode:
			{
				DWORD dwUncompressedSize = UncompressedSize();
				ASSERT(dwUncompressedSize > dwSize);	// Else, why bother?

				LPBYTE pData;
				if ((error = AllocateMemory(&pData, dwSize)) == ERRORCODE_None)
				{
					CImplode imp;

					imp.Compress((LPBYTE)m_pBitmap, dwUncompressedSize, pData);

					error = pDevice->huge_write(pData, dwSize);

					FreeMemory(pData);
				}
				break;
			}
			case COMPRESSION_Deflate:
			{
				DWORD dwUncompressedSize = UncompressedSize();
				ASSERT(dwUncompressedSize > dwSize);	// Else, why bother?

				LPBYTE pData = NULL;
				DWORD dwCompressedSize = 0;

				CDeflate def;

				def.Compress((LPBYTE)m_pBitmap, dwUncompressedSize, &pData, &dwCompressedSize);

				if (pData != NULL)
				{
					ASSERT(dwSize == dwCompressedSize);
					error = pDevice->huge_write(pData, dwSize);

					CDeflate::FreeData(pData);
				}
				break;
			}
			case COMPRESSION_JPEG:
			{
				CJPEGCompressor Compressor;

				ASSERT(m_Record.m_wBPP == 24);

				if ((error = Compressor.Init(pDevice, m_Record.m_wWidth, m_Record.m_wHeight, 3)) == ERRORCODE_None
					 && (error = Compressor.WriteLines(m_pBitmap, m_Record.m_wHeight)) == ERRORCODE_None
					 && (error = Compressor.Finish()) == ERRORCODE_None)
				{
					// Success!
				}

				break;
			}
		}
	}
	return error;
}

/*
// SizeofData()
//
// Return the size of the record.
// This is the complete size of the written part and the file image.
*/

ST_MAN_SIZE CThumbNailData::SizeofData(StorageDevicePtr pDevice)
{
	return CStandardDataRecord::SizeofData(pDevice)
				+ pDevice->size_record(sizeof(m_Record))
				+ BitmapSize();
}

ERRORCODE CThumbNailData::LoadBitmap(void)
{
	ERRORCODE error = ERRORCODE_None;
	DWORD dwUncompressedSize = UncompressedSize();

	if (m_pBitmap == NULL && dwUncompressedSize != 0)
	{
		DWORD dwSize = BitmapSize();
		StorageDevice* pDevice = manager->m_pDevice;
		if ((error = pDevice->seek(m_lBitmapPosition, ST_DEV_SEEK_SET)) == ERRORCODE_None)
		{
			if (m_Record.m_nCompression == COMPRESSION_JPEG)
			{
				// We want to allocate the whole thing once.
				// Source is the file.
				error = AllocateBitmap(dwUncompressedSize);
			}
			else
			{
				if ((error = AllocateBitmap(dwSize)) == ERRORCODE_None)
				{
					error = pDevice->huge_read(m_pBitmap, dwSize);
				}
			}

			if (error == ERRORCODE_None)
			{
				switch (m_Record.m_nCompression)
				{
#if 0
					case COMPRESSION_PackBits:
					{
						LPBYTE pData;
						DWORD dwUncompressedSize = UncompressedSize();
						if ((error = AllocateMemory(&pData, dwUncompressedSize)) == ERRORCODE_None)
						{
							pBytePtr = (LPBYTE)m_pBitmap;

							unPackBits(ReadingRead, pData, nUncompressedSize);

							FreeBitmap();
							m_pBitmap = pData;
						}
						break;
					}
#endif
					case COMPRESSION_Implode:
					{
						LPBYTE pData;
						DWORD dwUncompressedSize = UncompressedSize();
						if ((error = AllocateMemory(&pData, dwUncompressedSize)) == ERRORCODE_None)
						{
							CImplode imp;

							imp.Decompress((LPBYTE)m_pBitmap, m_Record.m_dwCompressedSize, pData);

							FreeBitmap();

							m_pBitmap = pData;
						}
						break;
					}
					case COMPRESSION_Deflate:
					{
						LPBYTE pData = NULL;
						DWORD dwDataSize = 0;

						DWORD dwUncompressedSize = UncompressedSize();
						if ((error = AllocateMemory(&pData, dwUncompressedSize)) == ERRORCODE_None)
						{
							CDeflate Deflator;
							LPBYTE pOutData;

							Deflator.Decompress((LPBYTE)m_pBitmap, m_Record.m_dwCompressedSize, &pOutData, &dwDataSize, pData, dwUncompressedSize);
							if (pOutData != NULL)
							{
								FreeBitmap();
#if 1
								ASSERT(pOutData == pData);
								ASSERT(dwDataSize == dwUncompressedSize);
								m_pBitmap = pData;
#else
								if ((error = AllocateBitmap(dwDataSize)) == ERRORCODE_None)
								{
#ifdef WIN32
									memcpy(m_pBitmap, pData, dwDataSize);
#else
									hmemcpy(m_pBitmap, pData, dwDataSize);
#endif
								}
								CDeflate::FreeData(pData);
#endif
							}
							break;
						}
						break;
					}
					case COMPRESSION_JPEG:
					{
						CJPEGDecompressor Decompressor;

						ASSERT(m_Record.m_wBPP == 24);

						if ((error = Decompressor.Init(pDevice)) == ERRORCODE_None)
						{
							if (Decompressor.ImageWidth() == (int)m_Record.m_wWidth
								 && Decompressor.ImageHeight() == (int)m_Record.m_wHeight
								 && Decompressor.ColorComponents() == 3)
							{
								if ((error = Decompressor.ReadLines(0, m_pBitmap, m_Record.m_wHeight)) == ERRORCODE_None
									 && (error = Decompressor.Finish()) == ERRORCODE_None)
								{
									// Success!
								}
							}
							else
							{
								// Dimensions do not match!
								ASSERT(FALSE);
							}
						}
						break;
					}
				}
			}
		}
	}
	if (error != ERRORCODE_None)
	{
		ASSERT(FALSE);
		FreeBitmap();
	}
	return error;
}

/*
// Assign method.
*/

void CThumbNailData::Assign(const CDataRecord& Record)
{
/* Assign the base record first. */
	CStandardDataRecord::Assign(Record);
	const CThumbNailData& s = (const CThumbNailData&)Record;

	m_Record = s.m_Record;
	if (s.m_pBitmap != NULL)
	{
		ASSERT(m_pBitmap == NULL);		// Lest we lose memory...

		DWORD dwUncompressedSize = UncompressedSize();
		if (AllocateBitmap(dwUncompressedSize) == ERRORCODE_None)
		{
#ifdef WIN32
			memcpy(m_pBitmap, s.m_pBitmap, dwUncompressedSize);
#else
			hmemcpy(m_pBitmap, s.m_pBitmap, dwUncompressedSize);
#endif
		}
	}
}

void CThumbNailData::FreeBitmap(void)
{
	FreeMemory(m_pBitmap);
	m_pBitmap = NULL;
}

void CThumbNailData::FreeMemory(BYTE* pMemory)
{
	// Free a huge block of memory allocated with AllocateMemory().
#ifdef WIN32
		delete [] (BYTE*)pMemory;
#else
		if (pMemory != NULL)
		{
//			delete [] (BYTE __huge *)pMemory;
			_hfree(pMemory);
		}
#endif
	pMemory = NULL;
}

ERRORCODE CThumbNailData::AllocateBitmap(DWORD dwSize)
{
	return AllocateMemory(&m_pBitmap, dwSize);
}

// Allocate a huge block of memory.
ERRORCODE CThumbNailData::AllocateMemory(BYTE** ppMemory, DWORD dwSize)
{
	ERRORCODE error = ERRORCODE_None;

	// Allocate a huge block of memory.
	*ppMemory = NULL;
	TRY
	{
#ifdef WIN32
		*ppMemory = new BYTE[dwSize];
#else
//		*ppMemory = new __huge BYTE[dwSize];
		*ppMemory = (LPBYTE)_halloc(dwSize, 1);
#endif
	}
	END_TRY
	if (*ppMemory == NULL)
	{
		ASSERT(FALSE);
		error = ERRORCODE_Memory;
	}

	ASSERT(error == ERRORCODE_None);
	return error;
}

