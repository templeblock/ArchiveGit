/*
// TIFF.C
//
// TIFF file handling module.
//
// $Header: /PM8/App/TIFF.CPP 1     3/03/99 6:12p Gbeddow $
//
// $Log: /PM8/App/TIFF.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 2     9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:26:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:48   Fred
// Initial revision.
// 
//    Rev 1.4   28 Jul 1997 14:59:40   Jay
// Support for reiniting collection graphics and other LZW TIFF fixes.
// 
//    Rev 1.3   02 Jul 1997 14:25:54   Jay
// LZW is disabled in Classic version
// 
//    Rev 1.2   11 Apr 1997 11:23:40   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.1   25 Aug 1996 12:59:24   Jay
// No longer sets default name at low level.
// 
//    Rev 1.0   14 Mar 1996 13:42:54   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:18   FRED
// Initial revision.
// 
//    Rev 2.1   09 Mar 1995 08:27:02   JAY
//  
// 
//    Rev 2.0   07 Feb 1995 15:48:14   JAY
// Initial revision.
// 
//    Rev 1.8   09 Jan 1995 16:26:52   JAY
// New stuff (cut & paste, etc).
// 
//    Rev 1.7   30 Nov 1994 16:33:20   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.6   04 Aug 1994 07:32:38   JAY
// Large model and 32-bit changes
// 
//    Rev 1.5   07 Jul 1994 11:28:08   JAY
// Got rid of seek and tell methods in the helper.
// 
//    Rev 1.4   23 May 1994 08:21:54   JAY
// Now uses prep_storage_file() instead of the direct init so we can use files
// based on the document database device.
// 
//    Rev 1.3   13 May 1994 13:31:24   JAY
// Added "unbuffered_read" to ReadOnlyFile to help TIFF code with strip offsets.
// Fixed ReadOnlyFile::seek() to handle buffers > 32767.
// Increased size of TIFF read buffer.
// 
//    Rev 1.2   25 Jan 1994 15:18:32   JAY
// More
// 
//    Rev 1.1   05 Jan 1994 08:06:24   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:10   JAY
// 
*/

#include "stdafx.h"
#include "grafrec.h"
#include "tiff.h"
#include "pmwcfg.h"

/////////////////////////////////////////////////////////////////////////////
// TIFFHelper

WORD TIFFHelper::tiff_x_resolution = 300;
WORD TIFFHelper::tiff_y_resolution = 300;

/*
// Return the info structure for this helper.
*/

CTIFFInfo& TIFFHelper::Info(void)
{
	return m_Info;
}

/*
// Read the TIFF helper info.
*/

ERRORCODE TIFFHelper::read(StorageDevicePtr device)
{
	return m_Info.read(device);
}

/*
// Write the TIFF helper info.
*/

ERRORCODE TIFFHelper::write(StorageDevicePtr device)
{
	return m_Info.write(device);
}

/*
// Size the TIFF helper info.
*/

ST_DEV_IO_SIZE TIFFHelper::size(StorageDevicePtr device)
{
	return m_Info.size(device);
}

/*
// Initialize (and possibly read) a TIFF structure.
*/

ERRORCODE TIFFHelper::init(GRAPHIC_CREATE_STRUCT_PTR gcs)
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

/* Open the file and extract the info we want. */

	int nWidth, nHeight, nXRes, nYRes;

	if ((error = m_Info.Open(pSource, TRUE,
									 &nWidth, &nHeight,
									 &nXRes, &nYRes)) != ERRORCODE_None)
	{
		return error;
	}

	if (m_Info.m_Info.compression == TIFF_COMP_LZW
			&& !GetConfiguration()->SupportsLZW(FALSE))
	{
		return ERRORCODE_LZWNotSupported;
	}

	graphic->record.x_size;
	graphic->record.y_size;
	graphic->record.x_resolution;
	graphic->record.y_resolution;

	if (nXRes == 0)
	{
		nXRes = tiff_x_resolution;
		nYRes = tiff_y_resolution;
	}

	graphic->record.x_size = nWidth;
	graphic->record.y_size = nHeight;
	graphic->record.x_resolution = nXRes;
	graphic->record.y_resolution = nYRes;

	graphic->record.storage = GRAPHIC_STORAGE_FILE;

	m_Info.Close();

	return ERRORCODE_None;
}

/////////////////////////////////////////////////////////////////////////////
// TIFFDriver

TIFFDriver::TIFFDriver(GraphicPtr g, StorageDevice* pSource /*=NULL*/)
	: CTIFFImageDriver(&((TIFFHelper*)g->my_little_helper())->Info())
{
	m_fFailedPrep = FALSE;
	if (pSource == NULL)
	{
		if (g->prep_storage_file(&file) != ERRORCODE_None)
		{
			m_fFailedPrep = TRUE;
		}
	}
	else
	{
		file.set_based_device(pSource);
	}
}

ERRORCODE TIFFDriver::open_image(PALETTE_INFO_PTR palette)
{
	// If we failed the prep, do not check the compoession variable.
	// Just say we could not open.
	if (m_fFailedPrep)
	{
		return ERRORCODE_Open;
	}

	// Info should already be set from the helper.
	if (m_pInfo->m_Info.compression == TIFF_COMP_LZW
			&& !GetConfiguration()->SupportsLZW(FALSE))
	{
		return ERRORCODE_LZWNotSupported;
	}

	return CTIFFImageDriver::open_image(palette);
}
