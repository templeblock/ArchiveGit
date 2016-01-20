/*
// TIFFDRV.CPP
//
// TIFF file driver module.
//
// $Header: /PM8/App/TIFFDRV.CPP 1     3/03/99 6:12p Gbeddow $
//
// $Log: /PM8/App/TIFFDRV.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 8     1/28/99 6:20p Johno
// Got rid of goofy "CDIBInfo is a member of CPmwDoc (not really)" syntax
// 
// 7     9/14/98 12:11p Jayn
// Removed system_heap. Switched to MFC in DLL.
// 
// 6     11/25/97 10:38a Johno
// Does "bottom up" dib write
// 
// 5     11/21/97 5:21p Johno
// General update
// 
// 4     11/20/97 10:37a Johno
// Use CDIBInfo::PrepareData
// 
// 3     11/19/97 2:38p Johno
// Re - sync to Source Safe
// 
// 2     11/18/97 5:50p Johno
// CDIBtoTIFFWriter
// 
//    Rev 1.0   14 Aug 1997 15:26:32   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:48   Fred
// Initial revision.
// 
//    Rev 1.3   11 Apr 1997 11:23:40   Jay
// Graphic cleanup; PhotoEnhancer support
// 
//    Rev 1.2   05 Dec 1996 12:30:28   Jay
// New image code for handling transparent bitmaps.
// 
//    Rev 1.1   08 Nov 1996 08:57:46   Jay
// Removed 16-bit compress.obj
// 
//    Rev 1.0   14 Mar 1996 13:47:32   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:09:26   FRED
// Initial revision.
// 
//    Rev 1.6   22 Dec 1995 08:00:56   JAY
// Variable used without being initialized!
// 
//    Rev 1.5   18 Sep 1995 08:25:48   JAY
// Made PackBits work.
// 
//    Rev 1.4   15 Sep 1995 15:30:34   JAY
// Moved WIN32 uncompress to COMP32.CPP.
// 
//    Rev 1.3   15 Sep 1995 14:04:22   JAY
// More 32-bit changes.
// 
//    Rev 1.2   13 Sep 1995 11:33:28   JAY
// New 32-bit stuff
// 
//    Rev 1.1   20 Jun 1995 15:46:58   JAY
// Wasn't handling "missing" virtual files correctly because it wasn't
// catching the open error.
// 
//    Rev 1.0   29 Mar 1995 09:00:44   JAY
// Initial revision.
*/

#include "stdafx.h"
#include "tiffdrv.h"
#include "compress.h"
#include "docdib.h"		// For CDIBInfo

/////////////////////////////////////////////////////////////////////////////
// Generic

/*
// Swap a word of data.
*/

static void near
swap_word(LPWORD w)
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
swap_long(LPLONG l)
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

/* The size, in bytes, of each type of data */

static int nTypeSizes[] =
{
	0,
	1,		/* TIFF_BYTE */
	1,		/* TIFF_ASCII */
	2,		/* TIFF_SHORT */
	4,		/* TIFF_LONG */
	8,		/* TIFF_RATIONAL */
	1,		/* TIFF_SBYTE */
	1,		/* TIFF_UNDEFINED */
	2,		/* TIFF_SSHORT */
	4,		/* TIFF_SLONG */
	8,		/* TIFF_SRATIONAL */
	sizeof(float),		/* TIFF_FLOAT */
	sizeof(double)		/* TIFF_DOUBLE */
};

/* TIFF entries (actually!) */

static TIFF_ENTRY
default_tiff_entries[] =
{
	/* (255) Subfile Type */
	{
		FALSE,
		FALSE, { 0 },
		{ TIFF_SubfileType, TIFF_SHORT, 1 }
	},

	/* (256) Image Width */
	{
		FALSE,
		FALSE, { 0 },
		{ TIFF_ImageWidth, TIFF_SHORT, 1 }
	},

	/* (257) Image Length */
	{
		FALSE,
		FALSE, { 0 },
		{ TIFF_ImageLength, TIFF_SHORT, 1 }
	},

	/* (258) Bits Per Sample */
	{
		FALSE,
		TRUE, { 1 },
		{ TIFF_BitsPerSample, TIFF_SHORT }
	},
	
	/* (259) Compression */
	{
		FALSE,
		TRUE, { TIFF_COMP_NONE },
		{ TIFF_Compression, TIFF_SHORT }
	},

	/* (262) Photometric Interpretation */
	{
		FALSE,
		FALSE, { 0 },
		{ TIFF_PhotometricInterp, TIFF_SHORT, 1 }
	},

#ifdef EXTENSIVE_TIFF

	/* (263) Threshholding */
	{
		FALSE,
		TRUE, { 1 },
		{ TIFF_Threshholding, TIFF_SHORT, 1 }
	},

	/* (264) Cell Width */
	{
		FALSE,
		FALSE, { 0 },
		{ TIFF_CellWidth, TIFF_SHORT, 1 }
	},

	/* (265) Cell Length */
	{
		FALSE,
		FALSE, { 0 },
		{ TIFF_CellLength, TIFF_SHORT, 1 }
	},

	/* (266) Fill Order */
	{
		FALSE,
		TRUE, { 1 },
		{ TIFF_FillOrder, TIFF_SHORT, 1 }
	},

	/* (269) Document Name */
	{
		FALSE,
		FALSE, { 0 },
		{ TIFF_DocumentName, TIFF_ASCII }
	},

	/* (270) Image Description */
	{
		FALSE,
		FALSE, { 0 },
		{ TIFF_ImageDescription, TIFF_ASCII }
	},

	/* (271) Make */
	{
		FALSE,
		FALSE, { 0 },
		{ TIFF_Make, TIFF_ASCII }
	},

	/* (272) Model */
	{
		FALSE,
		FALSE, { 0 },
		{ TIFF_Model, TIFF_ASCII }
	},
#endif

	/* (273) Strip Offsets */
	{
		FALSE,
		FALSE, { 0 },
		{ TIFF_StripOffsets, TIFF_LONG }
	},

#ifdef EXTENSIVE_TIFF
	/* (274) Orientation */
	{
		FALSE,
		TRUE, { 1 },
		{ TIFF_Orientation, TIFF_SHORT, 1 }
	},

#endif
	/* (277) Samples Per Pixel */
	{
		FALSE,
		TRUE, { 1 },
		{ TIFF_SamplesPerPixel, TIFF_SHORT, 1 }
	},

	/* (278) Rows Per Strip */
	{
		FALSE,
		TRUE, { -1L },			/* Infinity */
		{ TIFF_RowsPerStrip, TIFF_LONG }
	},

#ifdef EXTENSIVE_TIFF
	/* (279) Strip Byte Counts */
	{
		FALSE,
		FALSE, { 0 },
		{ TIFF_StripByteCounts, TIFF_LONG }
	},

	/* (280) Min Sample Value */
	{
		FALSE,
		TRUE, { 0 },
		{ TIFF_MinSampleValue, TIFF_SHORT }
	},

	/* (281) Max Sample Value */
	{
		FALSE,
		TRUE, { -1 },			/* Infinity */
		{ TIFF_MaxSampleValue, TIFF_SHORT }
	},
#endif

	/* (282) X Resolution */
	{
		FALSE,
		FALSE, { 0 }, 
		{ TIFF_XResolution, TIFF_RATIONAL, 1 }
	},

	/* (283) Y Resolution */
	{
		FALSE,
		FALSE, { 0 },
		{ TIFF_YResolution, TIFF_RATIONAL, 1 }
	},
	/* (284) Planar Configuration */
	{
		FALSE,
		TRUE, { 1 },
		{ TIFF_PlanarConfiguration, TIFF_SHORT, 1 }
	},

#ifdef EXTENSIVE_TIFF
	/* (285) Page Name */
	{
		FALSE,
		FALSE, { 0 },
		{ TIFF_PageName, TIFF_ASCII }
	},

	/* (286) X Position */
	{
		FALSE,
		FALSE, { 0 },
		{ TIFF_XPosition, TIFF_RATIONAL, 1 }
	},

	/* (287) Y Position */
	{
		FALSE,
		FALSE, { 0 },
		{ TIFF_YPosition, TIFF_RATIONAL, 1 }
	},
#endif
	/* (290) Gray Response Unit */
	{
		FALSE,
		TRUE, { 2 },
		{ TIFF_GrayResponseUnit, TIFF_SHORT, 1 }
	},

	/* (291) Gray Response Curve */
	{
		FALSE,
		FALSE, { 0 },
		{ TIFF_GrayResponseCurve, TIFF_SHORT, 1 }
	},

	/* (317) Predictor */
	{
		FALSE,
		TRUE, { 1 },
		{ TIFF_Predictor, TIFF_SHORT, 1 }
	},

	/* (320) ColorMap */
	{
		FALSE,
		FALSE, { 0 },
		{ TIFF_ColorMap, TIFF_SHORT, 1 }
	}
};

#define NUM_FIELDS	(sizeof(default_tiff_entries)/sizeof(*default_tiff_entries))

/////////////////////////////////////////////////////////////////////////////
// CTIFFInfo

CTIFFInfo::CTIFFInfo()
{
	m_pEntries = NULL;
}

CTIFFInfo::~CTIFFInfo()
{
	Close();
}

ERRORCODE CTIFFInfo::read(StorageDevicePtr device)
{
	return device->read(&m_Info, sizeof(m_Info));
}

ERRORCODE CTIFFInfo::write(StorageDevicePtr device)
{
	return device->write(&m_Info, sizeof(m_Info));
}

ST_DEV_IO_SIZE CTIFFInfo::size(StorageDevicePtr device)
{
	return sizeof(m_Info);
}

/*
//	Put default values in a TIFF open file structure.
*/

void near CTIFFInfo::SetDefaults(void)
{
	memcpy(m_pEntries, default_tiff_entries, sizeof(default_tiff_entries));
}

/*
// Swap tiff data if necessary.
*/

void near CTIFFInfo::SwapData(TIFF_DATA* pData, SHORT nType)
{
	if (m_Info.byte_order != BYTE_ORDER_IBM)
	{
		switch (nType)
		{
			case TIFF_SHORT:
			{
				swap_word((LPWORD)&pData->Short);
				break;
			}
			case TIFF_LONG:
			{
				swap_long(&pData->Long);
				break;
			}
			case TIFF_RATIONAL:
			{
				swap_long(&pData->Rational.numerator);
				swap_long(&pData->Rational.denominator);
				break;
			}
			default:
			{
				break;
			}
		}
	}
}

/*
//	Return the address of the entry corresponding to this tag.
//	Return NULL if there is no corresponding entry.
*/

TIFF_ENTRY_PTR near CTIFFInfo::FindEntry(SHORT nTag)
{
	TIFF_ENTRY* pEntry = m_pEntries;

	for (int nIndex = 0; nIndex < NUM_FIELDS; nIndex++, pEntry++)
	{
		int nEntryTag;

		if ((nEntryTag = pEntry->entry.tag) == nTag)
		{
			return pEntry;
		}

	/* A small optimization (tags are sorted in ascending order. */

		if (nEntryTag > nTag)		/* We passed our spot... */
		{
			break;			/* Abort the loop. */
		}
	}
	return NULL;
}

/*
//	Read the data from a tiff entry.
*/

ERRORCODE near CTIFFInfo::GetData(TIFF_TAG_TAG nTag, LONG lNumber, TIFF_DATA* pData)
{
	ERRORCODE error;			// General use.

	TIFF_ENTRY_PTR pEntry;
	SHORT nTypeSize;
	LPBYTE data_ptr;
	BOOL fNeedsSwapping;
	ST_DEV_POSITION offset;

/* Zero the data union. A Rational is the largest element, so zero it. */

	pData->Rational.numerator = pData->Rational.denominator = 0L;

	if (!m_Info.valid)
	{
		return ERRORCODE_NotInitialized;
	}

	if ((pEntry = FindEntry(nTag)) == NULL)
	{
		return ERRORCODE_BadParameter;
	}

	data_ptr = NULL;				/* No data pointed to, yet. */

	if (pEntry->entry.type >= MAX_TIFF_TAG_TYPE)
	{
	/* Bad type. */
		return ERRORCODE_IllegalType;
	}

	nTypeSize = nTypeSizes[pEntry->entry.type];

/* See if the entry was read. */

	if (pEntry->entry_valid)
	{
	/* The entry was read! */

		if (lNumber >= pEntry->entry.length)
		{
		/* Invalid item number. */
			return ERRORCODE_BadParameter;
		}

		if (nTypeSize*pEntry->entry.length > sizeof(LONG))
		{
		/* If the default has been set for a Rational, use it. */

			if ((pEntry->entry.type == TIFF_RATIONAL) &&
				(pEntry->entry.length == 1) &&
				(pEntry->default_valid))
			{
				memcpy(pData, &pEntry->default_value, nTypeSize);
				return ERRORCODE_None;
			}

		/* The value_offset is a file offset. */

			offset = pEntry->entry.value_offset;

			if (m_Info.byte_order != BYTE_ORDER_IBM)
			{
				swap_long(&offset);
			}

			if ((error = m_pDevice->seek(offset + (LONG)(lNumber*nTypeSize), ST_DEV_SEEK_SET)) == ERRORCODE_None)
			{
				if ((error = m_pDevice->read(pData, nTypeSize)) != ERRORCODE_None)
				{
					return error;
				}

			/* Swap the data if necessary. */

				SwapData(pData, pEntry->entry.type);

			/*
			// Special case for Rational values.
			// If this is a single value on disk, we can set it as the default.
			// This will prevent the value from being read multiple times.
			// (i.e. We cache it.)
			*/

				if ((pEntry->entry.type == TIFF_RATIONAL)
					&& (pEntry->entry.length == 1))
				{
				/* Set as the default value. */

					memcpy(&pEntry->default_value, pData, nTypeSize);

					pEntry->default_valid = TRUE;
				}
				return ERRORCODE_None;
			}
			else
			{
			/* Multiple items do not have defaults. Just return the error. */
				return error;
			}
		}
		else
		{
			/* The value_offset is a value. */
			/* Copy the 'number'th element out of the field. */

			data_ptr = ((LPBYTE)&pEntry->entry.value_offset) + lNumber*nTypeSize;
		}
	}

/* If we fell through without setting the pointer, use the default. */

	if (data_ptr == NULL)
	{
		if (pEntry->default_valid)
		{
		/* The entry has a default! Send it back. */
			data_ptr = (LPBYTE)&pEntry->default_value;
			fNeedsSwapping = FALSE;
		}
		else
		{
		/* No default either! */
			return ERRORCODE_BadParameter;
		}
	}
	else
	{
		fNeedsSwapping = TRUE;
	}
	memcpy(pData, data_ptr, nTypeSize);

	if (fNeedsSwapping)
	{
		SwapData(pData, pEntry->entry.type);
	}
	return ERRORCODE_None;
}

/*
// read_tiff_tags reads tags found in the specified file and fills the given
// TIFF field structure with the read tag values.  If a tag isn't found, a
// default value will be used. 
*/

ERRORCODE near CTIFFInfo::ReadTags(void)
{
	ERRORCODE error;

	TIFF_HEADER header;
	TIFF_IFD ifd;
	TIFF_FILE_ENTRY entry;
	TIFF_ENTRY_PTR pEntry;
	WORD wEntryCount;
	LONG lIfdAddress, lEntryOffset;

/* If we have already read the tags, don't read them again. */

	if (m_Info.valid)
	{
		return ERRORCODE_None;
	}

/* Initialize data to defaults */

	SetDefaults();

	m_Info.unused_offset = 0L;

#if 0
	if (type == GRAPHIC_TYPE_EPS)
	{
		if ((error = m_pDevice->seek(0x14, ST_DEV_SEEK_SET)) != ERRORCODE_None
				|| (error = m_pDevice->read(&m_Info.offset, sizeof(m_Info.offset))) != ERRORCODE_None)
		{
			return error;
		}
	}
#endif

/*
// Seek to the start and read in the header.
*/
	if ((error = m_pDevice->seek(0, ST_DEV_SEEK_SET)) != ERRORCODE_None
		|| (error = m_pDevice->read(&header, sizeof(TIFF_HEADER))) != ERRORCODE_None)
	{
		return error;
	}

/* Save the byte order for later. */

	if ((m_Info.byte_order = header.byte_order) != BYTE_ORDER_IBM)
	{
		swap_word(&header.version);
	}

/* Make sure header is reasonable */

	if (header.version != TIFF_VERSION)
	{
		return ERRORCODE_IllegalType;
	}

/* Start at the first IFD in the TIFF file. */

	lIfdAddress = header.ifd0_address;

/* Step through and read in all the IFD's */

	while (lIfdAddress != 0L)
	{
		if (header.byte_order != BYTE_ORDER_IBM)
		{
			swap_long(&lIfdAddress);
		}

	/* Move to the beginning of the IFD. */
	/* Read in top of IFD (entry count) */

		if ((error = m_pDevice->seek(lIfdAddress, ST_DEV_SEEK_SET)) != ERRORCODE_None
			 || (error = m_pDevice->read(&ifd, sizeof(TIFF_IFD))) != ERRORCODE_None)
		{
			return error;
		}

		if (header.byte_order != BYTE_ORDER_IBM)
		{
			swap_word(&(ifd.entry_count));
		}

	/* Read in the entries. */

		for (wEntryCount = ifd.entry_count; wEntryCount != 0; wEntryCount--)
		{	
			m_pDevice->tell(&lEntryOffset);

		/* Read in entry */

			if ((error = m_pDevice->read(&entry, sizeof(TIFF_FILE_ENTRY))) != ERRORCODE_None)
			{
				return error;
			}

		/* Correct the byte order if it is wrong. */

			if (m_Info.byte_order != BYTE_ORDER_IBM)
			{
				swap_word((LPWORD)&(entry.tag));
				swap_word((LPWORD)&(entry.type));
				swap_long(&(entry.length));
			}

		/* What kind of field have we got? */

			if ((pEntry = FindEntry(entry.tag)) != NULL_TIFF_ENTRY)
			{
			/* Copy the data. THIS IS A STRUCTURE COPY. */

				pEntry->entry = entry;

			/* Say this entry was read. */

				pEntry->entry_valid = TRUE;
				pEntry->entry_offset = lEntryOffset;
				if (pEntry->entry.length == 0)
				{
				/* This is a fix for some bad TIFF files. */
					pEntry->entry.length = 1;
				}
			}
		}

	/* Read the next IFD offset. */

#ifdef MULTIPLE_IFDS
		if (read(file, &lIfdAddress, sizeof(lIfdAddress)) != sizeof(lIfdAddress))
		{
			lIfdAddress = 0L;			/* Just terminate. */
		}
#else
		lIfdAddress = 0L;				/* Always quit after the first IFD */
#endif
	}

	m_Info.valid = TRUE;
	return ERRORCODE_None;
}
	
/*
//	Read the info for a TIFF file.
*/

ERRORCODE near CTIFFInfo::ReadInfo(LPINT pWidth, LPINT pHeight, \
								LPINT pXResolution, LPINT pYResolution)
{
	TIFF_DATA Data;
	TIFF_ENTRY* pEntry;
	ERRORCODE error;

	if (m_Info.valid)
	{
		return ERRORCODE_None;
	}

/* Read all tags. */

	if ((error = ReadTags()) != ERRORCODE_None)
	{
		return error;
	}

/* Read the image resolutions. */

/*
// A particular TIFF writer only writes one resolution. So we check both
// in the correct order.
*/

	if (GetData(TIFF_XResolution, 0L, &Data) == ERRORCODE_None
			|| GetData(TIFF_YResolution, 0L, &Data) == ERRORCODE_None)
	{
		if (Data.Rational.denominator == 0)
		{
			Data.Rational.denominator = 1;
		}
		*pXResolution = (int)((Data.Rational.numerator) /
				(Data.Rational.denominator));
	}

	if (GetData(TIFF_YResolution, 0L, &Data) == ERRORCODE_None
			|| GetData(TIFF_XResolution, 0L, &Data) == ERRORCODE_None)
	{
		if (Data.Rational.denominator == 0)
		{
			Data.Rational.denominator = 1;
		}
		*pYResolution = (int)((Data.Rational.numerator) /
				(Data.Rational.denominator));
	}

/* Read the width. */

	if (GetData(TIFF_ImageWidth, 0L, &Data) != ERRORCODE_None)
	{
	/* No width! */
		return ERRORCODE_IllegalType;
	}
	*pWidth = m_Info.width = Data.Short;

/* Read the length. */

	if (GetData(TIFF_ImageLength, 0L, &Data) != ERRORCODE_None)
	{
	/* No height! */
		return ERRORCODE_IllegalType;
	}
	*pHeight = m_Info.height = Data.Short;

/* Read the number of bits per sample. */

	GetData(TIFF_BitsPerSample, 0L, &Data);

	m_Info.bits_per_sample = Data.Short;

/*
// Read the number of samples per pixel.
*/

	GetData(TIFF_SamplesPerPixel, 0L, &Data);

	m_Info.samples_per_pixel = Data.Short;
	m_Info.byte_width=((m_Info.width*m_Info.bits_per_sample+7)/8)*Data.Short;

/* Read the photometric interpretation. */

	if (GetData(TIFF_PhotometricInterp, 0L, &Data) == ERRORCODE_None)
	{
		m_Info.photometric = Data.Short;
	}
	else
	{
	/* No interpretation! */
		m_Info.photometric = 0;
	}

/*
// Read the number of rows per strip.
// No error check is done because this value has a default, and if it got
// this far, the tags have been read.
*/

	GetData(TIFF_RowsPerStrip, 0L, &Data);

	m_Info.rows_per_strip = Data.Short;
	if ((Data.Short == -1) || (Data.Short > m_Info.height))
	{
		m_Info.rows_per_strip = m_Info.height;
	}

/*
// Read the planar configuration.
// No error check is done because this value has a default, and if it got
// this far, the tags have been read.
*/

	GetData(TIFF_PlanarConfiguration, 0L, &Data);

	m_Info.planar_configuration = Data.Short;

	if (m_Info.samples_per_pixel != 1 && m_Info.planar_configuration == 2)
	{
	/* Bad configuration! */
		return ERRORCODE_IllegalType;
	}

/*
// Read the compression value.
// No error check is done because this value has a default, and if it got
// this far, the tags have been read.
*/

	GetData(TIFF_Compression, 0L, &Data);
	m_Info.compression = Data.Short;

/*
// Read the strip offsets value.
// We don't use get_tiff_data here because we want the value in
// the tag field regardless of whether it is actual data or a pointer
// to disk data. get_tiff_data would actually read the disk data if we
// used it.
*/

	pEntry = FindEntry(TIFF_StripOffsets);

	if (!pEntry->entry_valid)
	{
	/* No default for strip offsets! */
		return ERRORCODE_IllegalType;
	}

	m_Info.strip_offsets = pEntry->entry.value_offset;

	if (m_Info.byte_order != BYTE_ORDER_IBM)
	{
		swap_long(&m_Info.strip_offsets);
	}

	if (pEntry->entry.length == 1 && pEntry->entry.type == TIFF_SHORT)
	{
	/* Make sure we only read the short value. */
		m_Info.strip_offsets &= 0x0FFFFL;
	}

/*
// Read the gray response curve data or color map data if it exists.
*/

	m_Info.palette_offset = -1L;

	if (m_Info.bits_per_sample > 1)
	{
	/* Gray scale! */

		if ((pEntry = FindEntry(TIFF_GrayResponseCurve))->entry_valid)
		{
			LONG curve_length;

		/* We have a curve. */

			m_Info.palette_offset = pEntry->entry.value_offset;
			curve_length = pEntry->entry.length;
			if (m_Info.byte_order != BYTE_ORDER_IBM)
			{
				swap_long(&curve_length);
				swap_long(&m_Info.palette_offset);
			}

			m_Info.gray_response_length = (SHORT)curve_length;
			GetData(TIFF_GrayResponseUnit, 0L, &Data);
			m_Info.gray_response_unit = Data.Short;

		/* Don't invert data. */

			m_pEntries[Tindex_PhotometricInterp].entry.value_offset =
							m_Info.photometric = 1;
		}
		else if ((pEntry = FindEntry(TIFF_ColorMap))->entry_valid)
		{
			m_Info.gray_response_unit = -1;		/* Flag as ColorMap */

		/* We have a color map. Store the offset. */

			m_Info.palette_offset = pEntry->entry.value_offset;

		/* Fix the offset if necessary. */

			if (m_Info.byte_order != BYTE_ORDER_IBM)
			{
				swap_long(&m_Info.palette_offset);
			}
		}
	}

	GetData(TIFF_Predictor, 0L, &Data);
	m_Info.predictor = Data.Short;

	return ERRORCODE_None;
}

/*
// This function opens a TIFF file and reads in all the tags.
// This function must be called before the other TIFF functions will
// operate. Also returned is additional information including dimension and
// resolution.
*/

ERRORCODE CTIFFInfo::Open(StorageDevicePtr pDevice, BOOL fForceTagRead, \
					LPINT pWidth, LPINT pHeight, \
					LPINT pXResolution, LPINT pYResolution)
{
/* Create the entries array. */

	m_pEntries = new TIFF_ENTRY[NUM_FIELDS];

	if (m_pEntries == NULL)
	{
		return ERRORCODE_Memory;
	}

/* Remember the device. */

	m_pDevice = pDevice;

/* If we must read (or re-read) the data, reset variables. */

	if (fForceTagRead)
	{
		m_Info.valid = FALSE; /* This is a new file. */
		m_Info.last_strip_read = -1L;
	}

/* Read the info. */

	return ReadInfo(pWidth, pHeight, pXResolution, pYResolution);
}

/*
// This function will close the object (freeing all resources).
// If will not necessarily do anything to the device.
*/

void CTIFFInfo::Close(void)
{
	delete [] m_pEntries;
	m_pEntries = NULL;
	m_pDevice = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CTIFFImageDriver

CTIFFImageDriver* CTIFFImageDriver::me = NULL;

/*
// A routine to read bytes during a LZW decompress session.
*/

BOOL CTIFFImageDriver::tiff_next_byte(LPBYTE byte)
{
	return me->file.read(byte, sizeof(*byte)) == ERRORCODE_None;
}

/*
//	Seek to a line in a TIFF file.
*/

ERRORCODE near CTIFFImageDriver::seek_tiff_line(VOID)
{
	ST_DEV_POSITION strip_offset;
	ERRORCODE error;

//	od("Seek to strip %d\r\n", seek_strip);

/* Get the strip offset so we know the beginning of the strip. */

	strip_offset = m_pInfo->m_Info.strip_offsets;

/*
// If this is a multiple strip TIFF, this offset is actually an offset to an
// array of offsets (one for each strip). We need to seek into this array and
// extract the real offset for the strip.
*/

	if (rows_per_strip != m_pInfo->m_Info.height)
	{
	/*
	// We need to read the next strip to seek to.
	// Strip offsets and data are usually in two separate places in the file.
	// We also don't want to buffer the strip data since we only want to read
	// a DWORD's worth. So we disable buffering while we "go underneath" the
	// buffering to pull out the next strip offset. This should improve data
	// reading throughput.
	*/

		strip_offset += seek_strip << 2;				/* (*4) */

//		od("Unbuffered read strip %ld (offset: %ld)...", seek_strip, strip_offset);
		if ((error = file.unbuffered_read(strip_offset /*+ m_pInfo->offset*/, &strip_offset, sizeof(strip_offset))) != ERRORCODE_None)
		{
			return error;
		}
//		od("Got strip offset %ld\r\n", strip_offset);

	/* If this is not IBM order, swap the offset now. */

		if (m_pInfo->m_Info.byte_order != BYTE_ORDER_IBM)
		{
			swap_long(&strip_offset);
		}
	}

/*
// After the seek, we will be at the first row.
*/

	row_in_strip = 0;

/*
// If we are starting part way down and not compressing,
// move down as part of the seek.
*/

	if (compression == TIFF_COMP_NONE && seek_row != 0)
	{
		strip_offset += (LONG)byte_width*(LONG)seek_row;
	/* We are already there! */
		row_in_strip = seek_row;
	}

/*
// Seek to where we want.
*/

	file.seek(strip_offset, ST_DEV_SEEK_SET);

	current_line = (SHORT)seek_strip * rows_per_strip + row_in_strip;

//	printf("S");
	return ERRORCODE_None;
}

/*
// Read a (possibly) partial TIFF block.
// We do this by reading lines from the TIFF file and passing them to
// process_tiff_line().
*/

ERRORCODE near CTIFFImageDriver::read_tiff_line(LPBYTE buffer)
{
	ERRORCODE error = ERRORCODE_None;
	SHORT skip = seek_row - row_in_strip;

/* We will be here when done. */

	current_line += skip;
	row_in_strip = seek_row;

//	od("Read line %d after skipping %d lines\r\n", current_line, skip);

	switch((WORD)(compression))
	{
		case TIFF_COMP_NONE:
		{
		/*
 		// If seek_tiff_line() worked, we should always be at the right line
		// when we get here. (Just a little trivia.)
		*/

			if (skip != 0)
			{
			/* Seek to where we want to go. */
				file.seek(skip*byte_width, ST_DEV_SEEK_CUR);
			}

			error = file.read(buffer, byte_width);

			break;
		}
		case TIFF_COMP_CCITT:
		{
			me = this;

			while (skip-- > 0)
			{
				uncompress(tiff_next_byte, buffer, m_pInfo->m_Info.width);
			}

			uncompress(tiff_next_byte, buffer, m_pInfo->m_Info.width);
			break;
		}
		case TIFF_COMP_LZW:
		{
			error = lzw->read((ULONG)skip*byte_width, buffer, byte_width);
			break;
		}
		case TIFF_COMP_PACKBITS:
		{
//			unPackBits(tiff_next_byte, buffer, (m_pInfo->width+7)/8);

			me = this;
			while (skip-- > 0)
			{
				unPackBits(tiff_next_byte, buffer, byte_width);
			}
			unPackBits(tiff_next_byte, buffer, byte_width);

			error = ERRORCODE_None;
			break;
		}
		default:
		{
			error = ERRORCODE_IllegalType;
			break;
		}
	}
	return error;
}

/*
// Initialize the gray-scale palette.
*/

ERRORCODE near CTIFFImageDriver::initialize_palette(PALETTE_INFO_PTR palette)
{
	LPBYTE cp;
 	BYTE c;
	LONG data_offset = m_pInfo->m_Info.palette_offset;
	ERRORCODE error = ERRORCODE_None;
	SHORT i, gap, max, curve_length;
	SHORT colors = palette->m_nColors;

	cp = (LPBYTE)palette->m_pData;

	if (data_offset != -1L)
	{
		if (m_pInfo->m_Info.gray_response_unit == -1)
		{
		/*
		// This is a palette image.
 		// The size of the palette should be what we have allocated for our
		// palette. Unfortunately, the file palette is stored planar and we
		// need RGB interleaved. So we have to read it elsewhere, then copy it
		// to our palette, interleaving it.
		*/
			if ((error = file.seek(data_offset, ST_DEV_SEEK_SET)) == ERRORCODE_None)
			{
				USHORT size = colors * sizeof(RGBTRIPLE) * sizeof(SHORT);
				LPBYTE ptr, s;
				SHORT offset = colors*sizeof(SHORT);

				ptr = NULL;
				TRY
					ptr = new BYTE[size];
				END_TRY
				if (ptr != NULL)
				{
					s = ptr;
					if ((error = file.read(s, size)) == ERRORCODE_None)
					{
					/*
					// Copy it over, interleaving it as we go.
					// Even though the source is an array of USHORTs, we treat it as
					// an array (twice as long) of UCHARs. We only want the high byte
					// of each component, so that's all we take.
	 				*/
						s++;						/* Offset to high byte of USHORTs */

						for (i = colors; i--; )
						{
							*cp++ = s[0];
							*cp++ = s[offset];
							*cp++ = s[offset*2];

						/* Advance to next short. */
							s += sizeof(SHORT);
						}
						palette->m_fRead = TRUE;
					}
					delete [] ptr;
				}
			}
		}
		else if (m_pInfo->m_Info.gray_response_unit < 4
						&& m_pInfo->m_Info.gray_response_unit > 0)
		{
			LPWORD ptr;
			LPWORD sp;
			static SHORT range_max[] = {20, 200, 2000, 20000 };

		/* Extract the pertinent information. */

			curve_length = m_pInfo->m_Info.gray_response_length;

		/* Read the gray response curve. */

			ptr = NULL;
			TRY
				ptr = new WORD[curve_length];
			END_TRY
			if (ptr != NULL)
			{
				sp = ptr;
				if ((error = file.seek(data_offset, ST_DEV_SEEK_SET)) == ERRORCODE_None)
				{
					if ((error = file.read(sp, curve_length*2)) == ERRORCODE_None)
					{
					/* Get the gray response unit and range end. */

						max = range_max[m_pInfo->m_Info.gray_response_unit-1];

					/*
					// Translate the data.
					// The gap variable is used for those curves less than 256
					// (e.g. 2, 4 bit gray-scales).
					// If we only are using 16 gray levels, we need to replicate each
					// value (256/16) or 16 times, etc.
					*/

						gap = colors/curve_length * 3;		/* "*3" for RGBTRIPLE */

						for (i = curve_length; i > 0; i--)
						{
							c = (BYTE)MulDiv(max - *sp++, 255, max);

							memset(cp, c, gap);
							cp += gap;
						}
						palette->m_fRead = TRUE;
					}
				}
				delete [] ptr;
			}
		}
	}
	return error;
}

/*
// The constructor for the TIFF driver.
*/

CTIFFImageDriver::CTIFFImageDriver(CTIFFInfo* pInfo)
{
	m_pInfo = pInfo;
	lzw = NULL;
}

/*
// The destructor for the TIFF driver.
*/

CTIFFImageDriver::~CTIFFImageDriver()
{
	delete lzw;
}

/*
// The image_info() method for TIFF files.
*/

ERRORCODE CTIFFImageDriver::image_info(IMAGE_INFO_PTR pInfo)
{
	UCHAR planes, bpp;
	IMAGE_TYPE image_type;

	bpp = (BYTE)m_pInfo->m_Info.bits_per_sample;
	planes = (BYTE)m_pInfo->m_Info.samples_per_pixel;

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

	pInfo->planes = planes;
	pInfo->bpp = bpp;
	pInfo->image_type = image_type;
	pInfo->width = m_pInfo->m_Info.width;
	pInfo->height = m_pInfo->m_Info.height;

	return ERRORCODE_None;
}

/*
// The open_image_file() method for TIFF files.
*/

ERRORCODE CTIFFImageDriver::open_image(PALETTE_INFO_PTR palette)
{
//	int junk;
	ERRORCODE error = ERRORCODE_None;

//	if ((error = m_pInfo->Open(&file, FALSE, &junk, &junk, &junk, &junk)) == ERRORCODE_None)
	{
	/* If this is grayscale, initialize the translate buffer. */

		if (our_info.bpp != 1 && our_info.planes == 1)
		{
			if ((error = initialize_palette(palette)) != ERRORCODE_None)
			{
				return error;
			}
		}

	/* Constant variables. */

		compression = m_pInfo->m_Info.compression;

		rows_per_strip = m_pInfo->m_Info.rows_per_strip;
		byte_width = m_pInfo->m_Info.byte_width;
		predictor = m_pInfo->m_Info.predictor == 2;
		invert = m_pInfo->m_Info.photometric == 0;

	/* Last row mask (which is OR'ed in - data is kept inverted). */

		last_mask = (BYTE)(0x00ff >> (m_pInfo->m_Info.width & 7));

		if (last_mask == 0xff)
		{
			last_mask = 0;
		}

	/*
	// Create the lzw parser if necessary.
	*/

		if (compression == TIFF_COMP_LZW)
		{
			if ((lzw = new LZWParser(&file, 8, FALSE)) == NULL
						|| !lzw->valid())
			{
				return ERRORCODE_Memory;
			}
		}

	/*
 	// Get us off to a good start.
	*/

		in_strip = -1;
		m_pInfo->m_Info.last_strip_read = -1L;		/* Kill this. */

	/* Begin buffered I/O. */

		ST_DEV_POSITION filesize;
		if ((error = file.length(&filesize)) == ERRORCODE_None)
		{
			ST_DEV_IO_SIZE size;
			if (filesize < 65000)
			{
				size = (ST_DEV_IO_SIZE)filesize;
			}
			else
			{
				size = 32767;
			}

			file.set_read_buffer(size);
			current_line = -1;
		}
	}
	return error;
}

/*
// The read_image_block() method for TIFF files.
*/

ERRORCODE CTIFFImageDriver::read_lines(int line, LPBYTE buffer, int count, LPBYTE pMask)
{
	ASSERT(pMask == NULL);

	ERRORCODE error;

	while (count-- > 0)
	{
	/* Compute the strip number and the row number within the strip. */

		seek_strip = (long)(line / rows_per_strip);
		seek_row = line % rows_per_strip;

	/* See if we backtracked in this strip. */

		if (seek_strip == in_strip && seek_row < row_in_strip)
		{
			in_strip = -1;
		}

	/* See if we're not in the right strip. */

		if (seek_strip != in_strip)
		{
		/*
		//	Optimization. If the last line read is just before this one,
		//	seek there directly.
		*/

			if ((seek_strip == m_pInfo->m_Info.last_strip_read)
				&& (seek_row == m_pInfo->m_Info.last_row_read+1)
				&& (compression != TIFF_COMP_LZW))
			{
				in_strip = seek_strip;
				row_in_strip = seek_row;
				file.seek(m_pInfo->m_Info.next_line_offset, ST_DEV_SEEK_SET);
			}
			else
			{
				if (in_strip != seek_strip)
				{
				/* Seek the beginning of the strip. */

					if ((error = seek_tiff_line()) != ERRORCODE_None)
					{
						m_pInfo->m_Info.last_strip_read = -1L;
						return error;
					}

				/* We are in this strip now. */

					in_strip = seek_strip;

					if (compression == TIFF_COMP_LZW)
					{
					/* Reset to the beginning of this strip. */
						lzw->reset();
					}
				}
			}
		}

	/* Now read the line. */

		if ((error = read_tiff_line(buffer)) != ERRORCODE_None)
		{
			break;
		}

	/* Handle the predictor. */

		if (predictor)
		{
			LPBYTE s, d;
			SHORT i;

			for (s = buffer,
						d = buffer+our_info.planes,
						i = byte_width-our_info.planes;
					i-- > 0;)
			{
				*d++ += *s++;
			}
		}

	/* If the data needs to be inverted, invert it now. */

		if (invert)
		{
			LPBYTE ptr;
			SHORT	i;

			ptr = buffer;
			for (i = byte_width; i > 0; i--)
			{
				*ptr = (BYTE)~*ptr;
				ptr++;
			}
		}

	/* Fix the last byte. */

		if (our_info.image_type == IMAGE_BILEVEL)
		{
			*(buffer + byte_width - 1) |= last_mask;
		}

	/* We're on the next line. */

		row_in_strip++;

		current_line++;
		buffer += byte_width;
	}

	if (error == ERRORCODE_None)
	{
		m_pInfo->m_Info.last_row_read = row_in_strip;
		m_pInfo->m_Info.last_strip_read = in_strip;
		file.tell(&m_pInfo->m_Info.next_line_offset);
	}
	return error;
}

/*
// The close_image_file() method for TIFF files.
*/

VOID CTIFFImageDriver::close_image(VOID)
{
//	m_pInfo->Close();

	if (lzw != NULL)
	{
		delete lzw;
		lzw = NULL;
	}
}

#include "util.h"       // For ROUNDTO32BITS

#define  TIFF_WRITE(d)     ERRORCODESave (pdev->huge_write ((void *)&d, sizeof(d)));
#define  TIFF_WRITEP(p, s) ERRORCODESave (pdev->huge_write ((void *)p, s));
#define  TIFF_TELL(d)      ERRORCODESave (pdev->tell (&d));
#define  TIFF_SEEK(d)      ERRORCODESave (pdev->seek (d, ST_DEV_SEEK_SET));
#define  TIFF_READ(d)      ERRORCODESave (pdev->huge_read (&d, sizeof(d)));

void
CDIBtoTIFFWriter::CreateTIFF (StorageDevice *pdev, CDIBInfo *pInfo, long dpi)
{
   BYTE        *pLine = NULL;
   long        i;
   BITMAPINFO  *pbi = pInfo->BitmapInfo ();
   DWORD       DataSize = pbi->bmiHeader.biSizeImage;
   long        Rows = pbi->bmiHeader.biHeight;
   
   BOOL        UpsideDown = (Rows > 0) ? TRUE : FALSE;
   if (Rows < 0)
      Rows = -Rows;
   DWORD       RowBytes = (pbi->bmiHeader.biBitCount * pbi->bmiHeader.biWidth) >> 3;
//   CString     Software = "PrintMaster ";
    
   ST_DEV_POSITION   LocationBitsPerSample;
   ST_DEV_POSITION   LocationStripOffset;
   ST_DEV_POSITION   LocationByteCount;
   ST_DEV_POSITION   LocationResolutionX;
   ST_DEV_POSITION   LocationResolutionY;
//   ST_DEV_POSITION   LocationSoftware;
   ST_DEV_POSITION   StartImageData;

   // Build a TIFF header
   TIFF_HEADER       th                   = {'II', TIFF_VERSION, Util::RoundToWORD ((DWORD)sizeof (TIFF_HEADER))};
   // Sorted by tag order (?)
   TIFF_IFD          ifd                  = {13};  // Number of TIFF_FILE_ENTRYs
   
   TIFF_FILE_ENTRY   TE_SubFile           = {TIFF_NewSubfileType,    TIFF_LONG,     1,    0};
   TIFF_FILE_ENTRY   TE_Width             = {TIFF_ImageWidth,        TIFF_LONG,     1,    pbi->bmiHeader.biWidth};
   TIFF_FILE_ENTRY   TE_Height            = {TIFF_ImageLength,       TIFF_LONG,     1,    pbi->bmiHeader.biHeight};
   TIFF_FILE_ENTRY   TE_BitsPerSample     = {TIFF_BitsPerSample,     TIFF_SHORT,    3,    0};  // Fill in later
   TIFF_FILE_ENTRY   TE_Compression       = {TIFF_Compression,       TIFF_SHORT,    1,    TIFF_COMP_NONE};
   TIFF_FILE_ENTRY   TE_Photometric       = {TIFF_PhotometricInterp, TIFF_SHORT,    1,    2};   // RGB 
   TIFF_FILE_ENTRY   TE_StripOffset       = {TIFF_StripOffsets,      TIFF_LONG,     Rows, 0};   // Fill in later
   TIFF_FILE_ENTRY   TE_SamplesPerPixel   = {TIFF_SamplesPerPixel,   TIFF_SHORT,    1,    pbi->bmiHeader.biBitCount >> 3}; // 3
   TIFF_FILE_ENTRY   TE_RowsPerStrip      = {TIFF_RowsPerStrip,      TIFF_LONG,     1,    1};
   TIFF_FILE_ENTRY   TE_StripByteCounts   = {TIFF_StripByteCounts,   TIFF_LONG,     Rows, 0};   // Fill in later
   TIFF_FILE_ENTRY   TE_ResolutionX       = {TIFF_XResolution,       TIFF_RATIONAL, 1,    0};   // Fill in later
   TIFF_FILE_ENTRY   TE_ResolutionY       = {TIFF_YResolution,       TIFF_RATIONAL, 1,    0};   // Fill in later
   TIFF_FILE_ENTRY   TE_ResolutionType    = {TIFF_ResolutionUnit,    TIFF_LONG,     1,    2};   // Inches
//   TIFF_FILE_ENTRY   TE_Software          = {TIFF_Software,          TIFF_ASCII,    Software.GetLength () + 1,    0}; // Fill in later
   DWORD             TE_Terminator        = {0};   

   TRY
   {
      TIFF_WRITE (th);

      TIFF_WRITE (ifd);
      TIFF_WRITE (TE_SubFile);
      TIFF_WRITE (TE_Width);
      TIFF_WRITE (TE_Height);
      // Remember where this is in the file
      TIFF_TELL (LocationBitsPerSample);
      TIFF_WRITE (TE_BitsPerSample);
      TIFF_WRITE (TE_Compression);
      TIFF_WRITE (TE_Photometric);
      // Remember where this is in the file
      TIFF_TELL (LocationStripOffset);
      TIFF_WRITE (TE_StripOffset);
      TIFF_WRITE (TE_SamplesPerPixel);
      TIFF_WRITE (TE_RowsPerStrip);
      // Remember where this is in the file
      TIFF_TELL (LocationByteCount);
      TIFF_WRITE (TE_StripByteCounts);
      // Remember where this is in the file
      TIFF_TELL (LocationResolutionX);
      TIFF_WRITE (TE_ResolutionX);
      // Remember where this is in the file
      TIFF_TELL (LocationResolutionY);
      TIFF_WRITE (TE_ResolutionY);
      TIFF_WRITE (TE_ResolutionType);
      // Remember where this is in the file
//   TIFF_TELL (LocationSoftware);
//   TIFF_WRITE (TE_Software);
  
      TIFF_WRITE (TE_Terminator);
   
      // Remember where this is in the file
//   TIFF_TELL (TE_Software.value_offset);
//   if(ERRORCODESave (pdev->huge_write ((void*)(const char*)Software, Software.GetLength () + 1)) != ERRORCODE_None)return;
   
      // Remember where this is in the file
      TIFF_TELL (TE_BitsPerSample.value_offset);
      BPS   bps (8);
      TIFF_WRITE (bps);
      // Remember where this is in the file
      TIFF_TELL (TE_StripByteCounts.value_offset);
      for (i = 0; i < Rows; i ++)
         TIFF_WRITE (RowBytes);   
   
      // Remember where this is in the file
      TIFF_TELL (TE_StripOffset.value_offset);
      // Make room for strip offset array
      TIFF_SEEK (TE_StripOffset.value_offset + Rows * sizeof (long));
   
      RATIONAL  ResolutionX = {300, 1}, ResolutionY = {300, 1};
      if (dpi > 0)
      {
         ResolutionX.numerator = ResolutionY.numerator = dpi;
      }      

      TIFF_TELL (TE_ResolutionX.value_offset);
      TIFF_WRITE (ResolutionX);
      TIFF_TELL (TE_ResolutionY.value_offset);
      TIFF_WRITE (ResolutionY);

      // Remember where this is in the file
      TIFF_TELL (StartImageData);
      // Allocate strip offset array
      CreateStripOffsetArray (Rows);
      pLine = new BYTE [RowBytes];

      LPBYTE   pRead;
      
      if (UpsideDown == TRUE)
      {
         pRead = pInfo->BitmapData() + (Util::RoundToDWORD (RowBytes) * (Rows - 1));
         for (i = 0; i < Rows; i ++)
         {
            TIFF_TELL (pStripOffsetArray [i]); 
            for (DWORD j = 0; j < RowBytes; j += 3)
            {
               pLine [j + 0] = pRead [j + 2];
               pLine [j + 1] = pRead [j + 1];
               pLine [j + 2] = pRead [j + 0];
            }
      
            TIFF_WRITEP (pLine, RowBytes);
      
            pRead -= Util::RoundToDWORD (RowBytes);
         }
      }
      else
      {
         pRead = pInfo->BitmapData();
         for (i = 0; i < Rows; i ++)
         {
            TIFF_TELL (pStripOffsetArray [i]); 
            for (DWORD j = 0; j < RowBytes; j += 3)
            {
               pLine [j + 0] = pRead [j + 2];
               pLine [j + 1] = pRead [j + 1];
               pLine [j + 2] = pRead [j + 0];
            }
      
            TIFF_WRITEP (pLine, RowBytes);
      
            pRead += Util::RoundToDWORD (RowBytes);
         }
      }

      // Go back and fill in this stuff
   //   TIFF_SEEK (LocationSoftware);
   //   TIFF_WRITE (TE_Software);

      TIFF_SEEK (LocationBitsPerSample);
      TIFF_WRITE (TE_BitsPerSample);

      TIFF_SEEK (LocationResolutionX);
      TIFF_WRITE (TE_ResolutionX);

      TIFF_SEEK (LocationResolutionY);
      TIFF_WRITE (TE_ResolutionY);

      TIFF_SEEK (LocationByteCount);
      TIFF_WRITE (TE_StripByteCounts);

      TIFF_SEEK (LocationStripOffset);
      TIFF_WRITE (TE_StripOffset);

      TIFF_SEEK (TE_StripOffset.value_offset);
      TIFF_WRITEP (pStripOffsetArray, StripOffsetArraySize * sizeof (long));
   }
   CATCH(CErrorcodeException, e)
   {
      ERRORCODESave (e->m_error);
   }
   CATCH(CMemoryException, e)
   {
      ERRORCODESave (ERRORCODE_Memory);
   }
   AND_CATCH_ALL (e)
   {
      ERRORCODESave (ERRORCODE_Fail);
   }
   END_CATCH_ALL

   if (pLine != NULL)
      delete [] pLine;
   
   KillStripOffsetArray ();
}
