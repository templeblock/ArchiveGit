/*
// $Header: /PM8/App/PREVREC.CPP 1     3/03/99 6:11p Gbeddow $
//
// Project preview record routines.
//
// $Log: /PM8/App/PREVREC.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 2     9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:24:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:04   Fred
// Initial revision.
// 
//    Rev 1.3   08 Nov 1996 08:57:44   Jay
// Removed 16-bit compress.obj
// 
//    Rev 1.2   10 May 1996 16:00:10   Jay
// New compression method.
// 
//    Rev 1.1   10 May 1996 14:05:04   Fred
// CDeflate
// 
//    Rev 1.0   14 Mar 1996 13:43:24   Jay
// Initial revision.
// 
//    Rev 1.1   06 Mar 1996 09:54:42   Jay
// Changes in 2.1
// 
//    Rev 2.9   27 Feb 1996 16:42:46   JAY
// Fixed a sign-extension problem.
// 
//    Rev 2.8   21 Sep 1995 13:46:48   JAY
// 
//    Rev 2.7   13 Sep 1995 11:33:26   JAY
// New 32-bit stuff
// 
//    Rev 2.6   10 Jul 1995 16:25:16   JAY
// Added "AlbumPath" to preview record.
// 
//    Rev 2.5   09 Jun 1995 10:47:38   JAY
// Improvements to preview code.
// 
//    Rev 2.4   07 Jun 1995 13:15:30   JAY
// Added function to create HBITMAP from a preview record.
// 
//    Rev 2.3   06 Jun 1995 10:09:34   JAY
// Now uses CImplode.
// 
//    Rev 2.2   05 Jun 1995 16:54:54   JAY
// Now supports Implode (PKWARE) library for better compression.
// 
//    Rev 2.1   05 Jun 1995 11:30:28   JAY
// New color previews.
// 
//    Rev 2.0   07 Feb 1995 15:48:04   JAY
// Initial revision.
// 
//    Rev 1.3   30 Nov 1994 16:34:04   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.2   01 Jun 1994 16:35:16   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.1   05 Jan 1994 08:05:40   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:24:56   JAY
// 
*/

#include "stdafx.h"
#include "cimplode.h"
#include "cdeflate.h"
#include "prevrec.h"
#include "compress.h"

#include <string.h>

extern const RGBQUAD* GetIdentityPalette(void);

static int nBytesWritten;
static LPBYTE pBytePtr;

static BOOL SizingWrite(UCHAR*, int nData)
{
	nBytesWritten += nData;
	return TRUE;
}

static BOOL WritingWrite(UCHAR* pData, int nData)
{
	memcpy(pBytePtr, pData, nData);
	pBytePtr += nData;
	return TRUE;
}

static BOOL ReadingRead(LPBYTE pByte)
{
	*pByte = *pBytePtr++;
	return TRUE;
}

/*****************************************************************************/
/*                          Project preview record                           */
/*****************************************************************************/

/*
// The creator for a project preview record.
*/

ERRORCODE ProjectPreviewRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
/* Create the new array record. */

	if ((*record = (DatabaseRecordPtr)(new ProjectPreviewRecord(number, type, owner, where))) == NULL)
	{
		return ERRORCODE_Memory;
	}
	return ERRORCODE_None;
}

/*
// The constructor for a project preview record.
*/

ProjectPreviewRecord::ProjectPreviewRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where)
{
	memset(&record, 0, sizeof(record));
//	record.valid = FALSE;
	bitmap_position = POSITION_UNKNOWN;
	bitmap = NULL;
}

/*
// The destructor for the project preview record.
*/

ProjectPreviewRecord::~ProjectPreviewRecord()
{
	FreeBitmap();
}

void ProjectPreviewRecord::FreeBitmap(void)
{
	delete [] (LPBYTE)bitmap;
	bitmap = NULL;
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE ProjectPreviewRecord::ReadData(StorageDevicePtr device)
{
	ERRORCODE error;

	bitmap = NULL;

	if ((error = device->read_record(&record, sizeof(record))) == ERRORCODE_None
		 && (error = device->read_cstring(m_csDescription)) == ERRORCODE_None)
	{
		if (!(record.m_wFlags & FLAG_HasAlbumPath)
			 || (error = device->read_cstring(m_csAlbumPath)) == ERRORCODE_None)
		{
			if (record.valid)
			{
				error = device->tell(&bitmap_position);
			}
		}
	}
	return error;
}

/*
// WriteData()
//
// Write the record.
*/

ERRORCODE ProjectPreviewRecord::WriteData(StorageDevicePtr device)
{
	ERRORCODE error;

	record.m_wFlags |= FLAG_HasAlbumPath;

	if ((error = device->write_record(&record, sizeof(record))) == ERRORCODE_None
		 && (error = device->write_cstring(m_csDescription)) == ERRORCODE_None
		 && (error = device->write_cstring(m_csAlbumPath)) == ERRORCODE_None)
	{
		if (record.valid)
		{
			if (bitmap != NULL)
			{
				DWORD dwSize = preview_size();

				switch (record.nCompression)
				{
					case COMPRESSION_Unknown:
					{
						ASSERT(FALSE);
						return ERRORCODE_IntError;
					}
					case COMPRESSION_None:
					{
						error = device->write(bitmap, (ST_DEV_IO_SIZE)dwSize);
						break;
					}
					case COMPRESSION_Deflate:
					{
						DWORD dwUncompressedSize = UncompressedSize();
						ASSERT(dwUncompressedSize > dwSize);	// Else, why bother?

						LPBYTE pData = NULL;
						DWORD dwCompressedSize = 0;

						CDeflate def;

						def.Compress((LPBYTE)bitmap, dwUncompressedSize, &pData, &dwCompressedSize);

						if (pData != NULL)
						{
							ASSERT(dwSize == dwCompressedSize);
							error = device->write(pData, (ST_DEV_IO_SIZE)dwSize);

							CDeflate::FreeData(pData);
						}
						break;
					}
				}
			}
		}
	}
	return error;
}

/*
// SizeofData()
//
// Return the size of the record.
*/

ST_MAN_SIZE ProjectPreviewRecord::SizeofData(StorageDevicePtr device)
{
	ST_MAN_SIZE size = device->size_record(sizeof(record))
							+ device->size_cstring(m_csDescription)
							+ device->size_cstring(m_csAlbumPath);

	if (record.valid)
	{
		size += preview_size();
	}

	return size;
}

DWORD ProjectPreviewRecord::DeflateSize(void)
{
	LPBYTE pData = NULL;
	DWORD dwSize = 0;

	CDeflate def;

	def.Compress((LPBYTE)bitmap, UncompressedSize(), &pData, &dwSize);

	CDeflate::FreeData(pData);

	return dwSize;
}

DWORD ProjectPreviewRecord::UncompressedSize(void)
{
	if (record.valid)
	{
		int nByteWidth;

		switch (record.nType)
		{
			case PREVIEW_TYPE_Monochrome:
			{
				nByteWidth = ((record.width+7)/8 + 1) & ~1;
				break;
			}
			case PREVIEW_TYPE_ColorDIB:
			{
				nByteWidth = (record.width + 3) & ~3;
				break;
			}
			default:
			{
				ASSERT(FALSE);
				nByteWidth = 0;
				break;
			}
		}
		return (DWORD)nByteWidth*(DWORD)record.height;
	}
	return 0L;
}

/*
// Return the size of the bitmap.
*/

LONG ProjectPreviewRecord::preview_size(VOID)
{
	DWORD dwSize = UncompressedSize();

	switch (record.nCompression)
	{
		case COMPRESSION_Unknown:
		{
		/* Figure it out. */
			record.nCompression = COMPRESSION_None;

			if (bitmap != NULL)
			{
			/* Try Deflate. */
				DWORD dwDeflateSize = DeflateSize();

				if (dwDeflateSize < dwSize && dwDeflateSize != 0)
				{
					record.nCompression = COMPRESSION_Deflate;
					dwSize = dwDeflateSize;
				}

				TRACE("Size: %lu bytes, Deflate: %lu bytes\r\n",
									dwSize, dwDeflateSize);

			}
			record.dwCompressedSize = dwSize;
			break;
		}
		default:
		{
			if (record.dwCompressedSize != 0)
			{
				dwSize = record.dwCompressedSize;
			}
			break;
		}
	}

	return dwSize;
}

/*
// Read the preview.
*/

ERRORCODE ProjectPreviewRecord::read_preview(VOIDPTR far *bm)
{
	ERRORCODE error = ERRORCODE_NotInitialized;

	if (record.valid)
	{
		if (bitmap == NULL)
		{
			if (bitmap_position != POSITION_UNKNOWN)
			{
			/* Read the bitmap. */

				if ((error = manager->m_pDevice->seek(bitmap_position, ST_DEV_SEEK_SET)) == ERRORCODE_None)
				{
					int nSize = (int)preview_size();

					if ((error = manager->m_pDevice->read_block((ST_DEV_IO_SIZE)nSize, &bitmap)) == ERRORCODE_None)
					{
						switch (record.nCompression)
						{
							case COMPRESSION_PackBits:
							{
								LPBYTE pData = NULL;
								int nUncompressedSize = (int)UncompressedSize();
								TRY
									pData = new BYTE[nUncompressedSize];
								END_TRY
								if (pData == NULL)
								{
									error = ERRORCODE_Memory;
								}
								else
								{
									pBytePtr = (LPBYTE)bitmap;

									unPackBits(ReadingRead, pData, nUncompressedSize);

									FreeBitmap();
									bitmap = pData;
								}
								break;
							}
							case COMPRESSION_Implode:
							{
								LPBYTE pData = NULL;
								int nUncompressedSize = (int)UncompressedSize();
								TRY
									pData = new BYTE[nUncompressedSize];
								END_TRY
								if (pData == NULL)
								{
									error = ERRORCODE_Memory;
								}
								else
								{
									CImplode imp;

									imp.Decompress((LPBYTE)bitmap, record.dwCompressedSize, pData);

									FreeBitmap();
									bitmap = pData;
								}
								break;
							}
							case COMPRESSION_Deflate:
							{
								LPBYTE pData = NULL;
								DWORD dwDataSize = 0;

								CDeflate def;

								def.Decompress((LPBYTE)bitmap, record.dwCompressedSize, &pData, &dwDataSize);

								if (pData != NULL)
								{
									FreeBitmap();
									TRY
										bitmap = new BYTE[(int)dwDataSize];
									END_TRY
									if (bitmap == NULL)
									{
										error = ERRORCODE_Memory;
									}
									else
									{
										memcpy(bitmap, pData, (int)dwDataSize);
									}

									CDeflate::FreeData(pData);
								}
								break;
							}
						}
					}
				}
			}
		}
		else
		{
		/* Bitmap is valid and loaded. */
			error = ERRORCODE_None;
		}
	}

	*bm = bitmap;

	return error;
}

/*
// Create an HBITMAP for the preview.
// If the preview is a DIB, it uses a screen DC to convert the DIB to a DDB.
*/

HBITMAP ProjectPreviewRecord::GetPreviewBitmap(CDC* pDC /*=NULL*/)
{
	VOIDPTR mem;
	HBITMAP hPreview = NULL;
	int nBitmapWidth = get_width();
	int nBitmapHeight = get_height();

	if (read_preview(&mem) == ERRORCODE_None)
	{
		switch (GetType())
		{
			case ProjectPreviewRecord::PREVIEW_TYPE_Monochrome:
			{
				hPreview = ::CreateBitmap(nBitmapWidth,
												  nBitmapHeight,
												  1, 1,
												  (LPBYTE)mem);
				break;
			}
			case ProjectPreviewRecord::PREVIEW_TYPE_ColorDIB:
			{
				HDC hDC;

				if (pDC == NULL)
				{
					hDC = ::GetDC(NULL);
				}
				else
				{
					hDC = pDC->GetSafeHdc();
				}

				if (hDC != NULL)
				{
					int nDataBytes = (int)preview_size();
					int nHeaderBytes = sizeof(BITMAPINFO) + 255*sizeof(RGBQUAD);

					BITMAPINFO* pbi = (BITMAPINFO*)(new BYTE[nHeaderBytes]);

					pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
					pbi->bmiHeader.biWidth = (LONG)nBitmapWidth;
					pbi->bmiHeader.biHeight = (LONG)nBitmapHeight;
					pbi->bmiHeader.biPlanes = 1;
					pbi->bmiHeader.biBitCount = 8;
					pbi->bmiHeader.biCompression = BI_RGB;
					pbi->bmiHeader.biSizeImage = (DWORD)nDataBytes;
					pbi->bmiHeader.biXPelsPerMeter =
						pbi->bmiHeader.biYPelsPerMeter = 0;
					pbi->bmiHeader.biClrUsed =
						pbi->bmiHeader.biClrImportant = 0;

					memcpy(pbi->bmiColors, GetIdentityPalette(), 256*sizeof(RGBQUAD));

					hPreview = ::CreateDIBitmap(hDC,
														 &pbi->bmiHeader,
														 CBM_INIT,
														 mem,
														 pbi,
														 DIB_RGB_COLORS);
					if (pDC == NULL)
					{
						::ReleaseDC(NULL, hDC);
					}
					delete [] (LPBYTE)pbi;
				}

				break;
			}
		}
	}
	return hPreview;
}

/*
// Attach a preview to the record.
// The preview bitmap should be allocated as "new BYTE[x]".
// Any previous bitnmap image is freed.
*/

ERRORCODE ProjectPreviewRecord::attach_preview_bitmap(VOIDPTR bm)
{
	if (!record.valid)
	{
		return ERRORCODE_NotInitialized;
	}
	else
	{
		FreeBitmap();
		bitmap = bm;
	}

	return ERRORCODE_None;
}

/*
// Attach a description to the preview.
*/

ERRORCODE ProjectPreviewRecord::set_description(LPCSTR string)
{
	modified();

	m_csDescription = string;

	return ERRORCODE_None;
}

/*
// Set the preview parameters. This makes the preview valid.
*/

VOID ProjectPreviewRecord::init_preview(SHORT w, SHORT h, SHORT xr, SHORT yr, SHORT nType /*=PREVIEW_TYPE_Monochrome*/)
{
/* Remove any old preview data. */

	remove_preview();

/* Set the new parameters. */

	record.width = w;
	record.height = h;
	record.x_resolution = xr;
	record.y_resolution = yr;
	record.nType = nType;
	record.nCompression = COMPRESSION_Unknown;
	record.dwCompressedSize = 0;

	record.valid = TRUE;
}

/*
// Remove the preview. The preview becomes "invalid".
*/

VOID ProjectPreviewRecord::remove_preview(VOID)
{
	if (record.valid)
	{
		record.valid = FALSE;

		FreeBitmap();

		bitmap_position = POSITION_UNKNOWN;
	}
}

/*
// Assign method.
*/

ERRORCODE ProjectPreviewRecord::assign(DatabaseRecordRef srecord)
{
	ERRORCODE error;

/* Get rid of any existing preview data. */

	remove_preview();

/* Assign the base record first. */

	if ((error = DatabaseRecord::assign(srecord)) == ERRORCODE_None)
	{
		ProjectPreviewRef precord = (ProjectPreviewRef)srecord;
		record = precord.record;
		m_csDescription = precord.m_csDescription;
		m_csAlbumPath = precord.m_csAlbumPath;
#if 0
		if (bitmap != NULL)
		{
		/* Copy the preview bitnmap. */
		}
#endif
	}
	return error;
}
