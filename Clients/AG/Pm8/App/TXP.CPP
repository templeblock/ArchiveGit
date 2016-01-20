/*
// Text insertion point "object" routines.
//
// $Header: /PM8/App/TXP.CPP 1     3/03/99 6:12p Gbeddow $
//
// $Log: /PM8/App/TXP.CPP $
// 
// 1     3/03/99 6:12p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:26:40   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:40:52   Fred
// Initial revision.
// 
//    Rev 1.1   26 Jun 1996 14:22:24   Jay
//  
// 
//    Rev 1.0   14 Mar 1996 13:44:10   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:48   FRED
// Initial revision.
// 
//    Rev 2.6   07 Jul 1995 16:10:52   JAY
// Misc changes.
// 
//    Rev 2.5   03 May 1995 18:08:40   FRED
// 
//    Rev 2.4   19 Apr 1995 12:31:38   JAY
//  
// 
//    Rev 2.3   29 Mar 1995 08:58:54   JAY
// Lots of changes
// 
//    Rev 2.2   15 Mar 1995 10:40:32   JAY
// Support for new text editing
// 
//    Rev 2.1   17 Feb 1995 16:44:54   JAY
// Changes necessary for text undo.
// 
//    Rev 2.0   07 Feb 1995 15:48:14   JAY
// Initial revision.
// 
//    Rev 1.8   30 Nov 1994 16:37:52   JAY
// stdafx.h; merged database code from album
// 
//    Rev 1.7   08 Apr 1994 11:05:02   JAY
// Returns the appropriate ERRORCODE_Full if the paragraph reports one.
// 
//    Rev 1.6   18 Mar 1994 11:07:06   FRED
// right_word() moves to the *start* of the next
// word to the right.
// 
// 
//    Rev 1.5   28 Jan 1994 17:10:58   JAY
//  
// 
//    Rev 1.4   19 Jan 1994 17:06:42   JAY
//  
// 
//    Rev 1.3   13 Jan 1994 17:00:20   JAY
//  
// 
//    Rev 1.2   05 Jan 1994 17:14:00   JAY
// Frames are released as modified during insert_text().
// 
//    Rev 1.1   05 Jan 1994 08:06:58   JAY
// Restructuring, etc.
//
//   Rev 1.0   16 Dec 1993 16:25:18   JAY
// 
*/

#include "stdafx.h"

#if 0
#include "pmgdb.h"
#include "txp.h"
#include "pararec.h"
#include "framerec.h"
#include "frameobj.h"
#include "styleit.h"
#include "utils.h"
#include "textflow.h"
#include "fontsrv.h"

/*
// Compatability mode.
*/

TXP_COMPATABILITY Txp::compatability = TXP_COMPATABILITY_Safe;

/*
// The constructor for a txp.
*/

Txp::Txp(PMGDatabasePtr ref) : database(ref), style(ref), select_style(ref)
{
	reset();
}

/*
// Reset the txp to not reference anything.
// This will get rid of all face references as well.
*/

VOID Txp::reset(BOOL fSaveFace /*=FALSE*/)
{
	object = NULL;
	c_index = -1;
	if (fSaveFace)
	{
	/* Just reset in case of stretching fudges. */
		style.set_size(style.get_base_size());
		style.set_size_fraction(style.get_base_size_fraction());
		style.set_expansion(FONT_EXPANSION_UNIT);
		style.set_expansion_fraction(0);
	}
	else
	{
		style.set_face(0);
	}
	select_style.set_face(0);
}

/*
// Say whether a txp is valid or not.
*/

BOOL Txp::valid(VOID)
{
	return object != NULL && c_index >= 0;
}

/*
// Initialize an insertion point.
*/

VOID Txp::init(FrameObjectPtr frame_object, C_INDEX offset, TXP_TOGGLE_FUNCTION tfunc, VOIDPTR tdata)
{
	reset();

	database = frame_object->get_database();
	object = frame_object;
	c_index = select_start = select_end = offset;
	f_record = frame_object->get_frame_record();
	p_record = database->paragraph_of_frame(f_record);
	visible = 0;
	toggle_function = tfunc;
	toggle_data = tdata;
	transition = TRUE;
}

/*
// Toggle all or part of a txp.
*/

VOID Txp::toggle(TXP_TOGGLE which)
{
	if (select_start == select_end)
	{
		which &= ~TXP_TOGGLE_SELECT;
	}

	if (which != 0 && toggle_function != NULL)
	{
		toggle_function(this, which, toggle_data);
	}
}

/*
// Turn the txp cursor(s) on.
*/

VOID Txp::on(TXP_TOGGLE which)
{
	if (valid())
	{
		if ((which &= (visible ^ which)) != 0)
		{
			visible ^= which;

			calc_all();

			toggle(which);
		}
	}
}

/*
// Turn the txp cursor(s) off.
*/

VOID Txp::off(TXP_TOGGLE which)
{
	if (valid())
	{
		if ((which &= visible) != 0)
		{
			visible ^= which;

			toggle(which);
		}
	}
}

/*
// Compute the style of the txp.
*/

ERRORCODE Txp::calc_style(VOID)
{
	ERRORCODE error;
	ParagraphPtr paragraph;

	if (!valid())
	{
		return ERRORCODE_NotInitialized;
	}

/* Get the paragraph where the txp currently is. */

	if ((paragraph = (ParagraphPtr)database->get_record(p_record, &error, RECORD_TYPE_Paragraph)) == NULL)
	{
		return error;
	}

/* Initialize our style. */

	StyleIterator si(paragraph);

/* Now get the style where we are. */

	if (c_index > 0)
	{
		si.compute(c_index-1);
	}

/* Set the style we found. */

	style = si.style;

	/* Get the composite style of the selected range */

	if (select_start != select_end)
	{
		si.compute_style_range(select_start, select_end-1, &select_style);
	}
	else
	{
		select_style = style;
	}

/* Release the paragraph. */

	paragraph->release();

/* No longer in a transition state. */

	transition = FALSE;

	return ERRORCODE_None;
}

/*
// Compute the x offset of ptxp->c_index.
*/

PCOORD Txp::x_offset(TEXT_WORD_PTR wp)
{
	ParagraphPtr paragraph;
	TEXT_WORD word;
	C_INDEX c_start;
	CHARACTER_PTR cp;
	PCOORD x;

/* Get the paragraph to do our computation. */

	if ((paragraph = (ParagraphPtr)database->get_record(p_record, NULL, RECORD_TYPE_Paragraph)) == NULL)
	{
		return -1;
	}

/* Get the word for us to use. */

	if (wp == NULL)
	{
		paragraph->word_of_character(c_index, wp = &word);
	}

/* Initialize the style. */

	StyleIterator si(paragraph);

/*
// Now we have the word, so we know the start of the word (x-wise).
// Find the character x_offset within the word.
*/

	FontServerPtr font_server = database->get_font_server();

	for (x = wp->x_offset,
							c_start = wp->c_start,
							cp = paragraph->get_character(c_start);
				c_start != c_index;
				c_start++, cp++)
	{
		si.compute(c_start);
		x += font_server->ch_delta_x(*cp, &si.style);
	}

	paragraph->release();

	return x;
}

/*
// Calculate a cursor box for a given c_index.
*/

ERRORCODE Txp::calc_cursor(VOID)
{
	ParagraphPtr paragraph;
	ERRORCODE error;
	W_INDEX w_index;
	TEXT_WORD word;
	SHORT last_offset;
	FramePtr frame;
	L_INDEX l_index;
	LINE line;
	PBOX box;

/* Get the word index of this character index. */

	if ((paragraph = (ParagraphPtr)database->get_record(p_record, &error, RECORD_TYPE_Paragraph)) == NULL)
	{
		return error;
	}

	w_index = paragraph->word_of_character(c_index, &word);
	last_offset = paragraph->number_of_characters();
	paragraph->release();

/* Get the x offset. */

	box.x0 = x_offset(&word);

/*
// Now get the frame so we can finalize the cursor box.
*/

	if ((frame = (FramePtr)database->get_record(f_record, &error, RECORD_TYPE_Frame)) == NULL)
	{
		return error;
	}

/*
// Get the line index corresponding to this word.
*/

	l_index = frame->line_of_word(w_index, &line);

/*
// Initialize the flags.
*/

	flags = 0;

/*
// Now we can set the cursor box.
*/

	PBOX bound = frame->get_bound();

	box.y0 = line.baseline - line.ascend;
	box.y1 = line.baseline + line.descend;
	box.x1 = box.x0
//					+ scale_pcoord(1, PAGE_RESOLUTION, redisplay_x_resolution);
					+ scale_pcoord(1, PAGE_RESOLUTION, 96);		// Cheat!

	FLAGS object_flags = object->get_flags();

	if (object_flags & OBJECT_FLAG_xflipped)
	{
		cursor_box.x0 = bound.x1 - box.x1;
		cursor_box.x1 = bound.x1 - box.x0;
	}
	else
	{
		cursor_box.x0 = bound.x0 + box.x0;
		cursor_box.x1 = bound.x0 + box.x1;
	}

	if (object_flags & OBJECT_FLAG_yflipped)
	{
		cursor_box.y0 = bound.y1 - box.y1;
		cursor_box.y1 = bound.y1 - box.y0;
	}
	else
	{
		cursor_box.y0 = bound.y0 + box.y0;
		cursor_box.y1 = bound.y0 + box.y1;
	}

	FLAGS frame_flags = frame->get_flags();

	if (frame_flags & FRAME_FLAG_stretch_text)
	{
		flags |= TXP_FLAG_stretch_text;
	}

	if (frame_flags & FRAME_FLAG_stretch_frame)
	{
		flags |= TXP_FLAG_stretch_frame;
	}

	frame->release();

/*
// Update the rest of the flags.
*/

	if (c_index == 0)
	{
		flags |= TXP_FLAG_start;
	}
	if (c_index == last_offset)
	{
		flags |= TXP_FLAG_end;
	}

	return ERRORCODE_None;
}

/*
// Calculate where the txp currently is on the page.
// The cursor box is constructed and placed in the txp for reference.
*/

ERRORCODE Txp::calc_all(VOID)
{
	ERRORCODE error;

/* A bad time to be calling this routine. */

	if (!valid())
	{
		return ERRORCODE_NotInitialized;
	}

/* compute the txp style. */

	if (compatability == TXP_COMPATABILITY_PMG || transition)
	{
		if ((error = calc_style()) != ERRORCODE_None)
		{
			return error;
		}
	}

	return calc_cursor();
}

/*
// Set the 'desired_x' field of the txp to the current x position.
*/

VOID Txp::set_desired_x()
{
	desired_x = x_offset();
}

/*
// Set a new position.
// which_end:
//		-1 is left end
//		 0 is no more selection
//		 1 is right end
//		 2 is unknown
*/

VOID Txp::set_position(C_INDEX c_new, TXP_END which_end, BOOL do_set_desired_x)
{
	C_INDEX	c_current = c_index;

/* Validate the position. */

	if (c_new < 0)
	{
		c_new = 0;
	}
	else
	{
		SHORT c_max = database->last_offset_in_paragraph(p_record);

		if (c_new > c_max)
		{
			c_new = c_max;
		}
	}

/* See what we're doing. */

	if (which_end == TXP_NO_SELECTION)
	{
		off(TXP_TOGGLE_SELECT);
		select_start = select_end = c_new;
	}
	else if (c_current != c_new)
	{
		C_INDEX new_start, new_end;
		C_INDEX	c_start = select_start,
					c_end = select_end;

	/* Handle special end cases. */

		if (c_start == c_end)
		{
		/* No selection yet. */
			which_end = c_new < c_current ? TXP_LEFT_END : TXP_RIGHT_END;
		}
		else if (which_end == TXP_UNKNOWN_END)
		{
			if (c_new <= c_start)
			{
				which_end = TXP_LEFT_END;
			}
			else if (c_new >= c_end)
			{
				which_end = TXP_RIGHT_END;
			}
			else
			{
				which_end = TXP_CURRENT_END;
			}
		}

		if (which_end == TXP_CURRENT_END)
		{
			if (c_current == c_start)
			{
				which_end = TXP_LEFT_END;
			}
			else
			{
				which_end = TXP_RIGHT_END;
			}
		}

	/* Keep things oriented correctly. */

		if (which_end == TXP_LEFT_END && c_new > c_end)
		{
			C_INDEX c_tmp;

			which_end = TXP_RIGHT_END;

			c_tmp = c_start;
			c_start = c_end;
			c_end = c_tmp;
		}

		if (which_end == TXP_RIGHT_END && c_new < c_start)
		{
			C_INDEX c_tmp;

			which_end = TXP_LEFT_END;

			c_tmp = c_start;
			c_start = c_end;
			c_end = c_tmp;
		}

	/* Now move the appropriate end. */

		if (which_end == TXP_LEFT_END)
		{
			new_start = c_new;
			new_end = c_end;

			if (new_start < c_start)
			{
				select_start = new_start;
				select_end = c_start;
			}
			else
			{
				select_end = new_start;
			}
		}
		else
		{
			new_start = c_start;
			new_end = c_new;

			if (new_end > c_end)
			{
				select_start = c_end;
				select_end = new_end;
			}
			else
			{
				select_start = new_end;
			}
		}

	/* Toggle what changed. */

		if (select_start != select_end)
		{
			toggle(TXP_TOGGLE_SELECT);
			visible |= TXP_TOGGLE_SELECT;
		}

	/* Set the new endpoints for the select. */

		select_start = new_start;
		select_end = new_end;
	}

/* Now move the cursor if it needs moving. */

	if (c_current != c_new)
	{
		off(TXP_TOGGLE_CURSOR);
		c_index = c_new;

	/* Set the desired x position if requested. */

		if (do_set_desired_x)
		{
			set_desired_x();
		}
	}
	transition = TRUE;			// Always?
}

/*
// Select all text.
*/

VOID Txp::select_all(VOID)
{
	off(TXP_TOGGLE_ALL);
	select_start = 0;
//	c_index =
 	 select_end = database->last_offset_in_paragraph(p_record);
	transition = TRUE;
	set_desired_x();
}

/*
// Select some text.
*/

void Txp::select_text(C_INDEX c_start, C_INDEX c_end)
{
	if (c_start > c_end)
	{
		C_INDEX c_tmp = c_start;
		c_start = c_end;
		c_end = c_tmp;
	}

	C_INDEX c_min = 0;
	C_INDEX c_max = database->last_offset_in_paragraph(p_record);

	if (c_start < c_min)
	{
		c_start = c_min;
	}

	if (c_end > c_max)
	{
		c_end = c_max;
	}

	off(TXP_TOGGLE_ALL);
	select_start = c_start;
	c_index = select_end = c_end;
	transition = TRUE;
	set_desired_x();
}

/*
// Deselect all text in a txp.
*/

VOID Txp::deselect(VOID)
{
	off(TXP_TOGGLE_SELECT);
	select_start = select_end = c_index;
}

/*
// Insert a character at the current txp.
*/

ERRORCODE Txp::insert_text(CHARACTER_PTR s, SHORT count, BOOL single)
{
	ERRORCODE error;
	ParagraphPtr paragraph;
	TEXT_WORD word;
	LINE line;
	W_INDEX w_index;
	L_INDEX l_index;
	C_INDEX c_insert, change_index;
	CHARACTER_DELTA cdelta;
	CHARACTER_RANGE crange;
	WORD_DELTA wdelta;
	WORD_RANGE wrange;
	FramePtr frame;
	PCOORD next_top, frame_height;
	SHORT new_size = -1, i;
	FLAGS frame_flags;
	LINE_PTR lp;
	SHORT skipped = 0;

/* Make sure we have an insertion point. */

	if (!valid())
	{
		return ERRORCODE_NotInitialized;
	}

/* Make sure we're all deselected. */

	if (select_start != select_end)
	{
		if (compatability == TXP_COMPATABILITY_Windows)
		{
			delete_text(1);
		}
		else
		{
			deselect();
		}
	}

/* Get the frame to check our flags. */

	if ((frame = (FramePtr)database->get_record(f_record, &error, RECORD_TYPE_Frame)) == NULL)
	{
		return error;
	}

	frame_flags = frame->get_flags();
	PBOX frame_bound = frame->get_bound();
	frame_height = frame_bound.y1 - frame_bound.y0;
/* End of bottom line. */
	lp = frame->get_line(frame->number_of_lines() - 1);
	next_top = lp->baseline + lp->descend;
/* Top of top line. */
	lp = frame->get_line(0);
	next_top -= lp->baseline - lp->ascend;

/* Get the paragraph so we can insert. */

	if ((paragraph = (ParagraphPtr)database->get_record(p_record, &error, RECORD_TYPE_Paragraph)) == NULL)
	{
		frame->release();
		return error;
	}

/* Validate the insertion position. */

	if (c_index > paragraph->number_of_characters())
	{
		od("txp_insert: invalid position\r\n");
		frame->release();
		paragraph->release();
		return ERRORCODE_BadParameter;
	}

/* If this is not a PMG compatability, pickup the style here. */

	TextStyle start_style(NULL);

	if (compatability != TXP_COMPATABILITY_PMG)
	{
		StyleIterator si(paragraph);

		si.compute(c_index);
		start_style = si.style;
	}

	ERRORCODE insert_error = ERRORCODE_None;

/* Insert all the text! */

	for (i = 0, c_insert = c_index; i < count; i++)
	{
		CHARACTER c;
		c = *s;
		if (!single)
		{
			s++;
		}

		/* Don't allow ENTER as first character in banners */

		if (c == BREAK_CHARACTER
		 	&& (frame_flags & FRAME_FLAG_no_initial_break)
		 	&& c_insert == 0)
		{
			++skipped;
			continue;
		}

	/*
	// If this is the last character, this is a fit_text frame, and we're
	// inserting a BREAK character, do our special fit.
	*/

		if (c_insert == paragraph->number_of_characters()
	 				&& (frame_flags & FRAME_FLAG_fit_text)
					&& c == BREAK_CHARACTER)
		{
			PCOORD new_bottom;

			if (new_size != -1)
			{
			/* Been here already! */
				break;
			}

		/*
	 	// We need to fit this text to the frame.
	 	*/

		/* The txp style should be the current. Use its ascend and descend.*/

			new_bottom = next_top + style.get_font_ascend() + style.get_font_descend();
			if (new_bottom > frame_height)
			{
			/*
	 		// This line is too tall.
	 		// Try to generate a smaller point size to make it fit.
			*/
				new_size = (SHORT)scale_pcoord(style.get_size(),
											frame_height-next_top,
											new_bottom-next_top)
									- 4;		/* A fudge factor! */

				change_index = c_insert;
				if (new_size < MINIMUM_SHRINK_SIZE)
				{
					new_size = -1;
//					warning_beep();
					break;
				}
			}
		}

	/* Do the insertion. */
		ERRORCODE this_error;

		if ((this_error = paragraph->insert_character(c_insert++, c)) != ERRORCODE_None)
		{
			if (this_error == ERRORCODE_Full)
			{
			/* Report the full state. */
				insert_error = this_error;
			}
			break;
		}
	}

	if ((count = i - skipped) == 0)
	{
		frame->release();
		paragraph->release(TRUE);
		return insert_error;
	}

/* Update the word offsets. */

	cdelta.c_start = c_index;
	cdelta.count = count;

	TextFlow textflow(database, object);

//	printf("insertion CDELTA (%d, %d)\n", cdelta);
	paragraph->adjust_style(cdelta);
	textflow.adjust_words(p_record, f_record, cdelta, &wdelta);
//	printf("insertion WDELTA (%d, %d)\n", wdelta);
	textflow.adjust_lines(f_record, wdelta);

/* Reform words. */

	crange.c_start = c_index;
	crange.c_end = c_index + count-1;

	textflow.rebuild_words(p_record, f_record, crange, &wdelta, &wrange);
//	printf("rebuild WDELTA (%d, %d)\n", wdelta);
	textflow.adjust_lines(f_record, wdelta);

/* Get the word index for this character. */

	w_index = paragraph->word_of_character(c_index, &word);
	l_index = frame->line_of_word(w_index, &line);

/* If we need to change the size, do it now. */

	if (new_size != -1)
	{
		STYLE_CHANGE change;
		STYLE_FRAC frac;

		change.c_index = change_index;
 		change.token = TOKEN_Size;

	/* Code borrowed from style.c */

		frac.num = new_size;
		frac.denom = style.get_size();
		change.new_value = *(LONG *)&frac;

	/* Insert the change. */

		paragraph->insert_change(paragraph->number_of_changes(), &change);
	}

	if (compatability != TXP_COMPATABILITY_PMG)
	{
	/* See if we need to adjust the range to fit the insertion style. */
		if (start_style != style)
		{
			paragraph->change_range_style(crange, &style, FALSE);
		}
	}

/* Size the words. */

	paragraph->size_words(wrange, FALSE);

/*
// If the inserted character is NOT solid and the word being inserted into
// IS solid and the word is the first word on its line, then this word will
// be broken if it is not the first character of the word, and the word may
// move up to the previous line if there is room. If it IS the first
// character of the word, the inserted character itself may move up to the
// previous line. In either event, we need to start flowing from the previous
// line to insure text flow integrity.
*/

#if 0
	if (character_type(c) != WORD_TYPE_solid
				&& (word.type == WORD_TYPE_solid || word.type == WORD_TYPE_tab)
				&& line.w_start == w_index)
#endif
	{
	/* If not the first line, back up to the previous line. */

		if (l_index != 0)
		{
		/* Get the image of the previous line. */

			frame->get_line_image(--l_index, &line);
		}
	}

/* Start at the beginning of the line if before our word. */

	if (line.w_start < wrange.w_start)
	{
		wrange.w_start = line.w_start;
	}

	frame->release(TRUE);
	paragraph->release(TRUE);

/* Rebuild the text lines. This completes the text flow. */

	textflow.rebuild_lines(object, wrange);

/* Move to the next offset. */

	set_position(c_index+count, TXP_NO_SELECTION, TRUE);

	return insert_error;
}

/*
// Delete a character at the current txp.
*/

ERRORCODE Txp::delete_text(SHORT count)
{
	ParagraphPtr paragraph;
	ERRORCODE error;
	CHARACTER_DELTA cdelta;
	WORD_DELTA wdelta;
	WORD_RANGE wrange;
	C_INDEX c_start, c_end;

/* Make sure we have an insertion point. */

	if (!valid())
	{
		return ERRORCODE_NotInitialized;
	}

/* Set the range to delete. */

	if (select_start == select_end)
	{
		off(TXP_TOGGLE_CURSOR);
		c_end = (c_start = c_index) + count;
	}
	else
	{
		c_start = select_start;
		c_end = select_end + count-1;

	/* Cursor will end up at the start of this range. */

		set_position(select_start, TXP_NO_SELECTION, FALSE);
	}

/* Get the paragraph so we can see if we need to delete. */

	if ((paragraph = (ParagraphPtr)database->get_record(p_record, &error, RECORD_TYPE_Paragraph)) == NULL)
	{
		return error;
	}

/* Validate the insertion position. */

	SHORT text_count = paragraph->number_of_characters();

	if (c_start != text_count)
	{
		C_INDEX c_index;
		CHARACTER_RANGE crange;

	 	if (c_end > text_count)
		{
			c_end = text_count-1;
		}

		if (c_end < c_start)
		{
			od("txp_delete: invalid position\r\n");
			paragraph->release();
			return ERRORCODE_BadParameter;
		}

	/* We're modifying this object. */

		paragraph->modified();

	/*
	// Do the deletion.
	// Remember whether we deleted a macro or not.
	*/

		BOOL fDeletedMacro = FALSE;			// Initialize

		for (c_index = c_end; --c_index >= c_start; )
		{
			paragraph->delete_character(c_index, &fDeletedMacro);
		}

	/* Update the macro flag if we deleted a macro. */

		if (fDeletedMacro)
		{
			paragraph->UpdateMacroState();
		}

	/* Update the word offsets. */

		cdelta.c_start = c_start;
		cdelta.count = c_start - c_end;

		TextFlow textflow(database, object);

//		printf("deletion CDELTA (%d, %d)\n", cdelta);
		paragraph->adjust_style(cdelta);
		textflow.adjust_words(p_record, f_record, cdelta, &wdelta);
//		printf("deletion WDELTA (%d, %d) [%Fp]\n", wdelta, &wdelta);
		textflow.adjust_lines(f_record, wdelta);

	/* Reform words. */

		crange.c_start =
			crange.c_end = c_start;

		textflow.rebuild_words(p_record, f_record, crange, &wdelta, &wrange);
//		printf("rebuild WDELTA (%d, %d)\n", wdelta);
		textflow.adjust_lines(f_record, wdelta);

	/* Size the words. */

		paragraph->size_words(wrange, FALSE);

	/* Rebuild the text lines. This completes the text flow. */

		textflow.rebuild_lines(object, wrange);

		set_desired_x();
	}

	transition = TRUE;

	paragraph->release();

	return ERRORCODE_None;
}

/*
// Handle a backspace request.
*/

ERRORCODE Txp::backspace(SHORT count)
{
	if (something_selected())
	{
		return delete_text(count);
	}

	if (count > c_index)
	{
		count = c_index;
	}

	if (count != 0)
	{
		set_position(c_index-count, TXP_NO_SELECTION, FALSE);
		return delete_text(count);
	}
	return ERRORCODE_None;
}

/*
// Set the txp c_index via a point location on the page.
*/

ERRORCODE Txp::move_to_point(PPNT p, TXP_END which_end, BOOL do_set_desired_x)
{
	ERRORCODE error;
	FramePtr frame;
	L_INDEX l_index;
	LINE_PTR lp;
	W_INDEX w_start, w_end;
	ParagraphPtr paragraph;
	W_INDEX w_index;
	TEXT_WORD_PTR wp;
	C_INDEX c_index, c_end;
	CHARACTER_PTR cp;
	PCOORD x_offset;

/*
// Get the frame so we can examine the lines.
*/

	if ((frame = (FramePtr)database->get_record(f_record, &error, RECORD_TYPE_Frame)) == NULL)
	{
		return error;
	}

	PBOX frame_bound = frame->get_bound();
	FLAGS object_flags = object->get_flags();

	if (object_flags & OBJECT_FLAG_xflipped)
	{
		p.x = frame_bound.x1 - p.x;
	}
	else
	{
		p.x -= frame_bound.x0;
	}
	if (object_flags & OBJECT_FLAG_yflipped)
	{
		p.y = frame_bound.y1 - p.y;
	}
	else
	{
		p.y -= frame_bound.y0;
	}

/* Find the line with this position. */

	SHORT line_count = frame->number_of_lines();

	for (l_index = 0, lp = frame->get_line(0);
									l_index < line_count-1;
									l_index++, lp++)
	{
		if (p.y < lp->baseline + lp->descend)
		{
			break;
		}
	}

/* Found the line. Get the word extent. */

	w_start = lp->w_start;
	w_end = lp->w_end;

	frame->release();

/*
// Now get the paragraph so we can check the words.
*/

	if ((paragraph = (ParagraphPtr)database->get_record(p_record, &error, RECORD_TYPE_Paragraph)) == NULL)
	{
		return error;
	}

/* Now look through the words until we find the word the point is in. */

	for (w_index = w_start, wp = paragraph->get_word(w_index);
										w_index < w_end;
										w_index++, wp++)
	{
		if (p.x < wp->x_offset + wp->width)
		{
			break;
		}
	}

/* Now we have the word. Find the character. */

	if (wp->type == WORD_TYPE_eop)
	{
		c_end = wp->c_start;
	}
	else
	{
		c_end = (wp+1)->c_start;
		if (w_index == w_end)
		{
		/* We must stay on this line. */
			c_end--;
		}
	}


/* Initialize our style. */

	StyleIterator si(paragraph);
	FontServerPtr font_server = database->get_font_server();

	for (x_offset = wp->x_offset, c_index = wp->c_start, cp = paragraph->get_character(c_index);
 							c_index < c_end;
							c_index++, cp++)
	{
		PCOORD width;

		si.compute(c_index);
		width = font_server->ch_delta_x(*cp, &si.style);

		if (p.x < x_offset + width/2)
		{
			break;
		}
		x_offset += width;
	}

	paragraph->release();

/* Set the new position. */

	set_position(c_index, which_end, do_set_desired_x);

	return ERRORCODE_None;
}

/*
// Change lines.
*/

ERRORCODE Txp::change_line(SHORT l_delta, BOOL shifted)
{
	ERRORCODE error;
	W_INDEX w_index;
	L_INDEX l_org_index, l_index;
	FramePtr frame;
	PPNT p;
	ParagraphPtr paragraph;

/* Get the line index for this position. */

	if ((paragraph = (ParagraphPtr)database->get_record(p_record, &error, RECORD_TYPE_Paragraph)) == NULL)
	{
		return error;
	}
	w_index = paragraph->word_of_character(c_index, NULL);
	paragraph->release();

/* Get the frame so we can continue. */

	if ((frame = (FramePtr)database->get_record(f_record, &error, RECORD_TYPE_Frame)) == NULL)
	{
		return error;
	}

	l_org_index = frame->line_of_word(w_index, NULL);

/* Update the index. */

	l_index = l_org_index + l_delta;

	if (l_index < 0)
	{
		l_index = 0;
	}
	else if (l_index >= frame->number_of_lines())
	{
		l_index = frame->number_of_lines()-1;
	}

/* Grab the baseline in case we need to move to there. */

	PBOX bound = frame->get_bound();
	FLAGS object_flags = object->get_flags();

	if (object_flags & OBJECT_FLAG_xflipped)
	{
		p.x = frame->get_bound().x1 - desired_x;
	}
	else
	{
		p.x = frame->get_bound().x0 + desired_x;
	}

	/* MH 5/21/93: do -1 because of case of font with descent=0 (TWIG) */
	p.y = frame->get_line(l_index)->baseline - 1;

	if (object_flags & OBJECT_FLAG_yflipped)
	{
		p.y = frame->get_bound().y1 - p.y;
	}
	else
	{
		p.y += frame->get_bound().y0;
	}

/* Release our friend the frame. */

	frame->release();

/* See if the lines are different. */

	if (l_index != l_org_index)
	{
	/* We've changed lines. Move to the new line now. */
		return move_to_point(p,
									shifted ? TXP_CURRENT_END : TXP_NO_SELECTION,
 									FALSE);
	}

	return ERRORCODE_None;
}

/*
// Move the insertion point left if possible.
*/

VOID Txp::left(BOOL shifted)
{
	set_position(c_index-1,
							shifted ? TXP_CURRENT_END : TXP_NO_SELECTION,
							TRUE);
}

/*
// Move the insertion point right if possible.
*/

VOID Txp::right(BOOL shifted)
{
	set_position(c_index+1,
							shifted ? TXP_CURRENT_END : TXP_NO_SELECTION,
							TRUE);
}

/*
// Move the insertion point left a solid word if possible.
// The insertion point ends up on the beginning of the word.
*/

VOID Txp::left_word(BOOL shifted)
{
	W_INDEX w_index;
	TEXT_WORD word;
	ParagraphPtr paragraph;

/* Grab the paragraph. */

	if ((paragraph = (ParagraphPtr)database->get_record(p_record, NULL, RECORD_TYPE_Paragraph)) == NULL)
	{
		return;
	}

/* Grab the current word. */

	w_index = paragraph->word_of_character(c_index, &word);

	if ((word.type != WORD_TYPE_solid && word.type != WORD_TYPE_macro)
			|| word.c_start == c_index)
	{
	/*
 	// Need to go left until we find a solid word.
	// If no solid word is found, the cursor is not moved (but any selected
	// text is deselected if not shifted).
	*/
		while (w_index-- != 0)
		{
		/* Grab the next word image. */
			if (paragraph->get_word_image(w_index, &word) == -1)
			{
				paragraph->release();
				return;
			}
		/* Is it solid? */
			if (word.type == WORD_TYPE_solid || word.type == WORD_TYPE_macro)
			{
				break;
			}
		}
	}

/* Release the paragraph. */

	paragraph->release();

/* Move to where we want to go. */

	set_position(word.c_start,
							shifted ? TXP_CURRENT_END : TXP_NO_SELECTION,
							TRUE);
}

/*
// Move the insertion point right a solid word if possible.
// The insertion point ends up on the end of the word.
*/

VOID Txp::right_word(BOOL shifted)
{
	W_INDEX w_index;
	TEXT_WORD word;
	BOOL hit_solid = FALSE;
	BOOL first_word = TRUE;
	ParagraphPtr paragraph;

/* Grab the paragraph. */

	if ((paragraph = (ParagraphPtr)database->get_record(p_record, NULL, RECORD_TYPE_Paragraph)) == NULL)
	{
		return;
	}

/* Grab the current word. */

	w_index = paragraph->word_of_character(c_index, NULL);

/*
// Need to go right until we find a solid word. Any solid word we are
// currently on does not count.
// If no solid word is found, the cursor is not moved (but any selected
// text is deselected if not shifted).
*/

	do
	{
	/* Grab the next word. */

		if (paragraph->get_word_image(w_index++, &word) == -1)
		{
			paragraph->release();
			return;
		}

	/* Update our solid status. */

		if (!first_word
			 && (word.type == WORD_TYPE_solid || word.type == WORD_TYPE_macro))
		{
			hit_solid = TRUE;
		}

		first_word = FALSE;

	} while (word.type != WORD_TYPE_eop	&& !hit_solid);

/* Release the paragraph. */

	paragraph->release();

/* Move to where we want to go. */

	set_position(word.c_start,
							shifted ? TXP_CURRENT_END : TXP_NO_SELECTION,
							TRUE);
}

/*
// Move the insertion point up a line if possible.
*/

VOID Txp::up(BOOL shifted)
{
	change_line(-1, shifted);
}

/*
// Move the insertion point down a line if possible.
*/

VOID Txp::down(BOOL shifted)
{
	change_line(1, shifted);
}

/*
// Return the first character index for this line.
*/

C_INDEX Txp::first_c_in_line(VOID)
{
	W_INDEX w_index;
	LINE line;
	TEXT_WORD word;
	ParagraphPtr paragraph;
	FramePtr frame;

	word.c_start = -1;

	if ((paragraph = (ParagraphPtr)database->get_record(p_record, NULL, RECORD_TYPE_Paragraph)) != NULL)
	{
		if ((frame = (FramePtr)database->get_record(f_record, NULL, RECORD_TYPE_Frame)) != NULL)
		{
			w_index = paragraph->word_of_character(c_index, NULL);
			frame->line_of_word(w_index, &line);
			paragraph->get_word_image(line.w_start, &word);

			frame->release();
		}
		paragraph->release();
	}

	return word.c_start;
}

/*
// Move the insertion point to beginning of line.
*/

VOID Txp::bol(BOOL shifted)
{
	C_INDEX c_new;

/* Move to the first character in this word. */

	if ((c_new = first_c_in_line()) != -1)
	{
		set_position(c_new,
								shifted ? TXP_CURRENT_END : TXP_NO_SELECTION,
								TRUE);
	}
}

C_INDEX Txp::last_c_in_line(VOID)
{
	W_INDEX w_index;
	LINE line;
	TEXT_WORD word;
	C_INDEX c_new = -1;
	ParagraphPtr paragraph;
	FramePtr frame;

	word.c_start = -1;

	if ((paragraph = (ParagraphPtr)database->get_record(p_record, NULL, RECORD_TYPE_Paragraph)) != NULL)
	{
		if ((frame = (FramePtr)database->get_record(f_record, NULL, RECORD_TYPE_Frame)) != NULL)
		{
			w_index = paragraph->word_of_character(c_index, NULL);
			frame->line_of_word(w_index, &line);
			paragraph->get_word_image(line.w_end, &word);

		/* Move to the first character in this word. */

			if (word.type == WORD_TYPE_eop)
			{
			/* No choice. */
				c_new = word.c_start;
			}
			else
			{
				paragraph->get_word_image(line.w_end+1, &word);
				c_new = word.c_start-1;
			}

			frame->release();
		}
		paragraph->release();
	}

	return c_new;
}

/*
// Move the insertion point to end of line.
*/

VOID Txp::eol(BOOL shifted)
{
	C_INDEX c_new;

	if ((c_new = last_c_in_line()) != -1)
	{
		set_position(c_new,
								shifted ? TXP_CURRENT_END : TXP_NO_SELECTION,
								TRUE);
	}
}

/*
// Move the insertion point to beginning of paragraph.
*/

VOID Txp::bop(BOOL shifted)
{
	set_position(0, shifted ? TXP_CURRENT_END : TXP_NO_SELECTION, TRUE);
}

/*
// Move the insertion point to end of paragraph.
*/

VOID Txp::eop(BOOL shifted)
{
	set_position(32767, shifted ? TXP_CURRENT_END : TXP_NO_SELECTION, TRUE);
}

/*
// Get the character range corresponding to this txp.
*/

VOID Txp::get_crange(CHARACTER_RANGE_PTR crange)
{
	C_INDEX last = database->last_offset_in_paragraph(p_record);
	if (select_start == select_end)
	{
		crange->c_start = 0;
		crange->c_end = last;
	}
	else
	{
		crange->c_start = select_start;
		crange->c_end = select_end;
	}

//	od("get_crange(%d, %d-1) [0, %d]\r\n", *crange,
//		database->last_offset_in_paragraph(p_record));

/* We have a selection. */

	if (crange->c_end != last)
	{
		crange->c_end--;
	}
}

ERRORCODE Txp::update_range(CHARACTER_RANGE* pRange /*=NULL*/)
{
	CHARACTER_RANGE crange;
	WORD_RANGE wrange;
	ParagraphPtr paragraph;
	ERRORCODE error;

	if (pRange == NULL)
	{
		get_crange(&crange);
	}
	else
	{
		crange = *pRange;
	}

	if ((paragraph = (ParagraphPtr)database->get_record(p_record, &error, RECORD_TYPE_Paragraph)) != NULL)
	{
		paragraph->crange_to_wrange(crange, &wrange);
//		od("word range (%d, %d) [0, %d]\r\n", wrange, paragraph->number_of_words());
		paragraph->size_words(wrange, TRUE);
		paragraph->release(TRUE);

	/* Update the frame. */

		TextFlow textflow(database, object);

		textflow.rebuild_lines(object, wrange);
		textflow.refresh_frame(f_record, crange);
	}
	return error;
}
#endif
