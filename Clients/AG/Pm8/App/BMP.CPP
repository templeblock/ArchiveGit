/*
// BMP.C
//
// Routines for reading a Windows BitMaP file.
//
// $Header: /PM8/App/BMP.CPP 1     3/03/99 6:03p Gbeddow $
//
// $Log: /PM8/App/BMP.CPP $
// 
// 1     3/03/99 6:03p Gbeddow
// 
// 6     9/14/98 12:05p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 5     8/20/98 11:41a Dennis
// Type casted x_resolution assignments.
// 
// 4     8/20/98 11:02a Jayn
// Resolution changes.
// 
// 3     8/18/98 9:49a Dennis
// Added check to open_image for valid Palette Parameter if image is 256
// color.
// 
// 2     1/29/98 8:11p Hforman
// add GlobalUnlock/GlobalFree in CreateBMPFromBitmap()
// 
//    Rev 1.0   14 Aug 1997 15:18:20   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:37:12   Fred
// Initial revision.
// 
//    Rev 1.9   11 Apr 1997 11:23:26   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.8   05 Dec 1996 12:30:04   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.7   05 Nov 1996 14:06:42   Jay
// Got rid of warnings, etc.
// 
//    Rev 1.6   01 Nov 1996 17:11:36   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.5   18 Oct 1996 11:03:42   Jay
// Now uses palette in b&w BMP.
// 
//    Rev 1.4   07 Oct 1996 13:19:46   Jay
// prep_storage_file()
// 
//    Rev 1.3   01 Oct 1996 10:48:20   DennisP
// Fixed missing type cast for 16-bit at FindDIBBits call at line 662.
// 
//    Rev 1.2   20 Sep 1996 13:41:20   Jay
// Support for Temp DIB graphic records.
// 
//    Rev 1.1   25 Aug 1996 12:59:20   Jay
// No longer sets default name at low level.
// 
//    Rev 1.0   14 Mar 1996 13:42:28   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:05:54   FRED
// Initial revision.
// 
//    Rev 2.1   13 Sep 1995 11:33:46   JAY
// New 32-bit stuff
// 
//    Rev 2.0   07 Feb 1995 15:47:32   JAY
// Initial revision.
// 
//    Rev 1.7   09 Jan 1995 16:26:46   JAY
// New stuff (cut & paste, etc).
// 
//    Rev 1.6   30 Nov 1994 16:34:10   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.5   04 Aug 1994 07:33:54   JAY
// Large model and 32-bit changes
// 
//    Rev 1.4   26 May 1994 13:40:10   JAY
// Added a couple of error checks.
// 
//    Rev 1.3   23 May 1994 08:21:54   JAY
// Now uses prep_storage_file() instead of the direct init so we can use files
// based on the document database device.
// 
//    Rev 1.2   13 Jan 1994 17:00:30   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 08:05:54   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:04   JAY
// 
*/

#include	"stdafx.h"

#include <memory.h>

#include "pmgdb.h"
#include	"grafrec.h"
#include	"grafobj.h"
#include	"bmp.h"
#include	"memdev.h"

// WIDTHBYTES takes # of bits in a scan line and rounds up to nearest
//  word.

#define WIDTHBYTES(bits)      (((bits) + 31) / 32 * 4)

POINT BMPHelper::m_pntResolution = { 300, 300 };

/*
// Read the BMP helper info.
*/

ERRORCODE BMPHelper::read(StorageDevicePtr device)
{
	return device->read_record(&record, sizeof(record));
}

/*
// Write the BMP helper info.
*/

ERRORCODE BMPHelper::write(StorageDevicePtr device)
{
	return device->write_record(&record, sizeof(record));
}

/*
// Size the BMP helper info.
*/

ST_DEV_IO_SIZE BMPHelper::size(StorageDevicePtr device)
{
	return device->size_record(sizeof(record));
}

/*
// Buffer routines and variables.
*/

ERRORCODE near BMPHelper::read_bmp_header(StorageDevicePtr file,
					LPWORD dots, LPWORD lines,	LPBYTE bpp,
					LPBYTE compression, LPDWORD header_size,
					LPDWORD bitmap_offset)
{
	BITMAPFILEHEADER file_header;
	union
	{
		BITMAPINFOHEADER	info;
		BITMAPCOREHEADER	core;
	} bitmap_header;
	ERRORCODE error;

/* Read the initial file header. */

	if ((error = file->read(&file_header, sizeof(file_header))) != ERRORCODE_None)
	{
		return error;
	}

	if (file_header.bfType != (SHORT)'B'+ (((SHORT)'M') << 8))
	{
		return ERRORCODE_IllegalType;
	}

	*bitmap_offset = file_header.bfOffBits;

/* Read the next word to see how big the thing is we need to read. */

	if ((error = file->read(&bitmap_header.info.biSize, sizeof(DWORD))) != ERRORCODE_None)
	{
		return error;
	}

/* Do a kludge for this BMP I found.  (OS/2 2.0?) */

	if (bitmap_header.info.biSize == sizeof(BITMAPINFOHEADER)-sizeof(DWORD))
	{
		bitmap_header.info.biSize += sizeof(DWORD);
	}

/* Read the rest of the header. */

	if ((error = file->read(&bitmap_header.info.biWidth,
				(ST_DEV_IO_SIZE)(bitmap_header.info.biSize-sizeof(DWORD)))) != ERRORCODE_None)
	{
		return error;
	}

	if ((*header_size = bitmap_header.info.biSize) == sizeof(BITMAPINFOHEADER))
	{
	/* Windows 3.0 bitmap. Use INFO_HEADER. */
		*dots = (WORD)bitmap_header.info.biWidth;
		*lines = (WORD)bitmap_header.info.biHeight;
		*bpp = (BYTE)bitmap_header.info.biBitCount;
		*compression = (BYTE)bitmap_header.info.biCompression;
	}
	else
	{
	/* OS/2 bitmap. Use CORE_HEADER. */
		*dots = bitmap_header.core.bcWidth;
		*lines = bitmap_header.core.bcHeight;
		*bpp = (BYTE)bitmap_header.core.bcBitCount;
		*compression = 0;
	}

	return error;
}

/*
// Initialize a Dr. Halo file.
*/

ERRORCODE BMPHelper::init(GRAPHIC_CREATE_STRUCT_PTR gcs)
{
	ERRORCODE error;

/* We need a file to read. */

	StorageDevicePtr pSource;
	ReadOnlyFile file;

	if (gcs == NULL || (pSource = gcs->pSourceDevice) == NULL)
	{
	/* Go directly to disk. */
		file.set_name(graphic->m_csFileName);
		pSource = &file;
	}

	if ((error = read_bmp_header(pSource,
						(LPWORD)&graphic->record.x_size, (LPWORD)&graphic->record.y_size,
						&record.bits_per_pixel,
						&record.compression,
						&record.header_size,
						&record.bitmap_offset)) == ERRORCODE_None)
	{
		graphic->record.x_resolution = (USHORT) m_pntResolution.x;
		graphic->record.y_resolution = (USHORT) m_pntResolution.y;
		graphic->record.storage = GRAPHIC_STORAGE_FILE;
	}

	return error;
}

/*
// The constructor for the BMP driver.
*/

BMPDriver::BMPDriver(GraphicPtr g, StorageDevice* pSource /*=NULL*/)
{
	graphic = g;
	if (pSource == NULL)
	{
		graphic->prep_storage_file(&file);
	}
	else
	{
		file.set_based_device(pSource);
	}
}

/*
// The image_info() method for pcx files.
*/

ERRORCODE BMPDriver::image_info(IMAGE_INFO_PTR info)
{
	BMPHelperPtr helper = (BMPHelperPtr)graphic->my_little_helper();

	info->planes = 1;
	info->do_yflip = TRUE;
	info->red_index = 2;			/* Damn reversed thing! */
	info->width = graphic->record.x_size;
	info->height = graphic->record.y_size;

	switch (info->bpp = helper->record.bits_per_pixel)
	{
		case 1:
		{
			info->image_type = IMAGE_BILEVEL;
			break;
		}
		case 4:
		case 8:
		{
			info->image_type = IMAGE_PALETTE;
			break;
		}
		case 24:
		{
			info->planes = 3;
			info->bpp = 8;
			info->image_type = IMAGE_FULL_COLOR;
			break;
		}
		default:
		{
			return ERRORCODE_IllegalType;
		}
	}

	return ERRORCODE_None;
}

/*
// The open_image() method for BMP files.
*/

ERRORCODE BMPDriver::open_image(PALETTE_INFO_PTR palette)
{
	BMPHelperPtr helper = (BMPHelperPtr)graphic->my_little_helper();
	ERRORCODE error;

/* If we need to read the palette, do it now. */

	if (our_info.image_type != IMAGE_FULL_COLOR)
	{
		SHORT size;

	/* Seek to the start of the palette. */

		if ((error = file.seek(helper->record.header_size+sizeof(BITMAPFILEHEADER), ST_DEV_SEEK_SET)) != ERRORCODE_None)
		{
			return error;
		}

	/* Compute the size of a palette element. */

		if (helper->record.header_size == sizeof(BITMAPINFOHEADER))
		{
		/* Do it as a Windows 3.0 header. */
			size = sizeof(RGBQUAD);
		}
		else
		{
		/* Do it as an OS/2 header. */
			size = sizeof(RGBTRIPLE);
		}

      ASSERT(palette != NULL);
      if(palette == NULL)
         return ERRORCODE_BadParameter;

		LPBYTE buffer = NULL;
		SHORT buffer_size = palette->m_nColors*size;

		TRY
			buffer = new BYTE[buffer_size];
		END_TRY

		if (buffer != NULL)
		{
			LPBYTE sp, p;
			SHORT i;

			if ((error = file.read(buffer, buffer_size)) != ERRORCODE_None)
			{
				delete [] buffer;
				return error;
			}

		/* Convert RGBQUADs or WINRGBTRIPLEs to our RGBTRIPLEs. */

			sp = buffer;
			p = (LPBYTE)palette->m_pData;

			for (i = palette->m_nColors; i-- > 0; )
			{
				*p++ = sp[0];
				*p++ = sp[1];
				*p++ = sp[2];
				sp += size;
			}
			delete [] buffer;
			palette->m_fRead = TRUE;
		}
	}

	source_compression = helper->record.compression;

/* Compute how many bytes there are in a (long-padded) row. */

	buffer_byte_width = (graphic->record.x_size*our_info.bpp*our_info.planes+7)/8;
	source_byte_width = (buffer_byte_width + 3) & ~3;

/* Seek to the start of the data. */

	current_line = 0;
	if ((error = file.seek(helper->record.bitmap_offset, ST_DEV_SEEK_SET)) == ERRORCODE_None)
	{
		file.set_read_buffer(2048);
	}

	return error;
}

/*
// The read_block() method for BMP files.
*/

ERRORCODE BMPDriver::read_lines(int line, LPBYTE buffer, int count, LPBYTE pMask)
{
	ASSERT(pMask == NULL);

	SHORT compression = source_compression;
	SHORT nibble_mode = (compression == 2);
	ERRORCODE error;
	SHORT row_bytes = source_byte_width;
	SHORT source_bytes = buffer_byte_width;

/* Find the starting place. */

	SHORT skip_count = line - current_line;

#if 0
	printf("row_bytes: %d, source_row_size:%d, source_row:%Fp\n",
					row_bytes, paint_info.source_row_size, paint_info.source_row);

	printf("read from %d to %d\n", paint_info.srce.y0, paint_info.srce.y1);
#endif

/* Skip the number of lines we need to. */

	if (skip_count != 0)
	{
		if (compression == 0)
		{
		/* Seek relatively. */

			file.seek((ST_DEV_POSITION)skip_count*(LONG)row_bytes, ST_DEV_SEEK_CUR);
			current_line += skip_count;
		}
		else
		{
			while (skip_count-- != 0)
			{
				BOOL done = FALSE;
				SHORT total = 0;

				while (!done)
				{
					UCHAR byte[2];

					if ((error = file.read(byte, 2)) != ERRORCODE_None)
					{
						return error;
					}

//					printf("skipping bytes: %02x, %02x\n", byte[0], byte[1]);

					if (byte[0] == 0)
					{
					/* This is an escape. */
						switch (byte[1])
						{
							case 0:
							{
							/* End of line. */
								done = TRUE;
								break;
							}
							case 1:
							{
							/* End of bitmap. */
								return ERRORCODE_Read;
							}
							case 2:
							{
							/* Movement. Skip the bytes which follow. */
//								file.seek(2, ST_DEV_SEEK_CUR);
								return ERRORCODE_InvalidRecord;
							}
							default:
							{
							/* An absolute sequence. Skip the data bytes. */
								if (nibble_mode)
								{
									byte[1] = (byte[1]+1)/2;
								}
								file.seek((byte[1]+1) & ~1, ST_DEV_SEEK_CUR);
								total += byte[1];
								break;
							}
						}
					}
					else
					{
						if (nibble_mode)
						{
							byte[0] = (byte[0]+1) / 2;
						}
						total += byte[0];
					}
				}
//				printf("((%d))\n", total);
				current_line++;
			}
		}
	}

/* Read the line. */

	while (count-- > 0)
	{
		if (compression == 0)
		{
		/* Read the line. */

			if ((error = file.read(buffer, source_bytes)) != ERRORCODE_None)
			{
				return error;
			}
			else if (row_bytes != source_bytes)
			{
				file.seek(row_bytes - source_bytes, ST_DEV_SEEK_CUR);
			}
		}
		else
		{
		/* Handle compressed data. */

			int size = source_bytes;
			BOOL done = FALSE;
			LPBYTE p = buffer;
			int total = 0;

			while (!done)
			{
				BYTE byte[2];

			/* Read the next two control bytes. */

				if ((error = file.read(byte, 2)) != ERRORCODE_None)
				{
					return error;
				}

//				printf("reading bytes: %02x, %02x\n", byte0, byte1);

				if (byte[0] == 0)
				{
				/* This is an escape. */
					switch (byte[1])
					{
						case 0:
						{
						/* End of line. */
							done = TRUE;
							break;
						}
						case 1:
						{
						/* End of bitmap. */
							done = TRUE;			/* Really done. */
							break;
						}
						case 2:
						{
						/*
						// Movement. Skip the bytes which follow.
						// We don't really know how to deal with this right now.
						*/
//							file.seek(2, ST_DEV_SEEK_CUR);
							return ERRORCODE_InvalidRecord;
						}
						default:
						{
						/* An absolute sequence. Read the data bytes. */
							int seek_extra;
//							printf("\t");
							if (nibble_mode)
							{
								byte[1] = ((byte[1]+1) / 2);
							}
							seek_extra = ((byte[1]+1) & ~1)-byte[1];
							total += byte[1];

							if (byte[1] > size)
							{
								seek_extra += byte[1] - size;
								byte[1] = (BYTE)size;
							}

							file.read(p, byte[1]);
							p += byte[1];
							size -= byte[1];

							if (seek_extra)
							{
								file.seek(seek_extra, ST_DEV_SEEK_CUR);
							}
//							printf("\n");
							break;
						}
					}
				}
				else
				{
				/* This is an encoded sequence. Store it. */
					if (nibble_mode)
					{
						byte[0] = (byte[0]+1)/2;
					}
					total += byte[0];

					if (size != 0)
					{
						if (byte[0] > size)
						{
							byte[0] = (BYTE)size;
						}
						memset(p, byte[1], byte[0]);
						size -= byte[0];
						p += (int)byte[0];
					}
				}
			}
//			printf("<<%d>>\n", total);
		}
		buffer += source_bytes;
		current_line++;
	}

	return ERRORCODE_None;
}

/////////////////////////////////////////////////////////////////////////////
// DIBImageDriver

DIBImageDriver::DIBImageDriver(LPBITMAPINFO pDib)
{
	m_pDib = pDib;
	ASSERT(m_pDib != NULL);
}

DIBImageDriver::~DIBImageDriver()
{
}

ERRORCODE DIBImageDriver::image_info(IMAGE_INFO_PTR info)
{
	ASSERT(m_pDib != NULL);
	if (m_pDib == NULL)
	{
		return ERRORCODE_IntError;
	}

	info->planes = 1;
	info->do_yflip = TRUE;		// Image is stored upside down.
	info->red_index = 2;			/* Damn reversed thing! */
	info->width = (SHORT)m_pDib->bmiHeader.biWidth;
	info->height = (SHORT)m_pDib->bmiHeader.biHeight;

	switch (info->bpp = (char)m_pDib->bmiHeader.biBitCount)
	{
		case 1:
		{
			info->image_type = IMAGE_BILEVEL;
			break;
		}
		case 4:
		case 8:
		{
			info->image_type = IMAGE_PALETTE;
			break;
		}
		case 24:
		{
			info->planes = 3;
			info->bpp = 8;
			info->image_type = IMAGE_FULL_COLOR;
			break;
		}
		default:
		{
			return ERRORCODE_IllegalType;
		}
	}

	return ERRORCODE_None;
}

ERRORCODE DIBImageDriver::open_image(PALETTE_INFO_PTR palette)
{
	ASSERT(m_pDib != NULL);
	if (m_pDib == NULL)
	{
		return ERRORCODE_IntError;
	}

	// If we have a palette, copy it now.
	if (our_info.image_type == IMAGE_PALETTE)
	{
		LPBYTE sp = (LPBYTE)m_pDib->bmiColors;
		LPBYTE p = (LPBYTE)palette->m_pData;

		// Convert RGBQUADs to our RGBTRIPLEs.
		for (int i = palette->m_nColors; i-- > 0; )
		{
			*p++ = sp[0];
			*p++ = sp[1];
			*p++ = sp[2];
			sp += sizeof(RGBQUAD);
		}
		palette->m_fRead = TRUE;
	}

/* Compute how many bytes there are in a (long-padded) row. */

	buffer_byte_width = (our_info.width*our_info.bpp*our_info.planes+7)/8;
	source_byte_width = (buffer_byte_width + 3) & ~3;

/* Seek to the start of the data. */

	current_line = 0;
#ifdef WIN32
	m_pCurrentByte = (LPBYTE)FindDIBBits(m_pDib);
#else
	m_pCurrentByte = (BYTE __huge*)FindDIBBits(m_pDib);
#endif

	return ERRORCODE_None;
}

ERRORCODE DIBImageDriver::read_lines(int line, LPBYTE buffer, int count, LPBYTE pMask)
{
	ASSERT(pMask == NULL);

	ASSERT(m_pDib != NULL);
	if (m_pDib == NULL)
	{
		return ERRORCODE_IntError;
	}

	int compression = (int)m_pDib->bmiHeader.biCompression;
	int nibble_mode = (compression == 2);
	int row_bytes = source_byte_width;
	int source_bytes = buffer_byte_width;

/* Find the starting place. */

	SHORT skip_count = line - current_line;

#if 0
	printf("row_bytes: %d, source_row_size:%d, source_row:%Fp\n",
					row_bytes, paint_info.source_row_size, paint_info.source_row);

	printf("read from %d to %d\n", paint_info.srce.y0, paint_info.srce.y1);
#endif

/* Skip the number of lines we need to. */

	if (skip_count != 0)
	{
		if (compression == 0)
		{
		/* Seek relatively. */

			m_pCurrentByte += skip_count*(LONG)row_bytes;
			current_line += skip_count;
		}
		else
		{
			while (skip_count-- != 0)
			{
				BOOL done = FALSE;
				SHORT total = 0;

				while (!done)
				{
					UCHAR byte[2];

					byte[0] = m_pCurrentByte[0];
					byte[1] = m_pCurrentByte[1];
					m_pCurrentByte += 2;

//					printf("skipping bytes: %02x, %02x\n", byte[0], byte[1]);

					if (byte[0] == 0)
					{
					/* This is an escape. */
						switch (byte[1])
						{
							case 0:
							{
							/* End of line. */
								done = TRUE;
								break;
							}
							case 1:
							{
							/* End of bitmap. */
								return ERRORCODE_Read;
							}
							case 2:
							{
							/* Movement. Skip the bytes which follow. */
//								file.seek(2, ST_DEV_SEEK_CUR);
								return ERRORCODE_InvalidRecord;
							}
							default:
							{
							/* An absolute sequence. Skip the data bytes. */
								if (nibble_mode)
								{
									byte[1] = (byte[1]+1)/2;
								}
								m_pCurrentByte += (byte[1]+1) & ~1;
								total += byte[1];
								break;
							}
						}
					}
					else
					{
						if (nibble_mode)
						{
							byte[0] = (byte[0]+1) / 2;
						}
						total += byte[0];
					}
				}
//				printf("((%d))\n", total);
				current_line++;
			}
		}
	}

/* Read the line. */

	while (count-- > 0)
	{
		if (compression == 0)
		{
		/* Read the line. */

#ifdef WIN32
			memcpy(buffer,  m_pCurrentByte, source_bytes);
#else
			hmemcpy(buffer, m_pCurrentByte, source_bytes);
#endif
			m_pCurrentByte += row_bytes;
		}
		else
		{
		/* Handle compressed data. */

			int size = source_bytes;
			BOOL done = FALSE;
			LPBYTE p = buffer;
			int total = 0;

			while (!done)
			{
				BYTE byte[2];

			/* Read the next two control bytes. */
				byte[0] = m_pCurrentByte[0];
				byte[1] = m_pCurrentByte[1];
				m_pCurrentByte += 2;

//				printf("reading bytes: %02x, %02x\n", byte0, byte1);

				if (byte[0] == 0)
				{
				/* This is an escape. */
					switch (byte[1])
					{
						case 0:
						{
						/* End of line. */
							done = TRUE;
							break;
						}
						case 1:
						{
						/* End of bitmap. */
							done = TRUE;			/* Really done. */
							break;
						}
						case 2:
						{
						/*
						// Movement. Skip the bytes which follow.
						// We don't really know how to deal with this right now.
						*/
//							file.seek(2, ST_DEV_SEEK_CUR);
							return ERRORCODE_InvalidRecord;
						}
						default:
						{
						/* An absolute sequence. Read the data bytes. */
							SHORT seek_extra;
//							printf("\t");
							if (nibble_mode)
							{
								byte[1] = ((byte[1]+1) / 2);
							}
							seek_extra = ((byte[1]+1) & ~1)-byte[1];
							total += byte[1];

							if (byte[1] > size)
							{
								seek_extra += byte[1] - size;
								byte[1] = (BYTE)size;
							}

							byte[1] = *m_pCurrentByte;
							p += byte[1];
							size -= byte[1];

							m_pCurrentByte += seek_extra + 1;
//							printf("\n");
							break;
						}
					}
				}
				else
				{
				/* This is an encoded sequence. Store it. */
					if (nibble_mode)
					{
						byte[0] = (byte[0]+1)/2;
					}
					total += byte[0];

					if (size != 0)
					{
						if (byte[0] > size)
						{
							byte[0] = (BYTE)size;
						}
						memset(p, byte[1], byte[0]);
						size -= byte[0];
						p += (SHORT)byte[0];
					}
				}
			}
//			printf("<<%d>>\n", total);
		}
		buffer += source_bytes;
		current_line++;
	}

	return ERRORCODE_None;
}

/////////////////////////////////////////////////////////////////////////////
// General utilities.

/*
// Create a BMP graphic from a packed DIB structure.
*/

ERRORCODE CreateBMPFromDIB(LPCSTR pName,
									PMGDatabase* pDatabase,
									BITMAPINFO* pBmi,
									GraphicObject*& pObject)
{
	ERRORCODE error;

	pObject = NULL;

	od("BMP - w: %ld, h: %ld, bits: %d\r\n",
		pBmi->bmiHeader.biWidth,
		pBmi->bmiHeader.biHeight,
		pBmi->bmiHeader.biBitCount);

/*
// Compute the size of the BMP data.
*/

	DWORD nByteWidth = (pBmi->bmiHeader.biWidth*pBmi->bmiHeader.biBitCount+7)/8;
	nByteWidth = (nByteWidth + 3) & ~3;

	DWORD dwSizeImage = (DWORD)nByteWidth*(DWORD)pBmi->bmiHeader.biHeight;
	int nColors = (int)((pBmi->bmiHeader.biClrUsed == 0)
								? pBmi->bmiHeader.biBitCount > 8
									? 0
									: (1 << pBmi->bmiHeader.biBitCount)
								: pBmi->bmiHeader.biClrUsed);

	DWORD dwSizeColors = nColors*sizeof(pBmi->bmiColors[0]);

	DWORD dwDataSize = pBmi->bmiHeader.biSize
							+ dwSizeColors
							+ dwSizeImage;

/*
// Build the device image.
// This will consist of two pieces:
// (1) Our constructed BMP file header, and
// (2) The actual BMP data.
*/

	MemoryDevice DataDevice;

	// (1) The header.

	BITMAPFILEHEADER Header;

	Header.bfType = (SHORT)'B' + (((SHORT)'M') << 8);
	Header.bfSize = sizeof(Header) + dwDataSize;
	Header.bfReserved1 = 0;
	Header.bfReserved2 = 0;
	Header.bfOffBits = sizeof(Header)
							+ pBmi->bmiHeader.biSize
							+ dwSizeColors;

	DataDevice.AddPiece(&Header, sizeof(Header));

	// (2) The BMP data.

	DataDevice.AddPiece(pBmi, dwDataSize);

/*
// Build the graphic creation structure.
// We want to embed source so it will use our little memory device.
*/

	GRAPHIC_CREATE_STRUCT gcs;

	gcs.m_csFileName = pName;
	gcs.embed_source = TRUE;
	gcs.pSourceDevice = &DataDevice;

/* Create the graphic. */

	if ((pObject = pDatabase->create_graphic_object(&gcs)) == NULL)
	{
		error = pDatabase->last_creation_error();
	}
	else
	{
	/* Success! */
		error = ERRORCODE_None;
	}

	return error;
}

ERRORCODE CreateBMPFromBitmap(LPCSTR pName,
										PMGDatabase* pDatabase,
										HBITMAP hBitmap,
										GraphicObject*& pObject,
										HPALETTE hPalette /*=NULL*/)
{
	HGLOBAL hDib;
	ERRORCODE error;

/* Convert the bitmap to a DIB. */

	if ((error = BitmapToDIB(hBitmap, hPalette, &hDib)) != ERRORCODE_None)
	{
		return error;
	}

/* Get the DIB bits. */

	BITMAPINFO* pBmi = (BITMAPINFO*)::GlobalLock(hDib);
	if (pBmi == NULL)
	{
		error = ERRORCODE_Memory;
	}
	else
	{
	/* Convert it to a BMP graphic. */
		error = CreateBMPFromDIB(pName, pDatabase, pBmi, pObject);
	}

	::GlobalUnlock(hDib);
	::GlobalFree(hDib);

	return error;
}

//---------------------------------------------------------------------
//
// Function:   DIBNumColors
//
// Purpose:    Given a pointer to a DIB, returns a number of colors in
//             the DIB's color table.
//
// Parms:      lpbi == pointer to DIB header (either BITMAPINFOHEADER
//                       or BITMAPCOREHEADER)
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

WORD DIBNumColors(LPVOID lpbi)
{
   WORD wBitCount;

      // If this is a Windows style DIB, the number of colors in the
      //  color table can be less than the number of bits per pixel
      //  allows for (i.e. lpbi->biClrUsed can be set to some value).
      //  If this is the case, return the appropriate value.

	DWORD dwClrUsed;

	dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;

	if (dwClrUsed)
	{
		return (WORD)dwClrUsed;
	}

   // Calculate the number of colors in the color table based on
   //  the number of bits per pixel for the DIB.

   wBitCount = ((LPBITMAPINFOHEADER) lpbi)->biBitCount;

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

//---------------------------------------------------------------------
//
// Function:   PaletteSize
//
// Purpose:    Given a pointer to a DIB, returns number of bytes
//             in the DIB's color table.
//
// Parms:      lpbi == pointer to DIB header (either BITMAPINFOHEADER
//                       or BITMAPCOREHEADER)
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

WORD DIBPaletteSize(LPVOID lpbi)
{
	return (DIBNumColors(lpbi) * sizeof(RGBQUAD));
}

//---------------------------------------------------------------------
//
// Function:   InitBitmapInfoHeader
//
// Purpose:    Does a "standard" initialization of a BITMAPINFOHEADER,
//             given the Width, Height, and Bits per Pixel for the
//             DIB.
//
//             By standard, I mean that all the relevant fields are set
//             to the specified values.  biSizeImage is computed, the
//             biCompression field is set to "no compression," and all
//             other fields are 0.
//
//             Note that DIBs only allow BitsPixel values of 1, 4, 8, or
//             24.  This routine makes sure that one of these values is
//             used (whichever is most appropriate for the specified
//             nBPP).
//
// Parms:      lpBmInfoHdr == Far pointer to a BITMAPINFOHEADER structure
//                            to be filled in.
//             dwWidth     == Width of DIB (not in Win 3.0 & 3.1, high
//                            word MUST be 0).
//             dwHeight    == Height of DIB (not in Win 3.0 & 3.1, high
//                            word MUST be 0).
//             nBPP        == Bits per Pixel for the DIB.
//
// History:   Date      Reason
//            11/07/91  Created
//
//---------------------------------------------------------------------

void InitBitmapInfoHeader(LPBITMAPINFOHEADER lpBmInfoHdr,
								  DWORD dwWidth,
								  DWORD dwHeight,
								  int nBPP)
{
   memset (lpBmInfoHdr, 0, sizeof (BITMAPINFOHEADER));

   lpBmInfoHdr->biSize      = sizeof (BITMAPINFOHEADER);
   lpBmInfoHdr->biWidth     = dwWidth;
   lpBmInfoHdr->biHeight    = dwHeight;
   lpBmInfoHdr->biPlanes    = 1;

   if (nBPP <= 1)
      nBPP = 1;
   else if (nBPP <= 4)
      nBPP = 4;
   else if (nBPP <= 8)
      nBPP = 8;
   else
      nBPP = 24;

   lpBmInfoHdr->biBitCount  = nBPP;
   lpBmInfoHdr->biSizeImage = WIDTHBYTES (dwWidth * nBPP) * dwHeight;
}

//---------------------------------------------------------------------
//
// Function:   FindDIBBits
//
// Purpose:    Given a pointer to a DIB, returns a pointer to the
//             DIB's bitmap bits.
//
// Parms:      lpbi == pointer to DIB header (either BITMAPINFOHEADER
//                       or BITMAPCOREHEADER)
//
// History:   Date      Reason
//             6/01/91  Created
//
//---------------------------------------------------------------------

LPSTR FindDIBBits(LPVOID lpbi)
{
   return ((LPSTR)lpbi + *(LPDWORD)lpbi + DIBPaletteSize(lpbi));
}

//---------------------------------------------------------------------
//
// Function:   BitmapToDIB
//
// Purpose:    Given a device dependent bitmap and a palette, returns
//             a handle to global memory with a DIB spec in it.  The
//             DIB is rendered using the colors of the palette passed in.
//
//             Stolen almost verbatim from ShowDIB.
//             Stolen almost verbatim from DibView (JN 1/5/95).
//
// Parms:      hBitmap == Handle to device dependent bitmap compatible
//                        with default screen display device.
//             hPal    == Palette to render the DDB with.  If it's NULL,
//                        use the default palette.
//
// History:   Date      Reason
//             6/01/91  Created
//					1/05/95  Stolen for PMW
//
//---------------------------------------------------------------------

ERRORCODE BitmapToDIB(HBITMAP hBitmap, HPALETTE hPal, HGLOBAL* phDIB)
{
   BITMAP             Bitmap;
   BITMAPINFOHEADER   bmInfoHdr;
	LPBITMAPINFOHEADER lpbmInfoHdr;
   LPSTR              lpBits;
   HDC                hMemDC;
   HANDLE             hDIB;
   HPALETTE           hOldPal = NULL;

	*phDIB = NULL;			/* In case of failure. */

   // Do some setup -- make sure the Bitmap passed in is valid,
   //  get info on the bitmap (like its height, width, etc.),
   //  then setup a BITMAPINFOHEADER.

   if (hBitmap == NULL)
	{
      return ERRORCODE_BadParameter;
	}

   if (!::GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap))
	{
      return ERRORCODE_BadParameter;
	}

   InitBitmapInfoHeader(&bmInfoHdr,
                         Bitmap.bmWidth,
                         Bitmap.bmHeight,
                         Bitmap.bmPlanes * Bitmap.bmBitsPixel);

   // Now allocate memory for the DIB.  Then, set the BITMAPINFOHEADER
   //  into this memory, and find out where the bitmap bits go.

   hDIB = ::GlobalAlloc(GHND, sizeof (BITMAPINFOHEADER)
							 + DIBPaletteSize(&bmInfoHdr)
							 + bmInfoHdr.biSizeImage);

   if (hDIB == NULL)
	{
      return ERRORCODE_Memory;
	}

   lpbmInfoHdr = (LPBITMAPINFOHEADER)::GlobalLock(hDIB);
   *lpbmInfoHdr = bmInfoHdr;
   lpBits = FindDIBBits(lpbmInfoHdr);

   // Now, we need a DC to hold our bitmap.  If the app passed us
   //  a palette, it should be selected into the DC.

   hMemDC = ::GetDC(NULL);

   if (hPal)
   {
      hOldPal = ::SelectPalette(hMemDC, hPal, FALSE);
      ::RealizePalette(hMemDC);
   }

   // We're finally ready to get the DIB.  Call the driver and let
   //  it party on our bitmap.  It will fill in the color table,
   //  and bitmap bits of our global memory block.

   if (!::GetDIBits(hMemDC,
                   hBitmap,
                   0,
                   Bitmap.bmHeight,
                   lpBits,
                   (LPBITMAPINFO) lpbmInfoHdr,
                   DIB_RGB_COLORS))
   {
      ::GlobalUnlock(hDIB);
      ::GlobalFree(hDIB);
      hDIB = NULL;
   }
   else
	{
      ::GlobalUnlock(hDIB);
	}

   // Finally, clean up and return.

   if (hOldPal)
	{
		::SelectPalette(hMemDC, hOldPal, FALSE);
	}

   ::ReleaseDC(NULL, hMemDC);

	*phDIB = hDIB;
   return ERRORCODE_None;
}
