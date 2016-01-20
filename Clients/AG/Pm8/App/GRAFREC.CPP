/*
// $Header: /PM8/App/GRAFREC.CPP 1     3/03/99 6:05p Gbeddow $
//
// Graphic data record routines.
//
// $Log: /PM8/App/GRAFREC.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 19    3/01/99 11:50a Lwilson
// Updated set_graphic_info to allow unknown data types.
// 
// 18    1/28/99 5:28p Gbeddow
// fixed bug in type_of_graphic() that prevented non-Print Shop vector
// graphics from being inserted from the art browser into the document
// 
// 17    1/11/99 1:32p Gbeddow
// support for reading/writing Print Shop square, row & column vector
// graphics in Pretzel collection files
// 
// 16    10/28/98 11:14a Mwilson
// fixed paste bug
// 
// 15    10/20/98 11:00a Mwilson
// added check for null pointer
// 
// 14    10/09/98 5:54p Jayn
// Another path manager fix.
// 
// 13    9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 12    8/20/98 11:41a Dennis
// Type casted x_resolution assignments.
// 
// 11    8/20/98 11:02a Jayn
// New resolution overrides. Small license path change.
// 
// 10    8/18/98 7:01p Jayn
// Made CreateImage work with palette images.
// 
// 9     8/10/98 3:47p Hforman
// Crop Copyright modifications
// 
// 8     3/26/98 10:09a Jayn
// Support for DPP (Serif) importing, previewing, etc.
// 
// 7     2/03/98 12:38p Hforman
// add type_of_art_extension(), get_extension()
// 
// 6     1/27/98 6:48p Hforman
// removed obsolete code
// 
// 5     1/20/98 8:54a Fredf
// Moved CTimeStamp from UTIL to TIMESTMP.
// 
// 4     1/19/98 1:20p Fredf
// Year 2000 compliance and exorcism of CCTL3D.
// 
// 3     1/15/98 3:57p Hforman
// remove "SHP" and "WPG", add "DIB" to type_of_graphic_extension()
// 
// 2     12/11/97 9:20a Jstoner
// vector to bitmap conversion for photo enhancer
// 
//    Rev 1.1   17 Sep 1997 09:11:44   Jay
// Bitmap rotation, PNG
// 
//    Rev 1.0   14 Aug 1997 15:21:14   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:38   Fred
// Initial revision.
// 
//    Rev 1.23   28 Jul 1997 14:59:38   Jay
// Support for reiniting collection graphics and other LZW TIFF fixes.
// 
//    Rev 1.22   27 Jun 1997 16:40:10   Jay
// Fixed memory overwrite.
// 
//    Rev 1.21   16 Jun 1997 14:27:42   Jay
// Won't call SetTimeStamp() if using source record (during assign)
// 
//    Rev 1.20   06 May 1997 17:18:20   dennisp
// Moved GetTimeStamp to Util.cpp
// 
//    Rev 1.19   01 May 1997 08:14:00   Jay
// MISSING graphics go to FILE.
// 
//    Rev 1.18   30 Apr 1997 12:29:58   Jay
// Can now determine type of embedded files
// 
//    Rev 1.17   14 Apr 1997 15:42:32   Jay
// Text box editing changes
// 
//    Rev 1.16   11 Apr 1997 14:55:40   Jay
// An assumption about row sizes wasn't valid. Compensated for it in the code.
// 
//    Rev 1.15   11 Apr 1997 11:23:30   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.14   06 Dec 1996 10:41:24   Jay
// Will now reinit a graphic even if from the cache.
// 
//    Rev 1.13   05 Nov 1996 14:06:50   Jay
// Got rid of warnings, etc.
// 
//    Rev 1.12   01 Nov 1996 17:11:38   Jay
// Got rid of a lot of warnings.
// 
//    Rev 1.11   01 Nov 1996 10:26:36   Jay
// Photoshop support
// 
//    Rev 1.10   18 Oct 1996 10:48:50   Jay
// Fixed a bug with image_type returning an error (prevented paste).
// 
//    Rev 1.9   10 Oct 1996 14:30:38   Jay
// More time stamps
// 
//    Rev 1.8   07 Oct 1996 17:02:36   Jay
// TimeStamp
// 
//    Rev 1.7   27 Sep 1996 10:05:40   Jay
// Fixed an error (width and height both set to width!).
// 
//    Rev 1.6   20 Sep 1996 13:41:20   Jay
// Support for Temp DIB graphic records.
// 
//    Rev 1.5   16 Sep 1996 17:22:48   Jay
// type_of_image()/type_of_graphic() can now return error result.
// 
//    Rev 1.4   25 Aug 1996 13:24:58   Jay
// Friendly names for pictures
// 
//    Rev 1.3   25 Aug 1996 12:59:58   Jay
// New friendly name code.
// 
//    Rev 1.2   16 May 1996 14:24:34   Jay
// JPEG
// 
//    Rev 1.1   10 May 1996 16:00:08   Jay
// New compression method.
// 
//    Rev 1.0   14 Mar 1996 13:46:34   Jay
// Initial revision.
// 
//    Rev 1.1   06 Mar 1996 09:54:50   Jay
// Changes in 2.1
// 
//    Rev 2.7   27 Feb 1996 16:44:26   JAY
// Type of file extension now uses dynamic portion of composite name.
// 
//    Rev 2.6   26 Feb 1996 16:30:08   JAY
// Now deletes pDriver when necessary.
// 
//    Rev 2.5   06 Oct 1995 11:46:38   JAY
// Reduced band size to 48000 (not 64000) for those bands which expanded when
// compressed.
// 
//    Rev 2.4   15 Sep 1995 14:04:18   JAY
// More 32-bit changes.
// 
//    Rev 2.3   18 Aug 1995 15:05:58   JAY
// Photo CD; graphic images, etc.
// 
//    Rev 2.2   07 Jul 1995 16:10:52   JAY
// Misc changes.
// 
//    Rev 2.1   29 Mar 1995 08:58:30   JAY
// Lots of changes
// 
//    Rev 2.0   07 Feb 1995 15:47:50   JAY
// Initial revision.
// 
//    Rev 1.16   09 Jan 1995 16:26:48   JAY
// New stuff (cut & paste, etc).
// 
//    Rev 1.15   30 Nov 1994 16:38:14   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.14   04 Aug 1994 07:34:02   JAY
// Large model and 32-bit changes
// 
//    Rev 1.13   15 Jul 1994 11:29:08   JAY
// Now NULLs 'name' and 'file_name' in delete_graphic_info() to prevent double
// free.
// 
//    Rev 1.12   13 Jul 1994 11:57:56   JAY
// New routines for setting name and reinit.
// 
//    Rev 1.11   07 Jul 1994 11:28:38   JAY
// Now frees the string returned by missing graphic handler.
// Now sets helper to NULL after it deletes it.
// 
//    Rev 1.10   10 Jun 1994 09:55:30   JAY
// Now only caches if 'proof'.
// Uses composite file name to parse file_name when generating default name.
// Doesn't explicitly release 'efile' during prep_file. It does it itself.
// 
//    Rev 1.9   09 Jun 1994 11:08:52   JAY
// Added hooks to GraphicCache in database.
// 
//    Rev 1.8   01 Jun 1994 16:35:14   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.7   31 May 1994 12:41:28   JAY
// Now calls the database's external prep before the existing internal ones.
// 
//    Rev 1.6   31 Mar 1994 16:03:54   JAY
// Added argument to missing picture handler.
// 
//    Rev 1.5   11 Mar 1994 14:23:42   JAY
// Additional code for WMF support.
// 
//    Rev 1.4   28 Feb 1994 10:50:50   JAY
// New arg to create_helper() to only allow Unknown or illegal types at certain
// (not general) times.
// 
//    Rev 1.3   25 Jan 1994 15:18:22   JAY
// More
// 
//    Rev 1.2   13 Jan 1994 17:00:14   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 08:05:42   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:24:58   JAY
// 
*/

#include "stdafx.h"

#include <string.h>

#include "pmw.h"
#include "pmgdb.h"
#include "compfn.h"
#include "util.h"

#include "grafrec.h"
#include "grafobj.h"
#include "efilerec.h"
#include "gcache.h"

#include "imagedef.h"
#include "db386.h"
#include "memdev.h"
#include "cdeflate.h"

#include "file.h"
#include "pcx.h"
#include "tiff.h"
#include "bmp.h"
#include "gif.h"
#include "halo.h"
#include "cgm.h"
#include "wpg.h"
#include "wmf.h"
#include "pcd.h"				// Photo CD
#include "jpeg.h"
#include "psd.h"
#include "pngimpl.h"

#include <windowsx.h>

/////////////////////////////////////////////////////////////////////////////
// GRAPHIC_CREATE_STRUCT

GRAPHIC_CREATE_STRUCT::GRAPHIC_CREATE_STRUCT()
{
	proof = TRUE;
	embed_source = FALSE;
	m_fCreateImage = FALSE;
	m_fSpecialCrop = FALSE;
	pSourceDevice = NULL;
	m_pTempDIB = NULL;
}

/*****************************************************************************/
/*                           Graphic record                                  */
/*****************************************************************************/

int GraphicRecord::m_nCopyrightCropHeight = 20;

/*
// The creator for a graphic record.
*/
ERRORCODE GraphicRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
	GraphicPtr graphic;

/* Create the new array record. */

	*record = NULL;

	if ((graphic = new GraphicRecord(number, type, owner, where)) == NULL)
	{
		return ERRORCODE_Memory;
	}

	ERRORCODE error = ERRORCODE_None;

	if (creation_data != NULL)
	{
		GRAPHIC_CREATE_STRUCT_PTR gcs = (GRAPHIC_CREATE_STRUCT_PTR)creation_data;

		error = graphic->set_graphic_info(gcs);
	}

	// If got an error, get rid of the graphic.
	if (error != ERRORCODE_None)
	{
		delete graphic;
		graphic = NULL;
	}

	// Pass back a pointer to whatever we ended up with.
	*record = (DatabaseRecordPtr)graphic;

	return error;
}

/*
// Get rid of the freeable info for this graphic record.
*/

VOID GraphicRecord::delete_graphic_info()
{
	m_csName.Empty();
	m_csFileName.Empty();

	if (helper != NULL)
	{
		delete helper;
		helper = NULL;
	}
}

/*
// Set the graphic's info from the graphic create struct passed.
*/

ERRORCODE GraphicRecord::set_graphic_info(GRAPHIC_CREATE_STRUCT_PTR gcs)
{
	ERRORCODE error = ERRORCODE_None;

	// Make sure we're clear.
	delete_graphic_info();

	// Copy the file name over.
	m_csName = gcs->m_csFriendlyName;
	m_csFileName = gcs->m_csFileName;

	// Set the graphic "friendly" name.
	if (m_csName.IsEmpty())
	{
		ERRORCODE error;
		// Set a default graphic name.
		if ((error = set_default_name()) != ERRORCODE_None)
		{
			return error;
		}
	}

	if (!gcs->m_csFileName.IsEmpty())
	{
		// Compute the type of graphic.
		record.type = type_of_graphic(gcs->m_csFileName, gcs->pSourceDevice);

		if ((int)record.type < 0)
		{
			// Some kind of error.
			return (ERRORCODE)record.type;
		}

	/* Create the helper. */
	/* Make the helper initialize. */

		if ((error = create_helper( TRUE )) == ERRORCODE_None && helper != NULL)
		{
			error = helper->init(gcs);
         if (gcs->pSourceDevice == NULL)
         {
			   SetTimeStamp();
         }
		}

		if (gcs->m_fSpecialCrop)
			record.flags |= FLAG_SpecialCrop;

	/*
	// If everything initialized correctly, see if we can stash the source
	// somewhere (in the document or a graphic cache).
	// We do not try and cache Photo CD images. They are huge, and we are unable
	// to use them once they're not accessible as a file.
	*/

		if ((error == ERRORCODE_None) && (record.type != GRAPHIC_TYPE_PCD))
		{
			if (gcs->m_fCreateImage)
			{
				CreateImage(gcs->pSourceDevice);
			}
			else if (gcs->embed_source)
			{
			/*
			// Embed our source.
			*/
				embed_source(gcs->pSourceDevice);
			}
			else
			{
			/*
			// Allow the database to cache the file for us if available.
			*/

				if (gcs->proof)
				{
					GraphicCachePtr gcache;

					if ((gcache = ((PMGDatabasePtr)database)->get_graphic_cache()) != NULL)
					{
						gcache->cache_graphic(gcs->m_csFileName);
					}
				}
			}
		}
	}
	else if (gcs->m_pTempDIB != NULL)
	{
		// We are initializing from a temporary DIB.
		record.type = GRAPHIC_TYPE_UNKNOWN;
		record.storage = GRAPHIC_STORAGE_TEMPDIB;

		// Fill the record out with the DIB info.
		record.x_size = (SHORT)gcs->m_pTempDIB->bmiHeader.biWidth;
		record.y_size = (SHORT)gcs->m_pTempDIB->bmiHeader.biHeight;
		record.x_resolution = (USHORT) BMPHelper::m_pntResolution.x;
		record.y_resolution = (USHORT) BMPHelper::m_pntResolution.y;
		m_pTempDIB = gcs->m_pTempDIB;
	}
	return error;
}

/*
// The constructor for a graphic record.
*/

GraphicRecord::GraphicRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where)
{
	memset(&record, 0, sizeof(record));

	helper = NULL;
	m_pTempDIB = NULL;
	m_fReinitedCached = FALSE;
}

/*
// The destructor for a graphic record.
*/

GraphicRecord::~GraphicRecord()
{
	delete_graphic_info();
}

/*
// Return the size of the bitmap for this graphic.
*/

ST_DEV_IO_SIZE GraphicRecord::bitmap_size(VOID)
{
	return ((record.x_size+7)/8) * record.y_size;
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE GraphicRecord::ReadData(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->read_record(&record, sizeof(record))) == ERRORCODE_None
			&& (error = device->read_cstring(m_csName)) == ERRORCODE_None
			&& (error = device->read_cstring(m_csFileName)) == ERRORCODE_None
			&& (error = create_helper(TRUE)) == ERRORCODE_None
 			&& (helper == NULL || (error = helper->read(device)) == ERRORCODE_None))
	{
		if (record.storage == GRAPHIC_STORAGE_MEMORY)
		{
			if ((error = device->read_block(bitmap_size(), &bitmap)) == ERRORCODE_Memory)
			{
			/* Memory error. That's OK. Switch to file instead. */
				record.storage = GRAPHIC_STORAGE_FILE;
				error = ERRORCODE_None;
			}
		}
		else if (record.storage == GRAPHIC_STORAGE_MISSING)
		{
			record.storage = GRAPHIC_STORAGE_FILE;
		}
	}

	// If the new time stamp is zero and the old time stamp is nonzero,
	// then initialize the new timestamp with the old timestamp.
	if (!record.TimeStamp.IsValid() && (record.dwOldTimeStamp != 0))
	{
		record.TimeStamp.Set((time_t)(record.dwOldTimeStamp));
	}

	// Always zero out the old time stamp.
	record.dwOldTimeStamp = 0;

	return error;
}

/*
// WriteData()
//
// Write the record.
*/

ERRORCODE GraphicRecord::WriteData(StorageDevicePtr device)
{
	ERRORCODE error;

	// We can not write these!
	ASSERT(record.storage != GRAPHIC_STORAGE_TEMPDIB);

	if ((error = device->write_record(&record, sizeof(record))) == ERRORCODE_None
			&& (error = device->write_cstring(m_csName)) == ERRORCODE_None
			&& (error = device->write_string(m_csFileName)) == ERRORCODE_None
			&& (helper == NULL || (error = helper->write(device)) == ERRORCODE_None))
	{
		if (record.storage == GRAPHIC_STORAGE_MEMORY)
		{
			error = device->write(bitmap, bitmap_size());
		}
	}

	return error;
}

/*
// SizeofData()
//
// Return the size of the record.
*/

ST_MAN_SIZE GraphicRecord::SizeofData(StorageDevicePtr device)
{
	ST_MAN_SIZE size = device->size_record(sizeof(record))
 						+ device->size_cstring(m_csName)
						+ device->size_cstring(m_csFileName);

/* Size the helper. */

	if (helper != NULL)
	{
		size += helper->size(device);
	}

	if (record.storage == GRAPHIC_STORAGE_MEMORY)
	{
		size += bitmap_size();
	}
	return size;
}

ERRORCODE GraphicRecord::create_helper(BOOL allow_unknown)
{
	if (helper != NULL)
	{
		delete helper;
		helper = NULL;
	}

	switch (record.type)
	{
		case GRAPHIC_TYPE_PCX:
		{
			helper = new PCXHelper(this);
			break;
		}
		case GRAPHIC_TYPE_TIFF:
		case GRAPHIC_TYPE_EPS:
		{
			helper = new TIFFHelper(this);
			break;
		}
		case GRAPHIC_TYPE_BMP:
		{
			helper = new BMPHelper(this);
			break;
		}
		case GRAPHIC_TYPE_GIF:
		{
			helper = new GIFHelper(this);
			break;
		}
		case GRAPHIC_TYPE_HALO:
		{
			helper = new HALOHelper(this);
			break;
		}
		case GRAPHIC_TYPE_CGM:
		{
			helper = new CGMHelper(this);
			break;
		}
		case GRAPHIC_TYPE_WPG:
		{
			helper = new WPGHelper(this);
			break;
		}
		case GRAPHIC_TYPE_WMF:
		{
			helper = new WMFHelper(this);
			break;
		}
		case GRAPHIC_TYPE_PCD:
		{
			helper = new PCDHelper(this);
			break;
		}
		case GRAPHIC_TYPE_JPEG:
		{
			helper = new JPEGHelper(this);
			break;
		}
		case GRAPHIC_TYPE_PSD:
		{
			helper = new PSDHelper(this);
			break;
		}
		case GRAPHIC_TYPE_PNG:
		{
			helper = new PNGHelper(this);
			break;
		}
		case GRAPHIC_TYPE_UNKNOWN:
		default:
		{
		/* This can happen with missing graphics from old document. */
			if (allow_unknown)
			{
				return ERRORCODE_None;
			}
			return ERRORCODE_IllegalType;
		}
	}
	return helper == NULL ? ERRORCODE_Memory : ERRORCODE_None;
}

static LPCSTR get_extension(LPCSTR name)
{
	int l;
	LPCSTR p;

	CompositeFileName cfn(name);
	name = cfn.get_virtual_name(TRUE);
	if(name != NULL)
	{
		if ((l = strlen(name)) < 5)
			return NULL;	// Can't have an extension

		p = name+l-3;

		if (*(p-1) != '.')
			return NULL;	// No extension
		else
		{
			static CString csExtension;
			csExtension = p;
			return csExtension;
		}
	}
	return NULL;
}

/*
// Return the type of a graphic based on its extension.
*/
PUBLIC GRAPHIC_TYPE
type_of_graphic_extension(LPCSTR name)
{
	static struct
	{
		CHAR				text[4];
		GRAPHIC_TYPE	type;
	} extension[] =
	{
/*		{ "SHP", GRAPHIC_TYPE_SHP}, */
		{ "TIF", GRAPHIC_TYPE_TIFF},
		{ "PCX", GRAPHIC_TYPE_PCX},
		{ "PCC", GRAPHIC_TYPE_PCX},
/*		{ "PIC", GRAPHIC_TYPE_LOTUS},*/
//		{ "POG", GRAPHIC_TYPE_POG},
//		{ "PNG", GRAPHIC_TYPE_PNG},
//		{ "CNG", GRAPHIC_TYPE_CNG},
//		{ "DAT", GRAPHIC_TYPE_DAT},
		{ "GIF", GRAPHIC_TYPE_GIF},
		{ "CUT", GRAPHIC_TYPE_HALO},
/*		{ "EPS", GRAPHIC_TYPE_EPS},*/
		{ "BMP", GRAPHIC_TYPE_BMP},
		{ "DIB", GRAPHIC_TYPE_BMP},
		{ "CGM", GRAPHIC_TYPE_CGM},
/*		{ "WPG", GRAPHIC_TYPE_WPG}, */
		{ "WMF", GRAPHIC_TYPE_WMF},
		{ "PCD", GRAPHIC_TYPE_PCD},
		{ "JPG", GRAPHIC_TYPE_JPEG},
		{ "PSD", GRAPHIC_TYPE_PSD},
		{ "PNG", GRAPHIC_TYPE_PNG},
		{ "PSG", GRAPHIC_TYPE_PS_SQUARE},	// PrintShop square vector graphic
		{ "PRG", GRAPHIC_TYPE_PS_ROW},		// PrintShop row vector graphic
		{ "PCG", GRAPHIC_TYPE_PS_COLUMN}		// PrintShop column vector graphic
	};

#define EXTENSION_COUNT	(sizeof(extension)/sizeof(extension[0]))

	LPCSTR ext = get_extension(name);
	if (ext == NULL)
		return GRAPHIC_TYPE_UNKNOWN;

	// Check all the extensions we know about.
	for (int i = 0; i < EXTENSION_COUNT; i++)
	{
		if (stricmp(ext, (LPSTR)extension[i].text) == 0)
		{
			return extension[i].type;	// Found it!
		}
	}

	return GRAPHIC_TYPE_UNKNOWN;
}

/*
// Return the type of a graphic.
*/

PUBLIC GRAPHIC_TYPE
type_of_graphic(LPCSTR name, StorageDevice* pSource /*=NULL*/)
{
	ReadOnlyFile file(name);

	if (pSource == NULL)
	{
		pSource = &file;
	}

/* Read some header bytes. */
	
	ST_DEV_POSITION Pos;
	BYTE buffer[16];
	ERRORCODE error;
	if ((error = pSource->tell(&Pos)) != ERRORCODE_None
		 || (error = pSource->seek(0, ST_DEV_SEEK_SET)) != ERRORCODE_None
		 || (error = pSource->read(buffer, sizeof(buffer))) != ERRORCODE_None
		 || (error = pSource->seek(Pos, ST_DEV_SEEK_SET)) != ERRORCODE_None)
	{
		return (GRAPHIC_TYPE)error;
	}

/*
// Test for various files.
// Tests are done in the order of certainty (i.e. tests that can uncondition-
// ally determine a file type are done first).
*/

/* Test for a WPG file. */

	if (buffer[0] == -1
			&& buffer[1] == 'W'
			&& buffer[2] == 'P'
			&& buffer[3] == 'C')
	{
	/* This is a WordPerfect file. See if it's a graphic. */
		if (buffer[9] == 0x16)
		{
			return GRAPHIC_TYPE_WPG;
		}
	/* Unknown WP file type. */
		return GRAPHIC_TYPE_UNKNOWN;
	}

/* Test for a TIFF file. */

	if ((buffer[0] == 'I' && buffer[1] == 'I')
			|| (buffer[0] == 'M' && buffer[1] == 'M'))
	{
		return GRAPHIC_TYPE_TIFF;
	}

/* Test for a GIF file. */

	if (buffer[0] == 'G' && buffer[1] == 'I' && buffer[2] == 'F')
	{
		return GRAPHIC_TYPE_GIF;
	}

/* Test for a BMP file. */

	if (buffer[0] == 'B' && buffer[1] == 'M')
	{
		return GRAPHIC_TYPE_BMP;
	}

	if (*(DWORD *)buffer == ALDUS_WMF_KEY)
	{
	/* Placeable Windows metafile. */
		return GRAPHIC_TYPE_WMF;
	}

/* Test for a PSD file. */

	if (buffer[0] == '8'
		 && buffer[1] == 'B'
		 && buffer[2] == 'P'
		 && buffer[3] == 'S'
		 && buffer[4] == 0
		 && buffer[5] == 1)
	{
		return GRAPHIC_TYPE_PSD;
	}

/* Test for a PNG file. */

	if (png_sig_cmp(buffer, 0, 8) == 0)
	{
		return GRAPHIC_TYPE_PNG;
	}

/* Test for PrintShop square vector graphic */
	if (strncmp((const char *)buffer, "PSDeluxe.PSG", 12) == 0)
		return GRAPHIC_TYPE_PS_SQUARE;

/* Test for PrintShop row vector graphic */
	if (strncmp((const char *)buffer, "PSDeluxe.PRG", 12) == 0)
		return GRAPHIC_TYPE_PS_ROW;

/* Test for PrintShop column vector graphic */
	if (strncmp((const char *)buffer, "PSDeluxe.PCG", 12) == 0)
		return GRAPHIC_TYPE_PS_COLUMN;

/* Just check for extensions now. */

	return type_of_graphic_extension(name);
}

/*
// Return the type of art (graphic or "pmo" or ...).
*/
PUBLIC GRAPHIC_TYPE
type_of_art_extension(LPCSTR name)
{
	LPCSTR ext = get_extension(name);

	if (ext == NULL)
		return GRAPHIC_TYPE_UNKNOWN;
	else if (stricmp(ext, "PMO") == 0)
		return GRAPHIC_TYPE_PMO;
	else if (stricmp(ext, "DPP") == 0)
		return GRAPHIC_TYPE_DPP;
	else
		return type_of_graphic_extension(name);
}

/*
// Set the default name from the file name.
*/

ERRORCODE GraphicRecord::set_default_name(VOID)
{
	LPSTR fn, s;
	CompositeFileName cfn(m_csFileName);

//	fn = (LPSTR)cfn.get_file_name();
	fn = (LPSTR)cfn.get_virtual_name(TRUE);				// But default to filename if none...

/* Copy the tail portion of the name to the node. */

	if(fn != NULL)
	{
		if ((s = strchr(fn, '\n')) == NULL)
		{
			LPSTR p;

		/* Not a library shape. Use the final name. */

			p = fn;

			if (p[0] != '\0' && p[1] == ':')
			{
			/* Skip the drive latter. */
				p += 2;
			}

		/* Go to the last slash in the name. */

			if ((s = strrchr(p, '\\')) == NULL)
			{
			/* No slash. Use the whole name. */
				s = p;
			}
			else
			{
			/* Move off of the slash. */
				s++;
			}
			set_name(s);
		}
		else
		{
		/* Library shape. Use just the shape part. */
			*s = '\0';
			set_name(fn);
			*s = '\n';
		}
	}
	return ERRORCODE_None;
}

/*
// What to do if a graphic can't be opened.
// This always returns the NoFile state. Check "storage" to see if it's
// missing or not.
*/

UpdateStatePtr GraphicRecord::handle_open_error(UpdateStatePtr ustate)
{
/* Mark this graphic data as being used. */

	modified();

/* Assume the worst. init_graphic() will set this if successful. */

	record.storage = GRAPHIC_STORAGE_MISSING;

	GRAPHIC_CREATE_STRUCT gcs;

	int16 index = 0;

	for (;;)
	{
		if (((PMGDatabasePtr)database)->handle_missing_graphic(this, &gcs, &index))
		{
		/*
		// They picked a graphic.
		// The string in the "gcs" needs to be freed.
		*/

			ERRORCODE error;

			error = set_graphic_info(&gcs);

			if (error == ERRORCODE_None)
			{
				return graphic_nofile_state(ustate);
			}
		}
		else
		{
		/* Some other error happened. Redraw as missing. */
			return graphic_nofile_state(ustate);
		}
	}
}

/*
// Assign method.
*/

ERRORCODE GraphicRecord::assign(DatabaseRecordRef srecord)
{
	ERRORCODE error;
/* Assign the base record first. */
	if ((error = DatabaseRecord::assign(srecord)) == ERRORCODE_None)
	{
		GraphicRecord& Record = (GraphicRecord&)srecord;

		m_csName = Record.m_csName;
		m_csFileName = Record.m_csFileName;
		bitmap = NULL;
		record = Record.record;
		record.reference_count = 1;

	/* Duplicate the embedded source if there is one. */

		record.source = 0;
		if (Record.record.source != 0)
		{
		/* Do the dup. */
			EmbeddedFileRecord* pEFile;

			if ((pEFile = (EmbeddedFileRecord*)Record.database->get_record(Record.record.source, NULL, RECORD_TYPE_EmbeddedFile)) != NULL)
			{
			/* We got the embedded file. Duplicate it over. */
				EmbeddedFileRecord* pNewEFile = (EmbeddedFileRecord*)pEFile->duplicate(database);
				if (pNewEFile != NULL)
				{
					record.source = pNewEFile->Id();
					pNewEFile->release();
				}
				pEFile->release();
			}
		}

	/* Duplicate the embedded image if there is one. */

		record.image = 0;
		if (Record.record.image != 0)
		{
		/* Do the dup. */
			EmbeddedFileRecord* pEFile;

			if ((pEFile = (EmbeddedFileRecord*)Record.database->get_record(Record.record.image, NULL, RECORD_TYPE_EmbeddedFile)) != NULL)
			{
			/* We got the embedded file. Duplicate it over. */
				EmbeddedFileRecord* pNewEFile = (EmbeddedFileRecord*)pEFile->duplicate(database);
				if (pNewEFile != NULL)
				{
					record.image = pNewEFile->Id();
					pNewEFile->release();
				}
				pEFile->release();
			}
		}

	/* Create the helper for this guy. */

		if ((error = create_helper()) == ERRORCODE_None)
		{
			ReadOnlyFile File;

			if ((error = prep_storage_file(&File, FALSE)) == ERRORCODE_None)
			{
				GRAPHIC_CREATE_STRUCT gcs;
				gcs.pSourceDevice = &File;
				error = helper->init(&gcs);
// PMGTODO: The following line was commented out. Why?
//				SetTimeStamp();
				record.TimeStamp = Record.record.TimeStamp;
			}
		}
	}
	return error;
}

/*
// Destroy method.
*/

void GraphicRecord::destroy(void)
{
	if (record.source != 0)
	{
		database->delete_record(record.source, RECORD_TYPE_EmbeddedFile);
		record.source = 0;
	}

	if (record.image != 0)
	{
		database->delete_record(record.image, RECORD_TYPE_EmbeddedFile);
		record.image = 0;
	}

	if (record.storage == GRAPHIC_STORAGE_BITMAP)
	{
		/*
		//	Free the associated low resolution bitmap
		*/
		if (record.number != 0)
		{
			database->delete_record(record.number);
			record.number = 0;
		}
	}
}

/*
// Embed this graphic's source in the document database.
*/

ERRORCODE GraphicRecord::embed_source(StorageDevice* pSource /*=NULL*/)
{
	if (record.type == GRAPHIC_TYPE_PCD)
	{
	/* We don't embed this one. */
		return ERRORCODE_None;
	}

	PMGDatabasePtr pmgdb = (PMGDatabasePtr)database;

/*
// Handle any old source.
*/

	if (record.source != 0)
	{
		pmgdb->delete_record(record.source, RECORD_TYPE_EmbeddedFile);
		record.source = 0;
	}

/*
// Create the embedded file.
*/

	EmbeddedFilePtr efile;

	if (pSource == NULL)
	{
		efile = pmgdb->create_embedded_file_record((VOIDPTR)(LPCSTR)m_csFileName);
	}
	else
	{
	/* Embed directly from source. */

		if ((efile = pmgdb->create_embedded_file_record(NULL)) != NULL)
		{
			ERRORCODE error;

			if ((error = efile->embed_file(*pSource)) != ERRORCODE_None)
			{
			/* We failed. */

				DB_RECORD_NUMBER id = efile->Id();
				efile->release();

				pmgdb->delete_record(id, RECORD_TYPE_EmbeddedFile);
				return error;
			}
		}
	}

	if (efile == NULL)
	{
		return pmgdb->last_creation_error();
	}

/* Rememeber the ID. */

	record.source = efile->Id();

/* Release the embedded file. */

	efile->release();

/* We have been modified. */

	modified();

	return ERRORCODE_None;
}

/*
// Create an image driver usable to read lines of bitmap data.
// This only works on bitmap formats (TIFF, GIF, etc).
*/

ImageDriver* GraphicRecord::CreateImageDriver(BOOL fUseImage /*=TRUE*/, StorageDevice* pSource /*=NULL*/)
{
	ImageDriver* pDriver = NULL;

	if (fUseImage && record.image != NULL)
	{
		GraphicImageDriver* pIDriver = new GraphicImageDriver(this);
		if (pIDriver->Init())
		{
			return pIDriver;
		}
		// Forget this route.
		delete pIDriver;
	}

	switch (record.type)
	{
		case GRAPHIC_TYPE_PCX:
		{
			pDriver = new PCXDriver(this, pSource);
			break;
		}
		case GRAPHIC_TYPE_TIFF:
		case GRAPHIC_TYPE_EPS:
		{
			pDriver = new TIFFDriver(this, pSource);
			break;
		}
		case GRAPHIC_TYPE_BMP:
		{
			pDriver = new BMPDriver(this, pSource);
			break;
		}
		case GRAPHIC_TYPE_GIF:
		{
			pDriver = new GIFDriver(this, pSource);
			break;
		}
		case GRAPHIC_TYPE_HALO:
		{
			pDriver = new HALODriver(this, pSource);
			break;
		}
		case GRAPHIC_TYPE_PCD:
		{
			ASSERT(pSource == NULL);
			pDriver = new PCDDriver(this);
			break;
		}
		case GRAPHIC_TYPE_JPEG:
		{
			pDriver = new JPEGDriver(this, pSource);
			break;
		}
		case GRAPHIC_TYPE_PSD:
		{
			pDriver = new PSDDriver(this, pSource);
			break;
		}
		case GRAPHIC_TYPE_PNG:
		{
			pDriver = new PNGDriver(this, pSource);
			break;
		}
		default:
		{
			break;
		}
	}
	return pDriver;
}

/*
// Create an image for this graphic.
*/

ERRORCODE GraphicRecord::CreateImage(StorageDevice* pSource /*=NULL*/)
{
	if (record.image != 0)
	{
	/* Already made. No more to do. */
		return ERRORCODE_None;
	}

/*
// We need to build the embedded file. This will be sourced by a memory
// device that we will construct with bands of compressed image.
// The entire image must fit in memory.
*/

	// Create the image driver we will be using.
	// We do *not* want to use the image if we have one.

	ImageDriver* pDriver = CreateImageDriver(FALSE, pSource);
	if (pDriver == NULL)
	{
		return ERRORCODE_IllegalType;
	}

/* Get the image info from the driver. */

	ERRORCODE error;
	IMAGE_INFO Info;

	if ((error = pDriver->info(&Info)) != ERRORCODE_None)
	{
		delete pDriver;
		return error;
	}

#if 0
/*
// For now, we only support 24-bit images.
// This is a simple implementation for supporting Photo CD. It should be
// extended given more time to support our 8-bit brothers as well. This would
// involve extending the format to include palette information.
*/

	if (Info.image_type != IMAGE_FULL_COLOR)
	{
		delete pDriver;
		return ERRORCODE_IllegalType;
	}
#endif

	int nSourceRowBytes = Info.row_bytes;

	// Bit-depths less than 8 bpp are already upgraded to 8.
	if (Info.bpp < 8)
	{
		Info.bpp = 8;
		Info.row_bytes = Info.width;
	}

	LPBYTE pSourceRow = NULL;
	if (Info.row_bytes != nSourceRowBytes)
	{
		ASSERT(Info.row_bytes < nSourceRowBytes);
		pSourceRow = new BYTE[nSourceRowBytes];
	}

/*
// Compute:
//		bytes in a row (as we store it)
//		data size
//		band size
//		number of bands
*/

	// DIB pad the data. This is how we store it.
	WORD wBandSize = 48000;		// Maximum band size.

	WORD wRowBytes = Info.row_bytes;
	DWORD dwDataSize = (DWORD)wRowBytes*(DWORD)Info.height;
	WORD wBandHeight;
	WORD wBandCount;

	if (dwDataSize <= wBandSize)
	{
		wBandHeight = Info.height;
		wBandCount = 1;
		wBandSize = (WORD)dwDataSize;
	}
	else
	{
		wBandHeight = wBandSize/wRowBytes;
		wBandCount = (Info.height + wBandHeight-1)/wBandHeight;
		wBandSize = wRowBytes*wBandHeight;
		ASSERT(wBandCount != 1);
	}

	PALETTE_INFO PaletteInfo;
	if (Info.image_type != IMAGE_FULL_COLOR)
	{
		PaletteInfo.m_nColors = Info.colors;
		ASSERT(PaletteInfo.m_pData == NULL);
		if (PaletteInfo.m_nColors > 256)
		{
			// This currently doesn't make sense.
			ASSERT(FALSE);
			return ERRORCODE_IllegalType;
		}
		TRY
		{
			PaletteInfo.m_pData = new RGBTRIPLE[PaletteInfo.m_nColors];
		}
		END_TRY
		if (PaletteInfo.m_pData == NULL)
		{
			// This shouldn't happen!
			ASSERT(FALSE);
			return ERRORCODE_Memory;
		}
	}

	if ((error = pDriver->open(&PaletteInfo)) == ERRORCODE_None)
	{

	/*
	// We need a memory device. This is what we will use as the source for
	// our embedding.
	*/

		MemoryDevice Device(TRUE);				// Will free the pieces.
		CDeflate Deflator;						// Our compressor.

		HGLOBAL hBand = NULL;
		LPBYTE pBand = NULL;

		TRY
		{
		/*
		// Allocate our header structure.
		*/
			UINT uHeaderSize = sizeof(CGraphicImageHeader)+wBandCount*sizeof(DWORD);
			CGraphicImageHeader* pHeader = (CGraphicImageHeader*)(new BYTE[uHeaderSize]);

			pHeader->m_dwBandCount = wBandCount;
			pHeader->m_dwBandSize = wBandSize;
			pHeader->m_dwBandHeight = wBandHeight;
			pHeader->m_wFlags = CGraphicImageHeader::FLAG_DeflateCompression;
			pHeader->m_Info = Info;

		/*
		// Add the header to the device.
		// It will be freed by the device's detructor now.
		*/

			Device.AddPiece(pHeader, uHeaderSize);

		/*
		// Initialize the starting offset for the data bands.
		// If we had palette info (or other info), we could insert it between
		// the header and the first data band. For now, though, we just start
		// the data right after the header.
		*/

			DWORD dwOffset = uHeaderSize;

		//
		// If we have a palette, write it now.
		//
			if (Info.image_type != IMAGE_FULL_COLOR)
			{
				// We have a palette to write.
				// The written palette will always be 256 colors, even though the
				// source palette may be smaller (since the data has been upgraded
				// to 8-bit).
				int nSize = 256*sizeof(RGBTRIPLE);
				ASSERT(PaletteInfo.m_nColors <= 256);
				BYTE* pPalette = new BYTE[nSize];
				memcpy(pPalette, PaletteInfo.m_pData, PaletteInfo.m_nColors*sizeof(RGBTRIPLE));

				Device.AddPiece(pPalette, nSize);

				// Offset to where the data should start.
				dwOffset += sizeof(RGBTRIPLE)*256;
			}

		/*
		// Allocate our uncompressed band. We only need one.
		*/
			hBand = ::GlobalAlloc(GMEM_MOVEABLE, wBandSize);
			if (hBand == NULL
				 || (pBand = (LPBYTE)::GlobalLock(hBand)) == NULL)
			{
				AfxThrowMemoryException();
			}

		/*
		// Now we need to march down the bands.
		*/

			int nLine = 0;			// Start at the top.
			WORD wThisBandHeight = wBandHeight;
			WORD wThisBandSize = wBandSize;
			for (WORD wBand = 0; wBand < wBandCount; wBand++)
			{
				// Handle the last band.
				if ((int)(nLine + wThisBandHeight) >= (int)Info.height)
				{
					wThisBandHeight = Info.height - nLine;
					wThisBandSize = wThisBandHeight*wRowBytes;
				}

			/* Read the band data into the buffer. */

				TRACE(">>> Read %d (%u)\n", nLine, wThisBandHeight);

				if (pSourceRow == NULL)
				{
					error = pDriver->read(nLine, pBand, wThisBandHeight);
					nLine += wThisBandHeight;
				}
				else
				{
					// Read it one line at a time since the source is larger.
					LPBYTE pDestRow = pBand;
					for (WORD w = 0; w < wThisBandHeight; w++)
					{
						if ((error = pDriver->read(nLine, pSourceRow, 1)) != ERRORCODE_None)
						{
							break;
						}
						memcpy(pDestRow, pSourceRow, Info.row_bytes);
						pDestRow += Info.row_bytes;
						nLine++;
					}
				}

				if (error != NULL)
				{
					break;
				}

			/*
			// Compute the compressed size of the data.
			*/

				DWORD dwCompressedSize = 0;
				LPBYTE pData = NULL;

				Deflator.Compress(pBand, wThisBandSize, &pData, &dwCompressedSize);
				if (pData == NULL)
				{
					error = ERRORCODE_Memory;
					break;
				}

				TRACE("Compress %u to %lu\n", wThisBandSize, dwCompressedSize);
			
			/*
			// Now we have our size.
			// Allocate it.
			*/
				LPBYTE pCompressedData = new BYTE[dwCompressedSize];

				memcpy(pCompressedData, pData, (size_t)dwCompressedSize);
				CDeflate::FreeData(pData);

				// Add this piece to the device.
				Device.AddPiece(pCompressedData, dwCompressedSize);

			/*
			// Set the offset for this band in the header.
			*/

				pHeader->m_dwOffset[wBand] = dwOffset;
				dwOffset += dwCompressedSize;
			}
			// Save the last offset (which is past all bands).
			pHeader->m_dwOffset[wBand] = dwOffset;

			if (error == ERRORCODE_None)
			{
			/*
			// Create the embedded file.
			*/

				PMGDatabasePtr pDatabase = (PMGDatabasePtr)database;

				EmbeddedFilePtr efile;

				if ((efile = pDatabase->create_embedded_file_record(NULL)) == NULL)
				{
					error = pDatabase->last_creation_error();
				}
				else
				{
					ERRORCODE error;

					if ((error = efile->embed_file(Device)) == ERRORCODE_None)
					{
					/* Remember the ID. */

						record.image = efile->Id();

						TRACE("Embedded file: start is %ld, size is %ld\n",
								efile->get_data_start(),
								efile->get_data_size());

					/* Release the embedded file. */

						efile->release();

					/* We have been modified. */

						modified();
					}
					else
					{
					/* We failed. */

						DB_RECORD_NUMBER id = efile->Id();
						efile->release();

						pDatabase->delete_record(id, RECORD_TYPE_EmbeddedFile);
					}
				}
			}

			::GlobalUnlock(hBand);
			::GlobalFree(hBand);
		}
		CATCH_ALL(e)
		{
			if (hBand != NULL)
			{
				if (pBand != NULL)
				{
					::GlobalUnlock(hBand);
				}
				::GlobalFree(hBand);
			}
			delete [] pSourceRow;
			delete [] pBand;
			delete pDriver;
			delete [] PaletteInfo.m_pData;
		// Assume memory error.
			return ERRORCODE_Memory;
		}
		END_CATCH_ALL
		pDriver->close();
	}
	delete [] pSourceRow;
	delete pDriver;
	delete [] PaletteInfo.m_pData;
	return error;
}

/*
// Write the image data for this object as BMP data.
*/

ERRORCODE GraphicRecord::WriteImageAsBMP(StorageDevice* pDevice)
{
	ERRORCODE error;

	// Create an image driver we can use to extract the data.
	ImageDriver* pDriver = CreateImageDriver(TRUE);
	if (pDriver == NULL)
	{
		return ERRORCODE_IllegalType;
	}

	PALETTE_INFO Palette;

	RGBQUAD* pDIBColors = NULL;
	LPBYTE pData = NULL;

	TRY
	{
		// Get the image info so we can build the BMP header.

		IMAGE_INFO Info;
		if ((error = pDriver->info(&Info)) == ERRORCODE_None)
		{
			int nRedIndex = Info.red_index;
			int nBlueIndex = nRedIndex ^ 2;
         int nDIBColors = 0;

			// Setup the palette for reading.
			if (Info.image_type != IMAGE_FULL_COLOR)
			{
				// Assign how many colors we have.
				Palette.m_nColors = Info.colors;

				ASSERT(Palette.m_nColors > 0);

            nDIBColors = Palette.m_nColors < 256 ? 256 : Palette.m_nColors;
				pDIBColors = new RGBQUAD[nDIBColors];
				Palette.m_pData = new RGBTRIPLE[Palette.m_nColors];
			}

			if ((error = pDriver->open(&Palette)) == ERRORCODE_None)
			{
				// Convert the palette over.
				if (Palette.m_pData != NULL)
				{
					if (!Palette.m_fRead)
					{
						Palette.BuildDefaultPalette(Info);
					}

					RGBQUAD* pDIBColor = pDIBColors;
					ASSERT(pDIBColor != NULL);
					RGBTRIPLE* pRGB = Palette.m_pData;
					ASSERT(pRGB != NULL);

					for (int nColor = 0; nColor < nDIBColors; nColor++)
					{
						pDIBColor->rgbRed = ((UCHAR *)pRGB)[nRedIndex];
						pDIBColor->rgbGreen = ((UCHAR *)pRGB)[1];
						pDIBColor->rgbBlue = ((UCHAR *)pRGB)[nBlueIndex];
						pDIBColor->rgbReserved = 0;
						pDIBColor++;
						pRGB++;
					}
				}

				// Got the info. Build the BMP header.
				BITMAPINFOHEADER bmi;

				int nColorBits = (int)((Info.bpp < 8) ? 8 : (Info.bpp*(int)Info.planes));
				// Init the bitmap info header.
				InitBitmapInfoHeader(&bmi, Info.width, Info.height, nColorBits);

				// Compute the size of the BMP data.
				DWORD nByteWidth = (bmi.biWidth*bmi.biBitCount+7)/8;
				nByteWidth = (nByteWidth + 3) & ~3;
				DWORD dwSizeImage = (DWORD)nByteWidth*bmi.biHeight;
				ASSERT(dwSizeImage == bmi.biSizeImage);

				// Compute the size of the palette.
				int nPaletteSize = nDIBColors*sizeof(RGBQUAD);

				// Init the file header.
				BITMAPFILEHEADER Header;

				DWORD dwHeaderSize = sizeof(Header) + bmi.biSize + nPaletteSize;

				Header.bfType = (SHORT)'B' + (((SHORT)'M') << 8);
				Header.bfSize = dwHeaderSize + dwSizeImage;
				Header.bfReserved1 = 0;
				Header.bfReserved2 = 0;
				Header.bfOffBits = dwHeaderSize;

				// Begin writing!
				if ((error = pDevice->write(&Header, sizeof(Header))) == ERRORCODE_None
					 && (error = pDevice->write(&bmi, sizeof(bmi))) == ERRORCODE_None
					 && (nPaletteSize == 0
							|| (error = pDevice->write(pDIBColors, nPaletteSize)) == ERRORCODE_None))
				{
					// Wrote the header. Write the data.
					pData = (LPBYTE)GlobalAllocPtr(GMEM_MOVEABLE, dwSizeImage);
					if (pData == NULL)
					{
						AfxThrowMemoryException();
					}
					// Read the lines.
					int nLines = (int)bmi.biHeight;
					int nRowDelta = nByteWidth;
#ifdef WIN32
					LPBYTE pRow = pData;
#else
					BYTE huge* pRow = (BYTE huge*)pData;
#endif
					if (!Info.do_yflip)
					{
						// Normal unflipped case: start at last line and work
						// to first line. BMPs are upside-down.
						nRowDelta = -nRowDelta;
						pRow += (long)(nLines-1)*(long)nByteWidth;
					}

					// Allocate a temporary row. The source row may be a different
					// size (larger) than what our destination byte width is (e.g.
					// monochrome bitmaps which round up to a multiple of 8).

					int nRowBytes = ((Info.row_bytes + 3) & ~3);
					LPBYTE pThisRow = new BYTE[nRowBytes];
               int nCopyBytes = Info.row_bytes < (int)nByteWidth ? nRowBytes : (int)nByteWidth;
					BOOL fConvertRGB = (Info.image_type == IMAGE_FULL_COLOR)
												&& nRedIndex == 0;

					for (int nLine = 0; nLine < nLines; nLine++)
					{
						// Read into our temp row.
						if ((error = pDriver->read(nLine, pThisRow, 1)) != ERRORCODE_None)
						{
							break;
						}
						// Copy to the data.
						if (fConvertRGB)
						{
							// RGB format with normal RGB order. Swap to make
							// BMP format happy.
							int nPixels = nCopyBytes/3;
							LPBYTE pSrc = pThisRow;
#ifdef WIN32
							LPBYTE pDest = pRow;
#else
							BYTE huge* pDest = pRow;
#endif
							for (int n = 0; n < nPixels; n++)
							{
								pDest[0] = pSrc[2];
								pDest[1] = pSrc[1];
								pDest[2] = pSrc[0];
								pSrc += 3;
								pDest += 3;
							}
						}
						else
						{
#ifdef WIN32
							memcpy(pRow, pThisRow, nCopyBytes);
#else
							hmemcpy(pRow, pThisRow, nCopyBytes);
#endif
						}
						pRow += nRowDelta;
					}
					delete [] pThisRow;

					// Write the lines to the file.
					if (error == ERRORCODE_None)
					{
						error = pDevice->huge_write(pData, dwSizeImage);
					}
				}
			}
		}
	}
	CATCH_ALL(e)
	{
		error = ERRORCODE_Memory;
	}
	END_CATCH_ALL

	// Clean up.
	if (pData != NULL)
	{
		GlobalFreePtr(pData);
	}
	delete [] pDIBColors;
	delete [] Palette.m_pData;
	delete pDriver;

	return error;
}

/*
// Set the name for this graphic record.
*/

void GraphicRecord::set_name(LPCSTR pNewName)
{
	m_csName = pNewName;
}

/*
// Re-initialize the graphic.
// This is typically called at document load time.
*/

ERRORCODE GraphicRecord::reinit(BOOL fCheckTimeStamp /*=FALSE*/)
{
	// Any cached images are assumed to be reinited at this point.
	m_fReinitedCached = TRUE;

	// Do the reinit.
	if (record.source == 0 && record.image == 0)
	{
		// Not embedded and no image. Init the helper with the graphic file.
		// If we want this to be conditional on the time stamp, check it now.

		if (fCheckTimeStamp)
		{
			CTimeStamp TimeStamp = GetTimeStamp();
			SetTimeStamp();
			if (TimeStamp == GetTimeStamp())
			{
				// Time stamp the same. No need to reinit!
				return ERRORCODE_None;
			}
		}

		if (helper != NULL)
		{
			TRACE1("Reinit '%s'\n", (LPCSTR)m_csFileName);
			return helper->init();
		}
	}
	return ERRORCODE_None;
}

/*
// Set up a file object to read the graphic data.
// This may just set the file name or it may set up the device on top of the
// database's device.
// It is assumed that the storage device coming is uninitialized (no file
// name of based device set).
*/

ERRORCODE GraphicRecord::prep_storage_file(StorageFilePtr file, BOOL fCheckTimeStamp /*=TRUE*/)
{
/*
// Embedded prep.
// Use an embedded file if one is available.
*/

	if (record.source != 0)
	{
		EmbeddedFileCPtr efile;

		if ((efile = (EmbeddedFileCPtr)database->get_record(record.source, NULL, RECORD_TYPE_EmbeddedFile, TRUE)) != NULL)
		{
			if (efile->prep_file(file, TRUE) == ERRORCODE_None)
			{
				return ERRORCODE_None;
			}
		}
	}

/*
// External prep.
// Allow the database's graphic cahce to do something special.
*/

	GraphicCachePtr gcache;
	ERRORCODE error;

	if ((gcache = ((PMGDatabasePtr)database)->get_graphic_cache()) != NULL)
	{
		for (int i = 0; i < 2; i++)		// Try at most twice.
		{
			if ((error = gcache->prep_graphic_file(m_csFileName, file)) == ERRORCODE_None)
			{
				/* We have been externally prep'ed. */

				// If we have not reinited from the cache, do it now.
				if (fCheckTimeStamp && !m_fReinitedCached)
				{
					error = reinit(TRUE);
				}

				return error;
			}
			if (error != ERRORCODE_DoesNotExist)
			{
			/* Some error besides "not cached yet" */
				break;
			}
			if (gcache->cache_graphic(m_csFileName) != ERRORCODE_None)
			{
			/* Could not cache. */
				break;
			}
		}
	}

/*
// Default file prep.
// Oh, well. Just use the file.
*/

	if (fCheckTimeStamp)
	{
		error = reinit(TRUE);
	}
	else
	{
		error = ERRORCODE_None;
	}

	file->set_name(m_csFileName);

	return error;
}

void GraphicRecord::SetTimeStamp(CTimeStamp* pTimeStamp /*=NULL*/)
{
	// Make copy of time stamp passes in, or create invalid time stamp
	// if NULL is passed in. This is juts a bit of work we do to make
	// it easier for the caller.
	CTimeStamp TimeStamp;
	TimeStamp.Invalidate();
	if (pTimeStamp != NULL)
	{
		TimeStamp = *pTimeStamp;
	}

	/*
	// If we don't have a time stamp passed in, try to get one from
	// the file now.
	*/

	if (!TimeStamp.IsValid())
	{
	   CString csName = StorageFile::TranslateFileName(m_csFileName, TRUE);
		if (!TimeStamp.SetFileModify(csName))
		{
			// Dummy value - something other than zero.
			FILETIME t;
			t.dwLowDateTime = (DWORD)(GET_PMWAPP()->GetBuildId()+1);
			t.dwHighDateTime = 0;
			TimeStamp.Set(t);
		}
	}

	// Set the time stamp if we have one.
	if (TimeStamp.IsValid() && (TimeStamp != record.TimeStamp))
	{
		record.TimeStamp = TimeStamp;
		modified();
	}
}
