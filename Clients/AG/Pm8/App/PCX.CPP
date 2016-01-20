/*
// PCX.CPP
//
// $Header: /PM8/App/PCX.CPP 1     3/03/99 6:09p Gbeddow $
//
// PCX routines.
//
// $Log: /PM8/App/PCX.CPP $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 8     1/28/99 6:20p Johno
// Got rid of goofy "CDIBInfo is a member of CPmwDoc (not really)" syntax
// 
// 7     9/14/98 12:09p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 6     8/20/98 11:41a Dennis
// Type casted x_resolution assignments.
// 
// 5     8/20/98 11:02a Jayn
// New resolution overrides. Small license path change.
// 
// 4     7/12/98 5:35p Jayn
// Fixed off by 1 errors in PCX export.
// 
// 3     11/25/97 10:37a Johno
// Moved write buffering to CDibToGraphicFile; does "bottom up" dib write
// 
// 2     11/21/97 5:21p Johno
// General update
// 
//    Rev 1.0   14 Aug 1997 15:23:36   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:39:40   Fred
// Initial revision.
// 
//    Rev 1.4   11 Apr 1997 11:23:34   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.3   05 Dec 1996 12:30:22   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.2   11 Oct 1996 09:40:10   Jay
// Fixed a potential crashing bug in read_pcx_line.
// 
//    Rev 1.1   25 Aug 1996 12:59:22   Jay
// No longer sets default name at low level.
// 
//    Rev 1.0   14 Mar 1996 13:46:02   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:08:14   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:48:02   JAY
// Initial revision.
// 
//    Rev 1.5   09 Jan 1995 16:26:30   JAY
// New stuff (cut & paste, etc).
// 
//    Rev 1.4   30 Nov 1994 16:34:04   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.3   04 Aug 1994 07:33:02   JAY
// Large model and 32-bit changes
// 
//    Rev 1.2   23 May 1994 08:21:26   JAY
// Now uses prep_storage_file() instead of the direct init so we can use files
// based on the document database device.
// 
//    Rev 1.1   05 Jan 1994 08:05:48   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:00   JAY
// 
*/

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <string.h>

#include "grafrec.h"
#include "pcx.h"
#include "utils.h"
#include "util.h"
#include "docdib.h"		// For CDIBInfo

POINT PCXHelper::m_pntResolution = { 200, 200 };

/*
// Read the PCX helper info.
*/

ERRORCODE PCXHelper::read(StorageDevicePtr device)
{
	return device->read_record(&record, sizeof(record));
}

/*
// Write the PCX helper info.
*/

ERRORCODE PCXHelper::write(StorageDevicePtr device)
{
	return device->write_record(&record, sizeof(record));
}

/*
// Size the PCX helper info.
*/

ST_DEV_IO_SIZE PCXHelper::size(StorageDevicePtr device)
{
	return device->size_record(sizeof(record));
}

/*
//	Read a PCX file header.
// Advance to the data.
*/

PRIVATE ERRORCODE
read_pcx_header(StorageDevicePtr file, WORD *dots, WORD *lines, WORD *row_bytes,\
								BYTE *nplanes, BYTE *bpp)
{
	ERRORCODE error;
	PCX_HEADER header;

/* Read the header. */

	if ((error = file->read(&header, sizeof(header))) == ERRORCODE_None)
	{
		if (header.d_Manuf != 10 || header.d_Encod != 1)
		{
		/* Not something we want ot deal with. */
			error = ERRORCODE_IllegalType;
		}
		else
		{
		/* Pass back what we got. */

			*dots = header.d_X2 - header.d_X1 + 1;
			*lines = header.d_Y2 - header.d_Y1 + 1;
			*row_bytes = header.d_Bplin;
			*nplanes = header.d_Nplanes;
			*bpp = header.d_Bitpx;

			file->seek(128L-sizeof(header), ST_DEV_SEEK_CUR);
		}
	}
	return error;
}

/*
// Initialize a PCX graphic.
*/

ERRORCODE PCXHelper::init(GRAPHIC_CREATE_STRUCT_PTR gcs)
{
	WORD dots, lines, row_bytes;
	BYTE nplanes, bpp;
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

/* Read the header. */

	if ((error = read_pcx_header(pSource, &dots, &lines, &row_bytes,
				&nplanes, &bpp)) == ERRORCODE_None)
	{
	/* Fill out the header. */

		graphic->record.x_size = dots;
		graphic->record.y_size = lines;
		graphic->record.x_resolution = (USHORT) m_pntResolution.x;
		graphic->record.y_resolution = (USHORT) m_pntResolution.y;
		graphic->record.storage = GRAPHIC_STORAGE_FILE;

	/* Fill out our info. */

		record.row_bytes = row_bytes;
		record.nplanes = nplanes;
		record.bits_per_pixel = bpp;

	/* Initialize the start of line variables. */

		for (SHORT i = 0; i < 9; i++)
		{
			record.lines[i].line = scale_number(lines, i, 8);
			record.lines[i].offset = 0L;
		}

		record.lines[0].offset = 128L;
	}

	return error;
}

/*
// The constructor for the PCX driver.
*/

PCXDriver::PCXDriver(GraphicPtr g, StorageDevice* pSource /*=NULL*/)
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

ERRORCODE PCXDriver::image_info(IMAGE_INFO_PTR info)
{
	PCXHelperPtr helper = (PCXHelperPtr)graphic->my_little_helper();
	UCHAR planes, bpp;
	IMAGE_TYPE image_type;

	planes = helper->record.nplanes;
	bpp = helper->record.bits_per_pixel;

	if (bpp == 0)
	{
		bpp = 1;
	}

	if (bpp == 1)
	{
		if (planes == 1)
		{
		/* 1x1 */
			image_type = IMAGE_BILEVEL;
		}
		else
		{
		/* 1xn */
			image_type = IMAGE_PLANAR;
		}
	}
	else
	{
		if (planes == 1)
		{
		/* nx1 */
			image_type = IMAGE_PALETTE;
		}
		else if (bpp == 8 && planes == 3)
		{
		/* 8x3 RGB */

			image_type = IMAGE_FULL_COLOR;
		}
		else
		{
			return ERRORCODE_IllegalType;
		}
	}

	info->planes = planes;
	info->bpp = bpp;
	info->image_type = image_type;
	info->width = graphic->record.x_size;
	info->height = graphic->record.y_size;

	return ERRORCODE_None;
}

/*
// Rewind to the beginning of the file.
*/

ERRORCODE PCXDriver::rewind()
{
	PCXHelperPtr helper = (PCXHelperPtr)graphic->my_little_helper();

	current_line = 0;
	locator = helper->record.lines;
	return file.seek(locator->offset, ST_DEV_SEEK_SET);
}

/*
// The open_image() method for PCX files.
*/

ERRORCODE PCXDriver::open_image(PALETTE_INFO_PTR palette)
{
	SHORT i;
	ERRORCODE error;

/* Read the header. */

	if (our_info.image_type == IMAGE_PALETTE)
	{
		CHAR ch;
		PCX_HEADER header;

	/* We must read the header to get version and maybe palette. */

		if ((error = file.read(&header, sizeof(header))) != ERRORCODE_None)
		{
			return error;
		}

	/* See if there is a palette in the file. */

		if (palette->m_nColors == 256)
		{
			if (header.d_Hard == 5)
			{
			/*
			// PCX (5) supports a 256 color palette at the END of the file.
			// The recommended technique is to seek 769 (palette size + 1)
			// back from the end of the file. If the byte there is a 12 (0x0c)
			// then the palette follows.
			*/
				file.seek(-769L, ST_DEV_SEEK_END);
				file.read(&ch, sizeof(ch));

				if (ch == 12)
				{
				/* We have a palette. */
					file.read(palette->m_pData, 256 * sizeof(RGBTRIPLE));
					palette->m_fRead = TRUE;
				}
			}
		}
		else
		{
			if (header.d_Hard != 3 && header.d_Hard != 4)	/* Ver. 3 & 4 have no palette info. */
			{
				COLOR_COMPONENT far *d, *s;

			/*
			// Move them one entry at a time. We want to see if the palette
			// is good.
			*/

				for (d = (COLOR_COMPONENT far *)palette->m_pData,
								s = (COLOR_COMPONENT *)header.d_clrmap,
								i = palette->m_nColors*3;
							i-- != 0;)
				{
					if ((*d++ = *s++) != 0)
					{
						palette->m_fRead = TRUE;
					}
				}
			}
		}
	}

/* Seek to the first line. */

	PCXHelperPtr helper = (PCXHelperPtr)graphic->my_little_helper();

	pcx_row_bytes = helper->record.row_bytes; /* Byte width of PCX file. */

	if ((error = rewind()) != ERRORCODE_None)
	{
		return error;
	}

/* Set the buffer. */

	file.set_read_buffer(2048);
	return ERRORCODE_None;
}

/*
// The read_lines() method for PCX files.
*/

ERRORCODE PCXDriver::read_lines(int line, LPBYTE buffer, int count, LPBYTE pMask)
{
	ASSERT(pMask == NULL);

	SHORT locator_line;							/* The hit line for the next locator. */
	SHORT skip_count = 0;						/* Rows to skip before reading. */
	ERRORCODE error;
	SHORT row_bytes = pcx_row_bytes;

	if (line < current_line)
	{
		if ((error = rewind()) != ERRORCODE_None)
		{
			return error;
		}
	}

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

	locator_line = (locator+1)->line;

/* Skip to the line we want. */

	while (skip_count != 0)
	{
		SHORT amount;

		if (current_line == locator_line)
		{
		/* Hit the next locator. Fill it in. */
			locator++;
			file.tell(&locator->offset);
			locator_line = (locator+1)->line;
		}

	/*
	// Compute how much to skip.
	// We either skip the amount we want or to the next locator line,
	// whichever is closer.
	*/

		amount = locator_line - current_line;

		if (amount > skip_count)
		{
			amount = skip_count;
		}

		if ((error = skip_pcx_lines(row_bytes, our_info.planes, amount)) != ERRORCODE_None)
		{
			return error;
		}

		current_line += amount;
		skip_count -= amount;
	}

/* Now read lines. */

	while (count-- > 0)
	{
	/*
 	// If we are at the next locator line, fill it in.
 	*/

		if (current_line == locator_line)
		{
		/* We are at this locator. Fill it in. */
			locator++;
			file.tell(&locator->offset);
			locator_line = (locator+1)->line;
		}

	/* Read the next PCX line. */

		if ((error = read_pcx_line(source_row_bytes,
				row_bytes, our_info.planes, buffer,
				our_info.image_type == IMAGE_FULL_COLOR)) != ERRORCODE_None)
		{
			return error;
		}

		current_line++;
		buffer += source_row_size;

	}

	if (current_line == locator_line)
	{
	/* We are at this locator. Fill it in. */
		locator++;
		file.tell(&locator->offset);
	}
	return ERRORCODE_None;
}

/*
// Skip some PCX lines.
// All PCX lines are stored separately, so this should work just fine.
*/

ERRORCODE near PCXDriver::skip_pcx_lines(SHORT row_bytes, SHORT nplanes, SHORT lines)
{
	UCHAR byte;
	LONG bytes;
	ERRORCODE error = ERRORCODE_None;

	/*
	// Read in the next line of data.
	*/

	bytes = (LONG)nplanes * (LONG)row_bytes * (LONG)lines;

	while (bytes > 0)
	{
	/* Read the next control byte. */

		if ((error = file.read(&byte, sizeof(byte))) != ERRORCODE_None)
		{
			return error;
		}

	/* See what type of sequence it is. */

		if ((byte & 0xc0) == 0xc0)
		{
		/* A multi-byte sequence. */
			bytes -= byte & 0x3F;
			file.read(&byte, sizeof(byte));
		}
		else
		{
		/* A single byte. */
			bytes--;
		}
	}

	if (bytes < 0)
	{
//		printf("***** bytes is %d *****\n", bytes);
	}

	return error;
}

/*
// Read a PCX line.
// All PCX lines are stored separately, so this should work just fine.
*/

ERRORCODE near PCXDriver::read_pcx_line(SHORT bytes, SHORT row_bytes, BYTE nplanes, LPBYTE current_byte, BOOL full_color)
{
	SHORT plane;
	UCHAR byte;
	SHORT carry_over = 0;
	ERRORCODE error = ERRORCODE_None;

	if (full_color)
	{
	/*
	// Handle 24-bit lines specially.
 	// We need to turn RGB lines into the byte interleaved RGB format we
	// need for processing.
	*/

		for (plane = 0; plane < nplanes; plane++)
		{
			SHORT x_byte, count, left;
			LPBYTE p;

			for (p = current_byte+plane, x_byte = 0; x_byte < row_bytes; )
			{
				if (carry_over == 0)
				{
				/* Read the next control byte. */

					if ((error = file.read(&byte, sizeof(byte))) != ERRORCODE_None)
					{
						return error;
					}

					if ((byte & 0xc0) == 0xc0)
					{
					/* A multi-byte sequence. */
						count = byte & 0x3F;
						file.read(&byte, sizeof(byte));
					}
					else
					{
					/* A single byte. */
						count = 1;
					}
				}
				else
				{
					count = carry_over;
					carry_over = 0;
				}

				if ((left = bytes - x_byte) > count)
				{
					left = count;
				}

				while (left-- > 0)
				{
					*p = byte;
					p += (SHORT)nplanes;
				}

				x_byte += count;
			}
			carry_over = x_byte - row_bytes;
		}
	}
	else
	{
		/*
		// Read in the next line of data.
		*/

		for (plane = nplanes; plane--; )
		{
			SHORT x_byte, count, left;

			for (x_byte = 0; x_byte < row_bytes; )
			{
				if (carry_over == 0)
				{
				/* Read the next control byte. */

					if ((error = file.read(&byte, sizeof(byte))) != ERRORCODE_None)
					{
						return error;
					}

					if ((byte & 0xc0) == 0xc0)
					{
					/* A multi-byte sequence. */
						count = byte & 0x3F;
						file.read(&byte, sizeof(byte));
					}
					else
					{
					/* A single byte. */
						count = 1;
					}
				}
				else
				{
					count = carry_over;
					carry_over = 0;
				}

				if ((left = bytes - x_byte) > count)
				{
					left = count;
				}

				if (left > 0)
				{
					memset(current_byte, byte, left);
					current_byte += left;
				}

				x_byte += count;
			}
			carry_over = x_byte - row_bytes;
		}
	}

	return error;
}

#define  PCXZERO  (BYTE)0xC0
#define  PCXONE   (BYTE)0xC1

struct BGR
{
   BYTE  Red;
   BYTE  Green;
   BYTE  Blue;
   BGR ()
   {
      Blue = Green = Red = 0;
   }

   void
   Assign (RGBQUAD rgbq)
   {
      Blue  = rgbq.rgbBlue;
      Green = rgbq.rgbGreen;
      Red   = rgbq.rgbRed;
   }
};

void
CDIBtoPCXWriter::CreatePCX (StorageDevice *pdev, CDIBInfo *pInfo, long dpi)
{
   BITMAPINFO  *pbi = pInfo->BitmapInfo ();
   // Must have a 8 bit dib
   ASSERT (pbi->bmiHeader.biBitCount == 8);
   // PCX dimensions are WORDs
   ASSERT (pbi->bmiHeader.biWidth < 0xFFFF);
   ASSERT (pbi->bmiHeader.biHeight < 0x7FFF);

   long  Rows = pbi->bmiHeader.biHeight;
   BOOL  UpsideDown = (Rows > 0) ? TRUE : FALSE;
   if (Rows < 0)
      Rows = -Rows;
   
   PCX_HEADER  Header;

   memset (&Header, 0, sizeof (PCX_HEADER));

   Header.d_Manuf = 10; // ???
   Header.d_Hard = 5;   // 256 color
   Header.d_Encod = 1;
   Header.d_Bitpx = (BYTE)pbi->bmiHeader.biBitCount;

   Header.d_X1 = Header.d_Y1 = 0;
   Header.d_X2 = (WORD)pbi->bmiHeader.biWidth-1;
   Header.d_Y2 = (WORD)Rows-1;

   Header.d_Hres = Header.d_Vres = (WORD)dpi;

   //Header.d_clrmap [] For 16 colors?

   Header.d_Nplanes = 1;
   Header.d_Bplin = (pbi->bmiHeader.biBitCount * (WORD)pbi->bmiHeader.biWidth) >> 3;

   TRY
   {
      pdev->huge_write ((BYTE*)&Header, sizeof (PCX_HEADER));
      pdev->seek (128, ST_DEV_SEEK_SET);

      struct
      {
         BYTE  count;
         BYTE  byte;
      } RLE;

      DWORD RowBytes = (pbi->bmiHeader.biBitCount * pbi->bmiHeader.biWidth) >> 3;
      BYTE  *pRead;
      
      if (UpsideDown == TRUE) // Do we support `biHeight < 0 == top-down DIB` ?
      {
         pRead = pInfo->BitmapData() + (Util::RoundToDWORD (RowBytes) * (Rows - 1));
         for (long i = 0; i < Rows; i ++)
         {
            DWORD idx = 0;
            while (idx < RowBytes)
            {
               RLE.byte = pRead [idx ++];
               RLE.count = PCXONE;

               while ((pRead [idx] == RLE.byte) && (idx < RowBytes) && (RLE.count < (BYTE)0xff))
               {
                  RLE.count ++;
                  idx ++;
               }
               if ((RLE.count > PCXONE) || (RLE.byte >= PCXZERO))
                  pdev->huge_write ((BYTE*)&RLE, 2);
               else
                  pdev->huge_write ((BYTE*)&RLE.byte, 1);
            }

            pRead -= Util::RoundToDWORD (RowBytes);
         }
      }
      else
      {
         Rows =- Rows;
         pRead = pInfo->BitmapData();

         for (long i = 0; i < Rows; i ++)
         {
            DWORD idx = 0;
            while (idx < RowBytes)
            {
               RLE.byte = pRead [idx ++];
               RLE.count = PCXONE;

               while ((pRead [idx] == RLE.byte) && (idx < RowBytes) && (RLE.count < (BYTE)0xff))
               {
                  RLE.count ++;
                  idx ++;
               }
               if ((RLE.count > PCXONE) || (RLE.byte >= PCXZERO))
                  pdev->huge_write ((BYTE*)&RLE, 2);
               else
                  pdev->huge_write ((BYTE*)&RLE.byte, 1);
            }

            pRead += Util::RoundToDWORD (RowBytes);
         }
      }

      RLE.byte = 12;
      pdev->huge_write ((BYTE*)&RLE.byte, 1);

      RGBQUAD  rgb, *prgb = pbi->bmiColors;
      BGR      bgr;

      for (DWORD idx = 0; idx < 256; idx ++)
      {
         rgb = prgb [idx];
         bgr.Assign (rgb);

         pdev->huge_write ((BYTE*)&bgr, 3);
      }
   }
   CATCH(CErrorcodeException, e)
   {
      ErrorCode = e->m_error;
   }
   CATCH(CMemoryException, e)
   {
      ErrorCode = ERRORCODE_Memory;
   }
   AND_CATCH_ALL (e)
   {
      ErrorCode = ERRORCODE_Fail;
   }
   END_CATCH_ALL
}
