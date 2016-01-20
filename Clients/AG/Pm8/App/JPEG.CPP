/*
// JPEG.CPP
//
// Routines for reading a JPEG file.
//
// $Header: /PM8/App/JPEG.CPP 1     3/03/99 6:06p Gbeddow $
//
// $Log: /PM8/App/JPEG.CPP $
// 
// 1     3/03/99 6:06p Gbeddow
// 
// 5     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 4     8/20/98 11:41a Dennis
// Type casted x_resolution assignments.
// 
// 3     8/20/98 11:02a Jayn
// New resolution overrides. Small license path change.
// 
// 2     8/18/98 10:38a Dennis
// Changed resolution to 300
// 
//    Rev 1.0   14 Aug 1997 15:21:58   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:56   Fred
// Initial revision.
// 
//    Rev 1.6   22 May 1997 16:13:16   Jay
// More new JPEG stuff.
// 
//    Rev 1.5   22 May 1997 10:30:24   Jay
// New Pegasus code
// 
//    Rev 1.4   11 Apr 1997 11:23:34   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.3   05 Dec 1996 12:30:14   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.2   25 Aug 1996 12:59:22   Jay
// No longer sets default name at low level.
// 
//    Rev 1.1   08 Aug 1996 18:10:56   Jay
// New JPEG compress and decompress classes
// 
//    Rev 1.0   16 May 1996 14:24:36   Jay
// JPEG
*/

#include	"stdafx.h"

#include <memory.h>

#include "pmgdb.h"
#include	"grafrec.h"
#include	"grafobj.h"
#include	"jpeg.h"
#include "jpegimpl.h"			// JPEG implementation classes.
#include "pegasus.h"

/////////////////////////////////////////////////////////////////////////////
// 

/////////////////////////////////////////////////////////////////////////////
// JPEGHelper

POINT JPEGHelper::m_pntResolution = { 300, 300 };

/*
// Read the JPEG helper info.
*/

ERRORCODE JPEGHelper::read(StorageDevicePtr pDevice)
{
	return pDevice->read_record(&m_Record, sizeof(m_Record));
}

/*
// Write the JPEG helper info.
*/

ERRORCODE JPEGHelper::write(StorageDevicePtr pDevice)
{
	return pDevice->write_record(&m_Record, sizeof(m_Record));
}

/*
// Size the JPEG helper info.
*/

ST_DEV_IO_SIZE JPEGHelper::size(StorageDevicePtr pDevice)
{
	return pDevice->size_record(sizeof(m_Record));
}

/*
// Initialize a JPEG file.
*/

ERRORCODE JPEGHelper::init(GRAPHIC_CREATE_STRUCT_PTR gcs)
{
	ERRORCODE error;

	// We need a file to read.

	StorageDevice* pSource;
	ReadOnlyFile file;

	if (gcs == NULL || (pSource = gcs->pSourceDevice) == NULL)
	{
		// Go directly to disk.
		file.set_name(graphic->m_csFileName);
		pSource = &file;
	}

#if 0
	CJPEGDecompressor Decompressor;

	if ((error = Decompressor.Init(pSource)) == ERRORCODE_None)
	{
		m_Record.m_wColorComponents = Decompressor.ColorComponents();

		graphic->record.x_size = Decompressor.ImageWidth();
		graphic->record.y_size = Decompressor.ImageHeight();

		graphic->record.x_resolution = 300;
		graphic->record.y_resolution = 300;
		graphic->record.storage = GRAPHIC_STORAGE_FILE;

		Decompressor.Abort();
	}
#else
	CPegasus Pegasus;
	CPegasusInputDevice Input(pSource);

	Pegasus.SetInput(&Input);
	Pegasus.AllocateGetQueue();
	if (Pegasus.FillInputQueue() == ERR_NONE)
	{
		long lType;
		if ((lType = Pegasus.QueryImageInfo()) == 0)
		{
			TRACE("Error getting image info\n");
			error = ERRORCODE_IllegalType;
		}
		else
		{
			PIC_PARM* pPicParm = Pegasus.GetPicParm();
			m_Record.m_wColorComponents = pPicParm->Head.biBitCount == 24 ? 3 : 1;

			graphic->record.x_size = (USHORT)pPicParm->Head.biWidth;
			long lHeight = (long)pPicParm->Head.biHeight;
			if (lHeight < 0)
			{
				lHeight = -lHeight;
			}
			graphic->record.y_size = (USHORT)lHeight;

			graphic->record.x_resolution = (USHORT) m_pntResolution.x;
			graphic->record.y_resolution = (USHORT) m_pntResolution.y;
			graphic->record.storage = GRAPHIC_STORAGE_FILE;

			error = ERRORCODE_None;
		}
	}
	else
	{
		error = ERRORCODE_Open;
	}
#endif

	return error;
}

/////////////////////////////////////////////////////////////////////////////
// JPEGDriver

/*
// The constructor for the JPEG driver.
*/

JPEGDriver::JPEGDriver(GraphicPtr g, StorageDevice* pSource /*=NULL*/)
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

	m_pDecompressor = NULL;
}

JPEGDriver::~JPEGDriver()
{
	delete m_pDecompressor;
}

/*
// The image_info() method for JPEG files.
*/

ERRORCODE JPEGDriver::image_info(IMAGE_INFO_PTR info)
{
	JPEGHelper* pHelper = (JPEGHelper*)graphic->my_little_helper();

	info->bpp = 8;
	info->planes = (char)pHelper->m_Record.m_wColorComponents;
	info->image_type = (info->planes == 1) ? IMAGE_GRAYSCALE : IMAGE_FULL_COLOR;
	// We now read JPEGs through the Pegasus library which returns data
	// in DIB format. Hence, we have reversed red and blue.
	info->red_index = 2;
	info->width = graphic->record.x_size;
	info->height = graphic->record.y_size;

	return ERRORCODE_None;
}

/*
// The open_image() method for JPEG files.
*/

ERRORCODE JPEGDriver::open_image(PALETTE_INFO_PTR palette)
{
	JPEGHelper* pHelper = (JPEGHelper*)graphic->my_little_helper();

	ERRORCODE error = ERRORCODE_None;

	TRY
	{
		m_pDecompressor = new CJPEGDecompressor;
		error = m_pDecompressor->Init(&file);
	}
#if 0
	CATCH(CJPEGException, e)
	{
		error = ERRORCODE_Read;
	}
#endif
	/*AND_*/CATCH(CErrorcodeException, e)
	{
		error = e->m_error;
	}
	AND_CATCH_ALL(e)
	{
		ASSERT(FALSE);
		error = ERRORCODE_IntError;
	}
	END_CATCH_ALL

	return error;
}

/*
// The read_block() method for JPEG files.
*/

ERRORCODE JPEGDriver::read_lines(int line, LPBYTE buffer, int count, LPBYTE pMask)
{
	ASSERT(pMask == NULL);

	if (m_pDecompressor == NULL)
	{
		return ERRORCODE_IntError;
	}

	return m_pDecompressor->ReadLines(line, buffer, count);
}

void JPEGDriver::close_image(VOID)
{
	m_pDecompressor->Finish();
}
