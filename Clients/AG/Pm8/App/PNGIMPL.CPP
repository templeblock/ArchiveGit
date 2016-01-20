/*
// PNG.CPP
//
// Routines for reading a PNG file.
//
// $Header: /PM8/App/PNGIMPL.CPP 1     3/03/99 6:10p Gbeddow $
//
// $Log: /PM8/App/PNGIMPL.CPP $
// 
// 1     3/03/99 6:10p Gbeddow
// 
// 10    2/08/99 3:33p Mwilson
// added routine to spilt png file int 2 dibs.  One for Alpha channel and
// one for color data.
// 
// 9     11/09/98 2:10p Dennis
// Added support for editing PNG graphics
// 
// 8     9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 7     8/20/98 11:41a Dennis
// Type casted x_resolution assignments.
// 
// 6     8/20/98 11:02a Jayn
// New resolution overrides. Small license path change.
// 
// 5     5/07/98 12:05p Jayn
// 
// 4     11/25/97 10:35a Johno
// Added CPNGWriter::WriteDIB - does "bottom up" dib write
// 
// 3     11/21/97 5:21p Johno
// General update
// 
// 2     11/19/97 5:32p Johno
// Added Jay's PNG writer code, modified for CStorageDevice
// 
//    Rev 1.0   17 Sep 1997 09:11:18   Jay
// Initial revision.
*/

#include	"stdafx.h"

#include <memory.h>

#include "pmgdb.h"
#include	"pngimpl.h"
#include	"util.h"

/////////////////////////////////////////////////////////////////////////////
// CPNGReader

//
// Constructor
//

CPNGReader::CPNGReader()
{
   m_pPNG = NULL;
   m_pInfo = NULL;
	m_pDevice = NULL;
	m_pRows = NULL;
	m_pRowMem = NULL;
}

//
// Destructor
//

CPNGReader::~CPNGReader()
{
	// Clean up after the read, and free any memory allocated.
	if (m_pPNG != NULL)
	{
		png_destroy_read_struct(&m_pPNG,
										m_pInfo == NULL ? NULL : &m_pInfo,
										(png_infopp)NULL);
	}
	delete [] m_pRows;
	Util::HugeFree(m_pRowMem);
}

//
// Initialize the reader.
//

ERRORCODE CPNGReader::Initialize(StorageDevice* pDevice, BOOL fReadInfo /*=TRUE*/)
{
	// Set the device used for reading.
	if ((m_pDevice = pDevice) == NULL)
	{
		ASSERT(FALSE);
		return ERRORCODE_BadParameter;
	}

	// Create the read structure.
	TRY
	{
		m_pPNG = png_create_read_struct(PNG_LIBPNG_VER_STRING,
												  (voidp)this,
												  OnError,
												  OnWarning);
	}
	END_TRY

	if (m_pPNG == NULL)
	{
		return ERRORCODE_Memory;
	}

	// Create the info structure.
	TRY
	{
		m_pInfo = png_create_info_struct(m_pPNG);
	}
	END_TRY
	if (m_pInfo == NULL)
	{
		return ERRORCODE_Memory;
	}

	// Set the read function.
	png_set_read_fn(m_pPNG,
						 (voidp)this,
						 OnRead);

	if (fReadInfo)
	{
		return ReadInfo();
	}

	return ERRORCODE_None;
}

//
// Read the info.
//

ERRORCODE CPNGReader::ReadInfo(void)
{
	// Read the info.
	TRY
	{
		png_read_info(m_pPNG, m_pInfo);
	}
	CATCH(CErrorcodeException, e)
	{
		return e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		return ERRORCODE_IntError;
	}
	END_CATCH_ALL

	return ERRORCODE_None;
}

// Static callback function.
void CPNGReader::OnError(png_structp pStruct, png_const_charp pszError)
{
	voidp pErrorData = png_get_error_ptr(pStruct);
	if (pErrorData != NULL)
	{
		((CPNGReader*)pErrorData)->ErrorFunction(pStruct, pszError);
	}
}

// Static callback function.
void CPNGReader::OnWarning(png_structp pStruct, png_const_charp pszWarning)
{
	voidp pErrorData = png_get_error_ptr(pStruct);
	if (pErrorData != NULL)
	{
		((CPNGReader*)pErrorData)->WarningFunction(pStruct, pszWarning);
	}
}

// Static callback function.
void CPNGReader::OnRead(png_structp pStruct, png_bytep pData, png_size_t nSize)
{
	voidp pIoData = png_get_io_ptr(pStruct);
	if (pIoData != NULL)
	{
		((CPNGReader*)pIoData)->ReadFunction(pStruct, pData, nSize);
	}
}

// Virtual callback function called by static one.
void CPNGReader::ErrorFunction(png_structp pStruct, png_const_charp pszError)
{
	TRACE("PNG Error: %s\n", pszError);
	ThrowErrorcodeException(ERRORCODE_Read);
}

// Virtual callback function called by static one.
void CPNGReader::WarningFunction(png_structp pStruct, png_const_charp pszWarning)
{
	TRACE("PNG Warning: %s\n", pszWarning);
}

// Virtual callback function called by static one.
void CPNGReader::ReadFunction(png_structp pStruct, png_bytep pData, png_size_t nSize)
{
	if (m_pDevice == NULL)
	{
		ThrowErrorcodeException(ERRORCODE_NotInitialized);
	}

	ERRORCODE error;
	if ((error = m_pDevice->read(pData, nSize)) != ERRORCODE_None)
	{
		ThrowErrorcodeException(error);
	}
}

//
// Allocate the rows we will be reading into.
//

ERRORCODE CPNGReader::AllocateRows(void)
{
   int nHeight = m_pPNG->height;
	TRY
		m_pRows = new LPBYTE[nHeight];
	END_TRY
	if (m_pRows == NULL)
	{
		return ERRORCODE_Memory;
	}

	// Strip 16-bit to 8-bit.
	if (m_pPNG->bit_depth == 16)
	{
		png_set_strip_16(m_pPNG);
	}

	// Update the info.
	png_read_update_info(m_pPNG, m_pInfo);

	// Allocate memory for the rows.
	DWORD dwRowBytes = png_get_rowbytes(m_pPNG, m_pInfo);
	DWORD dwSize = nHeight*dwRowBytes;
	m_pRowMem = (LPBYTE)Util::HugeAlloc(dwSize);

	// Set the row starts in the array.
#ifdef WIN32
	LPBYTE pRow = m_pRowMem;
#else
	BYTE __huge* pRow = (BYTE __huge*)m_pRowMem;
#endif
	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		m_pRows[nRow] = pRow;
		pRow += dwRowBytes;
	}
	return ERRORCODE_None;
}

//
// Read the image.
//

ERRORCODE CPNGReader::ReadImage(void)
{
	TRY
	{
		png_read_image(m_pPNG, m_pRows);
	}
	CATCH(CErrorcodeException, e)
	{
		return e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		return ERRORCODE_IntError;
	}
	END_CATCH_ALL

	return ERRORCODE_None;
}

//
// Return a pointer to a row.
//

LPBYTE CPNGReader::GetRow(int nRow)
{
	return m_pRows[nRow];
}

/////////////////////////////////////////////////////////////////////////////
// PNGHelper

// Global default resolution.
POINT PNGHelper::m_pntResolution = { 200, 200 };

/*
// Read the PNG helper info.
*/

ERRORCODE PNGHelper::read(StorageDevicePtr pDevice)
{
	return pDevice->read_record(&m_Record, sizeof(m_Record));
}

/*
// Write the PNG helper info.
*/

ERRORCODE PNGHelper::write(StorageDevicePtr pDevice)
{
	return pDevice->write_record(&m_Record, sizeof(m_Record));
}

/*
// Size the PNG helper info.
*/

ST_DEV_IO_SIZE PNGHelper::size(StorageDevicePtr pDevice)
{
	return pDevice->size_record(sizeof(m_Record));
}

/*
// Initialize a PNG file.
*/

ERRORCODE PNGHelper::init(GRAPHIC_CREATE_STRUCT_PTR gcs)
{
	ERRORCODE error = ERRORCODE_None;

	// We need a file to read.

	StorageDevice* pSource;
	ReadOnlyFile file;

	if (gcs == NULL || (pSource = gcs->pSourceDevice) == NULL)
	{
		// Go directly to disk.
		file.set_name(graphic->m_csFileName);
		pSource = &file;
	}

	CPNGReader Reader;

	if ((error = Reader.Initialize(pSource)) == ERRORCODE_None)
	{
		png_infop pInfo = Reader.GetInfo();
      TRACE("%d x %d; bd: %d, ct: %d, ",
         pInfo->width,
         pInfo->height,
         pInfo->bit_depth,
         pInfo->color_type);

		m_Record.m_wColorType = pInfo->color_type;
		m_Record.m_bBitDepth = pInfo->bit_depth;

		DWORD dwTotalSize = pInfo->height*pInfo->rowbytes;
      TRACE("Interlace: %d, Bytes: %ld\n", pInfo->interlace_type, dwTotalSize);

		graphic->record.x_size = (USHORT)pInfo->width;
		graphic->record.y_size = (USHORT)pInfo->height;

		graphic->record.x_resolution = (USHORT) m_pntResolution.x;
		graphic->record.y_resolution = (USHORT) m_pntResolution.y;
		graphic->record.storage = GRAPHIC_STORAGE_FILE;
	}

	return error;
}

/////////////////////////////////////////////////////////////////////////////
// PNGDriver

/*
// The constructor for the PNG driver.
*/

PNGDriver::PNGDriver(GraphicPtr g, StorageDevice* pSource /*=NULL*/)
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

	m_fRead = FALSE;
}

PNGDriver::~PNGDriver()
{
}

/*
// The image_info() method for PNG files.
*/

ERRORCODE PNGDriver::image_info(IMAGE_INFO_PTR info)
{
	PNGHelper* pHelper = (PNGHelper*)graphic->my_little_helper();

	info->bpp = pHelper->m_Record.m_bBitDepth;
   if (info->bpp == 16)
      info->bpp = 8;

	switch (pHelper->m_Record.m_wColorType)
	{
		case PNG_COLOR_TYPE_GRAY_ALPHA:
		{
			info->m_fHasMask = TRUE;
			info->m_fReverseMask = TRUE;
			info->m_nMaskBpp = info->bpp;
			// Fall through to...
		}
		case PNG_COLOR_TYPE_GRAY:
		{
			info->planes = 1;
			info->image_type = IMAGE_GRAYSCALE;
			break;
		}
		case PNG_COLOR_TYPE_PALETTE:
		{
			info->planes = 1;
			info->image_type = IMAGE_PALETTE;
      	info->red_index = 2;			/* Damn reversed thing! */
			break;
		}
		case PNG_COLOR_TYPE_RGB_ALPHA:
		{
			info->m_fHasMask = TRUE;
			info->m_fReverseMask = TRUE;
			info->m_nMaskBpp = info->bpp;
			// Fall through to...
		}
		case PNG_COLOR_TYPE_RGB:
		{
			info->planes = 3;
			info->image_type = IMAGE_FULL_COLOR;
			break;
		}
		default:
		{
			return ERRORCODE_IllegalType;
		}
	}
	info->width = graphic->record.x_size;
	info->height = graphic->record.y_size;

	return ERRORCODE_None;
}

/*
// The open_image() method for PNG files.
*/

ERRORCODE PNGDriver::open_image(PALETTE_INFO_PTR palette)
{
	PNGHelper* pHelper = (PNGHelper*)graphic->my_little_helper();

	ERRORCODE error;

	if ((error = m_Reader.Initialize(&file)) == ERRORCODE_None)
	{
		if (pHelper->m_Record.m_wColorType == PNG_COLOR_TYPE_PALETTE)
		{
			png_structp pPNG = m_Reader.GetStruct();
			png_infop pInfo = m_Reader.GetInfo();
			TRACE("Palette image\n");
			// We need to read the palette.
			if (png_get_valid(pPNG, pInfo, PNG_INFO_PLTE))
			{
				// Grab the palette from the struct.
				// The colors are in a different format, so we need to convert
				// each one.
				png_colorp pColors;
				int nColors;
				png_get_PLTE(pPNG, pInfo, &pColors, &nColors);
				if (nColors > palette->m_nColors)
				{
					ASSERT(FALSE);
					nColors = palette->m_nColors;
				}
				for (int nColor = 0; nColor < nColors; nColor++)
				{
					palette->m_pData[nColor].rgbtRed = pColors[nColor].red;
					palette->m_pData[nColor].rgbtGreen = pColors[nColor].green;
					palette->m_pData[nColor].rgbtBlue = pColors[nColor].blue;
				}
//				file.read(palette->m_pData, 256 * sizeof(RGBTRIPLE));
				palette->m_fRead = TRUE;
			}
			else
			{
				// No palette for palette image!
				ASSERT(FALSE);
			}
		}

		// Allocate an array for pointers to the rows.
		if ((error = m_Reader.AllocateRows()) != ERRORCODE_None)
		{
			return error;
		}

		// Have not read the rows yet.
		ASSERT(!m_fRead);
	}
	return error;
}

/*
// The read_block() method for PNG files.
*/

ERRORCODE PNGDriver::read_lines(int line, LPBYTE buffer, int count, LPBYTE pMask)
{
	PNGHelper* pHelper = (PNGHelper*)graphic->my_little_helper();
	BOOL fHasMask = (pHelper->m_Record.m_wColorType & PNG_COLOR_MASK_ALPHA);
	int nChannels = png_get_channels(m_Reader.GetStruct(), m_Reader.GetInfo());

	// If we have a mask to return, make sure we can.
	if (fHasMask)
	{
		nChannels--;				// Count not the mask channel.
	}

   // Make sure we have read the rows.
	if (!m_fRead)
	{
		m_fRead = TRUE;
		ERRORCODE error = m_Reader.ReadImage();
		if (error != ERRORCODE_None)
		{
			return error;
		}
	}

	// Extract the rows.
	for (int n = 0; n < count; n++)
	{
#ifdef WIN32
		LPBYTE pRow = m_Reader.GetRow(line++);
#else
		BYTE __huge* pRow = (BYTE __huge*)m_Reader.GetRow(line++);
#endif
		if (fHasMask)
		{
			LPBYTE pThisData = buffer;
			LPBYTE pThisMask = pMask;
			int nWidth = graphic->record.x_size;
			for (int x = 0; x < nWidth; x++)
			{
				for (int nChannel = 0; nChannel < nChannels; nChannel++)
				{
					*pThisData++ = *pRow++;
				}
//				*pThisMask++ = (*pRow++ == 0) ? 0 : 255;
				if(pMask)
					*pThisMask++ = (*pRow++ == 255) ? 255 : 0;
				else
					pRow++;
			}
			if(pMask)
				pMask += m_nMaskRowSize;
		}
		else
		{
			// Fast non-masked copy.
			Util::HMemCpy(buffer, pRow, source_row_size);
		}
		buffer += source_row_size;
	}

	return ERRORCODE_None;
}

void PNGDriver::close_image(VOID)
{
}

//
// The PNG writer.
//

//
// Constructor.
//

CPNGWriter::CPNGWriter()
{
   m_pPNG = NULL;
   m_pInfo = NULL;
   m_pdev = NULL;
}

//
// Destructor.
//

CPNGWriter::~CPNGWriter()
{
	// Clean up after the write, and free any memory allocated.
	if (m_pPNG != NULL)
	{
		png_destroy_write_struct(&m_pPNG,
										 m_pInfo == NULL ? NULL : &m_pInfo);
	}
}

//
// Initialize the writer with a Storage device
//

int CPNGWriter::Initialize (StorageDevice *pdev)
{
   m_pdev = pdev;
	// Create the read structure.
	TRY
	{
		m_pPNG = png_create_write_struct(PNG_LIBPNG_VER_STRING,
													(voidp)this,
													OnError,
													OnWarning);
	}
	END_TRY

	if (m_pPNG == NULL)
	{
		return ERRORCODE_Memory;
	}

	// Create the info structure.
	TRY
	{
		m_pInfo = png_create_info_struct(m_pPNG);
	}
	END_TRY
	if (m_pInfo == NULL)
	{
		return ERRORCODE_Memory;
	}

	// Set the write function.
	png_set_write_fn(m_pPNG,
						 (voidp)this,
						 OnWrite,
						 OnFlush);

	// Set the filter options to try.
   png_set_filter(m_pPNG,
						0,
						PNG_FILTER_NONE | PNG_FILTER_SUB | PNG_FILTER_PAETH);

	return ERRORCODE_None;
}

//
// Set the info for this image.
//

int CPNGWriter::SetInfo (int nChannels, int nRowBytes, int nWidth, int nHeight)
{
   if ((m_pdev == NULL) || (m_pPNG == NULL) || (m_pInfo == NULL))
	{
		ThrowErrorcodeException(ERRORCODE_NotInitialized);
	}

	int nType;
	switch (nChannels)
	{
		case 1:
		{
			nType = PNG_COLOR_TYPE_GRAY;
			break;
		}
		case 2:
		{
			nType = PNG_COLOR_TYPE_GRAY_ALPHA;
			break;
		}
		case 3:
		{
			nType = PNG_COLOR_TYPE_RGB;
			break;
		}
		case 4:
		{
			nType = PNG_COLOR_TYPE_RGB_ALPHA;
			break;
		}
	}
	m_nRowBytes = nRowBytes;

	png_set_IHDR(m_pPNG,
					 m_pInfo,
					 nWidth,
					 nHeight,
					 8,
					 nType,
					 PNG_INTERLACE_NONE,
					 PNG_COMPRESSION_TYPE_DEFAULT,
					 PNG_FILTER_TYPE_DEFAULT);
   	// Write the info.
   int error = ERRORCODE_None;
   TRY
   {
	   png_write_info(m_pPNG, m_pInfo);
   }
	CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		error = ERRORCODE_Write;
	}
	END_CATCH_ALL

   return error;
}

int CPNGWriter::WriteDIB (LPBYTE pData, BOOL UseBGR /*TRUE*/)
{
   if (UseBGR == TRUE)
      png_set_bgr (m_pPNG);

	// Allocate the rows for the data.
	LPBYTE* pRows = NULL;
   
   int nHeight = m_pPNG->height;
   BOOL  UpsideDown = (nHeight > 0) ? TRUE : FALSE;
   if (nHeight < 0)
      nHeight = -nHeight;

	TRY
		pRows = new LPBYTE[nHeight];
	END_TRY
	if (pRows == NULL)
	{
		return ERRORCODE_Memory;
	}

   LPBYTE pRow;
	// Set the row starts in the array.
	if (UpsideDown == TRUE)
   {
      pRow = pData + (Util::RoundToDWORD (m_nRowBytes) * (nHeight - 1));
	   for (int nRow = 0; nRow < nHeight; nRow++)
	   {
   		pRows[nRow] = pRow;
		   pRow -= Util::RoundToDWORD (m_nRowBytes);
	   }
   }
   else
   {
      pRow = pData;
	   for (int nRow = 0; nRow < nHeight; nRow++)
	   {
   		pRows[nRow] = pRow;
		   pRow += Util::RoundToDWORD (m_nRowBytes);
	   }
   }

	// Write the data.
	int error = ERRORCODE_None;
	TRY
	{
		png_write_image(m_pPNG, pRows);
		png_write_end(m_pPNG, m_pInfo);
	}
	CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		error = ERRORCODE_Write;
	}
	END_CATCH_ALL

	delete [] pRows;

	return error;
}

//
// Write the data.
//

int CPNGWriter::WriteData(LPBYTE pData, BOOL UseBGR /*TRUE*/)
{
   if (UseBGR == TRUE)
      png_set_bgr (m_pPNG);

	// Allocate the rows for the data.
	LPBYTE* pRows = NULL;
   int nHeight = m_pPNG->height;
	TRY
		pRows = new LPBYTE[nHeight];
	END_TRY
	if (pRows == NULL)
	{
		return ERRORCODE_Memory;
	}

	// Set the row starts in the array.
	LPBYTE pRow = pData;
	for (int nRow = 0; nRow < nHeight; nRow++)
	{
		pRows[nRow] = pRow;
		pRow += m_nRowBytes;
	}

	// Write the data.
	int error = ERRORCODE_None;
	TRY
	{
		png_write_image(m_pPNG, pRows);
		png_write_end(m_pPNG, m_pInfo);
	}
	CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		error = ERRORCODE_Write;
	}
	END_CATCH_ALL

	delete [] pRows;

	return error;
}

// Static callback function.
void CPNGWriter::OnError(png_structp pStruct, png_const_charp pszError)
{
	voidp pErrorData = png_get_error_ptr(pStruct);
	if (pErrorData != NULL)
	{
		((CPNGWriter*)pErrorData)->ErrorFunction(pStruct, pszError);
	}
}

// Static callback function.
void CPNGWriter::OnWarning(png_structp pStruct, png_const_charp pszWarning)
{
	voidp pErrorData = png_get_error_ptr(pStruct);
	if (pErrorData != NULL)
	{
		((CPNGWriter*)pErrorData)->WarningFunction(pStruct, pszWarning);
	}
}

// Static callback function.
void CPNGWriter::OnWrite(png_structp pStruct, png_bytep pData, png_size_t nSize)
{
	voidp pIoData = png_get_io_ptr(pStruct);
	if (pIoData != NULL)
	{
		((CPNGWriter*)pIoData)->WriteFunction(pStruct, pData, nSize);
	}
}

// Static callback function.
void CPNGWriter::OnFlush(png_structp pStruct)
{
	voidp pIoData = png_get_io_ptr(pStruct);
	if (pIoData != NULL)
	{
		((CPNGWriter*)pIoData)->FlushFunction(pStruct);
	}
}

// Virtual callback function called by static one.
void CPNGWriter::ErrorFunction(png_structp pStruct, png_const_charp pszError)
{
	TRACE("PNG Error: %s\n", pszError);
	ThrowErrorcodeException(ERRORCODE_Write);
}

// Virtual callback function called by static one.
void CPNGWriter::WarningFunction(png_structp pStruct, png_const_charp pszWarning)
{
	TRACE("PNG Warning: %s\n", pszWarning);
}

// Virtual callback function called by static one.
void CPNGWriter::WriteFunction(png_structp pStruct, png_bytep pData, png_size_t nSize)
{
   if (m_pdev == NULL)
	{
		ThrowErrorcodeException(ERRORCODE_NotInitialized);
	}

   ERRORCODE   e;
   if ((e = m_pdev->huge_write (pData, nSize)) != ERRORCODE_None)
      ThrowErrorcodeException (e); 
}

// Virtual callback function called by static one.
void CPNGWriter::FlushFunction(png_structp pStruct)
{
}

