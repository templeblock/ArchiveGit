/*
// HALO.CPP
//
// Routines for reading a Dr. Halo file.
//
// $Header: /PM8/App/HALO.CPP 1     3/03/99 6:05p Gbeddow $
//
// $Log: /PM8/App/HALO.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 2     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
//    Rev 1.0   14 Aug 1997 15:21:16   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:40   Fred
// Initial revision.
// 
//    Rev 1.4   11 Apr 1997 11:23:30   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.3   05 Dec 1996 12:30:08   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.2   07 Oct 1996 13:19:48   Jay
// prep_storage_file()
// 
//    Rev 1.1   25 Aug 1996 12:59:22   Jay
// No longer sets default name at low level.
// 
//    Rev 1.0   14 Mar 1996 13:44:34   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:44   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:47:52   JAY
// Initial revision.
// 
//    Rev 1.5   09 Jan 1995 16:26:56   JAY
// New stuff (cut & paste, etc).
// 
//    Rev 1.4   30 Nov 1994 16:33:24   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.3   04 Aug 1994 07:32:40   JAY
// Large model and 32-bit changes
// 
//    Rev 1.2   15 Jul 1994 11:29:04   JAY
// access_file().
// 
//    Rev 1.1   05 Jan 1994 08:06:38   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:16   JAY
*/

#include	"stdafx.h"

#include	<io.h>
#include	<string.h>
#include	<stdlib.h>

#include	"grafrec.h"
#include	"halo.h"
#include	"utils.h"

WORD	HALOHelper::halo_x_resolution = 200,
		HALOHelper::halo_y_resolution = 200;

/*
// Read the HALO helper info.
*/

ERRORCODE HALOHelper::read(StorageDevicePtr device)
{
	return device->read_record(&record, sizeof(record));
}

/*
// Write the HALO helper info.
*/

ERRORCODE HALOHelper::write(StorageDevicePtr device)
{
	return device->write_record(&record, sizeof(record));
}

/*
// Size the HALO helper info.
*/

ST_DEV_IO_SIZE HALOHelper::size(StorageDevicePtr device)
{
	return device->size_record(sizeof(record));
}

/*
// Read header information from a Dr. Halo file.
*/

ERRORCODE near HALOHelper::read_halo_header(StorageDevicePtr file,
					LPWORD dots, LPWORD lines,
					LPWORD x_res, LPWORD y_res)
{
	HALO_HEADER header;
	ERRORCODE error;

	if ((error = file->read(&header, sizeof(header))) == ERRORCODE_None)
	{
		*dots = header.xsize;
		*lines = header.ysize;

		*x_res = halo_x_resolution;
		*y_res = halo_y_resolution;
	}
	return error;
}

/*
// Initialize a Dr. Halo file.
*/

ERRORCODE HALOHelper::init(GRAPHIC_CREATE_STRUCT_PTR gcs)
{
	SHORT i;
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

	if ((error = read_halo_header(pSource,
 					(LPWORD)&graphic->record.x_size,
 					(LPWORD)&graphic->record.y_size,
 					(LPWORD)&graphic->record.x_resolution,
 					(LPWORD)&graphic->record.y_resolution)) == ERRORCODE_None)
	{
		graphic->record.storage = GRAPHIC_STORAGE_FILE;

	/* Initialize the start of line variables. */

		for (i = 0; i < 9; i++)
		{
			record.lines[i].line =
							scale_number(graphic->record.y_size, i, 8);
			record.lines[i].offset = 0L;
		}

		record.lines[0].offset = sizeof(HALO_HEADER);

		CHAR buffer[_MAX_PATH];
		strcpy(buffer, graphic->m_csFileName);

		CHAR *extension = strrchr(buffer, '.')+1;
		strcpy(extension, "PAL");
		record.has_pal_file = (access_file(buffer) == 0);
	}

	return error;
}

/*
// The constructor for the HALO driver.
*/

HALODriver::HALODriver(GraphicPtr g, StorageDevice* pSource /*=NULL*/)
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
// The image_info() method for Halo files.
*/

ERRORCODE HALODriver::image_info(IMAGE_INFO_PTR info)
{
	info->bpp = 8;
	info->planes = 1;
	info->image_type = IMAGE_PALETTE;
	info->width = graphic->record.x_size;
	info->height = graphic->record.y_size;

	return ERRORCODE_None;
}

/*
// The open_image() method for Halo files.
*/

ERRORCODE HALODriver::open_image(PALETTE_INFO_PTR palette)
{
	ERRORCODE error;
	HALOHelperPtr helper = (HALOHelperPtr)graphic->my_little_helper();

	if (helper->record.has_pal_file)
	{
		CHAR buffer[_MAX_PATH];
		strcpy(buffer, graphic->m_csFileName);

		CHAR *extension;

		extension = strrchr(buffer, '.')+1;
		strcpy(extension, "PAL");

		ReadOnlyFile pal_file(buffer);

		UCHAR type;
		SHORT extra;

		pal_file.seek(2, ST_DEV_SEEK_SET);
		pal_file.read(&type, sizeof(type));
		extra = (type == 0xE2) ? 0 : 4;

		pal_file.seek(0x28, ST_DEV_SEEK_SET);

		SHORT size = extra+256*3;
		
		LPWORD paldata = NULL;
		TRY
			paldata = new WORD[size];
		END_TRY

		if (paldata != NULL)
		{
			LPWORD sp;
			LPBYTE p;

			pal_file.read(paldata, size*sizeof(WORD));

		/* Move the palette over. */

			sp = paldata;
			p = (LPBYTE)palette->m_pData;

			for (SHORT i = 0, off = 0x28/2; i < 256; i++)
			{
				if (type == 0xE3)
				{
					SHORT diff = 0x100 - (off & 0x00FF);

				/* No palette entries cross file page (512 byte) boundaries. */

					if (diff < 3)
					{
					/* Advance to next page boundary (512 bytes in SHORTs). */
						off += diff;
						sp += diff;
					}
				}
				*p++ = (BYTE)*sp++;
				*p++ = (BYTE)*sp++;
				*p++ = (BYTE)*sp++;

				off += 3;
			}
			palette->m_fRead = TRUE;
			delete [] paldata;
		}
	}

/* Set the number of bytes in a source row. */

	bytes = graphic->record.x_size;

/* Seek to the first line. */

	current_line = 0;
	locator = helper->record.lines;

	if ((error = file.seek(locator->offset, ST_DEV_SEEK_SET)) != ERRORCODE_None)
	{
		return error;
	}

/* Set the buffer. */

	file.set_read_buffer(2048);
	return ERRORCODE_None;
}

/*
// The read_block() method for HALO files.
*/

ERRORCODE HALODriver::read_lines(int line, LPBYTE buffer, int count, LPBYTE pMask)
{
	ASSERT(pMask == NULL);

	ERRORCODE error = ERRORCODE_None;
	SHORT locator_line;							/* The hit line for this locator. */
	SHORT skip_count = 0;						/* Rows to skip before reading. */
	SHORT line_size;

/* Find the starting place. */

	if (current_line != line)
	{
	/* We must move there somehow. */

		while ((locator->line <= (unsigned)line) && (locator->offset != 0L))
		{
			locator++;
		}

	/* Back up to the one we want. */

		locator--;

	/* Seek there. */

		if (current_line < (SHORT)locator->line)
		{
		/* We advanced locators. Start from beginning of locator! */
			current_line = locator->line;
			if ((error = file.seek(locator->offset, ST_DEV_SEEK_SET)) != ERRORCODE_None)
			{	
				return error;
			}
		}
		skip_count = line - current_line;
	}

/* Remember the start of the next locator. */

	locator_line = (locator+1)->line;

/* Skip the number of lines we need to. */

	while (skip_count-- != 0)
	{
		if (current_line == locator_line)
		{
		/* Hit the next locator. Fill it in. */
			locator++;
			file.tell(&locator->offset);
			locator_line = (locator+1)->line;
		}

	/* Read the size for the next line. */

		file.read(&line_size, sizeof(line_size));

	/* Skip that many lines. */

		file.seek((ST_DEV_POSITION)line_size, ST_DEV_SEEK_CUR);
				
		current_line++;
	}

/* Now read lines until done. */

	while (count-- > 0)
	{
		LPBYTE p;

	/* If we are at the next locator line, fill it in. */

		if (current_line == locator_line)
		{
		/* We are at this locator. Fill it in. */
			locator++;
			file.tell(&locator->offset);
			locator_line = (locator+1)->line;
		}

	/* Read the next line. */

		file.read(&line_size, sizeof(line_size));

		p = buffer;

		while (line_size > 0)
		{
			BYTE count, byte;

			file.read(&count, sizeof(count));
			line_size--;

			if (count & 0x80)
			{
			/* Repeat sequence. */
				count &= 0x7f;
				file.read(&byte, sizeof(byte));
				line_size--;

				memset(p, byte, count);
			}
			else
			{
			/* Replace sequence. */
				line_size -= count;
				file.read(p, count);
			}
			p += count;
		}

		if (line_size < 0)
		{
//				printf("ERROR! WARNING! STOP! (%d)\n", line_size);
			return ERRORCODE_InvalidRecord;
		}

	/* We are already one line ahead. */

		current_line++;
		buffer += bytes;
	}

	if (current_line == locator_line)
	{
	/* Hit the next locator. Fill it in. */
		locator++;
		file.tell(&locator->offset);
	}
	return error;
}

