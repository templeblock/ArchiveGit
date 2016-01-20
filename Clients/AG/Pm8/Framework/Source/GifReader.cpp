/*	@doc
	@module GifReader.cpp - Implementation of the RGifReader class |

			Implementation of the <c RGifReader> class.

			This class provides an interface to a GIF file.  At this time it is
			purely a wrapper for reading the transparent index from a memory
			buffer.
	
	<cp>Copyright 1997 Brøderbund Software, Inc., all rights reserved
*/

#include "FrameworkIncludes.h"
#include "GifReader.h"
#include "LZWBuffer.h"

// Data types local to this class and aren't used by the rest of the system.

#define RETURN_IF_OUT_OF_DATA() { if (OutOfData()) { return FALSE; } }

#define GIF_SHORT_SIZE 2

#define TRANSPARENCY_SET 0x01
#define COLORMAP 0x80
#define BITS_PER_PIXEL_FACTOR 0x07

#define LEAVE_GRAPHIC 0x04
#define CLEAR_GRAPHIC 0x08
#define RESTORE_PREVIOUS 0x10

// Class constructor for a GIF frame used for an animation.
// A non-animated GIF will have only a single frame.
RGIFFrame::RGIFFrame(
	const SGIFImageDescriptor &Descriptor,	// Image description
	const SGIFColorTable *pColorTable,		// Local image palette if it has one.  NULL if there is no local palette for this image.
	const short *pnDelayTime,					// Delay time if set for this frame.  NULL if there is no delay time.
	const BYTE *pnTransparentColorIndex )	// Transparent color for the frame.  NULL if the frame has no transparent color.
{
	m_Descriptor = Descriptor;
	if (pColorTable != NULL)
	{
		SetColorTable( *pColorTable );
	}

	if (pnDelayTime != NULL)
	{
		m_nDelayTime = *pnDelayTime;
		m_bHasDelay = TRUE;
	}
	else
	{
		m_bHasDelay = FALSE;
		m_nDelayTime = 0;
	}

	if (pnTransparentColorIndex != NULL)
	{
		m_nTransparentColorIndex = *pnTransparentColorIndex;
		m_bHasTransparency = TRUE;
	}
	else
	{
		m_bHasTransparency = FALSE;
	}

	m_eClearRequest = CS_UNSPECIFIED;

	m_pBitmap = NULL;
}	

RGIFFrame::RGIFFrame()
{
	m_pColorTable = NULL;
	m_eClearRequest = CS_UNSPECIFIED;
	m_pBitmap = NULL;
	m_bHasTransparency = FALSE;
	m_bHasDelay = FALSE;
	m_nDelayTime = 0;
}

// Inform the caller if the image has a delay.
short RGIFFrame::GetDelay()
{
	if (m_bHasDelay)
		return m_nDelayTime;
	else
		return 0;
}


/* Method to provide the size of the palette associated with this frame.

	If there is no palette for this frame, the size is 0.
*/
int RGIFFrame::GetPaletteSize()
{
	int nPalSize = 0;

	// Make sure we have a palette.
	if (HasLocalPalette())
	{
		nPalSize = (int)(1<<((m_Descriptor.cPackedFields & BITS_PER_PIXEL_FACTOR)+1));
	}

	return nPalSize;
}



// Default Constructor.
RGifReader::RGifReader()
{
	m_pGlobalColorTable = NULL;
	m_pGifFrame = NULL;
	m_nLoopCount = 0;

	// Ensure we can't read data till we have some.
	m_nGIFBufferSize = -1;
	m_nByteCount = 0;
	m_bFoundLoopBlock = FALSE;
}

// Constucts a GIF interface and initializes the data buffer.
RGifReader::RGifReader(
	void *pGIFData,	// Provides the data to be read.
	int nBufferSize )	// Specifies the amount of data available.
{
	SetDataBuffer( pGIFData, nBufferSize );
	m_pGifFrame = NULL;
	m_pGlobalColorTable = NULL;
	m_nLoopCount = 1;	// Assume a one pass image.
	m_bFoundLoopBlock = FALSE;
}


RGifReader::~RGifReader()
{
	// Clean up any frame which was in production.
	if (m_pGifFrame != NULL)
	{
		CBitmap *pBitmap = m_pGifFrame->GetBitmap();
		if (pBitmap != NULL)
		{
			pBitmap->DeleteObject();
			delete pBitmap;
		}
		pBitmap = m_pGifFrame->GetMaskBitmap();
		if (pBitmap != NULL)
		{
			pBitmap->DeleteObject();
			delete pBitmap;
		}
		delete m_pGifFrame;
	}

	// Clean out the frame list.
	while (!m_FrameList.IsEmpty())
	{
		RGIFFrame *pFrame = static_cast<RGIFFrame*>(m_FrameList.RemoveHead());

		if (pFrame != NULL)
		{
			// Kill the provided bitmap if present.
			CBitmap *pBitmap = pFrame->GetBitmap();
			if (pBitmap != NULL)
			{
				pBitmap->DeleteObject();
				delete pBitmap;
			}
			pBitmap = pFrame->GetMaskBitmap();
			if (pBitmap != NULL)
			{
				pBitmap->DeleteObject();
				delete pBitmap;
			}

			delete pFrame;
		}
	}

	if (m_pGlobalColorTable != NULL)
	{
		delete m_pGlobalColorTable;
		m_pGlobalColorTable = NULL;
	}
}


/* RGifReader method that reads the entire GIF image from the buffer and builds
	the frame list so that it can be used for display and animation.

	Returns TRUE on success, FALSE if an error occurs.
 */
bool RGifReader::ReadGIF()
{
	ASSERT( m_pGIFData );

	// We can't read the GIF file if the buffer isn't starting at the beginning.
	if (!AtStartOfBuffer())
	{
		Rewind();
	}

	// Read in the required header block for the image.
	SGIFHeader GIFHeader;
	ReadHeader( &GIFHeader );

	// If the image isn't stored in GIF89a format, it won't have a transparent
	// index so we can cease processing the data.
	if (VerifyGIF(GIFHeader) == FALSE)
	{
		return FALSE;
	}

	TRACE( "GIF file verified\n" );

	// Read in the required logical screen descriptor block.
	int nPaletteSize;
	if (!ReadLogicalScreenDescriptor(&nPaletteSize))
	{
		// We couldn't read the descriptor, so fail.
		return FALSE;
	}

	TRACE( "Width = %d, Height = %d, Palette Size = %d, Has Global Palette = %s\n",
		m_ScreenDescriptor.nWidth, m_ScreenDescriptor.nWidth,
		1 << ((m_ScreenDescriptor.cScreenInfo & 0x07) + 1),
		(m_ScreenDescriptor.cScreenInfo & 0x80) == 0 ? "FALSE" : "TRUE" );
	TRACE( "Background Color = %d, Aspect Ration = %d\n", m_ScreenDescriptor.cBackground, m_ScreenDescriptor.cAspectRatio );

	// Process the blocks as they come looking for the transparent index.
	bool bCompleted = FALSE;
	bool bHitError = FALSE;
	while (!bCompleted && !bHitError)
	{
		EBlockLabel eToken;
		if (!ReadToken( &eToken ))
		{
			// Error reading a block token, no index found.
			bHitError = TRUE;
			continue;	// End the loop
		}

		// Process the token.
		switch (eToken)
		{
			case GIF_TRAILER:
				// This is the end of the GIF data stream.
				bCompleted = TRUE;
				TRACE( "GIF End-Of-File found.\n" );
				break;
			case GIF_EXT_INTRODUCER:
				bHitError = (ReadExtension() == FALSE);
				break;
			case GIF_IMAGE_DESCRIPTOR:
				bHitError = (ReadImageBlock() == FALSE);
				break;
			default:
				// Not a valid token.
				TRACE( "Invalid GIF token read.\n" );
				ASSERT( 0 );
				continue;
				break;
		}
	}

	return bCompleted;
}


/*	@mfunc	<c RGifReader> method that determines the transparent index for
			the current GIF file buffer.

	@rdesc	TRUE if a palette index was found, FALSE if not.

	@devnote The return value <p must> be tested since it's value is not changed
			 if a transparent index value is not found in the image data buffer.
*/
bool RGifReader::ReadTransparentIndex(
	int *pnTransparentIndex,	// @parm The transparent index for the file.
	int *pnGifPaletteSize )		// @parm The size of the Gif palette the index is from.
{
	ASSERT( m_pGIFData );

	// We can't read the GIF file if the buffer isn't starting at the beginning.
	if (!AtStartOfBuffer())
	{
		Rewind();
	}

	// Read in the required header block for the image.
	SGIFHeader GIFHeader;
	ReadHeader( &GIFHeader );

	// If the image isn't stored in GIF format, it won't have a transparent
	// index so we can cease processing the data.
	if (VerifyGIF(GIFHeader) == FALSE)
	{
		return FALSE;
	}

	// Read in the required logical screen descriptor block.
	int nPaletteSize;
	if (!ReadLogicalScreenDescriptor(&nPaletteSize))
	{
		// We couldn't read the descriptor, so fail.
		return FALSE;
	}

	// Provide the caller with the global image palette size.
	*pnGifPaletteSize = nPaletteSize;

	// Process the blocks as they come looking for the transparent index.
	bool bFound = FALSE;
	bool bFoundEOF = FALSE;	// Set to TRUE if we find the GIF terminator.
	while (!bFound && !bFoundEOF)
	{
		EBlockLabel eToken;
		if (!ReadToken( &eToken ))
		{
			// Error reading a block token, no index found.
			return FALSE;
		}

		// Process the token.
		switch (eToken)
		{
			case GIF_TRAILER:
				bFoundEOF = TRUE;
				continue;
				break;
			case GIF_EXT_INTRODUCER:
				{
					EBlockLabel eExtensionLabel;
					if (!ReadToken( &eExtensionLabel ))
					{
						return FALSE;
					}
					// Process the extension label.
					switch (eExtensionLabel)
					{
						case GIF_GRAPH_CONTRL_EXT:
							{
								short nTransparentIndex;
								// Read the graphic control block for the transparency.
								if (ReadTransparency( &nTransparentIndex ))
								{
									*pnTransparentIndex = (int)nTransparentIndex;
									return TRUE;
								}
								else
								{
									return FALSE;
								}
							}
							break;
						case GIF_APP_EXT:
							ReadApplicationExtension();
							break;
						default:
							// Read through the unsupported extension.
							if ( ReadUnsupportedBlock() == FALSE )
							{
								return FALSE;
							}
							break;
					}

					// Extension blocks must be terminated with a zero length block.
					// However custom extensions can have any number of blocks, so we must
					// continuously read blocks until we find one that is either of zero length
					// or until we run out of buffer space.
					bool bEndOfExtensionBlocks = FALSE;
					while (!bEndOfExtensionBlocks)
					{
						RETURN_IF_OUT_OF_DATA();
						BYTE nBlockLength = ReadOneByteOfData();
						if (nBlockLength != 0)
						{
							SkipXBytesOfData( nBlockLength );
						}
						else
						{
							bEndOfExtensionBlocks = TRUE;
						}
					}
				}
				break;
			case GIF_IMAGE_DESCRIPTOR:
				if ( ReadImageBlock() == FALSE )
				{
					return FALSE;
				}
				break;
			default:
				// Not a valid token.
				ASSERT( 0 );
				continue;
				break;
		}
	}

	return FALSE;
}


/*	@mfunc	<c RGifReader> method that determines whether the file stores an
				animated GIF image.

	@rdesc	TRUE if the image is an animated GIF, FALSE if not.
*/
BOOLEAN RGifReader::IsAnimatedGIF()
{
	SGIFHeader GIFHeader;

	ASSERT( m_pGIFData );

	// We can't read the GIF file if the buffer isn't starting at the beginning.
	if (!AtStartOfBuffer())
	{
		Rewind();
	}

	// Read in the required header block for the image.
	ReadHeader( &GIFHeader );

	// If the image isn't stored in GIF89a format, it won't have a transparent
	// index so we can cease processing the data.
	if (Verify89a(GIFHeader) == FALSE)
	{
		return FALSE;
	}

	// Read in the required logical screen descriptor block.
	int nPaletteSize;
	if (!ReadLogicalScreenDescriptor(&nPaletteSize))
	{
		// We couldn't read the descriptor, so fail.
		return FALSE;
	}

	// Process the blocks as they come looking for the transparent index.
	bool bFound = FALSE;
	bool bFoundEOF = FALSE;	// Set to TRUE if we find the GIF terminator.
	while (!bFound && !bFoundEOF)
	{
		EBlockLabel eToken;
		if (!ReadToken( &eToken ))
		{
			// Error reading a block token, no index found.
			return FALSE;
		}

		// Process the token.
		switch (eToken)
		{
			case GIF_TRAILER:
				bFoundEOF = TRUE;
				continue;
				break;
			case GIF_EXT_INTRODUCER:
				{
					EBlockLabel eExtensionLabel;
					if (!ReadToken( &eExtensionLabel ))
					{
						return FALSE;
					}
					// Process the extension label.
					switch (eExtensionLabel)
					{
						case GIF_GRAPH_CONTRL_EXT:
							ReadGraphicControlExtension();
							break;
						case GIF_APP_EXT:
							ReadApplicationExtension();
							if (m_bFoundLoopBlock)
							{
								if (m_nLoopCount != 1)
								{
									return TRUE;
								}
								else
								{
									return FALSE;
								}
							}
							break;
						default:
							// Read through the unsupported extension.
							if ( ReadUnsupportedBlock() == FALSE )
							{
								return FALSE;
							}
							break;
					}

					// Extension blocks must be terminated with a zero length block.
					// However custom extensions can have any number of blocks, so we must
					// continuously read blocks until we find one that is either of zero length
					// or until we run out of buffer space.
					bool bEndOfExtensionBlocks = FALSE;
					while (!bEndOfExtensionBlocks)
					{
						RETURN_IF_OUT_OF_DATA();
						BYTE nBlockLength = ReadOneByteOfData();
						if (nBlockLength != 0)
						{
							SkipXBytesOfData( nBlockLength );
						}
						else
						{
							bEndOfExtensionBlocks = TRUE;
						}
					}
				}
				break;
			case GIF_IMAGE_DESCRIPTOR:
				if ( ReadImageBlock() == FALSE )
				{
					return FALSE;
				}
				break;
			default:
				continue;	// Not a valid token.
				break;
		}
	}

	return FALSE;
}


/*	@mfunc <c RGifReader> method that reads a single byte of data from the
			GIF buffer.

	@rdesc	The byte of data read in.
*/
BYTE RGifReader::ReadOneByteOfData()
{
	BYTE cDataByte;

	cDataByte = *m_pGIFData;
	m_pGIFData++;
	m_nByteCount++;

	return cDataByte;
}


/* Skips over the specified number of bytes without reading the data.

	Returns : TRUE on success, FALSE if the data runs out.
 */
bool RGifReader::SkipXBytesOfData(
	int nBytesToSkip)	// The number of buffered bytes to skip over.
{
	bool bOutOfData = OutOfData( nBytesToSkip );

	m_nByteCount += nBytesToSkip;
	m_pGIFData += nBytesToSkip;

	return !bOutOfData;
}


/*	@mfunc <c RGifReader> method that reads a block of data into the buffer
			provided.

	@rdesc	TRUE if all of the data was able to be read.  FALSE if the data ran out.
*/
bool RGifReader::ReadXBytesOfData(
	BYTE *pBuffer,		// @parm The buffer to read the data into.
	int nBytesToRead )	// @parm The number of bytes to read into the buffer.
{
	if (OutOfData( nBytesToRead ))
	{
		// Ensure we'll be out for the next read as well.
		m_nByteCount += nBytesToRead;
		return FALSE;
	}
	// Fetch the data.
	for (int nIndex = 0; nIndex < nBytesToRead; nIndex++)
	{
		pBuffer[nIndex] = ReadOneByteOfData();
	}

	return TRUE;
}


/*	@mfunc <c RGifReader> method that reads in a short integer worth of
			data from the input buffer.

	@rdesc	TRUE if there was enough data to read the int.  FALSE if the data ran out.
*/
bool RGifReader::ReadOneShortOfData(
	short *pShortValue )	// @parm The address of the short variable to fill.
{
	if (OutOfData( GIF_SHORT_SIZE ))	// GIF shorts use 2 bytes.
	{
		// Ensure we'll be out for the next read as well.
		m_nByteCount += GIF_SHORT_SIZE;
		return FALSE;
	}

	*pShortValue = *((short *)m_pGIFData);
	m_pGIFData += GIF_SHORT_SIZE;
	m_nByteCount += GIF_SHORT_SIZE;

	return TRUE;
}


/*	@mfunc <c RGifReader> method that provides whether there was enough
			data left to read a single byte of data.

	@rdesc	TRUE if there is at least one more byte, FALSE if not.
*/
bool RGifReader::OutOfData()
{
	return (m_nByteCount > m_nGIFBufferSize);
}


/*	@mfunc <c RGifReader> method that verifies if there is enough data left
			to read the specified number of bytes.

	@rdesc	TRUE if enough data exists to perform a read of the specified size,
			FALSE if not.
*/
bool RGifReader::OutOfData(
	int nBytesDesired)	// @parm The number of bytes that will be read.
{
	return ((m_nByteCount + (nBytesDesired-1)) > m_nGIFBufferSize);
}



/*	@mfunc <c RGifReader> method that initializes the internal data buffer
			for reading the GIF.  This buffer must be set before any attempt to
			read the GIF data is attempted.
*/
void RGifReader::SetDataBuffer(
	void *pGIFData,		// @parm The GIF data buffer to set.
	int nBufferSize )	// @parm The size of the data buffer.
{
	// Set the internal buffer pointer.  This buffer shouldn't change unless
	// another buffer is set.
	m_pGIFBuffer = (BYTE *)pGIFData;
	// Set our pointer used for walking the image data.
	m_pGIFData = (BYTE *)pGIFData;
	// Set the size of the buffer for spotting when we've run out of data.
	m_nGIFBufferSize = nBufferSize;
	// Initialize our point in the buffer so we know when we've run out of data.
	m_nByteCount = 0;
}


/*	@mfunc <c RGifReader> method that reads in the GIF header with its
			version.

	@rdesc	TRUE if there was enough data to read the header, FALSE if not.
*/
bool RGifReader::ReadHeader(
	SGIFHeader *pGIFHeader)
{
	// Read the "GIF" block from the buffer.
	if (ReadXBytesOfData( pGIFHeader->GIF, 3 ) == FALSE)
	{
		return FALSE;
	}
	// Read the file version from the buffer.
	if (ReadXBytesOfData( pGIFHeader->Version, 3 ) == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}


/*	@mfunc <c RGifReader> method that checks the supplied GIF header and
			ensures that its version is at least 89a.

	@rdesc	TRUE if the file version is 89a, and FALSE if not.
*/
bool RGifReader::Verify89a(
	const SGIFHeader &GIFHeader )
{
	return ( (strncmp( (char *)GIFHeader.GIF, "GIF", 3 ) == 0 ) &&
			 (strncmp( (char *)GIFHeader.Version, "89a", 3 ) == 0) );
}


/*	RGifReader method that checks the supplied GIF header and ensures that it is
	a GIF image based on its header.  This only requires the first three
	characters to be GIF, and validates for debug builds that the version is
	one of the known version as of this routine's last update.

	Returns : TRUE if the file is a GIF file, and FALSE if not.
*/
bool RGifReader::VerifyGIF(
	const SGIFHeader &GIFHeader )
{
	ASSERT(	(strncmp( (char *)GIFHeader.Version, "87a", 3 ) == 0) ||
				(strncmp( (char *)GIFHeader.Version, "89a", 3 ) == 0) );
	return ( strncmp( (char *)GIFHeader.GIF, "GIF", 3 ) == 0 );
}


/* 	@mfunc <c RGifReader> method that reads the image description
			information from the buffer.

	@rdesc	TRUE if the logical screen descriptor could be read, FALSE if we ran out of data.
*/
bool RGifReader::ReadLogicalScreenDescriptor(
	int *pnPaletteSize )	// @parm The size of the global palette.
{
	// Read in the screen descriptor.
	ASSERT( sizeof( m_ScreenDescriptor ) == 7 );
	if ( !ReadXBytesOfData( (LPBYTE)&m_ScreenDescriptor, sizeof( m_ScreenDescriptor ) ) )
	{
		return FALSE;
	}

	// Read the global color palette if present.
	if (m_ScreenDescriptor.cScreenInfo & COLORMAP)
	{
		// If we don't have a color table, create one.
		if (m_pGlobalColorTable == NULL)
		{
			m_pGlobalColorTable = new SGIFColorTable;
		}

		int nPalSize = (int)(1<<((m_ScreenDescriptor.cScreenInfo & BITS_PER_PIXEL_FACTOR)+1));
		if ( ReadColorMap(m_pGlobalColorTable, nPalSize) == FALSE )
		{
			return FALSE;
		}
		*pnPaletteSize = nPalSize;
	}
	else
	{
		*pnPaletteSize = 0;
	}

	return TRUE;
}


/*	@mfunc <c RGifReader> method that reads a single label token out of the
			data stream.

	@rdesc	TRUE if data existed to read, FALSE if not.
*/
bool RGifReader::ReadToken(
	EBlockLabel *peToken )	// @parm The token variable to be set.
{
	RETURN_IF_OUT_OF_DATA();

	BYTE cDataToken = ReadOneByteOfData();
	switch (cDataToken)
	{
		case GIF_IMAGE_DESCRIPTOR:
		case GIF_EXT_INTRODUCER:
		case GIF_GRAPH_CONTRL_EXT:
		case GIF_PLAIN_TEXT_EXT:
		case GIF_APP_EXT:
		case GIF_COMMENT_EXT:
		case GIF_TRAILER:
			*peToken = (EBlockLabel)cDataToken;
			break;
		default:
			*peToken = GIF_UNKNOWN;
			break;
	}

	return TRUE;
}


/*	@mfunc <c RGifReader> method that reads the graphic control extension
			block from the data stream and extracts the transparent index from
			it.

	@rdesc	TRUE if the transparency was set, FALSE if not or data ran out.
*/
bool RGifReader::ReadTransparency(
	short *pnTransparentIndex )	// @parm The variable to store the transparent index.
{
	SGIFGraphicControlExtension GraphicExt;

	RETURN_IF_OUT_OF_DATA();
	BYTE cBlockSize = ReadOneByteOfData();
	RETURN_IF_OUT_OF_DATA();
	GraphicExt.cPackedField = ReadOneByteOfData();
	if (ReadOneShortOfData(&(GraphicExt.nDelayTime)) == FALSE)
	{
		return FALSE;
	}
	RETURN_IF_OUT_OF_DATA();
	GraphicExt.nTransparentColorIndex = (short)ReadOneByteOfData();

	// Ensure we've read the entire block in case the size is different in this file.
	int nSizeRead = 4;
	int nSizeToRead = (int)cBlockSize - nSizeRead;
	if (nSizeToRead > 0)
	{
		SkipXBytesOfData( nSizeToRead );
	}

	if (GraphicExt.cPackedField & TRANSPARENCY_SET)
	{
		*pnTransparentIndex = GraphicExt.nTransparentColorIndex;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/*	@mfunc <c RGifReader> method that reads the graphic control extension
			block from the data stream and extracts the delay time from it.

	@rdesc	TRUE if the block was read successfully, FALSE if not.
*/
bool RGifReader::ReadDelay(
	short *pnDelay )	// @parm The variable to store the delay time in.
{
	SGIFGraphicControlExtension GraphicExt;

	RETURN_IF_OUT_OF_DATA();
	BYTE cBlockSize = ReadOneByteOfData();
	RETURN_IF_OUT_OF_DATA();
	GraphicExt.cPackedField = ReadOneByteOfData();
	if (ReadOneShortOfData(&(GraphicExt.nDelayTime)) == FALSE)
	{
		return FALSE;
	}
	RETURN_IF_OUT_OF_DATA();
	GraphicExt.nTransparentColorIndex = (short)ReadOneByteOfData();

	// Ensure we've read the entire block in case the size is different in this file.
	int nSizeRead = 4;
	int nSizeToRead = (int)cBlockSize - nSizeRead;
	if (nSizeToRead > 0)
	{
		SkipXBytesOfData( nSizeToRead );
	}

	*pnDelay = GraphicExt.nDelayTime;
	
	// The block was read OK, so inform the caller.
	return TRUE;
}


/*	@mfunc <c RGifReader> method that reads past an unsuported block type.
			The data read is simply thrown away.

	@rdesc	TRUE if the block could be read past, FALSE if data ran out.
*/
bool RGifReader::ReadUnsupportedBlock()
{
	bool bReadOK = FALSE;

	RETURN_IF_OUT_OF_DATA();
	BYTE nBlockSize = (int)ReadOneByteOfData();

	if (nBlockSize != 0)
	{
		bReadOK = SkipXBytesOfData( nBlockSize );
	}

	return bReadOK;
}



/*	RGifReader method that reads past the image block.
	The image data is decompressed and placed into a bitmap for rendering later.

	Returns TRUE on successful creation of the image, FALSE if a failure occurs.
*/
bool RGifReader::ReadImageBlock()
{
	SGIFImageDescriptor ImageDescriptor;

	// Read in the image descriptor block.
	if ( ReadImageDescriptor( &ImageDescriptor ) == FALSE )
	{
		return FALSE;
	}

	TRACE( "Image %d Being Read : Left = %d, Right = %d, Width = %d, Height = %d, Has Palette = %s, Pal Size = %d, Is Interlaced = %s\n",
			m_FrameList.GetCount() + 1, ImageDescriptor.nLeftPosition, ImageDescriptor.nTopPosition,
			ImageDescriptor.nWidth, ImageDescriptor.nHeight,
			(ImageDescriptor.cPackedFields & COLORMAP) ? "TRUE" : "FALSE",
			(1<<((ImageDescriptor.cPackedFields & BITS_PER_PIXEL_FACTOR)+1)),
			(ImageDescriptor.cPackedFields & IDESC_INTERLACED) ? "TRUE" : "FALSE" );

	// Create a new image frame if one isn't in progress.
	if (m_pGifFrame == NULL)
	{
		m_pGifFrame = new RGIFFrame;
	}

	// Put the image descriptor into the frame being constructed.
	m_pGifFrame->SetDescriptor( ImageDescriptor );
	
	// Read in the color map if present.
	if (ImageDescriptor.cPackedFields & COLORMAP)
	{
		// Dynamically allocate the color table so it doesn't overload the stack!
		SGIFColorTable *pColorTable = new SGIFColorTable;

		int nPalSize = (int)(1<<((ImageDescriptor.cPackedFields & BITS_PER_PIXEL_FACTOR)+1));
		if ( ReadColorMap(pColorTable, nPalSize) == FALSE )
		{
			delete pColorTable;
			return FALSE;
		}
		else
		{
			m_pGifFrame->SetColorTable( *pColorTable );
			delete pColorTable;
		}
	}

	// Now process the rest of the image data throwing it all away.

	// First read off the minimum code size
	RETURN_IF_OUT_OF_DATA();
	BYTE nMinCodeSize = ReadOneByteOfData();

	// Now read off the image data.
	bool bFoundEndOfImage = FALSE;
	LZWBufferParser *pLZWParser = new LZWBufferParser( m_pGIFData, SpaceLeftInBuffer(), nMinCodeSize, TRUE );

	// Allocate a memory buffer to store the decompressed image data.
	CSize szImage = m_pGifFrame->GetSize();
	LPBYTE pImageData = new BYTE[szImage.cx * szImage.cy];

	int errorState = 0;
	if (m_pGifFrame->InterlacedGIF())
	{
		// Lines interleave on each pass, they follow the template below:
		//
		// Line #             Interlace Pass
		//   1   ---------  1
		//   2   ---------                    4
		//   3   ---------              3
		//   4   ---------                    4
		//   5   ---------        2
		//   6   ---------                    4
		//   7   ---------              3
		//   8   ---------                    4
		//   9   ---------  1
		//  10   ---------                    4
		//  11   ---------              3
		//       . . .

		int nLine;
		int nLineIncrement;
		int nOffsetIncrement;
		int nCurrentOffset = 0;
		for (int nPass = 1; (nPass <= 4) && (errorState == 0); nPass++)
		{
			// Read a pass.
			int nCurrentOffset = 0;
			switch (nPass)
			{
				case 1:
					// Starts right away, and jumps every 8 lines.
					nCurrentOffset = 0;
					nOffsetIncrement = 8 * szImage.cx;
					nLineIncrement = 8;
					nLine = 1;
					break;
				case 2:
					// Starts on the 5th line, and jumps every 8 lines.
					nCurrentOffset = 4 * szImage.cx;
					nOffsetIncrement = 8 * szImage.cx;
					nLineIncrement = 8;
					nLine = 5;
					break;
				case 3:
					// Starts on line 3, and jumps every 4 lines.
					nCurrentOffset = 2 * szImage.cx;
					nOffsetIncrement = 4 * szImage.cx;
					nLineIncrement = 4;
					nLine = 3;
					break;
				case 4:
					// Starts on the second line, and skips every two filling in the final gaps.
					nCurrentOffset = szImage.cx;
					nOffsetIncrement = 2 * szImage.cx;
					nLineIncrement = 2;
					nLine = 2;
					break;
				default:
					// This shouldn't happen!  If it does, set pass 4 values...
					ASSERT( 0 );
					nCurrentOffset = szImage.cx;
					nOffsetIncrement = 2 * szImage.cx;
					nLineIncrement = 2;
					nLine = 2;
					break;
			}

			while (errorState == 0 && (nLine <= szImage.cy))
			{
				// Read a line of data into the buffer.
				errorState = pLZWParser->read( 0, &pImageData[nCurrentOffset], szImage.cx );

				nLine+=nLineIncrement;
				nCurrentOffset += nOffsetIncrement;
			}
		}
	}
	else
	{
		// Read the entire image buffer... the data isn't intermixed.
		DWORD dwSizeToRead = (DWORD)szImage.cx * (DWORD)szImage.cy;
		DWORD dwSizeRead = 0;
		while ((dwSizeRead < dwSizeToRead) && !errorState)
		{
			// Read 10k at most in a single read.  (The read takes a short!)
			DWORD dwSizeBlock = __min( 10240l, dwSizeToRead - dwSizeRead );
			
			errorState = pLZWParser->read( 0, &pImageData[dwSizeRead], (SHORT)dwSizeBlock );

			dwSizeRead += dwSizeBlock;
		}

		if (errorState != 0)
		{
			bFoundEndOfImage = TRUE;
		}
	}

	// Move the buffer to the point we've read through rather than skip through it.
	SkipXBytesOfData( (int)pLZWParser->GetBytesRead() );

	// See if we have already read the image block terminator.  It's possible that we won't
	// if by coincedence we don't request a data block larger than the available block size.
	if (pLZWParser->HitImageBlockTerminator())
	{
		bFoundEndOfImage = TRUE;
	}

	// For most reads we'll need to drop into this block in order to read the
	// final image block terminator, only for image read errors will we ever
	// have block data left to sift through.
	while (!bFoundEndOfImage)
	{
		// Check for existing data
		if (OutOfData())
		{
			break;
		}

		// Fetch the size of the image block.
		int nBlockSize = (int)ReadOneByteOfData();
		// Have we reached the end of the file?
		if (nBlockSize == 0)
		{
			bFoundEndOfImage = TRUE;
		}
		else
		{
			TRACE( "We have excess image data, this shouldn't happen!\n" );
			ASSERT( 0 );

			// No, so read through this block.
			BYTE *pDumpBuffer = new BYTE[nBlockSize];
			bool bReadOK = ReadXBytesOfData( pDumpBuffer, nBlockSize );
			delete [] pDumpBuffer;
			if (!bReadOK)
			{
				// Dump out of hte loop, we couldn't read all the data.
				break;
			}
		}
	}
	delete pLZWParser;

	// Set the frame with the image read.
	CreateFrameBitmaps( pImageData );

	// We no longer need the decompressed image data buffer, so clean it up.
	delete [] pImageData;

	// Add the frame to the list of frames and clear it so we can start the next.
	m_FrameList.AddTail( m_pGifFrame );
	m_pGifFrame = NULL;

	return bFoundEndOfImage;
}


/*	@mfunc <c RGifReader> method that reads in the color map from the
			data stream.

	@rdesc	TRUE if the color map could be read, FALSE if data ran out.
*/
bool RGifReader::ReadColorMap(
	SGIFColorTable *pColorMap,	// The palette table to fill in.
	int nPaletteSize )				// The size of the palette block to read.
{
	// Build the color table.
	for (int nIndex = 0; nIndex < nPaletteSize; nIndex++)
	{
		RETURN_IF_OUT_OF_DATA();
		pColorMap->RGB[nIndex].Red = ReadOneByteOfData();
		RETURN_IF_OUT_OF_DATA();
		pColorMap->RGB[nIndex].Green = ReadOneByteOfData();
		RETURN_IF_OUT_OF_DATA();
		pColorMap->RGB[nIndex].Blue = ReadOneByteOfData();
	}

	return TRUE;
}	



/*	@mfunc <c RGifReader> method that reads the image descriptor data from
			the data stream and places it in the descriptor provided.
*/
bool RGifReader::ReadImageDescriptor(
	SGIFImageDescriptor *pImageDescriptor )	// @parm The image descriptor to fill.
{
	ASSERT( pImageDescriptor );

	return ReadXBytesOfData( (LPBYTE)pImageDescriptor, sizeof( SGIFImageDescriptor ) );
}


/* Reads the extension data from the file buffer.

	Returns : TRUE if the extension is read successfully, FALSE if an error occurs.
 */
bool RGifReader::ReadExtension()
{
	// Get the token for the extension.
	EBlockLabel eExtensionLabel;
	if (!ReadToken( &eExtensionLabel ))
	{
		return FALSE;
	}

	// Process the extension label.
	switch (eExtensionLabel)
	{
		case GIF_GRAPH_CONTRL_EXT:
			ReadGraphicControlExtension();
			TRACE( "GraphicControlExtension : Trans = %d, Has Delay = %s, Delay = %d, Clear State = %d\n",
					(m_pGifFrame->IsTransparent()) ? m_pGifFrame->GetTransparentIndex() : -1,
					m_pGifFrame->HasDelay() ? "TRUE" : "FALSE",
					m_pGifFrame->GetDelay(),
					m_pGifFrame->GetImageClearState() );
			break;
		case GIF_APP_EXT:
			ReadApplicationExtension();
			break;
		case GIF_PLAIN_TEXT_EXT:
			TRACE( "Plain Text Block : " );
		case GIF_COMMENT_EXT:
			if (eExtensionLabel == GIF_COMMENT_EXT)
			{
				TRACE( "Comment Block : " );
			}
		default:
			TRACE( "Reading Unsupported extension.\n" );
			// Read through the unsupported extension.
			if ( ReadUnsupportedBlock() == FALSE )
			{
				return FALSE;
			}
			break;
	}

	// Extension blocks must be terminated with a zero length block.
	// However custom extensions can have any number of blocks, so we must
	// continuously read blocks until we find one that is either of zero length
	// or until we run out of buffer space.
	bool bEndOfExtensionBlocks = FALSE;
	while (!bEndOfExtensionBlocks)
	{
		RETURN_IF_OUT_OF_DATA();
		BYTE nBlockLength = ReadOneByteOfData();
		if (nBlockLength != 0)
		{
			SkipXBytesOfData( nBlockLength );
		}
		else
		{
			bEndOfExtensionBlocks = TRUE;
		}
	}
	return TRUE;
}


/* Read the graphic control extension data from the buffer.

	Returns : TRUE on success, FALSE if an error occurs.
 */
bool RGifReader::ReadGraphicControlExtension()
{
	SGIFGraphicControlExtension GraphicExt;

	RETURN_IF_OUT_OF_DATA();
	BYTE cBlockSize = ReadOneByteOfData();

	if ( !ReadXBytesOfData((LPBYTE)&GraphicExt, sizeof( GraphicExt )) )
	{
		return FALSE;
	}

	// Ensure we've read the entire block in case the size is different in this file.
	int nSizeRead = sizeof( GraphicExt );
	int nSizeToRead = (int)cBlockSize - nSizeRead;
	if (nSizeToRead > 0)
	{
		SkipXBytesOfData( nSizeToRead );
	}

	// Create a new image frame if one isn't in progress.
	if (m_pGifFrame == NULL)
	{
		m_pGifFrame = new RGIFFrame;
	}

	// Fill in the known values of the currently in construction GIF frame.
	if (GraphicExt.cPackedField & TRANSPARENCY_SET)
	{
		m_pGifFrame->SetTransparentIndex( GraphicExt.nTransparentColorIndex );
	}

	if (GraphicExt.nDelayTime != 0)
	{
		m_pGifFrame->SetDelay( GraphicExt.nDelayTime );
	}

	// Set the method for how this frame should be erased prior to drawing the next frame.
	if (GraphicExt.cPackedField & LEAVE_GRAPHIC)
	{
		m_pGifFrame->SetImageClearState( CS_LEAVE_IMAGE );
	}
	else if (GraphicExt.cPackedField & CLEAR_GRAPHIC)
	{
		m_pGifFrame->SetImageClearState( CS_ERASE_TO_BACKGROUND );
	}
	else if (GraphicExt.cPackedField & RESTORE_PREVIOUS)
	{
		m_pGifFrame->SetImageClearState( CS_ERASE_TO_PREV_IMAGE );
	}
	else
	{
		m_pGifFrame->SetImageClearState( CS_UNSPECIFIED );
	}

	return TRUE;
}


/* Read the application extension data from the buffer.

	Returns : TRUE on success, FALSE if an error occurs.
 */
bool RGifReader::ReadApplicationExtension()
{
	bool bOK = TRUE;

	SGIFApplicationExtensionBlock blockApp;

	RETURN_IF_OUT_OF_DATA();
	BYTE cBlockSize = ReadOneByteOfData();

	ASSERT(cBlockSize == 11);
	ASSERT(sizeof(blockApp) == 11);
	ReadXBytesOfData((LPBYTE)&blockApp, sizeof(blockApp));

	if ((strncmp( (CHAR *)&blockApp.bAsciiCode[0], "NETSCAPE", 8 ) == 0) &&
		 (strncmp( (CHAR *)&blockApp.bBinaryCode[0], "2.0", 3 ) == 0))
	{
		// Netscape extension block.
		
		// Read the sub-block for looping data.
		BYTE nSubLen;
		if ( !ReadXBytesOfData((LPBYTE)&nSubLen, sizeof(nSubLen)) )
		{
			nSubLen = 0;
		}

		// Verify that the length is sufficient...
		SGIFNetscapeLoopBlock blockLoop;
		if (nSubLen >= sizeof(blockLoop))
		{
			ASSERT(sizeof(blockLoop) == 3);
			ReadXBytesOfData((LPBYTE)&blockLoop, sizeof(blockLoop));

			// Verify that the sub-block is the type we want.
			if (blockLoop.bBlockCode == 0x01)
			{
				// It's a netscape loop block, or so it claims, so save our loop data.
				m_nLoopCount = blockLoop.nLoopCount;
				m_bFoundLoopBlock = TRUE;
				TRACE( "Animation loops %d times.\n", m_nLoopCount );
			}

			// Keep us on track in case the extension block size ever changes.
			if (nSubLen > sizeof( blockLoop ))
			{
				int nSkipLen = (int)nSubLen - sizeof(blockLoop);
				SkipXBytesOfData( nSkipLen );
			}
		}
		else
		{
			// We received a different block from the one we were expecting,
			// So skip over it.
			bOK = SkipXBytesOfData( nSubLen );
		}
	}

	return bOK;
}

/* Provides the delay value for a specific frame of the animation.

	Returns : The animation delay in 100ths of seconds.
				 If an error occurs, this simply returns a delay value of 0.
*/
short RGifReader::GetDelay(
	int nAnimFrame )	// The frame of the animation who's delay is desired.
{
	short nDelay = 0;

	ASSERT( nAnimFrame >= 0 );
	ASSERT( nAnimFrame < m_FrameList.GetCount() );

	POSITION posFrame = m_FrameList.FindIndex( nAnimFrame );
	if (posFrame != NULL)
	{
		RGIFFrame *pFrame = (RGIFFrame *)m_FrameList.GetAt( posFrame );
		if (pFrame != NULL)
		{
			nDelay = pFrame->GetDelay();
		}
	}

	return nDelay;
}


/* Returns the clear state enumeration for the specified frame.
*/
EPImageClearState RGifReader::GetFrameClearRequest(
	int nAnimFrame )	// The index of the frame who's clear request is desired.
{
	EPImageClearState eClearState = CS_UNSPECIFIED;

	ASSERT( nAnimFrame >= 0 );
	ASSERT( nAnimFrame < m_FrameList.GetCount() );

	POSITION posFrame = m_FrameList.FindIndex( nAnimFrame );
	if (posFrame != NULL)
	{
		RGIFFrame *pFrame = (RGIFFrame *)m_FrameList.GetAt( posFrame );
		if (pFrame != NULL)
		{
			eClearState = pFrame->GetImageClearState();
		}
	}

	return eClearState;
}



/* Returns the position within the GIF "Screen" of the specified frame.
*/
CPoint RGifReader::GetFramePosition(
	int nAnimFrame )	// The index of the frame whose position is desired.
{
	CPoint ptPosition( 0, 0 );

	ASSERT( nAnimFrame >= 0 );
	ASSERT( nAnimFrame < m_FrameList.GetCount() );

	POSITION posFrame = m_FrameList.FindIndex( nAnimFrame );
	if (posFrame != NULL)
	{
		RGIFFrame *pFrame = (RGIFFrame *)m_FrameList.GetAt( posFrame );
		if (pFrame != NULL)
		{
			ptPosition = pFrame->GetPosition();
		}
	}

	return ptPosition;
}




/* Returns the size of the GIF "Screen" for the specified frame.
*/
CSize RGifReader::GetFrameSize(
	int nAnimFrame )	// The index of the frame whose size is desired.
{
	CSize sizeFrame( 1, 1 );

	ASSERT( nAnimFrame >= 0 );
	ASSERT( nAnimFrame < m_FrameList.GetCount() );

	POSITION posFrame = m_FrameList.FindIndex( nAnimFrame );
	if (posFrame != NULL)
	{
		RGIFFrame *pFrame = (RGIFFrame *)m_FrameList.GetAt( posFrame );
		if (pFrame != NULL)
		{
			sizeFrame = pFrame->GetSize();
		}
	}

	return sizeFrame;
}


/* Provides the address of the frame and mask bitmaps within the provided
	pointers for the specified frame.

	Returns : TRUE on success, FALSE if it is unable to find the frame.
*/
bool RGifReader::GetBitmaps(
	int nAnimFrame,		// The index of the frame whose bitmaps are desired.
	CBitmap **ppBitmap,	// A pointer to be assigned the address of the frame bitmap pointer
	CBitmap **ppMask )	// A pointer to be assigned the address of the mask bitmap pointer
{
	ASSERT( nAnimFrame >= 0 );
	ASSERT( nAnimFrame < m_FrameList.GetCount() );

	bool bSucceeded = FALSE;

	POSITION posFrame = m_FrameList.FindIndex( nAnimFrame );
	if (posFrame != NULL)
	{
		RGIFFrame *pFrame = (RGIFFrame *)m_FrameList.GetAt( posFrame );
		if (pFrame != NULL)
		{
			*ppBitmap = pFrame->GetBitmap();
			*ppMask = pFrame->GetMaskBitmap();

			bSucceeded = TRUE;
		}
	}

	return bSucceeded;
}



/* This method creates a Windows compatible palette for the specified frame.
	If the frame has a specific palette, this is used, otherwise the GIF's global
	palette is used.

	Note : For easier transparent blitting, the transparent index is moved into
			 the zero index slot.  The color that had been there is moved up into
			 the location that the transparent index had used.

	Returns : TRUE on success, FALSE on failure.
*/
bool RGifReader::CreatePalette(
	int nAnimFrame,			// The index of the frame to build a palette for.
	CPalette *ppalFrame )	// The palette to fill.
{
	ASSERT( (nAnimFrame >= 0) && (nAnimFrame < m_FrameList.GetCount()) );
	ASSERT( ppalFrame != NULL );

	bool bSucceeded = FALSE;

	POSITION posFrame = m_FrameList.FindIndex( nAnimFrame );
	if (posFrame != NULL)
	{
		RGIFFrame *pFrame = (RGIFFrame *)m_FrameList.GetAt( posFrame );
		if (pFrame != NULL)
		{
			if (pFrame->HasLocalPalette())
			{
				// Create the palette from the frame's palette.
				SGIFColorTable *pColorTable = pFrame->GetColorTable();
				BuildPalette( ppalFrame, pColorTable, pFrame->GetTransparentIndex(), pFrame->GetPaletteSize() );
			}
			else
			{
				// Create the palette from the global palette.
				if (m_pGlobalColorTable != NULL)
				{
					bSucceeded = BuildPalette( ppalFrame, m_pGlobalColorTable, pFrame->GetTransparentIndex(), GetGlobalPaletteSize() );
				}
				else
				{
					bSucceeded = FALSE;
					ASSERT( 0 );
				}
			}
		}
	}

	return bSucceeded;
}



/* Provides the size of the global palette.
	This is 0 if the palette is not defined.
*/
int RGifReader::GetGlobalPaletteSize()
{
	int nPalSize = 0;

	if (m_pGlobalColorTable != NULL)
	{
		nPalSize = (int)(1<<((m_ScreenDescriptor.cScreenInfo & BITS_PER_PIXEL_FACTOR)+1));
	}

	return nPalSize;
}



/* Constructs a palette based on the color table provided.

	Note : For easier transparent blitting, the transparent index is moved into
			 the zero index slot.  The color that had been there is moved up into
			 the location that the transparent index had used.

	Returns : TRUE on success, FALSE on failure.
*/
bool RGifReader::BuildPalette(
	CPalette *pPalette,
	SGIFColorTable *pColorTable,
	int nTransIndex,
	int nPalSize )
{
	ASSERT( pPalette != NULL );
	ASSERT( pColorTable != NULL );
	ASSERT( nPalSize > 0 );

	bool bSucceeded = FALSE;

	if (nPalSize > 0)
	{
		BYTE *pPalBuffer = new BYTE[sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nPalSize)];
		LOGPALETTE *plogPalette = (LOGPALETTE *)pPalBuffer;
		plogPalette->palVersion = 0x0300;
		plogPalette->palNumEntries = nPalSize;

		// Slot 0 is allocated to the transparent index, and is forced to black.
		plogPalette->palPalEntry[0].peRed   = 0;
		plogPalette->palPalEntry[0].peGreen = 0;
		plogPalette->palPalEntry[0].peBlue  = 0;
		plogPalette->palPalEntry[0].peFlags = (BYTE)0;

		for (int nIndex = 1; nIndex < nPalSize; nIndex++)
		{
			if (nIndex == nTransIndex)
			{
				// This slot was swapped with the 0 index slot to move the trans index to 0.
				plogPalette->palPalEntry[nIndex].peRed   = pColorTable->RGB[0].Red;
				plogPalette->palPalEntry[nIndex].peGreen = pColorTable->RGB[0].Green;
				plogPalette->palPalEntry[nIndex].peBlue  = pColorTable->RGB[0].Blue;
				plogPalette->palPalEntry[nIndex].peFlags = (BYTE)0;
			}
			else
			{
				plogPalette->palPalEntry[nIndex].peRed   = pColorTable->RGB[nIndex].Red;
				plogPalette->palPalEntry[nIndex].peGreen = pColorTable->RGB[nIndex].Green;
				plogPalette->palPalEntry[nIndex].peBlue  = pColorTable->RGB[nIndex].Blue;
				plogPalette->palPalEntry[nIndex].peFlags = (BYTE)0;
			}
		}

		bSucceeded = (pPalette->CreatePalette( plogPalette ) != FALSE);

		// Clean up the dynamic memory.
		delete [] pPalBuffer;
	}

	return bSucceeded;
}


/* Creates the frame bitmap and its mask from the raw image data.
*/
void RGifReader::CreateFrameBitmaps(
	LPBYTE pRawImageData )	// The raw data to create the bitmaps from.
{
	ASSERT( pRawImageData != NULL );

	// Put the raw bits into a pointer we can walk through the image.
	BYTE *pImageData = pRawImageData;

	// Determine the image size.
	CSize sizeFrame = m_pGifFrame->GetSize();

	// Get the color data for the frame.
	int nPalSize;
	SGIFColorTable *pColorTable;
	if (m_pGifFrame->HasLocalPalette())
	{
		// Create the palette from the frame's palette.
		pColorTable = m_pGifFrame->GetColorTable();
		nPalSize = m_pGifFrame->GetPaletteSize();
	}
	else
	{
		pColorTable = m_pGlobalColorTable;
		nPalSize = GetGlobalPaletteSize();
	}

	// Determine the transparent index for the frame.
	bool bHasTransparency = m_pGifFrame->IsTransparent();
	int nTransparentIndex = 0;
	if (bHasTransparency)
	{
		nTransparentIndex = m_pGifFrame->GetTransparentIndex();
	}

	// Create local memory devices to draw the bitmaps within.
	CDC dcFrame;
	CDC dcMask;
	dcFrame.CreateCompatibleDC( NULL );
	dcMask.CreateCompatibleDC( NULL );

	// Create a palette for them and select it into the devices for 8-bit support.
	CPalette palFrame;
	BuildPalette( &palFrame, pColorTable, nTransparentIndex, nPalSize );
	CPalette *pOldFramePal = dcFrame.SelectPalette( &palFrame, NULL );
	CPalette *pOldMaskPal = dcMask.SelectPalette( &palFrame, NULL );
	dcFrame.RealizePalette();
	dcMask.RealizePalette();

	// Create our bitmaps.  (Based on the Main Window DC, memory DCs create monochrome bitmaps)
	CBitmap *pFrameBitmap = new CBitmap();
	CBitmap *pMaskBitmap = new CBitmap();
	CDC *pMainDC = AfxGetMainWnd()->GetDC();
	pFrameBitmap->CreateCompatibleBitmap( pMainDC, sizeFrame.cx, sizeFrame.cy );
	pMaskBitmap->CreateCompatibleBitmap( pMainDC, sizeFrame.cx, sizeFrame.cy );
	AfxGetMainWnd()->ReleaseDC( pMainDC );

	// Select our new bitmap objects into the devices.
	CBitmap *pOldFrameBMP = dcFrame.SelectObject( pFrameBitmap );
	CBitmap *pOldMaskBMP = dcMask.SelectObject( pMaskBitmap );

	// Fill the frame with black for transparency support.  We'll only draw the non-transparent pixels.
	CBrush brBlack;
	brBlack.CreateSolidBrush( RGB(0,0,0) );
	dcFrame.FillRect( CRect( 0, 0, sizeFrame.cx, sizeFrame.cy ), &brBlack );
	brBlack.DeleteObject();
	// Fill the mask with white.  This will fail to erase the background when used to AND blit.
	CBrush brWhite;
	brWhite.CreateSolidBrush( RGB(255,255,255) );
	dcMask.FillRect( CRect( 0, 0, sizeFrame.cx, sizeFrame.cy ), &brWhite );
	brWhite.DeleteObject();

	// Walk through each row of the image.
	for (int nRow = 0; nRow < sizeFrame.cy; nRow++)
	{
		// Walk Each column of the image.
		for (int nCol = 0; nCol < sizeFrame.cx; nCol++)
		{
			if ((!bHasTransparency) || (pImageData[nCol] != nTransparentIndex))
			{
				// Set the mask bit to black...
				dcMask.SetPixel(nCol, nRow, RGB(0,0,0));
				// Set our color value.
				if ((pImageData[nCol] == 0) && bHasTransparency)
				{
					// We are swapping the transparent index into the zero slot, so the transindex is the colors new slot.
					dcFrame.SetPixel(nCol, nRow, RGB(pColorTable->RGB[nTransparentIndex].Red,
																pColorTable->RGB[nTransparentIndex].Green,
																pColorTable->RGB[nTransparentIndex].Blue));
				}
				else
				{
					// Put the index value in place.
					int nColorIndex = pImageData[nCol];
					dcFrame.SetPixel(nCol, nRow, RGB(pColorTable->RGB[nColorIndex].Red,
																pColorTable->RGB[nColorIndex].Green,
																pColorTable->RGB[nColorIndex].Blue));
				}
			}
		}

		// Skip the width of the original image in bytes to get the next line.
		pImageData += sizeFrame.cx;
	}

	// Clean up the palette.
	dcFrame.SelectPalette( pOldFramePal, NULL );
	dcMask.SelectPalette( pOldMaskPal, NULL );
	palFrame.DeleteObject();

	// Clean up the memory devices.
	dcFrame.SelectObject( pOldFrameBMP );
	dcMask.SelectObject( pOldMaskBMP );
	dcFrame.DeleteDC();
	dcMask.DeleteDC();

	// Store our newly created bitmaps into the frame which will hold them until
	// we are destroyed and free up the list of frames.
	m_pGifFrame->SetBitmaps( pFrameBitmap, pMaskBitmap );
}
