/*
// $Header: /PM8/App/FRAMEREC.CPP 1     3/03/99 6:05p Gbeddow $
//
// Frame record routines.
//
// $Log: /PM8/App/FRAMEREC.CPP $
// 
// 1     3/03/99 6:05p Gbeddow
// 
// 5     10/12/98 4:33p Jayn
// Rearranged some includes to make extraction easier.
// 
// 4     4/30/98 4:47p Jayn
// Replaced FillFormat with FillFormatV1 (old) and FillFormatV2 (new).
// The new one has the gradient members.
// 
// 3     4/29/98 6:00p Rlovejoy
// Draw objects use fill objects.
// 
// 2     1/13/98 8:54a Jayn
// TRY block around exceptional code.
// 
//    Rev 1.0   14 Aug 1997 15:21:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:38:34   Fred
// Initial revision.
// 
//    Rev 1.45   22 Jul 1997 10:32:20   Jay
// SetAttributes() now handles undefined (multi-defined) case.
// Won't crash if m_nColumns gets set to bad value.
// 
//    Rev 1.44   29 May 1997 08:45:02   Fred
// New effects support
// 
//    Rev 1.43   28 May 1997 11:05:54   Fred
// Frame background effects
// 
//    Rev 1.42   27 May 1997 11:06:50   Fred
// New effects
// 
//    Rev 1.41   23 May 1997 17:05:40   Fred
// New effects
// 
//    Rev 1.40   15 May 1997 16:42:48   Fred
// Overflow prevention
// 
//    Rev 1.39   09 Apr 1997 14:51:10   Fred
// Start of new font caching (fonts and metrics)
// 
//    Rev 1.38   24 Mar 1997 16:14:34   Jay
// Got rid of a memory leak.
// 
//    Rev 1.37   17 Mar 1997 14:48:08   Jay
// Warp shape palette and support.
// 
//    Rev 1.36   12 Mar 1997 16:52:20   Fred
// Up/Down arrows now work in warped and rotated text boxes
// 
//    Rev 1.35   12 Mar 1997 14:53:56   Fred
// Changes for flipped warped and rotated text
// 
//    Rev 1.34   07 Feb 1997 08:45:14   Fred
// Passes GetCharacterFormatting() to AdvanceWidth()
// 
//    Rev 1.33   28 Jan 1997 16:56:42   Fred
// Start of component drawing
// 
//    Rev 1.32   16 Jan 1997 08:59:54   Fred
// Switch to warp field data record
// 
//    Rev 1.31   09 Jan 1997 16:49:50   Fred
//  
// 
//    Rev 1.30   06 Jan 1997 10:15:28   Fred
// Start of new text object
// 
//    Rev 1.29   06 Jan 1997 08:34:34   Jay
// Optimizations for word and line array access.
// 
//    Rev 1.28   22 Jul 1996 18:28:56   Jay
// Stretched frame has 1 column
// 
//    Rev 1.27   18 Jul 1996 11:48:10   Jay
// ReleaseSubrecords()
// 
//    Rev 1.26   17 Jul 1996 18:09:12   Jay
//  
// 
//    Rev 1.25   15 Jul 1996 17:46:00   Jay
//  
// 
//    Rev 1.24   15 Jul 1996 11:34:26   Jay
//  
// 
//    Rev 1.23   12 Jul 1996 16:47:50   Jay
// Text wrap
// 
//    Rev 1.22   11 Jul 1996 17:44:22   Jay
//  
// 
//    Rev 1.21   10 Jul 1996 10:23:50   Jay
// Text states no longer allocated.
// 
//    Rev 1.20   09 Jul 1996 15:37:14   Jay
//  
// 
//    Rev 1.19   08 Jul 1996 17:51:32   Jay
// Linked frames, etc.
// 
//    Rev 1.18   03 Jul 1996 14:30:26   Jay
//  
// 
//    Rev 1.17   02 Jul 1996 11:24:52   Jay
//  
// 
//    Rev 1.16   28 Jun 1996 16:18:16   Jay
//  
// 
//    Rev 1.15   27 Jun 1996 13:27:38   Jay
//  
// 
//    Rev 1.14   26 Jun 1996 14:21:38   Jay
//  
// 
//    Rev 1.13   26 Jun 1996 09:06:28   Jay
//  
// 
//    Rev 1.12   25 Jun 1996 10:25:32   Jay
//  
// 
//    Rev 1.11   24 Jun 1996 18:06:12   Jay
//  
// 
//    Rev 1.10   21 Jun 1996 17:49:36   Jay
//  
// 
//    Rev 1.9   21 Jun 1996 15:43:38   Jay
// Stretch flow tweaks.
// 
//    Rev 1.8   21 Jun 1996 14:30:26   Jay
// Stretch text, etc.
// 
//    Rev 1.7   20 Jun 1996 16:15:56   Jay
// Frame refresh offsets
// 
//    Rev 1.6   19 Jun 1996 17:32:12   Jay
//  
// 
//    Rev 1.5   17 Jun 1996 13:19:46   Jay
//  
// 
//    Rev 1.4   13 Jun 1996 15:53:04   Jay
//  
// 
//    Rev 1.3   05 Jun 1996 10:28:20   Jay
//  
// 
//    Rev 1.2   04 Jun 1996 17:37:54   Jay
//  
// 
//    Rev 1.1   24 May 1996 09:26:56   Jay
// New text code.
// 
//    Rev 1.0   14 Mar 1996 13:48:18   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:07:24   FRED
// Initial revision.
// 
//    Rev 2.1   29 Mar 1995 08:58:28   JAY
// Lots of changes
// 
//    Rev 2.0   07 Feb 1995 15:47:48   JAY
// Initial revision.
// 
//    Rev 1.13   30 Nov 1994 16:33:30   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.12   04 Aug 1994 07:32:44   JAY
// Large model and 32-bit changes
// 
//    Rev 1.11   01 Jun 1994 16:35:20   JAY
// Restructured 'type' parameter passing
// 
//    Rev 1.10   18 Mar 1994 11:07:18   MACDUFF
// Corrected the text height computation (it was wrong when lines
// were of differing heights).
// 
//    Rev 1.9   18 Mar 1994 08:59:28   MACDUFF
// compute_flow_bounds() does not rely on the frame's line count,
// which may not be correct for the text being measured.
// 
//    Rev 1.8   17 Mar 1994 17:08:28   MACDUFF
// compute_flow_bounds() handles new line spacing correctly.
// 
//    Rev 1.7   16 Mar 1994 16:01:38   MACDUFF
// Divorced line spacing from font height
// 
//    Rev 1.6   15 Feb 1994 09:04:58   JAY
// stretch_text now limits text point size to 1000 pt.
// 
//    Rev 1.5   02 Feb 1994 11:31:46   JAY
// Fixed a blowing-up stretching bug.
// 
//    Rev 1.4   01 Feb 1994 14:02:56   JAY
// Debugging stuff with commented out DEBUG_ST.
// 
//    Rev 1.3   01 Feb 1994 10:21:22   JAY
// Fixed a stretch text bug where the style's point size could go to 0.
// 
//    Rev 1.2   13 Jan 1994 17:00:16   JAY
//  
// 
//    Rev 1.1   05 Jan 1994 08:05:44   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:24:58   JAY
// 
*/

#include "stdafx.h"

#include "pmgdb.h"

#include "framerec.h"
#include "frameobj.h"
#include "pararec.h"
#include "textrec.h"

#include <string.h>
#include <math.h>

#include "flexmath.h"
#include "utils.h"
#include "fontsrv.h"
#include "warpfld.h"

/*****************************************************************************/
/*                             Frame record                                  */
/*****************************************************************************/

/*
// The creator for a frame record.
*/

ERRORCODE FrameRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
/* Create the new array record. */

	if ((*record = (DatabaseRecordPtr)(new FrameRecord(number, type, owner, where))) == NULL)
	{
		return ERRORCODE_Memory;
	}
	return ERRORCODE_None;
}

/*
// The constructor for a project preview record.
*/


FrameRecord::FrameRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where)
{
	memset(&record, 0, sizeof(record));
	record.has_new_line_data = TRUE;
}

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE FrameRecord::ReadData(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->read_record(&record, sizeof(record))) == ERRORCODE_None)
	{
		error = line.read(device);
		if (error == ERRORCODE_None)
		{
			if (record.has_new_line_data)
			{
				error = line.read_new_data(device);
			}
			else
			{
				line.set_new_data_defaults();
			}
		}
	}

	return error;
}

/*
// WriteData()
//
// Write the record.
*/

ERRORCODE FrameRecord::WriteData(StorageDevicePtr device)
{
	ERRORCODE error;

	if ((error = device->write_record(&record, sizeof(record))) == ERRORCODE_None)
	{
		error = line.write(device);
		if (error == ERRORCODE_None)
		{
			line.write_new_data(device);
		}
	}
	return error;
}

/*
// SizeofData()
//
// Return the size of the record.
*/

ST_MAN_SIZE FrameRecord::SizeofData(StorageDevicePtr device)
{
	return device->size_record(sizeof(record))
			+ line.size(device);
}

/*
// Read a line entry from a frame.
*/

SHORT FrameRecord::get_line_image(L_INDEX l_index, LINE_PTR lp)
{
	LINE_PTR l;

	if ((l = (LINE_PTR)line.get_element(l_index)) == NULL)
	{
		return -1;
	}
	else
	{
		*lp = *l;
		return 0;
	}
}

/*
// Return the line of a word.
// Pass NULL for lp_ret if no return image is desired.
*/

L_INDEX FrameRecord::line_of_word(W_INDEX w_index, LINE_PTR lp_ret)
{
	L_INDEX l_index;
	LINE_PTR lp;

/* Look through lines for our word. */

	SHORT line_count = line.count();

	for (l_index = 0, lp = (LINE_PTR)line.get_element(0);
						l_index < line_count;
						l_index++, lp++)
	{
		if (w_index <= lp->w_end)
		{
			break;
		}
	}
 
/* See if we found a line. */

	if (l_index == line_count)
	{
	/* This is an unavoidable error. */
		od("line_of_word: line array is inconsistent\r\n");
		l_index = -1;
	}
	else
	{
	/* Return an image of this line if requested. */

		if (lp_ret != NULL)
		{
			*lp_ret = *lp;
		}
	}

	return l_index;
}

/*
// Assign method.
*/

ERRORCODE FrameRecord::assign(DatabaseRecordRef srecord)
{
	ERRORCODE error;

/* Assign the base record first. */

	if ((error = DatabaseRecord::assign(srecord)) == ERRORCODE_None)
	{
	/*
	// Now copy over the data.
	// Note that frames don't own their paragraphs. So we don't duplicate
	// the paragraph.
	*/

		record = ((FrameRef)srecord).record;
		record.paragraph = 0;			// No record yet.
		error = line.assign(((FrameRef)srecord).line);
	}
	return error;
}

/*
// Defines for stretch_text.
*/

#define	MIN_EXPANSION			5
#define	MAX_EXPANSION			15000
#define	MIN_SIZE_FRAC			0x1000
#define	MAX_STRETCH_SIZE		1000

/*
// Compute the bounds for this text when it is flowed.
*/

void FrameRecord::compute_flow_bounds(ParagraphPtr paragraph, PPNT frame_dims, PPNT_PTR text_extent_out, BOOL do_flow)
{
	TEXT_WORD_PTR wp;
	W_INDEX w_index;
	L_INDEX l_index;
	PCOORD tab_size;
	PPNT text_extent;
	LINE line;

	text_extent.x = text_extent.y = 0;

	if ((tab_size = frame_dims.x / 16) == 0)
	{
		tab_size = 1;
	}

	SHORT word_count = paragraph->number_of_words();

	l_index = 0;

	for (w_index = 0, wp = paragraph->get_word(0); w_index < word_count; )
	{
		PCOORD x_offset;
		PCOORD solid_x_offset;

	/* No metrics for this line yet. */

		x_offset = 0;
		solid_x_offset = 0;
		line.ascend = line.descend = line.line_spacing = 0;
		line.w_start = w_index;

		do
		{
			PCOORD width = wp->width;

//			od("(%d : %ld)", w_index, width);

		/* Calculate tab width if necessary. */

			if (wp->type == WORD_TYPE_tab)
			{
				width = ((x_offset + tab_size)/tab_size)*tab_size - x_offset;
			}

		/* See if it's time to break. */

			if (!do_flow)
			{
				if ((wp->flags & WORD_FLAG_override_flow) != 0)
				{
					if (w_index != line.w_start)
					{
						break;
					}
				}
			}

			if (do_flow)
			{
				if (x_offset + width > frame_dims.x			/* doesn't fit */
						&& w_index != line.w_start				/* not first word */
						&& !(wp->type == WORD_TYPE_space
									&& ((wp-1)->type == WORD_TYPE_solid
										 || (wp-1)->type == WORD_TYPE_macro))
						&& width != 0)						/* real word */
				{
					wp->flags |= WORD_FLAG_override_flow;
					break;
				}
			}
			x_offset += width;
			if (wp->type == WORD_TYPE_solid || wp->type == WORD_TYPE_macro)
			{
				solid_x_offset = x_offset;
			}

			if (!do_flow)
			{
				if (w_index != line.w_start)
				{
					wp->flags |= WORD_FLAG_override_keep;
				}
			}

			if (wp->ascend > line.ascend)
			{
				line.ascend = wp->ascend;
			}
			if (wp->descend > line.descend)
			{
				line.descend = wp->descend;
			}
			if (wp->line_spacing > line.line_spacing)
			{
				line.line_spacing = wp->line_spacing;
			}

			w_index++;

			if (wp++->type == WORD_TYPE_break)
			{
				break;
			}

		} while (w_index < word_count);

	/* Update with the metrics for this line. */

		if (text_extent.x < solid_x_offset)
		{
			text_extent.x = solid_x_offset;
		}

		if (l_index == 0)
		{
			text_extent.y += line.ascend;
		}
		else
		{
			text_extent.y += line.line_spacing - line.descend;
		}
		text_extent.y += line.descend;
		
		++l_index;
	}

//	od("\r\n");

	*text_extent_out = text_extent;
}

/*
// Stretch text support.
*/

//#define DEBUG_ST

ERRORCODE FrameRecord::stretch_text(VOID)
{
	PPNT original_dims, current_dims, text_extent;
	DB_RECORD_NUMBER p_record;
	WORD_RANGE wrange;
	ParagraphPtr paragraph;
	PCOORD delta;
	SHORT reference_expansion;
	uint32 reference_size;
	SHORT stretch_expansion;
	uint32 stretch_size;
	int16 shift;
	PCOORD max_y_error, max_x_error;
	PCOORD xerr, yerr;
	int16 iteration;
	ERRORCODE error;

/* Extract pertinent information from the frame. */

	p_record = get_paragraph();

	original_dims = get_stretch_dims();

	current_dims.x = record.bound.x1 - record.bound.x0;
	current_dims.y = record.bound.y1 - record.bound.y0;

	if (get_flags() & FRAME_FLAG_stretch_frame)
	{
		original_dims.x = current_dims.x;
	}

	/* Tentatively, max y error is 1/128th of the frame height. */
	max_y_error = current_dims.y >> 7;

	/* Tentatively, max x error is 1/128 of frame width */
	max_x_error = current_dims.x >> 7;

/*
// Stretching text has the following steps:
//
// (1) Size all text to the original size and expansion (100%).
// (2) Compute the bounding extent of this text.
// (3) Calculate the adjusted size and expansion to make the text fit in
//     bound of the frame.
// (4) Resize the text to these new parameters.
//
//	Repeat these steps until the error is within acceptable limits to
// a maximum of five iterations.
*/

/*
// Step 1: Size all text to a reference size and expansion.
*/

	if ((paragraph = (ParagraphPtr)database->get_record(p_record, &error, RECORD_TYPE_Paragraph)) == NULL)
	{
		return error;
	}

	TextStyleRef style = paragraph->get_style();
	delta = style.get_left_margin() + style.get_right_margin();
	current_dims.x -= delta;
	original_dims.x -= delta;

	/* Shrink the target dimensions to make sure stretched
	// text can never clip.
	*/
#if 1
	current_dims.x -= max_x_error;
	current_dims.y -= max_y_error;
#endif

	if (current_dims.x <= 0)
	{
		current_dims.x = 1;
	}

	if (current_dims.y <= 0)
	{
		current_dims.y = 1;
	}

/* Set the reference size and expansion. */

	reference_size = style.get_base_size();
	reference_expansion = FONT_EXPANSION_UNIT;

	style.set_size((SHORT)reference_size);
	style.set_expansion(reference_expansion);

	style.set_size_fraction(style.get_base_size_fraction());
	style.update_font_metrics();

	shift = 0;
	
	stretch_size = reference_size;
	stretch_expansion = reference_expansion;

	/* Loop until error is acceptable */
	iteration = 0;
	for (;;)
	{
	/* Scale up the reference size so that we will have a reasonable amount
		of precision */
		if (reference_size == 0)
		{
		/* Sanity! */
			reference_size = 1;
		}
		while (reference_size < 1000)
		{
			reference_size <<= 1;
			++shift;
		}
		while (reference_size > 10000)
		{
			reference_size >>= 1;
			--shift;
		}
	/* We always need a full word range. */
		wrange.w_start =
			wrange.w_end = -1;

	/* Do the size. */

		if ((error = paragraph->size_words(wrange, TRUE)) != ERRORCODE_None)
		{
			paragraph->release(TRUE);
			return error;
		}

	/*
	// Step 2: Compute the bounding extent of this text.
	*/

		compute_flow_bounds(paragraph, original_dims, &text_extent, iteration == 0);

		if (++iteration > 5)
		{
			break;
		}

	/*
	// Step 3: Calculate the adjusted size and expansion to make the text fit in
	//         bound of the frame.
	*/

		if (text_extent.y <= 0)
		{
			text_extent.y = 1;
		}

#ifdef DEBUG_ST
	printf("frame (%ld, %ld) text (%ld, %ld)\r\n", current_dims, text_extent);
#endif

		/* See how well we did */

		xerr = labs(current_dims.x - text_extent.x);
		yerr = labs(current_dims.y - text_extent.y);

#ifdef DEBUG_ST
printf("yerr = %ld max = %ld xerr = %ld max = %ld\r\n",
 yerr,
 max_y_error,
 xerr,
 max_x_error);
#endif
		if (yerr <= max_y_error)
		{
			if (xerr <= max_x_error ||
				 (text_extent.x < current_dims.x && reference_expansion == MAX_EXPANSION) ||
				 (text_extent.x > current_dims.x && reference_expansion == MIN_EXPANSION))
			{
				break;
			}
		}

		/* NOTE: if Y value was acceptable we do NOT recompute it, so as to
		// allow X to converge more rapidly.
		*/

		/* Compute new size and new expansion as needed */
		if (yerr > max_y_error)
		{
			unsigned long max_size = ((unsigned long)MAX_STRETCH_SIZE) << shift;

			stretch_size =	(uint32)ext_SafeLongMulDiv(
										reference_size,
										current_dims.y,
										text_extent.y);

			if (stretch_size > max_size)
			{
				stretch_size = max_size;
			}
		}
		else
		{
			stretch_size = reference_size;
		}

		if (text_extent.x <= 0)
		{
		/* No text! */
			stretch_expansion = FONT_EXPANSION_UNIT;
		}
		else
		{
			PCOORD tmp;

			tmp = ext_SafeLongMulDiv(reference_expansion, current_dims.x, text_extent.x);
			tmp = ext_SafeLongMulDiv(tmp, reference_size, stretch_size);
#ifdef DEBUG_ST
printf("Stretch size %ld, stretch expansion %ld\r\n", stretch_size >> shift, tmp);
#endif

			if ((ULONG)tmp > MAX_EXPANSION)
			{
				stretch_expansion = MAX_EXPANSION;
			}
			else if (tmp < MIN_EXPANSION)
			{
				stretch_expansion = MIN_EXPANSION;
			}
			else
			{
				stretch_expansion = (SHORT)tmp;
			}
		}
#ifdef DEBUG_ST
	printf("%ld, %d to %ld, %d\r\n",
						reference_size >> shift, reference_expansion,
						stretch_size >> shift, stretch_expansion);
#endif

		/* Store the new expansion */
		style.set_expansion(stretch_expansion);

		/* Shift the stretched point size into the size fields of the
			style structure */

		USHORT size = (USHORT)style.get_size();
		USHORT size_fraction = (USHORT)style.get_size_fraction();
		SplitFlex(
			ShiftedLongToFlex(stretch_size, shift),
			&size,
			&size_fraction);
		if (size == 0)
		{
		/* Sanity! */
			size = 1;
			size_fraction = 0;
		}

		style.set_size((SHORT)size);
		style.set_size_fraction((SHORT)size_fraction);

		style.update_font_metrics();

		reference_size = stretch_size;
		reference_expansion = stretch_expansion;
	}
	paragraph->release(TRUE);

#ifdef DEBUG_ST
if (iteration > 2) printf("iterations = %d\r\n", iteration - 1);
#endif
	return ERRORCODE_None;
}

/////////////////////////////////////////////////////////////////////////////
// CFrameRecord

CFrameRecord::CFrameRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
				: DatabaseRecord(number, type, owner, where)
{
	memset(&m_Record, 0, sizeof(m_Record));

	m_Record.m_nColumns = 1;
	m_Record.m_lColumnSpacing = MakeFixed(0.1);
	m_Record.m_Margins.Left = 
		m_Record.m_Margins.Top = 
		m_Record.m_Margins.Right = 
		m_Record.m_Margins.Bottom = MakeFixed(0.05);

	m_pLineArray = NULL;
	m_pWordArray = NULL;

	m_pWarpField = NULL;

	m_Record.m_nEffectsVersion = 1;		// FillFormatV1
	m_Record.m_Fill.SetType(FillFormatV1::FillNone);
	m_Record.m_Outline.SetType(OutlineFormat::OutlineNone);
	m_Record.m_Shadow.SetType(ShadowFormat::ShadowNone);
}

/*
// The destructor for this record.
*/

CFrameRecord::~CFrameRecord(void)
{
	ReleaseSubrecords();

	FreeWarpField();
}

/*
// Free the warp field.
*/

void CFrameRecord::FreeWarpField(void)
{
	delete m_pWarpField;
	m_pWarpField = NULL;
}

/*
// The creator for a frame record.
*/

ERRORCODE CFrameRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
/* Create the new record. */

	*record = NULL;
	TRY
	{
		*record = new CFrameRecord(number, type, owner, where);
	}
	END_TRY

	return (*record == NULL) ? ERRORCODE_Memory : ERRORCODE_None;
}

/*
// Assign method.
*/

ERRORCODE CFrameRecord::assign(DatabaseRecordRef srecord)
{
	ERRORCODE error;

	// Make sure we are all clean.
	ReleaseSubrecords();
	DestroyRecords();

	// Assign the base record first.
	if ((error = DatabaseRecord::assign(srecord)) == ERRORCODE_None)
	{
		CFrameRecord& SourceRecord = (CFrameRecord&)srecord;

		PMGDatabase* pSourceDatabase = (PMGDatabase*)SourceRecord.database;
		PMGDatabase* pDestDatabase = (PMGDatabase*)database;

		// Now copy over the data.
		// Note that frames don't own their paragraphs. So we don't duplicate
		// the paragraph. We assume somebody else duplicated that.

		// Save the things we do not want changed.
		DB_RECORD_NUMBER lText = m_Record.m_lText;
		DB_RECORD_NUMBER lPreviousFrame = m_Record.m_lPreviousFrame;
		DB_RECORD_NUMBER lNextFrame = m_Record.m_lNextFrame;
		DB_RECORD_NUMBER lPage = m_Record.m_lPage;
		CTextState TextState = m_Record.m_TextState;

		// Copy the record over.
		m_Record = SourceRecord.m_Record;

		// Restore what we did not want changed.
		m_Record.m_lText = lText;
		m_Record.m_lPreviousFrame = lPreviousFrame;
		m_Record.m_lNextFrame = lNextFrame;
		m_Record.m_lPage = lPage;
		m_Record.m_TextState = TextState;

		// Copy the arrays.
		m_Record.m_lLineArray = pSourceDatabase->duplicate_record(SourceRecord.m_Record.m_lLineArray, pDestDatabase);
		m_Record.m_lWordArray = pSourceDatabase->duplicate_record(SourceRecord.m_Record.m_lWordArray, pDestDatabase);

		// Free any old warp field.
		set_new_warp_field(0);

		// Duplicate the warp field record.
		if (SourceRecord.m_Record.m_WarpField > 0)
		{
			WarpFieldRecord* pRecord;

			if ((pRecord = (WarpFieldRecord*)(pSourceDatabase->get_record(SourceRecord.m_Record.m_WarpField, &error, RECORD_TYPE_WarpField))) != NULL)
			{
				// Make a create structure for the new record.

				WarpField_CreateStruct CreateStruct;

				CreateStruct.m_pszName = pRecord->Name();
				CreateStruct.m_BoundsData = *(pRecord->BoundsData());

			/* Duplicate the layout into our database. */

				m_Record.m_WarpField = ((PMGDatabasePtr)database)->new_warp_field_record(&CreateStruct);

			/* Release the old one. */

				pRecord->release();
			}
		}
	}

	return error;
}

/*
// Destroy this record.
// The record is being removed from the database.
// Any and all sub-records must be removed from the database as well.
// This should not FREE anything (in the memory sense); that's the job of the
// destructor.
*/

void CFrameRecord::destroy(void)
{
	// Release any locked records before freeing.
	ReleaseSubrecords();
	DestroyRecords();
}

/////////////////////////////////////////////////////////////////////////////
// Helper functions.

/*
// Release any records we may have locked.
*/

void CFrameRecord::ReleaseSubrecords(void)
{
	// Release the line array.
	ReleaseSubrecord((DatabaseRecord*&)m_pLineArray);
	// Release the word array.
	ReleaseSubrecord((DatabaseRecord*&)m_pWordArray);
}

void CFrameRecord::DestroyRecords(void)
{
	// Free the line array.
	DeleteSubrecord(m_Record.m_lLineArray, RECORD_TYPE_LineArray);
	// Free the word array.
	DeleteSubrecord(m_Record.m_lWordArray, RECORD_TYPE_WordArray);

	if (m_Record.m_WarpField > 0)
	{
		/* Decrement the warp field reference. */
		((PMGDatabasePtr)database)->free_warp_field_record(m_Record.m_WarpField);
	}

	FreeWarpField();
}

//
// Lock the line array.
//

void CFrameRecord::LockLineArray(void)
{
	if (m_pLineArray == NULL)
	{
		LockSubrecord((DatabaseRecord*&)m_pLineArray, m_Record.m_lLineArray, RECORD_TYPE_LineArray, TRUE);
	}
}

//
// Lock the word array.
//

void CFrameRecord::LockWordArray(void)
{
	if (m_pWordArray == NULL)
	{
		LockSubrecord((DatabaseRecord*&)m_pWordArray, m_Record.m_lWordArray, RECORD_TYPE_WordArray, TRUE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// Line routines.

LINE_COUNT CFrameRecord::NumberOfLines(void)
{
	TRY
	{
		LockLineArray();
	}
	CATCH_ALL(e)
	{
		return 0;
	}
	END_CATCH_ALL

	ASSERT(m_pLineArray != NULL);
	return (m_pLineArray == NULL) ? 0 : m_pLineArray->ElementCount();
}

void CFrameRecord::InsertLine(LINE_INDEX Index, const CTextLine* pLine, LINE_COUNT Count /*=1*/)
{
	LockLineArray();
	ASSERT(m_pLineArray != NULL);
	if (m_pLineArray != NULL)
	{
		m_pLineArray->Insert(Index, (LPVOID)pLine, Count);
	}
}

void CFrameRecord::DeleteLine(LINE_INDEX Index, LINE_COUNT Count /*=1*/)
{
	LockLineArray();
	ASSERT(m_pLineArray != NULL);
	if (m_pLineArray != NULL)
	{
		m_pLineArray->Delete(Index, Count);
	}
}

CTextLine* CFrameRecord::GetLine(LINE_INDEX Index)
{
	LockLineArray();
	ASSERT(m_pLineArray != NULL);
	return (m_pLineArray == NULL) ? NULL : (CTextLine*)m_pLineArray->GetAt(Index);
}

CTextLine* CFrameRecord::GetLine(LINE_INDEX Index, LINE_COUNT* pElementsAvailable)
{
	*pElementsAvailable = 0;
	LockLineArray();
	ASSERT(m_pLineArray != NULL);
	return (m_pLineArray == NULL) ? NULL : (CTextLine*)m_pLineArray->GetAt(Index, (DWORD*)pElementsAvailable);
}

/////////////////////////////////////////////////////////////////////////////
// Word routines.

WORD_COUNT CFrameRecord::NumberOfWords(void)
{
	LockWordArray();
	ASSERT(m_pWordArray != NULL);
	return (m_pWordArray == NULL) ? 0 : m_pWordArray->ElementCount();
}

void CFrameRecord::InsertWord(WORD_INDEX Index, const CTextWord* pWord, WORD_COUNT Count /*=1*/)
{
	LockWordArray();
	ASSERT(m_pWordArray != NULL);
	if (m_pWordArray != NULL)
	{
		m_pWordArray->Insert(Index, (LPVOID)pWord, Count);
	}
}

void CFrameRecord::DeleteWord(WORD_INDEX Index, WORD_COUNT Count /*=1*/)
{
	LockWordArray();
	ASSERT(m_pWordArray != NULL);
	if (m_pWordArray != NULL)
	{
		m_pWordArray->Delete(Index, Count);
	}
}

CTextWord* CFrameRecord::GetWord(WORD_INDEX Index)
{
	LockWordArray();
	ASSERT(m_pWordArray != NULL);
	return (m_pWordArray == NULL) ? NULL : (CTextWord*)m_pWordArray->GetAt(Index);
}

CTextWord* CFrameRecord::GetWord(WORD_INDEX Index, WORD_COUNT* pWordsAvailable)
{
	LockWordArray();
	ASSERT(m_pWordArray != NULL);
	return (m_pWordArray == NULL) ? NULL : (CTextWord*)m_pWordArray->GetAt(Index, (DWORD*)pWordsAvailable);
}

/////////////////////////////////////////////////////////////////////////////
// Text routines.

ERRORCODE CFrameRecord::CreateTextRecord(void)
{
	ERRORCODE error = ERRORCODE_None;

	ASSERT(TextRecord() == 0);
	if (TextRecord() == 0)
	{
	/* We need to create the text record. */
	/* Create a text record. */

		CTextRecord* pTextRecord;

		if ((pTextRecord = ((PMGDatabase*)database)->NewStory()) != NULL)
		{
		/* Got the text record. */

			TextRecord(pTextRecord->Id());

			// We are the first frame for this guy.

			pTextRecord->FirstFrame(Id());

		/*
		// Reset the text state.
		*/

			TextState().Reset();

			// Create the empty text state.
			CTextStyle Style;
			FontServer* pFontServer = ((PMGDatabase*)database)->get_font_server();
			pFontServer->SetDefault(&Style);

			// Insert the single paragraph.
			CTextParagraph Paragraph;
			Paragraph.m_lBaseParagraphStyle = Style.ParagraphStyle();
			Paragraph.m_StyleChange.m_lValue = Paragraph.m_lBaseParagraphStyle;

			pTextRecord->InsertParagraph(0, &Paragraph);
		
			pTextRecord->release(TRUE);
			error = ERRORCODE_None;
		}
		else
		{
			error = database->last_creation_error();
		}
	}
	return error;
}

void CFrameRecord::DeleteTextRecord(void)
{
	if (TextRecord() != 0)
	{
		((PMGDatabase*)database)->DeleteStory(TextRecord());
		TextRecord(0);
	}
}

CTextRecord* CFrameRecord::LockTextRecord(ERRORCODE* pError /*=NULL*/)
{
	return (CTextRecord*)database->get_record(TextRecord(), pError, RECORD_TYPE_Text);
}

/*
// Lock the previous frame record.
*/

CFrameRecord* CFrameRecord::LockPreviousFrame(void)
{
	return (CFrameRecord*)database->get_record(PreviousFrame(), NULL, RECORD_TYPE_Frame);
}

/*
// Lock the next frame record.
*/

CFrameRecord* CFrameRecord::LockNextFrame(void)
{
	return (CFrameRecord*)database->get_record(NextFrame(), NULL, RECORD_TYPE_Frame);
}

/*
// Attach a text record.
// Passing 0 will detach from the current text record.
*/

void CFrameRecord::AttachTextRecord(DB_RECORD_NUMBER lText)
{
	if (TextRecord() != lText)
	{
		// Out with the old...
		if (TextRecord() != 0)
		{
			TextRecord(0);
		}

		// ...and in with the new.
		if (lText != 0)
		{
			TextRecord(lText);
		}

		TextState().Reset();
	}
}

/////////////////////////////////////////////////////////////////////////////
// Text State routines.

/////////////////////////////////////////////////////////////////////////////
// Navigation routines.

/*
// Get the location for a character (computes word and line).
//
// The transition point between two frames creates an ambiguity: is the
// point at the end of this frame or the start of the next? The reality is
// that it can be either depending on what you're doing.
// The rule we use is that it is the responsibility of the caller to determine
// which frame is actually desired. By the time it gets here, it is assumed
// that the user wants the point in THIS frame. There is no ambiguity at this
// level.
*/

void CFrameRecord::GetCharacterLocation(CFrameLocation* pLocation)
{
	// Get the text state.
	CHARACTER_INDEX lFirstCharacter = FirstCharacter();

	pLocation->m_lFrame = Id();

	// Get the character to look for.
	CHARACTER_INDEX lCharacter = pLocation->m_lCharacter;

	// Search for the character.
	if (NumberOfLines() > 0 && lFirstCharacter != -1 && lCharacter >= lFirstCharacter)
	{
		// Start at the top of the frame.
		LINE_INDEX lLine = 0;
		WORD_INDEX lFirstWordInLine = 0;
		CHARACTER_INDEX lFirstCharacterInLine = lFirstCharacter;

		// Loop on lines until the line is found.
		while (lLine < NumberOfLines()-1)
		{
			CTextLine* pLine = GetLine(lLine);
			ASSERT(pLine != NULL);

			if (lCharacter < lFirstCharacterInLine + pLine->m_lCharacters)
			{
				break;
			}
			lFirstCharacterInLine += pLine->m_lCharacters;
			lFirstWordInLine += pLine->m_lWords;
			lLine++;
		}

		// Found the line. Now find the word.

		CTextLine* pLine = GetLine(lLine);

//		ASSERT(lCharacter <= lFirstCharacterInLine + pLine->m_lCharacters);

		CHARACTER_INDEX lFirstCharacterInWord = lFirstCharacterInLine;
		WORD_INDEX lWord = lFirstWordInLine;
		for (WORD_INDEX lWordInLine = 0; lWordInLine < pLine->m_lWords-1; lWordInLine++)
		{
			CTextWord* pWord = GetWord(lWord);
			ASSERT(pWord != NULL);
			if (lCharacter < lFirstCharacterInWord + pWord->m_lCharacters)
			{
				break;
			}
			lFirstCharacterInWord += pWord->m_lCharacters;
			lWord++;
		}

		CTextWord* pWord = GetWord(lWord);

		// Pass back what we found.
		pLocation->m_lLine = lLine;
		pLocation->m_lFirstCharacterInLine = lFirstCharacterInLine;
		pLocation->m_lFirstWordInLine = lFirstWordInLine;
		pLocation->m_Line = *pLine;

		pLocation->m_lWord = lWord;
		pLocation->m_lFirstCharacterInWord = lFirstCharacterInWord;
		pLocation->m_Word = *pWord;

		// Compute the column info.
		
		int nColumns;
		PCOORD lColumnSpacing;
		PCOORD lColumnWidth;

		GetColumnData(&nColumns, &lColumnSpacing, &lColumnWidth);
		pLocation->m_ColumnLeft = lColumnWidth*pLine->m_nColumn;
		pLocation->m_ColumnRight = pLocation->m_ColumnLeft + lColumnWidth;

		return;
	}

	// When all else fails, fail.
	ThrowErrorcodeException(ERRORCODE_DoesNotExist);
}

/*
// Get the location in a frame given a point within the frame (computes
// character, word and line).
//
// The transition point between two frames creates an ambiguity: is the
// point at the end of this frame or the start of the next? The reality is
// that it can be either depending on what you're doing.
// The rule we use is that it is the responsibility of the caller to determine
// which frame is actually desired. By the time it gets here, it is assumed
// that the user wants the point in THIS frame. There is no ambiguity at this
// level.
*/

void CFrameRecord::GetPointLocation(PPNT p, CFrameLocation* pLocation, CFrameObject* pFrameObject, BOOL fNormalized /*=FALSE*/)
{
	CTextRecord* pText = NULL;
	CTextIterator Iterator;

	pLocation->m_lFrame = Id();

	if (FirstCharacter() == -1)
	{
		ThrowErrorcodeException(ERRORCODE_DoesNotExist);
	}
	
	TRY
	{
		PBOX FrameBound = ObjectBounds();
		FLAGS wObjectFlags = ObjectFlags();

		// Make point relative to frame origin.
		p.x -= FrameBound.x0;
		p.y -= FrameBound.y0;

		// If the point is not "normalized", then it's in the display coordinate system
		// where it has been warped, flipped, and/or rotated. This is the typical case
		// where the user clicks somewhere on the screen and we want to find out the
		// character position of the click. The user is clicking on something that has been
		// displayed on the screen and is thus in its warped, flipped, and/or rotated state.
		if (!fNormalized)
		{
			// Undo any rotation.
			ANGLE dRotation = pFrameObject->get_rotation();
			if (dRotation != 0.0)
			{
				PPNT Origin;
				Origin.x = (FrameBound.x1-FrameBound.x0+1)/2;
				Origin.y = (FrameBound.y1-FrameBound.y0+1)/2;
				rotate_points(&p, 1, Origin, -dRotation);
			}

			// Undo any flipping.
			if ((wObjectFlags & OBJECT_FLAG_xflipped) != 0)
			{
				p.x = (FrameBound.x1-FrameBound.x0) - p.x;
			}
			if ((wObjectFlags & OBJECT_FLAG_yflipped) != 0)
			{
				p.y = (FrameBound.y1-FrameBound.y0) - p.y;
			}

			// Undo any warping.
			CWarpField* pWarpField = pFrameObject->GetWarpField();
			if (pWarpField != NULL)
			{
				// Make sure the warp field is set up to warp.
				int nShift = 0;

				while (!SafeToConvertPboxToFixed(&FrameBound) || !SafeToConvertPpntToFixed(&p))
				{
					nShift++;
					FrameBound.x0 >>= 1;
					FrameBound.y0 >>= 1;
					FrameBound.x1 >>= 1;
					FrameBound.y1 >>= 1;
					p.x >>= 1;
					p.y >>= 1;
				}

				CRect crFrameObject;
				RedisplayContext::convert_pbox(&FrameBound, crFrameObject, NULL);
				crFrameObject.OffsetRect(-crFrameObject.left, -crFrameObject.top);
				pWarpField->Transform(crFrameObject);

				p.x = MakeFixed(p.x);
				p.y = MakeFixed(p.y);
				pWarpField->UnwarpPoint(&p.x, &p.y);
				p.x = FixedRound(p.x) << nShift;
				p.y = FixedRound(p.y) << nShift;
			}
		}

		// Get the column data.
		int nColumns;
		PCOORD lColumnSpacing;
		PCOORD lColumnWidth;
		GetColumnData(&nColumns, &lColumnSpacing, &lColumnWidth);

		// Compute the column this point lies in.
		int nColumn = lColumnWidth == 0 ? 0 : (int)(p.x/lColumnWidth);
		if (nColumn >= nColumns)
		{
			nColumn = nColumns-1;
		}

	/*
	// Find the line extent containing this point.
	// We assume only one text line per y extent for now.
	*/

		// Start at the top of the frame.
		LINE_INDEX lLine = 0;
		WORD_INDEX lFirstWordInLine = 0;
		CHARACTER_INDEX lFirstCharacterInLine = FirstCharacter();

		for (lLine = 0; lLine < NumberOfLines()-1; )
		{
			CTextLine* pLine = GetLine(lLine);

			if (pLine->m_nColumn == nColumn)
			{
				PCOORD Top = pLine->m_Baseline - pLine->m_Ascend;
				PCOORD Bottom = pLine->m_Baseline + pLine->m_Descend;
				if (p.y <= Bottom)
				{
					break;
				}
			}
			else if (pLine->m_nColumn > nColumn)
			{
				// Went too far. See if we can back up.
				if (lLine != 0)
				{
					pLine = GetLine(--lLine);
					lFirstCharacterInLine -= pLine->m_lCharacters;
					lFirstWordInLine -= pLine->m_lWords;
				}
				// Either way, we are done.
				break;
			}
			// Advance to next line.
			lFirstCharacterInLine += pLine->m_lCharacters;
			lFirstWordInLine += pLine->m_lWords;
			lLine++;
		}

		// In this line! Find the word.

		CTextLine* pLine = GetLine(lLine);
		CHARACTER_INDEX lFirstCharacterInWord = lFirstCharacterInLine;
		WORD_INDEX lWord = lFirstWordInLine;
		for (WORD_INDEX lWordInLine = 0; lWordInLine < pLine->m_lWords-1; lWordInLine++)
		{
			CTextWord* pWord = GetWord(lWord);
			ASSERT(pWord != NULL);
			if (p.x < pWord->m_XOffset + pWord->m_Width)
			{
				break;
			}
			lFirstCharacterInWord += pWord->m_lCharacters;
			lWord++;
		}
		ASSERT(lWordInLine < pLine->m_lWords);

		// We found the word. Find the character.
		CTextWord* pWord = GetWord(lWord);

		ERRORCODE error;
		if ((pText = (CTextRecord*)database->get_record(TextRecord(), &error, RECORD_TYPE_Text)) == NULL)
		{
			ThrowErrorcodeException(error);
		}

		// Setup our iterator.
		Iterator.SetRecord(pText);

		FontServerPtr pFontServer = ((PMGDatabase*)database)->get_font_server();

		CHARACTER_COUNT lCharacters = pWord->m_lCharacters;
		// If this is the final word in the line (but not in the flowed text),
		// back off a character.
		if (lWordInLine == pLine->m_lWords-1)
		{
			BOOL fAtEnd = FALSE;
			if (pWord->m_wType == WORD_TYPE_break)
			{
				// Always back up. Otherwise, we end up on the next line.
				fAtEnd = FALSE;
			}
			else if (pWord->m_wType != WORD_TYPE_solid)
			{
				// Last word in line. Is it the last flowed character?
				fAtEnd = (NextFrame() != 0
									&& lFirstCharacterInWord + lCharacters == pText->NumberOfCharacters())
								|| (NextFrame() == 0
									 && lLine == NumberOfLines()-1);
			}
			if (!fAtEnd)
			{
				lCharacters--;
			}
		}

		// Find the character.

		CHARACTER_INDEX lIndex = lFirstCharacterInWord;
		PCOORD x_offset = pWord->m_XOffset;
		BOOL fTabWord = (pWord->m_wType == WORD_TYPE_tab);

		while (lCharacters-- > 0)
		{
			Iterator.SetPosition(lIndex);
			CHARACTER* cp = pText->GetCharacter(lIndex);
			CHARACTER c = *cp;
			if (c == '\t' && !fTabWord)
			{
				// Some tabs are not tabs...
				c = ' ';
			}

			PCOORD Width = pFontServer->GetAdvanceWidth(&Iterator.Style(), *cp);

			if (p.x < x_offset + Width/2)
			{
				break;
			}
			x_offset += Width;
			lIndex++;
		}

		pLocation->m_lCharacter = lIndex;

		// Pass back what we found.

		pLocation->m_lLine = lLine;
		pLocation->m_lFirstCharacterInLine = lFirstCharacterInLine;
		pLocation->m_lFirstWordInLine = lFirstWordInLine;
		pLocation->m_Line = *pLine;

		pLocation->m_lWord = lWord;
		pLocation->m_lFirstCharacterInWord = lFirstCharacterInWord;
		pLocation->m_Word = *pWord;

		pLocation->m_ColumnLeft = lColumnWidth*pLine->m_nColumn;
		pLocation->m_ColumnRight = pLocation->m_ColumnLeft + lColumnWidth;

		pText->release();
		pText = NULL;
	}
	CATCH_ALL(e)
	{
		Iterator.Reset();

		if (pText != NULL)
		{
			pText->release();
		}
		THROW_LAST();
	}
	END_CATCH_ALL
}

/*
// Return the number of characters in the frame.
*/

CHARACTER_COUNT CFrameRecord::NumberOfCharacters(void)
{
	// Count up all the characters in the lines.
	CHARACTER_COUNT lCount = 0;
	for (LINE_INDEX lLine = 0; lLine < NumberOfLines(); lLine++)
	{
		CTextLine* pLine = GetLine(lLine);
		lCount += pLine->m_lCharacters;
	}

	return lCount;
}

/*
// Update the refresh offsets in the frame.
*/

void CFrameRecord::UpdateRefreshOffsets(void)
{
	return; //j
	TRY
	{
		// Compute the extra refresh offsets for the frame. These are stored
		// as a PBOX which is the union of the bounding extents of all the
		// lines in the frame (set by TextFlow.) Note: line extents are always
		// computed by TextFlow relative to the normalized (unflipped, unrotated)
		// frame bounds. The refresh extents are also relative to the normalized
		// frame bounds and the bottom refresh extent (y1) is clipped to the
		// bottom of the frame.

		PBOX Offsets;
		Offsets.x0 = 0x7fffffff;
		Offsets.x1 = -Offsets.x0;
		Offsets.y0 = 0x7fffffff;
		Offsets.y1 = -Offsets.y0;

		LINE_COUNT lTotalLines = NumberOfLines();
		LINE_INDEX lLine = 0;
		for (LINE_COUNT lLines = lTotalLines; lLines > 0; )
		{
			// See how many lines we can grab at once.
			LINE_COUNT lThisLines;
			CTextLine* pLine = GetLine(lLine, &lThisLines);
			ASSERT(pLine != NULL && lThisLines > 0);
			ASSERT(lThisLines <= lLines);
			if (lThisLines > lLines)
			{
				lThisLines = lLines;
			}
			lLines -= lThisLines;

			// Process this many lines.
			while (lThisLines-- > 0)
			{
				if (pLine->m_ExtentLeft < Offsets.x0)
				{
					Offsets.x0 = pLine->m_ExtentLeft;
				}
				if (pLine->m_ExtentRight > Offsets.x1)
				{
					Offsets.x1 = pLine->m_ExtentRight;
				}
				PCOORD ThisY0 = pLine->m_Baseline - pLine->m_Ascend;
				if (ThisY0 < Offsets.y0)
				{
					Offsets.y0 = ThisY0;
				}
				PCOORD ThisY1 = pLine->m_Baseline + pLine->m_Descend;
				if (ThisY1 > Offsets.y1)
				{
					Offsets.y1 = ThisY1;
				}
				lLine++;
				pLine++;
			}
		}

		// ObjectBounds() returns the unrotated bounds with any margins.
		PCOORD FrameHeight = ObjectBounds().y1 - ObjectBounds().y0;
		if (Offsets.y1 > FrameHeight)
		{
			Offsets.y1 = FrameHeight;
		}

		m_Record.m_RefreshOffsets = Offsets;
	}
	CATCH_ALL(e)
	{
		memset(&m_Record.m_RefreshOffsets, 0, sizeof(PBOX));
	}
	END_CATCH_ALL
}

/////////////////////////////////////////////////////////////////////////////
// Column support.

// Get the column data for this frame.
// ColumnWidth is the width of a column excluding any spacing (since the
// spacing varies depending on the column!)

void CFrameRecord::GetColumnData(int* pColumns, PCOORD* pColumnSpacing, PCOORD* pColumnWidth, PBOX* pBound /*=NULL*/)
{
	if (pBound == NULL)
	{
		pBound = &m_Record.m_ObjectBounds;
	}
	PCOORD lFrameWidth = pBound->x1 - pBound->x0;

	// Set some defaults.
	int nColumns = Columns();
	PCOORD lColumnWidth;
	PCOORD lColumnSpacing;

	if (FrameFlags() & (FRAME_FLAG_stretch_text|FRAME_FLAG_stretch_frame))
	{
		nColumns = 1;
	}
	if (nColumns < 1)
	{
		// This should not happen, but...
		ASSERT(FALSE);
		// Fix it!
		nColumns = 0;
	}

	if (nColumns > 1)
	{
		// Compute the multi-column case. If the columns are too small,
		// revert to single-column.
		// "Too small" is defined as (an arbitrary) one-half inch.

		lColumnWidth = lFrameWidth/nColumns;

		// We need to check all three column cases (left, middle, right).
		
		lColumnSpacing = InchesToPage(ColumnSpacing());

		PCOORD LeftColumnSpacing = InchesToPage(Margins().Left + ColumnSpacing()/2);
		PCOORD RightColumnSpacing = InchesToPage(ColumnSpacing()/2 + Margins().Right);
		if (lColumnWidth - LeftColumnSpacing < PAGE_RESOLUTION/2
				|| lColumnWidth - lColumnSpacing < PAGE_RESOLUTION/2
				|| lColumnWidth - RightColumnSpacing < PAGE_RESOLUTION/2)
		{
			nColumns = 1;
		}
	}

	if (nColumns == 1)
	{
		// Compute the single-column case.
		lColumnWidth = lFrameWidth;
		lColumnSpacing = 0;
	}

	*pColumns = nColumns;
	*pColumnSpacing = lColumnSpacing;
	*pColumnWidth = lColumnWidth;
}

/////////////////////////////////////////////////////////////////////////////
// Frame attributes routines.

void CFrameRecord::GetAttributes(CFrameAttributes* pAttributes)
{
	if (pAttributes->m_fSet)
	{
		// Merge our attributes in. This mostly involves turning things off.
		if (pAttributes->m_nVerticalAlignment != m_Record.m_nVerticalAlignment)
		{
			pAttributes->m_nVerticalAlignment = -1;
		}
		if (pAttributes->m_Margins.Left != m_Record.m_Margins.Left)
		{
			pAttributes->m_Margins.Left = UNDEFINED_CFIXED;
		}
		if (pAttributes->m_Margins.Top != m_Record.m_Margins.Top)
		{
			pAttributes->m_Margins.Top = UNDEFINED_CFIXED;
		}
		if (pAttributes->m_Margins.Right != m_Record.m_Margins.Right)
		{
			pAttributes->m_Margins.Right = UNDEFINED_CFIXED;
		}
		if (pAttributes->m_Margins.Bottom != m_Record.m_Margins.Bottom)
		{
			pAttributes->m_Margins.Bottom = UNDEFINED_CFIXED;
		}
		if (pAttributes->m_nColumns != m_Record.m_nColumns)
		{
			pAttributes->m_nColumns = -1;
		}
		if (pAttributes->m_lColumnSpacing != m_Record.m_lColumnSpacing)
		{
			pAttributes->m_lColumnSpacing = -1;
		}

		if (pAttributes->m_Fill.m_nPattern != m_Record.m_Fill.m_nPattern)
		{
			pAttributes->m_Fill.m_nPattern = FillFormatV1::Undefined;
		}
		if (pAttributes->m_Fill.m_ForegroundColor != m_Record.m_Fill.m_ForegroundColor)
		{
			pAttributes->m_Fill.m_ForegroundColor = UNDEFINED_COLOR;
		}

		if (pAttributes->m_Outline.m_nStyle != m_Record.m_Outline.m_nStyle)
		{
			pAttributes->m_Outline.m_nStyle = OutlineFormat::Undefined;
		}
		if (pAttributes->m_Outline.m_nWidthType != m_Record.m_Outline.m_nWidthType)
		{
			pAttributes->m_Outline.m_nWidthType = -1;
		}
		if (pAttributes->m_Outline.m_lWidth != m_Record.m_Outline.m_lWidth)
		{
			pAttributes->m_Outline.m_lWidth = UNDEFINED_CFIXED;
		}
		if (pAttributes->m_Outline.m_nPattern != m_Record.m_Outline.m_nPattern)
		{
			pAttributes->m_Outline.m_nPattern = FillFormatV1::Undefined;
		}
		if (pAttributes->m_Outline.m_ForegroundColor != m_Record.m_Outline.m_ForegroundColor)
		{
			pAttributes->m_Outline.m_ForegroundColor = UNDEFINED_COLOR;
		}

		if (pAttributes->m_Shadow.m_nStyle != m_Record.m_Shadow.m_nStyle)
		{
			pAttributes->m_Shadow.m_nStyle = ShadowFormat::Undefined;
		}
		if (pAttributes->m_Shadow.m_nOffsetType != m_Record.m_Shadow.m_nOffsetType)
		{
			pAttributes->m_Shadow.m_nOffsetType = -1;
		}
		if (pAttributes->m_Shadow.m_lXOffset != m_Record.m_Shadow.m_lXOffset)
		{
			pAttributes->m_Shadow.m_lXOffset = UNDEFINED_CFIXED;
		}
		if (pAttributes->m_Shadow.m_lYOffset != m_Record.m_Shadow.m_lYOffset)
		{
			pAttributes->m_Shadow.m_lYOffset = UNDEFINED_CFIXED;
		}
		if (pAttributes->m_Shadow.m_nPattern != m_Record.m_Shadow.m_nPattern)
		{
			pAttributes->m_Shadow.m_nPattern = FillFormatV1::Undefined;
		}
		if (pAttributes->m_Shadow.m_ForegroundColor != m_Record.m_Shadow.m_ForegroundColor)
		{
			pAttributes->m_Shadow.m_ForegroundColor = UNDEFINED_COLOR;
		}
	}
	else
	{
		// Set the attributes to ours.
		pAttributes->m_fSet = TRUE;
		pAttributes->m_nVerticalAlignment = m_Record.m_nVerticalAlignment;
		pAttributes->m_Margins = m_Record.m_Margins;
		pAttributes->m_nColumns = m_Record.m_nColumns;
		pAttributes->m_lColumnSpacing = m_Record.m_lColumnSpacing;
		pAttributes->m_pWarpField = GetWarpField();

		pAttributes->m_Fill = m_Record.m_Fill;
		pAttributes->m_Outline = m_Record.m_Outline;
		pAttributes->m_Shadow = m_Record.m_Shadow;
	}
}

void CFrameRecord::SetAttributes(const CFrameAttributes& Attributes)
{
	ASSERT(Attributes.m_fSet);
	// Alignment
	if (Attributes.m_nVerticalAlignment != -1)
		m_Record.m_nVerticalAlignment = Attributes.m_nVerticalAlignment;
	// Margins
	if (Attributes.m_Margins.Left != UNDEFINED_CFIXED)
		m_Record.m_Margins.Left = Attributes.m_Margins.Left;
	if (Attributes.m_Margins.Top != UNDEFINED_CFIXED)
		m_Record.m_Margins.Top = Attributes.m_Margins.Top;
	if (Attributes.m_Margins.Right != UNDEFINED_CFIXED)
		m_Record.m_Margins.Right = Attributes.m_Margins.Right;
	if (Attributes.m_Margins.Bottom != UNDEFINED_CFIXED)
		m_Record.m_Margins.Bottom = Attributes.m_Margins.Bottom;
	// Columns
	if (Attributes.m_nColumns != -1)
		m_Record.m_nColumns = Attributes.m_nColumns;
	// Column spacing
	if (Attributes.m_lColumnSpacing != -1)
		m_Record.m_lColumnSpacing = Attributes.m_lColumnSpacing;

	DB_RECORD_NUMBER lRecord = 0;

	PMGDatabase* pDatabase = (PMGDatabase*)database;

	if (Attributes.m_pWarpField != NULL)
	{
		WarpField_CreateStruct CreateStruct;

		CreateStruct.m_pszName = Attributes.m_pWarpField->Name();
		Attributes.m_pWarpField->OriginalBounds()->ToData(CreateStruct.m_BoundsData);

		lRecord = pDatabase->new_warp_field_record(&CreateStruct);
	}

	// Set the new warp field. This will bump the reference count as needed.
	set_new_warp_field(lRecord);

	if (lRecord > 0)
	{
		// Get rid of our creation reference.
		pDatabase->free_warp_field_record(lRecord);
	}

	// Fill
	if (Attributes.m_Fill.m_nPattern != FillFormatV1::Undefined)
		m_Record.m_Fill.m_nPattern = Attributes.m_Fill.m_nPattern;
	if (Attributes.m_Fill.m_ForegroundColor != UNDEFINED_COLOR)
		m_Record.m_Fill.m_ForegroundColor = Attributes.m_Fill.m_ForegroundColor;

	// Outline
	if (Attributes.m_Outline.m_nStyle != OutlineFormat::Undefined)
		m_Record.m_Outline.m_nStyle = Attributes.m_Outline.m_nStyle;
	if (Attributes.m_Outline.m_nWidthType != -1)
		m_Record.m_Outline.m_nWidthType = Attributes.m_Outline.m_nWidthType;
	if (Attributes.m_Outline.m_lWidth != UNDEFINED_CFIXED)
		m_Record.m_Outline.m_lWidth = Attributes.m_Outline.m_lWidth;
	if (Attributes.m_Outline.m_nPattern != FillFormatV1::Undefined)
		m_Record.m_Outline.m_nPattern = Attributes.m_Outline.m_nPattern;
	if (Attributes.m_Outline.m_ForegroundColor != UNDEFINED_COLOR)
		m_Record.m_Outline.m_ForegroundColor = Attributes.m_Outline.m_ForegroundColor;

	// Shadow
	if (Attributes.m_Shadow.m_nStyle != ShadowFormat::Undefined)
		m_Record.m_Shadow.m_nStyle = Attributes.m_Shadow.m_nStyle;
	if (Attributes.m_Shadow.m_nOffsetType != -1)
		m_Record.m_Shadow.m_nOffsetType = Attributes.m_Shadow.m_nOffsetType;
	if (Attributes.m_Shadow.m_lXOffset != UNDEFINED_CFIXED)
		m_Record.m_Shadow.m_lXOffset = Attributes.m_Shadow.m_lXOffset;
	if (Attributes.m_Shadow.m_lYOffset != UNDEFINED_CFIXED)
		m_Record.m_Shadow.m_lYOffset = Attributes.m_Shadow.m_lYOffset;
	if (Attributes.m_Shadow.m_nPattern != FillFormatV1::Undefined)
		m_Record.m_Shadow.m_nPattern = Attributes.m_Shadow.m_nPattern;
	if (Attributes.m_Shadow.m_ForegroundColor != UNDEFINED_COLOR)
		m_Record.m_Shadow.m_ForegroundColor = Attributes.m_Shadow.m_ForegroundColor;

	modified();
}

/*
// Set our data given an old frame (like a conversion).
*/

ERRORCODE CFrameRecord::FromOldFrame(FrameRecord* pOldFrame)
{
	ERRORCODE error;

	// Copy over all the pertinent attributes from the frame itself.
	FrameFlags(pOldFrame->get_flags());
	StretchDims(pOldFrame->get_stretch_dims());
	ObjectBounds(pOldFrame->get_bound());

	// Lock the old paragraph.
	ParagraphRecord* pParagraph = (ParagraphRecord*)database->get_record(pOldFrame->get_paragraph(), &error, RECORD_TYPE_Paragraph);
	if (pParagraph != NULL)
	{
		// Set the frame attributes that used to be in the paragraph.
		VerticalAlignment(pParagraph->get_style().get_vertical_alignment());

		// Lock our text record.
		CTextRecord* pText = (CTextRecord*)database->get_record(TextRecord(), &error, RECORD_TYPE_Text);
		if (pText != NULL)
		{
			// Copy the paragraph text to the text record.
			pText->FromOldParagraph(pParagraph);
			pText->release(TRUE);
		}
		pParagraph->release();
	}

	return error;
}

void CFrameRecord::GetShadowOffsets(PCOORD* pX, PCOORD* pY)
{
	*pX = 0;
	*pY = 0;
	
	if (m_Record.m_Shadow.IsVisible())
	{
		if (m_Record.m_Shadow.m_nOffsetType == ShadowFormat::Absolute)
		{
			*pX = FixedInteger(MulDivFixed(m_Record.m_Shadow.m_lXOffset, PAGE_RESOLUTION, 72));
			*pY = FixedInteger(MulDivFixed(m_Record.m_Shadow.m_lYOffset, PAGE_RESOLUTION, 72));
		}
		else
		{
			// Compute extents to use for scaling.
			PBOX Bounds = ObjectBounds();
			PCOORD lX = Bounds.x1-Bounds.x0;
			PCOORD lY = Bounds.y1-Bounds.y0;
			if (lX < lY)
			{
				lY = lX;
			}
			if (lY < lX)
			{
				lX = lY;
			}

			*pX = FixedInteger(m_Record.m_Shadow.m_lXOffset*lX);
			*pY = FixedInteger(m_Record.m_Shadow.m_lYOffset*lY);

			// Enforce minimum size.
			PCOORD lMin = PAGE_RESOLUTION/12;

			lX = *pX;
			if (lX < 0)
			{
				lX = -lX;
			}

			lY = *pY;
			if (lY < 0)
			{
				lY = -lY;
			}

			PCOORD lMax = lX;
			if (lMax < lY)
			{
				lMax = lY;
			}

			if (lMax < lMin)
			{
				if (lMax == 0)
				{
					lX = lY = lMin;
				}
				else
				{
					lX = scale_pcoord(lX, lMin, lMax);
					lY = scale_pcoord(lY, lMin, lMax);
				}

				if (*pX >= 0)
				{
					*pX = lX;
				}
				else
				{
					*pX = -lX;
				}

				if (*pY >= 0)
				{
					*pY = lY;
				}
				else
				{
					*pY = -lY;
				}
			}
		}

		*pY = -*pY;
	}
}

void CFrameRecord::GetOutlineWidth(PCOORD* pX, PCOORD* pY)
{
	*pX = 0;
	*pY = 0;
	
	if (m_Record.m_Outline.IsVisible())
	{
		if (m_Record.m_Outline.m_nWidthType == OutlineFormat::Absolute)
		{
			*pX = *pY = FixedInteger(MulDivFixed(m_Record.m_Outline.m_lWidth, PAGE_RESOLUTION, 72));
		}
		else
		{
			// Compute extents to use for scaling.
			PBOX Bounds = ObjectBounds();
			PCOORD lX = Bounds.x1-Bounds.x0;
			PCOORD lY = Bounds.y1-Bounds.y0;
			if (lX < lY)
			{
				lY = lX;
			}
			if (lY < lX)
			{
				lX = lY;
			}

			*pX = FixedInteger(m_Record.m_Outline.m_lWidth*lX);
			*pY = FixedInteger(m_Record.m_Outline.m_lWidth*lY);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// Warp Field

/*
// Set a new warp field, freeing an old one.
*/

VOID CFrameRecord::set_new_warp_field(DB_RECORD_NUMBER l)
{
	if (m_Record.m_WarpField != l)
	{
		// Get rid of the old warp field.
		FreeWarpField();

		// Decrement the warp field record reference count.
		if (m_Record.m_WarpField > 0)
		{
			((PMGDatabasePtr)database)->free_warp_field_record(m_Record.m_WarpField);
		}

		// Set the new warp field.
		m_Record.m_WarpField = l;

		// Add a reference for the new one.
		if (m_Record.m_WarpField > 0)
		{
			((PMGDatabasePtr)database)->inc_warp_field_record(m_Record.m_WarpField);
		}
	}
}

/*
// Return a pointer to the warp field currently referenced by
// this object.
*/

CWarpField* CFrameRecord::GetWarpField(void)
{
	if (m_pWarpField == NULL)
	{
		if (m_Record.m_WarpField > 0)
		{
			WarpFieldRecord* pRecord;
			ERRORCODE error;

			if ((pRecord = (WarpFieldRecord*)database->get_record(m_Record.m_WarpField, &error, RECORD_TYPE_WarpField)) != NULL)
			{
				m_pWarpField = new CWarpField;
				m_pWarpField->Name(pRecord->Name());
				m_pWarpField->OriginalBounds()->FromData(*(pRecord->BoundsData()));
				pRecord->release();
			}
		}
	}
	return m_pWarpField;
}

/////////////////////////////////////////////////////////////////////////////
// Storage routines.

/*
// ReadData()
//
// Read the record.
*/

ERRORCODE CFrameRecord::ReadData(StorageDevice* pDevice)
{
	ERRORCODE error;
	if ((error = pDevice->read_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
		error = m_Intersections.ReadData(pDevice);
	}

	if (m_Record.m_nEffectsVersion == 0)
	{
		m_Record.m_nEffectsVersion = 1;
		Fill().SetColor(FillFormatV1::Black, TRANSPARENT_COLOR);
		Outline().SetType(OutlineFormat::OutlineNone);
		Outline().SetColor(FillFormatV1::Black, TRANSPARENT_COLOR);
		Shadow().SetType(ShadowFormat::ShadowNone);
		Shadow().SetColor(FillFormatV1::Black, TRANSPARENT_COLOR);
	}

	return error;
}

/*
// WriteData()
//
// Write the record.
*/

ERRORCODE CFrameRecord::WriteData(StorageDevice* pDevice)
{
	ERRORCODE error;
	if ((error = pDevice->write_record(&m_Record, sizeof(m_Record))) == ERRORCODE_None)
	{
		error = m_Intersections.WriteData(pDevice);
	}
	return error;
}

/*
// SizeofData()
//
// Return the size of the record.
*/

ST_MAN_SIZE CFrameRecord::SizeofData(StorageDevice* pDevice)
{
	return pDevice->size_record(sizeof(m_Record))
				+ m_Intersections.SizeofData(pDevice);
}

/////////////////////////////////////////////////////////////////////////////
// CLineArrayRecord

CLineArrayRecord::CLineArrayRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
	: CArrayRecord(number, type, owner, where)
{
	ElementSize(sizeof(CTextLine));			// ~28 bytes
	MaxBlockElements(100);						// ~28*100 = 2800 bytes
	ElementIncrement(8);							// ~28*8 = 224
}

/*
// The creator for a line array record.
*/

ERRORCODE CLineArrayRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
/* Create the new array record. */

	*record = NULL;
	TRY
	{
		*record = new CLineArrayRecord(number, type, owner, where);
	}
	END_TRY

	return (*record == NULL) ? ERRORCODE_Memory : ERRORCODE_None;
}

/////////////////////////////////////////////////////////////////////////////
// CWordArrayRecord

CWordArrayRecord::CWordArrayRecord(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, ST_DEV_POSITION far *where)
	: CArrayRecord(number, type, owner, where)
{
	ElementSize(sizeof(CTextWord));			// ~32 bytes
	MaxBlockElements(128);						// ~32*128 = 4096 bytes
	ElementIncrement(8);							// ~32*8 = 256
}

/*
// The creator for a word array record.
*/

ERRORCODE CWordArrayRecord::create(DB_RECORD_NUMBER number, DB_RECORD_TYPE type, DatabasePtr owner, VOIDPTR creation_data, ST_DEV_POSITION far *where, DatabaseRecordPtr far *record)
{
/* Create the new array record. */

	*record = NULL;
	TRY
	{
		*record = new CWordArrayRecord(number, type, owner, where);
	}
	END_TRY

	return (*record == NULL) ? ERRORCODE_Memory : ERRORCODE_None;
}

