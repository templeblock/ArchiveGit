/*
// GIF.CPP
//
// Routines for reading a GIF file.
//
// $Header: /PM8/App/GIF.CPP 1     3/03/99 6:05p Gbeddow $
//
// $Log: /PM8/App/GIF.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 7     1/25/99 3:44p Rgrenfell
// Added support for Netscape loop tag requesting.
// 
// 6     9/14/98 12:08p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 5     8/20/98 11:02a Jayn
// New resolution overrides. Small license path change.
// 
// 4     4/29/98 10:56a Jayn
// Now supports local color maps.
// 
// 3     1/13/98 11:15a Jayn
// Transparent GIF support.
// 
// 2     11/19/97 3:43p Jayn
// Rotated bitmap support.
// 
//    Rev 1.0   14 Aug 1997 15:21:08   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:36   Fred
// Initial revision.
// 
//    Rev 1.10   02 Jul 1997 14:25:50   Jay
// LZW is disabled in Classic version
// 
//    Rev 1.9   05 May 1997 08:53:44   Jay
// Added DLL. Better error handling.
// 
//    Rev 1.8   21 Apr 1997 09:29:22   Jay
// 16-bit changes for huge DIBs
// 
//    Rev 1.7   15 Apr 1997 15:44:44   Jay
// Card Server phase #1
// 
//    Rev 1.6   11 Apr 1997 11:23:28   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.5   05 Dec 1996 12:30:08   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.4   07 Oct 1996 13:22:02   Jay
// 
//    Rev 1.2   25 Aug 1996 12:59:22   Jay
// No longer sets default name at low level.
// 
//    Rev 1.1   15 May 1996 14:26:54   Jay
// Now supports interlaced GIF files.
// 
//    Rev 1.0   14 Mar 1996 13:47:50   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:38   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:47:48   JAY
// Initial revision.
// 
//    Rev 1.5   09 Jan 1995 16:26:30   JAY
// New stuff (cut & paste, etc).
// 
//    Rev 1.4   30 Nov 1994 16:37:04   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.3   04 Aug 1994 07:33:50   JAY
// Large model and 32-bit changes
// 
//    Rev 1.2   23 May 1994 08:22:20   JAY
// Now uses prep_storage_file() instead of the direct init so we can base the
// file on the document database device.
// 
//    Rev 1.1   05 Jan 1994 08:06:34   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:14   JAY
// 
*/

#include "stdafx.h"

#include <stdlib.h>
#include <string.h>

#include "grafrec.h"
#include "lzw.h"
#include "gif.h"
#include "imagebuf.h"
#include "pmwcfg.h"

/////////////////////////////////////////////////////////////////////////////
// GIFHelper

POINT GIFHelper::m_pntResolution = { 200, 200 };

GIFHelper::GIFHelper(GraphicPtr graphic): GraphicHelper(graphic)
{
	memset(&record, 0, sizeof(record));
}

/*
// Read the GIF helper info.
*/

ERRORCODE GIFHelper::read(StorageDevicePtr device)
{
	return device->read_record(&record, sizeof(record));
}

/*
// Write the GIF helper info.
*/

ERRORCODE GIFHelper::write(StorageDevicePtr device)
{
	return device->write_record(&record, sizeof(record));
}

/*
// Size the GIF helper info.
*/

ST_DEV_IO_SIZE GIFHelper::size(StorageDevicePtr device)
{
	return device->size_record(sizeof(record));
}

/*
// Read header information from a GIF file.
*/

ERRORCODE GIFHelper::ReadGifInfo(StorageDevicePtr file, GifHeaderInfo& Info)
{
	ERRORCODE error;
	GIF_HEADER header;

	if ((error = file->read(&header, sizeof(header))) == ERRORCODE_None)
	{
	/*
	// Do minimal GIF header verification.
	// I didn't know if it would be good to verify the whole string since
	// the '87a' part seems to be a version string, and if it ever changed
	// we would be in trouble. Of course, on the other hand, if the version did
	// change substantially, the file formats may be incompatible. Oh, well.
	//
	// Flash! There is an '89a' spec. and we read it fine. Ignore the version!
	*/

		if (header.header_id[0] != 'G'
				|| header.header_id[1] != 'I'
				|| header.header_id[2] != 'F')
		{
			return ERRORCODE_IllegalType;
		}

		// Well, we WOULD like to know if a file isn't one of the standard so we
		// can test for actual support...
		ASSERT( ((header.header_id[3] == '8') && (header.header_id[4] == '7') && (header.header_id[5] == 'a')) ||
				  ((header.header_id[3] == '8') && (header.header_id[4] == '9') && (header.header_id[5] == 'a')) );

		Info.nWidth = header.screen_width;
		Info.nHeight = header.screen_height;

		Info.nXRes = m_pntResolution.x;
		Info.nYRes = m_pntResolution.y;

		Info.bBPP = (header.flags & 0x07) + 1;
		Info.fHasColorMap = (header.flags & 0x80) != 0;

		if (Info.fHasColorMap)
		{
			SHORT real_colors = 1 << Info.bBPP;

			file->seek(real_colors*sizeof(RGBTRIPLE), ST_DEV_SEEK_CUR);
		}

		Info.nTransparentIndex = -1;		// Not until we read it!

		if (!find_image_block(file, &Info))
		{
			return ERRORCODE_Read;
		}

		IMAGE_HEADER image_header;

		if ((error = file->read(&image_header, sizeof(image_header))) == ERRORCODE_None)
		{
			Info.fInterlaced = (image_header.flags & 0x40) != 0;
		}
	}

	return error;
}

/*
// Skip to the next GIF block.
// A GIF block starts with:
// ','	Image block.
// '!'	Expansion block.
// ';'	Terminator.
// Returns the block type or 0 if error.
*/

BYTE GIFHelper::next_gif_block(StorageDevice* pFile)
{
	BYTE ch;

	while (pFile->read(&ch, sizeof(ch)) == ERRORCODE_None)
	{
		if (ch == ',' || ch == ';' || ch == '!')
		{
			return ch;
		}
	}
	return 0;
}

/*
// Find the image block.
*/

BOOL GIFHelper::find_image_block(StorageDevice* pFile, GifHeaderInfo* pInfo /*=NULL*/)
{
	BYTE ch;

	for (;;)
	{
		ch = next_gif_block(pFile);

		if (ch == '!')
		{
		/* Read code type. */

			pFile->read(&ch, sizeof(ch));
			TRACE("GIF Extension block: %x\n", ch);

			// Read the length.
			BYTE len;
			if (pFile->read(&len, sizeof(len)) != ERRORCODE_None)
			{
				len = 0;
			}

			// Remember the position here.
			ST_DEV_POSITION Here;
			pFile->tell(&Here);

			// See what type of block this is.
			switch (ch)
			{
				case 0xf9:
				{
					// Graphic control extension.

					if (pInfo != NULL)
					{
						// Read the block.
						GIF_CONTROL_EXTENSION Block;

						ASSERT(sizeof(Block) == 4);
						pFile->read(&Block, sizeof(Block));

						if (Block.bFlags & 1)
						{
							pInfo->nTransparentIndex = Block.bTransparentIndex;
						}
						else
						{
							pInfo->nTransparentIndex = -1;
						}

						pInfo->wDelayTime = Block.wDelayTime;
					}

					break;
				}
				case 0xff:
				{
					// Application extension label
					if (pInfo != NULL)
					{
						APPLICATION_EXTENSION_BLOCK blockApp;

						ASSERT(sizeof(blockApp) == 11);
						pFile->read(&blockApp, sizeof(blockApp));

						if ((strncmp( blockApp.bAsciiCode, "NETSCAPE", 8 ) == 0) &&
							 (strncmp( (CHAR *)blockApp.bBinaryCode, "2.0", 3 ) == 0))
						{
							// Netscape extension block.
							
							// Read the sub-block for looping data.
							BYTE nSubLen;
							if (pFile->read(&nSubLen, sizeof(nSubLen)) != ERRORCODE_None)
							{
								nSubLen = 0;
							}

							// Verify that the length is correct...
							if (nSubLen == 3)
							{
								NETSCAPE_LOOP_BLOCK blockLoop;
								ASSERT(sizeof(blockLoop) == 3);
								pFile->read(&blockLoop, sizeof(blockLoop));

								// Verify that the sub-block is the type we want.
								if (blockLoop.bBlockCode == 0x01)
								{
									pInfo->wLoopCount = blockLoop.wLoopCount;
									TRACE( "Animation loops %d times.\n", blockLoop.wLoopCount );
								}
							}
						}
					}

					break;
				}
				default:
				{
					break;
				}
			}

			// Skip past expansion block.
			while (len != 0)
			{
				Here += (ST_DEV_POSITION)(UCHAR)len;
				pFile->seek(Here, ST_DEV_SEEK_SET);
				if (pFile->read(&len, sizeof(len)) != ERRORCODE_None)
				{
					break;
				}
				else
				{
					Here++;	// Compensate for having read the length byte.
				}
			}
		}

		else if (ch == ',')
		{
		/* Found image block! */
			return TRUE;
		}

		else if (ch == ';' || ch == 0)
		{
		/* End of file. */
			return FALSE;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// GIFFile

/*
// Constructor for a GIF file.
*/

GIFFile::GIFFile(LPSTR name)
			: ReadOnlyFile(name),
 			  reading_image(FALSE),
			  gif_count(0)
{
}

ERRORCODE GIFFile::read(VOIDPTR ptr, ST_DEV_IO_SIZE size, ST_DEV_IO_SIZE *size_read)
{
/* Handle image reading differently. */

	if (reading_image)
	{
		ST_DEV_IO_SIZE total_read = 0;
		LPBYTE p = (LPBYTE)ptr;
		ERRORCODE error = ERRORCODE_None;

		if (gif_count == -1)
		{
			return ERRORCODE_Read;
		}

	/* Read as much as we want. */

		while (size != 0)
		{
			if (gif_count == 0)
			{
				BYTE cnt;

			/* Read the count for the next block. */

				if ((error = ReadOnlyFile::read(&cnt, sizeof(cnt))) != ERRORCODE_None)
				{
					if (size_read != NULL && error == ERRORCODE_Read)
					{
						error = ERRORCODE_None;
					}
					gif_count = -1;
					break;
				}

				gif_count = cnt;

				if (gif_count == 0)
				{
					gif_count = -1;
					break;
				}
			}

		/* Handle as much data as we can and want. */

			SHORT copy_count = __min((SHORT)size, gif_count);
			ST_DEV_IO_SIZE this_read;

			if ((error = ReadOnlyFile::read(p, copy_count, &this_read)) != ERRORCODE_None)
			{
				break;
			}
                                             
		/* See if we read all. */

			if (this_read == 0)
			{
				if (size_read == NULL)
				{
				/* Didn't read enough. */
					error = ERRORCODE_Read;
				}
				break;
			}

		/* Update our stuff. */

			total_read += this_read;
			p += copy_count;
			gif_count -= copy_count;
			size -= copy_count;
		}

		if (size_read != NULL)
		{
			*size_read = total_read;
		}
		return error;
	}
	else
	{
		return ReadOnlyFile::read(ptr, size, size_read);
	}
}

ERRORCODE GIFFile::seek(ST_DEV_POSITION position, ST_DEV_SEEK_MODE mode)
{
/* Cancel our read. Seek means we're doing something special. */
	if (reading_image)
	{
		gif_count = 0;
	}
	return ReadOnlyFile::seek(position, mode);
}

/*
// Initialize a GIF file.
*/

ERRORCODE GIFHelper::init(GRAPHIC_CREATE_STRUCT_PTR gcs)
{
	// GIF is LZW-compressed. See if we support it.
	if (!GetConfiguration()->SupportsLZW(FALSE))
	{
		return ERRORCODE_LZWNotSupported;
	}

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

	GifHeaderInfo Info;
	if ((error = ReadGifInfo(pSource, Info)) == ERRORCODE_None)
	{
		graphic->record.x_size = Info.nWidth;
		graphic->record.y_size = Info.nHeight;
		graphic->record.x_resolution = Info.nXRes;
		graphic->record.y_resolution = Info.nYRes;
		graphic->record.storage = GRAPHIC_STORAGE_FILE;

		record.m_bBPP = Info.bBPP;
		record.m_fHasColorMap = Info.fHasColorMap;
		record.m_fInterlaced = Info.fInterlaced;

		// We have both a boolean variable to control transparency as well
		// as a transparent color. This is redundant but it solves a problem:
		// record members will be zero if loading an old format. We want such
		// a case to indicate no transparency, and the boolean solves this.
		record.m_fHasTransparency = (Info.nTransparentIndex != -1);
		record.m_nTransparentIndex = Info.nTransparentIndex;
	}
	return error;
}

/*
// The constructor for the GIF driver.
*/

GIFDriver::GIFDriver(GraphicPtr g, StorageDevice* pSource /*=NULL*/)
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

	lzw = NULL;

	m_fDeInterlacing = FALSE;
	m_pBuffer = NULL;
	m_nTransparentIndex = -1;
}

/*
// The destructor for the GIF driver.
*/

GIFDriver::~GIFDriver()
{
	delete lzw;
	delete m_pBuffer;
}

/*
// The image_info() method for GIF files.
*/

ERRORCODE GIFDriver::image_info(IMAGE_INFO_PTR info)
{
	GIFHelper* pHelper = (GIFHelper*)graphic->my_little_helper();

	info->bpp = 8;
	info->planes = 1;
	info->image_type = IMAGE_PALETTE;
	info->width = graphic->record.x_size;
	info->height = graphic->record.y_size;

	info->m_fHasMask = pHelper->record.m_fHasTransparency;
	if (info->m_fHasMask)
	{
		info->m_fHasMask = TRUE;
		info->m_nMaskBpp = info->bpp;
	}

	return ERRORCODE_None;
}

/*
// Pass information.
// Pass 0:		0,  8, 16, 24, ... (mod = 0)
// Pass 1:		4, 12, 20, 28, ... (mod = 4)
// Pass 2:		2,  6, 10, 14, ... (mod = 2, 6)
// Pass 3:		1,  3,  5,  7, ... (mod = 1, 3, 5, 7)
*/

static struct
{
	int m_nStartOffset;			// starting offset
	int m_nRowShift;				// row skip (shift value)
	int m_nRowsPerBlockShift;	// How many rows per 8-bit block (shift value)
} PassInfo[] =
{
	{ 0, 3, 0 },
	{ 4, 3, 0 },
	{ 2, 2, 1 },
	{ 1, 1, 2 },
};

/*
// The open_image() method for GIF files.
*/

ERRORCODE GIFDriver::open_image(PALETTE_INFO_PTR palette)
{
	// GIF is LZW-compressed. See if we support it.
	if (!GetConfiguration()->SupportsLZW(FALSE))
	{
		return ERRORCODE_LZWNotSupported;
	}

	ERRORCODE error;

	if ((error = file.seek(sizeof(GIF_HEADER), ST_DEV_SEEK_SET)) != ERRORCODE_None)
	{
		return error;
	}

	GIFHelperPtr helper = (GIFHelperPtr)graphic->my_little_helper();
	SHORT real_colors = 1 << helper->record.m_bBPP;

	if (helper->record.m_fHasColorMap)
	{
		file.read(palette->m_pData, real_colors*sizeof(RGBTRIPLE));
		palette->m_fRead = TRUE;
	}

/* Seek to the beginning of the image. */

	current_line = 0;

/* Initialize buffered reads. */

	file.set_read_buffer(2048);

	if (!helper->find_image_block(&file))
	{
		return ERRORCODE_InvalidRecord;
	}

/* Found the image block. */

	IMAGE_HEADER image_header;

	file.read(&image_header, sizeof(image_header));
	if (image_header.flags & 0x80)
	{
		// We have a local color map. Read it.
		SHORT real_colors = 1 << ((image_header.flags & 7) + 1);

		file.read(palette->m_pData, real_colors*sizeof(RGBTRIPLE));
		palette->m_fRead = TRUE;
	}

	file.read(&bits, sizeof(bits));

	file.tell(&image_start);

	file.set_reading_image(TRUE);

/* Create the LZW parser. */

	if ((lzw = new LZWParser(&file, (SHORT)bits, TRUE)) == NULL
			|| !lzw->valid())
	{
		return ERRORCODE_Memory;
	}

	bytes = graphic->record.x_size;

/*
// If the image is interlaced, we need to set up for de-interlacing.
*/

	if (helper->record.m_fInterlaced)
	{
		m_pBuffer = new ImageBuffer(this);
		if ((error = m_pBuffer->init()) != ERRORCODE_None)
		{
			return error;
		}

		// Compute the pass starts.
		int nRows = graphic->record.y_size;

		m_nPassStart[0] = 0;
		m_nPassStart[1] = m_nPassStart[0] + PassSize(nRows, 0);
		m_nPassStart[2] = m_nPassStart[1] + PassSize(nRows, 1);
		m_nPassStart[3] = m_nPassStart[2] + PassSize(nRows, 2);

		m_fDeInterlacing = TRUE;
	}

	if (helper->record.m_fHasTransparency)
	{
		m_nTransparentIndex = helper->record.m_nTransparentIndex;
	}

	return ERRORCODE_None;
}

/*
// Compute the size of a pass.
*/

int GIFDriver::PassSize(int nRows, int nPass)
{
	// Indexed by pass and left over rows (beyond full blocks of 8).
	static int nLeftOverValues[4][8] =
	{
		// Pass 0.
		{ 0, 1, 1, 1, 1, 1, 1, 1 },
		// Pass 1.
		{ 0, 0, 0, 0, 0, 1, 1, 1 },
		// Pass 2.
		{ 0, 0, 0, 1, 1, 1, 1, 2 },
		// Pass 3.
		{ 0, 0, 1, 1, 2, 2, 3, 3 },
	};

	int nBlocks = nRows >> 3;				// Block size is 8
	int nLeftOver = nRows & 7;				// Incomplete block size

	// Compute rows in full blocks.
	return
			(nBlocks << PassInfo[nPass].m_nRowsPerBlockShift)	// Full blocks
			+ nLeftOverValues[nPass][nLeftOver];				// Rows in leftover
}

/*
// Compute the offset of a line in a pass.
*/

int GIFDriver::InterlacedLineOffset(int nLine, int nPass)
{
	return (nLine - PassInfo[nPass].m_nStartOffset) >> PassInfo[nPass].m_nRowShift;
}

int GIFDriver::InterlacedLine(int nLine)
{
	// Compute which pass this line is in. We use "mod 8" (AND with 7) to
	// determine this.
	static nPassMap[] = { 0, 3, 2, 3, 1, 3, 2, 3 };
	int nPass = nPassMap[nLine & 7];

	return m_nPassStart[nPass] + InterlacedLineOffset(nLine, nPass);
}

/*
// The read_lines() method for GIF files.
*/

ERRORCODE GIFDriver::read_lines(int nLine, LPBYTE pData, int nCount, LPBYTE pMask)
{
	ERRORCODE error = ERRORCODE_None;

	if (m_fDeInterlacing)
	{
		// Do special stuff for interlaced image.

		m_fDeInterlacing = FALSE;		// Turn off whilst reading.

		RECT r;

		r.left = 0;
		r.right = graphic->record.x_size;

		for (int nLines = nCount; nLines != 0; nLines--)
		{
		// Compute the interlaced line to read.
			int nThisLine = InterlacedLine(nLine);

			// The buffer needs to read lines consecutively in order to
			// cache them. If the line we want to read is beyond what has
			// been cached, read lines up to that point so they will all be
			// cached. We do not bother reading the line itself in this loop
			// since we will always just read it after the loop.

			while (nThisLine > current_line)
			{
				r.top = r.bottom = current_line;
				if ((error = m_pBuffer->GetSourceLine(current_line, r, pData, pMask)) != ERRORCODE_None)
				{
					break;
				}
			}

			if (error == ERRORCODE_None)
			{
				// Read the line we want from the cache.
				r.top = r.bottom = nThisLine;
				error = m_pBuffer->GetSourceLine(nThisLine, r, pData, pMask);
			}

			if (error != ERRORCODE_None)
			{
				break;
			}

			nLine++;
			pData += bytes;
			pMask += bytes;
		}

		m_fDeInterlacing = TRUE;		// Restore setting.
	}
	else
	{
		// Image is not interlaced. Read it straight.

		if (nLine < current_line)
		{
		/*
		// Reset back to beginning of image.
		// Slow! Ugh!
		*/
			lzw->reset();
			file.seek(image_start, ST_DEV_SEEK_SET);
			current_line = 0;
		}

		// Seek and read.

		if ((error = lzw->read((ULONG)(nLine-current_line)*bytes,
										pData,
										nCount*bytes)) == ERRORCODE_None)
		{
			// If we are returning a mask, generate the mask now.
			if (pMask != NULL)
			{
				int nBytes = nCount*bytes;
				for (int n = 0; n < nBytes; n++)
				{
					pMask[n] = (pData[n] == m_nTransparentIndex) ? 0xff : 0x00;
				}
			}

			// Bump the current line number.
			current_line = nLine + nCount;
		}
	}

	return error;
}

/*
// Close the image file.
*/

VOID GIFDriver::close_image(VOID)
{
	delete lzw;
	lzw = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CGIFWriter

// We write GIF87a files.
static char BASED_CODE cbGIFSignature[] = {'G', 'I', 'F', '8', '9', 'a'};

CGIFWriter::CGIFWriter()
{
}

CGIFWriter::~CGIFWriter()
{
}

void CGIFWriter::CreateGIF(CGIFDataSource* pSource,/* ptr to caller's pixel reader */
									CGIFDataSink* pSink,		/* ptr to caller's data writer */
									short nWidth,				/* width of image in pixels */
									short nHeight,				/* height of image in pixels */
									short nColorResolution,	/* bits of color resolution (1..8) */
									BOOL fInterlaced,			/* interlaced flag */
									short nBitsPerPixel,		/* bits per pixel (1..8) */
									BOOL fNormalize,			/* normalize the color map */
									RGBQUAD* pColorMap,		/* ptr to color map */
									int nTransparentIndex /*=-1*/)
{
	CGIFCompressor Compressor;
	TRY
	{
		// Establish our data sink.
		m_pSink = pSink;

		// Write the GIF header.
		WriteHeader(nWidth, nHeight, nColorResolution, 0, nBitsPerPixel, fNormalize, pColorMap, nTransparentIndex);

		// Write the graphic control extension block.
		if (nTransparentIndex != -1)
		{
			WriteGraphicControlExtension(nTransparentIndex);
		}

		// Write the image description.
		WriteImageDescription(0,
									 0,
									 nWidth,
									 nHeight,
									 0,
									 fInterlaced,
									 0,
									 0,
									 NULL,
									 nTransparentIndex);

		// Compress the data.
		Compressor.CompressData(nBitsPerPixel, pSource, pSink);

		// Write the terminator.
		WriteTerminator();
	}
	CATCH_ALL(e)
	{
		Compressor.FreeAllData();
		THROW_LAST();
	}
	END_CATCH_ALL
}

/*
// Write the header and screen description (followed by the color map).
*/

void CGIFWriter::WriteHeader(short nWidth,
									  short nHeight,
									  short nColorResolution,
									  short nFillColor,
									  short nBitsPerPixel,
									  BOOL fNormalize,
									  RGBQUAD* pColorMap,
									  int nTransparentIndex /*=-1*/)
{
	ASSERT(m_pSink != NULL);

	// Construct the header to write.
	GIF_HEADER Header;

	ASSERT(sizeof(Header.header_id) == sizeof(cbGIFSignature));
	memcpy(Header.header_id, cbGIFSignature, sizeof(Header.header_id));

	Header.screen_width = nWidth;
	Header.screen_height = nHeight;

	if (pColorMap == NULL)
	{
		Header.flags = ((nColorResolution - 1) & 0x07) << 4;
	}
	else
	{
		Header.flags = 0x80
								| ((nColorResolution - 1) << 4)
								| ((nBitsPerPixel - 1) & 0x0F);
	}

	Header.background_color = (BYTE)nFillColor;
	Header.zero = 0;			/* reserved */

	// Write the header data.
	WriteData((LPBYTE)&Header, sizeof(Header));

	// Write the color map if we have one.
	if (pColorMap != NULL)
	{
		WriteColorMap(nColorResolution, nBitsPerPixel, fNormalize, pColorMap, nTransparentIndex);
	}
}

/*
// Write the graphic control extension block.
*/

void CGIFWriter::WriteGraphicControlExtension(int nTransparentIndex)
{
	// Write an extension block start ('!').
	WriteExtensionBlock();

	// Write the extension type and size.
	BYTE bData[2];
	bData[0] = 0xf9;			// Graphic control extension type.
	bData[1] = sizeof(GIF_CONTROL_EXTENSION);
	WriteData(bData, 2);

	// Write the control extension data.
	GIF_CONTROL_EXTENSION Block;
	Block.bFlags = 0;
	Block.wDelayTime = 0;
	Block.bTransparentIndex = 0;

	if (nTransparentIndex != -1)
	{
		Block.bFlags |= 1;			// Transparent color
		Block.bTransparentIndex = nTransparentIndex;
	}
	WriteData((LPBYTE)&Block, sizeof(Block));

	// Write the zero terminator.
	bData[0] = 0;
	WriteData(bData, 1);
}

/*
// Write the color map.
*/

void CGIFWriter::WriteColorMap(short nColorResolution,
										 short nBitsPerPixel,
										 BOOL fNormalize,
										 RGBQUAD* pColorMap,
										 int nTransparentIndex /*=-1*/)
{
	ASSERT(m_pSink != NULL);
	ASSERT(pColorMap != NULL);

	short nNumColors = 1 << nBitsPerPixel;
	short n = (1 << nColorResolution) - 1;

	for (short i = 0; i < nNumColors; i++)
	{
		BYTE bColor[3];
		if (i == nTransparentIndex)
		{
			// Transparent color should be white.
			bColor[0] = \
			bColor[1] = \
			bColor[2] = 255;
		}
		else
		{
			bColor[0] = pColorMap[i].rgbRed;
			bColor[1] = pColorMap[i].rgbGreen;
			bColor[2] = pColorMap[i].rgbBlue;

			if (fNormalize)
			{
				bColor[0] = (BYTE)(((short)bColor[0])*255/n);
				bColor[1] = (BYTE)(((short)bColor[1])*255/n);
				bColor[2] = (BYTE)(((short)bColor[2])*255/n);
			}
		}
		WriteData(bColor, 3);
	}
}

void CGIFWriter::WriteImageDescription(short nLeftEdge,
													short nTopEdge,
													short nWidth,
													short nHeight,
													short nColorResolution,
													BOOL fInterlaced,
													int nBitsPerPixel,
													BOOL fNormalize,
													RGBQUAD* pColorMap,
													int nTransparentIndex /*=-1*/)
{
	// Write a separator (',').
	WriteSeparator();

	// Construct the header to write.
	IMAGE_HEADER Header;

	Header.left = nLeftEdge;
	Header.top = nTopEdge;
	Header.width = nWidth;
	Header.height = nHeight;

	BYTE bInterlaced = fInterlaced ? (1<<6) : 0;
	if (pColorMap == NULL)
	{
		Header.flags = bInterlaced;
	}
	else
	{
		Header.flags = 0x80 | bInterlaced | ((nBitsPerPixel - 1) & 0x0F);
	}

	// Write the image description.
	WriteData((LPBYTE)&Header, sizeof(Header));

	// Write the color map if we have one.
	if (pColorMap != NULL)
	{
		WriteColorMap(nColorResolution, nBitsPerPixel, fNormalize, pColorMap, nTransparentIndex);
	}
}

void CGIFWriter::WriteSeparator(void)
{
	BYTE b = ',';
	WriteData(&b, 1);
}

void CGIFWriter::WriteTerminator(void)
{
	BYTE b = ';';
	WriteData(&b, 1);
}

void CGIFWriter::WriteExtensionBlock(void)
{
	BYTE b = '!';
	WriteData(&b, 1);
}

/////////////////////////////////////////////////////////////////////////////
// CGIFCompressor

static const int nLargestCode = 4095;
static const int nTableSize = 5003;

CGIFCompressor::CGIFCompressor()
{
	m_pCodeTable = NULL;
}

CGIFCompressor::~CGIFCompressor()
{
	FreeAllData();
}

// Cleanup for exceptions, etc.
void CGIFCompressor::FreeAllData(void)
{
	FreeTable();
}

void CGIFCompressor::InitTable(short nMinCodeSize)
{
	m_nCodeSize = nMinCodeSize + 1;
	m_nClearCode = 1 << nMinCodeSize;
	m_nEofCode = m_nClearCode + 1;
	m_nFreeCode = m_nClearCode + 2;
	m_nMaxCode = 1 << m_nCodeSize;

	for (short i = 0; i < nTableSize; i++)
	{
		m_pCodeTable[i].m_nCodeId = 0;
	}
}

void CGIFCompressor::Flush(short n)
{
	BYTE b = (BYTE)n;
	WriteData(&b, 1);					// Write the count.
	if (n > 0)
	{
		WriteData(m_CodeBuffer, n);		// Write the data.
	}
}

void CGIFCompressor::WriteCode(short nCode)
{
	m_nByteOffset = m_nBitOffset >> 3;
	m_nBitsLeft = m_nBitOffset & 7;

	if (m_nByteOffset >= 254)
	{
		Flush(m_nByteOffset);
		m_CodeBuffer[0] = m_CodeBuffer[m_nByteOffset];
		m_nBitOffset = m_nBitsLeft;
		m_nByteOffset = 0;
	}

	if (m_nBitsLeft > 0)
	{
		long lTemp = ((long) nCode << m_nBitsLeft) | m_CodeBuffer[m_nByteOffset];
		m_CodeBuffer[m_nByteOffset] = (BYTE)lTemp;
		m_CodeBuffer[m_nByteOffset + 1] = (BYTE)(lTemp >> 8);
		m_CodeBuffer[m_nByteOffset + 2] = (BYTE)(lTemp >> 16);
	}
	else
	{
		m_CodeBuffer[m_nByteOffset] = (BYTE)nCode;
		m_CodeBuffer[m_nByteOffset + 1] = (BYTE)(nCode >> 8);
	}

	m_nBitOffset += m_nCodeSize;
}

void CGIFCompressor::CompressData(short nMinCodeSize, CGIFDataSource* pSource, CGIFDataSink* pSink)
/*
 * Function:
 *	Compress a stream of data bytes using the LZW algorithm.
 *
 * Inputs:
 *	nMinCodeSize
 *		the field size of an input value.  Should be in the range from
 *		1 to 9.
 */
{
	m_pSink = pSink;

	if (nMinCodeSize < 2 || nMinCodeSize > 9)
	{
		if (nMinCodeSize == 1)
		{
			nMinCodeSize = 2;
		}
		else
		{
			ASSERT(FALSE);
			return;
		}
	}

	// Allocate the table.
	m_pCodeTable = new CCodeEntry[nTableSize];

	BYTE b = (BYTE)nMinCodeSize;
	WriteData(&b, 1);					// record the minimum code size */
	m_nBitOffset = 0;
	InitTable(nMinCodeSize);
	WriteCode(m_nClearCode);
	m_nSuffixChar = pSource->ReadByte();

	short nHX, nD;			// I wish I knew what these two variables mean.

	if (m_nSuffixChar >= 0)
	{
		m_nPrefixCode = m_nSuffixChar;

		while ((m_nSuffixChar = pSource->ReadByte()) >= 0)
		{
			nHX = (m_nPrefixCode ^ m_nSuffixChar << 5) % nTableSize;
			nD = 1;

			for (;;)
			{
				if (m_pCodeTable[nHX].m_nCodeId == 0)
				{
					WriteCode(m_nPrefixCode);

					nD = m_nFreeCode;

					if (m_nFreeCode <= nLargestCode)
					{
						m_pCodeTable[nHX].m_nPriorCode = m_nPrefixCode;
						m_pCodeTable[nHX].m_bAddedChar = (BYTE)m_nSuffixChar;
						m_pCodeTable[nHX].m_nCodeId = m_nFreeCode;
						m_nFreeCode++;
					}

					if (nD == m_nMaxCode)
					{
						if (m_nCodeSize < 12)
						{
							m_nCodeSize++;
							m_nMaxCode <<= 1;
						}
						else
						{
							WriteCode(m_nClearCode);
							InitTable(nMinCodeSize);
						}
					}

					m_nPrefixCode = m_nSuffixChar;
					break;
				}

				if (m_pCodeTable[nHX].m_nPriorCode == m_nPrefixCode &&
					 m_pCodeTable[nHX].m_bAddedChar == (BYTE)m_nSuffixChar)
				{
					m_nPrefixCode = m_pCodeTable[nHX].m_nCodeId;
					break;
				}

				nHX += nD;
				nD += 2;
				if (nHX >= nTableSize)
				{
					nHX -= nTableSize;
				}
			}
		}

		ASSERT(m_nSuffixChar == -1);

		WriteCode(m_nPrefixCode);
	}
	else
	{
		ASSERT(m_nSuffixChar == -1);
	}

	WriteCode(m_nEofCode);

	// Make sure the code buffer is flushed.
	if (m_nBitOffset > 0)
	{
		Flush((m_nBitOffset + 7)/8);
	}

	Flush(0);				/* end-of-data */
	FreeTable();
}

void CGIFCompressor::FreeTable(void)
{
	delete [] m_pCodeTable;
	m_pCodeTable = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CGIFDataSourceLines

CGIFDataSourceLines::CGIFDataSourceLines(int nWidth, int nHeight, BOOL fInterlaced /*=FALSE*/)
{
	// Interlace code needs to be written!
	ASSERT(!fInterlaced);

	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_fInterlaced = fInterlaced;
	m_nLine = -1;
	m_nByteInLine = 0;
	m_pLine = NULL;
	TRY
		m_pLine = new BYTE[m_nWidth];
	END_TRY
}

CGIFDataSourceLines::~CGIFDataSourceLines()
{
	delete [] m_pLine;
}

int CGIFDataSourceLines::ReadByte(void)
{
	if (m_nLine == -1 || m_nByteInLine == m_nWidth)
	{
		// Advance to the next line.
		if (m_nLine == m_nHeight-1)
		{
			// Leave all variables alone in case of repeated calls.
			return -1;
		}
		// Move to next line.
		m_nLine++;
		// Start at first byte in the line.
		m_nByteInLine = 0;

		// Read the line from the derived class.
		int nLine = m_nLine;
		// If we are interlaced, compute the line to use.
		if (m_fInterlaced)
		{
			// Compute the interlaced line.
		}
		// Do the actual read.
		ReadLine(nLine, m_pLine);
	}

	// Return the next byte.
	ASSERT(m_nByteInLine < m_nWidth);
	return m_pLine[m_nByteInLine++];
}

/////////////////////////////////////////////////////////////////////////////
// CGIFDataSourceDIB

CGIFDataSourceDIB::CGIFDataSourceDIB(int nWidth, int nHeight, LPBYTE pData, BOOL fInterlaced /*=FALSE*/)
	: CGIFDataSourceLines(nWidth, nHeight, fInterlaced)
{
	m_pData = pData;
	m_nRowBytes = (nWidth + 3) & ~3;
}

void CGIFDataSourceDIB::FreeAll(void)
{
	// Cleanup for exception.
	// Nothing to do.
}

void CGIFDataSourceDIB::ReadLine(int nLine, LPBYTE pBuffer)
{
	// Flip the line.
	nLine = m_nHeight-1 - nLine;

	// Copy the data.
#ifdef WIN32
	LPBYTE pSource = m_pData + (m_nRowBytes*nLine);
	memcpy(pBuffer, pSource, m_nWidth);
#else
	BYTE huge* pSource = ((BYTE huge*)m_pData) + (DWORD)m_nRowBytes*(DWORD)nLine;
	hmemcpy(pBuffer, pSource, m_nWidth);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// CGIFDataSinkDevice

CGIFDataSinkDevice::CGIFDataSinkDevice(StorageDevice* pDevice)
{
	// Save a pointer to our device.
	m_pDevice = pDevice;
}

void CGIFDataSinkDevice::WriteData(LPBYTE pData, int nCount)
{
	// Write data.
	ERRORCODE error = m_pDevice->write(pData, nCount);

	// Handle errors.
	if (error != ERRORCODE_None)
	{
		ThrowErrorcodeException(error);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDIBtoGIFWriter
/*
// GIF writer that takes a DIB as source.
*/

CDIBtoGIFWriter::CDIBtoGIFWriter()
{
}

/*
// Create the GIF from a DIB.
*/

void CDIBtoGIFWriter::CreateGIF(BITMAPINFO* pbi, LPBYTE pData, CGIFDataSink* pSink, BOOL fInterlaced /*=FALSE*/, int nTransparentIndex /*=-1*/)
{
	short nWidth = (short)pbi->bmiHeader.biWidth;
	short nHeight = (short)pbi->bmiHeader.biHeight;

	// Setup the source.
	CGIFDataSourceDIB	Source(nWidth, nHeight, pData, fInterlaced);

	TRY
	{
		// Create the GIF file.
		CGIFWriter::CreateGIF(&Source,
									 pSink,
									 nWidth,
									 nHeight,
									 8,
									 fInterlaced,
									 8,
									 FALSE,
									 pbi->bmiColors,
									 nTransparentIndex);
	}
	CATCH_ALL(e)
	{
		Source.FreeAll();
		THROW_LAST();
	}
	END_CATCH_ALL
}
