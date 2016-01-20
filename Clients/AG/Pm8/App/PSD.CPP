/*
// PSD.CPP
//
// $Header: /PM8/App/PSD.CPP 1     3/03/99 6:11p Gbeddow $
//
// Photoshop (PSD) routines.
//
// $Log: /PM8/App/PSD.CPP $
// 
// 1     3/03/99 6:11p Gbeddow
// 
// 2     9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:25:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:14   Fred
// Initial revision.
// 
//    Rev 1.4   05 Jun 1997 16:44:48   Jay
// HasTransparency
// 
//    Rev 1.3   30 Apr 1997 10:40:28   Jay
// Now reads resolution from PSD file.
// 
//    Rev 1.2   11 Apr 1997 11:23:40   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.1   05 Dec 1996 12:30:28   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.0   01 Nov 1996 10:26:04   Jay
// Initial revision.
*/

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>
#include <windowsx.h>

#include "grafrec.h"
#include "psd.h"
#include "utils.h"
#include "cfixed.h"

/*
// Swap a word of data.
*/

static void near
SwapWords(WORD* pData, long lCount)
{
#ifdef WIN32
   WORD* p = pData;
#else
   WORD huge* p = (WORD huge*)pData;
#endif
   while (lCount-- > 0)
   {
#ifdef _WIN32
	   BYTE tmp = ((BYTE*)p)[0];
	   ((BYTE*)p)[0] = ((BYTE*)p)[1];
	   ((BYTE*)p)[1] = tmp;
#else
	   _asm
	   {
		   les	bx, dword ptr p
		   mov	ax, es:[bx]
		   xchg	ah, al
		   mov	es:[bx], ax
	   }
#endif
      p++;
   }
}

static void near
swap_word(WORD* w)
{
#ifdef _WIN32
	BYTE tmp = ((BYTE*)w)[0];
	((BYTE*)w)[0] = ((BYTE*)w)[1];
	((BYTE*)w)[1] = tmp;
#else
	_asm
	{
		les	bx, dword ptr w
		mov	ax, es:[bx]
		xchg	ah, al
		mov	es:[bx], ax
	}
#endif
}

/*
// Swap a long word of data.
*/

static void near
swap_dword(DWORD* l)
{
#ifdef _WIN32
	WORD tmp = ((WORD*)l)[0];
	((WORD*)l)[0] = ((WORD*)l)[1];
	((WORD*)l)[1] = tmp;

	swap_word(((WORD *)l)+0);
	swap_word(((WORD *)l)+1);
#else
	_asm
	{
		les	bx, dword ptr l

		mov	ax, es:[bx+0]
		mov	dx, es:[bx+2]
		xchg	ah, al
		xchg	dh, dl
		mov	es:[bx+2], ax
		mov	es:[bx+0], dx
	}
#endif
}

static int TypeOfPSDImage(int nMode)
{
	static int nImageType[] =
	{
		IMAGE_BILEVEL,			// 0 (Bitmap)
		IMAGE_GRAYSCALE,		// 1 (Grayscale)
		IMAGE_PALETTE,			// 2 (Indexed)
		IMAGE_FULL_COLOR,		// 3 (RGB)
		-1,						// 4 (CMYK)
		-1,						// 5 (HSL?)
		-1,						// 6 (HSB?)
		-1,						// 7 (Multichannel)
		IMAGE_GRAYSCALE,		// 8 (Duotone)
		-1							// 9 (Lab)
	};

	return nImageType[nMode];
}

/////////////////////////////////////////////////////////////////////////////
// PSDHelper

/*
// Read the PSD helper info.
*/

ERRORCODE PSDHelper::read(StorageDevicePtr device)
{
	return device->read_record(&m_Record, sizeof(m_Record));
}

/*
// Write the PSD helper info.
*/

ERRORCODE PSDHelper::write(StorageDevicePtr device)
{
	return device->write_record(&m_Record, sizeof(m_Record));
}

/*
// Size the PSD helper info.
*/

ST_DEV_IO_SIZE PSDHelper::size(StorageDevicePtr device)
{
	return device->size_record(sizeof(m_Record));
}

/*
// Initialize a PSD graphic.
*/

ERRORCODE PSDHelper::init(GRAPHIC_CREATE_STRUCT_PTR gcs)
{
	ERRORCODE error = ERRORCODE_None;

/* We need a file to read. */

	StorageDevicePtr pSource;
	ReadOnlyFile file;

	if (gcs == NULL || (pSource = gcs->pSourceDevice) == NULL)
	{
	/* Go directly to disk. */
		file.set_name(graphic->m_csFileName);
		file.set_read_buffer(4096);
		pSource = &file;
	}

/* Read the header. */

	PSDHeader Header;
	TRY
	{
		if ((error = pSource->read(&Header, sizeof(Header))) != ERRORCODE_None)
		{
			ThrowErrorcodeException(error);
		}

		// Swap everything.
		swap_word(&Header.m_wChannels);
		swap_dword(&Header.m_dwRows);
		swap_dword(&Header.m_dwColumns);
		swap_word(&Header.m_wDepth);
		swap_word(&Header.m_wMode);

		// Validate that we support this number of channels.
		if (Header.m_wChannels > MAX_CHANNELS)
		{
			ThrowErrorcodeException(ERRORCODE_IllegalType);
		}

		// Fill out the header.
		graphic->record.x_size = (USHORT)Header.m_dwColumns;
		graphic->record.y_size = (USHORT)Header.m_dwRows;
		graphic->record.x_resolution = 200;		// Default for now...
		graphic->record.y_resolution = 200;		// ...read later
		graphic->record.storage = GRAPHIC_STORAGE_FILE;

		// Fill out our info.
		m_Record.m_wChannels = Header.m_wChannels;
		m_Record.m_wDepth = Header.m_wDepth;
		m_Record.m_wMode = Header.m_wMode;

		// Initialize the start of line variables.

#if 0
      for (int i = 0; i < 9; i++)
		{
			m_Record.m_Lines[i].line = MulDiv((int)Header.m_dwRows, i, 8);
			m_Record.m_Lines[i].offset = 0L;
		}
#endif

		DWORD dwSize;

		//
		// Process the color mode data section.
		//
		// Read the section size.
		if ((error = pSource->read(&dwSize, sizeof(dwSize))) != ERRORCODE_None)
		{
			ThrowErrorcodeException(error);
		}
		swap_dword(&dwSize);

		pSource->tell(&m_Record.m_lColorDataSection);

		// Skip to the next section.
		pSource->seek(m_Record.m_lColorDataSection + (ST_DEV_POSITION)dwSize, ST_DEV_SEEK_SET);

		//
		// Process the image resources section.
		//
		// Read the section size.
		if ((error = pSource->read(&dwSize, sizeof(dwSize))) != ERRORCODE_None)
		{
			ThrowErrorcodeException(error);
		}
		swap_dword(&dwSize);

		pSource->tell(&m_Record.m_lImageResourcesSection);

		// Skip to the next section.
		pSource->seek(m_Record.m_lImageResourcesSection + (ST_DEV_POSITION)dwSize, ST_DEV_SEEK_SET);

		//
		// Process the layer and mask section.
		//
		// Read the section size.
		if ((error = pSource->read(&dwSize, sizeof(dwSize))) != ERRORCODE_None)
		{
			ThrowErrorcodeException(error);
		}
		swap_dword(&dwSize);

		pSource->tell(&m_Record.m_lLayerMaskSection);

		//
		// Read the image resources we care about.
		//

		// Read the resolution information.
		ReadResolution(pSource);

		// Skip to the next section.
		pSource->seek(m_Record.m_lLayerMaskSection + (ST_DEV_POSITION)dwSize, ST_DEV_SEEK_SET);

		//
		// Process the image data section.
		//

		pSource->tell(&m_Record.m_lImageDataSection);

		// Read the compression value.
		if ((error = pSource->read(&m_Record.m_wCompression, sizeof(m_Record.m_wCompression))) != ERRORCODE_None)
		{
			ThrowErrorcodeException(error);
		}

		//
		// Compute the sizes of all data channels.
		//

		if (m_Record.m_wCompression)
		{
			//
			// Compression requires special handling.
			//

			// We are compressed; read the compression counts.
			LPWORD pCounts = NULL;		// Needs to be NULL at start.
			if ((error = ReadCompressionCounts(pSource, pCounts)) != ERRORCODE_None)
			{
				ThrowErrorcodeException(error);
			}

			// Run through all channels and compute compressed sizes.
			LPWORD p = pCounts;
			int nRows = graphic->record.y_size;
			for (WORD wChannel = 0; wChannel < m_Record.m_wChannels; wChannel++)
			{
				// Compute the size of this channel.
				long	lChannelSize = 0;
				for (int nRow = 0; nRow < nRows; nRow++)
				{
					lChannelSize += *p++;
				}

				// Save the channel size in the record.
				m_Record.m_lChannelSizes[wChannel] = lChannelSize;
			}
			// Free the count data.
			GlobalFreePtr(pCounts);
		}
		else
		{
			//
			// Not compressed. All channels are the same size.
			//

			DWORD dwChannelSize = Header.m_dwColumns * Header.m_dwRows;

			// Set them all to the same size.
			for (WORD wChannel = 0; wChannel < m_Record.m_wChannels; wChannel++)
			{
				m_Record.m_lChannelSizes[wChannel] = (long)dwChannelSize;
			}
		}
	}
	CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL

	return error;
}

BOOL PSDHelper::HasTransparency(void)
{
	int nType = TypeOfPSDImage(m_Record.m_wMode);
	if (nType == -1)
	{
		return FALSE;
	}

	int nPlanes = (nType == IMAGE_FULL_COLOR) ? 3 : 1;

	return (m_Record.m_wChannels > nPlanes);
}

ERRORCODE PSDHelper::ReadCompressionCounts(StorageDevice* pFile, LPWORD& pSizes)
{
	ERRORCODE error = ERRORCODE_None;

   if (m_Record.m_wCompression)
	{
		// Should not be allocated yet.
		ASSERT(pSizes == NULL);

		// We have compressed data. Read the row counts.
		// There is a count for each row in each channel (rows*channels values).
		// Counts are 2-byte values.
		DWORD dwCount = (DWORD)m_Record.m_wChannels * (DWORD)graphic->record.y_size * 2;

		// Allocate the memory block.
		pSizes = (WORD*)GlobalAllocPtr(GMEM_MOVEABLE, dwCount*2);
		if (pSizes == NULL)
		{
			error = ERRORCODE_Memory;
		}
		else
		{
			// Read the values.
			if ((error = pFile->huge_read(pSizes, dwCount*2)) == ERRORCODE_None)
			{
				// We got the values. Unswap them.
				SwapWords(pSizes, dwCount);
			}
			else
			{
				// Failure! Free the memory block.
				GlobalFreePtr(pSizes);
				pSizes = NULL;
			}
		}
	}
	return error;
}

/*
// Read the resolution. This assumes m_lImageResourcesSection is set.
*/

void PSDHelper::ReadResolution(StorageDevice* pFile)
{
	DWORD dwDataSize;
	if (SeekImageResource(pFile, 0x03ED, dwDataSize) == ERRORCODE_None)
	{
		ASSERT(dwDataSize == sizeof(PSDResolutionInfo));
		if (dwDataSize == sizeof(PSDResolutionInfo))
		{
			PSDResolutionInfo Info;
			if (pFile->read(&Info, sizeof(Info)) == ERRORCODE_None)
			{
				// Extract the resolution.
				swap_dword((DWORD*)&Info.m_dwHRes);
				graphic->record.x_resolution = FixedInteger(Info.m_dwHRes);
				swap_dword((DWORD*)&Info.m_dwVRes);
				graphic->record.y_resolution = FixedInteger(Info.m_dwVRes);
				TRACE("Got PSD resolution %d x %d\n",
						graphic->record.x_resolution,
						graphic->record.y_resolution);
			}
		}
	}
}

/*
// Seek to the start of an image resource.
// We don't call this too often right now, so we just do the file read
// each time. If this became a high traffic area, we would probably want to
// read all the headers into memory and search it there.
*/

ERRORCODE PSDHelper::SeekImageResource(StorageDevice* pFile, WORD wSearchID, DWORD& dwDataSize)
{
	ERRORCODE error;

	// Seek to the start of the image resources section.
	ST_DEV_POSITION lPos = m_Record.m_lImageResourcesSection;
	ST_DEV_POSITION lEnd = m_Record.m_lLayerMaskSection-4;

	// Swap the word to look for. This is better than swapping everything else.
	swap_word(&wSearchID);

	while (lPos < lEnd)
	{
		// Seek to the current resource.
		if ((error = pFile->seek(lPos, ST_DEV_SEEK_SET)) != ERRORCODE_None)
		{
			break;
		}
		DWORD dwType;
		WORD wID;
		CString csName;
		if ((error = pFile->read(&dwType, sizeof(dwType))) != ERRORCODE_None
			 || (error = pFile->read(&wID, sizeof(wID))) != ERRORCODE_None
			 || (error = ReadPString(pFile, csName)) != ERRORCODE_None
			 || (error = pFile->read(&dwDataSize, sizeof(dwDataSize))) != ERRORCODE_None)
		{
			// Got an error. Break out now.
			break;
		}
		swap_dword(&dwDataSize);
		// We have the data. See if it's the one we're looking for.
		if (wID == wSearchID)
		{
			// Found it!
			break;
		}
		pFile->tell(&lPos);
		lPos += dwDataSize;
      if (dwDataSize & 1)
      {
         lPos++;
      }
	}
	return (lPos >= lEnd) ? ERRORCODE_DoesNotExist : error;
}

ERRORCODE PSDHelper::ReadPString(StorageDevice* pFile, CString& csString)
{
	ERRORCODE error;
	BYTE bSize;
	// Read the size.
	if ((error = pFile->read(&bSize, sizeof(bSize))) != ERRORCODE_None)
	{
		return error;
	}
	if (bSize == 0)
	{
		csString.Empty();
	}
	else
	{
		// Read the string.
		LPSTR p = csString.GetBuffer(bSize+1);
		error = pFile->read(p, bSize);
		p[bSize] = '\0';
		csString.ReleaseBuffer();
		if (error != ERRORCODE_None)
		{
			return error;
		}
	}

	// Skip any padding byte.
	if ((bSize & 1) == 0)
	{
		pFile->seek(1, ST_DEV_SEEK_CUR);
	}

	return error;
}

//
// Return the start of actual image data (past compression info).
//

ST_DEV_POSITION PSDHelper::ImageStart(void)
{
	ST_DEV_POSITION lStart = m_Record.m_lImageDataSection;
	lStart += 2;			// Skip compression value.
	if (m_Record.m_wCompression)
	{
		// Skip (2-byte) row counts.
		lStart += (long)m_Record.m_wChannels * (long)graphic->record.y_size * 2;
	}
	return lStart;
}

/////////////////////////////////////////////////////////////////////////////
// PSDDriver

/*
// The constructor for the PSD driver.
*/

PSDDriver::PSDDriver(GraphicPtr g, StorageDevice* pSource /*=NULL*/)
{
	// Assign the graphic.
	graphic = g;

	// Setup the file.
	if (pSource == NULL)
	{
		graphic->prep_storage_file(&file);
	}
	else
	{
		file.set_based_device(pSource);
	}

	// No sizes yet.
	m_pSizes = NULL;

	// No line yet.
	m_nCurrentLine = -1;
}

//
// The destructor for a PSD driver.
//

PSDDriver::~PSDDriver()
{
	// Free the row sizes if we allocated them.
	if (m_pSizes != NULL)
	{
		GlobalFreePtr(m_pSizes);
	}

	// Delete all the files we used.
	for (int nFile = 0; nFile < m_ChannelFiles.GetSize(); nFile++)
	{
		delete (ReadOnlyFile*)m_ChannelFiles.GetAt(nFile);
	}
}

/*
// The image_info() method for PSD files.
*/

ERRORCODE PSDDriver::image_info(IMAGE_INFO_PTR info)
{
	PSDHelper* pHelper = (PSDHelper*)graphic->my_little_helper();

	int nType = TypeOfPSDImage(pHelper->m_Record.m_wMode);
	if (nType == -1)
	{
		return ERRORCODE_IllegalType;
	}

   info->planes = (nType == IMAGE_FULL_COLOR) ? 3 : 1;
	info->bpp = (nType == IMAGE_BILEVEL) ? 1 : 8;
	info->image_type = (IMAGE_TYPE)nType;
	info->width = graphic->record.x_size;
	info->height = graphic->record.y_size;

	if (pHelper->m_Record.m_wChannels > info->planes)
	{
		info->m_fHasMask = TRUE;
		info->m_fReverseMask = TRUE;
		info->m_nMaskBpp = info->bpp;
	}

	return ERRORCODE_None;
}

/*
// The open_image() method for PSD files.
*/

ERRORCODE PSDDriver::open_image(PALETTE_INFO_PTR palette)
{
	PSDHelper* pHelper = (PSDHelper*)graphic->my_little_helper();

	// Extract some useful values.
	m_nChannels = pHelper->GetChannels();
	m_nRows = graphic->record.y_size;
	m_nColumns = graphic->record.x_size;
	m_wCompression = pHelper->m_Record.m_wCompression;

	// Compute the number of bytes in a row.
	m_nRowBytes = m_nColumns;
	if (our_info.image_type == IMAGE_BILEVEL)
	{
		m_nRowBytes = (m_nRowBytes + 7)/8;
	}

	ERRORCODE error;

	// Setup the palette.
	if (palette != NULL)
	{
		switch (pHelper->m_Record.m_wMode)
		{
			case 0:								// Bitmap.
			{
				// We need to do this since the bits are opposite what we default to.
				palette->m_pData[0].rgbtBlue =
				   palette->m_pData[0].rgbtGreen =
				   palette->m_pData[0].rgbtRed = 255;

            palette->m_pData[1].rgbtBlue =
				   palette->m_pData[1].rgbtGreen =
				   palette->m_pData[1].rgbtRed = 0;

            palette->m_fRead = TRUE;
				break;
			}
			case 2:								// Indexed.
			{
				ASSERT(palette->m_nColors == 256);

				if (palette->m_nColors == 256)
				{
					// Seek to the color data section to read the palette.
					if ((error = file.seek(pHelper->m_Record.m_lColorDataSection, ST_DEV_SEEK_SET)) != ERRORCODE_None)
					{
						return error;
					}

					// Allocate a block of memory to hold the non-interleaved data.
					LPBYTE pPaletteData = NULL;
					TRY
					{
						pPaletteData = new BYTE[768];
					}
					END_TRY
					if (pPaletteData == NULL)
					{
						return ERRORCODE_Memory;
					}

					// Read the data.
					if ((error = file.read(pPaletteData, 768)) != ERRORCODE_None)
					{
						delete [] pPaletteData;
						return error;
					}

					// Interleave it into the palette.
					LPBYTE pSource = pPaletteData;
					LPBYTE pDest = (LPBYTE)palette->m_pData;
					for (int i = 0; i < 256; i++, pSource++)
					{
						*pDest++ = pSource[0];
						*pDest++ = pSource[256];
						*pDest++ = pSource[512];
					}

					delete [] pPaletteData;
					palette->m_fRead = TRUE;
				}
				break;
			}
			default:
			{
				break;
			}
		}
	}

	// Seek to the start of the data (skip the compression value).
	if ((error = file.seek(pHelper->m_Record.m_lImageDataSection+2, ST_DEV_SEEK_SET)) != ERRORCODE_None)
	{
		return error;
	}

   // Read the compression data if we are compressed.
	if ((error = pHelper->ReadCompressionCounts(&file, m_pSizes)) != ERRORCODE_None)
	{
		return error;
	}

	// Setup the channel files.
	if ((error = SetupChannelFiles()) != ERRORCODE_None)
	{
		return error;
	}

	// All is well!
	return ERRORCODE_None;
}

/*
// The read_lines() method for PSD files.
*/

ERRORCODE PSDDriver::read_lines(int nLine, LPBYTE pImage, int nCount, LPBYTE pMask)
{
	PSDHelper* pHelper = (PSDHelper*)graphic->my_little_helper();

	ERRORCODE error;

	// Seek to the desired line.
	if (nLine != m_nCurrentLine)
	{
		// If the requested line is before our line, we need to go back to the start.
		if (nLine < m_nCurrentLine)
		{
			if ((error = Rewind()) != ERRORCODE_None)
			{
				return error;
			}
		}

		// We need this to be true before proceeding.
		ASSERT(nLine >= m_nCurrentLine);

		// We need to seek.
		if (pHelper->m_Record.m_wCompression)
		{
			// Skipping is harder with compression.
			for (int nChannel = 0; nChannel < m_nChannels; nChannel++)
			{
#ifdef WIN32
				WORD* pCount = m_pSizes;
#else
				WORD huge* pCount = (WORD huge *)m_pSizes;
#endif
				// Move to the right row count.
				pCount += nChannel*m_nRows + m_nCurrentLine;

				// Advance lines until there.
				ST_DEV_POSITION lPosition = m_ChannelPositions.GetAt(nChannel);
				for (int n = m_nCurrentLine; n < nLine; n++)
				{
					lPosition += *pCount++;
				}
				m_ChannelPositions.SetAt(nChannel, lPosition);
			}
		}
		else
		{
			// Skipping is easier without compression.
			for (int nChannel = 0; nChannel < m_nChannels; nChannel++)
			{
				ST_DEV_POSITION lPosition = m_ChannelPositions.GetAt(nChannel);
				lPosition += (nLine-m_nCurrentLine)*m_nRowBytes;
				m_ChannelPositions.SetAt(nChannel, lPosition);
			}
		}
		m_nCurrentLine = nLine;
	}

	// Now read rows until done.
	while (nCount-- > 0)
	{
		int nChannelsToRead = (our_info.image_type == IMAGE_FULL_COLOR) ? 3 : 1;
		ASSERT(nChannelsToRead <= m_nChannels);

		for (int nChannel = 0; nChannel < nChannelsToRead; nChannel++)
		{
			if ((error = ReadAChannel(pImage+nChannel, nChannel, nChannelsToRead)) != ERRORCODE_None)
			{
				return error;
			}
		}

		if (our_info.m_fHasMask)
		{
			if (pMask == NULL)
			{
				ASSERT(FALSE);
				return ERRORCODE_IntError;
			}
			if ((error = ReadAChannel(pMask, nChannelsToRead, 1)) != ERRORCODE_None)
			{
				return error;
			}
			pMask += m_nMaskRowSize;
		}

		m_nCurrentLine++;
		pImage += source_row_size;
	}

	return ERRORCODE_None;
}

static inline ERRORCODE NextByte(ReadOnlyFile* pFile, LPBYTE pByte)
{
	LPBYTE p;
	if ((p = pFile->fast_access(1)) == NULL)
	{
		ERRORCODE error;
		if ((error = pFile->read(pByte, sizeof(BYTE))) != ERRORCODE_None)
		{
			return error;
		}
	}
	else
	{
		*pByte = *p;
	}
	return ERRORCODE_None;
}

//
// Read the data for a channel.
//

ERRORCODE PSDDriver::ReadAChannel(LPBYTE pBuffer, int nChannel, int nChannels)
{
   ERRORCODE error = ERRORCODE_None;

   ASSERT(m_ChannelFiles.GetSize() > nChannel);
	ASSERT(m_ChannelPositions.GetSize() > nChannel);
	ASSERT(m_ChannelLowPositions.GetSize() > nChannel);

   ReadOnlyFile* pFile = (ReadOnlyFile*)m_ChannelFiles.GetAt(nChannel);
	ST_DEV_POSITION lPosition = m_ChannelPositions.GetAt(nChannel);
	ST_DEV_POSITION lLowPosition = m_ChannelLowPositions.GetAt(nChannel);

	// If there is more than one file, we are potentially jumping around.
	// We need to establish the context for this data stream.

	if (lPosition == 0)
	{
		// Special value. Go to start.
		if ((error = pFile->seek(0, ST_DEV_SEEK_SET)) != ERRORCODE_None)
      {
         return error;
      }
	}
	else
   {
		// We need to seek.
	   if (nChannels > 1)
	   {
			// We have multiple channels going. The low-level file will have
			// moved.
			// Seek absolutely in the low-level file to restore what the
			// based file believes to be the current position.
			// Then we can seek in the higher-level file without confusion.
			if ((error = file.seek(lLowPosition, ST_DEV_SEEK_SET)) != ERRORCODE_None)
         {
            return error;
         }
		}

		// Seek to the right position.
		if ((error = pFile->seek(lPosition, ST_DEV_SEEK_SET)) != ERRORCODE_None)
      {
         return error;
      }
   }

	// Now we can read the data.

	int nBytes = m_nRowBytes;

	if (m_wCompression)
	{
		// We inline the PackBits code here since we need to do some special
		// stuff during the read.

		while (nBytes > 0)
		{
			char count;
			BYTE data;

			// Read this byte.
			if ((error = NextByte(pFile, (LPBYTE)&count)) != ERRORCODE_None)
			{
				return error;
			}

			if (count < 0)
			{
			/* Repeat count */
				if ((error = NextByte(pFile, &data)) != ERRORCODE_None)
				{
					return error;
				}
				count = -count;
				while ((count >= 0) && (nBytes > 0))
				{
					*pBuffer = data;
					pBuffer += nChannels;
					nBytes--;
					count--;
				}
			}
			else
			{
			/* Replace count. */
				while ((count >= 0) && (nBytes > 0))
				{
					if ((error = NextByte(pFile, &data)) != ERRORCODE_None)
					{
						return error;
					}
					*pBuffer = data;
					pBuffer += nChannels;
					nBytes--;
					count--;
				}
			}
		}
	}
	else
	{
      BYTE data;
		while (nBytes > 0)
		{
			if ((error = NextByte(pFile, &data)) != ERRORCODE_None)
			{
				return error;
			}
			*pBuffer = data;
			pBuffer += nChannels;
			nBytes--;
		}
	}
	// Remember our positions.
	pFile->tell(&lPosition);
	m_ChannelPositions.SetAt(nChannel, lPosition);

	if (nChannels > 1)
	{
		// Remember the low-level file position also.
		file.tell(&lLowPosition);
		m_ChannelLowPositions.SetAt(nChannel, lLowPosition);
	}

   return ERRORCODE_None;
}

ERRORCODE PSDDriver::Rewind(void)
{
	// We are at the first line.
	m_nCurrentLine = 0;

	// Reset all channel files to the beginning.
	TRY
	{
		for (int nChannel = 0; nChannel < m_nChannels; nChannel++)
		{
			m_ChannelPositions.SetAtGrow(nChannel, 0);
		}
	}
	CATCH_ALL(e)
	{
		return ERRORCODE_Memory;
	}
	END_CATCH_ALL;

	return ERRORCODE_None;
}

ERRORCODE PSDDriver::SetupChannelFiles(void)
{
	PSDHelper* pHelper = (PSDHelper*)graphic->my_little_helper();

	// Compute the start of the first (aka next) channel.
	ST_DEV_POSITION lChannelStart = pHelper->ImageStart();
   ERRORCODE error = ERRORCODE_None;

	// We establish a file for each channel.
	for (int nChannel = 0; nChannel < m_nChannels; nChannel++)
	{
		// Compute the channel end.
		ST_DEV_POSITION lChannelSize = pHelper->m_Record.m_lChannelSizes[nChannel];
		ST_DEV_POSITION lChannelEnd = lChannelStart + lChannelSize;

		// Allocate the file for the channel.
		ReadOnlyFile* pFile = NULL;
		TRY
		{
			pFile = new ReadOnlyFile;
		}
		END_TRY
		if (pFile == NULL)
		{
			return ERRORCODE_Memory;
		}

		// Add this file to the array.
		m_ChannelFiles.Add(pFile);
		m_ChannelPositions.Add(0);		// Start at the beginning.
		m_ChannelLowPositions.Add(lChannelStart);

		// Setup this file based on the master file.
		pFile->set_based_device(&file);
		pFile->set_subfile(lChannelStart, lChannelEnd);

		// Compute the size of the read buffer.
		int nBufferSize = (lChannelSize < 4096) ? lChannelSize : 4096;

		// Set a read buffer.
		pFile->set_read_buffer(nBufferSize);

		// End of this channel is start of the next channel.
		lChannelStart = lChannelEnd;
	}

	return Rewind();
}

